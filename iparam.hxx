#if !defined(_IPARAM_HXX_)
#define _IPARAM_HXX_ 

#include "icursor.hxx"

class IIParam : public Param
{
public:
  //virtual operator Cda_Def*()= 0; 
  //
  virtual long    UniqueID() const  = 0;
  virtual bool    IsAlfaN()  = 0;
  virtual string& Name()     = 0; // retorna el nombre del parametro
  virtual sword   Number()   = 0; // retorna la numero del parametro
  virtual void    PreWork()  = 0;
  virtual void    PosWork()  = 0;
  virtual bool    ZeroRef()  = 0;

  virtual void    Bind(dboVarType AsType, VARIANT& StringLength) = 0;
  virtual void    BindArray(dboVarType AsType, short ArraySize, VARIANT& StringLength) = 0;
  virtual void    ClearBind() = 0;
  virtual bool    IsStrict() = 0;
  //
  virtual void    Internal_put_Value(VARIANT& Index, VARIANT& Value) = 0;	
  //
  virtual ~IIParam() {}
};

// abstract factory del param genérico
extern void
CreateNumericParam(IICursor& cursor, sword Wich, AP<IIParam>& rapParam);
//
extern void
CreateStringParam(IICursor& cursor, string& Wich, AP<IIParam>& rapParam);
#endif //_IPARAM_HXX_


