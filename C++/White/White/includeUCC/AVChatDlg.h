#ifndef _AVFDLGCHAT_H
#define _AVFDLGCHAT_H

class IDlgChatNotify
{
public:
	virtual void OnMessageSend(CString SendUserID,CString RecvUserID,CString text,CString fontface,CString fontsize,CString fontcolor,CString fontEffects)=0;        
																						//发送消息
	//virtual void OnGetConfUserList(list<string > & userlist)=0; //得到会议人员列表
    virtual CString OnGetUserName(CString userid)=0;            //得到用户名字                                      
	virtual CString GetLocalUserID()=0;			 			 //得到本地用户ID
	virtual CString GetLocalUserName()=0;          			 //得到本地用户名
	virtual bool IsInAVConference()=0;            				 //判断当前是否进入会议	
	virtual bool IsMyAuditor()=0;						//旁听者   
    virtual void DlgChatNotify_OnLButtonDown(UINT nFlags, CPoint point) = 0;    //高级会议拖动支持
	virtual IAVCRecordFactory* GetConferenceRecordFactory( void ) = 0;	//得到会议录像指针
	virtual bool GetUserIsSubMeeting(CString strUserID) = 0;	//得到用户是否开子组会议
	virtual void SaveRemoteConfChatStream(char* outPacket, int nLen, bool bKeyFrame) = 0;

};

class IDlgChat
{
public:
	virtual void ReleaseConnection()=0;    //释放资源
	virtual void SetShowWindow(bool bShow)=0;    //显示窗口
	virtual void SetMyWindowPos(CWnd * pWndInsertAfter,int x,int y,int cx,int cy,UINT nFlag)=0;//设置窗口位置					
	virtual void AddUserNameToList(CString UserName,CString UserID,bool bShowMsg = true)=0;  //增加会议人员
	virtual void DelUserNamefromList(CString UserName)=0; //删除会议人员

	virtual CWnd * GetWnd()=0;					//得到窗口句柄
	virtual void MySetParent(CWnd * pWnd)=0;		//设置父窗口
	virtual bool GetWindowVisible()=0;		//检测窗口句柄是否有效
	virtual void OnGetMessage(CString SendUserName,CString RecvUserName,CString text,CString fontface,CString fontsize,CString fontcolor,CString fontEffects)=0; 
	                                             //接收用户聊天消息
	virtual void OnSystemMessage(CString title,CString text)=0;
										//接收系统消息

    virtual void EnableWindow(bool bEnable = true) = 0; //窗口可用/不可用

	virtual BOOL StartConferenceRecord(void) = 0;				// 开始会议录像
	virtual void StopConferenceRecord(void) = 0;				// 停止会议录像

	virtual void SetConferencePlayer(void) = 0;				// 会议播放器
	virtual void SetRecordContent(const std::string& strText) = 0;
	virtual void ReSetFocus()= 0;

};

AFX_EXT_CLASS IDlgChat * CreateDlgChat(IDlgChatNotify& rNotify,CWnd * pCtrlWnd);

#endif