//MEDEC_SIP.h
#ifndef __MEDEC_SIP_H__
#define __MEDEC_SIP_H__

#include <MEDEC/MEDEC_Export.h>

// calback function definitions
typedef int __stdcall fptr_regstate(int, int);				// on registration state changed
typedef int __stdcall fptr_callstate(int, int);	// on call state changed
typedef int __stdcall fptr_callincoming(int, char*);	// on call incoming
typedef int __stdcall fptr_callholdconf(int);
typedef int __stdcall fptr_msgrec (char*, char*);
typedef int __stdcall fptr_buddystatus(int, int, const char*);
typedef int __stdcall fptr_dtmfdigit(int callId, int digit);
typedef int __stdcall fptr_mwi(int mwi, char* info);
typedef int __stdcall fptr_crep(int oldid, int newid);

// Callback registration 
extern "C" MEDEC_API int MEDEC_SIP_SetRegStateCallback(fptr_regstate cb);	  // register registration notifier
extern "C" MEDEC_API int MEDEC_SIP_SetCallStateCallback(fptr_callstate cb); // register call notifier
extern "C" MEDEC_API int MEDEC_SIP_SetCallIncoming(fptr_callincoming cb); // register incoming call notifier
extern "C" MEDEC_API int MEDEC_SIP_SetCallHoldConfirmCallback(fptr_callholdconf cb); // register call notifier
extern "C" MEDEC_API int MEDEC_SIP_SetMessageReceivedCallback(fptr_msgrec cb); // register call notifier
extern "C" MEDEC_API int MEDEC_SIP_SetBuddyStatusChangedCallback(fptr_buddystatus cb); // register call notifier
extern "C" MEDEC_API int MEDEC_SIP_SetDtmfDigitCallback(fptr_dtmfdigit cb); // register dtmf digit notifier
extern "C" MEDEC_API int MEDEC_SIP_SetMessageWaitingCallback(fptr_mwi cb); // register MWI notifier
extern "C" MEDEC_API int MEDEC_SIP_SetCallReplaced(fptr_crep cb); // register Call replaced notifier

// common API
extern "C" MEDEC_API int MEDEC_SIP_SetListenPort(unsigned short nPort);
extern "C" MEDEC_API int MEDEC_SIP_Start(void);
extern "C" MEDEC_API int MEDEC_SIP_Stop(void); 

extern "C" MEDEC_API int MEDEC_SIP_GetNumOfCodecs();
extern "C" MEDEC_API int MEDEC_SIP_GetCodec(int index, char* codec);
extern "C" MEDEC_API int MEDEC_SIP_SetCodecPriority(char* name, int index);
// call API
extern "C" MEDEC_API int MEDEC_SIP_RegisterAccount(char* uri, char* reguri, char* domain, char* username, char* password, char* proxy, bool isdefault);
extern "C" MEDEC_API int MEDEC_SIP_MakeCall(int accountId, const char* uri); 
extern "C" MEDEC_API int MEDEC_SIP_ReleaseCall(int callId); 
extern "C" MEDEC_API int MEDEC_SIP_AnswerCall(int callId, int code);
extern "C" MEDEC_API int MEDEC_SIP_HoldCall(int callId);
extern "C" MEDEC_API int MEDEC_SIP_RetrieveCall(int callId);
extern "C" MEDEC_API int MEDEC_SIP_XferCall(int callid, char* uri);
extern "C" MEDEC_API int MEDEC_SIP_XferCallWithReplaces(int callId, int dstSession);
extern "C" MEDEC_API int MEDEC_SIP_ServiceReq(int callId, int serviceCode, const char* destUri);
extern "C" MEDEC_API int MEDEC_SIP_DialDtmf(int callId, char* digits, int mode);
extern "C" MEDEC_API int MEDEC_SIP_RemoveAccounts();
extern "C" MEDEC_API int MEDEC_SIP_SendInfo(int callid, char* content);
extern "C" MEDEC_API int MEDEC_SIP_GetCurrentCodec(int callId, char* codec);
extern "C" MEDEC_API int MEDEC_SIP_MakeConference(int callId);
extern "C" MEDEC_API int MEDEC_SIP_SendCallMessage(int callId, char* message);
// IM & Presence api
extern "C" MEDEC_API int MEDEC_SIP_AddBuddy(char* uri, bool subscribe);
extern "C" MEDEC_API int MEDEC_SIP_RemoveBuddy(int buddyId);
extern "C" MEDEC_API int MEDEC_SIP_SendMessage(int accId, char* uri, char* message);
extern "C" MEDEC_API int MEDEC_SIP_SetStatus(int accId, int presence_state);

