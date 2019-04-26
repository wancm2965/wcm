
//========================================================================================
//版权所有：版权所有(C) 2008，上海华平信息技术股份有限公司
//系统名称：AVCON7
//作　　者：杜权胜
//创建日期：2008-3-31
//功能说明：数据传输服务器内核，包括文件传输，文件分发，文档共享，Flash共享，电子白板等内核模块。
//


#include "stdafx.h"
#include "HPDTS.h"
#include "CharArray.h"
#include "XBufferPool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------------------------
HPDTS::HPDTS(IHPDTSNotify&rIHPDTSNotify,bool bUI)
:WbDTS(rIHPDTSNotify,bUI)
,CWBDataModel(rIHPDTSNotify,bUI)
,MeetingDTS(rIHPDTSNotify,bUI)
,m_rIHPDTSNotify(rIHPDTSNotify)
,m_bUINotify(bUI)
{	
	m_dtsTemData.nCurPage = 0;
	m_dtsTemData.nTotalPage = 0;
	m_dtsTemData.nFlashStatus = 1;
	m_dtsTemData.nFlashPosition = 0;
	m_dtsTemData.nDocZoomSize = 1;	
	m_dtsTemData.nDocLeft = 0;	
	m_dtsTemData.nDocTop = 0;	
}
//---------------------------------------------------------------------------------------
void HPDTS::Init(void)
{
	
}

