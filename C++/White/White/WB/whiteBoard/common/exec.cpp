

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
*****                           计算直线的长度                       *****
*************************************************************************/
double CalDistancePointToPoint(const CPoint ptA, const CPoint ptB)
{
	//计算两点间的距离
	double dx = ptA.x - ptB.x;
	double dy = ptA.y - ptB.y;
	return sqrt(dx*dx + dy*dy);
}

/*************************************************************************
*****                        计算点到直线的距离                     *****
*************************************************************************/
double CalDistancePointToLine(const CPoint pt, const CPoint ptA, const CPoint ptB)
{
	
	//计算点pt到线段(A,B)的距离
	double length; /* 直线长度 */
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
/*计算点绕中心点旋转后的坐标
参数：
ptCenter：输入，中心点坐标
dbAngle：输入，旋转角度，顺时针为正
ptSrc：输入，点旋转前的坐标
ptDes：输出：点旋转后的坐标
返回值：成功返回TRUE,否则返回FALSE
*/
{
	//角度转弧度
	double dbRadian = dbAngle * PI / 180; 

	ptDes.x = (LONG)((ptSrc.x - ptCenter.x) * cos(dbRadian) + (ptSrc.y - ptCenter.y) * sin(dbRadian) + ptCenter.x + 0.5);
	ptDes.y = (LONG)(-(ptSrc.x - ptCenter.x) * sin(dbRadian) + (ptSrc.y - ptCenter.y) * cos(dbRadian) + ptCenter.y + 0.5);
}
/*
double PointRotationPoint(const CPoint& ptCenter, const CPoint& ptBegin, const CPoint& ptEnd)
/*计算点绕中心点旋转后的角度
参数：
ptCenter：输入，旋转中心点坐标
ptBegin：输入，旋转开始点
ptEnd：输入，旋转结束点
返回值：旋转角度
*//*
{
	double dbAngle = 0;
	double dbRadian = 0; //弧度

	//计算结束点到中点与起点直线的距离
	double dbDistance = CalDistancePointToLine(ptEnd, ptCenter, ptBegin);

	//计算中点与起点直线的距离
	double dbLine = CalDistancePointToPoint(ptCenter, ptEnd);

	dbRadian = asin(dbDistance / dbLine);

	dbAngle = dbRadian * 180 / PI;

	return dbAngle;
}
*/

double PointRotationPoint(const CPoint& ptCenter, const CPoint& ptBegin, const CPoint& ptEnd)
	/*计算点绕中心点旋转后的角度
	参数：
	ptCenter：输入，旋转中心点坐标
	ptBegin：输入，旋转开始点
	ptEnd：输入，旋转结束点
	返回值：旋转角度
	*/
{
	double dbAngle = 0;
	double dbRadian = 0; //弧度	
	int y;	//用于判断终点在起点与中心点所成直线的上下
	//计算结束点到基线的距离
	double dbDistance = CalDistancePointToLine(ptEnd, ptCenter, ptBegin);

	//计算中心点与结束点直线的距离
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

	dbAngle = dbRadian * 180 / PI;	//偏移角度		
	if(ptBegin.x == ptCenter.x && ptEnd.y < ptCenter.y)	//当起点与中心点组成的直线垂直，且终点在中心点之上时
	{
		if (ptEnd.x < ptBegin.x)	//终点与起点位置比较
		{
			return dbAngle;
		} 
		else
		{
			return 0 - dbAngle;
		}		
	}
	if(ptBegin.x == ptCenter.x && ptEnd.y > ptCenter.y)	//当起点与中心点组成的直线垂直，且终点在中心点之下时
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
	//鼠标位于以中心点为原点的第一象限
	if(ptEnd.x >= ptCenter.x && ptEnd.y < ptCenter.y)
	{
		if (ptBegin.x < ptCenter.x && ptBegin.y < ptCenter.y)	//如果是从第二象限移动至第一象限
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
			dbAngle = 0 - dbAngle;//偏移角度
			return dbAngle;
		}
	}
	//鼠标位于以中心点为原点的第二象限
	if(ptEnd.x < ptCenter.x && ptEnd.y <= ptCenter.y)
	{
		if (ptBegin.x > ptCenter.x && ptBegin.y < ptCenter.y)	//如果是从第一象限移动至第二象限
		{
			return dbAngle;
		}		
		if(ptEnd.y > y)
		{
			return dbAngle;
		}
		else
		{
			dbAngle = 0 - dbAngle;//偏移角度
			return dbAngle;
		}
	}
	//鼠标位于以中心点为原点的第三象限
	if(ptEnd.x <= ptCenter.x && ptEnd.y > ptCenter.y)
	{
		if (ptBegin.x > ptCenter.x && ptBegin.y > ptCenter.y)	//如果是从第四象限移动至第三象限
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
			dbAngle = 0 - dbAngle;//偏移角度
			return dbAngle;
		}
	}
	//鼠标位于以中心点为原点的第四象限
	if(ptEnd.x > ptCenter.x && ptEnd.y >= ptCenter.y)
	{
		if (ptBegin.x < ptCenter.x && ptBegin.y > ptCenter.y)	//如果是从第三象限移动至第四象限
		{
			return dbAngle;
		}		
		if(ptEnd.y < y)
		{
			return dbAngle;
		}
		else
		{
			dbAngle = 0 - dbAngle;//偏移角度
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
	*直线两点公式：x = (y - y1) * (x2 - x1) / (y2 - y1) + x1
	*          或  y = (x - x1) * (y2 - y1) / (x2 - x1) + y1
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
		if (dbTemp == doubletemp) //dbTemp是正数
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
		else //dbTemp是负数
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
		if (dbTemp == doubletemp) //dbTemp是正数
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
		else //dbTemp是负数
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
		if (dbTemp == doubletemp) //dbTemp是正数
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
		else //dbTemp是负数
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
		if (dbTemp == doubletemp) //dbTemp是正数
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
		else //dbTemp是负数
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
/*判断两个区域是否相交
参数：pRgn1、pRgn2 要判断相交的两个区域
返回值：两个区域相交返回TRUE，否则返回FALSE
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
/*判断直线与区域是否相交
参数：
ptLineA、ptLineB,输入，直线的两个端点
pRgn，输入，要判断的区域
返回值：如果直线与区域相交返回TRUE,否则返回FALSE
*/
{
	if (pRgn == NULL)
	{
		return FALSE;
	}

	CPoint ptArray[4];
	int nSelectSegment = 10;  //选择对象时的触发区域范围

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
	if(!QueryPerformanceFrequency( &frequence))//取高精度运行计数器的频率
	{
		return timeGetTime();
	}

	if (!QueryPerformanceCounter( &privious ))
	{
		return timeGetTime();
	}

	DWORD dwRet = (DWORD)(1000000 * privious.QuadPart / frequence.QuadPart ); //换算到微秒数

	return dwRet;//微秒
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

	SYSTEM_INFO info;                                   //用SYSTEM_INFO结构判断64位AMD处理器 

	GetSystemInfo(&info);                               //调用GetSystemInfo函数填充结构 

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
