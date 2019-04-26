// WBSavePictureDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBSavePictureDlg.h"
#include "afxdialogex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>

// CWBSavePictureDlg �Ի���
#define INDEX_USE_PPDF	(-1)

IMPLEMENT_DYNAMIC(CWBSavePictureDlg, CDialogEx)

CWBSavePictureDlg::CWBSavePictureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBSavePictureDlg::IDD, pParent)
	, m_strWritePage(_T(""))
	, m_bIsShow(false)
{
	m_pWBSSavePictureFile = new WBSavePictureFile;

	//��ȡ��Ļ��λͼ��
	CRect rect(0, 0,g_nWidth,g_nHeight);
	ZeroMemory(m_pWBSSavePictureFile,sizeof(WBSavePictureFile));
	m_pWBSSavePictureFile->bSave = FALSE;
	m_pWBSSavePictureFile->PageHdc = g_pWBPage->GetMemHDC();
	m_pWBSSavePictureFile->rectPage = rect;
	m_pBitmap=CBitmap::FromHandle(CopyPageBitmapToPicture(m_pWBSSavePictureFile));
}

CWBSavePictureDlg::~CWBSavePictureDlg()
{
	if (m_pWBSSavePictureFile)
	{
		delete m_pWBSSavePictureFile;
		m_pWBSSavePictureFile = NULL;
	}
	if (m_pBitmap)
	{
		m_pBitmap->DeleteObject();
		m_pBitmap = NULL;
	}
	CleanVectorPageNumber();
}

void CWBSavePictureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PAGE_ID, m_StrPageId);
	DDX_Text(pDX, IDC_EDIT_WEITE_PAGE, m_strWritePage);
	DDX_Control(pDX, IDC_RADIO_CUR_PAGE, m_BtnCurPage);
	DDX_Control(pDX, IDC_RADIO_ALL_PAGE, m_BtnALLPage);
	DDX_Control(pDX, IDC_RADIO_SELE_PAGE, m_BtnSelectPage);
	DDX_Control(pDX, IDC_EDIT_WEITE_PAGE, m_EditPageCount);
}

BEGIN_MESSAGE_MAP(CWBSavePictureDlg, CDialogEx)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(IDC_RADIO_CUR_PAGE, &CWBSavePictureDlg::OnRadioCurPage)
	ON_COMMAND(IDC_RADIO_ALL_PAGE, &CWBSavePictureDlg::OnRadioAllPage)
	ON_COMMAND(IDC_RADIO_SELE_PAGE, &CWBSavePictureDlg::OnRadioSelePage)
	ON_WM_ERASEBKGND()
	ON_WM_CHAR()
END_MESSAGE_MAP()


// CWBSavePictureDlg ��Ϣ�������


void CWBSavePictureDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CWBSavePictureDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CWBSavePictureDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnMouseMove(nFlags, point);
}

void CWBSavePictureDlg::OnRadioCurPage()
{
	// TODO: �ڴ���������������
	//m_BtnCurPage.SetCheck(1);
	m_EditPageCount.EnableWindow(FALSE);
	m_EditPageCount.SetWindowText(_T(""));
	SetIsSelectBtn(IDC_RADIO_CUR_PAGE);
}

void CWBSavePictureDlg::OnRadioAllPage()
{
	// TODO: �ڴ���������������
	//m_BtnALLPage.SetCheck(1);
	m_EditPageCount.EnableWindow(FALSE);
	m_EditPageCount.SetWindowText(_T(""));
	SetIsSelectBtn(IDC_RADIO_ALL_PAGE);
}

void CWBSavePictureDlg::OnRadioSelePage()
{
	// TODO: �ڴ���������������
	//m_BtnSelectPage.SetCheck(1);
	m_EditPageCount.EnableWindow(TRUE);
	m_EditPageCount.SetIsWritePageCount(true);
	SetIsSelectBtn(IDC_RADIO_SELE_PAGE);
}

void CWBSavePictureDlg::OnCancel()
{
	m_nSavePage = WBSaveCurPage;
	ShowWindow(SW_HIDE);
}

BOOL CWBSavePictureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CString strTip = _T("(1,3,2-4)");
	m_StrPageId.SetTextPosition(FALSE);
	m_StrPageId.SetCaption(strTip);
	m_StrPageId.SetWindowText(strTip);
	if (g_pWhiteBoardView == NULL)
	{
		return FALSE;
	}
	//m_BtnCurPage.SetCheck(1);
	//m_EditPageCount.SetReadOnly(TRUE);//ֻ��
	m_EditPageCount.EnableWindow(FALSE);
	SetIsSelectBtn(IDC_RADIO_CUR_PAGE);

	CString strPrompt;
	strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	if (strPrompt.IsEmpty())
	{
		strPrompt = _T("��ʾ");
	}
	CString strBtn = _T("");
	strBtn = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SavePageToPictureStrCur"));
	if (strBtn.IsEmpty())
	{
		strBtn = _T("��ǰҳ��");
	}
	m_BtnCurPage.SetButtonStyle(BS_OWNERDRAW);
	m_BtnCurPage.SetWindowText(strBtn);
	strBtn = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SavePageToPictureStrAll"));
	if (strBtn.IsEmpty())
	{
		strBtn = _T("ȫ��ҳ��");
	}
	m_BtnALLPage.SetButtonStyle(BS_OWNERDRAW);
	m_BtnALLPage.SetWindowText(strBtn);
	strBtn = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SavePageToPictureStrSelete"));
	if (strBtn.IsEmpty())
	{
		strBtn = _T("����ҳ��");
	}
	m_BtnSelectPage.SetButtonStyle(BS_OWNERDRAW);
	m_BtnSelectPage.SetWindowText(strBtn);
	
	//���ñ����ı�
	CString strScreenBtn;
	strScreenBtn = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ScreenText"));
	if (strScreenBtn.IsEmpty())
	{
		strScreenBtn = _T("����ΪͼƬ");
	}
	SetWindowText(strScreenBtn);

	CString strScreenBtnOKCancle;
	strScreenBtnOKCancle = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ScreenTextOK"));
	if (strScreenBtnOKCancle.IsEmpty())
	{
		strScreenBtnOKCancle = _T("ȷ��");
	}
	SetDlgItemText(IDOK,strScreenBtnOKCancle);

	strScreenBtnOKCancle = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ScreenTextCANCLE"));
	if (strScreenBtnOKCancle.IsEmpty())
	{
		strScreenBtnOKCancle = _T("ȡ��");
	}
	SetDlgItemText(IDCANCEL,strScreenBtnOKCancle);

	CRect rect;
	GetWindowRect(&rect);
	CRect WndRect;
	g_pWhiteBoardView->GetWindowRect(&WndRect);
	SetWindowPos(&wndTop,rect.left + 100,rect.top,rect.Width(),rect.Height(),SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	CenterWindow(g_pWhiteBoardView);

	if (ZSYDZ)
	{
		ShowWindow(SW_HIDE);
	}
	return TRUE;
}

