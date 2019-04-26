

#include "StdAfx.h"
#include "exec.h"

#ifdef _WIN32
#include <MMSystem.h>
#pragma  comment(lib,"winmm.lib")
#endif
//#include "math.h"

CString g_strExePath = _T("");
CString GetExePath(void)
{
	if (g_strExePath.IsEmpty())
	{
		TCHAR path[8192];
		int len = GetModuleFileName(AfxGetInstanceHandle(),path,8192);
		path[len]=0;
		TCHAR* ret = _tcsrchr(path,'\\');
		if(!ret)
			return _T("");
		int idxlen = (int)(ret - path + 1);
		path[idxlen]=0;
		g_strExePath = CString(path);
	}

	return g_strExePath;
}

/*************************************************************************
*****                           ����ֱ�ߵĳ���                       *****
*************************************************************************/
double CalDistancePointToPoint(const CPoint ptA, const CPoint ptB)
{
	//���������ľ���
	double dx = ptA.x - ptB.x;
	double dy = ptA.y - ptB.y;
	return sqrt(dx*dx + dy*dy);
}

/*************************************************************************
*****                        ����㵽ֱ�ߵľ���                     *****
*************************************************************************/
double CalDistancePointToLine(const CPoint pt, const CPoint ptA, const CPoint ptB)
{
	
	//�����pt���߶�(A,B)�ľ���
	double length; /* ֱ�߳��� */
	double a = ptB.y-ptA.y;
	double b = ptA.x-ptB.x;
	double c = ptB.x * ptA.y - ptA.x * ptB.y;
	return length = abs(a * pt.x + b *pt.y + c)/sqrt(a * a + b * b);
		/*	length = CalDistancePointToPoint(ptA,ptB);
	if(length == 0.0)
		return( CalDistancePointToPoint(ptA,pt));
	r = ((ptA.y - pt.x )*(ptA.y - ptB.y) - (ptA.x - pt.x )*(ptB.x - ptA.x))/(length*length);
	if(r > 1) /* perpendicular projection of P is on the forward extention of AB */
/*		return(min(CalDistancePointToPoint(pt, ptB),CalDistancePointToPoint(pt, ptA)));
	if(r < 0) /* perpendicular projection of P is on the backward extention of AB */
/*		return(min(CalDistancePointToPoint(pt, ptB),CalDistancePointToPoint(pt, ptA)));
	s = ((ptA.y - pt.y)*(ptB.x - ptA.x) - (ptA.x - pt.x )*(ptB.y - ptA.y))/(length*length);

	return(fabs(s*length));*/
}

void PointRotationPoint(
	const CPoint& ptCenter, 
	double dbAngle, 
	const CPoint& ptSrc, 
	CPoint& ptDes)
/*����������ĵ���ת�������
������
ptCenter�����룬���ĵ�����
dbAngle�����룬��ת�Ƕȣ�˳ʱ��Ϊ��
ptSrc�����룬����תǰ������
ptDes�����������ת�������
����ֵ���ɹ�����TRUE,���򷵻�FALSE
*/
{
	//�Ƕ�ת����
	double dbRadian = dbAngle * PI / 180; 

	ptDes.x = (LONG)((ptSrc.x - ptCenter.x) * cos(dbRadian) + (ptSrc.y - ptCenter.y) * sin(dbRadian) + ptCenter.x + 0.5);
	ptDes.y = (LONG)(-(ptSrc.x - ptCenter.x) * sin(dbRadian) + (ptSrc.y - ptCenter.y) * cos(dbRadian) + ptCenter.y + 0.5);
}
/*
double PointRotationPoint(const CPoint& ptCenter, const CPoint& ptBegin, const CPoint& ptEnd)
/*����������ĵ���ת��ĽǶ�
������
ptCenter�����룬��ת���ĵ�����
ptBegin�����룬��ת��ʼ��
ptEnd�����룬��ת������
����ֵ����ת�Ƕ�
*//*
{
	double dbAngle = 0;
	double dbRadian = 0; //����

	//��������㵽�е������ֱ�ߵľ���
	double dbDistance = CalDistancePointToLine(ptEnd, ptCenter, ptBegin);

	//�����е������ֱ�ߵľ���
	double dbLine = CalDistancePointToPoint(ptCenter, ptEnd);

	dbRadian = asin(dbDistance / dbLine);

	dbAngle = dbRadian * 180 / PI;

	return dbAngle;
}
*/

