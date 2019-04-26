# Main dependency list
ALL : ASN1DEFS H450ASN1DEFS INCLUDE




###################################################################################################
#
# ASN.1 Part
#
###################################################################################################
EXEDIR=..\asn1\compiler
ASN=$(EXEDIR)\asn.exe

# We only process this part if we have an ASN.1 compiler
!IF exist($(EXEDIR))

ASN1DEFS : "asn_objects\commonasn.c" "asn_objects\commonasn.h" "asn_objects\q931asn1.c" "asn_objects\q931asn1.h" H245_FULL H245_LEAN

asn_objects\commonasn.c : asn_messages\systemConfig.asn asn_messages\h450config.asn "$(ASN)"
   $(ASN) asn_messages\systemConfig.asn asn_messages\h450config.asn -o asn_objects\commonasn -p _common

!IF exist(..\h323addons\fwnat)

asn_objects\q931asn1.c asn_objects\q931asn1.h : asn_messages\q931asn1.asn asn_messages\q931Config.asn asn_messages\h225asn1.asn asn_messages\rasConfig.asn asn_messages\h235asn1.asn asn_messages\cap.asn asn_messages\Properties.asn ..\h323addons\fwnat\h460fwnatasn.asn "$(ASN)"
   $(ASN) asn_messages\q931asn1.asn asn_messages\q931Config.asn asn_messages\h225asn1.asn asn_messages\rasConfig.asn asn_messages\h235asn1.asn asn_messages\cap.asn asn_messages\Properties.asn ..\h323addons\fwnat\h460fwnatasn.asn -p _q931 -o asn_objects\q931asn1

!ELSE

asn_objects\q931asn1.c asn_objects\q931asn1.h : asn_messages\q931asn1.asn asn_messages\q931Config.asn asn_messages\h225asn1.asn asn_messages\rasConfig.asn asn_messages\h235asn1.asn asn_messages\cap.asn asn_messages\Properties.asn "$(ASN)"
   $(ASN) asn_messages\q931asn1.asn asn_messages\q931Config.asn asn_messages\h225asn1.asn asn_messages\rasConfig.asn asn_messages\h235asn1.asn asn_messages\cap.asn asn_messages\Properties.asn -p _q931 -o asn_objects\q931asn1

!ENDIF


H245_FULL : ..\h245\h245_full.c ..\h245\h245_full.h

H245_LEAN : ..\h245\h245_lean.c ..\h245\h245_lean.h

..\h245\h245_full.c ..\h245\h245_full.h : ..\h245\h245v13.asn ..\h245\h245Config.asn "$(ASN)"
   $(ASN) ..\h245\h245v13.asn ..\h245\h245Config.asn -p _h245 -o ..\h245\h245_full -d RV_H245_LEAN_H223==RV_NO

..\h245\h245_lean.c ..\h245\h245_lean.h : ..\h245\h245v13-lean.asn ..\h245\h245Config.asn "$(ASN)"
   $(ASN) ..\h245\h245v13-lean.asn ..\h245\h245Config.asn -p _h245 -o ..\h245\h245_lean -d RV_H245_LEAN_H223==RV_YES


!ELSE

ASN1DEFS :

!ENDIF





###################################################################################################
#
# H.450 ASN.1 Part
#
###################################################################################################

H450ASNDIR=..\h323addons\h450script
H450OUTDIR=..\h323addons\supserve

# We only process this part if we have an ASN.1 compiler and an H.450 add-on package
!IF exist($(EXEDIR))
!IF exist($(H450ASNDIR))

H450ASN1DEFS : "$(H450OUTDIR)\h450asn.c" "$(H450OUTDIR)\h450asn.h"

H450GENERAL=$(H450ASNDIR)\q931asn1-small.asn $(H450ASNDIR)\Remote-Operations-Information-Objects.asn $(H450ASNDIR)\H4501-General-Error-List.asn $(H450ASNDIR)\Addressing-Data-Elements.asn $(H450ASNDIR)\H225-Generic-Parameters-Definition.asn $(H450ASNDIR)\Manufacturer-specific-service-extension-definition.asn $(H450ASNDIR)\Remote-Operations-Apdus.asn $(H450ASNDIR)\H4501-Supplementary-ServiceAPDU-Structure.asn

