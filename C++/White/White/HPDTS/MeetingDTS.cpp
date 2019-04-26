
#include "stdafx.h"
#include "MeetingDTS.h"
#include "XBufferPool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------------------------
MeetingDTS::MeetingDTS(IHPDTSNotify&rIHPDTSNotify,bool bUI)
:m_rIHPDTSNotify(rIHPDTSNotify)
,m_bUI(bUI)
,m_ulParentSessionID(0)
{
}

//---------------------------------------------------------------------------------------
MeetingDTS::~MeetingDTS(void)
{	
	ClearMeetingData();
}

//---------------------------------------------------------------------------------------
void MeetingDTS::AddMeetingSession(unsigned long ulSessionID)
{
	{
		XAutoLock l(m_csMapSessionID);
		m_MapSessionID[ulSessionID]=ulSessionID;
	}	
	
	if (!m_bUI)//回复一个已链接状态，对方收到后，按需请求对象资源
	{
		AVDataOutPacket outPacket;
		outPacket << (UINT)MET_DTS_CONNECTED;			
		m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());
	}
}

//---------------------------------------------------------------------------------------
void MeetingDTS::DelMeetingSession(unsigned long ulSessionID)
{	
	{
		XAutoLock l(m_csMapSessionID);
		m_MapSessionID.erase(ulSessionID);
	}
}

//---------------------------------------------------------------------------------------
void MeetingDTS::ProMeetingUIData(const char* pData,int nLen)
{
	if (pData == NULL || nLen < 1)
	{
		return;
	}

	UINT usCmd;
	AVDataInPacket in((char*)pData,nLen);
	in >> usCmd;
	switch(usCmd)
	{
	case MET_DTS_ADD:
	case MET_DTS_UPDATE:
		{
			if (!m_bUI) 
			{				
				unsigned int nLevel=0;
				char chManiKey[1024] = {0};
				char chSubKey[1024] = {0};
				int nDataLen=0;	
				unsigned char* pReadData = NULL;
				in >> nLevel >> chManiKey >> chSubKey;
				pReadData = in.ReadData((int&)nDataLen);

				if (pReadData == NULL || nDataLen < 1)
				{
					return;
				}

				unsigned int nIndex = DelMeetingObj(chManiKey,chSubKey);
				nIndex +=1;				
				SaveMeetingObj(nLevel,chManiKey,chSubKey,(char*)pReadData,nDataLen,nIndex);//保存

				AVDataOutPacket outPacket;
				outPacket << usCmd;
				outPacket << nLevel;
				outPacket << chManiKey;
				outPacket << chSubKey;
				outPacket << nIndex;
				outPacket.WriteData(pReadData,nDataLen);
				DeliverMeetingData(LOCAL_SESSIONID,(char *)outPacket.GetData(),outPacket.GetLength());//分发数据
			}
			break;
		}
	case MET_DTS_DELETE:
		{
			if (!m_bUI)
			{				
				char chManiKey[1024] = {0};
				char chSubKey[1024] = {0};
				in >> chManiKey >> chSubKey;
				unsigned int nIndex = DelMeetingObj(chManiKey,chSubKey); //删除
				nIndex +=1;	

				AVDataOutPacket outPacket;
				outPacket << usCmd;
				outPacket << chManiKey;
				outPacket << chSubKey;
				outPacket << nIndex;
				DeliverMeetingData(LOCAL_SESSIONID,(char *)outPacket.GetData(),outPacket.GetLength());//分发数据
			}
			break;
		}
	case MET_DTS_REQUEST:
		{
			if (m_ulParentSessionID > 0)
			{
				m_rIHPDTSNotify.onDTSNetDataNotify(m_ulParentSessionID,pData,nLen); //向上级节点请求数据
			}
			break;
		}
	case MET_DTS_CHATMSG:
		{		
			DeliverMeetingData(LOCAL_SESSIONID,pData,nLen);//分发数据
			break;
		}
	default:
		printf("MeetingDTS::ProMeetingUIData unknown command.usCmd = %d\n",usCmd);
	}
}

