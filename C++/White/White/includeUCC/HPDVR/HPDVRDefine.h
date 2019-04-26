/************************************************************************/
/* HPDVRDefine.h                                                        */
/************************************************************************/
#pragma once

/*第三方设备*/
/*配置类型*/
#define HP3TH_PARAM_CHANNEL			0x0001	/*通道参数*/
#define HP3TH_PARAM_CHNTITLE		0x0002	/*标题参数*/
#define HP3TH_PARAM_V_MOTION		0x0003	/*侦测参数*/
#define HP3TH_PARAM_V_COVER			0x0004	/*遮掩参数*/
#define HP3TH_PARAM_V_BLOCK			0x0005	/*遮挡参数*/
#define HP3TH_PARAM_V_LOST			0x0006	/*丢失参数*/
#define HP3TH_PARAM_PTZCTRL			0x0007	/*云台参数*/

/*配置*/
#define HP3CMD_GETPARAM				0x00000000	/*获取参数*/
#define HP3CMD_SETPARAM				0x00000001  /*设置参数*/

/*设备*/
#define HP3CMD_INIT					0x00001000	/*设备初始化*/		
#define HP3CMD_EXIT					0x00001001	/*设备反初始化*/
#define HP3CMD_STARTLISTEN			0x00001002	/*开始侦听*/	
#define HP3CMD_STOPLISTEN			0x00001003	/*停止侦听*/
#define HP3CMD_SETCALLBACK			0x00001004	/*设置回调*/
#define HP3CMD_LOGIN				0x00001005	/*登录设备*/
#define HP3CMD_LOGOUT				0x00001006	/*注销设备*/
#define HP3CMD_REBOOT				0x00001007  /*重启设备*/
#define HP3CMD_REMOTECLICKKEY		0x00001008	/*远程按键*/
#define HP3CMD_GETSDKVERSION		0x00001009	/*设备版本*/
#define HP3CMD_GETLASTERROR			0x00001010	/*错误码*/

/*通道*/
#define HP3CMD_REALPLAY				0x00002000	/*实时预览*/
#define HP3CMD_STOPPLAY				0x00002001	/*停止预览*/	
#define HP3CMD_AUDIODATA			0x00002002	/*音频数据*/	
#define HP3CMD_VIDEODATA			0x00002003	/*视频数据*/
#define HP3CMD_STARTVOICE			0x00002004	/*开始对讲*/
#define HP3CMD_STOPVOICE			0x00002005	/*停止对讲*/
#define HP3CMD_VOICEDATA			0x00002006	/*对讲数据*/
#define HP3CMD_SWITCHSTREAM			0x00002007	/*切换流*/
#define HP3CMD_CAPTUREKEYFRAME		0x00002008	/*关键帧*/
#define HP3CMD_CAPTUREPICTURE		0x00002009	/*抓拍*/
#define HP3CMD_OPENALARMCHANNEL		0x00002010	/*打开报警*/	
#define HP3CMD_CLOSEALARMCHANNEL    0x00002011	/*停止报警*/

/*视频*/
#define HP3CMD_STARTVIDEOMOTION		0x00003000	/*启动侦测*/
#define HP3CMD_STOPVIDEOMOTION		0x00003001	/*停止侦测*/
#define HP3CMD_STARTVIDEOCOVER		0x00003002	/*启动遮掩*/
#define HP3CMD_STOPVIDEOCOVER		0x00003003	/*停止遮掩*/
#define HP3CMD_STARTVIDEOBLOCK		0x00003004	/*启动遮挡*/
#define HP3CMD_STOPVIDEOBLOCK		0x00003005	/*停止遮挡*/
#define HP3CMD_STARTVIDEOLOST		0x00003006	/*侦测丢失*/
#define HP3CMD_STOPVIDEOLOST		0x00003007	/*忽略丢失*/

/*云台*/
#define HP3CMD_PTZCONTROL			0x00004000	/*云台控制*/

/*状态*/
#define HP3CMD_STATUSCHANGED		0x00005000	/*状态改变*/





