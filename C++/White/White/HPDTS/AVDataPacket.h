#ifndef __AVDATAPACKET_H__
#define __AVDATAPACKET_H__

#ifdef WIN32
	#include <winsock2.h>
#else
	#include <sys/time.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <sys/uio.h>
	#include <fcntl.h>
	#include <semaphore.h>
	#include <pthread.h>
	#include <assert.h>
	#include <memory.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <linux/rtc.h>
	#include <errno.h>
#endif

//##ModelId=43A0F9F202FD
class AVDataOutPacket
{
public:
	//##ModelId=43A0F9F202FE
	AVDataOutPacket(bool bAutoDelete=true,int nBufferLength=1024)
		:m_pBuffer(NULL)
		,m_nBufferLength(nBufferLength)
		,m_bAutoDelete(bAutoDelete)
	{
		m_pBuffer=(unsigned char*)malloc(m_nBufferLength);
		if (m_pBuffer==NULL)
		{
			m_nBufferLength=0;
		}
		Reset();
	}
	//##ModelId=43A0F9F20301
	virtual~AVDataOutPacket(void)
	{
		if (m_pBuffer && m_bAutoDelete)
		{
			free(m_pBuffer);
			m_pBuffer=NULL;
		}
	}

	//##ModelId=43A0F9F20304
	void Reset()
	{
		m_pCursor = m_pBuffer;
	}

	//##ModelId=43A0F9F2030D
	unsigned char *GetData()
	{
		return m_pBuffer;
	}

	//##ModelId=43A0F9F2030E
	int GetLength()
	{
		int n;
		n=(int)(m_pCursor - m_pBuffer);
		return n;
	}

	//##ModelId=43A0F9F2030F
	void Write16(unsigned short w);
	//##ModelId=43A0F9F20311
	void Write32(unsigned int dw);
	//##ModelId=43A0F9F20313
	void Write64(unsigned long lw);
	//##ModelId=43A0F9F2031D
	void WriteData(unsigned char *buf, unsigned int n);
	//##ModelId=43A0F9F20320
	void WriteString(const char *str, unsigned int n);

	//##ModelId=43A0F9F20323
	AVDataOutPacket &operator <<(unsigned char b);
	//##ModelId=43A0F9F2032C
	AVDataOutPacket &operator <<(unsigned short w);
	//##ModelId=43A0F9F2032E
	AVDataOutPacket &operator <<(unsigned int dw);
	//##ModelId=43A0F9F20330
	AVDataOutPacket &operator <<(unsigned long lw);
	//##ModelId=43A0F9F20332
	AVDataOutPacket &operator <<(const char *str);

	//##ModelId=43A0F9F2033C
	unsigned char *Skip(int delta)
	{
		m_pCursor += delta;
		return (m_pCursor - delta);
	}

	//##ModelId=43A0F9F2033E
	unsigned char *SetCursor(unsigned char *pCur)
	{
		unsigned char *pOld = m_pCursor;
		m_pCursor = pCur;
		return pOld;
	}

protected:
	//##ModelId=43A0F9F20340
	unsigned char*	m_pBuffer;
	//##ModelId=43A0F9F20341
	unsigned int	m_nBufferLength;
	//##ModelId=43A0F9F2034B
	unsigned char*	m_pCursor;
	//##ModelId=43A0F9F2034C
	bool			m_bAutoDelete;
};


//##ModelId=43A0F9F2035B
class AVDataInPacket
{
public:
	//##ModelId=43A0F9F2035C
	AVDataInPacket(char *d, int n)
	{
		m_pCursor = m_pData = (unsigned char *) d;
		m_nDataLen = n;
	}

	//##ModelId=43A0F9F2036B
	AVDataInPacket &operator >>(unsigned char &b);
	//##ModelId=43A0F9F2036D
	AVDataInPacket &operator >>(unsigned short &w);
	//##ModelId=43A0F9F2036F
	AVDataInPacket &operator >>(unsigned int &dw);
	//##ModelId=43A0F9F20371
	AVDataInPacket &operator >>(unsigned long &lw);
	//##ModelId=43A0F9F20373
	AVDataInPacket &operator >>(const char* &str);
	//##ModelId=43A0F9F2037B
	AVDataInPacket &operator >>(char* str);

	//##ModelId=43A0F9F2037D
	unsigned int Read64();
	//##ModelId=43A0F9F2037E
	unsigned int Read32();
	//##ModelId=43A0F9F2037F
	unsigned short Read16();
	//##ModelId=43A0F9F20380
	unsigned char *ReadData(int &n);

	//##ModelId=43A0F9F2038A
	int GetLength()
	{
		return m_nDataLen;
	}

protected:
	//##ModelId=43A0F9F2038B
	unsigned char *m_pData;
	//##ModelId=43A0F9F2038C
	unsigned char *m_pCursor;
	//##ModelId=43A0F9F2038D
	int m_nDataLen;
};

#endif
