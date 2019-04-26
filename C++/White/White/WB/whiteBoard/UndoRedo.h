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

typedef std::map<unsigned long, XMemIni> MapWBAttributes;  //keyΪobjID,secondΪ���������   ����һ�Զ����IDΪ�ؼ���
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
	int              m_iIndexOfStatus;//��ǰ״̬
	unsigned __int32 m_iNumOfStatus;//�ܵ�״̬
	int				 m_iSlidePos;     //���������Ŀ����������¼�Ѿ�undo�˼���
	int              m_iSlideNum;      //����undo������Ŀ
public:
	void Init();
	void CleanUp();
	void SetWbStatusNum(unsigned __int32 iStatusNum);
	//<---net  �����͹���������
	void ProcessDTSWbCmdData(eWbCmdOperation eCmdType,char* pData,int iLen);
	void OnDelOnePageAllAttribute(char* pData, int iLen);	//ɾ��һҳ�����ж���
	void OnAddAttribute(char* pData,int iLen);//�������
	void OnChangeAttribute(char* pData,int iLen);//�޸�����
	void OnCleanUpAttribute(char* pData,int iLen);//ɾ������
	void OnDelAttribute(char* pData,int iLen);
	void OnRedo(char* pData,int iLen);
	void OnUndo(char* pData,int iLen);
	void OnClearUndo(char* pData,int iLen);		//��ճ�������
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
