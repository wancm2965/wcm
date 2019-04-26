//AVC_Packet.h

#ifndef __AVC_PACKET_H__
#define __AVC_PACKET_H__

#if defined(WIN32) && defined(_WIN32)
#include <winsock2.h>
#endif

#define AVC_PACKET_AV_HEADER_LEN	8
#define AVC_PACKET_AVK_HEADER_LEN	13

#define AVC_PACKET_AV				0x01
#define AVC_PACKET_SUB				0x02
#define AVC_PACKET_KEY				0x04
#define AVC_PACKET_MASK				~(0x07)

#define IS_VALID_PACKET(p)			(AVC_PACKET_MASK==(AVC_PACKET_MASK&~(*(unsigned char*)p)))
#define IS_AUDIO_PACKET(p)			(AVC_PACKET_AV!=(AVC_PACKET_AV&(*(unsigned char*)p)))
#define IS_VIDEO_PACKET(p)			(AVC_PACKET_AV==(AVC_PACKET_AV&(*(unsigned char*)p)))
#define IS_SUB_PACKET(p)			(AVC_PACKET_SUB==(AVC_PACKET_SUB&(*(unsigned char*)p)))
#define IS_KEY_PACKET(p)			(AVC_PACKET_KEY==(AVC_PACKET_KEY&(*(unsigned char*)p)))

#define SET_AVC_PACKET(p)			(*(unsigned char*)p=0)
#define SET_AUDIO_PACKET(p)			(*(unsigned char*)p&=~AVC_PACKET_AV)
#define SET_VIDEO_PACKET(p)			(*(unsigned char*)p|=AVC_PACKET_AV)
#define SET_SUB_PACKET(p)			(*(unsigned char*)p|=AVC_PACKET_SUB)
#define SET_TCP_PACKET(p)			(*(unsigned char*)p&=~AVC_PACKET_SUB)
#define SET_KEY_PACKET(p)			(*(unsigned char*)p|=AVC_PACKET_KEY)

#define GET_PACKET_SEQUENCE(p)		(ntohs(*(unsigned short*)((unsigned char*)p+1)))
#define SET_PACKET_SEQUENCE(p,seq)	(*(unsigned short*)((unsigned char*)p+1)=htons(seq))

#define GET_PACKET_TIMESTAMP(p)		(ntohl(*(unsigned long*)((unsigned char*)p+3)))
#define SET_PACKET_TIMESTAMP(p,ts)	(*(unsigned long*)((unsigned char*)p+3)=htonl(ts))

#define GET_PACKET_CODEC(p)			(*(unsigned char*)((unsigned char*)p+7))
#define SET_PACKET_CODEC(p,codec)	((*(unsigned char*)((unsigned char*)p+7))=(unsigned char)codec)

#define GET_PACKET_FRAME_RATE(p)	(*(unsigned char*)((unsigned char*)p+8))
#define SET_PACKET_FRAME_RATE(p,fr)	((*(unsigned char*)((unsigned char*)p+8))=(unsigned char)fr)

#define GET_PACKET_WIDTH(p)			(ntohs(*(unsigned short*)((unsigned char*)p+9)))
#define SET_PACKET_WIDTH(p,w)		(*(unsigned short*)((unsigned char*)p+9)=htons(w))

#define GET_PACKET_HEIGHT(p)		(ntohs(*(unsigned short*)((unsigned char*)p+11)))
#define SET_PACKET_HEIGHT(p,h)		(*(unsigned short*)((unsigned char*)p+11)=htons(h))

#endif