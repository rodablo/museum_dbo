#if !defined(_ISESSION_HXX_)
#define _ISESSION_HXX_

class IISession : public _Session
{
public:
  virtual ULONG __stdcall CursorAddRef() = 0;
  virtual ULONG __stdcall CursorRelease() = 0;

  virtual Lda_Def* GetLDA() = 0;
  //
  virtual ~IISession(){}
};

// abstract factory
extern void 
CreateSession(IISession*& rpSession);

#endif // _ISESSION_HXX_
