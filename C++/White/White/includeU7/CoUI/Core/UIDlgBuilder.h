#ifndef __UIDLGBUILDER_H__
#define __UIDLGBUILDER_H__

#include "../Utils/Utils.h"
#include "../Core/UIMarkup.h"

namespace CoUI {

class CControlUI;
class IDialogBuilderCallback
{
public:
    virtual CControlUI* CreateControl(LPCTSTR pstrClass) = 0;
};


class CPaintManagerUI;
class UILIB_API CDialogBuilder
{
public:
    CDialogBuilder();
    CControlUI* Create(STRINGorID xml, LPCTSTR type = NULL, IDialogBuilderCallback* pCallback = NULL,
        CPaintManagerUI* pManager = NULL, CControlUI* pParent = NULL, int resType = 0);
    CControlUI* Create(IDialogBuilderCallback* pCallback = NULL, CPaintManagerUI* pManager = NULL,
        CControlUI* pParent = NULL);

	bool LoadDefaultAttributeList(STRINGorID xml, LPCTSTR type, CPaintManagerUI* pManager);
	bool LoadDefaultAttributeList(CPaintManagerUI* pManager);

    CMarkup* GetMarkup();

    void GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const;
    void GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const;

	// 将字符串中的相对路径转换为绝对路径
private:
    CControlUI* _Parse(CMarkupNode* parent, CControlUI* pParent = NULL, CPaintManagerUI* pManager = NULL);
	CStdString RelativePath2AbsolutePath(LPCTSTR pStrImage);
 
	CMarkup m_xml;
    IDialogBuilderCallback* m_pCallback;
    LPCTSTR m_pstrtype;
};

} // namespace CoUI

#endif // __UIDLGBUILDER_H__
