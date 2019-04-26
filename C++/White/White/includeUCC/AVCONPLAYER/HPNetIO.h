#ifdef WIN32
#ifndef HPNETIO_AFX_EXT_CLASS 
#ifdef HPNETIO_EXT
#define HPNETIO_AFX_EXT_CLASS __declspec(dllexport)
#else
#define HPNETIO_AFX_EXT_CLASS __declspec(dllimport)
#endif
#endif
#else
#define AFX_EXT_CLASS
#define AFX_EXT_API
#endif

#include "HPNETIO/AVMTSocket.h"
#include "HPNETIO/libHPNETIO.h"
#include "HPNETIO/IHPNetIOQueue.h"
#include "HPNETIO/IHPMsgQueue.h"
#include "HPNETIO/AVMTSocketClient.h"
#include "HPNETIO/AVMTSocketTCP.h"
#include "HPNETIO/AVMTSConnection.h"
#include "HPNETIO/AVMTConnectTCPGroup.h"