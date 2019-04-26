#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "../inc/avc.h"
 
#include "avcodec.h"
#include "mem.h"


#define round(x)	((int)((x)+0.5))
#define min(x,y)	((x>y)?y:x)

#define PAYLOAD_LEN 988//1024-20IP-8TCP-8HEADER

typedef struct tagXVCodec
{
    AVCodec*				m_pAVCodec;
    AVCodecContext*			m_pAVCodecContext;
    AVFrame*				m_pAVFrame;
}XVCodec,*PXVCodec;

static int g_nInitCount=0;
void avc_init(void)
{
	if (g_nInitCount==0)
	{
		avcodec_init();
		avcodec_register_all();
	}
	g_nInitCount++;
}

void avc_uninit(void)
{
	g_nInitCount--;
	if (g_nInitCount<=0)
	{
		g_nInitCount=0;
		av_free_static();
	}
}


void*avc_encoder_open(AVC_CODEC_TYPE codecType,int nWidth,int nHeight,int nMaxRTPFrameSize,H263_RTP_CALLBACK H263RTPCallback,void*pUserData)
{
	PXVCodec pXVCodec=0;
	enum CodecID codecID=CODEC_ID_H263;


	switch (codecType)
	{
	case AVC_CODEC_H263:
		codecID=CODEC_ID_H263;
		break;
	case AVC_CODEC_H263P:
		codecID=CODEC_ID_H263P;
		break;
	case AVC_CODEC_MJPEG:
		codecID=CODEC_ID_MJPEG;
		break;
	default:

		return 0;
	}

	pXVCodec=(PXVCodec)av_malloc(sizeof(XVCodec));
	memset(pXVCodec,0,sizeof(XVCodec));
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
	
	pXVCodec->m_pAVCodecContext->codec = NULL;
	pXVCodec->m_pAVCodecContext->mb_decision = FF_MB_DECISION_SIMPLE; // choose only one MB type at a time
	pXVCodec->m_pAVCodecContext->me_method = ME_EPZS;

	pXVCodec->m_pAVCodecContext->max_b_frames = 0;
	pXVCodec->m_pAVCodecContext->pix_fmt = PIX_FMT_YUV420P;
	if (codecID==CODEC_ID_MJPEG)
		pXVCodec->m_pAVCodecContext->pix_fmt = PIX_FMT_YUVJ420P;

	// X-Lite does not like Custom Picture frequency clocks...
	pXVCodec->m_pAVCodecContext->time_base.num = 100; 
	pXVCodec->m_pAVCodecContext->time_base.den = 2997;
	//pXVCodec->m_pAVCodecContext->time_base.num = 1; 
	//pXVCodec->m_pAVCodecContext->time_base.den = nFrameRate;

	pXVCodec->m_pAVCodecContext->gop_size=300;
	//pXVCodec->m_pAVCodecContext->gop_size=nFrameRate*nKeyFrameInterval;

	//  pXVCodec->m_pAVCodecContext->flags = 0;
	// avoid copying input/output
	pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_INPUT_PRESERVED; // we guarantee to preserve input for max_b_frames+1 frames
	pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_EMU_EDGE;        // don't draw edges
	pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_PASS1;

	pXVCodec->m_pAVCodecContext->width  = nWidth;
	pXVCodec->m_pAVCodecContext->height = nHeight;

	pXVCodec->m_pAVFrame->linesize[0] = nWidth;
	pXVCodec->m_pAVFrame->linesize[1] = nWidth / 2;
	pXVCodec->m_pAVFrame->linesize[2] = nWidth / 2;

	//set target bitrate
	//{
	//	pXVCodec->m_pAVCodecContext->bit_rate = (nBitrate * 3) >> 2;			// average bit rate
	//	pXVCodec->m_pAVCodecContext->bit_rate_tolerance = nBitrate >> 1;
	//	pXVCodec->m_pAVCodecContext->rc_min_rate = 0;								// minimum bitrate
	//	pXVCodec->m_pAVCodecContext->rc_max_rate = nBitrate;					// maximum bitrate

	//	/* ratecontrol qmin qmax limiting method
	//	0-> clipping, 1-> use a nice continous function to limit qscale wthin qmin/qmax.
	//	*/

	//	pXVCodec->m_pAVCodecContext->rc_qsquish = 0;								// limit q by clipping 
	//	pXVCodec->m_pAVCodecContext->rc_eq = (char*) "1";							// rate control equation
	//	pXVCodec->m_pAVCodecContext->rc_buffer_size = nBitrate*64;

	//	pXVCodec->m_pAVCodecContext->rc_initial_buffer_occupancy=(pXVCodec->m_pAVCodecContext->rc_buffer_size*3)>>2;
	//	pXVCodec->m_pAVCodecContext->rc_buffer_aggressivity= (float)1.0;
	//}


	if (codecID==CODEC_ID_H263P)
	{
		// Annex D: Unrestructed Motion Vectors
		// Level 2+ 
		// works with eyeBeam, signaled via  non-standard "D"
		// fix me,crashed
		//--pXVCodec->m_pAVCodecContext->flags|=CODEC_FLAG_H263P_UMV;

		// Annex F: Advanced Prediction Mode
		// does not work with eyeBeam
		//pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_OBMC; 

		// Annex I: Advanced Intra Coding
		// Level 3+
		// works with eyeBeam
		// fix me,crashed
		//--pXVCodec->m_pAVCodecContext->flags|=CODEC_FLAG_AC_PRED;

		// Annex K: 
		// does not work with eyeBeam
		//pXVCodec->m_pAVCodecContext->flags|=CODEC_FLAG_H263P_SLICE_STRUCT;

		// Annex J: Deblocking Filter
		// works with eyeBeam
		// fix me,crashed
		//--pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_LOOP_FILTER;

		// Annex S: Alternative INTER VLC mode
		// does not work with eyeBeam
		//pXVCodec->m_pAVCodecContext->flags |= CODEC_FLAG_H263P_AIV;
	}

	//set temporal spatial trade off
	//{
	//	int nTSTO=31;
	//	int nMinQuant=2;

	//	pXVCodec->m_pAVFrame->quality = nMinQuant;

	//	pXVCodec->m_pAVCodecContext->max_qdiff = 6;//3; // max q difference between frames
	//	pXVCodec->m_pAVCodecContext->qcompress = 0.5; // qscale factor between easy & hard scenes (0.0-1.0)
	//	pXVCodec->m_pAVCodecContext->i_quant_factor = (float)-0.6; // qscale factor between p and i frames
	//	pXVCodec->m_pAVCodecContext->i_quant_offset = (float)0.0; // qscale offset between p and i frames
	//	pXVCodec->m_pAVCodecContext->me_subpel_quality = 8;

	//	pXVCodec->m_pAVCodecContext->qmin = nMinQuant;
	//	pXVCodec->m_pAVCodecContext->qmax = round ( (double)(31 - nMinQuant) / 31 * nTSTO + nMinQuant);
	//	pXVCodec->m_pAVCodecContext->qmax = min( pXVCodec->m_pAVCodecContext->qmax, 31);

	//	pXVCodec->m_pAVCodecContext->mb_qmin = pXVCodec->m_pAVCodecContext->qmin;
	//	pXVCodec->m_pAVCodecContext->mb_qmax = pXVCodec->m_pAVCodecContext->qmax;

	//	// Lagrange multipliers - this is how the context defaults do it:
	//	pXVCodec->m_pAVCodecContext->lmin = pXVCodec->m_pAVCodecContext->qmin * FF_QP2LAMBDA;
	//	pXVCodec->m_pAVCodecContext->lmax = pXVCodec->m_pAVCodecContext->qmax * FF_QP2LAMBDA; 

	//}

	if (pUserData && H263RTPCallback)
	{
		pXVCodec->m_pAVCodecContext->user_data = pUserData;
		pXVCodec->m_pAVCodecContext->rtp_payload_size = nMaxRTPFrameSize>PAYLOAD_LEN?PAYLOAD_LEN:nMaxRTPFrameSize;
		pXVCodec->m_pAVCodecContext->rtp_callback = H263RTPCallback;
	}
	else
	{
		pXVCodec->m_pAVCodecContext->user_data = 0;
		pXVCodec->m_pAVCodecContext->rtp_payload_size = 0;
		pXVCodec->m_pAVCodecContext->rtp_callback = 0;
	}

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

void avc_encoder_close(void*pHandle)
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

int avc_encoder_encode(void*pHandle,unsigned char*pYV12[],unsigned char*pStreamBuf,int nBufLen,int nQuant,int* pbKeyFrame)
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
		pXVCodec->m_pAVCodecContext->qscale=nQuant;

		nRet=avcodec_encode_video(pXVCodec->m_pAVCodecContext,pStreamBuf, nBufLen, pXVCodec->m_pAVFrame);

		*pbKeyFrame=pXVCodec->m_pAVCodecContext->coded_frame->key_frame;
	}

	return nRet;
}

