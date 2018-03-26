/* 
 * Copyright (c) 2010 Scott Vokes <vokes.s@gmail.com>
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *  
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <lua.h>
#include <lauxlib.h>
#include <sqlite3.h>
#include "luawrap.h"
#include "lsqlite.h"

#define SI static int
#define DEBUG 0
#define TRACE   if (DEBUG) printf


SI ver(lua_State *L) { lua_pushstring(L, sqlite3_libversion()); return 1; }
SI ver_num(lua_State *L) { lua_pushinteger(L, sqlite3_libversion_number()); return 1; }


SI pushres(lua_State *L, int res) {
        const char *s;
        switch (res) {
#define RS(str) lua_pushstring(L, str); return 1;   /* result: success */
        /* Success cases */
        case SQLITE_OK: RS("ok");           /* Successful result */
        case SQLITE_ROW: RS("row");         /* sqlite3_step() has another row ready */
        case SQLITE_DONE: RS("done");       /* sqlite3_step() has finished executing */
#undef RS
#define RF(str) s = str; break                      /* result: failure */
        /* Failure cases */
        case SQLITE_ERROR: RF("error");     /* SQL error or missing database */
        case SQLITE_INTERNAL: RF("internal");/* Internal logic error in SQLite */
        case SQLITE_PERM: RF("perm");       /* Access permission denied */
        case SQLITE_ABORT: RF("abort");     /* Callback routine requested an abort */
        case SQLITE_BUSY: RF("busy");       /* The database file is locked */
        case SQLITE_LOCKED: RF("locked");   /* A table in the database is locked */
        case SQLITE_NOMEM: RF("nomem");     /* A malloc() failed */
        case SQLITE_READONLY: RF("readonly");/* Attempt to write a readonly database */
        case SQLITE_INTERRUPT: RF("interrupt"); /* Operation terminated by sqlite3_interrupt()*/
        case SQLITE_IOERR: RF("ioerr");     /* Some kind of disk I/O error occurred */
        case SQLITE_CORRUPT: RF("corrupt"); /* The database disk image is malformed */
        case SQLITE_NOTFOUND: RF("notfound");/* NOT USED. Table or record not found */
        case SQLITE_FULL: RF("full");       /* Insertion failed because database is full */
        case SQLITE_CANTOPEN: RF("cantopen");/* Unable to open the database file */
        case SQLITE_PROTOCOL: RF("protocol");/* NOT USED. Database lock protocol error */
        case SQLITE_EMPTY: RF("empty");     /* Database is empty */
        case SQLITE_SCHEMA: RF("schema");   /* The database schema changed */
        case SQLITE_TOOBIG: RF("toobig");   /* String or BLOB exceeds size limit */
        case SQLITE_CONSTRAINT: RF("constraint"); /* Abort due to constraint violation */
        case SQLITE_MISMATCH: RF("mismatch");/* Data type mismatch */
        case SQLITE_MISUSE: RF("misuse");   /* Library used incorrectly */
        case SQLITE_NOLFS: RF("nolfs");     /* Uses OS features not supported on host */
        case SQLITE_AUTH: RF("auth");       /* Authorization denied */
        case SQLITE_FORMAT: RF("format");   /* Auxiliary database format error */
        case SQLITE_RANGE: RF("range");     /* 2nd parameter to sqlite3_bind out of range */
        case SQLITE_NOTADB: RF("notadb");   /* File opened that is not a database file */
#undef RF
        default: s = "unknown"; break;
        }
        lua_pushboolean(L, 0);
        lua_pushstring(L, s);
        return 2;
}


/************/
/* Database */
/************/

static int open_db(lua_State *L, const char *fn) {
        sqlite3 *db;
        LuaSQLite *ldb;
        int res = sqlite3_open(fn, &db);
        if (res == SQLITE_OK) {
                ldb = (LuaSQLite *)lua_newuserdata(L, sizeof(LuaSQLite));
                ldb->v = db;
                luaL_getmetatable(L, "LuaSQLite");
                lua_setmetatable(L, -2);
                return 1;
        } else {
                LERROR(sqlite3_errmsg(db));
                return 0;
        }
}

SI open(lua_State *L) {
        size_t len;
        const char *fn = luaL_optlstring(L, 1, ":memory:", &len);
        return open_db(L, fn);
}

SI open_memory(lua_State *L) {
        return open_db(L, ":memory:");
}

