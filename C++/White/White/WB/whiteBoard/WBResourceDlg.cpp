// CWBResourceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBResourceDlg.h"
#include "afxdialogex.h"
#include <gdiplus.h>
#include "common/XYIniReader.h"
#include "shlwapi.h" 
using namespace Gdiplus;

//����֧�ָ������͵��ļ��ĺ�׺�����÷ֺŸ��������ﶨ������ͽ�����Դ������ʾ
//��Ҫ����ĳһ���ͣ����ռ���
#define  PICTURETYPE "bmp;jpg;gif;tif;png;dib;tiff;jfif;ico;jpeg"
#define  DOCTYPE "doc;ppt;xls;pdf"
#define  MOVIETYPE "avi;wmv;mpeg;mpg"
#define  MUISCTYPE ""
#define  FLASHTYPE "swf;"

// CWBResourceDlg �Ի���

IMPLEMENT_DYNAMIC(CWBResourceDlg, CDialogEx)

//��������ͼ��С
#define	THUMBNAIL_WIDTH		100
#define	THUMBNAIL_HEIGHT	75

CWBResourceDlg::CWBResourceDlg(CWhiteBoardView* pParent /*=NULL*/)
	: CDialogEx(CWBResourceDlg::IDD, pParent)
{
	m_pWhiteBoardView = pParent;
	m_strImageDir = _T("");	
	m_ImageListTab = NULL;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

CWBResourceDlg::~CWBResourceDlg()
{
	if(m_ImageListTab)
	{
		delete m_ImageListTab;
		m_ImageListTab = NULL;
	}
	DestroyButtons();
}

void CWBResourceDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST_RESOURCE, m_ListThumbnail);
	CDialogEx::DoDataExchange(pDX);

}


BEGIN_MESSAGE_MAP(CWBResourceDlg, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_RESOURCETAB, &CWBResourceDlg::OnTcnSelchangeResourcetab)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESOURCE, &CWBResourceDlg::OnLvnItemchangedListResource)	
	ON_BN_CLICKED(IDC_ADDRESOURCE, &CWBResourceDlg::OnBnClickedAddresource)
	ON_BN_CLICKED(IDC_DELRESOURCE, &CWBResourceDlg::OnBnClickedDelresource)
	ON_BN_CLICKED(IDC_SETDEFAULT, &CWBResourceDlg::OnBnClickedSetdefault)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_RESOURCE, &CWBResourceDlg::OnLvnBegindragListResource)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RESOURCE, &CWBResourceDlg::OnNMDblclkListResource)
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_HIDERESOURCE, &CWBResourceDlg::OnBnClickedHideresource)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()


// CWBResourceDlg ��Ϣ�������


void CWBResourceDlg::OnTcnSelchangeResourcetab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//�õ���ǰѡ�б�ǩ�����������ػ��б��
	int nItem = m_TabDir.GetCurSel();
	m_strImageDir = m_VectorDirPath[nItem];
	LoadThumb();
	*pResult = 0;
}


void CWBResourceDlg::OnLvnItemchangedListResource(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	*pResult = 0;
}

BOOL CWBResourceDlg::OnInitDialog()
{	
	//CreateButton();
	//����ͼƬ�б�
	m_ImageListThumb.Create(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1);
	CRect rect;
	GetClientRect(&rect);
	rect.top += 33;	
	//�����б�򣬲�����ͼƬ�б�
	m_ListThumbnail.Create(LVS_SINGLESEL|LVS_ALIGNLEFT|WS_VISIBLE|LVS_ICON, rect, this, IDC_LIST_RESOURCE);
	m_ListThumbnail.SetImageList(&m_ImageListThumb, LVSIL_NORMAL);


	//�����б���ͼ�ؼ�����
	CString CurPicPath = GetCurrentPath();	//��ȡ��ǰ����ִ�е�Ŀ¼	
	CString SetDefaultPath = CurPicPath;
	CurPicPath += "\\image\\ListCtrlBG.png";        // ����LISTCTRL�ı���ͼ·��
	m_ListThumbnail.SetBkImage( CurPicPath.GetBuffer(CurPicPath.GetLength()) );
	m_ListThumbnail.SetTextBkColor(RGB(187, 217, 244));

	//���ð�ť
	CreateButton();
	rect.top -= 31;
	rect.left += 10;
	int temp =rect.bottom - 35;
	rect.bottom =rect.bottom - temp;
	rect.right -= 300;
	//������ǩ�ؼ�
	m_TabDir.Create(WS_VISIBLE | TCS_OWNERDRAWFIXED , rect, this, IDC_RESOURCETAB);

	//������Դ�������ļ����ɱ�ǩ
	SetTab();	
	m_TabDir.Invalidate();
	m_strFileName = "";
	m_bAddItem = FALSE; 

	return TRUE;
}