//---------------------------------------------------------------------------------------
void HPDTS::Release(void)
{	
	try
	{

		m_MapSendFilePath.clear();	

		{
			XAutoLock l(m_csMapAutoPause);
			m_MapAutoPause.clear();
		}

		{
			XAutoLock l(m_csMapPauseDownLoad);
			m_MapPauseDownLoad.clear();
		}

		{
			XAutoLock l(m_csMapPauseUpLoad);
			m_MapPauseUpLoad.clear();
		}

		{
			XAutoLock l(m_csMapSessionID);	
			m_MapSessionID.clear();	
		}

		ClearAllObject();
	}
	catch (...)
	{
		TRACE(TEXT("catch int HPDTS::Release \n"));
	}
}
//---------------------------------------------------------------------------------------
void HPDTS::AddDTSSession(unsigned long ulSessionID,bool bHost)
{
//	TRACE("ulSessioonID = %d ,bHost= %d",ulSessionID,bHost);
//	AddWBDTSSession(ulSessionID);
	AddWBDTSSession(ulSessionID,bHost);//huanglongquan 修改2012年5月15日
	//为原白板增加通道
	addChannel(ulSessionID);

	AddMeetingSession(ulSessionID);

	{
		XAutoLock l(m_csMapSessionID);
		m_MapSessionID[ulSessionID] = bHost;	
	}


	//发送文件列表
	if (!m_bUINotify)
	{	
		XBufferPool tXBufferPool;
		{
			XAutoLock f(m_csMapFileList);	
			for(MapFileList::iterator it = m_MapFileList.begin(); it != m_MapFileList.end(); it++)
			{
				DTS_FILE_LIST * pDtsFileList = it->second;
				if (pDtsFileList)
				{
					AVDataOutPacket outPacket;
					outPacket << (UINT)FILE_ONGET_FILELIST;
					outPacket << pDtsFileList->ulFileID;
					outPacket << pDtsFileList->ulFileLength;				
					outPacket << pDtsFileList->chSendUserID;
					outPacket << pDtsFileList->chFileName;	
					outPacket << pDtsFileList->chDescription;
					outPacket << pDtsFileList->chStartTime;

					XDataBuffer*pXDataBuffer=new XDataBuffer();
					pXDataBuffer->StoreData((char *)outPacket.GetData(),outPacket.GetLength());
					tXBufferPool.SetDataBuffer(pXDataBuffer);
				}
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

	//TRACE(TEXT("HPDTS::AddDTSSession ulSessionID = %u this = %u \n"),ulSessionID,this);
}

//---------------------------------------------------------------------------------------
void HPDTS::RemoveDTSSession(unsigned long ulSessionID)
{	
	RemoveWBDTSSession(ulSessionID);
	removeChannel(ulSessionID);

	DelMeetingSession(ulSessionID);

	XAutoLock l(m_csMapSessionID);
	m_MapSessionID.erase(ulSessionID);
}

//---------------------------------------------------------------------------------------
void HPDTS::ClearAllObject(void)
{
	{
		XAutoLock l(m_csMapFileList);	
		for(MapFileList::iterator it = m_MapFileList.begin(); it != m_MapFileList.end(); it++)
		{
			DTS_FILE_LIST * pDtsFileList = it->second;
			if (pDtsFileList)
			{
				unsigned long ulFileID = pDtsFileList->ulFileID;
				//删除临时文件
				char szPath[255] = {0};
#ifdef WIN32
				sprintf(szPath,"%s\\%u.dts",GetFileTemPath(),ulFileID);
				remove(szPath);
#else
				sprintf(szPath,"rm -f %s/%u.dts",GetFileTemPath(),ulFileID);
				system(szPath);  //删除文件
#endif

				ASSERT(pDtsFileList);
				delete pDtsFileList;
				pDtsFileList = NULL;
			}
		}

		m_MapFileList.clear();
	}

	{
		XAutoLock l(m_csMapObjects);	
		for(MapObjects::iterator it = m_MapObjects.begin(); it != m_MapObjects.end(); it++)
		{
			OBJ_DATA_BUF objData = (*it).second;
			if (objData.pData)
			{
				ASSERT(objData.pData);
				delete objData.pData;
				objData.pData = NULL;
			}
		}		
		m_MapObjects.clear();
	}
}
//---------------------------------------------------------------------------------------
int HPDTS::CopyFile( const char *source, const char *dest )
{	
	try
	{
		FILE *fs = NULL, *fd = NULL;
		char buff[65535] = {0};
		int n;

		fs=fopen( source, "rb" );
		fd=fopen( dest, "wb" );
		
		while( fs && fd )
		{
			if( !feof(fs))
			{
				n=fread( buff, sizeof(char), sizeof(buff), fs );
				if (n >=0)
				{
					fwrite( buff, sizeof(char), n, fd );
					fflush(fd);
				}
			} else if( feof(fs) ) {
				fclose( fd );
				fclose( fs );
				return( 0 );
			} else {
				fclose( fd );
				fclose( fs );
				return( -1 );
			}
		}
	}
	catch (...)
	{
//		TRACE(TEXT("catch int HPDTS::CopyFile \n"));
		return (-1);
	}
	return( 0 );
}

//---------------------------------------------------------------------------------------
//命令发送给所以连接

void HPDTS::SendCommandToAll(const char* pData,int nLen,unsigned long ulSessionID)
{
	std::map<unsigned long,unsigned long> mapTem;

	{
		XAutoLock l(m_csMapSessionID);
		MapSessionID::iterator it = m_MapSessionID.begin();
		while(it != m_MapSessionID.end()) 
		{
			unsigned long ulSID = (*it).first;
			if (ulSessionID != ulSID)
			{
				mapTem[ulSID] = ulSID;
			}
			it++;
		}
	}

	std::map<unsigned long,unsigned long>::iterator itMap = mapTem.begin();
	while (itMap != mapTem.end())
	{
		unsigned long ulSID = (*itMap).first;
		m_rIHPDTSNotify.onDTSNetDataNotify(ulSID,pData,nLen);
		itMap++;
	}
	mapTem.clear();	
}
//---------------------------------------------------------------------------------------
POINT HPDTS::FilePoint2Client(POINT pt)
{
	POINT ptClient;
	ptClient.x = pt.x;
	ptClient.y = pt.y;
	if (m_dtsTemData.ptFile.x > 0 && m_dtsTemData.ptFile.y > 0)
	{
		ptClient.x = (pt.x * m_dtsTemData.ptClient.x)/((float)(m_dtsTemData.ptFile.x * 1.00));
		ptClient.y = (pt.y * m_dtsTemData.ptClient.y)/((float)(m_dtsTemData.ptFile.y * 1.00));
	}
	return ptClient;
}

//---------------------------------------------------------------------------------------
POINT HPDTS::ClientPoint2File(POINT pt)
{
	POINT ptFile;
	ptFile.x = pt.x;
	ptFile.y = pt.y;
	if (m_dtsTemData.ptClient.x > 0 && m_dtsTemData.ptClient.y > 0)
	{
		ptFile.x = (pt.x * m_dtsTemData.ptFile.x)/((float)(m_dtsTemData.ptClient.x * 1.00));
		ptFile.y = (pt.y * m_dtsTemData.ptFile.y)/((float)(m_dtsTemData.ptClient.y * 1.00));
	}
	return ptFile;
}

//---------------------------------------------------------------------------------------
void HPDTS::WaitAutoPause(unsigned long ulSessionID,unsigned long ulFileID,unsigned long ulPosition)
{
	AVDataOutPacket outPacket;
	bool bRet=false;
	{
		XAutoLock l(m_csMapFileList);
		MapFileList::iterator it = m_MapFileList.find(ulFileID);
		if (it != m_MapFileList.end())
		{
			//回调等待下载
			outPacket << (UINT)FILE_UPDATE_STATUS;			
			outPacket << ulFileID;
			outPacket << (UINT)FILESTATUS_WAIT;

			bRet=true;
		}
	}

	if ( bRet)
	{
		m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());	

		//存储自动暂停位置
		char shSession[128] = {0};
		sprintf(shSession,"%u%u",ulFileID,ulSessionID);
		std::string strSession = shSession;//UL2STR(ulFileID) + UL2STR(ulSessionID);
		
		{
			XAutoLock c(m_csMapAutoPause);
			MapAutoPause::iterator itTrans = m_MapAutoPause.find(strSession);
			if (itTrans == m_MapAutoPause.end())
			{
				m_MapAutoPause[strSession] = ulPosition;
			}
		}
	}
}

//---------------------------------------------------------------------------------------
void HPDTS::RequestDownLoad(unsigned long ulSessionID,unsigned long ulFileID,unsigned long ulPosition,unsigned long ulFileLength)
{
	for (int i = 0;i < FILE_TRANS_PACKETCOUNT;i++)
	{
		if ((ulPosition + i*FILE_TRANS_MAXLENGTH) < ulFileLength)
		{
			AVDataOutPacket outPkt;
			outPkt << (UINT)FILE_ONREQUEST_DOWNLOAD;			
			outPkt << ulFileID;	
			outPkt << (unsigned long)(ulPosition + i*FILE_TRANS_MAXLENGTH);		
			outPkt << ulFileLength;
			m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPkt.GetData(),outPkt.GetLength());

#ifdef WIN32
//			LOG_WRITE(1, "FILE_ONREQUEST_DOWNLOAD", __FILE__, __LINE__, "ulPosition: %u   ulFileLength: %u ",(ulPosition + i*FILE_TRANS_MAXLENGTH),ulFileLength);
#endif
		}		
	}
}

//---------------------------------------------------------------------------------------
void HPDTS::processUIData(const char* pData,int nLen)
{
	try
	{
		UINT usCmd;
		AVDataInPacket in((char*)pData,nLen);
		in >> usCmd;

		switch(usCmd)
		{
		case WB_NEW_COMMAND_DATA:
			{
				processWBUIData(in);
			}
			break;
		case WB_COMMAND_DATA:
			{
				processWBCmdData(in,LOCAL_SESSIONID);
				break;
			}
		case MET_DTS_ADD:
		case MET_DTS_UPDATE:
		case MET_DTS_DELETE:
		case MET_DTS_REQUEST:
		case MET_DTS_CHATMSG:
			{
				ProMeetingUIData(pData,nLen);
				break;
			}
		case FILE_SEND_ADDFILE:
			{
				processSendAddFile(in);
				break;
			}
		case FILE_SEND_DELETEFILE:
			{
				processSendDeleteFile(in);
				break;
			}	
		case FILE_REQUEST_DOWNLOAD:
			{
				processRequestDownload(in);
				break;
			}
		case FILE_PAUSE_DOWNLOAD:
			{
				processPauseDownload(in);
				break;
			}
		case FILE_CONTINUE_DOWNLOAD:
			{
				processContinueDownload(in);
				break;
			}
		case FILE_PAUSE_UPLOAD:
			{
				processPauseUpload(in);
				break;
			}
		case FILE_CONTINUE_UPLOAD:
			{
				processContinueUpload(in);
				break;
			}
		case FILE_GET_FILEINFO:
			{
				processGetFileInfo(in);
				break;
			}
		case FILE_GET_POSITION:
			{
				processGetPosition(in);
				break;
			}
		case FILE_REQUEST_FILELIST:
			{
				processRequestFileList(in);
				break;
			}
		case FS_ADD_OBJ:
			{
				processFSAddObj(in,LOCAL_SESSIONID);
				break;
			}
		case FS_DELETE_OBJ:
			{
				processFSDeleteObj(in,LOCAL_SESSIONID);
				break;
			}
		case FS_CHANGE_PAGE:
			{
				processFSChangePage(in,LOCAL_SESSIONID);
				break;
			}
		case FS_GETCUR_PAGEOBJ:
			{
				processFSGetCurPageObj(in);
				break;
			}
		case FS_REQUEST_ALLOBJ:
			{
				processFSRequestAllObj(in,LOCAL_SESSIONID);
				break;
			}
		case FS_DELCUR_PAGEOBJ:
			{
				processFSDeleteCurPageObj(in);
				break;
			}
		case FS_SET_FILERECT:
			{
				processFSSetFileRect(in);
				break;
			}
		case FLASH_SHARE_PLAY:
			{
				processFlashPlay(in,LOCAL_SESSIONID);
				break;
			}
		case FLASH_SHARE_STOP:
			{
				processFlashStop(in,LOCAL_SESSIONID);
				break;
			}
		case FLASH_SHARE_PAUSE:
			{
				processFlashPause(in,LOCAL_SESSIONID);
				break;
			}
		case FLASH_SHARE_POSITION:
			{
				processFlashPosition(in,LOCAL_SESSIONID);
				break;
			}
		case FLASH_SHARE_REQUEST:
			{
				processFlashRequest(in,LOCAL_SESSIONID);
				break;
			}
		case FLASH_SHARE_SEEK:
			{
				processFlashSeek(in,LOCAL_SESSIONID);
				break;
			}
		case FS_SET_ZOOM:
			{
				processFSZoom(in,LOCAL_SESSIONID);
				break;
			}
		case FS_MOVE_POS:
			{
				processFSMovePos(in,LOCAL_SESSIONID);
				break;
			}
		case FS_GET_POS:
			{
				processFSGetPos(in,LOCAL_SESSIONID);
				break;
			}
		default:
			TRACE("HPDTS::processUIData unknown command.usCmd = %d\n",usCmd);
		}
	}
	catch (...)
	{
		TRACE(TEXT("catch void HPDTS::processUIData \n"));
	}
}

//---------------------------------------------------------------------------------------
void HPDTS::processDTSData(unsigned long ulSessionID,const char* pData,int nLen)
{
	try
	{
		UINT usCmd;
		AVDataInPacket in((char*)pData,nLen);
		in >> usCmd;
		switch(usCmd)
		{
		case WB_NEW_COMMAND_DATA:
			{
				processWBDTSData(in,ulSessionID);
				break;
			}
		case WB_COMMAND_DATA:
			{
				processWBCmdData(in,ulSessionID);
				break;
			}
		case MET_DTS_CONNECTED:
		case MET_DTS_ADD:
		case MET_DTS_UPDATE:
		case MET_DTS_DELETE:
		case MET_DTS_REQUEST:
		case MET_DTS_RESPONSE:
		case MET_DTS_CHATMSG:
		case MET_DTS_READY:
			{
				ProMeetingDTSData(ulSessionID,pData,nLen);
				break;
			}
		case FILE_GET_FILELIST:
			{
				processGetFileList(in,ulSessionID);
				break;
			}
		case FILE_ONGET_FILELIST:
			{
				processOnGetFileList(in,ulSessionID);
				break;
			}
		case FILE_INSERT_FILE:
			{
				processInsertFile(in,ulSessionID);
				break;
			}
		case FILE_UPDATE_STATUS:
			{
				processUpdateFileStatus(in,ulSessionID);
				break;
			}
		case FILE_DELETE_FILE:
			{
				processDeleteFile(in,ulSessionID);
				break;
			}
		case FILE_ONREQUEST_DOWNLOAD:
			{
				processOnRequestDownload(in,ulSessionID);
				break;
			}
		case FILE_ONDOWNLOAD_DATA:
			{
				processOnDownloadData(in,ulSessionID);
				break;
			}
		case FILE_AUTO_DOWNLOAD:
			{
				processAutoDownload(in,ulSessionID);
				break;
			}
		case FS_ADD_OBJ:
			{
				processFSAddObj(in,ulSessionID);
				break;
			}
		case FS_DELETE_OBJ:
			{
				processFSDeleteObj(in,ulSessionID);
				break;
			}
		case FS_CHANGE_PAGE:
			{
				processFSChangePage(in,ulSessionID);
				break;
			}
		case FS_REQUEST_ALLOBJ:
			{
				processFSRequestAllObj(in,ulSessionID);
				break;
			}
		case FLASH_SHARE_PLAY:
			{
				processFlashPlay(in,ulSessionID);
				break;
			}
		case FLASH_SHARE_STOP:
			{
				processFlashStop(in,ulSessionID);
				break;
			}
		case FLASH_SHARE_PAUSE:
			{
				processFlashPause(in,ulSessionID);
				break;
			}
		case FLASH_SHARE_POSITION:
			{
				processFlashPosition(in,ulSessionID);
				break;
			}
		case FLASH_SHARE_REQUEST:
			{
				processFlashRequest(in,ulSessionID);
				break;
			}
		case FLASH_SHARE_SEEK:
			{
				processFlashSeek(in,ulSessionID);
				break;
			}
		case FS_SET_ZOOM:
			{
				processFSZoom(in,ulSessionID);
				break;
			}
		case FS_MOVE_POS:
			{
				processFSMovePos(in,ulSessionID);
				break;
			}
		default:
			TRACE("HPDTS::processHPDTSData unknown command.usCmd = %d\n",usCmd);
		}
	}
	catch (...)
	{
		TRACE(TEXT("catch void HPDTS::processDTSData \n"));
	}
}
//---------------------------------------------------------------------------------------

void HPDTS::processWBCmdData(AVDataInPacket& in,unsigned long ulSessionID)
{	
	UINT nDataLen = 0;
	in >> nDataLen;	
	unsigned char* pData = NULL;
	pData = in.ReadData((int&)nDataLen);
	processWbData(ulSessionID,(char*)pData,nDataLen);
}
//---------------------------------------------------------------------------------------

void HPDTS::processGetFileList(AVDataInPacket& in,unsigned long ulSessionID)
{
	XBufferPool tXBufferPool;
	{
		XAutoLock l(m_csMapFileList);	
		for(MapFileList::iterator it = m_MapFileList.begin(); it != m_MapFileList.end(); it++)
		{
			DTS_FILE_LIST * pDtsFileList = it->second;
			if (pDtsFileList)
			{
				AVDataOutPacket outPacket;
				outPacket << (UINT)FILE_ONGET_FILELIST;
				outPacket << pDtsFileList->ulFileID;
				outPacket << pDtsFileList->ulFileLength;				
				outPacket << pDtsFileList->chSendUserID;
				outPacket << pDtsFileList->chFileName;	
				outPacket << pDtsFileList->chDescription;
				outPacket << pDtsFileList->chStartTime;

				XDataBuffer*pXDataBuffer=new XDataBuffer();
				pXDataBuffer->StoreData((char *)outPacket.GetData(),outPacket.GetLength());
				tXBufferPool.SetDataBuffer(pXDataBuffer);

				//m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());	
				//TRACE(TEXT("HPDTS::processGetFileList FILE_ONGET_FILELIST ulSessionID = %u  ulFileID = %u\n"),ulSessionID,pDtsFileList->ulFileID);
			}
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
void HPDTS::processOnGetFileList(AVDataInPacket& in,unsigned long ulSessionID)
{	
	unsigned long	ulFileID = 0;
	unsigned long	ulFileLength = 0;		
	char			chSendUserID[255] = {0};
	char			chFileName[128] = {0};	
	char			chDescription[255] = {0};
	char			chStartTime[30] = {0};
	char			chFilePath[255] = {0};	
	in >> ulFileID >> ulFileLength >> chSendUserID >> chFileName >> chDescription >> chStartTime;
	
	//分发文件列表
	AVDataOutPacket outPacket;
	outPacket << (UINT)FILE_ONGET_FILELIST;
	outPacket << ulFileID;
	outPacket <<ulFileLength;				
	outPacket << chSendUserID;
	outPacket << chFileName;	
	outPacket << chDescription;
	outPacket << chStartTime;

	//TRACE(TEXT("HPDTS::processOnGetFileList FILE_ONGET_FILELIST ulSessionID = %u  ulFileID = %u\n"),ulSessionID,ulFileID);


	//命令发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);
	
	//将文件列表写入缓存	
	SAVE_FILE_LIST	

	//回调界面数据
	if (m_bUINotify)
	{
		AVDataOutPacket outPacket;
		outPacket << (UINT)FILE_INSERT_FILE;			
		outPacket << ulFileID;
		outPacket << ulFileLength;			
		outPacket << chSendUserID;
		outPacket << chFileName;
		outPacket << chDescription;
		outPacket << chStartTime;
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
	}
	else //服务端主动请求下载文件(文件列表不是来自Host主机)	
	{
		/*
		bool bHost=true;
		{
			XAutoLock l(m_csMapSessionID);
			MapSessionID::iterator it = m_MapSessionID.find(ulSessionID);	
			if (it != m_MapSessionID.end())
			{
				bHost = (*it).second;
			}
		}
		if (!bHost)
		*/
		{
			
			RequestDownLoad(ulSessionID,ulFileID,0,ulFileLength);
			//added by luoning 2010-9-17
			if (ulSessionID != LOCAL_SESSIONID)
			{
				AVDataOutPacket outPacket;
				outPacket << (UINT)FS_REQUEST_ALLOBJ;	
				//发送给所以连接
				SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);
			}
			//end
		}
	}
}

//---------------------------------------------------------------------------------------
void HPDTS::processSendAddFile(AVDataInPacket& in)
{
	unsigned long	ulFileID = 0;
	unsigned long	ulFileLength = 0;		
	char			chSendUserID[255] = {0};
	char			chFileName[128] = {0};	
	char			chDescription[255] = {0};
	char			chStartTime[30] = {0};
	char			chFilePath[255] = {0};	

	unsigned long	ulSessionID = LOCAL_SESSIONID;

	in >> ulFileID >> ulFileLength >> chSendUserID >> chFileName >> chDescription >> chStartTime >> chFilePath;
	
	m_MapSendFilePath[ulFileID]=chFilePath;

	char* ret = strrchr(chFilePath,'\\');
	if(ret)
	{
		int idxlen = (int)(ret - chFilePath);
		chFilePath[idxlen]=0;
	}

	AVDataOutPacket outPacket;
	outPacket << (UINT)FILE_INSERT_FILE;			
	outPacket << ulFileID;
	outPacket << ulFileLength;				
	outPacket << chSendUserID;
	outPacket << chFileName;	
	outPacket << chDescription;
	outPacket << chStartTime;
	
	//分发列表
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);

	//将文件列表写入缓存	
	SAVE_FILE_LIST	

	//回调界面
	m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());

}

//---------------------------------------------------------------------------------------
void HPDTS::processSendDeleteFile(AVDataInPacket& in)
{
	processDeleteFile(in,LOCAL_SESSIONID);
}

//---------------------------------------------------------------------------------------
void HPDTS::processInsertFile(AVDataInPacket& in,unsigned long ulSessionID)
{
	unsigned long	ulFileID = 0;
	unsigned long	ulFileLength = 0;		
	char			chSendUserID[255] = {0};
	char			chFileName[128] = {0};	
	char			chDescription[255] = {0};
	char			chStartTime[30] = {0};
	char			chFilePath[255] = {0};	
	in >> ulFileID >> ulFileLength >> chSendUserID >> chFileName >> chDescription >> chStartTime;

	//分发文件列表
	AVDataOutPacket outPacket;
	outPacket << (UINT)FILE_INSERT_FILE;
	outPacket << ulFileID;
	outPacket <<ulFileLength;				
	outPacket << chSendUserID;
	outPacket << chFileName;	
	outPacket << chDescription;
	outPacket << chStartTime;

	//命令发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);

	//将文件列表写入缓存	
	SAVE_FILE_LIST	

	if (m_bUINotify)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
	}
	else //服务端主动请求下载文件(文件列表不是来自Host主机)	
	{
		/*
		bool bHost=true;
		{
			XAutoLock l(m_csMapSessionID);
			MapSessionID::iterator it = m_MapSessionID.find(ulSessionID);	
			if (it != m_MapSessionID.end())
			{
				bHost = (*it).second;
			}
		}
		if (!bHost)
		*/
		{
			RequestDownLoad(ulSessionID,ulFileID,0,ulFileLength);
		}
	}
}

