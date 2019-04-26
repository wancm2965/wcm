/*-------------------------------------------------------------------*\
|  CadLib Version 2.1                                                 |
|  Written by Omid Shahabi <omid_shahabi@hotmail.com>                 |
|  Copyright ?2002-2004                                              |
|  Pars Technology Development Co.                                    |
|                                                                     |
|  This software is distributed on an "AS IS" basis, WITHOUT WARRANTY |
|  OF ANY KIND, either express or implied.                            |
|                                                                     |
|                                                                     |
|  DXFDef.h                                                           |
\*-------------------------------------------------------------------*/

#ifndef DXFDEF_H
#define DXFDEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\DXFFile\Base\SimpleList.h"
// Handle to DXF
DECLARE_HANDLE(HDXF);
//#include "DrawingDef.h"

// Section Definitions
#define	SEC_NOTSET		0x0000
#define	SEC_HEADER		0x0001
#define	SEC_CLASSES		0x0002
#define	SEC_TABLES		    0x0004
#define	SEC_BLOCKS		0x0008
#define	SEC_ENTITIES	    0x0010
#define	SEC_OBJECTS		0x0020
#define	SEC_UNKNOWN	0x0040
typedef DWORD OBJHANDLE;

// Table Type Definitions
#define	TAB_NOTSET		    0x0000
#define	TAB_APPID		        0x0001
#define	TAB_BLOCKRECORD	0x0002
#define	TAB_DIMSTYLE      	0x0004
#define	TAB_LAYER	        	0x0008
#define	TAB_LTYPE	        	0x0010
#define	TAB_STYLE	        	0x0020
#define	TAB_UCS		        	0x0040
#define	TAB_VIEW	            	0x0080
#define	TAB_VPORT	        	0x0100
#define	TAB_UNKNOWN		0x0200

// Text Justification Types
#define	TJ_LEFT		        	    0x0000
#define	TJ_CENTER	        	0x0001
#define	TJ_RIGHT	            	0x0002
#define	TJ_ALIGNED	         	0x0003
#define	TJ_MIDDLE	        	0x0004
#define	TJ_FIT	    	        	0x0005
#define	TJ_BOTTOMLEFT	    0x0100
#define	TJ_BOTTOMCENTER	0x0101
#define	TJ_BOTTOMRIGHT	0x0102
#define	TJ_MIDDLELEFT    	0x0200
#define	TJ_MIDDLECENTER	0x0201
#define	TJ_MIDDLERIGHT  	0x0202
#define	TJ_TOPLEFT	        	0x0300
#define	TJ_TOPCENTER	        0x0301
#define	TJ_TOPRIGHT	    	0x0302

// MText Justification Types
#define	MTJ_TOPLEFT		        0x0001
#define	MTJ_TOPCENTER	        0x0002
#define	MTJ_TOPRIGHT		    0x0003
#define	MTJ_MIDDLELEFT	        0x0004
#define	MTJ_MIDDLECENTER	0x0005
#define	MTJ_MIDDLERIGHT	    0x0006
#define	MTJ_BOTTOMLEFT       0x0007
#define	MTJ_BOTTOMCENTER	0x0008
#define	MTJ_BOTTOMRIGHT	    0x0009

// PolyLine Flag Definitions
#define	POLYLINE_CLOSED			     0x0001
#define	POLYLINE_CURVEFIT		     0x0002
#define	POLYLINE_SPLINEFIT		     0x0004
#define	POLYLINE_POLYLINE		     0x0008
#define	POLYLINE_POLYMESH			 0x0010
#define	POLYLINE_NCLOSED		     0x0020
#define	POLYLINE_POLYFACEMESH	 0x0040
#define	POLYLINE_CONTINUELT		 0x0080

//  Structures
//  Base ----------
typedef struct tag_POINT2D {
	double x;
	double y;
} POINT2D, *PPOINT2D;

typedef struct tag_REALPOINT
{
	double	x;
	double	y;
	double	z;
} REALPOINT, *PREALPOINT;

typedef struct tag_DPOINT
{
	REALPOINT Point;
	REALPOINT Point2;
} DPOINT, PDPOINT;

