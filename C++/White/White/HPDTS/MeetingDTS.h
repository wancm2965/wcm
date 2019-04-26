
//========================================================================================
//版权所有: 版权所有(C) 2012，华平信息技术股份有限公司
//系统名称: AVCON
//作　　者: duquansheng
//创建日期: 2012-11-20
//功能说明: 会议内核头文件
//

#pragma once

#include <HPDTS/IHPDTS.h>
#include <HPDTS/DTSCmdType.h>
#include "XAutoLock.h"
#include "AVDataPacket.h"
#include <map>
#include <string>

//---------------------------------------------------------------------------------------
typedef struct _tagOBJMETDATA
{
	int				nLevel;
	std::string		strMainKey;
	std::string		strSubKey;
	unsigned int	nIndex;
	int				nLen;
	char*			pData;
	_tagOBJMETDATA()
	{
		nLevel	= 0;
		nIndex	= 0;
		nLen	= 0;
		pData	= NULL;
	};
}OBJMETDATA, *POBJMETDATA;

typedef std::map<unsigned long,unsigned long>	MapULong;
typedef std::map<std::string,OBJMETDATA>		MapMETObjects;

//---------------------------------------------------------------------------------------
class MeetingDTS
{
public:
	MeetingDTS(IHPDTSNotify&rIHPDTSNotify,bool bUI);
	virtual ~MeetingDTS(void);

public:
	void AddMeetingSession(unsigned long ulSessionID);
	void DelMeetingSession(unsigned long ulSessionID);

	void ProMeetingUIData(const char* pData,int nLen);
	void ProMeetingDTSData(unsigned long ulSessionID,const char* pData,int nLen);

protected:
	void ClearMeetingData(void);
	void SaveMeetingObj(int nLevel,std::string strMainKey,std::string strSubKey,char* pData,int nLen,unsigned int nIndex);
	unsigned int DelMeetingObj(std::string strMainKey,std::string strSubKey);
	void SendMeetingObj(unsigned long ulSessionID,int nLevel,bool bOnlyThis);
	void DeliverMeetingData(unsigned long ulSessionID,const char* pData,int nLen);

private:
	unsigned long			m_ulParentSessionID;
	bool					m_bUI;
	MapMETObjects			m_MapObjects;
	MapULong				m_MapSessionID;
	XCritSec				m_csMapSessionID;
	XCritSec				m_csMapObjects;
	IHPDTSNotify&			m_rIHPDTSNotify;
};
