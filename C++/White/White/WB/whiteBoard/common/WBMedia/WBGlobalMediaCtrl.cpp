// WBGlobalMediaCtrl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../WB/whiteBoard/WhiteBoardView.h"
#include "WBGlobalMediaCtrl.h"
#include "afxdialogex.h"
#include "ResourceWBMedia.h"
#include "../WB/whiteBoard/common/WBMedia/WBWindows.h"
#include "../Define.h"
#include "../WB/whiteBoard/whiteBoard.h"
#pragma  warning(disable:4244)
// CWBGlobalMediaCtrl �Ի���

CWBGlobalMediaCtrl::CWBGlobalMediaCtrl(CWBMedia* pMedia /*= NULL */ )
	: m_pGraphics(NULL)
	, nVolume(40)
	, m_preSizeWidth(0)
	, m_bMediaPlayPauseDown(FALSE)
	, m_bMediaStopDown(FALSE)
	, m_bMediaStepLoopDwon(FALSE)
	, m_pMedia(pMedia)
	,m_nPosSecond(0)
	,m_nPosMinute(0)
	,m_nPosSecondTime(0)
	,m_nPosMinuteTime(0)
	,m_nPosition(0)
	,m_nDuration(0)
	,m_bisEndPlay(FALSE)
	,m_bIsCtrlDownbackProgress(false)
	,m_ulPositionPause(0)
{
	m_BtnMediaPlay = NULL;
	m_BtnMediaPlayDown = NULL;
	m_BtnMediaPause = NULL;
	m_BtnMediaPauseDown = NULL;
	m_BtnMediaStop = NULL;
	m_BtnMediaStopDown = NULL;
	m_BtnMediaLoop = NULL;
	m_BtnMediaLoopDown = NULL;
	m_BtnSingalStep = NULL;
	m_BtnSingalStepDown = NULL;
	m_BtnFrameCapture = NULL;
	m_BtnMediaNotMute = NULL;
	m_BtnMediaSetMute = NULL;
	m_BtnMediaBg = NULL;
	InitBtnImage();
}

CWBGlobalMediaCtrl::~CWBGlobalMediaCtrl()
{
	if (NULL != m_BtnMediaPlay)
	{
		delete m_BtnMediaPlay;
		m_BtnMediaPlay = NULL;
	}
	if (NULL != m_BtnMediaPlayDown)
	{
		delete m_BtnMediaPlayDown;
		m_BtnMediaPlayDown = NULL;
	}
	if (NULL != m_BtnMediaPause)
	{
		delete m_BtnMediaPause;
		m_BtnMediaPause = NULL;
	}
	if (NULL != m_BtnMediaPauseDown)
	{
		delete m_BtnMediaPauseDown;
		m_BtnMediaPauseDown = NULL;
	}
	if (NULL != m_BtnMediaStop)
	{
		delete m_BtnMediaStop;
		m_BtnMediaStop = NULL;
	}
	if (NULL != m_BtnMediaStopDown)
	{
		delete m_BtnMediaStopDown;
		m_BtnMediaStopDown = NULL;
	}
	if (NULL != m_BtnMediaLoop)
	{
		delete m_BtnMediaLoop;
		m_BtnMediaLoop = NULL;
	}
	if (NULL != m_BtnMediaLoopDown)
	{
		delete m_BtnMediaLoopDown;
		m_BtnMediaLoopDown = NULL;
	}
	if (NULL != m_BtnSingalStep)
	{
		delete m_BtnSingalStep;
		m_BtnSingalStep = NULL;
	}
	if (NULL != m_BtnSingalStepDown)
	{
		delete m_BtnSingalStepDown;
		m_BtnSingalStepDown = NULL;
	}
	if (NULL != m_BtnFrameCapture)
	{
		delete m_BtnFrameCapture;
		m_BtnFrameCapture = NULL;
	}
	if (NULL != m_BtnMediaNotMute)
	{
		delete m_BtnMediaNotMute;
		m_BtnMediaNotMute = NULL;
	}
	if (NULL != m_BtnMediaSetMute)
	{
		delete m_BtnMediaSetMute;
		m_BtnMediaSetMute = NULL;
	}
	if (NULL != m_BtnMediaBg)
	{
		delete m_BtnMediaBg;
		m_BtnMediaBg = NULL;
	}

	if (m_pGraphics != NULL)
	{
		delete m_pGraphics;
		m_pGraphics = NULL;
	}
}

static BOOL inrect = FALSE ;

void CWBGlobalMediaCtrl::Draw(CDC *pDC,const CRect &rect)
{
	ChangeWindowPos(rect);
	m_BtnMediaBg->Draw(pDC->m_hDC, m_posRect);
	ShowStateContrl(pDC);
}

