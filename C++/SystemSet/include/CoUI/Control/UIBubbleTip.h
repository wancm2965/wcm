#ifndef __UIBUBBLETIP_H__
#define __UIBUBBLETIP_H__
#include <CoUI/Utils/Utils.h>
/*************************************************************\
*��飺�����ṩ���ؼ�͸������֧�֣���������ʾ���ڵȸ�������ʾ��С����*
*���ϣ�������̫����Ч�ʵͣ�����Դ���Ĵ󣬲�����ʹ�á�                *
*���ܣ��ṩ�򵥵���ʾ��ʾ���ܣ��������������������������ͨ���̳�ʵ*
*��֧��                                                              *
\*************************************************************/

//����Ϊ�����࣬���������ͬ������
namespace CoUI
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

class UILIB_API CRichWnd
{
public:
	CRichWnd(HWND hParent = NULL,RECT *rt = NULL,BYTE wAlpha = 255);//hParent��ʱ����
	virtual ~CRichWnd();
	void Invalidate(bool bImmediately = FALSE);//ˢ�´���
	void SetWAlpha(BYTE wAlpha);//���ô���͸����
	BYTE GetWAlpha();
	virtual operator HWND() { return m_hWnd; };
	static BOOL IsRichWnd(CRichWnd *richWnd);//�ж�һ�����ڶ���ָ���Ƿ���Ч�����ö����Ƿ����
	static BOOL DestroyRichWnd(CRichWnd *richWnd);//��ȫ���ٴ��ڶ���,��ֻ�д��ڶ���ָ��ʱ����ʹ������������ٶ���
protected:
	virtual void OnPaint(HDC hDC) = 0;
	virtual LRESULT MessageHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL RenderRichWnd(HWND hWnd,HDC hDCdes,POINT *ptDes,SIZE *size,HDC hDCsrc,POINT *ptSrc,COLORREF color,BLENDFUNCTION *blen,DWORD flag);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void DrawTextWithAlpha(HDC hDC, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, HFONT font, UINT uStyle,BOOL bRich = FALSE);//bRich,������ɫ�Ƿ�֧�ֽ������(��֧��͸���Ƚ���)���ݲ�����
	static void DrawThemeText(HWND hwnd,HDC hDC, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, HFONT font, UINT uStyle, int iGlowSize);
	LRESULT UserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
	static CStdPtrArray *g_WndList;
	BYTE m_wAlpha;			//����͸����
	HWND m_hParent;
	HWND m_hWnd;
	RECT m_wPos;
};


#define UWM_BUBBLETIP_LINK WM_USER+17

class UILIB_API CBubbleTip:public CRichWnd//������ʾ
{
public:
	//�������ܣ�
	//pt-ָ����ʾ���ָ���λ��
	//text-��ʾ����������
	//size-ָ����ʾ���С��Ҫ���������'\n'���з������Ϊ0ʱ�Զ�������Ⱥ͸߶ȣ��߶�Ϊ0ʱ�Զ������߶ȣ�
	//AnglePos-ָ����ǵ�λ�ã�x��yȡֵ����0-1000(ӳ����ʾ�����꣬���Ͻ�Ϊ(0,0)�����½�Ϊ(1000,1000)�����ڱ߽��ϵ�������Ч)
	//hFont-��������(�ò�������Ч����������text�а�html�﷨����)
	//hParent-�����ھ��
	//time-ָ����ʱ���٣���λ��0.1��,���ٴ��ڵ�ͬʱҲ�����ٸö���
	CBubbleTip(POINT pt,CStdString text,SIZE *size = NULL,POINT *AnglePos = NULL,HFONT hFont = NULL,HWND hParent = NULL,UINT time = 0,BYTE wAlpha = 255);
	~CBubbleTip();
protected:
	enum TIPTYPE {//���ڼ�ǵķ���
		TIPANGLE_LEFT = 1,
		TIPANGLE_UP,
		TIPANGLE_RIGHT,
		TIPANGLE_DOWN
	};
	HBITMAP GetTipBmp(BOOL bClear = FALSE,BOOL bRegister = FALSE);//��ȡ17x21��λͼ��ʾ�򣬵�bClearΪTRUEʱע��ʹ�þֲ���Դ,bRegisterΪTRUEע��ʹ�þֲ���Դ
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
		BUBBLETIP_TIMER_ID = 17//��ʾ���ö�ʱ��ID
	};

	struct LINK {
		RECT rt;
		CStdString LinkContent;
	};
	CStdPtrArray m_LinkList;//<����>����
	int m_pLinkID;
	TIPTYPE m_tipType;
	POINT m_AnglePos;//��ǵ�λ��
	HFONT m_hFont;//ȫ������
	RECT m_textRt;//�ı��Ļ�������
	CStdString m_tipStr;//�ı�����
	POINT m_OffSet;//������λ�õ�ƫ����
};
}// namespace CoUI

#endif // __UIANIMLABEL_H__