
#include <afxwin.h>
#include "AVMPlayer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern "C"
{
	// cpu_flag detection helper functions 
	extern int check_cpu_features(void);
	extern void sse_os_trigger(void);
	extern void sse2_os_trigger(void);
}


int AVMPlayer::m_cpu_flags=0;

void VIDEC_PostProcInit(void)
{
	int cpu_flags=0;
	cpu_flags=check_cpu_features();
	videcavc_pp_init( cpu_flags&FF_CPU_MMX,
			    cpu_flags&FF_CPU_MMXEXT,
				cpu_flags&FF_CPU_3DNOW,
				cpu_flags&FF_CPU_3DNOWEXT,
				cpu_flags&FF_CPU_SSE,
				cpu_flags&FF_CPU_SSE2);

	AVMPlayer::m_cpu_flags=cpu_flags;
}

void VIDEC_PostProcTerminate(void)
{

}


int AVMPlayer::getPPmode(int currentq)
{
	int result=0;

	int ppqual=currentq;
	if (ppqual<0) ppqual=0;
	if (ppqual>PP_QUALITY_MAX) ppqual=PP_QUALITY_MAX;
	static const int ppPresets[1+PP_QUALITY_MAX]=
	{
		 0,
		 LUM_H_DEBLOCK,
		 LUM_H_DEBLOCK|LUM_V_DEBLOCK,
		 LUM_H_DEBLOCK|LUM_V_DEBLOCK|CHROM_H_DEBLOCK,
		 LUM_H_DEBLOCK|LUM_V_DEBLOCK|CHROM_H_DEBLOCK|CHROM_V_DEBLOCK,
		 LUM_H_DEBLOCK|LUM_V_DEBLOCK|CHROM_H_DEBLOCK|CHROM_V_DEBLOCK|LUM_DERING,
		 LUM_H_DEBLOCK|LUM_V_DEBLOCK|CHROM_H_DEBLOCK|CHROM_V_DEBLOCK|LUM_DERING|CHROM_DERING
	};
	result=ppPresets[ppqual];
	return result;
}


int AVMPlayer::ppCpuCaps(int cpu_flags)
{
	int cpu=0;
	if (cpu_flags&FF_CPU_MMX)    cpu|=PP_CPU_CAPS_MMX;
	if (cpu_flags&FF_CPU_MMXEXT) cpu|=PP_CPU_CAPS_MMX2;
	if (cpu_flags&FF_CPU_3DNOW)  cpu|=PP_CPU_CAPS_3DNOW;

	return cpu;
}
void AVMPlayer::pp_mode_defaults(PPMode &ppMode)
{
	ppMode.lumMode=0;
	ppMode.chromMode=0;
	ppMode.maxTmpNoise[0]=700;
	ppMode.maxTmpNoise[1]=1500;
	ppMode.maxTmpNoise[2]=3000;
	ppMode.maxAllowedY=234;
	ppMode.minAllowedY=16;
	ppMode.baseDcDiff=256/8;
	ppMode.flatnessThreshold=56-16-1;
	ppMode.maxClippedThreshold=0.01f;
	ppMode.error=0;
	ppMode.forcedQuant=0;
}

