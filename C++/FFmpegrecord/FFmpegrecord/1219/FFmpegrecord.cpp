// FFmpegrecord.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "FFmpegrecord.h"
#define BUF_SIZE 4096*500
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000
static int g_nVideoCount = 0;
static bool g_bfirstRead = true;
static int fill_inH264(void *opaque, uint8_t *buf, int buf_size)
{
// 	int n = 1;
// 	return 1;
// 	struct buffer_data *bd = (struct buffer_data *)opaque;
// 	buf_size = FFMIN(buf_size, bd->size);
// 
// 	printf("ptr:%p size:%d\n", bd->ptr, bd->size);
// 
// 	/* copy internal buffer data to buf */
// 	memcpy(buf, bd->ptr, buf_size);
// 	bd->ptr += buf_size;
// 	bd->size -= buf_size;

	g_nVideoCount++;
	SYSTEMTIME currentTime;
	::GetLocalTime(&currentTime);
	CString strOut;
	strOut.Format(_T("[%.2d%.2d%.2d]AudioCount is %d !\n"), currentTime.wHour, currentTime.wMinute, currentTime.wSecond, g_nVideoCount);
	OutputDebugString(strOut);
	buf_size = 0;
	CFFmpegrecord* precord = (CFFmpegrecord*)opaque;
	if (precord)
	{
		SSDATA* pBufferData = (SSDATA*)precord->ReadVideoBuffData();
		if (pBufferData)
		{
			buf_size = pBufferData->nLen;
			memcpy(buf, (uint8_t*)pBufferData->pData, buf_size);
// 			buf_size = pBufferData->GetSize();
// 			memcpy(buf, (uint8_t*)pBufferData->GetBuffer(), buf_size);

			if (pBufferData->pData)
			{
				free(pBufferData->pData);
				pBufferData->pData = NULL;
			}
			delete pBufferData;
			pBufferData = NULL;
		}
	}
	return buf_size;
}

static int fill_inAac(void *opaque, uint8_t *buf, int buf_size)
{

	buf_size = 0;
	CFFmpegrecord* precord = (CFFmpegrecord*)opaque;
	if (precord)
	{
		SSDATA* pBufferData = (SSDATA*)precord->ReadAudioBuffData();
		if (pBufferData)
		{
			buf_size = pBufferData->nLen;
			memcpy(buf, (uint8_t*)pBufferData->pData, buf_size);
			if (pBufferData->pData)
			{
				free(pBufferData->pData);
				pBufferData->pData = NULL;
			}
			delete pBufferData;
			pBufferData = NULL;
		}
	}
	return buf_size;


// 	int nGetSize = 0;
// 	CFFmpegrecord* precord = (CFFmpegrecord*)opaque;
// 	if (precord)
// 	{
// 		if (g_bfirstRead)
// 		{
// 			g_bfirstRead = false;
// 			int nNeedSize = buf_size;
// 			buf_size = 0;
// 			do
// 			{
// 				SSDATA* pBufferData = (SSDATA*)precord->ReadAudioBuffData();
// 				if (pBufferData)
// 				{
// 					nGetSize = pBufferData->nLen;
// 					if (nGetSize > 0)
// 					{
// 						memcpy(buf, (uint8_t*)pBufferData->pData, nGetSize);
// 						if (pBufferData->pData)
// 						{
// 							free(pBufferData->pData);
// 							pBufferData->pData = NULL;
// 						}
// 						delete pBufferData;
// 						pBufferData = NULL;
// 					}
// 					nNeedSize -= nGetSize;
// 					buf += nGetSize;
// 					buf_size += nGetSize;
// 				}
// 			} while (nNeedSize > 300);
// 		}
// 		else
// 		{
// 			SSDATA* pBufferData = (SSDATA*)precord->ReadAudioBuffData();
// 			if (pBufferData)
// 			{
// 				nGetSize = pBufferData->nLen;
// 				if (nGetSize > 0)
// 				{
// 					memcpy(buf, (uint8_t*)pBufferData->pData, nGetSize);
// 					if (pBufferData->pData)
// 					{
// 						free(pBufferData->pData);
// 						pBufferData->pData = NULL;
// 					}
// 					delete pBufferData;
// 					pBufferData = NULL;
// 				}
// 				buf_size = nGetSize;
// 			}
// 		}
// 	}
// 	return buf_size;
}

