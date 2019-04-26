
/*
**
** File:    tame.c
**
** Description: Functions used to avoid possible explosion of the decoder
**              excitation in case of series of long term unstable filters
**              and when the encoder and the decoder are de-synchronized
**
** Functions:
**
**  Computing excitation error estimation :
**      Update_Err()
**  Test excitation error :
**      Test_Err()
*/

/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.0
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/
#include <stdlib.h>
#include <stdio.h>

/*
#include "typedef.h"
#include "basop.h"
#include "cst_lbc.h"
//#include "lbccodec.h"
#include "coder.h"
#include "exc_lbc.h"
#include "tame.h"
#include "tab_lbc.h"
*/
#include "TYPEDEF.H"
#include "BASOP.H"
#include "CST_LBC.H"
#include "CODER.H"
#include "EXC_LBC.H"
#include "TAME.H"
#include "TAB_LBC.H"


/*
**
** Function:        Update_Err()
**
** Description:     Estimation of the excitation error associated
**                  to the excitation signal when it is disturbed at
**                  the decoder, the disturbing signal being filtered
**                  by the long term synthesis filters
**                  one value for (SubFrLen/2) samples
**                  Updates the table pCodStat->Err
**
** Links to text:   Section
**
** Arguments:
**
**  Word16 Olp      Center value for pitch delay
**  Word16 AcLg     Offset value for pitch delay
**  Word16 AcGn     Index of Gain LT filter
**
** Outputs: None
**
** Return value:    None
**
*/

void Update_Err(CODSTATDEF*pCodStat, 
    Word16 Olp, Word16 AcLg, Word16 AcGn
)
{
    Word16 *ptr_tab;
    Word16 i, iz, temp1, temp2;
    Word16 Lag;
    Word32 Worst1, Worst0, L_temp;
    Word16 beta;

    Lag = Olp - (Word16)Pstep + AcLg;

    /* Select Quantization tables */
    i = 0 ;
    ptr_tab = tabgain85;
    if ( pCodStat->WrkRate == Rate63 ) {
        if ( Olp >= (Word16) (SubFrLen-2) ) ptr_tab = tabgain170;
    }
    else {
        ptr_tab = tabgain170;
    }
    beta = ptr_tab[(int)AcGn]; /* beta = gain * 8192 */


    if(Lag <= (SubFrLen/2)) {
        Worst0 = L_mls(pCodStat->Err[0], beta);
        Worst0 = L_shl(Worst0, 2);
        Worst0 = L_add(Err0, Worst0);
        Worst1 = Worst0;
    }

    else {
        iz = mult(Lag, 1092);   /* Lag / 30 */
        temp1 = add(iz, 1);
        temp2 = sub(shl(temp1, 5), shl(temp1, 1));      /* 30 (iz+1) */
        if(temp2 != Lag) {

            if(iz == 1) {
                Worst0 = L_mls(pCodStat->Err[0], beta);
                Worst0 = L_shl(Worst0, 2);
                Worst0 = L_add(Err0, Worst0);
                Worst1 = L_mls(pCodStat->Err[1], beta);
                Worst1 = L_shl(Worst1, 2);
                Worst1 = L_add(Err0, Worst1);
                if(Worst0 > Worst1) Worst1 = Worst0;
                else Worst0 = Worst1;
            }

            else {
                Worst0 = L_mls(pCodStat->Err[iz-2], beta);
                Worst0 = L_shl(Worst0, 2);
                Worst0 = L_add(Err0, Worst0);
                L_temp = L_mls(pCodStat->Err[iz-1], beta);
                L_temp = L_shl(L_temp, 2);
                L_temp = L_add(Err0, L_temp);
                if(L_temp > Worst0) Worst0 = L_temp;
                Worst1 = L_mls(pCodStat->Err[iz], beta);
                Worst1 = L_shl(Worst1, 2);
                Worst1 = L_add(Err0, Worst1);
                if(L_temp > Worst1) Worst1 = L_temp;
            }
        }
        else {  /* Lag % SubFrLen = 0 */
            Worst0 = L_mls(pCodStat->Err[iz-1], beta);
            Worst0 = L_shl(Worst0, 2);
            Worst0 = L_add(Err0, Worst0);
            Worst1 = L_mls(pCodStat->Err[iz], beta);
            Worst1 = L_shl(Worst1, 2);
            Worst1 = L_add(Err0, Worst1);
        }
    }

    for(i=4; i>=2; i--) {
        pCodStat->Err[i] = pCodStat->Err[i-2];
    }
    pCodStat->Err[0] = Worst0;
    pCodStat->Err[1] = Worst1;

    return;
}

/*
**
** Function:        Test_Err()
**
** Description:     Check the error excitation maximum for
**                  the subframe and computes an index iTest used to
**                  calculate the maximum nb of filters (in Find_Acbk) :
**                  Bound = Min(Nmin + iTest x pas, Nmax) , with
**                  AcbkGainTable085 : pas = 2, Nmin = 51, Nmax = 85
**                  AcbkGainTable170 : pas = 4, Nmin = 93, Nmax = 170
**                  iTest depends on the relative difference between
**                  errmax and a fixed threshold
**
** Links to text:   Section
**
** Arguments:
**
**  Word16 Lag1      1st long term Lag of the tested zone
**  Word16 Lag2      2nd long term Lag of the tested zone
**
** Outputs: None
**
** Return value:
**  Word16          index iTest used to compute Acbk number of filters
*/

Word16 Test_Err(CODSTATDEF*pCodStat, 
    Word16 Lag1, Word16 Lag2
)
{

    int i, i1, i2;
    Word16 zone1, zone2;
    Word32 Acc, Err_max;
    Word16 iTest;

    i2 = Lag2 + ClPitchOrd/2;
    zone2 = mult( (Word16) i2, (Word16) 1092);

    i1 = - SubFrLen + 1 + Lag1 - ClPitchOrd/2;
    if(i1 <= 0) i1 = 1;
    zone1 = mult( (Word16) i1, (Word16) 1092);

    Err_max = -1L;
    for(i=zone2; i>=zone1; i--) {
        Acc = L_sub(pCodStat->Err[i], Err_max);
        if(Acc > 0L) {
                Err_max = pCodStat->Err[i];
        }
    }
    Acc = L_sub(Err_max, ThreshErr);
    if((Acc > 0L) || (pCodStat->SinDet < 0 ) ) {
        iTest = 0;
    }
    else {
        Acc = L_negate(Acc);
        Acc = L_shr(Acc, DEC);
        iTest = extract_l(Acc);
    }

    return(iTest);
}
