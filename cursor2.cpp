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
*** DO'S
***/
void
TCursor::_BindParam(VARIANT& Wich, VARIANT& Value, VARIANT& AsType, VARIANT& Length)
{
  // temporalmente aqui
  AP<IIParam> apParam;
  // crea el parametro
  ::_CreateParam(*this, Wich, Value, AsType, Length, apParam);
  // verifica bind previo y lo elimina
  if (-1 == apParam->Number())
    {
      // lo busca para ver si es rebind
      MAP_NAME_2_P_IIPARAM::iterator i = m_mapName2P.find(apParam->Name());
      // si lo encontro?
      if (i != m_mapName2P.end())
	{
	  // ok lo busca en el vector y lo elimina
	  for (V_AP_IIPARAM::iterator b = m_vParam.begin(); b != m_vParam.end(); b++)
	      if ((*b)->UniqueID() == ((*i).second)->UniqueID())
		{
		  if (!(*b)->ZeroRef())
		    RAISE_INTERNAL(DBO_E_RUNTIME_CANT_DESTROY_S_S, 
				   "Param",(*b)->Name().c_str());
		  //
		  m_vParam.erase(b);
		  break;
		}
	  // elimina el key
	  m_mapName2P.erase(i); // verificar que esto borra el punterno solamente
	}
    }
  else
    {
      // lo busca para ver si es rebind
       MAP_NUMBER_2_P_IIPARAM::iterator i = m_mapNumber2P.find(apParam->Number());
      // si lo encontro?
      if (i != m_mapNumber2P.end())
	{
	  // ok lo busca en el vector y lo elimina
	  for (V_AP_IIPARAM::iterator b = m_vParam.begin(); b != m_vParam.end(); b++)
	      if ((*b)->UniqueID() == ((*i).second)->UniqueID())
		{
		  char buf[34];
		  if (!(*b)->ZeroRef())
		    RAISE_INTERNAL(DBO_E_RUNTIME_CANT_DESTROY_S_S, 
				   "Param", itoa((*b)->Number(), buf, 10));
		  m_vParam.erase(b);
		  break;
		}
	  //  elimina el key
	  m_mapNumber2P.erase(i); // verificar que esto borra el punterno solamente
	}
    }
  // bindea 
  apParam->_Bind(); 
  // bind OK agrega
  if (-1 == apParam->Number())
    m_mapName2P[apParam->Name()]     = apParam.get();
  else
    m_mapNumber2P[apParam->Number()] = apParam.get();   
  // agrega al map de TODOS los parametros y libera 
  m_vParam.insert(m_vParam.end(), apParam);
  //m_mapU2P[apParam->UniqueID()] = AP<IIParam>(apParam);
}

/**
  CreateParam
  Primero verifica que el parametro no exista (else genera un error) luego
  crea el parametro (vacio) listo pa' bindear.
  */

void 
TCursor::_Bind(VARIANT& Wich, 
	       dboVarType AsType, VARIANT& StringLength, VARIANT& Value, 
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
  pParam->Bind(AsType, StringLength, Value);
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
  string wich((ANSI(Wich)));
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
TCursor::_Execute(VARIANT& N)
{
  short n;
  if (GetShortFromVariant(n, N))
    CHECK_OCI(oexn(&m_cda, n, 0));    
  else
    // 1. Ejecuta y chequea errores
    CHECK_OCI(oexec(&m_cda));
  //
  m_fIsDirty = false;
}

bool
TCursor::_ExecutePL(VARIANT& N)
{
  // Ejecuta 
  ::oexec(&m_cda);
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

  // 2. Hace el Fetch
  if (fExecute/*0 == _lastRpco*/)
    ::oexfet(&m_cda, m_ulRowsXFetch, 0, 0);
  else
    ::ofen  (&m_cda, m_ulRowsXFetch);

  // Evalua que paso
  switch (m_cda.rc)
    {
    case 0:
      ChangeState(EQryFetched_TheInstance());
      break;

    case 1403:
      ChangeState(EQryEOF_TheInstance());
      break;
    
    default:
      // previo a mandar todo al cuerno...
      for_each(m_vCols.begin(), m_vCols.end(), FPosWorkPreRAISE<AP<IIColumn> >());    
      // Propaga el error
      RAISE_OCI(m_cda.rc);
    }

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
  //  
  CHECK_OCI(oclose(&m_cda));
  //
  ChangeState(EClosed_TheInstance());
}
