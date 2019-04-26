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
|  View.cpp                                                           |
\*-------------------------------------------------------------------*/

#include "stdafx.h"
#include "View.h"
#include "Colors.h"
#include "TextView.h"
#include "DimView.h"

/********************************************************************
 * Functions for calculating entities boundary rectangle            *
 ********************************************************************/
BOOL GetEntityRect(PDRAWING pDrawing, PENTITYHEADER pEntityHeader, LPVOID pEntityData, REALRECT* pRect);

/*-------------------------------------------------------------------*
|  GetLineRect                                                      |
|  Inputs:                                                          |
|      PENTITYHEADER pEntityHeader = pointer to entity data header  |
|      PENTLINE pLine = pointer to line data structure              |
|      REALRECT* pRect = pointer to entity boundary rectangle       |
|  Output: TRUE if everything is ok                                 |
*-------------------------------------------------------------------*/
BOOL GetLineRect(PENTITYHEADER pEntityHeader, PENTLINE pLine, REALRECT* pRect)
{
	pRect->top = __max(pLine->Point0.y, pLine->Point1.y);
	pRect->left = __min(pLine->Point0.x, pLine->Point1.x);
	pRect->bottom = __min(pLine->Point0.y, pLine->Point1.y);
	pRect->right = __max(pLine->Point0.x, pLine->Point1.x);
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetPointRect                                                     |
 |  Inputs:                                                          |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTPOINT pPoint = pointer to point data structure           |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetPointRect(PENTITYHEADER pEntityHeader, PENTPOINT pPoint, REALRECT* pRect)//
{
	pRect->top = pPoint->Point0.y;
	pRect->left = pPoint->Point0.x;
	pRect->bottom = pPoint->Point0.y;
	pRect->right = pPoint->Point0.x;
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetCircleRect                                                    |
 |  Inputs:                                                          |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTCIRCLE pCircle = pointer to circle data structure        |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetCircleRect(PENTITYHEADER pEntityHeader, PENTCIRCLE pCircle, REALRECT* pRect)
{
	pRect->top = pCircle->Point0.y + pCircle->Radius;
	pRect->left = pCircle->Point0.x - pCircle->Radius;
	pRect->bottom = pCircle->Point0.y - pCircle->Radius;
	pRect->right = pCircle->Point0.x + pCircle->Radius;
	return TRUE;
}

BOOL GetArcRect(PENTITYHEADER pEntityHeader, PENTARC pArc, REALRECT* pRect)
{
	pRect->top = pArc->Point0.y + pArc->Radius;
	pRect->left = pArc->Point0.x - pArc->Radius;
	pRect->bottom = pArc->Point0.y - pArc->Radius;
	pRect->right = pArc->Point0.x + pArc->Radius;
	return TRUE;
}

BOOL GetEllipseRect(PENTITYHEADER pEntityHeader, PENTELLIPSE pEllipse, REALRECT* pRect)
{
	pRect->top=pEllipse->CenterPoint.y-pEllipse->MajorAxisEndPoint.x*pEllipse->MinorToMajorRatio;
	pRect->left=pEllipse->CenterPoint.x-pEllipse->MajorAxisEndPoint.x;
	pRect->bottom=pEllipse->CenterPoint.y+pEllipse->MajorAxisEndPoint.x*pEllipse->MinorToMajorRatio;
	pRect->right=pEllipse->CenterPoint.x+pEllipse->MajorAxisEndPoint.x;
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetSolidRect                                                     |
 |  Inputs:                                                          |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTSOLID pSolid = pointer to solid data structure           |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetSolidRect(PENTITYHEADER pEntityHeader, PENTSOLID pSolid, REALRECT* pRect)
{
	pRect->top = __max(pSolid->Point0.y, pSolid->Point1.y);
	pRect->top = __max(pRect->top, pSolid->Point2.y);
	pRect->top = __max(pRect->top, pSolid->Point3.y);
	pRect->left = __min(pSolid->Point0.x, pSolid->Point1.x);
	pRect->left = __min(pRect->left, pSolid->Point2.x);
	pRect->left = __min(pRect->left, pSolid->Point3.x);
	pRect->bottom = __min(pSolid->Point0.y, pSolid->Point1.y);
	pRect->bottom = __min(pRect->bottom, pSolid->Point2.y);
	pRect->bottom = __min(pRect->bottom, pSolid->Point3.y);
	pRect->right = __max(pSolid->Point0.x, pSolid->Point1.x);
	pRect->right = __max(pRect->right, pSolid->Point2.x);
	pRect->right = __max(pRect->right, pSolid->Point3.x);
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetInsertBlockRect                                               |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTINSERT pInsert = pointer to insert data structure        |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
void GetInsertEntityRect(PENTINSERT pInsert, REALRECT* pRect)
{
	REALPOINT pt, ptOrigin, ptTopLeft, ptTopRight, ptBottomLeft, ptBottomRight;
	ZeroMemory(&ptOrigin, sizeof(REALPOINT));
	ZeroMemory(&pt, sizeof(REALPOINT));

	//左上点旋转伸缩
	pt.x = pRect->left;
	pt.y = pRect->top;
	ptTopLeft = PointRotationPoint(ptOrigin, pt, pInsert->RotationAngle , pInsert->XScale, pInsert->YScale);

	//右上点旋转伸缩
	pt.x = pRect->right;
	pt.y = pRect->top;
	ptTopRight = PointRotationPoint(ptOrigin, pt, pInsert->RotationAngle , pInsert->XScale, pInsert->YScale);

	//左下点旋转伸缩
	pt.x = pRect->left;
	pt.y = pRect->bottom;
	ptBottomLeft = PointRotationPoint(ptOrigin, pt, pInsert->RotationAngle , pInsert->XScale, pInsert->YScale);

	//右下点旋转伸缩
	pt.x = pRect->right;
	pt.y = pRect->bottom;
	ptBottomRight = PointRotationPoint(ptOrigin, pt, pInsert->RotationAngle , pInsert->XScale, pInsert->YScale);

	pRect->top = __max(__max(__max(ptTopLeft.y, ptTopRight.y), ptBottomLeft.y), ptBottomRight.y);
	pRect->left = __min(__min(__min(ptTopLeft.x, ptTopRight.x), ptBottomLeft.x), ptBottomRight.x);
	pRect->bottom =__min(__min(__min(ptTopLeft.y, ptTopRight.y), ptBottomLeft.y), ptBottomRight.y);
	pRect->right = __max(__max(__max(ptTopLeft.x, ptTopRight.x), ptBottomLeft.x), ptBottomRight.x);
}

BOOL GetInsertBlockRect(PDRAWING pDrawing, PENTITYHEADER pEntityHeader, PENTINSERT pInsert, REALRECT* pRect)
{
	BLOCKHEADER		BlockHeader;
	ENTITYHEADER	BlockEntityHeader;
	char			BlockEntityData[4096];

	pRect->left = DBL_MAX;
	pRect->top = DBL_MIN;
	pRect->right = DBL_MIN;
	pRect->bottom = DBL_MAX;

	//BlockHeader.Objhandle = pInsert->BlockHeaderObjhandle;
	strcpy_s(BlockHeader.Name, pInsert->BlockHeaderName);
	if(drwFindBlock_Direct(pDrawing, /*FIND_BYHANDLE*/FIND_BYNAME, &BlockHeader)>0)
	{
		if(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_FIRST)>0)
		{
			do{
				REALRECT	Rect;
				if(GetEntityRect(pDrawing, &BlockEntityHeader, BlockEntityData, &Rect))
				{
					GetInsertEntityRect(pInsert, &Rect);
					pRect->top = __max(Rect.top+pInsert->Point0.y, pRect->top);
					pRect->left = __min(Rect.left+pInsert->Point0.x, pRect->left);
					pRect->bottom = __min(Rect.bottom+pInsert->Point0.y, pRect->bottom);
					pRect->right = __max(Rect.right+pInsert->Point0.x, pRect->right);
				}
			} while(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_NEXT)>0);
		}
	}

	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetPolylineRect                                                  |
 |  Inputs:                                                          |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTPOLYLINE pPolyline = pointer to polyline data structure  |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetPolyCurveRect(PENTITYHEADER pEntityHeader, PENTPOLYCURVE pPolyline, REALRECT* pRect)
{
	pRect->top = pPolyline->pPolyPoints[0].y;
	pRect->left = pPolyline->pPolyPoints[0].x;
	pRect->bottom = pPolyline->pPolyPoints[0].y;
	pRect->right = pPolyline->pPolyPoints[0].x;

	for(int i=1; i<pPolyline->nPolyPoints; i++)
	{
		pRect->top = __max(pRect->top, pPolyline->pPolyPoints[i].y);
		pRect->left = __min(pRect->left, pPolyline->pPolyPoints[i].x);
		pRect->bottom = __min(pRect->bottom, pPolyline->pPolyPoints[i].y);
		pRect->right = __max(pRect->right, pPolyline->pPolyPoints[i].x);
	}

	return TRUE;
}

BOOL GetPolylineRect(PENTITYHEADER pEntityHeader, PENTPOLYLINEBASE pPolyline, REALRECT* pRect)
{
	BOOL result = GetPolyCurveRect(pEntityHeader, (PENTPOLYCURVE)pPolyline, pRect);
	
	if (pPolyline->pArrows != NULL && pPolyline->nArrows != 0)
	{
		for(int i=0; i<pPolyline->nArrows; i++)
		{
			pRect->top = __max(pRect->top, pPolyline->pArrows[i].y);
			pRect->left = __min(pRect->left, pPolyline->pArrows[i].x);
			pRect->bottom = __min(pRect->bottom, pPolyline->pArrows[i].y);
			pRect->right = __max(pRect->right, pPolyline->pArrows[i].x);
		}
	}

	return result;
}

BOOL GetSPlineRect(PENTITYHEADER pEntityHeader, PENTSPLINE pSPline, REALRECT* pRect)
{
	pSPline->pControlNode->start();
	pSPline->pControlNode->next();
	pRect->top=pSPline->pControlNode->get().y;
	pRect->left=pSPline->pControlNode->get().x;
	pRect->bottom=pSPline->pControlNode->get().y;
	pRect->right=pSPline->pControlNode->get().x;
	pSPline->pControlNode->start();
	while(pSPline->pControlNode->next())
	{
		pRect->top = __max(pRect->top, pSPline->pControlNode->get().y);
		pRect->left = __min(pRect->left, pSPline->pControlNode->get().x);
		pRect->bottom = __min(pRect->bottom, pSPline->pControlNode->get().y);
		pRect->right = __max(pRect->right,pSPline->pControlNode->get().x);
	}
	return TRUE;
}

BOOL GetMLineRect(PENTITYHEADER pEntityHeader, PENTMLINE pMline, REALRECT* pRect)
{
	pMline->pVetexPoint->start();
	pMline->pVetexPoint->next();
	pRect->top=pMline->pVetexPoint->get().VetexPoint.y;
	pRect->left=pMline->pVetexPoint->get().VetexPoint.x;
	pRect->bottom=pMline->pVetexPoint->get().VetexPoint.y;
	pRect->right=pMline->pVetexPoint->get().VetexPoint.x;
	pMline->pVetexPoint->start();
	while(pMline->pVetexPoint->next())
	{
		pRect->top = __max(pRect->top,pMline->pVetexPoint->get().VetexPoint.y);
		pRect->left = __min(pRect->left, pMline->pVetexPoint->get().VetexPoint.x);
		pRect->bottom = __min(pRect->bottom, pMline->pVetexPoint->get().VetexPoint.y);
		pRect->right = __max(pRect->right,pMline->pVetexPoint->get().VetexPoint.x);
	}
	return TRUE;
}

BOOL Get3DFaceRect(PENTITYHEADER pEntityHeader, PENT3DFACE p3DFace, REALRECT* pRect)
{
	pRect->top = __max(p3DFace->Point0.y, p3DFace->Point1.y);
	pRect->top = __max(pRect->top, p3DFace->Point2.y);
	pRect->top = __max(pRect->top, p3DFace->Point3.y);
	pRect->left = __min(p3DFace->Point0.x, p3DFace->Point1.x);
	pRect->left = __min(pRect->left, p3DFace->Point2.x);
	pRect->left = __min(pRect->left, p3DFace->Point3.x);
	pRect->bottom = __min(p3DFace->Point0.y, p3DFace->Point1.y);
	pRect->bottom = __min(pRect->bottom, p3DFace->Point2.y);
	pRect->bottom = __min(pRect->bottom, p3DFace->Point3.y);
	pRect->right = __max(p3DFace->Point0.x, p3DFace->Point1.x);
	pRect->right = __max(pRect->right, p3DFace->Point2.x);
	pRect->right = __max(pRect->right, p3DFace->Point3.x);
	return TRUE;
}

BOOL GetTraceRect(PENTITYHEADER pEntityHeader, PENTTRACE pTrace, REALRECT* pRect)
{
	pRect->top = __max(pTrace->Point0.y, pTrace->Point1.y);
	pRect->top = __max(pRect->top, pTrace->Point2.y);
	pRect->top = __max(pRect->top, pTrace->Point3.y);
	pRect->left = __min(pTrace->Point0.x, pTrace->Point1.x);
	pRect->left = __min(pRect->left, pTrace->Point2.x);
	pRect->left = __min(pRect->left, pTrace->Point3.x);
	pRect->bottom = __min(pTrace->Point0.y, pTrace->Point1.y);
	pRect->bottom = __min(pRect->bottom, pTrace->Point2.y);
	pRect->bottom = __min(pRect->bottom, pTrace->Point3.y);
	pRect->right = __max(pTrace->Point0.x, pTrace->Point1.x);
	pRect->right = __max(pRect->right, pTrace->Point2.x);
	pRect->right = __max(pRect->right, pTrace->Point3.x);
	return TRUE;
}

BOOL GetHatchRect(PENTITYHEADER pEntityHeader, PENTHATCH pHatch, REALRECT* pRect)
{
	pRect->top = pHatch->DRect.top;
	pRect->left = pHatch->DRect.left;
	pRect->bottom = pHatch->DRect.bottom;
	pRect->right = pHatch->DRect.right;
	return TRUE;
}

BOOL GetHelixRect(PENTITYHEADER pEntityHeader, PENTHELIX pHelix, REALRECT* pRect)
{
	double d = Distance(pHelix->AxisBasePoint, pHelix->Start);
	pRect->top = pHelix->AxisBasePoint.y + d;
	pRect->left = pHelix->AxisBasePoint.x - d;
	pRect->bottom = pHelix->AxisBasePoint.y - d;
	pRect->right = pHelix->AxisBasePoint.x + d;
	return TRUE;
}

BOOL GetAcadTableRect(PDRAWING pDrawing, PENTITYHEADER pEntityHeader, PENTACADTABLE pAcadTable, REALRECT* pRect)
{
	BLOCKHEADER		BlockHeader;
	ENTITYHEADER	BlockEntityHeader;
	char			BlockEntityData[4096];

	pRect->left = DBL_MAX;
	pRect->top = DBL_MIN;
	pRect->right = DBL_MIN;
	pRect->bottom = DBL_MAX;

	strcpy_s(BlockHeader.Name, pAcadTable->BlockHeaderName);
	if(drwFindBlock_Direct(pDrawing, FIND_BYNAME, &BlockHeader)>0)
	{
		if(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_FIRST)>0)
		{
			do{
				REALRECT	Rect;
				if(GetEntityRect(pDrawing, &BlockEntityHeader, BlockEntityData, &Rect))
				{
					pRect->top = __max(Rect.top+pAcadTable->Point.y, pRect->top);
					pRect->left = __min(Rect.left+pAcadTable->Point.x, pRect->left);
					pRect->bottom = __min(Rect.bottom+pAcadTable->Point.y, pRect->bottom);
					pRect->right = __max(Rect.right+pAcadTable->Point.x, pRect->right);
				}
			} while(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_NEXT)>0);
		}
	}
	
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetTextRect                                                      |
 |  Inputs:                                                          |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTTEXT pText = pointer to text data structure              |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetTextRect(PENTITYHEADER pEntityHeader, PENTTEXT pText, REALRECT* pRect)
{
	pRect->top = __max(pText->TextRect.top, pText->TextRect.bottom);
	pRect->left = __min(pText->TextRect.left, pText->TextRect.right);
	pRect->bottom = __min(pText->TextRect.top, pText->TextRect.bottom);
	pRect->right = __max(pText->TextRect.left, pText->TextRect.right);
	return TRUE;
}

BOOL GetMTextRect(PENTITYHEADER pEntityHeader, PENTMTEXT pText, REALRECT* pRect)
{
	return TRUE;
}

/*-------------------------------------------------------------------*
 |  GetEntityRect                                                    |
 |  Descriptions:                                                    |
 |      Calculates entity boundary rectangle                         |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      LPVOID pEntityData = pointer to entity data structure        |
 |      REALRECT* pRect = pointer to entity boundary rectangle       |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL GetEntityRect(PDRAWING pDrawing, PENTITYHEADER pEntityHeader, LPVOID pEntityData, REALRECT* pRect)//获取图元的显示范围
{
	BOOL	result;

	switch( pEntityHeader->EntityType )
	{
	case ENT_LINE:
		result = GetLineRect(pEntityHeader, (PENTLINE)pEntityData, pRect);
		break;

	case ENT_POINT:
		result = GetPointRect(pEntityHeader, (PENTPOINT)pEntityData, pRect);
		break;

	case ENT_CIRCLE:
		result = GetCircleRect(pEntityHeader, (PENTCIRCLE)pEntityData, pRect);
		break;

	case ENT_ELLIPSE:
		result = GetEllipseRect(pEntityHeader, (PENTELLIPSE)pEntityData, pRect);
		break;

	case ENT_TEXT:
		result = GetTextRect(pEntityHeader, (PENTTEXT)pEntityData, pRect);
		break;

	case ENT_ARC:
		result = GetArcRect(pEntityHeader, (PENTARC)pEntityData, pRect);
		break;

	case ENT_SOLID:
		result = GetSolidRect(pEntityHeader, (PENTSOLID)pEntityData, pRect);
		break;

	case ENT_INSERT:
		result = GetInsertBlockRect(pDrawing, pEntityHeader, (PENTINSERT)pEntityData, pRect);
		break;

	case ENT_POLYLINE:
	case ENT_LWPOLYLINE:
		result = GetPolylineRect(pEntityHeader, (PENTPOLYLINEBASE)pEntityData, pRect);
		break;

	case ENT_DIMENSION:
		result = GetDimRect(pDrawing, pEntityHeader, (PENTDIMENSION)pEntityData, pRect);
		break;

	case ENT_SPLINE:
		result=GetSPlineRect(pEntityHeader, (PENTSPLINE)pEntityData, pRect);
		break;

	case ENT_MLINE:
		result=GetMLineRect(pEntityHeader,(PENTMLINE)pEntityData,pRect);
		break;

	case ENT_3DFACE:
		result=Get3DFaceRect(pEntityHeader,(PENT3DFACE)pEntityData,pRect);
		break;

	case ENT_TRACE:
		result=GetTraceRect(pEntityHeader,(PENTTRACE)pEntityData,pRect);
		break;

	case ENT_HATCH:
		result=GetHatchRect(pEntityHeader,(PENTHATCH)pEntityData,pRect);
		break;

	case ENT_HELIX:
		result=GetHelixRect(pEntityHeader,(PENTHELIX)pEntityData,pRect);
		break;

	case ENT_ACADTABLE:
		result=GetAcadTableRect(pDrawing,pEntityHeader,(PENTACADTABLE)pEntityData,pRect);
		break;
	
	case ENT_WIPEOUT:
		result = GetPolyCurveRect(pEntityHeader,(PENTWIPEOUT)pEntityData,pRect);
		break;

	default: // Entity type is not accepted
		result = FALSE;
	}
	return result;
}

/********************************************************************
 * Functions for plot the drawing on DC                             *
 ********************************************************************/

// Units to pixel conversions
#define	U2PX(units)	pDrawing->View.WindowLeft + (int)((-pDrawing->View.ViewLeft + x+(units)*xScale) * pDrawing->View.PPU * pDrawing->View.ZoomLevel)
#define	U2PY(units)	pDrawing->View.WindowBottom - (int)((-pDrawing->View.ViewBottom + y+(units)*yScale) * pDrawing->View.PPU * pDrawing->View.ZoomLevel)
#define	U2PXROT(xUnits,yUnits)	pDrawing->View.WindowLeft + (int)((-pDrawing->View.ViewLeft + x+((xUnits)*xScale*cos(Rotation*PI/180)-(yUnits)*yScale*sin(Rotation*PI/180))*1) * pDrawing->View.PPU * pDrawing->View.ZoomLevel)
#define	U2PYROT(xUnits,yUnits)	pDrawing->View.WindowBottom - (int)((-pDrawing->View.ViewBottom + y+((xUnits)*xScale*sin(Rotation*PI/180)+(yUnits)*yScale*cos(Rotation*PI/180))*1) * pDrawing->View.PPU * pDrawing->View.ZoomLevel)

/*-------------------------------------------------------------------*
 |  DrawLine                                                         |
 |  Descriptions:                                                    |
 |      Draws a line in view area                                    |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      HDC hdc= handle of DC for drawing                            |
 |      int x1 = x1 of line                                          |
 |      int y1 = y1 of line                                          |
 |      int x2 = x2 of line                                          |
 |      int y2 = y2 of line                                          |
 |  Output: Nothing                                                  |
 *-------------------------------------------------------------------*/
void DrawLine(PDRAWING pDrawing, HDC hdc, COLORREF rgbcolor, int x1, int y1, int x2, int y2)
{
	double m;

	if(abs(x2-x1)>abs(y2-y1))
	{
		if(x1>x2)
		{
			int dummy;
			dummy = x1; x1 = x2; x2 = dummy;
			dummy = y1; y1 = y2; y2 = dummy;
		}
//		if((y2>pDrawing->View.WindowTop) && (y1<pDrawing->View.WindowBottom))
		{
			m = (double)(y2-y1)/(x2-x1); //求直线的斜率
			for(int i=__max(x1,pDrawing->View.WindowLeft); i<=__min(x2,pDrawing->View.WindowRight); i++)
			{
				int j = int(m*(i-x1))+y1;
				if((j>=pDrawing->View.WindowTop) && (j<=pDrawing->View.WindowBottom))
				  SetPixel( hdc, i, j, rgbcolor );//沿着直线画点
			}
		}
	}
	else
	{
		if(y1>y2)
		{
			int dummy;
			dummy = x1; x1 = x2; x2 = dummy;
			dummy = y1; y1 = y2; y2 = dummy;
		}
//		if((x2>pDrawing->View.WindowLeft) && (x1<pDrawing->View.WindowRight))
		{
			m = (double)(x2-x1)/(y2-y1);
			for(int j=__max(y1,pDrawing->View.WindowTop); j<=__min(y2,pDrawing->View.WindowBottom); j++)
			{
				int i = int(m*(j-y1))+x1;
				if((i>=pDrawing->View.WindowLeft) && (i<=pDrawing->View.WindowRight))
				  SetPixel( hdc, i, j, rgbcolor );
			}
		}
	}
}

/*-------------------------------------------------------------------*
 |  DrawDashLine                                                     |
 |  Descriptions:                                                    |
 |      Draws a line in view area                                    |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      HDC hdc= handle of DC for drawing                            |
 |      int x1 = x1 of line                                          |
 |      int y1 = y1 of line                                          |
 |      int x2 = x2 of line                                          |
 |      int y2 = y2 of line                                          |
 |      double PatternStartPoint                                     |
 |      double x = x position of insertion point (default = 0)       |
 |      double y = y position of insertion point (default = 0)       |
 |      double xScale = x scale factor (default = 1)                 |
 |      double yScale = y scale factor (default = 1)                 |
 |  Output: Nothing                                                  |
 *-------------------------------------------------------------------*/
void DrawDashLine(PDRAWING pDrawing, HDC hdc, LTYPE* pLType, double x1, double y1, double x2, double y2, COLORREF rgbcolor, double Scale, double PatternStartPoint, double x, double y, double xScale, double yScale, double Rotation)
{
	double x11, y11;
	double x12, y12;
	double m;
	double cosalpha;
	double l1, l2=0;
	double xend, yend;
	double MoveDirection = 1;
	int ElementNum;

	if(fabs(x2-x1)>fabs(y2-y1))
	{
		if(x1>x2)
			MoveDirection = -1;

		m = (y2-y1)/(x2-x1);
		cosalpha = 1 / sqrt(1+m*m);

		x11 = x1;//__max(x1,pDrawing->View.ViewLeft);
		y11 = y1 + m*(x11-x1);
		l1 = sqrt((x1-x11)*(x1-x11)+(y1-y11)*(y1-y11));
		l2 = fmod(PatternStartPoint + l1, pLType->PatternLength*Scale); 
		for(ElementNum=0; ElementNum<pLType->ElementsNumber; ElementNum++)
		{
			if(l2<fabs(pLType->Elements[ElementNum]*Scale))
				break;
			l2 = l2 - fabs(pLType->Elements[ElementNum]*Scale);
		}

		x12 = x11 + MoveDirection * (fabs(pLType->Elements[ElementNum]*Scale)-l2)*(cosalpha);
		y12 = y11 + (x12-x11)*(m);

		xend = x2;//__min(x2,pDrawing->View.ViewRight);
		while(MoveDirection*x12<MoveDirection*xend)
		{
			if(pLType->Elements[ElementNum]>=0)
//				DrawLine( pDrawing, hdc, rgbcolor, U2PX(x11), U2PY(y11), U2PX(x12), U2PY(y12) );
				DrawLine( pDrawing, hdc, rgbcolor, U2PXROT(x11,y11), U2PYROT(x11,y11), U2PXROT(x12,y12), U2PYROT(x12,y12) );
	
			ElementNum++;
			if(ElementNum>=pLType->ElementsNumber)
				ElementNum=0;

			x11 = x12;
			y11 = y12;
			x12 = x11 + MoveDirection * fabs(pLType->Elements[ElementNum]*Scale)*(cosalpha);
			y12 = y11 + (x12-x11)*(m);
		}
		if(pLType->Elements[ElementNum]>=0)
//			DrawLine( pDrawing, hdc, rgbcolor, U2PX(x11), U2PY(y11), U2PX(x2), U2PY(y2) );
			DrawLine( pDrawing, hdc, rgbcolor, U2PXROT(x11,y11), U2PYROT(x11,y11), U2PXROT(x2,y2), U2PYROT(x2,y2) );
	}
	else
	{
		if(y1>y2)
			MoveDirection = -1;

		m = (x2-x1)/(y2-y1);
		cosalpha = 1 / sqrt(1+m*m);

		y11 = y1;//__max(y1,pDrawing->View.WindowBottom);
		x11 = x1 + m*(y11-y1);
		l1 = sqrt((x1-x11)*(x1-x11)+(y1-y11)*(y1-y11));
		l2 = fmod(PatternStartPoint + l1, pLType->PatternLength*Scale); 
		for(ElementNum=0; ElementNum<pLType->ElementsNumber; ElementNum++)
		{
			if(l2<fabs(pLType->Elements[ElementNum]*Scale))
				break;
			l2 = l2 - fabs(pLType->Elements[ElementNum]*Scale);
		}

		y12 = y11 + MoveDirection * (fabs(pLType->Elements[ElementNum]*Scale)-l2)*(cosalpha);
		x12 = x11 + (y12-y11)*(m);

		yend = y2;//__min(y2,pDrawing->View.ViewTop);
		while(MoveDirection*y12<MoveDirection*yend)
		{
			if(pLType->Elements[ElementNum]>=0)
//				DrawLine( pDrawing, hdc, rgbcolor, U2PX(x11), U2PY(y11), U2PX(x12), U2PY(y12) );
				DrawLine( pDrawing, hdc, rgbcolor, U2PXROT(x11,y11), U2PYROT(x11,y11), U2PXROT(x12,y12), U2PYROT(x12,y12) );
	
			ElementNum++;
			if(ElementNum>=pLType->ElementsNumber)
				ElementNum=0;

			y11 = y12;
			x11 = x12;
			y12 = y11 + MoveDirection * fabs(pLType->Elements[ElementNum]*Scale)*(cosalpha);
			x12 = x11 + (y12-y11)*(m);
		}
		if(pLType->Elements[ElementNum]>=0)
//			DrawLine( pDrawing, hdc, rgbcolor, U2PX(x11), U2PY(y11), U2PX(x2), U2PY(y2) );
			DrawLine( pDrawing, hdc, rgbcolor, U2PXROT(x11,y11), U2PYROT(x11,y11), U2PXROT(x2,y2), U2PYROT(x2,y2) );
	}
}

void DrawNurbs(PDRAWING pDrawing,  HDC hdc, PREALPOINT CP, double *Knot, int Count, double x, double y, double xScale, double yScale, double Rotation)
{
	double t;
	double StepT;
	int j;
	REALPOINT p;
    
	p = CP[0];
	MoveToEx(hdc, U2PXROT(p.x, p.y), U2PYROT(p.x, p.y), 0); //从第一个点开始画
	for (j = 3; j < Count; j++)
	{
		StepT = (Knot[j + 1] - Knot[j]) / 25;
		for (t = Knot[j]; StepT && t < Knot[j + 1]; t += StepT)
		{
			p = NURBS_3(CP, Knot, j, t);
			LineTo(hdc, U2PXROT(p.x, p.y), U2PYROT(p.x, p.y));
		}
	}
	p = CP[Count - 1];
	LineTo(hdc, U2PXROT(p.x, p.y), U2PYROT(p.x, p.y));
}

void DrawPolygonMesh(PDRAWING pDrawing,  HDC hdc, PENTPOLYLINE pPolyline, COLORREF rgbcolor, double x, double y, double xScale, double yScale, double Rotation)
{
	if (pPolyline->pPolyPoints == NULL)
	{
		return;
	}
	assert(pPolyline->nPolyPoints > 0);

	int M = pPolyline->MeshM;
	int N = pPolyline->MeshN;
	bool flag = pPolyline->IsMeshNClosed /*&& pPolyLine->Lines.IsSolid()*/;
	vector<Point> dottedSingPoints;
	Point dottedSingPoint;

	for (int i = 0; i < pPolyline->nPolyPoints; i++)
	{
		dottedSingPoint.X = U2PXROT(pPolyline->pPolyPoints[i].x, pPolyline->pPolyPoints[i].y);
		dottedSingPoint.Y = U2PYROT(pPolyline->pPolyPoints[i].x, pPolyline->pPolyPoints[i].y);
		dottedSingPoints.push_back(dottedSingPoint);

		if (flag && (i + 1) % N == 0)
		{
			unsigned int Size = dottedSingPoints.size();
			dottedSingPoints.push_back(dottedSingPoints[Size - N]);
		}
	}
	int *array = NULL;
	int num = 0;

	if (/*pPolyLine->Lines.IsSolid() || */ (pPolyline->IsPolygonMesh /*&& !pPolyLine->IsPolyZThickness*/))
	{
		num = dottedSingPoints.size();
		int b = pPolyline->IsMeshNClosed ? 1 : 0;
		int b2 = pPolyline->IsMeshMClosed ? 1 : 0;
		int i = 0;
		for (i = 0; i < N; i++)
		{
			for (int j = i; j < num; j += N + b)
			{
				dottedSingPoints.push_back(dottedSingPoints[j]);
			}
			if (b2 == 1)
			{
				int j = i;
				dottedSingPoints.push_back(dottedSingPoints[j]);
			}
		}
		num = M + N;
		array = new int[num];
		for (i = 0; i < M; i++)
		{
			array[i] = N + b;
		}
		while (i < num)
		{
			array[i] = M + b2;
			i++;
		}
	}
	else
	{
		if (pPolyline->IsMeshNClosed /*|| pPolyLine->IsPolyZThickness*/)
		{
			int num2 = (int)round((double)((float)(pPolyline->nPolyPoints / M)));
			for (int j = 1; j < M; j++)
			{
				for (int i = 0; i < num2; i++)
				{
					num = num2 * j + i;
					dottedSingPoint.X = U2PXROT(pPolyline->pPolyPoints[num].x, pPolyline->pPolyPoints[num].y);
					dottedSingPoint.Y = U2PYROT(pPolyline->pPolyPoints[num].x, pPolyline->pPolyPoints[num].y);
					dottedSingPoints.push_back(dottedSingPoint);
					num -= num2;
					dottedSingPoint.X = U2PXROT(pPolyline->pPolyPoints[num].x, pPolyline->pPolyPoints[num].y);
					dottedSingPoint.Y = U2PYROT(pPolyline->pPolyPoints[num].x, pPolyline->pPolyPoints[num].y);
					dottedSingPoints.push_back(dottedSingPoint);
				}
			}
		}
		num = dottedSingPoints.size() / 2;
		array = new int[num];
		for (int i = 0; i < num; i++)
		{
			array[i] = 2;
		}
	}

	int num3 = 0;
	GraphicsPath *graphicsPath = new GraphicsPath();
	int num4 = 0;
	while (num4 < num && num3 < (int)(dottedSingPoints.size()))
	{
		Point *array2 = new Point[array[num4]];
		for(int l = 0; l < array[num4]; l++)
		{
			array2[l] = dottedSingPoints[num3 + l];
		}
		graphicsPath->StartFigure();
		graphicsPath->AddLines(array2, array[num4]);
		num3 += array[num4];
		num4++;
		
		delete[] array2;
		array2 = NULL;
	}

	Graphics graphics(hdc);
	Pen redPen(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor)), 0);
	graphics.DrawPath(&redPen, graphicsPath);
	graphics.ReleaseHDC(hdc);

	delete graphicsPath;
	graphicsPath = NULL;
	delete[] array;
	array = NULL;
	vector<Point>().swap(dottedSingPoints);
	dottedSingPoints.clear();
}

