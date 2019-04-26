#include "ImageInput.h"
#include "../VIDEC_API/XImage.h"
#include "XUtil.h"
	
ImageInput::ImageInput(void)
{
	m_nScreenIndex=-1;
	m_nWndIndex=-1;
	m_nFullScreen=0;
}
	
ImageInput::~ImageInput(void)
{

}

int ImageInput::Open(void)
{
	ImageInputMgr::Instance().AddImageInput(this);
	return 0;
}

void ImageInput::Close(void)
{
	ImageInputMgr::Instance().RemoveImageInput(this);
}

void ImageInput::SetScreenIndex(int nScreenIndex)
{
	if (nScreenIndex!=m_nScreenIndex)
	{
		int nOldScreenIndex=m_nScreenIndex;
		m_nScreenIndex=nScreenIndex;
		ImageInputMgr::Instance().OnImageInputScreenIndexChanged(this,nOldScreenIndex);
	}
}

int ImageInput::GetScreenIndex(void)
{
	return m_nScreenIndex;
}
	
void ImageInput::SetWndIndex(int nWndIndex)
{
	if( m_nWndIndex!=nWndIndex)
	{
		m_nWndIndex=nWndIndex;

		ImageInputMgr::Instance().OnImageInputWndIndexChanged(this);
	}
}
	
int ImageInput::GetWndIndex(void)
{
	return m_nWndIndex;
}

void ImageInput::SetFullScreen(int nFullScreen)
{
	m_nFullScreen=nFullScreen;
	ImageInputMgr::Instance().OnImageInputFullScreenChanged(this);
}
	
int ImageInput::GetFullScreen(void)
{
	return m_nFullScreen;
}


ScreenInput::ScreenInput(int x,int y,int cx,int cy)
{
	m_pFullScreenImageInput=NULL;
	m_nWndCount=0;

	SetScreenPos(x,y,cx,cy);
	
	for (int i=0;i<MAX_WND_COUNT;i++)
	{
		m_aWndRect[i].left=0;
		m_aWndRect[i].top=0;
		m_aWndRect[i].right=0;
		m_aWndRect[i].bottom=0;
		m_aMappedWndRect[i].left=0;
		m_aMappedWndRect[i].top=0;
		m_aMappedWndRect[i].right=0;
		m_aMappedWndRect[i].bottom=0;
	}
}


ScreenInput::~ScreenInput(void)
{

}

void ScreenInput::SetImageSize(int nWidth,int nHeight)
{
	{
		XAutoLock l(m_csImageInput);
		ImageMixer::SetImageSize(nWidth,nHeight);
	}

	if (m_nImageWidth>0 && m_nImageHeight>0 && m_nScreenCX>0 && m_nScreenCY>0)
	{
		double dblTempCX=(double)m_nImageWidth/(double)m_nScreenCX;
		double dblTempCY=(double)m_nImageHeight/(double)m_nScreenCY;

		for (int i=0;i<m_nWndCount;i++)
		{
			int x=m_aWndRect[i].left;
			int y=m_aWndRect[i].top;
			int cx=m_aWndRect[i].right-m_aWndRect[i].left;
			int cy=m_aWndRect[i].bottom-m_aWndRect[i].top;

			int nMappedX=(x-m_nScreenX)*dblTempCX;
			int nMappedY=(y-m_nScreenY)*dblTempCY;
			int nMappedCX=cx*dblTempCX;
			int nMappedCY=cy*dblTempCY;
			//20160427 解决新的VDP合成库在奇数时会出现偏色 YUV分离的问题 
			//nMappedX = nMappedX >>1 <<1;
			//nMappedY = nMappedY >>1 <<1;

			m_aMappedWndRect[i].left=nMappedX;
			m_aMappedWndRect[i].top=nMappedY;
			m_aMappedWndRect[i].right=nMappedX+nMappedCX;
			m_aMappedWndRect[i].bottom=nMappedY+nMappedCY;

			//char szDebug[1024]="";
			//sprintf(szDebug,"index=%d x=%d y=%d cx=%d cy=%d nMappedX=%d nMappedY=%d nMappedCX=%d nMappedCY=%d\n",i,x,y,cx,cy,nMappedX,nMappedY,nMappedCX,nMappedCY);
			//DebugOutput(szDebug);
		}
	}
}

