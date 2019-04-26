#pragma once
using namespace std;
#include "KBASE.h"
#include "KSYSClient.h"
#include "StructDefine.h"
#include "KMONDefine.h"
#pragma comment(lib,"KBASE.lib")
#pragma comment(lib,"KSYSClient.lib")
#include "HPDEV_Define.h"

#include <map>
#include <string>
using namespace std;
#define TYPE_PTZDLG		1
#define TYPE_PRESET		2

/************************************************************************/
/* 云台命令                                                             */
/************************************************************************/
#define CMD_UP			101   //向上
#define CMD_STOP_UP		201   
#define CMD_DOWN		102   //向下
#define CMD_STOP_DOWN   202
#define CMD_LEFT		103	  //向左
#define CMD_STOP_LEFT	203	  
#define CMD_RIGHT		104   //向右
#define CMD_STOP_RIGHT  204
#define CMD_DN			105   //变距近
#define CMD_STOP_DN     205
#define CMD_DF			106   //变距远
#define CMD_STOP_DF     206
#define CMD_FS			107   //变焦小
#define CMD_STOP_FS		207
#define CMD_FB			108   //变焦大
#define CMD_STOP_FB     208
#define CMD_HS			109   //光圈小
#define CMD_STOP_HS		209
#define CMD_HB			110   //光圈大
#define CMD_STOP_HB     210
#define CMD_AUTO		111	  //自动	
#define CMD_STOP_AUTO	211
#define CMD_AE_ON		112		//自动曝光ON
#define CMD_AE_OFF		113		//自动曝光OFF
#define CMD_EV_SET		114		//设置曝光修正值
#define CMD_EV_SET_END	126		//设置曝光修正值结束

#define CMD_LIGHT		240		//灯光
#define CMD_WIPER		241		//雨刷
#define CMD_FAN			242		//风扇

//预置点(新)
#define CMD_PRESET		212		//设置预置点
#define CMD_CLR_PRESET	213		//清除预置点
#define CMD_GOTO_PRESET	214		//调用预置点
#define CMD_AUTO_SCAN   218		//自动巡航 == CMD_PATROL_RUN

//预置点(原来)
#define CMD_PREST_SET	301
#define CMD_PREST_CALL  401

//巡航命令:Pattern
#define CMD_PATROL_START 216
#define CMD_PATROL_STOP	217
#define CMD_PATROL_RUN	218 //停止巡航可调用上面任一个控制命令

//云台控制 给用户的接口
class HPPTZ_Control
{
public:
	//----------------------------------
	//函数功能：设置对话框位置
	//返回值  ：无
	//----------------------------------
	virtual void SetWindowPos (int x,int y,int cx,int cy)=0;

	//----------------------------------
	//函数功能：显示对话框
	//参数说明：nCmdShow: true:显示 false:隐藏
	//返回值  ：无
	//----------------------------------
	virtual void ShowWindow(bool nCmdShow) = 0;

	//----------------------------------
	//函数功能：使对话框上控件有效或者无效
	//参数说明：bEnbale: TRUE:有效 FALSE:无效 Type:目前不需要 //云台TYPE_PTZDLG 还是预置点 
	//返回值  ：无
	//----------------------------------
	virtual void SetControlEnable(BOOL bEnable,int Type=0) = 0;

	//----------------------------------
	//函数功能：销毁对应的对话框
	//----------------------------------
	virtual void Destory()=0;
	

	//----------------------------------
	//函数功能：更新通道的预置点
	//参数说明：	Preid:预置点号 
	//				Name:预置点名称 
	//返回值  ：无
	//----------------------------------
	virtual void UpdatePreset(map<int,CString> info_map) = 0;


	//----------------------------------
	//函数功能：云台控制
	//参数说明：cmd: 控制命令 chnid:通道号
	//返回值  ：无
	//----------------------------------
	virtual void Control(int cmd, int chnid) = 0;

	//设置速度
	virtual void SetSpeed(int speed) = 0;

	//// [3/3/2009 lzy]
	////设置语言路径
	//virtual void  SetLanguageIniPath(CString strIniFullPath)=0;

