#pragma once

#include <Vfw.h>
#include <MMSystem.h>

#pragma comment (lib,"vfw32.lib")
#pragma comment (lib,"winmm.lib")
#include <cmath>
using namespace std;

// VedioScreen

typedef struct DATA 
{
	CString strFileName;
	CString strFilePath;
	int nRate;
	int nFrames;
	int nFlag;
	CPoint p1;
	CPoint p2;
	HWND   WndHwnd;
	CPoint pt;
}VedioData, *PVedioData;

//class VedioScreen : public CWnd
class VedioScreen
{
	//DECLARE_DYNAMIC(VedioScreen)

public:
	VedioScreen(CString strFilePath,CString strFileName,int nRate,CPoint Point1,CPoint Point2,int nFlag,HWND GetCurHwnd,CPoint pt = CPoint(0,0));
	virtual ~VedioScreen();

protected:
	//DECLARE_MESSAGE_MAP()
public:
	CPoint m_pt;
	CString m_strFilePath;
	CString m_strFileName;
	int     m_nRate;
	int     m_nFlag;
	CPoint  m_P1;
	CPoint  m_P2;
	HWND    m_WndHwnd;

public:
	AVISTREAMINFO  m_StrHdr;                 //定义一个avi 流信息结构体
	PAVIFILE       m_pFile;                  //定义一个avi 流文件
	PAVISTREAM     m_ps;                     //定义一个avi流
	PAVISTREAM     m_pComStream;             //定义一个avi流
	HRESULT        m_hr;
	AVICOMPRESSOPTIONS  m_pCompressOption;
	AVICOMPRESSOPTIONS FAR * opts[1]; 

public:
	BITMAPINFOHEADER *m_pHeaderInfo;
	BITMAPINFO        m_bInfo;
	int               m_nFrames;
	BOOL              m_IsStop;
	HANDLE  ThreadFun;
	DWORD   ThreadID;
	PVedioData pData;
	//static IHPNetNotify * m_HPWBDataNotify;
	//static BOOL           m_bSupportU7OpenFile;

public:
	static DWORD WINAPI VedioFun(LPVOID lp);
	void StartVedio();
	BOOL StopVedio();
	void PauseVedio();
	void UnPauseVedio();
	 void  SetU7HPWBDataNotify( IHPNetNotify* HPWBDataNotify );
	 void  SetSupportU7Open(BOOL bSupportU7OpenFile);
	 bool GetResidualPlaceSize(CString cstring);//磁盘空间
};
