#include <iostream.h>
#include <iomanip.h>
#define  WIN32
#define  STRICT
#define  _WIN32_WINNT 0x0400 
#include <windows.h>
#include <wincrypt.h>
#include <lm.h>         // for NetXxx API
#pragma  hdrstop
#include <string>
using namespace std;
#include "license.hxx"


// globales
TLicense2 g_lic;
bool g_fMensaje     = false;
bool g_fVencimiento = false;
bool g_fTrial       = false;
bool g_fDomain      = false;
bool g_fDomainSid   = false;

string g_mensaje;
string g_vencimiento;
string g_trial;
string g_domain;
string g_domainSid;

// prototipos
void FillLic();

/**
*** MAIN
***/
int 
main (int argc, char* argv[])
{
  try {
    // requiere opciones
    if (1 >= argc)
      throw string("missing data");
    // itera sobre las opciones
    for (int i = 1; i < argc; i++) {
      //
      if (!((i + 1) < argc))
	throw string("need arg");
      //
      if      (0 == strcmpi("msg", argv[i])) {
	g_mensaje = argv[++i];
      }
      else if (0 == strcmpi("venc", argv[i])) {
	g_vencimiento = argv[++i];  
      }
      else if (0 == strcmpi("trial", argv[i])) {
	g_trial = argv[++i];  
      }
      else if (0 == strcmpi("dom", argv[i])) {
	g_domain = argv[++i];  
      }
      else if (0 == strcmpi("domsid", argv[i])) {
	g_domainSid = argv[++i];  
      }
      else {
	string err = "opcion [" + string(argv[i]) + "] invalida";
	throw err;
      }
    }
    //
    //cout.width(2);
    //cout.fill('0');
    cout << g_mensaje << endl
	 << g_vencimiento << endl
	 << g_trial << endl
	 << g_domain << endl
	 << g_domainSid << endl;
  }
  catch (string str) {
    cerr << "\nerror: " << str << "..." << endl;
  }
}

/**
*** FILLLIC
***/
void
FillLic()
{
  // limpieza 
  ZeroMemory(&g_lic, sizeof(TLicense2));
  // mensaje
  string("Copyright(98)RODABLO").copy(g_lic.szLicenseInfoMsg, sizeof(g_lic.szLicenseInfoMsg));
}



#if __CREATELIC__

void  
CreateLic2()
{
  ::MessageBeep(-1);
  // INICIALIZA LA LICENCIA
  TLicense2 lic;
  ZeroMemory(&lic, sizeof(TLicense2));

  // MENSAJES
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
  st.wMonth        = 12; 
  st.wDayOfWeek    = 0; 
  st.wDay          = 7; 
  st.wHour         = 10; 
  st.wMinute       = 0; 
  st.wSecond       = 0; 
  st.wMilliseconds = 0;
  
  DATE venc = -1; // no funciona
  //   if (!SystemTimeToVariantTime(&st, &venc))
  //     return;
  
  // 7/04/98
  // 7/05/98

  // TRIAL
  st.wYear         = 1998;
  st.wMonth        = 10; 
  st.wDayOfWeek    = 0; 
  st.wDay          = 6; 
  st.wHour         = 10; 
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
  wstring             
  
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
      //      throw 0;

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

  //#define ENCRYPT_ALGORITHM   CALG_RC2
  //#define ENCRYPT_BLOCK_SIZE  8
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
      //        RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
      //                       "SOFTWARE\\Rodablo\\DBO2", 0,
      //                       HKEY hKey,     // handle of an open key 
      //                       LPCTSTR lpSubKey,      // address of subkey name 
      //                       DWORD Reserved,        // reserved 
      //                       LPTSTR lpClass,        // address of class string 
      //                       DWORD dwOptions,       // special options flag 
      //                       REGSAM samDesired,     // desired security access 
      //                       LPSECURITY_ATTRIBUTES lpSecurityAttributes,
      //                       PHKEY phkResult,       // address of buffer for opened handle  
      //                       LPDWORD lpdwDisposition        // address of disposition value buffer    );
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
