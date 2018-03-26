
#pragma once

namespace msdk{;
namespace mscom{;
//--------------------------------------------------------------------
//	IGenericMS
//	通用记录集接口

//  Demo( 使用prophelpers.h中的帮助类 ):
//  
//  IGenericDB* pDB; //   已经打开的连接
//  
//  UTIL::com_ptr<IGenericMS>	pRS;
//  RFAILED( pDB->Execute( _T("SELECT * FROM Table"), &pRS.m_p ) );
//  
//  while( !pRS->IsEOF() )
//  {
//  	UTIL::com_ptr<IProperty2>	prop;
//  	RFAILED( pRS->GetRecord( &prop.m_p ) );
//  	CPropSet propSet( prop );
//  
//  	int nFiled1 = propSet[0];
//  	LPTSTR szField2 = propSet[1];
//  
//  	RFAILED( pRS->MoveNext() );
//  }
//---
//  while( !pRS->IsEOF() )
//  {
//  	UTIL::com_ptr<IPropertyStr>	prop;
//  	RFAILED( pRS->GetRecord( &prop.m_p ) );
//  	CPropSet propStrSet( prop );
//  
//  	int nFiled1 = propSet["id"];
//  	LPTSTR szField2 = propSet["url"];
//  
//  	RFAILED( pRS->MoveNext() );
//  }
//--------------------------------------------------------------------
interface IGenericMS : public IUnknown
{
	//************************************
	// Method:    GetRecord 获取当前位置的记录
	// Returns:   如果成功获取记录，返回S_OK；否则返回E_FAIL
	// Parameter: ppRecord[out] 输出当前位置的记录；如果当前位置已经
	//	          为EOF，则输出NULL，且返回值为E_FAIL。
	// Remark:	  1.对ppRecord中的字段进行修改不会保存进数据库
	//			  2.读取ppRecord中的字段，键值使用字段的索引（从0开始）；
	//				除非在IGenericStmt::ExecuteDML或者IGenericDB::Execute
	//				中通过pFields为字段指定键值
	//************************************
	STDMETHOD( GetRecord )( IProperty2** ppRecord ) PURE;


	STDMETHOD( GetRecord )( IPropertyStr** ppRecord ) PURE;

	//************************************
	// Method:    GetScale 获取标量结果（即当前位置记录索引为0的字段值）
	// Returns:   如果成功获取记录，返回S_OK；否则返回E_FAIL
	// Parameter: pScale[out]
	// Remark:	  提供这个函数的目的是简化在查询聚集函数（如COUNT(*)）,
	//			  或者只查询单一字段（如ID）时的结果获取过程
	//************************************
	STDMETHOD( GetScale )( PROPVARIANT* pScale ) PURE;

	//************************************
	// Method:    MoveFirst 将当前位置移动到第一条记录
	// Returns:   成功返回S_OK；否则返回E_FAIL
	// Parameter: VOID
	// Remark:	  在首次获取第一条记录的时候，无需调用此函数；
	//			  在便利完所有记录的时候，如果需要从第一条再次
	//			  便利，才需要调用
	//************************************
	STDMETHOD( MoveFirst )( VOID ) PURE;


	//************************************
	// Method:    MoveNext 将当前位置移动到下一条记录
	// Returns:   成功返回S_OK；如果已经移动到EOF返回E_FALSE
	// Parameter: VOID
	// Remark:	  在遍历过程中需要调用此函数，GetRecord不会
	//			  自动移动当前位置
	//************************************
	STDMETHOD( MoveNext )( VOID ) PURE;
	
