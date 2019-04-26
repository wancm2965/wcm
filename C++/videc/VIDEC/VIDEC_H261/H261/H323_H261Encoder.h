// H323_H261Encoder.h: interface for the H323_H261Encoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_H323_H261ENCODER_H__0DEE1CAE_F4D7_4EDF_95A8_846033C5618C__INCLUDED_)
#define AFX_H323_H261ENCODER_H__0DEE1CAE_F4D7_4EDF_95A8_846033C5618C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseEncoder.h"
//#define  INTRA_MODE
#include "URateControl.h"
class P64FullEncoder;
class P64Encoder;


class H323_H261Encoder : public BaseEncoder  
{
public:
	virtual long Initialize(void* pParam1,void* pParam2);
	virtual long Compress(long enc_opt, void*pParam1,void*pParam2);
	virtual long End();
	H323_H261Encoder();
	virtual ~H323_H261Encoder();

private:
	
	virtual long PreProcessOneFrame(LPBYTE lpData, LPBYTE lpOut,UINT& len,BOOL& isKey,int& iQuant);
	
	virtual long EncodeAndGetPacket(LPBYTE lpOut,UINT& len,BOOL & bLastPacket);
	
	
private:
	bool				m_bFirstPacket;
	bool				m_bSyncPacket;
	bool				m_bEncoding;
	int					m_iFrameRate;
	int					m_iIFrameInterval;
	int					m_bandwidth;
	BYTE*				m_pConvertBuf;
	int					m_colorspace;
	URC_DATA			m_EncResult;
	URateControl*		m_pRateControl;
#ifdef INTRA_MODE
	P64Encoder*			m_pEncoder;	
#else
	P64FullEncoder*		m_pEncoder;
#endif

};

#endif // !defined(AFX_H323_H261ENCODER_H__0DEE1CAE_F4D7_4EDF_95A8_846033C5618C__INCLUDED_)
