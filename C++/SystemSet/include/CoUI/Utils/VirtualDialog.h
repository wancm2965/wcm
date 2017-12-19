#ifndef __VIRTUALDIALOG_H__
#define __VIRTUALDIALOG_H__
#include "../Utils/LogicChildWin.h"

namespace CoUI
{
	class CDialogContainer;
	class UILIB_API CVirtualDialog:public CLogicWinImplBaseBase
	{
	public:
		CVirtualDialog();
		virtual ~CVirtualDialog(void);
	public:	
		void OnFinalMessage();
		virtual void Release();
		//bModal指定显示方式,若使用模态(即bModal为true),则pRt参数忽略
		bool ShowDialog(CControlUI* ParentRoot, bool bModal = true, RECT* pRt = NULL);
		bool CloseDialog();
		//窗口对象销毁时会自动销毁其绑定的控件，
		CControlUI* GetControl(bool bclear = true);//是否从窗口移除该控件，若不希望窗口销毁时控件一起被销毁，则需移除
	protected:
		CDialogContainer* m_pDialogContainer;
	};
}//end of CoUI
#endif // __VIRTUALDIALOG_H__