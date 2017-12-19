// testffmpeg.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "NetStream.h"
int _tmain(int argc, _TCHAR* argv[])
{
	IFFmpegrecord*precord = IFFmpegrecord::Create();
	if (precord)
	{
		std::string strfilename = "d:\\ffmpegnet.mp4";
		precord->Open(strfilename.c_str());
	}


	CNetStream* pNetSream = new CNetStream();
	if (pNetSream)
	{
		pNetSream->startNete();
		Sleep(9000);
		pNetSream->SetReadStreamNotify(precord);
		pNetSream->openNete();
	}

	while (1)
	{
	}
	return 0;
}

