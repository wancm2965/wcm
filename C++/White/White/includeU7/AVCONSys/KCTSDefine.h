#pragma once
//����ҵ��
//=======================================================================================
// ���ֻ�����ҽ�Ʋ�����Ϣ
typedef struct _MEDICAID_PATIENT
{
	std::string strNumber;			//סԺ���
	std::string strName;			//����
	unsigned short usGender;		//�Ա�
	unsigned short usAge;			//����
	std::string strDepartment;		//��Ժ����
	std::string strSurgeon;			//����ҽ��
	std::string strSurgeonName;		//��������
	std::string strSurgeonDesc;		//������ע
	_MEDICAID_PATIENT()
	{
		usGender = 0;
		usAge = 0;
	}
}MEDICAID_PATIENT;
//=======================================================================================


//=======================================================================================
// edu����ģʽ�����б�
typedef struct _EDUROOMMODEINFO
{
	std::string	strEduRoomMode;//"1":ֱ������,"2":���ο���,"3":���߿���,"4":���ֻ�1,"5":���ֻ�2,"6":���ֻ�3,"7":���ֻ�4,"8":���ֻ�5
	std::string strEduRoomModeName;
}EDUROOMMODEINFO,*PEDUROOMMODEINFO;
typedef std::list<PEDUROOMMODEINFO> EDUROOMMODEINFOLST;

// ���ò�����־���Ͷ���
typedef enum _MMS_CALSSOPERATELOG_TYPE
{
	CALSSOPERATELOG_CLASSBEGINEND	= 0,	//���¿β���
	CALSSOPERATELOG_TEACH			= 1,	//�������ڿβ���
	CALSSOPERATELOG_POWERPOINT		= 2,	//��ʾ�μ�����
	CALSSOPERATELOG_STUSPEAK		= 3,	//ѧ�����Բ���
	CALSSOPERATELOG_ATTENTSTU		= 4,	//��עѧ������
}MMS_CALSSOPERATELOG_TYPE;
//=======================================================================================