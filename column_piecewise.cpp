/**
*** DBO2X(R)1996-2001
*** rodablo@rodablo.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "column.hxx"

TPWColumn::TPWColumn(TColumnD& cd) 
  : TColumn(cd, false), m_len(0)
{
  m_IICursor.SetPWColumn(this);
}

TPWColumn::~TPWColumn() 
{
  m_IICursor.SetPWColumn(0);
}

const ub4 MAX_LONG_SIZE = 2147483647L; 

void 
TPWColumn::PreWork() 
{
  // define los arrays auxiliares, AQUI NO HAY DATA! 
  CHECK_OCI(::odefinps(m_IICursor.GetCDA(), 
		       0, 		/* opcode */
		       m_pos,
		       (ub1*)0, 	/* bufctx */
		       MAX_LONG_SIZE,
		       m_bindType, 	/* SQLT_LBI, SQLT_BIN, SQLT_LNG */
		       m_scale,
		       m_pIndp,
		       0, -1, -1,	/* fmt, fmtl, fmtt */
		       m_pRLen,
		       m_pRC,
		       0, 0, 0, 0));	/* buf, ind, len, rc _skip */
}    

/**
*** Column
***/
HRESULT __stdcall 
TPWColumn::get_Ref(VARIANT* retv)
{
  try {
    // 
    CHECK_HRESULT(VariantClear(retv));
    V_VT(retv) = (VARTYPE)(VT_NULL);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TPWColumn::get_Copy(VARIANT* retv)
{
  try {
    CHECK_HRESULT(VariantClear(retv));    
    V_VT(retv) = (VARTYPE)(VT_NULL);
  }
  __AUTO_EXCEPT;
}

/**
*** IPWColumn 
***/
void 
TPWColumn::Prepare4NextPiece()
{
  // obtiene los datos de la chunk x venir
  void* dummy;
  CHECK_OCI(::ogetpi(m_IICursor.GetCDA(), 
		     &m_piece, (void**)&dummy, &m_iteration, &m_indexp));
  // declara el buffer
  m_len = sizeof(m_buffer);
  CHECK_OCI(::osetpi(m_IICursor.GetCDA(), m_piece, m_buffer, &m_len));
}

void 
TPWColumn::DeliverPiece(IDispatch* pSink)
{
//    VARIANT vtData;
//    VariantInit(&vtData);
//    V_VT(&vtData) = VT_NULL;
  // ATENCION VAN EN ORDEN INVERSO
  VARIANTARG	args[4];
  // FirstPiece
  VariantInit(&args[3]);
  V_VT(&args[3])   = VT_BOOL;
  V_BOOL(&args[3]) = (1 == m_piece) ? VARIANT_TRUE : VARIANT_FALSE;
  // Row
  VariantInit(&args[2]);
  V_VT(&args[2]) = VT_I4;
  V_I4(&args[2]) = m_iteration + 1;
  // Index 
  VariantInit(&args[1]);
  V_VT(&args[1]) = VT_I4;
  V_I4(&args[1]) = m_indexp;
  // Piece
  VariantInit(&args[0]);
  // 
  if (SQLT_LNG == m_bindType) {
    // calcula largo
    size_t size = MultiByteToWideChar(CP_ACP, 0, (char*)m_buffer, m_len, 0, 0);
    wchar_t*  _p = new wchar_t[size + 1]; 
    size = MultiByteToWideChar(CP_ACP, 0, (char*)m_buffer, m_len, _p, size);
    _p[size] = 0;
    //
    V_VT(&args[0])   = VT_BSTR;//VT_BYREF|VT_VARIANT;
    V_BSTR(&args[0]) = ::SysAllocString(_p);//&vtData;
    
    delete [] _p;
  } 
  else { // SQLT_LBI, SQL_BIN
    // crea un safe array  
    SAFEARRAYBOUND sab;
    sab.lLbound   = 0;
    sab.cElements = m_len;
    SAFEARRAY* pSA = SafeArrayCreate(VT_UI1, 1, &sab);
    void* pV;
    CHECK_HRESULT(SafeArrayAccessData(pSA, &pV));
    CopyMemory(pV, m_buffer, m_len);
    CHECK_HRESULT(SafeArrayUnaccessData(pSA));
    //
    V_VT(&args[0])    = VT_ARRAY|VT_UI1;
    V_ARRAY(&args[0]) = pSA;
  }
  //
  DISPPARAMS dp = {args, 0, 4, 0};
  CHECK_HRESULT(pSink->Invoke(DISPID_CURSOR_EVENT_FETCH_PIECE, 
			      IID_NULL, LOCALE_SYSTEM_DEFAULT,
			      DISPATCH_METHOD, &dp, 0, 0, 0));

  VariantClear(&args[0]);
}

//  /**
//  *** TLongRawPWColumn
//  ***/
//  TLongRawPWColumn::TLongRawPWColumn(TColumnD& cd)
//    : TPWColumn(cd), m_pSA(0), m_pvD(0)
//  {
//    // crea un safe array  
//    SAFEARRAYBOUND sab;
//    sab.lLbound   = 1;
//    sab.cElements = 3;
//    //  m_psaData     = SafeArrayCreate(VT_UI1, 1, &sab);
//    //if (0 == m_psaData) internal;
//  }

//  TLongRawPWColumn::~TLongRawPWColumn()
//  {
//    //  if (0 != m_psaData)
//    //    CHECK_HRESULT(SafeArrayDestroy(m_psaData));
//  }


//  //CHECK_HRESULT(SafeArrayAccessData(m_psaRows, &m_pV));
//  //CHECK_HRESULT(SafeArrayUnaccessData(m_psaRows));
