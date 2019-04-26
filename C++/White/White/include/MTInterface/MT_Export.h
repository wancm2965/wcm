//MTClient_Export.h

#ifndef __MT_CLIENT_EXPORT_H__
#define __MT_CLIENT_EXPORT_H__

#if defined(WIN32) || defined(_WIN64)
	#ifdef AVCON_MT_GESTURE_EXPORT
		#define AVCON_CLASS_MT_GESTURE	_declspec(dllexport)
		#define AVCON_API_MT_GESTURE	_declspec(dllexport)
		#pragma warning(disable: 4251)
	#elif AVCON_CLASS_MT_GESTURE_NONE
		#define AVCON_CLASS_MT_GESTURE
		#define AVCON_API_MT_GESTURE
	#else
		#define AVCON_CLASS_MT_GESTURE	_declspec(dllimport)
		#define AVCON_API_MT_GESTURE	_declspec(dllimport)
	#endif
#else
	#define AVCON_CLASS_MT_GESTURE 
#endif

#endif