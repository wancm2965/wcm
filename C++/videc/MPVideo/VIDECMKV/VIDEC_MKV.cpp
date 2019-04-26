#include <windows.h>
#include "VIDEC_MKV.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "avconmkvlib.h"
#include "avconcodec.h"
#include "mathematics.h"
#include "avio.h"
#include "intreadwrite.h"
#include "avc.h"

#ifndef AV_RB24
#   define AV_RB24(x)                           \
	((((const uint8_t*)(x))[0] << 16) |         \
	(((const uint8_t*)(x))[1] <<  8) |         \
	((const uint8_t*)(x))[2])
#endif


	typedef struct _mkv_rwaudivideo_ctx{
		AVFormatContext *oc;
		AVOutputFormat *fmt;
		AVStream *audio_st;
		AVStream *video_st;
		int				nFlagGotStartTimestamp;
		unsigned long	nStartTimestamp;

		int				nFlagWriteHeader;
		int				nFlagWriteAudioHeader;
		int				nFlagWriteVideoHeader;
	}mkv_rwaudivideo_ctx;


#define avcon_new(type,count)	(type*)malloc(sizeof(type)*(count))

	static mkv_rwaudivideo_ctx* avcon_ctxlist_new(const char * mkvfile)
	{
		mkv_rwaudivideo_ctx *ctxlist;
		ctxlist = avcon_new(mkv_rwaudivideo_ctx,1);
		memset(ctxlist,0x0,sizeof(mkv_rwaudivideo_ctx));
		return ctxlist;
	}

	static AVStream *add_audio_stream(mkv_rwaudivideo_ctx *listitem)
	{
		AVCodecContext *c;
		AVStream *st;

		st = av_new_stream(listitem->oc, 1);
		if (!st) {
			return NULL;
		}

		c = st->codec;
		c->codec_id = (enum CodecID)listitem->fmt->audio_codec;
		c->codec_type = CODEC_TYPE_AUDIO;
		c->bit_rate = 32000;
		c->sample_rate =32000;
		c->channels = 2;
		c->bits_per_sample = 16;
		c->codec_id = CODEC_ID_MP3LAME;

		return st;
	}
	/* add a video output stream */
	static AVStream *add_video_stream(mkv_rwaudivideo_ctx *listitem)
	{
		AVCodecContext *c;
		AVStream *st;

		st = av_new_stream(listitem->oc, 0);
		if (!st) {
			return NULL;
		}

		c = st->codec;
		c->codec_id = (enum CodecID)listitem->fmt->video_codec;
		c->codec_type = CODEC_TYPE_VIDEO;
		c->width = 704;
		c->height = 576;
		c->time_base.den = 60;
		c->time_base.num = 1;
		c->gop_size = 250;
		c->codec_id = CODEC_ID_H264;
		c->pix_fmt = PIX_FMT_YUV420P;

		return st;
	}


#ifdef __cplusplus
}
#endif

static bool g_VIDEC_MKV_Initialized=false;


VIDEC_MKV_API void* VIDEC_MKV_Open(const char*cszPathName)
{
	mkv_rwaudivideo_ctx *listitem;

	if (!g_VIDEC_MKV_Initialized)
	{
		g_VIDEC_MKV_Initialized=true;
		av_register_output_format(&matroska_muxer);
		register_protocol(&file_protocol);
	}

	listitem=avcon_ctxlist_new(cszPathName);
	if(listitem == NULL)
		return NULL;

	listitem->fmt = guess_format(NULL, cszPathName, NULL);
	if (!listitem->fmt) 
	{
		free(listitem);
		listitem=NULL;
		return NULL;
	}

	listitem->oc = av_alloc_format_context();
	if (!listitem->oc) 
	{
		free(listitem);
		listitem=NULL;
		return NULL;
	}
	listitem->oc->oformat = listitem->fmt;
	_snprintf(listitem->oc->filename, sizeof(listitem->oc->filename), "%s", cszPathName);

	listitem->video_st = add_video_stream(listitem);
	listitem->audio_st = add_audio_stream(listitem);

	if (av_set_parameters(listitem->oc, NULL) < 0) 
	{
		if (listitem->video_st)
		{
			if (listitem->video_st->codec)
			{
				av_free(listitem->video_st->codec);
				listitem->video_st->codec=NULL;
			}
			av_free(listitem->video_st);
			listitem->video_st=NULL;
		}
		if (listitem->audio_st)
		{
			if (listitem->audio_st->codec)
			{
				av_free(listitem->audio_st->codec);
				listitem->audio_st->codec=NULL;
			}
			av_free(listitem->audio_st);
			listitem->audio_st=NULL;
		}
		av_free(listitem->oc);
		listitem->oc=NULL;
		free(listitem);
		listitem=NULL;
		return NULL;
	}


	if (url_fopen(&listitem->oc->pb, cszPathName, URL_WRONLY) < 0) 
	{
		if (listitem->video_st)
		{
			if (listitem->video_st->codec)
			{
				av_free(listitem->video_st->codec);
				listitem->video_st->codec=NULL;
			}
			av_free(listitem->video_st);
			listitem->video_st=NULL;
		}
		if (listitem->audio_st)
		{
			if (listitem->audio_st->codec)
			{
				av_free(listitem->audio_st->codec);
				listitem->audio_st->codec=NULL;
			}
			av_free(listitem->audio_st);
			listitem->audio_st=NULL;
		}
		av_free(listitem->oc);
		listitem->oc=NULL;
		free(listitem);
		listitem=NULL;
		return NULL;
	}

	return listitem;
}

