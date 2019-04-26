//#include "stdafx.h"
#include "DecoderRenderer.h"
#include "IntelDecoderRenderer.h"

DecoderRenderer*DecoderRenderer::Create(DecoderRendererCallback&rCallback)
{
	return new IntelDecoderRenderer(rCallback);
}
