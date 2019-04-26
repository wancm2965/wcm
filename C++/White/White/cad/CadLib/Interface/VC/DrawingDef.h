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

// Handle to DRAWING
DECLARE_HANDLE(HDRAWING);

// Window Messages
#define	WM_PROGRESS		WM_USER+1

// Definitions
typedef DWORD OBJHANDLE;
#define	MAX_STRLEN 512
#define PI	3.1415926535897931

// Entities and Objects Definitions
#define	ENT_UNKNOWN	      0
#define	ENT_LINE		              1
#define	ENT_POINT		          2
#define	ENT_CIRCLE		          3
#define	ENT_ELLIPSE		          4
#define ENT_SPLINE		          5
#define ENT_TEXT		              6
#define ENT_ARC			          7
#define ENT_SOLID		          8
#define ENT_INSERT		          9
#define ENT_ATTDEF	         10
#define ENT_ATTRIB		         11
#define ENT_POLYLINE         12
#define ENT_DIMENSION	 13
#define ENT_MTEXT		         14
#define ENT_LEADER		     15
#define ENT_MLINE		         16
#define ENT_LWPOLYLINE    17
#define ENT_HATCH	             18
#define ENT_IMAGE              19
#define ENT_3DFACE	         20
#define ENT_TRACE               21
#define ENT_HELIX                22
#define ENT_ACADTABLE	     23
#define ENT_WIPEOUT          24

#define OBJ_DICTIONARY	100
#define OBJ_MLINESTYLE	    101
#define OBJ_CLASS		        102
#define OBJ_GROUP		        103
#define OBJ_PROXY		        104
#define OBJ_XRECORD		    105
#define OBJ_IDBUFFER	        106

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
	double	LastHeightUsed;	// 上次使用的高度  组码：42
	char FaceName[512];	    // 字体名称
} STYLE, *PSTYLE;

// Entity Structures *******************************************

