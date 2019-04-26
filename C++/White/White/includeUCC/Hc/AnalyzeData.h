#include "stdAfx.h"

#define CPLAYER_API  extern "C"__declspec(dllimport)

#define FILE_HEAD 0 //文件头
#define VIDEO_I_FRAME 1 //视频I帧
#define VIDEO_B_FRAME 2 //视频B帧
#define VIDEO_P_FRAME 3 //视频P帧
#define VIDEO_BP_FRAME 4 //视频BP帧
#define VIDEO_BBP_FRAME 5 //视频B帧B帧P帧
#define AUDIO_PACKET 10 //音频包

typedef struct{
	int nPacketType;//包类型
	char *pPacketBuffer;//缓存区地址
	DWORD dwPacketSize; //包的大小
	//绝对时标
	int nYear;//时标:年		
	int nMonth;//时标:月
	int nDay;//时标:日
	int nHour;//时标:时
	int nMinute;//时标:分
	int nSecond;//时标:秒
	// 相对时标
	DWORD dwTimeStamp;	// 相对时标
}PACKET_INFO;

CPLAYER_API BOOL __stdcall AnalyzeDataOpenStream(LONG lHandle);
CPLAYER_API BOOL __stdcall AnalyzeDataOpenFile(LONG lHandle,char *sFileName);
CPLAYER_API void __stdcall AnalyzeDataClose(LONG lHandle);
CPLAYER_API BOOL __stdcall AnalyzeDataInputData(LONG lHandle,PBYTE pBuffer,DWORD dwBufferSize);
CPLAYER_API int __stdcall AnalyzeDataGetPacket(LONG lHandle,PACKET_INFO *pPacketInfo);
