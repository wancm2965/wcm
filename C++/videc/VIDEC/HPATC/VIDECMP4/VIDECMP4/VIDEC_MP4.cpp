

#include "VIDEC_MP4_Util.h"
#include "H264FrameParser.h"
#include <string.h>

#ifdef _WIN32
#else
	#define stricmp  strcasecmp
#endif

#define  MIN_SYNC_TIME 100

void  VIDEC_MP4_SetMaxTimeSize(unsigned long ulMaxTime/*ulMaxTime：秒为单位；默认值:7200*/,unsigned long ulMaxSize/*ulMaxSize：默认值:2*1024*1024*1024*/)
{
	if (0 < ulMaxTime && ulMaxTime < 7200)
	{
		g_ulMaxTime	= ulMaxTime;
	}
	if (0 < ulMaxSize && ulMaxSize < 2*1024*1024*1024)
	{
		g_ulMaxSize	= ulMaxSize;
	}
}

void  VIDEC_MP4_SetFileFullCallback(const void*pUserData,void*pHandle,VIDEC_MP4_CALLBACK_FILE_FULL cb_file_full)
{
	MP4_AVStream* pMP4Stream = (MP4_AVStream *)pHandle;  
	if (pMP4Stream == NULL || pMP4Stream->hFileHandle == NULL)
	{
		return;
	}
	pMP4Stream->pUserData = (void *)pUserData;
	pMP4Stream->cb_file_full = cb_file_full;
}

void  VIDEC_MP4_GetFileTimeSize(void*pHandle,unsigned long& ulFileTime,unsigned long& ulFileSize)
{
	MP4_AVStream* pMP4Stream = (MP4_AVStream *)pHandle;  
	if (pMP4Stream == NULL || pMP4Stream->hFileHandle == NULL)
	{
		return;
	}
	ulFileTime = pMP4Stream->ulFileTime;
	ulFileSize = pMP4Stream->ulFileSize;
}

void* VIDEC_MP4_Open(const char*cszPathName)
{
	if(cszPathName == NULL)
	{
		return NULL;
	}

	std::string strName = cszPathName;
	int nPos = strName.rfind('.');
	if (nPos > 0)
	{
		strName = strName.substr(0,nPos);
		strName +=".mp4";
	}

	// create mp4 file
	MP4FileHandle hFileHandle = MP4Create(strName.c_str());
	if (hFileHandle == MP4_INVALID_FILE_HANDLE)
	{		
		return NULL;
	}

	MP4_AVStream* pMP4Stream = new MP4_AVStream;
	if (pMP4Stream)
	{
		pMP4Stream->hFileHandle = hFileHandle;
		pMP4Stream->nTimeScale = 90000;
		pMP4Stream->ulFirstWriteSampleTime = 0;
		pMP4Stream->cb_file_full = NULL;
		pMP4Stream->ulFileSize = 0;
		pMP4Stream->ulFileTime = 0;
		pMP4Stream->nFirstVideoSyncKey = -1;
		pMP4Stream->nTextTrackID = MP4_INVALID_TRACK_ID;
		pMP4Stream->pTextData = NULL;
		pMP4Stream->nTextDataLen = 0;
		pMP4Stream->nTextDataBufLen = 0;
		pMP4Stream->ulTextTimestamp = 0;


		for(int i = 0;i<MAX_AVSYNC_KEY;i++)
		{
			pMP4Stream->nAudioTrackID[i] = MP4_INVALID_TRACK_ID;
			pMP4Stream->nVideoTrackID[i] = MP4_INVALID_TRACK_ID;
			pMP4Stream->nAddVideoTrack[i] = 0;
			pMP4Stream->nAudioBufLen[i] = 0;
			pMP4Stream->nVideoBufLen[i] = 0;			
			pMP4Stream->pAudioData[i] = NULL;
			pMP4Stream->pVideoData[i] = NULL;
			pMP4Stream->nKeyFrame[i] = 1;
			pMP4Stream->ulAudioDataLen[i] = 0;
			pMP4Stream->ulVideoDataLen[i] = 0;
			pMP4Stream->ulAduioTimestamp[i] = 0;
			pMP4Stream->ulVideoTimestamp[i] = 0;
			pMP4Stream->ulAudioFilstTimestamp[i] = 0;
			pMP4Stream->ulVideoFilstTimestamp[i] = 0;
		}

		MP4SetVideoProfileLevel(pMP4Stream->hFileHandle, 0x01); //Simple Profile @ Level 3 //0x0F 0x7F
		MP4SetAudioProfileLevel(pMP4Stream->hFileHandle, 0x01);//0x01 0x02 0x0F  0xFE;	
		MP4SetTimeScale(pMP4Stream->hFileHandle, pMP4Stream->nTimeScale);
	}
	return pMP4Stream;
}

