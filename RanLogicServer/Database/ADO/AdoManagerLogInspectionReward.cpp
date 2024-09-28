#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "./AdoManager.h"

#include "../../SigmaCore/DebugInclude.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/DateTime.h"

#include "../../../RanLogic/Character/GLCharData.h"
#include "../../../RanLogic/Inventory/GLInventory.h"

namespace db
{

	int AdoManager::InspectionRewardPolicySelect()
	{
		/*
		[StartDate]
		,[EndDate]
		,[RewardMinute]
		*/

		sc::db::AdoExt Ado(m_LogDBConnString);

		if (!Ado.Execute4Cmd("dbo.InspectionRewardPolicySelect", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		do
		{   
			_variant_t StartDate,EndDate;
			int nRewordMinute=0;

			__time64_t tStartTime,tEndTime;
			tStartTime=tEndTime=0;


			Ado.GetCollect("StartDate",		StartDate);
			Ado.GetCollect("EndDate",		EndDate);       
			Ado.GetCollect("RewardMinute",  nRewordMinute);

			if( StartDate.GetVARIANT().vt == VT_DATE )
				tStartTime = sc::time::GetTime(StartDate);

			if( EndDate.GetVARIANT().vt == VT_DATE )
				tEndTime = sc::time::GetTime(EndDate);

		} while (Ado.Next());

		return sc::db::DB_OK;

	}

	int AdoManager::InspectionRewardPolicyInsert( const __time64_t tStart, const __time64_t tEnd, const int nRewordMinute )
	{
		/*
			@StartDate date,
			@EndDate date,
			@RewardMinute int,
			@nReturn int OUTPUT
		*/
		sc::db::AdoExt Ado(m_LogDBConnString);

		std::string StrStartDate = sc::time::DateTimeFormat(tStart, true, true);
		std::string StrEndDate = sc::time::DateTimeFormat(tEnd, true, true);
	
		APPEND_IPARAM_VARCHAR(Ado, "@StartDate", StrStartDate.c_str(), static_cast<long> (StrStartDate.length()));
		APPEND_IPARAM_VARCHAR(Ado, "@EndDate", StrEndDate.c_str(), static_cast<long> (StrEndDate.length()));
		APPEND_IPARAM_INT	 (Ado, "@RewardMinute", nRewordMinute);
		APPEND_OPARAM_BIGINT (Ado, "@nReturn");

		return ExecuteStoredProcedure("dbo.InspectionRewardPolicyInsert", Ado);
	}

	int AdoManager::InspectionRewardGetMinute( SCHARDATA2* pChaData2 )
	{
		/*
			@ChaNum int,
		*/

		if (pChaData2 == NULL)
			return sc::db::DB_ERROR;

		sc::db::AdoExt Ado(m_LogDBConnString);

		APPEND_IPARAM_INT( Ado, "@ChaNum", pChaData2->CharDbNum() );
		Ado.Execute4Cmd( "dbo.InspectionRewardGetMinute", adCmdStoredProc );

		int nRewordMunute=0;

		Ado.GetCollect("RewardMinute", nRewordMunute);

		// 보상 시간이 있을 때만 작업한다.
		if (nRewordMunute <= 0)
			return sc::db::DB_OK;

		// 시간 보상 시작
		std::vector<SITEMCUSTOM*> vecItemList;


// 		// 창고의 모든 아이템
// 		for (int i=0 ; i<EMSTORAGE_CHANNEL ; i++)
// 		{
// 			GLInventory::CELL_MAP* mapList = const_cast<GLInventory::CELL_MAP*>( pChaData2->m_cInventory.GetItemList() );
// 
// 			if ( mapList == NULL )
// 				continue;
// 
// 			BOOST_FOREACH ( GLInventory::CELL_MAP::value_type& rValue, *mapList  )
// 			{
// 				SINVENITEM* pInvenItem = rValue.second;
// 
// 				if (pInvenItem==NULL)
// 					continue;
// 
// 				vecItemList.push_back( &pInvenItem->sItemCustom );
// 			}
// 				
// 		}
		
		// 인벤의 모든 아이템
		GLInventory::CELL_MAP* mapList = const_cast<GLInventory::CELL_MAP*>( pChaData2->m_cInventory.GetItemList() );
		if ( mapList )
		{

			BOOST_FOREACH ( GLInventory::CELL_MAP::value_type& rValue, *mapList  )
			{
				SINVENITEM* pInvenItem = rValue.second;

				if (pInvenItem==NULL)
					continue;

				vecItemList.push_back( &pInvenItem->sItemCustom );
			}
		}


		// 들고있는 아이템
		for (int i=0 ; i<SLOT_TSIZE ; i++)
		{
			if (pChaData2->m_PutOnItems[i].GetNativeID()==NATIVEID_NULL())
				continue;

			vecItemList.push_back( &pChaData2->m_PutOnItems[i] );
		}


		CTime cTime(0);
		cTime += CTimeSpan(0, 0, nRewordMunute, 0);
		// 모든 시효성, 코스튬 아이템 시간 증가
		BOOST_FOREACH ( SITEMCUSTOM* pItemCustom, vecItemList )
		{

			
			if ( pItemCustom == NULL )
				continue;

			if ( pItemCustom->GetNativeID()==NATIVEID_NULL() )
				continue;

			// 영구 코스튬 시간값 이상하게 되지 않도록 조건 추가, 이 부분에서 시간이 이상하게 늘어서
			// GLChar::RESET_TIMELMT_ITEM() - Invalid CTime is set for an item 에러 메시지 발생
			if ( pItemCustom->nidDISGUISE != NATIVEID_NULL() && pItemCustom->tDISGUISE != 0 )
				pItemCustom->tDISGUISE += cTime.GetTime();

			if ( pItemCustom->IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
				pItemCustom->tBORNTIME += cTime.GetTime();
		}
		
		return sc::db::DB_OK;
	}


} // namespace db
