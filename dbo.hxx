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
extern HINSTANCE g_hInst;
extern ULONG     g_cLock;
extern ULONG     g_cObj;
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














