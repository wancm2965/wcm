#ifndef __KBASE_EXPORT_H__
#define __KBASE_EXPORT_H__

#ifdef WIN32
#ifdef KBASE_EXPORT
#define KBASE_API _declspec(dllexport)
#else
#define KBASE_API _declspec(dllimport)
#endif // KBASE_EXPORT
#else
#define KBASE_API
#endif // WIN32

#ifdef WIN32
// TODO: 应当优化代码以修复警告
#pragma warning(disable: 4251)
#endif

#endif