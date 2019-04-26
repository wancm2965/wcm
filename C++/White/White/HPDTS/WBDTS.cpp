//========================================================================================
//版权所有: 版权所有(C) 2008，上海华平信息技术股份有限公司
//系统名称: AVCON7
//作　　者: duquansheng
//创建日期: 2008-3-31
//功能说明: 白板内核
//

#include "stdafx.h"
#include "WbDTS.h"
#include "CharArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------------------------
WbDTS::WbDTS(IHPDTSNotify&rIHPDTSNotify,bool bUI)
:m_rIHPDTSNotify(rIHPDTSNotify)
,m_bUI(bUI)
{
	memset(&m_cwPos, 0, sizeof(m_cwPos));	
	addChannel(LOCAL_SESSIONID);
}

//---------------------------------------------------------------------------------------
WbDTS::~WbDTS(void)
{	
	ClearAllObject();
}

//---------------------------------------------------------------------------------------
void WbDTS::deliverGroupWbData(unsigned long channelId,char * data,int len)
{
	std::map<unsigned long,unsigned long> mapTem;

	{
		XAutoLock l(m_csMapChsId);
		MapULong2ULong::iterator it;
		for(it = m_MapChsId.begin(); it != m_MapChsId.end(); ++it){
			if((*it).second != channelId)
			{
				unsigned long ulSID = (*it).second;
				mapTem[ulSID] = ulSID;
			}
		}
	}
	std::map<unsigned long,unsigned long>::iterator itMap = mapTem.begin();
	while (itMap != mapTem.end())
	{
		unsigned long ulSID = (*itMap).first;
		SendWbData(ulSID,data,len);
		itMap++;
	}
	mapTem.clear();	
}

void WbDTS::SendWbData(unsigned long channelId,char* data,int len)
{
	if (channelId == LOCAL_SESSIONID)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify(data,len);
	}
	else
	{
		AVDataOutPacket outPacket;
		outPacket << (UINT)WB_COMMAND_DATA;
		outPacket << (UINT)len;
		outPacket.WriteData((unsigned char*)data,len);
		m_rIHPDTSNotify.onDTSNetDataNotify(channelId,(char*)outPacket.GetData(),outPacket.GetLength());
	}
}


//---------------------------------------------------------------------------------------
void WbDTS::addChannel(unsigned long channelId)
{
	{
		XAutoLock l(m_csMapChsId);
		//m_MapChsId[channelId]=(channelId);
		m_MapChsId.insert(MapULong2ULong::value_type(channelId,channelId));
	}
	
	m_csMapWBObjects.Lock();
	int count=m_MapWBObjects.size();
	MapWBObjects::iterator it;

	if(count > 0 ){
		int size = sizeof(unsigned long);
		int i = 0;
		int sndLen = 0;
		sndLen = count * size + sizeof(WB_COMMAND_HEAD);
		WB_COMMAND_HEAD head;
		memset(&head,0,sizeof(head));
		head.byType = DATA_TRANS_OBJECT_ALL;
		CharArray ca(sndLen + 1);
		memcpy(ca.buf,&head,sizeof(head));


		for(it = m_MapWBObjects.begin(); it != m_MapWBObjects.end(); ++it)
		{
			*(unsigned long*)(ca.buf + sizeof(head) + i*size) = (*it).first;
			i++;
		}
		m_csMapWBObjects.UnLock();

		SendWbData(channelId,ca.buf,sndLen);
	}
	else
	{
		m_csMapWBObjects.UnLock();
	}
	if (m_cwPos.bFullScreen)
	{
		WB_COMMAND_HEAD wbCmdHead;
		memset(&wbCmdHead, 0, sizeof(wbCmdHead));
		wbCmdHead.byType= DATA_TRANS_FULLSCREEN;

		int nSendLen = sizeof(wbCmdHead) + 5;
		char *pSendData = new char[nSendLen];
		memcpy(pSendData, &wbCmdHead, sizeof(wbCmdHead));
		int nCommand = wbCmdHead.byType;

		memcpy(pSendData + sizeof(WB_COMMAND_HEAD), &nCommand, 4);
		memcpy(pSendData + sizeof(wbCmdHead) + 4 ,	&(m_cwPos.bFullScreen),	1);
		SendWbData(channelId,pSendData,nSendLen);
		delete []pSendData;
		pSendData = NULL;
	}
	if (m_bUI && channelId != 1)
	{
		WB_COMMAND_HEAD wbSendCmdHead;
		memset(&wbSendCmdHead, 0, sizeof(wbSendCmdHead));
		wbSendCmdHead.byType = DATA_TRANS_REQUESTLASTPOSITION;
		SendWbData(channelId,(char*)&wbSendCmdHead,sizeof(WB_COMMAND_HEAD));
	}
}

//---------------------------------------------------------------------------------------
void WbDTS::removeChannel(unsigned long channelId)
{	
	XAutoLock l(m_csMapChsId);
	m_MapChsId.erase(channelId);
}

//---------------------------------------------------------------------------------------

