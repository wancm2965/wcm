// VIDEC264.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "VIDEC_264.h"
#include <mmsystem.h>

#  define UINT64_C(c)    c ## ULL
#  define INT64_C(c)    c ## LL

#define RTP_PAYLOAD_SIZE			750
#define MP4V_RTP_PAYLOAD_SIZE		900

#if defined(_WIN32)
#	define H264_RTP_PAYLOAD_SIZE	1300
#else
#	define H264_RTP_PAYLOAD_SIZE	900
#endif

#define _STDINT_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavutil/mem.h>
#include <libavcodec/avcodec.h>
#ifdef __cplusplus
}
#endif

typedef struct tagXVCodec
{
	AVCodec*				m_pAVCodec;
	AVCodecContext*			m_pAVCodecContext;
	AVFrame*				m_pAVFrame;
	VIDEC_H264_CODEC_TYPE	m_nCodecType;
}XVCodec,*PXVCodec;

VIDEC_264_API void*VIDEC264_EncoderOpen(VIDEC_H264_CODEC_TYPE nType,int nWidth,int nHeight,int nFrameRate,int nBitrate)
{
	PXVCodec pXVCodec=0;
	enum CodecID codecID=CODEC_ID_H263;

	switch (nType)
	{
	case VIDEC_H264_CODEC_H263:
		codecID=CODEC_ID_H263;
		break;
	case VIDEC_H264_CODEC_H263P:
	case VIDEC_H264_CODEC_H263PP:
		codecID=CODEC_ID_H263P;
		break;
	case VIDEC_H264_CODEC_H264:
		codecID=CODEC_ID_H264;
		break;
	case VIDEC_H264_CODEC_MPEG4:
		codecID=CODEC_ID_MPEG4;
		break;
	default:
		return 0;
	}

	pXVCodec=(PXVCodec)av_malloc(sizeof(XVCodec));
	memset(pXVCodec,0,sizeof(XVCodec));

	pXVCodec->m_nCodecType=nType;

	pXVCodec->m_pAVCodecContext = avcodec_alloc_context();
	pXVCodec->m_pAVFrame = avcodec_alloc_frame();
	if (pXVCodec->m_pAVCodecContext==NULL || pXVCodec->m_pAVFrame==NULL)
	{
		if(pXVCodec->m_pAVCodecContext!=NULL)
		{
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

		return NULL;
	}

	avcodec_get_frame_defaults(pXVCodec->m_pAVFrame);
	pXVCodec->m_pAVFrame->linesize[0] = nWidth;
	pXVCodec->m_pAVFrame->linesize[1] = nWidth / 2;
	pXVCodec->m_pAVFrame->linesize[2] = nWidth / 2;

	avcodec_get_context_defaults(pXVCodec->m_pAVCodecContext);

	pXVCodec->m_pAVCodec = avcodec_find_encoder(codecID);
	
	if (pXVCodec->m_pAVCodec==NULL)
	{
		if(pXVCodec->m_pAVCodecContext!=NULL)
		{
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

		return NULL;
	}

	switch(nType)
	{
	case VIDEC_H264_CODEC_H263:
	case VIDEC_H264_CODEC_H263P:
	case VIDEC_H264_CODEC_H263PP:
		{
			pXVCodec->m_pAVCodecContext->pix_fmt		= PIX_FMT_YUV420P;
			pXVCodec->m_pAVCodecContext->time_base.num  = 1;
			pXVCodec->m_pAVCodecContext->time_base.den  = nFrameRate;
			pXVCodec->m_pAVCodecContext->width = nWidth;
			pXVCodec->m_pAVCodecContext->height = nHeight;

			pXVCodec->m_pAVCodecContext->qmin = 4;
			pXVCodec->m_pAVCodecContext->qmax = 31;
			pXVCodec->m_pAVCodecContext->mb_decision = FF_MB_DECISION_RD;

			pXVCodec->m_pAVCodecContext->thread_count = 1;
			pXVCodec->m_pAVCodecContext->rtp_payload_size = RTP_PAYLOAD_SIZE;
			pXVCodec->m_pAVCodecContext->opaque = NULL;
			pXVCodec->m_pAVCodecContext->gop_size = nFrameRate*4;
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_QSCALE;
			pXVCodec->m_pAVCodecContext->global_quality = FF_QP2LAMBDA;
			pXVCodec->m_pAVCodecContext->max_b_frames = 0;
			
			pXVCodec->m_pAVCodecContext->bit_rate = (int) (nBitrate * 0.80f);
			pXVCodec->m_pAVCodecContext->bit_rate_tolerance = (int) (nBitrate * 0.20f);
		}
		break;
	case VIDEC_H264_CODEC_MPEG4:
		{
			int nMP4VProfile=1;
			if (nWidth<=176)
				nMP4VProfile=1;
			else if (nWidth<352)
				nMP4VProfile=2;
			else
				nMP4VProfile=3;

			pXVCodec->m_pAVCodecContext->pix_fmt		= PIX_FMT_YUV420P;
			pXVCodec->m_pAVCodecContext->time_base.num  = 1;
			pXVCodec->m_pAVCodecContext->time_base.den  = nFrameRate;
			pXVCodec->m_pAVCodecContext->width = nWidth;
			pXVCodec->m_pAVCodecContext->height = nHeight;
			pXVCodec->m_pAVCodecContext->mb_decision = FF_MB_DECISION_RD;
			pXVCodec->m_pAVCodecContext->noise_reduction = 250;
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_QSCALE;
			pXVCodec->m_pAVCodecContext->global_quality = FF_QP2LAMBDA;

			pXVCodec->m_pAVCodecContext->thread_count = 1;
			pXVCodec->m_pAVCodecContext->rtp_payload_size = MP4V_RTP_PAYLOAD_SIZE;
			pXVCodec->m_pAVCodecContext->opaque = NULL;
			pXVCodec->m_pAVCodecContext->bit_rate = (int) (nBitrate * 0.80f);
			pXVCodec->m_pAVCodecContext->bit_rate_tolerance = (int) (nBitrate * 0.20f);
			pXVCodec->m_pAVCodecContext->profile = nMP4VProfile>>4;
			pXVCodec->m_pAVCodecContext->level = nMP4VProfile & 0x0F;
			pXVCodec->m_pAVCodecContext->gop_size = nFrameRate*4;
			pXVCodec->m_pAVCodecContext->max_b_frames = 0;
			pXVCodec->m_pAVCodecContext->b_frame_strategy = 1;
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_AC_PRED;

		}
		break;
	case VIDEC_H264_CODEC_H264:
		{
			pXVCodec->m_pAVCodecContext->dsp_mask = (FF_MM_MMX | FF_MM_MMXEXT | FF_MM_SSE);

			pXVCodec->m_pAVCodecContext->pix_fmt		= PIX_FMT_YUV420P;
			pXVCodec->m_pAVCodecContext->time_base.num  = 1;
			pXVCodec->m_pAVCodecContext->time_base.den  = nFrameRate;
			pXVCodec->m_pAVCodecContext->width = nWidth;
			pXVCodec->m_pAVCodecContext->height = nHeight;

			pXVCodec->m_pAVCodecContext->rc_lookahead = 0;

			pXVCodec->m_pAVCodecContext->scenechange_threshold = 0;
			pXVCodec->m_pAVCodecContext->me_subpel_quality = 0;
			pXVCodec->m_pAVCodecContext->partitions = X264_PART_I4X4 | X264_PART_I8X8 | X264_PART_P8X8 | X264_PART_B8X8;
			pXVCodec->m_pAVCodecContext->me_method = ME_EPZS;
			pXVCodec->m_pAVCodecContext->trellis = 0;

			pXVCodec->m_pAVCodecContext->me_range = 16;
			pXVCodec->m_pAVCodecContext->max_qdiff = 4;
			pXVCodec->m_pAVCodecContext->mb_qmin = pXVCodec->m_pAVCodecContext->qmin = 10;
			pXVCodec->m_pAVCodecContext->mb_qmax = pXVCodec->m_pAVCodecContext->qmax = 51;
			pXVCodec->m_pAVCodecContext->qcompress = 0.6f;
			pXVCodec->m_pAVCodecContext->mb_decision = FF_MB_DECISION_SIMPLE;
			pXVCodec->m_pAVCodecContext->flags2 |= CODEC_FLAG2_FASTPSKIP;
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_LOOP_FILTER;
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
			pXVCodec->m_pAVCodecContext->max_b_frames = 0;
			pXVCodec->m_pAVCodecContext->b_frame_strategy = 1;
			pXVCodec->m_pAVCodecContext->chromaoffset = 0;

			pXVCodec->m_pAVCodecContext->crf = 22;
			pXVCodec->m_pAVCodecContext->thread_count = 0;
			pXVCodec->m_pAVCodecContext->rtp_payload_size = H264_RTP_PAYLOAD_SIZE;
			pXVCodec->m_pAVCodecContext->opaque = NULL;
			pXVCodec->m_pAVCodecContext->gop_size = nFrameRate * 4;
					
			pXVCodec->m_pAVCodecContext->profile = FF_PROFILE_H264_BASELINE;
			if (nWidth<=176)
				pXVCodec->m_pAVCodecContext->level = 10;
			else if (nWidth<352)
				pXVCodec->m_pAVCodecContext->level = 20;
			else if (nWidth<=352)
				pXVCodec->m_pAVCodecContext->level = 30;
			else
				pXVCodec->m_pAVCodecContext->level = 30;

			pXVCodec->m_pAVCodecContext->bit_rate = (int) (nBitrate * 0.80f);
			pXVCodec->m_pAVCodecContext->bit_rate_tolerance = (int) (nBitrate * 0.20f);
		}
		break;
	}
	switch(nType)
	{
	case VIDEC_H264_CODEC_H263:
		{	// H263 - 1996 
			break;
		}
	case VIDEC_H264_CODEC_H263P:
		{	// H263 - 1998 
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_H263P_UMV;		// Annex D+ 
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_AC_PRED;			// Annex I and T 
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_LOOP_FILTER;		// Annex J 
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_H263P_SLICE_STRUCT;	// Annex K 
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_H263P_AIV;			// Annex S 
			break;
		}
	case VIDEC_H264_CODEC_H263PP:
		{	// H263 - 2000 
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_H263P_UMV;		// Annex D+ 
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_AC_PRED;			// Annex I and T 
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_LOOP_FILTER;		// Annex J 
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_H263P_SLICE_STRUCT;	// Annex K 
			pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_H263P_AIV;			// Annex S 
			break;
		}
	}

	if (avcodec_open(pXVCodec->m_pAVCodecContext,pXVCodec->m_pAVCodec)<0) 
	{
		if(pXVCodec->m_pAVCodecContext!=NULL)
		{
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

		return NULL;
	}

	return pXVCodec;
}

VIDEC_264_API void VIDEC264_EncoderClose(void*pHandle)
{
	PXVCodec pXVCodec=(PXVCodec)pHandle;

	if (pXVCodec)
	{
		if(pXVCodec->m_pAVCodecContext!=NULL)
		{
			if (pXVCodec->m_pAVCodecContext->codec != NULL)
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

VIDEC_264_API int VIDEC264_EncoderEncode(void*pHandle,unsigned char*pYV12[],unsigned char*pStreamBuf,int nBufLen,int* pbKeyFrame)
{
	int nRet=0;
	PXVCodec pXVCodec=(PXVCodec)pHandle;
	if (pXVCodec)
	{
		pXVCodec->m_pAVFrame->data[0]=pYV12[0];
		pXVCodec->m_pAVFrame->data[1]=pYV12[1];
		pXVCodec->m_pAVFrame->data[2]=pYV12[2];

		pXVCodec->m_pAVFrame->pict_type=0;
		if (*pbKeyFrame)
		{
			pXVCodec->m_pAVFrame->pict_type=FF_I_TYPE;
		}
		pXVCodec->m_pAVFrame->pts = AV_NOPTS_VALUE;
		pXVCodec->m_pAVFrame->quality = pXVCodec->m_pAVCodecContext->global_quality;

		int nHeaderLen=0;
		if (pXVCodec->m_nCodecType==VIDEC_H264_CODEC_H264 && *pbKeyFrame)
		{
			if (pXVCodec->m_pAVCodecContext->extradata && pXVCodec->m_pAVCodecContext->extradata_size>0)
			{
				nHeaderLen=pXVCodec->m_pAVCodecContext->extradata_size;
				memcpy(pStreamBuf,pXVCodec->m_pAVCodecContext->extradata,pXVCodec->m_pAVCodecContext->extradata_size);
			}
		}

		nRet=avcodec_encode_video(pXVCodec->m_pAVCodecContext,pStreamBuf+nHeaderLen, nBufLen-nHeaderLen, pXVCodec->m_pAVFrame);
		if (nRet>0)
		{
			nRet+=nHeaderLen;
		}

		*pbKeyFrame=pXVCodec->m_pAVCodecContext->coded_frame->key_frame;
	}

	return nRet;

}


VIDEC_264_API void*VIDEC264_DecoderOpen(VIDEC_H264_CODEC_TYPE nType,int nWidth,int nHeight)
{
	PXVCodec pXVCodec=0;

	enum CodecID codecID=CODEC_ID_H264;
	switch (nType)
	{
	case VIDEC_H264_CODEC_H264:
		codecID=CODEC_ID_H264;
		break;
	case VIDEC_H264_CODEC_MPEG4:
		codecID=CODEC_ID_MPEG4;
		break;
	case VIDEC_H264_CODEC_H263:
	case VIDEC_H264_CODEC_H263P:
	case VIDEC_H264_CODEC_H263PP:
		codecID=CODEC_ID_H263;
		break;
	case VIDEC_H264_CODEC_VP8:
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


	pXVCodec->m_pAVCodec=avcodec_find_decoder(codecID);
	if (pXVCodec->m_pAVCodec==NULL)
	{
		VIDEC264_DecoderClose(pXVCodec);

		return NULL;
	}

	pXVCodec->m_pAVCodecContext=avcodec_alloc_context();
	pXVCodec->m_pAVFrame=avcodec_alloc_frame();
	if (pXVCodec->m_pAVCodecContext==NULL || pXVCodec->m_pAVFrame==NULL)
	{
		VIDEC264_DecoderClose(pXVCodec);

		return NULL;
	}
	pXVCodec->m_pAVCodecContext->pix_fmt = PIX_FMT_YUV420P;
	pXVCodec->m_pAVCodecContext->flags2 |= CODEC_FLAG2_FAST;

	pXVCodec->m_pAVCodecContext->width =nWidth;
	pXVCodec->m_pAVCodecContext->height=nHeight;

	avcodec_get_frame_defaults(pXVCodec->m_pAVFrame);

	if (codecID==CODEC_ID_H264)
	{
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
			avcodec_thread_init(pXVCodec->m_pAVCodecContext,nThreadCount);
		}
	}

	pXVCodec->m_pAVCodecContext->error_recognition=0;//解决RTP丢包，解码会崩溃的错

	if (avcodec_open(pXVCodec->m_pAVCodecContext,pXVCodec->m_pAVCodec)<0) 
	{
		VIDEC264_DecoderClose(pXVCodec);

		return NULL;
	}

	return pXVCodec;
}

VIDEC_264_API void VIDEC264_DecoderClose(void*pHandle)
{
	PXVCodec pXVCodec=(PXVCodec)pHandle;

	if (pXVCodec)
	{
		if (pXVCodec->m_pAVCodecContext)
		{
			//if (pXVCodec->m_pAVCodecContext->thread_count>1)
			//{
			//	avcodec_thread_free(pXVCodec->m_pAVCodecContext);
			//}

			if (pXVCodec->m_pAVCodecContext->codec) 
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

VIDEC_264_API int  VIDEC264_DecoderDecode(void*pHandle,unsigned char*pStreamData,int nStreamLen,unsigned char*pYV12[],int nStride[],int*pnWidth,int*pnHeight,int*pnUsedBytes,int*pnKeyFrame)
{
	int nGotPicture=0;
	int nUsedBytes=0;
	PXVCodec pXVCodec=(PXVCodec)pHandle;
	if (pXVCodec)
	{
		AVPacket tAVPacket;
		av_init_packet(&tAVPacket);
		tAVPacket.data=pStreamData;
		tAVPacket.size=nStreamLen;

		nUsedBytes=avcodec_decode_video2(pXVCodec->m_pAVCodecContext,pXVCodec->m_pAVFrame,&nGotPicture,&tAVPacket);
		if (nUsedBytes<0)
			nUsedBytes=nStreamLen;

		*pnUsedBytes=nUsedBytes;
		*pnWidth=pXVCodec->m_pAVCodecContext->width;
		*pnHeight=pXVCodec->m_pAVCodecContext->height;

		//char szDebug[1024];
		//sprintf(szDebug,"nLen=%d nUsedBytes=%d nGotPicture=%d\n",nStreamLen,nUsedBytes,nGotPicture);
		//OutputDebugStringA(szDebug);

		if (nGotPicture)
		{
			*pnKeyFrame=(pXVCodec->m_pAVFrame->key_frame);// || (pXVCodec->m_pAVFrame->pict_type==FF_I_TYPE));

			pYV12[0]=pXVCodec->m_pAVFrame->data[0];
			pYV12[1]=pXVCodec->m_pAVFrame->data[1];
			pYV12[2]=pXVCodec->m_pAVFrame->data[2];

			nStride[0]=pXVCodec->m_pAVFrame->linesize[0];
			nStride[1]=pXVCodec->m_pAVFrame->linesize[1];
			nStride[2]=pXVCodec->m_pAVFrame->linesize[2];

			//avpicture_layout((AVPicture *)pXVCodec->m_pAVFrame, pXVCodec->m_pAVCodecContext->pix_fmt, pXVCodec->m_pAVCodecContext->width, pXVCodec->m_pAVCodecContext->height,
			//	*out_data, retsize);

		}
	}

	return nGotPicture;
}

extern "C" int pthread_win32_process_attach_np(void);
extern "C" int pthread_win32_process_detach_np(void);
extern "C" int pthread_win32_thread_attach_np(void);
extern "C" int pthread_win32_thread_detach_np(void);

static int g_nInitCount=0;
void VIDEC264_Initialize(void)
{
	if (g_nInitCount==0)
	{
		//pthread_win32_process_attach_np();
		//pthread_win32_thread_attach_np();

		avcodec_init();
		avcodec_register_all();
	}
	g_nInitCount++;
}

void VIDEC264_UnInitialize(void)
{
	g_nInitCount--;
	if (g_nInitCount<=0)
	{
		g_nInitCount=0;
		//av_free_static();

		//pthread_win32_thread_detach_np ();
		//pthread_win32_process_detach_np ();
	}
}
