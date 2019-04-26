/****************************************************************************************************
                                 
								 半透明窗口类  CSemiTransWnd 
  
1. 类的用途

     闲来无事,见ActiveSkin控件的界面效果不错,就逆向了一下它的工作原理,后在网上查了些资料,写了这个类.

	 这个类只能在Win2000以上的环境下使用！


2. 编译环境

     Visual Studio 2003 V7.1.3091 ,在VC6下使用需要作些处理.

    由于用到了Win2000以后才有的函数,需在Stdafx.h开头添加两行宏定义,否则编译会出错! 

    #define WINVER        0x0500
    #define _WIN32_WINNT  0x0500

*****************************************************************************************************/

#pragma once

#define  TGA_32BITS_WITH_ALPAH   0x01   //32位带Alpha通道TGA文件


class CSemiTransWnd
{
public:
	bool SetRGBA(long  width, long height, DWORD * pRGBA); //设置RGBA数据
	bool GetRGBA(DWORD * pRGBA);	//获取RGBA数据
	bool GetAlphaChannel(BYTE * pAlpha); //获取Alpha通道
	bool SetAlphaChannel(BYTE * pAlpha,long mode=0); //设置Alpha通道
	void Update(long type=0,POINT * pTopLeft=NULL); //刷新窗口
	CDC * GetDC(void); //获取作图DC
	long GetWidth(void); //宽度
	long GetHeight(void); //高度
	HWND GetHwnd(void);//获取窗口句柄
	void Attach(HWND hWnd); //关联到窗体
	bool Load(const char * strPathname,DWORD type=TGA_32BITS_WITH_ALPAH); //装载文件
	bool Create(long width, long height,BYTE alpha=0x00); //创建
	void DestroyTransWnd();
	CSemiTransWnd(void); //构造
	~CSemiTransWnd(void);//析构

public:
 	BYTE        m_iWndSemiTransAlpha;      //窗体整体半透明Alpha值
    bool        m_bSemiTransAlphaUpdate;   //半透明Alpha值更新
	bool        m_bTransAlphaUpdate;       //全透明Alpha值更新

protected:
	HWND		m_hWnd;     //要进行半透明Alpha通道显示的窗口句柄
	long		m_iWidth;   //框架宽度
	long		m_iHeight;  //框架高度
	DWORD   *   m_pRGBA;    //指向框架RGBA数据的指针
	CBitmap *   m_pBmp;     //设备无关位图指针
	CBitmap *   m_pOldBmp;  //旧位图指针
	CDC    *    m_pCdc;     //DC指针

protected:
	bool LoadTgaFile(const char * strPathname, DWORD ** pRGBABuffer, long * width, long * height); //装载TGA文件
	void DealAlphaChannel(void); //处理Alpha通道

};