static int read_data(void *opaque, uint8_t *buf, int buf_size)
{
	return 1;
}

#define STREAM_FRAME_RATE 25
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */




CFFmpegrecord::CFFmpegrecord()
{
	m_pVideoDataBuffer = NULL;
	m_pAudioDataBuffer = NULL;
	m_bWriteBuff = false;
	m_bVideoAllow = false;
	m_bAudioAllow = false;
	m_bhasaudio = true;
	m_bStartThread = false;
	m_bStartThread = false;
#ifdef USE_OTHER_WAY
	m_pOC = NULL;
	m_vi = -1;
	m_ptsInc = 0;
	m_waitkey = 1;
#endif
}

CFFmpegrecord::~CFFmpegrecord()
{

}

int CFFmpegrecord::Open( const char*cszFileName )
{
	m_strFilename = cszFileName;
	m_bWriteBuff = true;
	return 0;
}

//close接口停止缓存数据流等待线程读取结束退出后销毁对象指针
void CFFmpegrecord::Close( void )
{	
	m_bWriteBuff = false;
	m_bStartThread = false;
	m_bVideoAllow = false;
	m_bAudioAllow = false;
	/*m_bhasaudio = true;*/
}

int CFFmpegrecord::WriteVideo(unsigned char*data, unsigned int len, uint32_t nTimestamp, bool bKeyFrame, unsigned int nWidth /*= 640*/, unsigned int nHeight /*= 480*/)
{
	if (m_bWriteBuff)
	{
		if (NULL == m_pVideoDataBuffer)
		{
			m_pVideoDataBuffer = new CSourceData();
		}
		if (m_pVideoDataBuffer)
		{
			m_pVideoDataBuffer->PushData((char*)data, len);
			if (!m_bVideoAllow)
			{
				if (m_pVideoDataBuffer->GetDataSize() > 0)
				{
					m_bVideoAllow = true;
					if (m_bAudioAllow)
					{
						startRecord();
					}
				}
			}
		}
	}
	return 0;
}

