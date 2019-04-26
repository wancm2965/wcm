/*********************************************************************
* This file implments the functions of satrasation enhancement with enhancement 
* level for image enhancement. 
*
* Author:	Li Renjie
*			2011/11/11
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


using namespace std;

#define COLOR_ENHANC_LEVELS		16
#define COLOR_ENHANC_STEP		0.1
#define M_PI					3.14159265358979323846


typedef struct tagSatraEnhancD_t
{
	int nW;
	int nH;
	int enhanc_levels;
} SatraEnhancD_t,*pSatraEnhancD_t;


static short int DY[COLOR_ENHANC_LEVELS][256][256];
static short int DU[COLOR_ENHANC_LEVELS][256][256];
static short int DV[COLOR_ENHANC_LEVELS][256][256];

static int beInitFlg = 0;


/* satra_enhanc_proc_init initializes the struct of current process.
*  It needs to be released after enhancement.
*
*  Parameters:
*			nW: width of current image
*			nH: height of current image
*			enhanc_levels: the level number of current process
*
*  Return value:
*			struct pointer: successful
*		   -1: error 
*		   -2: no input
*/
void *satra_enhanc_proc_init(int nW, int nH, int enhanc_levels = 16)
{
	int level,u,v;

	pSatraEnhancD_t pSatraEnhancD = (pSatraEnhancD_t)malloc(sizeof(SatraEnhancD_t));
	
	pSatraEnhancD->enhanc_levels = enhanc_levels;
	pSatraEnhancD->nW = nW/2;
	pSatraEnhancD->nH = nH/2;

	if(beInitFlg)
	{
		return pSatraEnhancD;
	}
	//initialize the mapping table
	float alpha = 0;
	float beta = 0;

	int D,E;

	float H;
	float C;

	float deg;

	for(level=0;level<enhanc_levels;level++)
	{
		for(u=0;u<256;u++)
			for(v=0;v<256;v++)
			{
				
				int R,G,B;
				int M,m;
				//int C;

				D = u - 128;
				E = v - 128;


				R = 1.140*E;
				G = (-0.395)*D - 0.58*E;
				B = 2.032*D;

				M = max(R,G);
				M = max(M,B);

				m = min(R,G);
				m = min(m,B);

				C = M - m;

				if(C==0 || (u<16 && v<16) || (u>240 && v>240))
				{
					DY[level][u][v] = 0;
					DU[level][u][v] = 0;
					DV[level][u][v] = 0;
					continue;
				}

				if(M==R)
				{
					H = fmod(((G - B)/C),6);
					H = H*60;
				}
				if(M==G)
				{
					H = (B - R)/C + 2.0;
					H = H*60;
				}
				if(M==B)
				{
					H = (R - G)/C + 4.0;
					H = H*60;
				}
				
				//to degree
				//deg = ((H+M_PI)*360)/(M_PI*2);
				if(H<0)
					H += 360;

				if(H>=360)
					H -= 360;

				deg = H/60;

				float dR = 0;
				float dG = 0;
				float dB = 0;
				//RGB
				//if((deg>=0 && deg<0.25) || (deg>=5.75 && deg<0))
				if(deg>=0 && deg<1)
				{
					dR = 0*level;
					dG = 0 - level*COLOR_ENHANC_STEP*C*fabs(fmod(deg,2) - 1);
					dB = 0 - level*COLOR_ENHANC_STEP*C;
				}

				//(X,C,0)
				if(deg>=1 && deg<2)
				//if(deg>=0.75 && deg<1.25)
				{
					dR = 0 - level*COLOR_ENHANC_STEP*C*fabs(fmod(deg,2) - 1);
					dG = 0*level;
					dB = 0 - level*COLOR_ENHANC_STEP*C;
				}
				//(0,C,X)
				if(deg>=2 && deg<3)
				//if(deg>=1.75 && deg<2.25)
				{
					dR = 0 - level*COLOR_ENHANC_STEP*C;
					dG = 0*level;
					dB = 0 - level*COLOR_ENHANC_STEP*C*fabs(fmod(deg,2) - 1);
				}
				//(0,X,C)
				if(deg>=3 && deg<4)
				//if(deg>=2.75 && deg<3.25)
				{
					dR = 0 - level*COLOR_ENHANC_STEP*C;
					dG = 0 - level*COLOR_ENHANC_STEP*C*fabs(fmod(deg,2) - 1);
					dB = 0*level;
				}
				//(X,0,C)
				if(deg>=4 && deg<5)
				//if(deg>=3.75 && deg<4.25)
				{
					dR = 0 - level*COLOR_ENHANC_STEP*C*fabs(fmod(deg,2) - 1);
					dG = 0 - level*COLOR_ENHANC_STEP*C;
					dB = 0*level;
				}
				//(C,0,X)
				if(deg>=5 && deg<6)
				//if(deg>=4.75 && deg<5.25)
				{
					dR = 0*level;
					dG = 0 - level*COLOR_ENHANC_STEP*C;
					dB = 0 - level*COLOR_ENHANC_STEP*C*fabs(fmod(deg,2) - 1);
				}

				short int DY_t = 0.257*dR + 0.504*dG + 0.098*dB + 0.5;
				short int DU_t = -0.148*dR - 0.291*dG + 0.439*dB + 0.5;
				short int DV_t = 0.439*dR - 0.368*dG - 0.071*dB + 0.5;
					
				float r_u=0;
				float r_v=0;
				float r_y=0;

				int Y_t=0;
				int U_t=0;
				int V_t=0;

				U_t = u + DU_t;
				if(U_t<16)
				{
					r_u = (u-16)/(float)abs(DU_t);

					DY_t = DY_t*r_u + 0.5;
					DU_t = DU_t*r_u + 0.5;
					DV_t = DV_t*r_u + 0.5;
				}
				V_t = v + DV_t;
				if(V_t<16)
				{
					r_v = (v-16)/(float)abs(DV_t);

					DY_t = DY_t*r_v + 0.5;
					DU_t = DU_t*r_v + 0.5;
					DV_t = DV_t*r_v + 0.5;
				}

				//
				U_t = u + DU_t;
				if(U_t>240)
				{
					r_u = abs(240 - u)/(float)abs(DU_t);

					DY_t = DY_t*r_u + 0.5;
					DU_t = DU_t*r_u + 0.5;
					DV_t = DV_t*r_u + 0.5;
				}
				V_t = v + DV_t;
				if(V_t>240)
				{
					r_v = abs(240 - v)/(float)abs(DV_t);

					DY_t = DY_t*r_v + 0.5;
					DU_t = DU_t*r_v + 0.5;
					DV_t = DV_t*r_v + 0.5;
				}
			

				DY[level][u][v] = DY_t;
				DU[level][u][v] = DU_t;
				DV[level][u][v] = DV_t;
	
			}

		
	}//level

	//filter



	beInitFlg = 1;
	return pSatraEnhancD;	
}

