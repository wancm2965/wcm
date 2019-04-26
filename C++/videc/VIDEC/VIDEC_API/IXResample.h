//IXResample.h
#ifndef __IXRESAMPLE_H__
#define __IXRESAMPLE_H__

class IXResample
{
public:
	IXResample(void){};
	virtual~IXResample(void){};
public:
	virtual int Connect(int nInSampleRate,int nOutSampleRate)=0;
	virtual void ReleaseConnections(void)=0;
	virtual int Convert(short*pInSamples,int nInSamples,short*pOutSamples,int&nOutSamples)=0;
};

IXResample*CreateIXResample(void);

#endif