void DrawPolyFaceMesh(PDRAWING pDrawing,  HDC hdc, PENTPOLYLINE pPolyline, COLORREF rgbcolor, double x, double y, double xScale, double yScale, double Rotation)
{
	if (pPolyline->pPolyFaceVertexIndexes == NULL)
	{
		return;
	}
	assert(pPolyline->nVertexIndexes > 3);

	SimpleList <Point> PointList;
	Point point, point2;
	int count = pPolyline->nPolyPoints;

	for (int i = 0; i < pPolyline->nVertexIndexes; i += 4)
	{
		int num = pPolyline->pPolyFaceVertexIndexes[i];
		if (num == 0)
		{
			continue;
		}
		for (int j = 1; j < 4; j++)
		{
			int num2 = pPolyline->pPolyFaceVertexIndexes[i + j];
			if (num2 == 0)
			{
				break;
			}
			int num3 = abs(num2);
			if (pPolyline->pPolyPoints != NULL && num > 0 && num2 != 0 && num <= count && num3 <= count)
			{
				point.X = U2PXROT(pPolyline->pPolyPoints[num - 1].x, pPolyline->pPolyPoints[num - 1].y);
				point.Y = U2PYROT(pPolyline->pPolyPoints[num - 1].x, pPolyline->pPolyPoints[num - 1].y);
				point2.X = U2PXROT(pPolyline->pPolyPoints[num3 - 1].x, pPolyline->pPolyPoints[num3 - 1].y);
				point2.Y = U2PYROT(pPolyline->pPolyPoints[num3 - 1].x, pPolyline->pPolyPoints[num3 - 1].y);
				if (point.Equals(point2))
				{
					SetPixel(hdc, point.X, point.Y, rgbcolor);
				}
				PointList.add(point);
				PointList.add(point2);
			}
			num = num2;
		}

		int num4 = pPolyline->pPolyFaceVertexIndexes[i];
		int num5 = abs(num4);
		if (pPolyline->pPolyPoints != NULL && num > 0 && num4 != 0 && num <= count && num5 <= count)
		{
			point.X = U2PXROT(pPolyline->pPolyPoints[num - 1].x, pPolyline->pPolyPoints[num - 1].y);
			point.Y = U2PYROT(pPolyline->pPolyPoints[num - 1].x, pPolyline->pPolyPoints[num - 1].y);
			point2.X = U2PXROT(pPolyline->pPolyPoints[num5 - 1].x, pPolyline->pPolyPoints[num5 - 1].y);
			point2.Y = U2PYROT(pPolyline->pPolyPoints[num5 - 1].x, pPolyline->pPolyPoints[num5 - 1].y);
			if (point.Equals(point2))
			{
				SetPixel(hdc, point.X, point.Y, rgbcolor);
			}
			PointList.add(point);
			PointList.add(point2);
		}
	}

	Graphics graphics(hdc);
	Pen redPen(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor)), 0);
	GraphicsPath *graphicsPath = new GraphicsPath();

	//Add GraphicsPath Points
	int nPoints = PointList.size;
	Point *pPoints = ReadSimpleListData(PointList);
	Point array[2];
	for (int l = 0; l < nPoints; l += 2)
	{
		array[0] = pPoints[l];
		array[1] = pPoints[l + 1];
		graphicsPath->StartFigure();
		graphicsPath->AddLines(array, 2);
	}

	//Draw Points
	graphics.DrawPath(&redPen, graphicsPath);

	graphics.ReleaseHDC(hdc);
	delete graphicsPath;
	graphicsPath = NULL;
	delete[] pPoints;
	pPoints = NULL;
	PointList.clear();
}

