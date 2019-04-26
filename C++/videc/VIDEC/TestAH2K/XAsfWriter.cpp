#include "stdafx.h"
#include "XAsfWriter.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x )  \
    if (NULL != x)      \
    {                   \
        x->Release();   \
        x = NULL;       \
    }
#endif
#ifndef BREAK_IF_FAILED
#define BREAK_IF_FAILED(hr) if(FAILED(hr)) break;
#endif

const WORD cAudioStreamNum = 6;
const WORD cVideoStreamNum = 8;

AVCONEC_ASFWriter*AVCONEC_ASFWriter::Create(void)
{
	return new XAsfWriter();
}

XAsfWriter::XAsfWriter(void)
{
	mProfile = NULL;
	mWriter  = NULL;
	mIHeaderInfo      = NULL;
	mIWriterAdvanced  = NULL;
	mIWriterAdvanced3 = NULL;
	mProfileManager   = NULL;
	WMCreateProfileManager(&mProfileManager);

	mAudioTime = 0;
	mVideoTime = 0;

	wcscpy(mDestFile, L"");
}

XAsfWriter::~XAsfWriter(void)
{

}

int XAsfWriter::Open(const WCHAR*pFile,FOURCC fourcc,int nWidth,int nHeight,int nFrameRate,int nBitRate)
{
	HRESULT hr = NOERROR;

	wcscpy(mDestFile, pFile);

	do
	{
		// Create an empty profile object
		hr = mProfileManager->CreateEmptyProfile(WMT_VER_9_0, &mProfile);
		BREAK_IF_FAILED(hr);
	
		hr=CreateStream(fourcc,nWidth,nHeight,nFrameRate,nBitRate);
		BREAK_IF_FAILED(hr);

		hr=CreateWriter();
		BREAK_IF_FAILED(hr);

		hr=ConfigWriter();
		BREAK_IF_FAILED(hr);

		// Write some attributes
		hr = WriteHeader(g_wszWMTitle, L"ASF Clip");
		hr = WriteHeader(g_wszWMDescription, L"Powered by AVCON");

		// Begin writing...
		hr = mWriter->BeginWriting();
		BREAK_IF_FAILED(hr);

	}while(0);

	if(FAILED(hr))
	{
		return -1;
	}

	return 0;
}

HRESULT XAsfWriter::CreateStream(FOURCC fourcc,int nWidth,int nHeight,int nFrameRate,int nBitRate)
{
	HRESULT hr = NOERROR;

	IWMStreamConfig *pStreamConfig	= NULL;  
	IWMMediaProps *pMediaProps		= NULL;  

	WCHAR   * wcStreamName       =   L"H264 Stream";  
	WCHAR   * wcConnectionName   =   L"H264 Connection";  

	WM_MEDIA_TYPE mt;  
	WMVIDEOINFOHEADER videoInfo;  

	const GUID WMMEDIATYPE_SubType_DivX  =    
	{0x58564944,   0x0000,   0x0010,   0x80,   0x00,   0x00,   0xAA,   0x00,   0x38,   0x9B,   0x71};  

	do
	{
		hr   =   mProfile->CreateNewStream(WMMEDIATYPE_Video,   &pStreamConfig);  
		BREAK_IF_FAILED(hr);
		   
		hr   =   pStreamConfig->SetStreamNumber(cVideoStreamNum);  
		BREAK_IF_FAILED(hr);
		hr   =   pStreamConfig->SetStreamName(wcStreamName);  
		BREAK_IF_FAILED(hr);
		hr   =   pStreamConfig->SetConnectionName(wcConnectionName);  
		BREAK_IF_FAILED(hr);
		hr   =   pStreamConfig->SetBufferWindow(3000);  
		BREAK_IF_FAILED(hr);
		hr   =   pStreamConfig->SetBitrate(nBitRate);  
		BREAK_IF_FAILED(hr);
		   
		   
		ZeroMemory(&videoInfo,   sizeof(videoInfo));  
		videoInfo.bmiHeader.biSize =   sizeof(BITMAPINFOHEADER);  
		videoInfo.bmiHeader.biCompression =   fourcc;  
		videoInfo.bmiHeader.biBitCount =   12;  
		videoInfo.bmiHeader.biWidth =   nWidth;  
		videoInfo.bmiHeader.biHeight =   nHeight;  
		videoInfo.bmiHeader.biPlanes =   1;  
		videoInfo.bmiHeader.biSizeImage =   0;  
		videoInfo.bmiHeader.biClrImportant =   0;  
		   
		videoInfo.rcSource.left =   0;  
		videoInfo.rcSource.top =   0;  
		videoInfo.rcSource.right =   videoInfo.bmiHeader.biWidth;  
		videoInfo.rcSource.bottom =   videoInfo.bmiHeader.biHeight;  
		videoInfo.rcTarget =   videoInfo.rcSource;  
		   
		videoInfo.dwBitRate =   nBitRate;  
		videoInfo.dwBitErrorRate =   0;  
		videoInfo.AvgTimePerFrame =   10000000   /   nFrameRate;  
		   
		ZeroMemory(&mt,   sizeof(mt));  
		mt.majortype =   WMMEDIATYPE_Video;  
		mt.subtype =   WMMEDIASUBTYPE_Base;  
		mt.subtype.Data1 = fourcc;  
		mt.bFixedSizeSamples =   FALSE;  
		mt.bTemporalCompression =   TRUE;  
		mt.lSampleSize =   0;  
		mt.formattype =   WMFORMAT_VideoInfo;  
		mt.pUnk =   NULL;  
		mt.cbFormat =   sizeof(WMVIDEOINFOHEADER);  
		mt.pbFormat =   (BYTE*)&videoInfo;  
		   
		hr   =   pStreamConfig->QueryInterface(   IID_IWMMediaProps,   (void**)&pMediaProps   );  
		BREAK_IF_FAILED(hr);
		hr   =   pMediaProps->SetMediaType(   &mt   );  
		BREAK_IF_FAILED(hr);
		   
		hr   =   mProfile->AddStream(pStreamConfig);  
		BREAK_IF_FAILED(hr);
		
	}while (0);

	SAFE_RELEASE(pStreamConfig);
	SAFE_RELEASE(pMediaProps);

	return hr;
}
void XAsfWriter::Close(void)
{
	if (mWriter)
	{
		mWriter->EndWriting();
	}

	SAFE_RELEASE(mProfile);
	SAFE_RELEASE(mProfileManager);
	SAFE_RELEASE(mIHeaderInfo);
	SAFE_RELEASE(mIWriterAdvanced);
	SAFE_RELEASE(mIWriterAdvanced3);
	SAFE_RELEASE(mWriter);
}

