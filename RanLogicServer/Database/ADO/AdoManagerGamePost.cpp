#include "../../pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../SigmaCore/Util/DateTime.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogic/Post/PostDefine.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "./AdoExt.h"
#include "./ADOManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

int AdoManager::PostMaintenance()										
{
    int nRowCount = 0;
	SQLRETURN sReturn = 0;

	VPOSTID vPostID;
	ITER_VPOSTID ipid;

	__time64_t currentTime = CTime::GetCurrentTime().GetTime();

	//Target Time
	__time64_t targetTime = currentTime - (__time64_t)GLPost::llPOST_EXPIRE_DURATION_SEC;
	CTime cTargetTime(targetTime);
	CString TargetDate = cTargetTime.Format("%Y-%m-%d %H:%M:%S");	


    sc::db::AdoExt Ado(m_GameDBConnString);

	// Returned Post cannot include  Chargement

    // All nations combine return post procedure.
    APPEND_IPARAM_VARCHAR(Ado, "@SendDate", TargetDate.GetString(), TargetDate.GetLength());
    if (!Ado.Execute4Cmd("dbo.PostInfoCommonMaintenance", adCmdStoredProc))
        return sc::db::DB_ERROR;

	/*if (m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG)
	{
        APPEND_IPARAM_VARCHAR(Ado, "@SendDate", TargetDate.GetString(), TargetDate.GetLength());
        if (!Ado.Execute4Cmd("dbo.PostInfoMaintenance1", adCmdStoredProc))
            return sc::db::DB_ERROR;
	}
	else
	{
        APPEND_IPARAM_VARCHAR(Ado, "@SendDate", TargetDate.GetString(), TargetDate.GetLength());
        if (!Ado.Execute4Cmd("dbo.PostInfoMaintenance2", adCmdStoredProc))
            return sc::db::DB_ERROR;
	}*/

    if (Ado.GetEOF())
        return sc::db::DB_OK;
	
    do
	{
        // Bind data
        LONGLONG llPostID = 0;
        Ado.GetCollect("PostID", llPostID);
        nRowCount++;
		vPostID.push_back(llPostID);
	} while (Ado.Next());

	// Return the charged POSTs before clean-up	
	for (ipid = vPostID.begin(); ipid != vPostID.end(); ipid++)
	{
		PostReturn(*ipid);
	}

	return sc::db::DB_OK;
}

