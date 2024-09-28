//
//mjeon.ado
// 2010.12.29
//

#include "stdafx.h"

#include "../../../SigmaCore/Database/DbDefine.h"
#include "../../../SigmaCore/Math/SeqUniqueGuid.h"

#include "ADOClass.hpp"
#include "ADOManager.h"

#include "../../../SigmaCore/Net/NetUtil.h"
#include "../../../SigmaCore/Util/DateTime.h"

#include "../../DataManager/CDataManagerProduct.h"
#include "../../DataManager/CDataManagerClubInven.h"

#include "../../../RanLogic/Item/GLItem.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Character/CharacterSlot/GLCharSlotMan.h"

#include "../../../RanLogicServer/Database/DbDefineLog.h"
#include "../../../RanLogicServer/Club/GLClubAgent.h"


#include <set>
#include <map>
//
// Test routine for BLOB handling
//
int CADOManager::TestBLOB()
{
	CString csQuery;



	//
	// GetChunk	
	//=====================================================================
	csQuery.Empty();
// 	csQuery.Format(_T(
// 					"SELECT ChaPutOnItems FROM ChaInfo WHERE ChaNum = 644"
// 					));

	/*
		GETCHUNK with SP

	
	 StoredProcedure
	
	CREATE PROCEDURE [dbo].[sp_TestSP]	
	AS
		SET NOCOUNT ON
	
	 	DECLARE
	 		@error_var int, 
	 		@rowcount_var int	
	 				
	 	BEGIN TRAN
	 	
	 	SELECT ChaPutOnItems FROM ChaInfo WHERE ChaNum = 644		
	 	
	 	SET NOCOUNT OFF
	 	RETURN 0
	*/
	

	csQuery.Format(_T(
					//"sp_TestSP"
					"SELECT ChaName, ChaPutOnItems FROM ChaInfo WHERE ChaNum = 2000"
					));



	CjADO ado(m_csGameDBConnString);

	//APPEND_IPARAM_INT(ado, "@ChaNum", 644);

	//if ( !EXECUTE_GETIMAGE(ado, csQuery) )//!ado.Execute4RS(csQuery, adOpenDynamic, adLockPessimistic, adCmdText) )
	//if ( !EXECUTE_SP(ado, csQuery) )
	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;


	CString		csName;
	se::ByteStream bytestream;
	bytestream.ClearBuffer();

	BYTE BYTEBUFFER[5000];
	ZeroMemory(BYTEBUFFER, sizeof(BYTEBUFFER));


	BEGIN_GETCHUNK(ado)
		ado.GetCollect("ChaName", csName);
		//ado.GetChunk("ChaPutOnItems", bytestream);
		ado.GetChunk("ChaPutOnItems", BYTEBUFFER);
	END_GETCHUNK(ado)
	//=====================================================================


	LPBYTE  pBuffer = NULL;
	DWORD	dwSize = 0;

	bytestream.GetBuffer(pBuffer, dwSize);




	/*
		SETCHUNK with SP


	CREATE PROCEDURE [dbo].[sp_TestSetChunkSP]
		@Chunk IMAGE
	AS
	SET NOCOUNT ON
	
	DECLARE
		@error_var int, 
		@rowcount_var int	
				
	BEGIN TRAN
	
	--SELECT ChaPutOnItems FROM ChaInfo WHERE ChaNum = 644
	
	UPDATE ChaInfo Set ChaPutOnItems=@Chunk WHERE ChaNum = 2000
	
	SET NOCOUNT OFF
	RETURN 0
	*/

	//
	// SetChunk	
	//=====================================================================
	csQuery.Empty();
// 	csQuery.Format(_T(					
// 					"UPDATE ChaInfo Set ChaPutOnItems=? WHERE ChaNum = 2000"
// 					));
	csQuery.Format(_T(
					"sp_TestSetChunkSP"
					));


	CjADO ado2(m_csGameDBConnString);

	_variant_t	vartBLOB;
	ULONG		nSizeOfChunk = 0;

	ado2.MakeChunk(vartBLOB, bytestream, nSizeOfChunk);

	APPEND_IPARAM_IMAGE(ado2, "@Chunk", vartBLOB, (long)nSizeOfChunk);
	

	if ( !EXECUTE_SETIMAGE_SP(ado2, csQuery) )
		return DB_ERROR;
	//=====================================================================


	return DB_OK;
}

int CADOManager::GmLogin( CString strGMID, CString strGMPassword, CString strUserIP )
{	
	CString csQuery;
	CString csUserDBConnString;

	int		nUserNum= 0;
	int		nReturn = 0;	

	csQuery.Empty();
	csQuery.Format(_T("gm_login"));
	

	CjADO ado(m_csUserDBConnString);


	APPEND_RPARAM_INT(ado);
	APPEND_IPARAM_VARCHAR(ado, "@userId", strGMID, strGMID.GetLength());	
	APPEND_IPARAM_VARCHAR(ado, "@userPass", strGMPassword, strGMPassword.GetLength());	
	APPEND_IPARAM_VARCHAR(ado, "@userIP", strUserIP, strUserIP.GetLength());
	APPEND_OPARAM_INT(ado, "@nGmNum");
	APPEND_OPARAM_INT(ado, "@nReturn");

	if ( !EXECUTE_SP(ado, csQuery) )
		return DB_ERROR;

	_variant_t varGmNum;
	if ( !ado.GetParam("@nGmNum", varGmNum) )
		return DB_ERROR;

	nUserNum = varGmNum.intVal;
	
	_variant_t varReturn;
	if ( !ado.GetParam("@nReturn", varReturn) )
		return DB_ERROR;

	 nReturn = varReturn.intVal;

	 //현재 로그인한 GM 정보 설정
	 SetUpGmInfo(nUserNum, strGMID, strUserIP);

	return nReturn;
}


int CADOManager::GetGmUserNum()
{
	return m_nGmUserNum;	//login 성공한 GM의 UserNum
}


int CADOManager::GetUserNum( CString strUserID )
{
	CString csQuery;

	csQuery.Empty();

#ifdef _GS
	csQuery.Format(_T("SELECT UserNum From GSUserInfo WHERE UserID='%s'"), strUserID.GetBuffer());
#else
	csQuery.Format(_T("SELECT UserNum From UserInfo WHERE UserID='%s'"), strUserID.GetBuffer());
#endif


	CjADO ado(m_csUserDBConnString);

	if ( !ado.Execute(csQuery) )
		return DB_ERROR;


	int nUserNum = 0;


	BEGIN_GETCOLLECT(ado)
		ado.GetCollect(0, nUserNum);
	END_GETCOLLECT(ado)


	if ( !ado.IsCollectedAll() )
		return DB_ERROR;

	return nUserNum;
}


//
//in case of ADO, server group information will be handled by another binary data file.
//
int	CADOManager::GetServerInfo( std::vector<viewServerInfo> &v )
{
	return DB_OK;
}


int CADOManager::GetAllChar( std::vector<AllCharNum> &v )
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T("SELECT ChaNum, UserNum, ChaName FROM ChaInfo ORDER BY ChaNum"));


	CjADO ado(m_csGameDBConnString);

	if ( !ado.Execute(csQuery) )
		return DB_ERROR;


	AllCharNum AllChar;
	
	BEGIN_GETCOLLECT(ado)
		//ZeroMemory(&AllChar, sizeof(AllChar)); DO NOT call ZeroMemory: the structure includes some other objects such like CString.
		ado.GetCollect(0, AllChar.ChaNum);
		ado.GetCollect(1, AllChar.UserNum);
		ado.GetCollect(2, AllChar.ChaName);

		if ( !ado.IsCollectedAll() )			
			return DB_ERROR;

		v.push_back(AllChar);		

	END_GETCOLLECT(ado)


	return DB_OK;
}

int CADOManager::GetAllInven( std::vector<AllCharNum> &v )
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
					"SELECT A.ChaNum, A.UserNum, A.ChaName FROM ChaInfo AS A, "							\
						"(SELECT  UserNum, MIN(ChaNum) AS ChaNum FROM ChaInfo GROUP BY UserNum) AS B "	\
					"WHERE A.ChaNum = B.ChaNum "														\
					"ORDER BY A.UserNum "
					));

	
	CjADO ado(m_csGameDBConnString);

	if ( !ado.Execute(csQuery) )
		return DB_ERROR;


	AllCharNum AllChar;	
	
	BEGIN_GETCOLLECT(ado)
		//ZeroMemory(&AllChar, sizeof(AllChar)); DO NOT call ZeroMemory here: the structure includes some other objects such like CString.
		ado.GetCollect(0, AllChar.ChaNum);
		ado.GetCollect(1, AllChar.UserNum);
		ado.GetCollect(2, AllChar.ChaName);

		if ( !ado.IsCollectedAll() )		
			return DB_ERROR;

		v.push_back(AllChar);

	END_GETCOLLECT(ado)
	
	return DB_OK;
}

int CADOManager::GetExchangeItemLog( const std::vector<int>& GenType, const std::vector<int>& ExchangFlag, std::vector<ExchangeItemLog>& vOut )
{
    CString csQuery;

    std::string strGenTypeNumbers = "";
    std::string strExchangeFlagNumbers = "";

    for ( size_t i = 0; i < GenType.size(); i++ )
    {
        if ( i == 0 )
            strGenTypeNumbers += sc::string::format( "%1%", GenType[i] );
        else
            strGenTypeNumbers += sc::string::format( ",%1%", GenType[i] );
    }

    for ( size_t i = 0; i < ExchangFlag.size(); i++ )
    {
        if ( i == 0 )
            strExchangeFlagNumbers += sc::string::format( "%1%", ExchangFlag[i] );
        else
            strExchangeFlagNumbers += sc::string::format( ",%1%", ExchangFlag[i] );
    }

    if ( GenType.size() > 0 && ExchangFlag.size() > 0 )
    {
        csQuery.Empty();
        csQuery.Format(_T(
            "SELECT CONVERT(BIGINT, MakeNum) AS MakeNum, COUNT(*) AS DupCount " \
            "FROM LogItemExchange "		                					    \
            "WHERE MakeNum <> 0 AND MakeType IN(%s) AND ExchangeFlag IN(%s) "	\
            "GROUP BY MakeNum having COUNT(*) > 1 ORDER BY DupCount DESC "     	\
            ), strGenTypeNumbers.c_str(), strExchangeFlagNumbers.c_str() );
    }
    else if ( GenType.size() > 0 )
    {
        csQuery.Empty();
        csQuery.Format(_T(
            "SELECT CONVERT(BIGINT, MakeNum) AS MakeNum, COUNT(*) AS DupCount " \
            "FROM LogItemExchange "						                	    \
            "WHERE MakeNum <> 0 AND MakeType IN(%s) "                           \
            "GROUP BY MakeNum having COUNT(*) > 1 ORDER BY DupCount DESC "      \
            ), strGenTypeNumbers.c_str() );
    }
    else if ( ExchangFlag.size() > 0 )
    {
        csQuery.Empty();
        csQuery.Format(_T(
            "SELECT CONVERT(BIGINT, MakeNum) AS MakeNum, COUNT(*) AS DupCount " \
            "FROM LogItemExchange "             							    \
            "WHERE MakeNum <> 0 AND ExchangeFlag IN(%s) "                    	\
            "GROUP BY MakeNum having COUNT(*) > 1 ORDER BY DupCount DESC "      \
            ), strExchangeFlagNumbers.c_str() );
    }
    else
    {
        csQuery.Empty();
        csQuery = CString(_T(
            "SELECT CONVERT(BIGINT, MakeNum) AS MakeNum, COUNT(*) AS DupCount " \
            "FROM LogItemExchange "             							    \
            "WHERE MakeNum <> 0 "                    	                        \
            "GROUP BY MakeNum having COUNT(*) > 1 ORDER BY DupCount DESC "      \
            ) );
    }


    CjADO ado(m_csLogDBConnString);

    if ( !ado.Execute(csQuery) )
        return DB_ERROR;

    ExchangeItemLog ExchangeItem;	

    BEGIN_GETCOLLECT(ado)
        //ZeroMemory(&ExchangeItem, sizeof(ExchangeItem)); DO NOT call ZeroMemory here: the structure includes some other objects such like CString.
        ado.GetCollect(0, ExchangeItem.MakeNum);
        ado.GetCollect(1, ExchangeItem.Count);

    if ( !ado.IsCollectedAll() )		
        return DB_ERROR;

    vOut.push_back(ExchangeItem);

    END_GETCOLLECT(ado)

    return DB_OK;
}

int CADOManager::GetExchanageItemHistory( LONGLONG InMakeNum, std::vector<ExchangeItemHistory>& vOut )
{
    CString csQuery;

    std::string strQueryTemp;
    strQueryTemp = sc::string::format(_T(
        "SELECT CONVERT(BIGINT, ExchangeNum) AS ExchangeNum, NIDMain, NIDSub, SGNum, SvrNum, FldNum "   \
        ", MakeType, CONVERT(BIGINT, MakeNum) AS MakeNum, ItemAmount, ItemFromFlag, ItemFrom "		    \
        ", ItemToFlag, ItemTo, ExchangeFlag, ExchangeDate, CONVERT(BIGINT, TradePrice) AS TradePrice "	\
        "FROM LogItemExchange "	            						                                    \
        "WHERE MakeNum = %1% "                                       	                                \
        "ORDER BY ExchangeDate DESC "                                  	                                \
        ), InMakeNum );

    csQuery.Empty();
    csQuery.Format( _T("%s"), strQueryTemp.c_str() );

    CjADO ado(m_csLogDBConnString);

    if ( !ado.Execute(csQuery) )
        return DB_ERROR;

    ExchangeItemHistory sExChangeHistory;

    BEGIN_GETCOLLECT(ado)
        //ZeroMemory(&ExchangeItem, sizeof(ExchangeItem)); DO NOT call ZeroMemory here: the structure includes some other objects such like CString.
        ado.GetCollect(0, sExChangeHistory.ExchangeNum);
        ado.GetCollect(1, sExChangeHistory.NIDMain);
        ado.GetCollect(2, sExChangeHistory.NIDSub);
        ado.GetCollect(3, sExChangeHistory.SGNum);
        ado.GetCollect(4, sExChangeHistory.SvrNum);
        ado.GetCollect(5, sExChangeHistory.FldNum);
        ado.GetCollect(6, sExChangeHistory.MakeType);
        ado.GetCollect(7, sExChangeHistory.MakeNum);
        ado.GetCollect(8, sExChangeHistory.ItemAmount);
        ado.GetCollect(9, sExChangeHistory.ItemFromFlag);
        ado.GetCollect(10, sExChangeHistory.ItemFrom);
        ado.GetCollect(11, sExChangeHistory.ItemToFlag);
        ado.GetCollect(12, sExChangeHistory.ItemTo);
        ado.GetCollect(13, sExChangeHistory.ExchangeFlag);
        ado.GetCollect(14, sExChangeHistory.ExchangeDate);
        ado.GetCollect(15, sExChangeHistory.TradePrice);

    if ( !ado.IsCollectedAll() )		
        return DB_ERROR;

    vOut.push_back(sExChangeHistory);

    END_GETCOLLECT(ado)

    return DB_OK;
}

int CADOManager::SearchCharacter(const std::string& Query, std::vector<CharInfo>& v)
{
    CjADO ado(m_csGameDBConnString);
    
    if (!ado.Execute(Query.c_str()))
        return DB_ERROR;

    return SearchCharacter(ado, v);
}

int CADOManager::SearchCharacter( std::strstream& strSearch, std::vector<CharInfo> &v )
{
	//TestBLOB();
	CString csQuery(strSearch.str());
	CjADO ado(m_csGameDBConnString);
	if (!ado.Execute(csQuery))
		return DB_ERROR;

    return SearchCharacter(ado, v);
}

int CADOManager::SearchCharacter(CjADO& Ado, std::vector<CharInfo>& v)
{
    CharInfo charinfo;
	CString  csTemp;

    if (Ado.GetEOF())
        return FALSE;

    do
    {
        //ZeroMemory(&charinfo, sizeof(charinfo)); DO NOT call ZeroMemory here: the structure includes some other objects such like CString.
		Ado.GetCollect(0,  charinfo.ChaNum);
		Ado.GetCollect(1,  charinfo.UserNum);
		Ado.GetCollect(2,  charinfo.ChaName);
		Ado.GetCollect(3,  charinfo.ChaTribe);
		Ado.GetCollect(4,  charinfo.ChaClass);
		Ado.GetCollect(5,  charinfo.SGNum);
		Ado.GetCollect(6,  charinfo.ChaLevel);
		Ado.GetCollect(7,  charinfo.ChaPower);
		Ado.GetCollect(8,  charinfo.ChaStrong);
		Ado.GetCollect(9,  charinfo.ChaStrength);
		Ado.GetCollect(10, charinfo.ChaSpirit);
		Ado.GetCollect(11, charinfo.ChaDex);
		Ado.GetCollect(12, charinfo.ChaIntel);
		Ado.GetCollect(13, charinfo.ChaStRemain);
		Ado.GetCollect(14, charinfo.ChaMoney);
		Ado.GetCollect(15, charinfo.ChaExp);
		Ado.GetCollect(16, charinfo.ChaOnline);
		Ado.GetCollect(17, charinfo.ChaDeleted);

// 		if ( !Ado.IsCollectedAll() )		
// 			return DB_ERROR;

		v.push_back(charinfo);

    } while (Ado.Next());

	return DB_OK;
}


int CADOManager::CreateNewCharacter( SCHARDATA2* pCharData2, int nSvrNum )
{
	if (!pCharData2)
	{
		return DB_ERROR;
	}

	CString csQuery;

	int		nChaNum = 0;
	DWORD	dwUserNum = pCharData2->GetUserID();
	DWORD	dwSvrNum = (DWORD)nSvrNum;


	//
	//Check the user can create a new character.
	//
	csQuery.Empty();
	csQuery.Format(_T(
					"SELECT COUNT(*) FROM ChaInfo WHERE ChaDeleted=0 AND UserNum=%d "					\
		), (int)dwUserNum);


	CjADO ado(m_csGameDBConnString);

	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;

	UINT	nTotalChaNum = 0;


	BEGIN_GETCOLLECT(ado)

		ado.GetCollect(0,  nTotalChaNum);

	END_GETCOLLECT(ado)


	if (nTotalChaNum >= MAX_ONESERVERCHAR_NUM_GMTOOL )
	{		
		return DB_CHA_MAX;
	}

	//
	//Let's create a new character here with just one query.
	//
	csQuery.Empty();
	csQuery.Format(_T(
					"SET NOCOUNT ON;"													\
					"INSERT INTO ChaInfo "												\
						"("																\
						"UserNum, SGNum, ChaName, ChaTribe, ChaClass, "					\
						"ChaBright, ChaLevel, ChaMoney, ChaDex, ChaIntel, "				\
						"ChaStrong, ChaPower, ChaSpirit, ChaStrength, ChaStRemain, "	\
						"ChaAttackP, ChaDefenseP, ChaFightA, ChaShootA, ChaExp, "		\
						"ChaSkillPoint, ChaHP, ChaMP, ChaSP, ChaPK, "					\
						"ChaStartMap, ChaStartGate, ChaPosX, ChaPosY, ChaPosZ, "		\
						"ChaHair, ChaHairColor, "										\
						"ChaSkills, ChaSkillSlot, ChaPutOnItems, ChaSkillFact, ChaInven "\
						") "															\
					"VALUES "															\
						"( "															\
						"%d, %d, '%s', %d, %d, "										\
						"%d, %d, %I64d, %d, %d, "										\
						"%d, %d, %d, %d, %d, "											\
						"%d, %d, %d, %d, %I64d, "										\
						"%d, %d, %d, %d, %d, "											\
						"%d, %d, %f, %f, %f, "											\
						"%d, %d, "														\
						"?, ?, ?, ?, ? "												\
						//"'', '', '', '' "												
						"); SELECT SCOPE_IDENTITY() "
		), dwUserNum, /*dwSvrNum*/pCharData2->m_dwServerID, pCharData2->m_szName, (int)pCharData2->m_emTribe, (int)pCharData2->m_emClass
		, pCharData2->m_nBright, (int)pCharData2->m_wLevel, pCharData2->GetInvenMoney(), (int)pCharData2->m_sStats.wDex, (int)pCharData2->m_sStats.wInt
		, (int)pCharData2->m_sStats.wStr, (int)pCharData2->m_sStats.wPow, (int)pCharData2->m_sStats.wSpi, (int)pCharData2->m_sStats.wSta, (int)pCharData2->m_wStatsPoint
		, pCharData2->m_powerAttack, pCharData2->m_powerDefence, (int)pCharData2->m_powerAttribute[SKILL::EMAPPLY_MELEE], (int)pCharData2->m_powerAttribute[SKILL::EMAPPLY_RANGE], pCharData2->m_sExperience.lnNow
		, (int)pCharData2->m_dwSkillPoint, pCharData2->m_sHP.nNow, pCharData2->m_sMP.nNow, pCharData2->m_sSP.nNow, (int)pCharData2->m_wPK
		, (int)pCharData2->m_sStartMapID.dwID, (int)pCharData2->m_dwStartGate, pCharData2->m_vStartPos.x, pCharData2->m_vStartPos.y, pCharData2->m_vStartPos.z		
		, (int)pCharData2->m_wHair, (int)pCharData2->m_wHairColor
		);


	
	if ( !REOPEN(ado) )
	{
		return DB_ERROR;
	}


	//
	// handling Chunks
	//

	se::ByteStream	bytestream;
	ULONG			nSizeOfData = 0;

	_variant_t		vartSKILLS;
	_variant_t		vartSKILLSLOT;
	_variant_t		vartPUTONITEMS;
	_variant_t		vartSKILLFACT;
	_variant_t		vartINVEN;		


	//
	// ChaSkills
	//
	if ( pCharData2->GETEXPSKILLS_BYBUF(bytestream) )
	{
		if ( ado.MakeChunk(vartSKILLS, bytestream, nSizeOfData) )
		{
			if ( !APPEND_IPARAM_IMAGE(ado, "ChaSkills", vartSKILLS, nSizeOfData) )
				return DB_ERROR;
		}
		else
			return DB_ERROR;
	}
	else
		return DB_ERROR;

	
	//
	// ChaSkillSlot
	//
	if ( pCharData2->GETSKILL_QUICKSLOT(bytestream) )
	{
		if ( ado.MakeChunk(vartSKILLSLOT, bytestream, nSizeOfData) )
		{
			if ( !APPEND_IPARAM_IMAGE(ado, "ChaSkillSlot", vartSKILLSLOT, nSizeOfData) )
				return DB_ERROR;
		}
		else
			return DB_ERROR;
	}
	else
		return DB_ERROR;


	//
	// ChaPutOnItems
	//
	if ( pCharData2->GETPUTONITEMS_BYBUF(bytestream) )
	{
		if ( ado.MakeChunk(vartPUTONITEMS, bytestream, nSizeOfData) )
		{
			if ( !APPEND_IPARAM_IMAGE(ado, "ChaPutOnItems", vartPUTONITEMS, nSizeOfData) )
				return DB_ERROR;
		}
		else
			return DB_ERROR;
	}
	else
		return DB_ERROR;


	//
	// ChaSkillFact
	//
	if ( pCharData2->GETSKILLFACT_BYBUF(bytestream) )
	{
		if ( ado.MakeChunk(vartSKILLFACT, bytestream, nSizeOfData) )
		{
			if ( !APPEND_IPARAM_IMAGE(ado, "ChaSkillFact", vartSKILLFACT, nSizeOfData) )
				return DB_ERROR;
		}
		else
			return DB_ERROR;
	}
	else
		return DB_ERROR;
	

	//
	// ChaInven
	//
	if ( pCharData2->GETINVENTORYE_BYBUF(bytestream) )
	{
		if ( ado.MakeChunk(vartINVEN, bytestream, nSizeOfData) )
		{
			if ( !APPEND_IPARAM_IMAGE(ado, "ChaInven", vartINVEN, nSizeOfData) )
				return DB_ERROR;
		}
		else
			return DB_ERROR;
	}
	else
		return DB_ERROR;
	
	if ( !EXECUTE_SETIMAGE(ado, csQuery) )
		return DB_ERROR;

	
	DWORD		dwNewChaNum	= 0;

	BEGIN_GETCOLLECT(ado)
		ado.GetCollect(0, dwNewChaNum);
	END_GETCOLLECT(ado)

	return dwNewChaNum;
}

