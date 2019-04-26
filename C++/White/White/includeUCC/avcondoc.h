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
	AVDOCS_INSTALL_DRIVER=0,	//���ڰ�װ����
	AVDOCS_TOTAL_PAGES,			//�ܹ�����ҳ
	AVDOCS_CURRENT_PAGE,		//��ǰת���˶���ҳ
	AVDOCS_DONE,				//ת�����
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
���ܣ�
	���ĵ�ת����Image�ļ����ú���������ת�����ŷ���
���룺
	strDocFile����ת�����ĵ�(.doc,.ppt,.xls,.pdf)
	strImgFile��ת�����ļ�������·�����ļ���
	hWnd��������Ϣ�Ĵ��ھ������ѡ��
	nUserMsg��������Ϣ��ID�ţ���ѡ��
�����
	��
���أ�
	ת���ɹ�����true,���򷵻�false
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
	���ܣ�
		��Image�ļ�
	���룺
		strImgFile��ת�����ļ�������·�����ļ���
	�����
		��
	���أ�
		ת���ɹ�����true,���򷵻�false
	*/
	virtual bool Open(const char*cszImgFile)=0;
	/*
	���ܣ�
		�ر�Image�ļ�
	���룺
		��
	�����
		��
	���أ�
		��
	*/
	virtual void Close(void)=0;
	/*
	���ܣ�
		ȡҳ����
	���룺
		��
	�����
		nWidth��ͼ��Ŀ�
		nHeihgt��ͼ��ĸ�
	���أ�
		�򿪵��ļ��Ϸ�����true,���򷵻�false
	*/
	virtual bool GetSize(int&nWidth,int&nHeight)=0;
	/*
	���ܣ�
		ȡ�ļ���ҳ��
	���룺
		��
	�����
		��
	���أ�
		�����ļ���ҳ�����򿪵��ļ��Ϸ�����ֵ����0���������0
	*/
	virtual int GetPages(void)=0;
	/*
	���ܣ�
		��ʾһҳ
	���룺
		nPageIndex��ҳ�ţ�0����ҳ��-1��
		hDC��Ҫ��ʾ��DC
		x,y����ʾ��DC��λ�õ���������
		nWidth����ʾ�Ŀ��
		nHeight����ʾ�ĸ߶�
	�����
		��
	���أ�
		�ɹ�����true,���򷵻�false
	*/
	virtual bool DrawPage(int nPageIndex,HDC hDC,int x ,int y ,int nWidth,int nHeight)=0;
	/*
	���ܣ�
		����һҳΪ�ļ�
	���룺
		nPageIndex��ҳ�ţ�0����ҳ��-1��
		cszTempFile���ļ�ȫ·��
	�����
		��
	���أ�
		�ɹ�����true,���򷵻�false
	*/
	virtual bool SavePageToTempFile(int nPageIndex,const char*cszTempFile)=0;
};

/*
���ܣ�
	����ImageFile����
���룺
	��
�����
	��
���أ�
	�ɹ����ض���ָ�룬���򷵻ؿ�
*/
AVCONDOC_API IAVImageFile*CreateIAVImageFile(void);


#endif