void DrawPolyLine(PDRAWING pDrawing,  HDC hdc, PENTPOLYCURVE pPolyline, COLORREF rgbcolor, double x, double y, double xScale, double yScale, double Rotation)
{
	if (pPolyline->pPolyPoints == NULL)
	{
		return;
	}
	assert(pPolyline->nPolyPoints > 0);

	int Count = pPolyline->nPolyPoints;
	Point *pPoints = (Point *)malloc(sizeof(Point) * Count);
    if (pPoints == NULL)
    {
		return;
    }

	for(int i = 0; i < Count; i++)
	{
		pPoints[i].X = U2PXROT(pPolyline->pPolyPoints[i].x, pPolyline->pPolyPoints[i].y);
		pPoints[i].Y = U2PYROT(pPolyline->pPolyPoints[i].x, pPolyline->pPolyPoints[i].y);
	}

	if (Count != 1)
	{
		Graphics graphics(hdc);
		Pen redPen(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor)), 0);

		if (pPolyline->Closed)
		{
			Point *pPointsNew = (Point *) realloc(pPoints, (1 + Count) * sizeof(Point));
			if (pPointsNew == NULL)
			{
				if (pPoints != NULL)
				{
					free(pPoints);
					pPoints = NULL;
				}
				graphics.ReleaseHDC(hdc);
				return;
			}
			pPoints = pPointsNew;
			pPoints[Count].X = U2PXROT(pPolyline->pPolyPoints[0].x, pPolyline->pPolyPoints[0].y);
			pPoints[Count].Y = U2PYROT(pPolyline->pPolyPoints[0].x, pPolyline->pPolyPoints[0].y);
			Count = Count + 1;
			graphics.DrawPolygon(&redPen, pPoints, Count);
		}
		else
		{
			graphics.DrawLines(&redPen, pPoints, Count);
		}

		graphics.ReleaseHDC(hdc);
	}
	else
	{
		SetPixel(hdc, pPoints->X, pPoints->Y, rgbcolor);
	}

	free(pPoints);
	pPoints = NULL;
}

