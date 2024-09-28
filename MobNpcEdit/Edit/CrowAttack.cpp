// ItemProperty2.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../MobNpcEdit.h"

#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinDataDummy.h"
#include "../../RanLogic/Crow/GLCrowData.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../../enginelib/GUInterface/GameTextControl.h"

#include "./SheetWithTab.h"
#include "./CrowAttack.h"
#include "../SelectDialog.h"

const std::string MAXATTACK = "공격 세트";

// CCrowAttack 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCrowAttack, CPropertyPage)
CCrowAttack::CCrowAttack( LOGFONT logfont )
	: CPropertyPage(CCrowAttack::IDD)
	, m_pFont ( NULL )
	, m_nSelectAttack(0)
	//, m_pVecCrowAttack( NULL )
{
	m_bDlgInit = FALSE;
	m_pItem = NULL;
	m_pDummyItem = new SCROWDATA;
	
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CCrowAttack::~CCrowAttack()
{
	SAFE_DELETE ( m_pDummyItem );
	SAFE_DELETE ( m_pFont );
}

void CCrowAttack::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CROW_ATTACK, m_listCrowAttack);
}


BEGIN_MESSAGE_MAP(CCrowAttack, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_CBN_SELCHANGE(IDC_COMBO_ANI_ATTACKTYPE, OnCbnSelchangeComboAniAttacktype)
	ON_BN_CLICKED(IDC_BUTTON_SELFBODYEFFECT, OnBnClickedButtonSelfbodyeffect)
	ON_BN_CLICKED(IDC_BUTTON_TARGETBODYEFFECT, OnBnClickedButtonTargetbodyeffect)
	ON_BN_CLICKED(IDC_BUTTON_TARGETEFFECT, OnBnClickedButtonTargeteffect)
	ON_BN_CLICKED(IDC_BUTTON_SKILL, OnBnClickedButtonSkill)
	ON_BN_CLICKED(IDC_BUTTON_BLOW, OnBnClickedButtonBlow)
	ON_BN_CLICKED(IDC_BUTTON_CROW_ATTACK_ADD, OnBnClickedButtonCrowAttackAdd)
	ON_BN_CLICKED(IDC_BUTTON_CROW_ATTACK_DEL, OnBnClickedButtonCrowAttackDel)
	ON_LBN_SELCHANGE(IDC_LIST_CROW_ATTACK, OnLbnSelchangeListCrowAttack)
END_MESSAGE_MAP()


// CCrowAttack 메시지 처리기입니다.

void CCrowAttack::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveCrowAttackTreePage ( m_CallPage, m_pItem );

	m_pItem = NULL;
}

void CCrowAttack::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	InverseUpdateItems();

	GLCrowDataMan::GetInstance().InsertCrowData ( m_pDummyItem->sNativeID, m_pDummyItem );

	m_pSheetTab->ActiveCrowAttackTreePage ( m_CallPage, m_pItem );

	m_pItem = NULL;
}

BOOL CCrowAttack::SetItem ( SCROWDATA* pItem, DWORD dwID )
{
	if ( m_pItem )
	{
		return FALSE;
	}

	m_pItem = pItem;
	m_sSelectedItem.dwID = dwID;
	//m_pVecCrowAttack = pVecCrowAttack;

	//	더미로 가져가기
	m_pDummyItem->Assign( *m_pItem );
	//m_vecDummyCrowAttack = *m_pVecCrowAttack;

	if ( m_bDlgInit ) 
	{
		InitDefaultCtrls ();
		UpdateItems ();		
	}

	return TRUE;
}