double PointRotationPoint(const CPoint& ptCenter, const CPoint& ptBegin, const CPoint& ptEnd)
	/*����������ĵ���ת��ĽǶ�
	������
	ptCenter�����룬��ת���ĵ�����
	ptBegin�����룬��ת��ʼ��
	ptEnd�����룬��ת������
	����ֵ����ת�Ƕ�
	*/
{
	double dbAngle = 0;
	double dbRadian = 0; //����	
	int y;	//�����ж��յ�����������ĵ�����ֱ�ߵ�����
	//��������㵽���ߵľ���
	double dbDistance = CalDistancePointToLine(ptEnd, ptCenter, ptBegin);

	//�������ĵ��������ֱ�ߵľ���
	double dbLine = CalDistancePointToPoint(ptCenter, ptEnd);

	if (dbLine == 0)
	{
		return 0;
	}

	if (ptBegin.x == ptCenter.x)
	{
		return 0;
	}

	dbRadian = asin(dbDistance / dbLine);

	dbAngle = dbRadian * 180 / PI;	//ƫ�ƽǶ�		
	if(ptBegin.x == ptCenter.x && ptEnd.y < ptCenter.y)	//����������ĵ���ɵ�ֱ�ߴ�ֱ�����յ������ĵ�֮��ʱ
	{
		if (ptEnd.x < ptBegin.x)	//�յ������λ�ñȽ�
		{
			return dbAngle;
		} 
		else
		{
			return 0 - dbAngle;
		}		
	}
	if(ptBegin.x == ptCenter.x && ptEnd.y > ptCenter.y)	//����������ĵ���ɵ�ֱ�ߴ�ֱ�����յ������ĵ�֮��ʱ
	{
		if (ptEnd.x > ptBegin.x)
		{
			return dbAngle;
		} 
		else
		{
			return 0 - dbAngle;
		}	
	}

	y = (ptEnd.x - ptCenter.x) * (ptBegin.y - ptCenter.y) / (ptBegin.x - ptCenter.x) + ptCenter.y;
	//���λ�������ĵ�Ϊԭ��ĵ�һ����
	if(ptEnd.x >= ptCenter.x && ptEnd.y < ptCenter.y)
	{
		if (ptBegin.x < ptCenter.x && ptBegin.y < ptCenter.y)	//����Ǵӵڶ������ƶ�����һ����
		{
			return 0 - dbAngle;
		}
		if (ptEnd.x == ptCenter.x && ptBegin.x > ptCenter.x && ptBegin.y < ptCenter.y)
		{
			return dbAngle;
		}
		if(ptEnd.y < y)
		{
			return dbAngle;
		}
		else
		{
			dbAngle = 0 - dbAngle;//ƫ�ƽǶ�
			return dbAngle;
		}
	}
	//���λ�������ĵ�Ϊԭ��ĵڶ�����
	if(ptEnd.x < ptCenter.x && ptEnd.y <= ptCenter.y)
	{
		if (ptBegin.x > ptCenter.x && ptBegin.y < ptCenter.y)	//����Ǵӵ�һ�����ƶ����ڶ�����
		{
			return dbAngle;
		}		
		if(ptEnd.y > y)
		{
			return dbAngle;
		}
		else
		{
			dbAngle = 0 - dbAngle;//ƫ�ƽǶ�
			return dbAngle;
		}
	}
	//���λ�������ĵ�Ϊԭ��ĵ�������
	if(ptEnd.x <= ptCenter.x && ptEnd.y > ptCenter.y)
	{
		if (ptBegin.x > ptCenter.x && ptBegin.y > ptCenter.y)	//����Ǵӵ��������ƶ�����������
		{
			return 0 - dbAngle;
		}
		if (ptEnd.x == ptCenter.x && ptBegin.x < ptCenter.x && ptBegin.y > ptCenter.y)
		{
			return dbAngle;
		}
		if(ptEnd.y > y)
		{
			return dbAngle;
		}
		else
		{
			dbAngle = 0 - dbAngle;//ƫ�ƽǶ�
			return dbAngle;
		}
	}
	//���λ�������ĵ�Ϊԭ��ĵ�������
	if(ptEnd.x > ptCenter.x && ptEnd.y >= ptCenter.y)
	{
		if (ptBegin.x < ptCenter.x && ptBegin.y > ptCenter.y)	//����Ǵӵ��������ƶ�����������
		{
			return dbAngle;
		}		
		if(ptEnd.y < y)
		{
			return dbAngle;
		}
		else
		{
			dbAngle = 0 - dbAngle;//ƫ�ƽǶ�
			return dbAngle;
		}
	}
	return dbAngle;
}

