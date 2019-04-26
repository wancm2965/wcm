#pragma once
#include <list>

//�����ļ���Ϣ�����ǩ
#define  WB_RESOURCE_FILENAME					"\\WBConfig.xml"
#define  WB_CARMERA_VIDEO_FILENAME              "\\WBCameraVideoFilename.xml"
#define  WB_CARMERA_AUDIAO_FILENAME             "\\WBCameraAudioFilename.xml"
#define  WB_RESOURCE_FILE_DIRECTORY				"\\Config"
#define  WB_RESOURCE_XML_HEAD					"ResourceDirectoryConfiguration"
#define  WB_RESOURCE_ELEMENT_NAME				"Map"
#define  WB_RESOURCE_ELEMENT_ATTRIBUTE_1		"Name"
#define  WB_RESOURCE_ELEMENT_ATTRIBUTE_2		"Path"
#define  WB_RESOURCE_ELEMENT_LAST_USING_ITEM	"LastUsedItem"
#define CONFIG_FILEPATH_OF_TOOLBAR				"\\WBConfig.ini"

//#define UWM_UPDATE_RESOURCE_GALLERY			(WM_USER+2787)	//���͸�����ԴĿ¼����Ϣ����Ϣ
//#define UWM_RESGALLERY_BOUNDARY				(WM_USER+2788)	//��ԴԽ�����Ϣ

//��֧�ֵ���չ��
#ifndef EResType
typedef enum
{
	WBRES_PICTURE = 0,
	WBRES_MOVIE,
	WBRES_FLASH,
	WBRES_DOC,
	WBRES_MUISC,
	WBRES_DXF //add by jiang chao 2013.09.05
}EResType;	//�ļ�����
#endif

typedef struct ResourceMap
{
#ifdef _UNICODE
private:
	std::wstring sResourceMapName;//ӳ������
	std::wstring sResourceMapPath;//ӳ���ַ
#else
private:
	std::string sResourceMapName;//ӳ������
	std::string sResourceMapPath;//ӳ���ַ
#endif
public:
	LPCTSTR GetMapName(){return sResourceMapName.c_str();}
	LPCTSTR GetMapPath(){return sResourceMapPath.c_str();}
	void SetMapName(LPCTSTR sMapName)
	{
#ifdef _UNICODE
		sResourceMapName = std::wstring(sMapName);
#else
		sResourceMapName = std::string(sMapName);
#endif
	
	}
	void SetMapPath(LPCTSTR sMapPath)
	{
#ifdef _UNICODE
		sResourceMapPath = std::wstring(sMapPath);
#else
		sResourceMapPath = std::string(sMapPath);
#endif
	}
	LPCTSTR GetNameFromPath()
	{
#ifdef _UNICODE
		std::wstring sFileName;
#else
		std::string sFileName;
#endif
		int nTag = sResourceMapPath.rfind('\\');
		if (nTag>=0)
		{
			sFileName = sResourceMapPath.substr(nTag+1, sResourceMapPath.size()-nTag-1);//�ļ���
		}
		return sFileName.c_str();
	};
	LPCTSTR GetDirFromPath()
	{
#ifdef _UNICODE
		std::wstring sDirName;
#else
		std::string sDirName;
#endif
		int nTag = sResourceMapPath.rfind('\\');
		if (nTag>=0)
		{
			sDirName = sResourceMapPath.substr(0, nTag+1);//Ŀ¼������"\"����
		}
		return sDirName.c_str();
	}
}SResourceMap, *PSResourceMap;

typedef std::list<PSResourceMap> ResMapArray, *PResMapArray;
typedef std::list<PSResourceMap>::iterator ResMapArrayIter;
typedef std::list<PSResourceMap>::const_iterator ResMapArrayConstIter;
