/**
***     DBO2(1997/1998)
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "cursor.hxx"

IIColumn* 
TCursor::GetColumn(int index)
{
  // valida el indice
  if (0 == index || index > m_vCols.size())
    RAISE_INTERNAL(DBO_E_RUNTIME_BAD_COL_INDEX_L, index);
  // retona
  return m_vCols[index - 1].get();
}

IIColumn* 
TCursor::GetColumn(string& name)
{
  // busca
  map<string, long, less<string> >::iterator i = m_mName2Col.find(name);
  // encontro ?
  if (i == m_mName2Col.end())
    RAISE_INTERNAL(DBO_E_RUNTIME_BAD_COL_NAME_S, name.c_str());
  // retorna
  return m_vCols[(*i).second].get();
}

IIParam* 
TCursor::GetParam(sword Number)
{  
  // busca
  MAP_NUMBER_2_P_IIPARAM::iterator i = m_mapNumber2P.find(Number);
  // no encontro 
  if (i == m_mapNumber2P.end())
    RAISE_INTERNAL(DBO_E_RUNTIME_BAD_PARAM_NUMBER_L,(long)Number);
  // retorna
  return (*i).second;
}

IIParam* 
TCursor::GetParam(string& name)
{
  // busca
   MAP_NAME_2_P_IIPARAM::iterator i = m_mapName2P.find(name);
  // no encontro ?
  if (i == m_mapName2P.end())
    RAISE_INTERNAL(DBO_E_RUNTIME_BAD_PARAM_NAME_S, name.c_str());
  // retorna
  return (*i).second;
}

/**
***
***
*** TEMPLATE TIENUMVARIANT
***/
template <class Tcontainer>
class TIENUMVARIANT
  :  public IEnumVARIANT
{
public:
  /// IUnknown
  virtual HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
  virtual ULONG   __stdcall AddRef() { 
    return ++_cRef; }
  virtual ULONG   __stdcall Release();

  /// IEnumVariant
  virtual HRESULT __stdcall Next(unsigned long celt, 
				 VARIANT* rgvar, 
				 unsigned long* pceltFetched);
  virtual HRESULT __stdcall Skip(unsigned long celt);
  virtual HRESULT __stdcall Reset();
  virtual HRESULT __stdcall Clone(IEnumVARIANT** ppEnum);

  TIENUMVARIANT(IICursor& iicursor, Tcontainer& Container);
  virtual ~TIENUMVARIANT();  

protected:
  ULONG         	_cRef;             // ref count
  IICursor&  		_IICursor;
  //
  Tcontainer&           _Container;
  Tcontainer::iterator 	_iterator;
};

template <class Tcontainer>
TIENUMVARIANT<Tcontainer>::TIENUMVARIANT(IICursor& iicursor, Tcontainer& Container)
  : _IICursor(iicursor),
    _Container(Container)
{
  _cRef = 0;
  _IICursor.AddRef();
  _iterator = _Container.begin();
}

template <class Tcontainer>
TIENUMVARIANT<Tcontainer>::~TIENUMVARIANT()
{
  _IICursor.Release();
}

template <class Tcontainer>
inline HRESULT __stdcall
TIENUMVARIANT<Tcontainer>::QueryInterface(REFIID riid, LPVOID* ppvObj) 
{
  if (0 == ppvObj)
    return E_INVALIDARG;

  *ppvObj = 0;
  // es vtable o ISupportErrorInfo?
  if (IsEqualIID(riid, IID_IEnumVARIANT) ||
      IsEqualIID(riid, IID_IUnknown))
    *ppvObj = this;
  else
    return E_NOINTERFACE;

  // retorna retenida
  ((IUnknown*)*ppvObj)->AddRef(); 
  return NOERROR;
}

template <class Tcontainer>
ULONG __stdcall
TIENUMVARIANT<Tcontainer>::Release()
{
  if(0 == --_cRef)
    {
      delete this; 
      return 0L;   // no puede retornar m_cRef porque aqui ya no existe
    }
  return _cRef;
}

/**
*** IEnumVARIANT
***/
template <class Tcontainer>
HRESULT __stdcall 
TIENUMVARIANT<Tcontainer>::Next(unsigned long celt, 
				 VARIANT* rgvar, 
				 unsigned long* pceltFetched)
{
  VARIANT* pV;
  // pesimismo inicial
  if (0 != pceltFetched)
    *pceltFetched = 0;
  // inicializa el array resultado
  for (pV = rgvar; pV < rgvar + celt; pV++)
    VariantInit(pV);
  // itera y retorna tantos como se solicitaron 
  for (pV = rgvar; ( pV < rgvar + celt ) && ( _iterator < _Container.end() );
       pV++, _iterator++)
    {
      // para la interface;
      IDispatch* pIDispatch;
      if (SUCCEEDED((*_iterator)->QueryInterface(IID_IDispatch, (void**)&pIDispatch)))
	{
	  V_VT(pV)       = VT_DISPATCH;
	  V_DISPATCH(pV) = pIDispatch;
	  // notifica
	  if (0 != pceltFetched)
	    (*pceltFetched)++;
	}
    }
  // returna en funcion del exito
  if (celt > (pV - rgvar))
    return ResultFromScode(S_FALSE);
  else	
    return NOERROR;
}

