#define CCTK_DEBUG_DEBUG

#if defined(CCTK_DEBUG_DEBUG)

#include <stdlib.h>

#undef malloc
#define malloc(x) CCTKi_Malloc(x,__LINE__,__FILE__)

#undef free
#define free(x) CCTKi_Free(x,__LINE__,__FILE__)

#undef calloc
#define calloc(x,y) CCTKi_Calloc(x, y,__LINE__,__FILE__) 

#undef realloc
#define realloc(x,y) CCTKi_Realloc(x, y,__LINE__,__FILE__)

void *CCTKi_Malloc(size_t size, int line, const char *file);
void  CCTKi_Free(void *pointer, int line, const char *file);
void *CCTKi_Calloc(size_t nmemb, size_t size, int line, const char *file);
void *CCTKi_Realloc(void *pointer, size_t size, int line, const char *file);

#endif
