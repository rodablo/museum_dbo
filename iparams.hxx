#if !defined(_IPARAMS_HXX_)
#define _IPARAMS_HXX_

#include "icursor.hxx"

class IIParams : public Params
{
public:
  virtual ~IIParams() {}  
};

// abstract factory
extern void 
CreateParams(IICursor& iicursor, IIParams*& rpParams);  

#endif // _IPARAMS_HXX_

