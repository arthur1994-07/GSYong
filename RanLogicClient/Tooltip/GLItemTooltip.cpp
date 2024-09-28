#include "pch.h"

#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../enginelib/GUInterface/UITextUtil.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Item/ItemDurability.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Inventory/GLInventorySale.h"
#include "../../RanLogic/Market/GLPrivateMarket.h"
#include "../../RanLogic/ItemPeriodExtension/ItemPeriodExtension.h"

#include "../../RanGfxUI/GfxInterface.h"

#include "../Char/GLCharacter.h"
#include "../Char/GLCharClient.h"
#include "../WishList/WishList.h"
#include "../GLGaeaClient.h"
#include "../NPCTalk/GLNpcTalkClient.h"
#include "../PrivateMarketSearchBuy/PrivateMarketClient.h"
#include "../ItemPreview/GLItemPreviewClient.h"
#include "../Level/GLLevelFileClient.h"

#include "GLItemTooltip.h"

GLItemTooltip::GLItemTooltip()
:	m_pItemData( NULL )
,	m_pItemDisguise( NULL )
,	m_pCharacter( NULL )
,	m_pOtherCharData( NULL )
,	m_fDurability( 1.0f )
,	m_wInvenPosX( 0 )
,	m_wInvenPosY( 0 )
{
	for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i)
		m_fDurabilityValue[i] = 1.0f;
}

GLItemTooltip::GLItemTooltip( const GLItemTooltip& value )
{
}

GLItemTooltip::~GLItemTooltip()
{
}

void GLItemTooltip::CutZero( CString& str, int nCount )
{
	// '.' 점이 없다는 것은 소수점 이하 자리가 없으므로 리턴
	if( str.Find( '.' ) == -1 )
		return;

	CString temp = str;

	//ncount 소숫점 몇째 자리 까지 잘라낼찌 정한다
	for( int i = 0; i < nCount + 1; ++i )
	{
		temp = str.Right( 1 );

		if( temp.Find( '0' ) != -1 || temp.Find( '.' ) != -1 )
			str = str.Left( str.GetLength() - 1 );
		else
			break;
	}
}

void GLItemTooltip::SetInvenPos( WORD wPosX, WORD wPosY )
{
	m_wInvenPosX = wPosX;
	m_wInvenPosY = wPosY;
}

bool GLItemTooltip::GetItemDataHeader( const SITEMCUSTOM& sItemCustom, LuaTable& tb )
{
	// ID가 같으면 아이템 데이터 새로 검색할 필요 없다.
	if( NULL == m_pItemData || m_sItemCustom.GetNativeID() != sItemCustom.GetNativeID() )
	{
		m_pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
		if( NULL == m_pItemData )
			return false;;
	}

	// ID가 같으면 아이템 데이터 새로 검색할 필요 없다.
	if( NULL == m_pItemDisguise || m_sItemCustom.nidDISGUISE != sItemCustom.nidDISGUISE )
		m_pItemDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );

	m_sItemCustom = sItemCustom;

	if( NULL == m_pCharacter )
		m_pCharacter = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();

	if( GLUseFeatures::GetInstance().IsUsingItemDurability() )
		m_fDurability = ItemDurability::GetInstance().GetPerformance( m_sItemCustom );

	GenName( tb );
	GenGrade( tb );
	GenSchool( m_pItemData->sBasicOp.dwReqSchool, tb );
	GenClass( m_pItemData->sBasicOp.dwReqCharClass, tb );
	GenSex( m_pItemData->sBasicOp.dwReqCharClass, tb );
	GenLevel( tb );
	GenStat( tb );

	return true;
}

bool GLItemTooltip::GetItemDataInfo( const SITEMCUSTOM& sItemCustom, LuaTable& tb, int nType, bool bSimple )
{
	// ID가 같으면 아이템 데이터 새로 검색할 필요 없다.
	if( NULL == m_pItemData || m_sItemCustom.GetNativeID() != sItemCustom.GetNativeID() )
	{
		m_pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
		if( NULL == m_pItemData )
			return false;
	}

	// ID가 같으면 아이템 데이터 새로 검색할 필요 없다.
	if( NULL == m_pItemDisguise || m_sItemCustom.nidDISGUISE != sItemCustom.nidDISGUISE )
		m_pItemDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );

	m_sItemCustom = sItemCustom;

	if( NULL == m_pCharacter )
		m_pCharacter = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();

	bool bShowDurablity = false;

	float fTempDurability = 1.f;
	
	if( GLUseFeatures::GetInstance().IsUsingItemDurability() )
	{
		ResetDurabilityValue();

		m_fDurability = ItemDurability::GetInstance().GetPerformance( m_sItemCustom );

		fTempDurability = m_fDurability;

		if( ItemDurability::GetInstance().IsUseWeapon(m_pItemData) )
		{					
			for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i )
			{
				if( GLCONST_CHAR::nDurabilityApply[i] ) m_fDurabilityValue[i] = m_fDurability;
			}

			m_fDurability = 1.f;

			bShowDurablity = true;
		}

		/*
		switch( nType )
		{
		case EMGFX_SLOT_INVENTORY :
			{
				if( m_pItemData->sBasicOp.emItemType == ITEM_SUIT && m_pItemData->sSuitOp.emSuit == SUIT_HANDHELD )
				{
					m_fDurability = 1.f;					
				}
			}
			break;
		case EMGFX_SLOT_CHARINFO :
			{
				//if( m_pItemData->sBasicOp.emItemType == ITEM_SUIT && m_pItemData->sSuitOp.emSuit == SUIT_HANDHELD && m_pItemData->sSuitOp.wDurabilityMax > 0 )
				if( ItemDurability::GetInstance().IsUseWeapon(m_pItemData) )
				{					
					for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i )
					{
						if( GLCONST_CHAR::nDurabilityApply[i] ) m_fDurabilityValue[i] = m_fDurability;
					}

					m_fDurability = 1.f;
					
					bShowDurablity = true;
				}
			}
			break;
		}
		*/
	}
		
	if( false == bSimple )
	{
		// 슬롯 타입
		tb.set( "SlotType", nType );

		GenTypeData( tb );			// 타입정보(무기, 복장 등)
		GenDurability( tb );		// 내구도
		GenPileNum( tb, nType );	// 소지량
		GenDrugUseEffect( tb );		// 약품류 사용효과
		GenCoolTime( tb );			// 재사용 시간
		GenExplain( tb );			// 아이템 설명
		GenExplainEx( tb );			// 아이템 설명
		GenThrowTradeSale( tb );	// 거래
		GenPrice( tb, nType );		// 살때, 팔때 가격
		GenActPoint( tb );			// 요구 포인트
		GenContribution( tb );		// 요구 기여도
		GenTime( tb );				// 기간
		GenSetItem( tb );			// 세트아이템
		GenType( tb );
		GenWishlist( tb, nType );	// 위시리스트
		GenStateBlow( tb );			// 상태이상	
		GenPreview( tb );			// 아이템 미리보기
		GenTradeItem( tb, nType );	// 교환 아이템
	}
	
	if( m_sItemCustom.IsCSApplied() )
	{
		Gen_CostumeState(tb, bSimple);
		GenRandomOpt( tb );			// 랜덤 옵션
		GenItemSkillLink( tb );	    // 아이템 스킬링크
	}
	else
	{
		GenDamage( tb );			// 데미지
		GenDefence( tb );			// 방어력
		GenHitRate( tb );			// 명중률
		GenAvoidRate( tb );			// 회피율
		GenCostSp( tb );			// 소모sp
		GenAddOpt( tb, bSimple );	// 가산 수치
		GenRandomOpt( tb );			// 랜덤 옵션
		GenItemSkillLink( tb );	    // 아이템 스킬링크
		GenExp( tb );				// 경험치 배율
		GenResist( tb );			// 저항
	}
	
	return true;
}

bool GLItemTooltip::GetItemDurabilityHeader(const SITEMCUSTOM& sItemCustom, LuaTable& tb, int nType, bool bSimple)
{
	float fTempDurability = ItemDurability::GetInstance().GetPerformance( m_sItemCustom );
	Durability_Header(tb, fTempDurability);
	return true;
}

bool GLItemTooltip::GetItemDurability(const SITEMCUSTOM& sItemCustom, LuaTable& tb, int nType, bool bSimple)
{
	float fTempDurability = ItemDurability::GetInstance().GetPerformance( m_sItemCustom );
	Durability_MakeBasic(tb, fTempDurability);
	Durability_MakeAdd(tb, fTempDurability);
	Durability_MakeRandom(tb, fTempDurability);

	return true;
}

void GLItemTooltip::GenName( LuaTable& tb )
{
	LuaTable tbName( GLWidgetScript::GetInstance().GetLuaState() );
	tbName.set( 1, m_sItemCustom.GETNAME().c_str() );
	tbName.set( 2, ITEM_INFOR_COLOR::dwItemRank[ m_pItemData->sBasicOp.emLevel ] );
	tb.set( "Name", tbName );
}

void GLItemTooltip::GenGrade( LuaTable& tb )
{
	tb.set( "Grade" , (int) m_sItemCustom.GETGRADE( EMGRINDING_NUNE ) );
}

void GLItemTooltip::GenSchool( DWORD dwReqSchool, LuaTable& tb )
{
	if( dwReqSchool == GLSCHOOL_ALL )
	{		
		tb.set( "School", "ALL" );		
	}
	else
	{
		if( dwReqSchool & GLSCHOOL_00 )
		{			 
			tb.set( "School", GLCONST_CHAR::strSCHOOLNAME[ school2index( GLSCHOOL_00 ) ].c_str() );
		}
		else if( dwReqSchool & GLSCHOOL_01 )
		{			
			tb.set( "School", GLCONST_CHAR::strSCHOOLNAME[ school2index( GLSCHOOL_01 ) ].c_str() );		
		}
		else if( dwReqSchool & GLSCHOOL_02 )
		{		 
			tb.set( "School", GLCONST_CHAR::strSCHOOLNAME[ school2index( GLSCHOOL_02 ) ].c_str() );
		}
	}
}

void GLItemTooltip::GenClass( DWORD dwReqCharClass, LuaTable& tb )
{		
	LuaTable tbClass( GLWidgetScript::GetInstance().GetLuaState() );

	int nCount = 1;

	if( dwReqCharClass == GLCC_ALL_ACTOR || dwReqCharClass == GLCC_TOOL_MAN || dwReqCharClass == GLCC_TOOL_WOMAN )
	{
		tbClass.set( nCount, "ALL" );
	}
	else
	{		
		if( dwReqCharClass & GLCC_FIGHTER_M )
		{		
			tbClass.set( nCount, GLCC_FIGHTER_M );	
			++nCount;
		}
		else if( dwReqCharClass & GLCC_FIGHTER_W )
		{
			tbClass.set( nCount, GLCC_FIGHTER_W );	
			++nCount;
		}

		if( dwReqCharClass & GLCC_ARMS_M ) 
		{
			tbClass.set( nCount, GLCC_ARMS_M );		
			++nCount;
		}
		else if( dwReqCharClass & GLCC_ARMS_W )
		{
			tbClass.set( nCount, GLCC_ARMS_W );		
			++nCount;
		}

		if( dwReqCharClass & GLCC_ARCHER_M )
		{			
			tbClass.set( nCount, GLCC_ARCHER_M );
			++nCount;
		}
		else if( dwReqCharClass & GLCC_ARCHER_W )
		{
			tbClass.set( nCount, GLCC_ARCHER_W );		
			++nCount;
		}

		if( dwReqCharClass & GLCC_SPIRIT_M )
		{
			tbClass.set( nCount, GLCC_SPIRIT_M );
			++nCount;
		}
		else if( dwReqCharClass & GLCC_SPIRIT_W )
		{
			tbClass.set( nCount, GLCC_SPIRIT_W );		
			++nCount;
		}

		if ( dwReqCharClass & GLCC_EXTREME_M )
		{
			tbClass.set( nCount, GLCC_EXTREME_M );
			++nCount;
		}
		else if( dwReqCharClass & GLCC_EXTREME_W )
		{
			tbClass.set( nCount, GLCC_EXTREME_W );		
			++nCount;
		}

		if( dwReqCharClass & GLCC_SCIENTIST_M )
		{
			tbClass.set( nCount, GLCC_SCIENTIST_M );
			++nCount;
		}
		else if( dwReqCharClass & GLCC_SCIENTIST_W )
		{
			tbClass.set( nCount, GLCC_SCIENTIST_W );		
			++nCount;
		}

		if( dwReqCharClass & GLCC_ASSASSIN_M )
		{
			tbClass.set( nCount, GLCC_ASSASSIN_M );
			++nCount;
		}
		else if( dwReqCharClass & GLCC_ASSASSIN_W )
		{
			tbClass.set( nCount, GLCC_ASSASSIN_W );		
			++nCount;
		}

		if( dwReqCharClass & GLCC_TRICKER_M )
		{
			tbClass.set( nCount, GLCC_TRICKER_M );
			++nCount;
		}
		else if( dwReqCharClass&GLCC_TRICKER_W )
		{
			tbClass.set( nCount, GLCC_TRICKER_W );		
			++nCount;
		}

		if( dwReqCharClass & GLCC_ACTOR_M )
		{
			tbClass.set( nCount, GLCC_ACTOR_M );
			++nCount;
		}
		else if( dwReqCharClass&GLCC_ACTOR_W )
		{
			tbClass.set( nCount, GLCC_ACTOR_W );		
			++nCount;
		}

		if( dwReqCharClass & GLCC_ETC_M	|| dwReqCharClass & GLCC_ETC_W )
			tbClass.set( nCount, GLCC_ETC_M );
	}	

	if ( m_pCharacter == NULL )
		m_pCharacter = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();

	if ( m_pOtherCharData )
		tbClass.set( "MyClass", m_pOtherCharData->m_emClass );
	else
		tbClass.set( "MyClass", m_pCharacter->m_emClass );

	tb.set( "Class", tbClass );
}

void GLItemTooltip::GenSex( DWORD dwReqCharClass, LuaTable& tb )
{
	LuaTable tbSexData( GLWidgetScript::GetInstance().GetLuaState() );
	if (m_pCharacter == NULL)
		m_pCharacter = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();

	if( dwReqCharClass != GLCC_ALL_ACTOR )
	{
		bool bMan = false;
		bool bWoman = false;

		if( dwReqCharClass & GLCC_FIGHTER_M ||
			dwReqCharClass & GLCC_ARMS_M ||
			dwReqCharClass & GLCC_ARCHER_M ||
			dwReqCharClass & GLCC_SPIRIT_M ||
			dwReqCharClass & GLCC_EXTREME_M ||
			dwReqCharClass & GLCC_SCIENTIST_M ||
			dwReqCharClass & GLCC_ASSASSIN_M ||
			dwReqCharClass & GLCC_TRICKER_M ||
			dwReqCharClass & GLCC_ETC_M ||
			dwReqCharClass & GLCC_ACTOR_M )
		{
			bMan = true;
		}

		if( dwReqCharClass & GLCC_FIGHTER_W ||
			dwReqCharClass & GLCC_ARMS_W ||
			dwReqCharClass & GLCC_ARCHER_W ||
			dwReqCharClass & GLCC_SPIRIT_W ||
			dwReqCharClass & GLCC_EXTREME_W ||
			dwReqCharClass & GLCC_SCIENTIST_W ||
			dwReqCharClass & GLCC_ASSASSIN_W ||
			dwReqCharClass & GLCC_TRICKER_W ||
			dwReqCharClass & GLCC_ETC_W ||
			dwReqCharClass & GLCC_ACTOR_W )
		{
			bWoman = true;
		}

		EMCHARCLASS emClass = GLCC_NONE;
		if ( m_pOtherCharData )
			emClass = m_pOtherCharData->m_emClass;
		else
			emClass = m_pCharacter->m_emClass;

		// 남, 여 모두 착용 가능하지 않을 때.
		if( bMan && bWoman )
		{
			tbSexData.set( "SameSex", true );
			tbSexData.set( "bMan", "ALL" );
		}
		else if( bMan )
		{

			// 남자 착용 가능 할 때.
			if( emClass & GLCC_FIGHTER_M ||
				emClass & GLCC_ARMS_M ||
				emClass & GLCC_ARCHER_M ||
				emClass & GLCC_SPIRIT_M ||
				emClass & GLCC_EXTREME_M ||
				emClass & GLCC_SCIENTIST_M ||
				emClass & GLCC_ASSASSIN_M ||
				emClass & GLCC_TRICKER_M ||
				emClass & GLCC_ETC_M ||
				emClass & GLCC_ACTOR_M )
				tbSexData.set( "SameSex", true );
			else
				tbSexData.set( "SameSex", false );

			tbSexData.set( "bMan", true );
		}
		else if( bWoman )
		{
			if( emClass & GLCC_FIGHTER_W ||
				emClass & GLCC_ARMS_W ||
				emClass & GLCC_ARCHER_W ||
				emClass & GLCC_SPIRIT_W ||
				emClass & GLCC_EXTREME_W ||
				emClass & GLCC_SCIENTIST_W ||
				emClass & GLCC_ASSASSIN_W ||
				emClass & GLCC_TRICKER_W ||
				emClass & GLCC_ETC_W ||
				emClass & GLCC_ACTOR_W  )
				tbSexData.set( "SameSex", true );
			else
				tbSexData.set( "SameSex", false );

			tbSexData.set( "bMan", false );
		}
	}
	else
	{
		tbSexData.set( "SameSex", true );
		tbSexData.set( "bMan", "ALL" );
	}

	tb.set( "Sex", tbSexData );
}

void GLItemTooltip::GenLevel( LuaTable& tb )
{	
	LuaTable tbLevel( GLWidgetScript::GetInstance().GetLuaState() );

	tbLevel.set( "LevelDW", m_pItemData->sBasicOp.wReqLevelDW );
	tbLevel.set( "LevelUP", m_pItemData->sBasicOp.wReqLevelUP );

	short nEditedWearableLevel = 0;
	for ( UINT i = 0; i<RandomOpt::NSIZE; ++i )
	{
		const BYTE nType = m_sItemCustom.randomOption.getType( i );
		if ( RandomOption::RANDOM_OPTION_VALUE_WEARABLELEVEL_ADD == nType )
		{
			nEditedWearableLevel = m_sItemCustom.getOptValue( i );
			break;
		}
	}

	tbLevel.set( "EditedWearableLevel", nEditedWearableLevel );

	tb.set( "Level", tbLevel );
}

void GLItemTooltip::GenStat( LuaTable& tb )
{
	LuaTable tbStat( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbItemStat( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbCharStat( GLWidgetScript::GetInstance().GetLuaState() );

	SCHARSTATS& rItemStats = m_pItemData->sBasicOp.sReqStats;

	tbItemStat.set( "Pow", rItemStats.wPow );
	tbItemStat.set( "Str", rItemStats.wStr );
	tbItemStat.set( "Spi", rItemStats.wSpi );
	tbItemStat.set( "Dex", rItemStats.wDex );	
	tbItemStat.set( "Sta", rItemStats.wSta );

	if ( m_pOtherCharData )
	{
		const SCHARSTATS& rCharStats = m_pOtherCharData->m_sSUMSTATS_SKILL;
		tbCharStat.set( "Pow", rCharStats.wPow );
		tbCharStat.set( "Str", rCharStats.wStr );
		tbCharStat.set( "Spi", rCharStats.wSpi );
		tbCharStat.set( "Dex", rCharStats.wDex );
		tbCharStat.set( "Sta", rCharStats.wSta );
	}
	else
	{
		const SCHARSTATS& rCharStats = m_pCharacter->m_sSUMSTATS_SKILL;
		tbCharStat.set( "Pow", rCharStats.wPow );
		tbCharStat.set( "Str", rCharStats.wStr );
		tbCharStat.set( "Spi", rCharStats.wSpi );
		tbCharStat.set( "Dex", rCharStats.wDex );
		tbCharStat.set( "Sta", rCharStats.wSta );
	}

	tbStat.set( "Item", tbItemStat );
	tbStat.set( "Char", tbCharStat );

	tb.set( "Stat", tbStat );
}

void GLItemTooltip::GenTypeData( LuaTable& tb )
{
	LuaTable tbType( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbDisguise( GLWidgetScript::GetInstance().GetLuaState() );

	tbType.set( "Level", m_pItemData->sBasicOp.emLevel );
	tbType.set( "LevelColor", ITEM_INFOR_COLOR::dwItemRank[ m_pItemData->sBasicOp.emLevel ] );

	switch( m_pItemData->BasicType() )
	{
	case ITEM_SUIT :
		{
			if( m_pItemData->Suit() == SUIT_HANDHELD )
			{
				GLITEM_ATT emAttack = m_pItemData->sSuitOp.emAttack;
				GLITEM_ATT emTempAttack = emAttack;

				if ( m_pOtherCharData )
				{
					emAttack = m_pOtherCharData->CONVERT_ITEMATT( emAttack );
				}
				else if ( m_pCharacter )
				{
					emAttack = m_pCharacter->CONVERT_ITEMATT( emAttack );
				}
				
				if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					CString tempStr( CGameTextMan::GetInstance().GetCommentText(
						"ITEMATTACK", emAttack ).GetString() );
					tbType.set( "Type", tempStr.GetBuffer() );
					tbType.set( "Weapon", true );
					tbType.set( "RealType", static_cast<int>(emTempAttack) );
				}
				else
				{
					tbType.set( "Type", COMMENT::ITEMATTACK[ emAttack ] );
					tbType.set( "Weapon", true );
					tbType.set( "RealType", static_cast<int>(emTempAttack) );
				}
			}
			else
			{				
				if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					CString tempStr(CGameTextMan::GetInstance().GetCommentText(
						"ITEMSUIT", m_pItemData->sSuitOp.emSuit ).GetString() );
					tbType.set( "Type", tempStr.GetBuffer() );
					tbType.set( "Weapon", false );
				}
				else
				{
					tbType.set( "Type", COMMENT::ITEMSUIT[ m_pItemData->sSuitOp.emSuit ].c_str() );
					tbType.set( "Weapon", false );
				}

				if( m_pItemDisguise != NULL && m_pItemDisguise->BasicId() != NATIVEID_NULL() )
				{
					tbDisguise.set( "Name", m_pItemDisguise->GetName() );
					tbDisguise.set( "ColorChange", m_pItemDisguise->IsColorChange() );
					tbType.set( "Disguise", tbDisguise );
				}
				else
				{
					tbType.set( "ColorChange", m_pItemData->IsColorChange() );
				}
			}

			tbType.set( "DisguiseOk", m_sItemCustom.IsDisguise() );
			tbType.set( "PutOn", true );
			tbType.set( "BasicOptionPrint", true );
		}
		break;


	default :
		{
			if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				tbType.set( "Type", m_pItemData->BasicType() < ITEM_NSIZE ? CGameTextMan::GetInstance().GetCommentText(
					"ITEMTYPE", m_pItemData->BasicType() ).GetString() : "" );
			}
			else
			{
				std::string  strType =  COMMENT::ItemTypeName( m_pItemData->BasicType() ).c_str();

				switch ( m_pItemData->BasicType() )
				{
				case ITEM_LOCK_BOX:
					
					strType.append( m_sItemCustom.bVietnamGainItem ? CGameTextMan::GetInstance().GetGameWord("LOCKBOX_TOOLTIP_LOCK", 0).GetString() : 
						CGameTextMan::GetInstance().GetGameWord("LOCKBOX_TOOLTIP_LOCK", 1).GetString());

					tbType.set( "Type", strType );

					break ;

				default:
					tbType.set( "Type", strType);
					break ;
				}
			}

			switch ( m_pItemData->BasicType() )
			{
			case ITEM_CHARM:
			case ITEM_ARROW:
			case ITEM_BULLET:
				tbType.set( "BasicOptionPrint", true );
				break;
			default:
				tbType.set( "BasicOptionPrint", false );
				break;
			}

			tbType.set( "PutOn", false );
		}
		break;
	}

	tb.set("TypeData", tbType);
}

