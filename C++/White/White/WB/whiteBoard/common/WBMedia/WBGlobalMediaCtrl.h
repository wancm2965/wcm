#pragma once

#include "ResourceWBMedia.h"
#include "WBWindows.h"
#include "../WB/whiteBoard/resource.h"
#include "WBToolTip.h"

// CWBGlobalMediaCtrl 对话框
class CWBMediaWindow;
class ImageEx;
class CWBImageSliderControl;
class CWBGlobalMediaCtrl
{
public:
	CWBGlobalMediaCtrl(CWBMedia* pWBMedia = NULL);   // 标准构造函数

	virtual ~CWBGlobalMediaCtrl();

	//进度条右边坐标
	void SetProgressRectRight(int right);

	//视频播放状态
	EMediaPlayState playState;

	//改变当前控制对话框的位置
	void ChangeWindowPos(const CRect &rect);
public:
	//画视频控制框
	void Draw(CDC *pDC, const CRect &rect);		
	
	//鼠标按下进行的操作
	int CtrlDown(CPoint point);	

	//鼠标弹起进行的操作
	int CtrlUp(CPoint point);	

	//判断是否按中按钮 
	int IsInButtonArea(const CPoint& pt); 

	//实时刷新进度条
	void TimerDraw(BOOL isStop = FALSE,BOOL isPause = FALSE);

	void SetMediaEdit(CPoint point);

private:
	//播放
	void ShowPlay(CDC* pDC);

	//播放按下
	void ShowPlayDown(CDC* pDC);

	//暂停
	void ShowPause(CDC* pDC);

	//暂停按下
	void ShowPauseDown(CDC* pDC);

	//停止
	void ShowStop(CDC* pDC);

	//停止按下
	void ShowStopDown(CDC* pDC);

	//循环
	void ShowMediaLoop(CDC* pDC);

	//循环按下
	void ShowMediaLoopDwon(CDC* pDC);

	//单步
	void ShowMediaStep(CDC* pDC);

	//单步按下
	void ShowMediaStepDown(CDC* pDC);

	//截图
	void ShowFrameCapture(CDC* pDC);

	//静音
	void ShowSetMute(CDC* pDC);

	//音量正常
	void ShowNotSetMute(CDC* pDC);

	//按钮显示控制
	void ShowStateContrl(CDC* pDC);

	//视频音量
	int nVolume;
	
	//背景图片
	ImageEx* m_BtnMediaBg;

	//播放按钮
	ImageEx* m_BtnMediaPlay;

	//播放按下按钮
	ImageEx* m_BtnMediaPlayDown;

	//暂停按钮
	ImageEx* m_BtnMediaPause;

	//暂停按下按钮
	ImageEx* m_BtnMediaPauseDown;

	//停止按钮
	ImageEx* m_BtnMediaStop;

	//停止按下按钮
	ImageEx* m_BtnMediaStopDown;

	//循环播放
	ImageEx* m_BtnMediaLoop;

	//循环播放按下按钮
	ImageEx* m_BtnMediaLoopDown;

	//单步按钮
	ImageEx* m_BtnSingalStep;

	//单步按下按钮
	ImageEx* m_BtnSingalStepDown;

	//截图
	ImageEx* m_BtnFrameCapture;

	//静音按钮
	ImageEx* m_BtnMediaSetMute;

	//音量正常按钮
	ImageEx* m_BtnMediaNotMute;

	//播放暂停按钮显示矩形区域
	CRect m_rctMediaPlayPause;

	//停止按钮显示矩形区域
	CRect m_rctMediaStop;

	//单步循环按钮显示矩形区域
	CRect m_rctStepLoop;

	//截图按钮显示区域
	CRect m_rctFrameCapture;

	//音量按钮显示区域
	CRect m_rctVoice;

	CRect m_rctTime;

	//进度滚动条
	Graphics *m_pGraphics;

	//控制的视频指针
	CWBMedia *m_pMedia;

	//工具条
	CTransparentButtonEx m_toolbarBtn;

	//视频对话框大小改变之前的宽度
	int m_preSizeWidth;

	//初始化图片
	void InitBtnImage();

	//停止是否按下
	BOOL m_bMediaStopDown;

	//暂停播放是否按下
	BOOL m_bMediaPlayPauseDown;

	//单步循环是否按下
	BOOL m_bMediaStepLoopDwon;

	//自己所在的区域
	CRect m_posRect;

	//视频暂停时，显示播放时间进度位置
	int m_nPosSecond;
	int m_nPosMinute;

	int m_nPosition;
	int	m_nDuration;
	//记录上一次的时间，将少重复执行
	int m_nPosSecondTime;
	int m_nPosMinuteTime;
	//单曲结束
	BOOL m_bisEndPlay;
protected:
	//停止按钮按下响应
	void OnMediaStop();

	//播放或暂停按钮按下响应
    void OnMediaPlayOrPause();

	//循环播放响应
	void OnMediaLoop();

	//静音响应
	void OnMeidaSetMute();

	//截图响应
	void OnCaptureToBMPFile();

	//进度条控制区域
	CRect progressRect;
	CRect backProgress;
	//声音控制条的显示区域
	CRect proVolume;
	CRect backVolume;

	//播放位置信息（时间为单位）
	unsigned long nPosition;
	unsigned long nDuration;

	bool m_bIsCtrlDownbackProgress;//点在进度条

	unsigned long m_ulPositionPause;
};
