//AVCONH264Header.h
#ifndef __AVCONH264HEADER_H__
#define __AVCONH264HEADER_H__

typedef struct tagAVCON_H264_HEADER
{
	unsigned short ID;
	unsigned short framerate:6;
	unsigned short reserved1:2;
	unsigned short reserved2:8;
}AVCON_H264_HEADER,*PAVCON_H264_HEADER;

#endif
