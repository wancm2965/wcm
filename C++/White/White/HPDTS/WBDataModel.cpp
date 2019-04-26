#include "WBDataModel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CWBDataModel::CWBDataModel(IHPDTSNotify&rIHPDTSNotify,bool bUI):
					  m_bUI(bUI)
					 ,m_pIHPDTSNotify(&rIHPDTSNotify)
					 ,m_iNumOfStatus(10)
					 ,m_iIndexOfStatus(0)
					 ,m_iSlidePos(0)
					 ,m_iSlideNum(0)
					 ,m_bMCUExistObj(false)
					 ,m_bAreadyRequestAllObj(false)
{
	Init();
}

CWBDataModel::~CWBDataModel(void)
{
	CleanUp();
}

void CWBDataModel::Init()
{
	m_vectorWbStatus.clear();
	m_mapSessionInfo.clear();
	m_vectorEventID.clear();
	if (m_bUI)
	{
		m_iNumOfStatus = 1;
	}		
	for (int i = 0;i<m_iNumOfStatus;i++)
	{
		XMemIni shareXml;
		MapWBAttributes mapData;
		shareXml.Set(KEY_OBJ_ID,SHARE_OBJID);
		mapData[SHARE_OBJID] = shareXml;
		m_vectorWbStatus.push_back(mapData);
		m_vectorEventID.push_back(0);
	}
	m_mapUserColorInfo.clear();
#ifdef HUBEI_MEDICAL_SDK
	InitUserColor();
#endif

	//add by zhuyuyuan begin
	//服务器端测试
	/*fstream ft; 
	ft.open("test.txt", ios_base::out|ios_base::app);	
	if (ft.is_open())
	{
		ft.seekp(0,ios_base::end);
		ft.write("this is test by zhuyuyuan\n",strlen("this is test by zhuyuyuan\n")+1);
		ft.flush();
		ft.close();
	}*/
	//初始化日志操作对象

// 	char ConfigFile[50];
// 	memset(ConfigFile,0,50);
// #ifdef WIN32
// 	strncpy(ConfigFile,TEXT("HPDTS_config.ini"),strlen(TEXT("HPDTS_config.ini"))+1);
// #else
// 	strncpy(ConfigFile,TEXT("/opt/avcond/conf/HPDTS_config.ini"),strlen(TEXT("/opt/avcond/conf/HPDTS_config.ini"))+1);
// #endif
// 	//char ConfigFile[50]= TEXT("HPDTS_config.ini");
// 	std::string clogDirectory = LOG_DIRECTORY;
// 	int cmaxBuff = 20;
// 	bool cisOpen = true;
// 	try
// 	{
// 		printf("HPDTS: in CWBDataModel::Init, reading HPDTS_config.ini\n");
// 		Config configSettings(ConfigFile); 
// 		clogDirectory  = configSettings.Read(TEXT("logDirectory"),clogDirectory );
// 		cmaxBuff = configSettings.Read(TEXT("maxBuff"), cmaxBuff);
// 		cisOpen = configSettings.Read(TEXT("isOpen"),cisOpen);
// 	}
// 	catch(struct File_not_found fnf)
// 	{
// 		//TRACE(TEXT("HPDTS:LogWrite:WriteLog function, open log file failed！\n"));
// 		std::string errmsg = TEXT("HPDTS:WBDataModel:Init function, do not find file") + fnf.filename;
// 		TRACE(errmsg.c_str());
// 	}
// 	catch(struct Key_not_found knf)
// 	{
// 		std::string errmsg = TEXT("HPDTS:WBDataModel:Init function, do not find key") + knf.key;
// 		TRACE(errmsg.c_str());
// 	}
// 	m_logWrite.SetLogDirectory(clogDirectory);
// 	m_logWrite.SetMaxBuffCount(cmaxBuff);
// 	m_logWrite.SetOpenState(cisOpen);
}
void CWBDataModel::InitUserColor()
{
	XAutoLock l(m_csMapRGBInfo);
	m_mapCorrespondingRGB.clear();
	SetUserColor(Default_Color);
	SetUserColor(Red_Color);
	SetUserColor(Green_Color);
	SetUserColor(Blue_Color);
	SetUserColor(Yellow_Color);
	SetUserColor(Fuchsia_Color);
	SetUserColor(Aqua_Color);
	SetUserColor(MidnightBlue_Color);
	SetUserColor(SeaGreen_Color);
	SetUserColor(Teal_Color);
	SetUserColor(DarkSlateGray_Color);
	SetUserColor(DarkGreen_Color);
	SetUserColor(SaddleBrown_Color);
	SetUserColor(Maroon_Color);
	SetUserColor(BlueViolet_Color);
	SetUserColor(DarkOrchid_Color);
}

void CWBDataModel::WBMapToWBMap(MapWBAttributes& mapFrom,MapWBAttributes& mapTo)
{
	IT_MapWBAttributes it = mapFrom.begin();
	while (it != mapFrom.end())
	{
		mapTo.insert(MapWBAttributes::value_type(it->first,it->second));
		it++;
	}
}
void CWBDataModel::CleanUp()
{
	m_vectorWbStatus.clear();
	m_mapSessionInfo.clear();
	m_vectorEventID.clear();
	m_mapCorrespondingRGB.clear();
	m_mapSessionInfo.clear();
}
void CWBDataModel::SetWbStatusNum(unsigned int iStatusNum)
{
	if (iStatusNum > MAX_STATUS_NUM)//
	{
		iStatusNum = MAX_STATUS_NUM;
	}
	m_iNumOfStatus = iStatusNum;
}
//IWBDTS
void CWBDataModel::processWBUIData(AVDataInPacket& in)//--->net
{
	UINT nDataLen = 0;
	in >> nDataLen;	
	unsigned char* pData = NULL;
	pData = in.ReadData((int&)nDataLen);
	if (pData)
	{
		int iCmdType = -1;
		iCmdType = *((int*)pData);
		ProcessUIWbCmdData((eWbCmdOperation)iCmdType,(char*)pData,nDataLen);
	}
}
void CWBDataModel::processWBDTSData(AVDataInPacket& in,unsigned long ulSessionID)//<---net
{
	UINT nDataLen = 0;
	in >> nDataLen;	
	unsigned char* pData = NULL;
	pData = in.ReadData((int&)nDataLen);
	if (pData)
	{
		int iCmdType = -1;
		iCmdType = *((int*)pData);
		ProcessDTSWbCmdData((eWbCmdOperation)iCmdType,ulSessionID,(char*)pData,nDataLen);
	}
}
void CWBDataModel::AddWBDTSSession(unsigned long ulSessionID)
{
	{
		XAutoLock SessionLock(m_csMapSessionID);
		SESSION_INFO tSessionInfo;
		tSessionInfo.ulSessionID = ulSessionID;
		m_mapSessionInfo.insert(MapSessionInfo::value_type(ulSessionID,tSessionInfo));
	}
}

//**************************************huanglongquan add by 2012年5月15日********************************/
void CWBDataModel::AddWBDTSSession( unsigned long ulSessionID,bool bHost )
{
	{
		XAutoLock SessionLock(m_csMapSessionID);
		SESSION_INFO tSessionInfo;
		tSessionInfo.ulSessionID = ulSessionID;
		tSessionInfo.bHost = bHost;
		m_mapSessionInfo.insert(MapSessionInfo::value_type(ulSessionID,tSessionInfo));
		int nMapsize = m_mapSessionInfo.size();
		if (nMapsize >1 && !m_bAreadyRequestAllObj && !m_bMCUExistObj)
		{
			if (bHost)
			{
				int iCmdType = CMD_REQUEST_ALL_OBJ_ATTRIBUTE;
				int iLen = sizeof(int);
				char* pdata = new char[iLen+1];
				memset(pdata,0,iLen+1);
				memcpy(pdata,(char*)&iCmdType,sizeof(int));
				pdata[iLen] = '\0';
				SendToPoint(ulSessionID,pdata,iLen);
				delete []pdata;
				pdata = NULL;
				m_bAreadyRequestAllObj = true;
			}
		}
	}
}
//**************************************huanglongquan add by 2012年5月15日********************************/


void CWBDataModel::RemoveWBDTSSession(unsigned long ulSessionID)
{
	{
		XAutoLock SessionLock(m_csMapSessionID);
		IT_MapSessionInfo it = m_mapSessionInfo.find(ulSessionID);
		if (it != m_mapSessionInfo.end())
		{
			m_mapSessionInfo.erase(it);
		}
	}
}
//end IWBDTS
//--->net
void CWBDataModel::ProcessUIWbCmdData(eWbCmdOperation eCmdType,char* pData,int iLen)
{
	PROC_UI_DATA(CMD_ADD_ATTRIBUTE,AddAttribute);
	PROC_UI_DATA(CMD_DEL_ATTRIBUTE,DelAttribute);
	PROC_UI_DATA(CMD_CHANGE_ATTRIBUTE,ChangeAttribute);
	PROC_UI_DATA(CMD_DEL_ALL_ATTRIBUTE,DelAllAttribute);// huanglongquan add
	PROC_UI_DATA(CMD_DEL_PAGE_ALL_ATTRIBUTE,DelPageAllAttribute);  // huanglongquan add
	PROC_UI_DATA(CMD_REQUEST_ALL_ATTRIBUTE,RequestAttribute);
	PROC_UI_DATA(CMD_REQUEST_ALL_OBJ_ATTRIBUTE,RequestAllObj);
	PROC_UI_DATA(CMD_REDO_STATUS,Redo);
	PROC_UI_DATA(CMD_UNDO_STATUS,Undo);
/*	PROC_UI_DATA(CMD_CHANGE_LAYER,Layer);*/
	PROC_UI_DATA(CMD_P2P_DATA,P2PData);
	PROC_UI_DATA(CMD_P2MCU_DATA,P2MCU);
	PROC_UI_DATA(CMD_BROADCAST_DATA,BroadcastData);
#ifdef HUBEI_MEDICAL_SDK
	PROC_UI_DATA(CMD_ASKFOR_USERCOLOR,AskForUserColor);
#endif

}
void CWBDataModel::AddAttribute(char* pData,int iLen)
{
	//本地同步
	if (m_bUI && m_pIHPDTSNotify)
	{
	//	m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
	}
	//网络同步
	int iCmdType = *((int*)pData);
	char* pBuff = NULL;
	pBuff = pData+ sizeof(int);
	if (pBuff)
	{
		XMemIni xmlPacket;
		string strValue = pBuff;  //因为在pBuff中后面的存储中有“\0”   这是把char*转换成string
// 		//保存日志
// 		//格式：事件名称；字符串长度；数据；数据长度
// 		m_logWrite.Write(TEXT("AddAttribute"), strlen(TEXT("AddAttribute")), pBuff, iLen-sizeof(int),true);		
		xmlPacket.ResetString(strValue);
		string strPath = "";
		string strFileType = "";//文件后缀名
		unsigned long ulObjID = 0;
		unsigned long iFileSize = 0;
		unsigned long ulEventID = 0;
		if (xmlPacket.Get(KEY_FILE_PATH,strPath)
			&& xmlPacket.Get(KEY_EVENT_ID,ulEventID)
			&& xmlPacket.Get(KEY_OBJ_ID,ulObjID) 
			&& strPath != "")
		{
			MapWBAttributes* pWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			if (pWbAttribute)
			{
				IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
				if ((it == pWbAttribute->end()) && GetFileStatus(strPath,iFileSize,strFileType))
				{
					xmlPacket.Set(KEY_FILE_SIZE,iFileSize);
					xmlPacket.Set(KEY_FILE_TYPE,strFileType);
					xmlPacket.Set(KEY_FILE_POS,iFileSize);
					pWbAttribute->insert(MapWBAttributes::value_type(ulObjID,xmlPacket));
	//				StartStream(ulObjID,ulEventID,iFileSize,strFileType);
					StartStream(xmlPacket,0);
				}
			}		
		}
		else if (xmlPacket.Get(KEY_OBJ_ID,ulObjID) && (!xmlPacket.Get(KEY_FILE_PATH,strPath) || strPath == ""))
		{
			xmlPacket.Set(KEY_FILE_SIZE,0);
			xmlPacket.Set(KEY_FILE_POS,0);
			xmlPacket.Set(KEY_FILE_TYPE,"");
			xmlPacket.Set(KEY_FILE_PATH,"");
			string strXml = "";
			xmlPacket.GetString(strXml);
			int iLength = strXml.size() + sizeof(int);
			char* pBuff = new char[iLength+1];
			memset(pBuff,0,iLength+1);
			memcpy(pBuff,(char*)&iCmdType,sizeof(int));
			memcpy(pBuff + sizeof(int),strXml.c_str(),strXml.size());
			pBuff[iLength] = '\0';
			SendToAll(pBuff,iLength+1);
			delete []pBuff;
			pBuff = NULL;
		}
	}
}
void CWBDataModel::RequestAttribute(char* pData,int iLen)
{
	SendToAll(pData,iLen);
}
void CWBDataModel::ChangeAttribute(char* pData,int iLen)
{
// 	//保存日志
// 	//格式：事件名称；字符串长度；数据；数据长度
// 	m_logWrite.Write(TEXT("ChangeAttribute"), strlen(TEXT("ChangeAttribute")), pData+sizeof(int), iLen-sizeof(int),true);
// 	if (m_bUI && m_pIHPDTSNotify)
// 	{
// 		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
// 	}
	SendToAll(pData,iLen);
}
void CWBDataModel::DelAttribute(char* pData,int iLen)
{
// 	//保存日志
// 	//格式：事件名称；字符串长度；数据；数据长度
// 	//数据格式为：CMD(int) objID(int) eventID(long) sessionID(long)
// 	char* pbuff = pData;
// 	unsigned int cmd = *((unsigned int*)pbuff);
// 	pbuff += sizeof(int);
// 	unsigned int objID = *((unsigned int*)pbuff);
// 	pbuff += sizeof(int);
// 	unsigned long eventID = *((unsigned long*)pbuff);
// 	pbuff += sizeof(long);
// 	unsigned long sessionID = *((unsigned long*)pbuff);
// 	char strData[128];
// 	memset(strData,0,128);
// 	sprintf(strData,TEXT("command: %u; objectID: %u; eventID: %u; sessionID: %u;"),cmd, objID, eventID, sessionID);
// 	m_logWrite.Write(TEXT("DelAttribute"), strlen(TEXT("DelAttribute")), strData, strlen(strData)+1);

// 	if (m_bUI && m_pIHPDTSNotify)
// 	{
// 		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
// 	}
	SendToAll(pData,iLen);
}


void CWBDataModel::DelAllAttribute( char* pData,int iLen )
{
	if (m_bUI)
	{
		int iCmdType = *((int*)pData);
		if (iCmdType == CMD_DEL_ALL_ATTRIBUTE)
		{
			MapWBAttributes* pMapWbAttribute = NULL;
			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			if (pMapWbAttribute)
			{
				pMapWbAttribute->clear();
			}
//			m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
		}
	}
	SendToAll(pData,iLen);
}

void CWBDataModel::DelPageAllAttribute( char* pData,int iLen )
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		int iCmdType = *((int*)pData);
		char* pBuff = pData + sizeof(int);
		int nPageID = *((int*)pBuff);
		pBuff += sizeof(int);
		unsigned long ulEventID = *((unsigned long*)pBuff);
		if (iCmdType == CMD_DEL_PAGE_ALL_ATTRIBUTE)
		{
			MapWBAttributes* pMapWbAttribute = NULL;
			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			if (pMapWbAttribute)
			{
				XAutoLock l(m_csVectorWBAttribute);
				std::vector<unsigned long> vectorWBobjs;
				vectorWBobjs.clear();
				IT_MapWBAttributes it = pMapWbAttribute->begin();
				while (it != pMapWbAttribute->end())
				{
					int nIsLock = 0;
					bool bLock = false;
					if (it->second.Get(KEY_OBJ_LOCK,nIsLock))
					{
						bLock = (bool)nIsLock;
					}
					if (bLock)
					{
						it++;
						continue;
					}
					int nObjPageId = 0;
					it->second.Get(KEY_OBJ_PAGE,nObjPageId);
					IT_MapWBAttributes itTemp;
					if (nObjPageId == nPageID)
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
			m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
		}
	}
	SendToAll(pData,iLen);
}

void CWBDataModel::RequestAllObj(char* pData,int iLen)
{
	SendToAll(pData,iLen);
}
void CWBDataModel::Redo(char* pData,int iLen)
{
// 	TRACE("redo");
// 	m_logWrite.Write(TEXT("Redo"), strlen(TEXT("Redo")), pData+sizeof(int), iLen-sizeof(int),true);
// 	if (m_bUI && m_pIHPDTSNotify)
// 	{
// 		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
// 	}
	SendToAll(pData,iLen);
}
void CWBDataModel::Undo(char* pData,int iLen)
{
// 	//保存日志
// 	//格式：事件名称；字符串长度；数据；数据长度
// 	TRACE("undo");
// 	m_logWrite.Write(TEXT("Undo"), strlen(TEXT("Undo")), pData+sizeof(int), iLen-sizeof(int),true);
// 	if (m_bUI && m_pIHPDTSNotify)
// 	{
// 		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
// 	}
	SendToAll(pData,iLen);
}
// void CWBDataModel::Layer(char* pData,int iLen)
// {
// 	//保存日志
// 	//格式：事件名称；字符串长度；数据；数据长度
// 	TRACE("layer");
// 	m_logWrite.Write(TEXT("Layer"), strlen(TEXT("Layer")), pData+sizeof(int), iLen-sizeof(int),true);
// 	if(m_bUI && m_pIHPDTSNotify)
// 	{
// 		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
// 	}
// 	SendToAll(pData,iLen);
// }

void CWBDataModel::P2PData(char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
		SendToAll(pData,iLen);
	}
	else
	{
		char* pBuff = pData;
		pBuff += sizeof(unsigned int);
		unsigned int iSessionID = *((unsigned int*)pData);
		SendToPoint(iSessionID,pBuff,iLen - sizeof(unsigned int));
	}
}
void CWBDataModel::BroadcastData(char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
//		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
		SendToAll(pData,iLen);
	}
}
void CWBDataModel::P2MCU(char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		char* pBuff = pData + sizeof(int);//不要type类型
		SendToAll(pBuff,iLen - sizeof(int));//只发送数据  因为在*pData前面的是type类型占用一个int大小
	}
}

