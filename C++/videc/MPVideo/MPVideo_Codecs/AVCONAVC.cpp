// AVCONAVC.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <AVCONAVC/AVCONAVC.h>
#include <mmsystem.h>


#define _STDINT_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavutil/mem.h>
#include <libavcodec/avcodec.h>
#include <x264/x264.h>
#ifdef __cplusplus
}
#endif

typedef struct tagX264ENC
{
	tagX264ENC(void)
		: enc(NULL)
	{
	}

	x264_param_t	params;
    x264_t*			enc;
}X264ENC,*PX264ENC;

AVCONAVC_API void*AVCONAVC_EncoderOpen(int nWidth,int nHeight,int nFrameRate,int nProfile,int nMaxRTPPayloadSize,int nThreadCount,int nBitrate,int nQMin,int nQMax,int nKeyIntMax)
{
	PX264ENC pX264Enc=new X264ENC;

	x264_param_default(&pX264Enc->params);

	pX264Enc->params.i_threads			= nThreadCount;
	pX264Enc->params.b_sliced_threads	= 1;
	pX264Enc->params.i_width			= nWidth;
	pX264Enc->params.i_height			= nHeight;
	pX264Enc->params.i_sync_lookahead	= 0;


	pX264Enc->params.i_fps_num			= nFrameRate;
	pX264Enc->params.i_fps_den			= 1;
	pX264Enc->params.i_csp				= X264_CSP_I420;

	pX264Enc->params.i_timebase_num		= 1;
	pX264Enc->params.i_timebase_den		= 1000;;

	pX264Enc->params.i_slice_max_size	= nMaxRTPPayloadSize;

	if (nWidth<=352)
	{
		pX264Enc->params.i_level_idc=20;
	}
	else if (nWidth<=1280)
	{
		pX264Enc->params.i_level_idc=31;
	}
	else
	{
		pX264Enc->params.i_level_idc=41;
	}

	pX264Enc->params.i_frame_total		= 0;
	pX264Enc->params.i_frame_reference	= 1;	//可以修改
	pX264Enc->params.i_bframe_pyramid	= 0;
	pX264Enc->params.b_intra_refresh		= 0;

	pX264Enc->params.i_keyint_max			= nKeyIntMax;
	pX264Enc->params.i_keyint_min			= 1;

	pX264Enc->params.b_interlaced			= 0;
	pX264Enc->params.i_cqm_preset			= X264_CQM_FLAT;
	pX264Enc->params.b_aud					= 0;
	pX264Enc->params.b_repeat_headers		= 1;
	pX264Enc->params.rc.i_aq_mode			= X264_AQ_AUTOVARIANCE;
	pX264Enc->params.rc.b_mb_tree			= 0;
	pX264Enc->params.rc.i_lookahead			= 0;

	pX264Enc->params.i_bframe_adaptive		= 0;


	// high definition might benefit from this
	if (nWidth >= 1280) 
	{
		pX264Enc->params.analyse.i_me_range		= 24;
	}

	pX264Enc->params.analyse.i_me_method		= X264_ME_HEX;

	pX264Enc->params.analyse.intra = X264_ANALYSE_I4x4 | X264_ANALYSE_I8x8; 
	pX264Enc->params.analyse.inter = X264_ANALYSE_I4x4 | X264_ANALYSE_I8x8 | 
								   X264_ANALYSE_PSUB16x16 | X264_ANALYSE_BSUB16x16 | X264_ANALYSE_PSUB8x8; 

	if (nQMax==51)
	{
		pX264Enc->params.rc.i_rc_method = X264_RC_CRF;
	}
	else
	{
		if (nQMax==50)
			nQMax=51;

		pX264Enc->params.rc.i_rc_method = X264_RC_ABR;
	}

	// custom QPs
	pX264Enc->params.rc.i_qp_min			= nQMin;
	pX264Enc->params.rc.i_qp_max			= nQMax;
	pX264Enc->params.rc.i_qp_constant		= nQMin;
	pX264Enc->params.rc.f_rf_constant		= nQMin;		// use for CRF as well

	pX264Enc->params.rc.i_bitrate				= nBitrate;
	pX264Enc->params.rc.i_vbv_max_bitrate		= nBitrate;
	pX264Enc->params.rc.i_vbv_buffer_size		= nBitrate;

	pX264Enc->params.i_sps_id					= 0;					// make sure !!!
	pX264Enc->params.i_log_level				= X264_LOG_NONE;
	pX264Enc->params.i_sync_lookahead			= 0;

	pX264Enc->params.b_deblocking_filter			= 1;
    pX264Enc->params.i_deblocking_filter_alphac0 = -2;
    pX264Enc->params.i_deblocking_filter_beta = -2;
    pX264Enc->params.analyse.f_psy_trellis = 0.25;
    pX264Enc->params.analyse.b_dct_decimate = 0;
    pX264Enc->params.rc.f_pb_factor = 1.1;
    pX264Enc->params.rc.f_ip_factor = 1.1;
    pX264Enc->params.rc.f_aq_strength = 0.5;
    pX264Enc->params.analyse.i_luma_deadzone[0] = 6;
    pX264Enc->params.analyse.i_luma_deadzone[1] = 6;
    pX264Enc->params.rc.f_qcompress = 0.8;

    pX264Enc->params.i_frame_reference = 1;
    pX264Enc->params.i_scenecut_threshold = 0;
    pX264Enc->params.b_deblocking_filter = 1;
    pX264Enc->params.i_deblocking_filter_alphac0 = 0;
    pX264Enc->params.i_deblocking_filter_beta = 0;
    pX264Enc->params.b_cabac = (nProfile==AVC_BASELINE_PROFILE?0:1);
	pX264Enc->params.i_bframe = (nProfile==AVC_BASELINE_PROFILE?0:0);
    pX264Enc->params.analyse.b_transform_8x8 = (nProfile==AVC_BASELINE_PROFILE?0:1);
    pX264Enc->params.analyse.i_me_method = X264_ME_HEX;
    pX264Enc->params.analyse.i_subpel_refine = 1;//数字大，耗CPU

    pX264Enc->params.rc.i_aq_mode = 0;
    pX264Enc->params.analyse.b_mixed_references = 0;
    pX264Enc->params.analyse.i_trellis = 0;
    pX264Enc->params.i_bframe_adaptive = X264_B_ADAPT_NONE;
    pX264Enc->params.rc.b_mb_tree = 0;
    pX264Enc->params.analyse.i_weighted_pred = X264_WEIGHTP_SIMPLE;
	pX264Enc->params.analyse.b_fast_pskip = 0;

	//"zerolatency"
	pX264Enc->params.rc.i_lookahead = 0;
	pX264Enc->params.i_sync_lookahead = 0;
	pX264Enc->params.i_bframe = 0;
	pX264Enc->params.b_sliced_threads = 1;
	pX264Enc->params.b_vfr_input = 0;
	pX264Enc->params.rc.b_mb_tree = 0;

	pX264Enc->params.rc.b_mb_tree = 1;//会降低码流，对相对静态的图像效果比较明显

	if (nProfile==AVC_BASELINE_PROFILE)
	{
		x264_param_apply_profile(&pX264Enc->params,"baseline");
	}

    if( ( pX264Enc->enc = x264_encoder_open( &pX264Enc->params ) ) == NULL )
    {
		delete pX264Enc;
		pX264Enc=NULL;
		return NULL;
	}

	return pX264Enc;
}

