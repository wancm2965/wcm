//
// MemStream.h
//
#include <streams.h>
#include "asyncio.h"
#include "asyncrdr.h"
#include "FileReader.h"

class CMemStream 
	: public CAsyncStream
{
public:
	IFileReader*	 m_pFileReader;
	INT				 m_nRenderBlocks;
	BOOL			 m_bReanderOK;
	BOOL			 m_bClosed;
public:
    CMemStream( LONGLONG llLength, DWORD dwKBPerSec = INFINITE) :
        m_llLength(llLength),
        m_llPosition(0),
        m_dwKBPerSec(dwKBPerSec)
    {
        m_dwTimeStart = timeGetTime();
		m_pFileReader=NULL;
		m_nRenderBlocks=0;
		m_bReanderOK=FALSE;
    }

	HRESULT SetPointer(LONGLONG llPos)
    {
        if (llPos < 0 || llPos > m_llLength) {
            return S_FALSE;
        } else {
            m_llPosition = llPos;
            return S_OK;
        }
    }
    HRESULT Read(PBYTE pbBuffer,
                 DWORD dwBytesToRead,
                 BOOL bAlign,
                 LPDWORD pdwBytesRead)
    {
        CAutoLock lck(&m_csLock);

        DWORD dwReadLength;
		BOOL bRet=FALSE;
        /*  Wait until the bytes are here! */
        DWORD dwTime = timeGetTime();
        if (m_llPosition + dwBytesToRead > m_llLength) {
            dwReadLength = (DWORD)(m_llLength - m_llPosition);
        } else {
            dwReadLength = dwBytesToRead;
        }
        DWORD dwTimeToArrive =
            ((DWORD)m_llPosition + dwReadLength) / m_dwKBPerSec;
        if (dwTime - m_dwTimeStart < dwTimeToArrive) {
        }
		if (m_pFileReader){
			*pdwBytesRead=m_pFileReader->ReadData(pbBuffer,(DWORD)m_llPosition,dwReadLength,1000);
			m_llPosition += *pdwBytesRead;
		}

        return S_OK;
    }
    LONGLONG Size(LONGLONG *pSizeAvailable)
    {
		if (pSizeAvailable)
			*pSizeAvailable = m_llLength;

        return m_llLength;
    }
    DWORD Alignment()
    {
        return 32768;
    }
    void Lock()
    {
        m_csLock.Lock();
    }
    void Unlock()
    {
        m_csLock.Unlock();
    }

private:
    CCritSec       m_csLock;
    LONGLONG	   m_llLength;
    LONGLONG       m_llPosition;
    DWORD          m_dwKBPerSec;
    DWORD          m_dwTimeStart;
};

class CMemReader : public CAsyncReader
{
public:

    //  We're not going to be CoCreate'd so we don't need registration
    //  stuff etc
    STDMETHODIMP Register()
    {
        return S_OK;
    }
    STDMETHODIMP Unregister()
    {
        return S_OK;
    }
    CMemReader(CMemStream *pStream, CMediaType *pmt, HRESULT *phr) :
        CAsyncReader(NAME("Mem Reader"), NULL, pStream, phr)
    {
        m_mt = *pmt;
    }
};