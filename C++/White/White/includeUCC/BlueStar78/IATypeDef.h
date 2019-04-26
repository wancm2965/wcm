/**********************************************************************
* @note
* Copyright 2009, Beijing BlueSky Technologies Inc.
* ALL RIGHTS RESERVED
* Permission is hereby granted to licensees of Beijing
* BlueSky Technologies Inc. Products to use or abstract
* this computer program for the sole purpose of implementing
* a product based on Beijing BlueSky Technologies Inc.
* No other rights to reproduce, use, or disseminate this
* computer program, whether in part or in whole, are granted.
* Beijing BlueSky Technologies Inc. makes no representation or
* warranties with respect to the performance of this computer
* program, and specifically disclaims any responsibility for
* any damages, special or consequential, connected with the
* use of this program.
* For details, see http://www.bstar.com.cn/
***********************************************************************/
/*****************************************************************************
*
* Copyright (C) BeiJing BlueStar Software Tech. Ltd., 2010
*
* $Id: BstarStandard.h 11 2010-12-08 16:14:25Z zhaoxiong $
*
* Bstar out IVS stream definition(1.3.06).
*
****************************************************************************/
#ifndef _BSTARSTANDARD_H_
#define _BSTARSTANDARD_H_

#define MAX_POLY_NUM 48
#define MAX_POLY_POINT_NUM 16
/*#define MAX_RULE_NUM 48*/
#define MAX_DEPTH_NUM 3

#define MAX_ALGEVENT_NUM 11
#define MAX_CHANNEL_NUM 16

//#define MAX_STDOBJ_NUM 4 //��������У�����

/** @struct MotPoint
* @brief points definition
*/
typedef struct
{
	unsigned short x; //�������, ������x
	unsigned short y; //�������, ������y
} MotPoint;


/** @struct MotQuad
* @brief quad definition
*/
typedef struct
{
	MotPoint topleft; //�������ϵ�
	MotPoint bottomright; //�������µ�
	MotPoint topright; //�������ϵ�
	MotPoint bottomleft; //�������µ�
} MotQuad;


/** @struct MotDeepT
* @brief depth definition
*/
typedef struct
{
	MotPoint top; ///< �������, �ϵ�����
	MotPoint bottom; ///< �������, �µ�����
} MotDeepT;



/** @struct MotRect
* @brief rectangle definition
*/
typedef struct
{
	MotPoint topleft; //�������ϵ�
	MotPoint bottomright; //�������µ�
} MotRect;



/** @struct MotObject
* @brief object structure definition
*/
typedef struct
{
	int id; //����ʵ����Ψһ��ʶ(ͬһ���͵���ͬ����ʵ��,��ͬһ�������ʶ��ͬ����ͬʵ����ŵ���)
	int attributes; //�������������bitmap�������복����
	MotQuad figure; //�����λ�ú��ⲿ����(�ı���)
} MotObject;



/** @struct MotPoly
* @brief area structure definition
*/
typedef struct
{
	unsigned char vertices; //(v1.3)ÿ������εĶ������
	unsigned char res[2]; //(v1.3)
	unsigned char zones; //(v1.3)�ڲ�����λ����(��ͨΪ0 ������������Ϊ��λ��λ���ų���λ��1+x ��)
	MotPoint points[MAX_POLY_POINT_NUM]; //����λ���б�
} MotPoly;


/** @struct MotDeep
* @brief depth structure definition
*/
typedef struct
{
	int depthHeight; // �궨��ʵ���˵ĸ߶�
	MotDeepT depthT[MAX_DEPTH_NUM]; // �궨���궨�ĸ�����
	MotDeepT    depthCheck;
} MotDeep;



// ��������ṹ
typedef struct
{
	MotPoly spot; //���ڵ����з�Χ(���������)
	unsigned char type; //̽�������
	unsigned char behavior; //�������Ϊ(ʵ�ʷ����Ľ��)
	short arg; //��Ϊ�Ĳ���
	char concerns; //(v1.3+)���õĸ���Ȥ��Ϊ��(bitmap),��ENTER|EXIT
	char res[3]; //(v1.3+)�����ֶ�(v1.3 ��ǰΪint res)
	int cnt; //����ĸ���
} MotIAEvent;


typedef enum
{
	ALL_RESET = 0,       //���ܹ��ܣ�ȫ������
	BORDER = 1,          //���ܹ��ܣ��ܽ籣��
	UNATTENDANT = 2,     //���ܹ��ܣ����������
	REMOVE = 3,          //���ܹ��ܣ���Ʒ����
	PERSONFIGHT = 4,           //���ܹ��ܣ���Ź���
	PERSONLINGER = 5,          //���ܹ��ܣ���Ա����
	PEOPLE_CLOSE = 6,    //���ܹ��ܣ���Ա�ӽ�
	TRAPWIRE = 8,        //���ܹ��ܣ����߼��
	MOT_PEOPLE_COUNT = 10, //���ܹ��ܣ�����ͳ��
	FACE_DETECT = 20,      //���ܹ��ܣ��������
	SENTRY_TRESPASS = 30,  //���ܹ��ܣ��ڱ����
	SENTRY_GUARDER = 31,   //���ܹ��ܣ��ڱ����
	ATM_PROTECTION = 40    //���ܹ��ܣ�ATM ����
} IVSType;


