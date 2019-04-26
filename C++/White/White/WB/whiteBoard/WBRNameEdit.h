



#pragma once


// CWBRNameEdit

class CWBRNameEdit : public CEdit
{
	DECLARE_DYNAMIC(CWBRNameEdit)

public:
	CWBRNameEdit();
	virtual ~CWBRNameEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


