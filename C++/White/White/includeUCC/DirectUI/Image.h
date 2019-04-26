#if !defined(_IMAGE_H_INCLUDED_)
#define _IMAGE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include "FileEx.h"

#define HIMETRIC_INCH   2540    // HIMETRIC units per inch

enum IMAGE_TYPE
{
	FT_BMP,
	FT_ICO,
	FT_CUR,
	FT_ANICUR,
	FT_JPG,
	FT_GIF,
	FT_TGA,
	FT_PNG,

	FT_UNKNOWN
};

class DIRECTUI_API CImage 
{
public:
	CImage();
	virtual ~CImage();

public:
	virtual int GetWidth(){return m_nWidth;}
	virtual int GetHeight(){return m_nHeight;}

	//draw
	virtual BOOL Draw( HDC hDC, int x, int y, LPRECT sr );
	virtual BOOL Draw( HDC hDC, int x, int y, LPRECT sr, COLORREF colTrans, BOOL bTrans );
	virtual BOOL Draw( HDC hDC, LPRECT r);
	virtual BOOL Draw( HDC hDC, int x, int y, COLORREF colTrans, BOOL bTrans );

	virtual BOOL StretchDraw(HDC hDC, LPRECT r,COLORREF crTransparent);
	virtual BOOL StretchDraw(HDC hDC, LPRECT tr, LPRECT sr,COLORREF crTransparent );

	//load image
	static  CImage* LoadImage(LPCTSTR szImagePath);
	static  CImage* LoadImage(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst = NULL); 

protected:
	BOOL		GetResource(LPCTSTR lpName, LPCTSTR lpType, HMODULE hInst, void* pResource, int& nBufSize);
	static IMAGE_TYPE	GetFileType(LPCTSTR szImagePath);

protected:
	int m_nWidth,m_nHeight;
	BOOL m_bLoadsucceed;

};

class DIRECTUI_API CBitmapImage : public CImage 
{
public:
	CBitmapImage();
	virtual ~CBitmapImage();
	void Attach(HBITMAP hBitmap){m_hBitmap = hBitmap;}
	void Detach(){m_hBitmap = NULL;}

public:
	BOOL LoadImage(LPCTSTR szImagePath);
	BOOL LoadImage(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst = NULL); 

	virtual BOOL Draw( HDC hDC, LPRECT r);
	virtual BOOL Draw( HDC hDC, int x, int y, COLORREF colTrans, BOOL bTrans );
	virtual BOOL Draw( HDC hDC, int x, int y, LPRECT sr );
	virtual BOOL Draw( HDC hDC, int x, int y, LPRECT sr, COLORREF colTrans, BOOL bTrans );

	virtual BOOL StretchDraw(HDC hDC, LPRECT r,COLORREF crTransparent);
	virtual BOOL StretchDraw(HDC hDC, LPRECT tr, LPRECT sr,COLORREF crTransparent );

	HRGN CreateRgnFromFile( COLORREF color );

public:
	HBITMAP m_hBitmap;
};

// Definitions for image types.
#define TGA_Null 0
#define TGA_Map 1
#define TGA_RGB 2
#define TGA_Mono 3
#define TGA_RLEMap 9
#define TGA_RLERGB 10
#define TGA_RLEMono 11
#define TGA_CompMap 32
#define TGA_CompMap4 33

#define	CIMAGE_MAX_MEMORY 256000000
#define CIMAGE_ERR_NOFILE _T("null file handler")
#define CIMAGE_ERR_NOIMAGE _T("null image!!!")

#define CXIMAGE_SUPPORT_BMP 1
#define CIMAGE_SUPPORT_TRANSFORMATION 1
#define CIMAGE_SUPPORT_BASICTRANSFORMATIONS 1
#define CIMAGE_SUPPORT_WINDOWS 1  
#define CIMAGE_SUPPORT_ALPHA    1
#define CIMAGE_SUPPORT_SELECTION 1
#define CIMAGE_SUPPORT_DECODE	1
#define CIMAGE_SUPPORT_TGA 1
#define CIMAGE_SUPPORT_PNG 1
#define CIMAGE_SUPPORT_JPG 1
#define CIMAGE_SUPPORT_INTERPOLATION 1

enum ENUM_CXIMAGE_FORMATS{
	CXIMAGE_FORMAT_UNKNOWN,
	CXIMAGE_FORMAT_TGA,
	CXIMAGE_FORMAT_PNG,
	CXIMAGE_FORMAT_JPG,
	CMAX_IMAGE_FORMATS
};

