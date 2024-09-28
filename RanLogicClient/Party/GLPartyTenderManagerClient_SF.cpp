#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/Msg/GLContrlPartyMsg.h"
#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Char/GLCharacter.h"
#include "../GLGaeaClient.h"

#include "../Tooltip/GLItemTooltip.h"

#include "GLPartyManagerClient.h"
#include "GLPartyTenderManagerClient.h"

// SF
LuaTable GLPartyTenderManagerClient::GetTenderItem(void)
{
	LuaTable tbItemList(GLWidgetScript::GetInstance().GetLuaState());

	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter == NULL )
		return tbItemList;

	// ��ȿ�� ��Ƽ���� �˻�;
	const GLPartyID& PartyID = pCharacter->GetPartyID();
	if ( PartyID.isValidParty() == false )
	{
		Reset();
		return tbItemList;
	}

	std::vector< DWORD > vecRmoveItemCustomKey;

	INT nItemCount(0);
	const INT nItemSize(static_cast<INT>(m_ItemCustomVector.size()));
	for ( INT i = 0; i < nItemSize; ++i )
	{
		ItemCustomMapIter iter = m_ItemCustomMap.find(m_ItemCustomVector[i]);
		if ( iter == m_ItemCustomMap.end() )
			continue;

		GLTenderItemCustomClient& ItemCustom = iter->second;
		// ���� ���ΰ�?;
		if (ItemCustom.m_mapID != m_pGaeaClient->GetActiveMapID())
			continue;

		const float fLimitRange = PartyID.isValidExpedition() ? ::pow(GLCONST_CHAR::fExpeditionTenderRange + 20.0f, 2.0f) : ::pow(GLCONST_CHAR::fPartyTenderRange + 20.0f, 2.0f);
		const float fDistance = ::D3DXVec3LengthSq(&D3DXVECTOR3(ItemCustom.m_vGeneratePosition - pCharacter->GetPosition()));

		// �̰� ���⼭ ���� �ʿ䰡������?;
		// �Ź� ������Ʈ ���Ƽ� üũ�ϴ°� �� �Ʊ����ϴ�;
		if ( fDistance > fLimitRange )
		{
			if ( ItemCustom.m_bShow == true )
			{
				ItemCustom.m_bShow = false;
				ItemCustom.m_bSelected = false;
				m_bNeedRefresh = true;
			}

			m_ItemCustomMap.erase(iter);		// �Ÿ��� ���� �ʴ´ٺ� �迭���� ��������.
			vecRmoveItemCustomKey.push_back( m_ItemCustomVector[i] );

			continue; // �ݰ� ��ȿ�� �˻�;	
		}

		if ( ItemCustom.m_bShow == false )
		{
			ItemCustom.m_bShow = true;
			m_bNeedRefresh = true;

			if( ItemCustom.m_bNew )
				ItemCustom.m_bNew = false;
		}
		LuaTable tbItem(GLWidgetScript::GetInstance().GetLuaState());
		tbItem.set( "NATIVEID", ItemCustom.m_ItemCustom.GetNativeID().Id() );		// ID;
		tbItem.set( "name", ItemCustom.m_ItemCustom.GETNAME() );					// ������ �̸�;
		tbItem.set( "turnNum", ItemCustom.m_ItemCustom.wTurnNum );					// ����;
		tbItem.set( "percent", ItemCustom.m_fPercent );								// �����;
		tbItem.set( "tenderTime", PartyID.isValidExpedition() == true ?
			GLCONST_CHAR::fExpeditionTenderTime : GLCONST_CHAR::fPartyTenderTime );		// ��ü �ð�;
		tbItem.set( "select", ItemCustom.m_bSelected );								// ���� ����;
		tbItem.set( "state", ItemCustom.m_emTenderState );							// ����? �Ұ���? ���?;
		
		// �Ʒ� �����͵�� ���ڿ��� �����ɶ����� ItemTooltip.lua�� �Լ��� ���� �����̵ȴ�;
		SITEM* pItem = GLItemMan::GetInstance().GetItem(ItemCustom.m_ItemCustom.Id());
		GLItemTooltip::GetInstance().GenSchool(pItem->sBasicOp.dwReqSchool, tbItem);	// �п� ���� ����;
		GLItemTooltip::GetInstance().GenClass(pItem->sBasicOp.dwReqCharClass, tbItem);	// Ŭ���� ���� ����;
		GLItemTooltip::GetInstance().GenSex(pItem->sBasicOp.dwReqCharClass, tbItem);	// ���� ���� ����;
		
		tbItem.set( "namecolor", ITEM_INFOR_COLOR::dwItemRank[pItem->sBasicOp.emLevel ] );


		LuaTable tbLevel(GLWidgetScript::GetInstance().GetLuaState());
		{
			tbLevel.set( "LevelDW", pItem->sBasicOp.wReqLevelDW );
			tbLevel.set( "LevelUP", pItem->sBasicOp.wReqLevelUP );
		}
		tbItem.set( "Level", tbLevel );	// ���� ���� ����;

		// �䱸/���� �ɷ�ġ ����;
		LuaTable tbConditionStatus(GLWidgetScript::GetInstance().GetLuaState());
		LuaTable tbCharacterStatus(GLWidgetScript::GetInstance().GetLuaState());
		{
			const GLCharacter* const pChar = m_pGaeaClient->GetCharacter();
			// ��ųȿ���� ��������ȿ���� ������ ������ġ ��꿡 ���Խ�Ű�� ���� ����(CRM#3014);
			const SCHARSTATS& rCharacterStats = pChar->m_sSUMSTATS_SKILL;	
			const SCHARSTATS& rConditionStats = pItem->sBasicOp.sReqStats;
			tbConditionStatus.set(1,rConditionStats.wPow);		tbCharacterStatus.set(1,rCharacterStats.wPow);
			tbConditionStatus.set(2,rConditionStats.wStr);		tbCharacterStatus.set(2,rCharacterStats.wStr);
			tbConditionStatus.set(3,rConditionStats.wSpi);		tbCharacterStatus.set(3,rCharacterStats.wSpi);
			tbConditionStatus.set(4,rConditionStats.wDex);		tbCharacterStatus.set(4,rCharacterStats.wDex);
			tbConditionStatus.set(5,rConditionStats.wInt);		tbCharacterStatus.set(5,rCharacterStats.wInt);
			tbConditionStatus.set(6,rConditionStats.wSta);		tbCharacterStatus.set(6,rCharacterStats.wSta);
			tbItem.set( "conditionStatus", tbConditionStatus );
			tbItem.set( "characterStatus", tbCharacterStatus );
		}

		tbItemList.set(++nItemCount, tbItem);
	}

	// �Ÿ��� ���� �ʴ� Ŀ���� �������� ����Ʈ���� ���� 
	std::vector< DWORD >::iterator vit = vecRmoveItemCustomKey.begin();
	for( vit; vit != vecRmoveItemCustomKey.end(); ++vit )
	{
		DWORD dwCoustom = (*vit);

		std::vector< DWORD >::iterator vitItemCoustom = m_ItemCustomVector.begin();
		for( vitItemCoustom; vitItemCoustom != m_ItemCustomVector.end(); )
		{
			if( (*vitItemCoustom) == dwCoustom )
				m_ItemCustomVector.erase(vitItemCoustom);
			else
				 ++vitItemCoustom;
		}
	}


	return tbItemList;
}

