/**
*** DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "param.hxx"

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
  //TGParam& _param;
  //
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
  // aloca los array's auxiliares
  _apIndp  = new sb2[nE];  
  _apLen   = new ub2[nE];  
  _apRC    = new ub2[nE]; 
  //
  //_curSize = nE;
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
#pragma message verificar poque no aparece el index en el <>
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
  //
  _curSize = nE;
  // 
  CHECK_OCI(::obndra(_Param, (text*)_Param.Name().c_str(), -1,
		     (ub1*)_ap.get(), (ub4)sizeof(sqlT), sqlt, -1,
		     _apIndp.get(), _apLen.get(), _apRC.get(),
		     (ub4)200/*nE*/, &_curSize,
		     (text*)0, -1, -1));
}

template <VARTYPE vt, class vT, sword sqlt, class sqlT>
void
TTABind<vt, vT, sqlt, sqlT>::PreWork()
{

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
      _apIndp[pos] = 0; // saca el nulo
      // convierte (si es necesario) y copia
      VARIANT v;
      VariantInit(&v); //   V_VT(&v) = vt;
      CHECK_HRESULT(VariantChangeType(&v, &Value, 0, vt));
      // asigna
      // _ap[pos] = static_cast<sqlT>(v.lVal);  _ap[pos] = *static_cast<sqlT*>(&(v.lVal));     
      _ap[pos] = *reinterpret_cast<sqlT*>(&(v.lVal));
      // declara tamaño
      _apLen[pos] = sizeof(sqlT);
      //
      VariantClear(&v);
    }
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
      (*(reinterpret_cast<vT*>(&retv->lVal))) = _ap[pos]; 
      //*static_cast<vT*>(&retv->lVal) = _ap[pos];
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
  char*  _p;
  size_t _nE;
  size_t _length;
  //
  ub4    _curSize;   // i/o
};

TStringABind::TStringABind(TGParam& param, size_t nE, size_t length)
  :  TABind(param, nE)
{
  _p  = new char[nE * (length + 1)];
  _nE      = nE;
  _curSize = nE;
  _length  = length + 1;
  //
  AP<char> p(_p);
  CHECK_OCI(::obndra(_Param, (text*)_Param.Name().c_str(), -1,
		     (ub1*)_p, (ub4)_length, SQLT_STR, -1,
		     _apIndp.get(), _apLen.get(), _apRC.get(),
		     (ub4)200/*nE*/, &_curSize,
		     (text*)0, -1, -1));
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
      _apIndp[pos] = 0; // saca el nulo
      // convierte (si es necesario) y copia
      VARIANT v;
      VariantInit(&v); 
      CHECK_HRESULT(VariantChangeType(&v, &Value, 0, VT_BSTR));
      // asigna
      lstrcpyn(_p + (pos * _length), ANSI(V_BSTR(&v)), _length);
      // declara tamaño
      _apLen[pos] = _length;
      //
      VariantClear(&v);
    }
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

/***********************************************************************************/


void 
TGParam::BindArray(dboVarType AsType, short ArraySize, VARIANT StringLength)
{
  switch (AsType)
    {
    case VT_UI1:// Byte 
    case VT_I2: 
      _apBind = new TTABind<VT_I2,   short,  SQLT_INT, short>(*this, ArraySize);  break; // Integer
    case VT_I4: 
      _apBind = new TTABind<VT_I4,   long,   SQLT_INT, long>(*this, ArraySize);   break; // Long
    case VT_R4: 
      _apBind = new TTABind<VT_R4,   float,  SQLT_FLT, float>(*this, ArraySize);  break; // Float
    case VT_R8: 
      _apBind = new TTABind<VT_R8,   double, SQLT_FLT, double>(*this, ArraySize); break; // Double
    case VT_DATE:
      _apBind = new TTABind<VT_DATE, DATE,   SQLT_DAT, ODATE>(*this, ArraySize);  break; // DATE
//  case VT_CY:
//    _apBind = new TTABind<VT_CY,   CY,     SQLT_FLT, OCY>(*this, arraySize);    break; // CY
    case VT_BSTR:
      {
      _apBind = new TStringABind(*this, ArraySize, 20);  
      }
      break; // BSTR
    default:
      RAISE_INTERNAL(DBO_E_RUNTIME_PARAM_TYPE_MISMATCH_I, (int)AsType);
    }  
}

