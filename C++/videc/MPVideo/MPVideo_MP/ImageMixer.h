#ifndef __IMAGEMIXER_H__
#define __IMAGEMIXER_H__

class ImageMixer
{
public:
	ImageMixer(void);
	virtual~ImageMixer(void);
public:
	virtual void SetImageSize(int nImageWidth,int nImageHeight);

	virtual unsigned int GetImageWidth(void);
	virtual unsigned int GetImageHeight(void);
	virtual unsigned int GetImageSize(void);
	virtual unsigned char*GetImageData(void);

	virtual bool MixImage(unsigned char*pImageData,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int x,unsigned int y,unsigned int cx,unsigned int cy);
	virtual bool FillRect(unsigned int x,unsigned int y,unsigned int cx,unsigned int cy);
	virtual bool DrawRect(unsigned int x,unsigned int y,unsigned int cx,unsigned int cy);
protected:	
	virtual bool MixImage(unsigned char*pToImageData, int nToWidth, int nToHeight, unsigned char*pImageData,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int x,unsigned int y,unsigned int cx,unsigned int cy);
	virtual void FillRect(unsigned char*pImageBuffer,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int nDstLeft,unsigned int nDstTop,unsigned int nDstWidth,unsigned int nDstHeight, unsigned char ucFilledValue);		
	virtual void DrawRect(unsigned char*pImageBuffer,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int nDstLeft,unsigned int nDstTop,unsigned int nDstWidth,unsigned int nDstHeight, unsigned char ucFilledValue);		
protected:
	unsigned char*		m_pImageBuffer;
	unsigned int		m_nImageSize;
	unsigned int		m_nImageWidth;
	unsigned int		m_nImageHeight;
	bool				m_bVDResample;
};

void ScaleLine(unsigned char*pSrcBuffer,unsigned char*pDstBuffer,unsigned int nSrcWidth,unsigned int nDstWidth);
void ScaleRect(unsigned char*pSrcBuffer,unsigned int nSrcWidth,unsigned int nSrcHeight,unsigned char*pImageBuffer,unsigned int nImageWidth,unsigned int nImageHeight,unsigned int nDstLeft,unsigned int nDstTop,unsigned int nDstWidth,unsigned int nDstHeight);
void CopyImageRect(int nSrcWidth,int nSrcHeight,int nPointBytes,unsigned char*pSrc,int nFromLeft,int nFromTop,int nDstWidth,int nDstHeight,unsigned char*pDst);

#endif