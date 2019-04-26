#include "StdAfx.h"
#include "MyDropTarget.h"
//#include "atlctl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

FORMATETC g_FormatEtc;
bool g_bFormatEtcInit = false;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void InitFormatEtc(void)
{
	if (g_bFormatEtcInit)
		return;

	g_bFormatEtcInit = true;

	//自定义剪贴板
	UINT format = RegisterClipboardFormat(_T("CF_HPMC"));
	//g_FormatEtc = {(CLIPFORMAT)format, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	g_FormatEtc.cfFormat	= (CLIPFORMAT)format;
	g_FormatEtc.ptd			= NULL;
	g_FormatEtc.dwAspect	= DVASPECT_CONTENT;
	g_FormatEtc.lindex		= -1;
	g_FormatEtc.tymed		= TYMED_HGLOBAL;
}

HRESULT CreateEnumFormatEtc (UINT cfmt, FORMATETC *afmt, IEnumFORMATETC **ppEnumFormatEtc)
{
	if (cfmt == 0 || afmt == 0 || ppEnumFormatEtc == 0)
		return E_INVALIDARG;
	*ppEnumFormatEtc = new CEnumFormatEtc (afmt, cfmt);
	return (*ppEnumFormatEtc) ? S_OK: E_OUTOFMEMORY;
}

void DeepCopyFormatEtc(FORMATETC *dest, FORMATETC *source)
{
	// copy the source FORMATETC into dest
	*dest = *source;

	if(source->ptd)
	{
		// allocate memory for the DVTARGETDEVICE if necessary
		dest->ptd = (DVTARGETDEVICE*)CoTaskMemAlloc(sizeof(DVTARGETDEVICE));

		// copy the contents of the source DVTARGETDEVICE into dest->ptd
		*(dest->ptd) = *(source->ptd);
	}
}

CEnumFormatEtc::CEnumFormatEtc(FORMATETC *pFormatEtc, int nNumFormats)
{
	m_lRefCount   = 1;
	m_nIndex      = 0;
	m_nNumFormats = nNumFormats;
	m_pFormatEtc  = new FORMATETC[nNumFormats];
	// make a new copy of each FORMATETC structure
	for(int i = 0; i < nNumFormats; ++i)
	{
		DeepCopyFormatEtc (&m_pFormatEtc[i], &pFormatEtc[i]);
	}
}

CEnumFormatEtc::~CEnumFormatEtc()
{
	// first free any DVTARGETDEVICE structures
	for(ULONG i = 0; i < m_nNumFormats; ++i)
	{
		if(m_pFormatEtc[i].ptd)
			CoTaskMemFree(m_pFormatEtc[i].ptd);
	}
	// now free the main array
	delete[] m_pFormatEtc;
}

