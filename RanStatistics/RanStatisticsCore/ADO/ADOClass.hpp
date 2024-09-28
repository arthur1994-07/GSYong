//
//mjeon.ado.wrapping class
// 2010.12.29
//

typedef void ( *PF_PRINT )(const CString &);


class CjADO {
public:
	CjADO(PF_PRINT pf = NULL);
	CjADO(CString &csConn, PF_PRINT pf = NULL);
	CjADO(LPCTSTR szConn, PF_PRINT pf = NULL);
	~CjADO();

	BOOL Open(CString &csConn);
	BOOL Open(LPCTSTR szConn);
	BOOL IsOpened();
	BOOL Execute(CString &csQuery);
	void Close();


	template <class T>	BOOL GetCollect(CString &csColumn, T &var);
	template <>			BOOL GetCollect(CString &csColumn, CString &var);	

	template <class T>	BOOL GetCollect(LPCTSTR szColumn, T &var);
	//template <>			BOOL GetCollect(LPCTSTR szColumn, CString &var);

	template <class T>	BOOL GetCollect(int idx, T &var);
	template <>			BOOL GetCollect(int idx, CString &var);
		

	BOOL MoveFirst();
	BOOL MoveNext();
	BOOL MovePrevious();
	BOOL MoveLast();
	BOOL Move(long nIndex);

	BOOL GetBOF();
	BOOL GetEOF();

	BOOL Next();	// MoveNext() + GetEOF()


	void OpenRecordSet();
	void CloseRecordSet();
	
	//
	// MUST call this method to handle error messages
	//
	void SetPrintMsg(PF_PRINT p) { m_pfPrintMsg = p; }

	BOOL PRINT(CString &csMsg);
	BOOL PRINT(LPCTSTR pMsg);

	void eHandler( ErrorsPtr errors );
	void eHandler( _com_error e);

private:

	BOOL			m_bConnected;

	_ConnectionPtr	m_pConnection;
	_RecordsetPtr	m_pRecordSet;

	_bstr_t			m_strConn;
	
	PF_PRINT		m_pfPrintMsg;
};


inline void CHECK_HRESULT(HRESULT x) {if FAILED(x) _com_issue_error(x);};



//
// mjeon
// Template functions MUST be implemented in header file (in same transaltion unit).
//

template <class T>
BOOL CjADO::GetCollect(CString &csColumn, T &var)
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

	return bRet;
}


template <>
BOOL CjADO::GetCollect(CString &csColumn, CString &var)
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

	return bRet;
}


template <class T>
BOOL CjADO::GetCollect(LPCTSTR szColumn, T &var)
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
BOOL CjADO::GetCollect(int idx, T &var)
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

	return bRet;
}


template <>
BOOL CjADO::GetCollect(int idx, CString &var)
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

	return bRet;
}


