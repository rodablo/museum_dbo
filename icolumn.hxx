#if !defined(_ICOLUMN_HXX_)
#define _ICOLUMN_HXX_ 

#include "icursor.hxx"

class IIColumn : public Column
{
public:
  virtual string& Name() = 0;             // retorna el nombre de la columna
  virtual sword   Pos() = 0;              // retorna la numero de la columna

  virtual void    PreWork()         = 0;  // prepara todo para bajar los datos
  virtual void    PosWork()         = 0;  // conversiones y/o unlocks  
  virtual void    PosWorkPreRAISE() = 0;  // pre RAISE  

  virtual bool    ZeroRef() = 0;

  virtual ~IIColumn() {}

  // retorna una referencia al safearray en un VARIANT
  //virtual void    GetVectorRef(VARIANT& v) = 0;
  //virtual void Clear/Free();
  // operadores < == ???
  // IsNull(long indice)
  //
};

// interfaz para procesar los PieceWise
class IPWColumn
{
public:
  virtual void Prepare4NextPiece() = 0;
  virtual void DeliverPiece(IDispatch* pSink) = 0;
};



// abstract factory
extern bool 
CreateColumn(IICursor& cursor, sword pos, IIColumn*& rpColumn);

#endif //_ICOLUMN_HXX_