VIDEC_MKV_API void VIDEC_MKV_Close(void*pHandle)
{
	mkv_rwaudivideo_ctx *listitem;

	listitem = (mkv_rwaudivideo_ctx*)pHandle;
	if( listitem == NULL)
		return ;

	av_write_trailer(listitem->oc);


	if (!(listitem->fmt->flags & AVFMT_NOFILE)) {
		/* close the output file */
		url_fclose((ByteIOContext *)&listitem->oc->pb);
	}

	if (listitem->video_st)
	{
		if (listitem->video_st->codec)
		{
			if (listitem->video_st->codec->extradata)
			{
				av_free(listitem->video_st->codec->extradata);
				listitem->video_st->codec->extradata=NULL;
			}
			av_free(listitem->video_st->codec);
			listitem->video_st->codec=NULL;
		}
		av_free(listitem->video_st);
		listitem->video_st=NULL;
	}
	if (listitem->audio_st)
	{
		if (listitem->audio_st->codec)
		{
			av_free(listitem->audio_st->codec);
			listitem->audio_st->codec=NULL;
		}
		av_free(listitem->audio_st);
		listitem->audio_st=NULL;
	}

	av_free(listitem->oc);
	listitem->oc=NULL;
	free(listitem);
	listitem=NULL;
}

VIDEC_MKV_API int VIDEC_MKV_SetAudio(void*pHandle,int nBitrate,int nSamplesRate,int nChannels,int nBitPerSample,AUDIO_TYPE nType)
{
	enum CodecID nCodecID=CODEC_ID_MP3LAME;
	switch (nType)
	{
	case AUDIO_MP3:
		nCodecID=CODEC_ID_MP3LAME;
		break;
	case AUDIO_AAC4:
        nCodecID=CODEC_ID_MPEG4AAC;
		break;
	}

	mkv_rwaudivideo_ctx *listitem;

	listitem = (mkv_rwaudivideo_ctx*)pHandle;
	if( listitem == NULL)
		return 1;

	if (listitem->audio_st==NULL)
		listitem->audio_st = add_audio_stream(listitem);


	listitem->audio_st->codec->bit_rate = nBitrate;
	listitem->audio_st->codec->sample_rate =nSamplesRate;// 44100; 96000, 88200, 64000, 48000, 44100, 32000,24000, 22050, 16000, 12000, 11025, 8000, 7350
	listitem->audio_st->codec->channels = nChannels;
	listitem->audio_st->codec->bits_per_sample = nBitPerSample;
	listitem->audio_st->codec->codec_id = nCodecID;

	return 0;
}

VIDEC_MKV_API int VIDEC_MKV_SetVideo(void*pHandle,int nFrameRate,int nWidth,int nHeight,VIDEO_TYPE nType)
{
	enum CodecID nCodecID=CODEC_ID_H264;
	switch (nType)
	{
	case VIDEO_H264:
		nCodecID=CODEC_ID_H264;
		break;
	case VIDEO_H263:
		nCodecID=CODEC_ID_H263;
		break;
	}

	if (nWidth==0 || nHeight==0)
	{
		nWidth=704;
		nHeight=576;
	}
	
	mkv_rwaudivideo_ctx *listitem;

	listitem = (mkv_rwaudivideo_ctx*)pHandle;
	if( listitem == NULL)
		return 1;

	if (listitem->video_st==NULL)
		listitem->video_st = add_video_stream(listitem);

	listitem->video_st->codec->width = nWidth;
	listitem->video_st->codec->height = nHeight;
	listitem->video_st->codec->time_base.den = nFrameRate;
	listitem->video_st->codec->time_base.num = 1;
	listitem->video_st->codec->gop_size = 250;
	listitem->video_st->codec->codec_id = nCodecID;
	listitem->video_st->codec->pix_fmt = PIX_FMT_YUV420P;

	return 0;
}