BOOL  CWBResourceDlg::GetImageFileNames()
{			
	CString	strExt;
	CString	strName;
	CString	strPattern;
	BOOL	bRC = TRUE;

	HANDLE					hFind = NULL;
	WIN32_FIND_DATA			FindFileData;
	std::vector<CString>	VectorImageNames;

	if ( m_strImageDir[m_strImageDir.GetLength() - 1] == TCHAR('\\') )	
	{
		strPattern.Format( TEXT("%s*.*"), m_strImageDir );
	}
	else
	{
		strPattern.Format( TEXT("%s\\*.*"), m_strImageDir );
	}

	hFind = ::FindFirstFile(strPattern, &FindFileData);	// strat search	
	if (hFind == INVALID_HANDLE_VALUE)
	{
		LPVOID  msg;
		::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
			NULL, 
			GetLastError(), 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&msg, 
			0, 
			NULL);
	//	MessageBox((LPTSTR)msg, CString((LPCSTR)IDS_TITLE), MB_OK|MB_ICONSTOP);
		::LocalFree(msg);

		return FALSE;
	}
/*
	// filter off the system files and directories
	if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  &&
		!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)     &&
		!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)     &&
		!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY))
	{  	  
		// test file extension
		strName = FindFileData.cFileName;
		strExt = PathFindExtension(strName);
		strExt = strExt.Right(strExt.GetLength() - 1);

		if ( (strExt.CompareNoCase( TEXT("bmp") ) == 0) ||
			(strExt.CompareNoCase( TEXT("jpg") ) == 0) ||
			(strExt.CompareNoCase( TEXT("gif") ) == 0) ||
			(strExt.CompareNoCase( TEXT("tif") ) == 0) ||
			(strExt.CompareNoCase( TEXT("png") ) == 0) ||
			(strExt.CompareNoCase( TEXT("dib") ) == 0) ||
			(strExt.CompareNoCase( TEXT("tiff") ) == 0) ||
			(strExt.CompareNoCase( TEXT("jfif") ) == 0) ||
			(strExt.CompareNoCase( TEXT("ico") ) == 0) ||
			(strExt.CompareNoCase(TEXT("jpeg")) == 0))
		{
			// save the image file name
			VectorImageNames.push_back(strName);
		}
	}  
	*/
	// loop through to add all of them to our vector	
	while (bRC)
	{
		bRC = ::FindNextFile(hFind, &FindFileData);
		if (bRC)
		{
			// filter off the system files and directories
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)     &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)     &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY))
			{
				// test file extension
				strName = FindFileData.cFileName;
				strExt = PathFindExtension(strName);
				strExt = strExt.Right(strExt.GetLength() - 1);
				//���������ͺ�׺��������һ���ַ���
				CString strFileType;
				strFileType = PICTURETYPE;
				strFileType += MOVIETYPE;
				strFileType += DOCTYPE;
				strFileType += MUISCTYPE;
				strFileType += FLASHTYPE;
				strExt.MakeLower();			
				//�ж������Ƿ���֧�ֵ�����
				if (strFileType.Find(strExt) != -1)
				{
					// save the image file name
					VectorImageNames.push_back(strName);
				}
			}
		}  
		else
		{
			DWORD err = ::GetLastError();
			if (err !=  ERROR_NO_MORE_FILES)
			{
				LPVOID msg;
				::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
					NULL, err, 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR)&msg, 0, NULL);
		//		MessageBox((LPTSTR)msg, CString((LPCSTR)IDS_TITLE), MB_OK|MB_ICONSTOP);
				::LocalFree(msg);
				::FindClose(hFind);
				return FALSE;
			}
		}
	} // end of while loop

	// close the search handle
	::FindClose(hFind);

	// update the names, if any
	if ( !VectorImageNames.empty() )
	{
		// reset the image name vector
		m_VectorImageNames.clear();
		m_VectorImageNames = VectorImageNames;
		return TRUE;
	}

	return FALSE;
}


