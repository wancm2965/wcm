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
|  TextView.cpp                                                       |
\*-------------------------------------------------------------------*/
#include "stdafx.h"
#include "TextView.h"
#include "Colors.h"
#include <regex>
using namespace std::tr1;

// Units to pixel conversions
#define	U2PXNOSCALE(units)	pDrawing->View.WindowLeft + (int)((-pDrawing->View.ViewLeft + x+ units) * pDrawing->View.PPU * pDrawing->View.ZoomLevel)
#define	U2PYNOSCALE(units)	pDrawing->View.WindowBottom - (int)((-pDrawing->View.ViewBottom + y+ units) * pDrawing->View.PPU * pDrawing->View.ZoomLevel)

/*-------------------------------------------------------------------*
 |  isTextInViewableArea                                             |
 |  Inputs:                                                          |
 |      PVIEW pView = pointer to the current Drawing View structure  |
 |      RECT *pTextRect = pointer to text boundary rectangle         |
 |  Output:                                                          |
 |      TRUE if text can be shown in viewable area                   |
 *-------------------------------------------------------------------*/
BOOL isTextInViewableArea(PVIEW pView, RECT *pTextRect)
{
	if((pTextRect->top>=pView->WindowTop) && (pTextRect->top<=pView->WindowBottom) &&
	   (pTextRect->left>=pView->WindowLeft) && (pTextRect->left<=pView->WindowRight))
	   return TRUE;
	if((pTextRect->bottom>=pView->WindowTop) && (pTextRect->bottom<=pView->WindowBottom) &&
	   (pTextRect->right>=pView->WindowLeft) && (pTextRect->right<=pView->WindowRight))
	   return TRUE;
	return FALSE;
}

/*-------------------------------------------------------------------*
替换wstring种的字符串
 *-------------------------------------------------------------------*/
wstring& replace_all_distinct(wstring& str, const wstring& old_value, const wstring& new_value)   
{   
	for(wstring::size_type  pos(0);   pos!=wstring::npos;   pos+=new_value.length())  
	{   
		if((pos=str.find(old_value,pos))!=wstring::npos)
		{
			str.replace(pos,old_value.length(),new_value);
		}
		else  
		{
			break;
		}
	}   
	return  str;   
}   

 vector<wstring> token_by_p(wstring& str)   
 {   
	 vector<wstring> strs;
	 wstring::size_type pos1(0),pos2(0);
	 wstring::size_type length=wstring(L"\\P").length();
	 while(( pos2 = str.find(wstring(L"\\P"),pos1)) != wstring::npos)
	 {
		 strs.push_back(str.substr(pos1,pos2-pos1));
		 pos1=pos2+length;
	 } 
	 strs.push_back(str.substr(pos1,str.length()-pos2));
	 return  strs; 
 }  

 long num(int num, int index)
 {
	 long s = 1;
	 int i=0;
	 while(i<index)
	 {
		 s *= num;
		 i++;
	 }
	 return s;
 }

 int WcharToHex(wchar_t ch)
 {
	 int n = -1;
	 switch(ch)
	 {
	 case '0':	n = 0;	break;
	 case '1':	n = 1;	break;
	 case '2':	n = 2;	break;
	 case '3':	n = 3;	break;
	 case '4':	n = 4;	break;
	 case '5':	n = 5;	break;
	 case '6':	n = 6;	break;
	 case '7':	n = 7;	break;
	 case '8':	n = 8;	break;
	 case '9':	n = 9;	break;
	 case 'A':
	 case 'a':	n = 10;	break;
	 case 'B':
	 case 'b':	n = 11;	break;
	 case 'C':
	 case 'c':	n = 12;	break;
	 case 'D':
	 case 'd':	n = 13;	break;
	 case 'E':
	 case 'e':	n = 14;	break;
	 case 'F':
	 case 'f':	n = 15;	break;
	 default:	break;
	 }
	 return n;
 }

