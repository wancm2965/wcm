// DSDisplayGraph.h: interface for the DSDisplayGraph class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DSDISPLAYGRAPH_H__
#define __DSDISPLAYGRAPH_H__


#include "DSOutputFilter.h"

class DSDisplayGraph 
{
public:
	DSDisplayGraph(void);
	virtual ~DSDisplayGraph(void);
public:
	virtual int Open(HWND hWndParent);
	virtual void Close(void);

	virtual int SetTitle(const char*cszTitle,unsigned long nColor);
	
	virtual bool DisplayFrame(unsigned char*pYUV420Data[3],int nStride[3],int nWidth,int nHeight);
	
	virtual void SetImageFormat(int nWidth,int nHeight);

	virtual int GetWidth(void){return m_nWidth;};
	virtual int GetHeight(void){return m_nHeight;};

	IVideoWindow*GetVideoWindow(void){return m_pVideoWindow;};

	HRESULT Connect(void);
	HRESULT Disconnect(void);

	HRESULT Start(void);
	HRESULT Stop(void);
	bool IsRunning(void);
protected:
	HRESULT CreateDisplayGraph(void);
protected:
	IVideoWindow*			m_pVideoWindow;
	IGraphBuilder*			m_pGraphBuilder;
	IMediaControl*			m_pMediaController;
	DSOutputFilter*			m_pSourceFilter;
	IBaseFilter*			m_pColorspaceConverterFilter;
	IBaseFilter*			m_pVideoRendererFilter;

	int						m_nWidth;
	int						m_nHeight;

	bool					m_bRunning;
	HWND					m_hWndParent;
	bool					m_bConnected;
};	


#endif 