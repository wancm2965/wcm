// WBResDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBFunctionResDlg.h"
#include "afxdialogex.h"
//#include "ResManage.h"
#include "WBResFunc.h"
#include <gdiplus.h>
#include "common/XYIniReader.h"
#include "shlwapi.h" 
#include "WBFunctionDlg.h"
using namespace Gdiplus;

#define	THUMBNAIL_WIDTH		40 //缩略图宽度
#define	THUMBNAIL_HEIGHT	28 //缩略图宽度
#define MYCOMPUTER "My Computer"

// CWBFunctionResDlg 对话框

IMPLEMENT_DYNAMIC(CWBFunctionResDlg, CDialogEx)

	CWBFunctionResDlg::CWBFunctionResDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBFunctionResDlg::IDD, pParent),m_pResThread(NULL),m_bRenderLocked(FALSE),m_bIsSpread(FALSE)
{
	m_pResoureTree = NULL;
	m_pResoureListCtl = NULL;	
	m_strImageDir = _T("");	
	//m_pResMap = CResManage::GetInstance()->GetResMap();
	m_SpreadStr = _T("");
}

CWBFunctionResDlg::~CWBFunctionResDlg()
{
	delete m_pResoureTree;
	m_pResoureTree = NULL;
	delete m_pResoureListCtl;
	m_pResoureListCtl = NULL;
}

void CWBFunctionResDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_WB_RESOURETREE, *m_pResoureTree);
}


BEGIN_MESSAGE_MAP(CWBFunctionResDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_WB_RESOURETREE, &CWBFunctionResDlg::OnTvnSelchangedWbResouretree)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_WB_RESOURETREE, &CWBFunctionResDlg::OnTvnItemexpandedWbResouretree)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_WB_RESOURETREE, &CWBFunctionResDlg::OnTvnBegindragWbResouretree)
	ON_NOTIFY(NM_DBLCLK, IDC_WB_RESOURETREE, &CWBFunctionResDlg::OnNMDblclkWbResouretree)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_WB_RESDLG_LISTCTL, &CWBFunctionResDlg::OnLvnBegindragWbResdlgListctl)
	ON_NOTIFY(NM_DBLCLK, IDC_WB_RESDLG_LISTCTL, &CWBFunctionResDlg::OnNMDblclkWbResdlgListctl)
END_MESSAGE_MAP()


// CWBFunctionResDlg 消息处理程序

BOOL CWBFunctionResDlg::OnInitDialog()
{	
	CRect nParentRc;//控制面板显示区域
	((CWBFunctionDlg *)GetParent())->GetClientRect(&nParentRc);
	MoveWindow(nParentRc.left + 10,nParentRc.top + 30 ,WB_FUNCTION_DLG_WIDTH - 15,nParentRc.bottom - nParentRc.top - 65);
// 	GetClientRect(&m_rect);
// 	m_rect.bottom = m_rect.bottom / 2;
// 	m_pResoureTree = new CTreeCtrl;
// 	m_pResoureTree->Create(TVS_HASBUTTONS|TVS_LINESATROOT|TVS_HASLINES,m_rect,this,IDC_WB_RESOURETREE);
	//m_pResoureTree->MoveWindow(m_rect);
// 	m_pResoureTree->ShowWindow(SW_SHOW);	
// 	m_hRoot = m_pResoureTree->InsertItem(MYCOMPUTER);//设置根节点
// 	InitTreeView(m_hRoot);
// 	m_pResoureTree->Expand(m_hRoot, TVE_EXPAND); 

// 	GetClientRect(&m_rect);
// 	m_rect.top = m_rect.bottom / 2 + 6;
// 	m_pResoureListCtl = new CListCtrl;
// 	m_pResoureListCtl->Create(LVS_ICON|LVS_AUTOARRANGE|LVS_ALIGNTOP|LVS_ALIGNLEFT,m_rect,this,IDC_WB_RESDLG_LISTCTL);
// 	m_pResoureListCtl->ShowWindow(SW_SHOW);	
// 
// 	m_imageList.Create(16, 16, ILC_COLOR8, 30, 30);
	//HICON *hIconForTreeLst;
	//hIconForTreeLst = AfxGetApp()->LoadIcon(IDI_FOLDERCLOSED);
// 	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_FOLDERCLOSED));
// 	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_FOLDEROPENED));
// 	m_pResoureTree->SetImageList(&m_imageList,TVSIL_NORMAL);


	//m_ImageListThumb.Create(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1);
	//列表框关联图片列表
	//m_pResoureListCtl->SetImageList(&m_ImageListThumb, LVSIL_NORMAL);
	return TRUE;
}

CTreeCtrl *CWBFunctionResDlg::GetResTree()
{
	return m_pResoureTree;
}
 
