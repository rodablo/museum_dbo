/**
*** DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
//#include "dispatch.hxx"

/**
*** LOADTYPEINFO
*** Ver de en el futuro mantener global la type info para evitar cargarla n veces
***/
LPTYPELIB g_pTLib = 0;

void
LoadTypeInfo(const IID& rIID, LPTYPEINFO& rpTI) // este argumento podria  volar
{
  // si es la primera vez carga... 
  if (0 == g_pTLib)
    {
      // pesimismo inicial
      rpTI = 0;
      // Carga el type Lib.
      if (FAILED(LoadRegTypeLib(__LIBID,
				__LIB_VER_MAJOR, __LIB_VER_MINOR, 
      				__LIB_LCID, &g_pTLib)) &&
	  // Si no esta registrada, intenta con el path, si OK queda registrada
	  FAILED(LoadTypeLib(WIDE(g_szModuleName), &g_pTLib))
	  ) 
	RAISE_INTERNAL(DBO_E_LOADTYPLIBFAIL);
    }

  // Obtiene la informacion de Tipo para la interface del objeto.
  HRESULT hr = g_pTLib->GetTypeInfoOfGuid(rIID, &rpTI);
  // si fallo
  if (FAILED(hr))
    RAISE_INTERNAL(DBO_E_LOADTYPLIBFAIL);
}

/**
***     RAISE_INTERNAL
***     Llama a FormatMessage localmente para aprovechar
***     los argumentos variables, pasa el string preformateado
***/
void
RAISE_INTERNAL(DWORD dwExceptionCode, ...)
{
  TCHAR*  pDesc;          // mensaje
  DWORD   dwRet;
  va_list ap;
  // inicializa el va_list
  va_start(ap, dwExceptionCode);
  // obtiene el mensaje
  dwRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_HMODULE,
                        g_hInst/*GetModuleHandle(g_szModuleName)*/, dwExceptionCode,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pDesc, 10, &ap);
  // libera la lista ¿hace falta?
  va_end(ap);
  // ¿problemas con el FormatMessage?
  if (0 == dwRet)
    { // crea un mensaje
      pDesc = 0;
    }
  // genera la excepcion y le pasa el ¿array? que es COPIADO por RaiseException
  RaiseException(dwExceptionCode, EXCEPTION_NONCONTINUABLE_EXCEPTION, 1, (DWORD*)&pDesc);
}


