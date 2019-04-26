// XDataBuffer.h: interface for the UDPPacket class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XDATABUFFER_H__
#define __XDATABUFFER_H__

#include <NETEC/NETEC_Export.h>

class NETEC_API XDataBuffer  
{
public:
	XDataBuffer(int32_t nExtraLen=0);
	virtual ~XDataBuffer(void);
public:
	void Release(void);
	bool StoreData(char*pData,int32_t nLen);
	char*GetData(void){return m_pData;}
	int32_t GetLen(void){return m_nLen;}
protected:
	char*				m_pData;
	int32_t					m_nLen;
	int32_t					m_nBufferSize;
	int32_t					m_nExtraLen;
};

#endif 