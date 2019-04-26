
	//========================================================================================
	//��Ȩ���У���Ȩ����(C) 2008���Ϻ���ƽ��Ϣ�����ɷ����޹�˾
	//ϵͳ���ƣ�AVCON7
	//�������ߣ���Ȩʤ
	//�������ڣ�2008-3-31
	//����˵�������ݴ���������ںˣ������ļ����䣬�ļ��ַ����ĵ�����Flash�������Ӱװ���ں�ģ�顣
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
typedef std::map<unsigned long,std::string> MapSendFilePath;		//�ͻ��˼�¼�ϴ��ļ�·��
typedef std::map<std::string,unsigned long> MapAutoPause;			//��¼�ļ��Զ���ͣλ��
typedef std::map<unsigned long,unsigned long> MapPauseDownLoad;		//��¼�ļ���ͣ���ص�λ��
typedef std::map<unsigned long,unsigned long> MapPauseUpLoad;		//��¼�ļ���ͣ�ϴ���λ��
typedef std::map<unsigned long,bool> MapSessionID;					//�洢����DTS���� "bool" ������¼�Ƿ�Ϊroot Server
typedef std::map<unsigned long,DTS_FILE_LIST *> MapFileList;		//�洢�����ļ��������Ϣ

typedef std::map<unsigned long,OBJ_DATA_BUF> MapObjects;				//��ע����

//---------------------------------------------------------------------------------------
//���ļ��б�д�뻺��	
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
	virtual void processUIData(const char* pData,int nLen);			//�����������
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
	
	//�ͻ��������
	void processSendAddFile(AVDataInPacket& in); //�ͻ��˷����ļ�
	void processSendDeleteFile(AVDataInPacket& in);//�ͻ��˷���ɾ���ļ�
	void processRequestDownload(AVDataInPacket& in);//�ͻ������������ļ�	
	void processContinueDownload(AVDataInPacket& in);//�ͻ��˼�������
	void processPauseDownload(AVDataInPacket& in);//�ͻ�����ͣ����
	void processContinueUpload(AVDataInPacket& in);//�ͻ��˼����ϴ�
	void processPauseUpload(AVDataInPacket& in);//�ͻ�����ͣ�ϴ�
	void processGetFileInfo(AVDataInPacket& in);//�ͻ��˻�ȡ�ļ���Ϣ
	void processGetPosition(AVDataInPacket& in);	//�õ��ļ�����λ��
	void processRequestFileList(AVDataInPacket& in);	//ȡ�ļ��б�
	
	//DTS�����	
	void processWBCmdData(AVDataInPacket& in,unsigned long ulSessionID);//WB ����
	void processGetFileList(AVDataInPacket& in,unsigned long ulSessionID);//qu�����ļ��б�
	void processOnGetFileList(AVDataInPacket& in,unsigned long ulSessionID);//�õ������ļ��б�
	void processInsertFile(AVDataInPacket& in,unsigned long ulSessionID);//�����ļ�
	void processUpdateFileStatus(AVDataInPacket& in,unsigned long ulSessionID);//�����ļ�״̬	
	void processDeleteFile(AVDataInPacket& in,unsigned long ulSessionID);//ɾ���ļ�
	void processOnRequestDownload(AVDataInPacket& in,unsigned long ulSessionID);//�յ����������ļ�
	void processOnDownloadData(AVDataInPacket& in,unsigned long ulSessionID);//�յ������ļ�����
	void processAutoDownload(AVDataInPacket& in,unsigned long ulSessionID);//�Զ����������ļ�����

	//�ĵ�������
	void processFSAddObj(AVDataInPacket& in,unsigned long ulSessionID);	//��ӱ�ע����
	void processFSDeleteObj(AVDataInPacket& in,unsigned long ulSessionID);	//ɾ����ע����
	void processFSChangePage(AVDataInPacket& in,unsigned long ulSessionID);  //�ĵ���ҳ
	void processFSGetCurPageObj(AVDataInPacket& in);  //�õ���ǰҳ���ж���
	void processFSRequestAllObj(AVDataInPacket& in,unsigned long ulSessionID);	//�������ж���
	void processFSDeleteCurPageObj(AVDataInPacket& in);		//ɾ����ǰҳ���ж���
	void processFSSetFileRect(AVDataInPacket& in);		//�����ĵ�ʵ�ʴ�С����ʾ�����С
	void processFSZoom(AVDataInPacket& in,unsigned long ulSessionID);		
	void processFSMovePos(AVDataInPacket& in,unsigned long ulSessionID);		
	void processFSGetPos(AVDataInPacket& in,unsigned long ulSessionID);	

	//FlashShare ����
	void processFlashPlay(AVDataInPacket& in,unsigned long ulSessionID); //��ʼ����Flash	
	void processFlashStop(AVDataInPacket& in,unsigned long ulSessionID);  //ֹͣ����Flash	
	void processFlashPause(AVDataInPacket& in,unsigned long ulSessionID);	//��ͣ����Flash
	void processFlashPosition(AVDataInPacket& in,unsigned long ulSessionID);	//����Flash����λ��
	void processFlashRequest(AVDataInPacket& in,unsigned long ulSessionID);	  //����Flash����λ��
	void processFlashSeek(AVDataInPacket& in,unsigned long ulSessionID);   //SeekFlash����λ��
};
