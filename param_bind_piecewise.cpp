/**
*** DBO2X(R)1996-2001
*** rodablo@rodablo.com
***/

/**
*** TPWBind
***/
class TPWBind : public TSBind , public IPWParam
{
public:
  // IIParam
  void put_Value(VARIANT& Index, VARIANT& Value);
  void get_Value(VARIANT& Index, VARIANT* retv);  
  void get_NVL(VARIANT& Index, VARIANT& NullVaLue, VARIANT* retv);
  // TBind
  // IPWParam
  // TPWBind
  TPWBind(TGParam& param, sword sqlt);
  virtual ~TPWBind();
  //
  void CallEvent(IDispatch* pSink);
  //
protected:
  enum {EMPTY, PIECE_READY, LAST_PIECE_READY} m_estado;
  VARIANT m_variantPiece;
  ub4     m_ub4PieceDataLength;
  char*   m_pvPieceData;
  //
  ub1	       m_ub1Piece;
  VARIANT_BOOL m_vbLastPiece;
};

const ub4 MAX_LONG_SIZE = 2147483647L;

TPWBind::TPWBind(TGParam& param, sword sqlt)
  :  TSBind(param), m_estado(EMPTY), m_pvPieceData(0) 
{
  // ve tema de no nulo!!!!!!! si se deja empty el 
  _indp = 0; 
  //
  VariantInit(&m_variantPiece);
  // vincula
  CHECK_OCI(::obindps(_Param, 0, (text*)_Param.Name().c_str(), -1,
		      (ub1*)0, MAX_LONG_SIZE, sqlt, 
		      0, &_indp, (ub2*)0, &_RC,	// scale, len, rc
		      0, 0, 0, 0, 		// skips
		      0, 0,			// maxsiz, cursiz;
		      (text*)0, -1, -1));	// fmt
  // declara el parametro al cursor
  _Param.GetIICursor().SetPWParam(this);
}

TPWBind::~TPWBind()
{
  VariantClear(&m_variantPiece);
}

// IParam

void 
TPWBind::put_Value(VARIANT& Index, VARIANT& Value)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_UNSPECTED_PROPERTY, "Value (put)");
}

void 
TPWBind::get_Value(VARIANT& Index, VARIANT* retv)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_UNSPECTED_PROPERTY, "Value (get)");
}

void 
TPWBind::get_NVL(VARIANT& Index, VARIANT& NullVaLue, VARIANT* retv)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_UNSPECTED_PROPERTY, "NVL");
}

void
TPWBind::CallEvent(IDispatch* pSink)
{
  // obtiene los datos del bloque requerido
  ub4 iteration, indexp;
  void* dummy;
  CHECK_OCI(::ogetpi(_Param, 
		     &m_ub1Piece, (void**)&dummy, &iteration, &indexp));
  // prepara los argumentos
  VARIANTARG  	args[4];
  // FirstPiece
  m_vbLastPiece = VARIANT_FALSE;
  VariantInit(&args[3]);
  V_VT(&args[3]) = VT_BOOL|VT_BYREF;
  V_BOOLREF(&args[3]) = &m_vbLastPiece;
  // Row
  VariantInit(&args[2]);
  V_VT(&args[2]) = VT_I4;
  V_I4(&args[2]) = iteration + 1;
  // Index 
  VariantInit(&args[1]);
  V_VT(&args[1]) = VT_I4;
  V_I4(&args[1]) = indexp + 1;
  // Piece
  VariantInit(&m_variantPiece);
  VariantInit(&args[0]);
  V_VT(&args[0]) = VT_VARIANT|VT_BYREF;
  V_VARIANTREF(&args[0]) = &m_variantPiece;
  // invoca al evento para req los datos
  DISPPARAMS dp = {args, 0, 4, 0};
  CHECK_HRESULT(pSink->Invoke(DISPID_CURSOR_EVENT_INSERT_PIECE, 
			      IID_NULL, LOCALE_SYSTEM_DEFAULT,
			      DISPATCH_METHOD, &dp, 0, 0, 0));
  m_estado = PIECE_READY;
}


