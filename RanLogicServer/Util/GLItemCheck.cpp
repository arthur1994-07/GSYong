#include "pch.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/Character/GLCharData.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Database/DBAction/DbActionGameItem.h"
#include "./GLItemLimit.h"
#include "./GLItemCheck.h"

#include "../../SigmaCore/DebugInclude.h"

namespace Item
{
	GLItemValidation::GLItemValidation()
		: m_pServer( NULL )
	{
	}

	GLItemValidation::~GLItemValidation()
	{
	}

	bool GLItemValidation::Load( const std::string& strFileName, GLGaeaServer* pGaeaServer )
	{
		m_pServer = pGaeaServer;
		m_strFilePath = strFileName;

		try
		{   
			sc::lua_init();
			if ( !sc::lua_doFile( m_strFilePath ) )
			{
				sc::ErrorMessageBox( sc::string::format( "%1% file missing.(error 7469)", m_strFilePath ), false );
				return false;
			}

			loadCommon();
			loadServer();

			return true;
		}
		catch ( const LuaPlus::LuaException& e )
		{
			sc::ErrorMessageBox( sc::string::format( "%1%, %2%", m_strFilePath, e.GetErrorMessage() ), false );
			return false;
		} 
	}

	int GLItemValidation::Do( EMSERVICE_PROVIDER ServiceProvider, SCHARDATA2* pCharacter )
	{
		/*
		������ ���� �����ϸ鼭 �Ʒ� ��ɵ��� �ּ�ó���Ѵ�.
		GLItemValidation ������ �˻� ���
		�ʵ� ������ ĳ���Ͱ� ��������� ���� �����Ѵ�. GLChar �� �ƴϴ�. coomonlogic �� �ִ� SCHADATA2�� ��� ���� �ڵ带 ���� �� ����. �ϵ� �ڵ� �Ѵ�.
		�ƴϴ�. ������ ��ȯ ����� �����Ѵ�. ���� ��񿡼� ���� �������� �� �ֱ� ����, �׸��� ���⿡ ������ ���� ���� �ڵ� ���� �ڵ� ����������.
		������ ���̵� ���� ��ɵ� ��񿡼� ���˳� �ϰ� ó���ϴ� �������� �Ѵ�.
		�����ڽ�Ƭ �ð��� �߸����� �������ִ� ��ɵ� �����Ѵ�. ��񿡼� ���߿� �ϰ� ������Ʈ
		�������� üũ�� ����, ��񿡼� ���� �����ϵ��� ��
		*/

		switch ( ServiceProvider )
		{
		case SP_GS:
			{
				ValidateVehicleRandomOption( pCharacter );	// ValidateVehicleRandomOption ValidateRandomOption ��������Ǹ� �ȵ�
				
				// ���� xml�� ������ �߰��ؼ� �����ʰ� �켱 �̷��� ó���Ѵ�. GS 3,4 ������ ����̴�.
				DWORD ServerGroup = m_pServer->GetServerGroup();
				if (  3 == ServerGroup || 4 == ServerGroup  )
				{
					ValidateRandomOption( pCharacter );
				}
			}
			break;
		case SP_THAILAND:
			{
				ValidateWrappedItem( pCharacter );
			}
			break;
		default:
			break;
		}

		return eSuccess;
		/*
		ExchangeItem( pCharacter );

		// ���� �ڽ�Ƭ �ð� ���� ó��
		RestoreTimelimitCostumeItem( pCharacter );

		ChangeItemID( pCharacter );

		int Result = eSuccess;
		switch ( ServiceProvider )
		{
		case SP_KOREA:
			Result = CheckRandomOption( pCharacter );
			break;
		default:
			break;
		}
		return Result;*/
	}

