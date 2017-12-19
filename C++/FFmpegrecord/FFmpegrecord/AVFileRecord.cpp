#include "stdafx.h"
#include "AVFileRecord.h"

int				g_nAudioFillInFrameLen = 6;
unsigned char	g_ucAudioFillInFrame[] = { 0x00, 0xc8, 0x00, 0x80, 0x23, 0x80 };
#define MAX_AV_SPACETIME 100
#define MXF_OUT_SAMPLES 48000
#define VIDEO_BASE 15360
#define AUDIO_BASE 44100
static int g_nAVFileRecordVideoCount = 0;

#define USE_FRAME 0
#define USE_TIME  1

//////////////////////////////////////////////////////////////////////////
CAVFileRecord::CAVFileRecord()
	: m_uFileSize(0)
	, m_pFormatCtx(NULL)
	, m_pVideoStream(NULL)
	, m_pAudioStream(NULL)
	, m_strFileName("")
	, m_ulVideoFrames(0)
	, m_ulAudioFrames(0)
	, m_bStartStream(false)
	, m_nVidoeTime(0)
	, m_nAudioTime(0)
	, m_pMP4Stream(NULL)
	, m_bInitHeader(false)
{
	if (!CAVFileRecord::m_bAVInit) {
		av_register_all();
		avcodec_register_all();
		av_log_set_level(AV_LOG_ERROR);
		CAVFileRecord::m_bAVInit = 1;
	}
}

CAVFileRecord::~CAVFileRecord(void)
{
	m_uFileSize = 0;
}

int CAVFileRecord::Open(const char*cszFileName)
{
	if (!CAVFileRecord::m_bAVInit)
		return -1;
	
	avformat_alloc_output_context2(&m_pFormatCtx, NULL, NULL, cszFileName);
	if (!m_pFormatCtx) {
		avformat_alloc_output_context2(&m_pFormatCtx, NULL, "mp4", cszFileName);
	}

	// 默认为mp4，如果再失败返回-1
	if (!m_pFormatCtx)
		return -1;

	// 打开输出文件
	if (!(m_pFormatCtx->oformat->flags & AVFMT_NOFILE)) {
		if (avio_open(&m_pFormatCtx->pb, cszFileName, AVIO_FLAG_WRITE) < 0) {
			char err[256] = { 0 };
			sprintf_s(err, 256, "Could not open output file %s", cszFileName);
			
			avformat_free_context(m_pFormatCtx);
			m_pFormatCtx = NULL;
			return -1;
		}
	}

	m_strFileName = cszFileName;
	m_pMP4Stream = new MP4_AVStream();
	if (m_pMP4Stream)
	{
		m_pMP4Stream->nAudioBufLen = 0;
		m_pMP4Stream->nVideoBufLen = 0;
		m_pMP4Stream->pAudioData = NULL;
		m_pMP4Stream->pVideoData = NULL;
		m_pMP4Stream->nKeyFrame = 1;
		m_pMP4Stream->ulAudioDataLen = 0;
		m_pMP4Stream->ulVideoDataLen = 0;
		m_pMP4Stream->ulAduioTimestamp = 0;
		m_pMP4Stream->ulVideoTimestamp = 0;
		m_pMP4Stream->ulAudioFilstTimestamp = 0;
		m_pMP4Stream->ulVideoFilstTimestamp = 0;
		m_pMP4Stream->nVideoWidth = 640;
		m_pMP4Stream->nVideoHeight = 480;
	}
	else
	{
		return -1;
	}
	return 0;
}

void CAVFileRecord::Close(void)
{
	KAutoLock lock(m_KCritSecAV);
	{
		if (m_pFormatCtx != NULL)
		{
			av_write_trailer(m_pFormatCtx);
			if (m_pFormatCtx && !(m_pFormatCtx->oformat->flags & AVFMT_NOFILE))
				avio_close(m_pFormatCtx->pb);
			avformat_free_context(m_pFormatCtx);
			m_pFormatCtx = NULL;
		}

		m_pVideoStream = NULL;
		m_pAudioStream = NULL;
		m_ulVideoFrames = 0;
		m_ulAudioFrames = 0;
		m_nVidoeTime = 0;
		m_nAudioTime = 0;
		m_bStartStream = false;
		if (NULL != m_pMP4Stream)
		{
			if (m_pMP4Stream->pAudioData != NULL)
			{
				free(m_pMP4Stream->pAudioData);
				m_pMP4Stream->pAudioData= NULL;
			}
			if (m_pMP4Stream->pVideoData != NULL)
			{
				free(m_pMP4Stream->pVideoData);
				m_pMP4Stream->pVideoData = NULL;
			}

			delete m_pMP4Stream;
			m_pMP4Stream = NULL;
		}
		m_bInitHeader = false;
	}	
}

