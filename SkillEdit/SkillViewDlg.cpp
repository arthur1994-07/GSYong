#include "stdafx.h"

#include "SkillEdit.h"
#include "SkillViewDlg.h"
#include "SkillIconImage.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "../SigmaCore/String/StringFormat.h"

#include "../RanLogic/GLUseFeatures.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/DxBase/RenderDevice.h"

#include "../RanLogic/GLSkillToolTip.h"

IMPLEMENT_DYNAMIC( SkillViewDlg, CDialog )

SkillViewDlg::SkillViewDlg( CWnd* pParent )
: CDialog( SkillViewDlg::IDD, pParent )
, m_skillLevel( 0 )
, m_pDefaultCharacter( NULL )
, m_MainSkillImage( NULL )
, m_MultiSkillImage( NULL )
{
	m_pDefaultCharacter = new GLCHARLOGIC();

	m_MainSkillImage = new SkillIconImage();
	m_MultiSkillImage = new SkillIconImage();
}

SkillViewDlg::~SkillViewDlg()
{
	SAFE_DELETE( m_pDefaultCharacter );
}

BEGIN_MESSAGE_MAP( SkillViewDlg, CDialog )
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_LEVEL, &SkillViewDlg::OnCbnSelchangeComboLevel)
END_MESSAGE_MAP()

// Message Procedure
void SkillViewDlg::SetSkill( GLSKILL* pSkill, UINT skillLevel )
{
	this->ShowWindow( SW_SHOW );

	if( !pSkill )
		return;

	m_skill = *pSkill;
	m_skillLevel = skillLevel;

	// Update List
	UpdateList( pSkill );
}

// Initialize
void SkillViewDlg::InitSkillImage()
{
	CWnd* pMainSkillImageWnd = GetDlgItem( IDC_MAIN_IMAGE );
	if( pMainSkillImageWnd )
	{
		pMainSkillImageWnd->SetWindowPos( NULL, 0, 0
			, SkillIconImage::EIconSize
			, SkillIconImage::EIconSize
			, SWP_NOMOVE );
		m_MainSkillImage->AttachWnd( pMainSkillImageWnd->GetSafeHwnd() );
	}

	CWnd* pMultiSkillImageWnd = GetDlgItem( IDC_MULTI_IMAGE );
	if( pMultiSkillImageWnd )
	{
		pMultiSkillImageWnd->SetWindowPos( NULL, 0, 0
			, SkillIconImage::EIconSize
			, SkillIconImage::EIconSize
			, SWP_NOMOVE );
		m_MultiSkillImage->AttachWnd( pMultiSkillImageWnd->GetSafeHwnd() );
	}
}

void SkillViewDlg::InitListMainSkill()
{
	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	m_ListMainSkill.SetExtendedStyle(
		m_ListMainSkill.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );
	m_ListMainSkill.GetClientRect( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	const int nColumnCount = 2;
	char* szColumnName[ nColumnCount ] = { "Name", "Explain" };
	int nColumnWidthPercent[ nColumnCount ] = { 15, 85 };

	for( int i=0; i<nColumnCount; ++i )
	{
		lvColumn.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = szColumnName[ i ];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent[ i ] ) / 100;
		m_ListMainSkill.InsertColumn( i, &lvColumn );
	}
}

void SkillViewDlg::InitListMultiTargetSkill()
{
	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	m_ListMultiTargetSkill.SetExtendedStyle(
		m_ListMultiTargetSkill.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );
	m_ListMultiTargetSkill.GetClientRect( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	const int nColumnCount = 2;
	char* szColumnName[ nColumnCount ] = { "Name", "Explain" };
	int nColumnWidthPercent[ nColumnCount ] = { 15, 85 };

	for( int i=0; i<nColumnCount; ++i )
	{
		lvColumn.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = szColumnName[ i ];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent[ i ] ) / 100;
		m_ListMultiTargetSkill.InsertColumn( i, &lvColumn );
	}
}

void SkillViewDlg::InitializeSkillView()
{
	InitSkillImage();
	InitListMainSkill();
	InitListMultiTargetSkill();

	// Timer 세팅
	SetTimer( 0, 1000 / 60, NULL );
}

