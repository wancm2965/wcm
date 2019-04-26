#pragma once
#include "UCCMessageBox.h"

class CGlobalMessageBoxManager
{
public:
	///添加一个MessageBox
	void AddMessageBox(CUCCMessageBox *pMess);
	///删除指定MessageBox
	void DelMessageBox(CUCCMessageBox *pMess);
	///删除所有的Messagebox
	void DeleteAllMessageBox();

private:
	///messagebox链表(安全性的使用，最好加上锁定操作)
	std::list<CUCCMessageBox*> m_listMessageBoxes;
};
