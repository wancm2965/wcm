#ifndef _INTERFACE_PLUGIN_U7_COMMON_H_
#define _INTERFACE_PLUGIN_U7_COMMON_H_


//////////////////////////////////////////////////////////////////////////

////消息宏定义
#define WM_USER_DLGSET_CHANGE		(WM_USER + 100)
#define WM_RESET_PROMPT				(WM_USER + 101)
#define WM_PTZBTN_PRESS				(WM_USER + 102)
#define WM_USER_GOBACK				(WM_USER + 104)
#define WM_RESET_QROMPT				(WM_USER + 105)


//////////////////////////////////////////////////////////////////////////
///////////////////////////插件ID名称标识定义区/////////////////////////
//////////////////////////////////////////////////////////////////////////
///

//U7会议插件ID
#define PLUGIN_NAME_U7_MEETING		"Plugin_U7_Meeting"

//U7主插件ID
#define PLUGIN_NAME_U7T_MAIN		"Plugin_U7T_Main"

//UI插件ID
#define  PLUGIN_NAME_U7_COMUI		"Plugin_U7_ComUI"

//频道插件ID
#define  PLUGIN_NAME_U7_CHANNEL		"Plugin_U7T_Channel"

//1
#define PLUGIN_NAME_U7_INFORMATION	"Plugin_U7_Infomation"

//数据库记录插件ID
#define PLUGIN_NAME_U7_DBRECORD		"Plugin_U7_DBRecord"

//预置插件ID
#define PLUGIN_NAME_U7_PRESET		"Plugin_U7_Preset"

//讨论插件ID
#define PLUGIN_NAME_U7_CHAT			"Plugin_U7_Chat"

//云台设置插件ID
#define PLUGIN_NAME_U7_YUNTAISET	"Plugin_U7_YuntaiSet"

//云台控制插件ID
#define PLUGIN_NAME_U7_CONTROLPTZ	"Plugin_U7_ControlPtz"



///登陆界面插件ID
#define  PLUGIN_NAME_U7_IM_LOGIN	"Plugin_U7_IM_Login"

//IM插件ID
#define PLUGIN_NAME_U7_IM_MON		"Plugin_U7_IM_Mon"

//IM插件ID
#define PLUGIN_NAME_U7_IM_FRIEND	"Plugin_U7_IM_Friend"

//IM插件ID
#define PLUGIN_NAME_U7_IM_TALK		"Plugin_U7_IM_Talk"



//????????????
#define PLUGIN_NAME_U7_AV_CAPTUREMGR		"Plugin_U7_AV_CaptureMgr"

//全局设置插件ID
#define PLUGIN_NAME_U7_AV_GLOBALSETTING		"Plugin_U7_AV_GlobalSetting"

//????????????
#define PLUGIN_NAME_U7_AV_PLAYERMGR			"Plugin_U7_AV_PlayerMgr"

//1
#define PLUGIN_NAME_U7_AV_WNDMGR			"Plugin_U7_AV_WndMgr"



//音视频设置插件ID
#define PLUGIN_NAME_U7_MEETING_AVSET		"Plugin_U7_Meeting_AVSet"

//设备列表插件ID
#define PLUGIN_NAME_U7_MEETING_DEVICE		"Plugin_U7_Meeting_Device"

//白板插件ID
#define PLUGIN_NAME_U7_MEETING_WHITEBOARD	"Plugin_U7_Meeting_WhiteBoard"

//人员列表插件ID
#define PLUGIN_NAME_U7_MEETING_MEMBERLIST	"Plugin_U7_Meeting_MemberList"

#define PLUGIN_NAME_U7T_MEETING_QUESTION	"Plugin_U7T_Meeting_Question"
//批量操作插件ID
#define PLUGIN_NAME_U7_MEETING_BATCHOPERATE	"Plugin_U7_Meeting_BatchOperate"

//请出插件ID
#define PLUGIN_NAME_U7_MEETING_INVITEOUT	"Plugin_U7_Meeting_InviteOut"

//邀请插件ID
#define PLUGIN_NAME_U7_MEETING_INVITEIN		"Plugin_U7_Meeting_InviteIn"

//设置公告板插件ID
#define PLUGIN_NAME_U7_MEETING_SETBILLBOARD "Plugin_U7Meeting_SetBillboard"

//标题插件ID
#define PLUGIN_NAME_U7_MEETING_TITLE		"Plugin_U7Meeting_Title"

//通知插件ID
#define PLUGIN_NAME_U7_MEETING_NOTICE		"Plugin_U7Meeting_Notice"

//静音插件ID
#define PLUGIN_NAME_U7_MEETING_MUTE			"Plugin_U7Meeting_Mute"

//屏幕导航插件ID
#define PLUGIN_NAME_U7_MEETING_SCREENNAVI	"Plugin_U7Meeting_ScreenNavi" 

//模板插件ID
#define PLUGIN_NAME_U7_MEETING_SELTEMPLATE	"Plugin_U7Meeting_SelTemplate"

//视频模式插件ID
#define PLUGIN_NAME_U7_MEETING_VIDEOMODE	"Plugin_U7Meeting_VideoMode"

//窗口属性插件ID
#define PLUGIN_NAME_U7_MEETING_WNDPROPERTY	"Plugin_U7Meeting_WndProperty"

///U7会议锁定插件ID
#define PLUGIN_NAME_U7_MEETING_LOCK			"Plugin_U7Meeting_Lock"



//成员插件ID
#define PLUGIN_NAME_U7_MM_MEMBER			"Plugin_U7_MM_Member"

//房间插件ID
#define PLUGIN_NAME_U7_MM_ROOM				"Plugin_U7_MM_Room"

