/*
**

** File:        "vad.c"
**
** Description:     Voice Activity Detection
**
** Functions:       Init_Vad()
**                  Vad()
**
**
*/
/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/

#include <stdio.h>

/*
#include "typedef.h"
#include "basop.h"
#include "cst_lbc.h"
#include "tab_lbc.h"
#include "lsp.h"
#include "vad.h"
#include "coder.h"
//#include "lbccodec.h"
*/
#include "TYPEDEF.H"
#include "BASOP.H"
#include "CST_LBC.H"
#include "TAB_LBC.H"
#include "LSP.H"
#include "VAD.H"
#include "CODER.H"


void    Init_Vad(CODSTATDEF*pCodStat)
{
    int i ;
    pCodStat->VadStat.Hcnt = 3 ;
    pCodStat->VadStat.Vcnt = 0 ;
    pCodStat->VadStat.Penr = 0x00000400L ;
    pCodStat->VadStat.Nlev = 0x00000400L ;

    pCodStat->VadStat.Aen = 0 ;

    pCodStat->VadStat.Polp[0] = 1 ;
    pCodStat->VadStat.Polp[1] = 1 ;
    pCodStat->VadStat.Polp[2] = SubFrLen ;
    pCodStat->VadStat.Polp[3] = SubFrLen ;

    for(i=0; i < LpcOrder; i++) pCodStat->VadStat.NLpc[i] = 0;

}

Flag Comp_Vad( CODSTATDEF*pCodStat, Word16 *Dpnt)
{
    int i,j ;

    Word32  Acc0,Acc1 ;
    Word16  Tm0, Tm1, Tm2 ;
    Word16  Minp ;

    Flag    VadState = 1 ;

    static  Word16  ScfTab[11] = {
         9170 ,
         9170 ,
         9170 ,
         9170 ,
        10289 ,
        11544 ,
        12953 ,
        14533 ,
        16306 ,
        18296 ,
        20529 ,
    } ;

    if ( !pCodStat->UseVx )
        return VadState ;

    /* Find Minimum pitch period */
    Minp = PitchMax ;
    for ( i = 0 ; i < 4 ; i ++ ) {
        if ( Minp > pCodStat->VadStat.Polp[i] )
            Minp = pCodStat->VadStat.Polp[i] ;
    }

    /* Check that all are multiplies of the minimum */
    Tm2 = 0 ;
    for ( i = 0 ; i < 4 ; i ++ ) {
        Tm1 = Minp ;
        for ( j = 0 ; j < 8 ; j ++ ) {
            Tm0 = sub( Tm1, pCodStat->VadStat.Polp[i] ) ;
            Tm0 = abs_s( Tm0 ) ;
            if ( Tm0 <= 3 )
                Tm2 ++ ;
            Tm1 = add( Tm1, Minp ) ;
        }
    }

    /* Update adaptation enable counter if not periodic and not sine */
    if ( (Tm2 == 4) || (pCodStat->SinDet < 0) )
        pCodStat->VadStat.Aen += 2 ;
    else
        pCodStat->VadStat.Aen -- ;

    /* Clip it */
    if ( pCodStat->VadStat.Aen > 6 )
        pCodStat->VadStat.Aen = 6 ;
    if ( pCodStat->VadStat.Aen < 0 )
        pCodStat->VadStat.Aen = 0 ;

    /* Inverse filter the data */
    Acc1 = 0L ;
    for ( i = SubFrLen ; i < Frame ; i ++ ) {

        Acc0 = L_mult( Dpnt[i], 0x2000 ) ;
        for ( j = 0 ; j < LpcOrder ; j ++ )
            Acc0 = L_msu( Acc0, Dpnt[i-j-1], pCodStat->VadStat.NLpc[j] ) ;
        Tm0 = round ( Acc0 ) ;
        Acc1 = L_mac( Acc1, Tm0, Tm0 ) ;
    }

    /* Scale the rezidual energy */
    Acc1 = L_mls( Acc1, (Word16) 2913 ) ;

    /* Clip noise level in any case */
    if ( pCodStat->VadStat.Nlev > pCodStat->VadStat.Penr ) {
        Acc0 = L_sub( pCodStat->VadStat.Penr, L_shr( pCodStat->VadStat.Penr, 2 ) ) ;
        pCodStat->VadStat.Nlev = L_add( Acc0, L_shr( pCodStat->VadStat.Nlev, 2 ) ) ;
    }


    /* Update the noise level, if adaptation is enabled */
    if ( !pCodStat->VadStat.Aen ) {
        pCodStat->VadStat.Nlev = L_add( pCodStat->VadStat.Nlev, L_shr( pCodStat->VadStat.Nlev, 5 ) ) ;
    }
    /* Decay Nlev by small amount */
    else {
        pCodStat->VadStat.Nlev = L_sub( pCodStat->VadStat.Nlev, L_shr( pCodStat->VadStat.Nlev,11 ) ) ;
    }

    /* Update previous energy */
    pCodStat->VadStat.Penr = Acc1 ;

    /* CLip Noise Level */
    if ( pCodStat->VadStat.Nlev < 0x00000080L )
        pCodStat->VadStat.Nlev = 0x00000080L ;
    if ( pCodStat->VadStat.Nlev > 0x0001ffffL )
        pCodStat->VadStat.Nlev = 0x0001ffffL ;

    /* Compute the treshold */
    Acc0 = L_shl( pCodStat->VadStat.Nlev, 13 ) ;
    Tm0 = norm_l( Acc0 ) ;
    Acc0 = L_shl( Acc0, Tm0 ) ;
    Acc0 &= 0x3f000000L ;
    Acc0 <<= 1 ;
    Tm1 = extract_h( Acc0 ) ;
    Acc0 = L_deposit_h( ScfTab[Tm0] ) ;
    Acc0 = L_mac( Acc0, Tm1, ScfTab[Tm0-1] ) ;
    Acc0 = L_msu( Acc0, Tm1, ScfTab[Tm0] ) ;
    Tm1 = extract_h( Acc0 ) ;
    Tm0 = extract_l( L_shr( pCodStat->VadStat.Nlev, 2 ) ) ;
    Acc0 = L_mult( Tm0, Tm1 ) ;
    Acc0 >>= 11 ;

    /* Compare with the treshold */
    if ( Acc0 > Acc1 )
        VadState = 0 ;

    /* Do the various counters */
    if ( VadState ) {
        pCodStat->VadStat.Vcnt ++ ;
        pCodStat->VadStat.Hcnt ++ ;
    }
    else {
        pCodStat->VadStat.Vcnt -- ;
        if ( pCodStat->VadStat.Vcnt < 0 )
            pCodStat->VadStat.Vcnt = 0 ;
    }

    if ( pCodStat->VadStat.Vcnt >= 2 ) {
        pCodStat->VadStat.Hcnt = 6 ;
        if ( pCodStat->VadStat.Vcnt >= 3 )
            pCodStat->VadStat.Vcnt = 3 ;
    }

    if ( pCodStat->VadStat.Hcnt ) {
        VadState = 1 ;
        if ( pCodStat->VadStat.Vcnt == 0 )
            pCodStat->VadStat.Hcnt -- ;
    }

    /* Update Periodicy detector */
    pCodStat->VadStat.Polp[0] = pCodStat->VadStat.Polp[2] ;
    pCodStat->VadStat.Polp[1] = pCodStat->VadStat.Polp[3] ;

    return VadState ;
}







