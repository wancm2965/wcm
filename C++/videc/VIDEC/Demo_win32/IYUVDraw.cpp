#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
//#include "VIDEC_csp.h"
#include "XAutoLock.h"
//#include "VIDEC_Convert.h"
#include "IYUVDraw.h"
#include "../jpeglib/jpegutils.h"

bool IYUVDraw::SaveToFileJPG(int nWidth,int nHeight,int nVirtualWidth,int nVirtualHeight,unsigned char*pData,int nLen,const char*cszFile)
{
	return false;
	/*unsigned char*pScaledBuf=NULL;
	unsigned char*pTempYUV=pData;

	if (nVirtualWidth==0)
	{
		nVirtualWidth=nWidth;
	}
	if (nVirtualHeight==0)
	{
		nVirtualHeight=nHeight;
	}

	if (nVirtualWidth!=nWidth || nVirtualHeight!=nHeight)
	{
		VIDEC_Resize*pResize=VIDEC_Resize::Create();
		if (pResize!=NULL)
		{
			if (pResize->Open()!=0)
			{
				pResize->Close();
				delete pResize;
				pResize=NULL;

				return false;
			}

			pScaledBuf=(unsigned char*)malloc(nVirtualWidth*nVirtualHeight*2);
			if (pScaledBuf==NULL)
			{
				pResize->Close();
				delete pResize;
				pResize=NULL;

				return false;
			}

			int nSrcYLen=nWidth*nHeight;
			PBYTE pSrcYUV[3];
			pSrcYUV[0]=pData;
			pSrcYUV[1]=pSrcYUV[0]+nSrcYLen;
			pSrcYUV[2]=pSrcYUV[1]+(nSrcYLen>>2);

			int nDstYLen=nVirtualWidth*nVirtualHeight;
			PBYTE pDstYUV[3];
			pDstYUV[0]=pScaledBuf;
			pDstYUV[1]=pDstYUV[0]+nDstYLen;
			pDstYUV[2]=pDstYUV[1]+(nDstYLen>>2);

			if (0!=pResize->Resize(pSrcYUV,nWidth,nHeight,pDstYUV,nVirtualWidth,nVirtualHeight))
			{
				pResize->Close();
				delete pResize;
				pResize=NULL;

				free(pScaledBuf);
				pScaledBuf=NULL;
				return false;
			}

			pResize->Close();
			delete pResize;
			pResize=NULL;

			pTempYUV=pScaledBuf;
		}
	}

	bool bRet=false;
	unsigned char*pY=pTempYUV;
	unsigned char*pU=pY+nVirtualWidth*nVirtualHeight;
	unsigned char*pV=pU+(nVirtualWidth*nVirtualHeight/4);
	if (0==WriteYUVBytesIntoJpegFile(cszFile,nVirtualWidth,nVirtualHeight,100,pY,pU,pV))
	{
		bRet=true;
	}
	else
	{
		bRet=false;
	}

	if (pScaledBuf!=NULL)
	{
		free(pScaledBuf);
		pScaledBuf=NULL;
	}

	return bRet;*/
}