void WbDTS::processWbData(unsigned long channelId,char * data,int len)
{
	if(len < sizeof(WB_COMMAND_HEAD))
		return;
	PWB_COMMAND_HEAD pWbHead = (PWB_COMMAND_HEAD)data;
	switch(pWbHead->byType)
	{
		case DATA_TRANS_OBJ_RESIZE:
			processResizeObj(channelId,data,len);
		break;
		case DATA_TRANS_ADDOBJECT:
			processAddObj(channelId,pWbHead);
			break;
		case DATA_TRANS_DELETE_ALL:
			processDeleteAll(channelId,data,len);
			break;
		case DATA_TRANS_DELETE_OBJECT:
			processDeleteObj(channelId,data,len);
			break;
		case DATA_TRANS_OBJREQUEST:
			processObjRequest(channelId,pWbHead);
			break;
		case DATA_TRANS_OBJRESPONSE:
			processObjResponse(channelId,data,len);
			break;	
		case DATA_TRANS_PAGE:
			processPageChanged(channelId,data,len);
			break;
		case DATA_TRANS_SCROLL:
			processScroll(channelId,data,len);
			break;
		case DATA_TRANS_MOVEOBJ:
			processMoveObj(channelId,data,len);
			break;
		case DATA_TRANS_LASTPOSITION:
			processTransLastPosition(channelId,data,len);
			break;
		case DATA_TRANS_TABCTRL_CHANGE:
			processTabCtrlChange(channelId,data,len);
			break;
		case DATA_TRANS_FULLSCREEN:
			processFullScreen(channelId,data,len);
			break;
		case DATA_TRANS_TOOLBARMOVE:
			processToolBarMove(channelId,data,len);
			break;
		case DATA_TRANS_OBJECT_ALL:
			processTransAllObj(channelId,data,len);
			break;
		case DATA_TRANS_REQUESTLASTPOSITION:
			processTransLastPositionRequest(channelId,data,len);
			break;
		case DATA_TRANS_RESPONSELASTPOSITION:
			processTransLastPositionResponse(channelId,data,len);
			break;
		case DATA_TRANS_DTOBJ_HEAD:
			processTransDTObjHead(channelId,data,len);
			break;
			//added by luoning 2010-7-8
		case DATA_TRANS_ROATE:
			processObjRoate(channelId,data,len);
			break;
		case DATA_TRANS_DELETE_OBJATTRIBUTE:
			processDeleteObjAttribute(channelId,data,len);
			break;
		case DATA_TRANS_DELETE_ALLOBJATTRIBUTE:
			processDeleteAllObjAttribute(channelId,data,len);
			break;
		case DATA_TRANS_PUT_OBJATTRIBUTE:
			processPutObjAttribute(channelId,data,len);
			break;
		case DATA_TRANS_GET_OBJATTRIBUTE:
			processGetObjAttribute(channelId,data,len);
			break;
		case DATA_TRANS_GET_ALLOBJATTRIBUTE:
			processGetAllObjAtribute(channelId,data,len);
			break;
		case DATA_TRANS_PREMOVE_LAYOUT://上移一层
			processPreMoveLayout(channelId,data,len);
			break;
		case DATA_TRANS_BACKMOVE_LAYOUT://下移一层
			processBackMoveLayout(channelId,data,len);
			break;
		case DATA_TRANS_MOVETOHEAD_LAYOUT:
			{
				processMoveToHeadLayout(channelId,data,len);
			}
			break;
		case DATA_TRANS_MOVETOTAIL_LAYOUT:
			{
				processMoveToTailLayout(channelId,data,len);
			}
			break;
		case DATA_TRANS_BK_LAYOUT:
			{
				processBKLayout(channelId,data,len);
			}
			break;
			//end
			//added by luoning 2010-8-4
		case DATA_TRANS_REQUESTBKCOLOR:
			{
				processRequestBKColor(channelId,data,len);
			}
			break;
		case DATA_TRANS_SET_BACKCOLOR:
			{
				processBKColor(channelId,data,len);
			}
			break;
			//end
		case TRANS_RESPONSEDTOBJDATA:
			processTransDTObjResponse(channelId,data,len);
			break;
		case TRANS_REQUESTDTOBJDATA:
			processTransDTObjRequest(channelId,data,len);
			break;
			//}}add end
		default:
			deliverGroupWbData(channelId,data,len);
			break;
	}
}

//---------------------------------------------------------------------------------------
void WbDTS::processAddObj(unsigned long channelId,PWB_COMMAND_HEAD pWbHead)
{
	WB_COMMAND_HEAD wbCmdHead;
	memset(&wbCmdHead, 0, sizeof(wbCmdHead));
	wbCmdHead.byPageId = pWbHead->byPageId;
	wbCmdHead.ulObjId = pWbHead->ulObjId;
	wbCmdHead.byType = DATA_TRANS_OBJREQUEST;
	SendWbData(channelId,(char *)&wbCmdHead, sizeof(wbCmdHead));
}

//---------------------------------------------------------------------------------------
void WbDTS::ClearAllObject(void)
{
	{
		XAutoLock l(m_csMapWBObjects);
		MapWBObjects::iterator it;
		for(it = m_MapWBObjects.begin(); it != m_MapWBObjects.end(); ++it) {
			OBJDATABUF odb = (*it).second;
			if(odb.pData)
			{
				int		nObjType = 0;
				memcpy(&nObjType, odb.pData + sizeof(WB_COMMAND_HEAD) + 4 , sizeof(int));
				if(nObjType == SHAPE_TEXT || nObjType == SHAPE_BITMAP)
				{
					char csFileName[MAX_PATH] ; 
					GetBigFileName((*it).first,csFileName);
					remove(csFileName);
				}
				delete []odb.pData;
				odb.pData = NULL;
			}
		}
		m_MapWBObjects.clear();
	}

	//added by luoning 2010-8-7
	{
		XAutoLock l(m_csMapWBObjAttribute);
// 		MapWBObjects::iterator it;
// 		for (it = m_mapWBObjAttribute.begin(); it != m_mapWBObjAttribute.end();it++)
// 		{
// 			OBJDATABUF odb = it->second;
// 			if (odb.pData)
// 			{
// 				delete []odb.pData;
// 				odb.pData = NULL;
// 			}
// 		}
		m_mapWBObjAttribute.clear();
	}
	//end
	{
		XAutoLock l(m_csUndoStack);
		for (VectorStack::iterator iter = m_vecUndoStack.begin(); iter != m_vecUndoStack.end(); iter++)
		{
			OBJDATABUF objData = *iter;
			ASSERT(objData.pData);
			if (objData.pData)
			{
				delete objData.pData;
				objData.pData = NULL;
			}			
		}	
		m_vecUndoStack.clear();
	}

	{
		XAutoLock l(m_csRedoStack);
		for (VectorStack::iterator iter = m_vecRedoStack.begin(); iter != m_vecRedoStack.end(); iter++)
		{
			OBJDATABUF objData = *iter;
			ASSERT(objData.pData);
			if (objData.pData)
			{
				delete objData.pData;
				objData.pData = NULL;
			}			
		}		
		m_vecRedoStack.clear();
	}
}

void WbDTS::PushRedoStack(const WB_COMMAND_HEAD& wbCmdHead)
{
	XAutoLock l(m_csRedoStack);
	OBJDATABUF ObjBuf;
	memset(&ObjBuf, 0, sizeof(ObjBuf));
	ObjBuf.nLen = sizeof(wbCmdHead);
	ObjBuf.pData = new char[ObjBuf.nLen];
	memcpy(ObjBuf.pData, &wbCmdHead, ObjBuf.nLen);
	m_vecRedoStack.push_back(ObjBuf);
}

bool WbDTS::IsHasReDo()
{
	XAutoLock l(m_csRedoStack);
	return (m_vecRedoStack.size() != 0);
}

bool WbDTS::PopRedoStack(WB_COMMAND_HEAD& wbCmdHead)
{
	XAutoLock l(m_csRedoStack);
	if (m_vecRedoStack.size() == 0)
	{
		return false;
	}
	OBJDATABUF ObjBuf = m_vecRedoStack.back();
	ASSERT(ObjBuf.pData);
	if (ObjBuf.pData)
	{
		PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)ObjBuf.pData;
		if (pHead)
		{			
			wbCmdHead.byPageId = pHead->byPageId;
			wbCmdHead.ulObjId = pHead->ulObjId;
			wbCmdHead.byType = pHead->byType;
			wbCmdHead.byTabId = pHead->byTabId;		
			delete ObjBuf.pData;
			ObjBuf.pData = NULL;
			return true;
		}
	}	
	m_vecRedoStack.pop_back();
	return false;
}

void WbDTS::PushUndoStack(const WB_COMMAND_HEAD& wbCmdHead)
{
	XAutoLock l(m_csUndoStack);
	OBJDATABUF ObjBuf;
	memset(&ObjBuf, 0, sizeof(ObjBuf));
	ObjBuf.nLen = sizeof(wbCmdHead);
	ObjBuf.pData = new char[ObjBuf.nLen];
	memcpy(ObjBuf.pData, &wbCmdHead, ObjBuf.nLen);
	m_vecUndoStack.push_back(ObjBuf);
}