void MeetingDTS::ProMeetingDTSData(unsigned long ulSessionID,const char* pData,int nLen)
{
	if (pData == NULL || nLen < 1)
	{
		return;
	}

	UINT usCmd;
	AVDataInPacket in((char*)pData,nLen);
	in >> usCmd;
	switch(usCmd)
	{
	case MET_DTS_CONNECTED:
		{
			m_ulParentSessionID = ulSessionID;

			if (m_bUI) //反馈给界面
			{
				m_rIHPDTSNotify.onDTSUIDataNotify(pData,nLen);
			}
			else //服务器，向上级节点请求所有对象
			{				
				AVDataOutPacket outPacket;
				outPacket << (UINT)MET_DTS_REQUEST;
				outPacket << (UINT)0;
				outPacket << (UINT)0;
				m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());
			}
			break;
		}
	case MET_DTS_ADD:
	case MET_DTS_UPDATE:
		{
			if (m_bUI) 
			{
				m_rIHPDTSNotify.onDTSUIDataNotify(pData,nLen);//反馈给界面
			}
			else 
			{
				unsigned int nLevel=0;
				unsigned int nIndex=0;
				char chManiKey[1024] = {0};
				char chSubKey[1024] = {0};
				int nDataLen=0;	
				unsigned char* pReadData = NULL;
				in >> nLevel >> chManiKey >> chSubKey >> nIndex;
				pReadData = in.ReadData((int&)nDataLen);
				DelMeetingObj(chManiKey,chSubKey);
				SaveMeetingObj(nLevel,chManiKey,chSubKey,(char*)pReadData,nDataLen,nIndex);//保存

				DeliverMeetingData(ulSessionID,pData,nLen);//分发数据
			}
			break;
		}
	case MET_DTS_DELETE:
		{
			if (m_bUI) 
			{
				m_rIHPDTSNotify.onDTSUIDataNotify(pData,nLen);//反馈给界面
			}
			else 
			{
				char chManiKey[1024] = {0};
				char chSubKey[1024] = {0};
				in >> chManiKey >> chSubKey;

				DelMeetingObj(chManiKey,chSubKey); //删除

				DeliverMeetingData(ulSessionID,pData,nLen); //分发数据
			}
			break;
		}
	case MET_DTS_READY: //请求前数据准备,通知客户端清理数据
		if (m_bUI) 
		{
			m_rIHPDTSNotify.onDTSUIDataNotify(pData,nLen);//反馈给界面
		}
		break;
	case MET_DTS_REQUEST: //收到请求数据
		{
			if (!m_bUI)
			{
				//在请求数据之前先通知客户端进行数据清理
				AVDataOutPacket outPacket;
				outPacket << UINT(MET_DTS_READY);
				m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char*)outPacket.GetData(),outPacket.GetLength());	

				unsigned int nLevel=0;
				unsigned int nOnlyThis=0;
				in >> nLevel >> nOnlyThis;
				SendMeetingObj(ulSessionID,nLevel,(nOnlyThis != 0));
			}
			break;
		}
	case MET_DTS_RESPONSE: //收到回调的请求的数据
		{
			if (m_bUI) 
			{
				m_rIHPDTSNotify.onDTSUIDataNotify(pData,nLen);//反馈给界面
			}
			else 
			{
				unsigned int nLevel=0;
				unsigned int nIndex=0;
				char chManiKey[1024] = {0};
				char chSubKey[1024] = {0};
				int nDataLen=0;	
				unsigned char* pReadData = NULL;
				in >> nLevel >> chManiKey >> chSubKey >> nIndex;
				pReadData = in.ReadData((int&)nDataLen);
				DelMeetingObj(chManiKey,chSubKey);
				SaveMeetingObj(nLevel,chManiKey,chSubKey,(char*)pReadData,nDataLen,nIndex);//保存

				DeliverMeetingData(ulSessionID,pData,nLen); //分发数据
			}
			break;
		}
	case MET_DTS_CHATMSG:
		{
			if (m_bUI) 
			{
				m_rIHPDTSNotify.onDTSUIDataNotify(pData,nLen);//反馈给界面
			}
			else
			{
				DeliverMeetingData(ulSessionID,pData,nLen); //分发数据
			}
			break;
		}
	default:
		printf("MeetingDTS::ProMeetingDTSData unknown command.usCmd = %d\n",usCmd);
	}
}

void MeetingDTS::ClearMeetingData(void)
{
	XAutoLock l(m_csMapObjects);
	MapMETObjects::iterator it = m_MapObjects.begin();
	while(it != m_MapObjects.end()) 
	{
		OBJMETDATA omd = (*it).second;
		if(omd.pData)
		{
			delete []omd.pData;
			omd.pData = NULL;
		}
		it++;
	}
	m_MapObjects.clear();
}

