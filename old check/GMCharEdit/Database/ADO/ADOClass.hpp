//
//mjeon.ado.wrapping class
// 2010.12.29
//
#if _MSC_VER > 1000
#pragma once
#endif

#include "../../../enginelib/Common/ByteStream.h"


typedef void ( *PF_PRINT )(const CString &);

#define REOPEN(ado)						ado.ReOpen()


#define EXECUTE_RAW(ado, query)			ado.Execute4Cmd(query)					// Raw Queries
#define EXECUTE_SP(ado, query)			ado.Execute4Cmd(query, adCmdStoredProc)	// Stored Procedure

#define EXECUTE_GETIMAGE(ado, query)	ado.Execute4Cmd(query)					// Get Binary-Images (it also can be used to get normal data-types)
#define EXECUTE_SETIMAGE(ado, query)	ado.Execute4Cmd(query)					// Set Binary-Images (it also can be used to set normal data-types)
#define EXECUTE_SETPARAM(ado, query)	ado.Execute4Cmd(query)					// Set Parameters for Normal Data-types with non-storedProcedure query

#define EXECUTE_GETIMAGE_SP(ado, query)	ado.Execute4Cmd(query, adCmdStoredProc)	// Set Binary-Images using SP (it also can be used to set normal data-types)
#define EXECUTE_SETIMAGE_SP(ado, query)	ado.Execute4Cmd(query, adCmdStoredProc)// Set Binary-Images using SP (it also can be used to set normal data-types)


/*
	참고: 아래와 같은 방식들도 가능하다.
	단, EXECUTE_GETIMAGE_SP의 경우는 SP로 parameter가 전달되지 않는다.

#define EXECUTE_RAW(ado, query)			ado.Execute(query)						// Raw Queries
#define EXECUTE_GETIMAGE(ado, query)	ado.Execute4RS(query)					// Get Binary-Images (it also can be used to get normal data-types)
#define EXECUTE_GETIMAGE_SP(ado, query)	ado.Execute4RS(query, adCmdStoredProc)	// Get Binary-Images using SP (it also can be used to get normal data-types)
*/


#define BEGIN_GETCOLLECT(ado)										\
	if ( ado.GetEOF() )												\
	{																\
		return FALSE;												\
	}																\
	else															\
	{																\
		do															\
		{


#define END_GETCOLLECT(ado)											\
		}															\
		while ( ado.Next() );										\
	}
	



#define BEGIN_GETCHUNK(ado)		BEGIN_GETCOLLECT(ado)
#define END_GETCHUNK(ado)		END_GETCOLLECT(ado)



//AppendParam(_bstr_t name, DataTypeEnum &type, ParameterDirectionEnum &direction, _variant_t &var, long size)

#define APPEND_IPARAM_INT(ado, name, var)			\
			ado.AppendParam((_bstr_t)(name), adInteger, adParamInput, _variant_t(var), sizeof(int))
#define APPEND_OPARAM_INT(ado, name)				\
			ado.AppendParam((_bstr_t)(name), adInteger, adParamOutput, _variant_t(NULL), sizeof(int))
#define APPEND_RPARAM_INT(ado)						\
			ado.AppendParam((_bstr_t)("RETURN_VALUE"), adInteger, adParamReturnValue, _variant_t(NULL), sizeof(int))

#define APPEND_IPARAM_TINYINT(ado, name, var)		\
			ado.AppendParam((_bstr_t)(name), adTinyInt, adParamInput, _variant_t(var), sizeof(BYTE))
#define APPEND_OPARAM_TINYINT(ado, name)			\
			ado.AppendParam((_bstr_t)(name), adTinyInt, adParamOutput, _variant_t(NULL), sizeof(BYTE))
#define APPEND_RPARAM_TINYINT(ado)					\
			ado.AppendParam((_bstr_t)("RETURN_VALUE"), adTinyInt, adParamReturnValue, _variant_t(NULL), sizeof(BYTE))

#define APPEND_IPARAM_SMALL(ado, name, var)			\
			ado.AppendParam((_bstr_t)(name), adSmallInt, adParamInput, _variant_t(var), sizeof(USHORT))
#define APPEND_OPARAM_SMALL(ado, name)				\
			ado.AppendParam((_bstr_t)(name), adSmallInt, adParamOutput,_variant_t(NULL), sizeof(USHORT))
#define APPEND_RPARAM_SMALL(ado)					\
			ado.AppendParam((_bstr_t)("RETURN_VALUE"), adSmallInt, adParamReturnValue, _variant_t(NULL), sizeof(USHORT))		

#define APPEND_IPARAM_VARCHAR(ado, name, var, size)	\
			ado.AppendParam((_bstr_t)(name), adVarChar, adParamInput, _variant_t(var), size)
#define APPEND_OPARAM_VARCHAR(ado, name/*, size*/)	\
			ado.AppendParam((_bstr_t)(name), adVarChar, adParamOutput, _variant_t(NULL), 0)//size)
