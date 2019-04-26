// XCodecInst.h: interface for the XCodecInst class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XCODECINST_H__
#define __XCODECINST_H__

#include "../../include/HPATC_Codec.h"

int GetCodecCount(void);
bool GetCodecInstByIndex(int nIndex,HPATC_CodecInst*pCodecInst);
bool GetCodecInstByID(int nID,HPATC_CodecInst*pCodecInst);


#endif