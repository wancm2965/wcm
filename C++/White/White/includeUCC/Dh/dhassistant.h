
#ifndef DHSYSABLE_H
#define DHSYSABLE_H

typedef enum
{
	ABILITY_WIRELESS_CFG = 18,			//wireless��������
	ABILITY_DEVALL_INFO = 26,			//�豸�������б�
	ABILITY_CARD_QUERY = 0x100,			//���Ų�ѯ����

} DH_SYS_ABILITY;


/************************************************************************
 * �ṹ�嶨��
************************************************************************/

// �豸֧�ֹ����б�
typedef struct 
{
	char IsFTPEnable;					//FTP
	char IsSMTPEnable;					//SMTP
	char IsNTPEnable;					//NTP
	char IsAutoMaintainEnable;			//�Զ�ά��
	char IsVideoCoverEnable;			//�����ڵ�
	char IsAutoRegisterEnable;			//����ע��
	char IsDHCPEnable;					//DHCP
	char IsUPNPEnable;					//UPNP
	char IsCommSnifferEnable;			//����ץ��
	char IsNetSnifferEnable;			//����ץ��
	char IsBurnEnable;					//��¼����
	char IsVideoMatrix;					//��Ƶ����
	char IsRev[244];					//�����ֶ�

}DH_DEV_ENABLE_INFO;

// ���Ų�ѯ�����ṹ��
typedef struct 
{
	char  IsCardQueryEnable;
	char  iRev[3];
} DH_CARD_QUERY_EN;

// wireless�����ṹ��
typedef struct 
{
	char  IsWirelessEnable;
	char  iRev[3];
}DH_WIRELESS_EN;

#endif // DHSYSABLE_H