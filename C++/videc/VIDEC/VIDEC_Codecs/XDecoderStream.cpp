#include <stdlib.h>
#include <memory.h>
#include "XDecoderStream.h"


VIDEC_API VIDEC_DecoderStream*VIDEC_DecoderStreamCreate(VIDEC_DecoderStreamCallback&rCallback)
{
	NEWRECORD3( XDecoderStream*,pXDecoderStream ,new XDecoderStream(rCallback) )
	NULLReturnRet( pXDecoderStream ,pXDecoderStream )  
	if (pXDecoderStream->Open()!=0)
	{
		pXDecoderStream->Close();
		delete pXDecoderStream;
		pXDecoderStream=NULL;
	}

	return pXDecoderStream;
}


XDecoderStream::XDecoderStream(VIDEC_DecoderStreamCallback&rCallback)
:m_rCallback(rCallback)
{
}

XDecoderStream::~XDecoderStream(void)
{
}

void XDecoderStream::OnDecoderStreamCallbackFrame(VIDEC_Image*pImage)
{
	m_rCallback.OnVIDEC_DecoderStreamCallbackFrame(pImage);
}
