#include "stdafx.h"
#include "XAH400.h"
#include "XUtil.h"
#include "XAutoLock.h"
#include "VIDEC_Header.h"
#include <AUDEC/AUDEC_Header.h>
#include <AUDEC/AUDEC_CodecID.h>
#include "VIDEC_CodecDef.h"
#include "../VIDEC_API/XImage.h"
#include "VIDEC_Convert.h"
#include "AVCONProduct.h"
#include "../VIDEC_csp/fastmemcpy.h"

#include <ATLBASE.H>

#ifdef SUPPORT_AH400

#pragma comment(lib, "../VIDEC_AH400/DS40xxSDK.lib")

#define AH400_DEV_MAX_COUNT		128
static XAH400*g_paXAH400[AH400_DEV_MAX_COUNT];
static XCritSec	g_csArrayXAH400;

void ImageStreamCallback(UINT channelNumber,void *Context);
int __cdecl StreamDirectReadCallback(ULONG channelNumber,void *DataBuf,DWORD Length,int FrameType,void *context);

extern DWORD GetUsedByProductType(void);

XAH400::XAH400(VIDEC_AH400Callback&rCallback)
:m_rCallback(rCallback)

,m_pImageBuffer(NULL)
,m_nDevID(0)
,m_nImageSize(0)
,m_pChannelHandle((HANDLE)0xFFFFFFFF)


,m_bNTSC(false)

,m_pRectResult(NULL)
,m_nRectCount(0)


,m_nFrameRate(15)
,m_bVideoStreamStarted(false)

,m_bSubVideoStreamStarted(false)

,m_pStreamBuf(NULL)
,m_usSeqNo(0)
,m_usSubSeqNo(0)

, m_nSkipCount(0)
, m_nSkipIndex(0)

,m_usAudioSeqNo(0)
,m_ulLastFrameTimestamp(0)
,m_ulLastPacketTimestamp(0)

,m_bEnableOSD(false)
{
}

XAH400::~XAH400(void)
{
}

int  XAH400::Open(int nDevID,int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate)
{
	m_nDevID=nDevID;
	if (m_nDevID>=AH400_DEV_MAX_COUNT)
	{
		return -1;
	}

	{
		XAutoLock l(g_csArrayXAH400);
		if (g_paXAH400[m_nDevID])
			return -1;
		m_pChannelHandle=::ChannelOpen(m_nDevID);
		if (m_pChannelHandle==(HANDLE)0xFFFFFFFF)
			return -1;

		g_paXAH400[m_nDevID]=this;
	}


	VideoStandard_t VideoStandard;
	int Brightness;
	int Contrast;
	int Saturation;
	int Hue;


	GetVideoPara(m_pChannelHandle, &VideoStandard, &Brightness, &Contrast, &Saturation, &Hue);
	m_bNTSC=(VideoStandard==StandardNTSC);


	m_pStreamBuf=(unsigned char*)MALLOCMSG(704*576*3/2);
	if (m_pStreamBuf==NULL)
	{
		return -1;
	}

	m_pImageBuffer=(unsigned char*)MALLOCMSG(704*576*3/2);
	if (m_pImageBuffer==NULL)
	{
		return -1;
	}

	if (nWidth<320 || nHeight<240)
	{
		m_nWidth=176;
		m_nHeight=(m_bNTSC?120:144);
	}
	else if (nWidth<640 || nHeight<480)
	{
		m_nWidth=352;
		m_nHeight=(m_bNTSC?240:288);
	}
	else
	{
		m_nWidth=704;
		m_nHeight=(m_bNTSC?480:576);
	}

	if (nFrameRate<=1)
	{
		m_nSkipCount=25;
		nFrameRate=1;
	}
	else if (nFrameRate<=5)
	{
		m_nSkipCount=5;
		nFrameRate=5;
	}
	else if (nFrameRate<=10)
	{
		m_nSkipCount=3;
		nFrameRate=10;
	}
	else if (nFrameRate<=15)
	{
		m_nSkipCount=2;
		nFrameRate=15;
	}
	else
	{
		m_nSkipCount=1;
		nFrameRate=30;
	}

	m_nFrameRate=nFrameRate;
	nFrameRate=30;

	::SetupSubChannel(m_pChannelHandle,0);

	m_nImageSize=(int)m_nWidth*(int)m_nHeight*3/2;
	::SetImageStream(m_pChannelHandle,1,nFrameRate,m_nWidth,m_nHeight,m_pImageBuffer);

	return 0;
}