bool WbDTS::IsHasUnDo()
{
	XAutoLock l(m_csRedoStack);
	return (m_vecUndoStack.size() != 0);
}

bool WbDTS::PopUndoStack(WB_COMMAND_HEAD& wbCmdHead)
{

	XAutoLock l(m_csUndoStack);
	if (m_vecUndoStack.size() == 0)
	{
		return false;
	}
	OBJDATABUF ObjBuf = m_vecUndoStack.back();
	ASSERT(ObjBuf.pData);
	if (ObjBuf.pData)
	{
		PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)ObjBuf.pData;
		if (pHead)
		{			
			wbCmdHead.byPageId = pHead->byPageId;
			wbCmdHead.ulObjId = pHead->ulObjId;
			wbCmdHead.byType = pHead->byType;
			wbCmdHead.byTabId = pHead->byTabId;		
			delete ObjBuf.pData;
			ObjBuf.pData = NULL;
			return true;
		}
	}	
	m_vecUndoStack.pop_back();
	return false;
}

//---------------------------------------------------------------------------------------
void WbDTS::processDeleteAll(unsigned long channelId,char* data,int len)
{
	ClearAllObject();

	memset(&m_cwPos, 0, sizeof(m_cwPos));	

	deliverGroupWbData(channelId,data, len);
}

//---------------------------------------------------------------------------------------
void WbDTS::processDeleteObj(unsigned long channelId,char* data,int len)
{
	{
		XAutoLock l(m_csMapWBObjects);
		PWB_COMMAND_HEAD pWbHead = (PWB_COMMAND_HEAD)data;
		for(int i = sizeof(WB_COMMAND_HEAD) + sizeof(unsigned long); i < len; i += sizeof(unsigned long))
		{
			unsigned long dwObjID = *(unsigned long*)(data + i);
			MapWBObjects::iterator it;
			for(it = m_MapWBObjects.begin(); it != m_MapWBObjects.end(); ++it)
			{
				OBJDATABUF odb = (*it).second;

				if(odb.pData)
				{
					PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)odb.pData;
					if(pHead && pHead->byPageId == pWbHead->byPageId && pHead->ulObjId == dwObjID)
					{
						int		nObjType = 0;
						memcpy(&nObjType, odb.pData + sizeof(WB_COMMAND_HEAD) + 4 , sizeof(int));
						if(nObjType == SHAPE_TEXT || nObjType == SHAPE_BITMAP)
						{
							char csFileName[MAX_PATH] ; 
							GetBigFileName(pHead->ulObjId,csFileName);
							remove(csFileName);
						}
						m_MapWBObjects.erase(it);		
						delete []odb.pData;
						odb.pData = NULL;
						break;
					}
				}
			}
		}
	}
	//added by luoning 2010-7-8
	{
		XAutoLock l(m_csMapWBObjAttribute);
		PWB_COMMAND_HEAD pWbHead = (PWB_COMMAND_HEAD)data;
		for(int i = sizeof(WB_COMMAND_HEAD) + sizeof(unsigned long); i < len; i += sizeof(unsigned long))
		{
			unsigned long dwObjID = *(unsigned long*)(data + i);
			MapWBAttributes::iterator it;
			it = m_mapWBObjAttribute.find(dwObjID);
			if (it != m_mapWBObjAttribute.end())
			{
				m_mapWBObjAttribute.erase(it);
			}
		}
	}
	//end
	deliverGroupWbData(channelId,data, len);
}

//---------------------------------------------------------------------------------------
void WbDTS::processObjRequest(unsigned long channelId,PWB_COMMAND_HEAD pWbHead)
{
	OBJDATABUF odb;
	bool bRet = false;
	{
		XAutoLock l(m_csMapWBObjects);
		MapWBObjects::iterator it;
		for(it = m_MapWBObjects.begin(); it != m_MapWBObjects.end(); ++it) 
		{
			odb = (*it).second;
			if(odb.pData)
			{	
				PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)odb.pData;
				if(pHead && pHead->ulObjId == pWbHead->ulObjId)
				{
					bRet = true;				
					break;
				}
			}
		}
	}
	if (bRet)
	{
		SendWbData(channelId,odb.pData, odb.nLen);
	}
}

//---------------------------------------------------------------------------------------
void WbDTS::processObjResponse(unsigned long channelId,char* data,int len)
{
	WB_COMMAND_HEAD wbCmdHead;
	{
		XAutoLock l(m_csMapWBObjects);
		OBJDATABUF ObjBuf;
		memset(&ObjBuf, 0, sizeof(ObjBuf));
		ObjBuf.nLen = len;
		ObjBuf.pData = new char[len];
		memcpy(ObjBuf.pData, data, ObjBuf.nLen);

		PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)ObjBuf.pData;
		
		memset(&wbCmdHead, 0, sizeof(wbCmdHead));
		wbCmdHead.byPageId = pHead->byPageId;
		wbCmdHead.ulObjId = pHead->ulObjId;
		wbCmdHead.byType = DATA_TRANS_ADDOBJECT;

		m_MapWBObjects[pHead->ulObjId]=ObjBuf;
	}

	deliverGroupWbData(channelId,(char *)&wbCmdHead, sizeof(wbCmdHead));
}


//---------------------------------------------------------------------------------------
void WbDTS::processPageChanged(unsigned long channelId,char* data,int len)
{
	PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)data;
	if (m_cwPos.dwWhiteBoardTotalPage < pHead->byPageId)
	{
		m_cwPos.dwWhiteBoardTotalPage = pHead->byPageId;
	}

	m_cwPos.dwWhiteBoardPage = pHead->byPageId;
	m_cwPos.ptWhiteBoard.x = m_cwPos.ptWhiteBoard.y = 0;
	deliverGroupWbData(channelId,data, len);
}

