/*********************************************************************
* This file implments the functions of color enhancement with enhancement 
* level for acvcon cee. 
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
#include <emmintrin.h>


using namespace std;

#define COLOR_ENHANC_LEVELS		16
#define COLOR_ENHANC_STEP		0.03    //0.05 or 0.06
#define M_PI					3.14159265358979323846

#define COLOR_SKIN_GAMMA		1.0

#define COLOR_AXIS_NUM			30

#define COLOR_SF_CENTER         1.5
#define COLOR_SF_CORE           1.5
#define COLOR_SF_TAPER          1.0

#define COLOR_HF_CENTER         0
#define COLOR_HF_CORE           8         
#define COLOR_HF_TAPER          2.0

#define COLOR_HF_CENTER_G		1	
#define COLOR_HF_CORE_G         1
#define COLOR_HF_TAPER_G        0.0


//#define COLOR_HF_CENTER_Y		1	
//#define COLOR_HF_CORE_Y         1
//#define COLOR_HF_TAPER_Y        0.0
#define COLOR_HF_CENTER_Y		1	
#define COLOR_HF_CORE_Y         0.6
#define COLOR_HF_TAPER_Y        0.0

//#define COLOR_HF_CENTER_R		1	
//#define COLOR_HF_CORE_R         1
//#define COLOR_HF_TAPER_R        0.0

//reduce the factor to avoid too bright red color
#define COLOR_HF_CENTER_R		0.8	
#define COLOR_HF_CORE_R         0.4
#define COLOR_HF_TAPER_R        0.0

#define COLOR_HF_CENTER_M		1	
#define COLOR_HF_CORE_M         1
#define COLOR_HF_TAPER_M        0.0

#define COLOR_HF_CENTER_B		1	
#define COLOR_HF_CORE_B         1
#define COLOR_HF_TAPER_B        0.0

#define COLOR_HF_CENTER_C		1	
#define COLOR_HF_CORE_C         0.6
#define COLOR_HF_TAPER_C        0.0

//
#define COLOR_SF_CENTER_G		1.3	
#define COLOR_SF_CORE_G         1.2
#define COLOR_SF_TAPER_G        1.0

#define COLOR_SF_CENTER_Y		1.3	
#define COLOR_SF_CORE_Y         1.2
#define COLOR_SF_TAPER_Y        1.0

//#define COLOR_SF_CENTER_R		1.3	
//#define COLOR_SF_CORE_R         1.2
//#define COLOR_SF_TAPER_R        1.0
//reduce the factor to avoid too bright red color
#define COLOR_SF_CENTER_R		1.1	
#define COLOR_SF_CORE_R         1.0
#define COLOR_SF_TAPER_R        1.0

#define COLOR_SF_CENTER_M		1.3	
#define COLOR_SF_CORE_M         1.2
#define COLOR_SF_TAPER_M        1.0

#define COLOR_SF_CENTER_B		1.3	
#define COLOR_SF_CORE_B         1.2
#define COLOR_SF_TAPER_B        1.0

#define COLOR_SF_CENTER_C		1.3	
#define COLOR_SF_CORE_C         1.2
#define COLOR_SF_TAPER_C        1.0


typedef struct tagColorEnhancD_t
{
	int nW;
	int nH;
	float enhanc_levels_max;
	int enhanc_level;
	int enhanc_level_h;

	float skin_uvf[256][256];
	float skin_uvf_rediu[360];
	float skin_distmax;
	float skin_u_centr;
	float skin_v_centr;
	int be_skin_read_data;
	int be_skin_preserv;
	int be_color_rotat;

} ColorEnhancD_t,*pColorEnhancD_t;

typedef struct tagUV_t
{
	unsigned char u;
	unsigned char v;
} UV_t,*pUV_t;

static UV_t uvmap[256][256];
static UV_t uvmap_skin[256][256];

static int be_uvmap_flag = 0; 

static float sf[COLOR_AXIS_NUM] = {COLOR_SF_TAPER_G, COLOR_SF_CORE_G, COLOR_SF_CENTER_G, COLOR_SF_CORE_G, COLOR_SF_TAPER_G, 
								   COLOR_SF_TAPER_C, COLOR_SF_CORE_C, COLOR_SF_CENTER_C, COLOR_SF_CORE_C, COLOR_SF_TAPER_C,
								   COLOR_SF_TAPER_B, COLOR_SF_CORE_B, COLOR_SF_CENTER_B, COLOR_SF_CORE_B, COLOR_SF_TAPER_B,
								   COLOR_SF_TAPER_M, COLOR_SF_CORE_M, COLOR_SF_CENTER_M, COLOR_SF_CORE_M, COLOR_SF_TAPER_M,
								   COLOR_SF_TAPER_R, COLOR_SF_CORE_R, COLOR_SF_CENTER_R, COLOR_SF_CORE_R, COLOR_SF_TAPER_R,
								   COLOR_SF_TAPER_Y, COLOR_SF_CORE_Y, COLOR_SF_CENTER_Y, COLOR_SF_CORE_Y, COLOR_SF_TAPER_Y 
								   };

static float hf[COLOR_AXIS_NUM] = {COLOR_HF_TAPER_G, COLOR_HF_CORE_G, COLOR_HF_CENTER_G, COLOR_HF_CORE_G, COLOR_HF_TAPER_G, 
								   COLOR_HF_TAPER_C, COLOR_HF_CORE_C, COLOR_HF_CENTER_C, COLOR_HF_CORE_C, COLOR_HF_TAPER_C,
								   COLOR_HF_TAPER_B, COLOR_HF_CORE_B, COLOR_HF_CENTER_B, COLOR_HF_CORE_B, COLOR_HF_TAPER_B,
								   COLOR_HF_TAPER_M, COLOR_HF_CORE_M, COLOR_HF_CENTER_M, COLOR_HF_CORE_M, COLOR_HF_TAPER_M,
								   COLOR_HF_TAPER_R, COLOR_HF_CORE_R, COLOR_HF_CENTER_R, COLOR_HF_CORE_R, COLOR_HF_TAPER_R,
								   COLOR_HF_TAPER_Y, COLOR_HF_CORE_Y, COLOR_HF_CENTER_Y, COLOR_HF_CORE_Y, COLOR_HF_TAPER_Y};

//OK
static float axis[COLOR_AXIS_NUM] = {	 29.946,  47.944,  59.967,  70.233,  88.994, //G
										 88.995,  98.467, 106.123, 114.305, 129.117, //C
									    129.118, 147.328, 167.307, 187.815, 209.945, //B
									    209.946, 227.944, 239.772, 250.089, 261.932, //M
									    261.933, 274.502, 286.243, 294.305, 309.117, //R
									    309.118, 327.328, 347.307,   0.100,   9.945};//Y


int color_enhanc_calc_map_pt(unsigned char U,unsigned char V,unsigned char *Uo,unsigned char *Vo,float ghf,float gsf,float skin_f,int rotat)
{
	float lsf = 1.0;
	float lhf = 1.0;

	float degree = 0.0;

	if(U==77 && V==163)
		U=U;


	degree = atan2((float)((int)V-128),(float)((int)U-128)) + M_PI;
	degree = degree*360.0/(2*M_PI);

	if(degree>8 && degree<9)
		degree = degree;

	//first compute the local saturation factor
	//which range
	int icolor  = 0;
	int subicolor = 0;

	float sf_high = 0;
	float sf_low = 0;

	float hf_high = 0;
	float hf_low = 0;

	float axis_high = 0;
	float axis_low = 0;

	float dangle = 0;

	bool beEdge = false;
	
	for(icolor=0;icolor<COLOR_AXIS_NUM-1;icolor++)
	{
		if(icolor==27)
		{
			if((degree>=axis[icolor] && degree<360) || (degree>=0 && degree<axis[icolor+1]))
			{	
				axis_high = axis[icolor+1];
				axis_low = axis[icolor];
				
				sf_high = sf[icolor+1];
				sf_low = sf[icolor];

				hf_high = hf[icolor+1];
				hf_low = hf[icolor];

				beEdge = true;

				break;
			}
			continue;
		}

		if(degree>=axis[icolor] && degree<axis[icolor+1])
		{	
			axis_high = axis[icolor+1];
			axis_low = axis[icolor];
			
			sf_high = sf[icolor+1];
			sf_low = sf[icolor];

			hf_high = hf[icolor+1];
			hf_low = hf[icolor];
			break;
		}
	}

	//edge
	if(icolor==COLOR_AXIS_NUM-1)
	{
		axis_high = axis[0];
		axis_low = axis[icolor];
		sf_high = sf[0];
		sf_low = sf[icolor];
		hf_high = hf[0];
		hf_low = hf[icolor];		
	}
	else
	{
		if(!beEdge)
		{
			lsf = sf_high + (sf_high - sf_low)*(degree-axis_high)/(axis_high - axis_low);
			lhf = hf_high + (hf_high - hf_low)*(degree-axis_high)/(axis_high - axis_low);

			//if(icolor==2 || icolor==7 || icolor==12 || icolor==17 || icolor==22 || icolor==27)
			//{
			//	dangle = degree - axis_low;
			//}
			//else
			//{
			//	if((icolor+1)==2 || (icolor+1)==7 || (icolor+1)==12 || (icolor+1)==17 || (icolor+1)==22 || (icolor+1)==27)
			//	{
			//		dangle = axis_high - degree;
			//	}		
			//}

			//compute the angle deviation
			//get the main axis
			int mainaxis = (icolor/5)*5 + 2;
			if(icolor%5 !=4)
			{
				if(icolor == COLOR_AXIS_NUM -2)
				{
					dangle = -(degree + 360 - axis[mainaxis]);
				
				}
				else
				{
					if(degree>axis[mainaxis])
					{
						dangle = axis[mainaxis] - degree;
					}
					else
					{
						dangle = axis[mainaxis] - degree;
					}
				}
			
			}

		}
		else
		{
			if(degree>=0 && degree<axis[icolor+1])
			{
				lsf = sf_high + (sf_high - sf_low)*(degree-axis_high)/(axis_high + 360 - axis_low);
				lhf = hf_high + (hf_high - hf_low)*(degree-axis_high)/(axis_high + 360 - axis_low);	

				//calculate the angle between current axis and main axis Y
				dangle = -(degree + 360 - axis_low);

			}
			else
			{
				lsf = sf_high + (sf_high - sf_low)*(degree-360-axis_high)/(axis_high + 360 - axis_low);
				lhf = hf_high + (hf_high - hf_low)*(degree-360-axis_high)/(axis_high + 360 - axis_low);	

				dangle = -(degree - axis_low);
				
			}			
		}
	}

	//lhf = lhf*2.0*M_PI/360 + ghf;

	lhf = lhf + ghf;

	if(lhf>1.0)
		lhf = 1.0;

	/*
	if(lhf>0)
		lhf = 1;
	else
	{
		if(lhf<0)
			lhf = -1;
	}
	*/

	dangle = dangle*2.0*M_PI/360*skin_f;

	float angle_c = M_PI*skin_f/3*rotat;

	//lsf = lsf*2.0*M_PI/360;

	//float u = (cosf(lhf)*((int)U - 128) + sinf(lhf)*((int)V - 128));
	//float v = (-sinf(lhf)*((int)U - 128) + cosf(lhf)*((int)V - 128));

	float u,v;