HRESULT CWBResourceDlg::CreateThumbnail(CString strFileName, HBITMAP* pThumbnail)  
{  
	LPITEMIDLIST pidlItems = NULL, pidlURL = NULL, pidlWorkDir = NULL;
	HRESULT hr;  
	WCHAR pszPath[MAX_PATH];  
	DWORD dwFlags = IEIFLAG_ASPECT;  
	SIZE size = { THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT };  
	IExtractImage* peiURL = NULL;	// nterface is used to request a thumbnail image from a Shell folder
	IShellFolder* psfDesktop = NULL;  
	IShellFolder* psfWorkDir = NULL;  
	IMalloc*	  pMalloc = NULL;	
	// ��ʱ������
	wchar_t wsBuffer[MAX_PATH];

	// ��ʼ��Com��
	hr = CoInitialize( NULL );
	if ( FAILED( hr ) )
	{
		AfxMessageBox( _T( "��ʼ��Com��ʧ��!" ) );
		goto OnExit;
	}

	// ���IMalloc�ӿ�
	hr = SHGetMalloc( &pMalloc );
	if ( FAILED( hr ) )
	{
		goto OnExit;
	}

	// ��������ļ���
	hr = SHGetDesktopFolder(&psfDesktop);  

	if(FAILED(hr)) goto OnExit;  	

	// get working directory  	
	MultiByteToWideChar(CP_ACP, 0, m_strImageDir, -1, wsBuffer, MAX_PATH);

	// ParseDisplayName:Translates a file object's or folder's display name into an item identifier list.
	hr = psfDesktop->ParseDisplayName(NULL, NULL, wsBuffer, NULL, &pidlWorkDir, NULL);  
	if(FAILED(hr)) goto OnExit;  
	hr = psfDesktop->BindToObject(pidlWorkDir, NULL, IID_IShellFolder, (LPVOID*)&psfWorkDir);  
	if(FAILED(hr)) goto OnExit;  
	psfDesktop->Release();  
	psfDesktop = NULL;  
	pMalloc->Free(pidlWorkDir);  
	pidlWorkDir = NULL;  

	// retrieve link information  
	MultiByteToWideChar(CP_ACP, 0, strFileName, -1, wsBuffer, MAX_PATH);
	hr = psfWorkDir->ParseDisplayName(NULL, NULL, wsBuffer, NULL, &pidlURL, NULL);  
	if(FAILED(hr)) goto OnExit;  

	// query IExtractImage  
	hr = psfWorkDir->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST*)&pidlURL, IID_IExtractImage, NULL, (LPVOID*)&peiURL);  
	if(FAILED(hr)) goto OnExit;  

	// define thumbnail properties  
	hr = peiURL->GetLocation(pszPath, MAX_PATH, NULL, &size, 24, &dwFlags);  
	if(FAILED(hr)) goto OnExit;  

	// generate thumbnail  
	hr = peiURL->Extract(pThumbnail);  
	if(FAILED(hr)) goto OnExit;  

	// clean-up IExtractImage  
	peiURL->Release();  
	peiURL = NULL;  

OnExit:  

	// free allocated structures  
	if(peiURL != NULL) peiURL->Release();  
	if(pidlURL != NULL) pMalloc->Free(pidlURL);  
	if(pidlWorkDir != NULL) pMalloc->Free(pidlWorkDir);  
	if(psfDesktop != NULL) psfDesktop->Release();  
	if(psfWorkDir != NULL) psfWorkDir->Release();  
	CoUninitialize();

	return hr;  
}


void  CWBResourceDlg::DrawThumbnails()
{
	CBitmap*    pImage = NULL;
	HBITMAP		hBmp = NULL;
	POINT		pt;
	CString		strPath;
	int			i;

	// no images
	if (m_VectorImageNames.empty())
	{
		return;
	}

	// set the length of the space between thumbnails
	// you can also calculate and set it based on the length of your list control
	int nGap = 20;

	// hold the window update to avoid flicking
	m_ListThumbnail.SetRedraw(FALSE);

	// reset our image list
	for (i = 0; i<m_ImageListThumb.GetImageCount(); i++)
	{
		m_ImageListThumb.Remove(i);	
	}
	

	// remove all items from list view
	if (m_ListThumbnail.GetItemCount() != 0)
	{
		m_ListThumbnail.DeleteAllItems();
	}

	// set the size of the image list
	m_ImageListThumb.SetImageCount(m_VectorImageNames.size());


	i = 0;

	// draw the thumbnails
	std::vector<CString>::iterator	iter;
	for (iter = m_VectorImageNames.begin(); iter != m_VectorImageNames.end(); iter++)
	{			
		HRESULT hr;

		hr = CreateThumbnail(*iter, &hBmp);	

		if (SUCCEEDED(hr))
		{			
			Bitmap img( hBmp, NULL );
			Bitmap* pThumbnail = static_cast<Bitmap*>(img.GetThumbnailImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, NULL, NULL));

			// attach the thumbnail bitmap handle to an CBitmap object
			pThumbnail->GetHBITMAP(NULL, &hBmp);
			pImage = new CBitmap();		 
			pImage->Attach(hBmp);

			// add bitmap to our image list
			m_ImageListThumb.Replace(i, pImage, NULL);

			// put item to display
			// set the image file name as item text
			m_ListThumbnail.InsertItem(i, m_VectorImageNames[i], i);

			// get current item position	 
			m_ListThumbnail.GetItemPosition(i, &pt);	 

			// shift the thumbnail to desired position
			pt.x = nGap + i*(THUMBNAIL_WIDTH + nGap);
			m_ListThumbnail.SetItemPosition(i, pt);
			i++;

			delete pImage;
			delete pThumbnail;
		} 
		else		//��Ŀǰ�в������������ļ���������ͼ���ʶ�ȡ���ǵ�ͼ��
		{	
			SHFILEINFO sfi;
			
			strPath.Format( TEXT("%s\\%s"), m_strImageDir, *iter );

			SHGetFileInfo (strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),SHGSI_ICON|SHGSI_SHELLICONSIZE |SHGFI_USEFILEATTRIBUTES );
			
			m_ImageListThumb.Replace(i, sfi.hIcon);
			
			m_ListThumbnail.InsertItem(i, m_VectorImageNames[i], i);

			

			// get current item position	 
			m_ListThumbnail.GetItemPosition(i, &pt);

			// shift the thumbnail to desired position
			pt.x = nGap + i*(THUMBNAIL_WIDTH + nGap);
			m_ListThumbnail.SetItemPosition(i, pt);
			i++;
		}
		
	}

	// let's show the new thumbnails
	m_ListThumbnail.SetRedraw(); 
}