void CWBDataModel::AskForUserColor( char* pData,int iLen )
{
	SendToAll(pData,iLen);
}

//<---net
void CWBDataModel::ProcessDTSWbCmdData(eWbCmdOperation eCmdType,unsigned long iSessionID,char* pData,int iLen)
{
//	TRACE("void CWBDataModel::ProcessDTSWbCmdData <--net   eCmdType = %d\n",eCmdType);
	PROC_DTS_DATA(CMD_ADD_ATTRIBUTE,OnAddAttribute);//添加属性 它的值是从200开始
	PROC_DTS_DATA(CMD_REQUEST_ALL_ATTRIBUTE,OnRequestAttribute); //获取全部属性
	PROC_DTS_DATA(CMD_CHANGE_ATTRIBUTE,OnChangeAttribute);   //改变属性
	PROC_DTS_DATA(CMD_CLEANUP_ATTRIBUTE,OnCleanUpAttribute);//清空全部属性
	PROC_DTS_DATA(CMD_DEL_ATTRIBUTE,OnDelAttribute); //删除属性
	PROC_DTS_DATA(CMD_DEL_ALL_ATTRIBUTE,OnDelAllAttribute);//删除所有对象  add by 2011年11月25日16:52:40
	PROC_DTS_DATA(CMD_DEL_PAGE_ALL_ATTRIBUTE,OnDelPageAllAttribute);
	PROC_DTS_DATA(CMD_REQUEST_ALL_OBJ_ATTRIBUTE,OnRequestAllObj);//申请全部对象 客服端对这个消息不处理( mcu可以向上请求  )
	PROC_DTS_DATA(CMD_REDO_STATUS,OnRedo); //前进一步				 
	PROC_DTS_DATA(CMD_UNDO_STATUS,OnUndo); //后退一步
/*	PROC_DTS_DATA(CMD_CHANGE_LAYER,OnLayer);//层改变 */
	PROC_DTS_DATA(CMD_P2P_DATA,OnP2PData); //点对点临时数据
	PROC_DTS_DATA(CMD_BROADCAST_DATA,OnBroadcastData);//广播临时数据
	PROC_DTS_DATA(CMD_START_STREAM,OnStartStream); //开始发送数据
	PROC_DTS_DATA(CMD_RESTART_STREAM,OnReStartStream) //重新发送数据
	PROC_DTS_DATA(CMD_ASKFOR_STREAM,OnAskForStream); //申请数据包
	PROC_DTS_DATA(CMD_SAVE_STREAM,OnSaveStream);	 //写入数据包
	PROC_DTS_DATA(CMD_END_STREAM,OnEndStream);  //完成发送数据
	PROC_DTS_DATA(CMD_STOP_STREAM,OnStopStream);//停止发送流
	PROC_DTS_DATA(CMD_ERROR_STREAM,OnErrorStream);
	PROC_DTS_DATA(CMD_RELATE_STREAM,OnRelateAttributeToStream);
	//被动模式
	PROC_DTS_DATA(CMD_PASSIVE_START,OnPassiveStartStream);
	PROC_DTS_DATA(CMD_PASSIVE_SAVE,OnPassiveSaveStream);
	PROC_DTS_DATA(CMD_PASSIVE_END,OnPassiveEndStream);
	PROC_DTS_DATA(CMD_PASSIVE_ERROR,OnPassiveErrorStream);
#ifdef HUBEI_MEDICAL_SDK
	PROC_DTS_DATA(CMD_ASKFOR_USERCOLOR,OnAskForUserColor);
	PROC_DTS_DATA(CMD_RELATIVE_USERCOLOR,OnRelativeUserColor);
	PROC_DTS_DATA(CMD_BROADCAST_USERNAMECOLOR,OnBroadcastUserColor)
#endif
}
void CWBDataModel::OnAddAttribute(unsigned long iSessionID,char* pData,int iLen)
{
// 	保存日志
// 		//格式：事件名称；字符串长度；数据；数据长度
// 		m_logWrite.Write(TEXT("OnAddAttribute"), strlen(TEXT("OnAddAttribute")), pData+sizeof(int), iLen-sizeof(int),true);
// 		printf("OnAddAttribute");
	m_bMCUExistObj = true;
	if (m_bUI && m_pIHPDTSNotify)
	{
		XAutoLock vectorLock(m_csVectorWBAttribute);
		MapWBAttributes* pMapWBAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pMapWBAttribute)
		{
			XMemIni xmlPacket;
			if (ToMemIni(xmlPacket,pData + sizeof(int),iLen - sizeof(int)))
			{
				unsigned long ulObjID = 0;
				if (xmlPacket.Get(KEY_OBJ_ID,ulObjID))
				{
					IT_MapWBAttributes it = pMapWBAttribute->find(ulObjID);
					if (it != pMapWBAttribute->end())
					{
						string strPath = "";
						if (it->second.Get(KEY_FILE_PATH,strPath) && strPath != "")
						{
							//pMapWBAttribute->erase(it);
							xmlPacket.Set(KEY_FILE_PATH,strPath);
							int iCmd = CMD_ADD_ATTRIBUTE;
							string strValue = "";
							xmlPacket.GetString(strValue);
							int iLength = sizeof(int) + strValue.size();
							char* pBuff = new char[iLength +1];
							if (pBuff)
							{
								memset(pBuff,0,iLength + 1);
								memcpy(pBuff,(char*)&iCmd,sizeof(int));
								memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
								pBuff[iLength] = '\0';
								m_pIHPDTSNotify->onDTSUIDataNotify(pBuff,iLength + 1);
								
								/**add by yang *********************************************************/
								IT_MapWBAttributes itPageInfo = pMapWBAttribute->find(SHARE_OBJID);
								if (pMapWBAttribute->end()!= itPageInfo)
								{
									XMemIni xmlPageInfo = itPageInfo->second;
									int iCmd2 = CMD_CHANGE_ATTRIBUTE;
									string strPageInfo = "";
									xmlPageInfo.GetString(strPageInfo);
									int iDataLen = sizeof(int) + strPageInfo.size();
									char* pSend = new char[iDataLen+1];
									if (NULL != pSend)
									{
										memset(pSend, 0, iDataLen+1);
										memcpy(pSend, (char*)&iCmd2, sizeof(int));
										memcpy(pSend+sizeof(int), strPageInfo.c_str(), strPageInfo.size());
										pSend[iDataLen] = '\0';
										m_pIHPDTSNotify->onDTSUIDataNotify(pSend, iDataLen+1);
										delete[] pSend;
										pSend = NULL;
									}
								}
								/**end by yang****************************************************************/

								delete pBuff;
								pBuff = NULL;
								return;
							}	
						}
						else		//非文件对象不保存，但是对于key_obj_id=0，表示的是当前页的信息，客户端必须把它替换成当前的。
						{
							if (SHARE_OBJID == ulObjID)
							{
								pMapWBAttribute->erase(ulObjID);
								pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID, xmlPacket));
							}
						}
					}
					else		//非文件对象不保存，但是对于key_obj_id=0，表示的是当前页的信息，客户端必须保存
					{
						if (SHARE_OBJID == ulObjID)
						{
							pMapWBAttribute->erase(ulObjID);
							pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID, xmlPacket));
						}
					}
				}
			}
		}
		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
	}
	else if (m_pIHPDTSNotify)
	{
		XAutoLock vectorLock(m_csVectorWBAttribute);
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
					IT_MapWBAttributes it = pMapWBAttribute->find(ulObjID);
					if (it == pMapWBAttribute->end())
					{
						pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID,xmlPacket));
					}
					else
					{
						std::string strValue = "";
						xmlPacket.DeleteKey(KEY_FILE_PATH);
						xmlPacket.GetString(strValue);
						it->second.SetString(strValue);
					}
	/*				pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID,xmlPacket));*/
				}
				SendToOthers(iSessionID,pData,iLen);
			}
		}
	}
}
void CWBDataModel::CheckXmlPacket(XMemIni & xmlPacket)
{
	xmlPacket.DeleteKey(KEY_FILE_PATH);
	xmlPacket.DeleteKey(KEY_FILE_SIZE);
	xmlPacket.DeleteKey(KEY_FILE_POS);
	xmlPacket.DeleteKey(KEY_FILE_TYPE);
}
void CWBDataModel::CheckXmlString(string &strValue)
{
	XMemIni xmlPacket;
	xmlPacket.ResetString(strValue);
	CheckXmlPacket(xmlPacket);
	xmlPacket.GetString(strValue);
}
void CWBDataModel::OnRequestAttribute(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		int iCmdType = *((int*)pData);
		unsigned long ulObjID = *((unsigned long*)(pData + sizeof(int)));
		if (iCmdType == CMD_REQUEST_ALL_ATTRIBUTE)
		{ 
			MapWBAttributes* pMapWbAttribute = NULL;
			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			if (pMapWbAttribute)
			{
				IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				if (it != pMapWbAttribute->end())
				{
					XMemIni xmlPacket;
					char* pBuff = pData + sizeof(int) + sizeof(unsigned long);
					string strValue = pBuff;
					xmlPacket.ResetString(strValue);
					it->second.GetString(strValue);
					xmlPacket.SetString(strValue);
					xmlPacket.GetString(strValue);
					int iLength = sizeof(int) + sizeof(unsigned long) + strValue.size();
					pBuff = new char[iLength + 1];
					if (pBuff)
					{
						memset(pBuff,0,iLength);
						memcpy(pBuff,(char*)&iCmdType,sizeof(int));
						memcpy(pBuff + sizeof(int) ,(char*)&ulObjID,sizeof(unsigned long));
						memcpy(pBuff + sizeof(int) + sizeof(unsigned long) ,strValue.c_str(),strValue.size());
						pBuff[iLength] = '\0';
						m_pIHPDTSNotify->onDTSUIDataNotify(pBuff,iLength+1);
						delete pBuff;
						pBuff = NULL;
					}				
				}
				else
				{
					m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
				}
			}
		}	
	}
	else
	{
		int iCmdType = *((int*)pData);
		unsigned long ulObjID = *((unsigned long*)(pData + sizeof(int)));
		if (iCmdType == CMD_REQUEST_ALL_ATTRIBUTE)
		{
			XAutoLock l(m_csVectorWBAttribute);
			MapWBAttributes* pMapWbAttribute = NULL;
			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			if (pMapWbAttribute)
			{
				IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				if (it != pMapWbAttribute->end())
				{
					XMemIni xmlPacket = it->second;
					string strValue = "";
					CheckXmlPacket(xmlPacket);
					xmlPacket.GetString(strValue);
					int iSize = strValue.size()+sizeof(int);
					if(iSize != 0)
					{
						char* pBuff = new char[iSize+1];
						memset(pBuff,0,iSize+1);
						if (pBuff)
						{
							memcpy(pBuff,(char*)&iCmdType,sizeof(int));
							memcpy(pBuff + sizeof(int),strValue.c_str(),iSize);
							pBuff[iSize] = '\0';
							SendToPoint(iSessionID,pBuff,iSize+1);
							delete pBuff;
							pBuff = NULL;
						}
					}	
				}
			}
		}
	}
}
void CWBDataModel::OnRequestAllObj(unsigned long iSessionID,char* pData,int iLen)
{
/*	TRACE("mcu接收到请求所有对象的请求命令\n");*/
	if (m_bUI && m_pIHPDTSNotify)
	{
		//客户端对这个消息不做处理
	}
	else
	{
		if (m_bMCUExistObj)		   //该变量是用来判断当前mcu中是否已经保存了白板对象，如果没有就往其他mcu请求所有对象
		{
			int iCmdType = *((int*)pData);
			{
				XAutoLock l(m_csVectorWBAttribute);
				MapWBAttributes* pMapWbAttribute = NULL;

				pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
				if (pMapWbAttribute)
				{
				
					IT_MapWBAttributes it = pMapWbAttribute->begin();
					while (it != pMapWbAttribute->end())
					{
						XMemIni xmlPacket = it->second;
						string strPath = "";
						unsigned long ulObjID = 0;
						xmlPacket.Get(KEY_OBJ_ID,ulObjID);
						unsigned long ulEventID = 0;
						xmlPacket.Get(KEY_EVENT_ID,ulEventID);
						if (xmlPacket.Get(KEY_FILE_PATH,strPath) && strPath != "")
						{
							unsigned long ulSize = 0;
							string strType = "";
							xmlPacket.Get(KEY_FILE_TYPE,strType);
							xmlPacket.Get(KEY_FILE_SIZE,ulSize);
							StartStream(ulObjID,ulEventID,ulSize,strType,iSessionID);
						}
						else
						{
							//表示没有资源关联
							string strValue = "";
							xmlPacket.GetString(strValue);
							if (strValue != "")
							{
								int iCmd = CMD_ADD_ATTRIBUTE;
								int iLength = sizeof(int) + strValue.size();
								char* pBuff = new char[iLength+1];
								if (pBuff)
								{
									memset(pBuff,0,iLength+1);
									memcpy(pBuff,(char*)&iCmd,sizeof(int));
									memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
									pBuff[iLength] = '\0';
									SendToPoint(iSessionID,pBuff,iLength+1);	
									delete []pBuff;
									pBuff = NULL;
								}
							}
						}
						it++;
					}
					/************************************************************************/
					/* add by yang     2013-06-06                                                                */
				
					IT_MapWBAttributes it_CurPage = pMapWbAttribute->find(SHARE_OBJID);
					if (pMapWbAttribute->end() != it_CurPage)
					{
						XMemIni xmlCurPage = it_CurPage->second;
						int iCmdSetCurPage = CMD_CHANGE_ATTRIBUTE;
						std::string strValue="";
						xmlCurPage.GetString(strValue);
						int iSendDataLen = sizeof(int)+strValue.size();
						char* pNew = new char[iSendDataLen+1];
						
						if (pNew)
						{
							memset(pNew, 0, iSendDataLen+1);
							memcpy(pNew, (char*)&iCmdSetCurPage, sizeof(int));
							memcpy(pNew+sizeof(int), strValue.c_str(), strValue.size());
							pNew[iSendDataLen] = '\0';
							SendToPoint(iSessionID, pNew, iSendDataLen+1);
							delete[] pNew;
							pNew = NULL;
						}
					}
					/*****end by yang******************************************************/
				}
			}
			m_bAreadyRequestAllObj = true;
		}
		else
		{
//**************************************huanglongquan add by 2012年5月15日********************************/
			bool bHost = false;
			unsigned long ulSId = 0;
			{
				XAutoLock SessionLock(m_csMapSessionID);
				if (m_mapSessionInfo.size() < 2)
				{
					m_bAreadyRequestAllObj = false;
					return;
				}
				IT_MapSessionInfo it = m_mapSessionInfo.begin();
				while (it != m_mapSessionInfo.end())
				{ 
					bHost = it->second.bHost;
					if (bHost)
					{
						ulSId =  it->second.ulSessionID;	
						if (ulSId != iSessionID)
						{
							SendToPoint(ulSId,pData, iLen);
						}
	//					break;
					}
					it++;
				}
				if (bHost)
				{
	//				SendToPoint(ulSId,pData, iLen);
				}
			}
	/*	m_bMCUExistObj = true;*/
//**************************************huanglongquan add by 2012年5月15日********************************/
		}
	}
}
void CWBDataModel::OnRedo(unsigned long iSessionID,char* pData,int iLen)
{
// 	//保存日志
// 	//格式：事件名称；字符串长度；数据；数据长度
// 	m_logWrite.Write(TEXT("OnRedo"), strlen(TEXT("OnRedo")), pData+sizeof(int), iLen-sizeof(int),true);
	if (m_bUI && m_pIHPDTSNotify)
	{
		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
	}
	else
	{
		if (m_iSlidePos == 0)
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
}
void CWBDataModel::OnUndo(unsigned long iSessionID,char* pData,int iLen)
{
// 	//保存日志
// 	//格式：事件名称；字符串长度；数据；数据长度
// 	m_logWrite.Write(TEXT("OnUndo"), strlen(TEXT("OnUndo")), pData+sizeof(int), iLen-sizeof(int),true);
	if (m_bUI && m_pIHPDTSNotify)
	{
		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
	}
	else
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
}
// void CWBDataModel::OnLayer(unsigned long iSessionID,char* pData,int iLen)
// {
// 	//保存日志
// 	//格式：事件名称；字符串长度；数据；数据长度
// 	m_logWrite.Write(TEXT("OnLayer"), strlen(TEXT("OnLayer")), pData+sizeof(int), iLen-sizeof(int),true);
// 	if (m_bUI &&m_pIHPDTSNotify)
// 	{
// 		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
// 	}
// 	else
// 	{
// 		char* pBuff = pData + sizeof(int);
// 		if (pBuff)
// 		{
// 			unsigned long ulEventID = *((unsigned long*)pBuff);
// 			pBuff += sizeof(unsigned long);
// 			unsigned long ulObjIDFrom = *((unsigned long*)pBuff);
// 			pBuff += sizeof(unsigned long);
// 			unsigned long ulObjIDTo = *((unsigned long*)pBuff);
// 
// 			XAutoLock l(m_csVectorWBAttribute);
// 			
// 			if (NeedToSlide(ulEventID))
// 			{
// 				SlideNextStatus();
// 			}
// 			MapWBAttributes* pMapWbAttribute = NULL;
// 			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
// 			if (pMapWbAttribute)
// 			{
// 				VectorWBAttributes vectorWbAttributes;
// 				vectorWbAttributes.clear();
// 				IT_MapWBAttributes it = pMapWbAttribute->begin();
// 
// 				while (it != pMapWbAttribute->end())
// 				{
// 					unsigned long ulObjID = 0;
// 					it->second.Get(KEY_OBJ_ID,ulObjID);
// 					IT_MapWBAttributes itTemp;
// 					if (ulObjID == ulObjIDFrom)
// 					{
// 						itTemp = pMapWbAttribute->find(ulObjIDTo);
// 						if (itTemp != pMapWbAttribute->end())
// 						{
// 							XMemIni xmlPacket;
// 							string strValue = "";
// 							itTemp->second.GetString(strValue);
// 							if (strValue != "")
// 							{
// 								xmlPacket.ResetString(strValue);
// 								xmlPacket.Set(KEY_EVENT_ID,ulEventID);
// 								vectorWbAttributes.push_back(xmlPacket);
// 							}
// 						}
// 					}	
// 					else if (ulObjID == ulObjIDTo)
// 					{
// 						itTemp = pMapWbAttribute->find(ulObjIDFrom);
// 						if (itTemp != pMapWbAttribute->end())
// 						{
// 							XMemIni xmlPacket;
// 							string strValue = "";
// 							itTemp->second.GetString(strValue);
// 							if (strValue != "")
// 							{
// 								xmlPacket.ResetString(strValue);
// 								xmlPacket.Set(KEY_EVENT_ID,ulEventID);
// 								vectorWbAttributes.push_back(xmlPacket);
// 							}
// 						}
// 					}
// 					else
// 					{
// 						XMemIni xmlPacket;
// 						string strValue = "";
// 					    it->second.GetString(strValue);
// 						if (strValue != "")
// 						{
// 							xmlPacket.ResetString(strValue);
// 							vectorWbAttributes.push_back(xmlPacket);
// 						}
// 					}
// 					it++;
// 				}
// 				pMapWbAttribute->clear();
// 				for (int i = 0;i < vectorWbAttributes.size();i++)
// 				{
// 					unsigned long ulObjID = 0;
// 					vectorWbAttributes[i].Get(KEY_OBJ_ID,ulObjID);
// 					pMapWbAttribute->insert(MapWBAttributes::value_type(ulObjID,vectorWbAttributes[i])); 
// 				}
// 				int iLength = sizeof(int) + 2*sizeof(unsigned long);
// 				pBuff = new char[iLength];
// 				if (pBuff)
// 				{
// 					memset(pBuff,0,iLength);
// 					memcpy(pBuff,pData,sizeof(int));
// 					memcpy(pBuff + sizeof(int),(char*)&ulObjIDFrom,sizeof(unsigned long));
// 					memcpy(pBuff + sizeof(int) + sizeof(unsigned long),(char*)&ulObjIDTo,sizeof(unsigned long));
// 					SendToAll(pBuff,iLength);
// 					delete pBuff;
// 					pBuff = NULL;
// 				}
// 				
// 			}	
// 		}
// 		
// 	}
// }
// void CWBDataModel::ReSortLayer(int iStatusFrom,int iStatusTo)
// {
// 	MapWBAttributes* pMapWbAttributeFrom = &m_vectorWbStatus[iStatusFrom];
// 	MapWBAttributes* pMapWbAttributeTo = &m_vectorWbStatus[iStatusTo];
// 
// 	IT_MapWBAttributes itTo = pMapWbAttributeTo->begin();
// 	IT_MapWBAttributes itFrom = pMapWbAttributeFrom->begin();
// 	unsigned long ulObjIDTo = 0;
// 	unsigned long ulObjIDFrom = 0;
// 	int iCmdType = 0;
// 	char* pBuff = NULL;
// 	while (itTo != pMapWbAttributeTo->end() && itFrom != pMapWbAttributeFrom->end())
// 	{
// 		itTo->second.Get(KEY_OBJ_ID,ulObjIDTo);
// 		itFrom->second.Get(KEY_OBJ_ID,ulObjIDFrom);
// 		if (ulObjIDTo == ulObjIDFrom)
// 		{
// 			itTo++;
// 			itFrom++;
// 			
// 		}
// 		else
// 		{
// 			IT_MapWBAttributes it = pMapWbAttributeFrom->find(ulObjIDTo);
// 			if (it !=pMapWbAttributeFrom->end())
// 			{
// 				iCmdType = CMD_CHANGE_LAYER;
// 	
// 				int iLength = sizeof(int) + 2*sizeof(unsigned long);
// 				pBuff = new char[iLength];
// 				if (pBuff)
// 				{
// 					memset(pBuff,0,iLength);
// 					memcpy(pBuff,(char*)&iCmdType,sizeof(int));
// 					memcpy(pBuff + sizeof(int),(char*)&ulObjIDFrom,sizeof(unsigned long));
// 					memcpy(pBuff + sizeof(int) + sizeof(unsigned long),(char*)&ulObjIDTo,sizeof(unsigned long));
// 					SendToAll(pBuff,iLength);
// 					delete pBuff;
// 					pBuff = NULL;
// 				}
// 				itTo++;
// 				itFrom++;
// 			}
// 			else
// 			{
// 				itTo++;
// 			}
// 		}
// 	}
// }
void CWBDataModel::StatusToStatus(int iFrom,int iTo)
{
	MapWBAttributes* pMapWbAttributeFrom = &m_vectorWbStatus[iFrom];
	MapWBAttributes* pMapWbAttributeTo = &m_vectorWbStatus[iTo];
	if (pMapWbAttributeFrom && pMapWbAttributeTo)
	{
		XAutoLock l(m_csVectorWBAttribute);
		IT_MapWBAttributes itTo = pMapWbAttributeTo->begin();
		IT_MapWBAttributes itFrom;
		unsigned long ulObjID = 0;
		
		int iCmdType = 0;
		string strValueTo = "";
		char* pBuff = NULL;

		while (itTo != pMapWbAttributeTo->end())
		{
			if (itTo->second.Get(KEY_OBJ_ID,ulObjID) && ulObjID != SHARE_OBJID)
			{
				itFrom = pMapWbAttributeFrom->find(ulObjID);
				if (itFrom == pMapWbAttributeFrom->end())
				{			
					string strPath = "";
					if (itTo->second.Get(KEY_FILE_PATH,strPath) && strPath != "")
					{
/*						string strType = "";*/
						unsigned long ulSize = 0;
						unsigned long ulPos = 0;
						XMemIni xmlobj;
						std::string  strTemp = "";
						itTo->second.GetString(strTemp);
						xmlobj.SetString(strTemp);
						xmlobj.DeleteKey(KEY_FILE_NAME);
// 						if (xmlobj.Get(KEY_FILE_SIZE,ulSize) && xmlobj.Get(KEY_FILE_POS,ulPos))
// 						{
// 							if (ulSize == ulPos)
// 							{
// 								 StartStream(xmlobj,0);
// 							}
// 							else
// 							{
// 								ReStartStream(ulObjID,xmlobj,0);
// 							}
// 						}
// 						else
// 						{
// 							StartStream(xmlobj,0);
// 						}
						StartStream(xmlobj,0);
					}
					else//表示没有关联资源文件
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
							SendToAll(pBuff,iLength+1);
							delete pBuff;
							pBuff = NULL;
						}   
					}
					           
				}
				else//说明存在
				{
					unsigned long ulEventIDTo = 0;
					unsigned long ulEventIDFrom = 0;
					itTo->second.Get(KEY_EVENT_ID,ulEventIDTo);
					itFrom->second.Get(KEY_EVENT_ID,ulEventIDFrom);
					if (ulEventIDFrom != ulEventIDTo)
					{
						iCmdType = CMD_CHANGE_ATTRIBUTE;
						itTo->second.GetString(strValueTo);

						int nTranspondData = 1;
						itTo->second.Set(KEY_TRANSPOND_DATA,nTranspondData);
						CheckXmlString(strValueTo);
						itTo->second.DeleteKey(KEY_TRANSPOND_DATA);

						int iLength = sizeof(int) + strValueTo.size();
						pBuff = new char[iLength+1];
						if (pBuff)
						{
							memset(pBuff,0,iLength + 1);
							memcpy(pBuff,(char*)&iCmdType,sizeof(int));
							memcpy(pBuff + sizeof(int),strValueTo.c_str(),strValueTo.size());
							pBuff[iLength] = '\0';
							SendToAll(pBuff,iLength+1);
							delete pBuff;
							pBuff = NULL;
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
			if (ulObjID != SHARE_OBJID)
			{
				itTo = pMapWbAttributeTo->find(ulObjID);
				if (itTo ==  pMapWbAttributeTo->end())
				{
					if (NeedToStopStream(ulObjID))
					{
						NotifyStopStream(ulObjID);
					}
					if (NeedToNotifyCleanUpAttribute(ulObjID))
					{
						NotifyCleanUpAttribute(ulObjID);
					}
					iCmdType = CMD_DEL_ATTRIBUTE;
				    unsigned long ulEventID   = m_vectorEventID[iTo];
					int iLength = sizeof(int) + sizeof(unsigned long)  + sizeof(unsigned long);
					pBuff = new char[iLength +1];
					if (pBuff)
					{
						memset(pBuff,0,iLength);
						memcpy(pBuff,(char*)&iCmdType,sizeof(int));
						memcpy(pBuff + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
						memcpy(pBuff +sizeof(int) + sizeof(unsigned long),(char*)&ulEventID,sizeof(unsigned long));	    //添加上了事件id用于白板刷新	by huanglongquan
						pBuff[iLength] = '\0';
						SendToAll(pBuff,iLength +1);
						delete pBuff;
						pBuff = NULL;
					}
				}
			}
			itFrom++;
		}
// 		//层顺序调整
// 		ReSortLayer(iFrom,iTo);
	}
}
// void CWBDataModel::StatusToStatus(int iFrom,int iTo)
// {
// 	MapWBAttributes* pMapWbAttribute = &m_vectorWbStatus[iFrom];
// 	MapWBAttributes* pMapWbAttributeNext = &m_vectorWbStatus[iTo];
// 	if (pMapWbAttributeNext && pMapWbAttribute)
// 	{
// 		XAutoLock l(m_csVectorWBAttribute);
// 		if (pMapWbAttribute->size() == pMapWbAttributeNext->size())
// 		{
// 			IT_MapWBAttributes itNext = pMapWbAttributeNext->begin();
// 			while (itNext != pMapWbAttributeNext->end())
// 			{
// 				unsigned long ulObjID = itNext->first;
// 				IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
// 				if (it != pMapWbAttribute->end())
// 				{
// 					string strValue = "";
// 					it->second.GetString(strValue);
// 					string strValueNext = "";
// 					itNext->second.GetString(strValueNext);
// 					if (strValue != strValueNext)
// 					{
// 						int iCmdType = CMD_CHANGE_ATTRIBUTE;
// 						int iLength = sizeof(int) + strValueNext.size();
// 						char* pBuff = new char[iLength+1];
// 						if (pBuff)
// 						{
// 							memset(pBuff,0,iLength + 1);
// 							memcpy(pBuff,(char*)&iCmdType,sizeof(int));
// 							memcpy(pBuff + sizeof(int),strValueNext.c_str(),strValueNext.size());
// 							pBuff[iLength] = '\0';
// 							SendToAll(pBuff,iLength+1);
// 							delete pBuff;
// 							pBuff = NULL;
// 						}
// 						break;
// 					}
// 				}
// 				itNext++;
// 			}
// 		}
// 		else if(pMapWbAttributeNext->size() > pMapWbAttribute->size())
// 		{
// 			IT_MapWBAttributes itNext = pMapWbAttributeNext->begin();
// 			while (itNext != pMapWbAttributeNext->end())
// 			{
// 				unsigned long ulObjID = itNext->first;
// 				IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
// 				if (it == pMapWbAttribute->end())
// 				{
// 					int iCmdType = CMD_ADD_ATTRIBUTE;
// 					string strValueNext = "";
// 					itNext->second.GetString(strValueNext);
// 					int iLength = sizeof(int) + strValueNext.size();
// 					char* pBuff = new char[iLength+1];
// 					if (pBuff)
// 					{
// 						memset(pBuff,0,iLength+1);
// 						memcpy(pBuff,(char*)&iCmdType,sizeof(int));
// 						memcpy(pBuff + sizeof(int),strValueNext.c_str(),strValueNext.size());
// 						pBuff[iLength] = '\0';
// 						SendToAll(pBuff,iLength+1);
// 						delete pBuff;
// 						pBuff = NULL;
// 					}
// 					break;
// 				}
// 				itNext++;
// 			}
// 		}
// 		else if(pMapWbAttributeNext->size() < pMapWbAttribute->size())
// 		{
// 			IT_MapWBAttributes it = pMapWbAttribute->begin();
// 			while (it != pMapWbAttribute->end())
// 			{
// 				unsigned long ulObjID = it->first;
// 				IT_MapWBAttributes itNext = pMapWbAttributeNext->find(ulObjID);
// 				if (itNext == pMapWbAttributeNext->end())
// 				{
// 					NotifyStopRecvStream(ulObjID);
// 					int iCmdType = CMD_DEL_ATTRIBUTE;
// 					int iLength = sizeof(int) + sizeof(unsigned long);
// 					char* pBuff = new char[iLength];
// 					if (pBuff)
// 					{
// 						memset(pBuff,0,iLength);
// 						memcpy(pBuff,(char*)&iCmdType,sizeof(int));
// 						memcpy(pBuff + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
// 						SendToAll(pBuff,iLength);
// 						delete pBuff;
// 						pBuff = NULL;
// 					}
// 					break;
// 				}
// 				it++;
// 			}
// 		}
// 	}
// }
void CWBDataModel::OnP2PData(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
	}
	else
	{
		P2PData(pData,iLen);
	}
}
void CWBDataModel::OnBroadcastData(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
	}
	else
	{
		SendToOthers(iSessionID,pData,iLen);
	}
}
void CWBDataModel::OnCleanUpAttribute(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
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
}

void CWBDataModel::OnDelAllAttribute(unsigned long iSessionID,char* pData,int iLen)
{
// 	//保存日志
// 	//格式：事件名称；字符串长度；数据；数据长度
// 	TRACE("CWDataModel ::OnDelAllAttribuet 删除所有对象");
// 	m_logWrite.Write(TEXT("OnDelAllAttribute"), strlen(TEXT("OnDelAllAttribute")), pData+sizeof(int), iLen-sizeof(int),true);

	if (m_bUI && m_pIHPDTSNotify)
	{
		int iCmdType = *((int*)pData);
		if (iCmdType == CMD_DEL_ALL_ATTRIBUTE)
		{
			MapWBAttributes* pMapWbAttribute = NULL;
			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			if (pMapWbAttribute)
			{
				pMapWbAttribute->clear();
			}
			m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
		}
	}
	else
	{
		MapWBAttributes* pMapWbAttribute = NULL;
		for (int i = 0;i<m_iNumOfStatus;i++)
		{
			pMapWbAttribute = &m_vectorWbStatus[i];
			if (pMapWbAttribute)
			{
				pMapWbAttribute->clear();
			}
		}
		m_vectorWbStatus.clear();
		m_vectorEventID.clear();		
		for (int i = 0;i<m_iNumOfStatus;i++)
		{
			XMemIni shareXml;
			MapWBAttributes mapData;
			shareXml.Set(KEY_OBJ_ID,SHARE_OBJID);
			mapData[SHARE_OBJID] = shareXml;
			m_vectorWbStatus.push_back(mapData);
			m_vectorEventID.push_back(0);
		}
		//SendToAll(pData,iLen);
		SendToOthers(iSessionID,pData,iLen);
	}
}

void CWBDataModel::OnDelPageAllAttribute( unsigned long iSessionID,char* pData, int iLen )
{
	if (m_bUI && m_pIHPDTSNotify)
	{
 		int iCmdType = *((int*)pData);
 		char* pBuff = pData + sizeof(int);
 		int nPageID = *((int*)pBuff);
 		pBuff += sizeof(int);
 		unsigned long ulEventID = *((unsigned long*)pBuff);
		if (iCmdType == CMD_DEL_PAGE_ALL_ATTRIBUTE)
		{
 			MapWBAttributes* pMapWbAttribute = NULL;
 			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
 			if (pMapWbAttribute)
 			{
 				XAutoLock l(m_csVectorWBAttribute);
				std::vector<unsigned long> vectorWBobjs;
				vectorWBobjs.clear();
				IT_MapWBAttributes it = pMapWbAttribute->begin();
				while (it != pMapWbAttribute->end())
				{
					int nIsLock = 0;
					bool bLock = false;
					if (it->second.Get(KEY_OBJ_LOCK,nIsLock))
					{
						bLock = (bool)nIsLock;
					}
					if (bLock)
					{
						it++;
						continue;
					}
					int nObjPageId = 0;
					it->second.Get(KEY_OBJ_PAGE,nObjPageId);
					IT_MapWBAttributes itTemp;
					if (nObjPageId == nPageID)
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
			m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
		}
	}
	else
	{
		int iCmdType = *((int*)pData);
		if (CMD_DEL_PAGE_ALL_ATTRIBUTE != iCmdType )
		{
			return;
		}
		char* pBuff = pData + sizeof(int);
		int nPageID = *((int*)pBuff);
		if (nPageID < 0)
		{
			return;
		}
		pBuff += sizeof(int);
		unsigned long ulEventID = *((unsigned long*)pBuff);

		if (NeedToSlide(ulEventID))
		{
			SlideNextStatus();
		}	
		MapWBAttributes* pMapWbAttribute = NULL;
		pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pMapWbAttribute)	  //首先查找对象id 保存到容器中 在从容器中取数据然后逐条删除它
		{
			XAutoLock l(m_csVectorWBAttribute);
			std::vector<unsigned long> vectorWBobjs;
			vectorWBobjs.clear();
			IT_MapWBAttributes it = pMapWbAttribute->begin();
			while (it != pMapWbAttribute->end())
			{
				int nIsLock = 0;
				bool bLock = false;
				if (it->second.Get(KEY_OBJ_LOCK,nIsLock))
				{
					bLock = (bool)nIsLock;
				}
				int nIsBK = 0;
				bool bIsKG = false;
				if (it->second.Get(KEY_OBJ_BKG, nIsBK))
				{
					bIsKG = (bool)nIsBK;
				}
				if (bLock || bIsKG)
				{
					it++;
					continue;
				}
				int nObjPageId = 0;
				it->second.Get(KEY_OBJ_PAGE,nObjPageId);
				IT_MapWBAttributes itTemp;
				if (nObjPageId == nPageID)
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
			SendToOthers(iSessionID,pData,iLen);
		}
	}
}
void CWBDataModel::OnChangeAttribute(unsigned long iSessionID,char* pData,int iLen)
{
	// 	//保存日志
	// 	//格式：事件名称；字符串长度；数据；数据长度
	// 	m_logWrite.Write(TEXT("OnChangeAttribute"), strlen(TEXT("OnChangeAttribute")), pData+sizeof(int), iLen-sizeof(int),true);
	m_bMCUExistObj = true;

	if (m_bUI && m_pIHPDTSNotify)
	{
		XAutoLock l(m_csVectorWBAttribute);
		char* pBuff = pData + sizeof(int);
		std::string strValue = "";
		if (pBuff)
		{
			strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.SetString(strValue);
//			CheckXmlPacketPath(xmlPacket);	    //更新文件对象的路径
			xmlPacket.DeleteKey(KEY_TRANSPOND_DATA);	 //删除判断是不是要转发信息的标识
			CheckXmlString(strValue);
			unsigned long ulObjID = 0;
			if (xmlPacket.Get(KEY_OBJ_ID,ulObjID))
			{	
				/************************************************************************/
				/* add by yang 2013-06-07
				此处是为了修改后进会议成员所在页不同步的问题*/
				if (SHARE_OBJID == ulObjID)
				{
					MapWBAttributes* pMapWBAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
					pMapWBAttribute->erase(ulObjID);
					pMapWBAttribute->insert(MapWBAttributes::value_type(ulObjID, xmlPacket));
				}
				/************************************************************************/
				int iCmdType = CMD_CHANGE_ATTRIBUTE;
				int iLength = strValue.size() + sizeof(int);
				pBuff = new char[iLength + 1];
				if (pBuff)
				{
					memset(pBuff,0,iLength + 1);
					memcpy(pBuff ,(char*)&iCmdType,sizeof(int));
					memcpy(pBuff + sizeof(int) ,strValue.c_str(),strValue.size());
					pBuff[iLength] = '\0';
					m_pIHPDTSNotify->onDTSUIDataNotify(pBuff,iLength + 1);
					delete pBuff;
					pBuff = NULL;
				}
				// 				MapWBAttributes* pMapWbAttribute = NULL;
				// 				pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
				// 				if (pMapWbAttribute)
				// 				{
				// 					IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				// 					if (it != pMapWbAttribute->end())
				// 					{
				// 						unsigned long ulPos = 0;
				// 						unsigned long ulSize = 0;
				// 						it->second.Get(KEY_FILE_POS,ulPos);
				// 						it->second.Get(KEY_FILE_SIZE,ulSize);
				// 						it->second.SetString(strValue);
				// 						if (ulPos == ulSize)
				// 						{
				// 							m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
				// 						}
				// 						else
				// 						{
				// //							int iCmdType = CMD_REQUEST_ALL_ATTRIBUTE;
				// 							int iCmdType = CMD_CHANGE_ATTRIBUTE;
				// 							int iLength = strValue.size() + sizeof(int) + sizeof(unsigned long);
				// 							pBuff = new char[iLength + 1];
				// 							if (pBuff)
				// 							{
				// 								memset(pBuff,0,iLength + 1);
				// 								memcpy(pBuff ,(char*)&iCmdType,sizeof(int));
				// 								memcpy(pBuff + sizeof(int) ,(char*)&ulObjID,sizeof(unsigned long));
				// 								memcpy(pBuff + sizeof(int) + sizeof(unsigned long),strValue.c_str(),strValue.size());
				// 								pBuff[iLength] = '\0';
				// 								m_pIHPDTSNotify->onDTSUIDataNotify(pBuff,iLength + 1);
				// 								delete pBuff;
				// 								pBuff = NULL;
				// 							}
				// 						}
				// 					}
				// 					else
				// 					{
				// 						m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
				// 					}
				// 				}
			}
		}
	}
	else
	{
		int iCmdType = *((int*)pData);
		char* pBuff = pData + sizeof(int);
		std::string strValue = "";
		if (pBuff)
		{
			strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.SetString(strValue);
			//			CheckXmlString(strValue);
			unsigned long ulObjID = 0;
			if (xmlPacket.Get(KEY_OBJ_ID,ulObjID))
			{
				XAutoLock l(m_csVectorWBAttribute);
				MapWBAttributes* pMapWbAttribute = NULL;
				CHECK_EXIST;

				if (ulObjID == SHARE_OBJID)
				{
					for (int i = 0;i < m_iNumOfStatus;i++)
					{
						pMapWbAttribute = &m_vectorWbStatus[i];
						IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
						if (it != pMapWbAttribute->end())
						{
						
							it->second.SetString(strValue);
						}
					}
					/************************************************************************/
					/* 这里用SendTOAll替代SendToOthers是因为
					在a端翻到上一页，b端翻到下一页会出现不同步问题*/
					
					SendToAll(pData, iLen);
					//SendToOthers(iSessionID,pData,iLen);
					/************************************************************************/
				}
				else
				{
					unsigned long ulEventID = 0;
					if (xmlPacket.Get(KEY_EVENT_ID,ulEventID) && NeedToSlide(ulEventID))
					{
						SlideNextStatus();
					}
					pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
					if (pMapWbAttribute)
					{
						IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
						if (it != pMapWbAttribute->end())				  //对象存在的情况下
						{
							xmlPacket.DeleteKey(KEY_FILE_PATH);
							xmlPacket.GetString(strValue);
							it->second.SetString(strValue);
					//****************************判断是转发消息还是处理数据再转发******
							int nTranspondData = 0;
							if ((xmlPacket.Get(KEY_TRANSPOND_DATA,nTranspondData)) && (nTranspondData != 0))
							{
								it->second.DeleteKey(KEY_TRANSPOND_DATA);
								SendToOthers(iSessionID,pData,iLen);
							}
							else
							{
								std::string strPath = "";
								it->second.Get(KEY_FILE_PATH,strPath);
								xmlPacket.Set(KEY_FILE_PATH,strPath);
								nTranspondData = 1;
								xmlPacket.Set(KEY_TRANSPOND_DATA,nTranspondData);
								xmlPacket.GetString(strValue);
								//						CheckXmlString(strValue);
								int iLength = strValue.size() + sizeof(int);
								pBuff = new char[iLength + 1];
								if (pBuff)
								{
									memset(pBuff,0,iLength + 1);
									memcpy(pBuff,(char*)&iCmdType,sizeof(int));
									memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
									pBuff[iLength] = '\0';
									SendToAll(pBuff,iLength + 1); //sendToOthers改为SendToAll
									delete pBuff;
									pBuff = NULL;
								}	
							}
						}
					}
				}
			}
		}
	}
}


void CWBDataModel::OnDelAttribute(unsigned long iSessionID,char* pData,int iLen)
{
// 	//保存日志
// 	//格式：事件名称；字符串长度；数据；数据长度
// 	//数据格式为：CMD(int) objID(int) eventID(long) sessionID(long)
// 	char* pbuff = pData;
// 	unsigned int cmd = *((unsigned int*)pbuff);
// 	pbuff += sizeof(int);
// 	unsigned int objID = *((unsigned int*)pbuff);
// 	pbuff += sizeof(int);
// 	unsigned long eventID = *((unsigned long*)pbuff);
// 	pbuff += sizeof(long);
// 	unsigned long sessionID = *((unsigned long*)pbuff);
// 	char strData[128];
// 	memset(strData,0,128);
// 	sprintf(strData,TEXT("command: %u; objectID: %u; eventID: %u; sessionID: %u;"),cmd, objID, eventID, sessionID);
// 	m_logWrite.Write(TEXT("OnDelAttribute"), strlen(TEXT("OnDelAttribute")), strData, strlen(strData)+1);

	if (m_bUI && m_pIHPDTSNotify)
	{
		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
	}
	else
	{
		int iCmdType = *((int*)pData);
		char* pBuff = pData + sizeof(int);
		unsigned long ulObjID = *((unsigned long*)pBuff);
		pBuff += sizeof(unsigned long);
		unsigned long ulEventID = *((unsigned long*)pBuff);
		if (iCmdType == CMD_DEL_ATTRIBUTE)
		{
			XAutoLock l(m_csVectorWBAttribute);
			CHECK_EXIST;
			if (NeedToStopStream(ulObjID))
			{
				NotifyStopStream(ulObjID);
			}
			if (NeedToSlide(ulEventID))
			{
				SlideNextStatus();
			}
			if (NeedToNotifyCleanUpAttribute(ulObjID))
			{
				NotifyCleanUpAttribute(ulObjID);
			}
			MapWBAttributes* pMapWbAttribute = NULL;
			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			if (pMapWbAttribute)
			{
				IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				if (it != pMapWbAttribute->end())
				{
					pMapWbAttribute->erase(it);
					SendToOthers(iSessionID,pData,iLen);
				}
			}
		}
	}
}

bool CWBDataModel::NeedToNotifyCleanUpAttribute(unsigned long ulObjID)
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

void CWBDataModel::NotifyCleanUpAttribute(unsigned long ulObjID,unsigned long ulSessionID)
{
	int iCmdType = CMD_CLEANUP_ATTRIBUTE;
	int iLength = sizeof(int) + sizeof(unsigned long);
	char* pData = new char[iLength];
	if (pData)
	{
		memset(pData,0,iLength);
		memcpy(pData,(char*)&iCmdType,sizeof(int));
		memcpy(pData + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
		if (ulSessionID == 0)
		{
			SendToAll(pData,iLength);
		}
		else
		{
			SendToOthers(ulSessionID,pData,iLength);
		}
		delete pData;
		pData = NULL;
	}
}
bool CWBDataModel::NeedToStopStream(unsigned long ulObjID)
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
void CWBDataModel::NotifyStopStream(unsigned long ulObjID)
{
	int iCmdType = CMD_STOP_STREAM;
	unsigned long ulSessionID = 0;
	MapWBAttributes* pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
	if (pMapWbAttribute)
	{
		IT_MapWBAttributes it  = pMapWbAttribute->find(ulObjID);
		if (it != pMapWbAttribute->end())
		{
			it->second.Get(KEY_FILE_SENDER,ulSessionID);
		}
	}
	int iLength = sizeof(int) + sizeof(unsigned long) + sizeof(unsigned long);
	char* pData = new char[iLength];
	if (pData)
	{
		memset(pData,0,iLength);
		memcpy(pData,(char*)&iCmdType,sizeof(int));
		memcpy(pData + sizeof(int) ,(char*)&ulObjID,sizeof(unsigned long));
		memcpy(pData+ sizeof(int) +sizeof(unsigned long),(char*)&ulSessionID,sizeof(unsigned long));
		//SendToOthers(ulSessionID,pData,iLength);
		SendToAll(pData,iLength);
		delete pData;
		pData = NULL;
	}
}
void CWBDataModel::OnRelateAttributeToStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		XMemIni xmlPacket;
		char* pBuff = pData;
		pBuff += sizeof(int);

		string strValue = pBuff;
		if (strValue != "")
		{
			xmlPacket.ResetString(strValue);
			CheckXmlPacket(xmlPacket);
			unsigned long ulObjID = 0;
			if (xmlPacket.Get(KEY_OBJ_ID,ulObjID))
			{
				XAutoLock l(m_csVectorWBAttribute);
				MapWBAttributes* pWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
				if (pWbAttribute)
				{
					IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
					if (it != pWbAttribute->end())
					{
						it->second.SetString(strValue);
						it->second.GetString(strValue);
					}
				}
			}
			int iCmd = CMD_RELATE_STREAM;
			int iLength = sizeof(int) + strValue.size();
			pBuff = new char[iLength + 1];
			memset(pBuff,0,iLength + 1);
			memcpy(pBuff,(char*)&iCmd,sizeof(int));
			memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
			pBuff[iLength] = '\0';
			m_pIHPDTSNotify->onDTSUIDataNotify(pBuff, iLength + 1);
			if (pBuff)
			{
				delete pBuff;
				pBuff = NULL;
			}
		}
		
	}
	else
	{
		XMemIni xmlPacket;
        char* pBuff = pData;
		pBuff += sizeof(int);
		string strValue = pBuff;
		if (strValue != "")
		{
			xmlPacket.SetString(strValue);
			unsigned long ulObjID = 0;
			CheckXmlString(strValue);
			if (xmlPacket.Get(KEY_OBJ_ID,ulObjID))
			{
// 				XAutoLock l(m_csVectorWBAttribute);
// 				MapWBAttributes* pMapWbAttribute = NULL;
// 				for (int i = 0;i<m_iNumOfStatus;i++)
// 				{
// 					pMapWbAttribute = &m_vectorWbStatus[i];
// 					if (pMapWbAttribute)
// 					{
// 						IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
// 						if(it != pMapWbAttribute->end())
// 						{
// 							it->second.SetString(strValue);
// 						}
// 					}
// 				}
				RelateAttributeToStream(iSessionID,ulObjID,false);
			}
		}
	}
}

void CWBDataModel::OnPassiveStartStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		int iCmdType = *((int*)pData);
		if (iCmdType == CMD_PASSIVE_START)
		{
			char* pBuff = pData + sizeof(int);
// 			unsigned long ulObjID = *((unsigned long*)pBuff);
// 			pBuff += sizeof(unsigned long);
// 			unsigned long ulFileSize = *((unsigned long*)pBuff);
// 			pBuff += sizeof(unsigned long);
// 			string strFileType = pBuff;
			
//**************************************huanglongquan add by 2012年5月15日********************************/
			string strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.ResetString(strValue);
			unsigned long ulObjID = 0;
			unsigned long ulEventID = 0;
			unsigned long ulFileSize = 0;
			string strFileType = "";
			xmlPacket.Get(KEY_OBJ_ID,ulObjID);
			xmlPacket.Get(KEY_EVENT_ID,ulEventID);
			xmlPacket.Get(KEY_FILE_SIZE,ulFileSize);
			xmlPacket.Get(KEY_FILE_TYPE,strFileType);
//**************************************huanglongquan add by 2012年5月15日********************************/
			{
				XAutoLock l(m_csVectorWBAttribute);
				char pPath[MAX_PATH] = {0};
				unsigned long ulFileID = 0;
				if ((!xmlPacket.Get(KEY_FILE_NAME,ulFileID))&& (ulFileID == 0))
				{
					ulFileID = GenerateSSRC();
					xmlPacket.Set(KEY_FILE_NAME,ulFileID);
				}
#ifdef WIN32
				sprintf(pPath,"%s\\%u.",GetFileTemPath(),ulFileID);	
#else
				sprintf(pPath,"%s/%u.",GetFileTemPath(),ulFileID);	
#endif
				string strPath = pPath;
				strPath += strFileType;
				xmlPacket.Set(KEY_FILE_PATH,strPath);
				xmlPacket.Set(KEY_FILE_POS,0);
				xmlPacket.Set(KEY_FILE_SENDER,iSessionID);
				MapWBAttributes* pWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
				if (pWbAttribute)
				{
					IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
					if (it == pWbAttribute->end())
					{
						pWbAttribute->insert(MapWBAttributes::value_type(ulObjID,xmlPacket));

// 						iCmdType  = CMD_REQUEST_ALL_ATTRIBUTE;
// 												iCmdType  = CMD_CHANGE_ATTRIBUTE;
// 												string strValue = "";
// 												xmlPacket.GetString(strValue);
// 												int iLength = sizeof(int) + sizeof(unsigned long) + strValue.size();
// 												pBuff = new char[iLength + 1];
// 												if (pBuff)
// 												{
// 													memset(pBuff,0,iLength + 1);
// 													memcpy(pBuff,(char*)&iCmdType,sizeof(int));
// 													memcpy(pBuff + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
// 													memcpy(pBuff + sizeof(int) + sizeof(unsigned long),strValue.c_str(),strValue.size());
// 													pBuff[iLength] = '\0';
// 													m_pIHPDTSNotify->onDTSUIDataNotify(pBuff,iLength);
// 													delete pBuff;
// 													pBuff = NULL;
// 												}
					}
					else
					{
						unsigned long ulTempFileSize = 0;
						unsigned long ulTempFilePos = 0;
						it->second.Get(KEY_FILE_PATH,strPath);
						it->second.Get(KEY_FILE_POS,ulTempFilePos);
						it->second.Get(KEY_FILE_SIZE,ulTempFileSize);
						if (ulTempFileSize == ulFileSize &&  ulTempFilePos == ulFileSize)
						{
							EndStream(iSessionID,ulObjID);
						}
					}
				}
			}

		}
	}
	else                  //用于多级mcu文件同步
	{
//**************************************huanglongquan add by 2012年5月14日********************************/
		int iCmdType = *((int*)pData);
		if (iCmdType == CMD_PASSIVE_START)
		{
			char* pBuff = pData + sizeof(int);
 			string strValue = pBuff;
 			XMemIni xmlPacket;
 			xmlPacket.ResetString(strValue);
 			unsigned long ulObjID = 0;
 			unsigned long ulEventID = 0;
 			unsigned long ulFileSize = 0;
 			string strFileType = "";
 			xmlPacket.Get(KEY_OBJ_ID,ulObjID);
 			xmlPacket.Get(KEY_EVENT_ID,ulEventID);
 			xmlPacket.Get(KEY_FILE_SIZE,ulFileSize);
 			xmlPacket.Get(KEY_FILE_TYPE,strFileType);
 
 			XAutoLock l(m_csVectorWBAttribute);
 			char pPath[MAX_PATH] = {0};
			unsigned long ulFileID = 0;
			if ((!xmlPacket.Get(KEY_FILE_NAME,ulFileID))&& (ulFileID == 0))
			{
				ulFileID = GenerateSSRC();
				xmlPacket.Set(KEY_FILE_NAME,ulFileID);
			}
 #ifdef WIN32
 			sprintf(pPath,"%s\\%u.",GetFileTemPath(),ulFileID);	
 #else
 			sprintf(pPath,"%s/%u.",GetFileTemPath(),ulFileID);	
 #endif
 			string strPath = pPath;
 			strPath += strFileType;
 			xmlPacket.Set(KEY_FILE_PATH,strPath);
 			xmlPacket.Set(KEY_FILE_POS,0);
 			xmlPacket.Set(KEY_FILE_SENDER,iSessionID);
			if (NeedToSlide(ulEventID))
			{
				SlideNextStatus();
			}
			MapWBAttributes* pMapWbAttribute = NULL;
			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
			if (it == pMapWbAttribute->end())
			{
				pMapWbAttribute->insert(MapWBAttributes::value_type(ulObjID,xmlPacket));
			}
			else
			{
				xmlPacket.GetString(strValue);
				it->second.ResetString(strValue);
			}
			for (int i = 0;i<m_iNumOfStatus;i++)
			{
				MapWBAttributes* pWbAttribute = &m_vectorWbStatus[i];
				if (pWbAttribute)
				{
					IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
					if (it != pWbAttribute->end())
					{
						it->second.Set(KEY_FILE_PATH,strPath);
					}
				}
			}
			//触发被动接受数据流
		//	StartPassiveStream(iSessionID,ulObjID,pData,iLen);
			StartPassiveStream(iSessionID,ulObjID,xmlPacket);

// 			//请求数据包
// 			AskForStream(iSessionID,ulObjID);
//**************************************huanglongquan add by 2012年5月14日********************************/
		}
	}
}
void CWBDataModel::OnReStartStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		int iCmdType = *((int*)pData);
		if (iCmdType == CMD_RESTART_STREAM)
		{
			char *pBuff = pData + sizeof(int);
			std::string strValue = "";
			strValue = pBuff;
			if (strValue != "")
			{
				XMemIni xmlPacket;
				CheckXmlString(strValue);
				xmlPacket.ResetString(strValue);
				unsigned long ulObjID = 0;
				if (xmlPacket.Get(KEY_OBJ_ID, ulObjID))
				{
					XAutoLock l (m_csVectorWBAttribute);
					MapWBAttributes* pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
					if (NULL != pMapWbAttribute)
					{
						IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
						if (it != pMapWbAttribute->end())
						{
							it->second.SetString(strValue);
							std::string strPath = ""   ;
							if (it->second.Get(KEY_FILE_PATH,strPath) )
							{
								if ( strPath != "")
								{
									bool bExistFile = false;
									if(! rename(strPath.c_str(),strPath.c_str()))			 //c++中没有直接判断文件是否存在，但是可以用此函数进行判断，返回值为0 表示文件重命名成功，表示有该文件存在
										bExistFile = true;
									// 								if (access(strPath.c_str(),F_OK) == 0)
									// 									bExistFile = true;
									if(bExistFile)
									{
										int iCmd = CMD_ADD_ATTRIBUTE;
										string strTemp = "";
										it->second.GetString(strTemp);
										int iLength = sizeof(int) + strTemp.size();
										char* pBuff = new char[iLength +1];
										if (pBuff)
										{
											memset(pBuff,0,iLength + 1);
											memcpy(pBuff,(char*)&iCmd,sizeof(int));
											memcpy(pBuff + sizeof(int),strValue.c_str(),strTemp.size());
											pBuff[iLength] = '\0';
											m_pIHPDTSNotify->onDTSUIDataNotify(pBuff,iLength + 1);
											delete pBuff;
											pBuff = NULL;
										}
									}
									else  //本地不存在这个文件 就向服务器请求
									{
										//请求数据包
										unsigned long ulPos = 0;
										it->second.Set(KEY_FILE_POS,ulPos);
										char pPath[MAX_PATH] = {0};
										unsigned long ulFileID = 0;
										ulFileID = GenerateSSRC();
#ifdef WIN32
										sprintf(pPath,"%s\\%u.",GetFileTemPath(),ulFileID);	
#else
										sprintf(pPath,"%s/%u.",GetFileTemPath(),ulFileID);	
#endif
										strPath = pPath;
										it->second.Set(KEY_FILE_PATH,strPath);	
										AskForStream(iSessionID,ulObjID);
									}
								}
								else		 //文件路径不对	  string = “”
								{
								}
							}
							else   //本地保存没有路径
							{

							}
						}	 //本地端找到这个对象记录
					}		//储存指针不为空
				}		//得到对象id号
			}	 //strvalue不为空
		}	 //判断命令类型
	}
	else		 //服务器端转发这个消息
	{
// 		int iCmdType = *((int*)pData);
// 		if (iCmdType == CMD_RESTART_STREAM)
// 		{
// 			char* pBuff = pData + sizeof(int);
// 			string strValue = pBuff;
// 			XMemIni xmlPacket;
// 			xmlPacket.ResetString(strValue);
// 			unsigned long ulObjID = 0;
// 			unsigned long ulEventID = 0;
// 			unsigned long ulFileSize = 0;
// 			string strFileType = "";
// 			xmlPacket.Get(KEY_OBJ_ID,ulObjID);
// 			xmlPacket.Get(KEY_EVENT_ID,ulEventID);
// 			xmlPacket.Get(KEY_FILE_SIZE,ulFileSize);
// 			xmlPacket.Get(KEY_FILE_TYPE,strFileType);
// 
// 			XAutoLock l(m_csVectorWBAttribute);
// 			char pPath[MAX_PATH] = {0};
// 			unsigned long ulFileID = 0;
// 			if ((!xmlPacket.Get(KEY_FILE_NAME,ulFileID))&& (ulFileID == 0))
// 			{
// 				ulFileID = GenerateSSRC();
// 				xmlPacket.Set(KEY_FILE_NAME,ulFileID);
// 			}
// 			//			unsigned long ulFileID = GenerateSSRC();
// #ifdef WIN32
// 			sprintf(pPath,"%s\\%u.",GetFileTemPath(),ulFileID);	
// #else
// 			sprintf(pPath,"%s/%u.",GetFileTemPath(),ulFileID);	
// #endif
// 			string strPath = pPath;
// 			strPath += strFileType;
// 			xmlPacket.Set(KEY_FILE_PATH,strPath);
// 			xmlPacket.Set(KEY_FILE_POS,0);
// 			xmlPacket.Set(KEY_FILE_SENDER,iSessionID);
// 			if (NeedToSlide(ulEventID))
// 			{
// 				SlideNextStatus();
// 			}
// 			MapWBAttributes* pMapWbAttribute = NULL;
// 			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
// 			IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
// 			if (it == pMapWbAttribute->end())
// 			{
// 				pMapWbAttribute->insert(MapWBAttributes::value_type(ulObjID,xmlPacket));
// 			}
// 			else
// 			{
// 				xmlPacket.GetString(strValue);
// 				it->second.ResetString(strValue);
// 			}
// 			for (int i = 0;i<m_iNumOfStatus;i++)			 
// 			{
// 				MapWBAttributes* pWbAttribute = &m_vectorWbStatus[i];
// 				if (pWbAttribute)
// 				{
// 					IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
// 					if (it != pWbAttribute->end())
// 					{
// 						it->second.Set(KEY_FILE_PATH,strPath);
// 					}
// 				}
// 			}
// 			//触发被动接受数据流
// 			//			StartPassiveStream(iSessionID,ulObjID,ulFileSize,strFileType);
// 			StartPassiveStream(iSessionID,ulObjID,xmlPacket);
// 			//请求数据包
// 			AskForStream(iSessionID,ulObjID);
// 		}
	}
}



			// 		char* pBuff = pData + sizeof(int);
			// 		unsigned long ulObjID = *((unsigned long*)pBuff);
			// 		unsigned long ulEventID = 0;
			// 		int iFileSize = 0;
			// 		string strFileType = "";
			// 		{
			// 			XAutoLock l(m_csVectorWBAttribute);
			// 			MapWBAttributes* pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			// 			if (pMapWbAttribute)
			// 			{
			// 				IT_MapWBAttributes it  = pMapWbAttribute->find(ulObjID);
			// 				if (it != pMapWbAttribute->end())
			// 				{
			// 					XMemIni xmlPacket = it->second;
			// 					if (xmlPacket.Get(KEY_EVENT_ID,ulEventID)
			// 					    && xmlPacket.Get(KEY_FILE_SIZE,iFileSize)
			// 					    && xmlPacket.Get(KEY_FILE_TYPE,strFileType))
			// 					{
			// 						StartStream(ulObjID,ulEventID,iFileSize,strFileType,iSessionID);
			// 						//
			// 						int iCmd = CMD_ADD_ATTRIBUTE;
			// 						string strValue = "";
			// 						xmlPacket.GetString(strValue);
			// 						int iLength = sizeof(int) + strValue.size();
			// 						char* pBuff = new char[iLength +1];
			// 						if (pBuff)
			// 						{
			// 							memset(pBuff,0,iLength + 1);
			// 							memcpy(pBuff,(char*)&iCmd,sizeof(int));
			// 							memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
			// 							pBuff[iLength] = '\0';
			// 							m_pIHPDTSNotify->onDTSUIDataNotify(pBuff,iLength + 1);
			// 							delete pBuff;
			// 							pBuff = NULL;
			// 						}
			// 					}
			// 				}
			// 			}
			// 		}