/*	u = (cosf(lhf*dangle)*((int)U - 128) + sinf(lhf*dangle)*((int)V - 128));
	v = (-sinf(lhf*dangle)*((int)U - 128) + cosf(lhf*dangle)*((int)V - 128));*/	

	u = (cosf(lhf*dangle + angle_c)*((int)U - 128) - sinf(lhf*dangle + angle_c)*((int)V - 128));
	v = (sinf(lhf*dangle + angle_c)*((int)U - 128) + cosf(lhf*dangle + angle_c)*((int)V - 128));

	degree = atan2(v,u) + M_PI;
	degree = degree*360.0/(2*M_PI);	

	//if(lsf<1.0)
	//	lsf = 1.0;
	lsf = (lsf + gsf - 1)*skin_f + 1;
	//if(lsf<1.0)
	//	lsf = 1.0;
	u = u*lsf;
	v = v*lsf;
	//
	if(u>=128)
	{
		float r = 127/u;
		u= u*r;
		v= v*r;
		lsf = lsf*r;
	}
	if(u<-128)
	{
		float r = -128/u;
		u= u*r;
		v= v*r;
		lsf = lsf*r;	
	
	}

	if(v>=128)
	{
		float r = 127/v;
		u= u*r;
		v= v*r;
		lsf = lsf*r;
	}
	if(v<-128)
	{
		float r = -128/v;
		u= u*r;
		v= v*r;
		lsf = lsf*r;	
	}
	//bounded
	u = u + 128 + 0.5;
	v = v + 128 + 0.5;

	if(u>255)
		u = 255;
	if(u<0)
		u = 0;
	if(v>255)
		v = 255;
	if(v<0)
		v = 0;	

	*Uo = (int)u;
    *Vo = (int)v;

	//degree = atan2((float)((int)v-128),(float)((int)u-128)) + M_PI;
	//degree = degree*360.0/(2*M_PI);

	return 0;

}