/// Skill Information 텍스트 세팅
//! 스킬 이미지
static void GetSkillImage( GLSKILL* pSkill, SkillIconImage* image )
{
	if( !pSkill )
	{
		// 이미지 초기화
		image->SetFileName( "" );
		image->SetIconID( NATIVEID_NULL() );

		return;
	}

	SNATIVEID sICONINDEX = pSkill->m_sEXT_DATA.sICONINDEX;
	std::string strTextureName = pSkill->m_sEXT_DATA.strICONFILE;

	image->SetFileName( strTextureName );
	image->SetIconID( sICONINDEX );
}

//! 등급/속성
static void GetGradeAttributeText( std::string& strText
								  , GLSKILL* pSkill )
{
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
}

//! 적용 타입
static void GetTypeText( std::string& strText
						, GLSKILL* pSkill )
{
	strText = "";

	// Info : 적용 타입
	switch ( pSkill->m_sBASIC.emACTION_TYPE )
	{
	case SKILL::EMACTION_TYPE_NORMAL:
		{
			strText = sc::string::format( "%s",
				ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 0 ) );
		}
		break;

	case SKILL::EMACTION_TYPE_BUFF:
		{
			strText = sc::string::format( "%s",
				ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 1 ) );
		}
		break;

	case SKILL::EMACTION_TYPE_DEBUFF:
		{
			strText = sc::string::format( "%s",
				ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 2 ) );
		}
		break;

	case SKILL::EMACTION_TYPE_LIMIT:
		{
			strText = sc::string::format( "%s",
				ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 3 ) );
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
			}
			break;

		case SKILL::EMAPPLY_RANGE:
			{
				strText += sc::string::format( "/ %s",
					ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 5 ) );
			}
			break;

		case SKILL::EMAPPLY_MAGIC:
			{
				strText += sc::string::format( "/ %s",
					ID2GAMEWORD( "SKILL_TOOLTIP_BASIC_TYPE", 6 ) );
			}
			break;
		}
	}
	else if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
	{
		strText += ID2GAMEWORD( "SKILL_BASIC_INFO", 8 );
	}
}

//! 사용 무기
static void GetUseWeapon( std::string& strText
						 , GLSKILL* pSkill )
{
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
			}
		}
	}
}

//! 소모 조건
static void GetConsumeCondition( std::string& strText
								, GLSKILL* pSkill
								, UINT skillLevel )
{
	strText = "";

	//	사용시 소진량
	if ( pSkill->m_sBASIC.emROLE != SKILL::EMROLE_PASSIVE )
	{
		const SKILL::SAPPLY& sAPPLY = pSkill->m_sAPPLY;
		const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[ skillLevel ];

		int nIndex = 0;

		//	화살 개수
		if ( 0 < sDATA_LVL.wUSE_ARROWNUM )
		{
			strText += sc::string::format("%s %d ",
				ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 0 ),
				sDATA_LVL.wUSE_ARROWNUM );
		}

		//	부적 개수
		if ( 0 < sDATA_LVL.wUSE_CHARMNUM )
		{
			strText += sc::string::format("%s %d ",
				ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 1 ),
				sDATA_LVL.wUSE_CHARMNUM );
		}

		//	총알 개수
		if ( 0 < sDATA_LVL.wUSE_BULLETNUM )
		{
			strText += sc::string::format("%s %d ",
				ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 7 ),
				sDATA_LVL.wUSE_BULLETNUM );
		}

		// 배터리 소모량
		if ( 0 < sDATA_LVL.nUSE_BATTERY )
		{
			strText += sc::string::format("%s %d ",
				ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 7 ),
				sDATA_LVL.nUSE_BATTERY);
		}

		//	HP 소진량
		if ( 0 < sDATA_LVL.wUSE_HP )
		{
			strText += sc::string::format("%s %d ",
				ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 3 ),
				sDATA_LVL.wUSE_HP);
		}

		//	MP 소진량
		if ( 0 < sDATA_LVL.wUSE_MP )
		{
			strText += sc::string::format("%s %d ",
				ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 4 ),
				sDATA_LVL.wUSE_MP);
		}

		//	SP 소진량
		if ( 0 < sDATA_LVL.wUSE_SP )
		{
			strText += sc::string::format("%s %d ",
				ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 5 ),
				sDATA_LVL.wUSE_SP);
		}

		//	CP 소진량
		if ( 0 < sDATA_LVL.wUSE_CP )
		{
			strText += sc::string::format("%s %d ",
				ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 6 ),
				sDATA_LVL.wUSE_CP);
		}
	}
}

