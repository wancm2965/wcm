#ifndef __HPMP_EXPORT_H__
#define __HPMP_EXPORT_H__

#ifdef HPMP_EXPORT
#define HPMP_API _declspec(dllexport)
#elif HPMP_DLL
#define HPMP_API _declspec(dllimport)
#else
#define HPMP_API
#endif

#endif