void CWBSavePictureDlg::OnOK()
{
	if (g_pWBPage == NULL || g_pWhiteBoardView == NULL)
	{
		return;
	}
	ShowWindow(SW_HIDE);
	CString strFilePath = _T("");
	if (m_BtnCurPage.GetIsSelect())
	{
		m_nSavePage = WBSaveCurPage;
	}
	else if (m_BtnALLPage.GetIsSelect())
	{
		m_nSavePage = WBSaveAllPage;
	}
	else 
	{
		m_nSavePage = WBSaveSelectPage;
		if (!CheckPageStr())
		{
			return;
		}
	}
	strFilePath = GetbIsOK();
	if (strFilePath.IsEmpty() && g_pWhiteBoardView)
	{
		g_pWhiteBoardView->GetWBSavePictureDlg();
		return;
	}
	switch(m_nSavePage)
	{
	case WBSaveCurPage:
		{
			CWBPage *pPage = NULL;
			pPage = g_pWBPage;
			SaveCurPageBitmapToPicture(pPage,strFilePath);
		}
		break;
	case WBSaveAllPage:
		{
			SaveAllPageBitmapToPicture(strFilePath);
		}
		break;
	case WBSaveSelectPage:
		{
			SaveSelctPageBitmapToPicture(strFilePath);
		}
		break;
	default:
		break;
	}
	if (m_strtempath != " ")
	{
		SavePdf();
	}
}

CString CWBSavePictureDlg::GetbIsOK()
{
	CString strFilePath = _T("");
	if (g_bSupportU7OpenFile)
	{
		//wchar_t szFilter[MAX_PATH];
		CString strFilter = _T("|BMP Files (*.bmp)|*.bmp")
							_T("|JPG Files (*.jpg)|*.jpg")
							_T("|PDF Files (*.pdf)|*.pdf")
							_T("|PNG Files(*.png)|*.png||");
		//MultiByteToWideChar(CP_ACP, 0, strFilter, -1, szFilter, MAX_PATH);
		CString strfilterName;
		strfilterName = g_strAvconUserID;
		SYSTEMTIME sys; 
		GetLocalTime( &sys );
		CString strTime;
		strTime.Format(_T("%d%d%d%d%d"),sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute);
		strfilterName += strTime;
		char* sFilePath = new char[MAX_PATH];
		memset(sFilePath,0,MAX_PATH);
		bool bOpen = g_pHPWBDataNotify->OpenSaveFile(FALSE, sFilePath, NULL, strFilter, g_pWhiteBoardView->GetSafeHwnd(),strfilterName);
		if (g_pWhiteBoardView == NULL)
		{
			delete[]sFilePath;
			return _T("");
		}
		if (bOpen)
		{
			strFilePath = sFilePath;
		}
		delete[] sFilePath;

		if (strFilePath.IsEmpty())
		{
			return _T("");
		}
		CString str;
		str = PathFindExtension(strFilePath);
		m_strtempath = " ";
		if (str == L".pdf")
		{
			m_strtempath = strFilePath;
			strFilePath = GetCurrentPath() +L"\\temporary\\a.jpg";
		}
		CString strpath = _T("");
		strpath = strFilePath;
		PathRemoveFileSpec(strpath.GetBuffer()); //�õ������·�������������ļ���
		if (!PathIsDirectory(strpath)) //�ж�·���Ƿ���һ����Ч��Ŀ¼
		{
			if(!CreateDirectory(strpath, NULL)) //����·�������ļ���
			{
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("��ʾ");
				}
				CString strFolderCreateFail;
				strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("FolderCreateFail"));
				if (strFolderCreateFail.IsEmpty())
				{
					strFolderCreateFail = _T("�ļ��д���ʧ�ܣ�");
				}
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strFolderCreateFail, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strFolderCreateFail);

				}
				else
				{
					AfxMessageBox(strFolderCreateFail);
				}
				//delete[] chFilePath;
				return _T("");
			}
		}
		//delete[] chFilePath;
		strpath.ReleaseBuffer();
		if (PathFileExists(strFilePath)) //�ж��ļ��Ƿ����
		{
			DeleteFile(strFilePath.GetBuffer(strFilePath.GetLength())); //ɾ���ļ�
			strFilePath.ReleaseBuffer();
		}
	} 
	else
	{
		CString strFilter = _T("|BMP Files (*.bmp)|*.bmp||")
				_T("|PDF Files (*.pdf)|*.pdf||");
			_T("|JPG Files (*.jpg)|*.jpg||");
		_T("|PNG Files(*.png)|*.png||");
		CFileDialog dlgFile(FALSE, _T("bmp"), NULL, OFN_HIDEREADONLY ,strFilter, NULL, 0, FALSE);

		dlgFile.m_ofn.lpstrTitle = _T("����Ϊ");

		if (dlgFile.DoModal() == IDOK)
		{
			if (!g_pWhiteBoardView)
			{
				return _T("");
			}
			strFilePath = dlgFile.GetPathName();
			if (PathFileExists(strFilePath))//����ļ����ھ�ɾ������ļ�
			{
				DeleteFile(strFilePath.GetBuffer(strFilePath.GetLength()));
				strFilePath.ReleaseBuffer();
			}
		}
	}
	return strFilePath;
}

