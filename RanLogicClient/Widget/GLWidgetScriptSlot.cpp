#include "pch.h"
#include "GLWidgetScript.h"

#include "../../RanGfxUI/GfxInterface.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"

void GLWidgetScript::ResetSlot( const char* strSlotID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->ResetSlot( strSlotID );
}

void GLWidgetScript::SetSlot_Item( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetItemSlotData( strSlotID, wMID, wSID, wCount, wRealMID, wRealSID );
}

void GLWidgetScript::SetSlot_Item_NonStrengthen ( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetItemSlotData_NonStrengthen( strSlotID, wMID, wSID, wCount, wRealMID, wRealSID );
}

void GLWidgetScript::SetSlot_ItemBornT( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, DWORD dwBornT )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetItemSlotData_BORNTIME( strSlotID, wMID, wSID, wCount, dwBornT );
}

void GLWidgetScript::SetSlot_Item_DWORD( const char* strSlotID, DWORD dwID, WORD wCount, DWORD dwRealID )
{
	NULL_INTERFACE_RETURN;
	SNATIVEID snativeid(dwID);
	SNATIVEID sRealItemID( dwRealID );
	GLWidgetScript::GetInstance().m_pInterface->SetItemSlotData( strSlotID, snativeid.Mid(), snativeid.Sid(), wCount, sRealItemID.Mid(), sRealItemID.Sid() );
}

void GLWidgetScript::SetSlot_Skill( const char* strSlotID, DWORD dwID )
{
	NULL_INTERFACE_RETURN;

	SNATIVEID sID( dwID );
	GLWidgetScript::GetInstance().m_pInterface->SetSkillSlotData( strSlotID, sID.wMainID, sID.wSubID );
}

void GLWidgetScript::SetSlot_SkillLevel( const char* strSlotID, DWORD dwID, WORD wLevel )
{
	NULL_INTERFACE_RETURN;

	SNATIVEID sID( dwID );
	GLWidgetScript::GetInstance().m_pInterface->SetSkillSlotData( strSlotID, sID.wMainID, sID.wSubID, wLevel );
}

void GLWidgetScript::SetSlot_Inventory( const char* strSlotID, int nPosX, int nPosY )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetInventorySlotData( strSlotID, nPosX, nPosY );
}

void GLWidgetScript::SetSlot_Rebuy( const char* strSlotID, int nRebuyID, WORD wMID, WORD wSID	)
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetRebuySlotData( strSlotID, nRebuyID, wMID, wSID );
}

void GLWidgetScript::SetSlot_SummonState( const char* strSlotID, const char* strIconName, INT nIconIdxX, INT nIconIdxY )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetSummonStateSlotData( strSlotID, strIconName, nIconIdxX, nIconIdxY );
}

void GLWidgetScript::SetSlot_Bank( const char* strSlotID, int nPosX, int nPosY )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetBankSlotData( strSlotID, nPosX, nPosY );
}

void GLWidgetScript::SetSlot_Wishlist( const char* strSlotID, int nPosX, int nPosY )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetWishlistSlotData( strSlotID, nPosX, nPosY );
}

void GLWidgetScript::SetSlot_CharInfo( const char* strSlotID, int nSlot )
{
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetCharInfoSlotData( strSlotID, 
		GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter()->m_PutOnItems[ EMSLOT( nSlot ) ], nSlot );
}

void GLWidgetScript::SetSlot_SQS( const char* strSlotID, DWORD dwID, int nIndex )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetSkillQuickSlotData( strSlotID, dwID, nIndex );
}

void GLWidgetScript::SetSlot_IQS( const char* strSlotID, DWORD dwID, int nIndex )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetItemQuickSlotData( strSlotID, dwID, nIndex );
}
void GLWidgetScript::SetSlot_MiniGame_Dice( const char* strSlotID, DWORD dwID, int nIndex)
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetItemMiniGameDiceRewardSlotData( strSlotID, dwID, nIndex );
}

void GLWidgetScript::SetSlot_NpcMarket( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetNpcMarketSlotData( strSlotID, wMID, wSID, wCount, nPosX, nPosY );
}

void GLWidgetScript::SetSlot_NpcMarketCart( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wIndex )
{
	NULL_INTERFACE_RETURN;

	SITEMCUSTOM sItem( SNATIVEID( wMID, wSID ) );
	sItem.wTurnNum = wCount;

	GLWidgetScript::GetInstance().m_pInterface->SetNpcMarketCartSlotData( strSlotID, sItem, wIndex );
}

