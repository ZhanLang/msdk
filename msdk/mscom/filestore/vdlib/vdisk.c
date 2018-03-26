

//#include <stdlib.h>

#include <ntddk.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntverp.h>

#include "nthdr.h"

#include "vdisk.h"
#include "fops.h"
#include "dyndisk.h"

#define RSVD_TAG	'rsvd'


void *__cdecl malloc( size_t size )
{
	return ExAllocatePoolWithTag( PagedPool, size, RSVD_TAG );
}


void __cdecl free(void *ptr)
{
	if (ptr != NULL)
		ExFreePool( ptr );
}

struct user_ctx {
	LIST_ENTRY user_lst;
	ULONG userid;
	ULONG driver_map;	/* 驱动器位图，只用 26位 */
};

struct user_data_ctx {
	LIST_ENTRY user_data_lst;
	ULONG userid;
	ULONG user_data_len;
	UCHAR user_data[1];
};

struct vdo_ctx {
	LIST_ENTRY vdo_lst;
	PDEVICE_OBJECT vdo_devobj;
};

struct base_devext {
	PSECURITY_CLIENT_CONTEXT security_client_context;
	LIST_ENTRY list_head;	/* IRP 链表 */
	KSPIN_LOCK list_lock;
	KEVENT request_event;	/* 用于线程工作的通知 */
	void *thread_pointer;
	BOOLEAN terminate_thread;
};

struct cdisk_devext {
	struct base_devext	hdr;
	LIST_ENTRY vdo_lst_hdr;
	KSPIN_LOCK vdo_lst_lock;
	LIST_ENTRY user_data_lst_hdr;	/* 用户数据链表 */
	KSPIN_LOCK user_data_lst_lock;
};

struct vdisk_devext {
	struct base_devext	hdr;
	LIST_ENTRY user_lst_hdr;	/* 用于记录 用户ID 及对应的 驱动器的链表 */
	KSPIN_LOCK user_lst_lock;
	ANSI_STRING file_name;
	UNICODE_STRING ui_file_name;	/* 唯一标识 */
	LARGE_INTEGER file_size;
	LARGE_INTEGER local_time;
	BOOLEAN read_only;
	AES_KEY encrypt_key;
	AES_KEY decrypt_key;
	struct dyndisk_state dd_stat;
	struct fixdisk_state sd_stat;
	int diskfile_type;
};



PDEVICE_OBJECT g_cdodevobj = NULL;



NTSTATUS
vdisk_get_obj_name( void *obj, PUNICODE_STRING name )
{
	NTSTATUS rc;
	POBJECT_NAME_INFORMATION  obj_name_info = NULL;
	WCHAR buf[64] = L"\0";
	ULONG length = 0;

	obj_name_info = (POBJECT_NAME_INFORMATION)buf;

	do {

		rc = ObQueryNameString( obj, obj_name_info, 64 * sizeof(WCHAR), &length );
		if (rc == STATUS_INFO_LENGTH_MISMATCH || rc == STATUS_BUFFER_OVERFLOW) {
			obj_name_info = malloc( length );
			if (obj_name_info == NULL)
				return STATUS_INSUFFICIENT_RESOURCES;
			RtlZeroMemory( obj_name_info, length );
			rc = ObQueryNameString( obj, obj_name_info, length, &length );        
		}

		if (!NT_SUCCESS( rc ))
			break;

		RtlCopyUnicodeString( name, &obj_name_info->Name );

	} while(FALSE);

	if ((void *)obj_name_info != (void *)buf)
		free( obj_name_info );

	return rc;
}

BOOLEAN vdisk_make_filename( char * inname, PUNICODE_STRING outname )
{
	NTSTATUS	rc;
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING uni_fn;
	IO_STATUS_BLOCK iosb;
	HANDLE fd;
	PFILE_OBJECT	fileobj;
	ANSI_STRING	ai_filename;

	RtlInitAnsiString( &ai_filename, inname );
	rc = RtlAnsiStringToUnicodeString( &uni_fn, &ai_filename, TRUE );
	if (!NT_SUCCESS( rc ))
		return FALSE;

	InitializeObjectAttributes( &oa, &uni_fn,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0,
		0 );

	rc = IoCreateFile( &fd, GENERIC_READ, &oa, &iosb, NULL, FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
		| FILE_WRITE_THROUGH, NULL, 0, CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING );

	RtlFreeUnicodeString( &uni_fn );

	if (!NT_SUCCESS( rc ))
		return FALSE;

	rc = ObReferenceObjectByHandle( fd, FILE_ALL_ACCESS, *IoFileObjectType, KernelMode, &fileobj, NULL );
	if (!NT_SUCCESS( rc )) {
		ZwClose( fd );
		return FALSE;
	}

	ZwClose( fd );

	rc = vdisk_get_obj_name( fileobj->DeviceObject, outname );
	if (!NT_SUCCESS( rc )) {
		return FALSE;
	}

	if (outname->MaximumLength < outname->Length + fileobj->FileName.Length) {
		return FALSE;
	}

	wcsncat( outname->Buffer, fileobj->FileName.Buffer, fileobj->FileName.Length / sizeof(WCHAR) );
	outname->Length = outname->Length + fileobj->FileName.Length;

	ObDereferenceObject( fileobj );

	DbgPrint( "kvdisk!make_filename %wZ\n", outname );

	return TRUE;
}

PDEVICE_OBJECT vdisk_find_mount_vdo( struct cdisk_devext *cdo_devext, PUNICODE_STRING filename )
{
	PDEVICE_OBJECT found_vdo_dev = NULL;
	PLIST_ENTRY	request = cdo_devext->vdo_lst_hdr.Flink;
	KIRQL  irql;

	KeAcquireSpinLock( &cdo_devext->vdo_lst_lock, &irql );

	while (request != &cdo_devext->vdo_lst_hdr) {

		struct vdo_ctx *vc = CONTAINING_RECORD(request, struct vdo_ctx, vdo_lst );
		struct vdisk_devext *vdo_ext = vc->vdo_devobj->DeviceExtension;

		if ((filename->Length == vdo_ext->ui_file_name.Length) 
			&& (filename->Length == RtlCompareMemory( filename->Buffer, vdo_ext->ui_file_name.Buffer, filename->Length ))) {
			found_vdo_dev = vc->vdo_devobj;
			break;
		}

		request = request->Flink;
	}

	KeReleaseSpinLock( &cdo_devext->vdo_lst_lock, irql );

	return found_vdo_dev;
}

struct user_ctx *vdisk_find_userid( struct vdisk_devext *vdo_ext, ULONG userid )
{
	struct user_ctx *found_user_ctx = NULL;
	PLIST_ENTRY	request = vdo_ext->user_lst_hdr.Flink;
	KIRQL  irql;

	KeAcquireSpinLock( &vdo_ext->user_lst_lock, &irql );

	while (request != &vdo_ext->user_lst_hdr) {

		struct user_ctx *uc = CONTAINING_RECORD(request, struct user_ctx, user_lst );

		if (uc->userid == userid ) {
			found_user_ctx = uc;
			break;
		}

		request = request->Flink;
	}

	KeReleaseSpinLock( &vdo_ext->user_lst_lock, irql );

	return found_user_ctx;
}

struct user_data_ctx *vdisk_find_user_data( struct cdisk_devext *cdo_ext, ULONG userid )
{
	struct user_data_ctx *found_user_ctx = NULL;
	PLIST_ENTRY	request = cdo_ext->user_data_lst_hdr.Flink;
	KIRQL  irql;

	KeAcquireSpinLock( &cdo_ext->user_data_lst_lock, &irql );

	while (request != &cdo_ext->user_data_lst_hdr) {

		struct user_data_ctx *udc = CONTAINING_RECORD(request, struct user_data_ctx, user_data_lst );

		if (udc->userid == userid ) {
			found_user_ctx = udc;
			break;
		}

		request = request->Flink;
	}

	KeReleaseSpinLock( &cdo_ext->user_data_lst_lock, irql );

	return found_user_ctx;
}