//功能插件ID
#define PLUGIN_NAME_U7_MM_FUNCTION			"Plugin_U7_MM_Function"

//呼叫插件ID
#define PLUGIN_NAME_U7_MM_CALL				"Plugin_U7_MM_Call"

//1	
#define PLUGIN_NAME_U7_MM_SVRREC			"Plugin_U7_MM_SvrRec"



//1
#define PLUGIN_NAME_U7_CALL_H323			"Plugin_U7_Call_H323"

//
#define PLUGIN_U7_CALL_SLIP					"Plugin_U7_Call_Sip"

//1
#define PLUGIN_NAME_U7_CFG_COMM				"Plugin_U7_Cfg_Comm"

//1
#define PLUGIN_NAME_U7_TM_MFC				"Plugin_U7_TM_MFC"

//
#define PLUGIN_NAME_U7MEETING_AVINFO		"Plugin_U7Meeting_AVInfo"


#define PLUGIN_U7C_TERMINALMESG				"Plugin_U7C_TerminalMesg"




//打开保存对话框插件ID
#define PLUGIN_NAME_U7T_OPENSAVE			"Plugin_U7T_OpenSave"

//媒体共享插件ID
#define PLUGIN_NAME_U7T_MEDIASHARE			"Plugin_U7T_MediaShare"

//文档共享菜单插件ID
#define PLUGIN_NAME_U7T_DOCSHAREMENU		"Plugin_U7T_DocShareMenu"

//文档共享插件ID
#define PLUGIN_NAME_U7T_DOCSHARE			"Plugin_U7T_DocShare"

//信息显示插件ID
#define PLUGIN_NAME_U7T_MESSAGEBOX			"Plugin_U7T_MessageBox"

//会议插件ID
#define PLUGIN_NAME_U7T_MEETING				"Plugin_U7T_Meeting"

//呼叫插件ID
#define PLUGIN_NAME_U7T_CALL				"Plugin_U7T_Call"

//诊断插件ID
#define PLUGIN_NAME_U7T_DIAGNOSIS			"Plugin_U7T_Diagnosis"

//诊断MCU插件ID
#define PLUGIN_NAME_U7T_DIAGNOSIS_MCU		"Plugin_U7T_Diagnosis_MCU"

//HPDTC插件ID
#define PLUGIN_NAME_U7T_HPDTC				"Plugin_U7T_HPDTC"

//U7T会议快速广播本地音视频插件ID
#define PLUGIN_NAME_U7T_Meeting_BoardCastAV	"Plugin_U7T_Meeting_BoardCastAVMine"

//U7T会议举手投票插件ID
#define PLUGIN_NAME_U7T_Meeting_VOTE		"Plugin_U7T_Meeting_VoteByShowHand"

//U7T会议投票插件ID
#define PLUGIN_NAME_U7T_MEETING_BALLOT		"Plugin_U7T_Meeting_Vote"

//本地视频预览插件ID
#define PLUGIN_NAME_U7T_LOCALCHANPREVIEW	"Plugin_U7T_LocalChanPreview"

//U7T会议视频流控制ID
#define PLUGIN_NAME_U7T_MEETING_STREAMCTRL  "Plugin_U7T_Meeting_StreamCtrl"

//U7T会议 "roll call" ID
#define PLUGIN_NAME_U7T_MEETING_ROLLCALL	"Plugin_U7T_Meeting_RollCall"

//视频参数插件ID
#define PLUGIN_NAME_U7T_VIDEOPARAMETER		"Plugin_U7T_VideoParameter"

//网络设置插件ID
#define PLUGIN_NAME_U7T_SET_NET				"Plugin_U7T_SET_NET"

//云台控制插件ID
#define PLUGIN_NAME_U7T_YUNTAICTRL			"Plugin_U7T_YuntaiCtrl"

//预置点设置插件ID
#define PLUGIN_NAME_U7T_PRESETPOINT			"Plugin_U7T_PreSetpoint"

//遥控插件ID
#define PLUGIN_NAME_U7_REMOTECONTROL        "Plugin_U7_RemoteControl"

//会议轮循
#define  PLUGIN_NAME_U7_MEETING_LUNXUN		"Plugin_U7_Meeting_LunXun"
//语音激励插件ID
#define PLUGIN_NAME_U7T_MEETING_SPEECHENCOURAGE	"Plugin_U7T_Meeting_SpeechEncourage"

//频道列表
#define  PLUGIN_NAME_U7T_CHANNELLIST       "Plugin_U7T_ChannelList"

//一键关闭所有打开频道列表的视频
#define  PLUGIN_NAME_U7T_CHANNELCLOSE       "Plugin_U7T_ChannelClose"

#define PLUGIN_NAME_U7T_AVMENU				"Plugin_U7T_AVMenu"

/************************************************************************/
/* 福建建行使用插件                                                     */
//发送辅流
#define  PLUGIN_U7T_MEETING_CCB_ASSISTFLOW       "Plugin_U7T_Meeting_CCB_AssistFlow"
/************************************************************************/

/************************************************************************/
/* 农发行使用插件                                                     */
//U7会议预置插件ID
#define PLUGIN_NAME_U7_MEETING_PRESET "Plugin_U7_Meeting_Preset"
/************************************************************************/

/************************************************************************/
/*文件分发                                                                 */
/************************************************************************/
#define PLUGIN_U7_HPFILESEND "Plugin_U7_HPFileSend"



//设置ICON的标识
#define	IDR_MAINFRAME			128		// 主程序图标ID
#define	IDR_MEETINGPLAYER		128		// 会议录像播放器图标ID

#endif
