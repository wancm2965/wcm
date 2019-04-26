#ifndef __IAVFONECLIENT_H__
#define __IAVFONECLIENT_H__


#include <string>

#include "../include/KXMPP.h"
#include "../include/IAVUserPlayer.h"
#include "../AVCONEC/AVCONEC.h"
#include "../AVCONEC/libHPAudioMixer/libHPAudioMixer.h"

typedef enum {
	AVFONE_CALL_PC2PHONE=0,
	AVFONE_CALL_PC2PC
}AVFONE_CALL_TYPE;
#define AVFONE_RTS_CALL	AVFONE_CALL_PC2PC


typedef enum {
	AVFONE_CALL_STATE_IDLE=0,
	AVFONE_CALL_STATE_RING_OUT,
	AVFONE_CALL_STATE_RING_IN,
	AVFONE_CALL_STATE_TALKING,
	AVFONE_CALL_STATE_END
}AVFONE_CALL_STATE;


class IAVFoneCallNotify
{
public:
	IAVFoneCallNotify(void){};
	virtual~IAVFoneCallNotify(void){};
public:
	virtual void OnIAVFoneCallNotifyRingOut(void)=0;
	virtual void OnIAVFoneCallNotifyRingIn(void)=0;
	virtual void OnIAVFoneCallNotifyEstablished(void)=0;
	virtual void OnIAVFoneCallNotifyDestroyed(const char*cszUserID)=0;
	virtual void OnIAVFoneCallNotifyAudioMaxVolume(short sAudioMaxVolume)=0;
	virtual void OnIAVFoneCallNotifyVideoChanged(bool bHasVideo)=0;
	virtual void OnIAVFoneCallNotifyBitrate(unsigned long ulAudioBitrate,unsigned long ulAudioLostPacket,unsigned long ulVideoBitrate,unsigned long ulVideoLostPacket)=0;
	virtual void OnIAVFoneCallNotifyAudioFrame(const void*pData,int nLen)=0;
	virtual void OnIAVFoneCallNotifyVideoFrame(const void*pData,int nLen,bool bKeyFrame)=0;
};

