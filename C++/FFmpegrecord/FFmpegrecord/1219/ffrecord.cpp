#include "stdafx.h"
#include "ffrecord.h"


cffrecord::cffrecord()
{
	m_pffm = NULL;
	m_emrecordST = RECORD_DISABLE;
}


cffrecord::~cffrecord()
{

}

int cffrecord::Init_Record(std::string stroutputfile, bool has_video, int tracks, std::string vcodec, int vbitrate, int gop, int width, int height, int fps_num, int fps_den, std::string acodec, std::string muxer_setting)
{
	if (NULL != m_pffm)
	{
		delete m_pffm;
		m_pffm = NULL;
	}
	m_pffm = new struct ffmpeg_mux();
	if (m_pffm)
	{
		Coverstringtochar(stroutputfile, m_pffm->params.file);
		m_pffm->params.has_video = has_video ? 1 : 0;
		m_pffm->params.tracks = tracks;
		Coverstringtochar(vcodec,m_pffm->params.vcodec);
		m_pffm->params.vbitrate = vbitrate;
		m_pffm->params.gop = gop;
		m_pffm->params.width = width;
		m_pffm->params.height = height;
		m_pffm->params.fps_num = fps_num/*30*/;
		m_pffm->params.fps_den = fps_den/*1*/;
		Coverstringtochar(acodec, m_pffm->params.acodec);
		Coverstringtochar(muxer_setting, m_pffm->params.muxer_settings);



		//Ä¬ÈÏÖµ

		m_pffm->params.vbitrate = 1024;
		m_pffm->params.gop = gop;
		m_pffm->params.fps_num = 30;
		m_pffm->params.fps_den = 1;

		std::string strcodec = "h264";
		Coverstringtochar(strcodec, m_pffm->params.vcodec);
		strcodec = "aac";
		Coverstringtochar(strcodec, m_pffm->params.acodec);

		struct audio_params *audio = (audio_params *)calloc(1, sizeof(*audio));
		if (audio)
		{
			audio->name = "simple_aac";
			audio->abitrate = 160;
			audio->sample_rate = 48000;
			audio->channels = 2;
		}
		m_pffm->audio = audio;



	}

	return 1;
}

int cffrecord::writepacket()
{
	return 1;
}

int cffrecord::init_mux_context()
{
	return 1;
}
