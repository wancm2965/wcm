#ifndef _MOTION_COMP_H
#define _MOTION_COMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "..\h261\config.h"

#define IT_QCIF	0
#define IT_CIF	1

#define COMPARISON >=  /* This is to compare for short-circuit exit */
/* DCT Coefficient Thresholds for coding blocks */

/* MC Threshold for coding blocks through filter*/

#define D_FILTERTHRESHOLD 6

/* Intra forced every so many blocks */

#define SEQUENCE_INTRA_THRESHOLD 131
#define BLOCKWIDTH 8
#define BLOCKHEIGHT 8
#define BLOCKSIZE 64
#define HUFFMAN_ESCAPE 0x1b01

	
#define EHUFF struct Modified_Encoder_Huffman

EHUFF
{
	int n;
	int *Hlen;
	int *Hcode;
};

extern EHUFF *MBAEHuff;
extern EHUFF *MVDEHuff;
extern EHUFF *CBPEHuff;
extern EHUFF *T1EHuff;
extern EHUFF *T2EHuff;
extern EHUFF *T3EHuff;

extern int QuantMType[] ; /* Quantization used */
extern int ReQuantMType[] ; /* Quantization used */	
extern int CBPMType[] ; /* CBP used in coding */
extern int IntraMType[] ; /* Intra coded macroblock */
extern int MFMType[] ; /* Motion forward vector used */
extern int FilterMType[] ; /* Filter flags */
extern int TCoeffMType[] ; /* Transform coeff. coded */	

extern int BlockJ[] ;
extern int BlockV[] ;
extern int BlockH[] ;

extern  int bit_set_mask[] ;

extern  u_short scan_tables[];


extern EHUFF* MakeEhuff(int n);
extern void   UnMakeEhuff(EHUFF *temp);
extern void   LoadETable(int* array,EHUFF* table);
extern void   InitHuff();

extern  int Bpos(int ImageType,int g, int m);
extern void GetPos(int ImageType ,int g, int m, int h, int v, int y, int* hpos, int* vpos);
extern void ReadBlock(short* store,u_char* src,int stride);
extern void WriteBlock(short *store,u_char *dst,int stride);

extern void LoadFilterMatrix(u_char *memloc,int stride,u_char *output);
extern void SubFCompensate(short* matrix, u_char* cur,u_char* ref, int stride);

extern void SubCompensate(short* matrix, u_char* cur,u_char* ref,int stride);
extern void AddCompensate(short* matrix,u_char *src,int stride);

extern void BoundDctMatrix(short* matrix);


#ifdef __cplusplus
}
#endif
#endif //end _P64V122_GLOBAL_H