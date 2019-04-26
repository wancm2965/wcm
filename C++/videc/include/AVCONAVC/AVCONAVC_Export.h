#ifndef __AVCONAVC_EXPORT_H__
#define __AVCONAVC_EXPORT_H__

#ifdef AVCONAVC_EXPORTS
#define AVCONAVC_API _declspec(dllexport)
#elif AVCONAVC_DLL
#define AVCONAVC_API _declspec(dllimport)
#else
#define AVCONAVC_API
#endif

#endif