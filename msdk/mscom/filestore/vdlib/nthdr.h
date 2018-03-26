#ifndef _NT_HEADER_H_
#define _NT_HEADER_H_

//
// We include some stuff from newer DDK:s here so that one
// version of the driver for all versions of Windows can
// be compiled with the Windows NT 4.0 DDK.
//
#if (VER_PRODUCTBUILD < 2195)

#define FILE_DEVICE_MASS_STORAGE            0x0000002d
#define IOCTL_STORAGE_CHECK_VERIFY2         CTL_CODE(IOCTL_STORAGE_BASE, 0x0200, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FILE_ATTRIBUTE_ENCRYPTED            0x00004000

#endif

#if (VER_PRODUCTBUILD < 2600)

#define IOCTL_DISK_GET_PARTITION_INFO_EX    CTL_CODE(IOCTL_DISK_BASE, 0x0012, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISK_GET_LENGTH_INFO          CTL_CODE(IOCTL_DISK_BASE, 0x0017, METHOD_BUFFERED, FILE_READ_ACCESS)

typedef enum _PARTITION_STYLE {
	PARTITION_STYLE_MBR,
	PARTITION_STYLE_GPT
} PARTITION_STYLE;

typedef unsigned __int64 ULONG64, *PULONG64;

typedef struct _PARTITION_INFORMATION_MBR {
	UCHAR PartitionType;
	BOOLEAN BootIndicator;
	BOOLEAN RecognizedPartition;
	ULONG HiddenSectors;
} PARTITION_INFORMATION_MBR, *PPARTITION_INFORMATION_MBR;

typedef struct _PARTITION_INFORMATION_GPT {
	GUID PartitionType;
	GUID PartitionId;
	ULONG64 Attributes;
	WCHAR Name[36];
} PARTITION_INFORMATION_GPT, *PPARTITION_INFORMATION_GPT;

typedef struct _PARTITION_INFORMATION_EX {
	PARTITION_STYLE PartitionStyle;
	LARGE_INTEGER StartingOffset;
	LARGE_INTEGER PartitionLength;
	ULONG PartitionNumber;
	BOOLEAN RewritePartition;
	union {
		PARTITION_INFORMATION_MBR Mbr;
		PARTITION_INFORMATION_GPT Gpt;
	};
} PARTITION_INFORMATION_EX, *PPARTITION_INFORMATION_EX;

typedef struct _GET_LENGTH_INFORMATION {
	LARGE_INTEGER Length;
} GET_LENGTH_INFORMATION, *PGET_LENGTH_INFORMATION;

#endif				// (VER_PRODUCTBUILD < 2600)

//
// We include some stuff from ntifs.h here so that
// the driver can be compiled with only the DDK.
//

#define TOKEN_SOURCE_LENGTH 8

typedef enum _TOKEN_TYPE {
	TokenPrimary = 1,
	TokenImpersonation
} TOKEN_TYPE;

typedef struct _TOKEN_SOURCE {
	CCHAR SourceName[TOKEN_SOURCE_LENGTH];
	LUID SourceIdentifier;
} TOKEN_SOURCE, *PTOKEN_SOURCE;

typedef struct _TOKEN_CONTROL {
	LUID TokenId;
	LUID AuthenticationId;
	LUID ModifiedId;
	TOKEN_SOURCE TokenSource;
} TOKEN_CONTROL, *PTOKEN_CONTROL;

typedef struct _SECURITY_CLIENT_CONTEXT {
	SECURITY_QUALITY_OF_SERVICE SecurityQos;
	PACCESS_TOKEN ClientToken;
	BOOLEAN DirectlyAccessClientToken;
	BOOLEAN DirectAccessEffectiveOnly;
	BOOLEAN ServerIsRemote;
	TOKEN_CONTROL ClientTokenControl;
} SECURITY_CLIENT_CONTEXT, *PSECURITY_CLIENT_CONTEXT;

#define PsDereferenceImpersonationToken(T)  { \
	if (ARGUMENT_PRESENT(T)) { \
		(ObDereferenceObject((T))); \
	} else { \
		; \
	} \
}

#define PsDereferencePrimaryToken(T) (ObDereferenceObject((T)))

