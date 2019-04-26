
//========================================================================================
//版权所有: 版权所有(C) 2008，上海华平信息技术股份有限公司
//系统名称: AVCON7
//作　　者: duquansheng
//创建日期: 2008-3-31
//功能说明: 白板内核头文件
//

#pragma once

#define FILE_TRANS_MAXLENGTH 8*1024
#ifndef MAX_PATH
#define MAX_PATH	4096
#endif

#include <HPDTS/IHPDTS.h>
#include <HPDTS/DTSCmdType.h>
#include "WbCommandType.h"
#include "XAutoLock.h"
#include "AVDataPacket.h"
#include "XMemIni.h"
#include <map>
#include <vector>


#ifdef _WIN32
#include <direct.h>
#endif

//---------------------------------------------------------------------------------------
typedef struct tag_OBJDATABUF
{
	char*			pData;
	int				nLen;
}OBJDATABUF, *LPOBJDATABUF;

typedef std::map<unsigned long,unsigned long> MapULong2ULong;
typedef std::map<unsigned long,OBJDATABUF> MapWBObjects;
typedef std::vector<OBJDATABUF> VectorStack;
//added by luoning 2010-7-12
typedef std::map<unsigned long,XMemIni> MapWBAttributes;
//end

//---------------------------------------------------------------------------------------
class WbDTS
{
public:
	WbDTS(IHPDTSNotify&rIHPDTSNotify,bool bUI);
	virtual ~WbDTS(void);
public:
	void processWbData(unsigned long channelId,char * data,int len);
	void SendWbData(unsigned long channelId,char* data,int len);
	void addChannel(unsigned long channelId);
	void removeChannel(unsigned long channelId);

	//{{DQS 2008.4.2 白板Undo / ReDo 功能预加接口。其他实现暂停。
	void PushRedoStack(const WB_COMMAND_HEAD& wbCmdHead);
	bool IsHasReDo();
	bool PopRedoStack(WB_COMMAND_HEAD& wbCmdHead);
	void PushUndoStack(const WB_COMMAND_HEAD& wbCmdHead);
	bool IsHasUnDo();
	bool PopUndoStack(WB_COMMAND_HEAD& wbCmdHead);
	//}}

protected:
	void processAddObj(unsigned long channelId,PWB_COMMAND_HEAD pWbHead);
	void ClearAllObject(void);
	void processDeleteAll(unsigned long channelId,char* data,int len);
	void processDeleteObj(unsigned long channelId,char* data,int len);
	void processObjRequest(unsigned long channelId,PWB_COMMAND_HEAD pWbHead);
	void processObjResponse(unsigned long channelId,char* data,int len);
	void deliverGroupWbData(unsigned long channelId,char * data,int len);

	void processPageChanged(unsigned long channelId,char* data,int len);	
	
	void processScroll(unsigned long channelId,char* data,int len);
	void processMoveObj(unsigned long channelId,char* data,int len);
	void processResizeObj(unsigned long channelId,char* data,int len);//added by luoning 2010-7-13
	void processBKColor(unsigned long channelId,char* data,int len);//added by luoning 2010-8-4
	void processRequestBKColor(unsigned long channelId,char* data,int len);
	//added by luoning 2010-7-8
	void processObjRoate(unsigned long channelId,char* data,int len);
	void processDeleteObjAttribute(unsigned long channelId,char* data,int len);
	void processDeleteAllObjAttribute(unsigned long channelId,char* data,int len);
	void processPutObjAttribute(unsigned long channelId,char* data,int len);
	void processGetObjAttribute(unsigned long channelId,char* data,int len);
	void processGetAllObjAtribute(unsigned long channelId,char* data,int len);
	void processPreMoveLayout(unsigned long channelId,char* data,int len);
	void processBackMoveLayout(unsigned long channelId,char* data,int len);
	void processBKLayout(unsigned long channelId,char* data,int len);
	void processMoveToHeadLayout(unsigned long channelId,char* data,int len);
	void processMoveToTailLayout(unsigned long channelId,char* data,int len);

	bool beBKAttribute(unsigned long dwObjID);
	int  FindCmdData(int iCmdType,char* strCmd,int ilen);
	bool BeInDeleteAttribute(int iCmd,char* pStrCmd,int iLen);
	//end
	void processTransLastPosition(unsigned long channelId,char* data,int len);
	void processTabCtrlChange(unsigned long channelId,char* data,int len);
	void processFullScreen(unsigned long channelId,char* data,int len);
	void processToolBarMove(unsigned long channelId,char* data,int len);

	void processTransAllObj(unsigned long channelId,char*pData,int nLen);
	void processTransLastPositionRequest(unsigned long channelId,char* data,int len);
	void processTransLastPositionResponse(unsigned long channelId,char* data,int len);
		
	bool FindObj(unsigned long ulObjID);		
	
	void processTransDTObjHead(unsigned long channelId,char* data,int len);
	void processTransDTObjResponse(unsigned long channelId,char* data,int len);
	void processTransDTObjRequest(unsigned long channelId,char* data,int len);
	void SaveDataToFile(char* data, int len, unsigned long dwOffset,unsigned long ulObjId);
	void GetBigFileName(unsigned long ulObjId, char* pFileName);

protected:
	bool					m_bUI;
	//added by luoning 2010-7-8
	MapWBAttributes			m_mapWBObjAttribute;
	XCritSec				m_csMapWBObjAttribute;
	//end
	MapWBObjects			m_MapWBObjects;
	MapULong2ULong			m_MapChsId;
	XCritSec				m_csMapChsId;
	XCritSec				m_csMapWBObjects;
	IHPDTSNotify&			m_rIHPDTSNotify;
	CLIENT_WINDOW_POSITION	m_cwPos;		
	MapULong2ULong			m_MapBigObj;

	VectorStack				m_vecUndoStack;
	VectorStack				m_vecRedoStack;
	XCritSec				m_csUndoStack;
	XCritSec				m_csRedoStack;
};
