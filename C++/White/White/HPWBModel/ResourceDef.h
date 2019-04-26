#pragma once
#include <string>
#include <map>
using namespace std;
#define CHANGE_BUT_TYPE 1
typedef enum _eType
{
	TYPE_NONE = 0,//
	TYPE_SELECT,		//ѡ��
	TYPE_PAGE,			//ҳ
	TYPE_FULLSCREEN,	//ȫ��
	TYPE_EXITFULLSCREEN,//�˳�ȫ��
	TYPE_ARROWS,		//ֱ�ߺͼ�ͷ
	TYPE_BIGHT,			//����
	TYPE_LINE,			//ֱ��
	TYPE_RUBBER,		//��Ƥ
	TYPE_RUBBER_SMALL,	//С��Ƥ
	TYPE_RUBBER_BIG,	//����Ƥ
	TYPE_RUBBER_OBJ,	//������Ƥ
	TYPE_RUBBER_CURVE,	//������Ƥ
	TYPE_IMAGE,			//����ͼƬ
	TYPE_VIDEO,			//������Ƶ
	TYPE_DOCUMENT,		//�����ĵ�
	TYPE_FLASH,			//����flash
	TYPE_ALLTOOL,		//������
	TYPE_RESOURCE,		//��ʾ��Դ�Ի���
	TYPE_DEL,			//ɾ��
	TYPE_PRE_PAGE,		//��һҳ
	TYPE_NEXT_PAGE,		//��һҳ
	TYPE_T,				//�ı�
	TYPE_REDO,			//��һ��
	TYPE_UNDO,			//��һ��
	TYPE_TRIANGLE,		//������
	TYPE_RECTANGLE,		//����
	TYPE_ROUNDNESS,     //Բ��
	TYPE_RHOMB,			//����
	TYPE_POLYGON,       //�����
	TYPE_FONT,			//����
	TYPE_ATTRIBUTE,		//����
	TYPE_SCREENSHOTS,	//����
	TYPE_MAQN,	        //�Ŵ�
	TYPE_SPOTLIGHT,     //�۹��
	TYPE_DESKTOPDRAW,   //�����ͼ
	TYPE_DDOPEN,        //�����ͼ��,����DD�����desktopDraw
	TYPE_DDFULLSCREEN,  //�����ͼȫ��
	TYPE_DDEFFECT,      //�����ͼЧ���л�
	TYPE_DDCTRL,        //�����ͼ������
	TYPE_DDCLOSE,       //�����ͼ�ر�
	TYPE_SAVE,			//����
	TYPE_PRINT,			//��ӡ
	TYPE_CLOSE,         //�ر�
	TYPE_OPEN,          //��
	TYPE_RESAVE,		//���
    TYPE_DLG_RESOURCE,  //��ʾ�͹ر���Դ������
	TYPE_DLG_RUBBER,    //���ӹ�����
	TYPE_DLG_GRAPHICS,  //ͼ�ι�����
	TYPE_DLG_FILE,      //�ļ�������
	TYPE_DLG_PAGE,      //ҳ������
    TYPE_CLR_REN,       //��
	TYPE_CLR_BLACK,     //��
	TYPE_CLR_YELLOW,    //��
	TYPE_CLR_ORANGE,    //��
	TYPE_CLR_BLUE,      //��
	TYPE_CLR_PURPLE,    //��
	TYPE_FUNCTIONDLG     //�������
}eType;

typedef std::map<int,string> MAP_PATH;