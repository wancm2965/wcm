

#if !defined(AFX_EXEC_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_)
#define AFX_EXEC_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <tpcshrd.h>

//��������֮��ľ���
double CalDistancePointToPoint(const CPoint ptA, const CPoint ptB);

//����㵽ֱ��֮��ľ���
double CalDistancePointToLine(const CPoint pt, const CPoint ptA, const CPoint ptB);

//����תһ���ǶȺ������
void PointRotationPoint(const CPoint& ptCenter, double dbAngle, const CPoint& ptSrc, CPoint& ptDes);

//����ת��ĽǶ�
double PointRotationPoint(const CPoint& ptCenter, const CPoint& ptBegin, const CPoint& ptEnd);

//��ֱ�ߵĽ���
CPoint CalLineCross(const CPoint& fistLinePtBegin, const CPoint& fistLinePtEnd, const CPoint& eraseLinePtBegin, const CPoint& eraseLinePtEnd);

//˫������������������������ֵ��X��Y���꣬X����ΪTURE��Y����ΪFALSE��
int Rounding(double dbTemp, BOOL bIsX_axis, int nIndex, BOOL bIsToZero = FALSE);

//˫������������������������ֵ��X��Y���꣬X����ΪTURE��Y����ΪFALSE��������һ���㣬����㰴ť����ʱ��
int RoundingOnePt(double dbTemp, BOOL bIsX_axis);

//�ж����������Ƿ��ཻ
BOOL IsRgnIntersectant(const CRgn *pRgn1, const CRgn *pRgn2);

//�ж�ֱ���Ƿ��������ཻ
BOOL IsLineRgnIntersectant(const CPoint& ptLineA, const CPoint& ptLineB, const CRgn *pRgn);

//�õ�ϵͳtemp�ļ���Ŀ¼
CString GetSysTempDirector();

CString GetCurrentPath();   //��ȡ��ǰִ�г����Ŀ¼

unsigned long GenerateSSRC();

void DisableTabFeedback(HWND hWnd);

DWORD GetSystemVersion();
CString GetExePath(void);
#endif