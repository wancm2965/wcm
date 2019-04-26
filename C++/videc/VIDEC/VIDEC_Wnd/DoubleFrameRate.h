//DoubleFrameRate.h
#ifndef __DOUBLEFRAMERATE_H__
#define __DOUBLEFRAMERATE_H__

#include "../VIDEC_API/XImage.h"
#include "../VIDEC_csp/fastmemcpy.h"

#ifdef _WIN64
extern "C"  void __stdcall BilinearMMX_Asm(unsigned char*pSrcLine0, unsigned char*pSrcLine2, unsigned char*pDstLine1, unsigned int nLineSize);
#endif

inline void BilinearMMX(unsigned char*pSrcLine0,unsigned char*pSrcLine2,unsigned char*pDstLine1,unsigned int nLineSize)
{
	nLineSize>>=2;

#ifdef _WIN64

	BilinearMMX_Asm((unsigned char*)pSrcLine0, (unsigned char*)pSrcLine2, (unsigned char*)pDstLine1, nLineSize);

#else

	__asm{
		emms
			pxor mm7, mm7
			mov ecx, nLineSize

			mov  esi, pSrcLine0
			mov  eax, pSrcLine2
			mov  edi, pDstLine1 

START_LOOP:
		movd mm1, [esi];
		movd mm3, [eax];

		punpcklbw mm1, mm7;
		punpcklbw mm3, mm7;
		paddw mm1, mm3;

		psrlw   mm1, 1;
		packuswb mm1,mm7;

		movd  [edi], mm1;

		add  esi, 4
			add  eax, 4
			add  edi, 4

			dec         ecx
			jnz         START_LOOP			
			emms			
	}

#endif

}


