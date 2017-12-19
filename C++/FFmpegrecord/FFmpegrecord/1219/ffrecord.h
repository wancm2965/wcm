#pragma once
class cffrecord
{
public:
	cffrecord();
	~cffrecord();


	int Init_Record(std::string stroutputfile,bool has_video,int tracks,std::string vcodec,int vbitrate,int gop,int width,int height,int fps_num,int fps_den,std::string acodec,std::string muxer_setting);



	int writepacket();

private:

	int init_mux_context();
	

private:
	struct ffmpeg_mux* m_pffm;
	_Record_STATUE   m_emrecordST;
		
};

