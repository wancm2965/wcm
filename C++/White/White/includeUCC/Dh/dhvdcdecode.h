//
//FileName: dhvdecdecode.h
//describe: 该头文件用来描述解码卡SDK功能的接口函数，错误号等信息。
//Date:		2007-11-14	create
//			2007-11-22	modify 增加了对pnx1700系列解码卡的支持
//			2007-12-28	modify 增加一个用于获取DSP初始化进度的接口函数（这一组函数对外不开放） 
//			2008-2-20	modify 增加了有关YUV数据直接送入解码卡显示的功能函数
//			2008-3-10	modify 增加了一组矩阵功能接口函数（未实现），
//						增加了一组文件播放控制，索引使用的接口函数（未实现）
//

#ifndef _DHVDCDECODE_
#define _DHVDCDECODE_

#ifndef VDCDECODE_API
#define VDCDECODE_API  extern "C" __declspec(dllexport) 
#endif

#define DHVDC_CALLMETHOD __stdcall
#define CALLBACK __stdcall

#define DHERR_SUCCESS 0                          //成功
#define DHERR_ALLOCATE_MEMORY -1                 //内存分配失败
#define DHERR_INVALID_HANDLE  -2                 //句柄无效
#define DHERR_DDRAW_CREATE_FAILED  -3            //DirectDraw创建失败
#define DHERR_DDRAW_CAPS_FAULT   -4              //DirectDraw获取显示设备能力失败
#define DHERR_SET_COOPERATIVELEVEL_FAILED  -5    //DirectDraw设置协作级别失败
#define DHERR_PRIMARY_SURFACE_CREATE_FAILED  -6  //DirectDraw创建主表面失败
#define DHERR_OVERLAY_CREATE_FAILED   -7         //DirectDraw创建overlay表面失败
#define DHERR_GET_OVERLAY_ADDRESS_FAILED   -8    //DirectDraw获取Overlay地址失败
#define DHERR_OVERLAY_UPDATE_FAILED   -9         //DirectDraw Update 失败
#define DHERR_SURFACE_NULL    -10                //DirectDraw 表面是NULL.
#define DHERR_FILEHEADER_UNKNOWN    -11          //文件头错误
#define DHERR_CREATE_FILE_FAILED    -12          //创建（打开）文件失败
#define DHERR_FILE_SIZE_ZERO   -13               //文件长度太短
#define DHERR_CREATE_OBJ_FAILED   -14            //创建线程或内核对象失败
#define DHERR_CHANNELMAGIC_MISMATCH   -15        //通道数据毁坏
#define DHERR_PARA_OVER    -16                   //参数非法
#define DHERR_ORDER   -17                        //调用顺序不对
#define DHERR_COMMAND   -18                      //命令传递失败
#define DHERR_UNSUPPORTED    -19                 //不支持
#define DHERR_DSPOPEN   -20                      //DSP 打开失败
#define DHERR_DSPLOAD   -21                      //DSP 下载失败
#define DHERR_ALLOCATE_DSPMEMORY   -22           //DSP 内存分配失败
#define DHERR_DSPCHECHER   -23                   //DSP 校验失败
#define DHERR_IMGFILE_UNKNOWN   -24              //未知的IMG文件
#define DHERR_INVALID_FILE    -25                //无效的文件
#define DHERR_INIT_FAILED	-26					 //初始化DSP失败
#define DHERR_TIMEOUT		-27					 //等待超时 
#define DHERR_NOEXIST		-28					 //没有解码卡存在	

typedef struct tagDSPDetail
{
	UINT encodeChannelCount;			//板卡包含的编码通道个数
	UINT firstEncodeChannelIndex;		//板卡上第一个编码通道的索引
	UINT decodeChannelCount;			//板卡包含的解码通道个数
	UINT firstDecodeChannelIndex;		//板卡上第一个解码通道的索引
	UINT displayChannelCount;			//板卡包含的视频输出通道个数
	UINT firstDisplayChannelIndex;		//板卡上第一个视频输出通道的索引
	UINT reserved1;						/*reserve*/
	UINT reserved2;						/*reserve*/
	UINT reserved3;						/*reserve*/
	UINT reserved4;						/*reserve*/
}DSP_DETAIL, *pDSPDETAIL;

