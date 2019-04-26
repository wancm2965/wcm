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
|  DREntities.cpp                                                     |
\*-------------------------------------------------------------------*/

#include "stdafx.h"
#include "DREntities.h"
#include "..\Base\SimpleList.h"
#include "..\..\Interface\VC\Operator.h"

static int  GCode = 0;
static char  strValue[2048] = {0};
static const  int  NumberOfPartsInLine = 2;
static const  int  NumberOfPartsInCircle = 47;
static const  int  NumberOfPartsInSpline = 24;

/*-------------------------------------------------------------------*
 |  ReadLineData                                                     |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTLINE pLine = pointer to Line structure                |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadLineData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTLINE pLine )
{
	ZeroMemory(pLine, sizeof(DXFENTLINE));
	pEntityHeader->EntityType = ENT_LINE;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 10:	// Start point X
			pLine->Point0.x = atof(strValue);
			break;
		case 20:	// Start point Y
			pLine->Point0.y = atof(strValue);
			break;
		case 30:	// Start point Z
			pLine->Point0.z = atof(strValue);
			break;
		case 11:	// End point X
			pLine->Point1.x = atof(strValue);
			break;
		case 21:	// End point Y
			pLine->Point1.y = atof(strValue);
			break;
		case 31:	// End point Z
			pLine->Point1.z = atof(strValue);
			break;
 		}

		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadPointData                                                    |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTPOINT pPoint = pointer to Point structure             |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadPointData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTPOINT pPoint )
{
	ZeroMemory(pPoint, sizeof(DXFENTPOINT));
	pEntityHeader->EntityType = ENT_POINT;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 10:	// Point location X
			pPoint->Point0.x = atof(strValue);
			break;
		case 20:	// Point location Y
			pPoint->Point0.y = atof(strValue);
			break;
		case 30:	// Point location Z
			pPoint->Point0.z = atof(strValue);
			break;
 		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}
	
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadCircleData                                                   |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTCIRCLE pPoint = pointer to Circle structure           |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadCircleData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTCIRCLE pCircle )
{
	ZeroMemory(pCircle, sizeof(DXFENTCIRCLE));
	pEntityHeader->EntityType = ENT_CIRCLE;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:	  // Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 10:	// Center point X
			pCircle->Point0.x = atof(strValue);
			break;
		case 20:	// Center point Y
			pCircle->Point0.y = atof(strValue);
			break;
		case 30:	// Center point Z
			pCircle->Point0.z = atof(strValue);
			break;
		case 40:	// Radius
			pCircle->Radius = atof(strValue);
			break;
 		}
	
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}
	
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadArcData                                                      |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTARC pArc = pointer to Arc structure                   |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadArcData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTARC pArc )
{
	ZeroMemory(pArc, sizeof(DXFENTARC));
	pEntityHeader->EntityType = ENT_ARC;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 10:	// Center point X
			pArc->Point0.x = atof(strValue);
			break;
		case 20:	// Center point Y
			pArc->Point0.y = atof(strValue);
			break;
		case 30:	// Center point Z
			pArc->Point0.z = atof(strValue);
			break;
		case 40:	// Radius
			pArc->Radius = atof(strValue);
			break;
		case 50:	// Start angle
			pArc->StartAngle = atof(strValue);
			break;
		case 51:	// End angle
			pArc->EndAngle = atof(strValue);
			break;
 		}

		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}
	
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadEllipseData                                                  |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTCIRCLE pPoint = pointer to Circle structure           |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadEllipseData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTELLIPSE pEllipse )
{
	ZeroMemory(pEllipse, sizeof(DXFENTELLIPSE));
	pEntityHeader->EntityType = ENT_ELLIPSE;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 10:	// Center point X
			pEllipse->CenterPoint.x = atof(strValue);
			break;
		case 20:	// Center point Y
			pEllipse->CenterPoint.y = atof(strValue);
			break;
		case 30:	// Center point Z
			pEllipse->CenterPoint.z = atof(strValue);
			break;
		case 11:	// Endpoint of major axis X
			pEllipse->MajorAxisEndPoint.x = atof(strValue);
			break;
		case 21:	// Endpoint of major axis Y
			pEllipse->MajorAxisEndPoint.y = atof(strValue);
			break;
		case 31:	// Endpoint of major axis Z
			pEllipse->MajorAxisEndPoint.z = atof(strValue);
			break;
		case 40:	// Ratio of minor axis to major axis
			pEllipse->MinorToMajorRatio = atof(strValue);
			break;
		case 41:	// Start parameter (this value is 0.0 for a full ellipse)
			pEllipse->StartParam = atof(strValue);
			break;
		case 42:	// End parameter (this value is 2pi for a full ellipse)
			pEllipse->EndParam = atof(strValue);
			break;
		case 210:
			pEllipse->ExtrudeDirectX = atof(strValue);
			break;
		case 220:
			pEllipse->ExtrudeDirectY = atof(strValue);
			break;
		case 230:
			pEllipse->ExtrudeDirectZ = atof(strValue);
			break;
 		}
	
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}
	
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadTextData  and  ReadMTextData                                 |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTTEXT pText = pointer to Text structure                |
 |      PDXFENTTMEXT pText = pointer to MText structure              |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
void ReadTextRect(PDXFENTTEXT pText)
{
	double TextHeight = pText->TextData.Height;
	REALPOINT pTopLeft, pBottomRight;

	//旋转、颠倒、反向
	bool  backward = ((pText->TextData.GenerationFlag & 2) != 0);
	bool  upsidedown = ((pText->TextData.GenerationFlag & 4) != 0);

	//获取文字长度
	int Len = wcslen(pText->strText);
	double TextWidth = Len * TextHeight /** 0.8f*/ * pText->TextData.WidthFactor;

	REALPOINT pPoint; //旋转前坐标
	ZeroMemory(&pPoint, sizeof(REALPOINT));
	PointRotationPoint(pText->TextData.Point1, pText->TextData.RotationAngle, pText->Point0, pPoint);  

	switch(pText->TextData.Justification)
	{
	//左边对正
	case TJ_LEFT:
		{
			pText->TextData.Point1 = pText->Point0;
		}
	case TJ_TOPLEFT:
	case TJ_MIDDLELEFT:
	case TJ_BOTTOMLEFT:
		{
			pTopLeft.x = (backward == true ? pText->Point0.x - TextWidth : pText->Point0.x);
			pTopLeft.y = (upsidedown == true ? pText->Point0.y : pText->Point0.y + TextHeight);
			pBottomRight.x = (backward == true ? pText->Point0.x : pText->Point0.x + TextWidth);
			pBottomRight.y = (upsidedown == true ? pText->Point0.y - TextHeight : pText->Point0.y);
		}
		break;
	//居中对正
	case TJ_CENTER:
	case TJ_MIDDLE:
	case TJ_TOPCENTER:
	case TJ_MIDDLECENTER:
	case TJ_BOTTOMCENTER:
		{
			double dx = fabs(pText->TextData.Point1.x - pPoint.x);
			pTopLeft.x = (backward == true ? pPoint.x - 2 * dx : pPoint.x);
			pTopLeft.y = (upsidedown == true ? pPoint.y : pPoint.y + TextHeight);
			pBottomRight.x = (backward == true ? pPoint.x : pPoint.x + 2 * dx);
			pBottomRight.y = (upsidedown == true ? pPoint.y - TextHeight : pPoint.y);
		}
		break;
	//右边对正
	case TJ_RIGHT:
	case TJ_ALIGNED:
	case TJ_FIT:
	case TJ_TOPRIGHT:
	case TJ_MIDDLERIGHT:
	case TJ_BOTTOMRIGHT:
		{
			double dx = fabs(pText->TextData.Point1.x - pPoint.x);
			pTopLeft.x = (backward == true ? pPoint.x - dx : pPoint.x);
			pTopLeft.y = (upsidedown == true ? pPoint.y : pPoint.y + TextHeight);
			pBottomRight.x = (backward == true ? pPoint.x : pPoint.x + dx);
			pBottomRight.y = (upsidedown == true ? pPoint.y - TextHeight : pPoint.y);
		}
		break;
	//缺省情况
	default:
		{
			pTopLeft = pText->TextData.Point1;
			pBottomRight = pText->Point0;
		}
		break;
	}

	//旋转后坐标
	REALPOINT p; //旋转前坐标
	ZeroMemory(&p, sizeof(REALPOINT));
	PointRotationPoint(pText->TextData.Point1, -pText->TextData.RotationAngle, pTopLeft, p);
	pText->TextRect.left = p.x;
	pText->TextRect.top = p.y;
	PointRotationPoint(pText->TextData.Point1, -pText->TextData.RotationAngle, pBottomRight, p);
	pText->TextRect.right = p.x;
	pText->TextRect.bottom = p.y;
}

BOOL ReadTextData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTTEXT pText )
{
	ZeroMemory(pText, sizeof(DXFENTTEXT));
	pEntityHeader->EntityType = ENT_TEXT;
	pText->TextData.WidthFactor = 1.0f;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 7:		// Text Style Name
			strcpy_s(pText->TextData.TextStyleName, strValue);
			break;
		case 10:	// First alignment point (in OCS) X
			pText->Point0.x = atof(strValue);
			break;
		case 20:	// First alignment point (in OCS) Y
			pText->Point0.y = atof(strValue);
			break;
		case 30:	// First alignment point (in OCS) Z
			pText->Point0.z = atof(strValue);
			break;
		case 72:	// Horizontal text justification type (default = 0)
			pText->TextData.Justification = (pText->TextData.Justification & 0xff00) | (char)atoi(strValue);
			break;
		case 73:	// Vertical text justification type (default = 0)
			pText->TextData.Justification = (pText->TextData.Justification & 0x00ff) | (char)atoi(strValue) << 8;
			break;
		case 11:	// Second alignment point (in OCS) X
			pText->TextData.Point1.x = atof(strValue);
			break;
		case 21:	// Second alignment point (in OCS) Y
			pText->TextData.Point1.y = atof(strValue);
			break;
		case 31:	// Second alignment point (in OCS) Z
			pText->TextData.Point1.z = atof(strValue);
			break;
		case 40:	// Text Height
			pText->TextData.Height = atof(strValue);
			break;
		case 41:	// Width Factor
			pText->TextData.WidthFactor = atof(strValue);
			break;
		case 50:	// Text Rotation Angle
			pText->TextData.RotationAngle = atof(strValue);
			break;
		case 1:		// Default value (the string itself)
			{
				int dwMinSize = 0;
				dwMinSize = MultiByteToWideChar (CP_UTF8, 0, strValue, -1, NULL, 0);
				MultiByteToWideChar (CP_UTF8, 0, strValue, -1, pText->strText, dwMinSize); 
			}
			break;
		case 51:   //Oblique angle
			pText->TextData.Oblique = atof(strValue);
			break;
		case 71:   //Generation Flag
			pText->TextData.GenerationFlag = atoi(strValue);
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	//获取文字区域
	ReadTextRect(pText);

	return TRUE;
}