int CFFmpegrecord::WriteAudio(unsigned char*data, unsigned int len, uint32_t nTimestamp)
{
	if (m_bWriteBuff)
	{
		if (NULL == m_pAudioDataBuffer)
		{
			m_pAudioDataBuffer = new CSourceData();
		}
		if (m_pAudioDataBuffer)
		{
			int packetLen = len + 7;
			char* pAACData = new char[packetLen];
			if (pAACData)
			{
				unsigned char adtsHeader[7] = { 0 };
				int profile = 2;
				int freqIdx = 4;
				int chanCfg = 2;				
				adtsHeader[0] = 0xFF;
				adtsHeader[1] = 0xF1;
				adtsHeader[2] = ((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2);
				adtsHeader[3] = ((chanCfg & 3) << 6) + (packetLen >> 11);
				adtsHeader[4] = (packetLen & 0x7ff) >> 3;
				adtsHeader[5] = ((packetLen & 0x7) << 5) + 0x1f;
				adtsHeader[6] = 0xFC;
				memcpy(pAACData, adtsHeader, 7);
				memcpy(pAACData + 7, data, len);
				m_pAudioDataBuffer->PushData(pAACData, packetLen);
				if (!m_bAudioAllow)
				{
					if (m_pAudioDataBuffer->GetDataSize() > 0)
					{
						m_bAudioAllow = true;
						if (m_bVideoAllow)
						{
							startRecord();
						}
					}
				}

				delete[] pAACData;
			}
			
		}
	}
	
	return 0;
}

int CFFmpegrecord::startRecord()
{
	if (m_bStartThread)
	{
		return -1;
	}
	m_bStartThread = true;
	StartThread();
	return 0;
}

void CFFmpegrecord::StopRecord()
{
	if (m_pAudioDataBuffer)
	{
		m_pAudioDataBuffer->ClearAllData();
		delete m_pAudioDataBuffer;
		m_pAudioDataBuffer = NULL;
	}
	if (m_pVideoDataBuffer)
	{
		m_pVideoDataBuffer->ClearAllData();
		delete m_pVideoDataBuffer;
		m_pVideoDataBuffer = NULL;
	}
}

void CFFmpegrecord::ThreadProcMain(void)
{
	startffmpeg();
	StopRecord();
	m_bStartThread = false;
	delete this;
}

SSDATA* CFFmpegrecord::ReadAudioBuffData()
{
	SSDATA* pBuffer = NULL;
	if (m_pAudioDataBuffer)
	{
		pBuffer = (SSDATA*)m_pAudioDataBuffer->PopData();
	}
	return pBuffer;
}

SSDATA* CFFmpegrecord::ReadVideoBuffData()
{
	SSDATA* pBuffer = NULL;
	if (m_pVideoDataBuffer)
	{
		pBuffer = (SSDATA*)m_pVideoDataBuffer->PopData();
	}
	return pBuffer;
}

void CFFmpegrecord::startffmpeg()
{
	AVOutputFormat *ofmt = NULL;
	//Input AVFormatContext and Output AVFormatContext
	AVFormatContext *ifmt_ctx_v = NULL, *ifmt_ctx_a = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
	int ret, i;
	int videoindex_v = -1, videoindex_out = -1;
	int audioindex_a = -1, audioindex_out = -1;
	int frame_index = 0;
	int64_t cur_pts_v = 0, cur_pts_a = 0;
	AVDictionary* pAVDictionary = NULL;
	AVInputFormat* ifmt = NULL;
	
	av_register_all();
	//Input
	ifmt_ctx_v = avformat_alloc_context();
	unsigned char * inH264 = (unsigned char *)av_malloc(65535);
	//AVIOContext *vio = avio_alloc_context(inH264, 65535, 0, &bd, fill_inH264, NULL, NULL);

	AVIOContext *vio = avio_alloc_context(inH264, 65535, 0, this, fill_inH264, NULL, NULL);
	ifmt_ctx_v->pb = vio;
	int err = avformat_open_input(&ifmt_ctx_v, "nothing", NULL, NULL);
	if (err != 0)
	{
		printf("\n");
		printf("Call avformat_open_input function failed!\n");
		return;
	}

	if ((ret = avformat_find_stream_info(ifmt_ctx_v, 0)) < 0) {
		printf("Failed to retrieve input stream information");
		goto end;
	}

	//bool bhas_audio = false;

	//test
	//m_bhasaudio = false;
	if (m_bhasaudio)
	{
		ifmt_ctx_a = avformat_alloc_context();
		unsigned char * inAac = (unsigned char *)av_malloc(1024);
		AVIOContext *aio = avio_alloc_context(inAac, 1024, 0, this, fill_inAac, NULL, NULL);
		ifmt_ctx_a->pb = aio;		
		err = avformat_open_input(&ifmt_ctx_a, "", NULL, NULL);
		if (err != 0)
		{
			char* buf = NULL;
			buf = (char*)av_malloc(1024);
			av_strerror(err, buf, 1024);
			printf("Couldn't open file: %d(%s)", err, buf);
			printf("\n");
			printf("Call avformat_open_input function failed!\n");

			m_bhasaudio = false;			
			//return;
		}
		if (m_bhasaudio)
		{
			if ((ret = avformat_find_stream_info(ifmt_ctx_a, 0)) < 0)
			{
				printf("Failed to retrieve input stream information");
				goto end;
			}
		}
		
	}


	//avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", NULL);
	//unsigned char* outbuffer = (unsigned char*)av_malloc(65535);
	//AVIOContext *avio_out = avio_alloc_context(outbuffer, 65535, 0, this, NULL, write_buffer, NULL);


	//ofmt_ctx->pb = avio_out;
	//ofmt_ctx->flags = AVFMT_FLAG_CUSTOM_IO;
	//Output
	avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, m_strFilename.c_str());
	if (!ofmt_ctx)
	{
		printf("Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}

	
	for (i = 0; i < ifmt_ctx_v->nb_streams; i++) 
	{
		//Create output AVStream according to input AVStream
		if (ifmt_ctx_v->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			AVStream *in_stream = ifmt_ctx_v->streams[i];
//  			AVRational rational;
//  			rational.den = 2;
//  			rational.num = 40;
//  			in_stream->r_frame_rate = rational;
			AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
			videoindex_v = i;
			if (!out_stream) 
			{
				printf("Failed allocating output stream\n");
				ret = AVERROR_UNKNOWN;
				goto end;
			}
			videoindex_out = out_stream->index;
// 			AVCodecContext* c;
// 			c = out_stream->codec;
// 			c->bit_rate = 384000;
// 			c->width = 640;
// 			c->height = 480;
// 			c->time_base.num = 2;
// 			c->time_base.den = 40;
			if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) 
			{
				printf("Failed to copy context from input to output stream codec context\n");
				goto end;
			}

			out_stream->codec->codec_tag = 0;
			if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
				out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
			break;
		}
	}

	if (m_bhasaudio)
	{
		for (i = 0; i < ifmt_ctx_a->nb_streams; i++)
		{
			//Create output AVStream according to input AVStream
			if (ifmt_ctx_a->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				AVStream *in_stream = ifmt_ctx_a->streams[i];
// 				AVRational rational;
// 				rational.den = 1;
// 				rational.num = 30;
// 				in_stream->r_frame_rate = rational;
				AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
				audioindex_a = i;
				if (!out_stream)
				{
					printf("Failed allocating output stream\n");
					ret = AVERROR_UNKNOWN;
					goto end;
				}
				audioindex_out = out_stream->index;
				//Copy the settings of AVCodecContext
				if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0)
				{
					printf("Failed to copy context from input to output stream codec context\n");
					goto end;
				}
// 				AVCodecContext* c;
// 				c = out_stream->codec;
// 				c->sample_fmt = AV_SAMPLE_FMT_S16;
// 				c->bit_rate = 16000;
// 				c->sample_rate = 8000;
// 				c->channels = AV_CH_LAYOUT_MONO;
// 				c->time_base.num = 1;
// 				c->time_base.den = 30;
// 				AVCodecContext* c;
// 				c = out_stream->codec;
// 				c->codec_type = AVMEDIA_TYPE_AUDIO;				
// 				/* put sample parameters */
// 				c->sample_fmt = AV_SAMPLE_FMT_S16;
// 				c->bit_rate = 64000;
// 				c->sample_rate = 44100;
// 				c->channels = 2;
				out_stream->codec->codec_tag = 0;
				if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
					out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;


				break;
			}
		}
	}
	//Open output file
	if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
	{
		if (avio_open(&ofmt_ctx->pb, m_strFilename.c_str(), AVIO_FLAG_WRITE) < 0)
		{
			printf("Could not open output file '%s'", m_strFilename.c_str());
			goto end;
		}
	}

	//Write file header
	if (avformat_write_header(ofmt_ctx, NULL) < 0)
	{
		printf("Error occurred when opening output file\n");
		goto end;
	}

	//FIX
#if USE_H264BSF
	AVBitStreamFilterContext* h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");
#endif
#if USE_AACBSF
	AVBitStreamFilterContext* aacbsfc = av_bitstream_filter_init("aac_adtstoasc");
#endif

	while (1) 
	{
		AVFormatContext *ifmt_ctx;
		int stream_index = 0;
		AVStream *in_stream, *out_stream;
		int nStreamIndexType = -1;
		bool bGetPack = false;
		//Get an AVPacket
		if (m_bhasaudio)
		{
			if (av_compare_ts(cur_pts_v, ifmt_ctx_v->streams[videoindex_v]->time_base, cur_pts_a, ifmt_ctx_a->streams[audioindex_a]->time_base) <= 0)
			{
				ifmt_ctx = ifmt_ctx_v;
				stream_index = videoindex_out;
				
				if (av_read_frame(ifmt_ctx, &pkt) >= 0)
				{
					bGetPack = true;
					do{
						nStreamIndexType = 1;
						in_stream = ifmt_ctx->streams[pkt.stream_index];
						out_stream = ofmt_ctx->streams[stream_index];
						if (pkt.stream_index == videoindex_v)
						{
							//FIX：No PTS (Example: Raw H.264)
							//Simple Write PTS
							if (pkt.pts == AV_NOPTS_VALUE)
							{
								//Write PTS
								AVRational time_base1 = in_stream->time_base;
								//Duration between 2 frames (us)
								int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
								//Parameters
								pkt.pts = (double)(frame_index*calc_duration) / (double)(av_q2d(time_base1)*AV_TIME_BASE);
								pkt.dts = pkt.pts;
								pkt.duration = (double)calc_duration / (double)(av_q2d(time_base1)*AV_TIME_BASE);
								frame_index++;
							}
							cur_pts_v = pkt.pts;
							break;
						}
					} while (av_read_frame(ifmt_ctx, &pkt) >= 0);
				}
				else
				{
					if (!m_bWriteBuff)
					{
						break;
					}
					
				}
			}
			else
			{
				ifmt_ctx = ifmt_ctx_a;
				stream_index = audioindex_out;
				if (av_read_frame(ifmt_ctx, &pkt) >= 0)
				{
					bGetPack = true;
					do
					{
						nStreamIndexType = 2;
						in_stream = ifmt_ctx->streams[pkt.stream_index];
						out_stream = ofmt_ctx->streams[stream_index];
						if (pkt.stream_index == audioindex_a)
						{
							//FIX：No PTS
							//Simple Write PTS
							if (pkt.pts == AV_NOPTS_VALUE)
							{
								//Write PTS
								AVRational time_base1 = in_stream->time_base;
								//Duration between 2 frames (us)
								int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
								//Parameters
								pkt.pts = (double)(frame_index*calc_duration) / (double)(av_q2d(time_base1)*AV_TIME_BASE);
								pkt.dts = pkt.pts;
								pkt.duration = (double)calc_duration / (double)(av_q2d(time_base1)*AV_TIME_BASE);
								frame_index++;
							}
							cur_pts_a = pkt.pts;
							break;
						}
					} while (av_read_frame(ifmt_ctx, &pkt) >= 0);
				}
				else
				{
					if (!m_bWriteBuff)
					{
						break;
					}
				}
			}
		}
		else
		{
			ifmt_ctx = ifmt_ctx_v;
			stream_index = videoindex_out;
			if (av_read_frame(ifmt_ctx, &pkt) >= 0)
			{
				bGetPack = true;
				do{
					nStreamIndexType = 1;
					in_stream = ifmt_ctx->streams[pkt.stream_index];
					out_stream = ofmt_ctx->streams[stream_index];
					if (pkt.stream_index == videoindex_v)
					{
						//FIX：No PTS (Example: Raw H.264)
						//Simple Write PTS
						if (pkt.pts == AV_NOPTS_VALUE)
						{
							//Write PTS
							AVRational time_base1 = in_stream->time_base;
							//Duration between 2 frames (us)
							int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
							//Parameters
							pkt.pts = (double)(frame_index*calc_duration) / (double)(av_q2d(time_base1)*AV_TIME_BASE);
							pkt.dts = pkt.pts;
							pkt.duration = (double)calc_duration / (double)(av_q2d(time_base1)*AV_TIME_BASE);
							frame_index++;
						}
						cur_pts_v = pkt.pts;
						break;
					}
				} while (av_read_frame(ifmt_ctx, &pkt) >= 0);
			}
			else
			{
				if (!m_bWriteBuff)
				{
					break;
				}
			}
		}
		

		if (bGetPack)
		{
			if (stream_index == 1)
			{
#if USE_AACBSF
				av_bitstream_filter_filter(aacbsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
				//Convert PTS/DTS
				pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
				pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
				pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
				pkt.pos = -1;
				pkt.stream_index = stream_index;
#endif
			}
			else if (stream_index == 0)
			{
#if USE_H264BSF
				av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
				//Convert PTS/DTS
				pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
				pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
				pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
				pkt.pos = -1;
				pkt.stream_index = stream_index;
#endif
			}

			


			//printf("Write 1 Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);
			//Write
			if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0)
			{
				printf("Error muxing packet\n");
				break;
			}
			av_free_packet(&pkt);
		}
	}
	//Write file trailer
	av_write_trailer(ofmt_ctx);

#if USE_H264BSF
	av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
	av_bitstream_filter_close(aacbsfc);
#endif

end:
	if (ifmt_ctx_v->pb)
	{
		ifmt_ctx_v->pb->opaque = NULL;
		av_freep(&ifmt_ctx_v->pb->buffer);
		av_freep(&ifmt_ctx_v->pb);
	}

	//test
	//av_file_unmap(buffer, buffer_size);
	
	if (m_bhasaudio)
	{
		ifmt_ctx_a->pb->opaque = NULL;
		av_freep(&ifmt_ctx_a->pb->buffer);
		av_freep(&ifmt_ctx_a->pb);
		//avio_close(ifmt_ctx_a->pb);
		avformat_close_input(&ifmt_ctx_a);
	}
	avio_close(ifmt_ctx_v->pb);
	avformat_close_input(&ifmt_ctx_v);
	
	/* close output */
	if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
	{
// 		av_freep(&ofmt_ctx->pb->buffer);
// 		ofmt_ctx->pb->opaque = NULL;
		avio_close(ofmt_ctx->pb);
	}

	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF)
	{
		printf("Error occurred.\n");
	}
}