//! 재사용 시간
static void GetReuseTime( std::string& strText
						 , GLSKILL* pSkill
						 , UINT skillLevel )
{
	strText = "";

	const SKILL::SAPPLY& sAPPLY = pSkill->m_sAPPLY;
	const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[skillLevel];

	if ( 0 < sDATA_LVL.fDELAYTIME )
	{
		strText = sc::string::format("%2.1f%s",
			sDATA_LVL.fDELAYTIME,
			ID2GAMEWORD( "WAITSERVER_TIMELEFT_UNIT", 0 ) );
	}
}

//! 스킬 영역
static void GetSkillScope( std::string& strText
						  , GLSKILL* pSkill
						  , GLCHARLOGIC* pTarget
						  , UINT skillLevel )
{
	strText = "";

	const SKILL::SBASIC& sBASIC = pSkill->m_sBASIC;
	const SKILL::SAPPLY& sAPPLY = pSkill->m_sAPPLY;

	const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[skillLevel];

	switch ( pSkill->m_sBASIC.emIMPACT_TAR )
	{
	case TAR_SELF: // 자신
		switch ( pSkill->m_sBASIC.emIMPACT_REALM )
		{
		case REALM_SELF: // '목표' 자신
			strText += sc::string::format( "%s", ID2GAMEINTEXT( "SKILL_AREA_SELF_SELF" ) );
			break;

		case REALM_ZONE: // '목표' 주위
			switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
			{
			case EMIMPACT_SIDE_PARTY: // 자기편에게
				if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
					strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_SELF_ZONE_OUR" ),
					sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
				break;

			case EMIMPACT_SIDE_ENEMY: // 적에게
				if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
					strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_SELF_ZONE_ENEMY" ),
					sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
				break;
			case EMIMPACT_SIDE_ANYBODY: // 모두에게
				if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
					strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_SELF_ZONE_ANYBODY" ),
					sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
				break;
			};
		};
		break;

	case TAR_SPEC: // 타겟
		{
			switch ( pSkill->m_sBASIC.emIMPACT_REALM )
			{
			case REALM_SELF: // '목표' 자신
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case EMIMPACT_SIDE_PARTY: // 자기편에게
					if ( 0 < sBASIC.wTARRANGE )
						strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_SELF_OUR" ),
						sBASIC.wTARRANGE * 0.1f );
					break;
				case EMIMPACT_SIDE_ENEMY: // 적에게
					if ( 0 < sBASIC.wTARRANGE )
						strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_SELF_ENEMY" ),
						sBASIC.wTARRANGE * 0.1f );
					break;
				case EMIMPACT_SIDE_ANYBODY: // 모두에게
					if ( 0 < sBASIC.wTARRANGE )
						strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_SELF_ANYBODY" ),
						sBASIC.wTARRANGE * 0.1f );
					break;
				};

			case REALM_ZONE: // '목표' 주위
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case EMIMPACT_SIDE_PARTY: // 자기편에게
					if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
						strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_ZONE_OUR" ),
						sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
					break;
				case EMIMPACT_SIDE_ENEMY: // 적에게
					if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
						strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_ZONE_ENEMY" ),
						sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
					break;
				case EMIMPACT_SIDE_ANYBODY: // 모두에게
					if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
						strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_ZONE_ANYBODY" ),
						sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
					break;
				};

			case REALM_FANWIZE:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case EMIMPACT_SIDE_ENEMY:
					if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wAPPLYANGLE && 0 < sDATA_LVL.wTARNUM )
						strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TARGET_FANWIZE_ENEMY" ),
						sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wAPPLYANGLE, sDATA_LVL.wTARNUM );
					break;
				};
			};
		}
		break;

	case TAR_SELF_TOSPEC: // 자신부터 타겟까지
		switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
		{
		case EMIMPACT_SIDE_ENEMY:
			WORD wPierce = sDATA_LVL.wPIERCENUM + pTarget->GETSUM_PIERCE() + 1;

			if ( 0 < sBASIC.wTARRANGE && 0 < sDATA_LVL.wTARNUM && 0 < wPierce )
				strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_TOSPEC_ENEMY" ),
				sBASIC.wTARRANGE * 0.1f, sDATA_LVL.wTARNUM, wPierce );
			break;
		};
		break;

	case TAR_ZONE: // 지면 위치의 주변 타겟
		switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
		{
		case EMIMPACT_SIDE_PARTY:
			if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
				strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_ZONE_OUR" ),
				sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
			break;
		case EMIMPACT_SIDE_ENEMY:
			if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
				strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_ZONE_ENEMY" ),
				sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
			break;
		case EMIMPACT_SIDE_ANYBODY:
			if ( 0 < sDATA_LVL.wAPPLYRANGE && 0 < sDATA_LVL.wTARNUM )
				strText += sc::string::format( ID2GAMEINTEXT( "SKILL_AREA_ZONE_ANYBODY" ),
				sDATA_LVL.wAPPLYRANGE * 0.1f, sDATA_LVL.wTARNUM );
			break;
		};
		break;

	case TAR_LAND: // 지면 위치
		strText += sc::string::format( "%s", ID2GAMEINTEXT( "SKILL_AREA_LAND" ) );
		break;
	};
}

