#if !defined(_PARAM_HXX_)
#define _PARAM_HXX_

#include "iparam.hxx"

/**
*** NEXTGEN
***/
class TGParam;
class TBind
{
public:
  // Param (usa referencias en los variants para evitar copiar los VARIANTS)
  virtual void get_Length(VARIANT& Index, short* retv) = 0;
  virtual void get_RC(VARIANT& Index, short* retv) = 0;
  virtual void get_IsNull(VARIANT& Index, VARIANT_BOOL* retv) = 0;
  virtual void put_Value(VARIANT& Index, VARIANT& Value) = 0;
  virtual void get_Value(VARIANT& Index, VARIANT* retv) = 0;
  virtual void get_NVL(VARIANT& Index, VARIANT& NullVaLue, VARIANT* retv) = 0;
  //
  virtual void PreWork() = 0;
  //
  TBind(TGParam& Param) : _Param(Param) {}
  virtual ~TBind() {}

protected:
  TGParam& _Param;
};

/**
*** TG(eneric)Param 
***                Esta clase finalmente se convertira en TParam cuando 
*** se termine la implementación genérica de parametros y se reemplace a la actual.
*** El motivo de esta nueva implementación es permitir el rebindeo del parámetro
*** sin perder la validéz de la referencia en el lenguaje controlador.
***/
class TGParam 
  :  public TIDISPATCH<IIParam, &IID_Param>
{
  friend void CreateNumericParam(IICursor& cursor, sword Wich, AP<IIParam>& rapParam);
  friend void CreateStringParam(IICursor& cursor, string& Wich, AP<IIParam>& rapParam);
public:
  // IUnknown
  ULONG __stdcall AddRef() { ++m_cRef; return m_IICursor.AddRef(); }
  ULONG __stdcall Release(){ --m_cRef; return m_IICursor.Release(); }
  // Param PROPS
  virtual HRESULT __stdcall get_Name(BSTR* retv);
  virtual HRESULT __stdcall get_Number(short* retv);
  virtual HRESULT __stdcall get_Length(VARIANT Index, short* retv);
  virtual HRESULT __stdcall get_RC(VARIANT Index, short* retv);
  virtual HRESULT __stdcall get_IsNull(VARIANT Index, VARIANT_BOOL* retv);
  virtual HRESULT __stdcall put_Value(VARIANT Index, VARIANT Value);
  virtual HRESULT __stdcall get_Value(VARIANT Index, VARIANT* retv);
  virtual HRESULT __stdcall put_Strict(VARIANT_BOOL Strict);
  virtual HRESULT __stdcall get_Strict(VARIANT_BOOL* retv);
  virtual HRESULT __stdcall get_NVL(VARIANT Index, VARIANT NullVaLue, VARIANT* retv);

  virtual HRESULT __stdcall Dummy1() {return 0;}
  virtual HRESULT __stdcall Dummy2() {return 0;}  
  virtual HRESULT __stdcall Dummy3() {return 0;}  
  virtual HRESULT __stdcall Dummy4() {return 0;}

  // IIParam
  virtual operator Cda_Def*() { return m_IICursor; }
  virtual long     UniqueID() const { return m_lUniqueID; }
  virtual bool     IsAlfaN()  {return -1 == m_swNumber;}
  virtual void     PreWork(); 
  virtual void     PosWork()  { }
  virtual sword    Number()   { return m_swNumber; }
  virtual string&  Name()     { return m_sName; }
  virtual void     Bind(dboVarType AsType, VARIANT& StringLength);
  virtual void     BindArray(dboVarType AsType, short ArraySize, VARIANT& StringLength);
  virtual void     ClearBind(){} 
  virtual bool     IsStrict() { return _fStrict || m_IICursor.IsStrict(); }
  // TGParam
  HRESULT IDispatchSEH();
  void    SetDirtyFlag()      { m_IICursor.SetDirtyFlag(); }
  //
  TGParam(IICursor& rIICursor);
  virtual ~TGParam();

protected:
  IICursor&   m_IICursor;
  static long m_lUniqueIDGenerator;
  long        m_lUniqueID;
  sword       m_swNumber;
  string      m_sName;
  bool        _fStrict;
  //
  AP<TBind>   _apBind;
};

inline
HRESULT TGParam::IDispatchSEH() {
  return ::IDispatchSEH(m_pEP, m_excepInfo, m_fException, m_rIID, 
                        *m_IICursor.GetLDA(), m_IICursor.GetCDA()->rc); 
}

#endif //_COL_HXX_




