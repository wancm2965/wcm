#ifndef _PICDLL_H_
#define _PICDLL_H_
class _declspec(dllexport) IPicCtrl
{
public:
	virtual BOOL LoadImage(CString strFilePath)=0;				//����Ҫ���Ƶ�ͼƬ·��
};

_declspec(dllexport) IPicCtrl* CreatePicCtrl(UINT nIDTemplate, CRect rc, CWnd* pParentWnd);
_declspec(dllexport) void DestroyPicCtrl(IPicCtrl *pIPicCtrl);


#endif