int CADOManager::CreateNewCharacter2( SCHARDATA2* pCharData2, int nSvrNum )
{
	if (!pCharData2)
	{
		return DB_ERROR;
	}

	CString csQuery;

	int nChaNum = 0;
	DWORD dwUserNum = pCharData2->GetUserID();
	DWORD dwSvrNum = ( DWORD )nSvrNum;


	//
	//Check the user can create a new character.
	//
	csQuery.Empty();
	csQuery.Format( _T( "SELECT COUNT(*) FROM ChaInfo WHERE ChaDeleted=0 AND UserNum=%d" ), ( int )dwUserNum );

	CjADO ado( m_csGameDBConnString );

	if ( !EXECUTE_RAW( ado, csQuery ) )
		return DB_ERROR;

	UINT nTotalChaNum = 0;
	BEGIN_GETCOLLECT( ado )
		ado.GetCollect( 0,  nTotalChaNum );
	END_GETCOLLECT( ado )

	if ( nTotalChaNum >= MAX_ONESERVERCHAR_NUM_GMTOOL )
	{		
		return DB_CHA_MAX;
	}

	if ( !REOPEN( ado ) )
	{
		return DB_ERROR;
	}

	// 테이블 구조 먼저 알아보기
	csQuery.Empty();
	csQuery = CString(_T(
		"IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[ChaNumSeed]') AND type in (N'U')) "	\
		"BEGIN "		\
		"	SELECT 1 "	\
		"END "			\
		"ELSE "			\
		"BEGIN "		\
		"	SELECT 0 "	\
		"END "			\
		) );

	if ( !EXECUTE_RAW( ado, csQuery ) )
		return DB_ERROR;

	int UseChaNumSeed = 0;
	BEGIN_GETCOLLECT( ado )
		ado.GetCollect( 0,  UseChaNumSeed );
	END_GETCOLLECT( ado )

	if ( !REOPEN(ado) )
	{
		return DB_ERROR;
	}

	DWORD ChaNum = 0;
	if ( 1 == UseChaNumSeed )
	{
		// 캐릭터번호 미리 받아오기
		csQuery.Empty();
		csQuery = CString(_T(
			"SET NOCOUNT ON "					\
			"DECLARE @seed int "				\
			"EXEC @seed = dbo.GetChaNumSeed "	\
			"SELECT @seed "						\
			) );

		if ( !EXECUTE_RAW( ado, csQuery ) )
			return DB_ERROR;

		BEGIN_GETCOLLECT( ado )
			ado.GetCollect( 0,  ChaNum );
		END_GETCOLLECT( ado )

		if ( !REOPEN(ado) )
		{
			return DB_ERROR;
		}

		//
		//Let's create a new character here with just one query.
		//
		csQuery.Empty();
		csQuery.Format(_T(
			"SET NOCOUNT ON;"													\
			"INSERT INTO ChaInfo "												\
			"("																	\
			"ChaNum, UserNum, SGNum, ChaName, ChaTribe, ChaClass, "				\
			"ChaBright, ChaLevel, ChaMoney, ChaDex, ChaIntel, "					\
			"ChaStrong, ChaPower, ChaSpirit, ChaStrength, ChaStRemain, "		\
			"ChaAttackP, ChaDefenseP, ChaFightA, ChaShootA, ChaExp, "			\
			"ChaSkillPoint, ChaHP, ChaMP, ChaSP, ChaPK, "						\
			"ChaStartMap, ChaStartGate, ChaPosX, ChaPosY, ChaPosZ, "			\
			"ChaHair, ChaHairColor, "											\
			"ChaSkills, ChaSkillSlot, ChaSkillFact "	\
			") "																\
			"VALUES "															\
			"( "																\
			"%d, %d, %d, '%s', %d, %d, "										\
			"%d, %d, %I64d, %d, %d, "											\
			"%d, %d, %d, %d, %d, "												\
			"%d, %d, %d, %d, %I64d, "											\
			"%d, %d, %d, %d, %d, "												\
			"%d, %d, %f, %f, %f, "												\
			"%d, %d, "															\
			"?, ?, ? "													\
			");"
			), ChaNum, dwUserNum, pCharData2->m_dwServerID, pCharData2->m_szName, (int)pCharData2->m_emTribe, (int)pCharData2->m_emClass
			, pCharData2->m_nBright, (int)pCharData2->m_wLevel, pCharData2->GetInvenMoney(), (int)pCharData2->m_sStats.wDex, (int)pCharData2->m_sStats.wInt
			, (int)pCharData2->m_sStats.wStr, (int)pCharData2->m_sStats.wPow, (int)pCharData2->m_sStats.wSpi, (int)pCharData2->m_sStats.wSta, (int)pCharData2->m_wStatsPoint
			, pCharData2->m_powerAttack, pCharData2->m_powerDefence, (int)pCharData2->m_powerAttribute[SKILL::EMAPPLY_MELEE], (int)pCharData2->m_powerAttribute[SKILL::EMAPPLY_RANGE], pCharData2->m_sExperience.lnNow
			, (int)pCharData2->m_dwSkillPoint, pCharData2->m_sHP.nNow, pCharData2->m_sMP.nNow, pCharData2->m_sSP.nNow, (int)pCharData2->m_wPK
			, (int)pCharData2->m_sStartMapID.dwID, (int)pCharData2->m_dwStartGate, pCharData2->m_vStartPos.x, pCharData2->m_vStartPos.y, pCharData2->m_vStartPos.z		
			, (int)pCharData2->m_wHair, (int)pCharData2->m_wHairColor
			);
	}
	else if ( 0 == UseChaNumSeed )
	{
		//
		//Let's create a new character here with just one query.
		//
		csQuery.Empty();
		csQuery.Format(_T(
			"SET NOCOUNT ON;"													\
			"INSERT INTO ChaInfo "												\
			"("																\
			"UserNum, SGNum, ChaName, ChaTribe, ChaClass, "					\
			"ChaBright, ChaLevel, ChaMoney, ChaDex, ChaIntel, "				\
			"ChaStrong, ChaPower, ChaSpirit, ChaStrength, ChaStRemain, "	\
			"ChaAttackP, ChaDefenseP, ChaFightA, ChaShootA, ChaExp, "		\
			"ChaSkillPoint, ChaHP, ChaMP, ChaSP, ChaPK, "					\
			"ChaStartMap, ChaStartGate, ChaPosX, ChaPosY, ChaPosZ, "		\
			"ChaHair, ChaHairColor, "										\
			"ChaSkills, ChaSkillSlot, ChaSkillFact "\
			") "															\
			"VALUES "															\
			"( "															\
			"%d, %d, '%s', %d, %d, "										\
			"%d, %d, %I64d, %d, %d, "										\
			"%d, %d, %d, %d, %d, "											\
			"%d, %d, %d, %d, %I64d, "										\
			"%d, %d, %d, %d, %d, "											\
			"%d, %d, %f, %f, %f, "											\
			"%d, %d, "														\
			"?, ?, ? "												\
			//"'', '', '', '' "												
			"); SELECT SCOPE_IDENTITY() "
			), dwUserNum, /*dwSvrNum*/pCharData2->m_dwServerID, pCharData2->m_szName, (int)pCharData2->m_emTribe, (int)pCharData2->m_emClass
			, pCharData2->m_nBright, (int)pCharData2->m_wLevel, pCharData2->GetInvenMoney(), (int)pCharData2->m_sStats.wDex, (int)pCharData2->m_sStats.wInt
			, (int)pCharData2->m_sStats.wStr, (int)pCharData2->m_sStats.wPow, (int)pCharData2->m_sStats.wSpi, (int)pCharData2->m_sStats.wSta, (int)pCharData2->m_wStatsPoint
			, pCharData2->m_powerAttack, pCharData2->m_powerDefence, (int)pCharData2->m_powerAttribute[SKILL::EMAPPLY_MELEE], (int)pCharData2->m_powerAttribute[SKILL::EMAPPLY_RANGE], pCharData2->m_sExperience.lnNow
			, (int)pCharData2->m_dwSkillPoint, pCharData2->m_sHP.nNow, pCharData2->m_sMP.nNow, pCharData2->m_sSP.nNow, (int)pCharData2->m_wPK
			, (int)pCharData2->m_sStartMapID.dwID, (int)pCharData2->m_dwStartGate, pCharData2->m_vStartPos.x, pCharData2->m_vStartPos.y, pCharData2->m_vStartPos.z		
			, (int)pCharData2->m_wHair, (int)pCharData2->m_wHairColor
			);
	}

	//
	// handling Chunks
	//

	se::ByteStream	bytestream;
	ULONG			nSizeOfData = 0;

	_variant_t		vartSKILLS;
	_variant_t		vartSKILLSLOT;
	_variant_t		vartPUTONITEMS;
	_variant_t		vartSKILLFACT;
	_variant_t		vartINVEN;		


	//
	// ChaSkills
	//
	if ( pCharData2->GETEXPSKILLS_BYBUF(bytestream) )
	{
		if ( ado.MakeChunk(vartSKILLS, bytestream, nSizeOfData) )
		{
			if ( !APPEND_IPARAM_IMAGE(ado, "ChaSkills", vartSKILLS, nSizeOfData) )
				return DB_ERROR;
		}
		else
			return DB_ERROR;
	}
	else
		return DB_ERROR;


	//
	// ChaSkillSlot
	//
	if ( pCharData2->GETSKILL_QUICKSLOT(bytestream) )
	{
		if ( ado.MakeChunk(vartSKILLSLOT, bytestream, nSizeOfData) )
		{
			if ( !APPEND_IPARAM_IMAGE(ado, "ChaSkillSlot", vartSKILLSLOT, nSizeOfData) )
				return DB_ERROR;
		}
		else
			return DB_ERROR;
	}
	else
		return DB_ERROR;


	//
	// ChaPutOnItems
	//
// 	if ( pCharData2->GETPUTONITEMS_BYBUF(bytestream) )
// 	{
// 		if ( ado.MakeChunk(vartPUTONITEMS, bytestream, nSizeOfData) )
// 		{
// 			if ( !APPEND_IPARAM_IMAGE(ado, "ChaPutOnItems", vartPUTONITEMS, nSizeOfData) )
// 				return DB_ERROR;
// 		}
// 		else
// 			return DB_ERROR;
// 	}
// 	else
// 		return DB_ERROR;


	//
	// ChaSkillFact
	//
	if ( pCharData2->GETSKILLFACT_BYBUF(bytestream) )
	{
		if ( ado.MakeChunk(vartSKILLFACT, bytestream, nSizeOfData) )
		{
			if ( !APPEND_IPARAM_IMAGE(ado, "ChaSkillFact", vartSKILLFACT, nSizeOfData) )
				return DB_ERROR;
		}
		else
			return DB_ERROR;
	}
	else
		return DB_ERROR;


	//
	// ChaInven
	//
// 	if ( pCharData2->GETINVENTORYE_BYBUF(bytestream) )
// 	{
// 		if ( ado.MakeChunk(vartINVEN, bytestream, nSizeOfData) )
// 		{
// 			if ( !APPEND_IPARAM_IMAGE(ado, "ChaInven", vartINVEN, nSizeOfData) )
// 				return DB_ERROR;
// 		}
// 		else
// 			return DB_ERROR;
// 	}
// 	else
// 		return DB_ERROR;

	if ( !EXECUTE_SETIMAGE(ado, csQuery) )
		return DB_ERROR;

	if ( 0 == UseChaNumSeed )
	{
		BEGIN_GETCOLLECT(ado)
			ado.GetCollect( 0, ChaNum );
		END_GETCOLLECT(ado)
	}

	// 착용 아이템
	std::vector< SINVENITEM_SAVE > vecItemsPuton;
	pCharData2->GETPUTONITEMS_BYVECTOR( vecItemsPuton );
	size_t ItemSizePuton = vecItemsPuton.size();
	for ( size_t loop = 0; loop < ItemSizePuton; loop++ )
	{
		SINVENITEM_SAVE& sItem = vecItemsPuton[loop];
		sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
		sItem.sItemCustom.GenerateBasicStat( false );
		sItem.sItemCustom.GenerateLinkSkill();
		sItem.sItemCustom.GenerateAddOption();
		ItemInsert( pCharData2->m_CharDbNum, INVEN_PUTON, &vecItemsPuton[loop], INSERT_PUTON_ITEM_NEW_CHA );
	}

	// 인벤토리
	std::vector< SINVENITEM_SAVE > vecItemsInven;
	pCharData2->m_cInventory.GET_ITEM_BY_VECTOR( vecItemsInven );
	size_t ItemSizeInven = vecItemsInven.size();
	for ( size_t loop = 0; loop < ItemSizeInven; loop++ )
	{
		SINVENITEM_SAVE& sItem = vecItemsInven[loop];
		sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
		sItem.sItemCustom.GenerateBasicStat( false );
		sItem.sItemCustom.GenerateLinkSkill();
		sItem.sItemCustom.GenerateAddOption();
		ItemInsert( pCharData2->m_CharDbNum, INVEN_INVEN, &vecItemsInven[loop], INSERT_INVEN_ITEM_NEW_CHA );
	}

	// 퀘스트 아이템 저장하기
	{
		// 진행 퀘스트
		GLQuestPlay::MAPQUEST& quest = pCharData2->m_cQuestPlay.GetQuestProc();
		GLQuestPlay::MAPQUEST_CITER iter = quest.begin();
		GLQuestPlay::MAPQUEST_CITER iter_end = quest.end();
		for ( ; iter != iter_end; ++iter )
		{
			std::vector< SINVENITEM_SAVE > vecItems;
			GLQUESTPROG* pQuestProg = (*iter).second;
			if ( !pQuestProg )
			{
				continue;
			}
			pQuestProg->m_sINVENTORY.GET_ITEM_BY_VECTOR( vecItems );
			size_t ItemSize = vecItems.size();
			for ( size_t loop = 0; loop < ItemSize; loop++ )
			{
				// Quest Item 일 경우에 Costume 의 MID, SID 가 Quest 의 MID,SID 이다.
				SINVENITEM_SAVE& sItem = vecItems[loop];
				sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
				sItem.sItemCustom.GenerateBasicStat( false );
				sItem.sItemCustom.GenerateLinkSkill();
				sItem.sItemCustom.GenerateAddOption();
				sItem.sItemCustom.nidDISGUISE.wMainID = pQuestProg->m_sNID.wMainID;
				sItem.sItemCustom.nidDISGUISE.wSubID = pQuestProg->m_sNID.wSubID;
				ItemInsert( pCharData2->m_CharDbNum, INVEN_QUEST_PROC, &vecItems[loop], INSERT_QUEST_ITEM_NEW_CHA );
			}
		}
	}

	{
		// 완료 퀘스트
		GLQuestPlay::MAPQUEST& quest = pCharData2->m_cQuestPlay.GetQuestEnd();
		GLQuestPlay::MAPQUEST_CITER iter = quest.begin();
		GLQuestPlay::MAPQUEST_CITER iter_end = quest.end();
		for ( ; iter != iter_end; ++iter )
		{
			std::vector< SINVENITEM_SAVE > vecItems;
			GLQUESTPROG* pQuestProg = (*iter).second;
			if ( !pQuestProg )
			{
				continue;
			}
			pQuestProg->m_sINVENTORY.GET_ITEM_BY_VECTOR( vecItems );
			size_t ItemSize = vecItems.size();
			for ( size_t loop = 0; loop < ItemSize; loop++ )
			{
				// Quest Item 일 경우에 Costume 의 MID, SID 가 Quest 의 MID,SID 이다.
				SINVENITEM_SAVE& sItem = vecItems[loop];
				sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
				sItem.sItemCustom.GenerateBasicStat( false );
				sItem.sItemCustom.GenerateLinkSkill();
				sItem.sItemCustom.GenerateAddOption();
				sItem.sItemCustom.nidDISGUISE.wMainID = pQuestProg->m_sNID.wMainID;
				sItem.sItemCustom.nidDISGUISE.wSubID = pQuestProg->m_sNID.wSubID;
				ItemInsert( pCharData2->m_CharDbNum, INVEN_QUEST_END, &vecItems[loop], INSERT_QUEST_END_ITEM_NEW_CHA );
			}
		}
	}

	return ChaNum;
}

int CADOManager::CharacterInfoBackup( int nChaNum )
{
    CjADO AdoGame(m_csGameDBConnString);

    APPEND_IPARAM_INT(AdoGame, "@dwChaNum", nChaNum);
    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    int Result = ExecuteStoredProcedureIntReturn("dbo.InsertChaInfoBackup", AdoGame, "@nReturn");
    if ( Result == DB_ERROR )
    {
        sc::writeLogError(std::string("CADOManager::CharacterInfoBackup() - InsertChaInfoBackup failed. - ChaNum : %d", nChaNum));

        return DB_CHA_DUF;
    }

    return Result;
}


