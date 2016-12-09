#include "SplitVideo.h"
#include <vector>

using std::vector;

SplitVideo::SplitVideo(string infile, string outfile) :inputFileName(infile), outputFileName(outfile)
{
	string::size_type pos = outfile.find_last_of(".");
	//����ļ����к��к�׺��������ļ�����׺�������������ļ����ĺ�׺
	if (pos != string::npos&&pos != 0)
	{
		suffixName = outfile.substr(pos);
	}
	else{
		suffixName = infile.substr(infile.find_last_of("."));
	}
}


SplitVideo::~SplitVideo()
{
}
vector<string> SplitVideo::getResultName()
{
	return vecResultName;
}
bool SplitVideo::executeSplit(unsigned int splitSeconds)
{
	AVPacket readPkt, splitKeyPacket;
	int ret;
	av_register_all();
	if ((ret = avformat_open_input(&ifmtCtx, inputFileName.c_str(), 0, 0)) < 0) {
		return false;
	}

	if ((ret = avformat_find_stream_info(ifmtCtx, 0)) < 0) {
		return false;
	}
	for (int i = 0; i < ifmtCtx->nb_streams; i++) {

		AVStream *in_stream = ifmtCtx->streams[i];
		if (in_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			video_index = i;
		}
		
	}
	//int den = ifmtCtx->streams[0]->avg_frame_rate.den;
	//int num = ifmtCtx->streams[0]->avg_frame_rate.num;
	int den = ifmtCtx->streams[video_index]->r_frame_rate.den;
	int num = ifmtCtx->streams[video_index]->r_frame_rate.num;
	float fps = (float)num / den;
	unsigned int splitVideoSize = fps*splitSeconds;
	string save_name;
	save_name = outputFileName.substr(0, outputFileName.find_last_of("."));
	string temp_name = save_name + "0"+suffixName;
	avformat_alloc_output_context2(&ofmtCtx, NULL, NULL, temp_name.c_str());
	if (!ofmtCtx) {
		return false;
	}
	if (!writeVideoHeader(ifmtCtx, ofmtCtx, temp_name))
	{
		return false;
	}
	vector<uint64_t> vecKeyFramePos;
	uint64_t frame_index = 0;
	uint64_t keyFrame_index = 0;
	int frameCount = 0;
	//��ȡ�ָ�㸽���Ĺؼ�֡λ��
	while (1)
	{
		++frame_index;
		ret = av_read_frame(ifmtCtx, &readPkt);
		if (ret < 0)
		{
			break;
		}
		//���ˣ�ֻ������Ƶ��
		if (readPkt.stream_index == video_index){
			
			++frameCount;
			if (readPkt.flags&AV_PKT_FLAG_KEY)
			{
				keyFrame_index = frame_index;
			}
			if (frameCount>splitVideoSize)
			{
				vecKeyFramePos.push_back(keyFrame_index);
				frameCount = 0;
			}
		}
		av_packet_unref(&readPkt);
	}

	avformat_close_input(&ifmtCtx);
	ifmtCtx = NULL;
	//Ϊ�����»�ȡavformatcontext
	if ((ret = avformat_open_input(&ifmtCtx, inputFileName.c_str(), 0, 0)) < 0) {
		return -1;
	}

	if ((ret = avformat_find_stream_info(ifmtCtx, 0)) < 0) {
		return -1;
	}
	int number = 0;
	av_init_packet(&splitKeyPacket);
	splitKeyPacket.data = NULL;
	splitKeyPacket.size = 0;
	if (vecKeyFramePos.empty()){
		vecKeyFramePos.push_back(frame_index);
	}
	vector<uint64_t>::iterator keyFrameIter = vecKeyFramePos.begin();

	keyFrame_index = *keyFrameIter;
	++keyFrameIter;
	frame_index = 0;
	int64_t lastPts = 0;
	int64_t lastDts = 0;
	int64_t prePts = 0;
	int64_t preDts = 0;
	while (1)
	{
		++frame_index;
		ret = av_read_frame(ifmtCtx, &readPkt);
		if (ret < 0)
		{
			break;
		}

		av_packet_rescale_ts(&readPkt, ifmtCtx->streams[readPkt.stream_index]->time_base, ofmtCtx->streams[readPkt.stream_index]->time_base);
		prePts = readPkt.pts;
		preDts = readPkt.dts;
		readPkt.pts -= lastPts;
		readPkt.dts -= lastDts;
		if (readPkt.pts < readPkt.dts)
		{
			readPkt.pts = readPkt.dts + 1;
		}
		//Ϊ�ָ�㴦�Ĺؼ�֡Ҫ���п���
		if (readPkt.flags&AV_PKT_FLAG_KEY&&frame_index == keyFrame_index)
		{
			av_copy_packet(&splitKeyPacket, &readPkt);
		}
		else{
			ret = av_interleaved_write_frame(ofmtCtx, &readPkt);
			if (ret < 0) {
				//break;

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
			av_write_trailer(ofmtCtx);
			avio_close(ofmtCtx->pb);
			avformat_free_context(ofmtCtx);
			vecResultName.push_back(temp_name);
			++number;
			char num[10];
			_itoa_s(number, num, 10);
			temp_name = save_name + num + suffixName;
			avformat_alloc_output_context2(&ofmtCtx, NULL, NULL, temp_name.c_str());
			if (!ofmtCtx) {
				ret = AVERROR_UNKNOWN;
				return false;
			}
			if (!writeVideoHeader(ifmtCtx, ofmtCtx, save_name + num + suffixName))
			{
				return false;
			}
			splitKeyPacket.pts = 0;
			splitKeyPacket.dts = 0;
			//����һ����Ƭ���Ĺؼ�֡д�뵽��һ����Ƭ����ʼ������֤��һ����Ƭ�Ŀ�ͷΪI֡
			ret = av_interleaved_write_frame(ofmtCtx, &splitKeyPacket);
		}

		
		av_packet_unref(&readPkt);

	}
	av_packet_unref(&splitKeyPacket);
	av_write_trailer(ofmtCtx);
	avformat_close_input(&ifmtCtx);
	avio_close(ofmtCtx->pb);
	avformat_free_context(ofmtCtx);
	vecResultName.push_back(temp_name);
	return true;
}

bool SplitVideo::writeVideoHeader(AVFormatContext *ifmtCtx, AVFormatContext *ofmtCtx, string out_filename)
{
	AVOutputFormat *ofmt = NULL;
	int ret;

	
	ofmt = ofmtCtx->oformat;
	for (int i = 0; i < ifmtCtx->nb_streams; i++) {
		
		//���������������������Create output AVStream according to input AVStream��
		AVStream *in_stream = ifmtCtx->streams[i];
		if (in_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			video_index = i;
		}
		AVStream *out_stream = avformat_new_stream(ofmtCtx, in_stream->codec->codec);
		if (!out_stream) {
			ret = AVERROR_UNKNOWN;
			return false;
		}
		//����AVCodecContext�����ã�Copy the settings of AVCodecContext��
		ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
		if (ret < 0) {
			return false;
		}
		out_stream->codec->codec_tag = 0;
		if (ofmtCtx->oformat->flags & AVFMT_GLOBALHEADER)
			out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
		
	}
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&ofmtCtx->pb, out_filename.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0) {
			return false;
		}
	}
	ret = avformat_write_header(ofmtCtx, NULL);
	if (ret < 0){
		return false;
	}
	return true;
}