void DrawPolyArrows(PDRAWING pDrawing,  HDC hdc, PENTPOLYLINEBASE pPolyline, COLORREF rgbcolor, double x, double y, double xScale, double yScale, double Rotation)
{
	if (pPolyline->pArrows == NULL)
	{
		return;
	}
	assert(pPolyline->nArrows > 3);

	Graphics graphics(hdc);
	Pen redPen(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor)), 0);
	SolidBrush solidBrush(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor)));

	Point *pArrows = new Point[4];
	int ArrowsCount = pPolyline->nArrows;

	for (int i = 0; i < ArrowsCount; i += 4)
	{
		for (int j = 0; j < 4; j++)
		{
			pArrows[j].X = U2PXROT(pPolyline->pArrows[i+j].x, pPolyline->pArrows[i+j].y);
			pArrows[j].Y = U2PYROT(pPolyline->pArrows[i+j].x, pPolyline->pArrows[i+j].y);
		}
		graphics.FillPolygon(&solidBrush, pArrows, 4);
		graphics.DrawPolygon(&redPen, pArrows, 4);
	}

	graphics.ReleaseHDC(hdc);
	delete[] pArrows;
	pArrows = NULL;
}

/*-------------------------------------------------------------------*
 |  PlotEntity                                                       |
 |  Descriptions:                                                    |
 |      Plots an entity into given Drawing Context                   |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      HDC hdc= handle of DC for drawing                            |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      LPVOID pEntityData = pointer to entity data structure        |
 |      double x = x position of insertion point (default = 0)       |
 |      double y = y position of insertion point (default = 0)       |
 |      double xScale = x scale factor (default = 1)                 |
 |      double yScale = y scale factor (default = 1)                 |
 |      double Rotation = rotation angle (default = 0)               |
 |  Output: Nothing                                                  |
 *-------------------------------------------------------------------*/
