#include "stdafx.h"
#include "ListUIFile.hpp"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#pragma comment(lib, "comctl32.lib")

const TCHAR* const kLblIconControlName = _T("LblIcon");
const TCHAR* const kLbFileNameControlName = _T("LbFileName");
const TCHAR* const kLbFileSizeControlName = _T("LbFileSize");

CListUIFile::CListUIFile(CPaintManagerUI& paint_manager)
: paint_manager_(paint_manager)
{
	SetItemShowHtml(true);
	m_nBmpCount = 0;
	bDlgBuilder = true;
}

CListUIFile::~CListUIFile()
{

}

void CListUIFile::AddNode( CString strName, SHFILEINFO& shfi, int nLeg,ListItem It)
{
	CListContainerElementUI* pListElement = NULL;
	paint_manager_.SetResourcePath(GetFolderPath());
	if(  !m_dlgBuilder.GetMarkup()->IsValid())
	{
		pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("FileItem.xml"), (UINT)0, NULL, &paint_manager_));
	}
	else 
	{
		pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create((UINT)0, &paint_manager_));
	}

	if (pListElement == NULL)
		return ;

	CLabelUI* pLabelName = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kLblIconControlName));
	if (pLabelName != NULL)
	{	
		IID IID_ImageList = {0x46EB5926, 0x582E, 0x4017, 0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x09, 0x50};
		HIMAGELIST hImageList = 0;
		SHGetImageList(SHIL_LARGE, (const IID &)IID_ImageList, (void**)&hImageList);
		HICON hIcon = ImageList_GetIcon(hImageList, shfi.iIcon, ILD_NORMAL);
		ICONINFO Iconinfo;
		ZeroMemory(&Iconinfo, sizeof(ICONINFO)); //初始化了Iconinfo
		GetIconInfo(hIcon,&Iconinfo);
		CString strFileName = _T("");
		CString strFilePath = _T("");
		CString strBmpPath = _T("");

		strBmpPath.Format(_T("%sskin\\TempBmp"), GetExePath());
		strFilePath.Format(_T("%sskin\\TempBmp\\strTempFileName_%d.bmp"), GetExePath(), m_nBmpCount++);
		if(!PathFileExists(strBmpPath))
		{
			MakeDirEx(CCommFunction::U2A(strBmpPath).c_str());
		}
		SaveBmp(Iconinfo.hbmColor,strFilePath);
		DeleteObject(Iconinfo.hbmColor);
		DeleteObject(Iconinfo.hbmMask);
		DestroyIcon(shfi.hIcon);
		DestroyIcon(hIcon);
		strFileName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('\\') - 1);
		CString strBK = _T("");
		// 设置透明掩码为黑色，如果需要更改，那么要同时更改保存图片时的背景色	wangxin 2012/09/06 17
		strBK.Format(_T("file='%s' dest='4,4,36,36' mask='#FF000000'"), strFilePath);//图标都是32X32的
	
		pLabelName->SetBkImage(strBK);
 		pLabelName->SetFixedHeight(40);
 		pLabelName->SetFixedWidth(40);
	}

	CLabelUI* pTextFileName = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kLbFileNameControlName));
	if (pTextFileName != NULL)
	{
		pTextFileName->OnEvent += MakeDelegate(this, &CListUIFile::OnItemlbEvent);
		pTextFileName->SetToolTip(strName);
		pTextFileName->SetText(strName);
	}

	CLabelUI* pTextFileSize = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kLbFileSizeControlName));
	if (pTextFileSize != NULL)
	{
		CString strFullFileName;
		strFullFileName.Format(_T("%s%s"),It.strFilePath,It.strFileName);
		CString strtemp = GetFileSize(strFullFileName);
		pTextFileSize->SetText(strtemp);
	}

	pListElement->SetTag(nLeg);
	pListElement->SetFixedHeight(40);
	//pListElement->SetFixedWidth(200);

	if(!CListUI::AddAt(pListElement,nLeg))
	{
		delete pListElement;
	}
}

bool CListUIFile::OnItemlbEvent( void* p )
{
	TEventUI *pEvent = static_cast<TEventUI*>(p);
	if (( pEvent->Type == UIEVENT_MOUSEHOVER 
		|| pEvent->Type == UIEVENT_MOUSELEAVE
		|| pEvent->Type == UIEVENT_MOUSEENTER
		|| pEvent->Type == UIEVENT_MOUSEMOVE) && pEvent->pSender)
	{
		CControlUI *pControl = pEvent->pSender->GetParent();
		while(pControl)
		{
			pControl->Event(*pEvent);
			pControl = pControl->GetParent();
		}
	}
	return true;
}