// WBGlobalMediaCtrl.cpp : 实现文件
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
// CWBGlobalMediaCtrl 对话框

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

//改变控制对话框位置，让它随着当前控制的视频对话框移动，并剪裁应该看不见的部分
void CWBGlobalMediaCtrl::ChangeWindowPos(const CRect &rect)
{
	//计算自身应该出现的位置的矩形区域
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
	
	//暂停播放按钮显示区域计算
	m_rctMediaPlayPause.SetRect(40 + offset + CtrlLeft, 28 + CtrlTop, 100 + offset + CtrlLeft, 88 + CtrlTop);
	//停止按钮显示区域计算
	m_rctMediaStop.SetRect(120 + offset + CtrlLeft, 28 + CtrlTop, 180 + offset + CtrlLeft, 88 + CtrlTop);
	//单步循环按钮显示区域计算
	m_rctStepLoop.SetRect(200 + offset + CtrlLeft, 28 + CtrlTop, 260 + offset + CtrlLeft, 88 + CtrlTop);
	//截图按钮显示区域计算
	m_rctFrameCapture.SetRect(280 + offset + CtrlLeft, 28 + CtrlTop, 340 + offset + CtrlLeft, 88 + CtrlTop);
	//声音按钮显示区域计算
	m_rctVoice.SetRect(350 + offset + CtrlLeft, 57 + CtrlTop, 375 + offset + CtrlLeft, 82 + CtrlTop);

	if ( NULL != m_pMedia )
	{
		BOOL bIsCreate = m_pMedia->IsActiveCreated();
		if (bIsCreate)
		{
			//进度条滑块显示区域计算
			backProgress.SetRect(CtrlLeft, CtrlTop + 2, CtrlLeft + CtrlWidth - 2, CtrlTop + 13);

			//时间显示区域
			m_rctTime.SetRect(CtrlLeft + 388 + offset*2, 15 + CtrlTop, CtrlLeft + 488 + offset*2, 35 + CtrlTop);
			

			//计算应该画出的进度条位置
			progressRect = backProgress;
			progressRect.top = backProgress.top + 2;
			progressRect.bottom = backProgress.bottom;

		    nPosition = m_pMedia->GetVedioPosition();
			nDuration = m_pMedia->GetVedioDuration();

			//暂停视频，在VIEW刷新时候，进度条保存原来播放的刻度wangxiaobin
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

	//声音进度条显示区域计算
	backVolume.SetRect(375 + offset + CtrlLeft, 65 + CtrlTop, 475 + offset + CtrlLeft, 75 + CtrlTop );

	//计算应该画出的声音进度条的位置
	proVolume = backVolume;
	proVolume.right = proVolume.left + static_cast<float>(backVolume.Width()*nVolume)/100;	
}

//暂停或播放按钮的事件响应
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
	//发送同步消息 
	XMemIni xmlObj;
	m_pMedia->SetXmlValue(xmlObj);
	g_pWhiteBoardView->ChangeObj(xmlObj);
}

//停止按钮消息响应
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
	//发送同步消息 
	XMemIni xmlObj;
	m_pMedia->SetXmlValue(xmlObj);
	g_pWhiteBoardView->ChangeObj(xmlObj);
}

//循环播放
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
	//发送同步消息
	XMemIni xmlObj;
	m_pMedia->SetXmlValue(xmlObj);
	g_pWhiteBoardView->ChangeObj(xmlObj);
}

