#include "stdafx.h"
#include "../../String/StringFormat.h"
#include "../../Log/LogMan.h"
#include "../DbDefine.h"
#include "AdoClass.h"

// ----------------------------------------------------------------------------
#include "../../DebugInclude.h"
// ----------------------------------------------------------------------------

#define NULL_STRING			"NULL"

namespace sc {
    namespace db {

CjADO::CjADO(PF_PRINT pf)
    : m_strConn("")
    , m_pConnection(NULL)
    , m_pCommand(NULL)
    , m_pRecordSet(NULL)
    , m_pfPrintMsg(NULL)
    , m_bConnected(FALSE)
    , m_bCollected(TRUE)    
{
	SetPrintMsg(pf);

	//PRINT(_T("ADO: CADO Constructed."));

	/*
	if( FAILED( ::CoInitialize(NULL) ) )
	{	
		PRINT(_T("::CoInitialized() Failure."));
	}
	*/
}

CjADO::CjADO(const std::string& csConn, PF_PRINT pf)
    : m_strConn(csConn.c_str())			//default UserDB
    , m_pConnection(NULL)
    , m_pCommand(NULL)
    , m_pRecordSet(NULL)
    , m_pfPrintMsg(NULL)
    , m_bConnected(FALSE)
    , m_bCollected(TRUE)
{
	SetPrintMsg(pf);
	//PRINT(_T("ADO: CADO Constructed."));
	/*
	if( FAILED( ::CoInitialize(NULL) ) )
	{	
		PRINT(_T("::CoInitialized() Failure."));
	}
	*/
	m_bConnected = Open(csConn);	
}

CjADO::CjADO(LPCTSTR szConn, PF_PRINT pf)
    : m_strConn(szConn)			//default UserDB
    , m_pConnection(NULL)
    , m_pCommand(NULL)
    , m_pRecordSet(NULL)
    , m_pfPrintMsg(NULL)
    , m_bConnected(FALSE)
    , m_bCollected(TRUE)
{
	SetPrintMsg(pf);
	//PRINT(_T("ADO: CADO Constructed."));
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
		eHandler("~CjADO", e);
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

	//PRINT(_T("ADO: CADO Destructed."));
}

BOOL CjADO::Open(const std::string& csConn)
{
	if (csConn.empty())
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

		m_strConn = _bstr_t(csConn.c_str());

		// ´ë¸¸ ðíÛ¯íûöÇÍöÊàÜôðë£¬ÙíÛöèÇà÷ó®ðÃíÂ ¹®Á¦
		// -> Not enough storage is available to complete this operation. 
		// -2147024882 (0x8007000E) 
		// ¿¡·¯ ¼öÁ¤
		// http://support.microsoft.com/kb/182423/en-us
		// ´Ù½Ã µ¹¸² default: adUseServer

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
        m_ProductVersion = m_pConnection->GetVersion();
	}
	catch( _com_error &e )
	{
		eHandler(csConn, e, true);

		PRINT(_T("ADO: OpenConnection Failure."));

		return FALSE;
	}
	catch (...)
	{
		PRINT(_T("Open(): Unhandled Exception!"));

		return FALSE;
	}

	//PRINT(_T("ADO: Opened Connection Successfully."));

	return TRUE;
}


BOOL CjADO::Open(LPCTSTR szConn)
{
	std::string csConn(szConn);

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
		eHandler("IsOpened", e);		
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
            eHandler("Close", e);		
		}
		catch (...)
		{
			PRINT(_T("Close(): Unhandled Exception!"));
		}

		//PRINT(_T("ADO: Closed Connection Successfully."));
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
			eHandler("CloseRecordSet", e);		
		}
		catch (...)
		{
			PRINT(_T("CloseRecordSet(): Unhandled Exception!"));
		}

		//PRINT(_T("ADO: Closed RecordSet Successfully."));
	}
}


//
//Execute via Connection
// generallay for Raw Queries
//
BOOL CjADO::Execute(const std::string &csQuery)
{
	BOOL bRet = FALSE;

	if (csQuery.empty())
	{
		PRINT(_T("Query is empty."));

		return bRet;
	}
	
	if ( !IsOpened() )
	{
		PRINT(_T("ADO: Execute() Failure. Connection MUST be opened."));

		return bRet;
	}

	//
	// Close previous RecordSet before opening of new RecordSet.
	//
	CloseRecordSet();

	try
	{
		m_pRecordSet = m_pConnection->Execute((_bstr_t) csQuery.c_str(), NULL, adOptionUnspecified);

		bRet = TRUE;
	}
	catch( _com_error &e )
	{
		eHandler(csQuery, e);
	}
	catch (...)
	{
		PRINT(_T("Execute(): Unhandled Exception!"));
	}

	//PRINT(_T("ADO: Executed the Query."));
	//PRINT(_T("ADO: Opened RecordSet Successfully."));

	/*
	if ( GetEOF() )
	{
		PRINT(_T("ADO: There's no data. RecordSet is Empty."));

		return FALSE;
	}
	*/

	return bRet;
}

