/**
***     DBO2(1997/1998)
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "icolumns.hxx"

class TColumns
  :  public TIDISPATCH<IIColumns, &IID_Columns>
{
public:
  /// IUnknown
  ULONG __stdcall AddRef() { ++m_cRef; return m_IICursor.AddRef(); }
  ULONG __stdcall Release(){ --m_cRef; return m_IICursor.Release(); }
  /// Columns
  STDMETHOD(get_Count)(long* retv);
  STDMETHOD(get_Item)(VARIANT Index, Column** ppColumn);
  //STDMETHOD(get__NewEnum)(IUnknown** ppIUnknown);
  ///
  TColumns(IICursor& iicursor);
  ~TColumns();

protected:
  HRESULT  IDispatchSEH();

protected:
  IICursor&  m_IICursor;        
};

inline
HRESULT TColumns::IDispatchSEH() {
  return ::IDispatchSEH(m_pEP, m_excepInfo, m_fException, m_rIID, 
                        *m_IICursor.GetLDA(), m_IICursor.GetCDA()->rc); 
}


/** Factory
*** (en el futuro sera de ICursor)
*** Uso referencias para forzar a que existan y no se pueda pasar NULL
***/
void 
CreateColumns(IICursor& iicursor, IIColumns*& rpColumns)  
{
  rpColumns = new TColumns(iicursor);
}

TColumns::TColumns(IICursor& iicursor)
  :  m_IICursor(iicursor)
{
  // m_IICursor.AddRef();
}

TColumns::~TColumns()
{
  //m_IICursor.Release();
}

HRESULT __stdcall
TColumns::get_Count(long* retv)
{
  *retv = m_IICursor.GetColumnCount();
  return NOERROR;
}

HRESULT __stdcall
TColumns::get_Item(VARIANT Index, Column** ppColumn)
{
  try {

#pragma message verificar que ppColumn != 0

    int i = 0; 
    // pesimismo inicia
    *ppColumn = 0;
    // determina el VT_ del indice
    switch (V_VT(&Index))
      {
//    case VT_I1: // Byte
//      i = (int)V_I1(&Index);  break;
      case VT_UI1: 
	i = (int)V_UI1(&Index); break;
      case VT_I2: // Integer
	i = (int)V_I2(&Index);  break;            
//    case VT_UI2: 
//	i = (int)V_UI2(&Index); break;
      case VT_I4: // Long     
	i = (int)V_I4(&Index);  break;      
//    case VT_UI4: 
//	i = (int)V_UI4(&Index); break;
      case VT_BSTR:
	break;
      default:
	RAISE_INTERNAL(DBO_E_RUNTIME_COL_INDEX_TYPE_MISMATCH_I, i);
      }
    // retorna
    if (0 != i)
      *ppColumn = (Column*)m_IICursor.GetColumn(i);  
    else
      {
	string temp(ANSI(V_BSTR(&Index)).to_upper());
	*ppColumn = (Column*)m_IICursor.GetColumn(temp);
      }
    // 
    (*ppColumn)->AddRef();
  }
  __AUTO_EXCEPT;
  
}

// HRESULT __stdcall
// TColumns::get__NewEnum(IUnknown** ppIUnknown)
// {
//   return NOERROR;
// }






