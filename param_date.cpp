/**
*** DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "param.hxx"

TDATEParam::TDATEParam(IICursor& rIICursor, VARIANT& Wich, VARIANT& Value)
  :  TScalarParam(rIICursor, Wich)
{
  // bindear esto!
  m_pV = &m_odate;
  // 
  Build(Value);
}

void 
TDATEParam::Build(VARIANT& Value)
{
  // 
  m_bindSize = sizeof(ODATE);
  m_bindType = SQLT_DAT;
  // si hay valor inicial (else el constructor de odate hizo lo suyo)
  if (VT_ERROR != V_VT(&Value))
    {
      // convierte a date (si es posible) y copia
      VARIANT v;
      CHECK_HRESULT(VariantChangeType(&v, &Value, 0, VT_DATE));
      // atencion que aqui pierde presicion (solo quedan los segundos
      m_odate = V_DATE(&v);
    }
}

HRESULT __stdcall 
TDATEParam::get_Value(VARIANT Index, VARIANT* retv)
{
  try {
    // no puede tener indice si no es array
    if (VT_ERROR != V_VT(&Index))
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_ONLY_IN_ARRAYS);
    // libera cualquier previo
    CHECK_HRESULT(VariantClear(retv));
    // es nulo ?
    if (0 > m_indp)
	V_VT(retv) = VT_NULL;
    else
      {
	// obtiene un variant
	V_VT(retv) = VT_DATE;
	V_DATE(retv)= (DATE)m_odate;
//      // obtiene un variant
// 	VARIANT v;
// 	V_VT(&v) = VT_DATE;
// 	V_DATE(&v)= (DATE)m_odate;
// 	// copia
// 	CHECK_HRESULT(VariantCopy(retv, &v));
      }
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TDATEParam::put_Value(VARIANT Index, VARIANT Value)
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
	// convierte a date (si es posible) y copia
	VARIANT v;
	VariantInit(&v);
	CHECK_HRESULT(VariantChangeType(&v, &Value, 0, VT_DATE));
	// atencion que aqui pierde presicion (solo quedan los segundos
	m_odate = V_DATE(&v);   
      } 
    // 
    m_IICursor.SetDirtyFlag();
  }
  __AUTO_EXCEPT;
}  


