
#.NOAUTODEPEND
#.NOSWAP

__VERSION_MAJOR = 2
__VERSION_MINOR = 1
__VERSION_BUILD = 65
__VERSION       = $(__VERSION_MAJOR).$(__VERSION_MINOR).$(__VERSION_BUILD)

!ifdef ALTERNATE
__ALTERNATE  = 1
__CODE_NAME  = ALTERNATE
__BASE_NAME  = $(__CODE_NAME)$(__VERSION_MAJOR)$(__VERSION_MINOR) 
__FILE_NAME  = alternate-$(__VERSION)
__PROGID     = $(__BASE_NAME)
__UUID_CLSID = ""
__UUID_LIBID = ""
!else
__ALTERNATE  = 0
__CODE_NAME  = DBO
__BASE_NAME  = $(__CODE_NAME)$(__VERSION_MAJOR)$(__VERSION_MINOR)
__FILE_NAME  = dbo-$(__VERSION)
__LEGAL_STR  = (1998) RODABLO para Origenes A.F.J.P.
__PROGID     = $(__BASE_NAME)
__UUID_CLSID = ca43d287-ef82-11cf-b8c6-00a02454eec8
__UUID_LIBID = ca43d288-ef82-11cf-b8c6-00a02454eec8
!endif
__DLL_NAME_S = "$(__FILE_NAME).dll"
__HLP_NAME_S = "$(__FILE_NAME).hlp"

###
### TOOLS...
###
!if "MRODABLOATH" == "$(COMPUTERNAME)"
VCROOT  = C:\VC
PRGFILE = "c:\Program Files"
!else
VCROOT  = C:\VC
PRGFILE = "c:\Archivos de programa"
!endif

#IMPLIB  = Implib
CC   = $(VCROOT)\bin\cl.exe
LINK = $(VCROOT)\bin\link.exe 
RC   = c:\bc5\bin\brc32.exe
MC   = $(VCROOT)\bin\mc.exe  
MIDL = $(VCROOT)\bin\midl.exe

!if "MRODABLOATH" == "$(COMPUTERNAME)"
ZIP     = "c:\Program Files\WinZip\WINZIP32.EXE"
ZIPSE   = "c:\Program Files\WinZip Self-Extractor\Winzipse.exe"
#ZIPSE   = "c:\Program Files\WinZip\Winzipse.exe"
!else
#RC      = "c:\Archivos de programa\DevStudio\SharedIDE\bin\rc.exe"
ZIP     = "c:\Archivos de programa\WinZip\WINZIP32.EXE"
ZIPSE   = "c:\Archivos de programa\WinZip Self-Extractor\Winzipse.exe"
#ZIPSE  = "c:\Archivos de programa\WinZip\Winzipse.exe"
!endif

###
### Paths
###
TMP       = c:\temp\object
BINARY    = c:\z\bin
SOURCE    = d:\work
TMP2      = $(TMP)\\
EMPTY     = 
ORANTROOT = c:\ORANT

.path.obj=$(TMP)
.path.cpp=$(SOURCE)
.path.hxx=$(SOURCE)
.path.res=$(TMP)
.path.zip=$(TMP)
.path.dll=$(BINARY)
.path.exe=$(BINARY)
.path.hlp=$(BINARY)
.path.cnt=$(BINARY)
.path.inf=$(BINARY)
.path.i  =$(TMP)

.suffixes: .hxx .c .cpp .obj .dll .exe

###
### 
###
LINKOBJS = \
	ZZdll.obj ZZlicense.obj ZZauxiliar.obj ZZclassfactory.obj ZZdispatch.obj \
	ZZsession.obj \
        ZZcursor.obj ZZcursor2.obj ZZcursor3.obj \
        ZZcolumns.obj ZZparams.obj \
        ZZcolumn.obj ZZcolumn_bstr.obj ZZcolumn_date.obj \
        ZZparam_g.obj ZZparam_bind.obj ZZparam_bind_array.obj 
###
###
### LINK
###
$(__FILE_NAME).dll: $(.path.obj)\dbomc.hxx $(.path.obj)\dboidl.hxx $(.path.obj)\dboidl.obj $(LINKOBJS:ZZ=$(EMPTY)) dbo.res dbo.def #makefile
   $(LINK) @&&|
