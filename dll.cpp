/**
*** DBO2(R)1997-2001
*** rodablo@rodablo.com
***/
#include "pch.hxx"
#include <initguid.h>
#include "dbo.hxx"

/**
*** 	TServer
***/



/**
***     GLOBALES
***/
TServer*  TServer::m_pTheOnlyInstance = 0;
char      g_szModuleName[] = STRINGIFY(__DLL_FNAME); // inicializar desde GetModuleName()

/**
***     DLLENTRYPOINT
***/
BOOL WINAPI 
DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID /*lpReserved*/)
{
  extern void Throw(unsigned int code, _EXCEPTION_POINTERS* pEP);

  if (DLL_PROCESS_ATTACH == fdwReason)
    {
      // 
      //extern void Throw(unsigned int code, _EXCEPTION_POINTERS* pEP);
      _set_se_translator(Throw);
      //	DebugBreak();  //asm { int 0x03 }
      // bye DLL_THREAD_ATTACH and DLL_THREAD_DETACH
      //DisableThreadLibraryCalls(hInstance);
      // crea el ¿Server? x aqui pasa una sola vez asi que no se chequea nada
      new TServer(hInstance);
      //
      return true;
    }
  else if (DLL_PROCESS_DETACH == fdwReason)
    {
      // libera typelib si lo hubiese
      extern LPTYPELIB g_pTLib;
      if (0 !=  g_pTLib) {
	g_pTLib->Release();
	g_pTLib = 0;
      }
      // destruye la representación del server
      delete TServer::TheInstance();
      //
      return true;
    }
  else if (DLL_THREAD_ATTACH == fdwReason)
    {
      _set_se_translator(Throw);
      return true;
    }
  else
    return false;
}

/**
***     DLLGETCLASSOBJECT
***/
HRESULT APIENTRY
DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppvObj)
{
  // por el new
 __try {
    // pesimismo inicial
    *ppvObj = 0;
    // valida la clase
    if (!IsEqualCLSID(rclsid, CLSID_Session))
      return CLASS_E_CLASSNOTAVAILABLE;
    // crea el classfactory
    IClassFactory* pSCF;
    CreateSessionCF(pSCF);
    if (0 == pSCF)
      return CLASS_E_CLASSNOTAVAILABLE;
    // solicita la interfaz
    HRESULT hr = pSCF->QueryInterface(riid, ppvObj);
    if (FAILED(hr)) {
      delete pSCF;
      return hr;
    }
    //
    return NOERROR;
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
    {
      // cuando???
      if (0 != *ppvObj)
        ((IUnknown*)*ppvObj)->AddRef();
      //
      return E_OUTOFMEMORY;
    }
}

/**
***     DLLCANUNLOADNOWN
***/
STDAPI
DllCanUnloadNow()
{
  return TServer::TheInstance()->DllCanUnloadNow();
}



/**
***
***/
bool
GetShortFromVariant(short& dest, VARIANT& src)
{
  // en funcion del tipo
  switch (V_VT(&src))
    {
    case VT_UI1:// Byte
      dest = (short)V_UI1(&src); break;
    case VT_I2: // Integer
      dest = (short)V_I2(&src);  break;
    case VT_I4: // Long
      dest = (short)V_I4(&src);  break;
    case VT_R4: // Single
      dest = (short)V_R4(&src);  break;
    case VT_R8: // Double
      dest = (short)V_R8(&src);  break;
    default:
      return false;
    }
  // ok
  return true;
}