void CWBDataModel::OnStartStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		int iCmdType = *((int*)pData);
		if (iCmdType == CMD_START_STREAM)
		{
// 			char* pBuff = pData + sizeof(int);
// 			unsigned long ulObjID = *((unsigned long*)pBuff);
// 			pBuff += sizeof(unsigned long);
// 			unsigned long ulFileSize = *((unsigned long*)pBuff);
// 			pBuff += sizeof(unsigned long);
// 			unsigned long ulEventID = *((unsigned long*)pBuff);
// 			pBuff += sizeof(unsigned long);
// 			string strFileType = pBuff;
//**************************************huanglongquan add by 2012年5月14日********************************/
			char* pBuff = pData + sizeof(int);
			string strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.ResetString(strValue);
			unsigned long ulObjID = 0;
			unsigned long ulEventID = 0;
			unsigned long ulFileSize = 0;
			string strFileType = "";
			xmlPacket.Get(KEY_OBJ_ID,ulObjID);
			xmlPacket.Get(KEY_EVENT_ID,ulEventID);
			xmlPacket.Get(KEY_FILE_SIZE,ulFileSize);
			xmlPacket.Get(KEY_FILE_TYPE,strFileType);
//**************************************huanglongquan add by 2012年5月14日********************************/
			{
				XAutoLock l(m_csVectorWBAttribute);
				string strPath = "";
				MapWBAttributes* pMapWbAttribute = NULL;
				pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
				if (pMapWbAttribute)
				{
					IT_MapWBAttributes it  = pMapWbAttribute->find(ulObjID);
					if (it == pMapWbAttribute->end())
					{
						char pPath[MAX_PATH] = {0};
						unsigned long ulFileID = 0;
						if ((!xmlPacket.Get(KEY_FILE_NAME,ulFileID))&& (ulFileID == 0))
						{
							 ulFileID = GenerateSSRC();
							 xmlPacket.Set(KEY_FILE_NAME,ulFileID);
						}
						
#ifdef WIN32
						sprintf(pPath,"%s\\%u.",GetFileTemPath(),ulFileID);	
#else
						sprintf(pPath,"%s/%u.",GetFileTemPath(),ulFileID);	
#endif
						strPath = pPath;
						strPath += strFileType;
						xmlPacket.Set(KEY_OBJ_ID,ulObjID);
						xmlPacket.Set(KEY_FILE_POS,0);
 						xmlPacket.Set(KEY_FILE_SENDER,iSessionID);
						xmlPacket.Set(KEY_FILE_PATH,strPath);						
						pMapWbAttribute->insert(MapWBAttributes::value_type(ulObjID,xmlPacket));
						AskForStream(iSessionID,ulObjID);
					}
					else
					{
						unsigned long ulTempFileSize = 0;
						unsigned long ulTempFilePos = 0;
						it->second.Get(KEY_FILE_PATH,strPath);
						it->second.Get(KEY_FILE_POS,ulTempFilePos);
						it->second.Get(KEY_FILE_SIZE,ulTempFileSize);
						if (ulTempFileSize == ulFileSize &&  ulTempFilePos == ulFileSize)
						{
							EndStream(iSessionID,ulObjID);
						}
					}
				}
			}
			
		}
	}
	else
	{
		int iCmdType = *((int*)pData);
		if (iCmdType == CMD_START_STREAM)
		{
/*			printf("m_bMCUExistObj = %d\n",m_bMCUExistObj);*/
			if ( !m_bMCUExistObj )
			{
				m_bMCUExistObj = true;
			}
			char* pBuff = pData + sizeof(int);
			string strValue = pBuff;
			XMemIni xmlPacket;
			xmlPacket.ResetString(strValue);
			unsigned long ulObjID = 0;
			unsigned long ulEventID = 0;
			unsigned long ulFileSize = 0;
			string strFileType = "";
			xmlPacket.Get(KEY_OBJ_ID,ulObjID);
			xmlPacket.Get(KEY_EVENT_ID,ulEventID);
			xmlPacket.Get(KEY_FILE_SIZE,ulFileSize);
			xmlPacket.Get(KEY_FILE_TYPE,strFileType);

			XAutoLock l(m_csVectorWBAttribute);
			char pPath[MAX_PATH] = {0};
			unsigned long ulFileID = 0;
			if ((!xmlPacket.Get(KEY_FILE_NAME,ulFileID))&& (ulFileID == 0))
			{
				ulFileID = GenerateSSRC();
				xmlPacket.Set(KEY_FILE_NAME,ulFileID);
			}
//			unsigned long ulFileID = GenerateSSRC();
#ifdef WIN32
			sprintf(pPath,"%s\\%u.",GetFileTemPath(),ulFileID);	
#else
			sprintf(pPath,"%s/%u.",GetFileTemPath(),ulFileID);	
#endif
			string strPath = pPath;
			strPath += strFileType;
			xmlPacket.Set(KEY_FILE_PATH,strPath);
			xmlPacket.Set(KEY_FILE_POS,0);
			xmlPacket.Set(KEY_FILE_SENDER,iSessionID);
			if (NeedToSlide(ulEventID))
			{
				SlideNextStatus();
			}
			MapWBAttributes* pMapWbAttribute = NULL;
			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
			if (it == pMapWbAttribute->end())
			{
				pMapWbAttribute->insert(MapWBAttributes::value_type(ulObjID,xmlPacket));
			}
			else
			{
				xmlPacket.GetString(strValue);
				it->second.ResetString(strValue);
			}
			for (int i = 0;i<m_iNumOfStatus;i++)			 
			{
				MapWBAttributes* pWbAttribute = &m_vectorWbStatus[i];
				if (pWbAttribute)
				{
					IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
					if (it != pWbAttribute->end())
					{
						it->second.Set(KEY_FILE_PATH,strPath);
					}
				}
			}
			//触发被动接受数据流
			StartPassiveStream(iSessionID,ulObjID,xmlPacket);
			//请求数据包
			AskForStream(iSessionID,ulObjID);
		}
	}
}
void CWBDataModel::OnAskForStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		//m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
		char* pBuff = NULL;
		pBuff = (char*)pData + sizeof(int);
		unsigned long ulObjID = *((unsigned long*)pBuff);
		pBuff += sizeof(unsigned long);
		unsigned long ulPos = *((unsigned long*)pBuff);
		pBuff += sizeof(unsigned long);
		unsigned long ulSize = *((unsigned long*)pBuff);
		MapWBAttributes* pWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pWbAttribute)
		{
			IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
			if (it != pWbAttribute->end())
			{
				string strPath = "";
				if (it->second.Get(KEY_FILE_PATH,strPath) && strPath != "")
				{
					int iCmd = CMD_SAVE_STREAM;
					FILE* pFile = NULL;
					pFile = fopen(strPath.c_str(),"rb");
					if (pFile)
					{
						int iLength = ulSize + sizeof(int) + sizeof(unsigned long);
						char* pStreamData = new char[iLength];
						if (pStreamData)
						{
							memset(pStreamData,0,iLength);
							memcpy(pStreamData,(char*)&iCmd,sizeof(int));
							memcpy(pStreamData + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
							fseek(pFile,ulPos,SEEK_SET);
							int iFlag = fread(pStreamData + sizeof(int) + sizeof(unsigned long),sizeof(char),ulSize,pFile);
							if (iFlag >= 0)
							{
								//it->second.Set(KEY_FILE_POS,ulSize);
								SendToPoint(iSessionID,pStreamData,iLength-ulSize+iFlag);
							}
							delete []pStreamData;
							pStreamData = NULL;
						}
						fclose(pFile);
					}
				}
			}
		}
	}
	else
	{
		char* pBuff = NULL;
		int iCmdType = *((int*)pData);
		pBuff = pData + sizeof(int);
		unsigned long ulObjID = *((unsigned long*)pBuff);
		pBuff += sizeof(unsigned long);
		unsigned long ulPos = *((unsigned long*)pBuff);
		pBuff += sizeof(unsigned long);
		unsigned long ulSize = *((unsigned long*)pBuff);
		SaveStream(iSessionID,ulObjID,ulPos,ulSize);
	}
}
void CWBDataModel::OnStopStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)//这个消息只有客户端收
	{
		XAutoLock l(m_csVectorWBAttribute);
		MapWBAttributes* pWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pWbAttribute)
		{
			unsigned long ulObjID = 0;
			char* pBuff = pData + sizeof(int);
			if (pBuff)
			{
				ulObjID = *((unsigned long*)pBuff);
				pBuff += sizeof(unsigned long);
				IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
				if (it != pWbAttribute->end())
				{
					pWbAttribute->erase(it);
				}
			}
		}
		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
	}
}
void CWBDataModel::OnPassiveSaveStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		//m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
		char* pBuff = pData + sizeof(int);
		unsigned long ulObjID = *((unsigned long*)pBuff);
		pBuff += sizeof(unsigned long);
		int iBuffSize = iLen - sizeof(int) - sizeof(unsigned long);
		{
			XAutoLock l(m_csVectorWBAttribute);
			MapWBAttributes* pWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			if (pWbAttribute)
			{
				string strPath = "";
				unsigned long ulSize = 0;
				unsigned long ulPos = 0;
				IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
				if (it != pWbAttribute->end() 
					&& it->second.Get(KEY_FILE_PATH,strPath) 
					&& it->second.Get(KEY_FILE_SIZE,ulSize)
					&& it->second.Get(KEY_FILE_POS,ulPos))
				{
#ifdef WIN32
					char pPath[_MAX_PATH] = {0};
					AvconGet_path(strPath.c_str(),pPath);
					string str_file_path = pPath;
					struct stat statbuf;
					if ( stat(str_file_path.c_str(),&statbuf) < 0)  //如果目录不存在就创建目录
					{
						int iPos = 0;
						iPos = str_file_path.find_first_of("\\",iPos);
						while (iPos != -1)
						{
							iPos = str_file_path.find_first_of("\\",iPos+1);
							string str_Path = str_file_path.substr(0,iPos);
							_mkdir(str_Path.c_str());
						}
					}
#endif			
					FILE* pFile = NULL;
					pFile = fopen(strPath.c_str(),"ab+");

					if (pFile)
					{			
						fseek(pFile,0,SEEK_END);
						fwrite(pBuff,sizeof(char),iBuffSize,pFile);
						fflush(pFile);
						fclose(pFile);
						it->second.Set(KEY_FILE_POS,ulPos + iBuffSize);
// 						int iCmdType = CMD_REQUEST_ALL_ATTRIBUTE;
// 						string strValue = "";
// 						it->second.GetString(strValue);
// 						int iLength = sizeof(int) + sizeof(unsigned long) + strValue.size();
// 						pBuff = new char[iLength + 1];
// 						if (pBuff)
// 						{
// 							memset(pBuff,0,iLength + 1);
// 							memcpy(pBuff,(char*)&iCmdType,sizeof(int));
// 							memcpy(pBuff + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
// 							memcpy(pBuff + sizeof(int) + sizeof(unsigned long) ,strValue.c_str(),strValue.size());
// 							pBuff[iLength] = '\0';
//						    	m_pIHPDTSNotify->onDTSUIDataNotify(pBuff,iLength + 1);
// 							delete pBuff;
// 							pBuff = NULL;
// 						}
					}
				}

			}
		}

	}
	else  //为多级mcu文件同步
	{
//**************************************huanglongquan add by 2012年5月14日********************************/	
		//写文件
		char* pBuff = pData;
		int iCmdType = *((int*)pBuff);
		pBuff += sizeof(int);
		unsigned long ulObjID = *((unsigned long*)pBuff);
		pBuff += sizeof(unsigned long);
		int iBuffSize = iLen - sizeof(int) - sizeof(unsigned long);
		unsigned long ulSize = 0;
		unsigned long ulPos = 0;
		string strPath = "";
		MapWBAttributes* pMapWbAttribute = NULL;
		for (int i = 0;i < m_iNumOfStatus;i++)
		{
			pMapWbAttribute = &m_vectorWbStatus[i];
			if (pMapWbAttribute)
			{
				IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				if (it != pMapWbAttribute->end() 
					&& it->second.Get(KEY_FILE_PATH,strPath) 
					&& it->second.Get(KEY_FILE_SIZE,ulSize)
					&& it->second.Get(KEY_FILE_POS,ulPos))
				{
					FILE* pFile = NULL;
					pFile = fopen(strPath.c_str(),"ab+");

					if (pFile)
					{	
						fseek(pFile,0,SEEK_END);
						fwrite(pBuff,sizeof(char),iBuffSize,pFile);
						fflush(pFile);
						fclose(pFile);
						{
							XAutoLock l(m_csVectorWBAttribute);
							for (int i = 0;i<m_iNumOfStatus;i++)
							{
								IT_MapWBAttributes itTemp = m_vectorWbStatus[i].find(ulObjID);
								if (itTemp != m_vectorWbStatus[i].end())
								{
									itTemp->second.Set(KEY_FILE_POS,ulPos + iBuffSize);
								}
							}
						}
						if (ulSize == (ulPos + iBuffSize))
						{
							SavePassiveStream(iSessionID,ulObjID,pBuff,iBuffSize);
//							EndStream(iSessionID,ulObjID);
						}
						else if (ulSize > (ulPos + iBuffSize))
						{
							SavePassiveStream(iSessionID,ulObjID,pBuff,iBuffSize);
//							AskForStream(iSessionID,ulObjID);
						}
						else if (ulSize < (ulPos + iBuffSize))
						{
							UnregistPassiveObjID(ulObjID);
							ErrorPassiveStream(iSessionID,ulObjID);
							ErrorStream(iSessionID,ulObjID);	
						}
					}
					break;
				}
				else
				{
					//调试错误信息
				}
			}
		}
//**************************************huanglongquan add by 2012年5月14日********************************/
	}
}