AVCONAVC_API void AVCONAVC_EncoderClose(void*pHandle)
{
	PX264ENC pX264Enc=(PX264ENC)pHandle;

	if (pX264Enc!=NULL)
	{
		if (pX264Enc->enc!=NULL)
		{
			// read out delayed frames
			x264_picture_t		picout;
			x264_nal_t			*nals;
			int					nalcnt, size = 0;
			do {
				size = x264_encoder_encode(pX264Enc->enc, &nals, &nalcnt, NULL, &picout);
			} while (size > 0);

			x264_encoder_close( pX264Enc->enc );
			pX264Enc->enc=NULL;
		}

		delete pX264Enc;
		pX264Enc=NULL;
	}
}

AVCONAVC_API int  AVCONAVC_EncoderEncode(void*pHandle,unsigned char*pYV12[],unsigned char*pStreamBuf,int nBufLen,int* pnKeyFrame,int* pnBFrame,int*pCurQP)
{
	PX264ENC pX264Enc=(PX264ENC)pHandle;

    x264_nal_t *nal;
    int nnal, i;

	x264_picture_t		pic_in, pic_out;
	memset(&pic_in, 0, sizeof(pic_in));
	memset(&pic_out, 0, sizeof(pic_out));

    for(i = 0; i < 3; i++)
	{
        pic_in.img.plane[i] = pYV12[i];
    }

	pic_in.i_pts			= timeGetTime();
	pic_in.i_type			= X264_TYPE_AUTO;
	pic_in.i_qpplus1		= 0;
	pic_in.img.i_csp		= X264_CSP_I420;
	pic_in.img.i_plane		= 3;

	pic_in.img.i_stride[0]=pX264Enc->params.i_width;
	pic_in.img.i_stride[1]=pX264Enc->params.i_width>>1;
	pic_in.img.i_stride[2]=pX264Enc->params.i_width>>1;

	if (*pnKeyFrame)
	{
		pic_in.i_type = *pnKeyFrame;//X264_TYPE_IDR;//X264_TYPE_I;
	}
 
	nBufLen=x264_encoder_encode(pX264Enc->enc, &nal, &nnal, &pic_in,&pic_out);

	//char szDebug[1024];
	//sprintf(szDebug,"%d\n",nBufLen);
	//OutputDebugStringA(szDebug);

    if (nBufLen<0)
        return -1;

	*pnKeyFrame=0;
	*pnBFrame=0;
	if (nBufLen==0)
	{
		return 0;
	}

	memcpy(pStreamBuf,nal->p_payload,nBufLen);


	if (pCurQP!=NULL)
	{
		*pCurQP=pic_out.i_qpplus1-1;
	}

 
	*pnKeyFrame=0;
	*pnBFrame=0;
    switch(pic_out.i_type){
    case X264_TYPE_IDR:
    case X264_TYPE_I:
		{
			*pnKeyFrame=-1;
		}
        break;
    case X264_TYPE_P:
    case X264_TYPE_BREF:
        *pnKeyFrame=0;
        break;
    case X264_TYPE_B:
        *pnKeyFrame=0;
		*pnBFrame=1;
        break;
	default:
		return 0;
    }

	return nBufLen;
}