SI close(lua_State *L) {
        LuaSQLite *ldb = check_db(1);
        return pushres(L, sqlite3_close(ldb->v));
}

SI interrupt(lua_State *L) {
        LuaSQLite *ldb = check_db(1);
        sqlite3_interrupt(ldb->v);
        return 0;
}
SI db_tostring(lua_State *L) {
        LuaSQLite *ldb = check_db(1);
        lua_pushfstring(L, "SQLite db: %p", ldb->v);
        return 1;
}

SI db_gc(lua_State *L) {
        LuaSQLite *ldb = check_db(1);
        sqlite3_close(ldb->v);
        return 0;
}

static void dump(lua_State *L) {
        int i;
        for (i=1; i<=lua_gettop(L); i++)
                printf(" -- %d -> %s (%s)\n",
                    i, lua_tostring(L, i), lua_typename(L, lua_type(L, i)));

}

SI changes(lua_State *L) {
        LuaSQLite *ldb = check_db(1);
        lua_pushinteger(L, sqlite3_changes(ldb->v));
        return 1;
}

SI last_insert_rowid(lua_State *L) {
        LuaSQLite *ldb = check_db(1);
        lua_pushinteger(L, sqlite3_last_insert_rowid(ldb->v));
        return 1;
}


/********/
/* Exec */
/********/

static void push_cell_table(lua_State *L, int len, char** cells) {
        int i;
        const char *cell;
        lua_createtable(L, len, 0);
        for (i=0; i < len; i++) {
                cell = cells[i];
                if (cell == NULL)
                        lua_pushnil(L);
                else
                        lua_pushstring(L, cell);
                lua_pushinteger(L, i + 1);
                lua_insert(L, -2);
                lua_settable(L, -3);
        }
}

/* Wrapper to call optional Lua callback. Called once per row. */
SI exec_cb(void *ls, int ncols, char** colText, char** colNames) {
        lua_State *L = (lua_State *) ls;
        int ok;
        if (DEBUG) printf("in exec_cb, top = %d\n", lua_gettop(L));
        assert(lua_type(L, 3) == LUA_TFUNCTION);
        lua_pushvalue(L, 3);    /* dup */
        push_cell_table(L, ncols, colNames);
        push_cell_table(L, ncols, colText);
        ok = lua_pcall(L, 2, 0, 0);
        if (DEBUG) printf("ok? %d\n", ok);
        return ok != 0;         /* 1 -> error, returns abort */
}

SI exec(lua_State *L) {
        LuaSQLite *ldb = check_db(1);
        const char *sql = luaL_checklstring(L, 2, NULL);
        char *err;
        int top = lua_gettop(L);
        int (*cb)(void *,int,char**,char**) = NULL;
        int res;
        if (top == 3 && lua_type(L, 3) == LUA_TFUNCTION) cb = exec_cb;

        res = sqlite3_exec(ldb->v, sql, cb, (void*)L, &err);
        if (res != SQLITE_OK) LERROR(err);
        return pushres(L, res);
}


/*************/
/* get_table */
/*************/

SI get_table(lua_State *L) {
        LuaSQLite *ldb = check_db(1);
        const char *sql = luaL_checkstring(L, 2);
        lua_pop(L, 2);
        char **qres;
        int nrow, ncol;
        char *err;
        int row, res;
        char **cells;
        res = sqlite3_get_table(ldb->v, sql, &qres, &nrow, &ncol, &err);
        if (res != SQLITE_OK) {
                sqlite3_free_table(qres);
                lua_pushstring(L, err);
                lua_error(L);
        }

        /* Make a table of rows, each of which is an array of column strings. */
        lua_createtable(L, nrow, 0);
        TRACE("nrow=%d, ncol=%d\n", nrow, ncol);
        for (row=0; row <= nrow; row++) {
                cells = qres + (ncol * row);
                push_cell_table(L, ncol, cells);

                if (row == 0)
                        lua_pushstring(L, "columns");
                else
                        lua_pushinteger(L, row);
                lua_insert(L, -2);
                lua_settable(L, -3);
        }

        sqlite3_free_table(qres);
        pushres(L, res);
        lua_insert(L, -2);
        return 2;
}


/**********/
/* Errors */
/**********/

