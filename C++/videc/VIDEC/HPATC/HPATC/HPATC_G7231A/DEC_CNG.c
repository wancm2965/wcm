/*
**
** File:            "dec_cng.c"
**
** Description:     Comfort noise generation
**                  performed at the decoder part
**
** Functions:       Init_Dec_Cng()
**                  Dec_Cng()
**
**
*/
/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>

/*
#include "typedef.h"
#include "cst_lbc.h"
#include "tab_lbc.h"
#include "util_lbc.h"
#include "lsp.h"
#include "exc_lbc.h"
#include "basop.h"
#include "util_cng.h"
#include "dec_cng.h"
#include "decod.h"
*/
#include "TYPEDEF.H"
#include "CST_LBC.H"
#include "TAB_LBC.H"
#include "UTIL_LBC.H"
#include "LSP.H"
#include "EXC_LBC.H"
#include "BASOP.H"
#include "UTIL_CNG.H"
#include "DEC_CNG.H"
#include "DECOD.H"
/*
**
** Function:        Init_Dec_Cng()
**
** Description:     Initialize Dec_Cng static variables
**
** Links to text:
**
** Arguments:       None
**
** Outputs:         None
**
** Return value:    None
**
*/
void Init_Dec_Cng(DECSTATDEF*pDecStat)
{
    int i;

    pDecStat->DecCng.PastFtyp = 1;
    pDecStat->DecCng.SidGain = 0;
    for(i=0; i<LpcOrder; i++) pDecStat->DecCng.LspSid[i] = LspDcTable[i] ;
    pDecStat->DecCng.RandSeed = 12345;
    return;
}

/*
**
** Function:           Dec_Cng()
**
** Description:        Receives Ftyp
**                     0  :  for untransmitted frames
**                     2  :  for SID frames
**                     Decodes SID frames
**                     Computes current frame excitation
**                     Computes current frame LSPs
**
** Links to text:
**
** Arguments:
**
**  Word16 Ftyp        Type of silence frame
**  LINEDEF *Line      Coded parameters
**  Word16 *DataExc    Current frame excitation
**  Word16 *QntLpc     Interpolated frame LPC coefficients
**
** Outputs:
**
**  Word16 *DataExc
**  Word16 *QntLpc
**
** Return value:       None
**
*/
void Dec_Cng( DECSTATDEF*pDecStat, Word16 Ftyp, LINEDEF *Line, Word16 *DataExc, Word16 *QntLpc)
{

    Word16 temp;
    int i;

    if(Ftyp == 2) {

 /*
  * SID Frame decoding
  */
        pDecStat->DecCng.SidGain = Dec_SidGain(Line->Sfs[0].Mamp);

        /* Inverse quantization of the LSP */
        Lsp_Inq( pDecStat->DecCng.LspSid, pDecStat->PrevLsp, Line->LspId, 0) ;
    }

    else {

/*
 * non SID Frame
 */
        if(pDecStat->DecCng.PastFtyp == 1) {

 /*
  * Case of 1st SID frame erased : quantize-decode
  * energy estimate stored in pDecStat->DecCng.SidGain
  * scaling factor in pDecStat->DecCng.CurGain
  */
            temp = Qua_SidGain(&pDecStat->DecCng.SidGain, &pDecStat->DecCng.CurGain, 0);
            pDecStat->DecCng.SidGain = Dec_SidGain(temp);
        }
    }


    if(pDecStat->DecCng.PastFtyp == 1) {
        pDecStat->DecCng.CurGain = pDecStat->DecCng.SidGain;
    }
    else {
        pDecStat->DecCng.CurGain = extract_h(L_add( L_mult(pDecStat->DecCng.CurGain,0x7000),
                    L_mult(pDecStat->DecCng.SidGain,0x1000) ) ) ;
    }
    Calc_Exc_Rand(pDecStat->WrkRate,pDecStat->DecCng.CurGain, pDecStat->PrevExc, DataExc,
                    &pDecStat->DecCng.RandSeed, Line);

    /* Interpolate the Lsp vectors */
    Lsp_Int( QntLpc, pDecStat->DecCng.LspSid, pDecStat->PrevLsp ) ;

    /* Copy the LSP vector for the next frame */
    for ( i = 0 ; i < LpcOrder ; i ++ )
        pDecStat->PrevLsp[i] = pDecStat->DecCng.LspSid[i] ;

    return;
}