void XAH400::Close(void)
{
	if (m_pChannelHandle!=(HANDLE)0xFFFFFFFF)
	{
		StopMotionDetect();

		if (m_bVideoStreamStarted)
		{
			::StopSubVideoCapture(m_pChannelHandle);
			::StopVideoCapture(m_pChannelHandle);
		}

		::SetImageStream(m_pChannelHandle,0,0,0,0,0);
		::ChannelClose(m_pChannelHandle);
		m_pChannelHandle=(HANDLE)0xFFFFFFFF;

		{
			XAutoLock l(g_csArrayXAH400);
			g_paXAH400[m_nDevID]=NULL;
		}
	}

	if (m_pImageBuffer)
	{
		FREEMSG(m_pImageBuffer);
		m_pImageBuffer=NULL;
	}

	if (m_pStreamBuf)
	{
		FREEMSG(m_pStreamBuf);
		m_pStreamBuf=NULL;
	}

	if (m_pRectResult!=NULL)
	{
		FREEMSG(m_pRectResult);
		m_pRectResult=NULL;
	}
}


int XAH400::StartVideoStream(int nBitrate,int nQuant,int qmin,int qmax,int nKeyFrameInterval,bool bEnableSub,int nSubScaleType,int nSubBitrate,int nSubQuant,int nSubQMin,int nSubQMax)
{
	if (!m_bVideoStreamStarted)
	{
		int n4CIFCount=0;
		int nTotalChannels=GetTotalChannels();
		int nTotalDSPs=GetTotalDSPs();
		bool b4ChannelsIn1DSP=true;//(nTotalChannels>=nTotalDSPs*4);
		//{{xmc for demo
		//bEnableSub=false;
		//nQuant-=2;
		//if (0)
		//}}
		{
			XAutoLock l(g_csArrayXAH400);

			if (b4ChannelsIn1DSP)
			{
				if (m_nDevID<4)
				{
					for (int i=0;i<4;i++)
					{
						if (g_paXAH400[i] && m_nDevID!=i)
						{
							if (g_paXAH400[i]->GetWidth()==704)
							{
								n4CIFCount++;
							}
						}
					}
				}
				else if (m_nDevID<8)
				{
					for (int i=4;i<8;i++)
					{
						if (g_paXAH400[i] && m_nDevID!=i)
						{
							if (g_paXAH400[i]->GetWidth()==704)
							{
								n4CIFCount++;
							}
						}
					}
				}
				else if (m_nDevID<12)
				{
					for (int i=8;i<12;i++)
					{
						if (g_paXAH400[i] && m_nDevID!=i)
						{
							if (g_paXAH400[i]->GetWidth()==704)
							{
								n4CIFCount++;
							}
						}
					}
				}
				else //if (m_nDevID<12)
				{
					for (int i=12;i<16;i++)
					{
						if (g_paXAH400[i] && m_nDevID!=i)
						{
							if (g_paXAH400[i]->GetWidth()==704)
							{
								n4CIFCount++;
							}
						}
					}
				}
			}
		}

		PictureFormat_t encoderType;
		int nWidth=m_nWidth;
		int nHeight=m_nHeight;
		switch (m_nWidth)
		{
		case 176:
			encoderType=ENC_QCIF_FORMAT;
			break;
		case 352:
			encoderType=ENC_CIF_FORMAT;
			break;
		case 704:
			if (b4ChannelsIn1DSP)
			{
				if (n4CIFCount<2)
				{
					encoderType=ENC_4CIF_FORMAT;
				}
				else
				{
					encoderType=ENC_CIF_FORMAT;
					nWidth>=1;
					nHeight>=1;
				}
			}
			else
			{
				encoderType=ENC_4CIF_FORMAT;
			}
			break;
		default:
			encoderType=ENC_CIF_FORMAT;
			break;
		}
		m_nWidth=nWidth;
		m_nHeight=nHeight;

		{
			nKeyFrameInterval=m_nFrameRate*4;
		}

		if (nQuant<1 || nQuant>31)
			nQuant=16;

		::SetupSubChannel(m_pChannelHandle,0);
		::SetEncoderPictureFormat(m_pChannelHandle,encoderType);
		::SetIBPMode(m_pChannelHandle, nKeyFrameInterval, 2, 1, m_nFrameRate);
		::SetDefaultQuant(m_pChannelHandle, nQuant, nQuant, nQuant+5);
		if (encoderType==ENC_4CIF_FORMAT)
		{
			::SetDeInterlace(m_pChannelHandle,1,5);
		}
		else
		{
			::SetDeInterlace(m_pChannelHandle,0,0);
		}

		if (nBitrate)
		{
			::SetBitrateControlMode(m_pChannelHandle,brCBR);
			::SetupBitrateControl(m_pChannelHandle,nBitrate*1000);
		}
		else
		{
			::SetBitrateControlMode(m_pChannelHandle,brVBR);
			::SetupBitrateControl(m_pChannelHandle,10240000);
		}

		::SetStreamType(m_pChannelHandle, STREAM_TYPE_AVSYNC);
		::StartVideoCapture(m_pChannelHandle);
		m_bVideoStreamStarted=true;

		if (bEnableSub)
		{
			m_nSubWidth=nWidth;
			m_nSubHeight=nHeight;
			switch (encoderType)
			{
			case ENC_CIF_FORMAT:
				{
					if (nSubScaleType>=1)
					{
						encoderType=ENC_QCIF_FORMAT;
						m_nSubWidth>>=1;
						m_nSubHeight>>=1;
					}
				}
				break;
			case ENC_4CIF_FORMAT:
				{
					if (nSubScaleType>=2 || n4CIFCount>=1)
					{
						encoderType=ENC_QCIF_FORMAT;
						m_nSubWidth>>=2;
						m_nSubHeight>>=2;
					}
					else// if (nSubScaleType>=1)
					{
						encoderType=ENC_CIF_FORMAT;
						m_nSubWidth>>=1;
						m_nSubHeight>>=1;
					}
				}
				break;
			}

			if (nSubQuant<1 || nSubQuant>31)
				nSubQuant=16;

			::SetupSubChannel(m_pChannelHandle,1);
			::SetSubEncoderPictureFormat(m_pChannelHandle,encoderType);
			::SetIBPMode(m_pChannelHandle, nKeyFrameInterval, 2, 1, m_nFrameRate);
			::SetDefaultQuant(m_pChannelHandle, nSubQuant, nSubQuant, nSubQuant+5);

			::SetBitrateControlMode(m_pChannelHandle,brVBR);
			::SetupBitrateControl(m_pChannelHandle,10240000);

			::StartSubVideoCapture(m_pChannelHandle);
		}

	}
	return 0;
}

