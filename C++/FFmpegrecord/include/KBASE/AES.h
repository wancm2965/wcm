// KAES.h: interface for the KAES class.

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "KBASE/KBASE_Export.h"

#include <string>

class KBASE_API KAES
{
public:
	KAES(unsigned char* key);	
	virtual ~KAES();
	//加密字符串，strPlaintext：需要加密的明文，strCiphertext：输出的密文，strKey：密钥
	static void EncryptionStr(const std::string& strPlaintext, std::string& strCiphertext, const std::string& strKey);
	//解密字符串，strCiphertext：需要解密的密文，strPlaintext：输出的明文，strKey：密钥
	static void DecryptStr(const std::string& strCiphertext, std::string& strPlaintext, const std::string& strKey);
	//加密文件，strFilePathName：需要加密文件的全路径名，strEncryptedFile：输出的加密文件的全路径名，strKey：密钥
	static void EncryptionFile(const std::string& strFilePathName, const std::string& strEncryptedFile, const std::string& strKey);
	//解密文件，strEncryptedFile：需要解密文件的全路径名，strFilePathName：输出的解密文件的全路径名（如不需要请赋空），strKey：密钥
	//返回值：解密后的文本内容
	static std::string DecryptFile(const std::string& strEncryptedFile, const std::string& strFilePathName, const std::string& strKey);
	
private:	
	//加密整段文字
	void CipherStr(const char *input,char *output);	
	//解密整段文字，bFlg：标记是否需要剔除补充字符
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