H4502=$(H450ASNDIR)\Call-Transfer-Operations.asn
H4503=$(H450ASNDIR)\Call-Diversion-Operations.asn
H4504=$(H450ASNDIR)\Call-Hold-Operations.asn
H4505=$(H450ASNDIR)\Call-Park-Pickup-Operations.asn
H4506=$(H450ASNDIR)\Call-Waiting-Operations.asn
H4507=$(H450ASNDIR)\Message-Waiting-Indication-Operations.asn
H4508=$(H450ASNDIR)\Name-Identification-Operations.asn
H4509=$(H450ASNDIR)\Call-Completion-Operations.asn
H45010=$(H450ASNDIR)\Call-Offer-Operation.asn
H45011=$(H450ASNDIR)\Call-Intrusion-Operations.asn
H45012=$(H450ASNDIR)\Common-Information-Operations.asn

H450SERVICES=$(H4502) $(H4503) $(H4504) $(H4505) $(H4506) $(H4507) $(H4508) $(H4509) $(H45010) $(H45011) $(H45012)


$(H450OUTDIR)\h450asn.c $(H450OUTDIR)\h450asn.h : $(H450GENERAL) $(H450SERVICES) "$(ASN)"
   $(ASN) $(GENERAL) $(SERVICES) -p _h450 -o $(H450OUTDIR)\h450asn -t h450asn $(H450GENERAL) $(H450SERVICES)

!ELSE

H450ASN1DEFS :

!ENDIF

!ELSE

H450ASN1DEFS :

!ENDIF






###################################################################################################
#
# Include Directory Part
#
###################################################################################################

# Directories
COMMONDIR=..\common
ASN1DIR=..\asn1
H245DIR=..\h245
CMDIR=.
ADDONSDIR=..\h323addons
INCDIR=..\include\.
COMMONINC=$(INCDIR)\common\.
ASN1INC=$(INCDIR)\asn1\.
H245INC=$(INCDIR)\h245\.
H323INC=$(INCDIR)\h323\.
RTPINC=$(INCDIR)\rtp\.

# Make sure include directory is created if necessary
INCLUDE_DIRS : INCLUDE_DIR INCLUDE_COMMON_DIR INCLUDE_ASN1_DIR INCLUDE_H245_DIR INCLUDE_H323_DIR INCLUDE_RTP_DIR

!IF exist($(INCDIR))
INCLUDE_DIR :
!ELSE
INCLUDE_DIR :
    mkdir $(INCDIR) > NUL
!ENDIF

!IF exist($(COMMONINC))
INCLUDE_COMMON_DIR :
!ELSE
INCLUDE_COMMON_DIR :
    mkdir $(COMMONINC) > NUL
!ENDIF

!IF exist($(ASN1INC))
INCLUDE_ASN1_DIR :
!ELSE
INCLUDE_ASN1_DIR :
    mkdir $(ASN1INC) > NUL
!ENDIF

!IF exist($(H245INC))
INCLUDE_H245_DIR :
!ELSE
INCLUDE_H245_DIR :
    mkdir $(H245INC) > NUL
!ENDIF

!IF exist($(H323INC))
INCLUDE_H323_DIR :
!ELSE
INCLUDE_H323_DIR :
    mkdir $(H323INC) > NUL
!ENDIF

!IF exist($(RTPINC))
INCLUDE_RTP_DIR :
!ELSE
INCLUDE_RTP_DIR :
    mkdir $(RTPINC) > NUL
!ENDIF



!IF DEFINED(UNDER_CE)
OS_INCLUDE = $(COMMONINC)\rvemvc.h $(COMMONINC)\rvwince.h $(COMMONINC)\rvmsvc.h
!ELSE
OS_INCLUDE = $(COMMONINC)\rvmsvc.h $(COMMONINC)\rvwin32.h
!ENDIF

INCLUDE : INCLUDE_DIRS COMMONCORE ASN1 H245 CM ADDONS

