#pragma once
#include <mscom\msplugin.h>
#include <database/genericdb.h>

//增加注释
interface ITaskDBFast : public IMsPlugin 
{
	STDMETHOD( CreateTask )(IN LPCWSTR szDBName,IN LPWSTR szTableName,OUT DWORD& dwPort) =0 ;
	
	STDMETHOD( ExecDML )(IN LPCWSTR szDBName,LPCWSTR szSql) =0 ;
	STDMETHOD( Execute )(IN LPCWSTR szDBName,LPCTSTR szSql,IGenericMS** ppRS ) =0 ;
	
	STDMETHOD( CloseTask )(IN LPCWSTR szDBName) =0 ;
};
MS_DEFINE_IID(ITaskDBFast, "{B80C24AF-DD6B-4188-A8B6-D58750448793}");

// {198E9227-F926-4D20-8824-DE24B6A4A855}
MS_DEFINE_GUID(CLSID_ITaskDBFast,
			0x198e9227, 0xf926, 0x4d20, 0x88, 0x24, 0xde, 0x24, 0xb6, 0xa4, 0xa8, 0x55);


//无盘模式的组件CLSID
// {F1E7CF3F-9B4E-4AE3-9BEE-2070EEF6AB5F}
MS_DEFINE_GUID(CLSID_ITaskDBFastDiskLess, 
			0xf1e7cf3f, 0x9b4e, 0x4ae3, 0x9b, 0xee, 0x20, 0x70, 0xee, 0xf6, 0xab, 0x5f);


// {A3204825-3E94-464B-8339-42EA4D2BDC03}
MS_DEFINE_GUID(CLSID_ITaskDBFastRemote,
			0xa3204825, 0x3e94, 0x464b, 0x83, 0x39, 0x42, 0xea, 0x4d, 0x2b, 0xdc, 0x3);

//当无盘模式FastDB创建内存失败，时，返回这个错误值。
#define E_FileLimitExeeded	-11112359


//sub sql参数说明.
/*
fprintf(stderr, "SubSQL commands:\n\n\
				open 'database-name' ( 'database-file-name' ) ';'\n\
				select ('*') from <table-name> where <condition> ';'\n\
				update <table-name> set <field-name> '=' <expression> {',' <field-name> '=' <expression>} where <condition> ';'\n\
				create table <table-name> '('<field-name> <field-type> {',' <field-name> <field-type>}')' ';' \n\
				alter table <table-name> '('<field-name> <field-type> {',' <field-name> <field-type>}')' ';' \n\
				rename <table-name> '.' <old-field-name> 'to' <new-field-name> ';' \n\
				delete from <table-name>\n\
				drop table <table-name>\n\
				drop index <table-name> {'.' <field-name>} ';'\n\
				create index on <table-name> {'.' <field-name>} ';'\n\
				drop hash <table-name> {'.' <field-name>};\n\
				create hash on <table-name> {'.' <field-name>}field> ';'\n\
				insert into <table-name> values '(' <value>{',' <value>} ')' ';'\n\
				backup [compactify] 'file-name'\n\
				start server URL number-of-threads\n\
				stop server URL\n\
				start http server URL\n\
				stop http server\n\
				describe <table-name>\n\
				import 'xml-file-name'\n\
				export 'xml-file-name'\n\
				show\n\
				profile\n\
				commit\n\
				rollback\n\
				autocommit (on|off)\n\
				exit\n\
				help\n\n");

				*/





