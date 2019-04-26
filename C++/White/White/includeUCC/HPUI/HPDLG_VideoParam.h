
#pragma once


//========================================================================
//������������Ƶ����������Դ��
//���ܣ��������ء�Զ�̻�������Ƶ�����ȡ��Աȶȡ����Ͷȡ�ɫ����٤����ֵ
//���ߣ���Ȩʤ
//ʱ�䣺2008-09-10
//˵����ͨ�����ûص�������ַʵ�ֲ����ص�
//--------------------------------------------------------------------------

#define ADJUST_TYPE_CANCEL		0	//�û�ȡ������
#define ADJUST_TYPE_APPLY		1	//�û�ҪӦ�õ�ǰ��Ƶ����
#define ADJUST_TYPE_PREVIEW		2	//�û��϶�������
#define ADJUST_TYPE_DEFAULT		3	//�û�ȡĬ��ֵ

#define ADJUST_MODE_INCREASE	0	//��������һ��
#define ADJUST_MODE_RESULT		1	//��������һ��

typedef struct HPUI_VIDEO_PARAM
{
	int iBrightness;	//����
	int iContrast;		//�Աȶ�
	int iSaturation;	//���Ͷ�
	int iHue;			//ɫ��
	int iGamma;			//٤��
	int iSharpen;		//��ֵ
	int iRed;
	int iGreen;
	int iBlue;
}Video_Param;


class AFX_EXT_CLASS HPDLG_VideoParamNotify
{
public:
	HPDLG_VideoParamNotify(){};
	~HPDLG_VideoParamNotify(){};
public:
	/* ������������Ƶ�����ص�����
	* �������ܣ��������ء�Զ�̻�������Ƶ�����ȡ��Աȶȡ����Ͷȡ�ɫ����٤����ֵ
	* ���������
	*		memberid,	��:��������ͨ������Ƶ
	*					����ͨ����:������������Ƶ
	*					����ֵ:����Զ����Ƶ
	*		iAdjustType ,0 :�û�ȡ������
	*					1 :�û�ҪӦ�õ�ǰ��Ƶ����
	*					2 :�û��϶�����������ʱ����Ԥ��ģʽ
	*					3 :�û�ȡĬ��ֵ������Ԥ��ģʽ
	*		iApplyMode,0:��������һ��
	*					1:��������һ��*/
	virtual void HPDLG_VideoParamSet(std::string memberid, unsigned long ulChannelID, 
		Video_Param iVideoParam, int iAdjustType, int iApplyMode) = 0;
};


class AFX_EXT_CLASS HPDLG_VideoParam
{	
public:
	HPDLG_VideoParam();
	virtual~ HPDLG_VideoParam(void);

	/* ��������������ʾ�Ի���
	 * pParent, �����ھ�� 
	 * bModal, �Ƿ���ģ̬��ʾ*/

	bool Create(HPDLG_VideoParamNotify& rNotify, bool bAdvanced = true, CWnd* pParent = NULL);
	void ShowWindow(CWnd* pParent, bool bModal = true);

	/* ǿ�ƹرնԻ���*/
	void Destroy();

public:

	/*����Ĭ�ϲ���
	* iVideoParam		������Ĭ��ֵ
	* iVideoParamMin	��������Сֵ
	* iVideoParamMax	���������ֵ*/
	void SetDefVideoParam(Video_Param iVideoParam, Video_Param iVideoParamMin, Video_Param iVideoParamMax);

	/*��ʼ����������
	* memberid			�û�ID
	* ulChannelIndex	ͨ��ID
	* iVideoParam		�����ĳ�ʼֵ*/
	void InitVideoParam(std::string memberid, unsigned long ulChannelIndex, Video_Param iVideoParam);

	/* ������������Ƶ�����ص�����
	 * �������ܣ��������ء�Զ�̻�������Ƶ�����ȡ��Աȶȡ����Ͷȡ�ɫ����٤����ֵ
	 * ���������
	 *		memberid,	��:��������ͨ������Ƶ
	 *					����ͨ����:������������Ƶ
	 *					����ֵ:����Զ����Ƶ
	 *		iAdjustType ,0 :�û�ȡ������
	 *					1 :�û�ҪӦ�õ�ǰ��Ƶ����
	 *					2 :�û��϶�����������ʱ����Ԥ��ģʽ
	 *					3 :�û�ȡĬ��ֵ������Ԥ��ģʽ
	 *		iApplyMode,0:��������һ��
	 *					1:��������һ��
	void HPDLG_SetVParamCallBack(void(CALLBACK* pCallBack)(std::string memberid, unsigned long ulChannelID, 
		Video_Param iVideoParam, int iAdjustType, int iApplyMode));*/

private:
	void*		m_hWnd;
};

