#include "stdafx.h"
#include "FFmpegFileRecord.h"


IFFmpegfilerecord*IFFmpegfilerecord::Create(IFFmpegfilerecordNotify& rNotify)
{
	IFFmpegfilerecord*pRecord = NULL;
	pRecord = new CFFmpegFileRecord(rNotify);
	return pRecord;
}

CFFmpegFileRecord::CFFmpegFileRecord(IFFmpegfilerecordNotify& rNotify)
	:m_rNotify(rNotify)
{

}

CFFmpegFileRecord::~CFFmpegFileRecord()
{
}

int CFFmpegFileRecord::CompoundFile(const char*cszFileName, const char*cszVideoFileName, const char*cszAudioFileName)
{
	bool bCompoundResult = false;
	AVOutputFormat *ofmt = NULL;
	//Input AVFormatContext and Output AVFormatContext
	AVFormatContext *ifmt_ctx_v = NULL, *ifmt_ctx_a = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
	int ret, i;
	int videoindex_v = -1, videoindex_out = -1;
	int audioindex_a = -1, audioindex_out = -1;
	int frame_index = 0;
	int64_t cur_pts_v = 0, cur_pts_a = 0;
		
	std::string strVideoFile = cszVideoFileName;
	std::string strAudioFile = cszAudioFileName;
	std::string strCompoundFile = cszFileName;
	const char *in_filename_v = strVideoFile.c_str();
	const char *in_filename_a = strAudioFile.c_str();
	const char *out_filename = strCompoundFile.c_str();

	av_register_all();
	//Input
	if ((ret = avformat_open_input(&ifmt_ctx_v, in_filename_v, 0, 0)) < 0) {
		printf("Could not open input file.");
		goto end;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx_v, 0)) < 0) {
		printf("Failed to retrieve input stream information");
		goto end;
	}

	if ((ret = avformat_open_input(&ifmt_ctx_a, in_filename_a, 0, 0)) < 0) {
		printf("Could not open input file.");
		goto end;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx_a, 0)) < 0) {
		printf("Failed to retrieve input stream information");
		goto end;
	}
	printf("===========Input Information==========\n");
	av_dump_format(ifmt_ctx_v, 0, in_filename_v, 0);
	av_dump_format(ifmt_ctx_a, 0, in_filename_a, 0);
	printf("======================================\n");
	//Output
	avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
	if (!ofmt_ctx) {
		printf("Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	ofmt = ofmt_ctx->oformat;

	for (i = 0; i < ifmt_ctx_v->nb_streams; i++)
	{
		//Create output AVStream according to input AVStream

		if (ifmt_ctx_v->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			AVStream *in_stream = ifmt_ctx_v->streams[i];
			AVRational rational;
			rational.den = 1;
			rational.num = 30;
			in_stream->r_frame_rate = rational;
			AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
			videoindex_v = i;
			if (!out_stream)
			{
				printf("Failed allocating output stream\n");
				ret = AVERROR_UNKNOWN;
				goto end;
			}
			videoindex_out = out_stream->index;
			AVCodecContext* c;
			c = out_stream->codec;
			c->bit_rate = 384000;
			c->width = 640;
			c->height = 480;
			c->time_base.num = 1;
			c->time_base.den = 30;
			//Copy the settings of AVCodecContext
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

	for (i = 0; i < ifmt_ctx_a->nb_streams; i++)
	{
		//Create output AVStream according to input AVStream
		if (ifmt_ctx_a->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			AVStream *in_stream = ifmt_ctx_a->streams[i];
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
			if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
				printf("Failed to copy context from input to output stream codec context\n");
				goto end;
			}
			out_stream->codec->codec_tag = 0;
			if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
				out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

			break;
		}
	}

	printf("==========Output Information==========\n");
	av_dump_format(ofmt_ctx, 0, out_filename, 1);
	printf("======================================\n");
	//Open output file
	if (!(ofmt->flags & AVFMT_NOFILE))
	{
		if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0)
		{
			printf("Could not open output file '%s'", out_filename);
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

		//Get an AVPacket
		if (av_compare_ts(cur_pts_v, ifmt_ctx_v->streams[videoindex_v]->time_base, cur_pts_a, ifmt_ctx_a->streams[audioindex_a]->time_base) <= 0)
		{
			ifmt_ctx = ifmt_ctx_v;
			stream_index = videoindex_out;

			if (av_read_frame(ifmt_ctx, &pkt) >= 0)
			{
				do{
					in_stream = ifmt_ctx->streams[pkt.stream_index];
					out_stream = ofmt_ctx->streams[stream_index];

					if (pkt.stream_index == videoindex_v){
						//FIX��No PTS (Example: Raw H.264)
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
				break;
			}
		}
		else
		{
			ifmt_ctx = ifmt_ctx_a;
			stream_index = audioindex_out;
			if (av_read_frame(ifmt_ctx, &pkt) >= 0)
			{
				do{
					in_stream = ifmt_ctx->streams[pkt.stream_index];
					out_stream = ofmt_ctx->streams[stream_index];

					if (pkt.stream_index == audioindex_a)
					{

						//FIX��No PTS
						//Simple Write PTS
						if (pkt.pts == AV_NOPTS_VALUE){
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
				break;
			}

		}

		//FIX:Bitstream Filter
#if USE_H264BSF
		av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
#if USE_AACBSF
		av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif


		//Convert PTS/DTS
		pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
		pkt.pos = -1;
		pkt.stream_index = stream_index;

		printf("Write 1 Packet. size:%5d\tpts:%lld\n", pkt.size, pkt.pts);
		//Write
		int nReslut = av_interleaved_write_frame(ofmt_ctx, &pkt);
		if (nReslut < 0)
		{
// 			char* buf = NULL;
// 			buf = (char*)av_malloc(1024);
// 			av_strerror(nReslut, buf, 1024);
// 			printf("Couldn't open file: %d(%s)", nReslut, buf);
			printf("Error muxing packet\n");
			break;
		}
		av_free_packet(&pkt);

	}
	//Write file trailer
	av_write_trailer(ofmt_ctx);


	bCompoundResult = true;

#if USE_H264BSF
	av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
	av_bitstream_filter_close(aacbsfc);
#endif

end:


	m_rNotify.OnCompoundResut(bCompoundResult?0:-1);

	avformat_close_input(&ifmt_ctx_v);
	avformat_close_input(&ifmt_ctx_a);
	/* close output */
	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
		printf("Error occurred.\n");
		return -1;
	}
	return bCompoundResult ? 0 : -1;
}
