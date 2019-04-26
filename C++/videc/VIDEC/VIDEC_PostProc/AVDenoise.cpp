// AVDenoise.cpp: implementation of the AVDenoise class.
//
//////////////////////////////////////////////////////////////////////

#include <afxwin.h>
#include "AVDenoise.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IXDenoise*CreateIXDenoise(int nWidth,int nHeight)
{
	NEWRECORD3( AVDenoise*,pAVDenoise,new AVDenoise() )
	NULLReturnRet( pAVDenoise , pAVDenoise )  
	if (!pAVDenoise->Connect(nWidth,nHeight))
	{
		pAVDenoise->ReleaseConnections();
		delete pAVDenoise;
		pAVDenoise=NULL;
	}
	return pAVDenoise;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVDenoise::AVDenoise()
{
	m_pContext=NULL;
	AVMPlayer::pp_mode_defaults(m_PPMode);

	m_nWidth=320;
	m_nHeight=240;
}

AVDenoise::~AVDenoise()
{

}

bool AVDenoise::Connect(int nWidth,int nHeight)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	unsigned int nCPUFlags=AVMPlayer::ppCpuCaps(AVMPlayer::m_cpu_flags);
	m_pContext=videcavc_pp_open(m_nWidth,m_nHeight,nCPUFlags);
	return (m_pContext!=NULL);
}

void AVDenoise::ReleaseConnections(void)
{
	if(m_pContext)
	{
		videcavc_pp_close(m_pContext);
		m_pContext=NULL;
	}
}


bool AVDenoise::Denoise(uint8_t * src[3], int srcStride[3], uint8_t * dst[3],int dstStride[3],int horizontalSize, int verticalSize)
{
	if (m_pContext)
	{
		m_PPMode.lumMode=m_PPMode.chromMode=TEMP_NOISE_FILTER;

		try{
			videcavc_pp_process(src,srcStride,dst,dstStride,horizontalSize,verticalSize,NULL, 0,&m_PPMode,m_pContext, PP_FORMAT_420);
			return true;
		}
		catch(...)
		{
			videcavc_pp_close(m_pContext);
			m_pContext=NULL;

			unsigned int nCPUFlags=AVMPlayer::ppCpuCaps(AVMPlayer::m_cpu_flags);
			m_pContext=videcavc_pp_open(m_nWidth,m_nHeight,nCPUFlags);
			
			return false;
		}
	}
	else
	{
		unsigned int nCPUFlags=AVMPlayer::ppCpuCaps(AVMPlayer::m_cpu_flags);
		m_pContext=videcavc_pp_open(m_nWidth,m_nHeight,nCPUFlags);
		return false;
	}
}