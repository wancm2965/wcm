//AVCONRTC.h
#ifndef __AVCONRTC_H__
#define __AVCONRTC_H__

#ifdef AVCONRTC_EXPORTS
#define AVCONRTC_API __declspec(dllexport)
#elif AVCONRTC_DLL
#define AVCONRTC_API __declspec(dllimport)
#else
#define AVCONRTC_API
#endif


#define AVCONRTC_ERROR_OK				0x00000000
#define AVCONRTC_ERROR_USER_ID			0x80000001
#define AVCONRTC_ERROR_PASSWORD			0x80000002
#define AVCONRTC_ERROR_SERVER			0x80000003
#define AVCONRTC_ERROR_LOGIN			0x80000004
#define AVCONRTC_ERROR_LICENCE			0x80000005
#define AVCONRTC_ERROR_PARAM			0x80000006
#define AVCONRTC_ERROR_ROOM				0x80000007
#define AVCONRTC_ERROR_CONNECTION		0x80000008
#define AVCONRTC_ERROR_CHANNEL			0x80000009

class AVCONRTC_API AVCONRTC
{
public:
	AVCONRTC(void);
	virtual~AVCONRTC(void);
public:
	static void Initialize(void);
	static void UnInitialize(void);
public:
	/*
	功能：设置用户
	输入：	cszUserID	用户帐号
			cszPassword	用户密码
	输出：无
	返回：无
	*/
	virtual void SetUser(const char*cszUserID,const char*cszPassword);
	
	/*
	功能：设置服务器
	输入：	cszIP	服务器地址
			nPort	服务器端口
	输出：无
	返回：无
	*/
	virtual void SetServer(const char*cszIP,unsigned short nPort);

	/*
	功能：设置录像路径
	输入：	cszPath	录像路径
	输出：无
	返回：无
	*/
	virtual void SetRecordPath(const char*cszPath);

	/*
	功能：设置拍照路径
	输入：	cszPath	拍照路径
	输出：无
	返回：无
	*/
	virtual void SetSnapshotPath(const char*cszPath);
	
	/*
	功能：设置音频设备
	输入：	nMicDevID	录音设备ID【缺省为-1，默认录音设备】
			nSpkDevID	放音设备ID【缺省为-1，默认录音设备】
	输出：无
	返回：无
	*/
	virtual void SetAudioDev(int nMicDevID=-1,int nSpkDevID=-1);
	
	/*
	功能：设置视频设备
	输入：	nDevID		视频采集设备ID【缺省为0，第一个采集设备】
			nWidth		视频宽度【176X144,320X240,352X288,640X480,704X576】
			nHeight		视频高度【1280X720,1920X1080】
			nFrameRate	视频帧率【1，5,10,15,30】
			nBitrate	视频码流【单位为Kbps,比如:32,64,128,256,384,512,768,1024】
	输出：无
	返回：无
	*/
	virtual void SetVideoDev(int nDevID=0,int nWidth=640,int nHeight=480,int nFrameRate=30,int nBitrate=512);
	
	/*
	功能：登录
	输入：无
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int Login(void);

	/*
	功能：注销
	输入：无
	输出：无
	返回：无
	*/
	virtual void Logout(void);

	/*
	功能：进入房间
	输入：	cszRoomID	房间ID
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int JoinRoom(const char*cszRoomID);
	
	/*
	功能：离开房间
	输入：	cszRoomID	房间ID
	输出：无
	返回：无
	*/
	virtual void LeaveRoom(const char*cszRoomID);
	
	/*
	功能：开始对特定用户对讲
	输入：	cszUserID	用户帐号
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int StartPushToTalkToUser(const char*cszUserID);
	
	/*
	功能：停止对特定用户对讲
	输入：	cszUserID	用户帐号
	输出：无
	返回：无
	*/
	virtual void StopPushToTalkToUser(const char*cszUserID);
	
	/*
	功能：开始对特定房间说话
	输入：	cszRoomID	房间ID
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int StartPushToTalkToRoom(const char*cszRoomID);
	
	/*
	功能：停止对特定房间说话
	输入：	cszRoomID	房间ID
	输出：无
	返回：无
	*/
	virtual void StopPushToTalkToRoom(const char*cszRoomID);

	/*
	功能：打开本地设备
	输入：	hParentWnd	显示视频的父窗口句柄
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int OpenLocalDev(HWND hParentWnd);
	
	/*
	功能：关闭本地设备
	输入：无
	输出：无
	返回：无
	*/
	virtual void CloseLocalDev(void);
	
	/*
	功能：设置本地视频窗口显示位置
	输入：	x	窗口的左边位置
			y	窗口的上边位置
			cx	窗口的宽度
			cy	窗口的高度
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int SetLocalDevWndPos(int x,int y,int cx,int cy);
	
	/*
	功能：打开用户设备
	输入：	cszUserID	用户帐号
			hParentWnd	显示视频的父窗口句柄
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int OpenUserDev(const char*cszUserID,HWND hParentWnd);
	
	/*
	功能：关闭用户设备
	输入：	cszUserID	用户帐号
	输出：无
	返回：无
	*/
	virtual void CloseUserDev(const char*cszUserID);
	
