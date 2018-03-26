#pragma once

#include <windows.h>
#include "inc/fastdb.h"
#include "inc/hashtab.h"
#include "inc/ttree.h"
#include "inc/rtree.h"
#include "inc/compiler.h"
#include "inc/server.h"

#include "inc/wwwapi.h"
#include "inc/subsql.h"

#include <stdio.h>
#include <stdarg.h>

#include  <string>
#include	<vector>
using namespace std;

#include "Helper.h"



//void GetExePath(std::string&  strExePath)
//{
//	CHAR path[MAX_PATH] = {0};
//	GetModuleFileNameA(NULL,path,MAX_PATH);
//	CHAR *p = strrchr(path,'\\');
//	if(p)
//	{
//		++p;
//		*p = '\0';
//		strExePath = path;
//	}
//}

struct tag_FieldInfo
{
	string	strFieldName;	
	int		nFieldType;
	int		nSubFieldType;			//当数组时起用.
	string	strRefTableName;		//引用表名
	string  strInverseRefName;		//反向引用名
};

struct tag_FieldInfoValue
{
	WCHAR	szFieldName[MAX_PATH];
	VARIANT var;
};




//通过派生强转,打开一些被保护的成员.
class CMyTableDescriptor: public dbTableDescriptor
{
public:
	unsigned int		GetFixedSize			(){return fixedSize;};
	dbFieldDescriptor*  GetColumns				(){return columns;};
	oid_t				GetTableID				(){return tableId;};
	int4				GetAutoincrementCount	(){return autoincrementCount;};
	dbFieldDescriptor*  GetHashedFields			(){return hashedFields;};
	int4				SetAutoincrementCount	(int4 nValue){return autoincrementCount = nValue;};
	dbFieldDescriptor*	GetIndexedField			(){return indexedFields;};

};

class CMyDBAnyCursor : public dbAnyCursor
{
public:
	CMyDBAnyCursor(dbTableDescriptor& aTable, dbCursorType aType, byte* rec)
		 : dbAnyCursor(aTable,aType,rec)
	{
		
		
	}

	bool gotoFirstNew(){return gotoFirst();};
	bool gotoNextNew(){return gotoNext();};

	oid_t GetCurrID(){return currId;};
		
private:
	
};

//ok了,建表成功了.
class CMyDataBase:  public  dbSubSql
{
public:
	CMyDataBase(INT iInitSize,int iIndexSize): dbSubSql(dbAllAccess,iInitSize,iIndexSize)
	{
		
	}
	BOOL ExecDML(LPCSTR szSql)
	{
		FAILEXIT_FALSE(szSql);
		//直接执行的语句,这里可以不用缓存了.
		string strSql = szSql;
		
		//如果没加分号,则补上一分号
		Helper::CTransStr theTrans;
		BOOL bFind = theTrans.FindCharInStr(strSql.c_str(),';');
		if(FALSE == bFind)
			strSql += "   ;";

		CDBSqlStream stream(strSql);

		bool bRet = this->parse(stream);
		FAILEXIT_FALSE(bRet);

		return TRUE;
	}
	BOOL Exec(LPCSTR szSql,dbAnyCursor** ppCursor)
	{	
		//直接把cursor 对象返回上层使用,这样也不用缓存.
		//缓存的数据生命期,由theCursor自己决定.
		FAILEXIT_FALSE(szSql);
		//直接执行的语句,这里可以不用缓存了.
		string strSql = szSql;
		//如果没加分号,则补上一分号
		Helper::CTransStr theTrans;
		BOOL bFind = theTrans.FindCharInStr(strSql.c_str(),';');
		if(FALSE == bFind)
			strSql += "   ;";

		CDBSqlStream stream(strSql);

		bool bRet = this->parse(stream,ppCursor);
		FAILEXIT_FALSE(bRet);
		FAILEXIT_FALSE(*ppCursor);

		return TRUE;
	}

	dbRecord* getRowNew(oid_t oid) 
	{
		return getRow(oid);
	}


	bool DumpRecordSet(const string& strTableName,dbAnyCursor* pCursor)
	{

		dbFieldDescriptor* columns = NULL;

		CMyTableDescriptor* desc =  (CMyTableDescriptor*)findTableByName(strTableName.c_str());
		if (NULL == desc) 
		{
			//error("No such table in database");
			return false;
		} 

		int n_deleted = pCursor->getNumberOfRecords();
		//这里对查到的结果进行显示.

		CMyDBAnyCursor* pCursorTemp = (CMyDBAnyCursor*)pCursor;

		if (pCursorTemp->gotoFirstNew()) 
		{ 
			dbFieldDescriptor* columnList;
			if (columns != NULL) { 
				columnList = columns;
				dbFieldDescriptor* cc = columns; 
				do { 
					dbFieldDescriptor* next = cc->next;
					dbFieldDescriptor* fd = desc->GetColumns();
					do { 
						if (cc->name == fd->name) { 
							*cc = *fd;
							cc->next = next;
							goto Found;
						}
					} while ((fd = fd->next) != desc->GetColumns());                                
					char buf[256];
					sprintf(buf, "Column '%s' is not found\n", cc->name);
					//error(buf);
Found:
					printf("%s ", fd->name);
					cc = next;
				} while (cc != columns);
			} else {                  
				columnList = desc->GetColumns();
				dbFieldDescriptor* fd = columnList;
				do { 
					printf("%s ", fd->name);
				} while ((fd = fd->next) != columnList);
			}
			printf("\n(");

			dumpRecord((byte*)getRow(pCursorTemp->GetCurrID()), columnList); 
			printf(")");
			while (pCursorTemp->gotoNextNew()) { 
				printf(",\n(");

				dumpRecord((byte*)getRow(pCursorTemp->GetCurrID()), columnList); 
				printf(")");
			}
			printf("\n\t%d records selected\n", pCursorTemp->getNumberOfRecords());
		} else { 
			fprintf(stderr, "No records selected\n");
		}

		return true;
	}


	bool DumpRecordSetNo(const string& strTableName,dbAnyCursor* pCursor)
	{

		dbFieldDescriptor* columns = NULL;

		CMyTableDescriptor* desc =  (CMyTableDescriptor*)findTableByName(strTableName.c_str());
		if (NULL == desc) 
		{
			//error("No such table in database");
			return false;
		} 

		int n_deleted = pCursor->getNumberOfRecords();
		//这里对查到的结果进行显示.

		CMyDBAnyCursor* pCursorTemp = (CMyDBAnyCursor*)pCursor;

		if (pCursorTemp->gotoFirstNew()) 
		{ 
			dbFieldDescriptor* columnList;
			if (columns != NULL) { 
				columnList = columns;
				dbFieldDescriptor* cc = columns; 
				do { 
					dbFieldDescriptor* next = cc->next;
					dbFieldDescriptor* fd = desc->GetColumns();
					do { 
						if (cc->name == fd->name) { 
							*cc = *fd;
							cc->next = next;
							goto Found;
						}
					} while ((fd = fd->next) != desc->GetColumns());                                
					char buf[256];
					sprintf(buf, "Column '%s' is not found\n", cc->name);
					//error(buf);
Found:
					//printf("%s ", fd->name);
					cc = next;
				} while (cc != columns);
			} else {                  
				columnList = desc->GetColumns();
				dbFieldDescriptor* fd = columnList;
				do { 
					//printf("%s ", fd->name);
				} while ((fd = fd->next) != columnList);
			}
			//printf("\n(");

			dumpRecordNo((byte*)getRow(pCursorTemp->GetCurrID()), columnList); 
			//printf(")");
			while (pCursorTemp->gotoNextNew()) { 
				//printf(",\n(");

				dumpRecordNo((byte*)getRow(pCursorTemp->GetCurrID()), columnList); 
				//printf(")");
			}
			printf("\n\t%d records selected\n", pCursorTemp->getNumberOfRecords());
		} else { 
			fprintf(stderr, "No records selected\n");
		}

		return true;
	}



private:
	bool isValidOid(oid_t oid)
	{
		if (oid < dbFirstUserId || oid >= currIndexSize) {
			return false;
		}
		return !(currIndex[oid]&(dbFreeHandleMarker|dbInternalObjectMarker));
	}

	

public:
	//查询来一下
	bool SelectFromTable(const string& strTableName)
	{
		dbFieldDescriptor* columns = NULL;

		CMyTableDescriptor* desc =  (CMyTableDescriptor*)findTableByName(strTableName.c_str());
		if (NULL == desc) 
		{
			//error("No such table in database");
			return false;
		} 
		//构建游标.
		CMyDBAnyCursor cursor(*desc, dbCursorViewOnly, NULL);
		dbDatabaseThreadContext* ctx = threadContext.get();
		ctx->interactive = true;
		ctx->catched = true;

		if (setjmp(ctx->unwind) == 0) 
		{
			//cursor.reset();
			select(&cursor);

			int n_deleted = cursor.getNumberOfRecords();
			//这里对查到的结果进行显示.

			if (cursor.gotoFirstNew()) 
			{ 
				dbFieldDescriptor* columnList;
				if (columns != NULL) { 
					columnList = columns;
					dbFieldDescriptor* cc = columns; 
					do { 
						dbFieldDescriptor* next = cc->next;
						dbFieldDescriptor* fd = desc->GetColumns();
						do { 
							if (cc->name == fd->name) { 
								*cc = *fd;
								cc->next = next;
								goto Found;
							}
						} while ((fd = fd->next) != desc->GetColumns());                                
						char buf[256];
						sprintf(buf, "Column '%s' is not found\n", cc->name);
						//error(buf);
Found:
						printf("%s ", fd->name);
						cc = next;
					} while (cc != columns);
				} else {                  
					columnList = desc->GetColumns();
					dbFieldDescriptor* fd = columnList;
					do { 
						printf("%s ", fd->name);
					} while ((fd = fd->next) != columnList);
				}
				printf("\n(");

				dumpRecord((byte*)getRow(cursor.GetCurrID()), columnList); 
				printf(")");
				while (cursor.gotoNextNew()) { 
					printf(",\n(");

					dumpRecord((byte*)getRow(cursor.GetCurrID()), columnList); 
					printf(")");
				}
				printf("\n\t%d records selected\n", 
					cursor.getNumberOfRecords());
			} else { 
				fprintf(stderr, "No records selected\n");
			}



		}

		return true;
	}

