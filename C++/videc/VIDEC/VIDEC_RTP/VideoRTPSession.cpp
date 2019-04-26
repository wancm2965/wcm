#include "VideoRTPSession.h"

VideoRTPSession::VideoRTPSession(VideoRTPSessionCallback&rCallback)
:m_rCallback(rCallback)
{

}

VideoRTPSession::~VideoRTPSession(void)
{

}

void VideoRTPSession::OnPollThreadStep()
{
	BeginDataAccess();
		
	if (GotoFirstSourceWithData())
	{
		do
		{
			RTPPacket *pack;
			RTPSourceData *srcdat;
			
			srcdat = GetCurrentSourceInfo();
			
			while ((pack = GetNextPacket()) != NULL)
			{
				ProcessRTPPacket(*srcdat,*pack);
				DeletePacket(pack);
			}
		} while (GotoNextSourceWithData());
	}
		
	EndDataAccess();
}

void VideoRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	m_rCallback.OnVideoRTPSessionCallbackRTPPacket(this,rtppack.GetPacketData(),rtppack.GetPacketLength(),rtppack.GetPayloadType(),rtppack.GetPacketLength()-rtppack.GetPayloadLength(),rtppack.GetSequenceNumber(),rtppack.GetTimestamp());
}