LuaTable GLPartyTenderManagerClient::GetMemberInfo(void)
{
	LuaTable tbPartyMembersInfo(GLWidgetScript::GetInstance().GetLuaState());
	
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	GLPartyClient* pParty = pPartyManager->GetParty(pCharacter->GetPartyID());
	if ( pParty == NULL )
		return tbPartyMembersInfo;

	INT nCount(0);
	for ( DWORD i = 0; i<MAXPARTY; ++i )
	{
		GLPartySlotClient* pMember = pParty->GetMember(i);
		if ( (pMember == NULL) || (pMember->isValid() == false) )
			continue;

		LuaTable tbMemberInfo(GLWidgetScript::GetInstance().GetLuaState());
		tbMemberInfo.set( "class", ID2GAMEWORD("CLASS_NAME_NO_SEX", DWORD(CharClassToClassIndex(pMember->m_emClass)) ) );
		tbMemberInfo.set( "level", pMember->m_Level );
		tbMemberInfo.set( "name", pMember->m_szName );
		tbMemberInfo.set( "select", pMember->isMemberState(EMPARTY_MEMBER_STATE_SELECTED) );
		tbPartyMembersInfo.set(++nCount, tbMemberInfo);
	}
	return tbPartyMembersInfo;
}

void GLPartyTenderManagerClient::SetSelectMember(DWORD dwSelectSlot)
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();

	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	GLPartyClient* pParty = pPartyManager->GetParty(pCharacter->GetPartyID());
	if ( pParty == NULL )
		return;

	GLPartySlotClient* pPartySlot = pParty->GetMember(dwSelectSlot);
	if ( pPartySlot == NULL )
		return;

	pPartySlot->InverseMemberState(EMPARTY_MEMBER_STATE_SELECTED);
}