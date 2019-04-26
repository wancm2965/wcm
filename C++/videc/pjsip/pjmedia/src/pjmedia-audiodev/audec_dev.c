///////////////////////////////////////////////////////////////////////////////////////////////////////
// audec_dev.c : 2012/01/10 : liurui
#include <pjmedia-audiodev/audiodev_imp.h>
#include <pj/assert.h>
#include <pj/log.h>
#include <pj/os.h>
#include <pj/string.h>
#include <pj/unicode.h>

#include <windows.h>
static CRITICAL_SECTION m_AudecAudioCritSec;

static int isNoSoundDev = 0;
int GetSoundDevSum(void)
{
	return isNoSoundDev;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define THIS_FILE			"audec_dev.c"

#ifdef _MSC_VER
#   pragma warning(push, 3)
#endif

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

#ifndef PJMEDIA_WMME_DEV_USE_MMDEVICE_API
#   define PJMEDIA_WMME_DEV_USE_MMDEVICE_API \
	(defined(_WIN32_WINNT) && (_WIN32_WINNT>=0x0600))
#endif

#if PJMEDIA_WMME_DEV_USE_MMDEVICE_API != 0
#   define DRV_QUERYFUNCTIONINSTANCEID     (DRV_RESERVED + 17)
#   define DRV_QUERYFUNCTIONINSTANCEIDSIZE (DRV_RESERVED + 18)
#endif

/* mingw lacks WAVE_FORMAT_ALAW/MULAW */
#ifndef WAVE_FORMAT_ALAW
#   define  WAVE_FORMAT_ALAW       0x0006
#endif
#ifndef WAVE_FORMAT_MULAW
#   define  WAVE_FORMAT_MULAW      0x0007
#endif

#if defined(PJ_WIN32_WINCE) && PJ_WIN32_WINCE!=0
#   pragma comment(lib, "Coredll.lib")
#elif defined(_MSC_VER)
#   pragma comment(lib, "winmm.lib")
#endif



/* WMME device info */
struct audec_dev_info
{
	pjmedia_aud_dev_info	 info;
	unsigned			 deviceId;
	const wchar_t		*endpointId;
};

/* WMME factory */
struct audec_factory
{
	pjmedia_aud_dev_factory	 base;
	pj_pool_t			*base_pool;
	pj_pool_t			*pool;
	pj_pool_factory		*pf;

	unsigned			 dev_count;
	struct audec_dev_info	*dev_info;
};

/* Individual WMME capture/playback stream descriptor */
struct audec_channel
{
	union
	{
		HWAVEIN   In;
		HWAVEOUT  Out;
	} hWave;

	WAVEHDR      *WaveHdr;
	HANDLE        hEvent;
	DWORD         dwBufIdx;
	DWORD         dwMaxBufIdx;
	pj_timestamp  timestamp;
};


/* Sound stream. */
struct audec_stream
{
	pjmedia_aud_stream	 base;		    /**< Base stream	       */
	pjmedia_aud_param	 param;		    /**< Settings	       */
	pj_pool_t           *pool;              /**< Memory pool.          */

	pjmedia_aud_rec_cb   rec_cb;            /**< Capture callback.     */
	pjmedia_aud_play_cb  play_cb;           /**< Playback callback.    */
	void                *user_data;         /**< Application data.     */

	pj_bool_t		 quit_flag;

	pj_bool_t		 rec_thread_exited;
	pj_bool_t		 rec_thread_initialized;
	pj_thread_desc	 rec_thread_desc;
	pj_thread_t		*rec_thread;

	pj_bool_t		 play_thread_exited;
	pj_bool_t		 play_thread_initialized;
	pj_thread_desc	 play_thread_desc;
	pj_thread_t		*play_thread;


	struct audec_channel  play_strm;         /**< Playback stream.      */
	struct audec_channel  rec_strm;          /**< Capture stream.       */

	void    		*buffer;	    /**< Temp. frame buffer.   */
	pjmedia_format_id	 fmt_id;	    /**< Frame format	       */
	pj_uint8_t		 silence_char;	    /**< Silence pattern       */
	unsigned		 bytes_per_frame;   /**< Bytes per frame       */

	pjmedia_frame_ext	*xfrm;		    /**< Extended frame buffer */
	unsigned		 xfrm_size;	    /**< Total ext frm size    */

	pj_thread_t         *thread;            /**< Thread handle.        */
	HANDLE               thread_quit_event; /**< Quit signal to thread */
};

///////////////////////////////////////////////////
//
static struct audec_stream * m_gaudecStream = NULL;
static pj_timestamp  play_timestamp = {0};
static pj_timestamp  capture_timestamp = {0};


/* Prototypes */
static pj_status_t audec_factory_init(pjmedia_aud_dev_factory *f);
static pj_status_t audec_factory_destroy(pjmedia_aud_dev_factory *f);
static pj_status_t audec_factory_refresh(pjmedia_aud_dev_factory *f);
static unsigned    audec_factory_get_dev_count(pjmedia_aud_dev_factory *f);
static pj_status_t audec_factory_get_dev_info(pjmedia_aud_dev_factory *f, 
											   unsigned index,
											   pjmedia_aud_dev_info *info);
static pj_status_t audec_factory_default_param(pjmedia_aud_dev_factory *f,
												unsigned index,
												pjmedia_aud_param *param);
static pj_status_t audec_factory_create_stream(pjmedia_aud_dev_factory *f,
												const pjmedia_aud_param *param,
												pjmedia_aud_rec_cb rec_cb,
												pjmedia_aud_play_cb play_cb,
												void *user_data,
												pjmedia_aud_stream **p_aud_strm);

static pj_status_t stream_get_param(pjmedia_aud_stream *strm,
									pjmedia_aud_param *param);
static pj_status_t stream_get_cap(pjmedia_aud_stream *strm,
								  pjmedia_aud_dev_cap cap,
								  void *value);
static pj_status_t stream_set_cap(pjmedia_aud_stream *strm,
								  pjmedia_aud_dev_cap cap,
								  const void *value);
static pj_status_t stream_start(pjmedia_aud_stream *strm);
static pj_status_t stream_stop(pjmedia_aud_stream *strm);
static pj_status_t stream_destroy(pjmedia_aud_stream *strm);


/* Operations */
static pjmedia_aud_dev_factory_op audec_factory_op =
{
	&audec_factory_init,
	&audec_factory_destroy,
	&audec_factory_get_dev_count,
	&audec_factory_get_dev_info,
	&audec_factory_default_param,
	&audec_factory_create_stream,
	&audec_factory_refresh
};

static pjmedia_aud_stream_op stream_op = 
{
	&stream_get_param,
	&stream_get_cap,
	&stream_set_cap,
	&stream_start,
	&stream_stop,
	&stream_destroy
};


/****************************************************************************
* Factory operations
*/
/*
* Init WMME audio driver.
*/
pjmedia_aud_dev_factory* pjmedia_audec_factory(pj_pool_factory *pf)
{
	struct audec_factory *f;
	pj_pool_t *pool;

	pool = pj_pool_create(pf, "audec base", 1000, 1000, NULL);
	f = PJ_POOL_ZALLOC_T(pool, struct audec_factory);
	f->pf = pf;
	f->base_pool = pool;
	f->base.op = &audec_factory_op;

	return &f->base;
}

/* Internal: Windows Vista and Windows 7 have their device
* names truncated when using the waveXXX api.  The names
* should be acquired from the MMDevice APIs
*/
#if PJMEDIA_WMME_DEV_USE_MMDEVICE_API != 0

#define COBJMACROS
#include <mmdeviceapi.h>
#define INITGUID
#include <Guiddef.h>
#include <FunctionDiscoveryKeys_devpkey.h>

DEFINE_GUID(CLSID_MMDeviceEnumerator, 0xBCDE0395, 0xE52F, 0x467C,
			0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E);
DEFINE_GUID(IID_IMMDeviceEnumerator, 0xA95664D2, 0x9614, 0x4F35,
			0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6);

static void get_dev_names(pjmedia_aud_dev_factory *f)
{
	struct audec_factory *wf = (struct audec_factory*)f;
	HRESULT              coinit = S_OK;
	HRESULT              hr = S_OK;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDeviceCollection *pDevices = NULL;
	UINT                 cDevices = 0;
	UINT                 nDevice = 0;

	coinit = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (coinit == RPC_E_CHANGED_MODE)
		coinit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (FAILED(coinit))
		goto on_error;

	hr = CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_INPROC_SERVER, &IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	if (FAILED(hr))
		goto on_error;
	hr = IMMDeviceEnumerator_EnumAudioEndpoints(pEnumerator, eAll,
		DEVICE_STATE_ACTIVE,
		&pDevices);
	if (FAILED(hr))
		goto on_error;
	hr = IMMDeviceCollection_GetCount(pDevices, &cDevices);
	if (FAILED(hr))
		goto on_error;

	for (nDevice = 0; nDevice < cDevices; ++nDevice) {
		IMMDevice      *pDevice = NULL;
		IPropertyStore *pProps = NULL;
		LPWSTR          pwszID = NULL;
		PROPVARIANT     varName;
		unsigned        i;

		PropVariantInit(&varName);

		hr = IMMDeviceCollection_Item(pDevices, nDevice, &pDevice);
		if (FAILED(hr))
			goto cleanup;
		hr = IMMDevice_GetId(pDevice, &pwszID);
		if (FAILED(hr))
			goto cleanup;
		hr = IMMDevice_OpenPropertyStore(pDevice, STGM_READ, &pProps);
		if (FAILED(hr))
			goto cleanup;
		hr = IPropertyStore_GetValue(pProps, &PKEY_Device_FriendlyName,
			&varName);
		if (FAILED(hr))
			goto cleanup;

		for (i = 0; i < wf->dev_count; ++i) {
			if (0 == wcscmp(wf->dev_info[i].endpointId, pwszID)) {
				wcstombs(wf->dev_info[i].info.name, varName.pwszVal,
					sizeof(wf->dev_info[i].info.name));
				break;
			}
		}

		PropVariantClear(&varName);

cleanup:
		if (pProps)
			IPropertyStore_Release(pProps);
		if (pwszID)
			CoTaskMemFree(pwszID);
		if (pDevice)
			hr = IMMDevice_Release(pDevice);
	}

on_error:
	if (pDevices)
		hr = IMMDeviceCollection_Release(pDevices);

	if (pEnumerator)
		hr = IMMDeviceEnumerator_Release(pEnumerator);

	if (SUCCEEDED(coinit))
		CoUninitialize();
}

#else

static void get_dev_names(pjmedia_aud_dev_factory *f)
{
	PJ_UNUSED_ARG(f);
}

#endif

/* Internal: build device info from WAVEINCAPS/WAVEOUTCAPS */
static void build_dev_info(UINT deviceId, struct audec_dev_info *wdi, 
						   const WAVEINCAPS *wic, const WAVEOUTCAPS *woc)
{
#define WIC_WOC(wic,woc,field)	(wic? wic->field : woc->field)

	pj_bzero(wdi, sizeof(*wdi));
	wdi->deviceId = deviceId;

	/* Device Name */
	if (deviceId==WAVE_MAPPER) {
		strncpy(wdi->info.name, "Wave mapper", sizeof(wdi->info.name));
		wdi->info.name[sizeof(wdi->info.name)-1] = '\0';
	} else {
		const pj_char_t *szPname = WIC_WOC(wic, woc, szPname);
		PJ_DECL_ANSI_TEMP_BUF(wTmp, sizeof(wdi->info.name));

		strncpy(wdi->info.name, 
			PJ_NATIVE_TO_STRING(szPname, wTmp, PJ_ARRAY_SIZE(wTmp)),
			sizeof(wdi->info.name));
		wdi->info.name[sizeof(wdi->info.name)-1] = '\0';
	}

	wdi->info.default_samples_per_sec = 16000;
	strcpy(wdi->info.driver, "WMME");

	if (wic) {
		wdi->info.input_count = wic->wChannels;
		wdi->info.caps |= PJMEDIA_AUD_DEV_CAP_INPUT_LATENCY;

		/* Sometimes a device can return a rediculously large number of 
		* channels. This happened with an SBLive card on a Windows ME box.
		* It also happens on Win XP!
		*/
		if (wdi->info.input_count<1 || wdi->info.input_count>256) {
			wdi->info.input_count = 2;
		}
	}

	if (woc) {
		wdi->info.output_count = woc->wChannels;
		wdi->info.caps |= PJMEDIA_AUD_DEV_CAP_OUTPUT_LATENCY;

		if (woc->dwSupport & WAVECAPS_VOLUME) {
			wdi->info.caps |= PJMEDIA_AUD_DEV_CAP_OUTPUT_VOLUME_SETTING;
		}

		/* Sometimes a device can return a rediculously large number of 
		* channels. This happened with an SBLive card on a Windows ME box.
		* It also happens on Win XP!
		*/
		if (wdi->info.output_count<1 || wdi->info.output_count>256) {
			wdi->info.output_count = 2;
		}
	}

	/* Extended formats */
	wdi->info.caps |= PJMEDIA_AUD_DEV_CAP_EXT_FORMAT;
	wdi->info.ext_fmt_cnt = 2;
	wdi->info.ext_fmt[0].id = PJMEDIA_FORMAT_PCMU;
	//wdi->info.ext_fmt[0].bitrate = 64000;
	//wdi->info.ext_fmt[0].vad = 0;
	wdi->info.ext_fmt[1].id = PJMEDIA_FORMAT_PCMA;
	//wdi->info.ext_fmt[1].bitrate = 64000;
	//wdi->info.ext_fmt[1].vad = 0;
}

/* API: init factory */
static pj_status_t audec_factory_init(pjmedia_aud_dev_factory *f)
{
	pj_status_t ret = audec_factory_refresh(f);
	if (ret != PJ_SUCCESS)
		return ret;

	

	PJ_LOG(4, (THIS_FILE, "audec audio initialized"));
	return PJ_SUCCESS;
}

/* API: refresh the device list */
static pj_status_t audec_factory_refresh(pjmedia_aud_dev_factory *f)
{
	struct audec_factory *wf = (struct audec_factory*)f;
	unsigned c;
	int i;
	int inputDeviceCount, outputDeviceCount, devCount=0;
	pj_bool_t waveMapperAdded = PJ_FALSE;

	if (wf->pool != NULL) {
		pj_pool_release(wf->pool);
		wf->pool = NULL;
	}

	/* Enumerate sound devices */
	wf->dev_count = 0;
	wf->pool = pj_pool_create(wf->pf, "audec", 1000, 1000, NULL);

	inputDeviceCount = waveInGetNumDevs();
	devCount += inputDeviceCount;

	outputDeviceCount = waveOutGetNumDevs();
	devCount += outputDeviceCount;

	// liurui
	{
		if( inputDeviceCount == 0 && outputDeviceCount == 0 ) 
		{
			isNoSoundDev = 1;
		}
		else 
		{
			isNoSoundDev = 0;
		}
	}
	// liurui

	if (devCount) {
		/* Assume there is WAVE_MAPPER */
		devCount += 2;
	}

	if (devCount==0) {
		PJ_LOG(4,(THIS_FILE, "audec found no sound devices"));
		/* Enabling this will cause pjsua-lib initialization to fail when there
		* is no sound device installed in the system, even when pjsua has been
		* run with --null-audio. Moreover, it might be better to think that
		* the WMME backend initialization is successfull, regardless there is
		* no audio device installed, as later application can check it using 
		* get_dev_count().
		return PJMEDIA_EAUD_NODEV;
		*/
		return PJ_SUCCESS;
	}

	wf->dev_info = (struct audec_dev_info*)
		pj_pool_calloc(wf->pool, devCount, 
		sizeof(struct audec_dev_info));

	//if (inputDeviceCount && outputDeviceCount) {
	//	/* Attempt to add WAVE_MAPPER as input and output device */
	//	WAVEINCAPS wic;
	//	MMRESULT mr;

	//	pj_bzero(&wic, sizeof(WAVEINCAPS));
	//	mr = waveInGetDevCaps(WAVE_MAPPER, &wic, sizeof(WAVEINCAPS));

	//	if (mr == MMSYSERR_NOERROR) {
	//		WAVEOUTCAPS woc;

	//		pj_bzero(&woc, sizeof(WAVEOUTCAPS));
	//		mr = waveOutGetDevCaps(WAVE_MAPPER, &woc, sizeof(WAVEOUTCAPS));
	//		if (mr == MMSYSERR_NOERROR) {
	//			build_dev_info(WAVE_MAPPER, &wf->dev_info[wf->dev_count], 
	//				&wic, &woc);
	//			wf->dev_info[wf->dev_count].endpointId = L"";
	//			++wf->dev_count;
	//			waveMapperAdded = PJ_TRUE;
	//		}
	//	}

	//}
	// {{ liurui 
	if (inputDeviceCount ) {
		/* Attempt to add WAVE_MAPPER as input and output device */
		WAVEINCAPS wic;
		MMRESULT mr;

		pj_bzero(&wic, sizeof(WAVEINCAPS));
		mr = waveInGetDevCaps(WAVE_MAPPER, &wic, sizeof(WAVEINCAPS));

		if (mr == MMSYSERR_NOERROR) {
			WAVEOUTCAPS woc;

			pj_bzero(&woc, sizeof(WAVEOUTCAPS));
			mr = waveOutGetDevCaps(WAVE_MAPPER, &woc, sizeof(WAVEOUTCAPS));
			if (mr == MMSYSERR_NOERROR) {
				build_dev_info(WAVE_MAPPER, &wf->dev_info[wf->dev_count], 
					&wic, &woc);
				wf->dev_info[wf->dev_count].endpointId = L"";
				++wf->dev_count;
				waveMapperAdded = PJ_TRUE;
			}
		}

	}
	else
	{
		if (outputDeviceCount) 
		{
			WAVEINCAPS wic;
			MMRESULT mr;
			WAVEOUTCAPS woc;
			pj_bzero(&woc, sizeof(WAVEOUTCAPS));
			mr = waveOutGetDevCaps(WAVE_MAPPER, &woc, sizeof(WAVEOUTCAPS));
			if (mr == MMSYSERR_NOERROR) {
				build_dev_info(WAVE_MAPPER, &wf->dev_info[wf->dev_count], 
					&wic, &woc);
				wf->dev_info[wf->dev_count].endpointId = L"";
				++wf->dev_count;
				waveMapperAdded = PJ_TRUE;
			}
		}
	}
	// }}

	if (inputDeviceCount > 0) {
		/* -1 is the WAVE_MAPPER */
		for (i = (waveMapperAdded? 0 : -1); i < inputDeviceCount; ++i) {
			UINT uDeviceID = (UINT)((i==-1) ? WAVE_MAPPER : i);
			WAVEINCAPS wic;
			MMRESULT mr;
			DWORD cbEndpointId;

			pj_bzero(&wic, sizeof(WAVEINCAPS));

			mr = waveInGetDevCaps(uDeviceID, &wic, sizeof(WAVEINCAPS));

			if (mr == MMSYSERR_NOMEM)
				return PJ_ENOMEM;

			if (mr != MMSYSERR_NOERROR)
				continue;

			build_dev_info(uDeviceID, &wf->dev_info[wf->dev_count], 
				&wic, NULL);

#if PJMEDIA_WMME_DEV_USE_MMDEVICE_API != 0
			/* Try to get the endpoint id of the audio device */
			wf->dev_info[wf->dev_count].endpointId = L"";

			mr = waveInMessage((HWAVEIN)IntToPtr(uDeviceID),
				DRV_QUERYFUNCTIONINSTANCEIDSIZE,
				(DWORD_PTR)&cbEndpointId, (DWORD_PTR)NULL);
			if (mr == MMSYSERR_NOERROR) {
				const wchar_t **epid = &wf->dev_info[wf->dev_count].endpointId;
				*epid = (const wchar_t*) pj_pool_calloc(wf->pool,
					cbEndpointId, 1);
				mr = waveInMessage((HWAVEIN)IntToPtr(uDeviceID),
					DRV_QUERYFUNCTIONINSTANCEID,
					(DWORD_PTR)*epid,
					cbEndpointId);
			}
#else
			PJ_UNUSED_ARG(cbEndpointId);
#endif

			++wf->dev_count;
		}
	}

	if( outputDeviceCount > 0 )
	{
		/* -1 is the WAVE_MAPPER */
		for (i = (waveMapperAdded? 0 : -1); i < outputDeviceCount; ++i) {
			UINT uDeviceID = (UINT)((i==-1) ? WAVE_MAPPER : i);
			WAVEOUTCAPS woc;
			MMRESULT mr;
			DWORD cbEndpointId;

			pj_bzero(&woc, sizeof(WAVEOUTCAPS));

			mr = waveOutGetDevCaps(uDeviceID, &woc, sizeof(WAVEOUTCAPS));

			if (mr == MMSYSERR_NOMEM)
				return PJ_ENOMEM;

			if (mr != MMSYSERR_NOERROR)
				continue;

			build_dev_info(uDeviceID, &wf->dev_info[wf->dev_count], 
				NULL, &woc);

#if PJMEDIA_WMME_DEV_USE_MMDEVICE_API != 0
			/* Try to get the endpoint id of the audio device */
			wf->dev_info[wf->dev_count].endpointId = L"";

			mr = waveOutMessage((HWAVEOUT)IntToPtr(uDeviceID),
				DRV_QUERYFUNCTIONINSTANCEIDSIZE,
				(DWORD_PTR)&cbEndpointId, (DWORD_PTR)NULL);
			if (mr == MMSYSERR_NOERROR) {
				const wchar_t **epid = &wf->dev_info[wf->dev_count].endpointId;
				*epid = (const wchar_t*)pj_pool_calloc(wf->pool,
					cbEndpointId, 1);
				mr = waveOutMessage((HWAVEOUT)IntToPtr(uDeviceID),
					DRV_QUERYFUNCTIONINSTANCEID,
					(DWORD_PTR)*epid, cbEndpointId);
			}
#else
			PJ_UNUSED_ARG(cbEndpointId);
#endif

			++wf->dev_count;
		}
	}

	/* On Windows Vista and Windows 7 get the full device names */
	get_dev_names(f);

	PJ_LOG(4, (THIS_FILE, "audec found %d devices:",
		wf->dev_count));
	for (c = 0; c < wf->dev_count; ++c) {
		PJ_LOG(4, (THIS_FILE, " dev_id %d: %s  (in=%d, out=%d)", 
			c,
			wf->dev_info[c].info.name,
			wf->dev_info[c].info.input_count,
			wf->dev_info[c].info.output_count));
	}

	return PJ_SUCCESS;
}

/* API: destroy factory */
static pj_status_t audec_factory_destroy(pjmedia_aud_dev_factory *f)
{
	struct audec_factory *wf = (struct audec_factory*)f;
	pj_pool_t *pool = wf->base_pool;

	

	pj_pool_release(wf->pool);
	wf->base_pool = NULL;
	pj_pool_release(pool);

	return PJ_SUCCESS;
}

/* API: get number of devices */
static unsigned audec_factory_get_dev_count(pjmedia_aud_dev_factory *f)
{
	struct audec_factory *wf = (struct audec_factory*)f;
	return wf->dev_count;
}

/* API: get device info */
static pj_status_t audec_factory_get_dev_info(pjmedia_aud_dev_factory *f, 
											   unsigned index,
											   pjmedia_aud_dev_info *info)
{
	struct audec_factory *wf = (struct audec_factory*)f;

	PJ_ASSERT_RETURN(index < wf->dev_count, PJMEDIA_EAUD_INVDEV);

	pj_memcpy(info, &wf->dev_info[index].info, sizeof(*info));

	return PJ_SUCCESS;
}

/* API: create default device parameter */
static pj_status_t audec_factory_default_param(pjmedia_aud_dev_factory *f,
												unsigned index,
												pjmedia_aud_param *param)
{
	struct audec_factory *wf = (struct audec_factory*)f;
	struct audec_dev_info *di = &wf->dev_info[index];

	PJ_ASSERT_RETURN(index < wf->dev_count, PJMEDIA_EAUD_INVDEV);

	pj_bzero(param, sizeof(*param));
	if (di->info.input_count && di->info.output_count) {
		param->dir = PJMEDIA_DIR_CAPTURE_PLAYBACK;
		param->rec_id = index;
		param->play_id = index;
	} else if (di->info.input_count) {
		param->dir = PJMEDIA_DIR_CAPTURE;
		param->rec_id = index;
		param->play_id = PJMEDIA_AUD_INVALID_DEV;
	} else if (di->info.output_count) {
		param->dir = PJMEDIA_DIR_PLAYBACK;
		param->play_id = index;
		param->rec_id = PJMEDIA_AUD_INVALID_DEV;
	} else {
		return PJMEDIA_EAUD_INVDEV;
	}

	param->clock_rate = di->info.default_samples_per_sec;
	param->channel_count = 1;
	param->samples_per_frame = di->info.default_samples_per_sec * 20 / 1000;
	param->bits_per_sample = 16;
	param->flags = PJMEDIA_AUD_DEV_CAP_INPUT_LATENCY |
		PJMEDIA_AUD_DEV_CAP_OUTPUT_LATENCY;
	param->input_latency_ms = PJMEDIA_SND_DEFAULT_REC_LATENCY;
	param->output_latency_ms = PJMEDIA_SND_DEFAULT_PLAY_LATENCY;

	return PJ_SUCCESS;
}

/* Internal: init WAVEFORMATEX */
static pj_status_t init_waveformatex(LPWAVEFORMATEX wfx, 
									 const pjmedia_aud_param *prm)
{

	pj_bzero(wfx, sizeof(WAVEFORMATEX));
	if (prm->ext_fmt.id == PJMEDIA_FORMAT_L16) {
		enum { BYTES_PER_SAMPLE = 2 };
		wfx->wFormatTag = WAVE_FORMAT_PCM; 
		wfx->nChannels = (pj_uint16_t)prm->channel_count;
		wfx->nSamplesPerSec = prm->clock_rate;
		wfx->nBlockAlign = (pj_uint16_t)(prm->channel_count * 
			BYTES_PER_SAMPLE);
		wfx->nAvgBytesPerSec = prm->clock_rate * prm->channel_count * 
			BYTES_PER_SAMPLE;
		wfx->wBitsPerSample = 16;

		return PJ_SUCCESS;

	} else if ((prm->flags & PJMEDIA_AUD_DEV_CAP_EXT_FORMAT) &&
		(prm->ext_fmt.id == PJMEDIA_FORMAT_PCMA ||
		prm->ext_fmt.id == PJMEDIA_FORMAT_PCMU))
	{
		unsigned ptime;

		ptime = prm->samples_per_frame * 1000 / 
			(prm->clock_rate * prm->channel_count);
		wfx->wFormatTag = (pj_uint16_t)
			((prm->ext_fmt.id==PJMEDIA_FORMAT_PCMA) ?
WAVE_FORMAT_ALAW : WAVE_FORMAT_MULAW);  
		wfx->nChannels = (pj_uint16_t)prm->channel_count;
		wfx->nSamplesPerSec = prm->clock_rate;
		wfx->nAvgBytesPerSec = prm->clock_rate * prm->channel_count;
		wfx->nBlockAlign = (pj_uint16_t)(wfx->nAvgBytesPerSec * ptime /
			1000);
		wfx->wBitsPerSample = 8;
		wfx->cbSize = 0;

		return PJ_SUCCESS;

	} else {

		return PJMEDIA_EAUD_BADFORMAT;

	}
}

/* Get format name */
static const char *get_fmt_name(pj_uint32_t id)
{
	static char name[8];

	if (id == PJMEDIA_FORMAT_L16)
		return "PCM";
	pj_memcpy(name, &id, 4);
	name[4] = '\0';
	return name;
}


/* API: create stream */
static pj_status_t audec_factory_create_stream(pjmedia_aud_dev_factory *f,
												const pjmedia_aud_param *param,
												pjmedia_aud_rec_cb rec_cb,
												pjmedia_aud_play_cb play_cb,
												void *user_data,
												pjmedia_aud_stream **p_aud_strm)
{
	struct audec_factory *wf = (struct audec_factory*)f;
	pj_pool_t *pool;
	struct audec_stream *strm;
	pj_uint8_t silence_char;
	pj_status_t status;

	switch (param->ext_fmt.id) {
case PJMEDIA_FORMAT_L16:
	silence_char = '\0';
	break;
case PJMEDIA_FORMAT_ALAW:
	silence_char = (pj_uint8_t)'\xd5';
	break;
case PJMEDIA_FORMAT_ULAW:
	silence_char = (pj_uint8_t)'\xff';
	break;
default:
	return PJMEDIA_EAUD_BADFORMAT;
	}

	/* Create and Initialize stream descriptor */
	pool = pj_pool_create(wf->pf, "audec-dev", 1000, 1000, NULL);
	PJ_ASSERT_RETURN(pool != NULL, PJ_ENOMEM);

	strm = PJ_POOL_ZALLOC_T(pool, struct audec_stream);
	pj_memcpy(&strm->param, param, sizeof(*param));
	strm->pool = pool;
	strm->rec_cb = rec_cb;
	strm->play_cb = play_cb;
	strm->user_data = user_data;
	strm->fmt_id = param->ext_fmt.id;
	strm->silence_char = silence_char;

	/*
	// Create player stream 
	if (param->dir & PJMEDIA_DIR_PLAYBACK) {
	unsigned buf_count;

	if ((param->flags & PJMEDIA_AUD_DEV_CAP_OUTPUT_LATENCY)==0) {
	strm->param.flags |= PJMEDIA_AUD_DEV_CAP_OUTPUT_LATENCY;
	strm->param.output_latency_ms = PJMEDIA_SND_DEFAULT_PLAY_LATENCY;
	}

	buf_count = strm->param.output_latency_ms * param->clock_rate * 
	param->channel_count / param->samples_per_frame / 1000;

	status = init_player_stream(wf, strm->pool,
	strm,
	&strm->play_strm,
	param,
	buf_count);

	if (status != PJ_SUCCESS) {
	stream_destroy(&strm->base);
	return status;
	}
	}

	// Create capture stream 
	if (param->dir & PJMEDIA_DIR_CAPTURE) {
	unsigned buf_count;

	if ((param->flags & PJMEDIA_AUD_DEV_CAP_INPUT_LATENCY)==0) {
	strm->param.flags |= PJMEDIA_AUD_DEV_CAP_INPUT_LATENCY;
	strm->param.input_latency_ms = PJMEDIA_SND_DEFAULT_REC_LATENCY;
	}

	buf_count = strm->param.input_latency_ms * param->clock_rate * 
	param->channel_count / param->samples_per_frame / 1000;

	status = init_capture_stream(wf, strm->pool,
	strm,
	&strm->rec_strm,
	param,
	buf_count);

	if (status != PJ_SUCCESS) {
	stream_destroy(&strm->base);
	return status;
	}
	}*/

	strm->buffer = pj_pool_alloc(pool, strm->bytes_per_frame);
	if (!strm->buffer) {
		pj_pool_release(pool);
		return PJ_ENOMEM;
	}

	/* If format is extended, must create buffer for the extended frame. */
	if (strm->fmt_id != PJMEDIA_FORMAT_L16) {
		strm->xfrm_size = sizeof(pjmedia_frame_ext) + 
			32 * sizeof(pjmedia_frame_ext_subframe) +
			strm->bytes_per_frame + 4;
		strm->xfrm = (pjmedia_frame_ext*)
			pj_pool_alloc(pool, strm->xfrm_size);
	}

	/* Create the stop event */
	strm->thread_quit_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (strm->thread_quit_event == NULL) {
		status = pj_get_os_error();
		stream_destroy(&strm->base);
		return status;
	}

	// Create and start the thread 
	/*
	status = pj_thread_create(pool, "wmme", &audec_dev_thread, strm, 0, 0, 
	&strm->thread);
	if (status != PJ_SUCCESS) {
	stream_destroy(&strm->base);
	return status;
	}*/

	/* Apply the remaining settings */
	if (param->flags & PJMEDIA_AUD_DEV_CAP_OUTPUT_VOLUME_SETTING) {
		stream_set_cap(&strm->base, PJMEDIA_AUD_DEV_CAP_OUTPUT_VOLUME_SETTING,
			&param->output_vol);
	}


	/* Done */
	strm->base.op = &stream_op;
	*p_aud_strm = &strm->base;

	m_gaudecStream = (struct audec_stream*)&strm->base;

	strm->rec_thread_exited = 1;
	strm->play_thread_exited = 1;

	InitializeCriticalSection(&m_AudecAudioCritSec);
	return PJ_SUCCESS;
}

/* API: Get stream info. */
static pj_status_t stream_get_param(pjmedia_aud_stream *s,
									pjmedia_aud_param *pi)
{
	struct audec_stream *strm = (struct audec_stream*)s;

	PJ_ASSERT_RETURN(strm && pi, PJ_EINVAL);

	pj_memcpy(pi, &strm->param, sizeof(*pi));

	/* Update the volume setting */
	if (stream_get_cap(s, PJMEDIA_AUD_DEV_CAP_OUTPUT_VOLUME_SETTING,
		&pi->output_vol) == PJ_SUCCESS)
	{
		pi->flags |= PJMEDIA_AUD_DEV_CAP_OUTPUT_VOLUME_SETTING;
	}

	return PJ_SUCCESS;
}

/* API: get capability */
static pj_status_t stream_get_cap(pjmedia_aud_stream *s,
								  pjmedia_aud_dev_cap cap,
								  void *pval)
{
	struct audec_stream *strm = (struct audec_stream*)s;

	PJ_ASSERT_RETURN(s && pval, PJ_EINVAL);

	if (cap==PJMEDIA_AUD_DEV_CAP_INPUT_LATENCY && 
		(strm->param.dir & PJMEDIA_DIR_CAPTURE)) 
	{
		/* Recording latency */
		*(unsigned*)pval = strm->param.input_latency_ms;
		return PJ_SUCCESS;
	} else if (cap==PJMEDIA_AUD_DEV_CAP_OUTPUT_LATENCY  && 
		(strm->param.dir & PJMEDIA_DIR_PLAYBACK))
	{
		/* Playback latency */
		*(unsigned*)pval = strm->param.output_latency_ms;
		return PJ_SUCCESS;
	} else if (cap==PJMEDIA_AUD_DEV_CAP_OUTPUT_VOLUME_SETTING &&
		strm->play_strm.hWave.Out)
	{
		/* Output volume setting */
		DWORD waveVol;
		MMRESULT mr;

		mr = waveOutGetVolume(strm->play_strm.hWave.Out, &waveVol);
		if (mr != MMSYSERR_NOERROR) {
			return PJMEDIA_AUDIODEV_ERRNO_FROM_WMME_OUT(mr);
		}

		waveVol &= 0xFFFF;
		*(unsigned*)pval = (waveVol * 100) / 0xFFFF;
		return PJ_SUCCESS;
	} else {
		return PJMEDIA_EAUD_INVCAP;
	}
}

/* API: set capability */
static pj_status_t stream_set_cap(pjmedia_aud_stream *s,
								  pjmedia_aud_dev_cap cap,
								  const void *pval)
{
	struct audec_stream *strm = (struct audec_stream*)s;

	PJ_ASSERT_RETURN(s && pval, PJ_EINVAL);

	if (cap==PJMEDIA_AUD_DEV_CAP_OUTPUT_VOLUME_SETTING &&
		strm->play_strm.hWave.Out)
	{
		/* Output volume setting */
		unsigned vol = *(unsigned*)pval;
		DWORD waveVol;
		MMRESULT mr;
		pj_status_t status;

		if (vol > 100)
			vol = 100;

		waveVol = (vol * 0xFFFF) / 100;
		waveVol |= (waveVol << 16);

		mr = waveOutSetVolume(strm->play_strm.hWave.Out, waveVol);
		status = (mr==MMSYSERR_NOERROR)? PJ_SUCCESS : 
			PJMEDIA_AUDIODEV_ERRNO_FROM_WMME_OUT(mr);
		if (status == PJ_SUCCESS) {
			strm->param.output_vol = *(unsigned*)pval;
		}
		return status;
	}

	return PJMEDIA_EAUD_INVCAP;
}

/* API: Start stream. */
static pj_status_t stream_start(pjmedia_aud_stream *strm)
{
	//struct audec_stream *stream = (struct audec_stream*)strm;
	
	play_timestamp.u64 = 0;
	capture_timestamp.u64 = 0;

	return PJ_SUCCESS;
}

/* API: Stop stream. */
static pj_status_t stream_stop(pjmedia_aud_stream *strm)
{
	int i;
	struct audec_stream *stream = (struct audec_stream*)strm;
	PJ_ASSERT_RETURN(stream != NULL, PJ_EINVAL);

	stream->quit_flag = 1;
	for (i=0; !stream->rec_thread_exited && i<100; ++i)
		pj_thread_sleep(10);
	for (i=0; !stream->play_thread_exited && i<100; ++i)
		pj_thread_sleep(10);

	pj_thread_sleep(1);

	play_timestamp.u64 = 0;
	capture_timestamp.u64 = 0;

	stream->play_thread_initialized = 0;
    stream->rec_thread_initialized = 0;

	PJ_LOG(5,(THIS_FILE, "Stopping stream.."));

	return PJ_SUCCESS;
}


/* API: Destroy stream. */
static pj_status_t stream_destroy(pjmedia_aud_stream *strm)
{
	struct audec_stream *stream = (struct audec_stream*)strm;
	unsigned i;

	PJ_ASSERT_RETURN(stream != NULL, PJ_EINVAL);

	stream->quit_flag = 1;
	for (i=0; !stream->rec_thread_exited && i<100; ++i) {
		pj_thread_sleep(1);
	}
	for (i=0; !stream->play_thread_exited && i<100; ++i) {
		pj_thread_sleep(1);
	}
	pj_pool_release(stream->pool);

	m_gaudecStream = NULL;  
	DeleteCriticalSection(&m_AudecAudioCritSec);

	return PJ_SUCCESS;
}
;

void DeviceCaptureCallBack(short * data, unsigned int len)
{
	pjmedia_frame pcm_frame, *frame;
	struct audec_stream *strm = (struct audec_stream*)m_gaudecStream;
	if(strm == NULL)
	{
		if(data != NULL)
			memset(data, 0, len*sizeof(short));
		return;
	}
	if (strm->quit_flag)
		goto on_break;

	//PJ_LOG(5,(THIS_FILE, "DeviceCaptureCallBack------ in!!!!!!!!!!!!!!"));

	if (strm->rec_thread_initialized == 0 || !pj_thread_is_registered()) 
	{
		pj_bzero(strm->rec_thread_desc, sizeof(pj_thread_desc));
		pj_thread_register("audec_rec", strm->rec_thread_desc, 
			&strm->rec_thread);
		strm->rec_thread_initialized = 1;
		PJ_LOG(5,(THIS_FILE, "Recorder thread started"));
		strm->rec_thread_exited = 1;
	}

	if(strm != NULL)
	{
		frame = &pcm_frame;
		frame->type = PJMEDIA_FRAME_TYPE_AUDIO;
		frame->size = len*sizeof(short);
		frame->buf = data;
		frame->timestamp.u64 = capture_timestamp.u64;
		frame->bit_info = 0;
		capture_timestamp.u64 += strm->param.samples_per_frame;

		EnterCriticalSection(&m_AudecAudioCritSec);
		(*strm->rec_cb)(strm->user_data, frame);
		LeaveCriticalSection(&m_AudecAudioCritSec);
	}
	return;

on_break:
	strm->rec_thread_exited = 1;
	return;
}

void DevicePlayCallBack(unsigned char * data, unsigned int len)
{
	pjmedia_frame pcm_frame, *frame;
	struct audec_stream *strm = (struct audec_stream*)m_gaudecStream;
	if(strm == NULL)
	{
		if(data != NULL)
			memset(data, 0, len*sizeof(short));
		return;
	}
	if (strm->quit_flag)
		goto on_break;

	if (strm->play_thread_initialized == 0 || !pj_thread_is_registered()) 
	{
		pj_bzero(strm->play_thread_desc, sizeof(pj_thread_desc));
		pj_thread_register("audec_play", strm->play_thread_desc,
			&strm->play_thread);
		strm->play_thread_initialized = 1;
		PJ_LOG(5,(THIS_FILE, "Player thread started"));		
		strm->play_thread_exited = 0;
	}

	if(strm != NULL)
	{
		frame = &pcm_frame;
		frame->type = PJMEDIA_FRAME_TYPE_AUDIO;
		frame->size = len*sizeof(short);
		frame->buf = data;
		frame->timestamp.u64 = play_timestamp.u64;
		frame->bit_info = 0;
		play_timestamp.u64 += strm->param.samples_per_frame;

		EnterCriticalSection(&m_AudecAudioCritSec);
		(*strm->play_cb)(strm->user_data, frame);
		LeaveCriticalSection(&m_AudecAudioCritSec);
	}
	return;

on_break:
	strm->play_thread_exited = 1;
	return;
}
