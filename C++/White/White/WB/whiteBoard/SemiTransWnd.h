/****************************************************************************************************
                                 
								 ��͸��������  CSemiTransWnd 
  
1. �����;

     ��������,��ActiveSkin�ؼ��Ľ���Ч������,��������һ�����Ĺ���ԭ��,�������ϲ���Щ����,д�������.

	 �����ֻ����Win2000���ϵĻ�����ʹ�ã�


2. ���뻷��

     Visual Studio 2003 V7.1.3091 ,��VC6��ʹ����Ҫ��Щ����.

    �����õ���Win2000�Ժ���еĺ���,����Stdafx.h��ͷ������к궨��,�����������! 

    #define WINVER        0x0500
    #define _WIN32_WINNT  0x0500

*****************************************************************************************************/

#pragma once

#define  TGA_32BITS_WITH_ALPAH   0x01   //32λ��Alphaͨ��TGA�ļ�


class CSemiTransWnd
{
public:
	bool SetRGBA(long  width, long height, DWORD * pRGBA); //����RGBA����
	bool GetRGBA(DWORD * pRGBA);	//��ȡRGBA����
	bool GetAlphaChannel(BYTE * pAlpha); //��ȡAlphaͨ��
	bool SetAlphaChannel(BYTE * pAlpha,long mode=0); //����Alphaͨ��
	void Update(long type=0,POINT * pTopLeft=NULL); //ˢ�´���
	CDC * GetDC(void); //��ȡ��ͼDC
	long GetWidth(void); //���
	long GetHeight(void); //�߶�
	HWND GetHwnd(void);//��ȡ���ھ��
	void Attach(HWND hWnd); //����������
	bool Load(const char * strPathname,DWORD type=TGA_32BITS_WITH_ALPAH); //װ���ļ�
	bool Create(long width, long height,BYTE alpha=0x00); //����
	void DestroyTransWnd();
	CSemiTransWnd(void); //����
	~CSemiTransWnd(void);//����

public:
 	BYTE        m_iWndSemiTransAlpha;      //���������͸��Alphaֵ
    bool        m_bSemiTransAlphaUpdate;   //��͸��Alphaֵ����
	bool        m_bTransAlphaUpdate;       //ȫ͸��Alphaֵ����

protected:
	HWND		m_hWnd;     //Ҫ���а�͸��Alphaͨ����ʾ�Ĵ��ھ��
	long		m_iWidth;   //��ܿ��
	long		m_iHeight;  //��ܸ߶�
	DWORD   *   m_pRGBA;    //ָ����RGBA���ݵ�ָ��
	CBitmap *   m_pBmp;     //�豸�޹�λͼָ��
	CBitmap *   m_pOldBmp;  //��λͼָ��
	CDC    *    m_pCdc;     //DCָ��

protected:
	bool LoadTgaFile(const char * strPathname, DWORD ** pRGBABuffer, long * width, long * height); //װ��TGA�ļ�
	void DealAlphaChannel(void); //����Alphaͨ��

};
