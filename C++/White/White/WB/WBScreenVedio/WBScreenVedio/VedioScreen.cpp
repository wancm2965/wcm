// VedioScreen.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VedioScreen.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// VedioScreen

#include <WindowsX.h>
#include <Vfw.h>
#include <MMSystem.h>
#pragma comment (lib,"vfw32.lib")
#pragma comment (lib,"winmm.lib")
#include <cmath>
using namespace std;

#define  FULLSCREEN 1
#define  AREASCREEN 2
#define  WNDSCREEN  3

//ȫ�ֱ����������߳��Ƿ����
BOOL IsStopThread = FALSE;

//IMPLEMENT_DYNAMIC(VedioScreen, CWnd)

VedioScreen::VedioScreen(CString strFilePath,CString strFileName,int nRate,CPoint Point1,CPoint Point2,int nFlag,HWND GetCurHwnd,CPoint pt /*= CPoint(0,0)*/)
{
	m_strFileName = strFileName;
	m_strFilePath = strFilePath;
	m_nRate = nRate;
	m_P1 = Point1;
	m_P2 = Point2;
	opts[0]= &m_pCompressOption; 
	m_nFrames = 0;
	m_IsStop = FALSE;
	m_nFlag = nFlag;
	m_WndHwnd = GetCurHwnd;
	m_pt = pt;
	//m_HPWBDataNotify = NULL;
	//m_bSupportU7OpenFile = FALSE;
}

VedioScreen::~VedioScreen()
{
	if (m_pVedioScreen)
	{
		m_pVedioScreen = NULL;
	}

}


// BEGIN_MESSAGE_MAP(VedioScreen, CWnd)
// END_MESSAGE_MAP()



// VedioScreen ��Ϣ�������

BOOL VedioScreen::StopVedio()
{
	/*-------------------------------------------------------------------------*/
	IsStopThread = TRUE;
	UnPauseVedio();
	WaitForMultipleObjects(1,&ThreadFun,FALSE,INFINITE);
	if (NULL != ThreadFun)
	{
		if (NULL != pData)
		{
			delete pData;
			pData = NULL;
		}
		CloseHandle(ThreadFun);
		//IsStopThread = FALSE;
		//return TRUE;
	}
	IsStopThread = FALSE;
	return TRUE;
	/*-------------------------------------------------------------------------*/
}

