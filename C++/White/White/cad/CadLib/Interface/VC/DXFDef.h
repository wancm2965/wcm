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
	char   CadVer[512]; //CAD�汾��
}DXFHEADER, *PDXFHEADER;

//  TABLES --------
typedef struct tag_DXFLAYER
{
	char	Name[512];					 // ͼ����
	int		StandardFlags;		     // ��׼���ֵ
	int		Color;						 // ͼ����ɫ
	char	LineType[512];				 // ����
} DXFLAYER, *PDXFLAYER;

typedef struct tag_DXFLTYPE
{
	char	Name[512];					    // ������
	int		StandardFlags;				// ��׼���ֵ
	char	DescriptiveText[512];		// ���͵�˵������
	int		ElementsNumber;			// ����Ԫ�ص���Ŀ
	double	TotalPatternLength;		// ͼ���ܳ���
	double	Elements[30];				// ��Ԫ��
} DXFLTYPE, *PDXFLTYPE;

typedef struct tag_DXFSTYLE
{
	char	Name[512];					          // ��ʽ��
	char	PrimaryFontFilename[512];	  // ��Ҫ�����ļ���
	char FaceName[512];                     // ��������
	int		StandardFlags;				      // ��׼���ֵ
	int		TextGenerationFlags;		  // �������ɱ�־ -> 2=Text is backward  4=Text is upside down
	double	FixedTextHeight;			      // �̶������ָ߶�
	double	WidthFactor;				          // �������
	double	Height;						          // �ϴ�ʹ�õĸ߶�
	double	ObliqueAngle;			          // ��б�Ƕ�
} DXFSTYLE, *PDXFSTYLE;

typedef struct tag_DXFDIMSTYLE
{
	char 	Name[512];					           // ��ע��ʽ��
	int		StandardFlags;				        // ��׼���ֵ
	int		DIMCLRD;					        // Dimension line & Arrow heads color
	double	DIMDLE;						        // Dimension line size after Extensionline
	int		DIMCLRE;					            // Extension line color
	double	DIMEXE;						        // Extension line size after Dimline
	double	DIMEXO;						        // Offset from origin
	char	DIMBLK1[512];				            // ��һ����ͷ
	char	DIMBLK2[512];				            // �ڶ�����ͷ
	double	DIMASZ;						        // ��ͷ��С
	char	DIMTXSTY[512];				        // ������ʽ
	DWORD	DIMTXSTYObjhandle;       // ������ʽ������
	int		DIMCLRT;					            // ������ɫ
	double	DIMTXT;						        // ���ָ߶�
	int		DIMTAD;						        // Vertical Text Placement
	double	DIMGAP;						        // 
} DXFDIMSTYLE, *PDXFDIMSTYLE;

//  BLOCKS --------
typedef struct tag_DXFBLOCKHEADER
{
	char		Name[512];		    // ����
	char		Flags;					// �����ͱ��
	REALPOINT	BasePoint;		// ��׼��
	char		LayerName[512];	// ͼ����
} DXFBLOCKHEADER, *PDXFBLOCKHEADER;

//  ENTITIES ------
typedef struct tag_DXFENTITYHEADER
{
	unsigned short	EntityType;			// ͼԪ����
	char		LayerName[512];			// ͼ����
	char		LTypeName[512];			// ������
	short		Color;					// ��ɫ
	double		Thickness;				// ���
	double		LineTypeScale;			// ���ͱ���
	double		ExtrusionDirection[3];	// ���췽��
	short		LineWeight;				// ����
} DXFENTITYHEADER, *PDXFENTITYHEADER;

//  2DCurve����
typedef struct tag_DXFENTCURVE2D {
	unsigned short EdgeType;    //0 = ����ߣ�1 = ֱ�ߣ�2 = Բ����3 = ��Բ����4 = ��������
	POINT2D StartPoint;	       //�������
	POINT2D EndPoint;             //�յ�����
	PPOINT2D pCoordPoint;     //�����
	int CoordCount;                  //��������
} DXFENTCURVE2D, *PDXFENTCURVE2D;

//  2DLineֱ��
typedef struct tag_DXFENTLINE2D : tag_DXFENTCURVE2D {
} DXFENTLINE2D, *PDXFENTLINE2D;

//  2DArcԲ��
typedef struct tag_DXFENTARC2D : public tag_DXFENTCURVE2D {
	POINT2D	Point0;                        //Բ��
	double		Radius;                        //�뾶
	double		StartAngle;                  //��ʼ�Ƕ�
	double		EndAngle;		               //�����Ƕ�
	BOOL		AnticlockWiseFlag;      //"��ʱ���־" ���룺73
} DXFENTARC2D, *PDXFENTARC2D;