void image_color_mapping_build(float uvf[256][256],float uvf_rediu[360],int enhanc_level=5,int enhanc_level_h=0,float v_skin_centr=0,float u_skin_centr=0,float skin_dist_max=1,float gamma=1.4,bool skin_preserv=true,int rotat=0)
{
	int U,V;

	if(enhanc_level_h>10)
		enhanc_level_h = 10;
	if(enhanc_level_h<0)
		enhanc_level_h = 0;

	float enhanc_level_f = 0.05*enhanc_level;
	float enhanc_level_h_f = 0.01*enhanc_level_h;

	//compute the mappping 
	if(skin_preserv)
	{
		for(U=0;U<256;U++)
			for(V=0;V<256;V++)
			{
				unsigned char Uo,Vo;
				//preserve skin color
				float skin_f = 1;
				//if(uvf[U][V]!=0)
				//{
				//	float dist = sqrtf((U-u_skin_centr)*(U-u_skin_centr) + (V-v_skin_centr)*(V-v_skin_centr));
				//	skin_f = powf(dist/skin_dist_max,gamma);
				//}
				if(uvf[U][V]!=0)
				{
					float degree = atan2((float)(V-v_skin_centr),(float)(U-u_skin_centr)) + M_PI;
					degree = degree*360.0/(2*M_PI);
					int angleQ = degree/1.0;
					float distmax = uvf_rediu[angleQ];
				    float dist = sqrtf((U-u_skin_centr)*(U-u_skin_centr) + (V-v_skin_centr)*(V-v_skin_centr));
					skin_f = powf(dist/distmax,gamma);

				}

				color_enhanc_calc_map_pt(U,V,&Uo,&Vo,enhanc_level_h_f,enhanc_level_f,skin_f,rotat);
				uvmap[U][V].u = Uo;
				uvmap[U][V].v = Vo;
			}
	}
	else
	{
		for(U=0;U<256;U++)
			for(V=0;V<256;V++)
			{
				unsigned char Uo,Vo;
				//preserve skin color
				color_enhanc_calc_map_pt(U,V,&Uo,&Vo,enhanc_level_h_f,enhanc_level_f,1.0,rotat);
				uvmap[U][V].u = Uo;
				uvmap[U][V].v = Vo;
			}	
	}
}