void GLItemTooltip::GenDurability( LuaTable& tb )
{
	LuaTable tbDurability( GLWidgetScript::GetInstance().GetLuaState() );	

	if( GLUseFeatures::GetInstance().IsUsingItemDurability() )
	{  
		tbDurability.set( "Dura", m_fDurability );

		if( m_pItemData->BasicType() == ITEM_SUIT )
		{
			// 내구도가 설정된 장비라면
			if( ItemDurability::GetInstance().IsUse( m_sItemCustom ) )
			{
				DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;

				// 내구도 컬러			
				if( m_fDurability >= 0 )
					dwColor = ItemDurability::GetInstance().GetTextColor( m_sItemCustom, dwColor );

				tbDurability.set( "Now", (int) ItemDurability::GetInstance().Get( m_sItemCustom ) );
				tbDurability.set( "Max", (int) ItemDurability::GetInstance().GetMax( m_sItemCustom ) );
				tbDurability.set( "Percent", (int) ItemDurability::GetInstance().GetPercent( m_sItemCustom ) );
				tbDurability.set( "Color", (int) dwColor );
			}
		}
	}

	tb.set( "Durability", tbDurability );
}

void GLItemTooltip::GenDamage( LuaTable& tb )
{
	LuaTable tbDamage( GLWidgetScript::GetInstance().GetLuaState() );

	EMCHARINDEX emCharIndex = GLCI_FIGHTER_M;
	WORD wLevel = 0;
	if ( m_pOtherCharData )
	{
		emCharIndex = m_pOtherCharData->GETCHARINDEX();
		wLevel = m_pOtherCharData->GETLEVEL();
	}
	else
	{
		emCharIndex = m_pCharacter->GetCharIndex();
		wLevel = m_pCharacter->GETLEVEL();
	}

	const GLPADATA& sDamage = m_sItemCustom.getdamage( 0.0f, emCharIndex, wLevel );

	//int nMinDmg = static_cast<int>( m_pItemData->sSuitOp.gdDamage.wLow ); 
	//int nMaxDmg = static_cast<int>( m_pItemData->sSuitOp.gdDamage.wHigh );
	//int nRndOptDamage = static_cast<int>( sDamage.wLow - m_pItemData->sSuitOp.gdDamage.wLow );
	int nMinDmg = static_cast<int>( m_sItemCustom.GetBasicAttackDamage().GetLow() ); 
	int nMaxDmg = static_cast<int>(  m_sItemCustom.GetBasicAttackDamage().GetHigh() );
	int nRndOptDamage = static_cast<int>( sDamage.wLow -  m_sItemCustom.GetBasicAttackDamage().GetLow() );
	int nExtraValue = static_cast<int>( m_sItemCustom.GETGRADE_DAMAGE() );
	BYTE uGRADE = m_sItemCustom.GETGRADE(EMGRINDING_DAMAGE);

	if(m_pItemDisguise != NULL  )
	{
		if(m_fDurability < 1.0f)
		{
			nRndOptDamage -= static_cast<int>( m_pItemDisguise->GetAttackDamage().GetLow() );
			nRndOptDamage = static_cast<int>( nRndOptDamage * m_fDurability );
			nExtraValue = static_cast<int>( nExtraValue * m_fDurability );

			nRndOptDamage += static_cast<int>( m_pItemDisguise->GetAttackDamage().GetLow() );
			GLDWDATA& damage = m_sItemCustom.GETDAMAGE();
			nMinDmg = static_cast<int>(damage.nNow * m_fDurability) - nRndOptDamage - nExtraValue;
			nMaxDmg = static_cast<int>(damage.nMax * m_fDurability) - nRndOptDamage - nExtraValue;
		}
	}
	else
	{
		if(m_fDurability < 1.0f)
		{
			nRndOptDamage = static_cast<int>( nRndOptDamage * m_fDurability );
			nExtraValue = static_cast<int>( nExtraValue * m_fDurability );
			GLDWDATA& damage = m_sItemCustom.GETDAMAGE();
			nMinDmg = static_cast<int>(damage.nNow * m_fDurability) - nRndOptDamage - nExtraValue;
			nMaxDmg = static_cast<int>(damage.nMax * m_fDurability) - nRndOptDamage - nExtraValue;
		}
	}

	tbDamage.set("Min", nMinDmg);
	tbDamage.set("Max", nMaxDmg);
	tbDamage.set("RndOpt", nRndOptDamage);
	tbDamage.set("Extra", nExtraValue);

	if ( GUID_NULL == m_sItemCustom.guid )
		tbDamage.set("bDontCreate", true);
	else
		tbDamage.set("bDontCreate", false);

	if ( -1 != m_pItemData->GetBasicAttackDamageIndex() )
		tbDamage.set("RandomStat", true);
	else
		tbDamage.set("RandomStat", false);

	tbDamage.set("bRebuild", m_pItemData->IsEditableBasicAttackDamage());

	tb.set("Damage", tbDamage);
}

void GLItemTooltip::GenDefence( LuaTable& tb )
{
	LuaTable tbDefence( GLWidgetScript::GetInstance().GetLuaState() );

	EMCHARINDEX emCharIndex = GLCI_FIGHTER_M;
	WORD wLevel = 0;
	if ( m_pOtherCharData )
	{
		emCharIndex = m_pOtherCharData->GETCHARINDEX();
		wLevel = m_pOtherCharData->GETLEVEL();
	}
	else
	{
		emCharIndex = m_pCharacter->GetCharIndex();
		wLevel = m_pCharacter->GETLEVEL();
	}

	//int nBasicDef = static_cast<int>( m_pItemData->sSuitOp.nDefense);
	//int nRndOptDef = static_cast<int>( 
	//	m_sItemCustom.getdefense( m_pCharacter->GETCHARINDEX(), m_pCharacter->GETLEVEL() ) - m_pItemData->sSuitOp.nDefense);
	int nBasicDef = static_cast<int>( m_sItemCustom.GetBasicDefence() );
	int nRndOptDef = static_cast<int>( 
			m_sItemCustom.getdefense( emCharIndex, wLevel ) - m_sItemCustom.GetBasicDefence());
		
	int nExtraValue = m_sItemCustom.GETGRADE_DEFENSE();
	BYTE uGRADE = m_sItemCustom.GETGRADE(EMGRINDING_DEFENSE);



	if( NULL != m_pItemDisguise )
	{
		if( m_fDurability < 1.0f )
		{
			nRndOptDef -= m_pItemDisguise->GetDefence(); 
			nRndOptDef = static_cast<int>( nRndOptDef * m_fDurability); 
			nExtraValue = static_cast<int>( nExtraValue * m_fDurability );
			nRndOptDef += m_pItemDisguise->GetDefence();
			nBasicDef = static_cast<int>(m_sItemCustom.GETDEFENSE()* m_fDurability) - nRndOptDef - nExtraValue;	
		}
	}
	else
	{
		if( m_fDurability < 1.0f )
		{
			nRndOptDef = static_cast<int>( nRndOptDef * m_fDurability );
			nExtraValue = static_cast<int>( nExtraValue * m_fDurability );
			nBasicDef = static_cast<int>(m_sItemCustom.GETDEFENSE() * m_fDurability) - nRndOptDef - nExtraValue;
		}
	}

	tbDefence.set("Def", nBasicDef);
	tbDefence.set("RndOpt", nRndOptDef);
	tbDefence.set("Extra", nExtraValue);

	if ( GUID_NULL == m_sItemCustom.guid )
		tbDefence.set("bDontCreate", true);
	else
		tbDefence.set("bDontCreate", false);

	if ( -1 != m_pItemData->GetBasicDefenceIndex() )
		tbDefence.set("RandomStat", true);
	else
		tbDefence.set("RandomStat", false);

	tbDefence.set("bRebuild", m_pItemData->IsEditableBasicDefence());

	tb.set("Defence", tbDefence);
}

void GLItemTooltip::GenHitRate( LuaTable& tb )
{
	LuaTable tbHitRate( GLWidgetScript::GetInstance().GetLuaState() );

	int nBasicHitRate = static_cast<int>( m_pItemData->sSuitOp.nHitRate );
	int nRndOptHitRate = static_cast<int>( m_sItemCustom.GETHITRATE() - m_pItemData->sSuitOp.nHitRate );

	if( m_pItemDisguise != NULL )
	{
		if(m_fDurability < 1.0f)
		{
			//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
			nRndOptHitRate -= m_pItemDisguise->sSuitOp.nHitRate;
			nRndOptHitRate = static_cast<int>( nRndOptHitRate * m_fDurability );	
			nRndOptHitRate += m_pItemDisguise->sSuitOp.nHitRate;
			nBasicHitRate = static_cast<int>( m_sItemCustom.GETHITRATE() * m_fDurability ) - nRndOptHitRate;	
		}
	}
	else
	{
		if(m_fDurability < 1.0f)
		{
			nRndOptHitRate = static_cast<int>( nRndOptHitRate * m_fDurability );
			// 실제 적용 수치와 튤팁 출력간의 오차 보정
			nBasicHitRate = static_cast<int>(m_sItemCustom.GETHITRATE() * m_fDurability) - nRndOptHitRate;
		}
	}

	tbHitRate.set( "Hit", nBasicHitRate );
	tbHitRate.set( "RndOpt", nRndOptHitRate );

	tb.set( "HitRate", tbHitRate );
}

void GLItemTooltip::GenAvoidRate( LuaTable& tb )
{
	LuaTable tbAvoidRate( GLWidgetScript::GetInstance().GetLuaState() );

	int nBasicAvoidRate = static_cast<int>( m_pItemData->sSuitOp.nAvoidRate );
	int nRndOptAvoidRate = static_cast<int>( m_sItemCustom.GETAVOIDRATE() - m_pItemData->sSuitOp.nAvoidRate );

	if(m_pItemDisguise != NULL)
	{
		if(m_fDurability < 1.0f)
		{
			//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
			nRndOptAvoidRate -= m_pItemDisguise->sSuitOp.nAvoidRate;
			nRndOptAvoidRate = static_cast<int>( (nRndOptAvoidRate -  m_pItemDisguise->sSuitOp.nAvoidRate) * m_fDurability ); 
			//코스튬 옵션은 랜덤 옵션에 포함됨
			nRndOptAvoidRate += m_pItemDisguise->sSuitOp.nAvoidRate;
			// 실제 적용 수치와 튤팁 출력간의 오차 보정
			nBasicAvoidRate = static_cast<int>(m_sItemCustom.GETAVOIDRATE() * m_fDurability) - nRndOptAvoidRate;
		}
	}	
	else
	{
		if( m_fDurability < 1.0f )
		{
			nRndOptAvoidRate = static_cast<int>( nRndOptAvoidRate * m_fDurability ); 
			nBasicAvoidRate = static_cast<int>(m_sItemCustom.GETAVOIDRATE() * m_fDurability) - nRndOptAvoidRate;
		}
	}

	tbAvoidRate.set( "Avoid", nBasicAvoidRate );
	tbAvoidRate.set( "RndOpt", nRndOptAvoidRate );

	tb.set( "AvoidRate", tbAvoidRate );
}

void GLItemTooltip::GenCostSp( LuaTable& tb )
{
	LuaTable tbCostSp( GLWidgetScript::GetInstance().GetLuaState() );

	int nBasicReqSP = static_cast<int>( m_pItemData->sSuitOp.wReqSP );
	int nRndOptReqSP = static_cast<int>( m_sItemCustom.GETREQ_SP() - m_pItemData->sSuitOp.wReqSP);

	if( NULL != m_pItemDisguise )
	{
		if( m_fDurability < 1.0f )
		{
			//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
			nRndOptReqSP -= static_cast<int>( m_pItemDisguise->sSuitOp.wReqSP );	
			nRndOptReqSP = static_cast<int>( nRndOptReqSP * m_fDurability );
			//코스튬 옵션은 랜덤 옵션에 포함됨
			nRndOptReqSP += static_cast<int>( m_pItemDisguise->sSuitOp.wReqSP );
			// 실제 적용 수치와 튤팁 출력간의 오차 보정
			nBasicReqSP = static_cast<int>( m_sItemCustom.GETREQ_SP()* m_fDurability ) - nRndOptReqSP;

		}
	}
	else
	{
		if( m_fDurability < 1.0f )
		{
			nRndOptReqSP = static_cast<int>( nRndOptReqSP * m_fDurability );
			// 실제 적용 수치와 튤팁 출력간의 오차 보정
			nBasicReqSP = static_cast<int>(m_sItemCustom.GETREQ_SP() * m_fDurability) - nRndOptReqSP;
		}
	}

	tbCostSp.set("Sp", nBasicReqSP);
	tbCostSp.set("RndOpt", nRndOptReqSP);

	tb.set("CostSp", tbCostSp);
}

void GLItemTooltip::GenPileNum( LuaTable& tb, int nType )
{
	LuaTable tbPileNum( GLWidgetScript::GetInstance().GetLuaState() );

	WORD wTurnNum(0);
	WORD wPileNum = m_pItemData->sDrugOp.wPileNum;
	if(nType == EMGFX_SLOT_NPCMARKET)
		wTurnNum = m_pItemData->GETAPPLYNUM();
	else
		wTurnNum = m_sItemCustom.wTurnNum;		

	tbPileNum.set("PileNum", wPileNum);	
	tbPileNum.set("TurnNum", wTurnNum);

	tb.set("Num", tbPileNum);
}

void GLItemTooltip::GenDrugUseEffect( LuaTable& tb )
{
	std::string strTemp;
	if ( m_pItemData->BasicType() == ITEM_CURE )
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			if( m_pItemData->sDrugOp.GetCureVolumn() == 0 )
			{
				strTemp = sc::string::format( "%s", 
					CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", m_pItemData->sDrugOp.emDrug).GetString() );
			}
			else
			{
				if( m_pItemData->sDrugOp.bRatio == true )
				{
					strTemp = sc::string::format( "%s :%d%%",
						CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", m_pItemData->sDrugOp.emDrug).GetString(),
						m_pItemData->sDrugOp.GetCureVolumn() );
				}
				else
				{
					strTemp = sc::string::format( "%s :%d",
						CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", m_pItemData->sDrugOp.emDrug).GetString(),
						m_pItemData->sDrugOp.GetCureVolumn() );
				}
			}
		}
		else
		{
			if( m_pItemData->sDrugOp.GetCureVolumn() == 0 )
			{
				strTemp = sc::string::format( "%s", COMMENT::ITEMDRUG[m_pItemData->sDrugOp.emDrug].c_str() );
			}
			else
			{
				if( m_pItemData->sDrugOp.bRatio == true )
				{
					strTemp = sc::string::format( "%s :%d%%",
						COMMENT::ITEMDRUG[m_pItemData->sDrugOp.emDrug].c_str(),
						m_pItemData->sDrugOp.GetCureVolumn() );
				}
				else
				{
					strTemp = sc::string::format( "%s :%d",
						COMMENT::ITEMDRUG[m_pItemData->sDrugOp.emDrug].c_str(),
						m_pItemData->sDrugOp.GetCureVolumn() );
				}
			}
		}
	}

	tb.set("DrugUseEffect", strTemp.c_str());
}