//  2DEllipse��Բ
typedef struct tag_DXFENTELLIPSE2D : public tag_DXFENTCURVE2D {
	POINT2D	CenterPoint;
	POINT2D	MajorAxisEndPoint;
	double		MinorToMajorRatio;
	double		StartParam;
	double		EndParam;
	BOOL		AnticlockWiseFlag; //"��ʱ���־" ���룺73
} DXFENTELLIPSE2D, *PDXFENTELLIPSE2D;

//  2DSpline��������
typedef struct tag_DXFENTSPLINE2D : public tag_DXFENTCURVE2D {
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
} DXFENTSPLINE2D,*PDXFENTSPLINE2D;

//  2DVertex����
typedef struct tag_DXFENTVERTEX2D {
	POINT2D		Point;             //������
	double			Bulge;			  //͹��
} DXFENTVERTEX2D, *PDXFENTVERTEX2D;

//  2DPolyLine�����
typedef struct tag_DXFENTPOLYLINE2D : public tag_DXFENTCURVE2D {
	unsigned short	Flag;                 //����߱�־
	int nVertex;          	                    //����߶�����
	PDXFENTVERTEX2D pVertex;	//����߶���
} DXFENTPOLYLINE2D, *PDXFENTPOLYLINE2D;

//  Lineֱ��
typedef struct tag_DXFENTLINE {
	REALPOINT Point0;
	REALPOINT Point1;
} DXFENTLINE, *PDXFENTLINE;

//  ArcԲ��
typedef struct tag_DXFENTARC {
	REALPOINT	Point0;                    //Բ��
	double		Radius;                        //�뾶
	double		StartAngle;                  //��ʼ�Ƕ�
	double		EndAngle;		               //�����Ƕ�
} DXFENTARC, *PDXFENTARC;

//  CircleԲ
typedef struct tag_DXFENTCIRCLE {
	REALPOINT	Point0;                       //Բ��
	double		Radius;                           //�뾶
} DXFENTCIRCLE, *PDXFENTCIRCLE;

//  Dimension��ע
typedef struct tag_DXFENTDIMENSION {
	REALPOINT	DefPoint3;
	REALPOINT	DefPoint4;
	double		DimRotationAngle;                 //��ע��ת�Ƕ�
	REALPOINT	DimLineDefPoint;                  
	char		BlockName[16];                    //����
	char		DimStyleName[512];                //��ע��ʽ��
	WCHAR	DimText[1024];                   //��ע����
} DXFENTDIMENSION, *PDXFENTDIMENSION;

//  Block Insert�����
typedef struct tag_DXFENTINSERT {
	REALPOINT	Point0;                         //�����
	double		XScale;                             //���췽��
	double		YScale;
	double		ZScale;
	double		RotationAngle;                  //��ת�Ƕ�
	char		BlockHeaderName[512];       //������
	double	StretchDirectZ;                     //���췽��Z
} DXFENTINSERT, *PDXFENTINSERT;

//  Vertex����ͼԪ
typedef struct tag_DXFENTVERTEX {
	REALPOINT		Point;             //������
	unsigned short	Flag;               //��־
	double			StartWidth;        //��ʼ���
	double			EndWidth;          //�յ���
	double			Bulge;			      //͹��
	double      	TangentDir;        //���߷���
	int PolyFaceVertexIndex1;      //�������񶥵���������ѡ������ʱ���֣�
	int PolyFaceVertexIndex2;      //�������񶥵���������ѡ������ʱ���֣�
	int PolyFaceVertexIndex3;      //�������񶥵���������ѡ������ʱ���֣�
	int PolyFaceVertexIndex4;      //�������񶥵���������ѡ������ʱ���֣�
} DXFENTVERTEX, *PDXFENTVERTEX;

//  Spline��������
typedef struct tag_DXFENTSPLINE {
	REALPOINT NormalVecetor;    //����ʸ��
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
    SimpleList<REALPOINT> *pControlNode;               //���Ƶ�ֵ
	SimpleList<REALPOINT> *pFittingNode;                 //��ϵ�ֵ
}DXFENTSPLINE,*PDXFENTSPLINE;

//  MLine����ͼԪ
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

//  Point���ͼԪ
typedef struct tag_DXFENTPOINT {
	REALPOINT	Point0;                  //����ֵ
} DXFENTPOINT, *PDXFENTPOINT;

