#if !defined(_CURSOR_HXX_)
#define _CURSOR_HXX_

#include "icursor.hxx"
#include "icolumn.hxx"
#include "iparam.hxx"
#include "icolumns.hxx"
#include "iparams.hxx"

/**
***
***/
class ECursor
{
  friend class TCursor;
public:
  virtual void get_EOF(TCursor* pC, VARIANT_BOOL* retv);

  //virtual void Enter(TCursor* pC);
  //virtual void Exit(TCursor* pC);
  //
  //Clear
  virtual void Open(TCursor* pC);
  virtual void ParseSQL(TCursor* pC, string& sSql);
  virtual void DefineColumns(TCursor* pC);
  virtual void BindParam(TCursor* pC, VARIANT& Wich, VARIANT& Value, 
			 VARIANT& AsType, VARIANT& Length);
  virtual void Bind(TCursor* pC, VARIANT& Wich, 
		    dboVarType AsType, VARIANT& StringLength, Param** retv);
  virtual void BindArray(TCursor* pC, BSTR Wich, short ArraySize, 
			 dboVarType AsType, VARIANT& StringLength, Param** retv);
  virtual void Execute(TCursor* pC, VARIANT& N, VARIANT& Offset);  
  virtual void Fetch(TCursor* pC, VARIANT_BOOL* retv);  
  virtual void Close(TCursor* pC);
  //
protected:
  //virtual void ChangeState(TCursor* pC, ECursor* pState); 
};

ECursor* EClosed_TheInstance();
ECursor* EOpen_TheInstance();
ECursor* EDeferredParse_TheInstance();

ECursor* EPLParsed_TheInstance();
ECursor* EPLExecuted_TheInstance();
ECursor* EPLEOF_TheInstance();

ECursor* EQryParsed_TheInstance();
ECursor* EQryExecuted_TheInstance();
ECursor* EQryFetched_TheInstance();
ECursor* EQryEOF_TheInstance();

