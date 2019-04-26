# Microsoft Developer Studio Project File - Name="asn1" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=asn1 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "asn1.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "asn1.mak" CFG="asn1 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "asn1 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "asn1 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "asn1 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\..\binaries\windows\release\bin"
# PROP Intermediate_Dir ".\..\binaries\windows\release\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ASN1_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /Ob2 /I "../common/ccore/netdrivers" /I "../common/cbase" /I "../common/ccore" /I "../common/config" /I "../common/config/arch" /I "../common/config/os" /I "../common/config/tool" /I "../common/cutils" /I "../common/ccore/memdrivers" /I "../common/cbase/netdrivers" /I "." /I "coder" /I "config" /I "pvt_pst" /I "utils" /I "../h323/cm" /I "../include/h323" /I "../common/adapters" /I "../common/adapters/windows" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "RV_RELEASE" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /machine:I386 /out:".\..\binaries\windows\release\bin/rvasn1.dll"

!ELSEIF  "$(CFG)" == "asn1 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\..\binaries\windows\debug\bin"
# PROP Intermediate_Dir ".\..\binaries\windows\debug\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ASN1_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "../common/adapters" /I "../common/adapters/windows" /I "../common/ccore/netdrivers" /I "../common/cbase" /I "../common/ccore" /I "../common/config" /I "../common/config/arch" /I "../common/config/os" /I "../common/config/tool" /I "../common/cutils" /I "../common/ccore/memdrivers" /I "../common/cbase/netdrivers" /I "." /I "coder" /I "config" /I "pvt_pst" /I "utils" /I "../h323/cm" /I "../include/h323" /D "_WINDOWS" /D "RV_DEBUG" /D "WIN32" /D "_DEBUG" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:".\..\binaries\windows\debug\bin/rvasn1.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "asn1 - Win32 Release"
# Name "asn1 - Win32 Debug"
# Begin Group "coder"

# PROP Default_Filter ""
# Begin Group "Q931"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\coder\q931.c
# End Source File
# Begin Source File

SOURCE=.\coder\q931.h
# End Source File
# End Group
# Begin Group "emanag"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\coder\coderbitbuffer.c
# End Source File
# Begin Source File

SOURCE=.\coder\coderbitbuffer.h
# End Source File
# Begin Source File

SOURCE=.\coder\emanag.c
# End Source File
# Begin Source File

SOURCE=.\coder\emanag.h
# End Source File
# End Group
# Begin Group "Per"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\coder\per.c
# End Source File
# Begin Source File

SOURCE=.\coder\per.h
# End Source File
# Begin Source File

SOURCE=.\coder\perBitString.c
# End Source File
# Begin Source File

SOURCE=.\coder\perBitString.h
# End Source File
# Begin Source File

SOURCE=.\coder\perchoic.c
# End Source File
# Begin Source File

SOURCE=.\coder\perchoic.h
# End Source File
# Begin Source File

SOURCE=.\coder\perchrst.c
# End Source File
# Begin Source File

SOURCE=.\coder\perchrst.h
# End Source File
# Begin Source File

SOURCE=.\coder\perintr.h
# End Source File
# Begin Source File

SOURCE=.\coder\peroctst.c
# End Source File
# Begin Source File

SOURCE=.\coder\peroctst.h
# End Source File
# Begin Source File

SOURCE=.\coder\peroid.c
# End Source File
# Begin Source File

SOURCE=.\coder\peroid.h
# End Source File
# Begin Source File

SOURCE=.\coder\perseq.c
# End Source File
# Begin Source File

SOURCE=.\coder\perseq.h
# End Source File
# Begin Source File

SOURCE=.\coder\perseqof.c
# End Source File
# Begin Source File

SOURCE=.\coder\perseqof.h
# End Source File
# Begin Source File

SOURCE=.\coder\persimpl.c
# End Source File
# Begin Source File

SOURCE=.\coder\persimpl.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\coder\coder.c
# End Source File
# Begin Source File

SOURCE=.\coder\Coder.h
# End Source File
# End Group
# Begin Group "config"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\config\ci.c
# End Source File
# Begin Source File

