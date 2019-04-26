//PipeClient.h
#ifndef __PIPECLIENT_H__
#define __PIPECLIENT_H__

class PipeClient
{
public:
	PipeClient(void);
	virtual~PipeClient(void);
public:
	virtual int Open(void);
	virtual void Close(void);
protected:

};

#endif
