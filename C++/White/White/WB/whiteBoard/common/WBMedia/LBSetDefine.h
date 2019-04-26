#pragma once
#include "AVDefine.h"

typedef struct _LBChannelSet	//通道设置
{
	int nIndex;					//通道编号
	CString strChannelName;		//通道名称
}LBChannelSet, *PLBChannelSet;

typedef struct _LBChannelManageSet	//通道管理设置
{
public:
	_LBChannelManageSet()
	: bDBPackets(FALSE), m_bEchoCancell(FALSE), m_bVAD(FALSE)
	, m_bImageDenoising(FALSE), m_bQuality(FALSE){}
	int nChannelIndex;					//通道编号
	HPLB_VideoSet VideoSet;				//视频设置
//	HPLB_AudioSet AudioSet;				//音频设置
	BOOL bDBPackets;					//是否双包容错
	BOOL m_bEchoCancell;				//是否回音消除
	BOOL m_bVAD;						//是否静音检测
	BOOL m_bImageDenoising;				//是否图像去噪
	BOOL m_bQuality;					//是否质量优先
}LBChannelManageSet, *PLBChannelManageSet;