// CWBResourceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBResourceDlg.h"
#include "afxdialogex.h"
#include <gdiplus.h>
#include "common/XYIniReader.h"
#include "shlwapi.h" 
using namespace Gdiplus;

//定义支持各种类型的文件的后缀名，用分号隔开，这里定义的类型将在资源库中显示
//如要屏蔽某一类型，留空即可
#define  PICTURETYPE "bmp;jpg;gif;tif;png;dib;tiff;jfif;ico;jpeg"
#define  DOCTYPE "doc;ppt;xls;pdf"
#define  MOVIETYPE "avi;wmv;mpeg;mpg"
#define  MUISCTYPE ""
#define  FLASHTYPE "swf;"

// CWBResourceDlg 对话框

IMPLEMENT_DYNAMIC(CWBResourceDlg, CDialogEx)

//定义缩略图大小
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


// CWBResourceDlg 消息处理程序


void CWBResourceDlg::OnTcnSelchangeResourcetab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	//得到当前选中标签的索引，并重画列表框
	int nItem = m_TabDir.GetCurSel();
	m_strImageDir = m_VectorDirPath[nItem];
	LoadThumb();
	*pResult = 0;
}


void CWBResourceDlg::OnLvnItemchangedListResource(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	*pResult = 0;
}

BOOL CWBResourceDlg::OnInitDialog()
{	
	//CreateButton();
	//创建图片列表
	m_ImageListThumb.Create(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1);
	CRect rect;
	GetClientRect(&rect);
	rect.top += 33;	
	//创建列表框，并关联图片列表
	m_ListThumbnail.Create(LVS_SINGLESEL|LVS_ALIGNLEFT|WS_VISIBLE|LVS_ICON, rect, this, IDC_LIST_RESOURCE);
	m_ListThumbnail.SetImageList(&m_ImageListThumb, LVSIL_NORMAL);


	//设置列表视图控件背景
	CString CurPicPath = GetCurrentPath();	//获取当前程序执行的目录	
	CString SetDefaultPath = CurPicPath;
	CurPicPath += "\\image\\ListCtrlBG.png";        // 设置LISTCTRL的背景图路径
	m_ListThumbnail.SetBkImage( CurPicPath.GetBuffer(CurPicPath.GetLength()) );
	m_ListThumbnail.SetTextBkColor(RGB(187, 217, 244));

	//设置按钮
	CreateButton();
	rect.top -= 31;
	rect.left += 10;
	int temp =rect.bottom - 35;
	rect.bottom =rect.bottom - temp;
	rect.right -= 300;
	//创建标签控件
	m_TabDir.Create(WS_VISIBLE | TCS_OWNERDRAWFIXED , rect, this, IDC_RESOURCETAB);

	//根据资源库配置文件生成标签
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
				//将所有类型后缀名都放入一个字符串
				CString strFileType;
				strFileType = PICTURETYPE;
				strFileType += MOVIETYPE;
				strFileType += DOCTYPE;
				strFileType += MUISCTYPE;
				strFileType += FLASHTYPE;
				strExt.MakeLower();			
				//判断类型是否是支持的类型
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
	// 临时缓冲区
	wchar_t wsBuffer[MAX_PATH];

	// 初始化Com库
	hr = CoInitialize( NULL );
	if ( FAILED( hr ) )
	{
		AfxMessageBox( _T( "初始化Com库失败!" ) );
		goto OnExit;
	}

	// 获得IMalloc接口
	hr = SHGetMalloc( &pMalloc );
	if ( FAILED( hr ) )
	{
		goto OnExit;
	}

	// 获得桌面文件夹
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
		else		//因目前尚不能生成其他文件类型缩略图，故读取它们的图标
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
//动态生成标签
BOOL CWBResourceDlg::SetTab()
{
	//char *pValue = new char[255];
	char ***buf = new char**; 
	CString strTabName;
	DWORD TabNum = 0;
	DWORD pBuffsize = 256;
	XYIniReader configIni(".\\WBResConfig.ini");	
	configIni.GetSectionData("资源库", buf, &TabNum, &pBuffsize);
	*buf = new char *[TabNum];
	for (int i = 0; i < TabNum; i++)
	{
		buf[0][i] = new char [256];
	}
	configIni.GetSectionData("资源库", buf, &TabNum, &pBuffsize);

	//如果没有任何键值则返回
	if (TabNum == 0)
	{
		return FALSE;
	}
	
	//创建图像列表
	if (NULL != m_ImageListTab)
	{  
		delete m_ImageListTab;
	}
	m_ImageListTab = new CImageList;
	if (!(m_ImageListTab->Create(30,30,ILC_COLOR24|ILC_MASK,1,1)))
	{  
		MessageBox(_T("创建ImageListChild出错")); 
		return FALSE;
	} 

	m_TabDir.SetImageList(m_ImageListTab);
	//设置图像大小
	m_TabDir.SetItemSize(CSize(50,30)); 
	for (int i = 0; i < TabNum; i++)
	{
	/*	CString str;
		str = buf[0][i];
		int nNameLength, nStrLength;			
		nNameLength = str.Find("*");	//判断文件夹路径标识符的位置,即"*"的位置；
		strTabName = str.Left(nNameLength - 1);	//提取文件夹名
		nStrLength = str.GetLength();		
		m_VectorDirPath.push_back(str.Right(nStrLength - nNameLength - 1));		//提取文件夹路径
		m_TabDir.InsertItem(i, strTabName); 	*/
		char *strPath = new char[MAX_PATH];
		char *strName = new char[MAX_PATH];
		configIni.GetKeyNameValue(strName, strPath, "*", buf[0][i]);
		strTabName = strName;		
		m_VectorDirPath.push_back(strPath);		//提取文件夹路径
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
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO   bBinfo; 	
	char *pDirName = new char[MAX_PATH];	
	memset(&bBinfo,   0,   sizeof(BROWSEINFO));     //定义结构并初始化 
	bBinfo.hwndOwner=this->m_hWnd;//设置对话框所有者句柄 
	char   strTmp[255]; 
	bBinfo.lpszTitle= "请选择您要添加的资源文件夹： "; 	
	bBinfo.ulFlags   =  BIF_STATUSTEXT | BIF_USENEWUI|BIF_RETURNONLYFSDIRS;	 //设置标志只允许选择目录    
	bBinfo.pszDisplayName = pDirName;	//保存文件夹名
	LPITEMIDLIST   lpDlist; 
	//用来保存返回信息的IDList，使用SHGetPathFromIDList函数转换为字符串 
	lpDlist=SHBrowseForFolder(&bBinfo);   //显示选择对话框 
	if(lpDlist!=NULL) 
	{ 
		SHGetPathFromIDList(lpDlist,strTmp);//把项目标识列表转化成目录 		 
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
					pItem.mask = TCIF_TEXT;		//设置读取标签名

					//得到当前标签名
					m_TabDir.GetItem(i, &pItem);					
					str.Format("您已经将这个文件夹添加在 %s 标签中了！", (CString)TabName);
					AfxMessageBox(str);
					delete pDirName;
					return;
				}
			}
			//将*存放在文件夹路径之前以便用来标识文件夹路径
			WritePrivateProfileString("资源库", pDirName, "*" + m_strImageDir, ".\\WBResConfig.ini");
			int n = m_TabDir.GetItemCount();
			m_VectorDirPath.push_back(m_strImageDir);	
			m_TabDir.InsertItem(n, pDirName);		
			m_TabDir.SetCurSel(n);
			m_TabDir.Invalidate();
			LoadThumb();
		}
		else
		{
			AfxMessageBox("您选择的不是有效的文件夹！");
		}
	} 
	delete pDirName;
}


