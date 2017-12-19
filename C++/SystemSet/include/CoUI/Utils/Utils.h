#ifndef __UTILS_H__
#define __UTILS_H__

#include "..\UIlibDeclare.h"

namespace CoUI {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
		{ }
		STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
		{ }
		LPCTSTR m_lpstr;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API UICPoint : public tagPOINT
	{
	public:
		UICPoint();
		UICPoint(const POINT& src);
		UICPoint(int x, int y);
		UICPoint(LPARAM lParam);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API UICSize : public tagSIZE
	{
	public:
		UICSize();
		UICSize(const SIZE& src);
		UICSize(const RECT &rc);
		UICSize(int cx, int cy);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API UICRect : public tagRECT
	{
	public:
		UICRect();
		UICRect(const RECT& src);
		UICRect(int iLeft, int iTop, int iRight, int iBottom);

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);
		void Union(UICRect& rc);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdPtrArray
	{
	public:
		CStdPtrArray(int iPreallocSize = 0);
		CStdPtrArray(const CStdPtrArray& src);
		~CStdPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdValArray
	{
	public:
		CStdValArray(int iElementSize, int iPreallocSize = 0);
		~CStdValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		const int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdString
	{
	public:
		enum { MAX_LOCAL_STRING_LEN = 63 };

		CStdString();
		CStdString(const TCHAR ch);
		CStdString(const CStdString& src);
		CStdString(LPCTSTR lpsz, int nLen = -1);
		~CStdString();

		void Empty();
		int GetLength() const;
		bool IsEmpty() const;
		TCHAR GetAt(int nIndex) const;
		void Append(LPCTSTR pstr);
		void Assign(LPCTSTR pstr, int nLength = -1);
		LPCTSTR GetData() const;

		/*******************************************************************
		**下面的成员函数实现的功能与CString中相同成员函数功能类似
		*******************************************************************/
		CStdString &MakeReverse( );
		CStdString SpanIncluding(LPCTSTR lpszCharSet) const;
		CStdString SpanExcluding(LPCTSTR lpszCharSet) const;
		CStdString &TrimLeft( );
		CStdString &TrimLeft(LPCTSTR lpszTargets);
		CStdString &TrimRight( );
		CStdString &TrimRight(LPCTSTR lpszTargets);
//////////////////////////////////////////////////////////////////		

		void SetAt(int nIndex, TCHAR ch);
		operator LPCTSTR() const;

		TCHAR operator[] (int nIndex) const;
		const CStdString& operator=(const CStdString& src);
		const CStdString& operator=(const TCHAR ch);
		const CStdString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
		CStdString(const char* ch);
		const CStdString& CStdString::operator=(LPCSTR lpStr);
		const CStdString& CStdString::operator+=(LPCSTR lpStr);
#else
		const CStdString& CStdString::operator=(LPCWSTR lpwStr);
		const CStdString& CStdString::operator+=(LPCWSTR lpwStr);
#endif
		CStdString operator+(const CStdString& src) const;
		CStdString operator+(LPCTSTR pstr) const;
		const CStdString& operator+=(const CStdString& src);
		const CStdString& operator+=(LPCTSTR pstr);
		const CStdString& operator+=(const TCHAR ch);

		bool operator == (LPCTSTR str) const;
		bool operator != (LPCTSTR str) const;
		bool operator <= (LPCTSTR str) const;
		bool operator <  (LPCTSTR str) const;
		bool operator >= (LPCTSTR str) const;
		bool operator >  (LPCTSTR str) const;

		int Compare(LPCTSTR pstr) const;
		int CompareNoCase(LPCTSTR pstr) const;

		void MakeUpper();
		void MakeLower();

		CStdString Left(int nLength) const;
		CStdString Mid(int iPos, int nLength = -1) const;
		CStdString Right(int nLength) const;

		int Find(TCHAR ch, int iPos = 0) const;
		int Find(LPCTSTR pstr, int iPos = 0) const;
		int ReverseFind(TCHAR ch) const;
		int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

		int __cdecl Format(LPCTSTR pstrFormat, ...);
		int __cdecl Format(LPCTSTR pstrFormat, va_list Args);
		int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);

	protected:
		LPTSTR m_pstr;
		TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	struct TITEM
	{
		CStdString Key;
		LPVOID Data;
		struct TITEM* pNext;
	};

	class UILIB_API CStdStringPtrMap
	{
	public:
		CStdStringPtrMap(int nSize = 83);
		~CStdStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

} // namespace CoUI
#endif // __UTILS_H__