void PlotEntity(PDRAWING pDrawing, HDC hdc, PENTITYHEADER pEntityHeader, LPVOID pEntityData, double x=0, double y=0, double xScale=1, double yScale=1, double Rotation=0)//真正的绘图函数
{
	LAYER		Layer;
	LTYPE		LType;
	COLORREF	rgbcolor;
	short		color;
	HPEN		pen, oldpen;
	HBRUSH		brush, oldbrush;
	BOOL		isLTypeAvailable = FALSE;
	// Exit and do not plot, if this object is deleted

	if(pEntityHeader->Deleted)
		return;

	if(pEntityHeader->EntityType!=ENT_TEXT)
	{
		// Initialize LayerName & LineTypeName & Color
		color = pEntityHeader->Color;

		Layer.Objhandle = pEntityHeader->LayerObjhandle;
		if(drwFindTableType_Direct(pDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer)>0)
		{
			if((pEntityHeader->Color==COLOR_BYBLOCK) || (pEntityHeader->Color==COLOR_BYLAYER))
				color = Layer.Color;
			if (Layer.Color<0)
			{
				// Layer is off
				return;
			}
		}
		else if(color>=COLOR_BYLAYER)
			color = 0;

		rgbcolor = CadColor[color];

		pen = CreatePen(PS_SOLID, 0, rgbcolor);
		oldpen = (HPEN)SelectObject(hdc, pen);
	
		LType.Objhandle = pEntityHeader->LTypeObjhandle;
		isLTypeAvailable = drwFindTableType_Direct(pDrawing, TAB_LTYPE, FIND_BYHANDLE, &LType);
	}

	//通过移动view 的位置来改变图像的位置，图像相对于view的位置是没有改变的
	switch(pEntityHeader->EntityType)
	{
	case ENT_LINE: //已解析 Insert已解析
		{
			/*if( !(((x1*xScale+x<pDrawing->View.ViewLeft) && (x2*xScale+x<pDrawing->View.ViewLeft)) || 
			((x1*xScale+x>pDrawing->View.ViewRight) && (x2*xScale+x>pDrawing->View.ViewRight)) || 
			((y1*yScale+y<pDrawing->View.ViewBottom) && (y2*yScale+y<pDrawing->View.ViewBottom)) || 
			((y1*yScale+y>pDrawing->View.ViewTop) && (y2*yScale+y>pDrawing->View.ViewTop))) )*/
			{
				double x1 = ((PENTLINE)pEntityData)->Point0.x;
				double y1 = ((PENTLINE)pEntityData)->Point0.y;
				double x2 = ((PENTLINE)pEntityData)->Point1.x;
				double y2 = ((PENTLINE)pEntityData)->Point1.y;

				if((LType.PatternLength==0) || (!isLTypeAvailable))
				{ // Solid Line
					DrawLine(pDrawing, hdc, rgbcolor, U2PXROT(x1,y1), U2PYROT(x1,y1), U2PXROT(x2,y2), U2PYROT(x2,y2));
				}
				else
				{ // Line has Pattern
					double scale = pEntityHeader->LineTypeScale;
					double linelen = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
					double PatternStartPoint = fabs(LType.Elements[0]*scale)/2;
					double LinePatternStartPoint = fmod(linelen,LType.PatternLength*scale)/2;
					if((linelen<LType.PatternLength*scale) || (LType.PatternLength*scale*pDrawing->View.PPU*pDrawing->View.ZoomLevel<5))
					{
						DrawLine(pDrawing, hdc, rgbcolor, U2PXROT(x1,y1), U2PYROT(x1,y1), U2PXROT(x2,y2), U2PYROT(x2,y2));
					}
					else
					{
						double m, cosalpha;
						double x11, y11;
						double x21, y21;

						if(fabs(x2-x1)>fabs(y2-y1))
						{
							if(x1>x2)
							{
								double dummy;
								dummy = x1; x1 = x2; x2 = dummy;
								dummy = y1; y1 = y2; y2 = dummy;
							}
							m = (y2-y1)/(x2-x1);
							cosalpha = 1 / sqrt(1+m*m);
							x11 = x1 + LinePatternStartPoint*cosalpha;
							y11 = y1 + (x11-x1)*m;
							x21 = x2 - LinePatternStartPoint*cosalpha;
							y21 = y2 + (x21-x2)*m;
						}
						else
						{
							if(y1>y2)
							{
								double dummy;
								dummy = x1; x1 = x2; x2 = dummy;
								dummy = y1; y1 = y2; y2 = dummy;
							}
							m = (x2-x1)/(y2-y1);
							cosalpha = 1 / sqrt(1+m*m);
							y11 = y1 + LinePatternStartPoint*cosalpha;
							x11 = x1 + (y11-y1)*m;
							y21 = y2 - LinePatternStartPoint*cosalpha;
							x21 = x2 + (y21-y2)*m;
						}

						DrawLine( pDrawing, hdc, rgbcolor, U2PXROT(x1,y1), U2PYROT(x1,y1), U2PXROT(x11,y11), U2PYROT(x11,y11) );
						DrawDashLine(pDrawing, hdc, &LType, x11, y11, x21, y21, rgbcolor, scale, PatternStartPoint, x, y, xScale, yScale, Rotation);
						DrawLine( pDrawing, hdc, rgbcolor, U2PXROT(x21,y21), U2PYROT(x21,y21), U2PXROT(x2,y2), U2PYROT(x2,y2) );
					}
				}
			}
		}
		break;

	case ENT_POINT: //已解析 Insert已解析
		{
			SetPixel(
				hdc,
				U2PXROT(((PENTPOINT)pEntityData)->Point0.x, ((PENTPOINT)pEntityData)->Point0.y),
				U2PYROT(((PENTPOINT)pEntityData)->Point0.x, ((PENTPOINT)pEntityData)->Point0.y),
				rgbcolor
				);
		}
		break;

	case ENT_CIRCLE: //已解析 Insert已解析
		{
			double x1,y1,x2,y2,xc,yc,r;
			x1 = ((PENTCIRCLE)pEntityData)->Point0.x-((PENTCIRCLE)pEntityData)->Radius;
			y1 = ((PENTCIRCLE)pEntityData)->Point0.y-((PENTCIRCLE)pEntityData)->Radius;
			x2 = ((PENTCIRCLE)pEntityData)->Point0.x+((PENTCIRCLE)pEntityData)->Radius;
			y2 = ((PENTCIRCLE)pEntityData)->Point0.y+((PENTCIRCLE)pEntityData)->Radius;
			xc = ((PENTCIRCLE)pEntityData)->Point0.x;
			yc = ((PENTCIRCLE)pEntityData)->Point0.y;
			r = ((PENTCIRCLE)pEntityData)->Radius;

			if((LType.PatternLength==0) || (!isLTypeAvailable))
			{ // Solid Line Circle
				Graphics graphics(hdc);
				Pen redPen(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor)), 0);

				double xcrot = U2PXROT(xc,yc);
				double ycrot = U2PYROT(xc,yc);
				int xr = abs(int(r*xScale*pDrawing->View.ZoomLevel*pDrawing->View.PPU));
				int yr = abs(int(r*yScale*pDrawing->View.ZoomLevel*pDrawing->View.PPU));
				double angle = Rotation*PI/180;

				double xm = xcrot* cos(-angle) - ycrot * sin(-angle);
				double ym = xcrot * sin(-angle) + ycrot * cos(-angle);
				graphics.RotateTransform(REAL(-Rotation), MatrixOrderAppend);
				graphics.TranslateTransform(REAL(xcrot-xm), REAL(ycrot-ym), MatrixOrderAppend);

				Rect  rect(INT(xcrot-xr),INT(ycrot-yr), 2*xr, 2*yr);
				graphics.DrawEllipse(&redPen, rect);
				graphics.ResetTransform();
				graphics.ReleaseHDC(hdc);
			}
			else
			{ // Circle has Pattern
				double scale = pEntityHeader->LineTypeScale;

				double primeter = 2*PI*r*xScale;

				if((primeter<LType.PatternLength*scale) || (LType.PatternLength*scale*pDrawing->View.PPU*pDrawing->View.ZoomLevel<5))
				{
					Ellipse(
						hdc,
						U2PXROT(x1,y1),
						U2PYROT(x1,y1),
						U2PXROT(x2,y2),
						U2PYROT(x2,y2)
						);
				}
				else
				{
					double n, alpha;
					double x11, y11;
					double x21, y21;
					double PatternStartPoint=0;
					double len = 0;

					scale = primeter/ceil(primeter/(LType.PatternLength*scale))/LType.PatternLength;
					n = ceil((primeter*pDrawing->View.ZoomLevel*pDrawing->View.PPU) / 5);
					if(n<8)	n = 8;
					if(n>100)
						n=100;
					alpha = 2*PI/n;

					x21 = xc + r;
					y21 = yc;
					for(int i=1; i<=n; i++)
					{
						x11 = x21;
						y11 = y21;
						x21 = xc + r*cos(alpha*double(i));
						y21 = yc + r*sin(alpha*double(i));
						len = sqrt((x11-x21)*(x11-x21)+(y11-y21)*(y11-y21));
						DrawDashLine(pDrawing, hdc, &LType, x11, y11, x21, y21, rgbcolor, scale, PatternStartPoint, x, y, xScale, yScale, Rotation);
						PatternStartPoint = fmod(PatternStartPoint+len,LType.PatternLength*scale);
					}
				}
			}
		}
		break;

	case ENT_ARC: //已解析 Insert已解析
		{
			double r = ((PENTARC)pEntityData)->Radius;
			int xr = U2PXROT(((PENTARC)pEntityData)->Point0.x, ((PENTARC)pEntityData)->Point0.y); 
			int yr = U2PYROT(((PENTARC)pEntityData)->Point0.x, ((PENTARC)pEntityData)->Point0.y); 

			int xc = abs(int(r*pDrawing->View.ZoomLevel*pDrawing->View.PPU));
			int yc = abs(int(r*pDrawing->View.ZoomLevel*pDrawing->View.PPU));

			Graphics graphics(hdc);
			Pen redPen(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor)), 0);
			
			double angle = Rotation*PI/180;
			double xm = xr* cos(-angle) - yr * sin(-angle);
			double ym = xr * sin(-angle) + yr * cos(-angle);
			graphics.RotateTransform((REAL)-Rotation, MatrixOrderAppend);
			graphics.TranslateTransform((REAL)(xr-xm), (REAL)(yr-ym), MatrixOrderAppend);

			graphics.TranslateTransform((REAL)xr, (REAL)yr);
			graphics.ScaleTransform((REAL)xScale, (REAL)yScale);
			//恢复绘图平面在水平和垂直方向的平移
			graphics.TranslateTransform((REAL)-xr, (REAL)-yr);

			double AngleStart = ((PENTARC)pEntityData)->StartAngle;
			double AngleEnd =  ((PENTARC)pEntityData)->EndAngle;
			double Angle = AngleStart < AngleEnd ? fabs(AngleEnd-AngleStart) : fabs(360-AngleStart+AngleEnd);

			graphics.DrawArc(&redPen, (REAL)(xr-xc), (REAL)(yr-yc), (REAL)(2*xc), (REAL)(2*yc), (REAL)CalCadAngleToGdiAngle(AngleEnd*PI/180) ,(REAL)Angle);
			graphics.ResetTransform();
			graphics.ReleaseHDC(hdc);
		}
		break;

	case ENT_ELLIPSE: //已解析 Insert已解析
		{
			PENTELLIPSE ellipse=(PENTELLIPSE)pEntityData;
			int xr = U2PXROT(ellipse->CenterPoint.x, ellipse->CenterPoint.y);
			int yr = U2PYROT(ellipse->CenterPoint.x, ellipse->CenterPoint.y);

			double longAxis=sqrt(ellipse->MajorAxisEndPoint.x*ellipse->MajorAxisEndPoint.x+ellipse->MajorAxisEndPoint.y*ellipse->MajorAxisEndPoint.y);
			int xc = abs(int(longAxis*pDrawing->View.ZoomLevel*pDrawing->View.PPU)); //长轴
			int yc = abs(int(longAxis*ellipse->MinorToMajorRatio*pDrawing->View.ZoomLevel*pDrawing->View.PPU)); //短轴

			double Angle=atan2(ellipse->MajorAxisEndPoint.y,ellipse->MajorAxisEndPoint.x);//角度

			Graphics graphics(hdc);
			Pen redPen(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor)), 0);

			double angle = Rotation*PI/180;
			double xm = xr* cos(-angle) - yr * sin(-angle);
			double ym = xr * sin(-angle) + yr * cos(-angle);
			graphics.RotateTransform((REAL)-Rotation, MatrixOrderAppend);
			graphics.TranslateTransform((REAL)(xr-xm), (REAL)(yr-ym), MatrixOrderAppend);

			graphics.TranslateTransform((REAL)xr, (REAL)yr);
			graphics.ScaleTransform((REAL)xScale, (REAL)yScale);
			graphics.RotateTransform((REAL)(-Angle*180/PI));
			//恢复绘图平面在水平和垂直方向的平移
			graphics.TranslateTransform((REAL)-xr, (REAL)-yr);

			graphics.SetSmoothingMode(SmoothingModeHighSpeed);
			graphics.DrawArc(&redPen
				,(REAL)(xr-xc)
				,(REAL)(yr-yc)
				,(REAL)(2*xc)
				,(REAL)(2*yc)
				,(REAL)(ellipse->ExtrudeDirectZ < 0 ? ellipse->StartParam*180/PI : min(-ellipse->StartParam*180/PI, -ellipse->EndParam*180/PI))
				,(REAL)(fabs(ellipse->StartParam-ellipse->EndParam)*180/PI));//后两个参数为起点角度和终点角度
			graphics.ResetTransform();
			graphics.ReleaseHDC(hdc);
		}
		break;

	case ENT_TEXT: //已解析 Insert已解析
		{
			PlotText(
				pDrawing,
				hdc,
				pEntityHeader,
				(PENTTEXT)pEntityData,
				x, y,
				xScale, yScale,
				Rotation
				);
		}
		break;

	case ENT_MTEXT: //已解析 Insert已解析
		{
			PlotMText(
				pDrawing,
				hdc, 
				pEntityHeader,
				(PENTMTEXT)pEntityData,
				x, y,
				xScale, yScale,
				Rotation
				);
		}
		break;

	case ENT_SOLID: //已解析 Insert已解析
		{
			POINT	p[4]; 
			p[0].x = U2PXROT(((PENTSOLID)pEntityData)->Point0.x, ((PENTSOLID)pEntityData)->Point0.y);
			p[0].y = U2PYROT(((PENTSOLID)pEntityData)->Point0.x, ((PENTSOLID)pEntityData)->Point0.y);
			p[1].x = U2PXROT(((PENTSOLID)pEntityData)->Point1.x, ((PENTSOLID)pEntityData)->Point1.y);
			p[1].y = U2PYROT(((PENTSOLID)pEntityData)->Point1.x, ((PENTSOLID)pEntityData)->Point1.y);
			p[3].x = U2PXROT(((PENTSOLID)pEntityData)->Point2.x, ((PENTSOLID)pEntityData)->Point2.y);
			p[3].y = U2PYROT(((PENTSOLID)pEntityData)->Point2.x, ((PENTSOLID)pEntityData)->Point2.y);
			p[2].x = U2PXROT(((PENTSOLID)pEntityData)->Point3.x, ((PENTSOLID)pEntityData)->Point3.y);
			p[2].y = U2PYROT(((PENTSOLID)pEntityData)->Point3.x, ((PENTSOLID)pEntityData)->Point3.y);

			brush = CreateSolidBrush(rgbcolor);
			oldbrush = (HBRUSH)SelectObject(hdc, brush);
			Polygon(hdc, p, 4);
			SelectObject(hdc, oldbrush);
			DeleteObject(brush);
		}
		break;

	case ENT_SPLINE: //已解析 Insert已解析
		{
			PDXFENTSPLINE Spline=(PDXFENTSPLINE)pEntityData;

			HBRUSH brush = CreateSolidBrush(rgbcolor);		
			HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);

			double *Knot = Spline->pNodePoint; //节点值
			int n = Spline->pControlNode->getSize();
			REALPOINT *pControlPoints = ReadSimpleListData(*Spline->pControlNode); //控制点值
		    
			DrawNurbs(pDrawing, hdc, pControlPoints, Knot, n, x, y, xScale, yScale, Rotation);

			delete[] pControlPoints;
			pControlPoints = NULL;

			SelectObject(hdc, oldbrush);
			DeleteObject(brush);
		}
		break;

	case ENT_MLINE: //已解析 Insert 已解析
		{
			PDXFENTMLINE mline=(PDXFENTMLINE)pEntityData;
			mline->pVetexPoint->start();
			mline->pVetexPoint->next();
			MoveToEx(
				hdc,
				U2PXROT(mline->pVetexPoint->get().VetexPoint.x, mline->pVetexPoint->get().VetexPoint.y),
				U2PYROT(mline->pVetexPoint->get().VetexPoint.x, mline->pVetexPoint->get().VetexPoint.y),
				NULL
				);

			while(mline->pVetexPoint->next())
			{
				LineTo(
					hdc,
					U2PXROT(mline->pVetexPoint->get().VetexPoint.x, mline->pVetexPoint->get().VetexPoint.y),
					U2PYROT(mline->pVetexPoint->get().VetexPoint.x, mline->pVetexPoint->get().VetexPoint.y)
					);
			}

			mline->pVetexPoint->start();
			mline->pVetexPoint->next();

			double xstart, ystart, xnext, ynext;
			xstart = mline->pVetexPoint->get().VetexPoint.x+mline->pVetexPoint->get().pElemParem[2]*mline->pVetexPoint->get().DirVetex.y;
			ystart = mline->pVetexPoint->get().VetexPoint.y+mline->pVetexPoint->get().pElemParem[2]*mline->pVetexPoint->get().DirVetex.x;

			MoveToEx(
				hdc,
				U2PXROT(xstart, ystart),
				U2PYROT(xstart, ystart),
				NULL
				);
			while(mline->pVetexPoint->next())
			{
				xnext = mline->pVetexPoint->get().VetexPoint.x+mline->pVetexPoint->get().pElemParem[2]*mline->pVetexPoint->get().DirVetex.y;
				ynext = mline->pVetexPoint->get().VetexPoint.y+mline->pVetexPoint->get().pElemParem[2]*mline->pVetexPoint->get().DirVetex.x;
				LineTo(
					hdc,
					U2PXROT(xnext, ynext),
					U2PYROT(xnext, ynext)
					);
			}

		}
		break;

	case ENT_POLYLINE: //已解析 Insert已解析
		{
			PENTPOLYLINE pPolyline = (PENTPOLYLINE)pEntityData;

			if (pPolyline->IsPolygonMesh) //多边形网格
			{
				DrawPolygonMesh(pDrawing, hdc, pPolyline, rgbcolor, x, y, xScale, yScale, Rotation);
				break;
			}
			if (pPolyline->IsPolyFaceMesh) //多面网格
			{
				DrawPolyFaceMesh(pDrawing, hdc, pPolyline, rgbcolor, x, y, xScale, yScale, Rotation);
				break;
			}

			DrawPolyLine(pDrawing, hdc, pPolyline, rgbcolor, x, y, xScale, yScale, Rotation); //三维多段线
			DrawPolyArrows(pDrawing, hdc, pPolyline, rgbcolor, x, y, xScale, yScale, Rotation); //多段线宽度不为零
		}
		break;

	case ENT_LWPOLYLINE: //已解析 Insert已解析
		{	
			PENTLWPOLYLINE LWPolyLine=(PENTLWPOLYLINE)pEntityData;

			DrawPolyLine(pDrawing, hdc, LWPolyLine, rgbcolor, x, y, xScale, yScale, Rotation); //优化多段线
			DrawPolyArrows(pDrawing, hdc, LWPolyLine, rgbcolor, x, y, xScale, yScale, Rotation); //优化多段线宽度不为零
		}
		break;

	case ENT_INSERT:  //已解析 Insert已解析
		{
			BLOCKHEADER		BlockHeader;
			ENTITYHEADER	 BlockEntityHeader;
			char			BlockEntityData[4096];
			//BlockHeader.Objhandle = ((PENTINSERT)pEntityData)->BlockHeaderObjhandle;
			strcpy_s(BlockHeader.Name, ((PENTINSERT)pEntityData)->BlockHeaderName);
			if(drwFindBlock_Direct(pDrawing, /*FIND_BYHANDLE*/FIND_BYNAME, &BlockHeader)>0)
			{
				if(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_FIRST)>0)
				{
					do{
						if(BlockEntityHeader.Color==0)
							BlockEntityHeader.Color = color;

						double StretchDirectZ = ((PENTINSERT)pEntityData)->StretchDirectZ;
						double xc = ((PENTINSERT)pEntityData)->Point0.x*StretchDirectZ;
						double yc = ((PENTINSERT)pEntityData)->Point0.y;
						double flag = fabs(xScale)/(xScale)*fabs(yScale)/(yScale);
						double angle = Rotation*PI/180;

						PlotEntity(
							pDrawing,
							hdc,
							&BlockEntityHeader,
							&BlockEntityData,
							(xc*cos(angle)*xScale-yc*sin(angle)*yScale)+x,
							(xc*sin(angle)*xScale+yc*cos(angle)*yScale)+y,
							((PENTINSERT)pEntityData)->XScale*xScale*StretchDirectZ,
							((PENTINSERT)pEntityData)->YScale*yScale,
							((PENTINSERT)pEntityData)->RotationAngle*flag*StretchDirectZ+Rotation
							);
					} while(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_NEXT)>0);
				}
			}
		}
		break;

	case ENT_LEADER: //解析有问题 
		{
			MoveToEx(
				hdc,
				U2PX(((PENTLEADER)pEntityData)->pVertex[0].Point.x),
				U2PY(((PENTLEADER)pEntityData)->pVertex[0].Point.y),
				NULL
				);
			for(int i=1; i<((PENTLEADER)pEntityData)->nVertex; i++)
			{
				//if(int(((PENTPOLYLINE)pEntityData)->pVertex[i].Bulge*100)==0)
				LineTo(
					hdc,
					U2PX(((PENTLEADER)pEntityData)->pVertex[i].Point.x),
					U2PY(((PENTLEADER)pEntityData)->pVertex[i].Point.y));
			}
			if(((PENTLEADER)pEntityData)->Flag & POLYLINE_CLOSED)
				LineTo(
				hdc,
				U2PX(((PENTLEADER)pEntityData)->pVertex[0].Point.x),
				U2PY(((PENTLEADER)pEntityData)->pVertex[0].Point.y)
				);
		}
		break;

	case ENT_DIMENSION: //已解析 Insert已解析
		{
			BLOCKHEADER		DimBlockHeader;
			ENTITYHEADER	 DimBlockEntityHeader;
			char			DimBlockEntityData[4096];
			DimBlockHeader.Objhandle = ((PENTDIMENSION)pEntityData)->BlockheaderObjhandle;
			if(drwFindBlock_Direct(pDrawing, FIND_BYHANDLE, &DimBlockHeader)>0)
			{
				//			LAYER	BlockLayer;
				//			Layer.Objhandle = BlockHeader.LayerObjhandle;
				//			drwFindTableType(hDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer);

				if(drwFindEntity_Direct(pDrawing, DimBlockHeader.Name, &DimBlockEntityHeader, &DimBlockEntityData, FIND_FIRST)>0)
				{
					do{
						if(DimBlockEntityHeader.Color==0)
							DimBlockEntityHeader.Color = color;

						PlotEntity(
							pDrawing,
							hdc,
							&DimBlockEntityHeader,
							&DimBlockEntityData,
							0+x,
							0+y,
							1*xScale,
							1*yScale,
							0+Rotation
							);
					} while(drwFindEntity_Direct(pDrawing, DimBlockHeader.Name, &DimBlockEntityHeader, &DimBlockEntityData, FIND_NEXT)>0);
				}
			}
		}
		break;

	case ENT_HATCH: //已解析 Insert已解析
		{
			PENTHATCH pHatch = (PENTHATCH)pEntityData;
			Graphics graphics(hdc);
			Color HatchColor(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor))); //图案填充颜色
			Pen Pen(HatchColor, 0);

			if (pHatch->HatchType == 0) //图案填充
			{
				GraphicsPath *graphicspath = new GraphicsPath();

				//CAD点坐标转化为GDI+坐标
				int Num = pHatch->ParseLinesNum;
				if (Num > 0 && NULL != pHatch->pParseLines)
				{
					Point *pParseLines = new Point[Num];
					Point array[2];
					for (int i = 0; i < Num; i++)
					{
						pParseLines[i].X = U2PXROT(pHatch->pParseLines[i].x, pHatch->pParseLines[i].y);
						pParseLines[i].Y = U2PYROT(pHatch->pParseLines[i].x, pHatch->pParseLines[i].y);
						if ((i + 1) % 2 == 0)
						{
							if (pParseLines[i - 1].X == pParseLines[i].X && pParseLines[i - 1].Y == pParseLines[i].Y)
							{
								SetPixel(hdc, pParseLines[i].X, pParseLines[i].Y, rgbcolor);
							}
							array[0] = pParseLines[i - 1];
							array[1] = pParseLines[i];
							graphicspath->StartFigure();
							graphicspath->AddLines(array, 2);
						}
					}
					graphics.DrawPath(&Pen, graphicspath);
					delete[] pParseLines;
					pParseLines = NULL;
				}
				delete graphicspath;
				graphicspath = NULL;
			}
			else //实体填充
			{
				PBOUNDARIES pBoundaries = pHatch->pBoundaries;
				Point *pPoints = NULL;
				BYTE *pTypes = NULL;
				int N = 0;

				for (int i = 0; i < pHatch->BoundaryPathNum; i++)
				{
					int Num = pBoundaries->BoundaryPointNum;

					Point *pPointsNew = (Point*) realloc(pPoints, (N + Num) * sizeof(Point));
					BYTE *pTypesNew = (BYTE*) realloc(pTypes, (N + Num) * sizeof(BYTE));
					if (pPointsNew == NULL || pTypesNew == NULL)
					{
						if (pPoints != NULL)
						{
							free(pPoints);
							pPoints = NULL;
						}
						if (pTypes != NULL)
						{
							free(pTypes);
							pTypes = NULL;
						}
						graphics.ReleaseHDC(hdc);
						return;
					}
					pPoints = pPointsNew;
					pTypes = pTypesNew;

					//转化点坐标
					for (int j = 0; j < Num; j++)
					{
						pPoints[N + j].X = U2PXROT(pBoundaries->pBoundaryPoint[j].x, pBoundaries->pBoundaryPoint[j].y);
						pPoints[N + j].Y = U2PYROT(pBoundaries->pBoundaryPoint[j].x, pBoundaries->pBoundaryPoint[j].y);
						pTypes[N + j] = 1;
					}

					N+= Num;
					pBoundaries++;
				}
				pTypes[0] = 0;
				pTypes[N - 1] = 129;

				//转化为Region
				GraphicsPath *graphicspath = new GraphicsPath(pPoints, pTypes, N);
				Region region;
				region.MakeEmpty();
				region.Xor(graphicspath);
				RectF bounds;
				region.GetBounds(&bounds, &graphics);
				if (EqualDouble(bounds.Width, 0.0f))
				{
					bounds.Width += 1.0f;
				}
				if (EqualDouble(bounds.Height, 0.0f))
				{
					bounds.Height += 1.0f;
				}

				switch(pHatch->pHatchSolid->SolidPatternType)
				{
				case 0:
					{
						SolidBrush brush(HatchColor);
						graphics.FillRegion(&brush, &region);
					}
					break;
				case 1:
					{
						unsigned int flag = -1, flag2 = -1;
						char *gName = pHatch->pHatchSolid->GradientName;

						Color c(pHatch->pHatchSolid->GradientColor[0]);
						Color color = Color(255 - c.GetAlpha(), c.GetR(), c.GetG(), c.GetB());

						Color c2(pHatch->pHatchSolid->GradientColor[1]);
						Color color2 = Color(255 - c2.GetAlpha(), c2.GetR(), c2.GetG(), c2.GetB());

						REAL angle = (REAL)CalCadAngleToGdiAngle(pHatch->pHatchSolid->ColorAngle);
						BOOL ref = pHatch->pHatchSolid->GradientRef;

						//直线形渐变色
						if (strcmp(gName, "LINEAR") == 0)
						{
							LinearGradientBrush linearGradientBrush(bounds, color, color2, angle, true);
							if (!ref) //居中选项
							{
								REAL factors[6] = {0.0f, 0.3f, 0.5f, 0.6f, 0.7f, 1.0f};
								REAL position[6] = {0.0f, 0.2f, 0.3f, 0.4f, 0.6f, 1.0f};
								linearGradientBrush.SetBlend(factors, position, 6);
							}
							graphics.FillRegion(&linearGradientBrush, &region);
						}

						//圆柱形or反转圆柱形渐变色
						else if ((flag = strcmp(gName, "CYLINDER")) == 0 || (flag2 = strcmp(gName, "INVCYLINDER")) == 0)
						{
							LinearGradientBrush linearGradientBrush(bounds, 
								(flag == 0 ) ? color : color2,
								(flag2 == 0) ? color : color2,
								angle, true);

							if (ref) //居中选项
							{
								REAL factors[11] = {0.0f, 0.3f, 0.6f, 0.8f, 0.95f, 1.0f, 0.95f, 0.8f, 0.6f, 0.3f, 0.0f};
								REAL position[11] = {0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
								linearGradientBrush.SetBlend(factors, position, 11);
							}
							else
							{
								REAL factors[11] = {0.0f, 0.65f, 0.9f, 0.95f, 0.95f, 0.9f, 0.8f, 0.7f, 0.5f, 0.3f, 0.0f};
								REAL position[11] = {0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
								linearGradientBrush.SetBlend(factors, position, 11);
							}
							graphics.FillRegion(&linearGradientBrush, &region);
						}

						//球形or反转球形渐变色
						else if ((flag = strcmp(gName, "SPHERICAL")) == 0 || (flag2 = strcmp(gName, "INVSPHERICAL")) == 0)
						{
							GraphicsPath Gpath;
							REAL dx = bounds.X + bounds.Width / 2;
							REAL dy = bounds.Y + bounds.Height / 2;
							REAL l = bounds.Width > bounds.Height ? bounds.Width : bounds.Height;
							REAL d = (REAL)(sqrt(2.0f) * l);
							Gpath.AddArc(dx - d / 2, dy - d / 2, d, d, 0, 360);

							PathGradientBrush pathGradientBrush(&Gpath);
							pathGradientBrush.SetCenterColor(flag == 0 ? color2 : color);
							INT N = 1;
							pathGradientBrush.SetSurroundColors(&(flag2 == 0 ? color2 : color), &N);

							if (ref) //居中选项
							{
								pathGradientBrush.SetCenterPoint(Point((INT)dx - 1, (INT)dy)); //纠正偏差
								REAL factors[11] = {0.0f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.0f};
								REAL position[11] = {0.0f, 0.4f, 0.45f, 0.5f, 0.55f, 0.6f, 0.65f, 0.7f, 0.8f, 0.9f, 1.0f};
								pathGradientBrush.SetBlend(factors, position, 11);
							} 
							else
							{
								pathGradientBrush.SetCenterPoint(Point((INT)bounds.X, (INT)bounds.Y)); //不居中选项
								REAL factors[11] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.3f, 0.6f, 0.8f, 0.9f, 0.95f, 1.0f};
								REAL position[11] = {0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
								pathGradientBrush.SetBlend(factors, position, 11);
							}

							Matrix *matrix = new Matrix();
							matrix->RotateAt(angle, PointF(dx, dy));
							pathGradientBrush.SetTransform(matrix);
							graphics.FillRegion(&pathGradientBrush, &region);
							delete matrix;
						}

						//半球形/反转半球形渐变色
						else if ((flag = strcmp(gName, "HEMISPHERICAL")) == 0 || (flag2 = strcmp(gName, "INVHEMISPHERICAL")) == 0)
						{
							GraphicsPath Gpath;
							PointF point = PointF((REAL)(bounds.X + bounds.Width / 2), (REAL)(bounds.Y + bounds.Height / 2));
							float num2 = (float)sqrt(Sqr((double)bounds.Width) + Sqr((double)bounds.Height));
							num2 *= 1.5f;
							RectF rect = RectF(point.X - num2 / 2.0f, point.Y - num2 / 2.0f, num2, num2);
							Gpath.AddEllipse(rect);

							PathGradientBrush pathGradientBrush(&Gpath);
							pathGradientBrush.SetCenterColor(flag == 0 ? color2 : color);
							INT N = 1;
							pathGradientBrush.SetSurroundColors(&(flag2 == 0 ? color2 : color), &N);

							//居中选项
							pathGradientBrush.SetCenterPoint(
								Point((INT)(rect.GetLeft() + rect.Width / (ref ? 2.0f : 10.0f)), 
								(INT)(rect.GetBottom() - rect.Height / 8.0f)));

							REAL factors[11] = {0.0f, 0.0f, 0.0f, 0.0f, 0.3f, 0.6f, 0.8f, 0.9f, 0.9f, 1.0f, 1.0f};
							REAL position[11] = {0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
							pathGradientBrush.SetBlend(factors, position, 11);

							Matrix *matrix = new Matrix();
							matrix->RotateAt(angle, point);
							pathGradientBrush.SetTransform(matrix);
							graphics.FillRegion(&pathGradientBrush, &region);
							delete matrix;
						}

						//曲线形/反转曲线形渐变色
						else if ((flag = strcmp(gName, "CURVED")) == 0 || (flag2 = strcmp(gName, "INVCURVED")) == 0)
						{
							GraphicsPath Gpath;
							REAL dx = bounds.X + bounds.Width / 2;
							REAL dy = bounds.Y + bounds.Height / 2;
							REAL d = (bounds.Width > bounds.Height ? bounds.Width : bounds.Height);

							PointF Array[] = 
							{
								PointF(dx - d, dy + d ),
								PointF(dx - d, dy - d ),
								PointF(dx + d, dy - d),
								PointF(dx + d, dy + d)
							};
							Gpath.AddCurve(Array, 4, ref ? 0.5f : 0.8f);

							PathGradientBrush pathGradientBrush(&Gpath);							
							pathGradientBrush.SetCenterColor(flag == 0 ? color2 : color);
							INT N = 1;
							pathGradientBrush.SetSurroundColors(&(flag2 == 0 ? color2 : color), &N);

							//居中选项
							pathGradientBrush.SetCenterPoint(Point((INT)(ref ? dx : dx - 0.5f * d), (INT)(ref ? dy + 1.5f * d : dy + 2.0f * d)));

							REAL factors[11] = {0.0f, 0.0f, 0.0f, 0.2f, 0.6f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
							REAL position[11] = {0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
							pathGradientBrush.SetBlend(factors, position, 11);

							Matrix *matrix = new Matrix();
							matrix->RotateAt(angle, PointF(dx, dy));
							pathGradientBrush.SetTransform(matrix);
							graphics.FillRegion(&pathGradientBrush, &region);
							delete matrix;
						}

						else
						{
							//set defaults
						}
					}
					break;
				default:
					break;
				}

				delete graphicspath;
				graphicspath = NULL;
				free(pPoints);
				pPoints = NULL;
				free(pTypes);
				pTypes = NULL;
			}

			graphics.ReleaseHDC(hdc);
		}
		break;	

	case ENT_3DFACE: //已解析 Insert已解析
		{
			POINT	p[4]; 
			p[0].x = U2PXROT(((PENT3DFACE)pEntityData)->Point0.x, ((PENT3DFACE)pEntityData)->Point0.y);
			p[0].y = U2PYROT(((PENT3DFACE)pEntityData)->Point0.x, ((PENT3DFACE)pEntityData)->Point0.y);
			p[1].x = U2PXROT(((PENT3DFACE)pEntityData)->Point1.x, ((PENT3DFACE)pEntityData)->Point1.y);
			p[1].y = U2PYROT(((PENT3DFACE)pEntityData)->Point1.x, ((PENT3DFACE)pEntityData)->Point1.y);
			p[2].x = U2PXROT(((PENT3DFACE)pEntityData)->Point2.x, ((PENT3DFACE)pEntityData)->Point2.y);
			p[2].y = U2PYROT(((PENT3DFACE)pEntityData)->Point2.x, ((PENT3DFACE)pEntityData)->Point2.y);
			p[3].x = U2PXROT(((PENT3DFACE)pEntityData)->Point3.x, ((PENT3DFACE)pEntityData)->Point3.y);
			p[3].y = U2PYROT(((PENT3DFACE)pEntityData)->Point3.x, ((PENT3DFACE)pEntityData)->Point3.y);

			if(((PENT3DFACE)pEntityData)->FirstSideVisible)
			{
				MoveToEx(hdc, p[0].x, p[0].y, NULL);
				LineTo(hdc, p[1].x, p[1].y);
			}
			if(((PENT3DFACE)pEntityData)->SecondSideVisible)
			{
				MoveToEx(hdc, p[1].x, p[1].y, NULL);
				LineTo(hdc, p[2].x, p[2].y);
			}
			if(((PENT3DFACE)pEntityData)->ThirdSideVisible)
			{
				MoveToEx(hdc, p[2].x, p[2].y, NULL);
				LineTo(hdc, p[3].x, p[3].y);
			}
			if(((PENT3DFACE)pEntityData)->FourthSideVisible)
			{
				MoveToEx(hdc, p[3].x, p[3].y, NULL);
				LineTo(hdc, p[0].x, p[0].y);
			}
		}
		break;

	case ENT_TRACE: //已解析 Insert已解析
		{
			Point* p = new Point[4];
			p[0].X = U2PXROT(((PENTTRACE)pEntityData)->Point0.x, ((PENTTRACE)pEntityData)->Point0.y);
			p[0].Y = U2PYROT(((PENTTRACE)pEntityData)->Point0.x, ((PENTTRACE)pEntityData)->Point0.y);
			p[1].X = U2PXROT(((PENTTRACE)pEntityData)->Point1.x, ((PENTTRACE)pEntityData)->Point1.y);
			p[1].Y = U2PYROT(((PENTTRACE)pEntityData)->Point1.x, ((PENTTRACE)pEntityData)->Point1.y);
			p[3].X = U2PXROT(((PENTTRACE)pEntityData)->Point2.x, ((PENTTRACE)pEntityData)->Point2.y);
			p[3].Y = U2PYROT(((PENTTRACE)pEntityData)->Point2.x, ((PENTTRACE)pEntityData)->Point2.y);
			p[2].X = U2PXROT(((PENTTRACE)pEntityData)->Point3.x, ((PENTTRACE)pEntityData)->Point3.y);
			p[2].Y = U2PYROT(((PENTTRACE)pEntityData)->Point3.x, ((PENTTRACE)pEntityData)->Point3.y);

			Graphics graphics(hdc);
			Pen redPen(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor)), 0);
			SolidBrush solidBrush(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor)));

			graphics.FillPolygon(&solidBrush, p, 4);
			graphics.DrawPolygon(&redPen, p, 4);
			graphics.ReleaseHDC(hdc);
			delete[] p;
			p = NULL;
		}
		break;

	case ENT_HELIX: //已解析 Insert已解析
		{
			PENTHELIX pHelix = (PENTHELIX)pEntityData;
			Graphics graphics(hdc);
			Pen redPen(Color(255, GetRValue(rgbcolor),GetGValue(rgbcolor) , GetBValue(rgbcolor)), 0);

			if (NULL != pHelix->pCoordPoint && pHelix->CoordCount > 0)
			{
				Point *pCoords = new Point[pHelix->CoordCount];
				for (int i = 0; i < pHelix->CoordCount; i++)
				{
					pCoords[i].X = U2PXROT(pHelix->pCoordPoint[i].x, pHelix->pCoordPoint[i].y);
					pCoords[i].Y = U2PYROT(pHelix->pCoordPoint[i].x, pHelix->pCoordPoint[i].y);
				}
				graphics.DrawLines(&redPen, pCoords, pHelix->CoordCount);
				delete[] pCoords;
				pCoords = NULL;
			}

			graphics.ReleaseHDC(hdc);
		}
		break;

	case ENT_ACADTABLE: //已解析 Insert已解析
		{
			BLOCKHEADER		BlockHeader;
			ENTITYHEADER	 BlockEntityHeader;
			char			BlockEntityData[4096];
			strcpy_s(BlockHeader.Name, ((PENTACADTABLE)pEntityData)->BlockHeaderName);
			if(drwFindBlock_Direct(pDrawing, FIND_BYNAME, &BlockHeader)>0)
			{
				if(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_FIRST)>0)
				{
					do{
						if(BlockEntityHeader.Color==0)
							BlockEntityHeader.Color = color;

						double xc = ((PENTACADTABLE)pEntityData)->Point.x;
						double yc = ((PENTACADTABLE)pEntityData)->Point.y;

						PlotEntity(
							pDrawing,
							hdc,
							&BlockEntityHeader,
							&BlockEntityData,
							(xc*cos(Rotation*PI/180)*xScale - yc*sin(Rotation*PI/180)*yScale) + x,
							(xc*sin(Rotation*PI/180)*xScale + yc*cos(Rotation*PI/180)*yScale) + y,
							xScale,
							yScale,
							Rotation
							);
					} while(drwFindEntity_Direct(pDrawing, BlockHeader.Name, &BlockEntityHeader, &BlockEntityData, FIND_NEXT)>0);
				}
			}
		}
		break;

	case ENT_WIPEOUT: //已解析 Insert已解析
		{
			PENTWIPEOUT pWipeOut = (PENTWIPEOUT)pEntityData;

			int nCount = pWipeOut->nPolyPoints;
			if (nCount > 2) //区域覆盖的点数大于2
			{
				Point *pPoints = new Point[nCount];
				for (int i = 0; i < nCount; i++)
				{
					pPoints[i].X = U2PXROT(pWipeOut->pPolyPoints[i].x, pWipeOut->pPolyPoints[i].y);
					pPoints[i].Y = U2PYROT(pWipeOut->pPolyPoints[i].x, pWipeOut->pPolyPoints[i].y);
				}

				Graphics graphics(hdc);
				SolidBrush solidBrush(Color(255, 0, 0, 0)); //背景色

				GraphicsPath *graphicspath = new GraphicsPath();
				graphicspath->AddPolygon(pPoints, nCount);
				graphicspath->SetFillMode(FillModeAlternate);

				Region region(graphicspath);
				graphics.SetClip(graphicspath, CombineModeReplace);
				graphics.FillRegion(&solidBrush, &region);
				graphics.ResetClip();

				graphics.ReleaseHDC(hdc);
				delete graphicspath;
				graphicspath = NULL;
				delete[] pPoints;
				pPoints = NULL;
			}

			DrawPolyLine(pDrawing, hdc, pWipeOut, rgbcolor, x, y, xScale, yScale, Rotation); //三维多段线
		}
		break;

	default:
		break;
	}

	if(pEntityHeader->EntityType!=ENT_TEXT)
	{
		SelectObject(hdc, oldpen);
		DeleteObject(pen);
	}
}

