#pragma once
#include <windows.h>
#ifdef MTFOLER_EXPORTS
#define DLL_MTFOLER_API __declspec(dllexport)
#else
#define DLL_MTFOLER_API __declspec(dllimport)
#endif
//���Ի���Ϊģ̬�Ի����򷵻�ֵ��Ч����Ϊ��ģ̬�Ի���
//���ɸ���Ϣ(WPARAM)���տ��ֽ������ַ���ָ��,(LPARAM)ʼ��ΪUWM_FOLERFILE
#define UWM_FOLERFILE (WM_USER+112)	
typedef BOOL(*PMTFILETYPE)(const wchar_t *);//�ļ����͹��˻ص���������

#define DRIVECONFIG_UNKNOWN 0x01 //δ֪�豸
//#define DRIVECONFIG_NO_ROOT_DIR 0x02 //ָ��·������Ч��
#define DRIVECONFIG_REMOVABLE 0x04 //�ƶ�����
#define DRIVECONFIG_FIXED 0x08 //Ӳ��
#define DRIVECONFIG_REMOTE 0x10 //�������
#define DRIVECONFIG_CDROM 0x20 // ����
#define DRIVECONFIG_RAMDISK 0x40 //δ֪�豸
#define DRIVECONFIG_ALL 0xFF //ȫѡ

struct MTFOLDERCONFIG{
	MTFOLDERCONFIG():bModal(FALSE),x(450),y(200),WndWidth(400),WndHeight(450),
		bMoreSelect(FALSE),bShowFile(FALSE),bSysTouch(TRUE),bShow(NULL),
		InitPage(NULL),DiscConfig(DRIVECONFIG_FIXED|DRIVECONFIG_REMOVABLE|DRIVECONFIG_CDROM),
		FileConfig(0xFFFF),bShowRecently(FALSE),InitPath(NULL),DriveName(NULL),future(0){};
	BOOL bModal;//�Ƿ�Ϊģ̬
	INT x;//�Ի������
	INT y;//�������Ӧ��Ļ����
	INT WndWidth;//�Ի����
	INT WndHeight;//�Ի����
	BOOL bMoreSelect;//�Ƿ�֧�ֶ�ѡ
	BOOL bShowFile;//��ΪTRUE��Ϊ�ļ�ѡ�񣬷���Ϊ�ļ���ѡ��
	BOOL bSysTouch;//��ΪTRUE��ʹ��ϵͳ����������ʹ���Զ��崥��������û���Զ��崥���������ṩ����֧��
	PMTFILETYPE bShow;//�ص����˺���ָ�룬��ΪNULL��ʹ��Ĭ�Ϲ���
	const wchar_t *InitPage;//���ñ�ǩ����ʽ:·��<��ǩ��>·��<��ǩ��>...����֧�ֶ��ǩ��ӣ���ǩ���в�Ҫ����'\'':'�������ַ�����ΪNULL����л����б�ǩ����Ϊ��/����ֻ��ʾ�ҵĵ���
	DWORD DiscConfig;//Ĭ����ʾӲ�̺Ϳ��ƶ�����
	DWORD FileConfig;//Ĭ����ʾ�����ļ�����.��..��
	BOOL bShowRecently;//�Ƿ���������ļ���¼��ʷ����Ŀ¼
	const wchar_t *InitPath;//ָ����ʼĿ¼��ΪNULL����ʾ�ϴ��˳�ʱ��Ŀ¼
	const wchar_t *DriveName;//��������������ʽ:<ϵͳ����()><���ش���()><���ƶ�����(0)>�����������()��ʾ��ʾ����������,��(0)�����б�����������Զ������
	//const wchar_t *InitLanguage; //���ñ�ǩ(��ʽ:ȷ��,ȡ��,�ļ�ѡ��,�ļ���ѡ��)
	DWORD future;//����
};

extern "C" DLL_MTFOLER_API void IniLanguage(const wchar_t *,const wchar_t *);

extern "C" DLL_MTFOLER_API const wchar_t *ShowFileUI(HWND,MTFOLDERCONFIG* pconfig = NULL);
extern "C" DLL_MTFOLER_API BOOL CloseFolder();


#define UWM_FOLERDEL (WM_USER+114)	
extern "C" DLL_MTFOLER_API const wchar_t *ShowDialogUI(HWND);
extern "C" DLL_MTFOLER_API BOOL CloseDialogUI();