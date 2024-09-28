#include "../../pch.h"
#include "../../../RanLogic/Post/GLPostData.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../DBAction/DbAction.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"


#include "../../../SigmaCore/DebugInclude.h"


//
//mjeon.Post
//

int COdbcManager::PostMaintenance()
{
    sc::writeLogError("COdbcManager::PostMaintenance use ADO");
    return sc::db::DB_ERROR;

/*
	int nRowCount = 0;
	SQLRETURN sReturn = 0;	


	VPOSTID vPostID;
	ITER_VPOSTID ipid;


    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
		return sc::db::DB_ERROR;

	__time64_t currentTime = CTime::GetCurrentTime().GetTime();
	

	//Target Time
	__time64_t targetTime = currentTime - (__time64_t)GLPost::llPOST_EXPIRE_DURATION_SEC;
	CTime cTargetTime(targetTime);
	CString TargetDate = cTargetTime.Format("%Y-%m-%d %H:%M:%S");	


	TCHAR szQuery[512] = {0};

	// Returned Post cannot include  Chargement		

	if (m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG)
	{
		_snprintf( szQuery, 512, 
				"SELECT PostID FROM dbo.PostInfo "												\
					"WHERE DeleteDate IS NULL AND ReturnDate IS NULL AND SendDate < '%s' "	\
					//
					// in Taiwan, all posts includes attachment are returned.
					//
					"AND ( ( Attach01 IS NOT NULL AND Attach01Accept = 0 ) OR ( AttachMoney > 0 AND MoneyAccept = 0 ) ) "	\
					"ORDER BY PostID "
				, TargetDate.GetString() );
	}
	else
	{
		_snprintf( szQuery, 512,
			"SELECT PostID FROM dbo.PostInfo "												\
				"WHERE DeleteDate IS NULL AND ReturnDate IS NULL AND SendDate < '%s' "	\
				"AND Attach01 IS NOT NULL AND Attach01Accept = 0 "						\
				"AND Charge > 0 AND ChargePayment = 0 "									\
				"ORDER BY PostID "
				, TargetDate.GetString() );
	}

	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szQuery);		
		Print(GetErrorString(spConn->hStmt));

		return sc::db::DB_ERROR;
	}


	LONGLONG	llPostID = 0;		SQLINTEGER	cbPostID = SQL_NTS;


	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(szQuery);		
			Print(GetErrorString(spConn->hStmt));

			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data

			::SQLGetData(spConn->hStmt,  1, SQL_C_SBIGINT,	&llPostID,		0, &cbPostID);

			nRowCount++;
		}
		else
		{
			break;
		}		

		vPostID.push_back(llPostID);
	}

	// Return the charged POSTs before clean-up.
	
	for (ipid = vPostID.begin(); ipid != vPostID.end(); ipid++)
	{
		PostReturn(*ipid);
	}

	return sc::db::DB_OK;
*/
}



int COdbcManager::PostCleanUp(char *pszCurrentDate, char *pszTargetDate)
{
    sc::writeLogError("COdbcManager::PostCleanUp. Do not call this function. Use ADO");
    return sc::db::DB_ERROR;
    /*
	TCHAR szQuery[128] = {0};
	_snprintf( szQuery, 128, "{call sp_PostClean('%s','%s',?)}", pszCurrentDate, pszTargetDate );
	int nReturn = m_pGameDB->ExecuteSpInt(szQuery);
	return nReturn;
    */
}


int COdbcManager::PostReturn(LONGLONG llPostID)
{
    sc::writeLogError("Do not use COdbcManager::PostReturn. Use ADO.");
    return sc::db::DB_ERROR;
/*
	SYSTEMTIME st;
	GetLocalTime(&st);	
	CTime cTime(st);
	CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");

	TCHAR szQuery[128] = {0};	

	_snprintf( szQuery, 128, "{call sp_PostReturn(%I64d,%d,'%s',?)}", llPostID, (BYTE)EMPOST_RETURN, strTime.GetString());

	int nReturn = m_pGameDB->ExecuteSpInt(szQuery);

	return nReturn;
*/
}


int COdbcManager::PostDelete(LONGLONG llPostID)
{
    sc::writeLogError("COdbcManager::PostDelete. Use ADO");
    return sc::db::DB_ERROR;
/*
	SYSTEMTIME st;
	GetLocalTime(&st);	
	CTime cTime(st);	
	CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");

	TCHAR szQuery[128] = {0};	

	_snprintf( szQuery, 128, "{call sp_PostDelete(%I64d,%d,'%s',?)}", llPostID, (BYTE)EMPOST_USER_DELETE, strTime.GetString());

	int nReturn = m_pGameDB->ExecuteSpInt(szQuery);
	
	return nReturn;	
*/
}


