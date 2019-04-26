#ifndef __AVDATAPACKET_H__
#define __AVDATAPACKET_H__
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

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
	void Write16(unsigned short w)
	{
		if(m_pCursor - m_pBuffer + sizeof(w) > m_nBufferLength)
		{
			int nFilled=(int)(m_pCursor - m_pBuffer);
			m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + sizeof(w))*2;
			m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
			if (m_pBuffer==NULL)
			{
				m_nBufferLength=0;
				Reset();
				return;
			}
			m_pCursor=m_pBuffer+nFilled;
		}

		*(unsigned short *)m_pCursor = w;
		m_pCursor += sizeof(w);
	}

	//##ModelId=43A0F9F20311
	void Write32(unsigned int dw)
	{
			if(m_pCursor - m_pBuffer + sizeof(dw) > m_nBufferLength)
			{
				int nFilled=(int)(m_pCursor - m_pBuffer);
				m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + sizeof(dw))*2;
				m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
				if (m_pBuffer==NULL)
				{
					m_nBufferLength=0;
					Reset();
					return;
				}
				m_pCursor=m_pBuffer+nFilled;
			}

			*(unsigned int *) m_pCursor = dw;
			m_pCursor += sizeof(dw);
	}

	//##ModelId=43A0F9F20313
	void Write64(unsigned long lw)
	{
			if(m_pCursor - m_pBuffer + sizeof(lw) > m_nBufferLength)
			{
				int nFilled=(int)(m_pCursor - m_pBuffer);
				m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + sizeof(lw))*2;
				m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
				if (m_pBuffer==NULL)
				{
					m_nBufferLength=0;
					Reset();
					return;
				}
				m_pCursor=m_pBuffer+nFilled;
			}

			*(unsigned long *) m_pCursor = lw;
			m_pCursor += sizeof(lw);
	}

	//##ModelId=43A0F9F2031D
	void WriteData(unsigned char *buf, unsigned int n)
	{
			if(m_pCursor - m_pBuffer + n > m_nBufferLength)
			{
				int nFilled=(int)(m_pCursor - m_pBuffer);
				m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + n)*2;
				m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
				if (m_pBuffer==NULL)
				{
					m_nBufferLength=0;
					Reset();
					return;
				}
				m_pCursor=m_pBuffer+nFilled;
			}

			memcpy(m_pCursor, buf, n);
			m_pCursor += n;
	}

	//##ModelId=43A0F9F20320
	void WriteString(const char *str, unsigned int n)
	{
			unsigned short len = n + 1;
			if(m_pCursor - m_pBuffer + n + sizeof(len) > m_nBufferLength)
			{
				int nFilled=(int)(m_pCursor - m_pBuffer);
				m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + n + sizeof(len))*2;
				m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
				if (m_pBuffer==NULL)
				{
					m_nBufferLength=0;
					Reset();
					return;
				}
				m_pCursor=m_pBuffer+nFilled;
			}

			*this << len;
			memcpy(m_pCursor, str, len);
			m_pCursor += len;
	}


	//##ModelId=43A0F9F20323
	AVDataOutPacket &operator <<(unsigned char b)
	{
		try
		{
			if(m_pCursor - m_pBuffer + sizeof(b) > m_nBufferLength)
			{
				int nFilled=(int)(m_pCursor - m_pBuffer);
				m_nBufferLength=(unsigned int)(m_pCursor - m_pBuffer + sizeof(b))*2;
				m_pBuffer=(unsigned char*)realloc(m_pBuffer,m_nBufferLength);
				if (m_pBuffer==NULL)
				{
					m_nBufferLength=0;
					Reset();
					return *this;
				}
				m_pCursor=m_pBuffer+nFilled;
			}

			*(unsigned char *) m_pCursor = b;
			m_pCursor += sizeof(b);

			return *this;
		}
		catch (...)
		{
			//logger << "AVDataOutPacket::<<" << endl;
			return *this;
		}
	}

	//##ModelId=43A0F9F2032C
	AVDataOutPacket &operator <<(unsigned short w)
	{
		Write16(htons(w));
		return *this;
	}

	//##ModelId=43A0F9F2032E
	AVDataOutPacket &operator <<(unsigned int dw)
	{
		Write32(htonl(dw));
		return *this;
	}

	//##ModelId=43A0F9F20330
	AVDataOutPacket &operator <<(unsigned long lw)
	{
		Write64(htonl(lw));
		return *this;
	}

	//##ModelId=43A0F9F20332
	AVDataOutPacket &operator <<(const char *str)
	{
		WriteString(str, (int)strlen(str));
		return *this;
	}

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
	AVDataInPacket &operator >>(unsigned char &b)
	{
		try
		{
			b = 0;
			if (m_pCursor <= m_pData + m_nDataLen - sizeof(b))
				b = *m_pCursor++;
			return *this;
		}
		catch (...)
		{
			//logger << "AVDataInPacket::>>unsigned char &" << endl;
			return *this;
		}
	}

	//##ModelId=43A0F9F2036D
	AVDataInPacket &operator >>(unsigned short &w)
	{
		w = ntohs(Read16());
		return *this;
	}

	//##ModelId=43A0F9F2036F
	AVDataInPacket &operator >>(unsigned int &dw)
	{
		dw = ntohl(Read32());
		return *this;
	}

	//##ModelId=43A0F9F20371
	AVDataInPacket &operator >>(unsigned long &lw)
	{
		lw = ntohl(Read64());
		return *this;
	}

	//##ModelId=43A0F9F20373
	AVDataInPacket &operator >>(const char* &str)
	{
		try
		{
			unsigned short len;
			operator >> (len);
			if (len && m_pCursor <= m_pData + m_nDataLen - len && !m_pCursor[len - 1])
			{
				str = (char *)m_pCursor;
				m_pCursor += len;
			}
			else
			{
				str = "";
			}
			return *this;
		}
		catch (...)
		{
			//logger << "AVDataInPacket::>>const char* &" << endl;
			return *this;
		}
	}

	//##ModelId=43A0F9F2037B
	AVDataInPacket &operator >>(char* str)
	{
		try
		{
			const char* cszStr=NULL;
			unsigned short len;
			operator >> (len);
			if (len && m_pCursor <= m_pData + m_nDataLen - len && !m_pCursor[len - 1])
			{
				cszStr = (char *)m_pCursor;
				m_pCursor += len;
			}
			else
			{
				cszStr = "";
			}
			strcpy_s(str,strlen(cszStr) + 1,cszStr);
			return *this;
		}
		catch (...)
		{
			//logger << "AVDataInPacket::>>char*" << endl;
			return *this;
		}
	}

	//##ModelId=43A0F9F2037D
	unsigned int Read64()
	{
		try
		{
			unsigned long lw = 0;
			if (m_pCursor <= m_pData + m_nDataLen - sizeof(lw))
			{
				lw = *(unsigned long *) m_pCursor;
				m_pCursor += sizeof(lw);
			}
			return lw;
		}
		catch (...)
		{
			//logger << "AVDataInPacket::Read64" << endl;
			return 0;
		}
	}

	//##ModelId=43A0F9F2037E
	unsigned int Read32()
	{
		try
		{
			unsigned int dw = 0;
			if (m_pCursor <= m_pData + m_nDataLen - sizeof(dw))
			{
				dw = *(unsigned int *) m_pCursor;
				m_pCursor += sizeof(dw);
			}
			return dw;
		}
		catch (...)
		{
			//logger << "AVDataInPacket::Read32" << endl;
			return 0;
		}
	}

	//##ModelId=43A0F9F2037F
	unsigned short Read16()
	{
		try
		{
			unsigned short w = 0;
			if (m_pCursor <= m_pData + m_nDataLen - sizeof(w))
			{
				w = *(unsigned short *) m_pCursor;
				m_pCursor += sizeof(w);
			}
			return w;
		}
		catch (...)
		{
			//logger << "AVDataInPacket::Read16" << endl;
			return 0;
		}
	}

	//##ModelId=43A0F9F20380
	unsigned char *ReadData(int &n)
	{
		try
		{
			n = (int)(m_nDataLen - (m_pCursor - m_pData));
			return m_pCursor;
		}
		catch (...)
		{
			//logger << "AVDataInPacket::ReadData" << endl;
			return NULL;
		}
	}

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