struct rgb_color { BYTE r,g,b; };

#ifndef PI
#define PI 3.141592653589793f
#endif

class DIRECTUI_API CTgaImage : public CImage
{
#pragma pack(1)
	typedef struct tagTgaHeader
	{
		BYTE   IdLength;            // Image ID Field Length
		BYTE   CmapType;            // Color Map Type
		BYTE   ImageType;           // Image Type

		WORD   CmapIndex;           // First Entry Index
		WORD   CmapLength;          // Color Map Length
		BYTE   CmapEntrySize;       // Color Map Entry Size

		WORD   X_Origin;            // X-origin of Image
		WORD   Y_Origin;            // Y-origin of Image
		WORD   ImageWidth;          // Image Width
		WORD   ImageHeight;         // Image Height
		BYTE   PixelDepth;          // Pixel Depth
		BYTE   ImagDesc;            // Image Descriptor
	} TGAHEADER;

	typedef struct tagRGBA{
		BYTE    rgbRed;
		BYTE    rgbGreen;
		BYTE    rgbBlue;
		BYTE    rgbReserved;
	} RGBA;
#pragma pack()

	//extensible information collector
	typedef struct tagCxImageInfo {
		DWORD	dwEffWidth;			//DWORD aligned scan line width
		BYTE*	pImage;				//THE IMAGE BITS
		CTgaImage* pGhost;			//if this is a ghost, pGhost points to the body
		CTgaImage* pParent;			//if this is a layer, pParent points to the body
		DWORD	dwType;				//original image format
		char	szLastError[256];	//debugging
		long	nProgress;			//monitor
		long	nEscape;			//escape
		long	nBkgndIndex;		//used for GIF, PNG, MNG
		RGBQUAD nBkgndColor;		//used for RGB transparency
		BYTE	nQuality;			//used for JPEG
		BYTE	nScale;				//used for JPEG <ignacio>
		long	nFrame;				//used for TIF, GIF, MNG : actual frame
		long	nNumFrames;			//used for TIF, GIF, MNG : total number of frames
		DWORD	dwFrameDelay;		//used for GIF, MNG
		long	xDPI;				//horizontal resolution
		long	yDPI;				//vertical resolution
		RECT	rSelectionBox;		//bounding rectangle
		BYTE	nAlphaMax;			//max opacity (fade)
		bool	bAlphaPaletteEnabled; //true if alpha values in the palette are enabled.
		bool	bEnabled;			//enables the painting functions
		long	xOffset;
		long	yOffset;
		DWORD	dwCodecOption;		//for GIF, TIF : 0=def.1=unc,2=fax3,3=fax4,4=pack,5=jpg
		RGBQUAD last_c;				//for GetNearestIndex optimization
		BYTE	last_c_index;
		bool	last_c_isvalid;
		long	nNumLayers;
		DWORD	dwFlags;			// 0x??00000 = reserved, 0x00??0000 = blend mode, 0x0000???? = layer id - user flags

	} CXIMAGEINFO;

	//interpolation methods:
	enum InterpolationMethod {
		IM_NEAREST_NEIGHBOUR=1,
		IM_BILINEAR		=2,
		IM_BSPLINE		=3,
		IM_BICUBIC		=4,
		IM_BICUBIC2		=5,
		IM_LANCZOS		=6,
		IM_BOX			=7,
		IM_HERMITE		=8,
		IM_HAMMING		=9,
		IM_SINC			=10,
		IM_BLACKMAN		=11,
		IM_BESSEL		=12,
		IM_GAUSSIAN		=13,
		IM_QUADRATIC	=14,
		IM_MITCHELL		=15,
		IM_CATROM		=16
	};

	enum OverflowMethod {
		OM_COLOR=1,
		OM_BACKGROUND=2,
		OM_TRANSPARENT=3,
		OM_WRAP=4,
		OM_REPEAT=5,
		OM_MIRROR=6
	};

public:
	CTgaImage();
	CTgaImage(const CTgaImage &src, bool copypixels = true, bool copyselection = true, bool copyalpha = true);
	CTgaImage(DWORD dwWidth, DWORD dwHeight, DWORD wBpp, DWORD imagetype = 0);
	virtual ~CTgaImage();

public:
	long	Draw(HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1, RECT* pClipRect = 0, bool bSmooth = false);
	long	Draw(HDC hdc, const RECT& rect, RECT* pClipRect=NULL, bool bSmooth = false);
	long	Stretch(HDC hdc, long xoffset, long yoffset, long xsize, long ysize, DWORD dwRop = SRCCOPY);
	long	Stretch(HDC hdc, const RECT& rect, DWORD dwRop = SRCCOPY);
	long	Tile(HDC hdc, RECT *rc);
	long	Draw2(HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1);
	long	Draw2(HDC hdc, const RECT& rect);

