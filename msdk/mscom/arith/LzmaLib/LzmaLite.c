/* 
** LzmaLite.c -- lzma wrapper
*/

#include <stdio.h>
#include <string.h>

#include "7z.h"
#include "7zAlloc.h"
#include "7zCrc.h"
#include "7zFile.h"
#include "7zVersion.h"
#include "LzmaLite.h"


static ISzAlloc g_Alloc = { SzAlloc, SzFree };

static int Buf_EnsureSize(CBuf *dest, size_t size)
{
    if (dest->size >= size)
        return 1;
    Buf_Free(dest, &g_Alloc);
    return Buf_Create(dest, size, &g_Alloc);
}

static SRes Utf16_To_Char(CBuf *buf, const UInt16 *s, int fileMode)
{
    int len = 0;
    for (len = 0; s[len] != '\0'; len++);

    {
        int size = len * 3 + 100;
        if (!Buf_EnsureSize(buf, size))
            return SZ_ERROR_MEM;
        {
            char defaultChar = '_';
            BOOL defUsed;
            int numChars = WideCharToMultiByte(fileMode ? (AreFileApisANSI() ? CP_ACP : CP_OEMCP) : CP_OEMCP,
                                                   0, s, len, (char *)buf->data, size, &defaultChar, &defUsed);
            if (numChars == 0 || numChars >= size)
                return SZ_ERROR_FAIL;
            buf->data[numChars] = 0;
            return SZ_OK;
        }
    }
}

int Char_To_Utf16(const char *in_char, size_t in_size, char *out_char, size_t out_size)
{
    size_t in_len ;
    size_t out_len;
    int i = 0;

    if(in_char == NULL)
        return;
    if(out_size > 2 * in_size + 2)
        return;

    /*utf 16 file header, Big-Endian FEFF;little-Endian FFFE*/
    out_char[0] = 0xFF;
    out_char[1] = 0xFE;
    for(i = 1; i < in_size; i = i + 1)
    {
        out_char[2 * i] = in_char[i - 1];
        out_char[2 * i + 1] = 0x00;
    }
    out_char[out_size] = '\0';
    return 0;
}

static WRes MyCreateDir(const char* path, const UInt16 *name)
{
    CBuf buf;

    char fullpath[MAX_PATH] = {0};
    strcpy(fullpath, path);
    strcat(fullpath, "\\");

    Buf_Init(&buf);
    Utf16_To_Char(&buf, name, 0);
    strcat(fullpath, (const char *)buf.data);
    Buf_Free(&buf, &g_Alloc);

    {
        char dbgmsg[1024];
        sprintf(dbgmsg, "lzmalite_uncompress, CreateDir fullpath = %s", fullpath);
        OutputDebugStringA(dbgmsg);
    }

    return CreateDirectoryA(fullpath, NULL) ? 0 : GetLastError();
}

static WRes OutFile_OpenUtf16(CSzFile *p, const char* path, const UInt16 *name)
{
    CBuf buf;

    char fullpath[MAX_PATH] = {0};
    strcpy(fullpath, path);

    Buf_Init(&buf);
    Utf16_To_Char(&buf, name, 0);
    strcat(fullpath, (const char *)buf.data);
    Buf_Free(&buf, &g_Alloc);

    {
        char dbgmsg[1024];
        sprintf(dbgmsg, "lzmalite_uncompress, OutFile_OpenUtf16 fullpath = %s",fullpath);
        OutputDebugStringA(dbgmsg);
    }

    return OutFile_Open(p, fullpath);
}

static SRes PrintString(const UInt16 *s)
{
    CBuf buf;
    SRes res;
    Buf_Init(&buf);
    res = Utf16_To_Char(&buf, s, 0);
    if (res == SZ_OK)
        fputs((const char *)buf.data, stdout);
    Buf_Free(&buf, &g_Alloc);
    return res;
}

