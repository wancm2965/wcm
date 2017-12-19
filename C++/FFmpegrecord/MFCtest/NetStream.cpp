#include "stdafx.h"
#include "NetStream.h"
#define RTP_VERSION			0x02
#define RTP_HEADER_LEN		0x0C
CNetStream::CNetStream()
{
	m_bGetStream = false;
	m_bAudioStop = false;
	m_bVideoStop = false;
	m_nRtpTimestamp = 0;
	m_nRtpSeqNo = 0;
}

static int g_nVideoCount = 0;
static int g_nVideoCount1 = 0;
CNetStream::~CNetStream()
{
	if (m_pNeteStream)
	{
		m_pNeteStream->Close();
		m_pNeteStream = NULL;
		INETECRecievestream::Stop_NETECEngine();		
	}
}

void CNetStream::startNete()
{
	std::string strServerIP = "192.168.1.209";
	unsigned short usServerPort = 4222;
	INETECRecievestream::Start_NETECEngine(strServerIP.c_str(), usServerPort);
}

int CNetStream::openNete()
{
	std::string strUserID = "12312321";
	m_pNeteStream = INETECRecievestream::Create(strUserID.c_str(), this);
	if (m_pNeteStream)
	{
		unsigned long nVideoID = 4115582487;
		unsigned long nAudioID = 13884848;
		std::string strPeerNodeID = "0-6D9F581A-66EA3B1E@jx";
		std::string strMCUID = "MCU-001@jx";
		std::string strMCUIP = "192.168.1.209";
		unsigned short nMCUPort = 4222;
		int nResult = m_pNeteStream->Open(nAudioID, nVideoID, strPeerNodeID.c_str(), "", 0, "", 0, strMCUID.c_str(), strMCUIP.c_str(), nMCUPort);
		if (nResult > -1)
		{
			m_pNeteStream->EnableAudio(1);
			m_pNeteStream->EnableVideo(1);
		}
		return nResult;
	}
	return -1;
}

void CNetStream::SetReadStreamNotify(IFFmpegrecord*precord)
{
	if (precord)
	{
		m_bGetStream = true;
		m_precord = precord;	
		m_bAudioStop = false;
		m_bVideoStop = false;
	}
	else
	{
		m_bGetStream = false;
		
	}
}

void CNetStream::OnNETECRecievestreamCallbackVideoData(unsigned char*pData, int nLen)
{
// 	g_nVideoCount1++;
// 	SYSTEMTIME currentTime;
// 	::GetLocalTime(&currentTime);
// 	CString strOut;
// 	strOut.Format(_T("[%.2d%.2d%.2d%.2d]NETECRecievestreamVideoCount is %d !\n"), currentTime.wHour, currentTime.wMinute, currentTime.wSecond, currentTime.wMilliseconds, g_nVideoCount1);
// 	OutputDebugString(strOut);
// 	if (!m_bStartThread)
// 	{
// 		m_bStartThread = true;
// 		StartThread();
// 	}
//	if (!m_bStartRecord)
//	{
// 		g_nVideoCount++;
// 		SYSTEMTIME currentTime;
// 		::GetLocalTime(&currentTime);
// 		CString strOut;
// 		strOut.Format(_T("[%.2d%.2d%.2d]VideoCount is %d !\n"), currentTime.wHour, currentTime.wMinute, currentTime.wSecond, g_nVideoCount);
// 		OutputDebugString(strOut);
		//return;
//	}
//	
//	if (!m_bWriteHeader)
//	{
// 		m_bWriteHeader = true;
// 		
// 		if (m_precord)
// 		{
// 			m_precord->WriteHeader(640, 480, 7, 48000, 2);
// 		}
// 		if (m_pNeteStream)
// 		{
// 			m_pNeteStream->RequestKeyFrame();
// 		}
// 		return;
//   }
//	
// 	SYSTEMTIME currentTime;
// 	::GetLocalTime(&currentTime);
// 	CString strOut;
// 	strOut.Format(_T("[%.2d%.2d%.2d]VideoCount is %d !\n"), currentTime.wHour, currentTime.wMinute, currentTime.wSecond, g_nVideoCount);
// 	OutputDebugString(strOut);

	if (m_bGetStream)
	{

		if (m_precord)
		{
		 	int nHeaderLen = VIDEC_HEADER_EXT_GET_LEN((void*)pData);
			int nKeyFrame = VIDEC_HEADER_EXT_GET_KEYFRAME(pData);
			int nWidth = VIDEC_HEADER_EXT_GET_VIRTUAL_WIDTH(pData);
			int nHeight = VIDEC_HEADER_EXT_GET_VIRTUAL_HEIGHT(pData); 		
			unsigned long nTimestamp = VIDEC_HEADER_EXT_GET_TIMESTAMP((void *)pData);
			m_precord->WriteVideo(pData + nHeaderLen, nLen - nHeaderLen, nTimestamp, nKeyFrame == 1 ? true:false, nWidth,nHeight);
		}
	}
	else
	{
		m_bVideoStop = true;
		if (m_bVideoStop && m_bAudioStop)
		{
			if (m_precord)
			{
				m_precord->Close();
			}
			m_precord = NULL;
		}
	}
}

