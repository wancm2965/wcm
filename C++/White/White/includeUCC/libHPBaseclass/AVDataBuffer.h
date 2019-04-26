// AVDataBuffer.h: interface for the UDPPacket class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVDATABUFFER_H__
#define __AVDATABUFFER_H__

class AFX_EXT_CLASS AVDataBuffer  
{
public:
	AVDataBuffer(int nExtraLen=0);
	virtual ~AVDataBuffer(void);
public:
	bool StoreData(char*pData,int nLen);
	char*GetData(void){return m_pData;}
	int GetLen(void){return m_nLen;}
protected:
	char*				m_pData;
	int					m_nLen;
	int					m_nBufferSize;
	int					m_nExtraLen;
};

#endif 