void CWBResourceDlg::LoadThumb()
{
/*	// validate image directory
	if (m_strImageDir.IsEmpty())
	{
		MessageBox(CString((LPCSTR)IDS_DIR_ERROR), CString((LPCSTR)IDS_TITLE), MB_OK|MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_PATH)->SetFocus();
		return;
	}
	*/
	// show hour glass cursor
	BeginWaitCursor();

	// get the names of bitmap files
	if ( !GetImageFileNames() )
	{
		// hold the window update to avoid flicking
		m_ListThumbnail.SetRedraw(FALSE);

		// reset our image list
		for (int i = 0; i<m_ImageListThumb.GetImageCount(); i++)
		{
			m_ImageListThumb.Remove(i);	
		}
	

		// remove all items from list view
		if (m_ListThumbnail.GetItemCount() != 0)
		{
			m_ListThumbnail.DeleteAllItems();
		}
		m_ListThumbnail.SetRedraw(TRUE); 
		EndWaitCursor();		
		return;
	}

	// draw thumbnail images in list control
	DrawThumbnails();	

	// set focus and select the first thumbnail in the list control
	m_ListThumbnail.SetFocus();
	m_ListThumbnail.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
	EndWaitCursor();
}
//��̬���ɱ�ǩ
BOOL CWBResourceDlg::SetTab()
{
	//char *pValue = new char[255];
	char ***buf = new char**; 
	CString strTabName;
	DWORD TabNum = 0;
	DWORD pBuffsize = 256;
	XYIniReader configIni(".\\WBResConfig.ini");	
	configIni.GetSectionData("��Դ��", buf, &TabNum, &pBuffsize);
	*buf = new char *[TabNum];
	for (int i = 0; i < TabNum; i++)
	{
		buf[0][i] = new char [256];
	}
	configIni.GetSectionData("��Դ��", buf, &TabNum, &pBuffsize);

	//���û���κμ�ֵ�򷵻�
	if (TabNum == 0)
	{
		return FALSE;
	}
	
	//����ͼ���б�
	if (NULL != m_ImageListTab)
	{  
		delete m_ImageListTab;
	}
	m_ImageListTab = new CImageList;
	if (!(m_ImageListTab->Create(30,30,ILC_COLOR24|ILC_MASK,1,1)))
	{  
		MessageBox(_T("����ImageListChild����")); 
		return FALSE;
	} 

	m_TabDir.SetImageList(m_ImageListTab);
	//����ͼ���С
	m_TabDir.SetItemSize(CSize(50,30)); 
	for (int i = 0; i < TabNum; i++)
	{
	/*	CString str;
		str = buf[0][i];
		int nNameLength, nStrLength;			
		nNameLength = str.Find("*");	//�ж��ļ���·����ʶ����λ��,��"*"��λ�ã�
		strTabName = str.Left(nNameLength - 1);	//��ȡ�ļ�����
		nStrLength = str.GetLength();		
		m_VectorDirPath.push_back(str.Right(nStrLength - nNameLength - 1));		//��ȡ�ļ���·��
		m_TabDir.InsertItem(i, strTabName); 	*/
		char *strPath = new char[MAX_PATH];
		char *strName = new char[MAX_PATH];
		configIni.GetKeyNameValue(strName, strPath, "*", buf[0][i]);
		strTabName = strName;		
		m_VectorDirPath.push_back(strPath);		//��ȡ�ļ���·��
		m_TabDir.InsertItem(i,strTabName);		

		delete strPath;
		delete strName;
	}

	m_strImageDir = m_VectorDirPath[0];
	LoadThumb();
	m_TabDir.Invalidate();
	for (int i = 0; i < TabNum; i++)
	{
		delete buf[0][i];
	}
	delete buf[0];
	delete buf;
	return TRUE;
}

