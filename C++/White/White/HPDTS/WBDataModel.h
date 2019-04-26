#pragma once
#include "stdafx.h"
#include "XAutoLock.h"
#include "IWBDTS.h"
#include <HPDTS/IHPDTS.h>
#include "HPDTS/DTSCmdType.h"
#include <map>
#include <vector>
#include "LogWrite.h"
#include "Config.h"


using namespace std;

#define PROC_UI_DATA(proc_cmd,proc_fun) \
{\
	if (eCmdType == proc_cmd)\
	{\
		proc_fun(pData,iLen);\
		return;\
	}\
} 

#define PROC_DTS_DATA(proc_cmd,proc_fun) \
{\
	if (eCmdType == proc_cmd)\
{\
	proc_fun(iSessionID,pData,iLen);\
	return;\
}\
}
#define CHECK_EXIST \
{\
if(!ObjExist(ulObjID))\
	return;\
}

#define MAX_STATUS_NUM 64
#ifndef MAX_PATH
#define MAX_PATH 4096
#endif
#define PACKET_LENGTH 8192  
#define KEY_TRANSPOND_DATA  7999
#define KEY_FILE_NAME	8000
typedef std::map<unsigned long,XMemIni> MapWBAttributes;
typedef MapWBAttributes::iterator IT_MapWBAttributes;

// typedef std::vector<XMemIni> VectorWBAttributes;
// typedef VectorWBAttributes::iterator IT_VectorWBAttributes;				  //这个地方几乎没有用到只是在图层那里用到，但图层操作是在客户端已经实现

typedef std::map<unsigned long,MapWBAttributes>MapWBPageObj;			 //huanglongquan add
typedef MapWBPageObj::iterator IT_MapWBPageObj;

 typedef std::vector<MapWBAttributes> VectorWBStatus;				  //huanglongquan注释
 typedef VectorWBStatus::iterator IT_VectorWBStatus;

//  typedef std::vector<MapWBPageObj> VectorWBStatus;				  //huanglongquan修改
//  typedef VectorWBStatus::iterator IT_VectorWBStatus;

typedef std::vector<unsigned long> VectorEventID;
typedef VectorEventID::iterator IT_VectorEventID;

typedef std::map<unsigned long,unsigned long> MapObjID;
typedef MapObjID::iterator IT_MapObjID;

typedef struct tSessionIDInformaiton 
{
	unsigned long ulSessionID;
	MapObjID mapPassiveObjID;
	bool bHost;
	tSessionIDInformaiton()
	{
		ulSessionID = 0;
		bHost = false;
		mapPassiveObjID.clear();
	}
}SESSION_INFO,*PSESSION_INFO;

typedef std::map<unsigned long,SESSION_INFO> MapSessionInfo;
typedef MapSessionInfo::iterator IT_MapSessionInfo;

#define  _MAX_COLORNUM	15

typedef struct _MyRGB 
{
	unsigned int nRed;
	unsigned int nGreen;
	unsigned int nBlue;
	bool bOccupy;
	_MyRGB()
	{
		nRed = 0;
		nGreen = 0;
		nBlue =0;
		bOccupy = false;
	}
}MyRGB,*PMyRGB;

typedef std::map<int ,MyRGB>MapCorrespondingRGB;
typedef MapCorrespondingRGB::iterator IT_MapCorrespondingRGB;

typedef std::map<string ,int >MapUserColorInfo;
typedef MapUserColorInfo::iterator IT_MapUserColorInfo;



class CWBDataModel :public IWBDTS
{
public:
	CWBDataModel(IHPDTSNotify&rIHPDTSNotify,bool bUI);
	~CWBDataModel(void);
protected:
	bool			 m_bUI;
	IHPDTSNotify*    m_pIHPDTSNotify;
	XCritSec		 m_csMapSessionID;
	XCritSec		 m_csVectorWBAttribute;
	XCritSec		 m_csMapRGBInfo;
	XCritSec		 m_csMapUserColorInfo;
	//data
	MapSessionInfo	 m_mapSessionInfo;
	VectorWBStatus   m_vectorWbStatus;
	VectorEventID	 m_vectorEventID;
	int              m_iIndexOfStatus;
	unsigned int m_iNumOfStatus;
	int				 m_iSlidePos;
	int              m_iSlideNum;
/*	LogWrite m_logWrite;*/
	bool			m_bMCUExistObj;			//当前mcu是否存在白板对象
	bool			m_bAreadyRequestAllObj;			//是否已经响应请求所有对象