void GLItemTooltip::GenCoolTime( LuaTable& tb )
{
	LuaTable tbCoolTime( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbMaxTime( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbNowTime( GLWidgetScript::GetInstance().GetLuaState() );

	if ( m_pItemData->sBasicOp.IsCoolTime() )
	{
		std::string strTime("");
		CTimeSpan cCoolTime( m_pItemData->sBasicOp.dwCoolTime );
		tbMaxTime.set( "Hour", cCoolTime.GetHours() );
		tbMaxTime.set( "Minute", cCoolTime.GetMinutes() );
		tbMaxTime.set( "Second", cCoolTime.GetSeconds() );
	}

	if ( m_pCharacter && m_pCharacter->IsCoolTime( m_pItemData->sBasicOp.sNativeID ) )
	{
		std::string strTime = "";
		__time64_t tCurTime =  GfxInterface::GetInstance()->GetGaeaClient()->GetCurrentTime().GetTime();
		__time64_t tCoolTime = m_pCharacter->GetMaxCoolTime( m_pItemData->sBasicOp.sNativeID );				

		CTimeSpan cReTime( tCoolTime - tCurTime );

		tbNowTime.set( "Hour", cReTime.GetHours() );
		tbNowTime.set( "Minute", cReTime.GetMinutes() );
		tbNowTime.set( "Second", cReTime.GetSeconds() );
	}

	tbCoolTime.set( 1, tbMaxTime );
	tbCoolTime.set( 2, tbNowTime );

	tb.set( "CoolTime", tbCoolTime );
}

void GLItemTooltip::GenType( LuaTable& tb )
{
	const EMITEM_TYPE& emItemType = m_pItemData->BasicType();
	switch( emItemType )
	{
	case ITEM_SKILL:
		{
			GetSkillItem( tb );
		}
		break;

	case ITEM_PET_SKILL:
		{
			GetPetSkillItem( tb );
		}
		break;

	case ITEM_VEHICLE:
		{
			GetVehicleItem( tb );
		}
		break;

	case ITEM_PET_CARD:
		{
			GetPetCardItem( tb );
		}
		break;

	case ITEM_LUNCHBOX:
		{
			GetLunchBoxItem( tb );
		}
		break;

	case ITEM_PET_DUALSKILL:
		{
			GetPetDualSkillItem( tb );
		}
		break;

	case ITEM_POINT_CARD_REFUND:
	case ITEM_POINT_CARD_NOT_REFUND:
		{
			// Note : 포인트 양 표시.
			GetPointCard( tb );
		}
		break;

	case ITEM_RANDOMITEM:
	case ITEM_LOCK_BOX:
	case ITEM_BOX:
	case ITEM_SELECTIVEFORM_BOX:
		{
			if ( false == m_pItemData->sBox.ShowContents )
				break;

			GetBox( tb );
		}
		break;

	case ITEM_RECALL :
		{
			GenDrugCall( tb );
		}
		break;
	};
}

void GLItemTooltip::GetSkillItem( LuaTable& tb )
{
	LuaTable tbSkill( GLWidgetScript::GetInstance().GetLuaState() );

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_pItemData->sSkillBookOp.sSkill_ID );
	if(pSkill)
	{
		const SKILL::SBASIC& sBASIC = pSkill->m_sBASIC;

		if(sBASIC.wTARRANGE)
			tbSkill.set( "Range", sBASIC.wTARRANGE );		

		if ( sBASIC.emUSE_LITEM )
		{
			if ( sBASIC.emUSE_LITEM != SKILL::SKILLATT_NOCARE )
				tbSkill.set( "LItem", sBASIC.emUSE_LITEM - 1 );	// 왼손 아이템 텍스트 인덱스		
		}

		if ( sBASIC.emUSE_RITEM )
		{
			if ( sBASIC.emUSE_RITEM != SKILL::SKILLATT_NOCARE )
				tbSkill.set( "RItem", sBASIC.emUSE_RITEM - 1 );	// 오른손 아이템 텍스트 인덱스					
		}

		const WORD wLevel = 0;
		SKILL::SLEARN& sLEARN = pSkill->m_sLEARN;
		SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[wLevel];

		//	1. 요구보유스킬
		SNATIVEID NeedSkillID = pSkill->m_sLEARN.sSKILL;			
		if ( NeedSkillID != NATIVEID_NULL() )
		{
			PGLSKILL pNeedSkill = GLSkillMan::GetInstance().GetData( NeedSkillID.wMainID, NeedSkillID.wSubID );
			GLCharacter* pCharClient = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();	
		
			if ( pNeedSkill ) 
			{
				tbSkill.set( "SkName", pNeedSkill->GetName() );  	

				if ( pCharClient->ISLEARNED_SKILL(NeedSkillID ))
					tbSkill.set( "SkCompare", true );
				else
					tbSkill.set( "SkCompare", false );	
			}

			//	2. 요구보유스킬레벨
			if ( 0 < sLVL.dwSKILL_LVL )
			{
				tbSkill.set( "SkLevel", sLVL.dwSKILL_LVL + 1 ); 	

				if ( m_pOtherCharData )
				{
					SKILL_MAP& map = m_pOtherCharData->m_ExpSkills;				
					SKILL_MAP_ITER iter = map.find ( NeedSkillID.dwID );
					if ( iter != map.end() )
					{
						SCHARSKILL& rCharSkill = (*iter).second;

						if(rCharSkill.wLevel >= sLVL.dwSKILL_LVL)
							tbSkill.set( "SkLevelCompare", true );
						else
							tbSkill.set( "SkLevelCompare", false );
					}
				}
				else
				{
					SKILL_MAP& map = m_pCharacter->m_ExpSkills;				
					SKILL_MAP_ITER iter = map.find ( NeedSkillID.dwID );
					if ( iter != map.end() )
					{
						SCHARSKILL& rCharSkill = (*iter).second;

						if(rCharSkill.wLevel >= sLVL.dwSKILL_LVL)
							tbSkill.set( "SkLevelCompare", true );
						else
							tbSkill.set( "SkLevelCompare", false );
					}
				}
			}

		}

		//	3. 요구경험치
		if( 0 < sLVL.dwSKP )
		{
			tbSkill.set( "Exp", sLVL.dwSKP );

			DWORD dwSkillPoint = 0;
			if ( m_pOtherCharData )
				dwSkillPoint = m_pOtherCharData->m_dwSkillPoint;
			else
				dwSkillPoint = m_pCharacter->m_dwSkillPoint;

			if( dwSkillPoint >= sLVL.dwSKP )
				tbSkill.set( "ExpCompare", true );
			else
				tbSkill.set( "ExpCompare", false );
		}

		//	4. 요구레벨
		if( 0 < sLVL.dwLEVEL )
		{
			tbSkill.set( "Level", sLVL.dwLEVEL );

			WORD wLevel = 0;
			if ( m_pOtherCharData )
				wLevel = m_pOtherCharData->GETLEVEL();
			else
				wLevel = m_pCharacter->GETLEVEL();

			if( wLevel >= int(sLVL.dwLEVEL) )
				tbSkill.set( "LevelCompare", true );
			else
				tbSkill.set( "LevelCompare", false );
		}

		//	1. 요구힘
		SCHARSTATS sCharStats = m_pCharacter->m_sSUMSTATS_SKILL;
		if ( m_pOtherCharData )
		{
			sCharStats = m_pOtherCharData->m_sSUMSTATS_SKILL;
		}

		//	1. 요구힘
		if( 0 < sLVL.sSTATS.wPow )
		{
			tbSkill.set( "Pow", sLVL.sSTATS.wPow );

			if( sCharStats.wPow >= sLVL.sSTATS.wPow )
				tbSkill.set( "PowCompare", true );
			else
				tbSkill.set( "PowCompare", false );
		}

		// 2. 요구체력
		if( 0 < sLVL.sSTATS.wStr )
		{
			tbSkill.set( "Str", sLVL.sSTATS.wStr );	

			if( sCharStats.wStr >= sLVL.sSTATS.wStr )
				tbSkill.set( "StrCompare", true );	
			else
				tbSkill.set( "StrCompare", false );	
		}

		// 3. 요구정신
		if( 0 < sLVL.sSTATS.wSpi )
		{
			tbSkill.set( "Spi", sLVL.sSTATS.wSpi );	

			if( sCharStats.wSpi >= sLVL.sSTATS.wSpi )
				tbSkill.set( "SpiCompare", true );	
			else
				tbSkill.set( "SpiCompare", false );	
		}

		// 4. 요구민첩
		if( 0 < sLVL.sSTATS.wDex )
		{
			tbSkill.set( "Dex", sLVL.sSTATS.wDex );	

			if( sCharStats.wDex >= sLVL.sSTATS.wDex )
				tbSkill.set( "DexCompare", true );	
			else
				tbSkill.set( "DexCompare", false );	
		}

		// 5. 요구근력
		if( 0 < sLVL.sSTATS.wSta )
		{
			tbSkill.set( "Sta", sLVL.sSTATS.wSta );	

			if( sCharStats.wSta >= sLVL.sSTATS.wSta )
				tbSkill.set( "StaCompare", true );	
			else
				tbSkill.set( "StaCompare", false );	
		}
	}

	tb.set( "NeedSk", tbSkill );
}

void GLItemTooltip::GetPetSkillItem( LuaTable& tb )
{
	LuaTable tbPetSkill( GLWidgetScript::GetInstance().GetLuaState() );	
	GLPetClient* pPetClient = GfxInterface::GetInstance()->GetGaeaClient()->GetPetClient();	
	SNATIVEID sSkillID = m_pItemData->sSkillBookOp.sSkill_ID;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
	if ( pSkill )
	{
		//	이미 배운 스킬
		if ( pPetClient->ISLEARNED_SKILL( sSkillID ) )
			tbPetSkill.set( "Already", true );
		else
			tbPetSkill.set( "Already", false );	
	}

	tb.set( "PetSkill", tbPetSkill );
}

void GLItemTooltip::GetVehicleItem( LuaTable& tb )
{
	LuaTable tbVehicle( GLWidgetScript::GetInstance().GetLuaState() );	
	bool bInfo = true;

	VEHICLEITEMINFO_MAP_ITER iter = m_pCharacter->m_mapVEHICLEItemInfo.find ( m_sItemCustom.m_VehicleDbNum );
	if ( iter == m_pCharacter->m_mapVEHICLEItemInfo.end() )
	{
		iter = m_pCharacter->m_mapVEHICLEItemInfoTemp.find ( m_sItemCustom.m_VehicleDbNum );
		if (iter == m_pCharacter->m_mapVEHICLEItemInfoTemp.end())
			bInfo = false;
	}

	if ( bInfo ) 
	{
		SVEHICLEITEMINFO sVehicleItemInfo = (*iter).second;
		int Count(1);
		for ( int i = 0; i < ACCE_TYPE_SIZE; ++i ) 
		{
			const SITEM* pItem = GLogicData::GetInstance().GetItem ( sVehicleItemInfo.m_PutOnItems[i].GetNativeID() );
			if ( pItem )
			{	
				LuaTable tbVehicleSkill( GLWidgetScript::GetInstance().GetLuaState() );	

				tbVehicleSkill.set( "Name", pItem->GetName() );
				tbVehicleSkill.set( "Parts", i );	
				tbVehicle.set( Count, tbVehicleSkill );
				++Count;
			}
		}

		tbVehicle.set( "BoostLearn", sVehicleItemInfo.m_dwBoosterID );
		tbVehicle.set( "Battery", sVehicleItemInfo.m_nFull/10.0f );
	}

	tb.set( "Vehicle", tbVehicle );
}

void GLItemTooltip::GetPetCardItem( LuaTable& tb )
{
	LuaTable tbPetCard( GLWidgetScript::GetInstance().GetLuaState() );	

	PETCARDINFO_MAP_ITER iter = m_pCharacter->m_mapPETCardInfo.find( m_sItemCustom.PetDbNum() );
	if ( iter == m_pCharacter->m_mapPETCardInfo.end() )
	{
		iter = m_pCharacter->m_mapPETCardInfoTemp.find ( m_sItemCustom.PetDbNum() );
		if ( iter == m_pCharacter->m_mapPETCardInfoTemp.end() ) return;
	}

	SPETCARDINFO sPetCardInfo = (*iter).second;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string>	vecPET_TYPE;
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("PET_TYPE", vecPET_TYPE, CGameTextMan::EM_COMMENT_TEXT);
		tbPetCard.set( "Type", vecPET_TYPE[sPetCardInfo.m_emTYPE].c_str() );
	}
	else
	{
		tbPetCard.set( "Type", COMMENT::PET_TYPE[sPetCardInfo.m_emTYPE].c_str() );
	}

	tbPetCard.set( "Satiety", sPetCardInfo.m_nFull/10.0f ); //포만도

	if( 2 <= sPetCardInfo.m_wMaxSkillSlot )
		tbPetCard.set( "DualSk", true );
	else
		tbPetCard.set( "DualSk", false );

	const SITEM* pItem = GLogicData::GetInstance().GetItem( sPetCardInfo.m_PutOnItems[ACCETYPEA].GetNativeID() );	
	if ( pItem )
		tbPetCard.set( "PartAName", pItem->GetName() ); //A파츠

	pItem = GLogicData::GetInstance().GetItem( sPetCardInfo.m_PutOnItems[ACCETYPEB].GetNativeID() );
	if ( pItem )
		tbPetCard.set( "PartBName", pItem->GetName() ); //B파츠

	if ( m_sItemCustom.tDISGUISE != 0 )
	{
		const CTime& currentTime = GfxInterface::GetInstance()->GetGaeaClient()->GetCurrentTime();
		CTime startTime   = m_sItemCustom.tBORNTIME;
		CTimeSpan timeSpan = currentTime - startTime;
		if( timeSpan.GetTotalSeconds() < (m_sItemCustom.tDISGUISE + m_sItemCustom.GetPeriodExDisguise()) )	
			tbPetCard.set( "DisguiseTime", m_sItemCustom.tDISGUISE + m_sItemCustom.GetPeriodExDisguise() - timeSpan.GetTotalSeconds() );
	}

	bool bDisplayEnablePetSkill = false;
	int Count(1);
	PETSKILL_MAP_CITER pos = sPetCardInfo.m_ExpSkills.begin();
	PETSKILL_MAP_CITER pos_end = sPetCardInfo.m_ExpSkills.end();
	for ( ;pos != pos_end; ++pos )
	{
		LuaTable tbPetSkill( GLWidgetScript::GetInstance().GetLuaState() );	
		const PETSKILL& sPetSkill = (*pos).second;
		PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData( sPetSkill.sNativeID );
		if ( pSKILL )
		{
			if ( !bDisplayEnablePetSkill )
				bDisplayEnablePetSkill = true;			

			tbPetSkill.set( 1, !bDisplayEnablePetSkill );
			tbPetSkill.set( 2, pSKILL->GetName() );			 		
		}

		tbPetCard.set( Count, tbPetSkill );
		++Count;
	}

	tb.set( "PetCard", tbPetCard );
}

void GLItemTooltip::GetLunchBoxItem( LuaTable& tb )
{
	LuaTable tbLunchBox( GLWidgetScript::GetInstance().GetLuaState() );	

	SNATIVEID sSkillID = m_pItemData->sSkillBookOp.sSkill_ID;
	WORD wSkillLevel = m_pItemData->sSkillBookOp.wSkill_Level;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
	if ( !pSkill )
		return;

	CString strImpact;
	pSkill->GetDescFormatByLevel( strImpact, wSkillLevel, GLSKILL::DESC_FORMAT_IMPACT | GLSKILL::DESC_FORMAT_SPEC );
	tbLunchBox.set( "Explain", strImpact.GetBuffer() );

	tb.set( "LunchBox", tbLunchBox );
}

void GLItemTooltip::GetPetDualSkillItem( LuaTable& tb )
{
	LuaTable tbPetDualSkill( GLWidgetScript::GetInstance().GetLuaState() );	
	GLPetClient* pPetClient = GfxInterface::GetInstance()->GetGaeaClient()->GetPetClient();

	if ( 2 <= pPetClient->m_wMaxSkillSlot )
		tbPetDualSkill.set( "Learn", true );
	else
		tbPetDualSkill.set( "Learn", false );

	tb.set( "PetDualSk", tbPetDualSkill );
}

void GLItemTooltip::GetPointCard( LuaTable& tb )
{
	LuaTable tbPointCard( GLWidgetScript::GetInstance().GetLuaState() );	
	tbPointCard.set( "Point", m_pItemData->sDrugOp.GetCureVolumn() );
	tb.set( "PointCard", tbPointCard );
}

void GLItemTooltip::GetBox( LuaTable& tb )
{
	LuaTable tbBoxItem( GLWidgetScript::GetInstance().GetLuaState() );

	int nTableCount = 1;

	// ITEM_BOX, ITEM_SELECTIVEFORM_BOX, ITEM_RANDOMITEM, ITEM_LOCK_BOX
	const EMITEM_TYPE emItemType( m_pItemData->BasicType() );
	const INT nBoxSize =
		( emItemType == ITEM_BOX || emItemType == ITEM_SELECTIVEFORM_BOX ) ?
		m_pItemData->sBox.vecItems.size() : m_pItemData->sRandomBox.vecBOX.size();

	ITEM::VEC_BOX vecBox = m_pItemData->sBox.vecItems;
	ITEM::VEC_RANDOMBOX vecRanBox = m_pItemData->sRandomBox.vecBOX;

	for( int i = 0; i < nBoxSize; ++i )
	{
		SITEMCUSTOM sBoxCustom =
			( emItemType == ITEM_BOX || emItemType == ITEM_SELECTIVEFORM_BOX ) ?
			m_pItemData->sBox.vecItems[i].nidITEM : m_pItemData->sRandomBox.vecBOX[i].nidITEM;
		if( false == sBoxCustom.GetNativeID().IsValidNativeID() )
			continue;

		const SITEM* pBoxItem = GLogicData::GetInstance().GetItem( sBoxCustom.GetNativeID() );
		if( NULL == pBoxItem )
			continue;

		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, pBoxItem->BasicId().Mid() );
		tbItem.set( 2, pBoxItem->BasicId().Sid() );
		if ( emItemType == ITEM_BOX || emItemType == ITEM_SELECTIVEFORM_BOX )
			tbItem.set( 3, 0 == m_pItemData->sBox.vecItems[ i ].dwAMOUNT ? 1 : m_pItemData->sBox.vecItems[ i ].dwAMOUNT );
		else
			tbItem.set( 3, 1 );


		tbBoxItem.set( nTableCount++, tbItem );
	}

	tb.set( "tbBoxItem", tbBoxItem );

	// 박스 아이템 미리보기 가능?
	tb.set( "BoxPreview", m_pItemData->sBox.ShowContents );
}

void GLItemTooltip::GenAddOpt( LuaTable& tb, bool bSimple )
{
	LuaTable tbAddOpt( GLWidgetScript::GetInstance().GetLuaState() );	
	const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;
//	EMITEM_VAR emDisguiseItemVol = NULL != m_pItemDisguise ? m_pItemDisguise->sSuitOp.sVOLUME.emTYPE : EMVAR_NONE;
	
	ITEM::SADDON arrVALUE[EMADD_SIZE];
	SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );
	
	// 기력치는 랜덤옵션의 영향을 받기 때문에 추가 계산이 필요하다;
	int nOption_MagicDamage = 0;

	for ( int i = 0; i < SITEMCUSTOM_ADDON::ADDON_SIZE; ++i )
	{
		bool bGen(true);
		if ( GUID_NULL == m_sItemCustom.guid )
			bGen = false;

		EMITEM_ADDON emTYPE = bGen ? m_sItemCustom.sAddonInfo.m_sADDON[i].emTYPE : m_pItemData->sSuitOp.sADDON[i].emTYPE;
		arrVALUE[emTYPE].emTYPE = emTYPE;

		// 가산옵션 값을 가져온다;
		const DWORD dwType = static_cast< DWORD >( emTYPE );
		const int nValue = static_cast< int >( m_sItemCustom.sAddonInfo.GetValue< int >( i, ITEM::bAddonValueInteger[ dwType ] ) * m_fDurability );
		const float fValue = static_cast< float >( m_sItemCustom.sAddonInfo.GetValue< float >( i, ITEM::bAddonValueInteger[ dwType ] ) * m_fDurability );


		if ( emTYPE != EMADD_NONE )
		{
			//내구도 적용
			if ( bGen )
			{
				if ( ITEM::bAddonValueInteger[emTYPE] )
					arrVALUE[ emTYPE ].nVALUE += nValue;
				else
					arrVALUE[ emTYPE ].fVALUE += fValue;
			}
			else
			{
				if ( ITEM::bAddonValueInteger[emTYPE] )
					arrVALUE[ emTYPE ].nVALUE += nValue;
				else
					arrVALUE[ emTYPE ].fVALUE += fValue;
			}

			// 먼저 가산옵션의 기력치를 세팅한다;
			// 최초에는 가산옵션의 기력치가 기본값이기 때문에 대입한다;
			if ( EMADD_MA == emTYPE )
			{
				nOption_MagicDamage = nValue;
			}

			// 랜덤일 경우 값에 그 개수를 세팅하도록 한다;
			if ( EMADD_RANDOM == emTYPE && false == bSimple )
			{
				if ( ITEM::bAddonValueInteger[emTYPE] )
					arrVALUE[ emTYPE ].nVALUE += 1;
				else
					arrVALUE[ emTYPE ].fVALUE += 1.f;
			}
		}
	}

	/// 랜덤옵션의 퍼센트대미지, 대미지, 기력치 값을 이용해 최종 기력치를 계산한다;
	if ( false == bSimple )
	{
		float fDamage_Rate = m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_DAMAGE);
		float fDamage_Value = m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DAMAGE);
		float fMagic_Value = m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MAGIC);

		// 퍼센트 대미지를 계산한다;
		nOption_MagicDamage += static_cast< int >( nOption_MagicDamage * fDamage_Rate * 0.01 );

		// 대미지 + 기력치 를 더해준다;
		nOption_MagicDamage += static_cast< int >( fDamage_Value );
		nOption_MagicDamage += static_cast< int >( fMagic_Value );

		if ( ITEM::bAddonValueInteger[EMADD_MA] )
			arrVALUE[ EMADD_MA ].nVALUE = nOption_MagicDamage;
		else
			arrVALUE[ EMADD_MA ].fVALUE = static_cast< float >( nOption_MagicDamage );
	}


	//arrVALUE[EMADD_HP] = static_cast<int>( m_sItemCustom.GETADDHP() * m_fDurability );
	//arrVALUE[EMADD_MP] = static_cast<int>( m_sItemCustom.GETADDMP() * m_fDurability );
	//arrVALUE[EMADD_SP] = static_cast<int>( m_sItemCustom.GETADDSP() * m_fDurability );
	//arrVALUE[EMADD_MA] = static_cast<int>( m_sItemCustom.GETADDMA() * m_fDurability );

//	코스튬 능력치
	if ( false == bSimple )
	{
		for( int i = 0; i < E_ADDED_MAX; ++i)
		{
			BYTE iType = (BYTE)(E_ADD_HITRATE+i);

			COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

			if( m_sItemCustom.costumeUser.getTypeInfo( iType, sData ) )
			{
				double iValue = COSSDATA.GetStatInc( m_pItemData->sSuitOp.emSuit, (COSTUME_STATS)(E_ADD_HITRATE+i) );

				int nApplyValue = (int)(iValue*sData.wInvestPt);

				COSTUME_STATS eType = (COSTUME_STATS)(iType);
				switch( eType )
				{
				case E_ADD_HITRATE   : arrVALUE[EMADD_HITRATE].nVALUE += nApplyValue; break;
				case E_ADD_AVOIDRATE : arrVALUE[EMADD_AVOIDRATE].nVALUE += nApplyValue; break;
				case E_ADD_DAMAGE    : arrVALUE[EMADD_DAMAGE].nVALUE += nApplyValue; break;
				case E_ADD_DEFENSE   : arrVALUE[EMADD_DEFENSE].nVALUE += nApplyValue; break;
				case E_ADD_HP        : arrVALUE[EMADD_HP].nVALUE += nApplyValue; break;
				case E_ADD_STATS_POW : arrVALUE[EMADD_STATS_POW].nVALUE += nApplyValue; break;
				case E_ADD_STATS_STA : arrVALUE[EMADD_STATS_STR].nVALUE += nApplyValue; break;
				case E_ADD_STATS_SPI : arrVALUE[EMADD_STATS_SPI].nVALUE += nApplyValue; break;
				case E_ADD_STATS_DEX : arrVALUE[EMADD_STATS_DEX].nVALUE += nApplyValue; break;
				case E_ADD_ENERGY    : arrVALUE[EMADD_MA].nVALUE += nApplyValue; break;
				case E_ADD_SHOOTING  : arrVALUE[EMADD_SA].nVALUE += nApplyValue; break;
				case E_ADD_MELEE     : arrVALUE[EMADD_PA].nVALUE += nApplyValue; break;
				}
			}
		}

		float fCostumVolume = 0.0f;

		for( int i = 0; i < E_VARRATE_MAX; ++i)
		{
			BYTE iType = (BYTE)(E_R_HP_INC+i);

			COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

			if( m_sItemCustom.costumeUser.getTypeInfo(iType, sData) )
			{
				float fValue = (float)COSSDATA.GetStatInc( m_pItemData->sSuitOp.emSuit, (COSTUME_STATS)(E_R_HP_INC+i));

				float fApplyValue = fValue*sData.wInvestPt;

				COSTUME_STATS eType = (COSTUME_STATS)iType;

				switch(eType)
				{
				case E_R_HP_INC      : ITEM::bAddonValueInteger[EMADDEX_INCR_HP] ? arrVALUE[EMADDEX_INCR_HP].nVALUE += fApplyValue : arrVALUE[EMADDEX_INCR_HP].fVALUE += fApplyValue; break;
				case E_R_MP_INC      : ITEM::bAddonValueInteger[EMADDEX_INCR_MP] ? arrVALUE[EMADDEX_INCR_MP].nVALUE += fApplyValue : arrVALUE[EMADDEX_INCR_MP].fVALUE += fApplyValue; break;
				case E_R_SP_INC      : ITEM::bAddonValueInteger[EMADDEX_INCR_SP] ? arrVALUE[EMADDEX_INCR_SP].nVALUE += fApplyValue : arrVALUE[EMADDEX_INCR_SP].fVALUE += fApplyValue; break;
				case E_R_HMSP_INC    : ITEM::bAddonValueInteger[EMADDEX_INCR_AP] ? arrVALUE[EMADDEX_INCR_AP].nVALUE += fApplyValue : arrVALUE[EMADDEX_INCR_AP].fVALUE += fApplyValue; break;
				case E_R_MOVESPEED   : ITEM::bAddonValueInteger[EMADDEX_INCR_MOVE_SPEED] ? arrVALUE[EMADDEX_INCR_MOVE_SPEED].nVALUE += fApplyValue : arrVALUE[EMADDEX_INCR_MOVE_SPEED].fVALUE += fApplyValue; break;
				case E_R_ATTACKSPEED : ITEM::bAddonValueInteger[EMADDEX_INCR_ATTACK_SPEED] ? arrVALUE[EMADDEX_INCR_ATTACK_SPEED].nVALUE += fApplyValue : arrVALUE[EMADDEX_INCR_ATTACK_SPEED].fVALUE += fApplyValue; break;
				case E_R_CRITICALHIT : ITEM::bAddonValueInteger[EMADDEX_INCR_CRITICAL_RATE] ? arrVALUE[EMADDEX_INCR_CRITICAL_RATE].nVALUE += fApplyValue : arrVALUE[EMADDEX_INCR_CRITICAL_RATE].fVALUE += fApplyValue; break;
				case E_R_STRIKEHIT   : ITEM::bAddonValueInteger[EMADDEX_INCR_CRUSHING_BLOW] ? arrVALUE[EMADDEX_INCR_CRUSHING_BLOW].nVALUE += fApplyValue : arrVALUE[EMADDEX_INCR_CRUSHING_BLOW].fVALUE += fApplyValue; break;
				}
			}
		}

		// 기존 코스튬에 있는 추가옵션은 커스텀 값에 등록되어있지 않아
		// 기존 코스튬 값은 따로 넣어줬습니다.
		// 문제가 생긴다면 기존 코스튬값도 커스텀 값에 등록하는 방향으로 수정
		// 하는 편이 좋아봅니다.

		// 일반 템은 위에서 따로 예외처리할것들 처리하고 넣는데;
		// 코스튬은 그냥 넣어도 상관없는가?;
		if( m_pItemDisguise != NULL )
		{
			for( int j = 0; j < ITEM::SSUIT::ADDON_SIZE; ++j )
			{
				if( m_pItemDisguise->sSuitOp.sADDON == NULL)
					break ;

				ITEM::SADDON sAddon = m_pItemDisguise->sSuitOp.sADDON[j];
				if( sAddon.IsNoneType() )
					continue ;

				// arrVALUE[sAddon.emTYPE] += sAddon.nVALUE;
				if ( ITEM::bAddonValueInteger[sAddon.emTYPE] )
					arrVALUE[sAddon.emTYPE].nVALUE += sAddon.nVALUE;
				else 
					arrVALUE[sAddon.emTYPE].fVALUE += sAddon.fVALUE;
			}
		}
	}

	// 아래 작업하던중;
	INT nTableCount(0);
	for( int i = 1; i < EMADD_SIZE; ++i )
	{
		if ( ITEM::bAddonValueInteger[i] )
		{
			if( arrVALUE[i].nVALUE != 0 || arrVALUE[i].emTYPE == EMADD_RANDOM )
			{
				LuaTable tbOpt( GLWidgetScript::GetInstance().GetLuaState() );	
				tbOpt.set( 1 , true );	// int인가?;
				tbOpt.set( 2, arrVALUE[i].nVALUE );
				tbOpt.set( 3, i );
				tbAddOpt.set( ++nTableCount , tbOpt );
 			}
		}
		else
		{
			if( arrVALUE[i].fVALUE != 0.0f || arrVALUE[i].emTYPE == EMADD_RANDOM )
			{
				LuaTable tbOpt( GLWidgetScript::GetInstance().GetLuaState() );	
				const bool bRate(COMMENT::ITEMADDON_VAR_SCALE[i] != -1);
				if ( bRate 
					&& m_pItemData->BasicType() != ITEM_PET_FOOD
					&& m_pItemData->BasicType() != ITEM_VEHICLE_OIL )
					arrVALUE[i].fVALUE *= COMMENT::ITEMADDON_VAR_SCALE[i];

				float fValue = 0.0f;
				if( (EMITEM_ADDON)i == EMVAR_MOVE_SPEED )
					fValue = m_sItemCustom.GETMOVESPEED() * m_fDurability;

				CString strTemp = sc::string::format( "%.2f", arrVALUE[i].fVALUE ).c_str();

				CutZero( strTemp, 2 );
				tbOpt.set( 1, false);	// int인가?;
				tbOpt.set( 2, strTemp.GetBuffer() );
				tbOpt.set( 3, i );
				tbOpt.set( 4, bRate );

				tbAddOpt.set( ++nTableCount , tbOpt );
			}
		}
	}
	
	tb.set( "AddOpt", tbAddOpt );
}

