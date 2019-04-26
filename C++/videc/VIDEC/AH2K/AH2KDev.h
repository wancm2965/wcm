#ifndef __AH2KDEV_H__
#define __AH2KDEV_H__

#include "AH2KAPI.h"

#include "PcieDevEx.h"

typedef enum 
{
	CHANNEL_TYPE_SD=0,
	CHANNEL_TYPE_VGA,
	CHANNEL_TYPE_HD,
}CHANNEL_TYPE;

#define FRAME_VERSION		0x01

#define FRAME_AUDIO			0
#define FRAME_I				1
#define FRAME_P				2
#define FRAME_VIDEO			4
#define FRAME_VIDEO_NO_SIGNAL	8
#define FRAME_VIDEO_AUDIO		16

#define MAX_AUDIO_LENGTH		45


typedef unsigned char	Uint8;
typedef unsigned short	Uint16;
typedef unsigned int	Uint32;

typedef struct {
	Uint8 	version;
	Uint8	frame_type;
	Uint8	channel;
	Uint8	framerate;
}PCI_BASE_FRAME,*PPCI_BASE_FRAME;

typedef struct {
	PCI_BASE_FRAME	base_frame;

	Uint16		nOutputWidth;
	Uint16		nOutputHeight;

	Uint16		nInputWidth;
	Uint16		nInputHeight;
	Uint8		nInputFrameRate;
	Uint8		nPortType;
	Uint8		nInputFrameType;
	Uint8		nReserved;
}PCI_VIDEO_FRAME,*PPCI_VIDEO_FRAME;


typedef struct {
	Uint8	enable;//0:DISABLE 1:CAP,2:ENC
	Uint8	scale_type;//0:F 1:I 2:Q
	Uint8	framerate;//1-60
	Uint8	channel_index;//0

	Uint16	width;
	Uint16	height;
}XAH_PCI2DSP,*PXAH_PCI2DSP;

//repeat define 
//#define MAX_IOBUF_COUNT		3

class AH2KDev  
{
public:
	AH2KDev(void);
	virtual ~AH2KDev(void);

	void ReadDataFromDsp(PIOBUF pIOBuf);
public:
	int Open(int nDevID,AH2K_VIDEO_CB video_cb,AH2K_AUDIO_CB audio_cb,const void*pUserData);
	void Close(void);
	int SetParams(int nScaleType,int nWidth,int nHeight,int nFrameRate);
	int HasVideoSignal(void){return m_nHasVideoSignal;}


	virtual void OnCapturedAudio(short*pSamples,int nSamples,int nSampleRate);

	virtual bool IsExpired(void);
	virtual void Reset(void);
public:
	static void BootLoad(int nDevID);
	static void BootLoad(int nDevID,PBYTE pData,DWORD dwLen,DEV_EX_TYPE nDevType);
protected:
	PcieDevEx		*m_pDev;

	unsigned char	m_nScaleType;	//0:F 1:I 2:Q
	int				m_nHasVideoSignal;


	int				m_nBufLen;

	int				m_nChannelIndex;
	CHANNEL_TYPE	m_nChannelType;

	unsigned long	m_nLastFrameTimestamp;
	int				m_nResetWidth;
	int				m_nResetHeight;
	int				m_nResetFrameRate;

	AH2K_VIDEO_CB	m_video_cb;
	AH2K_AUDIO_CB	m_audio_cb;
	const void*		m_pUserData;
};



#endif