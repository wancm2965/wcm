#include "decode_Player.h"

bool	g_bRegisterPlayerClass = false;


void*HPAVC_PlayerOpen(HPAVC_PLAYER_CB_DC cbdc,void*pUserData,void* hWnd)
{
	NEWRECORD3( decode_Player* ,pDecode_Player , new decode_Player(cbdc,pUserData) )
	if (pDecode_Player)
	{
		pDecode_Player->Open(hWnd);
	}
	return pDecode_Player;
}

void HPAVC_PlayerClose(void*pHandle)
{
	decode_Player* pDecode_Player = (decode_Player *)pHandle;  
	if (pDecode_Player)
	{
		pDecode_Player->Close();
		delete pDecode_Player;
		pDecode_Player = NULL;
	}
}

int  HPAVC_PlayerPlayData(void*pHandle,void*pData,int nLen)
{
	decode_Player* pDecode_Player = (decode_Player *)pHandle;  
	if (pDecode_Player)
	{
		return pDecode_Player->PlayData(pData,nLen);
	}
	return -1;
}

int  HPAVC_PlayerGetVideoWidth(void*pHandle)
{
	decode_Player* pDecode_Player = (decode_Player *)pHandle;  
	if (pDecode_Player)
	{
		return pDecode_Player->GetVideoWidth();
	}
	return -1;
}

int  HPAVC_PlayerGetVideoHeigh(void*pHandle)
{
	decode_Player* pDecode_Player = (decode_Player *)pHandle;  
	if (pDecode_Player)
	{
		return pDecode_Player->GetVideoHeight();
	}
	return -1;
}

bool HPAVC_PlayerCaptureToBMPFile(void*pHandle,const char*cszFile)
{
	decode_Player* pDecode_Player = (decode_Player *)pHandle;  
	if (pDecode_Player)
	{
		return pDecode_Player->CaptureToBMPFile(cszFile);
	}
	return -1;
}

bool HPAVC_PlayerSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)
{
	decode_Player* pDecode_Player = (decode_Player *)pHandle;  
	if (pDecode_Player)
	{
		return pDecode_Player->SetWindowPos(hWndInsertAfter/*HWND*/, x, y, cx, cy, nFlags);
	}
	return false;
}

void* HPAVC_PlayerGetWnd(void*pHandle)
{
	decode_Player* pDecode_Player = (decode_Player *)pHandle;  
	if (pDecode_Player)
	{
		return pDecode_Player->GetWnd();
	}
	return NULL;
}

int  HPAVC_PlayerSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)
{
	decode_Player* pDecode_Player = (decode_Player *)pHandle;  
	if (pDecode_Player)
	{
		return pDecode_Player->SetVideoParam(nBrightness,  nContrast,  nSaturation,  nHue, nDetail, nDenoise);
	}
	return -1;
}


HPAVC_Player*HPAVC_Player::Create(HPAVC_PlayerCallback*pCallback)
{
	return new decode_Player(pCallback);
}

decode_Player::decode_Player(HPAVC_PLAYER_CB_DC cbdc,void*pUserData)
:m_pCallback(NULL)
,m_pPipeline(NULL)
,m_cbdc(cbdc)
,m_pUserData(pUserData)
,m_pD3DRender11(NULL)
,m_pD3DRender9(NULL)
{
	m_bUseSoftwareDecoder = false;
	m_bUseH263Decoder = false;
}

decode_Player::decode_Player(HPAVC_PlayerCallback*pCallback)
:m_pCallback(pCallback)
,m_pPipeline(NULL)
,m_cbdc(NULL)
,m_pUserData(NULL)
,m_pD3DRender11(NULL)
,m_pD3DRender9(NULL)
{
	m_bUseSoftwareDecoder = false;
	m_bUseH263Decoder = false;
}

decode_Player::~decode_Player(void)
{
}

