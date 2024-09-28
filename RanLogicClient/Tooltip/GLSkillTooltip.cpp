#include "pch.h"
#include "GLSkillTooltip.h"

#include "../../RanLogic/GLSkillToolTip.h"
#include "../../RanLogic/Skill/GLSkill.h"
#include "../../RanLogic/Skill/GLSkillEffVarSet.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanGfxUI/GfxDef.h"

#include "../Char/GLCharacter.h"
#include "../ReferChar/ReferChar.h"
#include "../GLGaeaClient.h"

GLSkillTooltip::GLSkillTooltip()
:	m_pSkill( NULL )
,	m_pLearnedSkill( NULL )
{
}

GLSkillTooltip::GLSkillTooltip( const GLSkillTooltip& value )
{
}

GLSkillTooltip::~GLSkillTooltip()
{
}

GLSkillTooltip& GLSkillTooltip::GetInstance()
{
	static GLSkillTooltip Instance;
	return Instance;
}

bool GLSkillTooltip::GetHeader( const SNATIVEID& sID, LuaTable& tbHeader, WORD wSlotType, int nLevel )
{
	if( false == SaveSkill( sID ) )
		return false;

	// 슬롯 타입
	tbHeader.set( "SlotType", wSlotType );

	GenName( tbHeader );
	GenLevel( tbHeader, nLevel );
	GenGrade( tbHeader );
	GenAttribute( tbHeader );
	
	return true;
}

bool GLSkillTooltip::GetTooltip( const SNATIVEID& sID, LuaTable& tbTooltip, WORD wSlotType, int nLevel, int iDisguise )
{
	if( false == SaveSkill( sID ) )
		return false;

	// 슬롯 타입
	tbTooltip.set( "SlotType", wSlotType );

	GenType( tbTooltip );
	GenActionType( tbTooltip );
	GenApply( tbTooltip );
	GenUseItem( tbTooltip );
	GenUseCondition( tbTooltip, nLevel );
	GenBasicAttribute( tbTooltip, nLevel );
	GenRequestSkill( tbTooltip );
	GenDemandMoney( tbTooltip, nLevel );
	GenTarget( tbTooltip );
	GenArea( tbTooltip );
	GenPart( tbTooltip );
	GenRange( tbTooltip );
	GenApplyType( tbTooltip, nLevel );
	GenSideEffect( tbTooltip, nLevel );
	GenSpecialEffect( tbTooltip, nLevel );
	GenSpecialEffectCA( tbTooltip, nLevel );
	GenSpecialAbility( tbTooltip );
	GenSubSkill( tbTooltip, nLevel );
	GenDesc( tbTooltip, wSlotType, nLevel );
	GenBlow(tbTooltip, nLevel);	// 상태이상;
	GenDisguise( tbTooltip, iDisguise ); // 변장 전/후/NONE
	GenRequire( tbTooltip, nLevel );
	return true;
}

bool GLSkillTooltip::SaveSkill( const SNATIVEID& sID )
{
	GLSKILL* pNewSaveSkill = NULL;
	if( NULL != m_pSkill )
	{
		if( m_pSkill->GetId() != sID )
			pNewSaveSkill = GLSkillMan::GetInstance().GetData( sID );
		else
			pNewSaveSkill = m_pSkill;
	}
	else
	{
		pNewSaveSkill = GLSkillMan::GetInstance().GetData( sID );
	}

	if( NULL == pNewSaveSkill )
		return false;

	m_pSkill = pNewSaveSkill;

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	m_pLearnedSkill = pChar->GETLEARNED_SKILL( sID );

	return true;
}

void GLSkillTooltip::GenName( LuaTable& tb )
{
	LuaTable tbName( GLWidgetScript::GetInstance().GetLuaState() );
	
	tbName.set( 1, m_pSkill->GetId().wMainID );
	tbName.set( 2, m_pSkill->GetId().wSubID );
	tbName.set( 3, m_pSkill->GetName() );

	tb.set( "tbName", tbName );
}

