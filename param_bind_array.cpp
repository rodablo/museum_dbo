/**
*** DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "param.hxx"

#define __OBNDRA__ 0

#if _PEND_
#pragma message message Los nuevos Params por defecto son NULL
#endif
 
/**
*** TABind
***/
class TABind 
  : public TBind
{
public:
  // 
  virtual void get_Length(VARIANT& Index, short* retv);
  virtual void get_RC(VARIANT& Index, short* retv);
  virtual void get_IsNull(VARIANT& Index, VARIANT_BOOL* retv);
  // 
  //  virtual void PreWork();
  //
  TABind(TGParam& param, size_t nE);
  virtual ~TABind() {}

protected: // métodos
  virtual size_t ValidateIndex(VARIANT& Index);
  virtual bool   IsNull(size_t pos);// {return 0 > _apIndp[pos];} 

protected: // data 
  size_t   _nE;
  //
  AP<sb2>  _apIndp;    // i/o
  AP<ub2>  _apLen;     // i/o
  AP<ub2>  _apRC;      // o
};

TABind::TABind(TGParam& param, size_t nE)
  : TBind(param)
{
  _nE      = nE; 
  // aloca los array's auxiliares (_apIndp & _apLen se DEBEN! inicializar en las classes derivadas)
  _apIndp  = new sb2[nE];  
  _apLen   = new ub2[nE];  
  _apRC    = new ub2[nE]; 
}

size_t 
TABind::ValidateIndex(VARIANT& Index)
{
  // index es obligatorio
  if (VT_ERROR == V_VT(&Index))
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_NEED_INDEX_IN_ARRAYS);
  // 
  short index;
  if (!GetShortFromVariant(index, Index))
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_TYPE_MISMATCH_I, V_VT(&Index));
  // validate elements
  //
  if (0 == index || _nE < index)
    RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_INDEX_OUT_OF_RANGE_L,(long)index); 
#if _PEND_
#pragma message verificar poque no aparece el index en el <>
#endif
  //
  return index - 1;
}

bool 
TABind::IsNull(size_t pos) 
{
  return 0 > _apIndp[pos];
} 

void
TABind::get_Length(VARIANT& Index, short* retv)
{
  *retv = _apLen[ValidateIndex(Index)];
}

void
TABind::get_RC(VARIANT& Index, short* retv)
{
  *retv = _apRC[ValidateIndex(Index)];
}

void
TABind::get_IsNull(VARIANT& Index, VARIANT_BOOL* retv)
{
  *retv = IsNull(ValidateIndex(Index)) ? VARIANT_TRUE : VARIANT_FALSE;  
}

/**
*** TTABind
***/
template <VARTYPE vt, class vT, sword sqlt, class sqlT>
class TTABind  
  :  public TABind
{
public:
  void put_Value(VARIANT& Index, VARIANT& Value);
  void get_Value(VARIANT& Index, VARIANT* retv);  

  void PreWork();

  TTABind(TGParam& param, size_t nE);
  virtual ~TTABind() {}

protected: // métodos
protected: // data 
  AP<sqlT> _ap;
  //
  ub4      _curSize;   // i/o
};

template <VARTYPE vt, class vT, sword sqlt, class sqlT>
TTABind<vt, vT, sqlt, sqlT>::TTABind(TGParam& param, size_t nE)
  :  TABind(param, nE)
{
  // aloca el array de los datos
  _ap = new sqlT[nE];
  // inicializa _apIndp & _apLen
  for (int i = 0; i < nE; i++)
    {
      _apIndp[i] = -1;                  // inicialmente nulo
      _apLen[i]  = (ub2)sizeof(sqlT);   // len fijo
      _apRC[i]   = 0;
    }
  // ??
  _curSize = nE;
  // bindea
#if __OBNDRA__
  CHECK_OCI(::obndra(_Param, (text*)_Param.Name().c_str(), -1,
                     (ub1*)_ap.get(), (ub4)sizeof(sqlT), sqlt, -1,
                     _apIndp.get(), _apLen.get(), _apRC.get(),
                     (ub4)0/*20*//*nE*/,0/* &_curSize*/,
                     (text*)0, -1, -1));
#else
  CHECK_OCI(::obndrv(_Param, (text*)_Param.Name().c_str(), -1, 
		     (ub1*)_ap.get(), (ub4)sizeof(sqlT), sqlt, 0, _apIndp.get(), 
		     (text*)0, -1, -1));
#endif
}

template <VARTYPE vt, class vT, sword sqlt, class sqlT>
void
TTABind<vt, vT, sqlt, sqlT>::PreWork()
{
#if _PEND_
#pragma message Falta inicializar null & length
#endif
}

template <VARTYPE vt, class vT, sword sqlt, class sqlT>
void
TTABind<vt, vT, sqlt, sqlT>::put_Value(VARIANT& Index, VARIANT& Value)
{
  // valida indice
  size_t pos = ValidateIndex(Index);
  // procesa el Nulo
  if (VT_NULL == V_VT(&Value))
    _apIndp[pos] = -1;
  else
    {
      // asigna
      if (_Param.IsStrict())
        {
          // es estricto
          if (vt != V_VT(&Value))
            RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_VALUE_TYPE_MISMATCH_I_I, (int)vt, (int)V_VT(&Value));
          // asigna utilizando operator vT ()
	  _ap[pos] = *reinterpret_cast<vT*>(&Value.lVal);
	  //_ap[pos] = *static_cast<sqlT*>( static_cast<vT*>(&Value.lVal) );
        }
      else
        {
          // convierte (si es necesario) y copia
          VARIANT v;
          VariantInit(&v); //   V_VT(&v) = vt;
          CHECK_HRESULT(VariantChangeType(&v, &Value, 0, vt));
	  // asigna utilizando operator vT ()	
	  _ap[pos] = *reinterpret_cast<vT*>(&v.lVal);		
          //
          VariantClear(&v);
        }
      // declara tamaño
      // no tiene porque cambiar _apLen[pos] = sizeof(sqlT);
      // saca el nulo
      _apIndp[pos] = 0;
    }
  // notifica
  _Param.SetDirtyFlag();
}

