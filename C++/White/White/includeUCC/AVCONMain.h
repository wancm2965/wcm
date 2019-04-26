#ifndef AVCONMAIN_H
#define AVCONMAIN_H

#include "HPMeeting.h"

enum {
	BTN_PRESET	= 1,
	BTN_WNDPROP,
};

class IDlgConMainNotify
{
public:
	virtual void OnIDlgConMainNotifyExit(bool bneedrefresh = true) = 0;
	//virtual void OnIDlgConMainNotifyIsExitSure(bool bSure) = 0;//退出时，是否处于待确认状态
	//virtual void OnIDlgConMainNotifySetExitCode(int nCode) = 0;//退出时，是否处于待确认状态
	virtual void OnSwitchNDVR(void) = 0;
	virtual void ProcessLogout(int nErrCode)=0;
	virtual void OnIDlgConMainNotifyLogout(void)=0;
};

class IDlgConMain
{
public:
	virtual void ReleaseConnections(void) = 0;
	virtual void ResetWindowRgn(int nTMID, int nWndIndex, bool bEnabled) = 0;
	virtual void Show(void) = 0;
	virtual CWnd * GetMainWindow(void) = 0;
	virtual void OnQuestionArrived() = 0;

	//added by arrow.             begin
	virtual void TreeMemberRequest_HPTM() = 0;
	virtual void TreeMemberRequest_Option() = 0;
	virtual void TreeMemberRequest_WB() = 0;
	virtual void TreeMemberRequest_ScreenShare() = 0;
	virtual void TreeMemberRequest_Discuss() = 0;
	virtual void TreeMemberRequest_MediaShare() = 0;
	virtual void TreeMemberRequest_VideoFmtPresetting() = 0;
	virtual void TreeMemberRequest_InviteMember() = 0;
	virtual void TreeMemberRequest_InviteAllMember() = 0;
	virtual void TreeMemberRequest_QuitMeeting() = 0;
	virtual void TreeMemberRequest_ReSetState(std::string strMemberID = "", UINT nType = 0, int nFlag = 0) = 0;
	virtual void AddSysMsg(CString msg) = 0;
	virtual void SetConfStatus(CString status) = 0;
	virtual void Request_Stop_Open(const std::string& sType) = 0;
	virtual void ReSetConfTitle(CString membername,CString roomname,int membersum,int onlinesum) = 0;
	virtual void OnApplyBtnStatus(int nButton, bool bEnable) = 0;
	virtual void OnUpdateMemberBaseItem() = 0;
	virtual void OnShowMainWindow(bool show,bool bFromExe) = 0;
	virtual void OnResizeMemberTree(int x, int y, int cx, int cy) = 0;
	virtual RECT GetInitially_MemberTreeRect() const = 0;
	virtual RECT GetInitially_TempletRect() const = 0;
	virtual void SetFullTemplate(bool bFull, bool bP2Pcmd=false) = 0;
	virtual void ExitMeeting()=0;
	virtual bool IsFullScreen() = 0;//是否全屏
	//added by arrow.               end
	//显示标题
	virtual void SetMeetingTileInfo(CString sTitle,CString sFontName,DWORD colorRef)=0;
	virtual void ShowMeetingTitle(bool bShow) = 0;
	//公告
	virtual void SetMeetingAfficheInfo(CString sTitle,CString sFontName,DWORD colorRef)=0;
	virtual void ShowMeetingAffiche(bool bShow) = 0;

	//遥控器
	virtual void OnRequestExit() = 0;
	virtual void OnMeetingMaxmize() = 0;//会议窗口最大化
	virtual void OnMeetingExit() = 0;//退出会议
	virtual void TMCtrlNotify(bool bIsOpen)=0;//模板控制器回调改变图标

	//码流选择ToolBar
	virtual void ShowStreamType(int iType) = 0;
	//end
	virtual void ShowFrameRateControlMode(int nMode) = 0;

	virtual void ShowMinHide() = 0;

	virtual void SetToolBarTopMost(bool bTopMost) = 0;
	//任务栏相关
	virtual void OnMeetingShowLocalAV(bool bShow) = 0;
	virtual void ProcessChangePicture(int nDevID) = 0;
	virtual void SetMicVolumeByStatus(int nVolume) = 0;
	virtual void OnBtnClickAudioIn() = 0;
	virtual void OnBtnClickAudioOut() = 0;
	virtual void OnIHPAVCaptureNotifyAudioLevel(int nAudioLevel) = 0;
	virtual BOOL IsToolbarOperateShow() = 0;		//会议操作工具条是否显示
	virtual BOOL IsToolbarAuxiliaryShow() = 0;		//会议辅助工具条是否显示
	virtual void OnMenuOperateShow() = 0;			// 操作工具条
	virtual void OnMenuAuxiliaryShow() = 0;			// 辅助工具条
	//刷新会议标题
	virtual void RefreshConfTitle(void) = 0;

	//设置会议为焦点
	virtual void SetConFocus() = 0;

	virtual void EnableToolBarChangeScreen(BOOL bEnable) = 0;
	//自动点击切屏按钮
	virtual void AutoClickChangeScreen() = 0;

	//闭音
	virtual void SetCloseAudioOut(BOOL bStatus) = 0;
	virtual void SetCloseAudioIn(BOOL bStatus) = 0;
	virtual void SendAVStatus() = 0;
};

AFX_EXT_CLASS IDlgConMain* CreateIDlgConMain(IDlgConMainNotify& rIDlgConMainNotify,IHPMeeting* pIHPMeeting);

#endif