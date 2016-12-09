#pragma once
#include <string>
#include <vector>

extern "C"
{
#include "libavformat\avformat.h"
}
using std::vector;
using std::string;

class SplitVideo
{
public:
	/************************************************************************/
	/* ����infile: ������Ƶ�ļ���(�����ļ�·��),outfile: �����Ƶ�ļ���(����·��)*/
	/************************************************************************/
	SplitVideo(string infile,string outfile);
	
		
	~SplitVideo();
	

	/************************************************************************/
	/*            ִ�з�Ƭ������splitSecondsΪ��Ƭ��Ƶ��ʱ������λ:�룩     */
	/************************************************************************/
	bool executeSplit(unsigned int splitSeconds);
	vector<string> getResultName();
private:
	uint64_t splitFrameSize;
	vector<string> vecResultName;
	string suffixName;
	string inputFileName;
	string outputFileName;
	int video_index,audio_index;
	AVFormatContext *ifmtCtx=NULL, *ofmtCtx=NULL;
	bool writeVideoHeader(AVFormatContext *ifmt_ctx, AVFormatContext *ofmt_ctx, string out_filename);
	void flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index);
	bool getInputFormatCtx(AVFormatContext *ifmt_ctx, string inputfile);
};

