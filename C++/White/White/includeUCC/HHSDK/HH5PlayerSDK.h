 /******************************************************************************
* ϵͳ���ƣ�HH5PlayerSDK
* �ļ����ƣ�HH5PlayerSDK.h
* �汾    ��V5.5.0.1
* ˵��	  ����ģ���ṩ����Ƶ�Ľ�����ʾ�Ͳ���

* ��    ��: 2005��4��15��
  �������:	
* ����˵��: ��
******************************************************************************/
#ifndef HH5PLAYERSDK_H
#define HH5PLAYERSDK_H

#define DLLEXPORT_API  extern "C"__declspec(dllexport) 

#include <mmsystem.h>
#include "HHAVDefine.h"

/*****************************************************************************/
/*********************************  һ���궨��  ******************************/
/*****************************************************************************/
#define HH5PLAYER_MAX_PLAER		128											//���֧��128·����<nPort = 0 ~ 127>

#define HH5PLAYER_MSG_PLAY_END	0xFFFFFFFF									//��Ϣ����<�ļ����Ž���>
																			//��0xFFFFFFFF: ���Ž���: 0 ~ �ļ�ʱ�䳤�ȣ��룩
typedef	HHAV_INFO		HH5KAV_INFO;
typedef	PHHAV_INFO		PHH5KAV_INFO;

/*****************************************************************************/
/********************************  ���������ӿڶ���  *************************/
/*****************************************************************************/

//------------------------------  1����������ʼ��,�ͷ�  ----------------------------------//
//��ʼ��SDK��Դ
DLLEXPORT_API int __stdcall HH5PLAYER_InitSDK(HWND hHwnd);		
//�ͷ�  SDK��Դ			
DLLEXPORT_API int __stdcall HH5PLAYER_ReleaseSDK();				
//��ʼ��������(�μ�HH5PLAYER_InitPlayer2)			
DLLEXPORT_API int __stdcall HH5PLAYER_InitPlayer(USHORT nPort,HWND hWnd);
//�ͷŲ�����	
DLLEXPORT_API int __stdcall HH5PLAYER_ReleasePlayer(USHORT nPort);			

//------------------------------  2���ļ�����  ----------------------------------//
//���ļ�
DLLEXPORT_API int __stdcall HH5PLAYER_OpenStreamFileM(USHORT nPort,LPCTSTR filelist[], int filenum, DWORD &nTimeLength);
//�õ��ļ���Ϣ
DLLEXPORT_API int __stdcall HH5PLAYER_GetStreamFileInfo(USHORT nPort, DWORD *dwTimeLength, DWORD *dwFileLength, DWORD *dwWidth,DWORD *dwHeight);  
//����
DLLEXPORT_API int __stdcall HH5PLAYER_Play(USHORT nPort);                 
//���ٲ���  dwValue 1---1000֮�����     
DLLEXPORT_API int __stdcall HH5PLAYER_FastPlay(USHORT nPort,DWORD nValue);    
//���� 
DLLEXPORT_API int __stdcall HH5PLAYER_FastPlayBack(USHORT nPort,DWORD nValue); 
//��֡��
DLLEXPORT_API int __stdcall HH5PLAYER_FrameGO(USHORT nPort);          
//��֡��        
DLLEXPORT_API int __stdcall HH5PLAYER_FrameBack(USHORT nPort);             
//��ͣ      
DLLEXPORT_API int __stdcall HH5PLAYER_Pause(USHORT nPort);                     
//����
DLLEXPORT_API int __stdcall HH5PLAYER_Resume(USHORT nPort);              
//ֹͣ      
DLLEXPORT_API int __stdcall HH5PLAYER_Stop(USHORT nPort);                      
//�õ����ŵ�ʱ��
DLLEXPORT_API int __stdcall HH5PLAYER_GetPlayPosition(USHORT nPort,DWORD *dwPlayedTime);			
//���ò���λ��
DLLEXPORT_API int __stdcall HH5PLAYER_SetPlayPosition(USHORT nPort,float fOffset);				
//�ƶ�����ָ�뵽ָ����	
DLLEXPORT_API int __stdcall HH5PLAYER_SeekToSecond(USHORT nPort,int nSec);							
//ѭ������ 
DLLEXPORT_API int __stdcall HH5PLAYER_SetPlayLoop(USHORT nPort,bool  bIsLoop);						
//ע�Ქ��״̬��Ϣ(1.H5PLAYER_MSG_PLAY_END:�ļ����Ž���	2.���Ž���:0 ~ �ļ�ʱ�䳤��<��)
DLLEXPORT_API int __stdcall HH5PLAYER_RegPlayStatusMsg(USHORT nPort, HWND hWnd, UINT MessageID);	