//! 스킬 효과
static void GetSkillEffect( std::string& strText
						   , GLSKILL* pSkill
						   , GLCHARLOGIC* pTarget
						   , UINT skillLevel )
{
	strText = "";

	const SKILL::SAPPLY& sAPPLY = pSkill->m_sAPPLY;
	const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[ skillLevel ];

	SKILL::EMROLE emRole = pSkill->m_sBASIC.emROLE;

	SkillViewDlg::SKILL_INFOR_TYPE emInforType = SkillViewDlg::INFOR_NONE_TYPE;
	if ( emRole == SKILL::EMROLE_NORMAL )
	{
		if ( sDATA_LVL.fLIFE )
		{
			if ( sDATA_LVL.fBASIC_VAR > 0.0f )
				emInforType = SkillViewDlg::NORMAL_DURATION_PLUS;
			else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
				emInforType = SkillViewDlg::NORMAL_DURATION_MINUS;
		}
		else
		{
			if ( sDATA_LVL.fBASIC_VAR > 0.0f )
				emInforType = SkillViewDlg::NORMAL_NONDURATION_PLUS;
			else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
				emInforType = SkillViewDlg::NORMAL_NONDURATION_MINUS;
		}
	}
	else if ( emRole == SKILL::EMROLE_PASSIVE )
	{
		if ( sDATA_LVL.fLIFE )
		{
			if ( sDATA_LVL.fBASIC_VAR > 0.0f )
				emInforType = SkillViewDlg::PASSIVE_DURATION_PLUS;
			else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
				emInforType = SkillViewDlg::PASSIVE_DURATION_MINUS;
		}
		else
		{
			if ( sDATA_LVL.fBASIC_VAR > 0.0f )
				emInforType = SkillViewDlg::PASSIVE_NONDURATION_PLUS;
			else if ( sDATA_LVL.fBASIC_VAR < 0.0f )
				emInforType = SkillViewDlg::PASSIVE_NONDURATION_MINUS;
		}
	}

	int effectCount = 0;
	if( GLSkillToolTip::MakeTooltipSting_In_Skill_Type(
		pTarget, pSkill, skillLevel) != "" )
	{
		++effectCount;
		strText = sc::string::format( "%d. %s\r\n\r\n", effectCount,
			GLSkillToolTip::MakeTooltipSting_In_Skill_Type(
			pTarget, pSkill, skillLevel ) );
	}

	std::vector<std::string> skillImpact =
		GLSkillToolTip::MakeTooltipSting_In_Skill_Impact(
		pSkill, skillLevel );
	BOOST_FOREACH( std::string& addStr, skillImpact )
	{
		++effectCount;
		strText += sc::string::format( "%d. %s", effectCount, addStr.c_str() );
		strText += "\r\n\r\n";
	}

	std::vector<std::string> skillSpecial =
		GLSkillToolTip::MakeTooltipSting_In_Skill_Special(
		pSkill, skillLevel, true );
	BOOST_FOREACH( std::string& addStr, skillSpecial )
	{
		++effectCount;
		strText += sc::string::format( "%d. %s", effectCount, addStr.c_str() );
		strText += "\r\n\r\n";
	}
}

//! 상태이상 효과
static void GetSkillUnusualCondition( std::string& strText
									 , GLSKILL* pSkill
									 , UINT skillLevel )
{
	strText = "";

	std::vector< std::string > stateExplain =
		GLSkillToolTip::MakeTooltipSting_In_Skill_Special(
		pSkill, skillLevel, true );
	
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
		strText += "\r\n\r\n";
	}
}

