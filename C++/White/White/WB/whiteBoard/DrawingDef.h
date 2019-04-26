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
|  DrawingDef.h                                                       |
\*-------------------------------------------------------------------*/

#ifndef DRAWINGDEF_H
#define DRAWINGDEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SimpleList.h"
// Handle to DRAWING
DECLARE_HANDLE(HDRAWING);

// Window Messages
#define	WM_PROGRESS		WM_USER+1

// Definitions
typedef DWORD OBJHANDLE;
#define	MAX_STRLEN 512
//#define PI	3.1415926535

// Entities and Objects Definitions
#define	ENT_LINE		1
#define	ENT_POINT		2
#define	ENT_CIRCLE		3
#define	ENT_SHAPE		4
#define	ENT_ELLIPSE		5
#define ENT_SPLINE		6
#define ENT_TEXT		7
#define ENT_ARC			8
#define ENT_TRACE		9
#define ENT_REGION		10
#define ENT_SOLID		11
#define ENT_BLOCK		12
#define ENT_ENDBLK		13
#define ENT_INSERT		14
#define ENT_ATTDEF		15
#define ENT_ATTRIB		16
#define ENT_SEQEND		17
#define ENT_POLYLINE	19
#define ENT_VERTEX		20
#define ENT_LINE3D		21
#define ENT_FACE3D		22
#define ENT_DIMENSION	23
#define ENT_VIEWPORT	24
#define ENT_SOLID3D		25
#define ENT_RAY			26
#define ENT_XLINE		27
#define ENT_MTEXT		28
#define ENT_LEADER		29
#define ENT_TOLERANCE	30
#define ENT_MLINE		31
#define OBJ_DICTIONARY	32
#define OBJ_MLINESTYLE	33
#define OBJ_CLASS		34
#define ENT_BODY		35
#define OBJ_GROUP		36
#define OBJ_PROXY		100
#define OBJ_XRECORD		101
#define OBJ_IDBUFFER	102
#define ENT_LWPOLYLINE  103
#define ENT_HATCH       104
#define ENT_IMAGE       105

// Flag Definitions
#define	POLYLINE_CLOSED			1
#define	POLYLINE_CURVEFIT		2
#define	LWPOLYLINE_CLOSED		3	
#define	POLYLINE_SPLINEFIT		4
#define	POLYLINE_3DPLINE		8
#define	POLYLINE_3DMESH			16
#define	POLYLINE_CLOSEDN		32
#define	POLYLINE_POLYFACEMESH	64
#define	POLYLINE_CONTINUELT		128

// FindType Definitions
#define FIND_FIRST				0x0001
#define FIND_LAST				0x0002
#define FIND_NEXT				0x0004
#define FIND_PREVIOUS			0x0008
#define FIND_BYTYPE				0x0010
#define FIND_BYLAYEROBJHANDLE	0x0020
#define FIND_BYLTYPEOBJHANDLE	0x0040
#define FIND_BYCOLOR			0x0080
#define FIND_BYTHICKNESS		0x0100
#define FIND_BYNAME				0x0200
#define FIND_BYHANDLE			0x0400

// Color Definitions
#define	COLOR_BYLAYER	256
#define	COLOR_BYBLOCK	0
#define	COLOR_RED		1
#define	COLOR_YELLOW	2
#define	COLOR_GREEN		3
#define	COLOR_CYAN		4
#define	COLOR_BLUE		5
#define	COLOR_MAGENTA	6
#define	COLOR_WHITE		7

// Header Structures *******************************************

typedef struct tag_HEADER{
	char  CadVer[MAX_STRLEN]; //cad 版本名
}HEADER, *PHEADER;

// Tables Structures *******************************************

typedef struct tag_DIMSTYLE {       //标注样式表
	OBJHANDLE Objhandle;			// 对象句柄
	char	Name[MAX_STRLEN];		// 标注样式名
	char	StandardFlags;			// 标准标记值 组码：70
	                                //16 = 如果设置了此位，则表条目外部依赖于外部参照
		                           //32 = 如果同时设置了此位和位 16，则表明已成功融入了外部依赖的外部参照
		                           //64 = 如果设置了此位，则表明在上次编辑图形时，
	                              // 图形中至少有一个图元参照了表条目。
	                              //（此标志对 AutoCAD 命令很有用。大多数读取 DXF 文件的程序都可以忽略它，
	                              //并且无需由写入 DXF 文件的程序对其进行设置）
	double	dimasz;					// 箭头大小
//	OBJHANDLE dimblk1Objhandle;		// 1st Arrow head
//	OBJHANDLE dimblk2Objhandle;		// 2nd Arrow head
	char	dimblk1[16];			// 一号箭头 组码：6
	char	dimblk2[16];			// 二号箭头  组码：7
	short	dimclrd;				// 标注线和箭头的颜色 组码：176
	short	dimclre;				//扩展线颜色  组码：177
	short	dimclrt;				// 文本颜色 组码：178
	double	dimdle;					// 扩展线之后的标注线的大小 组码：46
	double	dimexe;					// 标注线之后的扩展线的大小 组码：44
	double	dimexo;					// 从原点的开始位移  组码：42
	double	dimgap;					// 从标注线开始的位移  组码：147
	double	dimtxt;					// 文本高度 组码：140
	char	dimtad;					// 垂直文本布局 组码：77
	OBJHANDLE dimtxstyObjhandle;	// 文本样式句柄
} DIMSTYLE, *PDIMSTYLE;

