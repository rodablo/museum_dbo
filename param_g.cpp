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
CreateNumericParam(IICursor& cursor, sword Wich, AP<IIParam>& rapParam)
{
  rapParam = new TGParam(cursor);
  static_cast<TGParam*>(rapParam.get())->m_swNumber = Wich;
}

void
CreateStringParam(IICursor& cursor, string& Wich, AP<IIParam>& rapParam)
{
  rapParam = new TGParam(cursor);
  static_cast<TGParam*>(rapParam.get())->m_swNumber = -1;
  static_cast<TGParam*>(rapParam.get())->m_sName = Wich;
}

// bool
// ValidateWichParam(VARIANT& rWich, sword* pNumber)
// {
//     *pNumber = -1;
//     // determina el VT_
//     switch (V_VT(&rWich))
//       {
//       case VT_UI1:// Byte
// 	*pNumber = (sword)V_UI1(&rWich); break;
//       case VT_I2: // Integer
// 	*pNumber = (sword)V_I2(&rWich);  break;            
//       case VT_I4: // Long
// 	*pNumber = (sword)V_I4(&rWich);  break;      
//       case VT_BSTR:
// 	*pNumber = -1;
// 	return true;
//       default:
// 	RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_TYPE_MISMATCH_I, (long)V_VT(&rWich));
//       }
//     // es number
//     return false;
// }

/**
*** TGParam
***/
long
TGParam::m_lUniqueIDGenerator = 10000;

TGParam::TGParam(IICursor& rIICursor)
  :  m_IICursor(rIICursor)
{
  // 
  _fStrict = true;
  // determina tipo de parametro (Numerico o Alfa) por el VT_ del indice
  //  m_swNumber = Wich;//-1;
  /*  switch (V_VT(&Wich)) // (pasar esto al constructor de TScalarParam, unificando los constructores)
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
    }*/
  // asigna numero unico
  m_lUniqueID = m_lUniqueIDGenerator++;
}

/*TGParam::TGParam(IICursor& rIICursor, string& Wich)
  :  m_IICursor(rIICursor)
{
  // 
  _fStrict = true;
  // es alfa
  m_swNumber = -1;
  // 
  m_sName = Wich;
  // asigna numero unico
  m_lUniqueID = m_lUniqueIDGenerator++;
}*/

TGParam::~TGParam()
{
  // algun tipo de bind (0) para avisarle al oci que ya no es valido UNBIND
}

/**
*** Param
***/
HRESULT __stdcall
TGParam::get_Name(BSTR* retv)
{
  try {
    //
    *retv = WIDE("pendiente"/*m_sName*/).SysAllocString(); 
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TGParam::get_Number(short* retv)
{
  try {
    // si no es numerico raise (IsNumeric)
    *retv = 0;
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TGParam::get_Length(VARIANT Index, short* retv)
{
  try {
    // 
    if (_apBind.is_empty())
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
    //
    _apBind->get_Length(Index, retv);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TGParam::get_RC(VARIANT Index, short* retv)
{
  try {
    // 
    if (_apBind.is_empty())
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
    //
    _apBind->get_RC(Index, retv);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TGParam::get_IsNull(VARIANT Index, VARIANT_BOOL* retv)
{
  try {
    // 
    if (_apBind.is_empty())
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
    //
    _apBind->get_IsNull(Index, retv);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TGParam::get_Value(VARIANT Index, VARIANT* retv)
{
  try {
    // 
    if (_apBind.is_empty())
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
    //
    _apBind->get_Value(Index, retv);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TGParam::put_Value(VARIANT Index, VARIANT Value)
{
  try {
    // 
    if (_apBind.is_empty())
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
    //
    _apBind->put_Value(Index, Value);
  }
  __AUTO_EXCEPT;
}  

HRESULT __stdcall 
TGParam::put_Strict(VARIANT_BOOL Strict)
{  
  _fStrict = (VARIANT_TRUE == Strict);
  return NOERROR;
}

HRESULT __stdcall 
TGParam::get_Strict(VARIANT_BOOL* retv)
{  
  *retv = IsStrict() ? VARIANT_TRUE : VARIANT_FALSE;
  return NOERROR;
}
/**
*** TGParam
***/
void 
TGParam::PreWork()
{
  //  if (!_apBind.is_empty())
  //     _apBind->PreWork();
  // not bind error
}

/*
HRESULT __stdcall 
TGParam::BindArray(VARIANT Value, VARIANT ArraySize, dboVarType AsType, VARIANT StringLength,
		   Param** retv)
{
  try {
    // 1. determino el tipo a. desde astype  b. desde value c. error
    // 2. determino size, a. desde array size b. des value
    // 3. creolo
    // 4. inicializo
    // 0. deleteo bind previo 
    // 5. bindeo

    // -----------

    // DETERMINA EL TIPO
    VARTYPE vt;
    // usando AsType
    if (dboVError != AsType)
      vt = (VARTYPE)AsType;
    // lo deduce desde Value
    else if (VT_ERROR != V_VT(&Value)) // lo deduce del valor inicial
      vt = (VARTYPE)(VT_ARRAY & V_VT(&Value));
    // no puede determinar el tipo
    else
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_CANT_DETERMINE_VTYPE);

    // DETERMINA EL TAMAÑO DEL ARRAY
    short arraySize;
    // usando ArraySize
    if (VT_ERROR != V_VT(&ArraySize)) // lo deduce del valor inicial
      {
      if (!GetShortFromVariant(arraySize, ArraySize))
 	  RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_ARRAYSIZE_MISMATCH_I, V_VT(&ArraySize));
      }
    // lo deduce de Value
    else if (0 != (VT_ARRAY & V_VT(&Value)))
      {
	// obtiene array
	SAFEARRAY* pSA;
	if (0 != (VT_BYREF & V_VT(&Value)))
	  pSA = V_ARRAY(&Value);
	else
	  pSA = *V_ARRAYREF(&Value);

	// verifica que sea vector
	if (1 != SafeArrayGetDim(pSA))
	  ;//RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_BAD_NUMBER_OF_DIMS);
    
	// obtiene el size
	long LBound, UBound, lSize;
	CHECK_HRESULT(SafeArrayGetLBound(pSA, 1, &LBound));
	CHECK_HRESULT(SafeArrayGetUBound(pSA, 1, &UBound));
	lSize = (UBound - LBound);
	
	// if (0x7fff < lSize)
// 	  ;//muy grande verifivar tambien alguna regle del OCI respecto a este maximo

	arraySize = (short)lSize;
      }
    else
      // no puede determinar el tipo
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_CANT_DETERMINE_ARRAY_SIZE);  

    // CREO BIND EN FUNCION DE VT E INICIALIZO
    AP<TBind> apBind;
    switch (vt)
      {
      case VT_UI1:// Byte 
      case VT_I2: // Integer
      case VT_I4: // Long
      case VT_R4: // Float
      case VT_R8: // Double
	apBind = new TNumberArrayBind(this, vt, arraySize, Value);
	break;
      case VT_DATE:
	break;
      case VT_BSTR:
	break;
      default:
	RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_TYPE_MISMATCH_I, vt);
    }  

    // DELETEO PREVIO
    _apBind = 0;

    // BINDEO NUEVO Y ASIGNO
    apBind->Bind();
    m_apBind = apBind;   
  }
  __AUTO_EXCEPT;
}
*/

/**
*** TBind()
***/
TBind::TBind(TGParam& Param)
  :  _Param(Param)
{
}






