void VIDEC_MP4_Close(void*pHandle)
{
	MP4_AVStream* pMP4Stream = (MP4_AVStream *)pHandle;  
	if (pMP4Stream == NULL || pMP4Stream->hFileHandle == NULL)
	{
		return;
	}

	if (pMP4Stream->nTextTrackID  != MP4_INVALID_TRACK_ID)
	{
		VIDEC_MP4_WriteText(pHandle,"  ");
	}

	MP4Close(pMP4Stream->hFileHandle);	

	for(int i = 0;i<MAX_AVSYNC_KEY;i++)
	{
		if (pMP4Stream->pAudioData[i] != NULL)
		{
			free(pMP4Stream->pAudioData[i]);
			pMP4Stream->pAudioData[i] = NULL;
		}
		if (pMP4Stream->pVideoData[i] != NULL)
		{
			free(pMP4Stream->pVideoData[i]);
			pMP4Stream->pVideoData[i] = NULL;
		}
	}
	if (pMP4Stream->pTextData!=NULL)
	{
		free(pMP4Stream->pTextData);
		pMP4Stream->pTextData=NULL;
	}

	delete pMP4Stream;
	pMP4Stream = NULL;
}

int VIDEC_MP4_WriteAudio(void*pHandle,const void *pData,int nLen,unsigned long nTimestamp,unsigned char ucAVSyncKey /* 0 <= nAVSyncKey < 128 标记同一路音视频唇音同步Key*/)
{
	//DebugOutput("VIDEC_MP4_WriteAudio nTimestamp = %u\n",nTimestamp);

	MP4_AVStream* pMP4Stream = (MP4_AVStream *)pHandle;  
	if (pMP4Stream == NULL || pMP4Stream->hFileHandle == NULL || ucAVSyncKey >= MAX_AVSYNC_KEY)
	{
		return -1;
	}

	if (pMP4Stream->nAudioTrackID[ucAVSyncKey]  == MP4_INVALID_TRACK_ID)
	{
		pMP4Stream->nAudioTrackID[ucAVSyncKey] = MP4AddAudioTrack(pMP4Stream->hFileHandle,44100, 1024 ,MP4_MPEG4_AUDIO_TYPE );
		if (pMP4Stream->nAudioTrackID[ucAVSyncKey]  != MP4_INVALID_TRACK_ID)
		{
			uint8_t ut[2] ={0x12,0x08};
			MP4SetTrackESConfiguration(pMP4Stream->hFileHandle,pMP4Stream->nAudioTrackID[ucAVSyncKey],ut,2);
		}
	}
	if (pMP4Stream->nAudioTrackID[ucAVSyncKey]  == MP4_INVALID_TRACK_ID)
	{			
		return -1;
	}

	unsigned long ulTimeTemp = nTimestamp;
	if (pMP4Stream->ulFirstWriteSampleTime == 0)
	{
		pMP4Stream->ulFirstWriteSampleTime	= MP4_GetTimestamp();
		pMP4Stream->ulAudioFilstTimestamp[ucAVSyncKey] = nTimestamp;
	}
	else if (pMP4Stream->ulAudioFilstTimestamp[ucAVSyncKey] == 0)
	{
		if (pMP4Stream->ulVideoFilstTimestamp[ucAVSyncKey] == 0)
		{
			unsigned long ulTime = MP4_GetTimestamp();
			if (ulTime - pMP4Stream->ulFirstWriteSampleTime > MIN_SYNC_TIME)
			{

				unsigned long nCycle = (ulTime - pMP4Stream->ulFirstWriteSampleTime)/22;
				for (unsigned long i = 0;i< nCycle;i++)
				{
					MP4WriteSample(pMP4Stream->hFileHandle, pMP4Stream->nAudioTrackID[ucAVSyncKey], g_ucAudioFillInFrame,g_nAudioFillInFrameLen, 22*44.1 , 0,1);	
				}
				nTimestamp -= (ulTime - pMP4Stream->ulFirstWriteSampleTime)%22;
			}
			pMP4Stream->ulAudioFilstTimestamp[ucAVSyncKey] = nTimestamp - (ulTime - pMP4Stream->ulFirstWriteSampleTime) + (ulTime - pMP4Stream->ulFirstWriteSampleTime)%22;
		}
		else
		{
			unsigned long nCycle = 0;
			if (nTimestamp > pMP4Stream->ulVideoFilstTimestamp[ucAVSyncKey] && nTimestamp - pMP4Stream->ulVideoFilstTimestamp[ucAVSyncKey] > MIN_SYNC_TIME)
			{
				nCycle = (nTimestamp - pMP4Stream->ulVideoFilstTimestamp[ucAVSyncKey])/22;
				for (unsigned long i = 0;i< nCycle;i++)
				{
					MP4WriteSample(pMP4Stream->hFileHandle, pMP4Stream->nAudioTrackID[ucAVSyncKey], g_ucAudioFillInFrame,g_nAudioFillInFrameLen, 22*44.1 , 0,1);	
				}
				nTimestamp -= (nTimestamp - pMP4Stream->ulVideoFilstTimestamp[ucAVSyncKey])%22;
			}			
			pMP4Stream->ulAudioFilstTimestamp[ucAVSyncKey] = pMP4Stream->ulVideoFilstTimestamp[ucAVSyncKey];
		}
	}

	if (pMP4Stream->ulFileSize >= g_ulMaxSize || pMP4Stream->ulFileTime >= g_ulMaxTime)
	{
		if (pMP4Stream->cb_file_full)
		{
			pMP4Stream->cb_file_full(pMP4Stream->pUserData,pMP4Stream,pMP4Stream->ulFileTime,pMP4Stream->ulFileSize);
		}
		return -1;
	}
	pMP4Stream->ulFileTime = (MP4_GetTimestamp() - pMP4Stream->ulFirstWriteSampleTime)/1000;
	pMP4Stream->ulFileSize +=nLen;

	if (pMP4Stream->ulAduioTimestamp[ucAVSyncKey] > 0 && pMP4Stream->pAudioData[ucAVSyncKey] != NULL && nTimestamp > pMP4Stream->ulAduioTimestamp[ucAVSyncKey])
	{
		if ((nTimestamp - pMP4Stream->ulAduioTimestamp[ucAVSyncKey]) > MIN_SYNC_TIME)
		{
			unsigned long nCycle = (nTimestamp - pMP4Stream->ulAduioTimestamp[ucAVSyncKey])/22;
			for (unsigned long i = 0;i< (nCycle-1);i++)
			{
				MP4WriteSample(pMP4Stream->hFileHandle, pMP4Stream->nAudioTrackID[ucAVSyncKey], g_ucAudioFillInFrame,g_nAudioFillInFrameLen, 22*44.1 , 0,1);	
			}
			MP4WriteSample(pMP4Stream->hFileHandle, pMP4Stream->nAudioTrackID[ucAVSyncKey], (uint8_t*)pMP4Stream->pAudioData[ucAVSyncKey], pMP4Stream->ulAudioDataLen[ucAVSyncKey],((nTimestamp - pMP4Stream->ulAduioTimestamp[ucAVSyncKey])%22 + 22)*44.1 , 0,1);	
		}
		else
		{
			MP4WriteSample(pMP4Stream->hFileHandle, pMP4Stream->nAudioTrackID[ucAVSyncKey], (uint8_t*)pMP4Stream->pAudioData[ucAVSyncKey], pMP4Stream->ulAudioDataLen[ucAVSyncKey],(nTimestamp - pMP4Stream->ulAduioTimestamp[ucAVSyncKey])*44.1 , 0,1);	
		}
	}	

	if (pMP4Stream->pAudioData[ucAVSyncKey]==NULL || pMP4Stream->nAudioBufLen[ucAVSyncKey]<nLen)
	{
		pMP4Stream->nAudioBufLen[ucAVSyncKey]=nLen;

		if (pMP4Stream->pAudioData[ucAVSyncKey]!=NULL)
		{
			free(pMP4Stream->pAudioData[ucAVSyncKey]);
			pMP4Stream->pAudioData[ucAVSyncKey]=NULL;
		}
		pMP4Stream->pAudioData[ucAVSyncKey]=(char*)malloc(nLen);
		if (pMP4Stream->pAudioData[ucAVSyncKey]==NULL)
		{
			return -1;
		}
	}

	if (pMP4Stream->pAudioData[ucAVSyncKey] && nTimestamp > pMP4Stream->ulAduioTimestamp[ucAVSyncKey])
	{
		memcpy(pMP4Stream->pAudioData[ucAVSyncKey],pData,nLen);
		pMP4Stream->ulAudioDataLen[ucAVSyncKey] = nLen;
		pMP4Stream->ulAduioTimestamp[ucAVSyncKey] = nTimestamp;
	}

	return 0;
}