typedef struct
{
	int /*BOARD_TYPE_DS*/ type; //板卡类型. 0--PNX1500; 1--PNX1700
	BYTE sn[16]; //序列号
	UINT dspCount; //板卡包含的DSP个数
	UINT firstDspIndex; //板卡上第一个DSP在所有DSP中的索引
	UINT encodeChannelCount; //板卡包含的编码通道个数
	UINT firstEncodeChannelIndex; //板卡上第一个编码通道在所有编码通道中的索引
	UINT decodeChannelCount; //板卡包含的解码通道个数
	UINT firstDecodeChannelIndex; //板卡上第一个解码通道在所有解码通
	UINT displayChannelCount; //板卡包含的视频输出通道个数
	UINT firstDisplayChannelIndex ; //板卡上第一个视频输出通道在所有视频输出通道中的索引
	UINT reserved1;
	UINT reserved2;
	UINT reserved3;
	UINT reserved4;
}DS_BOARD_DETAIL;

typedef struct
{
	UINT left;// 区域左边界
	UINT top; //区域上边界
	UINT width; //区域宽度
	UINT height; //区域高度
	COLORREF color; //区域背景色
	UINT param; //区域扩展参数
}REGION_PARAM;

typedef struct tagVersion{
	ULONG nSDKVer;
	ULONG nSDKBuildTime;

	ULONG nDriverVer;
	ULONG nDriBuildTime;

	ULONG nDSPVer;
	ULONG nDSPBuildTime;
}DHVDCVER, *pDHVDCVersion;

typedef struct tagDisplayPara
{
	long bToScreen;
	long bToVideoOut;
	long nLeft;
	long nTop;
	long nWidth;
	long nHeight;
	long nReserved;
}DISPLAY_PARA,*PDISPLAY_PARA;

typedef enum 
{
	PAL = 1,
	NTSC = 2,
	UnKnow = 3
}VideoStandard_t;
typedef enum
{
	VGA_MODE=1,
	TV_MODE=2
}VideoOutMode;

/*
	有关进度条的回调函数，因为解码卡初始化时间较长，尤其是存在多块卡的情况。
	所以使用这个函数来导出初始化的信息，nCurrent表示当前初始的DSP序号，nTotal
	表示系统中总共的DSP数量，dwUser是提供给用户的操作信息，扩展用。对外不开放。
*/
typedef void (CALLBACK *fnDecProgressCB) (int nCurrent, int nTotal, DWORD dwUser);
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDECProgressCB(fnDecProgressCB fnCallback, DWORD dwUser);


//init part