# Common Core related header files.
COMMONCORE : $(COMMONINC)\rvtypes.h $(COMMONINC)\rv64bits.h $(COMMONINC)\rverror.h $(COMMONINC)\rvconfig.h $(COMMONINC)\rvcflags.h $(COMMONINC)\rvdependencies.h $(COMMONINC)\rvusrconfig.h $(COMMONINC)\rvinterfacesdefs.h $(COMMONINC)\rvarchconfig.h $(COMMONINC)\rvarchdefs.h $(COMMONINC)\rvosconfig.h $(COMMONINC)\rvosdefs.h $(COMMONINC)\rvtoolconfig.h $(COMMONINC)\rvtooldefs.h $(COMMONINC)\rvccoredefs.h $(COMMONINC)\rvccoreconfig.h $(OS_INCLUDE)

$(COMMONINC)\rvtypes.h : $(COMMONDIR)\cutils\rvtypes.h
   copy $(COMMONDIR)\cutils\rvtypes.h $(COMMONINC) > NUL

$(COMMONINC)\rv64bits.h : $(COMMONDIR)\cutils\rv64bits.h
   copy $(COMMONDIR)\cutils\rv64bits.h $(COMMONINC) > NUL

$(COMMONINC)\rverror.h : $(COMMONDIR)\cutils\rverror.h
   copy $(COMMONDIR)\cutils\rverror.h $(COMMONINC) > NUL

$(COMMONINC)\rvconfig.h : $(COMMONDIR)\config\rvconfig.h
   copy $(COMMONDIR)\config\rvconfig.h $(COMMONINC) > NUL

$(COMMONINC)\rvcflags.h : $(COMMONDIR)\config\rvcflags.h
   copy $(COMMONDIR)\config\rvcflags.h $(COMMONINC) > NUL

$(COMMONINC)\rvdependencies.h : $(COMMONDIR)\config\rvdependencies.h
   copy $(COMMONDIR)\config\rvdependencies.h $(COMMONINC) > NUL

$(COMMONINC)\rvusrconfig.h : $(COMMONDIR)\config\rvusrconfig.h
   copy $(COMMONDIR)\config\rvusrconfig.h $(COMMONINC) > NUL

$(COMMONINC)\rvinterfacesdefs.h : $(COMMONDIR)\config\rvinterfacesdefs.h
   copy $(COMMONDIR)\config\rvinterfacesdefs.h $(COMMONINC) > NUL

$(COMMONINC)\rvarchconfig.h : $(COMMONDIR)\config\arch\rvarchconfig.h
   copy $(COMMONDIR)\config\arch\rvarchconfig.h $(COMMONINC) > NUL

$(COMMONINC)\rvarchdefs.h : $(COMMONDIR)\config\arch\rvarchdefs.h
   copy $(COMMONDIR)\config\arch\rvarchdefs.h $(COMMONINC) > NUL

$(COMMONINC)\rvosconfig.h : $(COMMONDIR)\config\os\rvosconfig.h
   copy $(COMMONDIR)\config\os\rvosconfig.h $(COMMONINC) > NUL

$(COMMONINC)\rvosdefs.h : $(COMMONDIR)\config\os\rvosdefs.h
   copy $(COMMONDIR)\config\os\rvosdefs.h $(COMMONINC) > NUL

$(COMMONINC)\rvwin32.h : $(COMMONDIR)\config\os\rvwin32.h
   copy $(COMMONDIR)\config\os\rvwin32.h $(COMMONINC) > NUL

$(COMMONINC)\rvwince.h : $(COMMONDIR)\config\os\rvwince.h
   copy $(COMMONDIR)\config\os\rvwince.h $(COMMONINC) > NUL

$(COMMONINC)\rvtoolconfig.h : $(COMMONDIR)\config\tool\rvtoolconfig.h
   copy $(COMMONDIR)\config\tool\rvtoolconfig.h $(COMMONINC) > NUL

$(COMMONINC)\rvtooldefs.h : $(COMMONDIR)\config\tool\rvtooldefs.h
   copy $(COMMONDIR)\config\tool\rvtooldefs.h $(COMMONINC) > NUL