//�ܽ籣��
typedef enum
{
	BORDER_RESET = 0, //�ܽ籣��������
	ENTER = 1, //�ܽ籣��������
	ESCER = 2, //�ܽ籣�����Ƴ�
	SETTLE = 4, //�ܽ籣��������
	BORDER_APPEAR = 8, //�ܽ籣��������
	BORDER_DISAPPEAR = 16, //�ܽ籣������ʧ
	BORDER_APPER_ENTER = 32 //�ܽ籣��������ͳ������ֹ���ͬʱʵ��
} BorderBehavior;


//��������
typedef enum
{
	UNATTENDANT_RESET = 0, //���������
	APPEAR = 1 //��������޵���
} UnattendantBehavior;


//��Ʒ����
typedef enum
{
	REMOVE_RESET = 0, //��Ʒ����������
	DISAPPEAR = 1 //��Ʒ���������е���
} RemoveBehavior;


//����
typedef enum
{
	TRAPWIRE_RESET = 0, //���ߣ�����
	CROSS_ABOVE = 1, //���ߣ��������´�Խ
	CROSS_BELOW = 2, //���ߣ��������ϴ�Խ
	CROSS_TWO = 3, //���ߣ��������������
	TOUCH_ABOVE = 4, //���ߣ�������������
	TOUCH_BELOW = 8 //���ߣ�������������
} TrapwireBehavior;


//����ͳ��
typedef enum
{
	PEOPLECOUNT_RESET = 0, //����ͳ�ƣ�����
	EQUALS = 1, //����ͳ�ƣ�����
	MORE_THAN = 2, //����ͳ�ƣ�����
	LESS_THAN = 4 //����ͳ�ƣ�С��
} PeopleCountBehavior;


//�ڱ����
typedef enum
{
	SENTRYTRESPASS_RESET = 0, //�ڱ������Ȧ����������
	INTRUDE = 1 //�ڱ������Ȧ����������
} SentryTrespassBehavior;


typedef enum
{
	SENTRYGUARDER_RESET = 0, //�ڱ������Ȧ����������
	GUARD_OK = 4, //�ڱ������Ȧ����������
	GUARD_LEAVE = 8, //�ڱ������Ȧ���������
	GUARD_SQUAT = 16, //�ڱ������Ȧ�������׸�
	GUARD_LIE = 32 //�ڱ������Ȧ������˯��
} SentryGuarderBehavior;


//�������
typedef enum
{
	FACEDETECTIN_RESET = 0, //������⣺����
	EXIST = 1, //������⣺�������������(���޵���)
	NONEXIST = 2 //������⣺���û����������(���е���)
} FaceDetectBehavior;

typedef enum
{
	REFPOINT_BOTTOM = 0,//��׼λ�� �ײ�
	REFPOINT_MIDDLE = 1,//��׼λ�� 1/4��
	REFPOINT_TOP = 2//��׼λ�� 1/2��
}RefPoint;

/////////////////////////////////////////////////////////////



/*
��ͨ����-start
BSCP_DVX7-B(¼���ֳ�)
118
*/
typedef struct
{
	int res; ///< �ܽ籣��û����ͨ�����������ӿ�
} IntrusionCustomerParam;


typedef struct
{
	int res; ///< ����û����ͨ�����������ӿ�
} DirectionCustomerParam;


typedef struct
{
	int maxObjHeight; ///< �������߶�
	int minObjHeight; ///< ������С�߶�
	int timeToAlarm; ///< �������, ����ͣ��������ʱ��󱨾���5-1800��
} UnattendedCustomerParam;



/** @struct RemoveObjectDetec
* @brief taken detection parameters
*/
typedef struct
{
	int maxObjHeight; ///< �������߶�
	int minObjHeight; ///< ������С�߶�
	int timeToAlarm; ///< �������, ����ͣ��������ʱ��󱨾���5-1800��
} RemoveCustomerParam;


typedef struct
{
	int counterUpLimit; ///< ������������(0-10)
	int counterDownLimit; ///< ������������(0-10)
	int timeToAlarm; ///< �������ʱ�䣨1-600��
} PeopleCountCustomerParam;



typedef struct
{
	int res; ///< �������Ԥ������
} NoFaceDetecCustomerParam;


typedef struct
{
	int alarmStartTime; ///< �������ʱ��
} FightCustomerParam;


typedef struct
{
	MotPoint start; ///< ATM �߽����ö���
	MotPoint end; ///< ATM �߽����õ͵�
	int timeToAlarm; ///< �������ʱ��
} PeopleCloseCustomerParam;



typedef struct
{
	int modeSignal; ///< ģʽ�ź�
	int sceneSignal; ///< �����ź�
	MotPoly spot;    //�Ѹ�����  
} GuardCustomerParam;


typedef struct
{
	int lingertimetoAlarm; ///< �������ʱ��
} LingerCustomerParam;


typedef struct
{
	MotPoint cardArea[4]; ///< ����
	MotPoint kbArea[4]; //����
	MotPoint activeArea[4]; //�����
	MotPoint mark[2][2]; //�궨 �������µ�
	int timeToAlarm; ///< �������ʱ��
} AtmCustomerParam;

/*
��ͨ����-end
*/


//////////////////////////////////////////////////////////////////
/*
Ipc��·���ܲ���ר�ò���-start
BSCP_DVX7-B(¼���ֳ�)
0x0003.0519
*/
typedef struct
{
	int behavior; ///< �ܽ籣��û����ͨ�����������ӿ�1
} IntrusionCustomerParamEx;


typedef struct
{
	int res; ///< ����û����ͨ�����������ӿ�
} DirectionCustomerParamEx;


