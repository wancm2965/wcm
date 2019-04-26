#pragma once
class IFindFileServer
{
public:
	virtual ~IFindFileServer(){};// +for leak by lgx [12/9/2008 Administrator]
	virtual BOOL StartServer(int port)=0;
	virtual void StopServer()=0;
};
IFindFileServer* FindFileServerStart(int port);
void FindFileServerStop(IFindFileServer* p);