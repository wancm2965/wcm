#ifndef __HPMatrixDLL_H_
#define __HPMatrixDLL_H_
#ifdef WIN32
#ifdef MATRIX_EXPORTS
#define MAXTRIX_EXT_API __declspec(dllexport)
#else
#define MAXTRIX_EXT_API __declspec(dllimport)
#endif
#else
#define MAXTRIX_EXT_API extern
#endif

typedef struct  //����ͷת�����Ƶķ���
{
    unsigned short cameranum;//�������
	unsigned short up;
	unsigned short down;
	unsigned short left;
	unsigned short right;
}direction_info;
typedef struct //������ź���
{
	int inputPort; //1��ʼ...
	int outputPort;//1��ʼ...
}signal_line;

enum command
{
	MATRIX_LOGIN,          //��ʾ�пͻ��˵�½
	MATRIX_LOGOUT,         //�ͻ��˵ǳ�
	MATRIX_SINGAL_LINE,    //�ı���������˺������
	MATRIX_CAMERA_BOLL,    //�ٿظ�
	MATRIX_UPDATE,         //����״̬
	MATRIX_UPDATE_END,     //��������
	MATRIX_MODIFY_CHNNAME, //�޸�ͨ������
	MATRIX_MODIFY_TYPE,	   //�޸ľ�������
	MATRIX_MODIFY_COM,	   //�޸Ĵ��ڲ���
	MATRIX_REQUEST_INFO,   //������������
    MATRIX_FEEDBACK_UPDATE,//��������
	MATRIX_FEEDBACK_LOGIN, //������½����
	MATRIX_FEEDBACK_SINGALLIN, //�����������������ĸı�
	MATRIX_TRADE_NAME          //����
};
enum updateType
{
	UPDATE_SINGAL_LINE,
	UPDATE_LOGIN,
	UPDATE_LOGOUT,
};
class IMatrix_Server_Notify//����˵Ľ���ͨ��ӿ�
{
public:
	virtual void On_NotifyInitMatrixCom()=0;
	virtual void On_NotifyOpenMatrixCom(int comPort,int baud,int mode)=0;
	virtual void On_NotifyCloseMatrixCom(int comPort)=0;

	virtual void On_NotifyMatrixSignalLines(const signal_line &signal)=0;//֪ͨNDVR����·ͨ���޸���
    virtual void On_NotifyMatrixDeviceName(const char* name,unsigned short channels,unsigned short outchannels)=0;//֪ͨNDVR ��·ͨ���������޸���

	virtual unsigned short On_NotifyGetLocalport()=0;//��ȡ���صĶ˿ں�
	virtual unsigned short On_NotifyGetComport()=0;//��ô��ں�
	virtual unsigned short On_NotifyGetBaud()=0;//��ȡ������
	virtual unsigned short On_NotifyGetCommode()=0;//��ȡ����ģʽ
	virtual unsigned short On_NotifyGetOutputNum()=0;
	virtual unsigned short On_NotifyGetInputNum()=0;
protected:
private:
};

class IMatrix_Server//������Ƶķ���˽ӿ�
{
public:
	
	virtual bool OpenMatrixCom(int port,int baud,int bit,int parity,int stopbit)=0;           //�򿪴���
	virtual bool CloseMatrixCom(int port)=0;                            //�رմ���
	virtual bool Release()=0;                                           //�رմ��ڣ��Լ��Ͽ���������
	virtual void SetNumOfInput(unsigned short num)=0;                   //���þ�����������Ŀ
	virtual void SetNumOfOutput(unsigned short num)=0;                  //���þ�����������Ŀ
	virtual void Set_From_To(const signal_line &signal)=0;              //���ôӵھ���ڼ�·���뵽�ڼ�·���
    virtual void Set_Channels_name(const char* name,unsigned short channels)=0;                                 //����ͨ��������
    virtual void Init()=0;                                              //��ʼ������
    virtual CString GetChannels_name(int channles)=0;
    virtual int GetNumOfInput()=0;
	virtual int GetNumOfOutput()=0;
	virtual void Control_CameraBoll(const direction_info &direction)=0; //���ƾ��󱾵�����ͷ����
	virtual bool Connect(unsigned short localport=4200)=0;              //�����������Ӳ���

	virtual int  GetComPortIndex()=0;
	virtual int  GetComBaudIndex()=0;
	virtual int  GetComBitIndex()=0;
	virtual int  GetComParityIndex()=0;
	virtual int  GetComStopbitIndex()=0;
    virtual int  GetOutportNum(int channels)=0;
	virtual int  GetMatrixTypeIndex()=0;
	virtual void SetMatrixTypeIndex(int modeIndex)=0;
	virtual void SetMatrixTradeName(unsigned short index)=0;
	virtual int  GetMatrixTradeName()=0;
protected:
private:
};
class IMatrix_Client_Notify//�ͻ��˵Ľ���ͨ��ӿ�
{
public:
	virtual void On_NotifyInitMatrixCom()=0;
	virtual void On_NotifyOpenMatrixCom()=0;
	virtual void On_NotifyCloseMatrixCom()=0;
	virtual void On_NotifyMatrixSignalLines()=0;
    virtual void On_NotifyUpdate(unsigned short type,void *data)=0; //֪ͨ����״̬�ĸ���
	virtual unsigned short On_NotifyGetLocalport()=0;//��ȡ���صĶ˿ں�
	virtual unsigned short On_NotifyGetServerport()=0;//�������˿ں�
	virtual char* On_NotifyGetServerip()=0;//������ip
	virtual char* On_NotifyGetUserID()=0;//�ͻ��˵�����
protected:
private:
};
class IMatrix_Client//������ƵĿͻ��˽ӿ�
{
public:
	virtual void SendCommand(unsigned short cmd,void *data,int len)=0;//����������
protected:
private:
};
class IMatrix_Wnd
{
public:
	virtual ~IMatrix_Wnd(){}
public:
	virtual void ShowWnd()=0;
	virtual void HideWnd()=0;
	virtual void ReleaseWnd()=0;
	virtual void FlashWnd()=0;//��ս������ʾ
	virtual void SetMatrixChannels(const signal_line& signal)=0;//�޸ľ���ͨ��
	virtual void SetMatrixChannelsName(const char* name,unsigned short channels)=0;//�޸�ͨ������
	virtual void SetMatrixChangeComPort(int port)=0;//�ı䴮�ڵ�����
	virtual void SetMatrixChangeComBaud(int baud)=0;
	virtual void SetMatrixChangeComBit(int bit)=0;
    virtual void SetMatrixChangeComParity(int parity)=0;
	virtual void SetMatrixChangeComStopbit(int stopbit)=0;
	virtual void SetMatrixChangeComMode(int mode)=0;//�ı� 16X4
	virtual bool IsVisibale(void) = 0;
    virtual void SetRight(bool beAVCON) = 0;
	virtual void UpdataResult(void) = 0;//���½����
	virtual void SetMatrixTradeName(unsigned short tradenameindex) = 0;//���ó��̵�����
};
class IMatrix_Wnd_Notify
{
public:
    virtual void On_NotifyWndChannelsChanged(const signal_line& signal,int cmd=-1)=0;//�޸ľ���ͨ��,cmd=-1��ʾ�Ǳ��ؽ��津��
	virtual void On_NotifyWndChannelsNameChanged(const char* name,unsigned short channels,int cmd=-1)=0;//�޸ľ���ͨ����,cmd=-1��ʾ�Ǳ��ؽ��津��
    virtual void On_NotifyWndChangeComState(int port,int baud,int bit,int parity,int stopbit,int cmd=-1)=0;//��ʾ���󴮿����õ��޸�,cmd=-1��ʾ�Ǳ��ؽ���Ĵ���
	virtual void On_NotifyWndStart()=0;//��������,��������ã���IMatrix_Wnd��ʼ����ʱ���Զ�����,Ҫ����Զ��FlashWnd
    virtual void On_NotifyWndChangeMode(int modeIndex,int cmd=-1)=0;
    virtual void On_NotifyWndChangeTradeName(int tradenameindex) = 0;
};
MAXTRIX_EXT_API IMatrix_Server* CreateMatrixServer(IMatrix_Server_Notify *pNotify);
MAXTRIX_EXT_API IMatrix_Client* CteateMatrixClient(IMatrix_Client_Notify *pNotify);
MAXTRIX_EXT_API IMatrix_Wnd* CreateMatrixWnd(IMatrix_Wnd_Notify* pNotify,CWnd* pWnd);
#endif