void CWBResourceDlg::OnBnClickedAddresource()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO   bBinfo; 	
	char *pDirName = new char[MAX_PATH];	
	memset(&bBinfo,   0,   sizeof(BROWSEINFO));     //����ṹ����ʼ�� 
	bBinfo.hwndOwner=this->m_hWnd;//���öԻ��������߾�� 
	char   strTmp[255]; 
	bBinfo.lpszTitle= "��ѡ����Ҫ��ӵ���Դ�ļ��У� "; 	
	bBinfo.ulFlags   =  BIF_STATUSTEXT | BIF_USENEWUI|BIF_RETURNONLYFSDIRS;	 //���ñ�־ֻ����ѡ��Ŀ¼    
	bBinfo.pszDisplayName = pDirName;	//�����ļ�����
	LPITEMIDLIST   lpDlist; 
	//�������淵����Ϣ��IDList��ʹ��SHGetPathFromIDList����ת��Ϊ�ַ��� 
	lpDlist=SHBrowseForFolder(&bBinfo);   //��ʾѡ��Ի��� 
	if(lpDlist!=NULL) 
	{ 
		SHGetPathFromIDList(lpDlist,strTmp);//����Ŀ��ʶ�б�ת����Ŀ¼ 		 
		m_strImageDir = strTmp;
		if (m_strImageDir != "\0")
		{
			for (int i = 0; i<m_VectorDirPath.size(); i++)
			{
				if (m_strImageDir == m_VectorDirPath[i])
				{
					TCITEM pItem;
					TCHAR TabName[256] = {0};
					CString str;

					pItem.pszText = TabName;
					pItem.cchTextMax = 256;
					pItem.mask = TCIF_TEXT;		//���ö�ȡ��ǩ��

					//�õ���ǰ��ǩ��
					m_TabDir.GetItem(i, &pItem);					
					str.Format("���Ѿ�������ļ�������� %s ��ǩ���ˣ�", (CString)TabName);
					AfxMessageBox(str);
					delete pDirName;
					return;
				}
			}
			//��*������ļ���·��֮ǰ�Ա�������ʶ�ļ���·��
			WritePrivateProfileString("��Դ��", pDirName, "*" + m_strImageDir, ".\\WBResConfig.ini");
			int n = m_TabDir.GetItemCount();
			m_VectorDirPath.push_back(m_strImageDir);	
			m_TabDir.InsertItem(n, pDirName);		
			m_TabDir.SetCurSel(n);
			m_TabDir.Invalidate();
			LoadThumb();
		}
		else
		{
			AfxMessageBox("��ѡ��Ĳ�����Ч���ļ��У�");
		}
	} 
	delete pDirName;
}


void CWBResourceDlg::OnBnClickedDelresource()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nItem = m_TabDir.GetCurSel();
	TCITEM pItem;
	TCHAR TabName[256] = {0};

	pItem.pszText = TabName;
	pItem.cchTextMax = 256;
	pItem.mask = TCIF_TEXT;		//���ö�ȡ��ǩ��

	//�õ���ǰ��ǩ��
	m_TabDir.GetItem(nItem, &pItem);

	//�ж��Ƿ�Ϊ��һ����ǩ����Ĭ����Դ�⣬����ǣ���ɾ��
	if (nItem == 0)
	{
		return;
	}

	//ɾ���������ļ��еļ�ֵ
	WritePrivateProfileString("��Դ��", TabName, NULL, ".\\WBResConfig.ini");

	//ɾ���ļ���·��
	std::vector<CString>::iterator	iter;
	for(iter = m_VectorDirPath.begin(); iter <= m_VectorDirPath.end(); iter++)
	{
		if (m_VectorDirPath.size() == 0)
		{
			break;
		}
		if (*iter == m_VectorDirPath[nItem])
		{
			m_VectorDirPath.erase(iter);
			break;
		}
	}

	//�������ñ�ǩ���㣬�ж��Ƿ�ɾ����һ����������һ������
	if (m_TabDir.GetItemCount() != 0 && m_TabDir.GetItemCount() != 1)
	{
		if (nItem == 0)
		{
			m_TabDir.DeleteItem(0);
			m_TabDir.SetCurSel(0);
			m_strImageDir = m_VectorDirPath[0];
			LoadThumb();
		}
		else
		{
			m_TabDir.DeleteItem(nItem);	
			m_TabDir.SetCurSel(nItem - 1);
			m_strImageDir = m_VectorDirPath[nItem - 1];
			LoadThumb();
		}		
	}
	else if (m_TabDir.GetItemCount() == 0)
	{
		return;
	}
	else
	{
		m_TabDir.DeleteItem(nItem);
		m_strImageDir = "";
//		m_TabDir.DestroyWindow();	//���ٱ�ǩ
		m_ListThumbnail.DeleteAllItems();
		m_ListThumbnail.SetRedraw();
	}
	m_TabDir.Invalidate();
}


