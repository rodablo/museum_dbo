/**
*** DBO2(1997/1998)
*** rodablo@hotmail.com
***/
#include "pch.hxx"
#include "dbo.hxx"
#include "column.hxx"

TBSTRColumn::TBSTRColumn(TColumnD& cd)
   :  TColumn(cd)
{
  m_pTmp = 0;
}

TBSTRColumn::~TBSTRColumn()
{
  // fue excepcion?
  if (0 != m_pTmp)
      delete [] m_pTmp;
}

void  
TBSTRColumn::PreWork()
{
  // alloco el vector auxiliar para bindear
  m_pTmp = new ub1[GetRowsXFetch() * m_bindSize];
  // define finalmente
  CHECK_OCI(::odefin(m_IICursor.GetCDA(), m_pos, (ub1*)m_pTmp, 
		     m_bindSize, m_bindType, m_scale, m_pIndp, 0, -1, -1, m_pRLen, m_pRC));
}

void  
TBSTRColumn::PosWork()
{
  // libera SysString anteriores
  CHECK_HRESULT(SafeArrayDestroyData(m_psaRows));
  CHECK_HRESULT(SafeArrayAllocData(m_psaRows));

  // Obtiene un puntero a la data del vector en crudo! y lo mantiene lockeado
  BSTR* pR;
  CHECK_HRESULT(SafeArrayAccessData(m_psaRows, (void**)&pR));
  // este itera en todo el array y tiene que ser en el leido solo
  for (long r = 0 ; r < m_IICursor.GetCount() ; r++)
    {
      // si es nulo o hubo error en este row ignora
      if (0 != m_pIndp[r] || 0 != m_pRC[r]) 
	{
	  pR[r] = 0; 
	  continue;
	}
      // crea un SysStr
      pR[r] = WIDE( ((char*)m_pTmp) + (m_bindSize * r) ).SysAllocString();
    }
  // primero (por el CHECK) libera el temp y luego invalida el puntero del bind
  delete [] m_pTmp;
  m_pTmp = 0;
  // deslockea el vector 
  CHECK_HRESULT(SafeArrayUnaccessData(m_psaRows));
}

void  
TBSTRColumn::PosWorkPreRAISE()
{
  // libera e invalida
  if (0 != m_pTmp)
    {
    delete [] m_pTmp;
    m_pTmp = 0;
    }
}
















