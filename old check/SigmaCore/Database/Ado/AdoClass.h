#ifndef _SC_ADO_CLASS_H_
#define _SC_ADO_CLASS_H_

#pragma once

#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

//ADO
// Windows Server 2003 R2에서 가지고 온 msado27.tlb로 replace해두었음. (KB983246 이슈)
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" rename ("EOF", "adoEOF") no_namespace

typedef void ( *PF_PRINT )(const std::string &);

#define REOPEN(ado)						ado.ReOpen()

#define EXECUTE_RAW(ado, query)			ado.Execute(query)						// Raw Queries
#define EXECUTE_SP(ado, query)			ado.Execute4Cmd(query, adCmdStoredProc)	// Stored Procedure
//#define EXECUTE_GETIMAGE(ado, query)	ado.Execute4RS(query)					// Get Binary-Images (it also can be used to get normal data-types)
#define EXECUTE_SETIMAGE(ado, query)	ado.Execute4Cmd(query)					// Set Binary-Images (it also can be used to set normal data-types)
#define EXECUTE_SETPARAM(ado, query)	ado.Execute4Cmd(query)					// Set Parameters for Normal Data-types with non-storedProcedure query
//#define EXECUTE_GETIMAGE_SP(ado, query)	ado.Execute4RS(query, adCmdStoredProc)	// Get Binary-Images using SP (it also can be used to get normal data-types)
#define EXECUTE_SETIMAGE_SP(ado, query)	ado.Execute4Cmd(query, adCmdStoredProc)	// Set Binary-Images using SP (it also can be used to set normal data-types)




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

//! See : http://www.w3schools.com/ADO/ado_datatypes.asp
//!       http://minwiki.mincoms.co.kr/index.php/ADO

//! int <-> int
#define APPEND_IPARAM_INT(ado, name, var)			\
    ado.AppendParam((_bstr_t)(name), adInteger, adParamInput, _variant_t(var), sizeof(int))
#define APPEND_OPARAM_INT(ado, name)				\
    ado.AppendParam((_bstr_t)(name), adInteger, adParamOutput, _variant_t(NULL), sizeof(int))
#define APPEND_RPARAM_INT(ado)						\
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adInteger, adParamReturnValue, _variant_t(NULL), sizeof(int))

//! __int64 <-> bigint
#define APPEND_IPARAM_BIGINT(ado, name, var)			\
    ado.AppendParam((_bstr_t)(name), adBigInt, adParamInput, _variant_t(var), sizeof(__int64))
#define APPEND_OPARAM_BIGINT(ado, name)				\
    ado.AppendParam((_bstr_t)(name), adBigInt, adParamOutput, _variant_t(NULL), sizeof(__int64))
#define APPEND_RPARAM_BIGINT(ado)						\
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adBigInt, adParamReturnValue, _variant_t(NULL), sizeof(__int64))

//! __int64 LONGLONG <-> Money
#define APPEND_IPARAM_MONEY(ado, name, var)          \
    ado.AppendParam((_bstr_t)(name), adCurrency, adParamInput, _variant_t(var), sizeof(LONGLONG))
#define APPEND_OPARAM_MONEY(ado, name)               \
    ado.AppendParam((_bstr_t)(name), adCurrency, adParamOutput, _variant_t(NULL), sizeof(LONGLONG))
#define APPEND_RPARAM_MONEY(ado)          \
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adCurrency, adParamReturnValue, _variant_t(NULL), sizeof(LONGLONG))

//! GUID
#define APPEND_IPARAM_GUID(ado, name, var) \
    ado.AppendParam((_bstr_t)(name), adGUID, adParamInput, _variant_t(var), sizeof(GUID))
#define APPEND_OPARAM_GUID(ado, name) \
    ado.AppendParam((_bstr_t)(name), adGUID, adParamOutput, _variant_t(NULL), sizeof(GUID))
#define APPEND_RPARAM_GUID(ado) \
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adGUID, adParamReturnValue, _variant_t(NULL), sizeof(GUID))