VIDEC_MKV_API int VIDEC_MKV_WriteAudio(void*pHandle,const void *pData,int nLen,unsigned long nTimestamp)
{
	mkv_rwaudivideo_ctx *listitem;
	AVPacket pkt;
	av_init_packet(&pkt);

	listitem = (mkv_rwaudivideo_ctx*)pHandle;
	if( listitem == NULL || listitem->audio_st==NULL)
		return -1;

	if (listitem->nFlagWriteHeader==0)
	{
		listitem->nFlagWriteHeader=1;
		listitem->nFlagWriteAudioHeader=1;
		av_write_header(listitem->oc);
	}
	else if (listitem->nFlagWriteAudioHeader==0)
	{
		listitem->nFlagWriteAudioHeader=1;
		av_rewrite_audio_header(listitem->oc,listitem->audio_st->codec->codec_id,listitem->audio_st->codec->sample_rate,listitem->audio_st->codec->bits_per_sample,listitem->audio_st->codec->channels);
	}

	if (listitem->nFlagGotStartTimestamp==0)
	{
		listitem->nFlagGotStartTimestamp=1;
		listitem->nStartTimestamp=nTimestamp;
	}

	if (nTimestamp-listitem->nStartTimestamp>0x7FFFFFFF)
	{
		return -1;
	}

	nTimestamp-=listitem->nStartTimestamp;

	pkt.pts= nTimestamp;
	pkt.size= nLen;
	pkt.flags |= PKT_FLAG_KEY;


	pkt.stream_index= listitem->audio_st->index;
	pkt.data= (uint8_t *)pData;
	av_write_frame(listitem->oc, &pkt);

	return 0;
}

VIDEC_MKV_API int VIDEC_MKV_WriteVideo(void*pHandle,const void *pData,int nLen,unsigned long nTimestamp,int nKeyFrame)
{
	mkv_rwaudivideo_ctx *listitem;
	listitem = (mkv_rwaudivideo_ctx*)pHandle;
	if( listitem == NULL || listitem->video_st==NULL)
	{
		return -1;
	}

	int nOffset=0;

	if (listitem->nFlagWriteHeader==0 || listitem->nFlagWriteVideoHeader==0)
	{
		if (listitem->video_st->codec->codec_id==CODEC_ID_H264)
		{
			uint32_t extra_data_size=0;

			unsigned char*pTempData=(unsigned char*)pData;
			while (nOffset+4<nLen)
			{
				if (AV_RB32(pTempData)==0x00000001 || AV_RB24(pTempData)==0x000001)
				{
					break;
				}
				else
				{
					nOffset++;
					pTempData++;
				}
			}

			get_avc_extra_data_size(pTempData,nLen-nOffset,&extra_data_size);
			if (extra_data_size)
			{
				listitem->video_st->codec->extradata=(uint8_t*)av_malloc(extra_data_size);
				memcpy(listitem->video_st->codec->extradata,pTempData,extra_data_size);
				listitem->video_st->codec->extradata_size=extra_data_size;

				if (listitem->nFlagWriteHeader==0)
				{
					listitem->nFlagWriteHeader=1;
					listitem->nFlagWriteVideoHeader=1;
					av_write_header(listitem->oc);
				}
				else
				{
					listitem->nFlagWriteVideoHeader=1;
					av_rewrite_video_header(listitem->oc,listitem->video_st->codec->height,listitem->video_st->codec->height,listitem->video_st->codec->codec_id,listitem->video_st->codec->extradata,listitem->video_st->codec->extradata_size);
				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if (listitem->nFlagWriteHeader==0)
			{
				listitem->nFlagWriteHeader=1;
				listitem->nFlagWriteVideoHeader=1;
				av_write_header(listitem->oc);
			}
			else
			{
				listitem->nFlagWriteVideoHeader=1;
				av_rewrite_video_header(listitem->oc,listitem->video_st->codec->height,listitem->video_st->codec->height,listitem->video_st->codec->codec_id,NULL,0);
			}
		}
	}


	AVPacket pkt;
	av_init_packet(&pkt);

	if (listitem->nFlagGotStartTimestamp==0)
	{
		listitem->nFlagGotStartTimestamp=1;
		listitem->nStartTimestamp=nTimestamp;
	}

	if (nTimestamp-listitem->nStartTimestamp>0x7FFFFFFF)
	{
		return -1;
	}

	nTimestamp-=listitem->nStartTimestamp;
	pkt.pts= nTimestamp;

	if (nKeyFrame)
		pkt.flags |= PKT_FLAG_KEY;
	pkt.stream_index= listitem->video_st->index;
	pkt.data= ((uint8_t*)pData)+nOffset;
	pkt.size= nLen-nOffset;

	av_write_frame(listitem->oc, &pkt);

	return 0;
}