SI errcode(lua_State *L) {
        LuaSQLite *ldb = check_db(1);
        lua_pushinteger(L, sqlite3_errcode(ldb->v));
        return 1;
}

SI errmsg(lua_State *L) {
        LuaSQLite *ldb = check_db(1);
        lua_pushstring(L, sqlite3_errmsg(ldb->v));
        return 1;
}


/**************/
/* Statements */
/**************/

SI prepare(lua_State *L) {
        LuaSQLite *ldb = check_db(1);
        size_t len;
        sqlite3_stmt *stmt;
        LuaSQLiteStmt *lstmt;
        const char *tail;
        const char *sql = luaL_checklstring(L, 2, &len);
        int res = sqlite3_prepare_v2(ldb->v, sql, len, &stmt, &tail);

        if (res == SQLITE_OK) {
                lstmt = (LuaSQLiteStmt *)lua_newuserdata(L, sizeof(LuaSQLiteStmt *));
                lstmt->v = stmt;
                luaL_getmetatable(L, "LuaSQLiteStmt");
                lua_setmetatable(L, -2);
                lua_pushstring(L, tail);
                return 2;
        } else {
                LERROR(sqlite3_errmsg(ldb->v));
                return 0;
        }
}

SI stmt_step(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        return pushres(L, sqlite3_step(s->v));
}

SI stmt_gc(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        return pushres(L, sqlite3_finalize(s->v));
}

SI stmt_reset(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        return pushres(L, sqlite3_reset(s->v));
}

SI stmt_tostring(lua_State *L) {
        LuaSQLiteStmt *lstmt = check_stmt(1);
        lua_pushfstring(L, "SQLite stmt: %p", lstmt->v);
        return 1;
}

SI stmt_sql(lua_State *L) {
        LuaSQLiteStmt *lstmt = check_stmt(1);
        lua_pushstring(L, sqlite3_sql(lstmt->v));
        return 1;
}

/***********************/
/* Columns and binding */
/***********************/

/*     int sqlite3_bind_blob(sqlite3_stmt*, int, const void*, int n, void(*)(void*)); */
/*     int sqlite3_bind_int64(sqlite3_stmt*, int, sqlite3_int64); */
/*     int sqlite3_bind_null(sqlite3_stmt*, int); */
/*     int sqlite3_bind_text16(sqlite3_stmt*, int, const void*, int, void(*)(void*)); */
/*     int sqlite3_bind_value(sqlite3_stmt*, int, const sqlite3_value*); */
/*     int sqlite3_bind_zeroblob(sqlite3_stmt*, int, int n); */

SI bind_param_idx(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        const char *name = luaL_checkstring(L, 2);
        return sqlite3_bind_parameter_index(s->v, name);
}

SI param_idx(lua_State *L, sqlite3_stmt *stmt, int i) {
        const char* key;
        if (lua_isnumber(L, i)) {
                return lua_tonumber(L, i);
        } else {
                key = luaL_checkstring(L, i);
                return sqlite3_bind_parameter_index(stmt, key);
        }
}

SI bind_dtype(sqlite3_stmt *s, lua_State *L, int idx) {
        int t;
        const char *v;
        size_t len;

        t = lua_type(L, -1);
        TRACE("Binding idx %d to type %d\n", idx, t);
        switch (t) {
        case LUA_TNIL:
                return pushres(L, sqlite3_bind_null(s, idx));
        case LUA_TBOOLEAN:
                return pushres(L, sqlite3_bind_int(s, idx, lua_tointeger(L, -1)));
        case LUA_TNUMBER:
                return pushres(L, sqlite3_bind_double(s, idx, lua_tonumber(L, -1)));
        case LUA_TSTRING:
                v = luaL_checklstring(L, -1, &len);
                return pushres(L, sqlite3_bind_text(s, idx, v,
                        len, SQLITE_TRANSIENT));
        default: LERROR("Cannot bind type");
        }
        return 0;

}

/* Take a k=v table and call bind(":" .. k, v) for each.
   Take a { v1, v2, v3 } table and call bind([i], v) for each. */