//截图
void CWBGlobalMediaCtrl::OnCaptureToBMPFile()
{
 	if (NULL != m_pMedia)
 	{
		if ( !m_pMedia->CaptureToBMPFile())
		{
			return;
		}
 	}
	//发送同步消息 
	XMemIni xmlObj;
	m_pMedia->SetXmlValue(xmlObj);
	if (NULL != g_pWhiteBoardView)
	{
		g_pWhiteBoardView->ChangeObj(xmlObj);
	}
	
}

 //静音
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
	 //发送同步消息 
	 XMemIni xmlObj;
	 m_pMedia->SetXmlValue(xmlObj);
	 g_pWhiteBoardView->ChangeObj(xmlObj);
 }

 //按钮显示控制
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
		 //计算应该画出的进度条位置
		 progressRect = backProgress;
		 progressRect.top = backProgress.top + 2 ;
		 progressRect.bottom = backProgress.bottom;

		 nPosition = m_pMedia->GetVedioPosition();
		 nDuration = m_pMedia->GetVedioDuration();
		 int nPosSecond1 = nPosition % 60;
		 m_nPosSecondTime = nPosSecond1;
		 progressRect.right = backProgress.left + static_cast<float>(backProgress.Width()* nPosition )/nDuration;
		
	 }
	
	 //进度条背景区域
	 SolidBrush  BackBrush(Color(255,182,186,184));
	 RectF backRect(progressRect.left+1, progressRect.top+1, backProgress.Width() - 1, backProgress.Height() - 2);
	 m_pGraphics->FillRectangle(&BackBrush, backRect);

	 //进度显示区域
	 RectF rcDuration(progressRect.left + 1, progressRect.top + 1 , progressRect.Width() - 1, progressRect.Height() - 2);
	  SolidBrush  DurBrush(Color(255,29,177,247));
	 m_pGraphics->FillRectangle(&DurBrush,rcDuration);
	 //声音控制条背景区域
	 RectF vicRect(backVolume.left, backVolume.top, backVolume.Width() - 1, backVolume.Height());
	 m_pGraphics->FillRectangle(&BackBrush, vicRect);

	 //声音控制条显示区域
	 RectF volRect(proVolume.left , proVolume.top  , proVolume.Width(), proVolume.Height() );
	 m_pGraphics->FillRectangle(&DurBrush,volRect);

	 //输出数值的计算
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
	 //显示播放当前时间
	 nPosMinute = m_nPosMinute;
	 nPosSecond = m_nPosSecond;

	 BOOL bIsCreate = m_pMedia->IsActiveCreated();
	 if (bIsCreate)
	 {
		 //时间显示(颜色填充)
		 RectF rcDurationTime(m_rctTime.left, m_rctTime.top, m_rctTime.Width(), m_rctTime.Height());
		 SolidBrush  DurBrushTime(Color(255,146,177,205));
		 m_pGraphics->FillRectangle(&DurBrushTime,rcDurationTime);

		 //显示时间
		 CFont font;
		 font.CreatePointFont(100, _T("宋体"), NULL);
		 pDC->SetBkMode(TRANSPARENT);
		 CString csOut;
		 csOut.Format(_T("%02d")_T(":")_T("%02d")_T("/")_T("%02d")_T(":")_T("%02d"), nPosMinute, nPosSecond, nDurMinute, nDurSecond);
		 pDC->TextOut(m_rctTime.left, m_rctTime.top, csOut, csOut.GetLength());
		 font.DeleteObject();
	 }
 }

 //显示播放按钮
 void CWBGlobalMediaCtrl::ShowPlay(CDC* pDC)
 {
	 m_BtnMediaPlay->Draw(pDC->m_hDC, m_rctMediaPlayPause);
 }

 //显示播放按下按钮
 void CWBGlobalMediaCtrl::ShowPlayDown( CDC* pDC )
 {
	 m_BtnMediaPlayDown->Draw(pDC->m_hDC, m_rctMediaPlayPause);
 }

 //显示暂停按钮
 void CWBGlobalMediaCtrl::ShowPause(CDC* pDC)
 {
	 m_BtnMediaPause->Draw(pDC->m_hDC, m_rctMediaPlayPause);
 }

 //显示暂停按下按钮
 void CWBGlobalMediaCtrl::ShowPauseDown( CDC* pDC )
 {
	 m_BtnMediaPauseDown->Draw(pDC->m_hDC, m_rctMediaPlayPause);
 }

 //显示停止按钮
 void CWBGlobalMediaCtrl::ShowStop( CDC* pDC )
 {
	 m_BtnMediaStop->Draw(pDC->m_hDC, m_rctMediaStop);
 }

 //显示停止按下按钮
 void CWBGlobalMediaCtrl::ShowStopDown( CDC* pDC )
 {
	 m_BtnMediaStopDown->Draw(pDC->m_hDC, m_rctMediaStop);
 }

 //显示单步播放按钮
 void CWBGlobalMediaCtrl::ShowMediaStep(CDC* pDC)
 {
	 m_BtnSingalStep->Draw(pDC->m_hDC, m_rctStepLoop);
 }

 //显示单步播放按下按钮
 void CWBGlobalMediaCtrl::ShowMediaStepDown( CDC* pDC )
 {
	 m_BtnSingalStepDown->Draw(pDC->m_hDC, m_rctStepLoop);
 }
 
 //显示循环按钮
 void CWBGlobalMediaCtrl::ShowMediaLoop(CDC* pDC)
 {
	 m_BtnMediaLoop->Draw(pDC->m_hDC, m_rctStepLoop);
 }

 //显示循环按下按钮
 void CWBGlobalMediaCtrl::ShowMediaLoopDwon( CDC* pDC )
 {
	 m_BtnMediaLoopDown->Draw(pDC->m_hDC, m_rctStepLoop);
 }
 //显示截图
 void CWBGlobalMediaCtrl::ShowFrameCapture( CDC* pDC )
 {
	 m_BtnFrameCapture->Draw(pDC->m_hDC, m_rctFrameCapture);
 }
 //显示静音按钮
 void CWBGlobalMediaCtrl::ShowSetMute(CDC* pDC)
 {
	 m_BtnMediaSetMute->Draw(pDC->m_hDC,m_rctVoice);
 }
 //显示正常音量按钮
 void CWBGlobalMediaCtrl::ShowNotSetMute(CDC* pDC)
 {
	 m_BtnMediaNotMute->Draw(pDC->m_hDC,m_rctVoice);
 }

