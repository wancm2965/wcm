#include <Windows.h>
#include "shlobj.h"

#define SHCNRF_InterruptLevel 0x0001                     //Interrupt level notifications from the file system
#define SHCNRF_ShellLevel   0x0002                       //Shell-level notifications from the shell
#define SHCNRF_RecursiveInterrupt 0x1000                 //Interrupt events on the whole subtree
#define SHCNRF_NewDelivery   0x8000                      //Messages received use shared memory

//typedef struct
//{
//	LPCITEMIDLIST pidl;                                  //Pointer to an item identifier list (PIDL) for which to receive notifications
//	BOOL fRecursive;                                     //Flag indicating whether to post notifications for children of this PIDL
//}SHChangeNotifyEntry;

typedef struct
{
	DWORD dwItem1;                                       // dwItem1 contains the previous PIDL or name of the folder.
	DWORD dwItem2;                                       // dwItem2 contains the new PIDL or name of the folder.
}SHNotifyInfo;

typedef ULONG
	(WINAPI* pfnSHChangeNotifyRegister)
	(
	HWND hWnd,
	int fSource,
	LONG fEvents,
	UINT wMsg,
	int cEntries,
	SHChangeNotifyEntry* pfsne 
	);

typedef BOOL (WINAPI* pfnSHChangeNotifyDeregister)(ULONG ulID);