int CAVFileRecord::WriteHeader(int nWidth, int nHeight, int nFrameRate, int nSampleRate, int nChannels)
{
	if (!m_pFormatCtx) // 未初始化
		return -1;

	AVOutputFormat* oformat = m_pFormatCtx->oformat;
	AVCodecID videoCodecID = AV_CODEC_ID_H264;
	AVCodecID audioCodecID = AV_CODEC_ID_AAC;

	// 创建视频流
	AVStream* st = add_video_stream(m_pFormatCtx, videoCodecID, nWidth, nHeight, nFrameRate);
	if (!st) {
		//LOG_ERROR("Call add_video_stream function occur error");
		return -1;
	}
	m_pVideoStream = st;

	// 创建音频流
	st = add_audio_stream(m_pFormatCtx, audioCodecID, nSampleRate, nChannels);
	if (!st) {
		//LOG_ERROR("Call add_audio_stream function occur error");
		return -1;
	}
	m_pAudioStream = st;

	/* print output stream information*/
	av_dump_format(m_pFormatCtx, 0, m_strFileName.c_str(), 1);

	/* Write the stream header, if any. */
	int ret = avformat_write_header(m_pFormatCtx, NULL);
	if (ret < 0) {
		char err[128] = { 0 };
		sprintf_s(err, 128, "Error occurred when opening output file: %s",
			av_make_error_string(err, 64, ret));
		//LOG_ERROR(err);
		m_pVideoStream = NULL;
		m_pAudioStream = NULL;
		return -1;
	}

	return 0;
}

