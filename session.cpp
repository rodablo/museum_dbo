/**
***     DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"

#include "icursor.hxx" 
#include "session.hxx"

/** Factory
*** (en el futuro sera de ISESSION)
*** Uso referencias para forzar a que existan y no se pueda pasar NULL
***/
void
CreateSession(IISession*& rpSession)
{
  // pesimismo inicial
  rpSession = new TSession();
  // unomas y retorna
  g_cObj++;
}

/**
***
***/
TSession::TSession()
{
  m_hWnd = 0;
  m_fVisible = 0;
  m_cCursorRef = 0;
  //Oracle
  m_fIsLogged = false;
  
  ZeroMemory(&m_lda, sizeof(m_lda));
  ZeroMemory(&m_hda, sizeof(m_hda));
}

TSession::~TSession()
{
  //  ::Beep(440,200);

  if (m_fIsLogged)
    ologof(&m_lda);    //   Logoff();
  //
  //   if (!m_bUserClosing && IsWindow(m_hWnd))
  //     ::DestroyWindow(m_hWnd);

  m_sUser.resize(0);
  m_sPassword.resize(0);
  m_sAlias.resize(0);

  // uno menos
  g_cObj--;
}

STDMETHODIMP
TSession::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
  if (0 == ppvObj)
    return E_INVALIDARG;
  //
  if (IsEqualIID(riid, IID_IConnectionPointContainer)) {
    *ppvObj = 0;
    //((IUnknown*)*ppvObj)->AddRef(); 
    return NOERROR;
  }
  else 
    return TIDISPATCH<IISession, &IID__Session>::QueryInterface(riid, ppvObj);
}

/**
*** PROPIEDADES
***/
// HRESULT __stdcall
// TSession::put_Visible(VARIANT_BOOL Visible)
// {
//   try {
//     m_fVisible = Visible;
//     ShowWindow(Visible ? SW_SHOW : SW_HIDE);
//    }
//   __AUTO_EXCEPT;
// }

// HRESULT __stdcall
// TSession::get_Visible(VARIANT_BOOL* retv)
// {
//   *retv = m_fVisible;
//   return NOERROR;
// }