#-Tpd -aa -L.\LIB;$(BCROOT)\LIB;C:\ORANT\OCI73\LIB\BORLAND; -v -c -n -V4.0 -w-dup -x @&&|
#.\lib\c0d32dyn.obj+
#$(TMP)\dboidl.obj+
#$(LINKOBJS:ZZ=$(TMP2)), $(BINARY)\$(__FILE_NAME).dll, $(TMP)\$(__FILE_NAME).map,+
#cg32.lib+
#cw32.lib+
#cw32mt.lib+
#$(BCROOT)\LIB\bidsf.lib+
#oleaut32.lib advapi32.lib ole2w32.lib import32.lib+
#d:\work\lib\
#ociw32.lib, dbo.def, $(TMP)\dbo.res

/DEBUG
#/DEBUGTYPE:COFF
#/PDB:NONE				# $(BINARY)\$(__FILE_NAME).pdb
/DLL
/NODEFAULTLIB                           # Ignore Libraries
/MACHINE:IX86                           # ALPHA # Specify Target Platform
/INCREMENTAL:NO
/LIBPATH:$(VCROOT)\lib
/OUT:$(BINARY)\$(__FILE_NAME).dll
/DEF:dbo.def 
$(LINKOBJS:ZZ=$(TMP2))
$(TMP)\dboidl.obj
c:\orant\oci73\lib\msvc\ociw32.lib
libcmtd.lib
####libcmt.lib
#msvcrt.lib
kernel32.lib
Msvcprt.lib
#libcmt.lib
#msvcrt.lib
ole32.lib 
uuid.lib #olepro32.lib
user32.lib
oleaut32.lib 
advapi32.lib 
netapi32.lib
$(TMP)\dbo.res
|

###
###
###
### CONFIG.H
###
$(TMP)\config.h: #makefile
	sed "s/@/#/g" < &&|	# uso el sed porque '#' es comentoario para el make
@ifndef __CONFIG_H_
@define __CONFIG_H_
// VERSION
@define __VERSION_MAJOR $(__VERSION_MAJOR)
@define __VERSION_MINOR $(__VERSION_MINOR)
@define __VERSION_BUILD $(__VERSION_BUILD)
@define __VERSION       $(__VERSION)
// NOMBRES
@define __ALTERNATE  $(__ALTERNATE)
@define __CODE_NAME  $(__CODE_NAME)
@define __BASE_NAME  $(__BASE_NAME)
@define __FILE_NAME  $(__FILE_NAME)
@define __DLL_NAME_S $(__DLL_NAME_S)
@define __HLP_NAME_S $(__HLP_NAME_S)
// UUIDs & Cia
@define __PROGID     $(__PROGID)
@define __UUID_CLSID "$(__UUID_CLSID)"
@define __UUID_LIBID "$(__UUID_LIBID)"

@define __LIB_VERSION_MAJOR $(__VERSION_MAJOR)
@define __LIB_VERSION_MINOR $(__VERSION_MINOR)
@define __LIB_VERSION       $(__VERSION_MAJOR).$(__VERSION_MINOR)
@define __LIB_LCID          0

@endif
| > $< 

###
###
### MC
### ERRORES WIN32
###
$(TMP)\dbomc.hxx: dbomc.mc #$(TMP)\dbomc.hxx $(TMP)\MSG00000.bin: dbomc.mc
    $(MC) -o -v -s -e hxx -h $(TMP)\ -r $(TMP)\ dbomc.mc
###
###
###
### IDL
###
$(TMP)\dboidl.hxx: dboidl.idl $(TMP)\config.h # Ver multiples targets en el make
#$(TMP)\dboidl.c: dboidl.idl # Ver multiples targets en el make
    $(MIDL) @&&|        
/ms_ext /char unsigned /nologo /win32 
/I $(TMP)
/I c:\mstools\include
/tlb $*.tlb /iid $*.c /h $< dboidl.idl
|
###
###
$(TMP)\dboidl.obj: $(TMP)\dboidl.c
	$(CC) /c /nologo /TC /Fo$@ $(TMP)\dboidl.c

###
###
###
### CPP's & opciones
###
.cpp.obj:
	$(CC) /c /nologo @&&|