class IAVFoneCall
{
public:
	IAVFoneCall(void){};
	virtual~IAVFoneCall(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual const char*GetUserID(void)=0;
	virtual const char*GetUserName(void)=0;
	virtual AVFONE_CALL_STATE GetCallState(void)=0;
	virtual AVFONE_CALL_TYPE GetCallType(void)=0;
	
	virtual bool AcceptCall(void)=0;
	virtual bool RejectCall(void)=0;
	virtual bool HangupCall(void)=0;

	virtual bool HasAudio(void)=0;
	virtual void Mute(bool bMute)=0;
	virtual bool IsMute(void)=0;
	virtual void SetVolume(int nVolume)=0;
	virtual int GetVolume(void)=0;
	virtual void ReceiveAudio(bool bEnable)=0;
	virtual bool IsReceivingAudio(void)=0;

	virtual bool HasVideo(void)=0;
	virtual void ShowWindow(bool bShow)=0;
	virtual bool IsWindowVisible(void)=0;
	virtual void SetWindowPos(int X,int Y,int cx,int cy)=0;
	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;
	virtual void ReceiveVideo(bool bEnable)=0; 
	virtual bool IsReceivingVideo(void)=0;
};

class IAVFoneDesktopCallNotify
{
public:
	IAVFoneDesktopCallNotify(void){};
	virtual~IAVFoneDesktopCallNotify(void){};
public:
	virtual void OnIAVFoneDesktopCallNotifyEstablished(void)=0;//Э�̽���
	virtual void OnIAVFoneDesktopCallNotifyDestroyed(void)=0;//Э������
	virtual void OnIAVFoneDesktopCallNotifyReceivedData(const void*pData,int nLen)=0;//���յ�����
	virtual void OnIAVFoneDesktopCallNotifyConnected(void)=0;//���ӽ��������Է�������
	virtual void OnIAVFoneDesktopCallNotifyDisconnected(void)=0;//���ӶϿ�
};

class IAVFoneDesktopCall
{
public:
	IAVFoneDesktopCall(void){};
	virtual~IAVFoneDesktopCall(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual const char*GetUserID(void)=0;
	virtual const char*GetUserName(void)=0;
	virtual AVFONE_CALL_STATE GetCallState(void)=0;
	virtual bool AcceptCall(void)=0;
	virtual bool RejectCall(void)=0;
	virtual bool HangupCall(void)=0;
	virtual int SendData(const void*pData,int nLen)=0;//�������ݣ�����ֵ����nLen��ʾ�ɹ�
};

class IAVFoneWBCallNotify
{
public:
	IAVFoneWBCallNotify(void){};
	virtual~IAVFoneWBCallNotify(void){};
public:
	virtual void OnIAVFoneWBCallNotifyEstablished(void)=0;//Э�̽���
	virtual void OnIAVFoneWBCallNotifyDestroyed(void)=0;//Э������
	virtual void OnIAVFoneWBCallNotifyReceivedData(const void*pData,int nLen)=0;//���յ�����
	virtual void OnIAVFoneWBCallNotifyConnected(void)=0;//���ӽ��������Է�������
	virtual void OnIAVFoneWBCallNotifyDisconnected(void)=0;//���ӶϿ�
};

class IAVFoneWBCall
{
public:
	IAVFoneWBCall(void){};
	virtual~IAVFoneWBCall(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual const char*GetUserID(void)=0;
	virtual const char*GetUserName(void)=0;
	virtual AVFONE_CALL_STATE GetCallState(void)=0;
	virtual bool AcceptCall(void)=0;
	virtual bool RejectCall(void)=0;
	virtual bool HangupCall(void)=0;
	virtual int SendData(const void*pData,int nLen)=0;//�������ݣ�����ֵ����nLen��ʾ�ɹ�
};

class IAVFoneFileCallNotify
{
public:
	IAVFoneFileCallNotify(void){};
	virtual~IAVFoneFileCallNotify(void){};
public:
	virtual void OnIAVFoneFileCallNotifyEstablished(void)=0;//Э�̽���
	virtual void OnIAVFoneFileCallNotifyDestroyed(void)=0;//Э������
	virtual void OnIAVFoneFileCallNotifyReceivedData(const void*pData,int nLen)=0;//���յ�����
	virtual void OnIAVFoneFileCallNotifyConnected(void)=0;//���ӽ��������Է�������
	virtual void OnIAVFoneFileCallNotifyDisconnected(void)=0;//���ӶϿ�
};

class IAVFoneFileCall
{
public:
	IAVFoneFileCall(void){};
	virtual~IAVFoneFileCall(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual const char*GetUserID(void)=0;
	virtual const char*GetUserName(void)=0;
	virtual AVFONE_CALL_STATE GetCallState(void)=0;
	virtual bool AcceptCall(void)=0;
	virtual bool RejectCall(void)=0;
	virtual bool HangupCall(void)=0;
	virtual int SendData(const void*pData,int nLen)=0;//�������ݣ�����ֵ����nLen��ʾ�ɹ�
};

enum {
	AVFONE_SIP_FAILED_CODE_REGISTER_FAILED=0,
	AVFONE_SIP_FAILED_CODE_REGISTER_UNAUTHENTICATED,
};

class IAVMeetingNotify
{
public:
	IAVMeetingNotify(void){};
	virtual~IAVMeetingNotify(void){};
public:
	virtual void OnIAVMeetingNotifyDestroyed(void)=0;//�������
	virtual void OnIAVMeetingNotifyUserJoin(const char*cszUserID,const char*cszUserName,unsigned short& usUserWndIndex)=0;//�û��������
	virtual void OnIAVMeetingNotifyUserLogin(const char*cszUserID,const char*cszUserName,unsigned short& usUserWndIndex)=0;//�û��������
	virtual void OnIAVMeetingNotifyUserLeave(const char*cszUserID)=0;//�û��뿪����
	virtual void OnIAVMeetingNotifyUserWndIndexChanged(const char*cszUserID,unsigned short ucUserWndIndex)=0;//�û���Ƶ����λ�øı�
	virtual void OnIAVMeetingNotifyChatMessage(const char*cszMsg,const char*cszFromUserID)=0;//��������
	virtual void OnIAVMeetingNotifyUserData(const char*cszUserData,const char*cszFromUserID)=0;//�����Զ�������
	virtual void OnIAVMeetingNotifySwitchTableWindow(unsigned short usTableWindowType)=0;//�л��󴰿�Chat,User,WB,DESKTOP
	virtual void OnIAVMeetingNotifyOpenChat(void)=0;//������
	virtual void OnIAVMeetingNotifyCloseChat(void)=0;//�ر�����
	virtual void OnIAVMeetingNotifyOpenWB(void)=0;//�򿪰װ�;
	virtual void OnIAVMeetingNotifyCloseWB(void)=0;//�رհװ�
	virtual void OnIAVMeetingNotifyOpenShareDesktop(void)=0;//�򿪹�������
	virtual void OnIAVMeetingNotifyCloseShareDesktop(void)=0;//�رչ�������
	virtual void OnIAVMeetingNotifyReceivedWBData(const char*pData,int nLen)=0;//�յ��װ�����
	virtual void OnIAVMeetingNotifyRejectedInvitation(const char*cszUserID)=0;//�û��ܾ��μӻ��������
	virtual void OnIAVMeetingNotifyCreateAVUserPlayer(const char*cszUserID)=0;//�յ��û���Ƶ��������
	virtual void OnIAVMeetingNotifyBitrate(unsigned long ulAudioBitrate,unsigned long ulAudioLostPacket,unsigned long ulVideoBitrate,unsigned long ulVideoLostPacket)=0;
	virtual bool OnIAVMeetingNotifyGetWBActive()=0;
	virtual bool OnIAVMeetingNotifyGetChatActive()=0;
	virtual int  OnIAVMeetingNotifyGetTableWindow()=0;
};

class IAVMeeting
{
public:
	virtual void ReleaseConnections(bool bCloseMeeting)=0;//�˳�����
	virtual IAVUserPlayer*CreateIAVUserPlayer(IAVUserPlayerNotify&rNotify,HWND hWnd,const char*cszUserID)=0;//�������û���Ƶ
	virtual IAVUserPlayer*CreateRingIAVUserPlayer(IAVUserPlayerNotify&rNotify,HWND hWnd,const char*cszUserID)=0;//�򿪶Է��û���Ƶ
	virtual void ChangeUserWndIndex(const char*cszUserID,unsigned short usUserWndIndex)=0;//�û���Ƶ����λ�øı�
	virtual void SendChatMessage(const char*cszMsg,const char*cszToUserID)=0;//���ͻ�������
	virtual void SendUserData(const char*cszUserData,const char*cszToUserID)=0;//�����Զ�������
	virtual void SwitchTableWindow(unsigned short usTableWindowType)=0;//�л��󴰿�Chat,User,WB,DESKTOP
	virtual bool IsMyMeeting(void)=0;//�ǲ����һ����
	virtual void EnableHost(bool bEnable)=0;//����������Ա���ƻ���
	virtual bool IsEnableHost(void)=0;//�Ƿ�����������Ա���ƻ���
	virtual bool CanHost(void)=0;//���ܷ���ƻ���
	virtual void OpenWB(void)=0;//�򿪰װ�
	virtual void CloseWB(void)=0;//�رհװ�
	virtual void OpenChat(void)=0;//������
	virtual void CloseChat(void)=0;//�ر�����
	virtual void OpenShareDesktop(void)=0;//�򿪹�������
	virtual void CloseShareDesktop(void)=0;//�رչ�������
	virtual void InviteUserJoinMeeting(const char*cszUserID,unsigned short usUserWndIndex)=0;//�����û��μӻ���
	virtual void CancelInvitation(const char*cszUserID)=0;//ȡ�������û��μӻ���
	virtual void SendWBData(const char*pData,int nLen)=0;//���Ͱװ�����
};


class IAVFoneClientNotify
{
public:
	IAVFoneClientNotify(void){};
	virtual~IAVFoneClientNotify(void){};
public:
	virtual void OnIAVFoneClientNotifyLog(const char*cszLogText)=0;

