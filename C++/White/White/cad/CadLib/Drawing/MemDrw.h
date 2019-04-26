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
typedef struct tag_TABLES //表段
{
	LAYER		Layer[MAX_LAYERS];              //图层数组   
	LTYPE		LType[MAX_LTYPES];              //线型数组
	STYLE		Style[MAX_STYLES];              //样式数组
	DIMSTYLE	DimStyle[MAX_DIMSTYLES];        //标注样式数组

	UINT		NumLayers;                      //图层数量
	UINT		NumLTypes;                     //线形数量 
	UINT		NumStyles;                    //样式数量
	UINT		NumDimStyles;                //标注样式数量
	
	UINT		CurrentLayer;               //当前图层号
	UINT		CurrentLType;               //当前线形号
	UINT		CurrentStyle;              //当前样式号
	UINT		CurrentDimStyle;           //当前标注样式号

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
	HEADER   Header;//头段 
	TABLES		Tables;//表段
	ENTITIES	Entities;//图元段
	BLOCKHEADER	Blocks[MAX_BLOCKS];//块段
	DWORD		BlocksNumber;//块数量
	DWORD		CurrentBlock;//当前块
	OBJHANDLE	LastObjhandle;//上一个对象的句柄
	DWORD		LastDimBlkNum;//上一个标注块
	VIEW		View;  //视图
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

BOOL drwDestroy(                   //销毁Drawing
	HDRAWING	hDrawing			// 只想要销毁的Drawing的句柄
);

// TABLES ******
OBJHANDLE drwAddTableType(            //向Drawing添加表类型
	HDRAWING	hDrawing,			// 指向当前DRAWING结构
	DWORD		dwTableType,		// 要添加的表类型
	LPVOID		pTableType			//指向添加的表类型结构的指针
);

BOOL drwDeleteTableType(
	HDRAWING	hDrawing,			// 指向当前DRAWING结构
	DWORD		dwTableType,		// 被移除的表类型
	OBJHANDLE	TableTypeObjhandle	// 指向被移除的表类型的句柄
);
         /*查找表类型*/
OBJHANDLE drwFindTableType_Direct( 
	PDRAWING	pDrawing,			// 指向当前DRAWING结构
	DWORD		dwTableType,		// 从表类型中得到的数据 ( TAB_NOTSET = search in all table types )
	DWORD		dwFindType,			// 查找类型
	LPVOID		pTableType			// 指向表类型结构
);

OBJHANDLE drwFindTableType(
	HDRAWING	hDrawing,			// 指向当前DRAWING结构
	DWORD		dwTableType,		// 从表类型中得到的数据 ( TAB_NOTSET = search in all table types )
	DWORD		dwFindType,			// 查找类型
	LPVOID		pTableType			// 指向表类型结构
);

// BLOCKS ******
OBJHANDLE drwAddBlock_Direct(
	PDRAWING		pDrawing,		// 指向当前DRAWING结构
	PBLOCKHEADER	pBlockHeader	// 指向 blockheader 结构
);

OBJHANDLE drwAddBlock(
	HDRAWING		hDrawing,		// 指向当前DRAWING结构
    PBLOCKHEADER	pBlockHeader	// 指向blockheader 结构
);

BOOL drwDeleteBlock(
	HDRAWING		hDrawing,		// 指向当前DRAWING结构
	OBJHANDLE		BlockObjhandle	// 要删除的block的句柄
);

OBJHANDLE drwFindBlock_Direct(
	PDRAWING		pDrawing,		// 指向当前DRAWING结构
	DWORD			dwFindType,		// 查找类型
	PBLOCKHEADER	pBlockHeader	// 指向 block header 数据
);

OBJHANDLE drwFindBlock(
	HDRAWING		hDrawing,		// 指向当前DRAWING结构
	DWORD			dwFindType,		// 查找类型
	PBLOCKHEADER	pBlockHeader	// 指向 block header数据
);

int drwGetBlocksNum(
	HDRAWING		hDrawing		// 指向当前DRAWING结构
);

// ENTITIES ******
OBJHANDLE drwAddEntity_Direct(
	PDRAWING		pDrawing,		// 指向当前DRAWING结构
	OBJHANDLE		BlockObjhandle,	// 要添加图元对象的块句柄. (NULL = Entity Section)
	PENTITYHEADER	pEntityHeader,	// 指向图元数据头部
	LPVOID			pEntityData		// 指向要添加的图元数据 
);

OBJHANDLE drwAddEntity(
	HDRAWING		hDrawing,		// 指向当前DRAWING结构
	OBJHANDLE		BlockObjhandle,	// 要添加图元对象的块句柄. (NULL = Entity Section)
	PENTITYHEADER	pEntityHeader,	// 指向图元数据头部
	LPVOID			pEntityData		// 指向要添加的图元数据 
);

BOOL drwDeleteEntity(
	HDRAWING		hDrawing,		// 指向当前DRAWING结构
	LPCTSTR			strBlockName,	// 要添加图元对象的块名. (NULL=Entity Section)
	OBJHANDLE		EntityHandle	// 要删除的图元的句柄
);

OBJHANDLE drwFindEntity_Direct(
	PDRAWING		pDrawing,		// 指向当前DRAWING结构
	LPCTSTR			strBlockName,	// 从中查找图元的块名. (NULL=Entity Section)
	PENTITYHEADER	pEntityHeader,	// 指向图元数据头部
	LPVOID			pEntityData,	// 指向图元数据
	DWORD			dwFindType,		// 查找类型
	DWORD*			pdwEntityPos=NULL// 查找到的图元的位置
);

OBJHANDLE drwFindEntity(
	HDRAWING		hDrawing,		// 指向当前DRAWING结构
	LPCTSTR			strBlockName,	// 从中查找图元的块名. (NULL=Entity Section)
	PENTITYHEADER	pEntityHeader,	// 指向图元数据头部
	LPVOID			pEntityData,	// 指向图元数据
	DWORD			dwFindType		// 查找类型
);

BOOL drwChangeEntity_Direct(
	PDRAWING		pDrawing,		// 指向当前DRAWING结构
	LPCTSTR			strBlockName,	// 要改变其中图元的块名. (NULL=Entity Section)
	PENTITYHEADER	pEntityHeader,	// 指向图元数据头部
	LPVOID			pEntityData		// 指向图元数据
);

BOOL drwChangeEntity(
	HDRAWING		hDrawing,		// 指向当前DRAWING结构
	LPCTSTR			strBlockName,	// 要改变其中图元的块名. (NULL=Entity Section)
	PENTITYHEADER	pEntityHeader,	// 指向图元数据头部
	LPVOID			pEntityData		// 指向图元数据
);

BOOL drwReleaseEntity(
	PENTITYHEADER	pEntityHeader,	// 指向图元数据头部
	LPVOID			pEntityData	    // 指向图元数据
);

#endif // MEMDRW_H
