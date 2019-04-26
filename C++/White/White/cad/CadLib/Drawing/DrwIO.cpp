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
|  DrwIO.cpp                                                          |
\*-------------------------------------------------------------------*/

#include "stdafx.h"
#include "DrwIO.h"
#include "..\DXFFile\Read\DXFRead.h"
#include "..\DXFFile\Write\DXFWrite.h"


/********************************************************************
 * Progress Functions                                               *
 ********************************************************************/
long	CurrentEntityNum;
long	TotalEntitiesNum;
int		ProgPercent;
int		NewProgPercent;
HWND	hProgWnd;

/*-------------------------------------------------------------------*
 |  NotifySaveProgress                                               |
 |  Descriptions:                                                    |
 |      Notifies parent window if progress percentage has changed    |
 |  Inputs: Nothing                                                  |
 |  Output: Nothing                                                  |
 *-------------------------------------------------------------------*/
void NotifySaveProgress( )
{
	NewProgPercent = long((double)++CurrentEntityNum*100/TotalEntitiesNum);
	if(NewProgPercent>ProgPercent)
	{
		ProgPercent = NewProgPercent;
		SendMessage(hProgWnd, WM_PROGRESS, ProgPercent, 0);
	}
}

/*-------------------------------------------------------------------*
 |  NotifyLoadProgress                                               |
 |  Descriptions:                                                    |
 |      Notifies parent window if progress percentage has changed    |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the current DXF input file            |
 |  Output: Nothing                                                  |
 *-------------------------------------------------------------------*/
void NotifyLoadProgress( PDXF pDxf )
{
	NewProgPercent = long((double)pDxf->Read.StoredStreamPos*100/pDxf->Read.FileSize);
	if(NewProgPercent>ProgPercent)
	{
		ProgPercent = NewProgPercent;
		SendMessage(hProgWnd, WM_PROGRESS, ProgPercent, 0);
	}
}


/********************************************************************
 * Functions to write drawing to data file                          *
 ********************************************************************/

/*-------------------------------------------------------------------*
 |  WriteEntityDataToFile                                            |
 |  Descriptions:                                                    |
 |      Writes an entity into the output file (DXF file)             |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      HDXF hDxf = handle to the current DXF output file            |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      LPVOID pEntityData = pointer to entity data structure        |
 |  Output: Nothing                                                  |
 *-------------------------------------------------------------------*/
