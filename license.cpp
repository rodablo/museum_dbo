/**
*** DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"

#include <wincrypt.h>
#include <lm.h>         // for NetXxx API

#define __CREATELIC__ 0
/*
\\RED_DESARROLLO

"DBO2 Developer"
"DBO2 User"

*/

WINOLEAUTAPI_(INT) SystemTimeToVariantTime(LPSYSTEMTIME lpSystemTime, double* pvtime);
//WINOLEAUTAPI_(INT) VariantTimeToSystemTime(double vtime, LPSYSTEMTIME lpSystemTime);

#if 0

n1 = f1 - f2

n2 = f1 + f2
---------------
n1 + n2 = f1 + f1 - f2 + f2

n1 + n2 = 2 f1

f1 = (n1 + n2) / 2 

f2 = n2 - f1

f2 = n2 - ((n1 + n2) / 2)
------------------------

  if (not in build range)
    deny now;

  if (have vencimiento)
    if (vencio)
      deny now;

  if (have trial)
    if (in trial)
      grant now;

  if (have domain)
    if (in domain)
      grant now;

  denied;

/*las licencias de origenes no requieren trial (luego de X) 
  eventualmente un modo de developer limitado podria funcionar sin vencimiento*/

#endif

class TLicense2
{
public:  
  // Informacion
  char          szLicenseInfoMsg[512];
  // Build
  long          lBuildStart;
  long          lBuildEnd;
  // Time
  DATE          dateEmision;
  DATE          dateN1;
  DATE          dateN2;
  // Domain
  char          szDomainName[256];
  DWORD         dwDomainSidSize;
  BYTE          byDomainSidData[128];
  // Reserved
  char          reserved1;
  char          reserved2[1024 - 1 - offsetof(TLicense2, reserved1)];
};

class TLicHdr
{
public:
  bool  fCanUseValidateCache;
  bool  fValidateCache;
  long  lSessionsC;
  DWORD dwBlockSize;
};

static char   szFMName[]          = "a1s2d3f4g5";
static HANDLE hFM                 = 0;
static char   szRegLicKey[]       = "SOFTWARE\\Rodablo\\DBO2";
static char   szRegLicValueName[] = "License";

static char   dummie1[] = 
{
  0x03,0x6c,0xb1,0x75,0x36,0x27,0x79,0xd9,
  0xe8,0xe1,0xb8,0xb3,0x86,0xa4,0xcf,0xac,
  0x93,0xaa,0x67,0xea,0xfa
};
static char   szPassword[] = 
{
  0x05,0x6c,0xb1,0x75,0x36,0x27,0x79,0xd9,
  0xe8,0x21,0xb8,0xe3,0x86,0xa4,0xcf,0xac,
  0x93,0xaa,0x77,0xea,0xfa
};
static char   dummie2[] = 
{
  0x05,0x6c,0xb1,0xf5,0x3e,0x27,0x79,0xd9,
  0x93,0xaa,0x77,0xea,0xfa
};


BSTR /*__stdcall*//*__pascal*/
LicenseInformation()
{
  //  return TString("...").RelinquishSysStr();
  return SysAllocString(WIDE("..."));
}

