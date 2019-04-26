#ifndef __AVMTSFRAME_H__
#define __AVMTSFRAME_H__

class AFX_EXT_CLASS AVMTSFrame
{
public:
	AVMTSFrame(void);
	~AVMTSFrame(void);
public:
	virtual bool SendHeader(unsigned long ulType,const void*pData,int nLen);
	virtual bool SendHeader(const char*pHeader,int nHeaderLen);
	virtual void Reset(void);
	virtual bool SendFrame(const char*pFrameData,int nLen);
	virtual bool HasSendFrameData(void);
	virtual void OnReceivedData(const char*pData,int nLen);
	virtual int SendData(const char*pData,int nLen)=0;
	virtual void OnReceivedFrame(const char*pFrameData,int nLen)=0;
protected:
	char*		m_pSendBuffer;
	int			m_nSendBufferLength;
	char*		m_pSendFrameData;
	int			m_nSendFrameDataLen;

	char*		m_pReceivedBuffer;
	int			m_nReceivedBufferLength;
	int			m_nReceivedDataLen;
};

#endif