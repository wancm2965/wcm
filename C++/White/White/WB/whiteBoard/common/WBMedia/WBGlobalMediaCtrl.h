#pragma once

#include "ResourceWBMedia.h"
#include "WBWindows.h"
#include "../WB/whiteBoard/resource.h"
#include "WBToolTip.h"

// CWBGlobalMediaCtrl �Ի���
class CWBMediaWindow;
class ImageEx;
class CWBImageSliderControl;
class CWBGlobalMediaCtrl
{
public:
	CWBGlobalMediaCtrl(CWBMedia* pWBMedia = NULL);   // ��׼���캯��

	virtual ~CWBGlobalMediaCtrl();

	//�������ұ�����
	void SetProgressRectRight(int right);

	//��Ƶ����״̬
	EMediaPlayState playState;

	//�ı䵱ǰ���ƶԻ����λ��
	void ChangeWindowPos(const CRect &rect);
public:
	//����Ƶ���ƿ�
	void Draw(CDC *pDC, const CRect &rect);		
	
	//��갴�½��еĲ���
	int CtrlDown(CPoint point);	

	//��굯����еĲ���
	int CtrlUp(CPoint point);	

	//�ж��Ƿ��а�ť 
	int IsInButtonArea(const CPoint& pt); 

	//ʵʱˢ�½�����
	void TimerDraw(BOOL isStop = FALSE,BOOL isPause = FALSE);

	void SetMediaEdit(CPoint point);

private:
	//����
	void ShowPlay(CDC* pDC);

	//���Ű���
	void ShowPlayDown(CDC* pDC);

	//��ͣ
	void ShowPause(CDC* pDC);

	//��ͣ����
	void ShowPauseDown(CDC* pDC);

	//ֹͣ
	void ShowStop(CDC* pDC);

	//ֹͣ����
	void ShowStopDown(CDC* pDC);

	//ѭ��
	void ShowMediaLoop(CDC* pDC);

	//ѭ������
	void ShowMediaLoopDwon(CDC* pDC);

	//����
	void ShowMediaStep(CDC* pDC);

	//��������
	void ShowMediaStepDown(CDC* pDC);

	//��ͼ
	void ShowFrameCapture(CDC* pDC);

	//����
	void ShowSetMute(CDC* pDC);

	//��������
	void ShowNotSetMute(CDC* pDC);

	//��ť��ʾ����
	void ShowStateContrl(CDC* pDC);

	//��Ƶ����
	int nVolume;
	
	//����ͼƬ
	ImageEx* m_BtnMediaBg;

	//���Ű�ť
	ImageEx* m_BtnMediaPlay;

	//���Ű��°�ť
	ImageEx* m_BtnMediaPlayDown;

	//��ͣ��ť
	ImageEx* m_BtnMediaPause;

	//��ͣ���°�ť
	ImageEx* m_BtnMediaPauseDown;

	//ֹͣ��ť
	ImageEx* m_BtnMediaStop;

	//ֹͣ���°�ť
	ImageEx* m_BtnMediaStopDown;

	//ѭ������
	ImageEx* m_BtnMediaLoop;

	//ѭ�����Ű��°�ť
	ImageEx* m_BtnMediaLoopDown;

	//������ť
	ImageEx* m_BtnSingalStep;

	//�������°�ť
	ImageEx* m_BtnSingalStepDown;

	//��ͼ
	ImageEx* m_BtnFrameCapture;

	//������ť
	ImageEx* m_BtnMediaSetMute;

	//����������ť
	ImageEx* m_BtnMediaNotMute;

	//������ͣ��ť��ʾ��������
	CRect m_rctMediaPlayPause;

	//ֹͣ��ť��ʾ��������
	CRect m_rctMediaStop;

	//����ѭ����ť��ʾ��������
	CRect m_rctStepLoop;

	//��ͼ��ť��ʾ����
	CRect m_rctFrameCapture;

	//������ť��ʾ����
	CRect m_rctVoice;

	CRect m_rctTime;

	//���ȹ�����
	Graphics *m_pGraphics;

	//���Ƶ���Ƶָ��
	CWBMedia *m_pMedia;

	//������
	CTransparentButtonEx m_toolbarBtn;

	//��Ƶ�Ի����С�ı�֮ǰ�Ŀ��
	int m_preSizeWidth;

	//��ʼ��ͼƬ
	void InitBtnImage();

	//ֹͣ�Ƿ���
	BOOL m_bMediaStopDown;

	//��ͣ�����Ƿ���
	BOOL m_bMediaPlayPauseDown;

	//����ѭ���Ƿ���
	BOOL m_bMediaStepLoopDwon;

	//�Լ����ڵ�����
	CRect m_posRect;

	//��Ƶ��ͣʱ����ʾ����ʱ�����λ��
	int m_nPosSecond;
	int m_nPosMinute;

	int m_nPosition;
	int	m_nDuration;
	//��¼��һ�ε�ʱ�䣬�����ظ�ִ��
	int m_nPosSecondTime;
	int m_nPosMinuteTime;
	//��������
	BOOL m_bisEndPlay;
protected:
	//ֹͣ��ť������Ӧ
	void OnMediaStop();

	//���Ż���ͣ��ť������Ӧ
    void OnMediaPlayOrPause();

	//ѭ��������Ӧ
	void OnMediaLoop();

	//������Ӧ
	void OnMeidaSetMute();

	//��ͼ��Ӧ
	void OnCaptureToBMPFile();

	//��������������
	CRect progressRect;
	CRect backProgress;
	//��������������ʾ����
	CRect proVolume;
	CRect backVolume;

	//����λ����Ϣ��ʱ��Ϊ��λ��
	unsigned long nPosition;
	unsigned long nDuration;

	bool m_bIsCtrlDownbackProgress;//���ڽ�����

	unsigned long m_ulPositionPause;
};
