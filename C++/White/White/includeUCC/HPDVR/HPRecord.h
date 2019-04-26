#ifndef __HPRECORD_H__
#define __HPRECORD_H__

//------------------------------------------------------------------
/*����꿪��*/
#define _FOR_MON/*���¼��*/
//#define _FOR_NVS/*�������豸¼��*/
//#define _USE_TIMESTAMP/*��ʱ��֡*/

//------------------------------------------------------------------
/*�ļ�ͷ����*/
#define AVF_SYNC_TIME				350 /*��Ƶͬ���ã��ȴ���Ƶ��ʱ��*/
#define AVF_FILE_LEN				300 /*�ļ��ָ��С*/

#define AVF_RECORD_NORMAL			0/*����¼��*/
#define AVF_RECORD_ALERT			1/*����¼��*/

#define	AVF_FRAME_TYPE_VIDEO		3/*��Ƶ��*/
#define	AVF_FRAME_TYPE_AUDIO		4/*��Ƶ��*/

#define	AVF_HEADER_PWD				2   //MD5����			[128�ֽ�]
#define	AVF_HEADER_RECORD_TYPE		3   //¼������			[1�ֽ�]		[0:����¼��,1:����¼��]
#define AVF_HEADER_START_TIME		4   //��ʼʱ��			[12�ֽ�]
#define AVF_HEADER_STOP_TIME		5   //����ʱ��			[12�ֽ�]
#define AVF_HEADER_DEV				6	//�豸				[128�ֽ�]
#define AVF_HEADER_CHN				7	//ͨ��				[1�ֽ�]
#define	AVF_HEADER_DESC				8	//¼������			[256�ֽ�]
#define	AVF_HEADER_PREV_FILE		9	//��һ���ļ�����	[128�ֽ�]
#define	AVF_HEADER_NEXT_FILE		10	//��һ���ļ�����	[128�ֽ�]
#define	AVF_HEADER_UPLOAD_STATE     11	//�ϴ�״̬			[1�ֽ�]		[0=δ�ϴ�,1=���ϴ�,2=�ϴ���,3=У����]
#define	AVF_HEADER_UPLOAD_BYTES     12	//�ϴ��ֽ���		[4�ֽ�]
#define	AVF_HEADER_TIMESTAMP		14	//��ʼʱ���		[4�ֽ�]
#define	AVF_HEADER_VERSION			15  //��Ʒ�汾��		[128�ֽ�]	[��:(AVCON4.x.xx AVCON5.x.xx AVCON6.x.xx NDVR2.x.xx)]
/*�������룬���ﶨ
#define AVF_HEADER_SIZE				0   //�ṹ�峤��
#define AVF_HEADER_FLAG				1   //¼��汾��
#define	AVF_HEADER_CHECKSUM			13	//�۸�У����		[32�ֽ�]	
*/

//-------------------------------------------------------------------
/*¼��ص��¼�*/
class IAVFRecordEvent { 
public:
	virtual void OnAVCRecordFactoryEvent_FullFile() = 0;/*¼���ļ���*/
};

//-------------------------------------------------------------------
/*��·��¼��ӿ�*/
class IAVFRecordSingleStream { 
public: 
	virtual void WriteData(char* pData, int nLen, bool bKeyFrame) = 0;
	virtual void NotifyStreamClose() = 0;
	virtual void WriteData(char* pData, int nLen, bool bKeyFrame, unsigned long ulTimestamp) = 0;
};

//-------------------------------------------------------------------
/*��ͨ¼��ӿ�*/
class IAVFRecordFactory { 
public:
	virtual bool Connect(const char* strAVCFile) = 0;
	virtual void ReleaseConnections() = 0;
	virtual bool CreateRecordSingleStream(char frameType, unsigned long ulSessionId, unsigned long cid, IAVFRecordSingleStream **ppSingleStream) = 0;
	virtual bool WriteHeader(void *data, int len, unsigned long dwFlag) = 0;/*д�ļ�ͷ*/
	virtual bool Connect(const char* strAVCFile, bool bSyncAudio/*Ӳ���ɼ�����ʱtrue�� mic�ɼ�����ʱfalse*/, bool bUpfrontStream/*�Ƿ���Ԥ¼��*/) = 0;
	virtual void CombineUpFrontStream(unsigned long ulTimestamp, unsigned long ulSessionId, char* pData, int nLen) = 0;/*�ϲ�Ԥ¼��*/
	virtual bool Connect(const char* strAVCFile, unsigned long ulTimestamp) = 0; 
};
AFX_EXT_API bool CreateAVFRecordFactory(IAVFRecordFactory** ppARF, IAVFRecordEvent * pEvent);


/*Ԥ¼��ӿ�*/
class IAVFPreRecordFactory { 
public: virtual ~IAVFPreRecordFactory() {};
	virtual bool Connect(const char*lpszFileName, int nLen, bool bSyncAudio/*Ӳ���ɼ�����ʱtrue�� mic�ɼ�����ʱfalse*/) = 0;
	virtual void ReleaseConnections() = 0;
	virtual void WriteData(char* pData, int nLen,bool bKeyFrame, int nFrameType) = 0;
	virtual bool CombinFrameData(IAVFRecordFactory* pRecordFactory, int nLen) = 0; /*�ϲ�Ԥ¼��*/
};
AFX_EXT_CLASS bool CreateAVFPreRecordFactory(IAVFPreRecordFactory** ppARF);
#endif//__HPRECORD_H__