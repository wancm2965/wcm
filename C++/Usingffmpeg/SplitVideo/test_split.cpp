#include <iostream>
#include <string>
#include <vector>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil\opt.h"

}
using std::cout;
using std::endl;
using std::string;
using std::vector;

AVCodecContext *encCtx;

bool SplitvideowriteVideoHeader(AVFormatContext *ifmt_ctx,AVFormatContext *ofmt_ctx,string out_filename)
{
	AVOutputFormat *ofmt = NULL;
	int ret;
	
	if (!ofmt_ctx) {
		printf("Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		return false;
	}
	ofmt = ofmt_ctx->oformat;
	for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
		//根据输入流创建输出流（Create output AVStream according to input AVStream）
		AVStream *in_stream = ifmt_ctx->streams[i];
		AVStream *out_stream = avformat_new_stream(ofmt_ctx,ifmt_ctx->streams[i]->codec->codec);

		if (!out_stream) {
			printf("Failed allocating output stream\n");
			ret = AVERROR_UNKNOWN;
			return false;
		}
		if (in_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			encCtx = out_stream->codec;
			encCtx->codec_id = AV_CODEC_ID_H264;
			encCtx->codec_type = AVMEDIA_TYPE_VIDEO;
			encCtx->pix_fmt = in_stream->codec->pix_fmt;
			if (in_stream->codec->time_base.den > 25)
			{
				encCtx->time_base = { 1, 25 };
				encCtx->pkt_timebase = { 1, 25 };

			}
			else{
				encCtx->time_base = in_stream->codec->time_base;
				encCtx->pkt_timebase = in_stream->codec->pkt_timebase;
			}
			encCtx->width = in_stream->codec->width;
			encCtx->height = in_stream->codec->height;
			//encCtx->framerate = { 1, 6 };
			//encCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
		}
		
		//复制AVCodecContext的设置（Copy the settings of AVCodecContext）
		//ofmt_ctx->streams[i] = out_stream;
		//ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
		//ofmt_ctx->streams[0]->time_base = { 1, 25 };
		/*if (ret < 0) {
			printf("Failed to copy context from input to output stream codec context\n");
			return false;
		}*/
		out_stream->codec->codec_tag = 0;
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&ofmt_ctx->pb, out_filename.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0) {
			printf("Could not open output file '%s'", out_filename);
			return false;
		}
	}
	ret = avformat_write_header(ofmt_ctx, NULL);
	if (ret < 0){
		return false;
	}
	return true;
}