	virtual IAVFoneCall*OnIAVFoneClientNotifyCalledByUser(const char*cszUserID,AVFONE_CALL_TYPE AVFoneCallType)=0;
	virtual void OnIAVFoneClientNotifyVideoCaptureChanged(void)=0;
	virtual void OnIAVFoneClientNotifyAudioCaptureMaxVolume(short usMaxVolume)=0;
	virtual void OnIAVFoneClientNotifyBitrate(unsigned long ulAudioBitrate,unsigned long ulAudioLostPacket,unsigned long ulVideoBitrate,unsigned long ulVideoLostPacket)=0;
	virtual IAVFoneDesktopCall*OnIAVFoneClientNotifyCalledByUser(const char*cszUserID,unsigned long ulType)=0;
	virtual IAVFoneWBCall*OnIAVFoneClientNotifyCalledByUser(const char*cszUserID)=0;
	virtual IAVFoneFileCall*OnIAVFoneClientNotifyCalledByUser(const char*cszUserID,const char* cszMD5SUM,const char* cszFileName,unsigned long ulFileLength,unsigned long ulType)=0;

	virtual void OnOnIAVFoneClientNotifyFilePositionChanged(unsigned long ulSeconds){};
	virtual void OnOnIAVFoneClientNotifyFilePause(void){};
	virtual void OnOnIAVFoneClientNotifyFilePlay(void){};
	