/TP
#/c                                      # Compile Without Linking
#/nologo
/Fp$(TMP)\$(__FILE_NAME).pch	        # header precopilado
/YXpch.hxx				# Automatic Use of Precompiled Headers
/X                                      # Ignore Standard Include Paths
/I$(TMP) 
/I$(VCROOT)\INCLUDE
#/IC:\MSTOOLS\INCLUDE
/I$(ORANTROOT)\OCI73\INCLUDE 
/DINC_OLE2;STRICT;_DEBUG 
/D_WINDLL
/LDd                                     # Creates a DLL
#/ML                			# Single-Threaded
/MT					# Multithreaded
#/Ob1                                     # inline only In-line Function Expansion

#/ML					# Single-Threaded 
#/Od					# Disable (Debug)
/GX                                     # Enable Exception Handling (unwind)
#/Gy                                     # Enable Function-Level Linking
/EHa                                    # specify the asynchronous exception handling model

/Od 					 # Disable (Debug)
/Oy-	# Enables the creation of frame pointers on the call stack.
/Odi	# Disables optimization.
/Yd     # Place Debug Info in Object File
/Z7	# Generates complete debugging information
#/Zn     # Turn Off SBRPACK


# -tWDE                              # Windows DLL, explicit functions exported
# -ff-                               # ANSI float
# -x                                 # enable exceptions
# #-xp                               # Enable exception location information
# -xd                                # Enable destructor cleanup
# -xds                               # Enable throwing exceptions from a DLL
# -RT                                # Enable run-time type information
# -d                                 # Duplicate strings merged
# -r                                 # Register variables
# -X-                                # Autodependency information
# -u                                 # Generate underscores
# -C                                 # nested comments
# -A- 
# -AT                                # borland extensions
# -w                                 # all warnings
# -R                                 # browse info in obj
# -v                                 # turns debugging on and inline expansion off
# -Od                                # Disable all optimizations
# -Jg                                # Smart Templates
# -5                                 # i386 pentium i486  
# #-vGd 
# #-vGt
# -wcln -wsig -wucp -w-par -w-pch- -wstv -wasm -wamp 
# -wobs -wpch -wdef -wnod -wamb -wbbf -wpin -wnak
# | -o$@ $. 

| /Fo$@ $. 

###
### Dependencias explicitas
###
### TOTOS = $(LINKOBJS:ZZ=$(EMPTY))
### MODULOS = $(TOTOS:obj=cpp)

dbo.hxx:		config.h dbomc.hxx dboidl.hxx auxiliar.hxx dispatch.hxx

dll.cpp: 		pch.hxx dbo.hxx
license.cpp: 		pch.hxx dbo.hxx
auxiliar.cpp: 		pch.hxx dbo.hxx
dispatch.cpp:		pch.hxx dbo.hxx

classfactory.cpp: 	isession.hxx pch.hxx dbo.hxx

session.hxx:		isession.hxx
session.cpp: 		session.hxx icursor.hxx pch.hxx dbo.hxx

icursor.hxx:		isession.hxx
cursor.hxx:		icursor.hxx icolumn.hxx iparam.hxx icolumns.hxx iparams.hxx
cursor.cpp: 		cursor.hxx pch.hxx dbo.hxx
cursor2.cpp: 		cursor.hxx pch.hxx dbo.hxx
cursor3.cpp: 		cursor.hxx pch.hxx dbo.hxx

icolumns.hxx:		icursor.hxx
columns.cpp: 		icolumns.hxx pch.hxx dbo.hxx
iparams.hxx:		icursor.hxx
params.cpp: 		iparams.hxx pch.hxx dbo.hxx

icolumn.hxx:		icursor.hxx
column.hxx:		icolumn.hxx
column.cpp: 		column.hxx pch.hxx dbo.hxx
column_bstr.cpp: 	column.hxx pch.hxx dbo.hxx
column_date.cpp: 	column.hxx pch.hxx dbo.hxx

iparam.hxx:		icursor.hxx
param.hxx:    		iparam.hxx
param_g.cpp: 		param.hxx pch.hxx dbo.hxx
param_bind.cpp:		param.hxx pch.hxx dbo.hxx
param_bind_array.cpp: 	param.hxx pch.hxx dbo.hxx

