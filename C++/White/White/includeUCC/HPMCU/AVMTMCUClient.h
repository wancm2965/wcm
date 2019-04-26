//AVMTMCUClient.h
#ifndef __IAVMTMCUCLIENT_H__
#define __IAVMTMCUCLIENT_H__


#include <NETEC/NETEC_Export.h>
#include <NETEC/NETEC.h>

class IAVAudioReceiverNotify
{
public:
	IAVAudioReceiverNotify(void){};
	virtual~IAVAudioReceiverNotify(void){};
public:
	virtual void OnIAVAudioReceiverNotifyReceivedAudioFrame(const char*pData,int nLen)=0;
	virtual void OnIAVAudioReceiverNotifyConnected(void)=0;
};

class IAVAudioReceiver
{
public:
	IAVAudioReceiver(void){};
	virtual ~IAVAudioReceiver(void){};
public:
	virtual void Close(void)=0;
	virtual void EnableReceive(bool bEnable)=0;
	virtual bool IsEnableReceive(void)=0;
	virtual bool IsDisconnected(void)=0;
	virtual void GetPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate)=0;
};


NETEC_API IAVAudioReceiver*CreateIAVAudioReceiver(IAVAudioReceiverNotify&rIAVAudioReceiverNotify,unsigned long ulAudioChannelID,
												  const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short usPeerMCUPort,const char*cszPeerMCUType,
												  const char*cszLocalMCUID,const char*cszLocalMCUIP,unsigned short usLocalMCUPort,const char*cszLocalMCUType,int nForceUDP=0,bool bAudioCast=false);

class IAVAudioSenderNotify
{
public:
	IAVAudioSenderNotify(void){};
	virtual~IAVAudioSenderNotify(void){};
public:
	virtual void OnIAVAudioSenderNotifyConnected(void)=0;
};

class IAVAudioSender
{
public:
	IAVAudioSender(void){};
	virtual ~IAVAudioSender(void){};
public:
	virtual void Close(void)=0;
	virtual void EnableSend(bool bEnable)=0;
	virtual bool IsEnableSend(void)=0;
	virtual bool IsDisconnected(void)=0;
	virtual void GetPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate)=0;
};

NETEC_API IAVAudioSender*CreateIAVAudioSender(IAVAudioSenderNotify&rIAVAudioSenderNotify,unsigned long ulAudioChannelID,
											 const char*cszLocalMCUID,const char*cszLocalMCUIP,unsigned short usLocalMCUPort,const char*cszLocalMCUType);

class IAVVideoReceiverNotify
{
public:
	IAVVideoReceiverNotify(void){};
	virtual~IAVVideoReceiverNotify(void){};
public:
	virtual void OnIAVVideoReceiverNotifyReceivedVideoFrame(const char*pData,int nLen)=0;
	virtual void OnIAVVideoReceiverNotifyConnected(void)=0;
};


class IAVVideoReceiver
{
public:
	IAVVideoReceiver(void){};
	virtual ~IAVVideoReceiver(void){};
public:
	virtual void Close(void)=0;
	virtual void EnableReceive(bool bEnable)=0;
	virtual bool IsEnableReceive(void)=0;
	virtual bool IsDisconnected(void)=0;
	virtual void RequestKeyFrame(unsigned char nRequestKeyFrame)=0;

	virtual void GetPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate)=0;

	virtual int SendPTZToSender(void*pData,int nLen)=0;
	virtual int SendCtrlToSender(void*pData,int nLen)=0;

	virtual int SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE nMode)=0;
	virtual int SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE nMode)=0;

	virtual int SetFrameRateControlCommand(unsigned char nCMD)=0;
	virtual int SetFrameTypeControlCommand(unsigned char nCMD)=0;

	virtual int SendDataToSender(void*pData,int nLen)=0;
};


NETEC_API IAVVideoReceiver*CreateIAVVideoReceiver(IAVVideoReceiverNotify&rIAVVideoReceiverNotify,unsigned long ulVideoChannelID,
												  const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short usPeerMCUPort,const char*cszPeerMCUType,
												  const char*cszLocalMCUID,const char*cszLocalMCUIP,unsigned short usLocalMCUPort,const char*cszLocalMCUType,int nForceUDP=0,bool bVideoCast=false);

class IAVVideoSenderNotify
{
public:
	IAVVideoSenderNotify(void){};
	virtual~IAVVideoSenderNotify(void){};
public:
	virtual void OnIAVVideoSenderNotifyConnected(void)=0;
	virtual void OnIAVVideoSenderNotifyRecvdPTZ(void*pData,int nLen){};
	virtual void OnIAVVideoSenderNotifyRecvdCtrl(void*pData,int nLen){};
};

class IAVVideoSender
{
public:
	IAVVideoSender(void){};
	virtual ~IAVVideoSender(void){};
public:
	virtual void Close(void)=0;
	virtual void EnableSend(bool bEnable)=0;
	virtual bool IsEnableSend(void)=0;
	virtual void OnReceivedEnable(void)=0;
	virtual void OnReceivedDisable(void)=0;
	virtual bool IsDisconnected(void)=0;

	virtual void GetPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate)=0;
	virtual unsigned long GetBufferedTimeMS(int nFrameType)=0;

	virtual void SetMaxBitrate(int nMaxBitrate)=0;
};
NETEC_API IAVVideoSender*CreateIAVVideoSender(IAVVideoSenderNotify&rIAVVideoSenderNotify,unsigned long ulVideoChannelID,
												  const char*cszLocalMCUID,const char*cszLocalMCUIP,unsigned short usLocalMCUPort,const char*cszLocalMCUType);


class IAVMTDataSessionClientNotify
{
public:
	IAVMTDataSessionClientNotify(void){};
	virtual~IAVMTDataSessionClientNotify(void){};
public:
	virtual void OnIAVMTDataSessionClientNotifyConnected(void)=0;
};

class IAVMTDataSessionClient
{
public:
	IAVMTDataSessionClient(void){};
	virtual~IAVMTDataSessionClient(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual bool IsDisconnected(void)=0;
};

NETEC_API IAVMTDataSessionClient*CreateIAVMTDataSessionClient(IAVMTDataSessionClientNotify&rNotify,unsigned long ulDataChannelID, 
															  const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short usPeerMCUPort,const char*cszPeerMCUType,
															  const char*cszLocalMCUID,const char*cszLocalMCUIP,unsigned short usLocalMCUPort,const char*cszLocalMCUType);



#endif