//------------------------------  3�������� ----------------------------------//
//�򿪲�����
DLLEXPORT_API int __stdcall HH5PLAYER_OpenStream(USHORT nPort);
//���벥����Ƶ
DLLEXPORT_API int __stdcall HH5PLAYER_PutDecStreamData(USHORT nPort,PBYTE pBuf, DWORD nSize, int nDataType);
//���벥����Ƶ��չ
DLLEXPORT_API int __stdcall HH5PLAYER_PutDecStreamDataEx(USHORT nPort,PBYTE pBuf, DWORD nSize, UINT nDataType, HH5KAV_INFO *pAVInfo);	
//ֹͣ������Ƶ
DLLEXPORT_API int __stdcall HH5PLAYER_StopStream(USHORT nPort);
//�رղ�����Ƶ
DLLEXPORT_API int __stdcall HH5PLAYER_CloseStream(USHORT nPort);
//ȡ����Ƶ֡��С
DLLEXPORT_API int __stdcall HH5PLAYER_GetStreamFrameSize(USHORT nPort);
//ȡ����Ƶ֡��С
DLLEXPORT_API int __stdcall HH5PLAYER_GetAudioFrameSize(USHORT nPort);
//���ò����ٶ�
DLLEXPORT_API int __stdcall HH5PLAYER_SetStreamPlaySpeed(USHORT nPort, USHORT nSpeed);
//ȡ�ò����ٶ�
DLLEXPORT_API int __stdcall HH5PLAYER_GetStreamPlaySpeed(USHORT nPort);
//���ò����ٶ�2, fValue:ÿ֡����ʱ��, ��: 40ms, 33.3ms, bDelayAdjust:�Ƿ����ж�֡ʱ, У׼ʱ��(ע:���ǶԹؼ�֡����,һ��Ҫ��Ϊfalse)
DLLEXPORT_API int __stdcall HH5PLAYER_SetStreamPlaySpeed2(USHORT nPort,float fValue,bool bDelayAdjust = true);

//------------------------------  4���������Ų��� ----------------------------------//
//������Ƶ
DLLEXPORT_API int __stdcall HH5PLAYER_SetAudio(USHORT nPort,bool bEnabled);       
//ˢ��ͼ��  
DLLEXPORT_API int __stdcall HH5PLAYER_UpDateImage(USHORT nPort);					
//ˢ����ʾ���ڱ߽�
DLLEXPORT_API int __stdcall HH5PLAYER_UpdateBounds(USHORT nPort);					
//������ʾ����  
DLLEXPORT_API int __stdcall HH5PLAYER_SetDisplayhWnd(USHORT nPort,HWND hWnd);		
//�ֲ��Ŵ���ʾ
DLLEXPORT_API int __stdcall HH5PLAYER_SetPartDisplay(USHORT nPort,HWND hPartWnd,RECT *pPartRect);	
//��׽��ǰ��ͼ��
DLLEXPORT_API int __stdcall HH5PLAYER_CaptureOnePicture(USHORT nPort,void **bmpbuf,int &bmpsize);	
//��ȡ����
DLLEXPORT_API int __stdcall HH5PLAYER_GetVolume(USHORT nPort, long *lpVolume);		
//��������
DLLEXPORT_API int __stdcall HH5PLAYER_SetVolume(USHORT nPort, long lVolume);		
//��ʾ�������� 0: ������  1: ������
DLLEXPORT_API int __stdcall HH5PLAYER_SetDecoderQulity(bool bQulity);				
//��ʾ����	bZoomIn:������ʾ,bDeInterlace:�����о��,bPlayMode  TRUE  ��������	FALSE ʵʱ����
DLLEXPORT_API int __stdcall HH5PLAYER_SetDecoderParam(bool bZoomIn, bool bDeInterlace, bool bPlayMode = FALSE);		
//������������ʱ�������֡��(1 ~ 60)
DLLEXPORT_API int __stdcall HH5PLAYER_SetVideoBufferSize(int nFrameNum = 15);		
//������л���
DLLEXPORT_API int __stdcall HH5PLAYER_ClearAllBuffer(USHORT nPort);										
//�����Ƶ����
DLLEXPORT_API int __stdcall HH5PLAYER_ClearVideoBuffer(USHORT nPort);	
//�����Ƶ����	
DLLEXPORT_API int __stdcall HH5PLAYER_ClearAudioBuffer(USHORT nPort);		
//���DirctX����
DLLEXPORT_API int __stdcall HH5PLAYER_ClearImage(USHORT nPort, DWORD dwColor = 0);	


