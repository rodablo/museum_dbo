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
SymbolicName=DBO_E_RUNTIME_LVALUE_NEED_PARETHESIS
Language=Neutral
Los argumentos necesitan '(' ')' cuando la expresion se evalua como LValue.
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
SymbolicName=DBO_E_RUNTIME_PARAM_NVL_TYPE_MISMATCH_I_I
Language=Neutral
NullVaLue DEBE! ser de tipo variant <%1!d!>, se encontró <%2!d!>.
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
SymbolicName=DBO_E_RUNTIME_DATE_ERROR
Language=Neutral
Error Interno convirtiendo DATE.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PIECEWISE_NEED_EVENTS
Language=Neutral
Operaciones piecewise requiren Eventos.
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_PIECEWISE_INSERT_TYPE_MISMATCH_S
Language=Neutral
Piecewise insert type mismatch, <%1> spected.
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


;/**
;*** LIC
;***/
MessageId=0x300
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_LIC_NOT_LOADED
Language=Neutral
DBO_E_RUNTIME_LIC_NOT_LOADED
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_LIC_ERROR
Language=Neutral
DBO_E_RUNTIME_LIC_ERROR
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_LIC_CRYPTO_ERROR
Language=Neutral
DBO_E_RUNTIME_LIC_CRYPTO_ERROR
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_LIC_EXPIRED
Language=Neutral
DBO_E_RUNTIME_LIC_EXPIRED
.

MessageId=
Severity=Error
Facility=RUNTIME
SymbolicName=DBO_E_RUNTIME_LIC_BAD_DOMAIN
Language=Neutral
DBO_E_RUNTIME_LIC_BAD_DOMAIN
.




