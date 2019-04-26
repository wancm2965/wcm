// XResampleSTF.h: interface for the XResampleSTF class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XRESAMPLESTF_H__
#define __XRESAMPLESTF_H__


#include "IXResample.h"

class XResampleSTF 
	: public IXResample  
{
public:
	XResampleSTF(void);
	virtual ~XResampleSTF(void);
public:
	int Connect(int nInSampleRate,int nOutSampleRate);
	void ReleaseConnections(void);
	int Convert(short*pInSamples,int nInSamples,short*pOutSamples,int&nOutSamples);
protected:
	int	m_nInSampleRate;
	int	m_nOutSampleRate;
	int	m_nSamplesPerFrame;

    double		m_factor;	/* Conversion factor = rate_out / rate_in.  */
    bool		m_large_filter;	/* Large filter?			    */
    bool		m_high_quality;	/* Not fast?				    */
    unsigned	m_xoff;		/* History and lookahead size, in samples   */
    short*		m_buffer;	/* Input buffer.			    */
};

#endif 