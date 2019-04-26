/* * * * * * * * * * * * * * * * * * *
*名称：D2D图集类	                 *
*功能：优化D2D图像存储方式			 *
*日期：2014年1月13日                 *
*版本：2.0(正式版)                   *
* * * * * * * * * * * * * * * * * * */
#pragma once
//D2D图像在分配内存时会存在一个最小分配内存，大量的小图片将导致
//内存被浪费，故采用图集技术减小这种浪费。
//应用场景：当大量同规格小图标需要被创建时，可以应用此类来管理。
#include <Windows.h>
#include <d2d1helper.h>
#ifndef _SAFERELEASE
#define _SAFERELEASE
template<class Interface>//安全释放类厂对象
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}
#endif

typedef UINT  HID2DBMP;
//相片规格大于128*128（64K）可以直接用位图
class ID2D1BmpAtlas//D2D图集类
{
public:
	static ID2D1BmpAtlas* CreateBmpAtlas(UINT cx, UINT cy, ID2D1RenderTarget *pRenderTarget);
public:
	void Release();
	D2D_RECT_U GetBitmap(HID2DBMP hD2dbmp);
	HID2DBMP AddBitmap(ID2D1Bitmap* pBitmap);
	HID2DBMP AddBitmap(UINT cx, UINT cy, BYTE* pDate, UINT WidthBytes = 0);
	HID2DBMP AddBitmap(LPCTSTR *fileStr);
	D2D_SIZE_U GetBmpSize(HID2DBMP hD2dbmp = 0);//获取单张相片尺寸,传零返回标准尺寸
	D2D_SIZE_U GetAlbumSize();

	void FreeBmpsFromBack(UINT Num = 1);
	void ClearAll( bool bClearMem = false );

	void DrawBitmap( ID2D1RenderTarget *pRenderTarget,HID2DBMP hD2dbmp,const D2D1_RECT_F &destRt,FLOAT opacity = 1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,const D2D1_RECT_F *sourceRectangle = NULL );
private:
	ID2D1BmpAtlas(UINT cx, UINT cy, ID2D1RenderTarget* pRenderTarget);
	virtual ~ID2D1BmpAtlas();
	
	D2D_RECT_U GetBmpRect(HID2DBMP hD2dbmp);
	UINT GetPage(HID2DBMP hD2dbmp);
	bool AddPage();
private:
	ID2D1Bitmap **m_pBitmap;//图集地址
	D2D_SIZE_U m_BmpSize;//图集规格（单张相片最大尺寸）
	D2D_SIZE_U m_AlbumSize;//相册大小（整张画布的尺寸）
	UINT m_uCurPos;//当前插入位置
	UINT m_uAlbumVolume;//图集单张总容量
	UINT m_uAlbumPages;//图集总页数
	D2D_SIZE_U **m_pBmpSize;//相片尺寸
	ID2D1RenderTarget *m_pRenderTarget;
};
