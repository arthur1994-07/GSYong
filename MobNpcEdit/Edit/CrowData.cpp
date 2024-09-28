// ItemProperty2.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../MobNpcEdit.h"
#include "../../RanLogic/Crow/GLCrowData.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"

#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"
#include "./SheetWithTab.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"
#include "../../RanLogic/NpcTalk/NpcDialogueSet.h"

#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../RanLogic/GLStringTable.h"
#include "../../RanLogic/Pet/GLPet.h"
#include "../../RanLogic/Summon/GLSummon.h"
#include "../SelectDialog.h"
#include "CrowData.h"

std::string MAXATTACK[ SCROWDATA::EMMAXATTACK + 1 ] =
{
	"공격 세트 00",
	"공격 세트 01",
	"공격 세트 02",
	"공격 세트 03",
	"공격 세트 04",
	"공격 세트 05",
	"공격 세트 06",
	"공격 세트 07",
	"공격 세트 08",
	"공격 세트 09",
	"랜덤공격세트",
};

// CCrowData 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCrowData, CPropertyPage)
CCrowData::CCrowData( LOGFONT logfont )
	: CPropertyPage(CCrowData::IDD)
	, m_pFont ( NULL )
{
	m_bDlgInit = FALSE;
	m_pItem = NULL;
	m_pDummyItem = new SCROWDATA;
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CCrowData::~CCrowData()
{
	SAFE_DELETE ( m_pDummyItem );
	SAFE_DELETE ( m_pFont );
}

void CCrowData::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCrowData, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_SKINFILE, OnBnClickedButtonSkinfile)
	ON_BN_CLICKED(IDC_BUTTON_TALKFILE, OnBnClickedButtonTalkfile)
	ON_BN_CLICKED(IDC_BUTTON_FALLING_EFFECT, OnBnClickedButtonFallingEffect)
	ON_BN_CLICKED(IDC_BUTTON_BLOW_EFFECT, OnBnClickedButtonBlowEffect)
	ON_BN_CLICKED(IDC_BUTTON_ITEMGENFILE, OnBnClickedButtonItemgenfile)
	ON_BN_CLICKED(IDC_BUTTON_BIRTH_EFFECT, OnBnClickedButtonBirthEffect)
	ON_BN_CLICKED(IDC_BUTTON_SALEFILE01, OnBnClickedButtonSalefile01)
	ON_BN_CLICKED(IDC_BUTTON_SALEFILE02, OnBnClickedButtonSalefile02)
	ON_BN_CLICKED(IDC_BUTTON_SALEFILE03, OnBnClickedButtonSalefile03)
	ON_EN_CHANGE(IDC_EDIT_CROWNAME, OnEnChangeEditCrowname)
	ON_BN_CLICKED(IDC_CHECK_AUTODROPOUT, OnBnClickedCheckAutodropout)
	ON_BN_CLICKED(IDC_BUTTON_TRIBE, OnBnClickedButtonTribe)
	ON_BN_CLICKED(IDC_BUTTON_CROW, OnBnClickedButtonCrow)
	ON_BN_CLICKED(IDC_BUTTON_BRIGHT, OnBnClickedButtonBright)
	ON_BN_CLICKED(IDC_BUTTON_PET_TYPE, OnBnClickedButtonPetType)
	ON_BN_CLICKED(IDC_BUTTON_SUMMON_TYPE, OnBnClickedButtonSummonType)
	ON_BN_CLICKED(IDC_BUTTON_NPC_TYPE, OnBnClickedButtonNPCType)
	ON_BN_CLICKED(IDC_BUTTON_HP_DN_ACT, OnBnClickedButtonHpDnAct)
	ON_BN_CLICKED(IDC_BUTTON_HP_DN_ATK_SET, OnBnClickedButtonHpDnAtkSet)
	ON_BN_CLICKED(IDC_BUTTON_HP_LIST_ADD, OnBnClickedButtonHpListAdd)
	ON_BN_CLICKED(IDC_BUTTON_HP_LIST_DEL, OnBnClickedButtonHpListDel)
    ON_BN_CLICKED(IDC_BUTTON_ITEMGENFILE2, &CCrowData::OnBnClickedButtonItemgenfile2)
	ON_BN_CLICKED(IDC_CHECK_BARRIER, &CCrowData::OnBnClickedCheckBarrier)
    ON_BN_CLICKED(IDC_CHECK_INTERACTION_RANGE, &CCrowData::OnBnClickedInteractionRange)
END_MESSAGE_MAP()


// CCrowData 메시지 처리기입니다.

void CCrowData::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pItem = NULL;
	m_pSheetTab->ActiveCrowTreePage ();
}

void CCrowData::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	InverseUpdateItems();
	
	GLCrowDataMan::GetInstance().InsertCrowData ( m_pDummyItem->sNativeID, m_pDummyItem );
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CGameTextMan::GetInstance().InsertString( strKeyName.c_str(), strName.c_str(), CGameTextMan::EM_CROW_TEXT );
	}
	else
	{
		GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::CROW );
	}

	m_pItem = NULL;
	m_pSheetTab->ActiveCrowTreePage ();
}

BOOL CCrowData::SetItem ( SCROWDATA* pItem )
{
	if ( m_pItem )
	{
		return FALSE;
	}

	m_pItem = pItem;

	//	더미로 가져가기
	m_pDummyItem->Assign ( *m_pItem );

	if ( m_bDlgInit ) 
	{
		InitDefaultCtrls ();
		UpdateItems ();		
	}

	return TRUE;
}

