
#include <afxwin.h>
#include "AVPostProcess.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IXPostProcess*CreateIXPostProcess(int nWidth,int nHeight,int nLevel)
{
	AVPostProcess*pAVPostProcess=new AVPostProcess();
	if (!pAVPostProcess->Connect(nWidth,nHeight,nLevel))
	{
		pAVPostProcess->ReleaseConnections();
		delete pAVPostProcess;
		pAVPostProcess=NULL;
	}
	return pAVPostProcess;
}



AVPostProcess::AVPostProcess(void)
{
	m_pPPContext=NULL;
	AVMPlayer::pp_mode_defaults(m_PPMode);
	m_PPMode.lumMode=m_nPPMode&15;
	m_PPMode.chromMode=(m_nPPMode>>4)&15;

	m_nWidth=320;
	m_nHeight=240;

	SetLevel(6);
}

AVPostProcess::~AVPostProcess(void)
{
}

bool AVPostProcess::Connect(int nWidth,int nHeight,int nLevel)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	unsigned int nCPUFlags=AVMPlayer::ppCpuCaps(AVMPlayer::m_cpu_flags);
	m_pPPContext=videcavc_pp_open(m_nWidth,m_nHeight,nCPUFlags);

	SetLevel(nLevel);

	return (m_pPPContext!=NULL);
}

void AVPostProcess::ReleaseConnections(void)
{
	if(m_pPPContext)
	{
		videcavc_pp_close(m_pPPContext);
		m_pPPContext=NULL;
	}
}

bool AVPostProcess::PostProcess(unsigned char * src[3], int srcStride[3], unsigned char * dst[3],int dstStride[3],int horizontalSize, int verticalSize,char*qscale_table,int qstride)
{
	if (m_pPPContext)
	{
		try{
			videcavc_pp_process(src,srcStride,dst,dstStride,horizontalSize,verticalSize,(int8_t*)qscale_table,qstride,&m_PPMode,m_pPPContext, 1);
			return true;
		}
		catch(...)
		{
			videcavc_pp_close(m_pPPContext);
			m_pPPContext=NULL;

			unsigned int nCPUFlags=AVMPlayer::ppCpuCaps(AVMPlayer::m_cpu_flags);
			m_pPPContext=videcavc_pp_open(m_nWidth,m_nHeight,nCPUFlags);
		
			return false;
		}
	}
	else
	{
		unsigned int nCPUFlags=AVMPlayer::ppCpuCaps(AVMPlayer::m_cpu_flags);
		m_pPPContext=videcavc_pp_open(m_nWidth,m_nHeight,nCPUFlags);
		return false;
	}
}

void AVPostProcess::SetLevel(int nLevel)
{
	if(nLevel>6)
		nLevel=6;
	else if (nLevel<0)
		nLevel=0;

	m_nPPMode=AVMPlayer::getPPmode(nLevel);
	m_PPMode.lumMode=m_nPPMode&15;
	m_PPMode.chromMode=(m_nPPMode>>4)&15;


	m_PPMode.baseDcDiff=128;
	m_PPMode.flatnessThreshold=7;
	if (m_PPMode.lumMode&H_DEBLOCK)
		m_PPMode.lumMode=(m_PPMode.lumMode&~H_DEBLOCK)|H_A_DEBLOCK;
	if (m_PPMode.lumMode&V_DEBLOCK)
		m_PPMode.lumMode=(m_PPMode.lumMode&~V_DEBLOCK)|V_A_DEBLOCK;
	if (m_PPMode.chromMode&H_DEBLOCK)
		m_PPMode.chromMode=(m_PPMode.chromMode&~H_DEBLOCK)|H_A_DEBLOCK;
	if (m_PPMode.chromMode&V_DEBLOCK)
		m_PPMode.chromMode=(m_PPMode.chromMode&~V_DEBLOCK)|V_A_DEBLOCK;
}
