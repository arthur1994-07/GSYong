//
//mjeon.ado.wrapping class
// 2010.12.29
//

#include "stdafx.h"
#include "../DBBase.h"
#include "ADOClass.hpp"


CjADO::CjADO(PF_PRINT pf)
:m_strConn(CString(""))
,m_pConnection(NULL)
,m_pCommand(NULL)
,m_pRecordSet(NULL)
,m_pfPrintMsg(NULL)
,m_bConnected(FALSE)
,m_bCollected(TRUE)
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
,m_pCommand(NULL)
,m_pRecordSet(NULL)
,m_pfPrintMsg(NULL)
,m_bConnected(FALSE)
,m_bCollected(TRUE)
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
,m_pCommand(NULL)
,m_pRecordSet(NULL)
,m_pfPrintMsg(NULL)
,m_bConnected(FALSE)
,m_bCollected(TRUE)
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

	try
	{
		m_pCommand = NULL;
		m_pRecordSet = NULL;
		m_pConnection = NULL;

		//following steps are done by ->Close() or automatically when ~CjADO() finishes.
		/*
		if( m_pConnection != NULL )
		{
			m_pCommand->Release();
			m_pRecordSet->Release();
			m_pConnection->Release();
		}
		*/
	}
	catch( _com_error &e )
	{
		eHandler(e);		
	}
	catch (...)
	{
		PRINT(_T("~CjADO(): Unhandled Exception!"));
	}

	/*
	if( FAILED( ::CoUninitialize(NULL) ) )
	{	
		PRINT(_T("::CoUninitialized() Failure."));
	}
	*/

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
		//m_pConnection	= NULL;
		//m_pCommand	= NULL;
		//m_pRecordSet	= NULL;

		CHECK_HRESULT( m_pConnection.CreateInstance(__uuidof(Connection)) );

		m_strConn = _bstr_t(csConn);

		CHECK_HRESULT( m_pConnection->Open(m_strConn, _T(""), _T(""), -1) );

		
		//
		// Command object is required to be created only once.
		//
		if (m_pCommand == NULL)
		{
			CHECK_HRESULT( m_pCommand.CreateInstance(__uuidof(Command)) );
		}

		m_pCommand->ActiveConnection = m_pConnection;
		//m_pCommand->CommandType = adCmdStoredProc;
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


