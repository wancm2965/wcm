//
//FileName: dhvdecdecode.h
//describe: ��ͷ�ļ������������뿨SDK���ܵĽӿں���������ŵ���Ϣ��
//Date:		2007-11-14	create
//			2007-11-22	modify �����˶�pnx1700ϵ�н��뿨��֧��
//			2007-12-28	modify ����һ�����ڻ�ȡDSP��ʼ�����ȵĽӿں�������һ�麯�����ⲻ���ţ� 
//			2008-2-20	modify �������й�YUV����ֱ��������뿨��ʾ�Ĺ��ܺ���
//			2008-3-10	modify ������һ������ܽӿں�����δʵ�֣���
//						������һ���ļ����ſ��ƣ�����ʹ�õĽӿں�����δʵ�֣�
//

#ifndef _DHVDCDECODE_
#define _DHVDCDECODE_

#ifndef VDCDECODE_API
#define VDCDECODE_API  extern "C" __declspec(dllexport) 
#endif

#define DHVDC_CALLMETHOD __stdcall
#define CALLBACK __stdcall

#define DHERR_SUCCESS 0                          //�ɹ�
#define DHERR_ALLOCATE_MEMORY -1                 //�ڴ����ʧ��
#define DHERR_INVALID_HANDLE  -2                 //�����Ч
#define DHERR_DDRAW_CREATE_FAILED  -3            //DirectDraw����ʧ��
#define DHERR_DDRAW_CAPS_FAULT   -4              //DirectDraw��ȡ��ʾ�豸����ʧ��
#define DHERR_SET_COOPERATIVELEVEL_FAILED  -5    //DirectDraw����Э������ʧ��
#define DHERR_PRIMARY_SURFACE_CREATE_FAILED  -6  //DirectDraw����������ʧ��
#define DHERR_OVERLAY_CREATE_FAILED   -7         //DirectDraw����overlay����ʧ��
#define DHERR_GET_OVERLAY_ADDRESS_FAILED   -8    //DirectDraw��ȡOverlay��ַʧ��
#define DHERR_OVERLAY_UPDATE_FAILED   -9         //DirectDraw Update ʧ��
#define DHERR_SURFACE_NULL    -10                //DirectDraw ������NULL.
#define DHERR_FILEHEADER_UNKNOWN    -11          //�ļ�ͷ����
#define DHERR_CREATE_FILE_FAILED    -12          //�������򿪣��ļ�ʧ��
#define DHERR_FILE_SIZE_ZERO   -13               //�ļ�����̫��
#define DHERR_CREATE_OBJ_FAILED   -14            //�����̻߳��ں˶���ʧ��
#define DHERR_CHANNELMAGIC_MISMATCH   -15        //ͨ�����ݻٻ�
#define DHERR_PARA_OVER    -16                   //�����Ƿ�
#define DHERR_ORDER   -17                        //����˳�򲻶�
#define DHERR_COMMAND   -18                      //�����ʧ��
#define DHERR_UNSUPPORTED    -19                 //��֧��
#define DHERR_DSPOPEN   -20                      //DSP ��ʧ��
#define DHERR_DSPLOAD   -21                      //DSP ����ʧ��
#define DHERR_ALLOCATE_DSPMEMORY   -22           //DSP �ڴ����ʧ��
#define DHERR_DSPCHECHER   -23                   //DSP У��ʧ��
#define DHERR_IMGFILE_UNKNOWN   -24              //δ֪��IMG�ļ�
#define DHERR_INVALID_FILE    -25                //��Ч���ļ�
#define DHERR_INIT_FAILED	-26					 //��ʼ��DSPʧ��
#define DHERR_TIMEOUT		-27					 //�ȴ���ʱ 
#define DHERR_NOEXIST		-28					 //û�н��뿨����	

typedef struct tagDSPDetail
{
	UINT encodeChannelCount;			//�忨�����ı���ͨ������
	UINT firstEncodeChannelIndex;		//�忨�ϵ�һ������ͨ��������
	UINT decodeChannelCount;			//�忨�����Ľ���ͨ������
	UINT firstDecodeChannelIndex;		//�忨�ϵ�һ������ͨ��������
	UINT displayChannelCount;			//�忨��������Ƶ���ͨ������
	UINT firstDisplayChannelIndex;		//�忨�ϵ�һ����Ƶ���ͨ��������
	UINT reserved1;						/*reserve*/
	UINT reserved2;						/*reserve*/
	UINT reserved3;						/*reserve*/
	UINT reserved4;						/*reserve*/
}DSP_DETAIL, *pDSPDETAIL;

