#include "pch.h"

#include "../../RanLogic/Item/ItemDurability.h"

#include "./GLCharacter.h"

bool GLCharacter::GetWeaponDurabilityValue(float& fDurability, float& fMaxDurability)
{
	const EMSLOT eRHand = IsUseArmSub() ? SLOT_RHAND_S : SLOT_RHAND;
	SITEMCUSTOM& sItemCustom = m_PutOnItems[(DWORD)eRHand];
	if ( sItemCustom.GetNativeID().IsValidNativeID() == false )
		return false;

	SITEM* pItem = GLItemMan::GetInstance().GetItem(sItemCustom.Id());
	if ( ItemDurability::GetInstance().IsUseWeapon(pItem) == false )
		return false;

	fDurability = static_cast<float>(sItemCustom.wDurability);
	fMaxDurability = static_cast<float>(pItem->sSuitOp.wDurabilityMax);
	return true;
}

void GLCharacter::DurabilityDropHit( unsigned int nDamage, BOOL bPvp )
{	
	float fBeforeWeaponDurability, fCurrentWeaponDurability, fMaxDurability;
	const bool bOk(GetWeaponDurabilityValue(fBeforeWeaponDurability, fMaxDurability));
		
	// super
	GLCHARLOGIC::DurabilityDropHit(nDamage, bPvp);
	if ( bOk == false || GetWeaponDurabilityValue(fCurrentWeaponDurability, fMaxDurability) == false )
		return;

	float fPercent(1.0f-fCurrentWeaponDurability/fMaxDurability);
	if ( fPercent <= GLCONST_CHAR::fDurabilityAlarmRemainPer )
	{
		// 차이가 있는지 확인한다;
		if ( fCurrentWeaponDurability!=fBeforeWeaponDurability )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetWeaponBoostInfo, "-f, -f, -f"
				, GLCONST_CHAR::fDurabilityAlarmRemainPer, fCurrentWeaponDurability, fMaxDurability );
		}
	}
}

void GLCharacter::DurabilityDropUseSkill()
{
	float fBeforeWeaponDurability, fCurrentWeaponDurability, fMaxDurability;
	const bool bOk(GetWeaponDurabilityValue(fBeforeWeaponDurability, fMaxDurability));
	
	// super
	GLCHARLOGIC::DurabilityDropUseSkill();
	if ( bOk == false || GetWeaponDurabilityValue(fCurrentWeaponDurability, fMaxDurability) == false )
		return;

	float fPercent(1.0f-fCurrentWeaponDurability/fMaxDurability);
	if ( fPercent <= GLCONST_CHAR::fDurabilityAlarmRemainPer )
	{
		// 차이가 있는지 확인한다;
		if ( fCurrentWeaponDurability!=fBeforeWeaponDurability )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetWeaponBoostInfo, "-f, -f, -f"
				, GLCONST_CHAR::fDurabilityAlarmRemainPer, fCurrentWeaponDurability, fMaxDurability );
		}
	}
}

void GLCharacter::DurabilityDropDeath( BOOL bPvp )
{
	float fBeforeWeaponDurability, fCurrentWeaponDurability, fMaxDurability;
	const bool bOk(GetWeaponDurabilityValue(fBeforeWeaponDurability, fMaxDurability));

	// super
	GLCHARLOGIC::DurabilityDropDeath(bPvp);
	if ( bOk == false || GetWeaponDurabilityValue(fCurrentWeaponDurability, fMaxDurability) == false )
		return;

	float fPercent(1.0f-fCurrentWeaponDurability/fMaxDurability);
	if ( fPercent <= GLCONST_CHAR::fDurabilityAlarmRemainPer )
	{
		// 차이가 있는지 확인한다;
		if ( fCurrentWeaponDurability!=fBeforeWeaponDurability )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetWeaponBoostInfo, "-f, -f, -f"
				, GLCONST_CHAR::fDurabilityAlarmRemainPer, fCurrentWeaponDurability, fMaxDurability );
		}
	}
}

void GLCharacter::ItemRepairing( EMSLOT _slot )
{
	float fBeforeWeaponDurability, fCurrentWeaponDurability, fMaxDurability;
	const bool bOk(GetWeaponDurabilityValue(fBeforeWeaponDurability, fMaxDurability));

	// super
	GLCHARLOGIC::ItemRepairing(_slot);
	if ( bOk == false || GetWeaponDurabilityValue(fCurrentWeaponDurability, fMaxDurability) == false )
		return;

	// 이전상태가 경고를 보여주고 있는 상태라면 꺼줘야한다;
	float fPercent(1.0f-fBeforeWeaponDurability/fMaxDurability);
	if ( fPercent <= GLCONST_CHAR::fDurabilityAlarmRemainPer )
	{
		// 차이가 있는지 확인한다;
		if ( fCurrentWeaponDurability!=fBeforeWeaponDurability )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetWeaponBoostInfo, "-f, -f, -f"
				, GLCONST_CHAR::fDurabilityAlarmRemainPer, fCurrentWeaponDurability, fMaxDurability );
		}
	}
}

void GLCharacter::Durabilitiy_INIT_DATA( EMSLOT slot )
{
	GLCHARLOGIC::Durabilitiy_INIT_DATA(slot);
}

LuaTable GLCharacter::GetWeaponBoostInitData(void)
{
	float fCurrentWeaponDurability, fMaxDurability;
	LuaTable tbWeaponBoostInit(GLWidgetScript::GetInstance().GetLuaState()); 
	if ( false == GetWeaponDurabilityValue(fCurrentWeaponDurability, fMaxDurability) )
		return tbWeaponBoostInit;

	tbWeaponBoostInit.set(1, GLCONST_CHAR::fDurabilityAlarmRemainPer);
	tbWeaponBoostInit.set(2, fCurrentWeaponDurability);
	tbWeaponBoostInit.set(3, fMaxDurability);
	return tbWeaponBoostInit;
}

void GLCharacter::SfSetWeaponBoostInfo(void)
{
	float fCurrentWeaponDurability, fMaxDurability;
	if ( false == GetWeaponDurabilityValue(fCurrentWeaponDurability, fMaxDurability) )
	{
		fCurrentWeaponDurability = fMaxDurability = -1;
		//return;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetWeaponBoostInfo, "-f, -f, -f"
		, GLCONST_CHAR::fDurabilityAlarmRemainPer, fCurrentWeaponDurability, fMaxDurability );
}