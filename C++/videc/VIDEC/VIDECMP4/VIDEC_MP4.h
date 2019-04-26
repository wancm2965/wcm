
#ifndef __VIDEC_MP4_H__
#define __VIDEC_MP4_H__

#ifdef VIDEC_MP4_EXPORT
#define VIDEC_MP4_API _declspec(dllexport)
#elif VIDEC_MP4_DLL
#define VIDEC_MP4_API _declspec(dllimport)
#else
#define VIDEC_MP4_API
#endif

typedef enum
{
	VIDEO_H264=0,
	VIDEO_H263=1,
}VIDEO_TYPE;

//录制多路音视频流API
/*MaxTime，或者MaxSize 其中一个达到最大值，将停止写入数据*/
VIDEC_MP4_API void  VIDEC_MP4_SetMaxTimeSize(unsigned long ulMaxTime/*ulMaxTime：秒为单位；默认值:7200*/,unsigned long ulMaxSize/*ulMaxSize：默认值:2*1024*1024*/);
typedef void (*VIDEC_MP4_CALLBACK_FILE_FULL)(const void*pUserData,void*pHandle,unsigned long ulFileTime,unsigned long ulFileSize);
VIDEC_MP4_API void  VIDEC_MP4_SetFileFullCallback(const void*pUserData,void*pHandle,VIDEC_MP4_CALLBACK_FILE_FULL cb_file_full);
VIDEC_MP4_API void  VIDEC_MP4_GetFileTimeSize(void*pHandle,unsigned long& ulFileTime,unsigned long& ulFileSize);


VIDEC_MP4_API void* VIDEC_MP4_Open(const char*cszPathName);
VIDEC_MP4_API void  VIDEC_MP4_Close(void*pHandle);

/*写入AAC裸数据*/
VIDEC_MP4_API int   VIDEC_MP4_WriteAudio(void*pHandle,const void *pData,int nLen,unsigned long nTimestamp,unsigned char ucAVSyncKey = 0 /* 0 <= nAVSyncKey < 128 标记同一路音视频唇音同步Key*/);

/*写入视频裸数据*/
VIDEC_MP4_API int   VIDEC_MP4_WriteVideo(void*pHandle,const void *pData,int nLen,unsigned long nTimestamp,int nKeyFrame,int nWidth,int nHeight,VIDEO_TYPE nType = VIDEO_H264,unsigned char ucAVSyncKey = 0 /* 0 <= nAVSyncKey < 128 标记同一路音视频唇音同步Key*/);

/*写入文本数据*/
VIDEC_MP4_API int   VIDEC_MP4_WriteText(void*pHandle,const char*cszText);


/*分离多路音视频流API*/
VIDEC_MP4_API int   VIDEC_MP4_Demux_Open(const char*srcfile,const char*dstfile);
VIDEC_MP4_API void  VIDEC_MP4_Demux_AddTrackToDstFile(unsigned int trackId);
VIDEC_MP4_API void  VIDEC_MP4_Demux_Close(void);
VIDEC_MP4_API int   VIDEC_MP4_Demux_GetTrackCount(void);/* trackId 从1开始编号*/
VIDEC_MP4_API const char* VIDEC_MP4_Demux_GetTrackType(unsigned int trackId);
VIDEC_MP4_API char* VIDEC_MP4_Demux_GetTrackInfo(unsigned int trackId=0/*trackId=0,循环追加所有的trackId*/);


//END


#endif