//! BYTE <-> 
#define APPEND_IPARAM_TINYINT(ado, name, var)		\
    ado.AppendParam((_bstr_t)(name), adTinyInt, adParamInput, _variant_t(var), sizeof(BYTE))
#define APPEND_OPARAM_TINYINT(ado, name)			\
    ado.AppendParam((_bstr_t)(name), adTinyInt, adParamOutput, _variant_t(NULL), sizeof(BYTE))
#define APPEND_RPARAM_TINYINT(ado)					\
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adTinyInt, adParamReturnValue, _variant_t(NULL), sizeof(BYTE))

//! Unsigned short, smallint
#define APPEND_IPARAM_SMALL(ado, name, var)			\
    ado.AppendParam((_bstr_t)(name), adSmallInt, adParamInput, _variant_t(var), sizeof(USHORT))
#define APPEND_OPARAM_SMALL(ado, name)				\
    ado.AppendParam((_bstr_t)(name), adSmallInt, adParamOutput,_variant_t(NULL), sizeof(USHORT))
#define APPEND_RPARAM_SMALL(ado)					\
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adSmallInt, adParamReturnValue, _variant_t(NULL), sizeof(USHORT))		

//! varchar
#define APPEND_IPARAM_VARCHAR(ado, name, var, size)	\
    ado.AppendParam((_bstr_t)(name), adVarChar, adParamInput, _variant_t(var), size)
#define APPEND_OPARAM_VARCHAR(ado, name/*, size*/)	\
    ado.AppendParam((_bstr_t)(name), adVarChar, adParamOutput, _variant_t(NULL), 0)//size)
#define APPEND_RPARAM_VARCHAR(ado /*, size*/)		\
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adVarChar, adParamReturnValue, _variant_t(NULL), 0)//size)

//! Binary
#define APPEND_IPARAM_IMAGE(ado, name, var, size)	\
    ado.AppendParam((_bstr_t)(name), adLongVarBinary, adParamInput, var, size)
#define APPEND_OPARAM_IMAGE(ado, name/*, size*/)	\
    ado.AppendParam((_bstr_t)(name), adLongVarBinary, adParamOutput, _variant_t(NULL), 0)//size)
#define APPEND_RPARAM_IMAGE(ado /*, size*/)			\
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adLongVarBinary, adParamReturnValue, _variant_t(NULL), 0)//size)

//! float
#define APPEND_IPARAM_FLOAT(ado, name, var)			\
    ado.AppendParam((_bstr_t)(name), adDouble, adParamInput, _variant_t(var), sizeof(float))
#define APPEND_OPARAM_FLOAT(ado, name)				\
    ado.AppendParam((_bstr_t)(name), adDouble, adParamOutput, _variant_t(NULL), sizeof(float))
#define APPEND_RPARAM_FLOAT(ado)						\
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adDouble, adParamReturnValue, _variant_t(NULL), sizeof(float))

//! Datetime 은 변환이 쉽지않음
//! 그냥 문자열로 변환해서 varchar 로 입력할것
//! 꼭 입력해야 되겠다면 SystemTimeToVariantTime 과 VariantTimeToSystemTime 을 이용해서 변환후 입력해야 함
/*
#define APPEND_IPARAM_DATETIME(ado, name, var)			\
    ado.AppendParam((_bstr_t)(name), adDBTimeStamp, adParamInput, _variant_t(var), sizeof(__time64_t))
#define APPEND_OPARAM_DATETIME(ado, name)				\
    ado.AppendParam((_bstr_t)(name), adDBTimeStamp, adParamOutput,_variant_t(NULL), sizeof(__time64_t))
#define APPEND_RPARAM_DATETIME(ado)					\
    ado.AppendParam((_bstr_t)("RETURN_VALUE"), adDBTimeStamp, adParamReturnValue, _variant_t(NULL), sizeof(__time64_t))
#define APPEND_IPARAM_DATETIME2(ado, name, var)			\
    ado.AppendParam((_bstr_t)(name), adDBTimeStamp, adParamInput, _variant_t(var), sizeof(DBTIMESTAMP))
*/
#define ADO_GET_COLUMN_VALUE(TEXT) \
    BOOL bRet = TRUE; \
    _variant_t	vData; \
    try \
    { \
        vData = m_pRecordSet->GetCollect(csColumn.c_str()); \
        if (vData.vt == VT_NULL) \
        { \
            var = 0; \
            bRet = FALSE; \
        } \
        else \
        { \
            TEXT; \
        } \
    } \
    catch (_com_error &e) \
    { \
        eHandler(csColumn, e); \
    } \
    catch (...) \
    { \
        PRINT(_T("GetCollect(): Unhandled Exception!")); \
    } \
    m_bCollected &= bRet; \
    return bRet; \