//  SoildͼԪ
typedef struct tag_DXFENTSOLID {
	REALPOINT	Point0;               //��һ���������
	REALPOINT	Point1;               //�ڶ����������            
	REALPOINT	Point2;               //�������������
	REALPOINT	Point3;               //���ĸ��������
} DXFENTSOLID, *PDXFENTSOLID;

//  Text������������
typedef struct tag_DXFTEXTDATA {
	double		Height;                       //�߶�
	double		RotationAngle;           //��ת�Ƕ�
	double		WidthFactor;             // ��� X ��������
	double		Oblique;                //��б��
	int		    GenerationFlag;     //�������ɱ�־
	short		Justification;          //����ˮƽ��������
	REALPOINT	Point1;              //�ڶ������
	REALPOINT ExtrudeDirect;          //���췽��
	char		TextStyleName[512];       //������ʽ��
} _DXFTEXTDATA,*_PDXFTEXTDATA;

//  MTextÿ����������

//  MText��������
typedef struct tag_DXFMTEXTDATA {
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
	double		LineSpaceScale;          //ҪӦ�õ��о��Ĭ�ϰٷֱȣ����֮��������Чֵ�ķ�Χ�� 0.25 �� 4.00
	short        FillBkFlag;                   //0 = �������ر� 1 = ʹ�ñ��������ɫ 2 = ʹ��ͼ�δ�����ɫ��Ϊ���������ɫ
	int            ColorIndex;                //����ɫ����ɫ������ţ�
	double		FillRectScale;              //�����С����ѡ����ȷ��������Χ�ı߿��С��
	int            FillAlpha;                    //���������ɫ��͸���ȣ�δʵ�֣�
} _DXFMTEXTDATA,*_PDXFMTEXTDATA;

//  Text��������ͼԪ
typedef struct tag_DXFENTTEXT {
	REALPOINT		Point0;               //�����
	_DXFTEXTDATA	TextData;           //��������
	WCHAR			strText[1024];    //�����ַ���
	REALRECT        	TextRect;           //��������
} DXFENTTEXT, *PDXFENTTEXT;

//  MText��������ͼԪ
typedef struct tag_DXFENTMTEXT {
	REALPOINT		Point0;                  //�����
	_DXFMTEXTDATA	TextData;          //��������
    WCHAR			strText[1024];       //��������
	char		    TextStyleName[512];    //������ʽ��
} DXFENTMTEXT, *PDXFENTMTEXT;

//  �����ͼԪ
typedef struct tag_DXFENTPOLYCURVE {
	unsigned short	Flag;                 //����߱�־
	bool Closed;              	            //������Ƿ�պ�
	int nPolyPoints;                        //����߽߱�����
	PREALPOINT pPolyPoints;        //����߽߱������
} DXFENTPOLYCURVE, *PDXFENTPOLYCURVE;

typedef struct tag_DXFENTPOLYLINEBASE : public tag_DXFENTPOLYCURVE {
	int nVertex;                              //����߶�����
	PDXFENTVERTEX pVertex;        //����߶���
	bool IsMeshMClosed;       	    //����������� M ����պ�
	bool IsMeshNClosed;	            //����������� N ����պ�
	double FixedWidth;           	    //����43 �̶����(��ѡ;Ĭ��ֵ = 0). �������Ϊ�ɱ���(����40 / 41), ��ʹ��
	unsigned int	MeshM;		        //���룺71 ��������� M �����������ѡ��Ĭ��ֵ = 0��
	unsigned int	MeshN;	   	  	    //���룺72 ��������� N �����������ѡ��Ĭ��ֵ = 0��
	int nArrows;						        //������߿�Ϊ���ʸ���������  
	PREALPOINT pArrows;			    //������߿�Ϊ���ʸ�������
} DXFENTPOLYLINEBASE, *PDXFENTPOLYLINEBASE;

//  PolyLine�����ͼԪ
typedef struct tag_DXFENTPOLYLINE : public tag_DXFENTPOLYLINEBASE {
	bool IsPolygonMesh;	        //����һ�����������
	bool IsPolyFaceMesh;	        //�������һ����������
	REALPOINT VPoint;	            //DXF��ʼ��Ϊ 0 APP�������⡱�㣻X ֵ�� Y ֵʼ��Ϊ 0��Z ֵ�Ƕ���ߵı��
	double StartWidth;	            //���룺40 Ĭ������ȣ���ѡ��Ĭ��ֵ = 0��
	double EndWidth;	                //���룺41 Ĭ�϶˵��ȣ���ѡ��Ĭ��ֵ = 0��
	unsigned short PolyFlag;	    //���룺75 ���ߺ�ƽ���������ͣ���ѡ��Ĭ��ֵ = 0��
	int nVertexIndexes;	            //�������񶥵�������
	int *pPolyFaceVertexIndexes;	//�������񶥵�����ֵ
} DXFENTPOLYLINE, *PDXFENTPOLYLINE;

