/*
**
** File:            "cod_cng.c"
**
** Description:     Comfort noise generation
**                  performed at the encoder part
**
** Functions:       Init_Cod_Cng()
**                  Cod_Cng()
**                  Update_Cng()
**
** Local functions:
**                  ComputePastAvFilter()
**                  CalcRC()
**                  LpcDiff()
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
#include "basop.h"
#include "lsp.h"
#include "lpc.h"
#include "util_cng.h"
#include "cod_cng.h"
#include "vad.h"
#include "coder.h"*/

#include "TYPEDEF.H"
#include "CST_LBC.H"
#include "TAB_LBC.H"
#include "UTIL_LBC.H"
#include "BASOP.H"
#include "LSP.H"
#include "LPC.H"
#include "UTIL_CNG.H"
#include "COD_CNG.H"
#include "VAD.H"
#include "CODER.H"


/* Declaration of local functions */
static void ComputePastAvFilter( CODSTATDEF*pCodStat, Word16 *Coeff);
static void CalcRC(Word16 *Coeff, Word16 *RC, Word16 *shRC);
static Flag LpcDiff(Word16 *RC, Word16 shRC, Word16 *Acf, Word16 alpha);

/*
**
** Function:        Init_Cod_Cng()
**
** Description:     Initialize Cod_Cng static variables
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
void Init_Cod_Cng(CODSTATDEF*pCodStat)
{
    int i;

    pCodStat->CodCng.CurGain = 0;

    for(i=0; i< SizAcf; i++) pCodStat->CodCng.Acf[i] = 0;

    for(i=0; i <= NbAvAcf; i++) pCodStat->CodCng.ShAcf[i] = 40;

    for(i=0; i < LpcOrder; i++) pCodStat->CodCng.SidLpc[i] = 0;

    pCodStat->CodCng.PastFtyp = 1;

    pCodStat->CodCng.RandSeed = 12345;

    return;
}


/*
**
** Function:           Cod_Cng()
**
** Description:        Computes Ftyp for inactive frames
**                              0  :  for untransmitted frames
**                              2  :  for SID frames
**                     Computes current frame excitation
**                     Computes current frame LSPs
**                     Computes the coded parameters of SID frames
**
** Links to text:
**
** Arguments:
**
**  Word16 *DataExc    Current frame synthetic excitation
**  Word16 *Ftyp     Characterizes the frame type for CNG
**  LINEDEF *Line      Quantized parameters (used for SID frames)
**  Word16 *QntLpc     Interpolated frame LPC coefficients
**
** Outputs:
**
**  Word16 *DataExc
**  Word16 *Ftyp
**  LINEDEF *Line
**  Word16 *QntLpc
**
** Return value:       None
**
*/
void Cod_Cng(CODSTATDEF*pCodStat, Word16 *DataExc, Word16 *Ftyp, LINEDEF *Line, Word16 *QntLpc)
{


    Word16 curCoeff[LpcOrder];
    Word16 curQGain;
    Word16 temp;
    int i;

 /*
  * Update Ener
  */
    for(i=NbAvGain-1; i>=1; i--) {
        pCodStat->CodCng.Ener[i] = pCodStat->CodCng.Ener[i-1];
    }

 /*
  * Compute LPC filter of present frame
  */
    pCodStat->CodCng.Ener[0] = Durbin(curCoeff, &pCodStat->CodCng.Acf[1], pCodStat->CodCng.Acf[0], &temp);

 /*
  * if first frame of silence => SID frame
  */
    if(pCodStat->CodCng.PastFtyp == 1) {
        *Ftyp = 2;
        pCodStat->CodCng.NbEner = 1;
        curQGain = Qua_SidGain(pCodStat->CodCng.Ener, pCodStat->CodCng.ShAcf, pCodStat->CodCng.NbEner);
    }

    else {
        pCodStat->CodCng.NbEner++;
        if(pCodStat->CodCng.NbEner > NbAvGain) pCodStat->CodCng.NbEner = NbAvGain;
        curQGain = Qua_SidGain(pCodStat->CodCng.Ener, pCodStat->CodCng.ShAcf, pCodStat->CodCng.NbEner);

 /*
  * Compute stationarity of current filter
  * versus reference filter
  */
        if(LpcDiff(pCodStat->CodCng.RC, pCodStat->CodCng.ShRC, pCodStat->CodCng.Acf, *pCodStat->CodCng.Ener) == 0) {
            /* transmit SID frame */
            *Ftyp = 2;
        }
        else {
            temp = abs_s(sub(curQGain, pCodStat->CodCng.IRef));
            if(temp > ThreshGain) {
                *Ftyp = 2;
            }
            else {
                /* no transmission */
                *Ftyp = 0;
            }
        }
    }

 /*
  * If SID frame : Compute SID filter
  */
    if(*Ftyp == 2) {

 /*
  * Evaluates local stationnarity :
  * Computes difference between current filter and past average filter
  * if signal not locally stationary SID filter = current filter
  * else SID filter = past average filter
  */
        /* Compute past average filter */
        ComputePastAvFilter(pCodStat,pCodStat->CodCng.SidLpc) ;

        /* If adaptation enabled, fill noise filter */
        if ( !pCodStat->VadStat.Aen ) {
            for(i=0; i<LpcOrder; i++) pCodStat->VadStat.NLpc[i] = pCodStat->CodCng.SidLpc[i];
        }

        /* Compute autocorr. of past average filter coefficients */
        CalcRC(pCodStat->CodCng.SidLpc , pCodStat->CodCng.RC, &pCodStat->CodCng.ShRC);

        if(LpcDiff(pCodStat->CodCng.RC, pCodStat->CodCng.ShRC, pCodStat->CodCng.Acf, *pCodStat->CodCng.Ener) == 0){
            for(i=0; i<LpcOrder; i++) {
                pCodStat->CodCng.SidLpc[i] = curCoeff[i];
            }
            CalcRC(curCoeff, pCodStat->CodCng.RC, &pCodStat->CodCng.ShRC);
        }

 /*
  * Compute SID frame codes
  */
        /* Compute LspSid */
        AtoLsp(pCodStat->CodCng.LspSid, pCodStat->CodCng.SidLpc, pCodStat->PrevLsp);
        Line->LspId = Lsp_Qnt(pCodStat->CodCng.LspSid, pCodStat->PrevLsp);
        Lsp_Inq(pCodStat->CodCng.LspSid, pCodStat->PrevLsp, Line->LspId, 0);

        Line->Sfs[0].Mamp = curQGain;
        pCodStat->CodCng.IRef = curQGain;
        pCodStat->CodCng.SidGain = Dec_SidGain(pCodStat->CodCng.IRef);

    } /* end of Ftyp=2 case (SID frame) */

 /*
  * Compute new excitation
  */
    if(pCodStat->CodCng.PastFtyp == 1) {
        pCodStat->CodCng.CurGain = pCodStat->CodCng.SidGain;
    }
    else {
          pCodStat->CodCng.CurGain = extract_h(L_add( L_mult(pCodStat->CodCng.CurGain,0x7000),
                    L_mult(pCodStat->CodCng.SidGain,0x1000) ) ) ;
    }
    Calc_Exc_Rand(pCodStat->WrkRate,pCodStat->CodCng.CurGain, pCodStat->PrevExc, DataExc,
                                                &pCodStat->CodCng.RandSeed, Line);

 /*
  * Interpolate LSPs and update PrevLsp
  */
    Lsp_Int(QntLpc, pCodStat->CodCng.LspSid, pCodStat->PrevLsp);
    for (i=0; i < LpcOrder ; i++) {
        pCodStat->PrevLsp[i] = pCodStat->CodCng.LspSid[i];
    }

 /*
  * Output & save frame type info
  */
    pCodStat->CodCng.PastFtyp = *Ftyp;
    return;
}

