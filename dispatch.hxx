/**
*** DBO2(1997/1998)
*** rodablo@hotmail.com
***/
/// soporte de errores para vtable
class TISEI : public ISupportErrorInfo
{
public:
  TISEI(IUnknown* pUnkObj, const IID* riid)
    :  m_pUnkObj(pUnkObj), m_riid(*riid) {}
  // IUnknown
  STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppvObj) {
    return m_pUnkObj->QueryInterface(riid, ppvObj);}
  STDMETHOD_(ULONG, AddRef)() { 
    return m_pUnkObj->AddRef(); }
  STDMETHOD_(ULONG, Release)() { 
    return m_pUnkObj->Release(); }
  // ISupportErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid) {
    return (riid == m_riid) ? NOERROR : ResultFromScode(S_FALSE);}
private:
  LPUNKNOWN  m_pUnkObj; // IUnknown del Objeto que la implementa
  const IID& m_riid;
};

/**
***
*** TException
***/
class TException
{
public:
  TException(_EXCEPTION_POINTERS* pEP){m_pEP = pEP;}

  //protected:
  //  TISEI         m_ISEI;     	// vtable errors
  // EXCEPINFO	m_excepInfo;	// automation
  _EXCEPTION_POINTERS* m_pEP;
};

//  class TOracleException : public TException
//  {
//  };
//  class TInternalException : public TException
//  {
//  };
//  class THRESULTException : public TException
//  {
//  };
//  class TOSException : public TException
//  {
//  public:
//  protected:
//    LPEXCEPTION_POINTERS  m_pEP;    
//  };

/**
*** TCriticalSection
***/
class TCriticalSection
{
public:
  explicit TCriticalSection() { ::InitializeCriticalSection(&_critical_section); }
  ~TCriticalSection()         { ::DeleteCriticalSection(&_critical_section); }
  //
  void Enter() { ::EnterCriticalSection(&_critical_section); }
  void Leave() { ::LeaveCriticalSection(&_critical_section); }
  //
  class Lock {
  public:
    explicit Lock(TCriticalSection& cs) : _cs(cs) { _cs.Enter(); }
    ~Lock() { _cs.Leave(); }
  private:
    TCriticalSection& _cs;
  };
private:
  CRITICAL_SECTION _critical_section;
};

/**
***
***
*** TEMPLATE TUNKNOWN
***/
// template <class IBase, const IID& rIID>
// class TUNKNOWN
//   :  public IBase
// {
// }

/**
***
***
*** TEMPLATE TDISPATCH
***/
                           //IID deberia ser &
template <class IBase, const IID* pIID>
class TIDISPATCH
  :  public IBase
{
public:
  /// IUnknown
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppvObj);
  virtual ULONG   STDMETHODCALLTYPE AddRef() { 
    TCriticalSection::Lock lock(m_oCs);
    return ++m_cRef; 
  }
  virtual ULONG   STDMETHODCALLTYPE Release();
  
  /// IDispatch
  virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) {
    *pctinfo = 1; return NOERROR; }
  virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTI, LCID lcid, ITypeInfo** ppTI);
  virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID /*riid*/, OLECHAR** rgszNames, 
 						  UINT cNames, LCID /*lcid*/, DISPID* rgdispid) {
    return DispGetIDsOfNames(m_pTI, rgszNames, cNames, rgdispid); }
  virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispidMember, REFIID riid, LCID lcid, 
					   WORD wFlags, DISPPARAMS* pdispparams, 
					   VARIANT* pvarResult, EXCEPINFO* pexcepinfo, 
					   UINT* puArgErr);
  /// soporte SEH
public:
//   void          RAISE_INTERNAL(DWORD dwExceptionCode, ...);
//   void          RAISE_LASTERROR_();
// #define         RAISE_LASTERROR RAISE_LASTERROR_()
//   void          RAISE_HRESULT(HRESULT hr);
//   void          RAISE_OCI(sword stat);
//   inline void   CHECK_HRESULT(HRESULT hr);
//   inline void   CHECK_OCI(sword stat);

  //  DWORD           IDispatchSEHFilter(LPEXCEPTION_POINTERS pEP);
  virtual HRESULT IDispatchSEH() = 0;

  /// implementacion
  virtual bool ZeroRef() {return 0 == m_cRef;}

public:
  TIDISPATCH();
  virtual ~TIDISPATCH();  

protected:

protected:
  TCriticalSection 	m_oCs;  
  volatile ULONG        m_cRef;             // ref count
  const IID&            m_rIID;
  LPTYPEINFO            m_pTI;              // typeinfo
  //excepciones
  TException*           m_pEx;
  //SEH
  TISEI                 m_ISEI;             // vtable errors
  EXCEPINFO             m_excepInfo;
  bool                  m_fException;
  LPEXCEPTION_POINTERS  m_pEP;              // Aqui pasa la info el SEH
};

extern void LoadTypeInfo(const IID& rIID, LPTYPEINFO& rpTI);


template <class IBase, const IID* pIID>
TIDISPATCH<IBase, pIID>::TIDISPATCH() 
  :  m_ISEI((IUnknown*)this, pIID),
     m_rIID(*pIID) 
{
  m_cRef       = 0;
  m_pTI        = 0;
  m_fException = false;
  m_pEx        = 0;
  //
  //  extern void LoadTypeInfo(const IID& rIID, LPTYPEINFO& rpTI);
  LoadTypeInfo(m_rIID, m_pTI);
}

template <class IBase, const IID* pIID>
TIDISPATCH<IBase, pIID>::~TIDISPATCH() {  
  if (0 != m_pTI)
    {
    m_pTI->Release(); 
    m_pTI = 0;
    }
  // 
  if (0 != m_pEx)
    delete m_pEx;
}

