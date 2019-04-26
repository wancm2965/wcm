#ifndef __COMECCALL_H__
#define __COMECCALL_H__

#include <string>
using namespace std;
#include "COMEC.h"

typedef struct tagCOMEC_CALL_STATICS
{
	int	nSendAudioBitrate;		//发送的音频码流
	int nSendAudioLostCount;	//发送的音频丢包数
	int nSendAudioLostPercent;	//发送的音频丢包率
	int nRecvAudioBitrate;		//接收的音频码流
	int nRecvAudioLostCount;	//接收的音频丢包数
	int nRecvAudioLostPercent;	//接收的音频丢包率
}COMEC_CALL_STATICS,*PCOMEC_CALL_STATICS;

class COMEC_API COMECCall
{
public:
	COMECCall(void);
	virtual~COMECCall(void);
public:
	virtual void Call_Attach(void);
	virtual void Call_Detach(void);

	//发出呼叫请求
	virtual int Call_OutGoing(const string&strAccountID,const string&strCallTo,string&strCallID);
	//接受呼叫请求
	virtual int Call_Answer(const string&strCallID);
	//拒绝呼叫请求或挂断呼叫
	virtual int Call_Hangup(const string&strCallID);

	//取呼叫数据
	virtual int Call_GetStatics(const string&strCallID,COMEC_CALL_STATICS&callStatics);

	//设置对方的音量（播放音量）
	virtual int Call_SetPeerVolume(const string&strCallID,int nVolume/*0-10-20*/);
	//获取对方的音量（播放音量）
	virtual int Call_GetPeerVolume(const string&strCallID,int&nVolume);
	//设置对方静音（播放静音）
	virtual int Call_SetPeerMute(const string&strCallID,bool bMute);
	//获取对方静音（播放静音）
	virtual int Call_GetPeerMute(const string&strCallID,bool&bMute);
	//获取对方当前音量
	virtual int Call_GetPeerLevel(const string&strCallID,int&nLevel/*0-10*/);

	//设置本地静音检测
	virtual int Call_SetLocalVAD(const string&strCallID,bool bVAD);
	//获取本地静音检测设置
	virtual int Call_GetLocalVAD(const string&strCallID,bool&bVAD);
	//设置本地静音
	virtual int Call_SetLocalMute(const string&strCallID,bool bMute);
	//获取本地静音设置
	virtual int Call_GetLocalMute(const string&strCallID,bool&bMute);
	//获取本地音量（采集音量）
	virtual int Call_GetLocalLevel(const string&strCallID,int&nLevel/*0-10*/);
	//发送DTMF
	virtual int Call_SendDTMF(const string&strCallID,char dtmf);

	//电话呼入事件
	virtual void OnCall_Incoming(const string&strAccountID,const string&strCallID,const string&strCallFrom)=0;//对方呼叫您
	//电话呼出后对方在响铃的事件
	virtual void OnCall_Ringing(const string&strCallID)=0;//对方响铃，等待对方响应
	//对方接受了呼叫请求的事件
	virtual void OnCall_Answered(const string&strCallID)=0;//对方接受
	//对方挂断了呼叫（包括拒绝）的事件
	virtual void OnCall_Hangup(const string&strCallID)=0;//对方挂断
	//呼叫建立的事件
	virtual void OnCall_Established(const string&strCallID)=0;//对话建立
	//呼叫结束的时间
	virtual void OnCall_Terminated(const string&strCallID)=0;//对话结束
};


#endif