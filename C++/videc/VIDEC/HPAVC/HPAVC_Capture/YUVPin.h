//YUVPin.h
#ifndef __YUVPIN_H__
#define __YUVPIN_H__

class YUVPinIn
{
public:
	YUVPinIn(void){};
	virtual~YUVPinIn(void){};
public:
	virtual void LockYUV(unsigned char**ppY,unsigned char**ppUV)=0;
	virtual void UnlockYUV(void)=0;
};

class StreamIn
{
public:
	StreamIn(void){};
	virtual~StreamIn(void){};
public:
	virtual void OnStreamIn(unsigned char*pData,int nLen,unsigned char nKeyFrame)=0;
};

#endif