void GLSkillTooltip::GenLevel( LuaTable& tb, int nLevel )
{
	if( nLevel == Disable_View_Level )
	{
		//tb.set( "tbLevel", NULL );
		return ;
	}

	if( -1 == nLevel )
		nLevel = NULL != m_pLearnedSkill ? m_pLearnedSkill->wLevel : 0;

	LuaTable tbLevel( GLWidgetScript::GetInstance().GetLuaState() );
	tbLevel.set( 1, m_pSkill->m_sBASIC.dwMAXLEVEL );
	tbLevel.set( 2, nLevel + 1 );

	tb.set( "tbLevel", tbLevel );
}

void GLSkillTooltip::GenGrade( LuaTable& tb )
{
	tb.set( "Grade", m_pSkill->m_sBASIC.dwGRADE );
}

void GLSkillTooltip::GenAttribute( LuaTable& tb )
{
	tb.set( "Attribute", (int) m_pSkill->m_sAPPLY.emELEMENT );
}

void GLSkillTooltip::GenType( LuaTable& tb )
{
	tb.set( "Type", (int) m_pSkill->m_sBASIC.emROLE );
}

void GLSkillTooltip::GenActionType( LuaTable& tb )
{
	tb.set( "ActionType", (int) m_pSkill->m_sBASIC.emACTION_TYPE );
}

void GLSkillTooltip::GenApply( LuaTable& tb )
{
	tb.set( "Apply", (int) m_pSkill->m_sBASIC.emAPPLY );
}

void GLSkillTooltip::GenUseItem( LuaTable& tb )
{
	SKILL::GLSKILL_ATT emSkillAtt = SKILL::SKILLATT_NOTHING;
	if( TRUE == m_pSkill->m_sLEARN.bHiddenWeapon )
	{
		SITEM* pItem = GLogicData::GetInstance().GetItem( m_pSkill->m_sLEARN.sHiddenWeapon );
		if( NULL != pItem )
			emSkillAtt = ItemAtt2SkillAtt( pItem->sSuitOp.emAttack, m_pSkill->m_sLEARN.dwCLASS );
	}

	LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
	tbItem.set( 1, (int) m_pSkill->m_sBASIC.emUSE_LITEM );
	tbItem.set( 2, (int) m_pSkill->m_sBASIC.emUSE_RITEM );
	tbItem.set( 3, (int) emSkillAtt );

	tb.set( "tbItem", tbItem );
}

void GLSkillTooltip::GenUseCondition( LuaTable& tb, int nLevel )
{
	if( -1 == nLevel )
		nLevel = NULL != m_pLearnedSkill ? m_pLearnedSkill->wLevel : 0;

	const SKILL::CDATA_LVL& sDataLevel = m_pSkill->m_sAPPLY.sDATA_LVL[ nLevel ];

	LuaTable tbUseCondition( GLWidgetScript::GetInstance().GetLuaState() );
	tbUseCondition.set( 1, sDataLevel.wUSE_ARROWNUM );
	tbUseCondition.set( 2, sDataLevel.wUSE_CHARMNUM );
	tbUseCondition.set( 3, sDataLevel.wUSE_BULLETNUM );
	tbUseCondition.set( 4, sDataLevel.wUSE_EXP );
	tbUseCondition.set( 5, sDataLevel.wUSE_HP );
	tbUseCondition.set( 6, sDataLevel.wUSE_MP );
	tbUseCondition.set( 7, sDataLevel.wUSE_SP );
	tbUseCondition.set( 8, sDataLevel.wUSE_CP );
	tbUseCondition.set( 9, sDataLevel.nUSE_BATTERY );
	tbUseCondition.set( 9, sDataLevel.wUSE_HP_PTY );
	tbUseCondition.set( 10, sDataLevel.wUSE_MP_PTY );
	tbUseCondition.set( 11, sDataLevel.wUSE_SP_PTY );
	tbUseCondition.set( 12, sDataLevel.wUSE_HP_PTY );

	tb.set( "tbUseCondition", tbUseCondition );
}

