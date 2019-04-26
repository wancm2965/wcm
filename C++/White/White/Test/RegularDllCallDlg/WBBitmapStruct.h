#pragma once
int  m_ToolbarButtonCount;
typedef struct bitmapStructInfo{
	UINT        id;
	UINT        bitmapId;
	UINT        buttonId;
	CStringW    tipChar;
} BITMAPSTRUCTINFO;
  BITMAPSTRUCTINFO m_BitmapInfo[50];