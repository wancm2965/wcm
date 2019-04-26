#include "IMXCall.h"	
#include "IMXCallMgr.h"	

IMXCall::IMXCall(const std::string&strUserID,const std::string&strUserName,const std::string&strCallID)
:m_strUserID(strUserID)
,m_strUserName(strUserName)
,m_strCallID(strCallID)

,m_pRemoteMainPlayChan(NULL)
,m_hVideoWndRemoteMain(NULL)
{

}


IMXCall::~IMXCall(void)
{

}

void IMXCall::Close(void)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->Close();
		delete m_pRemoteMainPlayChan;
		m_pRemoteMainPlayChan=NULL;
	}
}

int IMXCall::AcceptCall(void)
{
	return 0;
}

int IMXCall::HangupCall(void)
{
	return 0;
}

int IMXCall::OpenPlay(unsigned long nAudioID,unsigned long nVideoID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU)
{
	if (m_pRemoteMainPlayChan==NULL)
	{
		m_pRemoteMainPlayChan=IMXEC_PlayChan::Create(*this);
		if (m_pRemoteMainPlayChan!=NULL)
		{
			if (0!=m_pRemoteMainPlayChan->Open(m_hVideoWndRemoteMain,nAudioID,nVideoID,cszPeerNodeID,cszPeerNATIP,nPeerNATPort,cszPeerLocalIP,nPeerLocalPort,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort,0))
			{
				m_pRemoteMainPlayChan->Close();
				delete m_pRemoteMainPlayChan;
				m_pRemoteMainPlayChan=NULL;
			}
			else
			{
				m_pRemoteMainPlayChan->StartAudio();
				m_pRemoteMainPlayChan->StartVideo();
				m_pRemoteMainPlayChan->SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE_FULL);
				m_pRemoteMainPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);
			}
		}
	}

	return (m_pRemoteMainPlayChan!=NULL?0:1);
}

/******************************************************************************
* SetLocalMainVideoWnd
*���������ñ���������Ƶ��ʾ�Ĵ���
*���룺	hWnd			-��������Ƶ��ʾ�Ĵ��ھ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::SetLocalMainVideoWnd(HWND hWnd)
{
	return IMXCallMgr::Instance().SetLocalMainVideoWnd(hWnd);
}


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
int IMXCall::SetLocalMainVideoWndPos(int x,int y,int cx,int cy)
{
	return IMXCallMgr::Instance().SetLocalMainVideoWndPos(x,y,cx,cy);
}


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
void IMXCall::SetLocalMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	IMXCallMgr::Instance().SetLocalMainVideoParam(nBrightness, nContrast, nSaturation, nHue,nGamma,nSharpen,bSharpenLocalOnly);
}


/******************************************************************************
* SetLocalMainVideoEnableOSD
*���������ñ���������Ƶ�Ƿ�����OSD
*���룺	cszCallID		-���б�ʶ
		nEnable			-�Ƿ�����OSD
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::SetLocalMainVideoEnableOSD(int nEnable)
{
	IMXCallMgr::Instance().SetLocalMainVideoEnableOSD(nEnable);
}


/******************************************************************************
* SetLocalMainVideoOSDText
*���������ñ���������ƵOSD����
*���룺	cszCallID		-���б�ʶ
		cszText			-OSD����
		nAlign			-���뷽ʽ
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::SetLocalMainVideoOSDText(const char*cszText,int nAlign)//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
{
	IMXCallMgr::Instance().SetLocalMainVideoOSDText(cszText,nAlign);
}


/******************************************************************************
* SetLocalMainVideoOSDFont
*���������ñ���������ƵOSD����,�ֵ���ɫ�ͱ���ɫ
*���룺	logfont			-OSD����
		crText			-�ֵ���ɫ
		crBk			-����ɫ
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::SetLocalMainVideoOSDFont(LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	IMXCallMgr::Instance().SetLocalMainVideoOSDFont(logfont,crText,crBk);
}


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
int IMXCall::GetLocalMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)
{
	return IMXCallMgr::Instance().GetLocalMainVideoInfo(nWidth,nHeight,nVideoCodecType,hVideoWnd);
}


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
int IMXCall::GetLocalMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return IMXCallMgr::Instance().GetLocalMainVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}


/******************************************************************************
* SetRemoteMainVideoWnd
*����������Զ��������Ƶ��ʾ�Ĵ���
*���룺	��
		hWnd			-Զ������Ƶ��ʾ�Ĵ��ھ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::SetRemoteMainVideoWnd(HWND hWnd)
{
	m_hVideoWndRemoteMain=hWnd;
	return -1;
}


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
int IMXCall::SetRemoteMainVideoWndPos(int x,int y,int cx,int cy)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		return 0;
	}
	return -1;
}


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
void IMXCall::SetRemoteMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->SetVideoParam(nBrightness, nContrast, nSaturation, nHue,nGamma,nSharpen);
	}
}


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
int IMXCall::GetRemoteMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		hVideoWnd=m_pRemoteMainPlayChan->GetVideoWnd();
		nWidth=m_pRemoteMainPlayChan->GetVideoWidth();
		nHeight=m_pRemoteMainPlayChan->GetVideoHeight();

		return 0;
	}
	return -1;
}


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
int IMXCall::GetRemoteMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->GetVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return -1;
}


/******************************************************************************
* SetSecondVideoWnd
*���������ø�����Ƶ��ʾ�Ĵ���
*���룺��
		hWnd			-Զ�̸�����Ƶ��ʾ�Ĵ��ھ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::SetSecondVideoWnd(HWND hWnd)
{
	return -1;
}


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
int IMXCall::SetSecondVideoWndPos(int x,int y,int cx,int cy)
{
	return -1;
}


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
void IMXCall::SetSecondVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
}


/******************************************************************************
* SetSecondVideoEnableOSD
*���������ø�����Ƶ�Ƿ�����OSD
*���룺	nEnable			-�Ƿ�����OSD
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::SetSecondVideoEnableOSD(int nEnable)
{
}


/******************************************************************************
* SetSecondVideoOSDText
*���������ø�����ƵOSD����
*���룺	cszText			-OSD����
		nAlign			-���뷽ʽ
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::SetSecondVideoOSDText(const char*cszText,int nAlign)//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
{
}


/******************************************************************************
* SetSecondVideoOSDFont
*���������ø�����ƵOSD����,�ֵ���ɫ�ͱ���ɫ
*���룺	logfont			-OSD����
		crText			-�ֵ���ɫ
		crBk			-����ɫ
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::SetSecondVideoOSDFont(LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
}


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
int IMXCall::GetSecondVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)
{
	return -1;
}


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
int IMXCall::GetSecondVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return -1;
}


/******************************************************************************
* GetLocalAudioLevel
*��������ȡ������ƵLevel��ʵ��������С��
*���룺	��
*�����	nLevel			-ʵ��������С��0-10��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::GetLocalAudioLevel(int&nLevel)
{
	return IMXCallMgr::Instance().GetLocalAudioLevel(nLevel);
}


/******************************************************************************
* SetLocalAudioVolume
*��������ȡ������Ƶ����
*���룺	nVolume			-����
*�����	��
*����ֵ����
*****************************************************************************/
void IMXCall::SetLocalAudioVolume(int nVolume)
{
	IMXCallMgr::Instance().SetLocalAudioVolume(nVolume);
}