void GLSkillTooltip::GenBasicAttribute( LuaTable& tb, int nLevel )
{
	if( -1 == nLevel )
		nLevel = NULL != m_pLearnedSkill ? m_pLearnedSkill->wLevel : 0;

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	const SKILL::CDATA_LVL& sDataLevel = m_pSkill->m_sAPPLY.sDATA_LVL[ nLevel ];

	LuaTable tbBasicAttribute( GLWidgetScript::GetInstance().GetLuaState() );
	tbBasicAttribute.set( 1, sDataLevel.fBASIC_VAR );
	const float fDecreaseT(sDataLevel.fDELAYTIME*pChar->GetDecR_SkillDelayT(m_pSkill->m_sBASIC.dwGRADE-1))
		, fDelayTime(sDataLevel.fDELAYTIME-fDecreaseT);
	tbBasicAttribute.set( 2, sDataLevel.fDELAYTIME - pChar->GetDecR_SkillDelayT(m_pSkill->m_sBASIC.dwGRADE-1) );
	tbBasicAttribute.set( 3, sDataLevel.fLIFE + pChar->GetInc_SkillDurationTAdd(m_pSkill->m_sBASIC.dwGRADE-1));
	tbBasicAttribute.set( 4, sDataLevel.wAPPLYRANGE );
	tbBasicAttribute.set( 5, sDataLevel.wAPPLYANGLE );
	tbBasicAttribute.set( 6, sDataLevel.wPIERCENUM );
	tbBasicAttribute.set( 7, sDataLevel.wTARNUM );

	tb.set( "tbBasicAttribute", tbBasicAttribute );
}

void GLSkillTooltip::GenRequestSkill( LuaTable& tb )
{
	LuaTable tbRequestSkill( GLWidgetScript::GetInstance().GetLuaState() );
	tbRequestSkill.set( 1, m_pSkill->m_sLEARN.sSKILL.wMainID );
	tbRequestSkill.set( 2, m_pSkill->m_sLEARN.sSKILL.wSubID );

	tb.set( "tbRequestSkill", tbRequestSkill );
}

void GLSkillTooltip::GenDemandMoney( LuaTable& tb, INT nLevel )
{
	LuaTable tbDemandMoney( GLWidgetScript::GetInstance().GetLuaState() );
	// tbDemandMoney.set( 1, NULL == m_pLearnedSkill ? false : true );
	tbDemandMoney.set( 1, true );
	tbDemandMoney.set( 2, m_pSkill->m_sLEARN.sLVL_STEP[nLevel].dwReqMoney );
	tb.set( "tbDemandMoney", tbDemandMoney );
}

void GLSkillTooltip::GenTarget( LuaTable& tb )
{
	tb.set( "Target", (int) m_pSkill->m_sBASIC.emIMPACT_TAR );
}

void GLSkillTooltip::GenArea( LuaTable& tb )
{
	tb.set( "Area", (int) m_pSkill->m_sBASIC.emIMPACT_REALM );
}

void GLSkillTooltip::GenPart( LuaTable& tb )
{
	tb.set( "Part", (int) m_pSkill->m_sBASIC.emIMPACT_SIDE );
}

void GLSkillTooltip::GenRange( LuaTable& tb )
{
	tb.set( "Range", m_pSkill->m_sBASIC.wTARRANGE );
}

void GLSkillTooltip::GenApplyType( LuaTable& tb, int nLevel )
{
	tb.set( "ApplyType", GLSkillToolTip::MakeTooltipSting_In_Skill_Type(
		GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter(), m_pSkill, nLevel ).c_str() );
}

void GLSkillTooltip::GenSideEffect( LuaTable& tb, int nLevel )
{
	LuaTable tbSideEffect( GLWidgetScript::GetInstance().GetLuaState() );

	if( -1 == nLevel )
		nLevel = NULL != m_pLearnedSkill ? m_pLearnedSkill->wLevel : 0;

	const std::vector< SKILL::SIMPACT_ADDON >& vecAddOn = m_pSkill->m_sAPPLY.vecADDON;
	for( size_t i = 0; i < vecAddOn.size(); ++i )
	{
		LuaTable tbAddOn( GLWidgetScript::GetInstance().GetLuaState() );
		tbAddOn.set( 1, (int) vecAddOn[ i ].emADDON );
		// tbAddOn.set( 2, vecAddOn[ i ].fADDON_VAR[ nLevel ] );
		tbAddOn.set( 2, (float)(vecAddOn[ i ].fADDON_VAR[ nLevel ] * COMMENT::IMPACT_ADDON_SCALE[m_pSkill->m_sAPPLY.vecADDON[i].emADDON]) );
		tbAddOn.set( 3, vecAddOn[ i ].fRate[ nLevel ] );

		tbSideEffect.set( i + 1, tbAddOn );
	}

	tb.set( "tbSideEffect", tbSideEffect );
}