wchar_t  HexToWchar(wchar_t *str, int strlen)
{
	long hex = 0;
	int i=0, n=0;
	wchar_t *p = str;
	p += strlen - 1;
	if(str == NULL) return -1;
	if(strlen <= 0 || strlen > 10)	return -1;

	while(i<strlen) 
	{
		n = WcharToHex(*p--);
		hex += n*num(16, i++);
	} 

	wchar_t wch = -1;
	if(hex <0)  return wch;
	wch = (wchar_t)hex;
	return wch;
}

void replace_cad_regex(wstring& str)
{
	vector<wstring> vecstr;
	wregex wreg(L"\\\\U\\+(\\\w{4})");
	wsmatch m;
	std::wstring::const_iterator begin = str.begin();
	std::wstring::const_iterator end = str.end();

	while(regex_search(begin, end, m, wreg))
	{
		vecstr.push_back(m[0]);
		begin = m[0].second;
	}

	std::wstring tmp = L"";
	std::wstring ws = L"";

	for(vector<wstring>::const_iterator it = vecstr.begin(); it != vecstr.end(); ++it)
	{
		tmp = (wstring)(*it);
		replace_all_distinct(tmp, wstring(L"\\U+"), wstring(L""));
		ws = HexToWchar((LPWSTR)tmp.c_str(), tmp.length());
		replace_all_distinct(str, (*it), ws);
	}
	vector<wstring>().swap(vecstr);
	vecstr.clear();
}

void DecodeDxfStr(wstring& src)
{
	//先替换\\和\},\{  以免干扰正则表达式的匹配
	replace_all_distinct(src,wstring(L"\\\\"),wstring(L"\x007F"));
	replace_all_distinct(src,wstring(L"\\{"),wstring(L"\x001b"));
	replace_all_distinct(src,wstring(L"\\}"),wstring(L"\x0018"));

	replace_all_distinct(src,wstring(L"{"),wstring(L"\x0000"));
	replace_all_distinct(src,wstring(L"}"),wstring(L"\x0000"));

	wregex wreg(L"(\\\\(?!U|P|[^\\\w]))(.*?)\\;(.*?)"); //第一步判断文字中是否有字体样式,然后去除字体样式
	wstring wctrl(L"$3");
	if(regex_search(src, wreg))
	{
		src=regex_replace(src,wreg,wctrl);//匹配wreg
	}

	replace_all_distinct(src,wstring(L"\x007F"),wstring(L"\\\\"));
	replace_all_distinct(src,wstring(L"\x001b"),wstring(L"\\{"));
	replace_all_distinct(src,wstring(L"\x0018"),wstring(L"\\}"));

	//cad特殊字符替换，有待完善
	replace_all_distinct(src,wstring(L"%%p"),wstring(L"\x00B1"));
	replace_all_distinct(src,wstring(L"%%P"),wstring(L"\x00B1"));
	replace_all_distinct(src,wstring(L"%%c"),wstring(L"\x00D8"));
	replace_all_distinct(src,wstring(L"%%C"),wstring(L"\x00D8"));
	replace_all_distinct(src,wstring(L"%%d"),wstring(L"\x00B0"));
	replace_all_distinct(src,wstring(L"%%D"),wstring(L"\x00B0"));
	replace_all_distinct(src,wstring(L"\\A1;"),wstring(L""));

	//cad特殊字符在2000/2004中使用
	replace_cad_regex(src);

	replace_all_distinct(src,wstring(L"\\\\"),wstring(L"\\"));
	replace_all_distinct(src,wstring(L"\\{"),wstring(L"{"));
	replace_all_distinct(src,wstring(L"\\}"),wstring(L"}"));
	replace_all_distinct(src,wstring(L"\\~"),wstring(L"\x00A0"));
}

/*-------------------------------------------------------------------*
 |  PlotText                                                         |
 |  Descriptions:                                                    |
 |      Plots a text into given Drawing Context                      |
 |  Inputs:                                                          |
 |      PDRAWING pDrawing = pointer to the current DRAWING structure |
 |      HDC hdc= handle of DC for drawing                            |
 |      PENTITYHEADER pEntityHeader = pointer to entity data header  |
 |      PENTTEXT pText = pointer to text data                        |
 |      double x = x position of insertion point (default = 0)       |
 |      double y = y position of insertion point (default = 0)       |
 |      double xScale = x scale factor (default = 1)                 |
 |      double yScale = y scale factor (default = 1)                 |
 |      double Rotation = rotation angle (default = 0)               |
 |  Output: Nothing                                                  |
 *-------------------------------------------------------------------*/