/*
**	function name: DH_InitDecDevice
**	describe:	初始化解码卡，该函数是使用解码卡的必须操作，调用成功后，
**		方可进行之后的其他操作。
**	parameter: [out] 如果初始化成功，返回该地址上存储支持的解码通道个数。
**	return:	错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InitDecDevice(long *pDeviceTotal);

/*
**	function name: DH_ReleaseDecDevice
**	describe: 析构PC上为解码卡分配的资源，在程序结束退出时进行调用。
**	parameter: NULL
**  return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ReleaseDecDevice();

/*
**	有关这两个DirectDraw的函数，已经在程序内部处理，用户无需调用
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InitDirectDraw(HWND hParent, COLORREF colorKey);
VDCDECODE_API int DHVDC_CALLMETHOD DH_ReleaseDirectDraw();

/*
**	function name: DH_ChannelOpen
**	describe: 在初始化完成后，调用这个函数来获取操作的解码通道句柄。
**	parameter: nChannelNum[in] 需要获取某个解码通道的序号，从零开始
**			  HANDLE *phChannel[out] 对应输入的解码通道序号，分配对应的句柄 	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ChannelOpen(long nChannelNum, HANDLE *phChannel);

/*
**	function name:DH_ChannelClose
**	describe: 退出时调用该函数关闭解码通道。
**	parameter: [in] 解码通道句柄
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ChannelClose(HANDLE hChannel);

//Open Part
/*
**	function name:DH_OpenStream
**	describe: 使用流方式数据进行解码之前，调用该函数分配缓冲区的资源
**	parameter: HANDLE hChennel[in] 解码通道句柄
**			   PBYTE pFileHeadBuf[in] 必须置空
**			   DWORD dwSize [in] 缓冲区的大小			
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_OpenStream(HANDLE hChennel, PBYTE pFileHeadBuf, DWORD dwSize);

/*
**	function name:DH_ResetStream
**	describe: 清空对应解码缓冲区中的数据
**	parameter: [in] 解码通道句柄
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ResetStream(HANDLE hChannel);

/*
**	function name:DH_CloseStream
**	describe: 退出时调用该函数关闭解码通道的数据缓冲区。
**	parameter: [in] 解码通道句柄
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_CloseStream(HANDLE hChannel);

/*
**	function name:DH_InputData
**	describe: 送获取的流数据至对应的解码通道的缓冲中
**	parameter: HANDLE hChannel [in] 解码通道句柄
**			   PBYTE pBuf[in] 数据的缓冲区指针
**			   DWORD dwSize[int] 数据缓冲区大小		
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InputData(HANDLE hChannel, PBYTE pBuf, DWORD dwSize);

/*
**	function name:DH_OpenFile
**	describe: 使用文件回放方式进行解码上墙，这个功能与流方式解码上墙有些许不同，
**			使用这种方式上墙，不需要外界调用DH_InputData送数据，SDK内部会自动做一些处理。
**	parameter: HANDLE hChannel[in] 解码通道句柄
**			   LPTSTR szFileName[in] 文件的路径名指针	 
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_OpenFile(HANDLE hChannel, LPTSTR szFileName);

/*
**	function name:DH_CloseFile
**	describe: 退出时调用该函数关闭文件回放模式
**	parameter: [in] 解码通道句柄
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_CloseFile(HANDLE hChennel);

//Play part
/*
**	function name:DH_SetDisplayPara
**	describe: 设定显示的状态
**	parameter: [in] 解码通道句柄
**			   [in] 详见以下的结构体信息	
				结构说明：
				typedef struct{
				long bToScreen;
				long bToVideoOut;
				long nLeft;
				long nTop;
				long nWidth;
				long nHeight;
				long nReserved;
				}DISPLAY_PARA,*PDISPLAY_PARA;
				bToScreen 1输出到屏幕，0则反之。
				bToVideoOut 1否输出到VideoOut,0则反之。
				nleft, nTop, nWidth, nHeight 输出到屏幕上的范围，相对hParent（见3）客户区坐标。
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayPara(HANDLE hChannel, DISPLAY_PARA *pPara);

/*
**	function name:DH_Play
**	describe: 无论流方式还是文件方式，调用该函数成功后DSP开始进行解码并显示。
**	parameter: HANDLE hChannel[in] 解码通道句柄
**			   HWND hwnd [in] 解码后在PC端显示的窗口句柄	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_Play(HANDLE hChannel, HWND hwnd);

/*
**	function name:DH_Stop
**	describe: 停止解码显示
**	parameter: [in] 解码通道句柄
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_Stop(HANDLE hChannel);

/*
**	function name:DH_Pause
**	describe: 暂停\启动解码显示
**	parameter: HANDLE hChannel[in] 解码通道句柄
**			   ULONG bPause[in] 1为暂停解码，0为继续播放	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_Pause(HANDLE hChannel, ULONG bPause);

//sound part
/*
**	function name:DH_PlaySound
**	describe: 打开音频解码
**	parameter: [in] 解码通道句柄
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_PlaySound(HANDLE hChannel);

/*
**	function name:DH_StopSound
**	describe: 停止音频解码
**	parameter: [in] 解码通道句柄
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_StopSound(HANDLE hChannel);

/*
**	function name:DH_SetVolume(not use)
**	describe: 调节音量大小
**	parameter: [in] 解码通道句柄
**			   [in] 音量大小 0~0xffff。
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetVolume(HANDLE hChannel, ULONG nVolume);

//overlay part(not been used)
VDCDECODE_API int DHVDC_CALLMETHOD DH_RefreshSurface();
VDCDECODE_API int DHVDC_CALLMETHOD DH_ReStoreSurface();
VDCDECODE_API int DHVDC_CALLMETHOD DH_ClearSurface();


VDCDECODE_API int DHVDC_CALLMETHOD DH_ZoomOverlay(RECT *pSrcClientRect, RECT *pDecScreenRect);

//cut file(Record)
/*
**	function name:DH_StartCapFile
**	describe: 启动视频录像
**	parameter: [in] 解码通道句柄
**			   [in]	录像文件的路径名指针 
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_StartCapFile(HANDLE hChannel,LPTSTR sFileName);

/*
**	function name:DH_StopCapFile
**	describe: 停止视频录像
**	parameter: [in] 解码通道句柄
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_StopCapFile(HANDLE hChannel);

//capture picture
/*
**	function name:DH_GetYV12Image
**	describe: 获得截屏图像的YUV数据
**	parameter: [in] 解码通道句柄
**			   [out] yuv数据缓冲区指针
**			   [out] yuv数据缓冲区大小   		 
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetYV12Image(HANDLE hChannel, PBYTE pBuffer, ULONG nSize);

/*
**	function name:DH_GetPictureSize
**	describe: 获取截屏图片大小
**	parameter: [in] 解码通道句柄
**			   [out] 图片宽度
**			   [out] 图片高度	 
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetPictureSize(HANDLE hChannel,ULONG* pWidth, ULONG* pHeight);

/*
**	function name:DH_ConvertToBmpFile
**	describe: 将yuv数据转化成BMP图片，并进行保存。
**	parameter: BYTE * pBuf [in] yuv数据的缓冲区指针
**			   ULONG nSize [in] 缓冲区指针大小
**			   ULONG nWidth [in] 图片的宽度
**			   ULONG nHeight [in] 图片的高度
**			   char *sFileName [in] 保存图片的文件名称
**			   ULONG nReserved 保留					
**	return: 错误号
*/
VDCDECODE_API int 
DHVDC_CALLMETHOD DH_ConvertToBmpFile(BYTE * pBuf,ULONG nSize, ULONG nWidth,ULONG nHeight,
													   char *sFileName,ULONG nReserved);

