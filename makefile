.AUTODEPEND
.NOSWAP

__VERSION_MAJOR = 2
__VERSION_MINOR = 1
__VERSION_BUILD = 65
__VERSION       = $(__VERSION_MAJOR).$(__VERSION_MINOR).$(__VERSION_BUILD)

!if $d(ALTERNATE)
__ALTERNATE   = 1
__BASE_NAME   = ALTERNATE
__FILE_NAME   = alternate
__INST_NAME   = alternate-$(__VERSION)
!else
__ALTERNATE = 0
__CODE_NAME = DBO
__BASE_NAME = $(__CODE_NAME)$(__VERSION_MAJOR)$(__VERSION_MINOR)
__FILE_NAME = dbo21
__INST_NAME = dbo-$(__VERSION)
!endif

###
### TOOLS...
###
IMPLIB  = Implib
BCC32   = Bcc32 
BCC32I  = Bcc32i 
TLINK32 = TLink32
TLIB    = TLib
BRC32   = Brc32
TASM32  = Tasm32
MC      = c:\mstools\bin\mc.exe  
MIDL    = c:\mstools\bin\midl.exe
ZIP     = "c:\Archivos de programa\WinZip\WINZIP32.EXE"

###
### Paths
###
TMP       = c:\temp\object
BINARY    = c:\z\bin
SOURCE    = d:\work
TMP2      = $(TMP)\\
EMPTY     = 

.path.obj=$(TMP)
.path.cpp=$(SOURCE)
.path.res=$(TMP)
.path.zip=$(TMP)
.path.dll=$(BINARY)
.path.hlp=$(BINARY)
.path.cnt=$(BINARY)
.path.inf=$(BINARY)

###
###
###
###
###
### here link la DLL     
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
$(__FILE_NAME).dll: $(.path.obj)\dbomc.hxx $(.path.obj)\dboidl.hxx $(.path.obj)\dboidl.obj $(LINKOBJS:ZZ=$(EMPTY)) dbo.res dbo.def makefile
   $(TLINK32) -Tpd -aa -L.\LIB;C:\BC5\LIB;C:\ORANT\OCI73\LIB\BORLAND; -v -c -n -V4.0 -w-dup -x @&&|
.\lib\c0d32dyn.obj+
$(TMP)\dboidl.obj+
$(LINKOBJS:ZZ=$(TMP2)), $(BINARY)\$(__FILE_NAME).dll, $(TMP)\$(__FILE_NAME).map,+
#cg32.lib+
cw32.lib+
#cw32mt.lib+
#C:\BC5\LIB\bidsf.lib+
oleaut32.lib advapi32.lib ole2w32.lib import32.lib+
#.\crypto\rsa.lib+
#d:\work\lib\
ociw32.lib, dbo.def, $(TMP)\dbo.res
|
#    tdstrp32 $(BINARY)\dbo20.dll
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
#$(TMP)\dboidl.hxx dboidl.tlb: dboidl.idl # Ver multiples targets en el make

$(TMP)\dboidl.hxx: dboidl.idl # Ver multiples targets en el make
#$(TMP)\dboidl.c: dboidl.idl # Ver multiples targets en el make
    $(MIDL) @&&|        
/ms_ext /char unsigned /nologo /win32 /D __COMPAT00__=0
/D __ALTERNATE=$(__ALTERNATE)
/D __BASE_NAME=$(__BASE_NAME) 
/D __FILE_NAME=$(__FILE_NAME) 
/I c:\mstools\include /tlb $*.tlb /iid $*.c /h $< dboidl.idl
|

$(TMP)\dboidl.obj: $(TMP)\dboidl.c
    $(BCC32) -P- -c -o$@ $(TMP)\dboidl.c
###
###
###
### CPP's & opciones
###
.cpp.obj:
    BCC32 -c +&&|