/*
**
** Function:           Update_Acf()
**
** Description:        Computes & Stores sums of subframe-acfs
**
** Links to text:
**
** Arguments:
**
**  Word16 *Acf_sf     sets of subframes Acfs of current frame
**  Word16 *ShAcf_sf   corresponding scaling factors
**
** Output :            None
**
** Return value:       None
**
*/
void Update_Acf( CODSTATDEF*pCodStat, Word16 *Acf_sf, Word16 *ShAcf_sf)
{

    int i, i_subfr;
    Word16 *ptr1, *ptr2;
    Word32 L_temp[LpcOrderP1];
    Word16 sh1, temp;
    Word32 L_acc0;

    /* Update Acf and ShAcf */
    ptr2 = pCodStat->CodCng.Acf + SizAcf;
    ptr1 = ptr2 - LpcOrderP1;
    for(i=LpcOrderP1; i<SizAcf; i++) *(--ptr2) = *(--ptr1);
    for(i=NbAvAcf; i>=1; i--) pCodStat->CodCng.ShAcf[i] = pCodStat->CodCng.ShAcf[i-1];

    /* Search ShAcf_sf min for current frame */
    sh1 = ShAcf_sf[0];
    for(i_subfr=1; i_subfr<SubFrames; i_subfr++) {
        if(ShAcf_sf[i_subfr] < sh1) sh1 = ShAcf_sf[i_subfr];
    }
    sh1 = add(sh1, 14);  /* 2 bits of margin */

    /* Compute current sum of acfs */
    for(i=0; i<= LpcOrder; i++) L_temp[i] = 0;

    ptr2 = Acf_sf;
    for(i_subfr=0; i_subfr<SubFrames; i_subfr++) {
        temp = sub(sh1, ShAcf_sf[i_subfr]);
        for(i=0; i <= LpcOrder; i++) {
            L_acc0 = L_deposit_l(*ptr2++);
            L_acc0 = L_shl(L_acc0, temp);  /* shift right if temp<0 */
            L_temp[i] = L_add(L_temp[i], L_acc0);
        }
    }
    /* Normalize */
    temp = norm_l(L_temp[0]);
    temp = sub(16, temp);
    if(temp < 0) temp = 0;
    for(i=0; i <= LpcOrder; i++) {
        pCodStat->CodCng.Acf[i] = extract_l(L_shr(L_temp[i],temp));
    }

    pCodStat->CodCng.ShAcf[0] = sub(sh1, temp);

    return;
}

