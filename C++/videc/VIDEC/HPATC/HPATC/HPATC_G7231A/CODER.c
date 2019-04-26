/*
**
** File:        "coder.c"
**
** Description:     Top-level source code for G.723 dual-rate coder
**
** Functions:       Init_Coder()
**                  Coder()
**
**
*/
/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
#include "typedef.h"
#include "basop.h"
#include "cst_lbc.h"
#include "tab_lbc.h"
#include "coder.h"
#include "lpc.h"
#include "lsp.h"
#include "exc_lbc.h"
#include "util_lbc.h"
#include "vad.h"
#include "cod_cng.h"
#include "tame.h"*/
#include "TYPEDEF.H"
#include "BASOP.H"
#include "CST_LBC.H"
#include "TAB_LBC.H"
#include "CODER.H"
#include "LPC.H"
#include "LSP.H"
#include "EXC_LBC.H"
#include "UTIL_LBC.H"
#include "VAD.H"
#include "COD_CNG.H"
#include "TAME.H"

/*
   This file includes the coder main functions
*/

/*
**
** Function:        Init_Coder()
**
** Description:     Initializes non-zero state variables
**          for the coder.
**
** Links to text:   Section 2.21
** 
** Arguments:       None
**
** Outputs:     None
** 
** Return value:    None
**
*/
void  Init_Coder( CODSTATDEF*pCodStat,Flag UseVAD,enum Crate WrkRate)
{
    int   i ;

    /* Initialize encoder data structure with zeros */
    memset(pCodStat, 0, sizeof(CODSTATDEF));

    /* Initialize the previously decoded LSP vector to the DC vector */
    for ( i = 0 ; i < LpcOrder ; i ++ )
        pCodStat->PrevLsp[i] = LspDcTable[i] ;

    /* Initialize the taming procedure */
    for(i=0; i<SizErr; i++) pCodStat->Err[i] = Err0;

	pCodStat->WrkRate=WrkRate;
	pCodStat->UseVx=UseVAD;
	pCodStat->UseHp=1;
	if (pCodStat->UseVx)
	{
        Init_Vad(pCodStat);
        Init_Cod_Cng(pCodStat);
	}

    return;
}