void CWBDataModel::OnSaveStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		char* pBuff = pData;
		int iCmdType = *((int*)pBuff);
		pBuff += sizeof(int);
		unsigned long ulObjID = *((unsigned long*)pBuff);
		pBuff += sizeof(unsigned long);
		int iBuffSize = iLen - sizeof(int) - sizeof(unsigned long);
		MapWBAttributes* pWbAttribute = NULL;
		string strValue = "";
		pWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pWbAttribute)
		{
			IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
			if (it != pWbAttribute->end())
			{
				unsigned long ulFilePos = 0;
				unsigned long ulFileSize = 0;
				string strPath = "";
				if (it->second.Get(KEY_FILE_POS,ulFilePos) 
					&& it->second.Get(KEY_FILE_SIZE,ulFileSize)
					&& it->second.Get(KEY_FILE_PATH,strPath))
				{
#ifdef WIN32
					char pPath[_MAX_PATH] = {0};
					AvconGet_path(strPath.c_str(),pPath);
					string str_file_path = pPath;
					struct stat statbuf;
					if ( stat(str_file_path.c_str(),&statbuf) < 0)  //如果目录不存在就创建目录
					{
						int iPos = 0;
						iPos = str_file_path.find_first_of("\\",iPos);
						while (iPos != -1)
						{
							iPos = str_file_path.find_first_of("\\",iPos+1);
							string str_Path = str_file_path.substr(0,iPos);
							_mkdir(str_Path.c_str());
						}
					}
#endif
					FILE* pFile = NULL;
					pFile = fopen(strPath.c_str(),"ab+");
					if (pFile)
					{			
						fseek(pFile,0,SEEK_END);
						fwrite(pBuff,sizeof(char),iBuffSize,pFile);
						fflush(pFile);
						fclose(pFile);
					}

					if (ulFileSize == (ulFilePos + iBuffSize))
					{
						it->second.Set(KEY_FILE_POS,ulFilePos + iBuffSize);

						/*iCmdType = CMD_REQUEST_ALL_ATTRIBUTE;*/

// 						iCmdType = CMD_CHANGE_ATTRIBUTE;
// 						it->second.GetString(strValue);
// 						int iLength = sizeof(int) + sizeof(unsigned long) +strValue.size();
// 						pBuff = new char[iLength + 1];
// 						if (pBuff)
// 						{
// 							memset(pBuff,0,iLength + 1);
// 							memcpy(pBuff,(char*)&iCmdType,sizeof(int));
// 							memcpy(pBuff + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
// 							memcpy(pBuff + sizeof(int) + sizeof(unsigned long),strValue.c_str(),strValue.size());
// 							pBuff[iLength] = '\0';
// 							m_pIHPDTSNotify->onDTSUIDataNotify(pBuff,iLength + 1);
// 							delete pBuff;
// 							pBuff = NULL;
// 						}
						EndStream(iSessionID,ulObjID);
					}
					else if(ulFileSize > (ulFilePos + iBuffSize))
					{
						it->second.Set(KEY_FILE_POS,ulFilePos + iBuffSize);

// 						iCmdType = CMD_REQUEST_ALL_ATTRIBUTE;
// 						it->second.GetString(strValue);
// 						int iLength = sizeof(int) + sizeof(unsigned long) + strValue.size();
// 						pBuff = new char[iLength + 1];
// 						if (pBuff)
// 						{
// 							memset(pBuff,0,iLength + 1);
// 							memcpy(pBuff,(char*)&iCmdType,sizeof(int));
// 							memcpy(pBuff + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
// 							memcpy(pBuff + sizeof(int) + sizeof(unsigned long),strValue.c_str(),strValue.size());
// 							pBuff[iLength] = '\0';
// 							m_pIHPDTSNotify->onDTSUIDataNotify(pBuff,iLength + 1);
// 							delete pBuff;
// 							pBuff = NULL;
// 
// 						}
						AskForStream(iSessionID,ulObjID);
					}
					else if (ulFileSize < (ulFilePos + iBuffSize))
					{
						ErrorStream(iSessionID,ulObjID);
					}
				}
				
			}
		}
	}
	else
	{
		//写文件
		char* pBuff = pData;
		int iCmdType = *((int*)pBuff);
		pBuff += sizeof(int);
		unsigned long ulObjID = *((unsigned long*)pBuff);
		pBuff += sizeof(unsigned long);
		int iBuffSize = iLen - sizeof(int) - sizeof(unsigned long);
		unsigned long ulSize = 0;
		unsigned long ulPos = 0;
		string strPath = "";
		MapWBAttributes* pMapWbAttribute = NULL;
		for (int i = 0;i < m_iNumOfStatus;i++)
		{
			pMapWbAttribute = &m_vectorWbStatus[i];
			if (pMapWbAttribute)
			{
				IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				if (it != pMapWbAttribute->end() 
					&& it->second.Get(KEY_FILE_PATH,strPath) 
					&& it->second.Get(KEY_FILE_SIZE,ulSize)
					&& it->second.Get(KEY_FILE_POS,ulPos))
				{
					FILE* pFile = NULL;
					pFile = fopen(strPath.c_str(),"ab+");

					if (pFile)
					{			
						fseek(pFile,0,SEEK_END);
						fwrite(pBuff,sizeof(char),iBuffSize,pFile);
						fflush(pFile);
						fclose(pFile);
						{
							XAutoLock l(m_csVectorWBAttribute);
							for (int i = 0;i<m_iNumOfStatus;i++)				  //这里这样做的目的是防止文件还没传完就操作白板引起文件传输错误
							{
								IT_MapWBAttributes itTemp = m_vectorWbStatus[i].find(ulObjID);
								if (itTemp != m_vectorWbStatus[i].end())
								{
									itTemp->second.Set(KEY_FILE_POS,ulPos + iBuffSize);
								}
							}
						}
						if (ulSize == (ulPos + iBuffSize))
						{
							SavePassiveStream(iSessionID,ulObjID,pBuff,iBuffSize);
							EndStream(iSessionID,ulObjID);
						}
						else if (ulSize > (ulPos + iBuffSize))
						{
							SavePassiveStream(iSessionID,ulObjID,pBuff,iBuffSize);
							AskForStream(iSessionID,ulObjID);
						}
						else if (ulSize < (ulPos + iBuffSize))
						{
							UnregistPassiveObjID(ulObjID);
							ErrorPassiveStream(iSessionID,ulObjID);
							ErrorStream(iSessionID,ulObjID);	
						}
					}
					break;
				}
			}
		}
	}
}
void CWBDataModel::OnPassiveEndStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
	}
	else				//多级mcu文件同步
	{
//**************************************huanglongquan add by 2012年5月14日********************************/
		char* pBuff = pData;
		pBuff += sizeof(int);
		unsigned long ulObjID = *((unsigned long*)pBuff);
		{
			XAutoLock l(m_csVectorWBAttribute);
			RelateAttributeToStream(iSessionID,ulObjID);
		}
//**************************************huanglongquan add by 2012年5月14日********************************/
	}
}
void CWBDataModel::OnEndStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		char* pBuff = pData;
		int iCmdType = *((int*)pBuff);
		pBuff += sizeof(int);
		unsigned long ulObjID = *((unsigned long*)pBuff);
		{
			XAutoLock l(m_csVectorWBAttribute);
			MapWBAttributes* pMapWbAttribute = NULL;
			pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
			if (pMapWbAttribute)
			{
				IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				if (it != pMapWbAttribute->end())
				{
					RelateAttributeToStream(iSessionID,ulObjID);
				}
			}		
		}
	}
	else
	{
		char* pBuff = pData;
		pBuff += sizeof(int);
		unsigned long ulObjID = *((unsigned long*)pBuff);
		{
			XAutoLock l(m_csVectorWBAttribute);
			RelateAttributeToStream(iSessionID,ulObjID);
		}
	}
}
void CWBDataModel::OnPassiveErrorStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
	}
}


