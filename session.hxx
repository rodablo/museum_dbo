#if !defined(_SESSION_HXX_)
#define _SESSION_HXX_

#include "isession.hxx"

class TSession
  :  public TIDISPATCH<IISession, &IIDSESSION>
{
public:
  /// Session Propiedades
  STDMETHOD(get_Visible)(VARIANT_BOOL* retv);
  STDMETHOD(put_Visible)(VARIANT_BOOL Visible);
  STDMETHOD(get_User)(BSTR* retv);
  STDMETHOD(put_User)(BSTR User);
  STDMETHOD(get_Password)(BSTR* retv);
  STDMETHOD(put_Password)(BSTR Password);
  STDMETHOD(get_Alias)(BSTR* retv);
  STDMETHOD(put_Alias)(BSTR Alias);
  STDMETHOD(get_IsLogged)(VARIANT_BOOL* retv);
  STDMETHOD(put_AutoCommit)(VARIANT_BOOL AutoCommit);
  STDMETHOD(get_NonBlockingMode)(VARIANT_BOOL* retv);
  STDMETHOD(put_NonBlockingMode)(VARIANT_BOOL NonBlocking);
  STDMETHOD(get_RC)(short* retv);
  /// Session Metodos
  STDMETHOD(Logon)(VARIANT_BOOL NonBlocking);
  STDMETHOD(Logoff)();
  STDMETHOD(Commit)();
  STDMETHOD(Rollback)();
  STDMETHOD(Break)();
  STDMETHOD(CreateCursor)(ICURSOR** ppCursor);
  STDMETHOD(get_CursorCount)(long* retv);

  // IISession
  /// IUnknown
  ULONG __stdcall CursorAddRef() { ++m_cCursorRef; return AddRef(); }
  ULONG __stdcall CursorRelease(){ --m_cCursorRef; return Release(); }
  //operator Lda_Def*() {return &m_lda;}
  Lda_Def* GetLDA() {return &m_lda;}
  //
  TSession(); // hacer esto protected y el factory friend
  ~TSession();

protected:
  void     DoLogon(bool fNonBlocking);
  void     ShowWindow(int nCmdShow);
  HRESULT  IDispatchSEH();
  
protected:
  ULONG   m_cCursorRef;
  Lda_Def m_lda;               // oracle lda
  ub1     m_hda[HDA_SIZE];     // oracle hda
  bool    m_fVisible;
  string  m_sUser;
  string  m_sPassword;
  string  m_sAlias;
  bool    m_fIsLogged;
public:
  HWND    m_hWnd;              // ventana de diagnosticos
  bool    m_bUserClosing;      // indica que se esta cerrando
};

inline
HRESULT TSession::IDispatchSEH() {
  return ::IDispatchSEH(m_pEP, m_excepInfo, m_fException, m_rIID, m_lda, m_lda.rc);
}

#endif // _SESSION_HXX_




