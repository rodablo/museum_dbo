/**
*** DBO2X(R)1996-2001
*** rodablo@rodablo.com
***/
class TSimpleConnectionPoint : public IConnectionPointContainer
{
  friend class TConnectionPoint;
public:
  TSimpleConnectionPoint(LPUNKNOWN pUnkOuter, IID sinkIID, LPDISPATCH & rpSink);
  ~TSimpleConnectionPoint();
  // IUnknown
  STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj) {
    return m_pUnkOuter->QueryInterface(riid, ppvObj);}
  STDMETHODIMP_(ULONG) AddRef() { 
    ++m_cRef; return m_pUnkOuter->AddRef(); }
  STDMETHODIMP_(ULONG) Release() { 
    --m_cRef; return m_pUnkOuter->Release(); }
  //IConnectionPointContainer members         
  STDMETHODIMP EnumConnectionPoints(IEnumConnectionPoints ** ppEnum);     
  STDMETHODIMP FindConnectionPoint(REFIID riid, IConnectionPoint ** ppCP);
  //
private:				
  LPUNKNOWN     m_pUnkOuter; 	 // IUnknown del objeto que controla
  ULONG         m_cRef;      	 // ref count (solo para ver en que estado esta)
  IID	      	m_sinkIID;	 // iid del sink 
  LPDISPATCH& 	m_rpSink;	 // 
  class TConnectionPoint * m_pCP; // fordward decl del CP         
};