void CWBSavePictureDlg::PrintCurrentPage()
{
	CString strFilePath = _T("");	
	m_nSavePage = WBSaveCurPage;	
	strFilePath = GetbIsOK();
	CWBPage *pPage = NULL;
	pPage = g_pWBPage;
	SaveCurPageBitmapToPicture(pPage,strFilePath);
}

BOOL CWBSavePictureDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CString CurPicPath = GetCurrentPath();	//��ȡ��ǰ����ִ�е�Ŀ¼
	CImage img;
	CRect clicentRc;
	GetClientRect(&clicentRc);
	CurPicPath += L"\\SKIN\\WhiteBoard\\image\\functionDlg\\DlgBG.png";
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,clicentRc);
	}	
	return TRUE;
	//return CDialogEx::OnEraseBkgnd(pDC);
}

void CWBSavePictureDlg::SetIsSelectBtn(int  BtnId/* = 0*/)
{
	switch(BtnId)
	{
	case IDC_RADIO_CUR_PAGE:
		{
			m_BtnCurPage.SetIsSelect(TRUE);
			m_BtnALLPage.SetIsSelect(FALSE);
			m_BtnSelectPage.SetIsSelect(FALSE);
		}
		break;
	case IDC_RADIO_ALL_PAGE:
		{
			m_BtnCurPage.SetIsSelect(FALSE);
			m_BtnALLPage.SetIsSelect(TRUE);
			m_BtnSelectPage.SetIsSelect(FALSE);
		}
		break;
	case IDC_RADIO_SELE_PAGE:
		{
			m_BtnCurPage.SetIsSelect(FALSE);
			m_BtnALLPage.SetIsSelect(FALSE);
			m_BtnSelectPage.SetIsSelect(TRUE);
		}
		break;
	default:
		break;
	}
	m_BtnCurPage.Invalidate();
	m_BtnALLPage.Invalidate();
	m_BtnSelectPage.Invalidate();
}

HBITMAP CWBSavePictureDlg::CopyPageBitmapToPicture(pWBSavePictureFile pFile,CString strName /*= _T("")*/)
{                                                                          
	HDC       hScrDC, hMemDC;       // ��Ļ���ڴ��豸������
	HBITMAP    hBitmap, hOldBitmap;    // λͼ���
	int       nX, nY, nX2, nY2;       // ѡ����������
	int       nWidth, nHeight;
	CString		strFileName = _T("");
	CRect lpRect;
	BOOL bSave = FALSE;
	lpRect = pFile->rectPage;
	bSave = pFile->bSave;
	hScrDC = pFile->PageHdc;
	strFileName = strName;
	// ȷ��ѡ������Ϊ�վ���
	if (IsRectEmpty(lpRect))
		return NULL;
	hMemDC = CreateCompatibleDC(hScrDC);
	// ���ѡ����������
	nX = lpRect.left;
	nY = lpRect.top;
	nX2 = lpRect.right;
	nY2 = lpRect.bottom;

	//ȷ��ѡ�������ǿɼ���
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;

	// ����һ������Ļ�豸��������ݵ�λͼ
	hBitmap = CreateCompatibleBitmap
		(hScrDC, nWidth, nHeight);

	// ����λͼѡ���ڴ��豸��������
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	if(bSave)
	{
		//����������DC,��bSaveΪ��ʱ�ѿ�ʼ�����ȫ��λͼ,����ȡ���δ�С����
		CDC dcCompatible;
		dcCompatible.CreateCompatibleDC(CDC::FromHandle(hMemDC));
		dcCompatible.SelectObject(m_pBitmap);

		CImage img;
		img.Create(nWidth,nHeight,32);
		HDC hdc = img.GetDC();
		SetStretchBltMode(hdc,HALFTONE);

		BitBlt(hdc, 0, 0, nWidth, nHeight,
			dcCompatible, nX, nY, SRCCOPY);				
		img.Save(strFileName);
		img.ReleaseDC();
		dcCompatible.DeleteDC();
	}
	else
	{
		BitBlt(hMemDC, 0, 0, nWidth, nHeight,
			hScrDC, nX, nY, SRCCOPY);
	}

	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	//�õ���Ļλͼ�ľ��
	//DeleteDC(hScrDC);
	DeleteDC(hMemDC);

	if(bSave)
	{
// 		if (OpenClipboard()) 
// 		{
// 			//��ռ�����
// 			EmptyClipboard();
// 			//����Ļ����ճ������������,
// 			//hBitmap Ϊ�ղŵ���Ļλͼ���
// 			SetClipboardData(CF_BITMAP, hBitmap);
// 			//�رռ�����
// 			CloseClipboard();
// 		}
	}
	return hBitmap;
}