void ScreenInput::SetScreenPos(int x,int y,int cx,int cy)
{
	m_nScreenX=x=0;
	m_nScreenY=y=0;
	m_nScreenCX=cx;
	m_nScreenCY=cy;
}

void ScreenInput::SetScreenWndCount(int nWndCount)
{
	m_nWndCount=nWndCount;
	for (int i=m_nWndCount;i<MAX_WND_COUNT;i++)
	{
		m_aWndRect[i].left=0;
		m_aWndRect[i].top=0;
		m_aWndRect[i].right=0;
		m_aWndRect[i].bottom=0;

		m_aMappedWndRect[i].left=0;
		m_aMappedWndRect[i].top=0;
		m_aMappedWndRect[i].right=0;
		m_aMappedWndRect[i].bottom=0;
	}
}

void ScreenInput::SetScreenWndPos(int nWndIndex,int x,int y,int cx,int cy)
{
	if (nWndIndex>=0 && nWndIndex<MAX_WND_COUNT)
	{
		if (x>(m_nScreenCX-32))
		{
			x=m_nScreenCX-32;
		}

		if (x<0)
			x=0;

		if (cx<=0)
			cx=32;

		if (x+cx>m_nScreenCX)
		{
			cx=m_nScreenCX-x;
		}

		if (y>(m_nScreenCY-16))
		{
			y=m_nScreenCY-16;
		}

		if (y<0)
			y=0;

		if (cy<=0)
			cy=16;

		if (y+cy>m_nScreenCY)
		{
			cy=m_nScreenCY-y;
		}

		m_aWndRect[nWndIndex].left=x;
		m_aWndRect[nWndIndex].top=y;
		m_aWndRect[nWndIndex].right=x+cx;
		m_aWndRect[nWndIndex].bottom=y+cy;

		if (m_nImageWidth>0 && m_nImageHeight>0 && m_nScreenCX>0 && m_nScreenCY>0)
		{
			double dblTempCX=(double)m_nImageWidth/(double)m_nScreenCX;
			double dblTempCY=(double)m_nImageHeight/(double)m_nScreenCY;
			int nMappedX=(x-m_nScreenX)*dblTempCX;
			int nMappedY=(y-m_nScreenY)*dblTempCY;
			int nMappedCX=cx*dblTempCX;
			int nMappedCY=cy*dblTempCY;
			//20160427 解决新的VDP合成库在奇数时会出现偏色 YUV分离的问题 
			//nMappedX = nMappedX >>1 <<1;
			//nMappedY = nMappedY >>1 <<1;

			m_aMappedWndRect[nWndIndex].left=nMappedX;
			m_aMappedWndRect[nWndIndex].top=nMappedY;
			m_aMappedWndRect[nWndIndex].right=nMappedX+nMappedCX;
			m_aMappedWndRect[nWndIndex].bottom=nMappedY+nMappedCY;
			
			//char szDebug[1024]="";
			//sprintf(szDebug,"index=%d x=%d y=%d cx=%d cy=%d nMappedX=%d nMappedY=%d nMappedCX=%d nMappedCY=%d\n",nWndIndex,x,y,cx,cy,nMappedX,nMappedY,nMappedCX,nMappedCY);
			//DebugOutput(szDebug);
		}
	}
}

void ScreenInput::AddImageInput(ImageInput*pImageInput)
{
	XAutoLock l(m_csImageInput);
	if (m_listImageInput.find(pImageInput)==m_listImageInput.end())
	{
		m_listImageInput.push_back(pImageInput);
	}
}

void ScreenInput::RemoveImageInput(ImageInput*pImageInput)
{
	XAutoLock l(m_csImageInput);
	m_listImageInput.remove(pImageInput);
	if (m_pFullScreenImageInput==pImageInput)
	{
		m_pFullScreenImageInput=NULL;
	}
}

void ScreenInput::OnImageInputFullScreenChanged(ImageInput*pImageInput)
{
	XAutoLock l(m_csImageInput);
	if (pImageInput->GetFullScreen())
	{
		m_pFullScreenImageInput=pImageInput;
	}
	else
	{
		m_pFullScreenImageInput=NULL;
	}
}

