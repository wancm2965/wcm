
#include "BFCPProcess.h"
#include <re/re.h>
#include <re/bfcp.h>

typedef unsigned int	pj_size_t;
typedef long			pj_ssize_t;
#define BfcpMsgMaxLen 512 

BFCP_Process::BFCP_Process(SIP_BFCPCallback& rCallback)
:m_rBFCPCallback(rCallback)
{
	m_nBfcpRequestId=0;
	m_nBfcpLocalRequestId=0;
	m_nBfcpTransactionID=0;

	strcpy(m_strReqReason,"SendSecondVideo");
	m_nRequestConfId=1;
	m_nRequestUserId=2;
	m_nRequestFloorId=2;
	m_nRequestPriority=BFCP_PRIO_NORMAL;
	m_nRequestFloorControlType=e_Floor_Control_Client;
	m_nRequestSetupType=e_BFCP_Setup_Active;	
	m_nRequestTransportType=e_BFCP_Transport_UDP;

	memset(m_szSurplusBuf,0,MaxDataBufLen);
	m_nSurplusBufLen=0;
}


void BFCP_Process::SetBFCPConfId(int confId)
{
	m_nRequestConfId=confId;
}

void BFCP_Process::SetBFCPFloorId(int floorId)
{
	m_nRequestFloorId=floorId;
}

void BFCP_Process::SetBFCPUserId(int userId)
{
	m_nRequestUserId=userId;
}

void BFCP_Process::SetBFCPRequestPriority(unsigned short priority)
{
	m_nRequestPriority=priority;
}

void BFCP_Process::SetBFCPFloorControlType(EBFCPFloorControlType type)
{
	m_nRequestFloorControlType=type;
}

void BFCP_Process::SetBFCPSetupType(EBFCPSetupType type)
{
	m_nRequestSetupType=type;
}

void BFCP_Process::SetBFCPTransportType(EBFCPTransportType type)
{
	m_nRequestTransportType=type;
}




int BFCP_Process::SendBFCPFloorRequest() 
{
	int err=-1;
	uint32_t confId=m_nRequestConfId;
	uint16_t userId=m_nRequestUserId;
	uint16_t floorId=m_nRequestFloorId;

	m_nBfcpTransactionID++;
	enum bfcp_prim prim=BFCP_FLOOR_REQUEST;
	uint32_t attrc=0;

	enum bfcp_attrib type1=BFCP_FLOOR_ID;
	int nFloorIdAttr=floorId;
	attrc++;

	enum bfcp_attrib type2=BFCP_BENEFICIARY_ID;
	int nBeneficiaryIdAttr=userId;
	attrc++;

	enum bfcp_attrib type3=BFCP_PARTICIPANT_PROV_INFO;
	char requestReason[256]={0};
	strcpy(requestReason,m_strReqReason);
	attrc++;

	enum bfcp_attrib type4=BFCP_PRIORITY;
	enum bfcp_prio prio=(enum bfcp_prio)m_nRequestPriority;
	attrc++;

	unsigned char msgBuf[BfcpMsgMaxLen]={0};
	struct mbuf mb;
	pj_size_t size;

	mb.buf=msgBuf;
	mb.end=0;
	mb.pos=0;
	mb.size=BfcpMsgMaxLen;


	err = bfcp_msg_encode(&mb, prim, confId, m_nBfcpTransactionID, userId,attrc,
		type1,&nFloorIdAttr,type2,&nBeneficiaryIdAttr,type3,&requestReason,type4,&prio);


	mb.pos = 0;
	size=mb.end - mb.pos;

	return SendData((const char *)msgBuf,size);
}


