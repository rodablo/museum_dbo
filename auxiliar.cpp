/**
*** DBO11(R)1997
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"

/**
*** Module Varios
***/
int
BuildNumber()
{
  return (int)(__DBO2_BUILD_NUMBER__);
}

/**
*** MALLOC & FREE
***/
extern "C" 
{
  void* _RTLENTRY _EXPFUNC totomalloc(size_t size) {
    return ::HeapAlloc(::GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, size);
  }

  void* _RTLENTRY _EXPFUNC totorealloc(void* block, size_t size) {
    return ::HeapReAlloc(::GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, block,  size);
  }

  void _RTLENTRY _EXPFUNC totofree(void* block) {
    ::HeapFree(::GetProcessHeap(), /*HEAP_NO_SERIALIZE*/0, block);
  }
}

/**
*** ODATE
***/

/*
Commentarios desde el WIN32 Reference

A variant time is stored as an 8-byte real value (double), representing a date between 
January 1, 1753 and December 31, 2078, inclusive. The value 2.0 represents January 1, 1900; 
3.0 represents January 2, 1900, and so on. Adding 1 to the value increments the date by a day. 

The fractional part of the value represents the time of day. Therfore, 2.5 represents noon 
on January 1, 1900; 3.25 represents 6:00 a.m. on January 2, 1900, and so on. 
Negative numbers represent the dates prior to December 30, 1899.

// dias desde 1/1/1900 
V_DATE(pV) = (double) ((2.0 + pDT->dtdays) +                   
                                             // 300th de segundo desde mid
                                             ((1.0/(24.0*60*60*300)) * pDT->dttime));
*/

/*
DATE
The DATE datatype can update, insert, or retrieve a date value using the Oracle internal date binary format. A date in binary format contains seven bytes, as shown in Table 3 – 4.

BYTE                                       1       2     3     4   5    6      7
Meaning                                    Century Year  Month Day Hour Minute Second
Example (for 30–NOV–1992, 3:17 PM)   119     192   11    30  16   18     1

Table 3 – 4 Format of the DATE Datatype

    The century and year bytes are in an excess -100 notation. Dates Before Common Era (BCE) are 
less than 100. The era begins on 01-JAN-4712 BCE, which is Julian day 1. For this date, 
the century byte is 53, and the year byte is 88. The hour, minute, and second bytes 
are in excess-1 notation. The hour byte ranges from 1 to 24, the minute and second bytes 
from 1 to 60. If no time was specified when the date was created, 
the time defaults to midnight (1, 1, 1).
    
    When you input a date in binary format using the DATE external datatype, the database 
does not do consistency or range checking. All data in this format must be carefully validated 
before input. 

Note: There is little need to use the Oracle external DATE datatype in ordinary 
database operations. It is much more convenient to convert DATEs in character format, 
because the program usually displays (on a query) or inputs in a character format, such 
as 'DD-MON-YY’.

*/

ODATE::ODATE()
{
  _Century = 119;
  _Year    = 100;
  _Month   = 
  _Day     =   
  _Hour    = 
  _Minute  = 
  _Second  = 1;
}

ODATE::ODATE(const DATE& date)
{
  *this = date;
}

//#define  _WIN32
//#include <\mstools\include\oleauto.h> //VariantTimeToSystemTime...
WINOLEAUTAPI_(INT) SystemTimeToVariantTime(LPSYSTEMTIME lpSystemTime, double* pvtime);
WINOLEAUTAPI_(INT) VariantTimeToSystemTime(double vtime, LPSYSTEMTIME lpSystemTime);


ODATE& 
ODATE::operator = (const DATE& date)
{ 
  SYSTEMTIME st;
  // Convierte 
  if (!VariantTimeToSystemTime(date, &st))
    RAISE_LASTERROR_();//INTERNAL(1);
  // Extrae 
  _Century = (ub1)((st.wYear / 100) + 100);
  _Year    = (ub1)((st.wYear % 100) + 100);
  _Month   = (ub1) st.wMonth;
  _Day     = (ub1) st.wDay; 
  _Hour    = (ub1) ++st.wHour;
  _Minute  = (ub1) ++st.wMinute;
  _Second  = (ub1) ++st.wSecond;  
  //
  return *this;
}

ODATE::operator DATE ()
{
  SYSTEMTIME st;
  DATE       date;
  // Prepara la conversion
  st.wYear         = (WORD)(((_Century - 100) * 100) + (_Year - 100)); 
  st.wMonth        = _Month; 
  st.wDayOfWeek    = 0; 
  st.wDay          = _Day; 
  st.wHour         = (WORD)(_Hour - 1); 
  st.wMinute       = (WORD)(_Minute - 1); 
  st.wSecond       = (WORD)(_Second - 1); 
  st.wMilliseconds = 0;
  // DoIt
  if (!SystemTimeToVariantTime(&st, &date))
    RAISE_LASTERROR_();
  //
  return date;
}