typedef struct
{
	int targetTextureThres; ///< Ŀ����������ֵ
	int peopleDetecFlag; ///< �˼���־
	int maxObjHeight; ///< �������߶�
	int minObjHeight; ///< ������С�߶�
	int timeToAlarm; ///< �������, ����ͣ��������ʱ��󱨾���5-1800��
	int timeToStopAlarm; ///< ��������ʱ��
} UnattendedCustomerParamEx;



/** @struct RemoveObjectDetec
* @brief taken detection parameters
*/
typedef struct
{
	int targetTextureThres; ///< Ŀ����������ֵ
	int peopleDetecFlag; ///< �˼���־
	int maxObjHeight; ///< �������߶�
	int minObjHeight; ///< ������С�߶�
	int timeToAlarm; ///< �������, ����ͣ��������ʱ��󱨾���5-1800��
	int timeToStopAlarm; ///< ��������ʱ��
} RemoveCustomerParamEx;


typedef struct
{
	int counterUpLimit; ///< ������������(0-10)
	int counterDownLimit; ///< ������������(0-10)
	int timeToAlarm; ///< �������ʱ�䣨1-600��
} PeopleCountCustomerParamEx;



typedef struct
{
	int res; ///< �������Ԥ������
} NoFaceDetecCustomerParamEx;


typedef struct
{
	int objectMinWidth; ///< ̽������С���
	int objectMinHeight; ///< ̽������С�߶�
	int objectMaxWidth; ///< ̽���������
	int objectMaxHeight; ///< ̽�������߶�
	int alarmEndTime; ///< �����ӱ�ʱ��
} FightCustomerParamEx;


typedef struct
{
	MotPoint start; ///< �߶�, ����
	MotPoint end; ///< �߶�, �׵�
} MotSeg;


typedef struct
{
	unsigned char vertices; ///< ÿ������εĶ������
	MotPoint points[MAX_POLY_POINT_NUM]; ///< ����λ���б�
} MotPolyArea;


typedef struct
{
	int maxHeight; ///< ̽�������߶�
	int minHeight; ///< ̽������С�߶�
	int atmMachineNums; ///< ATM ������
	int timeToAlarm; ///< �������ʱ��
	MotSeg atmOne; ///< ATM һ����
	MotSeg atmDouble; ///< ATM ������
	MotPolyArea atmFloor; ///< ATM ��������
} PeopleCloseCustomerParamEx;


typedef struct
{
	int modeSignal; ///< ģʽ�ź�
	int offSiteSensitivity; ///< �Ѹ�����������
	MotPoly spot;    //�Ѹ�����  
} GuardCustomerParamEx;

typedef struct
{
	unsigned short  endNum;     // �������� 
	unsigned short  changeTime; // ����ʱ�� 
}GuarderAppParamEx;


typedef struct {
	GuardCustomerParamEx cusparam;
	GuarderAppParamEx apparam;
	unsigned char poly_type; ///���������: 1: ����Σ�2������	3.����
	unsigned char res[3];
}GuardCustParamEx;


typedef struct
{
	int lingertimetoAlarm; ///< �������ʱ��
} LingerCustomerParamEx;


typedef struct
{
	int timeToAlarm; ///< �������ʱ��
	int timeToStopAlarm; ///< ��������ʱ��
	int activeSensitivity; ///< ������������
	int keyCardSensitivity; ///< ���������������
	int textureSensitivity; ///< Ŀ��������������
	MotPoint cardArea[4]; ///< ����
	MotPoint kbArea[4]; ///< ����
	MotPoint activeArea[4]; ///< �����
} AtmCustomerParamEx;

/*
��ͨ����-end
*/


//////////////////////////////////////////////////////////////////


/*
�߼�����-start
*/
typedef struct
{
	int objectMinWidth; ///< ̽������С���
	int objectMinHeight; ///< ̽������С�߶�
	int statictimeToAlarm; ///< ̽�������ֹʱ��
	int referencePoint;//��׼λ��
} IntrusionAdvanceParam;



typedef struct
{
	int objectMinWidth; ///< ̽������С���
	int objectMinHeight; ///< ̽������С�߶�
	int statictimeToAlarm; ///< ̽�������ֹʱ��
	int referencePoint;//��׼λ��
} DirectionAdvanceParam;


typedef struct
{
	int sceneLevels; ///< ��Ӱ���Ʊ�־
	int peopleDetecFlag; ///< �˼���־
	int frameInterval ; ///< ������
	int timeToStopAlarm; ///< ��������ʱ��
	int referencePoint;//��׼λ��
} UnattendedAdvanceParam;



/** @struct RemoveObjectDetec
* @brief taken detection parameters
*/
typedef struct
{
	int sceneLevels; ///< ��Ӱ���Ʊ�־
	int peopleDetecFlag; ///< �˼���־
	int frameInterval ; ///< ������
	int timeToStopAlarm; ///< ��������ʱ��
	int referencePoint;//��׼λ��
} RemoveAdvanceParam;


typedef struct
{
	int objectMinWidth; ///< ̽������С���
	int objectMinHeight; ///< ̽������С�߶�
	int statictimeToAlarm; ///< ̽�������ֹʱ��
	int referencePoint;//��׼λ��
} PeopleCountAdvanceParam;


typedef struct
{
	int res; ///< ������Ᵽ���ӿ�
	int referencePoint;//��׼λ��
} NoFaceAdvanceParam;


typedef struct
{
	int objectMinWidth;     ///< ̽������С���
	int objectMinHeight;    ///< ̽������С�߶�
	int objectMaxWidth;     ///< ̽������С���
	int objectMaxHeight;    ///< ̽������С�߶�
	int alarmEndTime;       ///< �����ӱ�ʱ��
	int referencePoint;//��׼λ��
} FightAdvanceParam;