void CWBResourceDlg::OnBnClickedDelresource()
{
	// TODO: 在此添加控件通知处理程序代码
	int nItem = m_TabDir.GetCurSel();
	TCITEM pItem;
	TCHAR TabName[256] = {0};

	pItem.pszText = TabName;
	pItem.cchTextMax = 256;
	pItem.mask = TCIF_TEXT;		//设置读取标签名

	//得到当前标签名
	m_TabDir.GetItem(nItem, &pItem);

	//判断是否为第一个标签，即默认资源库，如果是，则不删除
	if (nItem == 0)
	{
		return;
	}

	//删除在配置文件中的键值
	WritePrivateProfileString("资源库", TabName, NULL, ".\\WBResConfig.ini");

	//删除文件夹路径
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

	//重新设置标签焦点，判断是否删除第一个焦点或最后一个焦点
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
//		m_TabDir.DestroyWindow();	//销毁标签
		m_ListThumbnail.DeleteAllItems();
		m_ListThumbnail.SetRedraw();
	}
	m_TabDir.Invalidate();
}


void CWBResourceDlg::OnBnClickedSetdefault()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO   bBinfo; 	
	char *pDirName = new char[MAX_PATH];	
	memset(&bBinfo,   0,   sizeof(BROWSEINFO));     //定义结构并初始化 
	bBinfo.hwndOwner=this->m_hWnd;//设置对话框所有者句柄 
	char   strTmp[255]; 
	bBinfo.lpszTitle= "请选择您要设置为默认资源库的文件夹： "; 	
	bBinfo.ulFlags   =  BIF_STATUSTEXT | BIF_USENEWUI|BIF_RETURNONLYFSDIRS;	 //设置标志只允许选择目录    
	bBinfo.pszDisplayName = pDirName;	//保存文件夹名
	LPITEMIDLIST   lpDlist; 
	//用来保存返回信息的IDList，使用SHGetPathFromIDList函数转换为字符串 
	lpDlist=SHBrowseForFolder(&bBinfo);   //显示选择对话框 
	if(lpDlist!=NULL) 
	{ 
		SHGetPathFromIDList(lpDlist,strTmp);//把项目标识列表转化成目录 		 
		m_strImageDir = strTmp;
		if (m_strImageDir != "\0")
		{
			//将*存放在文件夹路径之前以便用来标识文件夹路径
			WritePrivateProfileString("资源库", "资源库", "*" + m_strImageDir, ".\\WBResConfig.ini");
			int n = m_TabDir.GetItemCount();
			m_VectorDirPath[0] = m_strImageDir;			
			m_TabDir.SetCurSel(0);
			LoadThumb();
		}
		else
		{
			AfxMessageBox("您选择的不是有效的文件夹！");
		}
	} 
	delete pDirName;
}