void ScreenInput::OnImageInputWndIndexChanged(ImageInput*pImageInput)
{
	XAutoLock l(m_csImageInput);

	XListPtr tXListPtr;
	while (true)
	{
		ImageInput*pImageInputMinWndIndex=NULL;
		XListPtr::iterator iter=m_listImageInput.begin();
		while (iter!=m_listImageInput.end())
		{
			ImageInput*pImageInputTemp=(ImageInput*)(*iter);
			if (pImageInputMinWndIndex==NULL)
			{
				pImageInputMinWndIndex=pImageInputTemp;
			}
			else
			{
				if (pImageInputMinWndIndex->GetWndIndex()>pImageInputTemp->GetWndIndex())
				{
					pImageInputMinWndIndex=pImageInputTemp;
				}
			}

			++iter;
		}

		if (pImageInputMinWndIndex!=NULL)
		{
			m_listImageInput.remove(pImageInputMinWndIndex);
			tXListPtr.push_back(pImageInputMinWndIndex);
		}
		else
		{
			break;
		}
	}

	m_listImageInput.clear();
	while (tXListPtr.size()>0)
	{
		m_listImageInput.push_back(tXListPtr.front());
		tXListPtr.pop_front();
	}
}

VIDEC_Image*ScreenInput::GetImage(int nForce,int nFrameRate)
{
	bool bGotImage=false;

	XAutoLock l(m_csImageInput);

	VIDEC_Image*pImage=NULL;
	ImageInput*pImageInput=m_pFullScreenImageInput;
	if (pImageInput==NULL && m_nWndCount==1 && m_listImageInput.size()==1)
	{
		pImageInput=(ImageInput*)m_listImageInput.front();
	}
	if (pImageInput!=NULL)
	{
		if (m_pImageBuffer==NULL && m_nImageSize>0)
		{
			m_pImageBuffer=(unsigned char*)MALLOCMSG(m_nImageSize+(m_nImageSize>>2));
		}
		if (m_pImageBuffer!=NULL)
		{
			pImage=pImageInput->GetImage();
			if (pImage!=NULL)
			{
				bGotImage=true;
				int nWidth=pImage->GetWidth();
				int nHeight=pImage->GetHeight();
				if (nWidth==m_nImageWidth && nHeight==m_nImageHeight)
				{
					XImage*pXImage=dynamic_cast<XImage*>(pImage);
					pXImage->SetTimestamp(XGetTimestamp());

					return pImage;
				}
				else
				{
					ImageMixer::MixImage((unsigned char*)pImage->GetData(),pImage->GetWidth(),pImage->GetHeight(),0,0,m_nImageWidth,m_nImageHeight);

					pImage->Release();
					pImage=NULL;
				}
			}
			else
			{
				ImageMixer::FillRect(0,0,m_nImageWidth,m_nImageHeight);
			}
		}
	}
	else if (m_listImageInput.size())
	{
		if (m_pImageBuffer==NULL && m_nImageSize>0)
		{
			m_pImageBuffer=(unsigned char*)MALLOCMSG(m_nImageSize+(m_nImageSize>>2));
		}
		if (m_pImageBuffer!=NULL)
		{
			ImageMixer::FillRect(0,0,m_nImageWidth,m_nImageHeight);

			int x=0;
			int y=0;
			int cx=0;
			int cy=0;
			int nWndIndex=0;
			XListPtr::iterator iter=m_listImageInput.begin();
			while (iter!=m_listImageInput.end())
			{
				pImageInput=(ImageInput*)*iter;
				pImage=pImageInput->GetImage();
				if (pImage!=NULL)
				{
					bGotImage=true;

					nWndIndex=pImageInput->GetWndIndex();
					if (nWndIndex>=0 && nWndIndex<MAX_WND_COUNT)
					{
						x=m_aMappedWndRect[nWndIndex].left;
						y=m_aMappedWndRect[nWndIndex].top;
						cx=m_aMappedWndRect[nWndIndex].right-m_aMappedWndRect[nWndIndex].left;
						cy=m_aMappedWndRect[nWndIndex].bottom-m_aMappedWndRect[nWndIndex].top;

						ImageMixer::MixImage((unsigned char*)pImage->GetData(),pImage->GetWidth(),pImage->GetHeight(),x,y,cx,cy);
					}
					pImage->Release();
					pImage=NULL;
				}
				++iter;
			}
		}
	}
	
	if (!bGotImage && nForce)
	{
		if (m_pImageBuffer==NULL && m_nImageSize>0)
		{
			m_pImageBuffer=(unsigned char*)MALLOCMSG(m_nImageSize+(m_nImageSize>>2));
		}
		if (m_pImageBuffer!=NULL)
		{
			ImageMixer::FillRect(0,0,m_nImageWidth,m_nImageHeight);
			bGotImage=true;
		}
	}

	XImage*pXImage=NULL;
	if (bGotImage)
	{
		NEWRECORD( pXImage,new XImage(m_pImageBuffer,m_nImageSize) )
		NULLReturnRet( pXImage , pXImage )  
		pXImage->AddRef();
		m_pImageBuffer=NULL;

		pXImage->SetActualWidth(m_nImageWidth);
		pXImage->SetActualHeight(m_nImageHeight);
		pXImage->SetVirtualWidth(m_nImageWidth);
		pXImage->SetVirtualHeight(m_nImageHeight);
		pXImage->SetDoubleField(false);
		pXImage->SetEvenField(false);
		pXImage->SetOddField(false);
		pXImage->SetTimestamp(XGetTimestamp());
		pXImage->SetFrameRate(nFrameRate);
	}

	return pXImage;
}