void CCrowAttack::UpdateItems ()
{
	m_listCrowAttack.ResetContent();

    CString strTemp;

	CROWATTACK_VEC& vecCrowAttack =
		( m_sSelectedItem.Mid() == 0 ) ?
		m_pDummyItem->m_vecCrowAttackPage.at( m_sSelectedItem.Sid() ) :
	m_pDummyItem->m_vecCrowAttackEventPage.at( m_sSelectedItem.Sid() );

	for ( int i = 0; i < (int)vecCrowAttack.size(); ++i )
	{
		strTemp.Format( "%s%02d", MAXATTACK.c_str(), i );
		m_listCrowAttack.AddString( strTemp.GetString() );
	}

	if ( m_listCrowAttack.GetCount() > 0 )
	{
		m_nSelectAttack = 0;
		m_listCrowAttack.SetCurSel( m_nSelectAttack );
	}
	else
	{
		m_nSelectAttack = -1;
		m_listCrowAttack.SetCurSel( m_nSelectAttack );
	}
	
	UpdateMaxattack();
}

BOOL CCrowAttack::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	m_bDlgInit = TRUE;

	InitDefaultCtrls ();
	UpdateItems ();

	SetReadMode();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CCrowAttack::InverseUpdateItems ()
{
	//	이 부분은 특성상 바로바로 업데이트 한다.	
	//	하지만, 한번도 DropDown을 이용해 아이템을 바꾸지 않으면,
	//	저장되지 않기 때문에, 마지막에 데이타를 갱신한놈은
	//	따로 처리해주는 것이다.
	InverseUpdateMaxattack ();
}

void CCrowAttack::OnBnClickedButtonPrev()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateItems();

	GLCrowDataMan::GetInstance().InsertCrowData ( m_pDummyItem->sNativeID, m_pDummyItem );

	m_pSheetTab->ActiveCrowAttackTreePage ( m_CallPage, m_pItem );
	
	m_pItem = NULL;
}

void CCrowAttack::InitDefaultCtrls ()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string>	vecSUB_TYPE;
		std::string strATTACK_RGTYPE[EMATT_SIZE], strANI_MAINTYPE[AN_TYPE_SIZE], strANI_SUBTYPE[AN_SUB_SIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("ATTACK_RGTYPE", strATTACK_RGTYPE, CGameTextMan::EM_COMMENT_TEXT, EMATT_SIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("ANI_MAINTYPE", strATTACK_RGTYPE, CGameTextMan::EM_COMMENT_TEXT, AN_TYPE_SIZE);
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("ANI_SUBTYPE", vecSUB_TYPE, CGameTextMan::EM_COMMENT_TEXT);
		for ( int i = 0; i < AN_SUB_SIZE; ++i )
			strANI_SUBTYPE[i] = vecSUB_TYPE[i];

		SetWin_Combo_Init(this, IDC_COMBO_ATTACK_RANGE_TYPE, strATTACK_RGTYPE, EMATT_SIZE);
		SetWin_Combo_Init(this, IDC_COMBO_MAINTYPE,	strANI_MAINTYPE,	AN_TYPE_SIZE);
		SetWin_Combo_Init(this, IDC_COMBO_SUBTYPE,	strANI_SUBTYPE,		AN_SUB_SIZE);
		CListBox* pListBox;
		pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIATTACKKEYS );
		pListBox->ResetContent ();

		GetDlgItem ( IDC_BUTTON_NEXT )->EnableWindow ( FALSE );
	}
	else
	{
		SetWin_Combo_Init ( this, IDC_COMBO_ATTACK_RANGE_TYPE, COMMENT::ATTACK_RGTYPE, EMATT_SIZE );

		SetWin_Combo_Init ( this, IDC_COMBO_MAINTYPE, COMMENT::ANI_MAINTYPE, AN_TYPE_SIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SUBTYPE, COMMENT::ANI_SUBTYPE, AN_SUB_SIZE );

		CListBox* pListBox;
		pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIATTACKKEYS );
		pListBox->ResetContent ();

		GetDlgItem ( IDC_BUTTON_NEXT )->EnableWindow ( FALSE );
	}
}

// static BOOL _FindAniFileFunc ( void* value, SANIMCONINFO &Data)
// {
// 	return !Data.m_Name.compare((char*) value);
// }

