//DecoderRenderer.h
#ifndef __DECODERRENDERER_H__
#define __DECODERRENDERER_H__
#include <windows.h>
class DecoderRendererCallback
{
public:
	virtual void OnDecoderRendererCallbackDraw(HDC hDC){};
};
class DecoderRenderer
{
public:
	DecoderRenderer(void){};
	virtual~DecoderRenderer(void){};
public:
	virtual int Open(HWND hWnd)=0;
	virtual void Close(void)=0;
	virtual int DecodeFrame(unsigned char*pFrameData,int nLen)=0;
	virtual int GetWidth(void)=0;
	virtual int GetHeight(void)=0;
	virtual int SetAMP(int nBrightness,int nHue,int nSaturation,int nContrast)=0;

	static DecoderRenderer*Create(DecoderRendererCallback&rCallback);
};

#endif