class DoubleFrameRate
{
public:
	DoubleFrameRate(void)
		: m_pFrameBufOdd(NULL)
		, m_pFrameBufEven(NULL)
	{
		m_nBufLen=0;
	};
	virtual~DoubleFrameRate(void)
	{
		if (m_pFrameBufOdd!=NULL)
		{
			FREEMSG(m_pFrameBufOdd);
			m_pFrameBufOdd=NULL;
		}
		if (m_pFrameBufEven!=NULL)
		{
			FREEMSG(m_pFrameBufEven);
			m_pFrameBufEven=NULL;
		}
	}
public:
	virtual void OnFrameData(XImage*pXImage){};
	virtual int Process(XImage*pXImage)
	{
		if (pXImage->GetEvenField() && pXImage->GetOddField())
		{
			int nWidth=pXImage->GetActualWidth();
			int nHeight=pXImage->GetActualHeight();
			unsigned char*pData=(unsigned char*)pXImage->GetData();
			int nLen=pXImage->GetDataLen();

			if (m_pFrameBufOdd==NULL || m_pFrameBufEven==NULL || nLen>m_nBufLen)
			{
				if (m_pFrameBufOdd!=NULL)
				{
					FREEMSG(m_pFrameBufOdd);
					m_pFrameBufOdd=NULL;
				}
				if (m_pFrameBufEven!=NULL)
				{
					FREEMSG(m_pFrameBufEven);
					m_pFrameBufEven=NULL;
				}
				m_nBufLen=nLen;
				m_pFrameBufOdd=(unsigned char*)MALLOCMSG(m_nBufLen);
				m_pFrameBufEven=(unsigned char*)MALLOCMSG(m_nBufLen);
			}

			if (m_pFrameBufOdd==NULL || m_pFrameBufEven==NULL)
			{
				return -1;
			}

			int nHalfWidth=(nWidth>>1);
			int nHalfHeight=(nHeight>>1);
			int nYLen=nWidth*nHeight;
			int nUVLen=(nYLen>>2);

			unsigned char*pSrcEvenY=pData;
			unsigned char*pSrcEvenU=pSrcEvenY+nYLen;
			unsigned char*pSrcEvenV=pSrcEvenU+nUVLen;

			unsigned char*pSrcOddY=pSrcEvenY+nWidth*nHalfHeight;
			unsigned char*pSrcOddU=pSrcEvenU+nHalfWidth*nHalfHeight/2;
			unsigned char*pSrcOddV=pSrcEvenV+nHalfWidth*nHalfHeight/2;

			unsigned char*pDstEvenY=m_pFrameBufEven;
			unsigned char*pDstEvenU=pDstEvenY+nYLen;
			unsigned char*pDstEvenV=pDstEvenU+nUVLen;

			unsigned char*pDstOddY=m_pFrameBufOdd;
			unsigned char*pDstOddU=pDstOddY+nYLen;
			unsigned char*pDstOddV=pDstOddU+nUVLen;


			pDstEvenY+=(nWidth<<1);
			pSrcEvenY+=nWidth;

			pDstOddY+=(nWidth<<1);

			memset(pDstEvenU,127,nHalfWidth);
			pDstEvenU+=nHalfWidth;
			memset(pDstEvenV,127,nHalfWidth);
			pDstEvenV+=nHalfWidth;

			memset(pDstOddU,127,nHalfWidth);
			pDstOddU+=nHalfWidth;
			memset(pDstOddV,127,nHalfWidth);
			pDstOddV+=nHalfWidth;

			for (int i=1;i<(nHalfHeight>>1);i++)
			{
				fastmemcpy(pDstEvenY,pSrcEvenY,nWidth);
				pDstEvenY+=nWidth;

				unsigned char*pNextEvenY=pSrcEvenY+nWidth;
				BilinearMMX(pSrcEvenY,pNextEvenY,pDstEvenY,nWidth);
				pSrcEvenY+=nWidth;
				pDstEvenY+=nWidth;

				unsigned char*pNextOddY=pSrcOddY+nWidth;
				BilinearMMX(pSrcOddY,pNextOddY,pDstOddY,nWidth);
				pSrcOddY+=nWidth;
				pDstOddY+=nWidth;

				fastmemcpy(pDstOddY,pSrcOddY,nWidth);
				pDstOddY+=nWidth;

				fastmemcpy(pDstEvenY,pSrcEvenY,nWidth);
				pDstEvenY+=nWidth;

				pNextEvenY=pSrcEvenY+nWidth;
				BilinearMMX(pSrcEvenY,pNextEvenY,pDstEvenY,nWidth);
				pSrcEvenY+=nWidth;
				pDstEvenY+=nWidth;

				pNextOddY=pSrcOddY+nWidth;
				BilinearMMX(pSrcOddY,pNextOddY,pDstOddY,nWidth);
				pSrcOddY+=nWidth;
				pDstOddY+=nWidth;

				fastmemcpy(pDstOddY,pSrcOddY,nWidth);
				pDstOddY+=nWidth;

				fastmemcpy(pDstEvenU,pSrcEvenU,nHalfWidth);
				pDstEvenU+=nHalfWidth;
				fastmemcpy(pDstEvenV,pSrcEvenV,nHalfWidth);
				pDstEvenV+=nHalfWidth;

				unsigned char*pNextEvenU=pSrcEvenU+nHalfWidth;
				unsigned char*pNextEvenV=pSrcEvenV+nHalfWidth;
				BilinearMMX(pSrcEvenU,pNextEvenU,pDstEvenU,nHalfWidth);
				pSrcEvenU+=nHalfWidth;
				pDstEvenU+=nHalfWidth;
				BilinearMMX(pSrcEvenV,pNextEvenV,pDstEvenV,nHalfWidth);
				pSrcEvenV+=nHalfWidth;
				pDstEvenV+=nHalfWidth;

				unsigned char*pNextOddU=pSrcOddU+nHalfWidth;
				unsigned char*pNextOddV=pSrcOddV+nHalfWidth;
				BilinearMMX(pSrcOddU,pNextOddU,pDstOddU,nHalfWidth);
				pSrcOddU+=nHalfWidth;
				pDstOddU+=nHalfWidth;
				BilinearMMX(pSrcOddV,pNextOddV,pDstOddV,nHalfWidth);
				pSrcOddV+=nHalfWidth;
				pDstOddV+=nHalfWidth;

				fastmemcpy(pDstOddU,pSrcOddU,nHalfWidth);
				pDstOddU+=nHalfWidth;
				fastmemcpy(pDstOddV,pSrcOddV,nHalfWidth);
				pDstOddV+=nHalfWidth;
			}
			//后四行涂黑
			memset(m_pFrameBufEven,0,(nWidth<<2));
			memset(m_pFrameBufOdd,0,(nWidth<<2));
			//后四行涂黑
			pDstEvenY=m_pFrameBufEven;
			pDstEvenU=pDstEvenY+nYLen;
			pDstEvenV=pDstEvenU+nUVLen;

			pDstOddY=m_pFrameBufOdd;
			pDstOddU=pDstOddY+nYLen;
			pDstOddV=pDstOddU+nUVLen;

			int nLines=6;
			if (nHeight<1080)
				nLines=4;
			memset(pDstEvenY+nWidth*(nHeight-nLines),0,nWidth*nLines);
			memset(pDstEvenU+nHalfWidth*(nHalfHeight-(nLines>>1)),127,nHalfWidth*(nLines>>1));
			memset(pDstEvenV+nHalfWidth*(nHalfHeight-(nLines>>1)),127,nHalfWidth*(nLines>>1));
			memset(pDstOddY+nWidth*(nHeight-nLines),0,nWidth*nLines);
			memset(pDstOddU+nHalfWidth*(nHalfHeight-(nLines>>1)),127,nHalfWidth*(nLines>>1));
			memset(pDstOddV+nHalfWidth*(nHalfHeight-(nLines>>1)),127,nHalfWidth*(nLines>>1));


			NEWRECORD3( XImage*,pXImageEven,new XImage(m_pFrameBufEven,nLen) ) 
			if(pXImageEven ==NULL) return -1;
			pXImageEven->AddRef();
			m_pFrameBufEven=NULL;

			pXImageEven->SetActualWidth(pXImage->GetActualWidth());
			pXImageEven->SetActualHeight(pXImage->GetActualHeight());
			pXImageEven->SetVirtualWidth(pXImage->GetVirtualWidth());
			pXImageEven->SetVirtualHeight(pXImage->GetVirtualHeight());
			pXImageEven->SetDoubleField(pXImage->GetDoubleField());
			pXImageEven->SetEvenField(pXImage->GetDoubleField());
			pXImageEven->SetOddField(pXImage->GetDoubleField());


			NEWRECORD3( XImage*,pXImageOdd,new XImage(m_pFrameBufOdd,nLen) )
			if(pXImageOdd ==NULL) return -1;
			pXImageOdd->AddRef();
			m_pFrameBufOdd=NULL;

			pXImageOdd->SetActualWidth(pXImage->GetActualWidth());
			pXImageOdd->SetActualHeight(pXImage->GetActualHeight());
			pXImageOdd->SetVirtualWidth(pXImage->GetVirtualWidth());
			pXImageOdd->SetVirtualHeight(pXImage->GetVirtualHeight());
			pXImageOdd->SetDoubleField(pXImage->GetDoubleField());
			pXImageOdd->SetOddField(pXImage->GetDoubleField());
			pXImageOdd->SetOddField(pXImage->GetDoubleField());

			if (pXImage->GetEvenFieldFirst())
			{
				pXImageEven->SetTimestamp(pXImage->GetTimestamp());
				OnFrameData(pXImageEven);
				pXImageOdd->SetTimestamp(pXImage->GetTimestamp()+20);
				OnFrameData(pXImageOdd);
			}
			else
			{
				pXImageOdd->SetTimestamp(pXImage->GetTimestamp());
				OnFrameData(pXImageOdd);
				pXImageEven->SetTimestamp(pXImage->GetTimestamp()+20);
				OnFrameData(pXImageEven);
			}

			pXImageEven->Release();
			pXImageEven=NULL;


			pXImageOdd->Release();
			pXImageOdd=NULL;
			return 0;
		}

		if (pXImage->GetEvenField())
		{
			int nWidth=pXImage->GetActualWidth();
			int nHeight=pXImage->GetActualHeight();
			unsigned char*pData=(unsigned char*)pXImage->GetData();
			int nLen=pXImage->GetDataLen();

			if (m_pFrameBufEven==NULL || (nLen<<1)>m_nBufLen)
			{
				if (m_pFrameBufEven!=NULL)
				{
					FREEMSG(m_pFrameBufEven);
					m_pFrameBufEven=NULL;
				}
				m_nBufLen=(nLen<<1);
				m_pFrameBufEven=(unsigned char*)MALLOCMSG(m_nBufLen);
			}

			if (m_pFrameBufEven==NULL)
			{
				return -1;
			}


			int nHalfWidth=(nWidth>>1);
			unsigned char*pSrcEvenY=pData;
			unsigned char*pSrcEvenU=pSrcEvenY+nWidth*nHeight;
			unsigned char*pSrcEvenV=pSrcEvenU+(nWidth*nHeight>>2);

			unsigned char*pDstEvenY=m_pFrameBufEven;
			unsigned char*pDstEvenU=pDstEvenY+(nWidth*nHeight<<1);
			unsigned char*pDstEvenV=pDstEvenU+(nWidth*nHeight>>1);

			pDstEvenY+=(nWidth<<1);
			pSrcEvenY+=nWidth;

			memset(pDstEvenU,127,nHalfWidth);
			pDstEvenU+=nHalfWidth;
			memset(pDstEvenV,127,nHalfWidth);
			pDstEvenV+=nHalfWidth;

			for (int i=1;i<(nHeight>>1);i++)
			{
				fastmemcpy(pDstEvenY,pSrcEvenY,nWidth);
				pDstEvenY+=nWidth;

				unsigned char*pNextEvenY=pSrcEvenY+nWidth;
				BilinearMMX(pSrcEvenY,pNextEvenY,pDstEvenY,nWidth);
				pSrcEvenY+=nWidth;
				pDstEvenY+=nWidth;

				fastmemcpy(pDstEvenY,pSrcEvenY,nWidth);
				pDstEvenY+=nWidth;

				pNextEvenY=pSrcEvenY+nWidth;
				BilinearMMX(pSrcEvenY,pNextEvenY,pDstEvenY,nWidth);
				pSrcEvenY+=nWidth;
				pDstEvenY+=nWidth;

				fastmemcpy(pDstEvenU,pSrcEvenU,nHalfWidth);
				pDstEvenU+=nHalfWidth;
				fastmemcpy(pDstEvenV,pSrcEvenV,nHalfWidth);
				pDstEvenV+=nHalfWidth;

				unsigned char*pNextEvenU=pSrcEvenU+nHalfWidth;
				unsigned char*pNextEvenV=pSrcEvenV+nHalfWidth;
				BilinearMMX(pSrcEvenU,pNextEvenU,pDstEvenU,nHalfWidth);
				pSrcEvenU+=nHalfWidth;
				pDstEvenU+=nHalfWidth;
				BilinearMMX(pSrcEvenV,pNextEvenV,pDstEvenV,nHalfWidth);
				pSrcEvenV+=nHalfWidth;
				pDstEvenV+=nHalfWidth;
			}
			//后四行涂黑
			memset(m_pFrameBufEven,0,(nWidth<<2));
			//后四行涂黑
			pDstEvenY=m_pFrameBufEven;
			pDstEvenU=pDstEvenY+(nWidth*nHeight<<1);
			pDstEvenV=pDstEvenU+(nWidth*nHeight>>1);

			int nLines=6;
			if (nHeight<=288)
				nLines=4;
			memset(pDstEvenY+nWidth*((nHeight<<1)-nLines),0,nWidth*nLines);
			memset(pDstEvenU+nHalfWidth*(nHeight-(nLines>>1)),127,nHalfWidth*(nLines>>1));
			memset(pDstEvenV+nHalfWidth*(nHeight-(nLines>>1)),127,nHalfWidth*(nLines>>1));

			NEWRECORD3( XImage*,pXImageEven,new XImage(m_pFrameBufEven,nLen<<1) ) 
				if(pXImageEven ==NULL) return -1;
			pXImageEven->AddRef();
			m_pFrameBufEven=NULL;

			pXImageEven->SetActualWidth(pXImage->GetActualWidth());
			pXImageEven->SetActualHeight(pXImage->GetActualHeight()<<1);
			pXImageEven->SetVirtualWidth(pXImage->GetVirtualWidth());
			pXImageEven->SetVirtualHeight(pXImage->GetVirtualHeight());
			pXImageEven->SetDoubleField(pXImage->GetDoubleField());
			pXImageEven->SetEvenField(pXImage->GetDoubleField());
			pXImageEven->SetOddField(pXImage->GetDoubleField());
			pXImageEven->SetTimestamp(pXImage->GetTimestamp());

			OnFrameData(pXImageEven);

			pXImageEven->Release();
			pXImageEven=NULL;

			return 0;
		}

		if (pXImage->GetOddField())
		{
			int nWidth=pXImage->GetActualWidth();
			int nHeight=pXImage->GetActualHeight();
			unsigned char*pData=(unsigned char*)pXImage->GetData();
			int nLen=pXImage->GetDataLen();

			if (m_pFrameBufOdd==NULL || (nLen<<1)>m_nBufLen)
			{
				if (m_pFrameBufOdd!=NULL)
				{
					FREEMSG(m_pFrameBufOdd);
					m_pFrameBufOdd=NULL;
				}
				m_nBufLen=nLen<<1;
				m_pFrameBufOdd=(unsigned char*)MALLOCMSG(m_nBufLen);
			}

			if (m_pFrameBufOdd==NULL)
			{
				return -1;
			}

			int nHalfWidth=(nWidth>>1);
			unsigned char*pSrcOddY=pData;
			unsigned char*pSrcOddU=pSrcOddY+nWidth*nHeight;
			unsigned char*pSrcOddV=pSrcOddU+(nWidth*nHeight>>2);

			unsigned char*pDstOddY=m_pFrameBufOdd;
			unsigned char*pDstOddU=pDstOddY+(nWidth*nHeight<<1);
			unsigned char*pDstOddV=pDstOddU+(nWidth*nHeight>>1);


			pDstOddY+=(nWidth<<1);

			memset(pDstOddU,127,nHalfWidth);
			pDstOddU+=nHalfWidth;
			memset(pDstOddV,127,nHalfWidth);
			pDstOddV+=nHalfWidth;

			for (int i=1;i<(nHeight>>1);i++)
			{
				unsigned char*pNextOddY=pSrcOddY+nWidth;
				BilinearMMX(pSrcOddY,pNextOddY,pDstOddY,nWidth);
				pSrcOddY+=nWidth;
				pDstOddY+=nWidth;

				fastmemcpy(pDstOddY,pSrcOddY,nWidth);
				pDstOddY+=nWidth;

				pNextOddY=pSrcOddY+nWidth;
				BilinearMMX(pSrcOddY,pNextOddY,pDstOddY,nWidth);
				pSrcOddY+=nWidth;
				pDstOddY+=nWidth;

				fastmemcpy(pDstOddY,pSrcOddY,nWidth);
				pDstOddY+=nWidth;

				unsigned char*pNextOddU=pSrcOddU+nHalfWidth;
				unsigned char*pNextOddV=pSrcOddV+nHalfWidth;
				BilinearMMX(pSrcOddU,pNextOddU,pDstOddU,nHalfWidth);
				pSrcOddU+=nHalfWidth;
				pDstOddU+=nHalfWidth;
				BilinearMMX(pSrcOddV,pNextOddV,pDstOddV,nHalfWidth);
				pSrcOddV+=nHalfWidth;
				pDstOddV+=nHalfWidth;

				fastmemcpy(pDstOddU,pSrcOddU,nHalfWidth);
				pDstOddU+=nHalfWidth;
				fastmemcpy(pDstOddV,pSrcOddV,nHalfWidth);
				pDstOddV+=nHalfWidth;
			}
			//后四行涂黑
			memset(m_pFrameBufOdd,0,(nWidth<<2));
			//后四行涂黑
			pDstOddY=m_pFrameBufOdd;
			pDstOddU=pDstOddY+(nWidth*nHeight<<1);
			pDstOddV=pDstOddU+(nWidth*nHeight>>1);

			int nLines=6;
			if (nHeight<=288)
				nLines=4;
			memset(pDstOddY+nWidth*((nHeight<<1)-nLines),0,nWidth*nLines);
			memset(pDstOddU+nHalfWidth*(nHeight-(nLines>>1)),127,nHalfWidth*(nLines>>1));
			memset(pDstOddV+nHalfWidth*(nHeight-(nLines>>1)),127,nHalfWidth*(nLines>>1));

			NEWRECORD3( XImage*,pXImageOdd,new XImage(m_pFrameBufOdd,nLen<<1) )
				if(pXImageOdd ==NULL) return -1;
			pXImageOdd->AddRef();
			m_pFrameBufOdd=NULL;

			pXImageOdd->SetActualWidth(pXImage->GetActualWidth());
			pXImageOdd->SetActualHeight(pXImage->GetActualHeight()<<1);
			pXImageOdd->SetVirtualWidth(pXImage->GetVirtualWidth());
			pXImageOdd->SetVirtualHeight(pXImage->GetVirtualHeight());
			pXImageOdd->SetDoubleField(pXImage->GetDoubleField());
			pXImageOdd->SetOddField(pXImage->GetDoubleField());
			pXImageOdd->SetOddField(pXImage->GetDoubleField());
			pXImageOdd->SetTimestamp(pXImage->GetTimestamp());

			OnFrameData(pXImageOdd);

			pXImageOdd->Release();
			pXImageOdd=NULL;

			return 0;
		}

		return 0;
	}
protected:
	unsigned char*			m_pFrameBufOdd;
	unsigned char*			m_pFrameBufEven;
	int						m_nBufLen;
};

#endif
