#include "StdAfx.h"
#include ".\semitranswnd.h"

/****************************************************************************************************
函数：CSemiTransWnd::CSemiTransWnd(void)
功能： 类的构造函数
*****************************************************************************************************/
CSemiTransWnd::CSemiTransWnd(void)
{
	m_hWnd=NULL;
	m_iWidth=0;
	m_iHeight=0;
	m_pRGBA=NULL;
	m_pBmp=NULL;
	m_pCdc=NULL;
	m_pOldBmp=NULL;
	m_bSemiTransAlphaUpdate=TRUE;
	m_bTransAlphaUpdate=TRUE;
	m_iWndSemiTransAlpha=0xFF;
}
/****************************************************************************************************
函数：CSemiTransWnd::~CSemiTransWnd(void)
功能：析构函数
*****************************************************************************************************/

CSemiTransWnd::~CSemiTransWnd(void)
{
	//先将CDC选择回去
	if(m_pOldBmp&&m_pCdc)
		m_pCdc->SelectObject(m_pOldBmp);
	//删除原来的位图
	if(m_pBmp) 
	{
		delete m_pBmp;
		m_pBmp = NULL;
	}
	
	//删除DC
	if(m_pCdc)
	{
		delete m_pCdc;
		m_pCdc = NULL;
	}
	if(m_pRGBA)
	{
		delete m_pRGBA;
		m_pRGBA = NULL;
	}
}

/****************************************************************************************
函数： bool Create(long width, long height,BYTE alpha)
功能：不从位图装载，直接创建
*****************************************************************************************/
bool CSemiTransWnd::Create(long width, long height,BYTE alpha)
{
	if(width&&height)
	{
		DWORD * rgba=new DWORD[width*height];
		if(rgba)
		{
			long i,j;
			for(i=0;i<height;++i)
				for(j=0;j<width;++j)
				{
					rgba[i*width+j]=alpha<<24;
				}
			if(m_pRGBA)
				delete m_pRGBA;
			m_pRGBA=(DWORD *)rgba;
			m_iWidth=width;
			m_iHeight=height;
			return true;
		}
	}
	return false;
}

/****************************************************************************************************
函数：bool Load(const char * strPathname,DWORD type)
功能： 装载带Alpha通道的图像文件
*****************************************************************************************************/
bool CSemiTransWnd::Load(const char * strPathname,DWORD type)
{
	bool loadok=false;
	//装载文件
	switch(type)
	{
		case TGA_32BITS_WITH_ALPAH: // 32位带Alpha通道TGA文件
				loadok=LoadTgaFile(strPathname,&m_pRGBA,&m_iWidth,&m_iHeight); //装载Tga文件
				break;
		default:
				break;
	}
	//创建位图
	
	return loadok;
}

/****************************************************************************************
函数：void Attach(HWND hWnd)
功能： 关联到窗口
*****************************************************************************************/
void CSemiTransWnd::Attach(HWND hWnd)
{
	m_hWnd=hWnd;
	SetWindowLong(hWnd,GWL_EXSTYLE,GetWindowLong(hWnd,GWL_EXSTYLE)|WS_EX_LAYERED);
}

/****************************************************************************************
函数： long GetWidth(void)
功能： 返回宽度
*****************************************************************************************/
long CSemiTransWnd::GetWidth(void)
{
	return m_iWidth;
}

/****************************************************************************************
函数： long GetHeight(void)
功能： 返回高度
*****************************************************************************************/

