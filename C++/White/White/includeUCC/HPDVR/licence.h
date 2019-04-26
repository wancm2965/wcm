#ifndef __LICENCE_H__
#define __LICENCE_H__

#define MAX_LICENCE_STR_LEN			17
#define MAX_MAC_LEN					6

//����ֵ  0: ���ͨ��
//����ֵ  1: ϵ�к���Ч
//����ֵ  2: ���ڹ���
//����ֵ  3: û�������
//����ֵ  4: ϵ�кſջ���ϵ�кų��Ȳ���(16λ)
//����ֵ  5: MAC��ַ���Ͳ���
extern "C" __declspec(dllexport) int CheckLicence(
			unsigned char licence_str[MAX_LICENCE_STR_LEN],
			unsigned char mac_addr[MAX_MAC_LEN]);

//����ֵ -1: û�в������
//����ֵ  N: ͨ����N
extern "C" __declspec(dllexport) int GetDVRCHCount(void);

#endif
