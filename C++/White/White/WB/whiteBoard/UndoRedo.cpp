#pragma once
#include "stdafx.h"
#include "UndoRedo.h"
CWBDataModel::CWBDataModel():
m_iNumOfStatus(10)
	,m_iIndexOfStatus(0)
	,m_iSlidePos(0)
	,m_iSlideNum(0)
{
	Init();
}

CWBDataModel::~CWBDataModel( void )
{
	CleanUp();
}

void CWBDataModel::Init()
{
	m_vectorWbStatus.clear();
	m_vectorEventID.clear();   //会议事件
	/*	m_iNumOfStatus = 1;*/
	for (unsigned int i = 0;i<m_iNumOfStatus;i++)
	{
		XMemIni shareXml;
		MapWBAttributes mapData;
		shareXml.Set(KEY_OBJ_ID,SHARE_OBJID);
		/****************************************************/
		//主要为鹰眼撤销操作服务
		shareXml.Set(KEY_EVENT_ID, 0);
		shareXml.Set(KEY_OBJ_PAGE, 1);
		shareXml.Set(KEY_PAGEINDEX, 1);
		shareXml.Set(KEY_MOVE_EAGEL_EYE, _E_MOVE_EAGEL_EYE);
		shareXml.Set(KEY_PAGE_SCROLL_X, 0);
		shareXml.Set(KEY_PAGE_SCROLL_Y, 0);
		/****************************************************/
		mapData[SHARE_OBJID] = shareXml;
		m_vectorWbStatus.push_back(mapData);   //先把共享ID放进白板状态容器的末尾中
		m_vectorEventID.push_back(0);
	}
}

void CWBDataModel::CleanUp()
{
	m_vectorWbStatus.clear();
	m_vectorEventID.clear();
}

void CWBDataModel::SetWbStatusNum( unsigned __int32 iStatusNum )
{
	if (iStatusNum > MAX_UNDO_REDO_NUM)//
	{
		iStatusNum = MAX_UNDO_REDO_NUM;
	}
	m_iNumOfStatus = iStatusNum;
}

void CWBDataModel::ProcessDTSWbCmdData( eWbCmdOperation eCmdType,char* pData,int iLen )
{
	PROC_DTS_DATA(CMD_DEL_PAGE_ALL_ATTRIBUTE, OnDelOnePageAllAttribute);
	PROC_DTS_DATA(CMD_ADD_ATTRIBUTE,OnAddAttribute);//添加属性 
	PROC_DTS_DATA(CMD_CHANGE_ATTRIBUTE,OnChangeAttribute);   //改变属性
	PROC_DTS_DATA(CMD_CLEANUP_ATTRIBUTE,OnCleanUpAttribute);//清空全部属性
	PROC_DTS_DATA(CMD_DEL_ATTRIBUTE,OnDelAttribute); //删除属性
	PROC_DTS_DATA(CMD_REDO_STATUS,OnRedo); //前进一步
	PROC_DTS_DATA(CMD_UNDO_STATUS,OnUndo); //后退一步
	PROC_DTS_DATA(CMD_CLEAR_UNDO_QUEUE,OnClearUndo);//清空撤销队列
}

void CWBDataModel::OnDelOnePageAllAttribute(char* pData, int iLen)
{
	int pageIndex = *(int*)(pData+sizeof(int));
	unsigned long ulEventId = *(unsigned long*)(pData+2*sizeof(int));
	if (NeedToSlide(ulEventId))
	{
		SlideNextStatus();
		MapWBAttributes* pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pMapWbAttribute)
		{
			std::vector<unsigned long> vectorWBobjs;
			vectorWBobjs.clear();
			IT_MapWBAttributes it = pMapWbAttribute->begin();
			while (it != pMapWbAttribute->end())
			{
				int nIsLock = 0;
				BOOL bLock = false;
				if (it->second.Get(KEY_OBJ_LOCK,nIsLock))
				{
					bLock = (BOOL)nIsLock;
				}
				int nIsBK = 0;
				BOOL bIsKG = false;
				if (it->second.Get(KEY_OBJ_BKG, nIsBK))
				{
					bIsKG = (BOOL)nIsBK;
				}
				if (bLock || bIsKG)
				{
					it++;
					continue;
				}
				int nObjPageId = 0;
				it->second.Get(KEY_OBJ_PAGE,nObjPageId);
				IT_MapWBAttributes itTemp;
				if (nObjPageId == pageIndex)
				{
					if (it->first != SHARE_OBJID )
					{
						vectorWBobjs.push_back(it->first);
					}
				}	
				it++;
			}

			for (int i = 0;i < vectorWBobjs.size();i++)
			{
				unsigned long ulObjID = vectorWBobjs[i];
				IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				if (it != pMapWbAttribute->end())
				{
					pMapWbAttribute->erase(it);
				}
			}
		}
	}
}