	virtual BOOL Draw( HDC hDC, int x, int y, LPRECT sr);
	virtual BOOL Draw( HDC hDC, int x, int y, LPRECT sr, COLORREF colTrans, BOOL bTrans );
	virtual BOOL StretchDraw(HDC hDC, LPRECT tr, LPRECT sr,COLORREF crTransparent );

public:
	BOOL LoadImage(LPCTSTR szImagePath);
	BOOL LoadImage(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst = NULL); 
	virtual int  GetWidth(){return head.biWidth;}
	virtual int  GetHeight(){return head.biHeight;}

	BYTE*	GetBits(DWORD row = 0);
	bool	IsInside(long x, long y);
	DWORD	GetEffWidth()	const {return info.dwEffWidth;}
	DWORD	GetNumColors()	const {return head.biClrUsed;}
	WORD	GetBpp()		const {return head.biBitCount;}
	DWORD	GetType()		const {return info.dwType;}
	char*	GetLastError()	{return info.szLastError;}

	static float KernelBSpline(const float x);
	static float KernelLinear(const float t);
	static float KernelCubic(const float t);
	static float KernelGeneralizedCubic(const float t, const float a=-1);
	static float KernelLanczosSinc(const float t, const float r = 3);
	static float KernelBox(const float x);
	static float KernelHermite(const float x);
	static float KernelHamming(const float x);
	static float KernelSinc(const float x);
	static float KernelBlackman(const float x);
	static float KernelBessel_J1(const float x);
	static float KernelBessel_P1(const float x);
	static float KernelBessel_Q1(const float x);
	static float KernelBessel_Order1(float x);
	static float KernelBessel(const float x);
	static float KernelGaussian(const float x);
	static float KernelQuadratic(const float x);
	static float KernelMitchell(const float x);
	static float KernelCatrom(const float x);

protected:
	bool		Destroy();
	void*		Create(DWORD dwWidth, DWORD dwHeight, DWORD wBpp, DWORD imagetype);
	bool		Decode(CFileEx * hFile);
	bool		Decode(CFileEx *hFile, DWORD imagetype);
	bool		Decode(FILE *hFile) { CIOFile file(hFile); return Decode(&file); }
	BYTE		ExpandCompressedLine(BYTE* pDest,TGAHEADER* ptgaHead,CFileEx *hFile,int width, int y, BYTE rleLeftover);
	void		ExpandUncompressedLine(BYTE* pDest,TGAHEADER* ptgaHead,CFileEx *hFile,int width, int y, int xoffset);
	long		GetSize();
	DWORD		GetPaletteSize();
	RGBQUAD*	GetPalette() const;
	void		SetPaletteColor(BYTE idx, COLORREF cr);
	void		SetPaletteColor(BYTE idx, BYTE r, BYTE g, BYTE b, BYTE alpha=0);
	void		SetPaletteColor(BYTE idx, RGBQUAD c);
	bool		IsValid() const {return pDib!=0;}
	void		SetGrayPalette();
	bool		Mirror();
	void		Startup(DWORD imagetype);
	void		Copy(const CTgaImage &src, bool copypixels, bool copyselection, bool copyalpha);
	void		SetYDPI(long dpi);
	void		SetXDPI(long dpi);
	void		SetPalette(DWORD n, BYTE *r, BYTE *g, BYTE *b);
	void		SetPalette(RGBQUAD* pPal,DWORD nColors=256);
	void		SetPalette(rgb_color *rgb,DWORD nColors=256);
	void		SetPixelIndex(long x,long y,BYTE i);
	BYTE		GetPixelIndex(long x,long y);
	bool		Transfer(CTgaImage& from);
	RGBQUAD		GetTransColor();
	RGBQUAD		GetPaletteColor(BYTE idx);
	long		GetTransIndex()	const {return info.nBkgndIndex;}
	void		CopyInfo(const CTgaImage &src);
	bool		IsGrayScale();
	RGBQUAD		GetPixelColor(long x,long y, bool bGetAlpha = true);
	void		SetPixelColor(long x,long y,RGBQUAD c, bool bSetAlpha = false);
	void		SetPixelColor(long x,long y,COLORREF cr);