//�ı���ƶԻ���λ�ã��������ŵ�ǰ���Ƶ���Ƶ�Ի����ƶ���������Ӧ�ÿ������Ĳ���
void CWBGlobalMediaCtrl::ChangeWindowPos(const CRect &rect)
{
	//��������Ӧ�ó��ֵ�λ�õľ�������
	int CtrlLeft,CtrlTop,CtrlWidth,CtrlHight,CtrlRight;
	CtrlLeft = rect.left + g_pWhiteBoardView->getXnPos();
	CtrlRight = rect.right + g_pWhiteBoardView->getXnPos();
	CtrlTop = rect.bottom + g_pWhiteBoardView->getYnPos();
	CtrlWidth = rect.Width();
	CtrlHight = 100;
	CtrlTop = CtrlTop - 5;

	m_posRect.SetRect(CtrlLeft, CtrlTop, CtrlLeft + CtrlWidth, CtrlTop + CtrlHight );

	backProgress.right = rect.Width();

	int offset = 0;
	if (CtrlWidth >= 500)
	{
		offset = (CtrlWidth - 500)/2;
	}
	else
	{
		offset = CtrlWidth - 500;
	}
	
	//��ͣ���Ű�ť��ʾ�������
	m_rctMediaPlayPause.SetRect(40 + offset + CtrlLeft, 28 + CtrlTop, 100 + offset + CtrlLeft, 88 + CtrlTop);
	//ֹͣ��ť��ʾ�������
	m_rctMediaStop.SetRect(120 + offset + CtrlLeft, 28 + CtrlTop, 180 + offset + CtrlLeft, 88 + CtrlTop);
	//����ѭ����ť��ʾ�������
	m_rctStepLoop.SetRect(200 + offset + CtrlLeft, 28 + CtrlTop, 260 + offset + CtrlLeft, 88 + CtrlTop);
	//��ͼ��ť��ʾ�������
	m_rctFrameCapture.SetRect(280 + offset + CtrlLeft, 28 + CtrlTop, 340 + offset + CtrlLeft, 88 + CtrlTop);
	//������ť��ʾ�������
	m_rctVoice.SetRect(350 + offset + CtrlLeft, 57 + CtrlTop, 375 + offset + CtrlLeft, 82 + CtrlTop);

	if ( NULL != m_pMedia )
	{
		BOOL bIsCreate = m_pMedia->IsActiveCreated();
		if (bIsCreate)
		{
			//������������ʾ�������
			backProgress.SetRect(CtrlLeft, CtrlTop + 2, CtrlLeft + CtrlWidth - 2, CtrlTop + 13);

			//ʱ����ʾ����
			m_rctTime.SetRect(CtrlLeft + 388 + offset*2, 15 + CtrlTop, CtrlLeft + 488 + offset*2, 35 + CtrlTop);
			

			//����Ӧ�û����Ľ�����λ��
			progressRect = backProgress;
			progressRect.top = backProgress.top + 2;
			progressRect.bottom = backProgress.bottom;

		    nPosition = m_pMedia->GetVedioPosition();
			nDuration = m_pMedia->GetVedioDuration();

			//��ͣ��Ƶ����VIEWˢ��ʱ�򣬽���������ԭ�����ŵĿ̶�wangxiaobin
			if (m_pMedia->GetMediaPlayState() == Media_pausing)
			{
				nPosition = m_nPosition;
				nDuration = m_nDuration;
			}
			if (m_bisEndPlay && m_pMedia->GetMediaLoopState() == FALSE && m_pMedia->GetMediaPlayState() != Media_playing)
			{
				;
			}
			else
			{
				progressRect.right = backProgress.left + static_cast<float>(backProgress.Width()* nPosition )/nDuration;
			}
		}
	} 

	//������������ʾ�������
	backVolume.SetRect(375 + offset + CtrlLeft, 65 + CtrlTop, 475 + offset + CtrlLeft, 75 + CtrlTop );

	//����Ӧ�û�����������������λ��
	proVolume = backVolume;
	proVolume.right = proVolume.left + static_cast<float>(backVolume.Width()*nVolume)/100;	
}

//��ͣ�򲥷Ű�ť���¼���Ӧ
void CWBGlobalMediaCtrl::OnMediaPlayOrPause()
{
	if (NULL != m_pMedia)
	{
		if (m_pMedia->IsActiveCreated())
		{
			if (m_pMedia->GetMediaPlayState() == Media_pausing || m_pMedia->GetMediaPlayState() == Media_stop)
			{
				m_pMedia->MediaPlay();
			} 
			else
				if (m_pMedia->GetMediaPlayState() == Media_playing)
				{
					m_pMedia->MediaPause();
				}
		}
		else
		{
			if (m_pMedia->GetMediaPlayState() == Media_pausing || m_pMedia->GetMediaPlayState() == Media_stop)
			{
				m_pMedia->SetMediaPlayState(Media_playing);
			} 
			else
				if (m_pMedia->GetMediaPlayState() == Media_playing)
				{
					if (g_pWhiteBoardView == NULL)
					{
						return;
					}
					m_pMedia->GetMediaWindow()->Invalidate(TRUE);
					m_pMedia->SetMediaPlayState(Media_pausing);
					m_pMedia->GetMediaWindow()->Invalidate(TRUE);
				}
		}
	}
	m_bMediaPlayPauseDown = FALSE;
	//����ͬ����Ϣ 
	XMemIni xmlObj;
	m_pMedia->SetXmlValue(xmlObj);
	g_pWhiteBoardView->ChangeObj(xmlObj);
}