static void UInt64ToStr(UInt64 value, char *s)
{
    char temp[32];
    int pos = 0;
    do
    {
        temp[pos++] = (char)('0' + (unsigned)(value % 10));
        value /= 10;
    }
    while (value != 0);
    do
        *s++ = temp[--pos];
    while (pos);
    *s = '\0';
}

static char *UIntToStr(char *s, unsigned value, int numDigits)
{
    char temp[16];
    int pos = 0;
    do
        temp[pos++] = (char)('0' + (value % 10));
    while (value /= 10);
    for (numDigits -= pos; numDigits > 0; numDigits--)
        *s++ = '0';
    do
        *s++ = temp[--pos];
    while (pos);
    *s = '\0';
    return s;
}

#define PERIOD_4 (4 * 365 + 1)
#define PERIOD_100 (PERIOD_4 * 25 - 1)
#define PERIOD_400 (PERIOD_100 * 4 + 1)

static void ConvertFileTimeToString(const CNtfsFileTime *ft, char *s)
{
    unsigned year, mon, day, hour, min, sec;
    UInt64 v64 = (ft->Low | ((UInt64)ft->High << 32)) / 10000000;
    Byte ms[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    unsigned t;
    UInt32 v;
    sec = (unsigned)(v64 % 60);
    v64 /= 60;
    min = (unsigned)(v64 % 60);
    v64 /= 60;
    hour = (unsigned)(v64 % 24);
    v64 /= 24;

    v = (UInt32)v64;

    year = (unsigned)(1601 + v / PERIOD_400 * 400);
    v %= PERIOD_400;

    t = v / PERIOD_100;
    if (t ==  4) t =  3;
    year += t * 100;
    v -= t * PERIOD_100;
    t = v / PERIOD_4;
    if (t == 25) t = 24;
    year += t * 4;
    v -= t * PERIOD_4;
    t = v / 365;
    if (t ==  4) t =  3;
    year += t;
    v -= t * 365;

    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
        ms[1] = 29;
    for (mon = 1; mon <= 12; mon++)
    {
        unsigned s = ms[mon - 1];
        if (v < s)
            break;
        v -= s;
    }
    day = (unsigned)v + 1;
    s = UIntToStr(s, year, 4);
    *s++ = '-';
    s = UIntToStr(s, mon, 2);
    *s++ = '-';
    s = UIntToStr(s, day, 2);
    *s++ = ' ';
    s = UIntToStr(s, hour, 2);
    *s++ = ':';
    s = UIntToStr(s, min, 2);
    *s++ = ':';
    s = UIntToStr(s, sec, 2);
}

void PrintError(char *sz)
{
    char errstr[1024];
    sprintf(errstr, "ERROR: %s", sz);
    OutputDebugStringA(errstr);
}

#define kEmptyAttribChar '.'
static void GetAttribString(UInt32 wa, Bool isDir, char *s)
{
    s[0] = (char)(((wa & FILE_ATTRIBUTE_DIRECTORY) != 0 || isDir) ? 'D' : kEmptyAttribChar);
    s[1] = (char)(((wa & FILE_ATTRIBUTE_READONLY) != 0) ? 'R' : kEmptyAttribChar);
    s[2] = (char)(((wa & FILE_ATTRIBUTE_HIDDEN) != 0) ? 'H' : kEmptyAttribChar);
    s[3] = (char)(((wa & FILE_ATTRIBUTE_SYSTEM) != 0) ? 'S' : kEmptyAttribChar);
    s[4] = (char)(((wa & FILE_ATTRIBUTE_ARCHIVE) != 0) ? 'A' : kEmptyAttribChar);
    s[5] = '\0';
}

int MY_CDECL lzmalite_uncompress( const char *dest, const char *src, lzmalite_param *param )
{
    CFileInStream archiveStream;
    CLookToRead lookStream;
    CSzArEx db;
    SRes res;
    ISzAlloc allocImp;
    ISzAlloc allocTempImp;
    UInt16 *temp = NULL;
    size_t tempSize = 0;

    allocImp.Alloc = SzAlloc;
    allocImp.Free = SzFree;

    allocTempImp.Alloc = SzAllocTemp;
    allocTempImp.Free = SzFreeTemp;

    if (InFile_Open(&archiveStream.file, src))
    {
        PrintError("can not open input file");
        return 1;
    }

    FileInStream_CreateVTable(&archiveStream);
    LookToRead_CreateVTable(&lookStream, False);

    lookStream.realStream = &archiveStream.s;
    LookToRead_Init(&lookStream);

    CrcGenerateTable();

    SzArEx_Init(&db);
    res = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);

    if (res == SZ_OK)
    {
        UInt32 i;

        /*
        if you need cache, use these 3 variables.
        if you use external function, you can make these variable as static.
        */
        UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
        Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
        size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */

        char dbgmsg[1024];
        sprintf(dbgmsg, "lzmalite_uncompress, NumFiles = %d", db.db.NumFiles);
        OutputDebugStringA(dbgmsg);

        for (i = 0; i < db.db.NumFiles; i++)
        {
            size_t offset = 0;
            size_t outSizeProcessed = 0;
            const CSzFileItem *f = db.db.Files + i;
            size_t len;

            len = SzArEx_GetFileNameUtf16(&db, i, NULL);

            if (len > tempSize)
            {
                SzFree(NULL, temp);
                tempSize = len;
                temp = (UInt16 *)SzAlloc(NULL, tempSize * sizeof(temp[0]));
                if (temp == 0)
                {
                    res = SZ_ERROR_MEM;
                    break;
                }
            }

            SzArEx_GetFileNameUtf16(&db, i, temp);

            /*res = PrintString(temp);
            if (res != SZ_OK)
                break;*/

            if (f->IsDir)
                printf("/");
            else
            {
                res = SzArEx_Extract(&db, &lookStream.s, i,
                                     &blockIndex, &outBuffer, &outBufferSize,
                                     &offset, &outSizeProcessed,
                                     &allocImp, &allocTempImp);
                if (res != SZ_OK)
                    break;
            }

            {
                CSzFile outFile;
                size_t processedSize;
                size_t j;
                UInt16 *name = (UInt16 *)temp;
                const UInt16 *destPath = (const UInt16 *)name;

                for (j = 0; name[j] != 0; j++)
                {
                    if (name[j] == '/')
                    {
                        name[j] = 0;
                        MyCreateDir(dest, name);
                        name[j] = CHAR_PATH_SEPARATOR;
                    }
                }

                if (f->IsDir)
                {
                    MyCreateDir(dest, destPath);
                    printf("\n");
                    continue;
                }
                else if (OutFile_OpenUtf16(&outFile, dest, destPath))
                {
                    PrintError("can not open output file");
                    res = SZ_ERROR_FAIL;
                    continue;   // continue
                }

                processedSize = outSizeProcessed;
                if (File_Write(&outFile, outBuffer + offset, &processedSize) != 0 || processedSize != outSizeProcessed)
                {
                    PrintError("can not write output file");
                    res = SZ_ERROR_FAIL;
                    break;
                }
                if (File_Close(&outFile))
                {
                    PrintError("can not close output file");
                    res = SZ_ERROR_FAIL;
                    break;
                }
                if (f->AttribDefined)
                    SetFileAttributesW(destPath, f->Attrib);
            }
            printf("\n");
        }
        IAlloc_Free(&allocImp, outBuffer);
    }

    SzArEx_Free(&db, &allocImp);
    SzFree(NULL, temp);

    File_Close(&archiveStream.file);
    if (res == SZ_OK)
    {
        return 0;
    }

    if (res == SZ_ERROR_UNSUPPORTED)
        PrintError("decoder doesn't support this archive");
    else if (res == SZ_ERROR_MEM)
        PrintError("can not allocate memory");
    else if (res == SZ_ERROR_CRC)
        PrintError("CRC error");
    else
        printf("\nERROR #%d\n", res);

    return 1;
}
