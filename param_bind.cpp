/**
*** DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "param.hxx"

#if _PEND_
#pragma message Agregar un error si trunca strings en el put_Value
#endif

/**
*** TS(calar)Bind
***/
class TSBind 
  : public TBind
{
public:
  // 
  virtual void get_Length(VARIANT& Index, short* retv);
  virtual void get_RC(VARIANT& Index, short* retv);
  virtual void get_IsNull(VARIANT& Index, VARIANT_BOOL* retv);
  // 
  virtual void PreWork() {}
  //
  TSBind(TGParam& param);
  virtual ~TSBind() {}

protected: // métodos
  virtual bool   IsNull() {0 > _indp;}

protected: // data 
  //
  sb2  _indp;    // i/o
  ub2  _len;     // i/o
  ub2  _RC;      // o
};

TSBind::TSBind(TGParam& param)
  : TBind(param)
{
  _indp = -1;		// inicialmente nulo
}

void
TSBind::get_Length(VARIANT& Index, short* retv)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_PROP_ONLY_IN_ARRAYS_S, "Length");
}

void
TSBind::get_RC(VARIANT& Index, short* retv)
{
  RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_PROP_ONLY_IN_ARRAYS_S, "Length");
}

void
TSBind::get_IsNull(VARIANT& Index, VARIANT_BOOL* retv)
{
  // no puede tener indice si no es array
  if (VT_ERROR != V_VT(&Index))
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_ONLY_IN_ARRAYS);
  // 
  *retv = IsNull() ? VARIANT_TRUE : VARIANT_FALSE;  
}

/**
*** TTSBind
***/
template <VARTYPE vt, class vT, sword sqlt, class sqlT>
class TTSBind  
  :  public TSBind
{
public:
  void put_Value(VARIANT& Index, VARIANT& Value);
  void get_Value(VARIANT& Index, VARIANT* retv);  

  //  void PreWork();

  TTSBind(TGParam& param);
  virtual ~TTSBind() {}

protected: // métodos
protected: // data 
  sqlT _data;
};

template <VARTYPE vt, class vT, sword sqlt, class sqlT>
TTSBind<vt, vT, sqlt, sqlT>::TTSBind(TGParam& param)
  :  TSBind(param)
{
  if (_Param.IsAlfaN())
    CHECK_OCI(::obndrv(_Param, (text*)_Param.Name().c_str(), -1, 
		       (ub1*)&_data, (ub4)sizeof(sqlT), sqlt, 0, &_indp, 
		       (text*)0, -1, -1));
  else
    CHECK_OCI(::obndrn(_Param, _Param.Number(), 
		       (ub1*)&_data, (ub4)sizeof(sqlT), sqlt, 0, &_indp,
		       (text*)0, -1, -1));
}

template <VARTYPE vt, class vT, sword sqlt, class sqlT>
void
TTSBind<vt, vT, sqlt, sqlT>::put_Value(VARIANT& Index, VARIANT& Value)
{
  // no puede tener indice si no es array
  if (VT_ERROR != V_VT(&Index))
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_ONLY_IN_ARRAYS);
  // procesa el Nulo
  if (VT_NULL == V_VT(&Value))
    _indp = -1;
  else
    {
      // asigna
      if (_Param.IsStrict())
	{
	  // es estricto
	  if (vt != V_VT(&Value))
	    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_VALUE_TYPE_MISMATCH_I_I, (int)vt, (int)V_VT(&Value));
	  // asigna
	  _data = *reinterpret_cast<vT*>(&Value.lVal);
	}
      else
	{
	  // convierte (si es necesario) y copia
	  VARIANT v;
	  VariantInit(&v); 
	  CHECK_HRESULT(VariantChangeType(&v, &Value, 0, vt));
	  // asigna (primero castea la expresion al tipo del template
	  //_data = *(vT*)&v.lVal;	
	  _data = *reinterpret_cast<vT*>(&v.lVal);
	  // libera lo que fuese
	  VariantClear(&v);
	}
      // saca el nulo
      _indp = 0; 
    }
  // notifica
  _Param.SetDirtyFlag();
}

template <VARTYPE vt, class vT, sword sqlt, class sqlT>
void
TTSBind<vt, vT, sqlt, sqlT>::get_Value(VARIANT& Index, VARIANT* retv)
{
  // no puede tener indice si no es array
  if (VT_ERROR != V_VT(&Index))
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_ONLY_IN_ARRAYS);
  //
  CHECK_HRESULT(VariantClear(retv));
  //
  if (IsNull())
    V_VT(retv) = VT_NULL;
  else
    {
      retv->vt = vt;
      // asigna (eventualmente usando operador de conversion)	
      *reinterpret_cast<vT*>(&retv->lVal) = _data; 
    }
}

/**
*** TStringSBind
***/
class TStringSBind
  :  public TSBind
{
public:
  void put_Value(VARIANT& Index, VARIANT& Value);
  void get_Value(VARIANT& Index, VARIANT* retv);  

  TStringSBind(TGParam& param, size_t length);
  virtual ~TStringSBind();

protected: // métodos
protected: // data 
  char*  _data;
  size_t _length;
};

