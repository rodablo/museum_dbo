.AUTODEPEND
.NOSWAP

__VERSION_MAJOR = 2
__VERSION_MINOR = 1
__VERSION_BUILD = 71
__VERSION       = $(__VERSION_MAJOR).$(__VERSION_MINOR).$(__VERSION_BUILD)

!if $d(ALTERNATE)
__ALTERNATE   = 1
__BASE_NAME   = ALTERNATE
__FILE_NAME   = alternate
__INST_NAME   = alternate-$(__VERSION)
!else
__ALTERNATE   = 0
__BASE_NAME   = DBO21
__FILE_NAME   = dbo21
__INST_NAME   = dbo-$(__VERSION)
!endif

###
### TOOLS...
###
BCROOT  = k:\Borland\bc501
IMPLIB  = $(BCROOT)\bin\Implib
BCC32   = $(BCROOT)\bin\Bcc32 
BCC32I  = $(BCROOT)\bin\Bcc32i 
TLINK32 = $(BCROOT)\bin\TLink32
TLIB    = TLib
BRC32   = $(BCROOT)\bin\Brc32
TASM32  = Tasm32
MC      = k:\mstools\bin\mc.exe  
MIDL    = k:\mstools\bin\midl.exe
IS      = "c:\Program Files\InstallShield\InstallShield Express BC++ 5.02 Edition\isx.exe"
ZIP     = "c:\Program Files\WinZip\WINZIP32.EXE"
ZIPSE   = "c:\Program Files\WinZip Self-Extractor\Winzipse.exe"

###
### Paths
###
TMP       = c:\temp\object
BINARY    = c:\z\bin
SOURCE    = c:\z\work
TMP2      = $(TMP)\\
EMPTY     = 

.path.obj=$(TMP)
.path.cpp=$(SOURCE)
.path.res=$(TMP)
.path.dll=$(BINARY)
.path.hlp=$(BINARY)
.path.cnt=$(BINARY)

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
   $(TLINK32) -Tpd -aa -L.\LIB;$(BCROOT)\LIB;.\OCI73\LIB\BORLAND; -v -c -n -V4.0 -w-dup -x @&&|
.\lib\c0d32dyn.obj+
$(TMP)\dboidl.obj+
$(LINKOBJS:ZZ=$(TMP2)), $(BINARY)\$(__FILE_NAME).dll, $(TMP)\$(__FILE_NAME).map,+
#cg32.lib+
cw32.lib+
#cw32mt.lib+
#C:\BC5\LIB\bidsf.lib+
oleaut32.lib advapi32.lib ole2w32.lib import32.lib+
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
/D __ALTERNATE__=$(__ALTERNATE__)
/D __BASE_NAME=$(__BASE_NAME) 
/D __FILE_NAME=$(__FILE_NAME) 
/I k:\mstools\include /tlb $*.tlb /iid $*.c /h $< dboidl.idl
|

$(TMP)\dboidl.obj: $(TMP)\dboidl.c
    $(BCC32) -P- -c -o$@ $(TMP)\dboidl.c
###
###
###
### CPP's & opciones
###
.cpp.obj:
    $(BCC32) -c +&&|
-y                                 #
-H                                 #
-Hc                                #
-H=$(TMP)\$(__FILE_NAME).csm       #   
-H"PCH.HXX"
-I$(TMP) 
-I$(BCROOT)\INCLUDE
-IK:\MSTOOLS\INCLUDE
-I.\OCI73\INCLUDE 
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
    -I$(.path.obj);$(BCROOT)\INCLUDE; 
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
	k:\mstools\bin\HCW /C /E /M $(TMP)\help.hpj 
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
install: $(TMP)\$(__FILE_NAME).exe
	copy $(TMP)\$(__FILE_NAME).exe $(BINARY)\$(__INST_NAME).exe
###
###
### SE
###
$(TMP)\$(__FILE_NAME).exe: $(TMP)\$(__FILE_NAME).zip
	$(ZIPSE) $(TMP)\$(__FILE_NAME).zip @&&|
-i $(SOURCE)\dbo21.ico
-ishield3
-win32	
-le
-auto
|	
###
###
### ZIP
###
$(TMP)\$(__FILE_NAME).zip: $(TMP)\$(__FILE_NAME)\650mb\Disk1\_setup.1
#	PKZIP -ex $(TMP)\$(__FILE_NAME).zip $(TMP)\$(__FILE_NAME)\650mb\Disk1\*.*
	$(ZIP) -min -a -ex $< $(TMP)\$(__FILE_NAME)\650mb\Disk1\*.*
###
###
### IS
###
$(TMP)\$(__FILE_NAME)\650mb\Disk1\_setup.1: $(__FILE_NAME).dll $(__FILE_NAME).hlp $(__FILE_NAME).cnt
	tdstrp32 $(BINARY)\$(__FILE_NAME).dll
	copy &&|