typedef struct
{
	int maxHeight; ///< ̽�������߶�
	int minHeight; ///< ̽������С�߶�
	//int referencePoint;//��׼λ��
} PeopleCloseAdvanceParam;


typedef struct
{
	int       objectMinWidth;            ///< ̽������С���
	int       objectMinHeight;           ///< ̽������С�߶�
	int       statictimeToAlarm;         ///< ���ֹʱ��
	int referencePoint;//��׼λ��
} GuardAdvanceParam;


typedef struct
{
	int objectMinWidth; ///< ̽������С���
	int objectMinHeight; ///< ̽������С�߶�
	int statictimeToAlarm; ///< ���ֹʱ��
	int referencePoint;//��׼λ��
} LingerAdvanceParam;


typedef struct
{
	int timeToStopAlarm; ///< ��������ʱ��
	int frameInterval; ///< ������
	int referencePoint;//��׼λ��
} AtmAdvanceParam;

/*
�߼�����-end
*/




//////////////////////////////////////////////////////////////////////////////////



typedef struct
{
	unsigned char type; ///< ��������
	unsigned char behavior; ///< ����Ȥ����Ϊ
	short res[13]; ///< �����ֶ�
	union
	{
		int customerParams[32];                           ///< �û���ͨ����ʹ�õ��ڴ�
		IntrusionCustomerParam intrusionCusParam;         ///< ���ܹ��ܣ��ܽ籣��
		DirectionCustomerParam directionCusParam;         ///< ���ܹ��ܣ����߼��
		UnattendedCustomerParam unattendedCusParam;       ///< ���ܹ��ܣ����������
		RemoveCustomerParam removeCusParam;               ///< ���ܹ��ܣ���Ʒ����
		PeopleCountCustomerParam peopleCountCusParam;     ///< ���ܹ��ܣ�����ͳ��
		NoFaceDetecCustomerParam noFaceDetecCusParam;     ///< ���ܹ��ܣ��������
		FightCustomerParam fightCusParam;                 ///< ���ܹ��ܣ���Ź���
		PeopleCloseCustomerParam peopleCloseCusParam;     ///< ���ܹ��ܣ���Ա�ӽ�
		GuardCustomerParam guardCusParam;                 ///< ���ܹ��ܣ��ڱ����
		LingerCustomerParam lingerCusParam;               ///< ���ܹ��ܣ���Ա����
		AtmCustomerParam atmCusParam;                     ///< ���ܹ��ܣ�ATM ����
	} CustomerParams;

	int advancedParamEnable; ///< �Ƿ����߼�����

	union
	{
		int advancedParams[16]; ///< �߼�����ʹ�õ��ڴ�
		IntrusionAdvanceParam intrusionAdvanceParam;      ///< ���ܹ��ܣ��ܽ籣��
		DirectionAdvanceParam directionAdvanceParam;      ///< ���ܹ��ܣ����߼��
		UnattendedAdvanceParam unattendedAdvanceParam;    ///< ���ܹ��ܣ����������
		RemoveAdvanceParam removeAdvanceParam;            ///< ���ܹ��ܣ���Ʒ����
		PeopleCountAdvanceParam peopleCountAdvanceParam;  ///< ���ܹ��ܣ�����ͳ��
		NoFaceAdvanceParam noFaceAdvanceParam;            ///< ���ܹ��ܣ��������  //�ݲ�֧��
		FightAdvanceParam fightAdvanceParam;              ///< ���ܹ��ܣ���Ź���
		PeopleCloseAdvanceParam peopleCloseAdvanceParam;  ///< ���ܹ��ܣ���Ա�ӽ�
		GuardAdvanceParam guardAdvanceParam;              ///< ���ܹ��ܣ��ڱ����
		LingerAdvanceParam lingerAdvanceParam;            ///< ���ܹ��ܣ���Ա����
		AtmAdvanceParam atmAdvanceParam;                  ///< ���ܹ��ܣ�ATM ����
	} AdvanceParams;
} ConfigureRule;



typedef struct
{
	MotPoly spot; ///< �����������Ϣ
	unsigned int enable; ///< ���������õĹ����Ƿ�����
	unsigned int sensitivity; ///< ������
	unsigned int res[16]; ///< �����ֶ�
	ConfigureRule rule; ///< ������Ϣ
} ConfigureIAEvent;


typedef struct
{
	short version; ///< ������Ϣ��ʽ�İ汾�ţ���ǰΪ1
	short res; ///< �����ֶ�
	MotDeep depth; ///< ������궨����
	unsigned int polyCount; ///< ��ͨ���������������(1-255) Ŀǰ 48��
	ConfigureIAEvent polyList[MAX_POLY_NUM]; ///< ��������(polyCount ��)
} ConfigureIAFrame;


/*
 *�ӿڲ���*
 */

typedef struct 
{
	UCHAR start;  //�Ƿ����� 0:������ 1:����
	UCHAR bHour;  //��ʼʱ���Сʱ  0-23
	UCHAR eHour;    //����ʱ�� 0-23
	UCHAR bMinute;  //��ʼ�ķ�0-59
	UCHAR eMinute;  //�����ķ�0-59
	UCHAR sensity;  //������1-9
	UCHAR rec[2];   //
}VcaTimeSeg;

//
/*
* ���㷨��ص�Ӧ�ò���
*/
typedef struct
{
	unsigned short  endNum;     /**< �������� */
	unsigned short  changeTime; /**< ����ʱ�� */
}GuarderAppParam;