$(COMMONINC)\rvccoredefs.h : $(COMMONDIR)\ccore\rvccoredefs.h
   copy $(COMMONDIR)\ccore\rvccoredefs.h $(COMMONINC) > NUL

$(COMMONINC)\rvccoreconfig.h : $(COMMONDIR)\ccore\rvccoreconfig.h
   copy $(COMMONDIR)\ccore\rvccoreconfig.h $(COMMONINC) > NUL

$(COMMONINC)\rvmsvc.h : $(COMMONDIR)\config\tool\rvmsvc.h
   copy $(COMMONDIR)\config\tool\rvmsvc.h $(COMMONINC) > NUL

$(COMMONINC)\rvemvc.h : $(COMMONDIR)\config\tool\rvemvc.h
   copy $(COMMONDIR)\config\tool\rvemvc.h $(COMMONINC) > NUL



# ASN.1 related header files.
ASN1 : $(ASN1INC)\coder.h $(ASN1INC)\pvaltree.h $(ASN1INC)\psyntree.h $(ASN1INC)\ci.h $(ASN1INC)\cibuf.h $(ASN1INC)\msg.h

$(ASN1INC)\coder.h : $(ASN1DIR)\coder\coder.h
   copy $(ASN1DIR)\coder\coder.h $(ASN1INC) > NUL

$(ASN1INC)\pvaltree.h : $(ASN1DIR)\pvt_pst\pvaltree.h
   copy $(ASN1DIR)\pvt_pst\pvaltree.h $(ASN1INC) > NUL

$(ASN1INC)\psyntree.h : $(ASN1DIR)\pvt_pst\psyntree.h
   copy $(ASN1DIR)\pvt_pst\psyntree.h $(ASN1INC) > NUL

$(ASN1INC)\ci.h : $(ASN1DIR)\config\ci.h
   copy $(ASN1DIR)\config\ci.h $(ASN1INC) > NUL

$(ASN1INC)\cibuf.h : $(ASN1DIR)\config\cibuf.h
   copy $(ASN1DIR)\config\cibuf.h $(ASN1INC) > NUL

$(ASN1INC)\msg.h : $(ASN1DIR)\utils\msg.h
   copy $(ASN1DIR)\utils\msg.h $(ASN1INC) > NUL


# H.245 related header files.
H245 : $(H245INC)\cmctrl.h $(H245INC)\cmH245GeneralDefs.h $(H245INC)\faststart.h $(H245INC)\transpcap.h $(H245INC)\redencod.h $(H245INC)\userinput.h $(H245INC)\h245.h $(H245INC)\h245_lean.h $(H245INC)\h245_full.h

$(H245INC)\cmctrl.h : $(H245DIR)\cmctrl.h
   copy $(H245DIR)\cmctrl.h $(H245INC) > NUL

$(H245INC)\cmH245GeneralDefs.h : $(H245DIR)\cmH245GeneralDefs.h
   copy $(H245DIR)\cmH245GeneralDefs.h $(H245INC) > NUL

$(H245INC)\faststart.h : $(H245DIR)\faststart.h
   copy $(H245DIR)\faststart.h $(H245INC) > NUL

$(H245INC)\transpcap.h : $(H245DIR)\transpcap.h
   copy $(H245DIR)\transpcap.h $(H245INC) > NUL

$(H245INC)\redencod.h : $(H245DIR)\redencod.h
   copy $(H245DIR)\redencod.h $(H245INC) > NUL

$(H245INC)\userinput.h : $(H245DIR)\userinput.h
   copy $(H245DIR)\userinput.h $(H245INC) > NUL

$(H245INC)\h245.h : $(H245DIR)\h245.h
   copy $(H245DIR)\h245.h $(H245INC) > NUL

$(H245INC)\h245_lean.h : $(H245DIR)\h245_lean.h
   copy $(H245DIR)\h245_lean.h $(H245INC) > NUL

$(H245INC)\h245_full.h : $(H245DIR)\h245_full.h
   copy $(H245DIR)\h245_full.h $(H245INC) > NUL


# CM related header files.
# All of these files are needed for the $(INCDIR) of cm.h

