
#include "VIDEC_MP4_Util.h"


unsigned long	g_ulMaxTime	= 7200;
unsigned long	g_ulMaxSize = 2*1024*1024*1024;
int				g_nAudioFillInFrameLen = 6;
unsigned char	g_ucAudioFillInFrame[4098]={0x00 ,0xc8 ,0x00 ,0x80 ,0x23 ,0x80};

unsigned long MP4_GetTimestamp(void)
{
#ifdef WIN32
	return ::timeGetTime();//∫¡√Î
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec/1000; 
#endif
}

void DebugOutput(const char* fmt,...)
{
	va_list args;
	va_start(args, fmt);

	char buffer1[2048]={0};
#ifdef WIN32
	_vsnprintf(buffer1,sizeof(buffer1),fmt,args);
#else
	vsnprintf(buffer1,sizeof(buffer1),fmt,args);
#endif
#ifdef WIN32
	OutputDebugStringA(buffer1);
#endif
	va_end(args);
}

bool GetKeyNail(unsigned char* data,unsigned int usize,unsigned char* & pNail,unsigned int& uNailSize)
{
	int i = 0;
	while (i +4 < usize)
	{
		if (data[i] == 0x00 && data[i+1] == 0x00 && data[i+2] == 0x01 && (data[i+3]&0x1f) == 5)
		{
			pNail = data + i+3;
			uNailSize = usize -i- 3;
			return false;
		}
		i++;
	}
	pNail = data;
	uNailSize = usize;
	return false;
}


bool GetOneSlider(unsigned char* data,unsigned int usize,unsigned int& uOffSet,unsigned int& uSliderSize)
{
	if (usize < 4)
	{
		return false;
	}
	int i = uOffSet;
	while(i+4< usize)
	{
		if (data[i] == 0x00 && data[i+1] == 0x00 )
		{
			if (data[i+2] == 0x01)
			{
				uOffSet = i;
				uSliderSize = 3;
				return true;
			}
			if (data[i+2] == 0x00 && data[i+3] == 0x01)
			{
				uOffSet = i;
				uSliderSize = 4;
				return true;
			} 
		}
		i++;
	}
	return false;
}
bool GetOneNail(unsigned char* data,unsigned int usize,unsigned char* & pNail,unsigned int& uNailSize,unsigned int& uOffset)
{
	if (uOffset>= usize)
	{

	}

	int i=0;
	unsigned int uOffset1		= uOffset;
	unsigned int uSliderSize1	= 0;
	bool bFindSlider1		= GetOneSlider(data,usize,uOffset,uSliderSize1);
	if (!bFindSlider1)
	{
		pNail	  = data;
		uNailSize = usize;
		return false;
	}

	uOffset1 = uOffset;
	uOffset = uOffset1 + uSliderSize1;

	unsigned int uOffset2		= 0;
	unsigned int uSliderSize2	= 0;
	bool bFindSlider2		= GetOneSlider(data,usize,uOffset,uSliderSize2);
	uOffset2 = uOffset;
	if (!bFindSlider2)
	{
		pNail		= data + uOffset1 + uSliderSize1;
		uNailSize = usize - uOffset1- uSliderSize1 ;
		return false;
	}
	pNail		= data + uOffset1 + uSliderSize1;
	uNailSize   = uOffset2 - uOffset1- uSliderSize1;
	return true;
}