void CNetStream::OnNETECRecievestreamCallbackAudioData(unsigned char*pData, int nLen)
{
// 	g_nVideoCount++;
// 	SYSTEMTIME currentTime;
// 	::GetLocalTime(&currentTime);
// 	CString strOut;
// 	strOut.Format(_T("[%.2d%.2d%.2d]AudioCount is %d !\n"), currentTime.wHour, currentTime.wMinute, currentTime.wSecond, g_nVideoCount);
// 	OutputDebugString(strOut);
	if (m_bGetStream)
	{

		if (m_precord)
		{
		 	int nHeaderLen = AUDEC_HEADER_GET_LEN((void*)pData);
			unsigned long nTimestamp = AUDEC_HEADER_GET_TIMESTAMP((void *)pData);
			m_precord->WriteAudio(pData + nHeaderLen, nLen - nHeaderLen, nTimestamp);
		}

//      /*给AAC流添加ADTS头*/
// 		if (m_fAudio)
// 		{
// 			X_AUDIO_CODEC_ID xid = (X_AUDIO_CODEC_ID)AUDEC_HEADER_GET_CODEC_ID(pData);
// 			int nID = xid;
// 			int nHeaderLen = AUDEC_HEADER_GET_LEN((void*)pData);
// 			unsigned char adtsHeader[7] = { 0 };
// 
// 			int profile = 2;
// 			int freqIdx = 4;
// 			int chanCfg = 2;
// 			int packetLen = nLen - nHeaderLen + 7;
// 			adtsHeader[0] = 0xFF;
// 			adtsHeader[1] = 0xF1;
// 			adtsHeader[2] = ((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2);
// 			adtsHeader[3] = ((chanCfg & 3) << 6) + (packetLen >> 11);
// 			adtsHeader[4] = (packetLen & 0x7ff) >> 3;
// 			adtsHeader[5] = ((packetLen & 0x7) << 5) + 0x1f;
// 			adtsHeader[6] = 0xFC;
// 			fwrite(adtsHeader, 1, sizeof(adtsHeader), m_fAudio);
// 			fwrite(pData + nHeaderLen, 1, nLen - nHeaderLen, m_fAudio);		
// 		}

	}
	else
	{
		m_bAudioStop = true;
		if (m_bVideoStop && m_bAudioStop)
		{
			if (m_precord)
			{
				m_precord->Close();
			}
			m_precord = NULL;
		}
	}
	
}

