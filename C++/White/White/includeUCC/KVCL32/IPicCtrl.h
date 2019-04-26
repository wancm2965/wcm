#ifndef _PICDLL_H_
#define _PICDLL_H_
class _declspec(dllexport) IPicCtrl
{
public:
	virtual BOOL LoadImage(CString strFilePath)=0;				//设置要绘制的图片路径
};

_declspec(dllexport) IPicCtrl* CreatePicCtrl(UINT nIDTemplate, CRect rc, CWnd* pParentWnd);
_declspec(dllexport) void DestroyPicCtrl(IPicCtrl *pIPicCtrl);


#endif