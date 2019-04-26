#include "VQDThread.h"
#include "XUtil.h"
#include <map>

VIDEC_VQD*VIDEC_VQD::Create(VIDEC_VQDCallback&rCallback)
{
	//return new VQDThread(rCallback);
	NEWRECORDret3(  VIDEC_VQD* ,p , new VQDThread( rCallback) )   
}

void VIDEC_VQD::SetParams(unsigned long nDetectFlag,int nThreshold)
{
	VQDSetParams(nDetectFlag,nThreshold);
}

VOID CALLBACK VQDThreadMgrTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

class VQDThreadMgr
{
public:
	VQDThreadMgr(void)
	{
		m_nTimerID=0;
	}
	virtual~VQDThreadMgr(void)
	{
		if (m_nTimerID)
		{
			KillTimer(NULL,m_nTimerID);
			m_nTimerID=0;
		}
	}
public:
	void AddVQDThread(VQDThread*pVQDThread)
	{
		m_MapVQDThread[pVQDThread]=pVQDThread;
		if (m_nTimerID==0)
		{
			m_nTimerID=20121204;
			SetTimer(NULL,m_nTimerID,200,VQDThreadMgrTimerProc);
		}
	}
	void RemoveVQDThread(VQDThread*pVQDThread)
	{
		m_MapVQDThread.erase(pVQDThread);
		if (m_MapVQDThread.size()==0)
		{
			if (m_nTimerID)
			{
				KillTimer(NULL,m_nTimerID);
				m_nTimerID=0;
			}
		}
	}
	void OnTimer(UINT_PTR nIDEvent)
	{
		MapVQDThread::iterator iter=m_MapVQDThread.begin();
		while (iter!=m_MapVQDThread.end())
		{
			VQDThread*pVQDThread=iter->second;
			if (pVQDThread->IsDetecting())
			{
				++iter;
			}
			else
			{
				iter=m_MapVQDThread.begin();
			}
		}
	}
protected:
	typedef std::map<void*,VQDThread*>	MapVQDThread;

	MapVQDThread	m_MapVQDThread;
	unsigned int	m_nTimerID;
};

static VQDThreadMgr g_VQDThreadMgr;

VOID CALLBACK VQDThreadMgrTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	g_VQDThreadMgr.OnTimer(idEvent);
}


VQDThread::VQDThread(VIDEC_VQDCallback&rCallback)
:m_rCallback(rCallback)
,XImageSink(this)
{
	m_pHandle=NULL;
	m_bRunning=false;
	m_nDetectFlags=0;
	m_nDetectResult=VQDS_DETECTING;
}

VQDThread::~VQDThread(void)
{

}

int VQDThread::Open(int nDetectFlags)
{
	m_nDetectFlags=nDetectFlags;
	m_pHandle=VQDOpen(nDetectFlags);
	if (m_pHandle==NULL)
	{
		return -1;
	}

	m_bRunning=true;
	if (!StartThread())
	{
		return -1;
	}

	g_VQDThreadMgr.AddVQDThread(this);

	return 0;
}

void VQDThread::Close(void)
{
	g_VQDThreadMgr.RemoveVQDThread(this);

	XImageSink::Close();

	m_bRunning=false;
	WaitForStop();

	if (m_pHandle)
	{
		VQDClose(m_pHandle);
		m_pHandle=NULL;
	}

	{
		XAutoLock l(m_csListImage);
		while (m_listImage.size())
		{
			VIDEC_Image*pTempImage=(VIDEC_Image*)m_listImage.front();
			m_listImage.pop_front();
			pTempImage->Release();
		}
	}
}

void VQDThread::OnReceivedPreSend(int& nSend)
{
	nSend=1;
}

void VQDThread::OnReceivedImage(VIDEC_Image*pImage)
{
	if (m_nDetectResult!=VQDS_DETECTING)
	{
		return ;
	}

	XAutoLock l(m_csListImage);
	pImage->AddRef();
	m_listImage.push_back(pImage);
	while (m_listImage.size()>3)
	{
		VIDEC_Image*pTempImage=(VIDEC_Image*)m_listImage.front();
		m_listImage.pop_front();
		pTempImage->Release();
	}
}

void VQDThread::OnAttachedToImageSource(VIDEC_ImageSource*pImageSource)
{
	XImageSink::OnAttachedToImageSource(pImageSource);
}

void VQDThread::OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource)
{
	XImageSink::OnDetachedFromImageSource(pImageSource);
}

bool VQDThread::IsDetecting(void)
{
	if (m_nDetectResult==VQDS_DETECTING)
	{
		return true;
	}
	g_VQDThreadMgr.RemoveVQDThread(this);
	m_rCallback.OnVIDEC_VQDCallbackResult(m_nDetectResult);
	return false;
}

void VQDThread::ThreadProcMain(void)
{
	while (m_bRunning)
	{
		VIDEC_Image*pImage=NULL;
		{
			XAutoLock l(m_csListImage);
			if (m_listImage.size())
			{
				pImage=(VIDEC_Image*)m_listImage.front();
				m_listImage.pop_front();
			}
		}
		if (pImage)
		{
			unsigned long nResult=VQDDetectFrame(m_pHandle,(unsigned char*)pImage->GetData(),pImage->GetWidth(),pImage->GetHeight());
			if (nResult!=VQDS_DETECTING)
			{
				m_nDetectResult=nResult;
				m_bRunning=false;

				{
					XAutoLock l(m_csListImage);
					while (m_listImage.size())
					{
						VIDEC_Image*pTempImage=(VIDEC_Image*)m_listImage.front();
						m_listImage.pop_front();
						pTempImage->Release();
					}
				}
			}

			pImage->Release();
			pImage=NULL;
		}
		else
		{
			XSleep(30);
		}
	}
}