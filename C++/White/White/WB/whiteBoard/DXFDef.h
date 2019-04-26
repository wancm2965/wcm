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

#include "SimpleList.h"
// Handle to DXF
DECLARE_HANDLE(HDXF);
//#include "DrawingDef.h"
// Section Definitions
#define	SEC_NOTSET		0x0000
#define	SEC_HEADER		0x0001
#define	SEC_CLASSES		0x0002
#define	SEC_TABLES		0x0004
#define	SEC_BLOCKS		0x0008
#define	SEC_ENTITIES	0x0010
#define	SEC_OBJECTS		0x0020
#define	SEC_UNKNOWN		0x0040
typedef DWORD OBJHANDLE;
// Table Type Definitions
#define	TAB_NOTSET		0x0000
#define	TAB_APPID		0x0001
#define	TAB_BLOCKRECORD	0x0002
#define	TAB_DIMSTYLE	0x0004
#define	TAB_LAYER		0x0008
#define	TAB_LTYPE		0x0010
#define	TAB_STYLE		0x0020
#define	TAB_UCS			0x0040
#define	TAB_VIEW		0x0080
#define	TAB_VPORT		0x0100
#define	TAB_UNKNOWN		0x0200

// Text Justification Types
#define	TJ_LEFT			0x0000
#define	TJ_CENTER		0x0001
#define	TJ_RIGHT		0x0002
#define	TJ_ALIGNED		0x0003
#define	TJ_MIDDLE		0x0004
#define	TJ_FIT			0x0005
#define	TJ_BOTTOMLEFT	0x0100
#define	TJ_BOTTOMCENTER	0x0101
#define	TJ_BOTTOMRIGHT	0x0102
#define	TJ_MIDDLELEFT	0x0200
#define	TJ_MIDDLECENTER	0x0201
#define	TJ_MIDDLERIGHT	0x0202
#define	TJ_TOPLEFT		0x0300
#define	TJ_TOPCENTER	0x0301
#define	TJ_TOPRIGHT		0x0302


// Structures
//  Base ----------
typedef struct tag_REALPOINT
{
	double	x;
	double	y;
	double	z;
} REALPOINT, *PREALPOINT;

enum HatchPathType
{	
	HatchPathType_Pie,
	HatchPathType_Chord,
	HatchPathType_Circle,
	HatchPathType_PolyGon,
	HatchPathType_Ellipse,
	HatchPathType_EllipsePie,
	HatchPathType_EllipseChord,
	HatchPathType_NULL
};

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
	char	Name[512];					// 图层名
	int		StandardFlags;				// 标准标记值
	int		Color;						// 图层颜色
	char	LineType[512];				//   线型
} DXFLAYER, *PDXFLAYER;

typedef struct tag_DXFLTYPE
{
	char	Name[512];					//  线型名
	int		StandardFlags;				// 标准标记值
	char	DescriptiveText[512];		//  线型的说明文字
	int		ElementsNumber;				// 线型元素的数目
	double	TotalPatternLength;			// 图案总长度
	double	Elements[30];				// 线元素
} DXFLTYPE, *PDXFLTYPE;

typedef struct tag_DXFSTYLE
{
	char	Name[512];					// 样式名
	char	PrimaryFontFilename[512];	// 主要字体文件名
	int		StandardFlags;				// 标准标记值
	int		TextGenerationFlags;		// 文字生成标志 -> 2=Text is backward  4=Text is upside down
	double	FixedTextHeight;			// 固定的文字高度
	double	WidthFactor;				// 宽度因子
	double	Height;						// 上次使用的高度
	double	ObliqueAngle;				// 倾斜角度
} DXFSTYLE, *PDXFSTYLE;

typedef struct tag_DXFDIMSTYLE
{
	char	Name[512];					// 标注样式名
	int		StandardFlags;				// 标准标记值
	int		DIMCLRD;					// Dimension line & Arrow heads color
	double	DIMDLE;						// Dimension line size after Extensionline
	int		DIMCLRE;					// Extension line color
	double	DIMEXE;						// Extension line size after Dimline
	double	DIMEXO;						// Offset from origin
	char	DIMBLK1[512];				// 第一个箭头
	char	DIMBLK2[512];				// 第二个箭头
	double	DIMASZ;						// 箭头大小
	char	DIMTXSTY[512];				// 文字样式
	DWORD	DIMTXSTYObjhandle; // 文字样式对象句柄
	int		DIMCLRT;					// 文字颜色
	double	DIMTXT;						// 文字高度
	int		DIMTAD;						// Vertical Text Placement
	double	DIMGAP;						// 
} DXFDIMSTYLE, *PDXFDIMSTYLE;