int XAH400::StopVideoStream(void)
{
	if (m_bVideoStreamStarted)
	{
		m_bVideoStreamStarted=false;
		if (m_bSubVideoStreamStarted)
		{
			m_bSubVideoStreamStarted=false;
			::StopSubVideoCapture(m_pChannelHandle);
		}
		::StopVideoCapture(m_pChannelHandle);
	}
	return 0;
}

int XAH400::RequestKeyFrame(void)
{
	if (m_bVideoStreamStarted)
	{
		::SetupSubChannel(m_pChannelHandle,0);
		::CaptureIFrame(m_pChannelHandle);

		return 0;
	}

	return -1;
}

int XAH400::RequestSubKeyFrame(void)
{
	if (m_bVideoStreamStarted)
	{
		::SetupSubChannel(m_pChannelHandle,1);
		::CaptureIFrame(m_pChannelHandle);

		return 0;
	}

	return -1;
}

HANDLE XAH400::GetChannelHandle(void)
{
	return m_pChannelHandle;
}

void XAH400::OnImageStreamCallback(void)
{
	if ((m_nSkipIndex++)%m_nSkipCount)
	{
		return ;
	}

	{
		unsigned char*pImageData=(unsigned char*)MALLOCMSG(m_nImageSize);
		if (pImageData!=NULL)
		{
			int nImageSize=m_nImageSize;
			int nWidth=m_nWidth;
			bool bDoubleField=false;
			fastmemcpy(pImageData,m_pImageBuffer,m_nImageSize);

			XImage*pXImage;
			NEWRECORD( pXImage , new XImage(pImageData,nImageSize) );
NULLReturn( pXImage  )  

			pXImage->AddRef();

			pXImage->SetActualWidth(nWidth);
			pXImage->SetActualHeight(m_nHeight);
			pXImage->SetVirtualWidth(m_nWidth);
			pXImage->SetVirtualHeight(m_nHeight);
			pXImage->SetDoubleField(bDoubleField);
			pXImage->SetEvenField(bDoubleField);
			pXImage->SetOddField(bDoubleField);
			pXImage->SetTimestamp(XGetTimestamp());
			pXImage->SetFrameRate(m_nFrameRate);

			m_rCallback.OnVIDEC_AH400CallbackImage(pXImage);

			pXImage->Release();
			pXImage=NULL;
		}
	}
}


