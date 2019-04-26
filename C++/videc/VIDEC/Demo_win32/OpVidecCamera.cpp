#include "StdAfx.h"
#include "OpVidecCamera.h"


COpVidecCamera::MAP_OPCAMERA_CB	COpVidecCamera::m_mapCallback;

COpVidecCamera::COpVidecCamera(void)
	: m_pCB(NULL)
	, m_pVidecCamera(NULL)
	, m_nDevIndex(-1)
	, m_bWaitStop(false)
{
}

COpVidecCamera::~COpVidecCamera(void)
{
}

COpVidecCamera* COpVidecCamera::Create(COpVidecCameraCallback* pCallback)
{
	COpVidecCamera* pInst = new COpVidecCamera;
	pInst->m_pCB = pCallback;

	return pInst;
}

int COpVidecCamera::Open(int nDevIndex,int& nWidth,int& nHeight,int nFrameRate,int nTryMaxSize)
{
	m_nDevIndex = nDevIndex;

	MAP_OPCAMERA_CB::iterator it = m_mapCallback.begin();
	while (it != m_mapCallback.end())
	{
		if (nDevIndex == it->second)
		{
			if (m_pCB!=NULL)
			{
				m_mapCallback.insert(std::make_pair(m_pCB,nDevIndex));
			}
			return 0;
		}
		it++;
	}

	if (m_pCB!=NULL)
	{
		m_mapCallback.insert(std::make_pair(m_pCB,nDevIndex));
	}

	if (m_pVidecCamera == NULL)
	{
		m_pVidecCamera = VIDEC_Camera::Create(*this,1);
	}

	m_bWaitStop = false;
	StartThread();

	return m_pVidecCamera->Open(nDevIndex,nWidth,nHeight,nFrameRate,nTryMaxSize);
}

void COpVidecCamera::Close(void)
{
	m_bWaitStop = true;
	WaitForStop();

	if (m_pVidecCamera)
	{
		m_pVidecCamera->Close();
		delete m_pVidecCamera;
		m_pVidecCamera = NULL;
	}

	MAP_OPCAMERA_CB::iterator it = m_mapCallback.find(m_pCB);
	if (it != m_mapCallback.end())
	{
		m_mapCallback.erase(it);
	}

	{
		XAutoLock l(m_csListImage);
		while (m_listImage.size())
		{
			VIDEC_Image* pImage=(VIDEC_Image*)m_listImage.front();
			m_listImage.pop_front();
			pImage->Release();
			pImage=NULL;
		}
	}
}

void COpVidecCamera::ShowAdjustVideoWnd(HWND hWnd)
{

}

void COpVidecCamera::ShowVideoInputWnd(HWND hWnd)
{
}

bool COpVidecCamera::HasVideoInputWnd(void)
{
	return true;
}

bool COpVidecCamera::TVTunerPutChannel(long lChannel)
{
	return true;
}

void COpVidecCamera::ThreadProcMain(void)
{
	while (!m_bWaitStop)
	{
		VIDEC_Image* pImage=NULL;

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
			for (MAP_OPCAMERA_CB::iterator it=m_mapCallback.begin(); it!=m_mapCallback.end(); ++it)
			{
				COpVidecCameraCallback* pCb = it->first;
				if (pCb && m_nDevIndex==it->second)
				{
					pCb->OnOpVidecCameraaCallbackImageData(pImage);
				}
			}

			pImage->Release();
			pImage=NULL;
		}
		else
		{
			Sleep(1);
		}
	}
}

void COpVidecCamera::OnVIDEC_CameraCallbackImageData(VIDEC_Image*pImage)
{
	pImage->AddRef();
	XAutoLock l(m_csListImage);
	m_listImage.push_back(pImage);
}