void CCrowData::UpdateItems ()
{
	SetWin_Num_int ( this, IDC_EDIT_MID, m_pDummyItem->sNativeID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_SID, m_pDummyItem->sNativeID.wSubID );

	SetWin_Text ( this, IDC_EDIT_CROWNAME, m_pDummyItem->m_szName );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		SetWin_Text( this, IDC_EDIT_TRIBE,	CGameTextMan::GetInstance().GetCommentText("TRIBE", m_pDummyItem->m_emTribe) );
		SetWin_Text( this, IDC_EDIT_CROW,	CGameTextMan::GetInstance().GetCommentText("CROW", m_pDummyItem->m_emCrow) );
		SetWin_Text( this, IDC_EDIT_BRIGHT,	CGameTextMan::GetInstance().GetCommentText("BRIGHT", m_pDummyItem->m_emBright) );

		// PET
		if( m_pDummyItem->m_emPetType != PETTYPE_NONE )
			SetWin_Text( this, IDC_EDIT_PET_TYPE, CGameTextMan::GetInstance().GetText("PET_TYPE", m_pDummyItem->m_emPetType, CGameTextMan::EM_COMMENT_TEXT).GetString());
		else
			SetWin_Text( this, IDC_EDIT_PET_TYPE, "" );	

		// NPC
		SetWin_Text( this, IDC_EDIT_NPC_TYPE, CGameTextMan::GetInstance().GetCommentText("NPC_TYPE", m_pDummyItem->m_emNPCType).GetString() );
		SetWin_Text( this, IDC_EDIT_NPC_TYPE, CGameTextMan::GetInstance().GetCommentText("CROWACT_UP", m_pDummyItem->m_sAction.m_Pattern.m_emActPattern).GetString() );
		SetWin_Combo_Sel ( this, IDC_COMBO_MOVETYPE, CGameTextMan::GetInstance().GetCommentText("MOVETYPE", m_pDummyItem->m_sAction.m_emMoveType).GetString() );

		CListBox* pListBox = NULL;
		pListBox = (CListBox*) GetDlgItem ( IDC_LIST_HP );
		pListBox->ResetContent();
		CString str;
		for(int i = 0; i < ( int ) m_pDummyItem->m_sAction.m_vecPatternList.size(); i++ )
		{
			str.Format ( "%.f,  %s,  %s", m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_fPatternDNRate,
				CGameTextMan::GetInstance().GetCommentText("CROWACT_UP", m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_emActPattern).GetString(),
				MAXATTACK[ m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_dwPatternAttackSet ].c_str() );

			int nIndex = pListBox->AddString ( str );
			pListBox->SetItemData ( nIndex, i );

			//m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_dwPatternAttackSet
		}
	}
	else
	{
		SetWin_Text( this, IDC_EDIT_TRIBE, COMMENT::TRIBE[m_pDummyItem->m_emTribe].c_str() );
		SetWin_Text( this, IDC_EDIT_CROW, COMMENT::CROW[m_pDummyItem->m_emCrow].c_str() );
		SetWin_Text( this, IDC_EDIT_BRIGHT, COMMENT::BRIGHT[m_pDummyItem->m_emBright].c_str() );

		// PET
		if( m_pDummyItem->m_emPetType != PETTYPE_NONE )
			SetWin_Text( this, IDC_EDIT_PET_TYPE, COMMENT::PET_TYPE[m_pDummyItem->m_emPetType].c_str() );	
		else
			SetWin_Text( this, IDC_EDIT_PET_TYPE, "" );	

		// NPC
		SetWin_Text( this, IDC_EDIT_NPC_TYPE, COMMENT::NPC_TYPE[m_pDummyItem->m_emNPCType].c_str() );
		SetWin_Text( this, IDC_EDIT_HP_DN_ACT, COMMENT::szCROWACT_UP[ m_pDummyItem->m_sAction.m_Pattern.m_emActPattern ].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_MOVETYPE, COMMENT::MOVETYPE[m_pDummyItem->m_sAction.m_emMoveType].c_str() );

		CListBox* pListBox = NULL;
		pListBox = (CListBox*) GetDlgItem ( IDC_LIST_HP );
		pListBox->ResetContent();
		CString str;
		for(int i = 0; i < ( int ) m_pDummyItem->m_sAction.m_vecPatternList.size(); i++ )
		{
			str.Format ( "%.f,  %s,  %s", m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_fPatternDNRate,
				COMMENT::szCROWACT_UP[ m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_emActPattern ].c_str(),
				MAXATTACK[ m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_dwPatternAttackSet ].c_str() );

			int nIndex = pListBox->AddString ( str );
			pListBox->SetItemData ( nIndex, i );

			//m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_dwPatternAttackSet
		}
	}

	// SUMMON / SUMMON_TYPE은 따로 데이터로 빠져잇지않다;
	SetWin_Text( this, IDC_EDIT_SUMMON_TYPE, COMMENT::SUMMON_TYPE[m_pDummyItem->m_emSummonType].c_str() );	

	SetWin_Num_int ( this, IDC_EDIT_LEVEL, m_pDummyItem->m_wLevel );
	SetWin_Num_int ( this, IDC_EDIT_GENTIME, m_pDummyItem->m_dwGenTime );

	SetWin_Num_int ( this, IDC_EDIT_HP, m_pDummyItem->m_dwHP );
	SetWin_Num_int ( this, IDC_EDIT_MP, m_pDummyItem->m_wMP );
	SetWin_Num_int ( this, IDC_EDIT_SP, m_pDummyItem->m_wSP );

	SetWin_Num_float ( this, IDC_EDIT_HP_INC, m_pDummyItem->m_fRegenHP );
	SetWin_Num_float ( this, IDC_EDIT_MP_INC, m_pDummyItem->m_fRegenMP );
	SetWin_Num_float ( this, IDC_EDIT_SP_INC, m_pDummyItem->m_fRegenSP );

	SetWin_Num_int ( this, IDC_EDIT_AVOIDRATE, m_pDummyItem->m_wAvoidRate );
	SetWin_Num_int ( this, IDC_EDIT_HITRATE, m_pDummyItem->m_wHitRate );

	SetWin_Num_int ( this, IDC_EDIT_VIEWRANGE, m_pDummyItem->m_wViewRange );
	SetWin_Num_int ( this, IDC_EDIT_DEFENSE, m_pDummyItem->m_wDefense );

	SetWin_Num_int ( this, IDC_EDIT_BODY_RAD, m_pDummyItem->m_sAction.m_wBodyRadius );

	SetWin_Text ( this, IDC_EDIT_SKINFILE, m_pDummyItem->m_sAction.m_strSkinObj.c_str() );
	SetWin_Text ( this, IDC_EDIT_TALKFILE, m_pDummyItem->m_sAction.m_strTalkFile.c_str() );

	SetWin_Check ( this, IDC_CHECK_INVISIBLE, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_INVISIBLE );
	SetWin_Check ( this, IDC_CHECK_RECVISIBLE, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_RECVISIBLE );

	SetWin_Check ( this, IDC_CHECK_POSHOLD, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_POSHOLD );
	SetWin_Check ( this, IDC_CHECK_DIRHOLD, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_DIRHOLD );
	SetWin_Check ( this, IDC_CHECK_BARRIER, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_BARRIER );

	SetWin_Check ( this, IDC_CHECK_BOSS, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_BOSS );
	SetWin_Check ( this, IDC_CHECK_KNOCK, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_KNOCK );
	SetWin_Check ( this, IDC_CHECK_CD, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_CDCERTIFY );
	SetWin_Check ( this, IDC_CHECK_IGNORE_SHOCK, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_IGNORE_SHOCK );
	SetWin_Check ( this, IDC_CHECK_AUTODROPOUT, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_AUTODROP );

	SetWin_Check ( this, IDC_OVERLAP_CHECK, m_pDummyItem->m_bOverlapAttack );
	SetWin_Check ( this, IDC_NONDAMAGED_CHECK, m_pDummyItem->m_bNonDamaged );

    SetWin_Check ( this, IDC_HIDENAMING_CHECK, m_pDummyItem->m_bHideNaming );
    SetWin_Check ( this, IDC_NONTARGETING_CHECK, m_pDummyItem->m_bNonTargeting );
	SetWin_Check ( this, IDC_CHECK_RANDOMREGEN, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_RANDOMREGEN );

    SetWin_Num_float ( this, IDC_EDIT_SCALE, m_pDummyItem->m_fScale );

	SetWin_Check ( this, IDC_HIDE_TOOLTIP_CHECK, m_pDummyItem->m_sBasic.m_bHideTooltip );

	SetWin_Check ( this, IDC_CHECK_NOPENALTY, m_pDummyItem->m_bNoAllPenalty );
	SetWin_Check ( this, IDC_CHECK_NOPENALTY_NORMAL, m_pDummyItem->m_dwNoPenaltyFlag&EMCROWNOPENALTY_NORMAL );
	SetWin_Check ( this, IDC_CHECK_NOPENALTY_DEBUFF, m_pDummyItem->m_dwNoPenaltyFlag&EMCROWNOPENALTY_DEBUFF );
	SetWin_Check ( this, IDC_CHECK_NOPENALTY_LIMIT,  m_pDummyItem->m_dwNoPenaltyFlag&EMCROWNOPENALTY_LIMIT  );
	SetWin_Check ( this, IDC_CHECK_NOPENALTY_STATE,  m_pDummyItem->m_dwNoPenaltyFlag&EMCROWNOPENALTY_STATE  );

	if ( GetWin_Check ( this, IDC_CHECK_AUTODROPOUT ) ) GetDlgItem ( IDC_EDIT_AUTODROPOUT )->EnableWindow ();

	SetWin_Num_int ( this, IDC_EDIT_SPIRIT, m_pDummyItem->m_sResist.nSpirit );
	SetWin_Num_int ( this, IDC_EDIT_FIRE, m_pDummyItem->m_sResist.nFire );
	SetWin_Num_int ( this, IDC_EDIT_ICE, m_pDummyItem->m_sResist.nIce );
	SetWin_Num_int ( this, IDC_EDIT_ELECTRIC, m_pDummyItem->m_sResist.nElectric );
	SetWin_Num_int ( this, IDC_EDIT_POISON, m_pDummyItem->m_sResist.nPoison );
	SetWin_Num_float ( this, IDC_EDIT_DRIFF_HEIGHT, m_pDummyItem->m_sAction.m_fDriftHeight );
	
	SetWin_Num_int ( this, IDC_EDIT_BONUSEXP, m_pDummyItem->m_wBonusExp );
	SetWin_Num_int ( this, IDC_EDIT_BONUSCP, m_pDummyItem->m_wBonusCP );

	SetWin_Num_float ( this, IDC_EDIT_WALKVELO, m_pDummyItem->m_sAction.m_fWalkVelo );
	
	SetWin_Check ( this, IDC_CHECK_IS_RUN, m_pDummyItem->m_sAction.m_bRun );
	SetWin_Num_float ( this, IDC_EDIT_RUNVELO, m_pDummyItem->m_sAction.m_fRunVelo );
	SetWin_Num_float ( this, IDC_EDIT_AUTODROPOUT, m_pDummyItem->m_sAction.m_fLiveTime );

	SetWin_Num_int ( this, IDC_EDIT_GEN_MONEY, m_pDummyItem->m_sGenerate.m_dwGenMoney );
	SetWin_Num_int ( this, IDC_EDIT_GEN_MONEY_RATE, m_pDummyItem->m_sGenerate.m_wGenMoney_Rate );

    std::string strGenItem = "";
    BOOST_FOREACH( const std::string& strIter, m_pDummyItem->m_sGenerate.m_vecGeneralGenItem )
    {
        if( strIter.empty() )
            continue;
        strGenItem += strIter;
        strGenItem += "; ";
    }
	SetWin_Text ( this, IDC_EDIT_ITEMGENFILE, strGenItem.c_str() );

    std::string strSpecialGenItem = "";

    BOOST_FOREACH( const std::string& strIter, m_pDummyItem->m_sGenerate.m_vecSpecialGenItem )
    {
        if( strIter.empty() )
            continue;
        strSpecialGenItem += strIter;
        strSpecialGenItem += "; ";
    }
	SetWin_Text ( this, IDC_EDIT_SPECIALITEMGENFILE, strSpecialGenItem.c_str() );

	SetWin_Num_int ( this, IDC_EDIT_GEN_ITEM_MID, m_pDummyItem->m_sGenerate.m_sGenItemID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_GEN_ITEM_SID, m_pDummyItem->m_sGenerate.m_sGenItemID.wSubID );

	SetWin_Num_int ( this, IDC_EDIT_GEN_ITEM_RATE, m_pDummyItem->m_sGenerate.m_wGenItem_Rate );

	SetWin_Check ( this, IDC_CHECK_NO_FALL_AFTER, m_pDummyItem->m_sAction.m_bAfterFall_NoBody );
	SetWin_Text ( this, IDC_EDIT_FALLING_EFFECT, m_pDummyItem->m_sAction.m_strFallingEffect.c_str() );
	SetWin_Text ( this, IDC_EDIT_BLOW_EFFECT, m_pDummyItem->m_sAction.m_strBlowEffect.c_str() );
	SetWin_Text ( this, IDC_EDIT_BIRTH_EFFECT, m_pDummyItem->m_sAction.m_strBirthEffect.c_str() );

	SetWin_Text ( this, IDC_EDIT_SALEFILE01, m_pDummyItem->m_sAction.m_strSaleFile[0].c_str() );
	SetWin_Text ( this, IDC_EDIT_SALEFILE02, m_pDummyItem->m_sAction.m_strSaleFile[1].c_str() );
	SetWin_Text ( this, IDC_EDIT_SALEFILE03, m_pDummyItem->m_sAction.m_strSaleFile[2].c_str() );


	SetWin_Check ( this, IDC_CHECK_TARSHORT, m_pDummyItem->m_sAction.m_dwActFlag&EMCROWACT_TARSHORT );
	
	SetWin_Text( this, IDC_EDIT_HP_DN_ATK_SET, MAXATTACK[ m_pDummyItem->m_sAction.m_Pattern.m_dwPatternAttackSet ].c_str() );
	
	SetWin_Num_float( this, IDC_EDIT_ACTION_DN_RATE, m_pDummyItem->m_sAction.m_Pattern.m_fPatternDNRate );

	SetWin_Num_float ( this, IDC_EDIT_ACTION_DN_RATE, m_pDummyItem->m_sAction.m_Pattern.m_fPatternDNRate );

	SetWin_Check ( this, IDC_CHECK_MOB_LINK, m_pDummyItem->m_sAction.m_bMobLink );
	SetWin_Num_int ( this, IDC_EDIT_MOB_LINK_MID, m_pDummyItem->m_sAction.m_sMobLinkID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_MOB_LINK_SID, m_pDummyItem->m_sAction.m_sMobLinkID.wSubID );
	SetWin_Num_float(  this, IDC_EDIT_MOB_LINK_SCALE, m_pDummyItem->m_sAction.m_fMobLinkScale );
	SetWin_Num_float(  this, IDC_EDIT_MOB_LINK_DELAY, m_pDummyItem->m_sAction.m_fMobLinkDelay );

	SetWin_Num_int(  this, IDC_EDIT_GATHER_TIME_LOW, m_pDummyItem->m_wGatherTimeLow );
	SetWin_Num_int(  this, IDC_EDIT_GATHER_TIME_HIGH, m_pDummyItem->m_wGatherTimeHigh );
	SetWin_Num_float(  this, IDC_EDIT_GATHER_RATE, m_pDummyItem->m_fGatherRate );
	SetWin_Num_int(  this, IDC_EDIT_GATHER_ANIM, m_pDummyItem->m_wGatherAnimation );
	SetWin_Num_int ( this, IDC_EDIT_RESISTANCE_PROBABILITY, m_pDummyItem->m_wResist_Probability );

	if ( GetWin_Check ( this, IDC_CHECK_BARRIER ) ) 
	{
		GetDlgItem ( IDC_EDIT_BARRIER_RADIUS )->EnableWindow ();
		GetDlgItem ( IDC_EDIT_BARRIER_WIDTH )->EnableWindow ();
	}
	SetWin_Num_float(  this, IDC_EDIT_BARRIER_RADIUS, m_pDummyItem->m_fBarrierRadius );
	SetWin_Num_float(  this, IDC_EDIT_BARRIER_WIDTH, m_pDummyItem->m_fBarrierWidth );

    const bool bInteractionRange(m_pDummyItem->m_fRefenceActionableRange >= 0.0f);
    SetWin_Check(this, IDC_CHECK_INTERACTION_RANGE, bInteractionRange);
    GetDlgItem(IDC_EDIT_INTERACTION_RANGE)->EnableWindow(bInteractionRange);
    bInteractionRange ? SetWin_Num_float(this, IDC_EDIT_INTERACTION_RANGE, m_pDummyItem->m_fRefenceActionableRange) : SetWin_Text(this, IDC_EDIT_INTERACTION_RANGE, "");
}

