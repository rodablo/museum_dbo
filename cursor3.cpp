/**
***     DBO2(1997/1998)
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "cursor.hxx"

/**
*** ECursor
***/
void
ECursor::get_EOF(TCursor* pC, VARIANT_BOOL* retv) 
{
  *retv = VARIANT_FALSE;
}

void 
ECursor::Open(TCursor* pC)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_UNSPECTED_METHOD, "Open");
}

void 
ECursor::ParseSQL(TCursor* pC, string& sSql)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_UNSPECTED_METHOD, "ParseSQL");
}

void 
ECursor::DefineColumns(TCursor* pC)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_UNSPECTED_METHOD, "DefineColumns");
}

void 
ECursor::Bind(TCursor* pC, VARIANT& Wich, 
	      dboVarType AsType, VARIANT& StringLength, Param** retv)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_UNSPECTED_METHOD, "Bind");
}

void 
ECursor::BindArray(TCursor* pC, BSTR Wich, 
		   short ArraySize, dboVarType AsType, VARIANT& StringLength, Param** retv)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_UNSPECTED_METHOD, "BindArray");
}


void 
ECursor::Execute(TCursor* pC, VARIANT& N, VARIANT& Offset)  
{
  RAISE_INTERNAL(DBO_E_RUNTIME_UNSPECTED_METHOD, "Execute");
}

void 
ECursor::Fetch(TCursor* pC, VARIANT_BOOL* /*retv*/)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_UNSPECTED_METHOD, "Fetch");
}

void 
ECursor::Close(TCursor* pC)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_UNSPECTED_METHOD, "Close");
}

/**
*** ESTADOS
***/
class EClosed : public ECursor
{
public:
  void Open(TCursor* pC);
  void Close(TCursor* pC) {}
};

class EOpen : public ECursor
{
public:
  void ParseSQL(TCursor* pC, string& sSql);
  void Close(TCursor* pC);
};

class EDeferredParse : public ECursor
{
public:
  void DefineColumns(TCursor* pC);
  void Bind(TCursor* pC, VARIANT& Wich, 
	    dboVarType AsType, VARIANT& StringLength, Param** retv);
  void BindArray(TCursor* pC, BSTR Wich, 
		 short ArraySize, dboVarType AsType, VARIANT& StringLength, Param** retv);
  void Execute(TCursor* pC, VARIANT& N, VARIANT& Offset);
  void Fetch(TCursor* pC, VARIANT_BOOL* retv);
  void Close(TCursor* pC);

protected:
  void UpdateState(TCursor* pC);
};

class EQryParsed : public ECursor
{
public:
  //virtual void ParseSQL(TCursor* pC, string& sSql);
  void DefineColumns(TCursor* pC);
  void Bind(TCursor* pC, VARIANT& Wich, 
	    dboVarType AsType, VARIANT& StringLength, Param** retv);
  void BindArray(TCursor* pC, BSTR Wich, 
		 short ArraySize, dboVarType AsType, VARIANT& StringLength, Param** retv);
  void Execute(TCursor* pC, VARIANT& N, VARIANT& Offset);  
  void Fetch(TCursor* pC, VARIANT_BOOL* retv);
  void Close(TCursor* pC);
};

class EQryExecuted : public ECursor
{
public:
//   virtual void ParseSQL(TCursor* pC, string& sSql);
  void Execute(TCursor* pC, VARIANT& N, VARIANT& Offset);  
  void Fetch(TCursor* pC, VARIANT_BOOL* retv);
  void Close(TCursor* pC);
};

class EQryFetched : public ECursor
{
public:
  //virtual void ParseSQL(TCursor* pC);
  void Execute(TCursor* pC, VARIANT& N, VARIANT& Offset);  
  void Fetch(TCursor* pC, VARIANT_BOOL* retv);
  void Close(TCursor* pC);
};

class EQryEOF : public ECursor
{
public:
  void get_EOF(TCursor* pC, VARIANT_BOOL* retv);
  //virtual void ParseSQL(TCursor* pC);
  void Execute(TCursor* pC, VARIANT& N, VARIANT& Offset);  
  void Fetch(TCursor* pC, VARIANT_BOOL* retv);
  void Close(TCursor* pC);
};

class EPLParsed : public ECursor
{
public:
  //void ParseSQL(TCursor* pC, string& sSql);
  void Bind(TCursor* pC, VARIANT& Wich, 
	    dboVarType AsType, VARIANT& StringLength, Param** retv);
  void BindArray(TCursor* pC, BSTR Wich, short ArraySize,
		 dboVarType AsType, VARIANT& StringLength, Param** retv);
  void Execute(TCursor* pC, VARIANT& N, VARIANT& Offset);  
  void Close(TCursor* pC);
};

class EPLExecuted : public ECursor
{
public:
  //void ParseSQL(TCursor* pC, string& sSql);
  void Execute(TCursor* pC, VARIANT& N, VARIANT& Offset);  
  void Close(TCursor* pC);
};

