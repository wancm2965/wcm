#pragma once
#include "stdafx.h"
#include "WBLoginDlg.h"
#include "IPlugin/WB_Main/IPlugin_WB_Main.h"
#include "Independent_WBDlg.h"
#include "IPlugin\IU7_MM\IPlugin_U7_MM_Member.h"
#include "IPlugin\IU7_MM\IPlugin_U7_MM_Function.h"
using namespace AVCONPlugin;

// CWBMainDlg 对话框

class CWBMainDlg : public CDialog
	, public IHPNetNotify
	, public IPlugin_U7_MM_Member_Observer
	, public IPlugin_U7_MM_Function_Observer
{
	DECLARE_DYNAMIC(CWBMainDlg)

public:
	CWBMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBMainDlg();
	static CWBMainDlg *g_pMainDlg; //白板主窗口指针
	static bool g_isCreateWb;//是否创建白板
	static bool g_isNetHPDTSConnect;            //白板是不是具有通讯功能

	void ProcessWBToLocalData( char* pNotifyData );//接收电子白板内部发出来的信令

	// 对话框数据
	enum { IDD = IDD_WBMAINDLG };
private:

	//通讯指针
	void*	m_pHPNetNotify; //通讯指针
	CWBLoginDlg *m_pWBLoginDlg;
	CIndependent_WBDlg *m_pWhiteBoardDlg;			  //在重新写一个类	用来加载白板
	HWND m_hChildWnd; //白板句柄
	CRect m_RectWB;
private:
	bool		m_bShow;
	bool		m_bInit;
	BOOL	m_isOnline; //判断是不是已经联网
	BOOL	m_isReceiveBoardcast;
	BOOL	m_isLocalBoardCastWB; //判断是不是本地广播的
	BOOL   m_is;
	/*		CButtonUI*	m_pBtnUI;*/



	//菜单中图片
	//文件主菜单
	CBitmap LoginBmp; //登录网络版
	CBitmap LogoutBmp; //退出网络变成单机版
	CBitmap NewFileBmp; //新建文件
	CBitmap OpenFileBmp; //打开文件
	CBitmap SaveFileBmp; //保存文件
	CBitmap ResaveFileBmp; //另存文件
	CBitmap PrintFileBmp; //打印文件
	CBitmap CloseFileBmp; //退出文件
	//页主菜单
	CBitmap NewPageBmp; //新建页
	CBitmap PrePageBmp; //上一页
	CBitmap NextPageBmp; //下一页
	CBitmap FirstPageBmp; //第一页
	CBitmap LastPageBmp; //末页
	CBitmap INSERTPageBmp; //插入页
	CBitmap DELETEPageBmp; //删除页
	//编辑主菜单
	CBitmap SelectBmp; //选择
	CBitmap UndoBmp; //撤销
	CBitmap RedoBmp; //重做
	CBitmap CloneBmp; //克隆
	CBitmap InfinitCloneBmp; //无限克隆
	CBitmap CutBmp; //剪切
	CBitmap CopyBmp; //复制
	CBitmap PasteBmp; //粘贴
	CBitmap DeleteBmp; //删除
	CBitmap DeleteAllBmp; //全部删除

	CBitmap PreLayerBmp; //上一层
	CBitmap NextLayerBmp; //下一层
	CBitmap TopLayerBmp; //顶层
	CBitmap BottomLayerBmp; //底层

	CBitmap Deasil45Bmp; //顺时针45度
	CBitmap Deasil90Bmp; //顺时针90度
	CBitmap Widderstin45Bmp; //逆时针旋转45度
	CBitmap Widderstin90Bmp; //逆时针选择90度
	CBitmap Rotation180Bmp; //旋转189度
	CBitmap HirizontalBmp; //水平放置
	CBitmap VerticalBmp; //垂直放置
	//插入主菜单
	CBitmap PictureBmp; //图片
	CBitmap VedioBmp; //视频
	CBitmap FlashBmp; //FLASH
	CBitmap DocumentsBmp; //文档
	CBitmap CameraheadBmp; //摄像头
	CBitmap TextBmp; //文本
	CBitmap AnnotationBmp; //备注
	//绘制主菜单
	CBitmap BeelinesBmp; //直线
	CBitmap CurvesBmp; //曲线
	CBitmap RectBmp; //矩形
	CBitmap TrianglesBmp; //三角形
	CBitmap RoundBmp; //圆
	CBitmap RhombusBmp; //菱形

	CBitmap BigRubberBmp; //大擦
	CBitmap SmallRubberBmp; //小擦
	CBitmap CurveRubbersBmp; //曲擦
	CBitmap ObjRubbersBmp; //擦对象
	//工具主菜单
	CBitmap BaffleBmp; //挡板
	CBitmap SpotlightBmp; //聚光灯
	CBitmap MagnifierBmp; //放大镜
	CBitmap CutScreenBmp; //截屏
	CBitmap RecycleBmp; //回收站
	CBitmap ScreenRECBmp; //屏幕录制
	CBitmap DrawDesktopOpenBmp; //打开标注
	CBitmap DrawDesktopEffectSwitchBmp; //效果切换
	CBitmap DrawDesktopCloseBmp; //关闭标注
	CBitmap ResourceListBmp; //资源目录
	CBitmap ResourceManageBmp; //资源目录管理
	CBitmap FunctionDlgBmp; //功能面板

	//帮助主菜单
	CBitmap AboutBmp; //资源目录管理
	CBitmap HelpBmp; //功能面板

	HICON m_hIcon;

	CTreeCtrl m_memberListTree;//树形控件
	CImageList m_memberAvatarLst;//与会成员头像列表
	HTREEITEM m_memberlistTreeRoot;//成员列表根项
	HTREEITEM m_lastOfflineMemberHitem;//最末位置的离线成员
	HTREEITEM m_lastOnlineMemberHitem;//最末位置的在线成员

	int m_onLineCurMeetingNumber;//当前在线人数
	unsigned short m_usMemberType;	   //登入用户所在的用户组
	bool m_LimitsOfPower;//用户权限
	bool m_isNeedRealseTouchScreenAuthorize;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();

	void SetHPDTSNotify(IHPNetNotify * pHPDTSNotify);	//设置通讯指针
	void* GetHPDTSNotify();			   //获得通讯指针
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

	//登入框
	void OpenLoginDlg();
	void CloseLoginDlg();
	void DeleteLoginDlg();
	// 
	// 			//继承自IPlugin接口 
	// 	public:
	// 		// 获取插件名称
	// 		virtual std::string Name();
	// 		// 获取插件功能点
	// 		virtual std::string ExtendPoint();
	// 		// 初始化插件
	// 		virtual bool InitPlugin();
	// 		// 插件退出处理
	// 		virtual bool ClosePlugin();
	// 
	// 		//继承自IPlugin_U7Meeting_Comm接口 
	// 	public:
	// 		//界面调整接口
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

	//		virtual void ProcessLogout();//注销
	//		virtual	void ChangeBtnState(bool bClicked);

	//继承自IHPNetNotify接口
public:
	virtual void SendWbData(const char* pData,int iLen,bool bToNet = true);


	void InitMemberList();//联机版的成员列表
	void UpdateMemberStatus(std::string strMemberID, unsigned char nStatus);
	int GetOnlineMemberCounts();
	afx_msg void OnTvnSelchangedMemberlistTree(NMHDR *pNMHDR, LRESULT *pResult);


	// 继承自 IPlugin_U7_MM_Member_Observer 接口
	virtual void OnRoomGroupItem(const MMS_GROUP_EX& item){};
	virtual void OnRoomGroupItemEnd(void){};
	virtual void OnRoomMemberItem(const MMS_MEMBER_EX& item){};
	virtual void OnRoomMemberItemEnd(void);
	virtual void OnMemberCardItem(const char* strGroupID, const char* strMemberID, unsigned short nCardIndex, const char* strCardName){};
	virtual void OnMemberCardItemEnd(void){};
	virtual void OnSetCardName(const char* strGroupID, const char* strMemberID, unsigned short nCardIndex, const char* strCardName){};

	// 收到会议透传消息
	virtual void OnMMUserData(const char* strFromID, const char* strCmdType, const char* strUserData);

	virtual void OnMemberStatus(const char* strGroupID,const char* strMemberID,unsigned short usMemberType,unsigned char ucStatus);

	virtual void OnInvite(const std::string& strRoomID,const std::string& strRoomName,const std::string& strMemberID,const std::string& strMemberName){};
	virtual void OnReject( const std::string& strDomain,const std::string& strRoomID,const std::string& strInviteeID,const std::string& strInviteeName ){};

	// 继承IPlugin_U7_MM_Function_Observer接口
	virtual void OnBroadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,int nChannelType);
	virtual void OnCloseBroadcast(const char* strMemberID,int nCardIndex,int nChannelType);


	virtual void OnRecvChatMessage( const std::string& strMemberID,const std::string& strHead,const std::string& strBody,const std::string& strFormat ){};
	/* 拖动窗口 */
	virtual void OnDragWindow(int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID){};
	/* 改变模板 */
	virtual void OnSetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber){};

	/*设置当前激活屏*/
	virtual void OnSetCurrentScreenID(int nCurScrID){};

	virtual void OnSetFullScreen(const std::string& strMemberID,int nCardIndex,int nScreenID,int nWindowID,int nFullType){};

	/* 浮动窗口 */
	virtual void OnSetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY){};

	// 触摸授权回调
	virtual void OnTouchScreenAuthorize(bool bAuthorize);

	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
};