void satra_enhanc_proc_exit(void *pSatra)
{
	if(pSatra)
		free(pSatra);
}

/* imag_satra_enhanc_proc enhances the color of current image with format YUV420.
*  The enhancement level number is 16. From 0 to 15 (strongest).
*
*  Parameters:
*			pSatra: struct allocated by satra_enhanc_proc_init
*			pY: the gray image channel
*			pU: U channel
*			pV: V channel
*			enhanc_level: enhancement level
*
*  Return value:
*			0: successful
*		   -1: error 
*		   -2: no input
*/
int imag_satra_enhanc_proc(void *pSatra,unsigned char *pY,unsigned char *pU, unsigned char *pV,int enhanc_level)
{
	int h,w;
	int size;
	int px;
	int Y,U,V;
	int dY,dU,dV;

	int nW,nH;
	int levels;

	if(pSatra==0 || pY == 0 || pU == 0 || pV == 0)
		return -2;

	pSatraEnhancD_t pSatr = (pSatraEnhancD_t)pSatra;

	nW = pSatr->nW;
	nH = pSatr->nH;

	levels = pSatr->enhanc_levels;

	if(enhanc_level>levels)
		enhanc_level = levels - 1;


	for(h=0;h<nH;h++)
		for(w=0;w<nW;w++)
		{
			U = pU[h*nW+w];
			V = pV[h*nW+w];
			dU = DU[enhanc_level][U][V];
			dV = DV[enhanc_level][U][V];
			dY = DY[enhanc_level][U][V];

			U = U + dU;
			V = V + dV;
			if(U>240)
				U = 240;
			if(U<16)
				U = 16;
			if(V>240)
				V = 240;
			if(V<16)
				V = 16;

			pU[h*nW+w] = U;
			pV[h*nW+w] = V;

			dY = dY>>(7 - (enhanc_level>>2));

			
			
			Y = pY[(h*2)*(nW*2) + w*2];
			Y = Y +dY;

			if(Y>235)
				Y = 235;
			if(Y<16)
				Y = 16;
			pY[(h*2)*(nW*2) + w*2] = Y;

			Y = pY[(h*2)*(nW*2) + w*2 + 1];
			Y = Y +dY;

			if(Y>235)
				Y = 235;
			if(Y<16)
				Y = 16;

			pY[(h*2)*(nW*2) + w*2 + 1] = Y;
			
			Y = pY[(h*2)*(nW*2) + nW*2 + w*2 + 0];
			Y = Y +dY;

			if(Y>235)
				Y = 235;
			if(Y<16)
				Y = 16;
			pY[(h*2)*(nW*2) + nW*2 + w*2 + 0] = Y;

			Y = pY[(h*2)*(nW*2) + nW*2 + w*2 + 1];
			Y = Y +dY;

			if(Y>235)
				Y = 235;
			if(Y<16)
				Y = 16;
			pY[(h*2)*(nW*2) + nW*2 + w*2 + 1] = Y;
	
		}

	

	return 0;
}