//---------------------------------------------------------------------------------------
void WbDTS::processScroll(unsigned long channelId,char* data,int len)
{
	PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)data;
	switch(pHead->byTabId) 
	{
	case TAB_CTRL_WHITEBOARD:
		m_cwPos.ptWhiteBoard.x = *((int*)(data + sizeof(WB_COMMAND_HEAD) + 4));
		m_cwPos.ptWhiteBoard.y = *((int*)(data + sizeof(WB_COMMAND_HEAD) + 8));
		break;
	}
	deliverGroupWbData(channelId,data, len);
}
//--------------------------added by luoning  2010-7-8------------------------------------
bool WbDTS::BeInDeleteAttribute(int iCmd,char* pStrCmd,int iLen)
{
	int iPos = 0;
	while (iPos<iLen)
	{
		int iTempCmd = *((int*)(pStrCmd + iPos));
		if (iCmd == iTempCmd)
		{
			return true;
		}
		iPos += sizeof(int);
	}
	return false;
}
void WbDTS::processObjRoate(unsigned long channelId,char* data,int len)
{

}
void WbDTS::processDeleteObjAttribute(unsigned long channelId,char* data,int len)//删除对象的某个属性
{
	char* pData = data + sizeof(WB_COMMAND_HEAD) + 4;
	int iLen = len - sizeof(WB_COMMAND_HEAD) - 4;
	int iDataSize = *((int*)pData);
	pData += sizeof(int);
	unsigned long dwObjID = 0;
	dwObjID = *((unsigned long*)(pData + iDataSize));
	{
		XAutoLock l(m_csMapWBObjAttribute);
		char* pBuffer = new char[iDataSize];
		if (pBuffer == NULL)
		{
			return;
		}
		memset(pBuffer,0,iDataSize);
		memcpy(pBuffer,pData,iDataSize);
		
		MapWBAttributes::iterator it;
		it = m_mapWBObjAttribute.find(dwObjID);
		if (it != m_mapWBObjAttribute.end())
		{
			XMemIni* pObjAttribute = &(it->second);
			pObjAttribute->DeleteKeys(pBuffer);
			deliverGroupWbData(channelId,data,len);
		}
	}
}
void WbDTS::processDeleteAllObjAttribute(unsigned long channelId,char* data,int len)//删除对象的全部属性
{
	char* pData = data + sizeof(WB_COMMAND_HEAD) + 8;
	int iLen = len - sizeof(WB_COMMAND_HEAD) - 8;
	unsigned long dwObjID = 0;
	dwObjID = *((unsigned long*)pData);
	{
		XAutoLock l(m_csMapWBObjects);
		MapWBAttributes::iterator it;
		it = m_mapWBObjAttribute.find(dwObjID);
		if (it != m_mapWBObjAttribute.end())
		{
			XMemIni* pObjAttribute = &(it->second);
			if (pObjAttribute)
			{
				pObjAttribute->DeleteAllKey();
			}
			deliverGroupWbData(channelId,data,len);
		}
	}
 }