/**
*** A(), Inicializa la licencia 
***/
void __stdcall
LicLoad()
{
#if __CREATELIC__  
  void CreateLic2();
  CreateLic2();
#endif
  // intenta obtener el handle de la licencia cargada
  hFM = OpenFileMapping(/*FILE_MAP_READ*/FILE_MAP_WRITE/*PAGE_READWRITE*/, false, szFMName);
  // si no existe asume que es el primero y crea nuevo
  if (0 == hFM)
    {
      // lee el bloque de licencia de la registry
      HKEY   hRegKey = 0;
      PBYTE  pBlock = 0;
      PBYTE  pFM; 
      try 
	{ // abre el key
	  if (ERROR_SUCCESS != 
	      RegOpenKeyEx(HKEY_LOCAL_MACHINE, szRegLicKey, 0, KEY_READ, &hRegKey))
	    throw 0;
	  // determina el tamano del bloque
	  DWORD dwBlockSize;
	  if (ERROR_SUCCESS != 
	      RegQueryValueEx(hRegKey, szRegLicValueName, 0, 0, 0, &dwBlockSize))
	    throw 0;    
	  // aloca el buffer
	  pBlock = new BYTE[dwBlockSize];
	  // lee el bloque
	  DWORD dwType;
	  if (ERROR_SUCCESS != 
	      RegQueryValueEx(hRegKey, szRegLicValueName, 0, &dwType, pBlock, &dwBlockSize))
	    throw 0;   
	  // cierra el key
	  RegCloseKey(hRegKey);	  
	  hRegKey = 0;
	  // crea el bloque de mem compartida
	  hFM = CreateFileMapping((HANDLE)0xFFFFFFFF,            // use paging file
				  NULL,                         // no security attributes (por ahora)
				  PAGE_READWRITE,                // read/write access
				  0,                             // size: high 32-bits
				  sizeof(TLicHdr) + dwBlockSize, // size: low 32-bits
				  szFMName);                     // name of map object
	  if (0 == hFM) 
	    throw 0; 
	  // obtiene el puntero al file-mapped shared memory. 
	  pFM = (PBYTE)MapViewOfFile(hFM, FILE_MAP_WRITE, 0, 0, 0);    
	  if (0 == pFM)
	    throw 0;
	  // inicializa el header
	  ((TLicHdr*)pFM)->fCanUseValidateCache = false;
	  ((TLicHdr*)pFM)->fValidateCache = false;
	  ((TLicHdr*)pFM)->lSessionsC  = 0;	   
	  ((TLicHdr*)pFM)->dwBlockSize = dwBlockSize;
	  // copia el bloque
	  CopyMemory(pFM + sizeof(TLicHdr), pBlock, dwBlockSize);
	  // libera 
	  delete [] pBlock;
	  pBlock = 0;
	  UnmapViewOfFile(pFM);
	  pFM = 0;
	}
      catch (...)
	{ 
	  // algo fallo libera todo
	  delete [] pBlock;
	  if (0 != pFM) 
	    UnmapViewOfFile(pFM);	  
	  if (0 != hFM) 
	    { 
	      CloseHandle(hFM); 
	      hFM = 0;
	    }
	  RegCloseKey(hRegKey);	  
	}
    }
}

void __stdcall
LicFree()
{
  if (0 != hFM)
    {
      CloseHandle(hFM); 
      hFM = 0;
    }
}

