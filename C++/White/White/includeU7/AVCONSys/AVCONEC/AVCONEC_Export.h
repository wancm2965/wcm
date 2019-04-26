//AVCONEC_Export.h

#ifndef __AVCONEC_EXPORT_H__
#define __AVCONEC_EXPORT_H__

#ifdef AVCONEC_EXPORT
#define AVCONEC_API _declspec(dllexport)
#elif AVCONEC_DLL
#define AVCONEC_API _declspec(dllimport)
#else
#define AVCONEC_API
#endif

#endif