/* satra_enhanc_proc_init_skin initializes the struct of current process.
*  It needs to be released after enhancement.
*
*  Parameters:
*			nW: width of current image
*			nH: height of current image
*			enhanc_levels: the level number of current process
*			strSkinDataFile: training result file of skin data
*
*  Return value:
*			struct pointer: successful
*		   -1: error 
*		   -2: no input
*  Description:
*	It initialises the UV mapping tables with skin preserving and without preserving
*/

void *image_color_enhanc_proc_init(int nW, int nH,int enhanc_level=5, int enhanc_level_h=0,unsigned char*pSkinBinData=0,bool skin_preserv=true)
{
	pColorEnhancD_t pColorEnhancD = (pColorEnhancD_t)malloc(sizeof(ColorEnhancD_t));
	memset(pColorEnhancD,0,sizeof(ColorEnhancD_t));
	
	pColorEnhancD->enhanc_levels_max = 10;
	pColorEnhancD->nW = nW;
	pColorEnhancD->nH = nH;
	pColorEnhancD->enhanc_level = enhanc_level;
	pColorEnhancD->enhanc_level_h = enhanc_level_h;
	pColorEnhancD->be_skin_read_data = 0;

	if(be_uvmap_flag)
	{
		return pColorEnhancD;
	}

	float uvf[256][256]={0};
	float uvf_rediu[360]={0};
	float distmax = 0;
	float dist = 0;
	float u_mean = 0;
	float v_mean = 0;

	if(pSkinBinData)
	{
		int nDataLen=sizeof(float)*256*256;
		memcpy((&uvf[0][0]),pSkinBinData,nDataLen);
		pSkinBinData+=nDataLen;
		nDataLen=sizeof(float)*360;
		memcpy((&uvf_rediu[0]),pSkinBinData,nDataLen);
		pColorEnhancD->be_skin_read_data = 1;
	}

	//weighted mean according to the distribution
	if(pColorEnhancD->be_skin_read_data)
	{
		float totolf = 0;
		u_mean = 0;
		v_mean = 0;

		
		for(int u=0;u<256;u++)
			for(int v=0;v<256;v++)
			{
				totolf += uvf[u][v];
			}

		if(totolf==0)
		{
			totolf = 1;
		}

		for(int u=0;u<256;u++)
			for(int v=0;v<256;v++)
			{
				u_mean += u*(uvf[u][v]/totolf);
				v_mean += v*(uvf[u][v]/totolf);
			}
		
	
		pColorEnhancD->skin_distmax = distmax;
		pColorEnhancD->skin_u_centr = u_mean;
		pColorEnhancD->skin_v_centr = v_mean;

		memcpy(&(pColorEnhancD->skin_uvf[0][0]),&(uvf[0][0]),256*256*sizeof(float));
		memcpy(&(pColorEnhancD->skin_uvf_rediu[0]),&(uvf_rediu[0]),360*sizeof(float));


	}
	else
	{
		skin_preserv = false;
	}

	pColorEnhancD->be_skin_preserv = skin_preserv;
	image_color_mapping_build(uvf,uvf_rediu,enhanc_level,enhanc_level_h,v_mean,u_mean,distmax,COLOR_SKIN_GAMMA,skin_preserv,0);

	return pColorEnhancD;	
}