void GLWidgetScript::SetSlot_Trade(const char* strSlotID, int nPosX, int nPosY, bool _MyBox)
{
	NULL_INTERFACE_RETURN
	GLWidgetScript::GetInstance().m_pInterface->SetTradeSlotData(strSlotID, nPosX, nPosY, _MyBox);
}

void GLWidgetScript::SetSlot_PointShop( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetPointShopSlotData( strSlotID, wMID, wSID, wCount, nPosX, nPosY );
}

void GLWidgetScript::SetSlot_PointShopCart( const char* strSlotID, WORD wMID, WORD wSID, int nIndex )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetPointShopCartSlotData( strSlotID, wMID, wSID, nIndex );
}

void GLWidgetScript::SetSlot_ReferChar( const char* strSlotID, int nSlot )
{
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetReferSlotData( strSlotID, nSlot );
}

void GLWidgetScript::SetSlot_PMSPointTrade( const char* strSlotID, WORD wPosX, WORD wPosY )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetPMSPointTradeSlotData( strSlotID, wPosX, wPosY );
}

void GLWidgetScript::SetSlot_PMSPointTrade_Index( const char* strSlotID, DWORD dwIndex, DWORD dwCount )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetPMSPointTradeSlotData_Index( strSlotID, dwIndex, dwCount );
}

void GLWidgetScript::SetSlot_PrivateMarket( const char* strSlotID, WORD nPosX, WORD nPosY, bool bSelf )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetPrivateMarketSlotData( strSlotID, nPosX, nPosY, bSelf );
}

void GLWidgetScript::SetSlot_PrivateMarketSearch( const char* strSlotID, DWORD dwIndex, WORD wCount, bool bSearchPage )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetPrivateMarketSearchSlotData( strSlotID, dwIndex, wCount, bSearchPage );
}

void GLWidgetScript::SetSlot_ReceivePost( const char* strSlotID, DWORD dwIndex, DWORD dwCount )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetReceivePostSlotData( strSlotID, dwIndex, dwCount );
}

void GLWidgetScript::SetSlot_PostBoxSub_PreviewPageOpen( const char* strSlotID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetSlot_PostBoxSub_PreviewPageOpen( strSlotID );
}

void GLWidgetScript::SetSlot_Pet( const char* strSlotID, WORD wMID, WORD wSID, WORD nSlot )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetPetItemSlotData( strSlotID, wMID, wSID, nSlot );
}

void GLWidgetScript::SetSlot_ClubStorage( const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetClubStorageSlotData(strSlotID, Channel, nPosX, nPosY);
}

void GLWidgetScript::SetClubStorage_Item( const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetClubStorageItem(strSlotID, Channel, nPosX, nPosY);
}

void GLWidgetScript::SetSlot_Locker( const char* strSlotID, int nIndex, int nPosX, int nPosY )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetLockerSlot( strSlotID, nIndex, nPosX, nPosY );
}

void GLWidgetScript::SetSlot_RebuildCard( const char* strSlotID, int nPosX, int nPosY )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetRebuildCardSlot( strSlotID, nPosX, nPosY );
}

void GLWidgetScript::SetSlot_PartyDistribution(const char* strSlotID, DWORD dwItemID, WORD wSlotIndex)
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetItemSlotData_PartyDistribution( strSlotID, dwItemID, wSlotIndex );
}

void GLWidgetScript::SetSlot_ItemRepair( const char* strSlotID, int nSlot )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetItemRepairSlotData( strSlotID, nSlot );
}

void GLWidgetScript::SetSlot_LottoWinItem ( const char* strSlotID, DWORD dwLottoSystemID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetLottoWinItemSlotData( strSlotID, dwLottoSystemID );
}

void GLWidgetScript::SetSlot_PreWinListItem ( const char* strSlotID, DWORD dwLottoSystemID, int nRanking )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetLottoPreWinListItemSlotData( strSlotID, dwLottoSystemID, nRanking );
}

void GLWidgetScript::SetSlot_RanMobileReward( const char* strSlotID, int nIndex )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetRanMobileRewardItemSlotData( strSlotID, nIndex );
}