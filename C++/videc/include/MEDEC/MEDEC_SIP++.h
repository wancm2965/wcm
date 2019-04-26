//MEDEC_SIP++.h
#ifndef __MEDEC_SIPPP_H__
#define __MEDEC_SIPPP_H__

#include <MEDEC/MEDEC_Export.h>

class MEDEC_API MEDEC_SIP
{
public:
	MEDEC_SIP(void);
	virtual~MEDEC_SIP(void);
public:
	virtual  int Start(void);
	virtual  void Stop(void); 

	static  int SetListenPort(unsigned short nPort);
	static  int GetNumOfCodecs(void);
	static  int GetCodec(int nIndex, char* szCodec);
	static  int SetCodecPriority(char* szName, int nIndex);
	// call API
	static  int RegisterAccount(char* szUri, char* szRegUri, char* szDomain, char* szUserName, char* szPassword, char* szProxy, bool bDefault);
	static  int MakeCall(int nAccountID, const char* szUri); 
	static  int ReleaseCall(int nCallID); 
	static  int AnswerCall(int nCallID, int nCode);
	static  int HoldCall(int nCallID);
	static  int RetrieveCall(int nCallID);
	static  int XferCall(int nCallID, char* szUri);
	static  int XferCallWithReplaces(int nCallID, int nDstSession);
	static  int ServiceReq(int nCallID, int mServiceCode, const char* szDestUri);
	static  int DialDtmf(int nCallID, char* szDigits, int nMode);
	static  int RemoveAccounts(void);
	static  int SendInfo(int nCallID, char* szContent);
	static  int GetCurrentCodec(int nCallID, char* szCodec);
	static  int MakeConference(int nCallID);
	static  int SendCallMessage(int nCallID, char* szMessage);
	// IM & Presence api
	static  int AddBuddy(char* szUri, bool bSubscribe);
	static  int RemoveBuddy(int nBuddyID);
	static  int SendMsg(int nAccountID, char* szUri, char* szMessage);
	static  int SetStatus(int nAccountID, int nPresenceState);

	static  int GetPlayDeviceCount(void);
	static  int GetPlayDeviceName(int nIndex,char* szDeviceName);
	static  int GetRecordingDeviceCount(void);
	static  int GetRecordingDeviceName(int nIndex,char* szDeviceName);
	static  int SetSoundDevice(char* szPlaybackDeviceName, char* szRecordingDeviceName);
	static  int SetPlayDeviceVolume(int nVolume);
	static  int GetPlayDeviceVolume(void);
	static  int SetRecordingDeviceVolume(int nVolume);
	static  int GetRecordingDeviceVolume(void);

	static  int GetMicrophoneVolumeState(int&nState);

	static  int PlayDTMF(char dtmf);

	static  int SetMicphoneVolumeMute(int nMute);
	static  int GetMicphoneVolumeMute(void);

	virtual void OnMEDEC_SIP_RegState(int nAccountID,int nState){};
	virtual void OnMEDEC_SIP_CallState(int nCallID,int nState){};
	virtual void OnMEDEC_SIP_CallIncoming(int nCallID,char* szContact){};
	virtual void OnMEDEC_SIP_CallHold(int nCallID){};
	virtual void OnMEDEC_SIP_MsgRecvd(char*szFrom,char*szText){};
	virtual void OnMEDEC_SIP_BuddyStatus(int nBuddyID,int nStatus,const char*szText){};
	virtual void OnMEDEC_SIP_DtmfDigit(int nCallID,int nDigit){};
	virtual void OnMEDEC_SIP_MessageWaiting(int nWaiting,char*szInfo){};
	virtual void OnMEDEC_SIP_CallReplaced(int nOldCallID,int nNewCallID){};

	// {{ liurui 2013/03/14
	static  int SetSpeakerVolumeMute(int nMute);
	static  int GetSpeakerVolumeMute(void);
};

#endif