void CWBFunctionResDlg::CreateResTree()//创建树形控件
{
	GetClientRect(&m_rect);
	m_rect.bottom = m_rect.bottom / 3;
	m_pResoureTree = new CTreeCtrl;
	m_pResoureTree->Create(TVS_HASBUTTONS|TVS_LINESATROOT|TVS_HASLINES,m_rect,this,IDC_WB_RESOURETREE);
	//m_pResoureTree->MoveWindow(m_rect);
	m_pResoureTree->ShowWindow(SW_SHOW);	
	m_hRoot = m_pResoureTree->InsertItem(_T(MYCOMPUTER));//设置根节点
	InitTreeView(m_hRoot);
	m_pResoureTree->Expand(m_hRoot, TVE_EXPAND); 

	m_imageList.Create(16, 16, ILC_COLOR32, 30, 30);
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_FOLDERCLOSED));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_FOLDEROPENED));
	m_pResoureTree->SetImageList(&m_imageList,TVSIL_NORMAL);
}

void CWBFunctionResDlg::CreateResListCtl()//创建列表控件
{
	CRect rc;
	m_pResoureTree->GetClientRect(&rc);
	GetClientRect(&m_rect);
	rc.top = rc.bottom + 6;
	rc.bottom = m_rect.bottom;
	m_pResoureListCtl = new CListCtrl;
	m_pResoureListCtl->Create(LVS_ICON|LVS_AUTOARRANGE|LVS_SINGLESEL|LVS_ALIGNTOP,rc,this,IDC_WB_RESDLG_LISTCTL);
	m_pResoureListCtl->ShowWindow(SW_SHOW);	

	m_ImageListThumb.Create(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, ILC_COLOR32, 0, 1);
	//列表框关联图片列表
	m_pResoureListCtl->SetImageList(&m_ImageListThumb, LVSIL_NORMAL);
}

void CWBFunctionResDlg::InitTreeView(HTREEITEM hParent)//列出各分区
{

	int nPos = 0;
	UINT nCount = 0;
	CString strDrive = L"?:\\";

	DWORD dwDriveList = ::GetLogicalDrives();

	CString cTmp;

	while (dwDriveList)//从dwDriveList的值中解析出各个逻辑盘符
	{
		if (dwDriveList & 1) 
		{
			cTmp = strDrive;
			strDrive.SetAt (0, 0x41 + nPos);
			if (AddDrives(strDrive , hParent))
				nCount++;
		}
		dwDriveList >>= 1;
		nPos++;
	}
	return;
}

BOOL CWBFunctionResDlg::AddDrives(CString strDrive, HTREEITEM hParent)
{

	HTREEITEM hItem;
	UINT nType = ::GetDriveType ((LPCTSTR) strDrive);
	UINT nDrive = (UINT) strDrive[0] - 0x41;

	switch (nType) {

	case DRIVE_REMOVABLE:
		hItem = m_pResoureTree->InsertItem(strDrive, /*ILI_FLOPPYDRV, ILI_FLOPPYDRV, */hParent);
		AddDummyNode(hItem);
		break;

	case DRIVE_FIXED:
		hItem = m_pResoureTree->InsertItem(strDrive,/*  ILI_DRIVE, ILI_DRIVE, */hParent);
		AddDummyNode(hItem);
		break;

	case DRIVE_REMOTE:
		hItem = m_pResoureTree->InsertItem(strDrive,/* ILI_DRIVE, ILI_DRIVE,*/ hParent);
		AddDummyNode(hItem);
		break;

	case DRIVE_CDROM:
		hItem = m_pResoureTree->InsertItem(strDrive,/* ILI_CDDRV, ILI_CDDRV, */hParent);
		AddDummyNode(hItem);
		break;

	case DRIVE_RAMDISK:
		hItem = m_pResoureTree->InsertItem(strDrive,/* ILI_CDDRV, ILI_CDDRV, */hParent);
		AddDummyNode(hItem);
		break;

	default:
		return FALSE;
	}

	return true;

}

void CWBFunctionResDlg::AddDummyNode(HTREEITEM hItem)
{
	m_pResoureTree->InsertItem (L"", 0, 0, hItem);
}

CString CWBFunctionResDlg::GetPathFromItem(HTREEITEM hItem)
{
	CString strPathName;
	while (hItem != NULL) 
	{
		CString string = m_pResoureTree->GetItemText (hItem);
		if ((string.Right (1) != "\\") && !strPathName.IsEmpty ())
			string += "\\";
		strPathName = string + strPathName;
		hItem = m_pResoureTree->GetParentItem (hItem);
	}

	if(strPathName.Left(11) == MYCOMPUTER && strPathName.GetLength() > 11)
		strPathName = strPathName.Mid(12);

	return strPathName;

}


BOOL CWBFunctionResDlg::IsPathValid(CString &strPathName)
{

	if (strPathName.GetLength() == 3)
		return TRUE;

	HANDLE hFind;
	WIN32_FIND_DATA fd;
	BOOL bResult = FALSE;

	if ((hFind = ::FindFirstFile ((LPCTSTR) strPathName, &fd)) != INVALID_HANDLE_VALUE) 
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			bResult = TRUE;

		::FindClose(hFind);
	}
	return bResult;
}

