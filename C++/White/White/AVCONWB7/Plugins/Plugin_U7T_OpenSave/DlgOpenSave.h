#pragma once
#include "win_impl_base.hpp"
#include "ListUIFile.hpp"
#include "DBOpenRecord.h"
#include "IPlugin/IU7T.Comm/IPlugin_U7_ComUI.h"
#include <winioctl.h>

enum BTN_MARK
{
	BTN_MARK_RECENT =1,
	BTN_MARK_PERSONAL ,
	BTN_MARK_DEST ,
	BTN_MARK_DRIVES,
	BTN_MARK_REMOVABLE
};

typedef std::map<int, int>		MAP_COMBO;
typedef MAP_COMBO::iterator		MAP_COMBO_IT;

class CDlgOpenSave
	: public WindowImplBase
	, public IPlugin_U7_ComUI_Observer
{
public:
	CDlgOpenSave(BOOL bOpenSave, //TRUE --OpenDlg, FALSE ----SaveDlg
		LPCTSTR lpszDefExt = NULL,              //指定默认的文件扩展名 
		LPCTSTR lpszFilter = NULL ,             //指明可供选择的文件类型和相应的扩展名
		CString strMemberID=_T("Local"),		//用于保存人员打开文件记录,默认是保存在Local文件夹下
		CString strFilePath =_T("")			//另存时，需传所需保存文件的原路径);
		);             
	//文件类型说明和扩展名间用 | 分隔，同种类型文件的扩展名间可以用 ; 分割，每种文件类型间用 | 分隔，末尾用 || 指明
	virtual ~CDlgOpenSave(void);

	CString OpenFile();

private:
	CString SearchRange_Type;

	//////////////////////////////////////////////////////////////////////////
	// 继承WindowImplBase接口
public:
	virtual void Init();
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LPCTSTR GetWindowClassName() const;
	virtual CString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void OnFinalMessage(HWND hWnd);

public:
	void OnDblclkList(int iIndex);
	void OpenNewFolder(int nIndex);
	void UpdateTableState(FILETYPE FT);
	void ShowListContent(CString strPath,int nStartIndex = 0);
	BOOL IsShowFile(CString strName);
	void UpdateButtonState();
	void SetCurFileTypes();
	void OnComboxSelChanged(OPENSAVECOMBOX OSCom);
	void UpdateGobackBtnState();
	void UpdateUpwardBtnState(FILETYPE FT);
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	// 继承ICommUISet接口
public:
	virtual void SetFont( const TFontInfo* pFontInfo );
	virtual void SetTransparent(int nOpacity);
	virtual void SetUISize(float fScale);
	virtual void SetButtonSize(float fScale);
	virtual void SetFontSize(int nSize);

	virtual HWND GetHwnd(){return m_hWnd;};
	//////////////////////////////////////////////////////////////////////////

protected:
	virtual void Notify(TNotifyUI& msg);
	bool InitControl();
	void InitBtn();		// 初始化左侧按钮	wangxin 2012/09/06 15
	void InitRemovableDiskBtn();	// 初始化可移动磁盘按钮
	void AddDevice(CString strDeviceName);
	void DeviceArrival(DWORD dwMask);	//USB设备插入
	void DeviceRemove(DWORD dwMask);// USB设备移除
	BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc); // 获取磁盘属性
	void SetSpecialFolder(DWORD dwType);
	void ShowDrivesList();
	void OnOpenSaveButton(OPENSAVEBTN OSBtn);
	void OnNotifyShowListContent(CString strName, SHFILEINFO& fileInfo, int nLeg, ListItem it, bool bNeedSel = false);
	void OnNotifyOpenSaveButton(OPENSAVEBTN osb);
	void OnNotifyCreateNewFolder(CString strFullPath);
	void OnNotifyComboxSelChanged(OPENSAVECOMBOX osc);
	virtual void OnNotifyShowRecentDoc(void); //显示打开的媒体记录
	virtual void OnNotifySetFileList( ListItem it);
	void OnListItemchanged(int nIndex);
	void OnBeginEditFileName(int nIndex);
	void OnEndEditFileName();
	void EditFileName(int nIndex);
	void EditFileNameEnd();
	void DeleteByIndex(int nIndex);
	void OnOpenOK();
	void OnOpenCancel();
	void ShowOpenFileHistory();
	void ShowFileToList(CString strFilePath, CString strFileName, int &nLength);
	void SetDeskTopList();
	void OnUpdateOpenSaveButState(bool bIsEmpty);
	bool SaveFilePath(void);
	// 打开上一次的路径
	void OpenLastDir();
	//////////////////////////////////////////////////////////////////////////
	void ReleaseAllVector();