void CCrowAttack::InverseUpdateMaxattack()
{
	//// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CROWATTACK_VEC& vecCrowAttack =
		( m_sSelectedItem.Mid() == 0 ) ?
		m_pDummyItem->m_vecCrowAttackPage.at( m_sSelectedItem.Sid() ) :
	m_pDummyItem->m_vecCrowAttackEventPage.at( m_sSelectedItem.Sid() );

	if ( m_nSelectAttack < 0 ) return;
	if ( m_nSelectAttack >= (int)vecCrowAttack.size() ) return;

	int nIndex = m_nSelectAttack;
	SCROWATTACK &rCrowAttack = vecCrowAttack[nIndex];

	rCrowAttack.bUsed = GetWin_Check ( this, IDC_CHECK_IS_USE );

	rCrowAttack.emAttRgType = static_cast<EMATT_RGTYPE> ( GetWin_Combo_Sel ( this, IDC_COMBO_ATTACK_RANGE_TYPE ) );
	rCrowAttack.wRange = GetWin_Num_int ( this, IDC_EDIT_RANGE );

	rCrowAttack.sDamage.wHigh = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_MAX );
	rCrowAttack.sDamage.wLow = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_MIN );

	rCrowAttack.fDelay = GetWin_Num_float ( this, IDC_EDIT_DELAY );

	rCrowAttack.wUse_SP = GetWin_Num_int ( this, IDC_EDIT_DEC_SP );

	rCrowAttack.skill_id.wMainID = GetWin_Num_int ( this, IDC_EDIT_SKILL_MID );
	rCrowAttack.skill_id.wSubID = GetWin_Num_int ( this, IDC_EDIT_SKILL_SID );

	//	상태이상
	rCrowAttack.fBLOW_RATE = GetWin_Num_float ( this, IDC_EDIT_BLOW_RATE );
	rCrowAttack.fBLOW_LIFE = GetWin_Num_float ( this, IDC_EDIT_BLOW_LIFE );
	rCrowAttack.fBLOW_VAR1 = GetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE1 );
	rCrowAttack.fBLOW_VAR2 = GetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE2 );

	switch ( rCrowAttack.emBLOW_TYPE )
	{
	case EMBLOW_NUMB:
		//	이동속도.
		LIMIT ( rCrowAttack.fBLOW_VAR1, 1.0f, -1.0f );
		//	딜래이
		LIMIT ( rCrowAttack.fBLOW_VAR2, 10.0f, -1.0f );
		break;
	case EMBLOW_STUN:
		break;
	case EMBLOW_STONE:
		//	이동속도.
		LIMIT ( rCrowAttack.fBLOW_VAR1, 1.0f, -1.0f );
		break;
	case EMBLOW_BURN:
		break;
	case EMBLOW_FROZEN:
		//	이동속도.
		LIMIT ( rCrowAttack.fBLOW_VAR1, 1.0f, -1.0f );
		//	타격증폭
		LIMIT ( rCrowAttack.fBLOW_VAR2, 10.0f, -1.0f );
		break;

	case EMBLOW_MAD:
		break;
	case EMBLOW_POISON:
		break;
	case EMBLOW_CURSE:
		break;
	};

	rCrowAttack.strSelfBodyEffect = GetWin_Text ( this, IDC_EDIT_SELFBODYEFFECT );
	rCrowAttack.strTargBodyEffect = GetWin_Text ( this, IDC_EDIT_TARGETBODYEFFECT );
	rCrowAttack.strTargetEffect = GetWin_Text ( this, IDC_EDIT_TARGETEFFECT );
}

