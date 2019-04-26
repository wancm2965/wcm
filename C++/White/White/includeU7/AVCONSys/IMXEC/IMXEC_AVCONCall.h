//IMXEC_AVCONCall.h
#ifndef __IMXEC_AVCONCALL_H__
#define __IMXEC_AVCONCALL_H__

#include <IMXEC/IMXEC_Export.h>

#include <winsock2.h>


#define AVCON_CALL_ERROR_OK						0
#define AVCON_CALL_ERROR_USERID					1
#define AVCON_CALL_ERROR_PASSWORD				2
#define AVCON_CALL_ERROR_IP						3
#define AVCON_CALL_ERROR_PORT					4
#define AVCON_CALL_ERROR_NOT_CONNECTED_TO_MCU	5
#define AVCON_CALL_ERROR_USER_NOT_LOGGED_IN		6
#define AVCON_CALL_ERROR_USER_ALREADY_EXISTS	7
#define AVCON_CALL_ERROR_LICENCE				8
			
class IMXEC_API IMXEC_AVCONCall
{
public:
	IMXEC_AVCONCall(void);
	virtual~IMXEC_AVCONCall(void);
public:
	typedef enum
	{
		CDR_HANGUP_BY_LOCAL=0,
		CDR_HANGUP_BY_REMOTE,
		CDR_USER_NOT_FOUND,
		CDR_TIMEOUT,
		CDR_UNKOWN,
	}CALL_DESTROYED_REASON;

	typedef enum
	{
		VF_QCIF=0,
		VF_QVGA,
		VF_CIF,
		VF_VGA,
		VF_4CIF,
		VF_720P,
		VF_1080I,
		VF_1080P,
		VF_COUNT
	}VIDEO_FORMAT;
public:
	virtual void OnIMXEC_AVCONCallLoginOK(const char*cszUserID){};
	virtual void OnIMXEC_AVCONCallLoginFailed(const char*cszUserID,int nReason){};

	virtual void OnIMXEC_AVCONCallTrying(const char*cszUserID,const char*cszUserName){};
	virtual void OnIMXEC_AVCONCallRingOut(const char*cszUserID,const char*cszUserName){};
	virtual void OnIMXEC_AVCONCallRingIn(const char*cszUserID,const char*cszUserName){};
	virtual void OnIMXEC_AVCONCallEstablished(const char*cszUserID,const char*cszUserName){};
	virtual void OnIMXEC_AVCONCallDestroyed(const char*cszUserID,const char*cszUserName,IMXEC_AVCONCall::CALL_DESTROYED_REASON nReason){};

	virtual int Login(const char*cszUserID,const char*cszPassword,const char*cszServerIP,unsigned short nServerPort);
	virtual void Logout(void);

	static int MakeCall(const char*cszUserID);
	static int AcceptCall(const char*cszUserID);
	static void EndCall(const char*cszUserID);

	static int SetCallLocalWnd(const char*cszUserID,HWND hWnd);
	static int SetCallLocalWndPos(const char*cszUserID,int x,int y,int cx,int cy);
	static int SetCallRemoteWnd(const char*cszUserID,HWND hWnd);
	static int SetCallRemoteWndPos(const char*cszUserID,int x,int y,int cx,int cy);

	static void SetVideoDev(int nDevID,IMXEC_AVCONCall::VIDEO_FORMAT nFormat,int nBitrate,int nCbrOrVbr,int nFrameRate);
	static void SetAudioDev(int nMicDevID,int nSpkDevID,int nAEC,int nVAD,int nAGC,int nNS);

	static int GetVideoDevCount(void);
	static int GetVideoDevName(int nDevID,char*szName,int nMaxCount);
	static int GetMicDevCount(void);
	static int GetMicDevName(int nDevID,char*szName,int nMaxCount);
	static int GetSpkDevCount(void);
	static int GetSpkDevName(int nDevID,char*szName,int nMaxCount);

	static int Init(void);
	static void Terminate(void);
};

#endif
