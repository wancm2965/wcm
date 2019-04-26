

#if !defined(AFX_EXEC_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_)
#define AFX_EXEC_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <tpcshrd.h>

//计算两点之间的距离
double CalDistancePointToPoint(const CPoint ptA, const CPoint ptB);

//计算点到直线之间的距离
double CalDistancePointToLine(const CPoint pt, const CPoint ptA, const CPoint ptB);

//点旋转一定角度后的坐标
void PointRotationPoint(const CPoint& ptCenter, double dbAngle, const CPoint& ptSrc, CPoint& ptDes);

//点旋转后的角度
double PointRotationPoint(const CPoint& ptCenter, const CPoint& ptBegin, const CPoint& ptEnd);

//两直线的交点
CPoint CalLineCross(const CPoint& fistLinePtBegin, const CPoint& fistLinePtEnd, const CPoint& eraseLinePtBegin, const CPoint& eraseLinePtEnd);

//双精度数四舍五入成整数，区分点的X和Y坐标，X坐标为TURE，Y坐标为FALSE。
int Rounding(double dbTemp, BOOL bIsX_axis, int nIndex, BOOL bIsToZero = FALSE);

//双精度数四舍五入成整数，区分点的X和Y坐标，X坐标为TURE，Y坐标为FALSE。（用于一个点，比如点按钮缩放时）
int RoundingOnePt(double dbTemp, BOOL bIsX_axis);

//判断两个区域是否相交
BOOL IsRgnIntersectant(const CRgn *pRgn1, const CRgn *pRgn2);

//判断直线是否与区域相交
BOOL IsLineRgnIntersectant(const CPoint& ptLineA, const CPoint& ptLineB, const CRgn *pRgn);

//得到系统temp文件夹目录
CString GetSysTempDirector();

CString GetCurrentPath();   //获取当前执行程序的目录

unsigned long GenerateSSRC();

void DisableTabFeedback(HWND hWnd);

DWORD GetSystemVersion();
CString GetExePath(void);
#endif