CM : $(H323INC)\cm.h $(H323INC)\cmsize.h $(H323INC)\cmhook.h $(H323INC)\cmpreservation.h $(H323INC)\cmras.h $(H323INC)\rvcommon.h $(H323INC)\rv_defs.h $(H323INC)\seli.h $(H323INC)\rvh323watchdog.h $(H323INC)\mti.h $(H323INC)\stkutils.h $(H323INC)\q931asn1.h $(H323INC)\rvh323userconfig.h


$(H323INC)\cm.h : $(CMDIR)\cm\cm.h
   copy $(CMDIR)\cm\cm.h $(H323INC) > NUL

$(H323INC)\cmsize.h : $(CMDIR)\cm\cmsize.h
   copy $(CMDIR)\cm\cmsize.h $(H323INC) > NUL

$(H323INC)\cmhook.h : $(CMDIR)\cm\cmhook.h
   copy $(CMDIR)\cm\cmhook.h $(H323INC) > NUL

$(H323INC)\cmpreservation.h : $(CMDIR)\cm\cmpreservation.h
   copy $(CMDIR)\cm\cmpreservation.h $(H323INC) > NUL

$(H323INC)\cmcfgrt.h : $(CMDIR)\cm\cmcfgrt.h
   copy $(CMDIR)\cm\cmcfgrt.h $(H323INC) > NUL

$(H323INC)\rvh323userconfig.h : $(CMDIR)\cm\rvh323userconfig.h
   copy $(CMDIR)\cm\rvh323userconfig.h $(H323INC) > NUL

$(H323INC)\cmras.h : $(CMDIR)\ras\cmras.h
   copy $(CMDIR)\ras\cmras.h $(H323INC) > NUL

$(H323INC)\rvcommon.h : $(CMDIR)\middle\rvcommon.h
   copy $(CMDIR)\middle\rvcommon.h $(H323INC) > NUL

$(H323INC)\rv_defs.h : $(CMDIR)\middle\rv_defs.h
   copy $(CMDIR)\middle\rv_defs.h $(H323INC) > NUL

$(H323INC)\seli.h : $(CMDIR)\middle\seli.h
   copy $(CMDIR)\middle\seli.h $(H323INC) > NUL

$(H323INC)\rvh323watchdog.h : $(CMDIR)\middle\rvh323watchdog.h
   copy $(CMDIR)\middle\rvh323watchdog.h $(H323INC) > NUL

$(H323INC)\mti.h : $(CMDIR)\middle\mti.h
   copy $(CMDIR)\middle\mti.h $(H323INC) > NUL

$(H323INC)\stkutils.h : $(CMDIR)\cm\stkutils.h
   copy $(CMDIR)\cm\stkutils.h $(H323INC) > NUL

$(H323INC)\q931asn1.h : $(CMDIR)\asn_objects\q931asn1.h
   copy $(CMDIR)\asn_objects\q931asn1.h $(H323INC) > NUL



# ADDONS header files
# This part is processed only if the addons can be found in the stack's directory
ADDONS : RTP MIB SUPSERVE SSE H235 LDAP FWNAT OSP GEF


# RTP Addon header files
!IF exist(..\rtpRtcp\rtp)

!IF exist(..\rtpRtcp\rtp\rvrtpconfig.h)
# also copy new RTP files
RTP : $(RTPINC)\payload.h $(RTPINC)\rtcp.h $(RTPINC)\rtp.h $(RTPINC)\rvrtpconfig.h $(RTPINC)\rvrtpinterface.h $(RTPINC)\rvnetaddress.h $(RTPINC)\rvrtpheader.h $(RTPINC)\rvkey.h $(RTPINC)\rvrtpencryptionmode.h $(RTPINC)\rvrtpencryptionplugin.h $(RTPINC)\rvrtpencryptionkeyplugin.h $(RTPINC)\rvrtpencryptiondata.h $(RTPINC)\rvrtpnatfw.h
!ELSE
RTP : $(RTPINC)\payload.h $(RTPINC)\rtcp.h $(RTPINC)\rtp.h
!ENDIF

$(RTPINC)\payload.h : ..\rtpRtcp\rtp\payload.h
   copy ..\rtpRtcp\rtp\payload.h $(RTPINC) > NUL

