#if !defined(_IPARAM_HXX_)
#define _IPARAM_HXX_ 

#include "icursor.hxx"

class IIParam : public Param
{
public:
  //virtual operator Cda_Def*()= 0; 
  //
  virtual long    UniqueID() const  = 0;
  virtual string& Name()     = 0; // retorna el nombre del parametro
  virtual sword   Number()   = 0; // retorna la numero del parametro
  virtual void    PreWork()  = 0;
  virtual void    PosWork()  = 0;

  virtual void    _Bind()    = 0; // borrar
  virtual bool    ZeroRef()  = 0;

  virtual void    BindArray(dboVarType AsType, short ArraySize, VARIANT StringLength) = 0;
  virtual void    ClearBind() = 0;
  //
  virtual ~IIParam() {}
};

// esto aqui esta mal!
// bool
// ValidateWichParam(VARIANT& rWich, sword* pNumber);
 
// abstract factory (volar los parametros del bind)
extern void
_CreateParam(IICursor& cursor, VARIANT& Wich, VARIANT& Value, VARIANT& AsType, VARIANT& Length,
	    AP<IIParam>& rapParam);

// abstract factory del param genérico
extern void
CreateParam(IICursor& cursor, VARIANT& Wich, AP<IIParam>& rapParam);

extern void
CreateParam(IICursor& cursor, string& Wich, AP<IIParam>& rapParam);
#endif //_IPARAM_HXX_