int CAVFileRecord::WriteVideo(unsigned char*data, unsigned int len, unsigned long nTimestamp, bool bKeyFrame, unsigned int nWidth /*= 640*/, unsigned int nHeight /*=480*/)
{
	if (NULL == m_pMP4Stream)
	{
		return -1;
	}
	if (bKeyFrame)
	{
		m_bStartStream = true;
	}
	if (!m_bStartStream)
	{
		return -1;
	}
	KAutoLock lock(m_KCritSecAV);
	if (m_pMP4Stream->ulVideoFilstTimestamp == 0)
	{
		m_pMP4Stream->ulVideoFilstTimestamp = nTimestamp;
	}
	int nResult = -1;
	if (m_pMP4Stream->ulVideoTimestamp > 0 && m_pMP4Stream->pVideoData != NULL && nTimestamp > m_pMP4Stream->ulVideoTimestamp)
	{
		if ((m_pMP4Stream->ulVideoFilstTimestamp == m_pMP4Stream->ulVideoTimestamp) && m_pMP4Stream->nKeyFrame)
		{
			//第一帧且是关键帧
			unsigned long nTime = nTimestamp - m_pMP4Stream->ulVideoTimestamp;
			int nFrame = (int)1000 / nTime;
			if (!m_bInitHeader)
			{
				m_bInitHeader = true;
				WriteHeader(m_pMP4Stream->nVideoWidth, m_pMP4Stream->nVideoHeight, nFrame, 48000, 2);
#ifdef WIN32
				CString strOut;
				strOut.Format(_T("-----nFrame is %d -----!\n"), nFrame);
				OutputDebugString(strOut);
#endif // WIN32
								
			}
			else
			{
				//reset
			}			
		}
		else
		{
			m_pMP4Stream->ulVideoFilstTimestamp = nTimestamp;
		}
		nResult = WriteVideoData((unsigned char*)m_pMP4Stream->pVideoData, (unsigned int)m_pMP4Stream->ulVideoDataLen, (uint32_t)m_pMP4Stream->ulVideoTimestamp, (int)m_pMP4Stream->nKeyFrame);
		if (m_pMP4Stream->ulAduioTimestamp > 0 && m_pMP4Stream->ulVideoTimestamp > m_pMP4Stream->ulAduioTimestamp && (m_pMP4Stream->ulVideoTimestamp - m_pMP4Stream->ulAduioTimestamp) > MAX_AV_SPACETIME)
		{
			for (int i = 0; i < MAX_AV_SPACETIME / 20; i++)
			{
				WriteAudio(g_ucAudioFillInFrame, sizeof(g_ucAudioFillInFrame), m_pMP4Stream->ulAduioTimestamp + 22 * 44.1);
			}
		}
	}

	if (m_pMP4Stream->pVideoData == NULL || m_pMP4Stream->nVideoBufLen < len + 512)
	{
		m_pMP4Stream->nVideoBufLen = len + 512;
		if (m_pMP4Stream->pVideoData != NULL)
		{
			free(m_pMP4Stream->pVideoData);
			m_pMP4Stream->pVideoData = NULL;
		}
		m_pMP4Stream->pVideoData = (char*)malloc(len + 512);
		if (m_pMP4Stream->pVideoData == NULL)
		{
			return -1;
		}
	}

	if (m_pMP4Stream->pVideoData && nTimestamp > m_pMP4Stream->ulVideoTimestamp)
	{
		memcpy(m_pMP4Stream->pVideoData, data, len);
		m_pMP4Stream->ulVideoDataLen = len;		
		m_pMP4Stream->ulVideoTimestamp = nTimestamp;
		m_pMP4Stream->nKeyFrame = bKeyFrame ? 1 : 0;
		if (m_pMP4Stream->nKeyFrame)
		{
			m_pMP4Stream->nVideoWidth = nWidth;
			m_pMP4Stream->nVideoHeight = nHeight;
		}
	}

	return nResult;

// 	KAutoLock lock(m_KCritSecAV);
// 	if (!m_pFormatCtx || !m_pVideoStream) // 未初始化
// 		return -1;
// 
// 	AVPacket pkt = { 0 };
// 	av_init_packet(&pkt);
// 
//  	int64_t nCurrentTime = 0;
//  	if (m_nVidoeTime == 0)
//  	{
//  		pkt.pts = 0;
// 		m_nVidoeTime = nTimestamp;
//  	}
//  	else
//  	{
// 		nCurrentTime = nTimestamp;
// 		int64_t nDst = nCurrentTime - m_nVidoeTime;		
// 		m_nVidoeTime = nCurrentTime;
// 		int64_t ndtspts;
// 		AVRational av_time_base_q;
// 		av_time_base_q.num = 1;
// 		av_time_base_q.den = AV_TIME_BASE;
// 		AVRational av_time_base1;
// 		av_time_base1.num = 1;
// 		av_time_base1.den = 90000;
// 		ndtspts = av_rescale_q(nDst, av_time_base_q, av_time_base1);
//  		pkt.pts = ndtspts + m_nVpts;
//  	}
//  	
//  	pkt.dts = pkt.pts;
//  	m_nVpts = pkt.pts;
// 
// 	if (bKeyFrame) 
// 	{
// 		pkt.flags |= AV_PKT_FLAG_KEY;
// 	}
// 	else 
// 	{
// 		pkt.flags = 0;
// 	}
// 
// 	pkt.stream_index = m_pVideoStream->index;
// 	pkt.size = len; /* 帧大小 */
// 	pkt.data = (uint8_t*)av_malloc(pkt.size); /* 帧数据 */
// 	if (!pkt.data) 
// 	{
// 		//LOG_ERROR("Write video no data");
// 		return -1;
// 	}
// 	memcpy(pkt.data, data, pkt.size);
// 
// 	/* Write file video data */
// 	int ret = av_interleaved_write_frame(m_pFormatCtx, &pkt);
// 	if (ret < 0)
// 	{
// 		char err[128] = { 0 };
// 		sprintf_s(err, 128, "Error while writing video frame: %s",av_make_error_string(err, 64, ret));
// 	}
// 	else 
// 	{
// 		m_uFileSize += len;
// 	}
// 
// 	av_free(pkt.data);
// 	av_packet_unref(&pkt);
// 	return ret;
}

