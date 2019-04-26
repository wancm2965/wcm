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
|  MemDrw.h                                                           |
\*-------------------------------------------------------------------*/

#ifndef MEMDRW_H
#define MEMDRW_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Interface\VC\DXFDef.h"
#include "..\Interface\VC\DrawingDef.h"

// Definitions
#define		MAX_DIMSTYLES	64
#define		MAX_LAYERS		128
#define		MAX_LTYPES		128
#define		MAX_STYLES		128
#define	    MAX_BLOCKS		1024   

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

// Tables ***************************
typedef struct tag_TABLES //���
{
	LAYER		Layer[MAX_LAYERS];              //ͼ������   
	LTYPE		LType[MAX_LTYPES];              //��������
	STYLE		Style[MAX_STYLES];              //��ʽ����
	DIMSTYLE	DimStyle[MAX_DIMSTYLES];        //��ע��ʽ����

	UINT		NumLayers;                      //ͼ������
	UINT		NumLTypes;                     //�������� 
	UINT		NumStyles;                    //��ʽ����
	UINT		NumDimStyles;                //��ע��ʽ����
	
	UINT		CurrentLayer;               //��ǰͼ���
	UINT		CurrentLType;               //��ǰ���κ�
	UINT		CurrentStyle;              //��ǰ��ʽ��
	UINT		CurrentDimStyle;           //��ǰ��ע��ʽ��

//	OBJHANDLE	CurrentLayerHandle;		// Current Layer for drawing
//	OBJHANDLE	CurrentTextStyleHandle;	// Current TextStyle for drawing
//	OBJHANDLE	CurrentDimStyleHandle;	// Current DimStyle for drawing
//	OBJHANDLE	CurrentLineTypeHandle;	// Current LineType for drawing
//	short		CurrentColor;			// Current Color for drawing
//	short		CurrentLineWeight;		// Current LineWeight for drawing
} TABLES, *PTABLES;

// Drawing Structure ****************
typedef struct tag_DRAWING
{
	HEADER   Header;//ͷ�� 
	TABLES		Tables;//���
	ENTITIES	Entities;//ͼԪ��
	BLOCKHEADER	Blocks[MAX_BLOCKS];//���
	DWORD		BlocksNumber;//������
	DWORD		CurrentBlock;//��ǰ��
	OBJHANDLE	LastObjhandle;//��һ������ľ��
	DWORD		LastDimBlkNum;//��һ����ע��
	VIEW		View;  //��ͼ
} DRAWING, *PDRAWING;

//---------------------------------------------------------------------------
// Function declarations
//---------------------------------------------------------------------------
PDRAWING InitilizePDRAWING(HDRAWING hDrawing);
BOOL UnInitilizePDRAWING(HDRAWING hDrawing);

//---------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------
// Construction & Destruction ******
HDRAWING drwCreate();

BOOL drwDestroy(                   //����Drawing
	HDRAWING	hDrawing			// ֻ��Ҫ���ٵ�Drawing�ľ��
);

// TABLES ******
OBJHANDLE drwAddTableType(            //��Drawing��ӱ�����
	HDRAWING	hDrawing,			// ָ��ǰDRAWING�ṹ
	DWORD		dwTableType,		// Ҫ��ӵı�����
	LPVOID		pTableType			//ָ����ӵı����ͽṹ��ָ��
);

BOOL drwDeleteTableType(
	HDRAWING	hDrawing,			// ָ��ǰDRAWING�ṹ
	DWORD		dwTableType,		// ���Ƴ��ı�����
	OBJHANDLE	TableTypeObjhandle	// ָ���Ƴ��ı����͵ľ��
);
         /*���ұ�����*/
OBJHANDLE drwFindTableType_Direct( 
	PDRAWING	pDrawing,			// ָ��ǰDRAWING�ṹ
	DWORD		dwTableType,		// �ӱ������еõ������� ( TAB_NOTSET = search in all table types )
	DWORD		dwFindType,			// ��������
	LPVOID		pTableType			// ָ������ͽṹ
);

OBJHANDLE drwFindTableType(
	HDRAWING	hDrawing,			// ָ��ǰDRAWING�ṹ
	DWORD		dwTableType,		// �ӱ������еõ������� ( TAB_NOTSET = search in all table types )
	DWORD		dwFindType,			// ��������
	LPVOID		pTableType			// ָ������ͽṹ
);

// BLOCKS ******
OBJHANDLE drwAddBlock_Direct(
	PDRAWING		pDrawing,		// ָ��ǰDRAWING�ṹ
	PBLOCKHEADER	pBlockHeader	// ָ�� blockheader �ṹ
);

