#ifndef xerces_H_
#define xerces_H_

#ifndef LIBNAME
#define LIBNAME "rapidxml"
#endif

#ifndef LIBVER
#define LIBVER "0.1"
#endif

extern "C" {
int luaopen_rapidxml(void* L) ;
}
;

#endif