#include "stdAfx.h"

#define CPLAYER_API  extern "C"__declspec(dllimport)

#define FILE_HEAD 0 //�ļ�ͷ
#define VIDEO_I_FRAME 1 //��ƵI֡
#define VIDEO_B_FRAME 2 //��ƵB֡
#define VIDEO_P_FRAME 3 //��ƵP֡
#define VIDEO_BP_FRAME 4 //��ƵBP֡
#define VIDEO_BBP_FRAME 5 //��ƵB֡B֡P֡
#define AUDIO_PACKET 10 //��Ƶ��

typedef struct{
	int nPacketType;//������
	char *pPacketBuffer;//��������ַ
	DWORD dwPacketSize; //���Ĵ�С
	//����ʱ��
	int nYear;//ʱ��:��		
	int nMonth;//ʱ��:��
	int nDay;//ʱ��:��
	int nHour;//ʱ��:ʱ
	int nMinute;//ʱ��:��
	int nSecond;//ʱ��:��
	// ���ʱ��
	DWORD dwTimeStamp;	// ���ʱ��
}PACKET_INFO;

CPLAYER_API BOOL __stdcall AnalyzeDataOpenStream(LONG lHandle);
CPLAYER_API BOOL __stdcall AnalyzeDataOpenFile(LONG lHandle,char *sFileName);
CPLAYER_API void __stdcall AnalyzeDataClose(LONG lHandle);
CPLAYER_API BOOL __stdcall AnalyzeDataInputData(LONG lHandle,PBYTE pBuffer,DWORD dwBufferSize);
CPLAYER_API int __stdcall AnalyzeDataGetPacket(LONG lHandle,PACKET_INFO *pPacketInfo);
