#pragma once


//串口服务类

#include <string>
using namespace std; 

//设备类型
#define		DEV_TYPE_PTZ						0			//云台摄像机
#define		DEV_TYPE_ALARM						1			//报警
#define		DEV_TYPE_KEYBOARD					2			//多功能键盘

// 通信回调接口
class ICommunicateNotify
{
public:
	ICommunicateNotify(){};
	virtual ~ICommunicateNotify(){};

public:
	//virtual void ReciveKXMLPacket(KXMLPacket&t) = 0;		//接收通信包

};

//处理云台控制类
class IAVVideoControlCommandProcessor
{
public:
	IAVVideoControlCommandProcessor(){};
	virtual~IAVVideoControlCommandProcessor(){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual void OnDeliver(char*pData,int nLen)=0;

};

// 主机IO报警回调接口
class IAlarmIONotify
{
public:
	IAlarmIONotify(){};
	virtual ~IAlarmIONotify(){};

public:
	//----------------------------------
	//函数功能：通知报警相关信息
	//参数说明：nComPort:串口号 DefenceNo:防区号 Alarm: true 报警 false 停止报警
	//返回值  ：无
	//----------------------------------
	virtual void OnIONotify_InputAlarm(int nComPort,int DefenceNo,  bool Alarm) = 0;

};

//全功能键盘回调接口
class IKeyboardNotify
{

public:
	IKeyboardNotify(void){};
	virtual ~IKeyboardNotify(void){};

public:
	//----------------------------------
	//函数功能：流&屏绑定 一个通道绑定到一个屏幕 
	//参数说明：Monitor:屏幕 Camera:通道摄像机
	//返回值  ：无
	//----------------------------------
	virtual bool KeyboardBind(int Monitor, int Camera)=0;

	//----------------------------------
	//函数功能：触发云台控制 
	//参数说明：cmd:云台控制命令 Param: -1:停止  其它:控制
	//返回值  ：无
	//----------------------------------

	virtual void KeyboardPtzCtrl(int Cmd, int Param)=0;	

};

//云台控制通知消息类
class IPtzCtrlNotify
{

public:
	IPtzCtrlNotify(void){};
	virtual ~IPtzCtrlNotify(void){};

public:
	//----------------------------------
	//函数功能：启动指定的串口，从数据库中读取串口参数
	//参数说明：nComPort:串口号
	//返回值  ：无
	//----------------------------------
	virtual bool OnOpenCom(int nComPort)=0;

	//----------------------------------
	//函数功能：关闭指定的串口，保存当前串口参数，从数据库中读取 
	//参数说明：nComPort:串口号
	//返回值  ：无
	//----------------------------------
	virtual int OnCloseCom(int nComPort)=0;

};


//外部接口类
class IComServ
{


public:
	IComServ(void){};
	virtual ~IComServ(void){};

	//接口内部函数
public:
	//----------------------------------
	//函数功能：设置串口参数	保存到数据库
	//参数说明：nComport:端口号 nBaud:波特率 nProve:校验位 nDataBit;数据位 nStopBit:停止位
	//			nType:设备类型   此串口只能接一个接收设备，如：报警主机 多功能键盘
	//返回值  ：无
	//----------------------------------
	virtual void	SetComParam(int nComport,int nBaud,int nProve,int nDataBit,int nStopBit,int nType)=0;	

	//----------------------------------
	//函数功能：启动报警串口
	//参数说明：nComport:端口号 默认值是1号串口   pAlarmNotify: 报警接口回调
	//返回值  ：true:打开成功	false:打开失败
	//----------------------------------
	virtual bool	StartAlarmIo(int nComport,IAlarmIONotify* pAlarmNotify)=0;
	virtual int		StopAlarmIo(int nComport)=0;


	//----------------------------------
	//函数功能：启动全功能键盘串口
	//参数说明：nComport:端口号 默认值是2号串口   
	//返回值  ：true:打开成功	false:打开失败
	//----------------------------------
	virtual bool	StartKeyboardIo(int nComport,IKeyboardNotify* pKeyboardNotify)=0;
	virtual int		StopKeyboardIo(int nComport)=0;



	//----------------------------------
	//函数功能：设置全功能键盘接口回调
	//参数说明：pPtzCtrlNotify:云台控制通知回调
	//返回值  ：无
	//----------------------------------
//	virtual void	SetPtzControlNotify(IPtzCtrlNotify* pPtzCtrlNotify)=0;


	//----------------------------------
	//函数功能：云台控制
	//参数说明：nComport:端口号		CameraAddress:摄像机地址0-255	nComand:命令值 
	//			nPriority:优先级	nBaud:波特率		nType:云台类型	nSpeed:转速 
	//返回值  ：无
	//----------------------------------
//	virtual void	PtzControl(int nComPort,int CameraAddress,int nSpeed, int nBaud,int nComand)=0;

	virtual void	PtzControl(int nComPort,int CameraAddress,int nComand,int nPriority,int nType,int nSpeed, int nBaud)=0;
	

	virtual void	Release()=0;		//删除创建的串口服务接口指针

};


AFX_EXT_CLASS IComServ* CreateComServ();		//创建华平串口服务接口