void GLItemTooltip::GenExp(LuaTable &_table)
{
	// 경험치 배율
	float fExpMultiple = m_pItemData->GetExpMultiple(); //내구도 적용
	LuaTable tbExp( GLWidgetScript::GetInstance().GetLuaState() );	

	//내구도 적용
	if( (fExpMultiple > 1.0f) || (m_fDurability < 1.0f) )
	{
		fExpMultiple = ( (fExpMultiple - 1.0f) * m_fDurability ) + 1.0f;
	}

	COSTUMEUSERVALUE::sCostumeUser::sInvest sData;
	if( m_sItemCustom.costumeUser.getTypeInfo( (BYTE) E_EXPMULTIPLE, sData ) )
	{
		float fValue = (float)COSSDATA.GetStatInc( m_pItemData->sSuitOp.emSuit, E_EXPMULTIPLE);
		float fApplyValue = fValue*sData.wInvestPt;

		fExpMultiple += fApplyValue;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem(m_sItemCustom.nidDISGUISE);

	SITEMCUSTOM sCustomeItem(m_sItemCustom.nidDISGUISE);
	if ( pItem && sCustomeItem.IsDisguise() )
	{
		fExpMultiple += pItem->GetExpMultiple() - 1.0f;
		fExpMultiple = max( fExpMultiple, 1.0f );
	}

	if ( fExpMultiple != 1.0f )
	{
		CString strTemp = sc::string::format( "%.2f", fExpMultiple * 100.0f - 100.0f ).c_str();
		CutZero( strTemp, 2 );
		tbExp.set( "Exp", strTemp.GetBuffer() );
	}

	_table.set( "ExpMultiple", tbExp );
}

void GLItemTooltip::GenResist( LuaTable& tb )
{
	LuaTable tbResist( GLWidgetScript::GetInstance().GetLuaState() );	
	LuaTable tbResistAdd( GLWidgetScript::GetInstance().GetLuaState() );	

	const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;

	float fRegist = 0.0f;

	COSTUMEUSERVALUE::sCostumeUser::sInvest sData;
	if( m_sItemCustom.costumeUser.getTypeInfo( (BYTE) E_RESIST, sData ) )
	{
		float fValue = (float) COSSDATA.GetStatInc( m_pItemData->sSuitOp.emSuit, E_RESIST );
		fRegist = fValue * sData.wInvestPt;
	}

	int nELEC   = sSUIT.sResist.nElectric;
	int nFIRE   = sSUIT.sResist.nFire;
	int nICE    = sSUIT.sResist.nIce;
	int nPOISON = sSUIT.sResist.nPoison;
	int nSPIRIT = sSUIT.sResist.nSpirit;

	int nAddELEC( static_cast<int>(m_sItemCustom.GETRESIST_ELEC() - nELEC) ); 
	int nAddFIRE( static_cast<int>(m_sItemCustom.GETRESIST_FIRE() - nFIRE) ); 
	int nAddICE( static_cast<int>(m_sItemCustom.GETRESIST_ICE() - nICE) ); 
	int nAddPOISON( static_cast<int>(m_sItemCustom.GETRESIST_POISON() - nPOISON) ); 
	int nAddSPIRIT( static_cast<int>(m_sItemCustom.GETRESIST_SPIRIT() - nSPIRIT) );

	if ( m_pItemDisguise != NULL )
	{
		SITEMCUSTOM sCustomeItem(m_sItemCustom.nidDISGUISE);
		if( m_fDurability < 1.0f )
		{
			nELEC   = static_cast<int>( nELEC * m_fDurability );
			nFIRE   = static_cast<int>( nFIRE * m_fDurability );
			nICE    = static_cast<int>( nICE * m_fDurability );
			nPOISON = static_cast<int>( nPOISON * m_fDurability );
			nSPIRIT = static_cast<int>( nSPIRIT * m_fDurability );

			nAddELEC   = static_cast<int>( nAddELEC * m_fDurability );
			nAddFIRE   = static_cast<int>( nAddFIRE * m_fDurability );
			nAddICE    = static_cast<int>( nAddICE * m_fDurability );
			nAddPOISON = static_cast<int>( nAddPOISON * m_fDurability );
			nAddSPIRIT = static_cast<int>( nAddSPIRIT * m_fDurability );
		}

		if( m_pItemDisguise && sCustomeItem.IsDisguise() )
		{
			nAddELEC += m_pItemDisguise->sSuitOp.sResist.nElectric;
			nAddFIRE += m_pItemDisguise->sSuitOp.sResist.nFire;
			nAddICE += m_pItemDisguise->sSuitOp.sResist.nIce;
			nAddPOISON += m_pItemDisguise->sSuitOp.sResist.nPoison;
			nAddSPIRIT += m_pItemDisguise->sSuitOp.sResist.nSpirit;
		}
	}
	else
	{
		if( m_fDurability < 1.0f )
		{
			nELEC   = static_cast<int>( nELEC   * m_fDurability ) + (int) fRegist;
			nFIRE   = static_cast<int>( nFIRE   * m_fDurability ) + (int) fRegist;
			nICE    = static_cast<int>( nICE    * m_fDurability ) + (int) fRegist;
			nPOISON = static_cast<int>( nPOISON * m_fDurability ) + (int) fRegist;
			nSPIRIT = static_cast<int>( nSPIRIT * m_fDurability ) + (int) fRegist;

			nAddELEC   = static_cast<int>( nAddELEC   * m_fDurability );
			nAddFIRE   = static_cast<int>( nAddFIRE   * m_fDurability );
			nAddICE    = static_cast<int>( nAddICE    * m_fDurability );
			nAddPOISON = static_cast<int>( nAddPOISON * m_fDurability );
			nAddSPIRIT = static_cast<int>( nAddSPIRIT * m_fDurability );
		}
	}

	if ( nELEC || nFIRE || nICE || nPOISON || nSPIRIT ||
		nAddELEC || nAddFIRE || nAddICE || nAddPOISON || nAddSPIRIT )
	{
		tbResist.set( "Spirit", nSPIRIT );
		tbResist.set( "Fire", nFIRE );
		tbResist.set( "Ice", nICE );
		tbResist.set( "Elec", nELEC );
		tbResist.set( "Poison", nPOISON );

		tbResistAdd.set( "Spirit", nAddSPIRIT );
		tbResistAdd.set( "Fire", nAddFIRE );
		tbResistAdd.set( "Ice", nAddICE );
		tbResistAdd.set( "Elec", nAddELEC );
		tbResistAdd.set( "Poison", nAddPOISON );
	}

	tb.set( "Resist", tbResist );
	tb.set( "ResistAdd", tbResistAdd );
}

void GLItemTooltip::GenItemSkillLink( LuaTable& tb )
{
	LuaTable tbStateItemSkillLink( GLWidgetScript::GetInstance().GetLuaState() );

	//if( m_pItemData->sItemSkill.sSkillID.IsValidNativeID() )
	const SNATIVEID& sLinkSkillID = m_sItemCustom.GetLinkSkillID();

	//PGLSKILL pSkill = GLSkillMan::GetInstance().GetData(m_pItemData->sItemSkill.sSkillID);
	PGLSKILL pSkill = NULL;
	if ( sLinkSkillID.IsValidNativeID() )
	{
		pSkill = GLSkillMan::GetInstance().GetData( sLinkSkillID );
	}

	if( pSkill == NULL )
	{
		tbStateItemSkillLink.set( 1, false );
		tbStateItemSkillLink.set( 2, 0 );
		tbStateItemSkillLink.set( 3, 0 );
		tbStateItemSkillLink.set( 4, 0 );
		tbStateItemSkillLink.set( 5, "" );
		tbStateItemSkillLink.set( 6, 0 );
	}
	else
	{
		std::string strOccurRate = "";
		float fOccurRate = m_sItemCustom.GetLinkSkillOccurRate() * 100;
		if ( fOccurRate >= 1.f )
			strOccurRate = sc::string::format( "%d", static_cast< int >( m_sItemCustom.GetLinkSkillOccurRate()*100 ) );
		else
			strOccurRate = sc::string::format( "%.1f", m_sItemCustom.GetLinkSkillOccurRate()*100 );

		tbStateItemSkillLink.set( 1, true );
		tbStateItemSkillLink.set( 2, pSkill->GetId().wMainID );
		tbStateItemSkillLink.set( 3, pSkill->GetId().wSubID );
		//tbStateItemSkillLink.set(3, (int)(m_pItemData->sItemSkill.fOccuRate*100) );
		tbStateItemSkillLink.set( 4, strOccurRate );
		tbStateItemSkillLink.set( 5, pSkill->GetNameStr().c_str() );
		//tbStateItemSkillLink.set(5, (int)m_pItemData->sItemSkill.emSkillTarget );
		tbStateItemSkillLink.set( 6, (int)m_sItemCustom.GetLinkSkillTarget() );
	}

	// 생성하기 전인 아이템인지 확인한다;
	if ( GUID_NULL == m_sItemCustom.guid )
		tbStateItemSkillLink.set(7, true);
	else
		tbStateItemSkillLink.set(7, false);

	// 개조 가능한 아이템인지 판단한다;
	if ( -1 != m_pItemData->GetLinkSkillIndex() )
		tbStateItemSkillLink.set(8, true);
	else
		tbStateItemSkillLink.set(8, false);

	tb.set("ItemSkLink", tbStateItemSkillLink );
}

void GLItemTooltip::GenStateBlow( LuaTable& tb )
{
	LuaTable tbStateBlow( GLWidgetScript::GetInstance().GetLuaState() );	
	EMSTATE_BLOW emBLOW = m_pItemData->sSuitOp.sBLOW.emTYPE;

	if ( emBLOW !=EMBLOW_NONE )
	{
		float fLife = m_pItemData->sSuitOp.sBLOW.fLIFE;
		float fRate = m_pItemData->sSuitOp.sBLOW.fRATE;

		float fVAR1( 0.0f );
		float fVAR2( 0.0f );
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			fVAR1 = floorf( m_pItemData->sSuitOp.sBLOW.fVAR1 * COMMENT::BLOW_VAR1_SCALE[emBLOW] );
			fVAR2 = floorf( m_pItemData->sSuitOp.sBLOW.fVAR2 * COMMENT::BLOW_VAR2_SCALE[emBLOW] );
		}
		else
		{
			fVAR1 = floorf( m_pItemData->sSuitOp.sBLOW.fVAR1 * COMMENT::BLOW_VAR1_SCALE[emBLOW] );
			fVAR2 = floorf( m_pItemData->sSuitOp.sBLOW.fVAR2 * COMMENT::BLOW_VAR2_SCALE[emBLOW] );
		}        

		ITEM_TOOLTIP_STATE_BLOW emStateBlow = ITEM_TOOLTIP_VALUE;

		if ( fVAR1 != 0.0f )
		{
			if ( fVAR2 != 0.0f )
				emStateBlow = ITEM_TOOLTIP_VALUE_COUNT_2;
			else
				emStateBlow = ITEM_TOOLTIP_VALUE_COUNT_1_VAR1;
		}
		else
		{
			if( fVAR2 != 0.0f )
				emStateBlow = ITEM_TOOLTIP_VALUE_COUNT_1_VAR2;
		}

		switch ( emBLOW )
		{
		case EMBLOW_NUMB:   //	마비.
			{
				//확률은 수치값이 100%가 더해져서 적용되고 있다 표기출력은 - 100% (ex.딜레이변화 1.3 >> 툴팁 표시 스킬딜레이 +30%)
				if(fVAR2 > (1.0f * COMMENT::BLOW_VAR2_SCALE[emBLOW])) 
					fVAR2 -=  (1.0f * COMMENT::BLOW_VAR2_SCALE[emBLOW]);		
				tbStateBlow.set( "Type", EMBLOW_NUMB );
				tbStateBlow.set( "Rate", fRate );
				tbStateBlow.set( "Time", fLife );
				tbStateBlow.set( "Value1", fVAR1 );
				tbStateBlow.set( "Value2", fVAR2 );

			}
			break;

		case EMBLOW_STUN:   //	기절.
			{	
				tbStateBlow.set( "Type", EMBLOW_STUN );
				tbStateBlow.set( "Rate", fRate );
				tbStateBlow.set( "Time", fLife );
			}
			break;

		case EMBLOW_STONE:  //	석화.
			{
				tbStateBlow.set( "Type", EMBLOW_STONE );
				tbStateBlow.set( "Rate", fRate );
				tbStateBlow.set( "Time", fLife );
				tbStateBlow.set( "Value1", fVAR1 );
				tbStateBlow.set( "Value2", fVAR2 );
			}
			break;

		case EMBLOW_BURN:   //	화염.
			{
				//음수표기
				if ( fVAR2 > 0.0f )
					fVAR2 = -fVAR2;

				tbStateBlow.set( "Type", EMBLOW_BURN );
				tbStateBlow.set( "Rate", fRate );
				tbStateBlow.set( "Time", fLife );
				tbStateBlow.set( "Value1", fVAR2 );
			}
			break;

		case EMBLOW_FROZEN: //	냉동.
			{
				tbStateBlow.set( "Type", EMBLOW_FROZEN );
				tbStateBlow.set( "Rate", fRate );
				tbStateBlow.set( "Time", fLife );
				tbStateBlow.set( "Value1", fVAR1 );
				tbStateBlow.set( "Value2", fVAR2 );
			}
			break;

		case EMBLOW_MAD:    //	착란.
			{
				//음수표기
				if ( fVAR1 > 0.0f )
					fVAR1 = -fVAR1;

				//음수표기
				if ( fVAR2 > 0.0f )
					fVAR2 = -fVAR2;

				tbStateBlow.set( "Type", EMBLOW_MAD );
				tbStateBlow.set( "Rate", fRate );
				tbStateBlow.set( "Time", fLife );
				tbStateBlow.set( "Value1", fVAR1 );
				tbStateBlow.set( "Value2", fVAR2 );
			}
			break;

		case EMBLOW_POISON: //	중독.
			{
				if ( fVAR2 > 0.0f )
					fVAR2 = -fVAR2;

				tbStateBlow.set( "Type", EMBLOW_POISON );
				tbStateBlow.set( "Rate", fRate );
				tbStateBlow.set( "Time", fLife );					
				tbStateBlow.set( "Value1", fVAR2 );
			}
			break;

		case EMBLOW_CURSE:  //	저주.
			{
				tbStateBlow.set( "Type", EMBLOW_CURSE );
				tbStateBlow.set( "Rate", fRate );
				tbStateBlow.set( "Time", fLife );
				tbStateBlow.set( "Value1", fVAR1 );
			}
			break;
		}
	}

	tb.set( "StateBlow", tbStateBlow );
}

void GLItemTooltip::GenExplain( LuaTable& tb )
{
	LuaTable tbExplain( GLWidgetScript::GetInstance().GetLuaState() );	

	if ( (m_pItemData->BasicType() != ITEM_SKILL) || (m_pItemData->BasicType() != ITEM_PET_SKILL) )
	{
		const char* chComment = m_pItemData->GetComment();
		if ( chComment )
			tbExplain.set( "Comment", chComment ); 
	}
	else
	{
		if (!m_pItemData)
			return;
		SNATIVEID sSkillID = m_pItemData->sSkillBookOp.sSkill_ID;

		//	Note : 스킬 정보 가져옴.
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
		if ( pSkill )
		{
			const char* chDesc = pSkill->GetDesc();
			if ( chDesc )
				tbExplain.set( "Comment", chDesc );
		}
	}

	tb.set( "Explain", tbExplain );
}


void GLItemTooltip::GenExplainEx( LuaTable& tb )
{
	LuaTable tbExplainEx( GLWidgetScript::GetInstance().GetLuaState() );	

	if ( (m_pItemData->BasicType() != ITEM_SKILL) || (m_pItemData->BasicType() != ITEM_PET_SKILL) )
	{
		char szBeforeTemp[30] = {0, };
		char szAfterTemp[30] = {0, };
		char szTemp[30] = {0, };
		WORD wCurLev = m_pCharacter->GETLEVEL();
		LONGLONG llCurExp = m_pCharacter->GETEXP();
		LONGLONG llCurMaxExp = m_pCharacter->GETMAXEXP();
		if ( m_pOtherCharData )
		{
			wCurLev = m_pOtherCharData->GETLEVEL();
			llCurExp = m_pOtherCharData->GETEXP();
			llCurMaxExp = m_pOtherCharData->GETMAXEXP();
		}

		WORD wExpetedLev = 0;
		LONGLONG llRemainExp = 0;
		LONGLONG llRemainMaxExp = 0;
		LONGLONG llCompressorExp = 0;
		float fPercent = 0;
		float fAtferPercent = 0;
		std::string strTemp;
		std::string strExp;

		if( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_EXP_CAPSULE_TYPE1 || m_pItemData->sDrugOp.emDrug == ITEM_DRUG_EXP_CAPSULE_TYPE2 )
		{
			strTemp = sc::string::format( "%lu", (LONGLONG)m_sItemCustom.wDurability * 100000000 );

			sc::string::AddCommaToNum( strTemp.c_str(), szTemp );

			BOOL bCalExp = FALSE;

			EMCHARCLASS emClass = m_pCharacter->m_emClass;
			if ( m_pOtherCharData )
				emClass = m_pOtherCharData->m_emClass;

			if( emClass == GLCC_EXTREME_M || emClass == GLCC_EXTREME_W )
				bCalExp = GLOGICEX::GLCALCULATEEXP2( wCurLev, llCurExp, (LONGLONG)m_sItemCustom.wDurability * 100000000, wExpetedLev, llRemainExp, llRemainMaxExp);
			else
				bCalExp = GLOGICEX::GLCALCULATEEXP( wCurLev, llCurExp, (LONGLONG)m_sItemCustom.wDurability * 100000000, wExpetedLev, llRemainExp, llRemainMaxExp);

			// 랩업을 하는 경우
			if(  bCalExp == TRUE ) 
			{
				fPercent = float(llRemainExp) / float(llRemainMaxExp);
			}
			// 랩업을 못하는 경우
			else
			{
				wExpetedLev = wCurLev;
				fPercent = float(llCurExp + llRemainExp) / float(llCurMaxExp);
			}

			// 툴팁 표시에 최대 레벨을 넘어서지 않도록
			if( wExpetedLev > GLCONST_CHAR::wMAX_LEVEL )
			{
				wExpetedLev = GLCONST_CHAR::wMAX_LEVEL;
				fPercent = 1;
			}
			else if(wExpetedLev == GLCONST_CHAR::wMAX_LEVEL)
			{
				if(fPercent > 1) 
					fPercent = 1;
			}

			std::string strExp = sc::string::format( "Exp : %s ( %dLv, %.2f%% )", szTemp, wExpetedLev, fPercent * 100.0f);
			tbExplainEx.set( "CommentEx", (const char *)strExp.c_str() ); 
            
		}
		else if( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_EXP_COMPERSSOR_TYPE1 || m_pItemData->sDrugOp.emDrug == ITEM_DRUG_EXP_COMPERSSOR_TYPE2 )
		{
			if( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_EXP_COMPERSSOR_TYPE2 )
				llCompressorExp = 100000000000;
			else
				llCompressorExp = 10000000000;

			LONGLONG llMaxTempExp = ( llCurExp > llCurMaxExp ) ? llCurMaxExp : llCurExp;

			strTemp = sc::string::format( "%lu", llMaxTempExp );
			sc::string::AddCommaToNum( strTemp.c_str(), szBeforeTemp );
			fPercent = float(llMaxTempExp) / float(llCurMaxExp);

			if( llCompressorExp <= llCurExp )
			{
				strTemp = sc::string::format( "%lu", llMaxTempExp - llCompressorExp );
				sc::string::AddCommaToNum( strTemp.c_str(), szAfterTemp );
				fAtferPercent = float(llMaxTempExp - llCompressorExp) / float(llCurMaxExp);
			
				strExp = sc::string::format( "Before : %s ( %.2f%% ) \nAfter : %s ( %.2f%% )", szBeforeTemp, fPercent * 100.0f, szAfterTemp, fAtferPercent * 100.0f);
			}
			else
			{
				strExp = sc::string::format( "Before : %s ( %.2f%% ) \nAfter : %s ", szBeforeTemp, fPercent * 100.0f, ID2GAMEWORD("ITEM_EXP_COMPRESSOR", 0) );
			}

			tbExplainEx.set( "CommentEx", (const char *)strExp.c_str() );
		}

	}

	tb.set( "ExplainEx", tbExplainEx );
}

