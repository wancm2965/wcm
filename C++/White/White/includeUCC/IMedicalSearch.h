//========================================================================
//�������ļ���ѯ�ӿ�
//���ܣ�avmsplayer ��ȡԶ��¼���ļ��б���Ϣ
//���ߣ��� ��
//ʱ�䣺2011-2-16
//˵����
//--------------------------------------------------------------------------

#pragma once

//��ѯ�ص�����
class HPDLG_API IMedicalSearchNotify
{
public:
	IMedicalSearchNotify(){};
	virtual~ IMedicalSearchNotify(){};
	
	//������Ƶ�ļ�(�ļ�����ʽ)
	virtual void PlayAVMSFile(CString strFilePath, CString strFileNameKey) = 0;

	virtual void OpenPlayAVMFileEnd() = 0;
};

//��ѯ�Ի���
class AFX_EXT_CLASS HPDLG_MedicalSearch
{
public:
	HPDLG_MedicalSearch(void){};
	virtual~ HPDLG_MedicalSearch(void){};

	// ��ʾ/���ضԻ���
	virtual void ShowWindow(int nCmdShow) = 0;

	//��ǰ����Ƿ��Ѵ�
	virtual BOOL IsWindowShow() = 0;

	virtual CString PlayNextFile(CString strFileNameKey) = 0;
};	

// �����Ի���
HPDLG_API HPDLG_MedicalSearch* CreateMedicalSearchDlg(IMedicalSearchNotify& rNotify ,CWnd* pParent = NULL);

// ���ָ��
HPDLG_API HPDLG_MedicalSearch* GetMedicalSearchDlg(void);

// ���ٶԻ���
HPDLG_API void DestroyMedicalSearchDlg(void);
