//IMXCall.h
#ifndef __IMXCALL_H__
#define __IMXCALL_H__

#include <string>
#include "../IMXEC_Call/Callbase.h"
#include <IMXEC/IMXEC_MediaChannel.h>

class IMXCall
	: public CallBase
	, public IMXEC_PlayChanCallback
{
public:
	IMXCall(const std::string&strUserID,const std::string&strUserName,const std::string&strCallID);
	virtual~IMXCall(void);
public:
	typedef enum
	{
		CALL_DIR_IN=0,
		CALL_DIR_OUT,
		CALL_DIR_COUNT,
	}CALL_DIRECTION;

	typedef enum
	{
		CALL_STATUS_IDLE=0,
		CALL_STATUS_RING,
		CALL_STATUS_ESTABLISHED,
	}CALL_STATUS;
public:
	virtual void Close(void);

	virtual int AcceptCall(void);
	virtual int HangupCall(void);

	virtual int OpenPlay(unsigned long nAudioID,unsigned long nVideoID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU);

	/******************************************************************************
	* SetLocalMainVideoWnd
	*���������ñ���������Ƶ��ʾ�Ĵ���
	*���룺	hWnd			-��������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetLocalMainVideoWnd(HWND hWnd);

	/******************************************************************************
	* SetLocalMainVideoWndPos
	*���������ñ���������Ƶ��ʾ�Ĵ���λ��
	*���룺	x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetLocalMainVideoWndPos(int x,int y,int cx,int cy);

	/******************************************************************************
	* SetLocalMainVideoParam
	*���������ñ���������Ƶ����
	*���룺	nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetLocalMainVideoEnableOSD
	*���������ñ���������Ƶ�Ƿ�����OSD
	*���룺	cszCallID		-���б�ʶ
			nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoEnableOSD(int nEnable);

	/******************************************************************************
	* SetLocalMainVideoOSDText
	*���������ñ���������ƵOSD����
	*���룺	cszCallID		-���б�ʶ
			cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoOSDText(const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetLocalMainVideoOSDFont
	*���������ñ���������ƵOSD����,�ֵ���ɫ�ͱ���ɫ
	*���룺	logfont			-OSD����
			crText			-�ֵ���ɫ
			crBk			-����ɫ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoOSDFont(LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

	/******************************************************************************
	* GetLocalMainVideoInfo
	*��������ȡ����������Ƶ��Ϣ
	*���룺	��
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);

	/******************************************************************************
	* GetLocalMainVideoPacketStatistics
	*��������ȡ��������Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetRemoteMainVideoWnd
	*����������Զ��������Ƶ��ʾ�Ĵ���
	*���룺	��
			hWnd			-Զ������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetRemoteMainVideoWnd(HWND hWnd);

	/******************************************************************************
	* SetRemoteMainVideoWndPos
	*����������Զ��������Ƶ��ʾ�Ĵ���λ��
	*���룺	x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetRemoteMainVideoWndPos(int x,int y,int cx,int cy);

	/******************************************************************************
	* SetRemoteMainVideoParam
	*����������Զ��������Ƶ����
	*���룺	nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);

	/******************************************************************************
	* GetRemoteMainVideoInfo
	*��������ȡԶ��������Ƶ��Ϣ
	*���룺	��
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);

	/******************************************************************************
	* GetRemoteMainVideoPacketStatistics
	*��������ȡԶ������Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetSecondVideoWnd
	*���������ø�����Ƶ��ʾ�Ĵ���
	*���룺��
			hWnd			-Զ�̸�����Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSecondVideoWnd(HWND hWnd);

	/******************************************************************************
	* SetSecondVideoWndPos
	*���������ø�����Ƶ��ʾ�Ĵ���λ��
	*���룺	x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSecondVideoWndPos(int x,int y,int cx,int cy);

	/******************************************************************************
	* SetSecondVideoParam
	*���������ø�����Ƶ����
	*���룺	nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetSecondVideoEnableOSD
	*���������ø�����Ƶ�Ƿ�����OSD
	*���룺	nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoEnableOSD(int nEnable);

	/******************************************************************************
	* SetSecondVideoOSDText
	*���������ø�����ƵOSD����
	*���룺	cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoOSDText(const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetSecondVideoOSDFont
	*���������ø�����ƵOSD����,�ֵ���ɫ�ͱ���ɫ
	*���룺	logfont			-OSD����
			crText			-�ֵ���ɫ
			crBk			-����ɫ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoOSDFont(LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

	/******************************************************************************
	* GetSecondVideoInfo
	*��������ȡ������Ƶ��Ϣ
	*���룺	��
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetSecondVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);
	
	/******************************************************************************
	* GetSecondVideoPacketStatistics
	*��������ȡ������Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetSecondVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetLocalAudioLevel
	*��������ȡ������ƵLevel��ʵ��������С��
	*���룺	��
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalAudioLevel(int&nLevel);

	/******************************************************************************
	* SetLocalAudioVolume
	*��������ȡ������Ƶ����
	*���룺	nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalAudioVolume(int nVolume);

	/******************************************************************************
	* SetLocalAudioMute
	*��������ȡ������Ƶ�Ƿ���
	*���룺	nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalAudioMute(int nMute);

	/******************************************************************************
	* GetLocalAudioCodecID
	*��������ȡ������Ƶ������Ϣ
	*���룺	��
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalAudioCodecID(X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetLocalAudioPacketStatistics
	*��������ȡ������Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetRemoteAudioLevel
	*��������ȡԶ����ƵLevel��ʵ��������С��
	*���룺	��
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteAudioLevel(int&nLevel);

	/******************************************************************************
	* SetRemoteAudioVolume
	*��������ȡԶ����Ƶ����
	*���룺	nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteAudioVolume(int nVolume);

	/******************************************************************************
	* SetRemoteAudioMute
	*��������ȡԶ����Ƶ�Ƿ���
	*���룺	nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteAudioMute(int nMute);

	/******************************************************************************
	* GetRemoteAudioCodecID
	*��������ȡԶ����Ƶ����
	*���룺	��
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteAudioCodecID (X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetRemoteAudioPacketStatistics
	*��������ȡԶ����Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);


	/******************************************************************************
	* StartRecordLocalMainToAVI
	*��������ʼ��������¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordLocalMainToAVI(const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordLocalMainToAVI
	*������ֹͣ��������¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordLocalMainToAVI(void);

	/******************************************************************************
	* StartRecordRemoteMainToAVI
	*��������ʼԶ������¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordRemoteMainToAVI(const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordRemoteMainToAVI
	*������ֹͣԶ������¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordRemoteMainToAVI(void);

	/******************************************************************************
	* StartRecordSecondToAVI
	*��������ʼ����¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordSecondToAVI(const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordSecondToAVI
	*������ֹͣ����¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordSecondToAVI(void);

	/******************************************************************************
	* GetMainAudioID
	*��������ȡ������Ƶ��ʶ
	*���룺 ��
	*����� nMainAudioID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetMainAudioID(unsigned long&nMainAudioID);

	/******************************************************************************
	* GetMainVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 ��
	*����� nMainVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetMainVideoID(unsigned long&nMainVideoID);

	/******************************************************************************
	* GetSecondVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 ��
	*����� nSecondVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetSecondVideoID(unsigned long&nSecondVideoID);

	/******************************************************************************
	* SetMainVideoScreenIndex
	*����������������Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetMainVideoScreenIndex(int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenIndex
	*���������ø�����Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSecondVideoScreenIndex(int nScreenIndex);


	/******************************************************************************
	* SetSecondVideoScreenWnd
	*���������ø�����Ƶ������Ļ����λ��
	*���룺	nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetRemoteVideoScreenWnd
	*����������Զ����Ƶ������Ļ����λ��
	*���룺	nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetLocalRecordCallback
	*���������ñ���¼��ص�
	*���룺	pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalRecordCallback(IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetRemoteRecordCallback
	*����������Զ��¼��ص�
	*���룺pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteRecordCallback(IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetSecondRecordCallback
	*���������ø���¼��ص�
	*���룺	pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondRecordCallback(IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SaveLocalMainVideoToBMPFile
	*���������汾��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveLocalMainVideoToBMPFile(const char*cszPathName);

	/******************************************************************************
	* SaveRemoteMainVideoToBMPFile
	*����������Զ��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveRemoteMainVideoToBMPFile(const char*cszPathName);

	/******************************************************************************
	* SaveSecondVideoToBMPFile
	*���������渨����Ƶ��BMPͼƬ�ļ�
	*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveSecondVideoToBMPFile(const char*cszPathName);

#ifdef _TIANZHU_CACHE_
	/******************************************************************************
	* SetRemoteVideoCache
	*����������Զ���Ӳ��ŵĻ���֡��
	*���룺nCacheFrameCount	-�����֡����0 - 60��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetRemoteVideoCache(int nCacheFrameCount);
#endif

protected:
	//��Ƶ���ݻص�����
	virtual void OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop){};
	virtual void OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
	//��Ƶ���ݻص�����
	//nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
	virtual void OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode){};
protected:
	std::string					m_strUserID;
	std::string					m_strUserName;

	CALL_DIRECTION				m_CallDirection;
	CALL_STATUS					m_CallStatus;
	std::string					m_strCallID;

	IMXEC_PlayChan*				m_pRemoteMainPlayChan;
	HWND						m_hVideoWndRemoteMain;
};

#endif