//ֹͣ��ť��Ϣ��Ӧ
void CWBGlobalMediaCtrl::OnMediaStop()
{
	if (NULL != m_pMedia)
	{
		if (m_pMedia->IsActiveCreated())
		{
			m_pMedia->MediaStop();
			progressRect.right = 0;
		}
		else
		{
			if (g_pWhiteBoardView == NULL)
			{
				return;
			}
			m_pMedia->GetMediaWindow()->Invalidate(TRUE);
			m_pMedia->SetMediaPlayState(Media_stop);
			m_pMedia->GetMediaWindow()->Invalidate(TRUE);
		}
	}
	m_bMediaStopDown = FALSE;
	//����ͬ����Ϣ 
	XMemIni xmlObj;
	m_pMedia->SetXmlValue(xmlObj);
	g_pWhiteBoardView->ChangeObj(xmlObj);
}

//ѭ������
void CWBGlobalMediaCtrl::OnMediaLoop()
{
	if ( NULL != m_pMedia )
	{
		if (m_pMedia->IsActiveCreated())
		{
			nPosition = m_pMedia->GetVedioPosition();
			nDuration = m_pMedia->GetVedioDuration();
			if (nDuration  -1 <= nPosition)
			{
				return;
			}
			m_pMedia->MediaLoop();
		}
		else
		{
			BOOL loop = m_pMedia->GetMediaLoopState();
			m_pMedia->SetMediaLoopState(!loop);
		}
	}
	m_bMediaStepLoopDwon = FALSE;
	//����ͬ����Ϣ
	XMemIni xmlObj;
	m_pMedia->SetXmlValue(xmlObj);
	g_pWhiteBoardView->ChangeObj(xmlObj);
}