int COdbcManager::PostOpenAndTake(LONGLONG llPostID, BOOL bMoneyAccept, BOOL bAttach01Accept, BYTE bChargePayment, BYTE bReturnedPost )
{
    sc::writeLogError("COdbcManager::PostOpenAndTake. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szQuery[256] = {0,};

	int nReturn = 0;

	SYSTEMTIME st;
	GetLocalTime(&st);	
	CTime cTime(st);	
	CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");


	_snprintf( szQuery, 256, "{call sp_PostOpenTake(%I64d,%d,%d, %d,'%s', %d, ?)}", llPostID, (BYTE)bMoneyAccept, (BYTE)bAttach01Accept, bChargePayment, strTime.GetString(), (BYTE)bReturnedPost );

	nReturn = m_pGameDB->ExecuteSpInt(szQuery);

	return nReturn;
*/
}


int COdbcManager::PostOpen(LONGLONG llPostID)
{
    sc::writeLogError("COdbcManager::PostOpen. Use ADO");
    return sc::db::DB_ERROR;
/*
	SYSTEMTIME st;
	GetLocalTime(&st);	
	CTime cTime(st);	
	CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");

	TCHAR szQuery[128] = {0};	

	_snprintf( szQuery, 128, "{call sp_PostOpen(%I64d,%d,'%s',?)}", llPostID, (BYTE)EMPOST_READ, strTime.GetString());

	int nReturn = m_pGameDB->ExecuteSpInt(szQuery);
	
	return nReturn;	
*/
}


int COdbcManager::PostGetInfo(int iRecverNum, int &cntAllPost, int &cntNewPost)
{
    sc::writeLogError("COdbcManager::PostGetInfo Do not use function. Use ADO");
    return sc::db::DB_ERROR;
    /*
	if ( iRecverNum < 0)
	{
		return sc::db::DB_ERROR;
	}


	int nRowCount = 0;

	SQLRETURN sReturn = 0;


    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
		return sc::db::DB_ERROR;

	__time64_t currentTime = CTime::GetCurrentTime().GetTime();
	currentTime -= (__time64_t)GLPost::llPOST_EXPIRE_DURATION_SEC;			
	CTime cTime(currentTime);
	
	CString strTime = cTime.Format("%Y-%m-%d 00:00:00");


	TCHAR szQuery[512] = {0};

	_snprintf( szQuery, 512, "SELECT COUNT(*), "									\
								"(SELECT COUNT(*) "									\
								"FROM PostInfo WITH(INDEX(IDX_PostRecver)) "		\
								"WHERE Recver = %d AND (PostState = %d OR PostState = %d) "	\
									"AND RecvBackDate IS NULL "						\
									"AND (SendDate >= '%s' OR ReturnDate >= '%s') "	\
								") "												\
							"FROM PostInfo WITH(INDEX(IDX_PostRecver)) "			\
							"WHERE Recver = %d "									\
								"AND DeleteDate IS NULL "							\
								"AND (SendDate >= '%s' OR ReturnDate >= '%s' ) "	\
			,iRecverNum, EMPOST_UNREAD, EMPOST_RETURN
			,strTime.GetString(), strTime.GetString()
			,iRecverNum
			,strTime.GetString(), strTime.GetString()
			);


	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szQuery);		
		Print(GetErrorString(spConn->hStmt));
		
		return sc::db::DB_ERROR;
	}

	SQLINTEGER	nAllPost = 0,	cbAllPost = SQL_NTS;
	SQLINTEGER	nNewPost = 0,	cbNewPost = SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(szQuery);		
			Print(GetErrorString(spConn->hStmt));
			
			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data

			::SQLGetData(spConn->hStmt, 1, SQL_C_LONG, &nAllPost, 0, &cbAllPost);
			::SQLGetData(spConn->hStmt, 2, SQL_C_LONG, &nNewPost, 0, &cbNewPost);

			nRowCount++;
		}
		else
		{
			break;
		}		
	}


	if (nRowCount > 1)		// It MUST be 0 or 1.
		return sc::db::DB_ERROR;

	if (cbAllPost != 0 && cbAllPost != -1)
		cntAllPost = nAllPost;

	if (cbNewPost != 0 && cbNewPost != -1)
		cntNewPost = nNewPost;


	return sc::db::DB_OK;
    */
}


int COdbcManager::PostGetData(int iRecverNum, VPOST &vPost, int &nRowCount, DWORD dwRecverUserNum)
{
    sc::writeLogError("COdbcManager::PostGetData must be use ADO");
    return sc::db::DB_ERROR;
/*
	if ( iRecverNum < 0)
	{
		return sc::db::DB_ERROR;
	}

	//Empty the vector for POSTs
	vPost.clear();

	SAPOST aPost;	// A post

	SQLRETURN sReturn = 0;
	//int nRowCount = 0;

    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
		return sc::db::DB_ERROR;


	__time64_t currentTime = CTime::GetCurrentTime().GetTime();
	currentTime -= (__time64_t)GLPost::llPOST_EXPIRE_DURATION_SEC;			
	CTime cTime(currentTime);	
	CString strTime = cTime.Format("%Y-%m-%d 00:00:00");


	TCHAR szQuery[1024] = {0};

	//
	// dwPOST_HOW_MANY_GET +1 = 31
	// To check that there are more than 30 posts in DB.
	// If Rowcount is greater than 30, there must be more mails after this query.
	//
	_snprintf( szQuery, 1024,		
		"SELECT Top %d PostID, PostState, PostType, PostClass, "		\
			"Sender, Recver, "											\
			"SendDate, RecvDate, ReturnDate, RecvBackDate, DeleteDate, "\
			"RecverName, SenderName, "									\
			"Design, Charge, ChargePayment, Title, Content, "			\
			"AttachMoney, MoneyAccept, Attach01Accept "					\
		"FROM PostInfo WITH(INDEX(IDX_PostRecver)) "					\
		"WHERE Recver = %d AND DeleteDate IS NULL "						\
			"AND ( SendDate >= '%s' OR ReturnDate >= '%s' ) "			\
			"ORDER BY ( CASE WHEN PostState = %d THEN DATEADD(YEAR, 1, SendDate) "								\
							"WHEN (PostState = %d AND RecvBackDate IS NOT NULL ) THEN ReturnDate "				\
							"WHEN (PostState = %d AND RecvBackDate IS NULL ) THEN DATEADD(YEAR, 1, ReturnDate) "\
							"ELSE SendDate END) DESC "															\
		,GLCONST_POST::dwPOST_HOW_MANY_GET + 1, iRecverNum, strTime.GetString(), strTime.GetString(), EMPOST_UNREAD, EMPOST_RETURN, EMPOST_RETURN
		);


	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szQuery);		
		Print(GetErrorString(spConn->hStmt));

		return sc::db::DB_ERROR;
	}

	LONGLONG	llPostID = 0;		SQLINTEGER	cbPostID = SQL_NTS;
	SQLSMALLINT	byPostState = 0;	SQLINTEGER	cbPostState = SQL_NTS;
	SQLSMALLINT	byPostType = 0;		SQLINTEGER	cbPostType = SQL_NTS;
	SQLSMALLINT	byPostClass = 0;	SQLINTEGER	cbPostClass = SQL_NTS;
	SQLINTEGER	iSender = 0,					cbSender = SQL_NTS;
	SQLINTEGER	iRecver = 0,					cbRecver = SQL_NTS;

	SQLCHAR		SendDate[20]={0,};	SQLINTEGER	cbSendDate = SQL_NTS;
	SQLCHAR		RecvDate[20]={0,};	SQLINTEGER	cbRecvDate = SQL_NTS;
	SQLCHAR		ReturnDate[20]={0,};SQLINTEGER	cbReturnDate = SQL_NTS;
	SQLCHAR		RecvBackDate[20]={0,};SQLINTEGER cbRecvBackDate = SQL_NTS;
	SQLCHAR		DeleteDate[20]={0,};SQLINTEGER	cbDeleteDate = SQL_NTS;

	SQLCHAR		RecverName[33]={0,};SQLINTEGER	cbRecverName = SQL_NTS;
	SQLCHAR		SenderName[33]={0,};SQLINTEGER	cbSenderName = SQL_NTS;

	SQLINTEGER	iDesign = 0,					cbDesign = SQL_NTS;
	LONGLONG	llCharge = 0;		SQLINTEGER	cbCharge = SQL_NTS;
	SQLSMALLINT	byChargePayment = 0;SQLINTEGER	cbChargePayment = SQL_NTS;

	SQLCHAR		Title[POST_MAX_TITLE_LENGTH]={0,};	SQLINTEGER cbTitle = SQL_NTS;
	SQLCHAR		Content[POST_MAX_CONTENT_LENGTH]={0,}; SQLINTEGER cbContent = SQL_NTS;

	LONGLONG	llAttachMoney = 0;	SQLINTEGER	cbAttachMoney = SQL_NTS;
	SQLSMALLINT	byMoneyAccept = 0;	SQLINTEGER	cbMoneyAccept = SQL_NTS;
	SQLSMALLINT	byAttach01Accept=0;	SQLINTEGER	cbAttach01Accept = SQL_NTS;


	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(szQuery);		
			Print(GetErrorString(spConn->hStmt));

			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data

			::SQLGetData(spConn->hStmt,  1, SQL_C_SBIGINT,	&llPostID,		0, &cbPostID);
			::SQLGetData(spConn->hStmt,  2, SQL_C_SHORT,		&byPostState,	0, &cbPostState);
			::SQLGetData(spConn->hStmt,  3, SQL_C_SHORT,		&byPostType,	0, &cbPostType);
			::SQLGetData(spConn->hStmt,  4, SQL_C_SHORT,		&byPostClass,	0, &cbPostClass);
			::SQLGetData(spConn->hStmt,  5, SQL_C_LONG,		&iSender,		0, &cbSender);
			::SQLGetData(spConn->hStmt,  6, SQL_C_LONG,		&iRecver,		0, &cbRecver);

			::SQLGetData(spConn->hStmt,  7, SQL_C_CHAR,		SendDate,		20,&cbSendDate);
			::SQLGetData(spConn->hStmt,  8, SQL_C_CHAR,		RecvDate,		20,&cbRecvDate);
			::SQLGetData(spConn->hStmt,  9, SQL_C_CHAR,		ReturnDate,		20,&cbReturnDate);
			::SQLGetData(spConn->hStmt, 10, SQL_C_CHAR,		RecvBackDate,	20,&cbRecvBackDate);
			::SQLGetData(spConn->hStmt, 11, SQL_C_CHAR,		DeleteDate,		20,&cbDeleteDate);

			::SQLGetData(spConn->hStmt, 12, SQL_C_CHAR,		RecverName,		33,&cbRecverName);
			::SQLGetData(spConn->hStmt, 13, SQL_C_CHAR,		SenderName,		33,&cbSenderName);

			::SQLGetData(spConn->hStmt, 14, SQL_C_LONG,		&iDesign,		0, &cbDesign);
			::SQLGetData(spConn->hStmt, 15, SQL_C_SBIGINT,	&llCharge,		0, &cbCharge);
			::SQLGetData(spConn->hStmt, 16, SQL_C_SHORT,		&byChargePayment,0,&cbChargePayment);

			::SQLGetData(spConn->hStmt, 17, SQL_C_CHAR,		Title,	POST_MAX_TITLE_LENGTH, &cbTitle);
			::SQLGetData(spConn->hStmt, 18, SQL_C_CHAR,		Content,POST_MAX_CONTENT_LENGTH, &cbContent);

			::SQLGetData(spConn->hStmt, 19, SQL_C_SBIGINT,	&llAttachMoney,	0, &cbAttachMoney);
			::SQLGetData(spConn->hStmt, 20, SQL_C_SHORT,		&byMoneyAccept,	0, &cbMoneyAccept);
			::SQLGetData(spConn->hStmt, 21, SQL_C_SHORT,		&byAttach01Accept, 0, &cbAttach01Accept);

			ZeroMemory( &aPost, sizeof(SAPOST) );

			aPost.llPostID		= llPostID;
			aPost.byPostState	= (BYTE) byPostState;
			aPost.byPostType	= (BYTE) byPostType;
			aPost.byPostClass	= (BYTE) byPostClass;
			aPost.iSender		= iSender;
			aPost.iRecver		= iRecver;

			if (cbSendDate != 0 && cbSendDate != -1)
				::StringCchCopy(aPost.SendDate, 20, (const char*) SendDate);

			if (cbRecvDate != 0 && cbRecvDate != -1)
				::StringCchCopy(aPost.RecvDate, 20, (const char*) RecvDate);

			if (cbReturnDate != 0 && cbReturnDate != -1)
				::StringCchCopy(aPost.ReturnDate, 20, (const char*) ReturnDate);

			if (cbRecvBackDate != 0 && cbRecvBackDate != -1)
				::StringCchCopy(aPost.RecvBackDate, 20, (const char*) RecvBackDate);

			if (cbDeleteDate != 0 && cbDeleteDate != -1)
				::StringCchCopy(aPost.DeleteDate, 20, (const char*) DeleteDate);

			if (cbRecverName != 0 && cbRecverName != -1)
				::StringCchCopy(aPost.RecverName, 33, (const char*) RecverName);

			if (cbSenderName != 0 && cbSenderName != -1)
				::StringCchCopy(aPost.SenderName, 33, (const char*) SenderName);

			aPost.iDesign = (BYTE) iDesign;

			if (byPostState == EMPOST_RETURN)
			{
				aPost.llCharge		  = 0;
				aPost.byChargePayment = 0;
			}
			else
			{
				aPost.llCharge		  = llCharge;
				aPost.byChargePayment = (BYTE) byChargePayment;
			}
			
			if (cbTitle != 0 && cbTitle != -1)
				::StringCchCopy(aPost.Title, POST_MAX_TITLE_LENGTH, (const char*) Title);

			if (cbContent != 0 && cbContent != -1)
				::StringCchCopy(aPost.Content, POST_MAX_CONTENT_LENGTH, (const char*) Content);

			aPost.llAttachMoney	= llAttachMoney;
			aPost.byMoneyAccept	= (BYTE) byMoneyAccept;
			aPost.byAttach01Accept=(BYTE)byAttach01Accept;

			vPost.push_back(aPost);

			nRowCount++;
		}
		else
		{
			break;
		}		
	}

	int nLoop = 0;

	if (nRowCount > (int)(GLCONST_POST::dwPOST_HOW_MANY_GET))
	{		
		nLoop = GLCONST_POST::dwPOST_HOW_MANY_GET;
	}
	else
	{		
		nLoop = nRowCount;
	}

	//Attach01

	for (int i=0; i<nLoop; i++)
	{
		se::ByteStream ByteStream;		

		sReturn = m_pGameDB->ReadImagePostAttach("Attach01", vPost[i].llPostID, ByteStream);

		if (sReturn == sc::db::DB_ERROR)
			return sc::db::DB_ERROR;

		DWORD dwVersion = 0;
		DWORD dwSizeItem = 0;
		DWORD dwSize = (DWORD) 1;
		SITEMCUSTOM ITEMCUSTOM(NATIVEID_NULL());

		if ( !ByteStream.IsEmpty() )
		{
			ByteStream >> dwVersion;
			ByteStream >> dwSizeItem;
			ByteStream >> dwSize;
			ByteStream.ReadBuffer( (LPBYTE*)&ITEMCUSTOM, sizeof(SITEMCUSTOM) );
		}

		vPost[i].sAttach01 = ITEMCUSTOM;
	}

	return sc::db::DB_OK;
*/
}



int COdbcManager::PostGetState(int iRecverNum, VPOSTSTATE &vPost, int &nRowCount)
{
    sc::writeLogError("COdbcManager::PostGetState. Do not call this function. Use Ado.");
    return sc::db::DB_ERROR;

    /*
	if ( iRecverNum < 0)
	{
		return sc::db::DB_ERROR;
	}

	//Empty the vector for POSTs
	vPost.clear();

	SAPOSTSTATE aPostState;	// A post

	SQLRETURN sReturn = 0;

    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
		return sc::db::DB_ERROR;


	__time64_t currentTime = CTime::GetCurrentTime().GetTime();
	currentTime -= (__time64_t)GLPost::llPOST_EXPIRE_DURATION_SEC;			
	CTime cTime(currentTime);
	//CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
	CString strTime = cTime.Format("%Y-%m-%d 00:00:00");


	TCHAR szQuery[512] = {0};

	//
	// dwPOST_HOW_MANY_GET +1 = 31
	// To check that there are more than 30 posts in DB.
	// If Rowcount is greater than 30, there must be more mails after this query.
	//
	_snprintf( szQuery, 512, 

		"SELECT Top %d PostID, PostState, PostType, PostClass, "		\
		"Sender, Recver, Design, Charge, "								\
		"SendDate, RecvDate, ReturnDate, RecvBackDate, DeleteDate, "	\
		"RecverName, SenderName "										\
		"FROM PostInfo "												\
		"WHERE PostClass <> %d AND ( Recver = %d OR Sender = %d ) "		\
		"AND ( SendDate >= '%s' OR ReturnDate >= '%s' ) "				\
		"ORDER BY (CASE WHEN PostState = %d THEN ReturnDate ELSE SendDate END) DESC "		\

		,GLCONST_POST::dwPOST_STATE_HOW_MANY_GET + 1, EMPOST_SYSTEM, iRecverNum, iRecverNum, strTime.GetString(), strTime.GetString(), EMPOST_RETURN
		);


	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)szQuery, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szQuery);		
		Print(GetErrorString(spConn->hStmt));

		return sc::db::DB_ERROR;
	}

	LONGLONG	llPostID = 0;		SQLINTEGER	cbPostID = SQL_NTS;
	SQLSMALLINT	byPostState = 0;	SQLINTEGER	cbPostState = SQL_NTS;
	SQLSMALLINT	byPostType = 0;		SQLINTEGER	cbPostType = SQL_NTS;
	SQLSMALLINT	byPostClass = 0;	SQLINTEGER	cbPostClass = SQL_NTS;
	SQLINTEGER	iSender = 0,					cbSender = SQL_NTS;
	SQLINTEGER	iRecver = 0,					cbRecver = SQL_NTS;
	SQLINTEGER	iDesign = 0,					cbDesign = SQL_NTS;
	LONGLONG	llCharge = 0;		SQLINTEGER	cbCharge = SQL_NTS;
	

	SQLCHAR		SendDate[20]={0,};	SQLINTEGER	cbSendDate = SQL_NTS;
	SQLCHAR		RecvDate[20]={0,};	SQLINTEGER	cbRecvDate = SQL_NTS;
	SQLCHAR		ReturnDate[20]={0,};SQLINTEGER	cbReturnDate = SQL_NTS;
	SQLCHAR		RecvBackDate[20]={0,};SQLINTEGER cbRecvBackDate = SQL_NTS;
	SQLCHAR		DeleteDate[20]={0,};SQLINTEGER	cbDeleteDate = SQL_NTS;

	SQLCHAR		RecverName[33]={0,};SQLINTEGER	cbRecverName = SQL_NTS;
	SQLCHAR		SenderName[33]={0,};SQLINTEGER	cbSenderName = SQL_NTS;
	


	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(szQuery);		
			Print(GetErrorString(spConn->hStmt));

			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data

			::SQLGetData(spConn->hStmt,  1, SQL_C_SBIGINT,	&llPostID,		0, &cbPostID);
			::SQLGetData(spConn->hStmt,  2, SQL_C_SHORT,		&byPostState,	0, &cbPostState);
			::SQLGetData(spConn->hStmt,  3, SQL_C_SHORT,		&byPostType,	0, &cbPostType);
			::SQLGetData(spConn->hStmt,  4, SQL_C_SHORT,		&byPostClass,	0, &cbPostClass);
			::SQLGetData(spConn->hStmt,  5, SQL_C_LONG,		&iSender,		0, &cbSender);
			::SQLGetData(spConn->hStmt,  6, SQL_C_LONG,		&iRecver,		0, &cbRecver);
			::SQLGetData(spConn->hStmt,  7, SQL_C_LONG,		&iDesign,		0, &cbDesign);
			::SQLGetData(spConn->hStmt,  8, SQL_C_SBIGINT,	&llCharge,		0, &cbCharge);

			::SQLGetData(spConn->hStmt,  9, SQL_C_CHAR,		SendDate,		20,&cbSendDate);
			::SQLGetData(spConn->hStmt, 10, SQL_C_CHAR,		RecvDate,		20,&cbRecvDate);
			::SQLGetData(spConn->hStmt, 11, SQL_C_CHAR,		ReturnDate,		20,&cbReturnDate);
			::SQLGetData(spConn->hStmt, 12, SQL_C_CHAR,		RecvBackDate,	20,&cbRecvBackDate);
			::SQLGetData(spConn->hStmt, 13, SQL_C_CHAR,		DeleteDate,		20,&cbDeleteDate);

			::SQLGetData(spConn->hStmt, 14, SQL_C_CHAR,		RecverName,		33,&cbRecverName);
			::SQLGetData(spConn->hStmt, 15, SQL_C_CHAR,		SenderName,		33,&cbSenderName);

			ZeroMemory( &aPostState, sizeof(SAPOSTSTATE) );

			aPostState.llPostID		= llPostID;
			aPostState.byPostState	= (BYTE) byPostState;
			aPostState.byPostType	= (BYTE) byPostType;
			aPostState.byPostClass	= (BYTE) byPostClass;
			aPostState.iSender		= iSender;
			aPostState.iRecver		= iRecver;
			aPostState.iDesign		= iDesign;
			aPostState.llCharge		= llCharge;

			if (cbSendDate != 0 && cbSendDate != -1)
				::StringCchCopy(aPostState.SendDate, 20, (const char*) SendDate);

			if (cbRecvDate != 0 && cbRecvDate != -1)
				::StringCchCopy(aPostState.RecvDate, 20, (const char*) RecvDate);

			if (cbReturnDate != 0 && cbReturnDate != -1)
				::StringCchCopy(aPostState.ReturnDate, 20, (const char*) ReturnDate);

			if (cbRecvBackDate != 0 && cbRecvBackDate != -1)
				::StringCchCopy(aPostState.RecvBackDate, 20, (const char*) RecvBackDate);

			if (cbDeleteDate != 0 && cbDeleteDate != -1)
				::StringCchCopy(aPostState.DeleteDate, 20, (const char*) DeleteDate);

			if (cbRecverName != 0 && cbRecverName != -1)
				::StringCchCopy(aPostState.RecverName, 33, (const char*) RecverName);

			if (cbSenderName != 0 && cbSenderName != -1)
				::StringCchCopy(aPostState.SenderName, 33, (const char*) SenderName);
			

			vPost.push_back(aPostState);

			nRowCount++;
		}
		else
		{
			break;
		}		
	}

	return sc::db::DB_OK;
    */
}


int COdbcManager::PostGetSendState(int iRecverNum, VPOSTSTATE &vPost, int &nRowCount)
{
    sc::writeLogError("COdbcManager::PostGetSendState. Do not call this function. Use Ado.");
    return sc::db::DB_ERROR;
}


int COdbcManager::PostGetRecvState(int iRecverNum, VPOSTSTATE &vPost, int &nRowCount)
{
    sc::writeLogError("COdbcManager::PostGetRecvState. Do not call this function. Use Ado.");
    return sc::db::DB_ERROR;
}


/*
int COdbcManager::PostCheckAttachMoney(DWORD dwClientID, SPOSTINFO &PostInfo, SINVENITEM *pItem)
{
	TCHAR szQuery[128] = {0,};	
	int nReturn = 0;


	if ( pItem == NULL )
	{
		//TEXTCONSOLEMSG_WRITE( "[ERR] PostSend: Attach01 does not exist on the axis during PostSend." );

		return FALSE;
	}


	//Checking the amount of attachedMoney and Attach01 are valid.


	_snprintf( szQuery, 128, "{call sp_PostCheckAttach(%d,%I64d,%d,%d,%d,?)}", 
		PostInfo.iSender, PostInfo.llAttachMoney, 
		PostInfo.idAttach01.dwID, PostInfo.xAttach01, PostInfo.yAttach01);

	nReturn = m_pGameDB->ExecuteSpInt(szQuery);


	/ *
	0: sc::db::DB_OK or EMPOST_OK
	-1: sc::db::DB_ERROR
	2: AttachMoney > Character's InvenMoney	
	* /

	return nReturn;
}
*/


//
//mjeon.Post
//

int COdbcManager::PostSendCharge( SAPOST &aPOST, DWORD dwOpenAndTakeUserNum )
{
    sc::writeLogError("COdbcManager::PostSendCharge. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szQuery[512 + POST_MAX_CONTENT_LENGTH] = {0,};
	LONGLONG llPostID = 0;
	int nReturn = 0;

	SYSTEMTIME st;
	GetLocalTime(&st);
	CTime cTime(st);
	CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
	
	//
	//Swap the Recver and Sender of aPOST
	//iRecver sends Charge to iSender.
	//
	CString csTitle		= CString(ID2SERVERTEXT( "POSTBOX_CHARGE_TITLE" ));
	CString csContent	= CString(ID2SERVERTEXT( "POSTBOX_CHARGE_CONTENT" ));

	csTitle.Replace("'", "''");
	csContent.Replace("'", "''");


	LONGLONG llMoney = (LONGLONG)(aPOST.m_llCharge * (1.0f - GLCONST_POST::fPOST_COMMISSION));

	_snprintf( szQuery, 512 + POST_MAX_CONTENT_LENGTH, 
		"{call sp_PostSend(%d,%d,%d,%d,%d,'%s','%s','%s',%d,%I64d,'%s','%s',%I64d, NULL, ?)}", 
		EMPOST_UNREAD, EMPOST_MONEY, EMPOST_SYSTEM, aPOST.m_iRecver, aPOST.m_iSender,
		aPOST.m_RecverName,
        aPOST.m_SenderName,
		strTime.GetString(),
        0,
        (LONGLONG)0,
		csTitle.GetString(),
        csContent.GetString(),
		llMoney );

	llPostID = m_pGameDB->ExecuteSpLONGLONG(szQuery);

	// Now llPostID has PostID or ErrorCode.
	if (llPostID < sc::db::DB_OK)	//In case of Error Code
	{
		return (int)llPostID;		//Not PostID. It's ErrorCode
	}
	
	return sc::db::DB_OK;
*/
}


int COdbcManager::PostSend(SPOSTINFO &PostInfo, SITEMCUSTOM &itemcustom)
{
    sc::writeLogError("COdbcManager::PostSend. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szQuery[512 + POST_MAX_CONTENT_LENGTH] = {0,};
	LONGLONG llPostID = 0;
	int nReturn = 0;

	//
    // Insert base Post information into the DB.
	//
	SYSTEMTIME st;
	GetLocalTime(&st);
	CTime cTime(st);
	CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");

	CString csTitle = CString(PostInfo.Title);
	CString csContent = CString(PostInfo.Content);

	csTitle.Replace("'", "''");
	csContent.Replace("'", "''");

	
	_snprintf( szQuery, 512 + POST_MAX_CONTENT_LENGTH, 
		"{call sp_PostSend(%d,%d,%d,%d,%d,'%s','%s','%s',%d,%I64d,'%s','%s',%I64d, NULL, ?)}", 
		PostInfo.byPostState,
        PostInfo.byPostType,
        PostInfo.byPostClass, 
		PostInfo.iSender,
        PostInfo.iRecver,
        PostInfo.SenderName,
        PostInfo.RecverName,
		strTime.GetString(),
        PostInfo.iDesign,
        PostInfo.llCharge,
        csTitle.GetString(),
        csContent.GetString(),
		PostInfo.llAttachMoney );

	llPostID = m_pGameDB->ExecuteSpLONGLONG(szQuery);
	PostInfo.llPostID = llPostID;

	// Now llPostID has PostID or ErrorCode.

	if (llPostID < sc::db::DB_OK)	//In case of Error Code
	{
		return (int)llPostID;		//Not PostID. It's an ErrorCode
	}

	if (PostInfo.bAttach01)
	{
		//	
		// Put the attached item01 image into the DB.
		//
		se::ByteStream ByteStream;
		LPBYTE pBuffer = NULL;
		DWORD dwSize = 0;


		ByteStream.ClearBuffer ();

		ByteStream << SITEMCUSTOM::VERSION;
		ByteStream << (DWORD)sizeof(SITEMCUSTOM);

		dwSize = (DWORD) 1;
		ByteStream << dwSize;

		ByteStream.WriteBuffer ( (LPBYTE)&itemcustom, (DWORD)sizeof(SITEMCUSTOM)*dwSize );

		ByteStream.GetBuffer(pBuffer, dwSize);

		if (pBuffer != NULL)
		{
			TCHAR szQuery[128] = {0};
			_snprintf( szQuery, 128, "UPDATE PostInfo SET PostInfo.Attach01=? WHERE (PostID=%I64d)", llPostID );
			
			nReturn = m_pGameDB->WriteImage(szQuery, 0, (BYTE *) pBuffer, dwSize);	// second parameter,ChaNum, is not used in WriteImage().

			pBuffer = NULL;
		}


		if (nReturn != sc::db::DB_OK)
		{
			//delete the Post

			std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();

			if (!spConn)
			{
				sc::writeLogError(std::string("COdbcManager::PostSend() - failed during deletion of the post that couldn't be sent properly."));

				return sc::db::DB_ERROR;
			}

			CString csQuery;

			csQuery.Format(_T("DELETE FROM dbo.PostInfo WHERE PostID=%I64d"), llPostID);

			::SQLExecDirect(spConn->hStmt, (SQLCHAR*)csQuery.GetString(), SQL_NTS);
		}
	}
	else
	{
		nReturn = sc::db::DB_OK;
	}
	

	return nReturn;
*/
}


int COdbcManager::PostSendRollBack(LONGLONG &llPostID)
{
    sc::writeLogError("COdbcManager::PostSendRollBack. Use ADO");
    return sc::db::DB_ERROR;
/*
	//delete the Post
	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();

	if (!spConn)
	{
		sc::writeLogError(std::string("COdbcManager::PostSendRollBack() - failed during deletion of the post that couldn't be sent properly."));

		return sc::db::DB_ERROR;
	}

	CString csQuery;

	csQuery.Format(_T("DELETE FROM dbo.PostInfo WHERE PostID=%I64d"), llPostID);

	SQLRETURN ret = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*)csQuery.GetString(), SQL_NTS);

	return ret;
*/
}


//
//mjeon.Post.ChaNum
//
int COdbcManager::GetChaNum(const char *szCharName, int *pnChaNum, int *pnUserNum)
{
    sc::writeLogError("COdbcManager::GetChaNum. Use ADO");
    return sc::db::DB_ERROR;
/*
	if( szCharName == NULL )
		return sc::db::DB_ERROR;

	SQLRETURN sReturn = 0;

    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();

	if (!spConn)
		return sc::db::DB_ERROR;

	SQLINTEGER nChaNum = 0, cbChaNum=SQL_NTS;
	SQLINTEGER nUserNum = 0, cbUserNum=SQL_NTS;

	TCHAR szQuery[128] = {0};
	_snprintf( szQuery, 128, "SELECT ChaNum, UserNum FROM dbo.ChaInfo WHERE ChaName = '%s' AND ChaDeleted = 0", szCharName );

	sReturn = ::SQLExecDirect(spConn->hStmt,
		(SQLCHAR*)szQuery,
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szQuery);		
		Print(GetErrorString(spConn->hStmt));

		return sc::db::DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR)
		{
			Print(szQuery);
			Print(GetErrorString(spConn->hStmt));

			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(spConn->hStmt,  1, SQL_C_LONG, &nChaNum, 0, &cbChaNum);
			::SQLGetData(spConn->hStmt,  2, SQL_C_LONG, &nUserNum, 0, &cbUserNum);

			if (cbChaNum != 0 && cbChaNum != -1)
				*pnChaNum = nChaNum;
			else
			{
				return sc::db::DB_ERROR;
			}

			if (nUserNum >= 0)
				*pnUserNum = nUserNum;
			else
			{
				return sc::db::DB_ERROR;
			}

			break;
		}
		else
		{
			return sc::db::DB_ERROR;
		}		
	}

	return sc::db::DB_OK;
*/
}