BOOL CCrowData::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	m_bDlgInit = TRUE;

	InitDefaultCtrls ();
	UpdateItems ();

	SetReadMode ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CCrowData::InverseUpdateItems ()
{
	m_pDummyItem->sNativeID.wMainID = GetWin_Num_int ( this, IDC_EDIT_MID );
	m_pDummyItem->sNativeID.wSubID = GetWin_Num_int ( this, IDC_EDIT_SID );

	strKeyName = GetWin_Text ( this, IDC_EDIT_CROWNAME ).GetString();
	strName	= GetWin_Text ( this, IDC_EDIT_NAME ).GetString();

	// PET
	m_pDummyItem->m_wLevel = GetWin_Num_int ( this, IDC_EDIT_LEVEL );
	m_pDummyItem->m_dwGenTime = GetWin_Num_int ( this, IDC_EDIT_GENTIME );

	m_pDummyItem->m_dwHP = GetWin_Num_int ( this, IDC_EDIT_HP );
	m_pDummyItem->m_wMP = GetWin_Num_int ( this, IDC_EDIT_MP );
	m_pDummyItem->m_wSP = GetWin_Num_int ( this, IDC_EDIT_SP );

	m_pDummyItem->m_fRegenHP = GetWin_Num_float ( this, IDC_EDIT_HP_INC );
	m_pDummyItem->m_fRegenMP = GetWin_Num_float ( this, IDC_EDIT_MP_INC );
	m_pDummyItem->m_fRegenSP = GetWin_Num_float ( this, IDC_EDIT_SP_INC );

	m_pDummyItem->m_wAvoidRate = GetWin_Num_int ( this, IDC_EDIT_AVOIDRATE );
	m_pDummyItem->m_wHitRate = GetWin_Num_int ( this, IDC_EDIT_HITRATE );

	m_pDummyItem->m_wViewRange = GetWin_Num_int ( this, IDC_EDIT_VIEWRANGE );
	m_pDummyItem->m_wDefense = GetWin_Num_int ( this, IDC_EDIT_DEFENSE );

	m_pDummyItem->m_sAction.m_wBodyRadius = GetWin_Num_int ( this, IDC_EDIT_BODY_RAD );

	m_pDummyItem->m_sAction.m_strSkinObj = GetWin_Text ( this, IDC_EDIT_SKINFILE );
	m_pDummyItem->m_sAction.m_strTalkFile = GetWin_Text ( this, IDC_EDIT_TALKFILE );

	m_pDummyItem->m_bOverlapAttack = GetWin_Check ( this, IDC_OVERLAP_CHECK );
	m_pDummyItem->m_bNonDamaged    = GetWin_Check ( this, IDC_NONDAMAGED_CHECK );
	m_pDummyItem->m_bNoAllPenalty  = GetWin_Check ( this, IDC_CHECK_NOPENALTY );

    m_pDummyItem->m_bHideNaming    = GetWin_Check ( this, IDC_HIDENAMING_CHECK );
    m_pDummyItem->m_bNonTargeting  = GetWin_Check ( this, IDC_NONTARGETING_CHECK );

    m_pDummyItem->m_fScale  = GetWin_Num_float ( this, IDC_EDIT_SCALE );

	m_pDummyItem->m_sBasic.m_bHideTooltip = GetWin_Check( this, IDC_HIDE_TOOLTIP_CHECK );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_NOPENALTY_NORMAL ), m_pDummyItem->m_dwNoPenaltyFlag, EMCROWNOPENALTY_NORMAL );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_NOPENALTY_DEBUFF ), m_pDummyItem->m_dwNoPenaltyFlag, EMCROWNOPENALTY_DEBUFF );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_NOPENALTY_LIMIT ),  m_pDummyItem->m_dwNoPenaltyFlag, EMCROWNOPENALTY_LIMIT  );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_NOPENALTY_STATE ),  m_pDummyItem->m_dwNoPenaltyFlag, EMCROWNOPENALTY_STATE  );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_INVISIBLE ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_INVISIBLE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_RECVISIBLE ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_RECVISIBLE );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_POSHOLD ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_POSHOLD );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_DIRHOLD ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_DIRHOLD );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_BARRIER ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_BARRIER );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_BOSS ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_BOSS );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_KNOCK ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_KNOCK );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CD ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_CDCERTIFY );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_IGNORE_SHOCK ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_IGNORE_SHOCK );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_AUTODROPOUT ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_AUTODROP );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_RANDOMREGEN ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_RANDOMREGEN );

	m_pDummyItem->m_sAction.m_strTalkFile = GetWin_Text ( this, IDC_EDIT_TALKFILE );

	m_pDummyItem->m_sResist.nSpirit = GetWin_Num_int ( this, IDC_EDIT_SPIRIT );
	m_pDummyItem->m_sResist.nFire = GetWin_Num_int ( this, IDC_EDIT_FIRE );
	m_pDummyItem->m_sResist.nIce = GetWin_Num_int ( this, IDC_EDIT_ICE );
	m_pDummyItem->m_sResist.nElectric = GetWin_Num_int ( this, IDC_EDIT_ELECTRIC );
	m_pDummyItem->m_sResist.nPoison = GetWin_Num_int ( this, IDC_EDIT_POISON );

	m_pDummyItem->m_sAction.m_emMoveType = static_cast<EMMOVETYPE> ( GetWin_Combo_Sel ( this, IDC_COMBO_MOVETYPE ) );

	m_pDummyItem->m_sAction.m_fDriftHeight = GetWin_Num_float ( this, IDC_EDIT_DRIFF_HEIGHT );
	
	m_pDummyItem->m_wBonusExp = GetWin_Num_int ( this, IDC_EDIT_BONUSEXP );
	m_pDummyItem->m_wBonusCP = GetWin_Num_int ( this, IDC_EDIT_BONUSCP );

	m_pDummyItem->m_sAction.m_fWalkVelo = GetWin_Num_float ( this, IDC_EDIT_WALKVELO );
	
	m_pDummyItem->m_sAction.m_bRun = GetWin_Check ( this, IDC_CHECK_IS_RUN );
	m_pDummyItem->m_sAction.m_fRunVelo = GetWin_Num_float ( this, IDC_EDIT_RUNVELO );
	m_pDummyItem->m_sAction.m_fLiveTime = GetWin_Num_float ( this, IDC_EDIT_AUTODROPOUT );

	m_pDummyItem->m_sAction.m_Pattern.m_dwPatternAttackSet = 0;
	m_pDummyItem->m_sAction.m_Pattern.m_emActPattern = EMCROWACT_UP_IDLE;
	m_pDummyItem->m_sAction.m_Pattern.m_fPatternDNRate = EMCROWACT_UP_IDLE;

	m_pDummyItem->m_sAction.m_bMobLink = GetWin_Check ( this, IDC_CHECK_MOB_LINK );
	m_pDummyItem->m_sAction.m_sMobLinkID.wMainID = GetWin_Num_int ( this, IDC_EDIT_MOB_LINK_MID );
	m_pDummyItem->m_sAction.m_sMobLinkID.wSubID = GetWin_Num_int ( this, IDC_EDIT_MOB_LINK_SID );
	m_pDummyItem->m_sAction.m_fMobLinkScale = GetWin_Num_float( this, IDC_EDIT_MOB_LINK_SCALE );
	m_pDummyItem->m_sAction.m_fMobLinkDelay = GetWin_Num_float( this, IDC_EDIT_MOB_LINK_DELAY );

	m_pDummyItem->m_sGenerate.m_dwGenMoney = GetWin_Num_int ( this, IDC_EDIT_GEN_MONEY );
	m_pDummyItem->m_sGenerate.m_wGenMoney_Rate = GetWin_Num_int ( this, IDC_EDIT_GEN_MONEY_RATE );

	m_pDummyItem->m_sGenerate.m_sGenItemID.wMainID = GetWin_Num_int ( this, IDC_EDIT_GEN_ITEM_MID );
	m_pDummyItem->m_sGenerate.m_sGenItemID.wSubID = GetWin_Num_int ( this, IDC_EDIT_GEN_ITEM_SID );


    std::string strTempGeneralGenItem = GetWin_Text ( this, IDC_EDIT_ITEMGENFILE );
    std::string strTempSpecialGenItem = GetWin_Text ( this, IDC_EDIT_SPECIALITEMGENFILE );

    boost::split(m_pDummyItem->m_sGenerate.m_vecGeneralGenItem, strTempGeneralGenItem, boost::algorithm::is_any_of(",; "), boost::algorithm::token_compress_on);
    boost::split(m_pDummyItem->m_sGenerate.m_vecSpecialGenItem, strTempSpecialGenItem, boost::algorithm::is_any_of(",; "), boost::algorithm::token_compress_on);

	//m_pDummyItem->m_sGenerate.m_strGenItem = GetWin_Text ( this, IDC_EDIT_ITEMGENFILE );
	//m_pDummyItem->m_sGenerate.m_strSpecialGenItem = GetWin_Text ( this, IDC_EDIT_SPECIALITEMGENFILE );

	m_pDummyItem->m_sGenerate.m_wGenItem_Rate = GetWin_Num_int ( this, IDC_EDIT_GEN_ITEM_RATE );

	m_pDummyItem->m_sAction.m_bAfterFall_NoBody = GetWin_Check ( this, IDC_CHECK_NO_FALL_AFTER );
	m_pDummyItem->m_sAction.m_strFallingEffect = GetWin_Text ( this, IDC_EDIT_FALLING_EFFECT );
	m_pDummyItem->m_sAction.m_strBlowEffect = GetWin_Text ( this, IDC_EDIT_BLOW_EFFECT );
	m_pDummyItem->m_sAction.m_strBirthEffect = GetWin_Text ( this, IDC_EDIT_BIRTH_EFFECT );

	m_pDummyItem->m_sAction.m_strSaleFile[0] = GetWin_Text ( this, IDC_EDIT_SALEFILE01 );
	m_pDummyItem->m_sAction.m_strSaleFile[1] = GetWin_Text ( this, IDC_EDIT_SALEFILE02 );
	m_pDummyItem->m_sAction.m_strSaleFile[2] = GetWin_Text ( this, IDC_EDIT_SALEFILE03 );

	m_pDummyItem->m_wGatherTimeLow = GetWin_Num_int( this, IDC_EDIT_GATHER_TIME_LOW );
	m_pDummyItem->m_wGatherTimeHigh = GetWin_Num_int( this, IDC_EDIT_GATHER_TIME_HIGH );
	m_pDummyItem->m_fGatherRate = GetWin_Num_int( this, IDC_EDIT_GATHER_RATE );
	m_pDummyItem->m_wGatherAnimation = GetWin_Num_int( this, IDC_EDIT_GATHER_ANIM );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TARSHORT ), m_pDummyItem->m_sAction.m_dwActFlag, EMCROWACT_TARSHORT );
	m_pDummyItem->m_wResist_Probability = GetWin_Num_int( this, IDC_EDIT_RESISTANCE_PROBABILITY );
	m_pDummyItem->m_fBarrierRadius = GetWin_Num_float(  this, IDC_EDIT_BARRIER_RADIUS );
	m_pDummyItem->m_fBarrierWidth = GetWin_Num_float(  this, IDC_EDIT_BARRIER_WIDTH );
    m_pDummyItem->m_fRefenceActionableRange = GetWin_Check(this, IDC_CHECK_INTERACTION_RANGE) ? GetWin_Num_float(this, IDC_EDIT_INTERACTION_RANGE) : -1.0f;
}

