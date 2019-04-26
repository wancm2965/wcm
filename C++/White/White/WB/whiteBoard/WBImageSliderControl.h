#pragma once
#include "stdafx.h"
#include "ImageEx.h"
#include "./common/WBData.h"
//#include "WBData.h"
const BOOL OUT_SLIDER = FALSE;//鼠标点击在进度块外
const BOOL LEFT_SLIDER = 1;  //鼠标点击在进度块左
const BOOL RIGHT_SLIDER = 2; //鼠标点击在进度块右
const BOOL ON_SLIDER = 3;  //鼠标点击在进度块上
const BOOL MARGIN_SLIDER = 4;//在进度条上，但是不再移动槽区域内


class CWBObject;
class ImageEx;

/*名称：横向滑动条类(进度条类) addby:zhuyuyuan*/
class CWBImageSliderControl
{
public:
	CWBImageSliderControl(CWBObject *pWBObject = NULL);
	virtual ~CWBImageSliderControl(void);

public:
	//获取滑块的当前逻辑位置
	int GetPos() const;

	//设置滑块的逻辑位置
	void SetPos(int nPos);

	//获取滑块的最大逻辑值
	int GetRangeMax() const;

	//获取滑块的最小逻辑值
	int GetRangeMin() const;

	//获取滑块的最小和最大逻辑值
	void GetRange(int& nMin,int& nMax)const;

	//设置滑块的最小逻辑值
	void SetRangeMin(int nMin);

	//设置滑块的最大逻辑值
	void SetRangeMax(int nMax);

	//设置滑块的最小和最大逻辑值
	void SetRange(int nMin, int nMax);

	//获取当前拖动的起点和终点
	void GetSelection(int& nMin, int& nMax) const;

	//设置拖动的起点和终点
	void SetSelection(int nMin, int nMax);

	//获取跟踪条（trackbar）的坐标
	//void GetChannelRect(LPRECT lprc) const;
	
	//从依附对象获取控制条坐标
	//void GetCurRectFromWBObject();

	//获取进度条的坐标
	void GetRect(LPRECT lprc);

	//设置进度条的坐标
	void SetRect(RECT& lprc);

	//获取拖动图标的坐标
	//void GetThumbRect(LPRECT lprc);

	
	//设置拖动图标的坐标
	//void SetThumbRect(LPRECT lprc);

	//设置鼠标点击一次移动的的间隔
	void SetTicFreq(int nFreq);

	//获取总的刻度数
	int GetNumTics() const;

	//设置拖动图标的长度
	//void SetThumbLength(int nLength);

	//获取拖动图标的长度
	//int GetThumbLength() const;

	//画进度条
	void Draw(CDC *pDC = NULL);	

	//鼠标按下进行的操作,如果返回TRUE，那么使用者必须刷新窗口
	BOOL MouseDown(const CPoint& point);	

	//按钮移动进行的操作，那么使用者必须刷新窗口
	BOOL MouseMove(const CPoint& point);

	//按钮弹起进行的操作，那么使用者必须刷新窗口
	BOOL MouseUp(const CPoint& point);

	//判断是否按中进度条
	BOOL IsInSliderArea(const CPoint& pt); 

	
	//从文件初始化图片
	void InitImageFromFile(CString bkFileName, CString thumbFileName, CString thumbSelFileName);	

	//从资源初始化图片
	void InitImageFromResource(CString ImgType, CString bkID, CString thumbID, CString thumbSelID);	

	//设置padding的值
	void SetPadding(UINT leftPadding, UINT topPadding, UINT rightPadding, UINT bottomPadding);

	//获取padding的值
	void GetPadding(UINT& leftPadding, UINT& topPadding, UINT& rightPadding, UINT& bottomPadding);
	
	//获取背景的长宽
	BOOL GetSize(UINT& width, UINT& height);

private:
	ImageEx *m_pImgBk;//背景图
	ImageEx *m_pImgThumb;//滑块
	ImageEx *m_pImgThumbSel;//滑块（被选择状态）

	//在背景上限定滑动槽的位置
	UINT m_nLeftPadding;
	UINT m_nRightPadding;
	UINT m_nTopPadding;
	UINT m_nBottomPadding;

	CDC *m_pDC;//保存画图的DC

	//COLORREF	m_crThumbColor;
	//COLORREF	m_crThumbColorSelected;

	CWBObject *m_pWBObject;//应用进度条的对象

	BOOL m_bIsMouseDown;	//记录鼠标是否按下
	BOOL m_bMousePos;//记录鼠标点下的位置:进度条外、滑动块上、滑动块左、滑动块右
	BOOL m_bIsBlockChangeColor;//滑动块是否变色	
	BOOL m_bIsRectSet;//是否设定了滑动块的坐标

	CPoint m_ptBeginPoint; //记录鼠标拖动图标的起点
	CPoint m_ptCurPoint;  //记录鼠标拖动图标的当前点
		

	int m_nCurPos; //当前逻辑位置
	int m_nMaxPos; //逻辑最大值
	int m_nMinPos; //逻辑最小值
	int m_nSelBeginPos;//选择的起点
	int m_nSelEndPos;  //选择的终点
	int m_nTicFreq; //每次点击跳动的逻辑点数

	//在主操作对象创建、移动时候，要修改滑动条的坐标，当鼠标移动滑动块的时候，又要修改滑动块的坐标
	CRect m_rtRect;    //滑动条的坐标
	CRect m_rtThumbRect;//滑块的坐标

	int m_testValue;
};