int CADOManager::GetCharacterInfo( int nChaNum, int nUserNumber, GLCHARLOGIC* pChaData2 )
{
	if (!pChaData2)
	{
		return DB_ERROR;
	}

	/*CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"SELECT UserNum, ChaName, ChaTribe, ChaClass, SGNum, "							\
			"ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, "					\
			"ChaDex, ChaIntel, ChaStRemain, ChaMoney, ChaExp, "							\
			"ChaHP, ChaMP, ChaStartMap, ChaStartGate, ChaPosX, "						\
			"ChaPosY, ChaPosZ, GuNum, ChaBright, ChaAttackP, "							\
			"ChaDefenseP, ChaFightA, ChaShootA, ChaSP, ChaPK, "							\
			"ChaSkillPoint, ChaSaveMap, ChaSavePosX, ChaSavePosY, ChaSavePosZ, "		\
			"ChaSchool, ChaSex, ChaHair, ChaFace, ChaLiving, "							\
			"ChaInvenLine, ChaReturnMap, ChaReturnPosX, ChaReturnPosY, ChaReturnPosZ, "	\
			"ChaCP, ChaHairColor, "														\
			"ChaSkills, ChaSkillSlot, ChaPutOnItems, ChaQuest, ChaInven, "				\
			"ChaCoolTime, ChaSkillFact "												\
		"FROM ChaInfo WHERE ChaNum= %d "
		), nChaNum );


	CjADO ado(m_csGameDBConnString);
	
    APPEND_IPARAM_INT(ado, "@ChaNum",  nChaNum);
    APPEND_IPARAM_INT(ado, "@UserNum", nUserNumber);

	if ( !EXECUTE_GETIMAGE(ado, csQuery) )
		return DB_ERROR;*/


    CjADO ado(m_csGameDBConnString);

    if (nUserNumber <= 0 || nChaNum <= 0)
        return DB_ERROR;

    if (GetCharacterInfoBase(nUserNumber, nChaNum, pChaData2) ==DB_ERROR)
        return DB_ERROR;

	// Stats
	if ( DB_ERROR == GetCharacterInfoStats( nChaNum, pChaData2 ) )
		return DB_ERROR;

    // Skill
    if (GetCharacterInfoSkill(nChaNum, pChaData2) == DB_ERROR)
        return DB_ERROR;

    // Skill Quick Slot
    if (GetCharacterInfoSkillSlot(nChaNum, pChaData2) == DB_ERROR)
        return DB_ERROR;

    // Action Quick Slot
    if (GetCharacterInfoActionSlot(nChaNum, pChaData2) == DB_ERROR)
        return DB_ERROR;	

    // Quest
    if (GetCharacterInfoQuest(nChaNum, pChaData2) == DB_ERROR)
        return DB_ERROR;

    // 착용아이템
    if (GetCharacterInfoPutOnItems(nChaNum, pChaData2) == DB_ERROR)
        return DB_ERROR;	

    // 캐릭터 인벤토리
    if (GetCharacterInfoInven(nChaNum, pChaData2) == DB_ERROR)
        return DB_ERROR;

    if (GetCharacterInfoCoolTime(nChaNum, pChaData2) == DB_ERROR)
        return DB_ERROR;

    // 걸려있는 SKillFact
    if (GetCharacterInfoSkillFact(nChaNum, pChaData2) == DB_ERROR)
        return DB_ERROR;

	// 캐릭터 슬롯 데이터;
	if ( DB_ERROR == GetCharacterInfoSlots( nChaNum, pChaData2 ) )
		return DB_ERROR;

   /* APPEND_IPARAM_INT(ado, "@ChaNum",  nChaNum);
    APPEND_IPARAM_INT(ado, "@UserNum", nUserNumber);

    if ( !ado.Execute4Cmd(CString("dbo.ChaInfoSelectBase"), adCmdStoredProc) )
        return DB_ERROR;*/

	//CString	csChaName;

	//UINT	nUserNum	= 0;
	//UINT	nChaTribe	= 0;
	//UINT	nChaClass	= 0;
	//UINT	nSGNum		= 0;
	//UINT	nChaLevel	= 0;
	//UINT	nChaPower	= 0;
	//UINT	nChaStrong	= 0;
	//UINT	nChaStrength= 0;
	//UINT	nChaSpirit	= 0;
	//UINT	nChaDex		= 0;
	//UINT	nChaIntel	= 0;
	//UINT	nChaStRemain= 0;
	//UINT	nChaHP		= 0;
	//UINT	nChaMP		= 0;
	//UINT	nChaCP		= 0;
	//UINT	nChaStartMap= 0;
	//UINT	nChaStartGate=0;
	//UINT	nGuNum		= 0;
	//UINT	nChaBright	= 0;
	//UINT	nChaAttackP	= 0;
	//UINT	nChaDefenseP= 0;
	//UINT	nChaFightA	= 0;
	//UINT	nChaShootA	= 0;
	//UINT	nChaSP		= 0;
	//UINT	nChaPK		= 0;
	//UINT	nChaSkillPoint=0;
	//UINT	nChaSaveMap	= 0;
	//UINT	nChaSchool	= 0;
	//UINT	nChaFace	= 0;
	//UINT	nChaHair	= 0;
	//UINT	nChaSex		= 0;
	//UINT	nChaLiving	= 0;	
	//UINT	nChaReturnMap=0;
	//UINT	nChaInvenLine=0;
	//UINT	nChaHairColor=0;

	//LONGLONG llChaExp	= 0;
	//LONGLONG llChaMoney	= 0;	

	//float	fChaPosX	= 0.0;
	//float	fChaPosY	= 0.0;
	//float	fChaPosZ	= 0.0;

	//float	fChaSavePosX= 0.0;
	//float	fChaSavePosY= 0.0;
	//float	fChaSavePosZ= 0.0;

	//float	fChaReturnPosX = 0.0;
	//float	fChaReturnPosY = 0.0;
	//float	fChaReturnPosZ = 0.0;


	//se::ByteStream bytestream;
	//

	//BEGIN_GETCOLLECT(ado)

	//	ado.GetCollect( "UserNum"		,nUserNum );
	//	ado.GetCollect( "ChaName"		,csChaName );
	//	ado.GetCollect( "ChaTribe"		,nChaTribe );
	//	ado.GetCollect( "ChaClass"		,nChaClass );
	//	ado.GetCollect( "SGNum"			,nSGNum );

	//	ado.GetCollect( "ChaLevel"		,nChaLevel );
	//	ado.GetCollect( "ChaPower"		,nChaPower );
	//	ado.GetCollect( "ChaStrong"		,nChaStrong );
	//	ado.GetCollect( "ChaStrength"	,nChaStrength );
	//	ado.GetCollect( "ChaSpirit"		,nChaSpirit );
	//	
	//	ado.GetCollect( "ChaDex"		,nChaDex );
	//	ado.GetCollect( "ChaIntel"		,nChaIntel );
	//	ado.GetCollect( "ChaStRemain"	,nChaStRemain );
	//	ado.GetCollect( "ChaMoney"		,llChaMoney );		
	//	ado.GetCollect( "ChaExp"		,llChaExp );

	//	ado.GetCollect( "ChaHP"			,nChaHP );
	//	ado.GetCollect( "ChaMP"			,nChaMP );
	//	ado.GetCollect( "ChaStartMap"	,nChaStartMap );
	//	ado.GetCollect( "ChaStartGate"	,nChaStartGate );
	//	ado.GetCollect( "ChaPosX"		,fChaPosX );

	//	ado.GetCollect( "ChaPosY"		,fChaPosY );
	//	ado.GetCollect( "ChaPosZ"		,fChaPosZ );
	//	ado.GetCollect( "GuNum"			,nGuNum );
	//	ado.GetCollect( "ChaBright"		,nChaBright );
	//	ado.GetCollect( "ChaAttackP"	,nChaAttackP );

	//	ado.GetCollect( "ChaDefenseP"	,nChaDefenseP );
	//	ado.GetCollect( "ChaFightA"		,nChaFightA );
	//	ado.GetCollect( "ChaShootA"		,nChaShootA );
	//	ado.GetCollect( "ChaSP"			,nChaSP );
	//	ado.GetCollect( "ChaPK"			,nChaPK );

	//	ado.GetCollect( "ChaSkillPoint"	,nChaSkillPoint );
	//	ado.GetCollect( "ChaSaveMap"	,nChaSaveMap );
	//	ado.GetCollect( "ChaSavePosX"	,fChaSavePosX );
	//	ado.GetCollect( "ChaSavePosY"	,fChaSavePosY );
	//	ado.GetCollect( "ChaSavePosZ"	,fChaSavePosZ );

	//	ado.GetCollect( "ChaSchool"		,nChaSchool );
	//	ado.GetCollect( "ChaSex"		,nChaSex );
	//	ado.GetCollect( "ChaHair"		,nChaHair );
	//	ado.GetCollect( "ChaFace"		,nChaFace );
	//	ado.GetCollect( "ChaLiving"		,nChaLiving );

	//	ado.GetCollect( "ChaInvenLine"	,nChaInvenLine );
	//	ado.GetCollect( "ChaReturnMap"	,nChaReturnMap );
	//	ado.GetCollect( "ChaReturnPosX"	,fChaReturnPosX);
	//	ado.GetCollect( "ChaReturnPosY"	,fChaReturnPosY);		
	//	ado.GetCollect( "ChaReturnPosZ"	,fChaReturnPosZ);

	//	ado.GetCollect( "ChaCP"			,nChaCP );
	//	ado.GetCollect( "ChaHairColor"	,nChaHairColor );

	//	
	//	//
	//	//Chunks
	//	//
	//	if ( ado.GetChunk("ChaSkills"		,bytestream) )
	//		pChaData2->SETEXPSKILLS_BYBUF(bytestream);
	//	
	//	if ( ado.GetChunk("ChaPutOnItems"	,bytestream) )
	//		SETPUTONITEMS_BYBUF(pChaData2->m_PutOnItems, bytestream);

	//	if ( ado.GetChunk("ChaSkillSlot"	,bytestream) )
	//		pChaData2->SETSKILL_QUICKSLOT(bytestream);

	//	if ( ado.GetChunk("ChaQuest"		,bytestream) )
	//		pChaData2->SETQUESTPLAY(bytestream, true);

	//	if ( ado.GetChunk("ChaInven"		,bytestream) )
	//		pChaData2->SETINVENTORY_BYBUF(bytestream);

	//	if ( ado.GetChunk("ChaCoolTime"		,bytestream) )
	//		pChaData2->SETITEMCOOLTIME_BYBUF(bytestream);

	//	if ( ado.GetChunk("ChaSkillFact"	,bytestream) )
	//		pChaData2->SETSKILLFACT_BYBUF(bytestream);
	//	
	//END_GETCOLLECT(ado)

	//
	//	
	//if ( !ado.IsCollectedAll() )
	//	return DB_ERROR;


	//pChaData2->m_CharDbNum			= (DWORD) nChaNum;
	//
	//pChaData2->SetUserID( (DWORD) nUserNum );
	//pChaData2->SetName(csChaName.GetString());

	//pChaData2->m_emTribe			= EMTRIBE   (nChaTribe);
	//pChaData2->m_emClass			= EMCHARCLASS(nChaClass);
	//
	//pChaData2->m_dwServerID			= nSGNum;
	//pChaData2->m_wLevel				= (WORD) nChaLevel;

	//// wPow;	//	힘.
	//// wStr;	//	체력.
	//// wSpi;	//	정신.
	//// wDex;	//	민첩.
	//// wInt;	//	지력.
	//// wSta;	//	근력.		--(추가)
	//pChaData2->m_sStats.wPow		= (WORD) nChaPower;
	//pChaData2->m_sStats.wStr		= (WORD) nChaStrong;
	//pChaData2->m_sStats.wSta		= (WORD) nChaStrength;
	//pChaData2->m_sStats.wSpi		= (WORD) nChaSpirit;
	//pChaData2->m_sStats.wDex		= (WORD) nChaDex;
	//pChaData2->m_sStats.wInt		= (WORD) nChaIntel;

	//pChaData2->m_wStatsPoint		= (WORD) nChaStRemain;

	//pChaData2->m_lnMoney            = llChaMoney;
	//pChaData2->m_sExperience.lnNow  = llChaExp;

	//pChaData2->m_sHP.wNow			= (WORD) nChaHP;
	//pChaData2->m_sMP.wNow			= (WORD) nChaMP;
	//pChaData2->m_sStartMapID.dwID	= (DWORD) nChaStartMap;
	//pChaData2->m_dwStartGate		= (DWORD) nChaStartGate;
	//pChaData2->m_vStartPos.x		= (float) fChaPosX;

	//pChaData2->m_vStartPos.y		= (float) fChaPosY;
	//pChaData2->m_vStartPos.z		= (float) fChaPosZ,	
	////nGuNum;
	//pChaData2->m_nBright			= nChaBright;
	//pChaData2->m_wAP				= (WORD) nChaAttackP;

	//pChaData2->m_wDP				= (WORD) nChaDefenseP;
	//pChaData2->m_wPA				= (WORD) nChaFightA;
	//pChaData2->m_wSA				= (WORD) nChaShootA;
	//pChaData2->m_sSP.dwData 		= (WORD) nChaSP;
	//pChaData2->m_wPK				= (WORD) nChaPK;

	//nChaSkillPoint;
	//pChaData2->m_sSaveMapID.dwID	= (DWORD) nChaSaveMap;
	//pChaData2->m_vSavePos.x			= (float) fChaSavePosX; 
	//pChaData2->m_vSavePos.y			= (float) fChaSavePosY; 
	//pChaData2->m_vSavePos.z			= (float) fChaSavePosZ; 

	//pChaData2->m_wSchool			= (WORD) nChaSchool;
	//pChaData2->m_wSex				= (WORD) nChaSex;
	//pChaData2->m_wHair				= (WORD) nChaHair;
	//pChaData2->m_wHairColor			= (WORD) nChaHairColor;
	//pChaData2->m_wFace				= (WORD) nChaFace;
	//pChaData2->m_nLiving			= nChaLiving;

	//pChaData2->m_dwSkillPoint		= static_cast<DWORD> (nChaSkillPoint);

	//pChaData2->m_sLastCallMapID.dwID= (DWORD) nChaReturnMap;
	//pChaData2->m_vLastCallPos.x		= (float) fChaReturnPosX; 
	//pChaData2->m_vLastCallPos.y		= (float) fChaReturnPosY; 
	//pChaData2->m_vLastCallPos.z		= (float) fChaReturnPosZ;

	//// 인벤토리 라인
	//pChaData2->m_wINVENLINE			= (WORD) nChaInvenLine;

	//pChaData2->m_sCP.wNow			= (WORD) nChaCP;
	
	
	return DB_OK;	
}

int CADOManager::RestoreCharacterInfoBackup( int nChaNum )
{
    CjADO ado(m_csGameDBConnString);

    APPEND_IPARAM_INT(ado, "@dwChaNum", nChaNum);

    if (!ado.Execute4Cmd(CString("dbo.RestoreChaInfoBackup"), adCmdStoredProc) )
    {
        sc::writeLogError(std::string("CADOManager::RestoreCharacterInfoBackup() - RestoreChaInfoBackup failed. - ChaNum : %d", nChaNum));

        return DB_ERROR;
    }

	return DB_OK;
}

/* // NOT USED

int CADOManager::GetChaInfoFromChaNum( int nChaNum, std::vector<CharInfo> &v )
{
	return 0;
}


int CADOManager::GetChaInfoFromChaName( CString strChaName, std::vector<CharInfo> &v )
{
	return 0;
}


int CADOManager::GetChaInfoFromUserNum( int nUserNum, std::vector<CharInfo> &v )
{
	return 0;
}

*/



//
// ToDo: REPLACE to ReadUserInven!
//
int CADOManager::GetUserInven( GLCHARLOGIC* pChaData2 )
{
	return ReadUserInven(pChaData2);
}


int CADOManager::ReadUserInven( GLCHARLOGIC* pChaData2 )
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"SELECT UserMoney, ChaStorage2, ChaStorage3, ChaStorage4 "	\
		"FROM UserInven "														\
		"WHERE UserNum= %u "
		),  pChaData2->GetUserID());

	CjADO ado(m_csGameDBConnString);

	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;


	LONGLONG 	llUserMoney = 0;	
	_variant_t	vartStorageDate2;
	_variant_t	vartStorageDate3;
	_variant_t	vartStorageDate4;

	se::ByteStream bytestream;


	BEGIN_GETCOLLECT(ado)

		ado.GetCollect(0,  llUserMoney);
		ado.GetCollect(1,  vartStorageDate2);
		ado.GetCollect(2,  vartStorageDate3);
		ado.GetCollect(3,  vartStorageDate4);

	END_GETCOLLECT(ado)


	if ( !ado.IsCollectedAll() )
		return DB_ERROR;

	SYSTEMTIME	stStorage;
	CTime		ctStorage;	

	pChaData2->SetStorageMoney( llUserMoney );

	VariantTimeToSystemTime(vartStorageDate2.date, &stStorage);
	ctStorage = stStorage;
	pChaData2->m_tSTORAGE[0] = ctStorage.GetTime();

	VariantTimeToSystemTime(vartStorageDate2.date, &stStorage);
	ctStorage = stStorage;
	pChaData2->m_tSTORAGE[1] = ctStorage.GetTime();

	VariantTimeToSystemTime(vartStorageDate2.date, &stStorage);
	ctStorage = stStorage;
	pChaData2->m_tSTORAGE[2] = ctStorage.GetTime();

	// 아이템
	{
		std::vector< SINVENITEM_SAVE > vecItems;
		if ( DB_ERROR == ItemSelect( pChaData2->GetUserID(), INVEN_LOCKER, vecItems ) )
		{
			return DB_ERROR;
		}

		pChaData2->SETSTORAGE_BYVECTOR( vecItems );
	}
	return DB_OK;
}


int CADOManager::SaveCharacterBase( LPVOID _pbuffer )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;

	if (dwChaNum <= 0 || pCharData2->m_wLevel == 0)
		return DB_ERROR;

	DWORD dwUserNum = pCharData2->GetUserID();
	DWORD dwSvrNum = pCharData2->m_dwServerID;
	DWORD dwChaID = pCharData2->m_CharDbNum;

	GLCHARLOGIC* &p = pCharData2;

    LONGLONG ChaMoney = sc::db::AdjustMoney(pCharData2->GetInvenMoney());
    LONGLONG ChaExp = sc::db::AdjustMoney(pCharData2->m_sExperience.lnNow);

    std::string Query = stringformat(
		"UPDATE ChaInfo SET "																	    	        \
			"ChaBright= %1%, ChaLevel= %2%, ChaMoney= %3%, ChaDex=%4%, ChaIntel= %5%, "		    		        \
			"ChaStrong= %6%, ChaPower= %7%, ChaSpirit= %8%, ChaStrength= %9%, ChaStRemain=%10%, "	        	\
			"ChaAttackP= %11%, ChaDefenseP= %12%, ChaFightA= %13%, ChaShootA= %14%, ChaExp= %15%, "	        	\
			"ChaSkillPoint= %16%, ChaHP= %17%, ChaMP= %18%, ChaSP= %19%, ChaPK= %20%, "					        \
			"ChaStartMap= %21%, ChaStartGate= %22%, ChaPosX= %23%, ChaPosY= %24%, ChaPosZ= %25%, "		        \
			"ChaSaveMap= %26%, ChaSavePosX= %27%, ChaSavePosY= %28%, ChaSavePosZ= %29%, ChaFace= %30%, "    	\
			"ChaHair= %31%, ChaSex= %32%, ChaSchool= %33%, ChaLiving= %34%, ChaReturnMap= %35%, "		    	\
			"ChaReturnPosX= %36%, ChaReturnPosY= %37%, ChaReturnPosZ= %38%, ChaInvenLine= %39%, ChaCP= %40%, SkillSlotNum=%41% "	\
		"WHERE ChaNum = %42% "
		, p->m_nBright, (int)p->m_wLevel, ChaMoney, (int)p->m_sStats.wDex, (int)p->m_sStats.wInt
		, (int)p->m_sStats.wStr, (int)p->m_sStats.wPow, (int)p->m_sStats.wSpi, (int)p->m_sStats.wSta, (int)p->m_wStatsPoint
		, p->m_powerAttack, p->m_powerDefence, (int)p->m_powerAttribute[SKILL::EMAPPLY_MELEE], (int)p->m_powerAttribute[SKILL::EMAPPLY_RANGE], ChaExp
		, (int)p->m_dwSkillPoint, p->m_sHP.nNow, p->m_sMP.nNow, p->m_sSP.nNow, (int)p->m_wPK
		, (int)p->m_sStartMapID.dwID, (int)p->m_dwStartGate, p->m_vStartPos.x, p->m_vStartPos.y, p->m_vStartPos.z
		, (int)p->m_sSaveMapID.dwID, p->m_vSavePos.x, p->m_vSavePos.y, p->m_vSavePos.z, (int)p->m_wFace
		, (int)p->m_wHair, (int)p->m_wSex, (int)p->m_wSchool, p->m_nLiving, (int)p->m_sLastCallMapID.dwID
		, p->m_vLastCallPos.x, p->m_vLastCallPos.y, p->m_vLastCallPos.z, (int)p->m_wINVENLINE, (int)p->m_sCP.dwData, static_cast< int >( p->m_wSlotIndex )
		, (int)dwChaNum
		);

	CjADO ado(m_csGameDBConnString);

	if ( !EXECUTE_RAW(ado, CString(Query.c_str())) )
		return DB_ERROR;

	CString csLog;
	csLog.Format(_T("TRACK:UPDATE ChaInfo SET ... , ChaMoney= %I64d, ... WHERE ChaNum = %d"), p->GetInvenMoney(), (int)dwChaNum);

	WriteGMLog(csLog);

	if ( DB_ERROR == SaveCharacterSlots( (LPVOID)p ) )
		return DB_ERROR;

	return DB_OK;
}


int CADOManager::SaveCharacterInven( LPVOID _pbuffer, bool bClone /*= false*/ )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast< GLCHARLOGIC* >( _pbuffer );

	if ( pCharData2 == NULL )
		return DB_ERROR;

	DWORD ChaDbNum = pCharData2->m_CharDbNum;

	std::vector< SINVENITEM_SAVE > vecItems;
	pCharData2->m_cInventory.GET_ITEM_BY_VECTOR( vecItems );

	size_t ItemSize = vecItems.size();
	for ( size_t loop = 0; loop < ItemSize; loop++ )
	{
		SINVENITEM_SAVE& sItem = vecItems[loop];

		if ( bClone )
		{
			// guid 신규 발급, 디비에 신규 아이템으로 삽입
			sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
			sItem.sItemCustom.GenerateBasicStat( false );
			sItem.sItemCustom.GenerateLinkSkill();
			sItem.sItemCustom.GenerateAddOption();
			sItem.sItemCustom.SetDbState( db::DB_INSERT, true );
			ItemInsert( ChaDbNum, INVEN_INVEN, &vecItems[loop], INSERT_INVEN_ITEM );
			continue;
		}

		int DbState = sItem.sItemCustom.GetDbState();
		if ( db::DB_INSERT == DbState )
		{
			ItemInsert( ChaDbNum, INVEN_INVEN, &vecItems[loop], INSERT_INVEN_ITEM );
		}
		else if ( db::DB_UPDATE == DbState )
		{
			ItemUpdate( ChaDbNum, INVEN_INVEN, &vecItems[loop], UPDATE_INVEN_ITEM );
		}
		else if ( db::DB_DELETE == DbState )
		{
			ItemDelete( vecItems[loop].sItemCustom.GetGuid(), db::DB_DELETE, DELETE_INVEN_ITEM );
		}
		else
		{
			return DB_ERROR;
		}
	}

	return DB_OK;
}

int CADOManager::SaveCharacterSkill( LPVOID _pbuffer )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;

	if (dwChaNum < 0)
		return DB_ERROR;


	se::ByteStream	bytestream;
	_variant_t		vartBLOB;
	ULONG			nSizeOfData = 0;

	pCharData2->GETEXPSKILLS_BYBUF(bytestream);

	
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"UPDATE ChaInfo SET ChaSkills = ? WHERE ChaNum = %d"
		), dwChaNum);


	CjADO ado(m_csGameDBConnString);

	ado.MakeChunk(vartBLOB, bytestream, nSizeOfData);

	APPEND_IPARAM_IMAGE(ado, "ChaSkills", vartBLOB, nSizeOfData);


	if ( !EXECUTE_SETIMAGE(ado, csQuery) )
		return DB_ERROR;

	return DB_OK;
}


int CADOManager::SaveCharacterQuest( LPVOID _pbuffer )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;

	if (dwChaNum < 0)
		return DB_ERROR;


	se::ByteStream	bytestream( 40960 );
	_variant_t		vartBLOB;
	ULONG			nSizeOfData = 0;

	pCharData2->GETQUESTPLAY(bytestream);


	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"UPDATE ChaInfo SET ChaQuest = ? WHERE ChaNum = %d"
		), dwChaNum);


	CjADO ado(m_csGameDBConnString);

	ado.MakeChunk(vartBLOB, bytestream, nSizeOfData);

	APPEND_IPARAM_IMAGE(ado, "ChaQuest", vartBLOB, nSizeOfData);


	if ( !EXECUTE_SETIMAGE(ado, csQuery) )
		return DB_ERROR;

	return DB_OK;
}


int CADOManager::SaveCharacterSkillQuickSlot( LPVOID _pbuffer )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;

	if (dwChaNum < 0)
		return DB_ERROR;


	se::ByteStream	bytestream;
	_variant_t		vartBLOB;
	ULONG			nSizeOfData = 0;

	pCharData2->GETSKILL_QUICKSLOT(bytestream);


	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"UPDATE ChaInfo SET ChaSkillSlot = ? WHERE ChaNum = %d"
		), dwChaNum);


	CjADO ado(m_csGameDBConnString);

	ado.MakeChunk(vartBLOB, bytestream, nSizeOfData);

	APPEND_IPARAM_IMAGE(ado, "ChaSkillSlot", vartBLOB, nSizeOfData);


	if ( !EXECUTE_SETIMAGE(ado, csQuery) )
		return DB_ERROR;

	return DB_OK;
}


int CADOManager::SaveCharacterActionQuickSlot( LPVOID _pbuffer )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;

	if (dwChaNum < 0)
		return DB_ERROR;


	se::ByteStream	bytestream;
	_variant_t		vartBLOB;
	ULONG			nSizeOfData = 0;

	pCharData2->GETACTION_QUICKSLOT(bytestream);


	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"UPDATE ChaInfo SET ChaActionSlot = ? WHERE ChaNum = %d"
		), dwChaNum);


	CjADO ado(m_csGameDBConnString);

	ado.MakeChunk(vartBLOB, bytestream, nSizeOfData);

	APPEND_IPARAM_IMAGE(ado, "ChaActionSlot", vartBLOB, nSizeOfData);


	if ( !EXECUTE_SETIMAGE(ado, csQuery) )
		return DB_ERROR;

	return DB_OK;
}

int CADOManager::SaveCharacterUserInven( LPVOID _pbuffer )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;

	DWORD			dwUserID = pCharData2->GetUserID();


	SaveStorageDate(dwUserID, 1, pCharData2->m_tSTORAGE[0]);
	SaveStorageDate(dwUserID, 2, pCharData2->m_tSTORAGE[1]);
	SaveStorageDate(dwUserID, 3, pCharData2->m_tSTORAGE[2]);

	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T("UPDATE UserInven SET UserMoney = %I64d WHERE UserNum= %d ")
						, pCharData2->GetStorageMoney(), dwUserID);

	WriteGMLog(CString("TRACK:")+csQuery);

	CjADO ado(m_csGameDBConnString);

	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;

	std::vector< SINVENITEM_SAVE > vecItems;
	pCharData2->GETSTORAGE_BYVECTOR( vecItems );

	size_t ItemSize = vecItems.size();
	for ( size_t loop = 0; loop < ItemSize; loop++ )
	{
		SINVENITEM_SAVE& sItem = vecItems[loop];
		int DbState = sItem.sItemCustom.GetDbState();
		if ( db::DB_INSERT == DbState )
		{
			ItemInsert( dwUserID, INVEN_LOCKER, &vecItems[loop], INSERT_USER_LOCKER_ITEM );
		}
		else if ( db::DB_UPDATE == DbState )
		{
			ItemUpdate( dwUserID, INVEN_LOCKER, &vecItems[loop], UPDATE_USER_LOCKER_ITEM );
		}
		else if ( db::DB_DELETE == DbState )
		{
			ItemDelete( vecItems[loop].sItemCustom.GetGuid(), db::DB_DELETE, DELETE_USER_LOCKER_ITEM );
		}
		else
		{
			return DB_ERROR;
		}
	}


	return DB_OK;
}