//��ͼ
void CWBGlobalMediaCtrl::OnCaptureToBMPFile()
{
 	if (NULL != m_pMedia)
 	{
		if ( !m_pMedia->CaptureToBMPFile())
		{
			return;
		}
 	}
	//����ͬ����Ϣ 
	XMemIni xmlObj;
	m_pMedia->SetXmlValue(xmlObj);
	if (NULL != g_pWhiteBoardView)
	{
		g_pWhiteBoardView->ChangeObj(xmlObj);
	}
	
}

 //����
 void CWBGlobalMediaCtrl::OnMeidaSetMute()
 {
	 if ( NULL != m_pMedia )
	 {
		 if (m_pMedia->IsActiveCreated())
		 {
			 EVoiceState voiceState = m_pMedia->GetVoiceState();
			 int voice = m_pMedia->GetVedioVolume();
			 if (voiceState == VOICEOPEN)
			 {
				 m_pMedia->MediaSetMute(VOICECLOSE);
			 } 
			 else
			 {
				 m_pMedia->MediaSetMute(VOICEOPEN);
				 m_pMedia->SetVedioVolume(voice);
			 }
		 }
		 else
		 {
			  EVoiceState voiceState = m_pMedia->GetVoiceState();
			  if (voiceState == VOICEOPEN)
			  {
				  m_pMedia->SetVoiceState(VOICECLOSE);
			  } 
			  else
			  {
				  m_pMedia->SetVoiceState(VOICEOPEN);
			  }
		 }
	 }
	 //����ͬ����Ϣ 
	 XMemIni xmlObj;
	 m_pMedia->SetXmlValue(xmlObj);
	 g_pWhiteBoardView->ChangeObj(xmlObj);
 }

 //��ť��ʾ����
 void CWBGlobalMediaCtrl::ShowStateContrl( CDC* pDC )
 {
	 if (m_pMedia == NULL)
	 {
		 return;
	 }
	 if (TRUE == m_bMediaPlayPauseDown)
	 {
		 if (m_pMedia->GetMediaPlayState() == Media_playing)
		 {
			 ShowPauseDown(pDC);
		 }
		 else
		 {
			 ShowPlayDown(pDC);
		 }
	 }
	 else
	 {
		 if (m_pMedia->GetMediaPlayState() == Media_playing)
		 {
			 ShowPause(pDC);
		 }
		 else
		 {
			 ShowPlay(pDC);
		 }
	 }
	 if ( TRUE == m_bMediaStopDown)
	 {
		 ShowStopDown(pDC);
	 }
	 else
	 {
		 ShowStop(pDC);
	 }
	 if (TRUE == m_bMediaStepLoopDwon)
	 {
		 if (m_pMedia->GetMediaLoopState() == TRUE)
		 {
			 ShowMediaStepDown(pDC);
		 }
		 else
		 { 
			 ShowMediaLoopDwon(pDC);
		 }
	 }
	 else
	 {
		 if (m_pMedia->GetMediaLoopState() == TRUE)
		 {
			 ShowMediaStep(pDC);
		 }
		 else
		 { 
			 ShowMediaLoop(pDC);
		 }
	 }
	 ShowFrameCapture(pDC);
	 if (m_pMedia->GetVoiceState() == VOICEOPEN)
	 {
		 ShowNotSetMute(pDC);
	 } 
	 else
	 {
		 ShowSetMute(pDC);
	 }


	 m_pGraphics = Graphics::FromHDC(pDC->GetSafeHdc());
	 m_pGraphics->SetSmoothingMode(SmoothingModeHighQuality);
	 
	 if (m_pMedia->GetMediaPlayState() == Media_pausing)
	 {
		 //����Ӧ�û����Ľ�����λ��
		 progressRect = backProgress;
		 progressRect.top = backProgress.top + 2 ;
		 progressRect.bottom = backProgress.bottom;

		 nPosition = m_pMedia->GetVedioPosition();
		 nDuration = m_pMedia->GetVedioDuration();
		 int nPosSecond1 = nPosition % 60;
		 m_nPosSecondTime = nPosSecond1;
		 progressRect.right = backProgress.left + static_cast<float>(backProgress.Width()* nPosition )/nDuration;
		
	 }
	
	 //��������������
	 SolidBrush  BackBrush(Color(255,182,186,184));
	 RectF backRect(progressRect.left+1, progressRect.top+1, backProgress.Width() - 1, backProgress.Height() - 2);
	 m_pGraphics->FillRectangle(&BackBrush, backRect);

	 //������ʾ����
	 RectF rcDuration(progressRect.left + 1, progressRect.top + 1 , progressRect.Width() - 1, progressRect.Height() - 2);
	  SolidBrush  DurBrush(Color(255,29,177,247));
	 m_pGraphics->FillRectangle(&DurBrush,rcDuration);
	 //������������������
	 RectF vicRect(backVolume.left, backVolume.top, backVolume.Width() - 1, backVolume.Height());
	 m_pGraphics->FillRectangle(&BackBrush, vicRect);

	 //������������ʾ����
	 RectF volRect(proVolume.left , proVolume.top  , proVolume.Width(), proVolume.Height() );
	 m_pGraphics->FillRectangle(&DurBrush,volRect);

	 //�����ֵ�ļ���
	 int nDurMinute, nDurSecond;
	 int nPosMinute, nPosSecond;
	 nDurSecond = nDuration % 60;
	 nDurMinute = nDuration / 60;
	 nPosSecond = nPosition % 60;
	 nPosMinute = nPosition / 60;

	 if (m_pMedia->GetMediaPlayState() == Media_pausing)
	 {
		 m_nPosMinute = nPosMinute;
		 m_nPosSecond = nPosSecond;
	 }
	 //��ʾ���ŵ�ǰʱ��
	 nPosMinute = m_nPosMinute;
	 nPosSecond = m_nPosSecond;

	 BOOL bIsCreate = m_pMedia->IsActiveCreated();
	 if (bIsCreate)
	 {
		 //ʱ����ʾ(��ɫ���)
		 RectF rcDurationTime(m_rctTime.left, m_rctTime.top, m_rctTime.Width(), m_rctTime.Height());
		 SolidBrush  DurBrushTime(Color(255,146,177,205));
		 m_pGraphics->FillRectangle(&DurBrushTime,rcDurationTime);

		 //��ʾʱ��
		 CFont font;
		 font.CreatePointFont(100, _T("����"), NULL);
		 pDC->SetBkMode(TRANSPARENT);
		 CString csOut;
		 csOut.Format(_T("%02d")_T(":")_T("%02d")_T("/")_T("%02d")_T(":")_T("%02d"), nPosMinute, nPosSecond, nDurMinute, nDurSecond);
		 pDC->TextOut(m_rctTime.left, m_rctTime.top, csOut, csOut.GetLength());
		 font.DeleteObject();
	 }
 }

 //��ʾ���Ű�ť
 void CWBGlobalMediaCtrl::ShowPlay(CDC* pDC)
 {
	 m_BtnMediaPlay->Draw(pDC->m_hDC, m_rctMediaPlayPause);
 }

 //��ʾ���Ű��°�ť
 void CWBGlobalMediaCtrl::ShowPlayDown( CDC* pDC )
 {
	 m_BtnMediaPlayDown->Draw(pDC->m_hDC, m_rctMediaPlayPause);
 }

 //��ʾ��ͣ��ť
 void CWBGlobalMediaCtrl::ShowPause(CDC* pDC)
 {
	 m_BtnMediaPause->Draw(pDC->m_hDC, m_rctMediaPlayPause);
 }

 //��ʾ��ͣ���°�ť
 void CWBGlobalMediaCtrl::ShowPauseDown( CDC* pDC )
 {
	 m_BtnMediaPauseDown->Draw(pDC->m_hDC, m_rctMediaPlayPause);
 }

 //��ʾֹͣ��ť
 void CWBGlobalMediaCtrl::ShowStop( CDC* pDC )
 {
	 m_BtnMediaStop->Draw(pDC->m_hDC, m_rctMediaStop);
 }

 //��ʾֹͣ���°�ť
 void CWBGlobalMediaCtrl::ShowStopDown( CDC* pDC )
 {
	 m_BtnMediaStopDown->Draw(pDC->m_hDC, m_rctMediaStop);
 }

 //��ʾ�������Ű�ť
 void CWBGlobalMediaCtrl::ShowMediaStep(CDC* pDC)
 {
	 m_BtnSingalStep->Draw(pDC->m_hDC, m_rctStepLoop);
 }

 //��ʾ�������Ű��°�ť
 void CWBGlobalMediaCtrl::ShowMediaStepDown( CDC* pDC )
 {
	 m_BtnSingalStepDown->Draw(pDC->m_hDC, m_rctStepLoop);
 }
 
 //��ʾѭ����ť
 void CWBGlobalMediaCtrl::ShowMediaLoop(CDC* pDC)
 {
	 m_BtnMediaLoop->Draw(pDC->m_hDC, m_rctStepLoop);
 }

 //��ʾѭ�����°�ť
 void CWBGlobalMediaCtrl::ShowMediaLoopDwon( CDC* pDC )
 {
	 m_BtnMediaLoopDown->Draw(pDC->m_hDC, m_rctStepLoop);
 }
 //��ʾ��ͼ
 void CWBGlobalMediaCtrl::ShowFrameCapture( CDC* pDC )
 {
	 m_BtnFrameCapture->Draw(pDC->m_hDC, m_rctFrameCapture);
 }
 //��ʾ������ť
 void CWBGlobalMediaCtrl::ShowSetMute(CDC* pDC)
 {
	 m_BtnMediaSetMute->Draw(pDC->m_hDC,m_rctVoice);
 }
 //��ʾ����������ť
 void CWBGlobalMediaCtrl::ShowNotSetMute(CDC* pDC)
 {
	 m_BtnMediaNotMute->Draw(pDC->m_hDC,m_rctVoice);
 }

