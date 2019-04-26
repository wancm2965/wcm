# Microsoft Developer Studio Project File - Name="h323stack" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=h323stack - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "h323stack.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "h323stack.mak" CFG="h323stack - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "h323stack - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "h323stack - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "h323stack - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\..\binaries\windows\release\bin"
# PROP Intermediate_Dir ".\..\binaries\windows\release\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "H323STACK_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /Ob2 /I "cm" /I "asn_objects" /I "..\h245" /I "..\asn1\pvt_pst" /I "ras" /I "..\asn1\coder" /I "transport" /I "..\asn1\utils" /I "..\asn1\config" /I "cat" /I "mib" /I "middle" /I "../common/ares" /I "../common/ccore/netdrivers" /I "../common/cbase" /I "../common/ccore" /I "../common/config" /I "../common/config/arch" /I "../common/config/os" /I "../common/config/tool" /I "../common/cutils" /I "../common/ccore/memdrivers" /I "../common/cbase/netdrivers" /I "../common/adapters" /I "../common/adapters/windows" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "RV_RELEASE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /map /machine:I386 /out:".\..\binaries\windows\release\bin/rv32h323.dll"

!ELSEIF  "$(CFG)" == "h323stack - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\..\binaries\windows\debug\bin"
# PROP Intermediate_Dir ".\..\binaries\windows\debug\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "H323STACK_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "../common/adapters" /I "../common/adapters/windows" /I "cm" /I "asn_objects" /I "..\h245" /I "..\asn1\pvt_pst" /I "ras" /I "..\asn1\coder" /I "transport" /I "..\asn1\utils" /I "..\asn1\config" /I "cat" /I "mib" /I "middle" /I "../common/ares" /I "../common/ccore/netdrivers" /I "../common/cbase" /I "../common/ccore" /I "../common/config" /I "../common/config/arch" /I "../common/config/os" /I "../common/config/tool" /I "../common/cutils" /I "../common/ccore/memdrivers" /I "../common/cbase/netdrivers" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "RV_DEBUG" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /debug /machine:I386 /out:".\..\binaries\windows\debug\bin/rv32h323.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "h323stack - Win32 Release"
# Name "h323stack - Win32 Debug"
# Begin Group "asn_objects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\asn_objects\commonasn.c
# End Source File
# Begin Source File

SOURCE=.\asn_objects\commonasn.h
# End Source File
# Begin Source File

SOURCE=.\asn_objects\q931asn1.c
# End Source File
# Begin Source File

SOURCE=.\asn_objects\q931asn1.h
# End Source File
# End Group
# Begin Group "cm"

# PROP Default_Filter ""
# Begin Group "autoras"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cm\cmAutoRas.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmAutoRas.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmAutoRasAltGK.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmAutoRasAltGK.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmAutoRasCall.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmAutoRasCall.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmAutoRasEP.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmAutoRasEP.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmAutoRasIntr.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmrasinit.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmrasinit.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\cm\Cm.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmaltep.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmaltep.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmCall.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmCall.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmcallcid.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmcallcid.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmCallHighLevel.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmCallMessages.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmCallMPoint.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmcallobj.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmcallobj.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmcfgrt.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmcfgrt.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmConfig.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmConfig.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmControlTransport.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmCrossReference.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmCrossReference.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmdebprn.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmdebprn.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmDns.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmDns.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmGeneral.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmhook.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmhook.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmHosts.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmiAddOns.h
# End Source File
# Begin Source File

SOURCE=.\cm\Cmintr.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmiQ931.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmnew.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmparam.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmparam.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmPreservation.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmpreservation.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmQ931.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmQ931.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmsize.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmsize.h
# End Source File
# Begin Source File

SOURCE=.\cm\cmutils.c
# End Source File
# Begin Source File

SOURCE=.\cm\cmutils.h
# End Source File
# Begin Source File

SOURCE=.\cm\h323Version.h
# End Source File
# Begin Source File

SOURCE=.\cm\stkutils.c
# End Source File
# Begin Source File

SOURCE=.\cm\stkutils.h
# End Source File
# End Group
# Begin Group "cat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cat\cat.c
# End Source File
# Begin Source File

SOURCE=.\cat\cat.h
# End Source File
# Begin Source File

SOURCE=.\cat\catdb.h
# End Source File
# End Group
# Begin Group "mib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mib\cmmib.c
# End Source File
# Begin Source File

SOURCE=.\mib\cmmib.h
# End Source File
# Begin Source File

