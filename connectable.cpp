/**
*** DBO2X(R)1996-2001
*** rodablo@rodablo.com
***/
#include "pch.hxx"
#include "dbo.hxx"


/**
*** TConnectionPoint
***/
class TConnectionPoint : public IConnectionPoint
{
public:
  TConnectionPoint(TSimpleConnectionPoint * pCPC);
  ~TConnectionPoint();
  //IUnknown 
  STDMETHODIMP         QueryInterface(REFIID riid, LPVOID* ppvObj);
  STDMETHODIMP_(ULONG) AddRef() { return m_pCPC->AddRef(); } 
  STDMETHODIMP_(ULONG) Release(){ return m_pCPC->Release();}
  //IConnectionPoint 
  STDMETHODIMP GetConnectionInterface(IID * pIID);
  STDMETHODIMP GetConnectionPointContainer(IConnectionPointContainer ** ppCPC);
  STDMETHODIMP Advise(LPUNKNOWN pUnk, DWORD * pdwCookie);
  STDMETHODIMP Unadvise(DWORD dwCookie);
  STDMETHODIMP EnumConnections(IEnumConnections ** ppEnum);

private:
  TSimpleConnectionPoint* m_pCPC; 
  IID	      	m_iid;
  const DWORD   m_cookie;
};
  



//CConnectionPoint implementation

/*
 * CConnectionPoint::CConnectionPoint
 * CConnectionPoint::~CConnectionPoint
 *
 * Parameters (Constructor):
 *  pObj            PCBeeper of the object we're in.  We can
 *                  query this for the IConnectionPointContainer
 *                  interface we might need.
 */
//This object only supports one connection per point

#define ADVISEKEY   72388       //Arbitrary


TConnectionPoint::TConnectionPoint(TSimpleConnectionPoint * pCPC)
  :  m_pCPC(pCPC), m_cookie(2)
{
}

TConnectionPoint::~TConnectionPoint(void)
{
  //ReleaseInterface(m_pObj->m_pIPropNotifySink);
}


STDMETHODIMP 
TConnectionPoint::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
  if (0 == ppvObj)
    return ResultFromScode(E_INVALIDARG);
   
  *ppvObj = 0;

  if (IsEqualIID(riid, IID_IConnectionPoint) ||
      IsEqualIID(riid, IID_IUnknown)) {
    // retorna retenida
    *ppvObj = this;
    ((IUnknown*)*ppvObj)->AddRef(); 
    return NOERROR;
  }
  // fallo
  return ResultFromScode(E_NOINTERFACE);
}

STDMETHODIMP 
TConnectionPoint::GetConnectionInterface(IID * pIID)
{
  if (0 == pIID)
    return ResultFromScode(E_POINTER);
  // retorna el iid de la interfaz 
  *pIID = m_pCPC->m_sinkIID;
  return NOERROR;
}

STDMETHODIMP 
TConnectionPoint::GetConnectionPointContainer(IConnectionPointContainer ** ppCPC)
{	
  return m_pCPC->QueryInterface
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
  if (0 != m_pCPC->m_rpSink)
    return ResultFromScode(CONNECT_E_ADVISELIMIT);
  
  // verifica la interface (y la otiene)
  if (FAILED(pUnkSink->QueryInterface(m_pCPC->m_sinkIID, (void**)&pSink)))
    return ResultFromScode(CONNECT_E_CANNOTCONNECT);

  *pdwCookie = ADVISEKEY;
  m_pCPC->m_rpSink = pSink;
  return NOERROR;
}

STDMETHODIMP 
TConnectionPoint::Unadvise(DWORD dwCookie)
{
  if (0 == dwCookie)
    return ResultFromScode(E_INVALIDARG);

  if (ADVISEKEY != dwCookie)
    ResultFromScode(CONNECT_E_NOCONNECTION);

  //  ReleaseInterface(m_pObj->m_pIPropNotifySink);
  return NOERROR;
}

STDMETHODIMP 
TConnectionPoint::EnumConnections(LPENUMCONNECTIONS *ppEnum)
{
  *ppEnum = 0;
  return ResultFromScode(E_NOTIMPL);
}

/**
*** TSimpleConnectionPoint
***/
TSimpleConnectionPoint::TSimpleConnectionPoint(LPUNKNOWN pUnkOuter, 
					       IID sinkIID, LPDISPATCH & rpSink)
  :  m_pUnkOuter(pUnkOuter), m_cRef(0), m_sinkIID(sinkIID), m_rpSink(rpSink)
{
  m_pCP = new TConnectionPoint(this);
}

TSimpleConnectionPoint::~TSimpleConnectionPoint()
{
  delete m_pCP;
}

STDMETHODIMP 
TSimpleConnectionPoint::EnumConnectionPoints(IEnumConnectionPoints ** ppEnum)  
{
//    IConnectionPoint  *rgCP[CCONNPOINTS];    
//    UINT              i;    
//    PCEnumConnectionPoints  pEnum;    *ppEnum=NULL;    
//    for (i=0; i < CCONNPOINTS; i++)        
//      rgCP[i]=(IConnectionPoint *)m_rgpConnPt[i];    
//    //Create the enumerator:  we have only one connection point.    
//    pEnum=new CEnumConnectionPoints(this, CCONNPOINTS, rgCP);    
//    if (NULL==pEnum)        
//      return ResultFromScode(E_OUTOFMEMORY);    
//    pEnum->AddRef();    
//    *ppEnum=pEnum;    
//    return NOERROR;
  return ResultFromScode(E_NOTIMPL);
} 

STDMETHODIMP 
TSimpleConnectionPoint::FindConnectionPoint(REFIID riid, IConnectionPoint ** ppCP)   
{
  *ppCP = 0;    
  if (IsEqualIID(riid, m_sinkIID)) 
    return m_pCP->QueryInterface(IID_IConnectionPoint, (void**)ppCP);        
  // fallo
  return ResultFromScode(E_NOINTERFACE);    
}      