//setting and getting  part
/*
**	function name:DH_Jump
**	describe: 文件播放中的前跳或者后跳，调用该函数后，
**			图像会根据设定的时间间隔进行向前或者向后的跳放。
**	parameter: [in] 解码通道句柄
**			   [in] 跳转的方向
**	return: 错误号
**	remark: 还没有实现
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_Jump(HANDLE hChannel,ULONG nDirection);

/*
**	function name:DH_SetJumpInterval
**	describe: 设定跳放的时间间隔
**	parameter: [in] 解码通道句柄
**			   [in] 时间间隔
**	return: 错误号
**	remark: 还没有实现
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetJumpInterval(HANDLE hChannel,ULONG nSecond);

/*
**	function name:DH_GetSpeed
**	describe: 返回解码的速度大小
**	parameter: [in] 解码通道句柄
**			   [out] 解码速度 (1, 2 ,3, 4, -1, -2, -3, -4)
**	return: 错误号
**	remark: 还没有实现
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetSpeed(HANDLE hChannel,long *pSpeed);

/*
**	function name:DH_SetSpeed
**	describe: 设定解码的速度大小
**	parameter: [in] 解码通道句柄
**			   [in] 解码速度((1, 2 ,3, 4, -1, -2, -3, -4)) 
**	return: 错误号
**	remark: 还没有实现
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetSpeed(HANDLE hChannel,long nSpeed);

/*
**	function name:DH_SetPlayPos
**	describe: 设定文件播放的位置（百分比）
**	parameter: [in] 解码通道句柄
**			   [in] 位置（0～100） 
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetPlayPos(HANDLE hChannel,ULONG nPos);

/*
**	function name:DH_SetPlayPos
**	describe: 获得文件播放的位置（百分比）
**	parameter: [in] 解码通道句柄
**			   [out] 位置（0～100） 
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetPlayPos(HANDLE hChannel,ULONG* pPos);

/*
**	function name:DH_GetVersion
**	describe: 获得版本信息
**	parameter: [out] 版本信息详见上结构体定义
**			   
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetVersion(pDHVDCVersion pVersion);

/*
**	function name:DH_GetCurrentFrameRate
**	describe: 获得当前的解码帧率
**	parameter: [in] 解码通道句柄
**			   [out] 帧率信息
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetCurrentFrameRate(HANDLE hChannel,ULONG* pFrameRate);

/*
**	function name:DH_GetCurrentFrameNum
**	describe: 获得当前的解码帧序号
**	parameter: [in] 解码通道句柄
**			   [out] 帧序号
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetCurrentFrameNum(HANDLE hChannel,ULONG* pFrameNum);

/*
**	function name:DH_GetFileTotalFrames
**	describe: 获得解码帧总数
**	parameter: [in] 解码通道句柄
**			   [out] 总帧数
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetFileTotalFrames(HANDLE hChannel,ULONG* pTotalFrames);

/*
**	function name:DH_GetFileTime
**	describe: 获得文件播放的时间长度
**	parameter: [in] 解码通道句柄
**			   [out] 时间长度
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetFileTime(HANDLE hChannel, ULONG* pFileTime);


/*
**	function name:DH_GetCurrentFrameTime
**	describe: 获得当前文件播放帧的时间
**	parameter: [in] 解码通道句柄
**			   [out] 当前时间
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetCurrentFrameTime(HANDLE hChannel,ULONG* pFrameTime);

/*
**	function name:DH_GetPlayedFrames
**	describe: 获得已经解码的帧数
**	parameter: [in] 解码通道句柄
**			   [out] 已经解码的帧数
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetPlayedFrames(HANDLE hChannel,ULONG *pDecVFrames);

/*
**	function name:DH_GetDeviceSerialNo
**	describe: 获得卡的序列号
**	parameter: [in] 解码通道句柄
**			   [out] 卡的序列号
**	return: 错误号
**	remark: 请使用GetBoardDetail
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetDeviceSerialNo(HANDLE hChannel,ULONG *pDeviceSerialNo);

/*
**	function name:DH_SetFileEndMsg
**	describe: 设定文件播放结束，会发一个消息到指定窗口，
**			让用户对该消息进行处理。
**	parameter: [in] 解码通道句柄
**			   [in] 指定的窗口句柄
**			   [in] 自定义消息ID号	 
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetFileEndMsg(HANDLE hChannel,HWND hWnd,UINT nMsg);

/*
**	function name:DH_SetStreamOpenMode
**	describe: 设定流播放的模式，做一个实时性和流畅性的选择
**	parameter: [in] 解码通道句柄
**			   [in] 0表示不做任何设定，1～5值越大显示越流畅，但是延时越长 
**	return: 错误号
**	remark: 未实现
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetStreamOpenMode(HANDLE hChannel,ULONG nMode);

/*
**	function name:DH_GetStreamOpenMode
**	describe: 获取流播放的模式
**	parameter: [in] 解码通道句柄
**			   [out] 设定的模式大小值 
**	return: 错误号
**	remark: 未实现
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetStreamOpenMode(HANDLE hChannel,ULONG *pMode);

/*
**	function name:DH_SetVideoOutStandard
**	describe: 设定输出制式
**	parameter: [in] 解码通道句柄
**			   [in] PAL, NTSC两种 
**	return: 错误号
**	remark: 未实现
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetVideoOutStandard(HANDLE hChannel,ULONG nStandard);

/*
**	function name:DH_SetDspDeadlockMsg
**	describe: 在DSP无响应时，向窗口发送特定消息
**	parameter: [in] 窗口句柄
**			   [in] 自定义消息ID 
**	return: 错误号
**	remark: 未实现
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDspDeadlockMsg(HWND hWnd,UINT nMsg);

/*
**	function name:DH_GetChannelNum
**	describe: 获得对应DSP的通道号
**	parameter: [in] DSP的序号
**			   [out] 返回的通道号
**			   [in] 要得到的通道号
**			   [out]实际获得通道号个数		
**	return: 错误号
**	remark: 请使用GetBoardDetail代替
*/
VDCDECODE_API int DHVDC_CALLMETHOD 
DH_GetChannelNum(long nDspNum,long *pChannelNum,ULONG nNumsToGet,ULONG * pNumsGotten);