/**
*** Cursor
***/
class TCursor
  :  public TIDISPATCH<IICursor, &IIDCURSOR>
{
  friend class EClosed; friend class EOpen; friend class EDeferredParse; friend class EQryParsed; 
  friend class EQryExecuted; friend class EQryFetched; friend class EQryEOF;
  friend class EPLParsed; friend class EPLExecuted; friend class EPLEOF;
  friend void  CreateCursor(IISession& session, IICursor*& rpCursor);  
public:
  /// Cursor Propiedades
  STDMETHOD(get_EOF)(VARIANT_BOOL* retv);
  STDMETHOD(get_RowCount)(long* retv);
  STDMETHOD(get_RPC)(long* retv);
  STDMETHOD(get_RC)(short* retv);
  STDMETHOD(put_RowsXFetch)(long Max);
  STDMETHOD(get_RowsXFetch)(long* retv);
  STDMETHOD(get_IsDirty)(VARIANT_BOOL* retv);
  virtual HRESULT __stdcall put_Strict(VARIANT_BOOL Strict);
  virtual HRESULT __stdcall get_Strict(VARIANT_BOOL* retv);

  /// Params
  STDMETHOD(get_Params)(Params** ppParams);
  /// Columns
  STDMETHOD(get_Cols)(Columns** ppColumns);
  /// Metodos
  STDMETHOD(ParseSQL)(BSTR Sql);  
  STDMETHOD(DefineColumns)();
  STDMETHOD(DefineColumnAs)(short Position, dboVarType AsType);
  HRESULT __stdcall Bind(VARIANT Wich, 
			 dboVarType AsType, VARIANT StringLength, Param** retv);
  HRESULT __stdcall BindArray(BSTR Wich, short ArraySize, 
			      dboVarType AsType, VARIANT StringLength, Param** retv);
  HRESULT __stdcall BindCursor(BSTR Wich, Cursor** retv);
  STDMETHOD(Fetch)(VARIANT_BOOL* retv);
  STDMETHOD(Execute)(VARIANT N, VARIANT Start);
  // chau orden
  STDMETHOD(BindParam)(VARIANT Wich, VARIANT Value, VARIANT AsType, VARIANT Length);

  HRESULT __stdcall Dummy1() {}
  HRESULT __stdcall Dummy2() {}  
  HRESULT __stdcall Dummy3() {}  
  HRESULT __stdcall Dummy4() {}

  // IICursor
  operator Cda_Def*()                {return &m_cda;}  //operator Lda_Def*() {return  m_IISession;}
  bool     IsStrict()                {return _fStrict; }
  //
  Cda_Def*  GetCDA()                 {return &m_cda;}
  Lda_Def*  GetLDA()                 {return m_IISession.GetLDA();}
  ULONG     GetRowsXFetch()          {return m_ulRowsXFetch;}
  ULONG     GetCount()               {return m_cda.rpc - _lastRpc/*m_ulCount*/;}
  IIColumn* GetColumn(int index);
  IIColumn* GetColumn(string& name);
  int       GetColumnCount()         {return m_vCols.size();}
  IIParam*  GetParam(sword Number);
  IIParam*  GetParam(string& name);
  int       GetParamCount()          {return m_vParam.size();}
  void      SetDirtyFlag()           {m_fIsDirty = true;}
  HRESULT   NewEnumColumns(IUnknown** ppIUnknown);
  HRESULT   NewEnumParams(IUnknown** ppIUnknown);

  // TCursor
  HRESULT  IDispatchSEH();

  void _BindParam(VARIANT& Wich, VARIANT& Value, VARIANT& AsType, VARIANT& Length);
  void _Bind(VARIANT& Wich, dboVarType AsType, VARIANT& StringLength, Param** retv);
  void _BindArray(BSTR Wich, short ArraySize, 
		  dboVarType AsType, VARIANT& StringLength, Param** retv);
  void _DefineColumns();
  void _UndefineColumns();
  void _UndefineColumn(int pos);
  void _UndefineColumn(string& name);
  void _Execute(VARIANT& N, VARIANT& Offset);
  bool _ExecutePL(VARIANT& N, VARIANT& Offset);
  void _Fetch(bool fExecute, VARIANT_BOOL* retv);
  void _Clear();
  void _Close();

  void ChangeState(ECursor* pState); 
  //

//protected:
  TCursor(IISession& isession);
  ~TCursor();

protected:
  // ISession*
  IISession&            m_IISession;        // Server del que dependo
  Cda_Def               m_cda;
  //
  ECursor*              _pState;
  bool                  m_fIsDirty;
  bool                  _fStrict;
  // bool m_fNeedReparse;

  bool                  m_fColsDefined;
  //
  string                m_sTextoSql;
  ULONG                 m_ulRowsXFetch;     // Tamaño del array interno

  ub4                   _lastRpc;
  //
  IIColumns*                        m_pColumns;
  typedef vector<AP<IIColumn> >     V_AP_IICOLUMN;
  V_AP_IICOLUMN                     m_vCols;       
  map<string, long, less<string> >  m_mName2Col;
  //

  // Params 

  // cambiar estos typedef a container independiente

  typedef vector<AP<IIParam> >                 V_AP_IIPARAM;
  //typedef map<long, AP<IIParam>, less<long> >  MAP_UNIQUE_2_AP_IIPARAM;
  typedef map<string, IIParam*, less<string> > MAP_NAME_2_P_IIPARAM;
  typedef map<sword,  IIParam*, less<sword> >  MAP_NUMBER_2_P_IIPARAM;

  IIParams*              m_pParams; 
  //  MAP_UNIQUE_2_AP_IIPARAM m_vParam;  
  V_AP_IIPARAM           m_vParam; 
  MAP_NAME_2_P_IIPARAM   m_mapName2P;
  MAP_NUMBER_2_P_IIPARAM m_mapNumber2P;
};

inline
HRESULT TCursor::IDispatchSEH() {
  return ::IDispatchSEH(m_pEP, m_excepInfo, m_fException, m_rIID, *m_IISession.GetLDA(), m_cda.rc); 
}

/**
*** Algoritmos comunes a Cols y params
***/
template <class T>
class FPreWork : private unary_function<T, void>
{
public:
  void operator()(const T& elemento) { /*if (!elemento.is_empty())*/ elemento->PreWork(); }
};

template <class T>
class FPosWork : private unary_function<T, void>
{
public:
  void operator()(const T& elemento) { elemento->PosWork(); }
};

template <class T>
class FPosWorkPreRAISE : private unary_function<T, void>
{
public:
  void operator()(const T& elemento) { elemento->PosWorkPreRAISE(); }
};

// una funcion FRelease() para liberar y destruir las columnas!

#endif // _CURSOR_HXX_