	//************************************
	// Method:    IsEOF 判断是否已经遍历到EOF
	//************************************
	STDMETHOD_( BOOL, IsEOF )( VOID ) PURE;
};

MS_DEFINE_IID(IGenericMS, "{A2B1174F-EC18-45D1-8B1C-7526165DCCCE}");

//--------------------------------------------------------------------
//	IGenericStmt
//	通用预编译语句接口，使用预编译语句可以省去解析SQL的开销。但是
//	预编译语句的维护会消耗系统资源，所以应该为频繁的查询使用预编译
//
//  Demo( 使用prophelpers.h中的帮助类 ):
//  
//  IGenericDB* pDB; //   已经打开的连接
//  
//	LPTSTR szSQL = _T("INSERT INTO Table(ID, Name) VALUES(?,?)");
//  UTIL::com_ptr<IGenericStmt>	pStmt;
//  RFAILED( pDB->CompileStmt( szSQL, &pStmt.m_p ) );
//	CPropVar varID, varName;
//  
//  for( int i=0; i<100; i++ )
//  {
//  	varID = i;
//  	varName = _T("Somebody");
//  	
//		RFAILED( pStmt->BindParam( 0, &varID ) );
//  	RFAILED( pStmt->BindParam( 1, &varName ) );
//  	RFAILED( pStmt->ExecuteDML() );
//  
//  	pStmt->Reset();
//  }
//
//	Demo2：BindParams的使用
//	LPTSTR szSQL = _T("INSERT INTO Table(ID, Name) VALUES(?,?)");
//  UTIL::com_ptr<IGenericStmt>	pStmt;
//  RFAILED( pDB->CompileStmt( szSQL, &pStmt.m_p ) );
//	
//	IProperty2*	params;		// 构造好的IProperty2接口
//	CPropSet propSet( params );	
//  
//  for( int i=0; i<100; i++ )
//  {
//  	LPCTSTR szName = _T("Somebody");
//  	
//		propSet[0] = i;
//		propSet[1] = szName;
//  
//  	pStmt->Reset();
//		pStmt->BindParams( params );
//		RFAILED( pStmt->ExecuteDML() );
//  }
//--------------------------------------------------------------------
interface IGenericStmt : public IUnknown
{
	//************************************
	// Method:    BindParam 在参数化查询中绑定参数
	// Returns:   绑定成功返回S_OK，否则返回E_FAIL
	// Parameter: nIndex[in] 参数索引，从0开始
	// Parameter: value[in] 绑定的参数值
	//************************************
	STDMETHOD( BindParam )( INT nIndex, PROPVARIANT* value ) PURE;

	//************************************
	// Method:    Reset 重置预编译语句
	// Remark:	  在每次绑定所有参数之前需要调用此函数
	//************************************
	STDMETHOD( Reset )( VOID ) PURE;

	//************************************
	// Method:    ExecuteDML 执行DML语句，不返回结果集
	// Returns:   执行成功返回S_OK，否则返回E_FAIL
	// Parameter: pAffected[out]，如果关心被修改的记录数量，通过此
	//			  参数输出，否则传NULL
	//************************************
	STDMETHOD( ExecuteDML )( LPLONG pAffected=NULL ) PURE;

	//************************************
	// Method:    Execute 执行SELECT语句，返回结果集
	// Parameter: ppRST[out] 输出结果集
	// Parameter: pFields和nFields[in] 为结果集中的字段指定键值，用于
	//			  从IProperty2中用指定键值获取字段；如不指定则为从
	//			  0开始的连续整数
	//************************************
	STDMETHOD( Execute )( IGenericMS** ppRST, DWORD* pFields=NULL, INT nFields=0 ) PURE;

	//************************************
	// Method:   BindParams 在参数化查询中绑定全部参数
	// Returns:   绑定成功返回S_OK，否则返回E_FAIL
	// Parameter: params[in]，装载参数的容器。键值与参数从0开始的索引对应。
	// Remark:	  如果没有提供全部的参数值，会自动为没有提供值的参数绑定NULL
	//************************************
	STDMETHOD( BindParams )( IProperty2* params ) PURE;
};

MS_DEFINE_IID(IGenericStmt, "{B111DE8F-C40F-4955-9AAD-5CF02F029575}");

enum MsFieldType
{
	field_invalid = 0,
	field_bigint,
	field_int,
	field_string,
	field_binary,
	field_datetime,
	field_guid,
	field_smallint
};

//--------------------------------------------------------------------
//	IGenericDB 通用数据库连接接口
//--------------------------------------------------------------------
interface IGenericDB : public IUnknown
{
	//************************************
	// Method:    Connect 连接数据库
	// Returns:   连接成功返回S_OK，否则返回E_FAIL
	// Parameter: szConn[in] 连接字符串（登陆数据库的用户名等。对于当前
	//			  的SQLITE实现，只需要传入数据文件的全路径，以后可以扩展）
	// Parameter: lOptions 连接选项，用于扩展，暂时没有定义
	//************************************

