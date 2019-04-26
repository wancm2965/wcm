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
	double	LastHeightUsed;	// �ϴ�ʹ�õĸ߶�  ���룺42
	char FaceName[512];	    // ��������
} STYLE, *PSTYLE;

// Entity Structures *******************************************

typedef struct tag_ENTITYHEADER //ͼԪ��ͷ��
{
	unsigned short	 EntityType;			// ͼԪ����
	OBJHANDLE	 Objhandle;				// ���
	OBJHANDLE	 LayerObjhandle;		// ͼԪ����ͼ��ľ��
	OBJHANDLE  LTypeObjhandle;		// ͼԪʹ�õ����εľ��
	BOOL		Deleted;				// �Ƿ��ѱ�ɾ����־
	short		Color;					// ��ɫ (0=BYBLOCK, 256=BYLAYER, negative=layer is turned off)
	double		Thickness;				// ��� (default=0)
	double		LineTypeScale;			// ���ͱ��� (default=1.0)
	double		ExtrusionDirection[3];	// ���췽��. (default = 0, 0, 1)
	short		LineWeight;				//����
	DWORD	PreviousEntityPos;   //ǰһ��ͼԪλ��
	DWORD	NextEntityPos;       //��һ��ͼԪλ��
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

typedef struct tag_ENTCURVE2D {  //���߶���
	unsigned short EdgeType;    //0 = ����ߣ�1 = ֱ�ߣ�2 = Բ����3 = ��Բ����4 = ��������
	POINT2D StartPoint;	       //�������
	POINT2D EndPoint;             //�յ�����
    PPOINT2D pCoordPoint;     //�����
	int CoordCount;                  //��������
} ENTCURVE2D, *PENTCURVE2D;

typedef struct tag_ENTLINE2D : tag_ENTCURVE2D {
} ENTLINE2D, *PENTLINE2D;

typedef struct tag_ENTARC2D : public tag_ENTCURVE2D {
	POINT2D	Point0;                        //Բ��
	double		Radius;                        //�뾶
	double		StartAngle;                  //��ʼ�Ƕ�
	double		EndAngle;		               //�����Ƕ�
	BOOL		AnticlockWiseFlag;      //"��ʱ���־" ���룺73
} ENTARC2D, *PENTARC2D;

typedef struct tag_ENTELLIPSE2D : public tag_ENTCURVE2D {
	POINT2D	CenterPoint;
	POINT2D	MajorAxisEndPoint;
	double		MinorToMajorRatio;
	double		StartParam;
	double		EndParam;
	BOOL		AnticlockWiseFlag; //"��ʱ���־" ���룺73
} ENTELLIPSE2D, *PENTELLIPSE2D;

typedef struct tag_ENTSPLINE2D : public tag_ENTCURVE2D {
	int SplineOrder;                       //�������߽���,���룺94
	int SplineMarks;                       //�������߱�־,���룺73
	int NodeNum;		                   //�ڵ���,���룺95
	int ControlNodeNum;              //���Ƶ��������룺96
	int FittingPointNum;                //��ϵ���,���룺97
	POINT2D StartTangent;   	       //�������
	POINT2D EndTangent; 	           //�˵�����
	double *pNodePoint;               //�ڵ�ֵ�������Ŀ��,���룺40
	double Weight;                        //Ȩֵ,���룺42
	SimpleList<POINT2D> *pControlNode;  //���Ƶ�ֵ
	SimpleList<POINT2D> *pFittingNode;    //��ϵ�ֵ
} ENTSPLINE2D,*PENTSPLINE2D;

typedef struct tag_ENTVERTEX2D {
	POINT2D		Point;             //������
	double			Bulge;			  //͹��
} ENTVERTEX2D, *PENTVERTEX2D;

typedef struct tag_ENTPOLYLINE2D : public tag_ENTCURVE2D {
	unsigned short	Flag;                 //����߱�־
	int nVertex;          	                    //����߶�����
	PENTVERTEX2D pVertex;	        //����߶���
} ENTPOLYLINE2D, *PENTPOLYLINE2D;

typedef struct tag_ENTLINE {  //ֱ�߶�����㣬�յ�
	REALPOINT Point0;
	REALPOINT Point1;
} ENTLINE, *PENTLINE;

typedef struct tag_ENTARC {  //Բ������
	REALPOINT	Point0;              //Բ��
	double		Radius;              //�뾶  ���룺100
	double		StartAngle;              //��ʼ�Ƕ�  ���룺50
	double		EndAngle;              //�յ�Ƕ�  ���룺51
} ENTARC, *PENTARC;

typedef struct tag_ENTCIRCLE {   //Բ����
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
	char		BlockHeaderName[512];       //������
	double      StretchDirectZ;  //���췽��Z     ���룺230
} ENTINSERT, *PENTINSERT;

typedef struct tag_ENTPOINT {   //��ͼԪ����
	REALPOINT	Point0;
} ENTPOINT, *PENTPOINT;

typedef struct tag_ENTSOLID {      //ʵ��ͼԪ
	REALPOINT	Point0;
	REALPOINT	Point1;
	REALPOINT	Point2;
	REALPOINT	Point3;
} ENTSOLID, *PENTSOLID;

typedef struct tag_TEXTDATA {      //�����ı�����
	double		Height;                       //�߶�
	double		RotationAngle;           //��ת�Ƕ�
	double		WidthFactor;             // ��� X ��������
	double		Oblique;                //��б��
	int		    GenerationFlag;     //�������ɱ�־
	short		Justification;          //����ˮƽ��������
	REALPOINT	Point1;              //�ڶ������
	REALPOINT  ExtrudeDirect;          //���췽��
	OBJHANDLE	TextStyleObjhandle;   //�ı���ʽ���
} _TEXTDATA,*_PTEXTDATA;

typedef struct tag_MTEXTDATA {       //�����ı�
	double		Height;                       //�߶�
	double		RefRectWidth;            //���վ��ο��
 	short		Justification;               //���ֶ�������
 	short		TextDir;                      //ͼ�η���
 	REALPOINT ExtrudeDirect;         //���췽��
 	double		AngleVecX;                //���Ի���Ϊ��λ����ת�ǣ�ת��Ϊ��ȵķ���ʸ��X
 	double		AngleVecY;                //���Ի���Ϊ��λ����ת�ǣ�ת��Ϊ��ȵķ���ʸ��Y
	double		AngleVecZ;                //���Ի���Ϊ��λ����ת�ǣ�ת��Ϊ��ȵķ���ʸ��Z
	double		RotationAngle;           //�Ի���Ϊ��λ����ת�Ƕ�
	short        LineSpaceStyle;          //�������ֵ��о���ʽ����ѡ��: 1= ���٣������Ϊ�ϸߵ��ַ���2 = ��ȷ���������Ϊ�ϸߵ��ַ���
	double		LineSpaceScale;         //ҪӦ�õ��о��Ĭ�ϰٷֱȣ����֮��������Чֵ�ķ�Χ�� 0.25 �� 4.00
	short        FillBkFlag;                   //0 = �������ر� 1 = ʹ�ñ��������ɫ 2 = ʹ��ͼ�δ�����ɫ��Ϊ���������ɫ
	int            ColorIndex;                //����ɫ����ɫ������ţ�
	double		FillRectScale;              //�����С����ѡ����ȷ��������Χ�ı߿��С��
	int            FillAlpha;                    //���������ɫ��͸���ȣ�δʵ�֣�
} _MTEXTDATA,*_PMTEXTDATA;

typedef struct tag_ENTTEXT {
	REALPOINT	Point0;                     //�����
	_TEXTDATA	TextData;                 //�ı�����
	WCHAR		strText[1024];          //��������
	REALRECT    	TextRect;                 //��������
} ENTTEXT, *PENTTEXT;

typedef struct tag_ENTMTEXT {
	OBJHANDLE	TextStyleObjhandle;   //�ı���ʽ���
	REALPOINT	Point0;                       //�����
	_MTEXTDATA	TextData;               //�ı�����
	WCHAR		strText[1024];            //��������
} ENTMTEXT, *PENTMTEXT;

typedef struct tag_ENTVERTEX {      //����ͼԪ
  REALPOINT		Point;            //������
  unsigned short	Flag;              //��־
  double			StartWidth;       //�����
  double			EndWidth;         //�յ���
  double			Bulge;               //͹��
  double			TangentDir;      //���߷���
  int PolyFaceVertexIndex1;      //�������񶥵���������ѡ������ʱ���֣�
  int PolyFaceVertexIndex2;      //�������񶥵���������ѡ������ʱ���֣�
  int PolyFaceVertexIndex3;      //�������񶥵���������ѡ������ʱ���֣�
  int PolyFaceVertexIndex4;      //�������񶥵���������ѡ������ʱ���֣�
} ENTVERTEX, *PENTVERTEX;

typedef struct tag_ENTPOLYCURVE {
	unsigned short	Flag;                 //����߱�־
	bool Closed;              	            //������Ƿ�պ�
	int nPolyPoints;                        //����߽߱�����
	PREALPOINT pPolyPoints;        //����߽߱������
} ENTPOLYCURVE, *PENTPOLYCURVE;

typedef struct tag_ENTPOLYLINEBASE : public tag_ENTPOLYCURVE {
	int nVertex;				                //����߶�����
	PENTVERTEX pVertex;			    //����߶���
	bool IsMeshMClosed;       	    //����������� M ����պ�
	bool IsMeshNClosed;	            //����������� N ����պ�
	double FixedWidth;           	    //����43 �̶����(��ѡ;Ĭ��ֵ = 0). �������Ϊ�ɱ���(����40 / 41), ��ʹ��
	unsigned int	MeshM;		        //���룺71 ��������� M �����������ѡ��Ĭ��ֵ = 0��
	unsigned int	MeshN;	   	  	    //���룺72 ��������� N �����������ѡ��Ĭ��ֵ = 0��
	int nArrows;						        //������߿�Ϊ���ʸ���������  
	PREALPOINT pArrows;			    //������߿�Ϊ���ʸ�������
} ENTPOLYLINEBASE, *PENTPOLYLINEBASE;

typedef struct tag_ENTPOLYLINE : public tag_ENTPOLYLINEBASE{
	bool IsPolygonMesh;	        //����һ�����������
	bool IsPolyFaceMesh;	        //�������һ����������
	REALPOINT VPoint;	            //DXF��ʼ��Ϊ 0 APP�������⡱�㣻X ֵ�� Y ֵʼ��Ϊ 0��Z ֵ�Ƕ���ߵı��
	double StartWidth;	            //���룺40 Ĭ������ȣ���ѡ��Ĭ��ֵ = 0��
	double EndWidth;	                //���룺41 Ĭ�϶˵��ȣ���ѡ��Ĭ��ֵ = 0��
	unsigned short PolyFlag;	    //���룺75 ���ߺ�ƽ���������ͣ���ѡ��Ĭ��ֵ = 0��
	int nVertexIndexes;	            //�������񶥵�������
	int *pPolyFaceVertexIndexes;	//�������񶥵�����ֵ
} ENTPOLYLINE, *PENTPOLYLINE;

typedef struct tag_ENTLWPOLYLINE : public tag_ENTPOLYLINEBASE {
} ENTLWPOLYLINE, *PENTLWPOLYLINE;

typedef struct tag_ENTSPLINE {   //�������߶���
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
	SimpleList<REALPOINT> *pControlNode;                 //���Ƶ�ֵ
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

typedef struct tag_ENTELLIPSE {
  REALPOINT		CenterPoint;   //���ĵ�
  REALPOINT		MajorAxisEndPoint;  //��������ĵĳ���˵�
  double		MinorToMajorRatio;  //�����볤��ı��� ���룺40
  double		StartParam;       //��ʼ���������ڱպ���Բ����ֵΪ 0.0�����룺41
  double		EndParam;         //�������������ڱպ���Բ����ֵΪ 2pi�����룺42
  double		ExtrudeDirectX;  //���췽��X
  double		ExtrudeDirectY;  //���췽��Y ���췽�򣨿�ѡ��Ĭ��ֵ = 0, 0, 1��
  double		ExtrudeDirectZ;  //���췽��Z
} ENTELLIPSE, *PENTELLIPSE;

typedef struct tag_ENTLEADER{
	PENTVERTEX		pVertex;
	int				nVertex;
	unsigned short	Flag;
} ENTLEADER, *PENTLEADER;

typedef struct tag_ENT3DFACE{
	REALPOINT	 Point0;        //��һ�ӽǵ�
	REALPOINT	 Point1;        //�ڶ��ӽǵ�
	REALPOINT   Point2;        //�����ӽǵ�
	REALPOINT   Point3;        //�����ӽǵ�
	bool   FirstSideVisible;	    //1 = ��һ���߲��ɼ� default = 0
	bool   SecondSideVisible;	//2 = �ڶ����߲��ɼ� default = 0
	bool   ThirdSideVisible;	    //4 = �������߲��ɼ� default = 0
	bool   FourthSideVisible;	//8 = �������߲��ɼ� default = 0
} ENT3DFACE, *PENT3DFACE;

typedef struct tag_ENTTRACE{
	REALPOINT	 Point0;   //��һ�ӽǵ�
	REALPOINT	 Point1;   //�ڶ��ӽǵ�
	REALPOINT   Point2;   //�����ӽǵ�
	REALPOINT   Point3;   //�����ӽǵ�
} ENTTRACE, *PENTTRACE;

typedef  ENTCURVE2D  HATCHCURVE, *PHATCHCURVE;
typedef  ENTPOLYLINE2D  HATCHPOLYLINE, *PHATCHPOLYLINE;
typedef  ENTLINE2D  HATCHLINE, *PHATCHLINE;
typedef  ENTARC2D  HATCHARC, *PHATCHARC;
typedef  ENTELLIPSE2D  HATCHELLIPSE, *PHATCHELLIPSE;         
typedef  ENTSPLINE2D  HATCHSPLINE, *PHATCHSPLINE;                   

typedef struct tag_HATCHPATTERN {
	double lineAngle;      //ͼ��ֱ�߽Ƕ�
	REALPOINT baseP;   //ͼ��ֱ�߻���
	REALPOINT offset;   //ͼ��ֱ��ƫ��
	int dashNum;           //���߳�����Ŀ�� 
	double* lines;           //���߳��ȣ������Ŀ)
	BOOL  isDash; 	       //�Ƿ�Ϊ����
} HATCHPATTERN, *PHATCHPATTERN;

typedef struct tag_HATCHSOLID {
	unsigned short SolidPatternType;    //���룺450 ��ʾʵ��ͼ�����򽥱�ɫ��0 = ʵ��ͼ����� 1 = ����ɫ
	unsigned short ColorRefType;         //���룺452 ��¼������ɫ�ķ�����0 = ˫ɫ����ɫ 1 = ��ɫ����ɫ
	int ColorNum;             //���룺453 ��ʾ��ɫ��
	double ColorAngle;     //���룺460 �Ի��ȱ�ʾ�Ľ���ɫ����ת�Ƕȣ�Ĭ��ֵ = 0, 0��
	BOOL GradientRef;     //���룺461 �ж�����ɫ����
	double Hue;               //���룺462 ɫ��ֵ
	int GradientColor[2];  //���룺421 ��ɫֵ
	char GradientName[MAX_STRLEN];  //���룺470 ����ɫ����
} HATCHSOLID, *PHATCHSOLID;

typedef struct tag_HATCHBOUNDARY { 
	int BoundaryMark;   //�߽�·�����ͱ�־����λ���룩�����룺92
	int BoundaryNum;  //�߽�·���еı����������߽粻�Ƕ����ʱ�������룺93
	int BoundaryObjectNum;  //Դ�߽�����������룺97
	SimpleList <PHATCHCURVE> *pHatchCurveList;  //���߽߱�����
} HATCHBOUNDARY,*PHATCHBOUNDARY;

typedef struct tag_ENTHATCH {
	char  HatchName[MAX_STRLEN];//ͼ��������ƣ����룺2
	int HatchType;                            //ʵ������־��ʵ����� = 1��ͼ����� = 0�������룺70 
	unsigned short HatchRefFlag;     //�����Ա�־������ = 1���޹��� = 0�������룺71
	int BoundaryPathNum;               //�߽�·���������� �����룺91
	PHATCHBOUNDARY pHatchBoundary;  //���ı߽�·������
	int HatchStyle;				              //ͼ�������ʽ�����룺75
	HatchPatternStyle PatternStyle; //���ͼ����ʽ
	unsigned short PatternType;      //���ͼ�����ͣ�0 = �û����壻1 = Ԥ���壻2 = �Զ��壬���룺76
	double PatternFillAngle;             //���ͼ���Ƕȣ�����ͼ����䣩,���룺52
	double PatternFillScale;             //���ͼ���������ࣨ����ͼ����䣩,���룺41
	int BoundaryAnnotation;           //�߽�ע�ͱ�־,���룺73
	unsigned short BidirectionalMark; //���ͼ��˫���־������ͼ����䣩��0 = ��˫��1 = ˫�� ���룺77
	int LineNum;                             //ͼ������ֱ���������룺78
	PHATCHPATTERN pHatchPattern;  //ͼ��ֱ�����ݡ����� 78 ָ�����ظ�������
	PBOUNDARIES pBoundaries;   //ͼ�����߽���������(��������߽�)
	PHATCHSOLID pHatchSolid;    //�����ɫ���ݣ�����ʵ�����ͽ���ɫ��䣩
	int ParseLinesNum;                  //ͼ��������������Ŀ
	PREALPOINT pParseLines;        //ͼ����������������(������ͼ�����)
	HatchRect DRect;                    //ͼ������������
} ENTHATCH, *PENTHATCH;

typedef struct tag_ENTHELIX : public tag_ENTSPLINE {
	REALPOINT AxisBasePoint;  //�����
	REALPOINT Start;  //���
	double AxisVec[3];  //��ʸ��
	double Radius;  //�뾶
	double Turns;  //Ȧ��
	double TurnHeight;  //Ȧ��
	bool Flag;  //˳��ʱ���־
	int ConstraintType;  //Լ������
	int CoordCount;  //��������
	PREALPOINT pCoordPoint;  //�����
} ENTHELIX, *PENTHELIX;

typedef struct tag_ENTACADTABLE {
	char	BlockHeaderName[512];       //������
	REALPOINT Point;  		                //�����
} ENTACADTABLE, *PENTACADTABLE;

typedef struct tag_ENTWIPEOUT : public tag_ENTPOLYCURVE{
	REALPOINT Point;        //�����
	REALPOINT UVector;   //�����ص� U ʸ������ͼ��ɼ��ײ��ĵ㣬�Ӳ���㿪ʼ��
	REALPOINT VVector;   //�����ص� V ʸ������ͼ��ɼ����ĵ㣬�Ӳ���㿪ʼ��
	double UValue;            //DXF��ͼ���С�����أ��� U ֵ
	double VValue;            //DXF��ͼ���С�����أ��� V ֵ
	BOOL IsUseClipping;   //����״̬��0 = �رգ�1 = ��
	short Bright;                //����ֵ��0-100��Ĭ��ֵ = 50��
	short Contrast;            //�Աȶ�ֵ��0-100��Ĭ��ֵ = 50��
	short FadeUnder;        //�����ֵ��0-100��Ĭ��ֵ = 0��
	unsigned short ClippingBoundaryType;    //���ñ߽����͡�1 = ���Σ�2 = �����
	int nClip;                     //���ñ߽綥����
	REALPOINT *pClipPoints;  //DXF�����ñ߽綥��ֵ���� OCS �У��������Ŀ��
} ENTWIPEOUT, *PENTWIPEOUT;

// Blocks Structures *******************************************

typedef struct tag_BLOCKHEADER
{
	OBJHANDLE	Objhandle;			   // ���
	char		Name[MAX_STRLEN];	   // ����
	char		Flags;					           // �����ͱ�־
	REALPOINT	BasePoint;				   // ����
	OBJHANDLE	LayerObjhandle;	   // ͼ����
	ENTITIES	Entities;                       // ͼԪ
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
