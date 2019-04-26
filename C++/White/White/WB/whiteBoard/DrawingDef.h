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
	char  CadVer[MAX_STRLEN]; //cad �汾��
}HEADER, *PHEADER;

// Tables Structures *******************************************

typedef struct tag_DIMSTYLE {       //��ע��ʽ��
	OBJHANDLE Objhandle;			// ������
	char	Name[MAX_STRLEN];		// ��ע��ʽ��
	char	StandardFlags;			// ��׼���ֵ ���룺70
	                                //16 = ��������˴�λ�������Ŀ�ⲿ�������ⲿ����
		                           //32 = ���ͬʱ�����˴�λ��λ 16��������ѳɹ��������ⲿ�������ⲿ����
		                           //64 = ��������˴�λ����������ϴα༭ͼ��ʱ��
	                              // ͼ����������һ��ͼԪ�����˱���Ŀ��
	                              //���˱�־�� AutoCAD ��������á��������ȡ DXF �ļ��ĳ��򶼿��Ժ�������
	                              //����������д�� DXF �ļ��ĳ������������ã�
	double	dimasz;					// ��ͷ��С
//	OBJHANDLE dimblk1Objhandle;		// 1st Arrow head
//	OBJHANDLE dimblk2Objhandle;		// 2nd Arrow head
	char	dimblk1[16];			// һ�ż�ͷ ���룺6
	char	dimblk2[16];			// ���ż�ͷ  ���룺7
	short	dimclrd;				// ��ע�ߺͼ�ͷ����ɫ ���룺176
	short	dimclre;				//��չ����ɫ  ���룺177
	short	dimclrt;				// �ı���ɫ ���룺178
	double	dimdle;					// ��չ��֮��ı�ע�ߵĴ�С ���룺46
	double	dimexe;					// ��ע��֮�����չ�ߵĴ�С ���룺44
	double	dimexo;					// ��ԭ��Ŀ�ʼλ��  ���룺42
	double	dimgap;					// �ӱ�ע�߿�ʼ��λ��  ���룺147
	double	dimtxt;					// �ı��߶� ���룺140
	char	dimtad;					// ��ֱ�ı����� ���룺77
	OBJHANDLE dimtxstyObjhandle;	// �ı���ʽ���
} DIMSTYLE, *PDIMSTYLE;

typedef struct tag_LAYER {            //ͼ���
	OBJHANDLE Objhandle;			// ������
	char	Name[MAX_STRLEN];		// ͼ����
	char	StandardFlags;	               //��׼���  ���룺70
	                                  //��׼��ǣ���λ����ֵ����
	                                 //1 = ����ͼ�㣬����ⶳͼ�� 2 = Ĭ������������ӿ��ж���ͼ��
		                             //4 = ����ͼ�� 16 = ��������˴�λ�������Ŀ�ⲿ�������ⲿ����
		                             //32 = ���ͬʱ�����˴�λ��λ 16��������ѳɹ��������ⲿ�������ⲿ����
		                             //64 = ��������˴�λ����������ϴα༭ͼ��ʱ��ͼ����������һ��ͼԪ�����˱���Ŀ�����˱�־�� AutoCAD ��������á��������ȡ DXF �ļ��ĳ��򶼿��Ժ�����������������д�� DXF �ļ��ĳ������������ã�
	short	Color; 	 // ��ɫ��� ���룺62
	OBJHANDLE LineTypeObjhandle;	// ͼ������;��
	short	LineWeight;				// ͼ����߸�                  
	BOOL	PlotFlag;				// �Ƿ���Ʊ�� ���룺290  
	OBJHANDLE PlotStyleObjhandle;	// PlotStyleName ������
} LAYER, *PLAYER;

typedef struct tag_LTYPE {            //���ͱ�
	OBJHANDLE Objhandle;			// ������
	char	Name[MAX_STRLEN];		// ������ ���룺2
	char	StandardFlags;			// ��׼���  ���룺70
	char	DescriptiveText[512];	// ���͵�˵������  ���룺3
	short	ElementsNumber;			// ��Ԫ�ص���Ŀ   ���룺73
	double	Elements[30];			// ��Ԫ�ص�����
	double	PatternLength;          // ͼ���ܳ���  ���룺40
} LTYPE, *PLTYPE;

