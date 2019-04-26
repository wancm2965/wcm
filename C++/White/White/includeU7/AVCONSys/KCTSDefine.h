#pragma once
//定制业务
//=======================================================================================
// 数字化手术医疗病人信息
typedef struct _MEDICAID_PATIENT
{
	std::string strNumber;			//住院编号
	std::string strName;			//姓名
	unsigned short usGender;		//性别
	unsigned short usAge;			//年龄
	std::string strDepartment;		//入院科室
	std::string strSurgeon;			//手术医生
	std::string strSurgeonName;		//手术名称
	std::string strSurgeonDesc;		//手术备注
	_MEDICAID_PATIENT()
	{
		usGender = 0;
		usAge = 0;
	}
}MEDICAID_PATIENT;
//=======================================================================================


//=======================================================================================
// edu课堂模式名称列表
typedef struct _EDUROOMMODEINFO
{
	std::string	strEduRoomMode;//"1":直播课堂,"2":评课课堂,"3":离线课堂,"4":研讨会1,"5":研讨会2,"6":研讨会3,"7":研讨会4,"8":研讨会5
	std::string strEduRoomModeName;
}EDUROOMMODEINFO,*PEDUROOMMODEINFO;
typedef std::list<PEDUROOMMODEINFO> EDUROOMMODEINFOLST;

// 课堂操作日志类型定义
typedef enum _MMS_CALSSOPERATELOG_TYPE
{
	CALSSOPERATELOG_CLASSBEGINEND	= 0,	//上下课操作
	CALSSOPERATELOG_TEACH			= 1,	//主讲人授课操作
	CALSSOPERATELOG_POWERPOINT		= 2,	//演示课件操作
	CALSSOPERATELOG_STUSPEAK		= 3,	//学生发言操作
	CALSSOPERATELOG_ATTENTSTU		= 4,	//关注学生操作
}MMS_CALSSOPERATELOG_TYPE;
//=======================================================================================