/*-------------------------------------------------------------------*
 |  drwPlot                                                          |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      HDC hdc= handle of DC for drawing                            |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwPlot( HDRAWING hDrawing, HDC hdc )
{
	PDRAWING	pDrawing;
	ENTITYHEADER	  EntityHeader;
	char	EntityData[4096]={0};

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return FALSE;
	
	ULONG_PTR  m_gdiplusToken;
	GdiplusStartupInput  gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	if(pDrawing->View.Viewable) // Drawing isViewable
	{
		// Entities Section ----------------------------------
		if(drwFindEntity_Direct(pDrawing, NULL, &EntityHeader, &EntityData, FIND_FIRST)>0)
		{
			do{
				PlotEntity(pDrawing, hdc, &EntityHeader, &EntityData);
			} while(drwFindEntity_Direct(pDrawing, NULL, &EntityHeader, &EntityData, FIND_NEXT)>0);
		}
	}

	GdiplusShutdown(m_gdiplusToken);

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}

/*-------------------------------------------------------------------*
 |  drwInitView                                                      |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      int x = horizontal position of drawing window                |
 |      int y = vertical position of drawing window                  |
 |      int Width = width of drawing window                          |
 |      int Height = height of drawing window                        |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwInitView( HDRAWING hDrawing, int x, int y, int Width, int Height )
{
	PDRAWING		pDrawing;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return NULL;

	pDrawing->View.ZoomLevel = 1;
	pDrawing->View.PPU = 20;

	pDrawing->View.WindowLeft = x;
	pDrawing->View.WindowTop = y;
	pDrawing->View.WindowRight = x + Width;
	pDrawing->View.WindowBottom = y + Height;
	pDrawing->View.ViewLeft = 0;
	pDrawing->View.ViewBottom = 0;

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}

/*-------------------------------------------------------------------*
 |  drwPaint                                                         |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |	    HDC hdc	= handle to view window DC                           |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwPaint( HDRAWING hDrawing, HDC hdc ,COLORREF color )//绘图函数
{
	PDRAWING	pDrawing;
	HRGN		ViewRgn, oldViewRgn;
	HBRUSH	ViewBrush, oldViewBrush;  
	BOOL		result = TRUE;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return FALSE;

	ViewBrush = CreateSolidBrush(color);
	oldViewBrush = (HBRUSH)SelectObject(hdc, ViewBrush);

	Rectangle(
		hdc,
		pDrawing->View.WindowLeft,
		pDrawing->View.WindowTop,
		pDrawing->View.WindowRight,
		pDrawing->View.WindowBottom
		);

	ViewRgn = CreateRectRgn(
					pDrawing->View.WindowLeft,
					pDrawing->View.WindowTop,
					pDrawing->View.WindowRight,
					pDrawing->View.WindowBottom
				);
	oldViewRgn = (HRGN)SelectObject(hdc, ViewRgn);

	result = drwPlot(hDrawing, hdc);

	DeleteObject(ViewRgn);
	DeleteObject(ViewBrush);
	SelectObject(hdc, oldViewBrush);
	SelectObject(hdc, oldViewRgn);

	// UnInitilize pDrawing -----------------
	UnInitilizePDRAWING(hDrawing);

	return result;
}

/********************************************************************
 * Functions for changing view properties                           *
 ********************************************************************/