NTKERNELAPI VOID PsRevertToSelf(VOID);

NTKERNELAPI
    NTSTATUS SeCreateClientSecurity(PETHREAD Thread,
				    PSECURITY_QUALITY_OF_SERVICE
				    QualityOfService, BOOLEAN RemoteClient,
				    PSECURITY_CLIENT_CONTEXT ClientContext);

#define SeDeleteClientSecurity(C)  { \
	if (SeTokenType((C)->ClientToken) == TokenPrimary) { \
		PsDereferencePrimaryToken( (C)->ClientToken ); \
	} else { \
		PsDereferenceImpersonationToken( (C)->ClientToken ); \
	} \
}

NTKERNELAPI
    VOID SeImpersonateClient(PSECURITY_CLIENT_CONTEXT ClientContext,
			     PETHREAD ServerThread OPTIONAL);

NTKERNELAPI TOKEN_TYPE SeTokenType(PACCESS_TOKEN Token);

#ifndef SE_IMPERSONATE_PRIVILEGE
#define SE_IMPERSONATE_PRIVILEGE        (29L)
#endif

#define TOKEN_ASSIGN_PRIMARY            (0x0001)
#define TOKEN_DUPLICATE                 (0x0002)
#define TOKEN_IMPERSONATE               (0x0004)
#define TOKEN_QUERY                     (0x0008)
#define TOKEN_QUERY_SOURCE              (0x0010)
#define TOKEN_ADJUST_PRIVILEGES         (0x0020)
#define TOKEN_ADJUST_GROUPS             (0x0040)
#define TOKEN_ADJUST_DEFAULT            (0x0080)

#define TOKEN_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED |\
	TOKEN_ASSIGN_PRIMARY     |\
	TOKEN_DUPLICATE          |\
	TOKEN_IMPERSONATE        |\
	TOKEN_QUERY              |\
	TOKEN_QUERY_SOURCE       |\
	TOKEN_ADJUST_PRIVILEGES  |\
	TOKEN_ADJUST_GROUPS      |\
	TOKEN_ADJUST_DEFAULT)

typedef struct _TOKEN_PRIVILEGES {
	ULONG PrivilegeCount;
	LUID_AND_ATTRIBUTES Privileges[1];
} TOKEN_PRIVILEGES, *PTOKEN_PRIVILEGES;

NTSYSAPI
    NTSTATUS NTAPI ZwOpenProcessToken(HANDLE ProcessHandle,
				      ACCESS_MASK DesiredAccess,
				      PHANDLE TokenHandle);

NTSYSAPI
    NTSTATUS NTAPI NtAdjustPrivilegesToken(HANDLE TokenHandle,
					   BOOLEAN DisableAllPrivileges,
					   PTOKEN_PRIVILEGES NewState,
					   ULONG BufferLength,
					   PTOKEN_PRIVILEGES PreviousState
					   OPTIONAL, PULONG ReturnLength);

NTSYSAPI
NTSTATUS
ObQueryNameString(
		  IN PVOID  Object,
		  OUT POBJECT_NAME_INFORMATION  ObjectNameInfo,
		  IN ULONG  Length,
		  OUT PULONG  ReturnLength
		  );



//
// For backward compatibility with Windows NT 4.0 by Bruce Engle.
//
#ifndef MmGetSystemAddressForMdlSafe
#define MmGetSystemAddressForMdlSafe(MDL, PRIORITY) MmGetSystemAddressForMdlPrettySafe(MDL)

PVOID MmGetSystemAddressForMdlPrettySafe(PMDL Mdl)
{
	CSHORT MdlMappingCanFail;
	PVOID MappedSystemVa;

	MdlMappingCanFail = Mdl->MdlFlags & MDL_MAPPING_CAN_FAIL;

	Mdl->MdlFlags |= MDL_MAPPING_CAN_FAIL;

	MappedSystemVa = MmGetSystemAddressForMdl(Mdl);

	if (MdlMappingCanFail == 0)
		Mdl->MdlFlags &= ~MDL_MAPPING_CAN_FAIL;

	return MappedSystemVa;
}
#endif

int swprintf(wchar_t *, const wchar_t *, ...);

#endif				/* _NT_HEADER_H_ */
