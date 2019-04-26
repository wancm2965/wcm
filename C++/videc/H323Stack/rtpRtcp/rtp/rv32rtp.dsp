# Microsoft Developer Studio Project File - Name="rv32rtp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=rv32rtp - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rv32rtp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rv32rtp.mak" CFG="rv32rtp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rv32rtp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rv32rtp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rv32rtp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\..\..\binaries\windows\release\bin"
# PROP Intermediate_Dir ".\..\..\binaries\windows\release\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../common/ccore/netdrivers" /I "../../common/cbase" /I "../../common/ccore" /I "../../common/config" /I "../../common/config/arch" /I "../../common/config/os" /I "../../common/config/tool" /I "../../common/cutils" /I "../../common/ccore/memdrivers" /I "../../common/cbase/netdrivers" /I "../../common/adapters" /I "../../common/adapters/windows" /D "_WINDOWS" /D "NOTHREADS" /D "WIN32" /D "NDEBUG" /D "RV_RELEASE" /FD /c
# SUBTRACT CPP /WX /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gdi32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmtd"

!ELSEIF  "$(CFG)" == "rv32rtp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\..\..\binaries\windows\debug\bin"
# PROP Intermediate_Dir ".\..\..\binaries\windows\debug\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../common/adapters" /I "../../common/adapters/windows" /I "../../common/ccore/netdrivers" /I "../../common/cbase" /I "../../common/ccore" /I "../../common/config" /I "../../common/config/arch" /I "../../common/config/os" /I "../../common/config/tool" /I "../../common/cutils" /I "../../common/ccore/memdrivers" /I "../../common/cbase/netdrivers" /D "_WINDOWS" /D "NOTHREADS" /D "WIN32" /D "_DEBUG" /D "RV_DEBUG" /FD /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gdi32.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "rv32rtp - Win32 Release"
# Name "rv32rtp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bitfield.c
# End Source File
# Begin Source File

SOURCE=.\payload.c
# End Source File
# Begin Source File

SOURCE=.\rtcp.c
# End Source File
# Begin Source File

SOURCE=.\rtp.c
# End Source File
# Begin Source File

SOURCE=.\rtputil.c
# End Source File
# Begin Source File

SOURCE=.\rvrtpbuffer.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bitfield.h
# End Source File
# Begin Source File

SOURCE=.\payload.h
# End Source File
# Begin Source File

SOURCE=.\rtcp.h
# End Source File
# Begin Source File

SOURCE=.\rtp.h
# End Source File
# Begin Source File

SOURCE=.\rtputil.h
# End Source File
# Begin Source File

SOURCE=.\rvrtpbuffer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\rtp.rc
# End Source File
# End Target
# End Project