	MapCorrespondingRGB m_mapCorrespondingRGB;
	MapUserColorInfo m_mapUserColorInfo;
public:
	//nomal funtions
	void Init();
	void CleanUp();
	void SetWbStatusNum(unsigned int iStatusNum);
	void InitUserColor();
	void	SetUserColor(eUserColorWB eColorWB);
	bool GetUserColor(eUserColorWB eColorWB,unsigned int& nRed,unsigned int& nGreen,unsigned int& nBlue,bool bOccupy = false);
	//IWBDTS
	void processWBUIData(AVDataInPacket& in);			
	void processWBDTSData(AVDataInPacket& in,unsigned long ulSessionID);	
	void AddWBDTSSession(unsigned long ulSessionID);
	void AddWBDTSSession(unsigned long ulSessionID,bool bHost); //重载
	void RemoveWBDTSSession(unsigned long ulSessionID);
protected:
	//--->net
	void ProcessUIWbCmdData(eWbCmdOperation eCmdType,char* pData,int iLen);
	void AddAttribute(char* pData,int iLen);//添加属性
	void RequestAttribute(char* pData,int iLen);//获取属性
	void ChangeAttribute(char* pData,int iLen);//修改属性
	void DelAttribute(char* pData,int iLen);//删除属性
	void DelAllAttribute(char* pData,int iLen);//删除所有对象
	void DelPageAllAttribute(char* pData,int iLen);//删除页所有对象
	void RequestAllObj(char* pData,int iLen);
	void Redo(char* pData,int iLen);
	void Undo(char* pData,int iLen);
/*	void Layer(char* pData,int iLen);*/
	void P2PData(char* pData,int iLen);
	void BroadcastData(char* pData,int iLen);
	void P2MCU(char* pData,int iLen);
	void AskForUserColor(char* pData,int iLen);
	//<---net
	void ProcessDTSWbCmdData(eWbCmdOperation eCmdType,unsigned long iSessionID,char* pData,int iLen);
	void OnAddAttribute(unsigned long iSessionID,char* pData,int iLen);//添加属性
	void OnRequestAttribute(unsigned long iSessionID,char* pData,int iLen);//获取属性
	void OnChangeAttribute(unsigned long iSessionID,char* pData,int iLen);//修改属性
	void OnCleanUpAttribute(unsigned long iSessionID,char* pData,int iLen);//删除属性
	void OnDelAllAttribute(unsigned long iSessionID,char* pData,int iLen);//删除全部对象 
	void OnDelPageAllAttribute(unsigned long iSessionID,char* pData, int iLen);//处理删除页全部对象
	void OnDelAttribute(unsigned long iSessionID,char* pData,int iLen);
	void OnRequestAllObj(unsigned long iSessionID,char* pData,int iLen);//申请所有的对象
	void OnRedo(unsigned long iSessionID,char* pData,int iLen);
	void OnUndo(unsigned long iSessionID,char* pData,int iLen);
/*	void OnLayer(unsigned long iSessionID,char* pData,int iLen);*/
	void OnP2PData(unsigned long iSessionID,char* pData,int iLen);//点对点消息
	void OnBroadcastData(unsigned long iSessionID,char* pData,int iLen);//广播消息
	void OnStartStream(unsigned long iSessionID,char* pData,int iLen);//开始传输流
	void OnReStartStream(unsigned long iSessionID,char* pData,int iLen);//重新传输流
	void OnAskForStream(unsigned long iSessionID,char* pData,int iLen);//收到申请
	void OnRelateAttributeToStream(unsigned long iSessionID,char* pData,int iLen);//将属性和流关联
	void OnSaveStream(unsigned long iSessionID,char* pData,int iLen);
	void OnErrorStream(unsigned long iSessionID,char* pData,int iLen);
	void OnStopStream(unsigned long iSessionID,char* pData,int iLen);
	void OnEndStream(unsigned long iSessionID,char* pData,int iLen);
	void OnPassiveStartStream(unsigned long iSessionID,char* pData,int iLen);
	void OnPassiveSaveStream(unsigned long iSessionID,char* pData,int iLen);
	void OnPassiveEndStream(unsigned long iSessionID,char* pData,int iLen);
	void OnPassiveErrorStream(unsigned long iSessionID,char* pData,int iLen);
	void OnAskForUserColor(unsigned long iSessionID,const char* pData,int iLen); // 离该iSessionID客户端最进的MCU处理，然后转发该结果
	void OnRelativeUserColor(unsigned long iSessionID,char* pData,int iLen);//处理用户请求颜色结果
	void OnBroadcastUserColor(unsigned long iSessionID,char* pData, int iLen);
	//
	void StatusToStatus(int iFrom,int iTo);
/*	void ReSortLayer(int iStatusFrom,int iStatusTo);*/

/*	void StartPassiveStream(unsigned int iSessionID,unsigned long ulObjID,unsigned long ulSize,string strFileType);*/
/*	void StartPassiveStream(unsigned int iSessionID,unsigned long ulObjID,char* pData, int iLen);  //huanglongquan 修改*/
	void StartPassiveStream(unsigned int iSessionID,unsigned long ulObjID,XMemIni xmlPacket);
	void EndPassiveStream(unsigned int iSessionID,unsigned long ulObjID);
	void ErrorPassiveStream(unsigned int iSessionID,unsigned long ulObjID);
	void RelateAttributeToStream(unsigned int iSessionID,unsigned long ulObjID,bool bToPoint = true);
	void ErrorStream(unsigned int iSessionID,unsigned long ulObjID);
	void EndStream(unsigned int iSessionID,unsigned long ulObjID);
	void SavePassiveStream(unsigned int iSessionID,unsigned long ulObjID,char* pData,int iLen);
	void SaveStream(unsigned int iSessionID,unsigned long ulObjID,unsigned long ulPos,unsigned long ulSize);
	void StartStream(unsigned long ulObjID,unsigned long ulEventID,unsigned long ulSize,string strFileType,unsigned int iSessionID = 0);
/*	void StartStream(unsigned long ulObjID,int iTo,unsigned int iSessionID = 0);*/
	void StartStream(XMemIni xmlobj, unsigned int iSessionID);
	void ReStartStream(unsigned long ulObjID,unsigned long ulEventID,unsigned int iSessionID);
	void ReStartStream(unsigned long ulObjID,XMemIni xmlobj, unsigned long iSessionID);
	void AskForStream(unsigned int iSessionID,unsigned long ulObjID);
	void SendToOthers(unsigned long iSessionID,char* pData,int iLen,unsigned long ulPassiveObjID = 0);
	void SendToAll(char* pData,int iLen,unsigned long ulPassiveObjID = 0);
	void RegistPassiveObjID(unsigned long ulPassiveObjID);
	void RegistPassiveObjID(unsigned long ulSessionID,unsigned long ulPassiveObjID);
	void UnregistPassiveObjID(unsigned long ulPassiveObjID);
	bool CheckTranslateModel(string strPath,unsigned long ulPos);
	void SendToPoint(unsigned long iSessionID,char* pData,int iLen);
	bool NeedToNotifyCleanUpAttribute(unsigned long ulObjID);
	void NotifyCleanUpAttribute(unsigned long ulObjID,unsigned long ulSessionID = 0);
	bool ToMemIni(XMemIni &xmlPacket,char* pData,int iLen);
	bool NeedToSlide(unsigned long ulEventID);
	void SlideNextStatus();
	void prepareCoverStatus(int iIndexOfStatus);
	void NotifyStopStream(unsigned long ulObjID);
	bool NeedToStopStream(unsigned long ulObjID);
	bool ObjExistInAnyOtherStatus(unsigned long ulObjID,int iIndexStatus);
	bool ObjExist(unsigned long ulObjID);
	bool GetFileStatus(string strPath,unsigned long &iSize,string &strType);
	void WBMapToWBMap(MapWBAttributes& mapFrom,MapWBAttributes& mapTo);
	void CheckXmlPacket(XMemIni & xmlPacket);
	void CheckXmlString(string &strValue);
	void CheckXmlPacketPath(XMemIni &xmlPacket);
};