#define ADO_GET_COLUMN_BY_INDEX(TEXT) \
    BOOL bRet = TRUE; \
    _variant_t	vData; \
    try \
    { \
        vData = m_pRecordSet->GetCollect(_variant_t((LONG) idx)); \
        if (vData.vt == VT_NULL) \
        { \
            var = 0; \
            bRet = FALSE; \
        } \
        else \
        { \
            TEXT; \
        } \
    } \
    catch (_com_error &e) \
    { \
        eHandler("ADO_GET_COLUMN_BY_INDEX", e); \
    } \
    catch (...) \
    { \
        PRINT(_T("GetCollect(): Unhandled Exception!")); \
    } \
    m_bCollected &= bRet; \
    return bRet; \

namespace sc {
    namespace db {
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

class CjADO
{
public:
	CjADO(PF_PRINT pf=NULL);
	CjADO(const std::string& csConn, PF_PRINT pf=NULL);
	CjADO(LPCTSTR szConn, PF_PRINT pf=NULL);
	virtual ~CjADO();

protected:
    BOOL			m_bConnected;
    BOOL			m_bCollected;

    _ConnectionPtr	m_pConnection;
    _CommandPtr		m_pCommand;
    _RecordsetPtr	m_pRecordSet;

    _bstr_t			m_strConn;

    PF_PRINT		m_pfPrintMsg;
    std::string m_ProductVersion;

public:
	BOOL Open(const std::string &csConn);
	BOOL Open(LPCTSTR szConn);
	BOOL IsOpened();
	BOOL ReOpen();
    
    std::string GetProductVersion() const { return m_ProductVersion; }

	BOOL Execute(const std::string &csQuery);
	BOOL Execute4Cmd(const std::string &csQuery, CommandTypeEnum emCommandType = adCmdText);
    BOOL ExecuteStoredProcedure(const std::string& StoredProcedureName);
    int ExecuteStoredProcedureIntReturn(const std::string& SpName, const std::string& OutPutParamName);
    LONGLONG ExecuteStoredProcedureLonglongReturn(const std::string& SpName, const std::string& OutPutParamName);
	//BOOL Execute4RS(const std::string &csQuery, CommandTypeEnum emCommandType = adCmdText, CursorTypeEnum emCursorType = adOpenForwardOnly, LockTypeEnum emLockType = adLockReadOnly);