static ImageInputMgr g_ImageInputMgr;
ImageInputMgr::ImageInputMgr(void)
{
	for (int i=0;i<MAX_SCREEN_COUNT;i++)
	{
		m_apScreenInput[i]=NULL;
	}
}

ImageInputMgr::~ImageInputMgr(void)
{
	for (int i=0;i<MAX_SCREEN_COUNT;i++)
	{
		if (m_apScreenInput[i]!=NULL)
		{
			delete m_apScreenInput[i];
			m_apScreenInput[i]=NULL;
		}
	}
}

ImageInputMgr&ImageInputMgr::Instance(void)
{
	return g_ImageInputMgr;
}


int ImageInputMgr::SetScreenImageSize(int nScreenIndex,int nScreenWidth,int nScreenHeight)
{
	if (nScreenIndex<0 || nScreenIndex>=MAX_SCREEN_COUNT)
	{
		return -1;
	}

	if (nScreenIndex>=0 && nScreenIndex<=MAX_SCREEN_COUNT)
	{
		if (m_apScreenInput[nScreenIndex]!=NULL)
		{
			m_apScreenInput[nScreenIndex]->SetImageSize(nScreenWidth,nScreenHeight);
		}
	}
	return 0;
}

int ImageInputMgr::SetScreenPos(int nScreenIndex,int x,int y,int cx,int cy)
{
	if (nScreenIndex<0 || nScreenIndex>=MAX_SCREEN_COUNT)
	{
		return -1;
	}
	
	ScreenInput*pScreenInput=m_apScreenInput[nScreenIndex];
	if (pScreenInput==NULL)
	{
		NEWRECORD( pScreenInput ,new ScreenInput(x,y,cx,cy) )
		NULLReturnRet( pScreenInput , -1 )  
		m_apScreenInput[nScreenIndex]=pScreenInput;
	}
	pScreenInput->SetScreenPos(x,y,cx,cy);
	return 0;
}

int ImageInputMgr::SetScreenWndCount(int nScreenIndex,int nWndCount)
{
	if (nScreenIndex<0 || nScreenIndex>=MAX_SCREEN_COUNT)
	{
		return -1;
	}

	if (nScreenIndex>=0 && nScreenIndex<=MAX_SCREEN_COUNT)
	{
		if (m_apScreenInput[nScreenIndex]!=NULL)
		{
			m_apScreenInput[nScreenIndex]->SetScreenWndCount(nWndCount);
		}
	}
	return 0;
}