BOOL ReadMTextData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTMTEXT pText)
{
	ZeroMemory(pText, sizeof(DXFENTMTEXT));
	pEntityHeader->EntityType = ENT_MTEXT;
	pText->TextData.Justification = 1;
	pText->TextData.ExtrudeDirect.z = 1;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 48: // Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 7:  // Text Style Name
			strcpy_s(pText->TextStyleName, strValue);
			break;
		case 10:	// First alignment point (in OCS) X
			pText->Point0.x = atof(strValue);
			break;
		case 20:	// First alignment point (in OCS) Y
			pText->Point0.y = atof(strValue);
			break;
		case 30:	// First alignment point (in OCS) Z
			pText->Point0.z = atof(strValue);
			break;
		case 71:	// Horizontal text justification type (default = 0)
			pText->TextData.Justification = atoi(strValue);
			break;
		case 72: // Text Direction
			pText->TextData.TextDir = atoi(strValue);
			break;
		case 11:	// Second alignment point (in OCS) X
			pText->TextData.AngleVecX = atof(strValue);
			break;
		case 21:	// Second alignment point (in OCS) Y
			pText->TextData.AngleVecY = atof(strValue);
			break;
		case 31:	// Second alignment point (in OCS) Z
			pText->TextData.AngleVecZ = atof(strValue);
			break;
		case 40:	// Text Height
			pText->TextData.Height = atof(strValue);
			break;
		case 41:	// Ref Rectangle Width 
			pText->TextData.RefRectWidth = atof(strValue);
			break;
		case 50:	// Text Rotation Angle
			pText->TextData.RotationAngle = atof(strValue);
			break;
		case 73: // Text LineSpace Style
			pText->TextData.LineSpaceStyle = atoi(strValue);
			break;
		case 44: // Text LineSpace Scale
			pText->TextData.LineSpaceScale = atof(strValue);
			break;
		case 90: // Fill Background Flag
			pText->TextData.FillBkFlag = atoi(strValue);
			break;
		case 63: // Background Color Number
			pText->TextData.ColorIndex = atoi(strValue);
			break;
		case 45: // Fill Rect Size
			pText->TextData.FillRectScale = atof(strValue);
			break;
		case 3: // Attach String
			{
				WCHAR str[256] = L"";
				//str[0] = 0; //代码效率高
				int dwSize = MultiByteToWideChar (CP_UTF8, 0, strValue, -1, NULL, 0);
				MultiByteToWideChar (CP_UTF8, 0, strValue, -1, str, dwSize);
				wcscat_s(pText->strText, str);
			}
			break;
		case 1: // Default value (the string itself)
			{
				WCHAR str[256] = L"";
				//str[0] = 0; //代码效率高
				int dwMinSize = MultiByteToWideChar (CP_UTF8, 0, strValue, -1, NULL, 0);
				MultiByteToWideChar (CP_UTF8, 0, strValue, -1, str, dwMinSize); 
				wcscat_s(pText->strText, str);
			}
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadSolidData                                                    |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTSOLID pSolid = pointer to Solid structure             |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadSolidData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTSOLID pSolid )
{
	ZeroMemory(pSolid, sizeof(DXFENTSOLID));
	pEntityHeader->EntityType = ENT_SOLID;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 10:	// First corner X
			pSolid->Point0.x = atof(strValue);
			break;
		case 20:	// First corner Y
			pSolid->Point0.y = atof(strValue);
			break;
		case 30:	// First corner Z
			pSolid->Point0.z = atof(strValue);
			break;
		case 11:	// Second corner X
			pSolid->Point1.x = atof(strValue);
			break;
		case 21:	// Second corner Y
			pSolid->Point1.y = atof(strValue);
			break;
		case 31:	// Second corner Z
			pSolid->Point1.z = atof(strValue);
			break;
		case 12:	// Third corner X
			pSolid->Point2.x = atof(strValue);
			break;
		case 22:	// Third corner Y
			pSolid->Point2.y = atof(strValue);
			break;
		case 32:	// Third corner Z
			pSolid->Point2.z = atof(strValue);
			break;
		case 13:	// Fourth corner X
			pSolid->Point3.x = atof(strValue);
			break;
		case 23:	// Fourth corner Y
			pSolid->Point3.y = atof(strValue);
			break;
		case 33:	// Fourth corner Z
			pSolid->Point3.z = atof(strValue);
			break;
 		}

		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}
	
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadInsertData                                                   |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTINSERT pInsert = pointer to Insert structure          |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadInsertData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTINSERT pInsert )
{
	ZeroMemory(pInsert, sizeof(DXFENTINSERT));
	pEntityHeader->EntityType = ENT_INSERT;
	pInsert->XScale = 1.0f;
	pInsert->YScale = 1.0f;
	pInsert->ZScale = 1.0f;
	pInsert->StretchDirectZ = 1.0f;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 2:		// Block Name
			strcpy_s(pInsert->BlockHeaderName, strValue);
			break;
		case 10:	// Insertion point X
			pInsert->Point0.x = atof(strValue);
			break;
		case 20:	// Insertion point Y
			pInsert->Point0.y = atof(strValue);
			break;
		case 30:	// Insertion point Z
			pInsert->Point0.z = atof(strValue);
			break;
		case 41:	// X scale factor (optional; default = 1)
			pInsert->XScale = atof(strValue);
			break;
		case 42:	// Y scale factor (optional; default = 1)
			pInsert->YScale = atof(strValue);
			break;
		case 43:	// Z scale factor (optional; default = 1)
			pInsert->ZScale = atof(strValue);
			break;
		case 50:	// Rotation angle (optional; default = 0)
			pInsert->RotationAngle = atof(strValue);
			break;
		case 230:	// Z stretch direct (optional; default = 1)
			pInsert->StretchDirectZ = atof(strValue);
			break;
 		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadPolyLineData and ReadLWPolyLineData                          |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTPOLYLINE pPolyLine = pointer to PolyLine structure    |
 |      PDXFLWENTPOLYLINE pPolyLine = pointer to LWPolyLine structure|
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
enum IntersectionMode
{
	none = 0,
	point,
	line,
	plane
};

//读取PolyLine操作
static BOOL IsFlagValueInt16(unsigned int flag)
{
	return (flag & 16) != 0;
}

static BOOL IsFlagValueInt128(unsigned int flag)
{
	return flag == 128;
}

BOOL IsFlagValue(const DXFENTVERTEX &Vertex, const DXFENTVERTEX &Vertex2, const DXFENTVERTEX &Vertex3)
{
	DXFENTVERTEX Vertex4;
	ZeroMemory(&Vertex4, sizeof(DXFENTVERTEX));
	BOOL flag = EqualPoint(Vertex4.Point, Vertex3.Point);
	if(!flag)
	{
		double num1 = Atan2ToRad(Vertex.Point.x, Vertex.Point.y, Vertex3.Point.x, Vertex3.Point.y);
		double num2 = Atan2ToRad(Vertex.Point.x, Vertex.Point.y, Vertex2.Point.x, Vertex2.Point.y);
		double num3 = (num1 - num2) * 180.0 / PI;
		if (abs(num3) >= 180.0)
		{
			num3 = abs(num3) - 180.0;
		}
		num3 = abs(num3);
		flag = ((num3 < 29.0) | (num3 > 170.0));
	}
	return flag;
}

void GetRatio(const REALPOINT &Point, const REALPOINT &Point2, double &num, double &num2, double &num3)
{
	num = Point2.x - Point.x;
	num2 = Point2.y - Point.y;
	num3 = sqrt(num * num + num2 * num2);
	if (num3 != 0.0f)
	{
		num /= num3;
		num2 /= num3;
	}
}

int GetNum(PDXFENTPOLYLINEBASE pPolyLine, const REALPOINT &Point, int num)
{
	assert(pPolyLine->pPolyPoints != NULL);
	int num2 = num;
	int count = pPolyLine->nPolyPoints;
	while (num2 < count && (!EqualDouble(Point.x, pPolyLine->pPolyPoints[num2].x) 
		|| !EqualDouble(Point.y, pPolyLine->pPolyPoints[num2].y)))
	{
		num2++;
	}
	if (num2 == count)
	{
		num2 = -1;
	}
	return num2;
}

REALPOINT GetRealPointResult(const REALPOINT &Point, const REALPOINT &Point2, double num)
{
	REALPOINT Point3, result;
	result.x = (Point.x + Point2.x) / 2.0f;
	result.y = (Point.y + Point2.y) / 2.0f;
	result.z = Point.z;
	Point3.z = (Point2.z - result.z) / 15.0f;
	Point3.x = Point.x - result.x;
	Point3.y = Point.y - result.y;
	double num2 = (1.0f - num * num) / 2.0f / num;
	result.x += num2 * Point3.y;
	result.y -= num2 * Point3.x;
	return result;
}

REALPOINT GetRealPointResult2(const REALPOINT &Point, const REALPOINT &Point2, int num)
{
	REALPOINT item;
	item.x = Point.x + (double)num * Point2.x;
	item.y = Point.y + (double)num * Point2.y;
	item.z = Point.z;
	return item;
}

REALPOINT GetRealPointResult3(const REALPOINT &dPoint, double num, double num2, double num3)
{
	num /= 2.0f;
	REALPOINT result;
	result.x = dPoint.x - num3 * num;
	result.y = dPoint.y + num2 * num;
	result.z = dPoint.z;
	return result;
}

REALPOINT GetPolyPointsIndex(PDXFENTPOLYLINEBASE pPolyLine, int num)
{
	assert(pPolyLine->pPolyPoints != NULL);
	if (num < pPolyLine->nPolyPoints)
	{
		return pPolyLine->pPolyPoints[num];
	}
	return pPolyLine->pPolyPoints[0];
}

void GetVertexWidth(double StartWidth, double EndWidth, double FixedWidth, double &ptr, double &ptr2)
{
	if (FixedWidth == 0.0f)
	{
		ptr = StartWidth;
		ptr2 = EndWidth;
		return;
	}
	ptr = FixedWidth;
	ptr2 = FixedWidth;
}

BOOL EqualVertexWidth(double StartWidth, double EndWidth, double FixedWidth)
{
	return (FixedWidth == 0.0f && StartWidth == 0.0f && EndWidth == 0.0f);
}

//读取PolyLine点坐标
DXFENTVERTEX GetVertex(PDXFENTPOLYLINEBASE pPolyLine, const DXFENTVERTEX &Vertex, int num)
{
	DXFENTVERTEX Vertex2;
	ZeroMemory(&Vertex2, sizeof(DXFENTVERTEX));
	if (num > 0)
	{
		DXFENTVERTEX Vertex3 = pPolyLine->pVertex[num - 1];
		if (Vertex3.Bulge == 0.0 && !EqualVertexWidth(Vertex3.StartWidth, Vertex3.EndWidth, pPolyLine->FixedWidth))
		{
			Vertex2 = pPolyLine->pVertex[num - 1];
		}
	}
	else
	{
		DXFENTVERTEX Vertex3 = pPolyLine->pVertex[pPolyLine->nVertex - 1];
		if (pPolyLine->Closed && Vertex3.Bulge == 0.0 && !EqualVertexWidth(Vertex3.StartWidth, Vertex3.EndWidth, pPolyLine->FixedWidth))
		{
			Vertex2 = pPolyLine->pVertex[pPolyLine->nVertex - 1];
		}
	}
	if (EqualPoint(Vertex.Point, Vertex2.Point))
	{
		ZeroMemory(&Vertex2, sizeof(DXFENTVERTEX));
	}
	return Vertex2;
}

DXFENTVERTEX GetVertex2(PDXFENTPOLYLINEBASE pPolyLine, const DXFENTVERTEX &Vertex, int num)
{
	DXFENTVERTEX Vertex2;
	ZeroMemory(&Vertex2, sizeof(DXFENTVERTEX));
	if (Vertex.Bulge == 0.0 && !EqualVertexWidth(Vertex.StartWidth, Vertex.EndWidth, pPolyLine->FixedWidth))
	{
		if (num < pPolyLine->nVertex - 2)
		{
			Vertex2 = pPolyLine->pVertex[num + 2];
		}
		else
		{
			if (pPolyLine->Closed)
			{
				if (num == pPolyLine->nVertex - 1)
				{
					Vertex2 = pPolyLine->pVertex[1];
				}
				if (num == pPolyLine->nVertex - 2)
				{
					Vertex2 = pPolyLine->pVertex[0];
				}
			}
		}
		if (EqualPoint(Vertex.Point, Vertex2.Point))
		{
			ZeroMemory(&Vertex2, sizeof(DXFENTVERTEX));
		}
	}
	return Vertex2;
}

void ReadPolyFaceVertexIndex(PDXFENTPOLYLINE pPolyLine, int index)
{
	static int num = 0;

	int *pNew = (int *)realloc(pPolyLine->pPolyFaceVertexIndexes, (num + 4)* sizeof(int));
	if (pNew == NULL)
	{
		if (pPolyLine->pPolyFaceVertexIndexes != NULL)
		{
			free(pPolyLine->pPolyFaceVertexIndexes);
			pPolyLine->pPolyFaceVertexIndexes = NULL;
			pPolyLine->nVertexIndexes = 0;
		}
		num = 0;
		return;
	}

	pPolyLine->pPolyFaceVertexIndexes = pNew;
	pPolyLine->pPolyFaceVertexIndexes[num] = pPolyLine->pVertex[index].PolyFaceVertexIndex1;
	pPolyLine->pPolyFaceVertexIndexes[num+1] = pPolyLine->pVertex[index].PolyFaceVertexIndex2;
	pPolyLine->pPolyFaceVertexIndexes[num+2] = pPolyLine->pVertex[index].PolyFaceVertexIndex3;
	pPolyLine->pPolyFaceVertexIndexes[num+3] = pPolyLine->pVertex[index].PolyFaceVertexIndex4;
	pPolyLine->nVertexIndexes = num + 4;
	num += 4;
}

REALPOINT ReadPolyPoint(const DPOINT &dPoint, const DPOINT &dPoint2, IntersectionMode &intersectionMode)
{
	REALPOINT empty;
	ZeroMemory(&empty, sizeof(REALPOINT));
	intersectionMode = point;
	if (EqualDouble(dPoint.Point2.x, dPoint.Point.x))
	{
		empty.x = dPoint.Point2.x;
		if (!EqualDouble(dPoint2.Point2.x, dPoint2.Point.x))
		{
			double num = (dPoint2.Point2.y - dPoint2.Point.y) / (dPoint2.Point2.x - dPoint2.Point.x);
			double num2 = dPoint2.Point.y - dPoint2.Point.x * num;
			empty.y = num2 + empty.x * num;
		}
		else
		{
			if (EqualDouble(dPoint.Point.x, dPoint2.Point.x))
			{
				intersectionMode = line;
			} 
			else
			{
				intersectionMode = none;
			}
		}
		empty.z = dPoint.Point2.z;
	}
	else
	{
		if(EqualDouble(dPoint2.Point2.x, dPoint2.Point.x))
		{
			empty.x = dPoint2.Point.x;
			if (!EqualDouble(dPoint.Point2.x, dPoint.Point.x))
			{
				double num3 = (dPoint.Point2.y - dPoint.Point.y) / (dPoint.Point2.x - dPoint.Point.x);
				double num4 = dPoint.Point.y - dPoint.Point.x * num3;
				empty.y = num4 + empty.x * num3;
			} 
			else
			{
				if (EqualDouble(dPoint.Point.x, dPoint2.Point.x))
				{
					intersectionMode = line;
				} 
				else
				{
					intersectionMode = none;
				}
			}
			empty.z = dPoint2.Point.z;
		}
		else
		{
			double num = (dPoint2.Point2.y - dPoint2.Point.y) / (dPoint2.Point2.x - dPoint2.Point.x);
			double num2 = dPoint2.Point.y - dPoint2.Point.x * num;
			double num3 = (dPoint.Point2.y - dPoint.Point.y) / (dPoint.Point2.x - dPoint.Point.x);
			double num4 = dPoint.Point.y - dPoint.Point.x * num3;
			if (!EqualDouble(num3, num))
			{
				empty.x = (num2 - num4) / (num3 - num);
				empty.y = num3 * empty.x + num4;
			} 
			else
			{
				if (EqualDouble(num4, num2))
				{
					intersectionMode = line;
				}
				else
				{
					intersectionMode = none;
				}
			}
		}
	}
	return empty;
}

void ReadPolyDPoint(
	const DXFENTVERTEX &Vertex, 
	const DXFENTVERTEX &Vertex2, 
	double &ptr, 
	double &ptr2, 
	double &ptr3, 
	double &ptr4, 
	double &ptr5, 
	DPOINT &dPoint, 
	DPOINT &dPoint2, 
	double FixedWidth)
{
	GetVertexWidth(Vertex.StartWidth, Vertex.EndWidth, FixedWidth, ptr4, ptr5);
	GetRatio(Vertex.Point, Vertex2.Point, ptr, ptr2, ptr3);
	dPoint.Point = GetRealPointResult3(Vertex.Point, ptr4, ptr, ptr2);
	dPoint.Point2 = GetRealPointResult3(Vertex2.Point, ptr5, ptr, ptr2);
	dPoint2.Point = GetRealPointResult3(Vertex.Point, -ptr4, ptr, ptr2);
	dPoint2.Point2 = GetRealPointResult3(Vertex2.Point, -ptr5, ptr, ptr2);
}

void ReadPolyArrowsPointNoBulge(
	PDXFENTPOLYLINEBASE pPolyLine, 
	SimpleList <REALPOINT> &RealPointList, 
	const DXFENTVERTEX &Vertex, 
	const DXFENTVERTEX &Vertex2, 
	double &ptr, 
	double &ptr2, 
	double &ptr3, 
	double &ptr4, 
	double &ptr5, 
	int num)
{
	enum IntersectionMode intersectionMode = none;
	DPOINT dPoint, dPoint2, dPoint3, dPoint4, dPoint5, dPoint6;
	ReadPolyDPoint(Vertex, Vertex2, ptr, ptr2, ptr3, ptr4, ptr5, dPoint3, dPoint4, pPolyLine->FixedWidth);
	REALPOINT array[4];
	ZeroMemory(array, sizeof(REALPOINT) * 4);
	double num2 = ptr4;
	double num3 = ptr5;
	DXFENTVERTEX Vertex3 = GetVertex(pPolyLine, Vertex, num);
	if (IsFlagValue(Vertex, Vertex2, Vertex3))
	{
		array[0] = GetRealPointResult3(Vertex.Point, -num2, ptr, ptr2);
		array[1] = GetRealPointResult3(Vertex.Point, num2, ptr, ptr2);
	}
	else
	{
		ReadPolyDPoint(Vertex3, Vertex, ptr, ptr2, ptr3, ptr4, ptr5, dPoint, dPoint2, pPolyLine->FixedWidth);
		array[0] = ReadPolyPoint(dPoint2, dPoint4, intersectionMode);
		if(intersectionMode != point)
		{
			array[0] = GetRealPointResult3(Vertex.Point, -num2, ptr, ptr2);
		}
		array[1] = ReadPolyPoint(dPoint, dPoint3, intersectionMode);
		if(intersectionMode != point)
		{
			array[1] = GetRealPointResult3(Vertex.Point, num2, ptr, ptr2);
		}
	}
	Vertex3 = GetVertex2(pPolyLine, Vertex2, num);
	if (IsFlagValue(Vertex2, Vertex, Vertex3))
	{
		GetRatio(Vertex.Point, Vertex2.Point, ptr, ptr2, ptr3);
		array[2] = GetRealPointResult3(Vertex2.Point, num3, ptr, ptr2);
		array[3] = GetRealPointResult3(Vertex2.Point, -num3, ptr, ptr2);
	}
	else
	{
		ReadPolyDPoint(Vertex2, Vertex3, ptr, ptr2, ptr3, ptr4, ptr5, dPoint5, dPoint6, pPolyLine->FixedWidth);
		array[2] = ReadPolyPoint(dPoint3, dPoint5, intersectionMode);
		if(intersectionMode != point)
		{
			array[2] = GetRealPointResult3(Vertex2.Point, num3, ptr, ptr2);
		}
		array[3] = ReadPolyPoint(dPoint4, dPoint6, intersectionMode);
		if(intersectionMode != point)
		{
			array[3] = GetRealPointResult3(Vertex2.Point, -num3, ptr, ptr2);
		}
	}
	RealPointList.add(array[0]);
	RealPointList.add(array[1]);
	RealPointList.add(array[2]);
	RealPointList.add(array[3]);
}

void ReadPolyLinePointBulge(SimpleList <REALPOINT> &RealPointList, REALPOINT &item, REALPOINT &item2, double num, int num2, int num3)
{
	REALPOINT item3;
	ZeroMemory(&item3, sizeof(REALPOINT));
	double num4 = (item.x + item2.x) / 2.0;
	double num5 = (item.y + item2.y) / 2.0;
	double num6 = item.z;
	double num7 = (item2.z - num6) / 15.0;
	double num8 = item.x - num4;
	double num9 = item.y - num5;
	double num10 = num4 - num * num9;
	double num11 = num5 + num * num8;
	double num12 = (1.0 - num * num) / 2.0 / num;
	num4 += num12 * num9;
	num5 -= num12 * num8;
	num8 = num10 - num4;
	num9 = num11 - num5;
	num12 = sqrt(num8 * num8 + num9 * num9);
	double num13 = Atan2ToRad(num4, num5, item.x, item.y);
	double num14 = Atan2ToRad(num4, num5, item2.x, item2.y);
	if (num > 0.0 && num13 > num14)
	{
		num13 -= 6.2831853071795862;
	}
	if (num < 0.0 && num13 < num14)
	{
		num14 -= 6.2831853071795862;
	}
	RealPointList.add(item);
	num14 = (num13 - num14) / (double)num3;
	for (int i = 0; i < num3 - 1; i++)
	{
		num13 -= num14;
		double num15 = sin(num13);
		double num16 = cos(num13);
		item3.x = num4 + num12 * num16;
		item3.y = num5 + num12 * num15;
		item3.z = num6;
		RealPointList.add(item3);
		num6 += num7;
	}
	RealPointList.add(item2);
}

void ReadPolyLinePoint(PDXFENTPOLYLINEBASE pPolyLine)
{
	SimpleList <REALPOINT> RealPointList;
	bool flag = (pPolyLine->MeshM * pPolyLine->MeshN == pPolyLine->nVertex);
	int count = pPolyLine->nVertex;

	if (flag) //多边形网格
	{
		for (int i = 0; i < count; i++)
		{
			if (!IsFlagValueInt16(pPolyLine->pVertex[i].Flag))
			{
				if (!IsFlagValueInt128(pPolyLine->pVertex[i].Flag))
				{
					RealPointList.add(pPolyLine->pVertex[i].Point);
				}
				else //flag = 128
				{
					ReadPolyFaceVertexIndex((PDXFENTPOLYLINE)pPolyLine, i); //读取多面网格顶点索引值
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			if (!IsFlagValueInt16(pPolyLine->pVertex[i].Flag))
			{
				if (!IsFlagValueInt128(pPolyLine->pVertex[i].Flag))
				{
					REALPOINT dPoint =  pPolyLine->pVertex[i].Point;
					double bulge = pPolyLine->pVertex[i].Bulge;
					REALPOINT dPoint2;
					if (i == count - 1)
					{
						if (!pPolyLine->IsMeshNClosed && !pPolyLine->Closed)
						{
							RealPointList.add(dPoint);
							goto IL_2DA;
						}
						dPoint2 = pPolyLine->pVertex[0].Point;
					}
					else
					{
						dPoint2 = pPolyLine->pVertex[i+1].Point;
					}

					if (!EqualPoint(dPoint, dPoint2))
					{
						if (bulge == 0.0f)
						{
							RealPointList.add(dPoint);
							//set defaults
						}
						else
						{
							ReadPolyLinePointBulge(RealPointList, dPoint, dPoint2, bulge, 0, NumberOfPartsInCircle);
							//set defaults
						}
					}
					else
					{
						RealPointList.add(dPoint);
					}
				}
				else //flag = 128
				{
					ReadPolyFaceVertexIndex((PDXFENTPOLYLINE)pPolyLine, i); //读取多面网格顶点索引值
				}
			}
            IL_2DA:;
		}
	}

	pPolyLine->nPolyPoints = RealPointList.getSize();
	pPolyLine->pPolyPoints = ReadSimpleListData(RealPointList);
	RealPointList.clear();
}

void ReadPolyArrowsPoint(PDXFENTPOLYLINEBASE pPolyLine) 
{
	double num = 0.0f;
	double num2 = 0.0f;
	double num3 = 0.0f;
	double num4 = 0.0f;
	double num5 = 0.0f;
	int num6 = 1;
    int num7 = 0;
	int num8 = NumberOfPartsInCircle + 1;
	SimpleList <REALPOINT> RealPointList;
	DXFENTVERTEX Vertex = pPolyLine->pVertex[0];
	int count = pPolyLine->nVertex;

	if (count > 0 && Vertex.StartWidth > 0.0f)
	{
		bool flag = true;
		num3 = (double)Vertex.StartWidth;
		DXFENTVERTEX Vertex2;
		for (int i = 0; i < count; i++) 
		{
			Vertex2 = pPolyLine->pVertex[i];
			flag = (Vertex2.StartWidth == Vertex2.EndWidth && (double)Vertex2.StartWidth == num3 && flag);
			num3 = (double)Vertex2.StartWidth;
			if (!flag)
			{
				break;
			}
		}
		if (flag)
		{
			pPolyLine->FixedWidth = Vertex2.StartWidth;
		}
	}
	else
	{
		if (pPolyLine->FixedWidth == 0.0f)
		{
			//set default
		}
	}

	for (int i = 0; i < count - 1 + ToInt32(pPolyLine->Closed); i++)
	{
		DXFENTVERTEX Vertex2 = pPolyLine->pVertex[i];
		if ((pPolyLine->FixedWidth != 0.0f || Vertex2.StartWidth != 0.0f || Vertex2.EndWidth != 0.0f) /*&& !IsFlagValue1(Vertex2.Flag)*/)
		{
			GetVertexWidth(Vertex2.StartWidth, Vertex2.EndWidth, pPolyLine->FixedWidth, num4, num5);
			DXFENTVERTEX Vertex3;
			if (i != count - 1)
			{
				Vertex3 = pPolyLine->pVertex[i+1];
			}
			else
			{
			    Vertex3 = pPolyLine->pVertex[0];
			}

			GetRatio(Vertex2.Point, Vertex3.Point, num, num2, num3);
			if (Vertex2.Bulge == 0.0f)
			{
				if (!EqualPoint(Vertex2.Point, Vertex3.Point))
				{
					ReadPolyArrowsPointNoBulge(pPolyLine, RealPointList, Vertex2, Vertex3, num, num2, num3, num4, num5, i);
				}
			}
			else
			{
				if (num7 < i)
				{
					num7 = i;
				}
				int num9 = GetNum(pPolyLine, Vertex3.Point, num7);
				num7 = GetNum(pPolyLine, Vertex2.Point, num7);
				if (num9 >=0 && num7 >=0)
				{
					REALPOINT dPoint = GetRealPointResult(Vertex2.Point, Vertex3.Point, Vertex2.Bulge);
					double num10 = Distance(pPolyLine->pPolyPoints[num7], pPolyLine->pPolyPoints[num9]) * 0.5;
					num5 /= 2.0;
					if (num5 > num10)
					{
						num5 = num10;
					}
					num4 /= 2.0;
					if (num4 > num10)
					{
						num4 = num10;
					}
					double num11 = num4;
					double num12 = (num5 - num4) / (double)(num8 - 2);
					for( int j = num7 + ToInt32(EqualPoint(GetPolyPointsIndex(pPolyLine, num7), GetPolyPointsIndex(pPolyLine, num7 + 1))); j <= num9 - 1;  j += num6)
					{
						num4 = num11 + (double)(j - num7) * num12;
						num5 = num11 + (double)(j + 1 - num7) * num12;
						REALPOINT dPoint2 = GetPolyPointsIndex(pPolyLine, j);
						double num13 = Atan2ToRad(dPoint.x, dPoint.y, dPoint2.x, dPoint2.y);
						double num14 = sin(num13);
						double num15 = cos(num13);
						REALPOINT dPoint3;
						dPoint3.x = num4 * num15;
						dPoint3.y = num4 * num14;
						RealPointList.add(GetRealPointResult2(dPoint2, dPoint3, 1));
						RealPointList.add(GetRealPointResult2(dPoint2, dPoint3, -1));
						dPoint2 = GetPolyPointsIndex(pPolyLine, j + 1);
						num13 = Atan2ToRad(dPoint.x, dPoint.y, dPoint2.x, dPoint2.y);
						num14 = sin(num13);
						num15 = cos(num13);
						dPoint3.x= num5 * num15;
						dPoint3.y= num5 * num14;
						RealPointList.add(GetRealPointResult2(dPoint2, dPoint3, -1));
						RealPointList.add(GetRealPointResult2(dPoint2, dPoint3, 1));
					}
					num7 = num9;
				}
			}
		}
	}

	pPolyLine->nArrows = RealPointList.getSize();
	pPolyLine->pArrows =  ReadSimpleListData(RealPointList);
	RealPointList.clear();
}

BOOL ReadPolyLineData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTPOLYLINE pPolyLine )
{
	ZeroMemory(pPolyLine, sizeof(DXFENTPOLYLINE));
	pEntityHeader->EntityType = ENT_POLYLINE;
	ReadParamFromDxfFile(pDxf, GCode, strValue);	

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:      // Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;	
		case 48:   // Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 10:	// Polyline virtual point X
			pPolyLine->VPoint.x = atof(strValue);
			break;
		case 20:	// Polyline virtual point Y
			pPolyLine->VPoint.y = atof(strValue);
			break;
		case 30:	// Polyline virtual point Z（Polyline level）; default is 0
			pPolyLine->VPoint.z = atof(strValue);
			break;
		case 70:	// Polyline flag (bit-coded); default is 0
			{
				pPolyLine->Flag = atoi(strValue);
				if (pPolyLine->Flag & POLYLINE_CLOSED)
				{
					pPolyLine->Closed = true;
					pPolyLine->IsMeshMClosed = true;
				} 
				if (pPolyLine->Flag & POLYLINE_NCLOSED)
				{
					pPolyLine->IsMeshNClosed = true;
				}
				if (pPolyLine->Flag & POLYLINE_POLYFACEMESH)
				{
					pPolyLine->IsPolyFaceMesh = true;
				}
				if (pPolyLine->Flag & POLYLINE_POLYMESH)
				{
					pPolyLine->IsPolygonMesh = true;
				}
			}
			break;	
		case 40:	// Start width
			pPolyLine->StartWidth = atof(strValue);
			break;
		case 41:	// End width
			pPolyLine->EndWidth = atof(strValue);
			break;
		case 71:   // Polygon mesh M count
			pPolyLine->MeshM = atoi(strValue);
			break;
		case 72:   // Polygon mesh N count
			pPolyLine->MeshN = atoi(strValue);
			break;
		case 73:   // Smooth surface thickness M
			pPolyLine->MeshM = atoi(strValue);
			break;
		case 74:   // Smooth surface thickness N
			pPolyLine->MeshN = atoi(strValue);
			break;
		case 75:   // Curve and Smooth Polyface flag
			pPolyLine->PolyFlag = atoi(strValue);
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	// Reading Vertex Data
	SimpleList<DXFENTVERTEX> vertices;
	DXFENTVERTEX Vertex;
	ZeroMemory(&Vertex, sizeof(DXFENTVERTEX));
	while((GCode==0) && (strcmp(strValue,"VERTEX")==0))
	{
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);

		while(GCode!=0)
		{
			switch(GCode)
			{
			case 10:	// Start point X
				Vertex.Point.x = atof(strValue);
				break;		
			case 20:	// Start point Y
				Vertex.Point.y = atof(strValue);
				break;		
			case 30:	// Start point Z
				Vertex.Point.z = atof(strValue);
				break;
			case 40:	// Start Width
				Vertex.StartWidth = atof(strValue);
				break;
			case 41:	// End Width
				Vertex.EndWidth = atof(strValue);
				break;
			case 42:	// Bulge (optional; default is 0)
				Vertex.Bulge = atof(strValue);
				break;
			case 70:	// Vertex flag (default is 0)
				Vertex.Flag = atoi(strValue);
				break;
			case 71:	// PolyFace Vertex Index first
				Vertex.PolyFaceVertexIndex1 = atoi(strValue);
				break;
			case 72:	// PolyFace Vertex Index second
				Vertex.PolyFaceVertexIndex2 = atoi(strValue);
				break;
			case 73:	// PolyFace Vertex Index third
				Vertex.PolyFaceVertexIndex3 = atoi(strValue);
				break;
			case 74:	// PolyFace Vertex Index fourth
				Vertex.PolyFaceVertexIndex4 = atoi(strValue);
				break;
			}
			dxfStorePos(pDxf);
			ReadParamFromDxfFile(pDxf, GCode, strValue);
		}

		if (!IsFlagValueInt16(Vertex.Flag)) //Vertex flag not equal 16
		{
			pPolyLine->nVertex++;
		}

		vertices.add(Vertex);
		ZeroMemory(&Vertex, sizeof(DXFENTVERTEX));
	}

	pPolyLine->pVertex = new DXFENTVERTEX[pPolyLine->nVertex];
	int n = pPolyLine->nVertex - 1;
	vertices.start();
	while (vertices.next()) 
	{
	    Vertex = vertices.get();
		if (!IsFlagValueInt16(Vertex.Flag))
		{
			pPolyLine->pVertex[n] = Vertex;
			n--;
		}
	}
	vertices.clear();

	ReadPolyLinePoint(pPolyLine);
	ReadPolyArrowsPoint(pPolyLine);

	return TRUE;
}

BOOL ReadLWPolyLineData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTLWPOLYLINE pPolyLine )
{
	ZeroMemory(pPolyLine, sizeof(DXFENTPOLYLINE));
	pEntityHeader->EntityType = ENT_LWPOLYLINE;
	pPolyLine->FixedWidth = 0.0f;

	ReadParamFromDxfFile(pDxf, GCode, strValue);	
	int i = -1;

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;	
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 90:    // Number of Vertex
			{
				pPolyLine->nVertex=atoi(strValue);
				pPolyLine->pVertex = new DXFENTVERTEX[pPolyLine->nVertex];
				memset(pPolyLine->pVertex, 0, sizeof(DXFENTVERTEX) * pPolyLine->nVertex);
			}
			break;
		case 70:	// Polyline flag (bit-coded); default is 0
			pPolyLine->Flag = atoi(strValue);
			if (pPolyLine->Flag & POLYLINE_CLOSED)
			{
				pPolyLine->Closed = true;
				pPolyLine->IsMeshMClosed = true;
			}
			break;
		case 43:    // Polyline FixedWidth
			pPolyLine->FixedWidth = atof(strValue);
			break;
		case 10:	// Start point X
			i++;
			pPolyLine->pVertex[i].Point.x = atof(strValue);
			break;		
		case 20:	// Start point Y
			pPolyLine->pVertex[i].Point.y = atof(strValue);
			break;
		case 40:	// Start Width
			pPolyLine->pVertex[i].StartWidth = atof(strValue);
			break;
		case 41:	// End Width
			pPolyLine->pVertex[i].EndWidth = atof(strValue);
			break;
		case 42:	// Bulge (optional; default is 0)
			pPolyLine->pVertex[i].Bulge = atof(strValue);
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	ReadPolyLinePoint(pPolyLine);
	ReadPolyArrowsPoint(pPolyLine);

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadLeaderData                                                   |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTLEADER pLeader = pointer to Leader structure          |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadLeaderData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTLEADER pLeader )
{
	ZeroMemory(pLeader, sizeof(DXFENTLEADER));
	pEntityHeader->EntityType = ENT_LEADER;
	ReadParamFromDxfFile(pDxf, GCode, strValue);	

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;	
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 70:	// Polyline flag (bit-coded); default is 0
			pLeader->Flag = atoi(strValue);
			break;	
		case 76:
			pLeader->nVertex=atoi(strValue);
			{
				SimpleList<DXFENTVERTEX> vertices;
				DXFENTVERTEX Vertex;
				ZeroMemory(&Vertex, sizeof(DXFENTVERTEX));
				for (int i=0;i<pLeader->nVertex;i++)
				{
					dxfStorePos(pDxf);
					ReadParamFromDxfFile(pDxf, GCode, strValue);	
					for(int j=0;j<3;j++)
					{
						switch(GCode)
						{
						case 10:	// Start point X
							Vertex.Point.x = atof(strValue);
							break;		
						case 20:	// Start point X
							Vertex.Point.y = atof(strValue);
							break;		
						case 30:	// Start point X
							Vertex.Point.z = atof(strValue);
							break;		
						}
						dxfStorePos(pDxf);
						ReadParamFromDxfFile(pDxf, GCode, strValue);
					}
					vertices.add(Vertex);		
				}
				pLeader->pVertex = ReadSimpleListData(vertices);
			}
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadDimensionData                                                |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTDIMENSION pDimension = pointer to Dimension structure |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadDimensionData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTDIMENSION pDimension )
{
	ZeroMemory(pDimension, sizeof(DXFENTDIMENSION));
	pEntityHeader->EntityType = ENT_DIMENSION;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;	
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 10:	// Definition point (in WCS) X
			pDimension->DimLineDefPoint.x = atof(strValue);
			break;
		case 20:	// Definition point (in WCS) Y
			pDimension->DimLineDefPoint.y = atof(strValue);
			break;
		case 30:	// Definition point (in WCS) Z
			pDimension->DimLineDefPoint.z = atof(strValue);
			break;
		case 13:	// Definition point for linear and angular dimensions (in WCS) X
			pDimension->DefPoint3.x = atof(strValue);
			break;
		case 23:	// Definition point for linear and angular dimensions (in WCS) Y
			pDimension->DefPoint3.y = atof(strValue);
			break;
		case 33:	// Definition point for linear and angular dimensions (in WCS) Z
			pDimension->DefPoint3.z = atof(strValue);
			break;
		case 14:	// Definition point for linear and angular dimensions (in WCS) X
			pDimension->DefPoint4.x = atof(strValue);
			break;
		case 24:	// Definition point for linear and angular dimensions (in WCS) Y
			pDimension->DefPoint4.y = atof(strValue);
			break;
		case 34:	// Definition point for linear and angular dimensions (in WCS) Z
			pDimension->DefPoint4.z = atof(strValue);
			break;
		case 50:	// The rotation angle of the dimension text away from its default orientation (the direction of the dimension line)
			pDimension->DimRotationAngle = atof(strValue);
			break;
		case 3:		// Dimension style name
			strcpy_s(pDimension->DimStyleName, strValue);
			break;
		case 1:		// Dimension text explicitly entered by the user. Optional; default is the measurement. If null or "<>", the dimension measurement is drawn as the text, if " " (one blank space), the text is suppressed.
			{
				int i=MultiByteToWideChar(CP_UTF8,0,strValue,-1,NULL,-1);
				MultiByteToWideChar(CP_UTF8,0,strValue,-1,pDimension->DimText,i);
			}
			break;
		case 2:		// Name of the block that contains the entities that make up the dimension picture
			strcpy_s(pDimension->BlockName, strValue);
			break;
 		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadMLineData                                                    |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTMLINE pMLine = pointer to MLine structure             |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadMLineData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTMLINE pMLine )
{
	ZeroMemory(pMLine, sizeof(DXFENTMLINE));
	pEntityHeader->EntityType = ENT_MLINE;
	pMLine->pVetexPoint=new SimpleList<MLINEVETEX>();
	MLINEVETEX point;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0&&GCode!=11)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 40:
			pMLine->ScaleFacter=atof(strValue);//比例因子
			break;
		case 70:
			pMLine->Alige=atoi(strValue); //对正
			break;
		case 71:
			pMLine->Simble=atoi(strValue);  //标志
			break;
		case 72:
			pMLine->VetexNum=atoi(strValue); //顶点数
			break;
		case 10:
			pMLine->StartPoint.x=atof(strValue);
			break;
		case 20:
			pMLine->StartPoint.y=atof(strValue);
			break;
		case 30:
			pMLine->StartPoint.z=atof(strValue);
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}
	int i=0;
	int j=0;
	int k=0;
	while(GCode!=0)
    {
			k=0;
			i=0;
		while(k!=2)
		{
			 
			switch(GCode)
			{
			case 11:
				point.VetexPoint.x=atof(strValue);
				break;
			case 21:
				point.VetexPoint.y=atof(strValue);
				break;
			case 31:
				point.VetexPoint.z=atof(strValue);
				break;
			case 12:
				point.DirVetex.x=atof(strValue);
				break;
			case 22:
				point.DirVetex.y=atof(strValue);
				break;
			case 32:
				point.DirVetex.z=atof(strValue);
				break;
			case 13:
				point.ObliqueDirVetex.x=atof(strValue);
				break;
			case 23:
				point.ObliqueDirVetex.y=atof(strValue);
				break;
			case 33:
				point.ObliqueDirVetex.z=atof(strValue);
				break;
			case 74:
			case 41:
				point.pElemParem[i]=atof(strValue);//元素参数，
				i++;
				break;
				/*case 42:
				point->pRegionFillParem[j]=atof(strValue); //区域填充参数，组码：42
				j++;
				break;*/
			case 75:
				point.RegionFillParemNum=atoi(strValue);
				k++;
				break;
			}
			dxfStorePos(pDxf);
			ReadParamFromDxfFile(pDxf, GCode, strValue);
		}
		
		pMLine->pVetexPoint->add(point);

	}

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadSPlineData                                                   |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTSPLINE pSPline = pointer to SPline structure          |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadSPlineData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTSPLINE pSPline )
{
	ZeroMemory(pSPline, sizeof(DXFENTSPLINE));
	pEntityHeader->EntityType = ENT_SPLINE;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0&&GCode!=40)
	{
		switch(GCode)
		{
		case 8:		
			strcpy_s(pEntityHeader->LayerName, strValue);//图层名
			break;
		case 62:	
			pEntityHeader->Color = atoi(strValue);//样条曲线颜色
			break;
		case 70:
			pSPline->SplineMarks=atoi(strValue);//样条曲线标志
			break;
		case 71:
			pSPline->SplineOrder=atoi(strValue);//样条曲线阶数
			break;
		case 72:
			pSPline->NodeNum=atoi(strValue);//样条曲线节点数
			break;
		case 73:
			pSPline->ControlNodeNum=atoi(strValue);//样条曲线控制点数
			break;
		case 74:
			pSPline->FittingPointNum=atoi(strValue);//拟合点数
			break;
		case 42:
		    pSPline->NodeTolerance=atof(strValue);//节点公差
			break;
		case 43:
			pSPline->ControlTolerance=atof(strValue);//控制点公差
			break;
		case 44:
			pSPline->FittingTolerance=atof(strValue);//拟合点公差
			break;
		case 12:
			pSPline->StartTangent.x=atof(strValue);//起点切向
			break;
		case 22:
			pSPline->StartTangent.y=atof(strValue);
			break;
		case 32:
			pSPline->StartTangent.z=atof(strValue);
			break;
		case 13:
			pSPline->EndTangent.x=atof(strValue); //终点切向
			break;
		case 23:
			pSPline->EndTangent.y=atof(strValue);
			break;
		case 33:
			pSPline->EndTangent.z=atof(strValue);
			break;
		case 41:
			pSPline->Weight=atof(strValue);
			break;
		}
	    dxfStorePos(pDxf);
	    ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	pSPline->pNodePoint=new double[pSPline->NodeNum];
	SimpleList<REALPOINT> *ControlVertices=new SimpleList<REALPOINT>();
	SimpleList<REALPOINT> *FittingVertices=new SimpleList<REALPOINT>();
	double NodeVertex;
	REALPOINT ControlVertex;
	REALPOINT FittingVertex;
	int i=0;
	while((GCode==40))
	{
		NodeVertex=atof(strValue);
		pSPline->pNodePoint[i]=NodeVertex;
		i++;
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}
	while(GCode!=11 && GCode!=0 && GCode!=100/*在Helix中使用*/)
	{
		int j=0;
		while(GCode==10||GCode==20||GCode==30)
		{
			switch(GCode)
			{
			case 10:	// Start point X
				ControlVertex.x = atof(strValue);
				j++;
				break;		
			case 20:	// Start point X
				ControlVertex.y = atof(strValue);
				j++;
				break;		
			case 30:	// Start point X
				ControlVertex.z= atof(strValue);
				j++;
				break;
			}
			dxfStorePos(pDxf);
			ReadParamFromDxfFile(pDxf, GCode, strValue);
			if(j==3)
			{
				j=0;
				break;
			}
		}
		ControlVertices->add(ControlVertex);
	}
	while (GCode!=0 && GCode!=100/*在Helix中使用*/)
	{
		int j=0;
		while(GCode==11||GCode==21||GCode==31)
		{
			switch(GCode)
			{
			case 11:
				FittingVertex.x=atof(strValue);
				j++;
				break;
			case 21:
				FittingVertex.y=atof(strValue);
				j++;
				break;
			case 31:
				FittingVertex.z=atof(strValue);
				j++;
				break;
			}
			dxfStorePos(pDxf);
			ReadParamFromDxfFile(pDxf, GCode, strValue);
			if(j==3)
			{
				j=0;
				break;
			}
		}
		FittingVertices->add(FittingVertex);
	}
	
	pSPline->pFittingNode = FittingVertices;	
	pSPline->pControlNode=ControlVertices;
	
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  Read3DFaceData                                                   |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENT3DFACE p3DFace = pointer to 3DFace structure          |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
void Read3DFaceSideVisible(PDXFENT3DFACE p3DFace, int TotalSidesVisible)
{
	if ((TotalSidesVisible & 1) == 0)
	{
		p3DFace->FirstSideVisible = true;
	}
	if ((TotalSidesVisible & 2) == 0)
	{
		p3DFace->SecondSideVisible = true;
	}
	if ((TotalSidesVisible & 4) == 0)
	{
		p3DFace->ThirdSideVisible = true;
	}
	if ((TotalSidesVisible & 8) == 0)
	{
		p3DFace->FourthSideVisible = true;
	}
}

BOOL Read3DFaceData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENT3DFACE p3DFace )
{
	ZeroMemory(p3DFace, sizeof(DXFENT3DFACE));
	pEntityHeader->EntityType = ENT_3DFACE;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 10:	// First corner X
			p3DFace->Point0.x = atof(strValue);
			break;
		case 20:	// First corner Y
			p3DFace->Point0.y = atof(strValue);
			break;
		case 30:	// First corner Z
			p3DFace->Point0.z = atof(strValue);
			break;
		case 11:	// Second corner X
			p3DFace->Point1.x = atof(strValue);
			break;
		case 21:	// Second corner Y
			p3DFace->Point1.y = atof(strValue);
			break;
		case 31:	// Second corner Z
			p3DFace->Point1.z = atof(strValue);
			break;
		case 12:	// Third corner X
			p3DFace->Point2.x = atof(strValue);
			break;
		case 22:	// Third corner Y
			p3DFace->Point2.y = atof(strValue);
			break;
		case 32:	// Third corner Z
			p3DFace->Point2.z = atof(strValue);
			break;
		case 13:	// Fourth corner X
			p3DFace->Point3.x = atof(strValue);
			break;
		case 23:	// Fourth corner Y
			p3DFace->Point3.y = atof(strValue);
			break;
		case 33:	// Fourth corner Z
			p3DFace->Point3.z = atof(strValue);
			break;
		case 70:   //Total sides visible
			int TotalSidesVisible = atoi(strValue);
			Read3DFaceSideVisible(p3DFace, TotalSidesVisible);
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadTraceData                                                    |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTTRACE pTrace = pointer to Trace structure             |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadTraceData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTTRACE pTrace )
{
	ZeroMemory(pTrace, sizeof(DXFENTTRACE));
	pEntityHeader->EntityType = ENT_TRACE;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 10:	// First corner X
			pTrace->Point0.x = atof(strValue);
			break;
		case 20:	// First corner Y
			pTrace->Point0.y = atof(strValue);
			break;
		case 30:	// First corner Z
			pTrace->Point0.z = atof(strValue);
			break;
		case 11:	// Second corner X
			pTrace->Point1.x = atof(strValue);
			break;
		case 21:	// Second corner Y
			pTrace->Point1.y = atof(strValue);
			break;
		case 31:	// Second corner Z
			pTrace->Point1.z = atof(strValue);
			break;
		case 12:	// Third corner X
			pTrace->Point2.x = atof(strValue);
			break;
		case 22:	// Third corner Y
			pTrace->Point2.y = atof(strValue);
			break;
		case 32:	// Third corner Z
			pTrace->Point2.z = atof(strValue);
			break;
		case 13:	// Fourth corner X
			pTrace->Point3.x = atof(strValue);
			break;
		case 23:	// Fourth corner Y
			pTrace->Point3.y = atof(strValue);
			break;
		case 33:	// Fourth corner Z
			pTrace->Point3.z = atof(strValue);
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadHatchData                                                    |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTHATCH pHatch= pointer to Hatch structure              |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
//解析LWPolyLine边界
void ParseLWPolyLines(PHATCHPOLYLINEDATA pPolyLine)
{
	if (pPolyLine->pCoordPoint != NULL)
	{
		delete[] pPolyLine->pCoordPoint;
		pPolyLine->pCoordPoint = NULL;
	}

	SimpleList <POINT2D> CoordPointList;
	int nVertex = pPolyLine->nVertex;
	pPolyLine->StartPoint = pPolyLine->pVertex[0].Point;
	pPolyLine->EndPoint = pPolyLine->pVertex[nVertex-1].Point;

	int num = 46;
	int num2 = nVertex - 1;
	for (int i = 0; i <= num2; i++)
	{
		POINT2D Point = pPolyLine->pVertex[i].Point;
	    double num3 = pPolyLine->pVertex[i].Bulge;
		if (num3 != 0.0f)  //有凸度
		{
			POINT2D item;
			ZeroMemory(&item, sizeof(POINT2D));
			if ( i < num2)
			{
				item = pPolyLine->pVertex[i+1].Point;
			} 
			else
			{
				item = pPolyLine->pVertex[0].Point;
			}
			double num4 = (Point.x + item.x) * 0.5f;
			double num5 = (Point.y + item.y) * 0.5f;
			double num6 = (double)num3;
			double num7 = Point.x - num4;
			double num8 = Point.y - num5;
			double num9 = num4 - num6 * num8;
			double num10 = num5 + num6 * num7;
			num6 = (1.0f - num6 * num6) * 0.5f / num6;
			num4 += num6 * num8;
			num5 -= num6 * num7;
			num7 = num9 - num4;
			num8 = num10 - num5;
			num6 = sqrt(num7 * num7 + num8 * num8);
			double num11 = Atan2ToRad(num4, num5, Point.x, Point.y);
			double num12 = Atan2ToRad(num4, num5, item.x, item.y);
			if (num3 > 0.0f && num11 > num12)
			{
				num11 -= 2 * PI;
			}
			if (num3 < 0.0f && num11 < num12)
			{
				num12 -= 2 * PI;
			}
			POINT2D item2 = Point;
			CoordPointList.add(item2);
			num12 = (num11 - num12) / 47.0f;
			for (int j = 0; j < num; j++)
			{
				num11 -= num12;
				double num13 = sin(num11);
				double num14 = cos(num11);
				item2.x = num4 + num6 * num14;
				item2.y = num5 + num6 * num13;
				CoordPointList.add(item2);
			}
			CoordPointList.add(item);
		}
		else //无凸度
		{
			CoordPointList.add(Point);
		}
	}

	pPolyLine->CoordCount = CoordPointList.getSize();
	pPolyLine->pCoordPoint = ReadSimpleListData(CoordPointList);
	CoordPointList.clear();
}

//解析Line边界
void ParseLinePolyLines(PHATCHLINEDATA pLine)
{
	if (pLine->pCoordPoint != NULL)
	{
		delete[]pLine->pCoordPoint;
		pLine->pCoordPoint = NULL;
	}

	pLine->CoordCount = 2;
	pLine->pCoordPoint = new POINT2D[2];
	ZeroMemory(pLine->pCoordPoint, sizeof(POINT2D) * 2);
	
	pLine->pCoordPoint[0] = pLine->StartPoint;
	pLine->pCoordPoint[1] = pLine->EndPoint;
}

//解析Arc边界
void ParseArcPolyLines(PHATCHARCDATA pArc)
{
	if (pArc->pCoordPoint != NULL)
	{
		delete[] pArc->pCoordPoint;
		pArc->pCoordPoint = NULL;
	}

	pArc->CoordCount = NumberOfPartsInCircle ;
	pArc->pCoordPoint = new POINT2D[NumberOfPartsInCircle];
	ZeroMemory(pArc->pCoordPoint, sizeof(POINT2D) * NumberOfPartsInCircle);

	double num = pArc->EndAngle;
	double num2 = pArc->StartAngle;
	double num3 = 1.0;
	double num4 = 0.0;
	if (!pArc->AnticlockWiseFlag)
	{
		num4 = num;
		num = -num2;
		num2 = -num4;
	}
	double num5 = pArc->Radius;
	double num6 = pArc->Radius;
	double num7 = 0.0;
	double num8 = 1.0;
	if (num <= num2)
	{
		num += 360.0;
	}
	int num9 = NumberOfPartsInCircle;
	num2 *= 0.017453292519943295;
	num *= 0.017453292519943295;
	num4 = (num - num2) / (double)(num9 - 1);
	double num10;
	if (!pArc->AnticlockWiseFlag)
	{
		num3 = -1.0;
		num10 = num;
	}
	else
	{
		num10 = num2;
	}
	for (int i = 0; i < num9; i++)
	{
		double num11 = sin(num10);
		double num12 = cos(num10);
		num10 += num3 * num4;
		double num13 = num5 * num12;
		double num14 = num6 * num11;
		pArc->pCoordPoint[i].x = pArc->Point0.x + num13 * num8 - num14 * num7;
		pArc->pCoordPoint[i].y = pArc->Point0.y + num13 * num7 + num14 * num8;
	}
	pArc->StartPoint = pArc->pCoordPoint[0];
	pArc->EndPoint = pArc->pCoordPoint[NumberOfPartsInCircle-1];
}

//解析Ellipse边界
double GetEllipseParam(double num, double n)
{
	double num6 = 0;
	double num2 = num * PI / 180.0;
	double num3 = sin(num2);
	double num4 = cos(num2);
	if (abs(num4) > 1E-06)
	{
		double num5 = num3 / num4;
		num6 = atan(num5 / n) * 180.0 / PI;
		switch ((ToInt32(num3 < 0.0) << 1) + ToInt32(num4 < 0.0))
		{
		case 1:
		case 3:
			num6 += 180.0;
			num6 -= 360.0 * floor(num6 / 360.0);
			break;
		}
	}
	else
	{
		num6 = num;
	}
	return num6;
}

void ParseEllipsePolyLines(PHATCHELLIPSEDATA pEllipse)
{
	if (pEllipse->pCoordPoint != NULL)
	{
		delete[] pEllipse->pCoordPoint;
		pEllipse->pCoordPoint = NULL;
	}
	pEllipse->CoordCount = NumberOfPartsInCircle;
	pEllipse->pCoordPoint = new POINT2D[NumberOfPartsInCircle];
	ZeroMemory(pEllipse->pCoordPoint, sizeof(POINT2D) * NumberOfPartsInCircle);

	double num = pEllipse->EndParam;
	double num2 = pEllipse->StartParam;
	double num3 = 1.0;
	double num4 = 0.0;
	if (!pEllipse->AnticlockWiseFlag)
	{
		num4 = num;
		num = -num2;
		num2 = -num4;
	}
	double num5 = sqrt(Sqr(pEllipse->MajorAxisEndPoint.x) + Sqr(pEllipse->MajorAxisEndPoint.y));
	double num6 = num5 * pEllipse->MinorToMajorRatio;
	double n = atan2(pEllipse->MajorAxisEndPoint.y, pEllipse->MajorAxisEndPoint.x);
	double num7 = sin(n);
	double num8 = cos(n);
	num2 = GetEllipseParam(num2, pEllipse->MinorToMajorRatio);
	num = GetEllipseParam(num, pEllipse->MinorToMajorRatio);
	if (num <= num2)
	{
		num += 360.0;
	}
	int num9 = NumberOfPartsInCircle;
	num2 *= 0.017453292519943295;
	num *= 0.017453292519943295;
	num4 = (num - num2) / (double)(num9 - 1);
	double num10;
	if (!pEllipse->AnticlockWiseFlag)
	{
		num3 = -1.0;
		num10 = num;
	}
	else
	{
		num10 = num2;
	}
	for (int i = 0; i < num9; i++)
	{
		double num11 = sin(num10);
		double num12 = cos(num10);
		num10 += num3 * num4;
		double num13 = num5 * num12;
		double num14 = num6 * num11;
		pEllipse->pCoordPoint[i].x = pEllipse->CenterPoint.x + num13 * num8 - num14 * num7;
		pEllipse->pCoordPoint[i].y = pEllipse->CenterPoint.y + num13 * num7 + num14 * num8;
	}
	pEllipse->StartPoint = pEllipse->pCoordPoint[0];
	pEllipse->EndPoint = pEllipse->pCoordPoint[NumberOfPartsInCircle-1];
}

//解析Spline边界
double GetSplineParam(double *pNode, int num, int num2, double num3)
{
	if (num == 0)
	{
		return (double)ToInt32(pNode[num2] <= num3 && num3 < pNode[num2 + 1]);
	}
	double num4 = pNode[num2 + num] - pNode[num2];
	double num5 = (num3 - pNode[num2]) * GetSplineParam(pNode, num - 1, num2, num3);
	if (num4 == 0.0)
	{
		num5 = 0.0;
	}
	else
	{
		num5 /= num4;
	}
	double num6 = (pNode[num2 + num + 1] - pNode[num2 + 1]);
	double num7 = (pNode[num2 + num + 1] - num3) * GetSplineParam(pNode, num - 1, num2 + 1, num3);
	if (num6 == 0.0)
	{
		num7 = 0.0;
	}
	else
	{
		num7 /= num6;
	}
	return num5 + num7;
}

POINT2D GetCoordN(PPOINT2D pControlNode, double *pNode, int num, double num2)
{
	POINT2D result;
	ZeroMemory(&result, sizeof(POINT2D));
	for (int i = num - 3; i <= num; i++)
	{
		double num3 = GetSplineParam(pNode, 3, i, num2);
		POINT2D Point = pControlNode[i];
		result.x += Point.x * num3;
		result.y += Point.y * num3;
	}
	return result;
}

void ParseSplinePolyLines(PHATCHSPLINEDATA pSpline)
{
	if (pSpline->pCoordPoint != NULL)
	{
		delete[] pSpline->pCoordPoint;
		pSpline->pCoordPoint = NULL;
	}
	SimpleList <POINT2D> CoordPointList;

	int ControlNodeNum = pSpline->ControlNodeNum;
	int NodeNum =pSpline->NodeNum;
	PPOINT2D pControlNode = ReadSimpleListData(*pSpline->pControlNode);
	double *pNodePoint = pSpline->pNodePoint;

	if (NodeNum != 0)
	{
		double num = 0.041666666666666664;
		if (ControlNodeNum > 4 && pControlNode[3].x == pControlNode[4].x &&  pControlNode[3].y == pControlNode[4].y)
		{
			num = 0.01;
		}
		while (NodeNum - ControlNodeNum < 4)
		{
			int last = NodeNum-1;
			double *p = new double[NodeNum+1];
			for (int i = 0; i <= last; i++)
			{
				p[i] = pNodePoint[i];
			}
			p[NodeNum] = p[last];

			delete[] pSpline->pNodePoint;
			pSpline->pNodePoint = p;
			pNodePoint = p;
			pSpline->NodeNum = NodeNum+1;
		}
		POINT2D item = pControlNode[0];
		CoordPointList.add(item);
		for (int i = 3; i < ControlNodeNum; i++)
		{
			double num2 = pNodePoint[i];
			double num3 = num * abs(pNodePoint[i+1] - pNodePoint[i]);
			while (num2 < pNodePoint[i+1])
			{
				item = GetCoordN(pControlNode, pNodePoint, i, num2);
				CoordPointList.add(item);
				num2 += num3;
			}
		}
		item = pControlNode[ControlNodeNum - 1];
		CoordPointList.add(item);
	}
	else
	{
		for (int j = 0; j < ControlNodeNum; j++)
		{
			POINT2D item = pControlNode[j];
			CoordPointList.add(item);
		}
	}
	pSpline->pCoordPoint = ReadSimpleListData(CoordPointList);
	pSpline->CoordCount = CoordPointList.getSize();
	CoordPointList.clear();
	pSpline->StartPoint = pSpline->pCoordPoint[0];
	pSpline->EndPoint = pSpline->pCoordPoint[pSpline->CoordCount - 1];
	delete[] pControlNode;
	pControlNode = NULL;
}

//解析Hatch边界坐标点
void ParseBoundaryPolyLines(PHATCHBOUNDARYDATA pHatchBoundaryData, int BoundaryPathNum)
{
	int num = 0;
	while ( num < BoundaryPathNum && pHatchBoundaryData != NULL)
	{
		SimpleList <PHATCHCURVEDATA> *pHatchCurveList = pHatchBoundaryData->pHatchCurveList;
		pHatchCurveList->start();
		while (pHatchCurveList->next())
		{
			PHATCHCURVEDATA pHatchCurveData = pHatchCurveList->get();
			switch (pHatchCurveData->EdgeType)
			{
			case HatchPathType_PolyLine:
				ParseLWPolyLines((PHATCHPOLYLINEDATA)pHatchCurveData);
				break;
			case HatchPathType_Line:
				ParseLinePolyLines((PHATCHLINEDATA)pHatchCurveData);
				break;
			case HatchPathType_Circle:
				ParseArcPolyLines((PHATCHARCDATA)pHatchCurveData);
				break;
			case HatchPathType_Ellipse:
				ParseEllipsePolyLines((PHATCHELLIPSEDATA)pHatchCurveData);
				break;
			case HatchPathType_Spline:
				ParseSplinePolyLines((PHATCHSPLINEDATA)pHatchCurveData);
				break;
			}
		}
		pHatchBoundaryData++;
		num++;
	}
}

void ParseHatchBoundaries(const HATCHBOUNDARYDATA &HatchBoundary, PBOUNDARIES pBoundaries)
{
	SimpleList <POINT2D> RealPointList;
	POINT2D RealPoint;
	ZeroMemory(&RealPoint, sizeof(POINT2D));
	SimpleList <PHATCHCURVEDATA> *pHatchCurveList = HatchBoundary.pHatchCurveList;

	int Num = pHatchCurveList->getSize();
	PHATCHCURVEDATA *pCurveList = ReadSimpleListData(*pHatchCurveList);

	int j = 0, n = 0;
	for (int i = 0; i < Num; i++)
	{
		PHATCHCURVEDATA pCurveData = pCurveList[i];  //局部变量
		int count = pCurveData->CoordCount;                  //局部变量
		if (j > 0)
		{
			double d1 = Distance2D(pCurveData->StartPoint, RealPoint);  //局部变量
			double d2 = Distance2D(pCurveData->EndPoint, RealPoint);    //局部变量
			if (d1 < d2)
			{
				for (n = 0; n < count; n++)
					RealPointList.add(pCurveData->pCoordPoint[n]);
				RealPoint = pCurveData->pCoordPoint[n-1];
			}
			else
			{
				for (n = count - 1; n >= 0; n--)
					RealPointList.add(pCurveData->pCoordPoint[n]);
				RealPoint = pCurveData->pCoordPoint[0];
			}
		}
		else
		{
			for (n = 0; n < count; n++)
				RealPointList.add(pCurveData->pCoordPoint[n]);
			RealPoint = pCurveData->pCoordPoint[n-1];
		}
		delete[] pCurveData->pCoordPoint;
		pCurveData->pCoordPoint = NULL;
		pCurveData->CoordCount = 0;
		j++;
	}

	pBoundaries->BoundaryPointNum = RealPointList.getSize();
	pBoundaries->pBoundaryPoint = ReadSimpleListData(RealPointList);
	RealPointList.clear();
	delete pCurveList;
	pCurveList = NULL;
}

BOOL ReadBoundaryPolyLines(PDXFENTHATCH pHatch)
{
	int BoundaryNum = pHatch->BoundaryPathNum;
	PHATCHBOUNDARYDATA pHatchBoundaryData = pHatch->pHatchBoundaryData;

	if (pHatchBoundaryData == NULL || BoundaryNum == 0)
	{
		return FALSE;
	}

    if (pHatch->pBoundaries != NULL)
    {
		delete[] pHatch->pBoundaries->pBoundaryPoint;
		delete[] pHatch->pBoundaries;
		pHatch->pBoundaries->pBoundaryPoint = NULL;
		pHatch->pBoundaries->BoundaryPointNum = 0;
		pHatch->pBoundaries = NULL;
    }

	pHatch->pBoundaries = new BOUNDARIES[BoundaryNum];
	PBOUNDARIES pHatchBoundaries = pHatch->pBoundaries;

	//解析各个图元边界点坐标
	ParseBoundaryPolyLines(pHatchBoundaryData, BoundaryNum);

	int num = 0;
	while (num < BoundaryNum && pHatchBoundaryData != NULL)
	{
		//set defaults
		//解析图案填充边界点坐标
		ParseHatchBoundaries(*pHatchBoundaryData, pHatchBoundaries);
		pHatchBoundaryData++;
		pHatchBoundaries++;
		num++;
	}

	return TRUE;
}

//解析Hatch填充坐标点
void ReadHVLineWithBaseP(SimpleList <LineWithBaseP> &pLineBasePList,
	int num, 
	const REALPOINT &dPoint, 
	const REALPOINT &dPoint2, 
	const REALPOINT &dPoint3, 
	const REALPOINT &ACK)
{
	int id = num ^ 1;
	if (abs(GetPointValue(ACK, num)) > 1E-06)
	{
		REALPOINT Point, Point2, Point3, Point4; //局部变量SO, ENQ, ETX, EOT
		ZeroMemory(&Point, sizeof(REALPOINT));
		ZeroMemory(&Point2, sizeof(REALPOINT));
		ZeroMemory(&Point3, sizeof(REALPOINT));
		ZeroMemory(&Point4, sizeof(REALPOINT));

		Point.x = abs(ACK.x);
		Point.y = abs(ACK.y);
		
		double num2 = min(GetPointValue(dPoint, num), GetPointValue(dPoint2, num));
		double num3 = max(GetPointValue(dPoint, num), GetPointValue(dPoint2, num));
		int num4 = (int)ceil((num3 - GetPointValue(dPoint3, num)) / (GetPointValue(Point, num) * 2.0)) << 1;
		double num5 = 1.0 - (double)(ToInt32(ACK.x * ACK.y < 0.0) << 1);

		double n = GetPointValue(dPoint3, num) + (double)(num4 - num) * GetPointValue(Point, num);
		SetPointValue(Point2, num, n);
		n = GetPointValue(dPoint3, id) + (double)(num4 - num) * GetPointValue(Point, id) * num5;
		SetPointValue(Point2, id, n);

		LineWithBaseP LineBaseP;
		ZeroMemory(&LineBaseP, sizeof(LineWithBaseP));

		while (GetPointValue(Point2, num) >= num2)
		{
			SetPointValue(Point3, num, GetPointValue(Point2, num));
			SetPointValue(Point3, id, GetPointValue(dPoint2, id));
			SetPointValue(Point4, num, GetPointValue(Point2, num));
			SetPointValue(Point4, id, GetPointValue(dPoint, id));
	
			LineBaseP.BP.x = Point2.x;
			LineBaseP.BP.y = Point2.y;
			LineBaseP.P1.x = Point3.x;
			LineBaseP.P1.y = Point3.y;
			LineBaseP.P2.x = Point4.x;
			LineBaseP.P2.y = Point4.y;
			pLineBasePList.add(LineBaseP);

			SetPointValue(Point2, num, GetPointValue(Point2, num) - GetPointValue(Point, num));
			SetPointValue(Point2, id, GetPointValue(Point2, id) - GetPointValue(Point, id) * num5);
		}
	}
}

void ReadBFLineWithBaseP(SimpleList <LineWithBaseP> &pLineBasePList, 
	int result, 
	bool flag, 
	const HatchRect &DRect,
	const REALPOINT &dPoint, 
	const REALPOINT &ACK,
	double ENQ)
{
	REALPOINT Point, Point2, Point3; //局部变量ENQ, ETX, EOT
	ZeroMemory(&Point, sizeof(REALPOINT));
	ZeroMemory(&Point2, sizeof(REALPOINT));
	ZeroMemory(&Point3, sizeof(REALPOINT));

	double num = double(ToInt32(flag) - ToInt32(!flag));
	double num2 = (result == 0 ? DRect.BottomRight.y : DRect.TopLeft.y);
	double num3 = (result == 0 ? DRect.TopLeft.y : DRect.BottomRight.y);

	double d1 = DRect.BottomRight.x;   //局部变量STX
 	double d2 = DRect.TopLeft.x;           //局部变量ETX
	bool flag1 = false;                            //局部变量SOH
 	if (flag)
 	{
 		SwapT(d1, d2);
 		flag1 = !flag1;
 	}

	LineWithBaseP LineBaseP;
	ZeroMemory(&LineBaseP, sizeof(LineWithBaseP));

	Point = dPoint;
	while (true)
	{
		Point2.y = num2;
		Point2.x = Point.x + (num2 - Point.y) / ENQ;
		Point3.y = num3;
		Point3.x = Point.x + (num3 - Point.y) / ENQ;

		if (!flag1)
		{
			if (num * Fval(d2, Point3.x) <= 0.0)
			{
				break;
			}
		} 
		else
		{
			if (num * Fval(d2, Point2.x) <= 0.0)
			{
				break;
			}
		}

		LineBaseP.BP.x = Point.x;
		LineBaseP.BP.y = Point.y;
		LineBaseP.P1.x = Point2.x;
		LineBaseP.P1.y = Point2.y;
		LineBaseP.P2.x = Point3.x;
		LineBaseP.P2.y = Point3.y;
		pLineBasePList.add(LineBaseP);

		Point.x = Point.x + ACK.x;
		Point.y = Point.y + ACK.y;
	}

	Point = dPoint;
	Point.x = Point.x - ACK.x;
	Point.y = Point.y - ACK.y;
	while (true)
	{
		Point2.y = num2;
		Point2.x = Point.x + (num2 - Point.y) / ENQ;
		Point3.y = num3;
		Point3.x = Point.x + (num3 - Point.y) / ENQ;

		if (!flag1)
		{
			if (num * Fval(Point2.x, d1) <= 0.0)
			{
				break;
			}
		} 
		else
		{
			if (num * Fval(Point3.x, d1) <= 0.0)
			{
				break;
			}
		}

		LineBaseP.BP.x = Point.x;
		LineBaseP.BP.y = Point.y;
		LineBaseP.P1.x = Point2.x;
		LineBaseP.P1.y = Point2.y;
		LineBaseP.P2.x = Point3.x;
		LineBaseP.P2.y = Point3.y;
		pLineBasePList.add(LineBaseP);

		Point.x = Point.x - ACK.x;
		Point.y = Point.y - ACK.y;
	}
}

void ReadLineWithBaseP(SimpleList <LineWithBaseP> &pLineBasePList, 
	HATCHPATTERNDATA &HatchPatternData, 
	const HatchRect &DRect, 
	HatchPatternStyle &PatternStyle)
{
	//this.SOH = false; 局部变量SOH
	REALPOINT empty;
	ZeroMemory(&empty, sizeof(REALPOINT));
	double num = (HatchPatternData.lineAngle) * PI / 180.0;
	bool flag = false;  //局部变量SOH
	REALPOINT dPoint = HatchPatternData.offset; //局部变量ACK
	REALPOINT dPoint2; //局部变量SI
	ZeroMemory(&dPoint2, sizeof(REALPOINT));
	double d1 = tan(num);  //局部变量ENQ

	if (dPoint.x != 0.0f)
	{
		if (abs(PI / 2 - num) >= 1E-06 && abs(dPoint.y/dPoint.x - d1) < 1E-05)
		{
			return;
		}
	} 
	else
	{
		if (abs(PI / 2 - num) < 1E-06 || dPoint.y == 0.0f)
		{
			return;
		}
	}
	double d2 = sin(num);  //局部变量SI
	double d3 = cos(num);  //局部变量DLE
	if (abs(PI / 2 - num) >= 1E-06)
	{
		double d4 = DRect.top - d1 * DRect.left; //局部变量ACK
		if (dPoint.x == 0.0f)
		{
			dPoint2.x = DRect.left;
		}
		else
		{
			dPoint2.x = (HatchPatternData.baseP.y - HatchPatternData.baseP.x * dPoint.y / dPoint.x - d4) / (d1 - dPoint.y / dPoint.x);
		}
		dPoint2.y = d1 * dPoint2.x + d4;
	}
	else
	{
		dPoint2.x = DRect.left;
		dPoint2.y = dPoint.y * dPoint2.x / dPoint.x + HatchPatternData.baseP.y - HatchPatternData.baseP.x * dPoint.y / dPoint.x;
	}
	double d5 = floor(Distance(dPoint2, HatchPatternData.baseP) / Distance(dPoint, empty));   //局部变量SO
	double n = dPoint.x;
	if (Fval(dPoint2.x, HatchPatternData.baseP.x) * n <= 0.0f)
	{
		n = -n;
	}
	HatchPatternData.baseP.x += n * d5;

	double m = dPoint.y;
	if (Fval(dPoint2.y, HatchPatternData.baseP.y) * m <= 0.0f)
	{
		m = -m;
	}
	HatchPatternData.baseP.y += m * d5;

	double d6 = 0.0f;   //局部变量EOT
    if (HatchPatternData.lines != NULL)
    {
		for (int i = 0; i < HatchPatternData.dashNum; i++)
		{
			d6 += abs(HatchPatternData.lines[i]);
		}
    }

	REALPOINT dPoint3;   //局部变量DLE
	REALPOINT dPoint4;   //局部变量DC1
	ZeroMemory(&dPoint3, sizeof(REALPOINT));
	ZeroMemory(&dPoint4, sizeof(REALPOINT));

	if (d6 != 0.0f)
	{
		d5 = floor(Distance(DRect.TopLeft, dPoint2) / d6);
		double n1 = d5 * d6;
		dPoint3.x = HatchPatternData.baseP.x + d3 * n1;
		dPoint3.y = HatchPatternData.baseP.y + d2 * n1;
		dPoint4.x = HatchPatternData.baseP.x - d3 * n1;
		dPoint4.y = HatchPatternData.baseP.y - d2 * n1;
	}
	else
	{
		d6 = Distance(dPoint2, HatchPatternData.baseP);
		double m1 = 1.0 * d6;
		dPoint3.x = HatchPatternData.baseP.x + d3 * m1;
		dPoint3.y = HatchPatternData.baseP.y + d2 * m1;
		dPoint4.x = HatchPatternData.baseP.x - d3 * m1;
		dPoint4.y = HatchPatternData.baseP.y - d2 * m1;
	}

	if (Distance(DRect.TopLeft, dPoint3) < Distance(DRect.TopLeft, dPoint4))
	{
		HatchPatternData.baseP = dPoint3;
	} 
	else
	{
		HatchPatternData.baseP = dPoint4;
	}

	if (abs(d3) < 0.001)
	{
		PatternStyle = HatchPatternStyle_Vertical;
		ReadHVLineWithBaseP(pLineBasePList, 0, DRect.TopLeft, DRect.BottomRight, HatchPatternData.baseP, dPoint);
		return;
	}
	if (abs(d2) < 0.001)
	{
		PatternStyle = HatchPatternStyle_Horizontal;
		ReadHVLineWithBaseP(pLineBasePList, 1, DRect.BottomRight, DRect.TopLeft, HatchPatternData.baseP, dPoint);
		return;
	}
	if ((d2 > 0.0f && d3 > 0.0f) || (d2 < 0.0f && d3 < 0.0f))
	{
		PatternStyle = HatchPatternStyle_FDiagonal;
		bool flag = dPoint.y < dPoint.x * d1;
		ReadBFLineWithBaseP(pLineBasePList, 0, flag, DRect, HatchPatternData.baseP, dPoint, d1);
		return;
	}
	PatternStyle = HatchPatternStyle_BDiagonal;
	bool flag1 = dPoint.y > dPoint.x * d1;
	ReadBFLineWithBaseP(pLineBasePList, 1, flag1, DRect, HatchPatternData.baseP, dPoint, d1);
}

BOOL Point2DValue(const POINT2D &Point, const POINT2D &Point2, const POINT2D &Point3, const POINT2D &Point4)
{
	double num = Atan2(Point, Point3);
	double num2 = Atan2(Point, Point4);
	double num3 = Atan2(Point, Point2);
	double num4;
	if (num3 > 0.0f)
	{
		num4 = num3 - PI;
	}
	else
	{
		num4 = num3 + PI;
	}
	if (num2 < num)
	{
		SwapT(num2, num);
	}
	double num6 = num2 - num;
	BOOL result;
	if (EqualDouble(num6, PI))
	{
		result = true;
	}
	else
	{
		if (num6 < PI)
		{
			result = ((num3 > num && num2 > num3) || (num4 > num && num2 > num4));
		}
		else
		{
			result = (num3 < num || num2 < num3 || num4 < num || num2 < num4);
		}
	}
	return result;
}

void SortPoint2D(vector <POINT2D> &Point2DList, int num, int num2, int id)
{
	int num3 = 0;
	do
	{
		num3 = num;
		int num4 = num2;
		POINT2D Point = Point2DList[(num + num2) >> 1];
		while (true)
		{
			if (Point2DIdCmp(Point2DList[num3], Point, id) >= 0)
			{
				while (Point2DIdCmp(Point2DList[num4], Point, id) > 0)
				{
					num4--;
				}
				if (num3 <= num4)
				{
					SwapT(Point2DList[num3], Point2DList[num4]);
					num3++;
					num4--;
				}
				if (num3 > num4)
				{
					break;
				}
			}
			else
			{
				num3++;
			}
		}
		if (num < num4)
		{
			SortPoint2D(Point2DList, num, num4, id);
		}
		num = num3;
	}
	while (num3 < num2);
}

void ReadParseRealPoint(PBOUNDARIES pBoundaries, int BoundaryPathNum, LineWithBaseP lineWithBaseP, vector <POINT2D> &Point2DList2)
{
	if (pBoundaries == NULL || BoundaryPathNum == 0)
	{
		return;
	}

	vector <POINT2D> Point2DList3;
	double num = -1.0f;
	double num2 = -1.0f;
	bool flag = false;
	bool flag2 = false;
	POINT2D dPoint, dPoint2, dPoint3, dPoint4, dPoint5, dPoint6, dPoint7, dPoint8, dPoint9, dPoint10;
	POINT2D item, p; 
	ZeroMemory(&dPoint, sizeof(POINT2D));
	ZeroMemory(&dPoint2, sizeof(POINT2D));
	ZeroMemory(&dPoint3, sizeof(POINT2D));
	ZeroMemory(&dPoint4, sizeof(POINT2D));
	ZeroMemory(&dPoint5, sizeof(POINT2D));
	ZeroMemory(&dPoint6, sizeof(POINT2D));
	ZeroMemory(&dPoint7, sizeof(POINT2D));
	ZeroMemory(&dPoint8, sizeof(POINT2D));
	ZeroMemory(&dPoint9, sizeof(POINT2D));
	ZeroMemory(&dPoint10, sizeof(POINT2D));
	ZeroMemory(&item, sizeof(POINT2D));
	ZeroMemory(&p, sizeof(POINT2D));

	dPoint.x = lineWithBaseP.P1.x;
	dPoint.y = lineWithBaseP.P1.y;
	dPoint2.x = lineWithBaseP.P2.x;
	dPoint2.y = lineWithBaseP.P2.y;
    unsigned int count = Point2DList2.size();

	for (int i = 0; i < BoundaryPathNum; i++)
	{
		PPOINT2D pPoint2D = pBoundaries[i].pBoundaryPoint;
		int Num = pBoundaries[i].BoundaryPointNum;

		if (pPoint2D != NULL && Num != 0)
		{
			for (int j = 0; j < Num; j++)
			{
				if (j == Num - 1)
				{
					dPoint3 = pPoint2D[j];
					dPoint4 = pPoint2D[0];
				}
				else
				{
					dPoint3 = pPoint2D[j];
					dPoint4 = pPoint2D[j+1];
				}
				dPoint5 = dPoint3;
				dPoint6 = dPoint4;
				double num3 = Fval(dPoint2.x, dPoint.x) * Fval(dPoint6.y, dPoint5.y) - Fval(dPoint2.y, dPoint.y) * Fval(dPoint6.x, dPoint5.x);
				if (num3 != 0.0f)
				{
					num = (Fval(dPoint.y, dPoint5.y) * Fval(dPoint6.x, dPoint5.x) - Fval(dPoint.x, dPoint5.x) * Fval(dPoint6.y, dPoint5.y)) / num3;
					num2 = (Fval(dPoint.y, dPoint5.y) * Fval(dPoint2.x, dPoint.x) - Fval(dPoint.x, dPoint5.x) * Fval(dPoint2.y, dPoint.y)) / num3;
					if (num >= -1E-06 && num <= 1.000001 && num2 >= -1E-06 && num2 <= 1.000001)
					{
						dPoint10.x = dPoint.x + num * (dPoint2.x - dPoint.x);
						dPoint10.y = dPoint.y + num * (dPoint2.y - dPoint.y);
						bool flag3 = false;
						int index = 0;
						int count2 = Point2DList2.size();
						for (int k = 0; k < count2; k++)
						{
							if (EqualPoint2D(dPoint10, Point2DList2[k]))
							{
								flag3 = true;
								index = k;
							}
						}
						if ((EqualPoint2D(dPoint10, dPoint5) || EqualPoint2D(dPoint10, dPoint6)) && (count2 - count >= 1) && (EqualPoint2D(dPoint10, Point2DList2[count2 - 1]) || flag3))
						{
							int num4 = 1;
							if (EqualPoint2D(dPoint10, Point2DList2[count2 - 1]))
							{
								dPoint8 = dPoint3;
								p = dPoint8;
								if (j - num4 >= 0)
								{
									dPoint8 = pPoint2D[j - num4];
									while (EqualPoint2D(dPoint8, p))
									{
										p = dPoint8;
										num4++;
										if (j - num4 < 0)
										{
											break;
										}
										dPoint8 = pPoint2D[j - num4];
									}
								}
								dPoint9 = dPoint4;
							}
							else
							{
								dPoint8 = Point2DList3[index];
								p = dPoint8;
								if (j + num4 < Num - 1)
								{
									dPoint8 = pPoint2D[j + num4];
									while (EqualPoint2D(dPoint8, p))
									{
										p = dPoint8;
										num4++;
										dPoint8 = pPoint2D[j + num4];
									}
								}
								dPoint9 = dPoint3;
							}
							dPoint7 = dPoint2;
							if (EqualPoint2D(dPoint2, dPoint10))
							{
								dPoint7 = dPoint;
							}
							if (Point2DValue(dPoint10, dPoint7, dPoint8, dPoint9))
							{
								goto IL_5AB;
							}
						}
						if (flag2 && !EqualDouble(dPoint10.y, Point2DList2[count2 - 1].y))
						{
							flag = true;
						}
						item = dPoint10;
						Point2DList2.push_back(item);
						Point2DList3.push_back(dPoint4);
						flag2 = true;
					}
				}
				IL_5AB:;
			}
		}
	}

	if (Point2DList2.size() > 0 && count < Point2DList2.size() - 1)
	{
		SortPoint2D(Point2DList2, count, Point2DList2.size() - 1, 0);
		if (flag)
		{
			SortPoint2D(Point2DList2, count, Point2DList2.size() - 1, 1);
		}
	}
	Point2DList3.clear();
}

void AddHatchLinePoint(const REALPOINT &dPoint, double num, double num2, double num3, double num4, vector <REALPOINT> &RealPointList, int &ptr, int num5)
{
	REALPOINT Point;
	Point.x = dPoint.x + num3 * num / num2;
	Point.y = dPoint.y + num4 * num / num2;
	Point.z = 0.0f;
	RealPointList.push_back(Point);

	ptr++;
	if (ptr > num5 - 1)
	{
		ptr = 0;
	}
}

void ReadHatchLine(double *array, int count, const REALPOINT &dPoint, REALPOINT dPoint2, REALPOINT dPoint3, vector <REALPOINT> &DottedSingPts)
{
	 assert(DottedSingPts.size() == 0);
	 assert(array != NULL);

	 int num = 0;
	 int num2 = count >> 1 << 1;
	 REALPOINT dPoint4 = dPoint;
	 double num3 = 0.0f;
	 for (int i = 0; i < num2; i++)
	 {
		 num3 += abs(array[i]);
	 }
	 double num4 = dPoint2.x - dPoint4.x;
	 double num5 = dPoint2.y - dPoint4.y;
	 double num6 = sqrt(Sqr(num4) + Sqr(num5));
	 if (abs(num6) < 9.9999997473787516E-05 || num6 / num3 > 1000.0)
	 {
		 if (num6 == 0.0f)
		 {
			 dPoint2 = dPoint4;
		 }
		 DottedSingPts.push_back(dPoint4);
		 DottedSingPts.push_back(dPoint2);
		 return;
	 }
	 float num7;
	 if (dPoint4.x != dPoint2.x)
	 {
		 num7 = (float)atan((dPoint2.y - dPoint4.y) / (dPoint2.x - dPoint4.x));
	 }
	 else
	 {
		 num7 = 1.57079637f;
	 }
	 float num8 = 0.0f;
	 if (abs(dPoint3.y - dPoint4.y) >= 1E-06 || abs(dPoint3.y - dPoint2.y) >= 1E-06)
	 {
		 if (dPoint4.y < dPoint2.y)
		 {
			 num8 = (float)sqrt(Sqr(dPoint4.x - dPoint3.x) + Sqr(dPoint4.y - dPoint3.y));
		 }
		 else
		 {
			 num8 = (float)sqrt(Sqr(dPoint2.x - dPoint3.x) + Sqr(dPoint2.y - dPoint3.y));
		 }
		 if (dPoint3.y > dPoint4.y || dPoint3.y > dPoint2.y)
		 {
			 int num9 = Round((double)num8 / num3) + 1;
			 if (abs((double)num7 - 1.5707963267948966) > 1E-06)
			 {
				 if (tan((double)num7) >= 0.0)
				 {
					 dPoint3.x = (double)((float)(dPoint3.x - num3 * (double)num9 * cos((double)num7)));
					 dPoint3.y = (double)((float)(dPoint3.y - num3 * (double)num9 * sin((double)num7)));
				 }
				 else
				 {
					 dPoint3.x = (double)((float)(dPoint3.x + num3 * (double)num9 * cos((double)num7)));
					 dPoint3.y = (double)((float)(dPoint3.y + num3 * (double)num9 * sin((double)num7)));
				 }
			 }
			 else
			 {
				 dPoint3.y -= num3 * (double)num9;
			 }
		 }
		 else
		 {
			 int num9 = Round((double)num8 / num3);
			 if (abs((double)num7 - 1.5707963267948966) > 1E-06)
			 {
				 if (tan((double)num7) >= 0.0)
				 {
					 dPoint3.x = (double)((float)(dPoint3.x + num3 * (double)num9 * cos((double)num7)));
					 dPoint3.y = (double)((float)(dPoint3.y + num3 * (double)num9 * sin((double)num7)));
				 }
				 else
				 {
					 dPoint3.x = (double)((float)(dPoint3.x - num3 * (double)num9 * cos((double)num7)));
					 dPoint3.y = (double)((float)(dPoint3.y - num3 * (double)num9 * sin((double)num7)));
				 }
			 }
			 else
			 {
				 dPoint3.y += num3 * (double)num9;
			 }
		 }
	 }
	 else
	 {
		 if (dPoint4.x < dPoint2.x)
		 {
			 num8 = (float)sqrt(Sqr(dPoint4.x - dPoint3.x) + Sqr(dPoint4.y - dPoint3.y));
		 }
		 else
		 {
			 num8 = (float)sqrt(Sqr(dPoint2.x - dPoint3.x) + Sqr(dPoint2.y - dPoint3.y));
		 }
		 if (dPoint3.x > dPoint4.x || dPoint3.y > dPoint2.y)
		 {
			 int num9 = Round((double)num8 / num3) + 1;
			 dPoint3.x -= num3 * (double)num9;
		 }
		 else
		 {
			 int num9 = Round((double)num8 / num3);
			 dPoint3.x += num3 * (double)num9;
		 }
	 }
	 num8 = (float)sqrt(Sqr(dPoint4.x - dPoint3.x) + Sqr(dPoint4.y - dPoint3.y));
	 float num10 = (float)sqrt(Sqr(dPoint2.x - dPoint3.x) + Sqr(dPoint2.y - dPoint3.y));
	 if (num8 > num10)
	 {
		 num8 = num10;
		 REALPOINT dPoint5 = dPoint4;
		 dPoint4 = dPoint2;
		 dPoint2 = dPoint5;
		 num4 = (double)((float)(dPoint2.x - dPoint4.x));
		 num5 = (double)((float)(dPoint2.y - dPoint4.y));
	 }
	 double num11 = 0.0f;
	 if (num3 != (double)num8)
	 {
		 int i = -1;
		 float num12 = 0.0f;
		 while (true)
		 {
			 i++;
			 if (i > num2 - 1)
			 {
				 break;
			 }
			 num12 += (float)abs(array[i]);
			 if (num12 >= num8)
			 {
				 goto IL_2DA;
			 }
		 }
		 return;
         IL_2DA:;
		 if (array[i] >= 0.0)
		 {
			 AddHatchLinePoint(dPoint4, num11, num6, num4, num5, DottedSingPts, num, num2);
		 }
		 num11 = (double)(num12 - num8);
		 if (num11 >= num6)
		 {
			 num11 = num6;
			 if (array[i] >= 0.0)
			 {
				 AddHatchLinePoint(dPoint4, num11, num6, num4, num5, DottedSingPts, num, num2);
			 }
			 return;
		 }
		 if ((i == num2 - 1 || array[i] * array[i + 1] <= 0.0) && (i != num2 - 1 || array[i] * array[0] <= 0.0))
		 {
			 AddHatchLinePoint(dPoint4, num11, num6, num4, num5, DottedSingPts, num, num2);
		 }
		 for (i++; i <= num2 - 1; i++)
		 {
			 num11 += abs(array[i]);
			 if (num11 >= num6)
			 {
				 num11 = num6;
				 if (array[i] >= 0.0)
				 {
					 AddHatchLinePoint(dPoint4, num11, num6, num4, num5, DottedSingPts, num, num2);
				 }
				 return;
			 }
			 if ((i == num2 - 1 || array[i] * array[i + 1] <= 0.0) && (i != num2 - 1 || array[i] * array[0] <= 0.0))
			 {
				 AddHatchLinePoint(dPoint4, num11, num6, num4, num5, DottedSingPts, num, num2);
			 }
		 }
	 }
	 else
	 {
		 if (array[0] >= 0.0)
		 {
			 AddHatchLinePoint(dPoint4, num11, num6, num4, num5, DottedSingPts, num, num2);
		 }
	 }
	 num = 0;
	 while (true)
	 {
		 num11 += abs(array[num]);
		 if (abs(num11 - num6) < 1E-06 || num11 > num6)
		 {
			 break;
		 }
		 if ((num == num2 - 1 || array[num] * array[num + 1] <= 0.0) && (num != num2 - 1 || array[num] * array[0] <= 0.0))
		 {
			 AddHatchLinePoint(dPoint4, num11, num6, num4, num5, DottedSingPts, num, num2);
		 }
		 else
		 {
			 num++;
			 if (num > num2 - 1)
			 {
				 num = 0;
			 }
		 }
	 }
	 num11 = num6;
	 if (array[num] >= 0.0)
	 {
		AddHatchLinePoint(dPoint4, num11, num6, num4, num5, DottedSingPts, num, num2);
	 }
}

void ReadHatchParseLines(PDXFENTHATCH pHatch)
{
	PHATCHPATTERNDATA pHatchPattern = pHatch->pHatchPatternData;
	int LineNum = pHatch->LineNum;

	if (pHatchPattern == NULL || LineNum == 0)
	{
		return;
	}
	
	REALPOINT dPoint, dPoint2, basePt, item, item2;
	ZeroMemory(&dPoint, sizeof(REALPOINT));
	ZeroMemory(&dPoint2, sizeof(REALPOINT));
	ZeroMemory(&basePt, sizeof(REALPOINT));
	ZeroMemory(&item, sizeof(REALPOINT));
	ZeroMemory(&item2, sizeof(REALPOINT));

	if (pHatch->pParseLines != NULL)
	{
		delete[] pHatch->pParseLines;
		pHatch->ParseLinesNum = 0;
		pHatch->pParseLines = NULL;
	}

	//当前SimpleList链表不支持逆序(以后改进)
	SimpleList <REALPOINT> ParseLinesList;
	SimpleList <LineWithBaseP> LineBasePList;  //局部变量EXT 
	vector <REALPOINT> DottedSingPts;
	vector <LineWithBaseP> LineBasePList2;
	vector <POINT2D> Point2DList;

	LineWithBaseP LineBaseP;
	ZeroMemory(&LineBaseP, sizeof(LineWithBaseP));

	for (int i = 0; i < LineNum; i++)
	{
		LineBasePList.clear();
		LineBasePList2.clear();
		ReadLineWithBaseP(LineBasePList, pHatchPattern[i], pHatch->DRect, pHatch->PatternStyle);
		int count = LineBasePList.getSize();
		PLineWithBaseP pLineBaseP = ReadSimpleListData(LineBasePList);   //当前链表不支持逆序(以后再改)
		for (int j = 0; j < count; j++)
		{
			Point2DList.clear();
			ReadParseRealPoint(pHatch->pBoundaries, pHatch->BoundaryPathNum, pLineBaseP[j], Point2DList);
			int size = Point2DList.size();
			for (int k = 0; k <= size - 2; k += 2)
		    {
				LineBaseP.P1.x = Point2DList[k].x;
				LineBaseP.P1.y = Point2DList[k].y;
				LineBaseP.P2.x = Point2DList[k+1].x;
				LineBaseP.P2.y = Point2DList[k+1].y;
				LineBaseP.BP.x = pLineBaseP[j].BP.x;
				LineBaseP.BP.y = pLineBaseP[j].BP.y;
				LineBasePList2.push_back(LineBaseP);
		    }
		}
		if (pHatchPattern[i].isDash)
		{
			if (LineBasePList2.size() != 0)
			{
				int num = (int)floor((double)(pHatchPattern[i].lineAngle / 360.0f));
				if (pHatchPattern[i].lineAngle - (float)num * 360.0f >= 180.0f)
				{
					int num = pHatchPattern[i].dashNum;
					for (int n = 0; n <  num / 2; n++)
					{
						double tmp = pHatchPattern[i].lines[n];
						pHatchPattern[i].lines[n] = pHatchPattern[i].lines[num-n-1];
						pHatchPattern[i].lines[num-n-1] = tmp;
					}
				}
				for (unsigned int l = 0; l < LineBasePList2.size(); l++)
				{
					DottedSingPts.clear();
					dPoint.x = LineBasePList2[l].P1.x;
					dPoint.y = LineBasePList2[l].P1.y;
					dPoint.z = 0.0;
					dPoint2.x = LineBasePList2[l].P2.x;
					dPoint2.y = LineBasePList2[l].P2.y;
					dPoint2.z = 0.0;
					basePt.x = LineBasePList2[l].BP.x;
					basePt.y = LineBasePList2[l].BP.y;
					basePt.z = 0.0;
					if (pHatchPattern[i].dashNum > 1)
					{
						ReadHatchLine(pHatchPattern[i].lines, pHatchPattern[i].dashNum, dPoint, dPoint2, basePt, DottedSingPts);
						for (unsigned int m = 0; m < DottedSingPts.size(); m++)
						{
							ParseLinesList.add(DottedSingPts[m]);
						}
					} 
					else
					{
						item2 = dPoint;
						ParseLinesList.add(item2);
						item2 = dPoint2;
						ParseLinesList.add(item2);
					}
				}
			}
		}
		else
		{
			for (unsigned int l = 0; l < LineBasePList2.size(); l++)
			{
				item.x = LineBasePList2[l].P1.x;
				item.y = LineBasePList2[l].P1.y;
				item.z = 0.0;
				ParseLinesList.add(item);

				item.x = LineBasePList2[l].P2.x;
				item.y = LineBasePList2[l].P2.y;
				item.z = 0.0;
				ParseLinesList.add(item);
			}
		}
		if (pLineBaseP != NULL)
		{
			delete[] pLineBaseP;
			pLineBaseP = NULL;
		}
	}

	pHatch->ParseLinesNum = ParseLinesList.getSize();
	pHatch->pParseLines = ReadSimpleListData(ParseLinesList);

	ParseLinesList.clear();
	LineBasePList.clear();
	LineBasePList2.clear();
	DottedSingPts.clear();
	Point2DList.clear();
}

//读取Hatch矩形区域
void CloseBoundary(PBOUNDARIES pBoundaries, int Num)
{
	for (int i = 0; i < Num; i++)
	{
		if (pBoundaries != NULL)
		{
			PPOINT2D pBoundaryPoint = pBoundaries->pBoundaryPoint;
			int Count = pBoundaries->BoundaryPointNum;
			if (pBoundaryPoint != NULL &&  Count > 0)
			{
				POINT2D Point, Point2;
				ZeroMemory(&Point, sizeof(POINT2D));
				ZeroMemory(&Point2, sizeof(POINT2D));
				Point = pBoundaryPoint[0];
				Point2 = pBoundaryPoint[Count - 1];
				if (abs(Point.x - Point2.x) > 1E-06 && abs(Point.y - Point2.y) > 1E-06 && Count == 2)
				{
					PPOINT2D p = new POINT2D[Count+1];
					for (int j = 0; j < Count; j++)
					{
						p[j] = pBoundaryPoint[j];
					}
					p[Count] = pBoundaryPoint[0];

					delete[] pBoundaries->pBoundaryPoint;
					pBoundaries->pBoundaryPoint = p;
					pBoundaries->BoundaryPointNum = Count+1;
				}
			}
		}
		pBoundaries++;
	}
}

void InitHatchRect(PDXFENTHATCH pHatch)
{
	pHatch->DRect.left = 1E+20;
	pHatch->DRect.top = -1E+20;
	pHatch->DRect.z1 = 0.0f;
	pHatch->DRect.right = -1E+20;
	pHatch->DRect.bottom = 1E+20;
	pHatch->DRect.z2 = 0.0f;
	pHatch->DRect.TopLeft.x = pHatch->DRect.left;
	pHatch->DRect.TopLeft.y = pHatch->DRect.top;
	pHatch->DRect.TopLeft.z = pHatch->DRect.z1;
	pHatch->DRect.BottomRight.x = pHatch->DRect.right;
	pHatch->DRect.BottomRight.y = pHatch->DRect.bottom;
	pHatch->DRect.BottomRight.z = pHatch->DRect.z2;
	pHatch->DRect.width = -2E+20;
	pHatch->DRect.height = 2E+20;
}

void ReadHatchRect(PDXFENTHATCH pHatch)
{
	PBOUNDARIES pBoundaries = pHatch->pBoundaries;

	for (int i = 0; i < pHatch->BoundaryPathNum; i++)
	{
		if (pBoundaries != NULL)
		{
			PPOINT2D pRealPoint = pBoundaries->pBoundaryPoint;
			int Count = pBoundaries->BoundaryPointNum;
			if (pRealPoint != NULL && Count > 0)
			{
				for (int j = 0; j < Count; j++)
				{
					if (pHatch->DRect.left > pRealPoint[j].x)
					{
						pHatch->DRect.left = pRealPoint[j].x;
					}
					if (pHatch->DRect.top <pRealPoint[j].y)
					{
						pHatch->DRect.top = pRealPoint[j].y;
					}
					if (pHatch->DRect.right < pRealPoint[j].x)
					{
						pHatch->DRect.right = pRealPoint[j].x;
					}
					if (pHatch->DRect.bottom > pRealPoint[j].y)
					{
						pHatch->DRect.bottom = pRealPoint[j].y;
					}
				}
			}
		}
		pBoundaries++;
	}

	if (pHatch->DRect.left > pHatch->DRect.right)
	{
		SwapT(pHatch->DRect.left, pHatch->DRect.right);
	}
	if (pHatch->DRect.bottom > pHatch->DRect.top)
	{
		SwapT(pHatch->DRect.bottom, pHatch->DRect.top);
	}

	pHatch->DRect.TopLeft.x = pHatch->DRect.left;
	pHatch->DRect.TopLeft.y = pHatch->DRect.top;
	pHatch->DRect.TopLeft.z = pHatch->DRect.z1;
	pHatch->DRect.BottomRight.x = pHatch->DRect.right;
	pHatch->DRect.BottomRight.y = pHatch->DRect.bottom;
	pHatch->DRect.BottomRight.z = pHatch->DRect.z2;
	pHatch->DRect.width = abs(pHatch->DRect.left - pHatch->DRect.right);
	pHatch->DRect.height = abs(pHatch->DRect.top - pHatch->DRect.bottom);
	pHatch->DRect.Center.x = (pHatch->DRect.left + pHatch->DRect.right)/2;
	pHatch->DRect.Center.y = (pHatch->DRect.top + pHatch->DRect.bottom)/2;
	pHatch->DRect.Center.z = (pHatch->DRect.z1 + pHatch->DRect.z2)/2;
}

//从Dxf中读Hatch数据
BOOL ReadHatchBoundaryData( PDXF pDxf, PHATCHBOUNDARYDATA pHatchBoundaryData, int BoundaryPathNum )
{
	dxfStorePos(pDxf);
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	int nIndex = 0;
	while (nIndex < BoundaryPathNum) //读边界次数
	{
		SimpleList <PHATCHCURVEDATA> *pHatchList = new SimpleList <PHATCHCURVEDATA>();
		HATCHCURVEDATA *pHatchCurve = NULL;

		while (GCode!=0 && GCode!=75)
		{
			switch (GCode)
			{
			case 92:   //边界路径标志
				{
					int Flag = atoi(strValue);
					pHatchBoundaryData[nIndex].BoundaryMark = Flag;

					if ((Flag & 2) == 0)
					{
						while (GCode!=0 && GCode!=75)
						{
							if (GCode == 97)
							{
								goto IL_2DA;
							}
							switch(GCode)
							{
							case 93:   //边界路径的边数
								pHatchBoundaryData[nIndex].BoundaryNum = atoi(strValue);
								break;
							case 72:   //边界类型
								{
									HatchBoundaryType Type = (HatchBoundaryType)atoi(strValue);
									dxfStorePos(pDxf);
									ReadParamFromDxfFile(pDxf, GCode, strValue);
									switch (Type)
									{
									case HatchPathType_Line:  //直线边界
										{
											pHatchCurve = new HATCHLINEDATA();
											ZeroMemory(pHatchCurve, sizeof(HATCHLINEDATA));
											while (GCode!=72 && GCode!=75 && GCode!=97 && GCode!=0)
											{
												switch (GCode)
												{
												case 10:
													pHatchCurve->StartPoint.x = atof(strValue);
													break;
												case 20:
													pHatchCurve->StartPoint.y = atof(strValue);
													break;
												case 11:
													pHatchCurve->EndPoint.x = atof(strValue);
													break;
												case 21:
													pHatchCurve->EndPoint.y = atof(strValue);
													break;
												}
												dxfStorePos(pDxf);
												ReadParamFromDxfFile(pDxf, GCode, strValue);
											}
											((PHATCHLINEDATA)pHatchCurve)->EdgeType = HatchPathType_Line;
											pHatchList->add(pHatchCurve);
										}
										break;
									case HatchPathType_Circle:  //圆弧边界
										{
											pHatchCurve = new HATCHARCDATA();
											ZeroMemory(pHatchCurve, sizeof(HATCHARCDATA));
											while (GCode!=72 && GCode!=75 && GCode!=97 && GCode!=0)
											{
												switch (GCode)
												{
												case 10:
													((PHATCHARCDATA)pHatchCurve)->Point0.x = atof(strValue);
													break;
												case 20:
													((PHATCHARCDATA)pHatchCurve)->Point0.y = atof(strValue);
													break;
												case 40:
													((PHATCHARCDATA)pHatchCurve)->Radius = atof(strValue);
													break;
												case 50:
													((PHATCHARCDATA)pHatchCurve)->StartAngle = atof(strValue);
													break;
												case 51:
													((PHATCHARCDATA)pHatchCurve)->EndAngle = atof(strValue);
													break;
												case 73:
													((PHATCHARCDATA)pHatchCurve)->AnticlockWiseFlag = atoi(strValue);
													break;
												}
												dxfStorePos(pDxf);
												ReadParamFromDxfFile(pDxf, GCode, strValue);
											}
											((PHATCHARCDATA)pHatchCurve)->EdgeType = HatchPathType_Circle;
											pHatchList->add(pHatchCurve);
										}
										break;
									case HatchPathType_Ellipse:  //椭圆弧边界
										{
											pHatchCurve = new HATCHELLIPSEDATA();
											ZeroMemory(pHatchCurve, sizeof(HATCHELLIPSEDATA));
											while (GCode!=72 && GCode!=75 && GCode!=97 && GCode!=0)
											{
												switch (GCode)
												{
												case 10:
													((PHATCHELLIPSEDATA)pHatchCurve)->CenterPoint.x = atof(strValue);
													break;
												case 20:
													((PHATCHELLIPSEDATA)pHatchCurve)->CenterPoint.y = atof(strValue);
													break;
												case 11:
													((PHATCHELLIPSEDATA)pHatchCurve)->MajorAxisEndPoint.x = atof(strValue);
													break;
												case 21:
													((PHATCHELLIPSEDATA)pHatchCurve)->MajorAxisEndPoint.y = atof(strValue);
													break;
												case 40:
													((PHATCHELLIPSEDATA)pHatchCurve)->MinorToMajorRatio = atof(strValue);
													break;
												case 50:
													((PHATCHELLIPSEDATA)pHatchCurve)->StartParam = atof(strValue);
													break;
												case 51:
													((PHATCHELLIPSEDATA)pHatchCurve)->EndParam = atof(strValue);
													break;
												case 73:
													((PHATCHELLIPSEDATA)pHatchCurve)->AnticlockWiseFlag = atoi(strValue);
													break;
												}
												dxfStorePos(pDxf);
												ReadParamFromDxfFile(pDxf, GCode, strValue);
											}
											((PHATCHELLIPSEDATA)pHatchCurve)->EdgeType = HatchPathType_Ellipse;
											pHatchList->add(pHatchCurve);
										}
										break;
									case HatchPathType_Spline:  //样条曲线边界
										{
											pHatchCurve = new HATCHSPLINEDATA();
											ZeroMemory(pHatchCurve, sizeof(HATCHSPLINEDATA));
											SimpleList<POINT2D> *ControlVertices=new SimpleList<POINT2D>();
											SimpleList<POINT2D> *FittingVertices=new SimpleList<POINT2D>();
											POINT2D ControlVertex;
											POINT2D FittingVertex;
											ZeroMemory(&ControlVertex, sizeof(POINT2D));
											ZeroMemory(&FittingVertex, sizeof(POINT2D));

											int i = 0;
											bool flag = true;
											while (GCode!=72 && GCode!=75 && GCode!=0 && flag)
											{
												switch (GCode)
												{
												case 94:
													((PHATCHSPLINEDATA)pHatchCurve)->SplineOrder = atoi(strValue);
													break;
												case 73:
													((PHATCHSPLINEDATA)pHatchCurve)->SplineMarks = atoi(strValue);
													break;
												case 74:
													break;
												case 95:
													{
														int Num = atoi(strValue);
														((PHATCHSPLINEDATA)pHatchCurve)->NodeNum = Num;
														((PHATCHSPLINEDATA)pHatchCurve)->pNodePoint = new double[Num];
													}
													break;
												case 96:
													((PHATCHSPLINEDATA)pHatchCurve)->ControlNodeNum= atoi(strValue);
													break;
												case 40:
													((PHATCHSPLINEDATA)pHatchCurve)->pNodePoint[i] = atof(strValue);
													i++;
													break;
												case 10:
													ControlVertex.x = atof(strValue);
													break;
												case 20:
													ControlVertex.y = atof(strValue);
													ControlVertices->add(ControlVertex);
													break;
												case 42:
													((PHATCHSPLINEDATA)pHatchCurve)->Weight = atof(strValue);
													break;
												case 97:
													((PHATCHSPLINEDATA)pHatchCurve)->FittingPointNum = atoi(strValue);
													flag = false;
													break;
												case 11:
													FittingVertex.x = atof(strValue);
													break;
												case 21:
													FittingVertex.y = atof(strValue);
													FittingVertices->add(FittingVertex);
													break;
												case 12:
													((PHATCHSPLINEDATA)pHatchCurve)->StartTangent.x = atof(strValue);
													break;
												case 22:
													((PHATCHSPLINEDATA)pHatchCurve)->StartTangent.y = atof(strValue);
													break;
												case 13:
													((PHATCHSPLINEDATA)pHatchCurve)->EndTangent.x = atof(strValue);
													break;
												case 23:
													((PHATCHSPLINEDATA)pHatchCurve)->EndTangent.y = atof(strValue);
													break;
												}
												dxfStorePos(pDxf);
												ReadParamFromDxfFile(pDxf, GCode, strValue);
											}
											((PHATCHSPLINEDATA)pHatchCurve)->pControlNode = ControlVertices;
											((PHATCHSPLINEDATA)pHatchCurve)->pFittingNode = FittingVertices;
											((PHATCHSPLINEDATA)pHatchCurve)->EdgeType = HatchPathType_Spline;
											pHatchList->add(pHatchCurve);
										}
										break;
									default:
										if (pHatchCurve != NULL)
										{
											delete pHatchCurve;
											pHatchCurve = NULL;
										}
										break;
									}
									goto IL_2AD;
								}
								break;
							}
							dxfStorePos(pDxf);
							ReadParamFromDxfFile(pDxf, GCode, strValue);
							IL_2AD:;
						}
					} 
					else
					{
						int i = -1;
						pHatchCurve = new HATCHPOLYLINEDATA();
						ZeroMemory(pHatchCurve, sizeof(HATCHPOLYLINEDATA));
						while (GCode!=0 && GCode!=75 && GCode!=97)
						{
							switch(GCode)
							{
							case 72:
								break;
							case 73:
								((PHATCHPOLYLINEDATA)pHatchCurve)->Flag = atoi(strValue);
								break;
							case 93:
								{
									int nVertex = atoi(strValue);
									((PHATCHPOLYLINEDATA)pHatchCurve)->nVertex = nVertex;
									((PHATCHPOLYLINEDATA)pHatchCurve)->pVertex = new DXFENTVERTEX2D[nVertex];
									memset(((PHATCHPOLYLINEDATA)pHatchCurve)->pVertex, 0, sizeof(DXFENTVERTEX2D) * nVertex);
								}
								break;
							case 10:
								i++;
								((PHATCHPOLYLINEDATA)pHatchCurve)->pVertex[i].Point.x = atof(strValue);
								break;
							case 20:
								((PHATCHPOLYLINEDATA)pHatchCurve)->pVertex[i].Point.y = atof(strValue);
								break;
							case 42:
								((PHATCHPOLYLINEDATA)pHatchCurve)->pVertex[i].Bulge = atof(strValue);
								break;
							}
							dxfStorePos(pDxf);
							ReadParamFromDxfFile(pDxf, GCode, strValue);
						}
						((PHATCHPOLYLINEDATA)pHatchCurve)->EdgeType = HatchPathType_PolyLine;
						pHatchList->add(pHatchCurve);
						goto IL_2DA;
					}
				}
				break;
			case 97:   //源边界对象数
				pHatchBoundaryData[nIndex].BoundaryObjectNum = atoi(strValue);
				goto IL_DA2;
				break;
			}
			dxfStorePos(pDxf);
			ReadParamFromDxfFile(pDxf, GCode, strValue);
			IL_2DA:;
		}
		IL_DA2:;

		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);

		pHatchBoundaryData[nIndex].pHatchCurveList = pHatchList;
		nIndex++;
	}

	return TRUE;
}