class EPLEOF : public ECursor
{
public:
  void get_EOF(TCursor* pC, VARIANT_BOOL* retv);
  //void ParseSQL(TCursor* pC, string& sSql);
  void Execute(TCursor* pC, VARIANT& N, VARIANT& Offset);  
  void Close(TCursor* pC);
};

/**
*** SINGLETONS
***/
ECursor* EClosed_TheInstance()        { static EClosed s;        return &s; }
ECursor* EOpen_TheInstance()          { static EOpen s;          return &s; }
ECursor* EDeferredParse_TheInstance() { static EDeferredParse s; return &s; }

ECursor* EPLParsed_TheInstance()      { static EPLParsed s;      return &s; }
ECursor* EPLExecuted_TheInstance()    { static EPLExecuted s;    return &s; }
ECursor* EPLEOF_TheInstance()         { static EPLEOF s;         return &s; }

ECursor* EQryParsed_TheInstance()     { static EQryParsed s;     return &s; }
ECursor* EQryExecuted_TheInstance()   { static EQryExecuted s;   return &s; }
ECursor* EQryFetched_TheInstance()    { static EQryFetched s;    return &s; }
ECursor* EQryEOF_TheInstance()        { static EQryEOF s;        return &s; }

/**
*** VARIOS
***/
void
EDeferredParse::UpdateState(TCursor* pC)
{
  // si todo salio ok cambia de estado
  switch(pC->m_cda.ft)
    {
    case 4: // SELECT
      pC->ChangeState(EQryParsed_TheInstance());
      break;
    case 34:// PL/SQL EXECUTE
      pC->ChangeState(EPLParsed_TheInstance());
      break;
    default:
      //RAISE_INTERNAL
      break;
    }
}

/**
*** OPEN
***/
void 
EClosed::Open(TCursor* pC)
{
  CHECK_OCI(oopen(&pC->m_cda, pC->m_IISession.GetLDA(), (text*)0, -1, -1, (text*)0, -1));
  // exito
  pC->ChangeState(EOpen_TheInstance());
}

/**
*** PARSESQL
***/
#define DEFER_PARSE    1
#define NATIVE         1
#define VERSION_7      2

void 
EOpen::ParseSQL(TCursor* pC, string& sSql)
{
  // guarga la sentencia
  pC->m_sTextoSql = sSql;
  // parsea la sentencia
  CHECK_OCI(oparse(&pC->m_cda, (text*)pC->m_sTextoSql.c_str(), 
		   (sb4)-1, DEFER_PARSE, (ub4)VERSION_7));
  // si todo salio ok cambia de estado
  switch(pC->m_cda.ft)
    {
    case 0: // DEFER_PARSE
      pC->ChangeState(EDeferredParse_TheInstance());
      break;
    case 4: // SELECT
      pC->ChangeState(EQryParsed_TheInstance());
      break;
    case 34:// PL/SQL EXECUTE
      pC->ChangeState(EPLParsed_TheInstance());
      break;
    default:
      break;
    }
}

/**
*** BIND
***/
void 
EDeferredParse::Bind(TCursor* pC, VARIANT& Wich, 
		     dboVarType AsType, VARIANT& StringLength, 
		     Param** retv)
{
  pC->_Bind(Wich,AsType, StringLength, retv);
  //
  //????? va esto  UpdateState(pC);
}

void 
EQryParsed::Bind(TCursor* pC, VARIANT& Wich, 
		 dboVarType AsType, VARIANT& StringLength, 
		 Param** retv)
{
  pC->_Bind(Wich,AsType, StringLength, retv);
}

void 
EPLParsed::Bind(TCursor* pC, VARIANT& Wich, 
		dboVarType AsType, VARIANT& StringLength,   
		Param** retv)
{
  pC->_Bind(Wich,AsType, StringLength, retv);
}

/**
*** BINDARRAY
***/
void 
EDeferredParse::BindArray(TCursor* pC, BSTR Wich, short ArraySize,  
			  dboVarType AsType, VARIANT& StringLength, Param** retv)
{
  pC->_BindArray(Wich, ArraySize, AsType, StringLength, retv);
}

void 
EQryParsed::BindArray(TCursor* pC, BSTR Wich, short ArraySize,  
		     dboVarType AsType, VARIANT& StringLength, Param** retv)
{
  pC->_BindArray(Wich, ArraySize, AsType, StringLength, retv);
}

void 
EPLParsed::BindArray(TCursor* pC, BSTR Wich, short ArraySize,  
		   dboVarType AsType, VARIANT& StringLength, Param** retv)
{
  pC->_BindArray(Wich, ArraySize, AsType, StringLength, retv);
}

/**
*** DEFINECOLUMN
***/
void 
EDeferredParse::DefineColumns(TCursor* pC)
{
  pC->_DefineColumns();
  UpdateState(pC);
  
} 

void 
EQryParsed::DefineColumns(TCursor* pC)
{
  pC->_DefineColumns();
} 

/**
*** EXECUTE
***/
void 
EDeferredParse::Execute(TCursor* pC, VARIANT& N, VARIANT& Offset)
{
  pC->_Execute(N, Offset);
  UpdateState(pC);
}

