#ifndef AVCONEC60_AFX_EXT_CLASS 
#ifdef AVCONEC60_EXT
#define AVCONEC60_AFX_EXT_CLASS __declspec(dllexport)
#else
#define AVCONEC60_AFX_EXT_CLASS __declspec(dllimport)
#endif
#endif

#include "AVCONEC60/AUDEC_Engine.h"
#include "AVCONEC60/AUDEC_Export.h"
#include "AVCONEC60/AUDEC_MicCtrl.h"
#include "AVCONEC60/AUDEC_Packet.h"
#include "AVCONEC60/AUDEC_SpeakerCtrl.h"
#include "AVCONEC60/AVC_Packet.h"
#include "AVCONEC60/AVCONEC.h"
#include "AVCONEC60/AVCONECC.h"
#include "AVCONEC60/AVCONECPC.h"
#include "AVCONEC60/AVWaveMixerMic.h"
#include "AVCONEC60/AVWaveMixerSpeaker.h"
#include "AVCONEC60/IHPPicture.h"
#include "AVCONEC60/IXRGBCapture.h"
#include "AVCONEC60/IXSSECapture.h"
#include "AVCONEC60/libAVCONEC.h"
#include "AVCONEC60/libXMediaFileCapture.h"
#include "AVCONEC60/VIDEC_Decoder.h"
#include "AVCONEC60/VIDEC_Wnd.h"