void CWBSavePictureDlg::SaveAllPageBitmapToPicture(CString strFilePath /*= _T("")*/)
{
	if (strFilePath.IsEmpty())
	{
		return;
	}
	if (g_pWBPage == NULL || g_pWBFile == NULL || g_pWhiteBoardView == NULL)
	{
		return;
	}
	int nCountPage = g_pWBFile->GetPageCount();
	CString strName = _T("");
	CString strExt = _T("");
	CString strPageFilePath = _T("");
	CString strSavePath = _T("");
	strName = GetFileName(strFilePath);
	strExt = GetFileExt(strFilePath);
	strPageFilePath = GetFilePath(strFilePath);

	for (int nindex = 1;nindex <= nCountPage;nindex ++)
	{
		CWBPage *pPage = NULL;
		pPage = g_pWBFile->GetPage(nindex);
		if (pPage == NULL)
		{
			continue;
		}
		CString Name = _T("");
		Name = strName;
		CString strInt = _T("");
		strInt.Format(L"%d",nindex);
		if (nindex<10)
		{  
			 Name = Name + _T("-") + _T("0") + strInt;
		} 
		else
		{
			Name = Name + _T("-") + strInt;
		}
		strSavePath = strPageFilePath + _T("\\") + Name + _T(".") + strExt;
		SaveCurPageBitmapToPicture(pPage,strSavePath);
	}
}

void CWBSavePictureDlg::SaveCurPageBitmapToPicture(CWBPage *page /*= NULL*/,CString strFilePath /*= _T("")*/)
{
	if (strFilePath.IsEmpty() || page == NULL)
	{
		return;
	}
	if (PathFileExists(strFilePath))
	{
		DeleteFile(strFilePath);
	}
	if (m_pWBSSavePictureFile)
	{
		ZeroMemory(m_pWBSSavePictureFile,sizeof(WBSavePictureFile));

	}
	HDC CurPageHdc = NULL;
	CRect CurRectPage(0,0,0,0);
	CurPageHdc = page->GetMemHDC();
	CurRectPage.SetRect(0,0,g_nWidth,g_nHeight);
	m_pWBSSavePictureFile->bSave = TRUE;
	m_pWBSSavePictureFile->rectPage = CurRectPage;
	m_pWBSSavePictureFile->PageHdc = CurPageHdc;
	m_pWBSSavePictureFile->bSave = FALSE;
	m_pBitmap = CBitmap::FromHandle(CopyPageBitmapToPicture(m_pWBSSavePictureFile));
	m_pWBSSavePictureFile->bSave = TRUE;
	CopyPageBitmapToPicture(m_pWBSSavePictureFile,strFilePath);
	
}

void CWBSavePictureDlg::SaveSelctPageBitmapToPicture(CString strFilePath/* = _T("")*/)
{
	if (g_pWBFile == NULL || strFilePath.IsEmpty() || m_nPageNumber.size() == NULL)
	{
		return;
	}
	CWBPage *pPage = NULL;
	CString strName = _T("");
	CString strExt = _T("");
	CString strPageFilePath = _T("");
	CString strSavePath = _T("");
	strName = GetFileName(strFilePath);
	strExt = GetFileExt(strFilePath);
	strPageFilePath = GetFilePath(strFilePath);
	int nPage = 0;
	std::vector<int>::iterator it;
	for(it = m_nPageNumber.begin();it != m_nPageNumber.end();it ++)
	{
		nPage = *it;
		pPage = g_pWBFile->GetPage(nPage);
		if (pPage == NULL)
		{
			continue;
		}
		CString Name = _T("");
		Name = strName;
		CString strInt = _T("");
		strInt.Format(L"%d",nPage);
		if (nPage<10)
		{  
			Name = Name + _T("-") + _T("0") + strInt;
		} 
		else
		{
			Name = Name + _T("-") + strInt;
		}
		//Name = Name + _T("-") + strInt;
		strSavePath = strPageFilePath + _T("\\") + Name + _T(".") + strExt;
		SaveCurPageBitmapToPicture(pPage,strSavePath);
	}
}

CString CWBSavePictureDlg::GetFileName(CString str)
{
	CString strResult = str;
	strResult = strResult.Mid(strResult.ReverseFind(_T('\\')) +1 );
	strResult = strResult.Left(strResult.ReverseFind(_T('.')));
	return strResult;
}

CString CWBSavePictureDlg::GetFileExt(CString str)
{
	CString strResult = str;
	LPTSTR pszExtension = ::PathFindExtension(strResult);
	if (pszExtension != NULL)
		return pszExtension+1;
	strResult.Empty();
	return strResult;
}

CString CWBSavePictureDlg::GetFilePath( CString str )
{
	CString strResult = str;
	strResult = strResult.Left(strResult.ReverseFind(_T('\\')));
	return strResult;
}

bool CWBSavePictureDlg::CheckPageStr()
{
	if (g_pWBFile == NULL || g_pWBPage == NULL)
	{
		return false;
	}
	CString strWrite = _T("");
	CString strPage = _T("");
	GetDlgItemText(IDC_EDIT_WEITE_PAGE,strWrite);
	CString strPrompt;
	CString strFolderCreateFail = _T("");
	strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
	if (strPrompt.IsEmpty())
	{
		strPrompt = _T("��ʾ");
	}
	if(strWrite.IsEmpty())
	{
		strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SavePageToPictureString"));
		if (strFolderCreateFail.IsEmpty())
		{
			strFolderCreateFail = _T("����ҳ�Ų���Ϊ�գ�");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strFolderCreateFail, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFolderCreateFail);
		}
		else
		{
			AfxMessageBox(strFolderCreateFail);
		}
		return false;
	}
	//if (strWrite.CompareNoCase(m_strWritePage) != 0)
	//{
		CleanVectorPageNumber();
	//}
	m_strWritePage = strWrite;
	int ncountPage = g_pWBFile->GetPageCount();
	
