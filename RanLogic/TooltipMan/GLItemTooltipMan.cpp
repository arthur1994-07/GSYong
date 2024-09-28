#include "pch.h"

#include "../RanLogic/GLogicData.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/GLUseFeatures.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "./GLItemTooltipMan.h"

namespace Tooltip
{

	GLItemTooltipMan::GLItemTooltipMan ()
		: m_pItemData( NULL ) { }
	GLItemTooltipMan::~GLItemTooltipMan () { }

	void GLItemTooltipMan::Clear ()
	{
		m_pItemData = NULL;
		m_strItemDescription.clear();
	}

	void GLItemTooltipMan::GenerateItemTooltip ()
	{
		MakeItemDescriptionBasicInfo();
		
		if ( MakeItemDescriptionRequirement() )
			NextLine();
		if ( MakeItemDescriptionType() )
			NextLine();

		if ( MakeItemDescriptionAttack() )
			NextLine();
		if ( MakeItemDescriptionDefense() )
			NextLine();
		if ( MakeItemDescriptionEtc() )
			NextLine();

		if ( MakeItemDescriptionAddValue() )
			NextLine();
		if ( MakeItemDescriptionResist() )
			NextLine();
		if ( MakeItemDescriptionBlow() )
			NextLine();

		MakeItemDescriptionSetItem();
	}

	void GLItemTooltipMan::NextLine ()
	{
		m_strItemDescription += '\n';
	}

	void GLItemTooltipMan::CutZero ( CString& cstr, int nCount )
	{
		// '.' ���� ���ٴ� ���� �Ҽ��� ���� �ڸ��� �����Ƿ� ����;
		if (cstr.Find('.') == -1)
			return;

		CString temp = cstr;

		//ncount �Ҽ��� ��° �ڸ� ���� �߶��� ���Ѵ�;
		for (int i = 0; i < nCount + 1; i++ )
		{
			temp = cstr.Right( 1 );

			if ( temp.Find('0') != -1 || temp.Find('.') != -1 )
				cstr = cstr.Left( cstr.GetLength() - 1 );
			else
				break;
		}
	}

	void GLItemTooltipMan::MakeItemDescriptionBasicInfo ()
	{
		if ( !m_pItemData )
			return;

		MakeItemDescriptionSchool();
		MakeItemDescriptionClass();
		MakeItemDescriptionSex();

		NextLine();
	}

	void GLItemTooltipMan::MakeItemDescriptionSchool ()
	{
		if ( !m_pItemData )
			return;

		m_strItemDescriptionPiece = "";

		if ( m_pItemData->sBasicOp.dwReqSchool==GLSCHOOL_ALL )
		{
			m_strItemDescriptionPiece += ID2GAMEWORD( "ACADEMY_NAME", 3 );
			m_strItemDescriptionPiece += "/";
		}
		else
		{
			if ( m_pItemData->sBasicOp.dwReqSchool & GLSCHOOL_00)
			{
				m_strItemDescriptionPiece += GLCONST_CHAR::strSCHOOLNAME[school2index(GLSCHOOL_00)].c_str();
				m_strItemDescriptionPiece += "/";
			}

			if ( m_pItemData->sBasicOp.dwReqSchool & GLSCHOOL_01)
			{
				m_strItemDescriptionPiece += GLCONST_CHAR::strSCHOOLNAME[school2index(GLSCHOOL_01)].c_str();
				m_strItemDescriptionPiece += "/";
			}

			if ( m_pItemData->sBasicOp.dwReqSchool & GLSCHOOL_02)
			{
				m_strItemDescriptionPiece += GLCONST_CHAR::strSCHOOLNAME[school2index(GLSCHOOL_02)].c_str();
				m_strItemDescriptionPiece += "/";
			}
		}

		m_strItemDescription += m_strItemDescriptionPiece;
	}