SI bind_table(lua_State *L, LuaSQLiteStmt *s) {
        size_t len = lua_objlen(L, 2);
        int i, idx;
        const char* vartag = ":"; /* TODO make this an optional argument? */

        if (len == 0) {         /* k=v table */
                lua_pushnil(L); /* start at first key */
                while (lua_next(L, 2) != 0) {
                        lua_pushlstring(L, vartag, 1);
                        lua_pushvalue(L, -3);
                        lua_concat(L, 2); /* prepend : to key */
                        idx = sqlite3_bind_parameter_index(s->v, lua_tostring(L, -1));
                        if (idx == 0) {
                                lua_pushfstring(L, "Invalid statement parameter '%s'",
                                    lua_tostring(L, -1));
                                lua_error(L);
                        }
                        TRACE("Index is %d\n", idx);
                        lua_pop(L, 1);
                        bind_dtype(s->v, L, idx);
                        lua_pop(L, 1);
                        if (DEBUG) printf("* %s (%s) = %s (%s)\n",
                            lua_tostring(L, -2),
                            lua_typename(L, lua_type(L, -2)),
                            lua_tostring(L, -1),
                            lua_typename(L, lua_type(L, -1)));
                        lua_pop(L, 1);
                }
        } else {                /* array */
                for (i=0; i < len; i++) {
                        lua_pushinteger(L, i + 1);
                        lua_gettable(L, 2);
                        bind_dtype(s->v, L, i + 1);
                        lua_pop(L, 1);
                }
        }
        return 1;
}

SI stmt_bind(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int i;
        if (lua_type(L, 2) == LUA_TTABLE) return bind_table(L, s);

        i = param_idx(L, s->v, 2);
        return bind_dtype(s->v, L, i);
}

SI bind_param_count(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        return sqlite3_bind_parameter_count(s->v);
}

SI bind_clear(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        return pushres(L, sqlite3_clear_bindings(s->v));
}

SI bind_double(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int i = param_idx(L, s->v, 2);
        double v = luaL_checknumber(L, 3);
        return pushres(L, sqlite3_bind_double(s->v, i, v));
}

SI bind_int(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int i = param_idx(L, s->v, 2);
        int v = luaL_checkinteger(L, 3);
        return pushres(L, sqlite3_bind_int(s->v, i, v));
}

SI bind_null(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int i = param_idx(L, s->v, 2);
        return pushres(L, sqlite3_bind_null(s->v, i));
}

SI bind_text(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int i = param_idx(L, s->v, 2);
        size_t len;
        const char* v = luaL_checklstring(L, 3, &len);
        return pushres(L, sqlite3_bind_text(s->v, i, v, len, SQLITE_TRANSIENT));
}


/* const void *sqlite3_column_blob(sqlite3_stmt*, int iCol); */
/* int sqlite3_column_bytes(sqlite3_stmt*, int iCol); */
/* int sqlite3_column_bytes16(sqlite3_stmt*, int iCol); */
/* sqlite3_int64 sqlite3_column_int64(sqlite3_stmt*, int iCol); */
/* const void *sqlite3_column_text16(sqlite3_stmt*, int iCol); */
/* sqlite3_value *sqlite3_column_value(sqlite3_stmt*, int iCol); */

static int sqltype_of_char(lua_State *L, char c) {
        switch (c) {
        case 'i': return SQLITE_INTEGER;
        case 'f': return SQLITE_FLOAT;
        case 't': case 's': return SQLITE_TEXT;
        case 'b': return SQLITE_BLOB;
        case 'n': return SQLITE_NULL;
        default:
                lua_pushfstring(L, "Invalid column tag '%c' -- must be in 'iftb'", c);
                lua_error(L);
        }
        return -1;
}


static void push_col(lua_State *L, sqlite3_stmt *stmt, int i, int t) {
        double d;
        int n;
        const char *c;
        size_t tlen;
        switch(t) {
        case SQLITE_INTEGER:
                n = sqlite3_column_int(stmt, i);
                lua_pushinteger(L, n);
                break;
        case SQLITE_FLOAT:
                d = sqlite3_column_double(stmt, i);
                lua_pushnumber(L, d);
                break;
        case SQLITE_TEXT:
                tlen = sqlite3_column_bytes(stmt, i);
                c = sqlite3_column_text(stmt, i);
                lua_pushlstring(L, c, tlen);
                break;
        case SQLITE_BLOB:
                tlen = sqlite3_column_bytes(stmt, i);
                c = (const char*)sqlite3_column_blob(stmt, i);
                lua_pushlstring(L, c, tlen);
                break;
        case SQLITE_NULL:
                lua_pushnil(L);
                break;
        default:
                lua_pushfstring(L, "Bad SQLITE type: %d", t);
                lua_error(L);
                break;
        }
}