// 	char tempStart = strWrite.GetAt(0);
// 	char tempEnd = strWrite.GetAt(strWrite.GetLength() - 2);
// 	std::string strtempStart = "";
// 	strtempStart.insert(strtempStart.begin(),tempStart);
// 	std::string strtempEnd = "";
// 	strtempEnd.insert(strtempEnd.begin(),tempEnd);
 //	if (atoi(strtempStart.c_str()) == 0 || atoi(strtempEnd.c_str()) == 0)
	if (_ttoi(strWrite.GetBuffer()) == 0)
	{
		strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SavePageToPictureStringStartEnd"));
		if (strFolderCreateFail.IsEmpty())
		{
			strFolderCreateFail = _T("�����ַ���ͷ��β���������ֿ�ʼ��β��");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strFolderCreateFail, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFolderCreateFail);
		}
		else
		{
			AfxMessageBox(strFolderCreateFail);
		}
		return false;
 	}
	strWrite.ReleaseBuffer();

	char *token,*dot[MAX_PATH];
	char ch[] = ","; 
	int iplen = 0,digit = 0/*, isdig = 0*/;
	char   szTemp[MAX_PATH]; 
	memset(szTemp,0,sizeof(szTemp)); 
	strcpy_s(szTemp,ToStr(strWrite.GetBuffer()).c_str());
	strWrite.ReleaseBuffer();
	token =strtok(szTemp,ch);//�ã��ָ��ַ��� 
	//��һ�η��ش��е�һ�� ',' ֮ǰ���ַ���
	//�ڶ��ε��øú���strtok(NULL,","),��һ����������ΪNULL��������طָ����ݺ�����ִ�
	while (token != NULL)
	{
		dot[iplen] = token;
		iplen++;
		token =strtok(NULL,ch);
	}
	m_bIsShow = false;
	for (int i= 0;i < iplen;i++)
	{
		int isdig = 0;
		char *strTemp = dot[i];
		int nLen = strlen(strTemp);
		for(int j = 0;j < nLen;j++)
		{
			if (isdigit(strTemp[j]))
			{
				isdig++;
			}
		}
		if (nLen == isdig)
		{
			int page = atoi(strTemp);
			if (!SavePageNumberOk(page))
			{
				continue;
			}
		}
		else if (nLen >isdig)//˵���������ַ�����2-3������
		{
			TCHAR chn[] = _T("-"); 
			int nStare = 0,nEnd = 0;
			nStare = atoi(strTemp);
			CString strFine = _T("");
			CString str = ToDRAWStr(strTemp).c_str();
			
			strFine = str.Right(str.GetLength()-1-str.Find(chn));
			nEnd = _ttoi(strFine);

			for(int nIndex = nStare;nIndex <= nEnd;nIndex ++)
			{
				SavePageNumberOk(nIndex);
			}
		}
	}
	return true;
}


string CWBSavePictureDlg::U2A( CString strUnicodeString )
{
// 	ASSERT(!::IsBadStringPtrW(strUnicodeString,-1));
// 	int cchStr = ::WideCharToMultiByte(CP_UTF8, 0, strUnicodeString, wcslen(strUnicodeString), NULL, 0, NULL, NULL);
// 	char* pstr = new char[cchStr + 1];
// 	if( pstr != NULL )
// 		::WideCharToMultiByte(CP_UTF8, 0, strUnicodeString, -1, pstr, cchStr, NULL, NULL);
// 	pstr[cchStr] = '\0';
// 	std::string str(pstr);
// 	delete pstr;
// 	return str;
	return "";
}

void CWBSavePictureDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int a = 10;
	CDialogEx::OnChar(nChar, nRepCnt, nFlags);
}

bool CWBSavePictureDlg::SavePageNumberOk( int nPage )
{
	if (g_pWBFile == NULL)
	{
		return false;
	}
	int ncountPage = g_pWBFile->GetPageCount();
	if (nPage <= ncountPage)
	{
		if (m_nPageNumber.size() == NULL)
		{
			m_nPageNumber.push_back(nPage);
		}
		else
		{
			std::vector<int>::iterator it;
			for(it = m_nPageNumber.begin();it != m_nPageNumber.end();it ++)
			{
				if (*it == nPage)
				{
					return true;
				}
			}
			m_nPageNumber.push_back(nPage);
		}
		return true;
	}
	else
	{
		if (m_bIsShow)
		{
			return false;
		}
		CString strPrompt;
		strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
		if (strPrompt.IsEmpty())
		{
			strPrompt = _T("��ʾ");
		}
		CString strFolderCreateFail;
		strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SavePageToPictureStringBig"));
		if (strFolderCreateFail.IsEmpty())
		{
			strFolderCreateFail = _T("�����ҳ�Ŵ��ڴ��ڵ�ǰ���ҳ��,ֻ����װ���ڵ�ҳ");
		}
		if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
		{
			//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strFolderCreateFail, MB_OK,true);
			g_pHPWBDataNotify->ShowTipWindow(strFolderCreateFail);
		}
		else
		{
			AfxMessageBox(strFolderCreateFail);
		}
		m_bIsShow = true;
		return false;
	}
	return false;
}

void CWBSavePictureDlg::CleanVectorPageNumber()
{
	if (m_nPageNumber.size() != NULL)
	{
		m_nPageNumber.clear();
	}
}

PJPEG2PDF CWBSavePictureDlg::Jpeg2PDF_BeginDocument( double pdfW, double pdfH )
{
	PJPEG2PDF pPDF;

	pPDF = (PJPEG2PDF)malloc(sizeof(JPEG2PDF));
	if(pPDF) {
		memset(pPDF, 0, sizeof(JPEG2PDF));
		if(JPEG2PDF_DEBUG) 
			logMsg("PDF List Inited (pPDF = %p)\n", (int)pPDF, 2,3,4,5,6);
		pPDF->pageW = (unsigned long)(pdfW * PDF_DOT_PER_INCH);
		pPDF->pageH = (unsigned long)(pdfH * PDF_DOT_PER_INCH);

		if(JPEG2PDF_DEBUG)
			logMsg("PDF Page Size (%d %d)\n", pPDF->pageW, pPDF->pageH,3,4,5,6);

		pPDF->currentOffSet = 0;
		Jpeg2PDF_SetXREF(pPDF, 0, pPDF->currentOffSet, 'f');
		pPDF->currentOffSet += sprintf(pPDF->pdfHeader, "%%PDF-1.3\r\n%%%cJpeg2PDF Engine By: HH [ihaohu@gmail.com]%c\r\n", 0xFF, 0xFF);
		if(JPEG2PDF_DEBUG) logMsg("[pPDF=%p], header: %s", (int)pPDF, (int)pPDF->pdfHeader,3,4,5,6);

		pPDF->imgObj = 0;
		pPDF->pdfObj = 2;		/* 0 & 1 was reserved for xref & document Root *///0��1�Ǳ������ļ����ļ��ĸ�
	}

	return pPDF;
}

