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
  _fStrict = m_IICursor.IsStrict();
  // asigna numero unico
  m_lUniqueID = m_lUniqueIDGenerator++;
}

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
    if (!m_sName.empty())
      *retv = WIDE(m_sName).SysAllocString(); 
    else
      *retv = 0;
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TGParam::get_Number(short* retv)
{
  try {
    // si no es numerico raise (IsNumeric)
    *retv = (short)(-1 != m_swNumber ? m_swNumber : 0);
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

void 
TGParam::Internal_put_Value(VARIANT& Index, VARIANT& Value)
{
  //
  if (_apBind.is_empty())
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
  //
  _apBind->put_Value(Index, Value);
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

HRESULT __stdcall 
TGParam::get_NVL(VARIANT Index, VARIANT NullVaLue, VARIANT* retv)
{
  try {
    //
    if (_apBind.is_empty())
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
    //
    _apBind->get_NVL(Index, NullVaLue, retv);
  }
  __AUTO_EXCEPT;
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
























