// H323_H261Decoder.h: interface for the H323_H261Decoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_H323_H261DECODER_H__91F633EA_6B0E_4D0A_98C8_6CE4663A1F4F__INCLUDED_)
#define AFX_H323_H261DECODER_H__91F633EA_6B0E_4D0A_98C8_6CE4663A1F4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDecoder.h"

class P64Decoder;

class H323_H261Decoder : public BaseDecoder  
{
public:
	virtual long DeCompress(long dec_opt, void*pParam1,void*pParam2);
	virtual long Initialize(void* pParam1,void* pParam2);
	virtual long End();
	H323_H261Decoder();
	virtual ~H323_H261Decoder();
	
private:
	BOOL Resize(int _width, int _height);
	long PreDecode(LPBYTE lpData,UINT iInLength, LPBYTE lpOut,BOOL bLostPreviousPacket,BOOL bLastPacket,BOOL& bRenderFlag);
	long Decode(LPBYTE lpData,UINT iInLength, LPBYTE lpOut,BOOL bLostPreviousPacket,BOOL& bRenderFlag);
private:
	BYTE*				m_pConvertBuf;
	BYTE*				m_pBitBuf;
	BYTE*				m_pBitPos;
	UINT				m_nBitBufSize;
	unsigned char       m_cPacketSpan;
	long				m_nCovBufSize;
	int					m_colorspace;
	P64Decoder*			m_pDecoder;
};

#endif // !defined(AFX_H323_H261DECODER_H__91F633EA_6B0E_4D0A_98C8_6CE4663A1F4F__INCLUDED_)
