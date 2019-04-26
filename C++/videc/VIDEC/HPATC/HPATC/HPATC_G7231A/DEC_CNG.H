/*
**
** File:        "dec_cng.h"
**
** Description:     Function prototypes for "dec_cng.c"
**
*/
/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/

void Init_Dec_Cng(DECSTATDEF*pDecStat);
void Dec_Cng( DECSTATDEF*pDecStat, Word16 Ftyp, LINEDEF *Line, Word16 *DataExc,
                                                    Word16 *QntLpc);