//---------------------------------------------------------------------------------------
void MeetingDTS::SaveMeetingObj(int nLevel,std::string strMainKey,std::string strSubKey,char* pData,int nLen,unsigned int nIndex)
{
	if (pData == NULL || nLen < 1)
	{
		return;
	}

	std::string strKey = strMainKey + strSubKey;
	
	if (strKey.length() < 1)
	{
		return;
	}

	OBJMETDATA ObjBuf;
	ObjBuf.nLevel		= nLevel;
	ObjBuf.strMainKey	= strMainKey;
	ObjBuf.strSubKey	= strSubKey;
	ObjBuf.nLen			= nLen;
	ObjBuf.nIndex		= nIndex;
	ObjBuf.pData		= new char[ObjBuf.nLen];
	if (ObjBuf.pData)
	{
		memcpy(ObjBuf.pData, pData, ObjBuf.nLen);
		XAutoLock l(m_csMapObjects);	
		m_MapObjects[strKey]=ObjBuf;
	}
}

//---------------------------------------------------------------------------------------
unsigned int MeetingDTS::DelMeetingObj(std::string strMainKey,std::string strSubKey)
{
	std::string strKey = strMainKey + strSubKey;
	unsigned int nIndex = 0;

	if (strKey.length() < 1)
	{
		return nIndex;
	}

	XAutoLock l(m_csMapObjects);
	MapMETObjects::iterator it = m_MapObjects.find(strKey);
	if(it != m_MapObjects.end()) 
	{
		OBJMETDATA omd = (*it).second;
		if(omd.pData)
		{
			nIndex = omd.nIndex;
			delete []omd.pData;
			omd.pData = NULL;
		}
		m_MapObjects.erase(it);
	}
	return nIndex;
}

//---------------------------------------------------------------------------------------
void MeetingDTS::SendMeetingObj(unsigned long ulSessionID,int nLevel,bool bOnlyThis)
{
	XBufferPool tXBufferPool;
	{
		XAutoLock l(m_csMapObjects);
		MapMETObjects::iterator it = m_MapObjects.begin();
		while(it != m_MapObjects.end()) 
		{
			OBJMETDATA omd = (*it).second;
			if(omd.pData)
			{
				if ((bOnlyThis && omd.nLevel == nLevel) || (!bOnlyThis && omd.nLevel >= nLevel))
				{
					AVDataOutPacket outPacket(true,omd.nLen + 1024);
					outPacket << (UINT)MET_DTS_RESPONSE;			
					outPacket << (UINT)omd.nLevel;
					outPacket << omd.strMainKey.c_str();
					outPacket << omd.strSubKey.c_str();
					outPacket << omd.nIndex;
					outPacket.WriteData((unsigned char*)omd.pData,omd.nLen);

					XDataBuffer*pXDataBuffer=new XDataBuffer();
					pXDataBuffer->StoreData((char *)outPacket.GetData(),outPacket.GetLength());
					tXBufferPool.SetDataBuffer(pXDataBuffer);				
				}
			}
			it++;
		}
	}
	while(tXBufferPool.GetCount())
	{
		XDataBuffer*pXDataBuffer=tXBufferPool.GetDataBuffer();
		m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,pXDataBuffer->GetData(),pXDataBuffer->GetLen());	
		delete pXDataBuffer;
		pXDataBuffer=NULL;
	}
}

//---------------------------------------------------------------------------------------
void MeetingDTS::DeliverMeetingData(unsigned long ulSessionID,const char* pData,int nLen)
{
	MapULong mapTem;

	{
		XAutoLock l(m_csMapSessionID);
		MapULong::iterator it = m_MapSessionID.begin();
		while(it != m_MapSessionID.end())
		{
			if((*it).second != ulSessionID)
			{
				unsigned long ulSID = (*it).second;
				mapTem[ulSID] = ulSID;
			}
			it++;
		}
	}
	MapULong::iterator itMap = mapTem.begin();
	while (itMap != mapTem.end())
	{
		unsigned long ulSID = (*itMap).first;
		++itMap;
		m_rIHPDTSNotify.onDTSNetDataNotify(ulSID,pData,nLen);
	}
	mapTem.clear();	
}