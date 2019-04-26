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
		LPCTSTR lpszDefExt = NULL,              //ָ��Ĭ�ϵ��ļ���չ�� 
		LPCTSTR lpszFilter = NULL ,             //ָ���ɹ�ѡ����ļ����ͺ���Ӧ����չ��
		CString strMemberID=_T("Local"),		//���ڱ�����Ա���ļ���¼,Ĭ���Ǳ�����Local�ļ�����
		CString strFilePath =_T("")			//���ʱ���贫���豣���ļ���ԭ·��);
		);             
	//�ļ�����˵������չ������ | �ָ���ͬ�������ļ�����չ��������� ; �ָÿ���ļ����ͼ��� | �ָ���ĩβ�� || ָ��
	virtual ~CDlgOpenSave(void);

	CString OpenFile();

private:
	CString SearchRange_Type;

	//////////////////////////////////////////////////////////////////////////
	// �̳�WindowImplBase�ӿ�
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
	// �̳�ICommUISet�ӿ�
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
	void InitBtn();		// ��ʼ����ఴť	wangxin 2012/09/06 15
	void InitRemovableDiskBtn();	// ��ʼ�����ƶ����̰�ť
	void AddDevice(CString strDeviceName);
	void DeviceArrival(DWORD dwMask);	//USB�豸����
	void DeviceRemove(DWORD dwMask);// USB�豸�Ƴ�
	BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc); // ��ȡ��������
	void SetSpecialFolder(DWORD dwType);
	void ShowDrivesList();
	void OnOpenSaveButton(OPENSAVEBTN OSBtn);
	void OnNotifyShowListContent(CString strName, SHFILEINFO& fileInfo, int nLeg, ListItem it, bool bNeedSel = false);
	void OnNotifyOpenSaveButton(OPENSAVEBTN osb);
	void OnNotifyCreateNewFolder(CString strFullPath);
	void OnNotifyComboxSelChanged(OPENSAVECOMBOX osc);
	virtual void OnNotifyShowRecentDoc(void); //��ʾ�򿪵�ý���¼
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
	// ����һ�ε�·��
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
	CString GetFullFilePath();//��ȡѡ���ļ���ȫ·��****************************************************************
	CControlUI* CreateBtnItem( CString strPic,CString strBtnName );
	bool  CheckFileName(CString str, CString strPath);
private:
	CListUIFile				*m_pListContent;
	CEditUI					*m_pEditFileName;
	//CRichEditUI			*m_pEditAndRenameFile;
	
	CString					m_strMemberID;
	CString					m_strDesktop;
	CString					m_strComFileName;
	CString					m_strPersonal; // �����ҵ��ĵ�Ŀ¼
	CString					m_strRecent;	// ��������ļ���Ŀ¼
	CString					m_strCurPath;	// ���浱ǰ����Ŀ¼��·��
	vector <ListItem>		m_vListItem;
	
	vector <ListItem>		m_vFileListItem;
	int						m_nIndex;
	int						m_nCurIndexWnd; // ��ǰѡ�е��Ӵ���
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

	int                 m_nCurFolder;               //��¼��ǰ�򿪵��ļ���
	vector<pair<CString, FILETYPE> >     m_vFilePath;//�����Ѵ򿪹����ļ�
	vector<CString>     m_vFileType;              //��������ļ����͵��ַ���
	vector<CString>		m_vShowType;              //��ǰ������Ҫ��ʾ���ļ�����
	CString				m_strFileFullPathOut;
	BOOL				m_bOpenSave;

	CString         	m_strFilter;   // filter string	 // separate fields with '|', terminate with '||\0'
	CString         	m_strDefExt;

	CString				m_strPathName;  //out
	CString				m_strSourceName;//hxl 2012 5 12 ֻ�����ʱ������Դ�ļ���Դ·��,����ļ����ܱ��������ļ�����bug

	MAP_COMBO			m_mapCombo;	// Combo�ؼ���ӦҪ���ҵĴ���	wangxin 2012/09/06 15
	bool				m_bXPE;		// �Ƿ���XPE�����ն�
	HDEVNOTIFY			m_hDevNotify;// �豸֪ͨ���
	int					m_nEditIndex;				//��ǰ���ڱ༭���ļ�/�ļ��е���� -1Ϊ��Ч
};