void*avc_decoder_open(AVC_CODEC_TYPE codecType,int nWidth,int nHeight)
{
	PXVCodec pXVCodec=0;

	enum CodecID codecID=CODEC_ID_H263;


	switch (codecType)
	{
	case AVC_CODEC_H263:
	case AVC_CODEC_H263P:
		codecID=CODEC_ID_H263;
		break;
	case AVC_CODEC_H264:
		codecID=CODEC_ID_H264;
		break;
	case AVC_CODEC_MJPEG:
		codecID=CODEC_ID_MJPEG;
		break;
	default:
		return 0;
	}

	pXVCodec=(PXVCodec)av_malloc(sizeof(XVCodec));
	if (!pXVCodec)
	{
		return NULL;
	}

	pXVCodec->m_pAVCodec=avcodec_find_decoder(codecID);
	if (pXVCodec->m_pAVCodec==NULL)
	{
		avc_decoder_close(pXVCodec);

		return NULL;
	}

	pXVCodec->m_pAVCodecContext=avcodec_alloc_context();
	pXVCodec->m_pAVFrame=avcodec_alloc_frame();
	if (pXVCodec->m_pAVCodecContext==NULL || pXVCodec->m_pAVFrame==NULL)
	{
		avc_decoder_close(pXVCodec);

		return NULL;
	}

	pXVCodec->m_pAVCodecContext->width =nWidth;
	pXVCodec->m_pAVCodecContext->height=nHeight;

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
		avc_decoder_close(pXVCodec);

		return NULL;
	}

	return pXVCodec;
}