	int GLItemValidation::ExchangeItem( SCHARDATA2* pCharacter )
	{
		return eSuccess;
		/*
		if ( NULL == pCharacter )
		{
			return eCharacterNull;
		}

		// ������ ��ȯ(��ȯ) ó��
		size_t ItemExchangeSize = m_ItemExchange.size();
		for ( size_t loop = 0; loop < ItemExchangeSize; loop++ )
		{
			ItemExchange& sExchange = m_ItemExchange[loop];

			// �κ��丮
			DWORD dwItemCount = pCharacter->m_cInventory.CountTurnItem( sExchange.sOldItemID );
			if ( dwItemCount > 0 )
			{
				// ������ �κ��丮�� �׽�Ʈ
				GLInventory InventoryCopy;
				InventoryCopy.Assign( pCharacter->m_cInventory );
				int nResult = exchangeItem( pCharacter->CharDbNum(), sExchange.sOldItemID, sExchange.sNewItemID, sExchange.OldItemRatio, sExchange.NewItemRatio, sExchange.Remainder, &InventoryCopy, true );
				if ( eSuccess == nResult )
				{
					// ���� �κ��丮
					nResult = exchangeItem( pCharacter->CharDbNum(), sExchange.sOldItemID, sExchange.sNewItemID, sExchange.OldItemRatio, sExchange.NewItemRatio, sExchange.Remainder, &pCharacter->m_cInventory );
					if ( eSuccess != nResult )
					{
						reportError( pCharacter->CharDbNum(), nResult );
					}
				}
				else
				{
					reportError( pCharacter->CharDbNum(), nResult );
				}
			}

			// â��
			for ( int i = 0; i < EMSTORAGE_CHANNEL; i++ )
			{
				dwItemCount = pCharacter->m_cStorage[i].CountTurnItem( sExchange.sOldItemID );
				if ( 0 == dwItemCount )
				{
					continue;
				}

				// ������ â��� �׽�Ʈ
				GLInventoryStorage InventoryCopy;
				InventoryCopy.Assign( pCharacter->m_cStorage[i] );
				int nResult = exchangeItem( pCharacter->CharDbNum(), sExchange.sOldItemID, sExchange.sNewItemID, sExchange.OldItemRatio, sExchange.NewItemRatio, sExchange.Remainder, &InventoryCopy, true );
				if ( eSuccess != nResult )
				{
					reportError( pCharacter->CharDbNum(), nResult );
					continue;
				}

				// ���� â��
				nResult = exchangeItem( pCharacter->CharDbNum(), sExchange.sOldItemID, sExchange.sNewItemID, sExchange.OldItemRatio, sExchange.NewItemRatio, sExchange.Remainder, &pCharacter->m_cStorage[i] );
				if ( eSuccess != nResult )
				{
					reportError( pCharacter->CharDbNum(), nResult );
				}
			}
		}
		return eSuccess;*/
	}

	int GLItemValidation::RestoreTimelimitCostumeItem( SCHARDATA2* pCharacter )
	{
		return eSuccess;

		/*
		// ItemValidation.lua ���� �ʿ� ����
		// ���� �ڽ�Ƭ�� �����ۿ� �߶����ְ�, ������ ��ȿ�� ���� �ý����� �۵��ϸ�
		// tDISGUISE �� 0�� �ƴϰ� �ȴ�. �� �κ��� �ٽ� �����ش�. 0����
		if ( NULL == pCharacter )
		{
			return eCharacterNull;
		}

		// ����â
		for ( int loop = 0 ; loop < SLOT_TSIZE ; loop++ )
		{
			if ( pCharacter->m_PutOnItems[loop].GetNativeID() == NATIVEID_NULL() )
				continue;
			SITEMCUSTOM& sItemCustom = pCharacter->m_PutOnItems[loop];
			if ( sItemCustom.nidDISGUISE == SNATIVEID( false ) )
				continue;
			if ( sItemCustom.tDISGUISE == 0 )
				continue;
			SITEMCUSTOM TempItem( sItemCustom.nidDISGUISE );
			bool IsTimelimit = TempItem.IsTimeLimit( false );
			if ( false == IsTimelimit )
			{
				sItemCustom.tDISGUISE = 0;
			}
		}

		// �κ��丮
		const GLInventory::CELL_MAP& ItemMap = *( pCharacter->m_cInventory.GetItemList() );
		GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
		GLInventory::CELL_MAP_CITER iterEnd = ItemMap.end();
		for ( ; iter != iterEnd; ++iter )
		{
			SINVENITEM& sInvenItem = *(*iter).second;
			SITEMCUSTOM& sItemCustom = sInvenItem.sItemCustom;
			if ( sItemCustom.nidDISGUISE == SNATIVEID( false ) )
				continue;
			if ( sItemCustom.tDISGUISE == 0 )
				continue;
			SITEMCUSTOM TempItem( sItemCustom.nidDISGUISE );
			bool IsTimelimit = TempItem.IsTimeLimit( false );
			if ( false == IsTimelimit )
			{
				sItemCustom.tDISGUISE = 0;
			}
		}

		// ������Ŀ
		for ( int loop = 0; loop < EMSTORAGE_CHANNEL; loop++ )
		{
			const GLInventory::CELL_MAP& ItemMap = *( pCharacter->m_cStorage[loop].GetItemList() );
			GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
			GLInventory::CELL_MAP_CITER iterEnd = ItemMap.end();
			for ( ; iter != iterEnd; ++iter )
			{
				SINVENITEM& sInvenItem = *(*iter).second;
				SITEMCUSTOM& sItemCustom = sInvenItem.sItemCustom;
				if ( sItemCustom.nidDISGUISE == SNATIVEID( false ) )
					continue;
				if ( sItemCustom.tDISGUISE == 0 )
					continue;
				SITEMCUSTOM TempItem( sItemCustom.nidDISGUISE );
				bool IsTimelimit = TempItem.IsTimeLimit( false );
				if ( false == IsTimelimit )
				{
					sItemCustom.tDISGUISE = 0;
				}
			}
		}
		return eSuccess;*/
	}