void XAH400::OnVideoFrameMain(unsigned char*pData,unsigned long ulLen,bool bKeyFrame)
{
	if (m_pStreamBuf!=NULL)
	{
		unsigned char*pPacketData=m_pStreamBuf;


		VIDEC_HEADER_EXT_RESET(pPacketData);
		VIDEC_HEADER_EXT_SET_CODEC_ID(pPacketData,VIDEC_CODEC_AH400);
		VIDEC_HEADER_EXT_SET_DOUBLE_FIELD(pPacketData,0);
		VIDEC_HEADER_EXT_SET_KEYFRAME(pPacketData,(bKeyFrame?1:0));
		VIDEC_HEADER_EXT_SET_SEQUENCE(pPacketData,m_usSeqNo++);
		VIDEC_HEADER_EXT_SET_TIMESTAMP(pPacketData,XGetTimestamp());
		if (bKeyFrame)
		{
			VIDEC_HEADER_EXT_SET_ACTUAL_WIDTH(pPacketData,m_nWidth);
			VIDEC_HEADER_EXT_SET_ACTUAL_HEIGHT(pPacketData,m_nHeight);
			VIDEC_HEADER_EXT_SET_VIRTUAL_WIDTH(pPacketData,m_nWidth);
			VIDEC_HEADER_EXT_SET_VIRTUAL_HEIGHT(pPacketData,m_nHeight);
		}

		int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pPacketData);
		fastmemcpy(pPacketData+nHeaderLen,pData,ulLen);

		m_rCallback.OnVIDEC_AH400CallbackVideoStreamData(pPacketData,ulLen+nHeaderLen,m_nWidth,m_nHeight,nHeaderLen);
	}
}


void XAH400::OnVideoFrameSub(unsigned char*pData,unsigned long ulLen,bool bKeyFrame)
{
	if (m_pStreamBuf!=NULL)
	{
		unsigned char*pPacketData=m_pStreamBuf;

		VIDEC_HEADER_EXT_RESET(pPacketData);
		VIDEC_HEADER_EXT_SET_CODEC_ID(pPacketData,VIDEC_CODEC_AH400);
		VIDEC_HEADER_EXT_SET_DOUBLE_FIELD(pPacketData,0);
		VIDEC_HEADER_EXT_SET_KEYFRAME(pPacketData,(bKeyFrame?1:0));
		VIDEC_HEADER_EXT_SET_SEQUENCE(pPacketData,m_usSubSeqNo++);
		VIDEC_HEADER_EXT_SET_TIMESTAMP(pPacketData,XGetTimestamp());
		if (bKeyFrame)
		{
			VIDEC_HEADER_EXT_SET_ACTUAL_WIDTH(pPacketData,m_nSubWidth);
			VIDEC_HEADER_EXT_SET_ACTUAL_HEIGHT(pPacketData,m_nSubHeight);
			VIDEC_HEADER_EXT_SET_VIRTUAL_WIDTH(pPacketData,m_nSubWidth);
			VIDEC_HEADER_EXT_SET_VIRTUAL_HEIGHT(pPacketData,m_nSubHeight);
		}

		int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pPacketData);
		fastmemcpy(pPacketData+nHeaderLen,pData,ulLen);

		m_rCallback.OnVIDEC_AH400CallbackVideoSubStreamData(pPacketData,ulLen+nHeaderLen,m_nSubWidth,m_nSubHeight,nHeaderLen);
	}
}

