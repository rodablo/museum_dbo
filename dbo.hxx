/**
*** DBO2(1997/1998)
*** rodablo@hotmail.com
***/
//#pragma option -v-
#pragma warning(disable : 4786) 
#pragma warning(disable : 4355) 

#include <memory>
#include <vector>
#include <map>
#include <string>        // usa el string del stl (no poner .h)
#include <algorithm>
using namespace std;
//#pragma option -v


//nclude "dborc.h"
#include "config.h"
#include "dbomc.hxx"
#include "dboidl.hxx"
#include "auxiliar.hxx"

/**
*** PROTOTIPOS, GLOBALES y CONSTANTES
***/
//extern HINSTANCE g_hInst;
//extern ULONG     g_cLock;
//extern ULONG     g_cObj;
extern char      g_szModuleName[];


/**
*** TSESSIONCF
***/
//extern IClassFactory* CreateSessionCF();
extern void CreateSessionCF(IClassFactory*& rpSCF);

/***
**** IMPLEMENTACIONES
****/
#include "dispatch.hxx"
#include "connectable.hxx"

/**
*** 	TDll
***/
class TServer 
{
public:
  void Lock() {  
    TCriticalSection::Lock lock(m_oCs);
    ++m_cLock;
  }
  void Unlock() {
    TCriticalSection::Lock lock(m_oCs);
    --m_cLock;
  }
  void IncrObjectsCount() {  
    TCriticalSection::Lock lock(m_oCs);
    ++m_cObj;
  }
  void DecrObjectsCount() {
    TCriticalSection::Lock lock(m_oCs);
    --m_cObj;
  }
  HRESULT DllCanUnloadNow() {
    TCriticalSection::Lock lock(m_oCs);
    return ResultFromScode(((0L == m_cObj && 0L == m_cLock)) ? S_OK : S_FALSE);
  }
  //
  operator HINSTANCE () const { return m_hInstance; }
  static TServer* TheInstance() { return m_pTheOnlyInstance; }
  //
private:
  friend BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID);
  TServer(HINSTANCE hinstance) : m_hInstance(hinstance), m_cLock(0), m_cObj(0) { m_pTheOnlyInstance = this; }
  //
private:
  TCriticalSection m_oCs;
  HINSTANCE 	   m_hInstance;
  volatile ULONG   m_cLock;
  volatile ULONG   m_cObj;
  //
  static TServer*  m_pTheOnlyInstance;
};