void CFFmpegrecord::useffmpegstart()
{
	uint8_t *buf = (uint8_t *)av_mallocz(sizeof(uint8_t)*BUF_SIZE);

	
	av_register_all();
	AVCodec *pVideoCodec, *pAudioCodec;
	AVCodecContext *pVideoCodecCtx = NULL;
	AVCodecContext *pAudioCodecCtx = NULL;
	AVIOContext * pb = NULL;
	AVInputFormat *piFmt = NULL;
	AVFormatContext *pFmt = NULL;

	pb = avio_alloc_context(buf, BUF_SIZE, 0, NULL, read_data, NULL, NULL);
	if (!pb) 
	{
		fprintf(stderr, "avio alloc failed!\n");
		return ;
	}
	if (av_probe_input_buffer(pb, &piFmt, "", NULL, 0, 0) < 0) 
	{
		fprintf(stderr, "probe failed!\n");
		return;
	} 
	else 
	{
		fprintf(stdout, "probe success!\n");
		fprintf(stdout, "format: %s[%s]\n", piFmt->name, piFmt->long_name);
	}
	pFmt = avformat_alloc_context();
	pFmt->pb = pb;
	if (avformat_open_input(&pFmt, "", piFmt, NULL) < 0) 
	{
		fprintf(stderr, "avformat open failed.\n");
		return ;
	} 
	else 
	{
		fprintf(stdout, "open stream success!\n");
	}
	//以下就和文件处理一致了
	if (avformat_find_stream_info(pFmt,NULL) < 0) 
	{
		fprintf(stderr, "could not fine stream.\n");
		return ;
	}
	av_dump_format(pFmt, 0, "", 0);
	int videoindex = -1;
	int audioindex = -1;
	for (int i = 0; i < pFmt->nb_streams; i++) 
	{
		if ( (pFmt->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) &&
			(videoindex < 0) )
		{
			videoindex = i;
		}
		if ( (pFmt->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) &&
			(audioindex < 0) ) 
		{
			audioindex = i;
		}
	}
	if (videoindex < 0 || audioindex < 0) 
	{
		fprintf(stderr, "videoindex=%d, audioindex=%d\n", videoindex, audioindex);
		return ;
	}
	AVStream *pVst,*pAst;
	pVst = pFmt->streams[videoindex];
	pAst = pFmt->streams[audioindex];
	pVideoCodecCtx = pVst->codec;
	pAudioCodecCtx = pAst->codec;
	pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);
	if (!pVideoCodec) 
	{
		fprintf(stderr, "could not find video decoder!\n");
		return ;
	}
	if (avcodec_open2(pVideoCodecCtx, pVideoCodec,NULL) < 0) 
	{
		fprintf(stderr, "could not open video codec!\n");
		return ;
	}
	pAudioCodec = avcodec_find_decoder(pAudioCodecCtx->codec_id);
	if (!pAudioCodec) 
	{
		fprintf(stderr, "could not find audio decoder!\n");
		return ;
	}
	if (avcodec_open2(pAudioCodecCtx, pAudioCodec,NULL) < 0)
	{
		fprintf(stderr, "could not open audio codec!\n");
		return;
	}
	int got_picture;
	uint8_t samples[AVCODEC_MAX_AUDIO_FRAME_SIZE*3/2];
	AVFrame *pframe = av_frame_alloc();
	AVPacket pkt;
	av_init_packet(&pkt);
	while(1) {
		if (av_read_frame(pFmt, &pkt) >= 0) 
		{
			if (pkt.stream_index == videoindex)
			{
				
			}
			else if (pkt.stream_index == audioindex)
			{
				
			}
			av_free_packet(&pkt);
		}
	}
	av_free(buf);
	av_free(pframe);
}