TStringSBind::TStringSBind(TGParam& param, size_t length)
  :  TSBind(param)
{
  _length = length + 1;
  _data   = new char[_length];
  // engancha 
  AP<char> p(_data);
  // bindea
  if (_Param.IsAlfaN())
    CHECK_OCI(::obndrv(_Param, (text*)_Param.Name().c_str(), -1, 
		       (ub1*)_data, (ub4)_length, SQLT_STR, 0, &_indp, 
		       (text*)0, -1, -1));
  else
    CHECK_OCI(::obndrn(_Param, _Param.Number(), 
		       (ub1*)_data, (ub4)_length, SQLT_STR, 0, &_indp,
		       (text*)0, -1, -1));
  // desengancha
  p.release();
}

TStringSBind::~TStringSBind() 
{
  delete [] _data;
}

void 
TStringSBind::put_Value(VARIANT& Index, VARIANT& Value)
{
  // no puede tener indice si no es array
  if (VT_ERROR != V_VT(&Index))
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_ONLY_IN_ARRAYS);
  // procesa el Nulo
  if (VT_NULL == V_VT(&Value))
    _indp = -1;
  else
    {
      // asigna
      if (_Param.IsStrict())
	{
	  // es estricto
	  if (VT_BSTR != V_VT(&Value))
	    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_VALUE_TYPE_MISMATCH_I_I, 
			   (int)VT_BSTR, (int)V_VT(&Value));
	  // verifica el length
	  size_t valueLength = SysStringLen(V_BSTR(&Value));
	  if (valueLength > _length - 1)
	    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_STRING_TRUNCATION_I_I, 
			   int(_length - 1), int(valueLength));
	  // asigna
	  lstrcpyn(_data, ANSI(V_BSTR(&Value)), _length);
	}
      else
	{
	  // convierte (si es necesario) y copia
	  VARIANT v;
	  VariantInit(&v); 
	  CHECK_HRESULT(VariantChangeType(&v, &Value, 0, VT_BSTR));
	  // verifica el length
	  size_t valueLength = SysStringLen(V_BSTR(&v));
	  if (valueLength > _length - 1)
	    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_STRING_TRUNCATION_I_I, 
			   int(_length - 1), int(valueLength));
	  // asigna
	  lstrcpyn(_data, ANSI(V_BSTR(&v)), _length);
	  // libera el variant (si genero la excepcion esto no se desaloco)
	  VariantClear(&v);
	}
      // saca el nulo
      _indp = 0; 
    }
  // notifica
  _Param.SetDirtyFlag();
}

void 
TStringSBind::get_Value(VARIANT& Index, VARIANT* retv)
{
  // no puede tener indice si no es array
  if (VT_ERROR != V_VT(&Index))
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_ONLY_IN_ARRAYS);
  //
  CHECK_HRESULT(VariantClear(retv));
  //
  if (IsNull())
    V_VT(retv) = VT_NULL;
  else
    {
      V_VT(retv)   = VT_BSTR;
      V_BSTR(retv) = WIDE(_data).SysAllocString();
    }
}

void
TGParam::Bind(dboVarType AsType, VARIANT& StringLength)
{
  // primero que nada verifica que no se hayan olvidado el parentesis
  // 0 o -1 en AsType con StringLength vacio o bool en StringLength
  if ( (0 >= AsType && VT_ERROR == V_VT(&StringLength))
       ||
       VT_BOOL == V_VT(&StringLength))
    RAISE_INTERNAL(DBO_E_RUNTIME_LVALUE_NEED_PARETHESIS);
  //
  if (VT_BSTR == AsType)
    {
      size_t length;
      // determina el length
      if (VT_ERROR != V_VT(&StringLength))
	{
	  short _length;
	  if (!GetShortFromVariant(_length, StringLength))
	    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_LENGTH_TYPE_MISMATCH_I, V_VT(&StringLength));
	  length = _length;
	}
      //       else if (/*VT_ERROR !=*/VT_BSTR == V_VT(&Value))
      // 	{		
      // 	  length = SysStringLen(V_BSTR(&Value));
      // 	}
      else	
	RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_CANT_DETERMINE_STRING_SIZE);

      // verifica que no sea 0
      if (0 == length)
	RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_STRINGLENGTH_CANT_BE_ZERO);
      // crea
      _apBind = new TStringSBind(*this, length);
    }
  else
    {
      // solo en String
      if (VT_ERROR != V_VT(&StringLength))
	RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_STRINGLENGTH_ONLY_IN_STRING);
      // 
      switch (AsType)
	{
//      case VT_UI1:// Byte 
	case VT_I2: 
	  _apBind = new TTSBind<VT_I2,   short,  SQLT_INT, short>(*this);  break; // Integer
	case VT_I4: 
	  _apBind = new TTSBind<VT_I4,   long,   SQLT_INT, long>(*this);   break; // Long
	case VT_R4: 
	  _apBind = new TTSBind<VT_R4,   float,  SQLT_FLT, float>(*this);  break; // Float
	case VT_R8: 
	  _apBind = new TTSBind<VT_R8,   double, SQLT_FLT, double>(*this); break; // Double
	case VT_DATE:
	  _apBind = new TTSBind<VT_DATE, DATE,   SQLT_DAT, ODATE>(*this);  break; // DATE
//      case VT_CY:
//        _apBind = new TTSBind<VT_CY,   CY,     SQLT_FLT, OCY>(*this, arraySize);    break; // CY
	default:
	  RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_TYPE_MISMATCH_I, (int)AsType);
	}        
    }
}