typedef struct 
{
	UCHAR vcaType;       /**< ����IVSType,0��ʾȫ�� */
	UCHAR start;         /**< �Ƿ�������0����������1������ */
	UCHAR playDisplay;   /**< �Ƿ���ʾ�طţ�0����������1������ */
	UCHAR senceDisplay;  /**< �Ƿ��ֳ���ʾ��0����������1������ */
	UCHAR videoSize;     /**< ������Ƶ�ֱ��ʵĴ�С��1��DCIF 2��CIF 
											3��1/2D1 4��D1 5��QCIF����ʱֻ֧��CIF */
#if 1 
	/*
	* Ŀǰֻ���ڱ��ܿ�Ӧ�ò������Ժ���������е����ܲ���
	*/
	UCHAR res[3];

	union
	{
		int AppParam[1];                    /*<  �㷨Ӧ�ò������£�Ŀǰ������4���ֽڣ��Ժ����չ*/
		GuarderAppParam  guarderAppParam;   /**< �ڱ�Ӧ�ò��� */
	}AlgAppParam;
#endif
	VcaTimeSeg time[7][6];/**< ���ʱ��Σ�7��ÿ��6�� */
}VcaAppParam;



typedef ConfigureIAFrame VcaAlgParam;


typedef struct 
{
	UCHAR count;                 //���õ��㷨���͸���
	UCHAR res[7];                // ����
	VcaAppParam appParam[MAX_ALGEVENT_NUM]; // Ӧ�ò���
	VcaAlgParam algParam;        //�㷨����
}VcaParam;

typedef struct  
{
	unsigned int channel;
	VcaParam vcaParams[MAX_CHANNEL_NUM];
}BscpVcaPara;

typedef struct
{
	unsigned long long channel;
	VcaParam vcaParams[64];
}BscpVcaParaEx;
//����Э��֧�֣�һ��ͨ����Ӧ4������(��ʱ)��ÿ�������Ӧ���������㷨


/** @struct CalibrationCheck
* �궨У��ṹ��
*/
typedef struct
{
	int channelID;
	MotDeep depth;
} CalibrationCheck;


struct VcaAppParamEx
{
  unsigned char   vca_type; //�����㷨���� 
  unsigned char   res[15];  
  union
  { 
    unsigned char   vcaParam[64]; 
    IntrusionCustomerParamEx       insPara;             ///< ���ܹ��ܣ��ܽ� 
    DirectionCustomerParamEx       directionCusParam;   ///< ���ܹ��ܣ����߼�� 
    UnattendedCustomerParamEx      unattendedCusParam;  ///< ���ܹ��ܣ���������� 
    RemoveCustomerParamEx          removeCusParam;      ///< ���ܹ��ܣ���Ʒ���� 
    PeopleCountCustomerParamEx     peopleCountCusParam; ///< ���ܹ��ܣ�����ͳ�� 
    NoFaceDetecCustomerParamEx     noFaceDetecCusParam; ///< ���ܹ��ܣ�������� 
    FightCustomerParamEx           fightCusParam;       ///< ���ܹ��ܣ���Ź��� 
    PeopleCloseCustomerParamEx     peopleCloseCusParam; ///< ���ܹ��ܣ���Ա�ӽ� 
    GuardCustParamEx           guardCusParam;       ///< ���ܹ��ܣ��ڱ���� 
    LingerCustomerParamEx          lingerCusParam;      ///< ���ܹ��ܣ���Ա���� 
    AtmCustomerParamEx             atmCusParam;         ///< ���ܹ��ܣ�ATM���� 
  }uIaPara;////���ܲ�������,  �μ���¼4��ConfigureRule��union�ĸ����������ͣ� 

  VcaTimeSeg time[7][6]; //���ʱ��Ρ�7��ÿ��6 �� 
};

struct VcaRule
{
  unsigned char vca_type;      //< �������� 
  unsigned char behavior;  ///<  ����Ȥ����Ϊ 
  unsigned char res[2];     ///<  ����ֶ� 
};

struct VcaPolyEvent
{
  unsigned char poly_type; ///��������� 
  unsigned char poly_info; ///���������߱�ʾ���߷���
  unsigned char res[2]; 
  MotPoly spot;    ///<  �����������Ϣ 
  unsigned int vca_rule_num; ///< ��������������ܹ��ܸ��� 
  VcaRule rule[10];  ///< ������Ϣ /// ��Ч����ȡ����vca_rule_num ֵ�� 
};

struct ScenceParam
{
  int       minObjWidth;                    ///< �������, ��СĿ���� 
  int       minObjHeight;                  ///< �������, ��СĿ��߶� 
  int       statictimeToAlarm;               ///< ���ֹʱ�� 
  int       dayStartTime;                    ///< ������ʼʱ�� 
  int       dayEndTime;                      ///< �������ʱ�� 
  int       shadowControl;                   ///< ��Ӱ���Ʊ�־ 
  int       updateRate;                      ///< ���������� 
  int       noiseControl;                    ///< ͼ���������Ʊ�־ 
  int       objDecSensitivity;                  ///< Ŀ���������ȣ���0, 1, 2��//Ĭ���� 2 
  int       objTraceSensitivity;             ///< Ŀ����������� 
  int       reciprocate;                     ///< �������Ŷ����� 
  int       lightControl;                    ///< ���ձ仯���Ʊ�־ 
};

struct ScenceVcaCfg
{
  unsigned char vcamode ;    ///����ģʽ 0:SCENCE, 1:ATM 
  unsigned char scencetype;     ///�������� 0:in door ,1: out side, 2:ATM Panel 
    unsigned char analysis_pattern;   ///����ģʽ0:Area , 1:behavior   
    unsigned char valid_polycount;    ///��Ч����θ��� < 8 
    VcaPolyEvent polylist[8];    ///����� 
    ScenceParam scencevca;       ///�μ���¼�� 
};

