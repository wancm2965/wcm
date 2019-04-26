

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
	//����¼���������ַ
	virtual void SetRSSInfo(std::string sSvrIp0,std::string sSvrIp1,std::string sSvrIp2,int nPort)=0;
	//���������SendFile���Ͳ����ڵ���SendAllFile
	
	//�����ļ�
	virtual bool  SendFile(int nId ,_struct_SENDFILE_INFO &info) =0;
	
	//�����ļ��е���nYear/nMonth/nDay֮������е��ļ�	
	virtual void SendAllFile(std::string sFolder,int nYear = 1999,int nMonth = 1,int nDay =1) =0;
	
	//���ü���Ƿ����µĴ����ļ���ʱ���� Ĭ��Ϊ3����
	virtual void SetChenkSenderFileInterval(int nMin = 3) =0;
};

AFX_EXT_CLASS IUploadFileInterface * CreateUploadFileProcess(CWnd * pParent);
AFX_EXT_CLASS void ReleaseUploadFileProcess(IUploadFileInterface * pUpLoadFile);

