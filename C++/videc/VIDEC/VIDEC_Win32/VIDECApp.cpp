// VIDECApp.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "VIDECApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HINSTANCE g_hInstance;

/////////////////////////////////////////////////////////////////////////////
// CVIDECApp

BEGIN_MESSAGE_MAP(CVIDECApp, CWinApp)
	//{{AFX_MSG_MAP(CVIDECApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVIDECApp construction

CVIDECApp::CVIDECApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVIDECApp object

CVIDECApp theApp;


BOOL CVIDECApp::InitInstance() 
{
	g_hInstance=m_hInstance;

	return CWinApp::InitInstance();
}

int CVIDECApp::ExitInstance() 
{
	return CWinApp::ExitInstance();
}

//#include "../VIDEC_FRE/VIDECFREHeader.h"
//
//void * FreMalloc(size_t size,char * fileName,char * func,int nLine)
//{
//	void * p=NULL;
//	p = malloc(size);
//	if(p == NULL)
//		DebugOutput("FreMalloc NULL,File:%s,FUN:%s,Line:%d \n", fileName,   func,  nLine);
//
//	return p;
//}

