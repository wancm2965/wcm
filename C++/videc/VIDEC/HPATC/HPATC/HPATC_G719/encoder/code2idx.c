/*--------------------------------------------------------------------------*/
/* ITU-T G.722.1 Fullband Extension Annex X. Source Code                    */
/* © 2008 Ericsson AB. and Polycom Inc.                                     */
/* All rights reserved.                                                     */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*  Function  code2idx                                                      */
/*  ~~~~~~~~~~~~~~~~~~~                                                     */
/*                                                                          */
/*  Finding the index of a codevector                                       */
/*--------------------------------------------------------------------------*/
/*  short       *x  (i)  codevector                                         */
/*  short       *k  (o)  index of the codevector                            */
/*  short       r   (i)  number of indices                                  */
/*--------------------------------------------------------------------------*/
void code2idx(short *x, short *k, short r)
{
    short i, m, v;

    m = x[0];
    v = r - 1;
    for (i=1; i<8; i++)
    {
       m -= x[i];
    }
    m >>= 1;
    k[0] = m & v;
    for (i=1; i<8; i++)
    {
       k[i] = x[i] & v;
    }

    return;
}
