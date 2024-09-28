//
//mjeon.ado.wrapping class
// 2010.12.29
//

#include "stdafx.h"
#include "odbc/DBBase.h"
#include "ADOClass.hpp"


CjADO::CjADO(PF_PRINT pf)
:m_strConn(ADO_CONN_STR_USERDB)			//default UserDB
,m_pConnection(NULL)
,m_pRecordSet(NULL)
,m_pfPrintMsg(NULL)
,m_bConnected(FALSE)
{
	SetPrintMsg(pf);

	PRINT(_T("ADO: CADO Constructed."));
/*
	if( FAILED( ::CoInitialize(NULL) ) )
	{	
		PRINT(_T("::CoInitialized() Failure."));
	}
*/
}


CjADO::CjADO(CString &csConn, PF_PRINT pf)
:m_strConn(csConn)			//default UserDB
,m_pConnection(NULL)
,m_pRecordSet(NULL)
,m_pfPrintMsg(NULL)
,m_bConnected(FALSE)
{
	SetPrintMsg(pf);

	PRINT(_T("ADO: CADO Constructed."));
/*
	if( FAILED( ::CoInitialize(NULL) ) )
	{	
		PRINT(_T("::CoInitialized() Failure."));
	}
*/

	m_bConnected = Open(csConn);	
}


CjADO::CjADO(LPCTSTR szConn, PF_PRINT pf)
:m_strConn(szConn)			//default UserDB
,m_pConnection(NULL)
,m_pRecordSet(NULL)
,m_pfPrintMsg(NULL)
,m_bConnected(FALSE)
{
	SetPrintMsg(pf);

	PRINT(_T("ADO: CADO Constructed."));
/*
	if( FAILED( ::CoInitialize(NULL) ) )
	{	
		PRINT(_T("::CoInitialized() Failure."));
	}
*/

	m_bConnected = Open(szConn);
}


CjADO::~CjADO()
{
	Close();

	if( m_pConnection != NULL )
	{
		m_pConnection.Release();
		m_pConnection = NULL;
	}

//	::CoUninitialize();

	PRINT(_T("ADO: CADO Destructed."));
}


BOOL CjADO::Open(CString &csConn)
{
	if ( csConn.IsEmpty() )
	{
		PRINT(_T("ConnectionString is empty."));
		return FALSE;
	}

	try
	{
		m_pConnection = NULL;
		m_pRecordSet = NULL;

		CHECK_HRESULT( m_pConnection.CreateInstance(__uuidof(Connection)) );
		PRINT(_T("ADO: CreateInstance() succeed."));


		m_strConn = _bstr_t(csConn);

		CHECK_HRESULT( m_pConnection->Open(m_strConn, _T(""), _T(""), -1) );
		PRINT(_T("ADO: Open() succeed."));
	}
	catch( _com_error &e )
	{
		eHandler(e);

		PRINT(_T("ADO: OpenConnection Failure."));

		return FALSE;
	}
	catch (...)
	{
		PRINT(_T("Open(): Unhandled Exception!"));

		return FALSE;
	}


	PRINT(_T("ADO: Opened Connection Successfully."));

	return TRUE;
}


BOOL CjADO::Open(LPCTSTR szConn)
{
	CString csConn(szConn);

	return Open(csConn);
}


BOOL CjADO::IsOpened()
{
	if (m_pConnection == NULL)
		return FALSE;

	try	
	{
		if ( m_pConnection->GetState() == adStateClosed )
		return FALSE;
	}
	catch( _com_error &e )
	{
		eHandler(e);		
	}
	catch (...)
	{
		PRINT(_T("IsOpened(): Unhandled Exception!"));
	}
	
	return TRUE;
}


void CjADO::Close()
{
	if( m_pConnection )
	{
		CloseRecordSet();

		try
		{
			if( m_pConnection->GetState() != adStateClosed )
			{
				m_pConnection->Close();
			}
		}
		catch( _com_error &e )
		{
			eHandler(e);		
		}
		catch (...)
		{
			PRINT(_T("Close(): Unhandled Exception!"));
		}

		PRINT(_T("ADO: Closed Connection Successfully."));
	}
}