//---------------------------------------------------------------------------------------
void HPDTS::processUpdateFileStatus(AVDataInPacket& in,unsigned long ulSessionID)
{
	unsigned long	ulFileID = 0;	
	UINT nStatus = 0;	
	in >> ulFileID >> nStatus;

	if (!m_bUINotify)
	{
		return;
	}

	AVDataOutPacket outPacket;
	bool bRet=false;
	{
		XAutoLock l(m_csMapFileList);
		MapFileList::iterator it = m_MapFileList.find(ulFileID);
		if (it != m_MapFileList.end())
		{
			bRet=true;
			//拷贝文件
			if (nStatus == FILESTATUS_DONELOAD)
			{
				DTS_FILE_LIST * pDtsFileList = it->second;
				if (pDtsFileList && strlen(pDtsFileList->chFilePath) > 0)
				{
					char szOldName[255] = {0};
					char szNewName[255] = {0};
					sprintf(szOldName,"%s\\%u.dts",GetFileTemPath(),ulFileID);			
					sprintf(szNewName,"%s\\%s",pDtsFileList->chFilePath,pDtsFileList->chFileName);	

					CopyFile(szOldName,szNewName);					
					ASSERT(pDtsFileList);
				}
			}

			outPacket << (UINT)FILE_UPDATE_STATUS;			
			outPacket << ulFileID;
			outPacket << nStatus;		
		}
	}

	if (bRet)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
	}
}
//---------------------------------------------------------------------------------------
void HPDTS::processDeleteFile(AVDataInPacket& in,unsigned long ulSessionID)
{	
	try
	{
		unsigned long	ulFileID = 0;	
		in >> ulFileID;

		AVDataOutPacket outPacket;
		outPacket << (UINT)FILE_DELETE_FILE;			
		outPacket << ulFileID;
		bool bFind = false;

		{
			XAutoLock l(m_csMapFileList);
			MapFileList::iterator it = m_MapFileList.find(ulFileID);
			if (it != m_MapFileList.end())
			{
				bFind = true;
				DTS_FILE_LIST * pDtsFileList = it->second;
				if (pDtsFileList)
				{
					std::string strFilePath = pDtsFileList->chFilePath;
					std::string strFileName = pDtsFileList->chFileName;
					if (m_bUINotify && strFilePath != "" && pDtsFileList->ulFromSessionID != LOCAL_SESSIONID )
					{					
						char szFilePath[255] = {0};					
#ifdef WIN32
						sprintf(szFilePath,"%s\\%s",strFilePath.c_str(),strFileName.c_str());	
						remove(szFilePath);
#else
						sprintf(szFilePath,"rm -f %s/%s",strFilePath.c_str(),strFileName.c_str());
						system(szFilePath);  //删除文件
#endif
					}

					ASSERT(pDtsFileList);
					delete pDtsFileList;
					pDtsFileList = NULL;
				}
				m_MapFileList.erase(ulFileID);
			}
		}
		

		if (bFind && m_bUINotify)
		{
			m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
		}

		//同步删除文件列表
		SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);

		char szPath[255] = {0};

#ifdef WIN32
		sprintf(szPath,"%s\\%u.dts",GetFileTemPath(),ulFileID);	
		remove(szPath);
#else
		sprintf(szPath,"rm -f %s/%u.dts",GetFileTemPath(),ulFileID);	
		system(szPath);  //删除文件
#endif
	}
	catch (...)
	{
		TRACE(TEXT("catch void HPDTS::processDeleteFile \n"));
	}
}

