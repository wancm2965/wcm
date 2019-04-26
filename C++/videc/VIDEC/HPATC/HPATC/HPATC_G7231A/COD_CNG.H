/*
**
** File:        "cod_cng.h"
**
** Description:     Function prototypes for "cod_cng.c"
**
*/
/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/

void Init_Cod_Cng(CODSTATDEF*pCodStat);
void Cod_Cng(CODSTATDEF*pCodStat, Word16 *DataExc, Word16 *Ftyp, LINEDEF *Line, Word16 *QntLpc);
void Update_Acf(CODSTATDEF*pCodStat, Word16 *Acfsf, Word16 *Shsf);