/* Higher level interface, e.g.
 * id, key, count, score = s:columns("itif") --int, text, int, float */
SI columns(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        size_t len;
        int i;
        const char* cs = luaL_checklstring(L, 2, &len);

        int ct = sqlite3_column_count(s->v);
        if (len != ct) {
                lua_pushfstring(L, "Invalid column count %d, result has %d columns", len, ct);
                lua_error(L);
        }
        for (i=0; i<len; i++) push_col(L, s->v, i, sqltype_of_char(L, cs[i]));

        return len;
}

/* forward references */
SI row_iter(lua_State *L);
SI row_iter_list(lua_State *L);
SI row_iter_table(lua_State *L);

struct column {
        int type;  /* SQLITE_INTEGER, FLOAT, TEXT, BLOB, or NULL */
        const char *name;
};

typedef struct col_info {
        int ct;
        struct column col[0];
} col_info;


static col_info *get_col_info(lua_State *L, sqlite3_stmt *s) {
        int ct = sqlite3_column_count(s);
        int sz = sizeof(col_info) + ct*(sizeof(struct column));
        int i;
        col_info *ci = (col_info *)malloc(sz);
        if (ci == NULL) LERROR("col_info malloc fail");
        ci->ct = ct;
        for (i=0; i<ct; i++) {
                ci->col[i].type = 0;
                ci->col[i].name = NULL;
        }
        return ci;
}

/*
 * High-level statement result interface, returns an iterator.
 * First argument:
 *   "*l" returns an iterator yielding a list of each row's columns
 *   "*t" returns a col_name=col_val table for each row
 *   any other string is treated as with columns(s), above.
 *
 * Optional second argument (TODO): 
 * If the second argument is a function, it's mapped over each row,
 * otherwise the iterator is returned. If the function returns a
 * non-true value, the iteration is considered complete and it
 * resets the statement.
 *
 * The iterator/mapper take care of calling s:step(), checking its
 * status, and calling s:reset() when complete.
 */
SI rows(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        size_t len;
        const char* cols = luaL_checklstring(L, 2, &len);

        /* check for an optional func to map over it */
/*         int hasfunc = (lua_type(L, 3) == LUA_TFUNCTION); */
        col_info *ci;

        if (0) {
                dump(L);
                printf("(ignore 'unused vars' warning) %p, %s\n", s, cols);
        }
        if (strcmp(cols, "*l") == 0) {        /* value list */
                lua_pop(L, 1);
                ci = get_col_info(L, s->v);
                lua_pushlightuserdata(L, ci);
                lua_pushcclosure(L, row_iter_list, 2);
        } else if (strcmp(cols, "*t") == 0) { /* col_name=col_val table */
                lua_pop(L, 1);
                ci = get_col_info(L, s->v);
                lua_pushlightuserdata(L, ci);
                lua_pushcclosure(L, row_iter_table, 2);
        } else {
                lua_pushcclosure(L, row_iter, 2);
        }
        return 1;
}

/* Upvalues: [stmt, cols] */
SI row_iter(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(lua_upvalueindex(1));
        size_t len;
        const char* cols = luaL_checklstring(L, lua_upvalueindex(2), &len);
        int i;

        int status = sqlite3_step(s->v);
        if (status == SQLITE_ROW) {
                for (i=0; i<len; i++) push_col(L, s->v, i, sqltype_of_char(L, cols[i]));
                return len;
        } else {
                sqlite3_reset(s->v);
                if (status != SQLITE_DONE) { pushres(L, status); lua_error(L); }
                return 0;
        }
}

static void get_col_types(sqlite3_stmt *stmt, col_info *ci) {
        int i;
        /* Types are unavailable until after calling s:step(). */
        if (ci->col[0].type == 0) {      /* if not already set */
                for (i=0; i < ci->ct; i++) {
                        ci->col[i].type = sqlite3_column_type(stmt, i);
                        ci->col[i].name = sqlite3_column_name(stmt, i);
                        TRACE("col %d -> %d / %s\n",
                            i, ci->col[i].type, ci->col[i].name);
                }
        }

}

