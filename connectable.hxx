/**
*** DBO2X(R)1996-2001
*** rodablo@rodablo.com
***/

/**
*** TConnectionPoint
***/
class TConection;
class TConnectionPoint : public IConnectionPoint
{
public:
  TConnectionPoint(TConection * pC);
  ~TConnectionPoint();
  //IUnknown 
  STDMETHODIMP         QueryInterface(REFIID riid, LPVOID* ppvObj);
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();
  //IConnectionPoint 
  STDMETHODIMP GetConnectionInterface(IID * pIID);
  STDMETHODIMP GetConnectionPointContainer(IConnectionPointContainer ** ppCPC);
  STDMETHODIMP Advise(LPUNKNOWN pUnk, DWORD * pdwCookie);
  STDMETHODIMP Unadvise(DWORD dwCookie);
  STDMETHODIMP EnumConnections(IEnumConnections ** ppEnum);
  //
  void ReleaseSink();
private:
  TConection* m_pC; 
  const DWORD m_cookie;
};

/**
*** TConection
***/
class TConection : public IConnectionPointContainer
{
  friend class TConnectionPoint;
public:
  TConection(LPUNKNOWN pUnkOuter, IID iidSink);
  ~TConection();
  // IUnknown
  STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj) {
    return m_pUnkOuter->QueryInterface(riid, ppvObj);}
  STDMETHODIMP_(ULONG) AddRef() { ++m_cRef; return m_pUnkOuter->AddRef();  }
  STDMETHODIMP_(ULONG) Release(){ --m_cRef; return m_pUnkOuter->Release(); }
  //IConnectionPointContainer members         
  STDMETHODIMP EnumConnectionPoints(IEnumConnectionPoints ** ppEnum);     
  STDMETHODIMP FindConnectionPoint(REFIID riid, IConnectionPoint ** ppCP);
  // TConection
  bool 		HasSink() { return 0 != m_pSink; }
  IDispatch * 	Sink()    { return m_pSink; }
  //
private:				
  LPUNKNOWN        m_pUnkOuter;	// IUnknown del objeto que controla
  ULONG            m_cRef;      // ref count (solo para ver en que estado esta)
  IID	      	   m_iid;  	// iid del sink 
  LPDISPATCH       m_pSink;
  TConnectionPoint m_cp; 	// unico punto de connexion
};
