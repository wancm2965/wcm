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
	char   CadVer[512]; //CAD�汾��
}DXFHEADER, *PDXFHEADER;

//  TABLES --------
typedef struct tag_DXFLAYER
{
	char	Name[512];					// ͼ����
	int		StandardFlags;				// ��׼���ֵ
	int		Color;						// ͼ����ɫ
	char	LineType[512];				//   ����
} DXFLAYER, *PDXFLAYER;

typedef struct tag_DXFLTYPE
{
	char	Name[512];					//  ������
	int		StandardFlags;				// ��׼���ֵ
	char	DescriptiveText[512];		//  ���͵�˵������
	int		ElementsNumber;				// ����Ԫ�ص���Ŀ
	double	TotalPatternLength;			// ͼ���ܳ���
	double	Elements[30];				// ��Ԫ��
} DXFLTYPE, *PDXFLTYPE;

typedef struct tag_DXFSTYLE
{
	char	Name[512];					// ��ʽ��
	char	PrimaryFontFilename[512];	// ��Ҫ�����ļ���
	int		StandardFlags;				// ��׼���ֵ
	int		TextGenerationFlags;		// �������ɱ�־ -> 2=Text is backward  4=Text is upside down
	double	FixedTextHeight;			// �̶������ָ߶�
	double	WidthFactor;				// �������
	double	Height;						// �ϴ�ʹ�õĸ߶�
	double	ObliqueAngle;				// ��б�Ƕ�
} DXFSTYLE, *PDXFSTYLE;

typedef struct tag_DXFDIMSTYLE
{
	char	Name[512];					// ��ע��ʽ��
	int		StandardFlags;				// ��׼���ֵ
	int		DIMCLRD;					// Dimension line & Arrow heads color
	double	DIMDLE;						// Dimension line size after Extensionline
	int		DIMCLRE;					// Extension line color
	double	DIMEXE;						// Extension line size after Dimline
	double	DIMEXO;						// Offset from origin
	char	DIMBLK1[512];				// ��һ����ͷ
	char	DIMBLK2[512];				// �ڶ�����ͷ
	double	DIMASZ;						// ��ͷ��С
	char	DIMTXSTY[512];				// ������ʽ
	DWORD	DIMTXSTYObjhandle; // ������ʽ������
	int		DIMCLRT;					// ������ɫ
	double	DIMTXT;						// ���ָ߶�
	int		DIMTAD;						// Vertical Text Placement
	double	DIMGAP;						// 
} DXFDIMSTYLE, *PDXFDIMSTYLE;

//  BLOCKS --------
typedef struct tag_DXFBLOCKHEADER
{
	char		Name[512];				// ����
	char		Flags;					// �����ͱ��
	REALPOINT	BasePoint;				// ��׼��
	char		LayerName[512];			// ͼ����
} DXFBLOCKHEADER, *PDXFBLOCKHEADER;

//  ENTITIES ------
typedef struct tag_DXFENTITYHEADER
{
	unsigned short	EntityType;			// ͼԪ����
	char		LayerName[512];			// ͼ����
	char		LTypeName[512];			// ͼ��������
	short		Color;					// ��ɫ
	double		Thickness;				// ���
	double		LineTypeScale;			// ���ͱ���
	double		ExtrusionDirection[3];	// ѹ������
	short		LineWeight;				// ����
} DXFENTITYHEADER, *PDXFENTITYHEADER;

//ֱ��
typedef struct tag_DXFENTLINE {
	REALPOINT	Point0;                        //��ʼ��
	REALPOINT	Point1;                        //�յ�
} DXFENTLINE, *PDXFENTLINE;

//Բ��
typedef struct tag_DXFENTARC {
	REALPOINT	Point0;                         //Բ��
	double		Radius;                        //�뾶
	double		StartAngle;                    //��ʼ�Ƕ�
	double		EndAngle;                      //�����Ƕ�
} DXFENTARC, *PDXFENTARC;

//Բ
typedef struct tag_DXFENTCIRCLE {
	REALPOINT	Point0;                         //Բ��
	double		Radius;                           //�뾶
} DXFENTCIRCLE, *PDXFENTCIRCLE;