typedef struct tag_REALRECT
{
	double	top;
	double	left;
	double	bottom;
	double	right;
} REALRECT, *PREALRECT;

//  Header --------
typedef struct tag_DXFHEADER{
	char   CadVer[512]; //CAD版本号
}DXFHEADER, *PDXFHEADER;

//  TABLES --------
typedef struct tag_DXFLAYER
{
	char	Name[512];					 // 图层名
	int		StandardFlags;		     // 标准标记值
	int		Color;						 // 图层颜色
	char	LineType[512];				 // 线型
} DXFLAYER, *PDXFLAYER;

typedef struct tag_DXFLTYPE
{
	char	Name[512];					    // 线型名
	int		StandardFlags;				// 标准标记值
	char	DescriptiveText[512];		// 线型的说明文字
	int		ElementsNumber;			// 线型元素的数目
	double	TotalPatternLength;		// 图案总长度
	double	Elements[30];				// 线元素
} DXFLTYPE, *PDXFLTYPE;

typedef struct tag_DXFSTYLE
{
	char	Name[512];					          // 样式名
	char	PrimaryFontFilename[512];	  // 主要字体文件名
	char FaceName[512];                     // 字体名称
	int		StandardFlags;				      // 标准标记值
	int		TextGenerationFlags;		  // 文字生成标志 -> 2=Text is backward  4=Text is upside down
	double	FixedTextHeight;			      // 固定的文字高度
	double	WidthFactor;				          // 宽度因子
	double	Height;						          // 上次使用的高度
	double	ObliqueAngle;			          // 倾斜角度
} DXFSTYLE, *PDXFSTYLE;

typedef struct tag_DXFDIMSTYLE
{
	char 	Name[512];					           // 标注样式名
	int		StandardFlags;				        // 标准标记值
	int		DIMCLRD;					        // Dimension line & Arrow heads color
	double	DIMDLE;						        // Dimension line size after Extensionline
	int		DIMCLRE;					            // Extension line color
	double	DIMEXE;						        // Extension line size after Dimline
	double	DIMEXO;						        // Offset from origin
	char	DIMBLK1[512];				            // 第一个箭头
	char	DIMBLK2[512];				            // 第二个箭头
	double	DIMASZ;						        // 箭头大小
	char	DIMTXSTY[512];				        // 文字样式
	DWORD	DIMTXSTYObjhandle;       // 文字样式对象句柄
	int		DIMCLRT;					            // 文字颜色
	double	DIMTXT;						        // 文字高度
	int		DIMTAD;						        // Vertical Text Placement
	double	DIMGAP;						        // 
} DXFDIMSTYLE, *PDXFDIMSTYLE;

//  BLOCKS --------
typedef struct tag_DXFBLOCKHEADER
{
	char		Name[512];		    // 块名
	char		Flags;					// 块类型标记
	REALPOINT	BasePoint;		// 基准点
	char		LayerName[512];	// 图层名
} DXFBLOCKHEADER, *PDXFBLOCKHEADER;

//  ENTITIES ------
typedef struct tag_DXFENTITYHEADER
{
	unsigned short	EntityType;			// 图元类型
	char		LayerName[512];			// 图层名
	char		LTypeName[512];			// 线型名
	short		Color;					// 颜色
	double		Thickness;				// 厚度
	double		LineTypeScale;			// 线型比例
	double		ExtrusionDirection[3];	// 拉伸方向
	short		LineWeight;				// 线重
} DXFENTITYHEADER, *PDXFENTITYHEADER;

//  2DCurve曲线
typedef struct tag_DXFENTCURVE2D {
	unsigned short EdgeType;    //0 = 多段线；1 = 直线；2 = 圆弧；3 = 椭圆弧；4 = 样条曲线
	POINT2D StartPoint;	       //起点坐标
	POINT2D EndPoint;             //终点坐标
	PPOINT2D pCoordPoint;     //坐标点
	int CoordCount;                  //坐标点个数
} DXFENTCURVE2D, *PDXFENTCURVE2D;