void CWBDataModel::OnAskForUserColor( unsigned long iSessionID,const char* pData,int iLen )
{
//	TRACE("void CWBDataModel::OnAskForUserColor <--net \n");
	if (pData == NULL || iLen < 0 || *(pData + iLen -1) != '\0')
	{
		return;
	}
	string str_name = pData + sizeof(int);
	XAutoLock l(m_csMapUserColorInfo);
	IT_MapUserColorInfo itInfo = m_mapUserColorInfo.find(str_name);
	unsigned int neUserColorWB = Default_Color;
	if (itInfo != m_mapUserColorInfo.end())
	{
		neUserColorWB = itInfo->second;
//		TRACE("用户曾经进入该会议 \n neUserColorWB = %d, itInfo->second = %d \n",neUserColorWB,itInfo->second);
	}
	else
	{
		bool bRemainColor = false;
		XAutoLock lrgb(m_csMapRGBInfo);
		IT_MapCorrespondingRGB it = m_mapCorrespondingRGB.begin();
		while(it != m_mapCorrespondingRGB.end() )
		{
//			TRACE("进入while循环 it->first = %d\n",it->first);
			 if (it->second.bOccupy)
			 {
				 it++;
			 }
			 else
			 {
				bRemainColor = true;	
/*				TRACE("跳出循环 it->first = %d\n",it->first);*/
				break;
			 }
		}//while end

		if (bRemainColor)
		{
			neUserColorWB = it->first;
			it->second.bOccupy = true;
			m_mapUserColorInfo.insert(MapUserColorInfo::value_type(str_name,neUserColorWB));
/*			TRACE("从颜色map中分配一种颜色 it->first = %d, neUserColorWB = %d\n",it->first,neUserColorWB);*/
		}
		else
		{
/*			TRACE("未分配到颜色用默认颜色\n",it->first,neUserColorWB);*/
			neUserColorWB = Default_Color;
		}
	}
	int iCmd = CMD_RELATIVE_USERCOLOR;
	int nLen = sizeof(int) + sizeof(unsigned int) + str_name.length();
	char* pBuff = new char[nLen +1];
	if (pBuff)
	{
		memset(pBuff,0,nLen+1);
		memcpy(pBuff,(char*)&iCmd,sizeof(int));
		memcpy(pBuff + sizeof(int),(char*)&neUserColorWB,sizeof(unsigned int));
		memcpy(pBuff + sizeof(int) + sizeof(unsigned int),str_name.c_str(),str_name.length());
		pBuff[nLen] = '\0';
		SendToPoint(iSessionID,pBuff,nLen +1); 

 		memset(pBuff,0,sizeof(int));  //通知其他的服务器
 		iCmd = CMD_BROADCAST_USERNAMECOLOR;
 		memcpy(pBuff,(char*)&iCmd,sizeof(int));
 		SendToOthers(iSessionID,pBuff,nLen+1);
	}
	delete[] pBuff;
	pBuff = NULL;
}

