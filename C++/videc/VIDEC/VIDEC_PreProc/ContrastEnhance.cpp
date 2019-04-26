/*********************************************************************
* This file implments the functions of contrast enhancement with control 
* parameter for contrast enhancement. 
*
* Author:	Li Renjie
*			2012/01/15
* Revised:  
* Email:	lrjie2000@gmail.com
* 
* Description:
*
**************************************************************************/

#include <stdio.h>
#include <math.h>
#include <vector>
#include <omp.h>
#include <emmintrin.h>
#ifdef __ENABLE_CONTRAST_ENHANCE__

#define ENHANCE_IMAGESLICE_COUNT		32		
#define ENHANCE_LEVEL_STEP				3
#define ENHANCE_LEVELS					20
#define ENHANCE_GAIN					1
#define ENHANCE_SIGMOID_GAIN			0.018
#define ENHANCE_SHIFT_THR				40

typedef struct tagCONTRAST_ENHANCE
{
	int					nWidth;
	int					nHeight;
	int					nSize;
	int					nAlpha;
	int					nPosEnd;
	int					nStep;

	short int			contrast_enhance_table_logf[256];
	unsigned char*		u8GaussY;
	unsigned char*      u8Diff;

} CONTRAST_ENHANCE,*PCONTRAST_ENHANCE;

static _CRT_ALIGN(2) short int g_contrast_enhance_table_logf[1024];
static  int g_flag_contrast_enhance_initialized = 0;
static unsigned char g_D[64] = {0};
static int meanlog2Arr[256] = {0};

static short int gain_map_build_loopup_var[40][512] = {0};
static float gain_map_build_loopup_var_factor[40] = {
													 0.00, 0.17, 0.33, 0.50, 0.67, 0.83, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
													 0.82, 0.64, 0.46, 0.28, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10,
													 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10};
static short int gain_map_build_loopup_varlimit[256] = {
													   0,  0,  1,  2,  3,  4,  5,  5,  6,  7,  8,  9,  9, 10, 11, 12,
													  13, 13, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 21, 22, 23, 24,
													  24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 32, 33, 33,
													  34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 39, 40, 40, 40, 40,
													  40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
													  40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
													  40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
													  40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
													  40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
													  40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
													  40, 40, 39, 39, 39, 39, 38, 38, 38, 37, 37, 37, 36, 36, 36, 36,
													  35, 35, 35, 34, 34, 34, 33, 33, 33, 32, 32, 32, 32, 31, 31, 31,
													  30, 30, 30, 29, 29, 29, 28, 28, 28, 28, 27, 27, 27, 26, 26, 26,
													  26, 25, 25, 25, 24, 24, 24, 24, 23, 23, 23, 23, 22, 22, 22, 21,
													  21, 21, 21, 20, 20, 20, 20, 19, 19, 19, 19, 19, 18, 18, 18, 18,
													  17, 17, 17, 17, 16, 16, 16, 16, 16, 15, 15, 15, 15, 15, 14, 14}; //gaussian: 256.


/* this function can calculate the gray mean of image.
*  It estimates the mean by using subimage from image
*/
_inline int constrast_enhance_mean_calc(unsigned char*pu8Y,int size,int pxSum)
{
	int mean = 0;
	for(int i=0;i<size;i=i+8)
	{
		mean += pu8Y[i];
	}
	mean = mean/(float)pxSum + 0.5;

	return mean;
}