//  2DLine直线
typedef struct tag_DXFENTLINE2D : tag_DXFENTCURVE2D {
} DXFENTLINE2D, *PDXFENTLINE2D;

//  2DArc圆弧
typedef struct tag_DXFENTARC2D : public tag_DXFENTCURVE2D {
	POINT2D	Point0;                        //圆心
	double		Radius;                        //半径
	double		StartAngle;                  //起始角度
	double		EndAngle;		               //结束角度
	BOOL		AnticlockWiseFlag;      //"逆时针标志" 组码：73
} DXFENTARC2D, *PDXFENTARC2D;

//  2DEllipse椭圆
typedef struct tag_DXFENTELLIPSE2D : public tag_DXFENTCURVE2D {
	POINT2D	CenterPoint;
	POINT2D	MajorAxisEndPoint;
	double		MinorToMajorRatio;
	double		StartParam;
	double		EndParam;
	BOOL		AnticlockWiseFlag; //"逆时针标志" 组码：73
} DXFENTELLIPSE2D, *PDXFENTELLIPSE2D;

//  2DSpline样条曲线
typedef struct tag_DXFENTSPLINE2D : public tag_DXFENTCURVE2D {
	int SplineOrder;                       //样条曲线阶数,组码：94
	int SplineMarks;                       //样条曲线标志,组码：73
	int NodeNum;		                   //节点数,组码：95
	int ControlNodeNum;              //控制点数，组码：96
	int FittingPointNum;                //拟合点数,组码：97
	POINT2D StartTangent;   	       //起点切向
	POINT2D EndTangent; 	           //端点切向
	double *pNodePoint;               //节点值（多个条目）,组码：40
	double Weight;                        //权值,组码：42
	SimpleList<POINT2D> *pControlNode;  //控制点值
	SimpleList<POINT2D> *pFittingNode;    //拟合点值
} DXFENTSPLINE2D,*PDXFENTSPLINE2D;

//  2DVertex顶点
typedef struct tag_DXFENTVERTEX2D {
	POINT2D		Point;             //点坐标
	double			Bulge;			  //凸度
} DXFENTVERTEX2D, *PDXFENTVERTEX2D;

//  2DPolyLine多段线
typedef struct tag_DXFENTPOLYLINE2D : public tag_DXFENTCURVE2D {
	unsigned short	Flag;                 //多段线标志
	int nVertex;          	                    //多段线顶点数
	PDXFENTVERTEX2D pVertex;	//多段线顶点
} DXFENTPOLYLINE2D, *PDXFENTPOLYLINE2D;

//  Line直线
typedef struct tag_DXFENTLINE {
	REALPOINT Point0;
	REALPOINT Point1;
} DXFENTLINE, *PDXFENTLINE;

//  Arc圆弧
typedef struct tag_DXFENTARC {
	REALPOINT	Point0;                    //圆心
	double		Radius;                        //半径
	double		StartAngle;                  //起始角度
	double		EndAngle;		               //结束角度
} DXFENTARC, *PDXFENTARC;

//  Circle圆
typedef struct tag_DXFENTCIRCLE {
	REALPOINT	Point0;                       //圆心
	double		Radius;                           //半径
} DXFENTCIRCLE, *PDXFENTCIRCLE;

//  Dimension标注
typedef struct tag_DXFENTDIMENSION {
	REALPOINT	DefPoint3;
	REALPOINT	DefPoint4;
	double		DimRotationAngle;                 //标注旋转角度
	REALPOINT	DimLineDefPoint;                  
	char		BlockName[16];                    //块名
	char		DimStyleName[512];                //标注样式名
	WCHAR	DimText[1024];                   //标注文字
} DXFENTDIMENSION, *PDXFENTDIMENSION;

//  Block Insert块参照
typedef struct tag_DXFENTINSERT {
	REALPOINT	Point0;                         //插入点
	double		XScale;                             //拉伸方向
	double		YScale;
	double		ZScale;
	double		RotationAngle;                  //旋转角度
	char		BlockHeaderName[512];       //块名称
	double	StretchDirectZ;                     //拉伸方向Z
} DXFENTINSERT, *PDXFENTINSERT;

//  Vertex顶点图元
typedef struct tag_DXFENTVERTEX {
	REALPOINT		Point;             //点坐标
	unsigned short	Flag;               //标志
	double			StartWidth;        //起始宽度
	double			EndWidth;          //终点宽度
	double			Bulge;			      //凸度
	double      	TangentDir;        //切线方向
	int PolyFaceVertexIndex1;      //多面网格顶点索引（可选；非零时出现）
	int PolyFaceVertexIndex2;      //多面网格顶点索引（可选；非零时出现）
	int PolyFaceVertexIndex3;      //多面网格顶点索引（可选；非零时出现）
	int PolyFaceVertexIndex4;      //多面网格顶点索引（可选；非零时出现）
} DXFENTVERTEX, *PDXFENTVERTEX;

//  Spline样条曲线
typedef struct tag_DXFENTSPLINE {
	REALPOINT NormalVecetor;    //法向矢量
	int SplineMarks;                       //样条曲线标志,组码：70
	int SplineOrder;                       //样条曲线阶数,组码：71
	int NodeNum;	                       //节点数,组码：72
	int ControlNodeNum;                    //控制点数，组码：73
	int FittingPointNum;                   //拟合点数,组码：74
	double NodeTolerance;                  //节点公差,组码：42
	double  ControlTolerance;              //控制点公差，组码：43
	double FittingTolerance;               //拟合公差，组码：44
	REALPOINT StartTangent;                //起点切向
	REALPOINT EndTangent;                  //端点切向
	double *pNodePoint;                   //节点值（多个条目）,组码：40
	double Weight;                         //权值,组码：41
    SimpleList<REALPOINT> *pControlNode;               //控制点值
	SimpleList<REALPOINT> *pFittingNode;                 //拟合点值
}DXFENTSPLINE,*PDXFENTSPLINE;

//  MLine多线图元
typedef struct tag_MLINEVETEX{
    REALPOINT VetexPoint;                 //顶点坐标
	REALPOINT DirVetex;                   //从该顶点开始的线段的方向矢量（多个条目；每个顶点一个条目）
	REALPOINT ObliqueDirVetex;            //此顶点处的斜接方向矢量                
	double *pElemParem;       //元素参数，组码：41
	//int RegionFillParemNum;               //元素的区域填充参数的数目,组码：75
	//double RegionFillParem;               //区域填充参数，组码：42
	double *pRegionFillParem;
	int RegionFillParemNum; 
}MLINEVETEX,*PMLINEVETEX;

typedef struct tag_DXFENTMLINE{
	double ScaleFacter;                    //比例因子，组码：40
	int Alige;                             //对正：0 = 上；1 = 无；2 = 下,组码：70
	int Simble;                           //标志（按位编码值）：
	                                       //1 = 至少有一个顶点（代码 72 大于 0）
                                           //2 = 关闭
		                                   //4 = 禁止起点封口 
                                           //8 = 禁止端点封口 
	                                       //组码：71
	int VetexNum;                         //顶点数，组码：72
	REALPOINT StartPoint;                 //起点
	SimpleList<MLINEVETEX> *pVetexPoint;                 //顶点
	//REALPOINT DirVetex;                   //从该顶点开始的线段的方向矢量（多个条目；每个顶点一个条目）
	//REALPOINT ObliqueDirVetex;            //此顶点处的斜接方向矢量
	int ParemNum;                         //该元素的参数数目,组码：74
	//double ElemParem;                     //元素参数，组码：41
	//int RegionFillParemNum;                  //元素的区域填充参数的数目,组码：75
	//double RegionFillParem;               //区域填充参数，组码：42
      
}DXFENTMLINE,*PDXFENTMLINE;

//  Point多点图元
typedef struct tag_DXFENTPOINT {
	REALPOINT	Point0;                  //坐标值
} DXFENTPOINT, *PDXFENTPOINT;