int decode_Player::Open(void* hWnd/*HWND*/)
{
	int nR = 0;
 	m_pD3DRender11 = HPAVC_D3DRender::Create(this);
 	if (m_pD3DRender11 == NULL)
 	{
 		return -1;
 	}

 	m_pD3DRender11->Open((HWND)hWnd,true,true);
 	if (m_cx > 0)
 	{

 		m_pD3DRender11->SetWindowPos(m_hWndInsertAfter, m_x, m_y, m_cx, m_cy, m_nFlags);
 	}	

	//m_pD3DRender9 = HPAVC_D3DRender::Create(this);
	//if (m_pD3DRender9 == NULL)
	//{
	//	return -1;
	//}
	//m_pD3DRender9->Open(NULL,0,false,false);

	m_nWidth = 0;
	m_nHeight = 0;
	m_bWaitStop = false;
	StartThread();

	m_nLastTS=GetTickCount();
	m_nFrameCount=0;

	return nR;
}

void decode_Player::Close(void)
{
	m_bWaitStop = true;
	WaitForStop();

	{
		XAutoLock l(m_csListPacketData);
		while (m_listPacketData.size())
		{
			XPacketBuffer*pXPacketBuffer=NULL;
			pXPacketBuffer=(XPacketBuffer*)m_listPacketData.front();
			m_listPacketData.pop_front();
			if (pXPacketBuffer)
			{			
				delete pXPacketBuffer;
				pXPacketBuffer=NULL;
			}
		}
	}

	if (m_pPipeline)
	{
		if(m_pD3DRender11)
		{
			m_pD3DRender11->RenderFrameSurface(NULL,NULL);
		}
		m_pPipeline->Close();
		delete m_pPipeline;
		m_pPipeline = NULL;
	}
	if (m_pD3DRender11)
	{
		m_pD3DRender11->Close();
		delete m_pD3DRender11;
		m_pD3DRender11 = NULL;
	}
	if (m_pD3DRender9)
	{
		m_pD3DRender9->Close();
		delete m_pD3DRender9;
		m_pD3DRender9 = NULL;
	}
}