void CWBDataModel::OnAddAttribute( char* pData,int iLen )
{
	XMemIni xmlPacket;
	if (ToMemIni(xmlPacket,pData + sizeof(int),iLen - sizeof(int)))
	{
		unsigned long ulEventID = 0;
		if(xmlPacket.Get(KEY_EVENT_ID,ulEventID) && NeedToSlide(ulEventID))
		{
			SlideNextStatus();
		}
		MapWBAttributes* pMapWBAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pMapWBAttribute)
		{
			unsigned long ulObjID = 0;
			if (xmlPacket.Get(KEY_OBJ_ID,ulObjID))
			{
				pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID,xmlPacket));
			}
		}
	}
}

void CWBDataModel::OnChangeAttribute( char* pData,int iLen )
{
	int iCmdType = *((int*)pData);
	char* pBuff = pData + sizeof(int);
	string strValue = "";
	if (pBuff)
	{
		strValue = pBuff;
		XMemIni xmlPacket;
		xmlPacket.SetString(strValue);
		unsigned long ulObjID = 0;
		if (xmlPacket.Get(KEY_OBJ_ID,ulObjID))
		{
			MapWBAttributes* pMapWbAttribute = NULL;
			CHECK_EXIST;
			unsigned long ulEventID = 0;
			int moveeagel = 0;
			xmlPacket.Get(KEY_MOVE_EAGEL_EYE,moveeagel);
			if (ulObjID == SHARE_OBJID && moveeagel != _E_MOVE_EAGEL_EYE)
			{
				// 					for (int i = 0;i < m_iNumOfStatus;i++)
				// 					{
				// 						pMapWbAttribute = &m_vectorWbStatus[i];
				// 						IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				// 						if (it != pMapWbAttribute->end())
				// 						{
				// 							it->second.SetString(strValue);
				// 						}
				// 					}
				// 					pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
				// 					IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				// 					if (it != pMapWbAttribute->end())
				// 					{
				// 						it->second.ResetString(strValue);
				// 					}
			}
			else
			{
				if (xmlPacket.Get(KEY_EVENT_ID,ulEventID) && NeedToSlide(ulEventID))
				{
					SlideNextStatus();
				}

			}
			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			if (pMapWbAttribute)
			{
				IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				if (it != pMapWbAttribute->end())
				{
					it->second.SetString(strValue);
				}
			}	
		}
	}
}

void CWBDataModel::OnCleanUpAttribute( char* pData,int iLen )
{
	char* pBuff = pData + sizeof(int);
	unsigned long ulObjID = 0;
	if (pBuff)
	{
		ulObjID = *((unsigned long*)pBuff);
		MapWBAttributes* pMapWbAttribute = NULL;
		pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pMapWbAttribute)
		{
			IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
			if (it != pMapWbAttribute->end())
			{
				string strPath = "";
				it->second.Get(KEY_FILE_PATH,strPath);
				if (strPath != "")
				{
					remove(strPath.c_str());
				}
				pMapWbAttribute->erase(it);
			}
		}
	}

}

void CWBDataModel::OnDelAttribute( char* pData,int iLen )
{
	int iCmdType = *((int*)pData);
	char* pBuff = pData + sizeof(int);
	unsigned long ulObjID = *((unsigned long*)pBuff);
	pBuff += sizeof(unsigned long);
	unsigned long ulEventID = *((unsigned long*)pBuff);
	if (iCmdType == CMD_DEL_ATTRIBUTE)
	{
		CHECK_EXIST;
		if (NeedToSlide(ulEventID))
		{
			SlideNextStatus();
		}
		MapWBAttributes* pMapWbAttribute = NULL;
		pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pMapWbAttribute)
		{
			IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
			if (it != pMapWbAttribute->end())
			{
				pMapWbAttribute->erase(it);
			}
		}
	}
}

