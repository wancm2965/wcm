//XMemcpy.h
#ifndef __XMEMCPY_H__
#define __XMEMCPY_H__

#include <stdio.h>
#include <memory.h>

// SSE4.1 based memcpy that copies from video memory to system memory
void* gpu_memcpy(void* d, const void* s, size_t _size);

bool IsSSE41Enabled();


#endif
