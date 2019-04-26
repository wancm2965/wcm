
//========================================================================================
//版权所有: 版权所有(C) 2008，上海华平信息技术股份有限公司
//系统名称: AVCON7
//作　　者: duquansheng
//创建日期: 2008-5-19
//功能说明: 会议数据客户端动态库DTC.dll(文档共享、flash共享)
//

#ifndef _HPDTC_H
#define _HPDTC_H


#define USE_AVCONDOC_PRINT	//启用avcondoc虚拟打印
#undef  USE_AVCONDOC_PRINT


//文档共享回调
class IHPDTCDocShareNotify
{
public:
	IHPDTCDocShareNotify(){};
	~IHPDTCDocShareNotify(){};	

public:
	virtual void onDocShareNotifyData(const char* pData,int nLen) = 0;			//发送数据
	virtual void onDocShareNotifyRecordData(const char* pData,int nLen) = 0;	//录像数据
	virtual void onDocShareNotifyClose(void) = 0;								//内部关闭回调
	virtual void onDocShareNotifyOnLButtonDown(UINT nFlags, CPoint point){};	//会议拖动窗口用		
	virtual void onDocShareNotifyFullScreen(bool bFull,bool bSelf){};			//是否全屏窗口bSelf:是否自己发起的全屏
	virtual void onDocShareNotifyShowMode(bool bMode){};						//改变显示模式bMode：是否按文档比例显示
	virtual std::string GetLocalMemberID(void) = 0;								//获取本地用户ID
	virtual void onDocShareNotifyShowImgFile(void) = 0;							//文档下载完毕并已显示
	virtual	bool IsConferencePlayer(void) = 0;									//是否为会议播放器
	virtual	bool IsPrivilegeMaxWindow(void) = 0;								//是否有全屏窗口权限
	virtual void onDocShareNotifyActive(const std::string& strMemberID, unsigned long ulDocIndex) =0;
};

//文档共享
class IHPDTCDocShare
{
public:
	IHPDTCDocShare(){};
	~IHPDTCDocShare(){};
public:
	virtual bool Connect(void) = 0;										//建立连接
	virtual void Release(void) = 0;										//关闭释放资源
	virtual void processDocShareData(const char* pData,int nLen) = 0;	//处理数据
	virtual void SetLabelRight(bool bAllow) = 0;						//设置标注权限（默认允许）	
	virtual void SetCloseRight(bool bAllow) = 0;						//设置关闭权限（默认允许）	
	virtual void StartRecord(void) = 0;									//启动录像
	virtual void StopRecord(void) = 0;									//停止录像
	virtual void processRecordData(const char* pData,int nLen) = 0;		//处理录像数据
	virtual unsigned short GetDocWidth(void) = 0;						//获取文档宽度
	virtual unsigned short GetDocHeight(void) = 0;						//获取文档高度
	virtual HWND GetDocHwnd(void) = 0;									//获取窗口
	virtual void SetDocWindowPos(int x,int y,int cx,int cy) = 0;		//设置文档窗口大小
	virtual void ShowDocWindow(bool bShow) = 0;							//显示/隐藏窗口
	virtual void SetFullScreen(bool bFull) = 0;							//设置全屏
	virtual void SetMemberName(std::string strMemberName) = 0;			//设置用户名称
	virtual std::string GetMemberName(void) = 0;						//设置用户名称
	virtual std::string GetMemberID(void) = 0;							//获取用户ID
	virtual unsigned long GetDocIndex(void) = 0;						//获取DocIndex:文档序号
	virtual bool IsShowImgFile(void) = 0;								//是否已打开文档
	virtual bool IsShowMode(void) = 0;									//是否按文档比例显示
	virtual void HideShortDlg(void) = 0;								//隐藏所有快捷按钮
	virtual void OnParentMove(bool IsParentShow) = 0;
	virtual void OnControlDirect(DWORD dDerict)=0;						//方向键
};

//创建文档共享实例。strFile 需要打印的文件，此函数接口有文档打印阻塞，打印失败返回 NULL
AFX_EXT_CLASS IHPDTCDocShare* CreateDocShare(const std::string& strFile,unsigned long ulDocIndex,const std::string& strMemberID,IHPDTCDocShareNotify& rNotify,CWnd* pWnd);


//文件分发回调
class AFX_EXT_CLASS IHPDTCFileSendNotify
{
public:
	IHPDTCFileSendNotify(){};
	~IHPDTCFileSendNotify(){};	

public:
	virtual void onFileSendNotifyData(const char* pData,int nLen) = 0;			//发送数据到网络
    virtual void AutoPopupFileTip(CString UpFileUserName) = 0;
};
//文件分发
class AFX_EXT_CLASS IHPDTCFileSend
{
public:
	IHPDTCFileSend(){};
	~IHPDTCFileSend(){};
public:
	virtual bool Connect(void) = 0;										//建立连接
	virtual void Release(void) = 0;										//关闭释放资源
	virtual void processFileSendData(const char* pData,int nLen) = 0;	//处理网络数据	
	virtual void ShowFileWindow(bool bShow, bool bIsMyHost = true) = 0;	//显示窗口	nShowType：显示类型 真为主持人，假为与会者
	virtual void SetIsMyHost(bool bIsHost)=0;
};

//创建文档共享实例。strFile 需要打印的文件，此函数接口有文档打印阻塞，打印失败返回 NULL
AFX_EXT_CLASS IHPDTCFileSend* CreateFileSend(const std::string& strMemberID, IHPDTCFileSendNotify& rNotify, CWnd* pWnd, bool bIsMyHost = true);
AFX_EXT_CLASS void DeteleFileSend(IHPDTCFileSend* pHPDTCFileSend);
/*


//Flash共享回调
class IHPDTCFlashShareNotify
{
public:
	IHPDTCFlashShareNotify(){};
	~IHPDTCFlashShareNotify(){};	

public:
	virtual void onFlashShareNotifyNetData(const char* pData,int nLen) = 0;		//发送到网络数据
	virtual void onFlashShareNotifyRecordData(const char* pData,int nLen) = 0;	//录像数据
	virtual void onFlashShareNotifyClose() = 0;									//内部关闭回调
	virtual void onFlashShareNotifyOnLButtonDown(UINT nFlags, CPoint point){};	//会议拖动窗口用		

};

//Flash共享
class IHPDTCFlashShare
{
public:
	IHPDTCFlashShare(){};
	~IHPDTCFlashShare(){};
public:
	virtual void Release() = 0;											//关闭释放资源
	virtual void processFlashShareData(const char* pData,int nLen) = 0;	//处理网络数据
	virtual void StartRecord(void) = 0;									//启动录像
	virtual void StopRecord(void) = 0;									//停止录像

};

//创建Flash共享
AFX_EXT_CLASS IHPDTCFlashShare * CreateFlashShare(const std::string& strFile,const std::string& strUserID,IHPDTCFlashShareNotify& rNotify,CWnd* pWnd);

*/

#endif
