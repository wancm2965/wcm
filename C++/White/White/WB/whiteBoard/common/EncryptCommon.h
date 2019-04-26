#pragma once
#include <Windows.h>
#define KEYLENGTH 8

#define ASC2KEY 1
#define HEXKEY 2

typedef enum _SYSTEMTIMECompareEnum
{
	_TIME_BIG = 0,//大于
	_TIME_EQUAL,//等于
	_TIME_LESS,//小于
}SYSTEMTIMECompareEnum;
/*
比较BeginTime和EndTime两个日期时间，如果EndTime比BeginTime后则返回值为_TIME_BIG，同一天为_TIME_EQUAL，
如果EndTime比BeginTime前则返回值为_TIME_LESS
*/
SYSTEMTIMECompareEnum CompareBeginEndTime(SYSTEMTIME BeginTime,SYSTEMTIME EndTime)
{
	if (EndTime.wYear > BeginTime.wYear)//EndTime年份大于BeginTime年份
	{
		return _TIME_BIG;
	} 
	else if(EndTime.wYear == BeginTime.wYear)//EndTime年份等于BeginTime年份
	{
		if (EndTime.wMonth > BeginTime.wMonth)//EndTime月份大于BeginTime月份
		{
			return _TIME_BIG;
		} 
		else if(EndTime.wMonth = BeginTime.wMonth)//EndTime月份等于BeginTime月份
		{
			if (EndTime.wDay > BeginTime.wDay)
			{
				return _TIME_BIG;
			} 
			else if(EndTime.wDay == BeginTime.wDay)
			{
				return _TIME_EQUAL;
			}
			else
			{
				return _TIME_LESS;
			}
		}
		else//EndTime月份小于BeginTime月份
		{
			return _TIME_LESS;
		}
	}
	else//EndTime年份小于于BeginTime年份
	{
		return _TIME_LESS;
	}

}
//
void ChangeDateToSystemtime(char *DateData,SYSTEMTIME &BeginTime,SYSTEMTIME &EndTime,int DateDataLen)
{
	if (DateDataLen < 16)
	{
		return;
	}
	CString DateDataStr,TemStr;
	DateDataStr = DateData;
	TemStr = DateDataStr.Mid(0,4);
	BeginTime.wYear = _ttoi(TemStr);
	TemStr = DateDataStr.Mid(4,2);
	BeginTime.wMonth = _ttoi(TemStr);
	TemStr = DateDataStr.Mid(6,2);
	BeginTime.wDay = _ttoi(TemStr);
	TemStr = DateDataStr.Mid(8,4);
	EndTime.wYear = _ttoi(TemStr);
	TemStr = DateDataStr.Mid(12,2);
	EndTime.wMonth = _ttoi(TemStr);
	TemStr = DateDataStr.Mid(14,2);
	EndTime.wDay = _ttoi(TemStr);

}

void ChangeServerLoadStrToSystemTime(CString ServerLoadTime,SYSTEMTIME &Systemtime)
{

}
/*
int IsLoacalWB 本地使用白板还是网络使用白板
SYSTEMTIME AuthorizeBeginTime授权文件的开始时间
SYSTEMTIME AuthorizeEndTime授权文件的结束时间
SYSTEMTIME CurLoadServerTime当前服务器上返回的时间，仅在网络版中有效
SYSTEMTIME PreOpenSoftTime读取上次单机版的登陆时间
返回值：true为授权，false为不授权
*/
bool IsRightAuthorize(bool IsLoacalWB,SYSTEMTIME AuthorizeBeginTime,SYSTEMTIME AuthorizeEndTime,SYSTEMTIME CurLoadServerTime,SYSTEMTIME PreOpenSoftTime)
{
	bool bflag = false;
	if (IsLoacalWB)// 本地使用白板
	{
		SYSTEMTIME CurrentST = {0};
		GetLocalTime(&CurrentST);
		if (_TIME_LESS == CompareBeginEndTime(PreOpenSoftTime,CurrentST))//如果当前时间比上一次的登陆时间还要小，说明用户修改了系统时间
		{
			bflag = false;
		}
		else
		{
			if ((_TIME_LESS != CompareBeginEndTime(AuthorizeBeginTime,CurrentST)) && 
				(_TIME_LESS != CompareBeginEndTime(CurLoadServerTime,CurrentST))) 
			{
				bflag = true;
			}
		}
	} 
	else//网络使用白板
	{
		//服务器上返回的时间在授权文件之内
		if ((_TIME_LESS != CompareBeginEndTime(AuthorizeBeginTime,CurLoadServerTime)) && 
			(_TIME_LESS != CompareBeginEndTime(CurLoadServerTime,AuthorizeEndTime))) 
		{
			bflag = true;
		}
		
	}
	return bflag;
}

bool IsRightAuthorize(bool IsLoacalWB,CString AuthorizeBeginTimeStr,CString AuthorizeEndTimeStr,CString CurLoadServerTimeStr,CString PreOpenSoftTimeStr)
{
	if (IsLoacalWB)// 本地使用白板
	{
		SYSTEMTIME CurrentST = {0};
		CString CurLocalTimeStr = _T(""),TempStr;
		GetLocalTime(&CurrentST);
		CurLocalTimeStr.Format(_T("%d"),CurrentST.wYear);
		
		if (CurrentST.wMonth < 10)
		{
			TempStr.Format(_T("0%d"),CurrentST.wMonth);
			CurLocalTimeStr += TempStr;
		}
		else
		{
			TempStr.Format(_T("%d"),CurrentST.wMonth);
			CurLocalTimeStr += TempStr;
		}
		if (CurrentST.wDay < 10)
		{
			TempStr.Format(_T("0%d"),CurrentST.wDay);
			CurLocalTimeStr += TempStr;
		}
		else
		{
			TempStr.Format(_T("%d"),CurrentST.wDay);
			CurLocalTimeStr += TempStr;
		}
		if (CurrentST.wHour < 10)
		{
			TempStr.Format(_T("0%d"),CurrentST.wHour);
			CurLocalTimeStr += TempStr;
		}
		else
		{
			TempStr.Format(_T("%d"),CurrentST.wHour);
			CurLocalTimeStr += TempStr;
		}
		if (CurrentST.wMinute < 10)
		{
			TempStr.Format(_T("0%d"),CurrentST.wMinute);
			CurLocalTimeStr += TempStr;
		}
		else
		{
			TempStr.Format(_T("%d"),CurrentST.wMinute);
			CurLocalTimeStr += TempStr;
		}
		
		if (PreOpenSoftTimeStr.IsEmpty())//说明是第一次使用白板
		{
			PreOpenSoftTimeStr = CurLocalTimeStr;
		}
		if (CurLocalTimeStr.Compare(PreOpenSoftTimeStr) < 0)//如果当前时间比上一次的登陆时间还要小，说明用户修改了系统时间
		{
			return FALSE;
		}
		else
		{
			CurLocalTimeStr = CurLocalTimeStr.Mid(0,8);
			//当前时间在授权范围内
			if ((CurLocalTimeStr.Compare(AuthorizeBeginTimeStr) >= 0) && (CurLocalTimeStr.Compare(AuthorizeEndTimeStr) <= 0)) 
			{
				return TRUE;
			}
		}
	} 
	else//网络使用白板
	{
		//服务器上返回的时间在授权文件之内
		if ((CurLoadServerTimeStr.Compare(AuthorizeBeginTimeStr) >= 0) && (CurLoadServerTimeStr.Compare(AuthorizeEndTimeStr) <= 0)) 
		{
			return TRUE;
		}

	}
	return FALSE;
}



