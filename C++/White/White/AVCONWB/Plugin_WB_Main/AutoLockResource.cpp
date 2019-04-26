//////////////////////////////////////////////////////////////////////////
//FileName:			AutoLockResource.cpp								//
//																		//
//																		//
//																		//
//																		//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoLockResource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HINSTANCE	g_hInstance=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AutoLockResource::AutoLockResource()
{
	AFX_MODULE_STATE*	pModuleState	= AfxGetModuleState();

	if (pModuleState)
	{
		hCurrentResourceHandle		= pModuleState->m_hCurrentResourceHandle;
		pModuleState->m_hCurrentResourceHandle = g_hInstance;
	}
}

AutoLockResource::~AutoLockResource()
{
	AFX_MODULE_STATE*	pModuleState	= AfxGetModuleState();

	if (pModuleState)
	{
		pModuleState->m_hCurrentResourceHandle = hCurrentResourceHandle;
	}
}
