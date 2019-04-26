
class VideoPlay
{

};

class VideoRender
{

};


class VideoImagePool;
class VideoImage
{
public:
	VideoImage(VideoImagePool&rVideoImagePool);
	virtual~VideoImage(void);
public:
	virtual void AddRef(void);
	virtual void Release(void);
protected:
	VideoImagePool&	m_rVideoImagePool;
};

class VideoImagePool
{
public:
	VideoImagePool(int nVideoImageCount);
	virtual~VideoImagePool(void);
public:
	virtual VideoImage*GetFreeVideoImage(int nWidth,int nHeight);
	virtual void SetFreeVideoImage(VideoImage*pVideoImage);
protected:
		m_csListVideoImage;
		m_listFreeVideoImage;
		m_listUsedVideoImage;
	int	m_nVideoImageCount;
};

class VideoConsumerCallback
{
public:
	virtual void OnVideoConsumerCallbackPull(unsigned char**ppData,int&nLen,int&nWidth,int&nHeight,int&nFrameRate)=0;
};

class VideoConsumer
{
public:
	virtual int Open(long nHandle)=0;
	virtual void Close(void)=0;
	virtual void SetWndPos(int x,int y,int cx,int cy)=0;
	virtual void Repaint(void)=0;
};