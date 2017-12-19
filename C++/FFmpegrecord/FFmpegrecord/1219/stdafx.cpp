// stdafx.cpp : 只包括标准包含文件的源文件
// FFmpegrecord.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用


void header_free(struct header *header)
{
	free(header->data);
}

void free_avformat(struct ffmpeg_mux *ffm)
{
	if (ffm->output) {
		if ((ffm->output->oformat->flags & AVFMT_NOFILE) == 0)
			avio_close(ffm->output->pb);

		avformat_free_context(ffm->output);
		ffm->output = NULL;
	}

	if (ffm->audio_streams) {
		free(ffm->audio_streams);
	}

	ffm->video_stream = NULL;
	ffm->audio_streams = NULL;
	ffm->num_audio_streams = 0;
}

void ffmpeg_mux_free(struct ffmpeg_mux *ffm)
{
	if (ffm->initialized) {
		av_write_trailer(ffm->output);
	}

	free_avformat(ffm);

	header_free(&ffm->video_header);

	if (ffm->audio_header) {
		for (int i = 0; i < ffm->params.tracks; i++) {
			header_free(&ffm->audio_header[i]);
		}

		free(ffm->audio_header);
	}

	if (ffm->audio) {
		free(ffm->audio);
	}

	memset(ffm, 0, sizeof(*ffm));
}


void Coverstringtochar(std::string strSource, char*destr)
{
	int len = strSource.length();
	destr = (char *)malloc((len + 1)*sizeof(char));
	strSource.copy(destr, len, 0);
	
}

//获取系统的当前时间单位微秒(us)
int64_t GetSysTimeMicos()
{
#ifdef _WIN32
#define EPICHFILETIME (116444736000000000UL)
	FILETIME ft;
	LARGE_INTEGER li;
	int64_t tt = 0;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	tt = (li.QuadPart - EPICHFILETIME) / 10;
	return tt;
	//return ::timeGetTime();
#else
	timeval tv;
	gettimeofday(&tv, 0);
	return (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
#endif // _WIN32
	return 0;
}