long CSemiTransWnd::GetHeight(void)
{
	return m_iHeight;
}
/****************************************************************************************
函数： bool GetHwnd(void)
功能： 获取窗口句柄
*****************************************************************************************/
HWND CSemiTransWnd::GetHwnd(void)
{
	return m_hWnd;
}
/****************************************************************************************************
函数：bool LoadTgaFile(const char * strPathname, DWORD ** pRGBABuffer, long * width, long * height)
功能： 装载带Alpha通道的TGA图像文件
*****************************************************************************************************/
bool CSemiTransWnd::LoadTgaFile(const char * strPathname, DWORD ** pRGBABuffer, long * width, long * height)
{
	//定义TGA数据结构
	struct TargaHeader
	{
	BYTE IDLength;
	BYTE ColormapType;
	BYTE ImageType;
	BYTE ColormapSpecification[5];
	WORD XOrigin;
	WORD YOrigin;
	WORD ImageWidth;
	WORD ImageHeight;
	BYTE PixelDepth;
	BYTE ImageDescriptor;
	} tga;
	DWORD r,g,b,a;
	//检查输入参数
	if(!strPathname||!pRGBABuffer) 
		return false;
	//打开TGA文件
	FILE* file = fopen( strPathname, "rb" );
	if( NULL == file )	
		return false;
    //读取文件头
	fread( &tga, sizeof(TargaHeader), 1, file );
	// 只支持32位格式
	if( ( 0 != tga.ColormapType ) ||( tga.ImageType != 10 && tga.ImageType != 2 )||tga.PixelDepth!=32 )
	{
		fclose( file );
		return false;
	}
	// 跳过ID段
	if( tga.IDLength )
		fseek( file, tga.IDLength, SEEK_CUR );
    //临时变量
	DWORD m_dwWidth = tga.ImageWidth;
	DWORD m_dwHeight = tga.ImageHeight;
	DWORD m_dwBPP	 = tga.PixelDepth;
   //分配内存
	DWORD *m_pRGBAData = new DWORD[m_dwWidth*m_dwHeight];
 	if( m_pRGBAData == NULL )
	{
		fclose(file);
		return false;
	}
  //读取数据，为了和DIB的RGBA数据结构兼容，这里对数据顺序进行了调整 a b g r ==>  r g b a
	for( DWORD y=0; y<m_dwHeight; ++y)
	{
		DWORD dwOffset = y*m_dwWidth; //从上之下正序
		if( 0 != ( tga.ImageDescriptor & 0x0010 ) )
			dwOffset = (m_dwHeight-y-1)*m_dwWidth; //从下至上逆序
		for( DWORD x=0; x<m_dwWidth;)
		{
			if( tga.ImageType == 10 ) //压缩格式
			{
				BYTE PacketInfo = getc( file );
				WORD PacketType = 0x80 & PacketInfo;
				WORD PixelCount = ( 0x007f & PacketInfo ) + 1;
				if( PacketType ) //压缩类型
				{
					r = getc( file );
					g = getc( file );
					b = getc( file );
					a = getc( file );
					while( PixelCount-- )
					{
						m_pRGBAData[dwOffset+x] = (a<<24L)+(b<<16L)+(g<<8L)+(r);
						++x;
					}//end while
				}//end if
				else
				{
					while( PixelCount-- )
					{
						r = getc( file );
						g = getc( file );
						b = getc( file );
						a = getc( file );
						m_pRGBAData[dwOffset+x] = (a<<24L)+(b<<16L)+(g<<8L)+(r);
						++x;
					}//end while
				}//end else
			}//end if
			else //非压缩格式
			{
				r = getc( file );
				g = getc( file );
				b = getc( file );
				a = getc( file );
				m_pRGBAData[dwOffset+x] = (a<<24L)+(b<<16L)+(g<<8L)+(r);
				++x;
			} //end else
		} //end for x
	}//end for y 
	//关闭文件
	fclose( file );
	//输出
	
	if(*pRGBABuffer) //原来有位图数据 删除
		delete (*pRGBABuffer);
	*pRGBABuffer = m_pRGBAData;
	*width = m_dwWidth;
	*height = m_dwHeight;
	//返回成功
	return true;	
}

