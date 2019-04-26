/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef OWDIRECTXWEBCAMDRIVER_H
#define OWDIRECTXWEBCAMDRIVER_H

#include "VIDEC_Camera.h"

typedef enum {
	PIX_OSI_UNSUPPORTED = -1,
	PIX_OSI_YUV420P = 0, /* this value: 0 should not be modified. It is currently used as 0 in phmedia-video.c */
	PIX_OSI_I420,
	PIX_OSI_YUV422,
	PIX_OSI_YUV411,
	PIX_OSI_YUV422P,
	PIX_OSI_YUV444P,
	PIX_OSI_YUV424P,
	PIX_OSI_YUV41P,
	PIX_OSI_YUY2,
	PIX_OSI_YUYV,
	PIX_OSI_YVYU,
	PIX_OSI_UYVY,
	PIX_OSI_YV12,
	PIX_OSI_RGB555,
	PIX_OSI_RGB565,
	PIX_OSI_RGB1,
	PIX_OSI_RGB4,
	PIX_OSI_RGB8,
	PIX_OSI_RGBA32, //18
	PIX_OSI_RGB32, //19
	PIX_OSI_ARGB32, //20
	PIX_OSI_RGB24, //21
	PIX_OSI_BGR24, //22
	PIX_OSI_NV12 //23
} pixosi;

//Otherwise CoInitializeEx() is not recognized
//see http://support.microsoft.com/newsgroups/default.aspx?dg=microsoft.public.vc.atl&tid=85edd07d-040c-47a6-8e82-ddedfa77ce70&p=1
#define _WIN32_DCOM
//#define _WIN32_WINNT 0x0400

#include <tchar.h>
#include <dshow.h>
#include <atlbase.h>
#include <qedit.h>
#include <comutil.h>

#include <iostream>
#include <string>

/**
 * DirectX implementation of webcam driver
 *
 * @author Mathieu Stute
 * @author David Ferlier
 * @author Jerome Wagner
 * @author Philippe Bernery
 */
class DirectXWebcamDriver 
	: public VIDEC_Camera
	, public ISampleGrabberCB 
{
public:

	DirectXWebcamDriver(VIDEC_CameraCallback&rCallback);

	virtual ~DirectXWebcamDriver();

public:
	static void SetInput(int nDevice,int nInput);
	static void GetInput(int nDevice,int&nInput);
public:
	int Open(int nDevice,int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate,bool bAnyFormat);
	void Close(void);

	void ShowAdjustVideoWnd(HWND hWnd);
	void ShowVideoInputWnd(HWND hWnd);
	bool HasVideoInputWnd(void);
	bool TVTunerPutChannel(long lChannel);
protected:
	HRESULT ShowPropertyPage(HWND hwndParent,IUnknown* pIU, const WCHAR* name);
	void GetCapDevice(INT nCapDevice,IBaseFilter**ppCap);
	HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin);
	IPin * GetInPin ( IBaseFilter *pFilter, int Num );
	IPin * GetOutPin( IBaseFilter *pFilter, int Num );



	bool isOpen() const;

	void startCapture();


	void stopCapture();

	HRESULT setPalette(pixosi palette);

	pixosi getPalette() const;


	unsigned getFPS() const;

	HRESULT setResolution(unsigned width, unsigned height);

	unsigned getWidth() const;

	unsigned getHeight() const;

	void setBrightness(int brightness);

	int getBrightness() const;

	void setContrast(int contrast);

	int getContrast() const;


	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }

	// fake out any COM QI'ing
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);

	// callback to access the buffer - the original buffer is passed
	STDMETHODIMP SampleCB(double SampleTime, IMediaSample * pSample);

	// callback to access the buffer - a copy is passed
	STDMETHODIMP BufferCB(double dblSampleTime, BYTE * pBuffer, long lBufferSize);

private:
	CComPtr< IMediaControl > _pControl;
	CComPtr< IGraphBuilder > _pGraph;
	CComPtr< ICaptureGraphBuilder2 > _pBuild;
	CComPtr< IBaseFilter > _pCap;
	IBaseFilter * _pGrabberF;
	ISampleGrabber * _pGrabber;
	IBaseFilter * _pNull;
	IAMStreamConfig * _iam;

	bool					m_bCrossBar;
	int						m_nDevice;
	VIDEC_CameraCallback&	m_rCallback;

	/** Captured image width */
	unsigned _cachedWidth;

	/** Captured image height */
	unsigned _cachedHeight;

	/** Frame per seconds */
	unsigned _cachedFPS;

	/** Desired image palette */
	pixosi _cachedPalette;

	/** True if webcam is opened */
	bool _isOpen;

	/** Read device capabilities */
	void readCaps();

	/** Set device capabilites */
	HRESULT setCaps(pixosi palette, unsigned fps, unsigned resolutionWidth, unsigned resolutionHeight);
};

#endif	//OWDIRECTXWEBCAMDRIVER_H