typedef struct tag_LAYER {            //图层表
	OBJHANDLE Objhandle;			// 对象句柄
	char	Name[MAX_STRLEN];		// 图层名
	char	StandardFlags;	               //标准标记  组码：70
	                                  //标准标记（按位编码值）：
	                                 //1 = 冻结图层，否则解冻图层 2 = 默认情况下在新视口中冻结图层
		                             //4 = 锁定图层 16 = 如果设置了此位，则表条目外部依赖于外部参照
		                             //32 = 如果同时设置了此位和位 16，则表明已成功融入了外部依赖的外部参照
		                             //64 = 如果设置了此位，则表明在上次编辑图形时，图形中至少有一个图元参照了表条目。（此标志对 AutoCAD 命令很有用。大多数读取 DXF 文件的程序都可以忽略它，并且无需由写入 DXF 文件的程序对其进行设置）
	short	Color; 	 // 颜色编号 组码：62
	OBJHANDLE LineTypeObjhandle;	// 图层的线型句柄
	short	LineWeight;				// 图层的线高                  
	BOOL	PlotFlag;				// 是否绘制标记 组码：290  
	OBJHANDLE PlotStyleObjhandle;	// PlotStyleName 对象句柄
} LAYER, *PLAYER;

typedef struct tag_LTYPE {            //线型表
	OBJHANDLE Objhandle;			// 对象句柄
	char	Name[MAX_STRLEN];		// 线型名 组码：2
	char	StandardFlags;			// 标准标记  组码：70
	char	DescriptiveText[512];	// 线型的说明文字  组码：3
	short	ElementsNumber;			// 线元素的数目   组码：73
	double	Elements[30];			// 线元素的数组
	double	PatternLength;          // 图案总长度  组码：40
} LTYPE, *PLTYPE;

typedef struct tag_STYLE {             //文字样式表
	OBJHANDLE Objhandle;			// 对象句柄
	char	Name[MAX_STRLEN];		// 样式名   组码：2
	char	StandardFlags;			// 标准标记  组码：70
	char	TextGenerationFlags;	//文字生成标记 组码：71
	char	PrimaryFontFilename[256];// 主要字体文件名   组码：3
	char	BigFontFilename[256];	// 大字体文件名 组码：4
	double	ObliqueAngle;			// 倾斜角度 组码：50
	double	FixedTextHeight;		// 固定文本高度 组码：40
	double	WidthFactor;			// 宽度因子  组码：41
	double	LastHeightUsed;			// 上次使用的高度  组码：42
} STYLE, *PSTYLE;

// Entity Structures *******************************************

typedef struct tag_ENTITYHEADER //图元段头部
{
	unsigned short	EntityType;			// 图元类型
	OBJHANDLE	Objhandle;				// 句柄
	OBJHANDLE	LayerObjhandle;			// 图元所在图层的句柄
	OBJHANDLE	LTypeObjhandle;			// 图元使用的线形的句柄
	BOOL		Deleted;				// 是否已被删除标志
	short		Color;					// 颜色 (0=BYBLOCK, 256=BYLAYER, negative=layer is turned off)
	double		Thickness;				// 厚度 (default=0)
	double		LineTypeScale;			// 线性比例 (default=1.0)
	double		ExtrusionDirection[3];	// 拉伸方向. (default = 0, 0, 1)
	short		LineWeight;				//
	DWORD		PreviousEntityPos;   //前一个图元位置
	DWORD		NextEntityPos;       //下一个图元位置
} ENTITYHEADER, *PENTITYHEADER;

typedef struct tag_ENTITIES
{
	HGLOBAL		hEntities;   //  句柄
	DWORD		TotalSize;      //总的大小
	DWORD		FreePos;    //空闲位置
	DWORD		LastEntityPos;   //前一个图元位置
	DWORD		CurrentEntityPos; //当前图元位置
	DWORD		EntitiesNumber;  //图元数目
} ENTITIES, *PENTITIES;

