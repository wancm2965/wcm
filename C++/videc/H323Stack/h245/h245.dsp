# Microsoft Developer Studio Project File - Name="h245" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=h245 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "h245.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "h245.mak" CFG="h245 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "h245 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "h245 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "h245 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\..\binaries\windows\release\lib"
# PROP Intermediate_Dir ".\..\binaries\windows\release\obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /Ob2 /I "." /I "..\include\h323" /I "..\h323\mib" /I "..\h323\transport" /I "..\h323\ras" /I "..\h323\cat" /I "..\h323\middle" /I "..\h323\cm" /I "..\asn1\pvt_pst" /I "..\asn1\coder" /I "..\asn1\config" /I "..\asn1\utils" /I "../common/ares" /I "../common/ccore/netdrivers" /I "../common/cbase" /I "../common/ccore" /I "../common/config" /I "../common/config/arch" /I "../common/config/os" /I "../common/config/tool" /I "../common/cutils" /I "../common/ccore/memdrivers" /I "../common/adapters" /I "../common/adapters/windows" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "RV_RELEASE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "h245 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\..\binaries\windows\debug\lib"
# PROP Intermediate_Dir ".\..\binaries\windows\debug\obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "../common/adapters" /I "../common/adapters/windows" /I "." /I "..\include\h323" /I "..\h323\mib" /I "..\h323\transport" /I "..\h323\ras" /I "..\h323\cat" /I "..\h323\middle" /I "..\h323\cm" /I "..\asn1\pvt_pst" /I "..\asn1\coder" /I "..\asn1\config" /I "..\asn1\utils" /I "../common/ares" /I "../common/ccore/netdrivers" /I "../common/cbase" /I "../common/ccore" /I "../common/config" /I "../common/config/arch" /I "../common/config/os" /I "../common/config/tool" /I "../common/cutils" /I "../common/ccore/memdrivers" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "RV_DEBUG" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "h245 - Win32 Release"
# Name "h245 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Fast Start"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\faststart.c
# End Source File
# End Group
# Begin Group "Channels"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\cmchan.c
# End Source File
# Begin Source File

SOURCE=.\cmChanGet.c
# End Source File
# Begin Source File

SOURCE=.\cmChanOperations.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\caputils.c
# End Source File
# Begin Source File

SOURCE=.\cmConf.c
# End Source File
# Begin Source File

SOURCE=.\cmctrl.c
# End Source File
# Begin Source File

SOURCE=.\cmCtrlCap.c
# End Source File
# Begin Source File

SOURCE=.\cmCtrlMES.c
# End Source File
# Begin Source File

SOURCE=.\cmCtrlMMode.c
# End Source File
# Begin Source File

SOURCE=.\cmCtrlMpoint.c
# End Source File
# Begin Source File

SOURCE=.\cmCtrlMSD.c
# End Source File
# Begin Source File

SOURCE=.\cmCtrlRMode.c
# End Source File
# Begin Source File

SOURCE=.\cmCtrlRTD.c
# End Source File
# Begin Source File

SOURCE=.\cmH223Params.c
# End Source File
# Begin Source File

SOURCE=.\cmH225Params.c
# End Source File
# Begin Source File

SOURCE=.\cmH245.c
# End Source File
# Begin Source File

SOURCE=.\h245_full.c
# End Source File
# Begin Source File

SOURCE=.\h245_lean.c
# End Source File
# Begin Source File

SOURCE=.\redencod.c
# End Source File
# Begin Source File

SOURCE=.\transpcap.c
# End Source File
# Begin Source File

SOURCE=.\userinput.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\caputils.h
# End Source File
# Begin Source File

SOURCE=.\cmchan.h
# End Source File
# Begin Source File

SOURCE=.\cmChanGet.h
# End Source File
# Begin Source File

SOURCE=.\cmChanOperations.h
# End Source File
# Begin Source File

SOURCE=.\cmConf.h
# End Source File
# Begin Source File

SOURCE=.\cmControl.h
# End Source File
# Begin Source File

SOURCE=.\cmctrl.h
# End Source File
# Begin Source File

SOURCE=.\cmCtrlCap.h
# End Source File
# Begin Source File

SOURCE=.\cmCtrlMES.h
# End Source File
# Begin Source File

SOURCE=.\cmCtrlMMode.h
# End Source File
# Begin Source File

SOURCE=.\cmCtrlMpoint.h
# End Source File
# Begin Source File

SOURCE=.\cmCtrlMSD.h
# End Source File
# Begin Source File

SOURCE=.\cmCtrlRMode.h
# End Source File
# Begin Source File

SOURCE=.\cmCtrlRTD.h
# End Source File
# Begin Source File

SOURCE=.\cmH223Params.h
# End Source File
# Begin Source File

SOURCE=.\cmH225Params.h
# End Source File
# Begin Source File

SOURCE=.\cmH245.h
# End Source File
# Begin Source File

SOURCE=.\cmH245GeneralDefs.h
# End Source File
# Begin Source File

SOURCE=.\cmH245Mib.h
# End Source File
# Begin Source File

SOURCE=.\cmH245Object.h
# End Source File
# Begin Source File

SOURCE=.\cmictrl.h
# End Source File
# Begin Source File

SOURCE=.\cmiFastStart.h
# End Source File
# Begin Source File

SOURCE=.\cmiParams.h
# End Source File
# Begin Source File

SOURCE=.\faststart.h
# End Source File
# Begin Source File

SOURCE=.\h245_full.h
# End Source File
# Begin Source File

SOURCE=.\h245_lean.h
# End Source File
# Begin Source File

SOURCE=.\redencod.h
# End Source File
# Begin Source File

SOURCE=.\transpcap.h
# End Source File
# Begin Source File

SOURCE=.\userinput.h
# End Source File
# End Group
# End Target
# End Project