struct MotFlat 
{ 
  MotRect   flatRect[2];       ///< ƽ��궨���궨��:2�� 
}; 

struct CalibrationParam
{
  unsigned char calibration_type ; ///�궨���ͣ� ///0��DEEP ����궨��1��FLATƽ��궨 
  unsigned char res[3]; 
  union
  { 
    MotDeep deep ; ///����궨
    MotFlat   flat;  ///ƽ��궨
  }uMot;  
};

struct VcaCfgPara
{
  unsigned char enable; 
  unsigned char refPos;     ///��׼λ�� 
  unsigned char vcacount;     /// ���Ϊ 16 ��ǰ����Ч�������͸��������ֲ��� 
  unsigned char scence_num;   ///default == 4�����±�� 
  unsigned char enalbleScenceId;   /// scence[]��ʹ�ܵĲ��� Id  
  unsigned char calibrate_id;     ///�궨����ID 
  unsigned char res[6]; 
  CalibrationParam calibrate_param[2];  ///�궨���� ///��Ч ID  �� calibrate_id  ������ 
  VcaAppParamEx param[16];            //���������͵����ò�������Ч������ vcacountָ�� 
  ScenceVcaCfg scence[6];               ///���֧��6�ֳ������÷�ʽ,���������scence_numָ����  
};

struct VcaParamIpc
{
  unsigned int count;     ///ͨ�������� 
  VcaCfgPara cfgEx;       ///ͨ��������IPCĬ��Ϊ 1��DVRΪʵ��ͨ������Ĭ�� 16��������ʵ�֣� 
};

struct VcaParamDvr
{
  unsigned int count;     ///ͨ�������� 
  VcaCfgPara cfgEx[16];   ///ͨ��������IPCĬ��Ϊ 1��DVRΪʵ��ͨ������Ĭ�� 16��������ʵ�֣� 
};

//���ܼ�����
#ifndef _ENUM_VCA_ALGTYPE_
#define _ENUM_VCA_ALGTYPE_

typedef enum   
{   
  VCA_DECT_MIN = 0, //��Чֵ,Сֵ�ػ���(�������ܲ�ѯʱ��ʾȫ����������)   
  VCA_VIDEO_TOTALNUM, //����ͳ��   
  VCA_VIDEO_LEFTOBJECT, //������   
  VCA_VIDEO_STUMLINE, //����   
  VCA_VIDEO_PRIMITER, //�ܽ籣��   
  VCA_VIDEO_NONFACE, //���������   
  VCA_VIDEO_LOSSOBJECT, //��ʧ���   
  VCA_VIDEO_SENTRY , //���ڼ��   
  VCA_VIDEO_CONDUCT,	//���м��
  VCA_AUDIO_DETECT,  //��Ƶ����   
  VCA_DECT_MAX       //��Чֵ,��ֵ�ػ���   
}VCA_ALGTYPE;   

#endif
//���ܼ������(ע��:�������õĸ�������߽緶ΧΪ[(2,2), (350,286)],Ҳ����˵��2 ���� �ı߽�)
#ifndef _ENUM_VCA_RECT_
#define _ENUM_VCA_RECT_

typedef enum   
{   
  VCA_RECTWIDTH = 350,  //�������õ�����ϵX ���ֵ   
  VCA_RECTHEIGHT = 280, //�������õ�����ϵY ���ֵ   
  VCA_RECTBORDERX = 2,  //ˮƽ����ı߽�   
  VCA_RECTBORDERY = 2   //��ֱ����ı߽�   
};

#endif

//������
#ifndef _TAG_BSCP_INTERF_POINT_
#define _TAG_BSCP_INTERF_POINT_

typedef struct tagBSCP_INTERF_POINT
{
  USHORT x;				//������
  USHORT y;				//������
}BSCP_INTERF_POINT, *PBSCP_INTERF_POINT;
typedef BSCP_INTERF_POINT  VCA_POINT;
typedef PBSCP_INTERF_POINT  PVCA_POINT;

#endif

#ifndef _TAG_VCA_LINE_
#define _TAG_VCA_LINE_

typedef struct tagVCA_LINE 
{   
  VCA_POINT begin; //���   
  VCA_POINT end;   //�յ�   
}VCA_LINE, *PVCA_LINE;   

#endif

//��Է�CIF��ʽ��Ҫ���н��д�С����ӳ��(����ʱ) 
#ifndef _TAG_VCA_RECT_
#define _TAG_VCA_RECT_

typedef struct tagVCA_RECT   
{   
  VCA_POINT left_up;    //���Ͻ�����   
  VCA_POINT right_down; //���½�����   
}VCA_RECT, *PVCA_RECT;   

#endif

#ifndef _TAG_VCA_QUAD_
#define _TAG_VCA_QUAD_

typedef struct tagVCA_QUAD   
{   
  VCA_POINT points[4]; //�����Ͻǿ�ʼ˳ʱ��   
}VCA_QUAD, *PVCA_QUAD;   

#endif

//ͨ��������� 
#ifndef _TAG_VCA_SPOT_
#define _TAG_VCA_SPOT_