DWORD WINAPI VedioScreen::VedioFun(LPVOID lp)
{

	//::MessageBox(NULL,_T("�����̴߳�����,��ʼ��COM���!"),_T("��ʾ"),MB_OK);
	// ��ʼ��Com��
	HRESULT hr = S_OK;
	/*
	HRESULT hr = CoInitialize( NULL );
	if ( FAILED( hr ) )
	{
		AfxMessageBox( _T( "��ʼ��Com��ʧ��!" ) );	
		CoUninitialize();
		return FALSE;
	}
	*/
	CBitmap cbBmp;
	CRect   crRect;
	CRect   rectWnd;
	CDC     memDC;
	CPoint  Point;
	HICON   phicon;
	CDC*    pDeskDC;
	CDC     pFullDC;
	BITMAP bitmap;
	/*-------------------------------------------------------------------------*/

	AVISTREAMINFO  m_StrHdr;                 //����һ��avi ����Ϣ�ṹ��
	PAVIFILE       m_pFile = NULL;                  //����һ��avi ���ļ�
	PAVISTREAM     m_ps = NULL;                     //����һ��avi��
	PAVISTREAM     m_pComStream = NULL;             //����һ��avi��
	HRESULT        m_hr = NULL;
	AVICOMPRESSOPTIONS  m_pCompressOption;
	AVICOMPRESSOPTIONS FAR * opts[1] = {&m_pCompressOption}; 
	
	//::MessageBox(NULL,_T("�̴߳���������ʼ����vfw32.lib!"),_T("��ʾ"),MB_OK);
	AVIFileInit(); //��ʼ��vfw32.lib������AVI������غ���
	//::MessageBox(NULL,_T("�̴߳��������ɹ�����vfw32.lib!"),_T("��ʾ"),MB_OK);
	/*-------------------------------------------------------------------------*/

	PVedioData lpVedio = (PVedioData)lp;

	/*-------------------------------------------------------------------------*/
	int nX1Start;//¼�ƴ������Ͻ�X����
	int nY1End;  //¼�ƴ������Ͻ�Y����
	int nX2Start;//¼�ƴ��ڿ��+3
	int nY2End;  //¼�ƴ��ڸ߶�+3

	/*-------------------------------------------------------------------------*/

	if ( NULL == lpVedio->WndHwnd )
	{
		nX1Start  = lpVedio -> p1.x;
		nY1End    = lpVedio -> p1.y;
		nX2Start  = lpVedio -> p2.x - lpVedio -> p1.x + 3;
		nY2End    = lpVedio -> p2.y - lpVedio -> p1.y + 3;
	}else
	{
		if (::GetWindowRect(lpVedio->WndHwnd,&rectWnd))
		{
			nX1Start = rectWnd.left;
			nY1End   = rectWnd.top;
			nX2Start = rectWnd.right - rectWnd.left;
			nY2End   = rectWnd.bottom - rectWnd.top;
		}
	}
	/*-------------------------------------------------------------------------*/

	switch (lpVedio->nFlag)
	{
	case FULLSCREEN:
		{
			//��ȡ����DC
			//pDeskDC = GetDesktopWindow()->GetDC();
			//GetDesktopWindow()->GetClientRect(crRect);
// 			pDeskDC = CWnd::FromHandle(::GetDesktopWindow())->GetDC();
// 			CWnd::FromHandle(::GetDesktopWindow())->GetClientRect(crRect);

			pFullDC.CreateDC(L"DISPLAY",NULL,NULL,NULL);
			POINT pt;
			//::GetCursorPos(&pt);
			pt.x = lpVedio->pt.x;
			pt.y = lpVedio->pt.y;
			MONITORINFO oMonitor = {}; 
			oMonitor.cbSize = sizeof(oMonitor);
			::GetMonitorInfo(::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
			crRect = oMonitor.rcWork;
			//��ȡ������DC���ݵ�DC���Ի�ȡ��ɫͼ��
			memDC.CreateCompatibleDC(&pFullDC);
			cbBmp.CreateCompatibleBitmap(&pFullDC,crRect.Width(),crRect.Height());
			memDC.SelectObject(&cbBmp);
			//��ȡλͼ��Ϣ
			cbBmp.GetBitmap(&bitmap);
		}
		break;
	case AREASCREEN:
		{
			/*-------------------------------------------------------------------------*/

			//��ʼ��λͼ��Ϣ
			//pDeskDC = GetDesktopWindow()->GetDC();
			pDeskDC = CWnd::FromHandle(::GetDesktopWindow())->GetDC();
			memDC.CreateCompatibleDC(pDeskDC);
			cbBmp.CreateCompatibleBitmap(pDeskDC,nX2Start,nY2End);
			memDC.SelectObject(&cbBmp);
			cbBmp.GetBitmap(&bitmap);

			/*-------------------------------------------------------------------------*/
		}
		break;
	case WNDSCREEN:
		{
			//��ʼ��λͼ��Ϣ
			//pDeskDC = GetDesktopWindow()->GetDC();
			pDeskDC = CWnd::FromHandle(::GetDesktopWindow())->GetDC();
			memDC.CreateCompatibleDC(pDeskDC);
			cbBmp.CreateCompatibleBitmap(pDeskDC,nX2Start,nY2End);
			memDC.SelectObject(&cbBmp);
			cbBmp.GetBitmap(&bitmap);
		}
		break;
	default:
		break;
	}

	/*-------------------------------------------------------------------------*/
	bool bflag = true;
	CString strPath = _T("");
	strPath = lpVedio->strFilePath;
	VedioScreen *vedioscreen = (VedioScreen*)m_pVedioScreen;
	while (1)
	{
		//�����жϴ����Ƿ�����wangxiaobin
		if (vedioscreen)
		{
			bflag = vedioscreen->GetResidualPlaceSize(strPath);
			if (bflag == false)
			{
				vedioscreen->StopVedio();
			}
		}		
				
		switch (lpVedio->nFlag)
		{
		case FULLSCREEN:
			{
				//������DC�и���λͼ��memDC��
				memDC.BitBlt(0,0,bitmap.bmWidth,bitmap.bmHeight,&pFullDC,crRect.left,crRect.top,SRCCOPY);
			}
			break;
		case AREASCREEN:
			{
				memDC.BitBlt(0,0,nX2Start,nY2End,pDeskDC,nX1Start,nY1End,SRCCOPY);
			}
			break;
		case WNDSCREEN:
			{
				memDC.BitBlt(0,0,nX2Start,nY2End,pDeskDC,nX1Start,nY1End,SRCCOPY);
			}
			break;
		default:
			break;
		}
		/*-------------------------------------------------------------------------*/
		//��ͼ���ϻ��ƹ��
		::GetCursorPos(&Point);
		phicon = (HICON)::GetCursor();
		memDC.DrawIcon(Point,phicon);

		/*-------------------------------------------------------------------------*/
		//����λͼ�Ĵ�С�������ڴ�
		DWORD dwSize = bitmap.bmWidthBytes*bitmap.bmHeight;
		BYTE* lpData = new BYTE[dwSize];
		int nPanelSize  = 0;									//��¼��ɫ���С
		if(bitmap.bmBitsPixel<16)							//�ж��Ƿ�Ϊ���ɫλͼ
		{
			//��ɫ���СΪ2�Ķ��ٴη�
			nPanelSize = (int)pow((double)2,(double)bitmap.bmBitsPixel*sizeof(RGBQUAD));
		}

		/*-------------------------------------------------------------------------*/

		BITMAPINFOHEADER *pBInfo = new BITMAPINFOHEADER;
		pBInfo->biBitCount       = bitmap.bmBitsPixel;//���ش�С
		////pBInfo->biClrImportant   = 0;
		//pBInfo->biCompression    = 0;////
		pBInfo->biCompression    = BI_RGB;
		pBInfo->biHeight         = bitmap.bmHeight;
		pBInfo->biWidth          = bitmap.bmWidth;
		pBInfo->biPlanes         = bitmap.bmPlanes;
		pBInfo->biSize           = sizeof(BITMAPINFOHEADER);
		pBInfo->biSizeImage      = bitmap.bmWidthBytes*bitmap.bmHeight;////
		//pBInfo->biSizeImage     = pBInfo->biWidth * pBInfo->biHeight * pBInfo->biBitCount/8; 
		////pBInfo->biXPelsPerMeter  = 0;
		////pBInfo->biYPelsPerMeter  = 0;
//		pBInfo->biClrUsed        = 0;

		BITMAPINFO bInfo;
		bInfo.bmiHeader = *pBInfo;
		//��ָ������λͼ�л�ȡͼ��λ���浽�����У���Ϊ�豸�޹�λͼ��DIB��
		GetDIBits(memDC.m_hDC,cbBmp,0,pBInfo->biHeight,lpData,&bInfo,DIB_RGB_COLORS);

		/*-------------------------------------------------------------------------*/
		//����ͼ��֡��Ϊ0����ô��Ҫ�ȴ����ļ�
		if (lpVedio->nFrames == 0)
		{
			try
			{
				if(lpVedio->strFilePath.Right(1) != L"\\")
				{
					lpVedio->strFilePath = lpVedio->strFilePath + L"\\";
				}

				//::MessageBox(NULL,_T("׼������AVI�ļ�!"),_T("��ʾ"),MB_OK);
				//����AVI�ļ�
				m_hr = AVIFileOpen(&m_pFile,lpVedio->strFilePath+lpVedio->strFileName+_T(".avi"),OF_WRITE|OF_CREATE,NULL);
				//m_hr = AVIFileOpen(&m_pFile, _T("D:\\XPTest.avi"), OF_CREATE|OF_WRITE, NULL);
				if (0 != m_hr)
				{
					CString msg;
					CString strUnKnowError;
					strUnKnowError = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("UnKnowError"));
					if (strUnKnowError.IsEmpty())
					{
						strUnKnowError = _T("δ֪����");
					}
					switch (m_hr)
					{
					case AVIERR_BADFORMAT:
						msg = _T("AVIERR_BADFORMAT--The file couldn't be read, indicating a corrupt file or an unrecognized format");
						break;
					case AVIERR_MEMORY:
						msg = _T("AVIERR_MEMORY--The file could not be opened because of insufficient memory.");
						break;
					case AVIERR_FILEREAD:
						msg = _T("AVIERR_FILEREAD--A disk error occurred while reading the file.");
						break;
					case AVIERR_FILEOPEN:
						msg = _T("AVIERR_FILEOPEN--A disk error occurred while opening the file.");
						break;
					case REGDB_E_CLASSNOTREG:
						msg = _T("REGDB_E_CLASSNOTREG--According to the registry, the type of file specified in AVIFileOpen does not have a handler to process it. ");
						break;
					default:
						msg = _T("δ֪����");
						break;
					}
					CString errorMsg;
					CString strAVIFail;
					strAVIFail = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("AVIFail"));
					if (strAVIFail.IsEmpty())
					{
						strAVIFail = _T("����AVI�ļ�ʧ��! ����������");
					}
					CString strPrompt;
					strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
					if (strPrompt.IsEmpty())
					{
						strPrompt = _T("��ʾ");
					}
					errorMsg = strAVIFail + msg;

					if (m_bSupportU7OpenFile && m_HPWBDataNotify)
					{
						m_HPWBDataNotify->MessageBox(NULL,strPrompt, errorMsg, MB_OK,true);
					}
					else
					{
						::MessageBox(NULL, errorMsg, strPrompt, MB_OK);
					}
					
					OutputDebugString(msg);
					IsStopThread = TRUE;
					goto CLOSETHREAD;
				}
				//::MessageBox(NULL,_T("����AVI�ļ��ɹ�!"),_T("��ʾ"),MB_OK);

				memset(&m_StrHdr, 0, sizeof(m_StrHdr));
				m_StrHdr.fccType    = streamtypeVIDEO;//����һ����Ƶ��
				m_StrHdr.fccHandler = mmioStringToFOURCC(L"MSVC", 0);//ѹ�����
				m_StrHdr.dwFlags    = NULL; 
				m_StrHdr.dwCaps     = NULL; 
				m_StrHdr.wPriority  = NULL;   
				m_StrHdr.wLanguage  = NULL; 
				m_StrHdr.dwScale    = 1;
				m_StrHdr.dwRate     = lpVedio->nRate;
				m_StrHdr.dwStart    = 0; 
				m_StrHdr.dwLength   = 0;
				m_StrHdr.dwInitialFrames = 0;   
				m_StrHdr.dwSuggestedBufferSize = pBInfo->biSizeImage;
				m_StrHdr.dwQuality  =  7500;   
				m_StrHdr.dwSampleSize = 0;   
				m_StrHdr.dwEditCount  = 0;   
				m_StrHdr.dwFormatChangeCount = 0;  
				_tcscpy_s(m_StrHdr.szName,   L"AVI   VIDEO   #1");  
				SetRect(&m_StrHdr.rcFrame,0,0,pBInfo->biWidth,pBInfo->biHeight);

				//::MessageBox(NULL,_T("׼��������Ƶ��!"),_T("��ʾ"),MB_OK);
				//����һ����Ƶ����XP�ڴ�ʧ��
				m_hr= AVIFileCreateStream(m_pFile,&m_ps,&m_StrHdr);
				if (0 != m_hr)
				{
					CString strPrompt;
					strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
					if (strPrompt.IsEmpty())
					{
						strPrompt = _T("��ʾ");
					}
					CString strCreateVideoIOFail;
					strCreateVideoIOFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CreateVideoIOFail"));
					if (strCreateVideoIOFail.IsEmpty())
					{
						strCreateVideoIOFail = _T("������Ƶ��ʧ��!");
					}
					if (m_bSupportU7OpenFile && m_HPWBDataNotify)
					{
						m_HPWBDataNotify->MessageBox(NULL,strPrompt, strCreateVideoIOFail, MB_OK,true);
					}
					else
					{
						::MessageBox(NULL,strCreateVideoIOFail,strPrompt,MB_OK);
					}
					
					IsStopThread = TRUE;
					goto CLOSETHREAD;
				}
				//::MessageBox(NULL,_T("������Ƶ���ɹ�!"),_T("��ʾ"),MB_OK);

				//����ѹ����ʽ
				memset(&m_pCompressOption, 0, sizeof(m_pCompressOption));
				m_pCompressOption.fccType = streamtypeVIDEO;
				//����ѹ������������������ʱ��ѹ����������ͬ�Ļ�����װ�Ľ��������ܲ�һ��������ʹ�ó��õ�ѹ����
				m_pCompressOption.fccHandler = mmioStringToFOURCC(L"MSVC", 0);
				m_pCompressOption.dwQuality = 7500;//ѹ���������ò��������ݸ�ѹ����
				//m_pCompressOption.dwBytesPerSecond = 0;////
				m_pCompressOption.dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES;
				//m_pCompressOption.lpFormat = 0;////
				//m_pCompressOption.cbFormat = 0;////
				m_pCompressOption.dwKeyFrameEvery = 1;////
 				//m_pCompressOption.dwInterleaveEvery = 0;////
	// 			opts[0]->fccType = streamtypeVIDEO;
	// 			opts[0]->fccHandler = mmioStringToFOURCC("MSVC", 0);
	// 			opts[0]->dwQuality = 7500;
	// 			opts[0]->dwBytesPerSecond = 0;
	// 			opts[0]->dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES;
	// 			opts[0]->lpFormat = 0;
	// 			opts[0]->cbFormat = 0;
	// 			opts[0]->dwKeyFrameEvery = 1;
	// 			opts[0]->dwInterleaveEvery = 0;

				//::MessageBox(NULL,_T("׼������ѹ����Ƶ������������ʽ!"),_T("��ʾ"),MB_OK);
				//����ѹ����
				m_hr=AVIMakeCompressedStream(&m_pComStream,m_ps,&m_pCompressOption,NULL); 
				//�������ĸ�ʽ
				m_hr=AVIStreamSetFormat(m_pComStream,0,pBInfo,sizeof(BITMAPINFOHEADER));
				//::MessageBox(NULL,_T("����ѹ����Ƶ������������ʽ�ɹ�!"),_T("��ʾ"),MB_OK);
			}
			catch (CException* e)
			{
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("��ʾ");
				}
				CString strCreateVedioError;
				strCreateVedioError = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("CreateVedioError"));
				if (strCreateVedioError.IsEmpty())
				{
					strCreateVedioError = _T("������Ƶ�ļ�����");
				}
				TCHAR buff[512];
				e->GetErrorMessage(buff,512,NULL);
				CString msg;
				msg = strCreateVedioError + buff;
				OutputDebugString(msg);
				if (m_bSupportU7OpenFile && m_HPWBDataNotify)
				{
					m_HPWBDataNotify->MessageBox(NULL,strPrompt,msg, MB_OK,true);
				}
				else
				{
					::MessageBox(NULL,msg,strPrompt,MB_OK);
				}
				
				IsStopThread = TRUE;
				goto CLOSETHREAD;
			}
		}

		try
		{
			m_hr= AVIStreamWrite(m_pComStream,lpVedio->nFrames ,1,(LPBYTE)lpData,
				pBInfo->biSizeImage,AVIIF_KEYFRAME,NULL,NULL);
			ASSERT(m_hr == 0); 
			lpVedio->nFrames++;
		}
		catch (CException* e)
		{
			CString strPrompt;
			strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
			if (strPrompt.IsEmpty())
			{
				strPrompt = _T("��ʾ");
			}
			CString strWriteVedioError;
			strWriteVedioError = ReadConfigText(_T("WhiteBoardVideoScreenText"), _T("WriteVedioError"));
			if (strWriteVedioError.IsEmpty())
			{
				strWriteVedioError = _T("д����Ƶ�����ݳ���");
			}
			TCHAR buff[512];
			e->GetErrorMessage(buff,512,NULL);
			CString msg;
			msg = strWriteVedioError + buff;
			OutputDebugString(msg);
			if (m_bSupportU7OpenFile && m_HPWBDataNotify)
			{
				m_HPWBDataNotify->MessageBox(NULL,strPrompt,msg, MB_OK,true);
			}
			else
			{
				::MessageBox(NULL,msg,strPrompt,MB_OK);
			}
			
			IsStopThread = TRUE;
			goto CLOSETHREAD;
		}

		delete []lpData;
		delete pBInfo ;

		//��ͣ���壬����CPUʹ����
		if (lpVedio->nRate < 6)
		{
			Sleep(1000/lpVedio->nRate);
		} 
		else
		{
			Sleep(200);
		}

		/*-------------------------------------------------------------------------*/
		//�ж�ѭ���Ƿ����
