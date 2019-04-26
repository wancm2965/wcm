// XCodecInst.cpp: implementation of the XCodecInst class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#else
#include <memory.h>
#endif

#include "AUDEC_CodecID.h"
#include "XCodecInst.h"


#define AUDEC_MAX_CODECS	100

static HPATC_CodecInst g_mapCodecInst[AUDEC_MAX_CODECS];
static bool	g_bListCodecInstInited=false;
static int	g_nCodecCount=0;


HPATC_API bool HPATCGetCodecInstByID(int nID,HPATC_CodecInst*pCodecInst)
{
	return GetCodecInstByID(nID,pCodecInst);
}

static void InitListCodecInst(void)
{
	if (!g_bListCodecInstInited)
	{
		g_bListCodecInstInited=true;
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G711U,0,"G711U",8000,160,64000,"g711u 8khz 64kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G711A,8,"G711A",8000,160,64000,"g711a 8khz 64kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_GSM,3,"GSM",8000,160,13200,"gsm 8khz 13.2kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G7231_63,4,"G723",8000,240,6300,"g723.1 8khz 6.3kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G729,18,"G729",8000,160,8000,"g729 8khz 8kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}

		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_ILBC_15,97,"iLBC",8000,160,15200,"ilbc 8khz 15.2kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}

		//{
		//	HPATC_CodecInst CodecInst={X_AUDIO_CODEC_ILBC_13,97,"iLBC",8000,240,13333,"ilbc 8khz 13.333kbps"};
		//	g_mapCodecInst[g_nCodecCount++]=CodecInst;
		//}

		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_L16_8KHZ,12,"L16/8000",8000,160,128000,"l16 8khz 128kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_L16_16KHZ,12,"L16/16000",16000,320,256000,"l16 16khz 256kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_L16_32KHZ,12,"L16/32000",32000,640,512000,"l16 32khz 512kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_L16_48KHZ,12,"L16/48000",48000,480,768000,"l16 48khz 768kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}

		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_SIREN_G7221_7_24,121,"SIREN_G7221/16000",16000,320,24000,"Siren g722.1 16khz 24kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_SIREN_G7221_7_32,121,"SIREN_G7221/16000",16000,320,32000,"Siren g722.1 16khz 32kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}

		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_SIREN_G7221_14_24,122,"SIREN_G7221/32000",32000,640,24000,"Siren g722.1 32khz 24kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_SIREN_G7221_14_32,122,"SIREN_G7221/32000",32000,640,32000,"Siren g722.1 32khz 32kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_SIREN_G7221_14_48,122,"SIREN_G7221/32000",32000,640,48000,"Siren g722.1 32khz 48kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G7221_7_24,121,"G722_1/16000",16000,320,24000,"g.722.1 16khz 24kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G7221_7_32,121,"G722_1/16000",16000,320,32000,"g.722.1 16khz 32kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}

		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G7221_14_24,122,"G722_1/32000",32000,640,24000,"g.722.1 32khz 24kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G7221_14_32,122,"G722_1/32000",32000,640,32000,"g.722.1 32khz 32kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G7221_14_48,122,"G722_1/32000",32000,640,48000,"g.722.1 32khz 48kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_NB_475,120,"G7222/8000",8000,160,4750,"g722.2 8khz 4.75kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_NB_515,120,"G7222/8000",8000,160,5150,"g722.2 8khz 5.15kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_NB_59,120,"G7222/8000",8000,160,5900,"g722.2 8khz 5.9kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_NB_67,120,"G7222/8000",8000,160,6700,"g722.2 8khz 6.7kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_NB_74,120,"G7222/8000",8000,160,7400,"g722.2 8khz 7.4kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_NB_795,120,"G7222/8000",8000,160,7950,"g722.2 8khz 7.95kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_NB_102,120,"G7222/8000",8000,160,10200,"g722.2 8khz 10.2kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_NB_122,120,"G7222/8000",8000,160,12200,"g722.2 8khz 12.2kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}

		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_WB_7,121,"G7222/16000",16000,320,7000,"g722.2 16khz 7kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_WB_9,121,"G7222/16000",16000,320,9000,"g722.2 16khz 9kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_WB_12,121,"G7222/16000",16000,320,12000,"g722.2 16khz 12kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_WB_14,121,"G7222/16000",16000,320,14000,"g722.2 16khz 14kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_WB_16,121,"G7222/16000",16000,320,16000,"g722.2 16khz 16kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_WB_18,121,"G7222/16000",16000,320,18000,"g722.2 16khz 18kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_WB_20,121,"G7222/16000",16000,320,20000,"g722.2 16khz 20kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_WB_23,121,"G7222/16000",16000,320,23000,"g722.2 16khz 23kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AMR_WB_24,121,"G7222/16000",16000,320,24000,"g722.2 16khz 24kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_HIK_G726,123,"G726H",16000,640,24000,"g726.h 16khz 24kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_USER,123,"USER",16000,640,24000,"USER 16khz 24kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_HIK_G722,124,"G722H",16000,640,16000,"g722.h 16khz 16kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AAC,125,"AAC",48000,1024,64000,"AAC LC 48khz 64kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_EVRC,126,"EVRC",8000,160,8550,"EVRC 8khz(0.4kbps-8.55kbps)"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		//{
		//	HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G722_48,127,"G722",16000,320,48000,"g722 16khz 48kpbs"};
		//	g_mapCodecInst[g_nCodecCount++]=CodecInst;
		//}
		//{
		//	HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G722_56,128,"G722",16000,320,56000,"g722 16khz 56kpbs"};
		//	g_mapCodecInst[g_nCodecCount++]=CodecInst;
		//}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G722_64,129,"G722",16000,320,64000,"g722 16khz 64kpbs"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_MP3,130,"MP3",16000,576,24000,"mp3 16khz 24kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}


		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_AUDEC_24,132,"AUDEC/32000",32000,640,24000,"AUDEC 32khz 24kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}

		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_SPEEX_NB,133,"SpeexNB",8000,640,16000,"SpeexNB 8khz 16kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_ADPCM,134,"ADPCM",8000,161,32000,"ADPCM 8khz 32kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_OPUS_FWB_24,135,"OPUS/48000",48000,960,24000,"OPUS 48khz 24kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_OPUS_FWB_48,135,"OPUS/48000",48000,960,48000,"OPUS 48khz 48kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}

		///////////
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G719_32,136,"G719/48000",48000,960,32000,"G.719 48khz 32kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G719_48,136,"G719/48000",48000,960,48000,"G.719 48khz 48kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
		{
			HPATC_CodecInst CodecInst={X_AUDIO_CODEC_G719_64,136,"G719/48000",48000,960,64000,"G.719 48khz 64kbps"};
			g_mapCodecInst[g_nCodecCount++]=CodecInst;
		}
	}
}

int GetCodecCount(void)
{
	InitListCodecInst();
	return g_nCodecCount;
}

bool GetCodecInstByIndex(int nIndex,HPATC_CodecInst*pCodecInst)
{
	InitListCodecInst();

	if (nIndex>=0 && nIndex<g_nCodecCount)
	{
		*pCodecInst=g_mapCodecInst[nIndex];
		return true;
	}

	return false;
}

bool GetCodecInstByID(int nID,HPATC_CodecInst*pCodecInst)
{
	InitListCodecInst();

	for (int nIndex=0;nIndex<g_nCodecCount;++nIndex)
	{
		if (g_mapCodecInst[nIndex].id==nID)
		{
			*pCodecInst=g_mapCodecInst[nIndex];
			return true;
		}
	}

	return false;
}