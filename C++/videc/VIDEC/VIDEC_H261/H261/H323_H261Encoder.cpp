// H323_H261Encoder.cpp: implementation of the H323_H261Encoder class.
//
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"

#include "..\colorspace.h"
#include ".\..\utils\font.h"

#include "H323_H261Encoder.h"

#ifdef INTRA_MODE
#include "p64encoder.h"
#else
#include "p64fullencoder.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

H323_H261Encoder::H323_H261Encoder():m_pEncoder(NULL),m_pRateControl(NULL),m_bFirstPacket(true),
									 m_bEncoding(false),m_bSyncPacket(false),m_pConvertBuf(NULL)
{
	m_nMode=UENC_OPT_PACKET;
	End();
}

H323_H261Encoder::~H323_H261Encoder()
{
	End();
}


long H323_H261Encoder::Initialize(void *pParam1, void *pParam2)
{
	int iResult=UENC_OK;
	if (pParam1==NULL||pParam2==NULL) 
	{
		iResult=UENC_BAD_PARAM;
		return iResult;
	}
	//!release 
	End();
	
	UBitmapInfoHeader* pin=(UBitmapInfoHeader*)pParam1;
	UENCSETTINGS* pSettings=(UENCSETTINGS*)pParam2;
	
	if((pin->biWidth==352 && pin->biHeight==288 )||
		(pin->biWidth==176 && pin->biHeight==144)) 
	{
		m_BitmapHeader.biWidth=pin->biWidth;
		m_BitmapHeader.biHeight=pin->biHeight;
		
		m_iFrameRate=pSettings->frame_rate;
		m_bandwidth=pSettings->bitrate;
		if (pSettings->max_key_interval>0) 
		{
			m_iIFrameInterval=pSettings->max_key_interval;
		}
		else
			m_iIFrameInterval=20;
		
	}
	else
	{
		iResult=UENC_BAD_FORMAT;
		return iResult;
	}
	
	if (pin->biCompression==0 && pin->biBitCount==24) 
	{
		m_colorspace=ENC_CSP_RGB24;
	}
	else if (pin->biCompression==MAKEFOURCC('I','4','2','0')) 
	{
		
		m_colorspace=ENC_CSP_I420;
	
	}
	else
	{
		iResult=UENC_BAD_FORMAT;
		return iResult;
	}
	if (m_colorspace!=ENC_CSP_I420) 
	{
		m_pConvertBuf=new BYTE[m_BitmapHeader.biWidth*m_BitmapHeader.biHeight*pin->biBitCount/8];
	}

	m_pRateControl=new URateControl();
	m_pRateControl->RateControlInit(m_BitmapHeader.biWidth,m_BitmapHeader.biHeight,m_bandwidth,\
									m_iFrameRate,m_iIFrameInterval);
	int videoQuality=9, fillLevel=2;
#ifdef INTRA_MODE
	m_pEncoder=new P64Encoder(videoQuality,fillLevel);
#else
	m_pEncoder=new P64FullEncoder(videoQuality,fillLevel);
#endif
	m_pEncoder->SetSize(m_BitmapHeader.biWidth,m_BitmapHeader.biHeight);
	memcpy(&m_BitmapHeader,pin,sizeof(UBitmapInfoHeader));

	return iResult;

}