//���������£���������
int CWBGlobalMediaCtrl::CtrlDown( CPoint point )
{

	if ( !m_posRect.PtInRect(point) )
	{
		return WB_PS_NONE;
	}

	long nP, nD, nPd ;
	float nPc;
	//��������
	if (backProgress.PtInRect(point))
	{
		//�����ֹͣ״̬����ʲô������
		if (m_pMedia->GetMediaPlayState() == Media_stop)
		{
			return WB_PS_INMEDIACTRL;
		}
		m_bIsCtrlDownbackProgress = true;
		progressRect.right = point.x;
		nD = m_pMedia->GetVedioDuration();
		nPd = backProgress.Width();
		nPc = (float)progressRect.Width() / (float)nPd;
		nP = nPc * (float)nD ;
		if (nP +1 == nD)
		{
			m_pMedia->SeekVedioPosition(nD - 2);
		}
		else
		{
			m_pMedia->SeekVedioPosition(nP);
		}

		//����ͬ����Ϣ 
		XMemIni xmlObj;
		m_pMedia->SetXmlValue(xmlObj);
		g_pWhiteBoardView->ChangeObj(xmlObj);

		return WB_PS_INMEDIACTRL;
	}

	if (backVolume.PtInRect(point))
	{
		//�����ǰ״̬�Ǿ�������ʲô�������������������
		if (m_pMedia->GetVoiceState() == VOICECLOSE)
		{
			return TRUE;
		}
		//��������
		proVolume.right = point.x;
		if (NULL != m_pMedia)
		{
			nVolume = static_cast<float>(proVolume.Width()) * 100 /static_cast<float>(backVolume.Width());
			
			m_pMedia->SetCurVolume(nVolume);
			m_pMedia->SetVedioVolume(nVolume);
		}
		return WB_PS_INMEDIACTRL;
	}
	//������ڲ�����ͣ��ť������
	if (m_rctMediaPlayPause.PtInRect(point))
	{
		m_bMediaPlayPauseDown = TRUE;
		return WB_PS_INMEDIACTRL;
	}
	//�������ֹͣ��ť������
	if (m_rctMediaStop.PtInRect(point))
	{
		m_bMediaStopDown = TRUE;
		return WB_PS_INMEDIACTRL;
	}
	//������ڵ���ѭ����ť��
	if (m_rctStepLoop.PtInRect(point))
	{
		m_bMediaStepLoopDwon = TRUE;
		return WB_PS_INMEDIACTRL;
	}

	if (m_rctFrameCapture.PtInRect(point))
	{
		return WB_PS_INMEDIACTRL;
	}

	if (m_rctVoice.PtInRect(point))
	{
		return WB_PS_INMEDIACTRL; 
	}

	return WB_PS_OBJ;
}