	int GLItemValidation::CheckRandomOption( SCHARDATA2* pCharacter )
	{
		return eSuccess;

		/*
		int Result = eSuccess;
		if ( NULL == pCharacter )
		{
			Result = eCharacterNull;
			return Result;
		}

		// GM ����ó��
		if ( pCharacter->m_dwUserLvl >= USER_GM3 )
		{
			return Result;
		}

		// ����â
		for ( DWORD loop = 0 ; loop < SLOT_TSIZE ; loop++ )
		{
			if ( pCharacter->m_PutOnItems[loop].GetNativeID() == NATIVEID_NULL() )
				continue;
			if ( checkRandomOption( pCharacter->m_PutOnItems[loop] ) )
			{
				sc::writeLogInfo( sc::string::format( "GLItemValidation::checkRandomOption() isOverStat in puton chanum:%1% mid:%2% sid:%3%", pCharacter->CharDbNum(), pCharacter->m_PutOnItems[loop].GetNativeID().wMainID, pCharacter->m_PutOnItems[loop].GetNativeID().wSubID ) );
				Result = eInvalidRandomOption;
			}
		}

		// �κ��丮
		const GLInventory::CELL_MAP& ItemMap = *( pCharacter->m_cInventory.GetItemList() );
		GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
		GLInventory::CELL_MAP_CITER iterEnd = ItemMap.end();
		for ( ; iter != iterEnd; ++iter )
		{
			SINVENITEM& sInvenItem = *(*iter).second;
			if ( checkRandomOption( sInvenItem.sItemCustom ) )
			{
				sc::writeLogInfo( sc::string::format( "GLItemValidation::checkRandomOption() isOverStat in inventory chanum:%1% mid:%2% sid:%3%", pCharacter->CharDbNum(), sInvenItem.sItemCustom.GetNativeID().wMainID, sInvenItem.sItemCustom.GetNativeID().wSubID ) );
				Result = eInvalidRandomOption;
			}
		}

		// ������Ŀ
		for ( int loop = 0; loop < EMSTORAGE_CHANNEL; loop++ )
		{
			const GLInventory::CELL_MAP& ItemMap = *( pCharacter->m_cStorage[loop].GetItemList() );
			GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
			GLInventory::CELL_MAP_CITER iterEnd = ItemMap.end();
			for ( ; iter != iterEnd; ++iter )
			{
				SINVENITEM& sInvenItem = *(*iter).second;
				if ( checkRandomOption( sInvenItem.sItemCustom ) )
				{
					sc::writeLogInfo( sc::string::format( "GLItemValidation::checkRandomOption() isOverStat in user locker chanum:%1% mid:%2% sid:%3%", pCharacter->CharDbNum(), sInvenItem.sItemCustom.GetNativeID().wMainID, sInvenItem.sItemCustom.GetNativeID().wSubID ) );
					Result = eInvalidRandomOption;
				}
			}
		}

		return Result;*/
	}

