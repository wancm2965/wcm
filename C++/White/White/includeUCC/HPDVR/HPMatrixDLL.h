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

typedef struct  //摄像头转动控制的方向
{
    unsigned short cameranum;//摄像机号
	unsigned short up;
	unsigned short down;
	unsigned short left;
	unsigned short right;
}direction_info;
typedef struct //矩阵的信号流
{
	int inputPort; //1开始...
	int outputPort;//1开始...
}signal_line;

enum command
{
	MATRIX_LOGIN,          //表示有客户端登陆
	MATRIX_LOGOUT,         //客户端登出
	MATRIX_SINGAL_LINE,    //改变矩阵的输入端和输出端
	MATRIX_CAMERA_BOLL,    //操控杆
	MATRIX_UPDATE,         //更新状态
	MATRIX_UPDATE_END,     //结束更新
	MATRIX_MODIFY_CHNNAME, //修改通道名称
	MATRIX_MODIFY_TYPE,	   //修改矩阵类型
	MATRIX_MODIFY_COM,	   //修改串口参数
	MATRIX_REQUEST_INFO,   //界面请求数据
    MATRIX_FEEDBACK_UPDATE,//反馈更新
	MATRIX_FEEDBACK_LOGIN, //反馈登陆动作
	MATRIX_FEEDBACK_SINGALLIN, //反馈矩阵输入和输出的改变
	MATRIX_TRADE_NAME          //厂商
};
enum updateType
{
	UPDATE_SINGAL_LINE,
	UPDATE_LOGIN,
	UPDATE_LOGOUT,
};
class IMatrix_Server_Notify//服务端的界面通告接口
{
public:
	virtual void On_NotifyInitMatrixCom()=0;
	virtual void On_NotifyOpenMatrixCom(int comPort,int baud,int mode)=0;
	virtual void On_NotifyCloseMatrixCom(int comPort)=0;

	virtual void On_NotifyMatrixSignalLines(const signal_line &signal)=0;//通知NDVR，哪路通道修改了
    virtual void On_NotifyMatrixDeviceName(const char* name,unsigned short channels,unsigned short outchannels)=0;//通知NDVR 哪路通道的名字修改了

	virtual unsigned short On_NotifyGetLocalport()=0;//获取本地的端口号
	virtual unsigned short On_NotifyGetComport()=0;//获得串口号
	virtual unsigned short On_NotifyGetBaud()=0;//获取波特率
	virtual unsigned short On_NotifyGetCommode()=0;//获取串口模式
	virtual unsigned short On_NotifyGetOutputNum()=0;
	virtual unsigned short On_NotifyGetInputNum()=0;
protected:
private:
};

class IMatrix_Server//矩阵控制的服务端接口
{
public:
	
	virtual bool OpenMatrixCom(int port,int baud,int bit,int parity,int stopbit)=0;           //打开串口
	virtual bool CloseMatrixCom(int port)=0;                            //关闭串口
	virtual bool Release()=0;                                           //关闭串口，以及断开网络连接
	virtual void SetNumOfInput(unsigned short num)=0;                   //设置矩阵的输入口数目
	virtual void SetNumOfOutput(unsigned short num)=0;                  //设置矩阵的输出口数目
	virtual void Set_From_To(const signal_line &signal)=0;              //设置从第矩阵第几路输入到第几路输出
    virtual void Set_Channels_name(const char* name,unsigned short channels)=0;                                 //设置通道的名称
    virtual void Init()=0;                                              //初始化设置
    virtual CString GetChannels_name(int channles)=0;
    virtual int GetNumOfInput()=0;
	virtual int GetNumOfOutput()=0;
	virtual void Control_CameraBoll(const direction_info &direction)=0; //控制矩阵本地摄像头方向
	virtual bool Connect(unsigned short localport=4200)=0;              //这是网络连接部分

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
class IMatrix_Client_Notify//客户端的界面通告接口
{
public:
	virtual void On_NotifyInitMatrixCom()=0;
	virtual void On_NotifyOpenMatrixCom()=0;
	virtual void On_NotifyCloseMatrixCom()=0;
	virtual void On_NotifyMatrixSignalLines()=0;
    virtual void On_NotifyUpdate(unsigned short type,void *data)=0; //通知界面状态的更新
	virtual unsigned short On_NotifyGetLocalport()=0;//获取本地的端口号
	virtual unsigned short On_NotifyGetServerport()=0;//服务器端口号
	virtual char* On_NotifyGetServerip()=0;//服务器ip
	virtual char* On_NotifyGetUserID()=0;//客户端的名字
protected:
private:
};
class IMatrix_Client//矩阵控制的客户端接口
{
public:
	virtual void SendCommand(unsigned short cmd,void *data,int len)=0;//发送命令字
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
	virtual void FlashWnd()=0;//清空界面的显示
	virtual void SetMatrixChannels(const signal_line& signal)=0;//修改矩阵通道
	virtual void SetMatrixChannelsName(const char* name,unsigned short channels)=0;//修改通道名称
	virtual void SetMatrixChangeComPort(int port)=0;//改变串口的设置
	virtual void SetMatrixChangeComBaud(int baud)=0;
	virtual void SetMatrixChangeComBit(int bit)=0;
    virtual void SetMatrixChangeComParity(int parity)=0;
	virtual void SetMatrixChangeComStopbit(int stopbit)=0;
	virtual void SetMatrixChangeComMode(int mode)=0;//改变 16X4
	virtual bool IsVisibale(void) = 0;
    virtual void SetRight(bool beAVCON) = 0;
	virtual void UpdataResult(void) = 0;//更新结果栏
	virtual void SetMatrixTradeName(unsigned short tradenameindex) = 0;//设置厂商的名称
};
class IMatrix_Wnd_Notify
{
public:
    virtual void On_NotifyWndChannelsChanged(const signal_line& signal,int cmd=-1)=0;//修改矩阵通道,cmd=-1表示是本地界面触发
	virtual void On_NotifyWndChannelsNameChanged(const char* name,unsigned short channels,int cmd=-1)=0;//修改矩阵通道名,cmd=-1表示是本地界面触发
    virtual void On_NotifyWndChangeComState(int port,int baud,int bit,int parity,int stopbit,int cmd=-1)=0;//表示矩阵串口设置的修改,cmd=-1表示是本地界面的触发
	virtual void On_NotifyWndStart()=0;//启动工作,不对外调用，在IMatrix_Wnd初始化的时候自动调用,要触发远端FlashWnd
    virtual void On_NotifyWndChangeMode(int modeIndex,int cmd=-1)=0;
    virtual void On_NotifyWndChangeTradeName(int tradenameindex) = 0;
};
MAXTRIX_EXT_API IMatrix_Server* CreateMatrixServer(IMatrix_Server_Notify *pNotify);
MAXTRIX_EXT_API IMatrix_Client* CteateMatrixClient(IMatrix_Client_Notify *pNotify);
MAXTRIX_EXT_API IMatrix_Wnd* CreateMatrixWnd(IMatrix_Wnd_Notify* pNotify,CWnd* pWnd);
#endif