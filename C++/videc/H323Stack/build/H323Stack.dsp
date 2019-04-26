# Microsoft Developer Studio Project File - Name="H323Stack" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=H323Stack - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "H323Stack.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "H323Stack.mak" CFG="H323Stack - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "H323Stack - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "H323Stack - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "H323Stack - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../obj/Release/H323Stack"
# PROP Intermediate_Dir "../../../obj/Release/H323Stack"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../common/ares" /I "../common/cbase" /I "../common/ccore" /I "../common/ccore/netdrivers" /I "../common/ccore/memdrivers" /I "../common/cutils" /I "../common/config" /I "../common/config/arch" /I "../common/config/os" /I "../common/config/tool" /I "../asn1/coder" /I "../asn1/config" /I "../asn1/pvt_pst" /I "../asn1/utils" /I "../h323/middle" /I "../h323/cm" /I "../h323/ras" /I "../h323/asn_objects" /I "../h323/cat" /I "../h323/transport" /I "../h323/mib" /I "../h245" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib/Release/H323Stack.lib"

!ELSEIF  "$(CFG)" == "H323Stack - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../obj/Debug/H323Stack"
# PROP Intermediate_Dir "../../../obj/Debug/H323Stack"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../common/ares" /I "../common/cbase" /I "../common/ccore" /I "../common/ccore/netdrivers" /I "../common/ccore/memdrivers" /I "../common/cutils" /I "../common/config" /I "../common/config/arch" /I "../common/config/os" /I "../common/config/tool" /I "../asn1/coder" /I "../asn1/config" /I "../asn1/pvt_pst" /I "../asn1/utils" /I "../h323/middle" /I "../h323/cm" /I "../h323/ras" /I "../h323/asn_objects" /I "../h323/cat" /I "../h323/transport" /I "../h323/mib" /I "../h245" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /FR /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib/Debug/H323Stack.lib"

!ENDIF 

# Begin Target

# Name "H323Stack - Win32 Release"
# Name "H323Stack - Win32 Debug"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Group "ares"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\ares\ares__read_line.c
# End Source File
# Begin Source File

SOURCE=..\common\ares\ares_expand_name.c
# End Source File
# Begin Source File

SOURCE=..\common\ares\ares_init.c
# End Source File
# Begin Source File

SOURCE=..\common\ares\ares_process.c
# End Source File
# Begin Source File

SOURCE=..\common\ares\ares_query.c
# End Source File
# Begin Source File

SOURCE=..\common\ares\rvares.c
# End Source File
# Begin Source File

SOURCE=..\common\ares\rvoscomp.c
# End Source File
# End Group
# Begin Group "cbase"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\cbase\rvcbase.c
# End Source File
# Begin Source File

SOURCE=..\common\cbase\rvqueue.c
# End Source File
# Begin Source File

SOURCE=..\common\cbase\rvtimer.c
# End Source File
# Begin Source File

SOURCE=..\common\cbase\rvtimerengine.c
# End Source File
# End Group
# Begin Group "ccore"

# PROP Default_Filter ""
# Begin Group "memdrivers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\ccore\memdrivers\rvosmem.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\memdrivers\rvpoolmem.c
# End Source File
# End Group
# Begin Group "netdrivers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\ccore\netdrivers\rvhost.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\netdrivers\rvportrange.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\netdrivers\rvselect.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\netdrivers\rvsocket.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\netdrivers\rvtls.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\common\ccore\rv64ascii.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvccore.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvclock.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvlock.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvlog.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvloglistener.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvmemory.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvmutex.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvsemaphore.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvstdio.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvthread.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvtimestamp.c
# End Source File
# Begin Source File

SOURCE=..\common\ccore\rvtm.c
# End Source File
# End Group
# Begin Group "cutils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\cutils\rv64bits.c
# End Source File
# Begin Source File

SOURCE=..\common\cutils\rvaddress.c
# End Source File
# Begin Source File

SOURCE=..\common\cutils\rvansi.c
# End Source File
# Begin Source File

SOURCE=..\common\cutils\rvema.c
# End Source File
# Begin Source File

SOURCE=..\common\cutils\rvnet2host.c
# End Source File
# Begin Source File

SOURCE=..\common\cutils\rvntptime.c
# End Source File
# Begin Source File

SOURCE=..\common\cutils\rvobjlist.c
# End Source File
# Begin Source File

SOURCE=..\common\cutils\rvobjpool.c
# End Source File
# Begin Source File

SOURCE=..\common\cutils\rvpqueue.c
# End Source File
# Begin Source File

SOURCE=..\common\cutils\rvra.c
# End Source File
# Begin Source File

SOURCE=..\common\cutils\rvrandomgenerator.c
# End Source File
# Begin Source File

SOURCE=..\common\cutils\rvtime.c
# End Source File
# End Group
# End Group
# Begin Group "h245"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h245\caputils.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmchan.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmChanGet.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmChanOperations.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmConf.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmctrl.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmCtrlCap.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmCtrlMES.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmCtrlMMode.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmCtrlMpoint.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmCtrlMSD.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmCtrlRMode.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmCtrlRTD.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmH223Params.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmH225Params.c
# End Source File
# Begin Source File

SOURCE=..\h245\cmH245.c
# End Source File
# Begin Source File

SOURCE=..\h245\faststart.c
# End Source File
# Begin Source File

SOURCE=..\h245\h245.c
# End Source File
# Begin Source File

SOURCE=..\h245\redencod.c
# End Source File
# Begin Source File

SOURCE=..\h245\transpcap.c
# End Source File
# Begin Source File

SOURCE=..\h245\userinput.c
# End Source File
# End Group
# Begin Group "asn1"

