/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/proto.h"
#include "../include/cnst.h"
#include "../include/rom.h"

/*--------------------------------------------------------------------------*/
/*  Function  packingc                                                      */
/*  ~~~~~~~~~~~~~~~~~~                                                      */
/*                                                                          */
/*  Huffman encoding and packing indices of quantized coefficients          */
/*--------------------------------------------------------------------------*/
/*  short     *y      (i)   indices of the selected codevectors             */
/*  short     *R      (i)   number of bits per coefficinet                  */
/*  short     *pbits  (i)   pointer to bitstream                            */
/*  short     flag    (i)   Huffman code flag                               */
/*  short     N1      (i)   beginning sub-vector's number in the group      */
/*  short     N2      (i)   ending sub-vector's number in the group         */
/*  short     L       (i)   number of coefficients in each sub-vector       */
/*--------------------------------------------------------------------------*/
/*  short     return  (o)   length of Huffman code                          */
/*--------------------------------------------------------------------------*/
short packingc(short *y, short *R, short *pbits, short flag, short N1, short N2, short L)
{
    short i, j, n, r, v, rv, offset, sum;
    short nb_vecs, length, temp;
    short *pidx, *ptmp;


    pidx = y;
    nb_vecs = L >> 3;
    length = 0;
    if (flag==NOHUFCODE)
    {
      for (n=N1; n<N2; n++)
      {
         rv = R[n];
         if (rv>1)
         {
           idx2bitsc(pidx, L, rv, pbits);
           sum = rv * L;
           pbits += sum;
           pidx += L;
           length += sum;
         }
         else if (rv==1)
         {
           for (i=0; i<nb_vecs; i++)
           {
              idx2bitsc(pidx, 8, 1, pbits);
              pbits += 8;
              pidx += 8;
           }
           length += L;
         }
         else
         {
           pidx += L;
         }
      }
    }
    else
    {
      for (n=N1; n<N2; n++)
      {
         rv = R[n];
         if (rv>QBIT_MAX1)
         {
           idx2bitsc(pidx, L, rv, pbits);
           sum = rv * L;
           pbits += sum;
           pidx += L;
           length += sum;
         }
         else if (rv>1)
         {
           offset = huffoffset[rv];
           for (i=0; i<L; i++)
           {
              j = offset + (*pidx++);
              v = huffcoef[j];
              r = huffsizc[j];
              ptmp = pbits + r;
              for (j=0; j<r; j++)
              {
                 temp = (v >> j) & 1;
                 ptmp--;
                 *ptmp = G192_BIT1;
                 if (temp==0)
                 {
                   *ptmp = G192_BIT0;
                 }
              }
              pbits += r;
              length += r;
           }
         }
         else if (rv==1)
         {
           for (i=0; i<nb_vecs; i++)
           {
              idx2bitsc(pidx, 8, 1, pbits);
              pbits += 8;
              pidx += 8;
           }
           length += L;
         }
         else
         {
           pidx += L;
         }
      }
    }

    return(length);
}