void WriteEntityDataToFile( HDRAWING hDrawing, HDXF hDxf, PENTITYHEADER pEntityHeader, LPVOID pEntityData )//往文件写数据
{
	LAYER		Layer;
	LTYPE		LType;
	STYLE		Style;
	DIMSTYLE	DimStyle;
	char*		pLayerName;
	char*		pLTypeName;
	char*		pStyleName;
	char*		pDimStyleName;
	
	// Initialize LayerName & LineTypeName & Color
	Layer.Objhandle = pEntityHeader->LayerObjhandle;
	if(drwFindTableType(hDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer)>0)
		pLayerName = Layer.Name;
	else
		pLayerName = NULL;

	LType.Objhandle = pEntityHeader->LTypeObjhandle;
	if(drwFindTableType(hDrawing, TAB_LTYPE, FIND_BYHANDLE, &LType)>0)
		pLTypeName = LType.Name;
	else
		pLTypeName = NULL;

	dxfSetCurrentLayer(hDxf, pLayerName, pLTypeName);
	dxfSetCurrentColor(hDxf, pEntityHeader->Color);

	// Write entities data to file
	switch(pEntityHeader->EntityType)
	{
	case ENT_LINE:
		dxfLine(
			hDxf, 
			((PENTLINE)pEntityData)->Point0.x,
			((PENTLINE)pEntityData)->Point0.y,
			((PENTLINE)pEntityData)->Point1.x,
			((PENTLINE)pEntityData)->Point1.y
		);
		break;

	case ENT_POINT:
		dxfPoint( 
			hDxf,
			((PENTPOINT)pEntityData)->Point0.x,
			((PENTPOINT)pEntityData)->Point0.y,
			pEntityHeader->Thickness
		);
		break;

	case ENT_CIRCLE:
		dxfCircle(
			hDxf,
			((PENTCIRCLE)pEntityData)->Point0.x,
			((PENTCIRCLE)pEntityData)->Point0.y,
			((PENTCIRCLE)pEntityData)->Radius
		);
		break;

	case ENT_ELLIPSE:
		dxfEllipse(
			hDxf, 
			((PENTELLIPSE)pEntityData)->CenterPoint.x,
			((PENTELLIPSE)pEntityData)->CenterPoint.y,
			((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.x,
			((PENTELLIPSE)pEntityData)->MajorAxisEndPoint.y,
			((PENTELLIPSE)pEntityData)->MinorToMajorRatio,
			((PENTELLIPSE)pEntityData)->StartParam,
			((PENTELLIPSE)pEntityData)->EndParam
		);
		break;

	case ENT_TEXT:
		// Find textstyle name
		Style.Objhandle = ((PENTTEXT)pEntityData)->TextData.TextStyleObjhandle;
		if(drwFindTableType(hDrawing, TAB_STYLE, FIND_BYHANDLE, &Style)>0)
			pStyleName = Style.Name;
		else
			pStyleName = NULL;

	/*	dxfText( 
			hDxf,
			((PENTTEXT)pEntityData)->strText,
			((PENTTEXT)pEntityData)->Point0.x,
			((PENTTEXT)pEntityData)->Point0.y,
			((PENTTEXT)pEntityData)->TextData.SecondAlignmentPoint.x,
			((PENTTEXT)pEntityData)->TextData.SecondAlignmentPoint.y,
			((PENTTEXT)pEntityData)->TextData.Height,
			((PENTTEXT)pEntityData)->TextData.RotationAngle,
			((PENTTEXT)pEntityData)->TextData.Justification,
			((PENTTEXT)pEntityData)->TextData.WidthFactor,
			pStyleName
		);*/
		break;

	case ENT_ARC:
		dxfArc(
			hDxf,
			((PENTARC)pEntityData)->Point0.x,
			((PENTARC)pEntityData)->Point0.y,
			((PENTARC)pEntityData)->Radius,
			((PENTARC)pEntityData)->StartAngle,
			((PENTARC)pEntityData)->EndAngle,
			pEntityHeader->Thickness
		);
		break;

	case ENT_SOLID:
		dxfSolid( 
			hDxf,
			((PENTSOLID)pEntityData)->Point0,
			((PENTSOLID)pEntityData)->Point1,
			((PENTSOLID)pEntityData)->Point2,
			((PENTSOLID)pEntityData)->Point3
		);
		break;

	case ENT_INSERT:
		BLOCKHEADER	BlockHeader;
		//BlockHeader.Objhandle = ((PENTINSERT)pEntityData)->BlockHeaderObjhandle;
		strcpy_s(BlockHeader.Name, ((PENTINSERT)pEntityData)->BlockHeaderName);
		if(drwFindBlock(hDrawing, /*FIND_BYHANDLE*/FIND_BYNAME, &BlockHeader)>0)
			dxfInsertBlock(
				hDxf,
				BlockHeader.Name,
				((PENTINSERT)pEntityData)->Point0.x,
				((PENTINSERT)pEntityData)->Point0.y,
				((PENTINSERT)pEntityData)->XScale,
				((PENTINSERT)pEntityData)->YScale,
				((PENTINSERT)pEntityData)->RotationAngle
			);
		break;

	case ENT_POLYLINE:
		dxfPolyLine(
			hDxf, 
			(PDXFENTVERTEX)((PENTPOLYLINE)pEntityData)->pVertex,
			((PENTPOLYLINE)pEntityData)->nVertex,
			((PENTPOLYLINE)pEntityData)->Flag
			);
		break;
	/*case ENT_SPLINE:
		dxfPolyLine(
			hDxf, 
			(PDXFENTVERTEX)((PENTSPLINE)pEntityData)->pVertex,
			((PENTSPLINE)pEntityData)->nVertex,
			((PENTSPLINE)pEntityData)->Flag
			);
		break;*/
	case ENT_LWPOLYLINE:
		dxfLWPolyLine(
			hDxf, 
			(PDXFENTVERTEX)((PENTLWPOLYLINE)pEntityData)->pVertex,
			((PENTLWPOLYLINE)pEntityData)->nVertex,
			((PENTLWPOLYLINE)pEntityData)->Flag
			);
		break;

	case ENT_DIMENSION:
		pStyleName = NULL;
		pDimStyleName = NULL;
		// Find dimstyle name
		DimStyle.Objhandle = ((PENTDIMENSION)pEntityData)->DimStyleObjhandle;
		if(drwFindTableType(hDrawing, TAB_DIMSTYLE, FIND_BYHANDLE, &DimStyle)<=0)
			drwFindTableType(hDrawing, TAB_DIMSTYLE, FIND_FIRST, &DimStyle);
		if(DimStyle.dimtxstyObjhandle>0)
		{
			pDimStyleName = DimStyle.Name;
			// Find textstyle name of dimension
			Style.Objhandle = DimStyle.dimtxstyObjhandle;
			if(drwFindTableType(hDrawing, TAB_STYLE, FIND_BYHANDLE, &Style)>0)
				pStyleName = Style.Name;
		}

		dxfSetCurrentDimStyle(hDxf, pDimStyleName);
		dxfSetCurrentTextStyle(hDxf, pStyleName);

		dxfDimLinear(
			hDxf,
			((PENTDIMENSION)pEntityData)->DefPoint3.x,
			((PENTDIMENSION)pEntityData)->DefPoint3.y,
			((PENTDIMENSION)pEntityData)->DefPoint4.x,
			((PENTDIMENSION)pEntityData)->DefPoint4.y,
			((PENTDIMENSION)pEntityData)->DimLineDefPoint.x,
			((PENTDIMENSION)pEntityData)->DimLineDefPoint.y,
			((PENTDIMENSION)pEntityData)->DimRotationAngle,
			((PENTDIMENSION)pEntityData)->DimText
		);
		break;
	}
}

/*-------------------------------------------------------------------*
 |  drwSaveDataToFile                                                |
 |  Descriptions:                                                    |
 |      Saves drawing into an output file (DXF file)                 |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      DWORD Reserved = reserved, must be 0                         |
 |      LPCTSTR strFileName = output file name (Can not be NULL)     |
 |      HWND hWndProgress = handle of progress window                |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwSaveDataToFile( HDRAWING hDrawing, DWORD Reserved, const char *strFileName, HWND hWndProgress )
{
	PDRAWING	pDrawing;
	HDXF		hDxf;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return FALSE;

	if(strFileName==NULL)
	{
		// Filename can not be NULL
		GlobalUnlock(hDrawing);
		return FALSE;
	}

	// Set Progress Percent to 0
	hProgWnd = hWndProgress;
	ProgPercent = 0;
	SendMessage(hWndProgress, WM_PROGRESS, ProgPercent, 0);
	// Count Total Entities Number
	TotalEntitiesNum = pDrawing->Entities.EntitiesNumber;
	for(UINT i=0; i<pDrawing->BlocksNumber; i++)
		TotalEntitiesNum += pDrawing->Blocks[i].Entities.EntitiesNumber;
	CurrentEntityNum = 0;

	// Creating DXF file and writing data in it
	hDxf = dxfCreateFile(strFileName);

	// Header Section ------------------------------------------
	dxfSectionBegin(hDxf, SEC_HEADER);
	dxfSectionEnd(hDxf);

	// Tables Section ------------------------------------------
	dxfSectionBegin(hDxf, SEC_TABLES);

	//  LTYPE table type -------------------------
	dxfTableTypeBegin(hDxf, TAB_LTYPE);

	DXFLTYPE	ltype;
	for(UINT i=0; i<pDrawing->Tables.NumLTypes; i++)
	{
		strcpy_s(ltype.Name, pDrawing->Tables.LType[i].Name);
		ltype.StandardFlags = pDrawing->Tables.LType[i].StandardFlags;
		strcpy_s(ltype.DescriptiveText, pDrawing->Tables.LType[i].DescriptiveText);
		ltype.ElementsNumber = pDrawing->Tables.LType[i].ElementsNumber;
		CopyMemory(ltype.Elements, pDrawing->Tables.LType[i].Elements, sizeof(double)*30);
		ltype.TotalPatternLength = pDrawing->Tables.LType[i].PatternLength;

		dxfAddLinetype(hDxf, &ltype);
	}

	dxfTableTypeEnd(hDxf);

	//  LAYER table type -------------------------
	dxfTableTypeBegin(hDxf, TAB_LAYER);

	for(UINT i=0; i<pDrawing->Tables.NumLayers; i++)
	{
		LTYPE	LType;
		char*	pLTypeName;

		LType.Objhandle = pDrawing->Tables.Layer[i].LineTypeObjhandle;
		if(drwFindTableType(hDrawing, TAB_LTYPE, FIND_BYHANDLE, &LType)>0)
			pLTypeName = LType.Name;
		else
			pLTypeName = NULL;

		dxfAddLayer(
			hDxf,
			pDrawing->Tables.Layer[i].Name,
			pDrawing->Tables.Layer[i].Color,
			pLTypeName
		);

	}
	
	dxfTableTypeEnd(hDxf);

	//  STYLE table type -------------------------
	dxfTableTypeBegin( hDxf, TAB_STYLE );

	DXFSTYLE tstyle;
	for(UINT i=0; i<pDrawing->Tables.NumStyles; i++)
	{
		strcpy_s(tstyle.Name, pDrawing->Tables.Style[i].Name);
		strcpy_s(tstyle.PrimaryFontFilename, pDrawing->Tables.Style[i].PrimaryFontFilename);
		tstyle.StandardFlags = pDrawing->Tables.Style[i].StandardFlags;
		tstyle.TextGenerationFlags = pDrawing->Tables.Style[i].TextGenerationFlags;
		tstyle.FixedTextHeight = pDrawing->Tables.Style[i].FixedTextHeight;
		tstyle.WidthFactor = pDrawing->Tables.Style[i].WidthFactor;
		tstyle.Height = pDrawing->Tables.Style[i].LastHeightUsed;
		tstyle.ObliqueAngle = pDrawing->Tables.Style[i].ObliqueAngle;

		dxfAddTextStyle(hDxf, &tstyle);
	}

	dxfTableTypeEnd(hDxf);

	//  DIMSTYLE table type ----------------------
	dxfTableTypeBegin(hDxf, TAB_DIMSTYLE);
	
	DXFDIMSTYLE	dimstyle;
	for(UINT i=0; i<pDrawing->Tables.NumDimStyles; i++)
	{
		STYLE	Style;
		char*	pStyleName;

		Style.Objhandle = pDrawing->Tables.DimStyle[i].dimtxstyObjhandle;
		if(drwFindTableType(hDrawing, TAB_STYLE, FIND_BYHANDLE, &Style)>0)
			pStyleName = Style.Name;
		else
			pStyleName = NULL;

		strcpy_s(dimstyle.Name, pDrawing->Tables.DimStyle[i].Name);
		dimstyle.StandardFlags = pDrawing->Tables.DimStyle[i].StandardFlags;
		dimstyle.DIMCLRD = pDrawing->Tables.DimStyle[i].dimclrd;
		dimstyle.DIMDLE = pDrawing->Tables.DimStyle[i].dimdle;
		dimstyle.DIMCLRE = pDrawing->Tables.DimStyle[i].dimclre;
		dimstyle.DIMEXE = pDrawing->Tables.DimStyle[i].dimexe;
		dimstyle.DIMEXO = pDrawing->Tables.DimStyle[i].dimexo;
		strcpy_s(dimstyle.DIMBLK1, pDrawing->Tables.DimStyle[i].dimblk1);
		strcpy_s(dimstyle.DIMBLK2, pDrawing->Tables.DimStyle[i].dimblk2);
		dimstyle.DIMASZ = pDrawing->Tables.DimStyle[i].dimasz;
		strcpy_s(dimstyle.DIMTXSTY, pStyleName);
		dimstyle.DIMCLRT = pDrawing->Tables.DimStyle[i].dimclrt;
		dimstyle.DIMTXT = pDrawing->Tables.DimStyle[i].dimtxt;
		dimstyle.DIMTAD = pDrawing->Tables.DimStyle[i].dimtad;
		dimstyle.DIMGAP = pDrawing->Tables.DimStyle[i].dimgap;

		dxfAddDimStyle(hDxf, &dimstyle);
	}

	dxfTableTypeEnd(hDxf);

	dxfSectionEnd(hDxf);

	// Blocks Section ------------------------------------------
	dxfSectionBegin(hDxf, SEC_BLOCKS);

	BLOCKHEADER		BlockHeader;
	ENTITYHEADER	    EntityHeader;
	char			EntityData[4096];

	if(drwFindBlock(hDrawing, FIND_FIRST, &BlockHeader)>0)
	{
		do{
			LAYER	Layer;
			LPCTSTR	pLayerName;
			DWORD	CurrentBlockNum;

			Layer.Objhandle = BlockHeader.LayerObjhandle;
			if(drwFindTableType(hDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer)>0)
				pLayerName = Layer.Name;
			else
				pLayerName = NULL;
			dxfBlockBegin(hDxf, BlockHeader.Name, pLayerName, BlockHeader.BasePoint.x, BlockHeader.BasePoint.y, BlockHeader.Flags);

			CurrentBlockNum = pDrawing->CurrentBlock;
			if(drwFindEntity(hDrawing, BlockHeader.Name, &EntityHeader, &EntityData, FIND_FIRST)>0)
			{
				do{
					if(!EntityHeader.Deleted)
						WriteEntityDataToFile(hDrawing, hDxf, &EntityHeader, &EntityData);
					NotifySaveProgress();
				} while(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &EntityHeader, &EntityData, FIND_NEXT)>0);
			}
			pDrawing->CurrentBlock = CurrentBlockNum;

			dxfBlockEnd(hDxf);
		} while(drwFindBlock(hDrawing, FIND_NEXT, &BlockHeader)>0);
	}

	dxfSectionEnd(hDxf);

	// Entities Section ----------------------------------------
	dxfSectionBegin(hDxf, SEC_ENTITIES);

	if(drwFindEntity(hDrawing, NULL, &EntityHeader, &EntityData, FIND_FIRST)>0)
	{
		do{
			if(!EntityHeader.Deleted)
				WriteEntityDataToFile(hDrawing, hDxf, &EntityHeader, &EntityData);
			NotifySaveProgress();
		} while(drwFindEntity( hDrawing, NULL, &EntityHeader, &EntityData, FIND_NEXT )>0);
	}

	dxfSectionEnd(hDxf);

	// Set Progress Percent to 100
	ProgPercent = 100;
	SendMessage(hWndProgress, WM_PROGRESS, ProgPercent, 0);

	// Close dxf file
	dxfCloseFile(hDxf);

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}


/********************************************************************
 * Functions to read drawing from a data file                       *
 ********************************************************************/

/*-------------------------------------------------------------------*
 |  ReadEntitiesDataFromFile                                         |
 |  Descriptions:                                                    |
 |      Reads entities data from the input file (DXF file)           |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      PDXF pDxf = pointer to the current DXF output file           |
 |      OBJHANDLE BlockObjhandle = handle of block that entities     |
 |                     must be put in it. (NULL = Entity Section)    |
 |  Output: Nothing                                                  |
 *-------------------------------------------------------------------*/
void ReadEntitiesDataFromFile( PDRAWING pDrawing, PDXF pDxf, OBJHANDLE BlockObjhandle )//从文件读数据
{
	DXFENTITYHEADER	DxfEntityHeader;
	ENTITYHEADER	 EntityHeader;
	char			DxfEntityData[4096];

	DIMSTYLE	DimStyle;
	LAYER			Layer;
	LTYPE			LType;
	STYLE			Style;

	ENTLINE			Line;
	ENTARC			Arc;
	ENTCIRCLE		Circle;
	ENTELLIPSE		Ellipse;
	ENTDIMENSION	Dimension;
	ENTINSERT		Insert;
	ENTPOINT		Point;
	ENTSOLID		Solid;
	ENTTEXT			Text;
	ENTMTEXT		MText;
	ENTPOLYLINE		Polyline;
	ENTLWPOLYLINE   LWPolyline;
	ENTLEADER		 Leader;
	ENTSPLINE       Spline;
	ENTMLINE        mline;
	ENT3DFACE      Face3D;
	ENTTRACE         Trace;
	ENTHATCH        Hatch;
	ENTHELIX          Helix;
	ENTACADTABLE AcadTable;
	ENTWIPEOUT    WipeOut;

	char			buf[512];

	while(dxfReadEntityData_Direct(pDxf, &DxfEntityHeader, &DxfEntityData)) //读出EntityHeader和EntityData数据
	{
		ZeroMemory(&EntityHeader, sizeof(ENTITYHEADER));
		EntityHeader.EntityType = DxfEntityHeader.EntityType;
		EntityHeader.Color = DxfEntityHeader.Color;
		EntityHeader.Thickness = DxfEntityHeader.Thickness;
		EntityHeader.LineTypeScale = DxfEntityHeader.LineTypeScale;
		memcpy(EntityHeader.ExtrusionDirection, DxfEntityHeader.ExtrusionDirection, sizeof(double)*3);
		EntityHeader.LineWeight = DxfEntityHeader.LineWeight;

		// Looking for layer name
		if(strlen(DxfEntityHeader.LayerName)>0)
		{
			strcpy_s(Layer.Name, DxfEntityHeader.LayerName);
			if(drwFindTableType_Direct(pDrawing, TAB_LAYER, FIND_BYNAME, &Layer)>0)
				EntityHeader.LayerObjhandle = Layer.Objhandle;
			else
			{
				// Layer is unavailable
				// Set to layer "0"
			}
		}
		else
		{
			// Set to default layer "0"
		}

		// Looking for line type name
		if(strlen(DxfEntityHeader.LTypeName)>0)
		{
			strcpy_s(LType.Name, DxfEntityHeader.LTypeName);
			if(drwFindTableType_Direct(pDrawing, TAB_LTYPE, FIND_BYNAME, &LType)>0)
				EntityHeader.LTypeObjhandle = LType.Objhandle;
			else
			{
				// LType is unavailable
				// Set to default ltype
			}
		}
		else
		{
			// Set to default ltype
		}

		switch(DxfEntityHeader.EntityType) //根据EntityHeader段类型
		{	
		case ENT_HATCH:
			{
				ZeroMemory(&Hatch, sizeof(ENTHATCH));
				strcpy_s(Hatch.HatchName, ((PDXFENTHATCH)&DxfEntityData)->HatchName);
				Hatch.HatchType = ((PDXFENTHATCH)&DxfEntityData)->HatchType;
				Hatch.HatchRefFlag = ((PDXFENTHATCH)&DxfEntityData)->HatchRefFlag;
				Hatch.BoundaryPathNum = ((PDXFENTHATCH)&DxfEntityData)->BoundaryPathNum;
				Hatch.pHatchBoundary = (PHATCHBOUNDARY)(((PDXFENTHATCH)&DxfEntityData)->pHatchBoundaryData);
				Hatch.HatchStyle = ((PDXFENTHATCH)&DxfEntityData)->HatchStyle;
				Hatch.PatternStyle = ((PDXFENTHATCH)&DxfEntityData)->PatternStyle;
				Hatch.PatternType = ((PDXFENTHATCH)&DxfEntityData)->PatternType;
				Hatch.PatternFillAngle = ((PDXFENTHATCH)&DxfEntityData)->PatternFillAngle;
				Hatch.PatternFillScale = ((PDXFENTHATCH)&DxfEntityData)->PatternFillScale;
				Hatch.BoundaryAnnotation = ((PDXFENTHATCH)&DxfEntityData)->BoundaryAnnotation;
				Hatch.BidirectionalMark = ((PDXFENTHATCH)&DxfEntityData)->BidirectionalMark;
				Hatch.LineNum = ((PDXFENTHATCH)&DxfEntityData)->LineNum;
				Hatch.pHatchPattern = (PHATCHPATTERN)(((PDXFENTHATCH)&DxfEntityData)->pHatchPatternData);
				Hatch.pHatchSolid = (PHATCHSOLID)((PDXFENTHATCH)&DxfEntityData)->pHatchSolidData;
				Hatch.pBoundaries = ((PDXFENTHATCH)&DxfEntityData)->pBoundaries;
				Hatch.ParseLinesNum = ((PDXFENTHATCH)&DxfEntityData)->ParseLinesNum;
				Hatch.pParseLines = ((PDXFENTHATCH)&DxfEntityData)->pParseLines;
				Hatch.DRect = ((PDXFENTHATCH)&DxfEntityData)->DRect;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Hatch);
			}
			break;

		case ENT_LINE:
			{
				ZeroMemory(&Line, sizeof(ENTLINE));
				Line.Point0 = ((PDXFENTLINE)&DxfEntityData)->Point0;
				Line.Point1 = ((PDXFENTLINE)&DxfEntityData)->Point1;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Line);
			}
			break;

		case ENT_POINT:
			{
				ZeroMemory(&Point, sizeof(ENTPOINT));
				Point.Point0 = ((PDXFENTPOINT)&DxfEntityData)->Point0;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Point);
			}
			break;

		case ENT_CIRCLE:
			{
				ZeroMemory(&Circle, sizeof(ENTCIRCLE));
				Circle.Point0 = ((PDXFENTCIRCLE)&DxfEntityData)->Point0;
				Circle.Radius = ((PDXFENTCIRCLE)&DxfEntityData)->Radius;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Circle);
			}
			break;

		case ENT_ARC:
			{
				ZeroMemory(&Arc, sizeof(ENTARC));
				Arc.Point0 = ((PDXFENTARC)&DxfEntityData)->Point0;
				Arc.Radius = ((PDXFENTARC)&DxfEntityData)->Radius;
				Arc.StartAngle = ((PDXFENTARC)&DxfEntityData)->StartAngle;
				Arc.EndAngle = ((PDXFENTARC)&DxfEntityData)->EndAngle;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Arc);
			}
			break;

		case ENT_ELLIPSE:
			{
				ZeroMemory(&Ellipse, sizeof(ENTELLIPSE));
				Ellipse.CenterPoint = ((PDXFENTELLIPSE)&DxfEntityData)->CenterPoint;
				Ellipse.MajorAxisEndPoint = ((PDXFENTELLIPSE)&DxfEntityData)->MajorAxisEndPoint;
				Ellipse.MinorToMajorRatio = ((PDXFENTELLIPSE)&DxfEntityData)->MinorToMajorRatio;
				Ellipse.StartParam = ((PDXFENTELLIPSE)&DxfEntityData)->StartParam;
				Ellipse.EndParam = ((PDXFENTELLIPSE)&DxfEntityData)->EndParam;
				Ellipse.ExtrudeDirectX = ((PDXFENTELLIPSE)&DxfEntityData)->ExtrudeDirectX;
				Ellipse.ExtrudeDirectY = ((PDXFENTELLIPSE)&DxfEntityData)->ExtrudeDirectY;
				Ellipse.ExtrudeDirectZ = ((PDXFENTELLIPSE)&DxfEntityData)->ExtrudeDirectZ;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Ellipse);
			}
			break;

		case ENT_TEXT:
			{
				ZeroMemory(&Text, sizeof(ENTTEXT));
				Text.Point0 = ((PDXFENTTEXT)&DxfEntityData)->Point0;
				wcscpy_s(Text.strText, ((PDXFENTTEXT)&DxfEntityData)->strText);
				Text.TextData.Height = ((PDXFENTTEXT)&DxfEntityData)->TextData.Height;
				Text.TextData.RotationAngle = ((PDXFENTTEXT)&DxfEntityData)->TextData.RotationAngle;
				Text.TextData.WidthFactor = ((PDXFENTTEXT)&DxfEntityData)->TextData.WidthFactor;
				Text.TextData.Oblique = ((PDXFENTTEXT)&DxfEntityData)->TextData.Oblique;
				Text.TextData.GenerationFlag = ((PDXFENTTEXT)&DxfEntityData)->TextData.GenerationFlag;
				Text.TextData.Justification = ((PDXFENTTEXT)&DxfEntityData)->TextData.Justification;
				Text.TextData.Point1 = ((PDXFENTTEXT)&DxfEntityData)->TextData.Point1;
				Text.TextData.ExtrudeDirect = ((PDXFENTTEXT)&DxfEntityData)->TextData.ExtrudeDirect;
				Text.TextRect = ((PDXFENTTEXT)&DxfEntityData)->TextRect;

				// Looking for text style name
				if(strlen(((PDXFENTTEXT)&DxfEntityData)->TextData.TextStyleName)>0)
				{
					strcpy_s(Style.Name, ((PDXFENTTEXT)&DxfEntityData)->TextData.TextStyleName);
					if(drwFindTableType_Direct(pDrawing, TAB_STYLE, FIND_BYNAME, &Style)>0)
						Text.TextData.TextStyleObjhandle = Style.Objhandle;
					else
					{
						// Style is unavailable
						// Set to default style
					}
				}
				else
				{
					// Set to default style
				}
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Text);
			}
			break;

		case ENT_MTEXT:
			{
				ZeroMemory(&MText, sizeof(ENTMTEXT));
				MText.Point0 = ((PDXFENTMTEXT)&DxfEntityData)->Point0;
				wcscpy_s(MText.strText, ((PDXFENTMTEXT)&DxfEntityData)->strText);
				memcpy(&MText.TextData, _PMTEXTDATA(&(((PDXFENTMTEXT)&DxfEntityData)->TextData)), sizeof(_MTEXTDATA));

				// Looking for text style name
				if(strlen(((PDXFENTMTEXT)&DxfEntityData)->TextStyleName)>0)
				{
					strcpy_s(Style.Name, ((PDXFENTMTEXT)&DxfEntityData)->TextStyleName);
					if(drwFindTableType_Direct(pDrawing, TAB_STYLE, FIND_BYNAME, &Style)>0)
						MText.TextStyleObjhandle = Style.Objhandle;
					else
					{
						// Style is unavailable
						// Set to default style
					}
				}
				else
				{
					// Set to default style
				}
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &MText);
			}
			break;

		case ENT_SOLID:
			{
				ZeroMemory(&Solid, sizeof(ENTSOLID));
				Solid.Point0 = ((PDXFENTSOLID)&DxfEntityData)->Point0;
				Solid.Point1 = ((PDXFENTSOLID)&DxfEntityData)->Point1;
				Solid.Point2 = ((PDXFENTSOLID)&DxfEntityData)->Point2;
				Solid.Point3 = ((PDXFENTSOLID)&DxfEntityData)->Point3;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Solid);
			}
			break;

		case ENT_INSERT:
			{
				ZeroMemory(&Insert, sizeof(ENTINSERT));
				//BLOCKHEADER	BlockHeader;
				Insert.Point0 = ((PDXFENTINSERT)&DxfEntityData)->Point0;
				Insert.XScale = ((PDXFENTINSERT)&DxfEntityData)->XScale;
				Insert.YScale = ((PDXFENTINSERT)&DxfEntityData)->YScale;
				Insert.ZScale = ((PDXFENTINSERT)&DxfEntityData)->ZScale;
				Insert.RotationAngle = ((PDXFENTINSERT)&DxfEntityData)->RotationAngle;
				Insert.StretchDirectZ = ((PDXFENTINSERT)&DxfEntityData)->StretchDirectZ;
				strcpy_s(Insert.BlockHeaderName, ((PDXFENTINSERT)&DxfEntityData)->BlockHeaderName);
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Insert);

				//strcpy_s(BlockHeader.Name, ((PDXFENTINSERT)&DxfEntityData)->BlockHeaderName);
				//if((Insert.BlockHeaderObjhandle = drwFindBlock_Direct(pDrawing, FIND_BYNAME, &BlockHeader))>0)
				//	drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Insert);
				//else
				//{
					// Block name not found
				//}
			}
			break;

		case ENT_POLYLINE:
			{
				ZeroMemory(&Polyline, sizeof(ENTPOLYLINE));
				Polyline.nVertex = ((PDXFENTPOLYLINE)&DxfEntityData)->nVertex;
				Polyline.pVertex = ((PENTPOLYLINE)((PDXFENTPOLYLINE)&DxfEntityData))->pVertex;
				Polyline.Flag = ((PDXFENTPOLYLINE)&DxfEntityData)->Flag;
				Polyline.Closed = ((PDXFENTPOLYLINE)&DxfEntityData)->Closed;
				Polyline.FixedWidth = ((PDXFENTPOLYLINE)&DxfEntityData)->FixedWidth;
				Polyline.IsMeshMClosed = ((PDXFENTPOLYLINE)&DxfEntityData)->IsMeshMClosed;
				Polyline.IsMeshNClosed = ((PDXFENTPOLYLINE)&DxfEntityData)->IsMeshNClosed;
				Polyline.nPolyPoints = ((PDXFENTPOLYLINE)&DxfEntityData)->nPolyPoints;
				Polyline.pPolyPoints = ((PDXFENTPOLYLINE)&DxfEntityData)->pPolyPoints;
				Polyline.MeshM = ((PDXFENTPOLYLINE)&DxfEntityData)->MeshM;
				Polyline.MeshN = ((PDXFENTPOLYLINE)&DxfEntityData)->MeshN;
				Polyline.nArrows = ((PDXFENTPOLYLINE)&DxfEntityData)->nArrows;
				Polyline.pArrows = ((PDXFENTPOLYLINE)&DxfEntityData)->pArrows;
				Polyline.IsPolygonMesh = ((PDXFENTPOLYLINE)&DxfEntityData)->IsPolygonMesh;
				Polyline.IsPolyFaceMesh = ((PDXFENTPOLYLINE)&DxfEntityData)->IsPolyFaceMesh;
				Polyline.VPoint = ((PDXFENTPOLYLINE)&DxfEntityData)->VPoint;
				Polyline.StartWidth = ((PDXFENTPOLYLINE)&DxfEntityData)->StartWidth;
				Polyline.EndWidth = ((PDXFENTPOLYLINE)&DxfEntityData)->EndWidth;
				Polyline.PolyFlag = ((PDXFENTPOLYLINE)&DxfEntityData)->PolyFlag;
				Polyline.nVertexIndexes = ((PDXFENTPOLYLINE)&DxfEntityData)->nVertexIndexes;
				Polyline.pPolyFaceVertexIndexes = ((PDXFENTPOLYLINE)&DxfEntityData)->pPolyFaceVertexIndexes;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Polyline);
			}
			break;

		case ENT_LWPOLYLINE:
			{
				ZeroMemory(&LWPolyline, sizeof(ENTLWPOLYLINE));
				LWPolyline.nVertex = ((PDXFENTLWPOLYLINE)&DxfEntityData)->nVertex;
				LWPolyline.pVertex = ((PENTLWPOLYLINE)((PDXFENTLWPOLYLINE)&DxfEntityData))->pVertex;
				LWPolyline.Flag = ((PDXFENTLWPOLYLINE)&DxfEntityData)->Flag;
				LWPolyline.Closed = ((PDXFENTLWPOLYLINE)&DxfEntityData)->Closed;
				LWPolyline.FixedWidth = ((PDXFENTLWPOLYLINE)&DxfEntityData)->FixedWidth;
				LWPolyline.nArrows = ((PDXFENTLWPOLYLINE)&DxfEntityData)->nArrows;
				LWPolyline.nPolyPoints = ((PDXFENTLWPOLYLINE)&DxfEntityData)->nPolyPoints;
				LWPolyline.pArrows = ((PDXFENTLWPOLYLINE)&DxfEntityData)->pArrows;
				LWPolyline.pPolyPoints = ((PDXFENTLWPOLYLINE)&DxfEntityData)->pPolyPoints;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &LWPolyline);
			}
			break;

		case ENT_MLINE:
			{
				ZeroMemory(&mline, sizeof(ENTMLINE));
				mline.pVetexPoint=((PDXFENTMLINE)&DxfEntityData)->pVetexPoint;
				mline.ParemNum=((PDXFENTMLINE)&DxfEntityData)->ParemNum;
				mline.Alige=((PDXFENTMLINE)&DxfEntityData)->Alige;
				mline.ScaleFacter=((PDXFENTMLINE)&DxfEntityData)->ScaleFacter;
				mline.Simble=((PDXFENTMLINE)&DxfEntityData)->Simble;
				mline.StartPoint=((PDXFENTMLINE)&DxfEntityData)->StartPoint;
				mline.VetexNum=((PDXFENTMLINE)&DxfEntityData)->VetexNum;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &mline);
			}	
			break;

		case ENT_SPLINE:
			{
				ZeroMemory(&Spline, sizeof(ENTSPLINE));
				Spline.ControlNodeNum=((PDXFENTSPLINE)&DxfEntityData)->ControlNodeNum;
				Spline.ControlTolerance=((PDXFENTSPLINE)&DxfEntityData)->ControlTolerance;
				Spline.EndTangent=((PDXFENTSPLINE)&DxfEntityData)->EndTangent;
				Spline.FittingPointNum=((PDXFENTSPLINE)&DxfEntityData)->FittingPointNum;
				Spline.FittingTolerance=((PDXFENTSPLINE)&DxfEntityData)->FittingTolerance;
				Spline.NodeNum=((PDXFENTSPLINE)&DxfEntityData)->NodeNum;
				Spline.NodeTolerance=((PDXFENTSPLINE)&DxfEntityData)->NodeTolerance;
				Spline.NormalVecetor=((PDXFENTSPLINE)&DxfEntityData)->NormalVecetor;
				Spline.SplineMarks=((PDXFENTSPLINE)&DxfEntityData)->SplineMarks;
				Spline.SplineOrder=((PDXFENTSPLINE)&DxfEntityData)->SplineOrder;
				Spline.StartTangent=((PDXFENTSPLINE)&DxfEntityData)->StartTangent;
				Spline.Weight=((PDXFENTSPLINE)&DxfEntityData)->Weight;
				Spline.pControlNode=((PDXFENTSPLINE)&DxfEntityData)->pControlNode;
				Spline.pFittingNode=((PDXFENTSPLINE)&DxfEntityData)->pFittingNode;
				Spline.pNodePoint=((PDXFENTSPLINE)&DxfEntityData)->pNodePoint;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Spline);
			}
			break;

		case ENT_3DFACE:
			{
				ZeroMemory(&Face3D, sizeof(ENT3DFACE));
				Face3D.Point0 = ((PDXFENT3DFACE)&DxfEntityData)->Point0;
				Face3D.Point1 = ((PDXFENT3DFACE)&DxfEntityData)->Point1;
				Face3D.Point2 = ((PDXFENT3DFACE)&DxfEntityData)->Point2;
				Face3D.Point3 = ((PDXFENT3DFACE)&DxfEntityData)->Point3;
				Face3D.FirstSideVisible = ((PDXFENT3DFACE)&DxfEntityData)->FirstSideVisible;
				Face3D.SecondSideVisible = ((PDXFENT3DFACE)&DxfEntityData)->SecondSideVisible;
				Face3D.ThirdSideVisible = ((PDXFENT3DFACE)&DxfEntityData)->ThirdSideVisible;
				Face3D.FourthSideVisible = ((PDXFENT3DFACE)&DxfEntityData)->FourthSideVisible;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Face3D);
			}
			break;

		case ENT_TRACE:
			{
				ZeroMemory(&Trace, sizeof(ENTTRACE));
				Trace.Point0 = ((PDXFENTTRACE)&DxfEntityData)->Point0;
				Trace.Point1 = ((PDXFENTTRACE)&DxfEntityData)->Point1;
				Trace.Point2 = ((PDXFENTTRACE)&DxfEntityData)->Point2;
				Trace.Point3 = ((PDXFENTTRACE)&DxfEntityData)->Point3;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Trace);
			}
			break;

		case ENT_LEADER:
			{
				ZeroMemory(&Leader, sizeof(ENTLEADER));
				Leader.nVertex = ((PDXFENTLEADER)&DxfEntityData)->nVertex;
				Leader.pVertex = ((PENTLEADER)((PDXFENTLEADER)&DxfEntityData))->pVertex;
				Leader.Flag = ((PDXFENTLEADER)&DxfEntityData)->Flag;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Leader);
			}
			break;

		case ENT_DIMENSION:
			{
				BLOCKHEADER	DimBlockHeader;
				ZeroMemory(&Dimension, sizeof(ENTDIMENSION));
				Dimension.DefPoint3 = ((PDXFENTDIMENSION)&DxfEntityData)->DefPoint3;
				Dimension.DefPoint4 = ((PDXFENTDIMENSION)&DxfEntityData)->DefPoint4;
				Dimension.DimRotationAngle = ((PDXFENTDIMENSION)&DxfEntityData)->DimRotationAngle;
				Dimension.DimLineDefPoint = ((PDXFENTDIMENSION)&DxfEntityData)->DimLineDefPoint;
				wcscpy_s(Dimension.DimText, ((PDXFENTDIMENSION)&DxfEntityData)->DimText);

				strcpy_s(DimBlockHeader.Name, ((PDXFENTDIMENSION)&DxfEntityData)->BlockName);
				Dimension.BlockheaderObjhandle = drwFindBlock_Direct(pDrawing, FIND_BYNAME, &DimBlockHeader);
				unsigned long n = atol(strcpy(buf,&((PDXFENTDIMENSION)&DxfEntityData)->BlockName[2]));
				if(n>pDrawing->LastDimBlkNum)
					pDrawing->LastDimBlkNum = n;

				// Looking for dim style name
				if(strlen(((PDXFENTDIMENSION)&DxfEntityData)->DimStyleName)>0)
				{
					strcpy_s(DimStyle.Name, ((PDXFENTDIMENSION)&DxfEntityData)->DimStyleName);
					if(drwFindTableType_Direct(pDrawing, TAB_DIMSTYLE, FIND_BYNAME, &DimStyle)>0)
						Dimension.DimStyleObjhandle= DimStyle.Objhandle;
					else
					{
						// DimStyle is unavailable
						// Set to default dim style
					}
				}
				else
				{
					// Set to default dim style
				}
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Dimension);
			}
			break;

		case ENT_HELIX:
			{
				ZeroMemory(&Helix, sizeof(ENTHELIX));
				Helix.ControlNodeNum=((PDXFENTHELIX)&DxfEntityData)->ControlNodeNum;
				Helix.ControlTolerance=((PDXFENTHELIX)&DxfEntityData)->ControlTolerance;
				Helix.EndTangent=((PDXFENTHELIX)&DxfEntityData)->EndTangent;
				Helix.FittingPointNum=((PDXFENTHELIX)&DxfEntityData)->FittingPointNum;
				Helix.FittingTolerance=((PDXFENTHELIX)&DxfEntityData)->FittingTolerance;
				Helix.NodeNum=((PDXFENTHELIX)&DxfEntityData)->NodeNum;
				Helix.NodeTolerance=((PDXFENTHELIX)&DxfEntityData)->NodeTolerance;
				Helix.NormalVecetor=((PDXFENTHELIX)&DxfEntityData)->NormalVecetor;
				Helix.SplineMarks=((PDXFENTHELIX)&DxfEntityData)->SplineMarks;
				Helix.SplineOrder=((PDXFENTHELIX)&DxfEntityData)->SplineOrder;
				Helix.StartTangent=((PDXFENTHELIX)&DxfEntityData)->StartTangent;
				Helix.Weight=((PDXFENTHELIX)&DxfEntityData)->Weight;
				Helix.pControlNode=((PDXFENTHELIX)&DxfEntityData)->pControlNode;
				Helix.pFittingNode=((PDXFENTHELIX)&DxfEntityData)->pFittingNode;
				Helix.pNodePoint=((PDXFENTHELIX)&DxfEntityData)->pNodePoint;
				Helix.AxisBasePoint=((PDXFENTHELIX)&DxfEntityData)->AxisBasePoint;
				Helix.Start=((PDXFENTHELIX)&DxfEntityData)->Start;
				memcpy(Helix.AxisVec, ((PDXFENTHELIX)&DxfEntityData)->AxisVec, sizeof(double) * 3);
				Helix.Radius=((PDXFENTHELIX)&DxfEntityData)->Radius;
				Helix.Turns=((PDXFENTHELIX)&DxfEntityData)->Turns;
				Helix.TurnHeight = ((PDXFENTHELIX)&DxfEntityData)->TurnHeight;
				Helix.Flag = ((PDXFENTHELIX)&DxfEntityData)->Flag;
				Helix.ConstraintType = ((PDXFENTHELIX)&DxfEntityData)->ConstraintType;
				Helix.pCoordPoint = ((PDXFENTHELIX)&DxfEntityData)->pCoordPoint;
				Helix.CoordCount = ((PDXFENTHELIX)&DxfEntityData)->CoordCount;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &Helix);
			}
			break;

		case ENT_ACADTABLE:
			{
				ZeroMemory(&AcadTable, sizeof(ENTACADTABLE));
				AcadTable.Point = ((PDXFENTACADTABLE)&DxfEntityData)->Point;
				strcpy_s(AcadTable.BlockHeaderName, ((PDXFENTACADTABLE)&DxfEntityData)->BlockHeaderName);
			    drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &AcadTable);
			}
			break;

		case ENT_WIPEOUT:
			{
				ZeroMemory(&WipeOut, sizeof(ENTWIPEOUT));
				WipeOut.Point = ((PDXFENTWIPEOUT)&DxfEntityData)->Point;
				WipeOut.UVector = ((PDXFENTWIPEOUT)&DxfEntityData)->UVector;
				WipeOut.VVector = ((PDXFENTWIPEOUT)&DxfEntityData)->VVector;
				WipeOut.UValue = ((PDXFENTWIPEOUT)&DxfEntityData)->UValue;
				WipeOut.VValue = ((PDXFENTWIPEOUT)&DxfEntityData)->VValue;
				WipeOut.Flag = ((PDXFENTWIPEOUT)&DxfEntityData)->Flag;
				WipeOut.IsUseClipping = ((PDXFENTWIPEOUT)&DxfEntityData)->IsUseClipping;
				WipeOut.Bright = ((PDXFENTWIPEOUT)&DxfEntityData)->Bright;
				WipeOut.Contrast = ((PDXFENTWIPEOUT)&DxfEntityData)->Contrast;
				WipeOut.FadeUnder = ((PDXFENTWIPEOUT)&DxfEntityData)->FadeUnder;
				WipeOut.ClippingBoundaryType = ((PDXFENTWIPEOUT)&DxfEntityData)->ClippingBoundaryType;
				WipeOut.nClip = ((PDXFENTWIPEOUT)&DxfEntityData)->nClip;
				WipeOut.pClipPoints = ((PDXFENTWIPEOUT)&DxfEntityData)->pClipPoints;
				WipeOut.Closed = ((PDXFENTWIPEOUT)&DxfEntityData)->Closed;
				WipeOut.nPolyPoints = ((PDXFENTWIPEOUT)&DxfEntityData)->nPolyPoints;
				WipeOut.pPolyPoints = ((PDXFENTWIPEOUT)&DxfEntityData)->pPolyPoints;
				drwAddEntity_Direct(pDrawing, BlockObjhandle, &EntityHeader, &WipeOut);
			}
			break;

		default:
			break;
		}

		NotifyLoadProgress(pDxf);
	}
}

