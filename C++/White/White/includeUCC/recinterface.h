//-------------------------------------
// ¼��ƻ��ṹ
typedef struct _MMS_RECPLAN
{
	std::string		strStartTime;
	std::string		strEndTime;
}MMS_RECPLAN,*PMMS_RECPLAN;

typedef std::list<PMMS_RECPLAN> MMS_RECPLAN_LST;

//-------------------------------------
// ¼���ʽ�ṹ
typedef struct _MMS_RECFORMAT
{
	std::string		strVODDis;		//��Ļ�㲥�ֱ���
	unsigned short  usVODFPS;		//��Ļ�㲥֡��
	unsigned short	usVODBPS;		//��Ļ�㲥����
	std::string		strArchiveDis;	//��Ļ�鵵�ֱ���
	unsigned short  usArchiveFPS;	//��Ļ�鵵֡��
	unsigned short	usArchiveBPS;	//��Ļ�鵵����
	std::string		strPPTDisplay;	//PPT�ֱ���
	unsigned short  usPPTFPS;		//PPT֡��
	unsigned short	usPPTBPS;		//PPT����
	std::string		strSingleDis;	//���˷ֱ���
	unsigned short  usSingleFPS;	//����֡��
	unsigned short	usSingleBPS;	//��������
	bool			bRecTogether;	//�Ƿ�һ��¼��
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
// MMSģ����
//=======================================================================================
class AFX_EXT_CLASS KMMSSession
{
//...
	/****************************************************************
	* ¼������
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
// MMSģ���¼���
//=======================================================================================
class AFX_EXT_CLASS KMMSSessionEvent
{
//...
	/****************************************************************
	* ¼������
	****************************************************************/
	virtual void OnGetStorageServer(const std::string& strServer)=0;
	virtual void OnGetUploadPlan(bool bAutoUpload, MMS_RECPLAN_LST& listRecPlan)=0;
	virtual void OnGetRecordFormat(MMS_RECFORMAT& recFormat)=0;

	virtual void OnStartRecord(const std::string& strUserID,const std::string& strServerID="")=0; // strServerID��Ϊʧ��ֻ�������߻ص�����������������
	virtual void OnPauseRecord()=0;
	virtual void OnStopRecord()=0;
}
