#if !defined(_COLUMN_HXX_)
#define _COLUMN_HXX_

#include "icolumn.hxx"

#define MAXCOLNAM 255

class TColumnD 
{ 
public:
  TColumnD::TColumnD(IICursor& IICursor, sword pos);
  TColumnD::TColumnD(TColumnD& other);

public:
  string    m_sName;
  sword     m_pos;
  IICursor& m_IICursor;
  // ::odesc
  sb4       m_dbsize;          // max size
  sb2       m_dbtype;          // internal datatype code
  sb4       m_dsize;           // dispay size
  sb2       m_prec;            // de los numericos
  sb2       m_scale;           // escala de los numericos
  sb2       m_nullok;          // Nulls si/no
  //
  sb4       m_bindType;        // tipo a bindear
  long      m_bindSize;        // tamano del bind
  //
  VARTYPE   m_vtRowType;       // tipo del vector final
};

class TColumn 
  :  public TIDISPATCH<IIColumn, &IID_Column>, 
     protected TColumnD
{
public:
  /// IUnknown
  ULONG __stdcall AddRef() { ++m_cRef; return m_IICursor.AddRef(); }
  ULONG __stdcall Release(){ --m_cRef; return m_IICursor.Release(); }

  // Column 
  virtual HRESULT __stdcall get_Name(BSTR* retv);
  virtual HRESULT __stdcall get_Position(short* retv);

  virtual HRESULT __stdcall get_DisplaySize(long* retv);
  virtual HRESULT __stdcall get_Precision(short* retv);
  virtual HRESULT __stdcall get_Scale(short* retv);
  virtual HRESULT __stdcall get_IsNullOk(VARIANT_BOOL* retv);

  virtual HRESULT __stdcall get_RowCount(long* retv);
  virtual HRESULT __stdcall get_IsNull(long Index, VARIANT_BOOL* retv);
  virtual HRESULT __stdcall get_Length(long Index, short* retv);
  virtual HRESULT __stdcall get_RC(long Index, short* retv);
  virtual HRESULT __stdcall get_Ref(VARIANT* retv);
  virtual HRESULT __stdcall get_Copy(VARIANT* retv);

  // IIColumn
  virtual string& Name() { return m_sName; }       
  virtual sword   Pos()  { return m_pos; }        
  virtual void    PreWork();   
  virtual void    PosWork(); 
  virtual void    PosWorkPreRAISE();
  
  //TColumn
  HRESULT  IDispatchSEH();

  TColumn(TColumnD& cd);
  virtual ~TColumn();

protected:
  virtual bool ValidateIndex(long); // podia estar el la IIF
  virtual long GetRowsXFetch() {return m_IICursor.GetRowsXFetch(); }

protected:
  //  bool        m_fInFetch;  //
  //
  SAFEARRAY*  m_psaRows;   // data    
  // ::odefin
  sb2*        m_pIndp;     // x col fetch null-status
  ub2*        m_pRLen;     // x col len del la data movida
  ub2*        m_pRC;       // x col err code
};

inline
HRESULT TColumn::IDispatchSEH() {
  return ::IDispatchSEH(m_pEP, m_excepInfo, m_fException, m_rIID, 
                        *m_IICursor.GetLDA(), m_IICursor.GetCDA()->rc); 
}

// class TFIXEDCHARCol
class TBSTRColumn : public TColumn
{
public:
  // TColumn
  void PreWork();   
  void PosWork(); 
  void PosWorkPreRAISE();
  // 
  TBSTRColumn(TColumnD& cd);
  virtual ~TBSTRColumn();

protected:
  void* m_pTmp;
};

class TDATEColumn : public TColumn
{
public:
  // TColumn
  void PreWork();   
  void PosWork();   
  void PosWorkPreRAISE();
  // 
  TDATEColumn(TColumnD& cd);
  virtual ~TDATEColumn();

protected:
  ODATE* m_pTmp;
};

//class TERRColumn;

#endif //_COLUMN_HXX_