void PlotText(PDRAWING pDrawing, HDC hdc, PENTITYHEADER pEntityHeader, PENTTEXT pText, double x=0, double y=0, double xScale=1, double yScale=1, double Rotation=0)
{
	//解析文字字符串
	std::wstring wstrText(pText->strText);
	DecodeDxfStr(wstrText);

	//判断字符串是否为NULL
	if (wstrText == L"") 
	{
		return;
	}

	LAYER		Layer;
	STYLE		Style;
	COLORREF	rgbcolor;

	// Initialize LayerName & LineTypeName & Color
	short  color = pEntityHeader->Color;

	Layer.Objhandle = pEntityHeader->LayerObjhandle;
	if (drwFindTableType_Direct(pDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer)>0)
	{
		if((pEntityHeader->Color==COLOR_BYBLOCK) || (pEntityHeader->Color==COLOR_BYLAYER))
			color = Layer.Color;
		else if (Layer.Color<0)
		{
			return;
		}
	}
	else if (color>=COLOR_BYLAYER)
	{
		color = 0;
	}

	rgbcolor = CadColor[color];

	// Plot the Text in DC
	LOGFONT logfont;
	ZeroMemory(&logfont, sizeof(LOGFONT));
	strcpy_s(logfont.lfFaceName, "SimSun");
	logfont.lfWeight = FW_DONTCARE;
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfQuality = NONANTIALIASED_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	logfont.lfHeight = -MulDiv(int(pText->TextData.Height * pDrawing->View.PPU * pDrawing->View.ZoomLevel), GetDeviceCaps(hdc, LOGPIXELSY), 72);
	logfont.lfItalic = (pText->TextData.Oblique != 0.0f ? true : false);

	// Find Text Style
	Style.Objhandle = pText->TextData.TextStyleObjhandle;
	if(drwFindTableType_Direct(pDrawing, TAB_STYLE, FIND_BYHANDLE, &Style)>0)
	{
		if (pText->TextData.Height<=0)
		{
			logfont.lfHeight = -MulDiv(int(Style.LastHeightUsed * pDrawing->View.PPU * pDrawing->View.ZoomLevel), GetDeviceCaps(hdc, LOGPIXELSY), 72);
		}
		strcpy_s(logfont.lfFaceName, Style.FaceName); //字体名称
	}

	if(abs(logfont.lfHeight) > 1) 
	{
		PointF pLeftTop; //文字的左上点
		PointF pCenter;  //文字的操作点
		//SIZE size;

		//旋转、颠倒、反向
		bool  backward = ((pText->TextData.GenerationFlag & 2) != 0);
		bool  upsidedown = ((pText->TextData.GenerationFlag & 4) != 0);

		REALPOINT prePoint;
		ZeroMemory(&prePoint, sizeof(REALPOINT));
		PointRotationPoint(pText->TextData.Point1, 
			pText->TextData.RotationAngle,
			pText->Point0, prePoint);  //旋转前坐标

		if (backward) //文字反向
		{
			prePoint.x = 2 * pText->TextData.Point1.x - prePoint.x;
		}
		if (upsidedown) //文字颠倒
		{
			prePoint.y = 2 * pText->TextData.Point1.y - prePoint.y;
		}
		if (pText->TextData.WidthFactor != 1.0f) //文字宽度伸缩
		{
			prePoint.x = (prePoint.x - pText->TextData.Point1.x) / pText->TextData.WidthFactor + pText->TextData.Point1.x;
		}

		pCenter.X = (REAL)U2PXNOSCALE(pText->TextData.Point1.x); //文字的对称点X
		pCenter.Y = (REAL)U2PYNOSCALE(pText->TextData.Point1.y); //文字的对称点Y

		// Calculate text rect and format
		//GetTextExtentPoint32W(hdc, wstrText.c_str(), wstrText.length(), &size);
		switch(pText->TextData.Justification)
		{
		//基线对正
		case TJ_CENTER:
		case TJ_RIGHT:
		case TJ_ALIGNED:
		case TJ_FIT:
			{
				pLeftTop.X = (REAL)U2PXNOSCALE(prePoint.x);
				pLeftTop.Y = (REAL)U2PYNOSCALE(pText->TextData.Point1.y + pText->TextData.Height);
			}
			break;
		//顶端对正
		case TJ_TOPLEFT:
		case TJ_TOPCENTER:
		case TJ_TOPRIGHT:
			{
				pLeftTop.X = (REAL)U2PXNOSCALE(prePoint.x)/* - 0.5f * size.cx*/;
				pLeftTop.Y = (REAL)U2PYNOSCALE(pText->TextData.Point1.y)/* - 0.5f * size.cy*/;
			}
			break;
		//居中对正
		case TJ_MIDDLE:
		case TJ_MIDDLELEFT:
		case TJ_MIDDLECENTER:
		case TJ_MIDDLERIGHT:
			{
				pLeftTop.X = (REAL)U2PXNOSCALE(prePoint.x);
				pLeftTop.Y = (REAL)U2PYNOSCALE(pText->TextData.Point1.y + pText->TextData.Height / 2);
			}
			break;
		//底端对正
		case TJ_BOTTOMLEFT:
		case TJ_BOTTOMCENTER:
		case TJ_BOTTOMRIGHT:
			{
				pLeftTop.X = (REAL)U2PXNOSCALE(prePoint.x);
				pLeftTop.Y = (REAL)U2PYNOSCALE(prePoint.y+ pText->TextData.Height);
			}
			break;
		//缺省情况
		case TJ_LEFT:
		default:
			{
				pCenter.X = (REAL)U2PXNOSCALE(pText->Point0.x);
				pCenter.Y = (REAL)U2PYNOSCALE(pText->Point0.y);
				pLeftTop.X = (REAL)U2PXNOSCALE(pText->Point0.x);
				pLeftTop.Y = (REAL)U2PYNOSCALE(pText->Point0.y + pText->TextData.Height);
			}
			break;
		}

		Graphics graphics(hdc);
		SolidBrush  solidBrush(Color(255, GetRValue(rgbcolor), GetGValue(rgbcolor), GetBValue(rgbcolor)));
		Font font(hdc, &logfont);

		int xInsert = pDrawing->View.WindowLeft + (int)((-pDrawing->View.ViewLeft + x) * pDrawing->View.PPU * pDrawing->View.ZoomLevel);
		int yInsert = pDrawing->View.WindowBottom - (int)((-pDrawing->View.ViewBottom + y) * pDrawing->View.PPU * pDrawing->View.ZoomLevel);

		//Insert块参照 中心点缩放旋转
		graphics.TranslateTransform((REAL)xInsert, (REAL)yInsert);
		graphics.RotateTransform((REAL)-Rotation);
		graphics.ScaleTransform((REAL)xScale, (REAL)yScale);
		graphics.TranslateTransform((REAL)-xInsert, (REAL)-yInsert);

		//文字旋转、反向、颠倒、宽度伸缩
		graphics.TranslateTransform(pCenter.X, pCenter.Y);
		graphics.RotateTransform((REAL)-pText->TextData.RotationAngle);
		if (backward)  //文字反向
		{
			graphics.ScaleTransform(-1.0f, 1.0f);
		}
		if (upsidedown)  //文字颠倒
		{
			graphics.ScaleTransform(1.0f, -1.0f);
		}
		if (pText->TextData.WidthFactor != 1.0f) //文字宽度伸缩
		{
			graphics.ScaleTransform((REAL)pText->TextData.WidthFactor, 1.0f);
		}
		graphics.TranslateTransform(-pCenter.X, -pCenter.Y);
		
		//pLeftTop.X -= abs(logfont.lfHeight) * pDrawing->View.PPU * pDrawing->View.ZoomLevel;
		//pLeftTop.Y -= abs(logfont.lfHeight) * pDrawing->View.PPU * pDrawing->View.ZoomLevel;
		
		graphics.DrawString(wstrText.c_str(), -1, &font, pLeftTop, &solidBrush);
		graphics.ResetTransform();
		graphics.ReleaseHDC(hdc);
	}
}