void GLSkillTooltip::GenSpecialEffect( LuaTable& tb, int nLevel )
{
	if( -1 == nLevel )
		nLevel = NULL != m_pLearnedSkill ? m_pLearnedSkill->wLevel : 0;

	std::vector< std::string > vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Special( m_pSkill, (WORD) nLevel );

	LuaTable tbSpecialEffectStr( GLWidgetScript::GetInstance().GetLuaState() );
	for( size_t i = 0; i < vecString.size(); ++i )
		tbSpecialEffectStr.set( i + 1, vecString[ i ].c_str() );
	tb.set( "tbSpecialEffectStr", tbSpecialEffectStr );

	LuaTable tbSpecialEffect( GLWidgetScript::GetInstance().GetLuaState() );

	const std::vector< SKILL::SSPEC_ADDON >& vecSpec = m_pSkill->m_sAPPLY.vecSPEC;
	for( size_t i = 0; i < vecSpec.size(); ++i )
	{
		const SKILL::SSPEC& sSpec = vecSpec[ i ].sSPEC[ nLevel ];

		LuaTable tbSpec( GLWidgetScript::GetInstance().GetLuaState() );
		tbSpec.set( 1, (int) vecSpec[ i ].emSPEC );
		tbSpec.set( 2, sSpec.fVAR1 * COMMENT::SPEC_ADDON_VAR1_SCALE[vecSpec[ i ].emSPEC]);
		tbSpec.set( 3, sSpec.fVAR2 * COMMENT::SPEC_ADDON_VAR2_SCALE[vecSpec[ i ].emSPEC]);
		tbSpec.set( 4, sSpec.fRate );
		tbSpec.set( 5, sSpec.fRate2 );
		tbSpec.set( 6, sSpec.dwFLAG );
		tbSpec.set( 7, sSpec.dwNativeID.wMainID );
		tbSpec.set( 8, sSpec.dwNativeID.wSubID );
		tbSpec.set( 9, sSpec.dwLinkID.wMainID );
		tbSpec.set( 10, sSpec.dwLinkID.wSubID );
		tbSpec.set( 11, sSpec.nVAR1);
		tbSpec.set( 12, sSpec.nVAR2);

		tbSpecialEffect.set( i + 1, tbSpec );
	}

	tb.set( "tbSpecialEffect", tbSpecialEffect );
}

void GLSkillTooltip::GenSpecialEffectCA(LuaTable& tb, int nLevel)
{
	LuaTable tbSpecialEffectCA( GLWidgetScript::GetInstance().GetLuaState() );
	if( -1 == nLevel )
		nLevel = NULL != m_pLearnedSkill ? m_pLearnedSkill->wLevel : 0;

	const std::vector< SKILL::SSPEC_ADDON >& vecSpec = m_pSkill->m_sAPPLY.vecSPEC;
	for( size_t i = 0; i < vecSpec.size(); ++i )
	{
		const SKILL::SSPEC_ADDON& sSpecAddon = vecSpec[i];
		if ( sSpecAddon.emSPEC == EMSPECA_COUNTATTACK )
		{
			LuaTable tbSpecCA( GLWidgetScript::GetInstance().GetLuaState() );

			SNATIVEID sNID(sSpecAddon.sSPEC[nLevel].fVAR1, sSpecAddon.sSPEC[nLevel].fVAR2);
			const SKILL::SSPEC_CA& sSpecCA = m_pSkill->m_sAPPLY.sCAData.sSPEC_CA[nLevel];
			for ( int j = 0; j < SKILL::MAX_ADDON; ++j )
			{
				if ( sSpecCA.emSPEC[j] != EMSPECA_NULL && !sSpecCA.bAllApply )
				{
					tbSpecCA.set( j, (INT)sSpecCA.emSPEC[j] );
					tbSpecialEffectCA.set( 4, (INT)sNID.Mid() );
					tbSpecialEffectCA.set( 5, (INT)sNID.Sid() );
				}
				else
					tbSpecCA.set( j, (INT)EMSPECA_NULL );
			}

			tbSpecialEffectCA.set( 1, tbSpecCA );
			tbSpecialEffectCA.set( 2, sSpecCA.bDamageApply );
			tbSpecialEffectCA.set( 3, sSpecCA.bAllApply );
			break;
		}
	}
	tb.set( "tbSpecialEffectCA", tbSpecialEffectCA );
}