SOURCE=.\mib\rvh323mibstats.c
# End Source File
# Begin Source File

SOURCE=.\mib\rvh323mibstats.h
# End Source File
# End Group
# Begin Group "middle"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\middle\mti.c
# End Source File
# Begin Source File

SOURCE=.\middle\mti.h
# End Source File
# Begin Source File

SOURCE=.\middle\rv_defs.h
# End Source File
# Begin Source File

SOURCE=.\middle\rvcommon.h
# End Source File
# Begin Source File

SOURCE=.\middle\rvh323connection.h
# End Source File
# Begin Source File

SOURCE=.\middle\rvh323timer.c
# End Source File
# Begin Source File

SOURCE=.\middle\rvh323timer.h
# End Source File
# Begin Source File

SOURCE=.\middle\rvh323watchdog.c
# End Source File
# Begin Source File

SOURCE=.\middle\rvh323watchdog.h
# End Source File
# Begin Source File

SOURCE=.\middle\rvinternal.h
# End Source File
# Begin Source File

SOURCE=.\middle\rvthreadinstance.c
# End Source File
# Begin Source File

SOURCE=.\middle\rvthreadinstance.h
# End Source File
# Begin Source File

SOURCE=.\middle\seli.c
# End Source File
# Begin Source File

SOURCE=.\middle\seli.h
# End Source File
# End Group
# Begin Group "transport"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\transport\annexe.c
# End Source File
# Begin Source File

SOURCE=.\transport\annexe.h
# End Source File
# Begin Source File

SOURCE=.\transport\iannerw.c
# End Source File
# Begin Source File

SOURCE=.\transport\ianneses.c
# End Source File
# Begin Source File

SOURCE=.\transport\iannetmr.c
# End Source File
# Begin Source File

SOURCE=.\transport\iannexe.c
# End Source File
# Begin Source File

SOURCE=.\transport\iannexe.h
# End Source File
# Begin Source File

SOURCE=.\transport\llist.h
# End Source File
# Begin Source File

SOURCE=.\transport\tpkt.c
# End Source File
# Begin Source File

SOURCE=.\transport\tpkt.h
# End Source File
# Begin Source File

SOURCE=.\transport\transnet.c
# End Source File
# Begin Source File

SOURCE=.\transport\transnet.h
# End Source File
# Begin Source File

SOURCE=.\transport\transparent.c
# End Source File
# Begin Source File

SOURCE=.\transport\transparent.h
# End Source File
# Begin Source File

SOURCE=.\transport\transport.c
# End Source File
# Begin Source File

SOURCE=.\transport\transport.h
# End Source File
# Begin Source File

SOURCE=.\transport\transportint.h
# End Source File
# Begin Source File

SOURCE=.\transport\transStates.c
# End Source File
# Begin Source File

SOURCE=.\transport\transStates.h
# End Source File
# Begin Source File

SOURCE=.\transport\transutil.c
# End Source File
# Begin Source File

SOURCE=.\transport\transUtil.h
# End Source File
# End Group
# Begin Group "ras"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ras\cmiras.c
# End Source File
# Begin Source File

SOURCE=.\ras\cmiras.h
# End Source File
# Begin Source File

SOURCE=.\ras\cmras.c
# End Source File
# Begin Source File

SOURCE=.\ras\cmras.h
# End Source File
# Begin Source File

SOURCE=.\ras\rasdecoder.c
# End Source File
# Begin Source File

SOURCE=.\ras\rasdecoder.h
# End Source File
# Begin Source File

SOURCE=.\ras\rasdef.h
# End Source File
# Begin Source File

SOURCE=.\ras\rasin.c
# End Source File
# Begin Source File

SOURCE=.\ras\rasin.h
# End Source File
# Begin Source File

SOURCE=.\ras\rasirr.c
# End Source File
# Begin Source File

SOURCE=.\ras\rasirr.h
# End Source File
# Begin Source File

SOURCE=.\ras\rasout.c
# End Source File
# Begin Source File

SOURCE=.\ras\rasout.h
# End Source File
# Begin Source File

SOURCE=.\ras\rasparams.c
# End Source File
# Begin Source File

SOURCE=.\ras\rasparams.h
# End Source File
# Begin Source File

SOURCE=.\ras\rasutils.c
# End Source File
# Begin Source File

SOURCE=.\ras\rasutils.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\cm\cmH323Version.rc
# End Source File
# End Target
# End Project


