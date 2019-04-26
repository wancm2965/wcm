#ifndef __UIBUBBLETIP_H__
#define __UIBUBBLETIP_H__
#include <CoUI/Utils/Utils.h>
/*************************************************************\
*简介：该类提供像素级透明窗体支持，可用于提示窗口等高质量显示的小窗口*
*场合，若窗口太大则效率低，且资源消耗大，不建议使用。                *
*功能：提供简单的提示显示功能，对于鼠标操作或者其他需求可以通过继承实*
*现支持                                                              *
\*************************************************************/

//该类为窗口类，窗口与对象同存亡！
namespace CoUI
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

class UILIB_API CRichWnd
{
public:
	CRichWnd(HWND hParent = NULL,RECT *rt = NULL,BYTE wAlpha = 255);//hParent暂时无用
	virtual ~CRichWnd();
	void Invalidate(bool bImmediately = FALSE);//刷新窗口
	void SetWAlpha(BYTE wAlpha);//设置窗口透明度
	BYTE GetWAlpha();
	virtual operator HWND() { return m_hWnd; };
	static BOOL IsRichWnd(CRichWnd *richWnd);//判断一个窗口对象指针是否有效，即该对象是否存在
	static BOOL DestroyRichWnd(CRichWnd *richWnd);//安全销毁窗口对象,当只有窗口对象指针时建议使用这个函数销毁对象
protected:
	virtual void OnPaint(HDC hDC) = 0;
	virtual LRESULT MessageHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL RenderRichWnd(HWND hWnd,HDC hDCdes,POINT *ptDes,SIZE *size,HDC hDCsrc,POINT *ptSrc,COLORREF color,BLENDFUNCTION *blen,DWORD flag);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void DrawTextWithAlpha(HDC hDC, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, HFONT font, UINT uStyle,BOOL bRich = FALSE);//bRich,文字颜色是否支持渐变过渡(不支持透明度渐变)；暂不可用
	static void DrawThemeText(HWND hwnd,HDC hDC, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, HFONT font, UINT uStyle, int iGlowSize);
	LRESULT UserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
	static CStdPtrArray *g_WndList;
	BYTE m_wAlpha;			//窗口透明度
	HWND m_hParent;
	HWND m_hWnd;
	RECT m_wPos;
};


#define UWM_BUBBLETIP_LINK WM_USER+17

class UILIB_API CBubbleTip:public CRichWnd//气泡提示
{
public:
	//参数介绍：
	//pt-指定提示尖角指向的位置
	//text-提示的文字内容
	//size-指定提示框大小，要换行请插入'\n'换行符（宽度为0时自动调整宽度和高度，高度为0时自动调整高度）
	//AnglePos-指定尖角的位置，x和y取值都是0-1000(映射提示框坐标，左上角为(0,0)，右下角为(1000,1000)，不在边界上的坐标无效)
	//hFont-设置字体(该参数暂无效，字体请在text中按html语法配置)
	//hParent-父窗口句柄
	//time-指定延时销毁，单位是0.1秒,销毁窗口的同时也会销毁该对象
	CBubbleTip(POINT pt,CStdString text,SIZE *size = NULL,POINT *AnglePos = NULL,HFONT hFont = NULL,HWND hParent = NULL,UINT time = 0,BYTE wAlpha = 255);
	~CBubbleTip();
protected:
	enum TIPTYPE {//窗口尖角的方向
		TIPANGLE_LEFT = 1,
		TIPANGLE_UP,
		TIPANGLE_RIGHT,
		TIPANGLE_DOWN
	};
	HBITMAP GetTipBmp(BOOL bClear = FALSE,BOOL bRegister = FALSE);//获取17x21的位图提示框，当bClear为TRUE时注销使用局部资源,bRegister为TRUE注册使用局部资源
	HBITMAP GetAngleBmp( TIPTYPE tipType );
	void SetZero(int offx = 0, int offy = 0);
	void OnPaint(HDC hDC);
	int GetTextRect(RECT *rt, CStdString str,HFONT hFont = NULL);
	int DrawHtmlText(HDC hDC,RECT *rt, CStdString str,HFONT hFont = NULL,BOOL bCalculate = FALSE);
	void MoveWindow(POINT *pt = NULL,BOOL bShow = TRUE);
	void InitializeTip(SIZE *size,POINT *AnglePos);
	LRESULT MessageHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
	enum
	{
		BUBBLETIP_TIMER_ID = 17//提示类用定时器ID
	};

	struct LINK {
		RECT rt;
		CStdString LinkContent;
	};
	CStdPtrArray m_LinkList;//<链接>链表
	int m_pLinkID;
	TIPTYPE m_tipType;
	POINT m_AnglePos;//尖角的位置
	HFONT m_hFont;//全局字体
	RECT m_textRt;//文本的绘制区域
	CStdString m_tipStr;//文本内容
	POINT m_OffSet;//辅助定位用的偏移量
};
}// namespace CoUI

#endif // __UIANIMLABEL_H__