typedef struct tag_ENTLINE {       //直线对象，起点，终点
	REALPOINT	Point0;
	REALPOINT	Point1;
} ENTLINE, *PENTLINE;

typedef struct tag_ENTARC {        //圆弧对象
	REALPOINT	Point0;              //圆心
	double		Radius;              //半径  组码：100
	double		StartAngle;              //起始角度  组码：50
	double		EndAngle;              //终点角度  组码：51
} ENTARC, *PENTARC;

typedef struct tag_ENTCIRCLE {
	REALPOINT	Point0;              //圆心
	double		Radius;              //半径  组码：40
} ENTCIRCLE, *PENTCIRCLE;

typedef struct tag_ENTDIMENSION {   //标注图元对象
	REALPOINT	DefPoint3;              //
	REALPOINT	DefPoint4;
	double		DimRotationAngle;              //标注旋转角度
	REALPOINT	DimLineDefPoint;              //标注直线默认点
	OBJHANDLE	BlockheaderObjhandle;              //块头句柄
	OBJHANDLE	DimStyleObjhandle;              //标注样式句柄
	WCHAR		DimText[1024];              //标注文本
} ENTDIMENSION, *PENTDIMENSION;

typedef struct tag_ENTINSERT {      //块参照图元对象
	REALPOINT	Point0;              //插入点
	double		XScale;              //X轴缩放因子  组码：41
	double		YScale;              //Y轴缩放因子  组码：42
	double		ZScale;              //Z轴缩放因子   组码：43
	double		RotationAngle;              //旋转角度  组码：50
	OBJHANDLE	 BlockHeaderObjhandle;    //块头句柄
	double      StretchDirectZ;  //拉伸方向Z     组码：230
} ENTINSERT, *PENTINSERT;

//仍有一些图元对象程序没有进行解析
typedef struct tag_ENTPOINT
{   //点图元对象
	REALPOINT	Point0;
} ENTPOINT, *PENTPOINT;

typedef struct tag_ENTSOLID {      //实体图元
	REALPOINT	Point0;
	REALPOINT	Point1;
	REALPOINT	Point2;
	REALPOINT	Point3;
} ENTSOLID, *PENTSOLID;

typedef struct tag_TEXTDATA {      //单行文本数据
	double		Height;              //文本高度  组码：40
	double		RotationAngle;              //旋转角度  组码：50
	double		WidthFactor;              //宽度因子  组码：41
	double		Oblique;              //倾斜角度  组码：51
	char		GenerationFlag;              //文本生成标志 组码：71
	                                          //文字生成标志（可选；默认值 = 0）：
	                                          //2 = 文字反向（在 X 轴方向镜像）
		                                     //4 = 文字倒置（在 Y 轴方向镜像)

	short		Justification;              //文字垂直对正类型 组码：73  
	                                        //文字垂直对正类型（可选；默认值 = 0）整数代码（不是按位编码）
		                                   //0 = 基线对正；1 = 底端对正；2 = 居中对正；3 = 顶端对正
		                                   //详细信息请参见组 72 和 73 整数代码表

	REALPOINT	SecondAlignmentPoint;         //第二对齐点  组码:11 
	OBJHANDLE	TextStyleObjhandle;              //文本样式句柄
} _TEXTDATA,*_PTEXTDATA;

typedef struct tag_MTEXTDATA {       //多行文本
	double		Height;                         //标称（初始）文字高度 组码：40
	double		RotationAngle;              //以弧度为单位的旋转角度 组码：50
	double		WidthFactor;              //宽度因子
	double		Oblique;              //倾斜角度
	char		GenerationFlag;              //文本生成标志
	short		Justification;              //修正
	double		x_vec;                       //X轴方向矢量  组码：11
	double		y_vec;                       //Y轴方向矢量  组码：21
	double		z_vec;                      //Z轴方向矢量  组码：31
	OBJHANDLE	TextStyleObjhandle;          //文本样式句柄
} _MTEXTDATA,*_PMTEXTDATA;

typedef struct tag_ENTTEXT {
	REALPOINT	Point0;                     //插入点
	_TEXTDATA	TextData;                   //文本数据
	WCHAR		strText[1024];              //数据
} ENTTEXT, *PENTTEXT;

typedef struct tag_ENTMTEXT {
	REALPOINT	Point0; 
	_MTEXTDATA	TextData;
	WCHAR		strText[1024];
} ENTMTEXT, *PENTMTEXT;

typedef struct tag_ENTVERTEX {      //顶点图元
  REALPOINT			Point;
  unsigned short	Flag;         //标志
  double			StartWidth;       //起点宽度
  double			EndWidth;         //终点宽度
  double			Bulge;             //凸起
  double			TangentDir;        //切线方向
} ENTVERTEX, *PENTVERTEX;

//--------------this code is implemented by Tran duy Dung 2003/10/20
typedef struct tag_ENTPOLYLINE {   //多段线
	PENTVERTEX		pVertex;          //顶点
	int				nVertex;
	unsigned short	Flag;
} ENTPOLYLINE, *PENTPOLYLINE;

typedef struct tag_ENTSPLINE 
{
	REALPOINT NormalVecetor;     //法向矢量
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
}ENTSPLINE, *PENTSPLINE;

typedef struct tag_ENTMLINE{
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
}ENTMLINE,*PENTMLINE;

typedef struct tag_ENTLWPOLYLINE {
	PENTVERTEX		pVertex;
	int				nVertex;
	unsigned short	Flag;
} ENTLWPOLYLINE, *PENTLWPOLYLINE;

typedef struct tag_ENTELLIPSE {
  REALPOINT		CenterPoint;   //中心点
  REALPOINT		MajorAxisEndPoint;  //相对于中心的长轴端点
  double		MinorToMajorRatio;  //短轴与长轴的比例 组码：40
  double		StartParam;       //开始参数（对于闭合椭圆，该值为 0.0）组码：41
  double		EndParam;         //结束参数（对于闭合椭圆，该值为 2pi）组码：42
  double      StretchDirectX;  //拉伸方向X
  double      StretchDirectY;  //拉伸方向Y 拉伸方向（可选；默认值 = 0, 0, 1）
  double      StretchDirectZ;  //拉伸方向Z
} ENTELLIPSE, *PENTELLIPSE;

typedef struct tag_ENTLEADER{
	PENTVERTEX		pVertex;
	int				nVertex;
	unsigned short	Flag;
} ENTLEADER, *PENTLEADER;

typedef struct tag_ENTHATCHPOYLINEDATA{
	int   CovexMark;						//“有凸度”标志 ，组码：72
	int   CloseMark;						//“关闭”标志 ，组码：73
	int   VertexNum;                        //多段线顶点数,组码: 93
	SimpleList<REALPOINT> *pVertex;                    //顶点位置
	int   Covex;                            //凸度（可选；默认值 = 0），组码：42
}ENTHATCHPOYLINEDATA,*PENTHATCHPOYLINEDATA;

typedef struct tag_ENTHATCHARCDATA
{
	REALPOINT CenterPoint;                  //中心点位置
	float Radius;                           //半径,组码：40
	float StartAngle;                       //起点角度，组码：50
	float EndAngle;                         //终点角度，组码：51
	int   AcountClockWise;                  //逆时针标志，组码：73
}ENTHATCHARCDATA,*PENTHATCHARCDATA;

typedef struct tag_ENTHATCHELLIPSEDATA  
{
	REALPOINT		CenterPoint;             //中心点
	REALPOINT		MajorAxisEndPoint;       //相对于中心点的长轴端点
	double		MinorToMajorRatio;           //短轴的长度（占长轴长度的比例）,组码40
	double		StartParam;                  //起点角度,组码：50
	double		EndParam;                    //终点角度,组码：51
}ENTHATCHELLIPSEDATA,*PENTHATCHELLIPSEDATA;

typedef struct tag_ENTHATCHSPLINEDATA
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

}ENTHATCHSPLINEDATA,*PENTHATCHSPLINEDATA;

typedef struct tag_ENTHATCHLINEDATA
{
	double LineAngle;                       //图案直线角度,组码：53
	REALPOINT LineBase;                     //图案直线基点
	REALPOINT LineMigration;                //图案直线偏移
	int DotLineNum;                         //虚线长度项目数，组码： 79
	double DotLineLen;                      //虚线长度 ，组码：49
	REALPOINT	pStartVertex;
	REALPOINT	pEndVertex;

}ENTHATCHLINEDATA,*PENTHATCHLINEDATA;

typedef struct tag_ENTHATCHPATHLINEDATA
{
	/*double LineAngle;                       //图案直线角度,组码：53
	REALPOINT LineBase;                     //图案直线基点
	REALPOINT LineMigration;                //图案直线偏移
	int DotLineNum;                         //虚线长度项目数，组码： 79
	double DotLineLen; */                     //虚线长度 ，组码：49
	REALPOINT	pStartVertex;
	REALPOINT	pEndVertex;

}ENTHATCHPATHLINEDATA,*PENTHATCHPATHLINEDATA;