CLOSETHREAD:	if (IsStopThread == TRUE)
				{	
					if(NULL != m_pComStream)
					{
						AVIStreamClose(m_pComStream);
					}
					if (NULL != m_ps)
					{
						AVIStreamClose(m_ps);
					}
					if (NULL != m_pFile)
					{
						AVIFileRelease(m_pFile);
					}
					lpVedio->nFrames = 0;
					break;
				}
			}
	AVIFileExit();
	//ExitThread(GetCurrentThreadId());
	return 0;
}

void VedioScreen::StartVedio()
{
	pData = new VedioData;
	pData->strFilePath = m_strFilePath;
	pData->strFileName = m_strFileName;
	pData->nFrames = m_nFrames;
	pData->nRate = m_nRate;
	pData->p1 = m_P1;
	pData->p2 = m_P2;
	pData->nFlag = m_nFlag;
	pData->WndHwnd = m_WndHwnd;
	pData->pt = m_pt;
	////
	//MessageBox(_T("����StartVedio����!,��ʼ�����̣߳�"));
	GetLangPath();
	ThreadFun = CreateThread(NULL,0,VedioFun,pData,0,0);
	if (NULL == ThreadFun )
	{
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("��ʾ");
		}
		CString strCreateThreadFail;
		strCreateThreadFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("CreateThreadFail"));
		if (strCreateThreadFail.IsEmpty())
		{
			strCreateThreadFail = _T("�̴߳���ʧ��!");
		}
		if (m_bSupportU7OpenFile && m_HPWBDataNotify)
		{
			m_HPWBDataNotify->MessageBox(m_WndHwnd,strPrompt, strCreateThreadFail, MB_OK,true);
		}
		else
		{
			::MessageBox(NULL,strCreateThreadFail,strPrompt,MB_OK);
		}
		
		ExitProcess(0);
	} 