template <class Tcontainer>
HRESULT __stdcall 
TIENUMVARIANT<Tcontainer>::Skip(unsigned long celt)
{
  // se pasa del final ?
  if (celt > _Container.end() - _iterator)
    {
      _iterator = _Container.end();
      return ResultFromScode(S_FALSE);
    }
  else
    {
      _iterator = _iterator + celt;
      return NOERROR;
    }
}

template <class Tcontainer>
HRESULT __stdcall
TIENUMVARIANT<Tcontainer>::Reset()
{
  // resetea el iterador local de la enumeracion
  _iterator = _Container.begin();
  //
  return NOERROR;
}

template <class Tcontainer>
HRESULT __stdcall 
TIENUMVARIANT<Tcontainer>::Clone(IEnumVARIANT** ppEnum)
{
  //
  if (0 != ppEnum)
    *ppEnum = 0;
  //
  return NOERROR;
}

HRESULT   
TCursor::NewEnumColumns(IUnknown** ppIUnknown)
{
  if (0 == ppIUnknown)
    return NOERROR;
  // 
  TIENUMVARIANT<V_AP_IICOLUMN > * pIEV = new TIENUMVARIANT<V_AP_IICOLUMN >(*this, m_vCols);
  //
  HRESULT hr = pIEV->QueryInterface(IID_IUnknown, (void**)ppIUnknown);
  if (FAILED(hr))
    delete pIEV;
  //
  return hr;
}

HRESULT   
TCursor::NewEnumParams(IUnknown** ppIUnknown)
{
  if (0 == ppIUnknown)
    return NOERROR;
  // 
  TIENUMVARIANT<V_AP_IIPARAM > * pIEV = new TIENUMVARIANT<V_AP_IIPARAM >(*this, m_vParam);
  //
  HRESULT hr = pIEV->QueryInterface(IID_IUnknown, (void**)ppIUnknown);
  if (FAILED(hr))
    delete pIEV;
  //
  return hr;
}

// /**
// *** DO'S
// ***/
void 
TCursor::_Bind(VARIANT& Wich, 
	       dboVarType AsType, VARIANT& StringLength, 
	       Param** retv)
{
  IIParam* pParam;
  // determina tipo de parametro (Numerico o Alfa) por el VT_ del indice
  long   wichN = -1;
  string wichS;
  switch (V_VT(&Wich)) // (pasar esto al constructor de TScalarParam, unificando los constructores)
    {
    case VT_UI1:// Byte
      wichN = (long)V_UI1(&Wich); break;
    case VT_I2: // Integer
      wichN = (long)V_I2(&Wich);  break;            
    case VT_I4: // Long
      wichN = (long)V_I4(&Wich);  break;
    case VT_R4: // Single
      wichN = (long)V_R4(&Wich);  break;
    case VT_R8: // Double
      wichN = (long)V_R8(&Wich);  break;
    case VT_BSTR:
      wichS = ANSI(V_BSTR(&Wich));
      break;
    default:
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NAME_TYPE_MISMATCH_I, V_VT(&Wich));
    }
  // procede según sea numerico u alfa
  if (-1 == wichN)
    { // lo busca para ver si es rebind
      MAP_NAME_2_P_IIPARAM::iterator a = m_mapName2P.find(wichS);
      if (a != m_mapName2P.end())
	{ // ok lo busca en el vector
	  for (V_AP_IIPARAM::iterator b = m_vParam.begin(); b != m_vParam.end(); b++)
	    if ((*b)->UniqueID() == ((*a).second)->UniqueID())
	      {
		(*b)->ClearBind();   // unbind
		pParam = (*b).get(); 
		break;
	      }
	}
      else
	{ // primer bind
	  AP<IIParam> apP;              // crea uno 
	  CreateStringParam(*this, wichS, apP);      
	  pParam = apP.get();      
	  // 
	  m_mapName2P[apP->Name()] = apP.get();  // agraga el key
	  m_vParam.insert(m_vParam.end(), apP);
	}
    }
  else // es numérico
    { // lo busca para ver si es rebind
      MAP_NUMBER_2_P_IIPARAM::iterator a = m_mapNumber2P.find(wichN);
      if (a != m_mapNumber2P.end())
	{ // ok lo busca en el vector
	  for (V_AP_IIPARAM::iterator b = m_vParam.begin(); b != m_vParam.end(); b++)
	    if ((*b)->UniqueID() == ((*a).second)->UniqueID())
	      {
		(*b)->ClearBind();   // unbind
		pParam = (*b).get();    
		break;
	      }
	}
      else
	{ // primer bind
	  AP<IIParam> apP;              // crea uno 
	  CreateNumericParam(*this, wichN, apP);      
	  pParam = apP.get();      
	  // agraga el key 
	  m_mapNumber2P[apP->Number()] = apP.get();
	  m_vParam.insert(m_vParam.end(), apP);
	}
    }
  // ahora bindea
  pParam->Bind(AsType, StringLength/*, Value*/);
  // retorna si se solicito
  if (0 != retv)
    {
      pParam->AddRef();
      *retv = pParam;
    }
}

