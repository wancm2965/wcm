#pragma once

class EncryptAddUnencrypt
{
public:
	EncryptAddUnencrypt(void);
	~EncryptAddUnencrypt(void);
private:
	CString m_keyOriginalData;//��Կԭ�ģ�����Ϊ64bit������
public:
	//��EncryptData���м��ܲ�����
	void EncryptCode(CString EncryptData);
	//�������resultDataxָ�����Ϊ��
	void UnencryptCode(CString FilePath,char* resultData,int &resultDataLenth);
	void SetKeyOriginalData(CString KeyOriginalData);
};