//鼠标左键按下，调整进度
int CWBGlobalMediaCtrl::CtrlDown( CPoint point )
{

	if ( !m_posRect.PtInRect(point) )
	{
		return WB_PS_NONE;
	}

	long nP, nD, nPd ;
	float nPc;
	//调整进度
	if (backProgress.PtInRect(point))
	{
		//如果是停止状态，则什么都不做
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

		//发送同步消息 
		XMemIni xmlObj;
		m_pMedia->SetXmlValue(xmlObj);
		g_pWhiteBoardView->ChangeObj(xmlObj);

		return WB_PS_INMEDIACTRL;
	}

	if (backVolume.PtInRect(point))
	{
		//如果当前状态是静音，则什么都不做，否则调整音量
		if (m_pMedia->GetVoiceState() == VOICECLOSE)
		{
			return TRUE;
		}
		//调整音量
		proVolume.right = point.x;
		if (NULL != m_pMedia)
		{
			nVolume = static_cast<float>(proVolume.Width()) * 100 /static_cast<float>(backVolume.Width());
			
			m_pMedia->SetCurVolume(nVolume);
			m_pMedia->SetVedioVolume(nVolume);
		}
		return WB_PS_INMEDIACTRL;
	}
	//如果点在播放暂停按钮区域上
	if (m_rctMediaPlayPause.PtInRect(point))
	{
		m_bMediaPlayPauseDown = TRUE;
		return WB_PS_INMEDIACTRL;
	}
	//如果点在停止按钮区域上
	if (m_rctMediaStop.PtInRect(point))
	{
		m_bMediaStopDown = TRUE;
		return WB_PS_INMEDIACTRL;
	}
	//如果点在单步循环按钮上
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
	//如果点在进度条上
	if (backProgress.PtInRect(point))
	{
		g_pWhiteBoardView->Invalidate();
		return WB_PS_INMEDIACTRL;
	}
	//如果点在播放暂停按钮区域上
	if (m_rctMediaPlayPause.PtInRect(point))
	{
		inrect = FALSE;
		OnMediaPlayOrPause();
		return WB_PS_INMEDIACTRL;
	}
	//如果点在停止按钮区域上
	if (m_rctMediaStop.PtInRect(point))
	{
		OnMediaStop();
		return WB_PS_INMEDIACTRL;
	}
	//如果点在单步循环按钮上
	if (m_rctStepLoop.PtInRect(point))
	{
		inrect = FALSE;
		OnMediaLoop();
		return WB_PS_INMEDIACTRL;
	}
	//如果点在截图按钮上
	if (m_rctFrameCapture.PtInRect(point))
	{
		OnCaptureToBMPFile();
		return WB_PS_INMEDIACTRL;
	}
	//如果点在声音按钮上
	if (m_rctVoice.PtInRect(point))
	{
		OnMeidaSetMute();
		return WB_PS_INMEDIACTRL;
	}
	//如果点在音量大小条上
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
	//输出数值的计算
	int nDurMinute, nDurSecond;
	int nPosMinute, nPosSecond;
	nDurSecond = nDuration % 60;
	nDurMinute = nDuration / 60;
	nPosSecond = nPosition % 60;
	nPosMinute = nPosition / 60;


	//wangxiaobin将少代码执行次数，用来显示暂停时候视频的播放进度
 	if (m_nPosSecondTime == nPosSecond && isStop == FALSE)
 	{
		//不删除会出现奔溃。资源没释放引起wangxiaobin
		::ReleaseDC(g_pWhiteBoardView->GetSafeHwnd(), hdc);
		delete graph;
		graph = NULL;
		return;
 	}

 	m_nPosSecondTime = nPosSecond;
	
	if (isPause == FALSE)
	{
		if (isStop)//判断是否到最后
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
		//时间显示
		RectF rcDurationTime(m_rctTime.left - g_pWhiteBoardView->getXnPos(), m_rctTime.top - g_pWhiteBoardView->getYnPos(), m_rctTime.Width(), m_rctTime.Height());
		SolidBrush  DurBrushTime(Color(255,146,177,205));
		graph->FillRectangle(&DurBrushTime,rcDurationTime);

		//显示时间
		CFont font;
		font.CreatePointFont(100, _T("宋体"), NULL);
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
		//进度条背景区域
 		SolidBrush  BackBrush(Color(255,182,186,184));
 		RectF backRect(progressRect.left+1 - g_pWhiteBoardView->getXnPos(), progressRect.top + 2 - g_pWhiteBoardView->getYnPos(), backProgress.Width() - 1, backProgress.Height() - 2);
 		graph->FillRectangle(&BackBrush, backRect);
 		//进度显示区域
 		RectF rcDuration(progressRect.left + 1 - g_pWhiteBoardView->getXnPos(), progressRect.top + 2 - g_pWhiteBoardView->getYnPos(), backProgress.Width() - 1, progressRect.Height() - 2);
 		SolidBrush  DurBrush(Color(255,29,177,247));
 		graph->FillRectangle(&DurBrush,rcDuration);
		Sleep(200);

		if (m_pMedia->IsActiveCreated())
		{
			//时间显示
			RectF rcDurationTime(m_rctTime.left - g_pWhiteBoardView->getXnPos(), m_rctTime.top - g_pWhiteBoardView->getYnPos(), m_rctTime.Width(), m_rctTime.Height());
			SolidBrush  DurBrushTime(Color(255,146,177,205));
			graph->FillRectangle(&DurBrushTime,rcDurationTime);

			//显示时间
			CFont font;
			font.CreatePointFont(100, _T("宋体"), NULL);
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
		//计算应该画出的进度条位置
		progressRect = backProgress;
		progressRect.top = backProgress.top + 2 ;
		progressRect.bottom = backProgress.bottom;

		nPosition = m_pMedia->GetVedioPosition();
		nDuration = m_pMedia->GetVedioDuration();
	
		progressRect.right = backProgress.left + static_cast<float>(backProgress.Width()* nPosition )/nDuration;

		m_bisEndPlay = FALSE;

		graph->SetSmoothingMode(SmoothingModeHighQuality);
		//进度条背景区域
		SolidBrush  BackBrush(Color(255,182,186,184));
		RectF backRect(progressRect.left+1 - g_pWhiteBoardView->getXnPos(), progressRect.top + 2 - g_pWhiteBoardView->getYnPos(), backProgress.Width() - 1, backProgress.Height() - 2);
		graph->FillRectangle(&BackBrush, backRect);
		//进度显示区域
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
	//如果点在播放暂停按钮区域上
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
					str = _T("暂停");
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
					str = _T("播放");
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
				str = _T("停止");
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
					str = _T("循环");
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
					str = _T("单放");
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
				str = _T("拍照");
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
				str = _T("音量");
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