SOURCE=.\config\ci.h
# End Source File
# Begin Source File

SOURCE=.\config\cibuf.c
# End Source File
# Begin Source File

SOURCE=.\config\cibuf.h
# End Source File
# Begin Source File

SOURCE=.\config\cifile.c
# End Source File
# Begin Source File

SOURCE=.\config\cireg.c
# End Source File
# Begin Source File

SOURCE=.\config\cisupp.c
# End Source File
# Begin Source File

SOURCE=.\config\cisupp.h
# End Source File
# Begin Source File

SOURCE=.\config\civirtfile.c
# End Source File
# End Group
# Begin Group "pvt_pst"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pvt_pst\psyntree.c
# End Source File
# Begin Source File

SOURCE=.\pvt_pst\psyntree.h
# End Source File
# Begin Source File

SOURCE=.\pvt_pst\psyntreeDb.c
# End Source File
# Begin Source File

SOURCE=.\pvt_pst\psyntreeDb.h
# End Source File
# Begin Source File

SOURCE=.\pvt_pst\psyntreeDef.h
# End Source File
# Begin Source File

SOURCE=.\pvt_pst\psyntreeStackApi.c
# End Source File
# Begin Source File

SOURCE=.\pvt_pst\psyntreeStackApi.h
# End Source File
# Begin Source File

SOURCE=.\pvt_pst\pvaltree.c
# End Source File
# Begin Source File

SOURCE=.\pvt_pst\pvaltree.h
# End Source File
# Begin Source File

SOURCE=.\pvt_pst\pvaltreeDef.h
# End Source File
# Begin Source File

SOURCE=.\pvt_pst\pvaltreeStackApi.c
# End Source File
# Begin Source File

SOURCE=.\pvt_pst\pvaltreeStackApi.h
# End Source File
# End Group
# Begin Group "utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\utils\copybits.c
# End Source File
# Begin Source File

SOURCE=.\utils\copybits.h
# End Source File
# Begin Source File

SOURCE=.\utils\ema.c
# End Source File
# Begin Source File

SOURCE=.\utils\ema.h
# End Source File
# Begin Source File

SOURCE=.\utils\h32xLog.c
# End Source File
# Begin Source File

SOURCE=.\utils\h32xLog.h
# End Source File
# Begin Source File

SOURCE=.\utils\hash.c
# End Source File
# Begin Source File

SOURCE=.\utils\hash.h
# End Source File
# Begin Source File

SOURCE=.\utils\intutils.c
# End Source File
# Begin Source File

SOURCE=.\utils\intutils.h
# End Source File
# Begin Source File

SOURCE=.\utils\mlist.c
# End Source File
# Begin Source File

SOURCE=.\utils\mlist.h
# End Source File
# Begin Source File

SOURCE=.\utils\msg.c
# End Source File
# Begin Source File

SOURCE=.\utils\msg.h
# End Source File
# Begin Source File

SOURCE=.\utils\oidutils.c
# End Source File
# Begin Source File

SOURCE=.\utils\oidutils.h
# End Source File
# Begin Source File

SOURCE=.\utils\prnutils.c
# End Source File
# Begin Source File

SOURCE=.\utils\prnutils.h
# End Source File
# Begin Source File

SOURCE=.\utils\ra.c
# End Source File
# Begin Source File

SOURCE=.\utils\ra.h
# End Source File
# Begin Source File

SOURCE=.\utils\rpool.c
# End Source File
# Begin Source File

SOURCE=.\utils\rpool.h
# End Source File
# Begin Source File

SOURCE=.\utils\rtree.c
# End Source File
# Begin Source File

SOURCE=.\utils\rtree.h
# End Source File
# Begin Source File

SOURCE=.\utils\rvwatchdog.c
# End Source File
# Begin Source File

SOURCE=.\utils\rvwatchdog.h
# End Source File
# Begin Source File

SOURCE=.\utils\strutils.c
# End Source File
# Begin Source File

SOURCE=.\utils\strutils.h
# End Source File
# End Group
# End Target
# End Project