void CWBResourceDlg::OnBnClickedSetdefault()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO   bBinfo; 	
	char *pDirName = new char[MAX_PATH];	
	memset(&bBinfo,   0,   sizeof(BROWSEINFO));     //����ṹ����ʼ�� 
	bBinfo.hwndOwner=this->m_hWnd;//���öԻ��������߾�� 
	char   strTmp[255]; 
	bBinfo.lpszTitle= "��ѡ����Ҫ����ΪĬ����Դ����ļ��У� "; 	
	bBinfo.ulFlags   =  BIF_STATUSTEXT | BIF_USENEWUI|BIF_RETURNONLYFSDIRS;	 //���ñ�־ֻ����ѡ��Ŀ¼    
	bBinfo.pszDisplayName = pDirName;	//�����ļ�����
	LPITEMIDLIST   lpDlist; 
	//�������淵����Ϣ��IDList��ʹ��SHGetPathFromIDList����ת��Ϊ�ַ��� 
	lpDlist=SHBrowseForFolder(&bBinfo);   //��ʾѡ��Ի��� 
	if(lpDlist!=NULL) 
	{ 
		SHGetPathFromIDList(lpDlist,strTmp);//����Ŀ��ʶ�б�ת����Ŀ¼ 		 
		m_strImageDir = strTmp;
		if (m_strImageDir != "\0")
		{
			//��*������ļ���·��֮ǰ�Ա�������ʶ�ļ���·��
			WritePrivateProfileString("��Դ��", "��Դ��", "*" + m_strImageDir, ".\\WBResConfig.ini");
			int n = m_TabDir.GetItemCount();
			m_VectorDirPath[0] = m_strImageDir;			
			m_TabDir.SetCurSel(0);
			LoadThumb();
		}
		else
		{
			AfxMessageBox("��ѡ��Ĳ�����Ч���ļ��У�");
		}
	} 
	delete pDirName;
}


void CWBResourceDlg::OnLvnBegindragListResource(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (g_bIsDrawScreen) //�����Ļ��ͼ�򿪾�����
	{
		return;
	}

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	POSITION   pos=m_ListThumbnail.GetFirstSelectedItemPosition(); 
	while(pos) //�ж���ק�Ƿ�ѡ���ļ�
	{ 
		m_bAddItem = TRUE;
		int   nItem = m_ListThumbnail.GetNextSelectedItem(pos); 
		m_ListThumbnail.SetFocus();
		//ʹ�װ���ͼ����Ӵ������ַ��µ������Ϣ
		g_pWhiteBoardView->EnableMouseOrTouch();
		CheckType(nItem);
		switch(m_nFileType)
		{
		case PICTURE:
			{
				g_nDrawStatus = ADDPICTURE;
			}
			break;
		case MOVIE:
			{
				g_nDrawStatus = ADDMOVIE;
			}
			break;
		case DOC:
			{
				g_nDrawStatus = ADDDOC;
			}
			break;
		case MUISC:
			{
				g_nDrawStatus = ADDMUISC;
			}
			break;
		case FLASH:
			{
				g_nDrawStatus = ADDFLASH;
			}
			break;
		default:
			break;
		}
	}
	*pResult = 0;
}


void CWBResourceDlg::OnNMDblclkListResource(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (g_bIsDrawScreen) //�����Ļ��ͼ�򿪾�����
	{
		return;
	}

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	POSITION   pos=m_ListThumbnail.GetFirstSelectedItemPosition(); 
	while(pos) //�ж�˫��λ���Ƿ�ѡ���ļ�
	{ 
		int   nItem = m_ListThumbnail.GetNextSelectedItem(pos);  
		CheckType(nItem);

		// �����жϳ������������Ӧ�Ķ���
		switch(m_nFileType)
		{
		case PICTURE:
			{
				CWBPicture *pWBPicture = new CWBPicture(m_strFileName);
				//����ͼƬ������ܳ���view�Ĵ�С4-21����
				CRect viewRect;
				g_pWhiteBoardView->GetClientRect(&viewRect);
				pWBPicture->SetPictureBigerSzie(viewRect);

				g_pWBPage->AddWBObject(pWBPicture);
				g_pWhiteBoardView->Invalidate();

				//����ͬ����Ϣ
				XMemIni xmlObj;
				pWBPicture->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);

				//��Ӳ�����¼
				g_pWBFile->AddOperation(_T("����ͼƬ"));
			}
			break;
		case MOVIE:
			{
				CWBMovie *pWBMovie = new CWBMovie(m_strFileName);
		
				g_pWBPage->AddWBObject(pWBMovie);
				pWBMovie->Start();
				g_pWhiteBoardView->Invalidate();

				//����ͬ����Ϣ
				XMemIni xmlObj;
				pWBMovie->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);

				//��Ӳ�����¼
				g_pWBFile->AddOperation(_T("������Ƶ"));
			}
			break;
		case DOC:
			{
				CWBDocument *pWBDocument = new CWBDocument(m_strFileName);
				g_pWBPage->AddWBObject(pWBDocument);
				g_pWhiteBoardView->Invalidate();

				//����ͬ����Ϣ
				XMemIni xmlObj;
				pWBDocument->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);

				//��Ӳ�����¼
				g_pWBFile->AddOperation(_T("�����ĵ�"));
			}
			break;
		case MUISC:
			{
				
			}
			break;
		case FLASH:
			{
				CWBFlash *pWBFlash = new CWBFlash(m_strFileName);
	
				g_pWBPage->AddWBObject(pWBFlash);
				pWBFlash->init();
				g_pWhiteBoardView->Invalidate();

				//����ͬ����Ϣ
				XMemIni xmlObj;
				pWBFlash->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);

				//��Ӳ�����¼
				g_pWBFile->AddOperation(_T("����FLASH"));
			}
			break;
		default:
			break;
		}
				
	}	
	*pResult = 0;
}