/******************************************************************************
* SetLocalAudioMute
*��������ȡ������Ƶ�Ƿ���
*���룺	nMute			-�Ƿ���
*�����	��
*����ֵ����
*****************************************************************************/
void IMXCall::SetLocalAudioMute(int nMute)
{
	IMXCallMgr::Instance().SetLocalAudioMute(nMute);
}


/******************************************************************************
* GetLocalAudioCodecID
*��������ȡ������Ƶ������Ϣ
*���룺	��
*�����	nAudioCodecID	-����ID
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::GetLocalAudioCodecID(X_AUDIO_CODEC_ID &nAudioCodecID)
{
	return IMXCallMgr::Instance().GetLocalAudioCodecID(nAudioCodecID);
}


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
int IMXCall::GetLocalAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return IMXCallMgr::Instance().GetLocalAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}


/******************************************************************************
* GetRemoteAudioLevel
*��������ȡԶ����ƵLevel��ʵ��������С��
*���룺	��
*�����	nLevel			-ʵ��������С��0-10��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::GetRemoteAudioLevel(int&nLevel)
{
	nLevel=0;
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->GetLevel(nLevel);
	}
	return -1;
}


/******************************************************************************
* SetRemoteAudioVolume
*��������ȡԶ����Ƶ����
*���룺	nVolume			-����
*�����	��
*����ֵ����
*****************************************************************************/
void IMXCall::SetRemoteAudioVolume(int nVolume)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->SetVolume(nVolume);
	}
}


/******************************************************************************
* SetRemoteAudioMute
*��������ȡԶ����Ƶ�Ƿ���
*���룺	nMute			-�Ƿ���
*�����	��
*����ֵ����
*****************************************************************************/
void IMXCall::SetRemoteAudioMute(int nMute)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{

	}
}