/* ContrastEnhanceOpen_sse initializes the struct for contrast enhancement.
*  Para:
*      nWidth:		width of image
*      nHeight:		height of image
*      enhanc_level: initial enhancement level. 1~10
*  return value:
*      If successful, return the pointer to the struct.
*/
void *ContrastEnhanceOpen_sse_s_lut(int nWidth, int nHeight,int enhanc_level)
{
    PCONTRAST_ENHANCE pContrast = (PCONTRAST_ENHANCE) _aligned_malloc(sizeof(CONTRAST_ENHANCE),4);
	pContrast->nHeight = nHeight;
	pContrast->nWidth = nWidth;
	pContrast->nSize = nHeight*nWidth;

	pContrast->nStep = pContrast->nSize/ENHANCE_IMAGESLICE_COUNT;
	pContrast->nPosEnd = pContrast->nSize;


	pContrast->u8GaussY = (unsigned char *)MALLOCMSG(sizeof(unsigned char)*nWidth*nHeight);
	memset(pContrast->u8GaussY,0,sizeof(unsigned char)*nWidth*nHeight);


	pContrast->u8Diff = (unsigned char *)MALLOCMSG(sizeof(unsigned char)*nWidth*nHeight);
	memset(pContrast->u8Diff,0,sizeof(unsigned char)*nWidth*nHeight);

	if(g_flag_contrast_enhance_initialized==0)
	{
		g_flag_contrast_enhance_initialized = 1;

		int nRound;
		for(int i=0;i<1024;i++)
		{
			nRound = (logf(1.0 + i)+0.5);
			g_contrast_enhance_table_logf[i] = nRound;
		}

		for(int i=0;i<64;i++)
		{
			g_D[i] = i;
		}
		for(int i=0;i<256;i++)
		{
			meanlog2Arr[i] = logf((i<<4) + 1)/logf(2.0) + 0.5;
		}

		for(int diff=0;diff<40;diff++)
			for(int var=0;var<512;var++)
			{
				int limitedvar = gain_map_build_loopup_var_factor[diff]*(var-256);
				gain_map_build_loopup_var[diff][var] = limitedvar;
			}			

	}

	for(int i=0;i<256;i++)
	{
		pContrast->contrast_enhance_table_logf[i] = g_contrast_enhance_table_logf[i]*(enhanc_level<<ENHANCE_GAIN);
	}
	pContrast->nAlpha = enhanc_level<<ENHANCE_GAIN;

	return pContrast;
}

void ContrastEnhanceClose_sse_s_lut(void *pHandle)
{
	if(pHandle==NULL)
		return;

	PCONTRAST_ENHANCE pContrast = (PCONTRAST_ENHANCE) pHandle;

	if(pContrast->u8GaussY)
		FREEMSG(pContrast->u8GaussY);

	if(pContrast->u8Diff)
		FREEMSG(pContrast->u8Diff);

	_aligned_free(pContrast);
}