//------------------------------  5����Ƶ����� ----------------------------------//
//��Ƶ����
DLLEXPORT_API int   __stdcall HH5PLAYER_AudioEncodeData(unsigned short *BuffIn,   int InNum, BYTE *BuffOut);
//��Ƶ����
DLLEXPORT_API int   __stdcall HH5PLAYER_AudioDecodeData(BYTE *BuffIn,  int InNum, unsigned short  *BuffOut);

//G.726��Ƶ���� Ĭ��G.726 16kbps
DLLEXPORT_API int   __stdcall HH5PLAYER_G726AEncodeData(unsigned short *BuffIn,   int InNum, BYTE *BuffOut);
//G.726��Ƶ����
DLLEXPORT_API int   __stdcall HH5PLAYER_G726ADecodeData(BYTE *BuffIn,  int InNum, unsigned short  *BuffOut);

#define		HH5PLAYER_G722AEncodeData	HH5PLAYER_AudioEncodeData
#define		HH5PLAYER_G722ADecodeData	HH5PLAYER_AudioDecodeData

/*****************************************************************************/
/********************************  �������������ӿ�  *************************/
/*****************************************************************************/

//------------------------------  1����������1: ��Ƶ����� ----------------------------------//
//��HH98ϵ�к�HH58ϵ���豸�ṩ�Ľӿڣ����Ա����G.711 G.726 ADPCM��ʽ

//��Ƶ������չ
DLLEXPORT_API int   __stdcall HH5PLAYER_AudioEncodeDataEx(unsigned short *BuffIn,   int InNum, BYTE *BuffOut, UINT nEncodeType, UINT nBitrate);
//��Ƶ������չ
DLLEXPORT_API int   __stdcall HH5PLAYER_AudioDecodeDataEx(BYTE *BuffIn,  int InNum, unsigned short  *BuffOut, UINT nEncodeType, UINT nBitrate);

//------------------------------  2����������2: ��Ƶ���뼰��Ƶ���� ----------------------------------//
//ѡ���µ�H.264���� nVersion 1:old(123),2:new(125)
DLLEXPORT_API int   __stdcall HH5PLAYER_SelectH264(USHORT nPort,UINT nVersion);		
//���Ѻ۹��ܿ��� false: ��  true: ��		
DLLEXPORT_API int   __stdcall HH5PLAYER_WaitForVerticalBlank(bool bWait = false);			

//ע: ��Ҫ������Ƶͼ���ϵ���ͼƬ�����ߵ�ʱ���������º�����ʼ��������������bSupportDraw��Ϊtrue(�ڲ��Ŵ�����֧���ⲿ���ڲ�HDC)
//��ʼ������2
DLLEXPORT_API int   __stdcall HH5PLAYER_InitPlayer2(USHORT nPort,HWND hWnd,bool bSupportDraw = false);
//���Ż�ͼ
DLLEXPORT_API int   __stdcall HH5PLAYER_DrawImage( USHORT nPort,int x,int y,unsigned char *pBmpImage,int nSize);
//�����ı�OSD
DLLEXPORT_API int   __stdcall HH5PLAYER_DrawText(  USHORT nPort,int x,int y,char *pszText,LOGFONT ft,COLORREF crText,COLORREF crBack = -1/*crBack -1: ͸��*/);
//���û���
DLLEXPORT_API int   __stdcall HH5PLAYER_SetDrawPen(USHORT nPort,int nPenStyle,int nWidth, COLORREF crColor);
//����
DLLEXPORT_API int   __stdcall HH5PLAYER_DrawLine(  USHORT nPort,int x1,int y1,int x2,int y2);
//����
DLLEXPORT_API int   __stdcall HH5PLAYER_DrawRect(  USHORT nPort,int x1,int y1,int x2,int y2);
//�����ʾ
DLLEXPORT_API int   __stdcall HH5PLAYER_ClearDraw( USHORT nPort,int nType);					//nType: 0 all, 1 image, 2 text, 3 line, rect