BOOL CWBFunctionResDlg::IsMediaValid(CString &strPathName)
{

	// Return TRUE if the drive doesn't support removable media.
	UINT nDriveType = GetDriveType ((LPCTSTR) strPathName);
	if ((nDriveType != DRIVE_REMOVABLE) && (nDriveType != DRIVE_CDROM))
		return TRUE;

	// Return FALSE if the drive is empty (::GetVolumeInformation fails).
	//    DWORD dwSerialNumber;
	CString strDrive = strPathName.Left (3);
	UINT nDrive = (UINT) strDrive[0] - 0x41;

	//    if (!::GetVolumeInformation ((LPCTSTR) strDrive, NULL, 0,
	//        &dwSerialNumber, NULL, NULL, NULL, 0)) {
	//        m_dwMediaID[nDrive] = 0xFFFFFFFF;
	//        return FALSE;
	//    }

	// Also return FALSE if the disk's serial number has changed.
	//    if ((m_dwMediaID[nDrive] != dwSerialNumber) &&
	//        (m_dwMediaID[nDrive] != 0xFFFFFFFF)) {
	//        m_dwMediaID[nDrive] = dwSerialNumber;
	//        return FALSE;
	//    }

	// Update our record of the serial number and return TRUE.
	//    m_dwMediaID[nDrive] = dwSerialNumber;
	return TRUE;
}

HTREEITEM CWBFunctionResDlg::GetDriveNode(HTREEITEM hItem)
{

	HTREEITEM hParent;

	do 
	{
		hParent = m_pResoureTree->GetParentItem (hItem);
		if (hParent != NULL)
			hItem = hParent;
	} while (hParent != NULL);
	return hItem;

}

UINT CWBFunctionResDlg::DeleteChildren(HTREEITEM hItem)
{

	UINT nCount = 0;
	HTREEITEM hChild = m_pResoureTree->GetChildItem (hItem);

	while (hChild != NULL) 
	{
		HTREEITEM hNextItem = m_pResoureTree->GetNextSiblingItem(hChild);
		m_pResoureTree->DeleteItem (hChild);
		hChild = hNextItem;
		nCount++;

		//		dwTreeItemCount--;

	}
	return nCount;

}

UINT CWBFunctionResDlg::AddDirectoryNodes(HTREEITEM hItem, CString &strPathName) //caoyuqin2011-12-6
{

	HANDLE hFind;
	WIN32_FIND_DATA fd;
	UINT nCount = 0;

	CString strFileSpec = strPathName;
		if (strFileSpec.Right (1) != "\\")
			strFileSpec += "\\";
		strFileSpec += "*.*";

	if ((hFind = ::FindFirstFile ((LPCTSTR) strFileSpec, &fd)) ==	INVALID_HANDLE_VALUE) 
	{
			if (IsDriveNode (hItem))
				AddDummyNode (hItem);
			return 0;
	}

	CWaitCursor wait;
	//	m_LocalFiles.DeleteAllItems();

	do {
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //添加文件夹进树形列表
		{
			CString strFileName = (LPCTSTR) &fd.cFileName;			
			if ((strFileName != ".") && (strFileName != "..") && (fd.dwFileAttributes != 22))
			{
				HTREEITEM hChild =	m_pResoureTree->InsertItem ((LPCTSTR) &fd.cFileName,
					0,1,hItem , TVI_SORT);

				CString strNewPathName = strPathName;
				if (strNewPathName.Right (1) != "\\")
					strNewPathName += "\\";

				strNewPathName += (LPCTSTR) &fd.cFileName;
				SetButtonState (hChild, strNewPathName);
				nCount++;
			}
		}
//  		else//添加文件进树形列表
//  		{
//  			CString strFileName = (LPCTSTR) &fd.cFileName;
//  			CString sExt = PathFindExtension(strFileName);
//  			sExt.MakeLower();
//  			//过滤掉白板不支持的文件格式
//  			if (((sExt == ".bmp") || (sExt == ".jpg") || (sExt == ".gif") || (sExt == ".png") || (sExt == ".dib") 
//  				|| (sExt == ".tiff") || (sExt == ".jfif") || (sExt == ".ico") || (sExt == ".jpeg") 
//  				||(sExt == ".doc") || (sExt == ".ppt") || (sExt == ".xls") || (sExt == ".pdf")
//  				||(sExt == ".avi") || (sExt == ".wmv") || (sExt == ".mpeg") || (sExt == ".mpg")
//  				||(sExt == ".swf")))
//  			{
//  				HTREEITEM hChild = m_pResoureTree->InsertItem ((LPCTSTR) &fd.cFileName,
// 					/*ILI_TEXTFILE , ILI_TEXTFILE , */hItem , TVI_SORT);
// 			}
// 		}	

	} while (::FindNextFile (hFind, &fd));

	::FindClose (hFind);

	return nCount;

}

void CWBFunctionResDlg::SetButtonState(HTREEITEM hItem, CString &strPathName)
{
	if (HasSubdirectory (strPathName))
		AddDummyNode (hItem);
}