//---------------------------------------------------------------------------------------
//客户端调用
void HPDTS::processRequestDownload(AVDataInPacket& in)
{	
	unsigned long	ulFileID = 0;
	UINT			nReFresh = 0;
	in >> ulFileID >> nReFresh;
	unsigned long ulPosition;
	unsigned long ulFromSessionID;
	unsigned long ulFileLength;
	bool bFind = false;

	{
		XAutoLock l(m_csMapFileList);
		MapFileList::iterator it = m_MapFileList.find(ulFileID);
		if (it != m_MapFileList.end())
		{
			DTS_FILE_LIST * pDtsFileList = it->second;
			if (pDtsFileList)
			{
				ulPosition = pDtsFileList->ulPosition;
				ulFromSessionID = pDtsFileList->ulFromSessionID;
				ulFileLength = pDtsFileList->ulFileLength;

				if (nReFresh == 1)
				{
					ulPosition = 0;
					pDtsFileList->ulPosition = 0;
					char szPath[255] = {0};
					sprintf(szPath,"%s\\%u.dts",GetFileTemPath(),ulFileID);	
					remove(szPath);
				}
				
				if (ulPosition == 0)
				{
					//更新文件保存目录
					strcpy(pDtsFileList->chFilePath , GetFileSavePath());
				}	

				bFind = true;
			}
		}
	}

	if(bFind)
	{
		if (ulPosition == ulFileLength)
		{
			//文件下载完毕
			AVDataOutPacket outPacket;				
			outPacket << ulFileID;
			outPacket << (UINT)FILESTATUS_DONELOAD;	
			AVDataInPacket inPkt2((char*)outPacket.GetData(),outPacket.GetLength());
			processUpdateFileStatus(inPkt2,LOCAL_SESSIONID);
		}
		else
		{						
			//发送请求
			RequestDownLoad(ulFromSessionID,ulFileID,ulPosition,ulFileLength);
		}	
	}
}

//---------------------------------------------------------------------------------------
void HPDTS::processOnRequestDownload(AVDataInPacket& in,unsigned long ulSessionID)
{	
	try
	{
		unsigned long	ulFileID = 0;	
		unsigned long ulPosition = 0;
		unsigned long ulFileLength = 0;
		in >> ulFileID >> ulPosition >> ulFileLength;
		

		{
			XAutoLock c(m_csMapPauseUpLoad);
			//检测是否有暂停
			MapPauseUpLoad::iterator it = m_MapPauseUpLoad.find(ulFileID);
			if (it != m_MapPauseUpLoad.end())
			{		
				return;
			}
		}


		{
			XAutoLock c(m_csMapFileList);
			MapFileList::iterator it = m_MapFileList.find(ulFileID);
			if (it == m_MapFileList.end())
			{
				return;
			}
		}

		char pPath[MAX_PATH] = {0};	

		if (m_bUINotify)
		{
			sprintf(pPath,"%s",m_MapSendFilePath[ulFileID].c_str());	
		}
		else
		{
#ifdef WIN32
			sprintf(pPath,"%s\\%u.dts",GetFileTemPath(),ulFileID);	
#else
			sprintf(pPath,"%s/%u.dts",GetFileTemPath(),ulFileID);	
#endif
		}
		
		
		bool bOpen = false;
		bool bWaite = false;
		bool bSeekError = false;
		int nDataLen = 0;
		char* pBuffer = NULL;

		if (strlen(pPath) == 0)
		{
			return;
		}

		{
			XAutoLock lr(m_csReadWritFile);
			FILE*	fp = fopen(pPath, "rb");
			if	(fp)
			{
				bOpen = true;
				fseek(fp, 0, SEEK_END);
				DWORD dwSize = ftell(fp);
				if(ulPosition < dwSize)      //发送文件数据
				{		
					nDataLen = ((dwSize-ulPosition)>FILE_TRANS_MAXLENGTH?FILE_TRANS_MAXLENGTH:(dwSize-ulPosition));
					pBuffer = new char[nDataLen];
					int nRes = fseek(fp,ulPosition,SEEK_SET);
					if (!nRes)
					{
						fread(pBuffer, 1, nDataLen, fp);
					}
					else
					{
						bSeekError = true;
						delete[] pBuffer;
						pBuffer = NULL;	
					}
				}
				else
				{
					bWaite = true;
				}
				fclose(fp);
			}
		}
		

		if	(bOpen)
		{
			if (!bWaite)
			{
				if (!bSeekError)
				{
					AVDataOutPacket outPacket(true,nDataLen + 1024);
					outPacket << (UINT)FILE_ONDOWNLOAD_DATA;			
					outPacket << ulFileID;	
					outPacket << ulPosition;	
					outPacket << ulFileLength;
					outPacket << (UINT)nDataLen;	
					outPacket.WriteData((unsigned char*)pBuffer,nDataLen);

					m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());

					delete[] pBuffer;
					pBuffer = NULL;	

					//客户端回调上传完毕信息
					if (m_bUINotify)
					{	
						{
							//更新上传位置										
							XAutoLock l(m_csMapFileList);
							MapFileList::iterator it = m_MapFileList.find(ulFileID);
							if (it != m_MapFileList.end())
							{
								DTS_FILE_LIST * pDtsFileList = it->second;
								if (pDtsFileList)
								{
									pDtsFileList->ulPosition = ulPosition + nDataLen;
								}
							}
						}

						if (ulPosition + nDataLen >= ulFileLength)
						{
							AVDataOutPacket outPacket;	
							outPacket << ulFileID;
							outPacket << (UINT)FILESTATUS_DONEUP;	
							AVDataInPacket inPkt2((char*)outPacket.GetData(),outPacket.GetLength());
							processUpdateFileStatus(inPkt2,LOCAL_SESSIONID);
						}
					}
				}
				else
				{
					WaitAutoPause(ulSessionID,ulFileID,ulPosition);
				}
			}			
			else if (ulPosition < ulFileLength) //文件数据还未上传完毕,等待上传
			{			
				WaitAutoPause(ulSessionID,ulFileID,ulPosition);
			}			
		}
		else if (!m_bUINotify)  //文件不在此DTS上
		{
			bool bHost = false;
			unsigned long ulSId = 0;
			{
				XAutoLock l(m_csMapSessionID);
				MapSessionID::iterator it = m_MapSessionID.begin();
				while(it != m_MapSessionID.end()) 
				{
					bHost = (*it).second;
					if (bHost)
					{
						ulSId = (*it).first;						
						break;
					}
					it++;
				}
			}

			if (bHost)
			{
				RequestDownLoad(ulSId,ulFileID,ulPosition,ulFileLength);
			}

			//文件数据还未上传完毕,等待上传
			WaitAutoPause(ulSessionID,ulFileID,ulPosition);
		}
		else   //上传文件错误
		{
			AVDataOutPacket outPacket;
			outPacket << (UINT)FILE_UPLOAD_ERROR;			
			outPacket << ulFileID;
			m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
		}
	}
	catch (...)
	{
		TRACE(TEXT("catch void HPDTS::processOnRequestDownload \n"));
	}
}

