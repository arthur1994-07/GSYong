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

	// 유효한 파티인지 검사;
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
		// 같은 맵인가?;
		if (ItemCustom.m_mapID != m_pGaeaClient->GetActiveMapID())
			continue;

		const float fLimitRange = PartyID.isValidExpedition() ? ::pow(GLCONST_CHAR::fExpeditionTenderRange + 20.0f, 2.0f) : ::pow(GLCONST_CHAR::fPartyTenderRange + 20.0f, 2.0f);
		const float fDistance = ::D3DXVec3LengthSq(&D3DXVECTOR3(ItemCustom.m_vGeneratePosition - pCharacter->GetPosition()));

		// 이걸 여기서 해줄 필요가있을까?;
		// 매번 업데이트 돌아서 체크하는게 더 아깝긴하다;
		if ( fDistance > fLimitRange )
		{
			if ( ItemCustom.m_bShow == true )
			{
				ItemCustom.m_bShow = false;
				ItemCustom.m_bSelected = false;
				m_bNeedRefresh = true;
			}

			m_ItemCustomMap.erase(iter);		// 거리에 맞지 않는다변 배열에서 제거하자.
			vecRmoveItemCustomKey.push_back( m_ItemCustomVector[i] );

			continue; // 반경 유효성 검사;	
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
		tbItem.set( "name", ItemCustom.m_ItemCustom.GETNAME() );					// 아이템 이름;
		tbItem.set( "turnNum", ItemCustom.m_ItemCustom.wTurnNum );					// 갯수;
		tbItem.set( "percent", ItemCustom.m_fPercent );								// 진행률;
		tbItem.set( "tenderTime", PartyID.isValidExpedition() == true ?
			GLCONST_CHAR::fExpeditionTenderTime : GLCONST_CHAR::fPartyTenderTime );		// 전체 시간;
		tbItem.set( "select", ItemCustom.m_bSelected );								// 선택 여부;
		tbItem.set( "state", ItemCustom.m_emTenderState );							// 입찰? 불가능? 대기?;
		
		// 아래 데이터들로 문자열이 구성될때에는 ItemTooltip.lua의 함수를 통해 구성이된다;
		SITEM* pItem = GLItemMan::GetInstance().GetItem(ItemCustom.m_ItemCustom.Id());
		GLItemTooltip::GetInstance().GenSchool(pItem->sBasicOp.dwReqSchool, tbItem);	// 학원 제한 정보;
		GLItemTooltip::GetInstance().GenClass(pItem->sBasicOp.dwReqCharClass, tbItem);	// 클래스 제한 정보;
		GLItemTooltip::GetInstance().GenSex(pItem->sBasicOp.dwReqCharClass, tbItem);	// 성별 제한 정보;
		
		tbItem.set( "namecolor", ITEM_INFOR_COLOR::dwItemRank[pItem->sBasicOp.emLevel ] );


		LuaTable tbLevel(GLWidgetScript::GetInstance().GetLuaState());
		{
			tbLevel.set( "LevelDW", pItem->sBasicOp.wReqLevelDW );
			tbLevel.set( "LevelUP", pItem->sBasicOp.wReqLevelUP );
		}
		tbItem.set( "Level", tbLevel );	// 레벨 제한 정보;

		// 요구/현재 능력치 기입;
		LuaTable tbConditionStatus(GLWidgetScript::GetInstance().GetLuaState());
		LuaTable tbCharacterStatus(GLWidgetScript::GetInstance().GetLuaState());
		{
			const GLCharacter* const pChar = m_pGaeaClient->GetCharacter();
			// 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014);
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

	// 거리에 맞지 않는 커스텀 아이템을 리스트에서 제거 
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