// Update List
void SkillViewDlg::UpdateList( GLSKILL* pSkill )
{
	// Delete List
	m_ListMainSkill.DeleteAllItems();
	m_ListMultiTargetSkill.DeleteAllItems();

	// Delete Edit
	m_EditMainScope.SetWindowText("");
	m_EditMainEffect.SetWindowText("");
	m_EditMainCondition.SetWindowText("");
	m_EditMultiScope.SetWindowText("");
	m_EditMultiEffect.SetWindowText("");
	m_EditMultiCondition.SetWindowText("");

	// Delete ComboBox
	m_ComboBoxLevel.ResetContent();

	if( !pSkill )
		return;

	// ComboBox Level 초기화
	UINT maxLevel = pSkill->m_sBASIC.dwMAXLEVEL;
	for( UINT i=0; i<maxLevel; ++i )
		m_ComboBoxLevel.AddString( sc::string::format( "%d Level", i+1 ).c_str() );

	// ComboBox 세팅
	m_ComboBoxLevel.SetCurSel( m_skillLevel );

	// Update List
	UpdateMainSkillList( pSkill );
	UpdateMultiTargetSkillList( pSkill->GetSecondSkill() );
}

void SkillViewDlg::UpdateMainSkillList( GLSKILL* pSkill )
{
	static std::string listName[ MAIN_SKILL_COLUMN_COUNT ] = {
		ID2GAMEWORD( "SKILL_BASIC_INFO", 0 ),
		sc::string::format("%s/%s",
		ID2GAMEWORD( "SKILL_BASIC_INFO", 1 ),
		ID2GAMEWORD( "SKILL_BASIC_INFO", 2 ) ),
		ID2GAMEWORD( "SKILL_BASIC_INFO", 5 ),
		ID2GAMEWORD( "SKILL_BASIC_INFO", 9 ),
		ID2GAMEWORD( "SKILL_BASIC_INFO", 10 ),
		ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC", 0 )
	};

	if( !pSkill )
		return;

	// 스킬 이미지 세팅
	GetSkillImage( pSkill, m_MainSkillImage );

	std::string informationText = "";

	// 기본 스킬 정보 세팅
	for( int i=0; i<MAIN_SKILL_COLUMN_COUNT; ++i )
	{
		m_ListMainSkill.InsertItem( i, listName[ i ].c_str() );

		switch( i )
		{
		case MAIN_SKILL_NAME:
			informationText = pSkill->GetName();
			break;
		case MAIN_SKILL_GNA:
			GetGradeAttributeText( informationText, pSkill );
			break;
		case MAIN_SKILL_TYPE:
			GetTypeText( informationText, pSkill );
			break;
		case MAIN_SKILL_WEAPON:
			GetUseWeapon( informationText, pSkill );
			break;
		case MAIN_SKILL_CONSUME:
			GetConsumeCondition( informationText, pSkill, m_skillLevel );
			break;
		case MAIN_SKILL_DELAY:
			GetReuseTime( informationText, pSkill, m_skillLevel );
			break;
		}

		m_ListMainSkill.SetItemText( i, 1, informationText.c_str() );
	}

	// 스킬 설명 세팅
	const char* szDesc = pSkill->GetDesc();
	if( szDesc )
	{
		m_StaticMainExplain.SetWindowText( szDesc );
	}

	// 스킬 영역 세팅
	GetSkillScope( informationText, pSkill, m_pDefaultCharacter, m_skillLevel );
	m_EditMainScope.SetWindowText( informationText.c_str() );

	// 스킬 효과 세팅
	GetSkillEffect( informationText, pSkill, m_pDefaultCharacter, m_skillLevel );
	m_EditMainEffect.SetWindowText( informationText.c_str() );

	// 스킬 상태이상 효과 세팅
	GetSkillUnusualCondition( informationText, pSkill, m_skillLevel );
	m_EditMainCondition.SetWindowText( informationText.c_str() );
}

