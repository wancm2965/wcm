#ifndef __AVH264DECODERWRAPPER_H__
#define __AVH264DECODERWRAPPER_H__



#include <tchar.h>
#include "../../VIDEC_FRE/VIDECFREAPI.h"

#define  MAX_FILEPATH  1024 


class XAVCDllLoadLibrary
{
public:
	XAVCDllLoadLibrary(TCHAR *cszDllName)
	{
		m_hDllModule = LoadLibrary(cszDllName);
		m_bIsOK = (m_hDllModule != NULL);
		if (!m_bIsOK)
		{
			DWORD dwError = GetLastError();
			char msg[256] = { 0 };
			sprintf(msg, ("LoadLibrary error, DLL Name:%s,code: %d \n"), cszDllName,dwError);
			DebugOutput(logcoreerror,msg);
		}
	}
	virtual~XAVCDllLoadLibrary()
	{
		if (m_hDllModule)
		{
			FreeLibrary(m_hDllModule);
			m_hDllModule = NULL;
		}
	}
public:


	bool IsOK(void)
	{
		return m_bIsOK;
	}

	template<class T>
	void LoadFunction(T &pFunction, char *cszFunctionName)
	{

//#ifdef UNICODE
//		MultiByteToWideChar(CP_UTF8, 0, fn, -1, tfn, MAX_PATH);
//#endif

//		char name[MAX_FILEPATH];
//#ifdef UNICODE
//		WideCharToMultiByte(CP_UTF8, 0, cszFunctionName, -1, name, MAX_FILEPATH, 0, 0);
//#else
//		strcpy(name, cszFunctionName);
//#endif

		pFunction = m_hDllModule ? (T)GetProcAddress(m_hDllModule, cszFunctionName) : NULL;
		m_bIsOK &= (pFunction != NULL);
		if (!m_bIsOK)
		{
			DWORD dwError = GetLastError();
			char msg[512] = { 0 };
			sprintf(msg, ("DLL GetProcAddress error,Function:%s,code: %d \n"), cszFunctionName,dwError);
			DebugOutput(logcoreerror,msg);
		}
	}
protected:
	bool	m_bIsOK;
	HMODULE m_hDllModule;
};





class AVH264DECODERWrapper
{
private:
	XAVCDllLoadLibrary*	m_pXDllLoadLibrary;
public: 
	AVH264DECODERWrapper(HINSTANCE hInstance);
	~AVH264DECODERWrapper(void);

public:
	bool   m_bLoadFuncOK;

public:
	static AVH264DECODERWrapper&Instance(void);
	bool  IsOK(void);

	 void   (*pAVH264DECODER_Initialize)(void);
	 void   (*pAVH264DECODER_UnInitialize)(void);
	 void * (*pAVH264DECODER_DecoderOpen)(int nWidth, int nHeight, int AVCCodecID);
	 void   (*pAVH264DECODER_DecoderClose)(void*pHandle);
	 int    (*pAVH264DECODER_DecoderDecode)(void*pHandle, unsigned char*pStreamData, int nStreamLen, unsigned char*pYV12[], int nStride[], int*pnWidth, int*pnHeight, int*pnUsedBytes, int*pnKeyFrame, char**ppqscale_table, int*pqstride, int* pix_fmt);

};



bool  AVH264DECODERInit(void);
void  AVH264DECODERTerminate(void);
void* AVH264DECODEROpen(int nWidth, int nHeight, int AVCCodecID);
void  AVH264DECODERClose(void*pHandle);
int   AVH264DECODERDecode(void*pHandle, unsigned char*pStreamData, int nStreamLen, unsigned char*pYV12[], int nStride[], int*pnWidth, int*pnHeight, int*pnUsedBytes, int*pnKeyFrame, char**ppqscale_table, int*pqstride, int* pix_fmt);



#endif