void CjADO::OpenRecordSet()
{
	;//Done by assignment of return value of Execute();
}


void CjADO::CloseRecordSet()
{
	if( m_pRecordSet )
	{
		try
		{
			if( m_pRecordSet->GetState() != adStateClosed )
			{
				m_pRecordSet->Close();
			}
		}
		catch( _com_error &e )
		{
			eHandler(e);		
		}
		catch (...)
		{
			PRINT(_T("CloseRecordSet(): Unhandled Exception!"));
		}

		PRINT(_T("ADO: Closed RecordSet Successfully."));
	}
}


BOOL CjADO::Execute(CString &csQuery)
{
	if (csQuery.IsEmpty())
	{
		PRINT(_T("Query is empty."));
		return FALSE;
	}

	if ( !IsOpened() )
	{
		PRINT(_T("ADO: Execute() Failure. Connection MUST be opened."));

		return FALSE;
	}

	//
	// Close previous RecordSet before opening of new RecordSet.
	//
	CloseRecordSet();

	try
	{
		m_pRecordSet = m_pConnection->Execute( (_bstr_t)csQuery, NULL, adOptionUnspecified );
	}
	catch( _com_error &e )
	{
		eHandler(e);		
	}
	catch (...)
	{
		PRINT(_T("Execute(): Unhandled Exception!"));
	}

	PRINT(_T("ADO: Executed the Query."));
	PRINT(_T("ADO: Opened RecordSet Successfully."));

	if ( GetEOF() )
	{
		//PRINT(_T("ADO: There's no data. RecordSet is Empty."));

		return FALSE;
	}

	return TRUE;
}


BOOL CjADO::MoveFirst()
{
	BOOL bRet = TRUE;

	try
	{
		if( m_pRecordSet != NULL )
		{
			CHECK_HRESULT( m_pRecordSet->MoveFirst() );
		}
		else
		{
			PRINT(_T("MoveFirst(): RecordSet is NULL."));
			bRet = FALSE;
		}
	}
	catch( _com_error &e )
	{
		eHandler(e);
		bRet = FALSE;
	}
	catch (...)
	{
		PRINT(_T("MoveFirst(): Unhandled Exception!"));
		bRet = FALSE;
	}

	//PRINT(_T("ADO: Moved to First."));
	return bRet;
}


BOOL CjADO::MoveNext()
{
	BOOL bRet = TRUE;
	
	try
	{
		if (m_pRecordSet != NULL)
		{
			CHECK_HRESULT( m_pRecordSet->MoveNext() );
		}
		else
		{
			PRINT(_T("MoveNext(): RecordSet is NULL."));
			bRet = FALSE;
		}
	}
	catch( _com_error &e )
	{
		eHandler(e);
		bRet = FALSE;
	}
	catch (...)
	{
		PRINT(_T("MoveNext(): Unhandled Exception!"));
		bRet = FALSE;
	}

	//PRINT(_T("ADO: Moved to Next."));
	return bRet;
}


BOOL CjADO::MovePrevious()
{
	BOOL bRet = TRUE;

	try
	{
		if( m_pRecordSet != NULL )
		{
			CHECK_HRESULT( m_pRecordSet->MovePrevious() );
		}
		else
		{
			PRINT(_T("MovePrevious(): RecordSet is NULL."));
			bRet=FALSE;
		}
	}
	catch( _com_error &e )
	{
		eHandler(e);
		bRet = FALSE;
	}
	catch (...)
	{
		PRINT(_T("MovePrevious(): Unhandled Exception!"));
		bRet = FALSE;
	}

	//PRINT(_T("ADO: Moved to Previous."));
	return bRet;
}


BOOL CjADO::MoveLast()
{
	BOOL bRet = TRUE;

	try
	{
		if( m_pRecordSet != NULL )
		{
			CHECK_HRESULT(  m_pRecordSet->MoveLast() );
		}
		else
		{
			PRINT(_T("MoveLast(): RecordSet is NULL."));
			bRet=FALSE;
		}
	}
	catch( _com_error &e )
	{
		eHandler(e);
		bRet=FALSE;
	}
	catch (...)
	{
		PRINT(_T("MoveLast(): Unhandled Exception!"));
		bRet=FALSE;
	}

	//PRINT(_T("ADO: Moved to the Last."));
	return bRet;
}


