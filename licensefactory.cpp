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
WINOLEAUTAPI_(INT) SystemTimeToVariantTime(LPSYSTEMTIME lpSystemTime, double* pvtime);

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

// globales
TLicense2 g_lic;
PBYTE     g_pBlock = 0;
DWORD     g_dwBlockSize = 0;
static char   g_szPassword[] = 
{
  0x05,0x6c,0xb1,0x75,0x36,0x27,0x79,0xd9,
  0xe8,0x21,0xb8,0xe3,0x86,0xa4,0xcf,0xac,
  0x93,0xaa,0x77,0xea,0xfa
};

//
WORD g_venc_dia,  g_venc_mes,  g_venc_anio;
WORD g_trial_dia, g_trial_mes, g_trial_anio;

string g_trial;

string g_domain;
string g_domainSid;
string g_mensaje;

// prototipos
void FillLic();
void PrintHeader();
void Encripta();
void Dump();
WORD GetWORDfromEnv(char* var);

/**
*** MAIN
***/
int 
main (int argc, char* /*argv*/[])
{
  try {
    // no arg's 
    if (1 != argc)
      throw string("missing data");

    // vencimientos
    g_venc_dia   = GetWORDfromEnv("L_VENC_DIA");
    g_venc_mes   = GetWORDfromEnv("L_VENC_MES");
    g_venc_anio  = GetWORDfromEnv("L_VENC_ANIO");
    g_trial_dia  = GetWORDfromEnv("L_TRIAL_DIA");
    g_trial_mes  = GetWORDfromEnv("L_TRIAL_MES");
    g_trial_anio = GetWORDfromEnv("L_TRIAL_ANIO");

    //
    char* p;

    if (0 == (p = getenv("L_DOMAIN")))
      throw string( "variable L_DOMAIN indefinida");
    else
      g_domain = p;

    if (0 == (p = getenv("L_DOMAINSID")))
      throw string( "variable L_DOMAINSID indefinida");
    else
      g_domainSid = p;

    if (0 == (p = getenv("L_MENSAJE"))) 
      throw string( "variable L_MENSAJE indefinida");
    else
      g_mensaje = p;
    //
    FillLic();
    PrintHeader();
    Encripta();
    Dump();

    //
    //cout.width(2);
    //cout.fill('0');

    cout << g_mensaje << endl
	 << g_venc_dia << endl 
	 << g_trial_dia << endl
	 << g_domain << endl
	 << g_domainSid << endl;

  }
  catch (string str) {
    cerr << "\nerror: " << str << "..." << endl;
  }
  catch (...) {
    cerr << "\nbig problem\n" ;
  }
    
}

/**
***
***/
WORD 
GetWORDfromEnv(char* var)
{
  char* pvalue = getenv(var);
  //
  if (0 != pvalue) {
    WORD w = atoi(pvalue);
    //
    if (0 != w)
      return w;
  }
  //		
  string err = "variable[" + string(var) + "] indefinida";
  throw err;
}

