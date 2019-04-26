;-------------------------------------------------
; avfone-debug.nsi 安装程序
;-------------------------------------------------
Name "AVCON_WB"
Caption "AVCON_WB"
Icon ".\install.ico"
UninstallIcon ".\uninstall.ico"
InstallDir $PROGRAMFILES\AVCON_WB
OutFile "AVCON_WB.exe"
InstallDirRegKey HKLM "SOFTWARE\AVCON_WB" "Path"

RequestExecutionLevel admin

SetDateSave on
SetDatablockOptimize on
CRCCheck on
InstallColors FF8080 000030
XPStyle on

SilentInstall normal
SilentUninstall normal
AutoCloseWindow false
ShowInstDetails hide
ShowUninstDetails hide
BrandingText "http://www.avcon.com.cn"

;--------------------------------
; 语言
;--------------------------------
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\TradChinese.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\SimpChinese.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Japanese.nlf"

LicenseLangString myLicenseData ${LANG_ENGLISH} ".\00 Install Base\license-en.txt"
LicenseLangString myLicenseData ${LANG_TRADCHINESE} ".\00 Install Base\license-cht.txt"
LicenseLangString myLicenseData ${LANG_SIMPCHINESE} ".\00 Install Base\license-chs.txt"
LicenseLangString myLicenseData ${LANG_JAPANESE} ".\00 Install Base\license-jp.txt"

LicenseData $(myLicenseData)

LangString Name ${LANG_ENGLISH} "English"
LangString Name ${LANG_TRADCHINESE} "Traditional Chinese"
LangString Name ${LANG_SIMPCHINESE} "Simplified Chinese"
LangString Name ${LANG_JAPANESE} "Japanese"

LangString Lang ${LANG_ENGLISH} "en"
LangString Lang ${LANG_TRADCHINESE} "ct"
LangString Lang ${LANG_SIMPCHINESE} "cs"
LangString Lang ${LANG_JAPANESE} "jp"

;--------------------------------
; 安装过程页面
;--------------------------------
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------
; 安装过程
;--------------------------------
Section ""

  

  SetOutPath $INSTDIR

  FILE /R ".\BIN\*.DLL"
  FILE /R ".\BIN\*.EXE"
  FILE /R ".\BIN\*.INI"
  FILE /R ".\BIN\*.PNG"
  FILE /R ".\BIN\*.XML"
  FILE /R ".\BIN\*.BMP"
  FILE /R ".\BIN\*.cfg"
  
  
  ;..\00 BIN_VCRedist
  SetOutPath $INSTDIR\System
  File /r ".\00 BIN_VCRedist\vcredist_x86.exe"

  ClearErrors



  WriteRegStr HKLM SYSTEM\CurrentControlSet\Services\SharedAccess\Parameters\FirewallPolicy\StandardProfile\AuthorizedApplications\List	 "$INSTDIR\AVCON_WB.exe" "$INSTDIR\AVCON_WB.exe:*:Enabled:AVCON_WB"
  WriteRegStr HKLM SOFTWARE\AVCON_WB "Path" "$INSTDIR\"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AVCON_WB" "DisplayName" "AVCON_WB (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AVCON_WB" "UninstallString" '"$INSTDIR\uninst.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AVCON_WB" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AVCON_WB" "NoRepair" 1
  WriteUninstaller "uninst.exe"

  WriteRegStr HKLM SOFTWARE\AVCON_WB "Path" "$INSTDIR\"

  CreateShortCut "$DESKTOP\AVCON_WB.lnk" "$INSTDIR\AVCON_WB.exe" "" "$INSTDIR\AVCON_WB.exe" 0

  CreateDirectory "$SMPROGRAMS\AVCON_WB"
  CreateShortCut  "$SMPROGRAMS\AVCON_WB\AVCON_WB.lnk" "$INSTDIR\AVCON_WB.exe" "" "$INSTDIR\MediaFactory.exe" 0
  CreateShortCut  "$SMPROGRAMS\AVCON_WB\Uninstall.lnk" "$INSTDIR\uninst.exe" "" "$INSTDIR\uninst.exe" 0
  
  ;安装VC运行库
  SetDetailsPrint textonly ; 目的是只在上面显示提示文字，下面的框什么都不显示
  DetailPrint '正在安装 VC++ 运行库...'
  ExecWait '"$INSTDIR\System\vcredist_x86.exe" /q' ; 安装参数/quiet /passive /norestart
  Delete "$INSTDIR\System\vcredist_x86.exe" ; 删除文件

SectionEnd

;--------------------------------
; 卸载过程
;--------------------------------
Section "Uninstall"

  Delete "$INSTDIR\*.*"

  RMDir /r "$INSTDIR\"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\AVCON_WB\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\AVCON_WB"
  Delete "$DESKTOP\AVCON_WB.lnk"

SectionEnd

;--------------------------------
; 初始化时设置语言
;--------------------------------
Function .onInit

	;Language selection dialog

	Push ""
	Push ${LANG_ENGLISH}
	Push English
	Push ${LANG_TRADCHINESE}
	Push "Traditional Chinese"
	Push ${LANG_SIMPCHINESE}
	Push "Simplified Chinese"
	;Push ${LANG_FRENCH}
	;Push French
	Push ${LANG_JAPANESE}
	Push Japanese
	Push A ; A means auto count languages
	       ; for the auto count to work the first empty push (Push "") must remain
	;LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

	;Pop $LANGUAGE
	;StrCmp $LANGUAGE "cancel" 0 +2
	;	Abort
FunctionEnd

;--------------------------------
; 安装完成时执行
;--------------------------------
Function .onInstSuccess
	Exec '"$INSTDIR\AVCON_WB.exe"'
FunctionEnd
