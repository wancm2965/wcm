//NETEC_MediaReceiver.h
#ifndef __NETEC_MEDIARECEIVER_H__
#define __NETEC_MEDIARECEIVER_H__

#include <NETEC/NETEC_Export.h>
#include <NETEC/NETEC.h>

#define MEDIA_RECEIVER_NOT_STARTED			0x00000001
#define MEDIA_RECEIVER_CONNECTING			0x00000002
#define MEDIA_RECEIVER_CONNECTED			0x00000004
#define MEDIA_RECEIVER_DISCONNECTED			0x00000008

//ý�������
class NETEC_MediaReceiverCallback
{
public:
	NETEC_MediaReceiverCallback(void){};
	virtual~NETEC_MediaReceiverCallback(void){};
public:
	/******************************************************************************
	* OnNETEC_MediaReceiverCallbackAudioPacket
	*���������յ���Ƶ���ݰ��Ļص�
	*���룺	pData			-���ݰ�ָ��
			nLen			-���ݰ�����
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MediaReceiverCallbackAudioPacket(unsigned char*pData,int32_t nLen)=0;

	/******************************************************************************
	* OnNETEC_MediaReceiverCallbackVideoPacket
	*���������յ���Ƶ���ݰ��Ļص�
	*���룺	pData			-���ݰ�ָ��
			nLen			-���ݰ�����
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MediaReceiverCallbackVideoPacket(unsigned char*pData,int32_t nLen)=0;
};

class NETEC_API NETEC_MediaReceiver
{
public:
	NETEC_MediaReceiver(void){};
	virtual~NETEC_MediaReceiver(void){};
public:
	/******************************************************************************
	* Open
	*��������
	*���룺	cszPeerNodeID	-�Զ˽ڵ��ʶ
			cszPeerNATIP	-�Զ�����IP
			cszPeerLocalIP	-�Զ˱���IP
			nPeerLocalPort	-�Զ˱����˿�
			cszPeerMCUID	-�Զ�MCU��ʶ
			cszPeerMCUIP	-�Զ�MCU��ַ
			usPeerMCUPort	-�Զ�MCU�˿�
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t Open(const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort)=0;

	/******************************************************************************
	* Close
	*�������ر�
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void Close(void)=0;

	/******************************************************************************
	* SetAudioID
	*������������Ƶ��ʶ
	*���룺	ulAudioID	-��Ƶ��ʶ
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t SetAudioID(uint32_t ulAudioID)=0;

	/******************************************************************************
	* StartAudio
	*������������Ƶ
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t StartAudio(void)=0;

	/******************************************************************************
	* StopAudio
	*������ֹͣ��Ƶ
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void StopAudio(void)=0;

	/******************************************************************************
	* IsAudioStarted
	*��������ȡ�Ƿ���������Ƶ
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����>=0��0:δ�����������Ѿ������ˡ���ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t IsAudioStarted(void)=0;

	/******************************************************************************
	* EnableAudio
	*�����������Ƿ�ʼ������Ƶ
	*���룺	nEnable			-�Ƿ�ʼ������Ƶ��0��ֹͣ������Ƶ������ʼ������Ƶ��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual int32_t EnableAudio(int32_t nEnable)=0;

	/******************************************************************************
	* IsAudioEnable
	*��������ȡ�Ƿ�ʼ����������Ƶ
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����>=0��0:�Ѿ���ʼ���գ��Ѿ�ֹͣ���ա���ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t IsAudioEnable(void)=0;

	/******************************************************************************
	* GetAudioPacketStatistics
	*��������ȡ��Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t GetAudioPacketStatistics(uint32_t&ulTotalPackets,uint32_t&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate)=0;

	/******************************************************************************
	* GetAudioNetworkStatus
	*��������ȡ��Ƶ������״̬
	*���룺	��
	*����� nNetworkStatus	-������[0-100]
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t GetAudioNetworkStatus(uint32_t&nNetworkStatus)=0;


	/******************************************************************************
	* SetVideoID
	*����������������Ƶ��ʶ
	*���룺	ulVideoID	-������Ƶ��ʶ
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t SetVideoID(uint32_t ulVideoID)=0;

	/******************************************************************************
	* StartVideo
	*����������������Ƶ
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t StartVideo(void)=0;

	/******************************************************************************
	* StopVideo
	*������ֹͣ������Ƶ
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void StopVideo(void)=0;

	/******************************************************************************
	* IsVideoStarted
	*��������ȡ�Ƿ�������������Ƶ
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����>=0��0:δ�����������Ѿ������ˡ���ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t IsVideoStarted(void)=0;

	/******************************************************************************
	* EnableVideo
	*�����������Ƿ�ʼ������Ƶ
	*���룺	nEnable			-�Ƿ�ʼ������Ƶ��0��ֹͣ������Ƶ������ʼ������Ƶ��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual int32_t EnableVideo(int32_t nEnable)=0;

	/******************************************************************************
	* IsVideoEnable
	*��������ȡ�Ƿ�ʼ����������Ƶ
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����>=0��0:�Ѿ���ʼ���գ��Ѿ�ֹͣ���ա���ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t IsVideoEnable(void)=0;

	virtual int32_t SendCtrl(void*pData,int32_t nLen)=0;
	virtual int32_t SendPTZ(void*pData,int32_t nLen)=0;

	/******************************************************************************
	* GetVideoPacketStatistics
	*��������ȡ��Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t GetVideoPacketStatistics(uint32_t&ulTotalPackets,uint32_t&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate)=0;

	/******************************************************************************
	* GetVideoNetworkStatus
	*��������ȡ��Ƶ������״̬
	*���룺	��
	*����� nNetworkStatus	-������[0-100]
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t GetVideoNetworkStatus(uint32_t&nNetworkStatus)=0;

	/******************************************************************************
	* RequestKeyFrame
	*����������ؼ�֡
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t RequestKeyFrame(void)=0;

	/******************************************************************************
	* SetFrameRateControlMode
	*����������֡�ʿ���ģʽ
	*���룺	nMode			-֡�ʿ���ģʽ
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE nMode)=0;

	/******************************************************************************
	* SetFrameTypeControlMode
	*����������֡���Ϳ���ģʽ
	*���룺	nMode			-֡���Ϳ���ģʽ
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE nMode)=0;

	virtual int32_t SetAudioCast(bool bAudioCast)=0;
	virtual int32_t SetVideoCast(bool bVideoCast)=0;

	static NETEC_MediaReceiver*Create(NETEC_MediaReceiverCallback&rCallback);

	static void SetConferenceRecording(int32_t nFlagConferenceRecording);
	static int32_t GetConferenceRecording(void);

	virtual void SetRecording(int32_t nFlagRecording)=0;
};

#endif