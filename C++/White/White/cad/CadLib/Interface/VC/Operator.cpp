#include "stdafx.h"
#include "Operator.h"

int RotateXForm(HDC hdc,double x1,double y1, double xScale, double yScale, double Rotation)
{
	//旋转
	int nGraphicsMode = SetGraphicsMode(hdc, GM_ADVANCED);

	double fangle=-Rotation*PI/180;

	XFORM xform;
	
	//加缩放因子
    xform.eM11 = (float)cos(fangle)*(float)xScale;
	xform.eM12 = (float)sin(fangle)*(float)xScale; 
	xform.eM21 = (float)-sin(fangle)*(float)yScale;
	xform.eM22 = (float)cos(fangle)*(float)yScale;
	
	xform.eDx = (float)( x1- cos(fangle)*x1 +sin(fangle)*y1);
	xform.eDy = (float)( y1- cos(fangle)*y1 - sin(fangle)*x1);

	SetWorldTransform(hdc, &xform); 
	
	return nGraphicsMode;
}

void ResumeXForm(HDC hdc,int nGraphicsMode)
{
	// 恢复DC	
	XFORM xform;

	xform.eM11 = (float)1.0; 
	xform.eM12 = (float)0;
	xform.eM21 = (float)0;
	xform.eM22 = (float)1.0;
	xform.eDx = (float)0;
	xform.eDy = (float)0;   
	
	SetWorldTransform(hdc, &xform);
	SetGraphicsMode(hdc, nGraphicsMode);
}

void PointRotationPoint(const REALPOINT& ptCenter, double dbAngle, const REALPOINT& ptSrc, REALPOINT& ptDes) //顺时针旋转
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

double PointRotationPoint(const REALPOINT& ptCenter, const REALPOINT& ptBegin, const REALPOINT& ptEnd)
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

	y = (int)((ptEnd.x - ptCenter.x) * (ptBegin.y - ptCenter.y) / (ptBegin.x - ptCenter.x) + ptCenter.y);
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

REALPOINT PointRotationPoint(const REALPOINT& ptCenter, const REALPOINT& ptSrc, double dbAngle, double xScale, double yScale) //逆时针旋转
{
	REALPOINT ptDes;
	ZeroMemory(&ptDes, sizeof(REALPOINT));
	double dbRadian = dbAngle * PI / 180; 

	ptDes.x = (ptSrc.x - ptCenter.x) * cos(dbRadian) * xScale - (ptSrc.y - ptCenter.y) * sin(dbRadian) * yScale + ptCenter.x;
	ptDes.y = (ptSrc.x - ptCenter.x) * sin(dbRadian) * xScale + (ptSrc.y - ptCenter.y) * cos(dbRadian) * yScale + ptCenter.y;
	return ptDes;
}

