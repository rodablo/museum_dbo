/**
*** DBO2X(R)1996-2001
*** rodablo@rodablo.com
***/
//#include <oci.h>
#include "pch.hxx"
#include "dbo.hxx"
#if 0
typedef sb4 TOci8Ex;
inline void CHECK_OCI8(sword stat) { if (OCI_SUCCESS != stat) throw TOci8Ex(stat); }

HRESULT __stdcall PasswordChange(/* [in] */ BSTR User,
				 /* [in] */ BSTR OldPassword,
				 /* [in] */ BSTR NewPassword,
				 /* [in] */ BSTR Alias)
{
  string user = ANSI(User);
  string oldPassword = ANSI(OldPassword);
  string newPassword = ANSI(NewPassword);
  string alias = ANSI(Alias);

  HRESULT retv = NOERROR;

  OCIEnv*    	envhp = 0;
  OCIError*  	errhp = 0;
  OCIServer* 	srvhp = 0;
  OCISvcCtx*	svchp = 0;
  OCISession*	usrhp = 0;

  try {
    //
    CHECK_OCI8(OCIInitialize( (ub4)/*OCI_DEFAULT*/OCI_OBJECT, (dvoid *)0,
			      (dvoid * (*)(dvoid *, size_t)) 0,
			      (dvoid * (*)(dvoid *, dvoid *, size_t))0,
			      (void (*)(dvoid *, dvoid *)) 0 ) );

    // Allocate and initialise the environment handle - no need to call
    // call OCIHandleAlloc first.
    CHECK_OCI8(OCIEnvInit( (OCIEnv **) &envhp, OCI_DEFAULT, (size_t) 0, 
			   (dvoid **) 0 ) );

    // Allocate the error handle for all error handling.
    CHECK_OCI8(OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &errhp, OCI_HTYPE_ERROR, 
			       (size_t) 0, (dvoid **) 0) );

    // Allocate the server handle and create the access path by 
    // attaching.  Note no connect string alias is being passed in the
    // attach call as we are using a local connection.
    CHECK_OCI8(OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &srvhp, OCI_HTYPE_SERVER,
    			       (size_t) 0, (dvoid **) 0) );
    //
    CHECK_OCI8(OCIServerAttach(srvhp, errhp, 
    			       (text *)0/*alias.c_str()*/, 0/*alias.length()*/, 0) );
   
    // Allocate the service context handle and set the server attribute to
    // the server handle allocated above.
    CHECK_OCI8(OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &svchp, OCI_HTYPE_SVCCTX,
			       (size_t) 0, (dvoid **) 0) );

    // set attribute server context in the service context 
    CHECK_OCI8(OCIAttrSet( (dvoid *) svchp, OCI_HTYPE_SVCCTX, 
			   (dvoid *) srvhp, (ub4) 0, OCI_ATTR_SERVER, 
			   errhp) );


    // Allocate the session handle and add it to the session attribute of 
    // the service context.  Note, no username and password are added to 
    // the session handle since this will be done as part of the 
    // OCIChangePassword call.
    CHECK_OCI8(OCIHandleAlloc((dvoid *) envhp, (dvoid **)&usrhp, 
			      (ub4) OCI_HTYPE_SESSION, 
			      (size_t) 0, (dvoid **) 0) );

    CHECK_OCI8(OCIAttrSet((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX,
  			  (dvoid *) usrhp, (ub4) 0,
  			  (ub4) OCI_ATTR_SESSION, errhp));
    //
    CHECK_OCI8(OCIPasswordChange(svchp, errhp,
				 (CONST text*)user.c_str(),        
				 user.length(),
				 (CONST text*)oldPassword.c_str(), 
				 oldPassword.length(),
				 (CONST text*)newPassword.c_str(), 
				 newPassword.length(),
				 OCI_DEFAULT | OCI_AUTH) );
    //
    CHECK_OCI8(OCISessionEnd(svchp, errhp, usrhp, OCI_DEFAULT));
    //
    CHECK_OCI8(OCIServerDetach(srvhp, errhp, OCI_DEFAULT));
  }
  catch(TOci8Ex stat) {	
    text errbuf[512];
    text*  errmsg = 0; 
    sb4 errcode = 0;
    //
    switch (stat)
      {
      case OCI_SUCCESS:
	break;
      case OCI_SUCCESS_WITH_INFO:
	errmsg = (text*)"Error - OCI_SUCCESS_WITH_INFO";
	break;
      case OCI_NEED_DATA:
	errmsg = (text*)"Error - OCI_NEED_DATA";
	break;
      case OCI_NO_DATA:
	errmsg = (text*)"Error - OCI_NODATA";
	break;
      case OCI_ERROR:
	(void) OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode,
			   errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
	errmsg = (text*)errbuf;
	break;
      case OCI_INVALID_HANDLE:
	errmsg = (text*)"Error - OCI_INVALID_HANDLE";
	break;
      case OCI_STILL_EXECUTING:
	errmsg = (text*)"Error - OCI_STILL_EXECUTE";
	break;
      case OCI_CONTINUE:
	errmsg = (text*)"Error - OCI_CONTINUE";
	break;
      default:
	errmsg = (text*)"Unknow OCI Error";
	break;
      }  
    // retorna un error
    retv = DBO_E_ORACLE;
#if 0
    // crea una excepcion
    EXCEPINFO excepInfo;
    excepInfo.bstrDescription = WIDE((char*)errmsg).SysAllocString();
    excepInfo.bstrSource = WIDE("DBO22.PasswordChange").SysAllocString();
    excepInfo.scode = GetScode(retv);
    //
    ICreateErrorInfo* pCErrInfo;
    IErrorInfo*       pErrInfo;
    if (SUCCEEDED(CreateErrorInfo(&pCErrInfo)))
    {
      pCErrInfo->SetGUID( LIBID_DBO22 );
      pCErrInfo->SetSource(excepInfo.bstrSource);
      pCErrInfo->SetDescription(excepInfo.bstrDescription);
      //
      if (SUCCEEDED(pCErrInfo->QueryInterface(IID_IErrorInfo, 
					      (LPVOID FAR*)&pErrInfo)))
	{
          SetErrorInfo(0, pErrInfo);
          pErrInfo->Release();
        }
      pCErrInfo->Release();
    }
#endif
  }	                

  if (0 != usrhp) OCIHandleFree((dvoid *)usrhp, OCI_HTYPE_SESSION);
  if (0 != svchp) OCIHandleFree((dvoid *)svchp, OCI_HTYPE_SVCCTX);
  if (0 != srvhp) OCIHandleFree((dvoid *)srvhp, OCI_HTYPE_SERVER);
  if (0 != errhp) OCIHandleFree((dvoid *)errhp, OCI_HTYPE_ERROR);

  //  return DBO_E_RUNTIME_PROP_TEMP_READONLY;//NOERROR;
  return retv;
}

#endif