typedef struct tag_STYLE {             //������ʽ��
	OBJHANDLE Objhandle;			// ������
	char	Name[MAX_STRLEN];		// ��ʽ��   ���룺2
	char	StandardFlags;			// ��׼���  ���룺70
	char	TextGenerationFlags;	//�������ɱ�� ���룺71
	char	PrimaryFontFilename[256];// ��Ҫ�����ļ���   ���룺3
	char	BigFontFilename[256];	// �������ļ��� ���룺4
	double	ObliqueAngle;			// ��б�Ƕ� ���룺50
	double	FixedTextHeight;		// �̶��ı��߶� ���룺40
	double	WidthFactor;			// �������  ���룺41
	double	LastHeightUsed;			// �ϴ�ʹ�õĸ߶�  ���룺42
} STYLE, *PSTYLE;

// Entity Structures *******************************************

typedef struct tag_ENTITYHEADER //ͼԪ��ͷ��
{
	unsigned short	EntityType;			// ͼԪ����
	OBJHANDLE	Objhandle;				// ���
	OBJHANDLE	LayerObjhandle;			// ͼԪ����ͼ��ľ��
	OBJHANDLE	LTypeObjhandle;			// ͼԪʹ�õ����εľ��
	BOOL		Deleted;				// �Ƿ��ѱ�ɾ����־
	short		Color;					// ��ɫ (0=BYBLOCK, 256=BYLAYER, negative=layer is turned off)
	double		Thickness;				// ��� (default=0)
	double		LineTypeScale;			// ���Ա��� (default=1.0)
	double		ExtrusionDirection[3];	// ���췽��. (default = 0, 0, 1)
	short		LineWeight;				//
	DWORD		PreviousEntityPos;   //ǰһ��ͼԪλ��
	DWORD		NextEntityPos;       //��һ��ͼԪλ��
} ENTITYHEADER, *PENTITYHEADER;

typedef struct tag_ENTITIES
{
	HGLOBAL		hEntities;   //  ���
	DWORD		TotalSize;      //�ܵĴ�С
	DWORD		FreePos;    //����λ��
	DWORD		LastEntityPos;   //ǰһ��ͼԪλ��
	DWORD		CurrentEntityPos; //��ǰͼԪλ��
	DWORD		EntitiesNumber;  //ͼԪ��Ŀ
} ENTITIES, *PENTITIES;

typedef struct tag_ENTLINE {       //ֱ�߶�����㣬�յ�
	REALPOINT	Point0;
	REALPOINT	Point1;
} ENTLINE, *PENTLINE;

typedef struct tag_ENTARC {        //Բ������
	REALPOINT	Point0;              //Բ��
	double		Radius;              //�뾶  ���룺100
	double		StartAngle;              //��ʼ�Ƕ�  ���룺50
	double		EndAngle;              //�յ�Ƕ�  ���룺51
} ENTARC, *PENTARC;

typedef struct tag_ENTCIRCLE {
	REALPOINT	Point0;              //Բ��
	double		Radius;              //�뾶  ���룺40
} ENTCIRCLE, *PENTCIRCLE;

typedef struct tag_ENTDIMENSION {   //��עͼԪ����
	REALPOINT	DefPoint3;              //
	REALPOINT	DefPoint4;
	double		DimRotationAngle;              //��ע��ת�Ƕ�
	REALPOINT	DimLineDefPoint;              //��עֱ��Ĭ�ϵ�
	OBJHANDLE	BlockheaderObjhandle;              //��ͷ���
	OBJHANDLE	DimStyleObjhandle;              //��ע��ʽ���
	WCHAR		DimText[1024];              //��ע�ı�
} ENTDIMENSION, *PENTDIMENSION;

typedef struct tag_ENTINSERT {      //�����ͼԪ����
	REALPOINT	Point0;              //�����
	double		XScale;              //X����������  ���룺41
	double		YScale;              //Y����������  ���룺42
	double		ZScale;              //Z����������   ���룺43
	double		RotationAngle;              //��ת�Ƕ�  ���룺50
	OBJHANDLE	 BlockHeaderObjhandle;    //��ͷ���
	double      StretchDirectZ;  //���췽��Z     ���룺230
} ENTINSERT, *PENTINSERT;

