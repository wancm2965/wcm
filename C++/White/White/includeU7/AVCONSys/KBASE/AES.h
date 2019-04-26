// KAES.h: interface for the KAES class.

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class AFX_EXT_CLASS KAES
{
public:
	KAES(unsigned char* key);	
	virtual ~KAES();
	//�����ַ�����strPlaintext����Ҫ���ܵ����ģ�strCiphertext����������ģ�strKey����Կ
	static void EncryptionStr(const std::string& strPlaintext, std::string& strCiphertext, const std::string& strKey);
	//�����ַ�����strCiphertext����Ҫ���ܵ����ģ�strPlaintext����������ģ�strKey����Կ
	static void DecryptStr(const std::string& strCiphertext, std::string& strPlaintext, const std::string& strKey);
	//�����ļ���strFilePathName����Ҫ�����ļ���ȫ·������strEncryptedFile������ļ����ļ���ȫ·������strKey����Կ
	static void EncryptionFile(const std::string& strFilePathName, const std::string& strEncryptedFile, const std::string& strKey);
	//�����ļ���strEncryptedFile����Ҫ�����ļ���ȫ·������strFilePathName������Ľ����ļ���ȫ·�������粻��Ҫ�븳�գ���strKey����Կ
	//����ֵ�����ܺ���ı�����
	static std::string DecryptFile(const std::string& strEncryptedFile, const std::string& strFilePathName, const std::string& strKey);
	
private:	
	//������������
	void CipherStr(const char *input,char *output);	
	//�����������֣�bFlg������Ƿ���Ҫ�޳������ַ�
	void InvCipherStr(char *inut, char *output, bool bFlg = true);
	void InvCipher(char *inut, char *out);
	void Cipher(char *input, char *output);
	unsigned char* Cipher(unsigned char* input);
	
	unsigned char* InvCipher(unsigned char* input);
	void* Cipher(void* input, int length=0);
	void* InvCipher(void* input, int length);	
		

	void KeyExpansion(unsigned char* key, unsigned char w[][4][4]);	
	unsigned char FFmul(unsigned char a, unsigned char b);	
	void SubBytes(unsigned char state[][4]);
	void ShiftRows(unsigned char state[][4]);	
	void MixColumns(unsigned char state[][4]);	
	void AddRoundKey(unsigned char state[][4], unsigned char k[][4]);	
	void InvSubBytes(unsigned char state[][4]);	
	void InvShiftRows(unsigned char state[][4]);	
	void InvMixColumns(unsigned char state[][4]);
	int strToHex(const char *ch, char *hex);
	int hexToStr(const char *hex, char *ch);
	int ascillToValue(const char ch);
	char valueToHexCh(const int value);
	int getUCharLen(const unsigned char *uch);
	int strToUChar(const char *ch, unsigned char *uch);
	int ucharToStr(const unsigned char *uch, char *ch,int nLen);
	int ucharToHex(const unsigned char *uch, char *hex,int nLen);
	int hexToUChar(const char *hex, unsigned char *uch);

private:
	unsigned char m_Sbox[256];	
	unsigned char m_InvSbox[256];	
	unsigned char m_w[11][4][4];	
};