void CCrowAttack::UpdateMaxattack()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CROWATTACK_VEC& vecCrowAttack =
		( m_sSelectedItem.Mid() == 0 ) ?
		m_pDummyItem->m_vecCrowAttackPage.at( m_sSelectedItem.Sid() ) :
	m_pDummyItem->m_vecCrowAttackEventPage.at( m_sSelectedItem.Sid() );

	if ( m_nSelectAttack < 0 ) return;
	if ( m_nSelectAttack >= (int)vecCrowAttack.size() ) return;

	int nIndex = m_nSelectAttack;

	SCROWATTACK &rCrowAttack = vecCrowAttack[nIndex];

	SetWin_Check ( this, IDC_CHECK_IS_USE, rCrowAttack.bUsed );

	BOOL bSTATE_BLOW = ( rCrowAttack.emBLOW_TYPE!=EMBLOW_NONE );
	BOOL bBLOW_VAR1(TRUE), bBLOW_VAR2(TRUE);
	char *szBLOW_VAR1;
	char *szBLOW_VAR2;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strATTACK_RGTYPE[EMATT_SIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("ATTACK_RGTYPE", strATTACK_RGTYPE, CGameTextMan::EM_COMMENT_TEXT, EMATT_SIZE);

		SetWin_Combo_Sel ( this, IDC_COMBO_ATTACK_RANGE_TYPE, strATTACK_RGTYPE[rCrowAttack.emAttRgType].c_str() );
		//	상태이상;
		SetWin_Text( this, IDC_EDIT_BLOW, CGameTextMan::GetInstance().GetText("BLOW",rCrowAttack.emBLOW_TYPE, CGameTextMan::EM_COMMENT_TEXT).GetString() );
		szBLOW_VAR1 = const_cast<char*>(CGameTextMan::GetInstance().GetText("BLOW_VAR1", rCrowAttack.emBLOW_TYPE, CGameTextMan::EM_COMMENT_TEXT ).GetString());
		szBLOW_VAR2 = const_cast<char*>(CGameTextMan::GetInstance().GetText("BLOW_VAR2", rCrowAttack.emBLOW_TYPE, CGameTextMan::EM_COMMENT_TEXT ).GetString());
		if ( !strcmp(szBLOW_VAR1, CGameTextMan::GetInstance().GetText("BLOW_VAR1", EMBLOW_NONE, CGameTextMan::EM_COMMENT_TEXT).GetString()) )		bBLOW_VAR1 = FALSE;
		if ( !strcmp(szBLOW_VAR2, CGameTextMan::GetInstance().GetText("BLOW_VAR2", EMBLOW_NONE, CGameTextMan::EM_COMMENT_TEXT).GetString()) )		bBLOW_VAR2 = FALSE;

	}
	else
	{
		SetWin_Combo_Sel ( this, IDC_COMBO_ATTACK_RANGE_TYPE, COMMENT::ATTACK_RGTYPE[rCrowAttack.emAttRgType].c_str() );
		//	상태이상;
		SetWin_Text( this, IDC_EDIT_BLOW, COMMENT::BLOW[rCrowAttack.emBLOW_TYPE].c_str() );
		szBLOW_VAR1 = const_cast<char*>(COMMENT::BLOW_VAR1[rCrowAttack.emBLOW_TYPE].c_str());
		szBLOW_VAR2 = const_cast<char*>(COMMENT::BLOW_VAR2[rCrowAttack.emBLOW_TYPE].c_str());
		if ( !strcmp(szBLOW_VAR1,COMMENT::BLOW_VAR1[EMBLOW_NONE].c_str()) )		bBLOW_VAR1 = FALSE;
		if ( !strcmp(szBLOW_VAR2,COMMENT::BLOW_VAR2[EMBLOW_NONE].c_str()) )		bBLOW_VAR2 = FALSE;
	}
	SetWin_Enable ( this, IDC_EDIT_BLOW_RATE, bSTATE_BLOW );
	SetWin_Enable ( this, IDC_EDIT_BLOW_LIFE, bSTATE_BLOW );

	SetWin_Text ( this, IDC_STATIC_BLOWVAR1, szBLOW_VAR1 );
	SetWin_Text ( this, IDC_STATIC_BLOWVAR2, szBLOW_VAR2 );
	SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE1, bBLOW_VAR1 );
	SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE2, bBLOW_VAR2 );

	SetWin_Num_int ( this, IDC_EDIT_RANGE, rCrowAttack.wRange );

	SetWin_Num_int ( this, IDC_EDIT_DAMAGE_MAX, rCrowAttack.sDamage.wHigh );
	SetWin_Num_int ( this, IDC_EDIT_DAMAGE_MIN, rCrowAttack.sDamage.wLow );

	SetWin_Num_float ( this, IDC_EDIT_DELAY, rCrowAttack.fDelay );

	SetWin_Num_int ( this, IDC_EDIT_DEC_SP, rCrowAttack.wUse_SP );

	SetWin_Num_int ( this, IDC_EDIT_SKILL_MID, rCrowAttack.skill_id.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_SKILL_SID, rCrowAttack.skill_id.wSubID );

	SetWin_Num_float ( this, IDC_EDIT_BLOW_RATE, rCrowAttack.fBLOW_RATE );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_LIFE, rCrowAttack.fBLOW_LIFE );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE1, rCrowAttack.fBLOW_VAR1 );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE2, rCrowAttack.fBLOW_VAR2 );

	SetWin_Text ( this, IDC_EDIT_SELFBODYEFFECT, rCrowAttack.strSelfBodyEffect.c_str() );
	SetWin_Text ( this, IDC_EDIT_TARGETBODYEFFECT, rCrowAttack.strTargBodyEffect.c_str() );
	SetWin_Text ( this, IDC_EDIT_TARGETEFFECT, rCrowAttack.strTargetEffect.c_str() );

	//	Note : 공격 에니메이션 리스팅.
	//
	CString str;
	CComboBox *pComboBox;

	pComboBox = (CComboBox*) GetDlgItem ( IDC_COMBO_ANI_ATTACKTYPE );
	pComboBox->ResetContent ();
	
	DxSkinDataDummy DataDummy;
	BOOL bOK = DataDummy.LoadFile ( m_pItem->m_sAction.m_strSkinObj.c_str() );
	if ( !bOK )
	{
		MessageBox ( "FAILURE : Fiexed selected Crow skin file can not be read.", "ERROR", MB_OK );
		return;
	}
	
	size_t nSIZE = DataDummy.m_vecANIMINFO.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		const SANIMCONINFO *pAnimConInfo = DataDummy.m_vecANIMINFO[n];

		if ( pAnimConInfo->m_MainType==AN_ATTACK )
		{
			nIndex = pComboBox->AddString ( pAnimConInfo->m_strCfgFile.c_str() ); 
		}
	}

	SANIMCONINFO *pAnimConInfo = NULL;
	if ( !rCrowAttack.strAniFile.empty() )
	{
		if ( pAnimConInfo = DataDummy.FindAniInfo(rCrowAttack.strAniFile.c_str()) )
		{
			if ( pAnimConInfo->m_MainType!=AN_ATTACK )	pAnimConInfo = NULL;
		}
		else
		{
			MessageBox ( "ERROR : Previous selected animation file is lost.", "ERROR", MB_OK );
		}
	}

	if ( !pAnimConInfo )
	{
		nIndex = pComboBox->SetCurSel ( 0 );
		if ( nIndex!=-1 )
		{
			pComboBox->GetWindowText ( str );
			pAnimConInfo = DataDummy.FindAniInfo(str.GetString());
		}
	}
	
	if ( pComboBox->GetCurSel()==LB_ERR )
	{
		int nIndex = pComboBox->FindString ( 0, rCrowAttack.strAniFile.c_str() );
		if ( nIndex!=LB_ERR )	pComboBox->SetCurSel ( nIndex );
	}

	UpdateAttackAni ( rCrowAttack, pAnimConInfo );

	SetReadMode();
}

