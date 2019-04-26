#include "AUDEC_plc.h"
#include <stdlib.h>
#include <memory.h>


AUDEC_plc::AUDEC_plc(void)
:m_nSamplesPerFrame(0)
,m_pSamplesBuf(NULL)
{
}

AUDEC_plc::~AUDEC_plc(void)
{
}

int AUDEC_plc::Open(int nSampleRate,int nSamplesPerFrame)
{
	m_nSamplesPerFrame=nSamplesPerFrame;

	m_pSamplesBuf=(short*)malloc(nSamplesPerFrame<<1);

	// 090903 by Ryan Liu :
	m_pSamplesTmpBuf = (short*)malloc(nSamplesPerFrame<<1);

	if (m_plc.Open(nSampleRate,nSamplesPerFrame)<0)
		return -1;

	return 0;
}

void AUDEC_plc::Close(void)
{
	if (m_pSamplesBuf!=NULL)
	{
		free(m_pSamplesBuf);
		m_pSamplesBuf=NULL;
	}

	// 090903 by Ryan Liu :
	if (m_pSamplesTmpBuf!=NULL)
	{
		free(m_pSamplesTmpBuf);
		m_pSamplesTmpBuf=NULL;
	}

	m_plc.Close();
}

int AUDEC_plc::Save(short*pSamples)
{
	// 090903 by Ryan Liu :
	memcpy( m_pSamplesTmpBuf, pSamples, m_nSamplesPerFrame<<1 );
	m_plc.Save(/*pSamples*/m_pSamplesTmpBuf, m_pSamplesBuf);
	return 0;
}

int AUDEC_plc::Recover(short*pSamples)
{
	m_plc.Recover(pSamples, m_pSamplesBuf);
	return 0;
}