###
###
### .RES
### RECURSOS WINDOWS 
###
#$(TMP)\dbo.res :  dbo.rc dboidl.tlb $(TMP)\MSG00000.bin
#$(.path.res)\
dbo.res: dbo.rc $(TMP)\dboidl.hxx $(TMP)\dbomc.hxx $(TMP)\versioninfo.rc
    $(RC) -R @d:\work\&&|
   -I$(.path.obj);$(BCROOT)\INCLUDE; 
   -DINC_OLE2;STRICT;_DEBUG;  
   -FO$@ dbo.rc               
|

$(TMP)\versioninfo.rc:	#makefile
	copy &&|
VS_VERSION_INFO VERSIONINFO 
  FILEVERSION     $(__VERSION_MAJOR),$(__VERSION_MINOR),$(__VERSION_BUILD),0
  PRODUCTVERSION  $(__VERSION_MAJOR),$(__VERSION_MINOR),$(__VERSION_BUILD),0
  FILEFLAGSMASK   /*VS_FF_DEBUG |*/ VS_FF_PRERELEASE
  FILEFLAGS       VS_FF_PRERELEASE | VS_FF_SPECIALBUILD
  FILEOS          VOS_NT_WINDOWS32 //VOS_WINDOWS32
  FILETYPE        VFT_DLL
  FILESUBTYPE     VS_USER_DEFINED
BEGIN
  BLOCK "StringFileInfo"
  BEGIN
    BLOCK "00000000"
    BEGIN
      VALUE "CompanyName", "RODABLO\0"
      VALUE "FileDescription", "$(__BASE_NAME)\0"
      VALUE "FileVersion", "$(__VERSION)\0"
      VALUE "InternalName","$(__FILE_NAME)\0"
!if __ALTERNATE
      VALUE "LegalCopyright", ""
!else
      VALUE "LegalCopyright", "$(__LEGAL_STR)\0"
!endif
      VALUE "ProductName", "$(__FILE_NAME)\0"
      VALUE "ProductVersion", "$(__VERSION_MAJOR).$(__VERSION_MINOR)\0"
      VALUE "Build", "$(__VERSION_BUILD)\0"
    END	             
  END
  BLOCK "VarFileInfo"
  BEGIN                                // Language | Translation
    VALUE "Translation", 0x000, 0x000  // 
  END
END
| $< 
###
###
###
### HELP
###
viewhelp: help
	winhlp32 $(BINARY)\$(__FILE_NAME).hlp

help: $(__FILE_NAME).hlp $(__FILE_NAME).cnt	
# s/__TITLE__/$(__CODE_NAME) $(__VERSION_MAJOR).$(__VERSION_MINOR)/g
$(__FILE_NAME).cnt: $(SOURCE)\contents.cnt
	sed "s/FILE.HLP/$(__FILE_NAME).hlp/g" < $(SOURCE)\contents.cnt > $< # $(BINARY)\$(__FILE_NAME).cnt

$(__FILE_NAME).hlp: $(TMP)\help.hpj $(SOURCE)\caratula.rtf $(SOURCE)\topicos.rtf
	HCW /C /E /M $(TMP)\help.hpj 
###
###
### ESTO GENERA EL .HPJ
###
$(TMP)\help.hpj: #makefile
    sed "s/@/#/g" < &&|
; This file is maintained by HCW. Do not modify this file directly. (JA!JA!)

[OPTIONS]
HCW=0
COMPRESS=12 Hall Zeck
ERRORLOG=$(TMP)\help.err
LCID=0x2c0a 0x2 0x0 ;Spanish (Argentina)
REPORT=Yes
TITLE=$(__CODE_NAME) $(__VERSION_MAJOR).$(__VERSION_MINOR) Introduccón y Referencia
CNT=$(__FILE_NAME).cnt
COPYRIGHT=$(__LEGAL_STR)
TMPDIR=$(TMP)
BMROOT=.
ROOT=.
HLP=$(BINARY)\$(__FILE_NAME).hlp

[FILES]
$(SOURCE)\caratula.rtf
$(SOURCE)\topicos.rtf

[MAP]
@include $(SOURCE)\hlpsym.h

[WINDOWS]
main="$(__CODE_NAME) $(__VERSION_MAJOR).$(__VERSION_MINOR) Introducción y Referencia",(316,65,701,868),19460,,(r8454143); Ventana Principal del Help
ref="Navegador",(87,65,229,129),4,(r14876671),(r12632256),f2

