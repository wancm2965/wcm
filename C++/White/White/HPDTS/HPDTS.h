
	//========================================================================================
	//版权所有：版权所有(C) 2008，上海华平信息技术股份有限公司
	//系统名称：AVCON7
	//作　　者：杜权胜
	//创建日期：2008-3-31
	//功能说明：数据传输服务器内核，包括文件传输，文件分发，文档共享，Flash共享，电子白板等内核模块。
	//


#pragma once

#ifndef MAX_PATH
#define MAX_PATH	4096
#endif

#include <HPDTS/IHPDTS.h>
#include <HPDTS/DTSCmdType.h>

#include "XAutoLock.h"
#include "AVDataPacket.h"
#include "WbDTS.h"
#include "WBDataModel.h"
#include "MeetingDTS.h"
#include <string>
#include <map>

#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;

//---------------------------------------------------------------------------------------
typedef std::map<unsigned long,std::string> MapSendFilePath;		//客户端记录上传文件路径
typedef std::map<std::string,unsigned long> MapAutoPause;			//记录文件自动暂停位置
typedef std::map<unsigned long,unsigned long> MapPauseDownLoad;		//记录文件暂停下载的位置
typedef std::map<unsigned long,unsigned long> MapPauseUpLoad;		//记录文件暂停上传的位置
typedef std::map<unsigned long,bool> MapSessionID;					//存储所有DTS连接 "bool" 变量记录是否为root Server
typedef std::map<unsigned long,DTS_FILE_LIST *> MapFileList;		//存储传输文件的相关信息

typedef std::map<unsigned long,OBJ_DATA_BUF> MapObjects;				//标注对象

//---------------------------------------------------------------------------------------
//将文件列表写入缓存	
#define SAVE_FILE_LIST {\
	XAutoLock l(m_csMapFileList);\
	MapFileList::iterator it = m_MapFileList.find(ulFileID);\
	if (it == m_MapFileList.end()){\
	PDTS_FILE_LIST pDtsFileList = new DTS_FILE_LIST;\
	pDtsFileList->ulFileID = ulFileID;\
	pDtsFileList->ulPosition = 0;\
	pDtsFileList->ulFileLength = ulFileLength;\
	strcpy(pDtsFileList->chSendUserID,chSendUserID);\
	strcpy(pDtsFileList->chFileName , chFileName);\
	strcpy(pDtsFileList->chDescription , chDescription);\
	strcpy(pDtsFileList->chStartTime , chStartTime);\
	strcpy(pDtsFileList->chFilePath , chFilePath);\
	pDtsFileList->ulFromSessionID = ulSessionID;\
	m_MapFileList.insert(MapFileList::value_type(ulFileID,pDtsFileList));} }