//  BLOCKS --------
typedef struct tag_DXFBLOCKHEADER
{
	char		Name[512];				// 块名
	char		Flags;					// 块类型标记
	REALPOINT	BasePoint;				// 基准点
	char		LayerName[512];			// 图层名
} DXFBLOCKHEADER, *PDXFBLOCKHEADER;

//  ENTITIES ------
typedef struct tag_DXFENTITYHEADER
{
	unsigned short	EntityType;			// 图元类型
	char		LayerName[512];			// 图层名
	char		LTypeName[512];			// 图层类型名
	short		Color;					// 颜色
	double		Thickness;				// 厚度
	double		LineTypeScale;			// 线型比例
	double		ExtrusionDirection[3];	// 压缩方向
	short		LineWeight;				// 线重
} DXFENTITYHEADER, *PDXFENTITYHEADER;

//直线
typedef struct tag_DXFENTLINE {
	REALPOINT	Point0;                        //起始点
	REALPOINT	Point1;                        //终点
} DXFENTLINE, *PDXFENTLINE;

//圆弧
typedef struct tag_DXFENTARC {
	REALPOINT	Point0;                         //圆心
	double		Radius;                        //半径
	double		StartAngle;                    //起始角度
	double		EndAngle;                      //结束角度
} DXFENTARC, *PDXFENTARC;

//圆
typedef struct tag_DXFENTCIRCLE {
	REALPOINT	Point0;                         //圆心
	double		Radius;                           //半径
} DXFENTCIRCLE, *PDXFENTCIRCLE;

//标注
typedef struct tag_DXFENTDIMENSION {
	REALPOINT	DefPoint3;
	REALPOINT	DefPoint4;
	double		DimRotationAngle;                 //标注旋转角度
	REALPOINT	DimLineDefPoint;                  
	char		BlockName[16];                    //块名
	char		DimStyleName[512];                //标注样式名
	WCHAR		DimText[1024];                   //标注文字
} DXFENTDIMENSION, *PDXFENTDIMENSION;

//块参照
typedef struct tag_DXFENTINSERT {
	REALPOINT	Point0;                        //插入点
	double		XScale;                         //拉伸方向
	double		YScale;
	double		ZScale;
	double		RotationAngle;                  //旋转角度
	char		BlockHeaderName[512];              //块名称
	double    StretchDirectZ;                //拉伸方向Z
} DXFENTINSERT, *PDXFENTINSERT;

//顶点图元
typedef struct tag_DXFENTVERTEX {
	REALPOINT			Point;          //点坐标
	unsigned short	Flag;              //标志
	double			StartWidth;        //起始宽度
	double			EndWidth;          //终点宽度
	double			Bulge;             //突起
	double			TangentDir;        //
} DXFENTVERTEX, *PDXFENTVERTEX;

//nurbs曲线
typedef struct tag_DXFENTSPLINE{
	REALPOINT NormalVecetor;               //法向矢量
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
    SimpleList<REALPOINT> *pControlNode;                 //控制点值，
	SimpleList<REALPOINT> *pFittingNode;                 //拟合点值

}DXFENTSPLINE,*PDXFENTSPLINE;

//多线图元
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

//多点图元
typedef struct tag_DXFENTPOINT {
	REALPOINT	Point0;                  //坐标值
} DXFENTPOINT, *PDXFENTPOINT;

//soild图元
typedef struct tag_DXFENTSOLID {
	REALPOINT	Point0;               //第一个点的坐标
	REALPOINT	Point1;               //第二个点的坐标            
	REALPOINT	Point2;              //第三个点的坐标
	REALPOINT	Point3;              //第四个点的坐标
} DXFENTSOLID, *PDXFENTSOLID;

//单行文字数据
typedef struct tag_DXFTEXTDATA {
	double		Height;                  //高度
	double		RotationAngle;           //旋转角度
	double		WidthFactor;             // 相对 X 比例因子
	double		Oblique;                //倾斜角
	char		GenerationFlag;           //文字生成标志
	short		Justification;          //文字水平对正类型
	REALPOINT	SecondAlignmentPoint;   //第二对齐点
	char		TextStyleName[512];       //文字样式名
	double		x;
	double		y;
} _DXFTEXTDATA,*_PDXFTEXTDATA;

