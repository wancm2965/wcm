//IMXEC_Call.h
#ifndef __IMXEC_CALL_H__
#define __IMXEC_CALL_H__

#include <AVCONMP/IMXEC_Export.h>
#include <MPVideo/MPVideo.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <winsock2.h>
#include <windows.h>


//���н���ԭ��
typedef enum
{
	IMXEC_CALL_END_BY_TIMEOUT=0,		//���г�ʱ
	IMXEC_CALL_END_BY_REMOTE_HANGUP,	//Զ�˽�������
	IMXEC_CALL_END_BY_LOCAL_HANGUP		//������������
}IMXEC_CALL_END_REASON;

typedef enum
{
	IMXEC_CALL_DIR_UNKOWN=0,
	IMXEC_CALL_DIR_IN,
	IMXEC_CALL_DIR_OUT,
}IMXEC_CALL_DIRECTION;


//H235 Media use

typedef enum
{
	IMXEC_CALL_H235AUTH_UNKOWN=0,
	IMXEC_CALL_H235AUTH_SUCCESS,         /*H235��֤ͨ��״̬*/
	IMXEC_CALL_H235AUTH_FAILURE,         /*H235��֤ʧ��״̬*/
	IMXEC_CALL_H235MEDIA_FAILURE         /*H235ý����������ԿЭ��ʧ��״̬*/
}IMXEC_CALL_H235AUTH_STATE;


typedef enum
{
	IMXEC_CALL_MEDIA_UNKOWN=0,
	IMXEC_CALL_MEDIA_AUDIO,              /*��Ƶ����*/
	IMXEC_CALL_MEDIA_MAINVIDEO,          /*��Ƶ����*/
	IMXEC_CALL_MEDIA_H239VIDEO,          /*��������*/
	IMXEC_CALL_MEDIA_H224DATA            /*H224��������*/

}IMXEC_CALL_MEDIA_TYPE;





//PTZ[�����ң������]
typedef enum 
{
	PTZ_START_PAN_UP=0,		//��ʼ����
	PTZ_STOP_PAN_UP,		//ֹͣ����
	PTZ_START_PAN_DOWN,		//��ʼ����
	PTZ_STOP_PAN_DOWN,		//ֹͣ����
	PTZ_START_TILT_LEFT,	//��ʼ����
	PTZ_STOP_TILT_LEFT,		//ֹͣ����
	PTZ_START_TILT_RIGHT,	//��ʼ����
	PTZ_STOP_TILT_RIGHT,	//ֹͣ����
	PTZ_START_ZOOM_IN,		//��ʼ�Ŵ�
	PTZ_STOP_ZOOM_IN,		//ֹͣ�Ŵ�
	PTZ_START_ZOOM_OUT,		//��ʼ��С
	PTZ_STOP_ZOOM_OUT,		//ֹͣ��С
	PTZ_START_FOCUS_IN,		//��ʼ������۽�
	PTZ_STOP_FOCUS_IN,		//ֹͣ������۽�
	PTZ_START_FOCUS_OUT,	//��ʼ��Զ���۽�
	PTZ_STOP_FOCUS_OUT,		//ֹͣ��Զ���۽�
	PTZ_START_FOCUS_AUTO,	//��ʼ�Զ��۽�
	PTZ_STOP_FOCUS_AUTO,	//ֹͣ�Զ��۽�
	PTZ_START_IRIS_MINUS,	//��ʼ��Ȧ��С
	PTZ_STOP_IRIS_MINUS,	//ֹͣ��Ȧ��С
	PTZ_START_IRIS_PLUS,	//��ʼ��Ȧ���
	PTZ_STTOPIRIS_PLUS,		//ֹͣ��Ȧ���
}PTZ_ACTION;



class IMXEC_H323Callback
{
public:
	IMXEC_H323Callback(void){};
	virtual~IMXEC_H323Callback(void){};
public:
	//��Ƶ���ݻص�����nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
	virtual void OnIMXEC_H323CallbackMainVideoData(unsigned char*pData,int nLen,int nKeyFrame,VIDEC_CODEC_TYPE videoCodecType)=0;