//�ж��ļ�����
void CWBResourceDlg::CheckType(int nItem)
{
	CString strExt;
	m_strFileName = m_ListThumbnail.GetItemText(nItem, 0);
	strExt = PathFindExtension(m_strFileName);
	strExt = strExt.Right(strExt.GetLength() - 1);
	m_strFileName = m_strImageDir + "\\" + m_strFileName;	//��ȡ�ļ���	

	CString strFileType;
	strFileType = PICTURETYPE;	//����ͼƬ���ͺ�׺����ɵ��ַ���
	strExt.MakeLower();
	if (strFileType.Find(strExt) != -1)
	{
		m_nFileType = PICTURE;
		return;
	}

	strFileType = MOVIETYPE;	//������Ƶ���ͺ�׺����ɵ��ַ���
	if (strFileType.Find(strExt) != -1)
	{
		m_nFileType = MOVIE;
		return;
	}

	strFileType = DOCTYPE;		//�����ĵ����ͺ�׺����ɵ��ַ���
	if (strFileType.Find(strExt) != -1)
	{
		m_nFileType = DOC;
		return;
	}

	strFileType = MUISCTYPE;	//������Ƶ���ͺ�׺����ɵ��ַ���
	if (strFileType.Find(strExt) != -1)
	{
		m_nFileType = MUISC;
		return;
	}

	strFileType = FLASHTYPE;	//����FLASH���ͺ�׺����ɵ��ַ���
	if (strFileType.Find(strExt) != -1)
	{
		m_nFileType = FLASH;		
		return;
	}
}

LRESULT CWBResourceDlg::OnNcHitTest(CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	if (!m_bAddItem)
	{
		return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);
	}	
	return CDialogEx::OnNcHitTest(point);
}


void CWBResourceDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (m_bAddItem)
	{
		g_nDrawStatus = DRAW_NORMAL;
		m_bAddItem= FALSE;
		g_pWhiteBoardView->EnableMouseOrTouch(FALSE);
	}
	
	CDialogEx::OnLButtonUp(nFlags, point);
}


BOOL CWBResourceDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	//�������������Ϣ���ж��Ƿ�����ק״̬��������ק״̬��ת����Ӷ���״̬	
	if(   pMsg-> message   ==   WM_LBUTTONUP) 
	{   
		if (m_bAddItem)
		{
			int nId = ::GetDlgCtrlID(pMsg->hwnd);
			switch(nId)
			{
			case IDC_ADDRESOURCE:
			case IDC_DELRESOURCE:
			case IDC_SETDEFAULT:
			case IDC_LIST_RESOURCE:
			case IDC_RESOURCETAB:
			case IDC_HIDERESOURCE:
				{
					GetDlgItem(nId)->SendMessage(WM_LBUTTONUP);
					m_bAddItem= FALSE;
					g_nDrawStatus = DRAW_NORMAL;
					g_pWhiteBoardView->EnableMouseOrTouch(FALSE);
				}
				break;
			default:
				break;
			}

// 			if(pMsg-> hwnd   ==   GetDlgItem(IDC_ADDRESOURCE)-> m_hWnd) 
// 			{ 
// 				GetDlgItem(IDC_ADDRESOURCE)-> SendMessage(WM_LBUTTONUP);
// 				m_bAddItem= FALSE;
// 				g_nDrawStatus = DRAW_NORMAL;
// 				return   TRUE; 
// 			}
// 			if(pMsg-> hwnd   ==   GetDlgItem(IDC_DELRESOURCE)-> m_hWnd) 
// 			{ 
// 				GetDlgItem(IDC_DELRESOURCE)-> SendMessage(WM_LBUTTONUP); 
// 				m_bAddItem= FALSE;
// 				g_nDrawStatus = DRAW_NORMAL;
// 				return   TRUE; 
// 			}
// 			if(pMsg-> hwnd   ==   GetDlgItem(IDC_SETDEFAULT)-> m_hWnd) 
// 			{ 
// 				GetDlgItem(IDC_SETDEFAULT)-> SendMessage(WM_LBUTTONUP); 
// 				m_bAddItem= FALSE;
// 				g_nDrawStatus = DRAW_NORMAL;
// 				return   TRUE; 
// 			}
// 			if(pMsg-> hwnd   ==   GetDlgItem(IDC_LIST_RESOURCE)-> m_hWnd) 
// 			{ 
// 				GetDlgItem(IDC_LIST_RESOURCE)-> SendMessage(WM_LBUTTONUP); 
// 				m_bAddItem= FALSE;
// 				g_nDrawStatus = DRAW_NORMAL;
// 				return   TRUE; 
// 			}
// 			if(pMsg-> hwnd   ==   GetDlgItem(IDC_RESOURCETAB)-> m_hWnd) 
// 			{ 
// 				GetDlgItem(IDC_RESOURCETAB)-> SendMessage(WM_LBUTTONUP); 
// 				m_bAddItem= FALSE;
// 				g_nDrawStatus = DRAW_NORMAL;
// 				return   TRUE; 
// 			}
// 			if(pMsg-> hwnd   ==   GetDlgItem(IDC_HIDERESOURCE)-> m_hWnd) 
// 			{ 
// 				GetDlgItem(IDC_HIDERESOURCE)-> SendMessage(WM_LBUTTONUP); 
// 				m_bAddItem= FALSE;
// 				g_nDrawStatus = DRAW_NORMAL;
// 				return   TRUE; 
// 			}
		}

	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CWBResourceDlg::OnBnClickedHideresource()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//���ش���
	ShowWindow(SW_HIDE);
}