int decode_Player::PlayerTry(unsigned long nWidth,unsigned long nHeight,unsigned short usType)
{
	if (m_nWidth != nWidth || m_nHeight != nHeight)
	{
		if (m_pPipeline)
		{
			if(m_pD3DRender11)
			{
				m_pD3DRender11->RenderFrameSurface(NULL,NULL);
			}
			m_pPipeline->Close();
			delete m_pPipeline;
			m_pPipeline = NULL;
		}
	}
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	if (usType == VIDEC_CODEC_H263 || usType == VIDEC_CODEC_H263P) //插件解
	{
		if (m_bUseH263Decoder == false)
		{
			if (m_pPipeline)
			{
				if(m_pD3DRender11)
				{
					m_pD3DRender11->RenderFrameSurface(NULL,NULL);
				}
				m_pPipeline->Close();
				delete m_pPipeline;
				m_pPipeline = NULL;
			}
			m_bUseH263Decoder = true;
			m_bUseSoftwareDecoder = true;
		}
		if (m_pPipeline == NULL)
		{
			NEWRECORD( m_pPipeline , new CDecodingPipeline(*this) )
			if (m_pPipeline && m_pD3DRender11)
			{
				int rs = m_pPipeline->Open(m_hWnd,MFX_CODEC_H263,m_bUseSoftwareDecoder,m_bUseH263Decoder,m_pD3DRender11,m_pD3DRender9);
				if (rs != MFX_ERR_NONE)
				{
					if(m_pD3DRender11)
					{
						m_pD3DRender11->RenderFrameSurface(NULL,NULL);
					}
					m_pPipeline->Close();
					delete m_pPipeline;
					m_pPipeline = NULL;
				}
				else
				{
					m_pD3DRender11->SetWindowPos(m_hWndInsertAfter, m_x, m_y, m_cx, m_cy, m_nFlags);
				}
			}
		}
	}
	else if (usType == VIDEC_CODEC_H264 || usType == VIDEC_CODEC_H264_SVC)
	{
		//if ((nWidth>=640 && nHeight>=480))	//硬解
		{
			if (m_bUseSoftwareDecoder == true || m_bUseH263Decoder == true)
			{
				if (m_pPipeline)
				{
					if(m_pD3DRender11)
					{
						m_pD3DRender11->RenderFrameSurface(NULL,NULL);
					}
					m_pPipeline->Close();
					delete m_pPipeline;
					m_pPipeline = NULL;
				}
				m_bUseH263Decoder = false;
				m_bUseSoftwareDecoder = false;
			}
			if (m_pPipeline == NULL)
			{
				NEWRECORD( m_pPipeline , new CDecodingPipeline(*this) )
				if (m_pPipeline && m_pD3DRender11)
				{
					int rs = m_pPipeline->Open(m_hWnd,MFX_CODEC_AVC,m_bUseSoftwareDecoder,m_bUseH263Decoder,m_pD3DRender11,m_pD3DRender9);
					if (rs != MFX_ERR_NONE)
					{
						if(m_pD3DRender11)
						{
							m_pD3DRender11->RenderFrameSurface(NULL,NULL);
						}
						m_pPipeline->Close();
						delete m_pPipeline;
						m_pPipeline = NULL;
					}
					else
					{
						m_pD3DRender11->SetWindowPos(m_hWndInsertAfter, m_x, m_y, m_cx, m_cy, m_nFlags);
					}

				}
			}
		}
		/*else
		{
			if (m_bUseSoftwareDecoder == false || m_bUseH263Decoder == true)
			{
				if (m_pPipeline)
				{
					if(m_pD3DRender11)
					{
						m_pD3DRender11->RenderFrameSurface(NULL,NULL);
					}
					m_pPipeline->Close();
					delete m_pPipeline;
					m_pPipeline = NULL;
				}
				m_bUseH263Decoder = false;
				m_bUseSoftwareDecoder = true;
			}
			if (m_pPipeline == NULL)
			{
				m_pPipeline = new CDecodingPipeline(*this);
				if (m_pPipeline)
				{
					int rs = m_pPipeline->Open(m_hWnd,MFX_CODEC_AVC,m_bUseSoftwareDecoder,m_bUseH263Decoder);
					if (rs != MFX_ERR_NONE)
					{
						if(m_pD3DRender11)
						{
						m_pD3DRender11->RenderFrameSurface(NULL,NULL);
						}
						m_pPipeline->Close();
						delete m_pPipeline;
						m_pPipeline = NULL;
					}
					else
					{
						m_pD3DRender11->SetWindowPos(m_hWndInsertAfter, m_x, m_y, m_cx, m_cy, m_nFlags);
					}
				}
			}
		}*/
	}
	else //不支持的解码
	{
		if (m_pPipeline)
		{
			if(m_pD3DRender11)
			{
				m_pD3DRender11->RenderFrameSurface(NULL,NULL);
			}
			m_pPipeline->Close();
			delete m_pPipeline;
			m_pPipeline = NULL;
		}
		m_bUseH263Decoder = false;
		m_bUseSoftwareDecoder = false;
		return -1;
	}
	return 0;
}

void decode_Player::ThreadProcMain(void)
{
	while(!m_bWaitStop)
	{
		int nSize = 0;
		XPacketBuffer*pXPacketBuffer=NULL;
		{
			XAutoLock l(m_csListPacketData);
			nSize = m_listPacketData.size();
			if (nSize)
			{
				pXPacketBuffer=(XPacketBuffer*)m_listPacketData.front();
				m_listPacketData.pop_front();
			}
		}

		if (nSize > 7)
		{
			if (m_pD3DRender11)
			{
				m_pD3DRender11->SetSkipDisplay(4);
			}
		}

		if (pXPacketBuffer!=NULL)
		{				
			int nHeaderLen = VIDEC_HEADER_EXT_GET_LEN(pXPacketBuffer->pBuffer);
			unsigned short usKey = VIDEC_HEADER_EXT_GET_KEYFRAME(pXPacketBuffer->pBuffer);
			if (usKey)
			{
				unsigned short usType = VIDEC_HEADER_EXT_GET_CODEC_ID(pXPacketBuffer->pBuffer);
				unsigned long nWidth = VIDEC_HEADER_EXT_GET_VIRTUAL_WIDTH(pXPacketBuffer->pBuffer);
				unsigned long nHeight = VIDEC_HEADER_EXT_GET_VIRTUAL_HEIGHT(pXPacketBuffer->pBuffer);
				if (nWidth == 0 || nHeight == 0)
				{
					nWidth = VIDEC_HEADER_EXT_GET_ACTUAL_WIDTH(pXPacketBuffer->pBuffer);
					nHeight = VIDEC_HEADER_EXT_GET_ACTUAL_HEIGHT(pXPacketBuffer->pBuffer);
				}
				PlayerTry(nWidth,nHeight,usType);
			}
			if (m_pPipeline)
			{
				m_pPipeline->DecodeFrame((unsigned char*)pXPacketBuffer->pBuffer + nHeaderLen,pXPacketBuffer->nLen - nHeaderLen);
			}

			delete pXPacketBuffer;
			pXPacketBuffer = NULL;
		}
		else
		{
			Sleep(1);
		}
	}
}