int ImageInputMgr::SetScreenWndPos(int nScreenIndex,int nWndIndex,int x,int y,int cx,int cy)
{
	if (nScreenIndex<0 || nScreenIndex>=MAX_SCREEN_COUNT)
	{
		return -1;
	}

	if (nScreenIndex>=0 && nScreenIndex<=MAX_SCREEN_COUNT)
	{
		if (m_apScreenInput[nScreenIndex]!=NULL)
		{
			m_apScreenInput[nScreenIndex]->SetScreenWndPos(nWndIndex,x,y,cx,cy);
		}
	}
	return 0;
}

void ImageInputMgr::AddImageInput(ImageInput*pImageInput)
{
	XAutoLock l(m_csImageInput);
	if (m_listImageInput.find(pImageInput)==m_listImageInput.end())
	{
		m_listImageInput.push_back(pImageInput);
	}
}

void ImageInputMgr::RemoveImageInput(ImageInput*pImageInput)
{
	{
		XAutoLock l(m_csImageInput);
		m_listImageInput.remove(pImageInput);
	}

	int nScreenIndex=pImageInput->GetScreenIndex();
	if (nScreenIndex>=0 && nScreenIndex<=MAX_SCREEN_COUNT)
	{
		if (m_apScreenInput[nScreenIndex]!=NULL)
		{
			m_apScreenInput[nScreenIndex]->RemoveImageInput(pImageInput);
		}
	}
}

void ImageInputMgr::OnImageInputScreenIndexChanged(ImageInput*pImageInput,int nOldScreenIndex)
{
	int nNewScreenIndex=pImageInput->GetScreenIndex();
	if (nNewScreenIndex!=nOldScreenIndex)
	{
		if (nOldScreenIndex>=0 && nOldScreenIndex<=MAX_SCREEN_COUNT)
		{
			if (m_apScreenInput[nOldScreenIndex]!=NULL)
			{
				m_apScreenInput[nOldScreenIndex]->RemoveImageInput(pImageInput);
			}
		}
		if (nNewScreenIndex>=0 && nNewScreenIndex<=MAX_SCREEN_COUNT)
		{
			if (m_apScreenInput[nNewScreenIndex]!=NULL)
			{
				m_apScreenInput[nNewScreenIndex]->AddImageInput(pImageInput);
			}
		}
	}
}

void ImageInputMgr::OnImageInputFullScreenChanged(ImageInput*pImageInput)
{
	int nScreenIndex=pImageInput->GetScreenIndex();
	if (nScreenIndex>=0 && nScreenIndex<=MAX_SCREEN_COUNT)
	{
		if (m_apScreenInput[nScreenIndex]!=NULL)
		{
			m_apScreenInput[nScreenIndex]->OnImageInputFullScreenChanged(pImageInput);
		}
	}
}

void ImageInputMgr::OnImageInputWndIndexChanged(ImageInput*pImageInput)
{
	int nScreenIndex=pImageInput->GetScreenIndex();
	if (nScreenIndex>=0 && nScreenIndex<=MAX_SCREEN_COUNT)
	{
		if (m_apScreenInput[nScreenIndex]!=NULL)
		{
			m_apScreenInput[nScreenIndex]->OnImageInputWndIndexChanged(pImageInput);
		}
	}
}

ScreenInput*ImageInputMgr::GetScreenInput(int nScreenIndex)
{
	if (nScreenIndex>=0 && nScreenIndex<=MAX_SCREEN_COUNT)
	{
		return m_apScreenInput[nScreenIndex];
	}
	return NULL;
}


VIDEC_API int VIDEC_SetScreenPos(int nScreenIndex,int x,int y,int cx,int cy)
{
	return ImageInputMgr::Instance().SetScreenPos(nScreenIndex,x,y,cx,cy);
}

VIDEC_API int VIDEC_SetScreenWndCount(int nScreenIndex,int nWndCount)
{
	return ImageInputMgr::Instance().SetScreenWndCount(nScreenIndex,nWndCount);
}

VIDEC_API int VIDEC_SetScreenWndPos(int nScreenIndex,int nWndIndex,int x,int y,int cx,int cy)
{
	return ImageInputMgr::Instance().SetScreenWndPos(nScreenIndex,nWndIndex,x,y,cx,cy);
}