-y                                 #
-H                                 #
-Hc                                #
-H=$(TMP)\$(__FILE_NAME).csm       #   
-H"PCH.HXX"
-I$(TMP) 
-IC:\BC5\INCLUDE
-IC:\MSTOOLS\INCLUDE
-IC:\ORANT\OCI73\INCLUDE 
-I.\CRYPTO
-DINC_OLE2;STRICT;_DEBUG 
-D__ALTERNATE=$(__ALTERNATE)
-D__DBO2_BUILD_NUMBER__=$(__VERSION_BUILD)
-D__BASE_NAME=$(__BASE_NAME)
-D__FILE_NAME=$(__FILE_NAME)
#-DRWQE_STANDARD_LIBRARY           # ????
#-DRWSTD_NO_NAMESPACE              # si saco los namespaces no linkea los string
#-DRWSTD_NO_NEW_HEADER             # .....
##-DTHREAD=SINGLE
#-DRWSTD_NO_WSTR=0
-DRWDEBUG=1   			   # ¿¿¡¡ Atención !!??
#-D__USING_STD_NAMES__             #
#-D__STD_STRING                      
#-N                                 # Check stack overflow 
-tWDE                              # Windows DLL, explicit functions exported
-ff-                               # ANSI float
-x                                 # enable exceptions
#-xp                               # Enable exception location information
-xd                                # Enable destructor cleanup
-xds                               # Enable throwing exceptions from a DLL
-RT                                # Enable run-time type information
-d                                 # Duplicate strings merged
-r                                 # Register variables
-X-                                # Autodependency information
-u                                 # Generate underscores
-C                                 # nested comments
-A- 
-AT                                # borland extensions
-w                                 # all warnings
-R                                 # browse info in obj
-v                                 # turns debugging on and inline expansion off
-Od                                # Disable all optimizations
-Jg                                # Smart Templates
-5                                 # i386 pentium i486  
#-vGd 
#-vGt
-wcln -wsig -wucp -w-par -w-pch- -wstv -wasm -wamp -wobs -wpch -wdef -wnod -wamb -wbbf -wpin -wnak
| -o$@ $. 
###
###
### .RES
### RECURSOS WINDOWS 
###
#$(TMP)\dbo.res :  dbo.rc dboidl.tlb $(TMP)\MSG00000.bin
#$(.path.res)\
dbo.res: dbo.rc $(TMP)\dboidl.hxx $(TMP)\dbomc.hxx $(TMP)\versioninfo.rc
    $(BRC32) -R @&&|
    -I$(.path.obj);C:\BC5\INCLUDE; 
    -DINC_OLE2;STRICT;_DEBUG;  
    -FO$@ dbo.rc               
|

$(TMP)\versioninfo.rc:	makefile
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
      VALUE "FileDescription", "DBO2\0"
      VALUE "FileVersion", "$(__VERSION)\0"
      VALUE "InternalName","$(__FILE_NAME)\0"
!if __ALTERNATE
      VALUE "LegalCopyright", ""
!else
      VALUE "LegalCopyright", "(1998) RODABLO para Origenes A.F.J.P.\0"
!endif
      VALUE "ProductName", "DBO2\0"
      VALUE "ProductVersion", "$(__VERSION_MAJOR).$(__VERSION_MINOR)\0"
      VALUE "Build", "$(__VERSION_BUILD)\0"
    END	             
  END
  BLOCK "VarFileInfo"
  BEGIN                                // Language | Translation
    VALUE "Translation", 0x000, 0x000  // 
  END
END
| $(TMP)\versioninfo.rc
###
###
###
### HELP
###
viewhelp: help
	winhlp32 $(BINARY)\$(__FILE_NAME).hlp

help: $(__FILE_NAME).hlp $(__FILE_NAME).cnt	

$(__FILE_NAME).cnt: $(SOURCE)\contents.cnt
	copy $(SOURCE)\contents.cnt $(BINARY)\$(__FILE_NAME).cnt

$(__FILE_NAME).hlp: $(TMP)\help.hpj $(SOURCE)\caratula.rtf $(SOURCE)\topicos.rtf
	HCW /C /E /M $(TMP)\help.hpj 
###
###
### ESTO GENERA EL .HPJ
###
$(TMP)\help.hpj: makefile
    copy &&|
; This file is maintained by HCW. Do not modify this file directly. (JA!JA!)

[OPTIONS]
HCW=0
COMPRESS=12 Hall Zeck
ERRORLOG=$(TMP)\help.err
LCID=0x2c0a 0x2 0x0 ;Spanish (Argentina)
REPORT=Yes
TITLE=DBO 2.1 Introduccón y Referencia
CNT=$(__FILE_NAME).cnt
COPYRIGHT=(1998) RODABLO para Origenes A.F.J.P.
TMPDIR=$(TMP)
BMROOT=.
ROOT=.
HLP=$(BINARY)\$(__FILE_NAME).hlp

[FILES]
$(SOURCE)\caratula.rtf
$(SOURCE)\topicos.rtf

[MAP]
#include .\hlpsym.h

[WINDOWS]
main="DBO 2.1 Introducción y Referencia",(316,65,701,868),19460,,(r8454143); Ventana Principal del Help
ref="Navegador",(87,65,229,129),4,(r14876671),(r12632256),f2

[CONFIG]
CreateButton(100, "Session", JumpId($(__FILE_NAME).hlp, IDH_SESSION))
CreateButton(102, "Cursor",  JumpId($(__FILE_NAME).hlp, IDH_CURSOR))
CreateButton(104, "Param",   JumpId($(__FILE_NAME).hlp, IDH_PARAM))
CreateButton(106, "Column",  JumpId($(__FILE_NAME).hlp, IDH_COLUMN)) 
BrowseButtons ()
| $(TMP)\help.hpj
###
###
###
### INSTALLEXE
###
install: $(BINARY)\$(__INST_NAME).exe

###
### SE
###
$(BINARY)\$(__INST_NAME).exe: $(__INST_NAME).zip makefile
	winzipse $(TMP)\$(__INST_NAME).zip -t &&|
