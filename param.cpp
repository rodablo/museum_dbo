/**
*** DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "param.hxx"

/**
*** CreateParam
*** Param Abstract Factory
***/
void 
_CreateParam(IICursor& cursor, VARIANT& Wich, VARIANT& Value, VARIANT& AsType, VARIANT& Length, 
	    AP<IIParam>& rapParam)
{
  // determina tipo de parametro (Numerico o Alfa) por el VT_ del indice 
//   sword Number = -1;
//   switch (V_VT(&Wich)) // (pasar esto al constructor de TScalarParam, unificando los constructores)
//     {
//     case VT_UI1:// Byte 
// 	Number = (sword)V_UI1(&Wich); break;
//     case VT_I2: // Integer
// 	Number = (sword)V_I2(&Wich);  break;            
//     case VT_I4: // Long     
// 	Number = (sword)V_I4(&Wich);  break; 
//     case VT_R4: // Single     
// 	Number = (sword)V_R4(&Wich);  break; 
//     case VT_R8: // Double     
// 	Number = (sword)V_R8(&Wich);  break;      
//     case VT_BSTR:
// 	break;
//     default:
//       RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NAME_TYPE_MISMATCH_I, V_VT(&Wich));
//     }
  
  VARTYPE vt;
  // determina el tipo usando AsType
  if (VT_ERROR != V_VT(&AsType))
    {
      vt = (VARTYPE)V_I2(&AsType);
    }
  // lo deduce desde Valor inicial
  else if (VT_ERROR != V_VT(&Value)) // lo deduce del valor inicial
    {
      vt = V_VT(&Value);
    }
  else
    // no puede determinar el tipo
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_CANT_DETERMINE_VTYPE);

  // creo en funcion de vt
  switch (vt)
    {
    case VT_UI1:// Byte 
    case VT_I2: // Integer
    case VT_I4: // Long     
    case VT_R4: // Float
    case VT_R8: // Double
      rapParam = new TScalarParam(cursor, Wich);
      // setup separado para generar excepciones 
      ((TScalarParam*)rapParam.get())->Build(vt, Value);
      break;
    case VT_DATE:
      rapParam = new TDATEParam(cursor, Wich, Value);	
      break;
    case VT_BSTR:
      rapParam = new TBSTRParam(cursor, Wich);	
      // setup separado para generar excepciones 
      ((TBSTRParam*)rapParam.get())->Build(Value, Length);
      break;
      //case VT_BOOL:
    default:
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_TYPE_MISMATCH_I, vt);
    }
}

/**
*** TPARAM
***/
long 
TParam::m_lUniqueIDGenerator = 1;

TParam::TParam(IICursor& rIICursor)
  :  m_IICursor(rIICursor)
{
  // asigna numero unico
  m_lUniqueID = m_lUniqueIDGenerator++;
}

TParam::~TParam()
{
  // algun tipo de bind (0) para avisarle al oci que ya no es valido UNBIND
}