int CWBSavePictureDlg::Jpeg2PDF_AddJpeg( PJPEG2PDF pPDF, unsigned long imgW, unsigned long imgH, unsigned long fileSize, unsigned char *pJpeg, unsigned char isColor )
{
	int result = ERROR;
	PJPEG2PDF_NODE pNode;

	if(pPDF) {
		if(pPDF->nodeCount >= MAX_PDF_PAGES) {
			logMsg("Add JPEG into PDF Skipped. Reason: Reached Max Page Number (%d) in single PDF.\n", MAX_PDF_PAGES,2,3,4,5,6);
			return result;
		}

		pNode = (PJPEG2PDF_NODE)malloc(sizeof(JPEG2PDF_NODE));
		if(pNode) {
			long nChars, currentImageObject;
			char *pFormat, lenStr[256];
			pNode->JpegW = imgW;
			pNode->JpegH = imgH;
			pNode->JpegSize = fileSize;
			pNode->pJpeg = (char*)malloc(pNode->JpegSize);
			pNode->pNext = NULL;

			if(pNode->pJpeg != NULL) {
				memcpy(pNode->pJpeg, pJpeg, pNode->JpegSize);

				/* Image Object ͼ�� ����*/
				Jpeg2PDF_SetXREF(pPDF, INDEX_USE_PPDF, pPDF->currentOffSet, 'n');
				currentImageObject = pPDF->pdfObj;


				pPDF->currentOffSet += sprintf(pNode->preFormat, "\r\n%d 0 obj\r\n<</Type/XObject/Subtype/Image/Filter/DCTDecode/BitsPerComponent 8/ColorSpace/%s/Width %d/Height %d/Length %d>>\r\nstream\r\n",
					pPDF->pdfObj, ((isColor)? "DeviceRGB" : "DeviceGray"), pNode->JpegW, pNode->JpegH, pNode->JpegSize);

				pPDF->currentOffSet += pNode->JpegSize;

				pFormat = pNode->pstFormat;
				nChars = sprintf(pFormat, "\r\nendstream\r\nendobj\r\n");
				pPDF->currentOffSet += nChars;	pFormat += nChars;
				pPDF->pdfObj++;

				/* Page Object ҳ��Ķ���*/
				Jpeg2PDF_SetXREF(pPDF, INDEX_USE_PPDF, pPDF->currentOffSet, 'n');
				pNode->PageObj = pPDF->pdfObj;
				nChars = sprintf(pFormat, "%d 0 obj\r\n<</Type/Page/Parent 1 0 R/MediaBox[0 0 %d %d]/Contents %d 0 R/Resources %d 0 R>>\r\nendobj\r\n",
					pPDF->pdfObj, pPDF->pageW, pPDF->pageH, pPDF->pdfObj+1, pPDF->pdfObj+3);
				pPDF->currentOffSet += nChars;	pFormat += nChars;
				pPDF->pdfObj++;

				/* Contents Object in Page Object ���ݶ�����ҳ��Ķ���*/
				Jpeg2PDF_SetXREF(pPDF, INDEX_USE_PPDF, pPDF->currentOffSet, 'n');
				sprintf(lenStr, "q\r\n1 0 0 1 %.2f %.2f cm\r\n%.2f 0 0 %.2f 0 0 cm\r\n/I%d Do\r\nQ\r\n",
					PDF_LEFT_MARGIN, PDF_TOP_MARGIN, pPDF->pageW - 2*PDF_LEFT_MARGIN, pPDF->pageH - 2*PDF_TOP_MARGIN, pPDF->imgObj);
				nChars = sprintf(pFormat, "%d 0 obj\r\n<</Length %d 0 R>>stream\r\n%sendstream\r\nendobj\r\n",
					pPDF->pdfObj, pPDF->pdfObj+1, lenStr);
				pPDF->currentOffSet += nChars;	pFormat += nChars;
				pPDF->pdfObj++;

				/* Length Object in Contents Object ��������ݶ���ĳ���*/
				Jpeg2PDF_SetXREF(pPDF, INDEX_USE_PPDF, pPDF->currentOffSet, 'n');
				nChars = sprintf(pFormat, "%d 0 obj\r\n%ld\r\nendobj\r\n", pPDF->pdfObj, strlen(lenStr));
				pPDF->currentOffSet += nChars;	pFormat += nChars;
				pPDF->pdfObj++;

				/* Resources Object in Page Object ��Դ����ҳ�����*/
				Jpeg2PDF_SetXREF(pPDF, INDEX_USE_PPDF, pPDF->currentOffSet, 'n');
				nChars = sprintf(pFormat, "%d 0 obj\r\n<</ProcSet[/PDF/%s]/XObject<</I%d %d 0 R>>>>\r\nendobj\r\n",
					pPDF->pdfObj, ((isColor)? "ImageC" : "ImageB"), pPDF->imgObj, currentImageObject);
				pPDF->currentOffSet += nChars;	pFormat += nChars;
				pPDF->pdfObj++;

				pPDF->imgObj++;

				/* Update the Link List ���������б�*/
				pPDF->nodeCount++;
				if(1 == pPDF->nodeCount) {
					pPDF->pFirstNode = pNode;
				} else {
					pPDF->pLastNode->pNext = pNode;
				}

				pPDF->pLastNode = pNode;

				result = OK;
			} /* pNode->pJpeg allocated OK pjpeg pNode ->�������*/
		} /* pNode is valid pNode����Ч��*/
	} /* pPDF is valid PPDF����Ч��*/

	return result;
}

