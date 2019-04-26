//
//libavformat.a  libavutil.a libavcodec.a  libz.a  libgcc.a  libmingwex.a  libswscale.a libavfilter.a   libiconv.a  libmingw32.a  libintl.a   libswresample.a librtm.a  
// libx264.a dmoguids.lib libopencore-amrnb.a libopencore-amrwb.a libspeex.a 

#include "stdafx.h"
#include <AVCONAVC/H264Decoder.h>
#include <mmsystem.h>


#define _STDINT_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavutil/mem.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#ifdef __cplusplus
}
#endif


#include <tchar.h>
#include "../VIDEC_FRE/VIDECFREAPI.h"

typedef struct tagXVCodec
{
	tagXVCodec(void)
		: m_pAVCodec(NULL)
		, m_pAVCodecContext(NULL)
		, m_pAVFrame(NULL)
		, nFlag(0)
	{
	}
	int						nFlag;
    AVCodec*				m_pAVCodec;
    AVCodecContext*			m_pAVCodecContext;
    AVFrame*				m_pAVFrame;
}XVCodec,*PXVCodec;

//add ??
int gDecNewOutputTxt = 0;


#define  MAX_FILEPATH  1024 

void DebugOutputW(int nLevel,const TCHAR * cszInfo )
{
	static char info[MAX_FILEPATH];
#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, cszInfo, -1, info, MAX_FILEPATH, 0, 0);
#else
	strcpy(info, cszInfo);
#endif

	DebugOutput(nLevel,info);
}



AVH264DECODER_API void*AVH264DECODER_DecoderOpen(int nWidth,int nHeight,int AVCCodecID)
{
	gDecNewOutputTxt = 0;
	char  tmsg[256] = { 0 };
	sprintf(tmsg, ("H264Decoder_DecoderOpen w:%d,h:%d,codid:%d \n"),  nWidth,  nHeight,  AVCCodecID );
	DebugOutput( logharddec ,tmsg );

	PXVCodec pXVCodec=0;

	enum AVCodecID codecID=AV_CODEC_ID_H264;
	switch (AVCCodecID)
	{
	case AVC_CODEC_H263:
		codecID=AV_CODEC_ID_H263;
		break;
	case AVC_CODEC_H264:
		codecID=AV_CODEC_ID_H264;
		break;
	case AVC_CODEC_MPEG4:
		codecID=AV_CODEC_ID_MPEG4;
		break;
	case AVC_CODEC_MJPG:
		codecID=AV_CODEC_ID_MJPEG;
		break;
	default:
		return NULL;
	}

	pXVCodec=(PXVCodec)av_malloc(sizeof(XVCodec));
	if (!pXVCodec)
	{
		return NULL;
	}
	pXVCodec->m_pAVCodec=NULL;
	pXVCodec->m_pAVCodecContext=NULL;
	pXVCodec->m_pAVFrame=NULL;	

	pXVCodec->m_pAVCodec = avcodec_find_decoder((AVCodecID)codecID);
	if (pXVCodec->m_pAVCodec==NULL)
	{
		AVH264DECODER_DecoderClose(pXVCodec);

		return NULL;
	}

	pXVCodec->m_pAVCodecContext=avcodec_alloc_context3(pXVCodec->m_pAVCodec);
	pXVCodec->m_pAVFrame=av_frame_alloc();
	if (pXVCodec->m_pAVCodecContext==NULL || pXVCodec->m_pAVFrame==NULL)
	{
		AVH264DECODER_DecoderClose(pXVCodec);

		return NULL;
	}

	pXVCodec->m_pAVCodecContext->width =nWidth;
	pXVCodec->m_pAVCodecContext->height=nHeight;

	pXVCodec->m_pAVCodecContext->codec_id = (AVCodecID)codecID;
	if (codecID==AV_CODEC_ID_H264)
	{
		pXVCodec->m_pAVCodecContext->codec_tag             = MAKEFOURCC('A','V','C','1');
	}

	pXVCodec->m_pAVCodecContext->coded_width           = nWidth;
	pXVCodec->m_pAVCodecContext->coded_height          = nHeight;
	pXVCodec->m_pAVCodecContext->bits_per_coded_sample = 12;
	pXVCodec->m_pAVCodecContext->error_concealment     = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
	pXVCodec->m_pAVCodecContext->err_recognition       = AV_EF_CAREFUL;
	pXVCodec->m_pAVCodecContext->workaround_bugs       = FF_BUG_AUTODETECT;

	if (codecID == AV_CODEC_ID_H264)
		pXVCodec->m_pAVCodecContext->flags2           |= CODEC_FLAG2_SHOW_ALL;


	int nThreadCount=0;
	if (nWidth>=960 && nHeight>=540)
	{
		nThreadCount=2;
	}
	else if (nWidth>=1280 && nHeight>=720)
	{
		nThreadCount=4;
	}

	if (nThreadCount>0)
	{
		pXVCodec->m_pAVCodecContext->thread_count = nThreadCount;
		if (codecID==AV_CODEC_ID_H264)
		{
			pXVCodec->m_pAVCodecContext->thread_type = FF_THREAD_FRAME|FF_THREAD_SLICE;
		}
		else if (codecID==FF_THREAD_FRAME)
		{
			pXVCodec->m_pAVCodecContext->thread_type = FF_THREAD_FRAME;
		}
	}

	if (avcodec_open2(pXVCodec->m_pAVCodecContext,pXVCodec->m_pAVCodec,NULL)<0) 
	{
		AVH264DECODER_DecoderClose(pXVCodec);

		return NULL;
	}

	return pXVCodec;
}