typedef struct tag_ENTITYHEADER //图元段头部
{
	unsigned short	 EntityType;			// 图元类型
	OBJHANDLE	 Objhandle;				// 句柄
	OBJHANDLE	 LayerObjhandle;		// 图元所在图层的句柄
	OBJHANDLE  LTypeObjhandle;		// 图元使用的线形的句柄
	BOOL		Deleted;				// 是否已被删除标志
	short		Color;					// 颜色 (0=BYBLOCK, 256=BYLAYER, negative=layer is turned off)
	double		Thickness;				// 厚度 (default=0)
	double		LineTypeScale;			// 线型比例 (default=1.0)
	double		ExtrusionDirection[3];	// 拉伸方向. (default = 0, 0, 1)
	short		LineWeight;				//线重
	DWORD	PreviousEntityPos;   //前一个图元位置
	DWORD	NextEntityPos;       //下一个图元位置
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

typedef struct tag_ENTCURVE2D {  //曲线对象
	unsigned short EdgeType;    //0 = 多段线；1 = 直线；2 = 圆弧；3 = 椭圆弧；4 = 样条曲线
	POINT2D StartPoint;	       //起点坐标
	POINT2D EndPoint;             //终点坐标
    PPOINT2D pCoordPoint;     //坐标点
	int CoordCount;                  //坐标点个数
} ENTCURVE2D, *PENTCURVE2D;

typedef struct tag_ENTLINE2D : tag_ENTCURVE2D {
} ENTLINE2D, *PENTLINE2D;

typedef struct tag_ENTARC2D : public tag_ENTCURVE2D {
	POINT2D	Point0;                        //圆心
	double		Radius;                        //半径
	double		StartAngle;                  //起始角度
	double		EndAngle;		               //结束角度
	BOOL		AnticlockWiseFlag;      //"逆时针标志" 组码：73
} ENTARC2D, *PENTARC2D;

typedef struct tag_ENTELLIPSE2D : public tag_ENTCURVE2D {
	POINT2D	CenterPoint;
	POINT2D	MajorAxisEndPoint;
	double		MinorToMajorRatio;
	double		StartParam;
	double		EndParam;
	BOOL		AnticlockWiseFlag; //"逆时针标志" 组码：73
} ENTELLIPSE2D, *PENTELLIPSE2D;

typedef struct tag_ENTSPLINE2D : public tag_ENTCURVE2D {
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
} ENTSPLINE2D,*PENTSPLINE2D;

typedef struct tag_ENTVERTEX2D {
	POINT2D		Point;             //点坐标
	double			Bulge;			  //凸度
} ENTVERTEX2D, *PENTVERTEX2D;

typedef struct tag_ENTPOLYLINE2D : public tag_ENTCURVE2D {
	unsigned short	Flag;                 //多段线标志
	int nVertex;          	                    //多段线顶点数
	PENTVERTEX2D pVertex;	        //多段线顶点
} ENTPOLYLINE2D, *PENTPOLYLINE2D;

typedef struct tag_ENTLINE {  //直线对象，起点，终点
	REALPOINT Point0;
	REALPOINT Point1;
} ENTLINE, *PENTLINE;

typedef struct tag_ENTARC {  //圆弧对象
	REALPOINT	Point0;              //圆心
	double		Radius;              //半径  组码：100
	double		StartAngle;              //起始角度  组码：50
	double		EndAngle;              //终点角度  组码：51
} ENTARC, *PENTARC;

typedef struct tag_ENTCIRCLE {   //圆对象
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
	char		BlockHeaderName[512];       //块名称
	double      StretchDirectZ;  //拉伸方向Z     组码：230
} ENTINSERT, *PENTINSERT;

typedef struct tag_ENTPOINT {   //点图元对象
	REALPOINT	Point0;
} ENTPOINT, *PENTPOINT;

typedef struct tag_ENTSOLID {      //实体图元
	REALPOINT	Point0;
	REALPOINT	Point1;
	REALPOINT	Point2;
	REALPOINT	Point3;
} ENTSOLID, *PENTSOLID;

typedef struct tag_TEXTDATA {      //单行文本数据
	double		Height;                       //高度
	double		RotationAngle;           //旋转角度
	double		WidthFactor;             // 相对 X 比例因子
	double		Oblique;                //倾斜角
	int		    GenerationFlag;     //文字生成标志
	short		Justification;          //文字水平对正类型
	REALPOINT	Point1;              //第二对齐点
	REALPOINT  ExtrudeDirect;          //拉伸方向
	OBJHANDLE	TextStyleObjhandle;   //文本样式句柄
} _TEXTDATA,*_PTEXTDATA;

typedef struct tag_MTEXTDATA {       //多行文本
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
	double		LineSpaceScale;         //要应用的行距的默认百分比（五分之三）。有效值的范围从 0.25 到 4.00
	short        FillBkFlag;                   //0 = 背景填充关闭 1 = 使用背景填充颜色 2 = 使用图形窗口颜色作为背景填充颜色
	int            ColorIndex;                //背景色（颜色索引编号）
	double		FillRectScale;              //填充框大小（可选）：确定文字周围的边框大小。
	int            FillAlpha;                    //背景填充颜色的透明度（未实现）
} _MTEXTDATA,*_PMTEXTDATA;

typedef struct tag_ENTTEXT {
	REALPOINT	Point0;                     //插入点
	_TEXTDATA	TextData;                 //文本数据
	WCHAR		strText[1024];          //文字内容
	REALRECT    	TextRect;                 //文字区域
} ENTTEXT, *PENTTEXT;

typedef struct tag_ENTMTEXT {
	OBJHANDLE	TextStyleObjhandle;   //文本样式句柄
	REALPOINT	Point0;                       //插入点
	_MTEXTDATA	TextData;               //文本数据
	WCHAR		strText[1024];            //文字内容
} ENTMTEXT, *PENTMTEXT;

typedef struct tag_ENTVERTEX {      //顶点图元
  REALPOINT		Point;            //点坐标
  unsigned short	Flag;              //标志
  double			StartWidth;       //起点宽度
  double			EndWidth;         //终点宽度
  double			Bulge;               //凸度
  double			TangentDir;      //切线方向
  int PolyFaceVertexIndex1;      //多面网格顶点索引（可选；非零时出现）
  int PolyFaceVertexIndex2;      //多面网格顶点索引（可选；非零时出现）
  int PolyFaceVertexIndex3;      //多面网格顶点索引（可选；非零时出现）
  int PolyFaceVertexIndex4;      //多面网格顶点索引（可选；非零时出现）
} ENTVERTEX, *PENTVERTEX;

typedef struct tag_ENTPOLYCURVE {
	unsigned short	Flag;                 //多段线标志
	bool Closed;              	            //多段线是否闭合
	int nPolyPoints;                        //多段线边界点个数
	PREALPOINT pPolyPoints;        //多段线边界点坐标
} ENTPOLYCURVE, *PENTPOLYCURVE;

typedef struct tag_ENTPOLYLINEBASE : public tag_ENTPOLYCURVE {
	int nVertex;				                //多段线顶点数
	PENTVERTEX pVertex;			    //多段线顶点
	bool IsMeshMClosed;       	    //多边形网格以 M 方向闭合
	bool IsMeshNClosed;	            //多边形网格以 N 方向闭合
	double FixedWidth;           	    //组码43 固定宽度(可选;默认值 = 0). 如果设置为可变宽度(代码40 / 41), 则不使用
	unsigned int	MeshM;		        //组码：71 多边形网格 M 顶点计数（可选；默认值 = 0）
	unsigned int	MeshN;	   	  	    //组码：72 多边形网格 N 顶点计数（可选；默认值 = 0）
	int nArrows;						        //多段线线宽不为零的矢量坐标点数  
	PREALPOINT pArrows;			    //多段线线宽不为零的矢量坐标点
} ENTPOLYLINEBASE, *PENTPOLYLINEBASE;

typedef struct tag_ENTPOLYLINE : public tag_ENTPOLYLINEBASE{
	bool IsPolygonMesh;	        //这是一个多边形网格
	bool IsPolyFaceMesh;	        //多段线是一个多面网格
	REALPOINT VPoint;	            //DXF：始终为 0 APP：“虚拟”点；X 值和 Y 值始终为 0，Z 值是多段线的标高
	double StartWidth;	            //组码：40 默认起点宽度（可选；默认值 = 0）
	double EndWidth;	                //组码：41 默认端点宽度（可选；默认值 = 0）
	unsigned short PolyFlag;	    //组码：75 曲线和平滑曲面类型（可选；默认值 = 0）
	int nVertexIndexes;	            //多面网格顶点索引数
	int *pPolyFaceVertexIndexes;	//多面网格顶点索引值
} ENTPOLYLINE, *PENTPOLYLINE;

typedef struct tag_ENTLWPOLYLINE : public tag_ENTPOLYLINEBASE {
} ENTLWPOLYLINE, *PENTLWPOLYLINE;

typedef struct tag_ENTSPLINE {   //样条曲线对象
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
	SimpleList<REALPOINT> *pControlNode;                 //控制点值
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

typedef struct tag_ENTELLIPSE {
  REALPOINT		CenterPoint;   //中心点
  REALPOINT		MajorAxisEndPoint;  //相对于中心的长轴端点
  double		MinorToMajorRatio;  //短轴与长轴的比例 组码：40
  double		StartParam;       //开始参数（对于闭合椭圆，该值为 0.0）组码：41
  double		EndParam;         //结束参数（对于闭合椭圆，该值为 2pi）组码：42
  double		ExtrudeDirectX;  //拉伸方向X
  double		ExtrudeDirectY;  //拉伸方向Y 拉伸方向（可选；默认值 = 0, 0, 1）
  double		ExtrudeDirectZ;  //拉伸方向Z
} ENTELLIPSE, *PENTELLIPSE;

typedef struct tag_ENTLEADER{
	PENTVERTEX		pVertex;
	int				nVertex;
	unsigned short	Flag;
} ENTLEADER, *PENTLEADER;

typedef struct tag_ENT3DFACE{
	REALPOINT	 Point0;        //第一视角点
	REALPOINT	 Point1;        //第二视角点
	REALPOINT   Point2;        //第三视角点
	REALPOINT   Point3;        //第四视角点
	bool   FirstSideVisible;	    //1 = 第一条边不可见 default = 0
	bool   SecondSideVisible;	//2 = 第二条边不可见 default = 0
	bool   ThirdSideVisible;	    //4 = 第三条边不可见 default = 0
	bool   FourthSideVisible;	//8 = 第四条边不可见 default = 0
} ENT3DFACE, *PENT3DFACE;

typedef struct tag_ENTTRACE{
	REALPOINT	 Point0;   //第一视角点
	REALPOINT	 Point1;   //第二视角点
	REALPOINT   Point2;   //第三视角点
	REALPOINT   Point3;   //第四视角点
} ENTTRACE, *PENTTRACE;

typedef  ENTCURVE2D  HATCHCURVE, *PHATCHCURVE;
typedef  ENTPOLYLINE2D  HATCHPOLYLINE, *PHATCHPOLYLINE;
typedef  ENTLINE2D  HATCHLINE, *PHATCHLINE;
typedef  ENTARC2D  HATCHARC, *PHATCHARC;
typedef  ENTELLIPSE2D  HATCHELLIPSE, *PHATCHELLIPSE;         
typedef  ENTSPLINE2D  HATCHSPLINE, *PHATCHSPLINE;                   

typedef struct tag_HATCHPATTERN {
	double lineAngle;      //图案直线角度
	REALPOINT baseP;   //图案直线基点
	REALPOINT offset;   //图案直线偏移
	int dashNum;           //虚线长度项目数 
	double* lines;           //虚线长度（多个条目)
	BOOL  isDash; 	       //是否为虚线
} HATCHPATTERN, *PHATCHPATTERN;

typedef struct tag_HATCHSOLID {
	unsigned short SolidPatternType;    //组码：450 表示实体图案填充或渐变色；0 = 实体图案填充 1 = 渐变色
	unsigned short ColorRefType;         //组码：452 记录定义颜色的方法；0 = 双色渐变色 1 = 单色渐变色
	int ColorNum;             //组码：453 表示颜色数
	double ColorAngle;     //组码：460 以弧度表示的渐变色的旋转角度（默认值 = 0, 0）
	BOOL GradientRef;     //组码：461 判定渐变色定义
	double Hue;               //组码：462 色调值
	int GradientColor[2];  //组码：421 颜色值
	char GradientName[MAX_STRLEN];  //组码：470 渐变色名称
} HATCHSOLID, *PHATCHSOLID;

typedef struct tag_HATCHBOUNDARY { 
	int BoundaryMark;   //边界路径类型标志（按位编码），组码：92
	int BoundaryNum;  //边界路径中的边数（仅当边界不是多段线时），组码：93
	int BoundaryObjectNum;  //源边界对象数，组码：97
	SimpleList <PHATCHCURVE> *pHatchCurveList;  //曲线边界数据
} HATCHBOUNDARY,*PHATCHBOUNDARY;

typedef struct tag_ENTHATCH {
	char  HatchName[MAX_STRLEN];//图案填充名称，组码：2
	int HatchType;                            //实体填充标志（实体填充 = 1；图案填充 = 0），组码：70 
	unsigned short HatchRefFlag;     //关联性标志（关联 = 1；无关联 = 0），组码：71
	int BoundaryPathNum;               //边界路径（环）数 ，组码：91
	PHATCHBOUNDARY pHatchBoundary;  //填充的边界路径数据
	int HatchStyle;				              //图案填充样式，组码：75
	HatchPatternStyle PatternStyle; //填充图案样式
	unsigned short PatternType;      //填充图案类型：0 = 用户定义；1 = 预定义；2 = 自定义，组码：76
	double PatternFillAngle;             //填充图案角度（仅限图案填充）,组码：52
	double PatternFillScale;             //填充图案比例或间距（仅限图案填充）,组码：41
	int BoundaryAnnotation;           //边界注释标志,组码：73
	unsigned short BidirectionalMark; //填充图案双向标志（仅限图案填充）：0 = 非双向；1 = 双向 组码：77
	int LineNum;                             //图案定义直线数，组码：78
	PHATCHPATTERN pHatchPattern;  //图案直线数据。组码 78 指定的重复次数。
	PBOUNDARIES pBoundaries;   //图案填充边界坐标数据(用来计算边界)
	PHATCHSOLID pHatchSolid;    //填充颜色数据（仅限实体填充和渐变色填充）
	int ParseLinesNum;                  //图案填充解析坐标数目
	PREALPOINT pParseLines;        //图案填充解析坐标数据(用来画图案填充)
	HatchRect DRect;                    //图案填充矩形区域
} ENTHATCH, *PENTHATCH;

typedef struct tag_ENTHELIX : public tag_ENTSPLINE {
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
} ENTHELIX, *PENTHELIX;

typedef struct tag_ENTACADTABLE {
	char	BlockHeaderName[512];       //块名称
	REALPOINT Point;  		                //插入点
} ENTACADTABLE, *PENTACADTABLE;

typedef struct tag_ENTWIPEOUT : public tag_ENTPOLYCURVE{
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
} ENTWIPEOUT, *PENTWIPEOUT;

// Blocks Structures *******************************************

typedef struct tag_BLOCKHEADER
{
	OBJHANDLE	Objhandle;			   // 句柄
	char		Name[MAX_STRLEN];	   // 块名
	char		Flags;					           // 块类型标志
	REALPOINT	BasePoint;				   // 基点
	OBJHANDLE	LayerObjhandle;	   // 图层句柄
	ENTITIES	Entities;                       // 图元
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
