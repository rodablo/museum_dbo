/**
***     DBO2(1997/1998)
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "isession.hxx"

/**
*** TSessionCF
***/
class TSessionCF : public IClassFactory  
{
public:
  TSessionCF(){ m_cRef = 0; };
  // IUnknown
  STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppvObj);
  STDMETHOD_(ULONG, AddRef)();
  STDMETHOD_(ULONG, Release)();
  // IClassFactory 
  STDMETHOD(CreateInstance)(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID* ppvObj);
  STDMETHOD(LockServer)(BOOL fLock);
private:
  ULONG m_cRef;
};

STDMETHODIMP
TSessionCF::QueryInterface(REFIID riid, void** ppvObj)
{
  *ppvObj = 0;

  if (IsEqualIID(riid, IID_IUnknown) ||
      IsEqualIID(riid, IID_IClassFactory)
      )
    *ppvObj = this;
  else
    return E_NOINTERFACE;
  // retorna retenida
  ((IUnknown*)*ppvObj)->AddRef();
  return NOERROR;
}

ULONG STDMETHODCALLTYPE
TSessionCF::AddRef()
{
  return ++m_cRef;
}

ULONG STDMETHODCALLTYPE
TSessionCF::Release()
{   
  if (0 == --m_cRef)
    {
      delete this;
      return 0L;
    }
  return m_cRef;
}

//CLASS_E_NOTLICENSED
STDMETHODIMP
TSessionCF::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID* ppvObj)
{
  __try {
    // pesimismo inicial
    *ppvObj = 0;
    // No soporta agregacion
    if (0 != pUnkOuter)
      return CLASS_E_NOAGGREGATION;
    // verifica la licencia
    extern HRESULT LicValidate();
    HRESULT hr = LicValidate();
    if (FAILED(hr))
	return hr;
    // crea una instancia
    IISession* pS;
    CreateSession(pS);
    if (0 == pS)
      return E_OUTOFMEMORY;
    // Solicita la interfaz
    if (FAILED(pS->QueryInterface(riid, ppvObj)))
      {
	delete pS;
	return E_OUTOFMEMORY;
      }
    // retorna ok!
    return NOERROR;
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
    {
      return E_OUTOFMEMORY;
    }
}

STDMETHODIMP
TSessionCF::LockServer(BOOL fLock)
{
  if (fLock)
    g_cLock++;
  else
    g_cLock--;

  return NOERROR;
}

/**
***
***/
void
CreateSessionCF(IClassFactory*& rpSCF)
{
  rpSCF = (IClassFactory*) new TSessionCF;
}