void CCrowAttack::UpdateAttackAni ( SCROWATTACK &rCrowAttack, SANIMCONINFO *pAnimConInfo )
{
	CComboBox* pComboBox = NULL;
	CString str;
	int nIndex;

	SANIATTACK &sAniAttack = rCrowAttack.sAniAttack;
	if ( pAnimConInfo )
	{
		rCrowAttack.strAniFile = pAnimConInfo->m_strCfgFile.c_str();
        sAniAttack.Assign ( pAnimConInfo );
	}

	rCrowAttack.m_bAnimationMove	= pAnimConInfo->IsAnimationMove();
	rCrowAttack.m_vAnimationMovePos = pAnimConInfo->m_vAnimationMoveOffset;

	//	스킬 사용시 캐릭터의 에니메이션 종류
	pComboBox = (CComboBox*) GetDlgItem ( IDC_COMBO_MAINTYPE );
	pComboBox->SetCurSel ( sAniAttack.m_MainType );

	//	스킬 사용시 캐릭터의 에니메이션 종류
	pComboBox = (CComboBox*) GetDlgItem ( IDC_COMBO_SUBTYPE );
	pComboBox->SetCurSel ( sAniAttack.m_SubType );

	//	단위시간.
	str.Format ( "%d", sAniAttack.m_UNITTIME );
	GetDlgItem(IDC_EDIT_ANI_UNITKEY)->SetWindowText ( str );	

	//	에니 시작키.
	if ( sAniAttack.m_UNITTIME!=0 )		str.Format ( "%d", sAniAttack.m_dwSTime/sAniAttack.m_UNITTIME );
	else								str = "0";
	GetDlgItem ( IDC_EDIT_ANIBEGINKEY )->SetWindowText ( str );	

	//	에니 종료키.
	if ( sAniAttack.m_UNITTIME!=0 )		str.Format ( "%d", sAniAttack.m_dwETime/sAniAttack.m_UNITTIME );
	else								str = "0";
	GetDlgItem ( IDC_EDIT_ANIENDKEY )->SetWindowText ( str );	

	CListBox* pListBox = NULL;
	pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIATTACKKEYS );
	
	nIndex = 0;
	pListBox->ResetContent();
	while ( nIndex < sAniAttack.m_wDivCount )
	{
		if ( sAniAttack.m_UNITTIME!=0 )		str.Format ( "[%d] %d", nIndex, sAniAttack.m_wDivFrame[nIndex] / sAniAttack.m_UNITTIME );
		else								str = "0";
		nIndex = pListBox->AddString ( str );
		pListBox->SetItemData ( nIndex, (DWORD)sAniAttack.m_wDivFrame[nIndex] );
		nIndex++;
	}
}