	void RGBtoBGR(BYTE *buffer, int length);
	COLORREF	RGBQUADtoRGB (RGBQUAD c);
	RGBQUAD		RGBtoRGBQUAD(COLORREF cr);

	BYTE		GetNearestIndex(RGBQUAD c);
	DWORD		GetCodecOption() const {return info.dwCodecOption;}
	void		SetCodecOption(DWORD opt) {info.dwCodecOption = opt;}
	bool		SelectionDelete();
	void		OverflowCoordinates(float &x, float &y, OverflowMethod const ofMethod);
	void		OverflowCoordinates(long  &x, long &y, OverflowMethod const ofMethod);
	RGBQUAD		GetPixelColorWithOverflow(long x, long y, OverflowMethod const ofMethod=OM_BACKGROUND, RGBQUAD* const rplColor=0);

	RGBQUAD		BlindGetPixelColor(const long x,const long y);
	BYTE		BlindGetPixelIndex(const long x,const long y);

	RGBQUAD		GetPixelColorInterpolated(float x,float y, InterpolationMethod const inMethod=IM_BILINEAR, OverflowMethod const ofMethod=OM_BACKGROUND, RGBQUAD* const rplColor=0);

	BYTE*		AlphaGetPointer(const long x,const long y);

	bool		IsIndexed() const;

	void*		BlindGetPixelPointer(const long x, const long y);

	//	RGBQUAD GetAreaColorInterpolated(float const xc, float const yc, float const w, float const h, InterpolationMethod const inMethod, OverflowMethod const ofMethod=OM_BACKGROUND, RGBQUAD* const rplColor=0);

	BYTE	GetJpegQuality() const;
	void	SetJpegQuality(BYTE q);

	BYTE	GetJpegScale() const;
	void	SetJpegScale(BYTE q);

#if CIMAGE_SUPPORT_ALPHA
	//Alpha
	void AlphaClear();
	void AlphaCreate();
	void AlphaDelete();
	void AlphaInvert();
	bool AlphaMirror();
	bool AlphaFlip();
	bool AlphaCopy(CTgaImage &from);
	bool AlphaSplit(CTgaImage *dest);
	void AlphaStrip();
	void AlphaSet(BYTE level);
	bool AlphaSet(CTgaImage &from);
	void AlphaSet(long x,long y,BYTE level);
	BYTE AlphaGet(long x,long y);
	BYTE AlphaGetMax() const {return info.nAlphaMax;}
	void AlphaSetMax(BYTE nAlphaMax) {info.nAlphaMax=nAlphaMax;}
	bool AlphaIsValid(){return pAlpha!=0;}
	BYTE* AlphaGetBits() const {return pAlpha;}

	void AlphaPaletteClear();
	void AlphaPaletteEnable(bool enable=true){info.bAlphaPaletteEnabled=enable;}
	bool AlphaPaletteIsEnabled(){return info.bAlphaPaletteEnabled;}
	bool AlphaPaletteIsValid();
	bool AlphaPaletteSplit(CTgaImage *dest);
#endif //CIMAGE_SUPPORT_ALPHA

protected:
	void*				pDib; //contains the header, the palette, the pixels
	CXIMAGEINFO			info; //extended information
	BITMAPINFOHEADER    head; //standard header
	CTgaImage**			pLayers; //generic layers
	BYTE*				pSelection;	//selected region
	BYTE*				pAlpha; //alpha channel

};

#include "png\png.h"

class DIRECTUI_API CPngImage: public CTgaImage 
{
public:
	CPngImage();

	bool Decode(CFileEx * hFile);
	bool Decode(CFileEx *hFile, DWORD imagetype){return CTgaImage::Decode(hFile,imagetype);}
	bool Decode(FILE *hFile) { CIOFile file(hFile); return Decode(&file); }

	BOOL LoadImage(LPCTSTR szImagePath);
	BOOL LoadImage(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst = NULL);

	//	virtual BOOL Draw( HDC hDC, int x, int y, LPRECT sr);
	//	virtual BOOL Draw( HDC hDC, int x, int y, LPRECT sr, COLORREF colTrans, BOOL bTrans );
	//	virtual BOOL StretchDraw(HDC hDC, LPRECT tr, LPRECT sr,COLORREF crTransparent );

protected:
	void ima_png_error(png_struct *png_ptr, char *message);
	void expand2to4bpp(BYTE* prow);

