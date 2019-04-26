#ifdef WIN32
#ifndef HPBC_AFX_EXT_CLASS 
#ifdef HPBC_EXT
#define HPBC_AFX_EXT_CLASS __declspec(dllexport)
#else
#define HPBC_AFX_EXT_CLASS __declspec(dllimport)
#endif
#endif
#else
#define HPBC_AFX_EXT_CLASS
#define HPBC_AFX_EXT_API
#endif

#include "HPBC/AVAutoLock.h"
#include "HPBC/AVBufferPool.h"
#include "HPBC/AVCritSec.h"
#include "HPBC/AVDataBuffer.h"
#include "HPBC/AVEvent.h"
#include "HPBC/AVThread.h"
#include "HPBC/AVUtil.h"
#include "HPBC/ListPtr.h"