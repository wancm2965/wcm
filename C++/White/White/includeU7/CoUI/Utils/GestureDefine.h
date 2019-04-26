#pragma once
//-------------------------------数据类型------------------------------------

//手势 类型
typedef enum EGestureType
{
	GestureType_none,		
	GestureType_translation,	
	GestureType_direction	
}EGestureType;

//方向手势的 方向
typedef enum EGestureDirection
{
	GestureDirection_none		=0,	//0x00000000	//不定
	GestureDirection_right,			//0x00000001	//右方
	GestureDirection_right_up,		//0x00000002	//右上角
	GestureDirection_up,			//0x00000003	//上方
	GestureDirection_left_up,		//0x00000004	//左上角
	GestureDirection_left,			//0x00000005	//左方
	GestureDirection_left_down,		//0x00000006	//左下角
	GestureDirection_down,			//0x00000007	//下方
	GestureDirection_right_down		//0x00000008	//右下角
}EGestureDirection;
	
// Flags for CContainerUI::GetGestureFlag()
#define GSFLAG_TRANS_HOR		0x0001//支持水平滑动
#define GSFLAG_TRANS_VER		0x0002//支持竖直滑动
#define GSFLAG_TRANS_INERTIAL	0x0004//支持惯性滑动
#define GSFLAG_TRANS_FORBID		0x0008//禁止滑动分析
#define GSFLAG_TRANS		(GSFLAG_TRANS_HOR | GSFLAG_TRANS_VER)
#define GSFLAG_DIRECTION		0x0020//支持方向性分析
#define GSFLAG_DIRECTION_FORBID	0x0040//禁止方向性分析

//平移手势状态
#define TranslationState_none			 0x00000000
#define TranslationState_begin			 0x00000001
#define TranslationState_ing			 0x00000002
#define TranslationState_end			 0x00000004
#define TranslationState_inertia_begin	 0x00000008
#define TranslationState_inertia_ing	 0x00000010
#define TranslationState_inertia_end	 0x00000020