void CWBDataModel::OnRelativeUserColor( unsigned long iSessionID,char* pData,int iLen )
{
 	if (pData == NULL || iLen <0 || *(pData + iLen -1) != '\0')
 	{
 		return;
 	}
	if (m_pIHPDTSNotify&& m_bUI)
	{
		int iCmdType = *((int*)pData);
		char* pBuff = pData + sizeof(int);
		unsigned int unUserColor = *((unsigned int*)pBuff);
		pBuff += sizeof(unsigned int);
		string strUserName = pBuff;
		unsigned int nRed = 0;
		unsigned int nGreen = 0;
		unsigned int nBlue = 0;
		GetUserColor((eUserColorWB)unUserColor,nRed,nGreen,nBlue);
		int nLen = sizeof(int) + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int) + strUserName.length();
		char* pBuffTemp = new char[nLen +1];
		if (pBuffTemp)
		{
			memset(pBuffTemp,0,nLen);
 			memcpy(pBuffTemp,(char*)&iCmdType,sizeof(int));
 			memcpy(pBuffTemp + sizeof(int),(char*)&nRed,sizeof(unsigned int));
 			memcpy(pBuffTemp + sizeof(int) + sizeof(unsigned int), (char*)&nGreen,sizeof(unsigned int));
 			memcpy(pBuffTemp + sizeof(int) + sizeof(unsigned int) + sizeof(unsigned int),(char*)&nBlue,sizeof(unsigned int));
			memcpy(pBuffTemp + sizeof(int) + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int),strUserName.c_str(),strUserName.length()); 
			pBuffTemp[nLen] ='\0';
			m_pIHPDTSNotify->onDTSUIDataNotify(pBuffTemp,nLen + 1);
 			delete[] pBuffTemp;
 			pBuffTemp = NULL;
		}
	}
}