	//**
	//	CLSID_DbSqliteDB 链接方式：
	//		1、(有密码的SQLITE数据库)Database=c:\\database.db;Pwd=password
	//		2、(没有密码的SQLITE数据库) Database=c:\\database.db 或 c:\\database.db
	//*/
	STDMETHOD( Connect )( LPCTSTR szConn, LONG lOptions=0 ) PURE;

	//************************************
	// Method:    Close 断开数据库连接
	//************************************
	STDMETHOD( Close )( void ) PURE;

	//************************************
	// Method:    IsTableExists 查询指定的数据表是否存在
	// Returns:   存在返回TRUE，否则返回FALSE
	// Parameter: szTable[in] 表名
	//************************************
	STDMETHOD_( BOOL, IsTableExists )( LPCTSTR szTable ) PURE;

	//************************************
	// Method:    CompileStmt 预编译SQL语句
	// Returns:   编译成功返回S_OK，否则返回E_FAIL
	// Parameter: szSQL[in] 要编译的SQL语句
	// Parameter: ppStmt[out] 输出IGenericStmt指针
	//************************************
	STDMETHOD( CompileStmt )( LPCTSTR szSQL, IGenericStmt** ppStmt ) PURE;

	//************************************
	// Method:    ExecuteDML 直接执行DML语句（不预编译）
	// Returns:   执行成功返回S_OK，否则返回E_FAIL
	// Parameter: szSQL[in] : DML语句的内容
	// Parameter: pAffected[out]，如果关心被修改的记录数量，通过此
	//			  参数输出，否则传NULL
	//************************************
	STDMETHOD( ExecuteDML )( LPCTSTR szSQL, LONG* pAffected=NULL ) PURE;

	//************************************
	// Method:    Execute 直接执行SELECT语句，返回结果集（不预编译）
	// Returns:   执行成功返回S_OK，否则返回E_FAIL
	// Parameter: szSQL[in] : SELECT语句的内容
	// Parameter: ppRecordset[out] 输出结果集
	// Parameter: pFields和nFields[in] 为结果集中的字段指定键值，用于
	//			  从IProperty2中用指定键值获取字段；如不指定则为从
	//			  0开始的连续整数
	//************************************
	STDMETHOD( Execute )( LPCTSTR szSQL, IGenericMS** ppRecordset, DWORD* pFields=NULL, INT nFields=0 ) PURE;

	//************************************
	// Method:    GetTypeDecl 获取指定类型的声明类型
	// Returns:   成功返回S_OK；如果没有对应的声明类型，返回E_FAIL
	// Parameter: vt[in] 指定的类型
	// Parameter: szBuf[out] 声明类型将拷贝到这里
	// Parameter: nBufLen[in] szBuf的大小，以字符为单位
	// Remark:	  此函数的主要应用场景是动态生成CREATE TABLE语句。因为
	//			  不同的数据库中的数据声明类型有一定差别，所以应该动态的
	//			  获取声明类型来构造CREATE TABLE语句，从而实现对数据库
	//			  类型的抽象。
	//			  举个例子，对于GUID类型(vt=VT_CLSID)，如果使用的是SQLITE
	//			  数据库，对应的声明类型是BLOB；如果使用SQLSever2K5，得到
	//			  的声明类型是binary(16)
	//************************************
	STDMETHOD( GetTypeDecl )( VARTYPE vt, LPTSTR szBuf, INT nBufLen ) PURE;

	//************************************
	// Method:    事务操作的接口函数
	// Remark:	  因为不同数据库类型提供的事务操作语法有一定差别，所以建议
	//			  不要直接用SQL语句进行事务处理，而是使用下面提供的函数
	//************************************
	STDMETHOD( BeginTrans )( VOID ) PURE;
	STDMETHOD( CommitTrans )( VOID ) PURE;
	STDMETHOD( RollbackTrans )( VOID ) PURE;

	//************************************
	// Method:    事务操作的接口函数
	// Remark:	  因为不同数据库类型提供的事务操作语法有一定差别，所以建议
	//			  不要直接用SQL语句进行事务处理，而是使用下面提供的函数
	//************************************
	STDMETHOD( GetTypeDecl )( MsFieldType fieldType, DWORD dwSize, LPTSTR szBuf, INT nBufLen ) PURE;

