/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/cnst.h"
#include "../include/rom.h"

/*--------------------------------------------------------------------------*/
/*  Function  huffcheck                                                     */
/*  ~~~~~~~~~~~~~~~~~~~                                                     */
/*                                                                          */
/*  Check Huffman encoding for LVQ2 indices                                 */
/*--------------------------------------------------------------------------*/
/*  short     *y     (i)   indices of the selected codevectors              */
/*  short     *R     (i)   number of bits per coefficinet                   */
/*  short     N1     (i)   beginning sub-vector's number in the group       */
/*  short     N2     (i)   ending sub-vector's number in the group          */
/*  short     L      (o)   number of coefficients in each sub-vector        */
/*--------------------------------------------------------------------------*/
/*  short     return (o)   length of Huffman code                           */
/*--------------------------------------------------------------------------*/
short huffcheck(short *y, short *R, short N1, short N2, short L)
{
    short i, j, n, v, offset;
    short hcode_l;
    short *pidx;


    pidx = y;
    hcode_l = 0;
    for (n=N1; n<N2; n++)
    {
       v = R[n];
       if (v>QBIT_MAX1)
       {
         hcode_l += L * v;
         pidx += L;
       }
       else if (v>1)
       {
         offset = huffoffset[v];
         for (i=0; i<L; i++)
         {
            j = offset + (*pidx++);
            hcode_l += huffsizc[j];
         }
       }
       else if (v==1)
       {
         hcode_l += L;
         pidx += L;
       }
       else
       {
         pidx += L;
       }
    }

    return(hcode_l);
}
