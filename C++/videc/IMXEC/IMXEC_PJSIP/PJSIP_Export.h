//PJSIP_Export.h
#ifndef __PJSIP_EXPORT_H__
#define __PJSIP_EXPORT_H__

#ifdef PJSIP_EXPORT
#define PJSIP_API _declspec(dllexport)
#elif PJSIP_DLL
#define PJSIP_API _declspec(dllimport)
#else
#define PJSIP_API
#endif

#endif//__PJSIP_EXPORT_H__