long H323_H261Encoder::PreProcessOneFrame(LPBYTE lpData, LPBYTE lpOut,UINT& len,BOOL& isKey,int& iQuant)
{
	int iResult=UENC_OK;
	
	if (m_bEncoding) 
	{
		iResult=UENC_PRE_NO_GET;
		return iResult;
	}
	else
		m_bEncoding=true;

	memset(&m_EncResult,0,sizeof(URC_DATA));
	
	if (!m_pRateControl->CanEncThisFrame(0)) 
	{
		m_EncResult.is_key_frame=-1;
		m_pRateControl->RateControlUpdate(0,m_EncResult);	
		m_bEncoding=false;
		iResult=UENC_RC_OVERFLOW;
	#ifndef INTRA_MODE
		m_pEncoder->SkipCurrentFrame();
	#endif
		return iResult;
	}
	
	m_pRateControl->RateControlGetParam(iQuant,isKey);
//	iQuant=2;
//	isKey=TRUE;
	m_EncResult.is_key_frame=isKey;
	m_EncResult.quantizer=iQuant;

	m_pEncoder->SetQualityLevel(iQuant);

	BYTE* pInputImage=lpData;
	if ((m_colorspace!=ENC_CSP_I420) &&(m_pConvertBuf))
	{
		uint8_t * x_ptr=(uint8_t *)lpData;
		int x_stride=m_BitmapHeader.biWidth*3;
		uint8_t * y_dst=m_pConvertBuf;
		uint8_t * u_dst=m_pConvertBuf+m_BitmapHeader.biWidth*m_BitmapHeader.biHeight;
		uint8_t * v_dst=m_pConvertBuf+m_BitmapHeader.biWidth*m_BitmapHeader.biHeight*5/4;
		int y_stride=m_BitmapHeader.biWidth;
		int uv_stride=m_BitmapHeader.biWidth/2;
		int width=m_BitmapHeader.biWidth;
		int height=m_BitmapHeader.biHeight;
		int vflip=1;
		bgr_to_yv12_mmx(
			x_ptr, x_stride, y_dst, u_dst,v_dst,
			y_stride,
			uv_stride,
			width,
			height,
			vflip);
		pInputImage=m_pConvertBuf;
		
	}
#ifdef PRINT_IMAGE_INFO
	IMAGE Img;
	char cInfo[255];
	time_t mytime;
	struct tm when;
    time(&mytime);
	when = *localtime( &mytime );
  	sprintf(cInfo,"%s",asctime( &when ));
	Img.y=pInputImage;
	Img.u=pInputImage+m_BitmapHeader.biWidth*m_BitmapHeader.biHeight;
	Img.v=pInputImage+m_BitmapHeader.biWidth*m_BitmapHeader.biHeight*5/4;
	image_printf_h261(&Img,m_BitmapHeader.biWidth,m_BitmapHeader.biHeight,10,10,cInfo);
#endif
	if(isKey)//FastUpdate ¸ÃÖ¡±àÂëÎª¹Ø¼üÖ¡	
	{
		m_pEncoder->FastUpdatePicture();
	}
	memcpy(m_pEncoder->GetFramePtr(),pInputImage,m_BitmapHeader.biWidth*m_BitmapHeader.biHeight*3/2);

	m_pEncoder->PreProcessOneFrame();

	return iResult;
}

long H323_H261Encoder::EncodeAndGetPacket(LPBYTE lpOut,UINT& len,BOOL& bLastPacket)
{
	int iResult=UENC_OK;

	if (!m_bEncoding||!m_pEncoder->MoreToIncEncode())
	{
		iResult=UENC_PRE_NO_GET;
		return iResult; 
	}
	if(m_bFirstPacket)
	{
		len=4;
		unsigned int nH261Header=0;
		*((unsigned int*)(lpOut))=nH261Header;
		bLastPacket=TRUE;
		m_bSyncPacket=true;
		m_bFirstPacket=false;
		m_bEncoding=false;
		return iResult;
	}
	m_pEncoder->IncEncodeAndGetPacket(lpOut,len);

	m_EncResult.total_bits+=len*8;
	if (0!=m_pEncoder->MoreToIncEncode()) 
	{
		bLastPacket=FALSE;
	}
	else
	{
		bLastPacket=TRUE;
		m_pRateControl->RateControlUpdate(0,m_EncResult);
		
		//Next packet is sync point
		int iQuant=10;
		BOOL isKey=FALSE;
		m_pRateControl->RateControlGetParam(iQuant,isKey);
		if(isKey)
			m_bSyncPacket=true;
		
		m_bEncoding=false;
	}
			
	return iResult;


}

long H323_H261Encoder::Compress(long enc_opt, void *pParam1, void *pParam2)
{
	int iResult=UENC_OK;

	if (m_pEncoder==NULL||pParam1==NULL||pParam2==NULL) 
	{
		iResult=UENC_BAD_PARAM;
		return iResult;
	}

	UENC_FRAME * pFrame=(UENC_FRAME *)pParam1;
	UENC_RESULT * pResult=(UENC_RESULT*)pParam2;
	pResult->cType='P';
	if (enc_opt==UENC_OPT_PACKET_PRE) 
	{
		BOOL bIsKey=FALSE; 
		iResult=PreProcessOneFrame((LPBYTE)pFrame->image,(LPBYTE)pFrame->bitstream, \
									(UINT&)pFrame->length,bIsKey,pResult->iQuant);
	}
	else if (enc_opt==UENC_OPT_PACKET_GET) 
	{
		iResult=EncodeAndGetPacket((LPBYTE)pFrame->bitstream,(UINT&)pFrame->length,pResult->iLastPacket);
		if(m_bSyncPacket)
		{
			pResult->cType='I';
			m_bSyncPacket=false;
		}
	}
	else
	{	iResult=UENC_BAD_PARAM;
		return iResult;
	}
	pResult->iQuant=m_EncResult.quantizer;
	
	return iResult;
}

long H323_H261Encoder::End()
{
	
	if (m_pEncoder) 
	{
		delete m_pEncoder;
		m_pEncoder=NULL;
	}
	if (m_pRateControl) 
	{
		delete m_pRateControl;
		m_pRateControl=NULL;
	}
	if (m_pConvertBuf)
	{
		delete[] m_pConvertBuf;
		m_pConvertBuf=NULL;
	}
	m_bFirstPacket=true;
	m_bSyncPacket=false;
	memset(&m_BitmapHeader,0,sizeof(UBitmapInfoHeader));
	return UENC_OK;
}

