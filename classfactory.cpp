/**
***     DBO2(1997/1998)
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "isession.hxx"

#if 0
TSessionCF::TSessionCF()
{
  m_cRef = 0;
}

STDMETHODIMP
TSessionCF::QueryInterface(REFIID riid, void** ppvObj)
{
  *ppvObj = 0;

  if (IsEqualIID(riid, IID_IUnknown)      ||
      IsEqualIID(riid, IID_IClassFactory))
    *ppvObj = this;
#if __ICLASSFACTORY2__
  else if (IsEqualIID(riid, IID_IClassFactory2))
    *ppvObj = this;
#endif
  else
    return E_NOINTERFACE;

  // retorna retenida
  ((IUnknown*)*ppvObj)->AddRef();

  return NOERROR;
}


STDMETHODIMP_(ULONG)
  TSessionCF::AddRef()
{
  return ++m_cRef;
}


STDMETHODIMP_(ULONG)
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
TSessionCF::CreateInstance(LPUNKNOWN pUnkOuter,
			   REFIID riid,
			   LPVOID* ppvObj)
{
#if 1
  try {
    // pesimismo inicial
    *ppvObj = 0;
    // No soporta agregacion
    if (0 != pUnkOuter)
      return CLASS_E_NOAGGREGATION;
    // vencio
    if (a())
      return E_OUTOFMEMORY;//DBO_E_VENCIO;

    // crea una instancia
    IISession* pS;
    CreateSession(pS);

    if (0 == pS)
      return E_OUTOFMEMORY;
    // Solicita la interfaz
    // if (FAILED(pS->TTDispatch::QueryInterface(riid, ppvObj)))
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
#else
  return CLASS_E_NOTLICENSED;
#endif
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

#if __ICLASSFACTORY2__

STDMETHODIMP
TSessionCF::GetLicInfo(LPLICINFO pLicInfo)
{
  pLicInfo->cbLicInfo = sizeof(LICINFO);
  pLicInfo->fRuntimeKeyAvail = false;
  pLicInfo->fLicVerified     = false;
  return NOERROR;
}

STDMETHODIMP
TSessionCF::RequestLicKey(DWORD dwResrved, BSTR* pbstrKey)
{
  *pbstrKey = WIDE("TOTO").SysAllocString();
  return NOERROR;
}


STDMETHODIMP
TSessionCF::CreateInstanceLic(LPUNKNOWN pUnkOuter, LPUNKNOWN pUnkReserved, 
			      REFIID riid, BSTR bstrKey, LPVOID* ppvObj)
{

  try {
    // pesimismo inicial
    *ppvObj = 0;
    // No soporta agregacion
    if (0 != pUnkOuter)
      return CLASS_E_NOAGGREGATION;

    if ("TOTO" != string(TString(bstrKey)))
	return CLASS_E_NOTLICENSED;

    // vencio
    if (a())
      return E_OUTOFMEMORY;//DBO_E_VENCIO;

    // crea una instancia
    IISession* pS;
    CreateSession(pS);

    if (0 == pS)
      return E_OUTOFMEMORY;
    // Solicita la interfaz
    // if (FAILED(pS->TTDispatch::QueryInterface(riid, ppvObj)))
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

  //  * ppvObject = 0;
  //return E_NOTIMPL;//CLASS_E_NOTLICENSED;

}

#endif


#else

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
  STDMETHOD(CreateInstance)(LPUNKNOWN pUnkOuter,
                            REFIID riid, LPVOID* ppvObj);
  STDMETHOD(LockServer)(BOOL fLock);
private:
  ULONG m_cRef;
};

STDMETHODIMP
TSessionCF::QueryInterface(REFIID riid, void** ppvObj)
{
  *ppvObj = 0;

  if (IsEqualIID(riid, IID_IUnknown) ||
      IsEqualIID(riid, IID_IClassFactory))
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
  try {
    // pesimismo inicial
    *ppvObj = 0;
    // No soporta agregacion
    if (0 != pUnkOuter)
      return CLASS_E_NOAGGREGATION;
    // vencio
    //  return E_OUTOFMEMORY;//DBO_E_VENCIO;

    SYSTEMTIME st;
    GetLocalTime(&st);

//     if ((st.wYear - 1110) != (1998 - 1110) ||
// 	(st.wMonth + 432) != (2 + 432))
//       return E_UNEXPECTED;//E_OUTOFMEMORY;

    extern bool LicValidate();
    if (!LicValidate())
      return E_UNEXPECTED;//E_OUTOFMEMORY;

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

IClassFactory* 
TSessionCF_TheInstance()
{
  /*
    Si la instanci es unica genera gpf cuando sale
   */
  /*static*/ TSessionCF* p/* = 0*/;
  // si no existe lo crea
  //  if (0 == p)
      p = new TSessionCF;
  // 
  return (IClassFactory*)p;
}




#endif
