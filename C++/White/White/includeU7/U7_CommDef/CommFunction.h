#pragma once

class CCommFunction
{
public:
	static std::string U2A(CString cstring)
	{
		if (::IsBadStringPtrW(cstring,-1))
			return "";

		int cchStr = ::WideCharToMultiByte(CP_UTF8, 0, cstring, wcslen(cstring), NULL, 0, NULL, NULL);
		char* pstr = new char[cchStr + 1];
		::WideCharToMultiByte(CP_UTF8, 0, cstring, -1, pstr, cchStr, NULL, NULL);
		pstr[cchStr] = '\0';
		std::string str(pstr);
		delete []pstr;
		return str;
	}

	static CString A2U(const std::string& sstring)
	{
		int cchStr = (int) sstring.length() + 1;
		wchar_t* pwstr = new wchar_t[cchStr];
		::MultiByteToWideChar(CP_UTF8, 0, sstring.c_str(), -1, pwstr, cchStr);
		CString strTmp(pwstr);
		delete []pwstr;
		return strTmp;
	}

	static CString INT2CSTR(int nValue)
	{
		CString str;
		str.Format(_T("%d"), nValue);
		return str;
	}

	static CString ULONG2CSTR(unsigned long ulValue)
	{
		CString str;
		str.Format(_T("%u"), ulValue);
		return str;
	}

};