/******************************************************************************
* GetRemoteAudioCodecID
*��������ȡԶ����Ƶ����
*���룺	��
*�����	nAudioCodecID	-����ID
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::GetRemoteAudioCodecID (X_AUDIO_CODEC_ID &nAudioCodecID)
{
	return -1;
}


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
int IMXCall::GetRemoteAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->GetAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return -1;
}


/******************************************************************************
* StartRecordLocalMainToAVI
*��������ʼ��������¼��
*���룺	cszPathName			-¼���ļ�����
		nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::StartRecordLocalMainToAVI(const char*cszPathName,int nStandardFormat)
{
	return IMXCallMgr::Instance().StartRecordLocalMainToAVI(cszPathName,nStandardFormat);
}


/******************************************************************************
* StopRecordLocalMainToAVI
*������ֹͣ��������¼��
*���룺��
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::StopRecordLocalMainToAVI(void)
{
	IMXCallMgr::Instance().StopRecordLocalMainToAVI();
}


/******************************************************************************
* StartRecordRemoteMainToAVI
*��������ʼԶ������¼��
*���룺	cszPathName			-¼���ļ�����
		nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::StartRecordRemoteMainToAVI(const char*cszPathName,int nStandardFormat)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->StartRecordToAVI(cszPathName,nStandardFormat);
	}
	return -1;
}


/******************************************************************************
* StopRecordRemoteMainToAVI
*������ֹͣԶ������¼��
*���룺��
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::StopRecordRemoteMainToAVI(void)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->StopRecordToAVI();
	}
}


/******************************************************************************
* StartRecordSecondToAVI
*��������ʼ����¼��
*���룺	cszPathName			-¼���ļ�����
		nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::StartRecordSecondToAVI(const char*cszPathName,int nStandardFormat)
{
	return -1;
}


/******************************************************************************
* StopRecordSecondToAVI
*������ֹͣ����¼��
*���룺��
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::StopRecordSecondToAVI(void)
{
}


/******************************************************************************
* GetMainAudioID
*��������ȡ������Ƶ��ʶ
*���룺 ��
*����� nMainAudioID	-������Ƶ��ʶ
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::GetMainAudioID(unsigned long&nMainAudioID)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
	}
	return -1;
}


/******************************************************************************
* GetMainVideoID
*��������ȡ������Ƶ��ʶ
*���룺 ��
*����� nMainVideoID	-������Ƶ��ʶ
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::GetMainVideoID(unsigned long&nMainVideoID)
{
	return -1;
}


/******************************************************************************
* GetSecondVideoID
*��������ȡ������Ƶ��ʶ
*���룺 ��
*����� nSecondVideoID	-������Ƶ��ʶ
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::GetSecondVideoID(unsigned long&nSecondVideoID)
{
	return -1;
}


/******************************************************************************
* SetMainVideoScreenIndex
*����������������Ƶͼ���������
*���룺 cszCallID		-���б�ʶ
		nScreenIndex	-�������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::SetMainVideoScreenIndex(int nScreenIndex)
{
	return -1;
}


/******************************************************************************
* SetSecondVideoScreenIndex
*���������ø�����Ƶͼ���������
*���룺 cszCallID		-���б�ʶ
		nScreenIndex	-�������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::SetSecondVideoScreenIndex(int nScreenIndex)
{
	return -1;
}



/******************************************************************************
* SetSecondVideoScreenWnd
*���������ø�����Ƶ������Ļ����λ��
*���룺	nScreenIndex	-��Ļ��
		nWndIndex		-���ں�
		nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::SetSecondVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	return ;
}


/******************************************************************************
* SetRemoteVideoScreenWnd
*����������Զ����Ƶ������Ļ����λ��
*���룺	nScreenIndex	-��Ļ��
		nWndIndex		-���ں�
		nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::SetRemoteVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		m_pRemoteMainPlayChan->SetScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
}

/******************************************************************************
* SetLocalRecordCallback
*���������ñ���¼��ص�
*���룺	pRecordCallback	-¼��ص�
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::SetLocalRecordCallback(IMXEC_RecordCallback*pRecordCallback)
{
	return ;
}

/******************************************************************************
* SetRemoteRecordCallback
*����������Զ��¼��ص�
*���룺pRecordCallback	-¼��ص�
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::SetRemoteRecordCallback(IMXEC_RecordCallback*pRecordCallback)
{
	return ;
}


/******************************************************************************
* SetSecondRecordCallback
*���������ø���¼��ص�
*���룺	pRecordCallback	-¼��ص�
*�������
*����ֵ����
*****************************************************************************/
void IMXCall::SetSecondRecordCallback(IMXEC_RecordCallback*pRecordCallback)
{
	return ;
}


/******************************************************************************
* SaveLocalMainVideoToBMPFile
*���������汾��������Ƶ��BMPͼƬ�ļ�
*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::SaveLocalMainVideoToBMPFile(const char*cszPathName)
{
	return IMXCallMgr::Instance().SaveLocalMainVideoToBMPFile(cszPathName);
}


/******************************************************************************
* SaveRemoteMainVideoToBMPFile
*����������Զ��������Ƶ��BMPͼƬ�ļ�
*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::SaveRemoteMainVideoToBMPFile(const char*cszPathName)
{
	if (m_pRemoteMainPlayChan!=NULL)
	{
		return m_pRemoteMainPlayChan->CaptureToBMPFile(cszPathName)?0:-1;
	}
	return -1;
}


/******************************************************************************
* SaveSecondVideoToBMPFile
*���������渨����Ƶ��BMPͼƬ�ļ�
*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXCall::SaveSecondVideoToBMPFile(const char*cszPathName)
{
	return -1;
}


#ifdef _TIANZHU_CACHE_
int IMXCall::SetRemoteVideoCache(int nCacheFrameCount)
{
	if (m_pRemoteMainPlayChan)
	{
		return m_pRemoteMainPlayChan->SetRemoteVideoCache(nCacheFrameCount);
	}
	return -1;
}
#endif
