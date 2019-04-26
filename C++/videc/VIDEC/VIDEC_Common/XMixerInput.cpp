#include "XMixerInput.h"
#include "XMixer.h"

XMixerInput::XMixerInput(XMixer&rXMixer,int nRectIndex,bool bVisible)
:m_rXMixer(rXMixer)
,m_nRectIndex(nRectIndex)
,m_bVisible(bVisible)

,m_pImageBuf(NULL)
,m_pImageBufLen(0)
,m_nImageSize(0)
,m_nImageWidth(0)
,m_nImageHeight(0)
{

}

XMixerInput::~XMixerInput(void)
{

}

void XMixerInput::Close(void)
{
	m_rXMixer.RemoveXMixerInput(this);

	if (m_pImageBuf!=NULL)
	{
		free(m_pImageBuf);
		m_pImageBuf=NULL;
	}
}

void XMixerInput::SetRectIndex(int nIndex)
{
	m_nRectIndex=nIndex;
}

int XMixerInput::GetRectIndex(void)
{
	return m_nRectIndex;
}

void XMixerInput::SetVisible(bool bVisible)
{
	m_bVisible=bVisible;
}

bool XMixerInput::IsVisible(void)
{
	return m_bVisible;
}

int XMixerInput::InputImage(unsigned char*pData,int nLen,int nWidth,int nHeight)
{
	XAutoLock l(*this);

	if (m_pImageBuf==NULL || m_pImageBufLen<nLen)
	{
		if (m_pImageBuf!=NULL)
		{
			free(m_pImageBuf);
			m_pImageBuf=NULL;
		}

		m_pImageBufLen=nLen;
		m_pImageBuf=(unsigned char*)malloc(m_pImageBufLen);
		if (m_pImageBuf==NULL)
		{
			return -1;
		}
	}

	m_nImageSize=nLen;
	m_nImageWidth=nWidth;
	m_nImageHeight=nHeight;

	memcpy(m_pImageBuf,pData,nLen);

	return 0;
}


unsigned char*XMixerInput::GetImageData(void)
{
	return m_pImageBuf;
}

int XMixerInput::GetImageSize(void)
{
	return m_nImageSize;
}

int XMixerInput::GetImageWidth(void)
{
	return m_nImageWidth;
}

int XMixerInput::GetImageHeight(void)
{
	return m_nImageHeight;
}


XMixerImageInput::XMixerImageInput(XMixer&rXMixer,int nRectIndex,bool bVisible)
:XMixerInput(rXMixer,nRectIndex,bVisible)
{

}

XMixerImageInput::~XMixerImageInput(void)
{

}

void XMixerImageInput::Close(void)
{
	XMixerInput::Close();
}

void XMixerImageInput::SetRectIndex(int nIndex)
{
	XMixerInput::SetRectIndex(nIndex);
}

int XMixerImageInput::GetRectIndex(void)
{
	return XMixerInput::GetRectIndex();
}

void XMixerImageInput::SetVisible(bool bVisible)
{
	XMixerInput::SetVisible(bVisible);
}

bool XMixerImageInput::IsVisible(void)
{
	return XMixerInput::IsVisible();
}

int XMixerImageInput::InputImage(unsigned char*pData,int nLen,int nWidth,int nHeight)
{
	return XMixerInput::InputImage(pData,nLen,nWidth,nHeight);
}


XMixerRTPInput::XMixerRTPInput(XMixer&rXMixer,int nRectIndex,bool bVisible)
:XMixerInput(rXMixer,nRectIndex,bVisible)
{

}

XMixerRTPInput::~XMixerRTPInput(void)
{

}

void XMixerRTPInput::Close(void)
{
	XMixerInput::Close();
}

void XMixerRTPInput::SetRectIndex(int nIndex)
{
	XMixerInput::SetRectIndex(nIndex);
}

int XMixerRTPInput::GetRectIndex(void)
{
	return XMixerInput::GetRectIndex();
}

void XMixerRTPInput::SetVisible(bool bVisible)
{
	XMixerInput::SetVisible(bVisible);
}

bool XMixerRTPInput::IsVisible(void)
{
	return XMixerInput::IsVisible();
}

int XMixerRTPInput::InputRTP(unsigned char*pData,int nLen)
{
	return 0;
}


XMixerStreamInput::XMixerStreamInput(XMixer&rXMixer,int nRectIndex,bool bVisible)
:XMixerInput(rXMixer,nRectIndex,bVisible)
{

}

XMixerStreamInput::~XMixerStreamInput(void)
{

}

void XMixerStreamInput::Close(void)
{
	XMixerInput::Close();
}

void XMixerStreamInput::SetRectIndex(int nIndex)
{
	XMixerInput::SetRectIndex(nIndex);
}

int XMixerStreamInput::GetRectIndex(void)
{
	return XMixerInput::GetRectIndex();
}

void XMixerStreamInput::SetVisible(bool bVisible)
{
	XMixerInput::SetVisible(bVisible);
}

bool XMixerStreamInput::IsVisible(void)
{
	return XMixerInput::IsVisible();
}

int XMixerStreamInput::InputStream(unsigned char*pData,int nLen)
{
	return 0;
}
