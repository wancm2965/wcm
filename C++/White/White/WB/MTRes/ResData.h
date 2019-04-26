#pragma once
#include <list>
#include "D2DDriver.h"

#ifdef AVCON_MUTITOUCH_RESOURCE_EXPORTS
#define LIB_AVCON_RESOURCE_DECL __declspec(dllexport)
#else
#define LIB_AVCON_RESOURCE_DECL extern
#endif

#ifndef UWM_LOAD_COMPLETE_RESOURCE_GALLERY
#define UWM_LOAD_COMPLETE_RESOURCE_GALLERY	(WM_USER+2789)	//��ɼ�����Դ����Ϣ(���߳�)
#endif

//��֧�ֵ���չ��
#ifndef EResType
typedef enum
{
	WBRES_PICTURE = 0,
	WBRES_MOVIE,
	WBRES_FLASH,
	WBRES_DOC,
	WBRES_MUISC
}EResType;	//�ļ�����
#endif


typedef struct ResourceMap
{
private:
	tstring sResourceMapName;//ӳ������
	tstring sResourceMapPath;//ӳ���ַ
public:
	LPCTSTR GetMapName(){return sResourceMapName.c_str();}
	LPCTSTR GetMapPath(){return sResourceMapPath.c_str();}
	void SetMapName(LPCTSTR sMapName)
	{
		sResourceMapName = tstring(sMapName);
	}
	void SetMapPath(LPCTSTR sMapPath)
	{
		sResourceMapPath = tstring(sMapPath);
	}
	LPCTSTR GetNameFromPath()
	{
		tstring sFileName;
		int nTag = sResourceMapPath.rfind(TEXT('\\'));
		if (nTag>=0)
		{
			sFileName = sResourceMapPath.substr(nTag+1, sResourceMapPath.size()-nTag-1);//�ļ���
		}
		return sFileName.c_str();
	};
	LPCTSTR GetDirFromPath()
	{
		tstring sDirName;
		int nTag = sResourceMapPath.rfind(TEXT('\\'));
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