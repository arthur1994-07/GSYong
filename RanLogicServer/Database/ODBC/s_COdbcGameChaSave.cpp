#include "../../pch.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../Server/ServerCfg.h"
#include "../../Server/NetServer.h"
#include "../DBAction/DbAction.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 캐릭터를 저장한다
int	COdbcManager::SaveCharacter( LPVOID _pbuffer, bool bAutoSave /*= false*/ )
{
	sc::writeLogError("COdbcManager::SaveCharacter. Use ADO");
	return sc::db::DB_ERROR;

	if ( !_pbuffer )
        return sc::db::DB_ERROR;

	SCHARDATA2* pCharData2 = reinterpret_cast< SCHARDATA2* > (_pbuffer);

	DWORD dwChaNum = pCharData2->m_CharDbNum;
	
	// 해킹한 캐릭터의 경우 레벨이 0 으로 세팅될 가능성이 많다.	
	if ( dwChaNum == 0 || pCharData2->m_wLevel == 0 )
		return sc::db::DB_ERROR;
    
    DWORD dwUserNum = pCharData2->GetUserID();
    DWORD dwSvrNum = pCharData2->m_dwServerID;
    DWORD dwChaID = pCharData2->m_CharDbNum;

	int nDBRet = SaveCharacterBase( pCharData2, dwChaNum );
    if ( nDBRet == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;
		
	se::ByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize = 0;
	
	// Skill 
	pCharData2->GETEXPSKILLS_BYBUF( ByteStream );
	ByteStream.GetBuffer( pBuffer, dwSize );
	if ( pBuffer )
	{
		std::string Query( 
			sc::string::format( 
			"EXEC dbo.ChaInfoUpdateChaSkills %1%, ?", dwChaNum ) );

        /*std::string Query( 
            sc::string::format( 
                "UPDATE ChaInfo SET ChaSkills = ? WHERE ChaNum = %u", dwChaNum ) );*/

		int nResult = m_pGameDB->WriteImage( Query.c_str(), dwChaNum, ( BYTE* ) pBuffer, dwSize );
		if ( sc::db::DB_OK != nResult )
		{
			sc::writeLogError( 
				sc::string::format( 
				"SaveCharacter ChaInfoUpdateChaSkills fail, chanum %1%", dwChaNum ) );
		}
		pBuffer = NULL;
	}

	// Skill Quick Slot	
	pCharData2->GETSKILL_QUICKSLOT( ByteStream );
	ByteStream.GetBuffer( pBuffer, dwSize );
	if ( pBuffer )
	{
		std::string Query( 
			sc::string::format( 
			"EXEC dbo.ChaInfoUpdateChaSkillSlot %1%, ?", dwChaNum ) );

        /*std::string Query( 
            sc::string::format( 
                "UPDATE ChaInfo SET ChaSkillSlot = ? WHERE ChaNum = %u", dwChaNum ) );*/
		int nResult = m_pGameDB->WriteImage( Query.c_str(), dwChaNum, ( BYTE* ) pBuffer, dwSize );
		if ( sc::db::DB_OK != nResult )
		{
			sc::writeLogError( 
				sc::string::format( 
				"SaveCharacter ChaInfoUpdateChaSkillSlot fail, chanum %1%", dwChaNum ) );
		}
		pBuffer = NULL;
	}

	// 2003-11-27 : Add
	// Action Quick Slot
	pCharData2->GETACTION_QUICKSLOT( ByteStream );
	ByteStream.GetBuffer( pBuffer, dwSize );	
	if ( pBuffer )
	{
		std::string Query( 
			sc::string::format( 
			"EXEC dbo.ChaInfoUpdateChaActionSlot %1%, ?", dwChaNum ) );

        /*std::string Query( 
            sc::string::format( 
                "UPDATE ChaInfo SET ChaActionSlot = ? WHERE ChaNum = %u", dwChaNum ) );*/
		int nResult = m_pGameDB->WriteImage( Query.c_str(), dwChaNum, ( BYTE* ) pBuffer, dwSize );
		if ( sc::db::DB_OK != nResult )
		{
			sc::writeLogError( 
				sc::string::format( 
				"SaveCharacter ChaInfoUpdateChaActionSlot fail, chanum %1%", dwChaNum ) );
		}
		pBuffer = NULL;
	}

	// Put on item
	pCharData2->GETPUTONITEMS_BYBUF( ByteStream );
	ByteStream.GetBuffer( pBuffer, dwSize );
	if ( pBuffer )
	{
		std::string Query( 
			sc::string::format( 
			"EXEC dbo.ChaInfoUpdateChaPutOnItems %1%, ?", dwChaNum ) );

        /*std::string Query( 
            sc::string::format( 
                "UPDATE ChaInfo SET ChaPutOnItems = ? WHERE ChaNum = %u", dwChaNum ) );*/
		int nResult = m_pGameDB->WriteImage( Query.c_str(), dwChaNum, ( BYTE* ) pBuffer, dwSize );
		if ( sc::db::DB_OK != nResult )
		{
			sc::writeLogError( 
				sc::string::format( 
				"SaveCharacter ChaInfoUpdateChaPutOnItems fail, chanum %1%", dwChaNum ) );
		}
		pBuffer = NULL;
	}	

	// Inventory
	ByteStream.ClearBuffer();
	pCharData2->GETINVENTORYE_BYBUF( ByteStream );
	ByteStream.GetBuffer( pBuffer, dwSize );
	if ( pBuffer )
	{
		std::string Query( 
			sc::string::format( 
			"EXEC dbo.ChaInfoUpdateChaInven %1%, ?", dwChaNum ) );

        /*std::string Query( 
            sc::string::format( 
                "UPDATE ChaInfo SET ChaInven = ? WHERE ChaNum = %u", dwChaNum ) );*/
		int nResult = m_pGameDB->WriteImage( Query.c_str(), dwChaNum, ( BYTE* ) pBuffer, dwSize );
		if ( sc::db::DB_OK != nResult )
		{
			sc::writeLogError( 
				sc::string::format( 
				"SaveCharacter ChaInfoUpdateChaInven fail, chanum %1%", dwChaNum ) );
		}
		pBuffer = NULL;
	}	

	if ( m_ServiceProvider == SP_VIETNAM )
	{
		// VT ADD Inventory Save
		ByteStream.ClearBuffer();
		pCharData2->GETVTADDINVENTORYE_BYBUF( ByteStream );
		ByteStream.GetBuffer( pBuffer, dwSize );	
		if ( pBuffer != NULL )
		{
			std::string Query( 
				sc::string::format( 
				"UPDATE ChaInfo SET VTAddInven = ? WHERE ChaNum = %u", dwChaNum ) );
			int nResult = m_pGameDB->WriteImage( Query.c_str(), dwChaNum, ( BYTE* ) pBuffer, dwSize );
			if ( sc::db::DB_OK != nResult )
			{
				sc::writeLogError( 
					sc::string::format( 
					"SaveCharacter VT ADD Inventory Save fail, chanum %1%", dwChaNum ) );
			}
			pBuffer = NULL;
		}
	}

	if ( m_ServiceProvider == SP_HONGKONG )
	{
		// 대만 해킹문제로 인해서 로그아웃 하기전 ChaLevel이랑 ChaMoney 값 DB에 저장

		// 저장할 GameMoney를 암호화 한다.
		TCHAR szTemp1[512] = { 0, };
		char szTempChar[CHAR_TEMPMONEY] = { 0, };
		StringCchCopy( szTempChar, CHAR_TEMPMONEY, ENCRYPTMONEY( pCharData2->GetInvenMoney() ).c_str() );
		_snprintf( szTemp1, 512, "Exec sp_InsertChaLastInfo %d, '%s', %u",
			pCharData2->m_wLevel, szTempChar, dwChaNum );

		if ( m_pGameDB->ExecuteSQL( szTemp1 ) != sc::db::DB_OK )
		{
			return sc::db::DB_ERROR;
		}
		
		TCHAR szTemp2[512] = { 0, };
		StringCchCopy( szTempChar, CHAR_TEMPMONEY, ENCRYPTMONEY( pCharData2->GetStorageMoney() ).c_str() );
		_snprintf( szTemp2, 512, "Exec sp_InsertUserLastInfo %u, '%s'",
			pCharData2->GetUserID(), szTempChar );

		if ( m_pGameDB->ExecuteSQL( szTemp2 ) != sc::db::DB_OK )
		{
			return sc::db::DB_ERROR;
		}
	}

    // Character Quest information
	pCharData2->GETQUESTPLAY( ByteStream );
	ByteStream.GetBuffer( pBuffer, dwSize );
	if ( pBuffer )
	{
		std::string Query( 
			sc::string::format( 
			"EXEC dbo.ChaInfoUpdateChaQuest %1%, ?", dwChaNum ) );

        /*std::string Query( 
            sc::string::format( 
                "UPDATE ChaInfo SET ChaQuest = ? WHERE ChaNum = %u", dwChaNum ) );*/
		int nResult = m_pGameDB->WriteImage( Query.c_str(), dwChaNum, ( BYTE* ) pBuffer, dwSize );
		if ( sc::db::DB_OK != nResult )
		{
			sc::writeLogError( 
				sc::string::format( 
				"SaveCharacter ChaInfoUpdateChaQuest fail, chanum %1%", dwChaNum ) );
		}
		pBuffer = NULL;
	}

	// cool time
	pCharData2->GETITEMCOOLTIME_BYBUF( ByteStream );
	ByteStream.GetBuffer( pBuffer, dwSize );
	if ( pBuffer )
	{
		std::string Query(
			sc::string::format(
			"EXEC dbo.ChaInfoUpdateChaCoolTime %1%, ?", dwChaNum ) );

        /*std::string Query(
            sc::string::format(
                "UPDATE ChaInfo SET ChaCoolTime = ? WHERE ChaNum = %u", dwChaNum ) );*/
		int nResult = m_pGameDB->WriteImage( Query.c_str(), dwChaNum, ( BYTE* ) pBuffer, dwSize );
		if ( sc::db::DB_OK != nResult )
		{
			sc::writeLogError( 
				sc::string::format( 
				"SaveCharacter ChaInfoUpdateChaCoolTime fail, chanum %1%", dwChaNum ) );
		}
		pBuffer = NULL;
	}	

    // 사용자 인벤토리 저장
	if ( pCharData2->m_bServerStorage )
	{
		__int64 llMoney = pCharData2->GetStorageMoney();
		DWORD UserNum = pCharData2->GetUserID();
		if ( llMoney < 0 )
		{
			sc::writeLogError( 
				sc::string::format( 
				"$$$$$ Unusual money DB tried save.... $$$$$, %1%, %2%", UserNum, llMoney ) );
			llMoney = 0;
		}

		llMoney = sc::db::AdjustMoney( llMoney );

		std::string Query;
		switch ( m_ServiceProvider )
		{
		case SP_HONGKONG:
			Query = 
				sc::string::format( 
				"EXEC dbo.UpdateUserMoney_3B2C75D1D26646AB9B3E561A3F6CD095 %1%, %2%", UserNum, llMoney );
			break;
		default:
			Query =  
				sc::string::format( 
				"EXEC dbo.UpdateUserMoney %1%, %2%", UserNum, llMoney );
			break;
		}

		if ( m_pGameDB->ExecuteSQL( Query ) != sc::db::DB_OK )
		{
			sc::writeLogError(
				sc::string::format(
				"UpdateUserMoney ExecuteSQL DB_ERROR UserNum %1%",
				UserNum ) );
		}

		pCharData2->GETSTORAGE_BYBUF( ByteStream );
		ByteStream.GetBuffer( pBuffer, dwSize );		
		if ( pBuffer != NULL )
		{
			std::string Query( 
				sc::string::format( 
				"EXEC dbo.UserInvenUpdate %1%, ?", UserNum ) );

			/*std::string Query( 
				sc::string::format( 
				"UPDATE UserInven SET UserInven = ? WHERE UserNum = %u", UserNum ) );*/
			int nResult = m_pGameDB->WriteImage( Query.c_str(), UserNum, ( BYTE* ) pBuffer, dwSize );
			if ( sc::db::DB_OK != nResult )
			{
				sc::writeLogError( 
					sc::string::format( 
					"SaveCharacter UserInvenUpdate fail, UserNum %1%", UserNum ) );
			}
			pBuffer = NULL;
		}
	}

	// 걸려있는 SKillFact
	pCharData2->GETSKILLFACT_BYBUF( ByteStream );
	ByteStream.GetBuffer( pBuffer, dwSize );	
	if ( pBuffer != NULL )
	{
		std::string Query(
			sc::string::format(
			"EXEC dbo.ChaInfoUpdateChaSkillFact %1%, ?", dwChaNum ) );

        /*std::string Query(
            sc::string::format(
                "UPDATE ChaInfo SET ChaSkillFact = ? WHERE ChaNum = %u", dwChaNum ) );*/
		int nResult = m_pGameDB->WriteImage( Query.c_str(), dwChaNum, ( BYTE* ) pBuffer, dwSize );
		if ( sc::db::DB_OK != nResult )
		{
			sc::writeLogError( 
				sc::string::format( 
				"SaveCharacter ChaInfoUpdateChaSkillFact fail, chanum %1%", dwChaNum ) );
		}
		pBuffer = NULL;
	}

	if( nDBRet == sc::db::DB_OK_CHAR_ERROR )
		return sc::db::DB_OK_CHAR_ERROR;
	else
		return sc::db::DB_OK;
}

int COdbcManager::SaveCharacterExtraWithAdo( LPVOID _pbuffer )
{
	if ( !_pbuffer )
	{
		sc::writeLogError( std::string( "COdbcManager::SaveCharacterExtraWithAdo() - pvBuffer is NULL." ) );
		return sc::db::DB_ERROR;
	}

	SCHARDATA2* pCharData2 = reinterpret_cast< SCHARDATA2* >( _pbuffer );

	if( !pCharData2 )
	{
		sc::writeLogError( std::string( "COdbcManager::SaveCharacterExtraWithAdo() - pCharData2 is NULL." ) );
		return sc::db::DB_ERROR;
	}

	UINT nChaNum = pCharData2->m_CharDbNum;	

	if ( nChaNum == 0 || pCharData2->m_wLevel == 0 )	// 해킹한 캐릭터의 경우 레벨이 0 으로 세팅될 가능성이 많다.
	{
		sc::writeLogError( sc::string::format( "COdbcManager::SaveCharacterExtraWithAdo() - Wrong Character! (ChaNum:%1%, Level:%2%).", nChaNum, pCharData2->m_wLevel ) );
		return sc::db::DB_ERROR;
	}

	return S_OK;
}

int COdbcManager::SaveCharacterBase( SCHARDATA2* pCharData2, DWORD ChaDbNum )
{
	int nDBRet = sc::db::DB_OK;

	if ( NULL == pCharData2 )
	{
		return sc::db::DB_ERROR;
	}
	
	if (pCharData2->m_dwUserLvl < USER_GM4)
	{
		if( pCharData2->ValidationCharData() == TRUE)
		{
			nDBRet = sc::db::DB_OK_CHAR_ERROR;
		}
	}

    std::string Query;

    // 베트남 탐직방지 시스템의 추가로 캐릭터 저장하는 부분의 수정
    if ( m_ServiceProvider == SP_VIETNAM )
    {
        Query = sc::string::format( 
            "UPDATE ChaInfo SET "
            "ChaBright=%1%, ChaLevel=%2%, ChaMoney=%3%, ChaDex=%4%, ChaIntel=%5%, "
            "ChaSchool=%6%, ChaHair=%7%, ChaFace=%8%, ChaLiving=%9%, ChaStrong=%10%, "
            "ChaPower=%11%, ChaSpirit=%12%, ChaStrength=%13%, ChaStRemain=%14%, ChaAttackP=%15%, "
            "ChaDefenseP=%16%, ChaFightA=%17%, ChaShootA=%18%, ChaExp=%19%, ChaSkillPoint=%20%, "
            "ChaHP=%21%, ChaMP=%22%, ChaSP=%23%, ChaPK=%24%, ChaStartMap=%25%, "
            "ChaStartGate=%26%, ChaPosX=%27%, ChaPosY=%28%, ChaPosZ=%29%, ChaSaveMap=%30%, "
            "ChaSavePosX=%31%, ChaSavePosY=%32%, ChaSavePosZ=%33%, ChaReturnMap=%34%, ChaReturnPosX=%35%, "
            "ChaReturnPosY=%36%, ChaReturnPosZ=%37%, saveMoney=%38%, saveExp=%39%, itemCount=%40%, "
            "ChaReExp=%41%, ChaSafeTime=%42%, ChaCP=43 "
            "WHERE ChaNum=%44%", 
            pCharData2->m_nBright, 
            pCharData2->m_wLevel, 
            pCharData2->GetInvenMoney(), 
            pCharData2->m_sStats.wDex, 
            pCharData2->m_sStats.wInt, 

            pCharData2->m_wSchool, 
            pCharData2->m_wHair, 
            pCharData2->m_wFace, 
            pCharData2->m_nLiving, 
            pCharData2->m_sStats.wStr, 

            pCharData2->m_sStats.wPow, 
            pCharData2->m_sStats.wSpi, 
            pCharData2->m_sStats.wSta, 
            pCharData2->m_wStatsPoint,

            pCharData2->m_powerAttack,
            pCharData2->m_powerDefence,
            pCharData2->m_powerAttribute[SKILL::EMAPPLY_MELEE], 
            pCharData2->m_powerAttribute[SKILL::EMAPPLY_RANGE],
            pCharData2->m_sExperience.lnNow, 
            pCharData2->m_dwSkillPoint, 

            pCharData2->m_sHP.nNow, 
            pCharData2->m_sMP.nNow, 
            pCharData2->m_sSP.nNow, 
            pCharData2->m_wPK, 
            pCharData2->m_sStartMapID.dwID,

            pCharData2->m_dwStartGate, 
            pCharData2->m_vStartPos.x, 
            pCharData2->m_vStartPos.y, 
            pCharData2->m_vStartPos.z,
            ( int ) pCharData2->m_sSaveMapID.dwID, 

            pCharData2->m_vSavePos.x, 
            pCharData2->m_vSavePos.y, 
            pCharData2->m_vSavePos.z, 
            ( int ) pCharData2->m_sLastCallMapID.dwID, 
            pCharData2->m_vLastCallPos.x,

            pCharData2->m_vLastCallPos.y, 
            pCharData2->m_vLastCallPos.z,            
            pCharData2->m_lVNGainSysMoney,
            pCharData2->m_lVNGainSysExp,
            pCharData2->m_dwVietnamInvenCount,		
            
            pCharData2->m_lnRestorableExp,
            ( short )pCharData2->m_fCDMSafeTime,
            pCharData2->m_sCP.dwData, 
            ChaDbNum );
    }
    else
    {
		Query = sc::string::format( 
			"EXEC dbo.ChaInfoUpdateBase \
			%1%, %2%, %3%, %4%, %5%, \
			%6%, %7%, %8%, %9%, %10%, \
			%11%, %12%, %13%, %14%, %15%, \
			%16%, %17%, %18%, %19%, %20%, \
			%21%, %22%, %23%, %24%, %25%, \
			%26%, %27%, %28%, %29%, %30%, \
			%31%, %32%, %33%, %34%, %35%, \
			%36%, %37%, %38%, %39%, %40%, \
			%41%", 
			pCharData2->m_nBright, 
			static_cast< int >( pCharData2->m_wLevel ), 
			sc::db::AdjustMoney( pCharData2->GetInvenMoney() ), 
			static_cast< int >( pCharData2->m_sStats.wDex ), 
			static_cast< int >( pCharData2->m_sStats.wInt ), 

			static_cast< int >( pCharData2->m_wSchool ), 
			static_cast< int >( pCharData2->m_wHair ), 
			static_cast< int >( pCharData2->m_wFace ), 
			pCharData2->m_nLiving, 
			static_cast< int >( pCharData2->m_sStats.wStr ), 

			static_cast< int >( pCharData2->m_sStats.wPow ), 
			static_cast< int >( pCharData2->m_sStats.wSpi ), 
			static_cast< int >( pCharData2->m_sStats.wSta ), 
			static_cast< int >( pCharData2->m_wStatsPoint ), 
			pCharData2->m_powerAttack, 
			pCharData2->m_powerDefence, 
			pCharData2->m_powerAttribute[SKILL::EMAPPLY_MELEE], 
			pCharData2->m_powerAttribute[SKILL::EMAPPLY_RANGE],
			sc::db::AdjustMoney( pCharData2->m_sExperience.lnNow ), 
			static_cast< int >( pCharData2->m_dwSkillPoint ), 

			pCharData2->m_sHP.nNow, 
			pCharData2->m_sMP.nNow,
			pCharData2->m_sSP.nNow,
			static_cast< int >( pCharData2->m_wPK ), 
			static_cast< int >( pCharData2->m_sStartMapID.dwID ),

			static_cast< int >( pCharData2->m_dwStartGate ), 
			pCharData2->m_vStartPos.x, 
			pCharData2->m_vStartPos.y, 
			pCharData2->m_vStartPos.z,
			static_cast< int >( pCharData2->m_sSaveMapID.dwID ), 

			pCharData2->m_vSavePos.x, 
			pCharData2->m_vSavePos.y, 
			pCharData2->m_vSavePos.z, 
			static_cast< int >( pCharData2->m_sLastCallMapID.dwID ), 
			pCharData2->m_vLastCallPos.x,

			pCharData2->m_vLastCallPos.y, 
			pCharData2->m_vLastCallPos.z,            
			sc::db::AdjustMoney( pCharData2->m_lnRestorableExp ),
			sc::db::AdjustSmallInt( pCharData2->m_fCDMSafeTime ),
			static_cast< int >( pCharData2->m_sCP.dwData ),

			static_cast< int >( ChaDbNum )
			);
    }

    if ( m_pGameDB->ExecuteSp( Query ) != sc::db::DB_OK )
    {
		sc::writeLogError(
			sc::string::format(
			"SaveCharacterBase ExecuteSQL DB_ERROR chanum %1%",
			ChaDbNum ) );
        return sc::db::DB_ERROR;
    }
    else
    {
		if( nDBRet == sc::db::DB_OK_CHAR_ERROR )
			return sc::db::DB_OK_CHAR_ERROR;
		else 
			return sc::db::DB_OK;
    }
}

int COdbcManager::SaveInventoryAndGameMoney( LPVOID _pbuffer )
{
	if ( !_pbuffer )
		return sc::db::DB_ERROR;

	SCHARDATA2* pCharData2 = reinterpret_cast< SCHARDATA2* >( _pbuffer );

	DWORD dwChaNum = pCharData2->m_CharDbNum;

	if ( dwChaNum == 0 || pCharData2->m_wLevel == 0 )
		return sc::db::DB_ERROR;

	se::ByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize = 0;

	// Inventory
	pCharData2->GETINVENTORYE_BYBUF( ByteStream );
	ByteStream.GetBuffer( pBuffer, dwSize );	
	if ( pBuffer )
	{		
		std::string QueryInven( 
			sc::string::format( 
			"UPDATE ChaInfo SET ChaInfo.ChaInven = ? WHERE ChaNum = %u", dwChaNum ) );
		if ( sc::db::DB_OK != m_pGameDB->WriteImage( QueryInven.c_str(), dwChaNum, ( BYTE* )pBuffer, dwSize ) )
		{
			sc::writeLogError( 
				sc::string::format(
				"SaveInventoryAndGameMoney inventory save error ChaNum %1%", dwChaNum ) );
			pBuffer = NULL;
			return sc::db::DB_ERROR;
		}
		pBuffer = NULL;
	}

	// Game money
	std::string QueryGameMoney( 
		sc::string::format( 
		"UPDATE ChaInfo SET ChaMoney = %I64d WHERE ChaNum = %u", pCharData2->GetInvenMoney(), dwChaNum ) );
	if ( sc::db::DB_OK != m_pGameDB->ExecuteSQL( QueryGameMoney.c_str() ) )
	{
		sc::writeLogError( 
			sc::string::format(
			"SaveInventoryAndGameMoney game money save error ChaNum %1%", dwChaNum ) );
		return sc::db::DB_ERROR;
	}

	return sc::db::DB_OK;
}

int	COdbcManager::SaveChaPos(
	DWORD dwChaNum, 
	DWORD dwStartMapID,
	DWORD dwStartGate,
	FLOAT fStartPosX,
	FLOAT fStartPosY,
	FLOAT fStartPosZ,
	DWORD dwSaveMapID,
	FLOAT fSavePosX,
	FLOAT fSavePosY,
	FLOAT fSavePosZ,
	DWORD dwLastCallMapID,
	FLOAT fLastCallPosX,
	FLOAT fLastCallPosY,
	FLOAT fLastCallPosZ )
{
    sc::writeLogError("COdbcManager::SaveChaPos. Use ADO");
    return sc::db::DB_ERROR;
    /*
	TCHAR szTemp[512] = {0};
	StringCchPrintf(
        szTemp,
        512,
        "UPDATE ChaInfo SET "
		 "ChaStartMap=%d"
		 ", ChaStartGate=%d"
		 ", ChaPosX=%f"
		 ", ChaPosY=%f"
		 ", ChaPosZ=%f"
		 ", ChaSaveMap=%d"
		 ", ChaSavePosX=%f"
		 ", ChaSavePosY=%f"
		 ", ChaSavePosZ=%f"
		 ", ChaReturnMap=%d"
		 ", ChaReturnPosX=%f"
		 ", ChaReturnPosY=%f"
		 ", ChaReturnPosZ=%f"
		 " WHERE ChaNum=%u",
        (int) dwStartMapID,
        (int) dwStartGate,
        fStartPosX,
        fStartPosY,
        fStartPosZ,
        (int) dwSaveMapID,
        fSavePosX,
        fSavePosY,
        fSavePosZ,
        (int) dwLastCallMapID,
        fLastCallPosX,
        fLastCallPosY,
        fLastCallPosZ,
        dwChaNum);

	if (m_pGameDB->ExecuteSQL(szTemp) != sc::db::DB_OK)
	{
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return sc::db::DB_ERROR;
	}
	else
	{
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return sc::db::DB_OK;
	}
*/
}
