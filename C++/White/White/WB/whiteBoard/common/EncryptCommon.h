#pragma once
#include <Windows.h>
#define KEYLENGTH 8

#define ASC2KEY 1
#define HEXKEY 2

typedef enum _SYSTEMTIMECompareEnum
{
	_TIME_BIG = 0,//����
	_TIME_EQUAL,//����
	_TIME_LESS,//С��
}SYSTEMTIMECompareEnum;
/*
�Ƚ�BeginTime��EndTime��������ʱ�䣬���EndTime��BeginTime���򷵻�ֵΪ_TIME_BIG��ͬһ��Ϊ_TIME_EQUAL��
���EndTime��BeginTimeǰ�򷵻�ֵΪ_TIME_LESS
*/
SYSTEMTIMECompareEnum CompareBeginEndTime(SYSTEMTIME BeginTime,SYSTEMTIME EndTime)
{
	if (EndTime.wYear > BeginTime.wYear)//EndTime��ݴ���BeginTime���
	{
		return _TIME_BIG;
	} 
	else if(EndTime.wYear == BeginTime.wYear)//EndTime��ݵ���BeginTime���
	{
		if (EndTime.wMonth > BeginTime.wMonth)//EndTime�·ݴ���BeginTime�·�
		{
			return _TIME_BIG;
		} 
		else if(EndTime.wMonth = BeginTime.wMonth)//EndTime�·ݵ���BeginTime�·�
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
		else//EndTime�·�С��BeginTime�·�
		{
			return _TIME_LESS;
		}
	}
	else//EndTime���С����BeginTime���
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
int IsLoacalWB ����ʹ�ðװ廹������ʹ�ðװ�
SYSTEMTIME AuthorizeBeginTime��Ȩ�ļ��Ŀ�ʼʱ��
SYSTEMTIME AuthorizeEndTime��Ȩ�ļ��Ľ���ʱ��
SYSTEMTIME CurLoadServerTime��ǰ�������Ϸ��ص�ʱ�䣬�������������Ч
SYSTEMTIME PreOpenSoftTime��ȡ�ϴε�����ĵ�½ʱ��
����ֵ��trueΪ��Ȩ��falseΪ����Ȩ
*/
bool IsRightAuthorize(bool IsLoacalWB,SYSTEMTIME AuthorizeBeginTime,SYSTEMTIME AuthorizeEndTime,SYSTEMTIME CurLoadServerTime,SYSTEMTIME PreOpenSoftTime)
{
	bool bflag = false;
	if (IsLoacalWB)// ����ʹ�ðװ�
	{
		SYSTEMTIME CurrentST = {0};
		GetLocalTime(&CurrentST);
		if (_TIME_LESS == CompareBeginEndTime(PreOpenSoftTime,CurrentST))//�����ǰʱ�����һ�εĵ�½ʱ�仹ҪС��˵���û��޸���ϵͳʱ��
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
	else//����ʹ�ðװ�
	{
		//�������Ϸ��ص�ʱ������Ȩ�ļ�֮��
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
	if (IsLoacalWB)// ����ʹ�ðװ�
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
		
		if (PreOpenSoftTimeStr.IsEmpty())//˵���ǵ�һ��ʹ�ðװ�
		{
			PreOpenSoftTimeStr = CurLocalTimeStr;
		}
		if (CurLocalTimeStr.Compare(PreOpenSoftTimeStr) < 0)//�����ǰʱ�����һ�εĵ�½ʱ�仹ҪС��˵���û��޸���ϵͳʱ��
		{
			return FALSE;
		}
		else
		{
			CurLocalTimeStr = CurLocalTimeStr.Mid(0,8);
			//��ǰʱ������Ȩ��Χ��
			if ((CurLocalTimeStr.Compare(AuthorizeBeginTimeStr) >= 0) && (CurLocalTimeStr.Compare(AuthorizeEndTimeStr) <= 0)) 
			{
				return TRUE;
			}
		}
	} 
	else//����ʹ�ðװ�
	{
		//�������Ϸ��ص�ʱ������Ȩ�ļ�֮��
		if ((CurLoadServerTimeStr.Compare(AuthorizeBeginTimeStr) >= 0) && (CurLoadServerTimeStr.Compare(AuthorizeEndTimeStr) <= 0)) 
		{
			return TRUE;
		}

	}
	return FALSE;
}