$(RTPINC)\rtcp.h : ..\rtpRtcp\rtp\rtcp.h
   copy ..\rtpRtcp\rtp\rtcp.h $(RTPINC) > NUL

$(RTPINC)\rtp.h : ..\rtpRtcp\rtp\rtp.h
   copy ..\rtpRtcp\rtp\rtp.h $(RTPINC) > NUL

!IF exist(..\rtpRtcp\rtp\rvrtpconfig.h)
# also copy new RTP files

$(RTPINC)\rvrtpconfig.h : ..\rtpRtcp\rtp\rvrtpconfig.h
   copy ..\rtpRtcp\rtp\rvrtpconfig.h $(RTPINC) > NUL

$(RTPINC)\rvrtpinterface.h : ..\rtpRtcp\rtp\rvrtpinterface.h
   copy ..\rtpRtcp\rtp\rvrtpinterface.h $(RTPINC) > NUL

$(RTPINC)\rvnetaddress.h : ..\rtpRtcp\rtp\rvnetaddress.h
   copy ..\rtpRtcp\rtp\rvnetaddress.h $(RTPINC) > NUL

$(RTPINC)\rvrtpheader.h : ..\rtpRtcp\rtp\rvrtpheader.h
   copy ..\rtpRtcp\rtp\rvrtpheader.h $(RTPINC) > NUL

$(RTPINC)\rvkey.h : ..\rtpRtcp\rtp\rvkey.h
   copy ..\rtpRtcp\rtp\rvkey.h $(RTPINC) > NUL

$(RTPINC)\rvrtpencryptionmode.h : ..\rtpRtcp\rtp\rvrtpencryptionmode.h
   copy ..\rtpRtcp\rtp\rvrtpencryptionmode.h $(RTPINC) > NUL

$(RTPINC)\rvrtpencryptionplugin.h : ..\rtpRtcp\rtp\rvrtpencryptionplugin.h
   copy ..\rtpRtcp\rtp\rvrtpencryptionplugin.h $(RTPINC) > NUL

$(RTPINC)\rvrtpencryptionkeyplugin.h : ..\rtpRtcp\rtp\rvrtpencryptionkeyplugin.h
   copy ..\rtpRtcp\rtp\rvrtpencryptionkeyplugin.h $(RTPINC) > NUL

$(RTPINC)\rvrtpencryptiondata.h : ..\rtpRtcp\rtp\rvrtpencryptiondata.h
   copy ..\rtpRtcp\rtp\rvrtpencryptiondata.h $(RTPINC) > NUL

$(RTPINC)\rvrtpnatfw.h : ..\rtpRtcp\rtp\rvrtpnatfw.h
   copy ..\rtpRtcp\rtp\rvrtpnatfw.h $(RTPINC) > NUL

!ENDIF

!ELSE

RTP :

!ENDIF


# MIB Addon header files
!IF exist(snmp)

MIB : $(H323INC)\rvh341agent.h $(H323INC)\h341common.h $(H323INC)\snmpI.h $(H323INC)\applSnmpI.h

$(H323INC)\rvh341agent.h : snmp\rvh341agent.h
   copy snmp\rvh341agent.h $(H323INC) > NUL

$(H323INC)\h341common.h : snmp\h341common.h
   copy snmp\h341common.h $(H323INC) > NUL

$(H323INC)\snmpI.h : snmp\snmpI.h
   copy snmp\snmpI.h $(H323INC) > NUL

$(H323INC)\applSnmpI.h : applsnmp\applSnmpI.h
   copy applsnmp\applSnmpI.h $(H323INC) > NUL

!ELSE

MIB :

!ENDIF


# SUPS Addon header files
!IF exist($(ADDONSDIR)\supserve)

SUPSERVE : $(H323INC)\sssize.h $(H323INC)\h450.h $(H323INC)\h450asn.h

$(H323INC)\sssize.h : $(ADDONSDIR)\supserve\sssize.h
   copy $(ADDONSDIR)\supserve\sssize.h $(H323INC) > NUL