//多行文字数据
typedef struct tag_DXFMTEXTDATA {
	double		Height;             // //高度
	double		RotationAngle;          //旋转角度
	double		WidthFactor;            // 相对 X 比例因子
	double		Oblique;               //倾斜角
	char		GenerationFlag;           //文字生成标志
	short		Justification;          //文字水平对正类型
	double		x_vec;
	double		y_vec;
	double		z_vec;
	char		TextStyleName[512];
	double		x;
	double		y;
} _DXFMTEXTDATA,*_PDXFMTEXTDATA;

//单行文字图元
typedef struct tag_DXFENTTEXT {
	REALPOINT		Point0;             //插入位置
	_DXFTEXTDATA	TextData;           //文字数据
	WCHAR			strText[1024];      
} DXFENTTEXT, *PDXFENTTEXT;

//多行文字图元
typedef struct tag_DXFENTMTEXT {
	REALPOINT		Point0;      
	_DXFMTEXTDATA	TextData;
    WCHAR			strText[1024];
} DXFENTMTEXT, *PDXFENTMTEXT;

// this code is implemented by tran duy dung 20.10.2003
typedef struct tag_DXFENTPOLYLINE {
	PDXFENTVERTEX	pVertex;
	int				nVertex;
	unsigned short	Flag;
} DXFENTPOLYLINE, *PDXFENTPOLYLINE;

//多段线图元
typedef struct tag_DXFENTLWPOLYLINE{
	PDXFENTVERTEX	pVertex;
	int				nVertex;
	unsigned short	Flag;
}DXFENTLWPOLYLINE, *PDXFENTLWPOLYLINE;

//
typedef struct tag_DXFENTLEADER {
	PDXFENTVERTEX	pVertex;
	int				nVertex;
	unsigned short	Flag;
} DXFENTLEADER, *PDXFENTLEADER;

//椭圆图元
typedef struct tag_DXFENTELLIPSE {
  REALPOINT		CenterPoint;
  REALPOINT		MajorAxisEndPoint;
  double		MinorToMajorRatio;
  double		StartParam;
  double		EndParam;
  double       StretchDirectX; //拉伸方向X
  double       StretchDirectY; //拉伸方向Y
  double       StretchDirectZ; //拉伸方向Z
} DXFENTELLIPSE, *PDXFENTELLIPSE;

//图案填充
typedef struct tag_HATCHPOYLINEDATA{
	int   CovexMark;						//“有凸度”标志 ，组码：72
	int   CloseMark;						//“关闭”标志 ，组码：73
	int   VertexNum;                        //多段线顶点数,组码: 93
	SimpleList<REALPOINT> *pVertex;                    //顶点位置
	int   Covex;                            //凸度（可选；默认值 = 0），组码：42
}HATCHPOYLINEDATA,*PHATCHPOYLINEDATA;

typedef struct tag_HATCHARCDATA{

	REALPOINT CenterPoint;                  //中心点位置
	float Radius;                           //半径,组码：40
	float StartAngle;                       //起点角度，组码：50
	float EndAngle;                         //终点角度，组码：51
	int   AcountClockWise;                  //逆时针标志，组码：73
}HATCHARCDATA,*PHATCHARCDATA;

typedef struct tag_HATCHELLIPSEDATA  
{
	REALPOINT		CenterPoint;             //中心点
	REALPOINT		MajorAxisEndPoint;       //相对于中心点的长轴端点
	double		MinorToMajorRatio;           //短轴的长度（占长轴长度的比例）,组码40
	double		StartParam;                  //起点角度,组码：50
	double		EndParam;                    //终点角度,组码：51
}HATCHELLIPSEDATA,*PHATCHELLIPSEDATA;

typedef struct tag_HATCHSPLINEDATA
{
	int Order;                               //阶数，组码：94
	int Cycle;                               //周期，组码：74
	int Rational;                            //有理,组码：73
	int NodeNum;	                         //节点数,组码：95
	int ControlNum;                          //控制点数，组码：96
	REALPOINT NodePoint;                     //节点值（多个条目）,组码：40
	REALPOINT ControlPoint;                  //控制点
	double Weight;                           //权值,组码：42
	int  FittingDataNum;                     //拟合数据数目,组码：97
	REALPOINT FitDatum;                      //	拟合基准
	REALPOINT StartTangent;                  //起点切向
	REALPOINT EndTangent;                   //端点切向

}HATCHSPLINEDATA,*PHATCHSPLINEDATA;