/*
**	function name:DH_ResetDsp
**	describe: 调用该函数，重新启动DSP,内部自动保留当前的状态
**	parameter: [in] DSP序号
**	return: 错误号
**	remark: 未实现
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ResetDsp(long nDspNum);

/*
**	function name:DH_SetAudioPreview
**	describe: 开启或者关闭音频预览（PC端的声音播放）
**	parameter: [in] 解码通道句柄
**			   [in] TRUE:打开 FALSE:关闭
**	return: 错误号
**	remark: 
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetAudioPreview(HANDLE hChannel, BOOL bEnable);

/*
**	function name:DH_GetDspCount
**	describe: 调用该函数，获取总DSP数量，不需要初始化也可调用
**	parameter: NULL
**	return: DSP总数
*/
VDCDECODE_API unsigned int DHVDC_CALLMETHOD DH_GetDspCount();

/*
**	function name:DH_GetDspDetail
**	describe: 获取DSP详细信息
**	parameter: [in] DSP序号
**			   [out] DSP详细的信息，详见该结构体定义
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetDspDetail(UINT dspNum,DSP_DETAIL *pDspDetail);

/*
**	function name:DH_GetBoardCount
**	describe: 获取板卡详细信息
**	parameter: [in] 板卡序号（基于1...）,不能为0
**			   [out] 板卡详细的信息，详见该结构体定义
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetBoardDetail(UINT boardNum, DS_BOARD_DETAIL *pBoardDetail);

/*
**	function name:DH_OpenStreamEx
**	describe: 以音视频分开的方式打开流
**	parameter: [in] 解码通道句柄
**			   [in] 文件数据头指针
**			   [in] 文件头长度	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_OpenStreamEx(HANDLE hChannel, PBYTE pFileHeadBuf, DWORD nSize);

/*
**	function name:DH_OpenStreamEx
**	describe: 以音视频分开的方式关闭流
**	parameter: [in] 解码通道句柄
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_CloseStreamEx(HANDLE hChannel);

/*
**	function name:DH_InputVideoData
**	describe: 输入纯视频数据，在打开流之后才可以使用
**	parameter: [in] 解码通道句柄
**			   [in] 数据缓冲指针
**			   [in] 数据缓冲区大小		
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InputVideoData(HANDLE hChannel, PBYTE pBuf, DWORD dwSize);

/*
**	function name:DH_InputAudioData
**	describe: 输入纯音频数据，在打开流之后才可以使用
**	parameter: [in] 解码通道句柄
**			   [in] 数据缓冲指针
**			   [in] 数据缓冲区大小		
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InputAudioData(HANDLE hChannel,PBYTE pBuf,DWORD nSize);

/*
**	function name:DH_GetBoardCount
**	describe: 获得系统上的板卡数量
**	parameter:  NULL
**	return: 板卡数量
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetBoardCount();


/*
**	function name:DH_SetDefaultVideoStandard
**	describe: ：设置系统默认的视频制式。默认为PAL。
**				系统中所有视频输入通道中如果无视频输入，
**				则该通道会按照默认的制式处理。所有的视频输出通道的制式，
**				在系统启动时也会按照默认的制式处理。
**	parameter:  [in]视频制式
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDefaultVideoStandard(VideoStandard_t VideoStandard);

/*
**	function name:DH_GetDisplayChannelCount
**	describe: 获得系统上的显示通道的数量
**	parameter:  NULL
**	return: 显示通道数量
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetDisplayChannelCount();

/*
**	function name:DH_SetDisplayStandard
**	describe: 设置显示通道的视频制式
**	parameter:  [in] 视频显示通道索引
**				[in] 视频制式
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayStandard(UINT nDisplayChannel,VideoStandard_t VideoStandard);

/*
**	function name:DH_SetDisplayRegion
**	describe: 设置显示通道的视频制式
**	parameter:  [in] 视频显示通道索引(这个版本中该参数由解码句柄直接强制转化而来)
**				[in] 区域个数
**				[in] 区域参数
**				[in] 保留
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayRegion(UINT nDisplayChannel,UINT nRegionCount,
														REGION_PARAM *pParam,UINT nReserved);

/*
**	function name:DH_ClearDisplayRegion
**	describe: 把显示区域清空，显示SetDisplayRegion中所设置的背景色。Bit0－Bit15：对应区域1－16，对应位为1，则将该区域清空。
**	parameter: [in] 显示通道索引
**			   [in] 要清除的区域
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ClearDisplayRegion(UINT nDisplayChannel,UINT nRegionFlag);

/*
**	function name:DH_SetDisplayRegionPosition
**	describe: 改变某个显示区域的位置
**	parameter: [in] 显示通道索引
**			   [in] 原数据中将要处理的区域
**			   [in] UINT nLeft, UINT nTop调整后的位置	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayRegionPosition(UINT nDisplayChannel,
															   UINT nRegion, UINT nLeft, UINT nTop);

/*
**	function name:DH_FillDisplayRegion
**	describe: 用自定义的图像填充显示区域。pImage所指向图象的大小必须和SetDisplayRegion中设置的图像大小相同，否则图像无法正常显示。
**	parameter: [in] 显示通道索引
**			   [in] 要处理的区域
**			   [in] YUV420格式的图像指针	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_FillDisplayRegion(UINT nDisplayChannel,UINT nRegion, unsigned char *pImage);

/*
**	function name:DH_SetDecoderAudioOutput
**	describe: 设置解码通道的音频输出。将第nDecodeChannel路解码音频输出到所在MD卡上的第nOutputChannel个音频输出通道。
**			每块DS-4002MD和DS-4004MD卡，每一个DSP包含2路音频输出，对应于每一个DSP，nOutputChannel必须为0或1。
**			如果已经有其它的解码通道的音频在nOutputChannel输出，则系统会自动先将其停止。
**	parameter: [in] 解码通道索引
**			   [in] 开启、关闭
**			   [in] 要输出的显示通道索引	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDecoderAudioOutput(UINT nDecodeChannel,BOOL bOpen,UINT nOutputChannel);

/*
**	function name:DH_SetDecoderVideoOutput
**	describe: 设置解码通道的视频输出。将第nDecodeChannel路解码视频输出到所在MD卡上的第nOutputChannel个视频输出通道。
**	parameter: [in] 解码通道索引
**			   [in] 解码通道的输出端口	
**			   [in] 开启、关闭
**			   [in] 显示通道的索引	
**			   [in] 要输出的显示区域
**			   [in]	保留	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDecoderVideoOutput(UINT nDecodeChannel,UINT nPort,BOOL bOpen,
															UINT nDisplayChannel,UINT nDisplayRegion,UINT nReserved);
/*
**	function name:DH_SetDecoderVideoExtOutput
**	describe: 设置解码通道的视频外部输出（矩阵输出）。把视频图像从第nDecodeChannel个解码通道的第nPort路，
**		送到第nDisplayChannel(每块DS-4002MD卡有2个显示通道，每块DS-4004MD卡有4个显示通道，因此nDisplayChannel从0，1，2，3一直递增….)
**		个显示通道的第nDisplayRegion个区域。此函数中nPort必须为0或1（每个解码通道支持2路视频输出）。
**		如果bOpen为0，则nDisplayChannel、nDisplayRegion无意义。此函数实现解码通道的跨MD卡输出；。
**	parameter: [in] 解码通道索引
**			   [in] 解码通道的输出端口	
**			   [in] 开启、关闭
**			   [in] 显示通道的索引	
**			   [in] 要输出的显示区域
**			   [in]	保留	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDecoderVideoExtOutput(UINT nDecodeChannel,UINT nPort,BOOL bOpen,
															   UINT nDisplayChannel,UINT nDisplayRegion,UINT nReserved);
/*
**	function name:DH_SetFileRef
**	describe: 设定文件索引
**	parameter: [in] 解码通道句柄
**			   [in] 开启、关闭
**			   [in] 索引创建完成后回调函数
**	return: 错误号
*/
typedef void (*FILE_REF_DONE_CALLBACK)(UINT nChannel,UINT nSize);
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetFileRef(HANDLE hChannel,BOOL bEnable,FILE_REF_DONE_CALLBACK FileRefDoneCallback);