	virtual void OnIAVFoneCallNotifyAudioFrame(const void*pData,int nLen)=0;
	virtual void OnIAVFoneCallNotifyVideoFrame(const void*pData,int nLen,bool bKeyFrame)=0;

	virtual void OnIAVFoneMeetingInvite(const char*cszHostUserID,unsigned short usUserWndIndex)=0;
	virtual void OnIAVFoneMeetingInvitationCanceled(const char*cszHostUserID)=0;

	virtual void SetStatusTipText(CString text)=0;
	virtual CString GetLangString(const CString &csAppName,const CString &csKeyName)=0;
};


class IAVFVideoCapture
{
public:
	IAVFVideoCapture(void){};
	virtual~IAVFVideoCapture(void){};
public:
	virtual bool HasVideo(void)=0;
	virtual void ShowWindow(bool bShow)=0;
	virtual bool IsWindowVisible(void)=0;
	virtual void SetWindowPos(int X,int Y,int cx,int cy)=0;
	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;

	virtual void ShowAdjustVideoWnd(void)=0;
	virtual void ShowVideoInputWnd(void)=0;
	virtual bool HasVideoInputWnd(void)=0;
	virtual void Invert(bool bInverted)=0;
	virtual bool IsInverted(void)=0;
	virtual bool IsDeinterlaceEnable(void)=0;
	virtual void EnableDeinterlace(bool bDeinterlace)=0;
	virtual bool CaptureToBitmapFile(const char*cszBitmapFile)=0;
};

class IAVFoneClient
{
public:
	IAVFoneClient(void){};
	virtual~IAVFoneClient(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual unsigned short GetTCPPort(void)=0;
	virtual unsigned short GetUDPPort(void)=0;
	virtual bool GetHTTPTUNNEL(void)=0;//80 TCP
	virtual bool GetHTTPPROXY(void)=0;//443 TCP

	virtual IAVFoneCall*CreatePutCall(IAVFoneCallNotify&rNotify,const char*cszUserID,AVFONE_CALL_TYPE AVFoneCallType,HWND hWndVideo)=0;
	virtual IAVFoneCall*CreateRingCall(IAVFoneCallNotify&rNotify,const char*cszUserID,AVFONE_CALL_TYPE AVFoneCallType,HWND hWndVideo)=0;

	virtual void PlayDTMFTone(int nEventNumber)=0;
	virtual bool OpenAudioCapture(int nAudioDevice)=0;
	virtual bool HasAudioCapture(void)=0;
	virtual void CloseAudioCapture(void)=0;

	virtual bool OpenVidepCature(HWND hWnd,int nDevice,unsigned short usFrameRate,HP_VIDEO_SIZE_TYPE AVVideoSize,HP_VIDEO_QUALITY_TYPE AVVideoQuality,bool bDeinterlace,HP_VIDEO_CODEC_TYPE AVVideoCodec,HP_MAX_BITRATE_TYPE AVVideoMaxBitrate=HP_MAX_BITRATE_UNLIMITED)=0;
	virtual void CloseVideoCapture(void)=0;
	virtual IAVFVideoCapture*GetVideoCapture(void)=0;

	virtual bool OpenMediaFile(HWND hWnd,const TCHAR*cszMediaFile,unsigned short usFrameRate,HP_VIDEO_QUALITY_TYPE AVVideoQuality,bool bDeinterlace,HP_MAX_BITRATE_TYPE AVVideoMaxBitrate=HP_MAX_BITRATE_UNLIMITED)=0;
	virtual bool IsOpenMediaFile(void)=0;
	virtual void CloseMediaFile(void)=0;

	//UPnP NAT Setting
	virtual void SetNATExternalIP(const char*cszNATExternalIP)=0;
	virtual void SetNATTCPPort(unsigned short usTCPPort)=0;
	virtual void SetNATUDPPort(unsigned short usUDPPort)=0;
	virtual IXMPPSession*CreateIXMPPSession(IXMPPSessionEvent& rXMPPSessionEvent)=0;

	virtual IAVFoneDesktopCall*CreatePutCall(IAVFoneDesktopCallNotify&rNotify,const char*cszUserID,unsigned long ulType)=0;
	virtual IAVFoneDesktopCall*CreateRingCall(IAVFoneDesktopCallNotify&rNotify,const char*cszUserID)=0;

	virtual IAVFoneWBCall*CreatePutCall(IAVFoneWBCallNotify&rNotify,const char*cszUserID)=0;
	virtual IAVFoneWBCall*CreateRingCall(IAVFoneWBCallNotify&rNotify,const char*cszUserID)=0;

	virtual IAVFoneFileCall*CreatePutCall(IAVFoneFileCallNotify&rNotify,const char*cszUserID,const char* cszMD5SUM,const char* cszFileName,unsigned long ulFileLength,unsigned long ulType)=0;
	virtual IAVFoneFileCall*CreateRingCall(IAVFoneFileCallNotify&rNotify,const char*cszUserID,const char* cszMD5SUM)=0;

	virtual IAVMeeting*CreateIAVMeetingAndHost(IAVMeetingNotify&rNotify)=0;//���������ֻ���
	virtual IAVMeeting*CreateIAVMeetingAndJoin(IAVMeetingNotify&rNotify,const char*cszHostUserID)=0;//�������μӻ���
	virtual void RejectAVMeetingInvitation(const char*cszHostUserID)=0;//�ܾ���������
};

AFX_EXT_CLASS IAVFoneClient*CreateAVFoneClient(IAVFoneClientNotify&rNotify,unsigned short usUDPPort=0,unsigned short usTCPPort=0,bool bHTTPTUNNEL=false,bool bHTTPPROXY=false);


#endif