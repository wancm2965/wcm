//BaseRTP2Frame.h
#ifndef __BASERTP2FRAME_H__
#define __BASERTP2FRAME_H__

#include "XListPtr.h"
#include "XAutoLock.h"
#include "XSocket.h"
#include "RTPHeader.h"



class BaseRTP2Frame;
class BaseRTPFrameCallback
{
public:
	BaseRTPFrameCallback(void){};
	virtual~BaseRTPFrameCallback(void){};
public:
	virtual void OnBaseRTPFrameCallbackFramePacket(BaseRTP2Frame*pBaseRTPFrame,unsigned char*pPacketData,int nPacketLen,int nKeyFrame){};
};

class BaseRTP2Frame
{
public:
	BaseRTP2Frame(BaseRTPFrameCallback&rCallback);
	virtual~BaseRTP2Frame(void);
public:
	virtual int Open(int nHeaderMargin=0);
	virtual void Close(void);
	virtual int GetFrame(unsigned char*pFrameBuf,int nBufLen);
	virtual void OnRecvdRTPPacket(void*pPacketData,int nPacketLen);
protected:
	virtual void FlushRTPPackets(void);
	virtual int CalculateFrameSize(void);
	virtual int handle_rtp_packet(unsigned char*pPayloadData,int nPayloadLen,unsigned char*pFrameBuf,int nBufLen)=0;
protected:
	BaseRTPFrameCallback&	m_rCallback;

	unsigned char*			m_pFrameBuf;
	int						m_nFrameBufLen;

	int						m_nHeaderMargin;

	unsigned long			m_nLastRecvdRTPTime;
	bool					m_bGotFrame;
	int						m_nKeyFrame;
protected:
	class RTPFrame
	{
	public:
		RTPFrame(void)
			: pBuffer(NULL)
			, nLen(0)
			, bFirstPacket(false)
		{
		}
		virtual~RTPFrame(void)
		{
			if(pBuffer)
			{
				free(pBuffer);
				pBuffer=NULL;
			}
		}
	public:
		void Release(void)
		{
			if(pBuffer)
			{
				free(pBuffer);
				pBuffer=NULL;
			}

			delete this;
		}

		void*	pBuffer;
		int		nLen;
		bool	bFirstPacket;
	};

	XListPtr		m_ListRTPPacket;
	XCritSec		m_csListRTPPacket;
protected:
	virtual void PacketsToFrame(void);
};

#endif