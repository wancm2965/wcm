//XMixer.h
#ifndef __XMIXER_H__
#define __XMIXER_H__

#include <map>

#include "VIDEC_Mixer.h"
#include "XOutputImage.h"
#include "XOutputStream.h"
#include "XOutputRTP.h"
#include "XThreadBase.h"
#include "XImageMixer.h"
#include "XMixerInput.h"

typedef std::map<void*,XMixerInput*>	MapXMixerInput;

class XMixer
	: public VIDEC_Mixer
	, public XOutputImageMgr
	, public XOutputStreamMgr
	, public XOutputRTPMgr
	, public XThreadBase
	, public XImageMixer
{
public:
	XMixer(VIDEC_MixerNotify&rMixerNotify);
	virtual~XMixer(void);
public:
	int Open(int nWidth,int nHeight);
	void Close(void);
	
	int CreateMixerImageInput(VIDEC_MixerImageInput**ppMixerImageInput,int nRectIndex,bool bVisible);
	int CreateMixerRTPInput(VIDEC_MixerRTPInput**ppMixerRTPInput,int nRectIndex,bool bVisible);
	int CreateMixerStreamInput(VIDEC_MixerStreamInput**ppMixerStreamInput,int nRectIndex,bool bVisible);

	void RemoveXMixerInput(XMixerInput*pXMixerInput);

	int AddOutputImage(VIDEC_OutputImage*pOutputImage);
	void RemoveOutputImage(VIDEC_OutputImage*pOutputImage);

	int AddOutputStream(VIDEC_OutputStream*pOutputStream);
	void RemoveOutputStream(VIDEC_OutputStream*pOutputStream);
	
	int AddOutputRTP(VIDEC_OutputRTP*pOutputRTP);
	void RemoveOutputRTP(VIDEC_OutputRTP*pOutputRTP);

	int OutputImage(void*pData,int nLen,int nWidth,int nHeight);
protected:
	void ThreadProcMain(void);
protected:
	VIDEC_MixerNotify&	m_rMixerNotify;
	bool				m_bRunning;

	MapXMixerInput		m_MapXMixerInput;
	XCritSec			m_csMapXMixerInput;
};


#endif