/* constrast_enhance_gain_map_build can build the mapping table for gain according gaussian value 
*  and difference between gaussian and mean values.
*
*
*/
inline void constrast_enhance_gain_map_build_sse_s_lut_lookup(unsigned char Map[256][64],int mean,int enhanc_level,short int*contrast_enhance_table_logf)
{
	
	int Gaussian = 0;
	int diff = 0;

	unsigned char G[256] = {0};
	unsigned char *D = &(g_D[0]);

	unsigned char* pu8G = &(G[0]);
	unsigned char* pu8D = D;

	unsigned char *pu8Gend = pu8G + 256;
	unsigned char *pu8Dend = pu8D + 64;

	__m128i Ds = _mm_set1_epi16(32);
	__m128i mmAlpha = _mm_set1_epi16(enhanc_level);
	__m128i m0 = _mm_setzero_si128();

	//the starting address of mapping table
	unsigned char *pu8Map = &(Map[0][0]);

	int meanlog2= meanlog2Arr[mean];

	//using the sse to compute the mapping table
	for(int i = 0;i<256;i++)
	{
		__m128i Gi = _mm_set1_epi16(i);
		__m128i Dbuf = _mm_setzero_si128();

		pu8D = &(D[0]);

		for(;pu8D<pu8Dend;pu8D +=8)
		{
			__m128i Di=_mm_subs_epi16(_mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)pu8D),m0),Ds);		
			__m128i	mmSqrt = _mm_slli_epi16(_mm_mullo_epi16(_mm_mullo_epi16(Di,mmAlpha),_mm_max_epi16(_mm_sub_epi16(m0,Di),Di)),4);
			_mm_storel_epi64(&Dbuf,_mm_packus_epi16(_mm_add_epi16(_mm_srai_epi16(mmSqrt,meanlog2),Gi),m0));

			Dbuf.m128i_u8[0] = contrast_enhance_table_logf[Dbuf.m128i_u8[0]];
			Dbuf.m128i_u8[1] = contrast_enhance_table_logf[Dbuf.m128i_u8[1]];
			Dbuf.m128i_u8[2] = contrast_enhance_table_logf[Dbuf.m128i_u8[2]];
			Dbuf.m128i_u8[3] = contrast_enhance_table_logf[Dbuf.m128i_u8[3]];
			Dbuf.m128i_u8[4] = contrast_enhance_table_logf[Dbuf.m128i_u8[4]];
			Dbuf.m128i_u8[5] = contrast_enhance_table_logf[Dbuf.m128i_u8[5]];
			Dbuf.m128i_u8[6] = contrast_enhance_table_logf[Dbuf.m128i_u8[6]];
			Dbuf.m128i_u8[7] = contrast_enhance_table_logf[Dbuf.m128i_u8[7]];


			//Dbuf.m128i_u8[0] = Dbuf.m128i_u8[0]
			_mm_storel_epi64((__m128i*)pu8Map,_mm_packus_epi16(_mm_add_epi16(_mm_srai_epi16(_mm_mullo_epi16(_mm_unpacklo_epi8(_mm_loadl_epi64(&Dbuf),m0),Di),ENHANCE_LEVEL_STEP),Gi),m0));
			
			for(int j=0;j<8;j++)
			{
				//loop up table for mapping value
				int diff = abs(pu8D[j] - 32);
				int var = pu8Map[j] - i;
				var = gain_map_build_loopup_var[diff][var+256];
				int varlimit = gain_map_build_loopup_varlimit[i];
				if(var>varlimit)
					var = varlimit;
				else
				{
					if(var<-varlimit)
						var = -varlimit;
				}
				pu8Map[j] = i + var;

			}
			
			pu8Map += 8;
		}
	}

}


