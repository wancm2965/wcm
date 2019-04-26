#pragma once

class EncryptAddUnencrypt
{
public:
	EncryptAddUnencrypt(void);
	~EncryptAddUnencrypt(void);
private:
	CString m_keyOriginalData;//秘钥原文，必须为64bit的数据
public:
	//对EncryptData进行加密并保存
	void EncryptCode(CString EncryptData);
	//设进来的resultDatax指针最好为空
	void UnencryptCode(CString FilePath,char* resultData,int &resultDataLenth);
	void SetKeyOriginalData(CString KeyOriginalData);
};

