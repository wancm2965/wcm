/***********************************************************************
        Copyright (c) 2003 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#include "rvstdio.h"
#include "prnutils.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (RV_LOGMASK & RV_LOGLEVEL_DEBUG)
RVINTAPI void RVCALLCONV
printHexBuff(RvUint8* buf, int len, RvLogSource * msa)
{
    if (RvLogIsSelected(msa, RV_LOGLEVEL_DEBUG))
    {
        char tempBuff[128];
        int tempLen;
        int i ,j;

        for (i = 0; i < len; i++)
        {
            /* this loop was checked and there should be no problems with buffer
            overruns here*/
            tempLen = RvSprintf(tempBuff, "%5.5d   ", i);
            for (j = i; j < i+16; j++)
                if (j < len)
                {
                    tempLen += RvSprintf(tempBuff+tempLen, "%2.2x ", buf[j]);
                }
                else
                {
                    tempLen += RvSprintf(tempBuff+tempLen, "   ");
                }
            tempLen += RvSprintf(tempBuff+tempLen, "   |");
            for (j=i;j<RvMin(i+16,len);j++)
                tempLen += RvSprintf(tempBuff+tempLen, "%c", (buf[j]>=' ')?buf[j]:'.');
            tempLen += RvSprintf(tempBuff+tempLen, "|");
            RvLogTextDebug(msa, "%s", tempBuff);
            i = j-1;
        }
    }
}
#endif  /* (RV_LOGMASK & RV_LOGLEVEL_DEBUG) */


#ifdef __cplusplus
}
#endif