/**
*** TPWBinaryBind
***/
class TPWBinaryBind : public TPWBind {
public:
  // IPWParam
  void Start();
  void RequestPiece(IDispatch* pSink);
  void Finalize();
  //
  TPWBinaryBind(TGParam& param) 
    :  TPWBind(param, SQLT_LBI) {
  }
  ~TPWBinaryBind() {
  }
protected:
};
  
void 
TPWBinaryBind::Start()
{
}

void 
TPWBinaryBind::RequestPiece(IDispatch* pSink)
{
  // liera cualquier recurso de invocaciones previas
  if (TPWBind::EMPTY != m_estado) {
      SafeArrayUnaccessData(V_ARRAY(&m_variantPiece));
  }
  // invoca al evento
  CallEvent(pSink);
  //
  void* pV;  
  ub4   length;
  // determina el tipo variant de los datos
  if ((VT_UI1|VT_ARRAY) == m_variantPiece.vt) {
    long LBound, UBound;
    // determina el numero de elemantos (bytes en este caso)
    CHECK_HRESULT(SafeArrayGetLBound(V_ARRAY(&m_variantPiece), 1, &LBound));
    CHECK_HRESULT(SafeArrayGetUBound(V_ARRAY(&m_variantPiece), 1, &UBound));
    m_ub4PieceDataLength = UBound - LBound;
    // lockea el array
    CHECK_HRESULT(SafeArrayAccessData(V_ARRAY(&m_variantPiece), &pV));
  } 
  else
    RAISE_INTERNAL(DBO_E_RUNTIME_PIECEWISE_INSERT_TYPE_MISMATCH_S, "Byte Array");

  //    
  //VariantClear(&args[3]);
  //VariantClear(&args[0]);
  //
  //
  CHECK_OCI(::osetpi(_Param, 
		     VARIANT_TRUE == m_vbLastPiece? OCI_LAST_PIECE : m_ub1Piece, 
		     pV, &m_ub4PieceDataLength));
}

void 
TPWBinaryBind::Finalize()
{
  SafeArrayUnaccessData(V_ARRAY(&m_variantPiece));
  VariantClear(&m_variantPiece);
}

/**
*** TPWLongBind
***/
class TPWLongBind : public TPWBind {
public:
  // IPWParam
  void Start();
  void RequestPiece(IDispatch* pSink);
  void Finalize();
  //
  TPWLongBind(TGParam& param) 
    :  TPWBind(param, SQLT_LNG) {
  }
  ~TPWLongBind() {
    Finalize();
  }
protected:
};

void
TPWLongBind::Start()
{
}

void 
TPWLongBind::RequestPiece(IDispatch* pSink)
{
  try {
    // invoca al evento
    CallEvent(pSink);  
    // verifica el tipo
    if (VT_BSTR == m_variantPiece.vt) {
      // determina el tamaño del string (sin el 0)
      m_ub4PieceDataLength = SysStringLen(V_BSTR(&m_variantPiece));
      // aloca el buffer
      m_pvPieceData = new char[m_ub4PieceDataLength]; 
      // convierte
      if (0 == (m_ub4PieceDataLength = 
		WideCharToMultiByte(CP_ACP, 0, 
				    V_BSTR(&m_variantPiece), m_ub4PieceDataLength, 
				    m_pvPieceData, m_ub4PieceDataLength, 0, 0)))
	RAISE_LASTERROR_();
      // libera el string
      VariantClear(&m_variantPiece);
      // declara el buffer para el exec
      CHECK_OCI(::osetpi(_Param, 
			 VARIANT_TRUE == m_vbLastPiece? OCI_LAST_PIECE : m_ub1Piece,
			 m_pvPieceData, &m_ub4PieceDataLength));
    }
    else
      RAISE_INTERNAL(DBO_E_RUNTIME_PIECEWISE_INSERT_TYPE_MISMATCH_S, "String");
  } 
  catch (...) {  
    Finalize();
    throw;
  }
}

void 
TPWLongBind::Finalize()
{	
  // libera el string
  VariantClear(&m_variantPiece);
  // libera el buffer temporario
  if (0 != m_pvPieceData) {
    delete [] m_pvPieceData;
    m_pvPieceData = 0;
  }
}
