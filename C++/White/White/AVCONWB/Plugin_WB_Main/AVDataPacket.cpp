#include "stdafx.h"
#include "AVDataPacket.h"
//#include "Log.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//##ModelId=43A0F9F2030F
void AVDataOutPacket::Write16(unsigned short w)
{
	try
	{
		if(m_pCursor - m_pBuffer + sizeof(w) > m_nBufferLength)
		{
			int nFilled=m_pCursor - m_pBuffer;
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
	catch (...)
	{
		//logger << "AVDataOutPacket::Write16" << endl;
	}
}

//##ModelId=43A0F9F20311
void AVDataOutPacket::Write32(unsigned int dw)
{
	try
	{
		if(m_pCursor - m_pBuffer + sizeof(dw) > m_nBufferLength)
		{
			int nFilled=m_pCursor - m_pBuffer;
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
	catch (...)
	{
		//logger << "AVDataOutPacket::Write32" << endl;
	}
}

//##ModelId=43A0F9F20313
void AVDataOutPacket::Write64(unsigned long lw)
{
	try
	{
		if(m_pCursor - m_pBuffer + sizeof(lw) > m_nBufferLength)
		{
			int nFilled=m_pCursor - m_pBuffer;
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
	catch (...)
	{
		//logger << "AVDataOutPacket::Write64" << endl;
	}
}

//##ModelId=43A0F9F2031D
void AVDataOutPacket::WriteData(unsigned char *buf, unsigned int n)
{
	try
	{
		if(m_pCursor - m_pBuffer + n > m_nBufferLength)
		{
			int nFilled=m_pCursor - m_pBuffer;
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
	catch (...)
	{
		//logger << "AVDataOutPacket::WriteData" << endl;
	}
}

//##ModelId=43A0F9F20320
void AVDataOutPacket::WriteString(const char *str, unsigned int n)
{
	try
	{
		unsigned short len = n + 1;
		if(m_pCursor - m_pBuffer + n + sizeof(len) > m_nBufferLength)
		{
			int nFilled=m_pCursor - m_pBuffer;
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
	catch (...)
	{
		//logger << "AVDataOutPacket::WriteString" << endl;
	}
}

//##ModelId=43A0F9F20323
AVDataOutPacket &AVDataOutPacket::operator <<(unsigned char b)
{
	try
	{
		if(m_pCursor - m_pBuffer + sizeof(b) > m_nBufferLength)
		{
			int nFilled=m_pCursor - m_pBuffer;
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
AVDataOutPacket &AVDataOutPacket::operator <<(unsigned short w)
{
	Write16(htons(w));
	return *this;
}

//##ModelId=43A0F9F2032E
AVDataOutPacket &AVDataOutPacket::operator <<(unsigned int dw)
{
	Write32(htonl(dw));
	return *this;
}

//##ModelId=43A0F9F20330
AVDataOutPacket &AVDataOutPacket::operator <<(unsigned long lw)
{
	Write64(htonl(lw));
	return *this;
}

//##ModelId=43A0F9F20332
AVDataOutPacket &AVDataOutPacket::operator <<(const char *str)
{
	WriteString(str, (int)strlen(str));
	return *this;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//##ModelId=43A0F9F2037F
unsigned short AVDataInPacket::Read16()
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

//##ModelId=43A0F9F2037E
unsigned int AVDataInPacket::Read32()
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

//##ModelId=43A0F9F2037D
unsigned int AVDataInPacket::Read64()
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

//##ModelId=43A0F9F20380
unsigned char *AVDataInPacket::ReadData(int &n)
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

//##ModelId=43A0F9F2036B
AVDataInPacket &AVDataInPacket::operator >>(unsigned char &b)
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
AVDataInPacket &AVDataInPacket::operator >>(unsigned short &w)
{
	w = ntohs(Read16());
	return *this;
}

//##ModelId=43A0F9F2036F
AVDataInPacket &AVDataInPacket::operator >> (unsigned int &dw)
{
	dw = ntohl(Read32());
	return *this;
}

//##ModelId=43A0F9F20371
AVDataInPacket &AVDataInPacket::operator >>(unsigned long &lw)
{
	lw = ntohl(Read64());
	return *this;
}

//##ModelId=43A0F9F20373
AVDataInPacket &AVDataInPacket::operator >> (const char* &str)
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
AVDataInPacket &AVDataInPacket::operator >> (char* str)
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
		strcpy(str,cszStr);
		return *this;
	}
	catch (...)
	{
		//logger << "AVDataInPacket::>>char*" << endl;
		return *this;
	}
}