[CONFIG]
CreateButton(100, "Session", JumpId($(__FILE_NAME).hlp, IDH_SESSION))
CreateButton(102, "Cursor",  JumpId($(__FILE_NAME).hlp, IDH_CURSOR))
CreateButton(104, "Param",   JumpId($(__FILE_NAME).hlp, IDH_PARAM))
CreateButton(106, "Column",  JumpId($(__FILE_NAME).hlp, IDH_COLUMN)) 
BrowseButtons ()
| > $< #$(TMP)\help.hpj 
###
###
###
### INSTALLEXE
###
install: $(BINARY)\$(__FILE_NAME).exe

###
### SE
###
$(BINARY)\$(__FILE_NAME).exe: $(__FILE_NAME).zip #makefile
	$(ZIPSE) $(TMP)\$(__FILE_NAME).zip -t &&|
$(__CODE_NAME) $(__VERSION)
$(__LEGAL_STR)
| @&&|
-win32
-i $(SOURCE)\dbo21.ico
-st "Instalar $(__CODE_NAME) $(__VERSION)"
-setup
-le
#-auto
-o
-c "rundll32 setupapi,InstallHinfSection DoIt 132 .\$(__FILE_NAME).inf"
| 
	copy $(TMP)\$(__FILE_NAME).exe $(BINARY)\$(__FILE_NAME).exe
	del  /Q /F $(TMP)\$(__FILE_NAME).exe
###
###
### ZIP
###
$(__FILE_NAME).zip: $(__FILE_NAME).dll $(__FILE_NAME).hlp $(__FILE_NAME).cnt $(__FILE_NAME).inf
	tdstrp32 $(BINARY)\$(__FILE_NAME).dll
	del /Q /F $(TMP)\$(__FILE_NAME).zip
	$(ZIP) -min -a -ex $<  @&&|
$(BINARY)\$(__FILE_NAME).dll 
$(BINARY)\$(__FILE_NAME).hlp 
$(BINARY)\$(__FILE_NAME).cnt
$(BINARY)\$(__FILE_NAME).inf 
|

###
###
### .INF
###
$(__FILE_NAME).inf: #makefile
	copy &&|
;;;
;;; $(__FILE_NAME).inf
;;;
;;; Instalación/Desinstalacíon para el $(__CODE_NAME) $(__VERSION)
;;;
;;; $(__LEGAL_STR)
;;;
[version]
Signature="$$Windows NT$" 				;"$$CHICAGO$"
Provider=%RODABLO%

;;;
;;; DIRS
;;;
[DestinationDirs]
InfDirFiles = 17                       			; 17 - \Windows\INF directory
AppDirFiles = 24, %PROGRAMFILESDIR%\%DISP_NAME%\  	; 24 - LDID_APPS\Program Files\

;;; Repite DefaultInstall para que le alcance la opcion -c al  selfextractor 
;;;
[DoIt]
Copyfiles = AppDirFiles, InfDirFiles
AddReg    = AddRegSection

[DefaultInstall]
Copyfiles = AppDirFiles, InfDirFiles
AddReg    = AddRegSection

[Uninstall]
DelFiles = AppDirFiles, InfDirFiles
DelReg   = DelRegSection

;;;
;;; FILES
;;;
[AppDirFiles]
%FILE_NAME%.dll
%FILE_NAME%.hlp
%FILE_NAME%.cnt

[InfDirFiles]
%FILE_NAME%.inf

;;;
;;; REGISTRY
;;;
[AddRegSection]
;;; Registra el servidor
HKCR,$(__PROGID).Session,,,%SHORT_DISP_NAME%
HKCR,$(__PROGID).Session\Clsid,,,%CLSID%

HKCR,CLSID\%CLSID%,,,%SHORT_DISP_NAME%
HKCR,CLSID\%CLSID%\InProcServer32,,,%24%\%PROGRAMFILESDIR%\%DISP_NAME%\%FILE_NAME%.dll
HKCR,CLSID\%CLSID%\ProgId,,,$(__PROGID).Session
HKCR,CLSID\%CLSID%\Programmable,,,
HKCR,CLSID\%CLSID%\TypeLib,,,%LIBID%
HKCR,CLSID\%CLSID%\Version,,,$(__PROGID).Session
HKCR,CLSID\%CLSID%\VersionIndependentProgID,,,$(__PROGID).Session