void CCrowAttack::OnCbnSelchangeComboAniAttacktype()
{
	CString strTemp;
	CComboBox* pComboBox = NULL;

	pComboBox = (CComboBox*) GetDlgItem ( IDC_COMBO_ANI_ATTACKTYPE );
	int nIndex = pComboBox->GetCurSel ();
	SANIMCONINFO *pAnimConInfo = NULL;
	if ( nIndex==LB_ERR)	return;

	pComboBox->GetWindowText ( strTemp );

	DxSkinDataDummy DataDummy;
	BOOL bOK = DataDummy.LoadFile ( m_pItem->m_sAction.m_strSkinObj.c_str() );
	if ( !bOK )
	{
		MessageBox ( "FAILURE : Fiexed selected Crow skin file can not be read.", "ERROR", MB_OK );
		return;
	}

	pAnimConInfo = DataDummy.FindAniInfo(strTemp.GetString());
	if ( pAnimConInfo )
	{
		if ( pAnimConInfo->m_MainType!=AN_ATTACK )		pAnimConInfo = NULL;
	}
	else
	{
		MessageBox ( "ERROR : Previous selected animation file is lost.", "ERROR", MB_OK );
		return;
	}

	nIndex = m_nSelectAttack;

	CROWATTACK_VEC& vecCrowAttack =
		( m_sSelectedItem.Mid() == 0 ) ?
		m_pDummyItem->m_vecCrowAttackPage.at( m_sSelectedItem.Sid() ) :
	m_pDummyItem->m_vecCrowAttackEventPage.at( m_sSelectedItem.Sid() );

	SCROWATTACK &rCrowAttack = vecCrowAttack[ nIndex ];

	UpdateAttackAni ( rCrowAttack, pAnimConInfo );
}