void CCrowData::OnBnClickedButtonNext()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateItems();

	GLCrowDataMan::GetInstance().InsertCrowData ( m_pDummyItem->sNativeID, m_pDummyItem );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CGameTextMan::GetInstance().InsertString( strKeyName.c_str(), strName.c_str(), CGameTextMan::EM_CROW_TEXT );
	}
	else
	{
		GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::CROW );
	}

	m_pSheetTab->ActiveCrowAttackTreePage ( m_CallPage, m_pItem );

	m_pItem = NULL;
}

void CCrowData::InitDefaultCtrls ()
{
	// PET
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strMOVETYPE[MOVETYPE_NSIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("MOVETYPE", strMOVETYPE, CGameTextMan::EM_COMMENT_TEXT,MOVETYPE_NSIZE);
		SetWin_Combo_Init ( this, IDC_COMBO_MOVETYPE, strMOVETYPE, MOVETYPE_NSIZE );
	}
	else
	{
		SetWin_Combo_Init ( this, IDC_COMBO_MOVETYPE, COMMENT::MOVETYPE, MOVETYPE_NSIZE );
	}
	GetDlgItem ( IDC_BUTTON_PREV )->EnableWindow ( FALSE );
	GetDlgItem ( IDC_EDIT_AUTODROPOUT )->EnableWindow ( FALSE );
}

void CCrowData::OnBnClickedButtonSkinfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Skin File (*.chf)|*.chf|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".chf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CCrowData*)this);

	// NEED : 초기화 안되어 있음.
	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_SKINFILE, dlg.GetFileName().GetString() );
	}
}

void CCrowData::OnBnClickedButtonTalkfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Conversation File (*.ntk)|*.ntk|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".ntk",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CCrowData*)this);

	dlg.m_ofn.lpstrInitialDir = CNpcDialogueSet::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_TALKFILE, dlg.GetFileName().GetString() );
	}
}

void CCrowData::OnBnClickedButtonFallingEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_FALLING_EFFECT, dlg.GetFileName().GetString() );
	}
}

void CCrowData::OnBnClickedButtonBlowEffect()
{
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_BLOW_EFFECT, dlg.GetFileName().GetString() );
	}
}

void CCrowData::InitAllItem ()
{
	m_pItem = NULL;
}

void CCrowData::OnBnClickedButtonItemgenfile()
{
	CString szFilter = "Item Sale File (*.genitem)|*.genitem|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg ( TRUE, ".genitem", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CCrowData*)this );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		//SetWin_Text ( this, IDC_EDIT_ITEMGENFILE, dlg.GetFileName().GetString() );
        AddWin_Text_Semicolon( this, IDC_EDIT_ITEMGENFILE, dlg.GetFileName().GetString() );
	}
}
void CCrowData::OnBnClickedButtonItemgenfile2()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CString szFilter = "Item Sale File (*.genitem)|*.genitem|";

    //	Note : 파일 오픈 다이알로그를 만듬.
    CFileDialog dlg ( TRUE, ".genitem", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
        (CCrowData*)this );

    dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
    if ( dlg.DoModal() == IDOK )
    {
        //SetWin_Text ( this, IDC_EDIT_ITEMGENFILE, dlg.GetFileName().GetString() );
        AddWin_Text_Semicolon( this, IDC_EDIT_SPECIALITEMGENFILE, dlg.GetFileName().GetString() );
    }
}