int VIDEC_MP4_WriteVideo(void*pHandle,const void *pData,int nLen,unsigned long nTimestamp,int nKeyFrame,int nWidth,int nHeight,VIDEO_TYPE nType,unsigned char ucAVSyncKey /* 0 <= nAVSyncKey < MAX_AVSYNC_KEY 标记同一路音视频唇音同步Key*/)
{
	MP4_AVStream* pMP4Stream = (MP4_AVStream *)pHandle;  
	if (pMP4Stream == NULL || pMP4Stream->hFileHandle == NULL || ucAVSyncKey >= MAX_AVSYNC_KEY)
	{
		return -1;
	}

	if (nType == VIDEO_H264)
	{
		if (nKeyFrame && pData != NULL && nLen > 16 && pMP4Stream->nAddVideoTrack[ucAVSyncKey] == 0)
		{
			char spsBuf[1024] = {0};
			char ppsBuf[1024] = {0};
			char* outBuf = (char*)malloc(nLen + 512);
			if (outBuf == NULL)
			{
				return -1;
			}
			int outLen;int spsSize;int ppsSize;bool isKeyframe;int width; int height;
			ParseH264Frame((const char *)pData, nLen,outBuf,outLen,spsBuf,spsSize,ppsBuf,ppsSize,isKeyframe, width, height);
			if (spsSize > 0 && ppsSize > 0)
			{				
				pMP4Stream->nVideoTrackID[ucAVSyncKey] = MP4AddH264VideoTrack(pMP4Stream->hFileHandle, pMP4Stream->nTimeScale,MP4_INVALID_DURATION, width,height, \
					spsBuf[1],	//AVCProfileIndication
					spsBuf[2],	//profile_compat
					spsBuf[3],	//AVCLevelIndication
					3);			//4 bytes length before each NAL unit
				if (pMP4Stream->nVideoTrackID[ucAVSyncKey] != MP4_INVALID_TRACK_ID)
				{
					pMP4Stream->nAddVideoTrack[ucAVSyncKey] = 1;
					if (pMP4Stream->nFirstVideoSyncKey == -1)
					{
						pMP4Stream->nFirstVideoSyncKey = ucAVSyncKey;
					}
					MP4AddH264SequenceParameterSet(pMP4Stream->hFileHandle, pMP4Stream->nVideoTrackID[ucAVSyncKey], (const uint8_t *)spsBuf, spsSize );
					MP4AddH264PictureParameterSet(pMP4Stream->hFileHandle, pMP4Stream->nVideoTrackID[ucAVSyncKey], (const uint8_t *)ppsBuf,ppsSize);
				}
			}
			free(outBuf);
			outBuf = NULL;
		}
	}	
	else
	{
		if (pMP4Stream->nAddVideoTrack[ucAVSyncKey] == 0)
		{
			pMP4Stream->nVideoTrackID[ucAVSyncKey] = MP4AddH263VideoTrack(pMP4Stream->hFileHandle, pMP4Stream->nTimeScale,MP4_INVALID_DURATION , nWidth,nHeight,0,0,0,0);			//4 bytes length before each NAL unit
		}
		if (pMP4Stream->nVideoTrackID[ucAVSyncKey]  != MP4_INVALID_TRACK_ID)
		{
			pMP4Stream->nAddVideoTrack[ucAVSyncKey] = 1;
			if (pMP4Stream->nFirstVideoSyncKey == -1)
			{
				pMP4Stream->nFirstVideoSyncKey = ucAVSyncKey;
			}
		}
	}

	if (pMP4Stream->nVideoTrackID[ucAVSyncKey]  == MP4_INVALID_TRACK_ID)
	{
		return -1;
	}

	if (pMP4Stream->ulFirstWriteSampleTime == 0)
	{
		pMP4Stream->ulFirstWriteSampleTime	= MP4_GetTimestamp();
		pMP4Stream->ulVideoFilstTimestamp[ucAVSyncKey] = nTimestamp;

	}
	else if (pMP4Stream->ulVideoFilstTimestamp[ucAVSyncKey] == 0)
	{
		if (pMP4Stream->ulAudioFilstTimestamp[ucAVSyncKey] == 0)
		{
			unsigned long ulTime = MP4_GetTimestamp();
			if (ulTime - pMP4Stream->ulFirstWriteSampleTime > MIN_SYNC_TIME)
			{
				nTimestamp -= (ulTime - pMP4Stream->ulFirstWriteSampleTime);
			}
			pMP4Stream->ulVideoFilstTimestamp[ucAVSyncKey] = nTimestamp;
		}
		else
		{
			if (nTimestamp > pMP4Stream->ulAudioFilstTimestamp[ucAVSyncKey] && nTimestamp - pMP4Stream->ulAudioFilstTimestamp[ucAVSyncKey] > MIN_SYNC_TIME)
			{
				nTimestamp -=(nTimestamp - pMP4Stream->ulAudioFilstTimestamp[ucAVSyncKey]);
			}
			pMP4Stream->ulVideoFilstTimestamp[ucAVSyncKey] = pMP4Stream->ulAudioFilstTimestamp[ucAVSyncKey];
		}
	}
	
	if (pMP4Stream->ulFileSize >= g_ulMaxSize || pMP4Stream->ulFileTime >= g_ulMaxTime)
	{
		if (pMP4Stream->cb_file_full)
		{
			pMP4Stream->cb_file_full(pMP4Stream->pUserData,pMP4Stream,pMP4Stream->ulFileTime,pMP4Stream->ulFileSize);
		}
		return -1;
	}
	pMP4Stream->ulFileTime = (MP4_GetTimestamp() - pMP4Stream->ulFirstWriteSampleTime)/1000;
	pMP4Stream->ulFileSize +=nLen;

	if (pMP4Stream->ulVideoTimestamp[ucAVSyncKey] > 0 && pMP4Stream->pVideoData[ucAVSyncKey] != NULL && nTimestamp > pMP4Stream->ulVideoTimestamp[ucAVSyncKey])
	{
		MP4WriteSample(pMP4Stream->hFileHandle, pMP4Stream->nVideoTrackID[ucAVSyncKey],(uint8_t *)pMP4Stream->pVideoData[ucAVSyncKey] , pMP4Stream->ulVideoDataLen[ucAVSyncKey],(nTimestamp - pMP4Stream->ulVideoTimestamp[ucAVSyncKey])*90, 0,pMP4Stream->nKeyFrame[ucAVSyncKey]);
	}

	if (pMP4Stream->pVideoData[ucAVSyncKey]==NULL || pMP4Stream->nVideoBufLen[ucAVSyncKey] < nLen + 512)
	{
		pMP4Stream->nVideoBufLen[ucAVSyncKey]=nLen + 512;

		if (pMP4Stream->pVideoData[ucAVSyncKey]!=NULL)
		{
			free(pMP4Stream->pVideoData[ucAVSyncKey]);
			pMP4Stream->pVideoData[ucAVSyncKey]=NULL;
		}
		pMP4Stream->pVideoData[ucAVSyncKey]=(char*)malloc(nLen + 512);
		if (pMP4Stream->pVideoData[ucAVSyncKey]==NULL)
		{
			return -1;
		}
	}

	if (pMP4Stream->pVideoData[ucAVSyncKey] && nTimestamp > pMP4Stream->ulVideoTimestamp[ucAVSyncKey])
	{
		if (nType == VIDEO_H264)
		{
			int outLen=0;
			int spsSize;int ppsSize;bool isKeyframe;int width; int height;
			ParseH264Frame((const char *)pData,nLen,pMP4Stream->pVideoData[ucAVSyncKey],outLen,NULL,spsSize,NULL,ppsSize,isKeyframe, width, height);
			pMP4Stream->ulVideoDataLen[ucAVSyncKey]  = outLen;
		}
		else
		{
			memcpy(pMP4Stream->pVideoData[ucAVSyncKey],pData,nLen);
			pMP4Stream->ulVideoDataLen[ucAVSyncKey] = nLen;
		}
		pMP4Stream->ulVideoTimestamp[ucAVSyncKey] = nTimestamp;
		pMP4Stream->nKeyFrame[ucAVSyncKey] = nKeyFrame;
		pMP4Stream->nVideoType[ucAVSyncKey] = nType;
	}

	return 0;
}

