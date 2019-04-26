/*
**
** File:        "decod.c"
**
** Description:     Top-level source code for G.723 dual-rate decoder
**
** Functions:       Init_Decod()
**                  Decod()
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
//#include "lbccodec.h"
#include "decod.h"
#include "util_lbc.h"
#include "lpc.h"
#include "lsp.h"
#include "exc_lbc.h"
#include "dec_cng.h"
*/

#include "TYPEDEF.H"
#include "BASOP.H"
#include "CST_LBC.H"
#include "TAB_LBC.H"
#include "DECOD.H"
#include "UTIL_LBC.H"
#include "LPC.H"
#include "LSP.H"
#include "EXC_LBC.H"
#include "DEC_CNG.H"


/*
   The following structure contains all the static decoder
      variables.
*/

/*
**
** Function:        Init_Decod()
** 
** Description:     Initializes non-zero state variables
**          for the decoder.
**
** Links to text:   Section 3.11
** 
** Arguments:       None
** 
** Outputs:     None
** 
** Return value:    None
**
*/
void  Init_Decod(DECSTATDEF*pDecStat ,enum Crate WrkRate)
{
    int   i  ;

    /* Initialize encoder data structure with zeros */
    memset(pDecStat, 0, sizeof(DECSTATDEF));


    /* Initialize the previously decoded LSP vector to the DC vector */
    for ( i = 0 ; i < LpcOrder ; i ++ )
        pDecStat->PrevLsp[i] = LspDcTable[i] ;

    /* Initialize the gain scaling unit memory to a constant */
    pDecStat->Gain = (Word16) 0x1000 ;

	pDecStat->WrkRate=WrkRate;
	pDecStat->UsePf=1;
	Init_Dec_Cng(pDecStat);

    return;
}


/*
**
** Function:        Decod()
**
** Description:     Implements G.723 dual-rate decoder for  a frame
**          of speech
**
** Links to text:   Section 3
**
** Arguments:
**
**  Word16 *DataBuff    Empty buffer
**  Word16 Vinp[]       Encoded frame (22/26 bytes)
**

** Outputs:
**
**  Word16 DataBuff[]   Decoded frame (480 bytes)
**
** Return value:
**
**  Flag            Always True
**
*/