public:
	CString GetFullPath();  // filefullpath
	CString GetFilePath();  // return only filefullpath
	CString GetFileName();  // return only filefullpath
	CString GetFileExt();   // return only ext
	CString GetFileTitle(); // return file title
	void GetFileList(CString strPath);
	void GetFileToList(CString strFilePath, CString strFileName);
	CString GetFullFilePath();//获取选中文件的全路径****************************************************************
	CControlUI* CreateBtnItem( CString strPic,CString strBtnName );
	bool  CheckFileName(CString str, CString strPath);
private:
	CListUIFile				*m_pListContent;
	CEditUI					*m_pEditFileName;
	//CRichEditUI			*m_pEditAndRenameFile;
	
	CString					m_strMemberID;
	CString					m_strDesktop;
	CString					m_strComFileName;
	CString					m_strPersonal; // 保存我的文档目录
	CString					m_strRecent;	// 保存最近文件夹目录
	CString					m_strCurPath;	// 保存当前所打开目录的路径
	vector <ListItem>		m_vListItem;
	
	vector <ListItem>		m_vFileListItem;
	int						m_nIndex;
	int						m_nCurIndexWnd; // 当前选中的子窗口
	BTN_MARK				m_eMark;

	CDBOpenRecord			m_dbOpenRecord;
	MAP_OPENFILE_RECORD		m_mapOpenFileRecord;
	int						m_nCardIndex;
	bool					m_bIsClickOK;
	bool					m_bFirst;
	int						m_nListIndex;

	CLabelUI*				m_pLblTitle;
	CLabelUI*				m_pLblSearchRange;
	CLabelUI*				m_pLblFileName;
	CLabelUI*				m_pLblFileType;
	CLabelUI*				m_pLblIcon;
	CLabelUI*				m_pLblFileSize;
	CLabelUI*				m_pLblPath;

	CListUI*				m_pItemList;
	CDialogBuilder			m_dlgBuilder;
	SIZE					m_initSize;
// 	COptionUI* m_pOptRecent;
// 	COptionUI* m_pOptPerson;

	CButtonUI* 				m_pBtnClose;
// 	CButtonUI* 				m_pBtnDest;
// 	CButtonUI* 				m_pBtnComputer;
	CButtonUI* 				m_pBtnUpward;
	CButtonUI* 				m_pBtnNewForlder;
	CButtonUI* 				m_pBtnGoBack;
	CButtonUI* 				m_pBtnPreciews;
	CButtonUI* 				m_pBtnOpen;
	CButtonUI* 				m_pBtnCancel;
	CButtonUI* 				m_pBtnRename;
	CButtonUI* 				m_pBtnDelete;

	CComboUI* 				m_pComSearchRange;
	CComboUI* 				m_pComFileName;
	CComboUI* 				m_pComFileType;

	bool      				bRecentFileSelected;
	bool      				bPercentFileSelected;
	//////////////////////////////////////////////////////////////////////////

	int                 m_nCurFolder;               //记录当前打开的文件夹
	vector<pair<CString, FILETYPE> >     m_vFilePath;//保存已打开过的文件
	vector<CString>     m_vFileType;              //保存各行文件类型的字符串
	vector<CString>		m_vShowType;              //当前各种需要显示的文件类型
	CString				m_strFileFullPathOut;
	BOOL				m_bOpenSave;

	CString         	m_strFilter;   // filter string	 // separate fields with '|', terminate with '||\0'
	CString         	m_strDefExt;

	CString				m_strPathName;  //out
	CString				m_strSourceName;//hxl 2012 5 12 只在另存时，保存源文件资源路径,解决文件不能保存在新文件夹里bug

	MAP_COMBO			m_mapCombo;	// Combo控件对应要查找的磁盘	wangxin 2012/09/06 15
	bool				m_bXPE;		// 是否是XPE，即终端
	HDEVNOTIFY			m_hDevNotify;// 设备通知句柄
	int					m_nEditIndex;				//当前正在编辑的文件/文件夹的序号 -1为无效
};
