#pragma once
//-------------------------------��������------------------------------------

//���� ����
typedef enum EGestureType
{
	GestureType_none,		
	GestureType_translation,	
	GestureType_direction	
}EGestureType;

//�������Ƶ� ����
typedef enum EGestureDirection
{
	GestureDirection_none		=0,	//0x00000000	//����
	GestureDirection_right,			//0x00000001	//�ҷ�
	GestureDirection_right_up,		//0x00000002	//���Ͻ�
	GestureDirection_up,			//0x00000003	//�Ϸ�
	GestureDirection_left_up,		//0x00000004	//���Ͻ�
	GestureDirection_left,			//0x00000005	//��
	GestureDirection_left_down,		//0x00000006	//���½�
	GestureDirection_down,			//0x00000007	//�·�
	GestureDirection_right_down		//0x00000008	//���½�
}EGestureDirection;
	
// Flags for CContainerUI::GetGestureFlag()
#define GSFLAG_TRANS_HOR		0x0001//֧��ˮƽ����
#define GSFLAG_TRANS_VER		0x0002//֧����ֱ����
#define GSFLAG_TRANS_INERTIAL	0x0004//֧�ֹ��Ի���
#define GSFLAG_TRANS_FORBID		0x0008//��ֹ��������
#define GSFLAG_TRANS		(GSFLAG_TRANS_HOR | GSFLAG_TRANS_VER)
#define GSFLAG_DIRECTION		0x0020//֧�ַ����Է���
#define GSFLAG_DIRECTION_FORBID	0x0040//��ֹ�����Է���

//ƽ������״̬
#define TranslationState_none			 0x00000000
#define TranslationState_begin			 0x00000001
#define TranslationState_ing			 0x00000002
#define TranslationState_end			 0x00000004
#define TranslationState_inertia_begin	 0x00000008
#define TranslationState_inertia_ing	 0x00000010
#define TranslationState_inertia_end	 0x00000020
