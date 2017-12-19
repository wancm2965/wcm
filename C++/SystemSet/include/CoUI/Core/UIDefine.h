#ifdef _MSC_VER
#pragma once
#endif
//////////////BEGIN消息映射宏定义////////////////////////////////////////////////////
///

namespace CoUI
{

	//定义所有消息类型
	//////////////////////////////////////////////////////////////////////////

#define COUI_MSGTYPE_MENU					(0x0011)//(_T("menu"))
#define COUI_MSGTYPE_LINK					(0x0012)//(_T("link"))

#define COUI_MSGTYPE_TIMER					(0x0013)//(_T("timer"))
#define COUI_MSGTYPE_CLICK					(0x0014)//(_T("click"))

#define COUI_MSGTYPE_RETURN					(0x0015)//(_T("return"))
#define COUI_MSGTYPE_SCROLL					(0x0016)//(_T("scroll"))

#define COUI_MSGTYPE_DROPDOWN				(0x0017)//(_T("dropdown"))
#define COUI_MSGTYPE_SETFOCUS				(0x0018)//(_T("setfocus"))

#define COUI_MSGTYPE_KILLFOCUS				(0x0019)//(_T("killfocus"))
#define COUI_MSGTYPE_ITEMCLICK				(0x001A)//(_T("itemclick"))
#define COUI_MSGTYPE_TABSELECT				(0x001B)//(_T("tabselect"))

#define COUI_MSGTYPE_ITEMSELECT				(0x0020)//(_T("itemselect"))
#define COUI_MSGTYPE_ITEMEXPAND				(0x0021)//(_T("itemexpand"))
#define COUI_MSGTYPE_WINDOWINIT             (0x0022)//(_T("windowinit"))
#define COUI_MSGTYPE_BUTTONDOWN				(0x0023)//(_T("buttondown"))
#define COUI_MSGTYPE_MOUSEENTER				(0x0024)//(_T("mouseenter"))
#define COUI_MSGTYPE_MOUSELEAVE				(0x0025)//(_T("mouseleave"))

#define COUI_MSGTYPE_TEXTCHANGED			(0x0026)//(_T("textchanged"))
#define COUI_MSGTYPE_HEADERCLICK			(0x0027)//(_T("headerclick"))
#define COUI_MSGTYPE_ITEMDBCLICK			(0x0028)//(_T("itemdbclick"))
#define COUI_MSGTYPE_SHOWACTIVEX			(0x0029)//(_T("showactivex"))

#define COUI_MSGTYPE_ITEMCOLLAPSE			(0x002A)//(_T("itemcollapse"))
#define COUI_MSGTYPE_ITEMACTIVATE			(0x002B)//(_T("itemactivate"))
#define COUI_MSGTYPE_VALUECHANGED			(0x002C)//(_T("valuechanged"))

#define COUI_MSGTYPE_SELECTCHANGED			(0x0030)//(_T("selectchanged"))

#define	COUI_MSGTYPE_ITEMDOWN				(0x0031)//(_T("itemdown"))
#define	COUI_MSGTYPE_GESTURE				(0x0032)//(_T("gesture"))
#define	COUI_MSGTYPE_MENUCLICK				(0x0033)//(_T("menuclick"))
#define	COUI_MSGTYPE_DATEUPDATE				(0x0034)//(_T("dateupdate"))
#define	COUI_MSGTYPE_BUTTONUP				(0x0035)//(_T("LButtonUp"))

//Calendar Message
#define	COUI_MSGTYPE_CALBTNDOWN					(0x0041)//(_T("calbtndown"))
#define	COUI_MSGTYPE_PAGEUP						(0x0042)//(_T("pageup"))
#define	COUI_MSGTYPE_PAGEDOWN					(0x0043)//(_T("pagedown"))
#define	COUI_MSGTYPE_CALBTNUP					(0x0044)//(_T("calbtnup"))
#define	COUI_MSGTYPE_CALBTNDBLCICK				(0x0045)//(_T("calbtndblcick"))
#define	COUI_MSGTYPE_CALBUTTONUI_BUTTONDOWN		(0x0046)//(_T("calbuttonui_buttondown"))
#define	COUI_MSGTYPE_CALBUTTONUI_MOUSEMOVE		(0x0047)//(_T("calbuttonui_mousemove"))
#define	COUI_MSGTYPE_CALADJUSTUI_INIT			(0x0048)//(_T("caladjustui_init"))
#define	COUI_MSGTYPE_CALADJUSTUI_BTNLEFTDOWN	(0x0049)//(_T("caladjustui_btnleftdown"))
#define	COUI_MSGTYPE_CALADJUSTUI_BTNRIGHTDOWN	(0x004A)//(_T("caladjustui_btnrightdown"))

#define	COUI_MSGTYPE_CALADJUSTUI_BTNMIDDOWN		(0x004B)//(_T("caladjustui_btnmiddown"))
#define	COUI_MSGTYPE_CALDATECHANGEUI_BTNUP_Y	(0x004C)//(_T("caldatechangeui_btnup_y"))
#define	COUI_MSGTYPE_CALDATECHANGEUI_BTNDOWN_Y	(0x004D)//(_T("caldatechangeui_btndown_y"))
#define	COUI_MSGTYPE_CALDATECHANGEUI_BTNUP_M	(0x004E)//(_T("caldatechangeui_btnup_m"))
#define	COUI_MSGTYPE_CALDATECHANGEUI_BTNDOWN_M	(0x004F)//(_T("caldatechangeui_btndown_m"))
#define	COUI_MSGTYPE_CALDATECHANGEUI_BTNUP_D	(0x0050)//(_T("caldatechangeui_btnup_d"))
#define	COUI_MSGTYPE_CALDATECHANGEUI_BTNDOWN_D	(0x0051)//(_T("caldatechangeui_btndown_d"))

#define CoUI_MSGTYPE_CALBTNCLICK				(0x0052)
#define COUI_MSGTYPE_CALSELECTDATECHANGED		(0x0053)

#define COUI_MSGTYPE_TIPITEMCHANGE              (0x0054)
#define COUI_MSGTYPE_ITEMDOWNEND				(0x0055)
#define COUI_MSGTYPE_ITEMUP						(0x0056)

	
/*
 * NOTE: All Message Numbers below 0x0400 are RESERVED.
 *
 * non-Official CoUI Messages Start Here:
 */
#define COUI_MSG_USER							(0x0400)

//结束定义所有消息类型
//////////////////////////////////////////////////////////////////////////