	static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		CFileEx* hFile = (CFileEx*)png_ptr->io_ptr;
		if (hFile->Read(data,1,length) != length) png_error(png_ptr, "Read Error");
	}

	static void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		CFileEx* hFile = (CFileEx*)png_ptr->io_ptr;
		if (hFile->Write(data,1,length) != length) png_error(png_ptr, "Write Error");
	}

	static void user_flush_data(png_structp png_ptr)
	{
		CFileEx* hFile = (CFileEx*)png_ptr->io_ptr;
		if (!hFile->Flush()) png_error(png_ptr, "Flush Error");
	}
	static void user_error_fn(png_structp png_ptr,png_const_charp error_msg)
	{
		strncpy_s((char*)png_ptr->error_ptr,255,error_msg,255);
		longjmp(png_ptr->jmpbuf, 1);
	}
};


//jpg_start////////////////////////////////////////////////////////////////////////

//#if CXIMAGE_SUPPORT_JPG

//#define CXIMAGEJPG_SUPPORT_EXIF 1

extern "C" {
#include "jpeg/jpeglib.h"
#include "jpeg/jerror.h"
}

class DIRECTUI_API CJpgImage: public CTgaImage
{
public:
	CJpgImage();
	~CJpgImage();

	//	bool Load(const char * imageFileName){ return CxImage::Load(imageFileName,CXIMAGE_FORMAT_JPG);}
	//	bool Save(const char * imageFileName){ return CxImage::Save(imageFileName,CXIMAGE_FORMAT_JPG);}
	bool Decode(CFileEx * hFile);
	bool Decode(CFileEx *hFile, DWORD imagetype){return CTgaImage::Decode(hFile,imagetype);}
	bool Decode(FILE *hFile) { CIOFile file(hFile); return Decode(&file); }

	BOOL LoadImage(LPCTSTR szImagePath);
	BOOL LoadImage(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst = NULL);


#if CXIMAGE_SUPPORT_ENCODE
	bool Encode(CFileEx * hFile);
	bool Encode(FILE *hFile) { CxIOFile file(hFile); return Encode(&file); }
#endif // CXIMAGE_SUPPORT_ENCODE

	/*
	* EXIF support based on jhead-1.8 by Matthias Wandel <mwandel(at)rim(dot)net>
	*/

#if CXIMAGEJPG_SUPPORT_EXIF

#define MAX_COMMENT 1000
#define MAX_SECTIONS 20

	typedef struct tag_ExifInfo {
		char  Version      [5];
		char  CameraMake   [32];
		char  CameraModel  [40];
		char  DateTime     [20];
		int   Height, Width;
		int   Orientation;
		int   IsColor;
		int   Process;
		int   FlashUsed;
		float FocalLength;
		float ExposureTime;
		float ApertureFNumber;
		float Distance;
		float CCDWidth;
		float ExposureBias;
		int   Whitebalance;
		int   MeteringMode;
		int   ExposureProgram;
		int   ISOequivalent;
		int   CompressionLevel;
		float FocalplaneXRes;
		float FocalplaneYRes;
		float FocalplaneUnits;
		float Xresolution;
		float Yresolution;
		float ResolutionUnit;
		float Brightness;
		char  Comments[MAX_COMMENT];

		unsigned char * ThumbnailPointer;  /* Pointer at the thumbnail */
		unsigned ThumbnailSize;     /* Size of thumbnail. */

		bool  IsExif;
	} EXIFINFO;

	//--------------------------------------------------------------------------
	// JPEG markers consist of one or more 0xFF bytes, followed by a marker
	// code byte (which is not an FF).  Here are the marker codes of interest
	// in this program.  (See jdmarker.c for a more complete list.)
	//--------------------------------------------------------------------------

#define M_SOF0  0xC0            // Start Of Frame N
#define M_SOF1  0xC1            // N indicates which compression process
#define M_SOF2  0xC2            // Only SOF0-SOF2 are now in common use
#define M_SOF3  0xC3
#define M_SOF5  0xC5            // NB: codes C4 and CC are NOT SOF markers
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8            // Start Of Image (beginning of datastream)
#define M_EOI   0xD9            // End Of Image (end of datastream)
#define M_SOS   0xDA            // Start Of Scan (begins compressed data)
#define M_JFIF  0xE0            // Jfif marker
#define M_EXIF  0xE1            // Exif marker
#define M_COM   0xFE            // COMment 

#define PSEUDO_IMAGE_MARKER 0x123; // Extra value.

#define EXIF_READ_EXIF  0x01
#define EXIF_READ_IMAGE 0x02
#define EXIF_READ_ALL   0x03