void CCrowAttack::OnBnClickedButtonSelfbodyeffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect File (*.effskin_a)|*.effskin_a|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_SELFBODYEFFECT )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CCrowAttack::OnBnClickedButtonTargetbodyeffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect File (*.effskin_a)|*.effskin_a|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_TARGETBODYEFFECT )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CCrowAttack::OnBnClickedButtonTargeteffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_TARGETEFFECT )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CCrowAttack::InitAllItem ()
{
	m_pItem = NULL;
}

void CCrowAttack::OnBnClickedButtonSkill()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CCrowAttack::SetReadMode ()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	const int nSkipNum = 5;
	const int nSkipID[nSkipNum] = { IDC_BUTTON_PREV, IDC_BUTTON_NEXT, IDC_BUTTON_CANCEL, 
									IDC_BUTTON_OK, IDC_LIST_CROW_ATTACK };

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

void CCrowAttack::OnBnClickedButtonBlow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	
	dlg.SetStringList( COMMENT::BLOW, EMBLOW_SIZE );	

	CROWATTACK_VEC& vecCrowAttack =
		( m_sSelectedItem.Mid() == 0 ) ?
		m_pDummyItem->m_vecCrowAttackPage.at( m_sSelectedItem.Sid() ) :
	m_pDummyItem->m_vecCrowAttackEventPage.at( m_sSelectedItem.Sid() );

	SCROWATTACK &rCrowAttack = vecCrowAttack[m_nSelectAttack];
	dlg.m_nSelect = rCrowAttack.emBLOW_TYPE;

	BOOL bSTATE_BLOW = ( rCrowAttack.emBLOW_TYPE!=EMBLOW_NONE );
	BOOL bBLOW_VAR1(TRUE), bBLOW_VAR2(TRUE);
	char *szBLOW_VAR1;
	char *szBLOW_VAR2;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strBLOW[EMBLOW_SIZE], strBLOW_VAR1[EMBLOW_SIZE], strBLOW_VAR2[EMBLOW_SIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("BLOW", strBLOW, CGameTextMan::EM_COMMENT_TEXT, EMBLOW_SIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("BLOW_VAR1", strBLOW_VAR1, CGameTextMan::EM_COMMENT_TEXT, EMBLOW_SIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("BLOW_VAR2", strBLOW_VAR2, CGameTextMan::EM_COMMENT_TEXT, EMBLOW_SIZE);
		
		dlg.SetStringList( strBLOW, EMBLOW_SIZE );
		SCROWATTACK &rCrowAttack = m_pDummyItem->m_vecCrowAttack[m_nSelectAttack];
		dlg.m_nSelect = rCrowAttack.emBLOW_TYPE;

		if ( dlg.DoModal() == IDOK )
		{
			rCrowAttack.emBLOW_TYPE = (EMSTATE_BLOW) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_BLOW, strBLOW[rCrowAttack.emBLOW_TYPE].c_str() );

			szBLOW_VAR1 = const_cast<char*>(strBLOW_VAR1[rCrowAttack.emBLOW_TYPE].c_str());
			szBLOW_VAR2 = const_cast<char*>(strBLOW_VAR2[rCrowAttack.emBLOW_TYPE].c_str());
			if ( !strcmp(szBLOW_VAR1,strBLOW_VAR1[0].c_str()) )		bBLOW_VAR1 = FALSE;
			if ( !strcmp(szBLOW_VAR2,strBLOW_VAR2[0].c_str()) )		bBLOW_VAR2 = FALSE;

			SetWin_Enable ( this, IDC_EDIT_BLOW_LIFE, bSTATE_BLOW );
			SetWin_Enable ( this, IDC_EDIT_BLOW_RATE, bSTATE_BLOW );

			SetWin_Text ( this, IDC_STATIC_BLOWVAR1, szBLOW_VAR1 );
			SetWin_Text ( this, IDC_STATIC_BLOWVAR2, szBLOW_VAR2 );
			SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE1, bBLOW_VAR1 );
			SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE2, bBLOW_VAR2 );

			SetReadMode();
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::BLOW, EMBLOW_SIZE );
		SCROWATTACK &rCrowAttack = m_pDummyItem->m_vecCrowAttack[m_nSelectAttack];
		dlg.m_nSelect = rCrowAttack.emBLOW_TYPE;

		if ( dlg.DoModal() == IDOK )
		{
			rCrowAttack.emBLOW_TYPE = (EMSTATE_BLOW) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_BLOW, COMMENT::BLOW[rCrowAttack.emBLOW_TYPE].c_str() );

			szBLOW_VAR1 = const_cast<char*>(COMMENT::BLOW_VAR1[rCrowAttack.emBLOW_TYPE].c_str());
			szBLOW_VAR2 = const_cast<char*>(COMMENT::BLOW_VAR2[rCrowAttack.emBLOW_TYPE].c_str());

			if ( !strcmp(szBLOW_VAR1,COMMENT::BLOW_VAR1[EMBLOW_NONE].c_str()) )		bBLOW_VAR1 = FALSE;
			if ( !strcmp(szBLOW_VAR2,COMMENT::BLOW_VAR2[EMBLOW_NONE].c_str()) )		bBLOW_VAR2 = FALSE;

			SetWin_Enable ( this, IDC_EDIT_BLOW_LIFE, bSTATE_BLOW );
			SetWin_Enable ( this, IDC_EDIT_BLOW_RATE, bSTATE_BLOW );

			SetWin_Text ( this, IDC_STATIC_BLOWVAR1, szBLOW_VAR1 );
			SetWin_Text ( this, IDC_STATIC_BLOWVAR2, szBLOW_VAR2 );
			SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE1, bBLOW_VAR1 );
			SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE2, bBLOW_VAR2 );

			SetReadMode();

		}
	}
}

