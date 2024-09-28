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

		// ���� �ð��� ���� ���� �۾��Ѵ�.
		if (nRewordMunute <= 0)
			return sc::db::DB_OK;

		// �ð� ���� ����
		std::vector<SITEMCUSTOM*> vecItemList;


// 		// â���� ��� ������
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
		
		// �κ��� ��� ������
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


		// ����ִ� ������
		for (int i=0 ; i<SLOT_TSIZE ; i++)
		{
			if (pChaData2->m_PutOnItems[i].GetNativeID()==NATIVEID_NULL())
				continue;

			vecItemList.push_back( &pChaData2->m_PutOnItems[i] );
		}


		CTime cTime(0);
		cTime += CTimeSpan(0, 0, nRewordMunute, 0);
		// ��� ��ȿ��, �ڽ�Ƭ ������ �ð� ����
		BOOST_FOREACH ( SITEMCUSTOM* pItemCustom, vecItemList )
		{

			
			if ( pItemCustom == NULL )
				continue;

			if ( pItemCustom->GetNativeID()==NATIVEID_NULL() )
				continue;

			// ���� �ڽ�Ƭ �ð��� �̻��ϰ� ���� �ʵ��� ���� �߰�, �� �κп��� �ð��� �̻��ϰ� �þ
			// GLChar::RESET_TIMELMT_ITEM() - Invalid CTime is set for an item ���� �޽��� �߻�
			if ( pItemCustom->nidDISGUISE != NATIVEID_NULL() && pItemCustom->tDISGUISE != 0 )
				pItemCustom->tDISGUISE += cTime.GetTime();

			if ( pItemCustom->IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
				pItemCustom->tBORNTIME += cTime.GetTime();
		}
		
		return sc::db::DB_OK;
	}


} // namespace db