	HRESULT Execute4CmdNew( const std::string &csQuery, CommandTypeEnum emCommandType = adCmdText );
	HRESULT ExecuteStoredProcedureNew( const std::string& StoredProcedureName );

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
    template <class T>
    BOOL GetCollect(const std::string &csColumn, OUT T &var)
    {
        BOOL bRet = TRUE;
        _variant_t	vData;
        try
        {
            vData = m_pRecordSet->GetCollect(csColumn.c_str());
            if (vData.vt == VT_NULL)
            {
                var = 0;
                bRet = FALSE;
            }
            else
            {
                var = static_cast<T> (vData);
            }
        }
        catch (_com_error &e)
        {
            eHandler(csColumn, e);
        }
        catch (...)
        {
            PRINT("GetCollect(): Unhandled Exception!");
        }
        m_bCollected &= bRet;
        return bRet;
    }
/*
    template <>
    BOOL GetCollect(const std::string &csColumn, OUT WORD& var)
    {
        ADO_GET_COLUMN_VALUE(var = static_cast<WORD> (vData));
    }

    template <>
    BOOL GetCollect(const std::string &csColumn, OUT DWORD& var)
    {
        ADO_GET_COLUMN_VALUE(var = static_cast<DWORD> (vData));
    }

    template <>
    BOOL GetCollect(const std::string &csColumn, OUT SHORT& var)
    {
        ADO_GET_COLUMN_VALUE(var = static_cast<SHORT> (vData));
    }

    template <>
    BOOL GetCollect(const std::string &csColumn, OUT int& var)
    {
        ADO_GET_COLUMN_VALUE(var = static_cast<int> (vData));
    }

    template <>
    BOOL GetCollect(const std::string &csColumn, OUT float& var)
    {
        ADO_GET_COLUMN_VALUE(var = static_cast<float> (vData));
    }

    template <>
    BOOL GetCollect(const std::string &csColumn, OUT __int64& var)
    {
        ADO_GET_COLUMN_VALUE(var = static_cast<__int64> (vData));
    }
*/
    template <>
    BOOL GetCollect(const std::string &csColumn, OUT _variant_t& var)
    {
        ADO_GET_COLUMN_VALUE(var = vData);
    }

    template <>
    BOOL GetCollect(const std::string &csColumn, OUT std::string &var)
    {        
        BOOL bRet = TRUE;	// is NOT VT_NULL ?
        _variant_t	vData;
        try
        {
            vData = m_pRecordSet->GetCollect(csColumn.c_str());			
            if (vData.vt == VT_NULL)
                var = "";
            else
                var = (LPCTSTR)(_bstr_t)vData;
        }
        catch( _com_error &e )
        {
            eHandler(csColumn, e);
        }
        catch (...)
        {
            PRINT(_T("GetCollect(): Unhandled Exception!"));
        }
        m_bCollected &= bRet;
        return bRet;
    }

    template <class T>
    BOOL GetCollect(LPCTSTR szColumn, OUT T& var)
    {
        std::string csColumn(szColumn);	
        return GetCollect(csColumn, var);
    }

    template <>
    BOOL GetCollect(LPCTSTR szColumn, std::string& var)
    {
        std::string csColumn(szColumn);
        return GetCollect(csColumn, var);
    }


	template <class T>
    BOOL GetCollect(int idx, OUT T& var)
    {
        BOOL bRet = TRUE;
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
            {
                var = static_cast<T> (vData);
            }
        }
        catch (_com_error &e)
        {
            eHandler("ADO_GET_COLUMN_BY_INDEX", e);
        }
        catch (...)
        {
            PRINT("GetCollect(): Unhandled Exception!");
        }
        m_bCollected &= bRet;
        return bRet;
    }
/*
    template <>
    BOOL GetCollect(int idx, OUT WORD& var)
    {
        ADO_GET_COLUMN_BY_INDEX(var = static_cast<WORD> (vData));
    }

    template <>
    BOOL GetCollect(int idx, OUT DWORD& var)
    {
        ADO_GET_COLUMN_BY_INDEX(var = static_cast<DWORD> (vData));
    }

    template <>
    BOOL GetCollect(int idx, OUT SHORT& var)
    {
        ADO_GET_COLUMN_BY_INDEX(var = static_cast<SHORT> (vData));
    }

    template <>
    BOOL GetCollect(int idx, OUT int& var)
    {
        ADO_GET_COLUMN_BY_INDEX(var = static_cast<int> (vData));
    }

    template <>
    BOOL GetCollect(int idx, OUT float& var)
    {
        ADO_GET_COLUMN_BY_INDEX(var = static_cast<float> (vData));
    }

    template <>
    BOOL GetCollect(int idx, OUT __int64& var)
    {
        ADO_GET_COLUMN_BY_INDEX(var = static_cast<__int64> (vData));
    }
*/  
    template <>
    BOOL GetCollect(int idx, OUT _variant_t& var)
    {
        ADO_GET_COLUMN_BY_INDEX(var = vData);
    }
        