HRESULT XAsfWriter::CreateWriter(void)
{
	if (mWriter)
	{
		return NOERROR;
	}

	HRESULT hr = NOERROR;
	do
	{
		hr = WMCreateWriter(NULL, &mWriter);
		BREAK_IF_FAILED(hr);

		hr = mWriter->QueryInterface(IID_IWMWriterAdvanced, (void**)&mIWriterAdvanced);
		BREAK_IF_FAILED(hr);

		hr = mWriter->QueryInterface(IID_IWMHeaderInfo, (void**)&mIHeaderInfo);
		BREAK_IF_FAILED(hr);

		// Extra interfaces
		mWriter->QueryInterface(IID_IWMWriterAdvanced3, (void**)&mIWriterAdvanced3);

	} while (FALSE);

	return hr;
}

HRESULT XAsfWriter::ConfigWriter(void)
{
	HRESULT hr = NOERROR;

	do
	{
		hr = mWriter->SetProfile(mProfile);
		BREAK_IF_FAILED(hr);

		////////////////////////////////////////////////////////////////
		// Input settings on the writer
		DWORD cInputs = 0;
		hr = mWriter->GetInputCount(&cInputs);
		BREAK_IF_FAILED(hr);

		// Loop through all of the inputs on the writer. For each input, 
		// set the input properties to NULL. This tells the writer not 
		// to encode the samples it receives.
		for (DWORD i = 0; i < cInputs; i++)
		{
			hr = mWriter->SetInputProps(i, NULL);
			BREAK_IF_FAILED(hr);
		}
		////////////////////////////////////////////////////////////////

		// Set the destination file
		hr = mWriter->SetOutputFilename(mDestFile);
		BREAK_IF_FAILED(hr);

	} while (FALSE);

	return hr;
}

// Set specified metadata to the writer
HRESULT XAsfWriter::WriteHeader(const WCHAR * pwszName, WCHAR * pValue)
{
	WORD streamNum = 0; // file-level attribute
	WORD cbLength  = WORD(wcslen(pValue) * 2);
	HRESULT		hr = S_OK;

	if (mIHeaderInfo)
	{
		hr = mIHeaderInfo->SetAttribute(streamNum, pwszName, 
			WMT_TYPE_STRING, (const BYTE*) pValue, cbLength);
		return hr;
	}
	return E_FAIL;
}

int XAsfWriter::WriteVideo(unsigned char*pData,int nLen,bool bKeyFrame,unsigned long ulTimestamp)
{
	HRESULT hr = NOERROR;
	REFERENCE_TIME rtStart, rtEnd;

	rtStart=ulTimestamp*10000;
	rtEnd=rtStart+333333;
	{
		mVideoTime = rtStart;
		QWORD duration = QWORD(rtEnd - rtStart);

		INSSBuffer * pWmSample = NULL;
		hr = mWriter->AllocateSample(nLen, &pWmSample);
		if (SUCCEEDED(hr))
		{
			PBYTE pDest;
			pWmSample->GetBuffer(&pDest);
			memcpy(pDest, pData, nLen);
			pWmSample->SetLength(nLen);

			// Send sample to the writer
			DWORD flags = 0;
			if (bKeyFrame)
			{
				flags |= WM_SF_CLEANPOINT;
			}
			//if (S_OK == pSample->IsDiscontinuity())
			//{
			//	flags |= WM_SF_DISCONTINUITY;
			//}
			hr = mIWriterAdvanced->WriteStreamSample(cVideoStreamNum, rtStart, 
				DWORD(rtStart / 10000), duration, flags, pWmSample);
			pWmSample->Release();
		}
	}
	return 0;
}