void XAH400::OnAudioFramePacket(unsigned char*pData,unsigned long ulLen)
{
	if (ulLen>548)
	{
		return ;
	}


	unsigned char pAudioPacket[1024];
	unsigned long ulCurTimestamp=XGetTimestamp();
	if (ulCurTimestamp-m_ulLastFrameTimestamp<120)
	{
		unsigned long ulDelta1=ulCurTimestamp-m_ulLastPacketTimestamp;
		unsigned long ulDelta2=m_ulLastPacketTimestamp-ulCurTimestamp;
		unsigned long ulDelta=(ulDelta1>ulDelta2?ulDelta2:ulDelta1);
		if (ulDelta==ulDelta2)
		{
			m_ulLastPacketTimestamp+=1;
		}
		else 
		{
			m_ulLastPacketTimestamp=ulCurTimestamp;
		}
	}
	else
	{
		m_ulLastPacketTimestamp=ulCurTimestamp;
	}
	m_ulLastFrameTimestamp=ulCurTimestamp;

	AUDEC_HEADER_RESET(pAudioPacket);
	AUDEC_HEADER_SET_SEQUENCE(pAudioPacket,m_usAudioSeqNo++);
	AUDEC_HEADER_SET_TIMESTAMP(pAudioPacket,m_ulLastPacketTimestamp);
	AUDEC_HEADER_SET_CODEC_ID(pAudioPacket,X_AUDIO_CODEC_HIK_G726);
	AUDEC_HEADER_SET_FEC(pAudioPacket,1);//强制前向纠错


	int nHeaderSize=AUDEC_HEADER_GET_LEN(pAudioPacket);
	fastmemcpy(pAudioPacket+nHeaderSize,pData,ulLen);

	m_rCallback.OnVIDEC_AH400CallbackAudioStreamData(pAudioPacket,nHeaderSize+ulLen);
}


int XAH400::StartMotionDetect(void)
{
	int nRet=StartMotionDetection(m_pChannelHandle);
	return nRet;
}

int XAH400::StopMotionDetect(void)
{
	int nRet=StopMotionDetection(m_pChannelHandle);
	return nRet;
}

int XAH400::SetupMotionDetect(int iGrade/*灵敏度等级0-6*/,RECT *RectList, int iRectCount)
{
	if (m_pRectResult!=NULL)
	{
		FREEMSG(m_pRectResult);
		m_pRectResult=NULL;
	}

	m_pRectResult=(int*)MALLOCMSG(iRectCount*sizeof(int));
	m_nRectCount=iRectCount;
					
	int nRet=AdjustMotionDetectPrecision(m_pChannelHandle, 0x80000000|(iGrade), 2, 18);
	nRet=SetupMotionDetection(m_pChannelHandle, RectList, iRectCount);
	return nRet;
}
 
int XAH400::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	if (nBrightness<0)
		nBrightness=0;
	else if (nBrightness>255)
		nBrightness=255;
	if (nContrast<0)
		nContrast=0;
	else if (nContrast>127)
		nContrast=127;
	if (nSaturation<0)
		nSaturation=0;
	else if (nSaturation>127)
		nSaturation=127;
	if (nHue<0)
		nHue=0;
	else if (nHue>255)
		nHue=255;

	int nRet=SetVideoPara(m_pChannelHandle, nBrightness, nContrast, nSaturation, nHue);
	return 0;
}

void XAH400::OnMotionDetected(void*pData)
{
	if (m_pRectResult!=NULL)
	{
		MotionAnalyzer(m_pChannelHandle, (char *)pData, 0, m_pRectResult);
		m_rCallback.OnVIDEC_AH400CallbackMotionDetected(m_pRectResult,m_nRectCount);
	}
}

//设置遮掩并启动遮掩
int XAH400::SetupMask(RECT *RectList, int iRectCount)
{
	int nRet=::SetupMask(m_pChannelHandle,RectList,iRectCount);
	return nRet;
}

//停止遮掩
int XAH400::StopMask(void)
{
	int nRet=::StopMask(m_pChannelHandle);
	return nRet;
}

void XAH400::EnableOSD(bool bEnable)
{
	//m_bEnableOSD=bEnable;

	//SetOsd(m_pChannelHandle, m_bEnableOSD);
}

void XAH400::SetOSDText(void)
{
	//USHORT Format1[] = {16, 16, _OSD_YEAR4, '-',_OSD_MONTH2,'-',_OSD_DAY,' ',  _OSD_HOUR24, ':', _OSD_MINUTE,':', _OSD_SECOND, '\0'};
	//USHORT Format2[] = { 0, 0, 0x20, 0};

	//SetOsdDisplayMode(m_pChannelHandle, 255, TRUE, 1, Format1, Format2);
}