AVH264DECODER_API void AVH264DECODER_DecoderClose(void*pHandle)
{
	DebugOutput( logharddec , ("H264Decoder_DecoderClose.  \n") );

	PXVCodec pXVCodec=(PXVCodec)pHandle;

	if (pXVCodec)
	{
		if (pXVCodec->m_pAVCodecContext)
		{
			avcodec_close(pXVCodec->m_pAVCodecContext);
			
			av_free(pXVCodec->m_pAVCodecContext);
			pXVCodec->m_pAVCodecContext=NULL;
			pXVCodec->m_pAVCodec=NULL;
		}

		if (pXVCodec->m_pAVFrame)
		{
			av_free(pXVCodec->m_pAVFrame);
			pXVCodec->m_pAVFrame=NULL;
		}
	
		av_free(pXVCodec);
		pXVCodec=NULL;
	}
}


AVH264DECODER_API int  AVH264DECODER_DecoderDecode(void*pHandle,unsigned char*pStreamData,int nStreamLen,unsigned char*pYV12[],int nStride[],int*pnWidth,int*pnHeight,int*pnUsedBytes,int*pnKeyFrame,char**ppqscale_table,int*pqstride, int* pix_fmt)
{
	//add ??
	if (gDecNewOutputTxt < 10)
	{
		char  tmsg[512] = { 0 };
		sprintf(tmsg, ("H264Decoder_DecoderDecode Handle:%p,StreamLen:%d, Width:%d, Height:%d, KeyFrame:%d,qstride:%d pix_fmt:%d\n"), pHandle, nStreamLen, *pnWidth,*pnHeight,* pnKeyFrame, * pqstride, *pix_fmt );
		DebugOutput( logharddec ,tmsg);
		gDecNewOutputTxt ++;
	}

	int nGotPicture=0;
	int nUsedBytes=0;
	PXVCodec pXVCodec=(PXVCodec)pHandle;
	if (pXVCodec)
	{
		AVPacket tAVPacket;
		av_init_packet(&tAVPacket);
		tAVPacket.data=pStreamData;
		tAVPacket.size=nStreamLen;

		try
		{
			nUsedBytes=avcodec_decode_video2(pXVCodec->m_pAVCodecContext,pXVCodec->m_pAVFrame,&nGotPicture,&tAVPacket);
		}
		catch(...)
		{
			nUsedBytes=-1;
			nGotPicture=0;
		}

		if (nUsedBytes<0)
			nUsedBytes=nStreamLen;
		
		*pnUsedBytes=nUsedBytes;
	    *pnWidth=pXVCodec->m_pAVCodecContext->width;
		*pnHeight=pXVCodec->m_pAVCodecContext->height;
		*pix_fmt = pXVCodec->m_pAVCodecContext->pix_fmt;
		//char szDebug[1024];
		//sprintf(szDebug,"nLen=%d nUsedBytes=%d nGotPicture=%d\n",nStreamLen,nUsedBytes,nGotPicture);
		//OutputDebugStringA(szDebug);

		if (nGotPicture)
		{
			*ppqscale_table=(char*)pXVCodec->m_pAVFrame->qscale_table;
			*pqstride=pXVCodec->m_pAVFrame->qstride;

			*pnKeyFrame=(pXVCodec->m_pAVFrame->key_frame || (pXVCodec->m_pAVFrame->pict_type==AV_PICTURE_TYPE_I));

			pYV12[0]=pXVCodec->m_pAVFrame->data[0];
			pYV12[1]=pXVCodec->m_pAVFrame->data[1];
			pYV12[2]=pXVCodec->m_pAVFrame->data[2];

			nStride[0]=pXVCodec->m_pAVFrame->linesize[0];
			nStride[1]=pXVCodec->m_pAVFrame->linesize[1];
			nStride[2]=pXVCodec->m_pAVFrame->linesize[2];
		}
	}

	return nGotPicture;
}

void AVH264DECODER_Initialize(void)
{
	 avcodec_register_all();
}

void AVH264DECODER_UnInitialize(void)
{
}