void GLItemTooltip::GenThrowTradeSale( LuaTable& tb )
{
	LuaTable tbCondition( GLWidgetScript::GetInstance().GetLuaState() );	

	tbCondition.set( "Sale", m_sItemCustom.IsSale() );
	tbCondition.set( "Throw", m_sItemCustom.IsThrow() );
	tbCondition.set( "Trade", m_sItemCustom.IsExchange() );

	// 란 포인트 거래
	if( GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
	{				
		int RPTradeCountMax = m_pItemData->sBasicOp.RanPointTradeCount();
		int RPTradeCountNow = m_sItemCustom.wRanPointTradeCount;

		tbCondition.set( "RanTr", 0 < RPTradeCountMax - RPTradeCountNow );

		if( 0 < RPTradeCountMax )
			tbCondition.set( "RanTrCount", sc::string::format( " (%1%/%2%)", RPTradeCountMax-RPTradeCountNow, RPTradeCountMax ).c_str() );
	}

	// 락커 보관 불가능한지 판단
	tbCondition.set( "Locker", m_sItemCustom.IsNotStorage() );

	// 포장 가능
	tbCondition.set( "BoxWrappable", m_sItemCustom.IsBoxWrappable() );

	tb.set( "TrCond", tbCondition );
}

void GLItemTooltip::GenPrice( LuaTable& tb, int nType )
{
	switch( nType )
	{
		// 개인상점
	case EMGFX_SLOT_PRIVATEMARKET :
		{
			GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

			PrivateMarketClient* pPMarketClient = pClient->GetPrivateMarketClient();

			GLPrivateMarket* pPMarket = NULL;
			if( true == pPMarketClient->IsOpener() )
			{
				pPMarket = &m_pCharacter->m_sPMarket;
			}
			else
			{
				std::tr1::shared_ptr< GLCharClient > pChar = pClient->GetChar( pPMarketClient->GetOpenerID() );
				if( NULL == pChar )
					return;

				pPMarket = &pChar->m_sPMarket;
			}

			const SSALEITEM* pSaleItem = pPMarket->GetItem( SNATIVEID( m_wInvenPosX, m_wInvenPosY ) ); 
			if( NULL == pSaleItem )
				return;

			tb.set( "PMarketPrice", pSaleItem->llPRICE );
		}
		break;

	default :
		{
			LuaTable tbPrice( GLWidgetScript::GetInstance().GetLuaState() );
			tbPrice.set( 1, m_pItemData->sBasicOp.dwBuyPrice );
			tbPrice.set( 2, m_pItemData->sBasicOp.dwSellPrice );
			tbPrice.set( 3, m_pCharacter->GetBuyRate() );
			tbPrice.set( 4, m_pCharacter->GetSaleRate() );

			tb.set( "Price", tbPrice );
		}
		break;
	}
}

void GLItemTooltip::GenActPoint( LuaTable& tb )
{
	LuaTable tbActPoint( GLWidgetScript::GetInstance().GetLuaState() );	

	if ( m_pItemData->sBasicOp.wReqActPointDW || m_pItemData->sBasicOp.wReqActPointUP )
	{
		int emType = m_pItemData->sBasicOp.emReqActivityType;
		bool bReqActPoint = true;

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{		
			CString strTemp = CGameTextMan::GetInstance().GetCommentText("ACTIVITY_CLASS", emType).GetString();
			tbActPoint.set( "Type", strTemp.GetBuffer() );
			tbActPoint.set( "ReqPointDW", m_pItemData->sBasicOp.wReqActPointDW );			
		}
		else
		{			
			tbActPoint.set( "Type", COMMENT::ACTIVITY_CLASS[emType].c_str() );
			tbActPoint.set( "ReqPointDW", m_pItemData->sBasicOp.wReqActPointDW );			
		}

		if ( m_pItemData->sBasicOp.wReqActPointUP )
		{
			tbActPoint.set( "ReqActPointUP", m_pItemData->sBasicOp.wReqActPointUP );
			bReqActPoint = (m_pItemData->sBasicOp.wReqActPointUP >= m_pCharacter->m_llActivityPoint[emType] );			
		}
		bReqActPoint = bReqActPoint && (m_pItemData->sBasicOp.wReqActPointDW <= m_pCharacter->m_llActivityPoint[emType] );
		tbActPoint.set( "ComparePoint", bReqActPoint );
	}

	tb.set( "ActPoint", tbActPoint );
}

void GLItemTooltip::GenContribution( LuaTable& tb )
{
	LuaTable tbContribution( GLWidgetScript::GetInstance().GetLuaState() );	

	if ( m_pItemData->sBasicOp.dwReqContribution )
	{
		bool bReqPoint = true;

		std::string strText;
		tbContribution.set( "ReqContribution", m_pItemData->sBasicOp.dwReqContribution);

		int nContributionPoint = m_pCharacter->m_nContributionPoint;
		if ( m_pOtherCharData )
			nContributionPoint = m_pOtherCharData->m_nContributionPoint;

		bReqPoint = (m_pItemData->sBasicOp.dwReqContribution <= (DWORD)nContributionPoint);
		tbContribution.set( "CompareContri", bReqPoint );	
	}	

	tb.set( "Contribution", tbContribution );
}

void GLItemTooltip::GenTime( LuaTable& tb )
{
	LuaTable tbTime( GLWidgetScript::GetInstance().GetLuaState() );	

	std::string strText;

	if ( m_sItemCustom.tDISGUISE != 0 )
	{				
		CTime cTime(m_sItemCustom.tDISGUISE + m_sItemCustom.GetPeriodExDisguise());

		//	서버 타임으로 변환( TimeZone 계산 )
		GfxInterface::GetInstance()->GetGaeaClient()->GetConvertServerTime( cTime );

		if ( cTime.GetYear() != 1970 )
		{		
			std::string strExpireDate;
			strExpireDate = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
				(cTime.GetYear()%2000),  cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );

			INT nPriodExMaxCount(-1);
			if ( ItemPeriodExtension::Manager::GetInstance().IsRegItem(m_sItemCustom.nidDISGUISE, nPriodExMaxCount) )
			{
				if ( nPriodExMaxCount == 0 )
					strExpireDate.append( ID2GAMEINTEXT( "ITEM_PERIODEX_TOOLTIP", 0 ) );
				else
					strExpireDate.append( sc::string::format(ID2GAMEINTEXT( "ITEM_PERIODEX_TOOLTIP", 1 ), nPriodExMaxCount - m_sItemCustom.cCostumePeriodExtensionCount) );
			}
			tbTime.set( "DisguiseTime", strExpireDate.c_str() );
		}
	}

	if ( m_sItemCustom.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
	{
		CTime cTime( m_sItemCustom.tBORNTIME );
		GfxInterface::GetInstance()->GetGaeaClient()->GetConvertServerTime( cTime );

		if ( cTime.GetYear() != 1970 )
		{
			CTimeSpan sLMT(m_pItemData->sDrugOp.tTIME_LMT);
			cTime += sLMT + m_sItemCustom.GetPeriodExBasic();

			std::string strExpireDate;
			strExpireDate = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
				(cTime.GetYear ()%2000), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );	

			INT nPriodExMaxCount(-1);
			if ( ItemPeriodExtension::Manager::GetInstance().IsRegItem(m_sItemCustom.GetNativeID(), nPriodExMaxCount) )
			{
				if ( nPriodExMaxCount == 0 )
					strExpireDate.append( ID2GAMEINTEXT( "ITEM_PERIODEX_TOOLTIP", 0 ) );
				else
					strExpireDate.append( sc::string::format(ID2GAMEINTEXT( "ITEM_PERIODEX_TOOLTIP", 1 ), nPriodExMaxCount - m_sItemCustom.cPeriodExtensionCount) );
			}

			tbTime.set( "EndTime", strExpireDate.c_str() );
		}
	}

	// 코스튬 능력치 시간
	if( m_sItemCustom.costumeUser.tEndTime > 0 )
	{
		CTime cTime(m_sItemCustom.costumeUser.tEndTime);
		GfxInterface::GetInstance()->GetGaeaClient()->GetConvertServerTime( cTime );

		if ( cTime.GetYear() > 1970 )
		{
			std::string strExpireDate;

			strExpireDate = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
				(cTime.GetYear()%2000),  cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );

			tbTime.set( "CSAbilityEndTime", strExpireDate.c_str() );
		}
	}

	// 일시강화 만료일
	if( m_sItemCustom.sTLGrind.tFireTime > 0 )
	{
		CTime cTime(m_sItemCustom.sTLGrind.tFireTime);

		//	서버 타임으로 변환( TimeZone 계산 )
		GfxInterface::GetInstance()->GetGaeaClient()->GetConvertServerTime( cTime );

		if ( cTime.GetYear() > 1970 )
		{
			std::string strExpireDate;

			strExpireDate = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
				(cTime.GetYear()%2000),  cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );

			int iValue = 0;

			if( m_pItemData ) 
			{
				//if ( m_pItemData->sSuitOp.gdDamage == GLPADATA(0,0) )
				if ( m_sItemCustom.GetBasicAttackDamage() == GLPADATA(0,0) )
				{
					iValue = m_sItemCustom.cDEFENSE;
				}
				else
				{
					iValue = m_sItemCustom.cDAMAGE;
				}
			}

			tbTime.set( "TempEnchant", iValue );
			tbTime.set( "TEEndTime", strExpireDate);
		}
	}

	tb.set( "Time", tbTime );
}

void GLItemTooltip::GenWishlist(LuaTable& tb, int nType )
{
	if( nType != EMGFX_SLOT_INVEN_WISHLIST )
		return;

	LuaTable tbWishList( GLWidgetScript::GetInstance().GetLuaState() );	

	const WishList::ItemMap &ItemMap = GfxInterface::GetInstance()->GetGaeaClient()->GetWishList()->GetItemMap();

	WishList::ItemMapCIter IterBegin(ItemMap.begin());
	WishList::ItemMapCIter IterEnd(ItemMap.end());

	bool bItem(false);
	WishList::ItemSPtr spWish;
	for(; IterBegin != IterEnd; ++IterBegin)
	{
		spWish = IterBegin->second;

		if(spWish->m_sItem.Id() == m_pItemData->BasicId().dwID )
		{
			bItem = true;
			break;
		}
	}	

	if(!bItem)
		return;

	BYTE nWishType = spWish->GetType();
	CString strText;
	switch( nWishType )
	{
	case WishList::ETypeNpcStore:
		{
			WishList::SItemNpcStore* pItem = dynamic_cast< WishList::SItemNpcStore* >( spWish.get() );
			if( !pItem )
				return;

			strText.Format(
				"%s (%s / %d, %d)",
				GLogicData::GetInstance().GetCrowName( pItem->m_sCrowID ).c_str(),
				GfxInterface::GetInstance()->GetGaeaClient()->GetMapName( pItem->m_sMapID ),
				pItem->m_nMapX,
				pItem->m_nMapY );

			tbWishList.set( "from", strText.GetBuffer() );
			tbWishList.set( "Money", NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pItem->m_nPrice, 3, "," ));
			tbWishList.set( "Type", WishList::ETypeNpcStore );
		}
		break;

	case WishList::ETypePointStore:
		{
			WishList::SItemPointStore* pItem = dynamic_cast< WishList::SItemPointStore* >( spWish.get() );
			if( !pItem )
				return;				

			tbWishList.set( "Money", NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pItem->m_nPrice, 3, "," ) );
			tbWishList.set( "Type", WishList::ETypePointStore );
		}
		break;

	case WishList::ETypePrivatePerson:
		{
			WishList::SItemPrivatePerson* pItem = dynamic_cast< WishList::SItemPrivatePerson* >( spWish.get() );
			if( !pItem )
				return;

			tbWishList.set( "from", pItem->m_strCharName.c_str() );
			tbWishList.set( "Money", NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pItem->m_nPrice, 3, "," ));
			tbWishList.set( "Type", WishList::ETypePrivatePerson );
		}
		break;

	case WishList::ETypeChatLink:
		{
			WishList::SItemChatLink* pItem = dynamic_cast< WishList::SItemChatLink* >( spWish.get() );
			if( !pItem )
				return;

			tbWishList.set( "from", pItem->m_strCharName.c_str() );
			tbWishList.set( "Type", WishList::ETypeChatLink );
		}	
		break;

	case WishList::ETypeActivate:
		{
			WishList::SItemActivate* pItem = dynamic_cast< WishList::SItemActivate* >( spWish.get() );
			if( !pItem )
				return;

			tbWishList.set( "Money", NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pItem->m_nPrice, 3, "," ));
			tbWishList.set( "Type", WishList::ETypeActivate );
		}
		break;

	case WishList::ETypeTrade:
		{
			WishList::SItemTrade* pItem = dynamic_cast< WishList::SItemTrade* >( spWish.get() );
			if( !pItem )
				return;

			tbWishList.set( "from", pItem->m_strCharName.c_str() );
			tbWishList.set( "Type", WishList::ETypeTrade );
		}
		break;

	case WishList::ETypeReferChar:
		{
			WishList::SItemReferChar* pItem = dynamic_cast< WishList::SItemReferChar* >( spWish.get() );
			if( !pItem )
				return;

			tbWishList.set( "from", pItem->m_strCharName.c_str() );
			tbWishList.set( "Type", WishList::ETypeReferChar );
		}
		break;

	case WishList::ETypeInventory:
		{
			WishList::SItemInventory* pItem = dynamic_cast< WishList::SItemInventory* >( spWish.get() );
			if( !pItem )
				return;

			tbWishList.set( "Type", WishList::ETypeInventory );
		}
		break;

	case WishList::ETypeBank:
		{
			WishList::SItemBank* pItem = dynamic_cast< WishList::SItemBank* >( spWish.get() );
			if( !pItem )
				return;	

			tbWishList.set( "Type", WishList::ETypeBank );				
		}
		break;

	case WishList::ETypeLocker:
		{
			WishList::SItemLocker* pItem = dynamic_cast< WishList::SItemLocker* >( spWish.get() );
			if( !pItem )
				return;

			tbWishList.set( "Type", WishList::ETypeLocker );
		}
		break;
	}

	tb.set( "WishList", tbWishList );
}

void GLItemTooltip::GenSetItem(LuaTable &_table)
{
	if ( !GLUseFeatures::GetInstance().IsUsingSetItemUse() ) return;

	const SSETITEM *sSetItem = GLItemSet::GetInstance().GetInfoSetItem(m_sItemCustom.GetNativeID());

	if(NULL == sSetItem) return;	//셋트아이템 검색안됨 -> 출력 안함

	//현재 슬롯에 가지고 있는 동일한 셋트 아이템 목록 검색
	SSETITEMSEARCHINFO sInfoPutOnSetItem;

	if ( m_pOtherCharData )
	{
		for(UINT i = 0; i < m_pOtherCharData->m_vInfoPutOnSetItem.size(); ++i)
		{
			if( sSetItem->dwSetID == m_pOtherCharData->m_vInfoPutOnSetItem[i].dwSetID )
			{
				sInfoPutOnSetItem = m_pOtherCharData->m_vInfoPutOnSetItem[i];
				break;
			}
		}
	}
	else
	{
		for(UINT i = 0; i < m_pCharacter->m_vInfoPutOnSetItem.size(); ++i)
		{
			if( sSetItem->dwSetID == m_pCharacter->m_vInfoPutOnSetItem[i].dwSetID )
			{
				sInfoPutOnSetItem = m_pCharacter->m_vInfoPutOnSetItem[i];
				break;
			}
		}
	}

	GenSetItemName( _table, sSetItem, sInfoPutOnSetItem );
	GenSetItemParts( _table, sSetItem, sInfoPutOnSetItem );
	GenSetItemBonus( _table, sSetItem, sInfoPutOnSetItem );
}

void GLItemTooltip::GenSetItemName( LuaTable& tb,		
								   const SSETITEM* pSetItem, 
								   const SSETITEMSEARCHINFO& sInfoPutOnSetItem )
{	
	LuaTable tbSetItemName( GLWidgetScript::GetInstance().GetLuaState() );	
	std::string strResult = "";
	strResult += sc::string::format("%s ", pSetItem->strName ); //셋트 명칭
	strResult += sc::string::format( "(%d/%d)", sInfoPutOnSetItem.vItemID.size(), static_cast<int>(pSetItem->byMaxToolTipItemList) ); //셋트 명칭	
	tbSetItemName.set( "SetItemName", strResult.c_str() );
	tb.set( "SetItemName", tbSetItemName );
}

void GLItemTooltip::GenSetItemParts(LuaTable& tb, 
									const SSETITEM* pSetItem, 
									const SSETITEMSEARCHINFO& sInfoPutOnSetItem)
{
	LuaTable tbSetItemParts( GLWidgetScript::GetInstance().GetLuaState() );	
	LuaTable tbSetItemPartsPuton( GLWidgetScript::GetInstance().GetLuaState() );	
	LuaTable tbSetItemPartsNonePuton( GLWidgetScript::GetInstance().GetLuaState() );	

	// 세트 아이템의 필요 세트 부위 정보 가져오기;
	std::vector< STEMP_SUIT_PUTON > vecSuitPutOn;
	for ( UINT i=0; i<pSetItem->byMaxToolTipItemList; ++i )
	{
		STEMP_SUIT_PUTON sTempSuitPutOn;

		SITEM* pItem = GLogicData::GetInstance().GetItem( pSetItem->vecItemList[i] );
		if ( NULL == pItem )
			continue;

		sTempSuitPutOn.emSuit = pItem->sSuitOp.emSuit;

		vecSuitPutOn.push_back( sTempSuitPutOn );
	}

	// 세트아이템에 포함된 아이템 모두 검사;
	int nCountPutOn = 0;
	SNATIVEID sFindRingID( false );
	SNATIVEID sFindAccessoryID( false );
	for ( UINT i=0; i<pSetItem->vecItemList.size(); ++i )
	{
		// 최대 세트아이템 적용 수가 넘으면 더 이상 체크하지 않는다;
		if ( nCountPutOn >= pSetItem->byMaxToolTipItemList )
			break;

		SITEM *pItem = GLogicData::GetInstance().GetItem( pSetItem->vecItemList[i] );
		if ( NULL == pItem )
			continue;

		bool bPutOn = false;
		for ( UINT j=0; j<sInfoPutOnSetItem.vItemID.size(); ++j )
		{			
			SITEM* pItemTarget = GLogicData::GetInstance().GetItem( sInfoPutOnSetItem.vItemID[j] );

			if ( pItemTarget->sBasicOp.GetNativeID() == pItem->sBasicOp.GetNativeID() )
			{
				// 반지 중복 활성화 방지 -> 다른 반지는 중복된다;
				if ( pItemTarget->sSuitOp.emSuit == SUIT_FINGER )
				{
					if( sFindRingID != pItemTarget->BasicId() ) 
						sFindRingID = pItemTarget->BasicId();
					else
						continue;
				}

				// 장신구 중복 활성화 방지 -> 서로 다른 장신구는 중복된다;
				if ( pItemTarget->sSuitOp.emSuit == SUIT_ACCESSORIES )
				{
					if( sFindAccessoryID != pItemTarget->BasicId()) 
						sFindAccessoryID = pItemTarget->BasicId();
					else
						continue;
				}		

				bPutOn = true;
				break;
			}
		}

		for ( int j=0; j<vecSuitPutOn.size(); ++j )
		{
			STEMP_SUIT_PUTON& sSuitPutOn = vecSuitPutOn.at( j );
			if ( pItem->sSuitOp.emSuit == sSuitPutOn.emSuit )
			{
				if ( bPutOn )
				{
					sSuitPutOn.nCountPutOn++;
					++nCountPutOn;
				}
			}
		}
	}

	// 체크된 내용을 LuaTable 에 삽입한다;
	int nIndexPutOn = 1;
	int nIndexNone = 1;
	for ( int i=0; i<vecSuitPutOn.size(); ++i )
	{
		const STEMP_SUIT_PUTON& sSuitPutOn = vecSuitPutOn.at( i );
		if ( 0 < sSuitPutOn.nCountPutOn )
		{
			for ( int j=0; j<sSuitPutOn.nCountPutOn; ++j )
			{
				tbSetItemPartsPuton.set( nIndexPutOn, sSuitPutOn.emSuit );
				++nIndexPutOn;
			}
		}
		else
		{
			tbSetItemPartsNonePuton.set( nIndexNone, sSuitPutOn.emSuit );
			++nIndexNone;
		}
	}

	tbSetItemParts.set( 1, tbSetItemPartsPuton );
	tbSetItemParts.set( 2, tbSetItemPartsNonePuton );
	tb.set( "SetItemPart", tbSetItemParts );
}


void GLItemTooltip::GenSetItemBonus(LuaTable& tb, 
									const SSETITEM* pSetItem, 
									const SSETITEMSEARCHINFO& sInfoPutOnSetItem)
{
	LuaTable tbBonus( GLWidgetScript::GetInstance().GetLuaState() );		

	int Count(1);
	std::vector<std::string> strSetBonus;
	strSetBonus.resize(SLOT_TSIZE);

	//출력할 문자열 생성
	for(BYTE i = 0; i < pSetItem->vecBonus.size(); ++i)
	{
		LuaTable tbSetItemBonus( GLWidgetScript::GetInstance().GetLuaState() );	
		//사용 가능한 옵션 확인 
		if( GLItemSet::GetInstance().IsUsableOpt(pSetItem->vecBonus[i].sSpec.emSPEC) )
		{ 
			UINT nTerms(pSetItem->vecBonus[i].byTerms);
			// 보너스명
			tbSetItemBonus.set( "BonusName", static_cast<int>(pSetItem->vecBonus[i].sSpec.emSPEC) );
			// 수치
			tbSetItemBonus.set( "BonusNum", pSetItem->vecBonus[ i ].sSpec.sSPEC.fVAR1 );
			// 퍼센티지 표기?
			tbSetItemBonus.set( "IsPercentage", GLItemSet::GetInstance().IsPerentOpt( pSetItem->vecBonus[ i ].sSpec.emSPEC ) );
			// 세트 효과 발동 조건
			tbSetItemBonus.set( "Piece", nTerms );

			tbBonus.set( Count, tbSetItemBonus );
			++Count;
		}			
	}

	tb.set( "SetItemBonus", tbBonus );
}

