//-------------------------------------
// 录像计划结构
typedef struct _MMS_RECPLAN
{
	std::string		strStartTime;
	std::string		strEndTime;
}MMS_RECPLAN,*PMMS_RECPLAN;

typedef std::list<PMMS_RECPLAN> MMS_RECPLAN_LST;

//-------------------------------------
// 录像格式结构
typedef struct _MMS_RECFORMAT
{
	std::string		strVODDis;		//屏幕点播分辨率
	unsigned short  usVODFPS;		//屏幕点播帧率
	unsigned short	usVODBPS;		//屏幕点播码流
	std::string		strArchiveDis;	//屏幕归档分辨率
	unsigned short  usArchiveFPS;	//屏幕归档帧率
	unsigned short	usArchiveBPS;	//屏幕归档码流
	std::string		strPPTDisplay;	//PPT分辨率
	unsigned short  usPPTFPS;		//PPT帧率
	unsigned short	usPPTBPS;		//PPT码流
	std::string		strSingleDis;	//个人分辨率
	unsigned short  usSingleFPS;	//个人帧率
	unsigned short	usSingleBPS;	//个人码流
	bool			bRecTogether;	//是否一起录象
	_MMS_RECFORMAT()
	{
		usVODFPS=0;
		usVODBPS=0;
		usArchiveFPS=0;
		usArchiveBPS=0;
		usPPTFPS=0;
		usPPTBPS=0;
		usSingleFPS=0;
		usSingleBPS=0;
		bRecTogether=false;
	};
}MMS_RECFORMAT,*PMMS_RECFORMAT;

//=======================================================================================
// MMS模块类
//=======================================================================================
class AFX_EXT_CLASS KMMSSession
{
//...
	/****************************************************************
	* 录播函数
	****************************************************************/
	virtual void GetStorageServer()=0;
	virtual void GetUploadPlan()=0;
	virtual void GetRecordFormat()=0;
	virtual void SetRecordFormat(const MMS_RECFORMAT& recFormat)=0;

	virtual void StartRecord()=0;
	virtual void PauseRecord()=0;
	virtual void StopRecord()=0;
};

//=======================================================================================
// MMS模块事件类
//=======================================================================================
class AFX_EXT_CLASS KMMSSessionEvent
{
//...
	/****************************************************************
	* 录播函数
	****************************************************************/
	virtual void OnGetStorageServer(const std::string& strServer)=0;
	virtual void OnGetUploadPlan(bool bAutoUpload, MMS_RECPLAN_LST& listRecPlan)=0;
	virtual void OnGetRecordFormat(MMS_RECFORMAT& recFormat)=0;

	virtual void OnStartRecord(const std::string& strUserID,const std::string& strServerID="")=0; // strServerID空为失败只给申请者回调，否则所有主持人
	virtual void OnPauseRecord()=0;
	virtual void OnStopRecord()=0;
}