unsigned long CWBSavePictureDlg::Jpeg2PDF_EndDocument( PJPEG2PDF pPDF )
{
	long headerSize, tailerSize, pdfSize = 0;

	if(pPDF) {
		char strKids[MAX_PDF_PAGES * MAX_KIDS_STRLEN], *pTail = pPDF->pdfTailer;
		long i, nChars, xrefOffSet;
		PJPEG2PDF_NODE pNode;

		/* Catalog Object. This is the Last Object Ŀ¼�����������Ķ���*/
		Jpeg2PDF_SetXREF(pPDF, INDEX_USE_PPDF, pPDF->currentOffSet, 'n');
		nChars = sprintf(pTail, "%d 0 obj\r\n<</Type/Catalog/Pages 1 0 R>>\r\nendobj\r\n", pPDF->pdfObj);
		pPDF->currentOffSet += nChars;	pTail += nChars;

		/* Pages Object. It's always the Object 1 ҳ����������Ƕ���1*/
		Jpeg2PDF_SetXREF(pPDF, 1, pPDF->currentOffSet, 'n');

		strKids[0] = 0;
		pNode = pPDF->pFirstNode;
		while(pNode != NULL) {
			char curStr[9];
			sprintf(curStr, "%d 0 R ", pNode->PageObj);
			strcat(strKids, curStr);
			pNode = pNode->pNext;
		}

		if(strlen(strKids) > 1 && strKids[strlen(strKids) - 1] == ' ') strKids[strlen(strKids) - 1] = 0;

		nChars = sprintf(pTail, "1 0 obj\r\n<</Type /Pages /Kids [%s] /Count %d>>\r\nendobj\r\n", strKids, pPDF->nodeCount);
		pPDF->currentOffSet += nChars;	pTail += nChars;

		/* The xref & the rest of the tail β�͵Ĺ���������*/
		xrefOffSet = pPDF->currentOffSet;
		nChars = sprintf(pTail, "xref\r\n0 %d\r\n", pPDF->pdfObj+1);
		pPDF->currentOffSet += nChars;	pTail += nChars;

		for(i=0; i<=pPDF->pdfObj; i++) {
			nChars = sprintf(pTail, "%s", pPDF->pdfXREF[i]);
			pPDF->currentOffSet += nChars;	pTail += nChars;
		}

		nChars = sprintf(pTail, "trailer\r\n<</Root %d 0 R /Size %d>>\r\n", pPDF->pdfObj, pPDF->pdfObj+1);
		pPDF->currentOffSet += nChars;	pTail += nChars;

		nChars = sprintf(pTail, "startxref\r\n%d\r\n%%%%EOF\r\n", xrefOffSet);
		pPDF->currentOffSet += nChars;	pTail += nChars;
	}

	headerSize = strlen(pPDF->pdfHeader);
	tailerSize = strlen(pPDF->pdfTailer);
	if( headerSize && tailerSize && ( pPDF->currentOffSet > headerSize + tailerSize ) ) {
		pdfSize = pPDF->currentOffSet;
	}

	return pdfSize;
}

int CWBSavePictureDlg::Jpeg2PDF_GetFinalDocumentAndCleanup( PJPEG2PDF pPDF, unsigned char *outPDF, unsigned long *outPDFSize )
{
	int result = ERROR;

	if(pPDF) {
		PJPEG2PDF_NODE pNode, pFreeCurrent;

		if(outPDF && (*outPDFSize >= pPDF->currentOffSet)) {
			unsigned long nBytes, nBytesOut = 0;
			unsigned char *pOut = outPDF;

			nBytes = strlen(pPDF->pdfHeader);
			memcpy(pOut, pPDF->pdfHeader, nBytes);
			nBytesOut += nBytes; pOut += nBytes;

			pNode = pPDF->pFirstNode;
			while(pNode != NULL) {
				nBytes = strlen(pNode->preFormat);
				memcpy(pOut, pNode->preFormat, nBytes);
				nBytesOut += nBytes; pOut += nBytes;

				nBytes = pNode->JpegSize;
				memcpy(pOut, pNode->pJpeg, nBytes);
				nBytesOut += nBytes; pOut += nBytes;

				nBytes = strlen(pNode->pstFormat);
				memcpy(pOut, pNode->pstFormat, nBytes);
				nBytesOut += nBytes; pOut += nBytes;

				pNode = pNode->pNext;
			}

			nBytes = strlen(pPDF->pdfTailer);
			memcpy(pOut, pPDF->pdfTailer, nBytes);
			nBytesOut += nBytes; pOut += nBytes;

			*outPDFSize = nBytesOut;
			result = OK;
		}

		pNode = pPDF->pFirstNode;
		while(pNode != NULL) {
			if(pNode->pJpeg) free(pNode->pJpeg);
			pFreeCurrent = pNode;
			pNode = pNode->pNext;
			free(pFreeCurrent);
		}

		if(pPDF) {
			free(pPDF);
			pPDF = NULL;
		}
	}

	return result;
}