	//��Ƶ���ݻص�����
	virtual void OnIMXEC_H323CallbackMainAudioData(unsigned char*pData,int nLen,X_AUDIO_CODEC_ID audioCodecType)=0;

	//��Ƶ���ݻص�����nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
	virtual void OnIMXEC_H323CallbackSecondVideoData(unsigned char*pData,int nLen,int nKeyFrame,VIDEC_CODEC_TYPE videoCodecType)=0;
};

//������
class IMXEC_API IMXEC_Call
{
public:
	IMXEC_Call(void);
	virtual~ IMXEC_Call(void);
protected:
	int		m_nExclusive;
public:
	int GetExclusive(void);
public:
	/******************************************************************************
	* Start
	*��������������
	*���룺nExclusive		-�Ƿ���������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int Start(int nExclusive=0);

	/******************************************************************************
	* Stop
	*������ֹͣ����
	*���룺��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual void Stop(void);

	/******************************************************************************
	* OnIMXEC_CallRingIn
	*�����������¼������ն˺��б�����ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRingIn(const char*cszCallID,const char*cszUserID,const char*cszUserName)=0;

	/******************************************************************************
	* OnIMXEC_CallRingOut
	*�����������¼�������������гɹ���ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRingOut(const char*cszCallID,const char*cszUserID,const char*cszUserName)=0;

	/******************************************************************************
	* OnIMXEC_CallEstablished
	*���������н����¼�����˫������ͨ���󡾱���������Զ�˵ĺ��������Զ�˽����˱����ĺ�������ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallEstablished(const char*cszCallID,const char*cszUserID,const char*cszUserName)=0;

	/******************************************************************************
	* OnIMXEC_CallDestroyed
	*���������������¼�����ͨ�������󡾱����Ҷϻ�Զ�˹Ҷϡ�ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
			nReason			-����ԭ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallDestroyed(const char*cszCallID,const char*cszUserID,const char*cszUserName,IMXEC_CALL_END_REASON nReason)=0;

	/******************************************************************************
	* OnIMXEC_CallRecvdPTZ
	*���������յ�PTZ�¼������Է�������̨��������ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)=0;

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStart
	*������������ʼ�¼�����������ʼʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStart(const char*cszCallID)=0;

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStop
	*����������ֹͣ�¼���������ֹͣʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStop(const char*cszCallID)=0;

	/******************************************************************************
	* OnIMXEC_SIPVirtualLineRegistrationSuccess
	*������SIPע��ɹ��¼�
	*���룺	nLineID		-��·��ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_SIPVirtualLineRegistrationSuccess(int nLineID){};

	/******************************************************************************
	* OnIMXEC_SIPVirtualLineRegistrationError
	*������SIPע��ʧ���¼�
	*���룺	nLineID			-��·��ʶ
			cszErrorCode	-������
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_SIPVirtualLineRegistrationError(int nLineID,const char*cszErrorCode){};

	/******************************************************************************
	* OnIMXEC_H323RegistrationSuccess
	*������H323ע��ɹ��¼�
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_H323RegistrationSuccess(void){};

	/******************************************************************************
	* OnIMXEC_H323RegistrationError
	*������H323ע��ʧ���¼�
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_H323RegistrationError(void){};

	/******************************************************************************
	* MakeCall
	*�������������
	*���룺	cszUserID		-�Է����û�ID��IP
			cszProtocol		-����Э�飬"H323","SIP","IMXEC"
	*�������
	*����ֵ�����б�ʶ
	*****************************************************************************/
	static const char*MakeCall(const char*cszUserID,const char*cszProtocol);