/*-------------------------------------------------------------------*
 |  drwGetViewProperties                                             |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      PVIEW pView = pointer to view properties structure           |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwGetViewProperties( HDRAWING hDrawing, PVIEW pView )
{
	PDRAWING		pDrawing;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return FALSE;

	memcpy(pView, &pDrawing->View, sizeof(VIEW));

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}

/*-------------------------------------------------------------------*
 |  drwSetViewProperties                                             |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      PVIEW pView = pointer to view properties structure           |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwSetViewProperties( HDRAWING hDrawing, PVIEW pView )
{
	PDRAWING		pDrawing;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return FALSE;

	memcpy(&pDrawing->View, pView, sizeof(VIEW));

	pDrawing->View.ViewTop = pDrawing->View.ViewBottom + (pDrawing->View.WindowBottom-pDrawing->View.WindowTop)/pDrawing->View.ZoomLevel/pDrawing->View.PPU;
	pDrawing->View.ViewRight = pDrawing->View.ViewLeft + (pDrawing->View.WindowRight-pDrawing->View.WindowLeft)/pDrawing->View.ZoomLevel/pDrawing->View.PPU;

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}

/********************************************************************
 * Zoom Functions                                                   *
 ********************************************************************/

/*-------------------------------------------------------------------*
 |  drwGetDrawingBorder                                              |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |      PREALRECT pRect = pointer to drawing boundary rect           |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwGetDrawingBorder( HDRAWING hDrawing, PREALRECT pRect )
{
	PDRAWING		pDrawing;
	ENTITYHEADER	EntityHeader;
	char			EntityData[4096];
	REALRECT		Rect;
	double			x1 = DBL_MAX;
	double			y1 = DBL_MIN;
	double			x2 = DBL_MIN;
	double			y2 = DBL_MAX;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return NULL;

//	if(pDrawing->View.Viewable)
	{
		// Entities Section ----------------------------------
		if(drwFindEntity_Direct(pDrawing, NULL, &EntityHeader, &EntityData, FIND_FIRST)>0)
		{
			do{
				if(GetEntityRect(pDrawing, &EntityHeader, EntityData, &Rect))
				{
					y1 = __max(Rect.top, y1);
					x1 = __min(Rect.left, x1);
					y2 = __min(Rect.bottom, y2);
					x2 = __max(Rect.right, x2);
				}
			} while(drwFindEntity_Direct(pDrawing, NULL, &EntityHeader, &EntityData, FIND_NEXT)>0);
		}
	}
	
	pRect->left = x1;
	pRect->right = x2;
	pRect->bottom = y2;
	pRect->top = y1;

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}

/*-------------------------------------------------------------------*
 |  drwZoomExtents                                                   |
 |  Inputs:                                                          |
 |      HDRAWING hDrawing = handle to the current DRAWING structure  |
 |  Output: TRUE if everything is ok                                 |
 *-------------------------------------------------------------------*/
