#ifndef __HPRECORD_H__
#define __HPRECORD_H__


//音频同步
#define AVF_SYNC_TIME				350 //等待音频的时间
//分割大小
#define AVF_FILE_LEN				300 //文件分割大小
//录像主类型
#define AVF_RECORD_NORMAL			0	//常规录像
#define AVF_RECORD_ALERT			1	//报警录像

//帧类型
#define	AVF_FRAME_TYPE_VIDEO		3	//视频流
#define	AVF_FRAME_TYPE_AUDIO		4	//音频流


/*不用输入，库里定
#define AVF_HEADER_SIZE				0   //结构体长度
#define AVF_HEADER_FLAG				1   //录像版本号
#define AVF_HEADER_START_TIME		4   //开始时间			[12字节]
#define AVF_HEADER_STOP_TIME		5   //结束时间			[12字节]
#define	AVF_HEADER_CHECKSUM			13	//篡改校验码		[32字节]	
*/
#define	AVF_HEADER_PWD				2   //MD5密码			[128字节]
#define	AVF_HEADER_RECORD_TYPE		3   //录像类型			[1字节]		[0:常规录像,1:报警录像]
#define AVF_HEADER_DEV				6	//设备				[128字节]
#define AVF_HEADER_CHN				7	//通道				[1字节]
#define	AVF_HEADER_DESC				8	//录像描述			[256字节]
#define	AVF_HEADER_PREV_FILE		9	//上一个文件名称	[128字节]
#define	AVF_HEADER_NEXT_FILE		10	//下一个文件名称	[128字节]
#define	AVF_HEADER_UPLOAD_STATE     11	//上传状态			[1字节]		[0=未上传,1=正上传,2=上传完,3=校正完]
#define	AVF_HEADER_UPLOAD_BYTES     12	//上传字节数		[4字节]
#define	AVF_HEADER_TIMESTAMP		14	//开始时间戳		[4字节]
#define	AVF_HEADER_VERSION			15  //产品版本号		[128字节]	[如:(AVCON4.x.xx AVCON5.x.xx AVCON6.x.xx NDVR2.x.xx)]

//录像回调接口
class IAVFRecordEvent
{
public:
	virtual void OnAVCRecordFactoryEvent_FullFile() = 0;
};

// 时间帧事件
class IAVFTimestampEvent
{
public:
	virtual void OnClickTimeFrame(unsigned long ulTimestamp) = 0;
};

// 单路流录像接口
class IAVFRecordSingleStream
{
public:
	virtual void WriteData(char* pData, int nLen, bool bKeyFrame) = 0;
	virtual void NotifyStreamClose() = 0;
};

// 录像工厂接口
class IAVFRecordFactory
{
public:
	/*bSyncAudio参数主要用于监控，硬件采集声音时true， mic采集声音时false*/
	/*bUpfrontStream参数表示是否有预先录像流*/
	virtual bool Connect(const char* strAVCFile, bool bSyncAudio, bool bUpfrontStream) = 0;
	virtual void ReleaseConnections() = 0;
	virtual bool CreateRecordSingleStream(char frameType, unsigned long ulSessionId, unsigned long cid, IAVFRecordSingleStream **ppSingleStream) = 0;
	/*合并预录流*/
	virtual void CombineUpFrontStream(unsigned long ulTimestamp, unsigned long ulSessionId, char* pData, int nLen) = 0;
	/*写文件头*/
	virtual bool WriteHeader(void *data, int len, DWORD dwFlag) = 0;
};

// 预录像工厂接口(主要用于监控)
class IAVFPreRecordFactory
{
public:
	/*bSyncAudio参数主要用于监控，硬件采集声音时true， mic采集声音时false*/
	virtual bool Connect(const char*lpszFileName, int nLen, bool bSyncAudio) = 0;
	virtual void ReleaseConnections() = 0;
	virtual void WriteData(char* pData, int nLen,bool bKeyFrame, int nFrameType) = 0;
	//合并预录流
	virtual bool CombinFrameData(IAVFRecordFactory* pRecordFactory, int nLen) = 0;
};

AFX_EXT_API bool CreateAVFRecordFactory(IAVFRecordFactory** ppARF, IAVFRecordEvent * pEvent);
AFX_EXT_CLASS bool CreateAVFPreRecordFactory(IAVFPreRecordFactory** ppARF);


#endif//__HPRECORD_H__