
//========================================================================================
//��Ȩ����: ��Ȩ����(C) 2008���Ϻ���ƽ��Ϣ�����ɷ����޹�˾
//ϵͳ����: AVCON7
//��������: duquansheng
//��������: 2008-3-31
//����˵��: ���ݴ���������ں˽ӿ��ඨ��
//

#include "stdafx.h"
#include <HPDTS/IHPDTS.h>
#include <HPDTS/DTSCmdType.h>
#include "HPDTS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------------------------
static std::string		m_strFileTemPath = "";
static std::string		m_strFileSavePath = "";

//========================================================================================
//��������: ����HPDTSʵ��
//��������: duquansheng
//��������: 2008-3-31
//����˵��: rIHPDTSNotify:�ص����ã�bUI���Ƿ���Ҫ����ص�
//����ֵ  : HPDTSʵ��ָ��
//---------------------------------------------------------------------------------------
HPDTS_API IHPDTS* CreateIHPDTS(IHPDTSNotify& rIHPDTSNotify,bool bUI)
{
	return new HPDTS(rIHPDTSNotify,bUI);
}

//========================================================================================
//��������: WIN32�����£�ɾ��ָ��Ŀ¼
//��������: duquansheng
//��������: 2008-3-31
//����˵��: Ŀ¼·��
//����ֵ  : �Ƿ�ɹ�
//---------------------------------------------------------------------------------------
#ifdef WIN32
bool DeleteDirectory(const std::string& DirName)
{
	if (DirName.length() == 0)
	{
		return false;
	}
	CFileFind tempFind;
	char tempFileFind[MAX_PATH];
	sprintf(tempFileFind,"%s\\*.*",DirName.c_str());
	bool IsFinded=(bool)tempFind.FindFile(tempFileFind);
	while(IsFinded)
	{
		IsFinded=(bool)tempFind.FindNextFile();
		if(!tempFind.IsDots())
		{
			char foundFileName[MAX_PATH];
			strcpy(foundFileName,tempFind.GetFileName().GetBuffer(MAX_PATH));
			if(tempFind.IsDirectory())
			{
				char tempDir[MAX_PATH];
				sprintf(tempDir,"%s\\%s",DirName.c_str(),foundFileName);
				DeleteDirectory(tempDir);
			}
			else
			{
				char tempFileName[MAX_PATH];
				sprintf(tempFileName,"%s\\%s",DirName.c_str(),foundFileName);		
				DeleteFile(tempFileName);
			}
		}
	}
	tempFind.Close();
	if(!RemoveDirectory(DirName.c_str()))
	{
		TRACE(TEXT("ɾ��Ŀ¼ʧ�ܣ�\n"));
		return false;
	}
	return true;
}
#endif

//========================================================================================
//��������: ��ʼ��HPDTS
//��������: duquansheng
//��������: 2008-3-31
//����˵��: cszTemPath��HPDTS��ʹ�õ���ʱĿ¼��ע�⣺��Ŀ¼��HPDTS�ͷ��ǽ���ɾ��������
//����ֵ  : 
//---------------------------------------------------------------------------------------
HPDTS_API void HPDTSInit(const char* cszTemPath)
{
	printf("path=%s\n",cszTemPath);

	m_strFileTemPath=cszTemPath;
	//����Ŀ¼
#ifdef WIN32	
	m_strFileTemPath += "\\DTSTempDir";
	//���Ŀ¼
	HPDTSUnInit();
	//_mkdir(m_strFileTemPath.c_str());
	int iPos = 0;
	iPos = m_strFileTemPath.find_first_of("\\",iPos);
	while (iPos != -1)
	{
		iPos = m_strFileTemPath.find_first_of("\\",iPos+1);
		string strPath = m_strFileTemPath.substr(0,iPos);
		_mkdir(strPath.c_str());
	}
#else	
	m_strFileTemPath += "/DTSTempDir";
	//���Ŀ¼
	HPDTSUnInit();
	std::string mkdir="mkdir ";
	mkdir+=m_strFileTemPath.c_str();	
	system(mkdir.c_str());
	printf("file=%s\n",mkdir.c_str());
#endif

}

//========================================================================================
//��������: HPDTS�ͷ�
//��������: duquansheng
//��������: 2008-3-31
//����˵��: 
//����ֵ  : 
//---------------------------------------------------------------------------------------
HPDTS_API void HPDTSUnInit(void)
{
	if (m_strFileTemPath.length() == 0)
	{
		return;
	}

	int pos = m_strFileTemPath.find("DTSTempDir");
	if(pos < 1)
	{
		return;
	}
	//���Ŀ¼
#ifdef WIN32
	DeleteDirectory(m_strFileTemPath.c_str());	
#else
	std::string rmdir="rm -rf ";	
	rmdir+=m_strFileTemPath.c_str();	
	system(rmdir.c_str());	
#endif
}

//========================================================================================
//��������: �ļ����䣬�ļ��ַ�ʱ���ͻ��������ļ�����Ŀ¼,���ظ����ã��滻ԭ�����á�
//��������: duquansheng
//��������: 2008-3-31
//����˵��: cszSavePath���ļ�����ȫ·�����ɲ�����\����β��
//����ֵ  : 
//---------------------------------------------------------------------------------------
HPDTS_API void HPDTSSetSavePath(const char* cszSavePath)
{
	m_strFileSavePath = cszSavePath;
	//����Ŀ¼
#ifdef WIN32	
	_mkdir(m_strFileSavePath.c_str());
#else	
	std::string mkdir="mkdir ";
	mkdir+=m_strFileSavePath.c_str();	
	system(mkdir.c_str());
#endif

}

//========================================================================================
//��������: ��ȡDTS��ʱĿ¼
//��������: duquansheng
//��������: 2008-3-31
//����˵��: 
//����ֵ  : DTS��ʱĿ¼ȫ·��
//---------------------------------------------------------------------------------------
HPDTS_API const char* GetFileTemPath(void) 
{
	return m_strFileTemPath.c_str();
}

//========================================================================================
//��������: ��ȡDTS�ļ�����Ŀ¼
//��������: duquansheng
//��������: 2008-3-31
//����˵��: 
//����ֵ  : DTS�ļ�����ȫ·��
//---------------------------------------------------------------------------------------
HPDTS_API const char* GetFileSavePath(void)
{
	return m_strFileSavePath.c_str();
}

//---------------------------------------------------------------------------------------