void 
TCursor::_BindArray(BSTR Wich, short ArraySize,
		    dboVarType AsType, VARIANT& StringLength, 
		    Param** retv)
{
  IIParam* pParam;
  //
  string wich = ANSI(Wich);
  // busca a ver si es rebind
  MAP_NAME_2_P_IIPARAM::iterator a = m_mapName2P.find(wich);
  if (a != m_mapName2P.end())
    { // ok lo busca en el vector y asigna al temp (por si falla)
      for (V_AP_IIPARAM::iterator b = m_vParam.begin(); b != m_vParam.end(); b++)
	if ((*b)->UniqueID() == ((*a).second)->UniqueID())
	  {
	    (*b)->ClearBind();	    // unbind
	    pParam = (*b).get();    // elimina el key
	    break;                  // no more 
	  }
    }
  else
    { // primer bind
      AP<IIParam> apP;              // crea uno 
      CreateStringParam(*this, wich, apP);      
      pParam = apP.get();      
      // 
      m_mapName2P[apP->Name()] = apP.get();  // agraga el key
      m_vParam.insert(m_vParam.end(), apP);
    }
  // ahora bindea
  pParam->BindArray(AsType, ArraySize, StringLength);
  // retorna si se solicito
  if (0 != retv)
    {
      pParam->AddRef();
      *retv = pParam;
    }
}

void
TCursor::_DefineColumns()
{
  if (m_fColsDefined) 
    _UndefineColumns();

  // 3. Itera y genera las columnas
  for (unsigned pos = 1; ; pos++)
    {
      IIColumn* pCol;
      // brea la columna
      if (!CreateColumn(*this, pos, pCol))
 	break;
      //
      //m_fColsDefined = true;
      //pCol->AddRef()
      // atrapa
      AP<IIColumn> apCol(pCol);
      // Agrega al vector
      m_vCols.insert(m_vCols.end(), apCol);
      //m_vCols[pos] = apCol.release();
      // Agrega al map
      m_mName2Col[pCol->Name()] = pos - 1;
    }
  // 5. Resultados
  m_fColsDefined = true;
  //m_ulCount = 0;
  //m_ulCountAcumulado = 0;
  //m_fEOF = false;
}


void
TCursor::_UndefineColumns()
{
  // verifica que no existan referencias activas
  for (V_AP_IICOLUMN::iterator c = m_vCols.begin(); c != m_vCols.end(); c++)
    if (!(*c)->ZeroRef())
      RAISE_INTERNAL(DBO_E_RUNTIME_CANT_DESTROY_S_S, "Column",(*c)->Name().c_str());

  // borra
  m_mName2Col.erase(m_mName2Col.begin(), m_mName2Col.end());
  m_vCols.erase(m_vCols.begin(), m_vCols.end());

  //
  m_fColsDefined = false;
}

void
TCursor::_UndefineColumn(int pos)
{
}


void
TCursor::_UndefineColumn(string& name)
{
}

void
TCursor::_Execute(VARIANT& N, VARIANT& Offset)
{
  short n = 0, offset = 0;
  // valida y obtiene N 
  if (VT_ERROR != V_VT(&N) && !GetShortFromVariant(n, N))
    /* N type mismatch */;
  // valida y obtiene Offset
  if (VT_ERROR != V_VT(&Offset) && !GetShortFromVariant(offset, Offset))
    /* pendiente Offset type mismatch */;
  // inicializa si es piecewise
  if (0 != m_pPWParam)
    m_pPWParam->Start();
  // loop x si es piecewise
  while (1) {
    // ejecuta
    if (0 != n)
      ::oexn(&m_cda, n, offset);
    else
      ::oexec(&m_cda);
    // evalua el resultado
    switch (m_cda.rc) {    
    case 0:
      if (0 != m_pPWParam)
	m_pPWParam->Finalize();
      goto break_break; 

    case 3129: //OCI_MORE_INSERT_PIECES
      if (0 != m_pPWParam) 
  	m_pPWParam->RequestPiece(m_conection.Sink());
      break;

    case 1403:
    default:
      if (0 != m_pPWParam)
	m_pPWParam->Finalize();
      RAISE_OCI(m_cda.rc);
    }
  }
 break_break:
  //
  m_fIsDirty = false;
}