typedef struct tag_HATCHPATHLINEDATA
{
	/*double LineAngle;                       //图案直线角度,组码：53
	REALPOINT LineBase;                     //图案直线基点
	REALPOINT LineMigration;                //图案直线偏移
	int DotLineNum;                         //虚线长度项目数，组码： 79
	double DotLineLen; */                     //虚线长度 ，组码：49
	REALPOINT	pStartVertex;
	REALPOINT	pEndVertex;

}HATCHPATHLINEDATA,*PHATCHPATHLINEDATA;

typedef struct tag_HATCHLINEDATA
{
	double LineAngle;                       //图案直线角度,组码：53
	REALPOINT LineBase;                     //图案直线基点
	REALPOINT LineMigration;                //图案直线偏移
	int DotLineNum;                         //虚线长度项目数，组码： 79
	double DotLineLen;                     //虚线长度 ，组码：49
	REALPOINT	pStartVertex;
	REALPOINT	pEndVertex;

}HATCHLINEDATA,*PHATCHLINEDATA;

typedef struct tag_HATCHPATHDATA{
	//int PathRefMark;                         //边界路径类型标志,0 = 默认；1 = 外部；2 = 多段线
	//4 = 导出；8 = 文本框；16 = 最外层,组码 ：92
	//int Paths;                             //该边界路径中的边数（仅当边界不是多段线时）,组码：93
	int PathType;                          //边类型（仅当边界不是多段线时）：
	//1 = 直线；2 = 圆弧；3 = 椭圆弧；4 = 样条曲线,组码：72
	int OrgPathNum;                       //源边界对象数,组码97
	//int nLineNum;
	SimpleList<HATCHPATHLINEDATA> *pHatchPathLineData;
	PHATCHELLIPSEDATA pHatchEllipseData;
	PHATCHARCDATA pHatchArcData;
	PHATCHSPLINEDATA pHatchSplineData;
	PHATCHPOYLINEDATA pHatchPoyLineData;

}HATCHPATHDATA,*PHATCHPATHDATA;//边界路径数据

typedef struct tag_DXFENTHATCH{

	char  HatchName[512];					//填充图案名，组码：2
	int   HatchType;						//实体填充标志（实体填充 = 1；图案填充 = 0），组码：70
	int   RefFlag;							//关联性标志（关联 = 1；无关联 = 0），组码：71
	int   Pathes;							//边界路径（环）数 ，组码：91
	int   HatchStyle;						//填充样式，组码：75
	int   PathMark; 						//边界标志，组码：92（？）
	int   nPathSideNum;						//表示该边界路径中的边数，组码：93
	HatchPathType hatchpathtype;			//填充图案路径类型
	PHATCHPATHDATA HatchPathData;					//填充的边界路径数据
	int  PatternType;                       //填充图案类型,组码：76
	double  PatternFillAngle;               //填充图案角度（仅限图案填充）,组码：52
	double FillScale;                       //填充图案比例或间距（仅限图案填充）,组码：41
	int    BoundaryAnnotation;              //边界注释标志,组码：73
	int BidirectionalMark;                  //填充图案双向标志（仅限图案填充）：
	//0 = 非双向；1 = 双向 组码：77
	int LineNum;                            //图案定义直线数,组码：78
	//PHATCHLINEDATA *hatchlinedata;          //图案直线数据
	SimpleList<HATCHLINEDATA> *pHatchLineData;//图案直线数据

	//		bool m_bHasCovex;			             //是否有圆弧度段，组码：
	// 		double CornerPoints[8];					//如果是矩形，表示四个角点的坐标,依次为左上点，右上点，右下点，左下点
	// 		double m_RoundRatioX;					//圆角矩形的圆角半径与矩形长度比
	// 		double m_RoundRatioY;			//圆角矩形的圆角半径与矩形高度比

}DXFENTHATCH,*PDXFENTHATCH;

// Arrow Heads --------
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

// Arrow Heads Data
static ARROWHEAD ArrowHeads[] =
{
	{L"ClosedFilled", 1, L"SOLID", 0, 0, -1, -0.1667, -1, 0.1667, -1, 0.1667},
	{L"ClosedFilled", 2, L"SOLID", 0, 0, -1, -0.1667, -1, 0.1667, -1, 0.1667},
	{L"Oblique", 1, L"LINE", -0.5, -0.5, 0.5, 0.5, 0, 0, 0, 0},
	{L"Oblique", 2, L"LINE", -0.5, -0.5, 0.5, 0.5, 0, 0, 0, 0},
};

static int ArrowHeadsNum = sizeof(ArrowHeads) / sizeof(ARROWHEAD);


#endif // DXFDEF_H