void CWBGlobalMediaCtrl::InitBtnImage()
{
	m_BtnMediaPlay = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_PLAY"));
	m_BtnMediaPlayDown = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_PLAYDOWN"));
	m_BtnMediaPause = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_PAUSE"));
	m_BtnMediaPauseDown = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_DOWNPAUSE"));
	m_BtnMediaStop = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_STOP"));
	m_BtnMediaStopDown = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_DOWNSTOP"));
	m_BtnMediaLoop = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_LOOP"));
	m_BtnMediaLoopDown = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_LOOPDOWN"));
	m_BtnSingalStep = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_SINGLEPLAY"));
	m_BtnSingalStepDown = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_SINGLEPLAY_DOWN"));
	m_BtnFrameCapture = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_FRAMECAPTURE"));
	m_BtnMediaNotMute = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_SPEAKER"));
	m_BtnMediaSetMute = new ImageEx(_T("MEDIABITMAP"), _T("IDB_MEDIA_SPEAKER_MUTE"));
	m_BtnMediaBg = new ImageEx(_T("MEDIABITMAP"),_T("IDB_MEDIABG"));
	m_rctMediaPlayPause.SetRect(40, 28, 100, 88);
	m_rctMediaStop.SetRect(100, 28, 160, 88);
	m_rctStepLoop.SetRect(170, 28, 230, 88);
	m_rctFrameCapture.SetRect(260, 28, 320, 88);
	m_rctVoice.SetRect(350, 57, 375, 82);
}

int CWBGlobalMediaCtrl::CtrlUp( CPoint point )
{
	if ( !m_posRect.PtInRect(point) )
	{
		return WB_PS_NONE;
	}
	if (g_pWhiteBoardView == NULL)
	{
		return 0;
	}
	//������ڽ�������
	if (backProgress.PtInRect(point))
	{
		g_pWhiteBoardView->Invalidate();
		return WB_PS_INMEDIACTRL;
	}
	//������ڲ�����ͣ��ť������
	if (m_rctMediaPlayPause.PtInRect(point))
	{
		inrect = FALSE;
		OnMediaPlayOrPause();
		return WB_PS_INMEDIACTRL;
	}
	//�������ֹͣ��ť������
	if (m_rctMediaStop.PtInRect(point))
	{
		OnMediaStop();
		return WB_PS_INMEDIACTRL;
	}
	//������ڵ���ѭ����ť��
	if (m_rctStepLoop.PtInRect(point))
	{
		inrect = FALSE;
		OnMediaLoop();
		return WB_PS_INMEDIACTRL;
	}
	//������ڽ�ͼ��ť��
	if (m_rctFrameCapture.PtInRect(point))
	{
		OnCaptureToBMPFile();
		return WB_PS_INMEDIACTRL;
	}
	//�������������ť��
	if (m_rctVoice.PtInRect(point))
	{
		OnMeidaSetMute();
		return WB_PS_INMEDIACTRL;
	}
	//�������������С����
	if (backVolume.PtInRect(point))
	{
		g_pWhiteBoardView->Invalidate();
		return WB_PS_INMEDIACTRL;
	}

	return WB_PS_OBJ;
}

void CWBGlobalMediaCtrl::SetProgressRectRight( int right )
{
	progressRect.right = right;
}

int CWBGlobalMediaCtrl::IsInButtonArea( const CPoint& pt )
{
	if (m_posRect.PtInRect(pt))
	{
		int nTemp = CtrlDown(pt);
		return nTemp;
	}
	else
	{
		return WB_PS_NONE;
	}
}

