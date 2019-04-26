/************************************************************************/
/*接口定义                                                              */
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
	*描述：分割开始录像成功回调
	*输入：
	RecInfo		录像停止信息
	录像文件大小
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnStartRecord(const REC_INFO& RecInfo)=0;
	/******************************************************************************
	* OnStopRecord
	*描述：停止录像成功回调
	*输入：
	RecInfo		录像停止信息
	录像文件大小
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnStopRecord(const REC_INFO& RecInfo)=0;

	/******************************************************************************
	* OnStatus
	*描述：录像服务器状态回调
	*输入：
	uRecCount		在录像路数
	uFreeSpace		剩余空间
	eRecType		录像模板（正常还是覆盖）
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnStatus(unsigned short uRecCount,unsigned long uFreeSpace,RECMODE_TYPE eRecType)=0;


	/******************************************************************************
	* OnDeleteRecord
	*描述：删除文件成功回调
	*输入：
	strFilePath		录像文件全路径
	strDelteFile	删除文件名
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnDeleteRecord(std::string strFilePath, std::string strDelteFile)=0;


	/******************************************************************************
	* GetNeedDelFiles
	*描述：获取需要删除的文件
	*输入：RECDEL_TYP 代表什么样的文件
	*输出： 
	mapDelFiles key文件名,value 文件路径
	*返回值：无
	*****************************************************************************/
	virtual void GetNeedDelFiles(std::map<string,string>& mapDelFiles,RECDEL_TYPE eDelType)=0;


	/******************************************************************************
	* OnRecoverFile
	*描述：修复文件反馈
	*输入：strFilePath 全路径名，eResult结果，uFileSize大小，strEndTime结束时间
	*输出： 
	*返回值：无
	*****************************************************************************/
	virtual void OnRecoverFile(std::string strFilePath,RECOVER_RESULT eResult,unsigned long uFileSize,std::string strEndTime)=0;

	//会议录像文件满
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
	*描述：设置保存信息
	*输入：
	strAccout		-录播ID
	strPath			-保存路径
	RECSAVE_TYPE	-保存方式（为以后扩展，模板本地磁盘）
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void SetRecInfo(const std::string& strAccout,int nMaxRecCount,const std::string& strPath,RECSAVE_TYPE eSaveType = REC_SAVE_LOCAL)=0;

	/******************************************************************************
	* SetRecModeInfo
	*描述：设置录像信息
	*输入：
	eModeTye		-录像模式（正常；覆盖）
	uCutSize	    -切割大小值（M）
	uCutTime		-切割时间值（MIN）
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void SetRecModeInfo(RECMODE_TYPE eModeTye,unsigned short uCutSize,unsigned short uCutTime)=0;

	/******************************************************************************
	* StartEngine
	*描述：启动SDK,开始工作
	*输入：无
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual bool StartEngine(void)=0;

	/******************************************************************************
	* StartRecord
	*描述：开始录像
	*输入：
	 RecInfo	需要录像的信息
	 eStartType 录像启动类型（手动，警报，计划）
	*输出： 成功时 RecInfo中增加开始录像时间，录像文件名，录像路径
	*返回值：RECERR_CODE
	*****************************************************************************/
	virtual RECERR_CODE StartRecord(REC_INFO& RecInfo,RECSTART_TYPE eStartType =RECSTART_TYPE_NORMAL)=0;



	/******************************************************************************
	* ChangeAudio
	*描述：切换音频ID接音频
	*输入：
	strDevid  设备或者人员ID
	strChanid 通道ID
	uAudioID  新的音频ID
	*输出：   无
	*返回值： 无
	*****************************************************************************/
	virtual void ChangeAudio(const std::string& strDevid,const std::string& strChanid,unsigned long uAudioID)=0;

	/******************************************************************************
	* StopRecord
	*描述：停止录像
	*输入：
	strRoomID		会议ID
	strDevid		设备或者人员ID 
	strChanid		通道ID
	strrecuserid	发起人ID
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void StopRecord(const std::string& strRoomID,const std::string& strDevid,const std::string& strChanid,const std::string& strrecuserid,RECSTOP_TYPE eStopType = RECSTOP_TYPE_NORMAL)=0;

	/******************************************************************************
	* StartRecoverFile
	*描述：修复录像
	*输入：
	strFilePath	 文件全路径名
	*输出：
	*返回值：bool 创建与否
	*****************************************************************************/
	virtual bool StartRecoverFile(const std::string& strFilePath)=0;

	/******************************************************************************
	* DeletFile
	*描述：删除录像文件
	*输入：
	strDeletePathName	录像文件全路径
	strDeleteFile		录像文件名
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void DeletFile(const std::string& strDeletePathName)=0;

	/******************************************************************************
	* StopAllRecord
	*描述：停止所有录像
	*输入：无
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void StopAllRecord()=0;

	/******************************************************************************
	* StopEngine
	*描述：停止录像，停止SDK运行
	*输入：无
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void StopEngine()=0;

	/******************************************************************************	
	*描述：会议录像	
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


