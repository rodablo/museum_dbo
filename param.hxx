#if !defined(_PARAM_HXX_)
#define _PARAM_HXX_

#include "iparam.hxx"

class TParam 
  :  public TIDISPATCH<IIParam, &IID_Param>
{

public:
  /// IUnknown
  ULONG __stdcall AddRef() { ++m_cRef; return m_IICursor.AddRef(); }
  ULONG __stdcall Release(){ --m_cRef; return m_IICursor.Release(); }

  // Param 
  virtual HRESULT __stdcall get_Name(BSTR* retv);
  virtual HRESULT __stdcall get_Number(short* retv);
  virtual HRESULT __stdcall get_Length(VARIANT Index, short* retv);
  virtual HRESULT __stdcall get_RC(VARIANT Index, short* retv);

  // IIParam
  virtual long UniqueID() const { return m_lUniqueID; }
  virtual void PreWork() {}   
  virtual void PosWork() {} 
  virtual void BindArray(dboVarType AsType, short ArraySize, VARIANT StringLength) {}
  virtual void ClearBind(){} 
  //TParam
  HRESULT IDispatchSEH();
  void    SetDirtyFlag();

  TParam(IICursor& rIICursor);
  virtual ~TParam();

protected:
  IICursor&   m_IICursor;
  static long m_lUniqueIDGenerator;
  long        m_lUniqueID;
//bool        m_fIsDirty;

private:
  // Oculta para impedir la copia
  TParam(TParam& other);
  TParam& operator= (TParam&);
  TParam& operator= (const TParam&);
};

inline
HRESULT TParam::IDispatchSEH() {
  return ::IDispatchSEH(m_pEP, m_excepInfo, m_fException, m_rIID, 
                        *m_IICursor.GetLDA(), m_IICursor.GetCDA()->rc); 
}

/**
***
***/
class TScalarParam : public TParam
{
public:
  // Param 
//virtual HRESULT __stdcall put_Null(VARIANT Index, VARIANT_BOOL Null);
  virtual HRESULT __stdcall get_IsNull(VARIANT Index, VARIANT_BOOL* retv);
  virtual HRESULT __stdcall put_Value(VARIANT Index, VARIANT Value);
  virtual HRESULT __stdcall get_Value(VARIANT Index, VARIANT* retv);
  // IIParam
  virtual sword   Number()   { return m_swNumber; }
  virtual string& Name()     { return m_sName;}       
  virtual void    _Bind(); 
  // TScalarParam
  bool            IsNumeric(){ return -1 == m_swNumber; }
  void            Build(VARTYPE Vt, VARIANT& Value); // es publico pero solo lo ven en TScalarParam

  TScalarParam(IICursor& rIICursor, VARIANT& Wich);
  //TScalarParam(IICursor& rIICursor, sword Number);
  //TScalarParam(IICursor& rIICursor, string& Name);
  //  TScalarParam(IICursor& rIICursor, sword Number, VARTYPE Vt, VARIANT& Value);
  //  TScalarParam(IICursor& rIICursor, string& Name, VARTYPE Vt, VARIANT& Value);
  virtual ~TScalarParam();

protected:
  sword       m_swNumber;
  string      m_sName;
  VARIANT     m_V;
  // obndrv/obndrn
  void*       m_pV;           // io   here la data man!  
  sword       m_bindSize;     // i    size de un elemento
  sword       m_bindType;     // i    external datatype
  sb2         m_indp;         //
//sword       m_swScale;      // i    BCD
//ub4         m_ub4MaxSiz;    // i    Creo que maximo fisico en el array
//ub4         m_ub4CurSiz;    // io   Cuantos elementos se movieron
};

class TBSTRParam : public TScalarParam
{
public:
  // Param
  virtual HRESULT __stdcall put_Value(VARIANT Index, VARIANT Value);
  virtual HRESULT __stdcall get_Value(VARIANT Index, VARIANT* retv);

  // TBSTRParam
  TBSTRParam(IICursor& rIICursor, VARIANT& Wich);
  void Build(VARIANT& Value, VARIANT& Length);

protected:
  bool m_fFixedLength;
};

class TDATEParam : public TScalarParam
{
public:
  // Param
  virtual HRESULT __stdcall put_Value(VARIANT Index, VARIANT Value);
  virtual HRESULT __stdcall get_Value(VARIANT Index, VARIANT* retv);

  // TDATEParam
  TDATEParam(IICursor& rIICursor, VARIANT& Wich, VARIANT& Value);
  void Build(VARIANT& Value);

protected:
  ODATE m_odate;
};

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
  //
//   virtual void Bind() = 0;
  virtual void PreWork() = 0;
  //
  TBind(TGParam& Param);
  virtual ~TBind() {}

protected:
  TGParam& _Param;
};

// class TScalarBind
//   :  public TBind
// {
// };

// class TArrayBind
//   :  public TBind
// {
// public:
//   virtual void get_Length(VARIANT Index, short* retv);
//   virtual void get_RC(VARIANT Index, short* retv);
//   virtual void get_IsNull(VARIANT Index, VARIANT_BOOL* retv);
//   //
//   virtual void Bind();  
//   //
//   TArrayBind(TGParam* pParam, short arraySize);
//   virtual ~TArrayBind();

// protected:
//   int ValidateIndex(VARIANT Index);

// protected:
//   int      m_nE;       // numero de elementos 
//   sword    m_bindType;
//   //
//   sword    m_sizeOfR;
//   ub1*     m_pR;       // Ram
//   //
//   sb2*     m_pIndp;    // i/o 
//   ub2*     m_pLen;     // i/o
//   ub2*     m_pRC;      // o
//   // 
//   ub4      m_maxSize;
//   ub4      m_curSize;
// };

// class TNumberArrayBind
//   :  public TArrayBind
// {
// public:
//   virtual void put_Value(VARIANT Index, VARIANT Value);
//   virtual void get_Value(VARIANT Index, VARIANT* retv);
//   //
//   virtual void PreWork();
//   //
//   TNumberArrayBind(TGParam* pParam, VARTYPE vt, short arraySize, VARIANT& Value);
//   ~TNumberArrayBind();
// protected:
//   size_t  m_sizeOfE;
//   void*   m_pV;
// };

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
  friend TArrayBind;
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
  // IIParam
  virtual operator Cda_Def*() {return m_IICursor;} 
  virtual long     UniqueID() const { return m_lUniqueID; }
  virtual void     PreWork();
  virtual void     PosWork()  {} 
  virtual sword    Number()   { return m_swNumber; }
  virtual string&  Name()     { return m_sName; }       
  // este método esta por el momento, volarlo de IIParam
  virtual void     _Bind()    { ::Beep(440,4000); } 
  virtual void     BindArray(dboVarType AsType, short ArraySize, VARIANT StringLength);
  virtual void     ClearBind(){} 
  // TGParam
  HRESULT IDispatchSEH();
  //
  TGParam(IICursor& rIICursor, VARIANT& Wich);
  TGParam(IICursor& rIICursor, string&  Wich);
  virtual ~TGParam();

protected:
  IICursor&   m_IICursor;
  static long m_lUniqueIDGenerator;
  long        m_lUniqueID;
  sword       m_swNumber;
  string      m_sName;
  //
  AP<TBind>   _apBind;
};

inline
HRESULT TGParam::IDispatchSEH() {
  return ::IDispatchSEH(m_pEP, m_excepInfo, m_fException, m_rIID, 
                        *m_IICursor.GetLDA(), m_IICursor.GetCDA()->rc); 
}

#endif //_COL_HXX_