/*
**
** Function:           ComputePastAvFilter()
**
** Description:        Computes past average filter
**
** Links to text:
**
** Argument:
**
**  Word16 *Coeff      set of LPC coefficients
**
** Output:
**
**  Word16 *Coeff
**
** Return value:       None
**
*/
void ComputePastAvFilter( CODSTATDEF*pCodStat, Word16 *Coeff)
{
    int i, j;
    Word16 *ptr_Acf;
    Word32 L_sumAcf[LpcOrderP1];
    Word16 Corr[LpcOrder], Err;
    Word16 sh1, temp;
    Word32 L_acc0;

    /* Search ShAcf min */
    sh1 = pCodStat->CodCng.ShAcf[1];
    for(i=2; i <= NbAvAcf; i ++) {
        temp = pCodStat->CodCng.ShAcf[i];
        if(temp < sh1) sh1 = temp;
    }
    sh1 = add(sh1, 14);     /* 2 bits of margin : NbAvAcf <= 4 */

    /* Compute sum of NbAvAcf frame-Acfs  */
    for(j=0; j <= LpcOrder; j++) L_sumAcf[j] = 0;

    ptr_Acf = pCodStat->CodCng.Acf + LpcOrderP1;
    for(i=1; i <= NbAvAcf; i ++) {
        temp = sub(sh1, pCodStat->CodCng.ShAcf[i]);
        for(j=0; j <= LpcOrder; j++) {
            L_acc0 = L_deposit_l(*ptr_Acf++);
            L_acc0 = L_shl(L_acc0, temp); /* shift right if temp<0 */
            L_sumAcf[j] = L_add(L_sumAcf[j], L_acc0);
        }
    }

    /* Normalize */
    temp = norm_l(L_sumAcf[0]);
    temp = sub(16, temp);
    if(temp < 0) temp = 0;
    Err = extract_l(L_shr(L_sumAcf[0],temp));
    for(i=1; i<LpcOrderP1; i++) {
        Corr[i-1] = extract_l(L_shr(L_sumAcf[i],temp));
    }

    Durbin(Coeff, Corr, Err, &temp);

    return;
}