bool
TCursor::_ExecutePL(VARIANT& N, VARIANT& Offset)
{
  // Ejecuta 
  short n;
  if (GetShortFromVariant(n, N))
   {
      short offset = 0;
      // 
      if (VT_ERROR != V_VT(&Offset))
	GetShortFromVariant(offset, Offset);	
      //
      CHECK_OCI(oexn(&m_cda, n, offset));    
   }
  else
    // 1. Ejecuta y chequea errores
    CHECK_OCI(oexec(&m_cda));

  // Evalua que paso
  switch (m_cda.rc)
    {
    case 0:
      m_fIsDirty = false;
      return true;
    case 1403:
      return false;
    default:
      // Propaga el error
      RAISE_OCI(m_cda.rc);
      return false; // compilador gil!
    }
}

void
TCursor::_Fetch(bool fExecute, VARIANT_BOOL* retv)
{
  // pesimismo inicial;
  *retv = VARIANT_FALSE;

  // if execute _lastrpc = 0 o ver si no usar rowcount internamente 

  // PreFetch stuff
  for_each(m_vCols.begin(),  m_vCols.end(),  FPreWork<AP<IIColumn> >());    
  for_each(m_vParam.begin(), m_vParam.end(), FPreWork<AP<IIParam> >());

  // si es necesario ejecuta
  if (fExecute)
    CHECK_OCI(::oexec(&m_cda));
  // al fetch
  bool piecewise = false;
  bool first = true;
  while (1) {
    // hace el fetch
    ::ofen(&m_cda, m_ulRowsXFetch);
    switch (m_cda.rc) {
    case 0:
      // entrega la ultima pieza si la hubiese
      if (piecewise && 0 != m_pPWColumn) 
  	m_pPWColumn->DeliverPiece(m_conection.Sink());
      //
      ChangeState(EQryFetched_TheInstance());
      goto break_break;

    case 3130: // OCI_MORE_FETCH_PIECES 
      // chequeo
      /*if (0 == m_pPWColumn)  
	internal;*/
      // entrega la data si no es la primera pasada
      piecewise = true;
      if (first)
	first = false;
      else 
	m_pPWColumn->DeliverPiece(m_conection.Sink());
      // seprepara para el siguiente chunk
      m_pPWColumn->Prepare4NextPiece();
      break;

    case 1403:
      ChangeState(EQryEOF_TheInstance());
      goto break_break;
    
    default:
      // previo a mandar todo al cuerno...
      for_each(m_vCols.begin(), m_vCols.end(), FPosWorkPreRAISE<AP<IIColumn> >());    
      // Propaga el error
      RAISE_OCI(m_cda.rc);
    }
  }
 break_break:
  //
  m_fIsDirty = false;

  // PosFetch stuff
  for_each(m_vCols.begin(),   m_vCols.end(),   FPosWork<AP<IIColumn> >());    
  //for_each(m_vParams.begin(), m_vParams.end(), FPosFetch<AP<IIParam> >());

  // hay data nueva?
  if (_lastRpc != m_cda.rpc)
    // confirma que hubo data nueva
    *retv = VARIANT_TRUE;
}

void 
TCursor::_Clear()
{
  // verifica que no existan referencias activas
  for (V_AP_IICOLUMN::iterator c = m_vCols.begin(); c != m_vCols.end(); c++)
    if (!(*c)->ZeroRef())
      RAISE_INTERNAL(DBO_E_RUNTIME_CANT_DESTROY_S_S, "Column",(*c)->Name().c_str());
  //
  for (V_AP_IIPARAM::iterator p = m_vParam.begin(); p != m_vParam.end(); p++)
    if (!(*p)->ZeroRef())
      RAISE_INTERNAL(DBO_E_RUNTIME_CANT_DESTROY_S_S, "Param",(*p)->Name().c_str());

  // column
  m_mName2Col.erase(m_mName2Col.begin(), m_mName2Col.end());
  m_vCols.erase(m_vCols.begin(), m_vCols.end());

  // param
  m_mapName2P.erase(m_mapName2P.begin(), m_mapName2P.end());
  m_mapNumber2P.erase(m_mapNumber2P.begin(), m_mapNumber2P.end());
  m_vParam.erase(m_vParam.begin(), m_vParam.end());

  // muchos releases no deletes
}

void 
TCursor::_Close()
{
  //  no hay quien atrape esto cuando esta en piecewise
  //CHECK_OCI(oclose(&m_cda));
  oclose(&m_cda);
  //
  ChangeState(EClosed_TheInstance());
}