	//////////////BEGIN控件名称宏定义//////////////////////////////////////////////////
	///

#define  COUI_CTR_EDIT							(_T("Edit"))
#define  COUI_CTR_LIST							(_T("List"))
#define  COUI_CTR_TEXT							(_T("Text"))

#define  COUI_CTR_COMBO							(_T("Combo"))
#define  COUI_CTR_LABEL							(_T("Label"))
#define  COUI_CTR_FLASH							(_T("Flash"))
#define  COUI_CTR_BUTTON						(_T("Button"))
#define  COUI_CTR_OPTION						(_T("Option"))
#define  COUI_CTR_SLIDER						(_T("Slider"))

#define  COUI_CTR_CONTROL						(_T("Control"))
#define  COUI_CTR_ACTIVEX						(_T("ActiveX"))
#define  COUI_CTR_PROGRESS						(_T("Progress"))
#define  COUI_CTR_RICHEDIT						(_T("RichEdit"))
#define  COUI_CTR_OWNERDRAWEDIT					(_T("OwnerDrawEdit"))
#define  COUI_CTR_CHECKBOX						(_T("CheckBox"))
#define  COUI_CTR_COMBOBOX						(_T("ComboBox"))
#define  COUI_CTR_DATETIME						(_T("DateTime"))
#define  COUI_CTR_TREEVIEW						(_T("TreeView"))
#define  COUI_CTR_TREENODE						(_T("TreeNode"))

#define  COUI_CTR_COMBOEDIT						(_T("ComboEdit"))
#define  COUI_CTR_ANIMLABEL						(_T("AnimLabel"))
#define  COUI_CTR_CONTAINER						(_T("Container"))
#define  COUI_CTR_TABLAYOUT						(_T("TabLayout"))
#define  COUI_CTR_SCROLLBAR						(_T("ScrollBar"))
#define  COUI_CTR_WEBBROWSER					(_T("WebBrowser"))

#define  COUI_CTR_CHILDLAYOUT					(_T("ChildLayout"))
#define  COUI_CTR_DIALOGLAYOUT					(_T("DialogLayout"))
#define  COUI_CTR_TILELAYOUT					(_T("TileLayout"))
#define  COUI_CTR_VERTICALLAYOUT				(_T("VerticalLayout"))
#define  COUI_CTR_HORIZONTALLAYOUT				(_T("HorizontalLayout"))

#define  COUI_CTR_LISTHEADER					(_T("ListHeader"))
#define  COUI_CTR_LISTITEM						(_T("ListItem"))
#define  COUI_CTR_LISTELEMENT					(_T("ListElement"))
#define  COUI_CTR_LISTHEADERITEM				(_T("ListHeaderItem"))
#define  COUI_CTR_LISTTEXTELEMENT				(_T("ListTextElement"))
#define  COUI_CTR_LISTLABELELEMENT				(_T("ListLabelElement"))
#define  COUI_CTR_LISTCONTAINERELEMENT			(_T("ListContainerElement"))
//Calendar Control
#define  COUI_CTR_COMBOTREE                     (_T("ComboTree"))
#define  COUI_CTR_CALADJUST						(_T("CalAdjust"))
#define  COUI_CTR_CALBUTTON						(_T("CalButton"))
#define  COUI_CTR_CALDATEBTN					(_T("CalDateBtn"))
#define  COUI_CTR_CALDATECHANGE					(_T("CalDateChange"))
#define  COUI_CTR_CALENDAR						(_T("Calendar"))
#define  COUI_CTR_CHILDWINDOW					(_T("ChildWindow"))
#define  COUI_CTR_ILISTOWNER					(_T("IListOwner"))

#define  COUI_CTR_ICONTAINER					(_T("IContainer"))
#define  COUI_CTR_HWND							(_T("HWnd"))
#define  COUI_CTR_ILIST							(_T("IList"))
#define  COUI_CTR_MENUELEMENT					(_T("MenuElement"))
#define  COUI_CTR_MENU							(_T("Menu"))
#define  COUI_CTR_ITREEITEM						(_T("ITreeItem"))
#define  COUI_CTR_TREEITEM						(_T("TreeItem"))
#define  COUI_CTR_ITREE							(_T("ITree"))
#define  COUI_CTR_TREE							(_T("Tree"))


	///
	//////////////END控件名称宏定义//////////////////////////////////////////////////


}// namespace CoUI