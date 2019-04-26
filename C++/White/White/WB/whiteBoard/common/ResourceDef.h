#pragma once
//#include <string>
#include <map>
using namespace std;
#define CHANGE_BUT_TYPE 1
typedef enum _eType
{   
	TYPE_NONE = 0,//
	TYPE_BACKGROUND,    //��ť����
	TYPE_CHILDBG,       //�Ӱ�ť����
	TYPE_MEDIOVIDEOSCREEN,//��Ļ¼�ư�ť�ı���
	TYPE_RECYCLE,       //����վ
	TYPE_DICOM,			//ҽ��DICOM
	TYPE_SELECT,		//ѡ��
	TYPE_PAGE,			//ҳ
	TYPE_FULLSCREEN,	//ȫ��
	TYPE_BROADCAST,     //�㲥
	TYPE_BIGHT,			//����
	TYPE_LINE,			//ֱ��
	TYPE_RUBBER_BIG,	//����Ƥ
	TYPE_RUBBER_OBJ,	//������Ƥ
	TYPE_RUBBER_CURVE,	//������Ƥ
	TYPE_IMAGE,			//����ͼƬ
	TYPE_VIDEO,			//������Ƶ
	TYPE_DOCUMENT,		//�����ĵ�
	TYPE_FLASH,			//����flash
	TYPE_CAMERAHEAD,    //����ͷ
	TYPE_ALLTOOL,		//������
	TYPE_RESOURCE,		//��ʾ��Դ����
	TYPE_DEL,			//ɾ��
	TYPE_PRE_PAGE,		//��һҳ
	TYPE_NEXT_PAGE,		//��һҳ
	TYPE_T_H,			//ˮƽ�ı�
	TYPE_T_V,			//��ֱ�ı�
	TYPE_REDO,			//��һ��
	TYPE_UNDO,			//��һ��
	TYPE_CANTOUCH,      //������Ȩ
	TYPE_NOTOUCH,       //����δ��Ȩ
	TYPE_TRIANGLE,		//������
	TYPE_RECTANGLE,		//����
	TYPE_ROUNDNESS,     //Բ��
	TYPE_RHOMB,			//����
	TYPE_POLYGON,       //�����
	TYPE_SCREENSHOTS,	//����
	TYPE_MAQN,	        //�Ŵ�
	TYPE_SPOTLIGHT,     //�۹��
	TYPE_BAFFLEPLATE,   //����
	TYPE_PROTRACTOR,    //������
	TYPE_COMPASSES,     //Բ��

	TYPE_RULER,         //ֱ��
    TYPE_SETSQUARE,     //���ǰ�
	TYPE_SCREENREC,     //��Ļ¼��
	TYPE_DDOPEN,        //�����ͼ��,����DD�����desktopDraw
	TYPE_DDFULLSCREEN,  //�����ͼȫ��
	TYPE_DDEFFECT,      //�����ͼЧ���л�
	TYPE_DDCTRL,        //�����ͼ������
	TYPE_DDCLOSE,       //�����ͼ�ر�
	TYPE_SAVE,			//����
	TYPE_PRINT,			//��ӡ
	TYPE_CLOSE,         //�ر�
	TYPE_OPEN,          //��
	TYPE_COMBINE,       //�ϲ�
	TYPE_RESAVE,		//���
	TYPE_DLG_LINETYPE,  //����
	TYPE_DLG_RUBBER,    //����
	TYPE_DLG_GRAPHICS,  //ͼ��
	TYPE_DLG_RESOURCEFILE,//��Դ�ļ�
	TYPE_DLG_TOOL,        //����
	TYPE_DLG_DESKTOPDRAW,//��ע
	TYPE_DLG_FILE,      //�ļ�������
	TYPE_DLG_T,				//�ı�������
	TYPE_DLG_COLORPALETTE,   //��ɫ��Ի���
	TYPE_FUNCTIONDLG,       //�������
	TYPE_TOOLBAR_SHOW,         //��ʾ�����ع�����
	TYPE_TOOLBAR_HIDE,         //��ʾ�����ع�����
	TYPE_TOOLBOX_CLOSE ,       //������ر�
	TYPE_EAGLE_EYE,                  //ӥ��
	//��Ļ¼�ƶԻ����а�ť
	TYPE_FULLSCREENRECORD,   //ȫ��¼��
	TYPE_AREASCREENRECORD,   //����¼��
	TYPE_WINDOWSCREENRECORD,   //����¼��
	TYPE_PAUSESCREENRECORD,   //¼����ͣ
	TYPE_STOPSCREENRECORD,   //¼��ֹͣ
	TYPE_PROPERTYSCREENRECORD,   //¼�Ƶ�����
	TYPE_PATHSCREENRECORD,   //¼�Ƶ�·��ѡ��
	//���ԶԻ��������
	TYPE_LINECOLORBUTTON,	//����ɫ��ť
	TYPE_COLORRADIO,//
	TYPE_LINECOLORBUTTONDOWN,//������ɫ��ť
	//TYPE_ATTRIPADCOLORBUTTONDOWN,//������ɫ��ť
	TYPE_SOLIDCOLORBUTTON, //�����ɫ��ť
	TYPE_SOLIDCOLORBUTTONDOWN, //���������ɫ��ť
	//������ʱ�������͹ػ���ť
	TYPE_SINGLE_NETWORK,//����������
	TYPE_SINGLE_LOCAL,//�����汾��
	TYPE_SINGLE_SHOTDOWN,//������ʱ���Ϊ�ն˹ػ�
	TYPE_SINGLE_QUIT,     //���������Ϊpc���˳�����
	TYPE_ATRTRIANGLE , //����������
	TYPE_DYTRIANGLE,   //����������
	TYPE_ZJTRIANGLE,   //ֱ��������
	TYPE_DYZJTRIANGLE,  //����ֱ��������
	TYPE_PALLELOGAM,  //ƽ���ı���
	TYPE_PENTAGON,		//�����
	TYPE_HEXAGON,		//������
	TYPE_FIVEPOINTSTART, //�����
	TYPE_TRAPEZIUM,     //����
	TYPE_SAVE_PIC,		//�����ͼƬ
	TYPE_ICOCURVE,    //ͼ������
	TYPE_DXF,      //����Dxf
	TYPE_MAGCURVE, //ħ����
	TYPE_MIDI //midi�ļ�
}eType;

typedef std::map<int,CString> MAP_PATH;
