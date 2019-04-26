/************************************************************************/
/*�ӿڶ���                                                              */
/************************************************************************/

//RECORD_Engine.h
#ifndef __RECORD_ENGINE_H__
#define __RECORD_ENGINE_H__
#include "HPRecordDefine.h"


class IHPRecordNotify
{
public:

	/******************************************************************************
	* OnStartRecord
	*�������ָʼ¼��ɹ��ص�
	*���룺
	RecInfo		¼��ֹͣ��Ϣ
	¼���ļ���С
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnStartRecord(const REC_INFO& RecInfo)=0;
	/******************************************************************************
	* OnStopRecord
	*������ֹͣ¼��ɹ��ص�
	*���룺
	RecInfo		¼��ֹͣ��Ϣ
	¼���ļ���С
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnStopRecord(const REC_INFO& RecInfo)=0;

	/******************************************************************************
	* OnStatus
	*������¼�������״̬�ص�
	*���룺
	uRecCount		��¼��·��
	uFreeSpace		ʣ��ռ�
	eRecType		¼��ģ�壨�������Ǹ��ǣ�
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnStatus(unsigned short uRecCount,unsigned long uFreeSpace,RECMODE_TYPE eRecType)=0;


	/******************************************************************************
	* OnDeleteRecord
	*������ɾ���ļ��ɹ��ص�
	*���룺
	strFilePath		¼���ļ�ȫ·��
	strDelteFile	ɾ���ļ���
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnDeleteRecord(std::string strFilePath, std::string strDelteFile)=0;


	/******************************************************************************
	* GetNeedDelFiles
	*��������ȡ��Ҫɾ�����ļ�
	*���룺RECDEL_TYP ����ʲô�����ļ�
	*����� 
	mapDelFiles key�ļ���,value �ļ�·��
	*����ֵ����
	*****************************************************************************/
	virtual void GetNeedDelFiles(std::map<string,string>& mapDelFiles,RECDEL_TYPE eDelType)=0;


	/******************************************************************************
	* OnRecoverFile
	*�������޸��ļ�����
	*���룺strFilePath ȫ·������eResult�����uFileSize��С��strEndTime����ʱ��
	*����� 
	*����ֵ����
	*****************************************************************************/
	virtual void OnRecoverFile(std::string strFilePath,RECOVER_RESULT eResult,unsigned long uFileSize,std::string strEndTime)=0;

	//����¼���ļ���
	virtual void OnAVCRecordFactoryEvent_FullFile(const std::string& strRoomID, const std::string& strRecuserid)=0;

	virtual void OnStopConfRecord(const std::string& strRoomID)=0;

};


class RECORD_API IHPRecord
{
public:
	IHPRecord(){};
	virtual ~IHPRecord(){};
	/******************************************************************************
	* SetRecInfo
	*���������ñ�����Ϣ
	*���룺
	strAccout		-¼��ID
	strPath			-����·��
	RECSAVE_TYPE	-���淽ʽ��Ϊ�Ժ���չ��ģ�屾�ش��̣�
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void SetRecInfo(const std::string& strAccout,int nMaxRecCount,const std::string& strPath,RECSAVE_TYPE eSaveType = REC_SAVE_LOCAL)=0;

	/******************************************************************************
	* SetRecModeInfo
	*����������¼����Ϣ
	*���룺
	eModeTye		-¼��ģʽ�����������ǣ�
	uCutSize	    -�и��Сֵ��M��
	uCutTime		-�и�ʱ��ֵ��MIN��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void SetRecModeInfo(RECMODE_TYPE eModeTye,unsigned short uCutSize,unsigned short uCutTime)=0;

	/******************************************************************************
	* StartEngine
	*����������SDK,��ʼ����
	*���룺��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual bool StartEngine(void)=0;

	/******************************************************************************
	* StartRecord
	*��������ʼ¼��
	*���룺
	 RecInfo	��Ҫ¼�����Ϣ
	 eStartType ¼���������ͣ��ֶ����������ƻ���
	*����� �ɹ�ʱ RecInfo�����ӿ�ʼ¼��ʱ�䣬¼���ļ�����¼��·��
	*����ֵ��RECERR_CODE
	*****************************************************************************/
	virtual RECERR_CODE StartRecord(REC_INFO& RecInfo,RECSTART_TYPE eStartType =RECSTART_TYPE_NORMAL)=0;



	/******************************************************************************
	* ChangeAudio
	*�������л���ƵID����Ƶ
	*���룺
	strDevid  �豸������ԱID
	strChanid ͨ��ID
	uAudioID  �µ���ƵID
	*�����   ��
	*����ֵ�� ��
	*****************************************************************************/
	virtual void ChangeAudio(const std::string& strDevid,const std::string& strChanid,unsigned long uAudioID)=0;

	/******************************************************************************
	* StopRecord
	*������ֹͣ¼��
	*���룺
	strRoomID		����ID
	strDevid		�豸������ԱID 
	strChanid		ͨ��ID
	strrecuserid	������ID
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecord(const std::string& strRoomID,const std::string& strDevid,const std::string& strChanid,const std::string& strrecuserid,RECSTOP_TYPE eStopType = RECSTOP_TYPE_NORMAL)=0;

	/******************************************************************************
	* StartRecoverFile
	*�������޸�¼��
	*���룺
	strFilePath	 �ļ�ȫ·����
	*�����
	*����ֵ��bool �������
	*****************************************************************************/
	virtual bool StartRecoverFile(const std::string& strFilePath)=0;

	/******************************************************************************
	* DeletFile
	*������ɾ��¼���ļ�
	*���룺
	strDeletePathName	¼���ļ�ȫ·��
	strDeleteFile		¼���ļ���
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void DeletFile(const std::string& strDeletePathName)=0;

	/******************************************************************************
	* StopAllRecord
	*������ֹͣ����¼��
	*���룺��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void StopAllRecord()=0;

	/******************************************************************************
	* StopEngine
	*������ֹͣ¼��ֹͣSDK����
	*���룺��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void StopEngine()=0;

	/******************************************************************************	
	*����������¼��	
	*****************************************************************************/
	virtual RECERR_CODE StartConferenceRecord(REC_INFO& RecInfo)=0;
	virtual void SaveConfStream(std::string strRoomID,char* outPacket, int nLen, bool bKeyFrame, int nFrameType)=0;
	virtual RECERR_CODE StartConfAVRecord(REC_INFO& RecInfo)=0;
	virtual void StopConferenceRecord(const std::string& strRoomID,RECSTOP_TYPE eStopType = RECSTOP_TYPE_NORMAL)=0;
	virtual void StopConfAVRecord(const std::string& strRoomID,const std::string& strDevid,const std::string& strChanid,const std::string& strrecuserid)=0;
	virtual void StartConfDosRecord(const std::string& strRoomID, const std::string& strMemberID)=0;
	virtual void StopConfDosRecord(const std::string& strRoomID, const std::string& strMemberID)=0;
	virtual void SaveConfDosStream(const std::string& strRoomID, const std::string& strMebmerID, char* outPacket, int nLen)=0;
	virtual void StopAllConfRecord(bool bFromThread = false)=0;
};


RECORD_API IHPRecord* CreateHPRecord(IHPRecordNotify& rNotify);
#endif


