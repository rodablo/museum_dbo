/**
***     DBO2(1997/1998)
*** rodablo@hotmail.com
    +infrared +keyboard +PC 
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "cursor.hxx"
#include "iparams.hxx"
#include "icolumns.hxx"

/** Factory
*** (en el futuro sera de ICursor)
*** Uso referencias para forzar a que existan y no se pueda pasar NULL
***/
void 
CreateCursor(IISession& isession, IICursor*& rpCursor)
{
  // pesimismo inicial
  rpCursor = 0;
  // se aloca...
  AP<TCursor> ap = new TCursor(isession);
  // lo abre
  ap->_pState->Open(ap.get());  
  //ap->DoOpen();
  // retorna
  rpCursor = ap.release();
}

/**
***
***/
TCursor::TCursor(IISession& isession)
  :  m_IISession(isession)
{
  // Oracle
  //m_estado = CERRADO;
  ZeroMemory(&m_cda, sizeof(Cda_Def));

  // inicialmente cerrado
  _pState = EClosed_TheInstance();

  m_fColsDefined = false;
  m_fIsDirty = false;
  _fStrict = false;

  // Prop data
  m_ulRowsXFetch = 1;

  _lastRpc = 0;

  // crea las instancias contenidas de las colecciones  (no obtiene la interfaz)
  CreateParams(*this, m_pParams);
  CreateColumns(*this, m_pColumns);

  m_IISession.CursorAddRef(); // !!!!!!!!!!!!!!!!!!!!!!!!!!
}

TCursor::~TCursor()
{
//   // deletea la instancia contenida
//   delete m_pColumns;

//   // muchos releases no deletes
  //  ::Beep(880,200);

  _pState->Close(this);

//   // vacia las columnas
  //  m_vParam.erase(m_vParam.begin(), m_vParam.end());
  //  m_vCols.erase(m_vCols.begin(), m_vCols.end());


  m_mName2Col.erase(m_mName2Col.begin(), m_mName2Col.end());
  m_vCols.erase(m_vCols.begin(), m_vCols.end());

  // param
  m_mapName2P.erase(m_mapName2P.begin(), m_mapName2P.end());
  m_mapNumber2P.erase(m_mapNumber2P.begin(), m_mapNumber2P.end());
  m_vParam.erase(m_vParam.begin(), m_vParam.end());

  // deletea la instancia contenida
  delete m_pParams;
  delete m_pColumns;

  //
  m_IISession.CursorRelease(); // !!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void 
TCursor::ChangeState(ECursor* pState)
{
  // cambia de estado
  _pState = pState;
}

/**
*** PROPIEDADES
***/
HRESULT __stdcall 
TCursor::get_EOF(VARIANT_BOOL* retv)
{
  try {
    // lo pasa al estado
    _pState->get_EOF(this, retv);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TCursor::get_RowCount(long* retv)
{
  *retv = GetCount();
  return NOERROR;
}

HRESULT __stdcall
TCursor::get_RPC(long* retv)
{
  *retv = m_cda.rpc;/*m_ulCountAcumulado*/
  return NOERROR;
}

HRESULT __stdcall
TCursor::get_RC(short* retv)
{
  *retv = m_cda.rc;
  return NOERROR;
}


HRESULT __stdcall
TCursor::put_RowsXFetch(long Max)
  //  Aqui tendria que forzar la realocacion y/o rebindeo o por el
  //  momento verificar un bindeo previo y emitir un error
{
  try {
    if (m_fColsDefined)
      RAISE_INTERNAL(DBO_E_RUNTIME_CANT_MODIFY_ROW_AFTER_DEFINE);
#if _PEND_
#pragma message verificar que Max > 0 && Max < 32000
#endif
    // si elestado noes anterior a fetch emitir error o cols defined = falso

    // 0. Verifica limites razonables, En realidad no habria que controlar
    // y en el momento de alocar el SafeArray se generaria una excepcion.
    // CONDITION(DS_E_SEQUENCE, Max < 0 || Max > 5000);
    // 1. ...
    m_ulRowsXFetch = Max;
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TCursor::get_RowsXFetch(long* retv)
{
  *retv = m_ulRowsXFetch;
  return NOERROR;
}

HRESULT __stdcall 
TCursor::get_IsDirty(VARIANT_BOOL* retv)
{
  *retv = m_fIsDirty;
  return NOERROR;
}

HRESULT __stdcall 
TCursor::put_Strict(VARIANT_BOOL Strict)
{  
  _fStrict = (VARIANT_TRUE == Strict);
  return NOERROR;
}

HRESULT __stdcall 
TCursor::get_Strict(VARIANT_BOOL* retv)
{  
  *retv = IsStrict() ? VARIANT_TRUE : VARIANT_FALSE;
  return NOERROR;
}

HRESULT __stdcall
TCursor::get_Params(Params** ppParams)
{
  try {
    // Le pide la interfaz 
    CHECK_HRESULT(m_pParams->QueryInterface(IID_Params, (void**)ppParams));
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TCursor::get_Cols(Columns** ppColumns)
{
  try {
    // Le pide la interfaz 
    CHECK_HRESULT(m_pColumns->QueryInterface(IID_Columns, (void**)ppColumns));
  }
  __AUTO_EXCEPT;
}

/**
*** METODOS 
***/
HRESULT __stdcall
TCursor::ParseSQL(BSTR Sql)
{
  try {
    // lo pasa al estado
    _pState->ParseSQL(this, string(ANSI(Sql)));
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TCursor::DefineColumns()
{
  try {
    // lo pasa al estado
    _pState->DefineColumns(this);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TCursor::DefineColumnAs(short Position, dboVarType AsType)
{
  try {
    //
    RAISE_INTERNAL(DBO_E_RUNTIME_ABRITTA_S, "DefineColumnAs");
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TCursor::Bind(VARIANT Wich, 
	      dboVarType AsType, VARIANT StringLength, Param** retv)
{
  try {
    // lo pasa al autómata
    _pState->Bind(this, Wich, AsType, StringLength, retv);
   }
   __AUTO_EXCEPT;
}

HRESULT __stdcall
TCursor::BindArray(BSTR Wich, 
		   short ArraySize, dboVarType AsType, VARIANT StringLength, Param** retv)
{
  try {
    // lo pasa al autómata
    _pState->BindArray(this, Wich, ArraySize, AsType, StringLength, retv);
   }
   __AUTO_EXCEPT;
}

HRESULT __stdcall 
TCursor::BindCursor(BSTR Wich, Cursor** retv)
{
  try {
    // lo pasa al autómata
    //_pState->BindCursor(this, Wich, retv);
    RAISE_INTERNAL(DBO_E_RUNTIME_ABRITTA_S, "BindCursor");
    if (0 != retv)
      *retv = 0;
  }
  __AUTO_EXCEPT;
}


HRESULT __stdcall
TCursor::Fetch(VARIANT_BOOL FAR* retv)
{
  try {
    // lo pasa al estado
    _pState->Fetch(this, retv);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TCursor::Execute(VARIANT N, VARIANT Offset)
{
  try {
    // lo pasa al estado
    _pState->Execute(this, N, Offset);
  }
  __AUTO_EXCEPT;
}




