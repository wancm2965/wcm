//XMixerInput.h
#ifndef __XMIXERINPUT_H__
#define __XMIXERINPUT_H__

#include "VIDEC_Mixer.h"
#include "XAutoLock.h"


class XMixer;
class XMixerInput
	: public XCritSec
{
public:
	XMixerInput(XMixer&rXMixer,int nRectIndex,bool bVisible);
	virtual~XMixerInput(void);
public:
	virtual void Close(void);

	virtual void SetRectIndex(int nIndex);
	virtual int GetRectIndex(void);
	virtual void SetVisible(bool bVisible);
	virtual bool IsVisible(void);
	
	virtual int InputImage(unsigned char*pData,int nLen,int nWidth,int nHeight);

	virtual unsigned char*GetImageData(void);
	virtual int GetImageSize(void);
	virtual int GetImageWidth(void);
	virtual int GetImageHeight(void);
protected:
	XMixer&			m_rXMixer;

	int				m_nRectIndex;
	bool			m_bVisible;

	unsigned char*	m_pImageBuf;
	int				m_pImageBufLen;
	int				m_nImageSize;
	int				m_nImageWidth;
	int				m_nImageHeight;
};


class XMixerImageInput
	: public VIDEC_MixerImageInput
	, public XMixerInput
{
public:
	XMixerImageInput(XMixer&rXMixer,int nRectIndex,bool bVisible);
	virtual~XMixerImageInput(void);
public:
	void Close(void);

	void SetRectIndex(int nIndex);
	int GetRectIndex(void);
	void SetVisible(bool bVisible);
	bool IsVisible(void);
	
	int InputImage(unsigned char*pData,int nLen,int nWidth,int nHeight);
};

class XMixerRTPInput
	: public VIDEC_MixerRTPInput
	, public XMixerInput
{
public:
	XMixerRTPInput(XMixer&rXMixer,int nRectIndex,bool bVisible);
	virtual~XMixerRTPInput(void);
public:
	void Close(void);

	void SetRectIndex(int nIndex);
	int GetRectIndex(void);
	void SetVisible(bool bVisible);
	bool IsVisible(void);
	
	int InputRTP(unsigned char*pData,int nLen);
};


class XMixerStreamInput
	: public VIDEC_MixerStreamInput
	, public XMixerInput
{
public:
	XMixerStreamInput(XMixer&rXMixer,int nRectIndex,bool bVisible);
	virtual~XMixerStreamInput(void);
public:
	void Close(void);

	void SetRectIndex(int nIndex);
	int GetRectIndex(void);
	void SetVisible(bool bVisible);
	bool IsVisible(void);
	
	int InputStream(unsigned char*pData,int nLen);
};

#endif