int CWBSavePictureDlg::SavePdf( void )
{
	struct _finddata_t jpeg_file;
	long hFile;
	PJPEG2PDF pdfId;

	//CString str = GetCurrentPath() + "\\temporary\\a-1.jpg";
	string strname = GetCurrentPath() + "\\temporary";
	//memcpy(jpeg_file.name,strname.c_str(),strname.size());
	::chdir(strname.c_str());
	if( (hFile = _findfirst( "*.jpg", &jpeg_file )) == -1L ) {
		printf( "No *.jpg files in current directory!\n" );
		return 0;
	}

	pdfId = Jpeg2PDF_BeginDocument(5.5, 4.25);
	if(pdfId >= 0) {
		unsigned long pdfSize, pdfFinalSize;
		unsigned char  *pdfBuf;

		insertJPEGFile(jpeg_file.name, jpeg_file.size, pdfId);


		while( _findnext( hFile, &jpeg_file ) == 0 ) {
			insertJPEGFile(jpeg_file.name, jpeg_file.size, pdfId);
		}

		pdfSize = Jpeg2PDF_EndDocument(pdfId);
		pdfBuf = (unsigned char *)malloc(pdfSize);

		printf("=> Generating PDF File...");
		Jpeg2PDF_GetFinalDocumentAndCleanup(pdfId, pdfBuf, &pdfFinalSize);
		printf("Done\n");
		{
			//strname = m_strtempath;
			//::chdir(strname.c_str());
			
			//string xx = "C:\Users\Administrator\Desktop\f-1.png";
			//FILE *fp = fopen("C:\Users\Administrator\Desktop\f-1.pdf", "wb");
			std::string str = ToStr(m_strtempath.GetBuffer());
			m_strtempath.ReleaseBuffer();
			FILE *fp = fopen(str.c_str(), "wb");
			//FILE *fp = fopen(xx, "wb");
			fwrite(pdfBuf, sizeof(unsigned char), pdfFinalSize, fp);
			fclose(fp);
		}

		free(pdfBuf);
		CString str = GetCurrentPath() + "//temporary";
		DeleteDirectory(str);
	} else {
		printf("Error Init.\n");
	}

	_findclose( hFile );

	return 0;
}

int CWBSavePictureDlg::get_jpeg_size( unsigned char* data, unsigned int data_size, unsigned short *width, unsigned short *height )
{
	int i=0;  
	if(data[i] == 0xFF && data[i+1] == 0xD8 && data[i+2] == 0xFF && data[i+3] == 0xE0) {
		i += 4;
		if(data[i+2] == 'J' && data[i+3] == 'F' && data[i+4] == 'I' && data[i+5] == 'F' && data[i+6] == 0x00) {
			unsigned short block_length = data[i] * 256 + data[i+1];
			while(i<(int)data_size) {
				i+=block_length;              
				if(i >= (int)data_size) return 0;   
				if(data[i] != 0xFF) return 0;   
				if(data[i+1] == 0xC0) { 
					*height = data[i+5]*256 + data[i+6];
					*width = data[i+7]*256 + data[i+8];
					return 1;
				}
				else
				{
					i+=2;                             

					block_length = data[i] * 256 + data[i+1];  

				}
			}
			return 0;                   

		}else{ return 0; }                 

	}else{ return 0; }                  
}

void CWBSavePictureDlg::insertJPEGFile( const char *fileName, int fileSize, PJPEG2PDF pdfId )
{
	FILE  *fp;
	unsigned char *jpegBuf;
	int readInSize; 
	unsigned short jpegImgW, jpegImgH;

	jpegBuf = (unsigned char *)malloc(fileSize);

	fp = fopen(fileName, "rb");
	readInSize = fread(jpegBuf, sizeof(unsigned char), fileSize, fp);
	fclose(fp);

	if(readInSize != fileSize) 
		printf("Warning: File %s should be %d Bytes. But only read in %d Bytes.\n", fileName, fileSize, readInSize);

	if(1 == get_jpeg_size(jpegBuf, readInSize, &jpegImgW, &jpegImgH)) {
		printf("Adding %s (%dx%d) into PDF\n", fileName, jpegImgW, jpegImgH);
		Jpeg2PDF_AddJpeg(pdfId, jpegImgW, jpegImgH, readInSize, jpegBuf, 1);
	} else {
		printf("Warning: Can't obtain image dimension from %s. Skipped.\n", fileName);
	}

	free(jpegBuf);
}

void CWBSavePictureDlg::Jpeg2PDF_SetXREF( PJPEG2PDF pPDF, int index, int offset, char c )
{
	if(INDEX_USE_PPDF == index) 
		index = pPDF->pdfObj;

	if('f' == c) 
		sprintf(pPDF->pdfXREF[index], "%010d 65535 f\r\n", offset);
	else
		sprintf(pPDF->pdfXREF[index], "%010d 00000 %c\r\n", offset, c);

	if(JPEG2PDF_DEBUG) 
		logMsg("pPDF->pdfXREF[%d] = %s", index, (int)pPDF->pdfXREF[index], 3,4,5,6);
}

BOOL CWBSavePictureDlg::DeleteDirectory( CString strDir )
{
	if (strDir.GetLength() == 0)
	{
		return false;
	}
	CFileFind tempFind; 
	char tempFileFind[MAX_PATH]; 
	sprintf(tempFileFind,"%s\\*.*",strDir); 
	BOOL IsFinded=(BOOL)tempFind.FindFile(ToDRAWStr(tempFileFind).c_str()); 
	while(IsFinded) 
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile(); 
		if(!tempFind.IsDots()) 
		{ 
			TCHAR foundFileName[MAX_PATH]; 
			wcsncpy(foundFileName,tempFind.GetFileName().GetBuffer(MAX_PATH),tempFind.GetFileName().GetLength()); 
			tempFind.GetFileName().ReleaseBuffer();
			if(tempFind.IsDirectory()) 
			{ 
				//char tempDir[MAX_PATH]; 
				//sprintf(tempDir,"%s\\%s",strDir,foundFileName); 
				DeleteDirectory(foundFileName); 
			} 
			else 
			{ 
				//char tempFileName[MAX_PATH]; 
				//sprintf(tempFileName,"%s\\%s",strDir,foundFileName); 
				DeleteFile(foundFileName); 
			} 
		} 
	} 
	tempFind.Close(); 
	if(!RemoveDirectory(strDir)) 
	{ 	
		return FALSE; 
	} 

	return TRUE; 
}