void SkillViewDlg::UpdateMultiTargetSkillList( GLSKILL* pSkill )
{
	static std::string listName[ MULTI_SKILL_COLUMN_COUNT ] = {
		ID2GAMEWORD( "SKILL_BASIC_INFO", 0 ),
		sc::string::format("%s/%s",
		ID2GAMEWORD( "SKILL_BASIC_INFO", 1 ),
		ID2GAMEWORD( "SKILL_BASIC_INFO", 2 ) ),
		ID2GAMEWORD( "SKILL_BASIC_INFO", 5 )
	};

	// 스킬 이미지 세팅
	GetSkillImage( pSkill, m_MultiSkillImage );

	if( !pSkill )
		return;

	std::string informationText = "";

	// 기본 스킬 정보 세팅
	for( int i=0; i<MULTI_SKILL_COLUMN_COUNT; ++i )
	{
		m_ListMultiTargetSkill.InsertItem( i, listName[ i ].c_str() );

		switch( i )
		{
		case MULTI_SKILL_NAME:
			informationText = pSkill->GetName();
			break;
		case MULTI_SKILL_GNA:
			GetGradeAttributeText( informationText, pSkill );
			break;
		case MULTI_SKILL_TYPE:
			GetTypeText( informationText, pSkill );
			break;
		}

		m_ListMultiTargetSkill.SetItemText( i, 1, informationText.c_str() );
	}

	// 스킬 설명 세팅
	const char* szDesc = pSkill->GetDesc();
	if( szDesc )
	{
		m_StaticMainExplain.SetWindowText( szDesc );
	}

	// 스킬 영역 세팅
	GetSkillScope( informationText, pSkill, m_pDefaultCharacter, m_skillLevel );
	m_EditMultiScope.SetWindowText( informationText.c_str() );

	// 스킬 효과 세팅
	GetSkillEffect( informationText, pSkill, m_pDefaultCharacter, m_skillLevel );
	m_EditMultiEffect.SetWindowText( informationText.c_str() );

	// 스킬 상태이상 효과 세팅
	GetSkillUnusualCondition( informationText, pSkill, m_skillLevel );
	m_EditMultiCondition.SetWindowText( informationText.c_str() );
}

void SkillViewDlg::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize( nType, cx, cy );

	CRect pos;
	m_ListMainSkill,GetWindowRect( pos );
	ScreenToClient( &pos );
	m_ListMainSkill.SetWindowPos( NULL,
		pos.top, pos.left, cx, pos.bottom-pos.top,
		SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER );

	m_ListMultiTargetSkill,GetWindowRect( pos );
	ScreenToClient( &pos );
	m_ListMultiTargetSkill.SetWindowPos( NULL,
		pos.top, pos.left, cx, cy-pos.top,
		SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER );
}

BOOL SkillViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitializeSkillView();

	return TRUE;
}

void SkillViewDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_LIST_MAINSKILL, m_ListMainSkill );
	DDX_Control( pDX, IDC_LIST_MULTI_TARGET_SKILL, m_ListMultiTargetSkill );
	DDX_Control( pDX, IDC_COMBO_LEVEL, m_ComboBoxLevel );
	DDX_Control( pDX, IDC_MAIN_SKILL_EXPLAIN, m_StaticMainExplain );
	DDX_Control( pDX, IDC_MULTI_SKILL_EXPLAIN, m_StaticMultiExplain );
	DDX_Control( pDX, IDC_EDIT_MAIN_SCOPE, m_EditMainScope );
	DDX_Control( pDX, IDC_EDIT_MAIN_EFFECT, m_EditMainEffect );
	DDX_Control( pDX, IDC_EDIT_MAIN_CONDITION, m_EditMainCondition );
	DDX_Control( pDX, IDC_EDIT_MULTI_SCOPE, m_EditMultiScope );
	DDX_Control( pDX, IDC_EDIT_MULTI_EFFECT, m_EditMultiEffect );
	DDX_Control( pDX, IDC_EDIT_MULTI_CONDITION, m_EditMultiCondition );
}

void SkillViewDlg::PostNcDestroy()
{
	KillTimer( 0 );

	m_MainSkillImage->DetachWnd();
	SAFE_DELETE( m_MainSkillImage );

	m_MultiSkillImage->DetachWnd();
	SAFE_DELETE( m_MultiSkillImage );

	GLogicData::GetInstance().ClearData();

	CDialog::PostNcDestroy();
}

void SkillViewDlg::OnCbnSelchangeComboLevel()
{
	UINT level = m_ComboBoxLevel.GetCurSel();

	UINT maxLevel = m_skill.m_sBASIC.dwMAXLEVEL;
	if( level < 0 || level >= maxLevel )
		return;

	SetSkill( &m_skill, level );
}

void SkillViewDlg::OnClose()
{
	this->ShowWindow( SW_HIDE );
}

void SkillViewDlg::OnTimer( UINT_PTR nIDEvent )
{
	switch( nIDEvent )
	{
	case 0:
		CRenderDevice::OnLoop();
		break;
	}
}