	class DIRECTUI_API CxExifInfo
	{

		typedef struct tag_Section_t{
			BYTE*    Data;
			int      Type;
			unsigned Size;
		} Section_t;

	public:
		EXIFINFO* m_exifinfo;
		char m_szLastError[256];
		CxExifInfo(EXIFINFO* info = NULL);
		~CxExifInfo();
		bool DecodeExif(CFileEx * hFile, int nReadMode = EXIF_READ_EXIF);
		bool EncodeExif(CFileEx * hFile);
		void DiscardAllButExif();
	protected:
		bool process_EXIF(unsigned char * CharBuf, unsigned int length);
		void process_COM (const BYTE * Data, int length);
		void process_SOFn (const BYTE * Data, int marker);
		int Get16u(void * Short);
		int Get16m(void * Short);
		long Get32s(void * Long);
		unsigned long Get32u(void * Long);
		double ConvertAnyFormat(void * ValuePtr, int Format);
		void* FindSection(int SectionType);
		bool ProcessExifDir(unsigned char * DirStart, unsigned char * OffsetBase, unsigned ExifLength,
			EXIFINFO * const pInfo, unsigned char ** const LastExifRefdP);
		int ExifImageWidth;
		int MotorolaOrder;
		Section_t Sections[MAX_SECTIONS];
		int SectionsRead;
		bool freeinfo;
	};

	CxExifInfo* m_exif;
	EXIFINFO m_exifinfo;
	bool DecodeExif(CFileEx * hFile);
	bool DecodeExif(FILE * hFile) { CxIOFile file(hFile); return DecodeExif(&file); }

#endif //CXIMAGEJPG_SUPPORT_EXIF

	////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////        C x F i l e J p g         ////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	// thanks to Chris Shearer Cooper <cscooper(at)frii(dot)com>
	class CxFileJpg : public jpeg_destination_mgr, public jpeg_source_mgr
	{
	public:
		enum { eBufSize = 4096 };

		CxFileJpg(CFileEx* pFile)
		{
			m_pFile = pFile;

			init_destination = InitDestination;
			empty_output_buffer = EmptyOutputBuffer;
			term_destination = TermDestination;

			init_source = InitSource;
			fill_input_buffer = FillInputBuffer;
			skip_input_data = SkipInputData;
			resync_to_restart = jpeg_resync_to_restart; // use default method
			term_source = TermSource;
			next_input_byte = NULL; //* => next byte to read from buffer 
			bytes_in_buffer = 0;	//* # of bytes remaining in buffer 

			m_pBuffer = new unsigned char[eBufSize];
		}
		~CxFileJpg()
		{
			delete [] m_pBuffer;
		}

		static void InitDestination(j_compress_ptr cinfo)
		{
			CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;
			pDest->next_output_byte = pDest->m_pBuffer;
			pDest->free_in_buffer = eBufSize;
		}

		static boolean EmptyOutputBuffer(j_compress_ptr cinfo)
		{
			CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;
			if (pDest->m_pFile->Write(pDest->m_pBuffer,1,eBufSize)!=(size_t)eBufSize)
				ERREXIT(cinfo, JERR_FILE_WRITE);
			pDest->next_output_byte = pDest->m_pBuffer;
			pDest->free_in_buffer = eBufSize;
			return TRUE;
		}

		static void TermDestination(j_compress_ptr cinfo)
		{
			CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;
			size_t datacount = eBufSize - pDest->free_in_buffer;
			/* Write any data remaining in the buffer */
			if (datacount > 0) {
				if (!pDest->m_pFile->Write(pDest->m_pBuffer,1,datacount))
					ERREXIT(cinfo, JERR_FILE_WRITE);
			}
			pDest->m_pFile->Flush();
			/* Make sure we wrote the output file OK */
			if (pDest->m_pFile->Error()) ERREXIT(cinfo, JERR_FILE_WRITE);
			return;
		}

		static void InitSource(j_decompress_ptr cinfo)
		{
			CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
			pSource->m_bStartOfFile = TRUE;
		}

		static boolean FillInputBuffer(j_decompress_ptr cinfo)
		{
			size_t nbytes;
			CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
			nbytes = pSource->m_pFile->Read(pSource->m_pBuffer,1,eBufSize);
			if (nbytes <= 0){
				if (pSource->m_bStartOfFile)	//* Treat empty input file as fatal error 
					ERREXIT(cinfo, JERR_INPUT_EMPTY);
				WARNMS(cinfo, JWRN_JPEG_EOF);
				// Insert a fake EOI marker 
				pSource->m_pBuffer[0] = (JOCTET) 0xFF;
				pSource->m_pBuffer[1] = (JOCTET) JPEG_EOI;
				nbytes = 2;
			}
			pSource->next_input_byte = pSource->m_pBuffer;
			pSource->bytes_in_buffer = nbytes;
			pSource->m_bStartOfFile = FALSE;
			return TRUE;
		}

