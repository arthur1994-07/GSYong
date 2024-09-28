// DlgInvenElementEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgInvenElementEdit.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/Item/GLItemMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
// CDlgInvenElementEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInvenElementEdit, CDialog)
CDlgInvenElementEdit::CDlgInvenElementEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInvenElementEdit::IDD, pParent)

	, m_nDamage(0)
	, m_nDefense(0)

	, m_wDefenceStat ( 0 )
	
	, m_nElectric(0)
	, m_nFire(0)
	, m_nIce(0)
	, m_nPoison(0)
	, m_nSpirit(0)
	
	, m_wTurnNum(0)

	, m_nMainID(0)
	, m_nSubID(0)

	, m_strName(_T(""))

	, m_dwCOSTUME_MID(0)
	, m_dwCOSTUME_SID(0)

	, m_sLinkSkillID ( false )
	, m_wLinkSkillLevel ( 0 )
	, m_fLinkSkillOccurRate ( 0.f )

	, m_nRandomOpt1(0)
	, m_nRandomOpt2(0)
	, m_nRandomOpt3(0)
	, m_nRandomOpt4(0)
	, m_nRandomOpt5(0)
	, m_strGenID(_T(""))
	, m_strCosTime(_T(""))
{
}

CDlgInvenElementEdit::~CDlgInvenElementEdit()
{
}

void CDlgInvenElementEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	int nValue = 0;

	DDX_Text(pDX, IDC_EDIT6, m_nDamage);
	DDX_Text(pDX, IDC_EDIT4, m_nDefense);

	nValue = static_cast< int >( m_sDamageStat.wLow );
	DDX_Text(pDX, IDC_EDIT_DAMAGE_LOW, nValue);
	nValue = static_cast< int >( m_sDamageStat.wHigh );
	DDX_Text(pDX, IDC_EDIT_DAMAGE_HIGH, nValue);
	nValue = static_cast< int >( m_wDefenceStat );
	DDX_Text(pDX, IDC_EDIT_DEFENSE, nValue);

	DDX_Text(pDX, IDC_EDIT25, m_nElectric);
	DDX_Text(pDX, IDC_EDIT31, m_nFire);
	DDX_Text(pDX, IDC_EDIT24, m_nIce);
	DDX_Text(pDX, IDC_EDIT5, m_nPoison);
	DDX_Text(pDX, IDC_EDIT8, m_nSpirit);

	DDX_Text(pDX, IDC_EDIT9, m_wTurnNum);

	DDX_Text(pDX, IDC_EDIT1, m_nMainID);
	DDX_Text(pDX, IDC_EDIT7, m_nSubID);

	DDX_Text(pDX, IDC_EDIT2, m_strName);
	DDX_Control(pDX, IDC_EDIT11, m_editTIME);

	DDX_Text(pDX, IDC_EDIT23, m_dwCOSTUME_MID);
	DDX_Text(pDX, IDC_EDIT10, m_dwCOSTUME_SID);

	DDX_Control(pDX, IDC_COMBO_LINKSKILL_TYPE, m_comboLinkSkillTarget);
	nValue = static_cast< int >( m_sLinkSkillID.wMainID );
	DDX_Text(pDX, IDC_EDIT_LINKSKILL_MID, nValue);
	nValue = static_cast< int >( m_sLinkSkillID.wSubID );
	DDX_Text(pDX, IDC_EDIT_LINKSKILL_SID, nValue);
	nValue = static_cast< int >( m_wLinkSkillLevel );
	DDX_Text(pDX, IDC_EDIT_LINKSKILL_LEVEL, nValue);
	DDX_Text(pDX, IDC_EDIT_LINKSKILL_PROBABILITY, m_fLinkSkillOccurRate);

	DDX_Control(pDX, IDC_COMBO_ADD_OPT1, m_comboAddOpt1);
	DDX_Control(pDX, IDC_COMBO_ADD_OPT2, m_comboAddOpt2);
	DDX_Control(pDX, IDC_COMBO_ADD_OPT3, m_comboAddOpt3);
	DDX_Control(pDX, IDC_COMBO_ADD_OPT4, m_comboAddOpt4);
	DDX_Control(pDX, IDC_COMBO_ADD_OPT5, m_comboAddOpt5);
	DDX_Control(pDX, IDC_COMBO_ADD_OPT6, m_comboAddOpt6);

	DDX_Control(pDX, IDC_COMBO_RANDOM_OPT1, m_comboRandomOpt1);
	DDX_Control(pDX, IDC_COMBO_RANDOM_OPT2, m_comboRandomOpt2);
	DDX_Control(pDX, IDC_COMBO_RANDOM_OPT3, m_comboRandomOpt3);
	DDX_Control(pDX, IDC_COMBO_RANDOM_OPT4, m_comboRandomOpt4);
	DDX_Control(pDX, IDC_COMBO_RANDOM_OPT5, m_comboRandomOpt5);
	DDX_Text(pDX, IDC_EDIT_RANDOM_OPT1, m_nRandomOpt1);
	DDX_Text(pDX, IDC_EDIT_RANDOM_OPT2, m_nRandomOpt2);
	DDX_Text(pDX, IDC_EDIT_RANDOM_OPT3, m_nRandomOpt3);
	DDX_Text(pDX, IDC_EDIT_RANDOM_OPT4, m_nRandomOpt4);
	DDX_Text(pDX, IDC_EDIT_RANDOM_OPT5, m_nRandomOpt5);
	DDX_Text(pDX, IDC_EDIT14, m_strGenID);
	DDX_Text(pDX, IDC_EDIT21, m_strCosTime);
}