Flag    Decod( DECSTATDEF*pDecStat, Word16 *DataBuff, char *Vinp, Word16 Crc )
{
    int   i,j   ;

    Word32   Senr ;
    Word16   QntLpc[SubFrames*LpcOrder] ;
    Word16   AcbkCont[SubFrLen] ;

    Word16   LspVect[LpcOrder] ;
    Word16   Temp[PitchMax+Frame] ;
    Word16  *Dpnt ;

    LINEDEF  Line ;
    PFDEF    Pf[SubFrames] ;

    Word16   Ftyp;

    /*
    * Decode the packed bitstream for the frame.  (Text: Section 4;
    * pars of sections 2.17, 2.18)
    */
    Line = Line_Unpk( pDecStat,Vinp, &Ftyp, Crc ) ;

    /*
    * Update the frame erasure count (Text: Section 3.10)
    */
    if ( Line.Crc != (Word16) 0 ) {
        if(pDecStat->DecCng.PastFtyp == 1) Ftyp = 1;  /* active */
        else Ftyp = 0;  /* untransmitted */
    }

    if(Ftyp != 1) {

        /* Silence frame : do noise generation */
        Dec_Cng(pDecStat, Ftyp, &Line, DataBuff, QntLpc);
    }

    else {

        /*
        * Update the frame erasure count (Text: Section 3.10)
        */
        if ( Line.Crc != (Word16) 0 )
            pDecStat->Ecount = add( pDecStat->Ecount, (Word16) 1 ) ;
        else
            pDecStat->Ecount = (Word16) 0 ;

        if ( pDecStat->Ecount > (Word16) ErrMaxNum )
            pDecStat->Ecount = (Word16) ErrMaxNum ;

        /*
        * Decode the LSP vector for subframe 3.  (Text: Section 3.2)
        */
        Lsp_Inq( LspVect, pDecStat->PrevLsp, Line.LspId, Line.Crc ) ;

        /*
        * Interpolate the LSP vectors for subframes 0--2.  Convert the
        * LSP vectors to LPC coefficients.  (Text: Section 3.3)
        */
        Lsp_Int( QntLpc, LspVect, pDecStat->PrevLsp ) ;

        /* Copy the LSP vector for the next frame */
        for ( i = 0 ; i < LpcOrder ; i ++ )
            pDecStat->PrevLsp[i] = LspVect[i] ;

        /*
        * In case of no erasure, update the interpolation gain memory.
        * Otherwise compute the interpolation gain (Text: Section 3.10)
        */
        if ( pDecStat->Ecount == (Word16) 0 ) {
            pDecStat->InterGain = add( Line.Sfs[SubFrames-2].Mamp,
                                            Line.Sfs[SubFrames-1].Mamp ) ;
            pDecStat->InterGain = shr( pDecStat->InterGain, (Word16) 1 ) ;
            pDecStat->InterGain = FcbkGainTable[pDecStat->InterGain] ;
        }
        else
            pDecStat->InterGain = mult_r( pDecStat->InterGain, (Word16) 0x6000 ) ;


        /*
        * Generate the excitation for the frame
        */
        for ( i = 0 ; i < PitchMax ; i ++ )
            Temp[i] = pDecStat->PrevExc[i] ;

        Dpnt = &Temp[PitchMax] ;

        if ( pDecStat->Ecount == (Word16) 0 ) {

            for ( i = 0 ; i < SubFrames ; i ++ ) {

                /* Generate the fixed codebook excitation for a
                   subframe. (Text: Section 3.5) */
                Fcbk_Unpk( pDecStat,Dpnt, Line.Sfs[i], Line.Olp[i>>1], (Word16) i ) ;

                /* Generate the adaptive codebook excitation for a
                   subframe. (Text: Section 3.4) */
                Decod_Acbk( pDecStat->WrkRate, AcbkCont, &Temp[SubFrLen*i], Line.Olp[i>>1],
                                    Line.Sfs[i].AcLg, Line.Sfs[i].AcGn ) ;

                /* Add the adaptive and fixed codebook contributions to
                   generate the total excitation. */
                for ( j = 0 ; j < SubFrLen ; j ++ ) {
                    Dpnt[j] = shl( Dpnt[j], (Word16) 1 ) ;
                    Dpnt[j] = add( Dpnt[j], AcbkCont[j] ) ;
                }

                Dpnt += SubFrLen ;
            }

            /* Save the excitation */
            for ( j = 0 ; j < Frame ; j ++ )
                DataBuff[j] = Temp[PitchMax+j] ;

            /* Compute interpolation index. (Text: Section 3.10) */
            /* Use pDecStat->DecCng.SidGain and pDecStat->DecCng.CurGain to store    */
            /* excitation energy estimation                      */
            pDecStat->InterIndx = Comp_Info( Temp, Line.Olp[SubFrames/2-1],
                                       &pDecStat->DecCng.SidGain, &pDecStat->DecCng.CurGain ) ;

            /* Compute pitch post filter coefficients.  (Text: Section 3.6) */
            if ( pDecStat->UsePf )
                for ( i = 0 ; i < SubFrames ; i ++ )
                    Pf[i] = Comp_Lpf( pDecStat, Temp, Line.Olp[i>>1], (Word16) i ) ;

            /* Reload the original excitation */
            for ( j = 0 ; j < PitchMax ; j ++ )
                Temp[j] = pDecStat->PrevExc[j] ;
            for ( j = 0 ; j < Frame ; j ++ )
                Temp[PitchMax+j] = DataBuff[j] ;

            /* Perform pitch post filtering for the frame.  (Text: Section
               3.6) */
            if ( pDecStat->UsePf )
                for ( i = 0 ; i < SubFrames ; i ++ )
                    Filt_Lpf( DataBuff, Temp, Pf[i], (Word16) i ) ;

            /* Save Lsps --> LspSid */
            for(i=0; i< LpcOrder; i++)
                pDecStat->DecCng.LspSid[i] = pDecStat->PrevLsp[i];
        }

        else {

            /* If a frame erasure has occurred, regenerate the
               signal for the frame. (Text: Section 3.10) */
            Regen( DataBuff, Temp, pDecStat->InterIndx, pDecStat->InterGain,
                                        pDecStat->Ecount, &pDecStat->Rseed ) ;
        }

        /* Update the previous excitation for the next frame */
        for ( j = 0 ; j < PitchMax ; j ++ )
            pDecStat->PrevExc[j] = Temp[Frame+j] ;

        /* Resets random generator for CNG */
        pDecStat->DecCng.RandSeed = 12345;
    }

    /* Save Ftyp information for next frame */
    pDecStat->DecCng.PastFtyp = Ftyp;

    /*
    * Synthesize the speech for the frame
    */
    Dpnt = DataBuff ;
    for ( i = 0 ; i < SubFrames ; i ++ ) {

        /* Compute the synthesized speech signal for a subframe.
         * (Text: Section 3.7)
         */
        Synt( pDecStat, Dpnt, &QntLpc[i*LpcOrder] ) ;

        if ( pDecStat->UsePf ) {

            /* Do the formant post filter. (Text: Section 3.8) */
            Senr = Spf( pDecStat, Dpnt, &QntLpc[i*LpcOrder] ) ;

            /* Do the gain scaling unit.  (Text: Section 3.9) */
            Scale( pDecStat, Dpnt, Senr ) ;
        }

        Dpnt += SubFrLen ;
    }
    return (Flag) True ;
}
