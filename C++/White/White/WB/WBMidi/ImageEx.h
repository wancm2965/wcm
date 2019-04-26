// GDIPlusHelper.h: interface for the CGDIPlusHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_)
#define AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class ImageEx
{
public: 
	 ImageEx(IN LPCTSTR  sResourceType, IN LPCTSTR  sResource);   //从资源加载
	 ImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement = FALSE);
	 ImageEx(HBITMAP hbmp); 

	~ImageEx();
public:

	void	Draw(HDC hDC, CRect rect, int alpha = 255);
	void	Draw(HDC hDC,int alpha = 255);
	CRect	GetRect();
	bool	IsAnimatedGIF() { return m_nFrameCount > 1; }  // 判断是否是gif图片
	void	SetPause(bool bPause);                        //gif图片的暂停

	bool	IsPaused() { return m_bPause; }
	bool	InitAnimation();                             // gif动画初始化
	void	Destroy();

protected:

	bool				TestForAnimatedGIF();
	void				Initialize();        //参数初始化

	bool LoadFromBuffer(BYTE* pBuff, int nSize);
	bool GetResource(LPCTSTR lpName, LPCTSTR lpType, void* pResource, int& nBufSize);
	bool Load(CString sResourceType, CString sResource);

	void ThreadAnimation();        // gif动画线程

	static UINT WINAPI _ThreadAnimationProc(LPVOID pParam);

	//HANDLE			m_hThread;
	//HANDLE			m_hPause;
	//HANDLE			m_hExitEvent;
	HINSTANCE		m_hInst;
	
	UINT			m_nFrameCount;  //帧总数
	UINT			m_nFramePosition;   //第几帧
	bool			m_bIsInitialized;   //表示对象构造成功与否，不管是以资源还是文件的形式
	bool			m_bPause;
	PropertyItem*	m_pPropertyItem;    //如果为动画，会malloc内存，记得free
	IStream*		m_pStream;
	Bitmap *	    m_pBitmapBuf;   //缓冲图片
	Bitmap *		m_pTemp;
	
public:
	CRect m_rect;
	long lPause;
	bool DrawFrameGIF();          //画gif某一帧到缓冲图片
	
};
#endif // !defined(AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_)