	/*
	功能：设置用户视频窗口显示位置
	输入：	cszUserID	用户帐号
			x	窗口的左边位置
			y	窗口的上边位置
			cx	窗口的宽度
			cy	窗口的高度
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int SetUserDevWndPos(const char*cszUserID,int x,int y,int cx,int cy);
	
	/*
	功能：本地视频拍照
	输入：无
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int Snapshot(void);
	
	/*
	功能：对特定用户远程视频拍照【图片保存在用户本地】
	输入：	cszUserID	用户帐号
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int Snapshot(const char*cszUserID);
	
	/*
	功能：开始本地视频录像
	输入：无
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int StartRecording(void);
	
	/*
	功能：停止本地视频录像
	输入：无
	输出：无
	返回：无
	*/
	virtual void StopRecording(void);
	
	/*
	功能：开始对特定用户视频录像【录像保存在用户本地】
	输入：	cszUserID	用户帐号
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int StartRecording(const char*cszUserID);
	
	/*
	功能：停止对特定用户视频录像
	输入：	cszUserID	用户帐号
	输出：无
	返回：无
	*/
	virtual void StopRecording(const char*cszUserID);
	
	/*
	功能：发送数据给特定用户
	输入：	cszUserID	用户帐号
			cszData		字符串数据指针
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int SendDataToUser(const char*cszUserID,const char*cszData);
	
	/*
	功能：发送数据给特定房间
	输入：	cszRoomID	房间号
			cszData		字符串数据指针
	输出：无
	返回：成功返回0，否则返回错误码
	*/
	virtual int SendDataToRoom(const char*cszRoomID,const char*cszData);

	/*
	功能：服务器正在连接事件
	输入：无
	输出：无
	返回：无
	*/
	virtual void OnServerConnecting(void);

	/*
	功能：服务器已连接事件
	输入：无
	输出：无
	返回：无
	*/
	virtual void OnServerConnnected(void);

	/*
	功能：服务器连接断开事件
	输入：无
	输出：无
	返回：无
	*/
	virtual void OnServerDisconnected(void);

	/*
	功能：登录成功事件
	输入：无
	输出：无
	返回：无
	*/
	virtual void OnLogin(void);

	/*
	功能：注销事件【包括登录失败事件】
	输入：	nCode			错误码
			cszDescription	错误描述
	输出：无
	返回：无
	*/
	virtual void OnLogout(unsigned long nCode,const char*cszDescription);

	/*
	功能：用户进入房间事件【包括自己进入房间的事件】
	输入：	cszRoomID		房间号
			cszUserID		用户帐号
	输出：无
	返回：无
	*/
	virtual void OnUserJoinRoom(const char*cszRoomID,const char*cszUserID);

	/*
	功能：用户离开房间事件
	输入：	cszRoomID		房间号
			cszUserID		用户帐号
	输出：无
	返回：无
	*/
	virtual void OnUserLeaveRoom(const char*cszRoomID,const char*cszUserID);
	
	/*
	功能：用户开始对讲事件
	输入：	cszUserID		用户帐号
	输出：无
	返回：无
	*/
	virtual void OnStartPushToTalkFromUser(const char*cszUserID);
	
	/*
	功能：用户停止对讲事件
	输入：	cszUserID		用户帐号
	输出：无
	返回：无
	*/
	virtual void OnStopPushToTalkFromUser(const char*cszUserID);
	
	/*
	功能：房间开始说话事件
	输入：	cszRoomID		房间号码
			cszUserID		用户帐号
	输出：无
	返回：无
	*/
	virtual void OnStartPushToTalkFromRoom(const char*cszRoomID,const char*cszUserID);
	
	/*
	功能：房间停止说话事件
	输入：	cszRoomID		房间号码
			cszUserID		用户帐号
	输出：无
	返回：无
	*/
	virtual void OnStopPushToTalkFromRoom(const char*cszRoomID,const char*cszUserID);

	/*
	功能：开始录像事件
	输入：	cszPathName		录像的全路径
	输出：无
	返回：无
	*/
	virtual void OnStartRecording(const char*cszPathName);

	/*
	功能：停止录像事件
	输入：	cszPathName		录像的全路径
	输出：无
	返回：无
	*/
	virtual void OnStopRecording(const char*cszPathName);

	/*
	功能：拍照事件
	输入：	cszPathName		图像照片的全路径
	输出：无
	返回：无
	*/
	virtual void OnSnapshot(const char*cszPathName);
	
	/*
	功能：收到用户发送过来的数据事件
	输入：	cszUserID	用户帐号
			pData		数据指针
			nLen		数据长度
	输出：无
	返回：无
	*/
	virtual void OnRcvdDataFromUser(const char*cszUserID,const char*pData,int nLen);
	
	/*
	功能：收到会议发送过来的数据事件
	输入：	cszUserID	用户帐号
			pData		数据指针
			nLen		数据长度
	输出：无
	返回：无
	*/
	virtual void OnRcvdDataFromRoom(const char*cszRoomID,const char*cszUserID,const char*pData,int nLen);
};



#endif