extern "C" MEDEC_API int MEDEC_SIP_GetPlayDeviceCount(void);
extern "C" MEDEC_API int MEDEC_SIP_GetPlayDeviceName(int nIndex,char* szDeviceName);
extern "C" MEDEC_API int MEDEC_SIP_GetRecordingDeviceCount(void);
extern "C" MEDEC_API int MEDEC_SIP_GetRecordingDeviceName(int nIndex,char* szDeviceName);
extern "C" MEDEC_API int MEDEC_SIP_SetSoundDevice(char* playbackDeviceName, char* recordingDeviceName);
extern "C" MEDEC_API int MEDEC_SIP_SetPlayDeviceVolume(int nVolume);
extern "C" MEDEC_API int MEDEC_SIP_GetPlayDeviceVolume(void);
extern "C" MEDEC_API int MEDEC_SIP_SetRecordingDeviceVolume(int nVolume);
extern "C" MEDEC_API int AGC_SetRecordingDeviceVolume(int nVolume);
extern "C" MEDEC_API int MEDEC_SIP_GetRecordingDeviceVolume(void);

extern "C" MEDEC_API int MEDEC_SIP_GetMicrophoneVolumeState(int&nState);

extern "C" MEDEC_API int MEDEC_SIP_PlayDTMF(char dtmf);

extern "C" MEDEC_API int MEDEC_SIP_PollForEvents(int timeout);

extern "C" MEDEC_API int MEDEC_SetAEC(int nEchoTailMS,int nNS,int nPostAGC,int nPreAGC,int nAEC,int nAECLevel);
extern "C" MEDEC_API int MEDEC_GetSpeakerLevel(int&nLevel);

// liurui: 2011/3/15
extern "C" MEDEC_API int MEDEC_GetNoSoundDev(void);

extern "C" MEDEC_API int MEDEC_GetMicLevel(int&nLevel);

// Audec audio device selector
extern "C" MEDEC_API int MEDEC_SetAudecInputDevice(int nIndex);
extern "C" MEDEC_API int MEDEC_SetAudecOutputDevice(int nIndex);

extern "C" MEDEC_API int MEDEC_SetAudecInputVolume(int nVolume);
extern "C" MEDEC_API int MEDEC_GetAudecInputVolume(void);
extern "C" MEDEC_API int MEDEC_SetAudecOutputVolume(int nVolume);
extern "C" MEDEC_API int MEDEC_GetAudecOutputVolume(void);

extern "C" MEDEC_API int MEDEC_GetAudecInputDevName(int nDevID, char * szDevName);
extern "C" MEDEC_API int MEDEC_GetAudecOutputDevName(int nDevID, char * szDevName);

extern "C" MEDEC_API int MEDEC_GetAudecInputDevCount(void);
extern "C" MEDEC_API int MEDEC_GetAudecOutputDevCount(void);

extern "C" MEDEC_API int MEDEC_SetAudecInputType(int nType);

extern "C" MEDEC_API int MEDEC_SetAudecInputBoost(int nBoost);
extern "C" MEDEC_API int MEDEC_GetAudecInputBoost(void);

extern "C" MEDEC_API int MEDEC_SetAudecOutputMute(int nMute);
extern "C" MEDEC_API int MEDEC_GetAudecOutputMute(void);

extern "C" MEDEC_API float MEDEC_GetAudecMicphoneDBValue(void);
extern "C" MEDEC_API float MEDEC_GetAudecSpeakerDBValue(void);

extern "C" MEDEC_API int MEDEC_SetAudecInputMute(int nMute);
extern "C" MEDEC_API int MEDEC_GetAudecInputMute(void);

extern "C" MEDEC_API int MEDEC_SetAudecDummyEngine(int nFlag);
extern "C" MEDEC_API int MEDEC_GetAudecDummyEngine(void);

#endif