//����һЩͼԪ�������û�н��н���
typedef struct tag_ENTPOINT
{   //��ͼԪ����
	REALPOINT	Point0;
} ENTPOINT, *PENTPOINT;

typedef struct tag_ENTSOLID {      //ʵ��ͼԪ
	REALPOINT	Point0;
	REALPOINT	Point1;
	REALPOINT	Point2;
	REALPOINT	Point3;
} ENTSOLID, *PENTSOLID;

typedef struct tag_TEXTDATA {      //�����ı�����
	double		Height;              //�ı��߶�  ���룺40
	double		RotationAngle;              //��ת�Ƕ�  ���룺50
	double		WidthFactor;              //�������  ���룺41
	double		Oblique;              //��б�Ƕ�  ���룺51
	char		GenerationFlag;              //�ı����ɱ�־ ���룺71
	                                          //�������ɱ�־����ѡ��Ĭ��ֵ = 0����
	                                          //2 = ���ַ����� X �᷽����
		                                     //4 = ���ֵ��ã��� Y �᷽����)

	short		Justification;              //���ִ�ֱ�������� ���룺73  
	                                        //���ִ�ֱ�������ͣ���ѡ��Ĭ��ֵ = 0���������루���ǰ�λ���룩
		                                   //0 = ���߶�����1 = �׶˶�����2 = ���ж�����3 = ���˶���
		                                   //��ϸ��Ϣ��μ��� 72 �� 73 ���������

	REALPOINT	SecondAlignmentPoint;         //�ڶ������  ����:11 
	OBJHANDLE	TextStyleObjhandle;              //�ı���ʽ���
} _TEXTDATA,*_PTEXTDATA;

typedef struct tag_MTEXTDATA {       //�����ı�
	double		Height;                         //��ƣ���ʼ�����ָ߶� ���룺40
	double		RotationAngle;              //�Ի���Ϊ��λ����ת�Ƕ� ���룺50
	double		WidthFactor;              //�������
	double		Oblique;              //��б�Ƕ�
	char		GenerationFlag;              //�ı����ɱ�־
	short		Justification;              //����
	double		x_vec;                       //X�᷽��ʸ��  ���룺11
	double		y_vec;                       //Y�᷽��ʸ��  ���룺21
	double		z_vec;                      //Z�᷽��ʸ��  ���룺31
	OBJHANDLE	TextStyleObjhandle;          //�ı���ʽ���
} _MTEXTDATA,*_PMTEXTDATA;

typedef struct tag_ENTTEXT {
	REALPOINT	Point0;                     //�����
	_TEXTDATA	TextData;                   //�ı�����
	WCHAR		strText[1024];              //����
} ENTTEXT, *PENTTEXT;

typedef struct tag_ENTMTEXT {
	REALPOINT	Point0; 
	_MTEXTDATA	TextData;
	WCHAR		strText[1024];
} ENTMTEXT, *PENTMTEXT;

typedef struct tag_ENTVERTEX {      //����ͼԪ
  REALPOINT			Point;
  unsigned short	Flag;         //��־
  double			StartWidth;       //�����
  double			EndWidth;         //�յ���
  double			Bulge;             //͹��
  double			TangentDir;        //���߷���
} ENTVERTEX, *PENTVERTEX;

//--------------this code is implemented by Tran duy Dung 2003/10/20
typedef struct tag_ENTPOLYLINE {   //�����
	PENTVERTEX		pVertex;          //����
	int				nVertex;
	unsigned short	Flag;
} ENTPOLYLINE, *PENTPOLYLINE;

typedef struct tag_ENTSPLINE 
{
	REALPOINT NormalVecetor;     //����ʸ��
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
}ENTSPLINE, *PENTSPLINE;

typedef struct tag_ENTMLINE{
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
}ENTMLINE,*PENTMLINE;

typedef struct tag_ENTLWPOLYLINE {
	PENTVERTEX		pVertex;
	int				nVertex;
	unsigned short	Flag;
} ENTLWPOLYLINE, *PENTLWPOLYLINE;