//---------------------------------------------------------------------------------------
void HPDTS::processOnDownloadData(AVDataInPacket& in,unsigned long ulSessionID)
{	
	try
	{
		unsigned long	ulFileID = 0;	
		unsigned long ulPosition = 0;
		unsigned long ulFileLength = 0;
		UINT nDataLen = 0;
		in >> ulFileID >> ulPosition >> ulFileLength >> nDataLen;
		
		{
			XAutoLock l(m_csMapFileList);
			MapFileList::iterator it = m_MapFileList.find(ulFileID);
			if (it == m_MapFileList.end())
			{
				return;
			}
		}

#ifdef WIN32
		//LOG_WRITE(1, "HPDTS::processOnDownloadData", __FILE__, __LINE__, "ulPosition: %u   ulFileLength: %u ",ulPosition,ulFileLength);
#endif
		unsigned char* pData = NULL;
		pData = in.ReadData((int&)nDataLen);

		char pPath[MAX_PATH] = {0};	

#ifdef WIN32
		sprintf(pPath,"%s\\%u.dts",GetFileTemPath(),ulFileID);
#else
		sprintf(pPath,"%s/%u.dts",GetFileTemPath(),ulFileID);
#endif

		bool bOpenFile = false;
		
		{
			XAutoLock lr(m_csReadWritFile);

			FILE*	fp = NULL;
			if(ulPosition == 0)
				fp = fopen(pPath, "wb");
			else
				fp = fopen(pPath, "r+b");

			if	(fp)
			{
				bOpenFile = true;
				fseek(fp, 0, SEEK_END);
				unsigned long nSize = ftell(fp);
				if (ulPosition != nSize || ulPosition + nDataLen > ulFileLength)
				{					
#ifdef WIN32
					//if (ulPosition != nSize)
					//{
					//	LOG_WRITE(1, "FILE_DOWNLOAD_ERROR ", __FILE__, __LINE__, "ulPosition: %u != nSize: %u ulFileLength = %u",ulPosition,nSize,ulFileLength);
					//}
					//else
					//{
					//	LOG_WRITE(1, "FILE_DOWNLOAD_ERROR ", __FILE__, __LINE__, "ulPosition: %u  > ulFileLength: %u ",(ulPosition + nDataLen),ulFileLength);
					//}
#endif
					fclose(fp);					
					return;
				}

				fseek(fp, ulPosition, SEEK_SET);
				fwrite(pData, 1, nDataLen, fp);	
				fflush(fp);
				fclose(fp);
			}
		}

		if (bOpenFile)
		{
			{
				//更新下载位置
				XAutoLock l(m_csMapFileList);
				MapFileList::iterator it = m_MapFileList.find(ulFileID);
				if (it != m_MapFileList.end())
				{
					DTS_FILE_LIST * pDtsFileList = it->second;
					if (pDtsFileList)
					{
						pDtsFileList->ulPosition = ulPosition + nDataLen;
					}
				}
			}
		}
		else
		{
			if (m_bUINotify)  //回调下载文件错误
			{
				AVDataOutPacket outPacket;
				outPacket << (UINT)FILE_DOWNLOAD_ERROR;			
				outPacket << ulFileID;
				m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());

#ifdef WIN32
				//LOG_WRITE(1, "FILE_DOWNLOAD_ERROR ", __FILE__, __LINE__, "fopen failed");
				//remove(pPath);
#endif
			}
			return;
		}

		{
			//检测自动暂停下载			
			bool bMapAutoPause = false;
			{
				XAutoLock c(m_csMapAutoPause);
				bMapAutoPause = (m_MapAutoPause.size() > 0);
			}

			if (bMapAutoPause)
			{
				std::map<unsigned long,unsigned long> mapTem;
				{
					XAutoLock l(m_csMapSessionID);
					MapSessionID::iterator it = m_MapSessionID.begin();	
					while(it != m_MapSessionID.end()) 
					{
						unsigned long ulSessionID = (*it).first;
						mapTem[ulSessionID] = ulSessionID;
						it++;
					}
				}
				std::map<unsigned long,unsigned long>::iterator itMap = mapTem.begin();
				while (itMap != mapTem.end())
				{
					unsigned long ulSessionID = (*itMap).first;

					char shSession[128] = {0};
					sprintf(shSession,"%u%u",ulFileID,ulSessionID);
					std::string strSession = shSession;//UL2STR(ulFileID) + UL2STR(ulSessionID);	

					bool bRet = false;
					AVDataOutPacket outPacket;
					{
						XAutoLock c(m_csMapAutoPause);
						MapAutoPause::iterator itTrans = m_MapAutoPause.find(strSession);
						if (itTrans != m_MapAutoPause.end())
						{
							unsigned long ulPos = (*itTrans).second;
							if ((ulFileLength > (ulPos + FILE_TRANS_MAXLENGTH * FILE_TRANS_PACKETCOUNT * 2) && ((ulPosition + nDataLen) >= (ulPos + FILE_TRANS_MAXLENGTH * FILE_TRANS_PACKETCOUNT * 2)))
								|| (ulFileLength <= (ulPos + FILE_TRANS_MAXLENGTH * FILE_TRANS_PACKETCOUNT * 2) && ((ulPosition + nDataLen) == ulFileLength)))
							{
								bRet = true;
								outPacket << (UINT)FILE_AUTO_DOWNLOAD;			
								outPacket << ulFileID;
								m_MapAutoPause.erase(strSession);
							}				
						}
					}
					if (bRet)
					{
						m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());
					}					
					itMap++;	
				}
				mapTem.clear();	
			}
		}
	

		if ( ulPosition + nDataLen < ulFileLength)
		{
			{
				//检测是否有暂停
				XAutoLock c(m_csMapPauseDownLoad);
				MapPauseDownLoad::iterator it = m_MapPauseDownLoad.find(ulFileID);
				if (it != m_MapPauseDownLoad.end())
				{
					m_MapPauseDownLoad[ulFileID] = ulPosition + nDataLen;
					return;
				}
			}

			if ((ulPosition + FILE_TRANS_MAXLENGTH * FILE_TRANS_PACKETCOUNT) < ulFileLength)
			{
				//请求新的文件包 (3个长度后的包)
				AVDataOutPacket outPacket;
				outPacket << (UINT)FILE_ONREQUEST_DOWNLOAD;			
				outPacket << ulFileID;		
				outPacket << (unsigned long)(ulPosition + FILE_TRANS_MAXLENGTH * FILE_TRANS_PACKETCOUNT);
				outPacket << ulFileLength;
				m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());

#ifdef WIN32
				//LOG_WRITE(1, "FILE_ONREQUEST_DOWNLOAD", __FILE__, __LINE__, "ulPosition: %u   ulFileLength: %u ",(ulPosition + FILE_TRANS_MAXLENGTH * FILE_TRANS_PACKETCOUNT),ulFileLength);
#endif
	
			}		
		}
		else 
		{
			if (m_bUINotify)
			{				
				//文件下载完毕
				AVDataOutPacket outPacket;				
				outPacket << ulFileID;
				outPacket << (UINT)FILESTATUS_DONELOAD;	
				AVDataInPacket inPkt2((char*)outPacket.GetData(),outPacket.GetLength());
				processUpdateFileStatus(inPkt2,LOCAL_SESSIONID);
			}
			std::map<std::string,std::string> mapTem;
			{
				//清理自动暂停表
				XAutoLock l(m_csMapSessionID);
				MapSessionID::iterator it = m_MapSessionID.begin();		
				while(it != m_MapSessionID.end()) 
				{
					unsigned long ulSessionID = (*it).first;
					char shSession[128] = {0};
					sprintf(shSession,"%u%u",ulFileID,ulSessionID);
					std::string strSession = shSession;//UL2STR(ulFileID) + UL2STR(ulSessionID);			
					mapTem[strSession] = strSession;					
					it++;
				}
			}
			
			{
				XAutoLock c(m_csMapAutoPause);
				std::map<std::string,std::string>::iterator itMap = mapTem.begin();
				while (itMap != mapTem.end())
				{
					std::string strSession = (*itMap).first;					
					m_MapAutoPause.erase(strSession);
					itMap++;
				}
				mapTem.clear();	
			}
		}
	}
	catch (...)
	{
		TRACE(TEXT("catch void HPDTS::processOnDownloadData \n"));
	}
}

