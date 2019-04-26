//DATEC_PDFFile.h
#ifndef __DATEC_PDFFILE_H__
#define __DATEC_PDFFILE_H__

#include <DATEC/DATEC_Export.h>
#include <windows.h>

//PDF�ļ���ʾ��
class DATEC_API DATEC_PDFFile
{
public:
	DATEC_PDFFile(void){};
	virtual~DATEC_PDFFile(void){};
public:
	/******************************************************************************
	* Open
	*��������PDF�ļ���ʾ
	*���룺	cszPathName	-�ļ���ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int Open(const char*cszPathName)=0;
	
	/******************************************************************************
	* Close
	*�������ر�PDF�ļ���ʾ
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void Close(void)=0;

	/******************************************************************************
	* GetPageCount
	*��������ȡҳ��
	*���룺��
	*�������
	*����ֵ������ҳ��
	*****************************************************************************/
	virtual int GetPageCount(void)=0;

	/******************************************************************************
	* GetPageSize
	*��������ȡҳ���С
	*���룺��
	*�����	nWidth		-ҳ����
			nHeight		-ҳ��߶�
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetPageSize(int&nWidth,int&nHeight)=0;

	/******************************************************************************
	* DrawPage
	*��������һҳ
	*���룺	hDC			-Handle to device context.
			nPageIndex	-Page index, zero for the first page.
			x			-Left pixel position of the display area in the device coordination
			y			-Top pixel position of the display area in the device coordination
			cx			-Horizontal size (in pixels) for displaying the page
			cy			-Vertical size (in pixels) for displaying the page
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int DrawPage(HDC hDC,int nPageIndex,int x,int y,int cx,int cy)=0;
	
	
	/******************************************************************************
	* Create
	*����������PDF�ļ���ʾʵ��
	*���룺��
	*�������
	*����ֵ��PDF�ļ���ʾʵ��
	*****************************************************************************/
	static DATEC_PDFFile*Create(void);
};



#endif
