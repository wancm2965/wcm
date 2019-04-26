#include "motion_comp.h"

#include <malloc.h>
#include <assert.h>
#include "../utils/mem_transfer.h"


int ReQuantMType[] =  {1,1,1,1,0,1,1,0,1,1}; /* Quantization used */
int QuantMType[] =    {0,1,0,1,0,0,1,0,0,1}; /* Quantization used */
int CBPMType[] =      {0,0,1,1,0,1,1,0,1,1}; /* CBP used in coding */
int IntraMType[] =    {1,1,0,0,0,0,0,0,0,0}; /* Intra coded macroblock */
int MFMType[] =       {0,0,0,0,1,1,1,1,1,1}; /* Motion forward vector used */
int FilterMType[] =   {0,0,0,0,0,0,0,1,1,1}; /* Filter flags */
int TCoeffMType[] =   {1,1,1,1,0,1,1,0,1,1}; /* Transform coeff. coded */	

int BlockJ[] = {0,0,0,0,1,2};
int BlockV[] = {0,0,1,1,0,0};
int BlockH[] = {0,1,0,1,0,0};

int bit_set_mask[] =
{0x00000001,0x00000002,0x00000004,0x00000008,
0x00000010,0x00000020,0x00000040,0x00000080,
0x00000100,0x00000200,0x00000400,0x00000800,
0x00001000,0x00002000,0x00004000,0x00008000,
0x00010000,0x00020000,0x00040000,0x00080000,
0x00100000,0x00200000,0x00400000,0x00800000,
0x01000000,0x02000000,0x04000000,0x08000000,
0x10000000,0x20000000,0x40000000,0x80000000};


u_short scan_tables[] = 
{							/* zig_zag_scan */
0, 1, 8, 16, 9, 2, 3, 10,
17, 24, 32, 25, 18, 11, 4, 5,
12, 19, 26, 33, 40, 48, 41, 34,
27, 20, 13, 6, 7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36,
29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46,
53, 60, 61, 54, 47, 55, 62, 63};


EHUFF *MakeEhuff(int n)
{
	
	int i;
	EHUFF *temp;
	temp = (EHUFF*)malloc(sizeof(EHUFF));
	temp->n = n;
	temp->Hlen = (int *) calloc(n,sizeof(int));
	temp->Hcode = (int *) calloc(n,sizeof(int));
	for(i=0;i<n;i++)
    {
		temp->Hlen[i] = -1;
		temp->Hcode[i] = -1;
    }
	return(temp);
}

void UnMakeEhuff(EHUFF *temp)
{
	free(temp->Hlen); 
	free(temp->Hcode);
	free(temp);
}

void LoadETable(int* array,EHUFF* table)
{
	while(*array>=0)
    {
		if (*array>table->n)
		{
		//	printf("Table overflow.\n");
			return;
		}
		table->Hlen[*array] = array[1];
		table->Hcode[*array] = array[2];
		array+=3;
    }
}

int Bpos(int ImageType,int g, int m)
{
	
	switch (ImageType)
    {
    case IT_QCIF:
		return((m % 11) +((g>>1) * 3 + m / 11)* 11); //g:0,2,4
		break;
    case IT_CIF:
  		return(((g & 1) * 11 + m % 11) + ((g >> 1) * 3 + m / 11)*22);
		break;
    default:
	//	printf("Unknown image type: %d.\n",ImageType);
		break;
    }
	return(0);
}

void GetPos(int ImageType ,int g, int m, int h, int v, int y, int* hpos, int* vpos)
{
	int ver=2;
	int hor=2;
	if(y)
	{
		ver=1;
		hor=1;
	}
	switch (ImageType)
    {
    case IT_QCIF:
		*hpos = (m % 11)*hor + h;
		*vpos = (((g>>1) * 3) + (m / 11))*ver + v;//g:0,2,4
		break;
    case IT_CIF:
    	*hpos = (((g & 1) * 11) + (m % 11))*hor + h;
		*vpos = (((g >> 1) * 3) + (m / 11))*ver + v;
		break;
    default:
	//	printf("Unknown image type: %d.\n",ImageType);
		assert(0);
		break;
    }
}

void ReadBlock(short *store, u_char *src,int stride)
{
	transfer_8to16copy(store,src,stride);
}


void WriteBlock(short *store,u_char *dst,int stride)
{
	transfer_16to8copy(dst,store,stride);
}

void SubCompensate(short* matrix, u_char* cur,u_char* ref,int stride)
{
	transfer_8to16sub(matrix,cur,ref,stride);
}

void AddCompensate(short* matrix,u_char *dst,int stride)
{
	transfer_16to8add(dst,matrix, stride);
}

void LoadFilterMatrix(u_char *memloc,int stride, u_char *output)
{
	int i,j;
	short temp[64];
	short outTemp[64];
	short *ptr,*ptr1,*ptr2,*ptr3;
	u_char *ptrOut;
	for(ptr=temp,i=0;i<BLOCKHEIGHT;i++)
    {
		*(ptr++)=(*(memloc)<<2);
		for(j=1;j<BLOCKWIDTH-1;j++,ptr++)
		{
			*(ptr) =  *(memloc++);
			*(ptr) +=  (*(memloc++) << 1);
			*(ptr) +=  *(memloc--);
		}
		memloc++;
		*(ptr++) = (*(memloc++)<<2);
		memloc = memloc-BLOCKWIDTH+stride;
    }
	for(ptr=outTemp,ptr1=temp,ptr2=temp,ptr3=temp+(BLOCKWIDTH<<1),i=0;i<BLOCKHEIGHT;i++)
    {
		if ((i==0)||(i==7))
		{
			for(j=0;j<BLOCKWIDTH;j++) {*(ptr++) = *(ptr2++);}
		}
		else
		{
			for(j=0;j<BLOCKWIDTH;j++)
			{
				*(ptr) = (*(ptr2++)<<1);
				*(ptr) +=  *(ptr1++);
				*(ptr) +=  *(ptr3++);
				*ptr = (*ptr>>2);
				ptr++;
			}
		}
    }
	for(ptr=outTemp,ptrOut=output,ptr1=outTemp,i=0;i<BLOCKHEIGHT;i++)
    {
		for(j=0;j<BLOCKWIDTH;j++,ptr++,ptrOut++)
		{
			if (*ptr & 2) {*ptrOut=(*ptr>>2)+1;}
			else {*ptrOut=(*ptr>>2);}
		}
    }
}


void SubFCompensate(short* matrix, u_char *cur,u_char* ref, int stride)
{
	u_char temp[64];

	LoadFilterMatrix(ref,stride,temp);
	transfer_8to16sub_ext(matrix,cur,stride,temp,8);

}

void BoundDctMatrix(short* matrix)
{
	int k=32;
	for(;k<64;k++)
	{
		matrix[scan_tables[k]]=0;
	}
}