int AdoManager::PostCleanUp(char *pszCurrentDate, char *pszTargetDate)	
{
/*
    CREATE PROCEDURE [dbo].[sp_PostClean]	
        @CurrentDate	VARCHAR(20),
        @TargetDate		VARCHAR(20),
        @nReturn		int	OUTPUT
*/
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_RPARAM_INT(Ado);

    APPEND_IPARAM_VARCHAR(Ado, "@CurrentDate", pszCurrentDate, strlen(pszCurrentDate));
    APPEND_IPARAM_VARCHAR(Ado, "@TargetDate", pszTargetDate, strlen(pszTargetDate));
    
    APPEND_OPARAM_INT(Ado, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_PostClean", Ado, "@nReturn");
}

int AdoManager::PostReturn(LONGLONG llPostID)
{
    SYSTEMTIME st;
    GetLocalTime(&st);	
    CTime cTime(st);
    CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
/*
    CREATE PROCEDURE dbo.sp_PostReturn
        @PostID			BIGINT,	
        @PostState		TINYINT,
        @ReturnDate		VARCHAR(20),	
        @nReturn		int	OUTPUT
*/
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_RPARAM_INT(Ado);
    
    APPEND_IPARAM_BIGINT (Ado, "@PostID", llPostID);
    APPEND_IPARAM_TINYINT(Ado, "@PostState", (BYTE) EMPOST_RETURN);
    APPEND_IPARAM_VARCHAR(Ado, "@ReturnDate", strTime.GetString(), strTime.GetLength());

    APPEND_OPARAM_INT(Ado, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_PostReturn", Ado, "@nReturn");
}

int AdoManager::PostDelete(LONGLONG llPostID)
{
    SYSTEMTIME st;
    GetLocalTime(&st);	
    CTime cTime(st);	
    CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
/*
    CREATE PROCEDURE [dbo].[sp_PostDelete]
        @PostID			BIGINT,
        @PostState		TINYINT,
        @DeleteDate		VARCHAR(20),
        @nReturn		int	OUTPUT
*/    
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_RPARAM_INT(Ado);
    
    APPEND_IPARAM_BIGINT (Ado, "@PostID", llPostID);
    APPEND_IPARAM_TINYINT(Ado, "@PostState", (BYTE) EMPOST_USER_DELETE);
    APPEND_IPARAM_VARCHAR(Ado, "@DeleteDate", strTime.GetString(), strTime.GetLength());

    APPEND_OPARAM_INT(Ado, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_PostDelete", Ado, "@nReturn");
}

int AdoManager::PostGetInfo(int iRecverNum, int &cntAllPost, int &cntNewPost)
{
    if (iRecverNum < 0)
        return sc::db::DB_ERROR;

    int nRowCount = 0;
    SQLRETURN sReturn = 0;    

    __time64_t currentTime = CTime::GetCurrentTime().GetTime();
    currentTime -= (__time64_t) GLPost::llPOST_EXPIRE_DURATION_SEC;
    CTime cTime(currentTime);

    CString strTime = cTime.Format("%Y-%m-%d 00:00:00");

    sc::db::AdoExt Ado(m_GameDBConnString);

    APPEND_IPARAM_INT    (Ado, "@Recver1", iRecverNum);
    APPEND_IPARAM_INT    (Ado, "@Recver2", iRecverNum);
    APPEND_IPARAM_TINYINT(Ado, "@PostState1", static_cast<short> (EMPOST_UNREAD));
    APPEND_IPARAM_TINYINT(Ado, "@PostState2", static_cast<short> (EMPOST_RETURN));
    
    APPEND_IPARAM_VARCHAR(Ado, "@SendDate1", strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_VARCHAR(Ado, "@SendDate2", strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_VARCHAR(Ado, "@ReturnDate1", strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_VARCHAR(Ado, "@ReturnDate2", strTime.GetString(), strTime.GetLength());

    if (!Ado.Execute4Cmd("dbo.PostInfoGet", adCmdStoredProc))
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())
        return sc::db::DB_OK;

    int nAllPost = 0;
    int nNewPost = 0;

    do
    {
        Ado.GetCollect("AllPost", nAllPost);
        Ado.GetCollect("NewPost", nNewPost);
        nRowCount++;
    }  while (Ado.Next());

    if (nRowCount > 1)		// It MUST be 0 or 1.
        return sc::db::DB_ERROR;
    
    cntAllPost = nAllPost;
    cntNewPost = nNewPost;

    return sc::db::DB_OK;
}

int AdoManager::PostGetData(int iRecverNum, std::vector<SAPOST>& vPost, int &nRowCount, DWORD dwRecverUserNum)
{
    if (iRecverNum < 0)
        return sc::db::DB_ERROR;

    //Empty the vector for POSTs
    vPost.clear();

    __time64_t currentTime = CTime::GetCurrentTime().GetTime();
    currentTime -= (__time64_t)GLPost::llPOST_EXPIRE_DURATION_SEC;			
    CTime cTime(currentTime);	
    CString strTime = cTime.Format("%Y-%m-%d 00:00:00");

    //
    // dwPOST_HOW_MANY_GET +1 = 31
    // To check that there are more than 30 posts in DB.
    // If Rowcount is greater than 30, there must be more mails after this query.
    //
    sc::db::AdoExt Ado(m_GameDBConnString);

    APPEND_IPARAM_INT    (Ado, "@Recver",     iRecverNum);
    APPEND_IPARAM_VARCHAR(Ado, "@SendDate",   strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_VARCHAR(Ado, "@ReturnDate", strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_TINYINT(Ado, "@PostState1", static_cast<short> (EMPOST_UNREAD));
    APPEND_IPARAM_TINYINT(Ado, "@PostState2", static_cast<short> (EMPOST_RETURN));
    APPEND_IPARAM_TINYINT(Ado, "@PostState3", static_cast<short> (EMPOST_RETURN));
	APPEND_IPARAM_INT    (Ado, "@RecverUserNum", dwRecverUserNum);

    if (!Ado.Execute4Cmd("dbo.PostInfoGetData", adCmdStoredProc))
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())
        return sc::db::DB_OK;

    do
    {
        LONGLONG llPostID = 0;
        short byPostState = 0; short byPostType = 0; short byPostClass = 0;
        int iSender = 0; int iRecver = 0; DWORD dwSenderUserNum = 0;
        //std::string SendDate; std::string RecvDate; std::string ReturnDate; std::string RecvBackDate; std::string DeleteDate;
        _variant_t SendDate; _variant_t RecvDate; _variant_t ReturnDate; _variant_t RecvBackDate; _variant_t DeleteDate;
        std::string RecverName; std::string SenderName;
        int iDesign = 0; LONGLONG llCharge = 0; short byChargePayment = 0;
        std::string Title; std::string Content;
        LONGLONG llAttachMoney = 0; short byMoneyAccept = 0; short byAttach01Accept = 0;
		int iPostOption = 0;

        Ado.GetCollect("PostID",         llPostID);
        Ado.GetCollect("PostState",      byPostState);
        Ado.GetCollect("PostType",       byPostType);
        Ado.GetCollect("PostClass",      byPostClass);
        Ado.GetCollect("Sender",         iSender);
        Ado.GetCollect("Recver",         iRecver);
        Ado.GetCollect("SendDate",       SendDate);
        Ado.GetCollect("RecvDate",       RecvDate);
        Ado.GetCollect("ReturnDate",     ReturnDate);
        Ado.GetCollect("RecvBackDate",   RecvBackDate);
        Ado.GetCollect("DeleteDate",     DeleteDate);
        Ado.GetCollect("RecverName",     RecverName);
        Ado.GetCollect("SenderName",     SenderName);
        Ado.GetCollect("Design",         iDesign);
        Ado.GetCollect("Charge",         llCharge);
        Ado.GetCollect("ChargePayment",  byChargePayment);
        Ado.GetCollect("Title",          Title);
        Ado.GetCollect("Content",        Content);
        Ado.GetCollect("AttachMoney",    llAttachMoney);
        Ado.GetCollect("MoneyAccept",    byMoneyAccept);
        Ado.GetCollect("Attach01Accept", byAttach01Accept);

        std::vector<BYTE> vBuffer;
        Ado.GetChunk("Attach01", vBuffer);

		Ado.GetCollect("SenderUserNum", dwSenderUserNum);

		Ado.GetCollect( "PostOption", iPostOption );

        SAPOST aPost;	// A post       

        aPost.m_llPostID	  = llPostID;
        aPost.m_byPostState = (BYTE) byPostState;
        aPost.m_byPostType  = (BYTE) byPostType;
        aPost.m_byPostClass = (BYTE) byPostClass;
        aPost.m_iSender	  = iSender;
        aPost.m_iRecver	  = iRecver;        
        aPost.m_iDesign     = (BYTE) iDesign;
        aPost.m_llAttachMoney	= llAttachMoney;
        aPost.m_byMoneyAccept	= (BYTE) byMoneyAccept;
        aPost.m_byAttach01Accept=(BYTE)byAttach01Accept;

        if (byPostState == EMPOST_RETURN)
        {
            aPost.m_llCharge		  = 0;
            aPost.m_byChargePayment = 0;
        }
        else
        {
            aPost.m_llCharge		  = llCharge;
            aPost.m_byChargePayment = (BYTE) byChargePayment;
        }
        
        aPost.SetTitle(Title);        
        aPost.SetContent(Content);

        if( SendDate.GetVARIANT().vt == VT_DATE )
            aPost.m_SendDate = sc::time::GetTime(SendDate);

        if( RecvDate.GetVARIANT().vt == VT_DATE )
            aPost.m_RecvDate = sc::time::GetTime(RecvDate);

        if( ReturnDate.GetVARIANT().vt == VT_DATE )
            aPost.m_ReturnDate = sc::time::GetTime(ReturnDate);

        if( RecvBackDate.GetVARIANT().vt == VT_DATE )
            aPost.m_RecvBackDate = sc::time::GetTime(RecvBackDate);

        if( DeleteDate.GetVARIANT().vt == VT_DATE )
            aPost.m_DeleteDate = sc::time::GetTime(DeleteDate);

        aPost.SetRecverName(RecverName);        
        aPost.SetSenderName(SenderName);
        
        // Attach01
        se::ByteStream ByteStream(vBuffer);
        DWORD dwVersion = 0;
        DWORD dwSize = 0;
        DWORD dwNum = (DWORD) 1;
        SITEMCUSTOM ITEMCUSTOM(NATIVEID_NULL());
        if (!ByteStream.IsEmpty())
        {
			if ( -19771129 == iSender )
			{
				ITEMCUSTOM.AutoAssignForMigration( ByteStream );
			}
			else
			{
				ITEMCUSTOM.AutoAssign( ByteStream );
			}
        }

		// 아이템 변환전에 우편에 있던 guid 가 없던 우편이다. guid 신규 생성한다.
		if ( 1 == iPostOption )
		{
			sc::SeqUniqueGUID::Instance()->getGUID( ITEMCUSTOM.guid );
			ITEMCUSTOM.GenerateBasicStat( false );
			ITEMCUSTOM.GenerateLinkSkill();
			ITEMCUSTOM.GenerateAddOption();
		}

        aPost.m_sAttach01 = ITEMCUSTOM;

        vPost.push_back(aPost);

        nRowCount++;
    } while (Ado.Next());

    return sc::db::DB_OK;
}

int AdoManager::PostGetState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int& nRowCount)
{
    if (iRecverNum < 0)
        return sc::db::DB_ERROR;

    //Empty the vector for POSTs
    vPost.clear();    

    sc::db::AdoExt Ado(m_GameDBConnString);

    __time64_t currentTime = sc::time::GetCurrentTime(); //CTime::GetCurrentTime().GetTime();
    currentTime -= (__time64_t) GLPost::llPOST_EXPIRE_DURATION_SEC;
    CTime cTime(currentTime);
    //CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
    CString strTime = cTime.Format("%Y-%m-%d 00:00:00");
  
    // dwPOST_STATE_HOW_MANY_GET +1 = 31
    // To check that there are more than 30 posts in DB.
    // If Rowcount is greater than 30, there must be more mails after this query.    
    APPEND_IPARAM_INT    (Ado, "@PostClass", static_cast<int> (EMPOST_SYSTEM));
    APPEND_IPARAM_INT    (Ado, "@Recver", iRecverNum);    
    APPEND_IPARAM_INT    (Ado, "@Sender", iRecverNum);    
    APPEND_IPARAM_VARCHAR(Ado, "@SendDate", strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_VARCHAR(Ado, "@ReturnDate", strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_TINYINT(Ado, "@PostState", static_cast<int> (EMPOST_RETURN));
    
    if (!Ado.Execute4Cmd("dbo.PostInfoGetState", adCmdStoredProc))	
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())
        return sc::db::DB_OK;

    do
    {
        LONGLONG llPostID = 0;
        short byPostState = 0;
        short byPostType = 0;
        short byPostClass = 0;
        int iSender = 0;
        int iRecver = 0;
        int iDesign = 0;
        LONGLONG llCharge = 0;
        _variant_t SendDate;
        _variant_t RecvDate;
        _variant_t ReturnDate;
        _variant_t RecvBackDate;
        _variant_t DeleteDate;
        std::string RecverName;
        std::string SenderName;

        Ado.GetCollect("PostID", llPostID);
        Ado.GetCollect("PostState", byPostState);
        Ado.GetCollect("PostType", byPostType);
        Ado.GetCollect("PostClass", byPostClass);
        Ado.GetCollect("Sender", iSender);
        Ado.GetCollect("Recver", iRecver);
        Ado.GetCollect("Design", iDesign);
        Ado.GetCollect("Charge", llCharge);
        Ado.GetCollect("SendDate", SendDate);
        Ado.GetCollect("RecvDate", RecvDate);
        Ado.GetCollect("ReturnDate", ReturnDate);
        Ado.GetCollect("RecvBackDate", RecvBackDate);
        Ado.GetCollect("DeleteDate", DeleteDate);
        Ado.GetCollect("RecverName", RecverName);
        Ado.GetCollect("SenderName", SenderName);

        SAPOSTSTATE aPostState;	// A post

        aPostState.llPostID		= llPostID;
        aPostState.byPostState	= (BYTE) byPostState;
        aPostState.byPostType	= (BYTE) byPostType;
        aPostState.byPostClass	= (BYTE) byPostClass;
        aPostState.iSender		= iSender;
        aPostState.iRecver		= iRecver;
        aPostState.iDesign		= iDesign;
        aPostState.llCharge		= llCharge;

        if( SendDate.GetVARIANT().vt == VT_DATE )
            aPostState.SendDate = sc::time::GetTime(SendDate);

        if( RecvDate.GetVARIANT().vt == VT_DATE )
            aPostState.RecvDate =  sc::time::GetTime(RecvDate);

        if( ReturnDate.GetVARIANT().vt == VT_DATE )
            aPostState.ReturnDate = sc::time::GetTime(ReturnDate);

        if( RecvBackDate.GetVARIANT().vt == VT_DATE )
            aPostState.RecvBackDate = sc::time::GetTime(RecvBackDate);

        if( DeleteDate.GetVARIANT().vt == VT_DATE )
            aPostState.DeleteDate = sc::time::GetTime(DeleteDate);

        aPostState.SetRecverName(RecverName);
        aPostState.SetSenderName(SenderName);

        vPost.push_back(aPostState);
        nRowCount++;
    } while (Ado.Next());

    return sc::db::DB_OK;
}

int AdoManager::PostGetSendState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int& nRowCount)
{
    if (iRecverNum < 0)
        return sc::db::DB_ERROR;

    //Empty the vector for POSTs
    vPost.clear();    

    sc::db::AdoExt Ado(m_GameDBConnString);

    __time64_t currentTime = sc::time::GetCurrentTime(); //CTime::GetCurrentTime().GetTime();
    currentTime -= (__time64_t) GLPost::llPOST_EXPIRE_DURATION_SEC;
    CTime cTime(currentTime);
    //CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
    CString strTime = cTime.Format("%Y-%m-%d 00:00:00");

    // dwPOST_SEND_STATE_HOW_MANY_GET +1 = 16
    // To check that there are more than 15 posts in DB.
    // If Rowcount is greater than 15, there must be more mails after this query.    
    APPEND_IPARAM_INT    (Ado, "@PostClass", static_cast<int> (EMPOST_SYSTEM));
    APPEND_IPARAM_INT    (Ado, "@Sender", iRecverNum);    
    APPEND_IPARAM_VARCHAR(Ado, "@SendDate", strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_VARCHAR(Ado, "@ReturnDate", strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_TINYINT(Ado, "@PostState", static_cast<int> (EMPOST_RETURN));

    if (!Ado.Execute4Cmd("dbo.PostInfoGetSendState", adCmdStoredProc))	
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())
        return sc::db::DB_OK;

    do
    {
        LONGLONG llPostID = 0;
        short byPostState = 0;
        short byPostType = 0;
        short byPostClass = 0;
        int iSender = 0;
        int iRecver = 0;
        int iDesign = 0;
        LONGLONG llCharge = 0;
        _variant_t SendDate;
        _variant_t RecvDate;
        _variant_t ReturnDate;
        _variant_t RecvBackDate;
        _variant_t DeleteDate;
        std::string RecverName;
        std::string SenderName;

        Ado.GetCollect("PostID", llPostID);
        Ado.GetCollect("PostState", byPostState);
        Ado.GetCollect("PostType", byPostType);
        Ado.GetCollect("PostClass", byPostClass);
        Ado.GetCollect("Sender", iSender);
        Ado.GetCollect("Recver", iRecver);
        Ado.GetCollect("Design", iDesign);
        Ado.GetCollect("Charge", llCharge);
        Ado.GetCollect("SendDate", SendDate);
        Ado.GetCollect("RecvDate", RecvDate);
        Ado.GetCollect("ReturnDate", ReturnDate);
        Ado.GetCollect("RecvBackDate", RecvBackDate);
        Ado.GetCollect("DeleteDate", DeleteDate);
        Ado.GetCollect("RecverName", RecverName);
        Ado.GetCollect("SenderName", SenderName);

        SAPOSTSTATE aPostState;	// A post

        aPostState.llPostID		= llPostID;
        aPostState.byPostState	= (BYTE) byPostState;
        aPostState.byPostType	= (BYTE) byPostType;
        aPostState.byPostClass	= (BYTE) byPostClass;
        aPostState.iSender		= iSender;
        aPostState.iRecver		= iRecver;
        aPostState.iDesign		= iDesign;
        aPostState.llCharge		= llCharge;

        if( SendDate.GetVARIANT().vt == VT_DATE )
            aPostState.SendDate = sc::time::GetTime(SendDate);

        if( RecvDate.GetVARIANT().vt == VT_DATE )
            aPostState.RecvDate =  sc::time::GetTime(RecvDate);

        if( ReturnDate.GetVARIANT().vt == VT_DATE )
            aPostState.ReturnDate = sc::time::GetTime(ReturnDate);

        if( RecvBackDate.GetVARIANT().vt == VT_DATE )
            aPostState.RecvBackDate = sc::time::GetTime(RecvBackDate);

        if( DeleteDate.GetVARIANT().vt == VT_DATE )
            aPostState.DeleteDate = sc::time::GetTime(DeleteDate);

        aPostState.SetRecverName(RecverName);
        aPostState.SetSenderName(SenderName);

        vPost.push_back(aPostState);
        nRowCount++;
    } while (Ado.Next());

    return sc::db::DB_OK;
}

int AdoManager::PostGetRecvState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int& nRowCount)
{
    if (iRecverNum < 0)
        return sc::db::DB_ERROR;

    //Empty the vector for POSTs
    vPost.clear();    

    sc::db::AdoExt Ado(m_GameDBConnString);

    __time64_t currentTime = sc::time::GetCurrentTime(); //CTime::GetCurrentTime().GetTime();
    currentTime -= (__time64_t) GLPost::llPOST_EXPIRE_DURATION_SEC;
    CTime cTime(currentTime);
    //CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
    CString strTime = cTime.Format("%Y-%m-%d 00:00:00");

    // dwPOST_RECV_STATE_HOW_MANY_GET +1 = 16
    // To check that there are more than 15 posts in DB.
    // If Rowcount is greater than 15, there must be more mails after this query.    
    APPEND_IPARAM_INT    (Ado, "@PostClass", static_cast<int> (EMPOST_SYSTEM));
    APPEND_IPARAM_INT    (Ado, "@Recver", iRecverNum);    
    APPEND_IPARAM_VARCHAR(Ado, "@SendDate", strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_VARCHAR(Ado, "@ReturnDate", strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_TINYINT(Ado, "@PostState", static_cast<int> (EMPOST_RETURN));

    if (!Ado.Execute4Cmd("dbo.PostInfoGetRecvState", adCmdStoredProc))	
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())
        return sc::db::DB_OK;

    do
    {
        LONGLONG llPostID = 0;
        short byPostState = 0;
        short byPostType = 0;
        short byPostClass = 0;
        int iSender = 0;
        int iRecver = 0;
        int iDesign = 0;
        LONGLONG llCharge = 0;
        _variant_t SendDate;
        _variant_t RecvDate;
        _variant_t ReturnDate;
        _variant_t RecvBackDate;
        _variant_t DeleteDate;
        std::string RecverName;
        std::string SenderName;

        Ado.GetCollect("PostID", llPostID);
        Ado.GetCollect("PostState", byPostState);
        Ado.GetCollect("PostType", byPostType);
        Ado.GetCollect("PostClass", byPostClass);
        Ado.GetCollect("Sender", iSender);
        Ado.GetCollect("Recver", iRecver);
        Ado.GetCollect("Design", iDesign);
        Ado.GetCollect("Charge", llCharge);
        Ado.GetCollect("SendDate", SendDate);
        Ado.GetCollect("RecvDate", RecvDate);
        Ado.GetCollect("ReturnDate", ReturnDate);
        Ado.GetCollect("RecvBackDate", RecvBackDate);
        Ado.GetCollect("DeleteDate", DeleteDate);
        Ado.GetCollect("RecverName", RecverName);
        Ado.GetCollect("SenderName", SenderName);

        SAPOSTSTATE aPostState;	// A post

        aPostState.llPostID		= llPostID;
        aPostState.byPostState	= (BYTE) byPostState;
        aPostState.byPostType	= (BYTE) byPostType;
        aPostState.byPostClass	= (BYTE) byPostClass;
        aPostState.iSender		= iSender;
        aPostState.iRecver		= iRecver;
        aPostState.iDesign		= iDesign;
        aPostState.llCharge		= llCharge;

        if( SendDate.GetVARIANT().vt == VT_DATE )
            aPostState.SendDate = sc::time::GetTime(SendDate);

        if( RecvDate.GetVARIANT().vt == VT_DATE )
            aPostState.RecvDate =  sc::time::GetTime(RecvDate);

        if( ReturnDate.GetVARIANT().vt == VT_DATE )
            aPostState.ReturnDate = sc::time::GetTime(ReturnDate);

        if( RecvBackDate.GetVARIANT().vt == VT_DATE )
            aPostState.RecvBackDate = sc::time::GetTime(RecvBackDate);

        if( DeleteDate.GetVARIANT().vt == VT_DATE )
            aPostState.DeleteDate = sc::time::GetTime(DeleteDate);

        aPostState.SetRecverName(RecverName);
        aPostState.SetSenderName(SenderName);

        vPost.push_back(aPostState);
        nRowCount++;
    } while (Ado.Next());

    return sc::db::DB_OK;
}

int AdoManager::PostOpen(LONGLONG llPostID)
{
    SYSTEMTIME st;
    GetLocalTime(&st);	
    CTime cTime(st);	
    CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");

    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_RPARAM_INT(Ado);

    APPEND_IPARAM_BIGINT (Ado, "@PostID", llPostID);
    APPEND_IPARAM_TINYINT(Ado, "@PostState", (BYTE) EMPOST_READ);
    APPEND_IPARAM_VARCHAR(Ado, "@DeleteDate", strTime.GetString(), strTime.GetLength());

    APPEND_OPARAM_INT(Ado, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_PostOpen", Ado, "@nReturn");
}

int AdoManager::PostSend(SPOSTINFO &PostInfo, SITEMCUSTOM &itemcustom)
{
    TCHAR szQuery[512 + POST_MAX_CONTENT_LENGTH] = {0,};
    LONGLONG llPostID = 0;
    int nReturn = 0;

    // Insert base Post information into the DB.
    SYSTEMTIME st;
    GetLocalTime(&st);
    CTime cTime(st);
    CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");

    CString csTitle = CString(PostInfo.Title);
    CString csContent = CString(PostInfo.Content);

    csTitle.Replace("'", "''");
    csContent.Replace("'", "''");

    sc::db::AdoExt Ado(m_GameDBConnString);
	APPEND_RPARAM_BIGINT(Ado);	

    APPEND_IPARAM_TINYINT(Ado, "@PostState",   PostInfo.byPostState);
    APPEND_IPARAM_TINYINT(Ado, "@PostType",    PostInfo.byPostType);
    APPEND_IPARAM_TINYINT(Ado, "@PostClass",   PostInfo.byPostClass);
    APPEND_IPARAM_INT    (Ado, "@Sender",      PostInfo.iSender);
    APPEND_IPARAM_INT    (Ado, "@Recver",      PostInfo.iRecver);
    APPEND_IPARAM_VARCHAR(Ado, "@SenderName",  PostInfo.SenderName, strlen(PostInfo.SenderName));
    APPEND_IPARAM_VARCHAR(Ado, "@RecverName",  PostInfo.RecverName, strlen(PostInfo.RecverName));
    APPEND_IPARAM_VARCHAR(Ado, "@SendDate",    strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_INT    (Ado, "@Design",      PostInfo.iDesign);
    APPEND_IPARAM_BIGINT (Ado, "@Charge",      PostInfo.llCharge);
    APPEND_IPARAM_VARCHAR(Ado, "@Title",       csTitle.GetString(), csTitle.GetLength());
    APPEND_IPARAM_VARCHAR(Ado, "@Content",     csContent.GetString(), csContent.GetLength());
    APPEND_IPARAM_BIGINT (Ado, "@AttachMoney", PostInfo.llAttachMoney);
    
	_variant_t vartBLOB;
	
	ULONG nSizeOfChunk = 0;

    if (PostInfo.bAttach01)
    {
        // Put the attached item01 image into the DB.
        se::ByteStream ByteStream;
        LPBYTE pBuffer = NULL;
        DWORD dwSize = 0;

        ByteStream.ClearBuffer();
        ByteStream << SITEMCUSTOM::VERSION;
        ByteStream << (DWORD) sizeof(SITEMCUSTOM);
        dwSize = (DWORD) 1;
        ByteStream << dwSize;
        ByteStream.WriteBuffer((LPBYTE) &itemcustom, (DWORD) sizeof(SITEMCUSTOM) * dwSize);        
        if (!Ado.MakeChunk(vartBLOB, ByteStream, nSizeOfChunk))
        {
            sc::writeLogError(
                sc::string::format(
                    "PostSend Makechunk Size %1%",
                    nSizeOfChunk));
        }
    }
	else
	{
		/*
		BYTE byTemp = 0x00;
		nSizeOfChunk = sizeof(byTemp);
		Ado.MakeChunk(vartBLOB, &byTemp, nSizeOfChunk);
		*/
		//_variant_t vtMissingYours(DISP_E_PARAMNOTFOUND, VT_ERROR);
		vartBLOB.vt = VT_NULL;
		nSizeOfChunk = sizeof(_variant_t);		

        // 단순히 돈을 첨부한다면 이쪽으로 코드가 들어온다.
        // 에러를 남기는것은 올바르지 않다.
        //sc::writeLogError("PostSend Size 0");
	}
	
	APPEND_IPARAM_IMAGE(Ado, "@Attach01", vartBLOB, nSizeOfChunk);

	// 보내는 유저번호 추가
	APPEND_IPARAM_INT( Ado, "@SenderUserNum", PostInfo.dwUserNum );

	APPEND_OPARAM_BIGINT(Ado, "@nReturn");
    
    llPostID = ExecuteStoredProcedureLonglongReturn("dbo.sp_PostSend", Ado, "@nReturn");    

	// 롤백에 사용되는 우편번호 넣어준다.
	PostInfo.llPostID = llPostID;

    // Now llPostID has PostID or ErrorCode.
    if (llPostID < sc::db::DB_OK)	//In case of Error Code
        return (int) llPostID;		//Not PostID. It's an ErrorCode

    return sc::db::DB_OK;
}

int AdoManager::PostSendRollBack(LONGLONG &llPostID)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_IPARAM_BIGINT(Ado, "@PostID", llPostID);
    return ExecuteStoredProcedure("dbo.PostInfoDelete", Ado);
}

int AdoManager::PostOpenAndTake(LONGLONG llPostID, BOOL bMoneyAccept, BOOL bAttach01Accept, BYTE bChargePayment, BYTE bReturnedPost)
{   
    SYSTEMTIME st;
    GetLocalTime(&st);	
    CTime cTime(st);	
    CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");

    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_RPARAM_INT(Ado);
    
    APPEND_IPARAM_BIGINT (Ado, "@PostID", llPostID);
    APPEND_IPARAM_TINYINT(Ado, "@MoneyAccept", (BYTE) bMoneyAccept);
    APPEND_IPARAM_TINYINT(Ado, "@Attach01Accept", (BYTE) bAttach01Accept);
    APPEND_IPARAM_TINYINT(Ado, "@ChargePayment", bChargePayment);
    APPEND_IPARAM_VARCHAR(Ado, "@RecvDate", strTime.GetString(), strTime.GetLength());
    APPEND_IPARAM_TINYINT(Ado, "@Returned", (BYTE) bReturnedPost);

    APPEND_OPARAM_INT(Ado, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_PostOpenTake", Ado, "@nReturn");
}

int AdoManager::PostSendCharge(SAPOST &aPOST, DWORD dwOpenAndTakeUserNum)
{
    //TCHAR szQuery[512 + POST_MAX_CONTENT_LENGTH] = {0,};    
    //int nReturn = 0;

    SYSTEMTIME st;
    GetLocalTime(&st);
    CTime cTime(st);
    CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");

    //
    //Swap the Recver and Sender of aPOST
    //iRecver sends Charge to iSender.
    //
    CString csTitle		= CString(ID2SERVERTEXT("POSTBOX_CHARGE_TITLE"));
    CString csContent	= CString(ID2SERVERTEXT("POSTBOX_CHARGE_CONTENT"));

    csTitle.Replace("'", "''");
    csContent.Replace("'", "''");

    LONGLONG llMoney = GLogicData::GetInstance().MultiplyBigValueByPoint( aPOST.m_llCharge, (1.0f - GLCONST_POST::fPOST_COMMISSION ) );
    //LONGLONG llMoney = (LONGLONG)(aPOST.m_llCharge * (1.0f - GLCONST_POST::fPOST_COMMISSION));

    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_RPARAM_BIGINT(Ado);
    
    APPEND_IPARAM_TINYINT(Ado, "@PostState", EMPOST_UNREAD);
    APPEND_IPARAM_TINYINT(Ado, "@PostType", EMPOST_MONEY);
    APPEND_IPARAM_TINYINT(Ado, "@PostClass", EMPOST_SYSTEM);
    APPEND_IPARAM_INT    (Ado, "@Sender", aPOST.m_iRecver);
    APPEND_IPARAM_INT    (Ado, "@Recver", aPOST.m_iSender);
    APPEND_IPARAM_VARCHAR(Ado, "@SenderName", aPOST.m_RecverName, strlen(aPOST.m_RecverName));
    APPEND_IPARAM_VARCHAR(Ado, "@RecverName", aPOST.m_SenderName, strlen(aPOST.m_SenderName));
    APPEND_IPARAM_VARCHAR(Ado, "@SendDate", strTime.GetString(), strTime.GetLength());
    int Design = 0;
    APPEND_IPARAM_INT    (Ado, "@Design", Design);
    LONGLONG Charge = 0;
    APPEND_IPARAM_BIGINT (Ado, "@Charge", Charge);
    APPEND_IPARAM_VARCHAR(Ado, "@Title", csTitle.GetString(), csTitle.GetLength());
    APPEND_IPARAM_VARCHAR(Ado, "@Content", csContent.GetString(), csContent.GetLength());
    APPEND_IPARAM_BIGINT (Ado, "@AttachMoney", llMoney);
    
    _variant_t vartBLOB;
    vartBLOB.vt = VT_NULL;    
    ULONG nSizeOfChunk = sizeof(_variant_t);
    APPEND_IPARAM_IMAGE  (Ado, "@Attach01", vartBLOB, static_cast<long> (nSizeOfChunk));

	APPEND_IPARAM_INT    ( Ado, "@SenderUserNum", dwOpenAndTakeUserNum );

    APPEND_OPARAM_BIGINT(Ado, "@nReturn");

    LONGLONG llPostID = ExecuteStoredProcedureLonglongReturn("dbo.sp_PostSend", Ado, "@nReturn");

    // Now llPostID has PostID or ErrorCode.
    if (llPostID < sc::db::DB_OK)	//In case of Error Code
    {
        return (int)llPostID;		//Not PostID. It's ErrorCode
    }
    return sc::db::DB_OK;
}

int AdoManager::PostRnSelectAll( std::vector<PostInfo>& vecList OUT, DWORD dwRecvChaNum ) 
 {
	vecList.clear();

	sc::db::AdoExt Ado(m_GameDBConnString);
	Ado.AppendIParamInteger( "@RecvChaDbNum", dwRecvChaNum );

	if (!Ado.Execute4Cmd("dbo.PostRnSelectAll", adCmdStoredProc))	
		return sc::db::DB_ERROR;

	if (Ado.GetEOF())
		return sc::db::DB_OK;

	do
	{
		PostInfo postInfo;

		_variant_t Date;            // ExpirationDate
		std::vector<BYTE> vBuffer;  // RegistItem

		Ado.GetCollect("PostID",		postInfo.dwPostID);
		Ado.GetCollect("SendChaNum",	postInfo.dwSendChaNum);
		Ado.GetCollect("SendChaName",	postInfo.strSendChaName);
		Ado.GetCollect("SendUserNum",	postInfo.dwSendUserNum);
		Ado.GetCollect("RecvChaNum",	postInfo.dwRecvChaNum);
		Ado.GetCollect("SendDate",		Date);
		Ado.GetCollect("Style",			postInfo.wStyle);
		Ado.GetCollect("MailType",		postInfo.wMailType);
		Ado.GetCollect("AttachType",	postInfo.wAttachType);
		Ado.GetCollect("Read",			postInfo.bRead);
		Ado.GetCollect("Money",			postInfo.llMoney);
		Ado.GetChunk  ("Item",			vBuffer);
		Ado.GetCollect("Title",			postInfo.strTitle);
		Ado.GetCollect("Content",		postInfo.strContent);
		

		// ExpirationDate
		if( Date.GetVARIANT().vt == VT_DATE )
			postInfo.tSendDate = sc::time::GetTime(Date);

		// RegistItem
		se::ByteStream ByteStream(vBuffer);
		SITEMCUSTOM ITEMCUSTOM(NATIVEID_NULL());
		if (!ByteStream.IsEmpty())
			ITEMCUSTOM.AutoAssign( ByteStream );
		postInfo.sItemCutom = ITEMCUSTOM;


		vecList.push_back( postInfo );

	} while (Ado.Next());

	return  sc::db::DB_OK;
}

int AdoManager::PostRnSetRead( DWORD dwPostID )
{
	// 성공. 실패와 크게 연관 없다.
	sc::db::AdoExt Ado(m_GameDBConnString);
	{
		Ado.AppendIParamInteger( "@PostID", dwPostID );
	}
	
	Ado.ExecuteStoredProcedure( "dbo.PostRnSetRead" );
	return sc::db::DB_OK;
}

int AdoManager::PostRnDelAttach( DWORD dwPostID )
{
	sc::db::AdoExt Ado(m_GameDBConnString);
	{
		Ado.AppendRParamInteger();
		Ado.AppendIParamInteger( "@PostID", dwPostID );
		Ado.AppendOParamInteger( "@nRteturn" );
	}

	int nReturn = Ado.ExecuteStoredProcedureIntReturn( "dbo.PostRnDelAttach", "@nRteturn" );
	return nReturn;
}

int AdoManager::PostRnDel( DWORD dwPostID )
{
	sc::db::AdoExt Ado(m_GameDBConnString);
	{
		Ado.AppendRParamInteger();
		Ado.AppendIParamInteger( "@PostID", dwPostID );
		Ado.AppendOParamInteger( "@nRteturn" );
	}


	int nReturn = Ado.ExecuteStoredProcedureIntReturn( "dbo.PostRnDel", "@nRteturn" );
	return nReturn;
}

int AdoManager::PostRnSend(DWORD dwSendChaDBNum, 
						   std::string strSendChaName, 
						   DWORD dwSendUserNum, 
						   DWORD dwRecvChaDbNum, 
						   int nStyle,
						   int wMailType,
						   int wAttachType,
						   int wMoneyType,
						   LONGLONG llMoney, 
						   SITEMCUSTOM sItemCustom,
						   std::string strTitle, 
						   std::string strContent)
{

	sc::db::AdoExt Ado(m_GameDBConnString);
	APPEND_RPARAM_BIGINT(Ado);

	//아이템
	SITEMCUSTOM& sItem = sItemCustom;
	_variant_t vartBLOB;
	ULONG nSizeOfChunk = 0;
	se::ByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize = 0;

	ByteStream.ClearBuffer();
	ByteStream << SITEMCUSTOM::VERSION;
	ByteStream << (DWORD) sizeof(SITEMCUSTOM);
	dwSize = (DWORD) 1;
	ByteStream << dwSize;
	ByteStream.WriteBuffer((LPBYTE) &sItem, (DWORD) sizeof(SITEMCUSTOM) * dwSize);        
	if (!Ado.MakeChunk(vartBLOB, ByteStream, nSizeOfChunk))
		sc::writeLogError( sc::string::format( "PostSend Makechunk Size %1%", nSizeOfChunk));

	Ado.AppendIParamInteger( "@SendChaNum", dwSendChaDBNum );
	Ado.AppendIParamVarchar( "@SendChaName", strSendChaName.c_str(), strSendChaName.length() );
	Ado.AppendIParamInteger( "@SendUserNum", dwSendUserNum );
	Ado.AppendIParamInteger( "@RecvChaNum", dwRecvChaDbNum );
	Ado.AppendIParamSmall  ( "@Style", nStyle );
	Ado.AppendIParamSmall  ( "@MailType", wMailType );
	Ado.AppendIParamSmall  ( "@AttachType", wAttachType );
	Ado.AppendIParamMoney  ( "@Money", llMoney );
	Ado.AppendIParamImage  ( "@Item", vartBLOB, nSizeOfChunk );
	if( strTitle.empty() )
		Ado.AppendIParamInteger( "@Title", NULL );
	else
		Ado.AppendIParamVarchar( "@Title", strTitle.c_str(), strTitle.length() );
	if( strContent.empty() )
		Ado.AppendIParamInteger( "@Content", NULL );
	else
		Ado.AppendIParamVarchar( "@Content", strContent.c_str(), strContent.length() );
	Ado.AppendOParamInteger( "@nPostID" );

	int nReturn = Ado.ExecuteStoredProcedureIntReturn( "dbo.PostRnSend", "@nPostID" );

	return nReturn;
}

int AdoManager::PostRnReturn( DWORD dwPostID, DWORD dwReturnChaNum, std::string strReturnChaName, DWORD dwReturnUserNum, DWORD dwRecvChaNum )
{
	sc::db::AdoExt Ado(m_GameDBConnString);
	{
		Ado.AppendRParamInteger();
		Ado.AppendIParamInteger( "@PostID", dwPostID );
		Ado.AppendIParamInteger( "@ReturnChaNum", dwReturnChaNum );
		Ado.AppendIParamVarchar( "@ReturnChaName", strReturnChaName.c_str(), strReturnChaName.length() );
		Ado.AppendIParamInteger( "@RecvChaNum", dwReturnUserNum );
		Ado.AppendOParamInteger( "@nRteturn" );
	}

	int nReturn = Ado.ExecuteStoredProcedureIntReturn( "dbo.PostRnDelAttach", "@nRteturn" );
	return nReturn;
}

int AdoManager::PostCharacterCheck( DWORD dwRecvChaDbNum )
{
	sc::db::AdoExt Ado(m_GameDBConnString);
	APPEND_RPARAM_INT(Ado);

	APPEND_IPARAM_BIGINT (Ado, "@RecvChaDbNum", dwRecvChaDbNum);
	APPEND_OPARAM_INT(Ado, "@nReturn");

	int nReturn = Ado.ExecuteStoredProcedureIntReturn( "dbo.PostRnChaCheck", "@nReturn" );

	return nReturn;
}

} // namespace db