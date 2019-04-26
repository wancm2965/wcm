#pragma once
#include "stdafx.h"
#include "ImageEx.h"
#include "./common/WBData.h"
//#include "WBData.h"
const BOOL OUT_SLIDER = FALSE;//������ڽ��ȿ���
const BOOL LEFT_SLIDER = 1;  //������ڽ��ȿ���
const BOOL RIGHT_SLIDER = 2; //������ڽ��ȿ���
const BOOL ON_SLIDER = 3;  //������ڽ��ȿ���
const BOOL MARGIN_SLIDER = 4;//�ڽ������ϣ����ǲ����ƶ���������


class CWBObject;
class ImageEx;

/*���ƣ����򻬶�����(��������) addby:zhuyuyuan*/
class CWBImageSliderControl
{
public:
	CWBImageSliderControl(CWBObject *pWBObject = NULL);
	virtual ~CWBImageSliderControl(void);

public:
	//��ȡ����ĵ�ǰ�߼�λ��
	int GetPos() const;

	//���û�����߼�λ��
	void SetPos(int nPos);

	//��ȡ���������߼�ֵ
	int GetRangeMax() const;

	//��ȡ�������С�߼�ֵ
	int GetRangeMin() const;

	//��ȡ�������С������߼�ֵ
	void GetRange(int& nMin,int& nMax)const;

	//���û������С�߼�ֵ
	void SetRangeMin(int nMin);

	//���û��������߼�ֵ
	void SetRangeMax(int nMax);

	//���û������С������߼�ֵ
	void SetRange(int nMin, int nMax);

	//��ȡ��ǰ�϶��������յ�
	void GetSelection(int& nMin, int& nMax) const;

	//�����϶��������յ�
	void SetSelection(int nMin, int nMax);

	//��ȡ��������trackbar��������
	//void GetChannelRect(LPRECT lprc) const;
	
	//�����������ȡ����������
	//void GetCurRectFromWBObject();

	//��ȡ������������
	void GetRect(LPRECT lprc);

	//���ý�����������
	void SetRect(RECT& lprc);

	//��ȡ�϶�ͼ�������
	//void GetThumbRect(LPRECT lprc);

	
	//�����϶�ͼ�������
	//void SetThumbRect(LPRECT lprc);

	//���������һ���ƶ��ĵļ��
	void SetTicFreq(int nFreq);

	//��ȡ�ܵĿ̶���
	int GetNumTics() const;

	//�����϶�ͼ��ĳ���
	//void SetThumbLength(int nLength);

	//��ȡ�϶�ͼ��ĳ���
	//int GetThumbLength() const;

	//��������
	void Draw(CDC *pDC = NULL);	

	//��갴�½��еĲ���,�������TRUE����ôʹ���߱���ˢ�´���
	BOOL MouseDown(const CPoint& point);	

	//��ť�ƶ����еĲ�������ôʹ���߱���ˢ�´���
	BOOL MouseMove(const CPoint& point);

	//��ť������еĲ�������ôʹ���߱���ˢ�´���
	BOOL MouseUp(const CPoint& point);

	//�ж��Ƿ��н�����
	BOOL IsInSliderArea(const CPoint& pt); 

	
	//���ļ���ʼ��ͼƬ
	void InitImageFromFile(CString bkFileName, CString thumbFileName, CString thumbSelFileName);	

	//����Դ��ʼ��ͼƬ
	void InitImageFromResource(CString ImgType, CString bkID, CString thumbID, CString thumbSelID);	

	//����padding��ֵ
	void SetPadding(UINT leftPadding, UINT topPadding, UINT rightPadding, UINT bottomPadding);

	//��ȡpadding��ֵ
	void GetPadding(UINT& leftPadding, UINT& topPadding, UINT& rightPadding, UINT& bottomPadding);
	
	//��ȡ�����ĳ���
	BOOL GetSize(UINT& width, UINT& height);

private:
	ImageEx *m_pImgBk;//����ͼ
	ImageEx *m_pImgThumb;//����
	ImageEx *m_pImgThumbSel;//���飨��ѡ��״̬��

	//�ڱ������޶������۵�λ��
	UINT m_nLeftPadding;
	UINT m_nRightPadding;
	UINT m_nTopPadding;
	UINT m_nBottomPadding;

	CDC *m_pDC;//���滭ͼ��DC

	//COLORREF	m_crThumbColor;
	//COLORREF	m_crThumbColorSelected;

	CWBObject *m_pWBObject;//Ӧ�ý������Ķ���

	BOOL m_bIsMouseDown;	//��¼����Ƿ���
	BOOL m_bMousePos;//��¼�����µ�λ��:�������⡢�������ϡ��������󡢻�������
	BOOL m_bIsBlockChangeColor;//�������Ƿ��ɫ	
	BOOL m_bIsRectSet;//�Ƿ��趨�˻����������

	CPoint m_ptBeginPoint; //��¼����϶�ͼ������
	CPoint m_ptCurPoint;  //��¼����϶�ͼ��ĵ�ǰ��
		

	int m_nCurPos; //��ǰ�߼�λ��
	int m_nMaxPos; //�߼����ֵ
	int m_nMinPos; //�߼���Сֵ
	int m_nSelBeginPos;//ѡ������
	int m_nSelEndPos;  //ѡ����յ�
	int m_nTicFreq; //ÿ�ε���������߼�����

	//�����������󴴽����ƶ�ʱ��Ҫ�޸Ļ����������꣬������ƶ��������ʱ����Ҫ�޸Ļ����������
	CRect m_rtRect;    //������������
	CRect m_rtThumbRect;//���������

	int m_testValue;
};

