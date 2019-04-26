//fastmemcpy.h
#ifndef __FASTMEMCPY_H__
#define __FASTMEMCPY_H__

typedef void* (__cdecl FASTMEMCPY)(void * to, const void * from, size_t len);

extern FASTMEMCPY* fastmemcpy;

#endif