/**
*** Param PROPERTIES
***/
HRESULT __stdcall
TParam::get_Name(BSTR* retv)
{
  try {
    //
    *retv = WIDE("me olvide de retornar el name!"/*m_sName*/).SysAllocString(); 
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TParam::get_Number(short* retv)
{
  try {
    // si no es numerico raise (IsNumeric)
    *retv = 0;
  }
  __AUTO_EXCEPT;
}
HRESULT __stdcall 
TParam::get_Length(VARIANT Index, short* retv)
{
  try {
    /* probar la invocacion del destructor en caso de un RAISE en el constructor y 
       el catch(...) throw; para emular el __finally (esto se puede probar facilmente
       en un metodo automation
       */
    //    try {
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_PROP_ONLY_IN_ARRAYS_S, "Length");
      //    }
      //   catch (...) {
      //       // propaga 
      //       throw;
      //     }
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TParam::get_RC(VARIANT Index, short* retv)
{
  try {
    //
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_PROP_ONLY_IN_ARRAYS_S, "RC");
  }
  __AUTO_EXCEPT;
}

/**
*** TSCALARPARAM
***/
TScalarParam::TScalarParam(IICursor& rIICursor, VARIANT& Wich)
  :  TParam(rIICursor)
{
  m_indp = 0;
  // determina tipo de parametro (Numerico o Alfa) por el VT_ del indice 
  m_swNumber = -1;
  switch (V_VT(&Wich)) // (pasar esto al constructor de TScalarParam, unificando los constructores)
    {
    case VT_UI1:// Byte 
      m_swNumber = (sword)V_UI1(&Wich); break;
    case VT_I2: // Integer
      m_swNumber = (sword)V_I2(&Wich);  break;            
    case VT_I4: // Long     
      m_swNumber = (sword)V_I4(&Wich);  break; 
    case VT_R4: // Single     
      m_swNumber = (sword)V_R4(&Wich);  break; 
    case VT_R8: // Double     
      m_swNumber = (sword)V_R8(&Wich);  break;      
    case VT_BSTR:
      m_sName    = ANSI(V_BSTR(&Wich));
      break;
    default:
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NAME_TYPE_MISMATCH_I, V_VT(&Wich));
    }

  // bindear esto!
  m_pV = &m_V.lVal;
  //
  //  m_IICursor.AddRef();
}

void 
TScalarParam::Build(VARTYPE Vt, VARIANT& Value)
{
  //
  m_indp = 0;
  // tipo
  V_VT(&m_V) = Vt;
  // alloca según el tipo
  switch (V_VT(&m_V))
    {
    case VT_UI1:// Byte 
      m_bindSize = sizeof(unsigned char); // seria mejor usar sizeof(miembre del variant)
      m_bindType = SQLT_INT;
      break;
    case VT_I2: // Integer
      m_bindSize = sizeof(short);
      m_bindType = SQLT_INT;
      break;
    case VT_I4: // Long     
      m_bindSize = sizeof(long);
      m_bindType = SQLT_INT;
      break;
    case VT_R4: // Float
      m_bindSize = sizeof(float);
      m_bindType = SQLT_FLT;
      break;
    case VT_R8: // Double
      //*m_pV    = (VT_ERROR != V_VT(&Value)) ? V_R8(&Value) : 0; 
      m_bindSize = sizeof(double);
      m_bindType = SQLT_FLT;
      break;
    }
  // inicializa
  if (VT_ERROR != V_VT(&Value)) 
    {
      if (Vt == V_VT(&Value))
	CopyMemory(&m_V.lVal, &Value.lVal, m_bindSize);
      else
	// convierte a date (si es posible) y copia
	CHECK_HRESULT(VariantChangeType(&m_V, &Value, 0, Vt));
    }
  else
    ZeroMemory(&m_V.lVal, m_bindSize);
}

TScalarParam::~TScalarParam()
{
  // libera la IF del cursor
  //  m_IICursor.Release();
  // algun tipo de bind (0) para avisarle al oci que ya no es valido UNBIND
}

// HRESULT __stdcall
// TScalarParam::put_Null(VARIANT Index, VARIANT_BOOL Null)
// {
//   try {
//     // no puede tener indice si no es array
//     if (VT_ERROR != V_VT(&Index))
//       RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_ONLY_IN_ARRAYS);
//     // 
//     m_indp = (sb2)((VARIANT_FALSE == Null) ? 0 : -1);
//     // 
//     m_IICursor.SetDirtyFlag();
//   }
//   __AUTO_EXCEPT;
// }

HRESULT __stdcall
TScalarParam::get_IsNull(VARIANT Index, VARIANT_BOOL* retv)
{
  try {
    // no puede tener indice si no es array
    if (VT_ERROR != V_VT(&Index))
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_ONLY_IN_ARRAYS);
    // 
    *retv = 0 > m_indp ? VARIANT_TRUE : VARIANT_FALSE; 
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TScalarParam::get_Value(VARIANT Index, VARIANT* retv)
{
  try {
    // no puede tener indice si no es array
    if (VT_ERROR != V_VT(&Index))
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_ONLY_IN_ARRAYS);
    // es nulo ?
    if (0 > m_indp)
      {
	// libera cualquier previo
	CHECK_HRESULT(VariantClear(retv));
	V_VT(retv) = VT_NULL;
      }
    else
      // copia
      CHECK_HRESULT(VariantCopy(retv, &m_V));
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TScalarParam::put_Value(VARIANT Index, VARIANT Value)
{
  try {
    // no puede tener indice si no es array
    if (VT_ERROR != V_VT(&Index))
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_ONLY_IN_ARRAYS);
    // procesa el Nulo
    if (VT_NULL == V_VT(&Value))
      m_indp = -1;
    else
      {
	// saca el nulo
	m_indp = 0;
	// convierte (si es necesario) y copia
	CHECK_HRESULT(VariantChangeType(&m_V, &Value, 0, V_VT(&m_V)));
      }
    // 
    m_IICursor.SetDirtyFlag();
  }
  __AUTO_EXCEPT;
}  

/**
***
***/
// bool
// TParam::operator< (const IIParam& other) const
// {
//   return m_lUniqueID < other.UniqueID();
// }
void
TScalarParam::_Bind()
{                                                            //-1 no funka!
  if (IsNumeric())
//     CHECK_OCI(obndrv(m_IICursor.GetCDA(), (text*)m_sName.c_str(), -1, 
// 		     (ub1*)0, 0/*m_bindSize*/, m_bindType, 0, 0, 
// 		     (text*)0, -1, -1));
    CHECK_OCI(obndrv(m_IICursor.GetCDA(), (text*)m_sName.c_str(), -1, 
 		     (ub1*)m_pV, m_bindSize, m_bindType, 0, &m_indp, 
 		     (text*)0, -1, -1));
  else
    CHECK_OCI(obndrn(m_IICursor.GetCDA(), m_swNumber, 
		     (ub1*)m_pV, m_bindSize, m_bindType, 0, &m_indp,
		     (text*)0, -1, -1));
}