BOOL CWBFunctionResDlg::HasSubdirectory(CString &strPathName)
{

	HANDLE hFind;
	WIN32_FIND_DATA fd;
	BOOL bResult = FALSE;

	CString strFileSpec = strPathName;
	if (strFileSpec.Right (1) != "\\")
		strFileSpec += "\\";
	strFileSpec += "*.*";

	if ((hFind = ::FindFirstFile ((LPCTSTR) strFileSpec, &fd)) !=
		INVALID_HANDLE_VALUE) {
			do {
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					CString strFileName = (LPCTSTR) &fd.cFileName;
					if ((strFileName != ".") && (strFileName != ".."))
						bResult = TRUE;
				}
			} while (::FindNextFile (hFind, &fd) && !bResult);
			::FindClose (hFind);
	}

	return bResult;

}

BOOL CWBFunctionResDlg::IsDriveNode(HTREEITEM hItem)
{

	return (m_pResoureTree->GetParentItem (hItem) == NULL) ? TRUE : FALSE;
}

void CWBFunctionResDlg::OnOK()//屏蔽回车键
{
	return;
}

BOOL CWBFunctionResDlg::OnEraseBkgnd(CDC* pDC)
{
	//对话框背景	
	//CString CurPicPath;	//获取当前程序执行的目录
	CImage img;
	CRect clicentRc;
	GetClientRect(&clicentRc);
	CString CurPicPath = GetCurrentPath() + "\\SKIN\\WhiteBoard\\image\\functionDlg\\DlgBG.png";
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,clicentRc);
	}	

	return TRUE;
}

void CWBFunctionResDlg::OnSize(UINT nType, int cx, int cy)
{
	// TODO: 在此处添加消息处理程序代码
	CDialogEx::OnSize(nType, cx, cy);
	//GetClientRect(&m_rect);//得到树形列表将要放置的区域
}


void CWBFunctionResDlg::OnTvnSelchangedWbResouretree(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (bRenderLocked())
	{
		return;
	}
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_CurrenthItemOfTreeCtl = pNMTreeView->itemNew.hItem;

	CString strPathName = GetPathFromItem (m_CurrenthItemOfTreeCtl);
	*pResult = FALSE;

	if(strPathName == MYCOMPUTER)
		return;
	if (strPathName == m_SpreadStr)
		return;

	if (_tcscmp(strPathName,m_SpreadStr))
		return;

		
	CWaitCursor wait;

	if (!AddDirectoryNodes (m_CurrenthItemOfTreeCtl, strPathName))
		*pResult = TRUE;
	if (strPathName[1] == L':')//如果展开的是目录
	{
		m_LocalPath = strPathName;
		m_strImageDir = strPathName;
		if (m_strImageDir.Right(1) != L"\\")
		{
			m_strImageDir.Append(L"\\");
		}
		if (m_LocalPath.Right(1) != L"\\")
		{
			m_LocalPath.Append(L"\\");
		}
	}
	*pResult = 0;
}


void CWBFunctionResDlg::OnTvnItemexpandedWbResouretree(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (bRenderLocked())
	{
		return;
	}
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_CurrenthItemOfTreeCtl = pNMTreeView->itemNew.hItem;
	CString strPathName = GetPathFromItem (m_CurrenthItemOfTreeCtl);
	m_SpreadStr = strPathName;
	if (!IsMediaValid (strPathName)) 
	{
		HTREEITEM hRoot = GetDriveNode (m_CurrenthItemOfTreeCtl);
		m_pResoureTree->Expand (hRoot, TVE_COLLAPSE);
		DeleteChildren (hRoot);
		AddDummyNode (hRoot);
		*pResult = TRUE;
		return;
	}


	// Delete the item if strPathName no longer specifies a valid path.
	if (!IsPathValid (strPathName)) 
	{
		if(strPathName != MYCOMPUTER && strPathName != "")
		{
			m_pResoureTree->DeleteItem (m_CurrenthItemOfTreeCtl);
			*pResult = TRUE;
			return;
		}
	}

	CWaitCursor wait;
	// If the item is expanding, delete the dummy item attached to it
	// and add folder items. If the item is collapsing instead, delete
	// its folder items and add a dummy item if appropriate.
	if (pNMTreeView->action == TVE_EXPAND) 
	{
		if(strPathName != MYCOMPUTER)
		{
			DeleteChildren (m_CurrenthItemOfTreeCtl);
			if (!AddDirectoryNodes (m_CurrenthItemOfTreeCtl, strPathName))
				*pResult = TRUE;
		}
	}
	else {
		if(strPathName != MYCOMPUTER)
		{
			DeleteChildren (m_CurrenthItemOfTreeCtl);
			if (IsDriveNode (m_CurrenthItemOfTreeCtl))
				AddDummyNode (m_CurrenthItemOfTreeCtl);
			else
				SetButtonState (m_CurrenthItemOfTreeCtl, strPathName);
		}
	}
	if (strPathName[1] == L':')//如果展开的是目录
	{
		m_LocalPath = strPathName;
		m_strImageDir = strPathName;
		if (m_strImageDir.Right(1) != L"\\")
		{
			m_strImageDir.Append(L"\\");
		}
		if (m_LocalPath.Right(1) != L"\\")
		{
			m_LocalPath.Append(L"\\");
		}
		if (pNMTreeView->action == 2)//展开根时
		{
			LoadThumb();//画出m_strImageDir文件夹下白板支持的文件的缩略图
		}
		if (pNMTreeView->action == 1)//收缩树枝时
		{
			if (IsWindow(m_hWnd))
			{
				m_pResoureListCtl->SetRedraw(FALSE);
			}
			// reset our image list
			for (int i = 0; i<m_ImageListThumb.GetImageCount(); i++)
			{
				m_ImageListThumb.Remove(i);	
			}
			// remove all items from list view
			if (m_pResoureListCtl->GetItemCount() != 0)
			{
				m_pResoureListCtl->DeleteAllItems();
			}
			//m_pResoureListCtl->SetRedraw(TRUE); 
			if (IsWindow(m_hWnd))
			{
				m_pResoureListCtl->SetRedraw(TRUE);
			}
		}
	}
	*pResult = 0;
}