//---------------------------------------------------------------------------------------
void HPDTS::processAutoDownload(AVDataInPacket& in,unsigned long ulSessionID)
{	
	unsigned long	ulFileID = 0;	
	in >> ulFileID;

	//检测是否有暂停
	{
		XAutoLock c(m_csMapPauseDownLoad);
		MapPauseDownLoad::iterator it = m_MapPauseDownLoad.find(ulFileID);
		if (it != m_MapPauseDownLoad.end())
		{		
			return;
		}
	}
	unsigned long ulPosition;
	unsigned long ulFileLength;
	bool bFind = false;
	
	{
		XAutoLock l(m_csMapFileList);
		MapFileList::iterator it = m_MapFileList.find(ulFileID);
		if (it != m_MapFileList.end())
		{
			DTS_FILE_LIST * pDtsFileList = it->second;
			if (pDtsFileList)
			{
				ulPosition = pDtsFileList->ulPosition;			
				ulFileLength = pDtsFileList->ulFileLength;	
				bFind = true;
			}			
		}
	}
		
	if (bFind)
	{
#ifdef WIN32
		//LOG_WRITE(1, "HPDTS::processAutoDownload", __FILE__, __LINE__, "ulPosition: %u   ulFileLength: %u ",ulPosition,ulFileLength);
#endif
		RequestDownLoad(ulSessionID,ulFileID,ulPosition,ulFileLength);

		if (m_bUINotify)
		{
			AVDataOutPacket outPacket;				
			outPacket << ulFileID;
			outPacket << (UINT)FILESTATUS_TRANSMIT;	
			AVDataInPacket inPkt2((char*)outPacket.GetData(),outPacket.GetLength());
			processUpdateFileStatus(inPkt2,LOCAL_SESSIONID);
		}
	}	
}

//---------------------------------------------------------------------------------------
void HPDTS::processContinueDownload(AVDataInPacket& in)
{	
	unsigned long ulFileID = 0;	
	in >> ulFileID;	

	unsigned long ulPosition;
	unsigned long ulFromSessionID;
	unsigned long ulFileLength;
	bool bFind = false;

	bool bPauseDownLoad = false;
	{
		XAutoLock c(m_csMapPauseDownLoad);
		MapPauseDownLoad::iterator it = m_MapPauseDownLoad.find(ulFileID);
		if (it != m_MapPauseDownLoad.end())
		{
			bPauseDownLoad = true;
			m_MapPauseDownLoad.erase(ulFileID);
		}
	}
	if (bPauseDownLoad)
	{
		XAutoLock l(m_csMapFileList);
		MapFileList::iterator it = m_MapFileList.find(ulFileID);
		if (it != m_MapFileList.end())
		{
			DTS_FILE_LIST * pDtsFileList = it->second;
			if (pDtsFileList)
			{
				bFind = true;
				ulPosition = pDtsFileList->ulPosition;
				ulFromSessionID = pDtsFileList->ulFromSessionID;
				ulFileLength = pDtsFileList->ulFileLength;							
			}
		}
	}

	if (bFind)
	{
		AVDataOutPacket outPkt;			
		outPkt << ulFileID;		
		outPkt << ulPosition;
		outPkt << ulFileLength;		
		AVDataInPacket inPkt((char*)outPkt.GetData(),outPkt.GetLength());

		RequestDownLoad(ulFromSessionID,ulFileID,ulPosition,ulFileLength);		
	}


}

//---------------------------------------------------------------------------------------
void HPDTS::processPauseDownload(AVDataInPacket& in)
{	
	unsigned long ulFileID = 0;	
	in >> ulFileID;	

	XAutoLock c(m_csMapPauseDownLoad);
	m_MapPauseDownLoad[ulFileID] = 1;
}

//---------------------------------------------------------------------------------------
void HPDTS::processContinueUpload(AVDataInPacket& in)
{
	unsigned long ulFileID = 0;	
	in >> ulFileID;	
	bool bRet = false;
	{
		XAutoLock c(m_csMapPauseUpLoad);
		MapPauseUpLoad::iterator it = m_MapPauseUpLoad.find(ulFileID);
		if (it != m_MapPauseUpLoad.end())
		{
			bRet = true;
			m_MapPauseUpLoad.erase(ulFileID);
		}
	}

	if (bRet)
	{	
		std::map<unsigned long,unsigned long> mapTem;

		{
			XAutoLock l(m_csMapSessionID);
			MapSessionID::iterator itSession = m_MapSessionID.begin();
			while(itSession != m_MapSessionID.end()) 
			{
				unsigned long ulSessionID = (*itSession).first;			
				mapTem[ulSessionID] = ulSessionID;
				itSession++;
			}
		}

		std::map<unsigned long,unsigned long>::iterator itMap = mapTem.begin();
		while (itMap != mapTem.end())
		{
			unsigned long ulSessionID = (*itMap).first;
			AVDataOutPacket outPacket;
			outPacket << (UINT)FILE_AUTO_DOWNLOAD;			
			outPacket << ulFileID;					
			m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());	
			itMap ++;
		}
		mapTem.clear();	
	}	
}

//---------------------------------------------------------------------------------------
void HPDTS::processPauseUpload(AVDataInPacket& in)
{	
	unsigned long ulFileID = 0;	
	in >> ulFileID;	

	XAutoLock c(m_csMapPauseUpLoad);
	m_MapPauseUpLoad[ulFileID] = 1;
}

//---------------------------------------------------------------------------------------
void HPDTS::processGetFileInfo(AVDataInPacket& in)
{	
	unsigned long ulFileID = 0;	
	in >> ulFileID;	
	AVDataOutPacket outPacket;
	bool bRet = false;
	{
		XAutoLock l(m_csMapFileList);
		MapFileList::iterator it = m_MapFileList.find(ulFileID);
		if (it != m_MapFileList.end())
		{
			DTS_FILE_LIST * pDtsFileList = it->second;
			if (pDtsFileList)
			{
				outPacket << (UINT)FILE_ONGET_FILEINFO;
				outPacket << pDtsFileList->ulFileID;
				outPacket << pDtsFileList->ulFileLength;
				outPacket << pDtsFileList->ulPosition;
				outPacket << pDtsFileList->ulFromSessionID;				
				outPacket << pDtsFileList->chSendUserID;
				outPacket << pDtsFileList->chFileName;
				outPacket << pDtsFileList->chFilePath;
				outPacket << pDtsFileList->chDescription;
				outPacket << pDtsFileList->chStartTime;
				bRet = true;
			}
		}
	}
	if (bRet)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());		
	}	
}

//---------------------------------------------------------------------------------------
void HPDTS::processRequestFileList(AVDataInPacket& in)
{	
	AVDataOutPacket outPacket;
	outPacket << (UINT)FILE_GET_FILELIST;
	//命令发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),LOCAL_SESSIONID);
}

//---------------------------------------------------------------------------------------
void HPDTS::processGetPosition(AVDataInPacket& in)
{
	unsigned long	ulFileID = 0;
	in >> ulFileID;
	bool bRet = false;
	AVDataOutPacket outPacket;
	{
		XAutoLock l(m_csMapFileList);
		MapFileList::iterator it = m_MapFileList.find(ulFileID);
		if (it != m_MapFileList.end())
		{
			DTS_FILE_LIST * pDtsFileList = it->second;
			if (pDtsFileList)
			{
				unsigned long ulPosition = pDtsFileList->ulPosition;
				outPacket << (UINT)FILE_ONGET_POSITION;			
				outPacket << ulFileID;	
				outPacket << ulPosition;	
				bRet = true;			
			}
		}
	}
	if (bRet)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
	}
}

//---------------------------------------------------------------------------------------