BOOL CjADO::ReOpen()
{
	if ( IsOpened() )
	{
		Close();		
	}
	
	return ( Open(m_strConn) );
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
		CloseRecordSet();	// this call is optional. refer to following explanation about the close.

		try
		{
			if( m_pConnection->GetState() != adStateClosed )
			{
				/*
				The Close method is used to close a Connection object and certain associated objects.
				Closing an Connection object does not delete it from memory.
				You can later use the Open method to reopen the same Connection. 

				If there are open RecordSet objects associated with the Connection object being closed, 
				then all of the RecordSet objects are automatically closed. 				
				There is also a rollback of all pending changes in the open RecordSet objects. 
				Calling Close while a transaction is in progress will generate an error. 
				*/

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

BOOL CjADO::Execute(const std::string& Query)
{
    CString NewQuery(Query.c_str());
    return Execute(NewQuery);
}

//
//Execute via Connection
// generallay for Raw Queries
//
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
		m_pRecordSet = m_pConnection->Execute( (_bstr_t)csQuery, NULL, adOptionUnspecified);		
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

	/*
	if ( GetEOF() )
	{
		PRINT(_T("ADO: There's no data. RecordSet is Empty."));

		return FALSE;
	}
	*/

	return TRUE;
}

//
//Execute via Command
// especially for Stored Procedure
//
BOOL CjADO::Execute4Cmd(CString &csQuery, CommandTypeEnum emCommandType)
{
	if (csQuery.IsEmpty())
	{
		PRINT(_T("Query is empty."));

		return FALSE;
	}

	if ( !IsOpened() )
	{
		PRINT(_T("ADO: ExecuteSP() Failure. Connection MUST be opened."));

		return FALSE;
	}

	//
	// Close previous RecordSet before opening of new RecordSet.
	//
	CloseRecordSet();

	try
	{
		//
		// Set-up the Command specifically.
		//
		m_pCommand->PutCommandText((_bstr_t)csQuery);
		m_pCommand->PutCommandType(emCommandType);

		m_pRecordSet = m_pCommand->Execute(NULL, NULL, emCommandType);
	}
	catch( _com_error &e )
	{
		eHandler(e);
	}
	catch (...)
	{
		PRINT(_T("ExecuteSP(): Unhandled Exception!"));
	}

	PRINT(_T("ADO: Executed the Query."));
	PRINT(_T("ADO: Opened RecordSet Successfully."));

	/*
	if ( GetEOF() )
	{
		PRINT(_T("ADO: There's no data. RecordSet is Empty."));

		return FALSE;
	}
	*/

	return TRUE;
}


//
//Execute via RecordSet
// Customize the RecordSet configurations
//
BOOL CjADO::Execute4RS(CString &csQuery, CommandTypeEnum emCommandType, CursorTypeEnum emCursorType, LockTypeEnum emLockType)
{
	if (csQuery.IsEmpty())
	{
		PRINT(_T("Query is empty."));

		return FALSE;
	}

	if ( !IsOpened() )
	{
		PRINT(_T("ADO: ExecuteEx() Failure. Connection MUST be opened."));

		return FALSE;
	}

	//
	// Close previous RecordSet before opening of new RecordSet.
	//
	CloseRecordSet();

	try
	{
		m_pRecordSet.CreateInstance(__uuidof(Recordset));

		//
		// In some cases, following style of set-up could be used.
		//
		//m_pRecordSet->PutCursorType(adOpenDynamic);
		//m_pRecordSet->PutLockType(adLockPessimistic);	
		//

		CHECK_HRESULT( m_pRecordSet->Open(  _variant_t(csQuery),
											_variant_t( (IDispatch *)m_pConnection, TRUE ),
											emCursorType, adLockOptimistic, emCommandType)  );
											//adOpenDynamic, adLockPessimistic, adCmdText);
	}
	catch( _com_error &e )
	{
		eHandler(e);		
	}
	catch (...)
	{
		PRINT(_T("ExecuteEx(): Unhandled Exception!"));
	}

	PRINT(_T("ADO: Executed the Query."));
	PRINT(_T("ADO: Opened RecordSet Successfully."));

	/*
	if ( GetEOF() )
	{
		PRINT(_T("ADO: There's no data. RecordSet is Empty."));

		return FALSE;
	}
	*/

	return TRUE;
}


BOOL CjADO::AppendParam(_bstr_t name, DataTypeEnum type, ParameterDirectionEnum direction, IN _variant_t &var, long size)
{
	BOOL bRet = TRUE;

	_ParameterPtr param;

	
	try
	{
		param = m_pCommand->CreateParameter(name, type, direction, size);
		
		if (direction != adParamOutput && direction != adParamReturnValue)
		{			
			param->Value = var;
			param->Type = type;
		}

		CHECK_HRESULT( m_pCommand->Parameters->Append(param) );
	}
	catch( _com_error &e )
	{
		eHandler(e);
		bRet = FALSE;
	}
	catch (...)
	{
		PRINT(_T("AppendParam(): Unhandled Exception!"));
		bRet = FALSE;
	}

	return bRet;
}


BOOL CjADO::GetParam(_bstr_t name, OUT _variant_t &var)
{
	BOOL bRet = TRUE;

	try
	{
		var = m_pCommand->Parameters->GetItem(name)->Value;
	}
	catch( _com_error &e )
	{
		eHandler(e);
		bRet = FALSE;
	}
	catch (...)
	{
		PRINT(_T("GetParam(): Unhandled Exception!"));
		bRet = FALSE;
	}

	return bRet;
}


BOOL CjADO::MoveFirst()
{
	BOOL bRet = TRUE;
	m_bCollected = TRUE;

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
	m_bCollected = TRUE;
	
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
	m_bCollected = TRUE;

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
	m_bCollected = TRUE;

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
	m_bCollected = TRUE;

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
// Handling Chunk
//
BOOL CjADO::GetChunk(CString &csColumn, IN OUT LPBYTE pBuffer)
{
/*
	if (!pBuffer)
	{
		PRINT(_T("GetChunk(): parameter pBuffer is NULL."));
		return FALSE;
	}
*/
	/*
	long cnt = 0; 
	cnt = ado.m_pRecordSet->GetFields()->Count;		
	*/
	BOOL		bRet = TRUE;

	long		nSizeOfChunk = 0;
	_variant_t	vartBLOB;

	try
	{
		nSizeOfChunk = m_pRecordSet->GetFields()->GetItem(_variant_t("ChaPutOnItems"))->GetActualSize();

		if (nSizeOfChunk > 0)
		{
			vartBLOB = m_pRecordSet->GetFields()->GetItem(_variant_t("ChaPutOnItems"))->GetChunk(nSizeOfChunk);


			if( vartBLOB.vt == (VT_ARRAY | VT_UI1) )
			{			
				LPBYTE pBuf = NULL;

				SafeArrayAccessData( vartBLOB.parray, (void **)&pBuf );

				pBuffer = new BYTE[nSizeOfChunk];

				memcpy(pBuffer, pBuf, nSizeOfChunk);
				
				SafeArrayUnaccessData( vartBLOB.parray );
			}
			else
			{
				bRet = FALSE;
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	catch( _com_error &e )
	{
		eHandler(e);
	}
	catch (...)
	{
		PRINT(_T("TestBLOB(): Unhandled Exception!"));
	}

	m_bCollected &= bRet;

	return bRet;
}


BOOL CjADO::GetChunk(LPCTSTR pColumn, IN OUT LPBYTE pBuffer)
{
	if (!pColumn)
	{
		PRINT(_T("GetChunk(): parameter pColumn is NULL."));
		return FALSE;
	}

	CString csColumn(pColumn);

	return GetChunk(csColumn, pBuffer);
}



BOOL CjADO::GetChunk(CString &csColumn, OUT se::ByteStream &bytestream)
{
	/*
	long cnt = 0; 
	cnt = ado.m_pRecordSet->GetFields()->Count;		
	*/
	BOOL		bRet = TRUE;

	long		nSizeOfChunk = 0;
	_variant_t	vartBLOB;

	bytestream.ClearBuffer();

	try
	{
		nSizeOfChunk = m_pRecordSet->GetFields()->GetItem(_variant_t(csColumn))->GetActualSize();

		if (nSizeOfChunk > 0)
		{
			vartBLOB = m_pRecordSet->GetFields()->GetItem(_variant_t(csColumn))->GetChunk(nSizeOfChunk);

			if( vartBLOB.vt == (VT_ARRAY | VT_UI1) )
			{			
				LPBYTE pBuf = NULL;

				SafeArrayAccessData( vartBLOB.parray, (void **)&pBuf );						
				
				bytestream.WriteBuffer( pBuf, nSizeOfChunk );
				
				SafeArrayUnaccessData( vartBLOB.parray );
			}
			else
			{
				bRet = FALSE;
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	catch( _com_error &e )
	{
		eHandler(e);
	}
	catch (...)
	{
		PRINT(_T("TestBLOB(): Unhandled Exception!"));
	}

	m_bCollected &= bRet;

	return bRet;
}


BOOL CjADO::GetChunk(LPCTSTR pColumn, OUT se::ByteStream &bytestream)
{
	if (!pColumn)
	{
		PRINT(_T("GetChunk(): parameter pColumn is NULL."));
		return FALSE;
	}

	CString csColumn(pColumn);

	return GetChunk(csColumn, bytestream);
}


BOOL CjADO::MakeChunk(OUT _variant_t &vartBLOB, LPBYTE pData, IN ULONG &nSizeOfData)
{
	if ( !pData || nSizeOfData <= 0 )
	{
		PRINT(_T("MakeChunk(): Invalid pData or nSize."));

		return FALSE;
	}

	BOOL bRet = TRUE;

	SAFEARRAY *pSA;
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = 0;	
	rgsabound[0].cElements = nSizeOfData;

	pSA = SafeArrayCreate(VT_UI1, 1, rgsabound);


	//
	//Destination
	//
	void HUGEP *pSafeBuffer = NULL;
	SafeArrayAccessData( pSA, (void **)&pSafeBuffer );

	if ( !pSafeBuffer )
	{
		PRINT(_T("MakeChunk(): Invalid pSafeBuffer."));

		bRet = FALSE;
	}
	else
	{
		memcpy(pSafeBuffer, pData, nSizeOfData);
	}

	SafeArrayUnaccessData( pSA );

	vartBLOB.vt = (VT_ARRAY | VT_UI1);
	vartBLOB.parray = pSA;

	return bRet;
}


BOOL CjADO::MakeChunk(OUT _variant_t &vartBLOB, se::ByteStream &bytestream, OUT ULONG &nSizeOfData)
{
	//
	//Src
	//
	LPBYTE pData = NULL;
	bytestream.GetBuffer(pData, nSizeOfData);	//Got dwSizeOfStream


	if ( !pData || nSizeOfData <= 0 )
	{
		PRINT(_T("MakeChunk(): Invalid ByteStream."));

		return FALSE;
	}


	BOOL bRet = TRUE;

	SAFEARRAY *pSA;
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = 0;	
	rgsabound[0].cElements = nSizeOfData;

	pSA = SafeArrayCreate(VT_UI1, 1, rgsabound);


	//
	//Destination
	//
	void HUGEP *pSafeBuffer = NULL;
	SafeArrayAccessData( pSA, (void **)&pSafeBuffer );

	if ( !pSafeBuffer )
	{
		PRINT(_T("MakeChunk(): Invalid pSafeBuffer."));

		bRet = FALSE;
	}
	else
	{
		//
		//ByteStream::GetBuffer() MUST NOT be used for SafeArray.
		// Otherwise, SafeArray's reference count will be screwed and the buffer's destruction 
		// will cause some memory access-violation.
		//
		//bytestream.GetBuffer((LPBYTE &)pSafeBuffer, nSizeOfData);

		memcpy(pSafeBuffer, pData, nSizeOfData);
	}

	SafeArrayUnaccessData( pSA );

	vartBLOB.vt = (VT_ARRAY | VT_UI1);
	vartBLOB.parray = pSA;

	return bRet;
}


//
// Tool Functions
//

BOOL CjADO::PRINT(CString &csMsg)
{
	if (m_pfPrintMsg == NULL)
	{
		printf(csMsg.GetBuffer());
		printf("\n");
		return FALSE;
	}

	if ( csMsg.IsEmpty() )
		return FALSE;

	m_pfPrintMsg(csMsg);

	return TRUE;
}


BOOL CjADO::PRINT(LPCTSTR pMsg)
{
	if (m_pfPrintMsg == NULL)
	{
		if (pMsg == NULL)
			return FALSE;

		printf(pMsg);
		printf("\n");
		return FALSE;
	}

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

