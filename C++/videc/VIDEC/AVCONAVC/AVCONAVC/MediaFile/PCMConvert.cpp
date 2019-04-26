#include "stdafx.h"
#include "PCMConvert.h"

PCMConvert::PCMConvert(void)
:m_hACMStream(NULL)
{

}

PCMConvert::~PCMConvert(void)
{
}

bool PCMConvert::Connect(LPWAVEFORMATEX pwfxSrc,LPWAVEFORMATEX pwfxDst)
{
	MMRESULT result = acmStreamOpen(&m_hACMStream,
								  NULL, // driver
								  pwfxSrc, // source format
								  pwfxDst, // destination format
								  NULL, // no filter
								  NULL, // no callback
								  0, // instance data (not used)
								  0); // flags
	if (result != 0) 
	{
		return false;
	}
	return true;
}

void PCMConvert::ReleaseConnections(void)
{
	if (m_hACMStream != NULL)
	{
		acmStreamClose(m_hACMStream, 0);
		m_hACMStream=NULL;
	}
}

int PCMConvert::Convert(char*pSrc,int nSrc,char*pDst,int&nDst)
{
	PBYTE pInBuffer=(PBYTE)pSrc;
	PBYTE pOutBuffer=(PBYTE)pDst;
	ACMSTREAMHEADER acmStreamHeader;
	memset(&acmStreamHeader, 0, sizeof(acmStreamHeader));
	acmStreamHeader.cbStruct = sizeof(acmStreamHeader);
	acmStreamHeader.pbSrc = pInBuffer;
	acmStreamHeader.cbSrcLength = nSrc;
	acmStreamHeader.pbDst = pOutBuffer;
	acmStreamHeader.cbDstLength = nDst;

	// prep the acmStreamHeader
	MMRESULT result = acmStreamPrepareHeader(m_hACMStream, &acmStreamHeader, 0);
	if (result != 0) 
	{
		return 0;
	}

	result = acmStreamConvert(m_hACMStream, &acmStreamHeader, 0);
	if (result != 0) 
	{
		return 0;
	}

	nDst=acmStreamHeader.cbDstLengthUsed;

	return 1;
}