BOOL CjADO::ExecuteStoredProcedure(const std::string& StoredProcedureName)
{
    return Execute4Cmd(StoredProcedureName, adCmdStoredProc);
}

int CjADO::ExecuteStoredProcedureIntReturn(const std::string& SpName, const std::string& OutPutParamName)
{
    if (!Execute4Cmd(SpName, adCmdStoredProc))
    {
        sc::writeLogError(
            sc::string::format(
                "CjADO::ExecuteStoredProcedureIntReturn %1%", SpName));
        return sc::db::DB_ERROR;
    }

    _variant_t varReturn;
    if (!GetParam(OutPutParamName.c_str(), varReturn))
    {
        sc::writeLogError(
            sc::string::format(
                "CjADO::ExecuteStoredProcedureIntReturn %1% GetParam", SpName));
        return sc::db::DB_ERROR;
    }
    return varReturn.intVal;
}

LONGLONG CjADO::ExecuteStoredProcedureLonglongReturn(const std::string& SpName, const std::string& OutPutParamName)
{
    if (!Execute4Cmd(SpName, adCmdStoredProc))
    {
        sc::writeLogError(
            sc::string::format(
               "CjADO::ExecuteStoredProcedureLonglongReturn %1%", SpName));
        return sc::db::DB_ERROR;
    }

    _variant_t varReturn;
    if (!GetParam(OutPutParamName.c_str(), varReturn))
    {
        sc::writeLogError(
            sc::string::format(
                "CjADO::ExecuteStoredProcedureLonglongReturn %1% GetParam", SpName));
        return sc::db::DB_ERROR;
    }
    return varReturn.llVal;
}

//
//Execute via Command
// especially for Stored Procedure
//
BOOL CjADO::Execute4Cmd(const std::string &csQuery, CommandTypeEnum emCmdType)
{
	if (csQuery.empty())
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
		m_pCommand->PutCommandText((_bstr_t) csQuery.c_str());
		m_pCommand->PutCommandType(emCmdType);		

		m_pRecordSet = m_pCommand->Execute(NULL, NULL, emCmdType);
	}
	catch( _com_error &e )
	{
		eHandler(csQuery, e);
        return FALSE;
	}
	catch (...)
	{
		PRINT(_T("ExecuteSP(): Unhandled Exception!"));
        return FALSE;
	}

	//PRINT(_T("ADO: Executed the Query."));
	//PRINT(_T("ADO: Opened RecordSet Successfully."));

	/*
	if ( GetEOF() )
	{
		PRINT(_T("ADO: There's no data. RecordSet is Empty."));

		return FALSE;
	}
	*/

	return TRUE;
}

/*
//
//Execute via RecordSet
// Customize the RecordSet configurations
//
BOOL CjADO::Execute4RS(	
    const std::string& csQuery,
	CommandTypeEnum emCommandType,
    CursorTypeEnum emCursorType,
    LockTypeEnum emLockType
    )
{
	if (csQuery.empty())
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

		CHECK_HRESULT(
            m_pRecordSet->Open(
                _variant_t(csQuery.c_str()),
                _variant_t((IDispatch*) m_pConnection,
                TRUE),
            emCursorType,
            emLockType,
            emCommandType));
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

	//PRINT(_T("ADO: Executed the Query."));
	//PRINT(_T("ADO: Opened RecordSet Successfully."));

	
	//if ( GetEOF() )
	//{
    //  PRINT(_T("ADO: There's no data. RecordSet is Empty."));
	//	return FALSE;
	//}
	return TRUE;
}
*/

