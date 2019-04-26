#pragma once
// [12/14/2009 lzy]
//云台相关定义
//停止命令发送任意一个都可以

#define CMD_UP			101		//向上
#define CMD_STOP_UP		201		
#define CMD_DOWN		102		//向下
#define CMD_STOP_DOWN   202		
#define CMD_LEFT		103		//向左
#define CMD_STOP_LEFT	203		
#define CMD_RIGHT		104		//向右
#define CMD_STOP_RIGHT  204		
#define CMD_DN			105		//变距近
#define CMD_STOP_DN     205		
#define CMD_DF			106		//变距远
#define CMD_STOP_DF     206		
#define CMD_FS			107		//变焦小
#define CMD_STOP_FS		207		
#define CMD_FB			108		//变焦大
#define CMD_STOP_FB     208		
#define CMD_HS			109		//光圈小
#define CMD_STOP_HS		209		
#define CMD_HB			110		//光圈大
#define CMD_STOP_HB     210		
#define CMD_AUTO		111		//自动
#define CMD_STOP_AUTO	211		
#define CMD_AE_ON		112		//自动曝光ON
#define CMD_AE_OFF		113		//自动曝光OFF
#define CMD_EV_SET		114		//设置曝光修正值
#define CMD_EV_SET_END	126		//设置曝光修正值结束

//添加预置点控制命令：
//预置点可能大于100个
#define CMD_AUTO_SCAN	218		//自动巡航 == CMD_PATROL_RUN
#define CMD_PREST_SET	301		//设置预置点
#define CMD_PREST_CALL	401		//清除预置点
#define CMD_PREST_CLR	501		//调用预置点
//-----------------------------

//巡航命令:Pattern
#define CMD_PATROL_START	216
#define CMD_PATROL_STOP		217
#define CMD_PATROL_RUN		218	//停止巡航可调用上面任一个控制命令

#define CMD_SET_SPEED	220
#define MAX_PREPOINT	128		//预置点个数


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
	PTZ_TYPE_COUNT,			//这里为枚举总数

}PTZ_PROTOCOL_TYPE;	//云台协议类型

//值对应的字符串结构体
typedef struct tagMATCH_STRING
{
	int		 data;
	LPCTSTR  text;	
}MATCH_STRING;

#define ADD_MATCH_STRING(x)   x, _T(#x)	//转换对应的字符串