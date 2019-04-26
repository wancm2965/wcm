#include "StdAfx.h"
#include "EncryptAddUnencrypt.h"
#include "EncryptCommon.h"


EncryptAddUnencrypt::EncryptAddUnencrypt(void)
{
	m_keyOriginalData = _T("AVCON_WB");
}


EncryptAddUnencrypt::~EncryptAddUnencrypt(void)
{
}

void EncryptAddUnencrypt::EncryptCode( CString EncryptData)
{
	int key1ValidFlag;
	char subEncodeKey1[16][48];//���ܵ�����Կ
	char subDecodeKey1[16][48];//���ܵ�����Կ
	char* oriDataInBit = NULL;
	char* oriDataInHex = NULL;
	char* resultDataInBit = NULL;
	char* resultDataInHex = NULL;
	int oriDataInBitLength=0;
	char* PKeyCiphertextData = NULL;//��Կ�Ķ�����
	PKeyCiphertextData = checkValidKey(m_keyOriginalData,ASC2KEY,m_keyOriginalData.GetLength(),&key1ValidFlag);

	if (!checkValidPlainText(EncryptData))
	{
		AfxMessageBox(_T("���ĳ��ȷǷ�������������!"));
		return;
	}
	//����

	//���ɼ��ܼ����ܵ�����Կ
	genAllSubKey(PKeyCiphertextData,subEncodeKey1,subDecodeKey1);	

	//������ת��Ϊ��������ʽ
	oriDataInBit=CStringToBit_64Grouped(EncryptData,&oriDataInBitLength);

	//����������ʽ���ı�Ϊ16������ʽ
	oriDataInHex=BinToHex_4Grouped(oriDataInBit,oriDataInBitLength);


	//�Զ�������ʽ���Ľ��м��ܵõ�����������
	resultDataInBit=DESEncode(oriDataInBit,oriDataInBitLength,subEncodeKey1);

	//����������ʽ���ı�Ϊ16������ʽ
	resultDataInHex=BinToHex_4Grouped(resultDataInBit,oriDataInBitLength);

	CString strFilePath;
	//��������ļ�
	CFileDialog dlgFile(FALSE, _T("wbe"),_T("AVCONWB"), OFN_HIDEREADONLY , _T("UCB Files (AVCONWB.wbe)|AVCONWB.wbe||"), NULL, 0, FALSE);

	dlgFile.m_ofn.lpstrTitle = _T("����Ϊ");

	if (dlgFile.DoModal() == IDOK)
	{
		strFilePath = dlgFile.GetPathName();
		if (PathFileExists(strFilePath))//����ļ����ھ�ɾ������ļ�
		{
			//DeleteFile(T2A(strFilePath.GetBuffer(strFilePath.GetLength())));
			DeleteFile(strFilePath);
		}
		CFile file;

		if (strFilePath == "")
		{
			return ;
		}
		if (!file.Open(strFilePath, CFile::modeCreate|CFile::modeWrite))
		{
			return ;
		}
		file.Write(resultDataInHex,oriDataInBitLength);
		file.Close();
	}
	else
	{
		return;
	}

	if (NULL != PKeyCiphertextData)
	{
		delete PKeyCiphertextData;
		PKeyCiphertextData = NULL;
	}
	if (NULL != oriDataInBit)
	{
		delete oriDataInBit;
		oriDataInBit = NULL;
	}
	if (NULL != oriDataInHex)
	{
		delete oriDataInHex;
		oriDataInHex = NULL;
	}
	if (NULL != resultDataInBit)
	{
		delete resultDataInBit;
		resultDataInBit = NULL;
	}
	if (NULL != resultDataInHex)
	{
		delete resultDataInHex;
		resultDataInHex = NULL;
	}

}

void EncryptAddUnencrypt::SetKeyOriginalData( CString KeyOriginalData )
{
	m_keyOriginalData = KeyOriginalData;
}

void EncryptAddUnencrypt::UnencryptCode( CString FilePath,char* resultData,int &resultDataLenth )
{
	//wchar_t szTitel[MAX_PATH];
	//MultiByteToWideChar(CP_ACP, 0, (LPCSTR)FilePath.GetBuffer(), -1, szTitel, MAX_PATH);
	// ��ȡ�ļ�
	CFile file;
	
	if (!file.Open(FilePath, CFile::modeRead))
	{
		resultData = NULL;
		return ;
	}
	ULONGLONG lFileLength = file.GetLength(); //�ļ�����
	if( lFileLength < 256)
	{
		resultData = NULL;
		file.Close();
		return ;
	}
	char* readText = new char[256];
	memset(readText,0,256);
	file.Seek(0, CFile::begin);
	file.Read((char *)readText,256);
	file.Close();
	//����
	char* oriDataInBit = NULL;
	char* resultDataInBit = NULL;
	char* resultDataInHex = NULL;
	char subEncodeKey[16][48];//���ܵ�����Կ
	char subDecodeKey[16][48];//���ܵ�����Կ
	int keyValidFlag;
	CString KeyOriginalData = _T("AVCON_WB");
	char* PKeyCiphertextData = NULL;//��Կ�Ķ�����
	PKeyCiphertextData = checkValidKey(KeyOriginalData,ASC2KEY,KeyOriginalData.GetLength(),&keyValidFlag);
	//���ɼ��ܼ����ܵ�����Կ
	genAllSubKey(PKeyCiphertextData,subEncodeKey,subDecodeKey);	

	//��16��������ת��Ϊ��������ʽ
	CString tempReadText(readText);
	oriDataInBit=CStringInHexToBit_64Grouped(tempReadText,&resultDataLenth);

	//�Զ�������ʽ���Ľ��н��ܵõ�����������
	resultDataInBit=DESEncode(oriDataInBit,resultDataLenth,subDecodeKey);

	//����������ʽ���ı�Ϊ16������ʽ
	resultDataInHex=BinToHex_4Grouped(resultDataInBit,resultDataLenth);


	//��16������ʽ���ı�ΪCString��ʽ
	CString temp4=HexToCString(resultDataInHex,resultDataLenth/4);
	
	resultDataLenth = temp4.GetLength();
	
	WideCharToMultiByte( CP_ACP, 0, temp4.GetBuffer(), -1, resultData, 64, NULL, NULL );
	temp4.ReleaseBuffer();

	if (NULL != PKeyCiphertextData)
	{
		delete PKeyCiphertextData;
		PKeyCiphertextData = NULL;
	}
	if (NULL != oriDataInBit)
	{
		delete oriDataInBit;
		oriDataInBit = NULL;
	}
	if (NULL != resultDataInBit)
	{
		delete resultDataInBit;
		resultDataInBit = NULL;
	}
	if (NULL != resultDataInHex)
	{
		delete resultDataInHex;
		resultDataInHex = NULL;
	}
	if (NULL != readText)
	{
		delete readText;
		readText = NULL;
	}

}