CPoint CalLineCross(
	const CPoint& fistLinePtBegin, 
	const CPoint& fistLinePtEnd, 
	const CPoint& eraseLinePtBegin, 
	const CPoint& eraseLinePtEnd)
{
	CPoint pt = NULL;
	double doubletemp = 0.0;
	/*
	*ֱ�����㹫ʽ��x = (y - y1) * (x2 - x1) / (y2 - y1) + x1
	*          ��  y = (x - x1) * (y2 - y1) / (x2 - x1) + y1
	*/
	if (eraseLinePtBegin.y == eraseLinePtEnd.y)
	{
		doubletemp = ((double)(eraseLinePtBegin.y - fistLinePtBegin.y) * (double)(fistLinePtEnd.x - fistLinePtBegin.x)) / (double)(fistLinePtEnd.y -fistLinePtBegin.y) + fistLinePtBegin.x;
		
		pt.x = (int)(doubletemp + 0.5);
		pt.y = eraseLinePtBegin.y;
	} 
	else
	{
		doubletemp = ((double)(eraseLinePtBegin.x - fistLinePtBegin.x) * (double)(fistLinePtEnd.y - fistLinePtBegin.y)) / (double)(fistLinePtEnd.x -fistLinePtBegin.x) + fistLinePtBegin.y;

		pt.y = (int)(doubletemp + 0.5);
		pt.x = eraseLinePtBegin.x;
	}

	return pt;
}