NTSTATUS vdisk_get_info( struct cdisk_devext *cdo_ext, struct vdisk_info_ex *vi, size_t innr, size_t *outnr )
{
	PLIST_ENTRY	vdo_request = cdo_ext->vdo_lst_hdr.Flink;
	struct vdisk_info_ex *tmp_vi;
	KIRQL  vdo_irql;
	size_t nr = 0;

	tmp_vi = ExAllocatePoolWithTag( NonPagedPool, sizeof(struct vdisk_info_ex) * innr, RSVD_TAG );
	if (tmp_vi == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	memset( tmp_vi, 0, sizeof(struct vdisk_info_ex) * innr );
	*outnr = 0;

	KeAcquireSpinLock( &cdo_ext->vdo_lst_lock, &vdo_irql );

	while (vdo_request != &cdo_ext->vdo_lst_hdr) {

		KIRQL  user_irql;
		struct vdo_ctx *vc = CONTAINING_RECORD(vdo_request, struct vdo_ctx, vdo_lst );
		struct vdisk_devext *vdo_ext = vc->vdo_devobj->DeviceExtension;
		PLIST_ENTRY	user_request = vdo_ext->user_lst_hdr.Flink;

		KeAcquireSpinLock( &vdo_ext->user_lst_lock, &user_irql );

		while (user_request != &vdo_ext->user_lst_hdr) {

			struct user_ctx *uc = CONTAINING_RECORD(user_request, struct user_ctx, user_lst );
			

			if (nr >= innr) {
				(*outnr)++;
				user_request = user_request->Flink;
				continue;
			}
			memcpy( &tmp_vi[nr].decrypt_key, &vdo_ext->decrypt_key, sizeof( AES_KEY ) );
			memcpy( &tmp_vi[nr].encrypt_key, &vdo_ext->encrypt_key, sizeof( AES_KEY ) );
			memcpy( &tmp_vi[nr].filename, vdo_ext->file_name.Buffer, vdo_ext->file_name.Length );
			tmp_vi[nr].local_time = vdo_ext->local_time.QuadPart;
			tmp_vi[nr].userid = uc->userid;
			tmp_vi[nr].drvmap = uc->driver_map;
			tmp_vi[nr].filesize.QuadPart = vdo_ext->file_size.QuadPart;
			tmp_vi[nr].readonly = vdo_ext->read_only;
			nr++;
			(*outnr)++;

			user_request = user_request->Flink;
		}

		KeReleaseSpinLock( &vdo_ext->user_lst_lock, user_irql );

		vdo_request = vdo_request->Flink;
	}

	KeReleaseSpinLock( &cdo_ext->vdo_lst_lock, vdo_irql );

	memcpy( vi, tmp_vi, sizeof(struct vdisk_info_ex) * innr );

	ExFreePool( tmp_vi );

	return STATUS_SUCCESS;
}

PDEVICE_OBJECT vdisk_find_mount_vdo_byuser( struct cdisk_devext *cdo_devext, uint32_t userid, uint8_t letter, struct user_ctx **puc, struct vdo_ctx **pvc )
{
	PDEVICE_OBJECT found_vdo_dev = NULL;
	PLIST_ENTRY	request = cdo_devext->vdo_lst_hdr.Flink;
	KIRQL  irql;

	KeAcquireSpinLock( &cdo_devext->vdo_lst_lock, &irql );

	while (request != &cdo_devext->vdo_lst_hdr) {

		struct user_ctx *uc = NULL;
		struct vdo_ctx *vc = CONTAINING_RECORD(request, struct vdo_ctx, vdo_lst );
		struct vdisk_devext *vdo_ext = vc->vdo_devobj->DeviceExtension;

		uc = vdisk_find_userid( vdo_ext, userid );
		if ((uc != NULL) && ((uc->driver_map & (1 << letter)) != 0)) {
			found_vdo_dev = vc->vdo_devobj;

			if (puc != NULL)
				*puc = uc;

			if (pvc != NULL)
				*pvc = vc;

			break;

		}

		request = request->Flink;
	}

	KeReleaseSpinLock( &cdo_devext->vdo_lst_lock, irql );

	return found_vdo_dev;
}





NTSTATUS vdisk_create_device( PDEVICE_OBJECT cdo_devobj, PUNICODE_STRING device_name, struct vdisk_info *vdi, DEVICE_TYPE devtype );
NTSTATUS vdisk_delete_device( PDEVICE_OBJECT cdo_devobj, struct vdo_ctx *vc );


NTSTATUS vdisk_complete_request(PIRP irp, NTSTATUS rc)
{
	irp->IoStatus.Status = rc;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return rc;
}


NTSTATUS vdisk_create_close(PDEVICE_OBJECT devobj, PIRP irp)
{
	irp->IoStatus.Information = FILE_OPENED;
	return vdisk_complete_request(irp, STATUS_SUCCESS);
}

NTSTATUS vdisk_read_write(PDEVICE_OBJECT devobj, PIRP irp)
{
	struct vdisk_devext *devext;
	PIO_STACK_LOCATION iosl;

	devext = (struct vdisk_devext *)devobj->DeviceExtension;

	irp->IoStatus.Information = 0;

	iosl = IoGetCurrentIrpStackLocation(irp);
	if (!iosl->Parameters.Read.Length)
		return vdisk_complete_request(irp, STATUS_SUCCESS);

	IoMarkIrpPending(irp);
	ExInterlockedInsertTailList(&devext->hdr.list_head,
				    &irp->Tail.Overlay.ListEntry,
				    &devext->hdr.list_lock);
	KeSetEvent(&devext->hdr.request_event, 0, FALSE);

	return STATUS_PENDING;
}

NTSTATUS vdisk_ioctl(PDEVICE_OBJECT devobj, PIRP irp)
{
	NTSTATUS rc;
	PIO_STACK_LOCATION iosl;
	struct vdisk_devext *devext;
	char chdrvletter[2] = {'A',0};
	int driverletterid = 0;

	devext = (struct vdisk_devext *)devobj->DeviceExtension;
	iosl = IoGetCurrentIrpStackLocation(irp);

	irp->IoStatus.Information = 0;
	if (devobj == g_cdodevobj) {

		rc = STATUS_UNSUCCESSFUL;

		switch (iosl->Parameters.DeviceIoControl.IoControlCode) {

			case IOCTL_VDISK_CREATE_MOUNT_POINT:
				{
					if (iosl->Parameters.DeviceIoControl.InputBufferLength < sizeof(struct vdisk_info)) {
						rc = STATUS_INVALID_PARAMETER;
						break;
					}

					IoMarkIrpPending(irp);

					ExInterlockedInsertTailList(&devext->hdr.list_head,
						&irp->Tail.Overlay.
						ListEntry,
						&devext->hdr.list_lock);
					KeSetEvent(&devext->hdr.request_event, 0, FALSE);

					rc = STATUS_PENDING;
				}
				break;

			case IOCTL_VDISK_DELETE_MOUNT_POINT:
				{
					if (iosl->Parameters.DeviceIoControl.InputBufferLength < sizeof(struct vdisk_info)) {
						rc = STATUS_INVALID_PARAMETER;
						break;
					}
					IoMarkIrpPending(irp);

					ExInterlockedInsertTailList(&devext->hdr.list_head,
						&irp->Tail.Overlay.
						ListEntry,
						&devext->hdr.list_lock);
					KeSetEvent(&devext->hdr.request_event, 0, FALSE);

					rc = STATUS_PENDING;
				}
				break;

			case IOCTL_VDISK_SET_USER_DATA:
				{
					struct cdisk_devext *cdo_devext = (struct cdisk_devext *)devobj->DeviceExtension;
					struct vdisk_user_data *user_data;
					struct user_data_ctx *udc;

					if (iosl->Parameters.DeviceIoControl.InputBufferLength < sizeof(struct vdisk_user_data)) {
						rc = STATUS_BUFFER_TOO_SMALL;
						break;
					}

					user_data = (struct vdisk_user_data *)irp->AssociatedIrp.SystemBuffer;

					if (iosl->Parameters.DeviceIoControl.InputBufferLength < user_data->data_len - 1 + sizeof(struct vdisk_user_data)) {
						rc = STATUS_BUFFER_TOO_SMALL;
						break;
					}

					udc = vdisk_find_user_data( cdo_devext, user_data->userid );
					if (udc != NULL) {
						if (user_data->data_len <= udc->user_data_len) {
							memcpy( &udc->user_data, &user_data->data, user_data->data_len );
							udc->user_data_len = user_data->data_len;
						}

						/* 重新分配 */
						ExInterlockedRemoveHeadList( udc->user_data_lst.Blink, &cdo_devext->user_data_lst_lock );
						ExFreePool( udc ); udc = NULL;

					}

					udc = (struct user_data_ctx *)ExAllocatePoolWithTag( NonPagedPool, sizeof(struct user_data_ctx) + user_data->data_len - 1, RSVD_TAG );
					if (udc == NULL) {
						rc = STATUS_INSUFFICIENT_RESOURCES;
						break;
					}

					udc->userid = user_data->userid;
					udc->user_data_len = user_data->data_len;
					memcpy( &udc->user_data, &user_data->data, user_data->data_len );

					ExInterlockedInsertTailList( &cdo_devext->user_data_lst_hdr, &udc->user_data_lst, &cdo_devext->user_data_lst_lock );

					rc = STATUS_SUCCESS;
				}
				break;

			case IOCTL_VDISK_GET_USER_DATA:
				{
					struct cdisk_devext *cdo_devext = (struct cdisk_devext *)devobj->DeviceExtension;
					struct vdisk_user_data *user_data;
					void *data;
					struct user_data_ctx *udc;

					/* 传入一 userid */
					if (iosl->Parameters.DeviceIoControl.OutputBufferLength <
						sizeof(uint32_t)) {
							rc = STATUS_BUFFER_TOO_SMALL;
							break;
					}

					user_data = (struct vdisk_user_data *)irp->AssociatedIrp.SystemBuffer;
					data = irp->AssociatedIrp.SystemBuffer;

					udc = vdisk_find_user_data( cdo_devext, user_data->userid );
					if (udc == NULL) {

						rc = STATUS_INSUFFICIENT_RESOURCES;
						break;
					}

					if (iosl->Parameters.DeviceIoControl.OutputBufferLength < udc->user_data_len) {
						rc = STATUS_BUFFER_TOO_SMALL;
						break;
					}

					irp->IoStatus.Information = udc->user_data_len;

					memcpy( data, &udc->user_data, udc->user_data_len );

					
					rc = STATUS_SUCCESS;
				}
				break;

			case IOCTL_VDISK_QUERY_INFO:
				{
					size_t innr;
					size_t outnr = 0;
					struct cdisk_devext *cdo_devext = (struct cdisk_devext *)devobj->DeviceExtension;
					struct vdisk_info_ex *vi;

					if (iosl->Parameters.DeviceIoControl.OutputBufferLength <
						sizeof(struct vdisk_info_ex) + sizeof(size_t)) {
							rc = STATUS_BUFFER_TOO_SMALL;
							break;
					}

					innr = (iosl->Parameters.DeviceIoControl.OutputBufferLength - sizeof(size_t)) / sizeof(struct vdisk_info_ex);

					vi = (struct vdisk_info_ex *)((uint8_t *)irp->AssociatedIrp.SystemBuffer + sizeof(size_t));

					rc = vdisk_get_info( cdo_devext, vi, innr, &outnr );
				
					*(size_t *)irp->AssociatedIrp.SystemBuffer = outnr;
					irp->IoStatus.Information = innr * sizeof(struct vdisk_info_ex);
					DbgPrint( "kvdisk!query_info_ret_len:%08x\n", irp->IoStatus.Information );
				}
				break;

			default:
				rc = STATUS_NO_MEDIA_IN_DEVICE;

		}

		if (rc == STATUS_PENDING) {
			return rc;
		} else {
			return vdisk_complete_request( irp, rc );
		}

	}

	/* 其他设备请求 */

	switch (iosl->Parameters.DeviceIoControl.IoControlCode) {

	case IOCTL_VDISK_QUERY_FILE:
		{
			struct vdisk_info *vdi;

			if (iosl->Parameters.DeviceIoControl.
			    OutputBufferLength <
			    sizeof(struct vdisk_info) +
			    devext->file_name.Length - sizeof(UCHAR)) {
				rc = STATUS_BUFFER_TOO_SMALL;
				irp->IoStatus.Information = 0;
				break;
			}

			vdi =
			    (struct vdisk_info *)irp->AssociatedIrp.
			    SystemBuffer;

			vdi->filesize.QuadPart = devext->file_size.QuadPart;
			vdi->readonly = devext->read_only;
			vdi->filename_len = devext->file_name.Length;

			memcpy(vdi->filename, devext->file_name.Buffer,
			       devext->file_name.Length);

			rc = STATUS_SUCCESS;
			irp->IoStatus.Information = sizeof(struct vdisk_info) +
			    vdi->filename_len - sizeof(UCHAR);

			break;
		}



	case IOCTL_DISK_CHECK_VERIFY:
	case IOCTL_STORAGE_CHECK_VERIFY:
	case IOCTL_STORAGE_CHECK_VERIFY2:
		{
			rc = STATUS_SUCCESS;
			irp->IoStatus.Information = 0;
			break;
		}

	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
		{
			ULONGLONG length;
			PDISK_GEOMETRY disk_geometry;

			if (iosl->Parameters.DeviceIoControl.
			    OutputBufferLength < sizeof(DISK_GEOMETRY)) {
				rc = STATUS_BUFFER_TOO_SMALL;
				irp->IoStatus.Information = 0;
				break;
			}

			disk_geometry =
			    (PDISK_GEOMETRY) irp->AssociatedIrp.SystemBuffer;

			if (devext->diskfile_type == RSDISK_TYPE_FIXED) {
				length = devext->file_size.QuadPart - VDISK_SECTOR_SIZE;
			} else {
				length = devext->file_size.QuadPart;
			}

			disk_geometry->Cylinders.QuadPart =
			    length / VDISK_SECTOR_SIZE / 32 / 2;
			disk_geometry->MediaType = FixedMedia;
			disk_geometry->TracksPerCylinder = 2;
			disk_geometry->SectorsPerTrack = 32;
			disk_geometry->BytesPerSector = VDISK_SECTOR_SIZE;

			rc = STATUS_SUCCESS;
			irp->IoStatus.Information = sizeof(DISK_GEOMETRY);

			break;
		}

	case IOCTL_DISK_GET_LENGTH_INFO:
		{
			PGET_LENGTH_INFORMATION get_length_information;

			if (iosl->Parameters.DeviceIoControl.
			    OutputBufferLength <
			    sizeof(GET_LENGTH_INFORMATION)) {
				rc = STATUS_BUFFER_TOO_SMALL;
				irp->IoStatus.Information = 0;
				break;
			}

			get_length_information =
			    (PGET_LENGTH_INFORMATION) irp->AssociatedIrp.
			    SystemBuffer;
			if (devext->diskfile_type == RSDISK_TYPE_FIXED) {
				get_length_information->Length.QuadPart =
				    devext->file_size.QuadPart - VDISK_SECTOR_SIZE;
			} else {
				get_length_information->Length.QuadPart =
				    devext->file_size.QuadPart;
			}

			rc = STATUS_SUCCESS;
			irp->IoStatus.Information =
			    sizeof(GET_LENGTH_INFORMATION);

			break;
		}

	case IOCTL_DISK_GET_PARTITION_INFO:
		{
			ULONGLONG length;
			PPARTITION_INFORMATION partition_information;

			if (iosl->Parameters.DeviceIoControl.
			    OutputBufferLength <
			    sizeof(PARTITION_INFORMATION)) {
				rc = STATUS_BUFFER_TOO_SMALL;
				irp->IoStatus.Information = 0;
				break;
			}

			partition_information =
			    (PPARTITION_INFORMATION) irp->AssociatedIrp.
			    SystemBuffer;

			if (devext->diskfile_type == RSDISK_TYPE_FIXED) {
				length =
				    devext->file_size.QuadPart - VDISK_SECTOR_SIZE;
			} else {
				length = devext->file_size.QuadPart;
			}

			partition_information->StartingOffset.QuadPart = 0;
			partition_information->PartitionLength.QuadPart = length;
			partition_information->HiddenSectors = 1;
			partition_information->PartitionNumber = 0;
			partition_information->PartitionType = 0;
			partition_information->BootIndicator = FALSE;
			partition_information->RecognizedPartition = FALSE;
			partition_information->RewritePartition = FALSE;

			rc = STATUS_SUCCESS;
			irp->IoStatus.Information =
			    sizeof(PARTITION_INFORMATION);

			break;
		}

	case IOCTL_DISK_GET_PARTITION_INFO_EX:
		{
			ULONGLONG length;
			PPARTITION_INFORMATION_EX partition_information_ex;

			if (iosl->Parameters.DeviceIoControl.
			    OutputBufferLength <
			    sizeof(PARTITION_INFORMATION_EX)) {
				rc = STATUS_BUFFER_TOO_SMALL;
				irp->IoStatus.Information = 0;
				break;
			}

			partition_information_ex =
			    (PPARTITION_INFORMATION_EX) irp->AssociatedIrp.
			    SystemBuffer;

			if (devext->diskfile_type == RSDISK_TYPE_FIXED) {
				length =
				    devext->file_size.QuadPart - VDISK_SECTOR_SIZE;
			} else {
				length = devext->file_size.QuadPart;
			}

			partition_information_ex->PartitionStyle =
			    PARTITION_STYLE_MBR;
			partition_information_ex->StartingOffset.QuadPart = 0;
			partition_information_ex->PartitionLength.QuadPart = length;
			partition_information_ex->PartitionNumber = 0;
			partition_information_ex->RewritePartition = FALSE;
			partition_information_ex->Mbr.PartitionType = 0;
			partition_information_ex->Mbr.BootIndicator = FALSE;
			partition_information_ex->Mbr.RecognizedPartition =
			    FALSE;
			partition_information_ex->Mbr.HiddenSectors = 1;

			rc = STATUS_SUCCESS;
			irp->IoStatus.Information =
			    sizeof(PARTITION_INFORMATION_EX);

			break;
		}

	case IOCTL_DISK_IS_WRITABLE:
		{
			if (!devext->read_only)
				rc = STATUS_SUCCESS;
			else
				rc = STATUS_MEDIA_WRITE_PROTECTED;
			irp->IoStatus.Information = 0;
			break;
		}

	case IOCTL_DISK_MEDIA_REMOVAL:
	case IOCTL_STORAGE_MEDIA_REMOVAL:
		{
			rc = STATUS_SUCCESS;
			irp->IoStatus.Information = 0;
			break;
		}

	case IOCTL_DISK_SET_PARTITION_INFO:
		{
			if (devext->read_only) {
				rc = STATUS_MEDIA_WRITE_PROTECTED;
				irp->IoStatus.Information = 0;
				break;
			}

			if (iosl->Parameters.DeviceIoControl.InputBufferLength <
			    sizeof(SET_PARTITION_INFORMATION)) {
				rc = STATUS_INVALID_PARAMETER;
				irp->IoStatus.Information = 0;
				break;
			}

			rc = STATUS_SUCCESS;
			irp->IoStatus.Information = 0;

			break;
		}

	case IOCTL_DISK_VERIFY:
		{
			PVERIFY_INFORMATION verify_info;

			if (iosl->Parameters.DeviceIoControl.InputBufferLength <
			    sizeof(VERIFY_INFORMATION)) {
				rc = STATUS_INVALID_PARAMETER;
				irp->IoStatus.Information = 0;
				break;
			}

			verify_info =
			    (PVERIFY_INFORMATION) irp->AssociatedIrp.
			    SystemBuffer;

			rc = STATUS_SUCCESS;
			irp->IoStatus.Information = verify_info->Length;

			break;
		}

	default:
		rc = STATUS_INVALID_DEVICE_REQUEST;
		irp->IoStatus.Information = 0;
		break;
	}

	if (rc != STATUS_PENDING) {
		irp->IoStatus.Status = rc;
		IoCompleteRequest(irp, IO_NO_INCREMENT);
	}

	return rc;
}

NTSTATUS vdisk_adjust_privilege(ULONG privilege, BOOLEAN enable)
{
	NTSTATUS rc;
	HANDLE token_handle;
	TOKEN_PRIVILEGES token_privileges;

	rc = ZwOpenProcessToken(NtCurrentProcess(), TOKEN_ALL_ACCESS,
				&token_handle);
	if (!NT_SUCCESS(rc))
		return rc;

	token_privileges.PrivilegeCount = 1;
	token_privileges.Privileges[0].Luid = RtlConvertUlongToLuid(privilege);
	token_privileges.Privileges[0].Attributes =
	    enable ? SE_PRIVILEGE_ENABLED : 0;

	rc = NtAdjustPrivilegesToken(token_handle, FALSE, &token_privileges,
				     sizeof(token_privileges), NULL, NULL);
	ZwClose(token_handle);

	return rc;
}

NTSTATUS vdisk_open_diskfile( PDEVICE_OBJECT devobj, struct vdisk_info *vdi )
{
	int rc;
	char *filename;
	struct vdisk_devext *vdo_ext;
	struct rsdisk_header *hdr = 0;
	uint8_t *tmpbuf;
	void *fd;

	int aligned_hdrsize = sizeof(struct rsdisk_header);
	aligned_hdrsize = (aligned_hdrsize + CRYPT_BLOCK_SIZE - 1) & ~(CRYPT_BLOCK_SIZE - 1);

	vdo_ext = (struct vdisk_devext *)devobj->DeviceExtension;
	vdo_ext->read_only = vdi->readonly;

	memcpy(&vdo_ext->encrypt_key, &vdi->encrypt_key, sizeof(AES_KEY));
	memcpy(&vdo_ext->decrypt_key, &vdi->decrypt_key, sizeof(AES_KEY));

	vdo_ext->file_name.Length = vdi->filename_len;
	vdo_ext->file_name.MaximumLength = vdi->filename_len;
	vdo_ext->file_name.Buffer = ExAllocatePoolWithTag( NonPagedPool, vdi->filename_len, RSVD_TAG );
	if (!vdo_ext->file_name.Buffer) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	memcpy(vdo_ext->file_name.Buffer, vdi->filename, vdi->filename_len);

	filename = malloc( vdi->filename_len + 1 );
	if (!filename) {
		ExFreePool( vdo_ext->file_name.Buffer );
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	memcpy(filename, vdi->filename, vdi->filename_len);
	filename[vdi->filename_len] = '\0';

	vdo_ext->ui_file_name.Length = 0;
	vdo_ext->ui_file_name.MaximumLength = MAXIMUM_FILENAME_LENGTH * sizeof(WCHAR);
	vdo_ext->ui_file_name.Buffer = ExAllocatePoolWithTag( NonPagedPool, vdo_ext->ui_file_name.MaximumLength, RSVD_TAG );
	if (!vdo_ext->ui_file_name.Buffer) {
		ExFreePool( vdo_ext->file_name.Buffer );
		free( filename );
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	if (!vdisk_make_filename( vdi->filename, &vdo_ext->ui_file_name )) {
		ExFreePool( vdo_ext->file_name.Buffer );
		free( filename );
		ExFreePool( vdo_ext->ui_file_name.Buffer );
		return STATUS_INVALID_PARAMETER;
	}

	/* rs disk header */
	if (!vdo_ext->read_only) {
		fd = vdisk_file_open(filename, O_RDWR);	/* 读写方式用户传递决定 */
	} else {
		fd = vdisk_file_open(filename, O_RDONLY);
	}

	free( filename ); filename = NULL;

	if (!fd) {
		ExFreePool(vdo_ext->file_name.Buffer);
		return STATUS_UNSUCCESSFUL;
	}

	tmpbuf = malloc(aligned_hdrsize);
	if (!tmpbuf) {
		ExFreePool(vdo_ext->file_name.Buffer);
		vdisk_file_close(fd);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	memset(tmpbuf, 0, aligned_hdrsize);
	hdr = (struct rsdisk_header *)tmpbuf;

	if (vdisk_file_read(fd, tmpbuf, aligned_hdrsize) != aligned_hdrsize) {
		ExFreePool(vdo_ext->file_name.Buffer);
		free(tmpbuf);
		vdisk_file_close(fd);
		return STATUS_UNSUCCESSFUL;
	}

	if (hdr->magic != RSDISK_MAGIC || hdr->version != RSDISK_VERSION) {
		ExFreePool(vdo_ext->file_name.Buffer);
		free(tmpbuf);
		vdisk_file_close(fd);
		return STATUS_UNSUCCESSFUL;
	}

	if (hdr->cryptmagic != hdr->magic) {
		encrypt_aes_blocks(tmpbuf + 16, tmpbuf + 16, aligned_hdrsize - 16, 0,
			&vdo_ext->decrypt_key);
	}
	if (hdr->cryptmagic != hdr->magic) {
		ExFreePool(vdo_ext->file_name.Buffer);
		free(tmpbuf);
		vdisk_file_close(fd);
		return STATUS_UNSUCCESSFUL;
	}

	vdo_ext->diskfile_type = hdr->disk_type;
	if (vdo_ext->diskfile_type == RSDISK_TYPE_FIXED) {
		vdo_ext->sd_stat.fd = fd;
		vdo_ext->sd_stat.crypt_method = hdr->crypt_method;
		vdo_ext->sd_stat.crypt_method_header = vdo_ext->sd_stat.crypt_method;
		memcpy(&vdo_ext->sd_stat.aes_encrypt_key, &vdo_ext->encrypt_key, sizeof(AES_KEY));
		memcpy(&vdo_ext->sd_stat.aes_decrypt_key, &vdo_ext->decrypt_key, sizeof(AES_KEY));
		vdo_ext->file_size.QuadPart = vdisk_file_lseek(fd, 0, 2);
	} else if (vdo_ext->diskfile_type == RSDISK_TYPE_DYNAMIC) {
		vdo_ext->dd_stat.fd = fd;
		memcpy(&vdo_ext->dd_stat.aes_encrypt_key, &vdo_ext->encrypt_key, sizeof(AES_KEY));
		memcpy(&vdo_ext->dd_stat.aes_decrypt_key, &vdo_ext->decrypt_key, sizeof(AES_KEY));
		rc = dyndisk_open(&vdo_ext->dd_stat, hdr);
		if (rc < 0) {
			ExFreePool(vdo_ext->file_name.Buffer);
			free(tmpbuf);
			vdisk_file_close(fd);
			return STATUS_UNSUCCESSFUL;
		}
		vdo_ext->file_size.QuadPart =
			vdo_ext->dd_stat.total_sectors * VDISK_SECTOR_SIZE;
	} else {
		ExFreePool(vdo_ext->file_name.Buffer);
		free(tmpbuf);
		vdisk_file_close(fd);
		return STATUS_UNSUCCESSFUL;
	}

	free(tmpbuf);

	if (vdo_ext->read_only)
		devobj->Characteristics |= FILE_READ_ONLY_DEVICE;
	else
		devobj->Characteristics &= ~FILE_READ_ONLY_DEVICE;
	devobj->AlignmentRequirement = 0;

	return STATUS_SUCCESS;
}

NTSTATUS vdisk_close_diskfile( PDEVICE_OBJECT devobj )
{
	struct vdisk_devext *devext;

	ASSERT(devobj);

	devext = (struct vdisk_devext *)devobj->DeviceExtension;

	if (devext->file_name.Buffer)
		ExFreePool(devext->file_name.Buffer);

	if (devext->ui_file_name.Buffer)
		free(devext->ui_file_name.Buffer);

	if (devext->diskfile_type == RSDISK_TYPE_FIXED) {
		vdisk_file_close(devext->sd_stat.fd);
		memset(&devext->sd_stat, 0, sizeof(struct fixdisk_state));
	} else if (devext->diskfile_type == RSDISK_TYPE_DYNAMIC) {
		dyndisk_close(&devext->dd_stat);
		memset(&devext->dd_stat, 0, sizeof(struct dyndisk_state));
	}

	return STATUS_SUCCESS;
}

NTSTATUS vdisk_read_diskfile(PDEVICE_OBJECT devobj, PIRP irp)
{
	offset_t offset;
	long length;
	struct vdisk_devext *devext;
	PIO_STACK_LOCATION iosl;
	LARGE_INTEGER time;
	unsigned char *buffer;
	int ret = 0;

	devext = (struct vdisk_devext *)devobj->DeviceExtension;

	KeQuerySystemTime( &time );
	ExSystemTimeToLocalTime( &time, &devext->local_time );

	irp->IoStatus.Information = 0;
	iosl = IoGetCurrentIrpStackLocation(irp);

	if (iosl->Parameters.Read.ByteOffset.LowPart ==
	    FILE_USE_FILE_POINTER_POSITION
	    && iosl->Parameters.Read.ByteOffset.HighPart == -1) {
		offset = vdisk_file_tell(devext->dd_stat.fd);
	} else {
		offset = iosl->Parameters.Read.ByteOffset.QuadPart;
	}
	length = iosl->Parameters.Read.Length;

	if ((offset % VDISK_SECTOR_SIZE) || (length % VDISK_SECTOR_SIZE))
		return vdisk_complete_request(irp, STATUS_NOT_IMPLEMENTED);

	buffer =
	    (unsigned char *)MmGetSystemAddressForMdlSafe(irp->MdlAddress,
							  NormalPagePriority);
	if (!buffer)
		return vdisk_complete_request(irp,
					      STATUS_INSUFFICIENT_RESOURCES);
	if (devext->diskfile_type == RSDISK_TYPE_FIXED) {
		offset += VDISK_SECTOR_SIZE;
		vdisk_file_lseek(devext->sd_stat.fd, offset, 0);
		ret = vdisk_file_read(devext->sd_stat.fd, buffer, length);
		if (!ret || ret != length) {
			return vdisk_complete_request(irp, STATUS_UNSUCCESSFUL);
		}

		if (devext->sd_stat.crypt_method == RSDISK_CRYPT_AES) {
			encrypt_sectors(offset / VDISK_SECTOR_SIZE - 1, buffer,
					buffer, length / VDISK_SECTOR_SIZE, 0,
					&devext->decrypt_key);
		}

	} else if (devext->diskfile_type == RSDISK_TYPE_DYNAMIC) {
		if (dyndisk_read
		    (&devext->dd_stat, offset / VDISK_SECTOR_SIZE, buffer,
		     length / VDISK_SECTOR_SIZE) < 0)
			return vdisk_complete_request(irp, STATUS_UNSUCCESSFUL);
	} else {
		irp->IoStatus.Information = 0;
		return vdisk_complete_request(irp, STATUS_UNSUCCESSFUL);
	}

	irp->IoStatus.Information = length;
	return vdisk_complete_request(irp, STATUS_SUCCESS);
}

NTSTATUS vdisk_write_diskfile(PDEVICE_OBJECT devobj, PIRP irp)
{
	offset_t offset;
	long length;
	struct vdisk_devext *devext;
	PIO_STACK_LOCATION iosl;
	LARGE_INTEGER time;
	unsigned char *buffer;
	int ret = 0;

	devext = (struct vdisk_devext *)devobj->DeviceExtension;

	KeQuerySystemTime( &time );
	ExSystemTimeToLocalTime( &time, &devext->local_time );

	irp->IoStatus.Information = 0;
	iosl = IoGetCurrentIrpStackLocation(irp);

	if (iosl->Parameters.Write.ByteOffset.LowPart ==  FILE_USE_FILE_POINTER_POSITION  && iosl->Parameters.Write.ByteOffset.HighPart == -1) 
	{
		offset = vdisk_file_tell(devext->dd_stat.fd);
	} 
	else 
	{
		offset = iosl->Parameters.Write.ByteOffset.QuadPart;
	}

	length = iosl->Parameters.Write.Length;

	if ((offset % VDISK_SECTOR_SIZE) || (length % VDISK_SECTOR_SIZE))
	{
		return vdisk_complete_request(irp, STATUS_NOT_IMPLEMENTED);
	}

	buffer = (unsigned char *)MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
	if (!buffer)
		return vdisk_complete_request(irp, STATUS_INVALID_PARAMETER);

	if (devext->diskfile_type == RSDISK_TYPE_FIXED) 
	{
		offset += VDISK_SECTOR_SIZE;
		vdisk_file_lseek(devext->sd_stat.fd, offset, 0);
		if (devext->sd_stat.crypt_method == RSDISK_CRYPT_AES) 
		{
			uint8_t *encryptbuf = malloc( length );
			if (!encryptbuf) 
			{
				return vdisk_complete_request(irp, STATUS_UNSUCCESSFUL);
			}

			encrypt_sectors(offset / VDISK_SECTOR_SIZE - 1, encryptbuf, buffer, length / VDISK_SECTOR_SIZE, 1, &devext->encrypt_key);
			ret = vdisk_file_write(devext->sd_stat.fd, encryptbuf,length);
			free(encryptbuf);
		} 
		else 
		{
			ret =  vdisk_file_write(devext->sd_stat.fd, buffer, length);
		}
		if (!ret || ret != length) 
		{
			return vdisk_complete_request(irp, STATUS_UNSUCCESSFUL);
		}
	} 
	else if (devext->diskfile_type == RSDISK_TYPE_DYNAMIC) 
	{
		if (dyndisk_write(&devext->dd_stat, offset / VDISK_SECTOR_SIZE, buffer,length / VDISK_SECTOR_SIZE) < 0)
		{
			return vdisk_complete_request(irp, STATUS_UNSUCCESSFUL);
		}
	} 
	else 
	{
		irp->IoStatus.Information = 0;
		return vdisk_complete_request(irp, STATUS_UNSUCCESSFUL);
	}

	irp->IoStatus.Information = length;
	return vdisk_complete_request(irp, STATUS_SUCCESS);
}

void vdisk_irp_queue_worker(void *opaque)
{
	PDEVICE_OBJECT devobj;
	struct vdisk_devext *devext;
	PLIST_ENTRY request;
	PIRP irp;
	PIO_STACK_LOCATION iosl;

	ASSERT(opaque);

	devobj = (PDEVICE_OBJECT) opaque;
	devext = (struct vdisk_devext *)devobj->DeviceExtension;

	KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

	vdisk_adjust_privilege(SE_IMPERSONATE_PRIVILEGE, TRUE);

	while (1) {

		KeWaitForSingleObject(&devext->hdr.request_event, Executive,
				      KernelMode, FALSE, NULL);

		if (devext->hdr.terminate_thread)
			PsTerminateSystemThread(STATUS_SUCCESS);

		while (request =
		       ExInterlockedRemoveHeadList(&devext->hdr.list_head,
						   &devext->hdr.list_lock)) {

			irp =
			    CONTAINING_RECORD(request, IRP,
					      Tail.Overlay.ListEntry);
			iosl = IoGetCurrentIrpStackLocation(irp);

			switch (iosl->MajorFunction) {

			case IRP_MJ_READ:
				vdisk_read_diskfile(devobj, irp);
				break;

			case IRP_MJ_WRITE:
				vdisk_write_diskfile(devobj, irp);
				break;

			case IRP_MJ_DEVICE_CONTROL:
				switch (iosl->Parameters.DeviceIoControl.
					IoControlCode) {

				default:
					irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
					break;
				}

				IoCompleteRequest(irp, IO_NO_INCREMENT);
				break;

			default:
				vdisk_complete_request(irp, STATUS_DRIVER_INTERNAL_ERROR);
				break;
			}
		}
	}
}


NTSTATUS vdisk_create_mount_point( PDEVICE_OBJECT cdo_devobj, struct vdisk_info *vdi, DEVICE_TYPE devtype, PANSI_STRING dev_name )
{
	NTSTATUS rc = STATUS_SUCCESS;
	PDEVICE_OBJECT found_vdo_dev = NULL;
	struct cdisk_devext *cdo_devext = (struct cdisk_devext *)cdo_devobj->DeviceExtension;
	struct vdisk_devext *found_vdo_devext = NULL;
	struct user_ctx	*uc = NULL;
	PLIST_ENTRY	request = cdo_devext->vdo_lst_hdr.Flink;
	WCHAR device_name_buffer[MAXIMUM_FILENAME_LENGTH] = L"\0";
	UNICODE_STRING device_name = {0};
	UNICODE_STRING ui_filename = {0};

	ui_filename.Length = 0;
	ui_filename.MaximumLength = MAXIMUM_FILENAME_LENGTH * sizeof(WCHAR);
	ui_filename.Buffer = ExAllocatePoolWithTag( NonPagedPool, ui_filename.MaximumLength, RSVD_TAG );
	if (ui_filename.Buffer == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	if (!vdisk_make_filename( vdi->filename, &ui_filename )) {
		return STATUS_INVALID_PARAMETER;
	}

	found_vdo_dev = vdisk_find_mount_vdo( cdo_devext, &ui_filename );
	if (found_vdo_dev != NULL) {
		found_vdo_devext = found_vdo_dev->DeviceExtension;
	}

	ExFreePool( ui_filename.Buffer );
	
	if (found_vdo_devext != NULL) {

		uc = vdisk_find_userid( found_vdo_devext, vdi->userid );

		if (uc != NULL) {

			/* 已知用户 */
			uc->driver_map |= (ULONG)(1 << vdi->letter);

		} else {

			uc = ExAllocatePoolWithTag( NonPagedPool, sizeof(struct user_ctx), RSVD_TAG );
			if (uc == NULL)
				return STATUS_INSUFFICIENT_RESOURCES;

			uc->userid = vdi->userid;
			uc->driver_map = 0;
			uc->driver_map |= (ULONG)(1 << vdi->letter);

			/* 发现已存在的挂载文件，应该返回一个设备名 */
			ExInterlockedInsertTailList( &found_vdo_devext->user_lst_hdr,
				&uc->user_lst,
				&found_vdo_devext->user_lst_lock );

		}

		device_name.Buffer = device_name_buffer;
		device_name.Length = 0;
		device_name.MaximumLength = MAXIMUM_FILENAME_LENGTH * sizeof(WCHAR);

		rc = vdisk_get_obj_name( found_vdo_dev, &device_name );

		DbgPrint( "kvdisk!mount: dev: %wZ [%08x], [%08x -> %c], map: %x\n", &device_name, found_vdo_dev, vdi->userid, vdi->letter + 'A', uc->driver_map );

	} else {

		swprintf( device_name_buffer, DEVICE_NAME_PREFIX L"%u-%u", vdi->userid, vdi->letter );
		RtlInitUnicodeString( &device_name, device_name_buffer );

		/* 没有挂载此文件，创建 */
		rc = vdisk_create_device( cdo_devobj, &device_name, vdi, devtype );

	}

	if (NT_SUCCESS( rc )) {

		RtlUnicodeStringToAnsiString( dev_name, &device_name, FALSE );
	}

	return rc;
}


NTSTATUS vdisk_delete_mount_point( PDEVICE_OBJECT cdo_devobj, uint32_t userid, uint8_t letter )
{
	NTSTATUS rc = STATUS_SUCCESS;
	PDEVICE_OBJECT found_vdo_dev = NULL;
	struct user_ctx *uc = NULL;
	struct vdo_ctx *vc = NULL;

	found_vdo_dev = vdisk_find_mount_vdo_byuser( cdo_devobj->DeviceExtension, userid, letter, &uc, &vc );
	if ((found_vdo_dev != NULL) && (uc != NULL)) {

		struct vdisk_devext *vdo_ext = found_vdo_dev->DeviceExtension;
		struct cdisk_devext *cdo_ext = cdo_devobj->DeviceExtension;

		/* 去除 该 驱动器索引 */
		uc->driver_map &= ~(1 << letter);

		DbgPrint( "kvdisk!umount: dev: [%08x], [%08x -> %c], map: %x\n", found_vdo_dev, userid, letter + 'A', uc->driver_map );

		if (uc->driver_map == 0) {

			/* 释放 该 用户 及 对应的索引 */
			ExInterlockedRemoveHeadList( uc->user_lst.Blink, &vdo_ext->user_lst_lock );
			ExFreePool( uc ); uc = NULL;
			
			if (vdo_ext->user_lst_hdr.Flink == &vdo_ext->user_lst_hdr) {

				DbgPrint( "kvdisk!umount: delete dev: [%08x]\n", found_vdo_dev );
				vdisk_delete_device( cdo_devobj, vc );
			}
		}

	}

	return rc;
}


NTSTATUS vdisk_create_device( PDEVICE_OBJECT cdo_devobj, PUNICODE_STRING device_name, struct vdisk_info *vdi, DEVICE_TYPE devtype )
{
	NTSTATUS rc = STATUS_UNSUCCESSFUL;
	PDRIVER_OBJECT drvobj = NULL;
	PDEVICE_OBJECT vdo_devobj = NULL;
	struct cdisk_devext *cdo_devext = NULL;
	struct vdisk_devext *vdo_devext = NULL;
	struct vdo_ctx *vc = NULL;
	struct user_ctx *uc = NULL;
	HANDLE thread_handle = NULL;

	ASSERT( cdo_devobj != NULL );
	
	cdo_devext = (struct cdisk_devext *)cdo_devobj->DeviceExtension;
	drvobj = cdo_devobj->DriverObject;

	ASSERT( drvobj != NULL );

	do {

		rc = IoCreateDevice( drvobj, sizeof(struct vdisk_devext), device_name, devtype, 0, FALSE, &vdo_devobj );
		if (!NT_SUCCESS( rc ))
			break;

		vc = ExAllocatePoolWithTag( NonPagedPool, sizeof(struct vdo_ctx), RSVD_TAG );
		if (vc == NULL) {
			rc = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

		vc->vdo_devobj = vdo_devobj;

		ExInterlockedInsertTailList( &cdo_devext->vdo_lst_hdr,
			&vc->vdo_lst,
			&cdo_devext->vdo_lst_lock );

		vdo_devobj->Flags |= DO_DIRECT_IO;
		vdo_devobj->Flags &= ~DO_DEVICE_INITIALIZING;

		vdo_devext = (struct vdisk_devext *)vdo_devobj->DeviceExtension;

		InitializeListHead( &vdo_devext->hdr.list_head );
		KeInitializeSpinLock( &vdo_devext->hdr.list_lock );
		KeInitializeEvent( &vdo_devext->hdr.request_event, SynchronizationEvent, FALSE );
		vdo_devext->hdr.terminate_thread = FALSE;

		/* 初始化 user 链表 */
		InitializeListHead( &vdo_devext->user_lst_hdr );
		KeInitializeSpinLock( &vdo_devext->user_lst_lock );

		uc = ExAllocatePoolWithTag( NonPagedPool, sizeof(struct user_ctx), RSVD_TAG );
		if (uc == NULL) {
			rc = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

		uc->userid = vdi->userid;
		uc->driver_map = 0;
		uc->driver_map |= (1 << vdi->letter);

		ExInterlockedInsertTailList( &vdo_devext->user_lst_hdr,
			&uc->user_lst,
			&vdo_devext->user_lst_lock );

		rc = PsCreateSystemThread( &thread_handle, 0L, NULL, NULL, NULL,
			vdisk_irp_queue_worker, vdo_devobj );
		if (!NT_SUCCESS( rc )) {
			break;
		}

		rc = ObReferenceObjectByHandle( thread_handle, THREAD_ALL_ACCESS, NULL,
			KernelMode, &vdo_devext->hdr.thread_pointer,
			NULL );
		if (!NT_SUCCESS( rc )) {
			ZwClose( thread_handle );
			thread_handle = NULL;
			vdo_devext->hdr.terminate_thread = TRUE;
			KeSetEvent( &vdo_devext->hdr.request_event, 0, FALSE );
			break;
		}

		rc = vdisk_open_diskfile( vdo_devobj, vdi );

		if (NT_SUCCESS( rc )) {
			DbgPrint( "kvdisk!mount: dev: %wZ [%08x], [%08x -> %c], map: %x\n", device_name, vdo_devobj, vdi->userid, vdi->letter + 'A', uc->driver_map );
		}


	} while (FALSE);

	if (!NT_SUCCESS( rc )) {

		if (vdo_devobj != NULL)
			IoDeleteDevice( vdo_devobj );

		if (vc != NULL) {
			ExInterlockedRemoveHeadList( vc->vdo_lst.Blink, &cdo_devext->vdo_lst_lock );
			ExFreePool( vc );
		}

		if (uc != NULL) {
			ExInterlockedRemoveHeadList( uc->user_lst.Blink, &vdo_devext->user_lst_lock );
			ExFreePool( uc );
		}

		if (thread_handle != NULL) {
			ZwClose( thread_handle );
		}
	}

	return rc;
}

NTSTATUS vdisk_delete_device( PDEVICE_OBJECT cdo_devobj, struct vdo_ctx *vc )
{
	struct cdisk_devext *cdo_devext;
	struct vdisk_devext *vdo_devext;
	PDEVICE_OBJECT	vdo_devobj;

	vdo_devobj = vc->vdo_devobj;
	cdo_devext = (struct cdisk_devext *)cdo_devobj->DeviceExtension;
	vdo_devext = (struct vdisk_devext *)vdo_devobj->DeviceExtension;

	ASSERT(vdo_devobj);

	vdisk_close_diskfile( vdo_devobj );

	ExInterlockedRemoveHeadList( vc->vdo_lst.Blink, &cdo_devext->vdo_lst_lock );
	ExFreePool( vc );

	vdo_devext->hdr.terminate_thread = TRUE;

	KeSetEvent(&vdo_devext->hdr.request_event, 0, FALSE);
	KeWaitForSingleObject(vdo_devext->hdr.thread_pointer, Executive, KernelMode, FALSE, NULL);

	ObDereferenceObject(vdo_devext->hdr.thread_pointer);

	if (vdo_devext->hdr.security_client_context) {

		SeDeleteClientSecurity(vdo_devext->hdr.security_client_context);
		free(vdo_devext->hdr.security_client_context);
	}

	IoDeleteDevice(vdo_devobj);

	return STATUS_SUCCESS;
}

void vdisk_cdo_irp_queue_worker(void *opaque)
{
	PDEVICE_OBJECT devobj;
	struct cdisk_devext *devext;
	PLIST_ENTRY request;
	PIRP irp;
	PIO_STACK_LOCATION iosl;
	char chdrvletter[2] = {'A',0};

	ASSERT(opaque);

	devobj = (PDEVICE_OBJECT) opaque;
	devext = (struct cdisk_devext *)devobj->DeviceExtension;

	KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

	vdisk_adjust_privilege(SE_IMPERSONATE_PRIVILEGE, TRUE);

	while (1) {
		KeWaitForSingleObject(&devext->hdr.request_event, Executive,
			KernelMode, FALSE, NULL);

		if (devext->hdr.terminate_thread)
			PsTerminateSystemThread(STATUS_SUCCESS);

		while (request = ExInterlockedRemoveHeadList(&devext->hdr.list_head,&devext->hdr.list_lock)) {
			
			irp = CONTAINING_RECORD(request, IRP,Tail.Overlay.ListEntry);			
			iosl = IoGetCurrentIrpStackLocation(irp);
			
			switch (iosl->MajorFunction) {
				
			case IRP_MJ_DEVICE_CONTROL:

				switch (iosl->Parameters.DeviceIoControl.IoControlCode) {
			        
			        case IOCTL_VDISK_CREATE_MOUNT_POINT:
					{
						ANSI_STRING dev_name;
						struct vdisk_info *vdi;
						vdi = ((struct vdisk_info *)irp->AssociatedIrp.SystemBuffer);

						dev_name.Buffer = irp->AssociatedIrp.SystemBuffer;
						dev_name.Length = 0;
						dev_name.MaximumLength = iosl->Parameters.DeviceIoControl.OutputBufferLength;

						irp->IoStatus.Information = 0;

						irp->IoStatus.Status = vdisk_create_mount_point( devobj, vdi, FILE_DEVICE_DISK, &dev_name );
						if (NT_SUCCESS( irp->IoStatus.Status )) {
							irp->IoStatus.Information = dev_name.Length;
						}

					}
					break;
				
				case IOCTL_VDISK_DELETE_MOUNT_POINT:
					{
						struct vdisk_info *vdi;
						vdi = ((struct vdisk_info *)irp->AssociatedIrp.SystemBuffer);

						if (vdi->letter < 0 || vdi->letter > 25 || vdi->userid == 0) {
							irp->IoStatus.Status = STATUS_INVALID_PARAMETER;					
						} else {
							irp->IoStatus.Status = vdisk_delete_mount_point( devobj, vdi->userid, vdi->letter );
						}

						irp->IoStatus.Information = 0;

					}
					break;
				
				default:
					irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
					break;
				}

				IoCompleteRequest(irp, IO_NO_INCREMENT);
				break;
			default:
				vdisk_complete_request(irp, STATUS_DRIVER_INTERNAL_ERROR);
				break;
			}
		}
	}
}

void vdisk_unload( PDRIVER_OBJECT drvobj )
{
	struct cdisk_devext *cdo_devext = (struct cdisk_devext *)g_cdodevobj->DeviceExtension;
	UNICODE_STRING cdo_devicelink_name;
	struct user_data_ctx *found_user_ctx = NULL;
	PLIST_ENTRY	request = cdo_devext->user_data_lst_hdr.Flink;


	cdo_devext->hdr.terminate_thread = TRUE;
	KeSetEvent(&cdo_devext->hdr.request_event, 0, FALSE);
	KeWaitForSingleObject(cdo_devext->hdr.thread_pointer, Executive, KernelMode, FALSE, NULL);

	ObDereferenceObject(cdo_devext->hdr.thread_pointer);

	if (cdo_devext->hdr.security_client_context) {

		SeDeleteClientSecurity(cdo_devext->hdr.security_client_context);
		free(cdo_devext->hdr.security_client_context);
	}

	/* 删除 用户数据 */
	while (request = ExInterlockedRemoveHeadList( &cdo_devext->user_data_lst_hdr, &cdo_devext->user_data_lst_lock )) {

		struct user_data_ctx *udc = CONTAINING_RECORD( request, struct user_data_ctx, user_data_lst );
		ExFreePool( udc );
	}

	/* 删除 VDO */
	while (request = ExInterlockedRemoveHeadList( &cdo_devext->vdo_lst_hdr, &cdo_devext->vdo_lst_lock )) {

		struct vdo_ctx *vc = CONTAINING_RECORD( request, struct vdo_ctx, vdo_lst );
		struct vdisk_devext *vdo_ext = vc->vdo_devobj->DeviceExtension;

		/* 删除 用户信息 */
		while (request = ExInterlockedRemoveHeadList( &vdo_ext->user_lst_hdr, &vdo_ext->user_lst_lock )) {

			struct user_ctx *uc = CONTAINING_RECORD( request, struct user_ctx, user_lst );
			ExFreePool( uc );
		}

		if (vdo_ext->ui_file_name.Buffer != NULL)
			ExFreePool( vdo_ext->ui_file_name.Buffer );

		if (vdo_ext->file_name.Buffer != NULL)
			ExFreePool( vdo_ext->file_name.Buffer );

		/* 关闭已打开的文件 */
		if (vdo_ext->sd_stat.fd != NULL) {
			vdisk_file_close( vdo_ext->sd_stat.fd );
			vdo_ext->sd_stat.fd = NULL;
		}

		/* 停止 线程 */
		vdo_ext->hdr.terminate_thread = TRUE;
		KeSetEvent( &vdo_ext->hdr.request_event, 0, FALSE );

		IoDeleteDevice( vc->vdo_devobj );

		ExFreePool( vc );

	}

	/* 停止 线程 */
	cdo_devext->hdr.terminate_thread = TRUE;
	KeSetEvent( &cdo_devext->hdr.request_event, 0, FALSE );

	IoDeleteDevice( g_cdodevobj );
	RtlInitUnicodeString( &cdo_devicelink_name, DEVICE_LINK_NAME_CDO );
	IoDeleteSymbolicLink( &cdo_devicelink_name );

}

NTSTATUS DriverEntry( PDRIVER_OBJECT drvobj, PUNICODE_STRING regpath )
{
	NTSTATUS rc;
	UNICODE_STRING cdo_device_name;
	UNICODE_STRING cdo_devicelink_name;
	PDEVICE_OBJECT cdo_devobj;
	struct cdisk_devext *cdo_devext;
	HANDLE thread_handle;

	ASSERT(drvobj);

	//create CDO
	RtlInitUnicodeString(&cdo_device_name, DEVICE_NAME_CDO);
	rc = IoCreateDevice(drvobj, sizeof(struct cdisk_devext), &cdo_device_name,FILE_DEVICE_DISK, 0, FALSE, &cdo_devobj);
	if (!NT_SUCCESS(rc))
		return rc;

	RtlInitUnicodeString(&cdo_devicelink_name,DEVICE_LINK_NAME_CDO);
	rc = IoCreateSymbolicLink(&cdo_devicelink_name,&cdo_device_name);
	if (!NT_SUCCESS(rc)) 
	{
		IoDeleteDevice(cdo_devobj);
		return STATUS_UNSUCCESSFUL;
	}

	cdo_devobj->Flags |= DO_DIRECT_IO;	
	cdo_devext = (struct cdisk_devext *)cdo_devobj->DeviceExtension;

	g_cdodevobj = cdo_devobj;

	InitializeListHead( &cdo_devext->vdo_lst_hdr );
	KeInitializeSpinLock( &cdo_devext->vdo_lst_lock );

	/* 用户数据链 */
	InitializeListHead( &cdo_devext->user_data_lst_hdr );
	KeInitializeSpinLock( &cdo_devext->user_data_lst_lock );

	InitializeListHead(&cdo_devext->hdr.list_head);
	KeInitializeSpinLock(&cdo_devext->hdr.list_lock);
	KeInitializeEvent(&cdo_devext->hdr.request_event, SynchronizationEvent, FALSE);
	cdo_devext->hdr.terminate_thread = FALSE;
	rc = PsCreateSystemThread(&thread_handle, 0L, NULL, NULL, NULL,
		vdisk_cdo_irp_queue_worker, cdo_devobj);
	if (!NT_SUCCESS(rc)) {
		IoDeleteDevice(cdo_devobj);
		return rc;
	}
	rc = ObReferenceObjectByHandle(thread_handle, THREAD_ALL_ACCESS, NULL, KernelMode, &cdo_devext->hdr.thread_pointer, NULL);
	if (!NT_SUCCESS(rc)) {
		ZwClose(thread_handle);
		cdo_devext->hdr.terminate_thread = TRUE;
		KeSetEvent(&cdo_devext->hdr.request_event, 0, FALSE);
		IoDeleteDevice(cdo_devobj);
		return rc;
	}
	ZwClose(thread_handle);

	drvobj->MajorFunction[IRP_MJ_CREATE] = vdisk_create_close;
	drvobj->MajorFunction[IRP_MJ_CLOSE] = vdisk_create_close;
	drvobj->MajorFunction[IRP_MJ_READ] = vdisk_read_write;
	drvobj->MajorFunction[IRP_MJ_WRITE] = vdisk_read_write;
	drvobj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = vdisk_ioctl;
	drvobj->DriverUnload = vdisk_unload;

	return STATUS_SUCCESS;
}