void CWBFunctionResDlg::OnTvnBegindragWbResouretree(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (bRenderLocked())
	{
		return;
	}
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_CurrenthItemOfTreeCtl = pNMTreeView->itemNew.hItem;
	g_nMethodOfResToView = DRAG_AND_DROP;
	if (m_pResoureTree->GetChildItem(m_CurrenthItemOfTreeCtl) == NULL)//选中资源时
	{
		CString sExt;//选中资源的后缀名
		g_strSelectedResPath = GetPathFromItem (m_CurrenthItemOfTreeCtl);		
		sExt = PathFindExtension(g_strSelectedResPath);
		sExt.MakeLower();
		if ( (sExt == ".bmp") || (sExt == ".jpg") || (sExt == ".gif") 
			|| (sExt == ".png") || (sExt == ".dib") || (sExt == ".tiff") || (sExt == ".jfif") || (sExt == ".ico") || (sExt == ".jpeg") )//拖拽图片资源时
		{
			g_nDrawStatus = ADDPICTUREFROMRESDLG;

		}
		if ((sExt == ".doc") || (sExt == ".ppt") || (sExt == ".xls") || (sExt == ".pdf"))//拖拽文档资源时
		{
			g_nDrawStatus = ADDDOCFROMRESDLG;
		}
		if ((sExt == ".avi") || (sExt == ".wmv") || (sExt == ".mpeg") || (sExt == ".mpg")
			||(sExt == ".asf") || (sExt == ".mov") || (sExt == ".dat") || (sExt == ".rmvb")
			||(sExt == ".vob") || (sExt == ".mp4") || (sExt == ".f4v") || (sExt == ".mp3")
			||(sExt == ".wm") || (sExt == ".wma") || (sExt == ".rm") || (sExt == ".mts")||(sExt == ".mkv"))//拖拽视频资源时
		{
			g_nDrawStatus = ADDNEWMOVIEFROMRESDLG;
		}
		if ((sExt == ".swf") )//拖拽flash资源时
		{
			g_nDrawStatus = ADDFLASHFROMRESDLG;
		}
	}
	*pResult = 0;
}


