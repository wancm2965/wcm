#pragma once
#include "shai/XMemIni.h"
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <deque>
#include "shai\AVDataPacket.h"
using namespace std;

#define MAX_UNDO_REDO_NUM  64

#define PROC_DTS_DATA(proc_cmd,proc_fun) \
{\
	if (eCmdType == proc_cmd)\
{\
	proc_fun(pData,iLen);\
	return;\
}\
}
#define CHECK_EXIST \
{\
	if(!ObjExist(ulObjID))\
	return;\
}

typedef std::map<unsigned long, XMemIni> MapWBAttributes;  //key为objID,second为对象的属性   这是一以对象的ID为关键字
typedef MapWBAttributes::iterator IT_MapWBAttributes; 

typedef std::vector<XMemIni> VectorWBAttributes;
typedef VectorWBAttributes::iterator IT_VectorWBAttributes;

typedef std::vector<MapWBAttributes> VectorWBStatus;
typedef VectorWBStatus::iterator IT_VectorWBStatus;

typedef std::vector<unsigned long> VectorEventID;
typedef VectorEventID::iterator IT_VectorEventID;

class CWBDataModel 
{
public:
	CWBDataModel();
	~CWBDataModel(void);
protected:
	VectorWBStatus   m_vectorWbStatus;
	VectorEventID	 m_vectorEventID;
	int              m_iIndexOfStatus;//当前状态
	unsigned __int32 m_iNumOfStatus;//总的状态
	int				 m_iSlidePos;     //这个变量的目的是用来记录已经undo了几步
	int              m_iSlideNum;      //可以undo的总数目
public:
	void Init();
	void CleanUp();
	void SetWbStatusNum(unsigned __int32 iStatusNum);
	//<---net  处理发送过来的数据
	void ProcessDTSWbCmdData(eWbCmdOperation eCmdType,char* pData,int iLen);
	void OnDelOnePageAllAttribute(char* pData, int iLen);	//删除一页的所有对象
	void OnAddAttribute(char* pData,int iLen);//添加属性
	void OnChangeAttribute(char* pData,int iLen);//修改属性
	void OnCleanUpAttribute(char* pData,int iLen);//删除属性
	void OnDelAttribute(char* pData,int iLen);
	void OnRedo(char* pData,int iLen);
	void OnUndo(char* pData,int iLen);
	void OnClearUndo(char* pData,int iLen);		//清空撤销队列
/*	void OnLayer(char* pData,int iLen);*/
	//
	void StatusToStatus(int iFrom,int iTo);
	void ReSortLayer(int iStatusFrom,int iStatusTo);
	bool NeedToNotifyCleanUpAttribute(unsigned long ulObjID);
	BOOL ToMemIni(XMemIni &xmlPacket,char* pData,int iLen);
	bool NeedToSlide(unsigned long ulEventID);
	void SlideNextStatus();
	void prepareCoverStatus(int iIndexOfStatus);
	BOOL ObjExistInAnyOtherStatus(unsigned long ulObjID,int iIndexStatus);
	BOOL ObjExist(unsigned long ulObjID);
	void WBMapToWBMap(MapWBAttributes& mapFrom,MapWBAttributes& mapTo);
	void CheckXmlPacket(XMemIni & xmlPacket);
	void CheckXmlString(string &strValue);
};
