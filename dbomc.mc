;/**
;*** DBO2(1997/1998)
;*** rodablo@hotmail.com
;***/
MessageIdTypedef=DWORD

SeverityNames=(
    Success=0x0:STATUS_SEVERITY_SUCCESS
    Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
    Warning=0x2:STATUS_SEVERITY_WARNING
    Error=0x3:STATUS_SEVERITY_ERROR
    )

FacilityNames=(
    System=0x0:FACILITY_SYSTEM
    ORACLE=0x1:FACILITY_DBO_ORACLE
    RUNTIME=0x5:FACILITY_DBO_RUNTIME
    INTERNAL=0x6:FACILITY_DBO_ERROR   ;no va mas
    )

LanguageNames=(Neutral=0x000:MSG00000)


;/**
;*** RUNTIME BEGIN ====================
;***/
MessageId=0x1
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_BEGIN
Language=Neutral
runtime begin.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PROP_TEMP_READONLY
Language=Neutral
Propiedad temporalmente READ/ONLY.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_CANT_MODIFY_ROW_AFTER_DEFINE
Language=Neutral
Propiedad <RowsXFetch> READ-ONLY luego del invocar <DefineColumns> o <Fetch>.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_SESSION_ACTIVE_CURSOR_EXIST_LU
Language=Neutral
Existe(n) <%1!lu!> cursor(es) activo(s).
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_INVALID_COL_LU
Language=Neutral
Parámetro Col <%1!lu!> inválido.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_MUST_BE_SELECT
Language=Neutral
Debe ser SELECT.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_MUST_BE_PARSED
Language=Neutral
Debe ser Parseado.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_BAD_COL_INDEX_L
Language=Neutral
Columna <%1!d!> Inexistente.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_BAD_COL_NAME_S
Language=Neutral
Columna <%1> Inexistente.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_BAD_PARAM_NUMBER_L
Language=Neutral
Parametro <%1!d!> Inexistente.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_BAD_PARAM_NAME_S
Language=Neutral
Parametro <%1> Inexistente.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_COL_INDEX_TYPE_MISMATCH_I
Language=Neutral
Tipo variant <%1!d!> no valido como Index.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_ROW_INDEX_OUT_OF_RANGE_L
Language=Neutral
Row Index <%1!l!> fuera de rango.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_INDEX_OUT_OF_RANGE_L
Language=Neutral
Index de Param <%1!l!> fuera de rango.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_NOT_BINDED_S
Language=Neutral
El párametro <%1> no esta ¿bindeado?.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_NAME_TYPE_MISMATCH_I
Language=Neutral
Tipo variant <%1!d!> no valido como nombre/número de parametro.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_INDEX_TYPE_MISMATCH_I
Language=Neutral
Tipo variant <%1!d!> no valido como index de parametro.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_VALUE_TYPE_MISMATCH_I_I
Language=Neutral
Param Type Missmatch, se espera Tipo Variant <%1!d!>, se encontro <%2!d!>.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_EXIST_I
Language=Neutral
Parametro <%1!d!> existe.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_EXIST_S
Language=Neutral
Parametro <%1> existe.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_STRING_TRUNCATION_I_I
Language=Neutral
El String asignado al Parámetro quedará ¿trunco?.
StringLength = <%1!d!>, se encontró <%2!d!>.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_CANT_DETERMINE_STRING_SIZE
Language=Neutral
No es posible determinar el tamaño del parametro string (defina Value o Length).
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_CANT_DETERMINE_ARRAY_SIZE
Language=Neutral
No es posible determinar el número de elementos del parámetro array.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_CANT_DETERMINE_VTYPE
Language=Neutral
No puedo determinar el Tipo Variant del parametro.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_TYPE_MISMATCH_I
Language=Neutral
Tipo Variant <%1!d!> no válido como tipo de parametro.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_ARRAYSIZE_MISMATCH_I
Language=Neutral
Tipo Variant <%1!d!> no válido en argumento ArraySize de un parametro.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_LENGTH_TYPE_MISMATCH_I
Language=Neutral
Tipo Variant <%1!d!> no válido en argumento Length de un parametro.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_INDEX_ONLY_IN_ARRAYS
Language=Neutral
Index válido solo en parámetros tipo Array.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_STRINGLENGTH_ONLY_IN_STRING
Language=Neutral
StringLength válido solo en parámetros tipo String.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_STRINGLENGTH_CANT_BE_ZERO
Language=Neutral
El parametro tipo String no puede tener longitud 0,
revise StringLength o Value.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_NEED_INDEX_IN_ARRAYS
Language=Neutral
Index es requerido en parámetros tipo Array.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_NUMERIC_CANT_BE_ARRAYS
Language=Neutral
Los parametros Numericos NO pueden ser Arrays.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PARAM_PROP_ONLY_IN_ARRAYS_S
Language=Neutral
Propiedad <%1> Solo válida en parámetros tipo Array.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_UNSPECTED_PROPERTY
Language=Neutral
Invocación a la propiedad <%1> inesperada.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_UNSPECTED_METHOD
Language=Neutral
Invocación al método <%1> inesperada.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_CANT_DESTROY_S_S
Language=Neutral
No es posible destruir %1 <%2>, Existe una Referencia Activa.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_ABRITTA_S
Language=Neutral
No presione Abritta, <%1> no esta listo yet!
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_FETCH_O_DEFINE_IN_PL
Language=Neutral
Invocacion a Fetch o DefineColumn(s) ilegal en una sentencia PL/SQL.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_VARIANT_MISSMATCH_I
Language=Neutral
Error Interno convirtiendo Variant.
VARTYPE <%1!d!>.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_END
Language=Neutral
runtime end.
.
;/**
;*** RUNTIME END ====================
;***/


