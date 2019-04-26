// AVDeinterlace.cpp: implementation of the AVDeinterlace class.
//
//////////////////////////////////////////////////////////////////////

#include <afxwin.h>
#include "AVDeinterlace.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IXDeinterlace*CreateIXDeinterlace(int nWidth,int nHeight)
{
	NEWRECORD3( AVDeinterlace*,pAVDeinterlace ,new AVDeinterlace() )
	NULLReturnRet( pAVDeinterlace , pAVDeinterlace )  
	if (!pAVDeinterlace->Connect(nWidth,nHeight))
	{
		pAVDeinterlace->ReleaseConnections();
		delete pAVDeinterlace;
		pAVDeinterlace=NULL;
	}
	return pAVDeinterlace;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVDeinterlace::AVDeinterlace()
{
	m_pContext=NULL;
	AVMPlayer::pp_mode_defaults(m_PPMode);

	m_nWidth=320;
	m_nHeight=240;
}

AVDeinterlace::~AVDeinterlace()
{

}

bool AVDeinterlace::Connect(int nWidth,int nHeight)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	unsigned int nCPUFlags=AVMPlayer::ppCpuCaps(AVMPlayer::m_cpu_flags);

	m_pContext=videcavc_pp_open(nWidth,nHeight,nCPUFlags);

	return (m_pContext!=NULL);
}

void AVDeinterlace::ReleaseConnections(void)
{
	if(m_pContext)
	{
		videcavc_pp_close(m_pContext);
		m_pContext=NULL;
	}
}


bool AVDeinterlace::Deinterlace(uint8_t * src[3], int srcStride[3], uint8_t * dst[3],int dstStride[3],int horizontalSize, int verticalSize)
{
	if (m_pContext)
	{
		m_PPMode.lumMode=m_PPMode.chromMode=LINEAR_BLEND_DEINT_FILTER;
		//m_PPMode.lumMode=m_PPMode.chromMode=CUBIC_IPOL_DEINT_FILTER;
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