/****************************************************************************************
函数： CDC * GetDC(void)
功能：//返回用户作图的DC
*****************************************************************************************/
CDC * CSemiTransWnd::GetDC(void)
{
	if(m_iWidth&&m_iHeight&&m_pRGBA) //指针正确
	{
		//先将CDC选择回去
		if(m_pOldBmp&&m_pCdc)
		{
			m_pCdc->SelectObject(m_pOldBmp);
		}
		//删除原来的位图
		if(m_pBmp) 
		{
			delete m_pBmp;
			m_pBmp=NULL;
		}
		//删除DC
		if(m_pCdc)
		{
			delete m_pCdc;
			m_pCdc=NULL;
		}
		CDC dcScreen;
		dcScreen.Attach(::GetDC(NULL)); //获取屏幕设备上下文
		//创建用户DC
		m_pCdc=new CDC();
		if(m_pCdc)
		{
			m_pCdc->CreateCompatibleDC(&dcScreen);
			//创建位图
			m_pBmp=new CBitmap();
			HBITMAP hNewBmp;
			BITMAPINFO bmi;
			LPBYTE pBits;
			memset(&bmi,0,sizeof(BITMAPINFO));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = m_iWidth;
			bmi.bmiHeader.biHeight = m_iHeight;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32; // four 8-bit components
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = m_iWidth * m_iHeight * 4;
			// create our DIB section and select the bitmap into the dc
			hNewBmp = CreateDIBSection(dcScreen, &bmi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);
			m_pBmp->Attach(hNewBmp);
			//选择Object
			m_pOldBmp=m_pCdc->SelectObject(m_pBmp);
			BITMAP bmpInfo;
			m_pBmp->GetBitmap(&bmpInfo);
			memcpy((BYTE *)bmpInfo.bmBits,(BYTE *)m_pRGBA,m_iWidth*m_iHeight*4); //复制背景位图
		}
		return m_pCdc; //返回pDC
	}
	return NULL;
}
/****************************************************************************************
函数： void Update(void)
功能： 处理Alpha通道 处理用户绘制部分的不透明度!
*****************************************************************************************/

void CSemiTransWnd::DealAlphaChannel(void)
{
	if(m_pRGBA&&m_pBmp) //指针正确
		if(m_bSemiTransAlphaUpdate||m_bTransAlphaUpdate)//需要处理Alpha
		{
			//获取位图数据
			BITMAP bmpInfo;
			m_pBmp->GetBitmap(&bmpInfo);
			BYTE * rgba=(BYTE *)bmpInfo.bmBits;
			BYTE * rgbb=(BYTE *)m_pRGBA;		
			long i;
			for(i=0;i<bmpInfo.bmHeight*bmpInfo.bmWidth;++i)
    		{
				if(rgba[0]!=rgbb[0]||rgba[1]!=rgbb[1]||rgba[2]!=rgbb[2]) //颜色不同
					if(rgbb[3]==0x00&&m_bTransAlphaUpdate||  //透明Alpha值更新
						rgbb[3]&&m_bSemiTransAlphaUpdate) //半透明 Alpha值更新
					{
						rgba[3]=0xFF;
					}
				rgba+=4;
				rgbb+=4;
			}//end for
		}//end if
}

/****************************************************************************************
函数： void Update(long type,POINT *pTopLeft)
功能：将用户绘制的内容刷新到屏幕上
*****************************************************************************************/
void CSemiTransWnd::Update(long type,POINT *pTopLeft)
{
	if(m_hWnd&&m_iWidth&&m_iHeight&&m_pRGBA&&m_pCdc&&m_pBmp)//指针正确
	{
		//获取屏幕DC
		CDC dcScreen;
		dcScreen.Attach(::GetDC(NULL));
		//处理Alpha 通道
		DealAlphaChannel();
		CRect rectDlg;
		GetWindowRect(m_hWnd,&rectDlg);
		CPoint ptWindowScreenPosition(rectDlg.TopLeft());
		if(pTopLeft)
		{
			ptWindowScreenPosition.x=pTopLeft->x;
			ptWindowScreenPosition.y=pTopLeft->y;
		}
        //刷新到屏幕上
		CSize szWindow(m_iWidth, m_iHeight);
		CPoint ptSrc(0,0); 
		int alpha;
		switch(type)
		{
		case 2: //淡出
			for(alpha=m_iWndSemiTransAlpha;alpha>=0;--alpha)
			{
				BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, alpha,AC_SRC_ALPHA };
				UpdateLayeredWindow(m_hWnd, dcScreen, &ptWindowScreenPosition, &szWindow, *m_pCdc,&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
			}
			break;
		case 1: //淡入
			for(alpha=0;alpha<=m_iWndSemiTransAlpha;++alpha)
			{
				BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, alpha,AC_SRC_ALPHA };
				UpdateLayeredWindow(m_hWnd, dcScreen, &ptWindowScreenPosition, &szWindow, *m_pCdc,&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
			}
			break;
		case 0: //直接刷新
		default:
			BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, m_iWndSemiTransAlpha,AC_SRC_ALPHA };
			UpdateLayeredWindow(m_hWnd, dcScreen, &ptWindowScreenPosition, &szWindow, *m_pCdc,&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
			break;
		}
	}//end if
}


