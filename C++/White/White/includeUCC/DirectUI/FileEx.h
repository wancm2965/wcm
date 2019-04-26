
#if !defined (_DUI_FILEEX_INCLUDE_)
#define _DUI_FILEEX_INCLUDE_

#pragma once

#include <stdio.h>
#include <stdlib.h>

#ifdef DIRECTUI_EXPORTS
#define DIRECTUI_API __declspec(dllexport)
#else
#define DIRECTUI_API __declspec(dllimport)
#endif

class DIRECTUI_API CFileEx
{ 
public:
	CFileEx(void) { };
	virtual ~CFileEx() { };

	virtual bool	Close() = 0;
	virtual size_t	Read(void *buffer, size_t size, size_t count) = 0;
	virtual size_t	Write(const void *buffer, size_t size, size_t count) = 0;
	virtual bool	Seek(long offset, int origin) = 0;
	virtual long	Tell() = 0;
	virtual long	Size() = 0;
	virtual bool	Flush() = 0;
	virtual bool	Eof() = 0;
	virtual long	Error() = 0;
	virtual bool	PutC(unsigned char c)
	{
		// Default implementation
		size_t nWrote = Write(&c, 1, 1);
		return (bool)(nWrote == 1);
	}
	virtual long	GetC() = 0;
};

//////////////////////////////////////////////////////////
class DIRECTUI_API CIOFile : public CFileEx
{
public:
	CIOFile(FILE* fp = NULL)
	{
		m_fp = fp;
		m_bCloseFile = (bool)(fp==0);
	}

	~CIOFile()
	{
		Close();
	}
//////////////////////////////////////////////////////////
	bool Open(const char *filename, const char *mode)
	{
		if (m_fp) return false;	// Can't re-open without closing first

		fopen_s(&m_fp, filename, mode);
		if (!m_fp) return false;

		m_bCloseFile = true;

		return true;
	}
//////////////////////////////////////////////////////////
	virtual bool Close()
	{
		int iErr = 0;
		if ( (m_fp) && (m_bCloseFile) ){ 
			iErr = fclose(m_fp);
			m_fp = NULL;
		}
		return (bool)(iErr==0); 
	}
//////////////////////////////////////////////////////////
	virtual size_t	Read(void *buffer, size_t size, size_t count)
	{
		if (!m_fp) return 0;
		return fread(buffer, size, count, m_fp);
	}
//////////////////////////////////////////////////////////
	virtual size_t	Write(const void *buffer, size_t size, size_t count)
	{
		if (!m_fp) return 0;
		return fwrite(buffer, size, count, m_fp);
	}
//////////////////////////////////////////////////////////
	virtual bool Seek(long offset, int origin)
	{
		if (!m_fp) return false;
		return (bool)(fseek(m_fp, offset, origin) == 0);
	}
//////////////////////////////////////////////////////////
	virtual long Tell()
	{
		if (!m_fp) return 0;
		return ftell(m_fp);
	}
//////////////////////////////////////////////////////////
	virtual long	Size()
	{
		if (!m_fp) return -1;
		long pos,size;
		pos = ftell(m_fp);
		fseek(m_fp, 0, SEEK_END);
		size = ftell(m_fp);
		fseek(m_fp, pos,SEEK_SET);
		return size;
	}
//////////////////////////////////////////////////////////
	virtual bool	Flush()
	{
		if (!m_fp) return false;
		return (bool)(fflush(m_fp) == 0);
	}
//////////////////////////////////////////////////////////
	virtual bool	Eof()
	{
		if (!m_fp) return true;
		return (bool)(feof(m_fp) != 0);
	}
//////////////////////////////////////////////////////////
	virtual long	Error()
	{
		if (!m_fp) return -1;
		return ferror(m_fp);
	}
//////////////////////////////////////////////////////////
	virtual bool PutC(unsigned char c)
	{
		if (!m_fp) return false;
		return (bool)(fputc(c, m_fp) == c);
	}
//////////////////////////////////////////////////////////
	virtual long	GetC()
	{
		if (!m_fp) return EOF;
		return getc(m_fp);
	}
//////////////////////////////////////////////////////////
protected:
	FILE *m_fp;
	bool m_bCloseFile;
};