/*
**
** Function:        Coder()
**
** Description:     Implements G.723 dual-rate coder for    a frame
**          of speech
**
** Links to text:   Section 2
**
** Arguments:
**
**  Word16 DataBuff[]   frame (480 bytes)
**

** Outputs:
**
**  Word16 Vout[]       Encoded frame (20/24 bytes)
**
** Return value:
**
**  Flag            Always True
**
*/
Flag  Coder(CODSTATDEF*pCodStat, Word16 *DataBuff, char *Vout )
{
    int     i,j   ;

    /*
      Local variables
    */
    Word16   UnqLpc[SubFrames*LpcOrder] ;
    Word16   QntLpc[SubFrames*LpcOrder] ;
    Word16   PerLpc[2*SubFrames*LpcOrder] ;

    Word16   LspVect[LpcOrder] ;
    LINEDEF  Line  ;
    PWDEF    Pw[SubFrames]  ;

    Word16   ImpResp[SubFrLen] ;

    Word16  *Dpnt  ;

    Word16  Ftyp = 1 ;

    /*
      Coder Start
    */
    Line.Crc = (Word16) 0 ;

    Rem_Dc( pCodStat, DataBuff ) ;

    /* Compute the Unquantized Lpc set for whole frame */
    Comp_Lpc(pCodStat,  UnqLpc, pCodStat->PrevDat, DataBuff ) ;

    /* Convert to Lsp */
    AtoLsp( LspVect, &UnqLpc[LpcOrder*(SubFrames-1)], pCodStat->PrevLsp ) ;

    /* Compute the Vad */
    Ftyp = (Word16) Comp_Vad( pCodStat, DataBuff ) ;

    /* VQ Lsp vector */
    Line.LspId = Lsp_Qnt( LspVect, pCodStat->PrevLsp ) ;

    Mem_Shift( pCodStat->PrevDat, DataBuff ) ;

    /* Compute Perceptual filter Lpc coefficients */
    Wght_Lpc( PerLpc, UnqLpc ) ;

    /* Apply the perceptual weighting filter */
    Error_Wght( pCodStat, DataBuff, PerLpc ) ;

    /*
    // Compute Open loop pitch estimates
    */
    Dpnt = (Word16 *) malloc( sizeof(Word16)*(PitchMax+Frame) ) ;

    /* Construct the buffer */
    for ( i = 0 ; i < PitchMax ; i ++ )
        Dpnt[i] = pCodStat->PrevWgt[i] ;
    for ( i = 0 ; i < Frame ; i ++ )
        Dpnt[PitchMax+i] = DataBuff[i] ;

    Vec_Norm( Dpnt, (Word16) (PitchMax+Frame) ) ;

    j = PitchMax ;
    for ( i = 0 ; i < SubFrames/2 ; i ++ ) {
        Line.Olp[i] = Estim_Pitch( Dpnt, (Word16) j ) ;
        pCodStat->VadStat.Polp[i+2] = Line.Olp[i] ;
        j += 2*SubFrLen ;
    }

    if(Ftyp != 1) {

        /*
        // Case of inactive signal
        */
        free ( (char *) Dpnt ) ;

        /* Save PrevWgt */
        for ( i = 0 ; i < PitchMax ; i ++ )
            pCodStat->PrevWgt[i] = DataBuff[i+Frame-PitchMax] ;



        /* CodCng => Ftyp = 0 (untransmitted) or 2 (SID) */
        Cod_Cng(pCodStat, DataBuff, &Ftyp, &Line, QntLpc);

        /* Update the ringing delays */
        Dpnt = DataBuff;
        for( i = 0 ; i < SubFrames; i++ ) {

            /* Update exc_err */
            Update_Err(pCodStat, Line.Olp[i>>1], Line.Sfs[i].AcLg, Line.Sfs[i].AcGn);

            Upd_Ring( pCodStat, Dpnt, &QntLpc[i*LpcOrder], &PerLpc[i*2*LpcOrder],
                                                        pCodStat->PrevErr ) ;
            Dpnt += SubFrLen;
        }
    }

    else {

        /*
        // Case of Active signal  (Ftyp=1)
        */

        /* Compute the Hmw */
        j = PitchMax ;
        for ( i = 0 ; i < SubFrames ; i ++ ) {
            Pw[i] = Comp_Pw( Dpnt, (Word16) j, Line.Olp[i>>1] ) ;
            j += SubFrLen ;
        }

        /* Reload the buffer */
        for ( i = 0 ; i < PitchMax ; i ++ )
            Dpnt[i] = pCodStat->PrevWgt[i] ;
        for ( i = 0 ; i < Frame ; i ++ )
            Dpnt[PitchMax+i] = DataBuff[i] ;

        /* Save PrevWgt */
        for ( i = 0 ; i < PitchMax ; i ++ )
            pCodStat->PrevWgt[i] = Dpnt[Frame+i] ;

        /* Apply the Harmonic filter */
        j = 0 ;
        for ( i = 0 ; i < SubFrames ; i ++ ) {
            Filt_Pw( DataBuff, Dpnt, (Word16) j , Pw[i] ) ;
            j += SubFrLen ;
        }
        free ( (char *) Dpnt ) ;

        /* Inverse quantization of the LSP */
        Lsp_Inq( LspVect, pCodStat->PrevLsp, Line.LspId, Line.Crc ) ;

        /* Interpolate the Lsp vectors */
        Lsp_Int( QntLpc, LspVect, pCodStat->PrevLsp ) ;

        /* Copy the LSP vector for the next frame */
        for ( i = 0 ; i < LpcOrder ; i ++ )
            pCodStat->PrevLsp[i] = LspVect[i] ;

        /*
        // Start the sub frame processing loop
        */
        Dpnt = DataBuff ;

        for ( i = 0 ; i < SubFrames ; i ++ ) {

            /* Compute full impulse response */
            Comp_Ir( ImpResp, &QntLpc[i*LpcOrder],
                                            &PerLpc[i*2*LpcOrder], Pw[i] ) ;

            /* Subtract the ringing of previous sub-frame */
            Sub_Ring( pCodStat, Dpnt, &QntLpc[i*LpcOrder], &PerLpc[i*2*LpcOrder],
                                                   pCodStat->PrevErr, Pw[i] ) ;

            /* Compute adaptive code book contribution */
            Find_Acbk( pCodStat, Dpnt, ImpResp, pCodStat->PrevExc, &Line, (Word16) i ) ;

            /* Compute fixed code book contribution */
            Find_Fcbk( pCodStat, Dpnt, ImpResp, &Line, (Word16) i ) ;

            /* Reconstruct the excitation */
            Decod_Acbk( pCodStat->WrkRate, ImpResp, pCodStat->PrevExc, Line.Olp[i>>1],
                                    Line.Sfs[i].AcLg, Line.Sfs[i].AcGn ) ;

            for ( j = SubFrLen ; j < PitchMax ; j ++ )
                pCodStat->PrevExc[j-SubFrLen] = pCodStat->PrevExc[j] ;

            for ( j = 0 ; j < SubFrLen ; j ++ ) {
                Dpnt[j] = shl( Dpnt[j], (Word16) 1 ) ;
                Dpnt[j] = add( Dpnt[j], ImpResp[j] ) ;
                pCodStat->PrevExc[PitchMax-SubFrLen+j] = Dpnt[j] ;
            }

            /* Update exc_err */
            Update_Err(pCodStat, Line.Olp[i>>1], Line.Sfs[i].AcLg, Line.Sfs[i].AcGn);

            /* Update the ringing delays */
            Upd_Ring( pCodStat, Dpnt, &QntLpc[i*LpcOrder], &PerLpc[i*2*LpcOrder],
                                                       pCodStat->PrevErr ) ;

            Dpnt += SubFrLen ;
        }  /* end of subframes loop */

        /*
        // Save Vad information and reset CNG random generator
        */
        pCodStat->CodCng.PastFtyp = 1;
        pCodStat->CodCng.RandSeed = 12345;

    } /* End of active frame case */

    /* Pack the Line structure */
    Line_Pack( pCodStat,&Line, Vout, Ftyp ) ;

    return (Flag) True ;
}