BEGIN_MESSAGE_MAP(CDlgInvenElementEdit, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_DISGUISE, OnBnClickedBtnDisguise)
END_MESSAGE_MAP()


// CDlgInvenElementEdit 메시지 처리기입니다.

BOOL CDlgInvenElementEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Rate (-100)-(+100)
	// Resist (-100)-(+100)
	// Defence, Damage (-100)-(+100)
	// ReModel (개조횟수) 0-9
	// Used (사용횟수, ex> 화살갯수) 0-100
	SITEM *pItemData = GLogicData::GetInstance().GetItem(m_sItem.sItemCustom.GetNativeID());
	m_strName = pItemData->GetName(); //  sBasicOp. .strName; // szName;
	m_nMainID = m_sItem.sItemCustom.Mid();
	m_nSubID = m_sItem.sItemCustom.Sid();

	m_nDamage    = m_sItem.sItemCustom.cDAMAGE;
	m_nDefense   = m_sItem.sItemCustom.cDEFENSE;

	m_sDamageStat = m_sItem.sItemCustom.GetBasicAttackDamage();
	m_wDefenceStat = m_sItem.sItemCustom.GetBasicDefence();
	
	m_nElectric = m_sItem.sItemCustom.cRESIST_ELEC;
	m_nFire     = m_sItem.sItemCustom.cRESIST_FIRE;
	m_nIce      = m_sItem.sItemCustom.cRESIST_ICE;
	m_nPoison   = m_sItem.sItemCustom.cRESIST_POISON;
	m_nSpirit   = m_sItem.sItemCustom.cRESIST_SPIRIT;
	
	m_wTurnNum = m_sItem.sItemCustom.wTurnNum;

	m_dwCOSTUME_MID = m_sItem.sItemCustom.nidDISGUISE.wMainID;
	m_dwCOSTUME_SID = m_sItem.sItemCustom.nidDISGUISE.wSubID;

	CTime cTIME ( m_sItem.sItemCustom.tBORNTIME );
	CString strTIME = cTIME.Format ( "%Y/%m/%d %H:%M" );
	m_editTIME.SetWindowText ( strTIME );

	CTime cCosTIME ( m_sItem.sItemCustom.tDISGUISE );
	CString strCosTIME = cCosTIME.Format ( "%Y/%m/%d %H:%M" );
	m_strCosTime = strCosTIME;

	std::string strGuid = sc::string::uuidToString( ( UUID )m_sItem.sItemCustom.guid );
	m_strGenID.Format ( "%d/%d/%d/%s",
		m_sItem.sItemCustom.cGenType,
		// m_sItem.sItemCustom.cChnID,
		// m_sItem.sItemCustom.cFieldID,
		0,
		0,
		strGuid.c_str() );

	// Link Skill;
	{
		m_comboLinkSkillTarget.ResetContent();
		for ( int i=0; i<EMITEM_SKILL_SIZE; ++i )
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				int nIndex = m_comboLinkSkillTarget.AddString(
					CGameTextMan::GetInstance().GetCommentText(
					"SKILL_LINK_APPLY_TYPE", i ).GetString() );
				m_comboLinkSkillTarget.SetItemData( nIndex, i );
			}
			else
			{
				int nIndex = m_comboLinkSkillTarget.AddString(
					COMMENT::SKILL_LINK_TYPE[ i ].c_str() );
				m_comboLinkSkillTarget.SetItemData( nIndex, i );
			}
		}

		m_sLinkSkillID = m_sItem.sItemCustom.GetLinkSkillID();
		m_wLinkSkillLevel = m_sItem.sItemCustom.GetLinkSkillLevel();
		m_fLinkSkillOccurRate = m_sItem.sItemCustom.GetLinkSkillOccurRate();

		int nType = static_cast< int >( m_sItem.sItemCustom.GetLinkSkillTarget() );
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			m_comboAddOpt1.SelectString(
				nType-1, CGameTextMan::GetInstance().GetCommentText(
				"SKILL_LINK_APPLY_TYPE", nType ).GetString() );
		}
		else
		{
			m_comboLinkSkillTarget.SelectString(
				nType-1, COMMENT::SKILL_LINK_TYPE[ nType ].c_str() );
		}
	}

	// Add Option;
	{
		m_comboAddOpt1.ResetContent();
		m_comboAddOpt2.ResetContent();
		m_comboAddOpt3.ResetContent();
		m_comboAddOpt4.ResetContent();
		m_comboAddOpt5.ResetContent();
		m_comboAddOpt6.ResetContent();

		for ( UINT i=0; i<EMADD_SIZE; ++i )
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				int nIndex = m_comboAddOpt1.AddString( CGameTextMan::GetInstance().GetCommentText( "ITEMADDON", i ).GetString() );
				m_comboAddOpt1.SetItemData( nIndex, i );
				nIndex = m_comboAddOpt2.AddString( CGameTextMan::GetInstance().GetCommentText( "ITEMADDON", i ).GetString() );
				m_comboAddOpt2.SetItemData( nIndex, i );
				nIndex = m_comboAddOpt3.AddString( CGameTextMan::GetInstance().GetCommentText( "ITEMADDON", i ).GetString() );
				m_comboAddOpt3.SetItemData( nIndex, i );
				nIndex = m_comboAddOpt4.AddString( CGameTextMan::GetInstance().GetCommentText( "ITEMADDON", i ).GetString() );
				m_comboAddOpt4.SetItemData( nIndex, i );
				nIndex = m_comboAddOpt5.AddString( CGameTextMan::GetInstance().GetCommentText( "ITEMADDON", i ).GetString() );
				m_comboAddOpt5.SetItemData( nIndex, i );
				nIndex = m_comboAddOpt6.AddString( CGameTextMan::GetInstance().GetCommentText( "ITEMADDON", i ).GetString() );
				m_comboAddOpt5.SetItemData( nIndex, i );
			}
			else
			{
				int nIndex = m_comboAddOpt1.AddString( COMMENT::ITEMADDON[i].c_str() );
				m_comboAddOpt1.SetItemData( nIndex, i );
				nIndex = m_comboAddOpt2.AddString( COMMENT::ITEMADDON[i].c_str() );
				m_comboAddOpt2.SetItemData( nIndex, i );
				nIndex = m_comboAddOpt3.AddString( COMMENT::ITEMADDON[i].c_str() );
				m_comboAddOpt3.SetItemData( nIndex, i );
				nIndex = m_comboAddOpt4.AddString( COMMENT::ITEMADDON[i].c_str() );
				m_comboAddOpt4.SetItemData( nIndex, i );
				nIndex = m_comboAddOpt5.AddString( COMMENT::ITEMADDON[i].c_str() );
				m_comboAddOpt5.SetItemData( nIndex, i );
				nIndex = m_comboAddOpt6.AddString( COMMENT::ITEMADDON[i].c_str() );
				m_comboAddOpt5.SetItemData( nIndex, i );
			}
		}

		int nType = 0;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 0 );
			m_comboAddOpt1.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 1 );
			m_comboAddOpt2.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 2 );
			m_comboAddOpt3.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 3 );
			m_comboAddOpt4.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 4 );
			m_comboAddOpt5.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 5 );
			m_comboAddOpt6.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
		}
		else
		{
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 0 );
			m_comboAddOpt1.SelectString ( nType-1, COMMENT::ITEMADDON[nType].c_str() );
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 1 );
			m_comboAddOpt2.SelectString ( nType-1, COMMENT::ITEMADDON[nType].c_str() );
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 2 );
			m_comboAddOpt3.SelectString ( nType-1, COMMENT::ITEMADDON[nType].c_str() );
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 3 );
			m_comboAddOpt4.SelectString ( nType-1, COMMENT::ITEMADDON[nType].c_str() );
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 4 );
			m_comboAddOpt5.SelectString ( nType-1, COMMENT::ITEMADDON[nType].c_str() );
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( 5 );
			m_comboAddOpt6.SelectString ( nType-1, COMMENT::ITEMADDON[nType].c_str() );
		}

		int nDlgID_AddOption[ SITEMCUSTOM_ADDON::ADDON_SIZE ] =
		{
			IDC_EDIT_ADD_OPT1,
			IDC_EDIT_ADD_OPT2,
			IDC_EDIT_ADD_OPT3,
			IDC_EDIT_ADD_OPT4,
			IDC_EDIT_ADD_OPT5,
			IDC_EDIT_ADD_OPT6,
		};

		for ( UINT i=0; i<SITEMCUSTOM_ADDON::ADDON_SIZE; ++i )
		{
			nType = m_sItem.sItemCustom.sAddonInfo.GetOptType( i );
			if ( true == ITEM::bAddonValueInteger[ nType ] )
				SetWin_Num_int ( this, nDlgID_AddOption[ i ], m_sItem.sItemCustom.sAddonInfo.m_sADDON[i].nVALUE );
			else
				SetWin_Num_float ( this, nDlgID_AddOption[ i ], m_sItem.sItemCustom.sAddonInfo.m_sADDON[i].fVALUE );
		}
	}

	m_comboRandomOpt1.ResetContent();
	m_comboRandomOpt2.ResetContent();
	m_comboRandomOpt3.ResetContent();
	m_comboRandomOpt4.ResetContent();
	m_comboRandomOpt5.ResetContent();
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		for ( int i=0; i<RandomOption::RANDOM_OPTION_NSIZE; ++i )
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				int nIndex = m_comboRandomOpt1.AddString(CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", i).GetString());
				m_comboRandomOpt1.SetItemData ( nIndex, i );
				nIndex = m_comboRandomOpt2.AddString(CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", i).GetString());
				m_comboRandomOpt2.SetItemData ( nIndex, i );
				nIndex = m_comboRandomOpt3.AddString(CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", i).GetString());
				m_comboRandomOpt3.SetItemData ( nIndex, i );
				nIndex = m_comboRandomOpt4.AddString(CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", i).GetString());
				m_comboRandomOpt4.SetItemData ( nIndex, i );
				nIndex = m_comboRandomOpt5.AddString(CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", i).GetString());
				m_comboRandomOpt5.SetItemData ( nIndex, i );
			}
		}

		int nType;
		nType = m_sItem.sItemCustom.GETOptTYPE1();
		m_comboRandomOpt1.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", nType).GetString());
		nType = m_sItem.sItemCustom.GETOptTYPE2();
		m_comboRandomOpt2.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", nType).GetString());
		nType = m_sItem.sItemCustom.GETOptTYPE3();
		m_comboRandomOpt3.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", nType).GetString());
		nType = m_sItem.sItemCustom.GETOptTYPE4();
		m_comboRandomOpt4.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", nType).GetString());
		nType = m_sItem.sItemCustom.GETOptTYPE5();
		m_comboRandomOpt5.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", nType).GetString());
	}
	else
	{
		for ( int i=0; i<RandomOption::RANDOM_OPTION_NSIZE; ++i )
		{
			int nIndex = m_comboRandomOpt1.AddString(COMMENT::ITEM_RANDOM_OPT[i].c_str());
			m_comboRandomOpt1.SetItemData ( nIndex, i );
			nIndex = m_comboRandomOpt2.AddString(COMMENT::ITEM_RANDOM_OPT[i].c_str());
			m_comboRandomOpt2.SetItemData ( nIndex, i );
			nIndex = m_comboRandomOpt3.AddString(COMMENT::ITEM_RANDOM_OPT[i].c_str());
			m_comboRandomOpt3.SetItemData ( nIndex, i );
			nIndex = m_comboRandomOpt4.AddString(COMMENT::ITEM_RANDOM_OPT[i].c_str());
			m_comboRandomOpt4.SetItemData ( nIndex, i );
			nIndex = m_comboRandomOpt5.AddString(COMMENT::ITEM_RANDOM_OPT[i].c_str());
			m_comboRandomOpt5.SetItemData ( nIndex, i );
		}

		int nType;
		nType = m_sItem.sItemCustom.GETOptTYPE1();
		m_comboRandomOpt1.SelectString ( nType-1, COMMENT::ITEM_RANDOM_OPT[nType].c_str() );
		nType = m_sItem.sItemCustom.GETOptTYPE2();
		m_comboRandomOpt2.SelectString ( nType-1, COMMENT::ITEM_RANDOM_OPT[nType].c_str() );
		nType = m_sItem.sItemCustom.GETOptTYPE3();
		m_comboRandomOpt3.SelectString ( nType-1, COMMENT::ITEM_RANDOM_OPT[nType].c_str() );
		nType = m_sItem.sItemCustom.GETOptTYPE4();
		m_comboRandomOpt4.SelectString ( nType-1, COMMENT::ITEM_RANDOM_OPT[nType].c_str() );
		nType = m_sItem.sItemCustom.GETOptTYPE5();
		m_comboRandomOpt5.SelectString ( nType-1, COMMENT::ITEM_RANDOM_OPT[nType].c_str() );
	}

	m_nRandomOpt1 = m_sItem.sItemCustom.randomOption.getValue(0);
	m_nRandomOpt2 = m_sItem.sItemCustom.randomOption.getValue(1);
	m_nRandomOpt3 = m_sItem.sItemCustom.randomOption.getValue(2);
	m_nRandomOpt4 = m_sItem.sItemCustom.randomOption.getValue(3);
	m_nRandomOpt5 = m_sItem.sItemCustom.randomOption.getValue(4);

	// 코스튬 기간 변경용으로 만들었지만, 지금은 생성시간 변경용으로 사용한다
	m_dlgItemDisguise.m_cTime = CTime( m_sItem.sItemCustom.tBORNTIME );

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgInvenElementEdit::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	for( int i=0; i<RandomOpt::NSIZE ; i++)
		m_sItem.sItemCustom.randomOption.remove(0);

	int nIndex, nData;

	// Basic Stats;
	{
		GLPADATA sDamage;
		sDamage.wHigh = static_cast< WORD >( GetWin_Num_int( this, IDC_EDIT_DAMAGE_HIGH ) );
		sDamage.wLow = static_cast< WORD >( GetWin_Num_int( this, IDC_EDIT_DAMAGE_LOW ) );
		m_sItem.sItemCustom.SetBasicAttackDamage( sDamage );

		WORD wDefence = 0;
		wDefence = static_cast< WORD >( GetWin_Num_int( this, IDC_EDIT_DEFENSE ) );
		m_sItem.sItemCustom.SetBasicDefence( wDefence );
	}

	// Link Skill;
	{
		nIndex = m_comboLinkSkillTarget.GetCurSel();
		nData = static_cast< int >( m_comboLinkSkillTarget.GetItemData( nIndex ) );
		m_sItem.sItemCustom.SetLinkSkillTarget( static_cast< EMITEM_SKILL_LINK >( nData ) );

		SNATIVEID sSkillID( false );
		sSkillID.wMainID = static_cast< WORD >( GetWin_Num_int( this, IDC_EDIT_LINKSKILL_MID ) );
		sSkillID.wSubID = static_cast< WORD >( GetWin_Num_int( this, IDC_EDIT_LINKSKILL_SID ) );
		m_sItem.sItemCustom.SetLinkSkillID( sSkillID );

		int nSkillLevel = 0;
		nSkillLevel = GetWin_Num_int( this, IDC_EDIT_LINKSKILL_LEVEL );
		m_sItem.sItemCustom.SetLinkSkillLevel( static_cast< WORD >( nSkillLevel ) );

		float fSkillOccurRate = 0.f;
		fSkillOccurRate = GetWin_Num_float( this, IDC_EDIT_LINKSKILL_PROBABILITY );
		m_sItem.sItemCustom.SetLinkSkillOccurRate( fSkillOccurRate );
	}

	// Add Option;
	{
		nIndex = m_comboAddOpt1.GetCurSel();
		nData = (int)m_comboAddOpt1.GetItemData( nIndex );
		m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 0 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
		if ( true == ITEM::bAddonValueInteger[ nData ] )
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 0 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT1 );
		else
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 0 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT1 );

		nIndex = m_comboAddOpt2.GetCurSel();
		nData = (int)m_comboAddOpt2.GetItemData( nIndex );
		m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 1 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
		if ( true == ITEM::bAddonValueInteger[ nData ] )
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 1 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT2 );
		else
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 1 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT2 );

		nIndex = m_comboAddOpt3.GetCurSel();
		nData = (int)m_comboAddOpt3.GetItemData( nIndex );
		m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 2 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
		if ( true == ITEM::bAddonValueInteger[ nData ] )
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 2 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT3 );
		else
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 2 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT3 );

		nIndex = m_comboAddOpt4.GetCurSel();
		nData = (int)m_comboAddOpt4.GetItemData( nIndex );
		m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 3 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
		if ( true == ITEM::bAddonValueInteger[ nData ] )
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 3 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT4 );
		else
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 3 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT4 );

		nIndex = m_comboAddOpt5.GetCurSel();
		nData = (int)m_comboAddOpt5.GetItemData( nIndex );
		m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 4 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
		if ( true == ITEM::bAddonValueInteger[ nData ] )
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 4 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT5 );
		else
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 4 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT5 );

		nIndex = m_comboAddOpt6.GetCurSel();
		nData = (int)m_comboAddOpt6.GetItemData( nIndex );
		m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 5 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
		if ( true == ITEM::bAddonValueInteger[ nData ] )
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 5 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT6 );
		else
			m_sItem.sItemCustom.sAddonInfo.m_sADDON[ 5 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT6 );
	}

	nIndex = m_comboRandomOpt1.GetCurSel();
	nData = (int) m_comboRandomOpt1.GetItemData ( nIndex );
	m_sItem.sItemCustom.randomOption.assign(0, nData, m_nRandomOpt1);

	nIndex = m_comboRandomOpt2.GetCurSel();
	nData = (int) m_comboRandomOpt2.GetItemData ( nIndex );
	m_sItem.sItemCustom.randomOption.assign(1, nData, m_nRandomOpt2);

	nIndex = m_comboRandomOpt3.GetCurSel();
	nData = (int) m_comboRandomOpt3.GetItemData ( nIndex );
	m_sItem.sItemCustom.randomOption.assign(2, nData, m_nRandomOpt3);

	nIndex = m_comboRandomOpt4.GetCurSel();
	nData = (int) m_comboRandomOpt4.GetItemData ( nIndex );
	m_sItem.sItemCustom.randomOption.assign(3, nData, m_nRandomOpt4);

	nIndex = m_comboRandomOpt5.GetCurSel();
	nData = (int) m_comboRandomOpt5.GetItemData ( nIndex );
	m_sItem.sItemCustom.randomOption.assign(4, nData, m_nRandomOpt5);

	// 코스튬 기간 변경용으로 만들었지만, 지금은 생성시간 변경용으로 사용한다
	m_sItem.sItemCustom.tBORNTIME = m_dlgItemDisguise.m_cTime.GetTime();

	OnOK();
}

void CDlgInvenElementEdit::OnBnClickedBtnDisguise()
{
	// 코스튬 기간 변경용으로 만들었지만, 지금은 생성시간 변경용으로 사용한다
	if( m_dlgItemDisguise.DoModal() == IDOK )
	{
		CString strTime = m_dlgItemDisguise.m_cTime.Format( "%Y/%m/%d %H:%M" );
		m_editTIME.SetWindowText( strTime );

		UpdateData( FALSE );
	}
}