/*-------------------------------------------------------------------*
 |  drwLoadDataFromFile                                              |
 |  Descriptions:                                                    |
 |      Loads drawing from a drawing data file (DXF file)            |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      OBJHANDLE BlockObjhandle = handle of block that entities     |
 |                     must be put in it. (NULL = Entity Section)    |
 |      DWORD Reserved = reserved, must be 0                         |
 |      LPCTSTR strFileName = input file name (Can not be NULL)      |
 |      HWND hWndProgress = handle of progress window                |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/  //从dxf文件中加载数据
BOOL drwLoadDataFromFile( HDRAWING hDrawing, OBJHANDLE BlockObjhandle, DWORD Reserved, const char *strFileName, HWND hWndProgress )
{
	PDRAWING	pDrawing;
	HDXF		hDxf; 
	PDXF		pDxf;

	DWORD		dwCurrentSection;
	DWORD		dwCurrentTableType;

	DXFDIMSTYLE	DxfDimStyle;//标注样式
	DXFLAYER	DxfLayer;//图层
	DXFLTYPE 	DxfLType;
	DXFSTYLE	    DxfStyle;
	DIMSTYLE	DimStyle;
	LAYER		Layer;
	LTYPE		LType;
	STYLE		Style;

	DXFHEADER  DxfHeader;
	//HEADER  Header;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return FALSE;

	if(strFileName==NULL)
	{
		// Filename can not be NULL
		GlobalUnlock(hDrawing);
		return FALSE;
	}

	// Open current dxf file
	hDxf = dxfOpenFile(strFileName);

	// Initialize pDxf ------------------
	if((pDxf = InitilizePDXF(hDxf))==NULL)
	{
		GlobalUnlock(hDrawing);
		return FALSE;
	}

	// Set Progress Percent to 0
	hProgWnd = hWndProgress;
	ProgPercent = 0;
	SendMessage(hWndProgress, WM_PROGRESS, ProgPercent, 0);

	dwCurrentSection = dxfFindNextSection(hDxf); //寻找Section的组码段
	while(dwCurrentSection!=SEC_NOTSET)
	{
		switch(dwCurrentSection)
		{
		case SEC_HEADER:// Header Section
			if(dxfReadHeaderData(hDxf, &DxfHeader))
			{

			}
			break;

		case SEC_TABLES:	// Tables Section
			do{
				if((dwCurrentTableType = dxfFindNextTableType(hDxf))==TAB_NOTSET)
					break;
				switch(dwCurrentTableType)
				{
				case TAB_DIMSTYLE:	// DimStyle TableType
					do{
						if(dxfReadTableType(hDxf, &DxfDimStyle)!=TAB_DIMSTYLE) //循环查找组码为0 value值为DIMSTYLE,并解析DIMSTYLE
							break;                                                                               //以ENDTAB结束为止

						ZeroMemory(&DimStyle, sizeof(DIMSTYLE));
						strcpy_s(DimStyle.Name, DxfDimStyle.Name);
						DimStyle.StandardFlags = DxfDimStyle.StandardFlags;
						DimStyle.dimasz = DxfDimStyle.DIMASZ;
						strcpy_s(DimStyle.dimblk1, DxfDimStyle.DIMBLK1);
						strcpy_s(DimStyle.dimblk2, DxfDimStyle.DIMBLK2);
						DimStyle.dimclrd = DxfDimStyle.DIMCLRD;
						DimStyle.dimclre = DxfDimStyle.DIMCLRE;
						DimStyle.dimclrt = DxfDimStyle.DIMCLRT;
						DimStyle.dimdle = DxfDimStyle.DIMDLE;
						DimStyle.dimexe = DxfDimStyle.DIMEXE;
						DimStyle.dimexo = DxfDimStyle.DIMEXO;
						DimStyle.dimgap = DxfDimStyle.DIMGAP;
						DimStyle.dimtxt = DxfDimStyle.DIMTXT;
						DimStyle.dimtad = DxfDimStyle.DIMTAD;

						strcpy_s(Style.Name, DxfDimStyle.DIMTXSTY);
						if(drwFindTableType_Direct(pDrawing, TAB_STYLE, FIND_BYNAME, &Style)>0)
							DimStyle.dimtxstyObjhandle = Style.Objhandle;
						else
						{
							// Text style is unavailable
							// Using default text style
						}

						drwAddTableType(hDrawing, TAB_DIMSTYLE, &DimStyle);
					} while(dxfGetCurrentTableType(hDxf)==TAB_DIMSTYLE);
					break;

				case TAB_LAYER:		// Layer TableType
					do{
						if(dxfReadTableType(hDxf, &DxfLayer)!=TAB_LAYER) //循环查找组码为0 value值为LAYER,并解析LAYER
							break;                                                                    //以ENDTAB结束为止

						ZeroMemory(&Layer, sizeof(LAYER));
						strcpy_s(Layer.Name, DxfLayer.Name);
						Layer.StandardFlags = DxfLayer.StandardFlags;
						Layer.Color = DxfLayer.Color;
//						Layer.LineWeight = DxfLayer.LineWeight;
//						Layer.PlotFlag = DxfLayer.PlotFlag;
//						Layer.PlotStyleObjhandle;

						strcpy_s(LType.Name, DxfLayer.LineType);
						if(drwFindTableType_Direct(pDrawing, TAB_LTYPE, FIND_BYNAME, &LType)>0)
							Layer.LineTypeObjhandle = LType.Objhandle;
						else
						{
							// Line type is unavailable
							dxfCloseFile(hDxf);
							GlobalUnlock(hDrawing);
							return FALSE;
						}

						drwAddTableType(hDrawing, TAB_LAYER, &Layer);
					} while(dxfGetCurrentTableType(hDxf)==TAB_LAYER);
					break;

				case TAB_LTYPE:		// LType TableType
					do{
						if(dxfReadTableType(hDxf, &DxfLType)!=TAB_LTYPE) //循环查找组码为0 value值为LTYPE,并解析LTYPE
							break;                                                                    //以ENDTAB结束为止

						ZeroMemory(&LType, sizeof(LTYPE));
						strcpy_s(LType.Name, DxfLType.Name);
						LType.StandardFlags = DxfLType.StandardFlags;
						strcpy_s(LType.DescriptiveText, DxfLType.DescriptiveText);
						LType.ElementsNumber = DxfLType.ElementsNumber;
						memcpy(LType.Elements, DxfLType.Elements, sizeof(double)*30);			// Line Elements (Max=30)
						LType.PatternLength = DxfLType.TotalPatternLength;

						drwAddTableType(hDrawing, TAB_LTYPE, &LType);
					} while(dxfGetCurrentTableType(hDxf)==TAB_LTYPE);
					break;

				case TAB_STYLE:		// Style TableType
					do{
						if(dxfReadTableType(hDxf, &DxfStyle)!=TAB_STYLE) //循环查找组码为0 value值为STYLE,并解析STYLE
							break;                                                                   //以ENDTAB结束为止

						ZeroMemory(&Style, sizeof(STYLE));
						strcpy_s(Style.Name, DxfStyle.Name);
						Style.StandardFlags = DxfStyle.StandardFlags;
						Style.TextGenerationFlags = DxfStyle.TextGenerationFlags;
						strcpy_s(Style.PrimaryFontFilename, DxfStyle.PrimaryFontFilename);
//						strcpy(Style.BigFontFilename, DxfStyle.BigFontFilename);
						Style.ObliqueAngle = DxfStyle.ObliqueAngle;
						Style.FixedTextHeight = DxfStyle.FixedTextHeight;
						Style.WidthFactor = DxfStyle.WidthFactor;
						Style.LastHeightUsed = DxfStyle.Height;
						strcpy_s(Style.FaceName, DxfStyle.FaceName);

						drwAddTableType(hDrawing, TAB_STYLE, &Style);
					} while(dxfGetCurrentTableType(hDxf)==TAB_STYLE);
					break;
				}

			} while(dxfGetCurrentSection(hDxf)==SEC_TABLES);
			break;

		case SEC_BLOCKS:	// Blocks Section
			DXFBLOCKHEADER	DxfBlockHeader;
			BLOCKHEADER		BlockHeader;
			OBJHANDLE		BlockObjhandle1;

			do{
				if(dxfFindBlockHeader(hDxf, &DxfBlockHeader)) 
				{
					// Adding new block
					ZeroMemory(&BlockHeader, sizeof(BLOCKHEADER));
					strcpy_s(BlockHeader.Name, DxfBlockHeader.Name);
					BlockHeader.Flags = DxfBlockHeader.Flags;
					BlockHeader.BasePoint = DxfBlockHeader.BasePoint;

					strcpy_s(Layer.Name, DxfBlockHeader.LayerName);
					if(drwFindTableType_Direct(pDrawing, TAB_LAYER, FIND_BYNAME, &Layer)>0)
						BlockHeader.LayerObjhandle = Layer.Objhandle;
					else
					{
						// Layer is unavailable
						// Using default layer "0"
					}

					BlockObjhandle1 = drwAddBlock_Direct(pDrawing, &BlockHeader);

					// Add new block's entities data
					ReadEntitiesDataFromFile(pDrawing, pDxf, BlockObjhandle1);
				}
				else
					break;
			} while(dxfGetCurrentSection(hDxf)==SEC_BLOCKS);
			break;

		case SEC_ENTITIES:	// Entities Section
			ReadEntitiesDataFromFile(pDrawing, pDxf, BlockObjhandle);
			break;
		}

		dwCurrentSection = dxfFindNextSection(hDxf);
	}

	// Set Progress Percent to 100
	ProgPercent = 100;
	SendMessage(hWndProgress, WM_PROGRESS, ProgPercent, 0);

	// UnInitilize hDxf -----------------
	UnInitilizePDXF(hDxf);

	// Close dxf file
	if(!dxfCloseFile(hDxf))
		return FALSE;

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}
