/**
***     DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "iparams.hxx"

class TParams
  :  public TIDISPATCH<IIParams, &IID_Params>
{
public:
  /// IUnknown
  ULONG __stdcall AddRef() { ++m_cRef; return m_IICursor.AddRef(); }
  ULONG __stdcall Release(){ --m_cRef; return m_IICursor.Release(); }
  /// Columns
  STDMETHOD(get_Count)(long* retv);
  STDMETHOD(get_Item)(VARIANT Index, Param** ppParam);
  STDMETHOD(get__NewEnum)(IUnknown** ppIUnknown);
  ///
  TParams(IICursor& iicursor);
  ~TParams();

protected:
  HRESULT  IDispatchSEH();

protected:
  IICursor&  m_IICursor;        
};

inline
HRESULT TParams::IDispatchSEH() {
  return ::IDispatchSEH(m_pEP, m_excepInfo, m_fException, m_rIID, 
                        *m_IICursor.GetLDA(), m_IICursor.GetCDA()->rc); 
}

/** Factory
*** (en el futuro sera de ICursor)
*** Uso referencias para forzar a que existan y no se pueda pasar NULL
***/
void 
CreateParams(IICursor& iicursor, IIParams*& rpParams)  
{
  rpParams = new TParams(iicursor);
}

TParams::TParams(IICursor& iicursor)
  :  m_IICursor(iicursor)
{
  // m_IICursor.AddRef();
}

TParams::~TParams()
{
  //m_IICursor.Release();
}

HRESULT __stdcall
TParams::get_Count(long* retv)
{
  *retv = m_IICursor.GetParamCount();
  return NOERROR;
}

HRESULT __stdcall
TParams::get_Item(VARIANT Index, Param** ppParam)
{
  try {
    //
    sword i = -1; 
    // pesimismo inicia
    *ppParam = 0;
    // determina el VT_ del indice
    switch (V_VT(&Index))
      {
      case VT_UI1:// Byte 
	i = (sword)V_UI1(&Index); break;
      case VT_I2: // Integer
	i = (sword)V_I2(&Index);  break;            
      case VT_I4: // Long     
	i = (sword)V_I4(&Index);  break;      
      case VT_BSTR:
	break;
      default:
	RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_TYPE_MISMATCH_I, i);
      }
    // retorna
    if (-1 != i)
      *ppParam = (Param*)m_IICursor.GetParam(i);  
    else
      {
	//*ppParam = (Param*)m_IICursor.GetParam(string(TString(V_BSTR(&Index))));
	string a(ANSI(V_BSTR(&Index)));
	*ppParam = (Param*)m_IICursor.GetParam(a);
      }
    // 
    (*ppParam)->AddRef();
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TParams::get__NewEnum(IUnknown** ppIUnknown)
{
  return m_IICursor.NewEnumParams(ppIUnknown);
}