HRESULT __stdcall
TSession::put_User(BSTR User)
{
  try {
    //
    if (m_fIsLogged)
      RAISE_INTERNAL(DBO_E_RUNTIME_PROP_TEMP_READONLY);

    m_sUser = ANSI(User);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::get_User(BSTR* retv)
{
  try {
    *retv = WIDE(m_sUser).SysAllocString();
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::put_Password(BSTR Password)
{
  try {
    //
    if (m_fIsLogged)
      RAISE_INTERNAL(DBO_E_RUNTIME_PROP_TEMP_READONLY);

    //SysFreeString(Password);
    m_sPassword = ANSI(Password);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::get_Password(BSTR* retv)
{
  try {
    *retv = WIDE(m_sPassword).SysAllocString();
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::put_Alias(BSTR Alias)
{
  try {
    //
    if (m_fIsLogged)
      RAISE_INTERNAL(DBO_E_RUNTIME_PROP_TEMP_READONLY);
    //
    m_sAlias = ANSI(Alias);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::get_Alias(BSTR* retv)
{
  try {
    *retv = WIDE(m_sAlias).SysAllocString();
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::get_IsLogged(VARIANT_BOOL* retv)
{
  //
  *retv = m_fIsLogged ? VARIANT_TRUE : VARIANT_FALSE;
  //
  return NOERROR;
}

HRESULT __stdcall
TSession::put_AutoCommit(VARIANT_BOOL AutoCommit)
{  
  try {
    if (AutoCommit)
      CHECK_OCI(ocon(&m_lda));
    else
      CHECK_OCI(ocof(&m_lda));
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::put_NonBlockingMode(VARIANT_BOOL NonBlocking)
{
  try {
    if (NonBlocking)
      CHECK_OCI(onbset(&m_lda));
    else
      CHECK_OCI(onbclr(&m_lda));
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::get_NonBlockingMode(VARIANT_BOOL* retv)
{
  try {
    // ejecuta
    sword stat = onbtst(&m_lda);
    // evalua o genera error
    if (0 == stat)
      *retv = VARIANT_TRUE;
    else if (3128 == m_lda.rc)
      *retv = VARIANT_FALSE;
    else
      RAISE_OCI(stat);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::get_RC(short* retv)
{
  try {
    *retv = m_lda.rc;
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TSession::get_CursorCount(long* retv)
{
  *retv = m_cCursorRef;
  return NOERROR;
}

/**
*** METODOS 
***/
HRESULT __stdcall
TSession::CreateCursor(_Cursor** ppCursor)
{
  try {
    // conecta si es necesaro
    if (!m_fIsLogged)
      _Logon(false);
    // Crea un cursor
    IICursor* pC;
    ::CreateCursor(*this, pC);
    // engancha con el bloque
    AP<IICursor> apC(pC); 
    // Le pide la interfaz IIDCURSOR
    CHECK_HRESULT(pC->QueryInterface(IID__Cursor, (void**)ppCursor));
    // libera
    *ppCursor = apC.release();
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::Logon(VARIANT_BOOL NonBlocking)
{
  try {
    // previamente conectado
    // CONDITION(DS_E_PRECON, CONECTADO == m_estado);
    _Logon(NonBlocking);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::Logoff()
{
  try {
    // previamente desconectado
    //CONDITION(DS_E_PRECON, NOCONECTADO == m_estado);

    if (0 != m_cCursorRef)
      RAISE_INTERNAL(DBO_E_RUNTIME_SESSION_ACTIVE_CURSOR_EXIST_LU, m_cCursorRef);

    CHECK_OCI(ologof(&m_lda));

    m_fIsLogged = false;

    //ZeroMemory(&m_lda, sizeof(m_lda));
    //ZeroMemory(&m_hda, sizeof(m_hda));

  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::Commit()
{
  try {
    CHECK_OCI(ocom(&m_lda));
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::Rollback()
{
  try {
    CHECK_OCI(orol(&m_lda));
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::Break()
{
  try {
    CHECK_OCI(obreak(&m_lda));
  }
  __AUTO_EXCEPT;
}

/**
*** IMPLEMENTACION
***/
void 
TSession::_Logon(bool fNonBlocking)
{
  //limpieza
  //  ZeroMemory(&m_lda, sizeof(m_lda));
  //   ZeroMemory(&m_hda, sizeof(m_hda));
  // Here logea
  CHECK_OCI(olog(&m_lda, m_hda, 
		 (text*)m_sUser.c_str(),  -1,
		 (text*)m_sPassword.c_str(), -1,
		 (text*)m_sAlias.c_str(), -1, (fNonBlocking) ? OCI_LM_NBL : OCI_LM_DEF));
  // Notifica
  m_fIsLogged = true;
}

/**
***
***/
typedef sb4 TOci8Ex;
inline void CHECK_OCI8(sword stat) { if (OCI_SUCCESS != stat) throw TOci8Ex(stat); }

HRESULT __stdcall
TSession::PasswordChange(BSTR NewPassword)
{
   HRESULT retv = NOERROR;

   OCIEnv*    	envhp = 0;
   OCIError*  	errhp = 0;
   OCIServer* 	srvhp = 0;
   OCISvcCtx*	svchp = 0;
   OCISession*	usrhp = 0;

  try {  
    string newPassword = ANSI(NewPassword);
 
    //
    CHECK_OCI8(OCIInitialize( (ub4)/*OCI_DEFAULT*/OCI_OBJECT, (dvoid *)0,
			      (dvoid * (*)(dvoid *, size_t)) 0,
			      (dvoid * (*)(dvoid *, dvoid *, size_t))0,
			      (void (*)(dvoid *, dvoid *)) 0 ) );

    // Allocate and initialise the environment handle - no need to call
    // call OCIHandleAlloc first.
    CHECK_OCI8(OCIEnvInit( (OCIEnv **) &envhp, OCI_DEFAULT, (size_t) 0, 
			   (dvoid **) 0 ) );

    // Allocate the error handle for all error handling.
    CHECK_OCI8(OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &errhp, OCI_HTYPE_ERROR, 
			       (size_t) 0, (dvoid **) 0) );

    // Allocate the server handle and create the access path by 
    // attaching.  Note no connect string alias is being passed in the
    // attach call as we are using a local connection.
    CHECK_OCI8(OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &srvhp, OCI_HTYPE_SERVER,
    			       (size_t) 0, (dvoid **) 0) );
    //
    CHECK_OCI8(OCIServerAttach(srvhp, errhp, 
    			       (text *)m_sAlias.c_str(), m_sAlias.length(), 0) );
   
    // Allocate the service context handle and set the server attribute to
    // the server handle allocated above.
    CHECK_OCI8(OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &svchp, OCI_HTYPE_SVCCTX,
			       (size_t) 0, (dvoid **) 0) );

    // set attribute server context in the service context 
    CHECK_OCI8(OCIAttrSet( (dvoid *) svchp, OCI_HTYPE_SVCCTX, 
			   (dvoid *) srvhp, (ub4) 0, OCI_ATTR_SERVER, 
			   errhp) );


    // Allocate the session handle and add it to the session attribute of 
    // the service context.  Note, no username and password are added to 
    // the session handle since this will be done as part of the 
    // OCIChangePassword call.
    CHECK_OCI8(OCIHandleAlloc((dvoid *) envhp, (dvoid **)&usrhp, 
			      (ub4) OCI_HTYPE_SESSION, 
			      (size_t) 0, (dvoid **) 0) );

    CHECK_OCI8(OCIAttrSet((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX,
  			  (dvoid *) usrhp, (ub4) 0,
  			  (ub4) OCI_ATTR_SESSION, errhp));
    //
    CHECK_OCI8(OCIPasswordChange(svchp, errhp,
				 (CONST text*)m_sUser.c_str(),        
				 m_sUser.length(),
				 (CONST text*)m_sPassword.c_str(), 
				 m_sPassword.length(),
				 (CONST text*)newPassword.c_str(), 
				 newPassword.length(),
				 OCI_DEFAULT | OCI_AUTH) );
    // si llego aqui con exito actualiza el attributo para reintentar el logon
    m_sPassword = newPassword;
    //
    CHECK_OCI8(OCISessionEnd(svchp, errhp, usrhp, OCI_DEFAULT));
    //
    CHECK_OCI8(OCIServerDetach(srvhp, errhp, OCI_DEFAULT));
  }
  catch(TOci8Ex stat) {	
    text errbuf[512];
    text*  errmsg = 0; 
    sb4 errcode = 0;
    //
    switch (stat)
      {
      case OCI_SUCCESS:
	break;
      case OCI_SUCCESS_WITH_INFO:
	errmsg = (text*)"Error - OCI_SUCCESS_WITH_INFO";
	break;
      case OCI_NEED_DATA:
	errmsg = (text*)"Error - OCI_NEED_DATA";
	break;
      case OCI_NO_DATA:
	errmsg = (text*)"Error - OCI_NODATA";
	break;
      case OCI_ERROR:
	(void) OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode,
			   errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
	errmsg = (text*)errbuf;
	break;
      case OCI_INVALID_HANDLE:
	errmsg = (text*)"Error - OCI_INVALID_HANDLE";
	break;
      case OCI_STILL_EXECUTING:
	errmsg = (text*)"Error - OCI_STILL_EXECUTE";
	break;
      case OCI_CONTINUE:
	errmsg = (text*)"Error - OCI_CONTINUE";
	break;
      default:
	errmsg = (text*)"Unknow OCI Error";
	break;
      }  
    // retorna un error
    retv = DBO_E_ORACLE;
    // crea una excepcionble 
    m_fException = true; // solo para no vtable controllers
    EXCEPINFO excepInfo;
    m_excepInfo.bstrDescription = WIDE((char*)errmsg).SysAllocString();
    m_excepInfo.bstrSource = WIDE("DBO22").SysAllocString();
    m_excepInfo.scode = GetScode(retv);
    //
    ICreateErrorInfo* pCErrInfo;
    IErrorInfo*       pErrInfo;
    if (SUCCEEDED(CreateErrorInfo(&pCErrInfo)))
      {
	pCErrInfo->SetGUID( IID__Session);
	pCErrInfo->SetSource(m_excepInfo.bstrSource);
	pCErrInfo->SetDescription(m_excepInfo.bstrDescription);
	//
	if (SUCCEEDED(pCErrInfo->QueryInterface(IID_IErrorInfo, (LPVOID FAR*)&pErrInfo)))
	  {
	    SetErrorInfo(0, pErrInfo);
	    pErrInfo->Release();
	  }
	pCErrInfo->Release();
      }
  }	                

  if (0 != usrhp) OCIHandleFree((dvoid *)usrhp, OCI_HTYPE_SESSION);
  if (0 != svchp) OCIHandleFree((dvoid *)svchp, OCI_HTYPE_SVCCTX);
  if (0 != srvhp) OCIHandleFree((dvoid *)srvhp, OCI_HTYPE_SERVER);
  if (0 != errhp) OCIHandleFree((dvoid *)errhp, OCI_HTYPE_ERROR);

  //  return DBO_E_RUNTIME_PROP_TEMP_READONLY;//NOERROR;
  return retv;
}