typedef struct tagVCA_SPOT   
{   
  UCHAR vertices;       //2-16 
  UCHAR enabled;        //YES(1)|NO(0) 
  UCHAR type;           //��������(1) |  ��������(2) |  �ų�����(3) 
  UCHAR concerns;       //��ע����Ϊ��(bitmap)��ȡֵ����������Ϣ֡��ʽ�� 
  VCA_POINT points[16]; //�����Ͻǿ�ʼ˳ʱ��   
}VCA_SPOT, *PVCA_SPOT;   

#endif

//ͨ����Ϊ���� 
typedef struct tagVCA_PARAMS_78  
{ 
  USHORT delay_time; //����ʱ��(��)��0δ���� 
  USHORT alarm_time; //��������ʱ��(��),10-600,Ĭ��Ϊ30����������/��ʧ����Ч 
  USHORT lo_limit; //����(0-10)��0δ����, //������ͳ����Ч 
  USHORT hi_limit; //����(0-10)��0δ���ã�//������ͳ����Ч 
  USHORT aux_port; //�����������(1-8)��0δ���ã����������͸�����Ч 
  USHORT objectMinWidth;         /* �ܽ�-����-���� */
  USHORT objectMinHeight;        /* �ܽ�-����-���� */
  USHORT statictimeToAlarm;       /* �ܽ�-����-���� */
  USHORT maxObjHeight;          /* ����-��ʧ */
  USHORT minObjHeight;          /* ����-��ʧ */
  unsigned char removeTreeSwayFlag;    /* ����-��ʧ */
  unsigned char peopleDetecFlag;        /* ����-��ʧ */
  unsigned char frameInterval;           /* ����-��ʧ */
  unsigned char res[9];
} VCA_PARAMS_78, PVCA_PARAMS_78; 

//ͨ�ö������ 
#ifndef _TAG_VCA_OBJECT_
#define _TAG_VCA_OBJECT_

typedef struct tagVCA_OBJECT   
{ 
  USHORT object_height;       //��׼̽����ʵ�ʸ߶�(��λ: cm)   
  UCHAR res;   
  UCHAR object_depth;         //Ŀ�꾰�����������3--����,1--����   
  VCA_RECT object_figures[3]; //��׼̽��������������   
} VCA_OBJECT, PVCA_OBJECT; 

#endif

#ifndef _TAG_VCA_TIME_
#define _TAG_VCA_TIME_

typedef struct tagVCA_TIME   
{   
  UCHAR bMinute; //��ʼʱ��ķ� 0-59   
  UCHAR eMinute; //����ʱ��ķ� 0-59   
  UCHAR bHour;   //��ʼʱ���Сʱ 0-23   
  UCHAR eHour;   //����ʱ���Сʱ 0-23   
  UCHAR sensitivity; //����������0-9,Ĭ��ֵ�Զ�0(ǰ���뱳���Ĳ���)   
  UCHAR tolerance;   //������ݶ�0-9,Ĭ��ֵ5(����ߴ�仯�ٷֱ�%)   
  UCHAR rev;         //����   
  UCHAR start;       //�Ƿ����� 0:������  1:����   
}VCA_TIME, *PVCA_TIME;   

#endif

//ĳһ���ͷ����㷨   
typedef struct tagVCA_ALG_78   
{   
  UCHAR enabled;    //YES(1)|NO(0) 
  UCHAR algType;    //�㷨���ͣ�ȡֵVCA_ALGTYPE 
  UCHAR res[2];    //������ 
  VCA_PARAMS_78 defined_params;    //��Ϊ������ 
  VCA_OBJECT    defined_object; //��������� 
  VCA_SPOT defined_spots[4];    //���������   
}VCA_ALG_78, *PVCA_ALG_78;   

typedef struct tagVCA_NONFACE_78   
{   
  VCA_PARAMS_78 defined_params;     //��Ϊ������ 
  VCA_OBJECT    defined_object;  //��������� 
  VCA_SPOT defined_spots[4];     //���������   
}VCA_NONFACE_78, *PVCA_NONFACE_78;   

//�ܽ籣����ֵ����   
#define VCA_PERIMTER_AREA_NUM (4) //�ܽ����������   
#define VCA_STUM_LINE_NUM (4) //���������   
#define VCA_LEFT_AREA_NUM (4) //�����������   
#define VCA_LOSS_AREA_NUM (4) //��ʧ�������   
#define VCA_SENTRY_AREA_NUM (1) //�������������  

//�ܽ籣��   
typedef struct tagVCA_PERIMETER_78   
{   
  VCA_PARAMS_78 defined_params;    //��Ϊ������ 
  VCA_OBJECT    defined_object; //��������� 
  VCA_SPOT defined_spots[4];    //���������   
}VCA_PERIMETER_78, *PVCA_PERIMETER_78;   

//����������   
typedef struct tagVCA_STUMLINE_78   
{   
  VCA_PARAMS_78   defined_params;   //��Ϊ������ 
  VCA_OBJECT    defined_object;  //��׼̽�������� 
  VCA_SPOT defined_spots[4];     //��������   
}VCA_STUMLINE_78, *PVCA_STUMLINE_78;  

//����ͳ��   
typedef struct tagVCA_TOTALPEPSONNUM_78   
{   
  VCA_PARAMS_78   defined_params;  //��Ϊ������ 
  VCA_OBJECT    defined_object; //��׼̽�������� 
  VCA_SPOT defined_spots[4];    //������������   
}VCA_TOTALPEPSONNUM_78, *PVCA_TOTALPEPSONNUM_78;   

//��������   
typedef struct tagVCA_LEFTOBJECT_78   
{   
  VCA_PARAMS_78 defined_params;    //��Ϊ������ 
  VCA_OBJECT    defined_object; //��׼̽�������� 
  VCA_SPOT defined_spots[4];    //������������   
}VCA_LEFTOBJECT_78, *PVCA_LEFTOBJECT_78;   