//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
class HPDTS 
	: public IHPDTS	
	, public WbDTS
	, public CWBDataModel
	, public MeetingDTS
{
public:
	HPDTS(IHPDTSNotify&rIHPDTSNotify,bool bUI);
	virtual ~HPDTS(void){};

	virtual void Init(void);
	virtual void Release(void);
	virtual void processUIData(const char* pData,int nLen);			//处理界面数据
	virtual void processDTSData(unsigned long ulSessionID,const char* pData,int nLen);	
	virtual void AddDTSSession(unsigned long ulSessionID,bool bHost);
	virtual void RemoveDTSSession(unsigned long ulSessionID);

protected:
				
	bool					m_bUINotify;
	DTS_TEM_DATA			m_dtsTemData;

	MapAutoPause			m_MapAutoPause;
	MapSessionID			m_MapSessionID;	

	MapPauseDownLoad		m_MapPauseDownLoad;
	MapPauseUpLoad			m_MapPauseUpLoad;
	MapSendFilePath			m_MapSendFilePath;
	MapFileList				m_MapFileList;
	MapObjects				m_MapObjects;

	XCritSec				m_csMapPauseUpLoad;
	XCritSec				m_csMapPauseDownLoad;
	XCritSec				m_csMapSessionID;
	XCritSec				m_csMapAutoPause;
	XCritSec				m_csMapFileList;
	XCritSec				m_csMapObjects;
	XCritSec				m_csReadWritFile;

	IHPDTSNotify&			m_rIHPDTSNotify;		

public:
	void processDbPacket(char* pData,int nLen);
	void ClearAllObject(void);	
	int CopyFile( const char *source, const char *dest );
	POINT FilePoint2Client(POINT pt);
	POINT ClientPoint2File(POINT pt);
	void SendCommandToAll(const char* pData,int nLen,unsigned long ulSessionID);
	void WaitAutoPause(unsigned long ulSessionID,unsigned long ulFileID,unsigned long ulPosition);
	void RequestDownLoad(unsigned long ulSessionID,unsigned long ulFileID,unsigned long ulPosition,unsigned long ulFileLength);
	
	//客户端命令处理
	void processSendAddFile(AVDataInPacket& in); //客户端发送文件
	void processSendDeleteFile(AVDataInPacket& in);//客户端发送删除文件
	void processRequestDownload(AVDataInPacket& in);//客户端请求下载文件	
	void processContinueDownload(AVDataInPacket& in);//客户端继续下载
	void processPauseDownload(AVDataInPacket& in);//客户端暂停下载
	void processContinueUpload(AVDataInPacket& in);//客户端继续上传
	void processPauseUpload(AVDataInPacket& in);//客户端暂停上传
	void processGetFileInfo(AVDataInPacket& in);//客户端获取文件信息
	void processGetPosition(AVDataInPacket& in);	//得到文件传输位置
	void processRequestFileList(AVDataInPacket& in);	//取文件列表
	
	//DTS命令处理	
	void processWBCmdData(AVDataInPacket& in,unsigned long ulSessionID);//WB 数据
	void processGetFileList(AVDataInPacket& in,unsigned long ulSessionID);//qu所有文件列表
	void processOnGetFileList(AVDataInPacket& in,unsigned long ulSessionID);//得到所有文件列表
	void processInsertFile(AVDataInPacket& in,unsigned long ulSessionID);//插入文件
	void processUpdateFileStatus(AVDataInPacket& in,unsigned long ulSessionID);//更新文件状态	
	void processDeleteFile(AVDataInPacket& in,unsigned long ulSessionID);//删除文件
	void processOnRequestDownload(AVDataInPacket& in,unsigned long ulSessionID);//收到请求下载文件
	void processOnDownloadData(AVDataInPacket& in,unsigned long ulSessionID);//收到下载文件数据
	void processAutoDownload(AVDataInPacket& in,unsigned long ulSessionID);//自动发送下载文件请求

	//文档共享处理
	void processFSAddObj(AVDataInPacket& in,unsigned long ulSessionID);	//添加标注对象
	void processFSDeleteObj(AVDataInPacket& in,unsigned long ulSessionID);	//删除标注对象
	void processFSChangePage(AVDataInPacket& in,unsigned long ulSessionID);  //文档翻页
	void processFSGetCurPageObj(AVDataInPacket& in);  //得到当前页所有对象
	void processFSRequestAllObj(AVDataInPacket& in,unsigned long ulSessionID);	//请求所有对象
	void processFSDeleteCurPageObj(AVDataInPacket& in);		//删除当前页所有对象
	void processFSSetFileRect(AVDataInPacket& in);		//设置文档实际大小及显示区域大小
	void processFSZoom(AVDataInPacket& in,unsigned long ulSessionID);		
	void processFSMovePos(AVDataInPacket& in,unsigned long ulSessionID);		
	void processFSGetPos(AVDataInPacket& in,unsigned long ulSessionID);	

	//FlashShare 处理
	void processFlashPlay(AVDataInPacket& in,unsigned long ulSessionID); //开始播放Flash	
	void processFlashStop(AVDataInPacket& in,unsigned long ulSessionID);  //停止播放Flash	
	void processFlashPause(AVDataInPacket& in,unsigned long ulSessionID);	//暂停播放Flash
	void processFlashPosition(AVDataInPacket& in,unsigned long ulSessionID);	//更新Flash播放位置
	void processFlashRequest(AVDataInPacket& in,unsigned long ulSessionID);	  //请求Flash播放位置
	void processFlashSeek(AVDataInPacket& in,unsigned long ulSessionID);   //SeekFlash播放位置
};