DBO $(__VERSION)
©1998 RODABLO para OriGenes A.F.J.P
| @&&|
-win32
-i $(SOURCE)\dbo21.ico
-st "Instalar DBO $(__VERSION)"
-setup
-le
#-auto
-o
-c "rundll32 setupapi,InstallHinfSection DoIt 132 .\$(__INST_NAME).inf"
| 
	copy $(TMP)\$(__INST_NAME).exe $(BINARY)\$(__INST_NAME).exe
	del  /Q /F $(TMP)\$(__INST_NAME).exe
###
###
### ZIP
###
$(__INST_NAME).zip: $(__FILE_NAME).dll $(__FILE_NAME).hlp $(__FILE_NAME).cnt $(__INST_NAME).inf
	tdstrp32 $(BINARY)\$(__FILE_NAME).dll
	del /Q /F $(TMP)\$(__INST_NAME).zip
	$(ZIP) -min -a -ex $(TMP)\$(__INST_NAME).zip @&&|
$(BINARY)\$(__FILE_NAME).dll 
$(BINARY)\$(__FILE_NAME).hlp 
$(BINARY)\$(__FILE_NAME).cnt
$(BINARY)\$(__INST_NAME).inf 
|

###
###
### .INF
###
$(__INST_NAME).inf: makefile
	copy &&|
;;;
;;; $(__INST_NAME).inf
;;;
;;; Instalación/Desinstalacíon para el dbo $(__VERSION)
;;;
;;; ©1998 RODABLO
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
%INST_NAME%.inf

;;;
;;; REGISTRY
;;;
[AddRegSection]
;;; Registra el servidor
HKCR,DBO21.Session,,,%SHORT_DISP_NAME%
HKCR,DBO21.Session\Clsid,,,%CLSID%

HKCR,CLSID\%CLSID%,,,%SHORT_DISP_NAME%
HKCR,CLSID\%CLSID%\InProcServer32,,,%24%\%PROGRAMFILESDIR%\%DISP_NAME%\%FILE_NAME%.dll
HKCR,CLSID\%CLSID%\ProgId,,,DBO21.Session
HKCR,CLSID\%CLSID%\Programmable,,,
HKCR,CLSID\%CLSID%\TypeLib,,,%LIBID%
HKCR,CLSID\%CLSID%\Version,,,DBO21.Session
HKCR,CLSID\%CLSID%\VersionIndependentProgID,,,DBO21.Session

HKCR,TypeLib\%LIBID%\2.1,,,%SHORT_DISP_NAME%
HKCR,TypeLib\%LIBID%\2.1\FLAGS,,,0
HKCR,TypeLib\%LIBID%\2.1\HELPDIR,,,%24%\%PROGRAMFILESDIR%\%DISP_NAME%\%FILE_NAME%.hlp
HKCR,TypeLib\%LIBID%\2.1\0\win32,,,%24%\%PROGRAMFILESDIR%\%DISP_NAME%\%FILE_NAME%.dll

;;; Registra la desinstalación
HKLM,%APPUNINSTALLKEY%\%INST_NAME%,"DisplayName",,"%DISP_NAME%"
HKLM,%APPUNINSTALLKEY%\%INST_NAME%,"UninstallString",,"RunDll32 advpack.dll,LaunchINFSection %17%\%INST_NAME%.inf, Uninstall"

;;; Registra la licencia

[DelRegSection]
;;; Remueve el servidor
HKCR,DBO21.Session
HKCR,CLSID\%CLSID%
HKCR,TypeLib\%LIBID%
HKCR,Interface\%CLSID%

;;; Remueve la desinstalación
HKLM,%APPUNINSTALLKEY%\%INST_NAME%,

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
%INST_NAME%.inf = 1

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
DISKID          = "DBO 2.1 - Disco de instalacíon"
DISP_NAME       = "DBO $(__VERSION)"       
SHORT_DISP_NAME = "DBO $(__VERSION_MAJOR).$(__VERSION_MINOR)"       
INST_NAME       = "$(__INST_NAME)"
FILE_NAME       = "dbo21"
BASE_NAME       = "DBO21"
RODABLO         = "RODABLO"
CLSID	        = "{CA43D287-EF82-11cf-B8C6-00A02454EEC8}"
LIBID	        = "{CA43D288-EF82-11cf-B8C6-00A02454EEC8}"
| $(BINARY)\$(__INST_NAME).inf

###
###
### DEBUG
###
debug:  $(__FILE_NAME).dll
	td32.exe -sd$(SOURCE) -t$(BINARY) -c$(SOURCE)\dbo20.td2 "c:\Archivos de Programa\DevStudio\vb\vb5.exe" d:\z\xxx\vb\dbo20.vbp
###
###
### CLEAR
###     
###
clear:
	del /Q /F /S $(TMP)\*.*	
	del /Q /F    $(BINARY)\$(__FILE_NAME).*
	del /Q /F    $(BINARY)\$(__INST_NAME).inf
###
###	






