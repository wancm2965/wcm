//VBRControl.h
#include "VBRControl.h"
#include "XUtil.h"

#include <winsock2.h>


#define MAX_VBR_CTRL_INTERVAL	1600

VBRControl::VBRControl(void)
{
	int nMinQP=28;
	int nMaxQP=36;
	int nMaxBitrate=384;

	m_nMinQP=nMinQP;
	m_nMaxQP=nMaxQP;
	m_nCurQP=m_nMinQP;
	
	m_nMaxBitrate=nMaxBitrate;
	m_nCurBitrate=m_nMaxBitrate;
	m_nLastBitrate=0;
	

	m_ulLastOverflowTimestamp=XGetTimestamp();
	m_ulAdjustTimestamp=XGetTimestamp();
}

VBRControl::~VBRControl(void)
{
}

void VBRControl::Reset(int nMinQP,int nMaxQP,int nMaxBitrate)
{
	nMaxBitrate*=875;
	nMaxBitrate/=1000;

	if (m_nMaxBitrate!=nMaxBitrate)
	{
		if (m_nCurBitrate==m_nMaxBitrate)
		{
			m_nCurBitrate=m_nMaxBitrate=nMaxBitrate;
		}
		else
		{
			m_nMaxBitrate=nMaxBitrate;
			if (m_nCurBitrate>m_nMaxBitrate)
			{
				m_nCurBitrate=m_nMaxBitrate;
			}
		}
	}

	if (m_nMinQP!=nMinQP)
	{
		if (m_nCurQP<nMinQP)
		{
			m_nCurQP=m_nMinQP=nMinQP;
		}
		else
		{
			m_nMinQP=nMinQP;
		}
	}
		
	if (m_nMaxQP!=nMaxQP)
	{
		if (m_nCurQP>nMaxQP)
		{
			m_nCurQP=m_nMaxQP=nMaxQP;
		}
		else
		{
			m_nMaxQP=nMaxQP;
		}
	}

	m_nCurQP=(m_nMaxQP+m_nMinQP)/2;
}

int VBRControl::GetNextQP(int&nCanEncThisFrame)
{
	nCanEncThisFrame=1;

	unsigned long ulTimestamp=XGetTimestamp();
	int nBitrate=GetBitrate();
	if (nBitrate>m_nMaxBitrate && m_nCurQP==m_nMaxQP)
	{
		m_ulLastOverflowTimestamp=ulTimestamp;
		nCanEncThisFrame=0;
		return m_nCurQP;
	}

	int nRatio=nBitrate*100/m_nCurBitrate;
	int nOffsetBitrate=m_nCurBitrate/10;
	if (nOffsetBitrate>128)
		nOffsetBitrate=128;

	if (nBitrate>m_nCurBitrate)
	{
		m_ulLastOverflowTimestamp=ulTimestamp;

		if ((nBitrate+nOffsetBitrate)<m_nCurBitrate)
		{
		}
		else
		{
			m_nCurQP++;
		}

		if (m_nCurQP>m_nMaxQP)
		{
			m_nCurQP=m_nMaxQP;
		}
	}
	else if (nRatio<75)
	{
		if ((nBitrate+nOffsetBitrate)>=m_nCurBitrate)
		{
		}
		else if ((ulTimestamp-m_ulAdjustTimestamp>=500) && (ulTimestamp-m_ulLastOverflowTimestamp>=2000))
		{
			m_ulAdjustTimestamp=ulTimestamp;
			m_nCurQP--;
		}

		if (m_nCurQP<m_nMinQP)
		{
			m_nCurQP=m_nMinQP;
		}
	}

	//char szDebug[1024];
	//sprintf(szDebug,"m_nCurQP=%d nBitrate=%d m_nCurBitrate=%d\n",m_nCurQP,nBitrate,m_nCurBitrate);
	//OutputDebugString(szDebug);
	
	return m_nCurQP;
}