int CADOManager::SaveStorageDate( int nUserNum, int nStorageNum, __time64_t tTime )
{
	if (nStorageNum < 1 || nStorageNum > 3)
	{
		return DB_ERROR;
	}

	CTime ctDateTime(tTime);
	CString csTime = ctDateTime.Format("%Y-%m-%d %H:%M:%S");

	
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
					"UPDATE UserInven SET ChaStorage%d = %s "	\
					"WHERE UserNum = %d "
					), (nStorageNum + 1), csTime.GetBuffer(), nUserNum );


	CjADO ado(m_csGameDBConnString);

	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;

	return DB_OK;
}


int CADOManager::SaveCharacterSkillFact( LPVOID _pbuffer )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;

	if (dwChaNum < 0)
		return DB_ERROR;


	se::ByteStream	bytestream;
	_variant_t		vartBLOB;
	ULONG			nSizeOfData = 0;

	pCharData2->GETSKILLFACT_BYBUF(bytestream);


	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"UPDATE ChaInfo SET ChaSkillFact = ? WHERE ChaNum = %d"
		), dwChaNum);


	CjADO ado(m_csGameDBConnString);

	ado.MakeChunk(vartBLOB, bytestream, nSizeOfData);

	APPEND_IPARAM_IMAGE(ado, "ChaSkillFact", vartBLOB, nSizeOfData);


	if ( !EXECUTE_SETIMAGE(ado, csQuery) )
		return DB_ERROR;

	return DB_OK;
}


int CADOManager::SaveCharacterPutOnItem( LPVOID _pbuffer, bool bClone /*= false*/ )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast< GLCHARLOGIC* >( _pbuffer );

	if ( pCharData2 == NULL )
		return DB_ERROR;

	DWORD ChaDbNum = pCharData2->m_CharDbNum;

	std::vector< SINVENITEM_SAVE > vecItems;
	pCharData2->GETPUTONITEMS_BYVECTOR( vecItems );

	size_t ItemSize = vecItems.size();
	for ( size_t loop = 0; loop < ItemSize; loop++ )
	{
		SINVENITEM_SAVE& sItem = vecItems[loop];

		if ( bClone )
		{
			// guid 신규 발급, 디비에 신규 아이템으로 삽입
			sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
			sItem.sItemCustom.GenerateBasicStat( false );
			sItem.sItemCustom.GenerateLinkSkill();
			sItem.sItemCustom.GenerateAddOption();
			sItem.sItemCustom.SetDbState( db::DB_INSERT, true );
			ItemInsert( ChaDbNum, INVEN_PUTON, &vecItems[loop], INSERT_PUTON_ITEM );
			continue;
		}

		int DbState = sItem.sItemCustom.GetDbState();
		if ( db::DB_INSERT == DbState )
		{
			ItemInsert( ChaDbNum, INVEN_PUTON, &vecItems[loop], INSERT_PUTON_ITEM );
		}
		else if ( db::DB_UPDATE == DbState )
		{
			ItemUpdate( ChaDbNum, INVEN_PUTON, &vecItems[loop], UPDATE_PUTON_ITEM );
		}
		else if ( db::DB_DELETE == DbState )
		{
			ItemDelete( vecItems[loop].sItemCustom.GetGuid(), db::DB_DELETE, DELETE_PUTON_ITEM );
		}
		else
		{
			return DB_ERROR;
		}
	}
	return DB_OK;
}

int CADOManager::SaveCharacterSlots ( LPVOID _pbuffer )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast< GLCHARLOGIC* >( _pbuffer );
	if ( NULL == pCharData2 )
		return DB_ERROR;

	DWORD dwChaDBNum = pCharData2->CharDbNum();
	if ( dwChaDBNum < 0 )
		return DB_ERROR;

	for ( int i=0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
	{
		CjADO AdoGame( m_csGameDBConnString );

		const SCHAR_SLOT_DATA& sSlotData =
			GLCharSlotMan::Instance()->GetSlotData( dwChaDBNum, i );

		//if ( true == sSlotData.m_bLockSlot )
		//	continue;

		//	@ChaNum
		//	@SkillSlotNum
		//	@ChaPower
		//	@ChaStrong
		//	@ChaStrength
		//	@ChaSpirit
		//	@ChaDex
		//	@ChaIntel
		//	@ChaStRemain
		//	@ChaSkillPoint
		//	@ChaSkills
		//	@ChaSkillSlot
		//	@bLockSlot

		APPEND_IPARAM_INT( AdoGame, "@ChaNum", static_cast< int >( dwChaDBNum ) );
		APPEND_IPARAM_INT( AdoGame, "@SkillSlotNum", i );

		APPEND_IPARAM_INT( AdoGame, "@ChaPower", static_cast< int >( sSlotData.m_sStats.wPow ) );
		APPEND_IPARAM_INT( AdoGame, "@ChaStrong", static_cast< int >( sSlotData.m_sStats.wStr ) );
		APPEND_IPARAM_INT( AdoGame, "@ChaStrength", static_cast< int >( sSlotData.m_sStats.wSta ) );
		APPEND_IPARAM_INT( AdoGame, "@ChaSpirit", static_cast< int >( sSlotData.m_sStats.wSpi ) );
		APPEND_IPARAM_INT( AdoGame, "@ChaDex", static_cast< int >( sSlotData.m_sStats.wDex ) );
		APPEND_IPARAM_INT( AdoGame, "@ChaIntel", static_cast< int >( sSlotData.m_sStats.wInt ) );

		APPEND_IPARAM_INT( AdoGame, "@ChaStRemain", static_cast< int >( sSlotData.m_wStatsPoint ) );
		APPEND_IPARAM_INT( AdoGame, "@ChaSkillPoint", static_cast< int >( sSlotData.m_dwSkillPoint ) );

		{
			se::ByteStream bytestream;
			_variant_t vartBLOB;
			ULONG nSizeOfData = 0;

			sSlotData.GETEXPSKILLS_BYBUF( bytestream );

			AdoGame.MakeChunk( vartBLOB, bytestream, nSizeOfData );

			APPEND_IPARAM_IMAGE( AdoGame, "@ChaSkills", vartBLOB, nSizeOfData );
		}

		{
			se::ByteStream bytestream;
			_variant_t vartBLOB;
			ULONG nSizeOfData = 0;

			sSlotData.GETSKILL_QUICKSLOT( bytestream );

			AdoGame.MakeChunk( vartBLOB, bytestream, nSizeOfData );

			APPEND_IPARAM_IMAGE( AdoGame, "@ChaSkillSlot", vartBLOB, nSizeOfData );
		}

		//APPEND_IPARAM_TINYINT( AdoGame, "@bLockSlot", static_cast< BYTE >( sSlotData.m_bLockSlot ) );
		APPEND_IPARAM_TINYINT( AdoGame, "@bLockSlot", static_cast< BYTE >( pCharData2->m_arrLockSlotState[ i ] ) );

		if (FALSE == AdoGame.Execute4Cmd(CString("dbo.ChaInfoUpdateChaSlots"), adCmdStoredProc))
		{
			sc::writeLogError( "[ Character Slot ] [ Failed dbo.ChaInfoUpdateChaSlots ]" );
			return DB_ERROR;
		}
	}

	return DB_OK;
}

int CADOManager::SaveCharacter( LPVOID _pbuffer )
{
	GLCHARLOGIC* pCharData2 = reinterpret_cast<GLCHARLOGIC*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_CharDbNum;

	if (dwChaNum == 0 || pCharData2->m_wLevel == 0)
		return DB_ERROR;

	DWORD dwUserNum = pCharData2->GetUserID();
	DWORD dwSvrNum = pCharData2->m_dwServerID;
	DWORD dwChaID = pCharData2->m_CharDbNum;

	GLCHARLOGIC* &p = pCharData2;


	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"UPDATE ChaInfo SET "																		\
			"ChaBright= %d,	ChaLevel= %d, ChaMoney= %I64d, ChaDex=%d, ChaIntel= %d, "				\
			"ChaStrong= %d, ChaPower= %d, ChaSpirit= %d, ChaStrength= %d, ChaStRemain=%d, "			\
			"ChaAttackP= %d, ChaDefenseP= %d, ChaFightA= %d, ChaShootA= %d, ChaExp= %I64d, "		\
			"ChaSkillPoint= %d, ChaHP= %d, ChaMP= %d, ChaSP= %d, ChaPK= %d, "						\
			"ChaStartMap= %d, ChaStartGate= %d, ChaPosX= %f, ChaPosY= %f, ChaPosZ= %f, "			\
			"ChaSaveMap= %d, ChaSavePosX= %f, ChaSavePosY= %f, ChaSavePosZ= %f, ChaFace= %d, "		\
			"ChaHair= %d, ChaSchool= %d, ChaLiving= %d, ChaCP=%d, SkillSlotNum=%d "									\
		"WHERE ChaNum = %d "
		), p->m_nBright, (int)p->m_wLevel, p->GetInvenMoney(), (int)p->m_sStats.wDex, (int)p->m_sStats.wInt
		, (int)p->m_sStats.wStr, (int)p->m_sStats.wPow, (int)p->m_sStats.wSpi, (int)p->m_sStats.wSta, (int)p->m_wStatsPoint
		, p->m_powerAttack, p->m_powerDefence, (int)p->m_powerAttribute[SKILL::EMAPPLY_MELEE], (int)p->m_powerAttribute[SKILL::EMAPPLY_RANGE], p->m_sExperience.lnNow
		, (int)p->m_dwSkillPoint, p->m_sHP.nNow, p->m_sMP.nNow, p->m_sSP.nNow, (int)p->m_wPK
		, (int)p->m_sStartMapID.dwID, (int)p->m_dwStartGate, p->m_vStartPos.x, p->m_vStartPos.y, p->m_vStartPos.z
		, (int)p->m_sSaveMapID.dwID, p->m_vSavePos.x, p->m_vSavePos.y, p->m_vSavePos.z
		, (int)p->m_wHair, (int)p->m_wSchool, p->m_nLiving, (int)p->m_sCP.dwData, static_cast< int >( p->m_wSlotIndex )
		, (int)dwChaNum
		);


	//
	// If would be possible to query only once with following IMAGEs and
	//  surely, it would be more efficient.
	// But in this function, separate queries will be used with each pre-creaed functions
	//	that means i will re-use queries i alread made.
	//

	//if ( !SaveCharacterSkill((LPVOID)p) )
	//	return DB_ERROR;

	if ( !SaveCharacterQuest((LPVOID)p) )
		return DB_ERROR;

	//if ( !SaveCharacterSkillQuickSlot((LPVOID)p) )
	//	return DB_ERROR;

	if ( !SaveCharacterPutOnItem((LPVOID)p) )
		return DB_ERROR;

	if ( !SaveCharacterSkillFact((LPVOID)p) )
		return DB_ERROR;

	if ( !SaveCharacterInven((LPVOID)p) )
		return DB_ERROR;
	
	if (pCharData2->m_bServerStorage)
	{
		if ( !SaveCharacterUserInven((LPVOID)p) )
			return DB_ERROR;
	}

	if ( !SaveCharacterSlots( (LPVOID)p ) )
		return DB_ERROR;


	return DB_OK;
}

int CADOManager::CopyCharacterProductInfo( DWORD dwOriginalChaNum, DWORD dwNewChaNum )
{
    CString csQueryProductBook;
    CString csQueryProductIsGetExp;
    CString csQueryProductProductType;

    csQueryProductBook.Empty();
    csQueryProductIsGetExp.Empty();
    csQueryProductProductType.Empty();

    csQueryProductBook.Format(_T(
        "SET NOCOUNT ON "                    \
        "INSERT INTO dbo.ProductBook "       \
        "SELECT %d, [BookID] "               \
        "FROM dbo.ProductBook "              \
        "WHERE dbo.ProductBook.ChaNum = %d " \
        ), dwNewChaNum, dwOriginalChaNum  );

    csQueryProductIsGetExp.Format(_T(
        "SET NOCOUNT ON "                    \
        "INSERT INTO dbo.ProductIsGetExp "       \
        "SELECT %d, [ProductID] "               \
        "FROM dbo.ProductIsGetExp "              \
        "WHERE dbo.ProductIsGetExp.ChaNum = %d " \
        ), dwNewChaNum, dwOriginalChaNum  );

    csQueryProductProductType.Format(_T(
        "SET NOCOUNT ON "                    \
        "INSERT INTO dbo.ProductType "       \
        "SELECT %d, [ProductType] "               \
        "FROM dbo.ProductType "              \
        "WHERE dbo.ProductType.ChaNum = %d " \
        ), dwNewChaNum, dwOriginalChaNum  );


    CjADO ado(m_csGameDBConnString);

    if ( !EXECUTE_RAW(ado, csQueryProductBook) )
        return DB_ERROR;

    if ( !EXECUTE_RAW(ado, csQueryProductIsGetExp) )
        return DB_ERROR;

    if ( !EXECUTE_RAW(ado, csQueryProductProductType) )
        return DB_ERROR;

    return DB_OK;
}

int CADOManager::ItemSearch( int mid, int sid, std::vector< ITEM_SEARCH >& result )
{
	CString csQuery;
	csQuery.Empty();

	csQuery.Format( _T( 
		"SELECT ItemUUID, ChaNum, InvenType, ItemRemain, ItemPetNum, ItemVehicleNum "	\
		"FROM dbo.Item "	\
		"WHERE ItemMID = %d AND ItemSID = %d "	\
		"ORDER BY ChaNum ASC "
		), mid, sid );

	CjADO ado( m_csGameDBConnString );

	if ( !ado.Execute( csQuery ) )
		return DB_ERROR;	

	BEGIN_GETCOLLECT( ado )
		ITEM_SEARCH Item;
		
		CString strGuid;
		ado.GetCollect( 0, strGuid );
		char szGuid[ sc::string::UUID_LENGTH_BRACES ] = { 0 };
		StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, strGuid.GetString() );
		sc::string::stringToUUID( szGuid, Item.guid );
		
		ado.GetCollect( 1, Item.ChaNum );
		ado.GetCollect( 2, Item.InvenType );
		ado.GetCollect( 3, Item.TurnNum );
		ado.GetCollect( 4, Item.PetNum );
		ado.GetCollect( 5, Item.VehicleNum );
		if ( !ado.IsCollectedAll() )		
			return DB_ERROR;
		result.push_back( Item );
	END_GETCOLLECT( ado )

	return DB_OK;
}

int CADOManager::ItemSearch( CString& guid, std::vector< ITEM_SEARCH >& result )
{
	CString csQuery;
	csQuery.Empty();

	csQuery.Format( _T( 
		"SELECT ItemUUID, ChaNum, InvenType, ItemRemain, ItemPetNum, ItemVehicleNum "	\
		"FROM dbo.Item "	\
		"WHERE ItemUUID = '%s' "
		), guid.GetString() );

	CjADO ado( m_csGameDBConnString );

	if ( !ado.Execute( csQuery ) )
		return DB_ERROR;	

	BEGIN_GETCOLLECT( ado )
		ITEM_SEARCH Item;

		CString strGuid;
		ado.GetCollect( 0, strGuid );
		char szGuid[ sc::string::UUID_LENGTH_BRACES ] = { 0 };
		StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, strGuid.GetString() );
		sc::string::stringToUUID( szGuid, Item.guid );

		ado.GetCollect( 1, Item.ChaNum );
		ado.GetCollect( 2, Item.InvenType );
		ado.GetCollect( 3, Item.TurnNum );
		ado.GetCollect( 4, Item.PetNum );
		ado.GetCollect( 5, Item.VehicleNum );
		if ( !ado.IsCollectedAll() )		
			return DB_ERROR;
		result.push_back( Item );
	END_GETCOLLECT( ado )

	return DB_OK;
}

int CADOManager::ItemStatistic( std::vector< ITEM_STATISTIC >& result )
{
	CString csQuery;
	csQuery.Empty();

	csQuery.Format( _T( 
		"SELECT ItemMID, ItemSID, COUNT(*) AS Count "	\
		"FROM dbo.Item WITH (NOLOCK) "	\
		"WHERE InvenType IN( 1, 2, 3, 4, 5, 6, 7, 8, 9 ) "	\
		"GROUP BY ItemMID, ItemSID "	\
		"ORDER BY ItemMID, ItemSID "
		) );

	CjADO ado( m_csGameDBConnString );

	if ( !ado.Execute( csQuery ) )
		return DB_ERROR;	

	BEGIN_GETCOLLECT( ado )
		ITEM_STATISTIC Item;

		ado.GetCollect( 0, Item.ItemMid );
		ado.GetCollect( 1, Item.ItemSid );
		ado.GetCollect( 2, Item.Count );
		if ( !ado.IsCollectedAll() )		
			return DB_ERROR;
		result.push_back( Item );
	END_GETCOLLECT( ado )

	return DB_OK;
}

