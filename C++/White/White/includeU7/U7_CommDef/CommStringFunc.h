/*
	文件名：CommStringFunc.h
	功能：定义跨平台的与字符串相关的通用函数
	作者：李章斐
*/
#ifndef _COMM_STRING_FUNC_H
#define _COMM_STRING_FUNC_H

#include <string>
#include <map>

/* 按照指定序号(整形int)保存字符串 */
typedef std::map<int,std::string> STRING_MAP;
typedef STRING_MAP::iterator STRING_MAP_IT;

typedef std::map<std::string, std::string> STRING_MAP_EX;
typedef STRING_MAP_EX::iterator STRING_MAP_EX_IT;

class CCommStringFunc
{
public:
	//char 数组拷贝
    static void _Char_Copy(char* chDest, const char* chSource, int nDestMaxSize, int nCopyLen)
	{
        if (nDestMaxSize <= nCopyLen)
		{
            nCopyLen = nDestMaxSize - 1;
		}
#ifdef  WIN32
        strncpy_s(chDest, nDestMaxSize, chSource, nCopyLen);
#else
		strncpy(chDest, chSource, nCopyLen);
		chDest[nCopyLen] = '\0';
#endif
	}

	//通过字符串获取域名
	static std::string GetDomainID(const std::string& strSource)
	{
		int nFind = strSource.rfind('@');
		if (-1 == nFind)
			return "";

		return strSource.substr(nFind + 1, strSource.length() - 1);
	}

	//将字符串data按token字符分割，分割后的结果按序号存入STRING_MAP
	static STRING_MAP StrSplit(const std::string& data, const std::string& token)
	{
		STRING_MAP mapStringSplit;

		if (data.length() == 0)
		{
			return mapStringSplit;
		}

		int datalen = (int)data.length();
		int tokenlen = (int)token.length();
		int start = 0;
		int end = 0;
		int off = 0;
		int count = 0;
		while (end >= 0)
		{
			end = (int)data.find(token, start);
			off = end - start;
			std::string subdata = data.substr(start, off);
			mapStringSplit.insert(STRING_MAP::value_type(count, subdata));
			start = end + tokenlen;
			++count;
		}
		return mapStringSplit;
	}

	//将字符串data按token字符分割，分割后的结果再按"="号分割，最后存入STRING_MAP_EX
	static STRING_MAP_EX StrSplitEx(const std::string& data, const std::string& token)
	{
		STRING_MAP_EX mapStringSplit;

		if (data.length() == 0)
		{
			return mapStringSplit;
		}

		STRING_MAP mapSegment = StrSplit(data, token);
		for (int i = 0; i < (int)mapSegment.size(); ++i)
		{
			const std::string& seg = mapSegment[i];
			STRING_MAP b = StrSplit(seg, "=");
			mapStringSplit.insert(STRING_MAP_EX::value_type(b[0], b[1]));
		}
		return mapStringSplit;
	}

	//将多条选项字符串编码为一条字符串，在编码中标识当前选中哪一项，函数会对空格分隔符' '进行字符转义。
	static std::string MakeMultiOptString(const STRING_MAP& mapOptions, const int nSelIndex)
	{
		std::string strOutPut;
		std::string strTmpValue;
		STRING_MAP::const_iterator it = mapOptions.begin();
		for (; it != mapOptions.end(); ++it)
		{
			strTmpValue = it->second;
			// 找到空格，并对空格进行转义
			if (strTmpValue.find(' ') != -1)
			{
				std::string in = strTmpValue;
				std::string out = "";
				for (size_t j=0; j<in.length(); ++j)
				{
					std::string temp;
					switch (in[j])
					{
					case ' ':
						temp = "&blank;";
						break;
					default:
						temp.append(1, in[j]);
					}
					out += temp;
				}
				strTmpValue = out;
			}

			if (nSelIndex == it->first)
			{
				strOutPut += "&cursel;";
			}
			strOutPut += strTmpValue;
			strOutPut += " ";
		}
		return strOutPut;
	}

	// 对多选项字符串进行解包
	static bool DcMakeMultiOptString(const std::string& strInput, STRING_MAP& mapOptions, int& nSelIndex)
	{
		STRING_MAP mapTemp = StrSplit(strInput, " ");
		STRING_MAP_IT it = mapTemp.begin();
		for (; it != mapTemp.end(); ++it)
		{
			std::string strTemp = it->second;
			std::basic_string <char>::size_type index = strTemp.find("&cursel;");
			if (index != -1)
			{
				nSelIndex = it->first;
				strTemp.replace(index, 8, "");
			}
			while (true)
			{
				index = strTemp.find("&blank;");
				if (index != -1)
				{
					strTemp.replace(index, 7, " ");
				}
				else
				{
					break;
				}
			}
			mapOptions[it->first] = strTemp;
		}
		return true;
	}
};
#endif
