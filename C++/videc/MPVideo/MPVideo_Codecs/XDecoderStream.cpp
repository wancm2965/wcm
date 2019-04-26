#include <stdlib.h>
#include <memory.h>
#include "XDecoderStream.h"


MPVIDEO_API MPVideo_DecoderStream*MPVideo_DecoderStreamCreate(MPVideo_DecoderStreamCallback&rCallback)
{
	XDecoderStream*pXDecoderStream=new XDecoderStream(rCallback);
	if (pXDecoderStream->Open()!=0)
	{
		pXDecoderStream->Close();
		delete pXDecoderStream;
		pXDecoderStream=NULL;
	}

	return pXDecoderStream;
}


XDecoderStream::XDecoderStream(MPVideo_DecoderStreamCallback&rCallback)
:m_rCallback(rCallback)
{
}

XDecoderStream::~XDecoderStream(void)
{
}

void XDecoderStream::OnDecoderStreamCallbackFrame(MPVideo_Image*pImage)
{
	m_rCallback.OnMPVideo_DecoderStreamCallbackFrame(pImage);
}
