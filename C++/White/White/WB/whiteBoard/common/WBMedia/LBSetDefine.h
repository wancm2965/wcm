#pragma once
#include "AVDefine.h"

typedef struct _LBChannelSet	//ͨ������
{
	int nIndex;					//ͨ�����
	CString strChannelName;		//ͨ������
}LBChannelSet, *PLBChannelSet;

typedef struct _LBChannelManageSet	//ͨ����������
{
public:
	_LBChannelManageSet()
	: bDBPackets(FALSE), m_bEchoCancell(FALSE), m_bVAD(FALSE)
	, m_bImageDenoising(FALSE), m_bQuality(FALSE){}
	int nChannelIndex;					//ͨ�����
	HPLB_VideoSet VideoSet;				//��Ƶ����
//	HPLB_AudioSet AudioSet;				//��Ƶ����
	BOOL bDBPackets;					//�Ƿ�˫���ݴ�
	BOOL m_bEchoCancell;				//�Ƿ��������
	BOOL m_bVAD;						//�Ƿ������
	BOOL m_bImageDenoising;				//�Ƿ�ͼ��ȥ��
	BOOL m_bQuality;					//�Ƿ���������
}LBChannelManageSet, *PLBChannelManageSet;