#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int demangle(const char *mangled_name,char *unmangled_name,size_t buffersize);
	
#define UNAME_SIZE 512

#ifdef __cplusplus
}
#endif
	