int CAVFileRecord::WriteVideoData(unsigned char*data, unsigned int len, unsigned long nTimestamp, int bKeyFrame)
{
	
	if (!m_pFormatCtx || !m_pVideoStream) // 未初始化
		return -1;

	AVPacket pkt = { 0 };
	av_init_packet(&pkt);

#if USE_TIME
	if (m_nVidoeTime == 0)
	{
		pkt.pts = 0;
		m_nVidoeTime = nTimestamp;
	}
	else
	{
		int64_t nDst = nTimestamp - m_nVidoeTime;
		int64_t ndtspts;
		AVRational av_time_base_q;
		av_time_base_q.num = 1;
		av_time_base_q.den = 1000;
// 		AVRational av_time_base1;
// 		av_time_base1.num = 1;
// 		av_time_base1.den = 90000;
		ndtspts = av_rescale_q(nDst, av_time_base_q, m_pVideoStream->time_base);
		pkt.pts = ndtspts ;
	}	
	pkt.dts = pkt.pts;
#endif

#if USE_FRAME
 	AVRational av_time_base_q;
 	av_time_base_q.num = 1;
 	av_time_base_q.den = AV_TIME_BASE;
 	int64_t new_ts = 0;
 	new_ts = (m_ulVideoFrames++)*((double)AV_TIME_BASE / av_q2d(m_pVideoStream->r_frame_rate));
 	pkt.pts = av_rescale_q(new_ts, av_time_base_q, m_pVideoStream->time_base);
 	pkt.dts = pkt.pts;
#endif
	if (bKeyFrame)
	{
		pkt.flags |= AV_PKT_FLAG_KEY;
	}
	else
	{
		pkt.flags = 0;
	}

	pkt.stream_index = m_pVideoStream->index;
	pkt.size = len; /* 帧大小 */
	pkt.data = (uint8_t*)av_malloc(pkt.size); /* 帧数据 */
	if (!pkt.data)
	{
		//LOG_ERROR("Write video no data");
		return -1;
	}
	memcpy(pkt.data, data, pkt.size);

	/* Write file video data */
	int ret = av_interleaved_write_frame(m_pFormatCtx, &pkt);
	if (ret < 0)
	{
		char err[128] = { 0 };
		sprintf_s(err, 128, "Error while writing video frame: %s", av_make_error_string(err, 64, ret));

#ifdef WIN32
		CString strOut;
		strOut.Format(_T("Error while writing video frame: %s\n"), err);
		OutputDebugString(strOut);
#endif 
	}
	else
	{
		m_uFileSize += len;
	}

	av_free(pkt.data);
	av_packet_unref(&pkt);
	return ret;
}