BOOL CjADO::Move(long nIndex)
{
	BOOL bRet = TRUE;

	try
	{
		if( m_pRecordSet != NULL )
		{
			CHECK_HRESULT( m_pRecordSet->Move(nIndex) );
		}
		else
		{
			PRINT(_T("Move(): RecordSet is NULL."));
			bRet=FALSE;
		}
	}
	catch( _com_error &e )
	{
		eHandler(e);
		bRet = FALSE;
	}
	catch (...)
	{
		PRINT(_T("Move(): Unhandled Exception!"));
		bRet = FALSE;
	}

	//PRINT(_T("ADO: Moved to index."));
	return bRet;
}


BOOL CjADO::GetBOF()
{
	BOOL bRet = TRUE;
	
	try
	{
		if( m_pRecordSet != NULL )
			bRet = m_pRecordSet->GetBOF();
		else
		{
			PRINT(_T("Move(): RecordSet is NULL."));
			bRet = FALSE;
		}
	}
	catch( _com_error &e )
	{
		eHandler(e);
		bRet = FALSE;
	}
	catch (...)
	{
		PRINT(_T("Move(): Unhandled Exception!"));
		bRet = FALSE;
	}

	return bRet;
}


BOOL CjADO::GetEOF()
{
	BOOL bRet = TRUE;

	try
	{
		if( m_pRecordSet != NULL )
			bRet = m_pRecordSet->GetadoEOF();
		else
		{
			PRINT(_T("Move(): RecordSet is NULL."));
			bRet = FALSE;
		}
	}
	catch( _com_error &e )
	{
		eHandler(e);
		bRet = FALSE;
	}
	catch (...)
	{
		PRINT(_T("Move(): Unhandled Exception!"));
		bRet = FALSE;
	}

	return bRet;
}


BOOL CjADO::Next()
{
	if ( !MoveNext() )
		return FALSE;	

	if ( GetEOF() )
		return FALSE;

	return TRUE;
}


//
// Tool Functions
//

BOOL CjADO::PRINT(CString &csMsg)
{
	if (m_pfPrintMsg == NULL)
		return FALSE;

	if ( csMsg.IsEmpty() )
		return FALSE;

	m_pfPrintMsg(csMsg);

	return TRUE;
}


BOOL CjADO::PRINT(LPCTSTR pMsg)
{
	if (m_pfPrintMsg == NULL)
		return FALSE;	

	if (pMsg == NULL)
		return FALSE;

	m_pfPrintMsg( CString(pMsg) );

	return TRUE;
}


void CjADO::eHandler( _com_error e)
{
	CString csMsg = _T("");

	_bstr_t bstrSource( e.Source() );
	_bstr_t bstrDescription( e.Description() );

	csMsg.Format( _T("%s: %s"), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription );

	TRACE( csMsg );

	PRINT(csMsg);
}


void CjADO::eHandler( ErrorsPtr errors )
{	
	CString csMsg	=_T("");

	if( errors == NULL )
	{
		PRINT(csMsg);

		return; 
	}

	register long cntErrors = errors->GetCount();
	register long nIndex = 0;


	for (long i=0; i< cntErrors; i++ )
	{
		long	ErrorNum;

		_bstr_t Description;
		_bstr_t Source;
		_bstr_t SqlState;		

		ErrorPtr ep = errors->Item[nIndex];

		ErrorNum		= ep->GetNumber();

		if( !ErrorNum )
			continue;

		Description	= ep->GetDescription();
		Source		= ep->GetSource();
		SqlState	= ep->GetSQLState();


		csMsg.Format(_T("%s: (%u)%s - %s"),
			(LPCTSTR)Source, ErrorNum, (LPCTSTR)SqlState, ( LPCTSTR )Description );

		TRACE( csMsg );

		PRINT(csMsg);
	}

	errors->Clear();
}
