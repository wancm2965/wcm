/*
	video_proc.h

	2008.8.12
*/

#ifndef __VIDEO_PROC_H__
#define __VIDEO_PROC_H__


#define PORT_TYPE_CVBS		0
#define PORT_TYPE_DVI		1
#define PORT_TYPE_HDMI		2
#define PORT_TYPE_SDI		3
#define PORT_TYPE_SVIDEO	4
#define PORT_TYPE_VGA		5


#define BITRATE_CBR			0
#define BITRATE_VBR 		1

#define FRAME_VERSION		0x01

#define FRAME_AUDIO			0
#define FRAME_I				1
#define FRAME_P				2
#define FRAME_VIDEO			4
#define FRAME_VIDEO_NO_SIGNAL 8

#define _NUM_PIXELS			704
#define _NUM_LINES			576

#define NUM_PCI_CONTROL		2

typedef struct {
	Uint8 	version;
	Uint8	frame_type;
	Uint8	channel;
	Uint8	framerate;
}BASE_FRAME,*PBASE_FRAME;

typedef struct {
	BASE_FRAME	base_frame;

	Uint16		nOutputWidth;
	Uint16		nOutputHeight;

	Uint16		nInputWidth;
	Uint16		nInputHeight;
	Uint8		nInputFrameRate;
	Uint8		nPortType;
	Uint8		nInputFrameType;
	Uint8		nReserved;
}VIDEO_FRAME,*PVIDEO_FRAME;

typedef struct {
	BASE_FRAME	base_frame;

	Uint32		addr_pcm;
	Uint32		len_pcm;
	Uint32		sample_rate;
	
	Uint8		channels;
	Uint8		reserved1;
	Uint16		reserved2;
}AUDIO_FRAME,*PAUDIO_FRAME;

typedef struct {
	Uint8		channel;
	Uint8		flag_enable_cap;

	Uint8		framerate;
	Uint8		scale_type;
	Uint8		video_mode;
	
	Uint16		enc_width;
	Uint16		enc_height;

	Uint16		width;
	Uint16		height;
	
	Uint32		frame_count;


	Uns			bWriteDone;

	Ptr			pPCIData;
	Int			nPCIDataLen;

	Ptr			pPCIControl[NUM_PCI_CONTROL];
	int			numPCIControl;
// add by chenzhenbao
	int         curPCIControl;
    Ptr         m_yIn;
	Ptr         m_uIn;
	Ptr         m_vIn;
	int         m_420;
	Bool        vp_chan_interlaced;
	Bool 		g_VpIsVga;
	
}VID_CHAN_INFO,*PVID_CHAN_INFO;

void video_proc_init(void);
void vid_chan_process(PVID_CHAN_INFO pvi, Uint32 yIn, Uint32 uIn, Uint32 vIn, Uint8 actual_framerate,Uint8 nPortType,Uint16 nInputWidth,Uint16 nInputHeight,Bool nosignel);
PVID_CHAN_INFO vid_chan_init(Uint32 vid, Int maxWidth, Int maxHeight, Int numPCIControl);

/* heap IDs defined in the BIOS configuration file */
extern Int EXTERNALHEAP;
extern Int INTERNALHEAP;

void vid_chan_process_old(PVID_CHAN_INFO pvi, Uint32 yIn, Uint32 uIn, Uint32 vIn, Uint8 actual_framerate, Uint8 nPortType, Uint16 nInputWidth, Uint16 nInputHeight);
#endif
