#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/gassert.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/Common/SUBPATH.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogicClient/Char/GLCharacter.h"
#include "../GLGaeaClient.h"
#include "./GLActivityClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------//
GLActivityClient::GLActivityClient( GLGaeaClient* pGaeaClient )
:m_pGaeaClient(pGaeaClient)
{
}

GLActivityClient::~GLActivityClient()
{
}

const DWORD GLActivityClient::GetActivityCompletedCount( ENUM_ACTIVITY_CLASS emClass )
{
	DWORD dwCount = ActivityBaseData::GetInstance()->GetComplateCount( emClass );
	return dwCount;
}

const LONGLONG GLActivityClient::GetActivityPoint( ENUM_ACTIVITY_CLASS emClass )
{
	if( ACTIVITY_CLASS_SIZE < emClass )
		return 0;

	LONGLONG llPoint = m_pGaeaClient->GetCharacter()->m_llActivityPoint[emClass];
	return llPoint;
}

LuaTable GLActivityClient::GetActivityList( UI_ACTIVITY_PAGE emPage, ENUM_ACTIVITY_CLASS emGrade, 
											UI_ACTIVITY_CATEGORY emCategory )
{
	LuaTable tbActivityList( GLWidgetScript::GetInstance().GetLuaState() );

	MMACTIVITIES& mmActivity = ActivityBaseData::GetInstance()->GetData();
	ITER_MMACTIVITIES pos = mmActivity.begin();
	ITER_MMACTIVITIES pos_end = mmActivity.end();

	UINT nActivity = 1;
	for( ; pos != pos_end; ++pos ) 
	{
		LuaTable tbActivity( GLWidgetScript::GetInstance().GetLuaState() );
		Activity& cActivity = pos->second;

		if( FALSE == cActivity.GetValidate() )
			continue;

		if( emGrade != cActivity.m_nClass ) 
			continue;

		if( UI_ACTIVITY_CATEGORY_ALL != emCategory &&
			cActivity.GetType() != ConvertCategory( emCategory ) )
			continue;

		std::string strProgress = ID2GAMEWORD ( "ACTIVITY_STATUS_TEXT", 0 );
		std::string strComplete = "";

		if( UI_ACTIVITY_PAGE_ALL != emPage )
		{
			ENUM_ACTIVITY_STATUS emStatus = ACTIVITY_STATUS_INPROGRESS;
			switch( emPage ) 
			{
			case UI_ACTIVITY_PAGE_PROG :
				emStatus = ACTIVITY_STATUS_INPROGRESS;
				break;
			case UI_ACTIVITY_PAGE_COMP :
				emStatus = ACTIVITY_STATUS_COMPLETED;
				break;
			case UI_ACTIVITY_PAGE_NOTCOMP :
				emStatus = ACTIVITY_STATUS_CLOSED;
				break;
			};

			if( cActivity.GetStatus() != emStatus )
				continue;

			if( emStatus == ACTIVITY_STATUS_INPROGRESS && !cActivity.GetValue() )
				continue;
		}

		ENUM_ACTIVITY_SLOT_TYPE emSlotType = ENUM_ACTIVITY_SLOT_TYPE_IDLE;
		int nValue = 0;

		switch( cActivity.GetStatus() ) 
		{
		case ACTIVITY_STATUS_INPROGRESS :
			{
				//  레벨 달성일 경우
				if( ACTIVITY_MID_LEVEL == cActivity.m_id.wMainID )
				{
					emSlotType = ENUM_ACTIVITY_SLOT_TYPE_CONTINUE;
					nValue = m_pGaeaClient->GetCharacterLogic().m_wLevel;
				}
				else if( cActivity.GetValue() )
				{
					emSlotType = ENUM_ACTIVITY_SLOT_TYPE_CONTINUE;
					nValue = cActivity.GetValue();
				}
			}
			break;

		case ACTIVITY_STATUS_COMPLETED :
		case ACTIVITY_STATUS_ORDERING :
			{
				emSlotType = ENUM_ACTIVITY_SLOT_TYPE_COMP;
			}
			break;

		case ACTIVITY_STATUS_CLOSED :
			{
				emSlotType = ENUM_ACTIVITY_SLOT_TYPE_NOTCOMP;
			}
			break;
		}

		tbActivity.set( 1, (WORD) emSlotType );
		tbActivity.set( 2, cActivity.m_nByOrder );
		tbActivity.set( 3, cActivity.m_id.wMainID );
		tbActivity.set( 4, cActivity.GetTitle().c_str() );
		tbActivity.set( 5, cActivity.GetDesc().c_str() );
		tbActivity.set( 6, cActivity.GetRewardTitle().c_str() );
		tbActivity.set( 7, cActivity.GetRewardPoint() );
		tbActivity.set( 8, nValue );

		tbActivityList.set( nActivity++, tbActivity );
	}

	return tbActivityList;
}