int VIDEC_MP4_WriteText(void*pHandle,const char*cszText)
{
	MP4_AVStream* pMP4Stream = (MP4_AVStream *)pHandle;  
	if (pMP4Stream == NULL || pMP4Stream->hFileHandle == NULL)
	{
		return -1;
	}

	if (pMP4Stream->nFirstVideoSyncKey == -1 )
	{
		return -1;
	}

	unsigned long ulTime = MP4_GetTimestamp();
	if (pMP4Stream->nTextTrackID  == MP4_INVALID_TRACK_ID)
	{
		pMP4Stream->nTextTrackID = MP4AddTextTrack(pMP4Stream->hFileHandle,pMP4Stream->nVideoTrackID[pMP4Stream->nFirstVideoSyncKey]);
		if (pMP4Stream->nTextTrackID  != MP4_INVALID_TRACK_ID)
		{
			const uint8_t ut[32] = " ";
			MP4WriteSample(pMP4Stream->hFileHandle, pMP4Stream->nTextTrackID,ut , 1,(ulTime - pMP4Stream->ulFirstWriteSampleTime)*90, 0,1);
		}
	}
	else if(pMP4Stream->pTextData && pMP4Stream->ulTextTimestamp > 0 && ulTime > pMP4Stream->ulTextTimestamp)
	{
		MP4WriteSample(pMP4Stream->hFileHandle, pMP4Stream->nTextTrackID,(uint8_t *)pMP4Stream->pTextData , pMP4Stream->nTextDataLen,(ulTime - pMP4Stream->ulTextTimestamp)*90, 0,1);
	}

	int nLen = strlen(cszText);
	if (pMP4Stream->pTextData==NULL || pMP4Stream->nTextDataBufLen < nLen)
	{
		pMP4Stream->nTextDataBufLen=nLen;

		if (pMP4Stream->pTextData!=NULL)
		{
			free(pMP4Stream->pTextData);
			pMP4Stream->pTextData=NULL;
		}
		pMP4Stream->pTextData=(char*)malloc(nLen);
		if (pMP4Stream->pTextData==NULL)
		{
			return -1;
		}
	}
	if (pMP4Stream->pTextData)
	{
		memcpy(pMP4Stream->pTextData,cszText,nLen);
		pMP4Stream->nTextDataLen = nLen;		
		pMP4Stream->ulTextTimestamp = ulTime;
	}

	return 0;
}


