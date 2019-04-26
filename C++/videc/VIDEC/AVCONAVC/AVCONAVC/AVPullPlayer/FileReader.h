//
// FileReader.h
//

#ifndef __NET_FILE_READER_H__
#define __NET_FILE_READER_H__


#include "TCPSocket.h"


class IFileReader
{
public:
	virtual void Close(void)=0;
	virtual DWORD ReadData(PBYTE pbBuffer,LONGLONG dwPosition,DWORD dwReadLength,DWORD dwMilliSeconds)=0;
	virtual LONGLONG GetFileLength(void)=0;
};

class RemoteFileReader
	: public IFileReader
{
public:
	RemoteFileReader(void);
	virtual ~RemoteFileReader(void);
public:
	BOOL Open(const char* lpszIPAddress,const char* lpszPathName);
	void Close(void);
	DWORD ReadData(PBYTE pbBuffer,LONGLONG dwPosition,DWORD dwReadLength,DWORD dwMilliSeconds);
	LONGLONG GetFileLength(void);
private:
	TCPSocket	m_TCPSocket;
	DWORD		m_nFileLength;
};

class LocalFileReader
	: public IFileReader
{
public:
	LocalFileReader(void);
	virtual ~LocalFileReader(void);
public:
	BOOL Open(const char* lpszPathName);
	void Close(void);
	DWORD ReadData(PBYTE pbBuffer,LONGLONG dwPosition,DWORD dwReadLength,DWORD dwMilliSeconds);
	LONGLONG GetFileLength(void);
protected:
	FILE*		m_pFile;
	LONGLONG	m_nFileLength;
};

#endif