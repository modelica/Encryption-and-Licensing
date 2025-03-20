# Copyright (C) 2022 Modelica Association
#
#    Copyright (C) 2015 Modelon AB
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.
#
#     This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    BSD_License.txt file for more details.
#
#    You should have received a copy of the BSD_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
#


# -------------------------------------------------------------
# Window installer Library Encryption project.
# Installs the necessary components to compile and run
# the Library Encryption project.
#
# Plug-ins used are:
# 	nsis7z-plug-in (http://nsis.sourceforge.net/Nsis7z_plug-in)
#	zipdll-plug-in (http://nsis.sourceforge.net/Nsis7z_plug-in)
#
# These two plug-ins must be installed before you can compile
# the script.
#
# Components in this installer are:
# CMake - (http://www.cmake.org/)
# Strawberry Perl - (http://strawberryperl.com/)  32/64-bit
# Nasm - (http://nasm.sourceforge.net/)
# -------------------------------------------------------------

#Include Modern UI
!include "MUI.nsh"
!include "WordFunc.nsh"

# Include zip dll.
!include "ZipDLL.nsh" 

# Detect 64-bit Windows.
!include x64.nsh

# ---------------------
# General information.
# ---------------------

# Name of installer.
Outfile "LibraryEncryption_installer.exe"

Name "Library Encryption Components"
InstallDir "C:\SDK_LibraryEncryption"

Var appFolder

# ------------------
# UI configuration.
# ------------------

# Welcome page.
!define MUI_WELCOMEPAGE_TITLE 'Library Encryption components'
!define MUI_WELCOMEPAGE_TEXT 'Install components needed to compile and run the Library Encryption project.'
!insertmacro MUI_PAGE_WELCOME

# Components page.
!insertmacro MUI_PAGE_COMPONENTS    
!insertmacro MUI_PAGE_DIRECTORY		
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH		

# Uninstall page.
!define MUI_UNPAGE_WELCOME_TITLE 'Uninstall Library Encryption'  
!define MUI_WELCOMEPAGE_TEXT 'Uninstall Library Encryption project components.'
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

# -----------
# Languages.
# -----------
!insertmacro MUI_LANGUAGE "English"

Section
	# Create output direcory otherwise uninstaller will not be created.
	CreateDirectory "$INSTDIR"
	
	# Define uninstaller name.
	WriteUninstaller "$INSTDIR\LibraryEncryption_uninstaller.exe"
	
	# Folder where the files to install are located.
	StrCpy $appFolder "C:\SDK_LibraryEncryption\applications"	
SectionEnd



# --------------------
# Installer sections.
# --------------------

Section "Perl" SEC02

	# Add size(in kb) manually.
	AddSize 290000
	
	# Perl will start its own installer/uninstaller and setup it own paths.
	# You must manually add Strawberry/perl/bin to environment variable PATH.
	# If installing on Windows 64 bits, Perl will not
	# be installed if there already is a Perl 64-bit version installed.
	
	${If} ${RunningX64}
		# Add file to install.
		File "applications\strawberry-perl-5.22.0.1-64bit.msi"
		
		ExecWait '"$SYSDIR\msiexec" /i "strawberry-perl-5.22.0.1-64bit.msi"'
	${Else}
		# Add file to install.
		File "applications\strawberry-perl-5.22.0.1-32bit.msi"
		
		ExecWait '"$SYSDIR\msiexec" /i "strawberry-perl-5.22.0.1-32bit.msi"'
	${EndIf}   

SectionEnd


Section "Nasm" SEC03
	
	# Add size(in kb) manually.
	AddSize 2100
	
	# Add file to install.
	File "applications\nasm-2.11.08-win32.zip"
	
	# Extract Nasm.
	ZipDLL::extractall "nasm-2.11.08-win32.zip" "$INSTDIR"
	
	# Add environment variable.
	Push "$INSTDIR\nasm-2.11.08\"
	Exch $0
	Call AddEnvironmentVariable
	Pop $0
	
SectionEnd

Section "CMake 3.3" SEC04
	
	# Add size(in kb) manually.
	AddSize 28000
	
	# Add file to install.
	File "applications\cmake-3.3.0-rc4-win32-x86.zip"
	
	# Extract CMake.
	ZipDLL::extractall "cmake-3.3.0-rc4-win32-x86.zip" "$INSTDIR"
	
	# Add environment variable.
	Push "$INSTDIR\cmake-3.3.0-rc4-win32-x86\bin"
	Exch $0	
	Call AddEnvironmentVariable
	Pop $0
	
SectionEnd

# ----------------------
# Uninstaller sections.
# ----------------------

Section "Uninstall"

	# ------------------------
    # Delete the uninstaller.
	# ------------------------
    Delete "$INSTDIR\LibraryEncryption_uninstaller.exe"

	# ----------------
	# Delete folders.
	# ----------------
	RMDir /r "$INSTDIR\nasm-2.11.08"
	RMDir /r "$INSTDIR\cmake-3.3.0-rc4-win32-x86"
	RMDir /r "$INSTDIR"
	
	# ------------------------------
	# Remove environment variables.
	# ------------------------------
	
	# Nasm.
	Push "$INSTDIR\nasm-2.11.08\"
	Exch $0
	Call un.RemoveEnvironmentVariable
	Pop $0
	
	# CMake.
	Push "$INSTDIR\cmake-3.3.0-rc4-win32-x86\bin"
	Exch $0	
	Call un.RemoveEnvironmentVariable
	Pop $0
	
SectionEnd


!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Data compression library in C."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "Script language environment."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC03} "X86 assembler." 
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} "Cross-platform build system." 
!insertmacro MUI_FUNCTION_DESCRIPTION_END


# ---------------------------
# Add environment variable.
# ---------------------------
Function AddEnvironmentVariable

	# Read Path environment for current user.
	ReadRegStr $R1 HKCU "Environment" "PATH"

	# Is variable ($0) in Path already?
	${WordFind} "$R1" "$0" "E+1{" $R0

	StrCmp $R0 "$0" 0 notfound
	notfound:
		StrCpy $R1 "$R1;$0"
		WriteRegExpandStr HKCU "Environment" "PATH" "$R1"
		
FunctionEnd


# -------------------------------------
# Remove Nasm to environment variable.
# -------------------------------------
Function un.RemoveEnvironmentVariable

	# Read Path environment for current user.
	ReadRegStr $R1 HKCU "Environment" "PATH"
	
	# Is variable ($0) in Path already?
	${WordFind} "$R1" "$0" "E+1{" $R0

	StrCmp $R0 "$0" found 0
	found:
		${un.WordReplace} "$R1" ";$0" "" "+" $R1
		WriteRegExpandStr HKCU "Environment" "PATH" "$R1"

FunctionEnd






