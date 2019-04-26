#pragma once

//#include "InertiaObj.h"
#include "D2DDriver.h"
#include <wincodec.h>
#include <math.h>
#include <string>
#include <vector>
#include <Shlobj.h>
#include <ShellAPI.h>
#include "Define.h"
#include "DrapEffect.h"
#include "MTGestureImport.h"
#include "ID2D1BmpAtlas.h"
#define  SHOW_DELICON_RANGE 60
#define  PREVENT_DEL_RANGE  20
#define SNAP_THRESHOLD 2
#define FALLBACK_COEF (0.1f)
//�����Ƴ��߽�������Ƴ��߽�
#define UWM_DRAGOUT_RESOUCE		(WM_USER + 3333) 
#define UWM_DELFILE_OPEN        (WM_USER + 5555)
#define ICON_SIZE (96)
extern BOOL g_bShowDlg;

#ifndef _SAFERELEASE
#define _SAFERELEASE
template<class Interface>//
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}
#endif
//������
class CWBThumbnail : public AVCON_SDK_MTTouch::CMTObject
{
public:
	CWBThumbnail(HWND hWnd, CD2DDriver* d2dDriver, 
		float fpXPos, float fpYPos, float fpHeight, float fpXWndPos);
	virtual ~CWBThumbnail();

	virtual void OnGestureCombineHandle(const GestureCombinePar &gesPar);
	virtual bool BInRegion(const POINT &pt)const;
	virtual void OnCallbackGesture(const unsigned int &nMessage, 
		const unsigned int &nAppendMessage = 0);

	virtual void CleanUp(){delete this;};
public:
	unsigned int m_id;
	BOOL m_bisVideoFile ; //�ļ��Ƿ��������ɾ��
	BOOL m_bHorDirection;
	BOOL m_isDir;
	BOOL m_bSelect;
	//��Դ�����ھ��
    HWND m_hAttachWnd;
	//���ļ��м���λͼ
	HRESULT LoadBitmapFromFile(LPCTSTR uri);

	HRESULT LoadBitmapFromFileXP(LPCTSTR const uri);

	static HBITMAP LoadPictureFromFile(LPCTSTR,CD2DDriver *d2DDriver,PVOID *pDate = NULL,IWICBitmapSource **pSour = NULL);
	static void ClearAtlas();

	HRESULT GetItemBmp(LPCTSTR const str);
	void SetFilePath(LPCTSTR const uri);
	void SetFileName(LPCTSTR const fName)
	{
		m_strThumbName = tstring(fName);
	}

	void Render();
	BOOL InRegion(const float &x, const float &y)const;
	virtual void Translate(float x, float y, bool bInner = false);
	//
	virtual HRESULT Initialize();
	BOOL GetLastTouchInput(TOUCHINPUT *pInput);

	BOOL IsPastBounds();
	float GetHeight() { return m_fpHeight; };
	float GetWidth() { return m_fpWidth; };
	float GetYOffset();
	float GetXOffset();
	float SetXOffsetPos(float fXPos);
	LPCTSTR GetFileURI() { return m_strThumbURI.c_str(); }
	HBITMAP DrapIcon(BOOL bClear = FALSE);//bClearΪTRUE����վֲ���Դ
	LPCTSTR GetThumbName(){return m_strThumbName.c_str();}
	POINT GetThumbPosition(){return m_ptIcon;}
	//
	bool DeleteFile(LPCTSTR filePath);
public:
	void SetConstraints(RECT constraints) { m_constraints = constraints; }
	RECT m_constraints;
//	void SetBounds(RECT bounds) { m_bounds = bounds; }
	RECT m_bounds;

	const HWND GetHWnd()const { return m_hAttachWnd; }
	TOUCHINPUT CWBThumbnail::GetLastTouchInput() { return m_lastTouchInput; }

	//У׼��(�����������е�λ�ý���ˮƽ�����ϵ�������������ˮƽ����ĵ�����)
	void CalibrationXPosition(float fXOffSet);
	//����Ypos
	void SetYOffsetPos(float fYPos = 0.0);

	HMODULE m_hDialog;
public:
	//����Y��������
	void ResetYOffSet();

protected:
	float m_fpHeight, m_fpWidth, m_fpYPos , m_fpXWndPos;
	float m_fpYPosConst;//�������ʼ��Y����ƫ������Ϊ�����ã�
	TOUCHINPUT m_lastTouchInput;

	CD2DDriver* m_d2dDriver;
	D2D1::Matrix3x2F m_matrix;
	D2D1::Matrix3x2F m_MatrixTranslate;
	D2D1::Matrix3x2F m_MatrixFinal;

	HBITMAP m_IconImage;
	POINT m_FirstPoint;
	POINT m_ptIcon;

	float m_fTotalAngle;
	float m_fTotalScaleX;
	float m_fTotalScaleY;

	//Direct2D��أ�֧��WIN7
	HID2DBMP m_hD2DBmp;
	ID2D1Bitmap *m_pBitmap;
	ID2D1DCRenderTarget* m_spRT;
	static ID2D1BmpAtlas* s_pID2D1BmpAtlas;
	//GDI+��أ�֧��XP
	Bitmap* m_pGdiPlusBmp;

	XFORM m_xform;
	XFORM m_xformTranslate;
	XFORM m_xformFinal;

	//�����ļ���
	ID2D1SolidColorBrush *m_pBlackBrush;
	//ID2D1SolidColorBrush *m_pPinkBrush;
	IDWriteFactory *m_pDWriteFactory;
	IBindCtx *m_pBindContext;
	//
	IShellFolder*  m_psfDesktop;   
	IMalloc*	  m_pMalloc;

	//�ļ�·�����ļ���
	tstring m_strThumbURI;
	tstring m_strThumbName;

	//ͼ��
	SHFILEINFO m_sfi;
private:
	HDC m_hDC;
	// ��ȡ����ͼ�ľ��
	HRESULT CreateThumbnail( LPCTSTR strFileName, HBITMAP* pThumbnail );

	//d2d1.dll��������
	void  D2D1MakeRotateMatrixEx(
		__in   FLOAT angle,
		__in   D2D1_POINT_2F center,
		__out  D2D1_MATRIX_3X2_F *matrix
		);

	BOOL D2D1InvertMatrixEx( __inout D2D1_MATRIX_3X2_F *matrix )const;

	void InitializeXForm(XFORM * xForm);
	XFORM TranslateXForm(float x, float y);
	XFORM SetProductXForm(__in const XFORM & a, __in const XFORM & b);
	Point XFormTransformPoint(const XFORM & a, Point & point);
private:
	virtual void touchUpHandler(const TouchUp& te);
	virtual void touchDownHandler(const TouchDown& te);
};