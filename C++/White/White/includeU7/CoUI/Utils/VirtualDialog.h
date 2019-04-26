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
		//bModalָ����ʾ��ʽ,��ʹ��ģ̬(��bModalΪtrue),��pRt��������
		bool ShowDialog(CControlUI* ParentRoot, bool bModal = true, RECT* pRt = NULL);
		bool CloseDialog();
		//���ڶ�������ʱ���Զ�������󶨵Ŀؼ���
		CControlUI* GetControl(bool bclear = true);//�Ƿ�Ӵ����Ƴ��ÿؼ�������ϣ����������ʱ�ؼ�һ�����٣������Ƴ�
	protected:
		CDialogContainer* m_pDialogContainer;
	};
}//end of CoUI
#endif // __VIRTUALDIALOG_H__