	int GLItemValidation::ChangeItemID( SCHARDATA2* pCharacter )
	{
		return eSuccess;

		/*
		// ������ mid, sid �����ϴ� ���
		if ( NULL == pCharacter )
		{
			return eCharacterNull;
		}

		size_t ItemIDChangeSize = m_ItemIDChange.size();
		for ( size_t loop = 0; loop < ItemIDChangeSize; loop++ )
		{
			ItemIDChange& sChange = m_ItemIDChange[loop];

			// ����â
			for ( int loop = 0 ; loop < SLOT_TSIZE ; loop++ )
			{
				if ( pCharacter->m_PutOnItems[loop].GetNativeID() != sChange.sOldItemID )
					continue;
				SITEMCUSTOM& sItemCustom = pCharacter->m_PutOnItems[loop];
				if ( sItemCustom.IsBoxWrapped() )
				{
					sItemCustom.m_PetDbNum = sChange.sNewItemID.dwID;
				}
				else
				{
					sItemCustom.SetNativeID( sChange.sNewItemID );
				}
			}

			// �κ��丮
			const GLInventory::CELL_MAP& ItemMap = *( pCharacter->m_cInventory.GetItemList() );
			GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
			GLInventory::CELL_MAP_CITER iterEnd = ItemMap.end();
			for ( ; iter != iterEnd; ++iter )
			{
				SINVENITEM& sInvenItem = *(*iter).second;
				SITEMCUSTOM& sItemCustom = sInvenItem.sItemCustom;
				if ( sItemCustom.GetNativeID() != sChange.sOldItemID )
					continue;
				if ( sItemCustom.IsBoxWrapped() )
				{
					sItemCustom.m_PetDbNum = sChange.sNewItemID.dwID;
				}
				else
				{
					sItemCustom.SetNativeID( sChange.sNewItemID );
				}
			}

			// ������Ŀ
			for ( int loop = 0; loop < EMSTORAGE_CHANNEL; loop++ )
			{
				const GLInventory::CELL_MAP& ItemMap = *( pCharacter->m_cStorage[loop].GetItemList() );
				GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
				GLInventory::CELL_MAP_CITER iterEnd = ItemMap.end();
				for ( ; iter != iterEnd; ++iter )
				{
					SINVENITEM& sInvenItem = *(*iter).second;
					SITEMCUSTOM& sItemCustom = sInvenItem.sItemCustom;
					if ( sItemCustom.GetNativeID() != sChange.sOldItemID )
						continue;
					if ( sItemCustom.IsBoxWrapped() )
					{
						sItemCustom.m_PetDbNum = sChange.sNewItemID.dwID;
					}
					else
					{
						sItemCustom.SetNativeID( sChange.sNewItemID );
					}
				}
			}
		}

		return eSuccess;*/
	}

	int GLItemValidation::ValidateVehicleRandomOption( SCHARDATA2* pCharacter )
	{
		int Result = eSuccess;
		if ( NULL == pCharacter )
		{
			Result = eCharacterNull;
			return Result;
		}

		// GM ����ó��
		if ( pCharacter->m_dwUserLvl >= USER_GM3 )
		{
			return Result;
		}

		// ����â
		if ( FALSE == pCharacter->ValidateVehicleRandomOption() )
		{
			sc::writeLogError( sc::string::format( 
				"GLItemValidation::ValidateVehicleRandomOption() puton chanum %1%", pCharacter->CharDbNum() ) );
		}

		// �κ��丮
		if ( FALSE == pCharacter->m_cInventory.ValidateVehicleRandomOption() )
		{
			sc::writeLogError( sc::string::format( 
				"GLItemValidation::ValidateVehicleRandomOption() inventory chanum %1%", pCharacter->CharDbNum() ) );
		}

		// ������Ŀ
		for ( int loop = 0; loop < EMSTORAGE_CHANNEL; loop++ )
		{
			if ( FALSE == pCharacter->m_cStorage[loop].ValidateVehicleRandomOption() )
			{
				sc::writeLogError( sc::string::format( 
					"GLItemValidation::ValidateVehicleRandomOption() userlocker chanum %1%", pCharacter->CharDbNum() ) );
			}
		}

		return Result;
	}

	int GLItemValidation::ValidateWrappedItem( SCHARDATA2* pCharacter )
	{
		int Result = eSuccess;
		if ( NULL == pCharacter )
		{
			Result = eCharacterNull;
			return Result;
		}

		// GM ����ó��
		if ( pCharacter->m_dwUserLvl >= USER_GM3 )
		{
			return Result;
		}

		// �κ��丮
		std::vector< SITEMCUSTOM > vecPostItems;
		std::vector< SINVENITEM* > vecUnWrappedItems;
		if ( FALSE == pCharacter->m_cInventory.ValidateWrappedItem( vecPostItems, vecUnWrappedItems ) )
		{
			sc::writeLogInfo( sc::string::format( 
				"GLItemValidation::ValidateWrappedItem() inventory chanum %1%", pCharacter->CharDbNum() ) );
		}

		// ������Ŀ
		for ( int loop = 0; loop < EMSTORAGE_CHANNEL; loop++ )
		{
			if ( FALSE == pCharacter->m_cStorage[loop].ValidateWrappedItem( vecPostItems, vecUnWrappedItems ) )
			{
				sc::writeLogInfo( sc::string::format( 
					"GLItemValidation::ValidateWrappedItem() userlocker chanum %1%", pCharacter->CharDbNum() ) );
			}
		}

		// ������ ���ŵ� ������ ����
		size_t UnWrappedItemSize = vecUnWrappedItems.size();
		for ( size_t loop = 0; loop < UnWrappedItemSize; loop++ )
		{
			SINVENITEM* pInvenItem = vecUnWrappedItems[loop];
			if ( !pInvenItem )
			{
				continue;
			}

			m_pServer->AddGameAdoJob(
				db::DbActionPtr( 
				new db::EntireItemUpdate( pCharacter->CharDbNum(), INVEN_INVEN, pInvenItem->sSaveData, 19709 ) ) );
		}

		// ���� ����
		size_t PostItemSize = vecPostItems.size();
		for ( size_t loop = 0; loop < PostItemSize; loop++ )
		{
			std::string strRecverName( pCharacter->GetName() ), strSenderName( "System" );
			BOOL bResult = m_pServer->PostSendSystemMail(
				pCharacter->CharDbNum(), 
				strRecverName, 
				vecPostItems[loop], 
				0, 
				ATTENDANCE_REWARD_POST_DESIGN, 
				ATTENDANCE_REWARD_POST_SENDER, 
				strSenderName, 
				std::string( ID2SERVERTEXT( "WRAPPER_ITEM_RESTORE_POST_TITLE" ) ), 
				std::string( ID2SERVERTEXT( "WRAPPER_ITEM_RESTORE_POST_CONTENTS" ) ), 
				false
				);

			if ( FALSE == bResult )
			{
				sc::writeLogInfo( sc::string::format( 
					"GLItemValidation::ValidateWrappedItem() PostSendSystemMail fail. chanum %1% mid %2% sid %3%", 
					pCharacter->CharDbNum(), vecPostItems[loop].GetNativeID().wMainID, vecPostItems[loop].GetNativeID().wSubID ) );
			}
		}

		return Result;
	}