//ȡ���װ�Ի����ENTER��ESC����Ӧ 12-25���jin
void CWBResourceDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialogEx::OnOK();
}

void CWBResourceDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialogEx::OnCancel();
}


BOOL CWBResourceDlg::OnEraseBkgnd(CDC* pDC)
{
	//�Ի��򱳾�	


	CString CurPicPath = GetCurrentPath();	//��ȡ��ǰ����ִ�е�Ŀ¼
	CImage img;
	CRect clicentRc;
	GetClientRect(&clicentRc);
	CurPicPath += "\\image\\ResourceBackground.png";
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,clicentRc);
	}	

	return TRUE;

}

void CWBResourceDlg::CreateButton()
{  
	CRect rect,rectButton;
	GetClientRect(&rect);	

	//���ð�ťλ��ʱ��Ҫ�����Ͻǵ�Xλ��Ϊ׼
	CBitmapButton *pBtnClose = new CBitmapButton;	
	rectButton.SetRect(rect.BottomRight().x - 50,rect.TopLeft().y+5,rect.BottomRight().x-3,rect.TopLeft().y + 30);

	pBtnClose->Create(TEXT("�ر�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW, rectButton, this, IDC_HIDERESOURCE);
	pBtnClose->LoadBitmaps(IDB_BITMAP5);
	pBtnClose->Invalidate(TRUE);
	pBtnClose->SizeToContent();
	m_vecButtons.push_back(pBtnClose);

	CBitmapButton *pBtnDel = new CBitmapButton;	
	rectButton.SetRect(rect.BottomRight().x - 109,rect.TopLeft().y+5,rect.BottomRight().x-56,rect.TopLeft().y + 30);

	pBtnDel->Create(TEXT("ɾ����Դ"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW, rectButton, this, IDC_DELRESOURCE);
	pBtnDel->LoadBitmaps(IDB_BITMAP6);
	pBtnDel->Invalidate(TRUE);
	pBtnDel->SizeToContent();
	m_vecButtons.push_back(pBtnDel);

	CBitmapButton *pBtnAdd = new CBitmapButton;	
	rectButton.SetRect(rect.BottomRight().x - 168,rect.TopLeft().y+5,rect.BottomRight().x-115,rect.TopLeft().y + 30);

	pBtnAdd->Create(TEXT("�����Դ"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW, rectButton, this, IDC_ADDRESOURCE);
	pBtnAdd->LoadBitmaps(IDB_BITMAP7);
	pBtnAdd->Invalidate(TRUE);
	pBtnAdd->SizeToContent();
	m_vecButtons.push_back(pBtnAdd);

	CBitmapButton *pBtnDef = new CBitmapButton;	
	rectButton.SetRect(rect.BottomRight().x - 287,rect.TopLeft().y+5,rect.BottomRight().x-3,rect.TopLeft().y + 30);

	pBtnDef->Create(TEXT("����Ĭ����Դ�ļ���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW, rectButton, this, IDC_SETDEFAULT);
	pBtnDef->LoadBitmaps(IDB_BITMAP8);
	pBtnDef->Invalidate(TRUE);
	pBtnDef->SizeToContent();
	m_vecButtons.push_back(pBtnDef);
}


void CWBResourceDlg::DestroyButtons()
{
	int nButtonCount = m_vecButtons.size();
	if (nButtonCount <= 0)
	{
		return;
	}

	CBitmapButton *pButton;
	for (int i = 0; i < nButtonCount; i ++)
	{
		pButton = m_vecButtons[i];

		if (pButton != NULL)
		{
			if (::IsWindow(pButton->m_hWnd))
			{
				pButton->DestroyWindow();
			}
		}

		delete pButton;
	}
	m_vecButtons.clear();

	return;
}
