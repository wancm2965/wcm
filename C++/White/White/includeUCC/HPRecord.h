#ifndef __HPRECORD_H__
#define __HPRECORD_H__


//��Ƶͬ��
#define AVF_SYNC_TIME				350 //�ȴ���Ƶ��ʱ��
//�ָ��С
#define AVF_FILE_LEN				300 //�ļ��ָ��С
//¼��������
#define AVF_RECORD_NORMAL			0	//����¼��
#define AVF_RECORD_ALERT			1	//����¼��

//֡����
#define	AVF_FRAME_TYPE_VIDEO		3	//��Ƶ��
#define	AVF_FRAME_TYPE_AUDIO		4	//��Ƶ��


/*�������룬���ﶨ
#define AVF_HEADER_SIZE				0   //�ṹ�峤��
#define AVF_HEADER_FLAG				1   //¼��汾��
#define AVF_HEADER_START_TIME		4   //��ʼʱ��			[12�ֽ�]
#define AVF_HEADER_STOP_TIME		5   //����ʱ��			[12�ֽ�]
#define	AVF_HEADER_CHECKSUM			13	//�۸�У����		[32�ֽ�]	
*/
#define	AVF_HEADER_PWD				2   //MD5����			[128�ֽ�]
#define	AVF_HEADER_RECORD_TYPE		3   //¼������			[1�ֽ�]		[0:����¼��,1:����¼��]
#define AVF_HEADER_DEV				6	//�豸				[128�ֽ�]
#define AVF_HEADER_CHN				7	//ͨ��				[1�ֽ�]
#define	AVF_HEADER_DESC				8	//¼������			[256�ֽ�]
#define	AVF_HEADER_PREV_FILE		9	//��һ���ļ�����	[128�ֽ�]
#define	AVF_HEADER_NEXT_FILE		10	//��һ���ļ�����	[128�ֽ�]
#define	AVF_HEADER_UPLOAD_STATE     11	//�ϴ�״̬			[1�ֽ�]		[0=δ�ϴ�,1=���ϴ�,2=�ϴ���,3=У����]
#define	AVF_HEADER_UPLOAD_BYTES     12	//�ϴ��ֽ���		[4�ֽ�]
#define	AVF_HEADER_TIMESTAMP		14	//��ʼʱ���		[4�ֽ�]
#define	AVF_HEADER_VERSION			15  //��Ʒ�汾��		[128�ֽ�]	[��:(AVCON4.x.xx AVCON5.x.xx AVCON6.x.xx NDVR2.x.xx)]

//¼��ص��ӿ�
class IAVFRecordEvent
{
public:
	virtual void OnAVCRecordFactoryEvent_FullFile() = 0;
};

// ʱ��֡�¼�
class IAVFTimestampEvent
{
public:
	virtual void OnClickTimeFrame(unsigned long ulTimestamp) = 0;
};

// ��·��¼��ӿ�
class IAVFRecordSingleStream
{
public:
	virtual void WriteData(char* pData, int nLen, bool bKeyFrame) = 0;
	virtual void NotifyStreamClose() = 0;
};

// ¼�񹤳��ӿ�
class IAVFRecordFactory
{
public:
	/*bSyncAudio������Ҫ���ڼ�أ�Ӳ���ɼ�����ʱtrue�� mic�ɼ�����ʱfalse*/
	/*bUpfrontStream������ʾ�Ƿ���Ԥ��¼����*/
	virtual bool Connect(const char* strAVCFile, bool bSyncAudio, bool bUpfrontStream) = 0;
	virtual void ReleaseConnections() = 0;
	virtual bool CreateRecordSingleStream(char frameType, unsigned long ulSessionId, unsigned long cid, IAVFRecordSingleStream **ppSingleStream) = 0;
	/*�ϲ�Ԥ¼��*/
	virtual void CombineUpFrontStream(unsigned long ulTimestamp, unsigned long ulSessionId, char* pData, int nLen) = 0;
	/*д�ļ�ͷ*/
	virtual bool WriteHeader(void *data, int len, DWORD dwFlag) = 0;
};

// Ԥ¼�񹤳��ӿ�(��Ҫ���ڼ��)
class IAVFPreRecordFactory
{
public:
	/*bSyncAudio������Ҫ���ڼ�أ�Ӳ���ɼ�����ʱtrue�� mic�ɼ�����ʱfalse*/
	virtual bool Connect(const char*lpszFileName, int nLen, bool bSyncAudio) = 0;
	virtual void ReleaseConnections() = 0;
	virtual void WriteData(char* pData, int nLen,bool bKeyFrame, int nFrameType) = 0;
	//�ϲ�Ԥ¼��
	virtual bool CombinFrameData(IAVFRecordFactory* pRecordFactory, int nLen) = 0;
};

AFX_EXT_API bool CreateAVFRecordFactory(IAVFRecordFactory** ppARF, IAVFRecordEvent * pEvent);
AFX_EXT_CLASS bool CreateAVFPreRecordFactory(IAVFPreRecordFactory** ppARF);


#endif//__HPRECORD_H__