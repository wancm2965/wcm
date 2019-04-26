#ifndef __HPAVC_EXPORT_H__
#define __HPAVC_EXPORT_H__

#ifdef HPAVC_EXPORT
#define HPAVC_API _declspec(dllexport)
#elif HPAVC_DLL
#define HPAVC_API _declspec(dllimport)
#else
#define HPAVC_API
#endif

#endif