	STDMETHOD( GetAutoIDDecl )( LPTSTR szBuf, INT nBufLen ) PURE;

	//************************************
	// Method:    BindRealConn
	// Remark:	  显式绑定到物理数据库连接
	//************************************
	STDMETHOD( AttachRealConn )( VOID ) PURE;

	//************************************
	// Method:    BindRealConn
	// Remark:	  显式解除到物理数据库连接的绑定
	//************************************
	STDMETHOD( DetachRealConn )( VOID ) PURE;

	//************************************
	// Method:     GetTableFieldsCount 查询指定的数据表中字段个数
	// Returns:   成功返回S_OK；不成功返回E_FAIL
	// Parameter: szTable[in] 表名
	// Parameter: pdwCount[out] 表中字段的数量
	//************************************
	STDMETHOD( GetTableFieldsCount )( LPCTSTR szTable,DWORD& dwCount ) PURE;

	//************************************
	// Method:     GetTableFieldInfo 查询指定的数据表中字段个数
	// Returns:   成功返回S_OK；不成功返回E_FAIL
	// Parameter: szTable[in] 表名
	// Parameter: index[in]    字段索引值，第一个字段从1开始，逐步累加
	// Parameter: FieldName[out] 字段名称
	// Parameter: FieldType[out]    字段类型
	// Parameter: dwSize[out]    字段类型大小
	//************************************
	STDMETHOD(GetTableFieldInfo) ( LPCTSTR szTable,DWORD index, LPTSTR FieldName, LPTSTR FieldType, DWORD& dwSize  ) PURE;

	//判断此连接是否可用。以方便重连
	STDMETHOD_(BOOL, IsOpen)( VOID ) PURE;
};

MS_DEFINE_IID(IGenericDB, "{50A0CA4D-341F-44F2-86EC-8CC0BA5D7BBD}");

//--------------------------------------------------------------------
//	数据库访问接口的类型，由于不同的数据库操作接口
//--------------------------------------------------------------------
enum DBAPIType
{
	DBAPI_UNKNOWN	= 0,
	DBAPI_SQLITE,
	DBAPI_ADO,
	DBAPI_OTL
};

//--------------------------------------------------------------------
//	IMsDBSelector 此接口通过ROT获取
//--------------------------------------------------------------------
interface IMsDBSelector : public IMSBase
{
	//************************************
	// Method:    获取数据库操作接口
	// Returns:   成功返回S_OK，否则返回E_FAIL
	// Parameter: DBAPIType[in] 数据库访问接口的类型
	// Parameter: ppConn[out] IGenericConn指针
	// Remark:	  可以认为这是一个工厂函数，它会根据数据库访问接口的不同使用
	//			  不同的数据库抽象层实现
	//************************************
	STDMETHOD( GetConn )( DBAPIType type, IGenericDB** ppConn ) PURE;
};

MS_DEFINE_IID(IMsDBSelector, "{812DD829-BFC0-4126-BA6C-F3DA6E625C00}");


// {967D9D9A-BA09-43E6-98AC-F4E03408F8F4}
MS_DEFINE_GUID(CLSID_GenericDB, 
			0x967d9d9a, 0xba09, 0x43e6, 0x98, 0xac, 0xf4, 0xe0, 0x34, 0x8, 0xf8, 0xf4);



//--------------------------------------------------------------------
//	IMsDBManager 此接口通过ROT获取
//--------------------------------------------------------------------
enum MsDBType
{
	DB_INVALID  = 0	,
	DB_CLIENT		,
	DB_CENTER		,
	DB_LOGCENTER	,		//加一种类型，作为日志数据库的连接。
};

interface IMsDBManager : public IMsPlugin
{
	STDMETHOD( GetConn )( MsDBType type, IGenericDB** ppConn, LPCTSTR szConn=NULL ) PURE;

	enum DbgType
	{
		DBG_INVALID		= 0,
		DBG_CONN_POOL,
	};

	STDMETHOD( GetDbgInfo )( DbgType dwDbgType, IMsBuffer** ppDbgInfo, IMsBuffer* pParams=NULL ) PURE;

