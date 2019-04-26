#pragma once
// [12/14/2009 lzy]
//��̨��ض���
//ֹͣ���������һ��������

#define CMD_UP			101		//����
#define CMD_STOP_UP		201		
#define CMD_DOWN		102		//����
#define CMD_STOP_DOWN   202		
#define CMD_LEFT		103		//����
#define CMD_STOP_LEFT	203		
#define CMD_RIGHT		104		//����
#define CMD_STOP_RIGHT  204		
#define CMD_DN			105		//����
#define CMD_STOP_DN     205		
#define CMD_DF			106		//���Զ
#define CMD_STOP_DF     206		
#define CMD_FS			107		//�佹С
#define CMD_STOP_FS		207		
#define CMD_FB			108		//�佹��
#define CMD_STOP_FB     208		
#define CMD_HS			109		//��ȦС
#define CMD_STOP_HS		209		
#define CMD_HB			110		//��Ȧ��
#define CMD_STOP_HB     210		
#define CMD_AUTO		111		//�Զ�
#define CMD_STOP_AUTO	211		
#define CMD_AE_ON		112		//�Զ��ع�ON
#define CMD_AE_OFF		113		//�Զ��ع�OFF
#define CMD_EV_SET		114		//�����ع�����ֵ
#define CMD_EV_SET_END	126		//�����ع�����ֵ����

//���Ԥ�õ�������
//Ԥ�õ���ܴ���100��
#define CMD_AUTO_SCAN	218		//�Զ�Ѳ�� == CMD_PATROL_RUN
#define CMD_PREST_SET	301		//����Ԥ�õ�
#define CMD_PREST_CALL	401		//���Ԥ�õ�
#define CMD_PREST_CLR	501		//����Ԥ�õ�
//-----------------------------

//Ѳ������:Pattern
#define CMD_PATROL_START	216
#define CMD_PATROL_STOP		217
#define CMD_PATROL_RUN		218	//ֹͣѲ���ɵ���������һ����������

#define CMD_SET_SPEED	220
#define MAX_PREPOINT	128		//Ԥ�õ����


typedef enum _enumProtocol
{
	HN4201 = 0,
	HN4203,
	ALEC6319,
	PIH1016,
	TLAB011,
	TKC675,
	RM110,
	EIA,
	KRE302,
	PELCO_P,		//pelco-p
	PELCO_D,		//pelco-d
	SONY,			//sony
	SONY_M,			//sony-M
	YAAN_BOARD,
	YAAN_BALL,
	HC2030,
	VICON,
	WV_CSR,
	PWT,
	NES2000M,
	TAC_2,
	PTZ_TYPE_COUNT,			//����Ϊö������

}PTZ_PROTOCOL_TYPE;	//��̨Э������

//ֵ��Ӧ���ַ����ṹ��
typedef struct tagMATCH_STRING
{
	int		 data;
	LPCTSTR  text;	
}MATCH_STRING;

#define ADD_MATCH_STRING(x)   x, _T(#x)	//ת����Ӧ���ַ���