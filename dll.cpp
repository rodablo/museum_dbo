/**
*** DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include <initguid.h>
#include "dbo.hxx"

/*
probar de no uncluir el header de TString y ver si se incluye basic_string

 */

//basic_string<char, string_char_traits<char>, RWSTD_ALLOC_TYPE(char) > toto;

//#include "c:\mstools\include\aclapi.h"
/**
*** PROTOTIPOS
***/
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/**
***     GLOBALES
***/
HINSTANCE g_hInst = 0;
ULONG     g_cLock = 0;
ULONG     g_cObj  = 0;
char      g_szModuleName[] = STRINGIFY(__DLL_FNAME); // inicializar desde GetModuleName()

extern void __stdcall LicLoad();
extern void __stdcall LicFree();
/**
***     DLLENTRYPOINT
***/
/*extern "C"*/ BOOL WINAPI 
DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID /*lpReserved*/)
//BOOL WINAPI
//DllEntryPoint(HINSTANCE hInstance, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
  if (DLL_PROCESS_ATTACH == fdwReason)
    {
      // 
      extern void Throw(unsigned int code, _EXCEPTION_POINTERS* pEP);
      _set_se_translator(Throw);
      //	DebugBreak();  //asm { int 0x03 }
      //      LicLoad();
      // globlaliza
      g_hInst = hInstance;
      // bye DLL_THREAD_ATTACH and DLL_THREAD_DETACH
      DisableThreadLibraryCalls(hInstance);
#if 0
      // registra la ventana
      WNDCLASS wc;
      wc.style        = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
      wc.lpfnWndProc  = WndProc;
      wc.cbClsExtra   = 0;
      wc.cbWndExtra   = 4; // puntero al TSession
      wc.hInstance    = hInstance;
      wc.hIcon        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
      wc.hCursor      = LoadCursor(NULL, IDC_ARROW);
      wc.hbrBackground= (HBRUSH)(COLOR_WINDOW + 1);
      wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
      wc.lpszClassName= "DBOWndClass";
      //
      return RegisterClass(&wc);
#endif
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
      //
      //LicFree();
      //
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
  return ResultFromScode(((0L == g_cObj && 0L == g_cLock) /*|| a()*/) ? S_OK : S_FALSE);
}

#if 0
/**
***
***/
LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
    {
    case WM_PAINT:
      break;

    case WM_SIZE:
      break;

    case WM_COMMAND:
      {
        switch(GET_WM_COMMAND_ID(wParam,lParam))
          {
          case 101/*IDM_HIDE*/:
            break;

          case 103/*IDM_EXIT*/:
            //PostMessage(hwnd, WM_CLOSE, 0, 0L);
            break;
          }
      }
    break;
    /*
                case WM_CLOSE:
                // Hide the window to release the refcount added by CoLockObjectExternal
                // (See CLines::ShowWindow)
                //g_pDataServer->m_bUserClosing = TRUE;
                //g_pDataServer->ShowWindow(SW_HIDE);
                //DestroyWindow(hwnd);
                return 0L;

                case WM_DESTROY:
                //PostQuitMessage(0);
                break;
                */
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
    }
  return NULL;
}
#endif

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