void HPDTS::processFSAddObj(AVDataInPacket& in,unsigned long ulSessionID)
{
	unsigned long	ulObjId = 0;
	UINT nPage = 0;
	UINT nDataLen = 0;
	
	in >> ulObjId >> nPage >> nDataLen;
	unsigned char* pData = NULL;
	pData = in.ReadData((int&)nDataLen);
	
	AVDataOutPacket outTem,outTem2;	
	
	AVDataInPacket in2((char*)pData,nDataLen);
	UINT nPenType = 0;
	unsigned long color = 0;
	UINT nCount = 0;
	in2 >> nPenType >> color >> nCount;
	UINT x,y;		
	outTem << nPenType;
	outTem << color;
	outTem << nCount;

	outTem2 << nPenType;
	outTem2 << color;
	outTem2 << nCount;
	for(int i = 0;i<nCount;i++)	
	{
		in2 >> x >> y;
		POINT pt;
		pt.x = x;
		pt.y = y;
		POINT ptFile = ClientPoint2File(pt);	
		outTem << (UINT)ptFile.x;
		outTem << (UINT)ptFile.y;

		POINT ptClient = FilePoint2Client(pt);	
		outTem2 << (UINT)ptClient.x;
		outTem2 << (UINT)ptClient.y;
	}

	{
		//存储数据
		XAutoLock l(m_csMapObjects);	
		MapObjects::iterator it = m_MapObjects.find(ulObjId);
		if(it == m_MapObjects.end())
		{
			AVDataOutPacket outPacket;	
			outPacket << nPage;
			if (ulSessionID == LOCAL_SESSIONID)
			{
				outPacket << (UINT)outTem.GetLength();	
				outPacket.WriteData(outTem.GetData(),outTem.GetLength());
			}
			else
			{			
				outPacket <<nDataLen;	
				outPacket.WriteData(pData,nDataLen);
			}		
			char * data = new char[outPacket.GetLength()];
			memcpy(data,(char *)outPacket.GetData(),outPacket.GetLength());
			OBJ_DATA_BUF objData;
			objData.nLen = outPacket.GetLength();
			objData.pData = data;			
			m_MapObjects[ulObjId] = objData;
		}
	}


	AVDataOutPacket outPacket;
	outPacket << (UINT)FS_ADD_OBJ;
	outPacket << ulObjId;
	outPacket << nPage;
	if (ulSessionID == LOCAL_SESSIONID)
	{
		outPacket << (UINT)outTem.GetLength();	
		outPacket.WriteData(outTem.GetData(),outTem.GetLength());
	}
	else
	{			
		outPacket <<nDataLen;	
		outPacket.WriteData(pData,nDataLen);
	}
	//发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);

	//回调界面
	if (m_bUINotify)
	{
		AVDataOutPacket outPacket;
		outPacket << (UINT)FS_ADD_OBJ;
		outPacket << ulObjId;
		outPacket << nPage;
		if (ulSessionID == LOCAL_SESSIONID)
		{
			outPacket <<nDataLen;	
			outPacket.WriteData(pData,nDataLen);	
		}
		else
		{			
			outPacket << (UINT)outTem2.GetLength();	
			outPacket.WriteData(outTem2.GetData(),outTem2.GetLength());
		}	
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
	}
}

//---------------------------------------------------------------------------------------

void HPDTS::processFSDeleteObj(AVDataInPacket& in,unsigned long ulSessionID)
{
	unsigned long	ulObjId = 0;	
	
	in >> ulObjId;
	
	AVDataOutPacket outPacket;
	outPacket << (UINT)FS_DELETE_OBJ;
	outPacket << ulObjId;	
	//发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);
	//回调界面
	if (m_bUINotify)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
	}

	{
		//删除数据
		XAutoLock l(m_csMapObjects);	
		MapObjects::iterator it = m_MapObjects.find(ulObjId);
		if(it != m_MapObjects.end())
		{
			OBJ_DATA_BUF objData = (*it).second;
			if (objData.pData)
			{
				ASSERT(objData.pData);
				delete objData.pData;
				objData.pData = NULL;
			}
		}
		m_MapObjects.erase(ulObjId);
	}

}

//---------------------------------------------------------------------------------------
void HPDTS::processFSChangePage(AVDataInPacket& in,unsigned long ulSessionID)
{
	UINT nCurPage = 0;	
	in >> nCurPage;


	AVDataOutPacket outPacket;
	outPacket << (UINT)FS_CHANGE_PAGE;
	outPacket << nCurPage;	
	//发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);

	//回调界面
	if (m_bUINotify)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
		
		//取当前页对象数据
		{
			AVDataOutPacket outPacket;		
			outPacket << nCurPage;				
			AVDataInPacket in((char *)outPacket.GetData(),outPacket.GetLength());
			processFSGetCurPageObj(in);
		}
	}

	m_dtsTemData.nCurPage = nCurPage;
}

//---------------------------------------------------------------------------------------
void HPDTS::processFSGetCurPageObj(AVDataInPacket& in)
{
	UINT nCurPage = 0;	
	in >> nCurPage;

	AVDataOutPacket outPacket2;

	{		
		XAutoLock l(m_csMapObjects);
		outPacket2 << (UINT)FS_ON_CURPAGEOBJ;
		for(MapObjects::iterator it = m_MapObjects.begin(); it != m_MapObjects.end(); it++)
		{
			OBJ_DATA_BUF objData = (*it).second;
			if (objData.pData)
			{
				ASSERT(objData.pData);			
				AVDataInPacket in((char*)objData.pData,objData.nLen);		
				unsigned long	ulObjId = (*it).first;
				UINT nPage = 0;
				UINT nDataLen = 0;
				in >> nPage >> nDataLen;
				//取当前页对象数据
				if (nPage == nCurPage)
				{
					unsigned char* pData = NULL;
					pData = in.ReadData((int&)nDataLen);
					AVDataOutPacket outPacket;
					//outPacket << (UINT)FS_ADD_OBJ;
					outPacket << ulObjId;
					outPacket << nPage;

					AVDataOutPacket outTem;
					AVDataInPacket in2((char*)pData,nDataLen);
					UINT nPenType = 0;
					unsigned long color = 0;
					UINT nCount = 0;
					in2 >> nPenType >> color >> nCount;
					UINT x,y;		
					outTem << nPenType;
					outTem << color;
					outTem << nCount;
					for(int i = 0;i<nCount;i++)	
					{
						in2 >> x >> y;
						POINT pt;
						pt.x = x;
						pt.y = y;
						POINT ptClient = FilePoint2Client(pt);	
						outTem << (UINT)ptClient.x;
						outTem << (UINT)ptClient.y;
					}
					outPacket <<(UINT)outTem.GetLength();	
					outPacket.WriteData(outTem.GetData(),outTem.GetLength());
					//回调界面

					outPacket2 << (UINT)outPacket.GetLength();
					outPacket2.WriteData(outPacket.GetData(),outPacket.GetLength());				
				}
			}
		}
		outPacket2 << (UINT)0;
	}
	
	m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket2.GetData(),outPacket2.GetLength());	
}

//---------------------------------------------------------------------------------------
void HPDTS::processFSDeleteCurPageObj(AVDataInPacket& in)
{
	UINT nCurPage = 0;	
	in >> nCurPage;
	
	bool bFind = true;
	while (bFind)
	{
		bFind = false;
		unsigned long	ulObjId = 0;
		AVDataOutPacket outPacket;

		{
			XAutoLock l(m_csMapObjects);	
			for(MapObjects::iterator it = m_MapObjects.begin(); it != m_MapObjects.end(); it++)
			{
				OBJ_DATA_BUF objData = (*it).second;
				if (objData.pData)
				{
					ASSERT(objData.pData);			
					AVDataInPacket in((char*)objData.pData,objData.nLen);		
					ulObjId = (*it).first;
					UINT nPage = 0;
					UINT nDataLen = 0;
					in >> nPage >> nDataLen;
					//取当前页对象数据
					if (nPage == nCurPage)
					{
						bFind = true;						
						outPacket << (UINT)FS_DELETE_OBJ;
						outPacket << ulObjId;							
						ASSERT(objData.pData);
						delete objData.pData;
						objData.pData = NULL;
						m_MapObjects.erase(ulObjId);
						break;
					}
				}
			}
		}
		if (bFind)
		{			
			//发送给所以连接
			SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),LOCAL_SESSIONID);						
			m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());			
		}
	}
}

