/**
*** DBO2(1997/1998)
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "column.hxx"

TColumnD::TColumnD(IICursor& iicursor, sword pos)
  :  m_IICursor(iicursor), 
     m_pos(pos) 
{
}

TColumnD::TColumnD(TColumnD& other)
  :  m_IICursor(other.m_IICursor)
{
  // copia el string
  m_sName    = other.m_sName;
  m_pos      = other.m_pos;
  m_IICursor = other.m_IICursor;
  
  m_dbsize   = other.m_dbsize;     
  m_dbtype   = other.m_dbtype;          
  m_dsize    = other.m_dsize;           
  m_prec     = other.m_prec;            
  m_scale    = other.m_scale;           
  m_nullok   = other.m_nullok ;          
  //
  m_bindType = other.m_bindType;        
  m_bindSize = other.m_bindSize;        
  //
  m_vtRowType= other.m_vtRowType;    // el resto como RAM
  //CopyMemory(&m_pos, &other.m_pos, sizeof(TColumnD)-offsetof(TColumnD, m_pos));  
}

/**
*** CreateCol
*** ICol AbstractFactory
***/
bool 
CreateColumn(IICursor& cursor, sword pos, IIColumn*& pObj)
{
  TColumnD cd(cursor, pos);
 
  sb1 nom[MAXCOLNAM+1];   // nombre de la columna
  sb4 noml = sizeof(nom); // length del nom OBLIGATORIO!!!!!

  // pesimismo inicial
  pObj = 0;
  
  // describe la col (atencion que ::odescr interactua con los binds)
  ::odescr(cd.m_IICursor.GetCDA(), cd.m_pos,
	   &cd.m_dbsize, &cd.m_dbtype,
	   nom, &noml, &cd.m_dsize, &cd.m_prec, &cd.m_scale, &cd.m_nullok);

  // verifica si era PL/SQL
  if (34 == cd.m_IICursor.GetCDA()->ft)
    RAISE_INTERNAL(DBO_E_RUNTIME_FETCH_O_DEFINE_IN_PL);

  // ¿Existe una columna en esta pos?
  if (1007 == cd.m_IICursor.GetCDA()->rc)
    return false;

  // otros errores
  else if  (0 != cd.m_IICursor.GetCDA()->rc)
    RAISE_OCI((sword) cd.m_IICursor.GetCDA()->rc);

  // termina en 0 el nombre e inicializa el string
  nom[noml] = 0; 
  cd.m_sName = string((char*)nom);

  // pre-determina que tipo de columna usar (luego  Mapeo ORANET -> Variant)
  cd.m_bindType = cd.m_dbtype;

  // determina el tipo
  switch (cd.m_dbtype)
    {
    case SQLT_INT: // (ORANET TYPE) integer 
      cd.m_bindSize  = sizeof(long);
      cd.m_vtRowType = VT_I4;
      pObj = new TColumn(cd);
      break;
      
    case SQLT_FLT: // (ORANET TYPE) Floating point number 
    case SQLT_NUM: // (ORANET TYPE) oracle numeric 
      cd.m_bindType  = SQLT_FLT; //4
      cd.m_bindSize  = sizeof(double);
      cd.m_vtRowType = VT_R8;
      pObj = new TColumn(cd);
      break;

    case SQLT_LNG:
      cd.m_bindType  = SQLT_STR;
      cd.m_bindSize  = 50000L;//cd.m_dbsize + 1; //??????????????????????????????????????
      cd.m_vtRowType = VT_BSTR;
      pObj = new TBSTRColumn(cd);     
      break;

    case SQLT_RID:                                               /* rowid */
      cd.m_bindType  = SQLT_STR;
      cd.m_bindSize  = 19L;//cd.m_dbsize + 1; //??????????????????????????????????????
      cd.m_vtRowType = VT_BSTR;
      pObj = new TBSTRColumn(cd);     
      break;

#pragma message ¿A que se refiere con ANSI?????
    case SQLT_AFC: // Ansi fixed char 
    case SQLT_AVC: // Ansi Var char 
    case SQLT_CHR: // (ORANET TYPE) character string
      cd.m_bindType  = SQLT_STR;
      cd.m_bindSize  = cd.m_dbsize + 1; //??????????????????????????????????????
      cd.m_vtRowType = VT_BSTR;
      pObj = new TBSTRColumn(cd);     
      break;

    case SQLT_DAT: // date in oracle format 
      cd.m_bindSize = sizeof(ODATE); // creo que esto no es necesario (lo infiere de m_bindType) 
                                        // de echo povoca un GPF 
      cd.m_vtRowType = VT_DATE;
      pObj = new TDATEColumn(cd);     
      break;

    case SQLT_STR:                              /* zero terminated string */
    case SQLT_VNU:                      /* NUM with preceding length byte */
    case SQLT_PDN:                /* (ORANET TYPE) Packed Decimal Numeric */
  //case SQLT_LNG:                                                /* long */
    case SQLT_VCS:                           /* Variable character string */  //como rebindeo???
    case SQLT_NON:                     /* Null/empty PCC Descriptor entry */
    case SQLT_VBI:                                /* binary in VCS format */
    case SQLT_BIN:                                 /* binary data(DTYBIN) */
    case SQLT_LBI:                                         /* long binary */
    case SQLT_UIN:                                    /* unsigned integer */
    case SQLT_SLS:                       /* Display sign leading separate */
    case SQLT_LVC:                                 /* Longer longs (char) */
    case SQLT_LVB:                                  /* Longer long binary */
    case SQLT_CUR:                                        /* cursor  type */
    case SQLT_LAB:                                          /* label type */
    case SQLT_OSL:                                        /* oslabel type */
      cd.m_bindType  = SQLT_STR;
      cd.m_bindSize  = cd.m_dbsize + 1;
      cd.m_vtRowType = VT_BSTR;
      pObj = new TBSTRColumn(cd);     
      break;
    }
  //
  return true;  
}