	/******************************************************************************
	* AcceptCall
	*���������ܺ�������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int AcceptCall(const char*cszCallID);

	/******************************************************************************
	* HangupCall
	*�������ҶϺ���
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int HangupCall(const char*cszCallID);

	/******************************************************************************
	* SendPTZ
	*����������PTZ��������̨�������
	*���룺	cszCallID		-���б�ʶ
			nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	/******************************************************************************
	* StartSecondVideo
	*��������ʼ����
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int StartSecondVideo (const char*cszCallID);

	/******************************************************************************
	* StopSecondVideo
	*������ֹͣ����
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int StopSecondVideo (const char*cszCallID);


	/******************************************************************************
	* GetLocalMainVideoInfo
	*��������ȡ����������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);

	/******************************************************************************
	* GetLocalMainVideoPacketStatistics
	*��������ȡ��������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);


	/******************************************************************************
	* GetRemoteMainVideoInfo
	*��������ȡԶ��������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);

	/******************************************************************************
	* GetRemoteMainVideoPacketStatistics
	*��������ȡԶ������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);


	/******************************************************************************
	* GetSecondVideoInfo
	*��������ȡ������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);
	
	/******************************************************************************
	* GetSecondVideoPacketStatistics
	*��������ȡ������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetLocalAudioLevel
	*��������ȡ������ƵLevel��ʵ��������С��
	*���룺	cszCallID		-���б�ʶ
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetLocalAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetLocalAudioVolume
	*��������ȡ������Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	static void SetLocalAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetLocalAudioMute
	*��������ȡ������Ƶ�Ƿ���
	*���룺	cszCallID		-���б�ʶ
			nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	static void SetLocalAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetLocalAudioCodecID
	*��������ȡ������Ƶ������Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetLocalAudioPacketStatistics
	*��������ȡ������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetRemoteAudioLevel
	*��������ȡԶ����ƵLevel��ʵ��������С��
	*���룺	cszCallID		-���б�ʶ
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetRemoteAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetRemoteAudioVolume
	*��������ȡԶ����Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	static void SetRemoteAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetRemoteAudioMute
	*��������ȡԶ����Ƶ�Ƿ���
	*���룺	cszCallID		-���б�ʶ
			nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	static void SetRemoteAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetRemoteAudioCodecID
	*��������ȡԶ����Ƶ����
	*���룺	cszCallID		-���б�ʶ
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetRemoteAudioPacketStatistics
	*��������ȡԶ����Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetCallSupportSecondVideo
	*��������ȡ�����Ƿ�֧�ָ�����H239��
	*���룺	cszCallID		-���б�ʶ
	*�����	��
	*����ֵ��֧�ַ��ط�0����֧�ַ���0
	*****************************************************************************/
	static int GetCallSupportSecondVideo(const char*cszCallID);

	/******************************************************************************
	* GetMainAudioID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nMainAudioID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID);

	/******************************************************************************
	* GetMainVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nMainVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID);

	/******************************************************************************
	* GetSecondVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nSecondVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID);

	/******************************************************************************
	* SetMainVideoScreenIndex
	*����������������Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenIndex
	*���������ø�����Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenWnd
	*���������ø�����Ƶ������Ļ����λ��
	*���룺	cszCallID		-���б�ʶ
			nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetRemoteVideoScreenWnd
	*����������Զ����Ƶ������Ļ����λ��
	*���룺	cszCallID		-���б�ʶ
			nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);


	/******************************************************************************
	* SendDTMF
	*����������DTMF
	*���룺	cszCallID		-���б�ʶ
			nDTMF			-DTMF
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SendDTMF(const char*cszCallID,const char nDTMF);

	/******************************************************************************
	* StartMixingWithWaveFile
	*��������ʼ�Ժ��н���WAVE�ļ��������Է�������WAVE�ļ����ŵ�����
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-WAVE�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int StartMixingWithWaveFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* StopMixingWithWaveFile
	*������ֹͣ�Ժ��н���WAVE�ļ�����
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int StopMixingWithWaveFile(const char*cszCallID);

	/******************************************************************************
	* SetAudioOnly
	*�����������Ƿ�ֻ֧����Ƶ
	*���룺	nAudioOnly		-�Ƿ�ֻ֧����Ƶ��0��֧����Ƶ��Ƶ����0��ֻ֧����Ƶ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetAudioOnly(int nAudioOnly);

	/******************************************************************************
	* GetCallProtocol
	*��������ȡ����Э��
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ����غ���Э�飬ʧ�ܷ��ؿմ�""
	*****************************************************************************/
	static const char*GetCallProtocol(const char*cszCallID);

