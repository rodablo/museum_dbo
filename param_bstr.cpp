/**
*** DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "param.hxx"


TBSTRParam::TBSTRParam(IICursor& rIICursor, VARIANT& Wich)
  :  TScalarParam(rIICursor, Wich)
{
  // 0 para que el destructor lo detecte
  m_pV = 0;
}

void 
TBSTRParam::Build(VARIANT& Value, VARIANT& Length)
{
  // si hay un Value lo prepara para inicializar y obtener el length
  //TString value;
  string value; 
  if (VT_ERROR != V_VT(&Value))
    {
      // convierte a BSTR...
      VARIANT v;
      VariantInit(&v);
      CHECK_HRESULT(VariantChangeType(&v, &Value, 0, VT_BSTR));
      // y copia
      value = ANSI(V_BSTR(&v));
      // libera
      VariantClear(&v);
    }  
  // determina el length
  long length = 0;
  if (VT_ERROR != V_VT(&Length))
    {
      // verifica tipo del Length y convierte si es necesario
      switch (V_VT(&Length))
	{
	case VT_UI1:// Byte 
	  length = (sword)V_UI1(&Length); break;
	case VT_I2: // Integer
	  length = (sword) V_I2(&Length);  break;            
	case VT_I4: // Long     
	  length = (sword) V_I4(&Length);  break; 
	case VT_R4: // Single     
	  length = (sword) V_R4(&Length);  break; 
	case VT_R8: // Double     
	  length = (sword) V_R8(&Length);  break;  
	default:      
	  RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_LENGTH_TYPE_MISMATCH_I, V_VT(&Length));
	}
      // es de tamaño fijo
      m_fFixedLength = true;
    }
  else if (!value.empty()/*IsNull()*//*VT_ERROR != V_VT(&Value)*/)
    {
      // lo extrae del Value 
      length = value.length();
      //
      m_fFixedLength = false;
    }
  else
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_CANT_DETERMINE_STRING_SIZE);

  // 
  m_bindSize = length + 1; // largo (sin 0 final)
  m_bindType = SQLT_STR;   // string sin 0 final)

  // aloca el buffer
  m_pV = new char[m_bindSize];

  // 
  if (!value.empty/*IsNull*/()/*VT_ERROR != V_VT(&Value)*/)
    {
      /*string(*/value/*)*/.copy((char*)m_pV, m_bindSize);
    ((char*)m_pV)[value.length()]=0;
    }
  else
    ZeroMemory(m_pV, length);
}

HRESULT __stdcall 
TBSTRParam::get_Value(VARIANT Index, VARIANT* retv)
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
	// ritorna il variant
	V_VT(retv)   = VT_BSTR;
	V_BSTR(retv) = WIDE((char*)m_pV).SysAllocString();
      }
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TBSTRParam::put_Value(VARIANT Index, VARIANT Value)
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
	// convierte a BSTR (si es posible) y copia
	// convierte a string...
	VARIANT v;
	/*CHECK_HRESULT(*/VariantInit(&v)/*)*/;
	CHECK_HRESULT(VariantChangeType(&v, &Value, 0, VT_BSTR));
	string value(ANSI(V_BSTR(&v)));
	// y copia
	if (!m_fFixedLength)
	  {
	    // libera el previo
	    delete [] m_pV;
	    //
	    m_bindSize = value.length() + 1;
	    m_pV = new char[m_bindSize];
	    _Bind();
	  }

	value.copy((char*)m_pV, m_bindSize);      
	((char*)m_pV)[value.length()]=0;
      }
    // 
    m_IICursor.SetDirtyFlag();
  }
  __AUTO_EXCEPT;
}  