void CWBDataModel::OnErrorStream(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		m_pIHPDTSNotify->onDTSUIDataNotify(pData,iLen);
	}
}
void CWBDataModel::RelateAttributeToStream(unsigned int iSessionID,unsigned long ulObjID,bool bToPoint)
{
	if (m_bUI)
	{
		MapWBAttributes* pWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pWbAttribute)
		{
			IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
			if (it != pWbAttribute->end())
			{
				XMemIni xmlPacket = it->second;
				CheckXmlPacket(xmlPacket);
				xmlPacket.DeleteKey(KEY_EVENT_ID);
				string strXml = "";
				xmlPacket.GetString(strXml);
				if (strXml.size()>0)
				{
					int iCmd = CMD_RELATE_STREAM;
					int iLen = sizeof(int) + strXml.size();
					char* pData = new char[iLen+1];
					if (pData)
					{
						memset(pData,0,iLen+1);
						memcpy(pData,(char*)&iCmd,sizeof(int));
						memcpy(pData + sizeof(int),strXml.c_str(),strXml.size());
						pData[iLen] = '\0';
						SendToPoint(iSessionID,pData,iLen+1);
						delete pData;
						pData = NULL;
					}
				}
			}
		}
		
	}
	else
	{
		MapWBAttributes* pMapWbAttribute = NULL;
		pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pMapWbAttribute)
		{
			IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
			if (it != pMapWbAttribute->end())
			{
				XMemIni xmlPacket = it->second;
//				CheckXmlPacket(xmlPacket);
				string strXml = "";
				xmlPacket.GetString(strXml);
				if (strXml.size()>0)
				{
					int iCmd = CMD_ADD_ATTRIBUTE;//CMD_RELATE_STREAM;
					int iLen = sizeof(int) + strXml.size();
					char* pData = new char[iLen+1];
					if (pData)
					{
						memset(pData,0,iLen+1);
						memcpy(pData,(char*)&iCmd,sizeof(int));
						memcpy(pData + sizeof(int),strXml.c_str(),strXml.size());
						pData[iLen] = '\0';

						if (bToPoint == false)
						{ 
							SendToOthers(iSessionID,pData,iLen+1,ulObjID);
							UnregistPassiveObjID(ulObjID);
						}
						else
						{
							SendToPoint(iSessionID,pData,iLen+1);
						}
						delete pData;
						pData = NULL;
					}
				}
			}
		}	
	}
}
void CWBDataModel::ErrorPassiveStream(unsigned int iSessionID,unsigned long ulObjID)
{
	int iCmdType = CMD_PASSIVE_ERROR;
	int iLength = sizeof(int) + sizeof(unsigned long);
	char* pData = new char[iLength];
	if (pData)
	{
		memset(pData,0,iLength);
		memcpy(pData,(char*)&iCmdType,sizeof(int));
		memcpy(pData + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
		SendToOthers(iSessionID,pData,iLength);
		delete []pData;
		pData = NULL;
	}
}
void CWBDataModel::ErrorStream(unsigned int iSessionID,unsigned long ulObjID)
{
	int iCmdType = CMD_ERROR_STREAM;
	int iLength = sizeof(int) + sizeof(unsigned long);
	char* pData = new char[iLength];
	if (pData)
	{
		memset(pData,0,iLength);
		memcpy(pData,(char*)&iCmdType,sizeof(int));
		memcpy(pData + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
		SendToPoint(iSessionID,pData,iLength);
		delete []pData;
		pData = NULL;
	}
}
void CWBDataModel::EndPassiveStream(unsigned int iSessionID,unsigned long ulObjID)
{
	int iCmdType = CMD_PASSIVE_END;
	int iLength = sizeof(int) + sizeof(unsigned long);
	char* pData = new char[iLength];
	if (pData)
	{
		memset(pData,0,iLength);
		memcpy(pData,(char*)&iCmdType,sizeof(int));
		memcpy(pData + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
		SendToOthers(iSessionID,pData,iLength);
		delete []pData;
		pData = NULL;
	}
}
void CWBDataModel::EndStream(unsigned int iSessionID,unsigned long ulObjID)
{
	int iCmdType = CMD_END_STREAM;
	int iLength = sizeof(int) + sizeof(unsigned long);
	char* pData = new char[iLength];
	if (pData)
	{
		memset(pData,0,iLength);
		memcpy(pData,(char*)&iCmdType,sizeof(int));
		memcpy(pData + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
		SendToPoint(iSessionID,pData,iLength);
		delete []pData;
		pData = NULL;
	}
}
//让对方写入流
void CWBDataModel::SavePassiveStream(unsigned int iSessionID,unsigned long ulObjID,char* pData,int iLen)
{
	int iCmdType = CMD_PASSIVE_SAVE;
	int iLength = sizeof(int) + sizeof(unsigned long) + iLen;
	char* pBuff = new char[iLength];
	if (pBuff)
	{
		memset(pBuff,0,iLength);
		memcpy(pBuff,(char*)&iCmdType,sizeof(int));
		memcpy(pBuff + sizeof(int) ,(char*)&ulObjID,sizeof(unsigned long));
		memcpy(pBuff + sizeof(int) + sizeof(unsigned long),pData,iLen);
		SendToOthers(iSessionID,pBuff,iLength,ulObjID);
		delete pBuff;
		pBuff = NULL;
	}
}
bool CWBDataModel::CheckTranslateModel(string strPath,unsigned long ulPos)
{
	if (!m_bUI)
	{
		unsigned long ulFileSize = 0;
		string strFileType = "";
		GetFileStatus(strPath,ulFileSize,strFileType);
		if (ulFileSize == ulPos)
		{
			return true;
		}
	}
	return false;
}
void CWBDataModel::SaveStream(unsigned int iSessionID,unsigned long ulObjID,unsigned long ulPos,unsigned long ulSize)
{
	string strPath = "";
	MapWBAttributes* pMapWbAttribute = NULL;
	pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
	if (pMapWbAttribute)
	{
		IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
		if (it != pMapWbAttribute->end())
		{
			it->second.Get(KEY_FILE_PATH,strPath);
		}
		else
		{
			return;
		}
	}
	if (strPath != "")
	{
		int iCmd = CMD_SAVE_STREAM;
		FILE* pFile = NULL;
		
		//防止出现上传速度过慢，下载速度过快，出现申请的数据，还没传完
		if (CheckTranslateModel(strPath,ulPos))
		{
			RegistPassiveObjID(iSessionID,ulObjID);//下载模式变成被动
			return;
		}

		pFile = fopen(strPath.c_str(),"rb");
		if (pFile)
		{
			int iLength = ulSize + sizeof(int) + sizeof(unsigned long);
			char* pStreamData = new char[iLength];
			if (pStreamData)
			{
				memset(pStreamData,0,iLength);
				memcpy(pStreamData,(char*)&iCmd,sizeof(int));
				memcpy(pStreamData + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
				fseek(pFile,ulPos,SEEK_SET);
				int iFlag = fread(pStreamData + sizeof(int) + sizeof(unsigned long),sizeof(char),ulSize,pFile);
				if (iFlag >= 0)
				{
					SendToPoint(iSessionID,pStreamData,iLength - ulSize + iFlag);
				}
				delete []pStreamData;
				pStreamData = NULL;
			}
			fclose(pFile);
		}
	}
}
//**************************************huanglongquan 注释 by 2012年5月13日********************************/
// void CWBDataModel::StartPassiveStream(unsigned int iSessionID,unsigned long ulObjID,unsigned long ulSize,string strFileType)
// {
// 	unsigned long iBuffLen = strFileType.size() + 2*sizeof(unsigned long) + sizeof(int);
// 	char* pBuff = new char[iBuffLen+1];
// 	memset(pBuff,0,iBuffLen+1);
// 	int iCmd  = CMD_PASSIVE_START;
// 	memcpy(pBuff,(char*)&iCmd,sizeof(int));
// 	memcpy(pBuff + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
// 	memcpy(pBuff + sizeof(int) + sizeof(unsigned long),(char*)&ulSize,sizeof(unsigned long));
// 	memcpy(pBuff + sizeof(int) + 2*sizeof(unsigned long),strFileType.c_str(),strFileType.size());
// 	pBuff[iBuffLen] = '\0';
// 	RegistPassiveObjID(ulObjID);
// 	SendToOthers(iSessionID,pBuff,iBuffLen+1,ulObjID);
// 	delete []pBuff;
// 	pBuff = NULL;
// }
//**************************************huanglongquan 注释 by 2012年5月13日********************************/
//**************************************huanglongquan add by 2012年5月14日********************************/
// void CWBDataModel::StartPassiveStream( unsigned int iSessionID,unsigned long ulObjID,char* pData, int iLen )
// {
// 	unsigned long iBuffLen = iLen;
// 	char *pBuff = new char[iBuffLen +1];
// 	memset(pBuff,0,iBuffLen+1);
// 	int iCmd = CMD_PASSIVE_START;
// 	memcpy(pBuff,(char*)&iCmd,sizeof(int));
// 	memcpy(pBuff + sizeof(int),pData + sizeof(int),iLen - sizeof(int));
// 	pBuff[iBuffLen] = '\0';
// 	RegistPassiveObjID(ulObjID);
// 	SendToOthers(iSessionID,pBuff,iBuffLen,ulObjID);
// 	delete[] pBuff;
// 	pBuff = NULL;
// }

void CWBDataModel::StartPassiveStream( unsigned int iSessionID,unsigned long ulObjID,XMemIni xmlPacket )
{
	string strValue = "";
	xmlPacket.GetString(strValue);
	int iLength = sizeof(int) + strValue.size();
	char* pBuff = new char[iLength + 1];	
	if (pBuff)
	{
		int iCmdType = CMD_PASSIVE_START;
		memset(pBuff,0,iLength+1);
		memcpy(pBuff,(char*)&iCmdType,sizeof(int));
		memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
		pBuff[iLength] = '\0';
		RegistPassiveObjID(ulObjID);
		SendToOthers(iSessionID,pBuff,iLength);
		delete []pBuff;
		pBuff = NULL;
	}
}

//**************************************huanglongquan add by 2012年5月14日********************************/
//重新开始传流
void CWBDataModel::ReStartStream(unsigned long ulObjID,unsigned long ulEventID,unsigned int iSessionID)
{
	for (int i = 0;i< m_iNumOfStatus;i++)
	{
		MapWBAttributes* pWbAttribute = &m_vectorWbStatus[i];
		if (pWbAttribute)
		{
			IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
			if (it != pWbAttribute->end())
			{
				it->second.Set(KEY_FILE_POS,0);
				it->second.Set(KEY_FILE_PATH,"");
			}
		}
	}
	int iCmdType = CMD_RESTART_STREAM;
	int iLength = sizeof(int) + 2*sizeof(unsigned long);
	char* pData = new char[iLength];
	if (pData)
	{
		memset(pData,0,iLength);
		memcpy(pData,(char*)&iCmdType,sizeof(int));
		memcpy(pData + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
		memcpy(pData + sizeof(int) + sizeof(unsigned long),(char*)&ulEventID,sizeof(unsigned long));
		SendToPoint(iSessionID,pData,iLength);
		delete pData;
		pData = NULL;
	}
}

void CWBDataModel::ReStartStream(unsigned long ulObjID,XMemIni xmlobj, unsigned long iSessionID )
{
	for (int i = 0;i< m_iNumOfStatus;i++)
	{
		MapWBAttributes* pWbAttribute = &m_vectorWbStatus[i];
		if (pWbAttribute)
		{
			IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
			if (it != pWbAttribute->end())
			{
				it->second.Set(KEY_FILE_POS,0);
				it->second.Set(KEY_FILE_PATH,"");
			}
		}
	}
	{
		int iCmdType = CMD_RESTART_STREAM;
		int iLength = 0;
		xmlobj.DeleteKey(KEY_FILE_PATH);
		xmlobj.Set(KEY_FILE_POS,0);
		string strValue = "";
		xmlobj.GetString(strValue);
		if (strValue != "")
		{
			iLength = sizeof(int) + strValue.size();
			char* pBuff = new char[iLength + 1];
			if (pBuff)
			{
				memset(pBuff,0,iLength + 1);
				memcpy(pBuff,(char*)&iCmdType,sizeof(int));
				memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
				pBuff[iLength] = '\0';
				if (iSessionID == 0)
				{
					SendToAll(pBuff,iLength + 1);
				}
				else
				{
					SendToPoint(iSessionID,pBuff,iLength + 1);
				}
				delete []pBuff;
				pBuff = NULL;
			}
		}		
	}
	//**************************************huanglongquan add by 2012年5月15日********************************/
}

//开始传流 
void CWBDataModel::StartStream(unsigned long ulObjID,unsigned long ulEventID,unsigned long ulSize,string strFileType,unsigned int iSessionID)
{
	if (m_bUI && m_pIHPDTSNotify)
	{
		int iCmdType = CMD_START_STREAM;
		int iLength = 0;
		string strValue = "";
		MapWBAttributes* pWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		if (pWbAttribute)
		{
			IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
			if (it != pWbAttribute->end())
			{
				XMemIni xmlPacket = it->second;
				xmlPacket.DeleteKey(KEY_FILE_PATH);
				xmlPacket.Set(KEY_FILE_POS,0);
				xmlPacket.GetString(strValue);
				iLength = sizeof(int) + strValue.size();
				char* pBuff = new char[iLength + 1];
				if (pBuff)
				{
					memset(pBuff,0,iLength + 1);
					memcpy(pBuff,(char*)&iCmdType,sizeof(int));
					memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
					pBuff[iLength] = '\0';
					if (iSessionID == 0)
					{
						SendToAll(pBuff,iLength + 1);
					}
					else
					{
						SendToPoint(iSessionID,pBuff,iLength + 1);
					}
					delete []pBuff;
					pBuff = NULL;
				}
			}
		}
		
	}
	else
	{
/*			huanglongquan 注释	 by 2012年5月15日	
 		unsigned long ulBuffLen = strFileType.size() + 3*sizeof(unsigned long) + sizeof(int);
 		char* pBuff = new char[ulBuffLen+1];
 		memset(pBuff,0,sizeof(ulBuffLen+1));
 		int iCmd  = CMD_START_STREAM;
 		memcpy(pBuff,(char*)&iCmd,sizeof(int));
 		memcpy(pBuff + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
 		memcpy(pBuff + sizeof(int) + sizeof(unsigned long),(char*)&ulSize,sizeof(unsigned long));
 		memcpy(pBuff + sizeof(int) + 2*sizeof(unsigned long),(char*)&ulEventID,sizeof(unsigned long));
 		memcpy(pBuff + sizeof(int) + 3*sizeof(unsigned long),strFileType.c_str(),strFileType.size());
 		pBuff[ulBuffLen] = '\0';
 		if (iSessionID == 0)		
 		{
 			SendToAll(pBuff,ulBuffLen+1);
 		}
 		else
 		{
 			SendToPoint(iSessionID,pBuff,ulBuffLen+1);
 		}
 		delete []pBuff;
 		pBuff = NULL;
*/

//**************************************huanglongquan add by 2012年5月15日********************************/
		 int iCmdType = CMD_START_STREAM;
		 int iLength = 0;
		 string strValue = "";
		 MapWBAttributes* pWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
		 if (pWbAttribute)
		 {
			 IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
			 if (it != pWbAttribute->end())
			 {
				 XMemIni xmlPacket = it->second;
				 xmlPacket.DeleteKey(KEY_FILE_PATH);
				 xmlPacket.Set(KEY_FILE_POS,0);
				 xmlPacket.GetString(strValue);
				 iLength = sizeof(int) + strValue.size();
				 char* pBuff = new char[iLength + 1];
				 if (pBuff)
				 {
					 memset(pBuff,0,iLength + 1);
					 memcpy(pBuff,(char*)&iCmdType,sizeof(int));
					 memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
					 pBuff[iLength] = '\0';
					 if (iSessionID == 0)
					 {
						 SendToAll(pBuff,iLength + 1);
					 }
					 else
					 {
						 SendToPoint(iSessionID,pBuff,iLength + 1);
					 }
					 delete []pBuff;
					 pBuff = NULL;
				 }
			 }
		 }
//**************************************huanglongquan add by 2012年5月15日********************************/
	}
}

// void CWBDataModel::StartStream( unsigned long ulObjID,int iTo,unsigned int iSessionID /*= 0*/ )
// {
// 	  	if (m_bUI && m_pIHPDTSNotify)
// 	{
// 		int iCmdType = CMD_START_STREAM;
// 		int iLength = 0;
// 		string strValue = "";
// 		MapWBAttributes* pWbAttribute = &m_vectorWbStatus[iTo];
// 		if (pWbAttribute)
// 		{
// 			IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
// 			if (it != pWbAttribute->end())
// 			{
// 				XMemIni xmlPacket = it->second;
// 				xmlPacket.DeleteKey(KEY_FILE_PATH);
// 				xmlPacket.Set(KEY_FILE_POS,0);
// 				xmlPacket.GetString(strValue);
// 				iLength = sizeof(int) + strValue.size();
// 				char* pBuff = new char[iLength + 1];
// 				if (pBuff)
// 				{
// 					memset(pBuff,0,iLength + 1);
// 					memcpy(pBuff,(char*)&iCmdType,sizeof(int));
// 					memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
// 					pBuff[iLength] = '\0';
// 					if (iSessionID == 0)
// 					{
// 						SendToAll(pBuff,iLength + 1);
// 					}
// 					else
// 					{
// 						SendToPoint(iSessionID,pBuff,iLength + 1);
// 					}
// 					delete []pBuff;
// 					pBuff = NULL;
// 				}
// 			}
// 		}
// 		
// 	}
// 	else
// 	{
// /*			huanglongquan 注释	 by 2012年5月15日	
//  		unsigned long ulBuffLen = strFileType.size() + 3*sizeof(unsigned long) + sizeof(int);
//  		char* pBuff = new char[ulBuffLen+1];
//  		memset(pBuff,0,sizeof(ulBuffLen+1));
//  		int iCmd  = CMD_START_STREAM;
//  		memcpy(pBuff,(char*)&iCmd,sizeof(int));
//  		memcpy(pBuff + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
//  		memcpy(pBuff + sizeof(int) + sizeof(unsigned long),(char*)&ulSize,sizeof(unsigned long));
//  		memcpy(pBuff + sizeof(int) + 2*sizeof(unsigned long),(char*)&ulEventID,sizeof(unsigned long));
//  		memcpy(pBuff + sizeof(int) + 3*sizeof(unsigned long),strFileType.c_str(),strFileType.size());
//  		pBuff[ulBuffLen] = '\0';
//  		if (iSessionID == 0)		
//  		{
//  			SendToAll(pBuff,ulBuffLen+1);
//  		}
//  		else
//  		{
//  			SendToPoint(iSessionID,pBuff,ulBuffLen+1);
//  		}
//  		delete []pBuff;
//  		pBuff = NULL;
// */
// 
// //**************************************huanglongquan add by 2012年5月15日********************************/
// 		 int iCmdType = CMD_START_STREAM;
// 		 int iLength = 0;
// 		 string strValue = "";
// 		 MapWBAttributes* pWbAttribute = &m_vectorWbStatus[iTo];
// 		 if (pWbAttribute)
// 		 {
// 			 IT_MapWBAttributes it = pWbAttribute->find(ulObjID);
// 			 if (it != pWbAttribute->end())
// 			 {
// 				 XMemIni xmlPacket = it->second;
// 				 xmlPacket.DeleteKey(KEY_FILE_PATH);
// 				 xmlPacket.Set(KEY_FILE_POS,0);
// 				 xmlPacket.GetString(strValue);
// 				 iLength = sizeof(int) + strValue.size();
// 				 char* pBuff = new char[iLength + 1];
// 				 if (pBuff)
// 				 {
// 					 memset(pBuff,0,iLength + 1);
// 					 memcpy(pBuff,(char*)&iCmdType,sizeof(int));
// 					 memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
// 					 pBuff[iLength] = '\0';
// 					 if (iSessionID == 0)
// 					 {
// 						 SendToAll(pBuff,iLength + 1);
// 					 }
// 					 else
// 					 {
// 						 SendToPoint(iSessionID,pBuff,iLength + 1);
// 					 }
// 					 delete []pBuff;
// 					 pBuff = NULL;
// 				 }
// 			 }
// 		 }
// //**************************************huanglongquan add by 2012年5月15日********************************/
// 	}
// }

void CWBDataModel::StartStream(XMemIni xmlobj, unsigned int iSessionID )
{
	if ( m_pIHPDTSNotify)
	{
		int iCmdType = CMD_START_STREAM;
		int iLength = 0;
		xmlobj.DeleteKey(KEY_FILE_PATH);
		xmlobj.Set(KEY_FILE_POS,0);
		string strValue = "";
		xmlobj.GetString(strValue);
		if (strValue != "")
		{
			iLength = sizeof(int) + strValue.size();
			char* pBuff = new char[iLength + 1];
			if (pBuff)
			{
				memset(pBuff,0,iLength + 1);
				memcpy(pBuff,(char*)&iCmdType,sizeof(int));
				memcpy(pBuff + sizeof(int),strValue.c_str(),strValue.size());
				pBuff[iLength] = '\0';
				if (iSessionID == 0)
				{
					SendToAll(pBuff,iLength + 1);
				}
				else
				{
					SendToPoint(iSessionID,pBuff,iLength + 1);
				}
				delete []pBuff;
				pBuff = NULL;
			}
		}		
	}
//**************************************huanglongquan add by 2012年5月15日********************************/
}

//申请流
void CWBDataModel::AskForStream(unsigned int iSessionID,unsigned long ulObjID)
{
	int iBuffLen = 3*sizeof(unsigned long) + sizeof(int);
	unsigned long ulPacketLength = 64*PACKET_LENGTH;
	MapWBAttributes* pMapWbAttribute = NULL;
	{
		for (int i = 0;i < m_iNumOfStatus;i++)
		{
			pMapWbAttribute = &m_vectorWbStatus[i];
			if (pMapWbAttribute)
			{
				IT_MapWBAttributes it = pMapWbAttribute->find(ulObjID);
				if (it != pMapWbAttribute->end())
				{
					XMemIni xmlPacket = it->second;
					unsigned long ulPos = 0;
					unsigned long ulSize = 0;
					if (xmlPacket.Get(KEY_FILE_POS,ulPos) && xmlPacket.Get(KEY_FILE_SIZE,ulSize))
					{
						ulPacketLength = ulPacketLength>(ulSize - ulPos) ? ulSize - ulPos:ulPacketLength;
						int iCmdType = CMD_ASKFOR_STREAM;
						char* pBuff = new char[iBuffLen];
						memset(pBuff,0,iBuffLen);
						memcpy(pBuff,(char*)&iCmdType,sizeof(int));
						memcpy(pBuff + sizeof(int),(char*)&ulObjID,sizeof(unsigned long));
						memcpy(pBuff + sizeof(int) + sizeof(unsigned long),(char*)&ulPos,sizeof(unsigned long));
						memcpy(pBuff + sizeof(int) + 2*sizeof(unsigned long),(char*)&ulPacketLength,sizeof(unsigned long));
						SendToPoint(iSessionID,pBuff,iBuffLen);
						delete pBuff;
						pBuff = NULL;
					}
					break;
				}		
			}
		}
	}
}

void CWBDataModel::RegistPassiveObjID(unsigned long ulPassiveObjID)
{
	IT_MapSessionInfo it = m_mapSessionInfo.begin();
	while (it != m_mapSessionInfo.end())
	{
		IT_MapObjID itObjID = it->second.mapPassiveObjID.find(ulPassiveObjID);
		if (itObjID == it->second.mapPassiveObjID.end())
		{
			it->second.mapPassiveObjID.insert(MapObjID::value_type(ulPassiveObjID,ulPassiveObjID));
		}
		else
		{
			itObjID->second = ulPassiveObjID;
		}
		it++;
	}
}

void CWBDataModel::RegistPassiveObjID(unsigned long ulSessionID,unsigned long ulPassiveObjID)
{
	IT_MapSessionInfo it = m_mapSessionInfo.find(ulSessionID);
	if (it != m_mapSessionInfo.end())
	{
		IT_MapObjID itObjID = it->second.mapPassiveObjID.find(ulPassiveObjID);
		if (itObjID == it->second.mapPassiveObjID.end())
		{
			it->second.mapPassiveObjID.insert(MapObjID::value_type(ulPassiveObjID,ulPassiveObjID));
		}
		else
		{
			itObjID->second = ulPassiveObjID;
		}
	}
}

void CWBDataModel::UnregistPassiveObjID(unsigned long ulPassiveObjID)
{
	IT_MapSessionInfo it = m_mapSessionInfo.begin();
	while (it != m_mapSessionInfo.end())
	{
		IT_MapObjID itObjID = it->second.mapPassiveObjID.find(ulPassiveObjID);
		if (itObjID != it->second.mapPassiveObjID.end())
		{
			it->second.mapPassiveObjID.erase(itObjID);
		}
		it++;
	}
}

//发给除iSessionID以外的所有人
void CWBDataModel::SendToOthers(unsigned long iSessionID,char* pData,int iLen,unsigned long ulPassiveObjID)
{
	if (m_pIHPDTSNotify)
	{
		IT_MapSessionInfo it = m_mapSessionInfo.begin();
		unsigned long ulSessionID = 0;
		while (it != m_mapSessionInfo.end())
		{
			if (ulPassiveObjID == 0)
			{
				ulSessionID = it->second.ulSessionID;
				if (ulSessionID != iSessionID)
				{
					SendToPoint(ulSessionID,pData,iLen);
				}
				it++;
			}
			else
			{
				ulSessionID = it->second.ulSessionID;
				if (ulSessionID != iSessionID)
				{
					IT_MapObjID itObjID = it->second.mapPassiveObjID.find(ulPassiveObjID);
					if (itObjID != it->second.mapPassiveObjID.end())
					{
						SendToPoint(ulSessionID,pData,iLen);
					}
				}
				it++;
			}
		}
	}
}
//全网发送
void CWBDataModel::SendToAll(char* pData,int iLen,unsigned long ulPassiveObjID)
{
	if (m_pIHPDTSNotify)
	{
		IT_MapSessionInfo it = m_mapSessionInfo.begin();
		while (it != m_mapSessionInfo.end())
		{
			if (ulPassiveObjID == 0)
			{
				unsigned long ulSessionID = it->second.ulSessionID;
				SendToPoint(ulSessionID,pData,iLen);
				it++;
			}
			else
			{
				IT_MapObjID itOfObjID = it->second.mapPassiveObjID.find(ulPassiveObjID);
				if (itOfObjID != it->second.mapPassiveObjID.end())
				{
					unsigned long ulSessionID = it->second.ulSessionID;
					SendToPoint(ulSessionID,pData,iLen);
				}
				it++;
			}
		}
	}
}
//对点发送
void CWBDataModel::SendToPoint(unsigned long iSessionID,char* pData,int iLen)
{
	if (m_pIHPDTSNotify)
	{
		int iLength = iLen + sizeof(int);
		AVDataOutPacket outPacket;
		outPacket << (UINT)WB_NEW_COMMAND_DATA;
		outPacket << (UINT)iLen;
		outPacket.WriteData((unsigned char*)pData,iLen);
		m_pIHPDTSNotify->onDTSNetDataNotify(iSessionID,(char*)outPacket.GetData(),outPacket.GetLength());
	}
}
bool CWBDataModel::ToMemIni(XMemIni &xmlPacket,char* pData,int iLen)
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
//是否添加事件。  unsigned long ulEventID为事件ID
//假如当前接收到得事件ID和当前状态对应的事件ID是一样的，说明接收到相同事件的数据，不要添加新事件；
//如果------------------------------------------不同的，说明接收到新事件的数据，将新事件压入事件表，返回true，告知调用函数需要更新状态
bool CWBDataModel::NeedToSlide(unsigned long ulEventID)
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
// 	IT_MapWBAttributes it = m_vectorWbStatus[m_iIndexOfStatus].begin();
// 	while (it !=  m_vectorWbStatus[m_iIndexOfStatus].end())
// 	{
// 		unsigned long ulObjEventID = 0;
// 		if (it->second.Get(KEY_EVENT_ID,ulObjEventID) &&  ulObjEventID == ulEventID)
// 		{
// 			return false;
// 		}
// 		it++;
// 	}
// 	return true;
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
		m_vectorWbStatus[m_iIndexOfStatus] = m_vectorWbStatus[iOldIndexOfStatus];
		
		if (m_iSlidePos == 0)
		{
			if (m_iSlideNum<m_iNumOfStatus-1)
			{
				m_iSlideNum++;
			}
		}
		else if (m_iSlidePos < 0)
		{
			m_iSlideNum = m_iNumOfStatus + m_iSlidePos;
			m_iSlidePos = 0;
		}
		
	}
}
void CWBDataModel::prepareCoverStatus(int iIndexOfStatus)
{
	IT_MapWBAttributes it = m_vectorWbStatus[iIndexOfStatus].begin();
	while (it != m_vectorWbStatus[iIndexOfStatus].end())
	{
		unsigned long ulObjID = it->first;
		if (!ObjExistInAnyOtherStatus(ulObjID,iIndexOfStatus))
		{
			XMemIni xmlPacket = it->second;
			unsigned long ulSize = 0;
			unsigned long ulPos = 0;
			string strPath = "";
			if (xmlPacket.Get(KEY_FILE_SIZE,ulSize) && xmlPacket.Get(KEY_FILE_POS,ulPos) && xmlPacket.Get(KEY_FILE_PATH,strPath))
			{
				if (ulSize > ulPos)//表示没有完成
				{
					int iCmdType = CMD_STOP_STREAM;
					int iLen = sizeof(int) + sizeof(unsigned long);
					char* pData = new char[iLen];
					if (pData)
					{
						memset(pData,0,iLen);
						memcpy(pData,(char*)&iCmdType,sizeof(int));
						memcpy(pData + sizeof(int) ,(char*)&ulObjID,sizeof(unsigned long));
						unsigned long ulSessionID = 0;
						xmlPacket.Get(KEY_FILE_SENDER,ulSessionID);
						SendToPoint(ulSessionID,pData,iLen);
						delete pData;
						pData = NULL;
					}
				}
				remove(strPath.c_str());
			}
		}
		it++;
	}
}
bool CWBDataModel::ObjExistInAnyOtherStatus(unsigned long ulObjID,int iIndexStatus)
{
	for (int i = 0;i<m_iNumOfStatus;i++)
	{
		if (i != iIndexStatus)
		{
			if (m_vectorWbStatus[i].find(ulObjID) != m_vectorWbStatus[i].end())
			{
				return true;
			}
		}
	}
	return false;
}
bool CWBDataModel::ObjExist(unsigned long ulObjID)
{
	MapWBAttributes* pMapWbAttribute = &m_vectorWbStatus[m_iIndexOfStatus];
	if (pMapWbAttribute)
	{
		IT_MapWBAttributes it  = pMapWbAttribute->find(ulObjID);
		if (it != pMapWbAttribute->end())
		{
			return true;
		}
	}
	return false;
}
bool CWBDataModel::GetFileStatus(string strPath,unsigned long &iSize,string &strType)
{
	FILE* pFile = NULL;
	pFile = fopen(strPath.c_str(),"rb");
	if (pFile)
	{
		fseek(pFile,0,SEEK_END);
		iSize = ftell(pFile);
		fclose(pFile);
		pFile = NULL;
		int iPos = strPath.find_last_of('.',-1);
		//函数find_last_of()功能如下:
		//从index开始进行逆向搜索, 返回字符串中遇到的第一个与str中任意一个字符相匹配的字符索引, 如果都不匹配则返回string::npos,
		//从index开始进行逆向搜索, 返回字符串中遇到的第一个与str前导num个字符中任意一个像匹配的字符索引, 如果都不匹配则返回string::npos,
		//从index开始进行逆向搜索, 返回字符串中遇到的第一个与ch相匹配的字符索引, 如果没有匹配字符则返回string::npos.
		if (iPos != -1)
		{
			strType = strPath.substr(iPos+1,-1);//功能：从一个字符串复制一个从指定位置开始，并具有指定长度的子字符串。
			                                    //substr（）函数有两个参数，第一个表示字符开始的位置，第二个表示返回的长度
			return true;
		}
	}
	return false;
}

void CWBDataModel::CheckXmlPacketPath( XMemIni &xmlPacket )
{
 	std::string strPath = "";
 	if (xmlPacket.Get(KEY_FILE_PATH,strPath) && strPath != "")
 	{
 		char pPath[MAX_PATH] = {0};
 		int nPos = 0;
 		string strName = "";
 #ifdef WIN32
 		nPos = strPath.find_last_of("\\",-1);
 		if (nPos != string::npos)
 		{
 			strName = strPath.substr(nPos +2,-1);
 			sprintf(pPath,"%s\\%s",GetFileTemPath(),strName);	
 		}
 #else
 		nPos = strPath.find_last_of("/",-1);
 		if (nPos != string::npos)
 		{
 			strName = strPath.substr(nPos + 1,-1);
 			sprintf(pPath,"%s/%s",GetFileTemPath(),strName);	
 		}
 #endif
 		if (nPos != string::npos)
 		{
 			strPath = pPath;
 			xmlPacket.Set(KEY_FILE_PATH,strPath);
 		}
 	}
 	else
 	{
 		return;
 	}
}

void CWBDataModel::SetUserColor( eUserColorWB eColorWB )
{
	MyRGB rgb;
	switch (eColorWB)
	{
	case Red_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 255;
			rgb.nGreen = 0;
			rgb.nBlue = 0;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(Red_Color,rgb));
		}
		break;
	case Green_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 0;
			rgb.nGreen = 255;
			rgb.nBlue = 0;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(Green_Color,rgb));
		}
		break;
	case Blue_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 0;
			rgb.nGreen = 0;
			rgb.nBlue = 255;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(Blue_Color,rgb));
		}
		break;
	case Yellow_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 255;
			rgb.nGreen = 255;
			rgb.nBlue = 0;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(Yellow_Color,rgb));
		}
		break;
	case Fuchsia_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 255;
			rgb.nGreen = 0;
			rgb.nBlue = 255;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(Fuchsia_Color,rgb));
		}
		break;
	case Aqua_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 0;
			rgb.nGreen = 255;
			rgb.nBlue = 255;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(Aqua_Color,rgb));
		}
		break;
	case MidnightBlue_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 25;
			rgb.nGreen = 25;
			rgb.nBlue = 112;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(MidnightBlue_Color,rgb));
		}
		break;
	case SeaGreen_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 46;
			rgb.nGreen = 139;
			rgb.nBlue = 87;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(SeaGreen_Color,rgb));
		}
		break;
	case Teal_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 0;
			rgb.nGreen = 128;
			rgb.nBlue = 128;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(Teal_Color,rgb));
		}
		break;
	case DarkSlateGray_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 47;
			rgb.nGreen = 79;
			rgb.nBlue = 79;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(DarkSlateGray_Color,rgb));
		}
		break;
	case DarkGreen_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 0;
			rgb.nGreen = 100;
			rgb.nBlue = 0;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(DarkGreen_Color,rgb));
		}
		break;
	case SaddleBrown_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 139;
			rgb.nGreen = 69;
			rgb.nBlue = 19;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(SaddleBrown_Color,rgb));
		}
		break;
	case Maroon_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 128;
			rgb.nGreen = 0;
			rgb.nBlue = 0;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(Maroon_Color,rgb));
		}
		break;
	case BlueViolet_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 138;
			rgb.nGreen = 43;
			rgb.nBlue = 226;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(BlueViolet_Color,rgb));
		}
		break;
	case DarkOrchid_Color:
		{
			rgb.bOccupy = false;
			rgb.nRed = 153;
			rgb.nGreen = 50;
			rgb.nBlue = 24;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(DarkOrchid_Color,rgb));
		}
		break;
	default:
		{
			rgb.bOccupy = false;
			rgb.nRed = 0;
			rgb.nGreen = 0;
			rgb.nBlue = 0;
			m_mapCorrespondingRGB.insert(MapCorrespondingRGB::value_type(Default_Color,rgb));
		}
		break;
	}
}

