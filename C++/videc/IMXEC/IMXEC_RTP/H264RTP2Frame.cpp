
#include "H264RTP2Frame.h"


H264RTP2Frame::H264RTP2Frame(BaseRTPFrameCallback&rCallback)
:BaseRTP2Frame(rCallback)
{
}

H264RTP2Frame::~H264RTP2Frame(void)
{

}

int H264RTP2Frame::handle_rtp_packet(unsigned char*buf,int len,unsigned char*pFrameBuf,int nBufLen)
{
    unsigned char nal = buf[0];
    unsigned char type = (nal & 0x1f);
    int result= 0;
    unsigned char start_sequence[]= {0, 0, 0, 1};

	if (type==5 || type==7 || type==8)
	{
		m_nKeyFrame=1;
	}

    if (type >= 1 && type <= 23)
        type = 1;              // simplify the case. (these are all the nal types used internally by the h264 codec)
    switch (type) 
	{
    case 1:
        memcpy(pFrameBuf, start_sequence, sizeof(start_sequence));
        memcpy(pFrameBuf+sizeof(start_sequence), buf, len);
		result=sizeof(start_sequence)+len;
        break;
    case 28:                   // FU-A (fragmented nal)
        buf++;
        len--;                  // skip the fu_indicator
        {
            // these are the same as above, we just redo them here for clarity...
            unsigned char fu_indicator = nal;
            unsigned char fu_header = *buf;   // read the fu_header.
            unsigned char start_bit = fu_header >> 7;
//            unsigned char end_bit = (fu_header & 0x40) >> 6;
            unsigned char nal_type = (fu_header & 0x1f);
            unsigned char reconstructed_nal;

            // reconstruct this packet's true nal; only the data follows..
            reconstructed_nal = fu_indicator & (0xe0);  // the original nal forbidden bit and NRI are stored in this packet's nal;
            reconstructed_nal |= nal_type;

            // skip the fu_header...
            buf++;
            len--;

            if(start_bit) 
			{
                // copy in the start sequence, and the reconstructed nal....
                memcpy(pFrameBuf, start_sequence, sizeof(start_sequence));
                pFrameBuf[sizeof(start_sequence)]= reconstructed_nal;
                memcpy(pFrameBuf+sizeof(start_sequence)+sizeof(nal), buf, len);
				result=sizeof(start_sequence)+sizeof(nal)+len;
            } 
			else 
			{
                memcpy(pFrameBuf, buf, len);
				result=len;
            }
        }
        break;
    default:
        result= 0;
        break;
    }

    return result;
}