//��ʧ��Ʒ���   
typedef struct tagVCA_LOSSOBJECT_78   
{   
  VCA_PARAMS_78 defined_params; //��Ϊ������ 
  VCA_OBJECT    defined_object; //��׼̽�������� 
  VCA_SPOT defined_spots[4]; //������������   
}VCA_LOSSOBJECT_78, *PVCA_LOSSOBJECT_78;   

//����   
typedef struct tagVCA_SENTRY_78   
{   
  VCA_PARAMS_78 defined_params; //��Ϊ������ 
  VCA_OBJECT    defined_object; //��׼̽�������� 
  VCA_SPOT defined_spots[4]; //��������,����������[0]/������[1]/�ų���[2-3]   
}VCA_SENTRYOBJECT_78, *PVCA_SENTRYOBJECT_78; 

//ĳһͨ�������ܷ����ܽṹ 
typedef struct tagVCA_CFG_78   
{   
  UCHAR start; //�Ƿ�����   
  UCHAR playDisplay; //�Ƿ�ط���ʾ,0-������,1-����   
  UCHAR sceneDisplay; //�Ƿ��ֳ���ʾ,0-������,1-����   
  UCHAR videoSize; //������Ƶ�ķֱ��ʴ�С,1:DCIF 2:CIF 3:1/2D1 4:D1 4:QCIF,��ʱֻ֧��CIF   
  UINT rev; //����   
  VCA_TIME time[7][4]; //���ʱ���,һ��7��,0-7��ʾ����,��һ...������   
  //�����㷨 
  VCA_ALG_78 alg[8];    //ÿͨ����������4 �ֲ�ͬ�����㷨,ͨ�� alg[i].enabled�ж� 
  //��ǰ��ʹ��[0]   
}VCA_CFG_78, *PVCA_CFG_78; 

typedef struct tagVCA_ALGDECTVIDEOPARA_78   
{   
  char channel; //ͨ����ţ�1-16 
  char res[3];    // 
  VCA_CFG_78 vcaPara;
}VCA_ALGDECTVIDEOPARA_78, *PVCA_ALGDECTVIDEOPARA_78;

#ifndef _TAG_BSRALGDECT_LINE_
#define _TAG_BSRALGDECT_LINE_

typedef struct tagBSRALGDECT_LINE   
{   
  BSCP_INTERF_POINT begin; //���   
  BSCP_INTERF_POINT end; //�յ�   
}BSRALGDECT_LINE, *PBSRALGDECT_LINE;

#endif

//������������
#ifndef _TAG_BSRALGDECT_PER_STUMLINE_EX_
#define _TAG_BSRALGDECT_PER_STUMLINE_EX_

typedef struct tagBSRALGDECT_PER_STUMLINE_EX
{
  BSRALGDECT_LINE object_area; //��������
  INT direction;     //����,1-����, 2-����, 0-��ʾ��Ч
}BSRALGDECT_PER_STUMLINE_EX, *PBSRALGDECT_PER_STUMLINE_EX;

#endif

#define MAX_STDOBJ_NUM 3 //����������

//��Է�CIF��ʽ��Ҫ���н��д�С����ӳ��
#ifndef _TAG_BSRALGDECT_RECT_
#define _TAG_BSRALGDECT_RECT_

typedef struct tagBSRALGDECT_RECT
{
  VCA_POINT left_up;		//���Ͻ�����
  VCA_POINT right_down;	//���½�����
}BSRALGDECT_RECT, *PBSRALGDECT_RECT;

#endif

#ifndef _TAG_BSRALGDECT_OBJECT_
#define _TAG_BSRALGDECT_OBJECT_

typedef struct tagBSRALGDECT_OBJECT
{
  USHORT object_height; //��׼̽����ʵ�ʸ߶�(��λ: cm)  
  UCHAR res; 
  UCHAR object_depth; //Ŀ�꾰�1 ��ʾ�� object_size ��Ч��   
  BSRALGDECT_RECT object_figures[MAX_STDOBJ_NUM]; //��׼̽�����С   
}BSRALGDECT_OBJECT, *PBSRALGDECT_OBJECT;

#endif

//����������
#ifndef  _TAG_BSRALGDECT_STUMLINE_EX_
#define  _TAG_BSRALGDECT_STUMLINE_EX_

typedef struct tagBSRALGDECT_STUMLINE_EX
{     
  BSRALGDECT_OBJECT  defined_size;//��׼̽��������
  BSRALGDECT_PER_STUMLINE_EX Defined_area[BSRALGDECT_STUM_LINE_NUM]; //��������
}BSRALGDECT_STUMLINE_EX, *PBSRALGDECT_STUMLINE_EX;

#endif

#endif /* _BSTARPARAM_H_ */


//��������  qinyf  2011.5.24
/************************************************************************/
/*     ���汾���ܷ�Ϊ�������֣�Ӧ�ò���App ���ܲ�������Alg ��
1    ��dvr��ͨ�ţ�һ�ΰ�16��ͨ����������Ϣȫ����ȡ
2    ÿ��ͨ����������Ϣ��������˵�������� app��alg��
     app��洢��ȫ��10��������Ϣ��Ӧ����� �����ĸ����õ�
		 alg���ִ洢�����ͨ�������ܲ������������ܲ���������Ϊ���������֣�
		 ���֧��48������ÿ������ָ��Ϊ��ͬ������������������ͬ�����ܲ�����Ϣ*/
/************************************************************************/