void GLItemTooltip::GenRandomOpt( LuaTable& tb )
{
	if ( NULL == m_pItemData )
		return;

	LuaTable tbType( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbValue( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbIsFloat( GLWidgetScript::GetInstance().GetLuaState() );

	bool bRandomOption = false;
	WORD wMinCount = 0;
	WORD wMaxCount = 0;
	bool bDontCreate = false;

	if ( GUID_NULL == m_sItemCustom.guid )
		bDontCreate = true;
	else
		bDontCreate = false;

	const RandomOption::Manager& refRandomOptionManager = RandomOption::Manager::getInstance();
	const RandomOption::Entity* const pEntity = refRandomOptionManager.find( m_pItemData->sRandomOpt.szNAME );
	if ( NULL != pEntity )
	{
		bRandomOption = true;
		wMinCount = pEntity->GetMinGain();
		wMaxCount = pEntity->GetMaxGain();
	}

	for( int i = 0; i < RandomOpt::NSIZE; ++i )
	{
		const INT nTYPE(m_sItemCustom.randomOption.getType( i ));
		tbType.set( i + 1, nTYPE );
		//tbValue.set( i + 1, m_sItemCustom.randomOption.getValue( i ) );
		tbValue.set( i + 1, m_sItemCustom.getOptValue(i) * COMMENT::ITEM_RANDOM_OPT_SCALE[nTYPE] );
		float fScale(COMMENT::ITEM_RANDOM_OPT_SCALE[nTYPE])
			, fValue(m_sItemCustom.getOptValue(i))
			, fResult(fValue*fScale);
		//tbValue.set( i + 1, fResult );
		tbIsFloat.set( i + 1, (bool)m_sItemCustom.IsPerRandOpt(nTYPE) );
	}

	LuaTable tbRandomOpt( GLWidgetScript::GetInstance().GetLuaState() );
	tbRandomOpt.set( 1, tbType );
	tbRandomOpt.set( 2, tbValue );
	tbRandomOpt.set( 3, tbIsFloat );
	tbRandomOpt.set( 4, bRandomOption );
	tbRandomOpt.set( 5, wMinCount );
	tbRandomOpt.set( 6, wMaxCount );
	tbRandomOpt.set( 7, bDontCreate );

	tb.set( "tbRandOpt", tbRandomOpt );
}

void GLItemTooltip::GenPreview( LuaTable& tb )
{
	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;

	bool bPreview = m_pItemData->IsPreview();
	if( true == bPreview )
	{
		bPreview = pClient->GetItemPreviewClient()->IsEnableItemPreview( m_pItemData );;
	}
	else if( ITEM_BOX == m_pItemData->BasicType() )
	{
		for( size_t i = 0; i < m_pItemData->sBox.vecItems.size(); ++i )
		{
			SITEM* pBoxItem = GLogicData::GetInstance().GetItem( m_pItemData->sBox.vecItems[ i ].nidITEM );
			if( NULL == pBoxItem )
				continue;

			if( true == pBoxItem->IsPreview() )
			{
				bPreview = pClient->GetItemPreviewClient()->IsEnableItemPreview( pBoxItem );
				break;
			}
		}
	}

	// 미리보기 가능?
	tb.set( "IsSuitable", bPreview );
}

void GLItemTooltip::GenTradeItem( LuaTable& tb, int nType )
{
	if( EMGFX_SLOT_NPCMARKET != nType )
		return;

	int nMarketIndex = GLWidgetScript::GetInstance().LuaCallFunc< int >(
		NSWIDGET_SCRIPT::g_strFunc_NpcTalk_GetActiveMarket );
	if( -1 == nMarketIndex )
		return;

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( GLNpcTalkClient::GetInstance().GetNpcID() );
	if( NULL == pCrowData )
		return;

	GLInventorySale* pInvenSale = dynamic_cast< GLInventorySale* >( pCrowData->GetSaleInven( nMarketIndex ) );
	if( NULL == pInvenSale )
		return;

	std::vector< sExNeed > vecNeed;
	EEXTYPE emType = pInvenSale->GetExchangeList( m_sItemCustom.GetNativeID(), m_wInvenPosX, m_wInvenPosY, vecNeed );

	std::vector< sExNeed >::iterator iter = vecNeed.begin();

	LuaTable tbTradeItem( GLWidgetScript::GetInstance().GetLuaState() );
	switch( emType )
	{
	case EX_ITEM_TYPE :		// 아이템 교환
		{
			for( int i = 1; iter != vecNeed.end(); ++iter, ++i )
			{
				LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
				tbItem.set( 1, iter->sNativeID.Mid() );
				tbItem.set( 2, iter->sNativeID.Sid() );
				tbItem.set( 3, iter->iNeedCount );
				tbItem.set( 4, (int) emType );

				tbTradeItem.set( i, tbItem );
			}
		}
		break;

	case EX_POINT_TYPE :	// 포인트 교환
	case EX_LVP_TYPE :		// 생활점수 교환
		{
			for( int i = 1; iter != vecNeed.end(); ++iter, ++i )
			{
				LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
				tbItem.set( 1, -1 );
				tbItem.set( 2, -1 );
				tbItem.set( 3, iter->iNeedCount );
				tbItem.set( 4, (int) emType );

				tbTradeItem.set( i, tbItem );
			}
		}
		break;
	}

	tb.set( "TradeCondition", tbTradeItem );
}

void GLItemTooltip::ResetDurabilityValue( float fResetValue /* = 1.0f */ )
{
	for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i )
	{
		m_fDurabilityValue[i] = fResetValue;
	}
}

void GLItemTooltip::Durability_Header( LuaTable& tb, float fDurability )
{
	LuaTable tbHead( GLWidgetScript::GetInstance().GetLuaState() );	

	{
		LuaTable tbGrade( GLWidgetScript::GetInstance().GetLuaState() );

		tbGrade.set( "Grade" , ITEM_INFOR_COLOR::dwItemRank[ m_pItemData->sBasicOp.emLevel ] );
		tbHead.set("Grade", tbGrade);		
	}

	tb.set("Head", tbHead);
}

/* 주석 참조 */

void GLItemTooltip::Durability_MakeBasic( LuaTable& tb, float fDurability )
{
	LuaTable tbBasic( GLWidgetScript::GetInstance().GetLuaState() );	
	
	{ /*== 공격력 ================================================================================*/
		LuaTable tbDamage( GLWidgetScript::GetInstance().GetLuaState() );

		EMCHARINDEX emCharIndex = m_pCharacter->GetCharIndex();
		WORD wLevel = m_pCharacter->GETLEVEL();
		if ( m_pOtherCharData )
		{
			emCharIndex = m_pOtherCharData->GETCHARINDEX();
			wLevel = m_pOtherCharData->GETLEVEL();
		}

		const GLPADATA& sDamage = m_sItemCustom.getdamage( 0.0f, emCharIndex, wLevel );

		//int nMinDmg = static_cast<int>( m_pItemData->sSuitOp.gdDamage.wLow ); 
		//int nMaxDmg = static_cast<int>( m_pItemData->sSuitOp.gdDamage.wHigh );
		//int nRndOptDamage = static_cast<int>( sDamage.wLow - m_pItemData->sSuitOp.gdDamage.wLow );
		int nMinDmg = static_cast<int>( m_sItemCustom.GetBasicAttackDamage().GetLow() ); 
		int nMaxDmg = static_cast<int>( m_sItemCustom.GetBasicAttackDamage().GetHigh() );
		int nRndOptDamage = static_cast<int>( sDamage.wLow - m_sItemCustom.GetBasicAttackDamage().GetLow() );
		int nExtraValue = static_cast<int>( m_sItemCustom.GETGRADE_DAMAGE() );
		BYTE uGRADE = m_sItemCustom.GETGRADE(EMGRINDING_DAMAGE);

		// 이거 의도가 뭐지?
		// fDurability != 1.0f ?
		if( fDurability < 1.0f || fDurability > 1.0f)
		{
			nRndOptDamage = static_cast<int>( nRndOptDamage * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION] );
 			nExtraValue = static_cast<int>( nExtraValue * m_fDurabilityValue[DURABILITY_APPLY_BASIC] );
			GLDWDATA& damage = m_sItemCustom.GETDAMAGE(); // 총 데미지 값
		

			// 기존 루틴을 이용하는 방향으로 하기위해서 이렇게 수정했습니다.
			// 의도가 이해가지 않는 코드들이 많아서 기존루틴을 최대한 안건드리고 수정하는 방향으로.

			// 정확한 공식이나 관련해서 정리된 자료가 나오지 않는 이상은 기존 소스크드에서 벗어나지 않는 방향으로.


			nMinDmg = static_cast<int>(damage.nNow * m_fDurabilityValue[DURABILITY_APPLY_BASIC])
				- ( nRndOptDamage * m_fDurabilityValue[DURABILITY_APPLY_BASIC] )  - nExtraValue;

			// 랜덤 옵션값은 증가되면 안됩니다. 그러므로 fDuabilityVallue * 할경우 1.0으로되어있습니다.
			// 흰색 데미지에서는 1.3배 되기때문에 랜덤값 * 1.3배한걸 빼야합니다. ( 총 데미지 구할때 데미지 + 랜덤 * 1.3 이라서 )
		
			// 기존 코드가 왜 이렇게 짜여졌는가는 애시당초 내구도 시스템에서 부스트 시스템으로 기획사항이 변경되었을때
			// 기존 공식에다 그냥 1.3을 곱한걸로 보입니다. 초기기획은 내구도가 떨어지면 데미지가 떨어지는 시스템
			// Durability가 내구도라는 뜻인데 부스트로 쓰이는 이유.
			
			// 아래 값들을 구할때도 다 비슷하게 수정했습니다.

			// 일단 Q/A에서 UI상 값만 이상하게나오고 실제 적용값은 문제가 없다고하니 기존시스템에서 최대한 건들이지 않고
			// 값만 수정했습니다.


			nMaxDmg = static_cast<int>(damage.nMax * m_fDurabilityValue[DURABILITY_APPLY_BASIC]) 
				- ( nRndOptDamage  * m_fDurabilityValue[DURABILITY_APPLY_BASIC]  )  - nExtraValue;

			nExtraValue = static_cast<int>( m_sItemCustom.GETGRADE_DAMAGE(m_fDurabilityValue[DURABILITY_APPLY_BASIC]) );
		}

			

		tbDamage.set("Min", nMinDmg);
		tbDamage.set("Max", nMaxDmg);
		tbDamage.set("RndOpt", nRndOptDamage);
		tbDamage.set("Extra", nExtraValue);

		if ( GUID_NULL == m_sItemCustom.guid )
			tbDamage.set("bDontCreate", true);
		else
			tbDamage.set("bDontCreate", false);

		if ( -1 != m_pItemData->GetBasicAttackDamageIndex() )
			tbDamage.set("RandomStat", true);
		else
			tbDamage.set("RandomStat", false);

		tbDamage.set("bRebuild", m_pItemData->IsEditableBasicAttackDamage());

		tbBasic.set("Damage", tbDamage );
	}
		
	{ /*== 방어력 ================================================================================*/
		LuaTable tbDefense( GLWidgetScript::GetInstance().GetLuaState() );

		EMCHARINDEX emCharIndex = m_pCharacter->GetCharIndex();
		WORD wLevel = m_pCharacter->GETLEVEL();
		if ( m_pOtherCharData )
		{
			emCharIndex = m_pOtherCharData->GETCHARINDEX();
			wLevel = m_pOtherCharData->GETLEVEL();
		}

		//int nBasicDef = static_cast<int>( m_pItemData->sSuitOp.nDefense);
		//int nRndOptDef = static_cast<int>( 
		//	m_sItemCustom.getdefense( m_pCharacter->GETCHARINDEX(), m_pCharacter->GETLEVEL() ) - m_pItemData->sSuitOp.nDefense);
		int nBasicDef = static_cast<int>( m_sItemCustom.GetBasicDefence() );
		int nRndOptDef = static_cast<int>( 
			m_sItemCustom.getdefense( emCharIndex, wLevel ) - m_sItemCustom.GetBasicDefence() );
		int nExtraValue = m_sItemCustom.GETGRADE_DEFENSE();
		BYTE uGRADE = m_sItemCustom.GETGRADE(EMGRINDING_DEFENSE);
				
		if( fDurability < 1.0f || fDurability > 1.0f )
		{
			nRndOptDef = static_cast<int>( nRndOptDef * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION] );
			nExtraValue = static_cast<int>( nExtraValue * m_fDurabilityValue[DURABILITY_APPLY_BASIC] );
			nBasicDef = static_cast<int>(m_sItemCustom.GETDEFENSE() * m_fDurabilityValue[DURABILITY_APPLY_BASIC])
				- ( nRndOptDef  * m_fDurabilityValue[DURABILITY_APPLY_BASIC] ) - nExtraValue;
		}		

		tbDefense.set("Def"   , nBasicDef);
		tbDefense.set("RndOpt", nRndOptDef);
		tbDefense.set("Extra" , nExtraValue);

		if ( GUID_NULL == m_sItemCustom.guid )
			tbDefense.set("bDontCreate", true);
		else
			tbDefense.set("bDontCreate", false);

		if ( -1 != m_pItemData->GetBasicDefenceIndex() )
			tbDefense.set("RandomStat", true);
		else
			tbDefense.set("RandomStat", false);

		tbDefense.set("bRebuild", m_pItemData->IsEditableBasicDefence());

		tbBasic.set("Defence", tbDefense);
	}

	
	{	// 명중률, 회피율, 소모 SP		
		//	명중률
		LuaTable tbHitRate( GLWidgetScript::GetInstance().GetLuaState() );
		LuaTable tbAvoidRate( GLWidgetScript::GetInstance().GetLuaState() );
		LuaTable tbSpRate( GLWidgetScript::GetInstance().GetLuaState() );

		int nBasicHitRate  = static_cast<int>( m_pItemData->sSuitOp.nHitRate );
		int nRndOptHitRate = static_cast<int>( m_sItemCustom.GETHITRATE() - m_pItemData->sSuitOp.nHitRate );
				
		//내구도 적용 시
		if( fDurability < 1.0f || fDurability > 1.0f )
		{
			nRndOptHitRate = static_cast<int>( nRndOptHitRate * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION] );

			// 실제 적용 수치와 튤팁 출력간의 오차 보정
			nBasicHitRate = static_cast<int>(m_sItemCustom.GETHITRATE() * m_fDurabilityValue[DURABILITY_APPLY_BASIC])
				- ( nRndOptHitRate * m_fDurabilityValue[DURABILITY_APPLY_BASIC] );
		}
		
		//	회피율
		int nBasicAvoidRate = static_cast<int>( m_pItemData->sSuitOp.nAvoidRate );
		int nRndOptAvoidRate = static_cast<int>( m_sItemCustom.GETAVOIDRATE() - m_pItemData->sSuitOp.nAvoidRate );
				
		//내구도 적용 시
		if( fDurability < 1.0f || fDurability > 1.0f  )
		{
			nRndOptAvoidRate = static_cast<int>( nRndOptAvoidRate * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION] ); 

			nBasicAvoidRate = static_cast<int>(m_sItemCustom.GETAVOIDRATE() * m_fDurabilityValue[DURABILITY_APPLY_BASIC]) 
				- ( nRndOptAvoidRate * m_fDurabilityValue[DURABILITY_APPLY_BASIC] );	
		}
				
		//	소모 SP
		int nBasicReqSP = static_cast<int>( m_pItemData->sSuitOp.wReqSP );
		int nRndOptReqSP = static_cast<int>( m_sItemCustom.GETREQ_SP() - m_pItemData->sSuitOp.wReqSP);
		
		//내구도 적용 시
		if( fDurability < 1.0f || fDurability > 1.0f )
		{
			nRndOptReqSP = static_cast<int>( nRndOptReqSP * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION] );

			nBasicReqSP = static_cast<int>(m_sItemCustom.GETREQ_SP() * m_fDurabilityValue[DURABILITY_APPLY_BASIC])
				- nRndOptReqSP  * m_fDurabilityValue[DURABILITY_APPLY_BASIC];
		}
		
		tbHitRate.set( "Hit"   , nBasicHitRate );
		tbHitRate.set( "RndOpt", nRndOptHitRate );
		
		tbAvoidRate.set( "Avoid" , nBasicAvoidRate );
		tbAvoidRate.set( "RndOpt", nRndOptAvoidRate );

		tbSpRate.set( "Sp"    , nBasicReqSP );
		tbSpRate.set( "RndOpt", nRndOptReqSP );

		tbBasic.set("HitRate", tbHitRate );
		tbBasic.set("AvoidRate", tbAvoidRate );
		tbBasic.set("CostSp", tbSpRate );
	}

	tb.set("BASIC", tbBasic);
}

