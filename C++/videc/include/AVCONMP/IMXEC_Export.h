//AVCONMP_Export.h

#ifndef __AVCONMP_EXPORT_H__
#define __AVCONMP_EXPORT_H__

#ifdef AVCONMP_EXPORT
#define AVCONMP_API _declspec(dllexport)
#elif AVCONMP_DLL
#define AVCONMP_API _declspec(dllimport)
#else
#define AVCONMP_API
#endif

#ifdef IMXEC_EXPORT
#define IMXEC_API _declspec(dllexport)
#elif IMXEC_DLL
#define IMXEC_API _declspec(dllimport)
#else
#define IMXEC_API
#endif

#endif