double CalDistancePointToLine(const REALPOINT& pt, const REALPOINT& ptA, const REALPOINT& ptB)
{
	//计算点pt到线段(A,B)的距离
	double length; /* 直线长度 */
	double a = ptB.y-ptA.y;
	double b = ptA.x-ptB.x;
	double c = ptB.x * ptA.y - ptA.x * ptB.y;
	return length = fabs(a * pt.x + b *pt.y + c)/sqrt(a * a + b * b);
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

double CalDistancePointToPoint(const REALPOINT& ptA, const REALPOINT& ptB)
{
	//计算两点间的距离
	double dx = ptA.x - ptB.x;
	double dy = ptA.y - ptB.y;
	return sqrt(dx*dx + dy*dy);
}

/*该算法用于Nurbs样条曲线*/
double N(int n, int i, double t, double *Knot)
{
	double v1, d1, v2, d2;

	if (!n)
	{
		return ((Knot[i] <= t) && (t < Knot[i + 1])) ? (float)1 : (float)0;
	}

	d1 = (Knot[i + n] - Knot[i]);
	v1 = (t - Knot[i]) * N(n - 1, i, t, Knot);
	v1 = (d1 == 0) ? 0:(v1 / d1);

	d2 = (Knot[i + n + 1] - Knot[i + 1]);
	v2 = (Knot[i + n + 1] - t) * N(n - 1, i + 1, t, Knot);
	v2 = (d2 == 0) ? 0 : (v2 / d2);
	return (v1 + v2);
} 

REALPOINT NURBS_3(REALPOINT *CP, double *Knot, int j, double t)
{
	REALPOINT R;
	int i;
	double Ni;

	R.x = 0;
	R.y = 0;
	R.z = 0;

	for (i = j - 3; i < j + 1; i++)
	{
		Ni = N(3, i, t, Knot);
		R.x = R.x + CP[i].x * Ni;
		R.y = R.y + CP[i].y * Ni;
		R.z = R.z + CP[i].z * Ni;
	}
	return R;
}

/*下列函数用于角转换*/
double GetFixedAngle(double a)
{
	if (a>2*PI)
	{
		do 
		{
			a-=2*PI;
		} while (a>2*PI);
	}
	else if (a<0)
	{
		do 
		{
			a+=2*PI;
		} while (a<0);
	}
	return a;
}

double AngleXAxisymmetric(double angle, int flag/* = 0*/)
{
	double rot = GetFixedAngle(angle);

	//return radian or angle vaule 
	return ((flag == 0) ? (2*PI - rot) : (360 - rot * 180/PI));
}

double AngleYAxisymmetric(double angle, int flag/* = 0*/)
{
	double rot = 0.0;
	double m_Angle = GetFixedAngle(angle);
	//第一 二象限的角
	if(m_Angle <= PI && m_Angle >=0)
		rot = PI - m_Angle;
	else 
		rot = 3*PI - m_Angle;

	//return radian or angle vaule 
	return ((flag == 0) ?  rot : rot * 180/PI);
}

double CalCadAngleToGdiAngle(double angle)
{
    double rot = GetFixedAngle(angle)*180/PI;	
	if(rot >= 0 && rot <= 180)
		return (-rot);
	else
		return (360 - rot);
}

/*下列函数用于点操作*/
BOOL EqualDouble(double num, double num2)
{
	return fabs(num - num2) < 1E-06;
}

BOOL EqualPoint(const REALPOINT& Point, const REALPOINT& Point2)
{
	return (EqualDouble(Point.x, Point2.x) && EqualDouble(Point.y, Point2.y) && EqualDouble(Point.z, Point2.z));
}

BOOL EqualPoint2D(const POINT2D& Point, const POINT2D& Point2)
{
	return (EqualDouble(Point.x, Point2.x) && EqualDouble(Point.y, Point2.y));
}

int DoubleCmp(double num, double num2)
{
	if (!EqualDouble(num, num2))
	{
		if (num > num2)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	return 0;
}

int PointIdCmp(const REALPOINT& Point, const REALPOINT& Point2, int id)
{
	double num = GetPointValue(Point, id);
	double num2 = GetPointValue(Point2, id);
	return DoubleCmp(num, num2);
}

int Point2DIdCmp(const POINT2D& Point, const POINT2D& Point2, int id)
{
	double num = GetPoint2DValue(Point, id);
	double num2 = GetPoint2DValue(Point2, id);
	return DoubleCmp(num, num2);
}

double Fval(double num, double num2)
{
	return EqualDouble(num, num2) ? 0.0f : (num - num2);
}

double Distance(const REALPOINT& Point, const REALPOINT& Point2)
{
	double x = Sqr((Point2.x - Point.x));
	double y = Sqr((Point2.y - Point.y));
	double z = Sqr((Point2.z - Point.z));
	return sqrt(x + y + z);
}

double Distance2D(const POINT2D & Point, const POINT2D& Point2)
{
	double x = Sqr((Point2.x - Point.x));
	double y = Sqr((Point2.y - Point.y)); 
	return sqrt(x + y);
}

int ToInt32(BOOL Value)
{
	if (!Value)
	{
		return 0;
	}
	return 1;
}

int Round(double num)
{
	int num2 = (int)round(num);
	if ((double)num2 > num)
	{
		num2--;
	}
	return num2;
}

double Atan2ToRad(double num, double num2, double num3, double num4)
{
	double num5 = atan2(num4 - num2, num3 - num);
	if (num5 < 0.0f)
	{
		num5 += 2 * PI;
	}
	return num5;
}

double Atan2(const POINT2D& Point, const POINT2D& Point2)
{
	double x = Fval(Point2.x, Point.x);
	double y = Fval(Point2.y, Point.y);
	return atan2(y, x);
}

double Sqr(double x)
{
	return x * x;
}

double round(double r)
{
	return (r > 0.0f) ? floor(r + 0.5) : ceil(r - 0.5);
}

//REALPOINT Operator
double GetPointValue(const REALPOINT &p, int id)
{
	double value = 0.0f;
	switch (id)
	{
	case 0:
		value = p.x;
		break;
	case 1:
		value = p.y;
		break;
	case 2:
		value = p.z;
		break;
	default:
		break;
	}
	return value;
}

void SetPointValue(REALPOINT& p, int id, double value)
{
	switch (id)
	{
	case 0:
		p.x = value;
		break;
	case 1:
		p.y = value;
		break;
	case 2:
		p.z = value;
		break;
	default:
		break;
	}
}

double GetPoint2DValue(const POINT2D &p, int id)
{
	double value = 0.0f;
	switch (id)
	{
	case 0:
		value = p.x;
		break;
	case 1:
		value = p.y;
		break;
	default:
		break;
	}
	return value;
}

void SetPoint2DValue(POINT2D& p, int id, double value)
{
	switch (id)
	{
	case 0:
		p.x = value;
		break;
	case 1:
		p.y = value;
		break;
	default:
		break;
	}
}