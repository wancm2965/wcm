#pragma once
// [12/14/2009 lzy]

//参数
//串口从0开始传
//地址从0开始传
//预置点从0开始传

typedef struct tagPTZ_COMMAND_INFO
{
	int nType;/*云台类型*/
	int nComPort;/*COM口号*/
	int nSerialNumber;/*云台的编号*/
	int nSpeed;/*转速*/
	int nComand;/*命令*/
	int nPriority;/*优先级*/
	int nBaud;/*波特率*/		
}PTZ_COMMAND_INFO,*PPTZ_COMMAND_INFO;

//控制串口通知接口
class IPtzProcessorNotify
{
public:
	IPtzProcessorNotify(void){};
	virtual ~IPtzProcessorNotify(void){};

public:
	virtual bool OnOpenCom(int nComPort)=0;
	virtual bool OnCloseCom(int nComPort)=0;

};

//云台串口控制接口
class IPtzComProcessor
{
public:
	IPtzComProcessor(){};
	virtual~IPtzComProcessor(){};
public:
	virtual void ReleaseConnections(void)=0;
	//pdata 云台信息结构体
	virtual void OnDeliver(char*pData,int nLen)=0;

};

AFX_EXT_CLASS IPtzComProcessor *CreatePtzProcessor(IPtzProcessorNotify *pNotify=NULL);