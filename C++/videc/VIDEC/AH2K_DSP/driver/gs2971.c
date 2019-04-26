#include <edc.h>

#include "gs2971.h"
#include "hdedc.h"
/*++
gs2971 配置
--*/
static Int GS2971_ctrl(EDC_Handle handle, Uns cmd, Arg arg);
static EDC_Handle GS2971_open(String devName, Arg optArg);

EDC_Fxns GS2971_Fxns = {
    GS2971_open,
    INV,
    GS2971_ctrl                  
};

/*++
(rw)000h-001h 
(rocw)002h-003h
(r)004h-007h
(rw)008h-00ch
(rw)00fh-018h
(r)019h-01ch
(rw)01dh-01eh
(r)01fh-023h
(w)024h
(r)025h
(rw)026h
(rw)037h-039h
(rw)06ch-06dh
(rw)073h
--*/
Bool GS2971_init()
{
	if (GSPI_init())
	{
		GS2971_reset(0);
		return 1;
	}
	return 0;
}

typedef struct _VD_STD
{
	Uns cx;
	Uns cy;
}VD_STD;

static VD_STD theVdStdMap[]=
{
	{1280,720},	/*00h*/
	{1440,720},	/*01h*/
	{1280,720},	/*02h*/
	{2880,720},	/*03h*/
	{1280,720},	/*04h*/
	{1728,720},	/*05h*/
	{1280,720},	/*06h*/
	{3456,720},	/*07h*/
	{1280,720},	/*08h*/
	{3600,720},	/*09h*/

	{1920,1080},	/*0ah*/
	{1920,1080},	/*0bh*/
	{1920,1080},	/*0ch*/
	{1920,1080},	/*0dh*/
	{2304,1080},	/*0eh*/
	{2304,1080},	/*0fh*/
	{1920,1080},	/*10h*/
	{1920,1080},	/*11h*/
	{2400,1080},	/*12h*/
	{2400,1080},	/*13h*/

	{1920,1080},	/*14h*/
	{1920,1035},	/*15h*/
	/*SD*/	
	{720,488},		/*16h*/
	{720,488},		/*17h*/
	{720,576},		/*18h*/
	{720,488},		/*19h*/
	{1440,625},		/*1ah*/
	{1440,507},		/*1bh*/

};

static Uns sdi_last_input = 0xff;

static inline Uns _getInput(void)
{
	Uns input;

	GSPI_read(6, &input, 1);
	input = ((input & 0x3f00) >> 8);

	return input;
}

//
// return: 0 is input unchanging, 1 is input Change
//
Int GS2971_isChange(Int devId)
{
	//Uns buf[1];
	Uns vd_std;

	//GSPI_read(6, buf, sizeof(buf) / sizeof(buf[0]));
	vd_std = _getInput();

	if (sdi_last_input != vd_std)
	{
		sdi_last_input = vd_std;
		return 1;
	}

	return 0;
}

//
// return: 0 is no signal, 1 is have signal
//
Int GS2971_detect(Int devId, Arg arg)
{
	//Uns buf[1];
	Uns vd_std;
	GS2971_ConfParams *params = (GS2971_ConfParams *)arg;

	//GSPI_read(6, buf, sizeof(buf) / sizeof(buf[0]));
	//vd_std = ((buf[0] & 0x3F00) >> 8);
	vd_std = _getInput();

	params->hSize = 0;
	params->vSize = 0;
	if (vd_std < sizeof(theVdStdMap) / sizeof(theVdStdMap[0]))
	{
		if (vd_std == 0x0A || vd_std == 0x0C || vd_std == 0x15 || vd_std == 0x14 ||
		    vd_std == 0x16 || vd_std == 0x18 || vd_std == 0x17)
			params->interlaced = 1;
		else
			params->interlaced = 0;
		params->hSize = theVdStdMap[vd_std].cx;
		params->vSize = theVdStdMap[vd_std].cy;
		params->hProch = 0;
		params->vProch = 2;
	
		return 1;
	}
	else
	{
		Uns lines;
		Uns pixels;

		GSPI_read(0x1F, &pixels, 1);
		GSPI_read(0x22, &lines, 1);

		params->hSize = pixels & 0x3fff;/*13-0 active  words per line*/
		params->vSize = lines & 0x7ff;/*10-0*/

		if(params->vSize>1080)			
			params->vSize=1080;

		if(abs(params->vSize-720)<10)
			params->vSize=720;

		params->interlaced= lines & 0x800 ? 1 : 0;
		params->hProch=2;
		params->vProch=2;

		if(lines && pixels)
  		{
			return 1;
		}
	}

	return 0;
}

void GS2971_reset(Int devId)
{
//	VP_FSET(PCR2,PEREN,1);		/*使能VP2*/
//	VP_FSET(PFUNC1,PFUNC22,1);	/*VP1CTL2设置为GPIO*/
//	VP_FSET(PDIR1,PDIR22,1);	/*VP1CTL2设置为Output*/
//	VP_FSET(PDOUT1,PDOUT22,0);	
//	VP_FSET(PDOUT1,PDOUT22,1);	/*VP1CTL2设置为高*/
}

static EDC_Handle GS2971_open(String devName, Arg optArg)
{
	return (EDC_Handle)1;
}

static Int GS2971_ctrl(EDC_Handle handle, Uns cmd, Arg arg)
{
	Int devId = (Int)handle;
    switch (cmd)
    {
		case EDC_EXT_INIT:
			GS2971_init();
			break;

		case EDC_EXT_GETINPUT:
		{
			GS2971_ConfParams params;
			*(Int *)arg = GS2971_detect(devId, (Arg)&params);
			break;
		}
		case EDC_EXT_GETSTATUS:
			*(Int *)arg = GS2971_isChange(devId);
			break;

		case EDC_EXT_GETRES:
			GS2971_detect(devId, arg);
			break;

		case EDC_EXT_RESET:
			GS2971_reset(devId);
			break;

        case EDC_EXT_CONFIG:
			break;
	}

	return EDC_SUCCESS;
}

