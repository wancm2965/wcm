
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
void *satra_enhanc_proc_init(int nW, int nH, int enhanc_levels=16);


/* satra_enhanc_proc_exit releases the struct of current process.
*  It needs to be released after enhancement.
*
*  Parameters:
*			pSatra: struct allocated by satra_enhanc_proc_init
*
*  Return value:
*			return no value
*/
void satra_enhanc_proc_exit(void *pSatra);

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
int imag_satra_enhanc_proc(void *pSatra,unsigned char *pY,unsigned char *pU, unsigned char *pV,int enhanc_level);