int CADOManager::ItemSelect( int Owner, int InvenType, std::vector< SINVENITEM_SAVE >& vecItems )
{
	CjADO Ado( m_csGameDBConnString );
	APPEND_IPARAM_INT( Ado, "@ChaNum", Owner );
	APPEND_IPARAM_TINYINT( Ado, "@InvenType", InvenType );
	if ( !Ado.Execute4Cmd( CString( "dbo.sp_ItemGetItemList" ), adCmdStoredProc ) )
		return DB_ERROR;

	if ( Ado.GetEOF() )
	{
		return DB_OK;
	}

	do
	{
		CString strGuid;
		SNATIVEID sItemID( false );
		_variant_t CreateDate, CostumeExpireDate, CostumeStatEndDate, TempGrindFireDate, BasicPeriodEx, DisguisePeriodEx;
		SINVENITEM_SAVE sItem;

		DWORD dwBasicAttackDamageData = 0;
		WORD wBasicDefence = 0;

		SNATIVEID sLinkSkillID( false );
		WORD wLinkSkillLevel = 0;
		WORD wLinkSkillTarget = 0;
		float fLinkSkillOccurRate = 0.f;

		Ado.GetCollect( "ItemUUID", strGuid );
		char szGuid[ sc::string::UUID_LENGTH_BRACES ] = { 0 };
		StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, strGuid.GetString() );
		sc::string::stringToUUID( szGuid, sItem.sItemCustom.guid );

		Ado.GetCollect( "ItemMID", sItemID.wMainID );
		Ado.GetCollect( "ItemSID", sItemID.wSubID );

		sItem.sItemCustom.SetNativeID( sItemID );

		Ado.GetCollect( "ItemMakeType", sItem.sItemCustom.cGenType );
		Ado.GetCollect( "ItemCostumeMID", sItem.sItemCustom.nidDISGUISE.wMainID );
		Ado.GetCollect( "ItemCostumeSID", sItem.sItemCustom.nidDISGUISE.wSubID );

		Ado.GetCollect( "ItemPosX", sItem.wPosX );
		Ado.GetCollect( "ItemPosY", sItem.wPosY );

		Ado.GetCollect( "ItemRemain", sItem.sItemCustom.wTurnNum );

		Ado.GetCollect( "ItemCreateDate", CreateDate );
		Ado.GetCollect( "ItemCostumeExpireDate", CostumeExpireDate );

		Ado.GetCollect( "ItemBasicPeriodExTime", BasicPeriodEx );
		Ado.GetCollect( "ItemDisguisePeriodExTime", DisguisePeriodEx );
		Ado.GetCollect( "ItemBasicPeriodExCount", sItem.sItemCustom.cPeriodExtensionCount );
		Ado.GetCollect( "ItemDisuisePeriodExCount", sItem.sItemCustom.cCostumePeriodExtensionCount );

		if ( CreateDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.tBORNTIME = sc::time::GetTime( CreateDate );

		if ( CostumeExpireDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.tDISGUISE = sc::time::GetTime( CostumeExpireDate );

		if ( BasicPeriodEx.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.SetPeriodExBasic_FromDB(sc::time::GetTime( BasicPeriodEx ));

		if ( DisguisePeriodEx.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.SetPeriodExBasic_FromDB(sc::time::GetTime( DisguisePeriodEx ));

		Ado.GetCollect( "ItemDurability", sItem.sItemCustom.wDurability );
		Ado.GetCollect( "ItemDurabilityCount", sItem.sItemCustom.dwDurabilityCount );
		Ado.GetCollect( "PointTradeCount", sItem.sItemCustom.wRanPointTradeCount );
		Ado.GetCollect( "ItemAttack", sItem.sItemCustom.cDAMAGE );
		Ado.GetCollect( "ItemDefense", sItem.sItemCustom.cDEFENSE );
		Ado.GetCollect( "ItemRandomType1", sItem.sItemCustom.randomOption.option[0].cOptType );
		Ado.GetCollect( "ItemRandomValue1", sItem.sItemCustom.randomOption.option[0].nOptValue );
		Ado.GetCollect( "ItemRandomType2", sItem.sItemCustom.randomOption.option[1].cOptType );
		Ado.GetCollect( "ItemRandomValue2", sItem.sItemCustom.randomOption.option[1].nOptValue );
		Ado.GetCollect( "ItemRandomType3", sItem.sItemCustom.randomOption.option[2].cOptType );
		Ado.GetCollect( "ItemRandomValue3", sItem.sItemCustom.randomOption.option[2].nOptValue );
		Ado.GetCollect( "ItemRandomType4", sItem.sItemCustom.randomOption.option[3].cOptType );
		Ado.GetCollect( "ItemRandomValue4", sItem.sItemCustom.randomOption.option[3].nOptValue );
		Ado.GetCollect( "ItemRandomType5", sItem.sItemCustom.randomOption.option[4].cOptType );
		Ado.GetCollect( "ItemRandomValue5", sItem.sItemCustom.randomOption.option[4].nOptValue );
		Ado.GetCollect( "ItemResistFire", sItem.sItemCustom.cRESIST_FIRE );
		Ado.GetCollect( "ItemResistIce", sItem.sItemCustom.cRESIST_ICE );
		Ado.GetCollect( "ItemResistElec", sItem.sItemCustom.cRESIST_ELEC );
		Ado.GetCollect( "ItemResistPoison", sItem.sItemCustom.cRESIST_POISON );
		Ado.GetCollect( "ItemResistSpirit", sItem.sItemCustom.cRESIST_SPIRIT );
		Ado.GetCollect( "ItemPetNum", sItem.sItemCustom.m_PetDbNum );
		Ado.GetCollect( "ItemVehicleNum", sItem.sItemCustom.m_VehicleDbNum );
		Ado.GetCollect( "ItemMainColor", sItem.sItemCustom.dwMainColor );
		Ado.GetCollect( "ItemSubColor", sItem.sItemCustom.dwSubColor );

		Ado.GetCollect( "CostumeStatType1", sItem.sItemCustom.costumeUser.Invest[0].cStatType );
		Ado.GetCollect( "CostumeInvestPoint1", sItem.sItemCustom.costumeUser.Invest[0].wInvestPt );
		Ado.GetCollect( "CostumeStatType2", sItem.sItemCustom.costumeUser.Invest[1].cStatType );
		Ado.GetCollect( "CostumeInvestPoint2", sItem.sItemCustom.costumeUser.Invest[1].wInvestPt );
		Ado.GetCollect( "CostumeStatType3", sItem.sItemCustom.costumeUser.Invest[2].cStatType );
		Ado.GetCollect( "CostumeInvestPoint3", sItem.sItemCustom.costumeUser.Invest[2].wInvestPt );
		Ado.GetCollect( "CostumeStatType4", sItem.sItemCustom.costumeUser.Invest[3].cStatType );
		Ado.GetCollect( "CostumeInvestPoint4", sItem.sItemCustom.costumeUser.Invest[3].wInvestPt );
		Ado.GetCollect( "CostumeStatType5", sItem.sItemCustom.costumeUser.Invest[4].cStatType );
		Ado.GetCollect( "CostumeInvestPoint5", sItem.sItemCustom.costumeUser.Invest[4].wInvestPt );
		Ado.GetCollect( "CostumeStatType6", sItem.sItemCustom.costumeUser.Invest[5].cStatType );
		Ado.GetCollect( "CostumeInvestPoint6", sItem.sItemCustom.costumeUser.Invest[5].wInvestPt );
		Ado.GetCollect( "CostumeStatType7", sItem.sItemCustom.costumeUser.Invest[6].cStatType );
		Ado.GetCollect( "CostumeInvestPoint7", sItem.sItemCustom.costumeUser.Invest[6].wInvestPt );
		Ado.GetCollect( "CostumeStatEndDate", CostumeStatEndDate );
		if ( CostumeStatEndDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.costumeUser.tEndTime = sc::time::GetTime( CostumeStatEndDate );

		Ado.GetCollect( "TempGrindValue", sItem.sItemCustom.sTLGrind.cGradeValue );
		Ado.GetCollect( "TempGrindFireDate", TempGrindFireDate );
		if ( TempGrindFireDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.sTLGrind.tFireTime = sc::time::GetTime( TempGrindFireDate );

		BYTE LockBox = 0;
		Ado.GetCollect( "LockBox", LockBox );
		sItem.sItemCustom.bVietnamGainItem = LockBox != 0 ? true : false;

		Ado.GetCollect( "BasicAttackDamage", dwBasicAttackDamageData );
		Ado.GetCollect( "BasicDefence", wBasicDefence );

		if ( dwBasicAttackDamageData == MAXDWORD && wBasicDefence == MAXWORD )
		{
			// 두 값이 모두 세팅이 안되어 있는 상태라면 세팅해준다;
			// 이는 두 값이 0인 것과는 다른 의미이다;
			// 두 값이 모두 최고인 아이템은 존재하지도 않으며 존재해서는 안된다;
			sItem.sItemCustom.GenerateBasicStat();
		}
		else
		{
			sItem.sItemCustom.SetBasicAttackDamage( dwBasicAttackDamageData );
			sItem.sItemCustom.SetBasicDefence( wBasicDefence );
		}

		Ado.GetCollect( "ItemLinkSkillMID", sLinkSkillID.wMainID );
		Ado.GetCollect( "ItemLinkSkillSID", sLinkSkillID.wSubID );
		Ado.GetCollect( "ItemLinkSkillLevel", wLinkSkillLevel );
		Ado.GetCollect( "ItemLinkSkillTarget", wLinkSkillTarget );
		Ado.GetCollect( "ItemLinkSkillOccurRate", fLinkSkillOccurRate );

		if ( sLinkSkillID.dwID == MAXDWORD &&
			wLinkSkillLevel == MAXWORD &&
			wLinkSkillTarget == MAXWORD &&
			fLinkSkillOccurRate == -1.f )
		{
			sItem.sItemCustom.GenerateLinkSkill();
		}
		else
		{
			sItem.sItemCustom.SetLinkSkillID( sLinkSkillID );
			sItem.sItemCustom.SetLinkSkillLevel( wLinkSkillLevel );
			sItem.sItemCustom.SetLinkSkillTarget( static_cast< EMITEM_SKILL_LINK >( wLinkSkillTarget ) );
			sItem.sItemCustom.SetLinkSkillOccurRate( fLinkSkillOccurRate );
		}

		WORD wType(EMADD_NOTINIT);
		Ado.GetCollect( "ItemAddonType1", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[0].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType2", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[1].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType3", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[2].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType4", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[3].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType5", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[4].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType6", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[5].emTYPE = (EMITEM_ADDON)wType;
		if ( sItem.sItemCustom.sAddonInfo.IsInit() == false )
		{
			sItem.sItemCustom.GenerateAddOption();
		}
		else
		{
			Ado.GetCollect( "ItemAddonValue1", sItem.sItemCustom.sAddonInfo.m_sADDON[0].nVALUE );
			Ado.GetCollect( "ItemAddonValue2", sItem.sItemCustom.sAddonInfo.m_sADDON[1].nVALUE );
			Ado.GetCollect( "ItemAddonValue3", sItem.sItemCustom.sAddonInfo.m_sADDON[2].nVALUE );
			Ado.GetCollect( "ItemAddonValue4", sItem.sItemCustom.sAddonInfo.m_sADDON[3].nVALUE );
			Ado.GetCollect( "ItemAddonValue5", sItem.sItemCustom.sAddonInfo.m_sADDON[4].nVALUE );
			Ado.GetCollect( "ItemAddonValue6", sItem.sItemCustom.sAddonInfo.m_sADDON[5].nVALUE );
		}

		sItem.sItemCustom.SetDbState( db::DB_UPDATE );

		vecItems.push_back( sItem );
	} while ( Ado.Next() );

	return DB_OK;
}

BOOL CADOManager::CheckInven( DWORD dwUserNum )
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"SELECT COUNT(*) FROM UserInven WHERE UserNum= "
		), dwUserNum );


	CjADO ado(m_csGameDBConnString);

	if ( !EXECUTE_RAW(ado, csQuery) )
		return FALSE;


	UINT nUserInven = 0;	


	BEGIN_GETCOLLECT(ado)

		ado.GetCollect(0,  nUserInven);
		
	END_GETCOLLECT(ado)


	if ( !ado.IsCollectedAll() )
		return FALSE;

	if (nUserInven > 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


int CADOManager::MakeUserInven( DWORD dwUserNum )
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"INSERT INTO UserInven (UserNum, UserMoney, UserInven) VALUES (%d, %d, %s) "
		),dwUserNum,	0, "''");

	CjADO ado(m_csGameDBConnString);

	if ( !EXECUTE_RAW(ado, csQuery) )
		return FALSE;
	
	return DB_OK;
}


int CADOManager::WriteUserInven( LONGLONG llMoney, DWORD dwUserNum, GLCHARLOGIC *pCharData2 )
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"UPDATE UserInven SET UserMoney= %.I64d, UserInven = ? WHERE UserNum= %d "
		), pCharData2->GetStorageMoney(), dwUserNum);


	CjADO ado(m_csGameDBConnString);

	se::ByteStream	bytestream;
	_variant_t		vartBLOB;
	ULONG			nSizeOfData = 0;

	pCharData2->GETSTORAGE_BYBUF(bytestream);

	ado.MakeChunk(vartBLOB, bytestream, nSizeOfData);

	APPEND_IPARAM_IMAGE(ado, "UserInven", vartBLOB, nSizeOfData);


	if ( !EXECUTE_SETIMAGE(ado, csQuery) )
		return DB_ERROR;

	return DB_OK;
}


int CADOManager::WriteUserInven( LONGLONG llMoney, DWORD dwUserNum, char* pData, int nSizeOfData )
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"UPDATE UserInven SET UserMoney= %.I64d, UserInven = ? WHERE UserNum= %d "
		), llMoney, dwUserNum);


	CjADO ado(m_csGameDBConnString);

	_variant_t		vartBLOB;
	ULONG			uSizeOfData = (ULONG)nSizeOfData;

	ado.MakeChunk(vartBLOB, (LPBYTE)pData, uSizeOfData);

	APPEND_IPARAM_IMAGE(ado, "UserInven", vartBLOB, uSizeOfData);


	if ( !EXECUTE_SETIMAGE(ado, csQuery) )
		return DB_ERROR;

	return DB_OK;
}

int CADOManager::WriteGMLog(CString &csCmd)
{
	return WriteGMLog(csCmd.GetBuffer(), m_strUserIP.GetBuffer(), m_nGmUserNum);
}

int CADOManager::WriteGMLog(std::string &strCmd)
{
	return WriteGMLog(strCmd, m_strUserIP.GetBuffer(), m_nGmUserNum);
}

int CADOManager::WriteGMLog(std::string strCmd, std::string strIP, int nGmNum)
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T("INSERT INTO LogGmCmd (UserNum, GmCmd, UserIP) VALUES (%d, '%s', '%s')"), m_nGmUserNum, strCmd.c_str(), strIP.c_str());

	CjADO ado(m_csUserDBConnString);
	
	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;

	return DB_OK;
}

int CADOManager::WriteGMLog(std::strstream& strCmd, CString strUserIP)
{
	return WriteGMLog(m_nGmUserNum, strCmd, strUserIP);
}

int CADOManager::WriteGMLog(int nGmNum, std::strstream& strCmd, CString strUserIP)
{
	CString csQuery;
	int		nReturn = 0;

	csQuery.Empty();
	csQuery.Format(_T(
					"sp_gmEditLog"
					));


	CjADO ado(m_csUserDBConnString);

	CString csCmd(strCmd.str());

	APPEND_RPARAM_INT(ado);
	APPEND_IPARAM_INT(ado, "@nUserNum", nGmNum);
	APPEND_IPARAM_VARCHAR(ado, "@szGmCmd", csCmd, csCmd.GetLength());
	APPEND_IPARAM_VARCHAR(ado, "@userIP", strUserIP, strUserIP.GetLength());	
	APPEND_OPARAM_INT(ado, "@nReturn"); 

	if ( !EXECUTE_SP(ado, csQuery) )
		return DB_ERROR;

	_variant_t varOutput;
	if ( !ado.GetParam("@nReturn", varOutput) )
		return DB_ERROR;

	nReturn = varOutput.intVal;

	return nReturn;
}

int CADOManager::WriteLogRandomItem(const LOG_RANDOM_OPTION &_sLOG)
{
	CjADO Ado(m_csLogDBConnString);
	//APPEND_RPARAM_INT(Ado);


	CString csQuery;

	csQuery.Empty();
	csQuery.Format( _T(" \
		SELECT TOP 1 \
			[SGNum] \
		FROM [dbo].[ItemLogRandom] ") );

	if ( !EXECUTE_RAW(Ado, csQuery) )
		return FALSE;

	int nSGNum;

	BEGIN_GETCOLLECT(Ado)
		Ado.GetCollect(0,  nSGNum);
	END_GETCOLLECT(Ado)
		if ( !Ado.IsCollectedAll() )
			return FALSE;


	APPEND_IPARAM_INT  (Ado, "@NIDMain", _sLOG.m_Mid);
	APPEND_IPARAM_INT  (Ado, "@NIDSub",  _sLOG.m_Sid);

	APPEND_IPARAM_INT  (Ado, "@SGNum",  nSGNum);
	APPEND_IPARAM_INT  (Ado, "@SvrNum", _sLOG.m_ServerNumber);
	APPEND_IPARAM_INT  (Ado, "@FldNum", _sLOG.m_FieldNumber);

	APPEND_IPARAM_INT  (Ado, "@MakeType", _sLOG.m_MakeType);

	std::string strGuid = sc::string::uuidToString( ( UUID )_sLOG.m_guid );
	sc::string::upperString( strGuid );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );

	APPEND_IPARAM_INT  (Ado, "@RandomType1",  static_cast<int> (_sLOG.m_OptionType1));
	APPEND_IPARAM_INT  (Ado, "@RandomValue1", static_cast<int> (_sLOG.m_OptionValue1));
	APPEND_IPARAM_INT  (Ado, "@RandomStamp1", static_cast<int> (_sLOG.m_OptionStamp1));
	APPEND_IPARAM_INT  (Ado, "@RandomType2",  static_cast<int> (_sLOG.m_OptionType2));
	APPEND_IPARAM_INT  (Ado, "@RandomValue2", static_cast<int> (_sLOG.m_OptionValue2));
	APPEND_IPARAM_INT  (Ado, "@RandomStamp2", static_cast<int> (_sLOG.m_OptionStamp2));
	APPEND_IPARAM_INT  (Ado, "@RandomType3",  static_cast<int> (_sLOG.m_OptionType3));
	APPEND_IPARAM_INT  (Ado, "@RandomValue3", static_cast<int> (_sLOG.m_OptionValue3));
	APPEND_IPARAM_INT  (Ado, "@RandomStamp3", static_cast<int> (_sLOG.m_OptionStamp3));
	APPEND_IPARAM_INT  (Ado, "@RandomType4",  static_cast<int> (_sLOG.m_OptionType4));
	APPEND_IPARAM_INT  (Ado, "@RandomValue4", static_cast<int> (_sLOG.m_OptionValue4));
	APPEND_IPARAM_INT  (Ado, "@RandomStamp4", static_cast<int> (_sLOG.m_OptionStamp4));
	APPEND_IPARAM_INT  (Ado, "@RandomType5",  static_cast<int> (_sLOG.m_OptionType5));
	APPEND_IPARAM_INT  (Ado, "@RandomValue5", static_cast<int> (_sLOG.m_OptionValue5));
	APPEND_IPARAM_INT  (Ado, "@RandomStamp5", static_cast<int> (_sLOG.m_OptionStamp5));

	std::string DateStr = sc::time::DateTimeFormat(_sLOG.m_LogDate, true, true);

	APPEND_IPARAM_VARCHAR(Ado, "@RandomDate", DateStr.c_str(), DateStr.length());

	APPEND_IPARAM_INT( Ado, "@InsertOption", _sLOG.m_InsertOption );

	return ExecuteStoredProcedure("dbo.ItemLogRandomInsert", Ado);
}

int CADOManager::WriteLogItemExchange(const LOG_ITEM_EXCHANGE& sEXCHANGE)
{
	CjADO Ado(m_csLogDBConnString);
	//APPEND_RPARAM_INT(Ado);

	CString csQuery;

	csQuery.Empty();
	csQuery.Format( _T(" \
					   SELECT TOP 1 \
					   [SGNum] \
					   FROM [dbo].[ItemLog] ") );

	if ( !EXECUTE_RAW(Ado, csQuery) )
		return FALSE;

	int nSGNum;

	BEGIN_GETCOLLECT(Ado)
		Ado.GetCollect(0,  nSGNum);
	END_GETCOLLECT(Ado)
		if ( !Ado.IsCollectedAll() )
			return FALSE;



	APPEND_IPARAM_INT  (Ado, "@NIDMain", sEXCHANGE.m_nNID_M);
	APPEND_IPARAM_INT  (Ado, "@NIDSub", sEXCHANGE.m_nNID_S);
	APPEND_IPARAM_INT  (Ado, "@SGNum", nSGNum);
	APPEND_IPARAM_INT  (Ado, "@SvrNum", sEXCHANGE.m_nSvrNum);
	APPEND_IPARAM_INT  (Ado, "@FldNum", sc::db::AdjustTinyInt(sEXCHANGE.m_nFldNum));

	APPEND_IPARAM_INT  (Ado, "@MakeType", EMGEN_GMEDIT);

	std::string strGuid = sc::string::uuidToString( ( UUID )sEXCHANGE.m_guid );
	sc::string::upperString( strGuid );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );

	APPEND_IPARAM_INT  (Ado, "@ItemAmount",  sEXCHANGE.m_nNum);
	APPEND_IPARAM_INT  (Ado, "@ItemFromFlag", sEXCHANGE.m_nFromType);
	APPEND_IPARAM_INT  (Ado, "@ItemFrom", sEXCHANGE.m_nItemFrom);

	APPEND_IPARAM_INT  (Ado, "@ItemToFlag", sEXCHANGE.m_nToType);
	APPEND_IPARAM_INT  (Ado, "@ItemTo", sEXCHANGE.m_nItemTo);
	APPEND_IPARAM_INT  (Ado, "@ExchangeFlag", sEXCHANGE.m_nExchangeFlag);
	APPEND_IPARAM_INT  (Ado, "@Damage",  static_cast<byte> (sEXCHANGE.m_nDamage));
	APPEND_IPARAM_INT  (Ado, "@Defense", static_cast<byte> (sEXCHANGE.m_nDefense));

	APPEND_IPARAM_INT  (Ado, "@Fire",     static_cast<byte> (sEXCHANGE.m_nFire));
	APPEND_IPARAM_INT  (Ado, "@Ice",      static_cast<byte> (sEXCHANGE.m_nIce));
	APPEND_IPARAM_INT  (Ado, "@Poison",   static_cast<byte> (sEXCHANGE.m_nPoison));
	APPEND_IPARAM_INT  (Ado, "@Electric", static_cast<byte> (sEXCHANGE.m_nElectric));
	APPEND_IPARAM_INT  (Ado, "@Spirit",   static_cast<byte> (sEXCHANGE.m_nSpirit));

	APPEND_IPARAM_INT  (Ado, "@CostumeMID", sEXCHANGE.m_nCOSTUME_MID);
	APPEND_IPARAM_INT  (Ado, "@CostumeSID", sEXCHANGE.m_nCOSTUME_SID);

	APPEND_IPARAM_MONEY(Ado, "@TradePrice", 0);

	std::string DateStr = sc::time::DateTimeFormat(sEXCHANGE.m_LogDate, true, true);
	APPEND_IPARAM_VARCHAR(Ado, "@ExchangeDate", DateStr.c_str(), DateStr.length());

	APPEND_IPARAM_INT     (Ado, "@nBaseMainMapID", sEXCHANGE.m_nBaseMainMapID);
	APPEND_IPARAM_INT     (Ado, "@nBaseSubMapID", sEXCHANGE.m_nBaseSubMapID);
	APPEND_IPARAM_INT     (Ado, "@nGaeaMainMapID", sEXCHANGE.m_nGaeaMainMapID);
	APPEND_IPARAM_INT     (Ado, "@nGaeaSubMapID", sEXCHANGE.m_nGaeaSubMapID);

	APPEND_IPARAM_INT( Ado, "@nOriNIDMain", sEXCHANGE.m_nOriNIDMain );
	APPEND_IPARAM_INT( Ado, "@nOriNIDSub", sEXCHANGE.m_nOriNIDSub );

	APPEND_IPARAM_INT( Ado, "@nExchangeFlagDetail", sEXCHANGE.m_nExchangeFlagDetail );

	return ExecuteStoredProcedure("dbo.ItemLogInsert", Ado);
}

int CADOManager::GetClubInfo( const char* szSearch, std::vector<viewClubInfo> &v )
{
	// Not Used
	return DB_OK;
}


int CADOManager::GetActiveChaNum()
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"SELECT COUNT(*) FROM ChaInfo WHERE ChaDeleted = 0 "
		));


	CjADO ado(m_csGameDBConnString);

	int nCharacters = 0;

	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;

	BEGIN_GETCOLLECT(ado)

		ado.GetCollect(0, nCharacters);

	END_GETCOLLECT(ado)

	return nCharacters;
}


int CADOManager::GetDeletedChaNum()
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"SELECT COUNT(*) FROM ChaInfo WHERE ChaDeleted = 1 "
		));


	CjADO ado(m_csGameDBConnString);

	int nCharacters = 0;

	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;

	BEGIN_GETCOLLECT(ado)

		ado.GetCollect(0, nCharacters);

	END_GETCOLLECT(ado)

	return nCharacters;
}


int CADOManager::GetUserInvenNum()
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"SELECT COUNT(*) FROM UserInven"
		));


	CjADO ado(m_csGameDBConnString);

	int nUserInven = 0;

	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;

	BEGIN_GETCOLLECT(ado)

		ado.GetCollect(0, nUserInven);

	END_GETCOLLECT(ado)

	return nUserInven;
}


int CADOManager::GetClubNum()
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"SELECT COUNT(*) FROM dbo.GuildInfo"
		));


	CjADO ado(m_csGameDBConnString);

	int nGuild = 0;

	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;

	BEGIN_GETCOLLECT(ado)

		ado.GetCollect(0, nGuild);

	END_GETCOLLECT(ado)

	return nGuild;
}


LONGLONG CADOManager::GetSumChaMoney()
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"SELECT SUM( CONVERT ( BIGINT, ChaMoney ) ) FROM ChaInfo WHERE ChaDeleted = 0"
		));


	CjADO ado(m_csGameDBConnString);

	LONGLONG llTotalMoney = 0;

	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;

	BEGIN_GETCOLLECT(ado)

		ado.GetCollect(0, llTotalMoney);

	END_GETCOLLECT(ado)

	return llTotalMoney;
}


LONGLONG CADOManager::GetSumUserInvenMoney()
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"SELECT SUM( CONVERT ( BIGINT, UserMoney ) ) FROM UserInven"
		));


	CjADO ado(m_csGameDBConnString);

	LONGLONG llTotalMoney = 0;

	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;

	BEGIN_GETCOLLECT(ado)

		ado.GetCollect(0, llTotalMoney);

	END_GETCOLLECT(ado)

	return llTotalMoney;
}


LONGLONG CADOManager::GetSumClubInvenMoney()
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"SELECT SUM( CONVERT ( BIGINT, GuMoney ) ) FROM dbo.GuildInfo"
		));


	CjADO ado(m_csGameDBConnString);

	LONGLONG llTotalMoney = 0;

	if ( !EXECUTE_RAW(ado, csQuery) )
		return DB_ERROR;

	BEGIN_GETCOLLECT(ado)

		ado.GetCollect(0, llTotalMoney);

	END_GETCOLLECT(ado)

		return llTotalMoney;
}


void CADOManager::SetLocalIP()
{
	sc::net::initializeNetwork();

	CString ip = "";
	char szHostName[256];
	PHOSTENT pHostInfo; 

	int nRet = gethostname(szHostName,sizeof(szHostName));

	if( nRet == 0 )
	{
		if((pHostInfo = gethostbyname(szHostName)) != NULL)
		{
			ip = inet_ntoa(*(struct in_addr *)*pHostInfo->h_addr_list);
		}
	}
	else
	{
		int nCode = WSAGetLastError();
		int x = nCode;
	}

	m_strUserIP = ip;
}


CString CADOManager::GetLocalIP()
{
	return m_strUserIP;
}


/*

int CADOManager::ReadImage( const char* objName, int nChaNum, se::ByteStream &ByteStream )
{
	return 0;
}


int CADOManager::WriteImage( const char* objName, int nChaNum, char* pData, int nSize )
{
	return 0;
}


int CADOManager::GetInt( std::string& strSearch )
{
return 0;
}


LONGLONG CADOManager::GetBigInt( std::string& strSearch )
{
return 0;
}

*/


