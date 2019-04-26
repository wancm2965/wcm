#*********************************************************************************
#*                              <Include.mak>                                                              
#* 
#* Fill the ProxyAPI dir with application API header files.
#*
#*    Author                         Date
#*    ------                        ------
#*    Michael Zak                   May 2005                                                                                                
#*********************************************************************************


# Module directories - must have relative path from this makefile

ROOT_DIR = .\..\..
INCLUDE_DIR = $(ROOT_DIR)\include
ARTP_DIR=rtpRtcp\artp
SRTP_DIR=rtpRtcp\srtp
RTCPXR_DIR=rtpRtcp\rtcpxr
RTCPFB_DIR=rtpRtcp\rtcpfb

ALL : RTPAPI SRTPAPI RTCPXRAPI RTCPFBAPI


#*********************************************************************************
#*                           R T P   A P I 
#*********************************************************************************	
!IF exist($(ROOT_DIR)\$(ARTP_DIR))

RTP_FILES = \
	"$(ROOT_DIR)\$(ARTP_DIR)\rtp.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rtcp.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\payload.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvnetaddress.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpconfig.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpdoublekey.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpencryptiondata.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpencryptionkeyplugin.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpencryptionmode.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpencryptionplugin.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpheader.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpinterface.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtplogger.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpseli.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvkey.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpnatfw.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpstunfw.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rvrtpbuffer.h"\
	"$(ROOT_DIR)\$(ARTP_DIR)\rtcpbasetypes.h"


RTPAPI: RTPSTART $(RTP_FILES)

RTPSTART: 
	@-mkdir $(INCLUDE_DIR)\rtpRtcp
	@-mkdir $(INCLUDE_DIR)\$(ARTP_DIR)
	@echo --------------------- Updating RTP API... ---------------------

$(RTP_FILES): PHONEY
	@copy $@ $(INCLUDE_DIR)\$(ARTP_DIR) > NULL

!ELSE

RTPAPI:

!ENDIF
#*********************************************************************************
#*                           S R T P   A P I 
#*********************************************************************************	
# We only process this part if we have an SRTP

!IF exist($(ROOT_DIR)\$(SRTP_DIR))
	  
SRTPAPI : SRTPSTART "rv_srtp.h" "rvsrtpaesplugin.h" "rvsrtpconfig.h" "rvsrtpsha1.h"

SRTPSTART: 
	@echo --------------------- Updating SRTP API... ---------------------
	@-mkdir $(INCLUDE_DIR)\$(SRTP_DIR)

rv_srtp.h:   $(ROOT_DIR)\$(SRTP_DIR)\rvsrtp\rv_srtp.h
		 @copy $(ROOT_DIR)\$(SRTP_DIR)\rvsrtp\rv_srtp.h $(INCLUDE_DIR)\$(SRTP_DIR)\rv_srtp.h > NULL

rvsrtpaesplugin.h:   $(ROOT_DIR)\$(SRTP_DIR)\rvsrtp\rvsrtpaesplugin.h
		 @copy $(ROOT_DIR)\$(SRTP_DIR)\rvsrtp\rvsrtpaesplugin.h $(INCLUDE_DIR)\$(SRTP_DIR)\rvsrtpaesplugin.h > NULL

rvsrtpconfig.h:   $(ROOT_DIR)\$(SRTP_DIR)\rvsrtp\rvsrtpconfig.h
		 @copy $(ROOT_DIR)\$(SRTP_DIR)\rvsrtp\rvsrtpconfig.h $(INCLUDE_DIR)\$(SRTP_DIR)\rvsrtpconfig.h > NULL

rvsrtpsha1.h:	$(ROOT_DIR)\$(SRTP_DIR)\rvsrtp\rvsrtpsha1.h
		 @copy $(ROOT_DIR)\$(SRTP_DIR)\rvsrtp\rvsrtpsha1.h $(INCLUDE_DIR)\$(SRTP_DIR)\rvsrtpsha1.h > NULL
!ELSE

SRTPAPI :

!ENDIF

#*********************************************************************************
#*                           R T C P     X R      A P I 
#*********************************************************************************	
# We only process this part if we have an RTCP XR add-on

!IF exist($(ROOT_DIR)\$(RTCPXR_DIR))
	  
RTCPXRAPI : XRSTART "rvrtcpxr.h" "rtcpxrplugin.h"

XRSTART: 
	@echo --------------------- Updating RTCP XR API... ---------------------

rvrtcpxr.h:   $(ROOT_DIR)\rtpRtcp\rtcpxr\rvrtcpxr.h
		 @copy $(ROOT_DIR)\rtpRtcp\rtcpxr\rvrtcpxr.h $(INCLUDE_DIR)\$(ARTP_DIR)\rvrtcpxr.h > NULL

rtcpxrplugin.h:   $(ROOT_DIR)\rtpRtcp\rtcpxr\rtcpxrplugin.h
		 @copy $(ROOT_DIR)\rtpRtcp\rtcpxr\rtcpxrplugin.h $(INCLUDE_DIR)\$(ARTP_DIR)\rtcpxrplugin.h > NULL

!ELSE

RTCPXRAPI :

!ENDIF




#*********************************************************************************
#*                           R T C P     F B      A P I 
#*********************************************************************************	
# We only process this part if we have an RTCP FB add-on
	  
!IF exist($(ROOT_DIR)\$(RTCPFB_DIR))

RTCPFBAPI : FBSTART "rvrtcpfb.h" "rtcpfbplugin.h"

FBSTART: 
	@echo --------------------- Updating RTCP FB API... ---------------------

rvrtcpfb.h:   $(ROOT_DIR)\rtpRtcp\rtcpfb\rvrtcpfb.h
		 @copy $(ROOT_DIR)\rtpRtcp\rtcpfb\rvrtcpfb.h $(INCLUDE_DIR)\$(ARTP_DIR)\rvrtcpfb.h > NULL

rtcpfbplugin.h:   $(ROOT_DIR)\rtpRtcp\rtcpfb\rtcpfbplugin.h
		 @copy $(ROOT_DIR)\rtpRtcp\rtcpfb\rtcpfbplugin.h $(INCLUDE_DIR)\$(ARTP_DIR)\rtcpfbplugin.h > NULL

!ELSE

RTCPFBAPI :

!ENDIF


PHONEY:

