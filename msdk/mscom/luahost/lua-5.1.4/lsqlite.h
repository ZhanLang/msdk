#ifndef LSQLITE_H
#define LSQLITE_H

BOX_UD(sqlite3, LuaSQLite);
BOX_UD(sqlite3_stmt, LuaSQLiteStmt);

#define check_db(n) CHECK_UD(LuaSQLite, n)
#define check_stmt(n) CHECK_UD(LuaSQLiteStmt, n)

#endif
