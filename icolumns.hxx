#if !defined(_ICOLUMNS_HXX_)
#define _ICOLUMNS_HXX_

#include "icursor.hxx"

class IIColumns : public Columns
{
public:
  virtual ~IIColumns() {}  
};

// abstract factory
extern void 
CreateColumns(IICursor& iicursor, IIColumns*& rpColumns);  

#endif // _ICOLUMNS_HXX_