int BFCP_Process::SendBFCPFloorRequestStatus(unsigned short transactionId,unsigned short userId,unsigned short floorId,unsigned short status)
{
	int err=-1;
	uint32_t confId=m_nRequestConfId;

	enum bfcp_rstat reqStatus=(enum bfcp_rstat)status;
	enum bfcp_prim prim=BFCP_FLOOR_REQUEST_STAT;
	uint32_t attrc=0;

	enum bfcp_attrib type1=BFCP_FLOOR_REQUEST_INFO;
	struct bfcp_floor_reqinfo bfcpFloorReqInfo;
	bfcpFloorReqInfo.freqid=m_nBfcpLocalRequestId;

	bfcpFloorReqInfo.ors.freqid=m_nBfcpLocalRequestId;
	bfcpFloorReqInfo.ors.reqstat.stat=reqStatus;
	bfcpFloorReqInfo.ors.reqstat.qpos=0;
	bfcpFloorReqInfo.ors.statinfo=NULL;

	struct bfcp_floor_reqstat reqstat;
	reqstat.floorid=this->m_nRequestFloorId;
	reqstat.reqstat.stat=reqStatus;
	reqstat.reqstat.qpos=0;
	reqstat.statinfo=NULL;

	bfcpFloorReqInfo.frsv=&reqstat;
	bfcpFloorReqInfo.frsc=1;

	bfcpFloorReqInfo.bfi.bfid=this->m_nRequestUserId;
	bfcpFloorReqInfo.bfi.dname=NULL;
	bfcpFloorReqInfo.bfi.uri=NULL;

	bfcpFloorReqInfo.rbi.rbid=this->m_nRequestUserId;
	bfcpFloorReqInfo.rbi.dname=NULL;
	bfcpFloorReqInfo.rbi.uri=NULL;

	bfcpFloorReqInfo.prio=(enum bfcp_prio)m_nRequestPriority;

	bfcpFloorReqInfo.ppi=NULL;


	attrc++;


	unsigned char msgBuf[BfcpMsgMaxLen]={0};
	struct mbuf mb;
	pj_size_t size;

	mb.buf=msgBuf;
	mb.end=0;
	mb.pos=0;
	mb.size=BfcpMsgMaxLen;

	err = bfcp_msg_encode(&mb, prim, confId, transactionId, userId,attrc,
		type1,&bfcpFloorReqInfo);


	mb.pos = 0;
	size=mb.end - mb.pos;
	return SendData((const char *)msgBuf,size);
}

int BFCP_Process::SendBFCPFloorRelease() //Command
{

	int err=-1;
	uint32_t confId=m_nRequestConfId;
	uint16_t userId=m_nRequestUserId;
	uint16_t floorId=m_nRequestFloorId;

	enum bfcp_prim prim=BFCP_FLOOR_RELEASE;
	uint32_t attrc=0;

	enum bfcp_attrib type1=BFCP_FLOOR_REQUEST_ID;
	int nFloorRequestIdAttr=this->m_nBfcpRequestId;
	attrc++;

	unsigned char msgBuf[BfcpMsgMaxLen]={0};
	struct mbuf mb;
	pj_size_t size;

	mb.buf=msgBuf;
	mb.end=0;
	mb.pos=0;
	mb.size=BfcpMsgMaxLen;


	err = bfcp_msg_encode(&mb, prim, confId, m_nBfcpTransactionID, userId,attrc,
		type1,&nFloorRequestIdAttr);


	mb.pos = 0;
	size=mb.end - mb.pos;
	return SendData((const char *)msgBuf,size);
}



int BFCP_Process::SendBFCPHello() //hello
{
	int err=-1;
	uint32_t confId=m_nRequestConfId;
	uint16_t userId=m_nRequestUserId;
	uint16_t floorId=m_nRequestFloorId;

	enum bfcp_prim prim=BFCP_HELLO;
	uint32_t attrc=0;

	unsigned char msgBuf[BfcpMsgMaxLen]={0};
	struct mbuf mb;
	pj_size_t size;

	mb.buf=msgBuf;
	mb.end=0;
	mb.pos=0;
	mb.size=BfcpMsgMaxLen;

	err = bfcp_msg_encode(&mb, prim, confId, m_nBfcpTransactionID, userId,attrc);

	mb.pos = 0;
	size=mb.end - mb.pos;
	return SendData((const char *)msgBuf,size);
}

int BFCP_Process::SendBFCPHelloACK() //hello ack
{
	int err=-1;
	uint32_t confId=m_nRequestConfId;
	uint16_t userId=m_nRequestUserId;
	uint16_t floorId=m_nRequestFloorId;

	enum bfcp_prim prim=BFCP_HELLO_ACK;
	uint32_t attrc=0;


	enum bfcp_prim primv[1];
	primv[0]=BFCP_HELLO_ACK;
	struct bfcp_supprim sp;
	sp.primc=1;
	sp.primv=primv;

	enum bfcp_attrib attribv[1];
	attribv[0]=BFCP_FLOOR_ID;
	struct bfcp_supattr sa;
	sa.attrc=1;
	sa.attrv=attribv;


	enum bfcp_attrib type1=BFCP_SUPPORTED_PRIMITIVES;
	attrc++;

	enum bfcp_attrib type2=BFCP_SUPPORTED_ATTRIBUTES;
	attrc++;

	unsigned char msgBuf[BfcpMsgMaxLen]={0};
	struct mbuf mb;
	pj_size_t size;

	mb.buf=msgBuf;
	mb.end=0;
	mb.pos=0;
	mb.size=BfcpMsgMaxLen;

	err = bfcp_msg_encode(&mb, prim, confId, m_nBfcpTransactionID, userId,attrc,
		type1,&sp,type2,&sa);

	mb.pos = 0;
	size=mb.end - mb.pos;
	return SendData((const char *)msgBuf,size);

}