void GLSkillTooltip::GenSpecialAbility( LuaTable& tb )
{
	tb.set( "SpecialAbility", (int) m_pSkill->m_sSPECIAL_SKILL.emSSTYPE );
}

void GLSkillTooltip::GenSubSkill( LuaTable& tb, int nLevel )
{
	if( -1 == nLevel )
		nLevel = NULL != m_pLearnedSkill ? m_pLearnedSkill->wLevel : 0;

	LuaTable tbSubSkill( GLWidgetScript::GetInstance().GetLuaState() );

	// 멀티 스킬
	if( true == m_pSkill->m_sEXT_DATA.idMultiTargetSkill.IsValidNativeID() )
	{
		LuaTable tbMultiSkill( GLWidgetScript::GetInstance().GetLuaState() );
		tbMultiSkill.set( 1, m_pSkill->m_sEXT_DATA.idMultiTargetSkill.Mid() );
		tbMultiSkill.set( 2, m_pSkill->m_sEXT_DATA.idMultiTargetSkill.Sid() );

		tbSubSkill.set( 1, tbMultiSkill );
	}

	const SKILL::SAPPLY& sApply = m_pSkill->m_sAPPLY;
	for( size_t i = 0; i < sApply.vecSPEC.size(); ++i )
	{
		// 링크 스킬
		if( EMSPECA_SKILL_LINK == sApply.vecSPEC[ i ].emSPEC )
		{
			WORD wSkillLevel = max( nLevel - 1 , 0 );

			GLSKILL* pLinkSKill = GLSkillMan::GetInstance().GetData( 
				sApply.vecSPEC[ i ].sSPEC[ wSkillLevel ].dwNativeID );

			if( NULL != pLinkSKill )
			{
				LuaTable tbLinkSkill( GLWidgetScript::GetInstance().GetLuaState() );
				tbLinkSkill.set( 1, pLinkSKill->GetId().Mid() );
				tbLinkSkill.set( 2, pLinkSKill->GetId().Sid() );

				tbSubSkill.set( 2, tbLinkSkill );
			}
		}

		// 딜레이 액션 스킬
		if( EMSPECA_DELAY_ACTION_SKILL == sApply.vecSPEC[ i ].emSPEC )
		{
			SNATIVEID sID( static_cast< WORD >( sApply.vecSPEC[ i ].sSPEC[ 0 ].fVAR1 ),
				static_cast< WORD >( sApply.vecSPEC[ i ].sSPEC[ 0 ].fVAR2 ) );

			GLSKILL* pDelaySkill = GLSkillMan::GetInstance().GetData( sID );
			if( NULL != pDelaySkill )
			{
				LuaTable tbDelaySkill( GLWidgetScript::GetInstance().GetLuaState() );
				tbDelaySkill.set( 1, pDelaySkill->GetId().Mid() );
				tbDelaySkill.set( 2, pDelaySkill->GetId().Sid() );

				tbSubSkill.set( 3, tbDelaySkill );
			}
		}
	}

	tb.set( "SubSkill", tbSubSkill );
}