	// [5/19/2009 lzy]
	// nCmdShow SW_SHOW SW_HIDE
	virtual void ShowToolbar(int nCmdShow)=0;		//显示浮动窗口
	virtual void SetToolbarPos(int x,int y,int nReserved=0)=0;		//nReserved=0 显示title 1:p窗口控制 2:置顶
	virtual BOOL GetToolbarShow()=0;				//获取是否显示
	virtual void GetToolbarRect(CRect& rc)=0;	
	virtual void UpdateToolbarTitle(CString strTitle)=0;			//更新通道名称
	virtual void UpdateToolbarPrePointName(std::map<int,CString> & mapName)=0;		//更新预置点名称
	virtual void NotifySelectChann(string ChannID)=0;		//通知选择指定通道
	virtual void SetMixVideo(int WindowNum=0)=0; //设置混合视频
	virtual void NotifyMoveEnd()=0;	//通知浮动云台p窗口移动结束

};

//通知接口类
class HPPTZ_ControlNotify
{
public:

	//----------------------------------
	//函数功能：通知用户云台的命令
	//参数说明：cmd:云台命令
	//返回值  ：无
	//----------------------------------
	virtual void HPPTZ_ControlNotifyCall(int cmd,int speed,int subtype=0) = 0;

	//巡航
	virtual void HPPTZ_ControlNotifyCruise(bool bStart,int CruiseNo)=0;

	//轨迹
	virtual void HPPTZ_ControlNotifyTrack(bool bStart,int TrackNo)=0;

	virtual void HPPTZ_SetSpeed(int speed)=0;	//通知更改速度
	virtual void HPPTZ_NotifyState(BOOL bShow)=0;	//通知浮动云台状态改变 显示或者退出
	virtual void HPPTZ_NotifyToolbarChange()=0;		//通知浮动云台扩展获取折叠

};


//----------------------------------
//函数功能：创建云台控制对话框
//参数说明：pParentWnd:父窗口指针 pNotify:回调接口指针
//返回值  ：控制云台接口指针
//----------------------------------

AFX_EXT_CLASS  HPPTZ_Control* CreateHPPTZ(CWnd* pParentWnd ,HPPTZ_ControlNotify* pNotify);



#define CMD_SET_PTZ_PARAM					"cmd_set_ptz_param"/*设云台参数*/
#define CMD_GET_PTZ_PARAM					"cmd_get_ptz_param"/*取云台参数*/
#define CMD_ALARM_PARAM_SET					"cmd_set_alarm_param"/*设报警参数*/
#define CMD_ALARM_PARAM_GET					"cmd_get_alarm_param"/*取报警参数*/
#define CMD_ENCODER_RECORD_SET				"cmd_set_record_param"/*设录像参数*/
#define CMD_ENCODER_RECORD_GET				"cmd_get_record_param"/*取录像参数*/
#define CMD_VIDEOPLAN_SET					"cmd_set_record_schedule"/*设置录像计划表*/
#define CMD_VIDEOPLAN_GET					"cmd_get_record_schedule"/*获取录像计划表*/
#define CMD_CHANNEL_GET_MOVE_DETECT			"cmd_get_motion_detect"/*取移动侦测参数*/
#define CMD_CHANNEL_SET_MOVE_DETECT 		 "cmd_set_motion_detect"/*设移动侦测参数*/
#define CMD_HPDEV_REBOOT					"cmd_reboot"	//重启华平设备
#define CMD_VIDEOPLAN_START					 "cmd_start_defence"
#define CMD_VIDEOPLAN_STOP					"cmd_stop_defence"
#define CMD_GET_DEVCFGINFO					"cmd_get_devcfginfo"		//得到设备参数
#define CMD_SET_DEVCFGINFO					"cmd_set_devcfginfo"		//设置设备参数
#define CMD_REMOTE_CHANNEL_GET_AV_WIZARD	"cmd_get_channel_param"		//得到通道参数
#define CMD_REMOTE_CHANNEL_SET_AV_WIZARD	"cmd_set_channel_param"
#define CMD_LOCAL_IPSETTING					"cmd_locip_set"
#define CMD_AVCON_SERVER_INFO				"cmd_svr_address"
#define CMD_DOWNLOAD_SERVERLIST				"cmd_down_svrlst"

