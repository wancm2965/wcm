//MEDEC_Export.h

#ifndef __MEDEC_EXPORT_H__
#define __MEDEC_EXPORT_H__

#ifndef OS_LINUX//WIN32
#ifdef MEDEC_EXPORTS
#define MEDEC_API __declspec(dllexport)
#else
#define MEDEC_API __declspec(dllimport)
#endif
#else
#define MEDEC_API
#define __stdcall
#endif

#endif