bool CWBDataModel::GetUserColor( eUserColorWB eColorWB,unsigned int& nRed,unsigned int& nGreen,unsigned int& nBlue,bool bOccupy /*= false*/ )
{
	XAutoLock l(m_csMapRGBInfo);
	IT_MapCorrespondingRGB it = m_mapCorrespondingRGB.find(eColorWB);
	if (it != m_mapCorrespondingRGB.end())
	{
		nRed = it->second.nRed;
		nGreen = it->second.nGreen;
		nBlue = it->second.nBlue;
		it->second.bOccupy = bOccupy;
		return true;
	}
	return false;
}

void CWBDataModel::OnBroadcastUserColor( unsigned long iSessionID,char* pData, int iLen )
{
	if (pData == NULL || iLen < 0 || *(pData + iLen -1) != '\0')
	{
		return;
	}

	if (!m_bUI && m_pIHPDTSNotify) //服务器相应这个函数
	{
		char* pBuff = pData;
		pBuff += sizeof(int);
		unsigned int unUserColor = *((unsigned int*)pBuff);
		pBuff += sizeof(unsigned int);
		string str_name = pBuff;
		XAutoLock L(m_csMapUserColorInfo);
		IT_MapUserColorInfo it = m_mapUserColorInfo.find(str_name);
		if (it != m_mapUserColorInfo.end())
		{
			if (it->second == unUserColor)
			{
				return ;
			}
			else
			{
		//		TRACE("error 服务器已经存在该用户颜色，但是不一样！");
			}
		}
		else
		{
			m_mapUserColorInfo.insert(MapUserColorInfo::value_type(str_name,(eUserColorWB)unUserColor));
			XAutoLock lrgb(m_csMapRGBInfo);
			IT_MapCorrespondingRGB itrgb = m_mapCorrespondingRGB.find((eUserColorWB)unUserColor);
			if (itrgb != m_mapCorrespondingRGB.end())
			{
				itrgb->second.bOccupy = true;
			}
			SendToOthers(iSessionID,pData,iLen);
		}
	}	
}