void avc_decoder_close(void*pHandle)
{
	PXVCodec pXVCodec=(PXVCodec)pHandle;

	if (pXVCodec)
	{
		if (pXVCodec->m_pAVCodecContext)
		{
			if (pXVCodec->m_pAVCodecContext->thread_count>1)
			{
				avcodec_thread_free(pXVCodec->m_pAVCodecContext);
			}

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

int avc_decoder_decode(void*pHandle,unsigned char*pStreamData,int nStreamLen,unsigned char*pYV12[],int stride[],char**ppqscale_table,int*pqstride,int*pnWidth,int*pnHeight,int*pnUsedBytes,int*pnKeyFrame)
{
	int nGotPicture=0;
	int nUsedBytes=0;
	PXVCodec pXVCodec=(PXVCodec)pHandle;
	if (pXVCodec)
	{
		nUsedBytes=avcodec_decode_video(pXVCodec->m_pAVCodecContext,pXVCodec->m_pAVFrame,&nGotPicture,(unsigned char*)pStreamData,nStreamLen);
		if (nUsedBytes<0)
			nUsedBytes=nStreamLen;
		
		*pnUsedBytes=nUsedBytes;
	    *pnWidth=pXVCodec->m_pAVCodecContext->width;
		*pnHeight=pXVCodec->m_pAVCodecContext->height;

		if (nGotPicture)
		{
			*ppqscale_table=(char*)pXVCodec->m_pAVFrame->qscale_table;
			*pqstride=pXVCodec->m_pAVFrame->qstride;

			*pnKeyFrame=(pXVCodec->m_pAVFrame->key_frame);// || (pXVCodec->m_pAVFrame->pict_type==FF_I_TYPE));

			pYV12[0]=pXVCodec->m_pAVFrame->data[0];
			pYV12[1]=pXVCodec->m_pAVFrame->data[1];
			pYV12[2]=pXVCodec->m_pAVFrame->data[2];

			stride[0]=pXVCodec->m_pAVFrame->linesize[0];
			stride[1]=pXVCodec->m_pAVFrame->linesize[1];
			stride[2]=pXVCodec->m_pAVFrame->linesize[2];
		}
	}

	return nGotPicture;
}