void ImageStreamCallback(UINT channelNumber,void *Context)
{
	{
		XAutoLock l(g_csArrayXAH400);
		if (channelNumber<AH400_DEV_MAX_COUNT)
		{
			if (g_paXAH400[channelNumber])
			{
				g_paXAH400[channelNumber]->OnImageStreamCallback();
			}
		}
	}
}

int __cdecl StreamDirectReadCallback(ULONG channelNum,void *DataBuf,DWORD Length,int frameType,void *context)
{
	if (channelNum<AH400_DEV_MAX_COUNT)
	{
		switch (frameType)
		{
		case PktIFrames:
		case PktPFrames:
		case PktBBPFrames:
		case PktBPFrames:
		case PktSFrames:
			{
				XAutoLock l(g_csArrayXAH400);
				if (g_paXAH400[channelNum])
				{
					g_paXAH400[channelNum]->OnVideoFrameMain((unsigned char*)DataBuf,Length,(frameType==PktIFrames));
				}
			}
			break;
		case PktAudioFrames:
			{
				XAutoLock l(g_csArrayXAH400);
				if (g_paXAH400[channelNum])
				{
					g_paXAH400[channelNum]->OnAudioFramePacket((unsigned char*)DataBuf,Length);
				}
			}
			break;
		case PktOrigImage:
			{
				//TRACE(_T("OrigImage Length=%u\n"),Length);
			}
			break;
		case PktSysHeader:
			{
				//TRACE(_T("SysHeader Length=%u\n"),Length);
			}
			break;
		case PktSubIFrames:
		case PktSubPFrames:
		case PktSubBBPFrames:
			{
				XAutoLock l(g_csArrayXAH400);
				if (g_paXAH400[channelNum])
				{
					g_paXAH400[channelNum]->OnVideoFrameSub((unsigned char*)DataBuf,Length,(frameType==PktSubIFrames));
				}
			}
			break;
		case PktSubSysHeader:
			{
			}
			break;
		case PktError:
			{

			};
			break;
		case PktMotionDetection:
			{
				XAutoLock l(g_csArrayXAH400);
				if (g_paXAH400[channelNum])
				{
					g_paXAH400[channelNum]->OnMotionDetected(DataBuf);
				}
			};
			break;
		case PktDspStatus:
			{

			};
			break;
		};
	}
	return 0;
}



extern bool g_bUserError;

bool IsUsedHPHiKDriver(void)
{
	bool bUsedHPHiKDriver=false;

	for (int i=0;i<100;i++)
	{
		TCHAR szMainKey[1024];
		if (i<10)
		{
			_stprintf(szMainKey,_T("SYSTEM\\ControlSet001\\Control\\Class\\{4D36E96C-E325-11CE-BFC1-08002BE10318}\\000%d"),i);
		}
		else if (i<100)
		{
			_stprintf(szMainKey,_T("SYSTEM\\ControlSet001\\Control\\Class\\{4D36E96C-E325-11CE-BFC1-08002BE10318}\\00%d"),i);
		}
		else if (i<1000)
		{
			_stprintf(szMainKey,_T("SYSTEM\\ControlSet001\\Control\\Class\\{4D36E96C-E325-11CE-BFC1-08002BE10318}\\0%d"),i);
		}
		else// if (i<1000)
		{
			_stprintf(szMainKey,_T("SYSTEM\\ControlSet001\\Control\\Class\\{4D36E96C-E325-11CE-BFC1-08002BE10318}\\%d"),i);
		}

		CRegKey tRegKey;
		if (ERROR_SUCCESS==tRegKey.Open(HKEY_LOCAL_MACHINE,szMainKey))
		{
			TCHAR szValue[1024]=_T("");
			DWORD dwLen=1024;
			if (ERROR_SUCCESS==tRegKey.QueryStringValue(_T("ProviderName"),szValue,&dwLen))
			{
				if (_tcscmp(szValue,_T("SHANGHAI HUAPING COMPUTER TECHINOLOGY CO.,LTD."))==0)
				{
					bUsedHPHiKDriver=true;
				}
			}
		}

		tRegKey.Close();

		if (bUsedHPHiKDriver)
		{
			break;
		}
	}
	return bUsedHPHiKDriver;
}

