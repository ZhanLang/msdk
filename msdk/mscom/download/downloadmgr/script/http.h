#pragma once
#include <string>

namespace http{;

std::string http_post(const char* url ,const char* lpszPath, const char* lpszParam);
std::string http_request(const char* url);
bool http_download(const char*, const char* file);

char Dec2HexChar(short int n) ;

short int HexChar2Dec(char c);

std::string encode_url(const char* url);

std::string decode_url(const char* URL);
}