template <VARTYPE vt, class vT, sword sqlt, class sqlT>
void
TTABind<vt, vT, sqlt, sqlT>::get_Value(VARIANT& Index, VARIANT* retv)
{
  //
  CHECK_HRESULT(VariantClear(retv));
  //
  size_t pos = ValidateIndex(Index);
  //
  if (IsNull(pos))
    V_VT(retv) = VT_NULL;
  else
    {
      retv->vt = vt;
      // asigna (eventualmente usando operador de conversion)
      *reinterpret_cast<vT*>(&retv->lVal) = _ap[pos];
    }
}

/**
*** TStringABind
***/
class TStringABind
  :  public TABind
{
public:
  void put_Value(VARIANT& Index, VARIANT& Value);
  void get_Value(VARIANT& Index, VARIANT* retv);  

  void PreWork(){}

  TStringABind(TGParam& param, size_t nE, size_t length);
  virtual ~TStringABind();

protected: // métodos
protected: // data 
  char*   _p;
  size_t  _nE;
  size_t  _length;
  //
  ub4    _curSize;   // i/o
};

TStringABind::TStringABind(TGParam& param, size_t nE, size_t length)
  :  TABind(param, nE)
{
  _p       = new char[nE * (length + 1)];
  _nE      = nE;
  _curSize = nE;
  _length  = length + 1; 
  // inicializa _apIndp & _apLen
  for (int i = 0; i < nE; i++)
    {
      _apIndp[i] = -1;                  // inicialmente nulo
      _apLen[i]  = (ub2)_length;        // len fijo
      _apRC[i]   = 0;	
    }
  //
  AP<char> p(_p);
  //
#if __OBNDRA__
  CHECK_OCI(::obndra(_Param, (text*)_Param.Name().c_str(), -1,
                     (ub1*)_p, (ub4)_length, SQLT_STR, -1,
                     _apIndp.get(), _apLen.get(), _apRC.get(),
                     (ub4)0/*200 nE*/, 0/*&_curSize*/,
                     (text*)0, -1, -1));
#else
  CHECK_OCI(::obndrv(_Param, (text*)_Param.Name().c_str(), -1, 
		     (ub1*)_p, (ub4)_length, SQLT_STR, 0, _apIndp.get(), 
		     (text*)0, -1, -1));			
#endif
  //
  p.release();
}

TStringABind::~TStringABind() 
{
  delete [] _p;
}

void 
TStringABind::put_Value(VARIANT& Index, VARIANT& Value)
{
  // valida indice
  size_t pos = ValidateIndex(Index);
  // procesa el Nulo
  if (VT_NULL == V_VT(&Value))
    _apIndp[pos] = -1;
  else
    {
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
          lstrcpyn(_p + (pos * _length), ANSI(V_BSTR(&Value)), _length);
          //
          _apLen[pos] = (ub2)valueLength;
        }
      else
        {
          // convierte (si es necesario) y copia
          VARIANT v;
          VariantInit(&v); 
          CHECK_HRESULT(VariantChangeType(&v, &Value, 0, VT_BSTR));
          // asigna
          lstrcpyn(_p + (pos * _length), ANSI(V_BSTR(&v)), _length);
          //
          _apLen[pos] = (ub2)SysStringLen(V_BSTR(&v));
          //
          VariantClear(&v);
        }
      // declara tamaño
      //_apLen[pos] = (ub2)_length;
      // saca el nulo
      _apIndp[pos] = 0; 
    }
  // notifica
  _Param.SetDirtyFlag();
}

void 
TStringABind::get_Value(VARIANT& Index, VARIANT* retv)
{
  //
  CHECK_HRESULT(VariantClear(retv));
  //
  size_t pos = ValidateIndex(Index);
  //
  if (IsNull(pos))
    V_VT(retv) = VT_NULL;
  else
    {
      V_VT(retv)   = VT_BSTR;
      V_BSTR(retv) = WIDE( _p + (pos * _length) ).SysAllocString();
    }
}

/**
*** TGParam
***/
void 
TGParam::BindArray(dboVarType AsType, short ArraySize, VARIANT& StringLength)
{
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
      else
        RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_CANT_DETERMINE_STRING_SIZE); 
      //
      _apBind = new TStringABind(*this, ArraySize, length);  
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
          _apBind = new TTABind<VT_I2,   short,  SQLT_INT, short>(*this, ArraySize);  
          break; // Integer
        case VT_I4: 
          _apBind = new TTABind<VT_I4,   long,   SQLT_INT, long>(*this, ArraySize);   
          break; // Long
        case VT_R4: 
          _apBind = new TTABind<VT_R4,   float,  SQLT_FLT, float>(*this, ArraySize);  
          break; // Float
        case VT_R8: 
          _apBind = new TTABind<VT_R8,   double, SQLT_FLT, double>(*this, ArraySize); 
          break; // Double
        case VT_DATE:
          _apBind = new TTABind<VT_DATE, DATE,   SQLT_DAT, ODATE>(*this, ArraySize);  
          break; // DATE
          //  case VT_CY:
          //    _apBind = new TTABind<VT_CY,   CY,     SQLT_FLT, OCY>(*this, arraySize);
          //break; // CY
        default:
          RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_TYPE_MISMATCH_I, (int)AsType);
        }  
    }
}


