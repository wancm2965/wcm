// VIDEC.h : main header file for the VIDEC DLL
//

#if !defined(AFX_VIDEC_H__70868036_44CF_407A_A440_0829618DA554__INCLUDED_)
#define AFX_VIDEC_H__70868036_44CF_407A_A440_0829618DA554__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVIDECApp
// See VIDEC.cpp for the implementation of this class
//

class CVIDECApp : public CWinApp
{
public:
	CVIDECApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVIDECApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CVIDECApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEC_H__70868036_44CF_407A_A440_0829618DA554__INCLUDED_)
