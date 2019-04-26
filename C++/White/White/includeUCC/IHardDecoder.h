// [6/12/2009 lzy]
//-------------------------------------------
//	解码卡框架
//-------------------------------------------
#pragma once
//厂商(解码器类型)
#define	HPNVS_XMC	0/*华平*/
#define HPNVS_HIK	1/*海康*/
#define HPNVS_DHD   2/*大华*/
#define HPNVS_NZI	3/*南自*/

//定义一些错误码
#define CARD_SUCCESS			0
#define CARD_FAILED				1
#define CARD_NO_INPORT_USED		2			//没有输入端口可用
#define CARD_OPEN_CHANN_FAILED	3			//打开通道失败 
#define CARD_UNSUPPORT			4			//解码卡不支持此流

#define TM_FULL		1		//整个
#define TM_2x2		4		//4个窗口
#define TM_3x3		9		//9
#define TM_4x4		16		//16


#define HARD_DECODER_API extern "C" __declspec(dllimport)


//硬解码卡回调接口类
class IHardDecoderNotify 
{
	
public:
// 	virtual void OnDecoderNotifyImageChanged(unsigned short usWidth,unsigned short usHeight,unsigned char usFrameRate)=0;
// 	virtual void OnDecoderNotifyImageData(unsigned char*pData,int nLen)=0;
	virtual void OnNotifyResult(int relOutScreen,int Window,int Result)=0;//通知结果
};

//硬解码卡接口类
class IHardDecoder
{
public:
	IHardDecoder(void){};
	virtual~IHardDecoder(void){};
public:

	//-----------------------------------------------
	//功能:初始化解码卡，否则相关的解码卡功能不能使用
	//返回值:无
	//-----------------------------------------------
	virtual bool InitMDCard(void)=0;
	virtual void Release()=0;

	//-----------------------------------------------
	//功能:得到当前系统中输出端口个数
	//返回值:输出端口个数
	//-----------------------------------------------
	virtual int GetOutPortCount(void)=0;

	//-----------------------------------------------
	//功能:设置解码卡输出端口对应的模板类型
	//参数:OutNum:端口号 0-15  系统最多16路输出
	//	   TemplateType:模板类型,和用户端定义好
	//返回值:true:成功 false:失败
	//-----------------------------------------------
	virtual bool SetVideoOutMatchTemplate(int OutNum,int TemplateType)=0;

	//-----------------------------------------------
	// 功能:判断输出端口中的某个窗口是否已经被显输出显示
	// 参数: OutNum:端口号 0-15  系统最多16路输出
	//		 WindowNum:输出端口对应的模板上的窗口号 从0开始
	// 返回值: true:正在显示 false:没有显示
	//-----------------------------------------------
	virtual bool IsDisplay(int OutNum,int WindowNum)=0;

	//-----------------------------------------------
	// 功能:用户取消通道和解码卡窗口的关联显示
	// 参数: OutNum:端口号 0-15 系统最多16路输出
	//		 WindowNum:输出端口对应的模板上的窗口号 从0开始
	// 返回值: 无
	//-----------------------------------------------
	virtual void CloseDisplay(int OutNum,int WindowNum)=0;


	//-----------------------------------------------
	// 功能:显示通道到对应的解码器上,调用之前先判断此对应的窗口是否被显示,如果已经显示先关闭此对应的通道流
	// 参数: OutNum:端口号 0-15 一共最多16路输出
	//		 WindowNum:输出端口对应的模板上的窗口号 从0开始
	//		 pData: 通道数据 nLen:数据长度
	// 返回值: SUCCESS:设置正确 其它:相应的错误码
	//-----------------------------------------------
	virtual int  DisplayChannToDecoder(int OutNum,int WindowNum,const char*pData,int nLen)=0;

	//-----------------------------------------------
	// 功能:切换同一个输出端口上的两个窗口
	// 参数: OutNum:端口号
	//		 WindowNum1:窗口1 一定要已经打开
	//		 WindowNum2:窗口2
	// 返回值: true:切换成功 false:切换失败
	//-----------------------------------------------
	virtual bool SwitchWindow(int OutNum,int WindowNum1,int WindowNum2)=0;

	virtual void SetFilePathName(CString Name)=0;	//为了测试用
	virtual void FullScreen(int OutNum,int WindowNum,bool bFull)=0; //全屏指定的窗口
	virtual int GetFullIndex(int OutNum)=0;		//得到屏幕的全屏窗口

	virtual int  GetMaxWindow(int OutNum)=0;		//得到输出端口对应的最大窗口个数 
	virtual bool IsSupport(const char* pData,DWORD nLen)=0;
	virtual bool IsMaxInput()=0;		//是否已经达到最大输入个数
	virtual int GetMaxInput()=0;			//得到最大输入个数
	

};
// [7/8/2009 lzy]
//动态加载dll

// 功能:得到当前硬解码器类型
// 返回值: HPNVS_HIK:海康  -1代表其它 
HARD_DECODER_API int GetType(void);


//功能:根据类型创建硬解码卡
//参数说明: nMDType:解码卡类型 HPNVS_HIK等 
//			rNotify:通知回调接口
//			nReserved:备用
//返回值:对应的解码卡接口对象
HARD_DECODER_API IHardDecoder* CreateHardDecoder(int nMDType, IHardDecoderNotify* pNotify,int nReserved);

typedef int (*GETTYPE) (void);
typedef IHardDecoder* (*CREATEDECODER) (int,IHardDecoderNotify*,int);