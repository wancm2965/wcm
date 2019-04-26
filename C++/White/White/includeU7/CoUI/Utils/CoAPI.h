#ifndef __CoUI_API_H__
#define __CoUI_API_H__
#include "Utils.h"
#include <CoUI/Core/UIBase.h>

namespace CoUI {
	/////////////////////////////////////////////////////////////////////////////////////


#define COUI_MSGBOX_AUTODESTROY			(0x0001)//失去焦点消亡
#define COUI_MSGBOX_TIMEOUTDESTROY		(0x0002)//倒计时消亡
#define COUI_MSGBOX_CUSTOMEPOSITION		(0x0004)//自定义位置
#define COUI_MSGBOX_CONTENT_LINELIMIT	(0x0008)//内容最大行数
#define COUI_MSGBOX_CONTENT_CENTER		(0x0010)//内容居中对齐


#define COUI_MIN_TIMEOUT			(10)//最小计时时常

	typedef struct MsgboxAppendData
	{
		UINT nActionStyle;
		UINT nSecTimeout;//限时秒数,当有timeout标识时有效
		int nX;	//窗体左上角点x位置，当有自定义标识时有效，下同
		int nY;
		UINT nLineLimit;//内容的最大行数，当有最大行数限制时有效
		struct MsgboxAppendData()
		{
			nActionStyle = 0;
			nSecTimeout = COUI_MIN_TIMEOUT;
			nX = 0;
			nY = 0;
			nLineLimit = 0;
		}

	}MsgboxAppendData, *PMsgboxAppendData;

	enum CoUI_Lang
	{
		CoUI_Lang_ZH_CN,//中文
		CoUI_Lang_EN_US,//英文
	};

	//设置messagebox界面xml文件的完整路径,xml文件名必须是Messagebox.xml
	//返回值：负数说明设置失败（譬如路径不存在）；正数表示资源序号.
	int UILIB_API CoUI_AddMsgBoxSkin(LPCTSTR strResourcePath, CoUI_Lang lan = CoUI_Lang_ZH_CN);

	//************************************
	// 说明:    在窗口中弹出消息盒子
	// 参数:	hOwner   	消息框所属于的窗口句柄
	// 参数:	strTitle    消息框的标题
	// 参数:	strContent	消息框中要显示的内容
	// 参数:    nStyle		包括要弹出的窗口类型和图标类型
		                        //窗口类型包括MB_OK、MB_OKCANCEL、MB_ABORTRETRYIGNORE、MB_YESNOCANCEL、MB_YESNOMB_RETRYCANCEL，必须要有
		                        //图标类型包括MB_ICONINFORMATION、MB_ICONERROR、MB_ICONQUESTION，默认为MB_ICONINFORMATION
								//特殊类型说明： MB_REMEMBER 标识使用“不再询问”选项，勾选了该项后相应的返回值为：IDYESREM或者IDOKREM。
	// 参数:	nResID 数值零表示使用内部资源，否则使用指定资源（参考CoUI_AddMsgBoxSkin的返回值说明）
	// 返回值:  int			返回消息类型
	//************************************
	int UILIB_API CoUI_MessageBox(HWND hOwner, 
		LPCTSTR strTitle,
		LPCTSTR strContent,
		UINT nWndStye,
		const MsgboxAppendData &pAppendData,
		int nResID = 0);

	int UILIB_API CoUI_AfxMessageBox(
		LPCTSTR strContent, int nResID = 0);
	
	/* 销毁所有弹出窗口 */
	void UILIB_API CoUI_CloseAllMessageBox();
	
	/**************************************************************************************/
	//功能：CoUI_CFileDialog类为在窗口中弹出打开或保存文件对话框类														
	//		函数说明：CoUI_CFileDialog::Init函数的参数列表
	//			参数：bOpenSave					TRUE --打开文件, FALSE --保存文件						
	//			参数： lpszDefExt					指定默认的文件扩展名							
	//			参数： lpszFilter					指明可供选择的文件类型和相应的扩展名，当为NULL时为打开文件夹对话框
	//			参数： strMemberID				用于保存人员打开文件记录,默认是保存在Local文件夹下
	//			参数： hParent						父窗口句柄																
	//			参数： strFilePath					另存时，需传所需保存文件的原路径							
	/**************************************************************************************/
	class /*UILIB_API*/ CoUI_CFileDialog//暂时关闭此功能，等较稳定后开放
	{
	private:
		CoUI_CFileDialog( );
	public:
		void Init(BOOL bOpenSave, LPCTSTR lpszDefExt, LPCTSTR lpszFilter, LPCTSTR strMemberID, HWND hParent, LPCTSTR strFilePath);
		 UINT DoModal();
		 const CStdString GetPathName();
		 const CStdString GetFileName();
		 static CoUI_CFileDialog* GetInstance();
		 void DestroyOpenSave(void);

	private:
		static CoUI_CFileDialog* m_pInstance;
		CWindowWnd*	m_pDlgOpenSave;
		CStdString		m_strFilePathName;
		BOOL bOpenSave; 
		LPCTSTR lpszDefExt; 
		LPCTSTR lpszFilter; 
		LPCTSTR strMemberID; 
		HWND hParent; 
		LPCTSTR strFilePath;
		class CGarbo
		{
		public:
			~CGarbo();
		};
		static CGarbo Garbo;
	};

	//************************************
	// 说明:    在窗口中弹出不带走焦点的提示条
	// 参数:	strTip   	提示内容
	// 参数:	strTitle    存在时间
	// 该提示会在屏幕中央显示，并会在设定的时间后自动销毁
	// 只能显示宽400的两行字，应该足够使用了
	//************************************
	void UILIB_API CoUI_Tip( LPCTSTR strTip, int nSeconds = 3 );
} // namespace CoUI
#endif // __CoUI_API_H__