void WbDTS::processPutObjAttribute(unsigned long channelId,char* data,int len)
{
	char* pData = data + sizeof(WB_COMMAND_HEAD) + 4;
	int iLen = len - sizeof(WB_COMMAND_HEAD) - 4;
	int iSize = 0;
	memcpy(&iSize,pData,sizeof(int));
	if (iSize!=0)
	{
		pData = pData + sizeof(int);

		{
			XAutoLock l(m_csMapWBObjAttribute);
			unsigned long dwObjID = 0;	
			dwObjID = *((unsigned long*)(pData + iSize));
		
			MapWBAttributes::iterator it;
			it = m_mapWBObjAttribute.find(dwObjID);
			if (it != m_mapWBObjAttribute.end())
			{
				//找到了
				char* pBuffer = new char[iSize +1];
				if (pBuffer)
				{
					memset(pBuffer,0,iSize + 1);
					memcpy(pBuffer,pData,iSize);
					it->second.SetString(pBuffer);
					delete []pBuffer;
				}
			}
			else
			{
				
				char* pBuffer = new char[iSize + 1];
				if (pBuffer)
				{
					XMemIni objAttribute;
					memset(pBuffer,0,iSize +1);
					memcpy(pBuffer,pData,iSize);
					objAttribute.ResetString(pBuffer);
					m_mapWBObjAttribute[dwObjID] = objAttribute;
					delete []pBuffer;
				}	
			}
		}
		//SendWbData(channelId,data, len);
		deliverGroupWbData(channelId,data, len);
	}
}
void WbDTS::processGetAllObjAtribute(unsigned long channelId,char* data,int len)
{
	MapWBAttributes::iterator it;
	for (it = m_mapWBObjAttribute.begin();it!=m_mapWBObjAttribute.end();it++)
	{
		unsigned long dwObjID = it->first;
		XMemIni attribute = it->second;
		std::string strData = "";
		attribute.GetString(strData);
		if (strData != "")
		{
			int iLength = sizeof(WB_COMMAND_HEAD) + 4 + sizeof(int) + strData.size() + sizeof(unsigned long);
			char* pBuffer = NULL;
			pBuffer = new char[iLength];
			if (pBuffer)
			{
				memset(pBuffer,0,iLength);
				memcpy(pBuffer,data,sizeof(WB_COMMAND_HEAD) + 4);
				memcpy(pBuffer + sizeof(WB_COMMAND_HEAD) + 4,strData.c_str(),strData.size());
				memcpy(pBuffer + sizeof(WB_COMMAND_HEAD) + 4 + strData.size(), &dwObjID,sizeof(unsigned long));
				SendWbData(channelId,pBuffer,iLength);
				delete []pBuffer;
			}
		}
		
	}
}
void WbDTS::processGetObjAttribute(unsigned long channelId,char* data,int len)//获取一次只能获取以个对象的
{
	char* pData = data + sizeof(WB_COMMAND_HEAD) + 4;
	int iLen = len - sizeof(WB_COMMAND_HEAD) - 4;
	int iSize = 0;
	memcpy(&iSize,pData,sizeof(int));
	if (iSize!=0)
	{
		unsigned long	dwObjID = 0;
		pData += sizeof(int);
		dwObjID = *((unsigned long*)(pData+iSize)); 
		char* pBuffer = new char[iSize];
		if (pBuffer == NULL)
		{
			return;
		}
		memset(pBuffer,0,iSize);
		memcpy(pBuffer,pData,iSize);
		{
			XAutoLock l(m_csMapWBObjAttribute);
			MapWBAttributes::iterator it;
			it = m_mapWBObjAttribute.find(dwObjID);
			if (it != m_mapWBObjAttribute.end())
			{
				memcpy(pBuffer,pData,iSize);
				XMemIni objAttribute = it->second;
				std::string strValue = "";
				std::string strKey = pBuffer;
				objAttribute.GetSubString(pBuffer,iSize,strValue);
				int iLenght = strValue.size();
				if (strValue.size()>0)
				{
					int iDesLen = sizeof(WB_COMMAND_HEAD) + 4 + 4 + iLenght + sizeof(unsigned long);
					char* pDesData = new char[iDesLen];
					memset(pDesData,0,iDesLen);
					memcpy(pDesData,data,sizeof(WB_COMMAND_HEAD) + 4);
					memcpy(pDesData + sizeof(WB_COMMAND_HEAD) + 4,(char*)&iLenght,sizeof(int));
					memcpy(pDesData + sizeof(WB_COMMAND_HEAD) + 8,strValue.c_str(),iLenght);
					memcpy(pDesData + sizeof(WB_COMMAND_HEAD) + 8 +iLenght,(char*)&dwObjID,sizeof(unsigned long));
					SendWbData(channelId,pDesData,iDesLen);
					delete []pDesData;
				}
			}
		}
		delete []pBuffer;
	}
}
bool WbDTS::beBKAttribute(unsigned long dwObjID)
{
	MapWBAttributes::iterator it = m_mapWBObjAttribute.find(dwObjID);
	if (it != m_mapWBObjAttribute.end())
	{
		XMemIni attribute = it->second;
		unsigned short iValue = 0;
		if (attribute.Get(11,iValue))
		{
			if (iValue == 1)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return false;
}

void WbDTS::processPreMoveLayout(unsigned long channelId,char* data,int len)//上移一层
{
	WB_COMMAND_HEAD tHead;
	memcpy(&tHead,data,sizeof(WB_COMMAND_HEAD));

	char* pData = data + sizeof(WB_COMMAND_HEAD) + 4;
	int iLen = len - sizeof(WB_COMMAND_HEAD) - 4;
	int iSize = 0;
	memcpy(&iSize,pData,sizeof(int));
	pData += sizeof(int);
	if (iSize!=0)
	{
		unsigned long dwObjID1 = 0;
		int iLayOutID1 = 0;
		dwObjID1 = *((unsigned long*)pData);
		iLayOutID1 = *((int*)(pData + sizeof(unsigned long)));
		unsigned long dwObjID2 = 0;
		int iLayOutID2 = 0;
		dwObjID2 = *((unsigned long*)(pData + sizeof(unsigned long) + sizeof(int)));
		iLayOutID2 = *((int*)(pData + 2*sizeof(unsigned long) + sizeof(int)));
		MapWBAttributes::iterator it = m_mapWBObjAttribute.find(dwObjID1);
		if (it != m_mapWBObjAttribute.end())
		{
			it->second.Set(100,iLayOutID1);
		}
		it = m_mapWBObjAttribute.find(dwObjID2);
		if (it != m_mapWBObjAttribute.end())
		{
			it->second.Set(100,iLayOutID2);
		}
	}
	deliverGroupWbData(channelId,data, len);
}
void WbDTS::processBackMoveLayout(unsigned long channelId,char* data,int len)//下移一层
{
	WB_COMMAND_HEAD tHead;
	memcpy(&tHead,data,sizeof(WB_COMMAND_HEAD));

	char* pData = data + sizeof(WB_COMMAND_HEAD) + 4;
	int iLen = len - sizeof(WB_COMMAND_HEAD) - 4;
	int iSize = 0;
	memcpy(&iSize,pData,sizeof(int));
	pData += sizeof(int);
	if (iSize!=0)
	{
		unsigned long dwObjID1 = 0;
		int iLayOutID1 = 0;
		dwObjID1 = *((unsigned long*)pData);
		iLayOutID1 = *((int*)(pData + sizeof(unsigned long)));
		unsigned long dwObjID2 = 0;
		int iLayOutID2 = 0;
		dwObjID2 = *((unsigned long*)(pData + sizeof(unsigned long) + sizeof(int)));
		iLayOutID2 = *((int*)(pData + 2*sizeof(unsigned long) + sizeof(int)));
		MapWBAttributes::iterator it = m_mapWBObjAttribute.find(dwObjID1);
		if (it != m_mapWBObjAttribute.end())
		{
			it->second.Set(100,iLayOutID1);
		}
		it = m_mapWBObjAttribute.find(dwObjID2);
		if (it != m_mapWBObjAttribute.end())
		{
			it->second.Set(100,iLayOutID2);
		}
	}
	deliverGroupWbData(channelId,data, len);
}
void WbDTS::processBKLayout(unsigned long channelId,char* data,int len)
{
	WB_COMMAND_HEAD tHead;
	memcpy(&tHead,data,sizeof(WB_COMMAND_HEAD));

	char* pData = data + sizeof(WB_COMMAND_HEAD) + 4;
	int iLen = len - sizeof(WB_COMMAND_HEAD) - 4;
	int iSize = 0;
	memcpy(&iSize,pData,sizeof(int));
	pData += sizeof(int);
	if (iSize!=0)
	{
		unsigned long dwObjID = *((unsigned long*)pData);
		int iLayOutID = *((int*)(pData + sizeof(unsigned long)));
		MapWBAttributes::iterator it = m_mapWBObjAttribute.find(dwObjID);
		if (it != m_mapWBObjAttribute.end())
		{
			it->second.Set(100,iLayOutID);//layout
			int bBK = -1;
			bool bHave = it->second.Get(11,bBK);
			if (bBK == 1)
			{
				it->second.Set(11,0);
			}
			else if (bBK == 0 || bHave == false)
			{
				MapWBAttributes::iterator iter = m_mapWBObjAttribute.begin();
				while (iter != m_mapWBObjAttribute.end())
				{
					int flag = -1;
					bool bExit = iter->second.Get(11,flag);
					if (bExit == true && flag == 1)
					{
						iter->second.Set(11,0);
					}
					iter++;
				}
				it->second.Set(11,1);
			}
		}
	}
	deliverGroupWbData(channelId,data, len);
}
void WbDTS::processMoveToHeadLayout(unsigned long channelId,char* data,int len)
{
	WB_COMMAND_HEAD tHead;
	memcpy(&tHead,data,sizeof(WB_COMMAND_HEAD));

	char* pData = data + sizeof(WB_COMMAND_HEAD) + 4;
	int iLen = len - sizeof(WB_COMMAND_HEAD) - 4;
	int iSize = 0;
	memcpy(&iSize,pData,sizeof(int));
	pData += sizeof(int);
	if (iSize!=0)
	{
		unsigned long dwObjID1 = 0;
		int iLayOutID1 = 0;
		dwObjID1 = *((unsigned long*)pData);
		iLayOutID1 = *((int*)(pData + sizeof(unsigned long)));
		unsigned long dwObjID2 = 0;
		int iLayOutID2 = 0;
		dwObjID2 = *((unsigned long*)(pData + sizeof(unsigned long) + sizeof(int)));
		iLayOutID2 = *((int*)(pData + 2*sizeof(unsigned long) + sizeof(int)));
		MapWBAttributes::iterator it = m_mapWBObjAttribute.find(dwObjID1);
		if (it != m_mapWBObjAttribute.end())
		{
			it->second.Set(100,iLayOutID1);
		}
		if (dwObjID2 != 0)
		{
			it = m_mapWBObjAttribute.find(dwObjID2);
			if (it != m_mapWBObjAttribute.end())
			{
				it->second.Set(100,iLayOutID2);
			}
		}
	}
	deliverGroupWbData(channelId,data, len);
}
void WbDTS::processMoveToTailLayout(unsigned long channelId,char* data,int len)
{
	WB_COMMAND_HEAD tHead;
	memcpy(&tHead,data,sizeof(WB_COMMAND_HEAD));

	char* pData = data + sizeof(WB_COMMAND_HEAD) + 4;
	int iLen = len - sizeof(WB_COMMAND_HEAD) - 4;
	int iSize = 0;
	memcpy(&iSize,pData,sizeof(int));
	pData += sizeof(int);
	if (iSize!=0)
	{
		unsigned long dwObjID = 0;
		int iLayOutID = 0;
		dwObjID = *((unsigned long*)pData);
		iLayOutID = *((int*)(pData + sizeof(unsigned long)));
		MapWBAttributes::iterator it;
		it = m_mapWBObjAttribute.find(dwObjID);
		if (it != m_mapWBObjAttribute.end())
		{
			it->second.Set(100,iLayOutID);
		}
	}
	deliverGroupWbData(channelId,data, len);
}
int WbDTS::FindCmdData(int iCmdType,char* strCmd,int ilen)
{
	int iPos = 0;
	int iCmd = 0;
	int iSize = 0;
	while (iPos <ilen)
	{
		iCmd = *((int*)(strCmd + iPos));
		iSize = *((int*)(strCmd + iPos +sizeof(int)));
		//
		if (iCmd == iCmdType)
		{
			return iPos;
		}
		//
		iPos = iPos + sizeof(int) + sizeof(int) + iSize;
	}
	return -1;
}
//added by luoning 2010-8-4
void WbDTS::processRequestBKColor(unsigned long channelId,char* data,int len)
{
	unsigned long dwObjID = 0;
	{
		XAutoLock l(m_csMapWBObjAttribute);
		MapWBAttributes::iterator it;
 		it = m_mapWBObjAttribute.find(dwObjID);
 		if (it != m_mapWBObjAttribute.end())
 		{
			int iColor = 0;
 			it->second.Get(101,iColor);

			int iSize = sizeof(WB_COMMAND_HEAD) + 8;
			WB_COMMAND_HEAD head;
			head.ulObjId = 0;
			head.byPageId = 0;
			head.byTabId = 0;
			head.byType = (char)DATA_TRANS_SET_BACKCOLOR;
			int nCommand = head.byType;
			char* pData = new char[iSize];
			memset(pData,0,iSize);
			memcpy(pData,(char*)&head,sizeof(WB_COMMAND_HEAD));
			memcpy(pData + sizeof(WB_COMMAND_HEAD),(char*)&nCommand,sizeof(int));
			memcpy(pData + sizeof(WB_COMMAND_HEAD) + 4,(char*)&iColor,sizeof(int));
			SendWbData(channelId,pData,iSize);
			delete []pData;
		}
	}
	
}
void WbDTS::processBKColor(unsigned long channelId,char* data,int len)
{
	char* pData = data + sizeof(WB_COMMAND_HEAD) + 4;
	int iLen = len - sizeof(WB_COMMAND_HEAD) + 4;
	int iColor = *((int*)pData);

	{
		XAutoLock l(m_csMapWBObjAttribute);
		MapWBAttributes::iterator it;
		unsigned long dwObjID = 0;
		it = m_mapWBObjAttribute.find(dwObjID);
		if (it != m_mapWBObjAttribute.end())
		{
			it->second.Set(101,iColor);
		}
		else
		{
			XMemIni objAttribute;
			objAttribute.Set(101,iColor);
			m_mapWBObjAttribute[dwObjID] = objAttribute;	
		}
	}
 	deliverGroupWbData(channelId,data, len);
}
//end
//added by luoning 2010-7-13
void WbDTS::processResizeObj(unsigned long channelId,char* data,int len)
{
	char* pData = data + sizeof(WB_COMMAND_HEAD) + 4;
	int iLen = len - sizeof(WB_COMMAND_HEAD) + 4;
 	unsigned long dwObjID = *((unsigned long *)pData);
	RECT rc;
	memcpy(&rc,pData+sizeof(unsigned long),sizeof(RECT));
	MapWBObjects::iterator it;
	it = m_MapWBObjects.find(dwObjID);
	if (it != m_MapWBObjects.end())
	{
		OBJDATABUF odb = it->second;
		char *pTmpData = odb.pData + sizeof(WB_COMMAND_HEAD) +  4 +    8 +   sizeof(unsigned long)/* + sizeof(COLORREF) + sizeof(int)*/;

		PRECT lpRect = (PRECT)pTmpData;						
		lpRect->left   = rc.left;
		lpRect->right  = rc.right;
		lpRect->top    = rc.top;
		lpRect->bottom = rc.bottom;
		deliverGroupWbData(channelId,data, len);
	}
	//
}
//end
//---------------------------------------------------------------------------------------
void WbDTS::processMoveObj(unsigned long channelId,char* data,int len)
{
	char* pData = data + sizeof(WB_COMMAND_HEAD) + 4;
	int nLen = len - sizeof(WB_COMMAND_HEAD) - 4;

	unsigned long	dwObjID = 0;
	POINT	pt;
	memcpy(&pt, pData, 8);

	{
		XAutoLock l(m_csMapWBObjects);
		for (int i=0; i<((nLen-8)/4); i++)
		{
			dwObjID = *((unsigned long*)(pData+8+i*4));
			// 找符合条件的元素
			MapWBObjects::iterator it;		
			for(it = m_MapWBObjects.begin(); it != m_MapWBObjects.end(); it ++) {
				OBJDATABUF odb = (*it).second;
				PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)odb.pData;
				if (pHead->ulObjId == dwObjID) {				//如果找到，按 pt 进行移动
					int		nObjType = 0;
					unsigned long	dwSize = 0;
					memcpy(&nObjType, odb.pData + sizeof(WB_COMMAND_HEAD) + 4, sizeof(int));
					memcpy(&dwSize, odb.pData + sizeof(WB_COMMAND_HEAD) + 8, sizeof(int));

					switch (nObjType)
					{
					case SHAPE_COLOR_PEN:
					case SHAPE_PEN:
						{
							//移动参考 CPenObj;
							//						+ 	  WB_COMMAND_HEAD	  +PacketLen+type&size + objID    +      colorref     +  linewidth
							char *pTmpData = odb.pData + sizeof(WB_COMMAND_HEAD) +  4 +    8 +   sizeof(unsigned long) + sizeof(COLORREF) + sizeof(int);

							// PEN 里，此 RECT 无控制作用, 每个点必须移动
							PRECT lpRect = (PRECT)pTmpData;						
							lpRect->left	+= pt.x;
							lpRect->right	+= pt.x;
							lpRect->top		+= pt.y;
							lpRect->bottom	+= pt.y;

							// 移动画笔的每个点
							int nCount = 0;
							pTmpData += sizeof(RECT);
							memcpy(&nCount, pTmpData, sizeof(int));

							pTmpData += sizeof(int);
							for (int i=0; i<nCount; i++)
							{
								PPOINT lpPoint = (PPOINT)pTmpData;
								lpPoint->x += pt.x;
								lpPoint->y += pt.y;
								pTmpData += sizeof(POINT);
							}
						}
						break;

					case SHAPE_LINE:
					case SHAPE_EMPTY_RECT:			
					case SHAPE_FILL_RECT:
					case SHAPE_EMPTY_ELLIPSE:
					case SHAPE_FILL_ELLIPSE:
						{
							//移动参考 CLineObj;		// 数据结构与 PEN 相同
							char *pTmpData = odb.pData + sizeof(WB_COMMAND_HEAD) +  4 +    8 +   sizeof(unsigned long) + sizeof(COLORREF) + sizeof(int);

							PRECT lpRect = (PRECT)pTmpData;						
							lpRect->left	+= pt.x;
							lpRect->right	+= pt.x;
							lpRect->top		+= pt.y;
							lpRect->bottom	+= pt.y;
						}
						break;

					case SHAPE_TEXT:
						{
							//移动参考 CTextObj;		// no linewidth					
							char *pTmpData = odb.pData + sizeof(WB_COMMAND_HEAD) +  4 +    8 +   sizeof(unsigned long) + sizeof(COLORREF) /*+ sizeof(int)*/;

							PRECT lpRect = (PRECT)pTmpData;						
							lpRect->left	+= pt.x;
							lpRect->right	+= pt.x;
							lpRect->top		+= pt.y;
							lpRect->bottom	+= pt.y;
						}
						break;

					case SHAPE_BITMAP:
						{
							//移动参考 CBitmapObj;		// no colorref and linewidth
							char *pTmpData = odb.pData + sizeof(WB_COMMAND_HEAD) +  4 +    8 +   sizeof(unsigned long)/* + sizeof(COLORREF) + sizeof(int)*/;

							PRECT lpRect = (PRECT)pTmpData;						
							lpRect->left	+= pt.x;
							lpRect->right	+= pt.x;
							lpRect->top		+= pt.y;
							lpRect->bottom	+= pt.y;
						}
						break;
					}
				}
			}
		}
	}

	deliverGroupWbData(channelId,data, len);
}

//---------------------------------------------------------------------------------------
void WbDTS::processTransLastPosition(unsigned long channelId,char* data,int len)
{
	// 为了跟原来数据结构统一，数据包(去掉 WB_COMMAND_HEAD 外，前四个字节(unsigned long型)为数据包长度)
	WB_COMMAND_HEAD wbCmdHead;
	memset(&wbCmdHead, 0, sizeof(wbCmdHead));
	wbCmdHead.byType= DATA_TRANS_LASTPOSITION;
	unsigned long dwPacketLen = sizeof(m_cwPos);
	int nSendLen = sizeof(wbCmdHead) + sizeof(unsigned long) + sizeof(m_cwPos);
	char *pSendData = new char[nSendLen];
	memcpy(pSendData,										&wbCmdHead,		sizeof(wbCmdHead));
	memcpy(pSendData + sizeof(wbCmdHead),					&dwPacketLen,	sizeof(dwPacketLen));
	memcpy(pSendData + sizeof(wbCmdHead) + sizeof(unsigned long) ,	&m_cwPos,		sizeof(m_cwPos));
	SendWbData(channelId,pSendData,nSendLen);
	delete []pSendData;
	pSendData = NULL;
}

//---------------------------------------------------------------------------------------
void WbDTS::processTabCtrlChange(unsigned long channelId,char* data,int len)
{
	PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)data;
	m_cwPos.byTabId = pHead->byTabId;
	deliverGroupWbData(channelId,data, len);
}

