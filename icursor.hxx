#if !defined(_ICURSOR_HXX_)
#define _ICURSOR_HXX_

#include "isession.hxx"

class IIColumn;
class IIParam;

class IICursor : public ICURSOR
{
public:
  //
  virtual operator Cda_Def*() = 0;  //  virtual operator Lda_Def*() = 0;
  //
  virtual Cda_Def*  GetCDA() = 0;
  virtual Lda_Def*  GetLDA() = 0;
  virtual ULONG     GetRowsXFetch() = 0;
  virtual ULONG     GetCount() = 0;
  virtual IIColumn* GetColumn(int index) = 0;
  virtual IIColumn* GetColumn(string& name) = 0;
  virtual int       GetColumnCount() = 0;
  virtual IIParam*  GetParam(sword Number) = 0;
  virtual IIParam*  GetParam(string& name) = 0;
  virtual int       GetParamCount() = 0;
  virtual void      SetDirtyFlag() = 0;
  virtual bool      IsStrict() = 0;
  //
  virtual ~IICursor(){}
};

// abstract factory
extern void 
CreateCursor(IISession& session, IICursor*& rpCursor);  

#endif // _ICURSOR_HXX_

