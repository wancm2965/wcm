// AVUtil.h: interface for the AVUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVUTIL_H__
#define __AVUTIL_H__

HPBC_AFX_EXT_CLASS unsigned long AVGetTimestamp(void);
HPBC_AFX_EXT_CLASS unsigned short AVGetLastSequence(unsigned short usSequence1,unsigned short usSequence2);
HPBC_AFX_EXT_CLASS unsigned long AVGenerateSSRC(void);
HPBC_AFX_EXT_CLASS void AVSleep(unsigned long ulMS);


#endif 