//  LWPolyLine�����ͼԪ
typedef struct tag_DXFENTLWPOLYLINE : public tag_DXFENTPOLYLINEBASE {
}DXFENTLWPOLYLINE, *PDXFENTLWPOLYLINE;

//  Leader����ͼԪ
typedef struct tag_DXFENTLEADER {
	PDXFENTVERTEX	pVertex;
	int nVertex;
	unsigned short	Flag;
} DXFENTLEADER, *PDXFENTLEADER;

//  Ellipse��ԲͼԪ
typedef struct tag_DXFENTELLIPSE {
  REALPOINT		CenterPoint;
  REALPOINT		MajorAxisEndPoint;
  double		MinorToMajorRatio;
  double		StartParam;
  double		EndParam;
  double		ExtrudeDirectX; //���췽��X
  double		ExtrudeDirectY; //���췽��Y
  double		ExtrudeDirectZ; //���췽��Z
} DXFENTELLIPSE, *PDXFENTELLIPSE;

//  3DFaceͼԪ
typedef struct tag_DXFENT3DFACE{
	REALPOINT	 Point0; //��һ�ӽǵ�
	REALPOINT	 Point1; //�ڶ��ӽǵ�
	REALPOINT   Point2; //�����ӽǵ�
	REALPOINT   Point3; //�����ӽǵ�
	bool   FirstSideVisible;        //��һ���߿ɼ���
	bool   SecondSideVisible;   //�ڶ����߿ɼ���
	bool   ThirdSideVisible;       //�������߿ɼ���
	bool   FourthSideVisible;    //�������߿ɼ���
} DXFENT3DFACE, *PDXFENT3DFACE;

//  Trace����ͼԪ
typedef struct tag_DXFENTTRACE{
	REALPOINT	 Point0; //��һ�ӽǵ�
	REALPOINT	 Point1; //�ڶ��ӽǵ�
	REALPOINT   Point2; //�����ӽǵ�
	REALPOINT   Point3; //�����ӽǵ�
} DXFENTTRACE, *PDXFENTTRACE;

//  Hatchͼ�����
typedef  DXFENTCURVE2D  HATCHCURVEDATA, *PHATCHCURVEDATA;                 //���߽߱�����
typedef  DXFENTPOLYLINE2D  HATCHPOLYLINEDATA, *PHATCHPOLYLINEDATA;  //����߽߱�����
typedef  DXFENTLINE2D  HATCHLINEDATA, *PHATCHLINEDATA;                            //ֱ�߽߱�����
typedef  DXFENTARC2D  HATCHARCDATA, *PHATCHARCDATA;                              //Բ���߽�����
typedef  DXFENTELLIPSE2D  HATCHELLIPSEDATA, *PHATCHELLIPSEDATA;             //��Բ���߽�����
typedef  DXFENTSPLINE2D  HATCHSPLINEDATA, *PHATCHSPLINEDATA;               //�������߽߱�����

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
};  //ͼ���������

enum HatchBoundaryType {	
	HatchPathType_PolyLine = 0,
	HatchPathType_Line,
	HatchPathType_Circle,
	HatchPathType_Ellipse,
	HatchPathType_Spline
};  //�߽�����

enum HatchPatternStyle {
	HatchPatternStyle_Solid = 0,
	HatchPatternStyle_PatternData,
	HatchPatternStyle_Horizontal,
	HatchPatternStyle_Vertical,
	HatchPatternStyle_FDiagonal,
	HatchPatternStyle_BDiagonal,
	HatchPatternStyle_Cross,
	HatchPatternStyle_DiagCross
};  //ͼ����ʽ

typedef struct tag_LineWithBaseP {
	REALPOINT P1;
	REALPOINT P2;
	REALPOINT BP;
} LineWithBaseP, *PLineWithBaseP;  //��������

typedef struct tag_HATCHPATTERNDATA {
	double lineAngle;      //ͼ��ֱ�߽Ƕ�
	REALPOINT baseP;   //ͼ��ֱ�߻���
	REALPOINT offset;   //ͼ��ֱ��ƫ��
	int dashNum;           //���߳�����Ŀ�� 
	double *lines;           //���߳��ȣ������Ŀ)
	BOOL  isDash;          //�Ƿ�Ϊ����
} HATCHPATTERNDATA, *PHATCHPATTERNDATA;  //ͼ������

