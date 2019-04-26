#ifndef __RTOU_EXPORT_H__
#define __RTOU_EXPORT_H__

#ifdef RTOU_EXPORT
#define RTOU_API _declspec(dllexport)
#elif RTOU_DLL
#define RTOU_API _declspec(dllimport)
#else
#define RTOU_API
#endif

#endif