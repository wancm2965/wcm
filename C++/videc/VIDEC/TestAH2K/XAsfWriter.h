//XAsfWriter.h
#ifndef __XASFWRITER_H__
#define __XASFWRITER_H__

#include <wmsdk.h>
#include <mmsystem.h>

#include "AVCONEC_ASFWriter.h"

class XAsfWriter
	: public AVCONEC_ASFWriter
{
public:
	XAsfWriter(void);
	virtual ~XAsfWriter(void);
public:
	int Open(const WCHAR*pFile,FOURCC fourcc,int nWidth,int nHeight,int nFrameRate,int nBitRate);
	void Close(void);

	int WriteVideo(unsigned char*pData,int nLen,bool bKeyFrame,unsigned long ulTimestamp);
protected:
	HRESULT CreateStream(FOURCC fourcc,int nWidth,int nHeight,int nFrameRate,int nBitRate);
	HRESULT CreateWriter(void);
	HRESULT ConfigWriter(void);
	HRESULT WriteHeader(const WCHAR * pwszName, WCHAR * pValue);
protected:
	IWMProfileManager *	mProfileManager;
	IWMProfile *		mProfile;	
	IWMWriter*			mWriter;
	IWMHeaderInfo*		mIHeaderInfo;
	IWMWriterAdvanced*	mIWriterAdvanced;	
	IWMWriterAdvanced3*	mIWriterAdvanced3;

	// Current stream time
	REFERENCE_TIME		mAudioTime;
	REFERENCE_TIME		mVideoTime;

	WCHAR				mDestFile[4096];
};

#endif