/**
*** FILLLIC
***/
void
FillLic()
{
  // limpieza 
  ZeroMemory(&g_lic, sizeof(TLicense2));
  
  // MENSAJE
  if ("*" != g_mensaje)
    g_mensaje.copy(g_lic.szLicenseInfoMsg, sizeof(g_lic.szLicenseInfoMsg));

  // PLAZOS
  SYSTEMTIME st;
  // emision
  GetSystemTime(&st);
  if (!SystemTimeToVariantTime(&st, &g_lic.dateEmision))
    return;
  // vencimiento
  DATE venc = -1; // no funciona
  if ((WORD)-1 != g_venc_dia)
    {
      st.wYear      = g_venc_anio;
      st.wMonth     = g_venc_mes; 
      st.wDayOfWeek = 0; 
      st.wDay       = g_venc_dia; 
      st.wHour      = 10; 
      st.wMinute    = st.wSecond = st.wMilliseconds = 0;
      if (!SystemTimeToVariantTime(&st, &venc))
	throw string("error en SystemTimeToVariantTime");
    }
  // trial
  DATE trial = -1;
  if ((WORD)-1 != g_trial_dia)
    {
      st.wYear      = g_trial_anio;
      st.wMonth     = g_trial_mes; 
      st.wDayOfWeek = 0; 
      st.wDay       = g_trial_dia; 
      st.wHour      = 10; 
      st.wMinute    = st.wSecond = st.wMilliseconds = 0;
      if (!SystemTimeToVariantTime(&st, &trial))
	throw string("error en SystemTimeToVariantTime");
    }
  // COMBINA
  g_lic.dateN1 = venc - trial;
  g_lic.dateN2 = venc + trial;

  // DOMINIO
  if ("*" != g_domain)
    {
      NET_API_STATUS      nas;
      LPWSTR              pDC;
      PUSER_MODALS_INFO_2 pUmi2 = 0;

      // copia el nombre del dominio
      g_domain.copy(g_lic.szDomainName, sizeof(g_lic.szDomainName));

      // obtiene el PDC computer name del dominio (ver getanydc)
      nas = NetGetAnyDCName(NULL, WIDE(g_domain), (LPBYTE*)&pDC);
      if(nas != NERR_Success) 
        throw string("NetGetDCName");
  
      nas = NetUserModalsGet(pDC, 2, (LPBYTE *)&pUmi2);
      if(nas != NERR_Success) 
        throw string("NetUserModalsGet");
  
      // if the Sid is valid, obtain the size of the Sid
      if(!IsValidSid(pUmi2->usrmod2_domain_id))
        throw string("IsValidSid");

      g_lic.dwDomainSidSize = GetLengthSid(pUmi2->usrmod2_domain_id);

      if (sizeof(g_lic.byDomainSidData) < g_lic.dwDomainSidSize)
        throw string("(sizeof(g_lic.byDomainSidData) < g_lic.dwDomainSidSize)");

      //    if(!CopySid(sizeof(g_lic.byDomainSidData), g_lic.byDomainSidData, pUmi2->usrmod2_domain_id))    
      //      throw 0;
      CopyMemory(g_lic.byDomainSidData, pUmi2->usrmod2_domain_id, g_lic.dwDomainSidSize);

      NetApiBufferFree(pDC);	
      
      if(0 != pUmi2)
	NetApiBufferFree(pUmi2);
    }
}

/**
***
***/
void
Encripta()
{
  HCRYPTPROV hProv = 0;
  HCRYPTKEY  hKey  = 0;
  HCRYPTHASH hHash = 0;

  //#define ENCRYPT_ALGORITHM   CALG_RC2
  //#define ENCRYPT_BLOCK_SIZE  8
  // 
  // Get handle to the default provider.
  if(!CryptAcquireContext(&hProv, 0, 0, /*PROV_DSS*/ PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) 
    throw string("CryptAcquireContext");
  // Create a hash object.
  if(!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) 
    throw string("CryptCreateHash");
  // Hash in the password data.
  if(!CryptHashData(hHash, (PBYTE)g_szPassword, sizeof(g_szPassword)/*strlen(szPassword)*/, 0)) 
    throw string("CryptHashData");
  // Derive a session key from the hash object.
  if(!CryptDeriveKey(hProv, CALG_RC2/*ENCRYPT_ALGORITHM*/, hHash, 0, &hKey)) 
    throw string("CryptDeriveKey");
  // Destroy the hash object.
  CryptDestroyHash(hHash);
  hHash = 0;
  // crea un buffer mas grande paque sobre
  g_pBlock = new BYTE[1500];
  //
  CopyMemory(g_pBlock, &g_lic, sizeof(g_lic)); 
  g_dwBlockSize = sizeof(g_lic);
  // Encrypt data
  if(!CryptEncrypt(hKey, 0, true, 0, g_pBlock, &g_dwBlockSize, 1500))
    throw string("CryptEncrypt");

  // libera recursos
  if (0 != hKey)  CryptDestroyKey(hKey);
  if (0 != hHash) CryptDestroyHash(hHash);
  if (0 != hProv) CryptReleaseContext(hProv, 0);
}

void 
PrintHeader()
{
}

void 
Dump()
{
}


