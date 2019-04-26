#include "StdAfx.h"
#include ".\semitranswnd.h"

/****************************************************************************************************
������CSemiTransWnd::CSemiTransWnd(void)
���ܣ� ��Ĺ��캯��
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
������CSemiTransWnd::~CSemiTransWnd(void)
���ܣ���������
*****************************************************************************************************/

CSemiTransWnd::~CSemiTransWnd(void)
{
	//�Ƚ�CDCѡ���ȥ
	if(m_pOldBmp&&m_pCdc)
		m_pCdc->SelectObject(m_pOldBmp);
	//ɾ��ԭ����λͼ
	if(m_pBmp) 
	{
		delete m_pBmp;
		m_pBmp = NULL;
	}
	
	//ɾ��DC
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
������ bool Create(long width, long height,BYTE alpha)
���ܣ�����λͼװ�أ�ֱ�Ӵ���
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
������bool Load(const char * strPathname,DWORD type)
���ܣ� װ�ش�Alphaͨ����ͼ���ļ�
*****************************************************************************************************/
bool CSemiTransWnd::Load(const char * strPathname,DWORD type)
{
	bool loadok=false;
	//װ���ļ�
	switch(type)
	{
		case TGA_32BITS_WITH_ALPAH: // 32λ��Alphaͨ��TGA�ļ�
				loadok=LoadTgaFile(strPathname,&m_pRGBA,&m_iWidth,&m_iHeight); //װ��Tga�ļ�
				break;
		default:
				break;
	}
	//����λͼ
	
	return loadok;
}

/****************************************************************************************
������void Attach(HWND hWnd)
���ܣ� ����������
*****************************************************************************************/
void CSemiTransWnd::Attach(HWND hWnd)
{
	m_hWnd=hWnd;
	SetWindowLong(hWnd,GWL_EXSTYLE,GetWindowLong(hWnd,GWL_EXSTYLE)|WS_EX_LAYERED);
}

/****************************************************************************************
������ long GetWidth(void)
���ܣ� ���ؿ��
*****************************************************************************************/
long CSemiTransWnd::GetWidth(void)
{
	return m_iWidth;
}

/****************************************************************************************
������ long GetHeight(void)
���ܣ� ���ظ߶�
*****************************************************************************************/

long CSemiTransWnd::GetHeight(void)
{
	return m_iHeight;
}
/****************************************************************************************
������ bool GetHwnd(void)
���ܣ� ��ȡ���ھ��
*****************************************************************************************/
HWND CSemiTransWnd::GetHwnd(void)
{
	return m_hWnd;
}
/****************************************************************************************************
������bool LoadTgaFile(const char * strPathname, DWORD ** pRGBABuffer, long * width, long * height)
���ܣ� װ�ش�Alphaͨ����TGAͼ���ļ�
*****************************************************************************************************/
bool CSemiTransWnd::LoadTgaFile(const char * strPathname, DWORD ** pRGBABuffer, long * width, long * height)
{
	//����TGA���ݽṹ
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
	//����������
	if(!strPathname||!pRGBABuffer) 
		return false;
	//��TGA�ļ�
	FILE* file = fopen( strPathname, "rb" );
	if( NULL == file )	
		return false;
    //��ȡ�ļ�ͷ
	fread( &tga, sizeof(TargaHeader), 1, file );
	// ֻ֧��32λ��ʽ
	if( ( 0 != tga.ColormapType ) ||( tga.ImageType != 10 && tga.ImageType != 2 )||tga.PixelDepth!=32 )
	{
		fclose( file );
		return false;
	}
	// ����ID��
	if( tga.IDLength )
		fseek( file, tga.IDLength, SEEK_CUR );
    //��ʱ����
	DWORD m_dwWidth = tga.ImageWidth;
	DWORD m_dwHeight = tga.ImageHeight;
	DWORD m_dwBPP	 = tga.PixelDepth;
   //�����ڴ�
	DWORD *m_pRGBAData = new DWORD[m_dwWidth*m_dwHeight];
 	if( m_pRGBAData == NULL )
	{
		fclose(file);
		return false;
	}
  //��ȡ���ݣ�Ϊ�˺�DIB��RGBA���ݽṹ���ݣ����������˳������˵��� a b g r ==>  r g b a
	for( DWORD y=0; y<m_dwHeight; ++y)
	{
		DWORD dwOffset = y*m_dwWidth; //����֮������
		if( 0 != ( tga.ImageDescriptor & 0x0010 ) )
			dwOffset = (m_dwHeight-y-1)*m_dwWidth; //������������
		for( DWORD x=0; x<m_dwWidth;)
		{
			if( tga.ImageType == 10 ) //ѹ����ʽ
			{
				BYTE PacketInfo = getc( file );
				WORD PacketType = 0x80 & PacketInfo;
				WORD PixelCount = ( 0x007f & PacketInfo ) + 1;
				if( PacketType ) //ѹ������
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
			else //��ѹ����ʽ
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
	//�ر��ļ�
	fclose( file );
	//���
	
	if(*pRGBABuffer) //ԭ����λͼ���� ɾ��
		delete (*pRGBABuffer);
	*pRGBABuffer = m_pRGBAData;
	*width = m_dwWidth;
	*height = m_dwHeight;
	//���سɹ�
	return true;	
}

/****************************************************************************************
������ CDC * GetDC(void)
���ܣ�//�����û���ͼ��DC
*****************************************************************************************/
CDC * CSemiTransWnd::GetDC(void)
{
	if(m_iWidth&&m_iHeight&&m_pRGBA) //ָ����ȷ
	{
		//�Ƚ�CDCѡ���ȥ
		if(m_pOldBmp&&m_pCdc)
		{
			m_pCdc->SelectObject(m_pOldBmp);
		}
		//ɾ��ԭ����λͼ
		if(m_pBmp) 
		{
			delete m_pBmp;
			m_pBmp=NULL;
		}
		//ɾ��DC
		if(m_pCdc)
		{
			delete m_pCdc;
			m_pCdc=NULL;
		}
		CDC dcScreen;
		dcScreen.Attach(::GetDC(NULL)); //��ȡ��Ļ�豸������
		//�����û�DC
		m_pCdc=new CDC();
		if(m_pCdc)
		{
			m_pCdc->CreateCompatibleDC(&dcScreen);
			//����λͼ
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
			//ѡ��Object
			m_pOldBmp=m_pCdc->SelectObject(m_pBmp);
			BITMAP bmpInfo;
			m_pBmp->GetBitmap(&bmpInfo);
			memcpy((BYTE *)bmpInfo.bmBits,(BYTE *)m_pRGBA,m_iWidth*m_iHeight*4); //���Ʊ���λͼ
		}
		return m_pCdc; //����pDC
	}
	return NULL;
}
/****************************************************************************************
������ void Update(void)
���ܣ� ����Alphaͨ�� �����û����Ʋ��ֵĲ�͸����!
*****************************************************************************************/

void CSemiTransWnd::DealAlphaChannel(void)
{
	if(m_pRGBA&&m_pBmp) //ָ����ȷ
		if(m_bSemiTransAlphaUpdate||m_bTransAlphaUpdate)//��Ҫ����Alpha
		{
			//��ȡλͼ����
			BITMAP bmpInfo;
			m_pBmp->GetBitmap(&bmpInfo);
			BYTE * rgba=(BYTE *)bmpInfo.bmBits;
			BYTE * rgbb=(BYTE *)m_pRGBA;		
			long i;
			for(i=0;i<bmpInfo.bmHeight*bmpInfo.bmWidth;++i)
    		{
				if(rgba[0]!=rgbb[0]||rgba[1]!=rgbb[1]||rgba[2]!=rgbb[2]) //��ɫ��ͬ
					if(rgbb[3]==0x00&&m_bTransAlphaUpdate||  //͸��Alphaֵ����
						rgbb[3]&&m_bSemiTransAlphaUpdate) //��͸�� Alphaֵ����
					{
						rgba[3]=0xFF;
					}
				rgba+=4;
				rgbb+=4;
			}//end for
		}//end if
}

/****************************************************************************************
������ void Update(long type,POINT *pTopLeft)
���ܣ����û����Ƶ�����ˢ�µ���Ļ��
*****************************************************************************************/
void CSemiTransWnd::Update(long type,POINT *pTopLeft)
{
	if(m_hWnd&&m_iWidth&&m_iHeight&&m_pRGBA&&m_pCdc&&m_pBmp)//ָ����ȷ
	{
		//��ȡ��ĻDC
		CDC dcScreen;
		dcScreen.Attach(::GetDC(NULL));
		//����Alpha ͨ��
		DealAlphaChannel();
		CRect rectDlg;
		GetWindowRect(m_hWnd,&rectDlg);
		CPoint ptWindowScreenPosition(rectDlg.TopLeft());
		if(pTopLeft)
		{
			ptWindowScreenPosition.x=pTopLeft->x;
			ptWindowScreenPosition.y=pTopLeft->y;
		}
        //ˢ�µ���Ļ��
		CSize szWindow(m_iWidth, m_iHeight);
		CPoint ptSrc(0,0); 
		int alpha;
		switch(type)
		{
		case 2: //����
			for(alpha=m_iWndSemiTransAlpha;alpha>=0;--alpha)
			{
				BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, alpha,AC_SRC_ALPHA };
				UpdateLayeredWindow(m_hWnd, dcScreen, &ptWindowScreenPosition, &szWindow, *m_pCdc,&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
			}
			break;
		case 1: //����
			for(alpha=0;alpha<=m_iWndSemiTransAlpha;++alpha)
			{
				BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, alpha,AC_SRC_ALPHA };
				UpdateLayeredWindow(m_hWnd, dcScreen, &ptWindowScreenPosition, &szWindow, *m_pCdc,&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
			}
			break;
		case 0: //ֱ��ˢ��
		default:
			BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, m_iWndSemiTransAlpha,AC_SRC_ALPHA };
			UpdateLayeredWindow(m_hWnd, dcScreen, &ptWindowScreenPosition, &szWindow, *m_pCdc,&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
			break;
		}
	}//end if
}


/****************************************************************************************
������ bool GetAlphaChannel(BYTE * pAlpha)
���ܣ� ��ȡAlphaͨ������
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
������ bool GetRGBA(DWORD * pRGBA)
���ܣ� ��ȡRGBAͨ������
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
������ bool SetAlphaChannel(BYTE * pAlpha,long mode)
���ܣ�����Alphaͨ��
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
				rgba[3]=(*pAlpha); //ֻ�ı䵱ǰ��
				if(mode==1) //����ԭʼ��AlphaҲ�ı�
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
������ bool SetRGBA(long  width, long height, DWORD * pRGBA)
���ܣ� ����RGBA����
*****************************************************************************************/
bool CSemiTransWnd::SetRGBA(long  width, long height, DWORD * pRGBA)
{
	if(pRGBA)
	{
		//���
		m_iWidth=width;
		//�߶�
		m_iHeight=height;
		//ɾ��ԭ����
		if(m_pRGBA)
			delete m_pRGBA;
		m_pRGBA=pRGBA; //ָ��
		return true;
	}
	return false;
}

void CSemiTransWnd::DestroyTransWnd()
{
	//�Ƚ�CDCѡ���ȥ
	if(m_pOldBmp&&m_pCdc)
		m_pCdc->SelectObject(m_pOldBmp);
	//ɾ��ԭ����λͼ
	if(m_pBmp) 
	{
		delete m_pBmp;
		m_pBmp = NULL;
	}
	//ɾ��DC
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