void CWBResourceDlg::OnLvnBegindragListResource(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (g_bIsDrawScreen) //如果屏幕绘图打开就屏蔽
	{
		return;
	}

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	POSITION   pos=m_ListThumbnail.GetFirstSelectedItemPosition(); 
	while(pos) //判断拖拽是否选中文件
	{ 
		m_bAddItem = TRUE;
		int   nItem = m_ListThumbnail.GetNextSelectedItem(pos); 
		m_ListThumbnail.SetFocus();
		//使白板视图处理从触摸屏分发下的鼠标消息
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
	if (g_bIsDrawScreen) //如果屏幕绘图打开就屏蔽
	{
		return;
	}

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	POSITION   pos=m_ListThumbnail.GetFirstSelectedItemPosition(); 
	while(pos) //判断双击位置是否选中文件
	{ 
		int   nItem = m_ListThumbnail.GetNextSelectedItem(pos);  
		CheckType(nItem);

		// 根据判断出的类型添加相应的对象
		switch(m_nFileType)
		{
		case PICTURE:
			{
				CWBPicture *pWBPicture = new CWBPicture(m_strFileName);
				//控制图片的最大不能超出view的大小4-21金鹏
				CRect viewRect;
				g_pWhiteBoardView->GetClientRect(&viewRect);
				pWBPicture->SetPictureBigerSzie(viewRect);

				g_pWBPage->AddWBObject(pWBPicture);
				g_pWhiteBoardView->Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pWBPicture->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);

				//添加操作记录
				g_pWBFile->AddOperation(_T("插入图片"));
			}
			break;
		case MOVIE:
			{
				CWBMovie *pWBMovie = new CWBMovie(m_strFileName);
		
				g_pWBPage->AddWBObject(pWBMovie);
				pWBMovie->Start();
				g_pWhiteBoardView->Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pWBMovie->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);

				//添加操作记录
				g_pWBFile->AddOperation(_T("插入视频"));
			}
			break;
		case DOC:
			{
				CWBDocument *pWBDocument = new CWBDocument(m_strFileName);
				g_pWBPage->AddWBObject(pWBDocument);
				g_pWhiteBoardView->Invalidate();

				//发送同步消息
				XMemIni xmlObj;
				pWBDocument->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);

				//添加操作记录
				g_pWBFile->AddOperation(_T("插入文档"));
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

				//发送同步消息
				XMemIni xmlObj;
				pWBFlash->SetXmlValue(xmlObj);
				g_pWhiteBoardView->AddObj(xmlObj);

				//添加操作记录
				g_pWBFile->AddOperation(_T("插入FLASH"));
			}
			break;
		default:
			break;
		}
				
	}	
	*pResult = 0;
}

