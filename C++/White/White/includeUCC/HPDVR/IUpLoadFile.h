

typedef struct
{
	std::string sfileName;
	std::string sMId;
	std::string sChannelId;
	std::string sRtype;
	std::string sStartTime;
}_struct_SENDFILE_INFO;

#define MSG_SEND_FILE  WM_USER +101

class IUploadFileInterface
{

public:
	//设置录像服务器地址
	virtual void SetRSSInfo(std::string sSvrIp0,std::string sSvrIp1,std::string sSvrIp2,int nPort)=0;
	//如果调用了SendFile，就不能在调用SendAllFile
	
	//传输文件
	virtual bool  SendFile(int nId ,_struct_SENDFILE_INFO &info) =0;
	
	//发送文件夹的在nYear/nMonth/nDay之后的所有的文件	
	virtual void SendAllFile(std::string sFolder,int nYear = 1999,int nMonth = 1,int nDay =1) =0;
	
	//设置检查是否有新的传输文件的时间间隔 默认为3分钟
	virtual void SetChenkSenderFileInterval(int nMin = 3) =0;
};

AFX_EXT_CLASS IUploadFileInterface * CreateUploadFileProcess(CWnd * pParent);
AFX_EXT_CLASS void ReleaseUploadFileProcess(IUploadFileInterface * pUpLoadFile);