#ifdef USE_OTHER_WAY
int CFFmpegrecord::CreateMp4( AVFormatContext*&pOC,void*p,int len )
{
	int ret; // 成功返回0，失败返回1
	//const char* pszFileName = "output002.mp4";
	AVOutputFormat *fmt;
	AVCodec *video_codec;
	AVStream *m_pVideoSt;

	if (0x67 != get_nal_type(p, len))
	{
		printf("can not detect nal type");
		return -1;
	}
	

	fmt = m_pOC->oformat;

	if (fmt->video_codec != AV_CODEC_ID_NONE)
	{
		m_pVideoSt = add_stream(m_pOC, &video_codec, fmt->video_codec);
	}

	if (m_pVideoSt)
	{
		open_video(m_pOC, video_codec, m_pVideoSt); 
	}

	av_dump_format(m_pOC, 0, m_strFilename.c_str(), 1);

	/* open the output file, if needed */
	if (!(fmt->flags & AVFMT_NOFILE))
	{
		ret = avio_open(&m_pOC->pb, m_strFilename.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0)
		{
			printf("could not open '%s': %s\n", m_strFilename.c_str());
			return -1;
		}
	}

	/* Write the stream header, if any */
	ret = avformat_write_header(m_pOC, NULL);
	if (ret < 0)
	{
		printf("Error occurred when opening output file");
		return -1;
	}

	return 0;

}

