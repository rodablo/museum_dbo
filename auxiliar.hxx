#if !defined(_AUXSTRING_HXX_)
#define _AUXSTRING_HXX_ 

/**
***
***/

#define _PEND_ 0  
//#define PEND(msg)


/**
*** NEW/DELETE
***/
extern "C" {
// // void* __trackmalloc__(size_t size);
// // void* __trackrealloc__(void *block, size_t size);
// // void  __trackfree__(void *block);

//   void* malloc(size_t size);
//   void* realloc(void *block, size_t size);
//   void  free(void *block);
}

inline void*
operator new(size_t size) {
  return ::HeapAlloc(::GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, size);
  //return malloc(size);
  //return _trackmalloc__(size);
}

inline void*
operator new[](size_t size) {
  return ::HeapAlloc(::GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, size);
  //  return malloc(size);
  // return _trackmalloc__(size);
}

inline void
operator delete(void* ptr/*,[size_t Type_size]*/) {
  ::HeapFree(::GetProcessHeap(), /*HEAP_NO_SERIALIZE*/0, ptr);
  //free(ptr);
}

inline void
operator delete[](void* ptr/*,[size_t Type_size]*/) {
  ::HeapFree(::GetProcessHeap(), /*HEAP_NO_SERIALIZE*/0, ptr);
  //free(ptr);
}

/**
*** AP
***/
template<class T> class AP
{
public:
  explicit AP(T* p = 0) : the_p(p) {}
  AP(AP<T>& other) : the_p(other.release()) {}
  AP(const AP<T>& other) : the_p(const_cast<AP<T>&>(other).release()) {}
  ~AP() {delete the_p;}
  //
  AP& operator= (T* p) { the_p = p; return *this; }
  AP& operator= (AP<T>& other) { reset(other.release()); return *this; }
  AP& operator= (const AP<T>& other) { reset(const_cast<AP<T>&>(other).release()); return *this; }
  //
  T&   operator[] (const size_t pos) const { return the_p[pos]; }
  T&   operator*  ()        const { return *the_p;   }
  T*   operator-> ()        const { return the_p;    }
  T*   get        ()        const { return the_p;    }
  T*   release    ()              { return reset(0); }
  T*   reset      (T* p = 0)      { T* tmp = the_p; the_p = p; return tmp; }
  bool is_empty   ()        const { return 0 == the_p; }      

  //  bool operator<  (const AP<T>& other) const // ver que pasa si estan en 0
  //                                { return *the_p < *other.the_p; }

private:
  T* the_p;
};

/**
*** ANSI/WIDE
***/
class ANSI
{
public:
  ANSI(const wchar_t* src);
  ~ANSI() {if (0 != _p) delete _p;}

  //operator string
  operator const char*() const {return _p;}
  operator char*() {return _p;}
  const char* to_upper() {::CharUpperBuff(_p, lstrlen(_p)); return _p;}

private:
  char* _p;

private:
  ANSI(ANSI& other) {}
  operator = (ANSI& src) {}
  void* operator new(size_t size)   {return 0;}; // no new, no delete...
  void* operator new[](size_t size) {return 0;};
  void  operator delete(void* ptr)  {}
  void  operator delete[](void* ptr){}
};

inline
ANSI::ANSI(const wchar_t* src)
{
  // calcula largo
  size_t size = WideCharToMultiByte(CP_ACP, 0, src, -1, 0, 0, 0, 0);
  // aloca incluyendo uno mas para el 0
  _p = new char[size + 1]; 
  size = WideCharToMultiByte(CP_ACP, 0, src, -1, _p, size, 0, 0);
  _p[size] = 0;
}

/**
*** WIDE
***/
class WIDE
{
public:
  WIDE(string& src)     {this->operator= (src.c_str());}
  WIDE(const char* src) {this->operator= (src);}
  ~WIDE() {if (0 != _p) delete _p;}

  operator const wchar_t*() const {return _p;}
  operator wchar_t*() {return _p;}

  BSTR SysAllocString() {return ::SysAllocString(_p);}

protected:
  void operator = (const char* src);

private:
  wchar_t* _p;

private:
  WIDE(WIDE& other) {}
  operator = (WIDE& src) {}
  void* operator new(size_t size)   {return 0;}; // no new, no delete...
  void* operator new[](size_t size) {return 0;};
  void  operator delete(void* ptr)  {}
  void  operator delete[](void* ptr){}
};

void inline
WIDE::operator= (const char* src)
{
  // calcula largo
  size_t size = MultiByteToWideChar(CP_ACP, 0, src, -1, 0, 0);
  // aloca incluyendo uno mas para el 0
  _p = new wchar_t[size + 1]; 
  size = MultiByteToWideChar(CP_ACP, 0, src, -1, _p, size);
  _p[size] = 0;
}

/**
*** Oracle Internal DATE
***/
class ODATE
{
public:
  ODATE();
  ODATE(const DATE& date);
  //
  ODATE& operator = (const DATE& date);
  operator DATE ();
  //
protected:
  ub1 _Century;
  ub1 _Year;
  ub1 _Month;
  ub1 _Day; 
  ub1 _Hour;
  ub1 _Minute;
  ub1 _Second;
};

/**
*** VAR
***/
class VAR
{
public:
  explicit VAR() : _pV(0)   {}
  VAR(VARIANT& v) : _pV(&v) {}
  ~VAR()                    {}
  operator VARTYPE()  {return V_VT(_pV);}
  operator VARIANT*() {return _pV;}
  operator long();
  operator int();
  operator short();
  operator BSTR();
  
  bool IsBSTR()       {return VT_BSTR == V_VT(_pV);}
  
private:
  VARIANT* _pV;
};


bool
GetShortFromVariant(short& dest, VARIANT& src);





#endif //_AUXSTRING_HXX_ 

