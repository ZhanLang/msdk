#ifndef	BASE64_H__
#define	BASE64_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Ω‚√‹
long Base64Decode(IN const char* lpszDecoding, OUT unsigned char* pbytDecoded);

//º”√‹
long Base64Encode(IN unsigned char* lpbytEncoding, IN long lInputLength, OUT char* pszEncoded);

#endif	//BASE64_H__