//  Soild图元
typedef struct tag_DXFENTSOLID {
	REALPOINT	Point0;               //第一个点的坐标
	REALPOINT	Point1;               //第二个点的坐标            
	REALPOINT	Point2;               //第三个点的坐标
	REALPOINT	Point3;               //第四个点的坐标
} DXFENTSOLID, *PDXFENTSOLID;

//  Text单行文字数据
typedef struct tag_DXFTEXTDATA {
	double		Height;                       //高度
	double		RotationAngle;           //旋转角度
	double		WidthFactor;             // 相对 X 比例因子
	double		Oblique;                //倾斜角
	int		    GenerationFlag;     //文字生成标志
	short		Justification;          //文字水平对正类型
	REALPOINT	Point1;              //第二对齐点
	REALPOINT ExtrudeDirect;          //拉伸方向
	char		TextStyleName[512];       //文字样式名
} _DXFTEXTDATA,*_PDXFTEXTDATA;

//  MText每行文字数据

//  MText文字数据
typedef struct tag_DXFMTEXTDATA {
 	double		Height;                       //高度
 	double		RefRectWidth;            //参照矩形宽度
 	short		Justification;               //文字对正类型
 	short		TextDir;                      //图形方向
 	REALPOINT ExtrudeDirect;         //拉伸方向
 	double		AngleVecX;                //（以弧度为单位的旋转角）转换为相等的方向矢量X
 	double		AngleVecY;                //（以弧度为单位的旋转角）转换为相等的方向矢量Y
 	double		AngleVecZ;                //（以弧度为单位的旋转角）转换为相等的方向矢量Z
	double		RotationAngle;           //以弧度为单位的旋转角度
	short        LineSpaceStyle;          //多行文字的行距样式（可选）: 1= 至少（将替代为较高的字符）2 = 精确（将不替代为较高的字符）
	double		LineSpaceScale;          //要应用的行距的默认百分比（五分之三）。有效值的范围从 0.25 到 4.00
	short        FillBkFlag;                   //0 = 背景填充关闭 1 = 使用背景填充颜色 2 = 使用图形窗口颜色作为背景填充颜色
	int            ColorIndex;                //背景色（颜色索引编号）
	double		FillRectScale;              //填充框大小（可选）：确定文字周围的边框大小。
	int            FillAlpha;                    //背景填充颜色的透明度（未实现）
} _DXFMTEXTDATA,*_PDXFMTEXTDATA;

//  Text单行文字图元
typedef struct tag_DXFENTTEXT {
	REALPOINT		Point0;               //插入点
	_DXFTEXTDATA	TextData;           //文字数据
	WCHAR			strText[1024];    //文字字符串
	REALRECT        	TextRect;           //文字区域
} DXFENTTEXT, *PDXFENTTEXT;

//  MText多行文字图元
typedef struct tag_DXFENTMTEXT {
	REALPOINT		Point0;                  //插入点
	_DXFMTEXTDATA	TextData;          //文字数据
    WCHAR			strText[1024];       //文字内容
	char		    TextStyleName[512];    //文字样式名
} DXFENTMTEXT, *PDXFENTMTEXT;

//  多段线图元
typedef struct tag_DXFENTPOLYCURVE {
	unsigned short	Flag;                 //多段线标志
	bool Closed;              	            //多段线是否闭合
	int nPolyPoints;                        //多段线边界点个数
	PREALPOINT pPolyPoints;        //多段线边界点坐标
} DXFENTPOLYCURVE, *PDXFENTPOLYCURVE;

typedef struct tag_DXFENTPOLYLINEBASE : public tag_DXFENTPOLYCURVE {
	int nVertex;                              //多段线顶点数
	PDXFENTVERTEX pVertex;        //多段线顶点
	bool IsMeshMClosed;       	    //多边形网格以 M 方向闭合
	bool IsMeshNClosed;	            //多边形网格以 N 方向闭合
	double FixedWidth;           	    //组码43 固定宽度(可选;默认值 = 0). 如果设置为可变宽度(代码40 / 41), 则不使用
	unsigned int	MeshM;		        //组码：71 多边形网格 M 顶点计数（可选；默认值 = 0）
	unsigned int	MeshN;	   	  	    //组码：72 多边形网格 N 顶点计数（可选；默认值 = 0）
	int nArrows;						        //多段线线宽不为零的矢量坐标点数  
	PREALPOINT pArrows;			    //多段线线宽不为零的矢量坐标点
} DXFENTPOLYLINEBASE, *PDXFENTPOLYLINEBASE;

