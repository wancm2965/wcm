#ifndef _HPMCVIDEOCONTROL_H
#define _HPMCVIDEOCONTROL_H

typedef struct tagCONTROL_COMMAND_INFO
{
	int iSNType;//云台类型
	int iCom;//COM口号
	int iNumber;//云台的编号
	int iSpeed;//转速
	int iCmd;//命令
	int nPriority ;//优先级
}CONTROL_COMMAND_INFO,*PCONTROL_COMMAND_INFO;

class IDlgYTNotify
{
public:
	IDlgYTNotify(void){};
	virtual ~IDlgYTNotify(void){};
public:
	virtual void OnCommand(LONG lngCommand)=0;
	virtual void OnSpeed(int nSpeed)=0;
	virtual void OnSetParameter(CONTROL_COMMAND_INFO ccInfo) = 0;
};

class IDlgYT
{
public:
	IDlgYT(void){};
	virtual~IDlgYT(void){};
public:
	virtual BOOL DestroyWindow(void)=0;
	virtual void SetSpeed(int nSpeed)=0;
	virtual void GetWindowRect(LPRECT lpRect )=0; 
	virtual BOOL SetWindowPos (const CWnd* pWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags )=0; 
	virtual BOOL ShowWindow (int nCmdShow )=0; 
	virtual BOOL IsWindowVisible(void)=0;
	virtual CWnd*GetWnd(void)=0;
    virtual void EnableVideoControl(bool b1) = 0;
};

AFX_EXT_CLASS IDlgYT* CreateDlgYT(IDlgYTNotify *pNotify, int nChannelIndex, CWnd *pWndParent);

class IAVVideoCtrlComProNotify
{
public:
	IAVVideoCtrlComProNotify(void){}; 
	virtual ~IAVVideoCtrlComProNotify(void){};

public:
	virtual bool OnOpenCom(int nComPort)=0;
	virtual bool OnCloseCom(int nComPort)=0;

};

class IAVVideoControlCommandProcessor
{
public:
	IAVVideoControlCommandProcessor(){};
	virtual~IAVVideoControlCommandProcessor(){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual void OnDeliver(char*pData,int nLen)=0;

};

AFX_EXT_CLASS IAVVideoControlCommandProcessor *CreateVideoControlCommandProcessor(IAVVideoCtrlComProNotify *pNotify);

AFX_EXT_CLASS void WriteVideoControlSetting(int nChannel,int nType,int nComPort,int nSerialNumber,int nSpeed);
AFX_EXT_CLASS void ReadVideoControlSetting(int nChannel,int&nType,int&nComPort,int&nSerialNumber,int&nSpeed);
AFX_EXT_CLASS bool DoVideoControlSetting(int nChannel, CONTROL_COMMAND_INFO ccInfo, CWnd*pParent);
AFX_EXT_CLASS void ControlYunTai(int nChannelIndex, int nCmd);
AFX_EXT_CLASS void SetYunTaiSpeed(int nChannel, int nSpeed);
AFX_EXT_CLASS void SetYunTaiParameter(int nChannel, int nType, int nComPort, int nSerialNumber);
AFX_EXT_CLASS BOOL InitClient(const char *serverIP, int nChannelID);
AFX_EXT_CLASS BOOL InitServer();
AFX_EXT_CLASS void RemoveDlgYTNotifyMap(int nChannelIndex);
AFX_EXT_CLASS void DestroyClient();
AFX_EXT_CLASS void DestroyServer();
extern HINSTANCE	g_hInstance;
#endif