//---------------------------------------------------------------------------------------
void WbDTS::processFullScreen(unsigned long channelId,char* data,int len)
{
	memcpy(&m_cwPos.bFullScreen, data + sizeof(WB_COMMAND_HEAD) + 4, 1);
	deliverGroupWbData(channelId,data, len);
}

//---------------------------------------------------------------------------------------
void WbDTS::processToolBarMove(unsigned long channelId,char* data,int len)
{
	memcpy(&m_cwPos.rtToolBar, data + sizeof(WB_COMMAND_HEAD) + 4, sizeof(m_cwPos.rtToolBar));	
	deliverGroupWbData(channelId,data, len);
}

//---------------------------------------------------------------------------------------
void WbDTS::processTransAllObj(unsigned long channelId,char*pData,int nLen)
{
	WB_COMMAND_HEAD wbSendCmdHead;
	for(int i = 0; i < nLen; i += sizeof(int)) 
	{	
		unsigned long dwObjID = *(unsigned long*)(pData + sizeof(WB_COMMAND_HEAD) + i);
		if (!FindObj(dwObjID))
		{	
			memset(&wbSendCmdHead, 0, sizeof(wbSendCmdHead));
			wbSendCmdHead.ulObjId = dwObjID;
			wbSendCmdHead.byType = DATA_TRANS_OBJREQUEST;
			SendWbData(channelId,(char*)&wbSendCmdHead,sizeof(WB_COMMAND_HEAD));
		}
	}
	memset(&wbSendCmdHead, 0, sizeof(wbSendCmdHead));
	wbSendCmdHead.byType = DATA_TRANS_REQUESTLASTPOSITION;
	SendWbData(channelId,(char*)&wbSendCmdHead,sizeof(WB_COMMAND_HEAD));
}

