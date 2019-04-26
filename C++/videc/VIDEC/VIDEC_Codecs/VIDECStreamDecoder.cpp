#include <stdlib.h>
#include <memory.h>
#include "VIDECStreamDecoder.h"
#include "VIDEC_Header.h"
#include "VIDEC_HeaderV1.h"
#include "XUtil.h"

VIDEC_API VIDEC_StreamDecoder*VIDEC_StreamDecoder::Create(void)
{
	NEWRECORD3( VIDECStreamDecoder*,pVIDECStreamDecoder,new VIDECStreamDecoder() )
	NULLReturnRet( pVIDECStreamDecoder ,pVIDECStreamDecoder )  
	if (pVIDECStreamDecoder->Open()!=0)
	{
		pVIDECStreamDecoder->Close();
		delete pVIDECStreamDecoder;
		pVIDECStreamDecoder=NULL;
	}

	return pVIDECStreamDecoder;
}


VIDECStreamDecoder::VIDECStreamDecoder(void)
:m_codecType(VIDEC_CODEC_COUNT)
,m_nVirtualWidth(0)
,m_nVirtualHeight(0)
,m_bDoubleField(false)

,m_bEvenFieldFirst(true)

,m_usLastSeqNoDecode(0)
,m_bGotLastSeqNoDecode(false)


,m_nPacketWidth(0)
,m_nPacketHeight(0)

,m_ulFrameTimestamp(0)

,m_pResize(NULL)
,m_pResizedBuf(NULL)
,m_nResizedWidth(0)
,m_nResizedHeight(0)

,m_pXImage(NULL)
{
	m_bGotKeyFrame=false;
	m_pDecoderFrame=NULL;

	m_bGotEvenKeyFrameDecode=(false);
	m_bGotEvenKeyFrame0=false;
	m_bGotEvenKeyFrame1=false;
	m_bGotEvenKeyFrame2=false;
	m_bGotEvenKeyFrame3=false;

#ifdef _DEMO
	m_ulFirstTimestamp=XGetTimestamp();
#endif
}

VIDECStreamDecoder::~VIDECStreamDecoder(void)
{
}

int VIDECStreamDecoder::Open(void)
{
	return 0;
}

void VIDECStreamDecoder::Close(void)
{
	if (m_pDecoderFrame!=NULL)
	{
		m_pDecoderFrame->Close();
		delete m_pDecoderFrame;
		m_pDecoderFrame=NULL;
	}

	if (m_pResize!=NULL)
	{
		m_pResize->Close();
		delete m_pResize;
		m_pResize=NULL;
	}

	if (m_pResizedBuf!=NULL)
	{
		FREEMSG(m_pResizedBuf);
		m_pResizedBuf=NULL;
	}
}