	void GLItemTooltipMan::MakeItemDescriptionClass ()
	{
		if ( !m_pItemData )
			return;

		m_strItemDescriptionPiece = "";

		DWORD dwReqCharClass = m_pItemData->sBasicOp.dwReqCharClass;
		if( dwReqCharClass )
		{
			if ( dwReqCharClass==GLCC_ALL_ACTOR
				|| (dwReqCharClass == GLCC_MAN)
				|| (dwReqCharClass == GLCC_WOMAN) )
			{
				m_strItemDescriptionPiece += ID2GAMEWORD("ITEM_ALL_CLASS");
			}
			else
			{
				bool bDistinction = false;
				if ( (dwReqCharClass & GLCC_FIGHTER_M) 
					|| (dwReqCharClass & GLCC_FIGHTER_W) )
				{
					if ( bDistinction )
						m_strItemDescriptionPiece += "/";
					bDistinction = true;

					m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_FIGHTER_M ) );
				}

				if ( (dwReqCharClass & GLCC_ARMS_M) 
					|| (dwReqCharClass & GLCC_ARMS_W) )
				{
					if ( bDistinction )
						m_strItemDescriptionPiece += "/";
					bDistinction = true;

					m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARMS_M ) );
				}

				if ( (dwReqCharClass & GLCC_ARCHER_M)
					|| (dwReqCharClass & GLCC_ARCHER_W) )
				{
					if ( bDistinction )
						m_strItemDescriptionPiece += "/";
					bDistinction = true;

					m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARCHER_M ) );
				}

				if ( (dwReqCharClass & GLCC_SPIRIT_M)
					|| (dwReqCharClass & GLCC_SPIRIT_W) )
				{
					if ( bDistinction )
						m_strItemDescriptionPiece += "/";
					bDistinction = true;

					m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SPIRIT_M ) );
				}
				if ( (dwReqCharClass & GLCC_EXTREME_M)
					|| (dwReqCharClass & GLCC_EXTREME_W) )
				{
					if ( bDistinction )
						m_strItemDescriptionPiece += "/";
					bDistinction = true;

					m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_EXTREME_M ) );
				}

				if ( (dwReqCharClass & GLCC_SCIENTIST_M)
					|| (dwReqCharClass & GLCC_SCIENTIST_W) )
				{
					if ( bDistinction )
						m_strItemDescriptionPiece += "/";
					bDistinction = true;

					m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SCIENTIST_M ) );
				}

				if ( (dwReqCharClass & GLCC_ASSASSIN_M)
					|| (dwReqCharClass & GLCC_ASSASSIN_W) )
				{
					if ( bDistinction )
						m_strItemDescriptionPiece += "/";
					bDistinction = true;

					m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ASSASSIN_M ) );
				}

				if ( (dwReqCharClass & GLCC_TRICKER_M)
					|| (dwReqCharClass & GLCC_TRICKER_W) )
				{
					if ( bDistinction )
						m_strItemDescriptionPiece += "/";
					bDistinction = true;

					m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_TRICKER_M ) );
				}

				if ( (dwReqCharClass & GLCC_ETC_M)
					|| (dwReqCharClass & GLCC_ETC_W) )
				{
					if ( bDistinction )
						m_strItemDescriptionPiece += "/";
					bDistinction = true;

					m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ETC_M ) );
				}

				if ( (dwReqCharClass & GLCC_ACTOR_M)
					|| (dwReqCharClass & GLCC_ACTOR_W) )
				{
					if ( bDistinction )
						m_strItemDescriptionPiece += "/";
					bDistinction = true;

					m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ACTOR_M ) );
				}
			}
		}

		m_strItemDescription += m_strItemDescriptionPiece;
	}

	void GLItemTooltipMan::MakeItemDescriptionSex ()
	{
		if ( !m_pItemData )
			return;

		m_strItemDescriptionPiece = "";

		DWORD dwReqCharClass = m_pItemData->sBasicOp.dwReqCharClass;
		if ( (dwReqCharClass!=GLCC_ALL_SCIENTIST) )
		{
			bool bMan = false;
			bool bWoman = false;

			if (( dwReqCharClass & GLCC_FIGHTER_M )		||
				( dwReqCharClass & GLCC_ARMS_M )		||
				( dwReqCharClass & GLCC_ARCHER_M )		||
				( dwReqCharClass & GLCC_SPIRIT_M )		||
				( dwReqCharClass & GLCC_EXTREME_M )		||
				( dwReqCharClass & GLCC_SCIENTIST_M )	||
				( dwReqCharClass & GLCC_ASSASSIN_M )	||
				( dwReqCharClass & GLCC_TRICKER_M )		||
				( dwReqCharClass & GLCC_ETC_M)			||
				( dwReqCharClass & GLCC_ACTOR_M))
			{
				bMan = true;
			}

			if (( dwReqCharClass & GLCC_FIGHTER_W )		||
				( dwReqCharClass & GLCC_ARMS_W )		||
				( dwReqCharClass & GLCC_ARCHER_W )		||
				( dwReqCharClass & GLCC_SPIRIT_W )		||
				( dwReqCharClass & GLCC_EXTREME_W )		||
				( dwReqCharClass & GLCC_SCIENTIST_W )	||
				( dwReqCharClass & GLCC_ASSASSIN_W )	||
				( dwReqCharClass & GLCC_TRICKER_W )		||
				( dwReqCharClass & GLCC_ETC_W)			||
				( dwReqCharClass & GLCC_ACTOR_W))
			{
				bWoman = true;
			}

			if ( !bMan || !bWoman )
			{
				if ( bMan )
				{
					m_strItemDescriptionPiece += "/";
					m_strItemDescriptionPiece += ID2GAMEWORD( "BASIC_SEX", 0 );
				}
				else
				{
					m_strItemDescriptionPiece += "/";
					m_strItemDescriptionPiece += ID2GAMEWORD( "BASIC_SEX", 1 );
				}
			}
		}

		m_strItemDescription += m_strItemDescriptionPiece;
	}

	bool GLItemTooltipMan::MakeItemDescriptionRequirement ()
	{
		if ( !m_pItemData )
			return false;

		m_strItemDescriptionPiece = "";

		bool bMaking = false;

		// �䱸 ����;
		bool bDisplayLevel = false;
		if ( m_pItemData->sBasicOp.wReqLevelDW || m_pItemData->sBasicOp.wReqLevelUP )
		{
			bDisplayLevel = true;
			m_strItemDescriptionPiece += sc::string::format( "%s %d",
				ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 0), 
				m_pItemData->sBasicOp.wReqLevelDW );

			if ( m_pItemData->sBasicOp.wReqLevelUP )
				m_strItemDescriptionPiece += sc::string::format( "~%d", m_pItemData->sBasicOp.wReqLevelUP ); 

			bMaking = true;
		}

		SCHARSTATS& rItemStats = m_pItemData->sBasicOp.sReqStats;
		bool bDistinction = false;

		// ��������->��;
		if ( rItemStats.wPow )
		{
			m_strItemDescriptionPiece += sc::string::format( "%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 0 ), rItemStats.wPow );

			if ( bDisplayLevel )
			{
				bDisplayLevel = false;
				m_strItemDescriptionPiece += "/";
			}

			if ( bDistinction )
				m_strItemDescriptionPiece += "/";
			bDistinction = true;
			bMaking = true;
		}

		// ��������->ü��;
		if ( rItemStats.wStr )
		{
			m_strItemDescriptionPiece += sc::string::format( "%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 1 ), rItemStats.wStr );

			if ( bDisplayLevel )
			{
				bDisplayLevel = false;
				m_strItemDescriptionPiece += "/";
			}

			if ( bDistinction )
				m_strItemDescriptionPiece += "/";
			bDistinction = true;
			bMaking = true;
		}

		// ��������->���ŷ�;
		if ( rItemStats.wSpi )
		{
			m_strItemDescriptionPiece += sc::string::format("%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 2 ), rItemStats.wSpi );

			if ( bDisplayLevel )
			{
				bDisplayLevel = false;
				m_strItemDescriptionPiece += "/";
			}

			if ( bDistinction )
				m_strItemDescriptionPiece += "/";
			bDistinction = true;
			bMaking = true;
		}

		// ��������->��ø��;
		if ( rItemStats.wDex )
		{
			m_strItemDescriptionPiece += sc::string::format("%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 3 ), rItemStats.wDex );

			if ( bDisplayLevel )
			{
				bDisplayLevel = false;
				m_strItemDescriptionPiece += "/";
			}

			if ( bDistinction )
				m_strItemDescriptionPiece += "/";
			bDistinction = true;
			bMaking = true;
		}

		// ��������->����;
		if ( rItemStats.wInt )
		{
			m_strItemDescriptionPiece += sc::string::format("%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 4 ), rItemStats.wInt );

			if ( bDisplayLevel )
			{
				bDisplayLevel = false;
				m_strItemDescriptionPiece += "/";
			}

			if ( bDistinction )
				m_strItemDescriptionPiece += "/";
			bDistinction = true;
			bMaking = true;
		}

		// ��������->�ٷ�;
		if ( rItemStats.wSta )
		{
			m_strItemDescriptionPiece += sc::string::format("%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 5 ), rItemStats.wSta );

			if ( bDisplayLevel )
			{
				bDisplayLevel = false;
				m_strItemDescriptionPiece += "/";
			}

			if ( bDistinction )
				m_strItemDescriptionPiece += "/";
			bDistinction = true;
			bMaking = true;
		}

		m_strItemDescription += m_strItemDescriptionPiece;

		return bMaking;
	}

	bool GLItemTooltipMan::MakeItemDescriptionType ()
	{
		if ( !m_pItemData )
			return false;

		m_strItemDescriptionPiece = "";

		bool bMaking = false;
		switch ( m_pItemData->BasicType() )
		{
		case ITEM_SUIT:
			{
				if ( m_pItemData->Suit() == SUIT_HANDHELD )
				{
					// ���� �϶�;
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						m_strItemDescriptionPiece += sc::string::format( "%s(%s)",
							ID2GAMEWORD( "ITEM_BASIC_INFO", 11 ),
							CGameTextMan::GetInstance().GetCommentText(
							"ITEMATTACK", m_pItemData->sSuitOp.emAttack).GetString() );
					}
					else
					{
						m_strItemDescriptionPiece += sc::string::format( "%s(%s)",
							ID2GAMEWORD( "ITEM_BASIC_INFO", 11 ),
							COMMENT::ITEMATTACK[ m_pItemData->sSuitOp.emAttack ] );
					}

					bMaking = true;
				}
				else
				{
					// ���Ⱑ �ƴ� ����� �϶�;
					if ( m_pItemData->sSuitOp.IsSTATE(EMITEM_SUIT_UNIQUE) == true )
					{
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							m_strItemDescriptionPiece += sc::string::format( "%s(%s)",
								CGameTextMan::GetInstance().GetCommentText(
								"ITEMSUIT", m_pItemData->sSuitOp.emSuit).GetString(),
								ID2GAMEWORD("ITEM_BASIC_INFO", 19));
						}
						else
						{
							m_strItemDescriptionPiece += sc::string::format( "%s(%s)",
								COMMENT::ITEMSUIT[ m_pItemData->sSuitOp.emSuit ].c_str(),
								ID2GAMEWORD("ITEM_BASIC_INFO", 19));
						}

						bMaking = true;
					}
					else
					{
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							m_strItemDescriptionPiece += sc::string::format( "%s",
								CGameTextMan::GetInstance().GetCommentText(
								"ITEMSUIT", m_pItemData->sSuitOp.emSuit).GetString());
						}
						else
						{
							m_strItemDescriptionPiece += sc::string::format( "%s",
								COMMENT::ITEMSUIT[ m_pItemData->sSuitOp.emSuit ].c_str());
						}

						bMaking = true;
					}			
				}
			}
			break;

		default:
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					m_strItemDescriptionPiece += ( m_pItemData->BasicType() ) < ITEM_NSIZE ?
						CGameTextMan::GetInstance().GetCommentText( "ITEMTYPE", m_pItemData->BasicType() ).GetString() : "";

					bMaking = true;
				}
				else
				{
					m_strItemDescriptionPiece += COMMENT::ItemTypeName( m_pItemData->BasicType() ).c_str();

					bMaking = true;
				}
			}
		}

		m_strItemDescription += m_strItemDescriptionPiece;

		return bMaking;
	}

	bool GLItemTooltipMan::MakeItemDescriptionAttack ()
	{
		if ( !m_pItemData )
			return false;

		m_strItemDescriptionPiece = "";

		bool bDisplay = false;

		const int nBasicAttackDamageIndex = m_pItemData->GetBasicAttackDamageIndex();
		if ( -1 != nBasicAttackDamageIndex )
		{
			m_strItemDescriptionPiece += sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO", 0 ) );
			m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_BASIC_RANDOM_STAT_PREVIEW", 0 );

			bDisplay = true;

			m_strItemDescription += m_strItemDescriptionPiece;

			return bDisplay;
		}

		int nMinDmg = static_cast<int>( m_pItemData->GetAttackDamage().GetLow() ); 
		int nMaxDmg = static_cast<int>( m_pItemData->GetAttackDamage().GetHigh() );

		//VAR_PARAM �Լ����� 1 �����϶� 1�� ����ִ� �ڵ尡 �־ ƫ������ ���� �˻縦 �ѹ� �� �������
		if ( nMinDmg || nMaxDmg )
		{
			m_strItemDescriptionPiece += sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 0) );
			m_strItemDescriptionPiece += sc::string::format( "%d", nMinDmg );
			m_strItemDescriptionPiece += " ~ ";
			m_strItemDescriptionPiece += sc::string::format( "%d", nMaxDmg );

			bDisplay = true;
		}

		m_strItemDescription += m_strItemDescriptionPiece;

		return bDisplay;
	}

	bool GLItemTooltipMan::MakeItemDescriptionDefense ()
	{
		if ( !m_pItemData )
			return false;

		m_strItemDescriptionPiece = "";

		bool bDisplay = false;

		const int nBasicDefenceIndex = m_pItemData->GetBasicDefenceIndex();
		if ( -1 != nBasicDefenceIndex )
		{
			m_strItemDescriptionPiece += sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO", 1 ) );
			m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_BASIC_RANDOM_STAT_PREVIEW", 0 );

			bDisplay = true;

			m_strItemDescription += m_strItemDescriptionPiece;

			return bDisplay;
		}

		// ����;
		int nBasicDef = static_cast<int>( m_pItemData->GetDefence() );

		if ( nBasicDef )
		{
			m_strItemDescriptionPiece += sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 1) );
			m_strItemDescriptionPiece += sc::string::format( "%d", nBasicDef );

			bDisplay = true;
		}

		m_strItemDescription += m_strItemDescriptionPiece;

		return bDisplay;
	}

	bool GLItemTooltipMan::MakeItemDescriptionEtc ()
	{
		if ( !m_pItemData )
			return false;

		m_strItemDescriptionPiece = "";

		// ���߷�;
		bool bDisplay = false;
		int nBasicHitRate = static_cast<int>( m_pItemData->sSuitOp.nHitRate );
		if ( nBasicHitRate )
		{
			m_strItemDescriptionPiece += sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 3) );
			m_strItemDescriptionPiece += sc::string::format( "%d", nBasicHitRate );
			bDisplay = true;
		}

		// ȸ����;
		int nBasicAvoidRate = static_cast<int>( m_pItemData->sSuitOp.nAvoidRate );
		if ( nBasicAvoidRate )
		{
			if ( bDisplay )
				m_strItemDescriptionPiece += '\n';

			m_strItemDescriptionPiece += sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 4) );
			m_strItemDescriptionPiece += sc::string::format( "%d", nBasicAvoidRate );
			bDisplay = true;
		}

		// �Ҹ� SP;
		int nBasicReqSP = static_cast<int>( m_pItemData->sSuitOp.wReqSP );
		if ( nBasicReqSP )
		{
			if ( bDisplay )
				m_strItemDescriptionPiece += '\n';
			m_strItemDescriptionPiece += sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 7)  );
			m_strItemDescriptionPiece += sc::string::format( "%d", nBasicReqSP );
			bDisplay = true;
		}

		// ������;
		if ( m_pItemData->sDrugOp.wPileNum > 1 )
		{
			WORD wPileNum = m_pItemData->sDrugOp.wPileNum;
			WORD wTurnNum = 1;				

			if ( bDisplay )
				m_strItemDescriptionPiece += '\n';

			m_strItemDescriptionPiece += sc::string::format( "%s :%d/%d",
				ID2GAMEWORD("ITEM_TURN_INFO", 0 ),
				wTurnNum,
				wPileNum );

			bDisplay = true;
		}

		// ��ǰ�� ��� ȿ��;
		if ( m_pItemData->BasicType() == ITEM_CURE ||
			 m_pItemData->BasicType() == ITEM_EFFECT_RELEASE_CURE ||
			 m_pItemData->BasicType() == ITEM_COSTUME_EFFECT_RELEASE )
		{
			if ( bDisplay )
				m_strItemDescriptionPiece += '\n';

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				if( m_pItemData->sDrugOp.dwCureVolume == 0 )
					m_strItemDescriptionPiece += sc::string::format( "%s", CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", m_pItemData->sDrugOp.emDrug).GetString() );
				else
					m_strItemDescriptionPiece += sc::string::format( "%s :%d",CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", m_pItemData->sDrugOp.emDrug).GetString(), m_pItemData->sDrugOp.dwCureVolume );
			}
			else
			{
				if( m_pItemData->sDrugOp.dwCureVolume == 0 )
					m_strItemDescriptionPiece += sc::string::format( "%s", COMMENT::ITEMDRUG[m_pItemData->sDrugOp.emDrug].c_str() );
				else
					m_strItemDescriptionPiece += sc::string::format( "%s :%d",COMMENT::ITEMDRUG[m_pItemData->sDrugOp.emDrug].c_str(), m_pItemData->sDrugOp.dwCureVolume );
			}

			if ( m_pItemData->sDrugOp.bRatio )
				m_strItemDescriptionPiece += "%";

			bDisplay = true;
		}

		// ���� �ð�;
		if ( m_pItemData->sBasicOp.IsCoolTime() )
		{
			if ( bDisplay )
				m_strItemDescriptionPiece += '\n';

			std::string strTime = "";
			CTimeSpan cCoolTime( m_pItemData->sBasicOp.dwCoolTime );

			if ( cCoolTime.GetHours() > 0 )
				strTime += sc::string::format( "%d%s ", cCoolTime.GetHours(), ID2GAMEWORD("ITEM_BASIC_TIME", 0) );
			if ( cCoolTime.GetMinutes() > 0 )	
				strTime += sc::string::format( "%d%s ", cCoolTime.GetMinutes(), ID2GAMEWORD("ITEM_BASIC_TIME", 1) );
			if ( cCoolTime.GetSeconds() > 0 )	
				strTime += sc::string::format( "%d%s ", cCoolTime.GetSeconds(), ID2GAMEWORD("ITEM_BASIC_TIME", 2) );

			m_strItemDescriptionPiece += sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 8), strTime );

			bDisplay = true;
		}

		// �������� Ư�� Ÿ��;
		switch ( m_pItemData->BasicType() )
		{
		case ITEM_SKILL:
			{
				//DisplaySkillItem( m_pItemLabel2, sItemCustom );
			}
			break;

		case ITEM_PET_SKILL:
			{
				//DisplayPetSkillItem( m_pItemLabel2, sItemCustom );
			}
			break;

		case ITEM_GRINDING:
			{
			}
			break;

		case ITEM_VIETNAM_EXPGET:
			{
				if ( m_pItemData->sDrugOp.bRatio )
					m_strItemDescriptionPiece += sc::string::format( ID2GAMEWORD("ITEM_CATEGORY_VIETNAM_EXP", 1), m_pItemData->sDrugOp.dwCureVolume );
				else
					m_strItemDescriptionPiece += sc::string::format( ID2GAMEWORD("ITEM_CATEGORY_VIETNAM_EXP", 0), m_pItemData->sDrugOp.dwCureVolume );
			}
			break;

		case ITEM_VIETNAM_ITEMGET:
			{
				m_strItemDescriptionPiece += sc::string::format( ID2GAMEWORD("ITEM_CATEGORY_VIETNAM_ITEM", 0), m_pItemData->sDrugOp.dwCureVolume );
			}
			break;

		case ITEM_VEHICLE:
			{
				//DisplayVehicleItem( m_pItemLabel2, sItemCustom );
			}
			break;

		case ITEM_PET_CARD:
			{
				//DisplayPetCardItem( m_pItemLabel2, sItemCustom );
			}
			break;

		case ITEM_LUNCHBOX:
			{
				//DisplayLunchBoxItem( m_pItemLabel2, sItemCustom );
			}
			break;

		case ITEM_PET_DUALSKILL:
			{
				//DisplayPetDualSkillItem( m_pItemLabel2, sItemCustom );
			}
			break;

		case ITEM_POINT_CARD_REFUND:
		case ITEM_POINT_CARD_NOT_REFUND:
			{
				// ����Ʈ �� ǥ��.;
				m_strItemDescriptionPiece += sc::string::format( ID2GAMEWORD("POINT_CARD_POINT_VOLUME"), m_pItemData->sDrugOp.dwCureVolume );
			}
			break;

		case ITEM_RANDOMITEM:
		case ITEM_BOX:
			m_strItemDescriptionPiece += ID2GAMEINTEXT( "HELP_SHOW_ITEM_TOOLTIP" );
			break;
		};

		// �׿�;
		EMITEM_TYPE emItemType = m_pItemData->BasicType();
		if( emItemType != ITEM_PET_SKILL &&
			emItemType != ITEM_PET_DUALSKILL &&
			emItemType != ITEM_POINT_CARD_REFUND &&
			emItemType != ITEM_POINT_CARD_NOT_REFUND )
		{
			// �ñ�/���� ī�忡 �� �̸� ��ǥ ���;
			if ( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_REGEN )
			{
				if ( bDisplay )
					m_strItemDescriptionPiece += '\n';

				m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_BASIC_INFO", 17 );
				m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_CALL_MAP", 2 );

				bDisplay = true;
			}
			else if ( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_LASTCALL )
			{
				if ( bDisplay )
					m_strItemDescriptionPiece += '\n';

				m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_BASIC_INFO", 17 );
				m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_CALL_MAP", 2 );

				bDisplay = true;
			}
			else if ( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_TELEPORT )
			{
				if ( bDisplay )
					m_strItemDescriptionPiece += '\n';

				m_strItemDescriptionPiece += ID2GAMEWORD( "ITEM_BASIC_INFO", 17 );

				m_strItemDescriptionPiece += sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_TELEPORT_MAP", 0 ), "(null)" );
				m_strItemDescriptionPiece += sc::string::format( "%s :%d,%d", ID2GAMEWORD("ITEM_TELEPORT_MAP", 1 ), m_pItemData->sBasicOp.wPosX, m_pItemData->sBasicOp.wPosY );

				bDisplay = true;
			}
		}

		m_strItemDescription += m_strItemDescriptionPiece;

		return bDisplay;
	}

	bool GLItemTooltipMan::MakeItemDescriptionAddValue ()
	{
		if ( !m_pItemData )
			return false;

		m_strItemDescriptionPiece = "";

		const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;

		bool bDisplay = false;

		// �����ġ;
		{
			int arrVALUE[EMADD_SIZE];
			SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );

			for ( int i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
			{
				EMITEM_ADDON emTYPE = sSUIT.sADDON[i].emTYPE;

				if ( emTYPE != EMADD_NONE )
				{
					int nVALUE = static_cast<int>( sSUIT.sADDON[i].nVALUE );

					switch ( emTYPE )
					{
					case EMADD_HP:
					case EMADD_MP:
					case EMADD_SP:
					case EMADD_MA:
						break;
					case EMADD_HITRATE:		arrVALUE[EMADD_HITRATE] += nVALUE;		break;
					case EMADD_AVOIDRATE:	arrVALUE[EMADD_AVOIDRATE] += nVALUE;	break;
					case EMADD_DAMAGE:		arrVALUE[EMADD_DAMAGE] += nVALUE;		break;
					case EMADD_DEFENSE:		arrVALUE[EMADD_DEFENSE] += nVALUE;		break;
					case EMADD_STATS_POW:	arrVALUE[EMADD_STATS_POW] += nVALUE;	break;
					case EMADD_STATS_STR:	arrVALUE[EMADD_STATS_STR] += nVALUE;	break;
					case EMADD_STATS_SPI:	arrVALUE[EMADD_STATS_SPI] += nVALUE;	break;
					case EMADD_STATS_DEX:	arrVALUE[EMADD_STATS_DEX] += nVALUE;	break;
					case EMADD_STATS_INT:	arrVALUE[EMADD_STATS_INT] += nVALUE;	break;
					case EMADD_STATS_STA:	arrVALUE[EMADD_STATS_STA] += nVALUE;	break;
					case EMADD_PA:			arrVALUE[EMADD_PA] += nVALUE;			break;
					case EMADD_SA:			arrVALUE[EMADD_SA] += nVALUE;			break;
					};
				}
			}

			for( int i=1; i<EMADD_SIZE; ++i)
			{
				if( arrVALUE[i] != 0 )
				{
					m_strItemDescriptionPiece += sc::string::format(
						"%s :", ID2GAMEWORD( "ITEM_ADDON_INFO", i ) );

					if ( arrVALUE[i] > 0 )
						m_strItemDescriptionPiece += sc::string::format( "+%d", arrVALUE[i] );
					else if ( arrVALUE[i] < 0 )
						m_strItemDescriptionPiece += sc::string::format( "%d", arrVALUE[i] );

					m_strItemDescriptionPiece += "\n";
				}
			}
		}

		/* �����ϹǷ� �ּ�ó��;
		// Ư�����;
		{
			EMITEM_VAR emITEM_VAR = m_pItemData->sSuitOp.sVARIATE.emTYPE;
			EMITEM_VAR emITEM_VOL = m_pItemData->sSuitOp.sVOLUME.emTYPE;

			float arrVALUE[EMVAR_SIZE];
			SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );

			// Ư����� ( ��ȭ�� );
			if ( emITEM_VAR != EMVAR_NONE )
			{
				switch ( emITEM_VAR )
				{
				case EMVAR_HP:
				case EMVAR_MP:
				case EMVAR_SP:
				case EMVAR_AP:
					break;
				default:
					arrVALUE[emITEM_VAR] = (
						m_pItemData->sSuitOp.sVARIATE.fVariate );
					break;
				};
			}

			for ( int i = 1; i < EMVAR_SIZE; ++i )
			{
				if ( arrVALUE[i] != 0.0f ) 
				{
					if ( m_pItemData->BasicType() != ITEM_PET_FOOD
						&& m_pItemData->BasicType() != ITEM_VEHICLE_OIL )
						arrVALUE[i] *= COMMENT::ITEMVAR_SCALE[i];

					CString strTemp = sc::string::format( "%.2f", arrVALUE[i] ).c_str();

					CutZero ( strTemp, 2 );

					m_strItemDescriptionPiece = sc::string::format(
						"%s :",
						ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", i ) );
					

					m_strItemDescriptionPiece = sc::string::format(
						"%s", strTemp );
					if ( COMMENT::IsITEMVAR_SCALE(emITEM_VAR) )
						m_strItemDescriptionPiece += "%";
					
					bDisplay = true;
				}
			}
		}
		*/
		m_strItemDescription += m_strItemDescriptionPiece;
		return bDisplay;
	}

	bool GLItemTooltipMan::MakeItemDescriptionResist()
	{
		if ( !m_pItemData )
			return false;

		m_strItemDescriptionPiece = "";

		const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;

		// ����;
		{
			// �⺻ ���װ�;
			int nELEC   = sSUIT.sResist.nElectric;
			int nFIRE   = sSUIT.sResist.nFire;
			int nICE    = sSUIT.sResist.nIce;
			int nPOISON = sSUIT.sResist.nPoison;
			int nSPIRIT = sSUIT.sResist.nSpirit;

			if ( nELEC || nFIRE || nICE || nPOISON || nSPIRIT )
			{
				m_strItemDescriptionPiece += sc::string::format( "%s(%d) ",
					ID2GAMEWORD( "CHARACTER_RESI_STATIC", 5 ), nSPIRIT );

				m_strItemDescriptionPiece += sc::string::format( "%s(%d) ",
					ID2GAMEWORD( "CHARACTER_RESI_STATIC", 1 ), nFIRE );

				m_strItemDescriptionPiece += sc::string::format( "%s(%d) ",
					ID2GAMEWORD( "CHARACTER_RESI_STATIC", 2 ), nICE );

				m_strItemDescriptionPiece += sc::string::format( "%s(%d) ",
					ID2GAMEWORD( "CHARACTER_RESI_STATIC", 3 ), nELEC );

				m_strItemDescriptionPiece += sc::string::format( "%s(%d)",
					ID2GAMEWORD( "CHARACTER_RESI_STATIC", 4 ), nPOISON );

				return true;
			}
		}

		m_strItemDescription += m_strItemDescriptionPiece;

		return false;
	}

	bool GLItemTooltipMan::MakeItemDescriptionBlow ()
	{
		if ( !m_pItemData )
			return false;

		m_strItemDescriptionPiece = "";

		std::string strText;

		EMSTATE_BLOW emBLOW = m_pItemData->sSuitOp.sBLOW.emTYPE;
		bool bDisplay = false;

		if ( emBLOW !=EMBLOW_NONE )
		{
			float fLife = m_pItemData->sSuitOp.sBLOW.fLIFE;
			float fRate = m_pItemData->sSuitOp.sBLOW.fRATE;

			float fVAR1 = m_pItemData->sSuitOp.sBLOW.fVAR1 * COMMENT::BLOW_VAR1_SCALE[emBLOW];
			float fVAR2 = m_pItemData->sSuitOp.sBLOW.fVAR2 * COMMENT::BLOW_VAR2_SCALE[emBLOW];

			switch ( emBLOW )
			{
			case EMBLOW_NUMB:   //	����;
				{
					// Ȯ���� ��ġ���� 100%�� �������� ����ǰ� �ִ� ǥ������� - 100% (ex.�����̺�ȭ 1.3 >> ���� ǥ�� ��ų������ +30%);
					if(fVAR2 > (1.0f * COMMENT::BLOW_VAR2_SCALE[emBLOW])) 
						fVAR2 -=  (1.0f * COMMENT::BLOW_VAR2_SCALE[emBLOW]);

					strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_NUMB_RN" ), fRate, fLife, fVAR1 );

					// +��ȣ ���̱�;
					if( fVAR2 > 0.0f )
						strText += sc::string::format(" +%2.f%%", fVAR2);
					else
						strText += sc::string::format(" %2.f%%", fVAR2);

					bDisplay = true;
				}
				break;

			case EMBLOW_STUN:   //	����;
				{
					strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_STUN_RN" ), fRate, fLife );
					bDisplay = true;
				}
				break;

			case EMBLOW_STONE:  //	��ȭ;
				{
					strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_STONE_RN" ), fRate, fLife, fVAR1, fVAR2 );
					bDisplay = true;
				}
				break;

			case EMBLOW_BURN:   //	ȭ��;
				{
					// ���� ǥ��;
					if ( fVAR2 > 0.0f )
						fVAR2 = -fVAR2;

					strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_BURN_RN" ), fRate, fLife, fVAR2 );
					bDisplay = true;
				}
				break;

			case EMBLOW_FROZEN: //	�õ�;
				{
					strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_FROZEN_RN" ), fRate, fLife, fVAR1 );

					// +��ȣ ���̱�;
					if( fVAR2 > 0.0f )
						strText += sc::string::format(" +%2.f%%", fVAR2);
					else
						strText += sc::string::format(" %2.f%%", fVAR2);
					bDisplay = true;
				}
				break;

			case EMBLOW_MAD:    //	����;
				{
					// ���� ǥ��;
					if ( fVAR1 > 0.0f )
						fVAR1 = -fVAR1;

					// ���� ǥ��;
					if ( fVAR2 > 0.0f )
						fVAR2 = -fVAR2;
					strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_MAD_RN" ), fRate, fLife, fVAR1, fVAR2 );

					bDisplay = true;
				}
				break;

			case EMBLOW_POISON: //	�ߵ�;
				{
					if ( fVAR2 > 0.0f )
						fVAR2 = -fVAR2;

					strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_POISON_RN" ), fRate, fLife, fVAR2 );
					bDisplay = true;
				}
				break;

			case EMBLOW_CURSE:  //	����;
				{
					strText = sc::string::format( ID2GAMEINTEXT( "DES_STATE_BLOW_CURSE_RN" ), fLife, fRate, fVAR1 );
					bDisplay = true;
				}
				break;

			default:
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						strText = sc::string::format("%s :%s", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 0 ),CGameTextMan::GetInstance().GetCommentText("BLOW", emBLOW).GetString());
					else
						strText = sc::string::format("%s :%s", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 0 ), COMMENT::BLOW[emBLOW].c_str() );

					strText += "\n";

					strText += sc::string::format("%s :%.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 1 ), fLife );
					strText += "\n";

					strText += sc::string::format("%s :%.2f%%", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 2 ), fRate );
					strText += "\n";

					if( fVAR1 != 0.0f )
					{
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							if ( COMMENT::IsBLOW1_PER(emBLOW) )
								strText += sc::string::format("%s :%.2f%%", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1", emBLOW).GetString(), fVAR1 );
							else
								strText += sc::string::format("%s :%.2f", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1", emBLOW).GetString(), fVAR1 );
						}
						else
						{
							if ( COMMENT::IsBLOW1_PER(emBLOW) )
								strText += sc::string::format("%s :%.2f%%", COMMENT::BLOW_VAR1[emBLOW].c_str(), fVAR1 );
							else
								strText += sc::string::format("%s :%.2f", COMMENT::BLOW_VAR1[emBLOW].c_str(), fVAR1 );
						}

						strText += "\n";
					}

					if( fVAR2 != 0.0f )
					{
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							if ( COMMENT::IsBLOW2_PER(emBLOW) )
								strText += sc::string::format("%s :%.2f%%", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2", emBLOW).GetString(), fVAR2 );
							else
								strText += sc::string::format("%s :%.2f", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2", emBLOW).GetString(), fVAR2 );
						}
						else
						{
							if ( COMMENT::IsBLOW2_PER(emBLOW) )
								strText += sc::string::format("%s :%.2f%%", COMMENT::BLOW_VAR2[emBLOW].c_str(), fVAR2 );
							else
								strText += sc::string::format("%s :%.2f", COMMENT::BLOW_VAR2[emBLOW].c_str(), fVAR2 );
						}

						strText += "\n";
					}

					bDisplay = true;
				}
			};

			m_strItemDescriptionPiece += strText;

			m_strItemDescription += m_strItemDescriptionPiece;
		}

		return bDisplay;
	}

	void GLItemTooltipMan::MakeItemDescriptionSetItem ()
	{
		if ( !m_pItemData )
			return;

		if ( !GLUseFeatures::GetInstance().IsUsingSetItemUse() )
			return;

		const SSETITEM *sSetItem =
			GLItemSet::GetInstance().GetInfoSetItem(
			m_pItemData->sBasicOp.sNativeID );

		// ��Ʈ������ �˻��ȵ� -> ��� ����;
		if ( NULL == sSetItem )
			return;

		// ���� ���;
		if ( MakeItemDescriptionSetItemName( sSetItem ) )
			NextLine();
		//if ( MakeItemDescriptionSetItemParts( sSetItem ) )
		//	NextLine();
		if ( MakeItemDescriptionSetItemBonus( sSetItem ) )
			NextLine();
	}

	bool GLItemTooltipMan::MakeItemDescriptionSetItemName (
		const SSETITEM *sSetItem )
	{
		m_strItemDescriptionPiece = "";

		// ��Ʈ;
		m_strItemDescriptionPiece = sc::string::format(
			"%s", ID2GAMEWORD("ITEM_INFO_SET", 0) );

		// ��Ʈ ��Ī;
		m_strItemDescriptionPiece += sc::string::format(
			"%s ", sSetItem->strName );

		m_strItemDescription += m_strItemDescriptionPiece;

		return true;
	}

	bool GLItemTooltipMan::MakeItemDescriptionSetItemParts (
		const SSETITEM *sSetItem )
	{
		m_strItemDescriptionPiece = "";
		bool bDisplay = false;

		// ���� ��� ���� ����� ����;
		for ( UINT i = 0; i < sSetItem->byMaxToolTipItemList; ++i )
		{
			SITEM *sItem = GLogicData::GetInstance().GetItem(
				sSetItem->vecItemList[i] );

			if ( NULL == sItem ) continue;

			// ���;
			if ( i )	
			{
				// ���� ����;
				m_strItemDescriptionPiece += " / ";

				// �� ��;
				m_strItemDescriptionPiece = sc::string::format(
					"%s", ID2GAMEWORD( "ITEM_INFO_SET_PARTS",
					sItem->sSuitOp.emSuit ) );
			}
			else	    
			{
				// ó�� ��� " - " ������ ���ܻ�Ȳ ó��;
				m_strItemDescriptionPiece += " - ";

				m_strItemDescriptionPiece = sc::string::format(
					"%s", ID2GAMEWORD("ITEM_INFO_SET_PARTS",
					sItem->sSuitOp.emSuit ) );
			}

			bDisplay = true;
		}

		m_strItemDescription += m_strItemDescriptionPiece;

		return bDisplay;
	}

	bool GLItemTooltipMan::MakeItemDescriptionSetItemBonus (
		const SSETITEM *sSetItem )
	{
		m_strItemDescriptionPiece = "";	

		bool bDisplay = false;

		// ���2. �ҽ� ����ϰ�, for�� 2�� ���, vector ���� �̸� �Ҵ�;
		std::vector<std::string> strSetBonus;
		strSetBonus.resize( SLOT_TSIZE );

		// ��� �� ���ڿ� ����;
		for ( BYTE i = 0; i < sSetItem->vecBonus.size(); ++i )
		{
			// ��� ������ �ɼ� Ȯ��; 
			if ( GLItemSet::GetInstance().IsUsableOpt(
				sSetItem->vecBonus[i].sSpec.emSPEC ) )
			{    
				UINT nTerms( sSetItem->vecBonus[i].byTerms );

				// ó�� ǥ�� �� ����;
				if( strSetBonus[nTerms].empty() )
				{
					strSetBonus[nTerms] = sc::string::format(
						"%d %s ",
						nTerms,
						ID2GAMEWORD("ITEM_INFO_SET", 0) );
				}
				else
				{
					// ���ݹ���;
					strSetBonus[nTerms] += ", ";
				}

				// ���ʽ���;
				strSetBonus[nTerms] += sc::string::format( "%d ",
					ID2GAMEWORD("ITEM_INFO_SET_BONUS",
					static_cast<int>(sSetItem->vecBonus[i].sSpec.emSPEC) ) );

				// ��ġ;
				float fVal = sSetItem->vecBonus[i].sSpec.sSPEC.fVAR1;

				if ( fVal> 0.0f ) 
					strSetBonus[nTerms] += _T('+');

				if ( GLItemSet::GetInstance().IsPerentOpt(
					sSetItem->vecBonus[i].sSpec.emSPEC ) )
				{
					strSetBonus[nTerms] += sc::string::format(
						"%.2f", (fVal * 100.0f) );
					strSetBonus[nTerms] += _T("%");
				}
				else
				{
					strSetBonus[nTerms] += sc::string::format(
						"%d", static_cast<int>(fVal) );
				}		
			}	
		}

		// ���;
		for ( UINT i = 0; i < strSetBonus.size(); ++i )
		{	
			if ( !strSetBonus[i].empty() )
			{
				m_strItemDescriptionPiece += strSetBonus[i].c_str();
				m_strItemDescriptionPiece += "\n";

				bDisplay = true;
			}
		}

		m_strItemDescription += m_strItemDescriptionPiece;

		return bDisplay;
	}
}