$(H323INC)\h450.h : $(ADDONSDIR)\supserve\h450.h
   copy $(ADDONSDIR)\supserve\h450.h $(H323INC) > NUL

$(H323INC)\h450asn.h : $(ADDONSDIR)\supserve\h450asn.h
   copy $(ADDONSDIR)\supserve\h450asn.h $(H323INC) > NUL

!ELSE

SUPSERVE :

!ENDIF


# SSE Addon header files
!IF exist($(ADDONSDIR)\sse)

SSE : $(H323INC)\sse.h

$(H323INC)\sse.h : $(ADDONSDIR)\sse\sse.h
   copy $(ADDONSDIR)\sse\sse.h $(H323INC) > NUL

!ELSE

SSE :

!ENDIF


# H235 Addon header files
!IF exist($(ADDONSDIR)\h235)

H235 : $(H323INC)\h235_api.h

$(H323INC)\h235_api.h : $(ADDONSDIR)\h235\h235_api.h
   copy $(ADDONSDIR)\h235\h235_api.h $(H323INC) > NUL

!ELSE

H235 :

!ENDIF


# LDAP Addon header files
!IF exist($(ADDONSDIR)\rvh323ldap)

LDAP : $(H323INC)\rvh323ldap.h

$(H323INC)\rvh323ldap.h : $(ADDONSDIR)\rvh323ldap\rvh323ldap.h
   copy $(ADDONSDIR)\rvh323ldap\rvh323ldap.h $(H323INC) > NUL

!ELSE

LDAP :

!ENDIF


# FW/NAT Addon header files
!IF exist($(ADDONSDIR)\fwnat)

FWNAT : $(H323INC)\fwnat_api.h

$(H323INC)\fwnat_api.h : $(ADDONSDIR)\fwnat\fwnat_api.h
   copy $(ADDONSDIR)\fwnat\fwnat_api.h $(H323INC) > NUL

!ELSE

FWNAT :

!ENDIF


# OSP Addon header files
!IF exist($(ADDONSDIR)\osp)

OSP : $(H323INC)\rvosp.h

$(H323INC)\rvosp.h : $(ADDONSDIR)\osp\rvosp.h
   copy $(ADDONSDIR)\osp\rvosp.h $(H323INC) > NUL

!ELSE

OSP :

!ENDIF


# GEF Addon header files
!IF exist($(ADDONSDIR)\gef)

GEF : $(H323INC)\RvGef.h $(H323INC)\RvGefGeneral.h $(H323INC)\RvGefAmr.h $(H323INC)\RvGefG7221.h $(H323INC)\RvGefMpeg4.h $(H323INC)\RvGefH264.h $(H323INC)\RvGefH239.h

$(H323INC)\RvGef.h : $(ADDONSDIR)\gef\RvGef.h
   copy $(ADDONSDIR)\gef\RvGef.h $(H323INC) > NUL

$(H323INC)\RvGefGeneral.h : $(ADDONSDIR)\gef\RvGefGeneral.h
   copy $(ADDONSDIR)\gef\RvGefGeneral.h $(H323INC) > NUL

$(H323INC)\RvGefAmr.h : $(ADDONSDIR)\gef\RvGefAmr.h
   copy $(ADDONSDIR)\gef\RvGefAmr.h $(H323INC) > NUL

$(H323INC)\RvGefG7221.h : $(ADDONSDIR)\gef\RvGefG7221.h
   copy $(ADDONSDIR)\gef\RvGefG7221.h $(H323INC) > NUL

$(H323INC)\RvGefMpeg4.h : $(ADDONSDIR)\gef\RvGefMpeg4.h
   copy $(ADDONSDIR)\gef\RvGefMpeg4.h $(H323INC) > NUL

$(H323INC)\RvGefH264.h : $(ADDONSDIR)\gef\RvGefH264.h
   copy $(ADDONSDIR)\gef\RvGefH264.h $(H323INC) > NUL

$(H323INC)\RvGefH239.h : $(ADDONSDIR)\gef\RvGefH239.h
   copy $(ADDONSDIR)\gef\RvGefH239.h $(H323INC) > NUL

!ELSE

GEF :

!ENDIF