typedef struct tag_HATCHSOLIDDATA {
	unsigned short SolidPatternType;    //���룺450 ��ʾʵ��ͼ�����򽥱�ɫ��0 = ʵ��ͼ����� 1 = ����ɫ
	unsigned short ColorRefType;         //���룺452 ��¼������ɫ�ķ�����0 = ˫ɫ����ɫ 1 = ��ɫ����ɫ
	int ColorNum;             //���룺453 ��ʾ��ɫ��
	double ColorAngle;     //���룺460 �Ի��ȱ�ʾ�Ľ���ɫ����ת�Ƕȣ�Ĭ��ֵ = 0, 0��
	BOOL GradientRef;     //���룺461 �ж�����ɫ���� ��Ӧ���кͷǾ���
	double Hue;               //���룺462 ɫ��ֵ
	int GradientColor[2];  //���룺421 ��ɫֵ
	char GradientName[512];  //���룺470 ����ɫ����
} HATCHSOLIDDATA, *PHATCHSOLIDDATA;  //ʵ�������߽���ɫ�������

typedef struct tag_HATCHBOUNDARYDATA { 
	int BoundaryMark;   //�߽�·�����ͱ�־����λ���룩�����룺92
	int BoundaryNum;  //�߽�·���еı����������߽粻�Ƕ����ʱ�������룺93
	int BoundaryObjectNum;  //Դ�߽�����������룺97
	SimpleList <PHATCHCURVEDATA> *pHatchCurveList;  //���߽߱�����
}HATCHBOUNDARYDATA,*PHATCHBOUNDARYDATA;  //�߽�·������

typedef struct tag_BOUNDARIES {
	int BoundaryPointNum;             //�߽�����
	PPOINT2D pBoundaryPoint;      //�߽������
} BOUNDARIES, *PBOUNDARIES;  //�߽���������

typedef struct tag_DXFENTHATCH {
	char  HatchName[512];               //ͼ��������ƣ����룺2
	int HatchType;                            //ʵ������־��ʵ����� = 1��ͼ����� = 0�������룺70 
	unsigned short HatchRefFlag;     //�����Ա�־������ = 1���޹��� = 0�������룺71
	int BoundaryPathNum;               //�߽�·���������� �����룺91
	PHATCHBOUNDARYDATA pHatchBoundaryData;  //���ı߽�·������
	int HatchStyle;				              //ͼ�������ʽ�����룺75
	HatchPatternStyle PatternStyle; //���ͼ����ʽ
	unsigned short PatternType;      //���ͼ�����ͣ�0 = �û����壻1 = Ԥ���壻2 = �Զ��壬���룺76
	double PatternFillAngle;             //���ͼ���Ƕȣ�����ͼ����䣩,���룺52
	double PatternFillScale;             //���ͼ���������ࣨ����ͼ����䣩,���룺41
	int BoundaryAnnotation;           //�߽�ע�ͱ�־,���룺73
	unsigned short BidirectionalMark;  //���ͼ��˫���־������ͼ����䣩��0 = ��˫��1 = ˫�� ���룺77
	int LineNum;                             //ͼ������ֱ����������ͼ����䣩�����룺78
	PHATCHPATTERNDATA pHatchPatternData;  //ͼ��ֱ�����ݣ�����ͼ����䣩������ 78 ָ�����ظ�������
	PBOUNDARIES pBoundaries;   //ͼ�����߽���������(��������߽�)
	PHATCHSOLIDDATA pHatchSolidData;  //�����ɫ���ݣ�����ʵ�����ͽ���ɫ��䣩
	int ParseLinesNum;                  //ͼ��������������Ŀ
	PREALPOINT pParseLines;       //ͼ����������������(������ͼ�����)
	HatchRect DRect;                    //ͼ������������
} DXFENTHATCH, *PDXFENTHATCH;

//  Helix����ͼԪ
typedef struct tag_DXFENTHELIX : public tag_DXFENTSPLINE {
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
} DXFENTHELIX, *PDXFENTHELIX;

//  ACAD_TABLE���ͼԪ
typedef struct tag_DXFENTACADTABLE {
	char	BlockHeaderName[512];       //������
	REALPOINT Point;  		                //�����
} DXFENTACADTABLE, *PDXFENTACADTABLE;

//  WipeOut���򸲸�ͼԪ
typedef struct tag_DXFENTWIPEOUT : public tag_DXFENTPOLYCURVE {
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