typedef int (WINAPI *HHCBOnDraw)(USHORT nPort, HDC hdc, int nWidth, int nHeight, void *pContext);	//֧���ⲿ����HDC
DLLEXPORT_API int   __stdcall HH5PLAYER_RegCBOnDraw(USHORT nPort, HHCBOnDraw pCBOnDraw, void *pContext = NULL);

//------------------------------  3����������2: �Խ� ----------------------------------//
//typedef int (WINAPI *HHTalkCaptureData)(BYTE *pBuffer, int nBufLen, DWORD dwContext = 0, void *pContext = NULL);
typedef int (WINAPI *HHTalkCaptureData)(BYTE *pBuffer, int nBufLen, DWORD dwContext, void *pContext);
//��ʼ���Խ�
DLLEXPORT_API int __stdcall HH5PLAYER_TKInit(  HWND hWnd, HANDLE &hTalk);
DLLEXPORT_API int __stdcall HH5PLAYER_TKRegCaptureDataCB(HANDLE hTalk,HHTalkCaptureData pCBTalk,DWORD dwContext = 0,void *pContext = NULL);

//��ʼ�Խ�
DLLEXPORT_API int __stdcall HH5PLAYER_TKStart( HANDLE hTalk,WAVEFORMATEX *pInFormat, WAVEFORMATEX *pOutFormat);  
//�����Խ�
DLLEXPORT_API int __stdcall HH5PLAYER_TKStop(  HANDLE hTalk); 
//���ͱ��ضԽ�����
DLLEXPORT_API int __stdcall HH5PLAYER_TKSendToPCData(HANDLE hTalk, BYTE *pBuffer,int nBufLen);
//���öԽ���Ƶ
DLLEXPORT_API int __stdcall HH5PLAYER_TKSetVolume(   HANDLE hTalk, long lVolume);	
//ȡ�öԽ���Ƶ
DLLEXPORT_API int __stdcall HH5PLAYER_TKGetVolume(   HANDLE hTalk, long *lpVolume);
//�Խ��ͷ�
DLLEXPORT_API int __stdcall HH5PLAYER_TKRelease(HANDLE hTalk);

/*****************************************************************************/
/*******************************  �ġ�����֧�ֵĺ���  ************************/
/*****************************************************************************/
//================================================================
//���°��У�����֧�ֵĺ��� 
//           1. ���Կ���֧�����ڲ�����
//           2. ����Ƶͼ���ϵ���ͼƬ�����ߵȣ�����ͨ����������ʵ��
//================================================================
DLLEXPORT_API int __stdcall HH5PLAYER_SetDisplayGUID(char *pGuid);				
DLLEXPORT_API int __stdcall HH5PLAYER_SetDisplayGUIDEx(USHORT nPort, char *pGuid);
DLLEXPORT_API int __stdcall HH5PLAYER_SetDisplayWndEx(USHORT nPort,  unsigned int nFirstScreenWidth);

typedef int (WINAPI *YUVDataCallBack)(USHORT nPort,	
							     unsigned char *YBuf,
							     unsigned char *UBuf,
							     unsigned char *VBuf,
								 unsigned int  nYStride,
								 unsigned int  nUVStride,
								 unsigned int  nWidth,
								 unsigned int  nHeight,
							     unsigned int  nViFormat 	
								 );

DLLEXPORT_API int __stdcall HH5PLAYER_RegYUVDataCallBack(YUVDataCallBack pYUVDataCallBack, bool nDispImage);

typedef int (WINAPI *DrawImageCallBack)(USHORT nPort,	
							     HDC hdc,
							     unsigned char *pDispBuf,
								 unsigned int  nStride,
								 unsigned int  nDDrawMode
								 );
DLLEXPORT_API int __stdcall HH5PLAYER_RegDrawImageCallBack(DrawImageCallBack pDrawImageCallBack);


#endif  
