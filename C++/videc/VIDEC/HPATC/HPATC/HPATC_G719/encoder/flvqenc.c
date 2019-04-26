/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/proto.h"
#include "../include/cnst.h"
#include "../include/rom.h"

/*--------------------------------------------------------------------------*/
/*  Function  flvqenc                                                       */
/*  ~~~~~~~~~~~~~~~~~~~~~                                                   */
/*                                                                          */
/*  Encoding of Fast Lattice Vector Quantization (FLVQ)                     */
/*--------------------------------------------------------------------------*/
/*  short       **bitstream     (o)    bitstream vector                     */
/*  float       *coefs          (i)    MLT coefficient vector               */
/*  float       *coefs_norm     (o)    normalized MLT coefficient vector    */
/*  short       *R              (o)    bit-allocation vector                */
/*  short       *NumSpectumBits (i)    number of available bits             */
/*  short       *total_bits     (i)    total available bits                 */
/*  short       *is_transient   (i)    transient flag                       */
/*--------------------------------------------------------------------------*/
void flvqenc(
   short **bitstream,
   float *coefs,
   float *coefs_norm,
   short *R,
   short NumSpectumBits,
   short total_bits,
   short is_transient
)
{
   short i, j, k, m, n, r, v, im, nb_sfm, temp;
   short diff, sum;
   short hcode_l, offset, FlagL, FlagN, FlagC;
   short idx[NB_SFM], difidx[NB_SFM], idxbuf[NB_SFM];
   short normqlg2[NB_SFM], wnorm[NB_SFM], normbuf[NB_SFM];
   short ynrm[NB_SFM], ycof[STOP_BAND];
   short *pbits;
   short **ppbits;

   /*** Interleave MLT coefficients of 4 sub-vectors in case of transient ***/
   if (is_transient)
   {
      interleave_spectrum(coefs);
   }


   /*** Scalar quantization for norms ***/
   /* First group */
   logqnorm(coefs, ynrm, 32, WID_G1);
   j = ynrm[0];
   normqlg2[0] = dicnlg2[j];
   offset = WID_G1;
   for (i=1; i<SFM_G1; i++)
   {
      logqnorm(&coefs[offset], &ynrm[i], 40, WID_G1);
      offset += WID_G1;
   }
   /* Second group */
   for (i=SFM_G1; i<SFM_G1G2; i++)
   {
      logqnorm(&coefs[offset], &ynrm[i], 40, WID_G2);
      offset += WID_G2;
   }
   /* Third group */
   for (i=SFM_G1G2; i<SFM_N; i++)
   {
      logqnorm(&coefs[offset], &ynrm[i], 40, WID_G3);
      offset += WID_G3;
   }
   /* Forth group */
   for (i=SFM_N; i<NB_SFM; i++)
   {
      logqnorm(&coefs[offset], &ynrm[i], 40, WID_GX);
      offset += WID_GX;
   }
   for (i=0; i<NB_SFM; i++)
   {
      j = ynrm[i];
      normqlg2[i] = dicnlg2[j];
   }
   /* Find the maximum of normq and its position */
   temp = normqlg2[0];
   im = 0;
   for (i=1; i<NB_SFM; i++)
   {
      if (temp<normqlg2[i])
      {
         temp = normqlg2[i];
         im = i;
      }
   }
   m = SFM_N - 2;
   if ((total_bits<960) && (im<m))
   {
      nb_sfm = SFM_N;
      FlagL  = NOALLGROUPS;
   }
   else
   {
      nb_sfm = NB_SFM;
      FlagL  = ALLGROUPS;
   }
   /* Differential coding for indices of quantized norms */
   if (is_transient)
   {
     /* Reorder quantization indices and quantized norms */
     reordernorm(ynrm, normqlg2, idxbuf, normbuf);
     diffcod(normbuf, NB_SFM, idxbuf, &difidx[1]);
     difidx[0] = idxbuf[0];
     recovernorm(idxbuf, ynrm, normqlg2);
   }
   else
   {
     diffcod(normqlg2, NB_SFM, ynrm, &difidx[1]);
     difidx[0] = ynrm[0];
     normqlg2[0] = dicnlg2[ynrm[0]];
   }

   /*** Normalize coefficients with quantized norms ***/
   /* First group */
   normalizecoefs(&coefs[0], ynrm, 0, SFM_G1, WID_G1, &coefs_norm[0]);
   /* Second group */
   normalizecoefs(&coefs[NUMC_G1], ynrm, SFM_G1, SFM_G1G2, WID_G2, &coefs_norm[NUMC_G1]);
   /* Third group */
   normalizecoefs(&coefs[NUMC_G1G2], ynrm, SFM_G1G2, SFM_N, WID_G3, &coefs_norm[NUMC_G1G2]);
   /* Forth group */
   if (FlagL==ALLGROUPS)
   {
     normalizecoefs(&coefs[NUMC_N], ynrm, SFM_N, NB_SFM, WID_GX, &coefs_norm[NUMC_N]);
   }

   /*** Check Huffman encoding for QNorm indices ***/
   hcode_l = 0;
   for (i=1; i<NB_SFM; i++)
   {
      j = difidx[i];
      hcode_l += huffsizn[j];
   }
   FlagN = HUFCODE;
   if (hcode_l>=NUMNRMIBITS)
   {
      FlagN = NOHUFCODE;
      hcode_l = NUMNRMIBITS;
   }

   /*** Bit allocation ***/
   for (i=0; i<nb_sfm; i++)
   {
      idx[i] = i;
   }
   map_quant_weight(normqlg2, wnorm, is_transient);
   reordvct(wnorm, nb_sfm, idx);
   for (i=0; i<NB_SFM; i++)
   {
      R[i] = 0;
   }
   diff = NumSpectumBits - FLAGS_BITS - NORM0_BITS;
   v = diff - hcode_l;
   diff = v;

   bitalloc(wnorm, idx, diff, nb_sfm, QBIT_MAX2, R);
   /* Check bit allocation */
   bitallocsum(R, nb_sfm, &sum);
   diff = v - sum;

   /*** Lattice Vector Quantization for normalized MLT coefficients ***/
   /* First group */
   qcoefs(&coefs_norm[0], R, 0, SFM_G1, WID_G1, &ycof[0]);
   hcode_l = huffcheck(&ycof[0], R, 0, SFM_G1, WID_G1);

   /* Second group */
   qcoefs(&coefs_norm[NUMC_G1], R, SFM_G1, SFM_G1G2, WID_G2, &ycof[NUMC_G1]);
   k = huffcheck(&ycof[NUMC_G1], R, SFM_G1, SFM_G1G2, WID_G2);
   hcode_l += k;

   /* Third group */
   qcoefs(&coefs_norm[NUMC_G1G2], R, SFM_G1G2, SFM_N, WID_G3, &ycof[NUMC_G1G2]);
   k = huffcheck(&ycof[NUMC_G1G2], R, SFM_G1G2, SFM_N, WID_G3);
   hcode_l += k;

   /* Forth group */
   if (nb_sfm>SFM_N)
   {
     qcoefs(&coefs_norm[NUMC_N], R, SFM_N, NB_SFM, WID_GX, &ycof[NUMC_N]);
     k = huffcheck(&ycof[NUMC_N], R, SFM_N, NB_SFM, WID_GX);
     hcode_l += k;
   }
   n = sum - hcode_l;
   FlagC = NOHUFCODE;
   if (n>0)
   {
      FlagC = HUFCODE;
      diff += n;
   }

   /*** Packing indices to bit stream ***/
   pbits = *bitstream;
   /* Flags */
   *pbits = G192_BIT1;
   if (FlagL==0)
   {
      *pbits = G192_BIT0;
   }
   *++pbits = G192_BIT1;
   if (FlagN==0)
   {
      *pbits = G192_BIT0;
   }
   *++pbits = G192_BIT1;
   if (FlagC==0)
   {
      *pbits = G192_BIT0;
   }
   pbits++;

   /* Quantized norms */ 
   idx2bitsn(difidx[0], NORM0_BITS, pbits);
   pbits += NORM0_BITS;
   if (FlagN==HUFCODE)
   {
      /* Huffman encoding and then packing */
      for (i=1; i<NB_SFM; i++)
      {
         j = difidx[i];
         m = huffnorm[j];
         r = huffsizn[j];
         idx2bitsn(m, r, pbits);
         pbits += r;
      }
   }
   else
   {
      for (i=1; i<NB_SFM; i++)
      {
         idx2bitsn(difidx[i], NORMI_BITS, pbits);
         pbits += NORMI_BITS;
      }
   }

   /* Quantized MLT coefficients */
   /* First group */
   k = packingc(&ycof[0], R, pbits, FlagC, 0, SFM_G1, WID_G1);
   pbits += k;
   /* Second group */
   k = packingc(&ycof[NUMC_G1], R, pbits, FlagC, SFM_G1, SFM_G1G2, WID_G2);
   pbits += k;
   /* Third group */
   k = packingc(&ycof[NUMC_G1G2], R, pbits, FlagC, SFM_G1G2, SFM_N, WID_G3);
   pbits += k;
   /* Forth group */
   if (nb_sfm>SFM_N)
   {
      k = packingc(&ycof[NUMC_N], R, pbits, FlagC, SFM_N, NB_SFM, WID_GX);
      pbits += k;
   }
   
   /*** Processing for subframes allocated with no bits ***/
   ppbits = &pbits;
   procnobitsbfm(coefs_norm, R, idx, ycof, ppbits, nb_sfm, diff);

   *bitstream = pbits;
   return;
}
