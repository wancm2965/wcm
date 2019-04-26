#ifndef __AVCONDOC_H__
#define __AVCONDOC_H__

#ifdef AVCONDOC_EXPORTS
#define AVCONDOC_API __declspec(dllexport)
#else
#define AVCONDOC_API __declspec(dllimport)
#endif

#include <string>

//LPARAM
enum {
	AVDOCS_INSTALL_DRIVER=0,	//正在安装驱动
	AVDOCS_TOTAL_PAGES,			//总共多少页
	AVDOCS_CURRENT_PAGE,		//当前转换了多少页
	AVDOCS_DONE,				//转换完成
};

class IAVDocConvertor
{
public:
	IAVDocConvertor(void){};
	virtual~IAVDocConvertor(void){};
public:
	virtual void ReleaseConnections(void)=0;
};

/*
功能：
	将文档转换成Image文件，该函数阻塞，转换完后才返回
输入：
	strDocFile：被转换的文档(.doc,.ppt,.xls,.pdf)
	strImgFile：转换的文件，包括路径和文件名
	hWnd：接收消息的窗口句柄（可选）
	nUserMsg：接收消息的ID号（可选）
输出：
	无
返回：
	转换成功返回true,否则返回false
*/
AVCONDOC_API bool AVDocConvert(const std::string&strDocFile,const std::string&strImgFile,HWND hWnd,UINT nUserMsg);
//AVCONDOC_API IAVDocConvertor*CreateIAVDocConvertor(const std::string&strDocFile,const std::string&strImgFile,HWND hWnd,UINT nUserMsg);

class IAVImageFile
{
public:
	IAVImageFile(void){};
	virtual~IAVImageFile(void){};
public:
	/*
	功能：
		打开Image文件
	输入：
		strImgFile：转换的文件，包括路径和文件名
	输出：
		无
	返回：
		转换成功返回true,否则返回false
	*/
	virtual bool Open(const char*cszImgFile)=0;
	/*
	功能：
		关闭Image文件
	输入：
		无
	输出：
		无
	返回：
		无
	*/
	virtual void Close(void)=0;
	/*
	功能：
		取页宽，高
	输入：
		无
	输出：
		nWidth：图象的宽
		nHeihgt：图象的高
	返回：
		打开的文件合法返回true,否则返回false
	*/
	virtual bool GetSize(int&nWidth,int&nHeight)=0;
	/*
	功能：
		取文件的页数
	输入：
		无
	输出：
		无
	返回：
		返回文件的页数，打开的文件合法返回值大于0，否则等于0
	*/
	virtual int GetPages(void)=0;
	/*
	功能：
		显示一页
	输入：
		nPageIndex：页号（0到总页数-1）
		hDC：要显示的DC
		x,y：显示的DC的位置的左上坐标
		nWidth：显示的宽度
		nHeight：显示的高度
	输出：
		无
	返回：
		成功返回true,否则返回false
	*/
	virtual bool DrawPage(int nPageIndex,HDC hDC,int x ,int y ,int nWidth,int nHeight)=0;
	/*
	功能：
		保存一页为文件
	输入：
		nPageIndex：页号（0到总页数-1）
		cszTempFile：文件全路径
	输出：
		无
	返回：
		成功返回true,否则返回false
	*/
	virtual bool SavePageToTempFile(int nPageIndex,const char*cszTempFile)=0;
};

/*
功能：
	创建ImageFile对象
输入：
	无
输出：
	无
返回：
	成功返回对象指针，否则返回空
*/
AVCONDOC_API IAVImageFile*CreateIAVImageFile(void);


#endif