void CCrowData::OnBnClickedButtonBirthEffect()
{
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_BIRTH_EFFECT, dlg.GetFileName().GetString() );
	}
}

void CCrowData::OnBnClickedButtonSalefile01()
{
	CString szFilter = "Item Sale File (*.crowsale)|*.crowsale|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".crowsale",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CCrowData*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_SALEFILE01, dlg.GetFileName().GetString() );
	}
}

void CCrowData::OnBnClickedButtonSalefile02()
{
	CString szFilter = "Item Sale File (*.crowsale)|*.crowsale|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".crowsale",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CCrowData*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_SALEFILE02, dlg.GetFileName().GetString() );
	}
}

void CCrowData::OnBnClickedButtonSalefile03()
{
	CString szFilter = "Item Sale File (*.crowsale)|*.crowsale|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".crowsale",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CCrowData*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_SALEFILE03, dlg.GetFileName().GetString() );
	}
}

void CCrowData::OnEnChangeEditCrowname()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	const char * szTemp = GLStringTable::GetInstance().GetString(
//         GetWin_Text(this, IDC_EDIT_CROWNAME).GetString(),
//         GLStringTable::CROW);
// 	if( szTemp )
// 	{
// 		SetWin_Text ( this, IDC_EDIT_NAME, szTemp );
// 	}
// 	else
// 	{
// 		SetWin_Text ( this, IDC_EDIT_NAME, "" );
// 	}

	char* szTemp = NULL;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		szTemp = const_cast< char* >( CGameTextMan::GetInstance().GetText( GetWin_Text(this, IDC_EDIT_CROWNAME).GetString(), 0, CGameTextMan::EM_CROW_TEXT ).GetString() );
	}
	else
	{
		szTemp = const_cast< char* >( GLStringTable::GetInstance().GetString(
			GetWin_Text(this, IDC_EDIT_CROWNAME).GetString(),
			GLStringTable::CROW) );
	}

	SetWin_Text( this, IDC_EDIT_NAME, szTemp ? szTemp : "" );
}


void CCrowData::OnBnClickedCheckAutodropout()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( GetWin_Check ( this, IDC_CHECK_AUTODROPOUT ) ) 
	{
        GetDlgItem ( IDC_EDIT_AUTODROPOUT )->EnableWindow ();
	}
	else
	{
		GetDlgItem ( IDC_EDIT_AUTODROPOUT )->EnableWindow ( FALSE );
	}
}

void CCrowData::SetReadMode ()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	const int nSkipNum = 5;
	const int nSkipID[nSkipNum] = { IDC_EDIT_NAME, IDC_BUTTON_PREV, IDC_BUTTON_NEXT, 
									IDC_BUTTON_CANCEL, IDC_BUTTON_OK };

	int nID = 0;
	bool bOK = false;

	CWnd* pChildWnd = GetWindow(GW_CHILD);

	while (pChildWnd)
	{
		bOK = false;
		nID = pChildWnd->GetDlgCtrlID();	

		for ( int i = 0; i < nSkipNum; ++i )
		{
			if ( nID == nSkipID[i] )
			{
				bOK = true;	
				break;
			}
		}

		if ( !bOK )	pChildWnd->EnableWindow( FALSE );

		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
#endif

}

void CCrowData::OnBnClickedButtonTribe()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strTRIBE[TRIBE_NSIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("TRIBE", strTRIBE, CGameTextMan::EM_COMMENT_TEXT, TRIBE_NSIZE);
		dlg.SetStringList(strTRIBE, TRIBE_NSIZE);
		dlg.m_nSelect = m_pDummyItem->m_emTribe;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->m_emTribe = (EMTRIBE) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_TRIBE, strTRIBE[m_pDummyItem->m_emTribe].c_str() );
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::TRIBE, TRIBE_NSIZE );	
		dlg.m_nSelect = m_pDummyItem->m_emTribe;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->m_emTribe = (EMTRIBE) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_TRIBE, COMMENT::TRIBE[m_pDummyItem->m_emTribe].c_str() );
		}
	}
}

