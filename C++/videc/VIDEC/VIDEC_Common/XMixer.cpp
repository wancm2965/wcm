#include "XMixer.h"
#include "XUtil.h"

VIDEC_API VIDEC_Mixer*VIDEC_MixerCreate(VIDEC_MixerNotify&rVIDEC_MixerNotify,int nWidth,int nHeight)
{
	XMixer*pXMixer=new XMixer(rVIDEC_MixerNotify);
	if (pXMixer->Open(nWidth,nHeight)!=0)
	{
		pXMixer->Close();
		delete pXMixer;
		pXMixer=NULL;
	}

	return pXMixer;
}


XMixer::XMixer(VIDEC_MixerNotify&rMixerNotify)
:m_rMixerNotify(rMixerNotify)
,m_bRunning(false)
{

}

XMixer::~XMixer(void)
{

}

int XMixer::Open(int nWidth,int nHeight)
{
	if (0!=XImageMixer::Open(nWidth,nHeight))
	{
		return -1;
	}

	m_bRunning=true;
	if (!XThreadBase::StartThread())
	{
		m_bRunning=false;
		return -1;
	}

	return 0;
}


void XMixer::Close(void)
{
	m_bRunning=false;
	XThreadBase::WaitForStop();

	XOutputImageMgr::Close();
	XOutputStreamMgr::Close();
	XOutputRTPMgr::Close();

	XImageMixer::Close();
}
	
int XMixer::CreateMixerImageInput(VIDEC_MixerImageInput**ppMixerImageInput,int nRectIndex,bool bVisible)
{
	XAutoLock l(m_csMapXMixerInput);
	XMixerImageInput*pXMixerImageInput=new XMixerImageInput(*this,nRectIndex,bVisible);
	m_MapXMixerInput[(XMixerInput*)pXMixerImageInput]=pXMixerImageInput;

	*ppMixerImageInput=pXMixerImageInput;
	return 0;
}
	
int XMixer::CreateMixerRTPInput(VIDEC_MixerRTPInput**ppMixerRTPInput,int nRectIndex,bool bVisible)
{
	XAutoLock l(m_csMapXMixerInput);
	XMixerRTPInput*pXMixerRTPInput=new XMixerRTPInput(*this,nRectIndex,bVisible);
	m_MapXMixerInput[(XMixerInput*)pXMixerRTPInput]=pXMixerRTPInput;

	*ppMixerRTPInput=pXMixerRTPInput;
	return 0;
}	

int XMixer::CreateMixerStreamInput(VIDEC_MixerStreamInput**ppMixerStreamInput,int nRectIndex,bool bVisible)
{
	XAutoLock l(m_csMapXMixerInput);
	XMixerStreamInput*pXMixerStreamInput=new XMixerStreamInput(*this,nRectIndex,bVisible);
	m_MapXMixerInput[(XMixerInput*)pXMixerStreamInput]=pXMixerStreamInput;

	*ppMixerStreamInput=pXMixerStreamInput;
	return 0;
}

void XMixer::RemoveXMixerInput(XMixerInput*pXMixerInput)
{
	XAutoLock l(m_csMapXMixerInput);
	m_MapXMixerInput.erase(pXMixerInput);
}

int XMixer::AddOutputImage(VIDEC_OutputImage*pOutputImage)
{
	return XOutputImageMgr::AddOutputImage(pOutputImage);
}

void XMixer::RemoveOutputImage(VIDEC_OutputImage*pOutputImage)
{
	XOutputImageMgr::RemoveOutputImage(pOutputImage);
}

int XMixer::AddOutputStream(VIDEC_OutputStream*pOutputStream)
{
	return XOutputStreamMgr::AddOutputStream(pOutputStream);
}

void XMixer::RemoveOutputStream(VIDEC_OutputStream*pOutputStream)
{
	XOutputStreamMgr::RemoveOutputStream(pOutputStream);
}

int XMixer::AddOutputRTP(VIDEC_OutputRTP*pOutputRTP)
{
	return XOutputRTPMgr::AddOutputRTP(pOutputRTP);
}

void XMixer::RemoveOutputRTP(VIDEC_OutputRTP*pOutputRTP)
{
	XOutputRTPMgr::RemoveOutputRTP(pOutputRTP);
}

int XMixer::OutputImage(void*pData,int nLen,int nWidth,int nHeight)
{
	XOutputImageMgr::OutputImage(pData,nLen,nWidth,nHeight);
	XOutputStreamMgr::OutputImage(pData,nLen,nWidth,nHeight);
	XOutputRTPMgr::OutputImage(pData,nLen,nWidth,nHeight);
	return 0;
}

