#include <IMXEC/IMXEC_AVCONCall.h>
#include <IMXEC/IMXEC_VideoSetting.h>
#include <IMXEC/IMXEC_AudioSetting.h>
#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Engine.h>
#include "AVCONCall.h"

IMXEC_AVCONCall::IMXEC_AVCONCall(void)
{

}

IMXEC_AVCONCall::~IMXEC_AVCONCall(void)
{

}


int IMXEC_AVCONCall::Login(const char*cszUserID,const char*cszPassword,const char*cszServerIP,unsigned short nServerPort)
{
	AVCONCall::Instance().AddAVCONCall(this);
	return AVCONCall::Instance().Login(cszUserID,cszPassword,cszServerIP,nServerPort);
}

void IMXEC_AVCONCall::Logout(void)
{
	AVCONCall::Instance().Logout();
	AVCONCall::Instance().RemoveAVCONCall(this);
}

int IMXEC_AVCONCall::MakeCall(const char*cszUserID)
{
	return AVCONCall::Instance().MakeCall(cszUserID);
}

int IMXEC_AVCONCall::AcceptCall(const char*cszUserID)
{
	return AVCONCall::Instance().AcceptCall(cszUserID);
}

void IMXEC_AVCONCall::EndCall(const char*cszUserID)
{
	AVCONCall::Instance().EndCall(cszUserID);
}

int IMXEC_AVCONCall::SetCallLocalWnd(const char*cszUserID,HWND hWnd)
{
	return AVCONCall::Instance().SetCallLocalWnd(cszUserID,hWnd);
}

int IMXEC_AVCONCall::SetCallLocalWndPos(const char*cszUserID,int x,int y,int cx,int cy)
{
	return AVCONCall::Instance().SetCallLocalWndPos(cszUserID,x,y,cx,cy);;
}

int IMXEC_AVCONCall::SetCallRemoteWnd(const char*cszUserID,HWND hWnd)
{
	return AVCONCall::Instance().SetCallRemoteWnd(cszUserID,hWnd);
}

int IMXEC_AVCONCall::SetCallRemoteWndPos(const char*cszUserID,int x,int y,int cx,int cy)
{
	return AVCONCall::Instance().SetCallRemoteWndPos(cszUserID,x,y,cx,cy);
}

void IMXEC_AVCONCall::SetVideoDev(int nDevID,IMXEC_AVCONCall::VIDEO_FORMAT nFormat,int nBitrate,int nCbrOrVbr,int nFrameRate)
{
	VIDEO_FORMAT_TYPE nFormatType=VIDEO_FORMAT_CIF;
	switch (nFormat)
	{
	case VF_QCIF:
		{
			nFormatType=VIDEO_FORMAT_QCIF;
		}
		break;
	case VF_QVGA:
		{
			nFormatType=VIDEO_FORMAT_QVGA;
		}
		break;
	case VF_CIF:
		{
			nFormatType=VIDEO_FORMAT_CIF;
		}
		break;
	case VF_4CIF:
		{
			nFormatType=VIDEO_FORMAT_4CIF;
		}
		break;
	case VF_720P:
		{
			nFormatType=VIDEO_FORMAT_720P;
		}
		break;
	case VF_1080I:
		{
			nFormatType=VIDEO_FORMAT_1080I;
		}
		break;
	case VF_1080P:
		{
			nFormatType=VIDEO_FORMAT_1080P;
		}
		break;
	}

	IMXEC_VideoSetting::SetMainVideoDevParam(nDevID,nFrameRate,nBitrate,VIDEC_CODEC_H264_SVC,nFormatType);
}

void IMXEC_AVCONCall::SetAudioDev(int nMicDevID,int nSpkDevID,int nAEC,int nVAD,int nAGC,int nNS)
{
	IMXEC_AudioSetting::SetAudioDevParam(X_AUDIO_CODEC_G7221_14_24);

	AUDEC_GetEngine().AUDEC_SetMicDev(nMicDevID);
	AUDEC_GetEngine().AUDEC_SetSpeakerDev(nSpkDevID);
	AUDEC_GetEngine().AUDEC_SetAEC(nAEC!=0?1:0);
	AUDEC_GetEngine().AUDEC_SetVAD(nVAD);
	AUDEC_GetEngine().AUDEC_SetPostAGC(nAGC!=0?2:0);
	AUDEC_GetEngine().AUDEC_SetPreAGC(nAGC);
	AUDEC_GetEngine().AUDEC_SetPreNS(nNS!=0?3:0);
}

int IMXEC_AVCONCall::GetVideoDevCount(void)
{
	return VIDEC_GetDevCount();
}

int IMXEC_AVCONCall::GetVideoDevName(int nDevID,char*szName,int nMaxCount)
{
	return VIDEC_GetDevName(nDevID,szName,nMaxCount);
}

int IMXEC_AVCONCall::GetMicDevCount(void)
{
	return AUDEC_Engine::AUDEC_GetMicNumDevs();
}

int IMXEC_AVCONCall::GetMicDevName(int nDevID,char*szName,int nMaxCount)
{
	return AUDEC_Engine::AUDEC_GetMicDevDesc(nDevID,szName);;
}

int IMXEC_AVCONCall::GetSpkDevCount(void)
{
	return AUDEC_Engine::AUDEC_GetSpeakerNumDevs();
}

int IMXEC_AVCONCall::GetSpkDevName(int nDevID,char*szName,int nMaxCount)
{
	return AUDEC_Engine::AUDEC_GetSpeakerDevDesc(nDevID,szName);
}

int IMXEC_AVCONCall::Init(void)
{
	::CoInitialize(NULL);
	AUDEC_Engine::AUDEC_Init(0);
	VIDEC_Init();

	return 0;
}

void IMXEC_AVCONCall::Terminate(void)
{
	VIDEC_Terminate();
	AUDEC_Engine::AUDEC_Terminate();
	::CoUninitialize();
}