		static void SkipInputData(j_decompress_ptr cinfo, long num_bytes)
		{
			CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
			if (num_bytes > 0){
				while (num_bytes > (long)pSource->bytes_in_buffer){
					num_bytes -= (long)pSource->bytes_in_buffer;
					FillInputBuffer(cinfo);
					// note we assume that fill_input_buffer will never return FALSE,
					// so suspension need not be handled.
				}
				pSource->next_input_byte += (size_t) num_bytes;
				pSource->bytes_in_buffer -= (size_t) num_bytes;
			}
		}
		
		static void TermSource(j_decompress_ptr cinfo)
		{
			return;
		}
	protected:
		CFileEx  *m_pFile;
		unsigned char *m_pBuffer;
		bool m_bStartOfFile;
	};

public:
	enum CODEC_OPTION
	{
		ENCODE_BASELINE = 0x1,
		ENCODE_ARITHMETIC = 0x2,
		ENCODE_GRAYSCALE = 0x4,
		ENCODE_OPTIMIZE = 0x8,
		ENCODE_PROGRESSIVE = 0x10,
		ENCODE_LOSSLESS = 0x20,
		ENCODE_SMOOTHING = 0x40,
		DECODE_GRAYSCALE = 0x80,
		DECODE_QUANTIZE = 0x100,
		DECODE_DITHER = 0x200,
		DECODE_ONEPASS = 0x400,
		DECODE_NOSMOOTH = 0x800
	}; 
	
	int m_nPredictor;
	int m_nPointTransform;
	int m_nSmoothing;
	int m_nQuantize;
	J_DITHER_MODE m_nDither;
};

//#endif //CXIMAGE_SUPPORT_JPG

//jpg_end////////////////////////////////////////////////////////////////////////



class DIRECTUI_API CSkinImageIterator
{
	friend class CTgaImage;
protected:
	int Itx, Ity;		// Counters
	int Stepx, Stepy;
	BYTE* IterImage;	//  Image pointer
	CTgaImage *ima;
public:
	// Constructors
	CSkinImageIterator ( void );
	CSkinImageIterator ( CTgaImage *image );
	operator CTgaImage* ();

	// Iterators
	BOOL ItOK ();
	void Reset ();
	void Upset ();
	void SetRow(BYTE *buf, int n);
	void GetRow(BYTE *buf, int n);
	BYTE GetByte( ) { return IterImage[Itx]; }
	void SetByte(BYTE b) { IterImage[Itx] = b; }
	BYTE* GetRow(void);
	BYTE* GetRow(int n);
	BOOL NextRow();
	BOOL PrevRow();
	BOOL NextByte();
	BOOL PrevByte();

	void SetSteps(int x, int y=0) {  Stepx = x; Stepy = y; }
	void GetSteps(int *x, int *y) {  *x = Stepx; *y = Stepy; }
	BOOL NextStep();
	BOOL PrevStep();

	void SetY(int y);	/* AD - for interlace */
	int  GetY() {return Ity;}
	BOOL GetCol(BYTE* pCol, DWORD x);
	BOOL SetCol(BYTE* pCol, DWORD x);
};