const int splitSize = 100;
int main_sss(int argc, char *argv[])
{
	
	//输入对应一个AVFormatContext，输出对应一个AVFormatContext
	//（Input AVFormatContext and Output AVFormatContext）
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
	string in_filename, out_filename;
	int ret, i;
	AVPacket readPkt, splitKeyPacket;

	in_filename ="E:/video/remux/ts.mp4";
	out_filename = "E:/video/remux/tss.mp4";
	av_register_all();
	//输入（Input）
	if ((ret = avformat_open_input(&ifmt_ctx, in_filename.c_str(), 0, 0)) < 0) {
		printf("Could not open input file.");
		return -1;
	}
	
	if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		printf("Failed to retrieve input stream information");
		return -1;
	}
	
	int den=ifmt_ctx->streams[0]->avg_frame_rate.den;
	int num = ifmt_ctx->streams[0]->avg_frame_rate.num;
	//int den = ifmt_ctx->streams[0]->time_base.den;
	//int num = ifmt_ctx->streams[0]->time_base.num;
	float fps = (float)num / den;

	av_dump_format(ifmt_ctx, 0, in_filename.c_str(), 0);
	if (ret < 0) {
		printf("Error occurred when opening output file\n");
		return false;
	}
	//int frame_index = 0;
	int splitVideoSize = fps*splitSize;
	string save_name;
	save_name=out_filename.substr(0, out_filename.find_last_of("."));
	string temp_name = save_name + "0.mp4";
	avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, temp_name.c_str());
	SplitvideowriteVideoHeader(ifmt_ctx, ofmt_ctx, temp_name);
	vector<uint64_t> vecKeyFramePos;
	uint64_t frame_index = 0;
	uint64_t keyFrame_index = 0;
	int frameCount = 0;
	//读取分割点附近的关键帧位置
	while (1)
	{
		++frameCount;
		++frame_index;
		ret = av_read_frame(ifmt_ctx, &pkt);
		if (ret < 0)
		{
			break;
		}
		if (pkt.flags&AV_PKT_FLAG_KEY)
		{
			keyFrame_index = frame_index;
		}
		if (frameCount>splitVideoSize)
		{
			vecKeyFramePos.push_back(keyFrame_index);
			frameCount = 0;
		}
	}
	avformat_close_input(&ifmt_ctx);
	ifmt_ctx = NULL;
	//为了重新获取avformatcontext
	if ((ret = avformat_open_input(&ifmt_ctx, in_filename.c_str(), 0, 0)) < 0) {
		printf("Could not open input file.");
		return -1;
	}

	if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		printf("Failed to retrieve input stream information");
		return -1;
	}
	int number = 0;
	AVPacket key_pkt;
	av_init_packet(&key_pkt);
	key_pkt.data = NULL;
	key_pkt.size = 0;
	
	vector<uint64_t>::iterator keyFrameIter = vecKeyFramePos.begin();
	av_init_packet(&splitKeyPacket);
	splitKeyPacket.data = NULL;
	splitKeyPacket.size = 0;
	if (vecKeyFramePos.size() <= 0)
	{
		keyFrame_index = frame_index;
	}
	else{
		keyFrame_index = *keyFrameIter;
		++keyFrameIter;

	}
	frame_index = 0;
	int64_t lastPts = 0;
	int64_t lastDts = 0;
	int64_t prePts = 0;
	int64_t preDts = 0;
	while (1)
	{
		++frame_index;
		ret = av_read_frame(ifmt_ctx, &readPkt);
		if (ret < 0)
		{
			break;
		}
		av_packet_rescale_ts(&readPkt, ifmt_ctx->streams[readPkt.stream_index]->time_base, ofmt_ctx->streams[readPkt.stream_index]->time_base);
		prePts = readPkt.pts;
		preDts = readPkt.dts;
		readPkt.pts -= lastPts;
		readPkt.dts -= lastDts;
		if (readPkt.pts < readPkt.dts)
		{
			readPkt.pts = readPkt.dts + 1;
		}
		//为分割点处的关键帧要进行拷贝
		if (readPkt.flags&AV_PKT_FLAG_KEY&&frame_index == keyFrame_index)
		{
			av_copy_packet(&splitKeyPacket, &readPkt);
		}
		else{
			ret = av_interleaved_write_frame(ofmt_ctx, &readPkt);
			if (ret < 0) {
				//break;
				cout << "write failed" << endl;
			}
		}

		if (frame_index == keyFrame_index)
		{
			lastDts = preDts;
			lastPts = prePts;
			if (keyFrameIter != vecKeyFramePos.end())
			{
				keyFrame_index = *keyFrameIter;
				++keyFrameIter;
			}
			av_write_trailer(ofmt_ctx);
			avio_close(ofmt_ctx->pb);
			avformat_free_context(ofmt_ctx);
			++number;
			char num[10];
			sprintf_s(num, "%d.mp4", number);
			temp_name = save_name + num;
			avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, temp_name.c_str());
			if (!ofmt_ctx) {
				ret = AVERROR_UNKNOWN;
				return false;
			}
			if (!SplitvideowriteVideoHeader(ifmt_ctx, ofmt_ctx, save_name + num))
			{
				return false;
			}
			splitKeyPacket.pts = 0;
			splitKeyPacket.dts = 0;
			//把上一个分片处的关键帧写入到下一个分片的起始处，保证下一个分片的开头为I帧
			ret = av_interleaved_write_frame(ofmt_ctx, &splitKeyPacket);
		}
		av_packet_unref(&readPkt);

	}
	//int number = 0;
	//uint64_t lastPts = 0;
	//uint64_t lastDts = 0;
	//uint64_t prePts = 0;
	//uint64_t preDts = 0;
	//AVPacket key_pkt;
	//av_init_packet(&key_pkt);
	//key_pkt.data = NULL;
	//key_pkt.size = 0;
	//while (1) {
	//	//AVStream *in_stream, *out_stream;
	//	//获取一个AVPacket（Get an AVPacket）
	//	ret = av_read_frame(ifmt_ctx, &pkt);
	//	if (ret < 0)
	//		break;
	//	//in_stream = ifmt_ctx->streams[pkt.stream_index];
	//	//out_stream = ofmt_ctx->streams[pkt.stream_index];
	//	
	//	av_packet_rescale_ts(&pkt, ifmt_ctx->streams[pkt.stream_index]->time_base, ofmt_ctx->streams[pkt.stream_index]->time_base);
	//	prePts = pkt.pts;
	//	preDts = pkt.dts;
	//	pkt.pts -= lastPts;
	//	pkt.dts -= lastDts;
	//	if (pkt.flags&AV_PKT_FLAG_KEY)
	//	{
	//		av_copy_packet(&key_pkt, &pkt);
	//	}
	//	//pkt.duration = av_rescale_q(pkt.duration, ofmt_ctx->streams[0]->codec->time_base, ofmt_ctx->streams[0]->time_base);
	//	
	//	//pkt.pos = -1;
	//	//写入（Write）
	//	ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
	//	if (ret < 0) {
	//		break;
	//	}
	//	av_packet_unref(&pkt);
	//	if (frame_index >= splitVideoSize)
	//	{
	//		lastDts = preDts;
	//		lastPts = prePts;
	//		av_write_trailer(ofmt_ctx);
	//		avio_close(ofmt_ctx->pb);
	//		avformat_free_context(ofmt_ctx);
	//		++number;
	//		char num[10];
	//		_itoa_s(number, num, 10);
	//		temp_name = save_name + num + ".mp4";
	//		avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, temp_name.c_str());
	//		writeVideoHeader(ifmt_ctx, ofmt_ctx, save_name +num+ ".mp4");
	//		frame_index = 0;
	//		key_pkt.pts = 0;
	//		key_pkt.dts = 0;
	//		//写一个关键帧
	//		ret = av_interleaved_write_frame(ofmt_ctx, &key_pkt);
	//	}
	//	else{
	//		frame_index++;
	//	}
	//	

	//}
	av_packet_unref(&key_pkt);
	av_write_trailer(ofmt_ctx);
	avformat_close_input(&ifmt_ctx);
	avio_close(ofmt_ctx->pb);
	/* close output */
	/*if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);*/
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
		printf("Error occurred.\n");
		return -1;
	}
}