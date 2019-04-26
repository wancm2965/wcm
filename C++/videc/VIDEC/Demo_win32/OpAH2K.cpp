#include "StdAfx.h"
#include "OpAH2K.h"

COpAH2K::MAP_OPAH2K_CB COpAH2K::m_mapCallback;
COpAH2K::COpAH2K(void)
	: m_pAH2k(NULL)
	, m_nDevIndex(-1)
	, m_pCB(NULL)
{
}

COpAH2K::~COpAH2K(void)
{
}

COpAH2K* COpAH2K::Create(COpAH2KCallback* pCb)
{
	COpAH2K* pInst = new COpAH2K;
	pInst->m_pCB = pCb;
	
	return pInst;
}

int COpAH2K::Open(int nDevID,int nWidth,int nHeight,int nFrameRate)
{
	m_nDevIndex = nDevID;

	MAP_OPAH2K_CB::iterator it = m_mapCallback.begin();
	while (it != m_mapCallback.end())
	{
		if (nDevID==it->second)
		{
			if (m_pCB!=NULL)
			{
				m_mapCallback.insert(std::make_pair(m_pCB,nDevID));
			}
			return 0;
		}
		it++;
	}

	if (m_pCB!=NULL)
	{
		m_mapCallback.insert(std::make_pair(m_pCB,nDevID));
	}
	if (m_pAH2k == NULL)
	{
		m_pAH2k = VIDEC_AH2K::Create(*this);
	}

	m_bWaitStop = false;
	StartThread();

	return m_pAH2k->Open(nDevID,nWidth,nHeight,nFrameRate);
}

void COpAH2K::Close(void)
{
	m_bWaitStop = true;
	WaitForStop();

	if (m_pAH2k)
	{
		m_pAH2k->Close();
		delete m_pAH2k;
		m_pAH2k = NULL;
	}
	MAP_OPAH2K_CB::iterator it = m_mapCallback.find(m_pCB);
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

int COpAH2K::StartVideoStream(int nBitrate,int nBitrateControlType,int qmin,int qmax)
{
	//if (m_pAH2k)
	//{
	//	return m_pAH2k->StartVideoStream(nBitrate,nBitrateControlType,qmin,qmax);
	//}
	return -1;

}

int COpAH2K::RequestKeyFrame(void)
{
	//if (m_pAH2k)
	//{
	//	return m_pAH2k->RequestKeyFrame();
	//}
	return -1;

}

int COpAH2K::StopVideoStream(void)
{
	//if (m_pAH2k)
	//{
	//	return m_pAH2k->StopVideoStream();
	//}
	return -1;

}

void COpAH2K::OnVIDEC_AH2KCallbackPreSendImage(int&nSend)
{
	nSend = 1;
}

void COpAH2K::OnVIDEC_AH2KCallbackImage(VIDEC_Image*pImage)
{
	pImage->AddRef();
	XAutoLock l(m_csListImage);
	m_listImage.push_back(pImage);
}

void COpAH2K::OnVIDEC_AH2KCallbackAudioData( short*pSamples,int nSamples,int nSampleRate )
{
	//for (MAP_OPAH2K_CB::iterator it=m_mapCallback.begin(); it!=m_mapCallback.end(); ++it)
	//{
	//	COpAH2KCallback* pCb = it->first;
	//	if (pCb && m_nDevIndex==it->second)
	//	{
	//		pCb->OnOpAH2KCallbackAudioData(pSamples,nSamples,nSampleRate);
	//	}
	//}
}


void COpAH2K::ThreadProcMain(void)
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
			for (MAP_OPAH2K_CB::iterator it=m_mapCallback.begin(); it!=m_mapCallback.end(); ++it)
			{
				COpAH2KCallback* pCb = it->first;
				if (pCb && m_nDevIndex==it->second)
				{
					pCb->OnOpAH2KCallbackImage(pImage);
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