template <class IBase, const IID* pIID>
inline HRESULT STDMETHODCALLTYPE 
TIDISPATCH<IBase, pIID>::QueryInterface(REFIID riid, LPVOID* ppvObj) 
{
  TCriticalSection::Lock lock(m_oCs);
  if (0 == ppvObj)
    return E_INVALIDARG;

  *ppvObj = 0;
  // es vtable o ISupportErrorInfo?
  if (IsEqualIID(riid, IID_IDispatch) ||
      IsEqualIID(riid, IID_IUnknown)  ||
      IsEqualIID(riid, m_rIID))
    *ppvObj = this;
  else if (IsEqualIID(riid, IID_ISupportErrorInfo))
    *ppvObj = &m_ISEI;
  else
    return E_NOINTERFACE;

  // retorna retenida
  ((IUnknown*)*ppvObj)->AddRef(); 
  return NOERROR;
}

template <class IBase, const IID* pIID>
ULONG STDMETHODCALLTYPE
TIDISPATCH<IBase, pIID>::Release()
{
  // toma el lock
  m_oCs.Enter();
  //
  if(0 == --m_cRef)
   {
      // libera el lock antes de suicidarse
      m_oCs.Leave();
      // se suicida
      delete this; 
      //
      return 0L;   // no puede retornar m_cRef porque aqui ya no existe
    }
  //
  m_oCs.Leave();
  return m_cRef;
}

template <class IBase, const IID* pIID>
HRESULT STDMETHODCALLTYPE
TIDISPATCH<IBase, pIID>::GetTypeInfo(UINT iTI, LCID lcid, ITypeInfo** ppTI)
{
  *ppTI = 0;
  // ver SDK
  if (0 != iTI)
    return DISP_E_BADINDEX;
  // siempre antes de...
  m_pTI->AddRef();
  // ...pasarla
  *ppTI = m_pTI;
  return NOERROR;
}

template <class IBase, const IID* pIID>
HRESULT STDMETHODCALLTYPE
TIDISPATCH<IBase, pIID>::Invoke(DISPID dispidMember, REFIID riid,
				LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, 
				VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr)
{
  // antes de de invocar el metodo
  m_fException = false;

  /*
    The calling code is responsible for releasing all strings and objects referred to by rgvarg[ ] or placed in *pVarResult. As with other parameters that are passed by value, if the invoked member must maintain access to a string after returning, you should copy the string. Similarly, if the member needs access to a passed-object pointer after returning, it must call the AddRef function on the object. A common example occurs when an object property is changed to refer to a new object, using the DISPATCH_PROPERTYPUTREF flag.
  */

  HRESULT hr = DispInvoke(this, m_pTI, dispidMember, wFlags,
                          pdispparams, pvarResult, pexcepinfo, puArgErr);
  if (!m_fException)
    return hr;
  else
    {
      if (0 != pexcepinfo)
        CopyMemory(pexcepinfo, &m_excepInfo, sizeof(EXCEPINFO));

      return DISP_E_EXCEPTION;
    }
}

/**
*** SEH
***/
//  template <class IBase, const IID* pIID>
//  inline DWORD
//  TIDISPATCH<IBase, pIID>::IDispatchSEHFilter(LPEXCEPTION_POINTERS pEP) {
//    if (EXCEPTION_SINGLE_STEP != pEP->ExceptionRecord->ExceptionCode) {
//      m_pEP = pEP;                        // 1. salva la info en la instancia
//      return EXCEPTION_EXECUTE_HANDLER;
//    }
//    else
//      return EXCEPTION_CONTINUE_SEARCH;
//  }

HRESULT
IDispatchSEH(LPEXCEPTION_POINTERS& rpEP, 
	     EXCEPINFO&            rExcepInfo,
	     bool&                 rfException,
	     const IID&            rIID,
	     Lda_Def&              rLda, 
	     ub2                   rc);

void 
RAISE_INTERNAL(DWORD dwExceptionCode, ...);

inline void
RAISE_LASTERROR_() {
  // Obtiene el error
  DWORD dwErr = GetLastError();
  // genera la excepcion y le pasa el ¿array? (de 1) que es COPIADO por RaiseException
  RaiseException(DBO_E_LASTERROR, EXCEPTION_NONCONTINUABLE_EXCEPTION, 1, (DWORD*)&dwErr);
}

inline void
RAISE_HRESULT(HRESULT hr) {
  // crea un temporal (no se si es necesario pero el arg del inline puede ser register)
  HRESULT local = hr;
  // genera la excepcion y le pasa el ¿array? (de 1) que es COPIADO por RaiseException
  RaiseException(DBO_E_HRESULT, EXCEPTION_NONCONTINUABLE_EXCEPTION, 1, (DWORD*)&local);
}

inline void 
RAISE_OCI(sword stat) {
  // crea un temporal (no se si es necesario pero el arg del inline puede ser register)
  sword local = stat;
  RaiseException(DBO_E_ORACLE, EXCEPTION_NONCONTINUABLE, 1, (DWORD*)&local);
}

inline void
CHECK_HRESULT(HRESULT hr) {
  if (FAILED(hr))
    RAISE_HRESULT(hr);
}

inline void
CHECK_OCI(sword stat) {
  if (0 != stat)
    RAISE_OCI(stat);
}

//
//  #define __AUTO_EXCEPT						\
//  __except (IDispatchSEHFilter(GetExceptionInformation())) {	\
//    return IDispatchSEH(); }					\
//  return NOERROR
//

#define __AUTO_EXCEPT		\
    catch(TException e) {	\
        m_pEP = e.m_pEP;        \
        return IDispatchSEH();  \
        }	                \
return NOERROR
