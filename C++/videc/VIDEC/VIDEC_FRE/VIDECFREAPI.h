//VIDECFREAPI.h
#ifndef __VIDECFREAPI_H__
#define __VIDECFREAPI_H__

#include "VIDECFREHeader.h"


//ipc func add
void setharddecodercount(int nCount=0 );
void sethardencodercount(int nCount=0 );
void setd3ddisplay(int nUse =0);
void fresethardalldisable();
void fresetkeyframe(int nSecond);
void fresetlogcore(int nLevel);
void fresetlogmodule(int nLevel);
void frelogcode();
int  fresetpoolcount(int nKeyNameIndex ,int count);
void frehelp();



VIDECFRE_API unsigned long FRE_GetMaxKeyFrameInterval(void);

VIDECFRE_API void FREIPC_Init(void);
VIDECFRE_API void FREIPC_Terminate(void);

VIDECFRE_API void FRE_Init(void);
VIDECFRE_API void FRE_Terminate(void);

VIDECFRE_API int  FRE_GetDebugLevel(void);

VIDECFRE_API int  FRE_GetHWEncodeWidth(void);
VIDECFRE_API int  FRE_GetHWEncodeHeight(void);
VIDECFRE_API int  FRE_GetHWDecodeWidth(void);
VIDECFRE_API int  FRE_GetHWDecodeHeight(void);

VIDECFRE_API int  FRE_GetUseD3D(void);
VIDECFRE_API int  FRE_GetD3D11Decoder(void);
VIDECFRE_API int  FRE_GetPending(void);
VIDECFRE_API int  FRE_GetDelayMilliSecond(void);
VIDECFRE_API int  FRE_GetDelayFrameRate(void);
VIDECFRE_API int  FRE_GetD3D11Type(void);
VIDECFRE_API int  FRE_GetD3D11Debug(void);
VIDECFRE_API int  FRE_GetMaxEncoderCount(void);
VIDECFRE_API int  FRE_GetForceIntelEncoder(void);
VIDECFRE_API int  FRE_GetMaxDecoderCount(void);
VIDECFRE_API int  FRE_GetD3D11Total(void);
VIDECFRE_API int  FRE_GetSkipDisplay(void);
VIDECFRE_API int  FRE_GetPIT(void);

class VIDECFRE_API FrameRateEvent
{
public:
	FrameRateEvent(void);
	virtual~FrameRateEvent(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	virtual void WaitEvent(void);
	virtual void OnEvent(void);
	virtual void SetFrameRate(int nFrameRate);
protected:
	HANDLE	m_hEventFrameRate;	
	int		m_nFrameRate;
};

//void * FreMalloc(size_t size,char * fileName,char * func,int nLine);
//
//#define  MALLOCMSG(size)      FreMalloc(size,__FILE__,__FUNCTION__,__LINE__)  
//

//void VIDECFRE_API DebugOutput(int nLevel, char* msg);
//
//void VIDECFRE_API DebugOutput(char* msg);
//
//void VIDECFRE_API DebugOutput(const char* fmt,...);
//
//void VIDECFRE_API DebugOutput(int nLevel,const char* fmt,...);

#endif