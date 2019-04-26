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
	 ImageEx(IN LPCTSTR  sResourceType, IN LPCTSTR  sResource);   //����Դ����
	 ImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement = FALSE);
	 ImageEx(HBITMAP hbmp); 

	~ImageEx();
public:

	void	Draw(HDC hDC, CRect rect, int alpha = 255);
	void	Draw(HDC hDC,int alpha = 255);
	CRect	GetRect();
	bool	IsAnimatedGIF() { return m_nFrameCount > 1; }  // �ж��Ƿ���gifͼƬ
	void	SetPause(bool bPause);                        //gifͼƬ����ͣ

	bool	IsPaused() { return m_bPause; }
	bool	InitAnimation();                             // gif������ʼ��
	void	Destroy();

protected:

	bool				TestForAnimatedGIF();
	void				Initialize();        //������ʼ��

	bool LoadFromBuffer(BYTE* pBuff, int nSize);
	bool GetResource(LPCTSTR lpName, LPCTSTR lpType, void* pResource, int& nBufSize);
	bool Load(CString sResourceType, CString sResource);

	void ThreadAnimation();        // gif�����߳�

	static UINT WINAPI _ThreadAnimationProc(LPVOID pParam);

	//HANDLE			m_hThread;
	//HANDLE			m_hPause;
	//HANDLE			m_hExitEvent;
	HINSTANCE		m_hInst;
	
	UINT			m_nFrameCount;  //֡����
	UINT			m_nFramePosition;   //�ڼ�֡
	bool			m_bIsInitialized;   //��ʾ������ɹ���񣬲���������Դ�����ļ�����ʽ
	bool			m_bPause;
	PropertyItem*	m_pPropertyItem;    //���Ϊ��������malloc�ڴ棬�ǵ�free
	IStream*		m_pStream;
	Bitmap *	    m_pBitmapBuf;   //����ͼƬ
	Bitmap *		m_pTemp;
	
public:
	CRect m_rect;
	long lPause;
	bool DrawFrameGIF();          //��gifĳһ֡������ͼƬ
	
};
#endif // !defined(AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_)
