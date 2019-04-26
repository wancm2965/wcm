#pragma once
#include "UCCMessageBox.h"

class CGlobalMessageBoxManager
{
public:
	///���һ��MessageBox
	void AddMessageBox(CUCCMessageBox *pMess);
	///ɾ��ָ��MessageBox
	void DelMessageBox(CUCCMessageBox *pMess);
	///ɾ�����е�Messagebox
	void DeleteAllMessageBox();

private:
	///messagebox����(��ȫ�Ե�ʹ�ã���ü�����������)
	std::list<CUCCMessageBox*> m_listMessageBoxes;
};
