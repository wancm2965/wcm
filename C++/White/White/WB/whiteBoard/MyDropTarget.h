#pragma once
#include "oleidl.h"

#define WM_ON_DROP_NOTIFY		WM_USER + 1

class CEnumFormatEtc : public IEnumFORMATETC
{
public:
	//
	// IUnknown members
	//
	HRESULT __stdcall  QueryInterface (REFIID iid, void ** ppvObject);
	ULONG   __stdcall  AddRef (void);
	ULONG   __stdcall  Release (void);
	//
	// IEnumFormatEtc members
	//
	HRESULT __stdcall  Next  (ULONG celt, FORMATETC * rgelt, ULONG * pceltFetched);
	HRESULT __stdcall  Skip  (ULONG celt); 
	HRESULT __stdcall  Reset (void);
	HRESULT __stdcall  Clone (IEnumFORMATETC ** ppEnumFormatEtc);

	//
	// Construction / Destruction
	//
	CEnumFormatEtc(FORMATETC *pFormatEtc, int nNumFormats);
	~CEnumFormatEtc();

private:
	LONG		m_lRefCount;        // Reference count for this COM interface
	ULONG		m_nIndex;           // current enumerator index
	ULONG		m_nNumFormats;      // number of FORMATETC members
	FORMATETC*	m_pFormatEtc;       // array of FORMATETC objects
};

class CDataObjectEx : public IDataObject
{
public:
	CDataObjectEx(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count);
	~CDataObjectEx(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// 继承IDataObject接口
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, void** ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(void);
	ULONG STDMETHODCALLTYPE Release(void);

	virtual HRESULT STDMETHODCALLTYPE GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
	virtual HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium){ return DATA_E_FORMATETC; };
	virtual HRESULT STDMETHODCALLTYPE QueryGetData(__RPC__in_opt FORMATETC *pformatetc);
	virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(__RPC__in_opt FORMATETC *pformatectIn, __RPC__out FORMATETC *pformatetcOut);
	virtual HRESULT STDMETHODCALLTYPE SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease);
	virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDirection, __RPC__deref_out_opt IEnumFORMATETC **ppenumFormatEtc);
	virtual HRESULT STDMETHODCALLTYPE DAdvise(__RPC__in FORMATETC *pformatetc, DWORD advf, __RPC__in_opt IAdviseSink *pAdvSink, __RPC__out DWORD *pdwConnection){ return OLE_E_ADVISENOTSUPPORTED; };
	virtual HRESULT STDMETHODCALLTYPE DUnadvise(DWORD dwConnection){ return OLE_E_ADVISENOTSUPPORTED; };
	virtual HRESULT STDMETHODCALLTYPE EnumDAdvise(__RPC__deref_out_opt IEnumSTATDATA **ppenumAdvise){ return OLE_E_ADVISENOTSUPPORTED; };
	//////////////////////////////////////////////////////////////////////////

private:
	int LookupFormatEtc(FORMATETC *pFormatEtc);

	LONG		m_lRefCount;        // Reference count for this COM interface
	ULONG		m_nNumFormats;      // number of FORMATETC members
	FORMATETC*	m_pFormatEtc;       // array of FORMATETC objects
	STGMEDIUM*	m_pStgMedium;

	//FORMATETC	m_pformatetc;
	//STGMEDIUM	m_pmedium;
};

class CDropSource : public IDropSource
{
public:
	//
	// IUnknown members
	//
	HRESULT __stdcall QueryInterface    (REFIID iid, void ** ppvObject);
	ULONG   __stdcall AddRef            (void);
	ULONG   __stdcall Release           (void);
	//
	// IDropSource members
	//
	HRESULT __stdcall QueryContinueDrag (BOOL fEscapePressed, DWORD grfKeyState);
	HRESULT __stdcall GiveFeedback      (DWORD dwEffect);
	//
	// Constructor / Destructor
	//
	CDropSource();
	~CDropSource();
private:
	//
	// private members and functions
	//
	LONG    m_lRefCount;
};

class CMyDropTarget : public IDropTarget
{
public:
	CMyDropTarget(void);
	~CMyDropTarget(void);

public:
	BOOL DragDropRegister(HWND hWnd,DWORD AcceptKeyState=MK_LBUTTON);

public:
	//////////////////////////////////////////////////////////////////////////
	// 继承IDropTarget接口
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, void** ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(void);
	ULONG STDMETHODCALLTYPE Release(void);
	HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect);
	HRESULT STDMETHODCALLTYPE DragEnter(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave(void);
	HRESULT STDMETHODCALLTYPE Drop(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect);
	//////////////////////////////////////////////////////////////////////////

private:
	bool QueryDataObject(IDataObject *pDataObject);
	DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);

	HWND	m_hTargetWnd;
	DWORD	m_AcceptKeyState;
	ULONG	m_ulRefCount;
	bool	m_bAllowDrop;
};

void StartDragDrop(HWND hWnd, int nScreenID, int nWindowID, CFSRC menuCFSRC = CFSRC_TM, std::string strMemberID = "", int nChannelID = -1);