//
// !!!!!! std::vector<GLVEHICLE> &v 로 변경할 것 !!!!!!
//
int CADOManager::GetVehicleInfo(gmce::VVEHICLE &v, int nChaNum, int nVehicleNum)
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format(_T(
		"SELECT VehicleNum, VehicleType, VehicleName, VehicleCardMID, VehicleCardSID, "	\
			   "VehicleBattery, VehicleDeleted, VehicleBooster, VehicleCreateDate, "	\
			   "VehicleDeletedDate, VehiclePartsColor "				\
		"FROM VehicleInfo "																\
		"WHERE VehicleChaNum = %d "														\
		"ORDER BY VehicleNum "
		), nChaNum);


	CjADO ado(m_csGameDBConnString);

	if ( !EXECUTE_GETIMAGE(ado, csQuery) )
		return DB_ERROR;

	
	UINT	nNum;	
	UINT	nType;
	UINT	nMID;
	UINT	nSID;
	UINT	nBattery;
	UINT	nBooster;
	UINT	nDeleted;

	CString	csName;
	
	//
	//Dates
	//
	_variant_t	vartCreateDate;
	_variant_t	vartDeleteDate;

	//
	//Images
	//
	_variant_t	vartPutOnItems;
	_variant_t	vartPartsColor;

	//se::ByteStream bytestream;
	se::ByteStream bytestream2;
	
	SYSTEMTIME	stCreateDate;
	CTime		ctCreateDate;

	SYSTEMTIME	stDeleteDate;
	CTime		ctDeleteDate;

	gmce::VEHICLEINFO aINFO;	//an item of vehicle info.
	
	
	//
	//the result can be multi-rows.
	//	
	BEGIN_GETCOLLECT(ado)
		
		ado.GetCollect(0, nNum);
		ado.GetCollect(1, nType);
		ado.GetCollect(2, csName);
		ado.GetCollect(3, nMID);
		ado.GetCollect(4, nSID);

		ado.GetCollect(5, nBattery);
		ado.GetCollect(6, nDeleted);
		ado.GetCollect(7, nBooster);
		ado.GetCollect(8, vartCreateDate);
		ado.GetCollect(9, vartDeleteDate);

		if ( !ado.IsCollectedAll() )
			return DB_ERROR;		
		
		//
		//following Images may be NULL.
		//
		//ado.GetChunk("VehiclePutOnItems", bytestream);
		GetVehiclePutonItems( nNum, aINFO.glvehicle );

		ado.GetChunk("VehiclePartsColor", bytestream2);


		aINFO.glvehicle.OwnerDbNumSet(nChaNum);
		
		aINFO.nNum								= nNum;
		aINFO.glvehicle.m_emTYPE				= VEHICLE_TYPE(nType);
		aINFO.csName							= csName;
		aINFO.glvehicle.m_sVehicleID.wMainID	= nMID;
		aINFO.glvehicle.m_sVehicleID.wSubID		= nSID;

		aINFO.glvehicle.m_nFull					= nBattery;
		aINFO.bDeleted							= ( nDeleted == 0 ) ? FALSE : TRUE;
		aINFO.glvehicle.m_dwBoosterID			= nBooster;

		VariantTimeToSystemTime(vartCreateDate.date, &stCreateDate);
		ctCreateDate							= stCreateDate;
		aINFO.ctCreateDate						= ctCreateDate;

		VariantTimeToSystemTime(vartDeleteDate.date, &stDeleteDate);
		ctDeleteDate							= stDeleteDate;
		aINFO.ctDeleteDate						= ctDeleteDate;
		

		//SETPUTONITEMS_BYBUF( aINFO.glvehicle.m_PutOnItems,	bytestream  );
		SETITEMSCOLOR_BYBUF( aINFO.glvehicle.m_sColor,		bytestream2 );

		v.push_back(aINFO);

	END_GETCOLLECT(ado)


	return DB_OK;
}