HRESULT STDMETHODCALLTYPE CEnumFormatEtc::QueryInterface( REFIID iid, void** ppvObject )
{
	if (iid == IID_IUnknown || iid == IID_IEnumFORMATETC)
	{
		TRACE(_T("QueryInterface: Interface supported."));
		*ppvObject = this;
	}
	else
	{
		TRACE(_T("QueryInterface: Interface not supported."));
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
	//加计数
	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE CEnumFormatEtc::AddRef( void )
{
	return ++m_lRefCount;
}

ULONG STDMETHODCALLTYPE CEnumFormatEtc::Release( void )
{
	if (m_lRefCount)
		m_lRefCount--;

	if (0 == m_lRefCount)
		delete this;

	return m_lRefCount;
}

HRESULT CEnumFormatEtc::Reset (void)
{
	m_nIndex = 0;
	return S_OK;
}

HRESULT CEnumFormatEtc::Skip(ULONG celt)
{
	m_nIndex += celt;
	return (m_nIndex <= m_nNumFormats) ? S_OK : S_FALSE;
}

HRESULT CEnumFormatEtc::Clone(IEnumFORMATETC **ppEnumFormatEtc)
{
	HRESULT hResult;
	// make a duplicate enumerator
	hResult = CreateEnumFormatEtc(m_nNumFormats, m_pFormatEtc, ppEnumFormatEtc);
	if(hResult == S_OK)
	{
		// manually set the index state
		((CEnumFormatEtc *)*ppEnumFormatEtc)->m_nIndex = m_nIndex;
	}
	return hResult;
}

HRESULT CEnumFormatEtc::Next(ULONG celt, FORMATETC *pFormatEtc, ULONG *pceltFetched)
{
	ULONG copied = 0;
	// copy the FORMATETC structures into the caller's buffer
	while (m_nIndex < m_nNumFormats && copied < celt) 
	{
		DeepCopyFormatEtc (&pFormatEtc [copied], &m_pFormatEtc [m_nIndex]);
		++copied;
		++m_nIndex;
	}
	// store result
	if(pceltFetched != 0) 
		*pceltFetched = copied;
	// did we copy all that was requested?
	return (copied == celt) ? S_OK : S_FALSE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
HRESULT CreateDataObject (FORMATETC *fmtetc, STGMEDIUM *stgmeds, UINT count, IDataObject **ppDataObject)
{
	if(ppDataObject == 0)
		return E_INVALIDARG;

	*ppDataObject = new CDataObjectEx(fmtetc, stgmeds, count);

	return (*ppDataObject) ? S_OK: E_OUTOFMEMORY;
}

CDataObjectEx::CDataObjectEx(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count)
{
	// reference count must ALWAYS start at 1
	m_lRefCount    = 1;
	m_nNumFormats  = count;

	m_pFormatEtc   = new FORMATETC[count];
	m_pStgMedium   = new STGMEDIUM[count];

	for(int i = 0; i < count; ++i)
	{
		m_pFormatEtc[i] = fmtetc[i];
		m_pStgMedium[i] = stgmed[i];
	}
}

CDataObjectEx::~CDataObjectEx(void)
{
	if (m_pFormatEtc)
	{
		delete m_pFormatEtc;
		m_pFormatEtc = NULL;
	}
	if (m_pStgMedium)
	{
		delete m_pStgMedium;
		m_pStgMedium = NULL;
	}
}

HRESULT STDMETHODCALLTYPE CDataObjectEx::QueryInterface( REFIID iid, void** ppvObject )
{
	if (iid == IID_IUnknown || iid == IID_IDataObject)
	{
		TRACE(_T("QueryInterface: Interface supported."));
		*ppvObject = this;
	}
	else
	{
		TRACE(_T("QueryInterface: Interface not supported."));
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
	//加计数
	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE CDataObjectEx::AddRef( void )
{
	return ++m_lRefCount;
}

ULONG STDMETHODCALLTYPE CDataObjectEx::Release( void )
{
	if (m_lRefCount)
		m_lRefCount--;

	if (0 == m_lRefCount)
		delete this;

	return m_lRefCount;
}

HGLOBAL DupGlobalMemMem(HGLOBAL hMem)
{
	DWORD   len    = GlobalSize (hMem);
	PVOID   source = GlobalLock (hMem);
	PVOID   dest   = GlobalAlloc (GMEM_FIXED, len);
	memcpy (dest, source, len);
	GlobalUnlock (hMem);
	return dest;
}

int CDataObjectEx::LookupFormatEtc(FORMATETC *pFormatEtc)
{
	// 轮流检查格式看是否能找到匹配的格式
	for(int i = 0; i < m_nNumFormats; ++i)
	{
		if((m_pFormatEtc[i].tymed    &  pFormatEtc->tymed)   &&
			m_pFormatEtc[i].cfFormat == pFormatEtc->cfFormat &&
			m_pFormatEtc[i].dwAspect == pFormatEtc->dwAspect)
		{
			// return index of stored format
			return i;
		}
	}

	// error, format not found
	return -1;
}

HRESULT STDMETHODCALLTYPE CDataObjectEx::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
	int idx;
	// try to match the specified FORMATETC with one of our supported formats
	if((idx = LookupFormatEtc(pformatetcIn)) == -1)
		return DV_E_FORMATETC;
	// found a match - transfer data into supplied storage medium
	pmedium->tymed           = m_pFormatEtc[idx].tymed;
	pmedium->pUnkForRelease  = 0;
	// copy the data into the caller's storage medium
	switch(m_pFormatEtc[idx].tymed)
	{
	case TYMED_HGLOBAL:
		pmedium->hGlobal     = DupGlobalMemMem(m_pStgMedium[idx].hGlobal);
		break;
	default:
		return DV_E_FORMATETC;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDataObjectEx::QueryGetData(__RPC__in_opt FORMATETC *pformatetc)
{
	return (LookupFormatEtc(pformatetc) == -1) ? DV_E_FORMATETC : S_OK;
}

HRESULT STDMETHODCALLTYPE CDataObjectEx::GetCanonicalFormatEtc(__RPC__in_opt FORMATETC *pformatectIn, __RPC__out FORMATETC *pformatetcOut)
{
	// Apparently we have to set this field to NULL even though we don't do anything else
	pformatetcOut->ptd = NULL;
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDataObjectEx::SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDataObjectEx::EnumFormatEtc(DWORD dwDirection, __RPC__deref_out_opt IEnumFORMATETC **ppenumFormatEtc)
{
	// OLE仅仅支持得到方向成员
	if(dwDirection == DATADIR_GET)
	{
		// 在WIN2K下，你可以调用AIP函数SHCreateStdEnumFmtEtc来完成，但为了支持//所有的window平台，我们需要实现IEnumFormatEtc。
		return CreateEnumFormatEtc(m_nNumFormats, m_pFormatEtc, ppenumFormatEtc);
	}
	else
	{
		// the direction specified is not supported for drag+drop
		return E_NOTIMPL;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
HRESULT CreateDropSource(IDropSource **ppDropSource)
{
	if (ppDropSource == 0)
		return E_INVALIDARG;

	*ppDropSource = new CDropSource;

	return (*ppDropSource) ? S_OK: E_OUTOFMEMORY;
}

CDropSource::CDropSource()
{
	m_lRefCount = 1;
}

CDropSource::~CDropSource()
{
}

HRESULT STDMETHODCALLTYPE CDropSource::QueryInterface( REFIID iid, void** ppvObject )
{
	if (iid == IID_IUnknown || iid == IID_IDropSource)
	{
		TRACE(_T("QueryInterface: Interface supported."));
		*ppvObject = this;
	}
	else
	{
		TRACE(_T("QueryInterface: Interface not supported."));
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
	//加计数
	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE CDropSource::AddRef( void )
{
	return ++m_lRefCount;
}

ULONG STDMETHODCALLTYPE CDropSource::Release( void )
{
	if (m_lRefCount)
		m_lRefCount--;

	if (0 == m_lRefCount)
		delete this;

	return m_lRefCount;
}

HRESULT __stdcall CDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	// if the Escape key has been pressed since the last call, cancel the drop
	if(fEscapePressed == TRUE)
		return DRAGDROP_S_CANCEL;       
	// if the LeftMouse button has been released, then do the drop!
	if((grfKeyState & MK_LBUTTON) == 0)
		return DRAGDROP_S_DROP;
	// continue with the drag-drop
	return S_OK;
}

HRESULT __stdcall CDropSource::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CMyDropTarget::CMyDropTarget(void)
	: m_ulRefCount(1)
	, m_hTargetWnd(NULL)
	, m_bAllowDrop(false)
{
	InitFormatEtc();
}

CMyDropTarget::~CMyDropTarget(void)
{
}

BOOL CMyDropTarget::DragDropRegister( HWND hWnd, DWORD AcceptKeyState/*=MK_LBUTTON*/ )
{
	if (!IsWindow(hWnd))
		return FALSE;

	HRESULT s = ::RegisterDragDrop(hWnd,this);
	if (SUCCEEDED(s))
	{
		m_hTargetWnd = hWnd;
		m_AcceptKeyState = AcceptKeyState;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool CMyDropTarget::QueryDataObject(IDataObject *pDataObject)
{
	// does the data object support CF_TEXT using a HGLOBAL?
	return pDataObject->QueryGetData(&g_FormatEtc) == S_OK ? true : false;
}

DWORD CMyDropTarget::DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	DWORD dwEffect = 0;
	// 1. 检查pt来看是否允许drop操作在某个位置
	// 2. 计算出基于grfKeyState的drop效果
	if(grfKeyState & MK_CONTROL)
	{
		dwEffect = dwAllowed & DROPEFFECT_COPY;
	}
	else if(grfKeyState & MK_SHIFT)
	{
		dwEffect = dwAllowed & DROPEFFECT_MOVE;
	}
	// 3. 非键盘修饰符指定（或drop效果不允许），因此基于drop源的效果
	if(dwEffect == 0)
	{
		if(dwAllowed & DROPEFFECT_COPY) dwEffect = DROPEFFECT_COPY;
		if(dwAllowed & DROPEFFECT_MOVE) dwEffect = DROPEFFECT_MOVE;
	}
	return dwEffect;
}

//////////////////////////////////////////////////////////////////////////
// 继承IDropTarget接口
HRESULT STDMETHODCALLTYPE CMyDropTarget::QueryInterface( REFIID iid, void** ppvObject )
{
	if (iid == IID_IUnknown || iid == IID_IDropTarget)
	{
		TRACE(_T("QueryInterface: Interface supported."));
		*ppvObject = this;
	}
	else
	{
		TRACE(_T("QueryInterface: Interface not supported."));
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
	//加计数
	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE CMyDropTarget::AddRef( void )
{
	return ++m_ulRefCount;
}

ULONG STDMETHODCALLTYPE CMyDropTarget::Release( void )
{
	if (m_ulRefCount)
	{
		m_ulRefCount--;
	}
	return m_ulRefCount;
}

HRESULT STDMETHODCALLTYPE CMyDropTarget::DragOver( DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect )
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CMyDropTarget::DragEnter( __RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect )
{
	//if (grfKeyState != m_AcceptKeyState)
	//	*pdwEffect = DROPEFFECT_NONE;
	//else
	//	*pdwEffect = DROPEFFECT_MOVE;

	//return S_OK;

	// does the dataobject contain data we want?
	m_bAllowDrop = QueryDataObject(/*grfKeyState, pdwEffect,*/ pDataObj);
	if(m_bAllowDrop)
	{
		// get the dropeffect based on keyboard state
		*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
		//SetFocus (m_hWnd);
		//PositionCursor(m_hWnd, pt);
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CMyDropTarget::DragLeave( void )
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CMyDropTarget::Drop( __RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect )
{
	//PositionCursor(m_hWnd, pt);
	if (!m_bAllowDrop)
	{
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}

	STGMEDIUM stgmed;
	HRESULT ret = pDataObj->GetData(&g_FormatEtc, &stgmed);

	HPMC_DROP* pData	= (HPMC_DROP*)GlobalLock(stgmed.hGlobal);
	int nLen			= (int)GlobalSize(stgmed.hGlobal);
	GlobalUnlock(stgmed.hGlobal);
	if (sizeof(HPMC_DROP) != nLen)
	{
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}

	if (::IsWindow(m_hTargetWnd))
	{
		PostMessage(m_hTargetWnd, WM_ON_DROP_NOTIFY, (WPARAM)pData, nLen);
	}

	*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////

HANDLE StringToHandle(TCHAR *szText, int nTextLen)
{
	void  *ptr;
	// if text length is -1 then treat as a nul-terminated string
	if(nTextLen == -1)
		nTextLen = lstrlen (szText) + 1;
	// allocate and lock a global memory buffer. Make it fixed
	// data so we don't have to use GlobalLock
	ptr = (void *)GlobalAlloc(GMEM_FIXED, nTextLen);
	// copy the string into the buffer
	memcpy (ptr, szText, nTextLen);
	return ptr;
}

void StartDragDrop(HWND hWnd, int nScreenID, int nWindowID, CFSRC menuCFSRC /*= CFSRC_TM*/, std::string strMemberID /*= ""*/, int nChannelID /*= -1*/)
{
	InitFormatEtc();

	HPMC_DROP drop_data;
	int size = 0;
	if (menuCFSRC == CFSRC_AV)
	{
// 		HPMC_DROP drop_data;
		drop_data.Type		= CFSRC_AV;
		drop_data.hWnd		= hWnd;
		drop_data.AreaID	= nScreenID;
		strncpy_s(drop_data.MemberID, strMemberID.c_str(),sizeof(drop_data.MemberID)); 
		drop_data.ChannelID	= nChannelID;
		/*int */size = sizeof(drop_data);
	}
	else
	{
		// 		HPMC_DROP drop_data;
		drop_data.Type		= CFSRC_TM;
		drop_data.hWnd		= hWnd;
		drop_data.AreaID	= nScreenID;
		drop_data.ChannelID	= nWindowID;
		/*int */size = sizeof(drop_data);
	}

	HGLOBAL hClipBuffer;
	hClipBuffer		= GlobalAlloc(GMEM_DDESHARE, size);
	char* buffer	= (char*)GlobalLock(hClipBuffer);
	if (NULL == buffer)
		return;

	memcpy(buffer, &drop_data, size);
	GlobalUnlock(hClipBuffer);

	STGMEDIUM stgmed = {TYMED_HGLOBAL, {0}, 0};
	stgmed.hGlobal = hClipBuffer;

	LPDATAOBJECT lpDataObject = NULL;
	if (CreateDataObject(&g_FormatEtc, &stgmed, 1, &lpDataObject) != S_OK)
		return;

	LPDROPSOURCE lpDropSource = NULL;
	if (CreateDropSource(&lpDropSource) != S_OK)
		return;

	DWORD dwEffect = DROPEFFECT_MOVE;
	DWORD dwResult = ::DoDragDrop(lpDataObject, lpDropSource, DROPEFFECT_MOVE, &dwEffect);
	if(dwResult == DRAGDROP_S_DROP)
	{
		if(dwEffect == DROPEFFECT_MOVE)
		{
			// remove the data we just dropped from active document
		}
	}

	lpDropSource->Release();
	lpDataObject->Release();
	ReleaseStgMedium(&stgmed);
}