/**
***     este codigo deberia iterar sobre ->ExceptionRecord para
***     soportar excepciones anidadas
***/
HRESULT
IDispatchSEH(LPEXCEPTION_POINTERS& rpEP, 
	     EXCEPINFO&            rExcepInfo,
	     bool&                 rfException,
	     const IID&            rIID,
	     Lda_Def&              rLda,
	     ub2                   rc)
{
  HRESULT hrret/* = E_UNEXPECTED*/;                               // HRESULT de retorno
  // limpia e inicializa la ram
  ZeroMemory(&rExcepInfo, sizeof(EXCEPINFO));
  /// UTPC_E_INTERNAL
  if (DBO_E_RUNTIME_BEGIN < rpEP->ExceptionRecord->ExceptionCode &&
      DBO_E_RUNTIME_END   > rpEP->ExceptionRecord->ExceptionCode )
    {
      // recupera el mensaje
      TCHAR* pDesc = (TCHAR*)rpEP->ExceptionRecord->ExceptionInformation[0];
      // recupera el HRESULT especifico
      hrret = (HRESULT)rpEP->ExceptionRecord->ExceptionCode;
      // crea el string DESCRIPCION
      rExcepInfo.bstrDescription = WIDE(pDesc).SysAllocString();
      LocalFree(pDesc);
    }
  /// UTPC_E_LASTERROR
  else if (DBO_E_LASTERROR == rpEP->ExceptionRecord->ExceptionCode ||
	   DBO_E_HRESULT   == rpEP->ExceptionRecord->ExceptionCode )
    {
      DWORD dwErr;
      TCHAR* pDesc;
      // recupera el error
      dwErr = rpEP->ExceptionRecord->ExceptionInformation[0];
      // crea el hresult
      hrret = (DBO_E_LASTERROR == rpEP->ExceptionRecord->ExceptionCode) ? 
	HRESULT_FROM_WIN32(dwErr) : dwErr;
      // obtiene el mensaje
      /*dwRet =*/FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                               0, dwErr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               (LPTSTR)&pDesc, 0, 0);
      // crea el string DESCRIPCION
      rExcepInfo.bstrDescription = WIDE(pDesc).SysAllocString();
      LocalFree(pDesc);
    }
  /// 
  else if (DBO_E_ORACLE == rpEP->ExceptionRecord->ExceptionCode)
    {
      char* pszError = new char[2048];
      // genera el HRESULT
      hrret = DBO_E_ORACLE;
      // recupera el rc
      //sword rc = (sword)rpEP->ExceptionRecord->ExceptionInformation[0];
      // lee el mensage del oracle
      oerhms(&rLda, rc, (text*)pszError, 2048);
      // crea el string DESCRIPCION
      rExcepInfo.bstrDescription = WIDE(pszError).SysAllocString();
      // libera el temporal
      delete pszError;
    }
  else
    {
      //
      hrret = rpEP->ExceptionRecord->ExceptionCode;
      //
      switch(rpEP->ExceptionRecord->ExceptionCode)
        {
#define POROTO(m) \
          rExcepInfo.bstrDescription = WIDE(m).SysAllocString(); break;
        case EXCEPTION_ACCESS_VIOLATION:
          POROTO("DBO2 EXCEPTION_ACCESS_VIOLATION");
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
          POROTO("DBO2 EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
        case EXCEPTION_BREAKPOINT:
          POROTO("DBO2 EXCEPTION_BREAKPOINT");
        case EXCEPTION_DATATYPE_MISALIGNMENT:
          POROTO("DBO2 EXCEPTION_DATATYPE_MISALIGNMENT");
        case EXCEPTION_FLT_DENORMAL_OPERAND:
          POROTO("DBO2 EXCEPTION_FLT_DENORMAL_OPERAND");
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
          POROTO("DBO2 EXCEPTION_FLT_DIVIDE_BY_ZERO");
        case EXCEPTION_FLT_INEXACT_RESULT:
          POROTO("DBO2 EXCEPTION_FLT_INEXACT_RESULT");
        case EXCEPTION_FLT_INVALID_OPERATION:
          POROTO("DBO2 EXCEPTION_FLT_INVALID_OPERATION");
        case EXCEPTION_FLT_OVERFLOW:
          POROTO("DBO2 EXCEPTION_FLT_OVERFLOW");
        case EXCEPTION_FLT_STACK_CHECK:
          POROTO("DBO2 EXCEPTION_FLT_STACK_CHECK");
        case EXCEPTION_FLT_UNDERFLOW:
          POROTO("DBO2 EXCEPTION_FLT_UNDERFLOW");
        case EXCEPTION_ILLEGAL_INSTRUCTION:
          POROTO("DBO2 EXCEPTION_ILLEGAL_INSTRUCTION");
        case EXCEPTION_IN_PAGE_ERROR:
          POROTO("DBO2 EXCEPTION_IN_PAGE_ERROR");
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
          POROTO("DBO2 EXCEPTION_INT_DIVIDE_BY_ZERO");
        case EXCEPTION_INT_OVERFLOW:
          POROTO("DBO2 EXCEPTION_INT_OVERFLOW");
        case EXCEPTION_INVALID_DISPOSITION:
          POROTO("DBO2 EXCEPTION_INVALID_DISPOSITION");
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
          POROTO("DBO2 EXCEPTION_NONCONTINUABLE_EXCEPTION");
        case EXCEPTION_PRIV_INSTRUCTION:
          POROTO("DBO2 EXCEPTION_PRIV_INSTRUCTION");
        case EXCEPTION_SINGLE_STEP:
          POROTO("DBO2 EXCEPTION_SINGLE_STEP");
        case EXCEPTION_STACK_OVERFLOW:
          POROTO("DBO2 EXCEPTION_STACK_OVERFLOW");
        default:
          {
            TCHAR*  pDesc;
            // obtiene el mensaje
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
			  NULL,
			  hrret,
			  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			  (LPTSTR) &pDesc, 0, 0);
            // crea el string DESCRIPCION
            rExcepInfo.bstrDescription = WIDE(pDesc).SysAllocString();
            LocalFree(pDesc);
          }
        }
    }
  /// Aqui transfiero la informacion al automation
  // crea el string SOURCE
  rExcepInfo.bstrSource = WIDE("DBO22").SysAllocString();
  // otro retorno informal (revisar si el GetScode no strola nada)
  rExcepInfo.scode = GetScode(hrret);
  // notifica a los que acceden por Invoke
  rfException = true;
  // 5. Setea el objeto ErrInfo para los que acceden por vtable binding
  ICreateErrorInfo* pCErrInfo;
  IErrorInfo*       pErrInfo;
  //
  HRESULT hr = CreateErrorInfo(&pCErrInfo);
  if (SUCCEEDED(hr))
    {
      pCErrInfo->SetGUID(rIID);
      //
      if (rExcepInfo.bstrSource)
        pCErrInfo->SetSource(rExcepInfo.bstrSource);
      if (rExcepInfo.bstrDescription)
        pCErrInfo->SetDescription(rExcepInfo.bstrDescription);

      hr = pCErrInfo->QueryInterface(IID_IErrorInfo, (LPVOID FAR*)&pErrInfo);
      if (SUCCEEDED(hr))
        {
          SetErrorInfo(0, pErrInfo);
          pErrInfo->Release();
        }
      pCErrInfo->Release();
    }
  // Retorna el HRESULT que quiero que retorne el metodo automation
  return hrret;
}

/**
***
***
***/
void
Throw(unsigned int code, _EXCEPTION_POINTERS* pEP) 
{
  throw TException(pEP);
}


