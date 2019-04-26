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
	virtual bool IsDisconnected(unsigned long ulSessionCount);
	virtual void DetachedFromAVMTDataChannel(void);
	virtual unsigned long GetSessionCount(void);
	virtual unsigned long GetDataChannelID(void);
protected:
	XCritSec				m_csAVMTDataChannel;
	AVMTDataChannel*		m_pAVMTDataChannel;
};

class NETEC_API AVMTDataChannel
	: public IHPDTSNotify
{
public:
	AVMTDataChannel(unsigned long ulDataChannelID,bool bUI=false,bool bHost=true);
	virtual ~AVMTDataChannel(void);
public:
	virtual bool Connect(void);
	virtual void ReleaseConnections(void);
	virtual unsigned long GetSessionCount(void);
	virtual bool IsDirty(void);
	virtual bool IsEmpty(void);
	virtual void SetDataChannelID(unsigned long ulDataChannelID);
	unsigned long GetDataChannelID(void);
	void OnDataFromSession(unsigned long ulSessionID,const char*pData,int nLen);

	virtual void AddSession(AVMTDataSession*pAVMTDataSession);
	virtual void RemoveSession(AVMTDataSession*pAVMTDataSession);

	virtual void AttachToDataTR(AVMTDataTR*pAVMTDataTR);
	virtual void DetachFromDataTR(AVMTDataTR*pAVMTDataTR);

	virtual void ProcessUIData(const char*pData,int nLen);
	virtual void OnDataToUI(const char*pData,int nLen){};

	static void AddAVMTDataChannel(AVMTDataChannel*pAVMTDataChannel);
	static void RemoveAVMTDataChannel(unsigned long ulDataChannelID);
protected:
	virtual void onDTSNetDataNotify(unsigned long ulSessionID,const char* pData,int nLen);  //回调网络数据
	virtual void onDTSUIDataNotify(const char* pData,int nLen);							  //回调界面数据
protected:
	XCritSec			m_csAVMTDataTR;
	AVMTDataTR*			m_pAVMTDataTR;

	unsigned long		m_ulDataChannelID;
	XListPtr			m_listAVMTDataSession;
	XCritSec			m_csListAVMTDataSession;

	bool				m_bDirty;
	IHPDTS*				m_pIHPDTS;

	bool				m_bUI;
	bool				m_bHost;
};


#endif