void GLSkillTooltip::GenDesc( LuaTable& tb, WORD wSlotType, int nLevel )
{
	if( -1 == nLevel )
		nLevel = NULL != m_pLearnedSkill ? m_pLearnedSkill->wLevel : 0;

	std::string strDesc;
	if( wSlotType == EMGFX_SLOT_BUFF )
	{
		CString str;
		m_pSkill->GetDescFormatByLevel( str, (WORD) nLevel, 
			GLSKILL::DESC_FORMAT_IMPACT | GLSKILL::DESC_FORMAT_SPEC );

		strDesc = str.GetBuffer();
	}
	else
	{
		strDesc = m_pSkill->GetDesc();
	}

	///////////////////////////////

	
	GLCHARLOGIC* pTarget = NULL;
	pTarget = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if ( pTarget != NULL)
	{
		for( int i = 0; i < SKILLFACT_SIZE; ++i )
		{
			SSKILLFACT sSkillFact = pTarget->GetSkillFact(i);
			if( sSkillFact.sNATIVEID == m_pSkill->GetId())
			{
				CString strTime;

				CTime cCurTime( GLWidgetScript::GetInstance().m_pGaeaClient->GetCurrentTime() );
				GLWidgetScript::GetInstance().m_pGaeaClient->GetConvertServerTime( cCurTime );

				SSKILLFACT::GetTimeByStringFormat(
					strTime,
					cCurTime.GetTime(), 
					SSKILLFACT::STIME(sSkillFact),
					SSKILLFACT::EMTIMESTRINGFORMAT_SIMPLE);

				strTime = " " + strTime;
				strTime = ID2GAMEINTEXT("SKILLFACT_LIFETIME") + strTime;
				strDesc += "\n" + strTime;


				break;
			}
		}

	}

	tb.set( "Explanation", strDesc.c_str() );
}

LuaTable GLSkillTooltip::CalcDamage( bool bMyChar, WORD wLevel )
{
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	LuaTable tbDamage( GLWidgetScript::GetInstance().GetLuaState() );

	GLCHARLOGIC* pTarget = NULL;
	if( true == bMyChar )
		pTarget = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	else
		pTarget = GLWidgetScript::GetInstance().m_pGaeaClient->GetReferChar()->GetCompleteData();

	if( NULL == m_pSkill || NULL == pTarget )
	{
		tbDamage.set( 1, 0 );
		tbDamage.set( 2, 0 );

		return tbDamage;
	}

	const SKILL::SAPPLY& sAPPLY = m_pSkill->m_sAPPLY;
	// 스킬 레벨은 배열에서 0부터 시작하기때문에 -1을 해야한다.
	const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[ wLevel -1 ];
	const SKILL::SBASIC& sBASIC = m_pSkill->m_sBASIC;

	GLDWDATA gdDamage = pTarget->m_gdDAMAGE_SKILL;

	if( 0 < sDATA_LVL.wUSE_CHARMNUM )
	{
		EMSLOT emLHand = pTarget->GetCurLHand();
		/*SITEM* pItem = pTarget->GET_SLOT_ITEMDATA(emLHand);
		if( NULL != pItem )
		{
			GLPADATA& sDATA = pItem->sSuitOp.gdDamage;
			gdDamage.nNow += int(sDATA.wLow);
			gdDamage.nMax += int(sDATA.wHigh);
		}*/
		SITEMCUSTOM sLItemCustom = pTarget->GET_SLOT_ITEM(emLHand);
		const GLPADATA& sDATA = sLItemCustom.GetBasicAttackDamage();
		gdDamage.nNow += int(sDATA.wLow);
		gdDamage.nMax += int(sDATA.wHigh);
	}

	float fSkillDmg = -sDATA_LVL.fBASIC_VAR;
	const float fSkillDamageAdd(pChar->GetIncR_SkillDamageAdd(m_pSkill->m_sBASIC.dwGRADE-1));
	fSkillDmg += fSkillDmg * fSkillDamageAdd;

	const SKILL::EMAPPLY emApply = m_pSkill->m_sBASIC.emAPPLY;
	if( SKILL::EMAPPLY_MAGIC != emApply )
	{								
		gdDamage.nNow += pTarget->m_sSUMITEM.gdDamage.nNow;
		gdDamage.nMax += pTarget->m_sSUMITEM.gdDamage.nMax;
	}
	gdDamage.VAR_PARAM( pTarget->m_powerAttribute[ emApply ] );

	float fLowDmg = static_cast< float >( gdDamage.nNow );
	float fHighDmg = static_cast< float >( gdDamage.nMax );

	EMSLOT emRHand = pTarget->GetCurRHand();
	const SITEMCUSTOM& sRHAND = pTarget->GET_SLOT_ITEM( emRHand );

	WORD wGRADE = 0;
	if( sRHAND.GetNativeID() != SNATIVEID( false ) )
		wGRADE = sRHAND.GETGRADE( EMGRINDING_DAMAGE );

	float fGrade = static_cast< float >( wGRADE ) / GLCONST_CHAR::fDAMAGE_GRADE_K;

	fLowDmg += fSkillDmg + (fLowDmg  * fGrade);
	fHighDmg += fSkillDmg + (fHighDmg * fGrade);

	fLowDmg = fLowDmg * pTarget->m_fDamageRate;
	fHighDmg = fHighDmg * pTarget->m_fDamageRate;
	
	tbDamage.set( 1, fLowDmg );
	tbDamage.set( 2, fHighDmg );

	return tbDamage;
}

