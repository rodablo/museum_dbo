/**
***	DBO11(R)1997
*** rodablo@hotmail.com
***/
#ifndef	__PCH_H_
#define __PCH_H_

/**
*** opciones compilador
***/ 
//pragma option -d  // merge strings para las excepciones

/**
***
***/
#define WIN32
#define STRICT
#define _WIN32_WINNT 0x0400 

/**
*** INCLUDES
***/
#include <windows.h>
#include <windowsx.h>                 	// GET_WM_COMMAND_ID
//#include <olectl.h>                   // IClassFactory2
//#include <ocidl.h>			// IConnectionPointContainer, etc..
#include <olectl.h>			// IConnectionPointContainer, etc..
//#include <twinsys/string.h>	      // TString

/**
*** Oracle
*** Size of HDA area:
*** 256 for 32 bit arquitectures
***/
#define HDA_SIZE 256
extern "C"
	{
#include <ociapr.h>
	}

#endif