int CAVFileRecord::WriteAudio(unsigned char*data, unsigned int len, unsigned long nTimestamp)
{
	KAutoLock lock(m_KCritSecAV);
	if (!m_bStartStream)
	{
		return -1;
	}
	if (m_pMP4Stream->ulAudioFilstTimestamp == 0)
	{
		m_pMP4Stream->ulAudioFilstTimestamp = nTimestamp;
	}
	int nResult = -1;
	if (m_pMP4Stream->ulAduioTimestamp > 0 && m_pMP4Stream->pAudioData != NULL && nTimestamp > m_pMP4Stream->ulAduioTimestamp)
	{
// 		unsigned long uTimeSpace = nTimestamp - m_pMP4Stream->ulAduioTimestamp;
// 		if (uTimeSpace > 100)
// 		{
// 			unsigned long nCycle = (uTimeSpace) / 22;
// 			for (unsigned long i = 0; i < nCycle; i++)
// 			{
// 				WriteAudio(g_ucAudioFillInFrame, g_nAudioFillInFrameLen, m_pMP4Stream->ulAduioTimestamp + 22 * 44.1);
// 			}
// 		}
		nResult = WriteAudioData((unsigned char*)m_pMP4Stream->pAudioData, (unsigned int)m_pMP4Stream->ulAudioDataLen, m_pMP4Stream->ulAduioTimestamp);
	}
	if (m_pMP4Stream->pAudioData == NULL || m_pMP4Stream->nAudioBufLen < len)
	{
		m_pMP4Stream->nAudioBufLen = len;

		if (m_pMP4Stream->pAudioData != NULL)
		{
			free(m_pMP4Stream->pAudioData);
			m_pMP4Stream->pAudioData = NULL;
		}
		m_pMP4Stream->pAudioData = (char*)malloc(len);
		if (m_pMP4Stream->pAudioData == NULL)
		{
			return -1;
		}
	}
	if (m_pMP4Stream->pAudioData && nTimestamp > m_pMP4Stream->ulAduioTimestamp)
	{
		memcpy(m_pMP4Stream->pAudioData, data, len);
		m_pMP4Stream->ulAudioDataLen = len;
		m_pMP4Stream->ulAduioTimestamp = nTimestamp;
	}
	return nResult;
// 	KAutoLock lock(m_KCritSecAV);
// 	if (!m_pFormatCtx || !m_pAudioStream) // 未初始化
// 		return -1;
// 
// 	int packetLen = len;
// 	AVPacket pkt = { 0 };
// 	av_init_packet(&pkt);
// 	pkt.flags |= AV_PKT_FLAG_KEY;
// 	pkt.stream_index = m_pAudioStream->index;
// 
// 	int32_t frame_size = m_pAudioStream->codecpar->frame_size;
// 	if (frame_size == 0) frame_size = packetLen;
// 
// 	
// 	pkt.size = packetLen; /* 帧大小 */
// 	pkt.data = (uint8_t*)av_malloc(pkt.size); /* 帧数据 */
// 	if (!pkt.data) 
// 	{
// 		//LOG_ERROR("malloc pkt data error");
// 			return -1;
// 	}
// 
// 
// 	memcpy(pkt.data, data, pkt.size);
// 
// 	AVRational av_time_base_q;
// 	av_time_base_q.num = 1;
// 	av_time_base_q.den = AV_TIME_BASE;
// 	int64_t new_ts = 0;
// 	int32_t sample_rate = m_pAudioStream->codecpar->sample_rate;
// 	new_ts = (m_ulAudioFrames++)*(AV_TIME_BASE*frame_size / sample_rate);
// 	pkt.pts = av_rescale_q(new_ts, av_time_base_q, m_pAudioStream->time_base);
// 	pkt.dts = pkt.pts;
// 
// 	
// 
// // 	int64_t nCurrentTime = 0;
// // 	if (m_nAudioTime == 0)
// // 	{
// // 	 	pkt.pts = 0;
// // 		m_nAudioTime = GetSysTimeMicos();
// // 	}
// // 	else
// // 	{
// // 	 	//获取nCurrentTime
// // 	 	nCurrentTime = GetSysTimeMicos();
// // 		int64_t nDst = nCurrentTime - m_nAudioTime;
// // 		m_nAudioTime = nCurrentTime;
// // 		int64_t ndtspts = nDst*AUDIO_BASE / 1000000;
// // 		pkt.pts = ndtspts + m_nApts;
// // 	}	
// // 	pkt.dts = pkt.pts;
// // 	m_nApts = pkt.pts;
// 	/* Write file audio data */
// 	int ret = av_interleaved_write_frame(m_pFormatCtx, &pkt);
// 	if (ret < 0) 
// 	{
// 		char err[128] = { 0 };
// 		sprintf_s(err, 128, "Error while writing audio frame: %s",
// 			av_make_error_string(err, 64, ret));
// 		//LOG_ERROR(err);
// 	}
// 	else 
// 	{
// 		m_uFileSize += len;
// 	}
// 	av_free(pkt.data);
// 	av_packet_unref(&pkt);
// 	return ret;
}