void CWBFunctionResDlg::OnNMDblclkWbResouretree(NMHDR *pNMHDR, LRESULT *pResult)//双击资源时白板生成选中的对象
{
	// TODO: 在此添加控件通知处理程序代码
	if (!bRenderLocked())
	{
		return;
	}
	g_nMethodOfResToView = DOUBLE_CLK;
	if (m_pResoureTree->GetChildItem(m_CurrenthItemOfTreeCtl) == NULL)//选中资源时
	{
		CString sExt;//选中资源的后缀名
		//g_strSelectedResPath = GetPathFromItem (m_CurrenthItemOfTreeCtl);
 		m_SpreadStr = GetPathFromItem (m_CurrenthItemOfTreeCtl);
 		g_strSelectedResPath = m_SpreadStr;
		sExt = PathFindExtension(g_strSelectedResPath);
		sExt.MakeLower();
		if ( (sExt == ".bmp") || (sExt == ".jpg") || (sExt == ".gif") 
			|| (sExt == ".png") || (sExt == ".dib") || (sExt == ".tiff") || (sExt == ".jfif") || (sExt == ".ico") || (sExt == ".jpeg") )//双击图片资源时
		{
			g_nDrawStatus = ADDPICTUREFROMRESDLG;
			g_pWhiteBoardView->AddResToWBView(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
		}
		if ((sExt == ".doc") || (sExt == ".ppt") || (sExt == ".xls") || (sExt == ".pdf"))//双击文档资源时
		{
			g_nDrawStatus = ADDDOCFROMRESDLG;
			g_pWhiteBoardView->AddResToWBView(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
		}
		if ((sExt == ".avi") || (sExt == ".wmv") || (sExt == ".mpeg") || (sExt == ".mpg")
			||(sExt == ".asf") || (sExt == ".mov") || (sExt == ".dat") || (sExt == ".rmvb")
			||(sExt == ".vob") || (sExt == ".mp4") || (sExt == ".f4v") || (sExt == ".mp3")
			||(sExt == ".wm") || (sExt == ".wma") || (sExt == ".rm") || (sExt == ".mts")||(sExt == ".mkv"))//双击视频资源时
		{
			g_nDrawStatus = ADDNEWMOVIEFROMRESDLG;
			g_pWhiteBoardView->AddResToWBView(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
		}
		if ((sExt == ".swf") )//双击flash资源时
		{
			g_nDrawStatus = ADDFLASHFROMRESDLG;
			g_pWhiteBoardView->AddResToWBView(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
		}
	}
	*pResult = 0;
}



void CWBFunctionResDlg::InitDatas()
{
	//判断资源链表是否为空，如果为空，返回
	if (m_pResMap->size() <= 0)
	{
		//清空默认的资源目录
		m_strImageDir = _T("");
		OutputDebugString(_T("no resource in the list!"));
		return;
	}
	//不空，将资源链表中的信息显示
	ResMapArrayIter it = m_pResMap->begin();
	for (int nIndex = 0; it != m_pResMap->end(); it++, nIndex++)
	{
		m_strImageDir = (*it)->GetMapPath();      //得到当前路径
	}

	it = m_pResMap->begin();
	m_strImageDir = (*it)->GetMapPath();
	LoadThumb();
}

void CWBFunctionResDlg::LoadThumb()
{
	LockRender(TRUE);
	// show hour glass cursor
	BeginWaitCursor();
		
	// get the names of bitmap files
	if ( !GetImageFileNames() )
	{   
		// hold the window update to avoid flicking
		if (IsWindow(m_hWnd))
		{
			m_pResoureListCtl->SetRedraw(FALSE);
		}
		//m_pResoureListCtl->SetRedraw(FALSE);
		// reset our image list
		for (int i = 0; i<m_ImageListThumb.GetImageCount(); i++)
		{
			m_ImageListThumb.Remove(i);	
		}
		// remove all items from list view
		if (m_pResoureListCtl->GetItemCount() != 0)
		{
			m_pResoureListCtl->DeleteAllItems();
		}
		if (IsWindow(m_hWnd))
		{
			m_pResoureListCtl->SetRedraw(TRUE);
		}
		//m_pResoureListCtl->SetRedraw(TRUE);
		LockRender(FALSE);
		return;
	}
	//creat thread to Draw   bitmap
	SResPadThumbThread *pPar = new SResPadThumbThread;
	pPar->pSinWBResDlg = this;
	SIZE_T size = 10000000;
	if (m_pResThread)
	{
		delete m_pResThread;
		m_pResThread = NULL;
	}
	m_pResThread = pPar;

	m_pResThread->hThread = CreateThread( NULL, size, 
		(LPTHREAD_START_ROUTINE)CWBFunctionResDlg::AfxThreadProc,         //线程启动函数
		(LPVOID)m_pResThread,          //线程启动参数
		0,
		&(m_pResThread->ThreadID));
	m_pResThread = pPar;
	//DrawThumbnails();
	EndWaitCursor();
}

UINT CWBFunctionResDlg::AfxThreadProc( LPVOID pParam )
{
	HRESULT hr = S_OK;
	SResPadThumbThread * pPar = (SResPadThumbThread*)pParam;
	LPCTSTR lpszDirectory = pPar->strDirectory;
	CWBFunctionResDlg *pResMng = pPar->pSinWBResDlg;
	pResMng->DrawThumbnails();
	pResMng->LockRender(FALSE);
	return 0;
}

//采集图片的名字
BOOL  CWBFunctionResDlg::GetImageFileNames()
{	
	if (m_strImageDir.GetLength() < 1)
	{
		return FALSE;
	}

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
		OutputDebugString((LPCTSTR)msg);
		::LocalFree(msg);

		return FALSE;
	}

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
				OutputDebugString((LPCTSTR)msg);
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
		m_vecSupportFileName.clear();
		m_vecSupportFileName = VectorImageNames;
		return TRUE;
	}

	return FALSE;
}

//画缩略图
void  CWBFunctionResDlg::DrawThumbnails()
	// 在列表框中画图
{
	CBitmap*    pImage = NULL;
	HBITMAP		hBmp = NULL;
	//POINT		pt;
	CString		strPath;
	int			nIndex;

	// no images
	if (m_vecSupportFileName.empty())
	{
		return;
	}
	// set the length of the space between thumbnails
	// you can also calculate and set it based on the length of your list control

	// hold the window update to avoid flicking
	if (IsWindow(m_hWnd))
	{
		m_pResoureListCtl->SetRedraw(FALSE);
	}
	//m_pResoureListCtl->SetRedraw(FALSE);

	// reset our image list
	for (nIndex = 0; nIndex<m_ImageListThumb.GetImageCount(); nIndex++)
	{
		m_ImageListThumb.Remove(nIndex);	
	}
	
	// remove all items from list view
	if (m_pResoureListCtl->GetItemCount() != 0)
	{
		m_pResoureListCtl->DeleteAllItems();
	}
		
	// set the size of the image list
	m_ImageListThumb.SetImageCount(m_vecSupportFileName.size());
	
	nIndex = 0;
	// draw the thumbnails
	std::vector<CString>::iterator	iter;
	for (iter = m_vecSupportFileName.begin(); iter != m_vecSupportFileName.end(); iter++)
	{	
		//CString msg;
		//msg.Format(_T("当前生产第%d个文件 “%s” 的图标\n\r"),nIndex,*iter);
		//OutputDebugString(msg);
		HRESULT hr = S_FALSE;

		hr = CreateThumbnail(*iter, &hBmp);	
		if (SUCCEEDED(hr))
		{			
			Bitmap img( hBmp, NULL );
			Bitmap* pThumbnail = static_cast <Bitmap*>(img.GetThumbnailImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, NULL, NULL));

			// attach the thumbnail bitmap handle to an CBitmap object
			pThumbnail->GetHBITMAP(NULL, &hBmp);
			pImage = new CBitmap();		 
			pImage->Attach(hBmp);

			// add bitmap to our image list
			m_ImageListThumb.Replace(nIndex, pImage, NULL);

			// put item to display
			// set the image file name as item text
			m_pResoureListCtl->InsertItem(nIndex, m_vecSupportFileName[nIndex], nIndex);

			nIndex++;
			delete pImage;
			delete pThumbnail;
		} 
		else		//因目前尚不能生成其他文件类型缩略图，故读取它们的图标
		{	
			SHFILEINFO sfi;

			strPath.Format( TEXT("%s\\%s"), m_strImageDir, *iter );

			SHGetFileInfo (strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),SHGSI_ICON|SHGSI_SHELLICONSIZE |SHGFI_USEFILEATTRIBUTES );

			m_ImageListThumb.Replace(nIndex, sfi.hIcon);

			m_pResoureListCtl->InsertItem(nIndex, m_vecSupportFileName[nIndex], nIndex);
			nIndex++;
		}
	}

	// let's show the new thumbnails

	if (IsWindow(m_hWnd))
	{
	    m_pResoureListCtl->SetRedraw();  
	}
	//m_pResoureListCtl->SetRedraw();  
}

