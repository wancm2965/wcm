# Microsoft Developer Studio Project File - Name="FloatFR_aacdeclib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=FloatFR_aacdeclib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FloatFR_aacdeclib_mpeg4.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FloatFR_aacdeclib_mpeg4.mak" CFG="FloatFR_aacdeclib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FloatFR_aacdeclib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "FloatFR_aacdeclib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "FloatFR_aacdeclib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FloatFR_aacdeclib___Win32_Debug"
# PROP BASE Intermediate_Dir "FloatFR_aacdeclib___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Win32_D"
# PROP Intermediate_Dir "Win32_D/obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /Op /I "./" /I "../FloatFRlib" /I "../FloatFR_bitbuflib" /I "../FloatFR_sbrdeclib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "PARAMETRICSTEREO_3GPP" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /Op /I "./" /I "../FloatFRlib" /I "../FloatFR_bitbuflib" /I "../FloatFR_sbrdeclib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MONO_ONLY" /D "LP_SBR_ONLY" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "FloatFR_aacdeclib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FloatFR_aacdeclib___Win32_Release"
# PROP BASE Intermediate_Dir "FloatFR_aacdeclib___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Win32_O"
# PROP Intermediate_Dir "Win32_O/obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Od /Op /I "./" /I "../FloatFRlib" /I "../FloatFR_bitbuflib" /I "../FloatFR_sbrdeclib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "PARAMETRICSTEREO_3GPP" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Od /Op /I "./" /I "../FloatFRlib" /I "../FloatFR_bitbuflib" /I "../FloatFR_sbrdeclib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MONO_ONLY" /D "LP_SBR_ONLY" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "FloatFR_aacdeclib - Win32 Debug"
# Name "FloatFR_aacdeclib - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\aac_ram.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\aac_rom.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\aacdecoder.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\bitstream.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\block.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\channel.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\channelinfo.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\conceal.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\dse.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\imdct.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\longblock.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\pns.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\pulsedata.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\shortblock.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\stereo.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\streaminfo.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# Begin Source File

SOURCE=.\src\tns.c
# ADD BASE CPP /I "..\FloatFRlib .\\"
# ADD CPP /I "..\FloatFRlib .\\"
# End Source File
# End Group
# End Target
# End Project