int CNetStream::AudecHeaderToRTPHeader(char* pRTPPacket, char* pData, int nLen)
{
	X_AUDIO_CODEC_ID nCodecID = (X_AUDIO_CODEC_ID)AUDEC_HEADER_GET_CODEC_ID(pData);

	std::string strCodecName = "gsm";

	int nPayloadType = 0;
	int nSamplesPerFrame = 160;

	switch (nCodecID)
	{
	case X_AUDIO_CODEC_GSM:
	{
		strCodecName = "gsm";
		//nSampleRate = 8000;
		//nBitrate = 13200;
		nPayloadType = 3;
		nSamplesPerFrame = 160;
	}
	break;
	case X_AUDIO_CODEC_G711U:
	{
		strCodecName = "PCMU";
		//nSampleRate = 8000;
		//nBitrate = 64000;
		nPayloadType = 0;
		nSamplesPerFrame = 160;
	}
	break;
	case X_AUDIO_CODEC_G711A:
	{
		strCodecName = "PCMA";
		//nSampleRate = 8000;
		//nBitrate = 64000;
		nPayloadType = 8;
		nSamplesPerFrame = 160;
	}
	break;
	case X_AUDIO_CODEC_G722_64:
	{
		strCodecName = "G722";
		//nSampleRate = 16000;
		//nBitrate = 64000;
		nPayloadType = 9;
		nSamplesPerFrame = 160;
	}
	break;
	case X_AUDIO_CODEC_SIREN_G7221_7_24://16khz
	case X_AUDIO_CODEC_G7221_7_24://16khz
	{
		strCodecName = "g7221";
		//nSampleRate = 16000;
		//nBitrate = 24000;
		nPayloadType = 121;
		nSamplesPerFrame = 320;
	}
	break;
	case X_AUDIO_CODEC_SIREN_G7221_7_32://16khz
	case X_AUDIO_CODEC_G7221_7_32://16khz
	{
		strCodecName = "g7221";
		//nSampleRate = 16000;
		//nBitrate = 32000;
		nPayloadType = 122;
		nSamplesPerFrame = 320;
	}
	break;
	case X_AUDIO_CODEC_SIREN_G7221_14_24:
	case X_AUDIO_CODEC_G7221_14_24://32khz
	{
		strCodecName = "g7221";
		//nSampleRate = 32000;
		//nBitrate = 24000;
		nPayloadType = 123;
		nSamplesPerFrame = 640;
	}
	break;
	case X_AUDIO_CODEC_SIREN_G7221_14_32:
	case X_AUDIO_CODEC_G7221_14_32:
	{
		strCodecName = "g7221";
		//nSampleRate = 32000;
		//nBitrate = 32000;
		nPayloadType = 124;
		nSamplesPerFrame = 640;
	}
	break;
	case X_AUDIO_CODEC_G7221_14_48:
	{
		strCodecName = "g7221";
		//nSampleRate = 32000;
		//nBitrate = 48000;
		nPayloadType = 125;
		nSamplesPerFrame = 640;
	}
	break;
	case X_AUDIO_CODEC_G719_32:
	{
		strCodecName = "g719";
		//nSampleRate = 48000;
		//nBitrate = 32000;
		nPayloadType = 255;  //
		nSamplesPerFrame = 960;
	}
	break;
	case X_AUDIO_CODEC_OPUS_FWB_48:
	{
		strCodecName = "opus";
		//nSampleRate = 48000;
		//nBitrate = 64000;
		nPayloadType = 120;
		nSamplesPerFrame = 960;
	}
	break;
	case X_AUDIO_CODEC_AAC:
	{
		strCodecName = "opus";
		//nSampleRate = 48000;
		//nBitrate = 64000;
		nPayloadType = 120;
		nSamplesPerFrame = 960;
	}
	break;
	default:
		return -1;
		break;
	};


	int nHeaderLen = AUDEC_HEADER_GET_LEN(pData);
	int nPayloadLen = nLen - nHeaderLen;
	if (nPayloadLen <= 0 || (nPayloadLen + RTP_HEADER_LEN) > 1500)
	{
		return -1;
	}
	unsigned short nSeqNo = AUDEC_HEADER_GET_SEQUENCE(pData);
	unsigned int nTimestamp = AUDEC_HEADER_GET_TIMESTAMP(pData);

	PRTP_HEADER pRTPHeader = (PRTP_HEADER)pRTPPacket;
	memset(pRTPHeader, 0, RTP_HEADER_LEN);
	pRTPHeader->version = RTP_VERSION;
	pRTPHeader->type = nPayloadType;
	pRTPHeader->marker = AUDEC_HEADER_GET_SYN_POINT(pData);
	pRTPHeader->timestamp = htonl(m_nRtpTimestamp);
	pRTPHeader->ssrc = htonl((unsigned long)this);
	pRTPHeader->sequence = htons(++m_nRtpSeqNo);
	m_nRtpTimestamp += nSamplesPerFrame;

	memcpy(pRTPPacket + RTP_HEADER_LEN, pData + nHeaderLen, nPayloadLen);
	return nPayloadLen + RTP_HEADER_LEN;
}
