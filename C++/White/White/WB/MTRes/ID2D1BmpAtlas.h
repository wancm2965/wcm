/* * * * * * * * * * * * * * * * * * *
*���ƣ�D2Dͼ����	                 *
*���ܣ��Ż�D2Dͼ��洢��ʽ			 *
*���ڣ�2014��1��13��                 *
*�汾��2.0(��ʽ��)                   *
* * * * * * * * * * * * * * * * * * */
#pragma once
//D2Dͼ���ڷ����ڴ�ʱ�����һ����С�����ڴ棬������СͼƬ������
//�ڴ汻�˷ѣ��ʲ���ͼ��������С�����˷ѡ�
//Ӧ�ó�����������ͬ���Сͼ����Ҫ������ʱ������Ӧ�ô���������
#include <Windows.h>
#include <d2d1helper.h>
#ifndef _SAFERELEASE
#define _SAFERELEASE
template<class Interface>//��ȫ�ͷ��೧����
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
//��Ƭ������128*128��64K������ֱ����λͼ
class ID2D1BmpAtlas//D2Dͼ����
{
public:
	static ID2D1BmpAtlas* CreateBmpAtlas(UINT cx, UINT cy, ID2D1RenderTarget *pRenderTarget);
public:
	void Release();
	D2D_RECT_U GetBitmap(HID2DBMP hD2dbmp);
	HID2DBMP AddBitmap(ID2D1Bitmap* pBitmap);
	HID2DBMP AddBitmap(UINT cx, UINT cy, BYTE* pDate, UINT WidthBytes = 0);
	HID2DBMP AddBitmap(LPCTSTR *fileStr);
	D2D_SIZE_U GetBmpSize(HID2DBMP hD2dbmp = 0);//��ȡ������Ƭ�ߴ�,���㷵�ر�׼�ߴ�
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
	ID2D1Bitmap **m_pBitmap;//ͼ����ַ
	D2D_SIZE_U m_BmpSize;//ͼ����񣨵�����Ƭ���ߴ磩
	D2D_SIZE_U m_AlbumSize;//����С�����Ż����ĳߴ磩
	UINT m_uCurPos;//��ǰ����λ��
	UINT m_uAlbumVolume;//ͼ������������
	UINT m_uAlbumPages;//ͼ����ҳ��
	D2D_SIZE_U **m_pBmpSize;//��Ƭ�ߴ�
	ID2D1RenderTarget *m_pRenderTarget;
};
