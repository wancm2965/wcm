// AudioTranscode.h: interface for the AudioTranscode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VIDEC_MP4_UTIL_H__
#define __VIDEC_MP4_UTIL_H__


#include "../../include/VIDEC_MP4.h"
#include "../include/mp4v2/mp4v2.h"
#include <string>
using namespace std;

#ifdef _WIN32
#include <windows.h>
#include "mmsystem.h"
#pragma comment(lib, "winmm.lib")
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#endif

#define  MAX_AVSYNC_KEY			128

typedef struct _MP4_AVStream{

	MP4FileHandle	hFileHandle;
	int				nTimeScale;
	unsigned long	ulFirstWriteSampleTime;
	VIDEC_MP4_CALLBACK_FILE_FULL cb_file_full;
	void*			pUserData;
	unsigned long	ulFileSize;
	unsigned long	ulFileTime;
	int				nFirstVideoSyncKey;
	int				nTextTrackID;
	char*			pTextData;
	int				nTextDataLen;
	int				nTextDataBufLen;
	unsigned long   ulTextTimestamp;
	int				nAddVideoTrack[MAX_AVSYNC_KEY];
	int				nAudioTrackID[MAX_AVSYNC_KEY];
	int				nVideoTrackID[MAX_AVSYNC_KEY];
	char*			pAudioData[MAX_AVSYNC_KEY];
	char*			pVideoData[MAX_AVSYNC_KEY];
	int				nAudioBufLen[MAX_AVSYNC_KEY];
	int				nVideoBufLen[MAX_AVSYNC_KEY];
	int				nKeyFrame[MAX_AVSYNC_KEY];
	int				nVideoType[MAX_AVSYNC_KEY];
	unsigned long	ulAudioDataLen[MAX_AVSYNC_KEY];
	unsigned long	ulVideoDataLen[MAX_AVSYNC_KEY];
	unsigned long   ulAduioTimestamp[MAX_AVSYNC_KEY];
	unsigned long   ulVideoTimestamp[MAX_AVSYNC_KEY];
	unsigned long   ulAudioFilstTimestamp[MAX_AVSYNC_KEY];
	unsigned long   ulVideoFilstTimestamp[MAX_AVSYNC_KEY];
}MP4_AVStream;

extern int			 g_nAudioFillInFrameLen;
extern unsigned char g_ucAudioFillInFrame[4098];
extern unsigned long g_ulMaxTime;
extern unsigned long g_ulMaxSize;

unsigned long MP4_GetTimestamp(void);
void DebugOutput(const char* fmt,...);
bool GetKeyNail(unsigned char* data,unsigned int usize,unsigned char* & pNail,unsigned int& uNailSize);
bool GetOneSlider(unsigned char* data,unsigned int usize,unsigned int& uOffSet,unsigned int& uSliderSize);
bool GetOneNail(unsigned char* data,unsigned int usize,unsigned char* & pNail,unsigned int& uNailSize,unsigned int& uOffset);

#endif 