//创建缩略图
HRESULT  CWBFunctionResDlg::CreateThumbnail( CString strFileName, HBITMAP* pThumbnail )
{
	LPITEMIDLIST pidlItems = NULL, pidlURL = NULL, pidlWorkDir = NULL;
	HRESULT hr = S_OK;  
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
	/*
	hr = CoInitialize( NULL );
	if ( FAILED( hr ) )
	{

		OutputDebugString( _T( "初始化Com库失败!" ) );
		CoUninitialize();
		return S_FALSE;
	}
	*/
	// 获得IMalloc接口
	if ( SUCCEEDED( hr ) )
	{
		hr = SHGetMalloc( &pMalloc );

	}
	// 获得桌面文件夹
	hr = SHGetDesktopFolder(&psfDesktop);  
	if(SUCCEEDED(hr))
	{
		hr = SHGetDesktopFolder(&psfDesktop);  

	}
	// get working directory  	
	
	if(SUCCEEDED(hr))
	{
		//wangxiaobin我改的，m_strImageDir这个参数我不是很了解什么意思
		//MultiByteToWideChar(CP_ACP, 0, m_strImageDir, -1, wsBuffer, MAX_PATH);
		_tcscpy(m_strImageDir.GetBuffer(),wsBuffer);
	    hr = psfDesktop->ParseDisplayName(NULL, NULL, wsBuffer, NULL, &pidlWorkDir, NULL);
		m_strImageDir.ReleaseBuffer();
	}  
	 
	// ParseDisplayName:Translates a file object's or folder's display name into an item identifier list.
	if(SUCCEEDED(hr)) 
	{

	   hr = psfDesktop->BindToObject(pidlWorkDir, NULL, IID_IShellFolder, (LPVOID*)&psfWorkDir); 
	
	}

	// retrieve link information  
	if(SUCCEEDED(hr))
	{
		//MultiByteToWideChar(CP_ACP, 0, strFileName, -1, wsBuffer, MAX_PATH);
		_tcscpy(strFileName.GetBuffer(),wsBuffer);
		hr = psfWorkDir->ParseDisplayName(NULL, NULL, wsBuffer, NULL, &pidlURL, NULL);
		strFileName.ReleaseBuffer();
	}

	// query IExtractImage  
	if(SUCCEEDED(hr)) 
	{
		if (psfWorkDir)
		{
			hr = psfWorkDir->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST*)&pidlURL, IID_IExtractImage, NULL, (LPVOID*)&peiURL);
		}
	}

	// define thumbnail properties  

	if(SUCCEEDED(hr)) 
	{
		if (peiURL)
		{
			hr = peiURL->GetLocation(pszPath, MAX_PATH, NULL, &size, 24, &dwFlags);  
		}
	}
	// generate thumbnail  
	if(SUCCEEDED(hr)) 
	{
		if (peiURL)
		{
			hr = peiURL->Extract(pThumbnail);
		}
	}
		
	//peiURL->Release();  
	//peiURL = NULL;  
	// free allocated structures  
	if(peiURL != NULL) peiURL->Release();  
	if(pidlURL != NULL) pMalloc->Free(pidlURL);  
	if(pidlWorkDir != NULL) pMalloc->Free(pidlWorkDir);  
	if(psfDesktop != NULL) psfDesktop->Release();  
	if(psfWorkDir != NULL) psfWorkDir->Release();  
	//CoUninitialize();
	
	return hr; 
}

