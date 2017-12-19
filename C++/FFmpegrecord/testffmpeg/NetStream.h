#pragma once
#include "NETECReceiveStream/INETECRecieveStream.h"
#include "FFmpegrecord/IFFmpegrecord.h"
#include "VIDEC/VIDEC_Header.h"
#include "AUDEC/AUDEC_Header.h"
#include "AUDEC/AUDEC_CodecID.h"
class CNetStream :
	public CNETECRecievestreamCallback
{
public:
	CNetStream();
	~CNetStream();
	void startNete();
	int openNete();
	void SetReadStreamNotify(IFFmpegrecord*precord);
	virtual void OnNETECRecievestreamCallbackVideoData(unsigned char*pData, int nLen);
	virtual void OnNETECRecievestreamCallbackAudioData(unsigned char*pData, int nLen);


private:
	INETECRecievestream* m_pNeteStream;
	IFFmpegrecord*       m_precord;
};