bool IsNewHiKBoard(void)
{
	unsigned char pSN[16]={5,1,1,2,0,0,7,9,5,2,3,5,0,0,0,0};
	bool bNewSN=false;

	DS_BOARD_DETAIL dsBoardDetail;
	unsigned int ulBoardCount=GetBoardCount();
	for (unsigned i=0;i<ulBoardCount;i++)
	{
		GetBoardDetail(i,&dsBoardDetail);

		for (int j=0;i<16;j++)
		{
			if (dsBoardDetail.sn[j]<pSN[j])
				break;

			if (dsBoardDetail.sn[j]>pSN[j])
			{
				bNewSN=true;
				break;
			}
		}
		if (bNewSN)
		{
			break;
		}
	}

	return bNewSN;
}


static long g_nAH400InitedCount=0;
int VIDEC_AH400::Init(void)
{
	if (g_nAH400InitedCount==0)
	{
		{
			XAutoLock l(g_csArrayXAH400);
			for (int i=0;i<AH400_DEV_MAX_COUNT;i++)
			{
				g_paXAH400[i]=NULL;
			}
		}
	
		InitDSPs();

		if (::GetTotalChannels()>0)
		{
			::RegisterImageStreamCallback(::ImageStreamCallback,NULL);
			::RegisterStreamDirectReadCallback(::StreamDirectReadCallback,NULL);
		}

		if (GetUsedByProductType()==AVCON_PRODUCT_RS ||
			GetUsedByProductType()==AVCON_PRODUCT_PICC)
		{
			g_bUserError=false;

			if (IsNewHiKBoard())
			{
				if (!IsUsedHPHiKDriver())
				{
					g_bUserError=true;
				}
			}
		}
	}

	g_nAH400InitedCount++;

	return 0;
}

void VIDEC_AH400::Terminate(void)
{
	g_nAH400InitedCount--;
	if (g_nAH400InitedCount<=0)
	{
		g_nAH400InitedCount=0;

		{
			XAutoLock l(g_csArrayXAH400);
			for (int i=0;i<AH400_DEV_MAX_COUNT;i++)
			{
				if (g_paXAH400[i])
				{
					HANDLE pChannelHandle=g_paXAH400[i]->GetChannelHandle();
					if (pChannelHandle!=(HANDLE)0xFFFFFFFF)
					{
						::SetImageStream(pChannelHandle,0,0,0,0,0);
						::ChannelClose(pChannelHandle);
					}

					g_paXAH400[i]=NULL;
				}
			}
		}
	
		DeInitDSPs();
	}
}

int VIDEC_AH400::GetDevCount(void)
{
	int nChannelCount=0;
	nChannelCount=::GetTotalChannels();

	return nChannelCount;
}

int VIDEC_AH400::GetDevName(int nDevID,char*szName,int nMaxCount)
{
	if (nDevID<0 || nDevID>=VIDEC_AH400::GetDevCount())
		return -1;

	if (nMaxCount<=0)
		return -1;

	char szTemp[128];
	sprintf(szTemp,"AVCON AH400 DEV (%d)",nDevID);

	if (szName==NULL)
	{
		return strlen(szTemp)+1;
	}

	if (nMaxCount>=strlen(szTemp)+1)
	{
		strcpy(szName,szTemp);
	}
	else
	{
		strncpy(szName,szTemp,nMaxCount);
	}

	return strlen(szName)+1;
}

void*VIDEC_AH400::GetDevHandle(int nDevID)
{
	XAutoLock l(g_csArrayXAH400);
	if (nDevID<AH400_DEV_MAX_COUNT && nDevID>=0)
	{
		if (g_paXAH400[nDevID])
		{
			HANDLE pChannelHandle=g_paXAH400[nDevID]->GetChannelHandle();
			return pChannelHandle;
		}
	}

	return (void*)0xFFFFFFFF;
}

VIDEC_AH400*VIDEC_AH400::Create(VIDEC_AH400Callback&rCallback,int nDevID,int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate)
{
	XAH400*pXAH400;
	NEWRECORD( pXAH400 , new XAH400(rCallback) );
	NULLReturnRet( pXAH400  , pXAH400)  

	if (0!=pXAH400->Open(nDevID,nWidth,nHeight,nFrameRate,bAutoFrameRate))
	{
		pXAH400->Close();
		delete pXAH400;
		pXAH400=NULL;
	}

	return pXAH400;
}

#endif