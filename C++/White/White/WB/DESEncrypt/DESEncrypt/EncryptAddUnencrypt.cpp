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
	char subEncodeKey1[16][48];//加密的子秘钥
	char subDecodeKey1[16][48];//解密的子秘钥
	char* oriDataInBit = NULL;
	char* oriDataInHex = NULL;
	char* resultDataInBit = NULL;
	char* resultDataInHex = NULL;
	int oriDataInBitLength=0;
	char* PKeyCiphertextData = NULL;//秘钥的二进制
	PKeyCiphertextData = checkValidKey(m_keyOriginalData,ASC2KEY,m_keyOriginalData.GetLength(),&key1ValidFlag);

	if (!checkValidPlainText(EncryptData))
	{
		AfxMessageBox(_T("明文长度非法，请重新输入!"));
		return;
	}
	//加密

	//生成加密及解密的子密钥
	genAllSubKey(PKeyCiphertextData,subEncodeKey1,subDecodeKey1);	

	//将明文转化为二进制形式
	oriDataInBit=CStringToBit_64Grouped(EncryptData,&oriDataInBitLength);

	//将二进制形式明文变为16进制形式
	oriDataInHex=BinToHex_4Grouped(oriDataInBit,oriDataInBitLength);


	//对二进制形式明文进行加密得到二进制密文
	resultDataInBit=DESEncode(oriDataInBit,oriDataInBitLength,subEncodeKey1);

	//将二进制形式密文变为16进制形式
	resultDataInHex=BinToHex_4Grouped(resultDataInBit,oriDataInBitLength);

	CString strFilePath;
	//保存加密文件
	CFileDialog dlgFile(FALSE, _T("wbe"),_T("AVCONWB"), OFN_HIDEREADONLY , _T("UCB Files (AVCONWB.wbe)|AVCONWB.wbe||"), NULL, 0, FALSE);

	dlgFile.m_ofn.lpstrTitle = _T("保存为");

	if (dlgFile.DoModal() == IDOK)
	{
		strFilePath = dlgFile.GetPathName();
		if (PathFileExists(strFilePath))//如果文件存在就删除这个文件
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
	// 读取文件
	CFile file;
	
	if (!file.Open(FilePath, CFile::modeRead))
	{
		resultData = NULL;
		return ;
	}
	ULONGLONG lFileLength = file.GetLength(); //文件长度
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
	//解密
	char* oriDataInBit = NULL;
	char* resultDataInBit = NULL;
	char* resultDataInHex = NULL;
	char subEncodeKey[16][48];//加密的子秘钥
	char subDecodeKey[16][48];//解密的子秘钥
	int keyValidFlag;
	CString KeyOriginalData = _T("AVCON_WB");
	char* PKeyCiphertextData = NULL;//秘钥的二进制
	PKeyCiphertextData = checkValidKey(KeyOriginalData,ASC2KEY,KeyOriginalData.GetLength(),&keyValidFlag);
	//生成加密及解密的子密钥
	genAllSubKey(PKeyCiphertextData,subEncodeKey,subDecodeKey);	

	//将16进制密文转化为二进制形式
	CString tempReadText(readText);
	oriDataInBit=CStringInHexToBit_64Grouped(tempReadText,&resultDataLenth);

	//对二进制形式密文进行解密得到二进制明文
	resultDataInBit=DESEncode(oriDataInBit,resultDataLenth,subDecodeKey);

	//将二进制形式明文变为16进制形式
	resultDataInHex=BinToHex_4Grouped(resultDataInBit,resultDataLenth);


	//将16进制形式明文变为CString形式
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