/*
	constrast_enhance_gauss_diff_calc_sse_s_tmpl uses the new template to calculate the gaussian and difference.
	diff = (4X4 - X0 - X2 - X6 - X8)/16
*/
inline void constrast_enhance_gauss_diff_calc_sse_s_lut_tmpl(unsigned char *pY,int nWidth,int nHeight,unsigned char *u8Gauss,unsigned char *u8Diff)
{
	//buffers for previous line, current line, and next line
	unsigned char *pU8Y_curr = 0;
	unsigned char *pU8Y_next = 0;
	unsigned char *pU8Y_prev = 0;

	__m128i Ds = _mm_set1_epi16(32);
	__m128i m0 = _mm_setzero_si128();
	__m128i Dmax = _mm_set1_epi16(63);


	//variables for SSE2
	__m128i  _xmm0 = {0};  //unit in previous line
	__m128i  _xmm1 = {0};  //unit in current line
	__m128i  _xmm2 = {0};  //unit in next line
	__m128i  _xmm3 = {0};  //
	__m128i  _xmm5 = {0};  //
	__m128i  _xmm6 = {0};  //mean
	__m128i  _xmm7 = {0};  //gaussian

	//int LineLen = sizeof(unsigned char)*(nWidth);
	int LineLen = nWidth;

	//each unit includes 14 pixels.
	int sse_load_num = nWidth/6 - 1;
	int sse_load_num_left = nWidth%6;

	//_asm
	//{
	//	emms
	//}

	pU8Y_prev = pY;
	pU8Y_curr = pY + LineLen;
	pU8Y_next = pY + (LineLen<<1);

	u8Diff += nWidth;
	u8Gauss += nWidth;


	for (int i=1;i<nHeight-1;++i)
	{
		
		unsigned char *pU8Y_curr_end = pU8Y_curr + LineLen;

		_xmm0 = _mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)pU8Y_prev),m0);
		_xmm1 = _mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)pU8Y_curr),m0);
		_xmm2 = _mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)pU8Y_next),m0);

		//set _xmm7 to zero to use to save results 
		//_xmm7 = _mm_setzero_si128(); //gaussian
		//_xmm6 = _mm_setzero_si128(); //mean

		// first compute the first 8 pixels.i.e., low 8 bytes
		//begin to compute the gaussian value
		_xmm3 = _mm_add_epi16(_mm_slli_si128(_xmm0,2),_mm_srli_si128(_xmm0,2)); 
		_xmm5 = _mm_add_epi16(_mm_slli_si128(_xmm2,2),_mm_srli_si128(_xmm2,2));

		_xmm6 = _mm_add_epi16(_xmm3,_xmm5);//(X0 + X2 + X6 + X8)

		//current line
		_xmm7 = _mm_slli_epi16(_xmm1,2);  //*4. current pixel
		
		//diff
		//_xmm3 = _mm_sub_epi16(_xmm7,_xmm6);
		_xmm3 = _mm_srai_epi16(_mm_sub_epi16(_xmm7,_xmm6),4);

		_xmm7 = _mm_add_epi16(_xmm6,_xmm7);


		_xmm5 =	_mm_add_epi16(_mm_add_epi16(_xmm0,_xmm2),_mm_add_epi16(_mm_slli_si128(_xmm1,2),_mm_srli_si128(_xmm1,2)));
		_xmm7 =	_mm_add_epi16(_xmm7,_mm_slli_epi16(_xmm5,1));

		//gaussin
		_xmm7 = _mm_srai_epi16(_xmm7,4); ///16. OK

		//value - mean
		//_xmm4 = _mm_packus_epi16(_mm_adds_epi16(_mm_sub_epi16(_xmm7,_xmm6),Ds),m0);
		_mm_storel_epi64((__m128i*)u8Diff,_mm_packus_epi16(_mm_min_epi16(_mm_adds_epi16(_xmm3,Ds),Dmax),m0));
		_mm_storel_epi64((__m128i*)u8Gauss,_mm_packus_epi16(_xmm7,m0));

		u8Diff += 7;
		u8Gauss += 7;

		pU8Y_prev += 6;
		pU8Y_curr += 6;
		pU8Y_next += 6;

		//for(;pU8Y_curr<pU8Y_curr_end;)
		for(int load_num = 0;load_num < sse_load_num; load_num++)
		{
			_xmm0 = _mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)pU8Y_prev),m0);
			_xmm1 = _mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)pU8Y_curr),m0);
			_xmm2 = _mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)pU8Y_next),m0);

			//set _xmm7 to zero to use to save results 
			_xmm3 = _mm_add_epi16(_mm_slli_si128(_xmm0,2),_mm_srli_si128(_xmm0,2)); 
			_xmm5 = _mm_add_epi16(_mm_slli_si128(_xmm2,2),_mm_srli_si128(_xmm2,2));

			_xmm6 = _mm_add_epi16(_xmm3,_xmm5);//(X0 + X2 + X6 + X8)

			//current line
			_xmm7 = _mm_slli_epi16(_xmm1,2);  //*4. current pixel
			
			//diff
			_xmm3 = _mm_srai_epi16(_mm_sub_epi16(_xmm7,_xmm6),4);

			_xmm7 = _mm_add_epi16(_xmm6,_xmm7);

			_xmm5 =	_mm_add_epi16(_mm_add_epi16(_xmm0,_xmm2),_mm_add_epi16(_mm_slli_si128(_xmm1,2),_mm_srli_si128(_xmm1,2)));
			_xmm7 =	_mm_add_epi16(_xmm7,_mm_slli_epi16(_xmm5,1));

			//gaussin
			_xmm7 = _mm_srai_epi16(_xmm7,4); ///16. OK
			//mean
			//_xmm6 = _mm_srai_epi16(_xmm3,4); ///16. OK

			//value - mean
			//_xmm4 = _mm_packus_epi16(_mm_adds_epi16(_mm_sub_epi16(_xmm7,_xmm6),Ds),m0);
			_mm_storel_epi64((__m128i*)u8Diff,_mm_srli_si128(_mm_packus_epi16(_mm_min_epi16(_mm_adds_epi16(_xmm3,Ds),Dmax),m0),1));
			_mm_storel_epi64((__m128i*)u8Gauss,_mm_srli_si128(_mm_packus_epi16(_xmm7 ,m0),1));

			u8Diff += 6;
			u8Gauss += 6;

			pU8Y_prev += 6;
			pU8Y_curr += 6;
			pU8Y_next += 6;		
		}

		//process last pixels
		int pxnum = pU8Y_curr_end - pU8Y_curr;
		if(pxnum<=1)
		{
			u8Diff += pxnum-1;
			u8Gauss += pxnum-1;

			pU8Y_prev += pxnum;
			pU8Y_curr += pxnum;
			pU8Y_next += pxnum;	
			continue;
		}

		_xmm0 = _mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)pU8Y_prev),m0);
		_xmm1 = _mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)pU8Y_curr),m0);
		_xmm2 = _mm_unpacklo_epi8(_mm_loadl_epi64((const __m128i*)pU8Y_next),m0);

		//set _xmm7 to zero to use to save results 
		_xmm3 = _mm_add_epi16(_mm_slli_si128(_xmm0,2),_mm_srli_si128(_xmm0,2)); 
		_xmm5 = _mm_add_epi16(_mm_slli_si128(_xmm2,2),_mm_srli_si128(_xmm2,2));

		_xmm6 = _mm_add_epi16(_xmm3,_xmm5);//(X0 + X2 + X6 + X8)

		//current line
		_xmm7 = _mm_slli_epi16(_xmm1,2);  //*4. current pixel

		//diff
		//_xmm3 = _mm_sub_epi16(_xmm7,_xmm6);
		_xmm3 = _mm_srai_epi16(_mm_sub_epi16(_xmm7,_xmm6),4);

		_xmm7 = _mm_add_epi16(_xmm6,_xmm7);


		_xmm5 =	_mm_add_epi16(_mm_add_epi16(_xmm0,_xmm2),_mm_add_epi16(_mm_slli_si128(_xmm1,2),_mm_srli_si128(_xmm1,2)));
		_xmm7 =	_mm_add_epi16(_xmm7,_mm_slli_epi16(_xmm5,1));

		//gaussin
		_xmm7 = _mm_srai_epi16(_xmm7,4); ///16. OK

		//value - mean
		//_xmm4 = _mm_packus_epi16(_mm_adds_epi16(_mm_sub_epi16(_xmm7,_xmm6),Ds),m0);
		__m128i u8Diff_line_left;
		__m128i u8Gauss_line_left;
		
		_mm_storel_epi64(&u8Diff_line_left,_mm_srli_si128(_mm_packus_epi16(_mm_min_epi16(_mm_adds_epi16(_xmm3,Ds),Dmax),m0),1));
		_mm_storel_epi64(&u8Gauss_line_left,_mm_srli_si128(_mm_packus_epi16(_xmm7,m0),1));

		memcpy(u8Diff,&u8Diff_line_left,pxnum-1);
		memcpy(u8Gauss,&u8Gauss_line_left,pxnum-1);

		u8Diff += pxnum-1;
		u8Gauss += pxnum-1;

		pU8Y_prev += pxnum;
		pU8Y_curr += pxnum;
		pU8Y_next += pxnum;		
	}

}


