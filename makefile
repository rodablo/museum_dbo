.AUTODEPEND
.NOSWAP

__DBO2_BUILD_NUMBER__ = 59
__DBO2_BUILD_NUMBER_S_= "0059\0"

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

###
### Paths
###
TMP  = c:\temp\object
BINARY = c:\z\bin
SOURCE = d:\work
TMP2 = $(TMP)\\
EMPTY  = 

.path.obj=$(TMP)
.path.cpp=$(SOURCE)
.path.res=$(TMP)
.path.dll=$(BINARY)

####################################################################################################
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
dbo20.dll: $(.path.obj)\dbomc.hxx $(.path.obj)\dboidl.hxx $(.path.obj)\dboidl.obj $(LINKOBJS:ZZ=$(EMPTY)) dbo.res dbo.def makefile
   $(TLINK32) -Tpd -aa -L.\LIB;C:\BC5\LIB;C:\ORANT\OCI73\LIB\BORLAND; -v -c -n -V4.0 -w-dup -x @&&|
.\lib\c0d32dyn.obj+
$(TMP)\dboidl.obj+
$(LINKOBJS:ZZ=$(TMP2)), $(BINARY)\dbo20.dll, $(TMP)\dbo20.map,+
#cg32.lib+
cw32.lib+
#cw32mt.lib+
#C:\BC5\LIB\bidsf.lib+
oleaut32.lib advapi32.lib ole2w32.lib import32.lib+
ociw32.lib, dbo.def, $(TMP)\dbo.res
|
#    tdstrp32 $(BINARY)\dbo20.dll

####################################################################################################
###
### ERRORES WIN32
###
$(TMP)\dbomc.hxx: dbomc.mc #$(TMP)\dbomc.hxx $(TMP)\MSG00000.bin: dbomc.mc
    $(MC) -o -v -s -e hxx -h $(TMP)\ -r $(TMP)\ dbomc.mc

####################################################################################################
###
### IDL
###
#$(TMP)\dboidl.hxx dboidl.tlb: dboidl.idl # Ver multiples targets en el make

$(TMP)\dboidl.hxx: dboidl.idl # Ver multiples targets en el make
#$(TMP)\dboidl.c: dboidl.idl # Ver multiples targets en el make
    $(MIDL) @&&|        
/ms_ext /char unsigned /nologo /win32 /D __COMPAT00__=0
/I c:\mstools\include /tlb $*.tlb /iid $*.c /h $< dboidl.idl
|

$(TMP)\dboidl.obj: $(TMP)\dboidl.c
    $(BCC32) -P- -c -o$@ $(TMP)\dboidl.c

####################################################################################################
###
### CPP's & opciones
###
.cpp.obj:
    BCC32 -c +&&|
-y                                 #
-H                                 #
-Hc                                #
-H=$(TMP)\DBO20.CSM                #   
-H"PCH.HXX"
-I$(TMP) 
-IC:\BC5\INCLUDE
-IC:\MSTOOLS\INCLUDE
-IC:\ORANT\OCI73\INCLUDE 
-DINC_OLE2;STRICT;_DEBUG 
-D__DBO2_BUILD_NUMBER__=$(__DBO2_BUILD_NUMBER__)
#-DRWQE_STANDARD_LIBRARY           # ????
#-DRWSTD_NO_NAMESPACE              # si saco los namespaces no linkea los string
#-DRWSTD_NO_NEW_HEADER             # .....
##-DTHREAD=SINGLE
#-DRWSTD_NO_WSTR=0
-DRWDEBUG=1   
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

####################################################################################################
###
### RECURSOS WINDOWS 
###
#$(TMP)\dbo.res :  dbo.rc dboidl.tlb $(TMP)\MSG00000.bin
#$(.path.res)\
dbo.res: dbo.rc $(TMP)\dboidl.hxx $(TMP)\dbomc.hxx 
    $(BRC32) -R @&&|
    -I$(.path.obj);C:\BC5\INCLUDE; 
    -DINC_OLE2;STRICT;_DEBUG;  
    -d__DBO2_BUILD_NUMBER_S_=$(__DBO2_BUILD_NUMBER_S__)	
    -FO$@ dbo.rc               
|
####################################################################################################
###
### RELEASE
###

####################################################################################################
###
### DEBUG
###
debug:  dbo20.dll
        #td32.exe -sd$(SOURCE) -t$(BINARY) -c$(SOURCE)\dbo20.td2 "c:\Program Files\Microsoft Visual Basic\vb32.exe" $(SOURCE)\vb\dbo20.vbp      
        td32.exe -sd$(SOURCE) -t$(BINARY) -c$(SOURCE)\dbo20.td2 "c:\Archivos de Programa\DevStudio\vb\vb5.exe" d:\z\xxx\vb\dbo20.vbp

####################################################################################################
###
### CLEAR
###      
clear:
	del /Q $(TMP)\*.*	