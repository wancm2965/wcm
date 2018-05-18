;-------------------------------------------------
; avfone-debug.nsi 安装程序
;-------------------------------------------------
Name "AIServerTestDemo"
Caption "AIServerTestDemo"
Icon ".\install.ico"
UninstallIcon ".\uninstall.ico"
InstallDir $PROGRAMFILES\AIServerTestDemo
OutFile "AIServerTestDemo.exe"
InstallDirRegKey HKLM "SOFTWARE\AIServerTestDemo" "Path"

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
  
  
  ;..\00 BIN_VCRedist
  SetOutPath $INSTDIR\System
  File /r ".\00 BIN_VCRedist\vcredist_x86.exe"

  ClearErrors



  WriteRegStr HKLM SYSTEM\CurrentControlSet\Services\SharedAccess\Parameters\FirewallPolicy\StandardProfile\AuthorizedApplications\List	 "$INSTDIR\AIServerTestDemo.exe" "$INSTDIR\AIServerTestDemo.exe:*:Enabled:AIDemo"
  WriteRegStr HKLM SOFTWARE\AIServerTestDemo "Path" "$INSTDIR\"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AIServerTestDemo" "DisplayName" "AIServerTestDemo (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AIServerTestDemo" "UninstallString" '"$INSTDIR\uninst.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AIServerTestDemo" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AIServerTestDemo" "NoRepair" 1
  WriteUninstaller "uninst.exe"

  WriteRegStr HKLM SOFTWARE\AIServerTestDemo "Path" "$INSTDIR\"

  CreateShortCut "$DESKTOP\AIServerTestDemo.lnk" "$INSTDIR\AIServerTestDemo.exe" "" "$INSTDIR\AIServerTestDemo.exe" 0

  CreateDirectory "$SMPROGRAMS\AIServerTestDemo"
  CreateShortCut  "$SMPROGRAMS\AIServerTestDemo\AIServerTestDemo.lnk" "$INSTDIR\AIServerTestDemo.exe" "" "$INSTDIR\MediaFactory.exe" 0
  CreateShortCut  "$SMPROGRAMS\AIServerTestDemo\Uninstall.lnk" "$INSTDIR\uninst.exe" "" "$INSTDIR\uninst.exe" 0
  
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
  Delete "$SMPROGRAMS\AIServerTestDemo\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\AIServerTestDemo"
  Delete "$DESKTOP\AIServerTestDemo.lnk"

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
	Exec '"$INSTDIR\AIServerTestDemo.exe"'
FunctionEnd