typedef struct tag_ENTELLIPSE {
  REALPOINT		CenterPoint;   //���ĵ�
  REALPOINT		MajorAxisEndPoint;  //��������ĵĳ���˵�
  double		MinorToMajorRatio;  //�����볤��ı��� ���룺40
  double		StartParam;       //��ʼ���������ڱպ���Բ����ֵΪ 0.0�����룺41
  double		EndParam;         //�������������ڱպ���Բ����ֵΪ 2pi�����룺42
  double      StretchDirectX;  //���췽��X
  double      StretchDirectY;  //���췽��Y ���췽�򣨿�ѡ��Ĭ��ֵ = 0, 0, 1��
  double      StretchDirectZ;  //���췽��Z
} ENTELLIPSE, *PENTELLIPSE;

typedef struct tag_ENTLEADER{
	PENTVERTEX		pVertex;
	int				nVertex;
	unsigned short	Flag;
} ENTLEADER, *PENTLEADER;

typedef struct tag_ENTHATCHPOYLINEDATA{
	int   CovexMark;						//����͹�ȡ���־ �����룺72
	int   CloseMark;						//���رա���־ �����룺73
	int   VertexNum;                        //����߶�����,����: 93
	SimpleList<REALPOINT> *pVertex;                    //����λ��
	int   Covex;                            //͹�ȣ���ѡ��Ĭ��ֵ = 0�������룺42
}ENTHATCHPOYLINEDATA,*PENTHATCHPOYLINEDATA;

typedef struct tag_ENTHATCHARCDATA
{
	REALPOINT CenterPoint;                  //���ĵ�λ��
	float Radius;                           //�뾶,���룺40
	float StartAngle;                       //���Ƕȣ����룺50
	float EndAngle;                         //�յ�Ƕȣ����룺51
	int   AcountClockWise;                  //��ʱ���־�����룺73
}ENTHATCHARCDATA,*PENTHATCHARCDATA;

typedef struct tag_ENTHATCHELLIPSEDATA  
{
	REALPOINT		CenterPoint;             //���ĵ�
	REALPOINT		MajorAxisEndPoint;       //��������ĵ�ĳ���˵�
	double		MinorToMajorRatio;           //����ĳ��ȣ�ռ���᳤�ȵı�����,����40
	double		StartParam;                  //���Ƕ�,���룺50
	double		EndParam;                    //�յ�Ƕ�,���룺51
}ENTHATCHELLIPSEDATA,*PENTHATCHELLIPSEDATA;

typedef struct tag_ENTHATCHSPLINEDATA
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

}ENTHATCHSPLINEDATA,*PENTHATCHSPLINEDATA;

typedef struct tag_ENTHATCHLINEDATA
{
	double LineAngle;                       //ͼ��ֱ�߽Ƕ�,���룺53
	REALPOINT LineBase;                     //ͼ��ֱ�߻���
	REALPOINT LineMigration;                //ͼ��ֱ��ƫ��
	int DotLineNum;                         //���߳�����Ŀ�������룺 79
	double DotLineLen;                      //���߳��� �����룺49
	REALPOINT	pStartVertex;
	REALPOINT	pEndVertex;

}ENTHATCHLINEDATA,*PENTHATCHLINEDATA;

typedef struct tag_ENTHATCHPATHLINEDATA
{
	/*double LineAngle;                       //ͼ��ֱ�߽Ƕ�,���룺53
	REALPOINT LineBase;                     //ͼ��ֱ�߻���
	REALPOINT LineMigration;                //ͼ��ֱ��ƫ��
	int DotLineNum;                         //���߳�����Ŀ�������룺 79
	double DotLineLen; */                     //���߳��� �����룺49
	REALPOINT	pStartVertex;
	REALPOINT	pEndVertex;

}ENTHATCHPATHLINEDATA,*PENTHATCHPATHLINEDATA;

