
// stdafx.cpp : 只包括标准包含文件的源文件
// AIServerTestDemo.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


inline void base64encode(unsigned char* buffer, size_t bufferlen, std::string& encodedstr)
{
	const std::string ENCODETAB = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	bool add_crlf = false;
	size_t i = 0;
	size_t o = 0;

	encodedstr = "";
	while (i < bufferlen)
	{
		size_t remain = bufferlen - i;
		if (add_crlf && o && o % 76 == 0)
		{
			encodedstr += "\n";
		}
		switch (remain)
		{
		case 1:
			encodedstr += ENCODETAB[((buffer[i] >> 2) & 0x3f)];
			encodedstr += ENCODETAB[((buffer[i] << 4) & 0x30)];
			encodedstr += "==";
			break;
		case 2:
			encodedstr += ENCODETAB[((buffer[i] >> 2) & 0x3f)];
			encodedstr += ENCODETAB[((buffer[i] << 4) & 0x30) + ((buffer[i + 1] >> 4) & 0x0f)];
			encodedstr += ENCODETAB[((buffer[i + 1] << 2) & 0x3c)];
			encodedstr += "=";
			break;
		default:
			encodedstr += ENCODETAB[((buffer[i] >> 2) & 0x3f)];
			encodedstr += ENCODETAB[((buffer[i] << 4) & 0x30) + ((buffer[i + 1] >> 4) & 0x0f)];
			encodedstr += ENCODETAB[((buffer[i + 1] << 2) & 0x3c) + ((buffer[i + 2] >> 6) & 0x03)];
			encodedstr += ENCODETAB[(buffer[i + 2] & 0x3f)];
		}
		o += 4;
		i += 3;
	}
}



size_t Base64EncodeFromFile(const std::string& filename, std::string& encodedstr)
{
	if (filename.empty())
	{
		return 0;
	}

	FILE* fp = NULL;
	errno_t err = fopen_s(&fp,filename.c_str(), "rb");
	if (fp == NULL)
	{
		return 0;
	}

	while (!feof(fp))
	{
		unsigned char pBuffer[2048];
		size_t readlen = ::fread(pBuffer, sizeof(unsigned char), 2048, fp);
		std::string tmpstr;
		base64encode(pBuffer, readlen, tmpstr);
		encodedstr = encodedstr + tmpstr;
	}
	::fclose(fp);
	return encodedstr.length();
}