//  PolyLine多段线图元
typedef struct tag_DXFENTPOLYLINE : public tag_DXFENTPOLYLINEBASE {
	bool IsPolygonMesh;	        //这是一个多边形网格
	bool IsPolyFaceMesh;	        //多段线是一个多面网格
	REALPOINT VPoint;	            //DXF：始终为 0 APP：“虚拟”点；X 值和 Y 值始终为 0，Z 值是多段线的标高
	double StartWidth;	            //组码：40 默认起点宽度（可选；默认值 = 0）
	double EndWidth;	                //组码：41 默认端点宽度（可选；默认值 = 0）
	unsigned short PolyFlag;	    //组码：75 曲线和平滑曲面类型（可选；默认值 = 0）
	int nVertexIndexes;	            //多面网格顶点索引数
	int *pPolyFaceVertexIndexes;	//多面网格顶点索引值
} DXFENTPOLYLINE, *PDXFENTPOLYLINE;

//  LWPolyLine多段线图元
typedef struct tag_DXFENTLWPOLYLINE : public tag_DXFENTPOLYLINEBASE {
}DXFENTLWPOLYLINE, *PDXFENTLWPOLYLINE;

//  Leader引线图元
typedef struct tag_DXFENTLEADER {
	PDXFENTVERTEX	pVertex;
	int nVertex;
	unsigned short	Flag;
} DXFENTLEADER, *PDXFENTLEADER;

//  Ellipse椭圆图元
typedef struct tag_DXFENTELLIPSE {
  REALPOINT		CenterPoint;
  REALPOINT		MajorAxisEndPoint;
  double		MinorToMajorRatio;
  double		StartParam;
  double		EndParam;
  double		ExtrudeDirectX; //拉伸方向X
  double		ExtrudeDirectY; //拉伸方向Y
  double		ExtrudeDirectZ; //拉伸方向Z
} DXFENTELLIPSE, *PDXFENTELLIPSE;

//  3DFace图元
typedef struct tag_DXFENT3DFACE{
	REALPOINT	 Point0; //第一视角点
	REALPOINT	 Point1; //第二视角点
	REALPOINT   Point2; //第三视角点
	REALPOINT   Point3; //第四视角点
	bool   FirstSideVisible;        //第一条边可见性
	bool   SecondSideVisible;   //第二条边可见性
	bool   ThirdSideVisible;       //第三条边可见性
	bool   FourthSideVisible;    //第四条边可见性
} DXFENT3DFACE, *PDXFENT3DFACE;

//  Trace跟踪图元
typedef struct tag_DXFENTTRACE{
	REALPOINT	 Point0; //第一视角点
	REALPOINT	 Point1; //第二视角点
	REALPOINT   Point2; //第三视角点
	REALPOINT   Point3; //第四视角点
} DXFENTTRACE, *PDXFENTTRACE;

//  Hatch图案填充
typedef  DXFENTCURVE2D  HATCHCURVEDATA, *PHATCHCURVEDATA;                 //曲线边界数据
typedef  DXFENTPOLYLINE2D  HATCHPOLYLINEDATA, *PHATCHPOLYLINEDATA;  //多段线边界数据
typedef  DXFENTLINE2D  HATCHLINEDATA, *PHATCHLINEDATA;                            //直线边界数据
typedef  DXFENTARC2D  HATCHARCDATA, *PHATCHARCDATA;                              //圆弧边界数据
typedef  DXFENTELLIPSE2D  HATCHELLIPSEDATA, *PHATCHELLIPSEDATA;             //椭圆弧边界数据
typedef  DXFENTSPLINE2D  HATCHSPLINEDATA, *PHATCHSPLINEDATA;               //样条曲线边界数据