typedef struct tag_ENTHATCHPATHDATA{
	int PathRefMark;                         //�߽�·�����ͱ�־,0 = Ĭ�ϣ�1 = �ⲿ��2 = �����
	                                        //4 = ������8 = �ı���16 = �����,���� ��92
	int Paths;                             //�ñ߽�·���еı����������߽粻�Ƕ����ʱ��,���룺93
	int PathType;                          //�����ͣ������߽粻�Ƕ����ʱ����
	                                       //1 = ֱ�ߣ�2 = Բ����3 = ��Բ����4 = ��������,���룺72
	int OrgPathNum;                       //Դ�߽������,����97
	SimpleList<ENTHATCHPATHLINEDATA> *pHatchPathLineData;  //·��Ϊֱ��
	PENTHATCHELLIPSEDATA pHatchEllipseData;            //·��Ϊ��Բ��
	PENTHATCHARCDATA pHatchArcData;                   //·��ΪԲ��
	PENTHATCHSPLINEDATA pHatchSplineData;           //·��Ϊ��������
	PENTHATCHPOYLINEDATA pHatchPoyLineData;           //·��Ϊ�����
	
}ENTHATCHPATHDATA,*PENTHATCHPATHDATA;//�߽�·������

typedef struct tag_ENTHATCH{
	
	char  HatchName[512];					//���ͼ���������룺2
	int   HatchType;						//ʵ������־��ʵ����� = 1��ͼ����� = 0�������룺70
	int   RefFlag;							//�����Ա�־������ = 1���޹��� = 0�������룺71
	int   Pathes;							//�߽�·���������� �����룺91
	int   HatchStyle;						//�����ʽ�����룺75
	int   PathMark; 						//�߽��־�����룺92������
	int   PathSideNum;						//��ʾ�ñ߽�·���еı��������룺93
	HatchPathType hatchpathtype;			//���ͼ��·������
	PENTHATCHPATHDATA* HatchPathData;					//���ı߽�·������
	int  PatternType;                       //���ͼ������,���룺76
	double  PatternFillAngle;               //���ͼ���Ƕȣ�����ͼ����䣩,���룺52
	double FillScale;                       //���ͼ���������ࣨ����ͼ����䣩,���룺41
	int    BoundaryAnnotation;              //�߽�ע�ͱ�־,���룺73
	int BidirectionalMark;                  //���ͼ��˫���־������ͼ����䣩��
                                            //0 = ��˫��1 = ˫�� ���룺77
	int LineNum;                            //ͼ������ֱ����,���룺78
	//PENTHATCHLINEDATA *hatchlinedata;          //ͼ��ֱ������
	SimpleList<ENTHATCHLINEDATA> *pHatchLineData;

	//		bool m_bHasCovex;			             //�Ƿ���Բ���ȶΣ����룺
	// 		double CornerPoints[8];					//����Ǿ��Σ���ʾ�ĸ��ǵ������,����Ϊ���ϵ㣬���ϵ㣬���µ㣬���µ�
	// 		double m_RoundRatioX;					//Բ�Ǿ��ε�Բ�ǰ뾶����γ��ȱ�
	// 		double m_RoundRatioY;			//Բ�Ǿ��ε�Բ�ǰ뾶����θ߶ȱ�

}ENTHATCH,*PENTHATCH;

typedef struct tag_ENTIMAGE{//ͼ��ͼԪ����
	int ImageWidth;
	int ImageHeigth;
}ENTIMAGE, *PENTIMAGE;

// Blocks Structures *******************************************

typedef struct tag_BLOCKHEADER
{
	OBJHANDLE	Objhandle;				// ���
	char		Name[MAX_STRLEN];		// ����
	char		Flags;					// �����ͱ�־
	REALPOINT	BasePoint;				// ����
	OBJHANDLE	LayerObjhandle;			// ͼ����
	ENTITIES	Entities;                   //ͼԪ
} BLOCKHEADER, *PBLOCKHEADER;

// View Structure **********************************************
typedef struct tag_VIEW
{
	BOOL	Viewable;		// �Ƿ�ɼ�
	int		WindowLeft;		// Drawing Window Boundary Properties (In Pixels)//����
	int		WindowTop;		// Drawing Window Boundary Properties (In Pixels)
	int		WindowRight;	// Drawing Window Boundary Properties (In Pixels)
	int		WindowBottom;	// Drawing Window Boundary Properties (In Pixels)
	double	ViewLeft;		// Drawing View Properties (In Units)//��λ
	double	ViewBottom;		// Drawing View Properties (In Units)
	double	ViewRight;		// Drawing View Properties (In Units)
	double	ViewTop;		// Drawing View Properties (In Units)
	double	PPU;			// ÿ����Ԫ��������
	double	ZoomLevel;	// ���ű���
} VIEW, *PVIEW;

#endif // DRAWINGDEF_H