    template <>
    BOOL GetCollect(int idx, OUT std::string &var)
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
            eHandler("GetCollect", e);
        }
        catch (...)
        {
            PRINT(_T("GetCollect(): Unhandled Exception!"));
        }
        m_bCollected &= bRet;
        return bRet;
    }
    
	//BOOL GetChunk(const std::string& csColumn, OUT LPBYTE pBuffer);
    BOOL GetChunk(const std::string& csColumn, OUT std::vector<BYTE>& vBuffer);
	//BOOL GetChunk(LPCTSTR pColumn,	           OUT LPBYTE pBuffer);
    //BOOL GetChunk(LPCTSTR pColumn,	           OUT std::vector<BYTE>& vBuffer);

	BOOL MoveFirst();
	BOOL MoveNext();
	BOOL MovePrevious();
	BOOL MoveLast();
	BOOL Move(long nIndex);

	BOOL GetBOF();
	BOOL GetEOF();
    
    //long GetRecordCount();

	BOOL Next();	// MoveNext() + GetEOF()

	BOOL IsCollectedAll() {return m_bCollected;}


	void OpenRecordSet();
	void CloseRecordSet();
	//
	//=========================================================================


	//
	// Make a _variant_t variable inldues a Chunk data inside.
	//
	//BOOL MakeChunk(OUT _variant_t &vartBLOB, IN LPBYTE pData,				 IN ULONG &nSizeOfData);
	
	//
	// SHOULD call this method to handle error messages
	//
	void SetPrintMsg(PF_PRINT p)
	{
		m_pfPrintMsg = p;
	}
	
	BOOL PRINT(const std::string &csMsg);
	BOOL PRINT(LPCTSTR pMsg);

    void eHandler(const std::string& Caller, ErrorsPtr errors);
	HRESULT eHandler(const std::string& Caller, _com_error e, bool bSimple = false);

    // ------------------------------------------------------------------------
    //! int <-> int / Input Parameter / Output Parameter / Return Parameter
    void AppendIParamInteger(const std::string& name, int var);
    void AppendOParamInteger(const std::string& name);
    void AppendRParamInteger();

    //! __int64 <-> bigint / Input Parameter / Output Parameter / Return Parameter
    void AppendIParamBigint(const std::string& name, __int64 var);
    void AppendOParamBigint(const std::string& name);
    void AppendRParamBigint();

    //! __int64 LONGLONG <-> Money / Input Parameter / Output Parameter / Return Parameter
    void AppendIParamMoney(const std::string& name, __int64 var);
    void AppendOParamMoney(const std::string& name);
    void AppendRParamMoney();

    //! GUID / Input Parameter / Output Parameter / Return Parameter
    void AppendOParamGuid(const std::string& name);
    void AppendRParamGuid();

    //! BYTE <-> / Input Parameter / Output Parameter / Return Parameter
    void AppendIParamTinyint(const std::string& name, BYTE var);
    void AppendOParamTinyint(const std::string& name);
    void AppendRParamTinyint();

    //! Unsigned short, smallint / Input Parameter / Output Parameter / Return Parameter
    void AppendIParamSmall(const std::string& name, unsigned short var);
    void AppendOParamSmall(const std::string& name);
    void AppendRParamSmall();

    //! varchar / Input Parameter / Output Parameter / Return Parameter
    void AppendIParamVarchar(const std::string& name, const char* var, long size);
    void AppendOParamVarchar(const std::string& name/*, size*/);
    void AppendRParamVarchar(/*, size*/);

    //! Binary / Input Parameter / Output Parameter / Return Parameter
    void AppendIParamImage(const std::string& name, _variant_t& var, long size);
    void AppendOParamImage(const std::string& name/*, size*/);
    void AppendRParamImage(/*, size*/);

    //! float / Input Parameter / Output Parameter / Return Parameter
    void AppendIParamFloat(const std::string& name, float var);
    void AppendOParamFloat(const std::string& name);
    void AppendRParamFloat();
};


inline void CHECK_HRESULT(HRESULT x) {if FAILED(x) _com_issue_error(x);};
    } // namespace db
} // namespace sc
#endif // _SC_ADO_CLASS_H_