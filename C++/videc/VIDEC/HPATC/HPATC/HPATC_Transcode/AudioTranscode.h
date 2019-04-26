// AudioTranscode.h: interface for the AudioTranscode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AUDIOTRANSCODE_H__
#define __AUDIOTRANSCODE_H__


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string>

#include "../../include/AUDEC_CodecID.h"
#include "../../include/AUDEC_Header.h"
#include "../../include/HPATC_Transcode.h"
#include "../../include/HPATC_Codec.h"


#ifdef _WIN32

#include <winsock2.h>
#include <WS2TCPIP.H>
#include <errno.h>
#pragma comment(lib,"ws2_32.lib")

#else 

#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#endif /* _WIN32 */

//#include "XUtil.h"
//#include <map>
//#include "XThreadBase.h"
//#include "XDataBuffer.h"
//#include "XAutoLock.h"
//
//typedef std::map<unsigned short,XDataBuffer*> MapAudioPacket;

class AudioTranscode
	: public HPATC_Transcode
	, public HPATCDecoderNotify
	, public HPATCEncoderNotify
	//, public XThreadBase
{
public:
	AudioTranscode(HPATC_TranscodeCallback*pCallback);
	AudioTranscode(void*pUserData,HPATC_CALLBACK_AAC_DATA cb_aac_data);
	virtual ~AudioTranscode(void);
public:
	virtual int Open(int nSampleRate,int nSamplesPerFrame);
	virtual void Close(void);
	/*输入：pPacketData，带AUDEC音频包头的音频数据；nPacketLen音频数据数据长度；
	 *输出：0：转码成功；非0：转码失败；
	 *回调：AAC编码裸数据 */
	virtual int TranscodePacketData(void*pPacketData,int nPacketLen);
	virtual int TranscodePCMData(short*pSamples,int nSamples,int nSampleRate,unsigned long ulTimestamp);

	virtual void OnHPATCDecoderNotifyOutput(short*pSamples,int nSamples,int nSampleRate,bool bDiscontinious,unsigned long ulTimestamp);
	virtual void OnHPATCEncoderNotifyOutput(unsigned char*pEncoded,int nLen,unsigned long ulTimestamp);

	//virtual void ThreadProcMain(void);
	int DoTranscodePacketData(char*pPacketData,int nPacketLen,bool bDiscontinious);
protected:
	HPATC_TranscodeCallback*	m_pCallback;
	HPATC_CALLBACK_AAC_DATA		m_cb_aac_data;
	void*						m_pUserData;
	//bool						m_bWaitForStop;
	//MapAudioPacket				m_mapAudioPacket;
	//XCritSec					m_csMapAudioPacket;
	unsigned short				m_nSequence;
	//unsigned int				m_nTimeStamp;

	HPATCResample*				m_pHPATCResample;
	HPATCEncoder*				m_pHPATCEncoder;	
	HPATCDecoder*				m_pHPATCDecoder;	
	int							m_nDecodeSampleRate;
	int							m_nEncodeSampleRate;
	unsigned char*				m_pResampleBuf;
	int							m_nResampleBufLen;
	X_AUDIO_CODEC_ID			m_nDecodeCodecID;


};



#endif 