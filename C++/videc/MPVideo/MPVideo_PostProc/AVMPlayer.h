#ifndef _TLIBMPLAYER_H_
#define _TLIBMPLAYER_H_

#include "inttypes.h"
#include "postprocess.h"

#define videcavc_pp_init rave_pp_init
#define videcavc_pp_open rave_pp_open
#define videcavc_pp_close rave_pp_close
#define videcavc_pp_process rave_pp_process

enum
{
	FF_CPU_MMX     =0x00000001,
	FF_CPU_MMXEXT  =0x00000002,
	FF_CPU_SSE     =0x00000004,
	FF_CPU_SSE2    =0x00000008,
	FF_CPU_3DNOW   =0x00000010,
	FF_CPU_3DNOWEXT=0x00000020
};

class AVMPlayer
{
public: 
	static int ppCpuCaps(int cpu_flags);
	static void pp_mode_defaults(PPMode &ppMode);
	static int getPPmode(int currentq);

	static int m_cpu_flags;
};

#endif
