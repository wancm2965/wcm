#pragma once

#include <stdio.h>
#include <string>
#include <map>

using namespace std;

#include "../include/NETEC/NETEC_Node.h"
#include "../include/NETEC/NETEC_Core.h"
#include "../include/NETEC/NETEC_MediaReceiver.h"
#include "../include/HPATC_Transcode.h"
#include "../include/VIDEC_MP4.h"
#include "../include/VIDEC_Header.h"
#include "XThreadBase.h"
#include "XAutoLock.h"

extern void* g_mkvHandle;

void XSleep(unsigned long ulMS);
unsigned long XGetTimestamp(void);
//---------------------------------------------------------------------------------------
// player 类定义
class player 
	: public NETEC_MediaReceiverCallback
	, public HPATC_TranscodeCallback
	, public XThreadBase
	, public NETEC_Node
{
	//
public:
	player(void);
	virtual ~player(void);

	bool Start(std::string strAV,int nIndex);
	void Stop(void);

public:
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);
	virtual void OnNETEC_MediaReceiverCallbackAudioPacket(unsigned char*pData,int nLen);
	virtual void OnNETEC_MediaReceiverCallbackVideoPacket(unsigned char*pData,int nLen);
	virtual void OnHPATC_TranscodeCallbackPacketData(HPATC_Transcode*pTranscode,void*pPacketData,int nPacketLen,unsigned long ulTimestamp/*原音频编码时间*/);

	virtual void ThreadProcMain(void);


protected:
	NETEC_MediaReceiver*	m_pIAVReceiver;
	HPATC_Transcode*		m_pHPATC_Transcode;
	unsigned long			m_nAudChanID;
	unsigned long			m_nVidChanID;
	std::string				m_strNoidID;
	std::string				m_strMCUID;
	std::string				m_strMCUIP;
	int						m_nVideoWidth;
	int						m_nVideoHeigh;	
	int						m_nIndex;
	bool					m_bWaitStop;
	XCritSec				m_csTranscode;
	XCritSec				m_csWriteAVData;
};