/* Upvalues: [stmt, col_info] */
SI row_iter_list(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(lua_upvalueindex(1));
        sqlite3_stmt *stmt = s->v;
        col_info *ci = (col_info*)lua_touserdata(L, lua_upvalueindex(2));
        int i, status;
        assert(ci);
        status = sqlite3_step(stmt);
        get_col_types(stmt, ci);
        if (status == SQLITE_ROW) {
                lua_createtable(L, ci->ct, 0);
                for (i=0; i < ci->ct; i++) {
                        lua_pushinteger(L, i+1);
                        push_col(L, stmt, i, ci->col[i].type);
                        lua_settable(L, -3);
                }
                return 1;
        } else {
                sqlite3_reset(stmt);
                free(ci);
                if (status != SQLITE_DONE) { pushres(L, status); lua_error(L); }
                return 0;
        }
}

SI row_iter_table(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(lua_upvalueindex(1));
        sqlite3_stmt *stmt = s->v;
        col_info *ci = (col_info*)lua_touserdata(L, lua_upvalueindex(2));
        int i, status;
        assert(ci);

        status = sqlite3_step(stmt);
        get_col_types(stmt, ci);
        if (status == SQLITE_ROW) {
                lua_createtable(L, ci->ct, 0);
                for (i=0; i < ci->ct; i++) {
                        lua_pushstring(L, ci->col[i].name);
                        push_col(L, stmt, i, ci->col[i].type);
                        lua_settable(L, -3);
                }
                return 1;
        } else {
                sqlite3_reset(stmt);
                free(ci);
                if (status != SQLITE_DONE) { pushres(L, status); lua_error(L); }
                return 0;
        }

        return 0;
}

SI col_double(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int idx = luaL_checkinteger(L, 2);
        double d = sqlite3_column_double(s->v, idx - 1);
        lua_pushnumber(L, d);
        return 1;
}

SI col_int(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int idx = luaL_checkinteger(L, 2);
        int i = sqlite3_column_int(s->v, idx - 1);
        lua_pushnumber(L, i);
        return 1;
}

SI col_text(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int idx = luaL_checkinteger(L, 2);
        const unsigned char* t = sqlite3_column_text(s->v, idx - 1);
        lua_pushstring(L, t);
        return 1;
}

SI col_count(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        lua_pushinteger(L, sqlite3_column_count(s->v));
        return 1;
}

SI col_typestr(lua_State *L, int type) {
        const char *name;
        switch (type) {
        case SQLITE_INTEGER: name = "integer"; break;
        case SQLITE_FLOAT: name = "float"; break;
        case SQLITE_TEXT: name = "text"; break;
        case SQLITE_BLOB: name = "blob"; break;
        case SQLITE_NULL: name = "null"; break;
        default: name = "error"; break;
        }
        lua_pushstring(L, name);
        return 1;
}

SI col_type(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int idx = luaL_checkinteger(L, 2);
        int t = sqlite3_column_type(s->v, idx - 1);
        return col_typestr(L, t);
}

SI col_decltype(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int idx = luaL_checkinteger(L, 2);
        const char *t = sqlite3_column_decltype(s->v, idx - 1);
        lua_pushstring(L, t);
        return 1;
}

SI column_database_name(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int col = luaL_checkinteger(L, 2);
        const char *name = sqlite3_column_database_name(s->v, col);
        lua_pushstring(L, name);
        return 1;
}

SI column_table_name(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int col = luaL_checkinteger(L, 2);
        const char *name = sqlite3_column_table_name(s->v, col);
        lua_pushstring(L, name);
        return 1;
}

SI column_origin_name(lua_State *L) {
        LuaSQLiteStmt *s = check_stmt(1);
        int col = luaL_checkinteger(L, 2);
        const char *name = sqlite3_column_origin_name(s->v, col);
        lua_pushstring(L, name);
        return 1;
}

/*********/
/* Hooks */
/*********/

/* int sqlite3_busy_handler(sqlite3*, int(*)(void*,int), void*); */
SI busy_cb(void *ud, int callCt) {
        lua_State *L = (lua_State *) ud;
        int res;
        assert(L);
        lua_pushlightuserdata(L, L); /* should be per DB, not per VM */
        lua_gettable(L, LUA_REGISTRYINDEX);
        if (!lua_isfunction(L, -1)) {
                lua_pop(L, 1);
                lua_pushstring(L, "No callback function");
                lua_error(L);
                return -1;
        }
        lua_pushinteger(L, callCt);
        lua_pcall(L, 1, 1, 0);
        res = luaL_checkinteger(L, -1);
        return res;
}