	//查询来一下
	bool SelectFromTable(const string& strTableName,dbQuery& query)
	{
		dbFieldDescriptor* columns = NULL;

		CMyTableDescriptor* desc =  (CMyTableDescriptor*)findTableByName(strTableName.c_str());
		if (NULL == desc) 
		{
			//error("No such table in database");
			return false;
		} 
		//构建游标.
		CMyDBAnyCursor cursor(*desc, dbCursorViewOnly, NULL);
		dbDatabaseThreadContext* ctx = threadContext.get();
		ctx->interactive = true;
		ctx->catched = true;

		if (setjmp(ctx->unwind) == 0) 
		{
			//cursor.reset();
			select(&cursor,query);
			if (query.compileError()) 
			{
				dbExprNodeAllocator::GetInstance()->reset();
				ctx->catched = false;
				return false;
			}

			int n_deleted = cursor.getNumberOfRecords();
			//这里对查到的结果进行显示.

			if (cursor.gotoFirstNew()) 
			{ 
				dbFieldDescriptor* columnList;
				if (columns != NULL) { 
					columnList = columns;
					dbFieldDescriptor* cc = columns; 
					do { 
						dbFieldDescriptor* next = cc->next;
						dbFieldDescriptor* fd = desc->GetColumns();
						do { 
							if (cc->name == fd->name) { 
								*cc = *fd;
								cc->next = next;
								goto Found;
							}
						} while ((fd = fd->next) != desc->GetColumns());                                
						char buf[256];
						sprintf(buf, "Column '%s' is not found\n", cc->name);
						//error(buf);
Found:
						printf("%s ", fd->name);
						cc = next;
					} while (cc != columns);
				} else {                  
					columnList = desc->GetColumns();
					dbFieldDescriptor* fd = columnList;
					do { 
						printf("%s ", fd->name);
					} while ((fd = fd->next) != columnList);
				}
				printf("\n(");

				dumpRecord((byte*)getRow(cursor.GetCurrID()), columnList); 
				printf(")");
				while (cursor.gotoNextNew()) { 
					printf(",\n(");

					dumpRecord((byte*)getRow(cursor.GetCurrID()), columnList); 
					printf(")");
				}
				printf("\n\t%d records selected\n", 
					cursor.getNumberOfRecords());
			} else { 
				fprintf(stderr, "No records selected\n");
			}



		}
	
		return true;
	}

	bool DeleteFromTable(const string& strTableName,dbQuery& query)
	{//删数据终于成功了.

			dbTableDescriptor* desc =  findTableByName(strTableName.c_str());
			if (NULL == desc) 
			{
				//error("No such table in database");
				return false;
			} 
			//构建游标.
			CMyDBAnyCursor cursor(*desc, dbCursorForUpdate, NULL);
			dbDatabaseThreadContext* ctx = threadContext.get();
			ctx->interactive = true;
			ctx->catched = true;

			if (setjmp(ctx->unwind) == 0) 
			{

				//cursor.reset();
				select(&cursor,query);
				if (query.compileError()) 
				{
					dbExprNodeAllocator::GetInstance()->reset();
					ctx->catched = false;
					return false;
				}
				
				int n_deleted = cursor.getNumberOfRecords();
				cursor.removeAllSelected();
				printf("\n\t%d records deleted\n", n_deleted);

			} 
			else { 
				/*if (query.mutexLocked) { 
					query.mutexLocked = false;
					query.mutex.unlock();
				}*/
			}

			ctx->catched = false;
		

		return true;

	}


	bool InsertIntoTable(const string& strTableName,dbList* pValueList)
	{
		 dbTableDescriptor* desc =  findTableByName(strTableName.c_str());
		 if(NULL == desc)
		 {
			 //error("No such table in database");
			 return false;
		 }
		beginTransaction(dbExclusiveLock);
		insertRecord(pValueList, (CMyTableDescriptor*)desc);

		precommit();
		
		return true;
	}