void ContrastEnhanceProcess_sse_s_lut(void *pHandle, unsigned char *pY,int enhanc_level)
{

	if (!pHandle)
		return ;

	if(enhanc_level<=0)
		return;

	enhanc_level = 15;

	PCONTRAST_ENHANCE pContrast = (PCONTRAST_ENHANCE) pHandle;

	int nMeanValue = 0;
	int nTotalValue=0;

	int i32DiffHist[256] = {0};


	int nHeight = pContrast->nHeight;
	int nWidth = pContrast->nWidth;
	int nSize = pContrast->nSize;

	unsigned char *u8GaussY = pContrast->u8GaussY;
	unsigned char *u8Diff = pContrast->u8Diff;

	if(enhanc_level==0)
		return;

	if(enhanc_level>ENHANCE_LEVELS)
		enhanc_level = ENHANCE_LEVELS;

	//enhanc_level = ENHANCE_LEVELS*80;
	
	short int nAlpha = (enhanc_level<<ENHANCE_GAIN);

	//build mapping table
	unsigned char Map[256][64] = {0};

	//compute the mean gray
	int mean = constrast_enhance_mean_calc(pY,nSize,(nSize>>3));

	if(pContrast->nPosEnd == nSize)
	{
		if(nAlpha!=pContrast->nAlpha)
		{
			for(int i=0;i<256;i++)
			{
				pContrast->contrast_enhance_table_logf[i] = g_contrast_enhance_table_logf[i]*nAlpha;
			}

			pContrast->nAlpha = nAlpha;
			pContrast->nPosEnd = 0;
		}
		else
		{
			constrast_enhance_gain_map_build_sse_s_lut_lookup(Map,mean,nAlpha,pContrast->contrast_enhance_table_logf);
			
			//compute the Gaussian and difference value
			#pragma omp parallel firstprivate(nSize,nWidth,nHeight,u8GaussY,u8Diff,Map,pY)
			{
				#pragma omp sections  nowait 
				{
					#pragma omp section 
					{
						int nSize0 = (nSize>>1)-nWidth;
						constrast_enhance_gauss_diff_calc_sse_s_lut_tmpl(pY,nWidth,(nHeight>>1),u8GaussY,u8Diff);
						pY += nWidth;
						for(int px=nWidth;px<nSize0;px++)
						{
							*pY++ = Map[u8GaussY[px]][u8Diff[px]];
						}
					}
					#pragma omp section 
					{
						int addrshift = nWidth*((nHeight>>1) - 2);
						constrast_enhance_gauss_diff_calc_sse_s_lut_tmpl(pY + addrshift,nWidth,(nHeight>>1)+2,u8GaussY + addrshift,u8Diff + addrshift);
						pY = pY + (nSize>>1)-nWidth;
						int nSizeEnd = nSize - nWidth;
						for(int px=(nSize>>1)-nWidth;px<nSizeEnd;px++)
						{
							*pY++ = Map[u8GaussY[px]][u8Diff[px]];
						}
					}
				}
			}
		}
	}
	else
	{
		constrast_enhance_gain_map_build_sse_s_lut_lookup(Map,mean,nAlpha,pContrast->contrast_enhance_table_logf);


		#pragma omp parallel firstprivate(nSize,nWidth,nHeight,u8GaussY,u8Diff,Map,pY)
		{
			#pragma omp sections  nowait 
			{
				#pragma omp section 
				{
					constrast_enhance_gauss_diff_calc_sse_s_lut_tmpl(pY,nWidth,nHeight>>1,u8GaussY,u8Diff);
				}
			
				#pragma omp section
				{
					int addrshift = nWidth*((nHeight>>1) - 2);
					constrast_enhance_gauss_diff_calc_sse_s_lut_tmpl(pY + addrshift,nWidth,(nHeight>>1)+2,u8GaussY + addrshift,u8Diff + addrshift);
				}
			}
		}

		pContrast->nPosEnd += pContrast->nStep;
		pY += nWidth;
		for(int px=nWidth;px<pContrast->nPosEnd;px++)
		{
			//if(u8Diff[px]!=32 && u8Diff[px]!=31 && u8Diff[px] !=33)
				*pY++ = Map[u8GaussY[px]][u8Diff[px]];
		
		}

		//pContrast->nPosEnd = nSize;

	}

}

#endif