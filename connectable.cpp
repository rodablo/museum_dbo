/**
*** DBO2X(R)1996-2001
*** rodablo@rodablo.com
***/
#include "pch.hxx"
#include "dbo.hxx"

/**
*** TConnectionPoint
***/
TConnectionPoint::TConnectionPoint(TConection * pC)
  :  m_pC(pC), m_cookie(77777)
{
}

TConnectionPoint::~TConnectionPoint(void)
{
  ReleaseSink();
}

STDMETHODIMP_(ULONG) 
TConnectionPoint::AddRef()  
{ 
  return m_pC->AddRef(); 
} 

STDMETHODIMP_(ULONG) 
TConnectionPoint::Release() 
{ 
  return m_pC->Release();
}

STDMETHODIMP 
TConnectionPoint::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
  if (0 == ppvObj)
    return ResultFromScode(E_INVALIDARG);
  //
  if (IsEqualIID(riid, IID_IConnectionPoint) ||
      IsEqualIID(riid, IID_IUnknown)) {
    // retorna retenida
    *ppvObj = this;
    ((IUnknown*)*ppvObj)->AddRef(); 
    return NOERROR;
  }
  // fallo
  *ppvObj = 0; 
  return ResultFromScode(E_NOINTERFACE);
}

STDMETHODIMP 
TConnectionPoint::GetConnectionInterface(IID * pIID)
{
  if (0 == pIID)
    return ResultFromScode(E_POINTER);
  // retorna el iid de la interfaz 
  *pIID = m_pC->m_iid;
  return NOERROR;
}

STDMETHODIMP 
TConnectionPoint::GetConnectionPointContainer(IConnectionPointContainer ** ppCPC)
{	
  return m_pC->QueryInterface
    (IID_IConnectionPointContainer, (LPVOID*)ppCPC);
}

STDMETHODIMP 
TConnectionPoint::Advise(LPUNKNOWN pUnkSink, DWORD *pdwCookie)
{
  // ESTO ASUME QUE LA INTARFAZ ES IDISPATCH O DUAL, ESTO ESTA ACLARADO
  // EN EL TYPELIB ASUMO QUE CLIENTES OTROS QUE EL VB NO PASEN VTABLES 
  IDispatch * pSink; 
  *pdwCookie = 0;

  // verifica que no este conectado (solo implementa una coneccion)
  if (0 != m_pC->m_pSink)
    return ResultFromScode(CONNECT_E_ADVISELIMIT);
  
  // verifica la interface (y la otiene)
  if (FAILED(pUnkSink->QueryInterface(m_pC->m_iid, (void**)&pSink)))
    return ResultFromScode(CONNECT_E_CANNOTCONNECT);

  *pdwCookie = m_cookie;
  m_pC->m_pSink = pSink;
  return NOERROR;
}

STDMETHODIMP 
TConnectionPoint::Unadvise(DWORD dwCookie)
{
  if (0 == dwCookie)
    return ResultFromScode(E_INVALIDARG);

  if (m_cookie != dwCookie)
    ResultFromScode(CONNECT_E_NOCONNECTION);

  ReleaseSink();

  return NOERROR;
}

STDMETHODIMP 
TConnectionPoint::EnumConnections(LPENUMCONNECTIONS *ppEnum)
{
  *ppEnum = 0;
  return ResultFromScode(E_NOTIMPL);
}

void 
TConnectionPoint::ReleaseSink()
{
  if (0 != m_pC->m_pSink) {
    m_pC->m_pSink->Release();
    m_pC->m_pSink = 0;
  }
}

/**
*** TConection
***/
TConection::TConection(LPUNKNOWN pUnkOuter, IID iid) 
  :  
  m_pUnkOuter(pUnkOuter), m_cRef(0), 
  m_iid(iid), m_pSink(0), m_cp(this)
{
}

TConection::~TConection()
{
}

STDMETHODIMP 
TConection::EnumConnectionPoints(IEnumConnectionPoints ** ppEnum)  
{
  *ppEnum = 0;
  return ResultFromScode(E_NOTIMPL);
} 

STDMETHODIMP 
TConection::FindConnectionPoint(REFIID riid, IConnectionPoint ** ppCP)   
{
  if (0 == ppCP)
    return ResultFromScode(E_INVALIDARG);
  *ppCP = 0;    
  if (IsEqualIID(riid, m_iid)) 
    return m_cp.QueryInterface(IID_IConnectionPoint, (void**)ppCP);        
  // fallo
  return ResultFromScode(E_NOINTERFACE);    
}      