MP4FileHandle g_hSrcFileHandle = MP4_INVALID_FILE_HANDLE;
MP4FileHandle g_hDstFileHandle = MP4_INVALID_FILE_HANDLE;

//分离多路音视频流API
int   VIDEC_MP4_Demux_Open(const char*srcfile,const char*dstfile)
{
	if (g_hSrcFileHandle !=MP4_INVALID_FILE_HANDLE || g_hDstFileHandle!=MP4_INVALID_FILE_HANDLE || srcfile == NULL || dstfile == NULL)
	{
		return -1;
	}
	std::string strName = dstfile;
	int nPos = strName.rfind('.');
	if (nPos > 0)
	{
		strName = strName.substr(0,nPos);
		strName +=".mp4";
	}

	g_hSrcFileHandle = MP4Read(srcfile);
	if (g_hSrcFileHandle == MP4_INVALID_FILE_HANDLE)
	{		
		return -1;
	}

	// create mp4 file
	g_hDstFileHandle = MP4Create(strName.c_str());
	if (g_hDstFileHandle == MP4_INVALID_FILE_HANDLE)
	{		
		return -1;
	}

	return 0;
}

void  VIDEC_MP4_Demux_Close(void)
{
	if (g_hSrcFileHandle != MP4_INVALID_FILE_HANDLE)
	{
		MP4Close(g_hSrcFileHandle);	
		g_hSrcFileHandle = MP4_INVALID_FILE_HANDLE;
	}

	if (g_hDstFileHandle != MP4_INVALID_FILE_HANDLE)
	{
		MP4Close(g_hDstFileHandle);	
		g_hDstFileHandle = MP4_INVALID_FILE_HANDLE;
	}
}

