//
// FileReader.cpp
//

#include  "stdafx.h"
#include  "FileReader.h"
#include "../../VIDEC_FRE/VIDECFREHeader.h"

#define AVVOD_TCP_PORT					8288
#define AV_VOD_SESSION_TYPE_PULL_CLIENT	1

typedef struct tagAV_VOD_SESSION_INFO
{
	unsigned long		ulSSRC;
	unsigned long		ulLength;
}AV_VOD_SESSION_INFO,*PAV_VOD_SESSION_INFO;

typedef struct _SN_NET_FILE_READ_{
	DWORD			dwPosition;
	DWORD			dwReadLength;
}SN_NET_FILE_READ,*PSN_NET_FILE_READ;


RemoteFileReader::RemoteFileReader()
{
	m_nFileLength=0;
}

RemoteFileReader::~RemoteFileReader()
{
}

BOOL RemoteFileReader::Open(const char* lpszIPAddress,const char* lpszPathName)
{
	CSockAddr sa(lpszIPAddress,AVVOD_TCP_PORT);
	
	DWORD nFileLength=0;
	if(m_TCPSocket.Create()==false)
	{
		m_TCPSocket.Close();
		return FALSE;
	}

	if (m_TCPSocket.Connect(sa,5)==false)
	{
		m_TCPSocket.Close();
		return FALSE;
	}

	unsigned long ulSSRC=AV_VOD_SESSION_TYPE_PULL_CLIENT;
	int nLength=strlen(lpszPathName)+1;
	int nInfoSize=sizeof(AV_VOD_SESSION_INFO);
	int nBufferLen=nInfoSize+nLength;
	NEWRECORD3( char*, pBuffer , new char[nBufferLen] )
	if (pBuffer==NULL)
	{
		m_TCPSocket.Close();
		return FALSE;
	}

	PAV_VOD_SESSION_INFO pAVVODSessionInfo=(PAV_VOD_SESSION_INFO)pBuffer;
	pAVVODSessionInfo->ulSSRC=htonl(ulSSRC);
	pAVVODSessionInfo->ulLength=htonl(nLength);

	memcpy(pBuffer+nInfoSize,lpszPathName,nLength);

	if (m_TCPSocket.Send(pBuffer,nBufferLen)<1)
	{
		m_TCPSocket.Close();

		delete[]pBuffer;
		pBuffer=NULL;
		return FALSE;
	}

	if (m_TCPSocket.Receive((char*)(&nFileLength),4)!=4)
	{
		m_TCPSocket.Close();

		delete[]pBuffer;
		pBuffer=NULL;
		return FALSE;
	}

	delete[]pBuffer;
	pBuffer=NULL;

	m_nFileLength=nFileLength;
	return TRUE;
}

void RemoteFileReader::Close()
{
	m_TCPSocket.Close();
}


DWORD RemoteFileReader::ReadData(PBYTE pbBuffer,LONGLONG dwPosition,DWORD dwReadLength,DWORD dwMilliSeconds)
{
	if(IsBadWritePtr(pbBuffer,dwReadLength))
		return	0;
	DWORD dwBytesRead;
	DWORD dwActual=0;
	PBYTE pbData=pbBuffer;
	DWORD dwTemp=dwReadLength;

	SN_NET_FILE_READ SNNetFileRead;
	SNNetFileRead.dwPosition=dwPosition;
	SNNetFileRead.dwReadLength=dwReadLength;
	m_TCPSocket.Send((char*)(&SNNetFileRead),sizeof(SN_NET_FILE_READ));

	while (dwReadLength>0)
	{
		dwBytesRead=m_TCPSocket.Receive((char*)pbData,dwReadLength);
		if (dwBytesRead>0)
		{
			dwActual+=dwBytesRead;
			pbData+=dwBytesRead;
			dwReadLength-=dwBytesRead;
		}
		else
		{
			break;
		}
	}
	return dwActual;
}

LONGLONG RemoteFileReader::GetFileLength()
{
	return m_nFileLength;
}


LocalFileReader::LocalFileReader(void)
{
	m_pFile=NULL;
	m_nFileLength=0;
}

LocalFileReader::~LocalFileReader(void)
{

}

BOOL LocalFileReader::Open(const char* lpszPathName)
{
	m_pFile=fopen(lpszPathName,"rb");
	if (m_pFile==NULL)
	{
		return FALSE;
	}
	_fseeki64(m_pFile, 0L, SEEK_END);
	m_nFileLength = _ftelli64(m_pFile);
	_fseeki64(m_pFile,0L,SEEK_SET);

	return TRUE;
}

void LocalFileReader::Close(void)
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile=NULL;
	}
}

DWORD LocalFileReader::ReadData(PBYTE pbBuffer,LONGLONG dwPosition,DWORD dwReadLength,DWORD dwMilliSeconds)
{
	if (m_pFile)
	{
		LONGLONG nCurPos=_ftelli64(m_pFile);
		if (nCurPos!=dwPosition)
		{
			_fseeki64(m_pFile,dwPosition,0);
		}

		int nBytes=fread(pbBuffer,1,dwReadLength,m_pFile);
		if (nBytes>0)
		{
			return nBytes;
		}
	}

	return 0;
}

LONGLONG LocalFileReader::GetFileLength(void)
{
	return m_nFileLength;
}
