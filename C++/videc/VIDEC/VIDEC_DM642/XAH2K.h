// XAH2K.h: interface for the XAH2K class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XAH2K_H__
#define __XAH2K_H__

#include "VIDEC_Config.h"
#include "../VIDEC_API/XImage.h"
#include "VIDEC_AH2K.h"


#include "DM642DevEx.h"
#include "XThreadBase.h"
#include "VIDEC_convert.h"
#include "XAutoLock.h"

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
#define FRAME_DEINTERLACED	8


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
	Uint16		nReserved;
}PCI_VIDEO_FRAME,*PPCI_VIDEO_FRAME;

//typedef struct {
//	BASE_FRAME	base_frame;
//
//	Uint32		addr_pcm;
//	Uint32		len_pcm;
//	Uint32		sample_rate;
//	
//	Uint8		channels;
//	Uint8		reserved1;
//	Uint16		reserved2;
//}AUDIO_FRAME;

typedef struct {
	Uint8	enable;//0:DISABLE 1:CAP,2:ENC
	Uint8	scale_type;//0:F 1:I 2:Q
	Uint8	framerate;//1-60
	Uint8	channel_index;//0

	Uint16	width;
	Uint16	height;
}XAH_PCI2DSP,*PXAH_PCI2DSP;

#define MAX_IOBUF_COUNT		3

class XAH2K  
	: public VIDEC_AH2K
#ifdef __WWENCODER_H__
	, public WWEncoderChannelCallback
#endif
{
public:
	XAH2K(VIDEC_AH2KCallback&rCallback);
	virtual ~XAH2K(void);

	void ReadDataFromDsp(PIOBUF pIOBuf);
public:
	int Open(int nDevID,int nWidth,int nHeight,int nFrameRate);
	void Close(void);

	virtual int StartVideoStream(int nBitrate,int nBitrateControlType,int qmin,int qmax);
	virtual int RequestKeyFrame(void);
	virtual int StopVideoStream(void);

	virtual void DoStartVideoStream(void);
	virtual void DoStopVideoStream(void);

	virtual void OnCapturedAudio(short*pSamples,int nSamples,int nSampleRate);
	virtual void OnWWEncoderChannelCallbackPacketData(const void*pPacketData,int nPacketLen,int nWidth,int nHeight,int nHeaderLen);

	virtual bool IsExpired(void);
	virtual void Reset(void);
public:
	static void BootLoad(int nDevID);
	static void BootLoad(int nDevID,PBYTE pData,DWORD dwLen,DEV_EX_TYPE nDevType);
protected:
	VIDEC_AH2KCallback&	m_rCallback;

	DM642DevEx		*m_pDev;

	bool			m_bRunning;
	unsigned char	m_nScaleType;	//0:F 1:I 2:Q

	int				m_nActualWidth;
	int				m_nActualHeight;
	int				m_nVirtualWidth;
	int				m_nVirtualHeight;
	int				m_nFrameRate;
	int				m_nImageSize;

	XImage*			m_pXImage;
	PBYTE			m_pTempBuf;
	int				m_nBufLen;

	int				m_nChannelIndex;
	CHANNEL_TYPE	m_nChannelType;

	VIDEC_Resize*	m_pResize;
	unsigned short	m_usDstWidth;
	unsigned short	m_usDstHeight;

#ifdef __WWENCODER_H__
	XCritSec		m_csWWEncoderChannel;
	WWEncoderChannel*m_pWWEncoderChannel;
#endif
	bool			m_bStartStream;
	int				m_nBitrate;
	int				m_nBitrateControlType;
	int				m_nQMin;
	int				m_nQMax;


	int				m_nInputWidth;
	int				m_nInputHeight;
	int				m_nPortType;
	int				m_nInputFrameRate;

	unsigned long	m_nLastFrameTimestamp;
	int				m_nResetWidth;
	int				m_nResetHeight;
	int				m_nResetFrameRate;
};

#endif