void GLItemTooltip::Durability_MakeRandom( LuaTable& tb, float fDurability )
{
	LuaTable tbType( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbValue( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbIsFloat( GLWidgetScript::GetInstance().GetLuaState() );

	for( int i = 0; i < RandomOpt::NSIZE; ++i )
	{
		const INT nTYPE(m_sItemCustom.randomOption.getType( i ));
		tbType.set( i + 1, nTYPE );
		tbValue.set( i + 1, m_sItemCustom.randomOption.getValue( i )*COMMENT::ITEM_RANDOM_OPT_SCALE[i]*m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION] );
		tbIsFloat.set( i + 1, (bool)m_sItemCustom.IsPerRandOpt(nTYPE) );
	}

	LuaTable tbRandomOpt( GLWidgetScript::GetInstance().GetLuaState() );
	tbRandomOpt.set( 1, tbType );
	tbRandomOpt.set( 2, tbValue );
	tbRandomOpt.set( 3, tbIsFloat );

	tb.set( "RandomOpt", tbRandomOpt );

/*
	LuaTable tbType( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbValue( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbPer( GLWidgetScript::GetInstance().GetLuaState() );

	float fVal = 0.f;
	bool  bPer = false;

	for( int i = 0; i < RandomOpt::NSIZE; ++i )
	{		
		int nType = m_sItemCustom.randomOption.getType( i );

		if( nType > RandomOption::RANDOM_OPTION_NULL && nType < RandomOption::RANDOM_OPTION_NSIZE )
		{
			nType = 0; fVal = 0.f; bPer = false;
		}
		else
		{
			fVal = m_sItemCustom.randomOption.getValue( i )*COMMENT::ITEM_RANDOM_OPT_SCALE[i]*m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION];
			bPer = (m_sItemCustom.IsPerRandOpt(nType) == TRUE) ? true : false;
		}		

		tbType.set( i + 1, nType );
		tbValue.set( i + 1, fVal );
		tbPer.set( i + 1,  bPer);
	}

	LuaTable tbRandomOpt( GLWidgetScript::GetInstance().GetLuaState() );
	tbRandomOpt.set( 1, tbType );
	tbRandomOpt.set( 2, tbValue );

	tb.set( "RandomOpt", tbRandomOpt );
	*/
}

void GLItemTooltip::Durability_MakeAdd( LuaTable& tb, float fDurability )
{
	{
		LuaTable tbAddOpt( GLWidgetScript::GetInstance().GetLuaState() );	
		const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;
		ITEM::SADDON arrVALUE[EMADD_SIZE];
		SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );
		for ( int i = 0; i < SITEMCUSTOM_ADDON::ADDON_SIZE; ++i )
		{
			bool bGen(true);
			if ( GUID_NULL == m_sItemCustom.guid )
				bGen = false;

			EMITEM_ADDON emTYPE = bGen ? m_sItemCustom.sAddonInfo.m_sADDON[i].emTYPE : m_pItemData->sSuitOp.sADDON[i].emTYPE;
			arrVALUE[emTYPE].emTYPE = emTYPE;

			if ( emTYPE != EMADD_NONE )
			{
				// 내구도 적용;
				// 이전 Addon은 m_fDurabilityValue[DURABILITY_APPLY_ADD]로 설정하고;
				// 이전 SVOL, SVAL은 m_fDurability로 설정한다;
				// 왠지 이유는 모르겠다;
				if ( bGen )
				{
					if ( ITEM::bAddonValueInteger[emTYPE] )
						arrVALUE[ emTYPE ].nVALUE = static_cast<int>( m_sItemCustom.sAddonInfo.m_sADDON[i].nVALUE * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
					else
						arrVALUE[ emTYPE ].fVALUE = static_cast<float>( m_sItemCustom.sAddonInfo.m_sADDON[i].fVALUE * m_fDurability );
				}
				else
				{
					if ( ITEM::bAddonValueInteger[emTYPE] )
						arrVALUE[ emTYPE ].nVALUE = static_cast<int>( m_pItemData->sSuitOp.sADDON[i].nVALUE * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
					else
						arrVALUE[ emTYPE ].fVALUE = static_cast<float>( m_pItemData->sSuitOp.sADDON[i].fVALUE * m_fDurability );
				}
			}
		}

		arrVALUE[EMADD_HP].nVALUE += static_cast<int>(m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_HP) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION]);
		arrVALUE[EMADD_MP].nVALUE += static_cast<int>(m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MP) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION]);
		arrVALUE[EMADD_SP].nVALUE += static_cast<int>(m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_SP) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION]);

		float fRATE  = m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_DAMAGE) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION];
		float fVALUE = m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DAMAGE) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION];

		fVALUE += m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MAGIC) * m_fDurabilityValue[DURABILITY_APPLY_RANDOM_OPTION];

		if ( fRATE!=0 || fVALUE!=0)
		{
			arrVALUE[EMADD_MA].nVALUE = INT(arrVALUE[EMADD_MA].nVALUE*(100.0f+fRATE)*0.01f) + INT(fVALUE);
		}

		// 아래 특수를 여기다 넣는다;
		arrVALUE[EMADDEX_INCR_HP].fVALUE = ( m_sItemCustom.GETINCHP() );
		arrVALUE[EMADDEX_INCR_MP].fVALUE = ( m_sItemCustom.GETINCMP() );
		arrVALUE[EMADDEX_INCR_SP].fVALUE = ( m_sItemCustom.GETINCSP() );
		arrVALUE[EMADDEX_INCR_AP].fVALUE = ( m_sItemCustom.GETINCAP() ); 
		
		INT nTableCount(0);
		for( int i=1; i<EMADD_SIZE; ++i)
		{
			if ( ITEM::bAddonValueInteger[i] )
			{
				if( arrVALUE[i].nVALUE != 0 || arrVALUE[i].emTYPE == EMADD_RANDOM )
				{
					LuaTable tbOpt( GLWidgetScript::GetInstance().GetLuaState() );	
					tbOpt.set( 1, true );	// int인가?;
					tbOpt.set( 2, arrVALUE[i].nVALUE );
					tbOpt.set( 3, i );
					tbAddOpt.set( ++nTableCount , tbOpt );
				}
			}
			else
			{
				if( arrVALUE[i].fVALUE != 0.0f || arrVALUE[i].emTYPE == EMADD_RANDOM )
				{
					LuaTable tbOpt( GLWidgetScript::GetInstance().GetLuaState() );	
					const bool bRate(COMMENT::ITEMADDON_VAR_SCALE[i] != -1);

					float fValue(arrVALUE[i].fVALUE);
					if ( bRate
						&& m_pItemData->BasicType() != ITEM_PET_FOOD
						&& m_pItemData->BasicType() != ITEM_VEHICLE_OIL )
						fValue *= COMMENT::ITEMADDON_VAR_SCALE[i];

					if( (EMITEM_ADDON)i == EMVAR_MOVE_SPEED )
						fValue = m_sItemCustom.GETMOVESPEED() * m_fDurability;

					CString strTemp = sc::string::format( "%.4f", fValue ).c_str();

					CutZero( strTemp, 2 );
					tbOpt.set( 1, false );	// int인가?;
					tbOpt.set( 2, strTemp.GetBuffer() );
					tbOpt.set( 3, i );
					tbOpt.set( 4, bRate );

					tbAddOpt.set( ++nTableCount , tbOpt );
				}
			}
		}

		tb.set( "DuraAdd_AddOpt", tbAddOpt );
	}

	/* 경험치 배율 **************************************************************************************/
	{
		float fExpMultiple = m_pItemData->GetExpMultiple(); //내구도 적용
		LuaTable tbExp( GLWidgetScript::GetInstance().GetLuaState() );	

		if( (fExpMultiple > 1.0f) || (fDurability < 1.0f || fDurability > 1.0f) )
		{
			fExpMultiple = ( (fExpMultiple - 1.0f) * m_fDurability ) + 1.0f;
		}

		if ( fExpMultiple != 1.0f )
		{
			CString strTemp = sc::string::format( "%.2f", fExpMultiple * 100.0f - 100.0f ).c_str();
			CutZero( strTemp, 2 );
			tbExp.set( "Exp", strTemp.GetBuffer() );
		}

		tb.set( "Dura_ExpMultiple", tbExp );
	}

	/* 저항 **************************************************************************************/
	{

		LuaTable tbResist( GLWidgetScript::GetInstance().GetLuaState() );	
		LuaTable tbResistAdd( GLWidgetScript::GetInstance().GetLuaState() );	

		const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;

		int nELEC   = sSUIT.sResist.nElectric;
		int nFIRE   = sSUIT.sResist.nFire;
		int nICE    = sSUIT.sResist.nIce;
		int nPOISON = sSUIT.sResist.nPoison;
		int nSPIRIT = sSUIT.sResist.nSpirit;


		// 요청에 맞게 공식 수정

		int nAddELEC( static_cast<int>(m_sItemCustom.GETRESIST_ELEC(m_fDurabilityValue[DURABILITY_APPLY_ADD]) - nELEC) ); 
		int nAddFIRE( static_cast<int>(m_sItemCustom.GETRESIST_FIRE(m_fDurabilityValue[DURABILITY_APPLY_ADD]) - nFIRE) ); 
		int nAddICE( static_cast<int>(m_sItemCustom.GETRESIST_ICE(m_fDurabilityValue[DURABILITY_APPLY_ADD]) - nICE) ); 
		int nAddPOISON( static_cast<int>(m_sItemCustom.GETRESIST_POISON(m_fDurabilityValue[DURABILITY_APPLY_ADD]) - nPOISON) ); 
		int nAddSPIRIT( static_cast<int>(m_sItemCustom.GETRESIST_SPIRIT(m_fDurabilityValue[DURABILITY_APPLY_ADD]) - nSPIRIT) );
		
		{		
			if( fDurability < 1.0f || fDurability > 1.0f )
			{
				nELEC   = static_cast<int>( nELEC   * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nFIRE   = static_cast<int>( nFIRE   * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nICE    = static_cast<int>( nICE    * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nPOISON = static_cast<int>( nPOISON * m_fDurabilityValue[DURABILITY_APPLY_ADD] );
				nSPIRIT = static_cast<int>( nSPIRIT * m_fDurabilityValue[DURABILITY_APPLY_ADD] );

				nAddELEC   = static_cast<int>( nAddELEC );
				nAddFIRE   = static_cast<int>( nAddFIRE );
				nAddICE    = static_cast<int>( nAddICE  );
				nAddPOISON = static_cast<int>( nAddPOISON );
				nAddSPIRIT = static_cast<int>( nAddSPIRIT );
			}
		}

		if ( nELEC || nFIRE || nICE || nPOISON || nSPIRIT ||
			nAddELEC || nAddFIRE || nAddICE || nAddPOISON || nAddSPIRIT )
		{
			tbResist.set( "Spirit", nSPIRIT );
			tbResist.set( "Fire", nFIRE );
			tbResist.set( "Ice", nICE );
			tbResist.set( "Elec", nELEC );
			tbResist.set( "Poison", nPOISON );

			tbResistAdd.set( "Spirit", nAddSPIRIT );
			tbResistAdd.set( "Fire", nAddFIRE );
			tbResistAdd.set( "Ice", nAddICE );
			tbResistAdd.set( "Elec", nAddELEC );
			tbResistAdd.set( "Poison", nAddPOISON );
		}

		tb.set( "Resist", tbResist );
		tb.set( "ResistAdd", tbResistAdd );
	}

}

void GLItemTooltip::Gen_CostumeState( LuaTable& tb, bool bSimple )
{
	LuaTable tbDamage( GLWidgetScript::GetInstance().GetLuaState() );

	bool bGenItemToCs = false;

	GLCharacter* const pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	EMCHARINDEX charIdx = m_pCharacter->GETCHARINDEX();
	int nLevel = m_pCharacter->GETLEVEL();
	if ( m_pOtherCharData )
	{
		charIdx = m_pOtherCharData->GETCHARINDEX();
		nLevel = m_pOtherCharData->GETLEVEL();
	}

	GLPADATA& sDamage = m_sItemCustom.getdamage( 0.0f, charIdx, nLevel );

	// 공격 /////////////////////////////////////////////////////////////////////////////////
	if ( m_sItemCustom.nidDISGUISE != NATIVEID_NULL() )
	{
		SITEMCUSTOM sCustomeItem(m_sItemCustom.nidDISGUISE);

		if( sCustomeItem.IsDisguise() ) {
			bGenItemToCs = true;
		}
	}

	//int nMinDmg = static_cast<int>( m_pItemData->sSuitOp.gdDamage.wLow ); 
	//int nMaxDmg = static_cast<int>( m_pItemData->sSuitOp.gdDamage.wHigh );
	//int nRndOptDamage = static_cast<int>( sDamage.wLow - m_pItemData->sSuitOp.gdDamage.wLow );
	int nMinDmg = static_cast<int>( m_sItemCustom.GetBasicAttackDamage().GetLow() ); 
	int nMaxDmg = static_cast<int>( m_sItemCustom.GetBasicAttackDamage().GetHigh() );
	int nRndOptDamage = static_cast<int>( sDamage.wLow - m_sItemCustom.GetBasicAttackDamage().GetLow() );
	int nExtraValue = static_cast<int>( m_sItemCustom.GETGRADE_DAMAGE() );
	BYTE uGRADE = m_sItemCustom.GETGRADE(EMGRINDING_DAMAGE);

	if(m_fDurability < 1.0f)
	{
		nRndOptDamage = static_cast<int>( nRndOptDamage * m_fDurability );
		nExtraValue = static_cast<int>( nExtraValue * m_fDurability );
		GLDWDATA& damage = m_sItemCustom.GETDAMAGE();
		nMinDmg = static_cast<int>(damage.nNow * m_fDurability) - nRndOptDamage - nExtraValue;
		nMaxDmg = static_cast<int>(damage.nMax * m_fDurability) - nRndOptDamage - nExtraValue;
	}

	if( !bGenItemToCs ) {
		nMinDmg = 0;  nMaxDmg = 0; nRndOptDamage = 0; nExtraValue = 0; uGRADE = 0;
	}

	tbDamage.set("Min", nMinDmg);
	tbDamage.set("Max", nMaxDmg);
	tbDamage.set("RndOpt", nRndOptDamage);
	tbDamage.set("Extra", nExtraValue);

	if ( GUID_NULL == m_sItemCustom.guid )
		tbDamage.set("bDontCreate", true);
	else
		tbDamage.set("bDontCreate", false);

	if ( -1 != m_pItemData->GetBasicAttackDamageIndex() )
		tbDamage.set("RandomStat", true);
	else
		tbDamage.set("RandomStat", false);

	tbDamage.set("bRebuild", m_pItemData->IsEditableBasicAttackDamage());

	tb.set("Damage", tbDamage);

	///// 방어 /////////////////////////////////////////////////////////////////////////////////////////////

	LuaTable tbDefence( GLWidgetScript::GetInstance().GetLuaState() );

	const SITEM pTempITEM = *GLogicData::GetInstance().GetItem(m_sItemCustom.GetNativeID()); 

	COSTUMEUSERVALUE::sCostumeUser::sInvest rTemp;

	int iDefCsValue = 0;

	if( m_sItemCustom.costumeUser.getTypeInfo( (BYTE)E_DEFENCE, rTemp ) ) {

		const COSTUMESTATSDATA::sPsData* pData = COSSDATA.GetStatInfo(pTempITEM.sSuitOp.emSuit, E_DEFENCE);

		if( pData ) {
			iDefCsValue = (short)(rTemp.wInvestPt*pData->fInc);
		}
	}

	//int nBasicDef = static_cast<int>( m_pItemData->sSuitOp.nDefense);
	//int nRndOptDef = static_cast<int>( m_sItemCustom.getdefense() - m_pItemData->sSuitOp.nDefense);
	int nBasicDef = static_cast<int>( m_sItemCustom.GetBasicDefence() );
	int nRndOptDef = static_cast<int>( m_sItemCustom.getdefense() - m_sItemCustom.GetBasicDefence() );

	nExtraValue = m_sItemCustom.GETGRADE_DEFENSE();
	uGRADE = m_sItemCustom.GETGRADE(EMGRINDING_DEFENSE);

	if( !bGenItemToCs )
	{
		nBasicDef  = iDefCsValue;
		nRndOptDef = static_cast<int>( m_sItemCustom.getdefense(charIdx, nLevel) - iDefCsValue);

		iDefCsValue = 0;
	}

	if( m_fDurability < 1.0f )
	{
		nRndOptDef = static_cast<int>( nRndOptDef * m_fDurability );
		nExtraValue = static_cast<int>( nExtraValue * m_fDurability );
		nBasicDef = static_cast<int>(m_sItemCustom.GETDEFENSE() * m_fDurability) - nRndOptDef - nExtraValue;
	}
	
	tbDefence.set("Def", nBasicDef);
	tbDefence.set("RndOpt", nRndOptDef);
	tbDefence.set("Extra", nExtraValue);

	if ( GUID_NULL == m_sItemCustom.guid )
		tbDefence.set("bDontCreate", true);
	else
		tbDefence.set("bDontCreate", false);

	if ( -1 != m_pItemData->GetBasicDefenceIndex() )
		tbDefence.set("RandomStat", true);
	else
		tbDefence.set("RandomStat", false);

	tbDefence.set("bRebuild", m_pItemData->IsEditableBasicDefence());

	tb.set("Defence", tbDefence);

	// 명중률 /////////////////////////////////////////////////////////////////////////////////////////////////

	LuaTable tbHitRate( GLWidgetScript::GetInstance().GetLuaState() );

	int nBasicHitRate = static_cast<int>( m_pItemData->sSuitOp.nHitRate );
	int nRndOptHitRate = static_cast<int>( m_sItemCustom.GETHITRATE() - m_pItemData->sSuitOp.nHitRate );

	if( !bGenItemToCs )
	{
		nBasicHitRate  = 0;
		nRndOptHitRate = static_cast<int>( m_sItemCustom.GETHITRATE() );
	}
	
	if( m_pItemDisguise != NULL )
	{
		if(m_fDurability < 1.0f)
		{
			//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
			nRndOptHitRate -= m_pItemDisguise->sSuitOp.nHitRate;
			nRndOptHitRate = static_cast<int>( nRndOptHitRate * m_fDurability );	
			nRndOptHitRate += m_pItemDisguise->sSuitOp.nHitRate;
			nBasicHitRate = static_cast<int>( m_sItemCustom.GETHITRATE() * m_fDurability ) - nRndOptHitRate;	
		}
	}
	else
	{
		if(m_fDurability < 1.0f)
		{
			nRndOptHitRate = static_cast<int>( nRndOptHitRate * m_fDurability );
			// 실제 적용 수치와 튤팁 출력간의 오차 보정
			nBasicHitRate = static_cast<int>(m_sItemCustom.GETHITRATE() * m_fDurability) - nRndOptHitRate;
		}
	}
	
	tbHitRate.set( "Hit", nBasicHitRate );
	tbHitRate.set( "RndOpt", nRndOptHitRate );

	tb.set( "HitRate", tbHitRate );

	// 회피율 /////////////////////////////////////////////////////////////////////////////////////////////////
	
	LuaTable tbAvoidRate( GLWidgetScript::GetInstance().GetLuaState() );

	int nBasicAvoidRate = static_cast<int>( m_pItemData->sSuitOp.nAvoidRate );
	int nRndOptAvoidRate = static_cast<int>( m_sItemCustom.GETAVOIDRATE() - m_pItemData->sSuitOp.nAvoidRate );

	if( !bGenItemToCs )
	{
		nBasicAvoidRate  = 0;
		nRndOptAvoidRate = m_sItemCustom.GETAVOIDRATE();
	}

	if(m_pItemDisguise != NULL)
	{
		if(m_fDurability < 1.0f)
		{
			//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
			nRndOptAvoidRate -= m_pItemDisguise->sSuitOp.nAvoidRate;
			nRndOptAvoidRate = static_cast<int>( (nRndOptAvoidRate -  m_pItemDisguise->sSuitOp.nAvoidRate) * m_fDurability ); 
			//코스튬 옵션은 랜덤 옵션에 포함됨
			nRndOptAvoidRate += m_pItemDisguise->sSuitOp.nAvoidRate;
			// 실제 적용 수치와 튤팁 출력간의 오차 보정
			nBasicAvoidRate = static_cast<int>(m_sItemCustom.GETAVOIDRATE() * m_fDurability) - nRndOptAvoidRate;
		}
	}	
	else
	{
		if( m_fDurability < 1.0f )
		{
			nRndOptAvoidRate = static_cast<int>( nRndOptAvoidRate * m_fDurability ); 
			nBasicAvoidRate = static_cast<int>(m_sItemCustom.GETAVOIDRATE() * m_fDurability) - nRndOptAvoidRate;
		}
	}

	tbAvoidRate.set( "Avoid", nBasicAvoidRate );
	tbAvoidRate.set( "RndOpt", nRndOptAvoidRate );

	tb.set( "AvoidRate", tbAvoidRate );

	// 소모 SP /////////////////////////////////////////////////////////////////////////////////////////////////
	
	LuaTable tbCostSp( GLWidgetScript::GetInstance().GetLuaState() );

	int nBasicReqSP = static_cast<int>( m_pItemData->sSuitOp.wReqSP );
	int nRndOptReqSP = static_cast<int>( m_sItemCustom.GETREQ_SP() - m_pItemData->sSuitOp.wReqSP);

	if( NULL != m_pItemDisguise )
	{
		if( m_fDurability < 1.0f )
		{
			//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
			nRndOptReqSP -= static_cast<int>( m_pItemDisguise->sSuitOp.wReqSP );	
			nRndOptReqSP = static_cast<int>( nRndOptReqSP * m_fDurability );
			//코스튬 옵션은 랜덤 옵션에 포함됨
			nRndOptReqSP += static_cast<int>( m_pItemDisguise->sSuitOp.wReqSP );
			// 실제 적용 수치와 튤팁 출력간의 오차 보정
			nBasicReqSP = static_cast<int>( m_sItemCustom.GETREQ_SP()* m_fDurability ) - nRndOptReqSP;

		}
	}
	else
	{
		if( m_fDurability < 1.0f )
		{
			nRndOptReqSP = static_cast<int>( nRndOptReqSP * m_fDurability );
			// 실제 적용 수치와 튤팁 출력간의 오차 보정
			nBasicReqSP = static_cast<int>(m_sItemCustom.GETREQ_SP() * m_fDurability) - nRndOptReqSP;
		}
	}

	tbCostSp.set("Sp", nBasicReqSP);
	tbCostSp.set("RndOpt", nRndOptReqSP);

	tb.set("CostSp", tbCostSp);

	// 가산 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

	LuaTable tbAddOpt( GLWidgetScript::GetInstance().GetLuaState() );	
	//const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;
	int iGenItemToCs = 0;
	//const SITEM* pItem = NULL; GLogicData::GetInstance().GetItem(m_sItemCustom.GetNativeID());

	if ( m_sItemCustom.nidDISGUISE != NATIVEID_NULL() )
	{
		SITEMCUSTOM sCustomeItem(m_sItemCustom.nidDISGUISE);

		if( sCustomeItem.IsDisguise() ) {
			iGenItemToCs = 1;
			//pItem = GLogicData::GetInstance().GetItem(sCustomeItem.GetNativeID());
		}
	}
	/*else 
	{
		pItem = GLogicData::GetInstance().GetItem(m_sItemCustom.GetNativeID());
	}

	if( pItem == NULL ) return;*/

	ITEM::SADDON arrVALUE[EMADD_SIZE];
	SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );

	// 기력치는 랜덤옵션의 영향을 받기 때문에 추가 계산이 필요하다;
	int nOption_MagicDamage = 0;

	for ( int i = 0; i < SITEMCUSTOM_ADDON::ADDON_SIZE; ++i )
	{
		EMITEM_ADDON emTYPE = m_sItemCustom.sAddonInfo.m_sADDON[i].emTYPE;

		// 타입을 세팅한다;
		arrVALUE[emTYPE].emTYPE = emTYPE;

		const DWORD dwType = static_cast< DWORD >( emTYPE );
		const int nValue = m_sItemCustom.sAddonInfo.GetValue< const int >( i, ITEM::bAddonValueInteger[dwType] );
		const float fValue = m_sItemCustom.sAddonInfo.GetValue< const float >( i, ITEM::bAddonValueInteger[dwType] );

		// 값을 세팅한다;
		if ( emTYPE != EMADD_NONE )
		{
			//내구도 적용
			if ( ITEM::bAddonValueInteger[emTYPE] )
				arrVALUE[ emTYPE ].nVALUE += (static_cast<int>( nValue * m_fDurability ) * iGenItemToCs);
			else
				arrVALUE[ emTYPE ].fVALUE += (static_cast<float>( fValue * m_fDurability ) * iGenItemToCs);

			// 먼저 가산옵션의 기력치를 세팅한다;
			// 최초에는 가산옵션의 기력치가 기본값이기 때문에 대입한다;
			if ( EMADD_MA == emTYPE )
			{
				nOption_MagicDamage = nValue;
			}

			// 랜덤일 경우 값에 그 개수를 세팅하도록 한다;
			if ( EMADD_RANDOM == emTYPE && false == bSimple )
			{
				if ( ITEM::bAddonValueInteger[emTYPE] )
					arrVALUE[ emTYPE ].nVALUE += 1;
				else
					arrVALUE[ emTYPE ].fVALUE += 1.f;
			}
		}
	}

	//arrVALUE[EMADD_HP].nVALUE = static_cast<int>( m_sItemCustom.GETADDHP() * m_fDurability * iGenItemToCs );
	//arrVALUE[EMADD_MP].nVALUE = static_cast<int>( m_sItemCustom.GETADDMP() * m_fDurability * iGenItemToCs );
	//arrVALUE[EMADD_SP].nVALUE = static_cast<int>( m_sItemCustom.GETADDSP() * m_fDurability * iGenItemToCs );
	//arrVALUE[EMADD_MA].nVALUE = static_cast<int>( m_sItemCustom.GETADDMA() * m_fDurability * iGenItemToCs );

	/// 랜덤옵션의 퍼센트대미지, 대미지, 기력치 값을 이용해 최종 기력치를 계산한다;
	if ( false == bSimple )
	{
		float fDamage_Rate = m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_DAMAGE);
		float fDamage_Value = m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DAMAGE);
		float fMagic_Value = m_sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MAGIC);

		// 퍼센트 대미지를 계산한다;
		nOption_MagicDamage += static_cast< int >( nOption_MagicDamage * fDamage_Rate * 0.01 );

		// 대미지 + 기력치 를 더해준다;
		nOption_MagicDamage += static_cast< int >( fDamage_Value );
		nOption_MagicDamage += static_cast< int >( fMagic_Value );

		if ( ITEM::bAddonValueInteger[EMADD_MA] )
			arrVALUE[ EMADD_MA ].nVALUE = nOption_MagicDamage;
		else
			arrVALUE[ EMADD_MA ].fVALUE = static_cast< float >( nOption_MagicDamage );

		//	코스튬 능력치
		for( int i = 0; i < E_ADDED_MAX; ++i)
		{
			BYTE iType = (BYTE)(E_ADD_HITRATE+i);

			COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

			if( m_sItemCustom.costumeUser.getTypeInfo( iType, sData ) )
			{
				double iValue = COSSDATA.GetStatInc( m_pItemData->sSuitOp.emSuit, (COSTUME_STATS)(E_ADD_HITRATE+i) );

				int nApplyValue = (int)(iValue*sData.wInvestPt);

				COSTUME_STATS eType = (COSTUME_STATS)(iType);
				switch( eType )
				{
				case E_ADD_HITRATE   : arrVALUE[EMADD_HITRATE].nVALUE += nApplyValue; break;
				case E_ADD_AVOIDRATE : arrVALUE[EMADD_AVOIDRATE].nVALUE += nApplyValue; break;
				case E_ADD_DAMAGE    : arrVALUE[EMADD_DAMAGE].nVALUE += nApplyValue; break;
				case E_ADD_DEFENSE   : arrVALUE[EMADD_DEFENSE].nVALUE += nApplyValue; break;
				case E_ADD_HP        : arrVALUE[EMADD_HP].nVALUE += nApplyValue; break;
				case E_ADD_STATS_POW : arrVALUE[EMADD_STATS_POW].nVALUE += nApplyValue ; break;
				case E_ADD_STATS_STA : arrVALUE[EMADD_STATS_STR].nVALUE += nApplyValue ; break;
				case E_ADD_STATS_SPI : arrVALUE[EMADD_STATS_SPI].nVALUE += nApplyValue ; break;
				case E_ADD_STATS_DEX : arrVALUE[EMADD_STATS_DEX].nVALUE += nApplyValue ; break;
				case E_ADD_ENERGY    : arrVALUE[EMADD_MA].nVALUE += nApplyValue; break;
				case E_ADD_SHOOTING  : arrVALUE[EMADD_SA].nVALUE += nApplyValue; break;
				case E_ADD_MELEE     : arrVALUE[EMADD_PA].nVALUE += nApplyValue; break;
				}
			}
		}

		if ( m_sItemCustom.IsCSApplied() )
		{
			for( int i = 0; i < E_VARRATE_MAX; ++i)
			{
				BYTE iType = (BYTE)(E_R_HP_INC+i);

				COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

				if( m_sItemCustom.costumeUser.getTypeInfo(iType, sData) )
				{
					float fValue = (float)COSSDATA.GetStatInc( m_pItemData->sSuitOp.emSuit, (COSTUME_STATS)(E_R_HP_INC+i));

					float fApplyValue = fValue*sData.wInvestPt;

					COSTUME_STATS eType = (COSTUME_STATS)iType;

					switch(eType)
					{
					case E_R_HP_INC      : arrVALUE[EMADDEX_INCR_HP].fVALUE		+= fApplyValue; break;
					case E_R_MP_INC      : arrVALUE[EMADDEX_INCR_MP].fVALUE		+= fApplyValue; break;
					case E_R_SP_INC      : arrVALUE[EMADDEX_INCR_SP].fVALUE		+= fApplyValue; break;
					case E_R_HMSP_INC    : arrVALUE[EMADDEX_INCR_AP].fVALUE		+= fApplyValue; break;
					case E_R_MOVESPEED   : arrVALUE[EMADDEX_INCR_MOVE_SPEED].fVALUE	+= fApplyValue; break;
					case E_R_ATTACKSPEED : arrVALUE[EMADDEX_INCR_ATTACK_SPEED].fVALUE	+= fApplyValue; break;
					case E_R_CRITICALHIT : arrVALUE[EMADDEX_INCR_CRITICAL_RATE].fVALUE	+= fApplyValue; break;
					case E_R_STRIKEHIT   : arrVALUE[EMADDEX_INCR_CRUSHING_BLOW].fVALUE	+= fApplyValue; break;
					}
				}
			}
		}
		else
		{
			arrVALUE[EMADDEX_INCR_HP].fVALUE = ( m_sItemCustom.GETINCHP() * m_fDurability  * iGenItemToCs );
			arrVALUE[EMADDEX_INCR_MP].fVALUE = ( m_sItemCustom.GETINCMP() * m_fDurability  * iGenItemToCs );
			arrVALUE[EMADDEX_INCR_SP].fVALUE = ( m_sItemCustom.GETINCSP() * m_fDurability  * iGenItemToCs );
			arrVALUE[EMADDEX_INCR_AP].fVALUE = ( m_sItemCustom.GETINCAP() * m_fDurability  * iGenItemToCs ); 
		}
	}

	INT nTableCount(0);
	for( int i=1; i<EMADD_SIZE; ++i)
	{
		if ( ITEM::bAddonValueInteger[i] )
		{
			if( arrVALUE[i].nVALUE != 0 )
			{
				LuaTable tbOpt( GLWidgetScript::GetInstance().GetLuaState() );	
				tbOpt.set( 1, true );	// int인가?;
				tbOpt.set( 2, arrVALUE[i].nVALUE );
				tbOpt.set( 3, i );
				tbAddOpt.set( ++nTableCount , tbOpt );
			}
		}
		else
		{
			LuaTable tbOpt( GLWidgetScript::GetInstance().GetLuaState() );	
			if( arrVALUE[i].fVALUE != 0.0f )
			{
				// comment.lua 참조 %만들기 위해 만든 변수로 보임.
				if ( COMMENT::ITEMADDON_VAR_SCALE[i] != -1 )
				{
					if ( m_pItemData->BasicType() != ITEM_PET_FOOD
						&& m_pItemData->BasicType() != ITEM_VEHICLE_OIL )
						arrVALUE[i].fVALUE *= COMMENT::ITEMADDON_VAR_SCALE[i];
				}

				float fValue = 0.0f;
				if( (EMITEM_ADDON)i == EMVAR_MOVE_SPEED )
					fValue = m_sItemCustom.GETMOVESPEED() * m_fDurability;

				CString strTemp = sc::string::format( "%.2f", arrVALUE[i].fVALUE ).c_str();

				CutZero( strTemp, 2 );
				tbOpt.set( 1, false);	// int인가?;
				tbOpt.set( 2, strTemp.GetBuffer() );
				tbOpt.set( 3, i );
				tbOpt.set( 4, COMMENT::ITEMADDON_VAR_SCALE[i] != -1 );

				tbAddOpt.set( ++nTableCount , tbOpt );
			}
		}
	}

	tb.set( "AddOpt", tbAddOpt );

	// 특수기능 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 	LuaTable tbSpecialOpt( GLWidgetScript::GetInstance().GetLuaState() );	
