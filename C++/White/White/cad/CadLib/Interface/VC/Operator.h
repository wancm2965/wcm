#ifndef OPERATOR_H
#define OPERATOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DXFDef.h"
#include "DrawingDef.h"

int RotateXForm(HDC hdc, double x1, double y1, double xScale = 1.0f, double yScale = 1.0f, double Rotation = 0.0f);//旋转坐标系
void ResumeXForm(HDC hdc, int n);//还原坐标系

void PointRotationPoint(const REALPOINT& ptCenter, double dbAngle, const REALPOINT& ptSrc, REALPOINT& ptDes);
double PointRotationPoint(const REALPOINT& ptCenter, const REALPOINT& ptBegin, const REALPOINT& ptEnd);
REALPOINT PointRotationPoint(const REALPOINT& ptCenter, const REALPOINT& ptSrc, double dbAngle, double xScale, double yScale);

double CalDistancePointToLine(const REALPOINT& pt, const REALPOINT& ptA, const REALPOINT& ptB);
double CalDistancePointToPoint(const REALPOINT& ptA, const REALPOINT& ptB);

//function use calculation Nurbs Spline Curve
double N(int n, int i, double t, double *Knot);
REALPOINT NURBS_3(REALPOINT *	CP, double *Knot, int j, double t);

//function use calculation of X or Y axisymmetric angle
double GetFixedAngle(double a);
double AngleXAxisymmetric(double angle, int flag = 0);
double AngleYAxisymmetric(double angle, int flag = 0);
double CalCadAngleToGdiAngle(double angle);

//funciton use calculation of LWPolyLine and PolyLine
BOOL EqualDouble(double num, double num2);
BOOL EqualPoint(const REALPOINT& Point, const REALPOINT& Point2);
BOOL EqualPoint2D(const POINT2D& Point, const POINT2D& Point2);

int DoubleCmp(double num, double num2);
int PointIdCmp(const REALPOINT& Point, const REALPOINT& Point2, int id);
int Point2DIdCmp(const POINT2D& Point, const POINT2D& Point2, int id);

double Distance(const REALPOINT& Point, const REALPOINT& Point2);
double Distance2D(const POINT2D & Point, const POINT2D& Point2);

int ToInt32(BOOL Value);
int Round(double num);
double Fval(double num, double num2);
double Atan2ToRad(double num, double num2, double num3, double num4);
double Atan2(const POINT2D& Point, const POINT2D& Point2);
double Sqr(double x);
double round(double r);

void SetPointValue(REALPOINT &p, int id, double value);
double GetPointValue(const REALPOINT &p, int id);

void SetPoint2DValue(POINT2D &p, int id, double value);
double GetPoint2DValue(const POINT2D &p, int id);

template <typename class T> void SwapT(T &A, T &B)
{
	T num = A;
	A = B;
	B = num;
}
template <typename class T> T *ReadSimpleListData(SimpleList <T> &List) //new出来的指针要delete
{
	T *pData = NULL;
	int Count = List.getSize();
	if (Count != 0)
	{
		pData = new T[Count];
		if (pData != NULL)
		{
			ZeroMemory(pData, sizeof(T) * Count);
			int n = Count - 1;
			List.start();
			while (List.next()) {
				pData[n] = List.get();
				n--;
			}
		}
	}
	return pData;
}
#endif // OPERATOR_H