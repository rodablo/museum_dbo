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

/**
*** PROPIEDADES
***/
HRESULT __stdcall
TSession::put_Visible(VARIANT_BOOL Visible)
{
  try {
    m_fVisible = Visible;
    ShowWindow(Visible ? SW_SHOW : SW_HIDE);
   }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TSession::get_Visible(VARIANT_BOOL* retv)
{
  *retv = m_fVisible;
  return NOERROR;
}

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
TSession::CreateCursor(ICURSOR** ppCursor)
{
  try {
    // conecta si es necesaro
    if (!m_fIsLogged)
      DoLogon(false);
    // Crea un cursor
    IICursor* pC;
    ::CreateCursor(*this, pC);
    // engancha con el bloque
    AP<IICursor> apC(pC); 
    // Le pide la interfaz IIDCURSOR
    CHECK_HRESULT(pC->QueryInterface(IIDCURSOR, (void**)ppCursor));
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
    DoLogon(NonBlocking);
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
TSession::DoLogon(bool fNonBlocking)
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

void
TSession::ShowWindow(int nCmdShow)
{
  // Chequea qu so sea in llamado al cuete!
  if ((!m_fVisible && nCmdShow == SW_HIDE) || (m_fVisible && nCmdShow != SW_HIDE))
    return;

  m_fVisible = (nCmdShow == SW_HIDE) ? FALSE : TRUE;

  // Leer atentamente esto !!!
  //
  //
  // The Automation object shutdown behavior is as follows:
  // 1. If the object application is visible, it shuts down only in response to an
  // explicit user command (File/Exit) or it's programmatic equivalent (for example
  // the Quit method of the Application object).
  // 2. If the object application is not visible, it goes away when it's last
  // object is released.
  //
  // CoLockObjectExternal can be used to increment the ref count of the application object
  // when it is visible. This will implement shutdown behavior 1. When the application
  // goes invisible, CoLockObjectExternal is used to decrement the ref count. This will
  // implement shutdown behavior 2.

  if (m_fVisible)
    CoLockObjectExternal(this, TRUE /*fLock*/, TRUE/*ignored when fLock==TRUE*/);
  else
    CoLockObjectExternal(this, FALSE/*fLock*/, TRUE/*fLastLockReleases*/);

  ::ShowWindow (m_hWnd, nCmdShow);
}