void CWBGlobalMediaCtrl::TimerDraw(BOOL isStop /*= FALSE */,BOOL isPause /*= FALSE */)
{
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	HDC hdc = ::GetDC(g_pWhiteBoardView->GetSafeHwnd());
	Graphics* graph = Graphics::FromHDC(hdc);

 	if (!isStop)
 	{
 		nPosition = m_pMedia->GetVedioPosition();
 		nDuration = m_pMedia->GetVedioDuration();
 	}
	//�����ֵ�ļ���
	int nDurMinute, nDurSecond;
	int nPosMinute, nPosSecond;
	nDurSecond = nDuration % 60;
	nDurMinute = nDuration / 60;
	nPosSecond = nPosition % 60;
	nPosMinute = nPosition / 60;


	//wangxiaobin���ٴ���ִ�д�����������ʾ��ͣʱ����Ƶ�Ĳ��Ž���
 	if (m_nPosSecondTime == nPosSecond && isStop == FALSE)
 	{
		//��ɾ������ֱ�������Դû�ͷ�����wangxiaobin
		::ReleaseDC(g_pWhiteBoardView->GetSafeHwnd(), hdc);
		delete graph;
		graph = NULL;
		return;
 	}

 	m_nPosSecondTime = nPosSecond;
	
	if (isPause == FALSE)
	{
		if (isStop)//�ж��Ƿ����
		{
			m_nPosMinute = nDurMinute;
			m_nPosSecond = nDurSecond;
		}
		else
		{
			m_nPosMinute = nPosMinute;
			m_nPosSecond = nPosSecond;

			m_nPosition = nPosition;
			m_nDuration = nDuration;
		}
		
	}
	else
	{
		nPosSecond = m_nPosSecond;
		nPosMinute = m_nPosMinute;

		nPosition = m_nPosition;
		nDuration = m_nDuration;
	}
	
	if (m_pMedia->IsActiveCreated())
	{
		//ʱ����ʾ
		RectF rcDurationTime(m_rctTime.left - g_pWhiteBoardView->getXnPos(), m_rctTime.top - g_pWhiteBoardView->getYnPos(), m_rctTime.Width(), m_rctTime.Height());
		SolidBrush  DurBrushTime(Color(255,146,177,205));
		graph->FillRectangle(&DurBrushTime,rcDurationTime);

		//��ʾʱ��
		CFont font;
		font.CreatePointFont(100, _T("����"), NULL);
		::SetBkMode(hdc,TRANSPARENT);
		CString csOut;
		csOut.Format(_T("%02d")_T(":")_T("%02d")_T("/")_T("%02d")_T(":")_T("%02d"), nPosMinute, nPosSecond, nDurMinute, nDurSecond);
		::TextOut(hdc,m_rctTime.left - g_pWhiteBoardView->getXnPos(), m_rctTime.top - g_pWhiteBoardView->getYnPos(), csOut, csOut.GetLength());
		font.DeleteObject();
	}

	if (isStop)
	{
		m_bisEndPlay = TRUE;
//  		SolidBrush  BackBrush(Color(255,29,177,247));
//  		RectF backRect(backProgress.left - g_pWhiteBoardView->getXnPos(), backProgress.top - g_pWhiteBoardView->getYnPos() + 2, backProgress.Width() - 1, backProgress.Height());
//  		graph->FillRectangle(&BackBrush, backRect);
//  
//  		SolidBrush  DurBrush(Color(255,29,177,247));
//  		graph->FillRectangle(&DurBrush,backRect);
// 		Sleep(200);
// 		progressRect.right = backProgress.left;
		//��������������
 		SolidBrush  BackBrush(Color(255,182,186,184));
 		RectF backRect(progressRect.left+1 - g_pWhiteBoardView->getXnPos(), progressRect.top + 2 - g_pWhiteBoardView->getYnPos(), backProgress.Width() - 1, backProgress.Height() - 2);
 		graph->FillRectangle(&BackBrush, backRect);
 		//������ʾ����
 		RectF rcDuration(progressRect.left + 1 - g_pWhiteBoardView->getXnPos(), progressRect.top + 2 - g_pWhiteBoardView->getYnPos(), backProgress.Width() - 1, progressRect.Height() - 2);
 		SolidBrush  DurBrush(Color(255,29,177,247));
 		graph->FillRectangle(&DurBrush,rcDuration);
		Sleep(200);

		if (m_pMedia->IsActiveCreated())
		{
			//ʱ����ʾ
			RectF rcDurationTime(m_rctTime.left - g_pWhiteBoardView->getXnPos(), m_rctTime.top - g_pWhiteBoardView->getYnPos(), m_rctTime.Width(), m_rctTime.Height());
			SolidBrush  DurBrushTime(Color(255,146,177,205));
			graph->FillRectangle(&DurBrushTime,rcDurationTime);

			//��ʾʱ��
			CFont font;
			font.CreatePointFont(100, _T("����"), NULL);
			::SetBkMode(hdc,TRANSPARENT);
			CString csOut;
			csOut.Format(_T("%02d")_T(":")_T("%02d")_T("/")_T("%02d")_T(":")_T("%02d"), nDurMinute, nDurSecond, nDurMinute, nDurSecond);
			::TextOut(hdc,m_rctTime.left - g_pWhiteBoardView->getXnPos(), m_rctTime.top - g_pWhiteBoardView->getYnPos(), csOut, csOut.GetLength());
			font.DeleteObject();
		}

		if (m_pMedia->GetMediaLoopState() == FALSE)
		{	
			g_pWhiteBoardView->Invalidate();
		}
	}
	else
	{
		//����Ӧ�û����Ľ�����λ��
		progressRect = backProgress;
		progressRect.top = backProgress.top + 2 ;
		progressRect.bottom = backProgress.bottom;

		nPosition = m_pMedia->GetVedioPosition();
		nDuration = m_pMedia->GetVedioDuration();
	
		progressRect.right = backProgress.left + static_cast<float>(backProgress.Width()* nPosition )/nDuration;

		m_bisEndPlay = FALSE;

		graph->SetSmoothingMode(SmoothingModeHighQuality);
		//��������������
		SolidBrush  BackBrush(Color(255,182,186,184));
		RectF backRect(progressRect.left+1 - g_pWhiteBoardView->getXnPos(), progressRect.top + 2 - g_pWhiteBoardView->getYnPos(), backProgress.Width() - 1, backProgress.Height() - 2);
		graph->FillRectangle(&BackBrush, backRect);
		//������ʾ����
		RectF rcDuration(progressRect.left + 1 - g_pWhiteBoardView->getXnPos(), progressRect.top + 2 - g_pWhiteBoardView->getYnPos(), progressRect.Width() - 1, progressRect.Height() - 2);
		SolidBrush  DurBrush(Color(255,29,177,247));
		graph->FillRectangle(&DurBrush,rcDuration);
		
	}	
	::ReleaseDC(g_pWhiteBoardView->GetSafeHwnd(), hdc);
	delete graph;
	graph = NULL;
}