//判断文件类型
void CWBResourceDlg::CheckType(int nItem)
{
	CString strExt;
	m_strFileName = m_ListThumbnail.GetItemText(nItem, 0);
	strExt = PathFindExtension(m_strFileName);
	strExt = strExt.Right(strExt.GetLength() - 1);
	m_strFileName = m_strImageDir + "\\" + m_strFileName;	//获取文件名	

	CString strFileType;
	strFileType = PICTURETYPE;	//所有图片类型后缀名组成的字符串
	strExt.MakeLower();
	if (strFileType.Find(strExt) != -1)
	{
		m_nFileType = PICTURE;
		return;
	}

	strFileType = MOVIETYPE;	//所有视频类型后缀名组成的字符串
	if (strFileType.Find(strExt) != -1)
	{
		m_nFileType = MOVIE;
		return;
	}

	strFileType = DOCTYPE;		//所有文档类型后缀名组成的字符串
	if (strFileType.Find(strExt) != -1)
	{
		m_nFileType = DOC;
		return;
	}

	strFileType = MUISCTYPE;	//所有音频类型后缀名组成的字符串
	if (strFileType.Find(strExt) != -1)
	{
		m_nFileType = MUISC;
		return;
	}

	strFileType = FLASHTYPE;	//所有FLASH类型后缀名组成的字符串
	if (strFileType.Find(strExt) != -1)
	{
		m_nFileType = FLASH;		
		return;
	}
}

LRESULT CWBResourceDlg::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值

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
	// TODO: 在此添加专用代码和/或调用基类

	//拦截左键弹起消息，判断是否在拖拽状态，如在拖拽状态则不转到添加对象状态	
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
	// TODO: 在此添加控件通知处理程序代码

	//隐藏窗口
	ShowWindow(SW_HIDE);
}

//取消白板对话框对ENTER和ESC的响应 12-25添加jin
void CWBResourceDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}

void CWBResourceDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnCancel();
}


BOOL CWBResourceDlg::OnEraseBkgnd(CDC* pDC)
{
	//对话框背景	


	CString CurPicPath = GetCurrentPath();	//获取当前程序执行的目录
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

	//设置按钮位置时主要以左上角的X位置为准
	CBitmapButton *pBtnClose = new CBitmapButton;	
	rectButton.SetRect(rect.BottomRight().x - 50,rect.TopLeft().y+5,rect.BottomRight().x-3,rect.TopLeft().y + 30);

	pBtnClose->Create(TEXT("关闭"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW, rectButton, this, IDC_HIDERESOURCE);
	pBtnClose->LoadBitmaps(IDB_BITMAP5);
	pBtnClose->Invalidate(TRUE);
	pBtnClose->SizeToContent();
	m_vecButtons.push_back(pBtnClose);

	CBitmapButton *pBtnDel = new CBitmapButton;	
	rectButton.SetRect(rect.BottomRight().x - 109,rect.TopLeft().y+5,rect.BottomRight().x-56,rect.TopLeft().y + 30);

	pBtnDel->Create(TEXT("删除资源"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW, rectButton, this, IDC_DELRESOURCE);
	pBtnDel->LoadBitmaps(IDB_BITMAP6);
	pBtnDel->Invalidate(TRUE);
	pBtnDel->SizeToContent();
	m_vecButtons.push_back(pBtnDel);

	CBitmapButton *pBtnAdd = new CBitmapButton;	
	rectButton.SetRect(rect.BottomRight().x - 168,rect.TopLeft().y+5,rect.BottomRight().x-115,rect.TopLeft().y + 30);

	pBtnAdd->Create(TEXT("添加资源"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW, rectButton, this, IDC_ADDRESOURCE);
	pBtnAdd->LoadBitmaps(IDB_BITMAP7);
	pBtnAdd->Invalidate(TRUE);
	pBtnAdd->SizeToContent();
	m_vecButtons.push_back(pBtnAdd);

	CBitmapButton *pBtnDef = new CBitmapButton;	
	rectButton.SetRect(rect.BottomRight().x - 287,rect.TopLeft().y+5,rect.BottomRight().x-3,rect.TopLeft().y + 30);

	pBtnDef->Create(TEXT("设置默认资源文件夹"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW, rectButton, this, IDC_SETDEFAULT);
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