int CFFmpegrecord::getVopType( const void*p,int len )
{
	if ( !p || 6 >= len )
		return -1;

	unsigned char *b = (unsigned char*)p;

	// Verify NAL marker
	if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
	{  
		b++;
		if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
			return -1;
	} // end if

	b += 3;

	// Verify VOP id
	if ( 0xb6 == *b )
	{   
		b++;
		return ( *b & 0xc0 ) >> 6;
	} // end if

	switch( *b )
	{   
		case 0x65 : return 0;
		case 0x61 : return 1;
		case 0x01 : return 2;
	} // end switch

	return -1;
}

int CFFmpegrecord::get_nal_type( void*p,int len )
{
	if ( !p || 5 >= len )
		return -1;

	unsigned char *b = (unsigned char*)p;

	// Verify NAL marker
	if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
	{  
		b++;
		if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
			return -1;
	} // end if

	b += 3;

	return *b;
}

AVStream* CFFmpegrecord::add_stream( AVFormatContext*oc,AVCodec**codec,enum AVCodecID codec_id )
{
	AVCodecContext *c;
	AVStream *st;

	/* find the encoder */
	*codec = avcodec_find_encoder(codec_id);
	if (!*codec)
	{
		printf("could not find encoder for '%s' \n", avcodec_get_name(codec_id));
		exit(1);
	}

	st = avformat_new_stream(oc, *codec);
	if (!st)
	{
		printf("could not allocate stream \n");
		exit(1);
	}
	st->id = oc->nb_streams-1;
	c = st->codec;
	m_vi = st->index;

	switch ((*codec)->type)
	{
	case AVMEDIA_TYPE_AUDIO:
		c->sample_fmt = (*codec)->sample_fmts ? (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
		c->bit_rate = 64000;
		c->sample_rate = 44100;
		c->channels = 2;
		break;

	case AVMEDIA_TYPE_VIDEO:
		c->codec_id = codec_id;
		c->bit_rate = 90000;
		c->width = 480;
		c->height = 354;
		c->time_base.den = 15;
		c->time_base.num = 1;
		c->gop_size = 12;
		c->pix_fmt = STREAM_PIX_FMT;
		if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO)
		{
			c->max_b_frames = 2;
		}
		if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO)
		{
			c->mb_decision = 2;
		}
		break;

	default:
		break;
	}

	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
	{
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}

	return st;
}

