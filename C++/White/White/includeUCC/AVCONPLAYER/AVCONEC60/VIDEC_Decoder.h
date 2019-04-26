//VIDEC_Decoder.h
#ifndef __VIDEC_DECODER_H__
#define __VIDEC_DECODER_H__

class VIDEC_DecoderNotify
{
public:
	VIDEC_DecoderNotify(void){};
	virtual~VIDEC_DecoderNotify(void){};
public:
	virtual void OnVIDEC_DecoderNotifyImageChanged(unsigned short usWidth,unsigned short usHeight,unsigned char usFrameRate,bool bHalfScaled)=0;
	virtual void OnVIDEC_DecoderNotifyImageData(unsigned char*pData,int nLen)=0;
};

class VIDEC_Decoder
{
public:
	VIDEC_Decoder(void){};
	virtual~VIDEC_Decoder(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual void DecodePacket(unsigned char*pData,int nLen)=0;
	virtual unsigned long GetTotalLostCount(void)=0;
	virtual double GetBitrate(void)=0;
	virtual int GetPacketCount(void)=0;
};

AFX_EXT_CLASS VIDEC_Decoder*CreateVIDEC_Decoder(VIDEC_DecoderNotify&rVIDEC_DecoderNotify);

#endif