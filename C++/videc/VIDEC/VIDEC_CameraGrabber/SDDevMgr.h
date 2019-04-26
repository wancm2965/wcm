//SDDevMgr.h
#ifndef __SDDEVMGR_H__
#define __SDDEVMGR_H__

#include <winsock2.h>
#include <windows.h>

#include "VIDEC_OutputImage.h"
#include "VIDEC_OutputStream.h"

class SDDevMgr
{
public:
	static int Init(void);
	static void Terminate(void);

	static int GetFormatCount(void);
	static int GetFormatName(int nIndex,char*szName);
	static int GetFormatSize(int nIndex,int&nWidth,int&nHeight);
	static bool IsValidFormatSize(int&nWidth,int&nHeight);


	static int GetDevCount(void);
	static int GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount);

	static int SetDevCap(unsigned int nDevIndex,int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate);

	static int AddOutputImage(unsigned int nDevIndex,VIDEC_OutputImage*pOutputImage);
	static void RemoveOutputImage(unsigned int nDevIndex,VIDEC_OutputImage*pOutputImage);

	static int AddOutputStream(unsigned int nDevIndex,VIDEC_OutputStream*pOutputStream);
	static void RemoveOutputStream(unsigned int nDevIndex,VIDEC_OutputStream*pOutputStream);
};

#endif