struct HatchRect {
	double left;
	double top;
	double z1;
	double right;
	double bottom;
	double z2;
	double width;
	double height;
	REALPOINT Center;
	REALPOINT TopLeft;
	REALPOINT BottomRight;
};  //图案填充区域

enum HatchBoundaryType {	
	HatchPathType_PolyLine = 0,
	HatchPathType_Line,
	HatchPathType_Circle,
	HatchPathType_Ellipse,
	HatchPathType_Spline
};  //边界类型

enum HatchPatternStyle {
	HatchPatternStyle_Solid = 0,
	HatchPatternStyle_PatternData,
	HatchPatternStyle_Horizontal,
	HatchPatternStyle_Vertical,
	HatchPatternStyle_FDiagonal,
	HatchPatternStyle_BDiagonal,
	HatchPatternStyle_Cross,
	HatchPatternStyle_DiagCross
};  //图案样式

typedef struct tag_LineWithBaseP {
	REALPOINT P1;
	REALPOINT P2;
	REALPOINT BP;
} LineWithBaseP, *PLineWithBaseP;  //基点坐标

typedef struct tag_HATCHPATTERNDATA {
	double lineAngle;      //图案直线角度
	REALPOINT baseP;   //图案直线基点
	REALPOINT offset;   //图案直线偏移
	int dashNum;           //虚线长度项目数 
	double *lines;           //虚线长度（多个条目)
	BOOL  isDash;          //是否为虚线
} HATCHPATTERNDATA, *PHATCHPATTERNDATA;  //图案数据

typedef struct tag_HATCHSOLIDDATA {
	unsigned short SolidPatternType;    //组码：450 表示实体图案填充或渐变色；0 = 实体图案填充 1 = 渐变色
	unsigned short ColorRefType;         //组码：452 记录定义颜色的方法；0 = 双色渐变色 1 = 单色渐变色
	int ColorNum;             //组码：453 表示颜色数
	double ColorAngle;     //组码：460 以弧度表示的渐变色的旋转角度（默认值 = 0, 0）
	BOOL GradientRef;     //组码：461 判定渐变色定义 对应居中和非居中
	double Hue;               //组码：462 色调值
	int GradientColor[2];  //组码：421 颜色值
	char GradientName[512];  //组码：470 渐变色名称
} HATCHSOLIDDATA, *PHATCHSOLIDDATA;  //实体填充或者渐变色填充数据

typedef struct tag_HATCHBOUNDARYDATA { 
	int BoundaryMark;   //边界路径类型标志（按位编码），组码：92
	int BoundaryNum;  //边界路径中的边数（仅当边界不是多段线时），组码：93
	int BoundaryObjectNum;  //源边界对象数，组码：97
	SimpleList <PHATCHCURVEDATA> *pHatchCurveList;  //曲线边界数据
}HATCHBOUNDARYDATA,*PHATCHBOUNDARYDATA;  //边界路径数据

typedef struct tag_BOUNDARIES {
	int BoundaryPointNum;             //边界点个数
	PPOINT2D pBoundaryPoint;      //边界点坐标
} BOUNDARIES, *PBOUNDARIES;  //边界坐标数据

