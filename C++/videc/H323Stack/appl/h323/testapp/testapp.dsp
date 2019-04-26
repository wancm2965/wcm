# Microsoft Developer Studio Project File - Name="testapp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=testapp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "testapp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "testapp.mak" CFG="testapp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "testapp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "testapp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "testapp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\..\..\..\binaries\windows\release\bin"
# PROP Intermediate_Dir ".\..\..\..\binaries\windows\release\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "include\OpenLDAP\\" /I "lep" /I "..\..\include" /I "..\..\include\tcl" /I "..\..\..\include\common" /I "..\..\..\include\asn1" /I "..\..\..\include\h245" /I "..\..\..\include\rtp" /I "..\..\..\include\h323" /I "..\..\..\include\rvrtp" /D "NDEBUG" /D "RV_RELEASE" /D "_WINDOWS" /D "USE_SNMP" /D "_MBCS" /D "USE_RTP" /D "WIN32" /FD /Zm500 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tcl84.lib tk84.lib ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:".\..\..\..\binaries\windows\release\bin/h323testapp.exe" /libpath:"..\..\lib\OpenLDAP" /libpath:"..\..\lib\OpenSSL" /libpath:"..\..\lib"

!ELSEIF  "$(CFG)" == "testapp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\..\..\..\binaries\windows\debug\bin"
# PROP Intermediate_Dir ".\..\..\..\binaries\windows\debug\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "include\OpenLDAP\\" /I "lep" /I "..\..\include" /I "..\..\include\tcl" /I "..\..\..\include\common" /I "..\..\..\include\asn1" /I "..\..\..\include\h245" /I "..\..\..\include\rtp" /I "..\..\..\include\h323" /I "..\..\..\include\rvrtp" /D "_DEBUG" /D "RV_DEBUG" /D "_WINDOWS" /D "USE_SNMP" /D "_MBCS" /D "USE_RTP" /D "WIN32" /FD /GZ /Zm500 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tcl84.lib tk84.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /out:".\..\..\..\binaries\windows\debug\bin/h323testapp.exe" /pdbtype:sept /libpath:"..\..\lib\OpenLDAP" /libpath:"..\..\lib\OpenSSL" /libpath:"..\..\lib"

!ENDIF 

# Begin Target

# Name "testapp - Win32 Release"
# Name "testapp - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\rvsnlogo.ico
# End Source File
# Begin Source File

SOURCE=.\res\testapp.rc
# End Source File
# End Group
# Begin Group "lep"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lep\lep.c
# End Source File
# Begin Source File

SOURCE=.\lep\lep.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepCall.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepCall.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepChannel.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepChannel.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepControl.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepControl.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepConversions.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepConversions.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepDefs.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepFwNat.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepFwNat.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepH450.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepH450.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepHash.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepHash.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepHooks.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepHooks.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepLdap.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepLdap.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepRas.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepRas.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepRtp.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepRtp.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepSecurity.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepSecurity.h
# End Source File
# Begin Source File

SOURCE=.\lep\lepUtils.c
# End Source File
# Begin Source File

SOURCE=.\lep\lepUtils.h
# End Source File
# End Group
# Begin Group "gui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TAP_call.c
# End Source File
# Begin Source File

SOURCE=.\TAP_call.h
# End Source File
# Begin Source File

SOURCE=.\TAP_channel.c
# End Source File
# Begin Source File

SOURCE=.\TAP_channel.h
# End Source File
# Begin Source File

SOURCE=.\TAP_defs.h
# End Source File
# Begin Source File

SOURCE=.\TAP_general.c
# End Source File
# Begin Source File

SOURCE=.\TAP_general.h
# End Source File
# Begin Source File

SOURCE=.\TAP_h450.c
# End Source File
# Begin Source File

SOURCE=.\TAP_h450.h
# End Source File
# Begin Source File

SOURCE=.\TAP_init.c
# End Source File
# Begin Source File

SOURCE=.\TAP_init.h
# End Source File
# Begin Source File

SOURCE=.\TAP_log.c
# End Source File
# Begin Source File

SOURCE=.\TAP_log.h
# End Source File
# Begin Source File

SOURCE=.\TAP_main.c
# End Source File
# Begin Source File

SOURCE=.\TAP_ras.c
# End Source File
# Begin Source File

SOURCE=.\TAP_ras.h
# End Source File
# Begin Source File

SOURCE=.\TAP_scripts.c
# End Source File
# Begin Source File

SOURCE=.\TAP_security.c
# End Source File
# Begin Source File

SOURCE=.\TAP_security.h
# End Source File
# Begin Source File

SOURCE=.\TAP_tools.c
# End Source File
# Begin Source File

SOURCE=.\TAP_tools.h
# End Source File
# Begin Source File

SOURCE=.\TAP_wrapper.c
# End Source File
# Begin Source File

SOURCE=.\TAP_wrapper.h
# End Source File
# End Group
# End Target
# End Project