typedef struct
{
	int /*BOARD_TYPE_DS*/ type; //�忨����. 0--PNX1500; 1--PNX1700
	BYTE sn[16]; //���к�
	UINT dspCount; //�忨������DSP����
	UINT firstDspIndex; //�忨�ϵ�һ��DSP������DSP�е�����
	UINT encodeChannelCount; //�忨�����ı���ͨ������
	UINT firstEncodeChannelIndex; //�忨�ϵ�һ������ͨ�������б���ͨ���е�����
	UINT decodeChannelCount; //�忨�����Ľ���ͨ������
	UINT firstDecodeChannelIndex; //�忨�ϵ�һ������ͨ�������н���ͨ
	UINT displayChannelCount; //�忨��������Ƶ���ͨ������
	UINT firstDisplayChannelIndex ; //�忨�ϵ�һ����Ƶ���ͨ����������Ƶ���ͨ���е�����
	UINT reserved1;
	UINT reserved2;
	UINT reserved3;
	UINT reserved4;
}DS_BOARD_DETAIL;

typedef struct
{
	UINT left;// ������߽�
	UINT top; //�����ϱ߽�
	UINT width; //������
	UINT height; //����߶�
	COLORREF color; //���򱳾�ɫ
	UINT param; //������չ����
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
	�йؽ������Ļص���������Ϊ���뿨��ʼ��ʱ��ϳ��������Ǵ��ڶ�鿨�������
	����ʹ�����������������ʼ������Ϣ��nCurrent��ʾ��ǰ��ʼ��DSP��ţ�nTotal
	��ʾϵͳ���ܹ���DSP������dwUser���ṩ���û��Ĳ�����Ϣ����չ�á����ⲻ���š�
*/
typedef void (CALLBACK *fnDecProgressCB) (int nCurrent, int nTotal, DWORD dwUser);
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDECProgressCB(fnDecProgressCB fnCallback, DWORD dwUser);


//init part

/*
**	function name: DH_InitDecDevice
**	describe:	��ʼ�����뿨���ú�����ʹ�ý��뿨�ı�����������óɹ���
**		���ɽ���֮�������������
**	parameter: [out] �����ʼ���ɹ������ظõ�ַ�ϴ洢֧�ֵĽ���ͨ��������
**	return:	�����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InitDecDevice(long *pDeviceTotal);

/*
**	function name: DH_ReleaseDecDevice
**	describe: ����PC��Ϊ���뿨�������Դ���ڳ�������˳�ʱ���е��á�
**	parameter: NULL
**  return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ReleaseDecDevice();

/*
**	�й�������DirectDraw�ĺ������Ѿ��ڳ����ڲ������û��������
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InitDirectDraw(HWND hParent, COLORREF colorKey);
VDCDECODE_API int DHVDC_CALLMETHOD DH_ReleaseDirectDraw();

/*
**	function name: DH_ChannelOpen
**	describe: �ڳ�ʼ����ɺ󣬵��������������ȡ�����Ľ���ͨ�������
**	parameter: nChannelNum[in] ��Ҫ��ȡĳ������ͨ������ţ����㿪ʼ
**			  HANDLE *phChannel[out] ��Ӧ����Ľ���ͨ����ţ������Ӧ�ľ�� 	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ChannelOpen(long nChannelNum, HANDLE *phChannel);

/*
**	function name:DH_ChannelClose
**	describe: �˳�ʱ���øú����رս���ͨ����
**	parameter: [in] ����ͨ�����
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ChannelClose(HANDLE hChannel);

//Open Part
/*
**	function name:DH_OpenStream
**	describe: ʹ������ʽ���ݽ��н���֮ǰ�����øú������仺��������Դ
**	parameter: HANDLE hChennel[in] ����ͨ�����
**			   PBYTE pFileHeadBuf[in] �����ÿ�
**			   DWORD dwSize [in] �������Ĵ�С			
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_OpenStream(HANDLE hChennel, PBYTE pFileHeadBuf, DWORD dwSize);

/*
**	function name:DH_ResetStream
**	describe: ��ն�Ӧ���뻺�����е�����
**	parameter: [in] ����ͨ�����
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ResetStream(HANDLE hChannel);

/*
**	function name:DH_CloseStream
**	describe: �˳�ʱ���øú����رս���ͨ�������ݻ�������
**	parameter: [in] ����ͨ�����
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_CloseStream(HANDLE hChannel);

/*
**	function name:DH_InputData
**	describe: �ͻ�ȡ������������Ӧ�Ľ���ͨ���Ļ�����
**	parameter: HANDLE hChannel [in] ����ͨ�����
**			   PBYTE pBuf[in] ���ݵĻ�����ָ��
**			   DWORD dwSize[int] ���ݻ�������С		
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InputData(HANDLE hChannel, PBYTE pBuf, DWORD dwSize);

/*
**	function name:DH_OpenFile
**	describe: ʹ���ļ��طŷ�ʽ���н�����ǽ���������������ʽ������ǽ��Щ��ͬ��
**			ʹ�����ַ�ʽ��ǽ������Ҫ������DH_InputData�����ݣ�SDK�ڲ����Զ���һЩ����
**	parameter: HANDLE hChannel[in] ����ͨ�����
**			   LPTSTR szFileName[in] �ļ���·����ָ��	 
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_OpenFile(HANDLE hChannel, LPTSTR szFileName);

/*
**	function name:DH_CloseFile
**	describe: �˳�ʱ���øú����ر��ļ��ط�ģʽ
**	parameter: [in] ����ͨ�����
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_CloseFile(HANDLE hChennel);

//Play part
/*
**	function name:DH_SetDisplayPara
**	describe: �趨��ʾ��״̬
**	parameter: [in] ����ͨ�����
**			   [in] ������µĽṹ����Ϣ	
				�ṹ˵����
				typedef struct{
				long bToScreen;
				long bToVideoOut;
				long nLeft;
				long nTop;
				long nWidth;
				long nHeight;
				long nReserved;
				}DISPLAY_PARA,*PDISPLAY_PARA;
				bToScreen 1�������Ļ��0��֮��
				bToVideoOut 1�������VideoOut,0��֮��
				nleft, nTop, nWidth, nHeight �������Ļ�ϵķ�Χ�����hParent����3���ͻ������ꡣ
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayPara(HANDLE hChannel, DISPLAY_PARA *pPara);

/*
**	function name:DH_Play
**	describe: ��������ʽ�����ļ���ʽ�����øú����ɹ���DSP��ʼ���н��벢��ʾ��
**	parameter: HANDLE hChannel[in] ����ͨ�����
**			   HWND hwnd [in] �������PC����ʾ�Ĵ��ھ��	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_Play(HANDLE hChannel, HWND hwnd);

/*
**	function name:DH_Stop
**	describe: ֹͣ������ʾ
**	parameter: [in] ����ͨ�����
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_Stop(HANDLE hChannel);

/*
**	function name:DH_Pause
**	describe: ��ͣ\����������ʾ
**	parameter: HANDLE hChannel[in] ����ͨ�����
**			   ULONG bPause[in] 1Ϊ��ͣ���룬0Ϊ��������	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_Pause(HANDLE hChannel, ULONG bPause);

//sound part
/*
**	function name:DH_PlaySound
**	describe: ����Ƶ����
**	parameter: [in] ����ͨ�����
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_PlaySound(HANDLE hChannel);

/*
**	function name:DH_StopSound
**	describe: ֹͣ��Ƶ����
**	parameter: [in] ����ͨ�����
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_StopSound(HANDLE hChannel);

/*
**	function name:DH_SetVolume(not use)
**	describe: ����������С
**	parameter: [in] ����ͨ�����
**			   [in] ������С 0~0xffff��
**	return: �����
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
**	describe: ������Ƶ¼��
**	parameter: [in] ����ͨ�����
**			   [in]	¼���ļ���·����ָ�� 
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_StartCapFile(HANDLE hChannel,LPTSTR sFileName);

/*
**	function name:DH_StopCapFile
**	describe: ֹͣ��Ƶ¼��
**	parameter: [in] ����ͨ�����
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_StopCapFile(HANDLE hChannel);

//capture picture
/*
**	function name:DH_GetYV12Image
**	describe: ��ý���ͼ���YUV����
**	parameter: [in] ����ͨ�����
**			   [out] yuv���ݻ�����ָ��
**			   [out] yuv���ݻ�������С   		 
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetYV12Image(HANDLE hChannel, PBYTE pBuffer, ULONG nSize);

/*
**	function name:DH_GetPictureSize
**	describe: ��ȡ����ͼƬ��С
**	parameter: [in] ����ͨ�����
**			   [out] ͼƬ���
**			   [out] ͼƬ�߶�	 
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetPictureSize(HANDLE hChannel,ULONG* pWidth, ULONG* pHeight);

/*
**	function name:DH_ConvertToBmpFile
**	describe: ��yuv����ת����BMPͼƬ�������б��档
**	parameter: BYTE * pBuf [in] yuv���ݵĻ�����ָ��
**			   ULONG nSize [in] ������ָ���С
**			   ULONG nWidth [in] ͼƬ�Ŀ��
**			   ULONG nHeight [in] ͼƬ�ĸ߶�
**			   char *sFileName [in] ����ͼƬ���ļ�����
**			   ULONG nReserved ����					
**	return: �����
*/
VDCDECODE_API int 
DHVDC_CALLMETHOD DH_ConvertToBmpFile(BYTE * pBuf,ULONG nSize, ULONG nWidth,ULONG nHeight,
													   char *sFileName,ULONG nReserved);

//setting and getting  part
/*
**	function name:DH_Jump
**	describe: �ļ������е�ǰ�����ߺ��������øú�����
**			ͼ�������趨��ʱ����������ǰ�����������š�
**	parameter: [in] ����ͨ�����
**			   [in] ��ת�ķ���
**	return: �����
**	remark: ��û��ʵ��
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_Jump(HANDLE hChannel,ULONG nDirection);

/*
**	function name:DH_SetJumpInterval
**	describe: �趨���ŵ�ʱ����
**	parameter: [in] ����ͨ�����
**			   [in] ʱ����
**	return: �����
**	remark: ��û��ʵ��
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetJumpInterval(HANDLE hChannel,ULONG nSecond);

/*
**	function name:DH_GetSpeed
**	describe: ���ؽ�����ٶȴ�С
**	parameter: [in] ����ͨ�����
**			   [out] �����ٶ� (1, 2 ,3, 4, -1, -2, -3, -4)
**	return: �����
**	remark: ��û��ʵ��
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetSpeed(HANDLE hChannel,long *pSpeed);

/*
**	function name:DH_SetSpeed
**	describe: �趨������ٶȴ�С
**	parameter: [in] ����ͨ�����
**			   [in] �����ٶ�((1, 2 ,3, 4, -1, -2, -3, -4)) 
**	return: �����
**	remark: ��û��ʵ��
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetSpeed(HANDLE hChannel,long nSpeed);

/*
**	function name:DH_SetPlayPos
**	describe: �趨�ļ����ŵ�λ�ã��ٷֱȣ�
**	parameter: [in] ����ͨ�����
**			   [in] λ�ã�0��100�� 
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetPlayPos(HANDLE hChannel,ULONG nPos);

/*
**	function name:DH_SetPlayPos
**	describe: ����ļ����ŵ�λ�ã��ٷֱȣ�
**	parameter: [in] ����ͨ�����
**			   [out] λ�ã�0��100�� 
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetPlayPos(HANDLE hChannel,ULONG* pPos);

/*
**	function name:DH_GetVersion
**	describe: ��ð汾��Ϣ
**	parameter: [out] �汾��Ϣ����Ͻṹ�嶨��
**			   
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetVersion(pDHVDCVersion pVersion);

/*
**	function name:DH_GetCurrentFrameRate
**	describe: ��õ�ǰ�Ľ���֡��
**	parameter: [in] ����ͨ�����
**			   [out] ֡����Ϣ
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetCurrentFrameRate(HANDLE hChannel,ULONG* pFrameRate);

/*
**	function name:DH_GetCurrentFrameNum
**	describe: ��õ�ǰ�Ľ���֡���
**	parameter: [in] ����ͨ�����
**			   [out] ֡���
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetCurrentFrameNum(HANDLE hChannel,ULONG* pFrameNum);

/*
**	function name:DH_GetFileTotalFrames
**	describe: ��ý���֡����
**	parameter: [in] ����ͨ�����
**			   [out] ��֡��
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetFileTotalFrames(HANDLE hChannel,ULONG* pTotalFrames);

/*
**	function name:DH_GetFileTime
**	describe: ����ļ����ŵ�ʱ�䳤��
**	parameter: [in] ����ͨ�����
**			   [out] ʱ�䳤��
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetFileTime(HANDLE hChannel, ULONG* pFileTime);


/*
**	function name:DH_GetCurrentFrameTime
**	describe: ��õ�ǰ�ļ�����֡��ʱ��
**	parameter: [in] ����ͨ�����
**			   [out] ��ǰʱ��
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetCurrentFrameTime(HANDLE hChannel,ULONG* pFrameTime);

/*
**	function name:DH_GetPlayedFrames
**	describe: ����Ѿ������֡��
**	parameter: [in] ����ͨ�����
**			   [out] �Ѿ������֡��
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetPlayedFrames(HANDLE hChannel,ULONG *pDecVFrames);

/*
**	function name:DH_GetDeviceSerialNo
**	describe: ��ÿ������к�
**	parameter: [in] ����ͨ�����
**			   [out] �������к�
**	return: �����
**	remark: ��ʹ��GetBoardDetail
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetDeviceSerialNo(HANDLE hChannel,ULONG *pDeviceSerialNo);

/*
**	function name:DH_SetFileEndMsg
**	describe: �趨�ļ����Ž������ᷢһ����Ϣ��ָ�����ڣ�
**			���û��Ը���Ϣ���д���
**	parameter: [in] ����ͨ�����
**			   [in] ָ���Ĵ��ھ��
**			   [in] �Զ�����ϢID��	 
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetFileEndMsg(HANDLE hChannel,HWND hWnd,UINT nMsg);

/*
**	function name:DH_SetStreamOpenMode
**	describe: �趨�����ŵ�ģʽ����һ��ʵʱ�Ժ������Ե�ѡ��
**	parameter: [in] ����ͨ�����
**			   [in] 0��ʾ�����κ��趨��1��5ֵԽ����ʾԽ������������ʱԽ�� 
**	return: �����
**	remark: δʵ��
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetStreamOpenMode(HANDLE hChannel,ULONG nMode);

/*
**	function name:DH_GetStreamOpenMode
**	describe: ��ȡ�����ŵ�ģʽ
**	parameter: [in] ����ͨ�����
**			   [out] �趨��ģʽ��Сֵ 
**	return: �����
**	remark: δʵ��
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetStreamOpenMode(HANDLE hChannel,ULONG *pMode);

/*
**	function name:DH_SetVideoOutStandard
**	describe: �趨�����ʽ
**	parameter: [in] ����ͨ�����
**			   [in] PAL, NTSC���� 
**	return: �����
**	remark: δʵ��
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetVideoOutStandard(HANDLE hChannel,ULONG nStandard);

/*
**	function name:DH_SetDspDeadlockMsg
**	describe: ��DSP����Ӧʱ���򴰿ڷ����ض���Ϣ
**	parameter: [in] ���ھ��
**			   [in] �Զ�����ϢID 
**	return: �����
**	remark: δʵ��
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDspDeadlockMsg(HWND hWnd,UINT nMsg);

/*
**	function name:DH_GetChannelNum
**	describe: ��ö�ӦDSP��ͨ����
**	parameter: [in] DSP�����
**			   [out] ���ص�ͨ����
**			   [in] Ҫ�õ���ͨ����
**			   [out]ʵ�ʻ��ͨ���Ÿ���		
**	return: �����
**	remark: ��ʹ��GetBoardDetail����
*/
VDCDECODE_API int DHVDC_CALLMETHOD 
DH_GetChannelNum(long nDspNum,long *pChannelNum,ULONG nNumsToGet,ULONG * pNumsGotten);


/*
**	function name:DH_ResetDsp
**	describe: ���øú�������������DSP,�ڲ��Զ�������ǰ��״̬
**	parameter: [in] DSP���
**	return: �����
**	remark: δʵ��
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ResetDsp(long nDspNum);

/*
**	function name:DH_SetAudioPreview
**	describe: �������߹ر���ƵԤ����PC�˵��������ţ�
**	parameter: [in] ����ͨ�����
**			   [in] TRUE:�� FALSE:�ر�
**	return: �����
**	remark: 
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetAudioPreview(HANDLE hChannel, BOOL bEnable);

/*
**	function name:DH_GetDspCount
**	describe: ���øú�������ȡ��DSP����������Ҫ��ʼ��Ҳ�ɵ���
**	parameter: NULL
**	return: DSP����
*/
VDCDECODE_API unsigned int DHVDC_CALLMETHOD DH_GetDspCount();

/*
**	function name:DH_GetDspDetail
**	describe: ��ȡDSP��ϸ��Ϣ
**	parameter: [in] DSP���
**			   [out] DSP��ϸ����Ϣ������ýṹ�嶨��
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetDspDetail(UINT dspNum,DSP_DETAIL *pDspDetail);

/*
**	function name:DH_GetBoardCount
**	describe: ��ȡ�忨��ϸ��Ϣ
**	parameter: [in] �忨��ţ�����1...��,����Ϊ0
**			   [out] �忨��ϸ����Ϣ������ýṹ�嶨��
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetBoardDetail(UINT boardNum, DS_BOARD_DETAIL *pBoardDetail);

/*
**	function name:DH_OpenStreamEx
**	describe: ������Ƶ�ֿ��ķ�ʽ����
**	parameter: [in] ����ͨ�����
**			   [in] �ļ�����ͷָ��
**			   [in] �ļ�ͷ����	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_OpenStreamEx(HANDLE hChannel, PBYTE pFileHeadBuf, DWORD nSize);

/*
**	function name:DH_OpenStreamEx
**	describe: ������Ƶ�ֿ��ķ�ʽ�ر���
**	parameter: [in] ����ͨ�����
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_CloseStreamEx(HANDLE hChannel);

/*
**	function name:DH_InputVideoData
**	describe: ���봿��Ƶ���ݣ��ڴ���֮��ſ���ʹ��
**	parameter: [in] ����ͨ�����
**			   [in] ���ݻ���ָ��
**			   [in] ���ݻ�������С		
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InputVideoData(HANDLE hChannel, PBYTE pBuf, DWORD dwSize);

/*
**	function name:DH_InputAudioData
**	describe: ���봿��Ƶ���ݣ��ڴ���֮��ſ���ʹ��
**	parameter: [in] ����ͨ�����
**			   [in] ���ݻ���ָ��
**			   [in] ���ݻ�������С		
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InputAudioData(HANDLE hChannel,PBYTE pBuf,DWORD nSize);

/*
**	function name:DH_GetBoardCount
**	describe: ���ϵͳ�ϵİ忨����
**	parameter:  NULL
**	return: �忨����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetBoardCount();


/*
**	function name:DH_SetDefaultVideoStandard
**	describe: ������ϵͳĬ�ϵ���Ƶ��ʽ��Ĭ��ΪPAL��
**				ϵͳ��������Ƶ����ͨ�����������Ƶ���룬
**				���ͨ���ᰴ��Ĭ�ϵ���ʽ�������е���Ƶ���ͨ������ʽ��
**				��ϵͳ����ʱҲ�ᰴ��Ĭ�ϵ���ʽ����
**	parameter:  [in]��Ƶ��ʽ
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDefaultVideoStandard(VideoStandard_t VideoStandard);

/*
**	function name:DH_GetDisplayChannelCount
**	describe: ���ϵͳ�ϵ���ʾͨ��������
**	parameter:  NULL
**	return: ��ʾͨ������
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetDisplayChannelCount();

/*
**	function name:DH_SetDisplayStandard
**	describe: ������ʾͨ������Ƶ��ʽ
**	parameter:  [in] ��Ƶ��ʾͨ������
**				[in] ��Ƶ��ʽ
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayStandard(UINT nDisplayChannel,VideoStandard_t VideoStandard);

/*
**	function name:DH_SetDisplayRegion
**	describe: ������ʾͨ������Ƶ��ʽ
**	parameter:  [in] ��Ƶ��ʾͨ������(����汾�иò����ɽ�����ֱ��ǿ��ת������)
**				[in] �������
**				[in] �������
**				[in] ����
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayRegion(UINT nDisplayChannel,UINT nRegionCount,
														REGION_PARAM *pParam,UINT nReserved);

/*
**	function name:DH_ClearDisplayRegion
**	describe: ����ʾ������գ���ʾSetDisplayRegion�������õı���ɫ��Bit0��Bit15����Ӧ����1��16����ӦλΪ1���򽫸�������ա�
**	parameter: [in] ��ʾͨ������
**			   [in] Ҫ���������
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ClearDisplayRegion(UINT nDisplayChannel,UINT nRegionFlag);

/*
**	function name:DH_SetDisplayRegionPosition
**	describe: �ı�ĳ����ʾ�����λ��
**	parameter: [in] ��ʾͨ������
**			   [in] ԭ�����н�Ҫ���������
**			   [in] UINT nLeft, UINT nTop�������λ��	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayRegionPosition(UINT nDisplayChannel,
															   UINT nRegion, UINT nLeft, UINT nTop);

/*
**	function name:DH_FillDisplayRegion
**	describe: ���Զ����ͼ�������ʾ����pImage��ָ��ͼ��Ĵ�С�����SetDisplayRegion�����õ�ͼ���С��ͬ������ͼ���޷�������ʾ��
**	parameter: [in] ��ʾͨ������
**			   [in] Ҫ���������
**			   [in] YUV420��ʽ��ͼ��ָ��	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_FillDisplayRegion(UINT nDisplayChannel,UINT nRegion, unsigned char *pImage);

/*
**	function name:DH_SetDecoderAudioOutput
**	describe: ���ý���ͨ������Ƶ���������nDecodeChannel·������Ƶ���������MD���ϵĵ�nOutputChannel����Ƶ���ͨ����
**			ÿ��DS-4002MD��DS-4004MD����ÿһ��DSP����2·��Ƶ�������Ӧ��ÿһ��DSP��nOutputChannel����Ϊ0��1��
**			����Ѿ��������Ľ���ͨ������Ƶ��nOutputChannel�������ϵͳ���Զ��Ƚ���ֹͣ��
**	parameter: [in] ����ͨ������
**			   [in] �������ر�
**			   [in] Ҫ�������ʾͨ������	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDecoderAudioOutput(UINT nDecodeChannel,BOOL bOpen,UINT nOutputChannel);

/*
**	function name:DH_SetDecoderVideoOutput
**	describe: ���ý���ͨ������Ƶ���������nDecodeChannel·������Ƶ���������MD���ϵĵ�nOutputChannel����Ƶ���ͨ����
**	parameter: [in] ����ͨ������
**			   [in] ����ͨ��������˿�	
**			   [in] �������ر�
**			   [in] ��ʾͨ��������	
**			   [in] Ҫ�������ʾ����
**			   [in]	����	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDecoderVideoOutput(UINT nDecodeChannel,UINT nPort,BOOL bOpen,
															UINT nDisplayChannel,UINT nDisplayRegion,UINT nReserved);
/*
**	function name:DH_SetDecoderVideoExtOutput
**	describe: ���ý���ͨ������Ƶ�ⲿ��������������������Ƶͼ��ӵ�nDecodeChannel������ͨ���ĵ�nPort·��
**		�͵���nDisplayChannel(ÿ��DS-4002MD����2����ʾͨ����ÿ��DS-4004MD����4����ʾͨ�������nDisplayChannel��0��1��2��3һֱ������.)
**		����ʾͨ���ĵ�nDisplayRegion�����򡣴˺�����nPort����Ϊ0��1��ÿ������ͨ��֧��2·��Ƶ�������
**		���bOpenΪ0����nDisplayChannel��nDisplayRegion�����塣�˺���ʵ�ֽ���ͨ���Ŀ�MD���������
**	parameter: [in] ����ͨ������
**			   [in] ����ͨ��������˿�	
**			   [in] �������ر�
**			   [in] ��ʾͨ��������	
**			   [in] Ҫ�������ʾ����
**			   [in]	����	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDecoderVideoExtOutput(UINT nDecodeChannel,UINT nPort,BOOL bOpen,
															   UINT nDisplayChannel,UINT nDisplayRegion,UINT nReserved);
/*
**	function name:DH_SetFileRef
**	describe: �趨�ļ�����
**	parameter: [in] ����ͨ�����
**			   [in] �������ر�
**			   [in] ����������ɺ�ص�����
**	return: �����
*/
typedef void (*FILE_REF_DONE_CALLBACK)(UINT nChannel,UINT nSize);
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetFileRef(HANDLE hChannel,BOOL bEnable,FILE_REF_DONE_CALLBACK FileRefDoneCallback);

/*
**	function name:DH_GetFileAbsoluteTime
**	describe: ���õ��ļ�����ֹʱ�䣨����ʱ�䣩��SYSTEMTIME�к��������Ч��ʼ��Ϊ��
**	parameter: [in] ����ͨ�����
**			   [in] �ļ���ʼʱ��(���������Ч��ʼ��Ϊ��)
**			   [in] �ļ�����ʱ��(���������Ч��ʼ��Ϊ��)
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetFileAbsoluteTime( HANDLE hChannel, SYSTEMTIME *pStartTime,SYSTEMTIME *pEndTime);

/*
**	function name:DH_GetFileAbsoluteTime
**	describe: ���õ��ļ���ǰ�ľ���ʱ�䣬(���������Ч��ʼ��Ϊ��)
**	parameter: [in] ����ͨ�����
**			   [in] �ļ���ǰ�ľ���ʱ��(���������Ч��ʼ��Ϊ��)
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetCurrentAbsoluteTime(HANDLE hChannel,SYSTEMTIME *pTime);

/*
**	function name:DH_LocateByAbsoluteTime
**	describe: �������վ���ʱ�䶨λ�ļ���ֻ�ڻط��ļ����Ҵ���������Ч��SYSTEMTIME�к��������Ч��
**				�ļ�����ֹʱ����DH_GetFileAbsoluteTime������á�
**	parameter: [in] ����ͨ�����
**			   [in] ��λ�ľ���ʱ��(���������Ч��ʼ��Ϊ��)
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_LocateByAbsoluteTime(HANDLE hChannel,SYSTEMTIME time);

/*
**	function name:DH_LocateByFrameNumber
**	describe: ������֡�Ŷ�λ�ļ���ֻ�ڻط��ļ����Ҵ���������Ч���ļ�������֡������DH_GetFileTotalFrames������á�
**	parameter: [in] ����ͨ�����
**			   [in] ��λ��֡���
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_LocateByFrameNumber(HANDLE hChannel,UINT frmNum);

/*
**	function name:DH_ImportFileRef
**	describe: ���ļ��������롣Ҫʹ�ô˹��ܣ���������DH_SetFileRef�йص��ļ��������ܣ�Ȼ����DH_OpenFile֮���ٵ���������
**	parameter: [in] ����ͨ�����
**			   [in] �������ݵ��뻺��
**			   [in] �����С	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ImportFileRef(HANDLE hChannel,char *pBuffer,UINT nSize);

/*
**	function name:DH_ExportFileRef
**	describe: ���ļ�����������
**	parameter: [in] ����ͨ�����
**			   [in] �������ݵ��뻺��
**			   [in] �����С������С���������ݵĳ��ȣ��������ȿ��������������Ļص������л��	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_ExportFileRef(HANDLE hChannel,char *pBuffer,UINT nSize);

/*
**	function name:DH_LocateByFrameNumber
**	describe: ��ģ����Ƶ�������Ƶ����
**	parameter: [in] ģ����Ƶ�����ͨ����
**			   [in] TRUE/��������FALSE/ֹͣ����
**			   [in] ֡��
**			   [in] ͼ��Ŀ��(��ʱ֧��4CIF���704����֧������)
**			   [in] ͼ��ĸ߶�(��ʱ֧��4CIF�߶�PAL��576��NTSC��480����֧������)
**			   [in] ���������ݵĵ�ַ			
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayVideoCapture(UINT nDisplayChannel,BOOL bStart,UINT fps,
															 UINT width,UINT height,unsigned char *imageBuffer);

/*
**	function name:RegisterDisplayVideoCaptureCallback
**	describe: ע���ȡģ����Ƶ����Ļص�������
**	parameter: [in] IMAGE_STREAM_CALLBACK �ص�����
**			   [in] nDisplayChannel ģ����Ƶ�����ͨ����
**			   [in] void *context ���ûص�����ʱ�ṩ��������
**	return: �����
*/
typedef void (CALLBACK *IMAGE_STREAM_CALLBACK)(UINT nDisplayChannel,void *context);
VDCDECODE_API int DHVDC_CALLMETHOD DH_RegisterDisplayVideoCaptureCallback(IMAGE_STREAM_CALLBACK DisplayVideoCaptureCallback, void *context);

/*
**	function name:DH_SetDisplayVideoBrightness
**	describe: ������ģ����Ƶ��������ȡ�
**	parameter: [in] ģ����Ƶ�����ͨ����
**			   [in] ����ֵ��0��255��
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDisplayVideoBrightness(UINT chan,int Brightness);

/*
**	function name:DH_SetDecoderPostProcess
**	describe: �����ý������
**	parameter: [in] ����ͨ�����
**			   [in] bit 0��1������ȥ��˸���ܣ�0����
**				����λ������
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetDecoderPostProcess(HANDLE hChannel,UINT param);

/*
**	function name:DH_SetTVMargin
**	describe: ����TV�����λ�ã�ͨ������������������ֶ��ĵ���TV����е���ʾλ��
**	parameter: [in] ����ͨ�����
**			   [in] 0, 0, 800, 600����������
					0, 0, 400, 300����1/4����������Ļ�����Ͻ�
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetTVMargin(HANDLE hChannel, int left, int top, int right, int bottom);


//Raw data play part
/*
**	function name:DH_SetRawPlayMode
**	describe: �趨YUV��ǽģʽ
**	parameter: [in] ����ͨ�����
**			   [in] TRUE ��ʾ����YUV����ģʽ��FALSE��ʾ�˳���ģʽ
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetRawPlayMode(HANDLE hChannel, BOOL bRaw);

/*
**	function name:DH_UpdateVideoFormat
**	describe: ����YUV���ݸ�ʽ��ÿ�������иı���Ҫ���øú�����֪ͨSDK��һЩ��Ҫ�ı��
**	parameter: [in] ����ͨ�����
**			   [in] ͼ����
**			   [in] ͼ��߶�
**			   [in] ����֡��
**			   [in] ��Ƶ��ʽ 0---YUV420 1---YVU420		
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_UpdateVideoFormat(HANDLE hChannel, int nWidth, int nHeight,int nFrameRate,int videoFormat);

/*
**	function name:DH_InputRawData
**	describe: ��YUV������SDK�Ļ����У�����DH_Play֮�󼴿���ʾ��ǽ
**	parameter: [in] ����ͨ�����
**			   [in] YUV���ݵĻ�����ָ��
**			   [in] YUV���ݻ�������С	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_InputRawData(HANDLE hChannel, BYTE* pBuf, int nSize);


//Video output mode
/*
**  function name:DH_GetVideoMode
**	describe: ��ý��뿨����ʾģʽ��VGA����TV���
**	parameter: [in] DSP������,��1��ʼ������Ϊ0��
**			   [out] ��ʾģʽ��� 	
**	return: �����
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_GetVideoMode(UINT nDspNo, VideoOutMode *mode);

/*
**	function name:DH_SetVideoMode
**	describe: ���ý��뿨����ʾģʽ��VGA����TV���������Ĭ����VGA���ȡ�
**			ͬһ��ʾͨ��ֻ����һ��ģʽ������ʾ�����
**	parameter:[in] DSP��������,��1��ʼ���㣬0��ʾ��ȫ����DSP������
**			  [in] �����ʾģʽ	1--VGA 2--tv
**	return: �����		
*/
VDCDECODE_API int DHVDC_CALLMETHOD DH_SetVideoMode(UINT nDspNo, int mode);

#endif