int decode_Player::GetVideoWidth(void)
{
	if (m_nWidth !=0)
	{
		return m_nWidth;
	}
	if (m_pPipeline)
	{
		return m_pPipeline->GetVideoWidth();
	}
	return 0;
}

int decode_Player::GetVideoHeight(void)
{
	if (m_nHeight!=0)
	{
		return m_nHeight;
	}
	if (m_pPipeline)
	{
		return m_pPipeline->GetVideoHeight();
	}
	return 0;
}

void* decode_Player::GetWnd(void)
{
	if (m_pD3DRender11)
	{
		return m_pD3DRender11->GetWnd();
	}
	return NULL;
}

bool decode_Player::SetWindowPos(void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)
{
	if (m_pD3DRender11)
	{
		m_pD3DRender11->SetWindowPos(hWndInsertAfter,x,y,cx,cy,nFlags);
	}
	
	m_hWndInsertAfter = hWndInsertAfter; m_x = x; m_y = y; m_cx = cx; m_cy = cy; m_nFlags = nFlags;

	return false;
}

int decode_Player::PlayData(void*pPacketData,int nPacketSize)
{
#if (1)
	{
		m_nFrameCount++;
		if (GetTickCount()-m_nLastTS > 20000)
		{
			char szDebug[1024];
			sprintf(szDebug,"decode_Player::PlayData nFrameCount=%u  this = %u m_listPacketData.size = %u\n",m_nFrameCount,this,m_listPacketData.size());
			DebugOutput( logthread ,szDebug);

			m_nFrameCount=0;
			m_nLastTS=GetTickCount();
		}
	}
#endif
	{
		XAutoLock l(m_csListPacketData);
		NEWRECORD3( XPacketBuffer*,pXPacketBuffer,new XPacketBuffer )
		if (pXPacketBuffer)
		{
			pXPacketBuffer->pBuffer=MALLOCMSG(nPacketSize);
			if (pXPacketBuffer->pBuffer)
			{
				memcpy(pXPacketBuffer->pBuffer,pPacketData,nPacketSize);
				pXPacketBuffer->nLen=nPacketSize;
				m_listPacketData.push_back(pXPacketBuffer);
			}
			else
			{
				delete pXPacketBuffer;
				pXPacketBuffer=NULL;
			}
		}
	}


	return 0;
}

bool decode_Player::CaptureToBMPFile(const char*cszFile)
{
	if (m_pD3DRender11)
	{
		return m_pD3DRender11->CaptureToBMPFile(cszFile);
	}
	return false;
}

int decode_Player::SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)
{
	if (m_pPipeline)
	{
		return m_pPipeline->SetVideoParam(nBrightness, nContrast, nSaturation, nHue,nDetail,nDenoise);
	}
	return 0;
}

void decode_Player::OnDecodingPipelineCallbackDrawDC(void* hDC/*HDC*/)
{

}

void decode_Player::OnHPAVC_D3DRenderCallbackDrawDC(void* hDC)
{
	if (m_pCallback)
	{
		m_pCallback->OnHPAVC_PlayerCallbackDrawDC(hDC);
	}
	if (m_cbdc)
	{
		m_cbdc(m_pUserData,hDC);
	}
}