void CWBDataModel::OnRedo( char* pData,int iLen )
{
	if (m_iSlidePos == 0 )
	{
		return;
	}
	m_iSlidePos++;
	int iNextStatus = m_iIndexOfStatus + 1;
	if (iNextStatus >=m_iNumOfStatus)
	{
		iNextStatus = 0;
	}
	StatusToStatus(m_iIndexOfStatus,iNextStatus);
	m_iIndexOfStatus = iNextStatus;
}

void CWBDataModel::OnUndo( char* pData,int iLen )
{
	if (m_iSlidePos == (m_iSlideNum*(-1)))
	{
		return;
	}
	m_iSlidePos--;
	int iNextStatus = m_iIndexOfStatus - 1;
	if (iNextStatus < 0)
	{
		iNextStatus = m_iNumOfStatus -1;
	}
	StatusToStatus(m_iIndexOfStatus,iNextStatus);
	m_iIndexOfStatus = iNextStatus;
}

// void CWBDataModel::OnLayer(char* pData,int iLen)
// {
// 	XMemIni xmlPacket;
// 	if (ToMemIni(xmlPacket,pData + sizeof(int),iLen - sizeof(int)))
// 	{
// 		unsigned long ulObjID = 0;
// 		unsigned long ulEventID = 0;
// 		int  nLayerType	 =0;
// 		xmlPacket.Get(KEY_EVENT_ID,ulEventID);
// 
// 		if (NeedToSlide(ulEventID))
// 		{
// 			SlideNextStatus();
// 		}
// 		MapWBAttributes* pMapWBAttribute = &m_vectorWbStatus[m_iIndexOfStatus];	
// 		if (pMapWBAttribute)
// 		{
// 			if (xmlPacket.Get(KEY_OBJ_ID, ulObjID) && xmlPacket.Get(KEY_LAYER_CHANGETYPE,nLayerType))
// 			{
// 				VectorWBAttributes vectorWbAttributes;
// 				vectorWbAttributes.clear(); 
// 				XMemIni xmlIndex;
// 				IT_MapWBAttributes it = pMapWBAttribute->begin();
// 				int nLayerSite = 0; 
// 				int i =0;
// 				while(it != pMapWBAttribute->end())
// 				{
// 					unsigned long ulObjtempID = 0;
// 					it->second.Get(KEY_OBJ_ID,ulObjtempID);
// 					if (ulObjtempID == ulObjID)
// 					{
// 						nLayerSite = i;
// 						string strValue = "";
// 						xmlPacket.GetString(strValue);
// 						if (strValue != "")
// 						{
// 							xmlIndex.ResetString(strValue);
// 							//							xmlIndex.Set(KEY_LAYER_CHANGETYPE,nLayerType);
// 							xmlIndex.Set(KEY_EVENT_ID,ulEventID);
// 							vectorWbAttributes.push_back(xmlIndex);
// 						}
// 					}
// 					else
// 					{
// 						XMemIni xmltempPacket;
// 						string strValue = "";
// 						it->second.GetString(strValue);
// 						if (strValue != "")
// 						{
// 							xmltempPacket.ResetString(strValue);
// 							vectorWbAttributes.push_back(xmltempPacket);
// 						}
// 					}
// 					it++;
// 					i++;
// 				}
// 				pMapWBAttribute->clear();
// 				switch (nLayerType)
// 				{
// 				case WB_LAYER_UNDER :	 //up
// 					{
// 						int n = 0;
// 						for (int i = 0;i < vectorWbAttributes.size();i++)
// 						{
// 							if (i == (nLayerSite -1))
// 							{
// 								n=nLayerSite;
// 							}
// 							else
// 								if(i == nLayerSite)
// 								{
// 									n= nLayerSite-1;
// 								}
// 								else { n= i; }
// 								unsigned long ulObjID = 0;
// 								if (n >= vectorWbAttributes.size())
// 								{
// 									continue;
// 								}
// 								if(vectorWbAttributes[n].Get(KEY_OBJ_ID,ulObjID))
// 								{
// 									pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID,vectorWbAttributes[n]));
// 								}
// 
// 						}
// 					}
// 
// 					break;
// 				case WB_LAYER_SUPER:	   //down
// 					{
// 						int n = 0;
// 						for (int i = 0;i < vectorWbAttributes.size();i++)
// 						{
// 							if (i == (nLayerSite +1))
// 							{
// 								n=nLayerSite;
// 							}
// 							else
// 								if(i == nLayerSite)
// 								{
// 									n= nLayerSite+1;
// 								}
// 								else
// 								{ 
// 									n= i;
// 								}
// 								unsigned long ulObjID = 0;
// 								if (n >= vectorWbAttributes.size())
// 								{
// 									continue;
// 								}
// 								if(vectorWbAttributes[n].Get(KEY_OBJ_ID,ulObjID))
// 								{
// 									pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID,vectorWbAttributes[n]));
// 								}
// 						}
// 					}
// 
// 					break;
// 				case WB_LAYER_TOP:
// 					{
// 						int n = 0;
// 						for (int i = 0;i < vectorWbAttributes.size();i++)
// 						{
// 							if (i == nLayerSite)
// 							{
// 								continue;
// 							}
// 							n= i; 
// 							unsigned long ulObjID = 0;
// 							if(vectorWbAttributes[n].Get(KEY_OBJ_ID,ulObjID))
// 							{
// 								pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID,vectorWbAttributes[n]));
// 							}
// 						}
// 						if (nLayerSite >= vectorWbAttributes.size())
// 						{
// 							break;
// 						}
// 						unsigned long ulObjID = 0;
// 						if (vectorWbAttributes[nLayerSite].Get(KEY_OBJ_ID,ulObjID))
// 						{
// 							pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID,vectorWbAttributes[nLayerSite]));
// 						}
// 
// 					}		
// 					break;
// 				case WB_LAYER_BOTTOM:
// 					{
// 						int n = 0;
// 						unsigned long ulObjID = 0;
// 						vectorWbAttributes[nLayerSite].Get(KEY_OBJ_ID,ulObjID);
// 						pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID,vectorWbAttributes[nLayerSite]));
// 						for (int i = 0;i < vectorWbAttributes.size();i++)
// 						{
// 							if (i == nLayerSite)
// 							{
// 								continue;
// 							}
// 							n= i; 
// 							unsigned long ulObjID = 0;
// 							if(vectorWbAttributes[n].Get(KEY_OBJ_ID,ulObjID))
// 							{
// 								pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID,vectorWbAttributes[n]));
// 							}
// 						}
// 					}	
// 					break;
// 				default:
// 					break;
// 				}  //switch
// 			}
// 		}
// 	}
// }

