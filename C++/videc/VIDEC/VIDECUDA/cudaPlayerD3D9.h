//CudaDecodeD3D9.h
#ifndef __CUDADECODED3D9_H__
#define __CUDADECODED3D9_H__

#include "cudaPlayer.h"

#include <d3dx9.h>
#include <cuda.h>
#include <cuda_runtime_api.h>

#include <cudad3d9.h>

#include "VideoDecoder.h"
#include "ImageDX.h"

#include "cudaProcessFrame.h"
#include "cudaModuleMgr.h"


class cudaPlayerD3D9 
{
public:
	cudaPlayerD3D9(void);
	virtual ~cudaPlayerD3D9(void);
public:
	int Open( HWND hWnd, int nWidth, int nHeight );
	void Close( void) ;
	int PlayFrame( const unsigned char *pFrameData, int nFrameLen , int nFrameRate, int&nFlagReset) ;

	int Resize(void);

	int GetActualWidth(void);
	int GetActualHeight(void);

	int SkipDisplay(int nFlagSkipDisplay);
private:
	int RenderVideoFrame(CUVIDPARSERDISPINFO*pDisplayInfo) ;

	bool CreateCudaResources(void);

	bool CreateD3D9(void);
	void DestroyD3D9(void);

	bool CreateD3D9Device(void);
	void DestroyD3D9Device(void);

	HRESULT CreateD3D9Surface(unsigned int nWidth, unsigned int nHeight);

	HRESULT DestroyD3D9Surface();

	bool CreateCudaDecoder(void);
	void DestroyCudaDecoder(void);

	bool CreateCudaParser(void);
	void DestroyCudaParser(void);

	void DestroyCudaResources(void);

	bool copyDecodedFrameToTexture( CUVIDPARSERDISPINFO& oDisplayInfo, int *pbIsProgressive );

	void cudaPostProcessFrame(CUdeviceptr * ppDecodedFrame, size_t nDecodedPitch, 
                     CUdeviceptr * ppTextureData,  size_t nTexturePitch,
                     CUmodule cuModNV12toARGB, 
                     CUfunction fpCudaKernel, CUstream streamID);

	HRESULT drawScene(int field_num);


	void setD3D9View( );

	bool EnableDwmQueuing();

	static 
    int CUDAAPI 
    HandleVideoSequence(void * pUserData, CUVIDEOFORMAT * pFormat);

    // Called by the video parser to decode a single picture
    // Since the parser will deliver data as fast as it can, we need to make sure that the picture
    // index we're attempting to use for decode is no longer used for display
    static 
    int CUDAAPI 
    HandlePictureDecode(void * pUserData, CUVIDPICPARAMS * pPicParams);

    // Called by the video parser to display a video frame (in the case of field pictures, there may be
    // 2 decode calls per 1 display call, since two fields make up one frame)
    static
    int CUDAAPI 
    HandlePictureDisplay(void * pUserData, CUVIDPARSERDISPINFO *cuviddisp);


private:
	CUVIDPARSERDISPINFO*m_pDisplayInfo;
	bool                m_bDeviceLost ;

	bool                m_bUpdateCSC  ;
	bool                m_bUpdateAll  ;

	CUvideoctxlock       m_CtxLock ;

	D3DDISPLAYMODE        m_d3ddm;    
	D3DPRESENT_PARAMETERS m_d3dpp;    

	IDirect3D9		 * m_pD3D; // Used to create the D3DDevice
	IDirect3DDevice9 * m_pD3DDevice;

	// These are CUDA function pointers to the CUDA kernels
	CUmoduleManager  * m_pCudaModule;

	CUmodule           cuModNV12toARGB  ;
	CUfunction         gfpNV12toARGB    ;

	CUcontext          m_oContext ;
	CUdevice           m_oDevice  ;

	eColorSpace        m_eColorSpace ;
	float              m_nHue        ;

	VideoDecoder*		m_pVideoDecoder;

	ImageDX*			m_pImageDX;

	HWND				m_hWnd;


	CUvideoparser		m_hVideoParser;

	int					m_nActualWidth;
	int					m_nActualHeight;
	int					m_nFlagReset;

	unsigned int		m_nAdapter;
	int					m_nFlagSkipDisplay;
	HMONITOR			m_hMonitor;
	bool				m_bDwmEnabled;
	int					m_nFrameRate;

	bool				m_bMonitorChanged;

	unsigned long		m_nStartTimestamp;

	unsigned long			m_nLastTS;
	unsigned long			m_nFrameCount;
	unsigned long			m_nSkipFrameCount;

};


#endif