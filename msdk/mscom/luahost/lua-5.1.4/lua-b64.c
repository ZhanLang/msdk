/***
Fast base64 encoding and decoding.

@author Natanael Copa <ncopa@alpinelinux.org>
@copyright 2013
@license MIT/X11
@module b64

see http://tools.ietf.org/html/rfc4648


***/

#define LUA_CORE

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <string.h>

/* encode a raw buffer to base64.
 * encoded dest buffer size must be at least 1 + (((length + 2) / 3) * 4)
 * returns a zero terminated base64 string
 */
static const char *base64_tbl =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char *urlsafe_base64_tbl =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";


static int b64_encode_tbl(lua_State *L, const char *tbl)
{
	size_t len, n = 0;
	const unsigned char *src = (const unsigned char *)luaL_checklstring(L, 1, &len);
	luaL_Buffer dst;
	unsigned int triplet = 0;
	unsigned int pad;
	char s[4];

	luaL_buffinit(L, &dst);
	while (n < len) {
		triplet = (triplet << 8) | *src++;
		pad = 2 - (n % 3);
		n++;
		if (pad == 0) {
			s[0] = tbl[(triplet >> (3*6))];
			s[1] = tbl[(triplet >> (2*6)) & 63];
			s[2] = tbl[(triplet >> 6) & 63];
			s[3] = tbl[triplet & 63];
			luaL_addlstring(&dst, s, 4);
			triplet = 0;
		}
	}
	if (pad) {
		triplet = (triplet << (pad * 8));
		s[0] = tbl[(triplet >> (3*6))];
		s[1] = tbl[(triplet >> (2*6)) & 63];
		s[2] = pad > 1 ? '=' : tbl[(triplet >> 6) & 63];
		s[3] = '=';
		luaL_addlstring(&dst, s, 4);
	}
	luaL_pushresult(&dst);
	return 1;
}

static int b64_encode(lua_State *L)
{
	return b64_encode_tbl(L, base64_tbl);
}

static int b64_encode_urlsafe(lua_State *L)
{
	return b64_encode_tbl(L, urlsafe_base64_tbl);
}


static void init_reverse_base64_buffer(char *reverse_tbl, const char *tbl)
{
	unsigned char c;
	memset(reverse_tbl, 0, 256);
	for (c = 0; c<64; c++)
		reverse_tbl[ (unsigned int)tbl[c] ] = c;
}

static int b64_decode_tbl(lua_State *L, const char *tbl)
{
	size_t len, i = 0, n = 0;
	const char *src = luaL_checklstring(L, 1, &len);
	unsigned int triplet = 0;
	luaL_Buffer dst;

	luaL_buffinit(L, &dst);
	while (i < len) {
		unsigned char c = src[i++];
		n++;
		triplet = (triplet << 6) | tbl[c];
		if ((n & 3) == 0 || i == len) {
			int j;
			char b[3];
			for (j=2; j >=0; j--)
				b[2-j] = (triplet >> (j*8)) & 0xff;
			luaL_addlstring(&dst, b, 3);
			triplet=0;
		}
	}
	luaL_pushresult(&dst);
	return 1;
}

static int b64_decode(lua_State *L)
{
	static int reverse_initialized = 0;
	static char reverse_tbl[256];
	if (!reverse_initialized)
		init_reverse_base64_buffer(reverse_tbl, base64_tbl);
	return b64_decode_tbl(L, reverse_tbl);
}

static int b64_decode_urlsafe(lua_State *L)
{
	static int reverse_initialized = 0;
	static char reverse_tbl[256];
	if (!reverse_initialized)
		init_reverse_base64_buffer(reverse_tbl, urlsafe_base64_tbl);
	return b64_decode_tbl(L, reverse_tbl);
}

static const luaL_Reg b64_methods[] = {
	{ "encode", b64_encode},
	{ "encode_urlsafe", b64_encode_urlsafe},
	{ "decode", b64_decode},
	{ "decode_urlsafe", b64_decode_urlsafe},
	{ NULL, NULL},
};

#if LUA_VERSION_NUM < 502
#  define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif

LUA_API int luaopen_b64(lua_State *L)
{
	luaL_newlib(L, b64_methods);

	lua_pushvalue(L, -1);
	lua_setglobal(L, B64_LIBNAME);

	return 1;
}