int Rounding(double dbTemp, BOOL bIsX_axis, int nIndex, BOOL bIsToZero)
{
	int nTemp;
	int nabsTemp;
	double doubletemp;
	int inttemp = 0;
	double increment = 0.0;

	if (nIndex > 4)
	{
		nTemp = (int)(dbTemp + 0.5);
		return nTemp;
	}

	static double mintotalincrement_X_Plus[5] = {0};
	static double maxtotalincrement_X_Plus[5] = {0};
	static double mintotalincrement_X_Minus[5] = {0};
	static double maxtotalincrement_X_Minus[5] = {0};
	static double mintotalincrement_Y_Plus[5] = {0};
	static double maxtotalincrement_Y_Plus[5] = {0};
	static double mintotalincrement_Y_Minus[5] = {0};
	static double maxtotalincrement_Y_Minus[5] = {0};

	doubletemp = abs(dbTemp);
	inttemp = (int)(doubletemp);
	increment = doubletemp - inttemp;

	if (bIsToZero)
	{
		for (int i = 0; i < 5; i ++)
		{
			mintotalincrement_X_Plus[i] = 0;
			maxtotalincrement_X_Plus[i] = 0;
			mintotalincrement_X_Minus[i] = 0;
			maxtotalincrement_X_Minus[i] = 0;
			mintotalincrement_Y_Plus[i] = 0;
			maxtotalincrement_Y_Plus[i] = 0;
			mintotalincrement_Y_Minus[i] = 0;
			maxtotalincrement_Y_Minus[i] = 0;
		}
	}

	if (bIsX_axis)
	{
		if (dbTemp == doubletemp) //dbTemp������
		{
			if (increment < 0.5)
			{
				mintotalincrement_X_Plus[nIndex] += increment;
				if (mintotalincrement_X_Plus[nIndex] >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) + 1;
					mintotalincrement_X_Plus[nIndex] -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			else
			{
				maxtotalincrement_X_Plus[nIndex] += (1 - increment);
				if (maxtotalincrement_X_Plus[nIndex] >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) - 1;
					maxtotalincrement_X_Plus[nIndex] -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			nTemp = nabsTemp;
		} 
		else //dbTemp�Ǹ���
		{
			if (increment < 0.5)
			{
				mintotalincrement_X_Minus[nIndex] += increment;
				if (mintotalincrement_X_Minus[nIndex] >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) + 1;
					mintotalincrement_X_Minus[nIndex] -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			else
			{
				maxtotalincrement_X_Minus[nIndex] += (1 - increment);
				if (maxtotalincrement_X_Minus[nIndex] >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) - 1;
					maxtotalincrement_X_Minus[nIndex] -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			nTemp = -nabsTemp;
		}
	} 
	else
	{
		if (dbTemp == doubletemp) //dbTemp������
		{
			if (increment < 0.5)
			{
				mintotalincrement_Y_Plus[nIndex] += increment;
				if (mintotalincrement_Y_Plus[nIndex] >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) + 1;
					mintotalincrement_Y_Plus[nIndex] -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			else
			{
				maxtotalincrement_Y_Plus[nIndex] += (1 - increment);
				if (maxtotalincrement_Y_Plus[nIndex] >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) - 1;
					maxtotalincrement_Y_Plus[nIndex] -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			nTemp = nabsTemp;
		} 
		else //dbTemp�Ǹ���
		{
			if (increment < 0.5)
			{
				mintotalincrement_Y_Minus[nIndex] += increment;
				if (mintotalincrement_Y_Minus[nIndex] >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) + 1;
					mintotalincrement_Y_Minus[nIndex] -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			else
			{
				maxtotalincrement_Y_Minus[nIndex] += (1 - increment);
				if (maxtotalincrement_Y_Minus[nIndex] >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) - 1;
					maxtotalincrement_Y_Minus[nIndex] -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			nTemp = -nabsTemp;
		}
	}

	return nTemp;
}

int RoundingOnePt(double dbTemp, BOOL bIsX_axis)
{
	int nTemp;
	int nabsTemp;
	double doubletemp;
	int inttemp = 0;
	double increment = 0.0;

	static double mintotalincrement_X_Plus = 0;
	static double maxtotalincrement_X_Plus = 0;
	static double mintotalincrement_X_Minus = 0;
	static double maxtotalincrement_X_Minus = 0;
	static double mintotalincrement_Y_Plus = 0;
	static double maxtotalincrement_Y_Plus = 0;
	static double mintotalincrement_Y_Minus = 0;
	static double maxtotalincrement_Y_Minus = 0;

	doubletemp = abs(dbTemp);
	inttemp = (int)(doubletemp);
	increment = doubletemp - inttemp;

	if (bIsX_axis)
	{
		if (dbTemp == doubletemp) //dbTemp������
		{
			if (increment < 0.5)
			{
				mintotalincrement_X_Plus += increment;
				if (mintotalincrement_X_Plus >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) + 1;
					mintotalincrement_X_Plus -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			else
			{
				maxtotalincrement_X_Plus += (1 - increment);
				if (maxtotalincrement_X_Plus >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) - 1;
					maxtotalincrement_X_Plus -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			nTemp = nabsTemp;
		} 
		else //dbTemp�Ǹ���
		{
			if (increment < 0.5)
			{
				mintotalincrement_X_Minus += increment;
				if (mintotalincrement_X_Minus >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) + 1;
					mintotalincrement_X_Minus -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			else
			{
				maxtotalincrement_X_Minus += (1 - increment);
				if (maxtotalincrement_X_Minus >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) - 1;
					maxtotalincrement_X_Minus -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			nTemp = -nabsTemp;
		}
	} 
	else
	{
		if (dbTemp == doubletemp) //dbTemp������
		{
			if (increment < 0.5)
			{
				mintotalincrement_Y_Plus += increment;
				if (mintotalincrement_Y_Plus >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) + 1;
					mintotalincrement_Y_Plus -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			else
			{
				maxtotalincrement_Y_Plus += (1 - increment);
				if (maxtotalincrement_Y_Plus >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) - 1;
					maxtotalincrement_Y_Plus -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			nTemp = nabsTemp;
		} 
		else //dbTemp�Ǹ���
		{
			if (increment < 0.5)
			{
				mintotalincrement_Y_Minus += increment;
				if (mintotalincrement_Y_Minus >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) + 1;
					mintotalincrement_Y_Minus -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			else
			{
				maxtotalincrement_Y_Minus += (1 - increment);
				if (maxtotalincrement_Y_Minus >= 1.0)
				{
					nabsTemp = (int)(doubletemp + 0.5) - 1;
					maxtotalincrement_Y_Minus -= 1;
				}
				else
				{
					nabsTemp = (int)(doubletemp + 0.5);
				}
			}
			nTemp = -nabsTemp;
		}
	}

	return nTemp;
}

BOOL IsRgnIntersectant(const CRgn *pRgn1, const CRgn *pRgn2)
/*�ж����������Ƿ��ཻ
������pRgn1��pRgn2 Ҫ�ж��ཻ����������
����ֵ�����������ཻ����TRUE�����򷵻�FALSE
*/
{
	if (pRgn1 == NULL || pRgn2 == NULL)
	{
		return FALSE;
	}

	CRgn rgnNew;
	rgnNew.CreateRectRgn(0, 0, 50, 50);
	int nResult = rgnNew.CombineRgn(pRgn1, pRgn2, RGN_AND);

	if (nResult == ERROR || nResult == NULLREGION)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
	//rgnNew.DeleteObject();
}

BOOL IsLineRgnIntersectant(const CPoint& ptLineA, const CPoint& ptLineB, const CRgn *pRgn)
/*�ж�ֱ���������Ƿ��ཻ
������
ptLineA��ptLineB,���룬ֱ�ߵ������˵�
pRgn�����룬Ҫ�жϵ�����
����ֵ�����ֱ���������ཻ����TRUE,���򷵻�FALSE
*/
{
	if (pRgn == NULL)
	{
		return FALSE;
	}

	CPoint ptArray[4];
	int nSelectSegment = 10;  //ѡ�����ʱ�Ĵ�������Χ

	ptArray[0].x = ptLineA.x - nSelectSegment;
	ptArray[0].y = ptLineA.y - nSelectSegment;

	ptArray[1].x = ptLineB.x + nSelectSegment;
	ptArray[1].y = ptLineB.y - nSelectSegment;

	ptArray[2].x = ptLineB.x + nSelectSegment;
	ptArray[2].y = ptLineB.y + nSelectSegment;

	ptArray[3].x = ptLineA.x - nSelectSegment;
	ptArray[3].y = ptLineA.y + nSelectSegment;

	CRgn *pLineRgn = new CRgn();
	if (!pLineRgn->CreatePolygonRgn(ptArray, 4, ALTERNATE))
	{
		delete pLineRgn;
		return FALSE;
	}

	if (IsRgnIntersectant(pLineRgn, pRgn))
	{
		pLineRgn->DeleteObject();
		delete pLineRgn;
		return TRUE;
	}

	/*CRgn newRgn;
	newRgn.CreateRectRgn(0, 0, 50, 50);

	int nResult = newRgn.CombineRgn(pLineRgn, pRgn, RGN_AND);
	if (nResult != ERROR && nResult != NULLREGION)
	{
		delete pLineRgn;
		return TRUE;
	}*/

	ptArray[0].x = ptLineA.x - nSelectSegment;
	ptArray[0].y = ptLineA.y + nSelectSegment;

	ptArray[1].x = ptLineA.x + nSelectSegment;
	ptArray[1].y = ptLineA.y - nSelectSegment;

	ptArray[2].x = ptLineB.x + nSelectSegment;
	ptArray[2].y = ptLineB.y - nSelectSegment;

	ptArray[3].x = ptLineB.x - nSelectSegment;
	ptArray[3].y = ptLineB.y + nSelectSegment;

	delete pLineRgn;
	pLineRgn = NULL;
	pLineRgn = new CRgn();
	if (!pLineRgn->CreatePolygonRgn(ptArray, 4, ALTERNATE))
	{
		delete pLineRgn;
		return FALSE;
	}

	if (IsRgnIntersectant(pLineRgn, pRgn))
	{
		pLineRgn->DeleteObject();
		delete pLineRgn;
		return TRUE;
	}

/*
	CRgn newRgn1;
	newRgn1.CreateRectRgn(0, 0, 50, 50);

	nResult = newRgn1.CombineRgn(pLineRgn, pRgn, RGN_AND);
	if (nResult != ERROR && nResult != NULLREGION)
	{
		delete pLineRgn;
		return TRUE;
	}*/

	delete pLineRgn;
	return FALSE;
}

CString GetSysTempDirector()
{
	TCHAR szTempDir[MAX_PATH];
	GetWindowsDirectory(szTempDir, MAX_PATH);

	CString strTemp = szTempDir;
	strTemp += _T("\\Temp");

	return strTemp;
}

CString GetCurrentPath()
{
	TCHAR szPath[MAX_PATH];
	
	GetModuleFileName(NULL, szPath, MAX_PATH); 

	CString strPath = szPath;
	int nPos; 
	nPos = strPath.ReverseFind('\\'); 
	strPath = strPath.Left(nPos); 

	return strPath;
}

unsigned long GenerateSSRC(void)
{
#ifdef WIN32
	LARGE_INTEGER frequence, privious;
	if(!QueryPerformanceFrequency( &frequence))//ȡ�߾������м�������Ƶ��
	{
		return timeGetTime();
	}

	if (!QueryPerformanceCounter( &privious ))
	{
		return timeGetTime();
	}

	DWORD dwRet = (DWORD)(1000000 * privious.QuadPart / frequence.QuadPart ); //���㵽΢����

	return dwRet;//΢��
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec; 
#endif
}

void DisableTabFeedback( HWND hWnd )
{

	DWORD_PTR dwHwndTabletProperty = 
		TABLET_DISABLE_PRESSANDHOLD | 
		TABLET_DISABLE_PENTAPFEEDBACK | 
		TABLET_DISABLE_PENBARRELFEEDBACK |
		TABLET_DISABLE_SMOOTHSCROLLING|
		TABLET_DISABLE_FLICKS; 

	ATOM atom = GlobalAddAtom(MICROSOFT_TABLETPENSERVICE_PROPERTY);  
	SetProp(hWnd, MICROSOFT_TABLETPENSERVICE_PROPERTY, reinterpret_cast<HANDLE>(dwHwndTabletProperty));
	GlobalDeleteAtom(atom);

}

DWORD GetSystemVersion()
{

	SYSTEM_INFO info;                                   //��SYSTEM_INFO�ṹ�ж�64λAMD������ 

	GetSystemInfo(&info);                               //����GetSystemInfo�������ṹ 

	OSVERSIONINFOEX os; 

	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   

	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{ 
		return os.dwMajorVersion;
	} 
	else 
	{
		return 4;
	}

} 