void CCrowData::OnBnClickedButtonCrow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strCROW[CROW_NUM];
		CGameTextMan::GetInstance().LoadHelpCommentString("CROW", strCROW, CGameTextMan::EM_COMMENT_TEXT, CROW_NUM);
		dlg.SetStringList( strCROW, CROW_NUM );	
		dlg.m_nSelect = m_pDummyItem->m_emCrow;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->m_emCrow = (EMCROW) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_CROW, strCROW[m_pDummyItem->m_emCrow].c_str() );
		}

	}
	else
	{
		dlg.SetStringList( COMMENT::CROW, CROW_NUM );	
		dlg.m_nSelect = m_pDummyItem->m_emCrow;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->m_emCrow = (EMCROW) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_CROW, COMMENT::CROW[m_pDummyItem->m_emCrow].c_str() );
		}
	}
}

void CCrowData::OnBnClickedButtonBright()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strBRIGHT[BRIGHT_SIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("BRIGHT", strBRIGHT, CGameTextMan::EM_COMMENT_TEXT, BRIGHT_SIZE);
		dlg.SetStringList( strBRIGHT, BRIGHT_SIZE );	
		dlg.m_nSelect = m_pDummyItem->m_emBright;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->m_emBright = (EMBRIGHT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_BRIGHT, strBRIGHT[m_pDummyItem->m_emBright].c_str() );
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::BRIGHT, BRIGHT_SIZE );	
		dlg.m_nSelect = m_pDummyItem->m_emBright;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->m_emBright = (EMBRIGHT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_BRIGHT, COMMENT::BRIGHT[m_pDummyItem->m_emBright].c_str() );
		}
	}
}

void CCrowData::OnBnClickedButtonPetType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	// ( 2014.11.27 ) 펫 타입이 루아스크립트와 MobNPCEdit에서 둘다 설정하다가 실수로 다르게 입력하는 경우가 발생되어서 루아 스크립트에서만 입력된 값으로 사용하고 툴에서는 사용못하도록 막음 

	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string>	vecPET_TYPE;
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("PET_TYPE", vecPET_TYPE, CGameTextMan::EM_COMMENT_TEXT);
		const INT iPET_TYPESIZE = vecPET_TYPE.size();
		std::string* pStrPET_TYPE = new std::string[iPET_TYPESIZE];
		for ( INT i = 0; i < iPET_TYPESIZE; ++i )		
		{
			pStrPET_TYPE[i] = vecPET_TYPE[i];
		}
		dlg.SetStringList( pStrPET_TYPE, COMMENT::PET_DEFINE.size() );	
		dlg.m_nSelect = m_pDummyItem->m_emPetType;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->m_emPetType = (int) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_PET_TYPE, pStrPET_TYPE[m_pDummyItem->m_emPetType].c_str() );	
		}
		delete []pStrPET_TYPE;
	}
	else
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			std::vector<std::string>	vecPET_TYPE;
			CGameTextMan::GetInstance().LoadHelpUnknownCommentString("PET_TYPE", vecPET_TYPE, CGameTextMan::EM_COMMENT_TEXT);
			dlg.SetStringList( &vecPET_TYPE[0], COMMENT::PET_DEFINE.size() );
			dlg.m_nSelect = m_pDummyItem->m_emPetType;
			if ( dlg.DoModal() == IDOK )
			{
				m_pDummyItem->m_emPetType = (int) dlg.m_dwData;
				SetWin_Text( this, IDC_EDIT_PET_TYPE, vecPET_TYPE[m_pDummyItem->m_emPetType].c_str() );	
			}
		}
		else
		{
			dlg.SetStringList( &COMMENT::PET_TYPE[0], COMMENT::PET_DEFINE.size() );	

			dlg.m_nSelect = m_pDummyItem->m_emPetType;
			if ( dlg.DoModal() == IDOK )
			{
				m_pDummyItem->m_emPetType = (int) dlg.m_dwData;
				SetWin_Text( this, IDC_EDIT_PET_TYPE, COMMENT::PET_TYPE[m_pDummyItem->m_emPetType].c_str() );	
			}
		}
	}
}

void CCrowData::OnBnClickedButtonSummonType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	dlg.SetStringList( COMMENT::SUMMON_TYPE, SUMMON_TYPE_SIZE );	
	dlg.m_nSelect = m_pDummyItem->m_emSummonType;

	if ( dlg.DoModal() == IDOK )
	{
		m_pDummyItem->m_emSummonType = (SUMMON_TYPE) dlg.m_dwData;
		SetWin_Text( this, IDC_EDIT_SUMMON_TYPE, COMMENT::SUMMON_TYPE[m_pDummyItem->m_emSummonType].c_str() );	
	}
}

void CCrowData::OnBnClickedButtonNPCType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strNPC_TYPE[NPC_TYPE_SIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("NPC_TYPE", strNPC_TYPE, CGameTextMan::EM_COMMENT_TEXT, NPC_TYPE_SIZE);
		dlg.SetStringList( strNPC_TYPE, NPC_TYPE_SIZE );	
		dlg.m_nSelect = m_pDummyItem->m_emNPCType;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->m_emNPCType = (NPC_TYPE) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_NPC_TYPE, strNPC_TYPE[m_pDummyItem->m_emNPCType].c_str() );	
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::NPC_TYPE, NPC_TYPE_SIZE );	
		dlg.m_nSelect = m_pDummyItem->m_emNPCType;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->m_emNPCType = (NPC_TYPE) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_NPC_TYPE, COMMENT::NPC_TYPE[m_pDummyItem->m_emNPCType].c_str() );	
		}
	}
}

void CCrowData::OnBnClickedButtonHpDnAct()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strCROWACT_UP[EMCROWACT_UP_NSIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("CROWACT_UP", strCROWACT_UP, CGameTextMan::EM_COMMENT_TEXT, EMCROWACT_UP_NSIZE);
		dlg.SetStringList( strCROWACT_UP, EMCROWACT_UP_NSIZE );	
		dlg.m_nSelect = m_pDummyItem->m_sAction.m_Pattern.m_emActPattern;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->m_sAction.m_Pattern.m_emActPattern = (EMCROWACT_UP) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_HP_DN_ACT, strCROWACT_UP[ m_pDummyItem->m_sAction.m_Pattern.m_emActPattern ].c_str() );		
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::szCROWACT_UP, EMCROWACT_UP_NSIZE );	
		dlg.m_nSelect = m_pDummyItem->m_sAction.m_Pattern.m_emActPattern;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->m_sAction.m_Pattern.m_emActPattern = (EMCROWACT_UP) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_HP_DN_ACT, COMMENT::szCROWACT_UP[ m_pDummyItem->m_sAction.m_Pattern.m_emActPattern ].c_str() );		
		}
	}
}

