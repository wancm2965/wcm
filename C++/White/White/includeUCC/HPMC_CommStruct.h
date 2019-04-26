#pragma once

typedef enum {
	CFSRC_TREE	= 0,
	CFSRC_MAP	= 1,
	CFSRC_AV	= 2,
	CFSRC_MEM	= 3,
	CFSRC_TM	= 99,
	CFSRC_TVTREE  = 100,
	SFSRC_TVSCREEN =101,

}CFSRC;

typedef struct _HPMC_DROP
{
    int				Type;	 		//0���������1����ͼ��2������Ƶ���Ŵ��� 3:������Ա��
    HWND        	hWnd;    		//�϶�Դ�ľ�����������ϵĽڵ�HTREEITEM ���ߵ�ͼ�ľ������AV����ľ��
    int				AreaID;	 		//������ID�� ����������(0:����1���豸��2��ͨ��)
    char			MemberID[255];	//����������
    unsigned long	ChannelID;		//����������
}HPMC_DROP;
