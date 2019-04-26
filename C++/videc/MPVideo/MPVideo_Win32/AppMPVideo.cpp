// MPVideoApp.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "AppMPVideo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HINSTANCE g_hInstance;

/////////////////////////////////////////////////////////////////////////////
// AppMPVideo

BEGIN_MESSAGE_MAP(AppMPVideo, CWinApp)
	//{{AFX_MSG_MAP(AppMPVideo)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AppMPVideo construction

AppMPVideo::AppMPVideo()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only AppMPVideo object

AppMPVideo theApp;


BOOL AppMPVideo::InitInstance() 
{
	g_hInstance=m_hInstance;

	return CWinApp::InitInstance();
}

int AppMPVideo::ExitInstance() 
{
	return CWinApp::ExitInstance();
}