	int GLItemValidation::ValidateRandomOption( SCHARDATA2* pCharacter )
	{
		int Result = eSuccess;
		if ( NULL == pCharacter )
		{
			Result = eCharacterNull;
			return Result;
		}

		// GM ����ó��
		if ( pCharacter->m_dwUserLvl >= USER_GM3 )
		{
			return Result;
		}

		std::vector< SITEMCUSTOM > vecDeleteItems;

		// ����â
		if ( FALSE == pCharacter->ValidateRandomOption( vecDeleteItems ) )
		{
			sc::writeLogInfo( sc::string::format( 
				"GLItemValidation::ValidateRandomOption() puton chanum %1%", pCharacter->CharDbNum() ) );
		}

		// �κ��丮
		if ( FALSE == pCharacter->m_cInventory.ValidateRandomOption( vecDeleteItems ) )
		{
			sc::writeLogInfo( sc::string::format( 
				"GLItemValidation::ValidateRandomOption() inventory chanum %1%", pCharacter->CharDbNum() ) );
		}

		// ������Ŀ
		for ( int loop = 0; loop < EMSTORAGE_CHANNEL; loop++ )
		{
			if ( FALSE == pCharacter->m_cStorage[loop].ValidateRandomOption( vecDeleteItems ) )
			{
				sc::writeLogInfo( sc::string::format( 
					"GLItemValidation::ValidateRandomOption() userlocker chanum %1%", pCharacter->CharDbNum() ) );
			}
		}

		size_t DeleteItemSize = vecDeleteItems.size();
		for ( size_t loop = 0; loop < DeleteItemSize; loop++ )
		{
			sc::MGUID guid = vecDeleteItems[loop].GetGuid();
			int DbState = vecDeleteItems[loop].GetDbState();

			m_pServer->AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemDelete( guid, DbState, 59909 ) ) );
		}

		return Result;
	}

	int GLItemValidation::exchangeItem( 
		DWORD ChaDbNum, 
		const SNATIVEID& sOldItemID, 
		const SNATIVEID& sNewItemID, 
		DWORD OldItemRatio, 
		DWORD NewItemRatio, 
		DWORD Remainder, 
		GLInventory* pInventory, 
		bool bTest /*= false*/ )
	{
		return eSuccess;

		/*
		eSuccess, 
		eNothingConvert, 
		eInvalidItemData, 
		eCharacterNull, 
		eInventoryNull, 
		eInventorySpace, 
		ePileNumZero, 
		eInsertFail, 
		*/

		/*
		// NewItemRatio 0 �� ��� ����ó�� �߰� �ʿ�
		if ( 0 == NewItemRatio )
		{
			NewItemRatio = 1;
		}

		if ( NULL == m_pServer )
		{
			return eGaeaServerNull;
		}

		if ( NULL == pInventory )
		{
			return eInventoryNull;
		}

		DWORD dwOldItemCount = pInventory->CountTurnItem( sOldItemID );
		if ( 0 == dwOldItemCount )
		{
			return eNothingConvert;
		}
		
		DWORD dwConvertRatio = OldItemRatio / NewItemRatio;
		DWORD dwRemainder = ( dwOldItemCount % dwConvertRatio ) > 0 ? Remainder : 0;
		DWORD dwNewItemCount = dwOldItemCount / dwConvertRatio + dwRemainder;

		// ���� �������� �����Ѵ�.
		DWORD dwDeletedCount = 0;
		pInventory->DeleteItem( sOldItemID, dwDeletedCount );
		if ( false == bTest )
		{
			SITEMCUSTOM sLog( sOldItemID );
			m_pServer->GetItemLimit()->LogItemExchange(
				sLog,
				gamelog::ID_NONE, 0,
				gamelog::ID_CHAR, ChaDbNum,
				ITEM_ROUTE_DELETE, dwDeletedCount, 
				MapID( SNATIVEID( false ),SNATIVEID( false ) ), LOG_ITEM_DEL_ITEM_BY_VALIDATION,
				false );
		}

		// ���ο� �������� �����Ѵ�.
		const SITEM* pItem = GLogicData::GetInstance().GetItem( sNewItemID );
		if ( !pItem )
			return eInvalidItemData;

		WORD wNums = static_cast< WORD >( dwNewItemCount );
		BOOL bITEM_SPACE( FALSE );

		if ( pItem->ISPILE() )
		{
			WORD wPILENUM = pItem->sDrugOp.wPileNum;
			SNATIVEID sNID = pItem->sBasicOp.sNativeID;
			WORD wREQINSRTNUM = ( wNums * pItem->GETAPPLYNUM() );

			bITEM_SPACE = pInventory->ValidPileInsrt( wREQINSRTNUM, sNID, wPILENUM );
			if ( !bITEM_SPACE )
				return eInventorySpace;

			SITEMCUSTOM sDB_ITEM(sNID);
			sDB_ITEM.cGenType  = EMGEN_SYSTEM;
			sDB_ITEM.cChnID    = 0;
			sDB_ITEM.cFieldID  = 0;
			sDB_ITEM.wTurnNum  = wPILENUM;
			sDB_ITEM.tBORNTIME = CTime::GetCurrentTime().GetTime();

			{
				WORD wPILENUM = pItem->sDrugOp.wPileNum;

				// �̹� �������� �����ϴ� ��쿡�� �׾��ش�.
				const GLInventory::CELL_MAP &ItemMap = *( pInventory->GetItemList() );
				GLInventory::CELL_MAP_CITER iter = ItemMap.begin();
				for ( ; iter!=ItemMap.end(); ++iter )
				{
					SINVENITEM &sINVENITEM = *(*iter).second;
					SITEMCUSTOM &sITEMCUSTOM = sINVENITEM.sItemCustom;

					if ( sITEMCUSTOM.GetNativeID() != sDB_ITEM.GetNativeID() )
						continue;

					if ( sITEMCUSTOM.wTurnNum >= wPILENUM )
						continue;

					WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;
					if ( wREQINSRTNUM > wSURPLUSNUM )
					{
						sITEMCUSTOM.wTurnNum = wPILENUM;
						wREQINSRTNUM -= wSURPLUSNUM;
					}
					else
					{
						sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

						if ( false == bTest )
						{
							SITEMCUSTOM sLog( sNewItemID );
							m_pServer->GetItemLimit()->LogItemExchange(
								sLog,
								gamelog::ID_NONE, 0,
								gamelog::ID_CHAR, ChaDbNum,
								ITEM_ROUTE_SYSTEM, dwNewItemCount, 
								MapID( SNATIVEID( false ),SNATIVEID( false ) ), LOG_ITEM_ADD_ITEM_BY_LUA,
								false );
						}

						return eSuccess;
					}
				}

				if ( 0 == wPILENUM )
					return ePileNumZero;

				WORD wONENUM = wREQINSRTNUM / wPILENUM;
				WORD wITEMNUM = wONENUM;

				WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
				if ( wSPLITNUM > 0 )				wITEMNUM += 1;
				if ( wSPLITNUM==0 && wITEMNUM>=1 )	wSPLITNUM = wPILENUM;

				for ( WORD i=0; i<wITEMNUM; ++i )
				{
					WORD wInsertPosX(0), wInsertPosY(0);
					BOOL bITEM_SPACE = pInventory->FindInsertable( wInsertPosX, wInsertPosY );

					if ( bITEM_SPACE == FALSE )
					{
						return eInventorySpace;
					}

					SITEMCUSTOM sITEMCUSTOM( sDB_ITEM.GetNativeID() );
					sITEMCUSTOM.tBORNTIME = sDB_ITEM.tBORNTIME;

					sITEMCUSTOM.cGenType = sDB_ITEM.cGenType;
					sITEMCUSTOM.cChnID = sDB_ITEM.cChnID;
					sITEMCUSTOM.cFieldID = sDB_ITEM.cFieldID;

					if ( wITEMNUM==(i+1) )	sITEMCUSTOM.wTurnNum = wSPLITNUM;
					else					sITEMCUSTOM.wTurnNum = wPILENUM;

					BOOL bOK = pInventory->InsertItem( sITEMCUSTOM, wInsertPosX, wInsertPosY );
					if ( bOK == FALSE )
						return eInsertFail;
				}
				
				if ( false == bTest )
				{
					SITEMCUSTOM sLog( sNewItemID );
					m_pServer->GetItemLimit()->LogItemExchange(
						sLog,
						gamelog::ID_NONE, 0,
						gamelog::ID_CHAR, ChaDbNum,
						ITEM_ROUTE_SYSTEM, dwNewItemCount, 
						MapID( SNATIVEID( false ),SNATIVEID( false ) ), LOG_ITEM_ADD_ITEM_BY_LUA,
						false );
				}
			}
		}
		else
		{
			sc::RandomInt cRandomInt;
			for( WORD i=0; i<wNums; ++i )
			{
				WORD wInsertPosX(0), wInsertPosY(0);
				bITEM_SPACE = pInventory->FindInsertable( wInsertPosX, wInsertPosY );

				if ( !bITEM_SPACE )
					return eInventorySpace;

				SNATIVEID sNID = sNewItemID;

				SITEMCUSTOM sITEM_NEW( sNID );
				CTime cTIME = CTime::GetCurrentTime();
				sITEM_NEW.tBORNTIME = cTIME.GetTime();
				sITEM_NEW.wTurnNum = 1;
				sITEM_NEW.cGenType = EMGEN_SYSTEM;
				sITEM_NEW.cChnID   = 0;
				sITEM_NEW.cFieldID = 0;
				sITEM_NEW.lnGenNum = cRandomInt.Int64();

				BOOL bOK = pInventory->InsertItem( sITEM_NEW, wInsertPosX, wInsertPosY );
				if ( bOK == FALSE )
					return eInsertFail;
			}

			if ( false == bTest )
			{
				SITEMCUSTOM sLog( sNewItemID );
				m_pServer->GetItemLimit()->LogItemExchange(
					sLog,
					gamelog::ID_NONE, 0,
					gamelog::ID_CHAR, ChaDbNum,
					ITEM_ROUTE_SYSTEM, dwNewItemCount, 
					MapID( SNATIVEID( false ),SNATIVEID( false ) ), LOG_ITEM_ADD_ITEM_BY_LUA,
					false );
			}
		}
		return eSuccess;*/
	}

	bool GLItemValidation::checkRandomOption( const SITEMCUSTOM& sItemCustom )
	{
		return false;

		/*
		if ( FALSE == sItemCustom.IsSetRandOpt() )
			return false;
		for ( DWORD loop = 0; loop < 4; loop++ )
		{
			if ( isOverStat( sItemCustom.getOptType( loop ), sItemCustom.randomOption.getValue( loop ) ) )
			{
				sc::writeLogInfo( sc::string::format( "GLItemValidation::checkRandomOption() isOverStat type %1% value %2%", sItemCustom.getOptType( loop ), sItemCustom.randomOption.getValue( loop ) ) );
				return true;
			}
		}
		return false;*/
	}

	bool GLItemValidation::isOverStat( const RandomOption::RANDOM_OPTION& RandomType, const short& RandomValue )
	{
		return false;

		/*
		bool IsOverStat = false;
		switch ( RandomType )
		{
		case RandomOption::RANDOM_OPTION_RATE_DAMAGE:
			{
				if ( RandomValue >= 1221 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_RATE_DEFENCE:
			{
				if ( RandomValue >= 1221 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_RATE_HIT:
			{
				if ( RandomValue >= 501 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_RATE_AVOID:
			{
				if ( RandomValue >= 501 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_MP:
			{
				if ( RandomValue >= 170 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_SP:
			{
				if ( RandomValue >= 140 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_RESIST:
			{
				if ( RandomValue >= 16 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_DAMAGE:
			{
				if ( RandomValue >= 236 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_DEFENCE:
			{
				if ( RandomValue >= 160 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_HIT:
			{
				if ( RandomValue >= 55 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_AVOID:
			{
				if ( RandomValue >= 55 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_STR:
			{
				if ( RandomValue >= 600 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_MELEE:
			{
				if ( RandomValue >= 262 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_RANGE:
			{
				if ( RandomValue >= 262 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_MAGIC:
			{
				if ( RandomValue >= 131 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_POTION_HP:
			{
				if ( RandomValue >= 560 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_POTION_MP:
			{
				if ( RandomValue >= 130 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_POTION_SP:
			{
				if ( RandomValue >= 130 )
				{
					IsOverStat = true;
				}
			}
			break;
		case RandomOption::RANDOM_OPTION_VALUE_KILL_CP:
			{
				if ( RandomValue >= 15 )
				{
					IsOverStat = true;
				}
			}
			break;
		default:
			break;
		}
		return IsOverStat;*/
	}

	bool GLItemValidation::loadCommon()
	{
		try
		{        
			LuaPlus::LuaObject common = sc::lua_getGlobalFromName( "common" );
			for (LuaPlus::LuaTableIterator Iter( common ); Iter; Iter.Next() )
			{
				std::string strKey( ( const char* ) Iter.GetKey().GetString() );

				if ( strKey == "ItemExchange" )
				{
					for ( LuaPlus::LuaTableIterator Iter2( Iter.GetValue() ); Iter2; Iter2.Next() )
					{
						for ( LuaPlus::LuaTableIterator Iter3( Iter2.GetValue() ); Iter3; Iter3.Next() )
						{
							ItemExchange sExchange;
							sExchange.sOldItemID.wMainID = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
							sExchange.sOldItemID.wSubID  = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
							sExchange.sNewItemID.wMainID = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
							sExchange.sNewItemID.wSubID  = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
							sExchange.OldItemRatio = static_cast< DWORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
							sExchange.NewItemRatio = static_cast< DWORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
							sExchange.Remainder = static_cast< DWORD >( Iter3.GetValue().GetInteger() );

							m_ItemExchange.push_back( sExchange );

							std::string strMessage( 
								sc::string::format( 
								"GLItemValidation::loadCommon() %1% %2% %3% %4% %5% %6% %7%", 
								sExchange.sOldItemID.wMainID, 
								sExchange.sOldItemID.wSubID, 
								sExchange.sNewItemID.wMainID, 
								sExchange.sNewItemID.wSubID, 
								sExchange.OldItemRatio, 
								sExchange.NewItemRatio, 
								sExchange.Remainder ) );
							sc::writeLogInfo( strMessage );
						}
					}
				}

				if ( strKey == "ItemIDChange" )
				{
					for ( LuaPlus::LuaTableIterator Iter2( Iter.GetValue() ); Iter2; Iter2.Next() )
					{
						for ( LuaPlus::LuaTableIterator Iter3( Iter2.GetValue() ); Iter3; Iter3.Next() )
						{
							ItemIDChange sChange;
							sChange.sOldItemID.wMainID = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
							sChange.sOldItemID.wSubID  = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
							sChange.sNewItemID.wMainID = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
							sChange.sNewItemID.wSubID  = static_cast< WORD >( Iter3.GetValue().GetInteger() );

							m_ItemIDChange.push_back( sChange );

							std::string strMessage( 
								sc::string::format( 
								"GLItemValidation::loadCommon() %1% %2% %3% %4%", 
								sChange.sOldItemID.wMainID, 
								sChange.sOldItemID.wSubID, 
								sChange.sNewItemID.wMainID, 
								sChange.sNewItemID.wSubID ) );
							sc::writeLogInfo( strMessage );
						}
					}
				}
			}
			return true;
		}
		catch ( const LuaPlus::LuaException& e )
		{
			sc::ErrorMessageBox( sc::string::format( "%1%, %2%", m_strFilePath, e.GetErrorMessage()), false );
			return false;
		}
	}

	bool GLItemValidation::loadServer()
	{
		return true;
	}

	void GLItemValidation::reportError( DWORD ChaDbNum, int nResult )
	{
		switch ( nResult )
		{
		case eNothingConvert:
			sc::writeLogInfo( sc::string::format( "GLItemValidation eNothingConvert chanum %1%", ChaDbNum ) );
			break;
		case eInvalidItemData:
			sc::writeLogError( sc::string::format( "GLItemValidation eInvalidItemData chanum %1%", ChaDbNum ) );
			break;
		case eCharacterNull:
			sc::writeLogError( sc::string::format( "GLItemValidation eCharacterNull chanum %1%", ChaDbNum ) );
			break;
		case eInventoryNull:
			sc::writeLogError( sc::string::format( "GLItemValidation eInventoryNull chanum %1%", ChaDbNum ) );
			break;
		case eInventorySpace:
			sc::writeLogError( sc::string::format( "GLItemValidation eInventorySpace chanum %1%", ChaDbNum ) );
			break;
		case ePileNumZero:
			sc::writeLogError( sc::string::format( "GLItemValidation ePileNumZero chanum %1%", ChaDbNum ) );
			break;
		case eInsertFail:
			sc::writeLogError( sc::string::format( "GLItemValidation eInsertFail chanum %1%", ChaDbNum ) );
			break;
		case eGaeaServerNull:
			sc::writeLogError( sc::string::format( "GLItemValidation eGaeaServerNull chanum %1%", ChaDbNum ) );
			break;
		}
	}
}