HRESULT CjADO::Execute4CmdNew( const std::string &csQuery, CommandTypeEnum emCommandType /*= adCmdText*/ )
{
	if ( csQuery.empty() )
	{
		PRINT( _T( "Query is empty." ) );
		return E_FAIL;
	}

	if ( !IsOpened() )
	{
		PRINT(_T("ADO: ExecuteSP() Failure. Connection MUST be opened."));
		return E_FAIL;
	}

	CloseRecordSet();

	try
	{
		m_pCommand->PutCommandText( static_cast< _bstr_t >( csQuery.c_str() ) );
		m_pCommand->PutCommandType( emCommandType );		

		m_pRecordSet = m_pCommand->Execute( NULL, NULL, emCommandType );
	}
	catch( _com_error &e )
	{
		return eHandler( csQuery, e );
	}
	catch ( ... )
	{
		PRINT(_T( "ExecuteSP(): Unhandled Exception!" ) );
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CjADO::ExecuteStoredProcedureNew( const std::string& StoredProcedureName )
{
	return Execute4CmdNew( StoredProcedureName, adCmdStoredProc );
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
	catch (_com_error& e)
	{
		sc::writeLogError(
			sc::string::format(
				"CjADO::AppendParam %1%", (const char*) name));
        eHandler(std::string((char*) name), e);
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
        eHandler(std::string((char*) name), e);
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
		eHandler("MoveFirst", e);
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
		eHandler("MoveNext", e);
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
		eHandler("MovePrevious", e);
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
		eHandler("MoveLast", e);
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
		eHandler("Move", e);
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
		eHandler("GetBOF", e);
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
		eHandler("GetEOF", e);
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

/*
long CjADO::GetRecordCount()
{
    try
    {    
        if (m_pRecordSet)
            return m_pRecordSet->GetRecordCount();
        else
            return 0;
    }
    catch (_com_error& e)
    {
        eHandler(e);
        return 0;
    }
    catch (...)
    {
        PRINT(_T("GetRecordCount(): Unhandled Exception!"));
        return 0;
    }
}
*/
/*
// Handling Chunk
BOOL CjADO::GetChunk(const std::string &csColumn, IN OUT LPBYTE pBuffer)
{
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
		eHandler(csColumn, e);
	}
	catch (...)
	{
		PRINT(_T("TestBLOB(): Unhandled Exception!"));
	}

	m_bCollected &= bRet;

	return bRet;
}
*/
BOOL CjADO::GetChunk(const std::string& csColumn, OUT std::vector<BYTE>& vBuffer)
{
	const long CHUNK_SIZE = 1024;
	BOOL bRet = TRUE;

	try
	{
        long nSizeOfChunk = m_pRecordSet->GetFields()->GetItem(_variant_t(csColumn.c_str()))->GetActualSize();
        vBuffer.clear();
        vBuffer.reserve(nSizeOfChunk);
		if (nSizeOfChunk > 0)
		{
            _variant_t	vartBLOB;
            long ReadTotalSize = 0;
            char* pBuf = NULL;
            long ReadCount = 0;
            while (ReadTotalSize < nSizeOfChunk)
            {
                VariantInit(&vartBLOB);
			    vartBLOB = m_pRecordSet->GetFields()->GetItem(_variant_t(csColumn.c_str()))->GetChunk(CHUNK_SIZE);
                SafeArrayAccessData(vartBLOB.parray, (void**) &pBuf);
                ReadCount =vartBLOB.parray->rgsabound[0].cElements;
                size_t NewSize = vBuffer.size() + ReadCount;
                //vBuffer.resize(NewSize);
                //std::copy(pBuf, pBuf+ReadCount, vBuffer.end());
                for (long i=0; i<ReadCount; ++i)
                    vBuffer.push_back(*(pBuf+i));

                SafeArrayUnaccessData(vartBLOB.parray);
                ReadTotalSize += ReadCount;	
            }
		}
		else
		{
			bRet = FALSE;
		}
	}
	catch (_com_error& e)
	{
		eHandler(csColumn, e);
	}
	catch (...)
	{
		PRINT(_T("TestBLOB(): Unhandled Exception!"));
	}

	m_bCollected &= bRet;

	return bRet;
}
/*
BOOL CjADO::GetChunk(LPCTSTR pColumn, IN OUT LPBYTE pBuffer)
{
	if (!pColumn)
	{
		PRINT(_T("GetChunk(): parameter pColumn is NULL."));
		return FALSE;
	}
	std::string csColumn(pColumn);
	return GetChunk(csColumn, pBuffer);
}
*/
/*
BOOL CjADO::GetChunk(LPCTSTR pColumn, OUT std::vector<BYTE>& vBuffer)
{
    if (!pColumn)
    {
        PRINT(_T("GetChunk(): parameter pColumn is NULL."));
        return FALSE;
    }
    std::string csColumn(pColumn);
    return GetChunk(csColumn, vBuffer);
}
*/
/*
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
*/
//
// Tool Functions
//

BOOL CjADO::PRINT(const std::string& csMsg)
{
    if (csMsg.empty())
        return FALSE;

	if (m_pfPrintMsg)		
	    m_pfPrintMsg(csMsg);
    else
        sc::LogMan::getInstance()->write(csMsg, sc::LOG_ERROR);
	return TRUE;
}


BOOL CjADO::PRINT(LPCTSTR pMsg)
{
    if (!pMsg)
        return FALSE;

    std::string Msg(pMsg);
    if (m_pfPrintMsg)
	    m_pfPrintMsg(Msg);
    else
        sc::LogMan::getInstance()->write(Msg, sc::LOG_ERROR);
	return TRUE;
}

HRESULT CjADO::eHandler( const std::string& Caller, _com_error e, bool bSimple /*= false*/ )
{
	_bstr_t bstrSource( e.Source() );
	_bstr_t bstrDescription( e.Description() );

	if ( bSimple )
	{
		std::string csMsg( "SetConnectionString error" );
		TRACE( csMsg.c_str() );
		PRINT( csMsg );
		return E_FAIL;
	}
	else
	{
		std::string csMsg(
			sc::string::format(
			"caller:%s source:%s desc:%s error: %08lx msg:%s",
			Caller.c_str(),
			bstrSource.length() ? ( LPCTSTR )bstrSource : NULL_STRING,
			bstrDescription.length() ? ( LPCTSTR )bstrDescription : NULL_STRING,
			e.Error(),
			( LPCTSTR )e.ErrorMessage() ) );
		TRACE( csMsg.c_str() );
		PRINT( csMsg );
		return e.Error();
	}
}

void CjADO::eHandler(const std::string& Caller, ErrorsPtr errors)
{
	if ( NULL != errors )
	{
		std::string csMsg( _T( "" ) );

		register long cntErrors = errors->GetCount();
		register long nIndex = 0;

		for ( long i=0; i< cntErrors; i++ )
		{
			long	ErrorNum;

			_bstr_t Description;
			_bstr_t Source;
			_bstr_t SqlState;		

			ErrorPtr ep = errors->Item[nIndex];

			ErrorNum = ep->GetNumber();

			if ( !ErrorNum )
				continue;

			Description	= ep->GetDescription();
			Source		= ep->GetSource();
			SqlState	= ep->GetSQLState();


			csMsg = sc::string::format(
				"%1% %2% (%3%)%4% - %5%",
				Caller,
				( LPCTSTR ) Source,
				ErrorNum,
				( LPCTSTR ) SqlState,
				( LPCTSTR ) Description);
			TRACE( csMsg.c_str() );
			PRINT( csMsg );
		}
		errors->Clear();
	}
}

//! int / Input Parameter
void CjADO::AppendIParamInteger(const std::string& name, int var)
{
    AppendParam((_bstr_t)(name.c_str()), adInteger, adParamInput, _variant_t(var), sizeof(int));
}

//! int / Output Parameter
void CjADO::AppendOParamInteger(const std::string& name)
{
    AppendParam((_bstr_t)(name.c_str()), adInteger, adParamOutput, _variant_t(NULL), sizeof(int));
}

//! int / Return Parameter
void CjADO::AppendRParamInteger()
{
    AppendParam((_bstr_t)("RETURN_VALUE"), adInteger, adParamReturnValue, _variant_t(NULL), sizeof(int));
}

//! __int64 <-> bigint / Input Parameter
void CjADO::AppendIParamBigint(const std::string& name, __int64 var)
{
    AppendParam((_bstr_t)(name.c_str()), adBigInt, adParamInput, _variant_t(var), sizeof(__int64));
}

//! __int64 <-> bigint / Output Parameter
void CjADO::AppendOParamBigint(const std::string& name)
{
    AppendParam((_bstr_t)(name.c_str()), adBigInt, adParamOutput, _variant_t(NULL), sizeof(__int64));
}

//! __int64 <-> bigint / Return Parameter
void CjADO::AppendRParamBigint()
{
    AppendParam((_bstr_t)("RETURN_VALUE"), adBigInt, adParamReturnValue, _variant_t(NULL), sizeof(__int64));
}

//! __int64 LONGLONG <-> Money
void CjADO::AppendIParamMoney(const std::string& name, __int64 var)
{
    var = sc::db::AdjustMoney(var);
    AppendParam((_bstr_t)(name.c_str()), adCurrency, adParamInput, _variant_t(var), sizeof(LONGLONG));
}

void CjADO::AppendOParamMoney(const std::string& name)
{
    AppendParam((_bstr_t)(name.c_str()), adCurrency, adParamOutput, _variant_t(NULL), sizeof(LONGLONG));
}

void CjADO::AppendRParamMoney()
{
    AppendParam((_bstr_t)("RETURN_VALUE"), adCurrency, adParamReturnValue, _variant_t(NULL), sizeof(LONGLONG));
}

//! GUID
void CjADO::AppendOParamGuid(const std::string& name)
{
    AppendParam((_bstr_t)(name.c_str()), adGUID, adParamOutput, _variant_t(NULL), sizeof(GUID));
}

void CjADO::AppendRParamGuid()
{
    AppendParam((_bstr_t)("RETURN_VALUE"), adGUID, adParamReturnValue, _variant_t(NULL), sizeof(GUID));
}

//! BYTE <-> 
void CjADO::AppendIParamTinyint(const std::string& name, BYTE var)
{
    AppendParam((_bstr_t)(name.c_str()), adTinyInt, adParamInput, _variant_t(var), sizeof(BYTE));
}

void CjADO::AppendOParamTinyint(const std::string& name)
{
    AppendParam((_bstr_t)(name.c_str()), adTinyInt, adParamOutput, _variant_t(NULL), sizeof(BYTE));
}

void CjADO::AppendRParamTinyint()
{
    AppendParam((_bstr_t)("RETURN_VALUE"), adTinyInt, adParamReturnValue, _variant_t(NULL), sizeof(BYTE));
}

//! Unsigned short, smallint
void CjADO::AppendIParamSmall(const std::string& name, unsigned short var)
{
    AppendParam((_bstr_t)(name.c_str()), adSmallInt, adParamInput, _variant_t(var), sizeof(USHORT));
}

void CjADO::AppendOParamSmall(const std::string& name)
{
    AppendParam((_bstr_t)(name.c_str()), adSmallInt, adParamOutput,_variant_t(NULL), sizeof(USHORT));
}

void CjADO::AppendRParamSmall()
{
    AppendParam((_bstr_t)("RETURN_VALUE"), adSmallInt, adParamReturnValue, _variant_t(NULL), sizeof(USHORT));
}

//! varchar
void CjADO::AppendIParamVarchar(const std::string& name, const char* var, long size)
{
    AppendParam((_bstr_t)(name.c_str()), adVarChar, adParamInput, _variant_t(var), size);
}

void CjADO::AppendOParamVarchar(const std::string& name/*, size*/)
{
    AppendParam((_bstr_t)(name.c_str()), adVarChar, adParamOutput, _variant_t(NULL), 0);//size)
}

void CjADO::AppendRParamVarchar(/*, size*/)
{
    AppendParam((_bstr_t)("RETURN_VALUE"), adVarChar, adParamReturnValue, _variant_t(NULL), 0); //size)
}

//! Binary
void CjADO::AppendIParamImage(const std::string& name, _variant_t& var, long size)
{
    AppendParam((_bstr_t)(name.c_str()), adLongVarBinary, adParamInput, var, size);
}

void CjADO::AppendOParamImage(const std::string& name/*, size*/)
{
    AppendParam((_bstr_t)(name.c_str()), adLongVarBinary, adParamOutput, _variant_t(NULL), 0);//size)
}

void CjADO::AppendRParamImage(/*, size*/)
{
    AppendParam((_bstr_t)("RETURN_VALUE"), adLongVarBinary, adParamReturnValue, _variant_t(NULL), 0);//size)
}

//! float
void CjADO::AppendIParamFloat(const std::string& name, float var)
{
    AppendParam((_bstr_t)(name.c_str()), adDouble, adParamInput, _variant_t(var), sizeof(float));
}

void CjADO::AppendOParamFloat(const std::string& name)
{
    AppendParam((_bstr_t)(name.c_str()), adDouble, adParamOutput, _variant_t(NULL), sizeof(float));
}

void CjADO::AppendRParamFloat()
{
    AppendParam((_bstr_t)("RETURN_VALUE"), adDouble, adParamReturnValue, _variant_t(NULL), sizeof(float));
}

// ----------------------------------------------------------------------------
    } // namespace db
} // namespace sc
    