void CWBDataModel::StatusToStatus( int iFrom,int iTo )
{
	MapWBAttributes* pMapWbAttributeFrom = &m_vectorWbStatus[iFrom];
	MapWBAttributes* pMapWbAttributeTo = &m_vectorWbStatus[iTo];
	if (pMapWbAttributeFrom && pMapWbAttributeTo)
	{
		IT_MapWBAttributes itTo = pMapWbAttributeTo->begin();
		IT_MapWBAttributes itFrom;
		unsigned long ulObjID = 0;

		int iCmdType = 0;
		string strValueTo = "";
		char* pBuff = NULL;
		int moveEagel = 0;
		itTo->second.Get(KEY_MOVE_EAGEL_EYE,moveEagel);
		while (itTo != pMapWbAttributeTo->end())
		{
			if ((itTo->second.Get(KEY_OBJ_ID,ulObjID) && ulObjID != SHARE_OBJID) || (moveEagel == _E_MOVE_EAGEL_EYE))
			{
				itFrom = pMapWbAttributeFrom->find(ulObjID);
				if (itFrom == pMapWbAttributeFrom->end())		   //没有找到 就表示上一次操作是删除对象 ，所有要做的事把这个对象还原 发送的是添加对象操作
				{
					iCmdType = CMD_ADD_ATTRIBUTE;
					itTo->second.GetString(strValueTo);
					int iLength = sizeof(int) + strValueTo.size();
					pBuff = new char[iLength+1];
					if (pBuff)
					{
						memset(pBuff,0,iLength+1);
						memcpy(pBuff,(char*)&iCmdType,sizeof(int));
						memcpy(pBuff + sizeof(int),strValueTo.c_str(),strValueTo.size());
						pBuff[iLength] = '\0';
						WB_ProcessDataIn(pBuff,iLength+1);
						delete pBuff;
						pBuff = NULL;
					}										  
				}
				else//说明存在
				{
					int layerstype = 0;
					unsigned long ulEventIDTo = 0;
					unsigned long ulEventIDFrom = 0;
					itTo->second.Get(KEY_EVENT_ID,ulEventIDTo);
					itFrom->second.Get(KEY_EVENT_ID,ulEventIDFrom);
// 					if ((itFrom->second.Get(KEY_LAYER_CHANGETYPE,layerstype))&&(ulEventIDFrom != ulEventIDTo))
// 					{
// 						iCmdType = CMD_CHANGE_LAYER;
// 						std::string strValueForm = "";
// 						itFrom->second.GetString(strValueForm);
// 						XMemIni xmlobj;
// 						xmlobj.ResetString(strValueForm);
// 						switch (layerstype)
// 						{
// 						case WB_LAYER_BOTTOM:
// 							xmlobj.Set(KEY_LAYER_CHANGETYPE,WB_LAYER_TOP);
// 							break;
// 						case WB_LAYER_TOP:
// 							xmlobj.Set(KEY_LAYER_CHANGETYPE,WB_LAYER_BOTTOM);
// 							break;
// 						case WB_LAYER_SUPER:
// 							xmlobj.Set(KEY_LAYER_CHANGETYPE,WB_LAYER_UNDER);
// 							break;
// 						case WB_LAYER_UNDER:
// 							xmlobj.Set(KEY_LAYER_CHANGETYPE,WB_LAYER_SUPER);
// 							break;
// 						}
// 						xmlobj.GetString(strValueForm);
// 						//						CheckXmlString(strValueForm);
// 						int iLength = sizeof(int) + strValueForm.size();
// 						pBuff = new char[iLength+1];
// 						if (pBuff)
// 						{
// 							memset(pBuff,0,iLength + 1);
// 							memcpy(pBuff,(char*)&iCmdType,sizeof(int));
// 							memcpy(pBuff + sizeof(int),strValueForm.c_str(),strValueForm.size());
// 							pBuff[iLength] = '\0';
// 							WB_ProcessDataIn(pBuff,iLength+1);
// 							delete pBuff;
// 							pBuff = NULL;
// 						}
// 					}// if  CMD_CHANGE_LAYER
// 					else
					{
						if (ulEventIDFrom != ulEventIDTo)
						{	
							iCmdType = CMD_CHANGE_ATTRIBUTE;
							itTo->second.GetString(strValueTo);
							int iLength = sizeof(int) + strValueTo.size();
							pBuff = new char[iLength+1];
							if (pBuff)
							{
								memset(pBuff,0,iLength + 1);
								memcpy(pBuff,(char*)&iCmdType,sizeof(int));
								memcpy(pBuff + sizeof(int),strValueTo.c_str(),strValueTo.size());
								pBuff[iLength] = '\0';
								WB_ProcessDataIn(pBuff,iLength+1);
								delete pBuff;
								pBuff = NULL;
							}		
						}
					}

				}
			}
			itTo++;
		}
		//
		itFrom = pMapWbAttributeFrom->begin();
		while (itFrom != pMapWbAttributeFrom->end())
		{
			itFrom->second.Get(KEY_OBJ_ID,ulObjID);
			unsigned long ulFromEventId  =0;			  //from中添加事件id
			if (ulObjID != SHARE_OBJID)
			{
				itTo = pMapWbAttributeTo->find(ulObjID);
				if (itTo ==  pMapWbAttributeTo->end())	 //表明上一次操作是添加操作
				{
					iCmdType = CMD_DEL_ATTRIBUTE;
					itFrom->second.Get(KEY_EVENT_ID,ulFromEventId);
					int iLength = sizeof(int) + sizeof(unsigned long) + sizeof(unsigned long);
					pBuff = new char[iLength +1];
					if (pBuff)
					{
						memset(pBuff,0,iLength);
						memcpy(pBuff,(char*)&iCmdType,sizeof(int));
						memcpy(pBuff + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
						memcpy(pBuff + sizeof(int) + sizeof(unsigned long),(char*)&ulFromEventId,sizeof(unsigned long));	 //是让客服端刷新用的
						pBuff[iLength] = '\0';
						WB_ProcessDataIn(pBuff,iLength+1);
						delete pBuff;
						pBuff = NULL;
					}
				}
			}
			itFrom++;
		}
		//层顺序调整
		//		ReSortLayer(iFrom,iTo);
	}
}

void CWBDataModel::ReSortLayer( int iStatusFrom,int iStatusTo )
{
	MapWBAttributes* pMapWbAttributeFrom = &m_vectorWbStatus[iStatusFrom];
	MapWBAttributes* pMapWbAttributeTo = &m_vectorWbStatus[iStatusTo];

	IT_MapWBAttributes itTo = pMapWbAttributeTo->begin();
	IT_MapWBAttributes itFrom = pMapWbAttributeFrom->begin();
	unsigned long ulObjIDTo = 0;
	unsigned long ulObjIDFrom = 0;
	int iCmdType = 0;
	char* pBuff = NULL;
	while (itTo != pMapWbAttributeTo->end() && itFrom != pMapWbAttributeFrom->end())
	{
		itTo->second.Get(KEY_OBJ_ID,ulObjIDTo);
		itFrom->second.Get(KEY_OBJ_ID,ulObjIDFrom);
		if (ulObjIDTo == ulObjIDFrom)
		{
			itTo++;
			itFrom++;

		}
		else
		{
			IT_MapWBAttributes it = pMapWbAttributeFrom->find(ulObjIDTo);
			if (it !=pMapWbAttributeFrom->end())
			{
				iCmdType = CMD_CHANGE_LAYER;

				int iLength = sizeof(int) + 2*sizeof(unsigned long);
				pBuff = new char[iLength];
				if (pBuff)
				{
					memset(pBuff,0,iLength);
					memcpy(pBuff,(char*)&iCmdType,sizeof(int));
					memcpy(pBuff + sizeof(int),(char*)&ulObjIDFrom,sizeof(unsigned long));
					memcpy(pBuff + sizeof(int) + sizeof(unsigned long),(char*)&ulObjIDTo,sizeof(unsigned long));
					//		SendToAll(pBuff,iLength);
					WB_ProcessDataIn(pBuff,iLength);
					delete pBuff;
					pBuff = NULL;
				}
				itTo++;
				itFrom++;
			}
			else
			{
				itTo++;
			}
		}
	}
}

bool CWBDataModel::NeedToNotifyCleanUpAttribute( unsigned long ulObjID )
{
	MapWBAttributes* pMapWbAttribute = NULL;
	for (int i = 0;i < m_iNumOfStatus;i++)
	{
		pMapWbAttribute = &m_vectorWbStatus[i];
		IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
		if (it != pMapWbAttribute->end())
		{
			return false;
		}
	}
	return true;
}

BOOL CWBDataModel::ToMemIni( XMemIni &xmlPacket,char* pData,int iLen )
{
	if (!pData || iLen <=0)
	{
		return false;
	}
	char* pBuff = new char[iLen+1];
	string strKey = "";
	if (pBuff)
	{
		memset(pBuff,0,iLen);
		memcpy(pBuff,pData,iLen);
		pBuff[iLen] = '\0';
		strKey = pBuff;
		xmlPacket.ResetString(strKey);
		delete pBuff;
		pBuff = NULL;
		return true;
	}
	return false;
}

bool CWBDataModel::NeedToSlide( unsigned long ulEventID )
{
	if (m_vectorEventID.size() > m_iIndexOfStatus)
	{
		if (m_vectorEventID[m_iIndexOfStatus] == ulEventID)
		{
			return false;
		}
		else
		{
			int iStatus = m_iIndexOfStatus + 1;
			if (iStatus >= m_iNumOfStatus)
			{
				iStatus = 0;
			}
			m_vectorEventID[iStatus] = ulEventID;
			return true;
		}
	}
	return false;
}

void CWBDataModel::SlideNextStatus()
{
	int iOldIndexOfStatus = m_iIndexOfStatus;
	m_iIndexOfStatus++;
	if (m_iIndexOfStatus>=m_iNumOfStatus)
	{
		m_iIndexOfStatus = 0;
	}
	if (iOldIndexOfStatus != -1)
	{
		prepareCoverStatus(m_iIndexOfStatus);
		m_vectorWbStatus[m_iIndexOfStatus] = m_vectorWbStatus[iOldIndexOfStatus];//函数的主要功能是这一句

		m_iSlideNum += m_iSlidePos;
		++m_iSlideNum;
		if (m_iSlideNum > m_iNumOfStatus-1)
		{
			m_iSlideNum = m_iNumOfStatus-1;
		}

		m_iSlidePos = 0;
	}
}

void CWBDataModel::prepareCoverStatus( int iIndexOfStatus )
{
	IT_MapWBAttributes it = m_vectorWbStatus[iIndexOfStatus].begin();//std::map<unsigned long,XMemIni> MapWBAttributes;关键字是OBJID
	while (it != m_vectorWbStatus[iIndexOfStatus].end())
	{
		unsigned long ulObjID = it->first;     
		if (!ObjExistInAnyOtherStatus(ulObjID,iIndexOfStatus))  //这里是用来判断该对象是否都存在其他状态中 如果不存在，则条件满足
		{
			XMemIni xmlPacket = it->second;
			unsigned long ulSize = 0;
			unsigned long ulPos = 0;//对象资源文件下载的位置
			string strPath = "";
			if (xmlPacket.Get(KEY_FILE_SIZE,ulSize) && xmlPacket.Get(KEY_FILE_POS,ulPos) && xmlPacket.Get(KEY_FILE_PATH,strPath))//根据eAttributeKey来判断xmlPacket是否为空	
			{
				// 				if (ulSize > ulPos)//表示没有完成
				// 				{
				// 					int iCmdType = CMD_STOP_STREAM;			//CMD_STOP_STREAM为eWbCmdOperation中的一个值 停止发送流
				// 					int iLen = sizeof(int) + sizeof(unsigned long);
				// 					char* pData = new char[iLen];
				// 					if (pData)
				// 					{
				// 						memset(pData,0,iLen);
				// 						memcpy(pData,(char*)&iCmdType,sizeof(int));
				// 						memcpy(pData + sizeof(int) ,(char*)&ulObjID,sizeof(unsigned long));
				// 						unsigned long ulSessionID = 0;
				// 						xmlPacket.Get(KEY_FILE_SENDER,ulSessionID);//KEY_FILE_SENDER对象资源上传者
				// 					//	SendToPoint(ulSessionID,pData,iLen);
				// 						delete pData;
				// 						pData = NULL; 
				// 					}
				// 				}
				//由于会删除本地添加文件。暂时不要
				//remove(strPath.c_str());//删除临时文件
			}
		}
		it++;
	}
}

BOOL CWBDataModel::ObjExistInAnyOtherStatus( unsigned long ulObjID,int iIndexStatus )
{
	for (int i = 0;i<m_iNumOfStatus;i++)
	{
		if (i != iIndexStatus)
		{
			if (m_vectorWbStatus[i].find(ulObjID) != m_vectorWbStatus[i].end())
			{
				return true; //找到了
			}
		}
	}
	return false;
}

BOOL CWBDataModel::ObjExist( unsigned long ulObjID )
{
	MapWBAttributes* pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
	if (pMapWbAttribute)
	{
		IT_MapWBAttributes it  = pMapWbAttribute->find(ulObjID);//find其返回值为指向该元素的迭代器 
		if (it != pMapWbAttribute->end())
		{
			return true;
		}
	}
	return false;
}

void CWBDataModel::WBMapToWBMap( MapWBAttributes& mapFrom,MapWBAttributes& mapTo )
{
	IT_MapWBAttributes it = mapFrom.begin();
	while (it != mapFrom.end())
	{
		mapTo.insert(MapWBAttributes::value_type(it->first,it->second));
		it++;
	}
}

void CWBDataModel::CheckXmlPacket( XMemIni & xmlPacket )
{
	xmlPacket.DeleteKey(KEY_FILE_PATH);
	xmlPacket.DeleteKey(KEY_FILE_SIZE);
	xmlPacket.DeleteKey(KEY_FILE_POS);
	xmlPacket.DeleteKey(KEY_FILE_TYPE);
}

void CWBDataModel::CheckXmlString( string &strValue )
{
	XMemIni xmlPacket;
	xmlPacket.ResetString(strValue);
	CheckXmlPacket(xmlPacket);
	xmlPacket.GetString(strValue);
}

void CWBDataModel::OnClearUndo( char* pData,int iLen )
{
	m_vectorWbStatus[0] = m_vectorWbStatus[m_iIndexOfStatus];

	unsigned long ulEventID = m_vectorEventID[m_iIndexOfStatus];
	m_vectorEventID.clear();
	m_vectorEventID.assign(10, 0);
	m_vectorEventID[0] = ulEventID;

	m_iIndexOfStatus = 0;
	m_iSlideNum = 0;
	m_iSlidePos = 0;
}