//////////////////////////////////////////////////////////
class DIRECTUI_API CMemFileEx : public CFileEx 
{ 
public:
	CMemFileEx(BYTE* pBuffer = NULL, DWORD size = 0)
	{
		m_pBuffer = pBuffer;
		m_Position = 0;
		m_Size = m_Edge = size;
		m_bFreeOnClose = (bool)(pBuffer==0);
	}
//////////////////////////////////////////////////////////
	~CMemFileEx()
	{
		Close();
	}
//////////////////////////////////////////////////////////
	virtual bool Close()
	{
		if ( (m_pBuffer) && (m_bFreeOnClose) ){
			free(m_pBuffer);
			m_pBuffer = NULL;
			m_Size = 0;
		}
		return true;
	}
//////////////////////////////////////////////////////////
	bool Open()
	{
		if (m_pBuffer) return false;	// Can't re-open without closing first

		m_Position = m_Size = m_Edge = 0;
		m_pBuffer=(BYTE*)malloc(0);
		m_bFreeOnClose = true;

		return (m_pBuffer!=0);
	}
//////////////////////////////////////////////////////////
BYTE* GetBuffer(bool bDetachBuffer = true) { m_bFreeOnClose = !bDetachBuffer; return m_pBuffer;}
//////////////////////////////////////////////////////////
virtual size_t	Read(void *buffer, size_t size, size_t count)
{
	if (m_pBuffer==NULL) return 0;
	if (buffer==NULL) return 0;
	if (m_Position > (long)m_Size) return 0;
	
	long nCount = (long)(count*size);
	if (nCount == 0) return 0;
	
	long nRead;
	if (m_Position + nCount > (long)m_Size)
		nRead = (m_Size - m_Position);
	else
		nRead = nCount;
	
	memcpy(buffer, m_pBuffer + m_Position, nRead);
	m_Position += nRead;
	
	return (size_t)(nRead/size);
}

virtual size_t	Write(const void *buffer, size_t size, size_t count)
{
	if (m_pBuffer==NULL) return 0;
	if (buffer==NULL) return 0;
	
	long nCount = (long)(count*size);
	if (nCount == 0) return 0;
	
	if (m_Position + nCount > m_Edge) Alloc(m_Position + nCount);
	
	memcpy(m_pBuffer + m_Position, buffer, nCount);
	
	m_Position += nCount;
	
	if (m_Position > (long)m_Size) m_Size = m_Position;
	
	return count;
}
virtual bool	Seek(long offset, int origin)
{
	if (m_pBuffer==NULL) return false;
	long lNewPos = m_Position;
	
	if (origin == SEEK_SET)		 lNewPos = offset;
	else if (origin == SEEK_CUR) lNewPos += offset;
	else if (origin == SEEK_END) lNewPos = m_Size + offset;
	else return false;
	
	if (lNewPos < 0) return false;
	
	m_Position = lNewPos;
	return true;
}
	virtual long	Tell()
	{
		if (m_pBuffer==NULL) return -1;
		return m_Position;

	}
	virtual long	Size()
	{
		if (m_pBuffer==NULL) return -1;
		return m_Size;
	}

	virtual bool	Flush()
	{
		if (m_pBuffer==NULL) return false;
		return true;
	}

	virtual bool	Eof()
	{
		if (m_pBuffer==NULL) return true;
		return (m_Position >= (long)m_Size);
	}

	virtual long	Error()
	{
		if (m_pBuffer==NULL) return -1;
		return (m_Position > (long)m_Size);
	}

	virtual bool	PutC(unsigned char c)
	{
		if (m_pBuffer==NULL) return false;
		if (m_Position + 1 > m_Edge) Alloc(m_Position + 1);
		
		memcpy(m_pBuffer + m_Position, &c, 1);
		
		m_Position += 1;
		
		if (m_Position > (long)m_Size) m_Size = m_Position;
		
		return true;
	}

	virtual long	GetC()
	{
		if (Eof()) return EOF;
		return *(BYTE*)((BYTE*)m_pBuffer + m_Position++);
	}

protected:
	void	Alloc(DWORD dwNewLen)
	{
		if (dwNewLen > (DWORD)m_Edge)
		{
			// determine new buffer size
			DWORD dwNewBufferSize = (DWORD)m_Edge;
			while (dwNewBufferSize < dwNewLen) dwNewBufferSize += 4096;
			
			// allocate new buffer
			if (m_pBuffer == NULL) m_pBuffer = (BYTE*)malloc(dwNewBufferSize);
			else	m_pBuffer = (BYTE*)realloc(m_pBuffer, dwNewBufferSize);
			// I own this buffer now (caller knows nothing about it)
			m_bFreeOnClose = true;
			
			m_Edge = dwNewBufferSize;
		}
		return;
	}

	void	Free()
	{
		Close();
	}

protected:
	BYTE*	m_pBuffer;
	DWORD	m_Size;
	bool	m_bFreeOnClose;
	long	m_Position;	//current position
	long	m_Edge;		//buffer size
};

#endif //_DUI_FILEEX_INCLUDE_