#pragma once
#include "stdafx.h"
#include "WBLoginDlg.h"
#include "IPlugin/WB_Main/IPlugin_WB_Main.h"
#include "Independent_WBDlg.h"
#include "IPlugin\IU7_MM\IPlugin_U7_MM_Member.h"
#include "IPlugin\IU7_MM\IPlugin_U7_MM_Function.h"
using namespace AVCONPlugin;

// CWBMainDlg �Ի���

class CWBMainDlg : public CDialog
	, public IHPNetNotify
	, public IPlugin_U7_MM_Member_Observer
	, public IPlugin_U7_MM_Function_Observer
{
	DECLARE_DYNAMIC(CWBMainDlg)

public:
	CWBMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBMainDlg();
	static CWBMainDlg *g_pMainDlg; //�װ�������ָ��
	static bool g_isCreateWb;//�Ƿ񴴽��װ�
	static bool g_isNetHPDTSConnect;            //�װ��ǲ��Ǿ���ͨѶ����

	void ProcessWBToLocalData( char* pNotifyData );//���յ��Ӱװ��ڲ�������������

	// �Ի�������
	enum { IDD = IDD_WBMAINDLG };
private:

	//ͨѶָ��
	void*	m_pHPNetNotify; //ͨѶָ��
	CWBLoginDlg *m_pWBLoginDlg;
	CIndependent_WBDlg *m_pWhiteBoardDlg;			  //������дһ����	�������ذװ�
	HWND m_hChildWnd; //�װ���
	CRect m_RectWB;
private:
	bool		m_bShow;
	bool		m_bInit;
	BOOL	m_isOnline; //�ж��ǲ����Ѿ�����
	BOOL	m_isReceiveBoardcast;
	BOOL	m_isLocalBoardCastWB; //�ж��ǲ��Ǳ��ع㲥��
	BOOL   m_is;
	/*		CButtonUI*	m_pBtnUI;*/



	//�˵���ͼƬ
	//�ļ����˵�
	CBitmap LoginBmp; //��¼�����
	CBitmap LogoutBmp; //�˳������ɵ�����
	CBitmap NewFileBmp; //�½��ļ�
	CBitmap OpenFileBmp; //���ļ�
	CBitmap SaveFileBmp; //�����ļ�
	CBitmap ResaveFileBmp; //����ļ�
	CBitmap PrintFileBmp; //��ӡ�ļ�
	CBitmap CloseFileBmp; //�˳��ļ�
	//ҳ���˵�
	CBitmap NewPageBmp; //�½�ҳ
	CBitmap PrePageBmp; //��һҳ
	CBitmap NextPageBmp; //��һҳ
	CBitmap FirstPageBmp; //��һҳ
	CBitmap LastPageBmp; //ĩҳ
	CBitmap INSERTPageBmp; //����ҳ
	CBitmap DELETEPageBmp; //ɾ��ҳ
	//�༭���˵�
	CBitmap SelectBmp; //ѡ��
	CBitmap UndoBmp; //����
	CBitmap RedoBmp; //����
	CBitmap CloneBmp; //��¡
	CBitmap InfinitCloneBmp; //���޿�¡
	CBitmap CutBmp; //����
	CBitmap CopyBmp; //����
	CBitmap PasteBmp; //ճ��
	CBitmap DeleteBmp; //ɾ��
	CBitmap DeleteAllBmp; //ȫ��ɾ��

	CBitmap PreLayerBmp; //��һ��
	CBitmap NextLayerBmp; //��һ��
	CBitmap TopLayerBmp; //����
	CBitmap BottomLayerBmp; //�ײ�

	CBitmap Deasil45Bmp; //˳ʱ��45��
	CBitmap Deasil90Bmp; //˳ʱ��90��
	CBitmap Widderstin45Bmp; //��ʱ����ת45��
	CBitmap Widderstin90Bmp; //��ʱ��ѡ��90��
	CBitmap Rotation180Bmp; //��ת189��
	CBitmap HirizontalBmp; //ˮƽ����
	CBitmap VerticalBmp; //��ֱ����
	//�������˵�
	CBitmap PictureBmp; //ͼƬ
	CBitmap VedioBmp; //��Ƶ
	CBitmap FlashBmp; //FLASH
	CBitmap DocumentsBmp; //�ĵ�
	CBitmap CameraheadBmp; //����ͷ
	CBitmap TextBmp; //�ı�
	CBitmap AnnotationBmp; //��ע
	//�������˵�
	CBitmap BeelinesBmp; //ֱ��
	CBitmap CurvesBmp; //����
	CBitmap RectBmp; //����
	CBitmap TrianglesBmp; //������
	CBitmap RoundBmp; //Բ
	CBitmap RhombusBmp; //����

	CBitmap BigRubberBmp; //���
	CBitmap SmallRubberBmp; //С��
	CBitmap CurveRubbersBmp; //����
	CBitmap ObjRubbersBmp; //������
	//�������˵�
	CBitmap BaffleBmp; //����
	CBitmap SpotlightBmp; //�۹��
	CBitmap MagnifierBmp; //�Ŵ�
	CBitmap CutScreenBmp; //����
	CBitmap RecycleBmp; //����վ
	CBitmap ScreenRECBmp; //��Ļ¼��
	CBitmap DrawDesktopOpenBmp; //�򿪱�ע
	CBitmap DrawDesktopEffectSwitchBmp; //Ч���л�
	CBitmap DrawDesktopCloseBmp; //�رձ�ע
	CBitmap ResourceListBmp; //��ԴĿ¼
	CBitmap ResourceManageBmp; //��ԴĿ¼����
	CBitmap FunctionDlgBmp; //�������

	//�������˵�
	CBitmap AboutBmp; //��ԴĿ¼����
	CBitmap HelpBmp; //�������

	HICON m_hIcon;

	CTreeCtrl m_memberListTree;//���οؼ�
	CImageList m_memberAvatarLst;//����Աͷ���б�
	HTREEITEM m_memberlistTreeRoot;//��Ա�б����
	HTREEITEM m_lastOfflineMemberHitem;//��ĩλ�õ����߳�Ա
	HTREEITEM m_lastOnlineMemberHitem;//��ĩλ�õ����߳�Ա

	int m_onLineCurMeetingNumber;//��ǰ��������
	unsigned short m_usMemberType;	   //�����û����ڵ��û���
	bool m_LimitsOfPower;//�û�Ȩ��
	bool m_isNeedRealseTouchScreenAuthorize;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();

	void SetHPDTSNotify(IHPNetNotify * pHPDTSNotify);	//����ͨѶָ��
	void* GetHPDTSNotify();			   //���ͨѶָ��
	void SetIsOnline( BOOL isOnline );
	void SetWBStatic();
	void SetisLocalBoardCastWB(BOOL dispute);
	void SetMMS_Member_Type(unsigned short usMemberType);
	void LockMenu();
	void UnLockMenu();
	BOOL LimitsOfPower();
	void SetIsNeedRealseTouchScreenAuthorize(bool bIsNeedRealseTouchScreenAuthorize);
	bool GetIsNeedRealseTouchScreenAuthorize();
	CWBLoginDlg *GetLoginDlg();
	void SetIsReceiveBoardcast(bool bIsReceiveBoardcast);
protected:
	virtual void OnCancel();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//�����
	void OpenLoginDlg();
	void CloseLoginDlg();
	void DeleteLoginDlg();
	// 
	// 			//�̳���IPlugin�ӿ� 
	// 	public:
	// 		// ��ȡ�������
	// 		virtual std::string Name();
	// 		// ��ȡ������ܵ�
	// 		virtual std::string ExtendPoint();
	// 		// ��ʼ�����
	// 		virtual bool InitPlugin();
	// 		// ����˳�����
	// 		virtual bool ClosePlugin();
	// 
	// 		//�̳���IPlugin_U7Meeting_Comm�ӿ� 
	// 	public:
	// 		//��������ӿ�
	// 		virtual void SetUISize(float fScale){}
	// 		virtual void SetButtonSize(float fScale){}
	// 		virtual void SetFont(const TFontInfo* pFontInfo){}
	// 		virtual void SetOpacity(int nOpacity){}
	// 
	// 		virtual const char* GetButtonImage(void);
	// 		virtual const char* GetButtonName(void);
	// 		virtual void MainFrameHWnd(HWND hWnd){}
	// 		virtual void OnButtonClick(void);
	// 		virtual void SetButton(CButtonUI* pBtnUI);
	// 		virtual void ReleseResource();//hxl
public:
	virtual HWND GetHwnd();
	virtual HWND CreateWhiteBoard();
	virtual void DestroyWhiteBoard();
	virtual bool Connect();
	virtual void DisConnect();

	virtual void RequestAllObj();
	virtual void SendAllObj();
	virtual void CloseWBFile();
	virtual void SetLocalUserID(char *strUserID);
	virtual void MemberOffline(char *strUserID);
	virtual void Move(int x,int y,int cx,int cy,UINT nFlag);
	virtual void ProcessWbData(const char *pData, int nLen);
	virtual void UploadChildWnd( HWND HChildWnd );

	//		virtual void ProcessLogout();//ע��
	//		virtual	void ChangeBtnState(bool bClicked);

	//�̳���IHPNetNotify�ӿ�
public:
	virtual void SendWbData(const char* pData,int iLen,bool bToNet = true);


	void InitMemberList();//������ĳ�Ա�б�
	void UpdateMemberStatus(std::string strMemberID, unsigned char nStatus);
	int GetOnlineMemberCounts();
	afx_msg void OnTvnSelchangedMemberlistTree(NMHDR *pNMHDR, LRESULT *pResult);


	// �̳��� IPlugin_U7_MM_Member_Observer �ӿ�
	virtual void OnRoomGroupItem(const MMS_GROUP_EX& item){};
	virtual void OnRoomGroupItemEnd(void){};
	virtual void OnRoomMemberItem(const MMS_MEMBER_EX& item){};
	virtual void OnRoomMemberItemEnd(void);
	virtual void OnMemberCardItem(const char* strGroupID, const char* strMemberID, unsigned short nCardIndex, const char* strCardName){};
	virtual void OnMemberCardItemEnd(void){};
	virtual void OnSetCardName(const char* strGroupID, const char* strMemberID, unsigned short nCardIndex, const char* strCardName){};

	// �յ�����͸����Ϣ
	virtual void OnMMUserData(const char* strFromID, const char* strCmdType, const char* strUserData);

	virtual void OnMemberStatus(const char* strGroupID,const char* strMemberID,unsigned short usMemberType,unsigned char ucStatus);

	virtual void OnInvite(const std::string& strRoomID,const std::string& strRoomName,const std::string& strMemberID,const std::string& strMemberName){};
	virtual void OnReject( const std::string& strDomain,const std::string& strRoomID,const std::string& strInviteeID,const std::string& strInviteeName ){};

	// �̳�IPlugin_U7_MM_Function_Observer�ӿ�
	virtual void OnBroadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,int nChannelType);
	virtual void OnCloseBroadcast(const char* strMemberID,int nCardIndex,int nChannelType);


	virtual void OnRecvChatMessage( const std::string& strMemberID,const std::string& strHead,const std::string& strBody,const std::string& strFormat ){};
	/* �϶����� */
	virtual void OnDragWindow(int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID){};
	/* �ı�ģ�� */
	virtual void OnSetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber){};

	/*���õ�ǰ������*/
	virtual void OnSetCurrentScreenID(int nCurScrID){};

	virtual void OnSetFullScreen(const std::string& strMemberID,int nCardIndex,int nScreenID,int nWindowID,int nFullType){};

	/* �������� */
	virtual void OnSetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY){};

	// ������Ȩ�ص�
	virtual void OnTouchScreenAuthorize(bool bAuthorize);

	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
};