# PROP Default_Filter ""
# Begin Group "coder"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\asn1\coder\coder.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\coderbitbuffer.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\emanag.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\per.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\perBitString.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\perchoic.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\perchrst.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\peroctst.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\peroid.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\perseq.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\perseqof.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\persimpl.c
# End Source File
# Begin Source File

SOURCE=..\asn1\coder\q931.c
# End Source File
# End Group
# Begin Group "config"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\asn1\config\ci.c
# End Source File
# Begin Source File

SOURCE=..\asn1\config\cibuf.c
# End Source File
# Begin Source File

SOURCE=..\asn1\config\cifile.c
# End Source File
# Begin Source File

SOURCE=..\asn1\config\cireg.c
# End Source File
# Begin Source File

SOURCE=..\asn1\config\cisupp.c
# End Source File
# Begin Source File

SOURCE=..\asn1\config\civirtfile.c
# End Source File
# End Group
# Begin Group "pvt_pst"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\asn1\pvt_pst\psyntree.c
# End Source File
# Begin Source File

SOURCE=..\asn1\pvt_pst\psyntreeDb.c
# End Source File
# Begin Source File

SOURCE=..\asn1\pvt_pst\psyntreeStackApi.c
# End Source File
# Begin Source File

SOURCE=..\asn1\pvt_pst\pvaltree.c
# End Source File
# Begin Source File

SOURCE=..\asn1\pvt_pst\pvaltreeStackApi.c
# End Source File
# End Group
# Begin Group "utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\asn1\utils\copybits.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\ema.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\h32xLog.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\hash.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\intutils.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\mlist.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\msg.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\oidutils.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\prnutils.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\ra.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\rpool.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\rtree.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\rvwatchdog.c
# End Source File
# Begin Source File

SOURCE=..\asn1\utils\strutils.c
# End Source File
# End Group
# End Group
# Begin Group "h323"

# PROP Default_Filter ""
# Begin Group "asn_objects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h323\asn_objects\commonasn.c
# End Source File
# Begin Source File

SOURCE=..\h323\asn_objects\q931asn1.c
# End Source File
# End Group
# Begin Group "cat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h323\cat\cat.c
# End Source File
# End Group
# Begin Group "cm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h323\cm\cmAutoRas.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmAutoRasAltGK.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmAutoRasCall.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmAutoRasEP.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmCall.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmcallcid.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmCallHighLevel.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmCallMessages.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmCallMPoint.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmcallobj.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmcfgrt.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmConfig.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmControlTransport.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmCrossReference.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmdebprn.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmDns.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmGeneral.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmhook.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmHosts.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmnew.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmparam.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmpreservation.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmQ931.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmrasinit.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmsize.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\cmutils.c
# End Source File
# Begin Source File

SOURCE=..\h323\cm\stkutils.c
# End Source File
# End Group
# Begin Group "mib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h323\mib\cmmib.c
# End Source File
# Begin Source File

SOURCE=..\h323\mib\rvh323mibstats.c
# End Source File
# End Group
# Begin Group "middle"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h323\middle\mti.c
# End Source File
# Begin Source File

SOURCE=..\h323\middle\rvh323timer.c
# End Source File
# Begin Source File

SOURCE=..\h323\middle\rvh323watchdog.c
# End Source File
# Begin Source File

SOURCE=..\h323\middle\rvthreadinstance.c
# End Source File
# Begin Source File

SOURCE=..\h323\middle\seli.c
# End Source File
# End Group
# Begin Group "ras"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h323\ras\cmiras.c
# End Source File
# Begin Source File

SOURCE=..\h323\ras\cmras.c
# End Source File
# Begin Source File

SOURCE=..\h323\ras\rasdecoder.c
# End Source File
# Begin Source File

SOURCE=..\h323\ras\rasin.c
# End Source File
# Begin Source File

SOURCE=..\h323\ras\rasirr.c
# End Source File
# Begin Source File

SOURCE=..\h323\ras\rasout.c
# End Source File
# Begin Source File

SOURCE=..\h323\ras\rasparams.c
# End Source File
# Begin Source File

SOURCE=..\h323\ras\rasutils.c
# End Source File
# End Group
# Begin Group "transport"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h323\transport\annexe.c
# End Source File
# Begin Source File

SOURCE=..\h323\transport\iannerw.c
# End Source File
# Begin Source File

SOURCE=..\h323\transport\ianneses.c
# End Source File
# Begin Source File

SOURCE=..\h323\transport\iannetmr.c
# End Source File
# Begin Source File

SOURCE=..\h323\transport\iannexe.c
# End Source File
# Begin Source File

SOURCE=..\h323\transport\tpkt.c
# End Source File
# Begin Source File

SOURCE=..\h323\transport\transnet.c
# End Source File
# Begin Source File

SOURCE=..\h323\transport\transparent.c
# End Source File
# Begin Source File

SOURCE=..\h323\transport\transport.c
# End Source File
# Begin Source File

SOURCE=..\h323\transport\transStates.c
# End Source File
# Begin Source File

SOURCE=..\h323\transport\transutil.c
# End Source File
# End Group
# End Group
# Begin Group "rtpRtcp"

# PROP Default_Filter ""
# Begin Group "rtp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\rtpRtcp\rtp\bitfield.c
# End Source File
# Begin Source File

SOURCE=..\rtpRtcp\rtp\payload.c
# End Source File
# Begin Source File

SOURCE=..\rtpRtcp\rtp\rtcp.c
# End Source File
# Begin Source File

SOURCE=..\rtpRtcp\rtp\rtp.c
# End Source File
# Begin Source File

SOURCE=..\rtpRtcp\rtp\rtputil.c
# End Source File
# Begin Source File

SOURCE=..\rtpRtcp\rtp\rvrtpbuffer.c
# End Source File
# End Group
# End Group
# End Target
# End Project