	//++++++++++++++++++++++++++
	STDMETHOD_( BOOL, IsConnExist )(MsDBType dwDbgType ) PURE;
	STDMETHOD( ReSetConn )(VOID) PURE;
	//++++++++++++++++++++++++++
};

MS_DEFINE_IID(IMsDBManager, "{E7784FDD-CC1A-419A-9E6C-7FE0211A9B94}");

// {4AB1D199-538D-4FF4-810F-A6EE9A18B9D5}
MS_DEFINE_GUID(CLSID_MsDBManager, 
			0x4ab1d199, 0x538d, 0x4ff4, 0x81, 0xf, 0xa6, 0xee, 0x9a, 0x18, 0xb9, 0xd5);

//--------------------------------------------------------------------
//	以RAII来开启和提交/回滚事务
//--------------------------------------------------------------------
class CScopeTrans
{
public:
	CScopeTrans( IGenericDB* pConn )
		:m_pConn(pConn), m_bInTrans(FALSE)
	{
		if( m_pConn )
		{
			HRESULT hr = m_pConn->BeginTrans();
			m_bInTrans = SUCCEEDED(hr) ? TRUE : FALSE;
		}
	}

	~CScopeTrans()
	{
		if( m_pConn && m_bInTrans )
			m_pConn->RollbackTrans();
	}

	HRESULT Begin()
	{
		if( !m_pConn || m_bInTrans )
			return E_FAIL;

		HRESULT hr = m_pConn->BeginTrans();
		m_bInTrans = SUCCEEDED(hr) ? TRUE : FALSE;

		return hr;
	}

	HRESULT Commit()
	{
		if( !m_pConn || !m_bInTrans )
			return E_FAIL;

		HRESULT hr = m_pConn->CommitTrans();
		if( SUCCEEDED(hr) )
			m_bInTrans = FALSE;

		return hr;
	}

	HRESULT Rollback()
	{
		if( !m_pConn || !m_bInTrans )
			return E_FAIL;

		HRESULT hr = m_pConn->RollbackTrans();
		if( SUCCEEDED(hr) )
			m_bInTrans = FALSE;

		return hr;
	}

	BOOL IsInTransaction() { return m_bInTrans; }

private:
	IGenericDB*	m_pConn;
	BOOL		m_bInTrans;	
};

//--------------------------------------------------------------------
//	以RAII来显式绑定绑定到物理数据库连接
//--------------------------------------------------------------------
class CScopeAttach
{
public:
	CScopeAttach( IGenericDB* pConn )
		:m_pConn(pConn), m_bAttached(FALSE)
	{
		if( m_pConn )
		{
			HRESULT hr = m_pConn->AttachRealConn();
			m_bAttached = SUCCEEDED(hr) ? TRUE : FALSE;
		}
	}

	~CScopeAttach()
	{
		if( m_pConn && m_bAttached )
			m_pConn->DetachRealConn();
	}

private:
	IGenericDB*	m_pConn;
	BOOL		m_bAttached;	
};


	// {6C484F8F-B94F-4F59-A6AE-4D90805BF128}
MS_DEFINE_GUID(CLSID_DbAdoConn,
	0x6c484f8f, 0xb94f, 0x4f59, 0xa6, 0xae, 0x4d, 0x90, 0x80, 0x5b, 0xf1, 0x28);



//通过参数自动判断使用 CLSID_DbSqliteDB 还是 CLSID_DbSqliteSafeDB
// {3066762A-BE38-4f9e-8CFB-2037133486B3}
MS_DEFINE_GUID(CLSID_DbSqliteConn, 
	0x3066762a, 0xbe38, 0x4f9e, 0x8c, 0xfb, 0x20, 0x37, 0x13, 0x34, 0x86, 0xb3);



//sqlitedb.dll
// {2A7C48DA-D086-4E36-88DC-2734532FFC7E}
MS_DEFINE_GUID(CLSID_DbSqliteDB,
	0x2a7c48da, 0xd086, 0x4e36, 0x88, 0xdc, 0x27, 0x34, 0x53, 0x2f, 0xfc, 0x7e);


//sqlitesdb.dll
// {D9F63AD9-4A97-40e5-92FD-FA5ED7CA4252}
MS_DEFINE_GUID(CLSID_DbSqliteSafeDB, 
	0xd9f63ad9, 0x4a97, 0x40e5, 0x92, 0xfd, 0xfa, 0x5e, 0xd7, 0xca, 0x42, 0x52);

 };//namespace mscom{;
 };//namespace msdk{;
