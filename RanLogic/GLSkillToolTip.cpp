#include "pch.h"
#include  "GLSkillToolTip.h"
#include "./Skill/SpecialEffect/GLSpecExperiencePoint.h"
#include "./GLUseFeatures.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "GLogicDataMan.h"
#include "./Crow/GLCrowDataMan.h"

namespace GLSkillToolTip
{
    std::string MakeTooltipSting_In_Skill_Type( GLCHARLOGIC* pTarget, const GLSKILL* glSkill, WORD wLevel )
    {
        std::string strText = "";


        const SKILL::SAPPLY& sAPPLY = glSkill->m_sAPPLY;
        const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[wLevel];
        const SKILL::SBASIC& sBASIC = glSkill->m_sBASIC;

        SKILL::EMROLE emRole = glSkill->m_sBASIC.emROLE;

        SKILL_INFOR_TYPE emInforType = INFOR_NONE_TYPE;
        if ( emRole == SKILL::EMROLE_NORMAL )
        {
            if ( sDATA_LVL.fLIFE )
            {
                if ( sDATA_LVL.fBASIC_VAR > 0.0f )
                    emInforType = NORMAL_DURATION_PLUS;
                else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
                    emInforType = NORMAL_DURATION_MINUS;
            }
            else
            {
                if ( sDATA_LVL.fBASIC_VAR > 0.0f )
                    emInforType = NORMAL_NONDURATION_PLUS;
                else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
                    emInforType = NORMAL_NONDURATION_MINUS;
            }
        }
        else if ( emRole == SKILL::EMROLE_PASSIVE )
        {
            if ( sDATA_LVL.fLIFE )
            {
                if ( sDATA_LVL.fBASIC_VAR > 0.0f )
                    emInforType = PASSIVE_DURATION_PLUS;
                else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
                    emInforType = PASSIVE_DURATION_MINUS;
            }
            else
            {
                if ( sDATA_LVL.fBASIC_VAR > 0.0f )
                    emInforType = PASSIVE_NONDURATION_PLUS;
                else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
                    emInforType = PASSIVE_NONDURATION_MINUS;
            }
        }

        // Info : 적용 타입
        {
            //if ( emInforType != INFOR_NONE_TYPE )
            //{
            //    strText = ID2GAMEWORD( "SKILL_BASIC_INFO", 13);
            //    m_pSkillLabel5->AddText( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
            //}

            float fBASIC_VAR_SCALE;

			BOOL bSKILL_PER;
			std::string strSKILL_TYPE;
			
			fBASIC_VAR_SCALE = sDATA_LVL.fBASIC_VAR * COMMENT::SKILL_TYPES_SCALE[sAPPLY.emBASIC_TYPE];
			bSKILL_PER = COMMENT::IsSKILL_PER(sAPPLY.emBASIC_TYPE);
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				strSKILL_TYPE = CGameTextMan::GetInstance().GetCommentText("SKILL_TYPES", sAPPLY.emBASIC_TYPE).GetString();
			}
			else
			{
				strSKILL_TYPE = COMMENT::SKILL_TYPES[sAPPLY.emBASIC_TYPE];;
			}
            switch ( sAPPLY.emBASIC_TYPE )
            {
            case SKILL::EMFOR_HP://	체력치 변화.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                GLDWDATA gdDamage = pTarget->m_gdDAMAGE_SKILL;

                                if ( sDATA_LVL.wUSE_CHARMNUM > 0 )
                                {
                                    EMSLOT emLHand = pTarget->GetCurLHand();
                                    SITEMCUSTOM pItemCustom = pTarget->GET_SLOT_ITEM(emLHand);
									//GLPADATA& sDATA = pItem->sSuitOp.gdDamage;
									const GLPADATA& sDATA = pItemCustom.GetBasicAttackDamage();
									gdDamage.nNow += int(sDATA.wLow);
									gdDamage.nMax += int(sDATA.wHigh);
                                }

								int nSkillDmg = ::abs( int( sDATA_LVL.fBASIC_VAR ) );

								const SKILL::EMAPPLY emApply(glSkill->m_sBASIC.emAPPLY);

								switch ( emApply )
								{
								case SKILL::EMAPPLY_MELEE: // 격투;
									gdDamage.nNow += (pTarget->m_sSUMITEM.gdDamage.nNow + pTarget->m_sumPowerAttribute[SKILL::EMAPPLY_MELEE]);
									gdDamage.nMax += (pTarget->m_sSUMITEM.gdDamage.nMax + pTarget->m_sumPowerAttribute[SKILL::EMAPPLY_MELEE]);
									break;

								case SKILL::EMAPPLY_RANGE: // 사격;
									gdDamage.nNow += int(pTarget->m_sSUMITEM.gdDamage.nNow) + pTarget->m_sumPowerAttribute[SKILL::EMAPPLY_RANGE];
									gdDamage.nMax += int(pTarget->m_sSUMITEM.gdDamage.nMax) + pTarget->m_sumPowerAttribute[SKILL::EMAPPLY_RANGE];		
									break;

								case SKILL::EMAPPLY_MAGIC: // 마력;
									gdDamage.nNow += pTarget->m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC];
									gdDamage.nMax += pTarget->m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC];
									break;
								};

                                WORD wGRADE = 0;
                                EMSLOT emRHand = pTarget->GetCurRHand();
                                const SITEMCUSTOM& sRHAND = pTarget->GET_SLOT_ITEM(emRHand);
                                if ( sRHAND.GetNativeID() != SNATIVEID(false) )
                                    wGRADE = sRHAND.GETGRADE(EMGRINDING_DAMAGE);
                                float fGrade = static_cast<float>(wGRADE) / GLCONST_CHAR::fDAMAGE_GRADE_K;

                                gdDamage.nNow += static_cast< int >( nSkillDmg + static_cast<float>( gdDamage.nNow ) * fGrade );
                                gdDamage.nMax += static_cast< int >( nSkillDmg + static_cast<float>( gdDamage.nMax ) * fGrade );