//---------------------------------------------------------------------------------------
bool WbDTS::FindObj(unsigned long ulObjID)
{
	XAutoLock l(m_csMapWBObjects);
	MapWBObjects::iterator it=m_MapWBObjects.find(ulObjID);
	return (it!=m_MapWBObjects.end());
}

//---------------------------------------------------------------------------------------
void WbDTS::processTransLastPositionRequest(unsigned long channelId,char* data,int len)
{
	// 为了跟原来数据结构统一，数据包(去掉 WB_COMMAND_HEAD 外，前四个字节(unsigned long型)为数据包长度)
	WB_COMMAND_HEAD wbCmdHead;
	memset(&wbCmdHead, 0, sizeof(wbCmdHead));
	wbCmdHead.byType= DATA_TRANS_RESPONSELASTPOSITION;
	unsigned long dwPacketLen = sizeof(m_cwPos);
	int nSendLen = sizeof(wbCmdHead) + sizeof(unsigned long) + sizeof(m_cwPos);
	char *pSendData = new char[nSendLen];
	memcpy(pSendData,										&wbCmdHead,		sizeof(wbCmdHead));
	memcpy(pSendData + sizeof(wbCmdHead),					&dwPacketLen,	sizeof(dwPacketLen));
	memcpy(pSendData + sizeof(wbCmdHead) + sizeof(unsigned long) ,	&m_cwPos,		sizeof(m_cwPos));
	SendWbData(channelId,pSendData,nSendLen);
	delete []pSendData;
	pSendData = NULL;
}

//---------------------------------------------------------------------------------------
void WbDTS::processTransLastPositionResponse(unsigned long channelId,char* data,int len)
{
	memcpy(&m_cwPos, data + sizeof(WB_COMMAND_HEAD) + 4, sizeof(m_cwPos));	
	deliverGroupWbData(channelId,data, len);
}