/*
**	function name:DH_GetFileAbsoluteTime
**	describe: ：得到文件的起止时间（绝对时间），SYSTEMTIME中毫秒参数无效，始终为零
**	parameter: [in] 解码通道句柄
**			   [in] 文件开始时间(毫秒参数无效，始终为零)
**			   [in] 文件结束时间(毫秒参数无效，始终为零)
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetFileAbsoluteTime( HANDLE hChannel, SYSTEMTIME *pStartTime,SYSTEMTIME *pEndTime);

/*
**	function name:DH_GetFileAbsoluteTime
**	describe: ：得到文件当前的绝对时间，(毫秒参数无效，始终为零)
**	parameter: [in] 解码通道句柄
**			   [in] 文件当前的绝对时间(毫秒参数无效，始终为零)
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetCurrentAbsoluteTime(HANDLE hChannel,SYSTEMTIME *pTime);

/*
**	function name:DH_LocateByAbsoluteTime
**	describe: ：：按照绝对时间定位文件。只在回放文件、且打开索引后有效，SYSTEMTIME中毫秒参数无效。
**				文件的起止时间由DH_GetFileAbsoluteTime函数获得。
**	parameter: [in] 解码通道句柄
**			   [in] 定位的绝对时间(毫秒参数无效，始终为零)
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_LocateByAbsoluteTime(HANDLE hChannel,SYSTEMTIME time);

/*
**	function name:DH_LocateByFrameNumber
**	describe: ：按照帧号定位文件。只在回放文件、且打开索引后有效，文件的所有帧个数由DH_GetFileTotalFrames函数获得。
**	parameter: [in] 解码通道句柄
**			   [in] 定位的帧序号
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_LocateByFrameNumber(HANDLE hChannel,UINT frmNum);

/*
**	function name:DH_ImportFileRef
**	describe: ：文件索引导入。要使用此功能，必须先在DH_SetFileRef中关掉文件索引功能，然后在DH_OpenFile之后，再导入索引。
**	parameter: [in] 解码通道句柄
**			   [in] 索引数据导入缓冲
**			   [in] 缓冲大小	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ImportFileRef(HANDLE hChannel,char *pBuffer,UINT nSize);

/*
**	function name:DH_ExportFileRef
**	describe: ：文件索引导出。
**	parameter: [in] 解码通道句柄
**			   [in] 索引数据导入缓冲
**			   [in] 缓冲大小，不能小于索引数据的长度，索引长度可以在生成索引的回调函数中获得	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ExportFileRef(HANDLE hChannel,char *pBuffer,UINT nSize);

/*
**	function name:DH_LocateByFrameNumber
**	describe: ：模拟视频输出的视频捕获。
**	parameter: [in] 模拟视频输出的通道号
**			   [in] TRUE/启动捕获，FALSE/停止捕获
**			   [in] 帧率
**			   [in] 图象的宽度(暂时支持4CIF宽度704，不支持缩放)
**			   [in] 图象的高度(暂时支持4CIF高度PAL：576，NTSC：480，不支持缩放)
**			   [in] 捕获后存数据的地址			
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayVideoCapture(UINT nDisplayChannel,BOOL bStart,UINT fps,
															 UINT width,UINT height,unsigned char *imageBuffer);

/*
**	function name:RegisterDisplayVideoCaptureCallback
**	describe: 注册获取模拟视频输出的回调函数。
**	parameter: [in] IMAGE_STREAM_CALLBACK 回调函数
**			   [in] nDisplayChannel 模拟视频输出的通道号
**			   [in] void *context 调用回调函数时提供的上下文
**	return: 错误号
*/
typedef void (CALLBACK *IMAGE_STREAM_CALLBACK)(UINT nDisplayChannel,void *context);
VDCDECODE_API int DHVDC_CALLMETHOD DH_RegisterDisplayVideoCaptureCallback(IMAGE_STREAM_CALLBACK DisplayVideoCaptureCallback, void *context);