/////////////////////////////////////////////////////////////////////
inline
CSkinImageIterator::CSkinImageIterator(void)
{
	ima = 0;
	IterImage = 0;
	Itx = Ity = 0;
	Stepx = Stepy = 0;
}
/////////////////////////////////////////////////////////////////////
inline
CSkinImageIterator::CSkinImageIterator(CTgaImage *imageImpl): ima(imageImpl)
{
	if (ima) IterImage = ima->GetBits();
	Itx = Ity = 0;
	Stepx = Stepy = 0;
}
/////////////////////////////////////////////////////////////////////
inline
CSkinImageIterator::operator CTgaImage* ()
{
	return ima;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CSkinImageIterator::ItOK ()
{
	if (ima) return ima->IsInside(Itx, Ity);
	else	 return FALSE;
}
/////////////////////////////////////////////////////////////////////
inline void CSkinImageIterator::Reset()
{
	if (ima) IterImage = ima->GetBits();
	else	 IterImage=0;
	Itx = Ity = 0;
}
/////////////////////////////////////////////////////////////////////
inline void CSkinImageIterator::Upset()
{
	Itx = 0;
	Ity = ima->GetHeight()-1;
	IterImage = ima->GetBits() + ima->GetEffWidth()*(ima->GetHeight()-1);
}
/////////////////////////////////////////////////////////////////////
inline BOOL CSkinImageIterator::NextRow()
{
	if (++Ity >= (int)ima->GetHeight()) return 0;
	IterImage += ima->GetEffWidth();
	return 1;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CSkinImageIterator::PrevRow()
{
	if (--Ity < 0) return 0;
	IterImage -= ima->GetEffWidth();
	return 1;
}
/* AD - for interlace */
inline void CSkinImageIterator::SetY(int y)
{
	if ((y < 0) || (y > (int)ima->GetHeight())) return;
	Ity = y;
	IterImage = ima->GetBits() + ima->GetEffWidth()*y;
}
/////////////////////////////////////////////////////////////////////
inline void CSkinImageIterator::SetRow(BYTE *buf, int n)
{
	if (n<0) n = (int)ima->GetEffWidth();
	else n = min(n,(int)ima->GetEffWidth());

	if (IterImage) memcpy(IterImage,buf,n);
}
/////////////////////////////////////////////////////////////////////
inline void CSkinImageIterator::GetRow(BYTE *buf, int n)
{
	if ((buf!=NULL)&&(n>0)) memcpy(buf,IterImage,n);
}
/////////////////////////////////////////////////////////////////////
inline BYTE* CSkinImageIterator::GetRow()
{
	return IterImage;
}
/////////////////////////////////////////////////////////////////////
inline BYTE* CSkinImageIterator::GetRow(int n)
{
	SetY(n);
	return IterImage;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CSkinImageIterator::NextByte()
{
	if (++Itx < (int)ima->GetEffWidth()) return 1;
	else
		if (++Ity < (int)ima->GetHeight()){
			IterImage += ima->GetEffWidth();
			Itx = 0;
			return 1;
		} else
			return 0;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CSkinImageIterator::PrevByte()
{
	if (--Itx >= 0) return 1;
	else
		if (--Ity >= 0){
			IterImage -= ima->GetEffWidth();
			Itx = 0;
			return 1;
		} else
			return 0;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CSkinImageIterator::NextStep()
{
	Itx += Stepx;
	if (Itx < (int)ima->GetEffWidth()) return 1;
	else {
		Ity += Stepy;
		if (Ity < (int)ima->GetHeight()){
			IterImage += ima->GetEffWidth();
			Itx = 0;
			return 1;
		} else
			return 0;
	}
}
/////////////////////////////////////////////////////////////////////
inline BOOL CSkinImageIterator::PrevStep()
{
	Itx -= Stepx;
	if (Itx >= 0) return 1;
	else {       
		Ity -= Stepy;
		if (Ity >= 0 && Ity < (int)ima->GetHeight()) {
			IterImage -= ima->GetEffWidth();
			Itx = 0;
			return 1;
		} else
			return 0;
	}
}
/////////////////////////////////////////////////////////////////////
inline BOOL CSkinImageIterator::GetCol(BYTE* pCol, DWORD x)
{
	if ((pCol==0)||(ima->GetBpp()<8)||(x >= (DWORD)ima->GetWidth()))
		return 0;

	DWORD h = ima->GetHeight();
	DWORD line = ima->GetEffWidth();
	BYTE bytes = ima->GetBpp()>>3;
	BYTE* pSrc;
	for (DWORD y=0;y<h;y++){
		pSrc = ima->GetBits(y) + x*bytes;
		for (BYTE w=0;w<bytes;w++){
			*pCol++=*pSrc++;
		}
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CSkinImageIterator::SetCol(BYTE* pCol, DWORD x)
{
	if ((pCol==0)||(ima->GetBpp()<8)||(x >= (DWORD)ima->GetWidth()))
		return 0;
	DWORD h = ima->GetHeight();
	DWORD line = ima->GetEffWidth();
	BYTE bytes = ima->GetBpp()>>3;
	BYTE* pSrc;
	for (DWORD y=0;y<h;y++){
		pSrc = ima->GetBits(y) + x*bytes;
		for (BYTE w=0;w<bytes;w++){
			*pSrc++=*pCol++;
		}
	}
	return 1;
}


#endif //_IMAGE_H_INCLUDED_