/* static int set_busy_handler */
SI set_busy_handler(lua_State *L) {
        LuaSQLite *db = check_db(1);
        if (lua_isfunction(L, 2)) {
                lua_pushlightuserdata(L, L);
                lua_settable(L, LUA_REGISTRYINDEX);
                return pushres(L, sqlite3_busy_handler(db->v, &busy_cb, L));
        } else {
                LERROR("set_busy_handler needs function");
                return 0;
        }
}

SI set_busy_timeout(lua_State *L) {
        LuaSQLite *db = check_db(1);
        int ms = luaL_checkinteger(L, 2);
        return pushres(L, sqlite3_busy_timeout(db->v, ms));
}

/* void *sqlite3_commit_hook(sqlite3*, int(*)(void*), void*); */
/* void *sqlite3_rollback_hook(sqlite3*, void(*)(void *), void*); */
/* void sqlite3_progress_handler(sqlite3*, int, int(*)(void*), void*); */

/* void *sqlite3_update_hook( */
/*   sqlite3*,  */
/*   void(*)(void *,int ,char const *,char const *,sqlite3_int64), */
/*   void* */
/* ); */

/*************/
/* Extension */
/*************/

/*     * sqlite3_create_collation() */
/*     * sqlite3_create_module() */
/*     * sqlite3_aggregate_context() */
/*     * sqlite3_result() */
/*     * sqlite3_user_data() */
/*     * sqlite3_value() */


/* int sqlite3_create_function( */
/*   sqlite3 *db, */
/*   const char *zFunctionName, */
/*   int nArg, */
/*   int eTextRep, */
/*   void *pApp, */
/*   void (*xFunc)(sqlite3_context*,int,sqlite3_value**), */
/*   void (*xStep)(sqlite3_context*,int,sqlite3_value**), */
/*   void (*xFinal)(sqlite3_context*) */
/* ); */


/*********/
/* Blobs */
/*********/


/********/
/* Misc */
/********/

/* sqlite3_int64 sqlite3_memory_used(void); */
/* sqlite3_int64 sqlite3_memory_highwater(int resetFlag); */

SI sleep(lua_State *L) {
        int ms = luaL_checkinteger(L, 1);
        return pushres(L, sqlite3_sleep(ms));
}


/*************************/
/* Module and metatables */
/*************************/

/* General database connection. */
LIB(db_mt) = {
        { "exec", exec },
        { "get_table", get_table },
        { "errcode", errcode },
        { "errmsg", errmsg },
        { "prepare", prepare },
        { "close", close },
        { "interrupt", interrupt },
        { "changes", changes },
        { "set_busy_handler", set_busy_handler },
        { "set_busy_timeout", set_busy_timeout },
        { "last_insert_rowid", last_insert_rowid },
        { "__tostring", db_tostring },
        { "__gc", db_gc },
        { NULL, NULL },
};

LIB(stmt_mt) = {
        { "step", stmt_step },
        { "reset", stmt_reset },
        { "bind", stmt_bind },
        { "bind_double", bind_double },
        { "bind_int", bind_int },
        { "bind_null", bind_null },
        { "bind_text", bind_text },
        { "bind_param_count", bind_param_count },
        { "bind_param_index", bind_param_idx },
        { "clear", bind_clear },
        { "column_double", col_double },
        { "column_int", col_int },
        { "column_text", col_text },
        { "column_type", col_type },
        { "column_decltype", col_decltype },
        { "column_count", col_count },
        { "columns", columns },
        { "rows", rows },
        { "sql", stmt_sql },
        { "database_name", column_database_name },
        { "table_name", column_table_name },
        { "origin_name", column_origin_name },
        { "__tostring", stmt_tostring },
        { "__gc", stmt_gc },
        { NULL, NULL },
};

/* Binary blob handle. */
/* ... */


LIB(sqlite_lib) = {
        { "open", open },
        { "open_memory", open_memory },
        { "version", ver },
        { "version_number", ver_num },
        { "sleep", sleep },
        { NULL, NULL },
};

int luaopen_sqlite(lua_State *L) {
        set_MT("LuaSQLite", db_mt);
        set_MT("LuaSQLiteStmt", stmt_mt);
        luaL_register(L, "sqlite", sqlite_lib);
        return 1;
}