void CWBGlobalMediaCtrl::SetMediaEdit(CPoint point)
{	
	//������ڲ�����ͣ��ť������
	if (m_rctMediaPlayPause.PtInRect(point))
	{
		if (!inrect)
		{
			if(m_pMedia->GetMediaPlayState() == Media_playing)
			{
				CString str = _T("");
				str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_pause"));
				if (str.IsEmpty())
				{
					str = _T("��ͣ");
				}
				CWBToolTip::GetIns()->SetText(str);
				POINT pt;
				::GetCursorPos(&pt);
				CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
				CWBToolTip::GetIns()->Display();
			}
			if(m_pMedia->GetMediaPlayState() == Media_pausing||m_pMedia->GetMediaPlayState() == Media_stop)
			{
				CString str = _T("");
				str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_play"));
				if (str.IsEmpty())
				{
					str = _T("����");
				}
				CWBToolTip::GetIns()->SetText(str);
				POINT pt;
				::GetCursorPos(&pt);
				CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
				CWBToolTip::GetIns()->Display();
			}
		}
		inrect = TRUE;
	}

	else if (m_rctMediaStop.PtInRect(point))
	{
		if (!inrect)
		{
			CString str = _T("");
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_stop"));
			if (str.IsEmpty())
			{
				str = _T("ֹͣ");
			}
			CWBToolTip::GetIns()->SetText(str);
			POINT pt;
			::GetCursorPos(&pt);
			CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
			CWBToolTip::GetIns()->Display();
		}

		inrect = TRUE;

	}

	else if(m_rctStepLoop.PtInRect(point))
	{
		if (!inrect)
		{
			if (m_pMedia->GetMediaLoopState() == Media_Loop_Play)
			{
				CString str = _T("");
				str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_loop"));
				if (str.IsEmpty())
				{
					str = _T("ѭ��");
				}
				CWBToolTip::GetIns()->SetText(str);
				POINT pt;
				::GetCursorPos(&pt);
				CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
				CWBToolTip::GetIns()->Display();
			}
			else
			{
				CString str = _T("");
				str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_singlePlay"));
				if (str.IsEmpty())
				{
					str = _T("����");
				}
				CWBToolTip::GetIns()->SetText(str);
				POINT pt;
				::GetCursorPos(&pt);
				CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
				CWBToolTip::GetIns()->Display();
			}
		}

		inrect = TRUE;
	}

	else if (m_rctFrameCapture.PtInRect(point))
	{
		if (!inrect)
		{
			CString str = _T("");
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_photograph"));
			if (str.IsEmpty())
			{
				str = _T("����");
			}
			CWBToolTip::GetIns()->SetText(str);
			POINT pt;
			::GetCursorPos(&pt);
			CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
			CWBToolTip::GetIns()->Display();
		}

		inrect = TRUE;
	}

	else if (m_rctVoice.PtInRect(point))
	{
		if (!inrect)
		{
			CString str = _T("");
			str = ReadConfigText(_T("WhiteBoardMenuListText"),_T("movie_voice"));
			if (str.IsEmpty())
			{
				str = _T("����");
			}
			CWBToolTip::GetIns()->SetText(str);
			POINT pt;
			::GetCursorPos(&pt);
			CWBToolTip::GetIns()->SetPosition(CPoint(pt.x+10,pt.y+10));
			CWBToolTip::GetIns()->Display();
		}

		inrect = TRUE;
	}

	else
	{
		inrect = FALSE;
		CWBToolTip::GetIns()->Restore();
	}
}