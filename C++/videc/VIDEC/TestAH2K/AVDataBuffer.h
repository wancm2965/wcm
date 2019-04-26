// AVDataBuffer.h: interface for the UDPPacket class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVDATABUFFER_H__
#define __AVDATABUFFER_H__

class AVDataBuffer  
{
public:
	AVDataBuffer();
	virtual ~AVDataBuffer();
public:
	bool StoreData(char*pData,int nLen);
	char*GetData(void){return m_pData;}
	int GetLen(void){return m_nLen;}
protected:
	char*				m_pData;
	int					m_nLen;
	int					m_nBufferSize;
};

#endif 