#define CMD_FROMID							"FROMID"
#define CMD_TOID							"TOID"

//HHS命令添加
#define REMOTE_GET_DVRCONFIG				"REMOTE_GET_DVRCONFIG"
#define REMOTE_SET_DVRCONFIG				"REMOTE_SET_DVRCONFIG"	
#define USERCMD								"USERCMD"
#define REMOTE_SHOW						1	//远程对话框正在显示 
#define REMOTE_HIDE						0	//远程对话框没有显示

//把远程配置信息输出到外面
class IRemoteCfgOUT
{
public:
	//sCmdData:相关的命令 如：得到云台或者设置云台信息
	virtual	void SendData(KCmdUserDataPacket& sCmdData)=0;
};


//从其它地方把信息输入到远程配置
class IRemoteCfgIN
{
public:
	//从华平服务器传过来的相关信息 如：录像 报警
	virtual	void ReceiveData(KCmdUserDataPacket& sCmdData)=0;
	virtual	void DeleteDlg()=0;		//异常退出时可以调用

};

// [4/10/2009 lzy]
//修改为new出来
//远程配置对话框接口
class IRemoteCfg
{
public:
	virtual void ShowWindow(int nCmdShow)=0;
	virtual void Destory()=0;
	virtual	void UpdateInfo(MapMonDomainGroupItem& GroupItemInfo,MapMonDomainChannelItem& ChannelItemInfo,
		CString RootName,MMS_MCU* pmcu)=0;	//更新远程配置要初始化的信息
	virtual	void ReceiveData(KCmdUserDataPacket& sCmdData)=0; //通知华平更新数据
	virtual void NotifyUpdateChanState(const CHANNELINFO &chn_info)=0;//通知更新通道状态
	virtual int GetRemoteState()=0;		//得到远程配置状态 返回值:REMOTE_SHOW REMOTE_HIDE
	virtual void NotifyUpdateDev(string& name,string& groupid )=0;	//更改设备
	virtual void NotifyUpdateChann(string& name,string& channid )=0;	//更改通道 

};
//----------------------------------
//函数功能：创建创建远程配置对话框
//参数说明：pParentWnd:父窗口指针 RootName:根结点名称
//返回值  ：远程配置接口指针
//----------------------------------

AFX_EXT_CLASS  void ShowRemoteConfigDlg(CWnd* pParentWnd,MapMonGroupItem& GroupItemInfo,
										MapMonChannelItem& ChannelItemInfo,CString RootName,
										IRemoteCfgOUT* pOUT,IRemoteCfgIN** pIN,MMS_MCU* pmcu);


AFX_EXT_CLASS  IRemoteCfg* CreateRemoteConfigDlg(CWnd* pParentWnd ,IRemoteCfgOUT* pNotify);

//矩阵控制器设置对讲框接口
class IMatrixCtrl
{
public:
	virtual void ShowWindow(bool nCmdShow) = 0;
	virtual void Destory()=0;
	virtual void SetCurrenMatrixCtrl(const string& strMatrixDevID)=0;
	//更新矩阵控制器数据
	virtual	void UpdateMatrixDevInfo(MapMonDomainGroupItem& GroupItemInfo,MapMonDomainChannelItem& ChannelItemInfo)=0;
	virtual void OnChannelInfo(std::string strDevID,std::string strGroupName,const CHANNELINFO &chn_info)=0;
	virtual void OnMatrixCtrlRcvP2PData(const std::string&strNodeID,KCmdPacket& packet)=0;
	virtual void OnUpdateChannelName(const std::string& strChannelID,const std::string& strName, unsigned long ulGroupID)=0;
};

class HP_MatrixCtrlNotify
{
public:
	virtual void ChangeMatrix(const string& strMatrixCtrl,const std::string& strChanID,int nInput,const std::string strInputName,const std::string strMatrixID){};
	virtual bool GetMatrixInfo(const string& strMatrixCtrl,const string& strChanID,const string& strUserData){ return false ;};
};

AFX_EXT_CLASS  IMatrixCtrl* CreateMatrixCtrl(CWnd* pParentWnd ,HP_MatrixCtrlNotify* pNotify);