/********************************************************************
 * Functions for plot the Text                                      *
 ********************************************************************/
// GDIPlus获得字体宽度
SizeF GetTextBounds(const Font& font, const StringFormat& strFormat, const WCHAR* szText)
{
	GraphicsPath graphicsPathObj;
	FontFamily fontfamily;
	font.GetFamily(&fontfamily);
	graphicsPathObj.AddString(szText, -1, &fontfamily, font.GetStyle(), font.GetSize(),
		PointF(0,0), &strFormat);

	RectF rcBound;
	// 获取边界范围
	graphicsPathObj.GetBounds(&rcBound);
	// 返回文本的宽高
	return SizeF(rcBound.Width,rcBound.Height);
}

void PlotMText(PDRAWING pDrawing, HDC hdc, PENTITYHEADER pEntityHeader, PENTMTEXT pText, double x=0, double y=0, double xScale=1, double yScale=1, double Rotation=0)
{
	//解析文字字符串
	std::wstring wstrText(pText->strText);
	DecodeDxfStr(wstrText);

	if (wstrText == L"") //判断字符串是否为NULL
	{
		return;
	}

	LAYER		Layer;
	STYLE		Style;
	COLORREF	rgbcolor;

	// Initialize LayerName & LineTypeName & Color
	short  color = pEntityHeader->Color;

	Layer.Objhandle = pEntityHeader->LayerObjhandle;
	if (drwFindTableType_Direct(pDrawing, TAB_LAYER, FIND_BYHANDLE, &Layer)>0)
	{
		if ((pEntityHeader->Color == COLOR_BYBLOCK) || (pEntityHeader->Color == COLOR_BYLAYER))
			color = Layer.Color;
		else if (Layer.Color<0)
		{
			return;
		}
	}
	else if (color >= COLOR_BYLAYER)
	{
		color = 0;
	}

	rgbcolor = CadColor[color];

	// Plot the Text in DC
	LOGFONT logfont;
	ZeroMemory(&logfont, sizeof(LOGFONT));
	strcpy_s(logfont.lfFaceName, "SimSun");
	logfont.lfWeight = FW_DONTCARE;
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfCharSet = GB2312_CHARSET;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	logfont.lfHeight = -MulDiv(int(pText->TextData.Height * pDrawing->View.PPU * pDrawing->View.ZoomLevel), GetDeviceCaps(hdc, LOGPIXELSY), 72);
	
	// Find Text Style
	Style.Objhandle = pText->TextStyleObjhandle;
	if(drwFindTableType_Direct(pDrawing, TAB_STYLE, FIND_BYHANDLE, &Style)>0)
	{
		if(pText->TextData.Height<=0)
		{
			logfont.lfHeight = -MulDiv(int(Style.LastHeightUsed * pDrawing->View.PPU*pDrawing->View.ZoomLevel), GetDeviceCaps(hdc, LOGPIXELSY), 72);
		}
		strcpy_s(logfont.lfFaceName, Style.FaceName); //字体名称
	}
	
	if(abs(logfont.lfHeight) > 1) 
	{	
		Graphics graphics(hdc);	
		Font font(hdc, &logfont);
		SolidBrush  solidBrush(Color(255, GetRValue(rgbcolor), GetGValue(rgbcolor), GetBValue(rgbcolor)));

		PointF pLeftTop; //文字的左上点
		PointF pCenter;  //文字的插入点

		pCenter.X = (REAL)U2PXNOSCALE(pText->Point0.x); //文字的对称点X
		pCenter.Y = (REAL)U2PYNOSCALE(pText->Point0.y); //文字的对称点Y
		pLeftTop = pCenter; //初始化左上点

		//Insert块参照插入点
		int xInsert = pDrawing->View.WindowLeft + (int)((-pDrawing->View.ViewLeft + x) * pDrawing->View.PPU * pDrawing->View.ZoomLevel);
		int yInsert = pDrawing->View.WindowBottom - (int)((-pDrawing->View.ViewBottom + y) * pDrawing->View.PPU * pDrawing->View.ZoomLevel);

		//Insert块参照 中心点缩放
		graphics.TranslateTransform((REAL)xInsert, (REAL)yInsert);
		graphics.RotateTransform((REAL)-Rotation);
		graphics.ScaleTransform((REAL)xScale, (REAL)yScale);
		graphics.TranslateTransform((REAL)-xInsert, (REAL)-yInsert);

		//文字旋转
		double angle = atan2(pText->TextData.AngleVecY, pText->TextData.AngleVecX) * 180 / PI;
		graphics.TranslateTransform(pCenter.X, pCenter.Y);
		graphics.RotateTransform((REAL)-angle);
		graphics.TranslateTransform(-pCenter.X, -pCenter.Y);

		vector<wstring> vecStr = token_by_p(wstrText); //文字列表

		REAL LineSpaceWidth = (REAL)(pText->TextData.Height * pDrawing->View.PPU * pDrawing->View.ZoomLevel * pText->TextData.LineSpaceScale / 0.6f); //文字间距
		REAL TotalSpaceWidth = (vecStr.size() - 1) * LineSpaceWidth + abs(logfont.lfHeight); //文字的总间距

		int i = 0;
		for (auto it = vecStr.begin(); it != vecStr.end(); ++it)
		{
			wstring strLineText = (*it);
			StringFormat stringformat(StringAlignmentCenter);
			SizeF size = GetTextBounds(font, stringformat, strLineText.c_str());
			
			//文字样式
			switch(pText->TextData.Justification)
			{
			case MTJ_TOPLEFT:
				pLeftTop.Y = pCenter.Y + i * LineSpaceWidth;
				break;
			case MTJ_TOPCENTER:
				pLeftTop.X = pCenter.X - size.Width / 2;
				pLeftTop.Y = pCenter.Y + i * LineSpaceWidth;
				break;
			case MTJ_TOPRIGHT:
				pLeftTop.X = pCenter.X - size.Width;
				pLeftTop.Y = pCenter.Y +  i * LineSpaceWidth;
				break;
			case MTJ_MIDDLELEFT:
				pLeftTop.Y = (pCenter.Y - TotalSpaceWidth / 2) + i * LineSpaceWidth;
				break;
			case MTJ_MIDDLECENTER:
				pLeftTop.X = pCenter.X - size.Width / 2;
				pLeftTop.Y = (pCenter.Y - TotalSpaceWidth / 2) + i * LineSpaceWidth;
				break;
			case MTJ_MIDDLERIGHT:
				pLeftTop.X = pCenter.X - size.Width;
				pLeftTop.Y = (pCenter.Y - TotalSpaceWidth / 2) + i * LineSpaceWidth;
				break;
			case MTJ_BOTTOMLEFT:
				pLeftTop.Y = (pCenter.Y - TotalSpaceWidth) + i * LineSpaceWidth;
				break;
			case MTJ_BOTTOMCENTER:
				pLeftTop.X = pCenter.X - size.Width / 2;
				pLeftTop.Y = (pCenter.Y - TotalSpaceWidth) + i * LineSpaceWidth;
				break;
			case MTJ_BOTTOMRIGHT:
				pLeftTop.X = pCenter.X - size.Width;
				pLeftTop.Y = (pCenter.Y - TotalSpaceWidth) + i * LineSpaceWidth;
				break;
			}

			graphics.DrawString(strLineText.c_str(), -1, &font, pLeftTop/*, &stringFormat*/, &solidBrush);
			i++;
		}

		graphics.ResetTransform();
		graphics.ReleaseHDC(hdc);

		vector<wstring>().swap(vecStr);
		vecStr.clear();
	}
}