HKCR,TypeLib\%LIBID%\$(__VERSION_MAJOR).$(__VERSION_MINOR),,,%SHORT_DISP_NAME%
HKCR,TypeLib\%LIBID%\$(__VERSION_MAJOR).$(__VERSION_MINOR)\FLAGS,,,0
HKCR,TypeLib\%LIBID%\$(__VERSION_MAJOR).$(__VERSION_MINOR)\HELPDIR,,,%24%\%PROGRAMFILESDIR%\%DISP_NAME%\%FILE_NAME%.hlp
HKCR,TypeLib\%LIBID%\$(__VERSION_MAJOR).$(__VERSION_MINOR)\0\win32,,,%24%\%PROGRAMFILESDIR%\%DISP_NAME%\%FILE_NAME%.dll

;;; Registra la desinstalación
HKLM,%APPUNINSTALLKEY%\%FILE_NAME%,"DisplayName",,"%DISP_NAME%"
HKLM,%APPUNINSTALLKEY%\%FILE_NAME%,"UninstallString",,"RunDll32 advpack.dll,LaunchINFSection %17%\%FILE_NAME%.inf, Uninstall"

;;; Registra la licencia

[DelRegSection]
;;; Remueve el servidor
HKCR,$(__PROGID).Session
HKCR,CLSID\%CLSID%
HKCR,TypeLib\%LIBID%
HKCR,Interface\%CLSID%

;;; Remueve la desinstalación
HKLM,%APPUNINSTALLKEY%\%FILE_NAME%,

;;; Remueve la licencia

;;;
;;; Source file information
;;;
[SourceDisksNames.x86]
1 = %DiskId%,,,""

[SourceDisksFiles]
%FILE_NAME%.dll = 1 
%FILE_NAME%.hlp = 1
%FILE_NAME%.cnt = 1
%FILE_NAME%.inf = 1

;;;
;;; STRINGS 
;;; Este area hace de vínculo con el makefile
;;;

[Strings]
;;; Dirs 
PROGRAMFILESDIR = "Archivos de programa"
;;; Keys 
APPUNINSTALLKEY = "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall"
;;; Varios
DISKID          = "$(__CODE_NAME) $(__VERSION_MAJOR).$(__VERSION_MINOR) - Disco de instalacíon"
DISP_NAME       = "$(__CODE_NAME) $(__VERSION)"       
SHORT_DISP_NAME = "$(__CODE_NAME) $(__VERSION_MAJOR).$(__VERSION_MINOR)"       
FILE_NAME       = "$(__FILE_NAME)"
BASE_NAME       = $(__BASE_NAME)
RODABLO         = "RODABLO"
CLSID	        = {$(__UUID_CLSID)}
LIBID	        = {$(__UUID_LIBID)}
| $< 

###
###
### DEBUG
###
debug:  $(__FILE_NAME).dll
	copy $(BINARY)\$(__FILE_NAME).dll "c:\Archivos de Programa\$(__CODE_NAME) $(__VERSION)\$(__FILE_NAME).dll"
	copy $(BINARY)\$(__FILE_NAME).pdb "c:\Archivos de Programa\$(__CODE_NAME) $(__VERSION)\$(__FILE_NAME).pdb"

	td32.exe -sd$(SOURCE) -t$(BINARY) -c$(SOURCE)\dbo20.td2 "c:\Archivos de Programa\DevStudio\vb\vb5.exe" d:\vb\dbo20.vbp
###
###
### CLEAR
###     
###
clear:
	del /Q /F /S $(TMP)\*.*	
	del /Q /F    $(BINARY)\licensefactory.exe
	del /Q /F    $(BINARY)\$(__FILE_NAME).dll
	del /Q /F    $(BINARY)\$(__FILE_NAME).hlp
	del /Q /F    $(BINARY)\$(__FILE_NAME).cnt
	del /Q /F    $(BINARY)\$(__FILE_NAME).inf
	del /Q /F    $(BINARY)\$(__FILE_NAME).gid
###
###	
### LICENSEFACTORY
###
licensefactory: licensefactory.exe


licensefactory.exe: licensefactory.cpp 
	$(BCC32) -P @&&|
-H                              
-Hc                             
-H=$(TMP)\$&.csm      		
-o$(TMP)\$&.obj 
-e$@  
-I$(BCROOT)\include;c:\mstools\include
-L.\lib;$(BCROOT)\lib 
| $** oleaut32.lib advapi32.lib
###
###

