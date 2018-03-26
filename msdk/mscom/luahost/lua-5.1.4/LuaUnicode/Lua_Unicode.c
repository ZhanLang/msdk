#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <windows.h>

#include "Lua_Unicode.h"
#include "lauxlib.h"

#define BUFF_SIZE 1024
#define CHAR_SCALE (sizeof(wchar_t)/sizeof(char))
#define CMD_BUFF_SIZE 4096

wchar_t * AToU (const char *str)
{
	int textlen;
	wchar_t *result;

	textlen = MultiByteToWideChar (CP_ACP, 0, str, -1, NULL, 0);
	result = (wchar_t *) malloc ((textlen + 1) * sizeof (wchar_t));
	memset (result, 0, (textlen + 1) * sizeof (wchar_t));
	MultiByteToWideChar (CP_ACP, 0, str, -1, (LPWSTR) result, textlen);
	return result;
}

char * UToA (const wchar_t * str)
{
	char *result;
	int textlen;

	// wide char to multi char
	textlen = WideCharToMultiByte (CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char *) malloc ((textlen + 1) * sizeof (char));
	memset (result, 0, sizeof (char) * (textlen + 1));
	WideCharToMultiByte (CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}

wchar_t * U8ToU (const char *str)
{
	int textlen = 0;
	wchar_t *result = 0;

	textlen = MultiByteToWideChar (CP_UTF8, 0, str, -1, NULL, 0);
	result = (wchar_t *) malloc ((textlen + 1) * sizeof (wchar_t));
	memset (result, 0, (textlen + 1) * sizeof (wchar_t));
	MultiByteToWideChar (CP_UTF8, 0, str, -1, (LPWSTR) result, textlen);
	return result;
}

char * UToU8 (const wchar_t * str)
{
	char *result;
	int textlen;

	// wide char to multi char
	textlen = WideCharToMultiByte (CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char *) malloc ((textlen + 1) * sizeof (char));
	memset (result, 0, sizeof (char) * (textlen + 1));
	WideCharToMultiByte (CP_UTF8, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}

static int Unicode_a2u (lua_State * L)
{
	const char *str;
	wchar_t *result;

	/*传递第一个参数 */
	str = lua_tostring (L, -1);
	/*开始转换 */
	result = AToU (str);
	/*返回值， */
	lua_pushlstring (L, (char *) result, wcslen (result) * CHAR_SCALE);
    free(result);
	return 1;
}

static int Unicode_u2a (lua_State * L)
{
	const wchar_t *str;
	char *result;

	/*传递第一个参数 */
	str = (wchar_t *) lua_tostring (L, -1);
	/*开始转换 */
	result = UToA (str);
	/*返回值， */
	lua_pushstring (L, result);
    free(result);
	return 1;
}

static int Unicode_u2u8 (lua_State * L)
{
	const wchar_t *str;
	char *result;

	/*传递第一个参数 */
	str = (wchar_t *) lua_tostring (L, -1);
	/*开始转换 */
	result = UToU8 (str);
	/*返回值， */
	lua_pushstring (L, result);
    free(result);
	return 1;
}

static int Unicode_u82u (lua_State * L)
{
	const char *str = 0;
	wchar_t *result = 0;

	int len = 0;
	/*传递第一个参数 */
	str = lua_tostring (L, -1);
	/*开始转换 */
	result = U8ToU (str);
	/*返回值， */
	len = wcslen (result) * CHAR_SCALE;
	lua_pushlstring (L, (char *) result, wcslen (result) * CHAR_SCALE);
    free(result);
	return 1;
}

static int Unicode_a2u8 (lua_State * L)
{
	const char *str;
	wchar_t *temp;
	char *result;

	/*传递第一个参数 */
	str = lua_tostring (L, -1);
	/*开始转换 */
	temp = AToU (str);
	result = UToU8 (temp);
	/*返回值， */
	lua_pushstring (L, result);
    free(result);
	return 1;
}

static int Unicode_u82a (lua_State * L)
{
	const char *str;
	wchar_t *temp;
	char *result;

	/*传递第一个参数 */
	str = lua_tostring (L, -1);
	/*开始转换 */
	temp = U8ToU (str);
	result = UToA (temp);
	/*返回值， */
	lua_pushstring (L, result);
    free(result);
	return 1;
}

/*获取一个文件大小*/
static int _GetFileSize (const char *filename)
{
	long len;
	FILE *fp;

	/*用只读打开文件并seek到文件末尾的方式获取文件大小 */
	if ((fp = fopen (filename, "r")) == NULL)
	{
		printf ("%s is not invalid\n", filename);
		return 0;
	}
	fseek (fp, 0, SEEK_END);
	len = ftell (fp);
	fclose (fp);
	return len;
}

/*Lua 获取 文件大小*/
static int GetFileSizeW (lua_State * L)
{
	/*传递第一个参数，文件名 */
	const char *filename = lua_tostring (L, -1);
	lua_pushinteger (L, _GetFileSize (filename));
	return 1;
}

/*读取一个Unicode文件，使用fgetwc函数，IO导致速度较慢*/
static int GetAllFileWC (lua_State * L)
{
	/*传递第一个参数，文件名 */
	int i = 0;
	wchar_t *buf = 0;
	FILE *input = 0;
	const char *filename = lua_tostring (L, -1);
	/*获取文件大小 */
	int len;
	len = _GetFileSize (filename);
	/*设置缓存大小 */
	
	/*由于wchar_t长度为char长度一倍，所以buf空间大小为文件长度一半，再加末尾的 '\0' */
	buf = (wchar_t *) malloc (sizeof (wchar_t) * (len / CHAR_SCALE + 1));

	
	input = fopen (filename, "rb");
	while (!feof (input))
	{
		buf[i++] = fgetwc (input);
	}
	/*字符串末尾置零 */
	buf[i - 1] = L'\0';
	lua_pushlstring (L, (char *) buf, wcslen (buf) * CHAR_SCALE);
	free (buf);
	return 1;
}

/*读取一个Unicode文件，使用fgetws函数，速度较快*/
static int GetAllFileWS (lua_State * L)
{

	int i = 0;
	wchar_t *buf = 0;
	/*传递第一个参数，文件名 */
	const char *filename = lua_tostring (L, -1);
	/*获取文件大小 */
	FILE *input = fopen (filename, "rb");
	int len = _GetFileSize (filename);
	/*初始化变量 */
	wchar_t *all;
	all = (wchar_t *) malloc (sizeof (wchar_t) * (len / CHAR_SCALE + 1));
	memset (all, 0, sizeof (wchar_t) * (len / CHAR_SCALE + 1));

	

	buf = (wchar_t *) malloc (sizeof (wchar_t) * BUFF_SIZE);
	memset (buf, 0, sizeof (wchar_t) * BUFF_SIZE);
	while (!feof (input))
	{
		int j = 0;

		fgetws (buf, BUFF_SIZE, input);
		/*连接缓存空间，原先使用wcscat连接，但是很慢且有问题，使用指针后速度很快 */
		while (buf[j] != L'\0')
		{
			all[i++] = buf[j++];
		}
		/*清空临时buf */
		memset (buf, 0, sizeof (wchar_t) * BUFF_SIZE);
	}
	all[len / CHAR_SCALE] = L'\0';
	lua_pushlstring (L, (char *) all, wcslen (all) * CHAR_SCALE);
	/*删除临时变量 */
	free (buf);
	free (all);
	return 1;
}

//初始化字符串指针
char * StringInit ()
{
	char *str = (char *) malloc (1 * sizeof (char));
	memset (str, 0, 1 * sizeof (char));
	return str;
}

//设置字符串为指定字符
char * StringSet (char *str, const char *toset)
{
	int len = strlen (toset) + 1;
	str = (char *) realloc (str, len * sizeof (char));
	strcpy (str, toset);
	return str;
}

//在字符串末尾添加指定字符串
char * StringAppent (char *first, const char *last)
{
	int len = strlen (first) + strlen (last) + 1;

	first = (char *) realloc (first, len * sizeof (char));
	strcat (first, last);
	return first;
}

//在字符串指定位置添加指定字符串
char * StringInsert (char *str, const char *insert, int start)
{
	int i = 0 ,pos = 0,len = 0;
	//对开始插入的数字进行验证
	if (start > strlen (str))
	{
		start = strlen (str);
	}
	else if (start < 0)
	{
		start = 0;
	}
	//重新分配内容
	pos = strlen (str);
	len = strlen (str) + strlen (insert) + 1;
	str = (char *) realloc (str, len * sizeof (char));

	//使用指针的方式进行遍历
	
	//通过倒序方式将准备插入数据后位置的数据复制到最后
	for (i = 1; i <= pos - start; i++)
	{
		str[len - i - 1] = str[len - i - strlen (insert) - 1];
	}
	//将准备插入的数据插入到开始插入的地方
	for (i = 1; i <= strlen (insert); i++)
	{
		str[i + start - 1] = insert[i - 1];
	}
	//末尾置0
	str[len - 1] = '\0';
	return str;
}

static const luaL_reg PearFunctions[] = {
	{"a2u", Unicode_a2u},
	{"u2a", Unicode_u2a},
	{"u2u8", Unicode_u2u8},
	{"u82u", Unicode_u82u},
	{"a2u8", Unicode_a2u8},
	{"u82a", Unicode_u82a},
	{"getfilesizew", GetFileSizeW},
	{"getallfilewc", GetAllFileWC},
	{"getallfilews", GetAllFileWS},
	
	{NULL, NULL}
};
#if LUA_VERSION_NUM < 502
#  define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif

LUALIB_API int luaopen_LC (lua_State * L)
{
	luaL_newlib(L, PearFunctions);

	lua_pushvalue(L, -1);
	lua_setglobal(L, LUA_LC_LIBNAME);

	//luaL_openlib (L, LUA_LC_LIBNAME, PearFunctions, 0);
	return 1;
}
