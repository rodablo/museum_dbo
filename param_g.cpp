/**
*** DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "param.hxx"

/**
*** CreateParam
*** Param Abstract Factory
***/
void 
CreateParam(IICursor& cursor, VARIANT& Wich, AP<IIParam>& rapParam)
{
  // 
  rapParam = new TGParam(cursor, Wich);
}

void
CreateParam(IICursor& cursor, string& Wich, AP<IIParam>& rapParam)
{
  rapParam = new TGParam(cursor, Wich);
}

// bool
// ValidateWichParam(VARIANT& rWich, sword* pNumber)
// {
//     *pNumber = -1;
//     // determina el VT_
//     switch (V_VT(&rWich))
//       {
//       case VT_UI1:// Byte
// 	*pNumber = (sword)V_UI1(&rWich); break;
//       case VT_I2: // Integer
// 	*pNumber = (sword)V_I2(&rWich);  break;            
//       case VT_I4: // Long
// 	*pNumber = (sword)V_I4(&rWich);  break;      
//       case VT_BSTR:
// 	*pNumber = -1;
// 	return true;
//       default:
// 	RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_TYPE_MISMATCH_I, (long)V_VT(&rWich));
//       }
//     // es number
//     return false;
// }

/**
*** TGParam
***/
long
TGParam::m_lUniqueIDGenerator = 10000;

TGParam::TGParam(IICursor& rIICursor, VARIANT& Wich)
  :  m_IICursor(rIICursor)
{
  // determina tipo de parametro (Numerico o Alfa) por el VT_ del indice
  m_swNumber = -1;
  switch (V_VT(&Wich)) // (pasar esto al constructor de TScalarParam, unificando los constructores)
    {
    case VT_UI1:// Byte
      m_swNumber = (sword)V_UI1(&Wich); break;
    case VT_I2: // Integer
      m_swNumber = (sword)V_I2(&Wich);  break;            
    case VT_I4: // Long
      m_swNumber = (sword)V_I4(&Wich);  break;
    case VT_R4: // Single
      m_swNumber = (sword)V_R4(&Wich);  break;
    case VT_R8: // Double
      m_swNumber = (sword)V_R8(&Wich);  break;
    case VT_BSTR:
      m_sName    = ANSI(V_BSTR(&Wich));
      break;
    default:
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NAME_TYPE_MISMATCH_I, V_VT(&Wich));
    }
  // asigna numero unico
  m_lUniqueID = m_lUniqueIDGenerator++;
}

TGParam::TGParam(IICursor& rIICursor, string& Wich)
  :  m_IICursor(rIICursor)
{
  // es alfa
  m_swNumber = -1;
  // 
  m_sName = Wich;
  // asigna numero unico
  m_lUniqueID = m_lUniqueIDGenerator++;
}

TGParam::~TGParam()
{
  // algun tipo de bind (0) para avisarle al oci que ya no es valido UNBIND
}

