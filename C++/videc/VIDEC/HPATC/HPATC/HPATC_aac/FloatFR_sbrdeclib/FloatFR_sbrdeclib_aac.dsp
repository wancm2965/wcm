# Microsoft Developer Studio Project File - Name="FloatFR_sbrdeclib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=FloatFR_sbrdeclib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FloatFR_sbrdeclib_aac.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FloatFR_sbrdeclib_aac.mak" CFG="FloatFR_sbrdeclib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FloatFR_sbrdeclib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "FloatFR_sbrdeclib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "FloatFR_sbrdeclib - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Win32_D"
# PROP Intermediate_Dir "Win32_D/obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /Op /I "./" /I "../FloatFRlib" /I "../FloatFR_aacdec" /I "../FloatFR_aacdec/src" /I "../FloatFR_bitbuflib" /I "../mathlib" /D "_DEBUG" /D "PARAMETRICSTEREO_3GPP" /D "_MBCS" /D "_LIB" /D "WIN32" /FD /GZ /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /Op /I "./" /I "../FloatFRlib" /I "../FloatFR_aacdec" /I "../FloatFR_aacdec/src" /I "../FloatFR_bitbuflib" /I "../mathlib" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "WIN32" /D "MONO_ONLY" /D "LP_SBR_ONLY" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "FloatFR_sbrdeclib - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Win32_O"
# PROP Intermediate_Dir "Win32_O/obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Od /Op /I "./" /I "../FloatFRlib" /I "../FloatFR_aacdec" /I "../FloatFR_aacdec/src" /I "../FloatFR_bitbuflib" /I "../mathlib" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "WIN32" /D "PARAMETRICSTEREO_3GPP" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /GX /Od /Op /I "./" /I "../FloatFRlib" /I "../FloatFR_aacdec" /I "../FloatFR_aacdec/src" /I "../FloatFR_bitbuflib" /I "../mathlib" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "WIN32" /D "MONO_ONLY" /D "LP_SBR_ONLY" /FD /c
# SUBTRACT CPP /Fr
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

# Name "FloatFR_sbrdeclib - Win32 Debug"
# Name "FloatFR_sbrdeclib - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\env_calc.c
# End Source File
# Begin Source File

SOURCE=.\src\env_dec.c
# End Source File
# Begin Source File

SOURCE=.\src\env_extr.c
# End Source File
# Begin Source File

SOURCE=.\src\FFR_aacPLUScheck.c
# End Source File
# Begin Source File

SOURCE=.\src\freq_sca.c
# End Source File
# Begin Source File

SOURCE=.\src\huff_dec.c
# End Source File
# Begin Source File

SOURCE=.\src\hybrid.c

!IF  "$(CFG)" == "FloatFR_sbrdeclib - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FloatFR_sbrdeclib - Win32 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\lpp_tran.c
# End Source File
# Begin Source File

SOURCE=.\src\ps_bitdec.c

!IF  "$(CFG)" == "FloatFR_sbrdeclib - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FloatFR_sbrdeclib - Win32 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\ps_dec.c

!IF  "$(CFG)" == "FloatFR_sbrdeclib - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FloatFR_sbrdeclib - Win32 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\qmf_dec.c
# End Source File
# Begin Source File

SOURCE=.\src\sbr_bitb.c
# End Source File
# Begin Source File

SOURCE=.\src\sbr_crc.c
# End Source File
# Begin Source File

SOURCE=.\src\sbr_dec.c
# End Source File
# Begin Source File

SOURCE=.\src\sbr_fft.c
# End Source File
# Begin Source File

SOURCE=.\src\sbr_ram.c
# End Source File
# Begin Source File

SOURCE=.\src\sbr_rom.c
# End Source File
# Begin Source File

SOURCE=.\src\sbrdecoder.c
# End Source File
# End Group
# End Target
# End Project
