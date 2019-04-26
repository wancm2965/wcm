#ifndef __AVMTDataCHANNEL_H__
#define __AVMTDataCHANNEL_H__



#include <HPMCU/AVMTDataSession.h>
#include <NETEC/XAutoLock.h>
#include <HPDTS/IHPDTS.h>
#include <NETEC/XListPtr.h>


class NETEC_API AVMTDataChannel;
class NETEC_API AVMTDataTR
{
public:
	AVMTDataTR(AVMTDataChannel&rAVMTDataChannel);
	virtual~AVMTDataTR(void);
public:
	virtual void ReleaseConnections(void);
	virtual bool IsDisconnected(uint32_t ulSessionCount);
	virtual void DetachedFromAVMTDataChannel(void);
	virtual uint32_t GetSessionCount(void);
	virtual uint32_t GetDataChannelID(void);
protected:
	XCritSec				m_csAVMTDataChannel;
	AVMTDataChannel*		m_pAVMTDataChannel;
};

class NETEC_API AVMTDataChannel
	: public IHPDTSNotify
{
public:
	AVMTDataChannel(uint32_t ulDataChannelID,bool bUI=false,bool bHost=true);
	virtual ~AVMTDataChannel(void);
public:
	virtual bool Connect(void);
	virtual void ReleaseConnections(void);
	virtual uint32_t GetSessionCount(void);
	virtual bool IsDirty(void);
	virtual bool IsEmpty(void);
	virtual void SetDataChannelID(uint32_t ulDataChannelID);
	uint32_t GetDataChannelID(void);
	void OnDataFromSession(uint32_t ulSessionID,const char*pData,int nLen);

	virtual void AddSession(AVMTDataSession*pAVMTDataSession);
	virtual void RemoveSession(AVMTDataSession*pAVMTDataSession);

	virtual void AttachToDataTR(AVMTDataTR*pAVMTDataTR);
	virtual void DetachFromDataTR(AVMTDataTR*pAVMTDataTR);

	virtual void ProcessUIData(const char*pData,int nLen);
	virtual void OnDataToUI(const char*pData,int nLen){};

	static void AddAVMTDataChannel(AVMTDataChannel*pAVMTDataChannel);
	static void RemoveAVMTDataChannel(uint32_t ulDataChannelID);
protected:
	virtual void onDTSNetDataNotify(uint32_t ulSessionID,const char* pData,int nLen);  //回调网络数据
	virtual void onDTSUIDataNotify(const char* pData,int nLen);							  //回调界面数据
protected:
	XCritSec			m_csAVMTDataTR;
	AVMTDataTR*			m_pAVMTDataTR;

	uint32_t		m_ulDataChannelID;
	XListPtr			m_listAVMTDataSession;
	XCritSec			m_csListAVMTDataSession;

	bool				m_bDirty;
	IHPDTS*				m_pIHPDTS;

	bool				m_bUI;
	bool				m_bHost;
};


#endif