void BFCP_Process::OnBFCPTCPConnected(EBFCPTCPCONType nType)
{
	m_rBFCPCallback.OnSIPBFCPTCPConnected(nType);
}

void BFCP_Process::OnRcvdData(const char*pData,int nLen)
{
	int nBfcpMsgLen=0;
	struct mbuf mb;
	char szBuf[MaxDataBufLen*2];
	int nSurplus=0;
	int nGetLen=0;

	if( (nLen>MaxDataBufLen) || (nLen<=0) )
	{
		return;
	}

	if(m_nSurplusBufLen>0) 
	{
		memcpy(szBuf,m_szSurplusBuf,m_nSurplusBufLen);
	}
	memcpy(szBuf+m_nSurplusBufLen,pData,nLen);
	nSurplus=nLen+m_nSurplusBufLen;

	while(nSurplus>0)
	{
		if(nSurplus<BFCP_HDR_SIZE)
		{
			memcpy(m_szSurplusBuf,szBuf,nSurplus);
			m_nSurplusBufLen=nSurplus;
			break;
		}

		mb.buf=(unsigned char *)(szBuf+nGetLen);
		mb.pos=0;
		mb.size=mb.end=nSurplus;
		nBfcpMsgLen=get_bfcp_msg_len(&mb);

		mb.buf=(unsigned char *)(szBuf+nGetLen);
		mb.pos=0;
		mb.size=mb.end=nBfcpMsgLen;

		if(nBfcpMsgLen<BFCP_HDR_SIZE)
		{
			m_nSurplusBufLen=0;
			break;
		}

		if(nSurplus<nBfcpMsgLen)
		{
			memcpy(m_szSurplusBuf,szBuf,nSurplus);
			m_nSurplusBufLen=nSurplus;
			break;
		}
		//parse		
		ParseBFCPMsg(&mb);
		nGetLen=nGetLen+nBfcpMsgLen;
		nSurplus=nSurplus-nBfcpMsgLen;
	}
}

void BFCP_Process::ParseBFCPMsg(struct mbuf *mb)
{
	int err;struct sa paddr;
	struct bfcp_msg *bfcpMsg=NULL;
	enum bfcp_prim prim;
	enum bfcp_rstat reqStatus;

	struct bfcp_attr *attr=NULL;
	struct bfcp_attr *attr2=NULL;
	unsigned short floorReqId=0;
	unsigned short transactionId=0;
	unsigned short userId=0;
	unsigned short floorId=0;

	err=bfcp_msg_decode(&bfcpMsg,mb,&paddr);

	if (err==0) //ok
	{	
		transactionId=bfcp_msg_tid(bfcpMsg);
		userId=bfcp_msg_userid(bfcpMsg);
		prim=bfcp_msg_prim(bfcpMsg);
		switch(prim)
		{
		case BFCP_FLOOR_REQUEST_STAT:
			attr=bfcp_msg_attr(bfcpMsg,BFCP_FLOOR_REQUEST_INFO);
			if(attr)
			{	
				floorReqId=attr->v.fri.freqid;
				reqStatus=attr->v.fri.frsv->reqstat.stat;
				if(reqStatus==BFCP_PENDING) m_nBfcpRequestId=floorReqId;
				m_rBFCPCallback.OnSIPBFCPCallbackFloorRequestStatus(floorReqId,reqStatus);
			}
			break;
		case BFCP_FLOOR_RELEASE:
			attr=bfcp_msg_attr(bfcpMsg,BFCP_FLOOR_REQUEST_ID);
			if(attr)
			{	
				floorReqId=attr->v.freqid;			
				m_rBFCPCallback.OnSIPBFCPCallbackFloorRelease(transactionId,userId,floorReqId);
			}
			break;
		case BFCP_FLOOR_REQUEST:
		case BFCP_FLOOR_STATUS:
			attr2=bfcp_msg_attr(bfcpMsg,BFCP_FLOOR_ID);
			if(attr2) floorId=attr2->v.floorid;
			m_nBfcpLocalRequestId++;
			m_rBFCPCallback.OnSIPBFCPCallbackFloorRequest(transactionId,userId,floorId);
			break;
		case BFCP_HELLO:
			m_rBFCPCallback.OnSIPBFCPCallbackHello();
			break;
		default:
			break;
		}

		mem_deref(bfcpMsg);
	}

}