LuaTable GLActivityClient::GetActivitySalesItemList( ENUM_ACTIVITY_CLASS emGrade )
{
	LuaTable tbSalesItemList( GLWidgetScript::GetInstance().GetLuaState() );

	// 아이템 리스트를 설정
	MMACTIVITYSALESITEM& mmActivitySalesItem = ActivityBaseData::GetInstance()->GetSalesItemData();
	//m_pItemList->ClearList ();

	RANGE_MMACTIVITYSALESITEM Range = mmActivitySalesItem.equal_range( emGrade );

	int nSalesItem = 1;
	for ( ITER_MMACTIVITYSALESITEM iter = Range.first; iter != Range.second; ++iter )
	{
		SACTIVITYSALESITEM* pSaleData = &iter->second;
		const GLInventory::CELL_MAP *pItemList = pSaleData->pITEMS->GetItemList();
		{	
			GLInventory::CELL_MAP_CITER _iter     = pItemList->begin();
			GLInventory::CELL_MAP_CITER _iter_end = pItemList->end();

			for ( ; _iter!=_iter_end; ++_iter )
			{
				LuaTable tbSalesItem( GLWidgetScript::GetInstance().GetLuaState() );
				SINVENITEM* pInvenItem  = (*_iter).second;
				DWORD       dwItemID    = pInvenItem->Id();
				LONGLONG	llSellPrice = 0;

				const SITEM* pItemData = GLogicData::GetInstance().GetItem ( dwItemID );

				if ( !pItemData )
				{
					continue;
				}

				llSellPrice = (*pSaleData->pSELLPRICEMAP)[dwItemID];

				if( llSellPrice == 0 )
				{
					llSellPrice = (LONGLONG)pItemData->sBasicOp.dwBuyPrice;
				}

				// 요구 포인트 설정
				ENUM_ACTIVITY_CLASS emActivityClass =  ENUM_ACTIVITY_CLASS(pItemData->sBasicOp.emReqActivityType);

				tbSalesItem.set( 1, pItemData->sBasicOp.sNativeID.dwID );
				tbSalesItem.set( 2, pInvenItem->sItemCustom.GETNAME().c_str() );
				tbSalesItem.set( 3, emActivityClass );
				tbSalesItem.set( 4, pItemData->sBasicOp.wReqActPointDW );
				tbSalesItem.set( 5, llSellPrice );

				tbSalesItemList.set( nSalesItem, tbSalesItem );
				++nSalesItem;
			}
		}
	}

	return tbSalesItemList;
}

ENUM_ACTIVITY_CATEGORY GLActivityClient::ConvertCategory( UI_ACTIVITY_CATEGORY emCategory )
{
	switch ( emCategory )
	{
	case UI_ACTIVITY_CATEGORY_ALL:
		return ACTIVITY_CATEGORY_SIZE;
	case UI_ACTIVITY_CATEGORY_LEVEL:
		return ACTIVITY_CATEGORY_LEVEL;
	case UI_ACTIVITY_CATEGORY_KILL:
		return ACTIVITY_CATEGORY_KILL;
	case UI_ACTIVITY_CATEGORY_TAKE:
		return ACTIVITY_CATEGORY_TAKE;
	case UI_ACTIVITY_CATEGORY_MAP:
		return ACTIVITY_CATEGORY_MAP;
	case UI_ACTIVITY_CATEGORY_USE:
		return ACTIVITY_CATEGORY_USEITEM;
	}

	return ACTIVITY_CATEGORY_SIZE;
}