void 
EQryParsed::Execute(TCursor* pC, VARIANT& N, VARIANT& Offset)
{
  pC->_Execute(N, Offset);

  pC->ChangeState(EQryExecuted_TheInstance());
}

void 
EQryExecuted::Execute(TCursor* pC, VARIANT& N, VARIANT& Offset)
{
  pC->_Execute(N, Offset);
  //
  // ver si hay que testear algun cambio
}

void 
EQryFetched::Execute(TCursor* pC, VARIANT& N, VARIANT& Offset)
{
  pC->_Execute(N, Offset);
}

void 
EQryEOF::Execute(TCursor* pC, VARIANT& N, VARIANT& Offset)
{
  pC->_Execute(N, Offset);

  pC->ChangeState(EQryExecuted_TheInstance());
}

void 
EPLParsed::Execute(TCursor* pC, VARIANT& N, VARIANT& Offset)
{
  if (pC->_ExecutePL(N, Offset))
    pC->ChangeState(EPLExecuted_TheInstance());
  else
    pC->ChangeState(EPLEOF_TheInstance());
}

void 
EPLExecuted::Execute(TCursor* pC, VARIANT& N, VARIANT& Offset)
{
  if (!pC->_ExecutePL(N, Offset))
    pC->ChangeState(EPLEOF_TheInstance());
}

void 
EPLEOF::Execute(TCursor* pC, VARIANT& N, VARIANT& Offset)
{  
  if (pC->_ExecutePL(N, Offset))
    pC->ChangeState(EPLExecuted_TheInstance());
}

/**
*** FETCH
***/
void 
EDeferredParse::Fetch(TCursor* pC, VARIANT_BOOL* retv)
{
  // define las columnas
  pC->_DefineColumns();
  // resetea
  pC->_lastRpc = 0;
  // primer fetch (esto cambia el estado)
  pC->_Fetch(true, retv);
}  

void 
EQryParsed::Fetch(TCursor* pC, VARIANT_BOOL* retv)
{
  // define las columnas
  if (!pC->m_fColsDefined)
    pC->_DefineColumns();
  // resetea
  pC->_lastRpc = 0;
  // primer fetch (esto cambia el estado)
  pC->_Fetch(true, retv);
}  

void 
EQryExecuted::Fetch(TCursor* pC, VARIANT_BOOL* retv)
{
  // define las columnas si no es reejecucion
  if (!pC->m_fColsDefined)
    pC->_DefineColumns();
  // resetea
  pC->_lastRpc = 0;
  // primer fetch pero ya ejecutado (esto cambia el estado)
  pC->_Fetch(pC->m_fIsDirty/*false*/, retv);
}  

void 
EQryFetched::Fetch(TCursor* pC, VARIANT_BOOL* retv)
{
  // actualiza
  pC->_lastRpc =  (pC->m_fIsDirty) ? 0 :  pC->m_cda.rpc;
  // fetch lo pendiente (si es solo here habria que implementarlo aqui!)
  pC->_Fetch(pC->m_fIsDirty/*false*/, retv);
}  

void 
EQryEOF::Fetch(TCursor* pC, VARIANT_BOOL* retv)
{
  if (pC->m_fIsDirty)
    {
      // resetea
      pC->_lastRpc = 0;
      // primer fetch pero ya ejecutado (esto cambia el estado)
      pC->_Fetch(true/*pC->m_fIsDirty*//*false*/, retv);
    }
  else
    ECursor::Fetch(pC, retv);
}  

/**
*** CLOSE
***/
void 
EOpen::Close(TCursor* pC)
{
  pC->_Close();
}

void 
EDeferredParse::Close(TCursor* pC)
{
  pC->m_sTextoSql.resize(0);
  pC->_Close();
}

void 
EQryParsed::Close(TCursor* pC)
{
  pC->m_sTextoSql.resize(0);
  pC->_Close();
}

void 
EQryExecuted::Close(TCursor* pC)
{
  pC->m_sTextoSql.resize(0);
  pC->_Close();
}

void 
EQryFetched::Close(TCursor* pC)
{
  pC->m_sTextoSql.resize(0);
  pC->_Clear();
  pC->_Close();
}

void 
EQryEOF::Close(TCursor* pC)
{
  pC->m_sTextoSql.resize(0);
  pC->_Clear();
  pC->_Close();
}

void
EPLParsed::Close(TCursor* pC)
{
  pC->m_sTextoSql.resize(0);
  pC->_Clear();
  pC->_Close();
}

void
EPLExecuted::Close(TCursor* pC)
{
  pC->m_sTextoSql.resize(0);
  pC->_Clear();
  pC->_Close();
}

void
EPLEOF::Close(TCursor* pC)
{
  pC->m_sTextoSql.resize(0);
  pC->_Clear();
  pC->_Close();
}



/**
*** GET_EOF
***/
void 
EQryEOF::get_EOF(TCursor* pC, VARIANT_BOOL* retv) 
{
  *retv = VARIANT_TRUE; 
}

void 
EPLEOF::get_EOF(TCursor* pC, VARIANT_BOOL* retv) 
{
  *retv = VARIANT_TRUE; 
}


