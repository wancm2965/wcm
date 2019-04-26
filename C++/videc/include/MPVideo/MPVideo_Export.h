#ifndef __MPVIDEO_EXPORT_H__
#define __MPVIDEO_EXPORT_H__

#ifdef MPVIDEO_EXPORT
#define MPVIDEO_API _declspec(dllexport)
#elif MPVIDEO_DLL
#define MPVIDEO_API _declspec(dllimport)
#else
#define MPVIDEO_API
#endif

#endif