/**
***
***
*** TCOLUMN
***/
TColumn::TColumn(TColumnD& cd)
  :  TColumnD(cd)
{
  // crea el safeArray
  SAFEARRAYBOUND sab;
  sab.lLbound   = 1;
  sab.cElements = m_IICursor.GetRowsXFetch();
  
  m_psaRows = SafeArrayCreate(cd.m_vtRowType, 1, &sab);
  //INTERNAL(E_OUTOFMEMORY, 0 == m_psaVector);

  // crea los arrays auxiliares
  m_pIndp  = new sb2[sab.cElements];
  m_pRLen  = new ub2[sab.cElements];
  m_pRC    = new ub2[sab.cElements];

  // inicializa los arrays
  // ZeroMemory(...);

  // esto no se si va here
  //  m_IICursor.AddRef();
}

TColumn::~TColumn()
{
  // destruccion del array (y la data contenida)
  SafeArrayDestroy(m_psaRows);
  // destruye los arrays temporales
  delete [] m_pIndp; 
  delete [] m_pRLen;
  delete [] m_pRC;
  // libera la IF del cursor
  //  m_IICursor.Release();
}

void  
TColumn::PreWork()
{
  // Obtiene un puntero a la data del vector en crudo! y lo mantiene lockeado
  void* pV;
  CHECK_HRESULT(SafeArrayAccessData(m_psaRows, &pV));
  // define finalmente
  CHECK_OCI(::odefin(m_IICursor.GetCDA(), 
		     m_pos,
		     (ub1*)pV, 
		     m_bindSize,
		     m_bindType,
		     m_scale,
		     m_pIndp,
		     0, -1, -1,
		     m_pRLen,
		     m_pRC));
}

void
TColumn::PosWork()
{
  // invalida el puntero del bind y deslockea el vector 
  CHECK_HRESULT(SafeArrayUnaccessData(m_psaRows));
}

void  
TColumn::PosWorkPreRAISE()
{
  // invalida el puntero del bind y deslockea el vector 
  CHECK_HRESULT(SafeArrayUnaccessData(m_psaRows));
}

bool 
TColumn::ValidateIndex(long)
{
  return true;
}

/**
*** Column PROPERTIES
***/
HRESULT __stdcall
TColumn::get_Name(BSTR* retv)
{
  try {
    //
    *retv = WIDE(m_sName).SysAllocString(); 
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TColumn::get_Position(short* retv)
{
  try {
    // retorna la posicion
    *retv = (short)m_pos;
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TColumn::get_DisplaySize(long* retv)
{
  //
  *retv = m_dsize;
  //
  return NOERROR;
}

HRESULT __stdcall 
TColumn::get_Precision(short* retv)
{
  //
  *retv = m_prec;
  //
  return NOERROR;
}

HRESULT __stdcall
TColumn::get_Scale(short* retv)
{
  //
  *retv = m_scale;
  //
  return NOERROR;
}

HRESULT __stdcall 
TColumn::get_IsNullOk(VARIANT_BOOL* retv)
{
  //
  *retv = 0 != m_nullok ? VARIANT_TRUE : VARIANT_FALSE;
  //
  return NOERROR;
}

HRESULT __stdcall 
TColumn::get_RowCount(long* retv)
{
  return m_IICursor.get_RowCount(retv);
}

HRESULT __stdcall
TColumn::get_IsNull(long Index, VARIANT_BOOL* retv)
{
  try {
    // valida...
    if (ValidateIndex(Index))
      //*retv = (-1 != m_pIndp[Index - 1]) ? VARIANT_TRUE : VARIANT_FALSE;
      *retv = (0 > m_pIndp[Index - 1]) ? VARIANT_TRUE : VARIANT_FALSE;
    else
      RAISE_INTERNAL(DBO_E_RUNTIME_ROW_INDEX_OUT_OF_RANGE_L, Index);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TColumn::get_Length(long Index, short* retv)
{
  try {
    // valida...
    if (ValidateIndex(Index))
      *retv = m_pRLen[Index - 1];
    else
      RAISE_INTERNAL(DBO_E_RUNTIME_ROW_INDEX_OUT_OF_RANGE_L, Index);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TColumn::get_RC(long Index, short* retv)
{
  try {
    // valida...
    if (ValidateIndex(Index))
      *retv = m_pRC[Index - 1];
    else
      RAISE_INTERNAL(DBO_E_RUNTIME_ROW_INDEX_OUT_OF_RANGE_L, Index);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TColumn::get_Ref(VARIANT* retv)
{
  try {
    //verifica contenido valido
    // 
    VariantClear(retv);

    V_VT(retv) = (VARTYPE)(VT_ARRAY|VT_BYREF|m_vtRowType);
    V_ARRAYREF(retv) = &m_psaRows;
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TColumn::get_Copy(VARIANT* retv)
{
  try {
    VariantClear(retv);    

    SAFEARRAY* psaCopy;

    CHECK_HRESULT(SafeArrayCopy(m_psaRows, &psaCopy));

    V_VT(retv) = (VARTYPE)(VT_ARRAY|m_vtRowType);
    V_ARRAY(retv) = psaCopy;
  }
  __AUTO_EXCEPT;
}















