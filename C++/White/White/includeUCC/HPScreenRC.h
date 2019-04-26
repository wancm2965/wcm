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
	*�������رտ���ʵ��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void Stop(void);

	/******************************************************************************
	* IsStart
	*�������Ƿ�����������ʵ��
	*���룺��
	*�������
	*����ֵ��0��δ������1��������
	*****************************************************************************/
	static int IsStart(void);

	/******************************************************************************
	* Start
	*����������������ƿͻ���ʵ��
	*���룺hWndParent�������ھ����workrect�����ڸ����ڵ�λ��
	*�������
	*����ֵ��0��δ������1���������ͻ���ʵ����2�����ڴ�����һ�����ӣ��Ժ�����
	*****************************************************************************/
	static int Start(HWND hWndParent,RECT workrect,const char* host,HWND hWndCallBackMessage=NULL,const char* password="avcon",short port=4228);

	/******************************************************************************
	* SizeWindow
	*���������ÿ��ƴ���λ��
	*���룺workrect�����ڸ����ڵ�λ��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SizeWindow(RECT workrect);
		
	/******************************************************************************
	* TranslateScreenRCMsg
	*����������������ƽ��յĿ�ݼ�
	*���룺pMsg		-��������Ϣ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*������Ӧ�ó����CWinApp::PreTranslateMessage(MSG* pMsg)�е��ã����磺
	BOOL CScreenRXTestApp::PreTranslateMessage(MSG* pMsg)
	{
		TranslateScreenRCMsg(pMsg);
		return CWinApp::PreTranslateMessage(pMsg);
	}
	*****************************************************************************/
	static int TranslateScreenRCMsg(MSG *pMsg);

	/******************************************************************************
	* SendCAD
	*������Send CTRL+ALT+DEL to host
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SendCAD(void);

	/******************************************************************************
	* SendCMD
	*������Send CTRL+ALT+DEL to host
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SendCMD(void);
	
	/******************************************************************************
	* RequestRefresh
	*����������ˢ��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void RequestRefresh(void);
	
	/******************************************************************************
	* FullScreen
	*������ȫ�����ƴ���
	*���룺0  ��ȫ��������ȫ��
	*�������
	*����ֵ��0 �ɹ�   -1 ʧ��
	*****************************************************************************/
	static int SetFullScreen(int nFullScreen);

	/******************************************************************************
	* OpenFileTansfer
	*�����������ļ�
	*���룺�ļ�·��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void OpenFileTansfer();

	/******************************************************************************
	* IsOpenFileTransfer
	*������ȡ�������ļ�
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	static BOOL IsOpenFileTransfer();

	/******************************************************************************
	* ViewOnly
	*�������������Ȩ��
	*���룺bViewOnly   false:�п���Ȩ��  true:�޿���Ȩ��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void ViewOnly(BOOL bViewOnly);

	/******************************************************************************
	* SendCtrlEsc
	*����������Ctrl-Esc��ϼ�
	*���룺��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SendCtrlEsc(void);
	
	/******************************************************************************
	* SetUse8Bit
	*�����������Ƿ�ʹ��8λɫ
	*���룺nUse8Bit		-�Ƿ�ʹ��8λɫ��0����ʹ��8λɫ����0��ʹ��8λɫ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetUse8Bit(int nUse8Bit);
};

//������ƽ�����
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
	*����������
	*���룺hwnd: ����Ϣ���   strCmdline :������   -install  ������̨����ģʽ����-uninstall   ֹͣ��̨����
	*�������
	*����ֵ���������������   1:�ɹ�  0�����ɹ�
	*****************************************************************************/
	static int StartServer(HWND hwnd,char* szCmdline="");

	/******************************************************************************
	* StopServer
	*�������ر�
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void StopServer();

	/******************************************************************************
	* Stop
	*�������ر�Զ�̽�������
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void Stop(void);

	/******************************************************************************
	* Start
	*����������Զ�̽�����������
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void Start();
};



#endif