BOOL ReadHatchPatternData( PDXF pDxf, PHATCHPATTERNDATA pHatchPatternData )
{
	int i = -1; 
    int j = 0;

	dxfStorePos(pDxf);
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while (GCode != 0)
	{
		switch (GCode)
		{
		case 53:
			{
				i++;
				pHatchPatternData[i].lineAngle = atof(strValue);
			}
			break;
		case 43:
			pHatchPatternData[i].baseP.x = atof(strValue);
			break;
		case 44:
			pHatchPatternData[i].baseP.y = atof(strValue);
			break;
		case 45:
			pHatchPatternData[i].offset.x = atof(strValue);
			break;
		case 46:
			pHatchPatternData[i].offset.y = atof(strValue);
			break;
		case 79:
			{
				int Num = atoi(strValue);
				pHatchPatternData[i].dashNum = Num;
				if (Num == 0)
				{
					pHatchPatternData[i].lines = NULL;
				}
				else
				{
					pHatchPatternData[i].lines = new double[Num];
					memset(pHatchPatternData[i].lines, 0 , sizeof(double) * Num);
				}
				pHatchPatternData[i].isDash = (Num > 0);
				j = 0;
			}
			break;
		case 49:
			pHatchPatternData[i].lines[j] = atof(strValue);
			j++;
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	return TRUE;
}
 
BOOL ReadHatchData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTHATCH pHatch )
{
	ZeroMemory(pHatch, sizeof(DXFENTHATCH));
	pEntityHeader->EntityType = ENT_HATCH;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	//初始化Hatch矩形边界
	InitHatchRect(pHatch);
	int i = 0;
	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 2:     // Hatch Name
			strcpy_s(pHatch->HatchName, strValue);
			break;
		case 70:   // Hatch Type
			pHatch->HatchType = atoi(strValue);
			break;
		case 71:   // Hatch Ref Mark
			pHatch->HatchRefFlag = atoi(strValue);
			break;
		case 91:   //Hatch BoundaryPath Num
			{
				int Num = atoi(strValue);
				pHatch->BoundaryPathNum = Num;
				pHatch->pHatchBoundaryData = new HATCHBOUNDARYDATA[Num];
				memset(pHatch->pHatchBoundaryData, 0, sizeof(HATCHBOUNDARYDATA) * Num);
				ReadHatchBoundaryData(pDxf, pHatch->pHatchBoundaryData, Num);
				goto IL_2DA;
			}
			break;
		case 75:   //Hatch Style
			pHatch->HatchStyle = atoi(strValue);
			break;
		case 76:   //Pattern Type
			pHatch->PatternType = atoi(strValue);
			break;
		case 52:   //Pattern Angle
			pHatch->PatternFillAngle = atof(strValue);
			break;
		case 41:   //Pattern Scale
			pHatch->PatternFillScale = atof(strValue);
			break;
		case 73:   //Boundary Annotation
			pHatch->BoundaryAnnotation = atoi(strValue);
			break;
		case 77:   //Bi-Directional Mark
			pHatch->BidirectionalMark = atoi(strValue);
			break;
		case 78:   //Define Line num
			{
				int Num = atoi(strValue);
				pHatch->LineNum = Num;
				pHatch->pHatchPatternData = new HATCHPATTERNDATA[Num];
				memset(pHatch->pHatchPatternData, 0, sizeof(HATCHPATTERNDATA) * Num);
				ReadHatchPatternData(pDxf, pHatch->pHatchPatternData);
				goto IL_2DA;
			}
			break;
		case 450:  //Solid Pattern Type
			pHatch->pHatchSolidData = new HATCHSOLIDDATA();
			pHatch->pHatchSolidData->SolidPatternType = atoi(strValue);
			break;
		case 452:  //Color Ref Type
			pHatch->pHatchSolidData->ColorRefType = atoi(strValue);
			break;
		case 453:  //Color Num
			pHatch->pHatchSolidData->ColorNum = atoi(strValue);
			break;
		case 460:  //Color Angle
			pHatch->pHatchSolidData->ColorAngle = atof(strValue);
			break;
		case 461:  //Gradient Ref
			pHatch->pHatchSolidData->GradientRef = (atoi(strValue) ^ 1);
			break;
		case 462:  //Hue
			pHatch->pHatchSolidData->Hue = atof(strValue);
			break;
		case 421:  //Color Value
			pHatch->pHatchSolidData->GradientColor[i]= atoi(strValue);
			i++;
			break;
		case 470:  //Gradient Name
			strcpy_s(pHatch->pHatchSolidData->GradientName, strValue);
			break;
		default:
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
		IL_2DA:;
	}

	ReadBoundaryPolyLines(pHatch);
	CloseBoundary(pHatch->pBoundaries, pHatch->BoundaryPathNum);
	ReadHatchRect(pHatch);
	ReadHatchParseLines(pHatch);

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadHelixData                                                    |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTHELIX pHelix = pointer to Helix structure             |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
double Diff(int num, double *array)
{
	assert(array != NULL && num >= 0);
	return array[num + 1] - array[num];
}

void ParsePointCoords(PDXFENTHELIX pSpline, 
	int num,
	int num2, 
	bool flag,
	bool flag2,
	int num3,
	double *ptr,
	double *ptr2,
	double *ptr3,
	double *ptr4,
	int num4,
	double SOH,
	PREALPOINT p)
{
	ptr3[0] = 0.0f;
	ptr4[0] = 0.0f;
	ptr3[1] = -0.5f * (double)ToInt32(flag);
	if (flag)
	{
		ptr4[1] = 1.5f * (GetPointValue(p[1], num) - GetPointValue(p[0], num)) / Diff(0, ptr);
	}
	else
	{
		ptr4[1] = GetPointValue(pSpline->StartTangent, num) * SOH;
	}
	for (int i = 1; i < num2; i++)
	{
		double num5, num6, num7;
		if (i != num2 - 1)
		{
			num5 = Diff(i, ptr) / (ptr[i + 1] - ptr[i - 1]);
			num6 = Diff(i - 1, ptr) / (ptr[i + 1] - ptr[i - 1]);
			num7 = 3.0f * (num6 * (GetPointValue(p[i + 1], num) - GetPointValue(p[i], num)) / Diff(i, ptr) + num5 * (GetPointValue(p[i], num) - GetPointValue(p[i - 1], num)) / Diff(i - 1, ptr));
		}
		else
		{
			num6 = 0.0f;
			num5 = (double)ToInt32(flag2);
			if (flag2)
			{
				num7 = 3.0f * (GetPointValue(p[num2 - 1], num) - GetPointValue(p[num2 - 2], num)) / Diff(num2 - 2, ptr);
			} 
			else
			{
				num7 = 2.0f * GetPointValue(pSpline->EndTangent, num) * SOH;
			}
		}
		ptr3[i + 1] = num6 / (-2.0 - num5 * ptr3[i]);
		ptr4[i + 1] = (num5 * ptr4[i] - num7) / (-2.0 - num5 * ptr3[i]);
	}
	ptr3[num2] = 0.0f;
	ptr2[num2] = 0.0f;
	for (int i = num2 - 1; i >= 0; i--)
	{
		ptr2[i] = ptr3[i + 1] * ptr2[i + 1] + ptr4[i + 1];
	}

	REALPOINT value;
	for (int i = 0; i < num2 - 1; i++)
	{
		for (int j = 0; j < num4; j++)
		{
			double n = Distance(p[i + 1], p[i]) / SOH * (double)j / (double)num4;
			double num8 = n / Diff(i, ptr);
			value = *(pSpline->pCoordPoint + i * num4 + j);
			n = GetPointValue(p[i], num) * Sqr(1.0f - num8) * (1.0f + 2.0f * num8) + GetPointValue(p[i + 1], num) * Sqr(num8) * (3.0f - 2.0f * num8) + ptr2[i] * Diff(i, ptr) * num8 * Sqr(1.0f - num8) - ptr2[i + 1] * Diff(i, ptr) * Sqr(num8) * (1.0f - num8);
			SetPointValue(value, num, n);
			*(pSpline->pCoordPoint + i * num4 + j) = value;
		}
	}

	value = *(pSpline->pCoordPoint + num3 - 1);
	SetPointValue(value, num, GetPointValue(p[num2 - 1], num));
	*(pSpline->pCoordPoint + num3 - 1) = value;
	ZeroMemory(ptr2, sizeof(double) * (num2 + 1));
	ZeroMemory(ptr3, sizeof(double) * (num2 + 1));
	ZeroMemory(ptr4, sizeof(double) * (num2 + 1));
}

void ParseHelixPoints(PDXFENTHELIX pSpline)
{
	 pSpline->pControlNode->clear();
	 int num = NumberOfPartsInSpline;
	 int num2 = pSpline->FittingPointNum;
	 int i = 1;
	 bool flag = (pSpline->StartTangent.x == 0.0f && pSpline->StartTangent.y == 0.0f && pSpline->StartTangent.z == 0.0f);
	 bool flag2 = (pSpline->EndTangent.x == 0.0f && pSpline->EndTangent.y == 0.0f && pSpline->EndTangent.z == 0.0f);
	 double n = 0.0f;

	 //从链表中读取拟合点的坐标
	 PREALPOINT p = ReadSimpleListData(*pSpline->pFittingNode);

	 if (NULL == p) //判断是否读出来的拟合点坐标指针p为NULL
	 {
		 return;
	 }

	 while (i < num2)
	 {
		 double num3 = Distance(p[i], p[i-1]);
		 if (Fval(num3, 0.0f) > 0.0f)
		 {
			 n += num3;
			 i++;
		 }
		 else
		 {
			 //set defaults
			 num2--;
		 }
	 }

	 double *array = new double[num2];
	 ZeroMemory(array, sizeof(double) * num2);
	 double *array2 = new double[num2+ 1];
	 ZeroMemory(array2, sizeof(double) * (num2 + 1));
	 double *array3 = new double[num2 + 1];
	 ZeroMemory(array3, sizeof(double) * (num2 + 1));
	 double *array4 = new double[num2 + 1];
	 ZeroMemory(array4, sizeof(double) * (num2 + 1));
	 
	 if (array != NULL && array2 != NULL && array3 != NULL && array4 != NULL)
	 {
		 for (i = 1; i < num2 - 1; i++)
		 {
			 array[i] = array[i - 1] + Distance(p[i], p[i - 1]) / n;
		 }
		 array[num2 - 1] = 1.0f;
		 int num4 = (num2 - 1) * num;
		 pSpline->CoordCount = num4;
		 pSpline->pCoordPoint = new REALPOINT[num4];
		 memset(pSpline->pCoordPoint, 0, sizeof(REALPOINT) * num4);
		 for (i = 0; i < 3; i++)
		 {
			 ParsePointCoords(pSpline, i, num2, flag, flag2, num4, array, array2, array3, array4, num, n, p);
		 }

		 delete[] array;
		 array = NULL;
		 delete[] array2;
		 array2 = NULL;
		 delete[] array3;
		 array3 = NULL;
		 delete[] array4;
		 array4 = NULL;
	 }

	 delete[] p;
	 p = NULL;
}

void GetSplineFittingNode(PDXFENTHELIX pHelix)
{
	pHelix->pControlNode->clear();
	pHelix->pFittingNode->clear();
	pHelix->ControlNodeNum = 0;
	pHelix->FittingPointNum = 0;
	REALPOINT dPoint;
	dPoint.x = pHelix->Start.x - pHelix->AxisBasePoint.x;
	dPoint.y = pHelix->Start.y - pHelix->AxisBasePoint.y;
	dPoint.z = pHelix->Start.z - pHelix->AxisBasePoint.z;
	double num = sqrt(Sqr(dPoint.x) + Sqr(dPoint.y) + Sqr(dPoint.z));
	double num2 = atan2(dPoint.y, dPoint.x);
	int i = (int)ceil(16.0f * pHelix->Turns);
	double num3 = (pHelix->Radius - num) / (double)i;
	double num4 = pHelix->Turns * pHelix->TurnHeight / (double)i;
	double num5 = 2 * PI * pHelix->Turns / (double)i;
	if (pHelix->Flag)
	{
		num5 *= -1.0;
	}
	while (i >= 0)
	{
		double num6 = sin(num2);
		double num7 = cos(num2);
		dPoint.x = pHelix->AxisBasePoint.x + num7 * num;
		dPoint.y = pHelix->AxisBasePoint.y + num6 * num;
		pHelix->pFittingNode->add(dPoint);
		pHelix->FittingPointNum++;
		num2 += num5;
		num += num3;
		dPoint.z += num4;
		i--;
	}
}

BOOL ReadHelixData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTHELIX pHelix )
{
	ZeroMemory(pHelix, sizeof(DXFENTHELIX));
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while (GCode != 0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 70:
			ReadSPlineData(pDxf, pEntityHeader, (PDXFENTSPLINE)pHelix);
			break;
		case 10:
			pHelix->AxisBasePoint.x = atof(strValue);
			break;
		case 20:
			pHelix->AxisBasePoint.y = atof(strValue);
			break;
		case 30:
			pHelix->AxisBasePoint.z = atof(strValue);
			break;
		case 11:
			pHelix->Start.x = atof(strValue);
			break;
		case 21:
			pHelix->Start.y = atof(strValue);
			break;
		case 31:
			pHelix->Start.z = atof(strValue);
			break;
		case 12:
			pHelix->StartTangent.x = atof(strValue);
			pHelix->AxisVec[0] = pHelix->StartTangent.x;
			break;
		case 22:
			pHelix->StartTangent.y = atof(strValue);
			pHelix->AxisVec[1] = pHelix->StartTangent.y;
			break;
		case 32:
			pHelix->StartTangent.z = atof(strValue);
			pHelix->AxisVec[2] = pHelix->StartTangent.z;
			break;
		case 40:
			pHelix->Radius = atof(strValue);
			break;
		case 41:
			pHelix->Turns = atof(strValue);
			break;
		case 42:
			pHelix->TurnHeight = atof(strValue);
			break;
		case 290:
			pHelix->Flag = (atoi(strValue) == 0);
			break;
		case 280:
			pHelix->ConstraintType = atoi(strValue);
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	pEntityHeader->EntityType = ENT_HELIX;

	GetSplineFittingNode(pHelix);
	ParseHelixPoints(pHelix);

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadAcadTableData                                                    |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTACADTABLE pAcadTable = pointer to AcadTable structure |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL ReadAcadTableData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTACADTABLE pAcadTable )
{
	ZeroMemory(pAcadTable, sizeof(DXFENTACADTABLE));
	pEntityHeader->EntityType = ENT_ACADTABLE;

	ReadParamFromDxfFile(pDxf, GCode, strValue);

	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 2:		// Block Name
			strcpy_s(pAcadTable->BlockHeaderName, strValue);
			break;
		case 10:	// Insertion point X
			pAcadTable->Point.x = atof(strValue);
			break;
		case 20:	// Insertion point Y
			pAcadTable->Point.y = atof(strValue);
			break;
		case 30:	// Insertion point Z
			pAcadTable->Point.z = atof(strValue);
			break;
		}
		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  ReadWipeOutData                                                    |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      PDXFENTWIPEOUT pWipeOut = pointer to WipeOut structure |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
void ReadWipeOutPoint(PDXFENTWIPEOUT pWipeOut)
{
	pWipeOut->nPolyPoints = pWipeOut->nClip;
	pWipeOut->pPolyPoints = new REALPOINT[pWipeOut->nPolyPoints];
	if (NULL == pWipeOut->pPolyPoints)
	{
		return;
	}

	for (int i = 0; i < pWipeOut->nPolyPoints; i++)
	{
		double num = pWipeOut->UValue - 0.5f + pWipeOut->pClipPoints[i].x;
		double num2 = pWipeOut->VValue - 0.5f - pWipeOut->pClipPoints[i].y;

		pWipeOut->pPolyPoints[i].x = pWipeOut->Point.x + num * pWipeOut->UVector.x + num2 * pWipeOut->VVector.x;
		pWipeOut->pPolyPoints[i].y = pWipeOut->Point.y + num * pWipeOut->UVector.y + num2 * pWipeOut->VVector.y;
		pWipeOut->pPolyPoints[i].z = pWipeOut->Point.z + num * pWipeOut->UVector.z + num2 * pWipeOut->VVector.z;
	}
}

BOOL ReadWipeOutData( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, PDXFENTWIPEOUT pWipeOut )
{
	ZeroMemory(pWipeOut, sizeof(DXFENTWIPEOUT));
	pEntityHeader->EntityType = ENT_WIPEOUT;
	pWipeOut->Closed = true;
	ReadParamFromDxfFile(pDxf, GCode, strValue);

	int i = 0;
	while(GCode!=0)
	{
		switch(GCode)
		{
		case 8:		// Layer Name
			strcpy_s(pEntityHeader->LayerName, strValue);
			break;
		case 62:	// Color
			pEntityHeader->Color = atoi(strValue);
			break;
		case 6:		// Line Type
			strcpy_s(pEntityHeader->LTypeName, strValue);
			break;
		case 39:	// Thickness
			pEntityHeader->Thickness = atof(strValue);
			break;
		case 48:	// Linetype scale
			pEntityHeader->LineTypeScale = atof(strValue);
			break;
		case 10:	// Insert point X
			pWipeOut->Point.x = atof(strValue);
			break;
		case 20:	// Insert point Y
			pWipeOut->Point.y = atof(strValue);
			break;
		case 30:	// Insert point Z
			pWipeOut->Point.z = atof(strValue);
			break;
		case 11:	// Pixel UVector point X
			pWipeOut->UVector.x = atof(strValue);
			break;
		case 21:	// Pixel UVector point Y
			pWipeOut->UVector.y = atof(strValue);
			break;
		case 31:	// Pixel UVector point Z
			pWipeOut->UVector.z = atof(strValue);
			break;
		case 12:	// Pixel VVector point X
			pWipeOut->VVector.x = atof(strValue);
			break;
		case 22:	// Pixel VVector point Y
			pWipeOut->VVector.y = atof(strValue);
			break;
		case 32:	// Pixel VVector point Z
			pWipeOut->VVector.z = atof(strValue);
			break;
		case 13:	// Image Size U value
			pWipeOut->UValue = atof(strValue);
			break;
		case 23:	// Image Size V value
			pWipeOut->VValue = atof(strValue);
			break;
		case 70:	// Image Display attribute
			pWipeOut->Flag = atoi(strValue);
			break;
		case 280:	// Clipping state
			pWipeOut->IsUseClipping = atoi(strValue);
			break;
		case 281:	// Bright value
			pWipeOut->Bright = atoi(strValue);
			break;
		case 282:	// Contrast value
			pWipeOut->Contrast = atoi(strValue);
			break;
		case 283:	// FadeUnder value
			pWipeOut->FadeUnder = atoi(strValue);
			break;
		case 71:	// Clipping Boundary type
			pWipeOut->ClippingBoundaryType = atoi(strValue);
			break;
		case 91:	// Clipping Boundary Vertex count
			pWipeOut->nClip = atoi(strValue);
			pWipeOut->pClipPoints = new REALPOINT[pWipeOut->nClip];
			ZeroMemory(pWipeOut->pClipPoints, sizeof(REALPOINT) * pWipeOut->nClip);
			break;
		case 14:	// Clipping Boundary Vertex X
			pWipeOut->pClipPoints[i].x = atof(strValue);
			break;
		case 24:	// Clipping Boundary Vertex Y
			pWipeOut->pClipPoints[i].y = atof(strValue);
			i++;
			break;
		default:
			break;
		}

		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	ReadWipeOutPoint(pWipeOut);

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  SetEntityHeaderDefaultValues                                     |
 |  Inputs:                                                          |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |  Output: Nothing!                                                 |
 *-------------------------------------------------------------------*/
void SetEntityHeaderDefaultValues( PDXFENTITYHEADER pEntityHeader )
{
	ZeroMemory(pEntityHeader, sizeof(DXFENTITYHEADER));
	pEntityHeader->Color = 256;				// Color (Default=BYLAYER)
	pEntityHeader->Thickness = 0;				// Thickness (default=0)
	pEntityHeader->LineTypeScale = 1.0;			// Linetype scale (default=1.0)
	pEntityHeader->ExtrusionDirection[0] = 0;	// Extrusion direction. (default = 0, 0, 1)
	pEntityHeader->ExtrusionDirection[1] = 0;
	pEntityHeader->ExtrusionDirection[2] = 1;
}

/*-------------------------------------------------------------------*
 |  dxfReadEntityData_Direct                                         |
 |  Inputs:                                                          |
 |      PDXF pDxf = pointer to the openning DXF file structure       |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      LPVOID pEntityData = pointer to entity data structure to read|
 |  Output: TRUE if new entity is loaded                             |
 *-------------------------------------------------------------------*/
BOOL dxfReadEntityData_Direct( PDXF pDxf, PDXFENTITYHEADER pEntityHeader, LPVOID pEntityData )
{
	BOOL	result;

	// Check if current section is ENTITIES or BLOCKS
	if(pDxf->Read.CurrentSection!=SEC_ENTITIES)
	{
		if((pDxf->Read.CurrentSection!=SEC_BLOCKS) || (!pDxf->Read.isBlockOpen))
		{
			// Can not read entity data
			return FALSE;
		}
	}

	dxfStorePos(pDxf);
	ReadParamFromDxfFile(pDxf, GCode, strValue);
	if(GCode!=0)
	{
		// Can not read entity data
		dxfRestorePos(pDxf);
		return FALSE;
	}

	SetEntityHeaderDefaultValues(pEntityHeader);	// Set Entities Header to Default Values
	result = FALSE;

	do {
		if(strcmp(strValue, "LINE")==0)
		{
			ReadLineData(pDxf, pEntityHeader, (PDXFENTLINE)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue, "POINT")==0)
		{
			ReadPointData(pDxf, pEntityHeader, (PDXFENTPOINT)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue, "CIRCLE")==0)
		{
			ReadCircleData(pDxf, pEntityHeader, (PDXFENTCIRCLE)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue, "ELLIPSE")==0)
		{
			ReadEllipseData(pDxf, pEntityHeader, (PDXFENTELLIPSE)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue, "TEXT")==0||strcmp(strValue,"ATTRIB")==0)
		{
			ReadTextData(pDxf, pEntityHeader, (PDXFENTTEXT)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue,"MTEXT")==0)
		{
			ReadMTextData(pDxf,pEntityHeader,(PDXFENTMTEXT)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue, "ARC")==0)
		{
			ReadArcData(pDxf, pEntityHeader, (PDXFENTARC)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue, "SOLID")==0)
		{
			ReadSolidData(pDxf, pEntityHeader, (PDXFENTSOLID)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue, "INSERT")==0)
		{
			ReadInsertData(pDxf, pEntityHeader, (PDXFENTINSERT)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue, "POLYLINE")==0){
			ReadPolyLineData(pDxf, pEntityHeader, (PDXFENTPOLYLINE)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue, "LWPOLYLINE")==0){
			ReadLWPolyLineData(pDxf, pEntityHeader, (PDXFENTLWPOLYLINE)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue, "DIMENSION")==0)
		{
			ReadDimensionData(pDxf, pEntityHeader, (PDXFENTDIMENSION)pEntityData);
			result = TRUE;
			break;
		}
		else if(strcmp(strValue,"LEADER")==0)
		{
			ReadLeaderData(pDxf,pEntityHeader,(PDXFENTLEADER)pEntityData);
			result=TRUE;
			break;
		}
		//else if(strcmp(strValue,"MLINE")==0)
		//{
		//	ReadMLineData(pDxf,pEntityHeader,(PDXFENTMLINE)pEntityData);
		//	result=TRUE;
		//	break;
		//}
		else if(strcmp(strValue,"SPLINE")==0)
		{
			ReadSPlineData(pDxf,pEntityHeader,(PDXFENTSPLINE)pEntityData);
			result=TRUE;
			break;
		}
		else if(strcmp(strValue, "3DFACE")==0)
		{
			Read3DFaceData(pDxf,pEntityHeader,(PDXFENT3DFACE)pEntityData);
			result=TRUE;
			break;
		}
		else if(strcmp(strValue, "TRACE")==0)
		{
			ReadTraceData(pDxf,pEntityHeader,(PDXFENTTRACE)pEntityData);
			result=TRUE;
			break;
		}
		else if(strcmp(strValue,"HATCH")==0)
		{
			ReadHatchData(pDxf,pEntityHeader,(PDXFENTHATCH)pEntityData);
		    result=TRUE;
			break;
		}
		else if(strcmp(strValue,"HELIX")==0)
		{
			ReadHelixData(pDxf,pEntityHeader,(PDXFENTHELIX)pEntityData);
			result=TRUE;
			break;
		}
		else if(strcmp(strValue,"ACAD_TABLE")==0)
		{
			ReadAcadTableData(pDxf,pEntityHeader,(PDXFENTACADTABLE)pEntityData);
			result=TRUE;
			break;
		}
		else if(strcmp(strValue,"WIPEOUT")==0)
		{
			ReadWipeOutData(pDxf,pEntityHeader,(PDXFENTWIPEOUT)pEntityData);
			result=TRUE;
			break;
		}
		else
		{
			do {
				ReadParamFromDxfFile(pDxf, GCode, strValue);
			} while(GCode!=0);
		}
	} while((strcmp(strValue, "ENDBLK")!=0) && (strcmp(strValue, "ENDSEC")!=0) && (pDxf->Read.CurrentPos<pDxf->Read.FileSize));

	if((pDxf->Read.CurrentSection==SEC_BLOCKS) && (pDxf->Read.isBlockOpen))
	{
		if((GCode==0) && (strcmp(strValue, "ENDBLK")==0))
		{
			pDxf->Read.isBlockOpen = FALSE;
			do{
				dxfStorePos(pDxf);
				ReadParamFromDxfFile(pDxf, GCode, strValue);
			} while(GCode!=0);
			if(strcmp(strValue, "ENDSEC")==0)
				pDxf->Read.CurrentSection = SEC_NOTSET; // Blocks section has been finished
			else // Reached to the next block
				dxfRestorePos(pDxf);
		}
		else
			dxfRestorePos(pDxf);
	}
	else
	{
		if((GCode==0) && (strcmp(strValue, "ENDSEC")==0))
			pDxf->Read.CurrentSection = SEC_NOTSET; // Entities section has been finished
		else
			dxfRestorePos(pDxf);
	}

	return result;
}

/*-------------------------------------------------------------------*
 |  dxfReadEntityData                                                |
 |  Inputs:                                                          |
 |      HDXF hDxf = handle to the openning DXF file structure        |
 |      PDXFENTITYHEADER pEntityHeader =                             |
 |                  pointer to entity data header                    |
 |      LPVOID pEntityData = pointer to entity data structure to read|
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL dxfReadEntityData( HDXF hDxf, PDXFENTITYHEADER pEntityHeader, LPVOID pEntityData )
{
	PDXF	pDxf;

	// Initialize pDxf ------------------
	if((pDxf = InitilizePDXF(hDxf))==NULL)
		return FALSE;

	if(!dxfReadEntityData_Direct(pDxf, pEntityHeader, pEntityData))
	{
		GlobalUnlock(hDxf);
		return FALSE;
	}

	// UnInitilize hDxf -----------------
	return UnInitilizePDXF(hDxf);
}