bool 
LicValidate()
{
  // hay licencia?
  if (0 == hFM)
    return false;
  //
  bool       fLicValida = false;
  PBYTE      pFM        = 0;
  PBYTE      pTempLic   = 0;

  LPWSTR              pDC   = 0;
  PUSER_MODALS_INFO_2 pUmi2 = 0;

  HCRYPTPROV hProv = 0;
  HCRYPTKEY  hKey  = 0;
  HCRYPTHASH hHash = 0;

  //
  try
    {
      // obtiene el puntero a la licencia
      pFM = (PBYTE)MapViewOfFile(hFM,FILE_MAP_WRITE/* FILE_MAP_READ*/, 0, 0, 0);
      if (0 == pFM)
	throw 0;
      // decifra
      // Get handle to the default provider.
      if(!CryptAcquireContext(&hProv, 0, 0, /*PROV_DSS*/PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) 
	throw 0;
      // Crea hash, hashea el password, y deriva el key
      if(!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) 
	throw 0;
      if(!CryptHashData(hHash, (PBYTE)szPassword, sizeof(szPassword)/*strlen(szPassword)*/, 0)) 
	throw 0;
      if(!CryptDeriveKey(hProv, CALG_RC2, hHash, 0, &hKey)) 
	throw 0;
      // Destruye el hash.
      CryptDestroyHash(hHash);
      hHash = 0;
      // aloca bufer para version decifrada, y copia el buffer
      DWORD dwSize = ((TLicHdr*)pFM)->dwBlockSize;
      pTempLic = new BYTE[dwSize];
      CopyMemory(pTempLic, pFM + sizeof(TLicHdr), dwSize);
      //memcpy(pTempLic, (pFM + sizeof(TLicHdr)), dwSize);
      // decifra
      if(!CryptDecrypt(hKey, 0, true, 0, pTempLic, &dwSize)) 
	throw 0;
      //
      //
      //
      // AQUI COMIENZA LA DECISION 

      SYSTEMTIME st;
      DATE       dateNow;
      TLicense2* pL = (TLicense2*)pTempLic;

#define VENC  (DATE)((pL->dateN1 + pL->dateN2) / 2)
#define TRIAL (DATE)(pL->dateN2 - VENC)

      GetLocalTime(&st);
      
      if (!SystemTimeToVariantTime(&st, &dateNow))
	throw 0;

      // VENCIMIENTO FORZADO
      if (0.0 < VENC)
	if (dateNow < VENC)
	  // deny now!
	  throw 0; 
       
      // TRIAL
     if (0.0 < TRIAL)
       if (dateNow < TRIAL)
	 fLicValida = true;

     // DOMAIN
     if (!fLicValida && 0 != pL->szDomainName[0])
       {
	 NET_API_STATUS      nas;
	 // obtain the PDC computer name of the supplied domain name
	 nas = NetGetDCName(NULL, WIDE(pL->szDomainName), (LPBYTE*)&pDC);
	 //nas = NetGetDCName(NULL, TString(pL->szDomainName), (LPBYTE*)&pDC);
	 if(nas != NERR_Success) 
	   throw 0;
	 
	 nas = NetUserModalsGet(pDC, 2, (LPBYTE *)&pUmi2);
	 if(nas != NERR_Success) 
	   throw 0;
	 
	 if(!IsValidSid((PSID)pL->byDomainSidData))
	   throw 0;

	 if (EqualSid(pUmi2->usrmod2_domain_id, (PSID)pL->byDomainSidData))
	   fLicValida = true;
       }

     // DENY
     
     // if (dateNow < ((TLicense*)pTempLic)->dateVencimiento)
     //fLicValida = true;

      // AQUI TERMINO LA DECISION
      //
      //
      //
    }
  catch (...)
    {
      // algo fallo REPROBO!
      fLicValida = false; // realmente innecesario
    }

  // liberacion de recursos
  delete [] pTempLic;
  // libera el view
  UnmapViewOfFile(pFM);    
  // Domain
  if (0 != pDC) 
    NetApiBufferFree(pDC);
  if (0 != pUmi2)
    NetApiBufferFree(pUmi2);
  // Destruye session key.
  if(0 != hKey) 
    CryptDestroyKey(hKey);
  // Destruye el hash
  if(0 != hHash) 
    CryptDestroyHash(hHash);
  // Release provider handle.
  if(0 != hProv) 
    CryptReleaseContext(hProv, 0);
  // retorna el resultado de la validacion
  return fLicValida;
}

bool
LicFastValidate()
{
  return true;
}

#if __CREATELIC__

void  
CreateLic2()
{
  // INICIALIZA LA LICENCIA
  TLicense2 lic;
  ZeroMemory(&lic, sizeof(TLicense2));

  // MENSAGES
  lstrcpy(lic.szLicenseInfoMsg, "Copyright(98)RODABLO");

  // VERSION
  lic.lBuildStart = 0;
  lic.lBuildEnd   = 1;

  // TIME
  SYSTEMTIME st;

  // EMISION
  GetSystemTime(&st);
  if (!SystemTimeToVariantTime(&st, &lic.dateEmision))
    return;

  // VENCIMIENTO
  st.wYear         = 1998;
  st.wMonth        = 9; 
  st.wDayOfWeek    = 0; 
  st.wDay          = 6; 
  st.wHour         = 14; 
  st.wMinute       = 0; 
  st.wSecond       = 0; 
  st.wMilliseconds = 0;
  
  DATE venc = -1;
  //  if (!SystemTimeToVariantTime(&st, &venc))
  //    return;
  
  // 7/04/98
  // 7/05/98

  // TRIAL
  st.wYear         = 1998;
  st.wMonth        = 7; 
  st.wDayOfWeek    = 0; 
  st.wDay          = 7; 
  st.wHour         = 14; 
  st.wMinute       = 0; 
  st.wSecond       = 0; 
  st.wMilliseconds = 0;
  
  DATE trial;
  if (!SystemTimeToVariantTime(&st, &trial))
    return;
  
  // 
  lic.dateN1 = venc - trial;
  lic.dateN2 = venc + trial;

  // DOMINIO
  NET_API_STATUS      nas;
  LPWSTR              pDC;
  PUSER_MODALS_INFO_2 pUmi2 = 0;
  string              sDomName("RED_ORIGENES");
  
  bool fError = false;
  try
    {
      // copia el nombre del dominio
      sDomName.copy(lic.szDomainName, sizeof(lic.szDomainName));

      // obtiene el PDC computer name del dominio
      nas = NetGetDCName(NULL, WIDE(sDomName), (LPBYTE*)&pDC);
      if(nas != NERR_Success) 
	throw 0;//SetLastError(nas);
  
      nas = NetUserModalsGet(pDC, 2, (LPBYTE *)&pUmi2);
      if(nas != NERR_Success) 
	throw 0;
  
      // if the Sid is valid, obtain the size of the Sid
      if(!IsValidSid(pUmi2->usrmod2_domain_id))
	throw 0;

      lic.dwDomainSidSize = GetLengthSid(pUmi2->usrmod2_domain_id);

      if (sizeof(lic.byDomainSidData) < lic.dwDomainSidSize)
	throw 0;

//    if(!CopySid(sizeof(lic.byDomainSidData), lic.byDomainSidData, pUmi2->usrmod2_domain_id))    
// 	throw 0;

      CopyMemory(lic.byDomainSidData, pUmi2->usrmod2_domain_id, lic.dwDomainSidSize);
    }
  catch (...)
    {
      fError = true;
    }

  NetApiBufferFree(pDC);

  if(0 != pUmi2)
    NetApiBufferFree(pUmi2);

  if (fError)
     return;

  //
  // ENCRIPCION
  //
  PBYTE pBlock = 0;
  DWORD dwBlockSize = 0;

  HCRYPTPROV hProv = 0;
  HCRYPTKEY  hKey  = 0;
  HCRYPTHASH hHash = 0;

  //#define ENCRYPT_ALGORITHM	CALG_RC2
  //#define ENCRYPT_BLOCK_SIZE	8
  // 
  fError = false;
  try
    {
      // Get handle to the default provider.
      if(!CryptAcquireContext(&hProv, 0, 0, /*PROV_DSS*/ PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) 
	throw 0;
      // Create a hash object.
      if(!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) 
	throw 0;
      // Hash in the password data.
      if(!CryptHashData(hHash, (PBYTE)szPassword, sizeof(szPassword)/*strlen(szPassword)*/, 0)) 
	throw 0;
      // Derive a session key from the hash object.
      if(!CryptDeriveKey(hProv, CALG_RC2/*ENCRYPT_ALGORITHM*/, hHash, 0, &hKey)) 
	throw 0;
      // Destroy the hash object.
      CryptDestroyHash(hHash);
      hHash = 0;
      // crea un buffer mas grande paque sobre
      pBlock = new BYTE[1500];
      //
      CopyMemory(pBlock, &lic, sizeof(lic)); 
      dwBlockSize = sizeof(lic);
    
      // Encrypt data
      if(!CryptEncrypt(hKey, 0, true, 0, pBlock, &dwBlockSize, 1500))
	throw 0;
    }
  catch (...)
    {
      delete [] pBlock;
      fError = true;
    }
  
  // libera recursos
  if (0 != hKey)  CryptDestroyKey(hKey);
  if (0 != hHash) CryptDestroyHash(hHash);
  if (0 != hProv) CryptReleaseContext(hProv, 0);

  // si fue error chau
  if (fError)
    return;

  //
  // REGISTRY
  //
  HKEY hRegKey = 0;
  //  
  try 
    { // abre
//       if (ERROR_SUCCESS !=      
// 	  RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
// 			 "SOFTWARE\\Rodablo\\DBO2", 0,
// 			 HKEY hKey,	// handle of an open key 
// 			 LPCTSTR lpSubKey,	// address of subkey name 
// 			 DWORD Reserved,	// reserved 
// 			 LPTSTR lpClass,	// address of class string 
// 			 DWORD dwOptions,	// special options flag 
// 			 REGSAM samDesired,	// desired security access 
// 			 LPSECURITY_ATTRIBUTES lpSecurityAttributes,
// 			 PHKEY phkResult,	// address of buffer for opened handle  
// 			 LPDWORD lpdwDisposition 	// address of disposition value buffer    );
      if (ERROR_SUCCESS !=      
	  RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Rodablo\\DBO2", 0, KEY_WRITE, &hRegKey))
	throw 0;
      // graba  
      if (ERROR_SUCCESS !=      
	  RegSetValueEx(hRegKey, szRegLicValueName, 0, REG_BINARY, pBlock, dwBlockSize))
	throw 0;
      // 
      delete[] pBlock; 
      pBlock = 0;
      // flush  
      if (ERROR_SUCCESS != 
	  RegFlushKey(hRegKey))
	throw 0;
    }
  catch (...)
    {
    } 
  //
  if (0 != pBlock)
    delete [] pBlock;
  //
  RegCloseKey(hRegKey);
}

#endif
