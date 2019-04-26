#ifndef _GS2971_H_
#define _GS2971_H_

#include <std.h>
#include "gspi.h"


typedef struct _GS2971_ConfParams
{
	Int	hSize;
	Int vSize;
	Int hProch;
	Int vProch;
	
    Bool interlaced; 
}GS2971_ConfParams;


Bool GS2971_init();
Int GS2971_detect(Int devId, Arg arg);
void GS2971_reset(Int devId);



#endif /*_GS2971_H_*/