	/******************************************************************************
	* GetHasVideo
	*��������ȡ�Ƿ�ֻ����Ƶ
	*���룺cszCallID		-���б�ʶ
	*�������
	*����ֵ���������Ƶ����ô���ط�0�����򷵻�0
	*****************************************************************************/
	static int GetHasVideo(const char*cszCallID);

	/******************************************************************************
	* GetSecondVideoDirection
	*��������ȡ�����ķ���
	*���룺	cszCallID		-���б�ʶ
	*����� nDir			-�����ķ���
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir);




	//H235 Media use

	/******************************************************************************
	* GetH235AuthCheckState
	*��������ȡH235��֤���״̬ ����ȡ������֤��Ϊ����ʱ����֤���
	*���룺	cszCallID		  -���б�ʶ
	*����� nState  -��֤���״̬
			nLength -������֤ʧ��ԭ���ַ�������
	*����ֵ���ɹ�������֤ʧ��ԭ��ʧ�ܷ��ؿ�
	*****************************************************************************/
	const char* GetH235AuthCheckState(const char*cszCallID,IMXEC_CALL_H235AUTH_STATE &nState,int &nLength);

	/******************************************************************************
	* OnChannelRecvMediaEncryption
	*����������ý��ͨ���Ѽ���״̬֪ͨ�����򿪼��ܵĽ���ý��ͨ��ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	nMediaType		-ý������
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnChannelRecvMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType){};

	/******************************************************************************
	* OnChannelSendMediaEncryption
	*����������ý��ͨ���Ѽ���״̬֪ͨ�����򿪼��ܵķ���ý��ͨ��ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	nMediaType		-ý������
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnChannelSendMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType){};

	/******************************************************************************
	* SetExternalIP
	*��������������ӳ��IP
	*���룺cszExternalIP		-����ӳ��IP
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetExternalIP(const char*cszExternalIP);

	/******************************************************************************
	* SetMaxCallNumber
	*���������������и���
	*���룺nMaxCallNumber		-�����и���
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetMaxCallNumber(unsigned short nMaxCallNumber);

	/******************************************************************************
	* SetMaxVideoTypeNumber
	*������������Ƶ���ϳɸ�ʽ��
	*���룺nMaxVideoTypeNumber		-��Ƶ���ϳɸ�ʽ��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetMaxVideoTypeNumber(unsigned short nMaxVideoTypeNumber);

	/******************************************************************************
	* SetMaxTranscodeNumber
	*������������Ƶ���ת�����IP
	*���룺nMaxTranscodeNumber		-��Ƶ���ת�����
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetMaxTranscodeNumber(unsigned short nMaxTranscodeNumber);

	/******************************************************************************
	* SetMaxTranscodeNumber
	*������������Ƶת�뽵֡ģʽ
	*���룺nTranscodeSkipFrameCount		-(0:����֡��1����1��֡��2����1/4)
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetTranscodeSkipFrameCount(unsigned short nTranscodeSkipFrameCount);

	/******************************************************************************
	* SetTranscodeBitrate
	*������������Ƶת������
	*���룺nTranscodeBitrate		-(ת������)
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetTranscodeBitrate(unsigned int nTranscodeBitrate);

	/******************************************************************************
	* SetH323Callback
	*����������H323�ص�
	*���룺	cszCallID		-���б�ʶ
			pCallback		-�ص�ָ��
	*�������
	*����ֵ����
	*****************************************************************************/
	static void SetH323Callback(const char*cszCallID,IMXEC_H323Callback* pCallback);
};



#endif