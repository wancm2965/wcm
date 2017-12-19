#include "stdafx.h"
#include "NetStream.h"


CNetStream::CNetStream()
{
}


CNetStream::~CNetStream()
{
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
		unsigned long nVideoID = 38267241;
		unsigned long nAudioID = 1575892854;
		std::string strPeerNodeID = "0-216E9FD-D868752B@jx";
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
	m_precord = precord;
}

void CNetStream::OnNETECRecievestreamCallbackVideoData(unsigned char*pData, int nLen)
{
	if (m_precord)
	{
		int nHeaderLen = VIDEC_HEADER_EXT_GET_LEN((void*)pData);
		int nkeyFrame = VIDEC_HEADER_EXT_GET_KEYFRAME((void*)pData);
		int nWidth = (int)VIDEC_HEADER_EXT_GET_ACTUAL_WIDTH(pData);
		int nHeight = (int)VIDEC_HEADER_EXT_GET_ACTUAL_HEIGHT(pData);
		unsigned long nTimestamp = VIDEC_HEADER_EXT_GET_TIMESTAMP((void *)pData);
		m_precord->WriteVideo(pData + nHeaderLen, nLen - nHeaderLen, nTimestamp, nkeyFrame, nWidth, nHeight);
	}
}

void CNetStream::OnNETECRecievestreamCallbackAudioData(unsigned char*pData, int nLen)
{
	if (m_precord)
	{
		int nHeaderLen = AUDEC_HEADER_GET_LEN((void*)pData);
		unsigned long nTimestamp = AUDEC_HEADER_GET_TIMESTAMP((void *)pData);
		m_precord->WriteAudio(pData + nHeaderLen, nLen - nHeaderLen, nTimestamp);
	}
}