//---------------------------------------------------------------------------------------
void HPDTS::processFSRequestAllObj(AVDataInPacket& in,unsigned long ulSessionID)
{
	if (ulSessionID == LOCAL_SESSIONID)
	{
		AVDataOutPacket outPacket;
		outPacket << (UINT)FS_REQUEST_ALLOBJ;	
		//发送给所以连接
		SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);
	}
	else
	{
		if (m_MapObjects.size() == 0)
		{
			AVDataOutPacket outPacket;
			outPacket << (UINT)FS_REQUEST_ALLOBJ;	
			//发送给所以连接
			SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);
			return;
		}
		XBufferPool tXBufferPool;
		{
			XAutoLock l(m_csMapObjects);	
			for(MapObjects::iterator it = m_MapObjects.begin(); it != m_MapObjects.end(); it++)
			{
				OBJ_DATA_BUF objData = (*it).second;
				if (objData.pData)
				{
					ASSERT(objData.pData);			
					AVDataInPacket in((char*)objData.pData,objData.nLen);		
					unsigned long	ulObjId = (*it).first;
					UINT nPage = 0;
					UINT nDataLen = 0;
					in >> nPage >> nDataLen;				
					unsigned char* pData = NULL;
					pData = in.ReadData((int&)nDataLen);
					AVDataOutPacket outPacket;
					outPacket << (UINT)FS_ADD_OBJ;
					outPacket << ulObjId;
					outPacket << nPage;
					outPacket << nDataLen;	
					outPacket.WriteData(pData,nDataLen);				

					XDataBuffer*pXDataBuffer=new XDataBuffer();
					pXDataBuffer->StoreData((char *)outPacket.GetData(),outPacket.GetLength());
					tXBufferPool.SetDataBuffer(pXDataBuffer);
				}
			}
		}
		
		while(tXBufferPool.GetCount())
		{
			XDataBuffer*pXDataBuffer=tXBufferPool.GetDataBuffer();
			m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,pXDataBuffer->GetData(),pXDataBuffer->GetLen());	
			delete pXDataBuffer;
			pXDataBuffer=NULL;
		}

		//当前页
		if (m_dtsTemData.nCurPage != 0)
		{
			AVDataOutPacket outPacket;
			outPacket << (UINT)FS_CHANGE_PAGE;
			outPacket << m_dtsTemData.nCurPage;	
			m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());
		}
		if (m_dtsTemData.nDocZoomSize != 1)
		{
			AVDataOutPacket outPacket;
			outPacket << (UINT)FS_SET_ZOOM;
			outPacket << m_dtsTemData.nDocZoomSize;	
			m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());
		}
		if (m_dtsTemData.nDocLeft > 0 || m_dtsTemData.nDocTop > 0 )
		{
			AVDataOutPacket outPacket;
			outPacket << (UINT)FS_MOVE_POS;
			outPacket << m_dtsTemData.nDocLeft;	
			outPacket << m_dtsTemData.nDocTop;	
			m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());
		}
	}
}

//---------------------------------------------------------------------------------------
void HPDTS::processFSSetFileRect(AVDataInPacket& in)
{
	UINT x1 = 0;
	UINT y1 = 0;
	UINT x2 = 0;
	UINT y2 = 0;
	in >> x1 >> y1 >> x2 >> y2;
	m_dtsTemData.ptFile.x = x1;
	m_dtsTemData.ptFile.y = y1;
	m_dtsTemData.ptClient.x = x2;
	m_dtsTemData.ptClient.y = y2;
}
void HPDTS::processFlashPlay(AVDataInPacket& in,unsigned long ulSessionID)
{
	UINT nFlashPosition = 0;
	in >> nFlashPosition;	
	m_dtsTemData.nFlashPosition = nFlashPosition;
	m_dtsTemData.nFlashStatus = 1;
	AVDataOutPacket outPacket;
	outPacket << (UINT)FLASH_SHARE_PLAY;	
	outPacket << nFlashPosition;
	//发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);
	if (m_bUINotify && ulSessionID != LOCAL_SESSIONID)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
	}
}
void HPDTS::processFlashStop(AVDataInPacket& in,unsigned long ulSessionID)
{
	UINT nFlashPosition = 0;
	in >> nFlashPosition;	
	m_dtsTemData.nFlashPosition = nFlashPosition;
	m_dtsTemData.nFlashStatus = 0;
	AVDataOutPacket outPacket;
	outPacket << (UINT)FLASH_SHARE_STOP;	
	outPacket << nFlashPosition;
	//发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);
	if (m_bUINotify && ulSessionID != LOCAL_SESSIONID)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
	}
}	
void HPDTS::processFlashPause(AVDataInPacket& in,unsigned long ulSessionID)
{	
	m_dtsTemData.nFlashStatus = 0;
	AVDataOutPacket outPacket;
	outPacket << (UINT)FLASH_SHARE_PAUSE;		
	//发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);
	if (m_bUINotify && ulSessionID != LOCAL_SESSIONID)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
	}
}
void HPDTS::processFlashPosition(AVDataInPacket& in,unsigned long ulSessionID)
{	
	UINT nFlashPosition = 0;
	in >> nFlashPosition;	
	m_dtsTemData.nFlashPosition = nFlashPosition;

	AVDataOutPacket outPacket;
	outPacket << (UINT)FLASH_SHARE_POSITION;
	outPacket << nFlashPosition;
	//发送给所以连接	
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);
}

void HPDTS::processFSZoom(AVDataInPacket& in,unsigned long ulSessionID)
{	
	UINT nZoomSize = 1;
	in >> nZoomSize;

	m_dtsTemData.nDocZoomSize = nZoomSize;

	AVDataOutPacket outPacket;
	outPacket << (UINT)FS_SET_ZOOM;
	outPacket << nZoomSize;
	//发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);

	if (m_bUINotify && ulSessionID != LOCAL_SESSIONID)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
	}
}

void HPDTS::processFSGetPos(AVDataInPacket& in,unsigned long ulSessionID)
{
	POINT pt;
	pt.x = m_dtsTemData.nDocLeft;
	pt.y = m_dtsTemData.nDocTop;
	POINT ptFile = FilePoint2Client(pt);	

	AVDataOutPacket outPacket;
	outPacket << (UINT)FS_MOVE_POS;
	outPacket << (UINT)ptFile.x;
	outPacket << (UINT)ptFile.y;

	m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
}

void HPDTS::processFSMovePos(AVDataInPacket& in,unsigned long ulSessionID)
{	
	UINT nDocLeft = 0;
	UINT nDocTop = 0;
	in >> nDocLeft >> nDocTop;	

	if (ulSessionID == LOCAL_SESSIONID)
	{
		POINT pt;
		pt.x = nDocLeft;
		pt.y = nDocTop;
		POINT ptFile = ClientPoint2File(pt);	
		nDocLeft = ptFile.x;
		nDocTop = ptFile.y;
	}	

	m_dtsTemData.nDocLeft = nDocLeft;
	m_dtsTemData.nDocTop = nDocTop;

	AVDataOutPacket outPacket;
	outPacket << (UINT)FS_MOVE_POS;
	outPacket << nDocLeft;
	outPacket << nDocTop;
	//发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);

	if (m_bUINotify && ulSessionID != LOCAL_SESSIONID)
	{
		POINT pt;
		pt.x = nDocLeft;
		pt.y = nDocTop;
		POINT ptFile = FilePoint2Client(pt);	
		nDocLeft = ptFile.x;
		nDocTop = ptFile.y;

		AVDataOutPacket outPacket2;
		outPacket2 << (UINT)FS_MOVE_POS;
		outPacket2 << nDocLeft;
		outPacket2 << nDocTop;

		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket2.GetData(),outPacket2.GetLength());
	}
}

void HPDTS::processFlashSeek(AVDataInPacket& in,unsigned long ulSessionID)
{	
	UINT nFlashPosition = 0;
	in >> nFlashPosition;	
	m_dtsTemData.nFlashPosition = nFlashPosition;

	AVDataOutPacket outPacket;
	outPacket << (UINT)FLASH_SHARE_SEEK;
	outPacket << nFlashPosition;
	//发送给所以连接
	SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);

	if (m_bUINotify && ulSessionID != LOCAL_SESSIONID)
	{
		m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
	}
}

void HPDTS::processFlashRequest(AVDataInPacket& in,unsigned long ulSessionID)
{
	if (ulSessionID == LOCAL_SESSIONID)
	{
		AVDataOutPacket outPacket;
		outPacket << (UINT)FLASH_SHARE_REQUEST;
		//发送给所以连接
		SendCommandToAll((char *)outPacket.GetData(),outPacket.GetLength(),ulSessionID);
	}
	else
	{
		if (m_bUINotify)
		{
			UINT nFlashStatus = 0;
			UINT nFlashPosition = 0;
			in >> nFlashStatus >> nFlashPosition;
			AVDataOutPacket outPacket;
			outPacket << (UINT)FLASH_SHARE_REQUEST;
			outPacket << nFlashStatus;
			outPacket << nFlashPosition;
			m_rIHPDTSNotify.onDTSUIDataNotify((char *)outPacket.GetData(),outPacket.GetLength());
		}
		else
		{
			AVDataOutPacket outPacket;
			outPacket << (UINT)FLASH_SHARE_REQUEST;
			outPacket << m_dtsTemData.nFlashStatus;
			outPacket << m_dtsTemData.nFlashPosition;
			m_rIHPDTSNotify.onDTSNetDataNotify(ulSessionID,(char *)outPacket.GetData(),outPacket.GetLength());
		}		
	}
}