int CADOManager::GetVehiclePutonItems( int VehicleNum, GLVEHICLE& Vehicle )
{
	CjADO Ado( m_csGameDBConnString );
	APPEND_IPARAM_INT( Ado, "@VehicleNum", VehicleNum );
	if ( !Ado.Execute4Cmd( CString( "dbo.sp_ItemGetItemVehiclePuton" ), adCmdStoredProc ) )
		return DB_ERROR;

	if ( Ado.GetEOF() )
	{
		return DB_OK;
	}

	do
	{
		CString strGuid;
		SNATIVEID sItemID( false );
		_variant_t CreateDate, CostumeExpireDate, BasicPeriodEx, DisguisePeriodEx;
		SINVENITEM_SAVE sItem;

		Ado.GetCollect( "ItemUUID", strGuid );
		char szGuid[ sc::string::UUID_LENGTH_BRACES ] = { 0 };
		StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, strGuid.GetString() );
		sc::string::stringToUUID( szGuid, sItem.sItemCustom.guid );

		Ado.GetCollect( "ItemMID", sItemID.wMainID );
		Ado.GetCollect( "ItemSID", sItemID.wSubID );

		sItem.sItemCustom.SetNativeID( sItemID );

		Ado.GetCollect( "ItemMakeType", sItem.sItemCustom.cGenType );
		Ado.GetCollect( "ItemCostumeMID", sItem.sItemCustom.nidDISGUISE.wMainID );
		Ado.GetCollect( "ItemCostumeSID", sItem.sItemCustom.nidDISGUISE.wSubID );

		Ado.GetCollect( "ItemPosX", sItem.wPosX );
		Ado.GetCollect( "ItemPosY", sItem.wPosY );

		Ado.GetCollect( "ItemRemain", sItem.sItemCustom.wTurnNum );

		Ado.GetCollect( "ItemCreateDate", CreateDate );
		Ado.GetCollect( "ItemCostumeExpireDate", CostumeExpireDate );

		Ado.GetCollect( "ItemBasicPeriodExTime", BasicPeriodEx );
		Ado.GetCollect( "ItemDisguisePeriodExTime", DisguisePeriodEx );
		Ado.GetCollect( "ItemBasicPeriodExCount", sItem.sItemCustom.cPeriodExtensionCount );
		Ado.GetCollect( "ItemDisuisePeriodExCount", sItem.sItemCustom.cCostumePeriodExtensionCount );

		if ( CreateDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.tBORNTIME = sc::time::GetTime( CreateDate );

		if ( CostumeExpireDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.tDISGUISE = sc::time::GetTime( CostumeExpireDate );

		if ( BasicPeriodEx.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.SetPeriodExBasic_FromDB(sc::time::GetTime( BasicPeriodEx ));

		if ( DisguisePeriodEx.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.SetPeriodExDisguise_FromDB(sc::time::GetTime( DisguisePeriodEx ));

		Ado.GetCollect( "ItemDurability", sItem.sItemCustom.wDurability );
		Ado.GetCollect( "ItemDurabilityCount", sItem.sItemCustom.dwDurabilityCount );
		Ado.GetCollect( "PointTradeCount", sItem.sItemCustom.wRanPointTradeCount );
		Ado.GetCollect( "ItemAttack", sItem.sItemCustom.cDAMAGE );
		Ado.GetCollect( "ItemDefense", sItem.sItemCustom.cDEFENSE );
		Ado.GetCollect( "ItemRandomType1", sItem.sItemCustom.randomOption.option[0].cOptType );
		Ado.GetCollect( "ItemRandomValue1", sItem.sItemCustom.randomOption.option[0].nOptValue );
		Ado.GetCollect( "ItemRandomType2", sItem.sItemCustom.randomOption.option[1].cOptType );
		Ado.GetCollect( "ItemRandomValue2", sItem.sItemCustom.randomOption.option[1].nOptValue );
		Ado.GetCollect( "ItemRandomType3", sItem.sItemCustom.randomOption.option[2].cOptType );
		Ado.GetCollect( "ItemRandomValue3", sItem.sItemCustom.randomOption.option[2].nOptValue );
		Ado.GetCollect( "ItemRandomType4", sItem.sItemCustom.randomOption.option[3].cOptType );
		Ado.GetCollect( "ItemRandomValue4", sItem.sItemCustom.randomOption.option[3].nOptValue );
		Ado.GetCollect( "ItemRandomType5", sItem.sItemCustom.randomOption.option[4].cOptType );
		Ado.GetCollect( "ItemRandomValue5", sItem.sItemCustom.randomOption.option[4].nOptValue );
		Ado.GetCollect( "ItemResistFire", sItem.sItemCustom.cRESIST_FIRE );
		Ado.GetCollect( "ItemResistIce", sItem.sItemCustom.cRESIST_ICE );
		Ado.GetCollect( "ItemResistElec", sItem.sItemCustom.cRESIST_ELEC );
		Ado.GetCollect( "ItemResistPoison", sItem.sItemCustom.cRESIST_POISON );
		Ado.GetCollect( "ItemResistSpirit", sItem.sItemCustom.cRESIST_SPIRIT );
		Ado.GetCollect( "ItemPetNum", sItem.sItemCustom.m_PetDbNum );
		Ado.GetCollect( "ItemVehicleNum", sItem.sItemCustom.m_VehicleDbNum );
		Ado.GetCollect( "ItemMainColor", sItem.sItemCustom.dwMainColor );
		Ado.GetCollect( "ItemSubColor", sItem.sItemCustom.dwSubColor );

		sItem.sItemCustom.SetDbState( db::DB_UPDATE );

		Vehicle.SETPUTONITEM( static_cast< VEHICLE_ACCESSORYTYPE >( sItem.wPosX ), sItem.sItemCustom );

	} while ( Ado.Next() );

	return DB_OK;
}

int CADOManager::GetProductInfo( int nChaNum )
{
	dm::CDataManagerProduct::Instance()->Clear();

	// Type Select;
	CjADO adoType( m_csGameDBConnString );

	APPEND_IPARAM_INT( adoType, "@ChaNum", nChaNum );

	if( !adoType.Execute4Cmd(
		CString( "dbo.ProductTypeSelect" ), adCmdStoredProc ) )
		return DB_ERROR;

	if( !adoType.GetEOF() && !adoType.GetBOF() )
	{
		int productType = 0;
		do 
		{
			adoType.GetCollect( "ProductType", productType );
			dm::CDataManagerProduct::Instance()->AddTypeDB( productType );
		} while ( adoType.Next() );
	}

	CjADO adoExp( m_csGameDBConnString );

	// 숙련도 Select;
	APPEND_IPARAM_INT( adoExp, "@ChaNum", nChaNum );

	if( !adoExp.Execute4Cmd(
		CString( "dbo.ProductIsGetExpSelect" ), adCmdStoredProc ) )
		return DB_ERROR;

	if( !adoExp.GetEOF() && !adoExp.GetBOF() )
	{
		int productID = 0;
		do 
		{
			adoExp.GetCollect( "ProductID", productID );
			dm::CDataManagerProduct::Instance()->AddExpDB( productID );
		} while ( adoExp.Next() );
	}

	CjADO adoBook( m_csGameDBConnString );

	// 제작서 Select;
	APPEND_IPARAM_INT( adoBook, "@ChaNum", nChaNum );

	if( !adoBook.Execute4Cmd(
		CString( "dbo.sp_ProductBookLoad" ), adCmdStoredProc ) )
		return DB_ERROR;

	if( !adoBook.GetEOF() && !adoBook.GetBOF() )
	{
		int bookID = 0;
		do 
		{
			adoBook.GetCollect( "BookID", bookID );
			dm::CDataManagerProduct::Instance()->AddBookDB( bookID );
		} while ( adoBook.Next() );
	}

	return DB_OK;
}

int CADOManager::SaveProductChangeSet()
{
	int addPoints[ Product::ETypeTotal ] = { 0 , };
	for( unsigned int i=0; i<dm::CDataManagerProduct::Instance()->GetChangeSetCount(); ++i )
	{
		dm::CDataManagerProduct::PRODUCT_CHANGE_SET changeSet = 
			dm::CDataManagerProduct::Instance()->GetChangeSet( i );
		if( changeSet.action == dm::CDataManagerProduct::NONE )
			continue;

		/// Game DB;
		switch( changeSet.action )
		{
		case dm::CDataManagerProduct::EMLEARN_TYPE:
			if( changeSet.bState )
				LearnProductType( changeSet.dwProductType );
			else
				ResetProductType( changeSet.dwProductType, addPoints[ changeSet.dwProductType ] );
			break;
		case dm::CDataManagerProduct::EMLEARN_BOOK:
			if( changeSet.bState )
				LearnProductBook( changeSet.dwProductType, changeSet.dwProductID );
			else
				ResetProductBook( changeSet.dwProductType, changeSet.dwProductID );
			break;
		case dm::CDataManagerProduct::EMPICKUP_EXP:
			if( changeSet.bState )
				LearnProductExp( changeSet.dwProductType, changeSet.dwProductID, addPoints[ changeSet.dwProductType ] );
			else
				ResetProductExp( changeSet.dwProductType, changeSet.dwProductID, addPoints[ changeSet.dwProductType ] );
			break;
		}
	}

	dm::CDataManagerProduct::Instance()->ClearChangeSet();

	return DB_OK;
}

int CADOManager::LearnProductType( DWORD dwProductType )
{
	int nChaNum = dm::CDataManagerProduct::Instance()->GetChaNum();

	// Game DB
	CjADO adoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( adoGame, "@ChaNum", nChaNum );
	APPEND_IPARAM_INT( adoGame, "@ProductType", dwProductType );
	APPEND_OPARAM_INT( adoGame, "@nReturn" );

	if( ExecuteStoredProcedureIntReturn(
		"dbo.ProductTypeInsert", adoGame, "@nReturn" )
		== DB_ERROR )
		return DB_ERROR;

	// Log DB
	CjADO adoLog( m_csLogDBConnString );

	APPEND_IPARAM_INT( adoLog, "@ChaNum", nChaNum );
	APPEND_IPARAM_VARCHAR( adoLog, "@Action", "LEARN", 10 );
	APPEND_IPARAM_INT( adoLog, "@Type", dwProductType );
	APPEND_IPARAM_INT( adoLog, "@ID", 0 );
	APPEND_IPARAM_VARCHAR( adoLog, "@Stat", "EDIT", 10 );
	APPEND_IPARAM_INT( adoLog, "@Point", 0 );
	APPEND_IPARAM_INT( adoLog, "@TotalPoint", 0 );

	if( !adoLog.Execute4Cmd( CString( "dbo.sp_CharProductLog" ),
		adCmdStoredProc) )
	{
		dm::CDataManagerProduct::Instance()->Clear();
		return DB_ERROR;
	}

	return DB_OK;
}

int CADOManager::ResetProductType( DWORD dwProductType, int& addPoint )
{
	int nChaNum = dm::CDataManagerProduct::Instance()->GetChaNum();

	// Game DB
	CjADO adoType(m_csGameDBConnString);

	APPEND_IPARAM_INT( adoType, "@ChaNum", nChaNum );
	APPEND_IPARAM_INT( adoType, "@ProductType", dwProductType );
	APPEND_OPARAM_INT( adoType, "@nReturn" );

	if( ExecuteStoredProcedureIntReturn(
		"dbo.ProductTypeDelete", adoType, "@nReturn" )
		== DB_ERROR)
		return DB_ERROR;

	// Log DB
	CjADO adoLog( m_csLogDBConnString );

	APPEND_IPARAM_INT( adoLog, "@ChaNum", nChaNum );
	APPEND_IPARAM_VARCHAR( adoLog, "@Action", "RESET", 10 );
	APPEND_IPARAM_INT( adoLog, "@Type", dwProductType );
	APPEND_IPARAM_INT( adoLog, "@ID", 0 );
	APPEND_IPARAM_VARCHAR( adoLog, "@Stat", "EDIT", 10 );
	APPEND_IPARAM_INT( adoLog, "@Point", 0 );
	APPEND_IPARAM_INT( adoLog, "@TotalPoint", 0 );

	if( !adoLog.Execute4Cmd( CString( "dbo.sp_CharProductLog" ),
		adCmdStoredProc) )
	{
		dm::CDataManagerProduct::Instance()->Clear();
		return DB_ERROR;
	}

	/// DB에서 Table끼리의 관계가 설정되어 있지 않으며;
	/// DB Table 구조를 직접 바꾸는 것은 위험도가 커서;
	/// 코드에서 직접 제거해준다;
	PtrVector_ProductRecipe vecRecipe;
	GLProductRecipeMan::GetInstance().Find(
		vecRecipe, static_cast<BYTE>(
		dwProductType ) );

	BOOST_FOREACH( const SProductRecipe* pRecipe, vecRecipe )
	{
		if( dm::CDataManagerProduct::Instance()->IsExpDB( pRecipe->dwKey ) )
			ResetProductExp( dwProductType, pRecipe->dwKey, addPoint );

		if( dm::CDataManagerProduct::Instance()->IsBookDB( pRecipe->dwKey ) )
			ResetProductBook( dwProductType, pRecipe->dwKey );
	}


	return DB_OK;
}

int CADOManager::LearnProductBook( DWORD dwProductType, DWORD dwProductBook )
{
	int nChaNum = dm::CDataManagerProduct::Instance()->GetChaNum();

	// Game DB
	CjADO adoGame(m_csGameDBConnString);

	APPEND_IPARAM_INT( adoGame, "@ChaNum", nChaNum );
	APPEND_IPARAM_INT( adoGame, "@BookID", dwProductBook );

	if( ExecuteStoredProcedure(
		"dbo.sp_ProductBookSave", adoGame )
		!= DB_OK)
		return DB_ERROR;

	// Log DB
	CjADO adoLog( m_csLogDBConnString );

	APPEND_IPARAM_INT( adoLog, "@ChaNum", nChaNum );
	APPEND_IPARAM_VARCHAR( adoLog, "@Action", "IMS", 10 );
	APPEND_IPARAM_INT( adoLog, "@Type", dwProductType );
	APPEND_IPARAM_INT( adoLog, "@ID", dwProductBook );
	APPEND_IPARAM_VARCHAR( adoLog, "@Stat", "EDIT", 10 );
	APPEND_IPARAM_INT( adoLog, "@Point", 0 );
	APPEND_IPARAM_INT( adoLog, "@TotalPoint", 0 );

	if( !adoLog.Execute4Cmd( CString( "dbo.sp_CharProductLog" ),
		adCmdStoredProc) )
	{
		dm::CDataManagerProduct::Instance()->Clear();
		return DB_ERROR;
	}

	return DB_OK;
}

int CADOManager::ResetProductBook( DWORD dwProductType, DWORD dwProductBook )
{
	int nChaNum = dm::CDataManagerProduct::Instance()->GetChaNum();

	// Game DB
	CjADO adoGame(m_csGameDBConnString);

	APPEND_IPARAM_INT( adoGame, "@ChaNum", nChaNum );
	APPEND_IPARAM_INT( adoGame, "@BookID", dwProductBook );
	APPEND_OPARAM_INT( adoGame, "@nReturn" );

	if( ExecuteStoredProcedure(
		"dbo.ProductBookDelete", adoGame )
		!= DB_OK)
		return DB_ERROR;

	// Log DB
	CjADO adoLog( m_csLogDBConnString );

	APPEND_IPARAM_INT( adoLog, "@ChaNum", nChaNum );
	APPEND_IPARAM_VARCHAR( adoLog, "@Action", "RESET MP", 10 );
	APPEND_IPARAM_INT( adoLog, "@Type", dwProductType );
	APPEND_IPARAM_INT( adoLog, "@ID", dwProductBook );
	APPEND_IPARAM_VARCHAR( adoLog, "@Stat", "EDIT", 10 );
	APPEND_IPARAM_INT( adoLog, "@Point", 0 );
	APPEND_IPARAM_INT( adoLog, "@TotalPoint", 0 );

	if( !adoLog.Execute4Cmd( CString( "dbo.sp_CharProductLog" ),
		adCmdStoredProc) )
	{
		dm::CDataManagerProduct::Instance()->Clear();
		return DB_ERROR;
	}

	return DB_OK;
}

int CADOManager::LearnProductExp( DWORD dwProductType, DWORD dwProductID, int& addPoint )
{
	int nChaNum = dm::CDataManagerProduct::Instance()->GetChaNum();

	// Game DB
	CjADO adoGame(m_csGameDBConnString);

	APPEND_IPARAM_INT( adoGame, "@ChaNum", nChaNum );
	APPEND_IPARAM_INT( adoGame, "@ProductID", dwProductID );
	APPEND_OPARAM_INT( adoGame, "@nReturn" );

	if( ExecuteStoredProcedureIntReturn(
		"dbo.ProductIsGetExpInsert", adoGame, "@nReturn")
		!= DB_OK )
		return DB_ERROR;

	// Log DB
	const SProductRecipe productRecipe =
		GLProductRecipeMan::GetInstance()
		.m_mapKeyProductRecipe[ dwProductID ];

	int point = ( productRecipe.wWsp==MAXWORD ) ? 0 : productRecipe.wWsp;
	addPoint += point;

	int totalPoint = dm::CDataManagerProduct::Instance()->
		GetTypeTotalExpFromDB( dwProductType ) + addPoint;

	CjADO adoLog( m_csLogDBConnString );

	APPEND_IPARAM_INT( adoLog, "@ChaNum", nChaNum );
	APPEND_IPARAM_VARCHAR( adoLog, "@Action", "MIX", 10 );
	APPEND_IPARAM_INT( adoLog, "@Type", dwProductType );
	APPEND_IPARAM_INT( adoLog, "@ID", dwProductID );
	APPEND_IPARAM_VARCHAR( adoLog, "@Stat", "EDIT", 10 );
	APPEND_IPARAM_INT( adoLog, "@Point", point );
	APPEND_IPARAM_INT( adoLog, "@TotalPoint", totalPoint );

	if( !adoLog.Execute4Cmd( CString( "dbo.sp_CharProductLog" ),
		adCmdStoredProc) )
	{
		dm::CDataManagerProduct::Instance()->Clear();
		return DB_ERROR;
	}

	return DB_OK;
}

int CADOManager::ResetProductExp( DWORD dwProductType, DWORD dwProductID, int& addPoint )
{
	int nChaNum = dm::CDataManagerProduct::Instance()->GetChaNum();

	// Game DB
	CjADO adoGame(m_csGameDBConnString);

	APPEND_IPARAM_INT( adoGame, "@ChaNum", nChaNum );
	APPEND_IPARAM_INT( adoGame, "@ProductID", dwProductID );
	APPEND_OPARAM_INT( adoGame, "@nReturn" );

	if( ExecuteStoredProcedureIntReturn(
		"dbo.ProductIsGetExpDelete", adoGame, "@nReturn" )
		!= DB_OK )
		return DB_ERROR;

	// Log DB
	const SProductRecipe productRecipe =
		GLProductRecipeMan::GetInstance()
		.m_mapKeyProductRecipe[ dwProductID ];

	int point = ( productRecipe.wWsp==MAXWORD ) ? 0 : productRecipe.wWsp;
	addPoint -= point;

	int totalPoint = dm::CDataManagerProduct::Instance()->
		GetTypeTotalExpFromDB( dwProductType ) + addPoint;

	CjADO adoLog( m_csLogDBConnString );

	APPEND_IPARAM_INT( adoLog, "@ChaNum", nChaNum );
	APPEND_IPARAM_VARCHAR( adoLog, "@Action", "RESET MIX", 10 );
	APPEND_IPARAM_INT( adoLog, "@Type", dwProductType );
	APPEND_IPARAM_INT( adoLog, "@ID", dwProductID );
	APPEND_IPARAM_VARCHAR( adoLog, "@Stat", "EDIT", 10 );
	APPEND_IPARAM_INT( adoLog, "@Point", point );
	APPEND_IPARAM_INT( adoLog, "@TotalPoint", totalPoint );

	if( !adoLog.Execute4Cmd( CString( "dbo.sp_CharProductLog" ),
		adCmdStoredProc) )
	{
		dm::CDataManagerProduct::Instance()->Clear();
		return DB_ERROR;
	}

	return DB_OK;
}

int CADOManager::GetClubInven( int nChaNum )
{
	// Game DB
	CjADO adoGame( m_csGameDBConnString );

	APPEND_IPARAM_INT( adoGame, "@ChaNum", nChaNum );

	if( !adoGame.Execute4Cmd(
		CString( "dbo.sp_ChaClubInvenSelect" ), adCmdStoredProc ) )
		return DB_ERROR;

	if( !adoGame.GetEOF() && !adoGame.GetBOF() )
	{
		DWORD GuNum = 0;
		adoGame.GetCollect( "GuNum", GuNum );
		dm::CDataManagerClubInven::Instance()->SetGuNum( GuNum );

		LONGLONG llMoney = 0;
		adoGame.GetCollect( "GuMoney", llMoney );
		dm::CDataManagerClubInven::Instance()->SetClubInvenMoney( llMoney );

		dm::CDataManagerClubInven::Instance()->SetValidStorage( true );

		std::vector< SINVENITEM_SAVE > vecItems;
		if ( DB_ERROR == ItemSelect( GuNum, INVEN_CLUB_LOCKER, vecItems ) )
		{
			dm::CDataManagerClubInven::Instance()->SetValidStorage( false );
			return DB_ERROR;
		}

		dm::CDataManagerClubInven::Instance()->SETSTORAGE_BYVECTOR( vecItems );
	}
	else
	{
		dm::CDataManagerClubInven::Instance()->SetValidStorage( false );
	}

	return DB_OK;
}

int CADOManager::SaveClubInven( int nChaNum, DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems )
{
	CjADO adoGame( m_csGameDBConnString );

	APPEND_IPARAM_INT( adoGame, "@ChaNum", nChaNum );
	APPEND_IPARAM_MONEY( adoGame, "@Money",
		dm::CDataManagerClubInven::Instance()->GetClubInvenMoney() );

	if( !adoGame.Execute4Cmd( CString( "dbo.sp_ChaClubInvenSave" ),
		adCmdStoredProc ) )
	{
		return DB_ERROR;
	}

	size_t ItemSize = vecItems.size();
	for ( size_t loop = 0; loop < ItemSize; loop++ )
	{
		// Club locker Item 일 경우 아이템 테이블에 ChaNum 이 ClubNum 이다. 
		SINVENITEM_SAVE& sItem = vecItems[loop];
		int DbState = sItem.sItemCustom.GetDbState();
		if ( db::DB_INSERT == DbState )
		{
			ItemInsert( ClubNum, INVEN_CLUB_LOCKER, &vecItems[loop], INSERT_CLUB_LOCKER_ITEM );
		}
		else if ( db::DB_UPDATE == DbState )
		{
			ItemUpdate( ClubNum, INVEN_CLUB_LOCKER, &vecItems[loop], UPDATE_CLUB_LOCKER_ITEM );
		}
		else if ( db::DB_DELETE == DbState )
		{
			ItemDelete( vecItems[loop].sItemCustom.GetGuid(), db::DB_DELETE, DELETE_CLUB_LOCKER_ITEM );
		}
		else
		{
			return DB_ERROR;
		}
	}

	return DB_OK;
}

int CADOManager::GetCharacterSkill(int nChaNum, GLCHARLOGIC* pChaData2)
{
    if (!pChaData2)
    {
        return DB_ERROR;
    }

    CString csQuery;

    csQuery.Empty();
    csQuery.Format(
        "SELECT UserNum, ChaName, ChaSkills FROM ChaInfo WHERE ChaNum= %d", nChaNum);

    CjADO ado(m_csGameDBConnString);

    if ( !EXECUTE_GETIMAGE(ado, csQuery) )
        return DB_ERROR;


    CString	csChaName;
    UINT	nUserNum	= 0;

    se::ByteStream bytestream;


    BEGIN_GETCOLLECT(ado)

    ado.GetCollect( "UserNum"		,nUserNum );
    ado.GetCollect( "ChaName"		,csChaName );
    
    //
    //Chunks
    //
    if ( ado.GetChunk("ChaSkills"		,bytestream) )
        pChaData2->SETEXPSKILLS_BYBUF(bytestream);
    
    END_GETCOLLECT(ado)

    if ( !ado.IsCollectedAll() )
        return DB_ERROR;


    pChaData2->m_CharDbNum			= (DWORD) nChaNum;

    pChaData2->SetUserID( (DWORD) nUserNum );
    ::StringCchCopy(pChaData2->m_szName, CHR_ID_LENGTH, csChaName.GetBuffer());    

    return DB_OK;	
}

int CADOManager::GetClubNumToTransfer( std::vector< DWORD >& vecClubNum )
{
	CjADO AdoGame( m_csGameDBConnString );
	if ( !AdoGame.Execute4Cmd( CString( "dbo.sp_GetAllGuildItemTransferStatus" ), adCmdStoredProc ) )
		return DB_ERROR;

	if ( AdoGame.GetEOF() )
		return DB_OK;

	do
	{
		int GuNum = 0;
		AdoGame.GetCollect( "GuNum", GuNum );
		if ( GuNum != 0 )
		{
			vecClubNum.push_back( GuNum );
		}
	} while ( AdoGame.Next() );

	return DB_OK;
}

int CADOManager::TransferClubLockerItems( DWORD ClubNum /*= 0*/ )
{
	// ClubNum 0 모든 클럽 락커 아이템 변환
	// ClubNum <> 0 특정 클럽 락커 아이템 변환
	if ( 0 == ClubNum )
	{
		CjADO AdoGame( m_csGameDBConnString );
		if ( !AdoGame.Execute4Cmd( CString( "dbo.sp_GetAllGuildItemTransferStatus" ), adCmdStoredProc ) )
			return DB_ERROR;

		if ( AdoGame.GetEOF() )
			return DB_OK;

		std::vector< DWORD > vecGuNum;
		do
		{
			int GuNum = 0;
			AdoGame.GetCollect( "GuNum", GuNum );
			vecGuNum.push_back( GuNum );
		} while ( AdoGame.Next() );

		size_t GuNumSize = vecGuNum.size();
		for ( size_t loop = 0; loop < GuNumSize; loop++ )
		{
			if ( 0 == vecGuNum[loop] )
			{
				continue;
			}

			TransferClubLockerItems( vecGuNum[loop] );
		}
	}
	else
	{
		int Status = 0;
		int MigrationState = 1;
		CjADO Ado( m_csGameDBConnString );
		APPEND_IPARAM_INT( Ado, "@GuNum", ClubNum );
		if ( !Ado.Execute4Cmd( CString( "dbo.sp_GetGuildItemTransferStatus" ), adCmdStoredProc ) )
			return DB_ERROR;

		if ( Ado.GetEOF() )
		{
			return DB_ERROR;
		}

		do
		{
			Ado.GetCollect( "Inven", Status );
			Ado.GetCollect( "MigrationState", MigrationState );
		} while ( Ado.Next() );

		if ( Status != 1 )
		{
			CjADO AdoGame( m_csGameDBConnString );
			APPEND_IPARAM_INT( AdoGame, "@GuNum", ClubNum );
			
			int TransferResult = 1;
			if ( DB_OK != ExecuteStoredProcedure( "dbo.sp_GetGuildInven", AdoGame ) )
			{
				TransferResult = 2;
			}
			else
			{
				if ( AdoGame.GetEOF() )
				{
					TransferResult = 1;
				}
				else
				{
					se::ByteStream ByteStream;
					AdoGame.GetChunk( CString( "GuStorage" ), ByteStream );

					GLClubAgent sClub( NULL );
					if ( 0 == MigrationState )
					{
						sClub.SETSTORAGE_BYBUF_FOR_MIGRATION( ByteStream );
					}
					else
					{
						sClub.SETSTORAGE_BYBUF( ByteStream );
					}

					std::vector< SINVENITEM_SAVE > vecItems;
					sClub.GETSTORAGE_BYVECTOR( vecItems );

					size_t ItemSize = vecItems.size();
					for ( size_t loop = 0; loop < ItemSize; loop++ )
					{
						// Club locker Item 일 경우 아이템 테이블에 ChaNum 이 ClubNum 이다. 
						SINVENITEM_SAVE& sItem = vecItems[loop];
						sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
						sItem.sItemCustom.GenerateBasicStat( false );
						sItem.sItemCustom.GenerateLinkSkill();
						sItem.sItemCustom.GenerateAddOption();
						ItemInsert( ClubNum, INVEN_CLUB_LOCKER, &vecItems[loop], INSERT_CLUB_LOCKER_ITEM_FOR_TRANSFER );
					}
				}
			}

			CjADO Ado( m_csGameDBConnString );
			APPEND_RPARAM_INT( Ado );
			APPEND_IPARAM_INT( Ado, "@GuNum", ClubNum );
			APPEND_IPARAM_INT( Ado, "@Inven", TransferResult );
			APPEND_OPARAM_INT( Ado, "@Return");

			ExecuteStoredProcedureIntReturn( "dbo.sp_SetGuildItemTransferStatus", Ado, "@Return" );
		}
	}
	return DB_OK;
}

int CADOManager::GetUserNumToTransfer( std::vector< DWORD >& vecUserNum )
{
// 	CjADO AdoGame( m_csGameDBConnString );
// 	if ( !AdoGame.Execute4Cmd( CString( "dbo.sp_GetAllUserInvenTransferStatus" ), adCmdStoredProc ) )
// 	{
// 		return DB_ERROR;
// 	}
// 
// 	if ( AdoGame.GetEOF() )
// 	{
// 		return DB_OK;
// 	}
// 
// 	do
// 	{
// 		DWORD Num = 0;
// 		AdoGame.GetCollect( "UserNum", Num );
// 		if ( Num != 0 )
// 		{
// 			vecUserNum.push_back( Num );
// 		}
// 	} while ( AdoGame.Next() );
// 
// 	return DB_OK;

	CString csQuery;
	csQuery.Empty();
	csQuery.Format( _T( "SELECT UserNum FROM ItemTransferStatus WHERE Result=0 ORDER BY LastLoginDate DESC" ) );

	CjADO ado( m_csGameDBConnString );

	if ( !EXECUTE_RAW( ado, csQuery ) )
		return DB_ERROR;

	DWORD UserNum = 0;
	
	BEGIN_GETCOLLECT( ado )
		ado.GetCollect( 0,  UserNum );
		vecUserNum.push_back( UserNum );
	END_GETCOLLECT( ado )

	return DB_OK;
}

// int CADOManager::GetTotalUserNumToTransfer( size_t& TotalUserNum )
// {
// 	CString csQuery;
// 	csQuery.Empty();
// 	csQuery.Format( _T( "SELECT COUNT(UserNum) FROM ItemTransferStatus WHERE Result=0" ) );
// 
// 	CjADO ado( m_csGameDBConnString );
// 
// 	if ( !EXECUTE_RAW( ado, csQuery ) )
// 		return DB_ERROR;
// 
// 	BEGIN_GETCOLLECT( ado )
// 		ado.GetCollect( 0,  TotalUserNum );
// 	END_GETCOLLECT( ado )
// 
// 	return DB_OK;
// }

int CADOManager::PrepareItemTransfer( DWORD UserNum )
{
	CjADO Ado( m_csGameDBConnString );
	APPEND_RPARAM_INT( Ado );
	APPEND_IPARAM_INT( Ado, "@UserNum", UserNum );
	APPEND_OPARAM_INT( Ado, "@Return");

	return ExecuteStoredProcedureIntReturn( "dbo.sp_PrepareItemTransfer", Ado, "@Return" );
}

int CADOManager::CompleteItemTransfer( DWORD UserNum )
{
	CString csQuery;

	csQuery.Empty();
	csQuery.Format( _T( "UPDATE dbo.ItemTransferStatus SET Result = 1 WHERE UserNum = %d"), UserNum );

	CjADO ado( m_csGameDBConnString );

	if ( !EXECUTE_RAW( ado, csQuery ) )
		return DB_ERROR;

	return DB_OK;
}

int CADOManager::UpdateUserInvenOption( DWORD UserNum, int Option )
{
	// 유저락커 락 해제, sp_ItemGetItemLocker 에서 락을 걸도록 되어 있으며, GMCharEdit 에서 이 부분이 호출될 경우 락이 걸리는 문제 발생
	// 게임로직 변경 및 저장프로시저 수정을 하려했으나, 툴 수정으로 마무리한다.
	CjADO Ado( m_csGameDBConnString );
	APPEND_IPARAM_INT( Ado, "@UserNum", UserNum );
	APPEND_IPARAM_INT( Ado, "@Option", Option );
	if ( !Ado.Execute4Cmd( CString( "dbo.sp_UpdateUserInvenOption" ), adCmdStoredProc ) )
		return DB_ERROR;
	return DB_OK;
}

int CADOManager::TransferLockerItems( DWORD UserNum /*= 0*/ )
{
	if ( 0 == UserNum )
	{
		std::vector< DWORD > vecUserNum;
		{
			// 모든 UserInven 변환
			CjADO AdoGame( m_csGameDBConnString );
			if ( !AdoGame.Execute4Cmd( CString( "dbo.sp_GetAllUserInvenTransferStatus" ), adCmdStoredProc ) )
			{
				return DB_ERROR;
			}

			if ( AdoGame.GetEOF() )
			{
				return DB_OK;
			}

			do
			{
				DWORD Num = 0;
				AdoGame.GetCollect( "UserNum", Num );
				vecUserNum.push_back( Num );
			} while ( AdoGame.Next() );
		}

		size_t UserNumSize = vecUserNum.size();
		for ( size_t loop = 0; loop < UserNumSize; loop++ )
		{
			if ( 0 == vecUserNum[loop] )
			{
				continue;
			}

			TransferLockerItems( vecUserNum[loop] );
		}
	}
	else
	{
		// 변환 상태 확인
		CjADO AdoGame( m_csGameDBConnString );
		APPEND_IPARAM_INT( AdoGame, "@UserNum", UserNum );
		if ( !AdoGame.Execute4Cmd( CString( "dbo.sp_GetUserItemTransferStatus" ), adCmdStoredProc ) )
		{
			return DB_ERROR;
		}

		// 테이블에 데이터가 있는 유저만 변환한다. 데이터가 없는 경우는 초기에 넣는 것을 빼먹었거나, 신규 유저이다.
		if ( AdoGame.GetEOF() )
		{
			return DB_OK;
		}

		int InvenStatus = 0;
		do
		{
			AdoGame.GetCollect( "Inven", InvenStatus );
		} while ( AdoGame.Next() );

		// 0 초기 상태
		// 1 변환 완료
		// 그 외는 에러 상황
		if ( 1 != InvenStatus )
		{
			// 변환한다
			// 가져오기
			int TransferResult = 1;
			GLCHARLOGIC sCharacter;
			sCharacter.SetUserID( UserNum );
			if ( DB_ERROR == GetUserBinaryInvenItems( UserNum, &sCharacter ) )
			{
				TransferResult = 2;
			}
			else
			{
				// 저장
				std::vector< SINVENITEM_SAVE > vecItems;
				sCharacter.GETSTORAGE_BYVECTOR( vecItems );
				size_t ItemSize = vecItems.size();
				for ( size_t loop = 0; loop < ItemSize; loop++ )
				{
					SINVENITEM_SAVE& sItem = vecItems[loop];
					sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
					sItem.sItemCustom.GenerateBasicStat( false );
					sItem.sItemCustom.GenerateLinkSkill();
					sItem.sItemCustom.GenerateAddOption();
					ItemInsert( UserNum, INVEN_LOCKER, &vecItems[loop], INSERT_USER_LOCKER_ITEM_FOR_TRANSFER_ALL );
				}
			}

			CjADO Ado( m_csGameDBConnString );
			APPEND_RPARAM_INT( Ado );
			APPEND_IPARAM_INT( Ado, "@UserNum", UserNum );
			APPEND_IPARAM_INT( Ado, "@Inven", TransferResult );
			APPEND_OPARAM_INT( Ado, "@Return");
			ExecuteStoredProcedureIntReturn( "dbo.sp_SetUserItemTransferStatus", Ado, "@Return" );
		}
	}
	return DB_OK;
}

int CADOManager::GetChaNumToTransfer( std::vector< DWORD >& vecChaNum )
{
	CjADO AdoGame( m_csGameDBConnString );
	if ( !AdoGame.Execute4Cmd( CString( "dbo.sp_GetAllCharacterItemTransferStatus" ), adCmdStoredProc ) )
	{
		return DB_ERROR;
	}

	if ( AdoGame.GetEOF() )
	{
		return DB_OK;
	}

	do
	{
		DWORD Num = 0;
		AdoGame.GetCollect( "ChaNum", Num );
		if ( Num != 0 )
		{
			vecChaNum.push_back( Num );
		}
	} while ( AdoGame.Next() );

	return DB_OK;
}

int CADOManager::GetCharacterItemTransferTarget( int UserNum, std::vector< DWORD >& vecChaNum )
{
	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@UserNum", UserNum );
	if ( !AdoGame.Execute4Cmd( CString( "dbo.sp_GetCharacterItemTransferTarget" ), adCmdStoredProc ) )
	{
		return DB_ERROR;
	}

	if ( AdoGame.GetEOF() )
	{
		return DB_OK;
	}

	do
	{
		DWORD ChaNum = 0;
		AdoGame.GetCollect( "ChaNum", ChaNum );
		vecChaNum.push_back( ChaNum );
	} while ( AdoGame.Next() );

	return DB_OK;
}

int CADOManager::TransferCharacterItems( DWORD ChaNum /*= 0*/ )
{
	if ( 0 == ChaNum )
	{
		std::vector< DWORD > vecChaNum;
		{
			// 모든 캐릭터 아이템 변환
			CjADO AdoGame( m_csGameDBConnString );
			if ( !AdoGame.Execute4Cmd( CString( "dbo.sp_GetAllCharacterItemTransferStatus" ), adCmdStoredProc ) )
			{
				return DB_ERROR;
			}

			if ( AdoGame.GetEOF() )
			{
				return DB_OK;
			}

			do
			{
				DWORD Num = 0;
				AdoGame.GetCollect( "ChaNum", Num );
				vecChaNum.push_back( Num );
			} while ( AdoGame.Next() );
		}

		size_t ChaNumSize = vecChaNum.size();
		for ( size_t loop = 0; loop < ChaNumSize; loop++ )
		{
			if ( 0 == vecChaNum[loop] )
			{
				continue;
			}

			TransferCharacterItems( vecChaNum[loop] );
		}
	}
	else
	{
		SCHARDATA2 sCharacter;
		sCharacter.m_CharDbNum = ChaNum;

		// 변환 상태 확인
		CjADO AdoGame( m_csGameDBConnString );
		APPEND_IPARAM_INT( AdoGame, "@ChaNum", ChaNum );
		if ( !AdoGame.Execute4Cmd( CString( "dbo.sp_GetCharacterItemTransferStatus" ), adCmdStoredProc ) )
		{
			return DB_ERROR;
		}

		int PutonStatus = 0;
		int InvenStatus = 0;
		int QuestStatus = 0;
		int PetStatus = 0;
		int VehicleStatus = 0;
		int PostStatus = 0;
		int PrivateMarketStatus = 0;

		int PutonTransferResult = 1;
		int InvenTransferResult = 1;
		int QuestTransferResult = 1;
		int PetTransferResult = 1;
		int VehicleTransferResult = 1;
		int PostTransferResult = 1;
		int PrivateMarketTransferResult = 1;

		if ( AdoGame.GetEOF() )
		{
			return DB_OK;
		}

		do
		{
			AdoGame.GetCollect( "Puton", PutonStatus );
			AdoGame.GetCollect( "Inven", InvenStatus );
			AdoGame.GetCollect( "Quest", QuestStatus );
			AdoGame.GetCollect( "Pet", PetStatus );
			AdoGame.GetCollect( "Vehicle", VehicleStatus );
			AdoGame.GetCollect( "Post", PostStatus );
			AdoGame.GetCollect( "PrivateMarket", PrivateMarketStatus );
		} while ( AdoGame.Next() );

		int MigrationTarget = 0;
		{
			CjADO ado( m_csGameDBConnString );
			APPEND_RPARAM_INT( ado );
			APPEND_IPARAM_INT( ado, "@nChaNum",	ChaNum );
			APPEND_OPARAM_INT( ado, "@nReturn");
			MigrationTarget = ExecuteStoredProcedureIntReturn( "dbo.sp_GetMigrationTarget", ado, "@nReturn" );
		}

		{
			// puton
			if ( 1 != PutonStatus )
			{
				// 착용 아이템 가져오기
				if ( DB_ERROR == GetCharacterBinaryPutOnItems( ChaNum, &sCharacter, MigrationTarget ) )
				{
					PutonTransferResult = 2;
				}
				else
				{
					// 착용 아이템 저장
					std::vector< SINVENITEM_SAVE > vecItems;
					sCharacter.GETPUTONITEMS_BYVECTOR( vecItems );
					size_t ItemSize = vecItems.size();
					for ( size_t loop = 0; loop < ItemSize; loop++ )
					{
						SINVENITEM_SAVE& sItem = vecItems[loop];
						sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
						sItem.sItemCustom.GenerateBasicStat( false );
						sItem.sItemCustom.GenerateLinkSkill();
						sItem.sItemCustom.GenerateAddOption();
						ItemInsert( ChaNum, INVEN_PUTON, &vecItems[loop], INSERT_PUTON_ITEM_FOR_TRANSFER_ALL );
					}
				}
			}
		}

		{
			// inventory
			if ( 1 != InvenStatus )
			{
				// 인벤토리 가져오기
				if ( DB_ERROR == GetCharacterBinaryInvenItems( ChaNum, &sCharacter, MigrationTarget ) )
				{
					InvenTransferResult = 2;
				}
				else
				{
					// 인벤토리 저장
					std::vector< SINVENITEM_SAVE > vecItems;
					sCharacter.m_cInventory.GET_ITEM_BY_VECTOR( vecItems );
					size_t ItemSize = vecItems.size();
					for ( size_t loop = 0; loop < ItemSize; loop++ )
					{
						SINVENITEM_SAVE& sItem = vecItems[loop];
						sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
						sItem.sItemCustom.GenerateBasicStat( false );
						sItem.sItemCustom.GenerateLinkSkill();
						sItem.sItemCustom.GenerateAddOption();
						ItemInsert( ChaNum, INVEN_INVEN, &vecItems[loop], INSERT_INVEN_ITEM_FOR_TRANSFER_ALL );
					}
				}
			}
		}

		{
			// quest
			if ( 1 != QuestStatus )
			{
				// 퀘스트 가져오기
				if ( DB_ERROR == GetCharacterBinaryQuest( ChaNum, &sCharacter, MigrationTarget ) )
				{
					QuestTransferResult = 2;
				}
				else
				{
					// 퀘스트 아이템 저장하기
					{
						// 진행 퀘스트
						GLQuestPlay::MAPQUEST& quest = sCharacter.m_cQuestPlay.GetQuestProc();
						GLQuestPlay::MAPQUEST_CITER iter = quest.begin();
						GLQuestPlay::MAPQUEST_CITER iter_end = quest.end();
						for ( ; iter != iter_end; ++iter )
						{
							std::vector< SINVENITEM_SAVE > vecItems;
							GLQUESTPROG* pQuestProg = (*iter).second;
							if ( !pQuestProg )
							{
								continue;
							}
							pQuestProg->m_sINVENTORY.GET_ITEM_BY_VECTOR( vecItems );
							size_t ItemSize = vecItems.size();
							for ( size_t loop = 0; loop < ItemSize; loop++ )
							{
								// Quest Item 일 경우에 Costume 의 MID, SID 가 Quest 의 MID,SID 이다.
								SINVENITEM_SAVE& sItem = vecItems[loop];
								sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
								sItem.sItemCustom.GenerateBasicStat( false );
								sItem.sItemCustom.GenerateLinkSkill();
								sItem.sItemCustom.GenerateAddOption();
								sItem.sItemCustom.nidDISGUISE.wMainID = pQuestProg->m_sNID.wMainID;
								sItem.sItemCustom.nidDISGUISE.wSubID = pQuestProg->m_sNID.wSubID;
								ItemInsert( ChaNum, INVEN_QUEST_PROC, &vecItems[loop], INSERT_QUEST_ITEM_FOR_TRANSFER_ALL );
							}
						}
					}
					/*
					{
						// 완료 퀘스트
						GLQuestPlay::MAPQUEST& quest = sCharacter.m_cQuestPlay.GetQuestEnd();
						GLQuestPlay::MAPQUEST_CITER iter = quest.begin();
						GLQuestPlay::MAPQUEST_CITER iter_end = quest.end();
						for ( ; iter != iter_end; ++iter )
						{
							std::vector< SINVENITEM_SAVE > vecItems;
							GLQUESTPROG* pQuestProg = (*iter).second;
							if ( !pQuestProg )
							{
								continue;
							}
							pQuestProg->m_sINVENTORY.GET_ITEM_BY_VECTOR( vecItems );
							size_t ItemSize = vecItems.size();
							for ( size_t loop = 0; loop < ItemSize; loop++ )
							{
								// Quest Item 일 경우에 Costume 의 MID, SID 가 Quest 의 MID,SID 이다.
								SINVENITEM_SAVE& sItem = vecItems[loop];
								sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
								sItem.sItemCustom.nidDISGUISE.wMainID = pQuestProg->m_sNID.wMainID;
								sItem.sItemCustom.nidDISGUISE.wSubID = pQuestProg->m_sNID.wSubID;
								ItemInsert( ChaNum, INVEN_QUEST_END, &vecItems[loop], INSERT_QUEST_END_ITEM_FOR_TRANSFER_ALL );
							}
						}
					}
					*/
					{
						// 새로운 버전으로 퀘스트 저장
						if ( DB_OK != SaveCharacterQuest( &sCharacter ) )
						{
							QuestTransferResult = 3;
						}
					}
				}
			}
		}

		{
			if ( 1 != PetStatus )
			{
				// Pet
				// 캐릭터에 속하는 모든 pet 을 불러와서 아이템 변환한다.
				// 캐릭터가 가지고 있는 모든 펫 가져오기
				std::vector< GLPET > vecPets;
				if ( DB_ERROR == GetAllPetPutonItems( ChaNum, vecPets ) )
				{
					PetTransferResult = 2;
				}
				else
				{
					// 저장하기
					size_t PetSize = vecPets.size();
					for ( size_t loop = 0; loop < PetSize; loop++ )
					{
						std::vector< SINVENITEM_SAVE > vecItems;
						GLPET& sPet = vecPets[loop];
						sPet.GETPUTONITEMS_BYVECTOR( vecItems );

						size_t ItemSize = vecItems.size();
						for ( size_t loop2 = 0; loop2 < ItemSize; loop2++ )
						{
							// Pet puton Item 일 경우 아이템 테이블에 ChaNum 이 PetNum 이다. 
							SINVENITEM_SAVE& sItem = vecItems[loop2];
							sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
							sItem.sItemCustom.GenerateBasicStat( false );
							sItem.sItemCustom.GenerateLinkSkill();
							sItem.sItemCustom.GenerateAddOption();
							ItemInsert( sPet.m_DbNum, INVEN_PET, &vecItems[loop2], INSERT_PET_ITEM_FOR_TRANSFER_ALL );
						}

						// 펫 마이그레이션 처리
						SetPetMigrationState( ChaNum, sPet.m_DbNum, 1 );
					}
				}
			}
		}

		{
			if ( 1 != VehicleStatus )
			{
				// Vehicle
				// 캐릭터에 속하는 모든 Vehicle 을 불러와서 아이템 변환한다.
				// 캐릭터가 가지고 있는 모든 Vehicle 가져오기
				std::vector< GLVEHICLE > vecVehicles;
				if ( DB_ERROR == GetAllVehiclePutonItems( ChaNum, vecVehicles ) )
				{
					VehicleTransferResult = 2;
				}
				else
				{
					// 저장하기
					size_t VehicleSize = vecVehicles.size();
					for ( size_t loop = 0; loop < VehicleSize; loop++ )
					{
						std::vector< SINVENITEM_SAVE > vecItems;
						GLVEHICLE& sVehicle = vecVehicles[loop];
						sVehicle.GETPUTONITEMS_BYVECTOR( vecItems );

						size_t ItemSize = vecItems.size();
						for ( size_t loop2 = 0; loop2 < ItemSize; loop2++ )
						{
							// Pet puton Item 일 경우 아이템 테이블에 ChaNum 이 PetNum 이다. 
							SINVENITEM_SAVE& sItem = vecItems[loop2];
							sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
							sItem.sItemCustom.GenerateBasicStat( false );
							sItem.sItemCustom.GenerateLinkSkill();
							sItem.sItemCustom.GenerateAddOption();
							ItemInsert( sVehicle.m_DbNum, INVEN_VEHICLE, &vecItems[loop2], INSERT_VEHICLE_ITEM_FOR_TRANSFER_ALL );
						}

						// 탈것 마이그레이션 처리
						SetVehicleMigrationState( ChaNum, sVehicle.m_DbNum, 1 );
					}
				}
			}
		}

		{
			// Post
			PostTransferResult = 0;
		}

		{
			// PrivateMarket
			PrivateMarketTransferResult = 0;
		}

		CjADO Ado( m_csGameDBConnString );
		APPEND_RPARAM_INT( Ado );
		APPEND_IPARAM_INT( Ado, "@ChaNum", ChaNum );
		APPEND_IPARAM_INT( Ado, "@Puton", PutonTransferResult );
		APPEND_IPARAM_INT( Ado, "@Inven", InvenTransferResult );
		APPEND_IPARAM_INT( Ado, "@Quest", QuestTransferResult );
		APPEND_IPARAM_INT( Ado, "@Pet", PetTransferResult );
		APPEND_IPARAM_INT( Ado, "@Vehicle", VehicleTransferResult );
		APPEND_IPARAM_INT( Ado, "@Post", PostTransferResult );
		APPEND_IPARAM_INT( Ado, "@PrivateMarket", PrivateMarketTransferResult );
		APPEND_OPARAM_INT( Ado, "@Return");
		ExecuteStoredProcedureIntReturn( "dbo.sp_SetCharacterItemTransferStatus", Ado, "@Return" );

		// 캐릭터
		SetMigrationTarget( ChaNum );
	}
	return DB_OK;
}

int CADOManager::ItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType )
{
	if ( pItem == NULL )
		return DB_ERROR;

	const SITEM* pItemData = GLogicData::GetInstance().GetItem( pItem->sItemCustom.GetRealNativeID() );
	if ( !pItemData )
	{
		sc::writeLogError( sc::string::format(
			"CADOManager::ItemInsert InvenType: %1% mid: %2% sid: %3%", InvenType, pItem->sItemCustom.GetRealNativeID().wMainID, pItem->sItemCustom.GetRealNativeID().wSubID ) );
		return DB_OK;
	}

	std::string strGuid = sc::string::uuidToString( pItem->sItemCustom.guid );
	sc::string::upperString( strGuid );

	CjADO Ado( m_csGameDBConnString );
	APPEND_RPARAM_INT( Ado );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_IPARAM_INT( Ado, "@ChaNum", ChaDbNum );
	APPEND_IPARAM_TINYINT( Ado, "@InvenType", InvenType );
	APPEND_IPARAM_SMALL( Ado, "@ItemMID", pItem->sItemCustom.GetRealNativeID().wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemSID", pItem->sItemCustom.GetRealNativeID().wSubID );
	APPEND_IPARAM_TINYINT( Ado, "@ItemMakeType", pItem->sItemCustom.cGenType );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeMID", pItem->sItemCustom.nidDISGUISE.wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeSID", pItem->sItemCustom.nidDISGUISE.wSubID );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosX", pItem->wPosX );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosY", pItem->wPosY );
	APPEND_IPARAM_SMALL( Ado, "@ItemRemain", pItem->sItemCustom.wTurnNum );

	std::string ItemCreateDate = sc::time::DateTimeFormat( pItem->sItemCustom.tBORNTIME, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemCreateDate", ItemCreateDate.c_str(), ItemCreateDate.length() );

	std::string CostumeExpireDate = sc::time::DateTimeFormat( pItem->sItemCustom.tDISGUISE, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemCostumeExpireDate", CostumeExpireDate.c_str(), CostumeExpireDate.length() );

	APPEND_IPARAM_INT( Ado, "@ItemDurability", pItem->sItemCustom.wDurability );
	APPEND_IPARAM_INT( Ado, "@ItemDurabilityCount", pItem->sItemCustom.dwDurabilityCount );
	APPEND_IPARAM_INT( Ado, "@PointTradeCount", pItem->sItemCustom.wRanPointTradeCount );

	APPEND_IPARAM_TINYINT( Ado, "@ItemAttack", pItem->sItemCustom.cDAMAGE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemDefense", pItem->sItemCustom.cDEFENSE );

	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType1", pItem->sItemCustom.randomOption.option[0].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue1", pItem->sItemCustom.randomOption.option[0].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType2", pItem->sItemCustom.randomOption.option[1].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue2", pItem->sItemCustom.randomOption.option[1].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType3", pItem->sItemCustom.randomOption.option[2].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue3", pItem->sItemCustom.randomOption.option[2].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType4", pItem->sItemCustom.randomOption.option[3].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue4", pItem->sItemCustom.randomOption.option[3].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType5", pItem->sItemCustom.randomOption.option[4].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue5", pItem->sItemCustom.randomOption.option[4].nOptValue );

	APPEND_IPARAM_TINYINT( Ado, "@ItemResistFire", pItem->sItemCustom.cRESIST_FIRE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistIce", pItem->sItemCustom.cRESIST_ICE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistElec", pItem->sItemCustom.cRESIST_ELEC );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistPoison", pItem->sItemCustom.cRESIST_POISON );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistSpirit", pItem->sItemCustom.cRESIST_SPIRIT );

	APPEND_IPARAM_INT( Ado, "@ItemPetNum", pItem->sItemCustom.m_PetDbNum );
	APPEND_IPARAM_INT( Ado, "@ItemVehicleNum", pItem->sItemCustom.m_VehicleDbNum );

	APPEND_IPARAM_INT( Ado, "@ItemMainColor", pItem->sItemCustom.dwMainColor );
	APPEND_IPARAM_INT( Ado, "@ItemSubColor", pItem->sItemCustom.dwSubColor );

	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType1", pItem->sItemCustom.costumeUser.Invest[0].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint1", pItem->sItemCustom.costumeUser.Invest[0].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType2", pItem->sItemCustom.costumeUser.Invest[1].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint2", pItem->sItemCustom.costumeUser.Invest[1].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType3", pItem->sItemCustom.costumeUser.Invest[2].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint3", pItem->sItemCustom.costumeUser.Invest[2].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType4", pItem->sItemCustom.costumeUser.Invest[3].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint4", pItem->sItemCustom.costumeUser.Invest[3].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType5", pItem->sItemCustom.costumeUser.Invest[4].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint5", pItem->sItemCustom.costumeUser.Invest[4].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType6", pItem->sItemCustom.costumeUser.Invest[5].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint6", pItem->sItemCustom.costumeUser.Invest[5].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType7", pItem->sItemCustom.costumeUser.Invest[6].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint7", pItem->sItemCustom.costumeUser.Invest[6].wInvestPt );
	std::string CostumeStatEndDate = sc::time::DateTimeFormat( pItem->sItemCustom.costumeUser.tEndTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@CostumeStatEndDate", CostumeStatEndDate.c_str(), CostumeStatEndDate.length() );

	APPEND_IPARAM_TINYINT( Ado, "@TempGrindValue", pItem->sItemCustom.sTLGrind.cGradeValue );
	std::string TempGrindFireDate = sc::time::DateTimeFormat( pItem->sItemCustom.sTLGrind.tFireTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@TempGrindFireDate", TempGrindFireDate.c_str(), TempGrindFireDate.length() );

	APPEND_IPARAM_INT( Ado, "@ItemVersion", SINVENITEM_SAVE::VERSION );
	APPEND_IPARAM_INT( Ado, "@ItemOption", 0 );

	APPEND_IPARAM_TINYINT( Ado, "@LockBox", pItem->sItemCustom.bVietnamGainItem == true ? 1 : 0 );

	APPEND_IPARAM_INT( Ado, "@BasicAttackDamage", pItem->sItemCustom.GetBasicAttackDamage().GetData() );
	APPEND_IPARAM_SMALL( Ado, "@BasicDefence", pItem->sItemCustom.GetBasicDefence() );

	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillMID", pItem->sItemCustom.GetLinkSkillID().wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillSID", pItem->sItemCustom.GetLinkSkillID().wSubID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillLevel", pItem->sItemCustom.GetLinkSkillLevel() );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillTarget", static_cast< WORD >( pItem->sItemCustom.GetLinkSkillTarget() ) );
	APPEND_IPARAM_FLOAT( Ado, "@ItemLinkSkillOccurRate", pItem->sItemCustom.GetLinkSkillOccurRate() );
	
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType1", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[0].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue1", pItem->sItemCustom.sAddonInfo.m_sADDON[0].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType2", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[1].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue2", pItem->sItemCustom.sAddonInfo.m_sADDON[1].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType3", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[2].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue3", pItem->sItemCustom.sAddonInfo.m_sADDON[2].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType4", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[3].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue4", pItem->sItemCustom.sAddonInfo.m_sADDON[3].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType5", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[4].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue5", pItem->sItemCustom.sAddonInfo.m_sADDON[4].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType6", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[5].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue6", pItem->sItemCustom.sAddonInfo.m_sADDON[5].nVALUE );

	APPEND_OPARAM_INT( Ado, "@Return");

	return ExecuteStoredProcedureIntReturn( "dbo.sp_ItemCreate", Ado, "@Return" );
}

int CADOManager::ItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType )
{
	if ( pItem == NULL )
		return DB_ERROR;

	std::string strGuid = sc::string::uuidToString( pItem->sItemCustom.guid );
	sc::string::upperString( strGuid );

	CjADO Ado( m_csGameDBConnString );
	APPEND_RPARAM_INT( Ado );

	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_IPARAM_INT( Ado, "@ChaNum", ChaDbNum );
	APPEND_IPARAM_TINYINT( Ado, "@InvenType", InvenType );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeMID", pItem->sItemCustom.nidDISGUISE.wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeSID", pItem->sItemCustom.nidDISGUISE.wSubID );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosX", pItem->wPosX );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosY", pItem->wPosY );
	APPEND_IPARAM_SMALL( Ado, "@ItemRemain", pItem->sItemCustom.wTurnNum );

	std::string CostumeExpireDate = sc::time::DateTimeFormat( pItem->sItemCustom.tDISGUISE, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemCostumeExpireDate", CostumeExpireDate.c_str(), CostumeExpireDate.length() );

	std::string strExtension_basicTime = sc::time::DateTimeFormat( pItem->sItemCustom.GetPeriodExBasic_ConvertDBData(), true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemBasicPeriodExTime", strExtension_basicTime.c_str(), strExtension_basicTime.length() );

	std::string strExtension_disuiseTime = sc::time::DateTimeFormat( pItem->sItemCustom.GetPeriodExDisguise_ConvertDBData(), true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemDisguisePeriodExTime", strExtension_disuiseTime.c_str(), strExtension_disuiseTime.length() );

	APPEND_IPARAM_INT( Ado, "@ItemBasicPeriodExCount", pItem->sItemCustom.cPeriodExtensionCount );
	APPEND_IPARAM_INT( Ado, "@ItemDisuisePeriodExCount", pItem->sItemCustom.cCostumePeriodExtensionCount );

	APPEND_IPARAM_INT( Ado, "@ItemDurability", pItem->sItemCustom.wDurability );
	APPEND_IPARAM_INT( Ado, "@ItemDurabilityCount", pItem->sItemCustom.dwDurabilityCount );
	APPEND_IPARAM_INT( Ado, "@PointTradeCount", pItem->sItemCustom.wRanPointTradeCount );

	APPEND_IPARAM_TINYINT( Ado, "@ItemAttack", pItem->sItemCustom.cDAMAGE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemDefense", pItem->sItemCustom.cDEFENSE );

	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType1", pItem->sItemCustom.randomOption.option[0].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue1", pItem->sItemCustom.randomOption.option[0].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType2", pItem->sItemCustom.randomOption.option[1].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue2", pItem->sItemCustom.randomOption.option[1].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType3", pItem->sItemCustom.randomOption.option[2].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue3", pItem->sItemCustom.randomOption.option[2].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType4", pItem->sItemCustom.randomOption.option[3].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue4", pItem->sItemCustom.randomOption.option[3].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType5", pItem->sItemCustom.randomOption.option[4].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue5", pItem->sItemCustom.randomOption.option[4].nOptValue );

	APPEND_IPARAM_TINYINT( Ado, "@ItemResistFire", pItem->sItemCustom.cRESIST_FIRE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistIce", pItem->sItemCustom.cRESIST_ICE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistElec", pItem->sItemCustom.cRESIST_ELEC );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistPoison", pItem->sItemCustom.cRESIST_POISON );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistSpirit", pItem->sItemCustom.cRESIST_SPIRIT );

	APPEND_IPARAM_INT( Ado, "@ItemPetNum", pItem->sItemCustom.m_PetDbNum );
	APPEND_IPARAM_INT( Ado, "@ItemVehicleNum", pItem->sItemCustom.m_VehicleDbNum );

	APPEND_IPARAM_INT( Ado, "@ItemMainColor", pItem->sItemCustom.dwMainColor );
	APPEND_IPARAM_INT( Ado, "@ItemSubColor", pItem->sItemCustom.dwSubColor );

	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType1", pItem->sItemCustom.costumeUser.Invest[0].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint1", pItem->sItemCustom.costumeUser.Invest[0].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType2", pItem->sItemCustom.costumeUser.Invest[1].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint2", pItem->sItemCustom.costumeUser.Invest[1].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType3", pItem->sItemCustom.costumeUser.Invest[2].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint3", pItem->sItemCustom.costumeUser.Invest[2].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType4", pItem->sItemCustom.costumeUser.Invest[3].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint4", pItem->sItemCustom.costumeUser.Invest[3].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType5", pItem->sItemCustom.costumeUser.Invest[4].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint5", pItem->sItemCustom.costumeUser.Invest[4].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType6", pItem->sItemCustom.costumeUser.Invest[5].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint6", pItem->sItemCustom.costumeUser.Invest[5].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType7", pItem->sItemCustom.costumeUser.Invest[6].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint7", pItem->sItemCustom.costumeUser.Invest[6].wInvestPt );
	std::string CostumeStatEndDate = sc::time::DateTimeFormat( pItem->sItemCustom.costumeUser.tEndTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@CostumeStatEndDate", CostumeStatEndDate.c_str(), CostumeStatEndDate.length() );

	APPEND_IPARAM_TINYINT( Ado, "@TempGrindValue", pItem->sItemCustom.sTLGrind.cGradeValue );
	std::string TempGrindFireDate = sc::time::DateTimeFormat( pItem->sItemCustom.sTLGrind.tFireTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@TempGrindFireDate", TempGrindFireDate.c_str(), TempGrindFireDate.length() );

	APPEND_IPARAM_INT( Ado, "@ItemVersion", SINVENITEM_SAVE::VERSION );
	APPEND_IPARAM_INT( Ado, "@ItemOption", 0 );

	APPEND_IPARAM_TINYINT( Ado, "@LockBox", pItem->sItemCustom.bVietnamGainItem == true ? 1 : 0 );

	APPEND_IPARAM_INT( Ado, "@BasicAttackDamage", pItem->sItemCustom.GetBasicAttackDamage().GetData() );
	APPEND_IPARAM_SMALL( Ado, "@BasicDefence", pItem->sItemCustom.GetBasicDefence() );

	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillMID", pItem->sItemCustom.GetLinkSkillID().wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillSID", pItem->sItemCustom.GetLinkSkillID().wSubID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillLevel", pItem->sItemCustom.GetLinkSkillLevel() );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillTarget", static_cast< WORD >( pItem->sItemCustom.GetLinkSkillTarget() ) );
	APPEND_IPARAM_FLOAT( Ado, "@ItemLinkSkillOccurRate", pItem->sItemCustom.GetLinkSkillOccurRate() );

	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType1", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[0].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue1", pItem->sItemCustom.sAddonInfo.m_sADDON[0].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType2", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[1].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue2", pItem->sItemCustom.sAddonInfo.m_sADDON[1].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType3", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[2].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue3", pItem->sItemCustom.sAddonInfo.m_sADDON[2].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType4", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[3].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue4", pItem->sItemCustom.sAddonInfo.m_sADDON[3].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType5", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[4].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue5", pItem->sItemCustom.sAddonInfo.m_sADDON[4].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType6", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[5].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue6", pItem->sItemCustom.sAddonInfo.m_sADDON[5].nVALUE );

	APPEND_OPARAM_INT( Ado, "@Return");

	return ExecuteStoredProcedureIntReturn( "dbo.sp_ItemSave", Ado, "@Return" );
}

int CADOManager::ItemDelete( sc::MGUID& Guid, int State, int CallType )
{
	std::string strGuid = sc::string::uuidToString( Guid );
	sc::string::upperString( strGuid );

	CjADO Ado( m_csGameDBConnString );
	APPEND_RPARAM_INT( Ado );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_OPARAM_INT( Ado, "@Return");

	return ExecuteStoredProcedureIntReturn( "dbo.sp_ItemDel", Ado, "@Return" );
}
