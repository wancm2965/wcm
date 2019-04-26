//DATEC_PDFConvert.h
#ifndef __DATEC_PDFCONVERT_H__
#define __DATEC_PDFCONVERT_H__

#include <DATEC/DATEC_Export.h>

//PDFת����
class DATEC_API DATEC_PDFConvert
{
public:
	typedef enum
	{
		PDF_STATUS_INITIALIZING=0,		//���ڳ�ʼ��
		PDF_STATUS_CONVERTING,			//����ת��
		PDF_STATUS_CONVERT_OK,			//ת���ɹ�
		PDF_STATUS_CONVERT_FAILED,		//ת��ʧ��
	}PDF_STATUS;
public:
	DATEC_PDFConvert(void){};
	virtual~DATEC_PDFConvert(void){};
public:
	/******************************************************************************
	* Open
	*��������PDF�ļ�ת��
	*���룺	cszDocPathName		-Ҫת�����ĵ���ȫ·��
			cszPDFPathName		-ת�����PDF�ĵ���ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int Open(const char*cszDocPathName,const char*cszPDFPathName)=0;
	
	/******************************************************************************
	* Close
	*�������ر�PDF�ļ�ת��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void Close(void)=0;

	/******************************************************************************
	* GetStatus
	*��������ȡ״̬
	*���룺��
	*�������
	*����ֵ������״̬
	*****************************************************************************/
	virtual DATEC_PDFConvert::PDF_STATUS GetStatus(void)=0;
	
	/******************************************************************************
	* Create
	*����������PDFת��ʵ��
	*���룺��
	*�������
	*����ֵ��PDFת��ʵ��
	*****************************************************************************/
	static DATEC_PDFConvert*Create(void);

	static void SetTempPath(const char*cszTempPath);

	static int InstallDriver(void);
};

#endif

