#ifndef _BME_MOTION_EST_H
#define _BME_MOTION_EST_H

extern int FastBME(MBParam * const pParam,
		  FRAMEINFO * const current,
		  FRAMEINFO * const reference,
		  int rx,int ry,int cx,int cy,
		  int& VAR,int& VAROR,int& MWOR,
		  int& MX,int& MY,int& MV,int& OMV);
extern int BMEMotionEstimation(MBParam * const pParam,
		  FRAMEINFO * const current,
		  FRAMEINFO * const reference);

#endif