typedef struct tag_DXFENTHATCH {
	char  HatchName[512];               //图案填充名称，组码：2
	int HatchType;                            //实体填充标志（实体填充 = 1；图案填充 = 0），组码：70 
	unsigned short HatchRefFlag;     //关联性标志（关联 = 1；无关联 = 0），组码：71
	int BoundaryPathNum;               //边界路径（环）数 ，组码：91
	PHATCHBOUNDARYDATA pHatchBoundaryData;  //填充的边界路径数据
	int HatchStyle;				              //图案填充样式，组码：75
	HatchPatternStyle PatternStyle; //填充图案样式
	unsigned short PatternType;      //填充图案类型：0 = 用户定义；1 = 预定义；2 = 自定义，组码：76
	double PatternFillAngle;             //填充图案角度（仅限图案填充）,组码：52
	double PatternFillScale;             //填充图案比例或间距（仅限图案填充）,组码：41
	int BoundaryAnnotation;           //边界注释标志,组码：73
	unsigned short BidirectionalMark;  //填充图案双向标志（仅限图案填充）：0 = 非双向；1 = 双向 组码：77
	int LineNum;                             //图案定义直线数（仅限图案填充），组码：78
	PHATCHPATTERNDATA pHatchPatternData;  //图案直线数据（仅限图案填充）。组码 78 指定的重复次数。
	PBOUNDARIES pBoundaries;   //图案填充边界坐标数据(用来计算边界)
	PHATCHSOLIDDATA pHatchSolidData;  //填充颜色数据（仅限实体填充和渐变色填充）
	int ParseLinesNum;                  //图案填充解析坐标数目
	PREALPOINT pParseLines;       //图案填充解析坐标数据(用来画图案填充)
	HatchRect DRect;                    //图案填充矩形区域
} DXFENTHATCH, *PDXFENTHATCH;

//  Helix螺旋图元
typedef struct tag_DXFENTHELIX : public tag_DXFENTSPLINE {
	REALPOINT AxisBasePoint;  //轴基点
	REALPOINT Start;  //起点
	double AxisVec[3];  //轴矢量
	double Radius;  //半径
	double Turns;  //圈数
	double TurnHeight;  //圈高
	bool Flag;  //顺逆时针标志
	int ConstraintType;  //约束类型
	int CoordCount;  //坐标点个数
	PREALPOINT pCoordPoint;  //坐标点
} DXFENTHELIX, *PDXFENTHELIX;

//  ACAD_TABLE表格图元
typedef struct tag_DXFENTACADTABLE {
	char	BlockHeaderName[512];       //块名称
	REALPOINT Point;  		                //插入点
} DXFENTACADTABLE, *PDXFENTACADTABLE;

//  WipeOut区域覆盖图元
typedef struct tag_DXFENTWIPEOUT : public tag_DXFENTPOLYCURVE {
	REALPOINT Point;        //插入点
	REALPOINT UVector;   //单像素的 U 矢量（沿图像可见底部的点，从插入点开始）
	REALPOINT VVector;   //单像素的 V 矢量（沿图像可见左侧的点，从插入点开始）
	double UValue;            //DXF：图像大小（像素）的 U 值
	double VValue;            //DXF：图像大小（像素）的 V 值
	BOOL IsUseClipping;   //剪裁状态：0 = 关闭；1 = 打开
	short Bright;                //亮度值（0-100；默认值 = 50）
	short Contrast;            //对比度值（0-100；默认值 = 50）
	short FadeUnder;        //淡入度值（0-100；默认值 = 0）
	unsigned short ClippingBoundaryType;    //剪裁边界类型。1 = 矩形；2 = 多边形
	int nClip;                     //剪裁边界顶点数
	REALPOINT *pClipPoints;  //DXF：剪裁边界顶点值（在 OCS 中）（多个条目）
} DXFENTWIPEOUT, *PDXFENTWIPEOUT;

//  Arrow Heads --------
struct ARROWHEAD
{
	LPCTSTR	ArrowHeadName;
	int		BLKNum;
	LPCTSTR	Command;
	double	Param1;
	double	Param2;
	double	Param3;
	double	Param4;
	double	Param5;
	double	Param6;
	double	Param7;
	double	Param8;
};

//  Arrow Heads Data
static ARROWHEAD ArrowHeads[] =
{
	{"ClosedFilled", 1, "SOLID", 0, 0, -1, -0.1667, -1, 0.1667, -1, 0.1667},
	{"ClosedFilled", 2, "SOLID", 0, 0, -1, -0.1667, -1, 0.1667, -1, 0.1667},
	{"Oblique", 1, "LINE", -0.5, -0.5, 0.5, 0.5, 0, 0, 0, 0},
	{"Oblique", 2, "LINE", -0.5, -0.5, 0.5, 0.5, 0, 0, 0, 0},
};

static int ArrowHeadsNum = sizeof(ArrowHeads) / sizeof(ARROWHEAD);

#endif // DXFDEF_H