//---------------------------------------------------------------------------------------
void WbDTS::processTransDTObjHead(unsigned long channelId,char* data,int len)
{
	PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)data;
	m_csMapWBObjects.Lock();
	OBJDATABUF ObjBuf;
	memset(&ObjBuf, 0, sizeof(ObjBuf));
	ObjBuf.pData = new char[len];
	ObjBuf.nLen = len;
	//TRACE("OBJBUF SIZE : %d\n",ulSize);
	memcpy(ObjBuf.pData, data, len);

	m_MapWBObjects[pHead->ulObjId]=ObjBuf;
	m_csMapWBObjects.UnLock();

	if (channelId == 1)
	{
		deliverGroupWbData(channelId, data, len);
	}
	else
	{
		int nSendDataLen = sizeof(WB_COMMAND_HEAD) + sizeof(unsigned long);
		char*	pSendData = new char[nSendDataLen];
		pHead->byType = TRANS_REQUESTDTOBJDATA;
		memcpy((char *)pSendData, (char *)pHead, sizeof(WB_COMMAND_HEAD));
		unsigned long dwOffset = 0;
		memcpy(pSendData + sizeof(WB_COMMAND_HEAD), &dwOffset, sizeof(unsigned long));
		SendWbData(channelId,pSendData, nSendDataLen);
		delete[] pSendData;
		pSendData = NULL;
	}
	
}

//---------------------------------------------------------------------------------------
void WbDTS::processTransDTObjResponse(unsigned long channelId,char* data,int len)
{
	int nSendDataLen = 0;
	char*	pSendData = NULL; 
	PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)data;
	unsigned long	dwOffset = *((unsigned long*)(data + sizeof(WB_COMMAND_HEAD)));
	int nDataLen =len - sizeof(WB_COMMAND_HEAD) - sizeof(unsigned long);
	OBJDATABUF objBuf;
	bool bRet = false;
	{
		XAutoLock l(m_csMapWBObjects);
		MapWBObjects::iterator it = m_MapWBObjects.find(pHead->ulObjId);
		if (it != m_MapWBObjects.end())
		{
			bRet = true;
			objBuf = (*it).second;
		}
	}
	
	if(bRet && objBuf.pData)
	{
		unsigned long ulObjSize = *((unsigned long*)(objBuf.pData + objBuf.nLen - sizeof(unsigned long)));
		if(nDataLen > 0)
		{
			char* pData = new char[nDataLen];
			TRACE("dwOffset = %d nDataLen = %d ulObjSize = %d\n",dwOffset,nDataLen,ulObjSize);
			memcpy(pData, data + sizeof(WB_COMMAND_HEAD) + sizeof(unsigned long),nDataLen);
			SaveDataToFile(pData,nDataLen,dwOffset,pHead->ulObjId);
			delete[] pData;
			pData = NULL;
			dwOffset = dwOffset + nDataLen;

			if(dwOffset < ulObjSize)
			{
				nSendDataLen = sizeof(WB_COMMAND_HEAD) + sizeof(unsigned long);
				pSendData = new char[nSendDataLen]; 
				pHead->byType = TRANS_REQUESTDTOBJDATA;
				memcpy((char *)pSendData, (char *)pHead, sizeof(WB_COMMAND_HEAD));
				memcpy(pSendData + sizeof(WB_COMMAND_HEAD), &dwOffset, sizeof(unsigned long));
				SendWbData(channelId,pSendData, nSendDataLen);
			}
			else if(dwOffset == ulObjSize)
			{
				unsigned long	dwSize = 0;
				char pPath[MAX_PATH];
				GetBigFileName(pHead->ulObjId,pPath);

				FILE*	fp = fopen(pPath, "rb");

				if	(fp)
				{
					fseek(fp, 0, SEEK_END);
					dwSize = ftell(fp);	
					char* pBuffer = new char[objBuf.nLen+dwSize];
					memcpy(pBuffer, objBuf.pData, objBuf.nLen);
					fseek(fp,0,SEEK_SET);
					fread(pBuffer+objBuf.nLen, 1, dwSize, fp);
					fclose(fp);
					PWB_COMMAND_HEAD pSendHead = (PWB_COMMAND_HEAD)pBuffer;
					pSendHead->byType = DATA_TRANS_OBJRESPONSE;
					SendWbData(1,pBuffer, objBuf.nLen+dwSize);
					delete []pBuffer;	
					pBuffer = NULL;
				}
				deliverGroupWbData(channelId,objBuf.pData, objBuf.nLen);
			}
		}
		if (pSendData)
		{
			delete[] pSendData;
			pSendData = NULL;
		}

	}
}

//---------------------------------------------------------------------------------------
void WbDTS::processTransDTObjRequest(unsigned long channelId,char* data,int len)
{
	if (channelId == 1)
	{
		return;
	}

	PWB_COMMAND_HEAD pHead = (PWB_COMMAND_HEAD)data;
	unsigned long	dwOffset = *((unsigned long*)(data + sizeof(WB_COMMAND_HEAD)));

	unsigned long	dwSize = 0;
	char pPath[MAX_PATH];
	GetBigFileName(pHead->ulObjId, pPath);

	FILE*	fp = fopen(pPath, "rb");
	int nSendDataLen = 0 ;
	char*	pSendData = NULL;

	if	(fp)
	{
		fseek(fp, 0, SEEK_END);
		dwSize = ftell(fp);	

		if(dwOffset < dwSize)
		{
			int nDataLen = ((dwSize-dwOffset)>FILE_TRANS_MAXLENGTH?FILE_TRANS_MAXLENGTH:(dwSize-dwOffset));
			char* pBuffer = new char[nDataLen];
			fseek(fp,dwOffset,SEEK_SET);
			fread(pBuffer, 1, nDataLen, fp);
			nSendDataLen = nDataLen + sizeof(WB_COMMAND_HEAD) + sizeof(unsigned long);
			pSendData = new char[nSendDataLen];
			pHead->byType = TRANS_RESPONSEDTOBJDATA;

			memcpy((char *)pSendData, (char *)pHead, sizeof(WB_COMMAND_HEAD));
			memcpy(pSendData + sizeof(WB_COMMAND_HEAD), &dwOffset, sizeof(unsigned long));
			memcpy(pSendData + sizeof(WB_COMMAND_HEAD) + sizeof(unsigned long), pBuffer, nDataLen);
			delete[] pBuffer;
			pBuffer = NULL;		
		}
		fclose(fp);
		SendWbData(channelId,pSendData, nSendDataLen);
		delete[] pSendData;
		pSendData = NULL;
	}

}

//---------------------------------------------------------------------------------------
void WbDTS::SaveDataToFile(char* data, int len, unsigned long dwOffset,unsigned long ulObjId)
{
	char csFileName[MAX_PATH] ; 
	GetBigFileName(ulObjId, csFileName);

	
	FILE*	fp = NULL;
	if(dwOffset == 0)
		fp = fopen(csFileName, "wb");
	else
		fp = fopen(csFileName, "r+b");

	if	(fp)
	{
		fseek(fp, dwOffset, SEEK_SET);
		fwrite(data, 1, len, fp);	
		fclose(fp);
	}	
}

//---------------------------------------------------------------------------------------
void WbDTS::GetBigFileName(unsigned long ulObjId,char* pFileName)
{
	char sFileID[256]="";	

#ifdef WIN32
	sprintf(sFileID,"\\%u",ulObjId );
#else
	sprintf(sFileID,"/%u",ulObjId );
#endif

	strcpy(pFileName,GetFileTemPath());
	strcat(pFileName,sFileID);
	strcat(pFileName,".dts");
}
