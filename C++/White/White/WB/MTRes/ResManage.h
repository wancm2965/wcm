#pragma once
#include "ResData.h"
#include "tchar.h"
#define CONFIG_FILEPATH_OF_TOOLBAR	(TEXT("\\WBConfig.ini"))

class CResManage
{
protected:
	CResManage();
	~CResManage();

public:
	/*******************************************************************
	�������ƣ�GetInstance
	�������ܣ������ʴ˶������ں�������ȡʵ������
	����˵������
	����ֵ  ��CResManage��ȫ��ʵ������
	��ע    ��
	*******************************************************************/
	static CResManage *GetInstance();

	/*******************************************************************
	�������ƣ�GetResType
	�������ܣ������ߺ������ж���Դ����
	����˵����
	strFileFullPath����Դ�ļ���·��
	����ֵ  ��
	WBRES_PICTURE, ͼƬ�ļ�
	WBRES_MOVIE,   ��Ӱ�ļ�
	WBRES_FLASH,   ��Ƶ�ļ�
	WBRES_DOC,     doc�ĵ�
	WBRES_MUISC    �����ļ�
	��ע    ��
	*******************************************************************/
	static int GetResType(const std::string &strFileFullPath);

	/*******************************************************************
	�������ƣ�IsResFileExtensionValid
	�������ܣ������ߺ�������Դ�ļ���չ���Ƿ���Ч(�����Ƿ���������֧�ֵ�����)
	����˵����strFileName����Դ�ļ�·��
	����ֵ  �������ļ�ʱ�װ�֧�ֵģ�����TRUE������FALSE
	��ע    ��
	*******************************************************************/
	BOOL IsResFileExtensionValid(LPCTSTR strFileName);

public:
	/*******************************************************************
	�������ƣ�GetResMap
	�������ܣ���ȡ��Դ���ݶ���ָ��
	����˵������
	����ֵ  ������һ��ResMapArray *���͵�ָ��
	��ע    ��
	*******************************************************************/
	ResMapArray *GetResMap();

	////���ش�����Դ������ʾ״̬
	//const BOOL IsResWndShow();
	////��ʾӰ����Դ����
	//void ShowMTResourceWindow();
	////�ж��Ƿ���ʾ
	//BOOL IsMTResourceWindowShow();

	////������Դ����ָ��
	//void SetResWindows(CWBResGallery *pResGally);
	////������Դ�ⴰ�ڵĸ�����
	//void SetResWindowsParent(HWND hWnd){m_hResFather = hWnd;}

	/*******************************************************************
	�������ƣ�GetCurResourceIndex
	�������ܣ���ȡ��ǰʹ�õ���Դ������(���㿪ʼ)
	����˵������
	����ֵ  �����ص�ǰʹ�õ���Դ����
	��ע    ��
	*******************************************************************/
	unsigned int GetCurResourceIndex();

	/*******************************************************************
	�������ƣ�SetCurResourceIndex
	�������ܣ����õ�ǰʹ�õ���Դ������(���㿪ʼ)
	����˵����ָ������Դ����
	����ֵ  ����
	��ע    ��
	*******************************************************************/
	void SetCurResourceIndex(unsigned int nIndex);

	/*******************************************************************
	�������ƣ�LoadWBResourceSetting
	�������ܣ��������ļ��ж�ȡ��д��������Ϣ
	����˵������
	����ֵ  ����
	��ע    �������ļ������ڰ�װĿ¼�µ�WBConfig.xml
	*******************************************************************/
	void LoadWBResourceSetting();

	/*******************************************************************
	�������ƣ�SaveWBResourceSetting
	�������ܣ������û�������Ϣ��������Դ�ļ������ļ�·������ǰʹ�õ���Դ������
	����˵������
	����ֵ  ��������ȷ����TRUE�����򷵻�FALSE
	��ע    �������ļ������ڰ�װĿ¼�µ�WBConfig.xml
	*******************************************************************/
	BOOL SaveWBResourceSetting();
	
	/********************************************************************
	���������ļ���д·��
	********************************************************************/
	void SetAvconIniPath(LPCTSTR strPath);
	tstring GetAvconIniPath();

private:
	/*******************************************************************
	�������ƣ�ClearResData
	�������ܣ������Դ����
	����˵������
	����ֵ  ����
	��ע    ������Դ�����������ա�
	*******************************************************************/
	void ClearResData();

	int CResManage::ReadResourcePathCount();
	void CResManage::ReadResourcePath(int i,TCHAR* pathname,TCHAR* pathstr);
	void WriteResourcePathCount(int iValue);
	void WriteResourcePath(int i,const TCHAR* pathname,const TCHAR* pathstr);

private:
	//��ԴĿ¼������Ϣ
	ResMapArray m_resMap;

	//��̬Ψһʵ������
	static CResManage m_inst;

	//�����ʼ��·��
	tstring m_strAvconIniPath;
	////��Դ����
	//CWBResGallery *m_pResGally;
	//HWND m_hResFather;

	////��Դ�����Ƿ���ʾ
	//BOOL m_bResGallyShow;
	//��ǰѡ�е���Դ����
	unsigned int m_nIndexOfCurUsingResource;
};