#define APPEND_RPARAM_VARCHAR(ado /*, size*/)		\
			ado.AppendParam((_bstr_t)("RETURN_VALUE"), adVarChar, adParamReturnValue, _variant_t(NULL), 0)//size)

#define APPEND_IPARAM_IMAGE(ado, name, var, size)	\
			ado.AppendParam((_bstr_t)(name), adLongVarBinary, adParamInput, var, size)
#define APPEND_OPARAM_IMAGE(ado, name/*, size*/)	\
			ado.AppendParam((_bstr_t)(name), adLongVarBinary, adParamOutput, _variant_t(NULL), 0)//size)
#define APPEND_RPARAM_IMAGE(ado /*, size*/)			\
			ado.AppendParam((_bstr_t)("RETURN_VALUE"), adLongVarBinary, adParamReturnValue, _variant_t(NULL), 0)//size)

//! __int64 LONGLONG <-> Money
#define APPEND_IPARAM_MONEY(ado, name, var)          \
    ado.AppendParam((_bstr_t)(name), adCurrency, adParamInput, _variant_t(var), sizeof(LONGLONG))
#define APPEND_OPARAM_MONEY(ado, name)               \
    ado.AppendParam((_bstr_t)(name), adCurrency, adParamOutput, _variant_t(NULL), sizeof(LONGLONG))
#define APPEND_RPARAM_MONEY(ado)          \
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adCurrency, adParamReturnValue, _variant_t(NULL), sizeof(LONGLONG))

//! float
#define APPEND_IPARAM_FLOAT(ado, name, var)			\
    ado.AppendParam((_bstr_t)(name), adDouble, adParamInput, _variant_t(var), sizeof(float))
#define APPEND_OPARAM_FLOAT(ado, name)				\
    ado.AppendParam((_bstr_t)(name), adDouble, adParamOutput, _variant_t(NULL), sizeof(float))
#define APPEND_RPARAM_FLOAT(ado)						\
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adDouble, adParamReturnValue, _variant_t(NULL), sizeof(float))



/*

ADO connection pooling tips

The ADO Connection object implicitly uses IDataInitialize.
However, this means your application needs to keep at least one instance of a Connection object 
instantiated for each unique user-at all times. 
Otherwise, the pool will be destroyed when the last Connection object for that string is closed.

(The exception to this is if you use Microsoft Transaction Server. 
In this case, the pool is destroyed only if all of the connections in the pool have been closed 
by client applications and are allowed to time out.)

*/

//
// In current implementation, the connection pooling is explictly controlled by
//  CADOManager::EnableConnectionPooling().
//




//
//mjeon
// One of the purpose of this class is that showing how-to implement ADO codes in various ways.
// Even though the result is same, various styles of implementation are taken.
// Therefore, it would be possible to optimize your own ADO code with a specific style.
//

class CjADO {
public:
	CjADO(PF_PRINT pf = NULL);
	CjADO(CString &csConn, PF_PRINT pf = NULL);
	CjADO(LPCTSTR szConn, PF_PRINT pf = NULL);
	virtual ~CjADO();


	BOOL Open(CString &csConn);
	BOOL Open(LPCTSTR szConn);
	BOOL IsOpened();
	BOOL ReOpen();
	
    BOOL Execute(const std::string& Query);
	BOOL Execute(CString &csQuery);
	BOOL Execute4Cmd(CString &csQuery, CommandTypeEnum emCommandType = adCmdText);
	BOOL Execute4RS(CString &csQuery, CommandTypeEnum emCommandType = adCmdText, CursorTypeEnum emCursorType = adOpenForwardOnly, LockTypeEnum emLockType = adLockReadOnly);


	void Close();

	_RecordsetPtr GetRecordSet() { return m_pRecordSet; }




	//----------------------------- Command methods ---------------------------
	//	
	BOOL AppendParam(_bstr_t name, DataTypeEnum type, ParameterDirectionEnum direction, IN _variant_t &var, long size);
	BOOL GetParam(_bstr_t name, OUT _variant_t &var);
	//
	//=========================================================================

	


	//---------------------------- RecordSet methods --------------------------
	//
	template <class T>	BOOL GetCollect(CString &csColumn, OUT T &var);
	template <>			BOOL GetCollect(CString &csColumn, OUT CString &var);	

	template <class T>	BOOL GetCollect(LPCTSTR szColumn,  OUT T &var);
	//template <>			BOOL GetCollect(LPCTSTR szColumn, CString &var);

	template <class T>	BOOL GetCollect(int idx, OUT T &var);
	template <>			BOOL GetCollect(int idx, OUT CString &var);


	BOOL GetChunk(CString &csColumn, OUT LPBYTE pBuffer);
	BOOL GetChunk(LPCTSTR pColumn,	 OUT LPBYTE pBuffer);

	BOOL GetChunk(CString &csColumn, OUT se::ByteStream &bytestream);
	BOOL GetChunk(LPCTSTR pColumn,	 OUT se::ByteStream &bytestream);


	BOOL MoveFirst();
	BOOL MoveNext();
	BOOL MovePrevious();
	BOOL MoveLast();
	BOOL Move(long nIndex);