;/**
;*** ORACLE
;***/
MessageId=
Severity=Error
Facility=ORACLE
SymbolicName=DBO_E_ORACLE
Language=Neutral
Oracle Error <%1!u!>, %2.
.


;/**
;*** LASTERROR
;***/
MessageId=
Severity=Error
Facility=INTERNAL
SymbolicName=DBO_E_LASTERROR
Language=Neutral
x.
.

MessageId=
Severity=Error
Facility=INTERNAL
SymbolicName=DBO_E_HRESULT
Language=Neutral
x.
.

MessageId=
Severity=Error
Facility=INTERNAL
SymbolicName=DBO_E_LOADTYPLIBFAIL
Language=Neutral
No se pudo cargar la libreria de tipos.
.







;//   const HRESULT DS_E_INTERNO	 		= DS_E_BASE + 0;	// error Interno
;//   const HRESULT DS_E_EXCEPTION 		= DS_E_BASE + 1;	// OS exception
;//   const HRESULT DS_E_PROPRO	 		= DS_E_BASE + 2;	// propiedad read only
;//   const HRESULT DS_E_OLDDATA	 		= DS_E_BASE + 3;	// data de un fetch anterior
;//   const HRESULT DS_E_BADCOLN	 		= DS_E_BASE + 4;	// bad col #, Name
;//   const HRESULT DS_E_NOCON	 		= DS_E_BASE + 5;	// No esta conectado
;//   const HRESULT DS_E_PRECON	 		= DS_E_BASE + 6;	// Ya estaba conectado
;//   const HRESULT DS_E_RANGO	 		= DS_E_BASE + 7;	// Fuera de rango
;//   const HRESULT DS_E_CONVERTTOANSI 	= DS_E_BASE + 8;	// Problemas convirtiendo
;//   const HRESULT DS_E_SEQUENCE 		= DS_E_BASE + 9;	// Fuera de secuencia
;//   const HRESULT DS_E_CREATEWIN		= DS_E_BASE + 10;	// No puedo crear el window
;//   const HRESULT DS_E_LOADTYPLIBFAIL	= DS_E_BASE + 11;	// No pudo calgar la libreria de tipos
;//   const HRESULT DS_E_CREATESQL		= DS_E_BASE + 12;	// No se puede crear un SQL sin conectar
;//   const HRESULT DS_E_VENCIO			= DS_E_BASE + 13;	// No se puede crear un SQL sin conectar
;//   const HRESULT DS_E_LAST		 		= DS_E_BASE + 14;	// Ultimo error + 1
;//   STRINGTABLE DISCARDABLE
;//   BEGIN
;//       2000	"Error Interno."
;//       2001	"Excepción OS."
;//;      2002	"Propiedad temporalmente Read/Only."
;//;      2003    "La información en el Array es de un Fetch anterior."
;//       2004	"Número de columna inválido."
;//       2005	"No esta conectado."
;//       2006	"Ya estaba conectado."
;//       2007	"Fuera de Rango"
;//       2008	"Error convirtiendo string a ANSI"
;//       2009	"Fuera de secuencia ¿?."
;//       2010	"No se pudo crear la ventana."
;//       2011	"No se pudo cargar la libreria de tipos (ds.tlb)."
;//       2012	"No es posible crear un IDboSql si no esta Conectado."
;//       2013	"Vencio."
;//   END
