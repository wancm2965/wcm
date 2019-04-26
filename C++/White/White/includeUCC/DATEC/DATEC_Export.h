//DATEC_Export.h

#ifndef __DATEC_EXPORT_H__
#define __DATEC_EXPORT_H__

#ifdef DATEC_EXPORT
#define DATEC_API _declspec(dllexport)
#elif DATEC_DLL
#define DATEC_API _declspec(dllimport)
#else
#define DATEC_API
#endif

#endif