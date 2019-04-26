//HPDecoderChannel.h
#ifndef __HPDECODERCHANNEL_H__
#define __HPDECODERCHANNEL_H__

class IHPDecoderChannelNotify
{
public:
	IHPDecoderChannelNotify(void){};
	virtual~IHPDecoderChannelNotify(void){};
public:
	virtual void OnDecoderNotifyImageChanged(unsigned short usWidth,unsigned short usHeight,unsigned char usFrameRate)=0;
	virtual void OnDecoderNotifyImageData(unsigned char*pData,int nLen)=0;
};

class IHPDecoderChannel
{
public:
	IHPDecoderChannel(void){};
	virtual~IHPDecoderChannel(void){};
public:
	virtual bool Connect(void)=0;
	virtual void Release(void)=0;
	virtual void DecodePacket(unsigned char*pData,int nLen,unsigned short usWidth,unsigned short usHeight,unsigned char usFrameRate)=0;
	virtual bool EnableVideoOutput(unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex)=0;
	virtual bool DisableVideoOutput(unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex)=0;
};

AFX_EXT_CLASS IHPDecoderChannel*CreateDecoderChannel(int nMDType, IHPDecoderChannelNotify&rNotify, unsigned long ulDecoderChannelIndex);

AFX_EXT_API void InitMDCard(void);
AFX_EXT_API void UnInitMDCard(void);
AFX_EXT_API int GetDisplayCount(int nMDType);
AFX_EXT_API int GetDecoderCount(int nMDType);
AFX_EXT_API bool EnableDecoderVideoOutput(int nMDType, unsigned long ulDecoderChannelIndex,unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex);
AFX_EXT_API bool DisableDecoderVideoOutput(int nMDType, unsigned long ulDecoderChannelIndex,unsigned long ulDisplayChannelIndex,unsigned long ulRectIndex);

#endif//__HPDECODERCHANNEL_H__