int VIDECStreamDecoder::Decode(void*pPacketData,int nPacketSize,void**ppYUVData,int&nWidth,int&nHeight)
{
	if (VIDEC_HEADER_IS_VALID(pPacketData))
	{
		unsigned char*pStreamData=NULL;
		int nStreamLen=0;

		int nHeaderLen=VIDEC_HEADER_GET_LEN(pPacketData);
		pStreamData=(unsigned char*)pPacketData+nHeaderLen;
		nStreamLen=nPacketSize-nHeaderLen;

		VIDEC_CODEC_TYPE codecType=(VIDEC_CODEC_TYPE)VIDEC_HEADER_GET_CODEC_ID(pPacketData);

		if (codecType>=VIDEC_CODEC_COUNT)
		{
			return -1;
		}

		unsigned short usCurSeqDecode=VIDEC_HEADER_GET_SEQUENCE(pPacketData);
		//中途有丢包，可能是播放器做了SEEK操作
		if (m_bGotLastSeqNoDecode)
		{
			m_usLastSeqNoDecode++;
			if (m_usLastSeqNoDecode!=usCurSeqDecode)
			{
				m_bGotLastSeqNoDecode=false;
			}
		}

		if (!m_bGotLastSeqNoDecode)
		{
			bool bKeyFrame=VIDEC_HEADER_GET_KEY_FRAME(pPacketData)!=0;
			bool bSubKeyFrame0=VIDEC_HEADER_GET_SUB_KEYFRAME0(pPacketData)!=0;
			if (bKeyFrame && bSubKeyFrame0)
			{
				m_bGotLastSeqNoDecode=true;

				m_usLastSeqNoDecode=usCurSeqDecode;

				m_bGotEvenKeyFrameDecode=false;

				m_bGotEvenKeyFrame0=false;
				m_bGotEvenKeyFrame1=false;
				m_bGotEvenKeyFrame2=false;
				m_bGotEvenKeyFrame3=false;
			}
			else
			{
				return -1;
			}
		}


		m_ulFrameTimestamp=VIDEC_HEADER_GET_TIMESTAMP(pPacketData);
		m_bEvenFieldFirst=VIDEC_HEADER_GET_EVEN_FIELD_FIRST(pPacketData)!=0;
		m_bDoubleField=VIDEC_HEADER_GET_DOUBLE_FIELD(pPacketData)!=0;
		unsigned int nEvenSize=VIDEC_HEADER_GET_EVEN_SIZE(pPacketData);
		unsigned int nOddSize=VIDEC_HEADER_GET_ODD_SIZE(pPacketData);

		bool bDecoderChanged=false;
		if (VIDEC_HEADER_GET_KEY_FRAME(pPacketData))
		{
			unsigned short usWidth=VIDEC_HEADER_GET_ACTUAL_WIDTH(pPacketData);
			unsigned short usHeight=VIDEC_HEADER_GET_ACTUAL_HEIGHT(pPacketData);
			m_nVirtualWidth=VIDEC_HEADER_GET_VIRTUAL_WIDTH(pPacketData);
			m_nVirtualHeight=VIDEC_HEADER_GET_VIRTUAL_HEIGHT(pPacketData);
			if (m_nVirtualWidth==0)
				m_nVirtualWidth=usWidth;
			if (m_nVirtualHeight==0)
				m_nVirtualWidth=usHeight;

			if (usWidth!=m_nPacketWidth || m_nPacketHeight!=usHeight)
			{
				m_nPacketWidth=usWidth;
				m_nPacketHeight=usHeight;
				bDecoderChanged=true;
			}

			bool bGotEvenKeyFrame=VIDEC_HEADER_GET_EVEN_KEYFRAME(pPacketData)!=0;
			if (!m_bGotEvenKeyFrameDecode)
			{
				m_bGotEvenKeyFrameDecode=VIDEC_HEADER_GET_SUB_KEYFRAME0(pPacketData)!=0;
			}
			if ( bGotEvenKeyFrame && m_bGotEvenKeyFrameDecode)
			{
				m_bGotEvenKeyFrameDecode=true;
				if (VIDEC_HEADER_GET_SUB_KEYFRAME0(pPacketData))
				{
					m_bGotEvenKeyFrame0=true;
				}
				if (VIDEC_HEADER_GET_SUB_KEYFRAME1(pPacketData))
				{
					m_bGotEvenKeyFrame1=true;
				}
				if (VIDEC_HEADER_GET_SUB_KEYFRAME2(pPacketData))
				{
					m_bGotEvenKeyFrame2=true;
				}
				if (VIDEC_HEADER_GET_SUB_KEYFRAME3(pPacketData))
				{
					m_bGotEvenKeyFrame3=true;
				}
			}
		}

		if (!m_bGotLastSeqNoDecode)
		{
			return -1;
		}
		
		if (!m_bGotEvenKeyFrameDecode)
		{
			return -1;
		}

		if (codecType!=m_codecType)
		{
			m_codecType=codecType;
			bDecoderChanged=true;
		}

		if (bDecoderChanged ||
			m_pDecoderFrame==NULL)
		{
			if (m_pDecoderFrame!=NULL)
			{
				m_pDecoderFrame->Close();
				delete m_pDecoderFrame;
				m_pDecoderFrame=NULL;
			}
			m_pDecoderFrame=VIDEC_DecoderCreate(*this,m_codecType,m_nPacketWidth,m_nPacketHeight);
		}

		int nRet=0;
		if (m_bDoubleField)
		{
			if (m_bEvenFieldFirst)
			{
				if (nEvenSize && m_bGotEvenKeyFrameDecode)
				{
					if (m_pDecoderFrame!=NULL)
					{
						nRet|=m_pDecoderFrame->DecodeFramePacket(pStreamData,nEvenSize);
					}
				}
			}
			else
			{
				if (nEvenSize && m_bGotEvenKeyFrameDecode)
				{
					if (m_pDecoderFrame!=NULL)
					{
						nRet|=m_pDecoderFrame->DecodeFramePacket(pStreamData,nEvenSize);
					}
				}
			}

			return nRet;
		}
		else
		{
			if (m_pDecoderFrame!=NULL)
			{
				nRet|=m_pDecoderFrame->DecodeFramePacket(pStreamData,nStreamLen);
			}
		}

		if (m_bGotEvenKeyFrame0 &&
			m_bGotEvenKeyFrame1 &&
			m_bGotEvenKeyFrame2 &&
			m_bGotEvenKeyFrame3)
		{
		}
		else
		{
			return -1;
		}

		if (m_pXImage!=NULL)
		{
			unsigned char*pFrameData=(unsigned char*)m_pXImage->GetData();
			if (nRet==0 && m_nPacketWidth>0 && m_nPacketHeight>0)
			{
				if (m_nVirtualWidth!=m_nPacketWidth || m_nVirtualHeight!=m_nPacketHeight)
				{
					if (m_pResize==NULL)
					{
						m_pResize=VIDEC_Resize::Create();
						if (m_pResize->Open()!=0)
						{
							m_pResize->Close();
							delete m_pResize;
							m_pResize=NULL;
						}
					}

					if (m_pResize!=NULL)
					{
						if (m_nResizedWidth!=m_nVirtualWidth ||
							m_nResizedHeight!=m_nVirtualHeight ||
							m_pResizedBuf==NULL)
						{
							if (m_pResizedBuf!=NULL)
							{
								FREEMSG(m_pResizedBuf);
								m_pResizedBuf=NULL;
							}

							m_nResizedWidth=m_nVirtualWidth;
							m_nResizedHeight=m_nVirtualHeight;
							m_pResizedBuf=(unsigned char*)MALLOCMSG(m_nResizedWidth*(m_nResizedHeight<<1));
						}

						if (m_pResizedBuf!=NULL)
						{
							unsigned int nSrcYBufferSize = m_nPacketWidth * m_nPacketHeight;
							unsigned int nSrcUVBufferSize = nSrcYBufferSize >> 2;

							unsigned char*src[3];
							src[0]= (unsigned char*)pFrameData;
							src[1] = src[0] + nSrcYBufferSize;
							src[2] = src[1] + nSrcUVBufferSize;

							unsigned int nDstYBufferSize = m_nResizedWidth*m_nResizedHeight;
							unsigned int nDstUVBufferSize = nDstYBufferSize >> 2;

							unsigned char*dst[3];
							dst[0] = m_pResizedBuf;
							dst[1] = dst[0] + nDstYBufferSize;
							dst[2] = dst[1] + nDstUVBufferSize;

							if (0==m_pResize->Resize(src,m_nPacketWidth,m_nPacketHeight,dst,m_nResizedWidth,m_nResizedHeight))
							{
								*ppYUVData=m_pResizedBuf;
								nWidth=m_nResizedWidth;
								nHeight=m_nResizedHeight;
								return 0;
							}
						}
					}
				}

				*ppYUVData=pFrameData;
				nWidth=m_nPacketWidth;
				nHeight=m_nPacketHeight;
				return 0;
			}

			m_pXImage->Release();
			m_pXImage=NULL;
		}
	}
	else if (VIDEC_HEADER_EXT_IS_VALID(pPacketData))
	{
		unsigned char*pStreamData=NULL;
		int nStreamLen=0;

		int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pPacketData);
		pStreamData=(unsigned char*)pPacketData+nHeaderLen;
		nStreamLen=nPacketSize-nHeaderLen;

		VIDEC_CODEC_TYPE codecType=(VIDEC_CODEC_TYPE)VIDEC_HEADER_EXT_GET_CODEC_ID(pPacketData);

		if (codecType>=VIDEC_CODEC_COUNT)
		{
			return -1;
		}

		unsigned short usCurSeqDecode=VIDEC_HEADER_EXT_GET_SEQUENCE(pPacketData);
		//中途有丢包，可能是播放器做了SEEK操作
		if (m_bGotLastSeqNoDecode)
		{
			m_usLastSeqNoDecode++;
			if (m_usLastSeqNoDecode!=usCurSeqDecode)
			{
				if (m_usLastSeqNoDecode==(unsigned short)(usCurSeqDecode-1))
				{
					m_usLastSeqNoDecode=usCurSeqDecode;
				}
				else
				{
					m_bGotLastSeqNoDecode=false;
				}
			}
		}

		if (!m_bGotLastSeqNoDecode)
		{
			bool bKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData)!=0;
			if (bKeyFrame)
			{
				m_bGotLastSeqNoDecode=true;
				m_usLastSeqNoDecode=usCurSeqDecode;
			}
			else
			{
				return -1;
			}
		}


		m_ulFrameTimestamp=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData);
		m_bEvenFieldFirst=VIDEC_HEADER_EXT_GET_EVEN_FIELD_FIRST(pPacketData)!=0;
		m_bDoubleField=VIDEC_HEADER_EXT_GET_DOUBLE_FIELD(pPacketData)!=0;

		bool bDecoderChanged=false;
		if (VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData))
		{
			unsigned short usWidth=VIDEC_HEADER_EXT_GET_ACTUAL_WIDTH(pPacketData);
			unsigned short usHeight=VIDEC_HEADER_EXT_GET_ACTUAL_HEIGHT(pPacketData);
			m_nVirtualWidth=VIDEC_HEADER_EXT_GET_VIRTUAL_WIDTH(pPacketData);
			m_nVirtualHeight=VIDEC_HEADER_EXT_GET_VIRTUAL_HEIGHT(pPacketData);
			if (m_nVirtualWidth==0)
				m_nVirtualWidth=usWidth;
			if (m_nVirtualHeight==0)
				m_nVirtualWidth=usHeight;

			if (usWidth!=m_nPacketWidth || m_nPacketHeight!=usHeight)
			{
				m_nPacketWidth=usWidth;
				m_nPacketHeight=usHeight;
				bDecoderChanged=true;
			}
		}

		if (!m_bGotLastSeqNoDecode)
		{
			return -1;
		}
		
		if (codecType!=m_codecType)
		{
			m_codecType=codecType;
			bDecoderChanged=true;
		}

		if (bDecoderChanged ||
			m_pDecoderFrame==NULL)
		{
			if (m_pDecoderFrame!=NULL)
			{
				m_pDecoderFrame->Close();
				delete m_pDecoderFrame;
				m_pDecoderFrame=NULL;
			}
			m_pDecoderFrame=VIDEC_DecoderCreate(*this,m_codecType,m_nPacketWidth,m_nPacketHeight);
		}

		int nRet=0;
		if (m_pDecoderFrame!=NULL)
		{
			nRet|=m_pDecoderFrame->DecodeFramePacket(pStreamData,nStreamLen);
		}

		if (m_pXImage!=NULL)
		{
			unsigned char*pFrameData=(unsigned char*)m_pXImage->GetData();
			if (nRet==0 && m_nPacketWidth>0 && m_nPacketHeight>0)
			{
				int nActualWidth=m_nPacketWidth;
				int nAcutualHeight=m_nPacketHeight;
				if (m_bDoubleField)
				{
					nAcutualHeight>>=1;
				}

				if (m_nVirtualWidth!=m_nPacketWidth || m_nVirtualHeight!=nAcutualHeight)
				{
					if (m_pResize==NULL)
					{
						m_pResize=VIDEC_Resize::Create();
						if (m_pResize->Open()!=0)
						{
							m_pResize->Close();
							delete m_pResize;
							m_pResize=NULL;
						}
					}

					if (m_pResize!=NULL)
					{
						if (m_nResizedWidth!=m_nVirtualWidth ||
							m_nResizedHeight!=m_nVirtualHeight ||
							m_pResizedBuf==NULL)
						{
							if (m_pResizedBuf!=NULL)
							{
								FREEMSG(m_pResizedBuf);
								m_pResizedBuf=NULL;
							}

							m_nResizedWidth=m_nVirtualWidth;
							m_nResizedHeight=m_nVirtualHeight;
							m_pResizedBuf=(unsigned char*)MALLOCMSG(m_nResizedWidth*(m_nResizedHeight<<1));
						}

						if (m_pResizedBuf!=NULL)
						{
							unsigned int nSrcYBufferSize = m_nPacketWidth * m_nPacketHeight;
							unsigned int nSrcUVBufferSize = nSrcYBufferSize >> 2;

							unsigned char*src[3];
							src[0]= (unsigned char*)pFrameData;
							src[1] = src[0] + nSrcYBufferSize;
							src[2] = src[1] + nSrcUVBufferSize;

							unsigned int nDstYBufferSize = m_nResizedWidth*m_nResizedHeight;
							unsigned int nDstUVBufferSize = nDstYBufferSize >> 2;

							unsigned char*dst[3];
							dst[0] = m_pResizedBuf;
							dst[1] = dst[0] + nDstYBufferSize;
							dst[2] = dst[1] + nDstUVBufferSize;

							if (0==m_pResize->Resize(src,m_nPacketWidth,nAcutualHeight,dst,m_nResizedWidth,m_nResizedHeight))
							{
								*ppYUVData=m_pResizedBuf;
								nWidth=m_nResizedWidth;
								nHeight=m_nResizedHeight;
								return 0;
							}
						}
					}
				}

				*ppYUVData=pFrameData;
				nWidth=m_nPacketWidth;
				nHeight=m_nPacketHeight;
				return 0;
			}

			m_pXImage->Release();
			m_pXImage=NULL;
		}
	}

	return -1;
}

void VIDECStreamDecoder::OnVIDEC_DecoderCallbackFrame(VIDEC_Decoder*pDecoderFrame,VIDEC_Image*pImage)
{
	if (m_pXImage!=NULL)
	{
		m_pXImage->Release();
		m_pXImage=NULL;
	}

	m_pXImage=dynamic_cast<XImage*>(pImage);
	if (m_pXImage!=NULL)
	{
		m_pXImage->AddRef();
	}
}