// 	else
// 	{
// 		MessageBox(_T("�����̳߳ɹ���"));
// 	}
}


void VedioScreen::PauseVedio()
{
	SuspendThread(ThreadFun); //�����̵߳�ִ��
}


void VedioScreen::UnPauseVedio()
{
	ResumeThread(ThreadFun); //�ָ��̵߳�ִ��
}

void VedioScreen::SetU7HPWBDataNotify( IHPNetNotify* HPWBDataNotify )
{
	m_HPWBDataNotify = HPWBDataNotify;
}

void VedioScreen::SetSupportU7Open( BOOL bSupportU7OpenFile )
{
	m_bSupportU7OpenFile = bSupportU7OpenFile;
}
bool VedioScreen::GetResidualPlaceSize( CString cstring )
{
	bool bFlag = true;
	ULARGE_INTEGER   ulTotalSize;       //�����ܿռ��С       
	ULARGE_INTEGER   ulFreeSize;        //����ʣ��ռ��С
	ULARGE_INTEGER   ulAvailLableSize;  //¼���ļ���Ҫ�Ŀռ��С

	CString csStr;
	csStr = cstring.Left(3);
	::GetDiskFreeSpaceEx(csStr,&ulTotalSize,&ulFreeSize,&ulAvailLableSize);

	if (ulTotalSize.QuadPart / 1024 /1024 < 10)
	{
		CString strMessagePrompt;
		strMessagePrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("MessagePrompt"));
		if (strMessagePrompt.IsEmpty())
		{
			strMessagePrompt = _T("��Ϣ��ʾ");
		}
		CString strDiskNotEnough10;
		strDiskNotEnough10 = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("DiskNotEnough10"));
		if (strDiskNotEnough10.IsEmpty())
		{
			strDiskNotEnough10 = _T("������ʣ��ռ䲻��10��ֹͣ¼�ơ�");
		}
		if (m_bSupportU7OpenFile && m_HPWBDataNotify)
		{
			m_HPWBDataNotify->MessageBox(NULL,strMessagePrompt, strDiskNotEnough10, MB_OK,true);
		}
		else
		{
			MessageBox(NULL,strDiskNotEnough10,strMessagePrompt,MB_OK);
		}
		bFlag = false;
	} 
	return bFlag;
}