/*
**	function name:DH_SetDisplayVideoBrightness
**	describe: ：设置模拟视频输出的亮度。
**	parameter: [in] 模拟视频输出的通道号
**			   [in] 亮度值（0～255）
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayVideoBrightness(UINT chan,int Brightness);

/*
**	function name:DH_SetDecoderPostProcess
**	describe: ：设置解码后处理。
**	parameter: [in] 解码通道句柄
**			   [in] bit 0，1：设置去闪烁功能；0：否；
**				其它位保留。
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDecoderPostProcess(HANDLE hChannel,UINT param);

/*
**	function name:DH_SetTVMargin
**	describe: 调整TV输出的位置，通过调用这个函数可以手动的调整TV输出中的显示位置
**	parameter: [in] 解码通道句柄
**			   [in] 0, 0, 800, 600即代表满屏
					0, 0, 400, 300代表1/4屏，且在屏幕的左上角
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetTVMargin(HANDLE hChannel, int left, int top, int right, int bottom);


//Raw data play part
/*
**	function name:DH_SetRawPlayMode
**	describe: 设定YUV上墙模式
**	parameter: [in] 解码通道句柄
**			   [in] TRUE 表示进入YUV播放模式，FALSE表示退出该模式
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetRawPlayMode(HANDLE hChannel, BOOL bRaw);

/*
**	function name:DH_UpdateVideoFormat
**	describe: 更新YUV数据格式，每次数据有改变需要调用该函数来通知SDK做一些必要的变更
**	parameter: [in] 解码通道句柄
**			   [in] 图像宽度
**			   [in] 图像高度
**			   [in] 传输帧率
**			   [in] 视频格式 0---YUV420 1---YVU420		
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_UpdateVideoFormat(HANDLE hChannel, int nWidth, int nHeight,int nFrameRate,int videoFormat);

/*
**	function name:DH_InputRawData
**	describe: 送YUV数据至SDK的缓冲中，调用DH_Play之后即可显示上墙
**	parameter: [in] 解码通道句柄
**			   [in] YUV数据的缓冲区指针
**			   [in] YUV数据缓冲区大小	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InputRawData(HANDLE hChannel, BYTE* pBuf, int nSize);


//Video output mode
/*
**  function name:DH_GetVideoMode
**	describe: 获得解码卡的显示模式，VGA还是TV输出
**	parameter: [in] DSP的索引,从1开始，不能为0。
**			   [out] 显示模式类别 	
**	return: 错误号
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetVideoMode(UINT nDspNo, VideoOutMode *mode);

/*
**	function name:DH_SetVideoMode
**	describe: 设置解码卡的显示模式，VGA还是TV输出，其中默认是VGA优先。
**			同一显示通道只能有一种模式进行显示输出。
**	parameter:[in] DSP的索引号,从1开始计算，0表示对全部的DSP做操作
**			  [in] 输出显示模式	1--VGA 2--tv
**	return: 错误号		
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetVideoMode(UINT nDspNo, int mode);

#endif