void CFFmpegrecord::open_video( AVFormatContext*oc,AVCodec*codec,AVStream*st )
{
	int ret;
	AVCodecContext *c = st->codec;

	/* open the codec */
	ret = avcodec_open2(c, codec, NULL);
	if (ret < 0)
	{
		printf("could not open video codec");
		//exit(1);
	}
}


void CFFmpegrecord::WriteVideo( AVFormatContext*&pOC,void*data,int nlen )
{
	int ret;

	if ( 0 > m_vi )
	{
		printf("vi less than 0");
		//return -1;
	}
	AVStream *pst = m_pOC->streams[ m_vi ];

	// Init packet
	AVPacket pkt;

	// 我的添加，为了计算pts
	AVCodecContext *c = pst->codec;

	av_init_packet( &pkt );
	pkt.flags |= ( 0 >= getVopType(data,nlen)) ? AV_PKT_FLAG_KEY : 0;   

	pkt.stream_index = pst->index;
	pkt.data = (uint8_t*)data;
	pkt.size = nlen;

	// Wait for key frame
	if ( m_waitkey )
		if ( 0 == ( pkt.flags & AV_PKT_FLAG_KEY ) )
			return ;
		else
			m_waitkey = 0;


	pkt.pts = (m_ptsInc++) * (90000/STREAM_FRAME_RATE);
	//pkt.dts = (ptsInc++) * (90000/STREAM_FRAME_RATE);

	ret = av_interleaved_write_frame(m_pOC,&pkt);
	if (ret < 0)
	{
		printf("cannot write frame");
	}

}

void CFFmpegrecord::CloseMp4( AVFormatContext*&pOC )
{
	m_waitkey = -1;
	m_vi = -1;

	if (m_pOC)
		av_write_trailer(m_pOC);

	if (m_pOC && !(m_pOC->oformat->flags & AVFMT_NOFILE))
		avio_close(m_pOC->pb);

	if (m_pOC)
	{
		avformat_free_context(m_pOC);
		m_pOC = NULL;
	}
}

#endif