BOOL drwZoomExtents( HDRAWING hDrawing )
{
	PDRAWING		pDrawing;
	REALRECT		rect;
	double			ZoomLevelX=0.0, ZoomLevelY=0.0;

	// Initialize pDrawing ------------------
	if((pDrawing = InitilizePDRAWING(hDrawing))==NULL)
		return NULL;

	drwGetDrawingBorder(hDrawing, &rect);

	// Change Drawing View Properties
	ZoomLevelX = (pDrawing->View.WindowBottom-pDrawing->View.WindowTop-20)/((rect.top-rect.bottom)*pDrawing->View.PPU);
	ZoomLevelY = (pDrawing->View.WindowRight-pDrawing->View.WindowLeft-20)/((rect.right-rect.left)*pDrawing->View.PPU);
	pDrawing->View.ZoomLevel = (ZoomLevelX < ZoomLevelY ? ZoomLevelX : ZoomLevelY);
	pDrawing->View.ViewBottom = rect.bottom - ((pDrawing->View.WindowBottom-pDrawing->View.WindowTop)/(pDrawing->View.ZoomLevel*pDrawing->View.PPU) - (rect.top-rect.bottom))/2;
	pDrawing->View.ViewLeft = rect.left - ((pDrawing->View.WindowRight-pDrawing->View.WindowLeft)/(pDrawing->View.ZoomLevel*pDrawing->View.PPU) - (rect.right-rect.left))/2;
	pDrawing->View.ViewTop = pDrawing->View.ViewBottom + (pDrawing->View.WindowBottom-pDrawing->View.WindowTop)/pDrawing->View.ZoomLevel/pDrawing->View.PPU;
	pDrawing->View.ViewRight = pDrawing->View.ViewLeft + (pDrawing->View.WindowRight-pDrawing->View.WindowLeft)/pDrawing->View.ZoomLevel/pDrawing->View.PPU;

	// UnInitilize pDrawing -----------------
	return UnInitilizePDRAWING(hDrawing);
}