void  VIDEC_MP4_Demux_AddTrackToDstFile(unsigned int trackId)
{
	if (g_hSrcFileHandle == MP4_INVALID_FILE_HANDLE || g_hDstFileHandle == MP4_INVALID_FILE_HANDLE)
	{
		return;
	}
	MP4CopyTrack(g_hSrcFileHandle,trackId,g_hDstFileHandle);
}

int   VIDEC_MP4_Demux_GetTrackCount(void)/* trackId 从0开始编号*/
{
	if (g_hSrcFileHandle == MP4_INVALID_FILE_HANDLE)
	{
		return 0;
	}
	return MP4GetNumberOfTracks(g_hSrcFileHandle);
}

const char* VIDEC_MP4_Demux_GetTrackType(unsigned int trackId)
{
	if (g_hSrcFileHandle == MP4_INVALID_FILE_HANDLE)
	{
		return NULL;
	}
	const char * type = MP4GetTrackType(g_hSrcFileHandle,trackId);
	if (!stricmp(type, "vide")
		|| !stricmp(type, "video")
		|| !stricmp(type, "mp4v")
		|| !stricmp(type, "avc1")
		|| !stricmp(type, "s263")  // 3GPP H.263
		|| !stricmp(type, "encv")) {
			return "video";
	}

	if (!stricmp(type, "soun")
		|| !stricmp(type, "sound")
		|| !stricmp(type, "audio")
		|| !stricmp(type, "enca")
		|| !stricmp(type, "samr")  // 3GPP AMR
		|| !stricmp(type, "sawb")  // 3GPP AMR/WB
		|| !stricmp(type, "mp4a")) {
			return "audio";
	}

	return type;
}

char* VIDEC_MP4_Demux_GetTrackInfo(unsigned int trackId/*trackId=0,循环追加所有的trackId*/)
{
	if (g_hSrcFileHandle == MP4_INVALID_FILE_HANDLE)
	{
		return NULL;
	}
	return  MP4Info(g_hSrcFileHandle,trackId);	
}