/*
**
** Function:           CalcRC()
**
** Description:        Computes function derived from
**                     the autocorrelation of LPC coefficients
**                     used for Itakura distance
**
** Links to text:
**
** Arguments :
**
**  Word16 *Coeff      set of LPC coefficients
**  Word16 *RC         derived from LPC coefficients autocorrelation
**  Word16 *ShRC       corresponding scaling factor
**
** Outputs :
**
**  Word16 *RC
**  Word16 *ShRC
**
** Return value:       None
**
*/
void CalcRC(Word16 *Coeff, Word16 *RC, Word16 *ShRC)
{
    int i, j;
    Word16 sh1;
    Word32 L_acc;

    L_acc = 0L;
    for(j=0; j<LpcOrder; j++) {
        L_acc = L_mac(L_acc, Coeff[j], Coeff[j]);
    }
    L_acc = L_shr(L_acc, 1);
    L_acc = L_add(L_acc, 0x04000000L);  /* 1 << 2 * Lpc_justif. */
    sh1 = norm_l(L_acc) - (Word16)2;    /* 1 bit because of x2 in RC[i], i> 0*/
                                /* & 1 bit margin for Itakura distance */
    L_acc = L_shl(L_acc, sh1); /* shift right if < 0 */
    RC[0] = round(L_acc);

    for(i=1; i<=LpcOrder; i++) {
        L_acc = L_mult( (Word16) 0xE000, Coeff[i-1]);   /* - (1 << Lpc_justif.) */
        for(j=0; j<LpcOrder-i; j++) {
            L_acc = L_mac(L_acc, Coeff[j], Coeff[j+i]);
        }
        L_acc = L_shl(L_acc, sh1);
        RC[i] = round(L_acc);
    }
    *ShRC = sh1;
    return;
}

/*
**
** Function:           LpcDiff()
**
** Description:        Comparison of two filters
**                     using Itakura distance
**                     1st filter : defined by *ptrAcf
**                     2nd filter : defined by *RC
**                     the autocorrelation of LPC coefficients
**                     used for Itakura distance
**
** Links to text:
**
** Arguments :
**
**  Word16 *RC         derived from LPC coefficients autocorrelation
**  Word16 ShRC        corresponding scaling factor
**  Word16 *ptrAcf     pointer on signal autocorrelation function
**  Word16 alpha       residual energy in LPC analysis using *ptrAcf
**
** Output:             None
**
** Return value:       flag = 1 if similar filters
**                     flag = 0 if different filters
**
*/
Flag LpcDiff(Word16 *RC, Word16 ShRC, Word16 *ptrAcf, Word16 alpha)
{
    Word32 L_temp0, L_temp1;
    Word16 temp;
    int i;
    Flag diff;

    L_temp0 = 0L;
    for(i=0; i<=LpcOrder; i++) {
        temp = shr(ptrAcf[i], 2);  /* + 2 margin bits */
        L_temp0 = L_mac(L_temp0, RC[i], temp);
    }

    temp = mult_r(alpha, FracThresh);
    L_temp1 = L_add((Word32)temp, (Word32)alpha);
    temp = add(ShRC, 9);  /* 9 = Lpc_justif. * 2 - 15 - 2 */
    L_temp1 = L_shl(L_temp1, temp);

    if(L_temp0 < L_temp1) diff = 1;
    else diff = 0;
    return(diff);
}