void image_color_enhanc_proc_exit(void *pSatra)
{
	if(pSatra)
		free(pSatra);
}

/* imag_color_enhanc_proc_skin enhances the color of current image with format YUV420.
*  The enhancement level number is 16. From 0 to 15 (strongest).
*
*  Parameters:
*			pSatra: struct allocated by satra_enhanc_proc_init
*			pY: the gray image channel
*			pU: U channel
*			pV: V channel
*			enhanc_level: enhancement level
*           beSkin_preserv: determine whether skin color is preserved. true: YES; false: NO
*
*  Return value:
*			0: successful
*		   -1: error 
*		   -2: no input
*/
int image_color_enhanc_proc(void *pColorEnhancD,unsigned char *pY,unsigned char *pU, unsigned char *pV,int enhanc_level,int enhanc_level_h, bool beSkin_preserv,int rotat)
{
	unsigned char U,V;

	if(pColorEnhancD==0)
		return -2;
	
	pColorEnhancD_t pColorE = (pColorEnhancD_t)pColorEnhancD;

	int nW = pColorE->nW;
	int nH = pColorE->nH;
	float enhance_max = pColorE->enhanc_levels_max;
	int enhanc_level_old = pColorE->enhanc_level;
	int enhanc_level_h_old = pColorE->enhanc_level_h;
	int skin_preserv_old = pColorE->be_skin_preserv;

	if(enhanc_level>pColorE->enhanc_levels_max)
		enhanc_level= pColorE->enhanc_levels_max;

	if(enhanc_level_old!=enhanc_level)
	{
		if(pColorE->be_skin_read_data==0)
			beSkin_preserv = false;
		image_color_mapping_build(pColorE->skin_uvf,pColorE->skin_uvf_rediu,enhanc_level,enhanc_level_h,pColorE->skin_v_centr,pColorE->skin_u_centr,pColorE->skin_distmax,COLOR_SKIN_GAMMA,beSkin_preserv,rotat);
		pColorE->enhanc_level = enhanc_level;
		pColorE->enhanc_level_h = enhanc_level_h;
		pColorE->be_skin_preserv = beSkin_preserv;
		pColorE->be_color_rotat = rotat;
	}
	else
	{
		if(beSkin_preserv!=skin_preserv_old)
		{
			if(pColorE->be_skin_read_data==0)
				beSkin_preserv = false;
			else
			{
				image_color_mapping_build(pColorE->skin_uvf,pColorE->skin_uvf_rediu,enhanc_level,enhanc_level_h,pColorE->skin_v_centr,pColorE->skin_u_centr,pColorE->skin_distmax,COLOR_SKIN_GAMMA,beSkin_preserv,rotat);	
				pColorE->be_skin_preserv = beSkin_preserv;
				pColorE->enhanc_level_h = enhanc_level_h;
				pColorE->be_color_rotat = rotat;
			}
		}
	}

	enhanc_level_h_old = pColorE->enhanc_level_h;
	if(enhanc_level_h_old!=enhanc_level_h)
	{
		if(pColorE->be_skin_read_data==0)
			beSkin_preserv = false;
		image_color_mapping_build(pColorE->skin_uvf,pColorE->skin_uvf_rediu,enhanc_level,enhanc_level_h,pColorE->skin_v_centr,pColorE->skin_u_centr,pColorE->skin_distmax,COLOR_SKIN_GAMMA,beSkin_preserv,rotat);
		pColorE->enhanc_level = enhanc_level;
		pColorE->enhanc_level_h = enhanc_level_h;
		pColorE->be_skin_preserv = beSkin_preserv;
		pColorE->be_color_rotat = rotat;
	}
	if(rotat!=pColorE->be_color_rotat)
	{
		if(pColorE->be_skin_read_data==0)
			beSkin_preserv = false;
		image_color_mapping_build(pColorE->skin_uvf,pColorE->skin_uvf_rediu,enhanc_level,enhanc_level_h,pColorE->skin_v_centr,pColorE->skin_u_centr,pColorE->skin_distmax,COLOR_SKIN_GAMMA,beSkin_preserv,rotat);
		pColorE->enhanc_level = enhanc_level;
		pColorE->enhanc_level_h = enhanc_level_h;
		pColorE->be_skin_preserv = beSkin_preserv;
		pColorE->be_color_rotat = rotat;	
	
	}

	int size = nW*nH;
	for(int px = 0;px<size; px++)
	{
		U = pU[px];
		V = pV[px];
		pU[px] = uvmap[U][V].u;
		pV[px] = uvmap[U][V].v;

	}

	return 0;
}


void *image_color_enhanc_open_fast(int nW,int nH,unsigned char*pSkinBinData)
{
	void *pSkinEnhanc = image_color_enhanc_proc_init(nW/2,nH/2,10,5,pSkinBinData,true);
	return pSkinEnhanc;
}

void image_color_enhanc_fast(void *pColorEnhancD,unsigned char *pU, unsigned char *pV)
{
	unsigned char *pY = NULL;
	int enhanc_level = 8;
	int enhanc_level_h = 5;
	bool beSkin_preserv = true;
	int rotat = 0;
	image_color_enhanc_proc(pColorEnhancD,pY,pU,pV,enhanc_level,enhanc_level_h,beSkin_preserv,rotat);
}

void image_color_enhanc_exit_fast(void *pColorEnhancD)
{
	image_color_enhanc_proc_exit(pColorEnhancD);
}