//��ע
typedef struct tag_DXFENTDIMENSION {
	REALPOINT	DefPoint3;
	REALPOINT	DefPoint4;
	double		DimRotationAngle;                 //��ע��ת�Ƕ�
	REALPOINT	DimLineDefPoint;                  
	char		BlockName[16];                    //����
	char		DimStyleName[512];                //��ע��ʽ��
	WCHAR		DimText[1024];                   //��ע����
} DXFENTDIMENSION, *PDXFENTDIMENSION;

//�����
typedef struct tag_DXFENTINSERT {
	REALPOINT	Point0;                        //�����
	double		XScale;                         //���췽��
	double		YScale;
	double		ZScale;
	double		RotationAngle;                  //��ת�Ƕ�
	char		BlockHeaderName[512];              //������
	double    StretchDirectZ;                //���췽��Z
} DXFENTINSERT, *PDXFENTINSERT;

//����ͼԪ
typedef struct tag_DXFENTVERTEX {
	REALPOINT			Point;          //������
	unsigned short	Flag;              //��־
	double			StartWidth;        //��ʼ���
	double			EndWidth;          //�յ���
	double			Bulge;             //ͻ��
	double			TangentDir;        //
} DXFENTVERTEX, *PDXFENTVERTEX;

//nurbs����
typedef struct tag_DXFENTSPLINE{
	REALPOINT NormalVecetor;               //����ʸ��
	int SplineMarks;                       //�������߱�־,���룺70
	int SplineOrder;                       //�������߽���,���룺71
	int NodeNum;	                       //�ڵ���,���룺72
	int ControlNodeNum;                    //���Ƶ��������룺73
	int FittingPointNum;                   //��ϵ���,���룺74
	double NodeTolerance;                  //�ڵ㹫��,���룺42
	double  ControlTolerance;              //���Ƶ㹫����룺43
	double FittingTolerance;               //��Ϲ�����룺44
	REALPOINT StartTangent;                //�������
	REALPOINT EndTangent;                  //�˵�����
	double *pNodePoint;                   //�ڵ�ֵ�������Ŀ��,���룺40
	double Weight;                         //Ȩֵ,���룺41
    SimpleList<REALPOINT> *pControlNode;                 //���Ƶ�ֵ��
	SimpleList<REALPOINT> *pFittingNode;                 //��ϵ�ֵ

}DXFENTSPLINE,*PDXFENTSPLINE;

//����ͼԪ
typedef struct tag_MLINEVETEX{
    REALPOINT VetexPoint;                 //��������
	REALPOINT DirVetex;                   //�Ӹö��㿪ʼ���߶εķ���ʸ���������Ŀ��ÿ������һ����Ŀ��
	REALPOINT ObliqueDirVetex;            //�˶��㴦��б�ӷ���ʸ��                
	double *pElemParem;       //Ԫ�ز��������룺41
	//int RegionFillParemNum;               //Ԫ�ص���������������Ŀ,���룺75
	//double RegionFillParem;               //���������������룺42
	double *pRegionFillParem;
	int RegionFillParemNum; 
}MLINEVETEX,*PMLINEVETEX;

typedef struct tag_DXFENTMLINE{
	double ScaleFacter;                    //�������ӣ����룺40
	int Alige;                             //������0 = �ϣ�1 = �ޣ�2 = ��,���룺70
	int Simble;                           //��־����λ����ֵ����
	                                       //1 = ������һ�����㣨���� 72 ���� 0��
                                           //2 = �ر�
		                                   //4 = ��ֹ����� 
                                           //8 = ��ֹ�˵��� 
	                                       //���룺71
	int VetexNum;                         //�����������룺72
	REALPOINT StartPoint;                 //���
	SimpleList<MLINEVETEX> *pVetexPoint;                 //����
	//REALPOINT DirVetex;                   //�Ӹö��㿪ʼ���߶εķ���ʸ���������Ŀ��ÿ������һ����Ŀ��
	//REALPOINT ObliqueDirVetex;            //�˶��㴦��б�ӷ���ʸ��
	int ParemNum;                         //��Ԫ�صĲ�����Ŀ,���룺74
	//double ElemParem;                     //Ԫ�ز��������룺41
	//int RegionFillParemNum;                  //Ԫ�ص���������������Ŀ,���룺75
	//double RegionFillParem;               //���������������룺42
      
}DXFENTMLINE,*PDXFENTMLINE;

//���ͼԪ
typedef struct tag_DXFENTPOINT {
	REALPOINT	Point0;                  //����ֵ
} DXFENTPOINT, *PDXFENTPOINT;