	BOOL GetBOF();
	BOOL GetEOF();

	BOOL Next();	// MoveNext() + GetEOF()

	BOOL IsCollectedAll() {return m_bCollected;}


	void OpenRecordSet();
	void CloseRecordSet();
	//
	//=========================================================================


	//
	// Make a _variant_t variable inldues a Chunk data inside.
	//
	BOOL MakeChunk(OUT _variant_t &vartBLOB, IN LPBYTE pData,				 IN ULONG &nSizeOfData);
	BOOL MakeChunk(OUT _variant_t &vartBLOB, IN se::ByteStream &bytestream, OUT ULONG &nSizeOfData);
	
	
	//
	// SHOULD call this method to handle error messages
	//
	void SetPrintMsg(PF_PRINT p)
	{
		m_pfPrintMsg = p;
	}
	
	BOOL PRINT(CString &csMsg);
	BOOL PRINT(LPCTSTR pMsg);

	void eHandler( ErrorsPtr errors );
	void eHandler( _com_error e);

private:
	BOOL			m_bConnected;
	BOOL			m_bCollected;

	_ConnectionPtr	m_pConnection;
	_CommandPtr		m_pCommand;
	_RecordsetPtr	m_pRecordSet;

	_bstr_t			m_strConn;
	
	PF_PRINT		m_pfPrintMsg;
};


inline void CHECK_HRESULT(HRESULT x) {if FAILED(x) _com_issue_error(x);};



//
// mjeon
// Template functions MUST be implemented in header file (in same translation unit).
//


//----------------------- AppendParam templates -------------------------

/*
template <class T>
BOOL CjADO::AppendParam(CString csName, DataTypeEnum &type, ParameterDirectionEnum &dir, T var)
{
	_ParameterPtr param;
	
	param = m_pCommand->CreateParameter((_bstr_t)csName, 
		type, adParamInput, 128);

	_variant_t var_t(var);
	param->Value = var_t;


	m_pCommand->Parameters->Append(param);
}
*/




//----------------------- GetCollect templates --------------------------

template <class T>
BOOL CjADO::GetCollect(CString &csColumn, OUT T &var)
{
	BOOL bRet = TRUE;	// is NOT VT_NULL ? 

	_variant_t	vData;

	try
	{
		vData = m_pRecordSet->GetCollect(csColumn.GetBuffer());
		if (vData.vt == VT_NULL)
		{
			var = 0;
			bRet = FALSE;
		}
		else
			var = (T)vData;
	}
	catch( _com_error &e )
	{
		eHandler(e);
	}
	catch (...)
	{
		PRINT(_T("GetCollect(): Unhandled Exception!"));
	}

	m_bCollected &= bRet;

	return bRet;
}


template <>
BOOL CjADO::GetCollect(CString &csColumn, OUT CString &var)
{
	BOOL bRet = TRUE;	// is NOT VT_NULL ? 

	_variant_t	vData;

	try
	{
		vData = m_pRecordSet->GetCollect(csColumn.GetBuffer());			
		if (vData.vt == VT_NULL)
			var = "";
		else
			var = (LPCTSTR)(_bstr_t)vData;
	}
	catch( _com_error &e )
	{
		eHandler(e);
	}
	catch (...)
	{
		PRINT(_T("GetCollect(): Unhandled Exception!"));
	}

	m_bCollected &= bRet;

	return bRet;
}


template <class T>
BOOL CjADO::GetCollect(LPCTSTR szColumn, OUT T &var)
{
	CString csColumn(szColumn);
	
	return GetCollect(csColumn, var);
}

/*
template <>
BOOL CjADO::GetCollect(LPCTSTR szColumn, CString &var)
{
	CString csColumn(szColumn);

	return GetCollect(csColumn, var);
}
*/

template <class T>
BOOL CjADO::GetCollect(int idx, OUT T &var)
{
	BOOL bRet = TRUE;	// is NOT VT_NULL ? 

	_variant_t	vData;

	try
	{
		vData = m_pRecordSet->GetCollect(_variant_t((LONG) idx));
		if (vData.vt == VT_NULL)
		{
			var = 0;
			bRet = FALSE;
		}
		else
			var = (T)vData;
	}
	catch( _com_error &e )
	{
		eHandler(e);
	}
	catch (...)
	{
		PRINT(_T("GetCollect(): Unhandled Exception!"));
	}

	m_bCollected &= bRet;

	return bRet;
}


template <>
BOOL CjADO::GetCollect(int idx, OUT CString &var)
{
	BOOL bRet = TRUE;	// is NOT VT_NULL ? 

	_variant_t	vData;

	try
	{
		vData = m_pRecordSet->GetCollect(_variant_t((LONG) idx));			
		if (vData.vt == VT_NULL)
			var = "";
		else
			var = (LPCTSTR)(_bstr_t)vData;
	}
	catch( _com_error &e )
	{
		eHandler(e);
	}
	catch (...)
	{
		PRINT(_T("GetCollect(): Unhandled Exception!"));
	}

	m_bCollected &= bRet;

	return bRet;
}

