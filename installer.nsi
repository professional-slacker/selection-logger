; Selection Logger NSIS Installer Script
; Requires NSIS (Nullsoft Scriptable Install System)

!include "MUI2.nsh"

; Basic installer settings
Name "Selection Logger"
OutFile "SelectionLogger-Setup.exe"
InstallDir "$PROGRAMFILES\SelectionLogger"
RequestExecutionLevel admin ; Require admin rights for service installation

; Modern UI settings
!define MUI_ABORTWARNING
!define MUI_ICON "icon.ico"
!define MUI_UNICON "icon.ico"

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

; Installer sections
Section "Selection Logger" SecMain
  SetOutPath "$INSTDIR"

  ; Install files
  File "selection-logger.exe"
  File "README.md"
  File "LICENSE.txt"

  ; Create uninstaller
  WriteUninstaller "$INSTDIR\uninstall.exe"

  ; Create start menu shortcuts
  CreateDirectory "$SMPROGRAMS\Selection Logger"
  CreateShortcut "$SMPROGRAMS\Selection Logger\Selection Logger.lnk" "$INSTDIR\selection-logger.exe"
  CreateShortcut "$SMPROGRAMS\Selection Logger\Uninstall.lnk" "$INSTDIR\uninstall.exe"

  ; Add to PATH (optional)
  ; EnVar::AddValue "PATH" "$INSTDIR"

  ; Write registry entries for uninstaller
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SelectionLogger" \
                   "DisplayName" "Selection Logger"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SelectionLogger" \
                   "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SelectionLogger" \
                   "DisplayIcon" "$INSTDIR\selection-logger.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SelectionLogger" \
                   "Publisher" "Professional Slacker"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SelectionLogger" \
                   "DisplayVersion" "1.0.0"

  ; Install as Windows service
  ExecWait '"$INSTDIR\selection-logger.exe" install'
  ExecWait '"$INSTDIR\selection-logger.exe" start'

SectionEnd

; Uninstaller section
Section "Uninstall"
  ; Stop and uninstall service
  ExecWait '"$INSTDIR\selection-logger.exe" stop'
  ExecWait '"$INSTDIR\selection-logger.exe" uninstall'

  ; Remove files
  Delete "$INSTDIR\selection-logger.exe"
  Delete "$INSTDIR\README.md"
  Delete "$INSTDIR\LICENSE.txt"
  Delete "$INSTDIR\uninstall.exe"

  ; Remove shortcuts
  Delete "$SMPROGRAMS\Selection Logger\Selection Logger.lnk"
  Delete "$SMPROGRAMS\Selection Logger\Uninstall.lnk"
  RMDir "$SMPROGRAMS\Selection Logger"

  ; Remove directory
  RMDir "$INSTDIR"

  ; Remove registry entries
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SelectionLogger"

SectionEnd