typedef struct tagXVCodec
{
    AVCodec*				m_pAVCodec;
    AVCodecContext*			m_pAVCodecContext;
    AVFrame*				m_pAVFrame;
}XVCodec,*PXVCodec;

AVCONAVC_API void*AVCONAVC_DecoderOpen(int nWidth,int nHeight,int AVCCodecID)
{
	PXVCodec pXVCodec=0;

	enum CodecID codecID=CODEC_ID_H264;
	switch (AVCCodecID)
	{
	case AVC_CODEC_H263:
		codecID=CODEC_ID_H263;
		break;
	case AVC_CODEC_H264:
		codecID=CODEC_ID_H264;
		break;
	case AVC_CODEC_MPEG4:
		codecID=CODEC_ID_MPEG4;
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

	avcodec_register_all();

	pXVCodec->m_pAVCodec=avcodec_find_decoder(codecID);
	if (pXVCodec->m_pAVCodec==NULL)
	{
		AVCONAVC_DecoderClose(pXVCodec);

		return NULL;
	}

	pXVCodec->m_pAVCodecContext=avcodec_alloc_context3(pXVCodec->m_pAVCodec);
	pXVCodec->m_pAVFrame=avcodec_alloc_frame();
	if (pXVCodec->m_pAVCodecContext==NULL || pXVCodec->m_pAVFrame==NULL)
	{
		AVCONAVC_DecoderClose(pXVCodec);

		return NULL;
	}

	pXVCodec->m_pAVCodecContext->width =nWidth;
	pXVCodec->m_pAVCodecContext->height=nHeight;

	pXVCodec->m_pAVCodecContext->codec_id              = codecID;
	if (codecID==CODEC_ID_H264)
	{
		pXVCodec->m_pAVCodecContext->codec_tag             = MAKEFOURCC('A','V','C','1');
	}

	pXVCodec->m_pAVCodecContext->coded_width           = nWidth;
	pXVCodec->m_pAVCodecContext->coded_height          = nHeight;
	pXVCodec->m_pAVCodecContext->bits_per_coded_sample = 12;
	pXVCodec->m_pAVCodecContext->error_concealment     = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
	pXVCodec->m_pAVCodecContext->err_recognition       = AV_EF_CAREFUL;
	pXVCodec->m_pAVCodecContext->workaround_bugs       = FF_BUG_AUTODETECT;

	if (codecID == CODEC_ID_H264)
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
		if (codecID==CODEC_ID_H264)
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
		AVCONAVC_DecoderClose(pXVCodec);

		return NULL;
	}

	return pXVCodec;
}

AVCONAVC_API void AVCONAVC_DecoderClose(void*pHandle)
{
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

AVCONAVC_API int  AVCONAVC_DecoderDecode(void*pHandle,unsigned char*pStreamData,int nStreamLen,unsigned char*pYV12[],int nStride[],int*pnWidth,int*pnHeight,int*pnUsedBytes,int*pnKeyFrame,char**ppqscale_table,int*pqstride)
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


void AVCONAVC_Initialize(void)
{

}

void AVCONAVC_UnInitialize(void)
{

}