                                gdDamage.nNow = static_cast< int >( static_cast<float>( gdDamage.nNow ) * pTarget->m_fDamageRate );
                                gdDamage.nMax = static_cast< int >( static_cast<float>( gdDamage.nMax ) * pTarget->m_fDamageRate );

                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE1_2")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5)
									, static_cast< float >( nSkillDmg )
                                    , static_cast< float >( gdDamage.nNow )
									, static_cast< float >( gdDamage.nMax ) );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_MP://	기력치 변화.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 6), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 6), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 6), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 6), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 6), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 6), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 6), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 6), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_SP://	근력치 변화.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 7), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 7), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 7), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 7), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 7), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 7), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 7), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 7), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_VARHP://	채력치 변화율.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_VARMP://	기력치 변화율.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_VARSP://	근력치 변화율.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_DEFENSE://	방어치 변화.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_CURE://	해독. ( 해독 종류 필요 )
                {
                    strText = sc::string::format( "%s", ID2GAMEINTEXT("SKILL_INFOR_CURE") );
                }
                break;

            case SKILL::EMFOR_HITRATE://	명중율.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_AVOIDRATE://	회피율.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_VARAP://	모든 (체력,기력,근력) 변화율.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    , sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_VARDAMAGE://	공격치 변화량.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_VARDEFENSE://	방어치 변화량.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_PA://	격투치.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_SA://	사격치.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                };
                break;

            case SKILL::EMFOR_MA://	방어치.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_HP_RATE://	HP 증폭율.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE6_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE6")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE6_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE6")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_MP_RATE://	MP 증폭율.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE6_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE6")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE6_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE6")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_SP_RATE://	SP 증폭율.
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE6_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE6")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE6_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE6")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, ID2GAMEWORD("ITEM_ADDON_INFO", 5), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE1_1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE1")
                                    , ID2GAMEWORD("ITEM_ADDON_INFO", 5), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_RESIST: //	저항 변화량
                {
                    switch ( emInforType )
                    {
                    case NORMAL_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case NORMAL_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_DURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                    ,sDATA_LVL.fLIFE, strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_PLUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            if ( bSKILL_PER == TRUE )
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                    , strSKILL_TYPE.c_str(), -fBASIC_VAR_SCALE );
                            }
                            else
                            {
                                strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                    , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                            }
                        }
                        break;
                    };
                }
                break;

            case SKILL::EMFOR_SUMMON_ACTIVETIME: //	소환시간 증가
                {
                    switch ( emInforType )
                    {
                    case PASSIVE_NONDURATION_PLUS:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE5")
                                , strSKILL_TYPE.c_str(), sDATA_LVL.fBASIC_VAR );
                        }
                        break;

                    case PASSIVE_NONDURATION_MINUS:
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE5")
                                , strSKILL_TYPE.c_str(), -sDATA_LVL.fBASIC_VAR );
                        }
                        break;
                    };
                }
                break;
            };
        }
        return strText;
    }

    std::vector<std::string>MakeTooltipSting_In_Skill_Impact( const GLSKILL* glSkill, WORD wLevel )
    {
        std::string strText = "";
        std::vector<std::string> vecstrText;

        const SKILL::SAPPLY& sAPPLY = glSkill->m_sAPPLY;
        const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[wLevel];

        // Info : 부가 효과
        {
            SKILL::EMROLE emRole = glSkill->m_sBASIC.emROLE;

            if ( !sAPPLY.vecADDON.empty() )
            {
				for ( DWORD i = 0; i < sAPPLY.vecADDON.size(); ++i )
				{
					const float fADDON_VAR = sAPPLY.vecADDON[i].fADDON_VAR[wLevel];
					float fLife = sAPPLY.sDATA_LVL[ wLevel ].fLIFE;
					float fADDON_VAR_SCALE = fADDON_VAR * COMMENT::IMPACT_ADDON_SCALE[sAPPLY.vecADDON[i].emADDON];
					vecstrText.push_back(MakeTooltipSting_In_Skill_Impact( fLife, fADDON_VAR, fADDON_VAR_SCALE, emRole, sAPPLY.vecADDON[i].emADDON ));
				}
            }
        }
        return vecstrText;
    }

    std::string MakeTooltipSting_In_Skill_Impact(
        float fLife,
        const float fADDON_VAR, 
        float fADDON_VAR_SCALE, 
        SKILL::EMROLE emRole, 
        EMIMPACT_ADDON emADDON )
    {
        std::string strText = "";

        SKILL_INFOR_TYPE emInforType = INFOR_NONE_TYPE;
        if ( emRole == SKILL::EMROLE_NORMAL )
        {
            if ( fLife )
            {
                if ( fADDON_VAR > 0.0f )
                    emInforType = NORMAL_DURATION_PLUS;
                else if ( fADDON_VAR < 0.0f )
                    emInforType = NORMAL_DURATION_MINUS;
            }
            else
            {
                if ( fADDON_VAR > 0.0f )
                    emInforType = NORMAL_NONDURATION_PLUS;
                else if ( fADDON_VAR < 0.0f )
                    emInforType = NORMAL_NONDURATION_MINUS;
            }
        }
        else if ( emRole == SKILL::EMROLE_PASSIVE )
        {
            if (fLife )
            {
                if ( fADDON_VAR > 0.0f )
                    emInforType = PASSIVE_DURATION_PLUS;
                else if ( fADDON_VAR < 0.0f )
                    emInforType = PASSIVE_DURATION_MINUS;
            }
            else
            {
                if ( fADDON_VAR > 0.0f )
                    emInforType = PASSIVE_NONDURATION_PLUS;
                else if ( fADDON_VAR < 0.0f )
                    emInforType = PASSIVE_NONDURATION_MINUS;
            }
        }

        int nIndex = static_cast<int>(emADDON);
        int nINC = 1;
        std::string strInc= "";
        std::string strUItext = "";

        if ( fADDON_VAR > 0.0f )
        {
            nINC = 1;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				strInc = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_PLUS).GetString();
            else
				strInc = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_PLUS].c_str();
        }
        else if ( fADDON_VAR < 0.0f )
        {
            nINC = -1;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				strInc = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_PLUS).GetString();
			else
				strInc = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_MINUS].c_str();
        }

		BOOL bIsIMPACT_ADDON_PER = FALSE;
		std::string strIMPACT_ADDON;
		bIsIMPACT_ADDON_PER = COMMENT::IsIMPACT_ADDON_PER(emADDON);
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			strIMPACT_ADDON = CGameTextMan::GetInstance().GetCommentText("IMPACT_ADDON", emADDON).GetString();
		}
		else
		{
			strIMPACT_ADDON = COMMENT::IMPACT_ADDON[emADDON].c_str();
		}

        switch ( emADDON )
        {
        case EMIMPACTA_HP_ADD:   //  HP 회복력 증감
            {
                strUItext = sc::string::format( "SKILL_IMPACT_ADDON_%d", nIndex );
                strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, fADDON_VAR * nINC, strInc.c_str() );
            }
            break;
        case EMIMPACTA_MP_ADD:  //  MP 회복력 증감
            {
                strUItext = sc::string::format( "SKILL_IMPACT_ADDON_%d", nIndex );
                strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, fADDON_VAR * nINC, strInc.c_str()  );
            }
            break;
        case EMIMPACTA_SP_ADD:  //  SP 회복력 증감
            {
                strUItext = sc::string::format( "SKILL_IMPACT_ADDON_%d", nIndex );
                strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, fADDON_VAR * nINC, strInc.c_str()  );
            }
            break;
        case EMIMPACTA_CP_ADD:  //  CP 회복력 증감
            {
                strUItext = sc::string::format( "SKILL_IMPACT_ADDON_%d", nIndex );
                strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, fADDON_VAR * nINC, strInc.c_str()  );
            }
            break;
        case EMIMPACTA_CP_DUR:  //  CP 자동지속 증감
            {
                strUItext = sc::string::format( "SKILL_IMPACT_ADDON_%d", nIndex );
                strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, fADDON_VAR * nINC, strInc.c_str()  );
            }
            break;

        case EMIMPACTA_HITRATE:     // 명중율 변화량.
        case EMIMPACTA_AVOIDRATE:   // 회피율 변화량.
        case EMIMPACTA_DAMAGE:  // 공격치 변화량.
        case EMIMPACTA_DEFENSE: // 방어치 변화량.
        case EMIMPACTA_VARHP:   // HP 변화율.
        case EMIMPACTA_VARMP:   // MP 변화율.
        case EMIMPACTA_VARSP:   // SP 변화율.
        case EMIMPACTA_VARAP:   // HP,MP,SP 변화율.
        case EMIMPACTA_DAMAGE_RATE:     // 공격치 변화율.
        case EMIMPACTA_DEFENSE_RATE:    // 방어치 변화율.
        case EMIMPACTA_PA:      //	격투치.
        case EMIMPACTA_SA:      //	사격치.
        case EMIMPACTA_MA:      //	방어치.
        case EMIMPACTA_HP_RATE: //	HP 증폭율.
        case EMIMPACTA_MP_RATE: //	MP 증폭율.
        case EMIMPACTA_SP_RATE: //	SP 증폭율.
        case EMIMPACTA_RESIST:  //	저항 수치 변경
        case EMIMPACTA_STATS:   //	스탯 수치 변경 ( 힘,민첩,정신 )
            {
                switch ( emInforType )
                {
                case NORMAL_DURATION_PLUS:
                    {
                        if ( bIsIMPACT_ADDON_PER )
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4_1")
                                ,fLife, strIMPACT_ADDON.c_str(), fADDON_VAR_SCALE );
                        }
                        else
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE4")
                                ,fLife, strIMPACT_ADDON.c_str(), fADDON_VAR);
                        }
                    }
                    break;

                case NORMAL_DURATION_MINUS:
                    {
                        if ( bIsIMPACT_ADDON_PER )
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4_1")
                                ,fLife, strIMPACT_ADDON.c_str(), -fADDON_VAR_SCALE );
                        }
                        else
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE4")
                                ,fLife, strIMPACT_ADDON.c_str(), -fADDON_VAR );
                        }
                    }
                    break;

                case NORMAL_NONDURATION_PLUS:
                    {
                        if ( bIsIMPACT_ADDON_PER )
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5_1")
                                , strIMPACT_ADDON.c_str(), fADDON_VAR_SCALE );
                        }
                        else
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_INCREASE_CASE5")
                                , strIMPACT_ADDON.c_str(), fADDON_VAR );
                        }
                    }
                    break;

                case NORMAL_NONDURATION_MINUS:
                    {
                        if ( bIsIMPACT_ADDON_PER )
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5_1")
                                , strIMPACT_ADDON.c_str(), -fADDON_VAR_SCALE );
                        }
                        else
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_NORMAL_DECREASE_CASE5")
                                , strIMPACT_ADDON.c_str(), -fADDON_VAR );
                        }
                    }
                    break;

                case PASSIVE_DURATION_PLUS:
                    {
                        if ( bIsIMPACT_ADDON_PER )
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2_1")
                                ,fLife, strIMPACT_ADDON.c_str(), fADDON_VAR_SCALE );
                        }
                        else
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE2")
                                ,fLife, strIMPACT_ADDON.c_str(), fADDON_VAR );
                        }
                    }
                    break;

                case PASSIVE_DURATION_MINUS:
                    {
                        if ( bIsIMPACT_ADDON_PER )
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2_1")
                                ,fLife, strIMPACT_ADDON.c_str(), -fADDON_VAR_SCALE );
                        }
                        else
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE2")
                                ,fLife, strIMPACT_ADDON.c_str(), -fADDON_VAR );
                        }
                    }
                    break;

                case PASSIVE_NONDURATION_PLUS:
                    {
                        if ( bIsIMPACT_ADDON_PER )
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3_1")
                                , strIMPACT_ADDON.c_str(), fADDON_VAR_SCALE );
                        }
                        else
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_INCREASE_CASE3")
                                , strIMPACT_ADDON.c_str(), fADDON_VAR );
                        }
                    }
                    break;

                case PASSIVE_NONDURATION_MINUS:
                    {
                        if ( bIsIMPACT_ADDON_PER )
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3_1")
                                , strIMPACT_ADDON.c_str(), -fADDON_VAR_SCALE );
                        }
                        else
                        {
                            strText = sc::string::format( ID2GAMEINTEXT("SKILL_INFOR_PASSIVE_DECREASE_CASE3")
                                , strIMPACT_ADDON.c_str(), -fADDON_VAR);
                        }
                    }
                    break;
                };
            }
            break;
        };
        return strText;
    }
    std::vector<std::string> MakeTooltipSting_In_Skill_Special( const GLSKILL* glSkill, WORD wLevel, bool bIsTool )
    {
        const SKILL::SAPPLY& sAPPLY = glSkill->m_sAPPLY;
        const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[wLevel];
        EMIMPACT_TARGET emIMPACT_TAR = glSkill->m_sBASIC.emIMPACT_TAR;


        std::vector<std::string> vecstrText;

        // Info : 특수 효과
        {
            if ( !sAPPLY.vecSPEC.empty() )
            {
                for ( DWORD i = 0; i <  sAPPLY.vecSPEC.size(); ++i )
                {
                    std::string strText = "";

                    const SKILL::SSPEC& sSPEC = sAPPLY.vecSPEC[i].sSPEC[wLevel];
					float fVAR1_SCALE, fVAR2_SCALE;
					fVAR1_SCALE = sSPEC.fVAR1 * COMMENT::SPEC_ADDON_VAR1_SCALE[sAPPLY.vecSPEC[i].emSPEC];
					fVAR2_SCALE = sSPEC.fVAR2 * COMMENT::SPEC_ADDON_VAR2_SCALE[sAPPLY.vecSPEC[i].emSPEC];
                    float fLife = sAPPLY.sDATA_LVL[ wLevel ].fLIFE;
					const SKILL::SSPEC_CA& sSPECCA = sAPPLY.sCAData.sSPEC_CA[wLevel];

                    vecstrText.push_back( MakeTooltipSting_In_Skill_Special(fLife, fVAR1_SCALE, fVAR2_SCALE, sAPPLY.vecSPEC[i].emSPEC, sSPEC, emIMPACT_TAR, bIsTool, sSPECCA ) );

                }
            }
        }
        return vecstrText;
    }
    std::string MakeTooltipSting_In_Skill_Special( 
        float fLife,
        float fVAR1_SCALE,
        float fVAR2_SCALE,
        EMSPEC_ADDON emSPEC,
        const SKILL::SSPEC& sSPEC, EMIMPACT_TARGET emIMPACT_TAR,
		bool bIsTool,
		const SKILL::SSPEC_CA& sSPECCA )
    {
		std::string strText = "";
		std::string strUItext = "SKILL_SPEC_ADDON_";
            std::string strTemp = "";
            CString straddText1;
            CString straddText2;
            std::string str = "";
            int nIndex = static_cast<int>( emSPEC );

            switch ( emSPEC )
            {
			case EMSPECA_ACTIVATE_DAMAGE :
				{
					// 피해발동은 따로 출력함으로 [스킬 효과]에 출력되지않도록함;
					if ( bIsTool == false )
						break;

					if( fLife != 0.0f )
					{
						int iSubNum = 0;

						DWORD dwAttackCnt = (DWORD)(int)(fVAR1_SCALE);//sSPEC.nVAR1;
						DWORD dwDamageCnt = (DWORD)(int)(fVAR2_SCALE);//sSPEC.nVAR2;

						std::string szSkilName;
						
						PGLSKILL pSkill = GLogicData::GetInstance().GetSkillData(sSPEC.dwNativeID);

						if( pSkill ) {
							szSkilName = pSkill->GetNameStr().c_str();
						}
						else {
							szSkilName = "none";
						}

						iSubNum = ( dwAttackCnt > 0 && dwDamageCnt > 0 ) ? 3 : 0;

						if( iSubNum == 0 ) iSubNum = ( dwAttackCnt > 0 ) ? 1 : 0;
						if( iSubNum == 0 ) iSubNum = ( dwDamageCnt > 0 ) ? 2 : 0;
						
						switch( iSubNum )
						{
						case 0 :
							{
								strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
								strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife );
							}
							break;
						case 1 :
							{
								strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_%d", nIndex, iSubNum );
								strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, dwAttackCnt, szSkilName.c_str());
							}
							break;
						case 2 :
							{
								strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_%d", nIndex, iSubNum );
								strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, dwDamageCnt, szSkilName.c_str());
							}
							break;
						case 3 :
							{
								strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_%d", nIndex, iSubNum );
								strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, dwAttackCnt, dwDamageCnt, szSkilName.c_str());
							}
							break;
						}
					}
				}
				break;
			case EMSPECA_SPECIAL_IMMUNE :
				{
					DWORD dwValue = (DWORD)sSPEC.nVAR1;

					if( fLife != 0.0f && dwValue != 0 )
					{
						strTemp += sc::string::format("%1%", ID2GAMEWORD( "ITEM_INFO_SET_BONUS", dwValue ));	

						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, strTemp.c_str() );
					}
				}
				break;
			case EMSPECA_PROVOKE :
				{
					DWORD dwValue = (DWORD)(int)(fVAR1_SCALE);//(DWORD)sSPEC.fVAR1;

					if( fLife != 0.0f && dwValue != SNATIVEID::ID_NULL )
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, dwValue );
					}
				}
				break;
			case EMSPECA_DISGUISE   :
				{
					//if( fLife != 0.0f && sSPEC.fVAR1 > 0 )
					if( fLife != 0.0f && fVAR1_SCALE > 0 )
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife );
					}
				}
				break;
			case EMSPECA_CAMERA_HIT :
				{
					BOOL bShow = (fLife != 0.0f) ? TRUE : FALSE;

					//bShow = ( (sSPEC.fVAR1 == 0 && sSPEC.fVAR2 == 0) || !bShow ) ? FALSE : TRUE;
					//bShow = ( (fVAR1_SCALE == 0 && fVAR2_SCALE == 0) || !bShow ) ? FALSE : TRUE;
					//bShow = ( fVAR1_SCALE == 0 || !bShow ) ? FALSE : TRUE;

					int iVar1 = (int)fVAR1_SCALE;
					//int iVar2 = (int)fVAR2_SCALE;

					if( bShow )
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife );
						//strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, sSPEC.fVAR1, sSPEC.fVAR2);
					}
				}
				break;
			case EMSPECA_FEIGN_DEATH   :
				{
					if( fLife != 0.0f )
					{
						int iSubNum = 0;

						BOOL bCheckTarget = sSPEC.dwFLAG&FEIGN_DEATH_TARGETING_DISABLE;
						BOOL bBufDel	  = sSPEC.dwFLAG&FEIGN_DEATH_BUF_DEL;

						iSubNum = ( bCheckTarget && bBufDel > 0 ) ? 3 : 0;

						if( iSubNum == 0 ) iSubNum = ( bCheckTarget > 0 ) ? 1 : 0;
						if( iSubNum == 0 ) iSubNum = ( bBufDel > 0 ) ? 2 : 0;

						switch( iSubNum )
						{
						case 0 :
							{
								strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
								strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife );
							}
							break;
						case 1 :
							{
								std::string szIndex = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
								std::string szTemp  = sc::string::format( ID2GAMEINTEXT( szIndex.c_str() ), fLife );

								strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_%d", nIndex, iSubNum );
								strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), szTemp.c_str());
							}
							break;
						case 2 :
							{
								std::string szIndex = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
								std::string szTemp  = sc::string::format( ID2GAMEINTEXT( szIndex.c_str() ), fLife );

								strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_%d", nIndex, iSubNum );
								strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), szTemp.c_str());
							}
							break;
						case 3 :
							{
								std::string szIndex = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
								std::string szTemp  = sc::string::format( ID2GAMEINTEXT( szIndex.c_str() ), fLife );

								strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_%d", nIndex, iSubNum );
								strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), szTemp.c_str());
							}
							break;
						}
					}
				}
				break;
			case EMSPECA_ENTRANCE_EXIT :
				{
					//SNATIVEID sNative( sSPEC.fVAR1, sSPEC.fVAR2 );
					SNATIVEID sNative( fVAR1_SCALE, fVAR2_SCALE );

					if( sNative != NATIVEID_NULL() )
					{
						PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData ( sNative );

						if( pCrow != NULL )
						{							
							strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
							strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), pCrow->GetName() );
						}						
					}					
				}
				break;
			case EMSPECA_SCREEN_THROW :
				{
					if( fLife != 0.0f )
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife );
					}
				}
				break;
			case EMSPECA_COUNTATTACK :
				{
					if( fLife != 0.0f )
					{
						std::string strTemp;

						SNATIVEID sSkillID( sSPEC.fVAR1, sSPEC.fVAR2 );

						for( int i = 0; i < SKILL::MAX_ADDON; ++i )
						{
							if( sSPECCA.emSPEC[i] != EMSPECA_NULL && !sSPECCA.bAllApply ) {

								if( !strTemp.empty() ) strTemp += sc::string::format("%1%", ",");

								strTemp += sc::string::format("%1%", COMMENT::SPEC_ADDON[ sSPECCA.emSPEC[i] ].c_str());								
							}
						}

						if( sSkillID == NATIVEID_NULL() )
						{						
							if( !strTemp.empty() )
							{								
								// %1%초 동안, '@설정반격스킬효과@' 효과가 1회에 한해 반격이 발동됩니다.
								strText = sc::string::format( ID2GAMEINTEXT( "SKILL_SPEC_ADDON_102_1" ), fLife, strTemp.c_str() );

							}
							else
							{
								if( sSPECCA.bDamageApply && !sSPECCA.bAllApply )
								{
									// %1%초 동안, '모든 스킬 피해'를 1회에 한해 반격이 발동됩니다.
									strText = sc::string::format( ID2GAMEINTEXT( "SKILL_SPEC_ADDON_102_2" ), fLife, ID2GAMEWORD("SKILL_CA_EFFECT_SETTING", 0) );
								}
								else
								if( !sSPECCA.bDamageApply && sSPECCA.bAllApply )
								{
									// %1%초 동안, '모든 스킬 효과'를 1회에 한해 반격이 발동됩니다.
									strText = sc::string::format( ID2GAMEINTEXT( "SKILL_SPEC_ADDON_102_2" ), fLife, ID2GAMEWORD("SKILL_CA_EFFECT_SETTING", 1) );
								}
								else
								if( sSPECCA.bDamageApply && sSPECCA.bAllApply )
								{
									// %1%초 동안, '모든 스킬 피해, 효과'를 1회에 한해 반격이 발동됩니다.
									strText = sc::string::format( ID2GAMEINTEXT( "SKILL_SPEC_ADDON_102_2" ), fLife, ID2GAMEWORD("SKILL_CA_EFFECT_SETTING", 2) );
								}
							}							
						}
						else
						{
							SNATIVEID sID = SNATIVEID( static_cast<WORD>(sSPEC.fVAR1), static_cast<WORD>(sSPEC.fVAR2) );

							PGLSKILL pSkill = GLogicData::GetInstance().GetSkillData(sID);

							if( !strTemp.empty() )
							{
								// %1%초 동안, '@설정반격스킬효과@' 효과가 1회에 한해 반격하며, 반격 실패시 '@스킬명@'이 발동됩니다.
								strText = sc::string::format( ID2GAMEINTEXT( "SKILL_SPEC_ADDON_102" ), fLife, strTemp.c_str(), pSkill ? pSkill->GetNameStr().c_str() : " ");								
							}
							else {								
								if( sSPECCA.bDamageApply && !sSPECCA.bAllApply )
								{
									// %1%초 동안, '모든 스킬 피해'를 1회에 한해 반격하며, 반격 실패시 '@스킬명@'이 발동됩니다.
									strText = sc::string::format( ID2GAMEINTEXT( "SKILL_SPEC_ADDON_102_3" ), fLife, ID2GAMEWORD("SKILL_CA_EFFECT_SETTING", 0), pSkill ? pSkill->GetNameStr().c_str() : " ");									
								}
								else
								if( !sSPECCA.bDamageApply && sSPECCA.bAllApply )
								{
									// %1%초 동안, '모든 스킬 효과'를 1회에 한해 반격하며, 반격 실패시 '@스킬명@'이 발동됩니다.
									strText = sc::string::format( ID2GAMEINTEXT( "SKILL_SPEC_ADDON_102_3" ), fLife, ID2GAMEWORD("SKILL_CA_EFFECT_SETTING", 1), pSkill ? pSkill->GetNameStr().c_str() : " ");
								}
								else
								if( sSPECCA.bDamageApply && sSPECCA.bAllApply )
								{
									// %1%초 동안, '모든 스킬 피해, 효과'를 1회에 한해 반격하며, 반격 실패시 '@스킬명@'이 발동됩니다.
									strText = sc::string::format( ID2GAMEINTEXT( "SKILL_SPEC_ADDON_102_3" ), fLife, ID2GAMEWORD("SKILL_CA_EFFECT_SETTING", 2), pSkill ? pSkill->GetNameStr().c_str() : " ");
								}
							}							
						}						
					}
				}
				break;
            case EMSPECA_PUSHPULL://	당기기/밀기.
                {
					if ( fVAR2_SCALE == 0 )
						break;

                    if ( fVAR1_SCALE > 0.0f )
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    else
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_%d", nIndex, 1 );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                        fVAR2_SCALE, fVAR1_SCALE );
                }
                break;

            case EMSPECA_REFDAMAGE://	대미지 반사.
                {
                    if ( fLife != 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( "%s", ID2GAMEINTEXT( strUItext.c_str() ) );
                    }
                }
                break;

            case EMSPECA_REBIRTH://	부활.
                {
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                        fVAR1_SCALE );
                }
                break;

            case EMSPECA_HP_GATHER://	HP의 일정량을 빼앗음.
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                        fVAR1_SCALE );
                }
                break;

            case EMSPECA_MP_GATHER://	MP의 일정량을 빼앗음.
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                        fVAR1_SCALE );
                }
                break;

            case EMSPECA_SP_GATHER://	SP의 일정량을 빼앗음.
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                        fVAR1_SCALE );
                }
                break;

            case EMSPECA_HP_DIV://	HP의 일부를 나누어 줌.
                {
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( "%s", ID2GAMEINTEXT( strUItext.c_str() ) );
                }
                break;

            case EMSPECA_MP_DIV://	MP의 일부를 나누어 줌.
                {
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( "%s", ID2GAMEINTEXT( strUItext.c_str() ) );
                }
                break;

            case EMSPECA_SP_DIV://	SP의 일부를 나누어 줌.
                {
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( "%s", ID2GAMEINTEXT( strUItext.c_str() ) );
                }
                break;

            case EMSPECA_NONBLOW://	상태이상 방지.
                {
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_NUMB).GetString() );

						if ( sSPEC.dwFLAG & DIS_STUN )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_STUN).GetString() );

						if ( sSPEC.dwFLAG & DIS_STONE )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_STONE).GetString() );

						if ( sSPEC.dwFLAG & DIS_BURN )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_BURN).GetString() );

						if ( sSPEC.dwFLAG & DIS_FROZEN )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_FROZEN).GetString() );

						if ( sSPEC.dwFLAG & DIS_MAD )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_MAD).GetString() );

						if ( sSPEC.dwFLAG & DIS_POISON )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_POISON).GetString() );

						if ( sSPEC.dwFLAG & DIS_CURSE )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_CURSE).GetString() );
					}
					else
					{
						straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_NUMB].c_str() );

						if ( sSPEC.dwFLAG & DIS_STUN )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_STUN].c_str() );

						if ( sSPEC.dwFLAG & DIS_STONE )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_STONE].c_str() );

						if ( sSPEC.dwFLAG & DIS_BURN )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_BURN].c_str() );

						if ( sSPEC.dwFLAG & DIS_FROZEN )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_FROZEN].c_str() );

						if ( sSPEC.dwFLAG & DIS_MAD )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_MAD].c_str() );

						if ( sSPEC.dwFLAG & DIS_POISON )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_POISON].c_str() );

						if ( sSPEC.dwFLAG & DIS_CURSE )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_CURSE].c_str() );
					}
                    //' ,' 마지막 단어 콤마를 때기위한 코드
                    straddText1 = straddText1.Left(straddText1.GetLength()-2);
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    //strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), strTemp.c_str() );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), straddText1.GetString() );
                }
                break;

            case EMSPECA_RECBLOW://	상태이상 회복.
                {
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						if ( sSPEC.dwFLAG & DIS_NUMB )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_NUMB).GetString() );

						if ( sSPEC.dwFLAG & DIS_STUN )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_STUN).GetString() );

						if ( sSPEC.dwFLAG & DIS_STONE )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_STONE).GetString() );

						if ( sSPEC.dwFLAG & DIS_BURN )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_BURN).GetString() );

						if ( sSPEC.dwFLAG & DIS_FROZEN )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_FROZEN).GetString() );

						if ( sSPEC.dwFLAG & DIS_MAD )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_MAD).GetString() );

						if ( sSPEC.dwFLAG & DIS_POISON )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_POISON).GetString() );

						if ( sSPEC.dwFLAG & DIS_CURSE )
							straddText1.AppendFormat( "%s, ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_CURSE).GetString() );
					}
					else
					{
						if ( sSPEC.dwFLAG & DIS_NUMB )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_NUMB].c_str() );

						if ( sSPEC.dwFLAG & DIS_STUN )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_STUN].c_str() );

						if ( sSPEC.dwFLAG & DIS_STONE )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_STONE].c_str() );

						if ( sSPEC.dwFLAG & DIS_BURN )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_BURN].c_str() );

						if ( sSPEC.dwFLAG & DIS_FROZEN )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_FROZEN].c_str() );

						if ( sSPEC.dwFLAG & DIS_MAD )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_MAD].c_str() );

						if ( sSPEC.dwFLAG & DIS_POISON )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_POISON].c_str() );

						if ( sSPEC.dwFLAG & DIS_CURSE )
							straddText1.AppendFormat( "%s, ", COMMENT::BLOW[EMBLOW_CURSE].c_str() );
					}
                    //' ,' 마지막 단어 콤마를 때기위한 코드
                    straddText1 = straddText1.Left(straddText1.GetLength()-2);

                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    //strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), strTemp.c_str() );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), straddText1.GetString() );
                }
                break;

            case EMSPECA_PIERCE://	관통정도.
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    if ( fLife != 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife, fVAR1_SCALE );  
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR1_SCALE );
                    }
                }
                break;

            case EMSPECA_RANGE_PSY_LONG://	물리 사격형 사정거리 및 적용영역 증가( 감소 )
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    if ( fVAR1_SCALE > 0.0f )
                    {
                        if ( fLife != 0.0f )
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fLife, fVAR1_SCALE );
                        }
                        else
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fVAR1_SCALE );
                        }
                    }
                    else
                    {
                        if ( fLife != 0.0f )
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fLife, -fVAR1_SCALE );
                        }
                        else
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_3", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                -fVAR1_SCALE );
                        }
                    }
                }
                break;

            case EMSPECA_MOVEVELO://	이동속도.
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    if ( fVAR1_SCALE > 0.0f )
                    {
                        if ( fLife != 0.0f )
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fLife, fVAR1_SCALE );
                        }
                        else
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fVAR1_SCALE );
                        }
                    }
                    else
                    {
                        if ( fLife != 0.0f )
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fLife, -fVAR1_SCALE );
                        }
                        else
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_3", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                -fVAR1_SCALE );
                        }
                    }
                }
                break;

            case EMSPECA_ONWARD://	진격.
                {
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                        fVAR1_SCALE, fVAR2_SCALE );
                }
                break;

            case EMSPECA_INVISIBLE://	은신
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, fVAR1_SCALE );

                }
                break;

            case EMSPECA_RECVISIBLE://	은신 감지
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, fVAR1_SCALE );
                }
                break;

            case EMSPECA_ATTACKVELO://	공격속도.
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    if (fVAR1_SCALE < 0.0f )
                    {
                        if ( fLife != 0.0f )
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fLife, -fVAR1_SCALE );
                        }
                        else
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                -fVAR1_SCALE );
                        }
                    }
                    else
                    {
                        if ( fLife != 0.0f )
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fLife, fVAR1_SCALE );
                        }
                        else
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_3", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fVAR1_SCALE );
                        }
                    }
                }
                break;

            case EMSPECA_SKILLDELAY://	스킬 딜래이 시간 변화.
                {
					if ( fVAR2_SCALE == 0 )
						break;
                    std::string strTemptext1 = " ";
                    int nInc1 = 1;


                    if ( fVAR1_SCALE < 0 )
                    {
                        nInc1 = -1 ;

						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext1 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_MINUS).GetString();
						else
							strTemptext1 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_MINUS].c_str();
                        
                    }
                    else
                    {
                        nInc1 = 1 ;

						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext1 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_PLUS).GetString();
						else
							strTemptext1 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_PLUS].c_str();
                    }

                    if (sSPEC.dwFLAG == (EMSPEC_SKILL_ASING_DWORD_BEFORE|
                        EMSPEC_SKILL_ASING_DWORD_AFTER) )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR2_SCALE, fLife, fVAR1_SCALE * nInc1, strTemptext1  );
                    }

                    if (sSPEC.dwFLAG == EMSPEC_SKILL_ASING_DWORD_BEFORE )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR2_SCALE, fVAR1_SCALE * nInc1, strTemptext1 );
                    }
                      if (sSPEC.dwFLAG == EMSPEC_SKILL_ASING_DWORD_AFTER )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR2_SCALE, fLife, fVAR1_SCALE * nInc1, strTemptext1);
                    }
                }
                break;

            case EMSPECA_CRUSHING_BLOW://  강한타격 스킬
                {
					if ( fVAR2_SCALE == 0 )
						break;

					if( fVAR1_SCALE == 0 )
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fVAR2_SCALE );
					}
					else
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
							fVAR2_SCALE, GLCONST_CHAR::fPUSHPULL_VELO, fVAR1_SCALE );
					}
                }
                break;

            case EMSPECA_PSY_DAMAGE_REDUCE:// 물리 데미지 흡수율
                {
                    if ( fLife != 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife, fVAR1_SCALE );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR1_SCALE );
                    }
                }
                break;

            case EMSPECA_MAGIC_DAMAGE_REDUCE:// 매직 데미지 흡수율
                {
                    if ( fLife != 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife, fVAR1_SCALE );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR1_SCALE );
                    }
                }
                break;

            case EMSPECA_PSY_DAMAGE_REFLECTION:// 물리 데미지 반사율
                {
					if ( fVAR2_SCALE == 0 )
						break;
                    if ( fLife != 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife, fVAR2_SCALE, fVAR1_SCALE );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife, fVAR2_SCALE, fVAR1_SCALE );
                    }
                }
                break;

            case EMSPECA_MAGIC_DAMAGE_REFLECTION:// 매직 데미지 반사율
                {
					if ( fVAR2_SCALE == 0 )
						break;
                    if ( fLife != 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife, fVAR2_SCALE, fVAR1_SCALE );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR2_SCALE, fVAR1_SCALE );
                    }
                }
                break;

            case EMSPECA_EFFECT_REMOVE://	효과 제거 ( 일반,버프,디버프,리미트 종류별로 )
                {
                    if ( sSPEC.dwFLAG == SKILL::EMACTION_TYPE_NORMAL )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_0", nIndex );
                        strText = sc::string::format( "%s", ID2GAMEINTEXT( strUItext.c_str() ) );
                    }
                    else if ( sSPEC.dwFLAG == SKILL::EMACTION_TYPE_BUFF )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( "%s", ID2GAMEINTEXT( strUItext.c_str() ) );
                    }
                    else if ( sSPEC.dwFLAG == SKILL::EMACTION_TYPE_DEBUFF )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
                        strText = sc::string::format( "%s", ID2GAMEINTEXT( strUItext.c_str() ) );
                    }
                    else if ( sSPEC.dwFLAG == SKILL::EMACTION_TYPE_LIMIT )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_3", nIndex );
                        strText = sc::string::format( "%s", ID2GAMEINTEXT( strUItext.c_str() ) );
                    }
                }
                break;

            case EMSPECA_STUN://	스턴 효과
                {
                    if ( fLife != 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife );
                    }
                    /*else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( "%s", ID2GAMEINTEXT( strUItext.c_str() ) );
                    }*/
                }
                break;

            case EMSPECA_DEFENSE_SKILL_ACTIVE://	피격시 발동
                {
                    GLSKILL* pSkillTemp = GLSkillMan::GetInstance().GetData( sSPEC.dwNativeID );
                    if ( pSkillTemp )
                    {
                        strTemp = sc::string::format( "%s Lv%d", pSkillTemp->GetName(),sSPEC.dwFLAG );

                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fVAR1_SCALE );
                    }
                }
                break;

            case EMSPECA_PULLIN://	당겨 오기
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fVAR1_SCALE, fVAR2_SCALE );
                }
                break;

            case EMSPECA_KNOCKBACK://	밀어 내기
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                        fVAR1_SCALE, GLCONST_CHAR::fPUSHPULL_VELO, fVAR2_SCALE );
                }
                break;

            case EMSPECA_DURATION_DAMAGE://	지속데미지
                {
					if ( fLife == 0 )
						break;
                    if ( fVAR2_SCALE > 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife, fVAR1_SCALE, fVAR2_SCALE );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife, fVAR1_SCALE, fVAR2_SCALE * -1 );
                    }
                }
                break;

            case EMSPECA_CURSE://	저주
                {
                    if ( fLife != 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife, fVAR1_SCALE );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR1_SCALE );
                    }
                }
                break;

            case EMSPECA_TARRANGE_ALL://	스킬 사정거리 변화
                {
					if ( fVAR1_SCALE == 0 )
						break;
					std::string strTemptext1 = " ";
					int nInc1 = 1;


					if ( fVAR1_SCALE < 0 )
					{
						nInc1 = -1 ;

						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext1 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_MINUS).GetString();
						else
							strTemptext1 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_MINUS].c_str();
					}
					else
					{
						nInc1 = 1 ;

						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext1 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_PLUS).GetString();
						else
							strTemptext1 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_PLUS].c_str();
					}

                    if ( fLife != 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife, fVAR1_SCALE * nInc1, strTemptext1 );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR1_SCALE* nInc1, strTemptext1 );
                    }
                }
                break;

            case EMSPECA_APPLYRANGE_ALL://	스킬 적용영역 변화
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    if ( fVAR1_SCALE > 0.0f )
                    {
                        if ( fLife != 0.0f )
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fLife, fVAR1_SCALE );
                        }
                        else
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fVAR1_SCALE );
                        }
                    }
                    else
                    {
                        if ( fLife != 0.0f )
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                fLife, -fVAR1_SCALE );
                        }
                        else
                        {
                            strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_3", nIndex );
                            strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                                -fVAR1_SCALE );
                        }
                    }
                }
                break;

            case EMSPECA_NON_DRUG://	물약 사용 금지 ( 약품류 전체 금지 )
                {
                    if ( fLife != 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( "%s", ID2GAMEINTEXT( strUItext.c_str() ) );
                    }
                }
                break;

            case EMSPECA_NON_SKILL://	스킬 사용 금지 ( 약품류 전체 금지 )
                {
                    if ( fLife != 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( "%s", ID2GAMEINTEXT( strUItext.c_str() ) );
                    }
                }
                break;

            case EMSPECA_IGNORED_DAMAGE://	데미지 흡수(무시)
                {
					if ( DWORD(fVAR1_SCALE) != 0 && DWORD(fVAR2_SCALE) != 0 )
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
							fLife, fVAR1_SCALE, fVAR2_SCALE);
					}
					else if (DWORD(fVAR2_SCALE) != 0 )
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
							fLife, fVAR2_SCALE);
					}
					else if (DWORD(fVAR1_SCALE) != 0 )
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
							fLife, fVAR1_SCALE);
					}                    
                }
                break;

			case EMSPECA_EXP_RATE:	//  경험치 획득률
				{
					if ( fVAR1_SCALE == 0 )
						break;
					if ( fVAR1_SCALE > 0.0f )
					{
						if ( fLife != 0.0f )
						{
							strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
							strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
								fLife, fVAR1_SCALE );
						}
						else
						{
							strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
							strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
								fVAR1_SCALE );
						}
					}
					else
					{
						if ( fLife != 0.0f )
						{
							strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
							strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
								fLife, -fVAR1_SCALE );
						}
						else
						{
							strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_3", nIndex );
							strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
								-fVAR1_SCALE );
						}
					}
				}
				break;

                //봉주
            case EMSPECA_TELEPORT: //순간이동
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    if ( emIMPACT_TAR == TAR_SELF || emIMPACT_TAR == TAR_SPEC || emIMPACT_TAR == TAR_SELF_TOSPEC )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), 1 );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), 1 );
                    }
                }
                break;

            case EMSPECA_SWAPPOS: // 위치교대
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    if ( sSPEC.dwFLAG == EMSPEC_SWAPPOS_DIR_BOTH)
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fVAR1_SCALE );
                    }
                    else if ( sSPEC.dwFLAG == EMSPEC_SWAPPOS_DIR_SELF)
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fVAR1_SCALE );
                    }
                    else if ( sSPEC.dwFLAG == EMSPEC_SWAPPOS_DIR_TARGET)
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fVAR1_SCALE );
                    }
                }
                break;

            case EMSPECA_INCREASE_EFF: // 강화 효과
                {
					if ( ( fVAR1_SCALE == 0 ) && ( fVAR2_SCALE == 0 ) )
						break;
                    for ( int i = 0; i < SKILL::EMAPPLY_NSIZE; i++ )
                    {
                        if ( sSPEC.dwFLAG & ( 1 <<  i  ) )
                        {
							if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
								straddText1.AppendFormat("%s, ", CGameTextMan::GetInstance().GetCommentText("SKILL_APPLY", i).GetString() );
                            else
								straddText1.AppendFormat( "%s, ", COMMENT::SKILL_APPLY[i].c_str());
                        }
                    }

                    std::string strTemptext1 = " ";
                    std::string strTemptext2 = " ";
                    int nInc1 = 1;
                    int nInc2 = 1;

                    if ( fVAR1_SCALE > 0 )
					{
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext1 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_PLUS).GetString();
						else
							strTemptext1 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_PLUS].c_str();
					}
                    else
                    {
                        nInc1 = -1;

						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext1 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_MINUS).GetString();
						else
							strTemptext1 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_MINUS].c_str();

                    }

                    if ( fVAR2_SCALE > 0 )
					{
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext2 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_PLUS).GetString();
						else
							strTemptext2 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_PLUS].c_str();
					}
                    else
                    {
                        nInc2 = -1;
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext2 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_MINUS).GetString();
						else
							strTemptext2 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_MINUS].c_str();
                    }
                    
                    straddText1 = straddText1.Left(straddText1.GetLength()-2);
                
					if ( fVAR2_SCALE == 0 )
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
					strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
						fLife, straddText1, fVAR1_SCALE*nInc1,strTemptext1 );
					}
					else if ( fVAR1_SCALE == 0 )
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
					strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
						fLife, straddText1, fVAR2_SCALE * nInc2, strTemptext2 );
					}
					else
					{
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                        fLife, straddText1, fVAR1_SCALE*nInc1,strTemptext1, fVAR2_SCALE * nInc2, strTemptext2 );
					}
                }
                break;

            case EMSPECA_ILLUSION: // 허상효과
                {
					if ( fVAR2_SCALE == 0 )
						break;
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                        fLife );
                }
                break;

            case EMSPECA_VEHICLE_GETOFF: // 하차 효과
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    if ( fLife > 0 && fVAR2_SCALE == 1 )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR1_SCALE, fLife );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR1_SCALE );
                    }
                }
                break;

            case EMSPECA_ENDURE_VARY: // 지속시간 증감
                {
					if ( (fVAR1_SCALE == 0) && (fVAR2_SCALE == 0) )
						break;
                    for ( int i = 0; i < SKILL::EMACTION_TYPE_NSIZE; i++ )
                    {
                        if ( sSPEC.dwFLAG & ( 1 <<  i  ) )
                        {
							if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
								straddText1.AppendFormat("%s, ", CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE", i).GetString() );
							else
								straddText1.AppendFormat( "%s, ", COMMENT::SKILL_ACTION_TYPE[ i ].c_str());
                        }
                    }


                    std::string strTemptext1 = " ";
                    std::string strTemptext2 = " ";
                    int nInc1 = 1;
                    int nInc2 = 1;

                    if ( fVAR1_SCALE > 0 )
					{
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext1 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_PLUS).GetString();
						else
							strTemptext1 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_PLUS].c_str();
					}
                    else
                    {
                        nInc1 = -1;
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext1 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_MINUS).GetString();
						else
							strTemptext1 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_MINUS].c_str();
                    }

                    if ( fVAR2_SCALE > 0 )
					{
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext2 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_PLUS).GetString();
						else
							strTemptext2 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_PLUS].c_str();
					}
                    else
                    {
                        nInc2 = -1;
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							strTemptext2 = CGameTextMan::GetInstance().GetCommentText("SPEC_INC_TYPE", EMSPEC_INC_TYPE_MINUS).GetString();
						else
							strTemptext2 = COMMENT::SPEC_INC_TYPE[EMSPEC_INC_TYPE_MINUS].c_str();
                    }

                    straddText1 = straddText1.Left(straddText1.GetLength()-2);
                    straddText2 = straddText2.Left(straddText2.GetLength()-2);

                    if (  fVAR2_SCALE == 0  )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            straddText1, fVAR1_SCALE * nInc1,  strTemptext1 );
                    }
					else if ( fVAR1_SCALE == 0 )
					{
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
							fLife, straddText1, fVAR2_SCALE * nInc2,  strTemptext2 );
					}

                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            straddText1, fVAR1_SCALE * nInc1, strTemptext1, fLife, fVAR2_SCALE * nInc2, strTemptext2 );
                    }

                }
                break;

            case EMSPECA_STIGMA: // 낙인효과
                {
					if ( fVAR2_SCALE == 0 )
						break;
                    for ( int i = 0; i <EMSPEC_STIGMA_TYPE_DWORD_DAMAGE_SKILL; i++ )
                    {
                        if ( sSPEC.dwFLAG  & ( 1 <<  i   ) )
                        {
							if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
								straddText1.AppendFormat("%s, ", CGameTextMan::GetInstance().GetCommentText("SPEC_STIGMA_TYPE", i).GetString() );
							else
								straddText1.AppendFormat( "%s, ", COMMENT::SPEC_STIGMA_TYPE[ i ].c_str());
                        }
                    }
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, straddText1.GetString(), fVAR2_SCALE );
                }
                break;

            case EMSPECA_HALLUCINATE: // 환독효과
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    if ( sSPEC.dwFLAG == 1  )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife );
                    }
                }
                break;

            case EMSPECA_RELEASE_ENDURE: // 피격시 효과 해제
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),fVAR1_SCALE );
                }
                break;
            case EMSPECA_LINK_HP: // 고리 효과 ( HP 링크 )
                {
                    if ( fLife > 0 && fVAR2_SCALE == 1 )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fLife );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),fLife  );
                    }
                }
                break;

            case EMSPECA_RELEASE_PET: //  수독효과( 펫 해제 )
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    if (  fLife == 0 )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR1_SCALE );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),
                            fVAR1_SCALE, fLife );
                    }
                }
                break;

            case EMSPECA_IMMUNE: // 면역 효과
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    for ( int i = 0; i < SKILL::EMAPPLY_NSIZE; i++ )
                    {
                        if ( static_cast<DWORD>(fVAR1_SCALE) & ( 1 <<  i   ) )
                        {
							if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
								straddText1.AppendFormat("%s, ", CGameTextMan::GetInstance().GetCommentText("SKILL_APPLY", i).GetString() );
							else
								straddText1.AppendFormat( "%s, ", COMMENT::SKILL_APPLY[ i ].c_str());
                        }
                    }

                    for ( int i = 0; i < SKILL::EMACTION_TYPE_NSIZE; i++ )
                    {
                        if ( static_cast<DWORD>(fVAR2_SCALE) & ( 1 <<  i  ) )
                        {
							if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
								straddText1.AppendFormat("%s, ", CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE", i).GetString() );
							else
								straddText2.AppendFormat( "%s, ", COMMENT::SKILL_ACTION_TYPE[ i ].c_str());
                        }
                        
                    }
                    straddText1 = straddText1.Left(straddText1.GetLength()-2);
                    straddText2 = straddText2.Left(straddText2.GetLength()-2);


                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), 
                                                                                    fLife, 
                                                                                    straddText1.GetString(), 
                                                                                    straddText2.GetString() );
                }
                break;

            case EMSPECA_TURN_STATE_BATTLE: // 전투상태 효과 ( 전투 상태로 바꿈 )
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),fVAR1_SCALE, fVAR2_SCALE );
                }
                break;

            case EMSPECA_TURN_STATE_PEACE: // 전투상태 해제 ( 평화 상태로 바꿈 )
                {
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),1);
                }
                break;
            case EMSPECA_TAUNT: // 도발 효과
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                    strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),fVAR1_SCALE, fLife, fVAR2_SCALE );
                }
                break;

            case EMSPECA_DOMINATION: // 지배 효과
                {
					if ( fVAR1_SCALE == 0 )
						break;
                    if ( sSPEC.dwFLAG == 1  )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),fVAR1_SCALE, fLife, fVAR2_SCALE );
                    }
                    else
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),fVAR1_SCALE, fLife, fVAR2_SCALE );
                    }
                }
                break;
			case EMSPECA_RANDOM_EXP_RATE:				
				{
					SKILL::ExperiencePointSettings* const pExpSettings(SKILL::ExperiencePointSettings::getInstance());					
					const float fIncreaseRate(pExpSettings->getRate(DWORD(fVAR1_SCALE)));
					const DWORD nAccum = DWORD(fVAR2_SCALE);
					if ( fIncreaseRate == 0 || nAccum == 0 )
						break;
					
					if ( nAccum == 1 )					
					{						
						if ( fIncreaseRate > 0.0f )
						{
							strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", EMSPECA_EXP_RATE);
							strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),fIncreaseRate );
						}
						else
						{
							strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_3", EMSPECA_EXP_RATE);
							strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),fIncreaseRate );
						}
					}
					else
					{
						if ( fIncreaseRate > 0.0f )
						{
							strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", EMSPECA_RANDOM_EXP_RATE);
							strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),fIncreaseRate, nAccum );
						}
						else
						{
							strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_3", EMSPECA_RANDOM_EXP_RATE);
							strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ),fIncreaseRate, nAccum );
						}
					}
				}
				break;
			case EMSPECA_SKILL_LINK:
				{
					if ( bIsTool == false )
						break;
					if ( fVAR1_SCALE == 0 )
						break;
					strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
					strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fVAR1_SCALE*100 );
				}
				break;
			case EMSPECA_DAMAGE_VALUE:
			case EMSPECA_DEFENSE_VALUE:
			case EMSPECA_HIT_VALUE:
			case EMSPECA_AVOID_VALUE:
			case EMSPECA_HP_VALUE:
			case EMSPECA_MP_VALUE:
			case EMSPECA_SP_VALUE:
			case EMSPECA_CP_VALUE:
			case EMSPECA_ATTACK_RANGE_VALUE:
			case EMSPECA_DIS_SP_VALUE:
			case EMSPECA_RESIST_VALUE:
			case EMSPECA_STATS_POW_VALUE:
			case EMSPECA_STATS_STR_VALUE:
			case EMSPECA_STATS_SPI_VALUE:
			case EMSPECA_STATS_DEX_VALUE:
			case EMSPECA_STATS_STA_VALUE:
			case EMSPECA_PA_VALUE:
			case EMSPECA_SA_VALUE:
			case EMSPECA_MA_VALUE:
			case EMSPECA_HP_POTION_INC_VALUE:
			case EMSPECA_MP_POTION_INC_VALUE:
			case EMSPECA_SP_POTION_INC_VALUE:
			case EMSPECA_CP_INC_VALUE:
			case EMSPECA_MOVEVELO_VALUE:
				{
					if ( fVAR1_SCALE < 0.0f )
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
					else
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );

					strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), int(fVAR1_SCALE) );
				}
				break;
			case EMSPECA_DAMAGE_RATE:
			case EMSPECA_DEFENSE_RATE:
			case EMSPECA_HIT_RATE:
			case EMSPECA_AVOID_RATE:
			case EMSPECA_HP_VARIATION_RATE:
			case EMSPECA_MP_VARIATION_RATE:
			case EMSPECA_SP_VARIATION_RATE:
			case EMSPECA_HMS_VARIATION_RATE:
			case EMSPECA_DAMAGE_RATE_ALL:
				{
					if ( fVAR1_SCALE < 0.0f )
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
					else
						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );

					strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fVAR1_SCALE );
				}
				break;

			case EMSPECA_VIEW_RANGE_RATE: // 시야 제한;
				{
					if ( int(fVAR1_SCALE) >= 1 )
						break;
					else if ( fVAR1_SCALE < 0.0f )
						break;
					const int _maxViewRange(static_cast<int>((fVAR1_SCALE * 100.0f)));
					strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
					strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), _maxViewRange );
				}
				break;

			case EMSPECA_AIRBORNE:				// 에어본;
				{
					if ( fLife <= 0.0f )
						break;
					strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
					strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife );
				}

				break;
			case EMSPECA_SUMMON_CHANGE_SKILLPAGE:		// 소환수 스킬 페이지 변경;
				//strText = "소환수 스킬 페이지";
				break;
			case EMSPECA_SUMMON_EVENT_SKILL:			// 소환수 이벤트 스킬;
				//strText =  "소환수 이벤트 스킬";
				break;
			case EMSPECA_SUMMON_TARGETING:				// 소환수 타겟 지정;
				//strText =  "소환수 타겟 지정";
				break;
			case EMSPECA_SUMMON_RUNAWAY:				// 소환수 도망;
				//strText =  "소환수 도망";
				break;
			case EMSPECA_DELAY_ACTION_SKILL:			// 지연발동
				{
					if ( bIsTool == false )
						break;

					if ( fLife <= 0.0f )
						break;
					strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
					GLSKILL* pGlSkill = GLogicData::GetInstance().GetSkillData( 
						SNATIVEID( static_cast<WORD>(sSPEC.fVAR1), static_cast<WORD>(sSPEC.fVAR2 )) );
					std::string strSkillname ="";
					if ( pGlSkill )
						strSkillname = pGlSkill->GetNameStr();
					strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, strSkillname.c_str() );
				}
				break;
			case EMSPECA_CLONE:							// 복제효과
				{
					if ( fLife <= 0.0f )
						break;
					strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
					strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), fLife, sSPEC.fVAR1  );
				}
				break;
            case EMSPECA_MOVEVELO_LIMIT:
                {
					//  [3/20/2015 gbgim] - RM# 1028;
					// 기존 0%~100%으로 출력하던걸 -100%~0%로 바꿔달라고 요청이옴;
					// 원하는 의도와 문자열 맥락이 서로 맞지않다고 얘기했지만 먹히지않음;
                    if ( sSPEC.fVAR1 >= 0.0f )
                    {            
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_1", nIndex );
                        std::string strTemp = sc::string::format(ID2GAMEINTEXT(strUItext.c_str()), fVAR1_SCALE-100.0f);
                        straddText1.AppendFormat( "%s ", strTemp.c_str() );
                    }
                    if ( sSPEC.fVAR2 >= 0.0f )
                    {
                        strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d_2", nIndex );
                        std::string strTemp = sc::string::format(ID2GAMEINTEXT(strUItext.c_str()), fVAR2_SCALE-100.0f);
                        straddText1.AppendFormat( "%s ", strTemp.c_str() );
                    }
                    strText = straddText1.GetString();
                }
                break;

			case EMSPECA_REMOVE_SKILL_EFFECT:
				{
					DWORD dwValue = (DWORD)sSPEC.nVAR1;

					if ( dwValue != 0 )
					{
						strTemp += sc::string::format( "%1%", ID2GAMEWORD( "ITEM_INFO_SET_BONUS", dwValue ) );	

						strUItext = sc::string::format( "SKILL_SPEC_ADDON_%d", nIndex );
						strText = sc::string::format( ID2GAMEINTEXT( strUItext.c_str() ), strTemp.c_str() );
					}
				}
				break;
            };

		return strText;
	}

	std::vector<std::string> MakeTooltipSting_In_Skill_Link( const GLSKILL* glSkill, WORD wLevel )
	{
		const SKILL::SAPPLY& sAPPLY = glSkill->m_sAPPLY;
		const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[wLevel];
		EMIMPACT_TARGET emIMPACT_TAR = glSkill->m_sBASIC.emIMPACT_TAR;


		std::vector<std::string> vecstrText;

		if ( !sAPPLY.vecSPEC.empty() )
		{
			for ( DWORD i = 0; i <  sAPPLY.vecSPEC.size(); ++i )
			{
				std::string strText = "";

				if( sAPPLY.vecSPEC[i].emSPEC != EMSPECA_SKILL_LINK )
					continue;

				SNATIVEID sLinkID = sAPPLY.vecSPEC[i].sSPEC[wLevel].dwLinkID;
				if( sLinkID == NATIVEID_NULL() )
					continue;

				SNATIVEID sNativeID = sAPPLY.vecSPEC[i].sSPEC[wLevel].dwNativeID;
				if( sNativeID == NATIVEID_NULL() )
					continue;

				GLSKILL* pSLinkSkill = GLSkillMan::GetInstance().GetData( sLinkID.wMainID, sLinkID.wSubID );
				if( !pSLinkSkill )
					continue;

				GLSKILL* pSNativeSkill = GLSkillMan::GetInstance().GetData( sNativeID.wMainID, sNativeID.wSubID );
				if( !pSNativeSkill )
					continue;

				strText = sc::string::format( ID2GAMEINTEXT( "SKILL_SPEC_LINK" ),
											  pSLinkSkill->GetNameStr() ,					// 조건부스킬;
											  sAPPLY.vecSPEC[i].sSPEC[wLevel].fVAR1*100,	// 확률;
											  pSNativeSkill->GetNameStr() );				// 발동 스킬;
				vecstrText.push_back( strText );
			}
		}
		return vecstrText;
    }


	// 등급/속성;
	bool MakeTooltipString_In_Skill_Grade (
		std::string& strText,
		GLSKILL* pSkill )
	{
		if ( !pSkill )
			return false;

		strText = "";
		strText = sc::string::format( "%d%s", pSkill->m_sBASIC.dwGRADE, ID2GAMEWORD( "SKILL_BASIC_INFO", 1 ) );
		strText += "/";

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			strText += sc::string::format( " %s", CGameTextMan::GetInstance().GetText("ELEMENT", pSkill->m_sAPPLY.emELEMENT, CGameTextMan::EM_COMMENT_TEXT).GetString());
		}
		else
		{
			strText += sc::string::format( " %s", COMMENT::ELEMENT[ pSkill->m_sAPPLY.emELEMENT ] );
		}

		return true;
	}

	// 적용 타입;
	bool MakeTooltipString_In_Skill_ApplyType (
		std::string& strText,
		GLSKILL* pSkill )
	{
		if ( !pSkill )
			return false;

		bool bDisplay = false;
		strText = "";

		// Info : 적용 타입
		switch ( pSkill->m_sBASIC.emACTION_TYPE )
		{
		case SKILL::EMACTION_TYPE_NORMAL:
			{
				strText = sc::string::format( "%s",
					ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 0 ) );
				bDisplay = true;
			}
			break;

		case SKILL::EMACTION_TYPE_BUFF:
			{
				strText = sc::string::format( "%s",
					ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 1 ) );
				bDisplay = true;
			}
			break;

		case SKILL::EMACTION_TYPE_DEBUFF:
			{
				strText = sc::string::format( "%s",
					ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 2 ) );
				bDisplay = true;
			}
			break;

		case SKILL::EMACTION_TYPE_LIMIT:
			{
				strText = sc::string::format( "%s",
					ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 3 ) );
				bDisplay = true;
			}
			break;
		}

		if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_NORMAL )
		{
			switch ( pSkill->m_sBASIC.emAPPLY )
			{
			case SKILL::EMAPPLY_MELEE:
				{
					strText += sc::string::format( "/ %s",
						ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 4 ) );
					bDisplay = true;
				}
				break;

			case SKILL::EMAPPLY_RANGE:
				{
					strText += sc::string::format( "/ %s",
						ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 5 ) );
					bDisplay = true;
				}
				break;

			case SKILL::EMAPPLY_MAGIC:
				{
					strText += sc::string::format( "/ %s",
						ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 6 ) );
					bDisplay = true;
				}
				break;
			}
		}
		else if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
		{
			strText += ID2GAMEWORD( "SKILL_BASIC_INFO", 8 );
			bDisplay = true;
		}

		return bDisplay;
	}

	// 사용 무기;
	bool MakeTooltipString_In_Skill_UseWeapon (
		std::string& strText,
		GLSKILL* pSkill )
	{
		if ( !pSkill )
			return false;

		bool bDisplay = false;
		strText = "";

		const SKILL::SBASIC& sBASIC = pSkill->m_sBASIC;

		if ( ( sBASIC.emUSE_LITEM && (sBASIC.emUSE_LITEM != SKILL::SKILLATT_NOCARE ) ) || 
			( sBASIC.emUSE_RITEM && (sBASIC.emUSE_RITEM != SKILL::SKILLATT_NOCARE ) ) )
		{
			bool bDistinction = false;
			if ( sBASIC.emUSE_LITEM )
			{
				if ( sBASIC.emUSE_LITEM != SKILL::SKILLATT_NOCARE )
				{
					bDistinction = true;

					strText += sc::string::format( "%s",
						ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM",
						sBASIC.emUSE_LITEM - 1 ) );
					bDisplay = true;
				}
			}

			if ( sBASIC.emUSE_RITEM )
			{
				if ( sBASIC.emUSE_RITEM != SKILL::SKILLATT_NOCARE )
				{
					if ( bDistinction )
						strText += "/";

					strText += sc::string::format( "%s",
						ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM",
						sBASIC.emUSE_RITEM - 1 ) );
					bDisplay = true;
				}
			}
		}

		return bDisplay;
	}

	// 소모 조건;
	bool MakeTooltipString_In_Skill_Requirement (
		std::string& strText,
		GLSKILL* pSkill,
		WORD wLevel )
	{
		if ( !pSkill )
			return false;

		bool bDisplay = false;
		strText = "";

		//	사용시 소진량
		if ( pSkill->m_sBASIC.emROLE != SKILL::EMROLE_PASSIVE )
		{
			const SKILL::SAPPLY& sAPPLY = pSkill->m_sAPPLY;
			const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[ wLevel ];

			int nIndex = 0;

			//	화살 개수
			if ( 0 < sDATA_LVL.wUSE_ARROWNUM )
			{
				strText += sc::string::format("%s %d ",
					ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 0 ),
					sDATA_LVL.wUSE_ARROWNUM );
				bDisplay = true;
			}

			//	부적 개수
			if ( 0 < sDATA_LVL.wUSE_CHARMNUM )
			{
				strText += sc::string::format("%s %d ",
					ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 1 ),
					sDATA_LVL.wUSE_CHARMNUM );
				bDisplay = true;
			}

			//	총알 개수
			if ( 0 < sDATA_LVL.wUSE_BULLETNUM )
			{
				strText += sc::string::format("%s %d ",
					ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 7 ),
					sDATA_LVL.wUSE_BULLETNUM );
				bDisplay = true;
			}

			// 배터리 소모량
			if ( 0 < sDATA_LVL.nUSE_BATTERY )
			{
				strText += sc::string::format("%s %d ",
					ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 7 ),
					sDATA_LVL.nUSE_BATTERY);
				bDisplay = true;
			}

			//	HP 소진량
			if ( 0 < sDATA_LVL.wUSE_HP )
			{
				strText += sc::string::format("%s %d ",
					ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 3 ),
					sDATA_LVL.wUSE_HP);
				bDisplay = true;
			}

			//	MP 소진량
			if ( 0 < sDATA_LVL.wUSE_MP )
			{
				strText += sc::string::format("%s %d ",
					ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 4 ),
					sDATA_LVL.wUSE_MP);
				bDisplay = true;
			}

			//	SP 소진량
			if ( 0 < sDATA_LVL.wUSE_SP )
			{
				strText += sc::string::format("%s %d ",
					ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 5 ),
					sDATA_LVL.wUSE_SP);
				bDisplay = true;
			}

			//	CP 소진량
			if ( 0 < sDATA_LVL.wUSE_CP )
			{
				strText += sc::string::format("%s %d ",
					ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 6 ),
					sDATA_LVL.wUSE_CP);
				bDisplay = true;
			}
		}

		return bDisplay;
	}

	// 재사용 시간;
	bool MakeTooltipString_In_Skill_ReuseTime (
		std::string& strText,
		GLSKILL* pSkill,
		WORD wLevel )
	{
		if ( !pSkill )
			return false;

		bool bDisplay = false;
		strText = "";

		const SKILL::SAPPLY& sAPPLY = pSkill->m_sAPPLY;
		const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[wLevel];

		if ( 0 < sDATA_LVL.fDELAYTIME )
		{
			strText = sc::string::format("%2.1f%s",
				sDATA_LVL.fDELAYTIME,
				ID2GAMEWORD( "WAITSERVER_TIMELEFT_UNIT", 0 ) );
			bDisplay = true;
		}

		return bDisplay;
	}

	// 스킬 영역;
	bool MakeTooltipString_In_Skill_SkillScope (
		std::string& strText,
		GLSKILL* pSkill,
		WORD wLevel,
		GLCHARLOGIC* pTarget )
	{
		if ( !pSkill )
			return false;

		bool bDisplay = false;
		strText = "";

		const SKILL::SBASIC& sBASIC = pSkill->m_sBASIC;
		const SKILL::SAPPLY& sAPPLY = pSkill->m_sAPPLY;

		const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[wLevel];

		switch ( pSkill->m_sBASIC.emIMPACT_TAR )
		{
		case TAR_SELF: // 자신;
			switch ( pSkill->m_sBASIC.emIMPACT_REALM )
			{
			case REALM_SELF: // '목표' 자신;
				strText += sc::string::format( "%s", ID2GAMEINTEXT( "SKILL_AREA_SELF_SELF" ) );
				bDisplay = true;
				break;

			case REALM_ZONE: // '목표' 주위;
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case EMIMPACT_SIDE_PARTY: // 자기편에게;
					if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
					{
						strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_SELF_ZONE_OUR" ),
							sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
						bDisplay = true;
					}
					break;

				case EMIMPACT_SIDE_ENEMY: // 적에게;
					if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
					{
						strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_SELF_ZONE_ENEMY" ),
							sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
						bDisplay = true;
					}
					break;
				case EMIMPACT_SIDE_ANYBODY: // 모두에게;
					if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
					{
						strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_SELF_ZONE_ANYBODY" ),
							sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
						bDisplay = true;
					}
					break;
				};
			};
			break;

		case TAR_SPEC: // 타겟;
			{
				switch ( pSkill->m_sBASIC.emIMPACT_REALM )
				{
				case REALM_SELF: // '목표' 자신;
					switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
					{
					case EMIMPACT_SIDE_PARTY: // 자기편에게;
						if ( 0 < sBASIC.wTARRANGE )
						{
							strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_SELF_OUR" ),
								sBASIC.wTARRANGE * 0.1f );
							bDisplay = true;
						}
						break;
					case EMIMPACT_SIDE_ENEMY: // 적에게;
						if ( 0 < sBASIC.wTARRANGE )
						{
							strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_SELF_ENEMY" ),
								sBASIC.wTARRANGE * 0.1f );
							bDisplay = true;
						}
						break;
					case EMIMPACT_SIDE_ANYBODY: // 모두에게;
						if ( 0 < sBASIC.wTARRANGE )
						{
							strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_SELF_ANYBODY" ),
								sBASIC.wTARRANGE * 0.1f );
							bDisplay = true;
						}
						break;
					};

				case REALM_ZONE: // '목표' 주위;
					switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
					{
					case EMIMPACT_SIDE_PARTY: // 자기편에게;
						if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
						{
							strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_ZONE_OUR" ),
								sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
							bDisplay = true;
						}
						break;
					case EMIMPACT_SIDE_ENEMY: // 적에게;
						if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
						{
							strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_ZONE_ENEMY" ),
								sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
							bDisplay = true;
						}
						break;
					case EMIMPACT_SIDE_ANYBODY: // 모두에게;
						if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
						{
							strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_ZONE_ANYBODY" ),
								sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
							bDisplay = true;
						}
						break;
					};

				case REALM_FANWIZE:
					switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
					{
					case EMIMPACT_SIDE_ENEMY:
						if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYANGLE && 0 < sDATA_LVL.wTARNUM )
						{
							strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_FANWIZE_ENEMY" ),
								sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYANGLE, sDATA_LVL.wTARNUM );
							bDisplay = true;
						}
						break;
					};
				};
			}
			break;

		case TAR_SELF_TOSPEC: // 자신부터 타겟까지;
			switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
			{
			case EMIMPACT_SIDE_ENEMY:
				WORD wPierce = sDATA_LVL.wPIERCENUM + pTarget->GETSUM_PIERCE() + 1;

				if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wTARNUM && 0 < wPierce )
				{
					strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TOSPEC_ENEMY" ),
						sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wTARNUM, wPierce );
					bDisplay = true;
				}
				break;
			};
			break;

		case TAR_ZONE: // 지면 위치의 주변 타겟;
			switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
			{
			case EMIMPACT_SIDE_PARTY:
				if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
				{
					strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_ZONE_OUR" ),
						sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
					bDisplay = true;
				}
				break;
			case EMIMPACT_SIDE_ENEMY:
				if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
				{
					strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_ZONE_ENEMY" ),
						sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
					bDisplay = true;
				}
				break;
			case EMIMPACT_SIDE_ANYBODY:
				if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
				{
					strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_ZONE_ANYBODY" ),
						sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
					bDisplay = true;
				}
				break;
			};
			break;

		case TAR_LAND: // 지면 위치;
			strText += sc::string::format( "%s", ID2GAMEINTEXT( "SKILL_AREA_LAND" ) );
			bDisplay = true;
			break;
		}

		return bDisplay;
	}

	// 스킬 효과;
	bool MakeTooltipString_In_Skill_SkillEffect (
		std::string& strText,
		GLSKILL* pSkill,
		WORD wLevel,
		GLCHARLOGIC* pTarget )
	{
		if ( !pSkill )
			return false;

		bool bDisplay = false;
		strText = "";

		const SKILL::SAPPLY& sAPPLY = pSkill->m_sAPPLY;
		const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[ wLevel ];

		int effectCount = 0;
		if( GLSkillToolTip::MakeTooltipSting_In_Skill_Type(
			pTarget, pSkill, wLevel) != "" )
		{
			++effectCount;
			strText = sc::string::format( "%d. %s\n\n", effectCount,
				GLSkillToolTip::MakeTooltipSting_In_Skill_Type(
				pTarget, pSkill, wLevel ) );
			bDisplay = true;
		}

		std::vector<std::string> skillImpact =
			GLSkillToolTip::MakeTooltipSting_In_Skill_Impact(
			pSkill, wLevel );
		BOOST_FOREACH( std::string& addStr, skillImpact )
		{
			++effectCount;
			strText += sc::string::format( "%d. %s", effectCount, addStr.c_str() );
			strText += "\r\n\r\n";
			bDisplay = true;
		}

		std::vector<std::string> skillSpecial =
			GLSkillToolTip::MakeTooltipSting_In_Skill_Special(
			pSkill, wLevel, true );
		BOOST_FOREACH( std::string& addStr, skillSpecial )
		{
			++effectCount;
			strText += sc::string::format( "%d. %s", effectCount, addStr.c_str() );
			strText += "\r\n\r\n";
			bDisplay = true;
		}

		return bDisplay;
	}

	// 상태 이상;
	bool MakeTooltipString_In_Skill_Blow (
		std::string& strText,
		GLSKILL* pSkill,
		WORD wLevel )
	{
		if ( !pSkill )
			return false;

		bool bDisplay = false;
		strText = "";

		std::vector< std::string > stateExplain =
			GLSkillToolTip::MakeTooltipSting_In_Skill_Special(
			pSkill, wLevel, true );

		for ( DWORD i=0 ; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
		{
			std::string strName;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				strName = CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON", pSkill->m_sAPPLY.vecSPEC[i].emSPEC).GetString();
			}
			else
			{
				strName = 
					COMMENT::SPEC_ADDON[ pSkill->m_sAPPLY.vecSPEC[i].emSPEC ];
			}

			strText += sc::string::format( "%s. ", i+1 );
			strText += strName;
			strText += " : ";
			strText += stateExplain.at( i );
			strText += "\n\n";
			bDisplay = true;
		}

		return bDisplay;
	}

}
