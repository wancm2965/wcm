/*********************************************************************
* This file defines the functions of color enhancement with enhancement 
* level. This module is called as AVCON CEE 
*
* Author:	Li Renjie
*			2011/11/11
* Revised:  
* Email:	lrjie2000@gmail.com
* 
* Description: 
*
**************************************************************************/


/* image_color_enhanc_open_fast initializes the struct of current process.
*  It needs to be released after enhancement.
*
*  Parameters:
*			nW: width of current image
*			nH: height of current image
*			strSkinDataFile: training result file of skin data
*
*  Return value:
*			struct pointer: successful
*		   -1: error 
*		   -2: no input
*  Description:
*	It initialises the UV mapping tables with skin preserving
*/
void *image_color_enhanc_open_fast(int nW,int nH,unsigned char*pSkinBinData);

/* image_color_enhanc_fast enhances the color of current image with format YUV420.
*  The enhancement level number is 10 and hue level 5. 
*
*  Parameters:
*			pColorEnhancD: struct allocated by image_color_enhanc_open_fast
*			pU: U channel
*			pV: V channel
*  Return value:
*			return no value
*/
void image_color_enhanc_fast(void *pColorEnhancD,unsigned char *pU, unsigned char *pV);

/* image_color_enhanc_exit_fast releases the struct of current process.
*  It needs to be released after enhancement.
*
*  Parameters:
*			pColorEnhancD: struct allocated by image_color_enhanc_open_fast
*
*  Return value:
*			return no value
*/
void image_color_enhanc_exit_fast(void *pColorEnhancD);