OBJHANDLE drwAddBlock(
	HDRAWING		hDrawing,		// ָ��ǰDRAWING�ṹ
    PBLOCKHEADER	pBlockHeader	// ָ��blockheader �ṹ
);

BOOL drwDeleteBlock(
	HDRAWING		hDrawing,		// ָ��ǰDRAWING�ṹ
	OBJHANDLE		BlockObjhandle	// Ҫɾ����block�ľ��
);

OBJHANDLE drwFindBlock_Direct(
	PDRAWING		pDrawing,		// ָ��ǰDRAWING�ṹ
	DWORD			dwFindType,		// ��������
	PBLOCKHEADER	pBlockHeader	// ָ�� block header ����
);

OBJHANDLE drwFindBlock(
	HDRAWING		hDrawing,		// ָ��ǰDRAWING�ṹ
	DWORD			dwFindType,		// ��������
	PBLOCKHEADER	pBlockHeader	// ָ�� block header����
);

int drwGetBlocksNum(
	HDRAWING		hDrawing		// ָ��ǰDRAWING�ṹ
);

// ENTITIES ******
OBJHANDLE drwAddEntity_Direct(
	PDRAWING		pDrawing,		// ָ��ǰDRAWING�ṹ
	OBJHANDLE		BlockObjhandle,	// Ҫ���ͼԪ����Ŀ���. (NULL = Entity Section)
	PENTITYHEADER	pEntityHeader,	// ָ��ͼԪ����ͷ��
	LPVOID			pEntityData		// ָ��Ҫ��ӵ�ͼԪ���� 
);

OBJHANDLE drwAddEntity(
	HDRAWING		hDrawing,		// ָ��ǰDRAWING�ṹ
	OBJHANDLE		BlockObjhandle,	// Ҫ���ͼԪ����Ŀ���. (NULL = Entity Section)
	PENTITYHEADER	pEntityHeader,	// ָ��ͼԪ����ͷ��
	LPVOID			pEntityData		// ָ��Ҫ��ӵ�ͼԪ���� 
);

BOOL drwDeleteEntity(
	HDRAWING		hDrawing,		// ָ��ǰDRAWING�ṹ
	LPCTSTR			strBlockName,	// Ҫ���ͼԪ����Ŀ���. (NULL=Entity Section)
	OBJHANDLE		EntityHandle	// Ҫɾ����ͼԪ�ľ��
);

OBJHANDLE drwFindEntity_Direct(
	PDRAWING		pDrawing,		// ָ��ǰDRAWING�ṹ
	LPCTSTR			strBlockName,	// ���в���ͼԪ�Ŀ���. (NULL=Entity Section)
	PENTITYHEADER	pEntityHeader,	// ָ��ͼԪ����ͷ��
	LPVOID			pEntityData,	// ָ��ͼԪ����
	DWORD			dwFindType,		// ��������
	DWORD*			pdwEntityPos=NULL// ���ҵ���ͼԪ��λ��
);

OBJHANDLE drwFindEntity(
	HDRAWING		hDrawing,		// ָ��ǰDRAWING�ṹ
	LPCTSTR			strBlockName,	// ���в���ͼԪ�Ŀ���. (NULL=Entity Section)
	PENTITYHEADER	pEntityHeader,	// ָ��ͼԪ����ͷ��
	LPVOID			pEntityData,	// ָ��ͼԪ����
	DWORD			dwFindType		// ��������
);

BOOL drwChangeEntity_Direct(
	PDRAWING		pDrawing,		// ָ��ǰDRAWING�ṹ
	LPCTSTR			strBlockName,	// Ҫ�ı�����ͼԪ�Ŀ���. (NULL=Entity Section)
	PENTITYHEADER	pEntityHeader,	// ָ��ͼԪ����ͷ��
	LPVOID			pEntityData		// ָ��ͼԪ����
);

BOOL drwChangeEntity(
	HDRAWING		hDrawing,		// ָ��ǰDRAWING�ṹ
	LPCTSTR			strBlockName,	// Ҫ�ı�����ͼԪ�Ŀ���. (NULL=Entity Section)
	PENTITYHEADER	pEntityHeader,	// ָ��ͼԪ����ͷ��
	LPVOID			pEntityData		// ָ��ͼԪ����
);

BOOL drwReleaseEntity(
	PENTITYHEADER	pEntityHeader,	// ָ��ͼԪ����ͷ��
	LPVOID			pEntityData	    // ָ��ͼԪ����
);

#endif // MEMDRW_H
