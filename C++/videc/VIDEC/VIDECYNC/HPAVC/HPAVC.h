#ifndef __HPAVC_H__
#define __HPAVC_H__


#include "mfxvideo.h"


mfxStatus	 MFX_Init(void);
int			 GetIntelAdapterNum(void);
mfxIMPL	 GetIntelAdapterIMPL(void);

#endif //__HPAVC_H__