/****************************************************************************************
函数： bool GetAlphaChannel(BYTE * pAlpha)
功能： 获取Alpha通道数据
*****************************************************************************************/
bool CSemiTransWnd::GetAlphaChannel(BYTE * pAlpha)
{
	if(pAlpha&&m_pBmp)
	{
		long i,j;
		BITMAP bmp;
		m_pBmp->GetBitmap(&bmp);
		BYTE * rgba=(BYTE *)bmp.bmBits;
		BYTE * alpha=pAlpha;
		for(i=0;i<m_iHeight;++i)
			for(j=0;j<m_iWidth;++j)
			{
				*alpha=rgba[3];
				++alpha;
				rgba+=4;
			}
		return true;
	}
	return false;
}
/****************************************************************************************
函数： bool GetRGBA(DWORD * pRGBA)
功能： 获取RGBA通道数据
*****************************************************************************************/
bool CSemiTransWnd::GetRGBA(DWORD * pRGBA)
{
	if(pRGBA&&m_pRGBA)
	{
		memcpy(pRGBA,m_pRGBA,m_iWidth*m_iHeight);
		return true;
	}
	return false;
}


/****************************************************************************************
函数： bool SetAlphaChannel(BYTE * pAlpha,long mode)
功能：设置Alpha通道
*****************************************************************************************/
bool CSemiTransWnd::SetAlphaChannel(BYTE * pAlpha,long mode)
{
	if(pAlpha&&m_pBmp)
	{
		BITMAP bmpInfo;
		m_pBmp->GetBitmap(&bmpInfo);
		BYTE * rgba=(BYTE *)bmpInfo.bmBits;
		BYTE * rgbb=(BYTE *)m_pRGBA;				
		long i,j;
		for(i=0;i<m_iHeight;++i)
			for(j=0;j<m_iWidth;++j)
			{
				rgba[3]=(*pAlpha); //只改变当前的
				if(mode==1) //将最原始的Alpha也改变
					rgbb[3]=(*pAlpha);
				++pAlpha;
				rgba+=4;
				rgbb+=4;
     		}//end for
		return true;
	}//end if
	return false;
}
/****************************************************************************************
函数： bool SetRGBA(long  width, long height, DWORD * pRGBA)
功能： 设置RGBA数据
*****************************************************************************************/
bool CSemiTransWnd::SetRGBA(long  width, long height, DWORD * pRGBA)
{
	if(pRGBA)
	{
		//宽度
		m_iWidth=width;
		//高度
		m_iHeight=height;
		//删除原来的
		if(m_pRGBA)
			delete m_pRGBA;
		m_pRGBA=pRGBA; //指针
		return true;
	}
	return false;
}

void CSemiTransWnd::DestroyTransWnd()
{
	//先将CDC选择回去
	if(m_pOldBmp&&m_pCdc)
		m_pCdc->SelectObject(m_pOldBmp);
	//删除原来的位图
	if(m_pBmp) 
	{
		delete m_pBmp;
		m_pBmp = NULL;
	}
	//删除DC
	if(m_pCdc)
	{
		delete m_pCdc;
		m_pCdc = NULL;
	}
	if(m_pRGBA)
	{
		delete m_pRGBA;
		m_pRGBA = NULL;
	}
}
