//MEDEC_DevRecorder.h
#ifndef __MEDEC_DEVRECORDER_H__
#define __MEDEC_DEVRECORDER_H__

#include <MEDEC/MEDEC_Export.h>

class MEDEC_DevRecorderCallback
{
public:
	virtual void OnMEDEC_DevRecorderCallbackSamples(short*pSamples,int nSamples,int nSampleRate)=0;
};

class MEDEC_API MEDEC_DevRecorder
{
public:
	MEDEC_DevRecorder(void){};
	virtual~MEDEC_DevRecorder(void){};
public:
	virtual int Open(void)=0;
	virtual void Close(void)=0;

	static MEDEC_DevRecorder*Create(MEDEC_DevRecorderCallback&rCallback);
};

#endif
