//HPScreenRC.h
#ifndef __HPSCREEN_RC_H__
#define __HPSCREEN_RC_H__
#include <Windows.h>

#define HPSCREENRC_EXPORTS
#ifdef HPSCREENRC_EXPORTS
#define HPSCREENRC_API __declspec(dllexport)
#else
#define HPSCREENRC_API __declspec(dllimport)
#endif

#define HPSCREENRC_MESSAGE		13995
#define IDS_SENDFILEINFO		13996
#define IDS_FULLSCREEN_RESTORY	13997

/*HPSCREENRC_MESSAGE TEXT
13996 "Sending callback information file"
13997 "Not full screen mode"
50194 "Connecting to server ..."
50195 "Failed to connect to server !"
50196 "Connected"
50199 "Protocol Version error"
50203 "Wrong Password"
50214 "Connection closed (1)"
50216 "Server closed connection \n\r -The server running as application don't have permission to unlock screen. \n\r -Manual closed \n\r -Network disconnect"
50223 "Closing viewer"
50274 "Server closed connection"
*/

class HPSCREENRC_API HPScreenRC
{
public:	

	/******************************************************************************
	* Stop
	*描述：关闭控制实例
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void Stop(void);

	/******************************************************************************
	* IsStart
	*描述：是否已启动控制实例
	*输入：无
	*输出：无
	*返回值：0：未启动；1：已启动
	*****************************************************************************/
	static int IsStart(void);

	/******************************************************************************
	* Start
	*描述：创建桌面控制客户端实例
	*输入：hWndParent：父窗口句柄；workrect：基于父窗口的位置
	*输出：无
	*返回值：0：未启动；1：已启动客户端实例；2：正在处理上一个连接，稍后再试
	*****************************************************************************/
	static int Start(HWND hWndParent,RECT workrect,const char* host,HWND hWndCallBackMessage=NULL,const char* password="avcon",short port=4228);

	/******************************************************************************
	* SizeWindow
	*描述：设置控制窗口位置
	*输入：workrect：基于父窗口的位置
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SizeWindow(RECT workrect);
		
	/******************************************************************************
	* TranslateScreenRCMsg
	*描述：捕获桌面控制接收的快捷键
	*输入：pMsg		-主进程消息
	*输出：无
	*返回值：成功返回0，失败返回-1
	*请在主应用程序的CWinApp::PreTranslateMessage(MSG* pMsg)中调用，比如：
	BOOL CScreenRXTestApp::PreTranslateMessage(MSG* pMsg)
	{
		TranslateScreenRCMsg(pMsg);
		return CWinApp::PreTranslateMessage(pMsg);
	}
	*****************************************************************************/
	static int TranslateScreenRCMsg(MSG *pMsg);

	/******************************************************************************
	* SendCAD
	*描述：Send CTRL+ALT+DEL to host
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SendCAD(void);

	/******************************************************************************
	* SendCMD
	*描述：Send CTRL+ALT+DEL to host
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void SendCMD(void);
	
	/******************************************************************************
	* RequestRefresh
	*描述：请求刷新
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void RequestRefresh(void);
	
	/******************************************************************************
	* FullScreen
	*描述：全屏控制窗口
	*输入：0  非全屏，其他全屏
	*输出：无
	*返回值：0 成功   -1 失败
	*****************************************************************************/
	static int SetFullScreen(int nFullScreen);

	/******************************************************************************
	* OpenFileTansfer
	*描述：传输文件
	*输入：文件路径
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void OpenFileTansfer();

	/******************************************************************************
	* IsOpenFileTransfer
	*描述：取消传输文件
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	static BOOL IsOpenFileTransfer();

	/******************************************************************************
	* ViewOnly
	*描述：桌面控制权限
	*输入：bViewOnly   false:有控制权限  true:无控制权限
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void ViewOnly(BOOL bViewOnly);

	/******************************************************************************
	* SendCtrlEsc
	*描述：发送Ctrl-Esc组合键
	*输入：无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SendCtrlEsc(void);
	
	/******************************************************************************
	* SetUse8Bit
	*描述：设置是否使用8位色
	*输入：nUse8Bit		-是否使用8位色【0：不使用8位色，非0：使用8位色】
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int SetUse8Bit(int nUse8Bit);
};

//桌面控制接收类
#define HPSCREENSERVER_EXPORTS
#ifdef HPSCREENSERVER_EXPORTS
#define HPSCREENSERVER_API __declspec(dllexport)
#else
#define HPSCREENSERVER_API __declspec(dllimport)
#endif

#define HPREMOTE_MESSAGE	10001		//Remote server call back info    ID
#define HPDISCONNECTION		0			//Remote server disconnect client
#define HPCONNECT			1			//Remote server connect
#define HPFILETRANSFER		2			//Remote server filetransfer finish

class HPSCREENSERVER_API HPScreenServer
{
public:
	/******************************************************************************
	* StartServer
	*描述：启动
	*输入：hwnd: 抛消息句柄   strCmdline :命令行   -install  启动后台服务模式，，-uninstall   停止后台服务
	*输出：无
	*返回值：无命令行情况下   1:成功  0：不成功
	*****************************************************************************/
	static int StartServer(HWND hwnd,char* szCmdline="");

	/******************************************************************************
	* StopServer
	*描述：关闭
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void StopServer();

	/******************************************************************************
	* Stop
	*描述：关闭远程接收连接
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void Stop(void);

	/******************************************************************************
	* Start
	*描述：开启远程接受所有连接
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	static void Start();
};



#endif