// 	float fArrVALUE[EMVAR_SIZE];
// 	SecureZeroMemory( fArrVALUE, sizeof(fArrVALUE) );
//  	if ( m_sItemCustom.IsCSApplied() )
//  	{
//  		for( int i = 0; i < E_VARRATE_MAX; ++i)
//  		{
//  			BYTE iType = (BYTE)(E_R_HP_INC+i);
//  
//  			COSTUMEUSERVALUE::sCostumeUser::sInvest sData;
//  
//  			if( m_sItemCustom.costumeUser.getTypeInfo(iType, sData) )
//  			{
//  				float fValue = (float)COSSDATA.GetStatInc( m_pItemData->sSuitOp.emSuit, (COSTUME_STATS)(E_R_HP_INC+i));
//  
//  				float fApplyValue = fValue*sData.wInvestPt;
//  
//  				COSTUME_STATS eType = (COSTUME_STATS)iType;
//  
//  				switch(eType)
//  				{
//  				case E_R_HP_INC      : fArrVALUE[EMVAR_HP]            += fApplyValue; break;
//  				case E_R_MP_INC      : fArrVALUE[EMVAR_MP]            += fApplyValue; break;
//  				case E_R_SP_INC      : fArrVALUE[EMVAR_SP]            += fApplyValue; break;
//  				case E_R_HMSP_INC    : fArrVALUE[EMVAR_AP]            += fApplyValue; break;
//  				case E_R_MOVESPEED   : fArrVALUE[EMVAR_MOVE_SPEED]    += fApplyValue; break;
//  				case E_R_ATTACKSPEED : fArrVALUE[EMVAR_ATTACK_SPEED]  += fApplyValue; break;
//  				case E_R_CRITICALHIT : fArrVALUE[EMVAR_CRITICAL_RATE] += fApplyValue; break;
//  				case E_R_STRIKEHIT   : fArrVALUE[EMVAR_CRUSHING_BLOW] += fApplyValue; break;
//  				}
//  			}
//  		}
//  	}
// 	else
// 	{
// 		EMITEM_VAR emITEM_VAR = m_pItemData->sSuitOp.sVARIATE.emTYPE;
// 		EMITEM_VAR emITEM_VOL = m_pItemData->sSuitOp.sVOLUME.emTYPE;
// 
// 		if ( emITEM_VAR != EMVAR_NONE )
// 		{
// 			switch ( emITEM_VAR )
// 			{
// 			case EMVAR_HP:
// 			case EMVAR_MP:
// 			case EMVAR_SP:
// 			case EMVAR_AP:
// 				break;
// 			default:
// 				fArrVALUE[emITEM_VAR] = ( m_pItemData->sSuitOp.sVARIATE.fVariate * m_fDurability  * iGenItemToCs );
// 				break;
// 			};
// 		}
// 
// 		fArrVALUE[EMVAR_HP] = ( m_sItemCustom.GETINCHP() * m_fDurability  * iGenItemToCs );
// 		fArrVALUE[EMVAR_MP] = ( m_sItemCustom.GETINCMP() * m_fDurability  * iGenItemToCs );
// 		fArrVALUE[EMVAR_SP] = ( m_sItemCustom.GETINCSP() * m_fDurability  * iGenItemToCs );
// 		fArrVALUE[EMVAR_AP] = ( m_sItemCustom.GETINCAP() * m_fDurability  * iGenItemToCs ); 
// 
// 		float fCostumVolume = 0.0f;
// 
// 		EMITEM_VAR emDisguiseItemVol = EMVAR_NONE;
// 		if( NULL != m_pItemDisguise )
// 			emDisguiseItemVol = m_pItemDisguise->sSuitOp.sVOLUME.emTYPE;
// 		//위쪽은 이동속도를 제외한 특수능력 연산
// 
// 		/**********************이동속도**********************************/
// 		if( emITEM_VOL != emDisguiseItemVol && 
// 			emITEM_VOL != EMVAR_NONE && 
// 			emDisguiseItemVol != EMVAR_NONE )
// 		{
// 			LuaTable tbOpt( GLWidgetScript::GetInstance().GetLuaState() );	
// 			float fVOLUME = 0.0f;
// 			int nIndex = 0;
// 			CString strTemp;
// 
// 			// Basic--------------------------------------------
// 			if( emITEM_VOL == EMVAR_MOVE_SPEED )
// 				fVOLUME = m_sItemCustom.GETMOVESPEED() * m_fDurability;
// 			else
// 				fVOLUME = m_pItemData->sSuitOp.sVOLUME.fVolume * m_fDurability;
// 
// 			if( 0.0f != fVOLUME )
// 			{			
// 				strTemp = sc::string::format( "%.4f", fVOLUME ).c_str();
// 				CutZero( strTemp, 4 );			
// 
// 				tbOpt.set( 1, strTemp.GetBuffer() );
// 				tbOpt.set( 2, emITEM_VOL );
// 				tbOpt.set( 3, true );
// 			}
// 
// 			// Disguise------------------------------------------------
// 			if( emDisguiseItemVol == EMVAR_MOVE_SPEED )
// 				fVOLUME = m_sItemCustom.GETMOVESPEED();
// 			else
// 				fVOLUME = fCostumVolume;
// 
// 			if( 0.0f != fVOLUME )
// 			{
// 
// 				strTemp = sc::string::format( "%.4f", fVOLUME ).c_str();
// 				CutZero( strTemp, 4 );			
// 
// 				tbOpt.set( 1, strTemp.GetBuffer() );
// 				tbOpt.set( 2, emDisguiseItemVol );
// 				tbOpt.set( 3, true );
// 			}
// 
// 			tbSpecialOpt.set( EMVAR_MOVE_SPEED, tbOpt);
// 		}
// 		else if( emITEM_VOL != EMVAR_NONE || emDisguiseItemVol != EMVAR_NONE )
// 		{
// 			LuaTable tbOpt( GLWidgetScript::GetInstance().GetLuaState() );	
// 			float fVOLUME = 0.0f;
// 			if( emITEM_VOL == EMVAR_MOVE_SPEED || emDisguiseItemVol == EMVAR_MOVE_SPEED )
// 				fVOLUME = m_sItemCustom.GETMOVESPEED() * m_fDurability;
// 			else
// 				fVOLUME = (m_pItemData->sSuitOp.sVOLUME.fVolume * m_fDurability) + fCostumVolume;	//공격속도
// 
// 			if( 0.0f != fVOLUME )
// 			{
// 				CString strTemp = sc::string::format( "%.4f", fVOLUME ).c_str();
// 				CutZero( strTemp, 4 );	
// 
// 				tbOpt.set( 1, strTemp.GetBuffer() );
// 				tbOpt.set( 2, emITEM_VOL );
// 				tbOpt.set( 3, true );
// 			}
// 
// 			tbSpecialOpt.set( EMVAR_MOVE_SPEED, tbOpt);
// 		}
// 		/**************************************************************/
// 	}
// 
// 	for ( int i = 1; i < EMVAR_SIZE; ++i )
// 	{
// 		LuaTable tbOpt( GLWidgetScript::GetInstance().GetLuaState() );	
// 		if ( fArrVALUE[i] != 0.0f ) 
// 		{
// 			if ( m_pItemData->BasicType() != ITEM_PET_FOOD
// 				&& m_pItemData->BasicType() != ITEM_VEHICLE_OIL )
// 				fArrVALUE[i] *= COMMENT::ITEMVAR_SCALE[i];
// 
// 			CString strTemp = sc::string::format( "%.2f", fArrVALUE[i] ).c_str();
// 
// 			CutZero( strTemp, 2 );
// 			tbOpt.set( 1, strTemp.GetBuffer() );
// 			tbOpt.set( 2, i );
// 			tbOpt.set( 3, false );
// 		}
// 
// 		// 능력치 부여된 코스튬 이속 옵션 출력안되어 주석 처리함.
// 		// 어차피 아래 조건 충족하면 덮는다.
// 		//if( i == EMVAR_MOVE_SPEED )
// 		//	continue;
// 
// 		tbSpecialOpt.set( i, tbOpt);
//	}
//	tb.set( "SpecialOpt", tbSpecialOpt );

	// 경험치 배율 /////////////////////////////////////////////////////////////////////////////////////////////////
	
	float fExpMultiple = m_pItemData->GetExpMultiple(); //내구도 적용

	if( iGenItemToCs == 0 )
		fExpMultiple = 1.f;

	LuaTable tbExp( GLWidgetScript::GetInstance().GetLuaState() );	

	//내구도 적용
	if( (fExpMultiple > 1.0f) || (m_fDurability < 1.0f) )
	{
		fExpMultiple = ( (fExpMultiple - 1.0f) * m_fDurability ) + 1.0f;
	}

	COSTUMEUSERVALUE::sCostumeUser::sInvest sData;
	if( m_sItemCustom.costumeUser.getTypeInfo( (BYTE) E_EXPMULTIPLE, sData ) )
	{
		float fValue = (float)COSSDATA.GetStatInc( m_pItemData->sSuitOp.emSuit, E_EXPMULTIPLE);
		float fApplyValue = fValue*sData.wInvestPt;

		fExpMultiple += fApplyValue;
	}

	//pItem = GLogicData::GetInstance().GetItem(m_sItemCustom.nidDISGUISE);

	//SITEMCUSTOM sCustomeItem(m_sItemCustom.nidDISGUISE);
	//if ( pItem && sCustomeItem.IsDisguise() )
	//{
	//	fExpMultiple += pItem->GetExpMultiple() - 1.0f;
	//	fExpMultiple = max( fExpMultiple, 1.0f );
	//}

	if ( fExpMultiple != 1.0f )
	{
		CString strTemp = sc::string::format( "%.2f", fExpMultiple * 100.0f - 100.0f ).c_str();
		CutZero( strTemp, 2 );
		tbExp.set( "Exp", strTemp.GetBuffer() );
	}

	tb.set( "ExpMultiple", tbExp );

	// 저항 /////////////////////////////////////////////////////////////////////////////////////////
	LuaTable tbResist( GLWidgetScript::GetInstance().GetLuaState() );	
	LuaTable tbResistAdd( GLWidgetScript::GetInstance().GetLuaState() );	

	//const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;

	float fRegist = 0.0f;

	//COSTUMEUSERVALUE::sCostumeUser::sInvest sData;
	sData.reset();
	if( m_sItemCustom.costumeUser.getTypeInfo( (BYTE) E_RESIST, sData ) )
	{
		float fValue = (float) COSSDATA.GetStatInc( m_pItemData->sSuitOp.emSuit, E_RESIST );
		fRegist = fValue * sData.wInvestPt;
	}

	const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;
	int nELEC   = sSUIT.sResist.nElectric;
	int nFIRE   = sSUIT.sResist.nFire;
	int nICE    = sSUIT.sResist.nIce;
	int nPOISON = sSUIT.sResist.nPoison;
	int nSPIRIT = sSUIT.sResist.nSpirit;

	if( iGenItemToCs == 0 )
	{
		nELEC = static_cast< int >( fRegist );
		nFIRE = static_cast< int >( fRegist );
		nICE = static_cast< int >( fRegist );
		nPOISON = static_cast< int >( fRegist );
		nSPIRIT = static_cast< int >( fRegist );
	}

	int nAddELEC( static_cast<int>(m_sItemCustom.GETRESIST_ELEC() - nELEC) ); 
	int nAddFIRE( static_cast<int>(m_sItemCustom.GETRESIST_FIRE() - nFIRE) ); 
	int nAddICE( static_cast<int>(m_sItemCustom.GETRESIST_ICE() - nICE) ); 
	int nAddPOISON( static_cast<int>(m_sItemCustom.GETRESIST_POISON() - nPOISON) ); 
	int nAddSPIRIT( static_cast<int>(m_sItemCustom.GETRESIST_SPIRIT() - nSPIRIT) );

	if( m_fDurability < 1.0f )
	{
		nELEC   = static_cast<int>( nELEC   * m_fDurability ) + (int) fRegist;
		nFIRE   = static_cast<int>( nFIRE   * m_fDurability ) + (int) fRegist;
		nICE    = static_cast<int>( nICE    * m_fDurability ) + (int) fRegist;
		nPOISON = static_cast<int>( nPOISON * m_fDurability ) + (int) fRegist;
		nSPIRIT = static_cast<int>( nSPIRIT * m_fDurability ) + (int) fRegist;

		nAddELEC   = static_cast<int>( nAddELEC   * m_fDurability );
		nAddFIRE   = static_cast<int>( nAddFIRE   * m_fDurability );
		nAddICE    = static_cast<int>( nAddICE    * m_fDurability );
		nAddPOISON = static_cast<int>( nAddPOISON * m_fDurability );
		nAddSPIRIT = static_cast<int>( nAddSPIRIT * m_fDurability );
	}
	
	if ( nELEC || nFIRE || nICE || nPOISON || nSPIRIT ||
		nAddELEC || nAddFIRE || nAddICE || nAddPOISON || nAddSPIRIT )
	{
		tbResist.set( "Spirit", nSPIRIT );
		tbResist.set( "Fire", nFIRE );
		tbResist.set( "Ice", nICE );
		tbResist.set( "Elec", nELEC );
		tbResist.set( "Poison", nPOISON );

		tbResistAdd.set( "Spirit", nAddSPIRIT );
		tbResistAdd.set( "Fire", nAddFIRE );
		tbResistAdd.set( "Ice", nAddICE );
		tbResistAdd.set( "Elec", nAddELEC );
		tbResistAdd.set( "Poison", nAddPOISON );
	}

	tb.set( "Resist", tbResist );
	tb.set( "ResistAdd", tbResistAdd );
}

void GLItemTooltip::GenDrugCall( LuaTable& tb )
{
	LuaTable tbDrugCall( GLWidgetScript::GetInstance().GetLuaState() );

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	// 맵 이름, 좌표
	bool bValid = false;
	std::string strMapName = "";
	int nPosX = 0;
	int nPosY = 0;

	switch( m_pItemData->sDrugOp.emDrug )
	{
	case ITEM_DRUG_CALL_REGEN :
		{
			strMapName = pClient->GetMapName( m_pCharacter->m_sStartMapID );

			SMAPNODE* pMapNode = pClient->FindMapNode( m_pCharacter->m_sStartMapID );
			if( NULL != pMapNode )
			{
				GLLevelFileClient cLevelFile( pClient );
				BOOL bOk = cLevelFile.LoadFile( pMapNode->m_LevelFile.c_str(), NULL );
				if( TRUE == bOk )
				{	
					D3DXVECTOR3 vStartPos;
					PDXLANDGATE pGate = cLevelFile.GetLandGateMan()->FindLandGate( m_pCharacter->m_dwStartGate );
					if( NULL != pGate )
					{
						vStartPos = pGate->GetGenPos( DxLandGate::GEN_RENDUM );
					}
					else
					{
						pGate = cLevelFile.GetLandGateMan()->FindLandGate( DWORD( 0 ) );
						if( NULL != pGate )
							vStartPos = pGate->GetGenPos( DxLandGate::GEN_RENDUM );
					}

					cLevelFile.GetMapAxisInfo().Convert2MapPos( vStartPos.x, vStartPos.z, nPosX, nPosY );

					bValid = true;
				}
			}
		}
		break;

	case ITEM_DRUG_CALL_LASTCALL :
		{
			if( true == m_pCharacter->m_sLastCallMapID.IsValidNativeID() )
			{
				strMapName = pClient->GetMapName( m_pCharacter->m_sLastCallMapID );

				SMAPNODE* pMapNode = pClient->FindMapNode( m_pCharacter->m_sLastCallMapID );
				if( NULL != pMapNode )
				{
					GLLevelFileClient cLevelFile( pClient );
					BOOL bOk = cLevelFile.LoadFile( pMapNode->m_LevelFile.c_str(), NULL );
					if( TRUE == bOk )
					{	
						cLevelFile.GetMapAxisInfo().Convert2MapPos( 
							m_pCharacter->m_vLastCallPos.x, m_pCharacter->m_vLastCallPos.z, nPosX, nPosY );

						bValid = true;
					}
				}
			}
			//else
			//{
			//	bValid = false;
			//}
		}
		break;

	case ITEM_DRUG_CALL_TELEPORT :
		{
			strMapName = pClient->GetMapName( m_pItemData->sBasicOp.sSubID );
			nPosX = (int) m_pItemData->sBasicOp.wPosX;
			nPosX = (int) m_pItemData->sBasicOp.wPosY;

			bValid = true;
		}
		break;
	}

	tbDrugCall.set( 1, bValid );
	tbDrugCall.set( 2, strMapName.c_str() );
	tbDrugCall.set( 3, nPosX );
	tbDrugCall.set( 4, nPosY );

	tb.set( "tbDrugCall", tbDrugCall );
}