typedef struct tag_ENTHATCHPATHDATA{
	int PathRefMark;                         //边界路径类型标志,0 = 默认；1 = 外部；2 = 多段线
	                                        //4 = 导出；8 = 文本框；16 = 最外层,组码 ：92
	int Paths;                             //该边界路径中的边数（仅当边界不是多段线时）,组码：93
	int PathType;                          //边类型（仅当边界不是多段线时）：
	                                       //1 = 直线；2 = 圆弧；3 = 椭圆弧；4 = 样条曲线,组码：72
	int OrgPathNum;                       //源边界对象数,组码97
	SimpleList<ENTHATCHPATHLINEDATA> *pHatchPathLineData;  //路径为直线
	PENTHATCHELLIPSEDATA pHatchEllipseData;            //路径为椭圆弧
	PENTHATCHARCDATA pHatchArcData;                   //路径为圆弧
	PENTHATCHSPLINEDATA pHatchSplineData;           //路径为养条曲线
	PENTHATCHPOYLINEDATA pHatchPoyLineData;           //路径为多段线
	
}ENTHATCHPATHDATA,*PENTHATCHPATHDATA;//边界路径数据

typedef struct tag_ENTHATCH{
	
	char  HatchName[512];					//填充图案名，组码：2
	int   HatchType;						//实体填充标志（实体填充 = 1；图案填充 = 0），组码：70
	int   RefFlag;							//关联性标志（关联 = 1；无关联 = 0），组码：71
	int   Pathes;							//边界路径（环）数 ，组码：91
	int   HatchStyle;						//填充样式，组码：75
	int   PathMark; 						//边界标志，组码：92（？）
	int   PathSideNum;						//表示该边界路径中的边数，组码：93
	HatchPathType hatchpathtype;			//填充图案路径类型
	PENTHATCHPATHDATA* HatchPathData;					//填充的边界路径数据
	int  PatternType;                       //填充图案类型,组码：76
	double  PatternFillAngle;               //填充图案角度（仅限图案填充）,组码：52
	double FillScale;                       //填充图案比例或间距（仅限图案填充）,组码：41
	int    BoundaryAnnotation;              //边界注释标志,组码：73
	int BidirectionalMark;                  //填充图案双向标志（仅限图案填充）：
                                            //0 = 非双向；1 = 双向 组码：77
	int LineNum;                            //图案定义直线数,组码：78
	//PENTHATCHLINEDATA *hatchlinedata;          //图案直线数据
	SimpleList<ENTHATCHLINEDATA> *pHatchLineData;

	//		bool m_bHasCovex;			             //是否有圆弧度段，组码：
	// 		double CornerPoints[8];					//如果是矩形，表示四个角点的坐标,依次为左上点，右上点，右下点，左下点
	// 		double m_RoundRatioX;					//圆角矩形的圆角半径与矩形长度比
	// 		double m_RoundRatioY;			//圆角矩形的圆角半径与矩形高度比

}ENTHATCH,*PENTHATCH;

typedef struct tag_ENTIMAGE{//图像图元对象
	int ImageWidth;
	int ImageHeigth;
}ENTIMAGE, *PENTIMAGE;

// Blocks Structures *******************************************

typedef struct tag_BLOCKHEADER
{
	OBJHANDLE	Objhandle;				// 句柄
	char		Name[MAX_STRLEN];		// 块名
	char		Flags;					// 块类型标志
	REALPOINT	BasePoint;				// 基点
	OBJHANDLE	LayerObjhandle;			// 图层句柄
	ENTITIES	Entities;                   //图元
} BLOCKHEADER, *PBLOCKHEADER;

// View Structure **********************************************
typedef struct tag_VIEW
{
	BOOL	Viewable;		// 是否可见
	int		WindowLeft;		// Drawing Window Boundary Properties (In Pixels)//像素
	int		WindowTop;		// Drawing Window Boundary Properties (In Pixels)
	int		WindowRight;	// Drawing Window Boundary Properties (In Pixels)
	int		WindowBottom;	// Drawing Window Boundary Properties (In Pixels)
	double	ViewLeft;		// Drawing View Properties (In Units)//单位
	double	ViewBottom;		// Drawing View Properties (In Units)
	double	ViewRight;		// Drawing View Properties (In Units)
	double	ViewTop;		// Drawing View Properties (In Units)
	double	PPU;			// 每个单元的像素数
	double	ZoomLevel;	// 缩放比例
} VIEW, *PVIEW;

#endif // DRAWINGDEF_H