void GLSkillTooltip::GenBlow( LuaTable& tb, int nLevel )
{
/*
	local emBLOW = tbBlow[0]
	local fLife = tbBlow[1]
	local fRate = tbBlow[2]
	local fVAR1 = tbBlow[3]
	local fVAR2 = tbBlow[4]
*/


	const SKILL::SAPPLY& sAPPLY = m_pSkill->m_sAPPLY;
	EMSTATE_BLOW emBLOW = sAPPLY.emSTATE_BLOW;

	LuaTable tbBlow( GLWidgetScript::GetInstance().GetLuaState() );
	tbBlow.set( 0, (int) emBLOW );
	tbBlow.set( 1, (float) sAPPLY.sDATA_LVL[ nLevel ].fLIFE );
	tbBlow.set( 2, (float) sAPPLY.sSTATE_BLOW[nLevel].fRATE );
	tbBlow.set( 3, (float) sAPPLY.sSTATE_BLOW[nLevel].fVAR1 );
	tbBlow.set( 4, (float) sAPPLY.sSTATE_BLOW[nLevel].fVAR2 );
	tb.set( "tbBlow", tbBlow );
}

void GLSkillTooltip::GenDisguise( LuaTable& tb, int iDisguise )
{
	tb.set( "DisguiseSkill", iDisguise );
}

void GLSkillTooltip::GenRequire( LuaTable& tb, int nLevel )
{
	if( -1 == nLevel )
		nLevel = NULL != m_pLearnedSkill ? m_pLearnedSkill->wLevel + 1 : 0;

	if( SKILL::MAX_LEVEL <= nLevel )
		nLevel = SKILL::MAX_LEVEL - 1;

	const SKILL::SLEARN& sLearn = m_pSkill->m_sLEARN;
	const SKILL::SLEARN_LVL& sRequire = sLearn.sLVL_STEP[ nLevel ];

	GLSKILL* pNeedSkill = GLSkillMan::GetInstance().GetData( sLearn.sSKILL );
	
	LuaTable tbRequire( GLWidgetScript::GetInstance().GetLuaState() );
	tbRequire.set( 1, sRequire.dwLEVEL );			// 레벨
	tbRequire.set( 2, sRequire.dwSKP );				// 스킬 포인트
	tbRequire.set( 3, sRequire.sSTATS.wPow );		// 힘
	tbRequire.set( 4, sRequire.sSTATS.wStr );		// 체력
	tbRequire.set( 5, sRequire.sSTATS.wSpi );		// 정신
	tbRequire.set( 6, sRequire.sSTATS.wDex );		// 민첩
	tbRequire.set( 7, sRequire.sSTATS.wSta );		// 근력

	// 선행 스킬 이름, 레벨
	if( NULL == pNeedSkill )
	{
		tbRequire.set( 8, "" );
		tbRequire.set( 9, -1 );
		tbRequire.set( 10, -1 );
	}
	else
	{
		tbRequire.set( 8, pNeedSkill->GetName() );
		tbRequire.set( 9, sRequire.dwSKILL_LVL + 1 );
		tbRequire.set( 10, sLearn.sSKILL.dwID );
	}

	tbRequire.set( 11, sLearn.bOnlyOneStats );		// 스탯 하나만 충족되도 되는가?

	tb.set( "Require", tbRequire );
}