void CWBFunctionResDlg::OnLvnBegindragWbResdlgListctl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	g_nMethodOfResToView = DRAG_AND_DROP;
	int nItem;
	nItem = m_pResoureListCtl->GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);//得到当前选中项的索引值
	g_strSelectedResPath = m_LocalPath + m_vecSupportFileName[nItem];
	CString sExt;//选中资源的后缀名		
	sExt = PathFindExtension(g_strSelectedResPath);
	sExt.MakeLower();
	if ( (sExt == ".bmp") || (sExt == ".jpg") || (sExt == ".gif") 
		|| (sExt == ".png") || (sExt == ".dib") || (sExt == ".tiff") || (sExt == ".jfif") || (sExt == ".ico") || (sExt == ".jpeg") )//拖拽图片资源时
	{
		g_nDrawStatus = ADDPICTUREFROMRESDLG;
		//g_pWhiteBoardView->AddResFromResDlg(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
	}
	if ((sExt == ".doc") || (sExt == ".ppt") || (sExt == ".xls") || (sExt == ".pdf"))//拖拽文档资源时
	{
		g_nDrawStatus = ADDDOCFROMRESDLG;
		//g_pWhiteBoardView->AddResFromResDlg(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
	}
	if ((sExt == ".avi") || (sExt == ".wmv") || (sExt == ".mpeg") || (sExt == ".mpg")
		||(sExt == ".asf") || (sExt == ".mov") || (sExt == ".dat") || (sExt == ".rmvb")
		||(sExt == ".vob") || (sExt == ".mp4") || (sExt == ".f4v") || (sExt == ".mp3")
		||(sExt == ".wm") || (sExt == ".wma") || (sExt == ".rm") || (sExt == ".mts")||(sExt == ".mkv"))//拖拽视频资源时
	{
		g_nDrawStatus = ADDNEWMOVIEFROMRESDLG;
		//g_pWhiteBoardView->AddResFromResDlg(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
	}
	if ((sExt == ".swf") )//拖拽flash资源时
	{
		g_nDrawStatus = ADDFLASHFROMRESDLG;
		//g_pWhiteBoardView->AddResFromResDlg(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
	}
	*pResult = 0;
}


void CWBFunctionResDlg::OnNMDblclkWbResdlgListctl(NMHDR *pNMHDR, LRESULT *pResult)
{
	
 	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
 	// TODO: 在此添加控件通知处理程序代码
 	g_nMethodOfResToView = DOUBLE_CLK;
 	int nItem;
 	nItem = m_pResoureListCtl->GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);//得到当前选中项的索引值
 	g_strSelectedResPath = m_LocalPath + m_vecSupportFileName[nItem];
 	CString sExt;//选中资源的后缀名
 	sExt = PathFindExtension(g_strSelectedResPath);
 	sExt.MakeLower();
 	if ( (sExt == ".bmp") || (sExt == ".jpg") || (sExt == ".gif") 
 		|| (sExt == ".png") || (sExt == ".dib") || (sExt == ".tiff") || (sExt == ".jfif") || (sExt == ".ico") || (sExt == ".jpeg") )//双击图片资源时
 	{
 		g_nDrawStatus = ADDPICTUREFROMRESDLG;
 		g_pWhiteBoardView->AddResToWBView(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
 	}
 	if ((sExt == ".doc") || (sExt == ".ppt") || (sExt == ".xls") || (sExt == ".pdf"))//双击文档资源时
 	{
 		g_nDrawStatus = ADDDOCFROMRESDLG;
 		g_pWhiteBoardView->AddResToWBView(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
 	}
 	if ((sExt == ".avi") || (sExt == ".wmv") || (sExt == ".mpeg") || (sExt == ".mpg")
 		||(sExt == ".asf") || (sExt == ".mov") || (sExt == ".dat") || (sExt == ".rmvb")
 		||(sExt == ".vob") || (sExt == ".mp4") || (sExt == ".f4v") || (sExt == ".mp3")
 		||(sExt == ".wm") || (sExt == ".wma") || (sExt == ".rm") || (sExt == ".mts")||(sExt == ".mkv"))//双击视频资源时
 	{
 		g_nDrawStatus = ADDNEWMOVIEFROMRESDLG;
 		g_pWhiteBoardView->AddResToWBView(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
 	}
 	if ((sExt == ".swf") )//双击flash资源时
 	{
 		g_nDrawStatus = ADDFLASHFROMRESDLG;
 		g_pWhiteBoardView->AddResToWBView(g_nDrawStatus,g_strSelectedResPath,g_nMethodOfResToView);
 	}
 	*pResult = 0;
}