int CAVFileRecord::WriteAudioData(unsigned char*data, unsigned int len, uint32_t nTimestamp)
{	
	
	if (!m_pFormatCtx || !m_pAudioStream) // 未初始化
		return -1;

	int packetLen = len;
	AVPacket pkt = { 0 };
	av_init_packet(&pkt);
	pkt.flags |= AV_PKT_FLAG_KEY;
	pkt.stream_index = m_pAudioStream->index;

	int32_t frame_size = m_pAudioStream->codecpar->frame_size;
	if (frame_size == 0) frame_size = packetLen;


	pkt.size = packetLen; /* 帧大小 */
	pkt.data = (uint8_t*)av_malloc(pkt.size); /* 帧数据 */
	if (!pkt.data)
	{
		//LOG_ERROR("malloc pkt data error");
		return -1;
	}
	memcpy(pkt.data, data, pkt.size);

#if USE_FRAME
	AVRational av_time_base_q;
	av_time_base_q.num = 1;
	av_time_base_q.den = AV_TIME_BASE;
	int64_t new_ts = 0;
	int32_t sample_rate = m_pAudioStream->codecpar->sample_rate;
	new_ts = (m_ulAudioFrames++)*(AV_TIME_BASE*frame_size / sample_rate);
	pkt.pts = av_rescale_q(new_ts, av_time_base_q, m_pAudioStream->time_base);
	pkt.dts = pkt.pts;
#endif

#if USE_TIME
	if (m_nAudioTime == 0)
	{
	  	pkt.pts = 0;
		m_nAudioTime = nTimestamp;
// 		if (m_nAudioTime > m_nVidoeTime && m_nVidoeTime > 0)
// 		{
// 			m_nAudioTime = m_nVidoeTime;
// 		}
	}
	else
	{
		int64_t nDst = nTimestamp - m_nVidoeTime;
		int64_t ndtspts;
		AVRational av_time_base_q;
		av_time_base_q.num = 1;
		av_time_base_q.den = 1000;
		ndtspts = av_rescale_q(nDst, av_time_base_q, m_pAudioStream->time_base);
		pkt.pts = ndtspts ;
	}
	pkt.dts = pkt.pts;
#endif
	
	/* Write file audio data */
	int ret = av_interleaved_write_frame(m_pFormatCtx, &pkt);
	if (ret < 0)
	{
		char err[128] = { 0 };
		sprintf_s(err, 128, "Error while writing audio frame: %s",
			av_make_error_string(err, 64, ret));
#ifdef WIN32
		CString strOut;
		strOut.Format(_T("Error while writing audio frame: %s\n"), err);
		OutputDebugString(strOut);
#endif 

	}
	else
	{
		m_uFileSize += len;
	}
	av_free(pkt.data);
	av_packet_unref(&pkt);
	return ret;
}

unsigned long CAVFileRecord::GetFileSize()
{
	return m_uFileSize;
}

AVStream* CAVFileRecord::add_video_stream(AVFormatContext *oc, AVCodecID codec_id, int width, int height, int fps /*= 60*/)
{
	// 初始化视频编码器
	AVStream* st = avformat_new_stream(oc, NULL);
	if (!st) {
		//LOG_ERROR("Failed allocating output stream");
		return 0;
	}
	st->id = oc->nb_streams - 1;

	AVCodec* pCodec = avcodec_find_encoder(codec_id);
	AVCodecContext* codec_ctx_v = avcodec_alloc_context3(pCodec);
	if (!codec_ctx_v) {
		//LOG_ERROR("Could not alloc an encoding context");
		return 0;
	}

	codec_ctx_v->codec_type = AVMEDIA_TYPE_VIDEO;
	codec_ctx_v->codec_id = codec_id;
	codec_ctx_v->width = width;
	codec_ctx_v->height = height;
	codec_ctx_v->time_base.num = 1;
	codec_ctx_v->time_base.den = fps;
	if (codec_id == AV_CODEC_ID_MPEG2VIDEO) {
		/* just for testing, we also add B-frames */
		codec_ctx_v->max_b_frames = 2;
	}
	if (codec_id == AV_CODEC_ID_MPEG1VIDEO) {
		/* Needed to avoid using macroblocks in which some coeffs overflow.
		* This does not happen with normal video, it just happens here as
		* the motion of the chroma plane does not match the luma plane. */
		codec_ctx_v->mb_decision = 2;
	}
	if (codec_id == AV_CODEC_ID_H264) {
		/* Must be h264baseline otherwise be erro. r*/
		codec_ctx_v->profile = FF_PROFILE_H264_BASELINE;
	}
	codec_ctx_v->pix_fmt = AV_PIX_FMT_YUV420P;

	st->time_base = codec_ctx_v->time_base;
	st->r_frame_rate.den = 1;
	st->r_frame_rate.num = fps;
	st->avg_frame_rate.den = 1;
	st->avg_frame_rate.num = fps;

	/* Some formats want stream headers to be separate. */
	codec_ctx_v->codec_tag = 0;
	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		codec_ctx_v->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	/* Copy the stream parameters to the muxer */
	int ret = avcodec_parameters_from_context(st->codecpar, codec_ctx_v);
	avcodec_free_context(&codec_ctx_v);
	if (ret < 0) {
		//LOG_ERROR("Could not copy the stream parameters");
		return 0;
	}

	return st;
}

