/*
**
**  lzma wrapper
**  目前只实现了解压功能
**  jinpeiqi 2013.07.11
*/

#ifndef _BFLZMA_H_
#define _BFLZMA_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Types.h"


#ifdef __cplusplus
extern "C" {
#endif

    typedef void (*callback) (char *filename, uint64_t size, void *p); /* 解压callback */

    typedef struct
    {
        callback cb;              /* callback */
    } lzmalite_param;

    /**
     * @param dest  // 目标目录。注意一定要确保该目录已经创建
     * @param src   // 需要解压的7z文件
     * @param param // 参数，解压回调（未完成）
     * @return      // 1 失败 0 成功
     * @remarks 解压文件.
     */
    int MY_CDECL lzmalite_uncompress(const char *dest, const char *src, lzmalite_param *param);


#ifdef __cplusplus
}
#endif

#endif  /* _BFLZMA_H_ */