[InstallShield Wizard]
iDate=1
iTime=1
Flag=0
ISX.EXE Size=668160
ISX.EXE Date=12.49.10  18/12/1996
ISX.EXE Ver=1.11.0.0
SETUP.EXE Size=44928
SETUP.EXE Date=13.04.12  4/11/1996
SETUP.EXE Ver=3.0.107.0
SETUP.INS Size=66760
SETUP.INS Date=17.50.16  7/3/1997
SETUP.INS Ver=Not available
_INST16.EX_ Size=66760
_INST16.EX_ Date=17.50.16  7/3/1997
_INST16.EX_ Ver=Not available
_INST32I.EX_ Size=320276
_INST32I.EX_ Date=16.17.32  5/11/1996
_INST32I.EX_ Ver=Not available
ISDEPEND.INI Size=5102
ISDEPEND.INI Date=16.31.20  11/6/1996
ISDEPEND.INI Ver=Not available
SWDEPEND.INI Size=4605
SWDEPEND.INI Date=1.12.52  12/3/1997
SWDEPEND.INI Ver=Not available
ICOMP.EXE Size=119808
ICOMP.EXE Date=15.05.10  15/1/1996
ICOMP.EXE Ver=3.00.062
SPLIT.EXE Size=90624
SPLIT.EXE Date=15.09.36  15/1/1996
SPLIT.EXE Ver=3.00.060
PACKLIST.EXE Size=87552
PACKLIST.EXE Date=15.10.30  15/1/1996
PACKLIST.EXE Ver=3.00.060
Version=1.11a
DevTool=for Borland C++ 5.0
Platform=Win32
PtrBase=38200
PtrPosY=73
PtrPage=0
DisksBuilt=1
DisksDir=dbo21\650MB\
TabsVisit=000000000000000000
LangNum=0
[VisualDesign]
AppName=DBO $(__VERSION)
Version=$(__VERSION)
Company=RODABLO
Title=DBO $(__VERSION)
TitleType=1
BackgrndAlign=2
Backgrnd=1
BackgrndColor=0
Uninstall=1
Silent=1
SmsMode=0
[RegEntries]
Reg1Path=HKEY_CLASSES_ROOT
Reg1Val1Type=0
Reg1Val1Name=(Default)
Reg1Val1Data=(value not set)
Reg1Vals=1
Reg2Path=HKEY_CURRENT_USER
Reg2Val1Type=0
Reg2Val1Name=(Default)
Reg2Val1Data=(value not set)
Reg2Vals=1
Reg3Path=HKEY_LOCAL_MACHINE
Reg3Val1Type=0
Reg3Val1Name=(Default)
Reg3Val1Data=(value not set)
Reg3Vals=1
Reg4Path=HKEY_USERS
Reg4Val1Type=0
Reg4Val1Name=(Default)
Reg4Val1Data=(value not set)
Reg4Vals=1
Reg5Path=HKEY_CURRENT_CONFIG
Reg5Val1Type=0
Reg5Val1Name=(Default)
Reg5Val1Data=(value not set)
Reg5Vals=1
Reg6Path=HKEY_DYN_DATA
Reg6Val1Type=0
Reg6Val1Name=(Default)
Reg6Val1Data=(value not set)
Reg6Vals=1
Reg7Path=HKEY_CLASSES_ROOT\CLSID
Reg7Val1Type=0
Reg7Val1Name=(Default)
Reg7Val1Data=(value not set)
Reg7Vals=1
Reg8Path=HKEY_CLASSES_ROOT\CLSID\{CA43D287-EF82-11cf-B8C6-00A02454EEC8}
Reg8PathUninstall=1
Reg8Val1Type=0
Reg8Val1Name=(Default)
Reg8Val1Data=DBO $(__VERSION_MAJOR).$(__VERSION_MINOR)
Reg8Vals=1
Reg9Path=HKEY_CLASSES_ROOT\CLSID\{CA43D287-EF82-11cf-B8C6-00A02454EEC8}\InProcServer32
Reg9Val1Type=0
Reg9Val1Name=(Default)
Reg9Val1Data=<INSTALLDIR>\dbo21.dll
Reg9Vals=1
Reg10Path=HKEY_CLASSES_ROOT\CLSID\{CA43D287-EF82-11cf-B8C6-00A02454EEC8}\ProgID
Reg10Val1Type=0
Reg10Val1Name=(Default)
Reg10Val1Data=DBO21.Session
Reg10Vals=1
Reg11Path=HKEY_CLASSES_ROOT\CLSID\{CA43D287-EF82-11cf-B8C6-00A02454EEC8}\Programmable
Reg11Val1Type=0
Reg11Val1Name=(Default)
Reg11Val1Data=(value not set)
Reg11Vals=1
Reg12Path=HKEY_CLASSES_ROOT\CLSID\{CA43D287-EF82-11cf-B8C6-00A02454EEC8}\TypeLib
Reg12Val1Type=0
Reg12Val1Name=(Default)
Reg12Val1Data={CA43D288-EF82-11cf-B8C6-00A02454EEC8}
Reg12Vals=1
Reg13Path=HKEY_CLASSES_ROOT\CLSID\{CA43D287-EF82-11cf-B8C6-00A02454EEC8}\Version
Reg13Val1Type=0
Reg13Val1Name=(Default)
Reg13Val1Data=2.1
Reg13Vals=1
Reg14Path=HKEY_CLASSES_ROOT\CLSID\{CA43D287-EF82-11cf-B8C6-00A02454EEC8}\VersionIndependentProgID
Reg14Val1Type=0
Reg14Val1Name=(Default)
Reg14Val1Data=DBO21.Session
Reg14Vals=1
Reg15Path=HKEY_CLASSES_ROOT\DBO21.Session
Reg15PathUninstall=1
Reg15Val1Type=0
Reg15Val1Name=(Default)
Reg15Val1Data=DBO $(__VERSION_MAJOR).$(__VERSION_MINOR)
Reg15Vals=1
Reg16Path=HKEY_CLASSES_ROOT\DBO21.Session\Clsid
Reg16Val1Type=0
Reg16Val1Name=(Default)
Reg16Val1Data={CA43D287-EF82-11cf-B8C6-00A02454EEC8}
Reg16Vals=1
Reg17Path=HKEY_CLASSES_ROOT\TypeLib
Reg17Val1Type=0
Reg17Val1Name=(Default)
Reg17Val1Data=(value not set)
Reg17Vals=1
Reg18Path=HKEY_CLASSES_ROOT\TypeLib\{CA43D288-EF82-11cf-B8C6-00A02454EEC8}
Reg18PathUninstall=1
Reg18Val1Type=0
Reg18Val1Name=(Default)
Reg18Val1Data=(value not set)
Reg18Vals=1
Reg19Path=HKEY_CLASSES_ROOT\TypeLib\{CA43D288-EF82-11cf-B8C6-00A02454EEC8}\2.1
Reg19Val1Type=0
Reg19Val1Name=(Default)
Reg19Val1Data=DBO $(__VERSION_MAJOR).$(__VERSION_MINOR)
Reg19Vals=1
Reg20Path=HKEY_CLASSES_ROOT\TypeLib\{CA43D288-EF82-11cf-B8C6-00A02454EEC8}\2.1\0
Reg20Val1Type=0
Reg20Val1Name=(Default)
Reg20Val1Data=(value not set)
Reg20Vals=1
Reg21Path=HKEY_CLASSES_ROOT\TypeLib\{CA43D288-EF82-11cf-B8C6-00A02454EEC8}\2.1\0\win32
Reg21Val1Type=0
Reg21Val1Name=(Default)
Reg21Val1Data=<INSTALLDIR>\dbo21.dll
Reg21Vals=1
Reg22Path=HKEY_CLASSES_ROOT\TypeLib\{CA43D288-EF82-11cf-B8C6-00A02454EEC8}\2.1\FLAGS
Reg22Val1Type=0
Reg22Val1Name=(Default)
Reg22Val1Data=0
Reg22Vals=1
Reg23Path=HKEY_CLASSES_ROOT\TypeLib\{CA43D288-EF82-11cf-B8C6-00A02454EEC8}\2.1\HELPDIR
Reg23Val1Type=0
Reg23Val1Name=(Default)
Reg23Val1Data=<INSTALLDIR>\dbo21.hlp
Reg23Vals=1
Regs=23
[Groups]
Groups=2
Group1Size=1886030
Group1Files=2
Group1Name=Dll
Group1Dir=<INSTALLDIR>
Group1File1=C:\z\bin\dbo21.dll
Group1File2=$(SOURCE)\license.reg
Group2Size=50643
Group2Files=2
Group2Name=Help
Group2Dir=<INSTALLDIR>
Group2File1=C:\z\bin\DBO21.HLP
Group2File2=c:\z\bin\dbo21.cnt
[Components]
Components=2
Component1Groups=1
Component1Name=DLL
Component1Description=Archivos requeridos para el funcionamiento.
Component1GroupList=1 
Component2Groups=1
Component2Name=Ayuda en linea
Component2Description=Archivos de la ayuda en linea.
Component2GroupList=2 
[Types]
Types=1
Type1Components=2
Type1Name=Complete 
Type1ComponentList=1 2 
[Sequence]
DestinationLocationDir=<WINDISK>\Archivos de programa\RODABLO\DBO $(__VERSION)
SelectProgramFolderName=Dbo21
SplashScreen=1
SplashScreenBmp=$(SOURCE)\is\splash.bmp
DestinationLocation=1
BeginFileTransfer=1
ProgressBar=1
SetupComplete=1
| $(TMP)\$(__FILE_NAME).iwz
	$(IS) $(TMP)\$(__FILE_NAME).iwz
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
###
###	






