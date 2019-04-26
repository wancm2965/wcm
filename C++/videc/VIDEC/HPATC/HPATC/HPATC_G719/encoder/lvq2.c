/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* ?2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/

#include "../include/proto.h"

/*--------------------------------------------------------------------------*/
/*  Function  lvq2                                                          */
/*  ~~~~~~~~~~~~~~                                                          */
/*                                                                          */
/*  High-rate lattice vector quantization                                   */
/*--------------------------------------------------------------------------*/
/*  short     *x  (i)   arbitrary vector                                    */
/*  short     *k  (o)   index of the selected codevector                    */
/*  short     r   (i)   number of indices                                   */
/*  Word16    R   (i)   number of bits per coefficinet                      */
/*--------------------------------------------------------------------------*/
void lvq2(short *x, short *k, short r, short R)
{
    short i, j;
    short err, temp, temp1, temp2;
    short v[8], y[8], kk[8], w[8];


    codesearch(x, v, R);
    code2idx(v, k, r);
    idx2code(k, y, R);

    temp = 0;
    for (i=0; i<8; i++)
    {
       err = v[i] - y[i];
       err = (err<0) ? -err : err;
       temp += err;
    }
    if (temp!=0)
    {
      for (i=0; i<15; i++)
      {
         for (j=0; j<8; j++)
         {
            if ((x[j]&1)!=0)
            {
              x[j] = (x[j] >> 1) + 1;
            }
            else
            {
              x[j] = x[j] >> 1;
            }
         }
         codesearch(x, v, R);
         code2idx(v, k, r);
         idx2code(k, y, R);
         temp1 = 0;
         for (j=0; j<8; j++)
         {
            err = v[j] - y[j];
            err = (err<0) ? -err : err;
            temp1 += err;
         }
         if (temp1==0)
         {
           break;
         }
      }

      if (temp1!=0)
      {
        for (i=0; i<8; i++)
        {
           k[i] = 0;
        }
      }
      else
      {
        for (i=1; i<16; i++)
        {
           for (j=0; j<8; j++)
           {
              temp2 = (x[j] * i) >> 4;
              if (temp2<0)
              {
                temp2++;
              }
              w[j] = x[j] + temp2;
           }
           codesearch(w, v, R);
           code2idx(v, kk, r);
           idx2code(kk, y, R);
           temp2 = 0;
           for (j=0; j<8; j++)
           {
              err = v[j] - y[j];
              err = (err<0) ? -err : err;
              temp2 += err;
           }
           if (temp2!=0)
           {
             break;
           }
           for (j=0; j<8; j++)
           {
              k[j] = kk[j];
           }
        }
      }
    }


    return;
}