void CCrowAttack::OnBnClickedButtonCrowAttackAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CROWATTACK_VEC& vecCrowAttack =
		( m_sSelectedItem.Mid() == 0 ) ?
		m_pDummyItem->m_vecCrowAttackPage.at( m_sSelectedItem.Sid() ) :
	m_pDummyItem->m_vecCrowAttackEventPage.at( m_sSelectedItem.Sid() );

	if ( (int)vecCrowAttack.size() >= SCROWDATA::EMMAXATTACK ) 
	{
		MessageBox ( "Max CrowAttack" );
		return;
	}

	SCROWATTACK sCrowAttack;
	vecCrowAttack.push_back( sCrowAttack );
	
	UpdateItems();
}

void CCrowAttack::OnBnClickedButtonCrowAttackDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CROWATTACK_VEC& vecCrowAttack =
		( m_sSelectedItem.Mid() == 0 ) ?
		m_pDummyItem->m_vecCrowAttackPage.at( m_sSelectedItem.Sid() ) :
	m_pDummyItem->m_vecCrowAttackEventPage.at( m_sSelectedItem.Sid() );

	int nIndex = m_listCrowAttack.GetCurSel();
	if ( nIndex < 0 ) return;
	if ( nIndex >= (int)vecCrowAttack.size() ) return;

	if ( vecCrowAttack.size() == 1 )
	{
		MessageBox( "CrowAttack must have at least one." );
		return;
	}

	CROWATTACK_VEC_ITER pos = vecCrowAttack.begin();
	vecCrowAttack.erase( pos+nIndex );
	UpdateItems();
}

void CCrowAttack::OnLbnSelchangeListCrowAttack()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateMaxattack();
	m_nSelectAttack = m_listCrowAttack.GetCurSel();
	UpdateMaxattack();
}