//soildͼԪ
typedef struct tag_DXFENTSOLID {
	REALPOINT	Point0;               //��һ���������
	REALPOINT	Point1;               //�ڶ����������            
	REALPOINT	Point2;              //�������������
	REALPOINT	Point3;              //���ĸ��������
} DXFENTSOLID, *PDXFENTSOLID;

//������������
typedef struct tag_DXFTEXTDATA {
	double		Height;                  //�߶�
	double		RotationAngle;           //��ת�Ƕ�
	double		WidthFactor;             // ��� X ��������
	double		Oblique;                //��б��
	char		GenerationFlag;           //�������ɱ�־
	short		Justification;          //����ˮƽ��������
	REALPOINT	SecondAlignmentPoint;   //�ڶ������
	char		TextStyleName[512];       //������ʽ��
	double		x;
	double		y;
} _DXFTEXTDATA,*_PDXFTEXTDATA;

//������������
typedef struct tag_DXFMTEXTDATA {
	double		Height;             // //�߶�
	double		RotationAngle;          //��ת�Ƕ�
	double		WidthFactor;            // ��� X ��������
	double		Oblique;               //��б��
	char		GenerationFlag;           //�������ɱ�־
	short		Justification;          //����ˮƽ��������
	double		x_vec;
	double		y_vec;
	double		z_vec;
	char		TextStyleName[512];
	double		x;
	double		y;
} _DXFMTEXTDATA,*_PDXFMTEXTDATA;

//��������ͼԪ
typedef struct tag_DXFENTTEXT {
	REALPOINT		Point0;             //����λ��
	_DXFTEXTDATA	TextData;           //��������
	WCHAR			strText[1024];      
} DXFENTTEXT, *PDXFENTTEXT;

//��������ͼԪ
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

//�����ͼԪ
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

//��ԲͼԪ
typedef struct tag_DXFENTELLIPSE {
  REALPOINT		CenterPoint;
  REALPOINT		MajorAxisEndPoint;
  double		MinorToMajorRatio;
  double		StartParam;
  double		EndParam;
  double       StretchDirectX; //���췽��X
  double       StretchDirectY; //���췽��Y
  double       StretchDirectZ; //���췽��Z
} DXFENTELLIPSE, *PDXFENTELLIPSE;

//ͼ�����
typedef struct tag_HATCHPOYLINEDATA{
	int   CovexMark;						//����͹�ȡ���־ �����룺72
	int   CloseMark;						//���رա���־ �����룺73
	int   VertexNum;                        //����߶�����,����: 93
	SimpleList<REALPOINT> *pVertex;                    //����λ��
	int   Covex;                            //͹�ȣ���ѡ��Ĭ��ֵ = 0�������룺42
}HATCHPOYLINEDATA,*PHATCHPOYLINEDATA;

typedef struct tag_HATCHARCDATA{

	REALPOINT CenterPoint;                  //���ĵ�λ��
	float Radius;                           //�뾶,���룺40
	float StartAngle;                       //���Ƕȣ����룺50
	float EndAngle;                         //�յ�Ƕȣ����룺51
	int   AcountClockWise;                  //��ʱ���־�����룺73
}HATCHARCDATA,*PHATCHARCDATA;

typedef struct tag_HATCHELLIPSEDATA  
{
	REALPOINT		CenterPoint;             //���ĵ�
	REALPOINT		MajorAxisEndPoint;       //��������ĵ�ĳ���˵�
	double		MinorToMajorRatio;           //����ĳ��ȣ�ռ���᳤�ȵı�����,����40
	double		StartParam;                  //���Ƕ�,���룺50
	double		EndParam;                    //�յ�Ƕ�,���룺51
}HATCHELLIPSEDATA,*PHATCHELLIPSEDATA;

typedef struct tag_HATCHSPLINEDATA
{
	int Order;                               //���������룺94
	int Cycle;                               //���ڣ����룺74
	int Rational;                            //����,���룺73
	int NodeNum;	                         //�ڵ���,���룺95
	int ControlNum;                          //���Ƶ��������룺96
	REALPOINT NodePoint;                     //�ڵ�ֵ�������Ŀ��,���룺40
	REALPOINT ControlPoint;                  //���Ƶ�
	double Weight;                           //Ȩֵ,���룺42
	int  FittingDataNum;                     //���������Ŀ,���룺97
	REALPOINT FitDatum;                      //	��ϻ�׼
	REALPOINT StartTangent;                  //�������
	REALPOINT EndTangent;                   //�˵�����

}HATCHSPLINEDATA,*PHATCHSPLINEDATA;