void make_dsi(unsigned int sampling_frequency_index, unsigned int channel_configuration, unsigned char* dsi)
{
	unsigned int object_type = 2; // AAC LC by default
	dsi[0] = (object_type << 3) | (sampling_frequency_index >> 1);
	dsi[1] = ((sampling_frequency_index & 1) << 7) | (channel_configuration << 3);
}

int get_sr_index(unsigned int sampling_frequency)
{
	switch (sampling_frequency)
	{
	case 96000: return 0;
	case 88200: return 1;
	case 64000: return 2;
	case 48000: return 3;
	case 44100: return 4;
	case 32000: return 5;
	case 24000: return 6;
	case 22050: return 7;
	case 16000: return 8;
	case 12000: return 9;
	case 11025: return 10;
	case 8000:  return 11;
	case 7350:  return 12;
	default: return 0;
	}
}

AVStream* CAVFileRecord::add_audio_stream(AVFormatContext *oc, AVCodecID codec_id, int samples, int channels /*= 2*/)
{
	// 初始化音频编码器
	AVStream *st = avformat_new_stream(oc, NULL);
	if (!st) {
		//LOG_ERROR("Failed allocating output stream");
		return 0;
	}
	st->id = oc->nb_streams - 1;

	AVCodec* pCodec = avcodec_find_encoder(codec_id);
	AVCodecContext* codec_ctx_a = avcodec_alloc_context3(pCodec);
	if (!codec_ctx_a) {
		//LOG_ERROR("Could not alloc an encoding context");
		return 0;
	}

	codec_ctx_a->codec_type = AVMEDIA_TYPE_AUDIO;
	codec_ctx_a->sample_fmt = pCodec->sample_fmts ?
		pCodec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
	codec_ctx_a->channel_layout = channels > 1 ? AV_CH_LAYOUT_STEREO : AV_CH_LAYOUT_MONO;
	codec_ctx_a->channels = av_get_channel_layout_nb_channels(codec_ctx_a->channel_layout);

	
	{
		if (codec_id == AV_CODEC_ID_AAC)
		{
			unsigned char dsi[2];
			make_dsi((unsigned int)get_sr_index((unsigned int)samples), (unsigned int)channels, dsi);
			codec_ctx_a->extradata_size = 2;
			codec_ctx_a->extradata = (uint8_t*)av_malloc(2);
			memcpy(codec_ctx_a->extradata, dsi, 2);
			codec_ctx_a->frame_size = 1024;
		}
		else if (codec_id == AV_CODEC_ID_MP3)
		{
			codec_ctx_a->frame_size = 1152;
		}
	}

	codec_ctx_a->codec_id = codec_id;
	codec_ctx_a->sample_rate = samples;
	codec_ctx_a->block_align = 1;
	codec_ctx_a->time_base.num = 1;
	codec_ctx_a->time_base.den = samples;

	st->time_base = codec_ctx_a->time_base;
	st->sample_aspect_ratio.num = 1;
	st->sample_aspect_ratio.den = 1;
	st->discard = AVDISCARD_NONE;

	/* Some formats want stream headers to be separate. */
	codec_ctx_a->codec_tag = 0;
	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		codec_ctx_a->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	/* Copy the stream parameters to the muxer */
	int ret = avcodec_parameters_from_context(st->codecpar, codec_ctx_a);
	avcodec_free_context(&codec_ctx_a);
	if (ret < 0) {
		//LOG_ERROR("Could not copy the stream parameters");
		return 0;
	}

	return st;
}

bool CAVFileRecord::m_bAVInit = 0;