void XMixer::ThreadProcMain(void)
{
	unsigned long ulPreviousTime=XGetTimestamp();
	unsigned long ulFrameInterval=0;
	int nMaxFrameRate=0;
	int nError=0;
	while (m_bRunning)
	{
		if (XOutputImageMgr::GetMaxFrameRate()>nMaxFrameRate)
			nMaxFrameRate=XOutputImageMgr::GetMaxFrameRate();
		if (XOutputStreamMgr::GetMaxFrameRate()>nMaxFrameRate)
			nMaxFrameRate=XOutputStreamMgr::GetMaxFrameRate();
		if (XOutputRTPMgr::GetMaxFrameRate()>nMaxFrameRate)
			nMaxFrameRate=XOutputRTPMgr::GetMaxFrameRate();
		if (nMaxFrameRate<=0)
		{
			XSleep(100);
			continue;
		}

		if (nMaxFrameRate>30)
			nMaxFrameRate=30;

		ulFrameInterval=1000/nMaxFrameRate;

		nError+=(int)ulFrameInterval;
		unsigned long ulNow=XGetTimestamp();
		unsigned long ulDelta=ulNow-ulPreviousTime;
		nError-=(int)ulDelta;
		ulPreviousTime=ulNow;

		if (nError>0)
		{
			XSleep(nError);
			//char szDebug[1024];
			//sprintf(szDebug,"nError=%d\n",nError);
			//OutputDebugString(szDebug);
		}
		//else
		//{
		//	OutputDebugString("nError=0\n");
		//}


		int nFullScreenRectIndex=m_rMixerNotify.OnVIDEC_MixerNotifyGetFullScreenRectIndex();
		int nRectCount=m_rMixerNotify.OnVIDEC_MixerNotifyGetRectCount();
		bool bFilledRect=false;
		
		//先看看有没有全屏幕的视频
		if (nFullScreenRectIndex>=0 && nFullScreenRectIndex<nRectCount)
		{
			XMixerInput*pXMixerInput=NULL;
			XAutoLock l(m_csMapXMixerInput);
			MapXMixerInput::iterator iter=m_MapXMixerInput.begin();
			while (iter!=m_MapXMixerInput.end())
			{
				pXMixerInput=iter->second;
				if (pXMixerInput->GetRectIndex()==nFullScreenRectIndex)
				{
					break;
				}
				pXMixerInput=NULL;
				++iter;
			}

			if (pXMixerInput!=NULL)
			{
				pXMixerInput->Lock();
				
				unsigned char*pImageData=pXMixerInput->GetImageData();
				if (pImageData)
				{
					MixImage(pImageData,pXMixerInput->GetImageWidth(),pXMixerInput->GetImageHeight(),0,0,m_nImageWidth,m_nImageHeight);
					bFilledRect=true;
				}

				pXMixerInput->UnLock();
			}
		}
		
		//如果没有，那么一个个混像
		if (!bFilledRect)
		{
			for (int nRectIndex=0;nRectIndex<nRectCount;nRectIndex++)
			{
				int x=0;
				int y=0;
				int cx=0;
				int cy=0;
				m_rMixerNotify.OnVIDEC_MixerNotifyGetRect(nRectIndex,x,y,cx,cy);

				XMixerInput*pXMixerInput=NULL;
				XAutoLock l(m_csMapXMixerInput);
				MapXMixerInput::iterator iter=m_MapXMixerInput.begin();
				while (iter!=m_MapXMixerInput.end())
				{
					pXMixerInput=iter->second;
					if (pXMixerInput->GetRectIndex()==nRectIndex)
					{
						break;
					}
					pXMixerInput=NULL;
					++iter;
				}

				if (pXMixerInput!=NULL)
				{
					pXMixerInput->Lock();
					
					unsigned char*pImageData=pXMixerInput->GetImageData();
					if (pImageData)
					{
						MixImage(pImageData,pXMixerInput->GetImageWidth(),pXMixerInput->GetImageHeight(),x,y,cx,cy);
						bFilledRect=true;
					}

					pXMixerInput->UnLock();
				}
				
				if (!bFilledRect)
				{
					FillRect(x,y,cx,cy,255,0,0);
				}
			}
		}

		//将混好的视频送去压缩
		OutputImage(m_pImageBuffer,m_nImageSize,m_nImageWidth,m_nImageHeight);

		//控制帧率
		//XSleep(ulFrameInterval);
	}
}