typedef struct tag_HATCHPATHLINEDATA
{
	/*double LineAngle;                       //ͼ��ֱ�߽Ƕ�,���룺53
	REALPOINT LineBase;                     //ͼ��ֱ�߻���
	REALPOINT LineMigration;                //ͼ��ֱ��ƫ��
	int DotLineNum;                         //���߳�����Ŀ�������룺 79
	double DotLineLen; */                     //���߳��� �����룺49
	REALPOINT	pStartVertex;
	REALPOINT	pEndVertex;

}HATCHPATHLINEDATA,*PHATCHPATHLINEDATA;

typedef struct tag_HATCHLINEDATA
{
	double LineAngle;                       //ͼ��ֱ�߽Ƕ�,���룺53
	REALPOINT LineBase;                     //ͼ��ֱ�߻���
	REALPOINT LineMigration;                //ͼ��ֱ��ƫ��
	int DotLineNum;                         //���߳�����Ŀ�������룺 79
	double DotLineLen;                     //���߳��� �����룺49
	REALPOINT	pStartVertex;
	REALPOINT	pEndVertex;

}HATCHLINEDATA,*PHATCHLINEDATA;

typedef struct tag_HATCHPATHDATA{
	//int PathRefMark;                         //�߽�·�����ͱ�־,0 = Ĭ�ϣ�1 = �ⲿ��2 = �����
	//4 = ������8 = �ı���16 = �����,���� ��92
	//int Paths;                             //�ñ߽�·���еı����������߽粻�Ƕ����ʱ��,���룺93
	int PathType;                          //�����ͣ������߽粻�Ƕ����ʱ����
	//1 = ֱ�ߣ�2 = Բ����3 = ��Բ����4 = ��������,���룺72
	int OrgPathNum;                       //Դ�߽������,����97
	//int nLineNum;
	SimpleList<HATCHPATHLINEDATA> *pHatchPathLineData;
	PHATCHELLIPSEDATA pHatchEllipseData;
	PHATCHARCDATA pHatchArcData;
	PHATCHSPLINEDATA pHatchSplineData;
	PHATCHPOYLINEDATA pHatchPoyLineData;

}HATCHPATHDATA,*PHATCHPATHDATA;//�߽�·������

typedef struct tag_DXFENTHATCH{

	char  HatchName[512];					//���ͼ���������룺2
	int   HatchType;						//ʵ������־��ʵ����� = 1��ͼ����� = 0�������룺70
	int   RefFlag;							//�����Ա�־������ = 1���޹��� = 0�������룺71
	int   Pathes;							//�߽�·���������� �����룺91
	int   HatchStyle;						//�����ʽ�����룺75
	int   PathMark; 						//�߽��־�����룺92������
	int   nPathSideNum;						//��ʾ�ñ߽�·���еı��������룺93
	HatchPathType hatchpathtype;			//���ͼ��·������
	PHATCHPATHDATA HatchPathData;					//���ı߽�·������
	int  PatternType;                       //���ͼ������,���룺76
	double  PatternFillAngle;               //���ͼ���Ƕȣ�����ͼ����䣩,���룺52
	double FillScale;                       //���ͼ���������ࣨ����ͼ����䣩,���룺41
	int    BoundaryAnnotation;              //�߽�ע�ͱ�־,���룺73
	int BidirectionalMark;                  //���ͼ��˫���־������ͼ����䣩��
	//0 = ��˫��1 = ˫�� ���룺77
	int LineNum;                            //ͼ������ֱ����,���룺78
	//PHATCHLINEDATA *hatchlinedata;          //ͼ��ֱ������
	SimpleList<HATCHLINEDATA> *pHatchLineData;//ͼ��ֱ������

	//		bool m_bHasCovex;			             //�Ƿ���Բ���ȶΣ����룺
	// 		double CornerPoints[8];					//����Ǿ��Σ���ʾ�ĸ��ǵ������,����Ϊ���ϵ㣬���ϵ㣬���µ㣬���µ�
	// 		double m_RoundRatioX;					//Բ�Ǿ��ε�Բ�ǰ뾶����γ��ȱ�
	// 		double m_RoundRatioY;			//Բ�Ǿ��ε�Բ�ǰ뾶����θ߶ȱ�

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