	bool CreateNewTable(const string& strTableName,const vector<tag_FieldInfo>& vFieldsSet)
	{
		char* name = (char*)strTableName.c_str();
		int varyingLength = (int)strlen(name)+1;

		static const struct 
		{ 
			int size;
			int alignment;
		} typeDesc[] = 
		{ 
			{ sizeof(bool), sizeof(bool) }, 
			{ sizeof(int1), sizeof(int1) }, 
			{ sizeof(int2), sizeof(int2) }, 
			{ sizeof(int4), sizeof(int4) }, 
			{ sizeof(db_int8), sizeof(db_int8) }, 
			{ sizeof(real4), sizeof(real4) }, 
			{ sizeof(real8), sizeof(real8) }, 
			{ sizeof(dbVarying), 4 }, 
			{ sizeof(oid_t), sizeof(oid_t) }, 
			{ sizeof(dbVarying), 4 },
			{0}, // tpMethodBool,
			{0}, // tpMethodInt1,
			{0}, // tpMethodInt2,
			{0}, // tpMethodInt4,
			{0}, // tpMethodInt8,
			{0}, // tpMethodReal4,
			{0}, // tpMethodReal8,
			{0}, // tpMethodString,
			{0}, // tpMethodReference,
			{0}, // tpStructure,
			{0}, // tpRawBinary,
			{0}, // tpStdString,
			{ sizeof(rectangle), sizeof(coord_t) }, // tpRectangle,
			{ sizeof(dbVarying), 4 },  // tpWString,
			{0}, // tpStdWString,
			{0}, // tpMethodWString,
			{0} // tpUnknown
		};

		const int maxFields = 256;
		tableField fields[maxFields];
		int nFields = 0;
		int nColumns = 0;
		//取每个字段.
		for(; nFields < vFieldsSet.size(); ++nFields)
		{ 
			if (nFields+1 == maxFields) 
			{ 
				//error("Too many fields");
				break;
			}
			/*if (!expect("field name", tkn_ident)) { 
				break;
			}*/
			const tag_FieldInfo* pFieldInfo = &(vFieldsSet[nFields]);
			string strFieldName = pFieldInfo->strFieldName;
			int nFieldType = pFieldInfo->nFieldType;
			int nSubFieldType = pFieldInfo->nSubFieldType;
			string strRefTableName = pFieldInfo->strRefTableName;
			string strInverseRefName = pFieldInfo->strInverseRefName;

			

			int nameLen = (int)strFieldName.length() + 1;
			fields[nFields].name = new char[nameLen];
			strcpy(fields[nFields].name, strFieldName.c_str());

			varyingLength += nameLen + 2;

			int type = nFieldType;
			fields[nFields].type = type;
			if (type == dbField::tpUnknown) 
			{ 
				break;
			}
			nColumns += 1;
			if (type == dbField::tpArray)
			{
				if (nFields+1 == maxFields)
				{ 
					//error("Too many fields");
					break;
				}
				fields[nFields].name = new char[nameLen+2];
				sprintf(fields[nFields].name, "%s[]", fields[nFields-1].name);
				varyingLength += nameLen+2+2;
				type = nSubFieldType;
				if (type == dbField::tpUnknown) 
				{ 
					break;
				}
				if (type == dbField::tpArray) 
				{ 
					//error("Arrays of arrays are not supported by CLI");
					break;
				}
				if (type == dbField::tpReference) 
				{
					fields[nFields].refTableName = (char*)strRefTableName.c_str();
					varyingLength += (int)strlen(strRefTableName.c_str());
					if (strInverseRefName.empty() != true)
					{ 
						fields[nFields-1].inverseRefName = (char*)strInverseRefName.c_str();
						varyingLength += (int)strlen(strInverseRefName.c_str());
					}                   
				}
				fields[nFields++].type = type;
			} 
			else if (type == dbField::tpReference) 
			{ 
				fields[nFields-1].refTableName = (char*)strRefTableName.c_str();
				varyingLength += (int)strlen(strRefTableName.c_str());
				if (strInverseRefName.empty() != true)
				{ 
					fields[nFields-1].inverseRefName = (char*)strInverseRefName.c_str();
					varyingLength += (int)strlen(strInverseRefName.c_str());
				}                      
			}
		}

		//如果字段初始化结束,则开始建表.
		this->beginTransaction(dbExclusiveLock);
		dbTableDescriptor* oldDesc = this->findTableByName(name);
		if (oldDesc != NULL) 
		{//找到旧表,
			//表己存在,不可再新建了.
			//error("Table already exists");
			return false;
		} 

		dbTable* table; 
		oid_t oid;
		//建新表.
		oid = this->allocateRow(dbMetaTableId, 
			sizeof(dbTable) + sizeof(dbField)*nFields + varyingLength);
		table = (dbTable*)this->getRow(oid);    

		int offs = sizeof(dbTable) + sizeof(dbField)*nFields;
		table->name.offs = offs;
		table->name.size = (nat4)strlen(name)+1;
		strcpy((char*)table + offs, name);
		offs += table->name.size;
		size_t size = sizeof(dbRecord);
		table->fields.offs = sizeof(dbTable);
		dbField* field = (dbField*)((char*)table + table->fields.offs);
		offs -= sizeof(dbTable);
		bool arrayComponent = false;

		for (int i = 0; i < nFields; i++) 
		{ 
			field->name.offs = offs;
			field->name.size = (nat4)strlen(fields[i].name) + 1;
			strcpy((char*)field + offs, fields[i].name);
			offs += field->name.size;

			field->tableName.offs = offs;
			if (fields[i].refTableName)
			{ 
				field->tableName.size = (nat4)strlen(fields[i].refTableName) + 1;
				strcpy((char*)field + offs, fields[i].refTableName);
				offs += field->tableName.size;
			}
			else 
			{ 
				field->tableName.size = 1;
				*((char*)field + offs++) = '\0';
			}

			field->inverse.offs = offs;
			if (fields[i].inverseRefName) 
			{ 
				field->inverse.size = (nat4)strlen(fields[i].inverseRefName) + 1;
				strcpy((char*)field + offs, fields[i].inverseRefName);
				offs += field->inverse.size;
			}
			else 
			{ 
				field->inverse.size = 1;
				*((char*)field + offs++) = '\0';
			}

			field->flags = 0;
			field->type = fields[i].type;
			field->size = typeDesc[fields[i].type].size;
			if (!arrayComponent) 
			{ 
				size = DOALIGN(size, typeDesc[fields[i].type].alignment);
				field->offset = (int)size;
				size += field->size;
			}
			else 
			{ 
				field->offset = 0;
			}
			field->hashTable = 0;
			field->tTree = 0;
			arrayComponent = field->type == dbField::tpArray; 
			field += 1;
			offs -= sizeof(dbField);
		}
		table->fields.size = nFields;
		table->fixedSize = (nat4)size;
		table->nRows = 0;
		table->nColumns = nColumns;
		table->firstRow = 0;
		table->lastRow = 0;

		//建新表
		this->linkTable(new dbTableDescriptor(this, table), oid);

		this->completeDescriptorsInitialization();

		return true;
	}

	private:



		void dumpRecord(byte* base, dbFieldDescriptor* first)
		{
			int i, n;
			byte* elem;
			dbFieldDescriptor* fd = first;
			do { 
				if (fd != first) { 
					printf(", ");
				}
				switch (fd->type) { 
		  case dbField::tpBool:
			  printf("%s", *(bool*)(base + fd->dbsOffs) 
				  ? "true" : "false");
			  continue;
		  case dbField::tpInt1:
			  printf("%d", *(int1*)(base + fd->dbsOffs)); 
			  continue;       
		  case dbField::tpInt2:
			  printf("%d", *(int2*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpInt4:
			  printf("%d", *(int4*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpInt8:
			  printf(INT8_FORMAT, *(db_int8*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpReal4:
			  printf("%f", *(real4*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpReal8:
			  printf("%f", *(real8*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpString:
			  printf("'%s'", (char*)base+((dbVarying*)(base+fd->dbsOffs))->offs);
			  continue;
		  case dbField::tpWString:
			  printf("'%ls'", (wchar_t*)((char*)base+((dbVarying*)(base+fd->dbsOffs))->offs));
			  continue;
		  case dbField::tpReference:
			  printf("#%lx", (unsigned long)*(oid_t*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpRectangle:
			  {
				  int i, sep = '(';
				  rectangle& r = *(rectangle*)(base + fd->dbsOffs);
				  for (i = 0; i < rectangle::dim*2; i++) { 
					  printf("%c%f", sep, (double)r.boundary[i]);
					  sep = ',';
				  }
				  printf(")");
			  }
			  continue;
		  case dbField::tpRawBinary:
			  n = (int)fd->dbsSize;
			  elem = base + fd->dbsOffs;
			  printf("(");
			  for (i = 0; i < n; i++) { 
				  if (i != 0) { 
					  printf(", ");
				  }
				  printf("%02x", *elem++);
			  }
			  printf(")");
			  continue;
		  case dbField::tpArray:
			  n = ((dbVarying*)(base + fd->dbsOffs))->size;
			  elem = base + ((dbVarying*)(base + fd->dbsOffs))->offs;
			  printf("(");
			  for (i = 0; i < n; i++) { 
				  if (i != 0) { 
					  printf(", ");
				  }
				  dumpRecord(elem, fd->components);
				  elem += fd->components->dbsSize;
			  }
			  printf(")");
			  continue;
		  case dbField::tpStructure:
			  if (dateFormat != NULL 
				  && fd->components->next == fd->components 
				  && strcmp(fd->components->name, "stamp") == 0) 
			  { 
				  char buf[64];
				  printf(((dbDateTime*)(base + fd->components->dbsOffs))->asString(buf, sizeof buf, dateFormat));
				  continue;
			  }
			  printf("(");
			  dumpRecord(base, fd->components);
			  printf(")");
				}
			} while ((fd = fd->next) != first);
		}

		void dumpRecordNo(byte* base, dbFieldDescriptor* first)
		{
			return;
			int i, n;
			byte* elem;
			dbFieldDescriptor* fd = first;
			do { 
				if (fd != first) { 
					printf(", ");
				}
				switch (fd->type) { 
		  case dbField::tpBool:
			  printf("%s", *(bool*)(base + fd->dbsOffs) 
				  ? "true" : "false");
			  continue;
		  case dbField::tpInt1:
			  printf("%d", *(int1*)(base + fd->dbsOffs)); 
			  continue;       
		  case dbField::tpInt2:
			  printf("%d", *(int2*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpInt4:
			  printf("%d", *(int4*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpInt8:
			  printf(INT8_FORMAT, *(db_int8*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpReal4:
			  printf("%f", *(real4*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpReal8:
			  printf("%f", *(real8*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpString:
			  printf("'%s'", (char*)base+((dbVarying*)(base+fd->dbsOffs))->offs);
			  continue;
		  case dbField::tpWString:
			  printf("'%ls'", (wchar_t*)((char*)base+((dbVarying*)(base+fd->dbsOffs))->offs));
			  continue;
		  case dbField::tpReference:
			  printf("#%lx", (unsigned long)*(oid_t*)(base + fd->dbsOffs)); 
			  continue;
		  case dbField::tpRectangle:
			  {
				  int i, sep = '(';
				  rectangle& r = *(rectangle*)(base + fd->dbsOffs);
				  for (i = 0; i < rectangle::dim*2; i++) { 
					  printf("%c%f", sep, (double)r.boundary[i]);
					  sep = ',';
				  }
				  printf(")");
			  }
			  continue;
		  case dbField::tpRawBinary:
			  n = (int)fd->dbsSize;
			  elem = base + fd->dbsOffs;
			  printf("(");
			  for (i = 0; i < n; i++) { 
				  if (i != 0) { 
					  printf(", ");
				  }
				  printf("%02x", *elem++);
			  }
			  printf(")");
			  continue;
		  case dbField::tpArray:
			  n = ((dbVarying*)(base + fd->dbsOffs))->size;
			  elem = base + ((dbVarying*)(base + fd->dbsOffs))->offs;
			  printf("(");
			  for (i = 0; i < n; i++) { 
				  if (i != 0) { 
					  printf(", ");
				  }
				  dumpRecord(elem, fd->components);
				  elem += fd->components->dbsSize;
			  }
			  printf(")");
			  continue;
		  case dbField::tpStructure:
			  if (dateFormat != NULL 
				  && fd->components->next == fd->components 
				  && strcmp(fd->components->name, "stamp") == 0) 
			  { 
				  char buf[64];
				  printf(((dbDateTime*)(base + fd->components->dbsOffs))->asString(buf, sizeof buf, dateFormat));
				  continue;
			  }
			  printf("(");
			  dumpRecord(base, fd->components);
			  printf(")");
				}
			} while ((fd = fd->next) != first);
		}






		int initializeRecordFields(dbList* node, byte* dst, int offs, 
			dbFieldDescriptor* first)
		{
			dbFieldDescriptor* fd = first;
			dbList* component;
			byte* elem;
			coord_t* coord;
			int len, elemOffs, elemSize;

			do { 
				if (node->type == dbList::nString && fd->type != dbField::tpString) { 
					char* s = node->sval;
					long  ival;
					switch (fd->type) {
			  case dbField::tpBool:
				  *(bool*)(dst+fd->dbsOffs) = *s == '1' || *s == 't' || *s == 'T';
				  break;
			  case dbField::tpInt1:
				  if (sscanf(s, "%ld", &ival) != 1) { 
					  return -1;
				  }
				  *(int1*)(dst+fd->dbsOffs) = (int1)ival;
			  case dbField::tpInt2:
				  if (sscanf(s, "%ld", &ival) != 1) { 
					  return -1;
				  }
				  *(int2*)(dst+fd->dbsOffs) = (int2)ival;
			  case dbField::tpInt4:
				  if (sscanf(s, "%ld", &ival) != 1) { 
					  return -1;
				  }
				  *(int4*)(dst+fd->dbsOffs) = (int4)ival;
			  case dbField::tpInt8:
				  if (sscanf(s, "%ld", &ival) != 1) { 
					  return -1;
				  }
				  *(db_int8*)(dst+fd->dbsOffs) = ival;
				  break;
			  case dbField::tpReal4:
				  if (sscanf(s, "%f", (real4*)(dst+fd->dbsOffs)) != 1) { 
					  return -1;
				  }
				  break;
			  case dbField::tpReal8:
				  if (sscanf(s, "%lf", (real8*)(dst+fd->dbsOffs)) != 1) { 
					  return -1;
				  }
				  break;
			  case dbField::tpWString:
				  ((dbVarying*)(dst+fd->dbsOffs))->offs = offs;
				  len = mbstowcs(NULL, node->sval, 0);
				  ((dbVarying*)(dst+fd->dbsOffs))->size = len+1;
				  mbstowcs((wchar_t*)(dst + offs), node->sval, len);
				  *((wchar_t*)(dst + offs) + len) = '\0';
				  offs += len*sizeof(wchar_t);
				  break;
					}
#ifdef AUTOINCREMENT_SUPPORT
				} else if (node->type == dbList::nAutoinc) {        
					if (fd->type == dbField::tpInt4) {
						*(int4*)(dst+fd->dbsOffs) = ((CMyTableDescriptor*)fd->defTable)->GetAutoincrementCount();//fd->defTable->autoincrementCount;
					} else { 
						return -1;
					}
#endif
				} else { 
					switch (fd->type) { 
			  case dbField::tpBool:
				  *(bool*)(dst+fd->dbsOffs) = node->bval;
				  break;
			  case dbField::tpInt1:
				  *(int1*)(dst+fd->dbsOffs) = (int1)node->ival;
				  break;
			  case dbField::tpInt2:
				  *(int2*)(dst+fd->dbsOffs) = (int2)node->ival;
				  break;
			  case dbField::tpInt4:
				  *(int4*)(dst+fd->dbsOffs) = (int4)node->ival;
				  break;
			  case dbField::tpInt8:
				  *(db_int8*)(dst+fd->dbsOffs) = node->ival;
				  break;
			  case dbField::tpReal4:
				  *(real4*)(dst+fd->dbsOffs) = (real4)node->fval;
				  break;
			  case dbField::tpReal8:
				  *(real8*)(dst+fd->dbsOffs) = node->fval;
				  break;
			  case dbField::tpReference:
				  if (isValidOid((oid_t)node->ival)) {                
					  *(oid_t*)(dst+fd->dbsOffs) = (oid_t)node->ival;
				  } else { 
					  return -1;
				  }
				  break;
			  case dbField::tpString:
				  ((dbVarying*)(dst+fd->dbsOffs))->offs = offs;
				  len = (int)strlen(node->sval) + 1;
				  ((dbVarying*)(dst+fd->dbsOffs))->size = len;
				  memcpy(dst + offs, node->sval, len);
				  offs += len;
				  break;
			  case dbField::tpRawBinary:
				  len = node->aggregate.nComponents;
				  component = node->aggregate.components;
				  elem = dst + fd->dbsOffs;
				  while (--len >= 0) { 
					  *elem++ = (byte)component->ival;
					  component = component->next;
				  }
				  break;
			  case dbField::tpRectangle:
				  len = node->aggregate.nComponents;
				  component = node->aggregate.components;
				  coord = (coord_t*)(dst + fd->dbsOffs);
				  assert(len == rectangle::dim*2);                    
				  while (--len >= 0) {
					  *coord++ = (component->type == dbList::nInteger) 
						  ? (coord_t)component->ival : (coord_t)component->fval;
					  component = component->next;
				  }
				  break;
			  case dbField::tpArray:
				  len = node->aggregate.nComponents;
				  elem = (byte*)DOALIGN(size_t(dst) + offs, fd->components->alignment);
				  offs = (int)(elem - dst);
				  ((dbVarying*)(dst+fd->dbsOffs))->offs = offs;
				  ((dbVarying*)(dst+fd->dbsOffs))->size = len;
				  elemSize = (int)fd->components->dbsSize;  
				  elemOffs = len*elemSize;
				  offs += elemOffs;
				  component = node->aggregate.components;
				  while (--len >= 0) { 
					  elemOffs = initializeRecordFields(component, elem, elemOffs, 
						  fd->components);
					  elemOffs -= elemSize;
					  elem += elemSize;
					  component = component->next;
				  }
				  offs += elemOffs;
				  break;
			  case dbField::tpStructure:
				  offs = initializeRecordFields(node->aggregate.components, 
					  dst, offs, fd->components);
					}
				}
				node = node->next;
			} while ((fd = fd->next) != first);

			return offs;
		}

		int calculateRecordSize(dbList* node, int offs, 
			dbFieldDescriptor* first)
		{
			dbFieldDescriptor* fd = first;
			do { 
				if (node == NULL) { 
					return -1;
				}
				if (fd->type == dbField::tpArray) { 
					if (node->type != dbList::nTuple) { 
						return -1;
					}
					int nElems = node->aggregate.nComponents;
					offs = (int)(DOALIGN(offs, fd->components->alignment) 
						+ nElems*fd->components->dbsSize);
					if (fd->attr & dbFieldDescriptor::HasArrayComponents) {
						dbList* component = node->aggregate.components;
						while (--nElems >= 0) { 
							int d = calculateRecordSize(component,offs,fd->components);
							if (d < 0) return d;
							offs = d;
							component = component->next;
						}
					} 
				} else if (fd->type == dbField::tpString) { 
					if (node->type != dbList::nString) { 
						return -1;
					}
					offs += (int)strlen(node->sval) + 1;
				} else if (fd->type == dbField::tpRectangle) {
					if (node->type != dbList::nTuple) { 
						return -1;
					}
					int nCoords = node->aggregate.nComponents;
					if (nCoords != rectangle::dim*2) {
						return -1;
					}
					dbList* component = node->aggregate.components;
					while (--nCoords >= 0) {
						if (component->type != dbList::nInteger && component->type != dbList::nReal) {
							return -1;
						}
						component = component->next;
					}
				} else if (fd->type == dbField::tpRawBinary) { 
					if (node->type != dbList::nTuple) { 
						return -1;
					}
					int nElems = node->aggregate.nComponents;
					dbList* component = node->aggregate.components;
					if (size_t(nElems) > fd->dbsSize) { 
						return -1;
					}
					while (--nElems >= 0) { 
						if (component->type != dbList::nInteger
							|| (component->ival & ~0xFF) != 0) 
						{ 
							return -1;
						}
						component = component->next;
					}
#ifdef AUTOINCREMENT_SUPPORT
				} else if (node->type == dbList::nAutoinc) {        
					if (fd->type != dbField::tpInt4) {
						return -1;
					}
#endif
				} else { 
					if (!((node->type == dbList::nBool && fd->type == dbField::tpBool)
						|| (node->type == dbList::nInteger 
						&& (fd->type == dbField::tpInt1
						|| fd->type == dbField::tpInt2
						|| fd->type == dbField::tpInt4
						|| fd->type == dbField::tpInt8
						|| fd->type == dbField::tpReference))
						|| (node->type == dbList::nReal 
						&& (fd->type == dbField::tpReal4
						|| fd->type == dbField::tpReal8))
						|| (node->type == dbList::nTuple 
						&& fd->type == dbField::tpStructure)))
					{
						return -1;
					}
					if (fd->attr & dbFieldDescriptor::HasArrayComponents) {
						int d = calculateRecordSize(node->aggregate.components,
							offs, fd->components);
						if (d < 0) return d;
						offs = d;
					}
				}
				node = node->next;
			} while ((fd = fd->next) != first);
			return offs;    
		}




		bool insertRecord(dbList* list, CMyTableDescriptor* desc)
		{
			int size = calculateRecordSize(list, (int)desc->GetFixedSize(), desc->GetColumns());
			if (size < 0) { 
				//error("Incompatible types in insert statement");
				return false;
			}
			oid_t oid = allocateRow(desc->GetTableID(), size);
			byte* dst = (byte*)getRow(oid);    
			dbTable* table = (dbTable*)getRow(desc->GetTableID());
#ifdef AUTOINCREMENT_SUPPORT
			desc->SetAutoincrementCount(table->count);
#endif
			initializeRecordFields(list, dst, (int)desc->GetFixedSize(), desc->GetColumns());

			int nRows = table->nRows;
			dbFieldDescriptor* fd;
			for (fd = desc->GetHashedFields(); fd != NULL; fd = fd->nextHashedField){
				dbHashTable::insert(this, fd, oid, nRows);
			}
			for (fd = desc->GetIndexedField(); fd != NULL; fd = fd->nextIndexedField) { 
				if (fd->type == dbField::tpRectangle) { 
					dbRtree::insert(this, fd->tTree, oid, fd->dbsOffs);
				} else { 
					dbTtree::insert(this, fd->tTree, oid, fd->type, (int)fd->dbsSize, fd->_comparator, fd->dbsOffs);
				}
			}    
			return true;
		}

private:
		 char*    dateFormat;


	
};

//class CMyDataBase: public dbDatabase
//{
//public:
//	bool isValidOid(oid_t oid)
//	{
//		if (oid < dbFirstUserId || oid >= currIndexSize) {
//			return false;
//		}
//		return !(currIndex[oid]&(dbFreeHandleMarker|dbInternalObjectMarker));
//	}
//
//public:
//	bool SelectFromTable(const string& strTableName,CMyDBAnyCursor** ppCursor,dbQuery & theQuery)
//	{
//		dbFieldDescriptor* columns = NULL;
//
//		CMyTableDescriptor* desc =  (CMyTableDescriptor*)findTableByName(strTableName.c_str());
//		if (NULL == desc) 
//		{
//			//error("No such table in database");
//			return false;
//		} 
//		//构建游标.
//		dbDatabaseThreadContext* ctx = threadContext.get();
//
//		*ppCursor = new CMyDBAnyCursor(*desc, dbCursorViewOnly, NULL);
//		if(NULL == *ppCursor)
//			return false;
//		
//		CMyDBAnyCursor* pTempCursor = *ppCursor;
//
//		ctx->interactive = true;
//		ctx->catched = true;
//
//		if (setjmp(ctx->unwind) == 0) 
//		{
//			//cursor.reset();
//			select(pTempCursor,theQuery);
//			if (theQuery.compileError()) 
//			{
//				dbExprNodeAllocator::GetInstance()->reset();
//				ctx->catched = false;
//				return false;
//			}
//
//			int nCount = pTempCursor->getNumberOfRecords();
//		}
//
//		return true;
//	}
//
//	//查询来一下
//	bool SelectFromTable(const string& strTableName)
//	{
//		dbFieldDescriptor* columns = NULL;
//
//		CMyTableDescriptor* desc =  (CMyTableDescriptor*)findTableByName(strTableName.c_str());
//		if (NULL == desc) 
//		{
//			//error("No such table in database");
//			return false;
//		} 
//		//构建游标.
//		CMyDBAnyCursor cursor(*desc, dbCursorViewOnly, NULL);
//		dbDatabaseThreadContext* ctx = threadContext.get();
//		ctx->interactive = true;
//		ctx->catched = true;
//
//		if (setjmp(ctx->unwind) == 0) 
//		{
//			//cursor.reset();
//			select(&cursor);
//
//			int n_deleted = cursor.getNumberOfRecords();
//			//这里对查到的结果进行显示.
//
//			if (cursor.gotoFirstNew()) 
//			{ 
//				dbFieldDescriptor* columnList;
//				if (columns != NULL) { 
//					columnList = columns;
//					dbFieldDescriptor* cc = columns; 
//					do { 
//						dbFieldDescriptor* next = cc->next;
//						dbFieldDescriptor* fd = desc->GetColumns();
//						do { 
//							if (cc->name == fd->name) { 
//								*cc = *fd;
//								cc->next = next;
//								goto Found;
//							}
//						} while ((fd = fd->next) != desc->GetColumns());                                
//						char buf[256];
//						sprintf(buf, "Column '%s' is not found\n", cc->name);
//						//error(buf);
//Found:
//						printf("%s ", fd->name);
//						cc = next;
//					} while (cc != columns);
//				} else {                  
//					columnList = desc->GetColumns();
//					dbFieldDescriptor* fd = columnList;
//					do { 
//						printf("%s ", fd->name);
//					} while ((fd = fd->next) != columnList);
//				}
//				printf("\n(");
//
//				dumpRecord((byte*)getRow(cursor.GetCurrID()), columnList); 
//				printf(")");
//				while (cursor.gotoNextNew()) { 
//					printf(",\n(");
//
//					dumpRecord((byte*)getRow(cursor.GetCurrID()), columnList); 
//					printf(")");
//				}
//				printf("\n\t%d records selected\n", 
//					cursor.getNumberOfRecords());
//			} else { 
//				fprintf(stderr, "No records selected\n");
//			}
//
//
//
//		}
//
//		return true;
//	}
//
//	//查询来一下
//	bool SelectFromTable(const string& strTableName,dbQuery& query)
//	{
//		dbFieldDescriptor* columns = NULL;
//
//		CMyTableDescriptor* desc =  (CMyTableDescriptor*)findTableByName(strTableName.c_str());
//		if (NULL == desc) 
//		{
//			//error("No such table in database");
//			return false;
//		} 
//		//构建游标.
//		CMyDBAnyCursor cursor(*desc, dbCursorViewOnly, NULL);
//		dbDatabaseThreadContext* ctx = threadContext.get();
//		ctx->interactive = true;
//		ctx->catched = true;
//
//		if (setjmp(ctx->unwind) == 0) 
//		{
//			//cursor.reset();
//			select(&cursor,query);
//			if (query.compileError()) 
//			{
//				dbExprNodeAllocator::GetInstance()->reset();
//				ctx->catched = false;
//				return false;
//			}
//
//			int n_deleted = cursor.getNumberOfRecords();
//			//这里对查到的结果进行显示.
//
//			if (cursor.gotoFirstNew()) 
//			{ 
//				dbFieldDescriptor* columnList;
//				if (columns != NULL) { 
//					columnList = columns;
//					dbFieldDescriptor* cc = columns; 
//					do { 
//						dbFieldDescriptor* next = cc->next;
//						dbFieldDescriptor* fd = desc->GetColumns();
//						do { 
//							if (cc->name == fd->name) { 
//								*cc = *fd;
//								cc->next = next;
//								goto Found;
//							}
//						} while ((fd = fd->next) != desc->GetColumns());                                
//						char buf[256];
//						sprintf(buf, "Column '%s' is not found\n", cc->name);
//						//error(buf);
//Found:
//						printf("%s ", fd->name);
//						cc = next;
//					} while (cc != columns);
//				} else {                  
//					columnList = desc->GetColumns();
//					dbFieldDescriptor* fd = columnList;
//					do { 
//						printf("%s ", fd->name);
//					} while ((fd = fd->next) != columnList);
//				}
//				//printf("\n(");
//
//				dumpRecord((byte*)getRow(cursor.GetCurrID()), columnList); 
//				//printf(")");
//				while (cursor.gotoNextNew()) { 
//					//printf(",\n(");
//
//					dumpRecord((byte*)getRow(cursor.GetCurrID()), columnList); 
//					//printf(")");
//				}
//				printf("\n\t%d records selected\n", 
//					cursor.getNumberOfRecords());
//			} else { 
//				fprintf(stderr, "No records selected\n");
//			}
//
//
//
//		}
//	
//		return true;
//	}
//
//	bool DeleteFromTable(const string& strTableName,dbQuery& query)
//	{//删数据终于成功了.
//
//			dbTableDescriptor* desc =  findTableByName(strTableName.c_str());
//			if (NULL == desc) 
//			{
//				//error("No such table in database");
//				return false;
//			} 
//			//构建游标.
//			CMyDBAnyCursor cursor(*desc, dbCursorForUpdate, NULL);
//			dbDatabaseThreadContext* ctx = threadContext.get();
//			ctx->interactive = true;
//			ctx->catched = true;
//
//			if (setjmp(ctx->unwind) == 0) 
//			{
//
//				//cursor.reset();
//				select(&cursor,query);
//				if (query.compileError()) 
//				{
//					dbExprNodeAllocator::GetInstance()->reset();
//					ctx->catched = false;
//					return false;
//				}
//				
//				int n_deleted = cursor.getNumberOfRecords();
//				cursor.removeAllSelected();
//				printf("\n\t%d records deleted\n", n_deleted);
//
//			} 
//			else { 
//				/*if (query.mutexLocked) { 
//					query.mutexLocked = false;
//					query.mutex.unlock();
//				}*/
//			}
//
//			ctx->catched = false;
//		
//
//		return true;
//
//	}
//
//
//	bool InsertIntoTable(const string& strTableName,dbList* pValueList)
//	{
//		 dbTableDescriptor* desc =  findTableByName(strTableName.c_str());
//		 if(NULL == desc)
//		 {
//			 //error("No such table in database");
//			 return false;
//		 }
//		beginTransaction(dbExclusiveLock);
//		insertRecord(pValueList, (CMyTableDescriptor*)desc);
//
//		precommit();
//		
//		return true;
//	}
//
//	bool CreateNewTable(const string& strTableName,const vector<tag_FieldInfo>& vFieldsSet)
//	{
//		char* name = (char*)strTableName.c_str();
//		int varyingLength = (int)strlen(name)+1;
//
//		static const struct 
//		{ 
//			int size;
//			int alignment;
//		} typeDesc[] = 
//		{ 
//			{ sizeof(bool), sizeof(bool) }, 
//			{ sizeof(int1), sizeof(int1) }, 
//			{ sizeof(int2), sizeof(int2) }, 
//			{ sizeof(int4), sizeof(int4) }, 
//			{ sizeof(db_int8), sizeof(db_int8) }, 
//			{ sizeof(real4), sizeof(real4) }, 
//			{ sizeof(real8), sizeof(real8) }, 
//			{ sizeof(dbVarying), 4 }, 
//			{ sizeof(oid_t), sizeof(oid_t) }, 
//			{ sizeof(dbVarying), 4 },
//			{0}, // tpMethodBool,
//			{0}, // tpMethodInt1,
//			{0}, // tpMethodInt2,
//			{0}, // tpMethodInt4,
//			{0}, // tpMethodInt8,
//			{0}, // tpMethodReal4,
//			{0}, // tpMethodReal8,
//			{0}, // tpMethodString,
//			{0}, // tpMethodReference,
//			{0}, // tpStructure,
//			{0}, // tpRawBinary,
//			{0}, // tpStdString,
//			{ sizeof(rectangle), sizeof(coord_t) }, // tpRectangle,
//			{ sizeof(dbVarying), 4 },  // tpWString,
//			{0}, // tpStdWString,
//			{0}, // tpMethodWString,
//			{0} // tpUnknown
//		};
//
//		const int maxFields = 256;
//		tableField fields[maxFields];
//		UINT nFields = 0;
//		int nColumns = 0;
//		//取每个字段.
//		for(; nFields < vFieldsSet.size(); ++nFields)
//		{ 
//			if (nFields+1 == maxFields) 
//			{ 
//				//error("Too many fields");
//				break;
//			}
//			/*if (!expect("field name", tkn_ident)) { 
//				break;
//			}*/
//			const tag_FieldInfo* pFieldInfo = &(vFieldsSet[nFields]);
//			string strFieldName = pFieldInfo->strFieldName;
//			int nFieldType = pFieldInfo->nFieldType;
//			int nSubFieldType = pFieldInfo->nSubFieldType;
//			string strRefTableName = pFieldInfo->strRefTableName;
//			string strInverseRefName = pFieldInfo->strInverseRefName;
//
//			
//
//			int nameLen = (int)strFieldName.length() + 1;
//			fields[nFields].name = new char[nameLen];
//			strcpy(fields[nFields].name, strFieldName.c_str());
//
//			varyingLength += nameLen + 2;
//
//			int type = nFieldType;
//			fields[nFields].type = type;
//			if (type == dbField::tpUnknown) 
//			{ 
//				break;
//			}
//			nColumns += 1;
//			if (type == dbField::tpArray)
//			{
//				if (nFields+1 == maxFields)
//				{ 
//					//error("Too many fields");
//					break;
//				}
//				fields[nFields].name = new char[nameLen+2];
//				sprintf(fields[nFields].name, "%s[]", fields[nFields-1].name);
//				varyingLength += nameLen+2+2;
//				type = nSubFieldType;
//				if (type == dbField::tpUnknown) 
//				{ 
//					break;
//				}
//				if (type == dbField::tpArray) 
//				{ 
//					//error("Arrays of arrays are not supported by CLI");
//					break;
//				}
//				if (type == dbField::tpReference) 
//				{
//					fields[nFields].refTableName = (char*)strRefTableName.c_str();
//					varyingLength += (int)strlen(strRefTableName.c_str());
//					if (strInverseRefName.empty() != true)
//					{ 
//						fields[nFields-1].inverseRefName = (char*)strInverseRefName.c_str();
//						varyingLength += (int)strlen(strInverseRefName.c_str());
//					}                   
//				}
//				fields[nFields++].type = type;
//			} 
//			else if (type == dbField::tpReference) 
//			{ 
//				fields[nFields-1].refTableName = (char*)strRefTableName.c_str();
//				varyingLength += (int)strlen(strRefTableName.c_str());
//				if (strInverseRefName.empty() != true)
//				{ 
//					fields[nFields-1].inverseRefName = (char*)strInverseRefName.c_str();
//					varyingLength += (int)strlen(strInverseRefName.c_str());
//				}                      
//			}
//		}
//
//		//如果字段初始化结束,则开始建表.
//		this->beginTransaction(dbExclusiveLock);
//		dbTableDescriptor* oldDesc = this->findTableByName(name);
//		if (oldDesc != NULL) 
//		{//找到旧表,
//			//表己存在,不可再新建了.
//			//error("Table already exists");
//			return false;
//		} 
//
//		dbTable* table; 
//		oid_t oid;
//		//建新表.
//		oid = this->allocateRow(dbMetaTableId, 
//			sizeof(dbTable) + sizeof(dbField)*nFields + varyingLength);
//		table = (dbTable*)this->getRow(oid);    
//
//		int offs = sizeof(dbTable) + sizeof(dbField)*nFields;
//		table->name.offs = offs;
//		table->name.size = (nat4)strlen(name)+1;
//		strcpy((char*)table + offs, name);
//		offs += table->name.size;
//		size_t size = sizeof(dbRecord);
//		table->fields.offs = sizeof(dbTable);
//		dbField* field = (dbField*)((char*)table + table->fields.offs);
//		offs -= sizeof(dbTable);
//		bool arrayComponent = false;
//
//		for (UINT i = 0; i < nFields; i++) 
//		{ 
//			field->name.offs = offs;
//			field->name.size = (nat4)strlen(fields[i].name) + 1;
//			strcpy((char*)field + offs, fields[i].name);
//			offs += field->name.size;
//
//			field->tableName.offs = offs;
//			if (fields[i].refTableName)
//			{ 
//				field->tableName.size = (nat4)strlen(fields[i].refTableName) + 1;
//				strcpy((char*)field + offs, fields[i].refTableName);
//				offs += field->tableName.size;
//			}
//			else 
//			{ 
//				field->tableName.size = 1;
//				*((char*)field + offs++) = '\0';
//			}
//
//			field->inverse.offs = offs;
//			if (fields[i].inverseRefName) 
//			{ 
//				field->inverse.size = (nat4)strlen(fields[i].inverseRefName) + 1;
//				strcpy((char*)field + offs, fields[i].inverseRefName);
//				offs += field->inverse.size;
//			}
//			else 
//			{ 
//				field->inverse.size = 1;
//				*((char*)field + offs++) = '\0';
//			}
//
//			field->flags = 0;
//			field->type = fields[i].type;
//			field->size = typeDesc[fields[i].type].size;
//			if (!arrayComponent) 
//			{ 
//				size = DOALIGN(size, typeDesc[fields[i].type].alignment);
//				field->offset = (int)size;
//				size += field->size;
//			}
//			else 
//			{ 
//				field->offset = 0;
//			}
//			field->hashTable = 0;
//			field->tTree = 0;
//			arrayComponent = field->type == dbField::tpArray; 
//			field += 1;
//			offs -= sizeof(dbField);
//		}
//		table->fields.size = nFields;
//		table->fixedSize = (nat4)size;
//		table->nRows = 0;
//		table->nColumns = nColumns;
//		table->firstRow = 0;
//		table->lastRow = 0;
//
//		//建新表
//		this->linkTable(new dbTableDescriptor(this, table), oid);
//
//		this->completeDescriptorsInitialization();
//
//		return true;
//	}
//
//	private:
//
//
//
//		void dumpRecord(byte* base, dbFieldDescriptor* first)
//		{
//			return;
//			int i, n;
//			byte* elem;
//			dbFieldDescriptor* fd = first;
//			do { 
//				if (fd != first) { 
//					printf(", ");
//				}
//				switch (fd->type) { 
//		  case dbField::tpBool:
//			  printf("%s", *(bool*)(base + fd->dbsOffs) 
//				  ? "true" : "false");
//			  continue;
//		  case dbField::tpInt1:
//			  printf("%d", *(int1*)(base + fd->dbsOffs)); 
//			  continue;       
//		  case dbField::tpInt2:
//			  printf("%d", *(int2*)(base + fd->dbsOffs)); 
//			  continue;
//		  case dbField::tpInt4:
//			  printf("%d", *(int4*)(base + fd->dbsOffs)); 
//			  continue;
//		  case dbField::tpInt8:
//			  printf(INT8_FORMAT, *(db_int8*)(base + fd->dbsOffs)); 
//			  continue;
//		  case dbField::tpReal4:
//			  printf("%f", *(real4*)(base + fd->dbsOffs)); 
//			  continue;
//		  case dbField::tpReal8:
//			  printf("%f", *(real8*)(base + fd->dbsOffs)); 
//			  continue;
//		  case dbField::tpString:
//			  printf("'%s'", (char*)base+((dbVarying*)(base+fd->dbsOffs))->offs);
//			  continue;
//		  case dbField::tpWString:
//			  printf("'%ls'", (wchar_t*)((char*)base+((dbVarying*)(base+fd->dbsOffs))->offs));
//			  continue;
//		  case dbField::tpReference:
//			  printf("#%lx", (unsigned long)*(oid_t*)(base + fd->dbsOffs)); 
//			  continue;
//		  case dbField::tpRectangle:
//			  {
//				  int i, sep = '(';
//				  rectangle& r = *(rectangle*)(base + fd->dbsOffs);
//				  for (i = 0; i < rectangle::dim*2; i++) { 
//					  printf("%c%f", sep, (double)r.boundary[i]);
//					  sep = ',';
//				  }
//				  printf(")");
//			  }
//			  continue;
//		  case dbField::tpRawBinary:
//			  n = (int)fd->dbsSize;
//			  elem = base + fd->dbsOffs;
//			  printf("(");
//			  for (i = 0; i < n; i++) { 
//				  if (i != 0) { 
//					  printf(", ");
//				  }
//				  printf("%02x", *elem++);
//			  }
//			  printf(")");
//			  continue;
//		  case dbField::tpArray:
//			  n = ((dbVarying*)(base + fd->dbsOffs))->size;
//			  elem = base + ((dbVarying*)(base + fd->dbsOffs))->offs;
//			  printf("(");
//			  for (i = 0; i < n; i++) { 
//				  if (i != 0) { 
//					  printf(", ");
//				  }
//				  dumpRecord(elem, fd->components);
//				  elem += fd->components->dbsSize;
//			  }
//			  printf(")");
//			  continue;
//		  case dbField::tpStructure:
//			  if (dateFormat != NULL 
//				  && fd->components->next == fd->components 
//				  && strcmp(fd->components->name, "stamp") == 0) 
//			  { 
//				  char buf[64];
//				  printf(((dbDateTime*)(base + fd->components->dbsOffs))->asString(buf, sizeof buf, dateFormat));
//				  continue;
//			  }
//			  printf("(");
//			  dumpRecord(base, fd->components);
//			  printf(")");
//				}
//			} while ((fd = fd->next) != first);
//		}
//
//
//
//
//		int initializeRecordFields(dbList* node, byte* dst, int offs, 
//			dbFieldDescriptor* first)
//		{
//			dbFieldDescriptor* fd = first;
//			dbList* component;
//			byte* elem;
//			coord_t* coord;
//			int len, elemOffs, elemSize;
//
//			do { 
//				if (node->type == dbList::nString && fd->type != dbField::tpString) { 
//					char* s = node->sval;
//					long  ival;
//					switch (fd->type) {
//			  case dbField::tpBool:
//				  *(bool*)(dst+fd->dbsOffs) = *s == '1' || *s == 't' || *s == 'T';
//				  break;
//			  case dbField::tpInt1:
//				  if (sscanf(s, "%ld", &ival) != 1) { 
//					  return -1;
//				  }
//				  *(int1*)(dst+fd->dbsOffs) = (int1)ival;
//			  case dbField::tpInt2:
//				  if (sscanf(s, "%ld", &ival) != 1) { 
//					  return -1;
//				  }
//				  *(int2*)(dst+fd->dbsOffs) = (int2)ival;
//			  case dbField::tpInt4:
//				  if (sscanf(s, "%ld", &ival) != 1) { 
//					  return -1;
//				  }
//				  *(int4*)(dst+fd->dbsOffs) = (int4)ival;
//			  case dbField::tpInt8:
//				  if (sscanf(s, "%ld", &ival) != 1) { 
//					  return -1;
//				  }
//				  *(db_int8*)(dst+fd->dbsOffs) = ival;
//				  break;
//			  case dbField::tpReal4:
//				  if (sscanf(s, "%f", (real4*)(dst+fd->dbsOffs)) != 1) { 
//					  return -1;
//				  }
//				  break;
//			  case dbField::tpReal8:
//				  if (sscanf(s, "%lf", (real8*)(dst+fd->dbsOffs)) != 1) { 
//					  return -1;
//				  }
//				  break;
//			  case dbField::tpWString:
//				  ((dbVarying*)(dst+fd->dbsOffs))->offs = offs;
//				  len = mbstowcs(NULL, node->sval, 0);
//				  ((dbVarying*)(dst+fd->dbsOffs))->size = len+1;
//				  mbstowcs((wchar_t*)(dst + offs), node->sval, len);
//				  *((wchar_t*)(dst + offs) + len) = '\0';
//				  offs += len*sizeof(wchar_t);
//				  break;
//					}
//#ifdef AUTOINCREMENT_SUPPORT
//				} else if (node->type == dbList::nAutoinc) {        
//					if (fd->type == dbField::tpInt4) {
//						*(int4*)(dst+fd->dbsOffs) = ((CMyTableDescriptor*)fd->defTable)->GetAutoincrementCount();//fd->defTable->autoincrementCount;
//					} else { 
//						return -1;
//					}
//#endif
//				} else { 
//					switch (fd->type) { 
//			  case dbField::tpBool:
//				  *(bool*)(dst+fd->dbsOffs) = node->bval;
//				  break;
//			  case dbField::tpInt1:
//				  *(int1*)(dst+fd->dbsOffs) = (int1)node->ival;
//				  break;
//			  case dbField::tpInt2:
//				  *(int2*)(dst+fd->dbsOffs) = (int2)node->ival;
//				  break;
//			  case dbField::tpInt4:
//				  *(int4*)(dst+fd->dbsOffs) = (int4)node->ival;
//				  break;
//			  case dbField::tpInt8:
//				  *(db_int8*)(dst+fd->dbsOffs) = node->ival;
//				  break;
//			  case dbField::tpReal4:
//				  *(real4*)(dst+fd->dbsOffs) = (real4)node->fval;
//				  break;
//			  case dbField::tpReal8:
//				  *(real8*)(dst+fd->dbsOffs) = node->fval;
//				  break;
//			  case dbField::tpReference:
//				  if (isValidOid((oid_t)node->ival)) {                
//					  *(oid_t*)(dst+fd->dbsOffs) = (oid_t)node->ival;
//				  } else { 
//					  return -1;
//				  }
//				  break;
//			  case dbField::tpString:
//				  ((dbVarying*)(dst+fd->dbsOffs))->offs = offs;
//				  len = (int)strlen(node->sval) + 1;
//				  ((dbVarying*)(dst+fd->dbsOffs))->size = len;
//				  memcpy(dst + offs, node->sval, len);
//				  offs += len;
//				  break;
//			  case dbField::tpRawBinary:
//				  len = node->aggregate.nComponents;
//				  component = node->aggregate.components;
//				  elem = dst + fd->dbsOffs;
//				  while (--len >= 0) { 
//					  *elem++ = (byte)component->ival;
//					  component = component->next;
//				  }
//				  break;
//			  case dbField::tpRectangle:
//				  len = node->aggregate.nComponents;
//				  component = node->aggregate.components;
//				  coord = (coord_t*)(dst + fd->dbsOffs);
//				  assert(len == rectangle::dim*2);                    
//				  while (--len >= 0) {
//					  *coord++ = (component->type == dbList::nInteger) 
//						  ? (coord_t)component->ival : (coord_t)component->fval;
//					  component = component->next;
//				  }
//				  break;
//			  case dbField::tpArray:
//				  len = node->aggregate.nComponents;
//				  elem = (byte*)DOALIGN(size_t(dst) + offs, fd->components->alignment);
//				  offs = (int)(elem - dst);
//				  ((dbVarying*)(dst+fd->dbsOffs))->offs = offs;
//				  ((dbVarying*)(dst+fd->dbsOffs))->size = len;
//				  elemSize = (int)fd->components->dbsSize;  
//				  elemOffs = len*elemSize;
//				  offs += elemOffs;
//				  component = node->aggregate.components;
//				  while (--len >= 0) { 
//					  elemOffs = initializeRecordFields(component, elem, elemOffs, 
//						  fd->components);
//					  elemOffs -= elemSize;
//					  elem += elemSize;
//					  component = component->next;
//				  }
//				  offs += elemOffs;
//				  break;
//			  case dbField::tpStructure:
//				  offs = initializeRecordFields(node->aggregate.components, 
//					  dst, offs, fd->components);
//					}
//				}
//				node = node->next;
//			} while ((fd = fd->next) != first);
//
//			return offs;
//		}
//
//		int calculateRecordSize(dbList* node, int offs, 
//			dbFieldDescriptor* first)
//		{
//			dbFieldDescriptor* fd = first;
//			do { 
//				if (node == NULL) { 
//					return -1;
//				}
//				if (fd->type == dbField::tpArray) { 
//					if (node->type != dbList::nTuple) { 
//						return -1;
//					}
//					int nElems = node->aggregate.nComponents;
//					offs = (int)(DOALIGN(offs, fd->components->alignment) 
//						+ nElems*fd->components->dbsSize);
//					if (fd->attr & dbFieldDescriptor::HasArrayComponents) {
//						dbList* component = node->aggregate.components;
//						while (--nElems >= 0) { 
//							int d = calculateRecordSize(component,offs,fd->components);
//							if (d < 0) return d;
//							offs = d;
//							component = component->next;
//						}
//					} 
//				} else if (fd->type == dbField::tpString) { 
//					if (node->type != dbList::nString) { 
//						return -1;
//					}
//					offs += (int)strlen(node->sval) + 1;
//				} else if (fd->type == dbField::tpRectangle) {
//					if (node->type != dbList::nTuple) { 
//						return -1;
//					}
//					int nCoords = node->aggregate.nComponents;
//					if (nCoords != rectangle::dim*2) {
//						return -1;
//					}
//					dbList* component = node->aggregate.components;
//					while (--nCoords >= 0) {
//						if (component->type != dbList::nInteger && component->type != dbList::nReal) {
//							return -1;
//						}
//						component = component->next;
//					}
//				} else if (fd->type == dbField::tpRawBinary) { 
//					if (node->type != dbList::nTuple) { 
//						return -1;
//					}
//					int nElems = node->aggregate.nComponents;
//					dbList* component = node->aggregate.components;
//					if (size_t(nElems) > fd->dbsSize) { 
//						return -1;
//					}
//					while (--nElems >= 0) { 
//						if (component->type != dbList::nInteger
//							|| (component->ival & ~0xFF) != 0) 
//						{ 
//							return -1;
//						}
//						component = component->next;
//					}
//#ifdef AUTOINCREMENT_SUPPORT
//				} else if (node->type == dbList::nAutoinc) {        
//					if (fd->type != dbField::tpInt4) {
//						return -1;
//					}
//#endif
//				} else { 
//					if (!((node->type == dbList::nBool && fd->type == dbField::tpBool)
//						|| (node->type == dbList::nInteger 
//						&& (fd->type == dbField::tpInt1
//						|| fd->type == dbField::tpInt2
//						|| fd->type == dbField::tpInt4
//						|| fd->type == dbField::tpInt8
//						|| fd->type == dbField::tpReference))
//						|| (node->type == dbList::nReal 
//						&& (fd->type == dbField::tpReal4
//						|| fd->type == dbField::tpReal8))
//						|| (node->type == dbList::nTuple 
//						&& fd->type == dbField::tpStructure)))
//					{
//						return -1;
//					}
//					if (fd->attr & dbFieldDescriptor::HasArrayComponents) {
//						int d = calculateRecordSize(node->aggregate.components,
//							offs, fd->components);
//						if (d < 0) return d;
//						offs = d;
//					}
//				}
//				node = node->next;
//			} while ((fd = fd->next) != first);
//			return offs;    
//		}
//
//
//
//
//		bool insertRecord(dbList* list, CMyTableDescriptor* desc)
//		{
//			int size = calculateRecordSize(list, (int)desc->GetFixedSize(), desc->GetColumns());
//			if (size < 0) { 
//				//error("Incompatible types in insert statement");
//				return false;
//			}
//			oid_t oid = allocateRow(desc->GetTableID(), size);
//			byte* dst = (byte*)getRow(oid);    
//			dbTable* table = (dbTable*)getRow(desc->GetTableID());
//#ifdef AUTOINCREMENT_SUPPORT
//			desc->SetAutoincrementCount(table->count);
//#endif
//			initializeRecordFields(list, dst, (int)desc->GetFixedSize(), desc->GetColumns());
//
//			int nRows = table->nRows;
//			dbFieldDescriptor* fd;
//			for (fd = desc->GetHashedFields(); fd != NULL; fd = fd->nextHashedField){
//				dbHashTable::insert(this, fd, oid, nRows);
//			}
//			for (fd = desc->GetIndexedField(); fd != NULL; fd = fd->nextIndexedField) { 
//				if (fd->type == dbField::tpRectangle) { 
//					dbRtree::insert(this, fd->tTree, oid, fd->dbsOffs);
//				} else { 
//					dbTtree::insert(this, fd->tTree, oid, fd->type, (int)fd->dbsSize, fd->_comparator, fd->dbsOffs);
//				}
//			}    
//			return true;
//		}
//
//private:
//		 char*    dateFormat;
//
//
//	
//};

class CMyDBServer : public dbServer
{
public:

	CMyDBServer(dbDatabase* db,
		char const* serverURL, 
		int optimalNumberOfThreads = 8,  
		int connectionQueueLen = 64
		): dbServer(
		db
		,serverURL
		,optimalNumberOfThreads
		,connectionQueueLen
		)
	{

	}


	BOOL ReInitSocket(
		const string& strLocalHost
		,int optimalNumberOfThreads = 8
		,int connectionQueueLen = 64
		)
	{
		//FAILEXIT_FALSE(NULL != globalAcceptSock);
		if(NULL != localAcceptSock)
		{
			delete localAcceptSock;
			localAcceptSock = NULL;
		}
		if(NULL != globalAcceptSock)
		{
			delete globalAcceptSock;
			globalAcceptSock = NULL;
		}

		if(this->URL)
			delete[] this->URL;

		this->URL = new char[strlen(strLocalHost.c_str())+1];
		strcpy(URL, strLocalHost.c_str());
		globalAcceptSock = socket_t::create_global(strLocalHost.c_str(), connectionQueueLen);
		if (!globalAcceptSock->is_ok()) 
		{ 
			//globalAcceptSock->get_error_text(buf, sizeof buf);
			//dbTrace("Failed to create global socket: %s\n", buf);
			delete globalAcceptSock;
			globalAcceptSock = NULL;
		}
		localAcceptSock = socket_t::create_local(strLocalHost.c_str(), connectionQueueLen);
		if (!localAcceptSock->is_ok()) 
		{ 
			//localAcceptSock->get_error_text(buf, sizeof buf);
			//dbTrace("Failed to create local socket: %s\n", buf);
			delete localAcceptSock;
			localAcceptSock = NULL;
		}

		return TRUE;
	}

	BOOL IsCreateGlobalSockSuccessed()
	{
		if(globalAcceptSock) 
			return TRUE; 
		return FALSE;
	};

	BOOL IsCreateLocalSockSuccessed()
	{
		if(localAcceptSock)
			return TRUE;
		return FALSE;
	}

};