void CCrowData::OnBnClickedButtonHpDnAtkSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CSelectDialog dlg;

	dlg.SetStringList( MAXATTACK, SCROWDATA::EMMAXATTACK + 1 );	

	dlg.m_nSelect = m_pDummyItem->m_sAction.m_Pattern.m_dwPatternAttackSet;

	if ( dlg.DoModal() == IDOK )
	{
		if ( dlg.m_dwData >= m_pDummyItem->m_vecCrowAttack.size() &&
			 dlg.m_dwData != SCROWPATTERN::RANDOMPATTERNNUM  ) 
		{
			MessageBox( "CrowAttack Select Error" );
			return;
		}
		m_pDummyItem->m_sAction.m_Pattern.m_dwPatternAttackSet = dlg.m_dwData;
		SetWin_Text( this, IDC_EDIT_HP_DN_ATK_SET, MAXATTACK[ m_pDummyItem->m_sAction.m_Pattern.m_dwPatternAttackSet ].c_str() );	
	}
}

void CCrowData::OnBnClickedButtonHpListAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDummyItem->m_sAction.m_Pattern.m_fPatternDNRate = GetWin_Num_float ( this, IDC_EDIT_ACTION_DN_RATE );

    if( m_pDummyItem->m_sAction.m_Pattern.m_fPatternDNRate == 0.0f &&
		m_pDummyItem->m_sAction.m_Pattern.m_emActPattern == EMCROWACT_UP_IDLE &&
		m_pDummyItem->m_sAction.m_Pattern.m_dwPatternAttackSet == 0 )
	{
		return;
	}

	if( m_pDummyItem->m_sAction.m_vecPatternList.size() >= SCROWACTION::PATTERNNUM )
	{
		MessageBox ( "ERROR : MonsterAI는 10 개만큼만 저장할 수 있습니다", "ERROR", MB_OK );

		return;
	}

	m_pDummyItem->m_sAction.m_vecPatternList.push_back( m_pDummyItem->m_sAction.m_Pattern );
	
	std::sort ( 
		m_pDummyItem->m_sAction.m_vecPatternList.begin(), 
		m_pDummyItem->m_sAction.m_vecPatternList.end(), 
		CROW_PATTERN_CMP() );

	m_pDummyItem->m_sAction.m_Pattern.m_dwPatternAttackSet = 0;
	m_pDummyItem->m_sAction.m_Pattern.m_emActPattern = EMCROWACT_UP_IDLE;
	m_pDummyItem->m_sAction.m_Pattern.m_fPatternDNRate = 0.0f;

	SetWin_Num_float ( this, IDC_EDIT_ACTION_DN_RATE, m_pDummyItem->m_sAction.m_Pattern.m_fPatternDNRate );
	SetWin_Text( this, IDC_EDIT_HP_DN_ATK_SET, MAXATTACK[ m_pDummyItem->m_sAction.m_Pattern.m_dwPatternAttackSet ].c_str() );
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		SetWin_Text( this, IDC_EDIT_HP_DN_ACT
			, CGameTextMan::GetInstance().GetCommentText("CROWACT_UP", m_pDummyItem->m_sAction.m_Pattern.m_emActPattern));
		CListBox* pListBox = NULL;
		pListBox = (CListBox*) GetDlgItem ( IDC_LIST_HP );
		pListBox->ResetContent();
		CString str;
		for(int i = 0; i < ( int ) m_pDummyItem->m_sAction.m_vecPatternList.size(); i++ )
		{
			str.Format ( "%.f,  %s,  %s", m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_fPatternDNRate,
				CGameTextMan::GetInstance().GetCommentText("CROWACT_UP", m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_emActPattern ),
				MAXATTACK[ m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_dwPatternAttackSet ].c_str() );

			int nIndex = pListBox->AddString ( str );
			pListBox->SetItemData ( nIndex, i );
		}
	}
	else
	{
		SetWin_Text( this, IDC_EDIT_HP_DN_ACT, COMMENT::szCROWACT_UP[ m_pDummyItem->m_sAction.m_Pattern.m_emActPattern ].c_str() );
		CListBox* pListBox = NULL;
		pListBox = (CListBox*) GetDlgItem ( IDC_LIST_HP );
		pListBox->ResetContent();
		CString str;
		for(int i = 0; i < ( int ) m_pDummyItem->m_sAction.m_vecPatternList.size(); i++ )
		{
			str.Format ( "%.f,  %s,  %s", m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_fPatternDNRate,
				COMMENT::szCROWACT_UP[ m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_emActPattern ].c_str(),
				MAXATTACK[ m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_dwPatternAttackSet ].c_str() );

			int nIndex = pListBox->AddString ( str );
			pListBox->SetItemData ( nIndex, i );
		}
	}
}

void CCrowData::OnBnClickedButtonHpListDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_HP );
	int nIndex = pListBox->GetCurSel ();
	if ( nIndex < 0 ) return ;

	if( m_pDummyItem->m_sAction.m_vecPatternList.size() <= 0 )
	{
		MessageBox ( "ERROR : List가 0 개입니다", "ERROR", MB_OK );
		return;
	}

	m_pDummyItem->m_sAction.m_vecPatternList.erase( m_pDummyItem->m_sAction.m_vecPatternList.begin() + nIndex );

	pListBox->ResetContent();
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString str;
		for(int i = 0; i < ( int ) m_pDummyItem->m_sAction.m_vecPatternList.size(); i++ )
		{
			str.Format ( "%.f,  %s,  %s", m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_fPatternDNRate,
				CGameTextMan::GetInstance().GetCommentText("CROWACT_UP", m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_emActPattern),
				MAXATTACK[ m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_dwPatternAttackSet ].c_str() );
			
			int nIndex = pListBox->AddString ( str );
			pListBox->SetItemData ( nIndex, i );
		}
	}
	else
	{
		CString str;
		for(int i = 0; i < ( int ) m_pDummyItem->m_sAction.m_vecPatternList.size(); i++ )
		{
			str.Format ( "%.f,  %s,  %s", m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_fPatternDNRate,
				COMMENT::szCROWACT_UP[ m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_emActPattern ].c_str(),
				MAXATTACK[ m_pDummyItem->m_sAction.m_vecPatternList[ i ].m_dwPatternAttackSet ].c_str() );

			int nIndex = pListBox->AddString ( str );
			pListBox->SetItemData ( nIndex, i );
		}
	}
}
void CCrowData::OnBnClickedCheckBarrier()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( GetWin_Check ( this, IDC_CHECK_BARRIER ) ) 
	{
		GetDlgItem ( IDC_EDIT_BARRIER_RADIUS )->EnableWindow ();
		GetDlgItem ( IDC_EDIT_BARRIER_WIDTH )->EnableWindow ();
	}
	else
	{
		GetDlgItem ( IDC_EDIT_BARRIER_RADIUS )->EnableWindow ( FALSE );
		GetDlgItem ( IDC_EDIT_BARRIER_WIDTH )->EnableWindow ( FALSE );
	}
}

void CCrowData::OnBnClickedInteractionRange()
{
    GetDlgItem(IDC_EDIT_INTERACTION_RANGE)->EnableWindow(GetWin_Check(this, IDC_CHECK_INTERACTION_RANGE));
}