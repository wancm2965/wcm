# Microsoft Developer Studio Project File - Name="h323nmake" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=h323nmake - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "h323nmake.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "h323nmake.mak" CFG="h323nmake - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "h323nmake - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "h323nmake - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "h323nmake - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f h323nmake.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "h323nmake.exe"
# PROP BASE Bsc_Name "h323nmake.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /NOLOGO /f "h323nmake.msdev.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "h323nmake.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "h323nmake - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f h323nmake.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "h323nmake.exe"
# PROP BASE Bsc_Name "h323nmake.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /NOLOGO /f "h323nmake.msdev.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "h323nmake.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "h323nmake - Win32 Release"
# Name "h323nmake - Win32 Debug"

!IF  "$(CFG)" == "h323nmake - Win32 Release"

!ELSEIF  "$(CFG)" == "h323nmake - Win32 Debug"

!ENDIF 

# Begin Group "ASN.1 Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\asn_messages\cap.asn
# End Source File
# Begin Source File

SOURCE=.\asn_messages\h225asn1.asn
# End Source File
# Begin Source File

SOURCE=.\asn_messages\h235asn1.asn
# End Source File
# Begin Source File

SOURCE=.\asn_messages\h450Config.asn
# End Source File
# Begin Source File

SOURCE=.\asn_messages\Properties.asn
# End Source File
# Begin Source File

SOURCE=.\asn_messages\q931asn1.asn
# End Source File
# Begin Source File

SOURCE=.\asn_messages\q931Config.asn
# End Source File
# Begin Source File

SOURCE=.\asn_messages\rasConfig.asn
# End Source File
# Begin Source File

SOURCE=.\asn_messages\systemConfig.asn
# End Source File
# End Group
# Begin Group "H.450 ASN.1 Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\h323addons\h450script\Addressing-Data-Elements.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Call-Completion-Operations.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Call-Diversion-Operations.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Call-Hold-Operations.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Call-Intrusion-Operations.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Call-Offer-Operation.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Call-Park-Pickup-Operations.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Call-Transfer-Operations.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Call-Waiting-Operations.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Common-Information-Operations.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\H225-generic-parameters-definition.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\H4501-General-Error-List.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\H4501-Supplementary-ServiceAPDU-Structure.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Manufacturer-specific-service-extension-definition.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Message-Waiting-Indication-Operations.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Name-Identification-Operations.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\q931asn1-small.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Remote-Operations-Apdus.asn"
# End Source File
# Begin Source File

SOURCE="..\h323addons\h450script\Remote-Operations-Information-Objects.asn"
# End Source File
# End Group
# Begin Group "Makefiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\h323nmake.msdev.mak
# End Source File
# End Group
# Begin Group "H.245 ASN.1 Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h245\h245Config.asn
# End Source File
# Begin Source File

SOURCE=..\h245\h245v11.asn
# End Source File
# End Group
# End Target
# End Project