/**
*** Param
***/
HRESULT __stdcall
TGParam::get_Name(BSTR* retv)
{
  try {
    //
    *retv = WIDE("pendiente"/*m_sName*/).SysAllocString(); 
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TGParam::get_Number(short* retv)
{
  try {
    // si no es numerico raise (IsNumeric)
    *retv = 0;
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TGParam::get_Length(VARIANT Index, short* retv)
{
  try {
    // 
    if (_apBind.is_empty())
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
    //
    _apBind->get_Length(Index, retv);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TGParam::get_RC(VARIANT Index, short* retv)
{
  try {
    // 
    if (_apBind.is_empty())
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
    //
    _apBind->get_RC(Index, retv);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall
TGParam::get_IsNull(VARIANT Index, VARIANT_BOOL* retv)
{
  try {
    // 
    if (_apBind.is_empty())
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
    //
    _apBind->get_IsNull(Index, retv);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TGParam::get_Value(VARIANT Index, VARIANT* retv)
{
  try {
    // 
    if (_apBind.is_empty())
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
    //
    _apBind->get_Value(Index, retv);
  }
  __AUTO_EXCEPT;
}

HRESULT __stdcall 
TGParam::put_Value(VARIANT Index, VARIANT Value)
{
  try {
    // 
    if (_apBind.is_empty())
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NOT_BINDED_S, "name");
    //
    _apBind->put_Value(Index, Value);
  }
  __AUTO_EXCEPT;
}  

/**
*** TGParam
***/
void 
TGParam::PreWork()
{
  //  if (!_apBind.is_empty())
  //     _apBind->PreWork();
  // not bind error
}

// void 
// TGParam::Bind()
// {
// }




/*
HRESULT __stdcall 
TGParam::BindArray(VARIANT Value, VARIANT ArraySize, dboVarType AsType, VARIANT StringLength,
		   Param** retv)
{
  try {
    // 1. determino el tipo a. desde astype  b. desde value c. error
    // 2. determino size, a. desde array size b. des value
    // 3. creolo
    // 4. inicializo
    // 0. deleteo bind previo 
    // 5. bindeo

    // -----------

    // DETERMINA EL TIPO
    VARTYPE vt;
    // usando AsType
    if (dboVError != AsType)
      vt = (VARTYPE)AsType;
    // lo deduce desde Value
    else if (VT_ERROR != V_VT(&Value)) // lo deduce del valor inicial
      vt = (VARTYPE)(VT_ARRAY & V_VT(&Value));
    // no puede determinar el tipo
    else
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_CANT_DETERMINE_VTYPE);

    // DETERMINA EL TAMAÑO DEL ARRAY
    short arraySize;
    // usando ArraySize
    if (VT_ERROR != V_VT(&ArraySize)) // lo deduce del valor inicial
      {
      if (!GetShortFromVariant(arraySize, ArraySize))
 	  RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_ARRAYSIZE_MISMATCH_I, V_VT(&ArraySize));
      }
    // lo deduce de Value
    else if (0 != (VT_ARRAY & V_VT(&Value)))
      {
	// obtiene array
	SAFEARRAY* pSA;
	if (0 != (VT_BYREF & V_VT(&Value)))
	  pSA = V_ARRAY(&Value);
	else
	  pSA = *V_ARRAYREF(&Value);

	// verifica que sea vector
	if (1 != SafeArrayGetDim(pSA))
	  ;//RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_BAD_NUMBER_OF_DIMS);
    
	// obtiene el size
	long LBound, UBound, lSize;
	CHECK_HRESULT(SafeArrayGetLBound(pSA, 1, &LBound));
	CHECK_HRESULT(SafeArrayGetUBound(pSA, 1, &UBound));
	lSize = (UBound - LBound);
	
	// if (0x7fff < lSize)
// 	  ;//muy grande verifivar tambien alguna regle del OCI respecto a este maximo

	arraySize = (short)lSize;
      }
    else
      // no puede determinar el tipo
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_CANT_DETERMINE_ARRAY_SIZE);  

    // CREO BIND EN FUNCION DE VT E INICIALIZO
    AP<TBind> apBind;
    switch (vt)
      {
      case VT_UI1:// Byte 
      case VT_I2: // Integer
      case VT_I4: // Long
      case VT_R4: // Float
      case VT_R8: // Double
	apBind = new TNumberArrayBind(this, vt, arraySize, Value);
	break;
      case VT_DATE:
	break;
      case VT_BSTR:
	break;
      default:
	RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_TYPE_MISMATCH_I, vt);
    }  

    // DELETEO PREVIO
    _apBind = 0;

    // BINDEO NUEVO Y ASIGNO
    apBind->Bind();
    m_apBind = apBind;   
  }
  __AUTO_EXCEPT;
}
*/

/**
*** TBind()
***/
TBind::TBind(TGParam& Param)
  :  _Param(Param)
{
}

#if 0
/**
*** TArrayBind
***/
TArrayBind::TArrayBind(TGParam* pParam, short arraySize)
  :  TBind(pParam)
{
  //
  m_nE = arraySize;
  m_pR = 0;
  m_bindType = 0;
  //  m_sizeOfElement = 0;
  // primero aloco
  AP<sb2>  apIndp(new sb2[m_nE]);  
  AP<ub2>  apLen(new ub2[m_nE]);  
  AP<ub2>  apRC(new ub2[m_nE]);   
  // no hubo problemas 
  m_pIndp = apIndp.release();
  m_pLen  = apLen.release();
  m_pRC   = apRC.release();
}

TArrayBind::~TArrayBind()
{
  delete [] m_pIndp;
  delete [] m_pLen;
  delete [] m_pRC;
  delete [] m_pR;
}

int
TArrayBind::ValidateIndex(VARIANT Index)
{
  // index es obligatorio
  if (VT_ERROR == V_VT(&Index))
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NEED_INDEX_IN_ARRAYS);
  // 
  short index;
  if (!GetShortFromVariant(index, Index))
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_TYPE_MISMATCH_I, V_VT(&Index));
  // validate elements
  if (0 == index || m_nE < index)
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_OUT_OF_RANGE_L,(long)index); 
  //
  return index;
}

void 
TArrayBind::get_Length(VARIANT Index, short* retv)
{
  // 
  *retv = m_pLen[ValidateIndex(Index) - 1]; 
}

void
TArrayBind::get_RC(VARIANT Index, short* retv)
{
  // 
  *retv = m_pRC[ValidateIndex(Index) - 1]; 
}

void
TArrayBind::get_IsNull(VARIANT Index, VARIANT_BOOL* retv)
{
  //
  *retv = 0 > m_pIndp[ValidateIndex(Index) - 1] ? VARIANT_TRUE : VARIANT_FALSE; 
}

void
TArrayBind::Bind()
{
  CHECK_OCI(obndra(m_pParam->m_IICursor.GetCDA(),
		   (text*)m_pParam->m_sName.c_str(), -1,
		   m_pR+sizeof(ub4)/*m_pR*/, *(ub4*)m_pR/*-1*/, m_bindType, -1,
		   m_pIndp, m_pLen, m_pRC,
		   m_maxSize, &m_curSize,
 		   (text*)0, -1, -1));
}

/**
*** TNumberArrayBind
***/
TNumberArrayBind::TNumberArrayBind(TGParam* pParam,
				   VARTYPE vt, short arraySize, VARIANT& Value)
  :  TArrayBind(pParam, arraySize)
{
  /* 1. determina el tamaño y tipo externo del elemento
     2. aloca el buffer (incluyendo el espacio para el tamaño)
     3. inicializo con value
  */
  // determina el tamaño y tipo externo del elemento
  m_sizeOfE = 0;
  switch (vt)
    {
    case VT_UI1:// Byte 
      m_sizeOfE = sizeof(unsigned char); 
      m_bindType = SQLT_INT;
      break;
    case VT_I2: // Integer
      m_sizeOfE = sizeof(short);
      m_bindType = SQLT_INT;
      break;
    case VT_I4: // Long     
      m_sizeOfE = sizeof(long);
      m_bindType = SQLT_INT;
      break;
    case VT_R4: // Float
      m_sizeOfE = sizeof(float);
      m_bindType = SQLT_FLT;
      break;
    case VT_R8: // Double
      m_sizeOfE = sizeof(double);
      m_bindType = SQLT_FLT;
      break;
    }
  // aloca e inicializa los punteros
  size_t sizeOfA = m_sizeOfE * m_nE;
  AP<ub1> apR(new ub1[sizeOfA /*+ sizeof(ub4)*/]);
  m_pR = apR.get();
  //  m_pV = m_pR + sizeof(ub4); 
  //*(ub4*)m_pR = sizeOfA;
  m_sizeOfR = sizeOfA;

  m_maxSize = m_nE;//sizeOfA;

  m_pLen[0] = m_pLen[1] = m_sizeOfE;//8;
  // inicializa

  // libera el seguro
  apR.release();
}

TNumberArrayBind::~TNumberArrayBind()
{
}

void 
TNumberArrayBind::PreWork()
{
  //for_each()
}

void
TNumberArrayBind::put_Value(VARIANT Index, VARIANT Value)
{
  //validar y sacar el valor
}

void
TNumberArrayBind::get_Value(VARIANT Index, VARIANT* retv)
{
}

#endif





















