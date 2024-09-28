// ItemProperty2.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../SkillEdit.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/TransformSet/GLTransformSet.h"
#include "../../RanLogic/Skill/SpecialEffect/GLSpecExperiencePoint.h"
#include "../SelectDialog.h"

#include "./SheetWithTab.h"
#include "./SkillLevelData.h"


#include "../../RanLogic/GLSkillToolTip.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

// CSkillLevelData 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSkillLevelData, CPropertyPage)
CSkillLevelData::CSkillLevelData( LOGFONT logfont )
	: CPropertyPage(CSkillLevelData::IDD)
	, m_pFont ( NULL )
	, m_nSelectLevel(0)
	, m_nSelectImpact(-1)
	, m_nSelectSpec(-1)
{
	m_bDlgInit = FALSE;
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CSkillLevelData::~CSkillLevelData()
{
	SAFE_DELETE ( m_pFont );
}

void CSkillLevelData::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IMPACT_ADDON_SEL, m_listImpactAddon );
	DDX_Control(pDX, IDC_LIST_SPEC_TYPE_SEL, m_listSpecAddon );
	DDX_Control(pDX, IDC_LIST_COUNTATTACK, m_listSpecCAOption );
	DDX_Control(pDX, IDC_SPEC_VAR_LIST, m_listSpecOption);
	DDX_Control(pDX, IDC_LIST_SPECIAL_IMMUNE, m_listSpecImmune);
}


BEGIN_MESSAGE_MAP(CSkillLevelData, CPropertyPage)		
	ON_BN_CLICKED(IDC_BUTTON_SPECIAL_IMMUNE_ADD, OnBnClickedButtonSpecImmuneAdd)
	ON_BN_CLICKED(IDC_BUTTON_SPECIAL_IMMUNE_DEL, OnBnClickedButtonSpecImmuneDel)
	ON_BN_CLICKED(IDC_BUTTON_CA_ADD, OnBnClickedButtonSpecCaAdd)
	ON_BN_CLICKED(IDC_BUTTON_CA_DEL, OnBnClickedButtonSpecCaDel)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_SPECIAL_BUTTON1, OnBnClickedSpecialButton1)
	ON_BN_CLICKED(IDC_BUTTON_LEVEL_DATA_LEFT, OnBnClickedButtonLevelDataLeft)
	ON_BN_CLICKED(IDC_BUTTON_LEVEL_DATA_RIGHT, OnBnClickedButtonLevelDataRight)
	ON_BN_CLICKED(IDC_BUTTON_LEVEL_DATA_BUNCHLEFT, OnBnClickedButtonLevelDataBunchLeft)
	ON_BN_CLICKED(IDC_BUTTON_LEVEL_DATA_BUNCHRIGHT, OnBnClickedButtonLevelDataBunchRight)
	ON_LBN_SELCHANGE(IDC_LIST_IMPACT_ADDON_SEL, OnLbnSelchangeListImpactAddonSel)
	ON_LBN_SELCHANGE(IDC_LIST_SPEC_TYPE_SEL, OnLbnSelchangeListSpecTypeSel)
	ON_LBN_SELCHANGE(IDC_LIST_COUNTATTACK, OnLbnSelchangeListSpecCAOption)
	ON_BN_CLICKED(IDC_SPECIAL_BUTTON2, OnBnClickedSpecialButton2)
	ON_EN_CHANGE(IDC_EDIT_SPEC_VAR2, &CSkillLevelData::OnEnChangeEditSpecVar2)	
	ON_BN_CLICKED(IDC_RADIO_SPEC1, &CSkillLevelData::OnBnClickedRadioSpec1)
	ON_BN_CLICKED(IDC_RADIO_SPEC2, &CSkillLevelData::OnBnClickedRadioSpec2)
	ON_BN_CLICKED(IDC_RADIO_SPEC3, &CSkillLevelData::OnBnClickedRadioSpec3)
	ON_BN_CLICKED(IDC_RADIO_SPEC4, &CSkillLevelData::OnBnClickedRadioSpec4)
    ON_EN_UPDATE(IDC_EDIT_SPEC_VAR2, &CSkillLevelData::OnEnUpdateEditSpecVar2)
    ON_EN_CHANGE(IDC_EDIT_SPEC_VAR1, &CSkillLevelData::OnEnChangeEditSpecVar1)
    ON_EN_CHANGE(IDC_EDIT_AGE, &CSkillLevelData::OnEnChangeEditAge)
    ON_BN_CLICKED(IDC_CHECK_CHK1, &CSkillLevelData::OnBnClickedCheckChk1)
    ON_BN_CLICKED(IDC_CHECK_CHK2, &CSkillLevelData::OnBnClickedCheckChk2)
    ON_BN_CLICKED(IDC_CHECK_CHK3, &CSkillLevelData::OnBnClickedCheckChk3)
    ON_BN_CLICKED(IDC_CHECK_CHK4, &CSkillLevelData::OnBnClickedCheckChk4)
    ON_BN_CLICKED(IDC_CHECK_CHK5, &CSkillLevelData::OnBnClickedCheckChk5)
    ON_BN_CLICKED(IDC_CHECK_CHK6, &CSkillLevelData::OnBnClickedCheckChk6)
    ON_BN_CLICKED(IDC_CHECK_CHK7, &CSkillLevelData::OnBnClickedCheckChk7)
    ON_BN_CLICKED(IDC_CHECK_CHK8, &CSkillLevelData::OnBnClickedCheckChk8)
	ON_BN_CLICKED(IDC_CHECK_CA_DAMAGE, &CSkillLevelData::OnBnClickedCheckCADamage)
	ON_BN_CLICKED(IDC_CHECK_CA_ALL, &CSkillLevelData::OnBnClickedCheckCAAll)
	ON_BN_CLICKED(IDC_CHECK_TARGET, &CSkillLevelData::OnBnClickedCheckTarget)
	ON_BN_CLICKED(IDC_CHECK_BUF_DEL, &CSkillLevelData::OnBnClickedCheckBufDel)

	ON_EN_CHANGE(IDC_EDIT_SKILL_MID, &CSkillLevelData::OnEnChangeEditSpecMid)
	ON_EN_CHANGE(IDC_EDIT_SKILL_SID, &CSkillLevelData::OnEnChangeEditSpecSid)
	

    ON_CBN_SELCHANGE(IDC_COMBO_REMOVE_ACTION_TYPE, &CSkillLevelData::OnCbnSelchangeComboRemoveActionType)
    ON_EN_CHANGE(IDC_EDIT_IMPACT_VAR, &CSkillLevelData::OnEnChangeEditImpactVar)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CSkillLevelData 메시지 처리기입니다.

void CSkillLevelData::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveSkillTreePage ();
}

void CSkillLevelData::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	InverseUpdateItems();

	GLSkillMan::GetInstance().SetData ( m_Item.m_sBASIC.sNATIVEID.wMainID, m_Item.m_sBASIC.sNATIVEID.wSubID, new GLSKILL(m_Item) );

	//m_pSheetTab->ActiveSkillTreePage ();

    SetWin_Text ( this, IDC_EDIT_SAVE_STATE, "Save Successed." );

    // SetTimer는 기존에 같은 ID 로 정의된 이벤트가 있으면
    // 그 이벤트를 죽이고 새로운 이벤트를 덮어씌으무로
    // 그에 따른 예외처리는 따로 하지 않습니다.
    SetTimer ( TIMER_SKILLLEVELDATA_SAVE_STATE_ID, SAVE_MESSAGE_APPEAR_TIME, NULL );
}

BOOL CSkillLevelData::SetItem ( DWORD GUID )
{
	if ( GUID == eNOT_FOUND )
	{
		return FALSE;
	}

	WORD MID = (WORD)((GUID & eMASK_0xFFFF0000) >> eSHIFT_16BIT);
	WORD SID = (WORD)((GUID & eMASK_0x0000FFFF) /*>> eBIT_16BITSHIFT*/);

	m_Item = *GLSkillMan::GetInstance().GetData ( MID, SID );	

	if ( m_bDlgInit ) 
	{
		InitDefaultCtrls ();
		UpdateItems ();		
	}

	return TRUE;
}

void CSkillLevelData::UpdateItems ()
{

	m_nSelectLevel = 0;
	m_nSelectImpact = -1;
	m_nSelectSpec = -1;

    m_listImpactAddon.ResetContent();
	m_listSpecAddon.ResetContent();		

	if ( m_Item.m_sAPPLY.vecADDON.size() > 0 )
	{
		SetWin_ShowWindow( this, IDC_STATIC_ADDON, SW_SHOW ); 
		SetWin_ShowWindow( this, IDC_STATIC_IMPACT_ADDON_VAR, SW_SHOW ); 
		SetWin_ShowWindow( this, IDC_STATIC_IMPACT_ADDON_VAR2, SW_SHOW ); // by luxes.
		SetWin_ShowWindow( this, IDC_EDIT_IMPACT_VAR, SW_SHOW );
		SetWin_ShowWindow( this, IDC_EDIT_IMPACT_VAR2, SW_SHOW ); // by luxes.
		SetWin_ShowWindow( this, IDC_LIST_IMPACT_ADDON_SEL, SW_SHOW );
        SetWin_ShowWindow( this, IDC_STATIC_IMPACT_TOOLTIP, SW_SHOW );

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			std::vector<std::string> vecIMPACT_ADDON;
			CGameTextMan::GetInstance().LoadHelpUnknownCommentString("IMPACT_ADDON", vecIMPACT_ADDON, CGameTextMan::EM_COMMENT_TEXT);
			for ( DWORD i=0 ; i < m_Item.m_sAPPLY.vecADDON.size(); ++i )
			{
				CString strNAME = vecIMPACT_ADDON[m_Item.m_sAPPLY.vecADDON[i].emADDON].c_str();
				int nIndex = m_listImpactAddon.AddString ( strNAME );
				m_listImpactAddon.SetItemData ( nIndex, i );
			}
		}
		else
		{
			for ( DWORD i=0 ; i < m_Item.m_sAPPLY.vecADDON.size(); ++i )
			{
				CString strNAME = COMMENT::IMPACT_ADDON[m_Item.m_sAPPLY.vecADDON[i].emADDON].c_str();
				int nIndex = m_listImpactAddon.AddString ( strNAME );
				m_listImpactAddon.SetItemData ( nIndex, i );
			} //for
		}

		m_nSelectImpact = 0;
		m_listImpactAddon.SetCurSel( m_nSelectImpact );
	}
	else
	{
		SetWin_ShowWindow( this, IDC_STATIC_ADDON, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_STATIC_IMPACT_ADDON_VAR, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_STATIC_IMPACT_ADDON_VAR2, SW_HIDE ); // by luxes.
		SetWin_ShowWindow( this, IDC_EDIT_IMPACT_VAR, SW_HIDE );
		SetWin_ShowWindow( this, IDC_EDIT_IMPACT_VAR2, SW_HIDE ); // by luxes.
		SetWin_ShowWindow( this, IDC_LIST_IMPACT_ADDON_SEL, SW_HIDE );
        SetWin_ShowWindow( this, IDC_STATIC_IMPACT_TOOLTIP, SW_HIDE );
	} //if..else
	
	if ( m_Item.m_sAPPLY.vecSPEC.size() > 0 )
	{
		SetWin_ShowWindow( this, IDC_STATIC_SPEC, SW_SHOW );		
		SetWin_ShowWindow( this, IDC_LIST_SPEC_TYPE_SEL, SW_SHOW );
        SetWin_ShowWindow( this, IDC_STATIC_SPEC_TOOLTIP, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_SPEC_VAR1, SW_SHOW ); 
		SetWin_ShowWindow( this, IDC_STATIC_SPEC_VAR2, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_SPEC_VAR3, SW_SHOW ); // by luxes.
		SetWin_ShowWindow( this, IDC_STATIC_SPEC_VAR4, SW_SHOW ); // by luxes.
		SetWin_ShowWindow( this, IDC_STATIC_SPEC_DWFLAG, SW_SHOW );

		SetWin_ShowWindow( this, IDC_EDIT_SPEC_VAR1, SW_SHOW ); 
		SetWin_ShowWindow( this, IDC_EDIT_SPEC_VAR2, SW_SHOW );
		SetWin_ShowWindow( this, IDC_EDIT_SPEC_VAR3, SW_SHOW ); // by luxes.
		SetWin_ShowWindow( this, IDC_EDIT_SPEC_VAR4, SW_SHOW ); // by luxes.		

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			std::vector<std::string> vecSPEC_ADDON;
			CGameTextMan::GetInstance().LoadHelpUnknownCommentString("SPEC_ADDON", vecSPEC_ADDON, CGameTextMan::EM_COMMENT_TEXT);
			for ( DWORD i=0 ; i < m_Item.m_sAPPLY.vecSPEC.size(); ++i )
			{
				CString strNAME = vecSPEC_ADDON[m_Item.m_sAPPLY.vecSPEC[i].emSPEC].c_str();
				int nIndex = m_listSpecAddon.AddString ( strNAME );
				m_listSpecAddon.SetItemData ( nIndex, i );
			}
		}
		else
		{
			for ( DWORD i=0 ; i < m_Item.m_sAPPLY.vecSPEC.size(); ++i )
			{
				CString strNAME = COMMENT::SPEC_ADDON[m_Item.m_sAPPLY.vecSPEC[i].emSPEC].c_str();
				int nIndex = m_listSpecAddon.AddString ( strNAME );
				m_listSpecAddon.SetItemData ( nIndex, i );				
			}
		}

		m_nSelectSpec = 0;
		m_listSpecAddon.SetCurSel( m_nSelectImpact );
	}
	else
	{
		SetWin_ShowWindow( this, IDC_STATIC_SPEC, SW_HIDE );		
		SetWin_ShowWindow( this, IDC_LIST_SPEC_TYPE_SEL, SW_HIDE );
        SetWin_ShowWindow( this, IDC_STATIC_SPEC_TOOLTIP, SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_SPEC_VAR1, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_STATIC_SPEC_VAR2, SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_SPEC_VAR3, SW_HIDE ); // by luxes.
		SetWin_ShowWindow( this, IDC_STATIC_SPEC_VAR4, SW_HIDE ); // by luxes.
		SetWin_ShowWindow( this, IDC_STATIC_SPEC_DWFLAG, SW_HIDE );
		SetWin_ShowWindow( this, IDC_EDIT_SPEC_VAR1, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_EDIT_SPEC_VAR2, SW_HIDE );
		SetWin_ShowWindow( this, IDC_EDIT_SPEC_VAR3, SW_HIDE ); // by luxes.
		SetWin_ShowWindow( this, IDC_EDIT_SPEC_VAR4, SW_HIDE ); // by luxes.		
		SetWin_ShowWindow( this, IDC_EDIT_SPEC_DWFLAG, SW_HIDE );

		SetWin_ShowWindow( this, IDC_SPEC_VAR_LIST, SW_HIDE ); 

		SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_CHECK_CHK2, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_CHECK_CHK3, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_CHECK_CHK4, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_CHECK_CHK5, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_CHECK_CHK6, SW_HIDE );
		SetWin_ShowWindow( this, IDC_CHECK_CHK7, SW_HIDE );
		SetWin_ShowWindow( this, IDC_CHECK_CHK8, SW_HIDE );

		SetWin_ShowWindow( this, IDC_RADIO_SPEC1, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_RADIO_SPEC2, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_RADIO_SPEC3, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_RADIO_SPEC4, SW_HIDE ); 

		SetWin_ShowWindow( this, IDC_EDIT_DEFENSE_SKILL_MID, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_EDIT_DEFENSE_SKILL_SID, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_EDIT_DEFENSE_SKILL_LEVEL, SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_DEFENSE_LEVEL, SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_DEFENSE_SKILL, SW_HIDE );		

		SetWin_ShowWindow( this, IDC_EDIT_ONWARD_SUBANI, SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_ONWARD_SUBANI, SW_HIDE );

		SetWin_ShowWindow( this, IDC_STATIC_REMOVE_ACTION_TYPE, SW_HIDE );
		SetWin_ShowWindow( this, IDC_COMBO_REMOVE_ACTION_TYPE, SW_HIDE );
		SetWin_ShowWindow( this, IDC_COMBO_DURATION_ATTB, SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_DURATION_ATTB, SW_HIDE );
		
		SetWin_ShowWindow( this, IDC_COMBO_INCREASE_EFF, SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_INCREASE_EFF, SW_HIDE );

		SetWin_ShowWindow( this, IDC_EDIT_TRIGGER_TYPE_LINK_MID, SW_HIDE );
		SetWin_ShowWindow( this, IDC_EDIT_TRIGGER_TYPE_LINK_SID, SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_TRIGGER_TYPE_LINK, SW_HIDE ); 

		SetWin_ShowWindow( this, IDC_STATIC_TF_FORM_TYPE, SW_HIDE );
		
		SetWin_ShowWindow( this, IDC_STATIC_SKILL_MID, SW_HIDE);
		SetWin_ShowWindow( this, IDC_STATIC_SKILL_SID, SW_HIDE);
		SetWin_ShowWindow( this, IDC_EDIT_SKILL_MID  , SW_HIDE);
		SetWin_ShowWindow( this, IDC_EDIT_SKILL_SID  , SW_HIDE);

		SetWin_ShowWindow( this, IDC_STATIC_SPECIAL_IMMUNE, SW_HIDE );
		SetWin_ShowWindow( this, IDC_LIST_SPECIAL_IMMUNE, SW_HIDE);
		SetWin_ShowWindow( this, IDC_BUTTON_SPECIAL_IMMUNE_ADD, SW_HIDE);
		SetWin_ShowWindow( this, IDC_BUTTON_SPECIAL_IMMUNE_DEL  , SW_HIDE);

		SetWin_ShowWindow( this, IDC_CHECK_BUF_DEL  , SW_HIDE);
		SetWin_ShowWindow( this, IDC_CHECK_TARGET  , SW_HIDE);

		SetWin_ShowWindow( this, IDC_CHECK_CA_DAMAGE  , SW_HIDE);
		SetWin_ShowWindow( this, IDC_CHECK_CA_ALL  , SW_HIDE);

		SetWin_ShowWindow( this, IDC_STATIC_MAN  , SW_HIDE);
		SetWin_ShowWindow( this, IDC_STATIC_WOMAN  , SW_HIDE);
	} //if..else

	UpdateSkillLevelData ();	
}

BOOL CSkillLevelData::OnInitDialog()
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

void CSkillLevelData::InverseUpdateItems ()
{
	//	이 부분은 특성상 바로바로 업데이트 한다.	
	//	하지만, 한번도 DropDown을 이용해 아이템을 바꾸지 않으면,
	//	저장되지 않기 때문에, 마지막에 데이타를 갱신한놈은
	//	따로 처리해주는 것이다.
	InverseUpdateSkillLevelData ();
}

void CSkillLevelData::OnBnClickedButtonPrev()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//InverseUpdateItems();

	//GLSkillMan::GetInstance().SetData ( m_Item.m_sBASIC.sNATIVEID.wMainID,
	//	m_Item.m_sBASIC.sNATIVEID.wSubID, new GLSKILL(m_Item) );

	DWORD CombineID =
		(
			(DWORD)(m_Item.m_sBASIC.sNATIVEID.wMainID & eMASK_0x0000FFFF) << eSHIFT_16BIT |
			(DWORD)(m_Item.m_sBASIC.sNATIVEID.wSubID  & eMASK_0x0000FFFF) /*<< eSHIFT_16BIT |*/
		);

	m_pSheetTab->m_SkillDataPage.SetCallPage ( m_CallPage );
	m_pSheetTab->m_SkillDataPage.SetItem ( CombineID );
	m_pSheetTab->SetActivePage ( SKILLDATAPAGE );
}

void CSkillLevelData::InitDefaultCtrls ()
{

	GetDlgItem ( IDC_BUTTON_NEXT )->EnableWindow ( FALSE );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strSKILLACTIONTYPE[SKILL::EMACTION_TYPE_NSIZE], strELEMENT[EMELEMENT_MAXNUM];
		CGameTextMan::GetInstance().LoadHelpCommentString("SKILL_ACTION_TYPE", strSKILLACTIONTYPE, CGameTextMan::EM_COMMENT_TEXT, SKILL::EMACTION_TYPE_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("ELEMENT", strELEMENT, CGameTextMan::EM_COMMENT_TEXT, EMELEMENT_MAXNUM);

		SetWin_Combo_Init(this, IDC_COMBO_REMOVE_ACTION_TYPE, strSKILLACTIONTYPE, SKILL::EMACTION_TYPE_NSIZE);
		SetWin_Combo_Init(this, IDC_COMBO_DURATION_ATTB, strELEMENT, EMELEMENT_MAXNUM);
	}
	else
	{
		SetWin_Combo_Init ( this, IDC_COMBO_REMOVE_ACTION_TYPE, COMMENT::SKILL_ACTION_TYPE, SKILL::EMACTION_TYPE_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_DURATION_ATTB, COMMENT::ELEMENT, EMELEMENT_MAXNUM );	
	}
}


void CSkillLevelData::InverseUpdateSkillLevelData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD dwSLevel = m_nSelectLevel;	

	//	기본 
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].fDELAYTIME = GetWin_Num_float ( this, IDC_EDIT_DELAYTIME );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].fLIFE = GetWin_Num_float ( this, IDC_EDIT_AGE );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wAPPLYRANGE = GetWin_Num_int ( this, IDC_EDIT_APPLYRANGE );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wAPPLYNUM = GetWin_Num_int ( this, IDC_EDIT_APPLYNUM );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wAPPLYANGLE = GetWin_Num_int ( this, IDC_EDIT_APPLYANGLE );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wPIERCENUM = GetWin_Num_int ( this, IDC_EDIT_PIERCENUM );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].fBASIC_VAR = GetWin_Num_float ( this, IDC_EDIT_VARIATION );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wTARNUM = GetWin_Num_int ( this, IDC_EDIT_TARNUM );

	//	사용조건
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_HP = GetWin_Num_int ( this, IDC_EDIT_DEC_HP );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_MP = GetWin_Num_int ( this, IDC_EDIT_DEC_MP );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_SP = GetWin_Num_int ( this, IDC_EDIT_DEC_SP );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_CP = GetWin_Num_int ( this, IDC_EDIT_DEC_CP );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_EXP = GetWin_Num_int ( this, IDC_EDIT_DEC_EXP );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_ARROWNUM = GetWin_Num_int ( this, IDC_EDIT_DEC_ARROW );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_CHARMNUM = GetWin_Num_int ( this, IDC_EDIT_DEC_CHARM );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_BULLETNUM = GetWin_Num_int ( this, IDC_EDIT_DEC_BULLET );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].nUSE_BATTERY = GetWin_Num_int ( this, IDC_EDIT_DEC_BATTERY );
	
	//	사용조건 PARTY
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_HP_PTY = GetWin_Num_int ( this, IDC_EDIT_DEC_HP_PTY );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_MP_PTY = GetWin_Num_int ( this, IDC_EDIT_DEC_MP_PTY );
	m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_SP_PTY = GetWin_Num_int ( this, IDC_EDIT_DEC_SP_PTY );

	//	습득조건(1가지만 충족할 경우 유무)
	m_Item.m_sLEARN.bOnlyOneStats = GetWin_Check ( this, IDC_CHECK_REQ_ONLY_ONE_STAT );

	//	습득 요구치
	m_Item.m_sLEARN.sLVL_STEP[dwSLevel].dwLEVEL = GetWin_Num_int ( this, IDC_EDIT_REQ_LEVEL );
	m_Item.m_sLEARN.sLVL_STEP[dwSLevel].dwSKP = GetWin_Num_int ( this, IDC_EDIT_REQ_EXP );
	m_Item.m_sLEARN.sLVL_STEP[dwSLevel].dwSKILL_LVL = GetWin_Num_int ( this, IDC_EDIT_REQ_SKILL_LEVEL );

	m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wPow = GetWin_Num_int ( this, IDC_EDIT_REQ_POW );
	m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wStr = GetWin_Num_int ( this, IDC_EDIT_REQ_STR );

	m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wSpi = GetWin_Num_int ( this, IDC_EDIT_REQ_SPI );
	m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wDex = GetWin_Num_int ( this, IDC_EDIT_REQ_DEX );

	m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wInt = GetWin_Num_int ( this, IDC_EDIT_REQ_INT );
	m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wSta = GetWin_Num_int ( this, IDC_EDIT_REQ_STA );
	m_Item.m_sLEARN.sLVL_STEP[dwSLevel].dwReqMoney = GetWin_Num_LONGLONG( this, IDC_EDIT_REQ_MONEY );

	//	상태이상
	m_Item.m_sAPPLY.sSTATE_BLOW[dwSLevel].fRATE = GetWin_Num_float ( this, IDC_EDIT_BLOW_RATE );
	m_Item.m_sAPPLY.sSTATE_BLOW[dwSLevel].fVAR1 = GetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE1 );
	m_Item.m_sAPPLY.sSTATE_BLOW[dwSLevel].fVAR2 = GetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE2 );

	SKILL::SSTATE_BLOW &sBLOW = m_Item.m_sAPPLY.sSTATE_BLOW[dwSLevel];
	switch ( m_Item.m_sAPPLY.emSTATE_BLOW )
	{
	case EMBLOW_NUMB:
		//	이동속도.
		LIMIT ( sBLOW.fVAR1, 1.0f, -1.0f );
		//	딜래이
		LIMIT ( sBLOW.fVAR2, 10.0f, -1.0f );
		break;
	case EMBLOW_STUN:
		break;
	case EMBLOW_STONE:
		//	이동속도.
		LIMIT ( sBLOW.fVAR1, 1.0f, -1.0f );
		break;
	case EMBLOW_BURN:
		break;
	case EMBLOW_FROZEN:
		//	이동속도.
		LIMIT ( sBLOW.fVAR1, 1.0f, -1.0f );
		//	타격증폭
		LIMIT ( sBLOW.fVAR2, 10.0f, -1.0f );
		break;

	case EMBLOW_MAD:
		break;
	case EMBLOW_POISON:
		break;
	case EMBLOW_CURSE:	
		break;
	};


	InverseUpdateImpactAddon();
	InverseUpdateSpecAddon();
}

void CSkillLevelData::InverseUpdateImpactAddon()
{
	DWORD dwSLevel = m_nSelectLevel;

	if ( m_nSelectImpact < 0 ) return;
	if ( m_nSelectImpact >= static_cast<int>( m_Item.m_sAPPLY.vecADDON.size() ) ) return;

	SKILL::SIMPACT_ADDON& sImpactAddon = m_Item.m_sAPPLY.vecADDON[m_nSelectImpact];
    	
	//	부가효과
	sImpactAddon.fADDON_VAR[dwSLevel] = GetWin_Num_float ( this, IDC_EDIT_IMPACT_VAR );
	sImpactAddon.fRate[dwSLevel] = GetWin_Num_float( this, IDC_EDIT_IMPACT_VAR2 ) * 0.01f; // 퍼센트 값을 소수로.

	float& fRate = sImpactAddon.fRate[dwSLevel];
	LIMIT( fRate, 100.0f, -100.0f ); // -10000% ~ 10000% 까지.

	float &fADDON_VAR = sImpactAddon.fADDON_VAR[dwSLevel];
	switch ( sImpactAddon.emADDON )
	{
	case EMIMPACTA_VARHP:
		LIMIT ( fADDON_VAR, 10.0f, -10.0f );
		break;

	case EMIMPACTA_VARMP:
		LIMIT ( fADDON_VAR, 10.0f, -10.0f );
		break;

	case EMIMPACTA_VARSP:
		LIMIT ( fADDON_VAR, 10.0f, -10.0f );
		break;

	case EMIMPACTA_VARAP:
		LIMIT ( fADDON_VAR, 10.0f, -10.0f );
		break;

	case EMIMPACTA_DAMAGE_RATE:
		LIMIT ( fADDON_VAR, 10.0f, -10.0f );
		break;
	case EMIMPACTA_DEFENSE_RATE:
		LIMIT ( fADDON_VAR, 10.0f, -10.0f );
		break;
	};

}

void CSkillLevelData::InverseUpdateSpecAddon()
{
	DWORD dwSLevel = m_nSelectLevel;

	if ( m_nSelectSpec < 0 )
		return;
	if ( m_nSelectSpec >= static_cast<int>( m_Item.m_sAPPLY.vecSPEC.size() ) )
		return;

	SKILL::SSPEC_ADDON& sSpecAddon = m_Item.m_sAPPLY.vecSPEC[m_nSelectSpec];
	sSpecAddon.sSPEC[dwSLevel].dwFLAG = 0x00;

	//	특수기능
	sSpecAddon.sSPEC[dwSLevel].fVAR1 = GetWin_Num_float ( this, IDC_EDIT_SPEC_VAR1 );
	sSpecAddon.sSPEC[dwSLevel].fVAR2 = GetWin_Num_float ( this, IDC_EDIT_SPEC_VAR2 );
	sSpecAddon.sSPEC[dwSLevel].fRate = GetWin_Num_float( this, IDC_EDIT_SPEC_VAR3 ) * 0.01f;
	sSpecAddon.sSPEC[dwSLevel].fRate2 = GetWin_Num_float( this, IDC_EDIT_SPEC_VAR4 ) * 0.01f;
	sSpecAddon.sSPEC[dwSLevel].dwFLAG = GetWin_Num_int( this, IDC_EDIT_SPEC_DWFLAG );

	switch ( sSpecAddon.emSPEC )
	{ 
	case EMSPECA_ACTIVATE_DAMAGE :
		{
			sSpecAddon.sSPEC[dwSLevel].nVAR1 = GetWin_Num_int( this, IDC_EDIT_SPEC_VAR1 );
			sSpecAddon.sSPEC[dwSLevel].nVAR2 = GetWin_Num_int( this, IDC_EDIT_SPEC_VAR2 );
			sSpecAddon.sSPEC[dwSLevel].dwNativeID.wMainID = GetWin_Num_int ( this, IDC_EDIT_SKILL_MID );
			sSpecAddon.sSPEC[dwSLevel].dwNativeID.wSubID = GetWin_Num_int ( this, IDC_EDIT_SKILL_SID );
		}
		break;
	case EMSPECA_SPECIAL_IMMUNE :
		{
			sSpecAddon.sSPEC[dwSLevel].nVAR1 = GetWin_Num_int( this, IDC_EDIT_SPEC_VAR1 );
			sSpecAddon.sSPEC[dwSLevel].nVAR2 = GetWin_Num_int( this, IDC_EDIT_SPEC_VAR2 );

			if( m_listSpecImmune.GetCount() > 0 )
			{
				EMSPEC_ADDON eData = (EMSPEC_ADDON)m_listSpecImmune.GetItemData(0);

				sSpecAddon.sSPEC[dwSLevel].nVAR1 = int(eData);
			}

			if( dwSLevel == 0 && m_listSpecImmune.GetCount() > 0 )
			{
				for( int i = 1; i < SKILL::MAX_LEVEL ; ++i )
				{
					sSpecAddon.sSPEC[i].nVAR1 = sSpecAddon.sSPEC[0].nVAR1;
				}
			}
			else
			if( dwSLevel > 0 && m_listSpecImmune.GetCount() == 0 )
			{
				EMSPEC_ADDON eData = (EMSPEC_ADDON)(sSpecAddon.sSPEC[0].nVAR1);

				if( eData != EMSPECA_NULL )
				{
					CString strNAME = COMMENT::SPEC_ADDON[eData].c_str();
					int nIndex = m_listSpecImmune.AddString ( strNAME );
					m_listSpecImmune.SetItemData ( nIndex, eData );

					SetWin_Num_int( this, IDC_EDIT_SPEC_VAR1, (int)eData );
				}
			}

		}
		break;
	case EMSPECA_DISGUISE :
		{
			sSpecAddon.sSPEC[dwSLevel].fVAR2 = float(GetWin_Combo_Sel(this, IDC_COMBO_REMOVE_ACTION_TYPE));

			//SetWin_Num_float( this, IDC_EDIT_SPEC_VAR2, sSpecAddon.sSPEC[dwSLevel].fVAR2 );
		}
		break;
	case EMSPECA_FEIGN_DEATH   :
		{
			SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TARGET  ), sSpecAddon.sSPEC[0].dwFLAG, FEIGN_DEATH_TARGETING_DISABLE );
			SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_BUF_DEL ), sSpecAddon.sSPEC[0].dwFLAG, FEIGN_DEATH_BUF_DEL );

			for( int i = 1; i < SKILL::MAX_LEVEL ; ++i )
			{
				sSpecAddon.sSPEC[i].dwFLAG = sSpecAddon.sSPEC[0].dwFLAG;

				SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TARGET  ), sSpecAddon.sSPEC[i].dwFLAG, FEIGN_DEATH_TARGETING_DISABLE );
				SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_BUF_DEL ), sSpecAddon.sSPEC[i].dwFLAG, FEIGN_DEATH_BUF_DEL );				
			}
			//SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TARGET  ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, FEIGN_DEATH_TARGETING_DISABLE );
			//SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_BUF_DEL ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, FEIGN_DEATH_BUF_DEL );
		}
		break;
	case EMSPECA_ENTRANCE_EXIT :
		{			
			for( int i = 1; i < SKILL::MAX_LEVEL ; ++i )
			{
				sSpecAddon.sSPEC[i].fVAR1 = sSpecAddon.sSPEC[0].fVAR1;
				sSpecAddon.sSPEC[i].fVAR2 = sSpecAddon.sSPEC[0].fVAR2;
			}				
		}
		break;
	case EMSPECA_COUNTATTACK :
		{
			m_Item.m_sAPPLY.sCAData.sSPEC_CA[dwSLevel].bDamageApply = GetWin_Check ( this, IDC_CHECK_CA_DAMAGE );
			m_Item.m_sAPPLY.sCAData.sSPEC_CA[dwSLevel].bAllApply    = GetWin_Check ( this, IDC_CHECK_CA_ALL );
		}
		break;
	case EMSPECA_NONBLOW:
	case EMSPECA_RECBLOW:
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK1 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, DIS_NUMB );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK2 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, DIS_STUN );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK3 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, DIS_STONE );

		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK4 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, DIS_BURN );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK5 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, DIS_FROZEN );

		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK6 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, DIS_MAD );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK7 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, DIS_POISON );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK8 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, DIS_CURSE );
		break;
	case EMSPECA_DEFENSE_SKILL_ACTIVE:
		sSpecAddon.sSPEC[dwSLevel].dwNativeID.wMainID = GetWin_Num_int ( this, IDC_EDIT_DEFENSE_SKILL_MID );
		sSpecAddon.sSPEC[dwSLevel].dwNativeID.wSubID = GetWin_Num_int ( this, IDC_EDIT_DEFENSE_SKILL_SID );
		sSpecAddon.sSPEC[dwSLevel].dwFLAG = GetWin_Num_int ( this, IDC_EDIT_DEFENSE_SKILL_LEVEL );
		break;
	case EMSPECA_ONWARD:
		sSpecAddon.sSPEC[dwSLevel].dwFLAG = GetWin_Num_int ( this, IDC_EDIT_ONWARD_SUBANI );
		break;
	case EMSPECA_EFFECT_REMOVE:
		sSpecAddon.sSPEC[dwSLevel].dwFLAG = static_cast<SKILL::EMACTION_TYPE> ( GetWin_Combo_Sel ( this, IDC_COMBO_REMOVE_ACTION_TYPE ) );	
		break;
	case EMSPECA_DURATION_DAMAGE:
		sSpecAddon.sSPEC[dwSLevel].dwFLAG = GetWin_Combo_Sel ( this, IDC_COMBO_DURATION_ATTB );
		break;
	case EMSPECA_INCREASE_EFF:		
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK1 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			SKILL::EMAPPLY_DWORD_MELEE );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK2 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			SKILL::EMAPPLY_DWORD_RANGE );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK3 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			SKILL::EMAPPLY_DWORD_MAGIC );
		break;
	case EMSPECA_INVISIBLE:
		SetCheck_Flags(GetWin_Check(this, IDC_CHECK_CHK1), sSpecAddon.sSPEC[dwSLevel].dwFLAG, true);
		break;

	case EMSPECA_SWAPPOS:
		int i;
		for( i = 0; i < EMSPEC_SWAPPOS_DIR_NSIZE; i++ )
		{
			if( GetWin_Check( this, IDC_RADIO_SPEC1+i ) )
				break;
		} //for
		sSpecAddon.sSPEC[dwSLevel].dwFLAG = i;
		break;

	case EMSPECA_ILLUSION:
		sSpecAddon.sSPEC[dwSLevel].nVAR1 = GetWin_Num_int( this, IDC_EDIT_SPEC_VAR1 );
		sSpecAddon.sSPEC[dwSLevel].nVAR2 = GetWin_Num_int( this, IDC_EDIT_SPEC_VAR2 );
		break;

	case EMSPECA_RELEASE_ENDURE:
		sSpecAddon.sSPEC[dwSLevel].dwFLAG = 0x0;
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK1 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			EMSPEC_RELEASE_ENDURE_TYPE_DWORD_NORMAL );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK1 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			EMSPEC_RELEASE_ENDURE_TYPE_DWORD_SKILL );		
		break;
	case EMSPECA_ENDURE_VARY:
        SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK1 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
            SKILL::EMACTION_TYPE_DWORD_NORMAL );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK2 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			SKILL::EMACTION_TYPE_DWORD_BUFF );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK3), sSpecAddon.sSPEC[dwSLevel].dwFLAG,
			SKILL::EMACTION_TYPE_DWORD_DEBUFF );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK4), sSpecAddon.sSPEC[dwSLevel].dwFLAG,
			SKILL::EMACTION_TYPE_DWORD_LIMIT);
		break;
	case EMSPECA_IMMUNE:
		{
			DWORD dwVAR1 = 0x0;
			SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK1 ), dwVAR1, SKILL::EMAPPLY_DWORD_MELEE );
			SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK2), dwVAR1, SKILL::EMAPPLY_DWORD_RANGE );
			SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK3), dwVAR1, SKILL::EMAPPLY_DWORD_MAGIC);
			sSpecAddon.sSPEC[dwSLevel].fVAR1 = static_cast<float>(dwVAR1);

			DWORD dwVAR2 = 0x0;
			SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK5 ), dwVAR2, SKILL::EMACTION_TYPE_DWORD_NORMAL );
			SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK6), dwVAR2, SKILL::EMACTION_TYPE_DWORD_BUFF );
			SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK7), dwVAR2, SKILL::EMACTION_TYPE_DWORD_DEBUFF);
			SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK8), dwVAR2, SKILL::EMACTION_TYPE_DWORD_LIMIT);
			sSpecAddon.sSPEC[dwSLevel].fVAR2 = static_cast<float>(dwVAR2);
		}		
		break;
	case EMSPECA_SKILLDELAY:
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK1 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			EMSPEC_SKILL_ASING_DWORD_BEFORE );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK2), sSpecAddon.sSPEC[dwSLevel].dwFLAG,
			EMSPEC_SKILL_ASING_DWORD_AFTER );
		break;
	case EMSPECA_LINK_HP:
		{
			DWORD dwFLAG = FALSE;
			SetCheck_Flags(GetWin_Check(this, IDC_CHECK_CHK1), dwFLAG, TRUE);
			sSpecAddon.sSPEC[dwSLevel].fVAR2 = float(dwFLAG);
		}		
		break;
    case EMSPECA_VEHICLE_GETOFF:
        {
            DWORD dwFLAG = FALSE;
            SetCheck_Flags(GetWin_Check(this, IDC_CHECK_CHK1), dwFLAG, TRUE);
            sSpecAddon.sSPEC[dwSLevel].fVAR2 = float(dwFLAG);
        }		
        break;
	case EMSPECA_STIGMA:
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK1), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			EMSPEC_STIGMA_TYPE_DWORD_DAMAGE_NORMAL );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK2), sSpecAddon.sSPEC[dwSLevel].dwFLAG,
			EMSPEC_STIGMA_TYPE_DWORD_DAMAGE_SKILL );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK3), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			EMSPEC_STIGMA_TYPE_DWORD_BUFF_SKILL);
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK4), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			EMSPEC_STIGMA_TYPE_DWORD_DEBUFF_SKILL);
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK5), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			EMSPEC_STIGMA_TYPE_DWORD_LIMIT_SKILL);
		break;
	case EMSPECA_HALLUCINATE:
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK1 ), sSpecAddon.sSPEC[dwSLevel].dwFLAG, 
			EMSPEC_HALLUCINATE_DWORD_TRANSFORM);
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK2), sSpecAddon.sSPEC[dwSLevel].dwFLAG,
			EMSPEC_HALLUCINATE_DWORD_EMOTICON );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CHK3), sSpecAddon.sSPEC[dwSLevel].dwFLAG,
			EMSPEC_HALLUCINATE_DWORD_NONTARGETABLE );		
		sSpecAddon.sSPEC[dwSLevel].fVAR1 = float(GetWin_Combo_Sel(this, IDC_COMBO_REMOVE_ACTION_TYPE));
		break;
	case EMSPECA_DOMINATION:
		if ( GetWin_Check ( this, IDC_RADIO_SPEC2 ) )
			sSpecAddon.sSPEC[dwSLevel].dwFLAG = EMSPEC_DOMINATE_TYPE_CASTER;
		else if ( GetWin_Check ( this, IDC_RADIO_SPEC3 ) )
			sSpecAddon.sSPEC[dwSLevel].dwFLAG = EMSPEC_DOMINATE_TYPE_RANDOM;
		else if ( GetWin_Check ( this, IDC_RADIO_SPEC4 ) )
			sSpecAddon.sSPEC[dwSLevel].dwFLAG = EMSPEC_DOMINATE_TYPE_FIX;
		else
			sSpecAddon.sSPEC[dwSLevel].dwFLAG = EMSPEC_DOMINATE_TYPE_RANDOM;
		break;
	case EMSPECA_SKILL_LINK:
		sSpecAddon.sSPEC[dwSLevel].nVAR2 = GetWin_Num_int( this, IDC_EDIT_SPEC_VAR2 );

		for( int i=0; i<SKILL::MAX_LEVEL; ++i )
		{
			sSpecAddon.sSPEC[i].dwNativeID.wMainID = GetWin_Num_int ( this, IDC_EDIT_DEFENSE_SKILL_MID );
			sSpecAddon.sSPEC[i].dwNativeID.wSubID = GetWin_Num_int ( this, IDC_EDIT_DEFENSE_SKILL_SID );

			sSpecAddon.sSPEC[i].dwLinkID.wMainID = GetWin_Num_int ( this, IDC_EDIT_TRIGGER_TYPE_LINK_MID );
			sSpecAddon.sSPEC[i].dwLinkID.wSubID = GetWin_Num_int ( this, IDC_EDIT_TRIGGER_TYPE_LINK_SID );
		}
		break;
	case EMSPECA_AIRBORNE:
		if ( GetWin_Check ( this, IDC_CHECK_CHK1) )
			sSpecAddon.sSPEC[dwSLevel].dwFLAG = true;
		else
			sSpecAddon.sSPEC[dwSLevel].dwFLAG = false;
		break;

	case EMSPECA_SUMMON_CHANGE_SKILLPAGE:
	case EMSPECA_SUMMON_EVENT_SKILL:
	case EMSPECA_SUMMON_TARGETING:
	case EMSPECA_SUMMON_RUNAWAY:
    case EMSPECA_MOVEVELO_LIMIT:
		break;

	case EMSPECA_REMOVE_SKILL_EFFECT:
		{
			sSpecAddon.sSPEC[dwSLevel].nVAR1 = GetWin_Num_int( this, IDC_EDIT_SPEC_VAR1 );

			// 리스트에서 가져온 데이터 세팅;
			if( m_listSpecImmune.GetCount() > 0 )
			{
				EMSPEC_ADDON eData = (EMSPEC_ADDON)m_listSpecImmune.GetItemData(0);

				sSpecAddon.sSPEC[dwSLevel].nVAR1 = int(eData);
			}

			// 1레벨에 세팅한 값은 모든 레벨에 같은값 세팅한다;
			if( dwSLevel == 0 && m_listSpecImmune.GetCount() > 0 )
			{
				for( int i = 1; i < SKILL::MAX_LEVEL ; ++i )
				{
					sSpecAddon.sSPEC[i].nVAR1 = sSpecAddon.sSPEC[0].nVAR1;
				}
			}
			else
			{
				if( dwSLevel > 0 && m_listSpecImmune.GetCount() == 0 )
				{
					EMSPEC_ADDON eData = (EMSPEC_ADDON)(sSpecAddon.sSPEC[0].nVAR1);

					if( eData != EMSPECA_NULL )
					{
						CString strNAME = COMMENT::SPEC_ADDON[eData].c_str();
						int nIndex = m_listSpecImmune.AddString ( strNAME );
						m_listSpecImmune.SetItemData ( nIndex, eData );

						SetWin_Num_int( this, IDC_EDIT_SPEC_VAR1, (int)eData );
					}
				}
			}
		}
		break;
	}	

	SKILL::SSPEC &sSPEC = sSpecAddon.sSPEC[dwSLevel];

	if ( m_Item.m_sBASIC.bMobEffectRate )
	{
		float& fRate = sSPEC.fRate;
		LIMIT( fRate, 100.0f, -100.0f ); // -10000% ~ 10000% 까지.

		float& fRate2 = sSPEC.fRate2;
		LIMIT( fRate2, 100.0f, -100.0f ); // -10000% ~ 10000% 까지.
	}

	switch ( sSpecAddon.emSPEC )
	{
	case EMSPECA_REBIRTH:
		LIMIT ( sSPEC.fVAR1, 1.0f, 0.0f );   // 부활후 회복률
		break;

	case EMSPECA_PUSHPULL:
		//	이동속도.
		LIMIT ( sSPEC.fVAR1, 100.0f, -100.0f );	//	밀거나 당기는 거리.
		LIMIT ( sSPEC.fVAR2, 1.0f, 0.0f );		//	발생 확율.
		break;

	case EMSPECA_MOVEVELO:
		//	이동속도.
		LIMIT ( sSPEC.fVAR1, 4.0f, -1.0f );
		break;

	case EMSPECA_HP_GATHER:
		LIMIT ( sSPEC.fVAR1, 0.99f, 0.00f );
		break;

	case EMSPECA_MP_GATHER:
		LIMIT ( sSPEC.fVAR1, 0.99f, 0.00f );
		break;

	case EMSPECA_SP_GATHER:
		LIMIT ( sSPEC.fVAR1, 0.99f, 0.00f );
		break;

	case EMSPECA_PSY_DAMAGE_REDUCE:
		LIMIT ( sSPEC.fVAR1, 1.0f, -1.0f );
		break;
	case EMSPECA_MAGIC_DAMAGE_REDUCE:
		LIMIT ( sSPEC.fVAR1, 1.0f, -1.0f );
		break;
	
	case EMSPECA_PSY_DAMAGE_REFLECTION:
		LIMIT ( sSPEC.fVAR1, 1.0f, -1.0f );
		LIMIT ( sSPEC.fVAR2, 1.0f, 0.0f );
		break;
	
	case EMSPECA_MAGIC_DAMAGE_REFLECTION:
		LIMIT ( sSPEC.fVAR1, 1.0f, -1.0f );
		LIMIT ( sSPEC.fVAR2, 1.0f, 0.0f );
		break;
	case EMSPECA_DEFENSE_SKILL_ACTIVE:
		LIMIT ( sSPEC.fVAR1, 1.0f, 0.0f );
		LIMIT ( sSPEC.dwFLAG, (DWORD)8, (DWORD)0 );
		break;
	case EMSPECA_PULLIN:
		LIMIT ( sSPEC.fVAR1, 1.0f, 0.0f );		//	발생 확율.
		LIMIT ( sSPEC.fVAR2, 1000.0f, 0.0f );	//	이동 속도
		break;
	case EMSPECA_ONWARD:
		LIMIT ( sSPEC.fVAR1, 1.0f, 0.0f );		//	발생 확율.
		LIMIT ( sSPEC.fVAR2, 1000.0f, 0.0f );	//	이동 속도
		break;
	case EMSPECA_KNOCKBACK:
		LIMIT ( sSPEC.fVAR1, 1.0f, 0.0f );		//	발생 확율.
		LIMIT ( sSPEC.fVAR2, 100.0f, 0.0f );	//	거리
		break;
    case EMSPECA_VEHICLE_GETOFF:  //봉주
        LIMIT ( sSPEC.fVAR1, 1.0f, 0.0f );		//	발생 확율.
        LIMIT ( sSPEC.fVAR2, 1.0f, 0.0f );	    //	지속 여부
        break;
    case EMSPECA_ENDURE_VARY:  //봉주
        LIMIT ( sSPEC.fVAR1, 10.0f, 0.0f );		//	효과값 1비율.
        LIMIT ( sSPEC.fVAR2, 10.0f, 0.0f );	    //	효과값 2비율.
        LIMIT ( sSPEC.dwFLAG, (DWORD)15, (DWORD)0 );
        break;
	case EMSPECA_DURATION_DAMAGE:
		LIMIT ( sSPEC.dwFLAG, (DWORD)EMELEMENT_MAXNUM, (DWORD)EMELEMENT_SPIRIT );
		break;
	case EMSPECA_CURSE:
		LIMIT ( sSPEC.fVAR1, 1.0f, -1.0f );
		break;
	case EMSPECA_EFFECT_REMOVE:
		LIMIT ( sSPEC.dwFLAG, (DWORD)SKILL::EMACTION_TYPE_NSIZE, (DWORD)SKILL::EMACTION_TYPE_NORMAL );
		break;
	case EMSPECA_INVISIBLE:
		LIMIT( sSPEC.dwFLAG, (DWORD)1, (DWORD)0);
		break;
	case EMSPECA_RELEASE_ENDURE:		
		LIMIT(float(sSPEC.fVAR1), 1.0f, 0.0f);		
		break;
	case EMSPECA_RELEASE_PET:
		LIMIT ( sSPEC.fVAR1, 1.0f, 0.0f );
		LIMIT ( sSPEC.fVAR2, 1.0f, 0.0f );
		break;
	case EMSPECA_STIGMA:
		{
			const float fDistance = static_cast<float>(GLCONST_CHAR::dwReleaseStigmaDistance);
			LIMIT( sSPEC.fVAR1, fDistance, 0.0f);		
		}		
		break;
	case EMSPECA_INCREASE_EFF:
		LIMIT(sSPEC.fVAR1, 1.0f, -1.0f);
		break;
	case EMSPECA_DOMINATION:
		LIMIT(sSPEC.fVAR1, 1.0f, 0.0f);
		break;
	case EMSPECA_RANDOM_EXP_RATE:
		break;
	case EMSPECA_SKILL_LINK:
		LIMIT ( sSPEC.fVAR1, 1.0f, 0.0f );
		break;
	case EMSPECA_AIRBORNE:
		LIMIT ( sSPEC.fVAR1, m_Item.m_sAPPLY.sDATA_LVL[m_nSelectLevel].fLIFE * 0.95f, 0.1f);
		break;
	case EMSPECA_SUMMON_CHANGE_SKILLPAGE:
		LIMIT ( sSPEC.fVAR1, 4.0f, 0.0f );		// 기본 슬롯;
		break;
	case EMSPECA_SUMMON_EVENT_SKILL:
		LIMIT ( sSPEC.fVAR1, 9.0f, 0.0f );		// 기본 슬롯;
		break;
	case EMSPECA_SUMMON_TARGETING:
	case EMSPECA_SUMMON_RUNAWAY:
        break;
    case EMSPECA_MOVEVELO_LIMIT:
        LIMIT ( sSPEC.fVAR1, 1000.0f, -1.0f );
        LIMIT ( sSPEC.fVAR2, 1000.0f, -1.0f );
		break;
	case EMSPECA_REMOVE_SKILL_EFFECT:
		break;
	};
}

void CSkillLevelData::UpdateSkillLevelData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTemp;

	DWORD dwSLevel = m_nSelectLevel;
	
	BOOL bSTATE_BLOW( m_Item.m_sAPPLY.emSTATE_BLOW!=EMBLOW_NONE ),bBLOW_VAR1(TRUE),bBLOW_VAR2(TRUE);
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		SetWin_Text(this, IDC_EDIT_LEVEL_DATA, CGameTextMan::GetInstance().GetCommentText("SKILL_LEVEL", dwSLevel).GetString());
		// 기본;
		strTemp.Format ("Basic Attribute : %s", CGameTextMan::GetInstance().GetCommentText("SKILL_TYPES", m_Item.m_sAPPLY.emBASIC_TYPE).GetString());
		SetWin_Text ( this, IDC_STATIC_DEFAULT, strTemp.GetString() );

		//	상태이상;
		strTemp.Format ( "State Trouble : %s", CGameTextMan::GetInstance().GetCommentText("BLOW", m_Item.m_sAPPLY.emSTATE_BLOW).GetString() );
		SetWin_Text ( this, IDC_STATIC_BLOW, strTemp.GetString() );

		if ( !strcmp(CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1",m_Item.m_sAPPLY.emSTATE_BLOW).GetString()
					,CGameTextMan::GetInstance().GetCommentText("BLOW",0).GetString() ) )
			bBLOW_VAR1 = FALSE;
		if ( !strcmp(CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2",m_Item.m_sAPPLY.emSTATE_BLOW).GetString()
			,CGameTextMan::GetInstance().GetCommentText("BLOW",0).GetString() ) )
			bBLOW_VAR2 = FALSE;

		SetWin_Text ( this, IDC_STATIC_BLOWVAR1, CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1",m_Item.m_sAPPLY.emSTATE_BLOW).GetString() );
		SetWin_Text ( this, IDC_STATIC_BLOWVAR2, CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2",m_Item.m_sAPPLY.emSTATE_BLOW).GetString() );
		SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE1, bBLOW_VAR1 );
		SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE2, bBLOW_VAR2 );
	}
	else
	{
		SetWin_Text( this, IDC_EDIT_LEVEL_DATA, COMMENT::SKILL_LEVEL[dwSLevel].c_str() );
		//	기본; 
		strTemp.Format ( "Basic Attribute : %s", COMMENT::SKILL_TYPES[m_Item.m_sAPPLY.emBASIC_TYPE].c_str() );
		SetWin_Text ( this, IDC_STATIC_DEFAULT, strTemp.GetString() );

		//	상태이상
		strTemp.Format ( "State Trouble : %s", COMMENT::BLOW[m_Item.m_sAPPLY.emSTATE_BLOW].c_str() );
		SetWin_Text ( this, IDC_STATIC_BLOW, strTemp.GetString() );

		const char *szBLOW_VAR1 = COMMENT::BLOW_VAR1[m_Item.m_sAPPLY.emSTATE_BLOW].c_str();
		const char *szBLOW_VAR2 = COMMENT::BLOW_VAR2[m_Item.m_sAPPLY.emSTATE_BLOW].c_str();
		
		if ( !strcmp(szBLOW_VAR1,COMMENT::BLOW_VAR1[EMBLOW_NONE].c_str()) )
			bBLOW_VAR1 = FALSE;
		if ( !strcmp(szBLOW_VAR2,COMMENT::BLOW_VAR2[EMBLOW_NONE].c_str()) )
			bBLOW_VAR2 = FALSE;
		
		SetWin_Text ( this, IDC_STATIC_BLOWVAR1, szBLOW_VAR1 );
		SetWin_Text ( this, IDC_STATIC_BLOWVAR2, szBLOW_VAR2 );
		SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE1, bBLOW_VAR1 );
		SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE2, bBLOW_VAR2 );
	}
	
	SetWin_Num_float ( this, IDC_EDIT_DELAYTIME, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].fDELAYTIME );
	SetWin_Num_float ( this, IDC_EDIT_AGE, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].fLIFE );
	SetWin_Num_int ( this, IDC_EDIT_APPLYRANGE, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wAPPLYRANGE );
	SetWin_Num_int ( this, IDC_EDIT_APPLYNUM, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wAPPLYNUM );
	SetWin_Num_int ( this, IDC_EDIT_APPLYANGLE, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wAPPLYANGLE );
	SetWin_Num_int ( this, IDC_EDIT_PIERCENUM, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wPIERCENUM );
	SetWin_Num_float ( this, IDC_EDIT_VARIATION, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].fBASIC_VAR );
	SetWin_Num_int ( this, IDC_EDIT_TARNUM, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wTARNUM );

	//	사용조건
	SetWin_Num_int ( this, IDC_EDIT_DEC_HP, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_HP );
	SetWin_Num_int ( this, IDC_EDIT_DEC_MP, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_MP );
	SetWin_Num_int ( this, IDC_EDIT_DEC_SP, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_SP );
	SetWin_Num_int ( this, IDC_EDIT_DEC_CP, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_CP );
	SetWin_Num_int ( this, IDC_EDIT_DEC_EXP, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_EXP );
	SetWin_Num_int ( this, IDC_EDIT_DEC_ARROW, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_ARROWNUM );
	SetWin_Num_int ( this, IDC_EDIT_DEC_CHARM, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_CHARMNUM );
	SetWin_Num_int ( this, IDC_EDIT_DEC_BULLET, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_BULLETNUM );
	SetWin_Num_int ( this, IDC_EDIT_DEC_BATTERY, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].nUSE_BATTERY );

	//	사용조건 PARTY
	SetWin_Num_int ( this, IDC_EDIT_DEC_HP_PTY, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_HP_PTY );
	SetWin_Num_int ( this, IDC_EDIT_DEC_MP_PTY, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_MP_PTY );
	SetWin_Num_int ( this, IDC_EDIT_DEC_SP_PTY, m_Item.m_sAPPLY.sDATA_LVL[dwSLevel].wUSE_SP_PTY );

	//	습득조건(1가지만 충족할 경우 유무)
	SetWin_Check ( this, IDC_CHECK_REQ_ONLY_ONE_STAT, m_Item.m_sLEARN.bOnlyOneStats );

	//	습득 요구치
	SetWin_Num_int ( this, IDC_EDIT_REQ_LEVEL, m_Item.m_sLEARN.sLVL_STEP[dwSLevel].dwLEVEL );
	SetWin_Num_int ( this, IDC_EDIT_REQ_EXP, m_Item.m_sLEARN.sLVL_STEP[dwSLevel].dwSKP );
	SetWin_Num_int ( this, IDC_EDIT_REQ_SKILL_LEVEL, m_Item.m_sLEARN.sLVL_STEP[dwSLevel].dwSKILL_LVL );

	SetWin_Num_int ( this, IDC_EDIT_REQ_POW, m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wPow );
	SetWin_Num_int ( this, IDC_EDIT_REQ_STR, m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wStr );

	SetWin_Num_int ( this, IDC_EDIT_REQ_SPI, m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wSpi );
	SetWin_Num_int ( this, IDC_EDIT_REQ_DEX, m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wDex );

	SetWin_Num_int ( this, IDC_EDIT_REQ_INT, m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wInt );
	SetWin_Num_int ( this, IDC_EDIT_REQ_STA, m_Item.m_sLEARN.sLVL_STEP[dwSLevel].sSTATS.wSta );
	SetWin_Num_LONGLONG( this, IDC_EDIT_REQ_MONEY, m_Item.m_sLEARN.sLVL_STEP[dwSLevel].dwReqMoney );

	SetWin_Enable ( this, IDC_EDIT_BLOW_RATE, bSTATE_BLOW );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_RATE, m_Item.m_sAPPLY.sSTATE_BLOW[dwSLevel].fRATE );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE1, m_Item.m_sAPPLY.sSTATE_BLOW[dwSLevel].fVAR1 );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE2, m_Item.m_sAPPLY.sSTATE_BLOW[dwSLevel].fVAR2 );


	//	특수 기능
	SetWin_Enable ( this, IDC_SPECIAL_BUTTON1, FALSE );
	SetWin_Enable ( this, IDC_SPECIAL_BUTTON2, FALSE );
	SetWin_Enable ( this, IDC_SPECIAL_BUTTON3, FALSE );

	if( m_Item.m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSFORM )
		SetWin_Enable ( this, IDC_SPECIAL_BUTTON1, TRUE );
	else if( m_Item.m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_SUMMONS )
		SetWin_Enable ( this, IDC_SPECIAL_BUTTON2, TRUE );

	UpdateImpactAddon();
	UpdateSpecAddon();

	SetReadMode();
}

void CSkillLevelData::UpdateImpactAddon()
{
	DWORD dwSLevel = m_nSelectLevel;

	if ( m_nSelectImpact < 0 )
        return;
	if ( m_nSelectImpact >= (int)m_Item.m_sAPPLY.vecADDON.size() )
        return;	


	//	부가효과
	BOOL bRate(TRUE);
	if ( !m_Item.m_sBASIC.bMobEffectRate )
		bRate = FALSE;

	SetWin_Enable ( this, IDC_EDIT_IMPACT_VAR2, bRate );

	SetWin_Num_float ( this, IDC_EDIT_IMPACT_VAR, m_Item.m_sAPPLY.vecADDON[m_nSelectImpact].fADDON_VAR[dwSLevel] );
	SetWin_Num_float ( this, IDC_EDIT_IMPACT_VAR2, m_Item.m_sAPPLY.vecADDON[m_nSelectImpact].fRate[ dwSLevel ] * 100.0f );

    SetWin_Text( this, IDC_STATIC_IMPACT_TOOLTIP, 
				 GLSkillToolTip::MakeTooltipSting_In_Skill_Impact(
				 GetWin_Num_float( this, IDC_EDIT_AGE ),
				 GetWin_Num_float( this, IDC_EDIT_IMPACT_VAR ),
				 GetWin_Num_float( this, IDC_EDIT_IMPACT_VAR2 ) * GetWin_Num_float ( this, IDC_EDIT_IMPACT_VAR ),
				 m_Item.m_sBASIC.emROLE,
				 m_Item.m_sAPPLY.vecADDON[m_nSelectImpact].emADDON).c_str() );
}

void CSkillLevelData::UpdateSpecAddon()
{
	DWORD dwSLevel = m_nSelectLevel;
	
	if( m_nSelectSpec < 0 )
		return;

	if( m_nSelectSpec >= (int)m_Item.m_sAPPLY.vecSPEC.size() )
		return;

	m_listSpecCAOption.ResetContent();
	m_listSpecImmune.ResetContent();

	const SKILL::SSPEC_ADDON sSpecAddon = m_Item.m_sAPPLY.vecSPEC[m_nSelectSpec];

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		//	특수기능;
		BOOL			bSPEC_VAR1(TRUE), bSPEC_VAR2(TRUE), bRate(TRUE), bRate2(TRUE), bDWFLAG(TRUE);
		const char*		szSPEC_VAR1 = CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR1", sSpecAddon.emSPEC).GetString();;
		const char*		szSPEC_VAR2 = CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR2", sSpecAddon.emSPEC).GetString();;

		if( !strcmp(szSPEC_VAR1,CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR1",EMSPECA_NULL).GetString()) )
			bSPEC_VAR1 = FALSE;

		if( !strcmp(szSPEC_VAR2,CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR2",EMSPECA_NULL).GetString()) )
			bSPEC_VAR2 = FALSE;

		if( !m_Item.m_sBASIC.bMobEffectRate ||
			!strcmp(szSPEC_VAR1,CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR1",EMSPECA_NULL).GetString()) )
			bRate = FALSE;

		if( !m_Item.m_sBASIC.bMobEffectRate ||
			!strcmp(szSPEC_VAR2,CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR2",EMSPECA_NULL).GetString()) )
			bRate2 = FALSE;

		SetWin_Text( this, IDC_STATIC_SPEC_VAR1, szSPEC_VAR1 );
		SetWin_Text( this, IDC_STATIC_SPEC_VAR2, szSPEC_VAR2 );
		SetWin_Enable( this, IDC_EDIT_SPEC_VAR1, bSPEC_VAR1 );
		SetWin_Enable( this, IDC_EDIT_SPEC_VAR2, bSPEC_VAR2 );
		SetWin_Enable( this, IDC_EDIT_SPEC_VAR3, bRate );
		SetWin_Enable( this, IDC_EDIT_SPEC_VAR4, bRate2 );
		SetWin_Enable( this, IDC_EDIT_SPEC_DWFLAG, bDWFLAG );
		SetWin_Enable( this, IDC_STATIC_SPEC_VAR1, bSPEC_VAR1 );
		SetWin_Enable( this, IDC_STATIC_SPEC_VAR2, bSPEC_VAR2 );
	}
	else
	{
		//	특수기능;
		BOOL			bSPEC_VAR1(TRUE), bSPEC_VAR2(TRUE), bRate(TRUE), bRate2(TRUE), bDWFLAG(TRUE);
		const char*		szSPEC_VAR1 = COMMENT::SPEC_ADDON_VAR1[sSpecAddon.emSPEC].c_str();
		const char*		szSPEC_VAR2 = COMMENT::SPEC_ADDON_VAR2[sSpecAddon.emSPEC].c_str();

		if( !strcmp(szSPEC_VAR1,COMMENT::SPEC_ADDON_VAR1[EMSPECA_NULL].c_str()) )
			bSPEC_VAR1 = FALSE;

		if( !strcmp(szSPEC_VAR2,COMMENT::SPEC_ADDON_VAR2[EMSPECA_NULL].c_str()) )
			bSPEC_VAR2 = FALSE;

		if( !m_Item.m_sBASIC.bMobEffectRate ||
			!strcmp(szSPEC_VAR1,COMMENT::SPEC_ADDON_VAR1[EMSPECA_NULL].c_str()) )
			bRate = FALSE;

		if( !m_Item.m_sBASIC.bMobEffectRate ||
			!strcmp(szSPEC_VAR2,COMMENT::SPEC_ADDON_VAR2[EMSPECA_NULL].c_str()) )
			bRate2 = FALSE;

		switch ( sSpecAddon.emSPEC )
		{
		case EMSPECA_ENTRANCE_EXIT :
			{
				if( m_nSelectLevel != 0 )
				{
					bSPEC_VAR1 = FALSE;
					bSPEC_VAR2 = FALSE;
				}
				else
				{
					bSPEC_VAR1 = TRUE;
					bSPEC_VAR2 = TRUE;
				}
			}
			break;
		}

		SetWin_Text( this, IDC_STATIC_SPEC_VAR1, szSPEC_VAR1 );
		SetWin_Text( this, IDC_STATIC_SPEC_VAR2, szSPEC_VAR2 );
		SetWin_Enable( this, IDC_EDIT_SPEC_VAR1, bSPEC_VAR1 );
		SetWin_Enable( this, IDC_EDIT_SPEC_VAR2, bSPEC_VAR2 );
		SetWin_Enable( this, IDC_EDIT_SPEC_VAR3, bRate );
		SetWin_Enable( this, IDC_EDIT_SPEC_VAR4, bRate2 );
		SetWin_Enable( this, IDC_EDIT_SPEC_DWFLAG, bDWFLAG );
		SetWin_Enable( this, IDC_STATIC_SPEC_VAR1, bSPEC_VAR1 );
		SetWin_Enable( this, IDC_STATIC_SPEC_VAR2, bSPEC_VAR2 );
	}
	SetWin_Num_float( this, IDC_EDIT_SPEC_VAR1, sSpecAddon.sSPEC[dwSLevel].fVAR1 );
	SetWin_Num_float( this, IDC_EDIT_SPEC_VAR2, sSpecAddon.sSPEC[dwSLevel].fVAR2 );    
	SetWin_Num_float( this, IDC_EDIT_SPEC_VAR3, sSpecAddon.sSPEC[dwSLevel].fRate * 100.0f );
	SetWin_Num_float( this, IDC_EDIT_SPEC_VAR4, sSpecAddon.sSPEC[dwSLevel].fRate2 * 100.0f );
	SetWin_Num_int( this, IDC_EDIT_SPEC_DWFLAG, sSpecAddon.sSPEC[dwSLevel].dwFLAG);

	//	일단 다 숨긴다.
	SetWin_ShowWindow( this, IDC_SPEC_VAR_LIST, SW_HIDE ); 

	SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_CHECK_CHK2, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_CHECK_CHK3, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_CHECK_CHK4, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_CHECK_CHK5, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_CHECK_CHK6, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_CHECK_CHK7, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_CHECK_CHK8, SW_HIDE ); 

	SetWin_ShowWindow( this, IDC_RADIO_SPEC1, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_RADIO_SPEC2, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_RADIO_SPEC3, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_RADIO_SPEC4, SW_HIDE ); 

	SetWin_ShowWindow( this, IDC_EDIT_DEFENSE_SKILL_MID, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_EDIT_DEFENSE_SKILL_SID, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_EDIT_DEFENSE_SKILL_LEVEL, SW_HIDE ); 	
	SetWin_ShowWindow( this, IDC_STATIC_DEFENSE_LEVEL, SW_HIDE );
	SetWin_ShowWindow( this, IDC_STATIC_DEFENSE_SKILL, SW_HIDE );	

	SetWin_ShowWindow( this, IDC_EDIT_ONWARD_SUBANI, SW_HIDE );
	SetWin_ShowWindow( this, IDC_STATIC_ONWARD_SUBANI, SW_HIDE );

	SetWin_ShowWindow( this, IDC_STATIC_REMOVE_ACTION_TYPE, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_COMBO_REMOVE_ACTION_TYPE, SW_HIDE ); 

	SetWin_ShowWindow( this, IDC_STATIC_DURATION_ATTB, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_COMBO_DURATION_ATTB, SW_HIDE ); 

	SetWin_ShowWindow( this, IDC_COMBO_INCREASE_EFF, SW_HIDE );
	SetWin_ShowWindow( this, IDC_STATIC_INCREASE_EFF, SW_HIDE );

	SetWin_ShowWindow( this, IDC_STATIC_SPEC_DWFLAG, SW_HIDE );
	SetWin_ShowWindow( this, IDC_EDIT_SPEC_DWFLAG, SW_HIDE );

	SetWin_ShowWindow( this, IDC_EDIT_TRIGGER_TYPE_LINK_MID, SW_HIDE );
	SetWin_ShowWindow( this, IDC_EDIT_TRIGGER_TYPE_LINK_SID, SW_HIDE );
	SetWin_ShowWindow( this, IDC_STATIC_TRIGGER_TYPE_LINK, SW_HIDE );  

	// 반격
	SetWin_ShowWindow( this, IDC_STATIC_COUNTATTACK, SW_HIDE);
	SetWin_ShowWindow( this, IDC_LIST_COUNTATTACK, SW_HIDE);
	SetWin_ShowWindow( this, IDC_BUTTON_CA_ADD, SW_HIDE);
	SetWin_ShowWindow( this, IDC_BUTTON_CA_DEL, SW_HIDE);

	// 죽은척
	SetWin_ShowWindow( this, IDC_CHECK_TARGET , SW_HIDE);
	SetWin_ShowWindow( this, IDC_CHECK_BUF_DEL, SW_HIDE);

	SetWin_Check( this, IDC_RADIO_SPEC1, false);
	SetWin_Check( this, IDC_RADIO_SPEC2, false);
	SetWin_Check( this, IDC_RADIO_SPEC3, false);
	SetWin_Check( this, IDC_RADIO_SPEC4, false);

	SetWin_ShowWindow( this, IDC_CHECK_CA_DAMAGE, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_CHECK_CA_ALL, SW_HIDE ); 

	SetWin_ShowWindow( this, IDC_STATIC_TF_FORM_TYPE, SW_HIDE );

	// 특수효과 면역
	SetWin_ShowWindow( this, IDC_STATIC_SPECIAL_IMMUNE    , SW_HIDE);
	SetWin_ShowWindow( this, IDC_LIST_SPECIAL_IMMUNE      , SW_HIDE);
	SetWin_ShowWindow( this, IDC_BUTTON_SPECIAL_IMMUNE_ADD, SW_HIDE);
	SetWin_ShowWindow( this, IDC_BUTTON_SPECIAL_IMMUNE_DEL, SW_HIDE);

	// 피해발동
	SetWin_ShowWindow( this, IDC_STATIC_SKILL_MID, SW_HIDE);
	SetWin_ShowWindow( this, IDC_STATIC_SKILL_SID, SW_HIDE);
	SetWin_ShowWindow( this, IDC_EDIT_SKILL_MID  , SW_HIDE);
	SetWin_ShowWindow( this, IDC_EDIT_SKILL_SID  , SW_HIDE);

	SetWin_ShowWindow( this, IDC_STATIC_WOMAN  , SW_HIDE);
	SetWin_ShowWindow( this, IDC_STATIC_MAN  , SW_HIDE);
	
	//	특수기능 : 상태이상 방지(회복)
	switch ( sSpecAddon.emSPEC )
	{
	case EMSPECA_ACTIVATE_DAMAGE :
		{
			SetWin_ShowWindow( this, IDC_STATIC_SPEC_VAR3, SW_HIDE);
			SetWin_ShowWindow( this, IDC_STATIC_SPEC_VAR4, SW_HIDE);
			SetWin_ShowWindow( this, IDC_EDIT_SPEC_VAR3  , SW_HIDE);
			SetWin_ShowWindow( this, IDC_EDIT_SPEC_VAR4  , SW_HIDE);

			SetWin_ShowWindow( this, IDC_STATIC_SKILL_MID, SW_SHOW);
			SetWin_ShowWindow( this, IDC_STATIC_SKILL_SID, SW_SHOW);
			SetWin_ShowWindow( this, IDC_EDIT_SKILL_MID  , SW_SHOW);
			SetWin_ShowWindow( this, IDC_EDIT_SKILL_SID  , SW_SHOW);

			SetWin_Num_int( this, IDC_EDIT_SPEC_VAR1, sSpecAddon.sSPEC[dwSLevel].nVAR1 );
			SetWin_Num_int( this, IDC_EDIT_SPEC_VAR2, sSpecAddon.sSPEC[dwSLevel].nVAR2 );
			SetWin_Num_int ( this, IDC_EDIT_SKILL_MID, sSpecAddon.sSPEC[dwSLevel].dwNativeID.wMainID );
			SetWin_Num_int ( this, IDC_EDIT_SKILL_SID, sSpecAddon.sSPEC[dwSLevel].dwNativeID.wSubID );
		}
		break;
	case EMSPECA_SPECIAL_IMMUNE :
		{			
			SetWin_ShowWindow( this, IDC_STATIC_SPECIAL_IMMUNE, SW_SHOW);
			SetWin_ShowWindow( this, IDC_LIST_SPECIAL_IMMUNE, SW_SHOW);
			SetWin_ShowWindow( this, IDC_BUTTON_SPECIAL_IMMUNE_ADD, SW_SHOW);
			SetWin_ShowWindow( this, IDC_BUTTON_SPECIAL_IMMUNE_DEL, SW_SHOW);

			SetWin_Num_int( this, IDC_EDIT_SPEC_VAR1, sSpecAddon.sSPEC[dwSLevel].nVAR1 );
			SetWin_Num_int( this, IDC_EDIT_SPEC_VAR2, sSpecAddon.sSPEC[dwSLevel].nVAR2 );

			if( m_nSelectLevel != 0 )
			{
				SetWin_Enable( this, IDC_STATIC_SPECIAL_IMMUNE, FALSE);
				SetWin_Enable( this, IDC_LIST_SPECIAL_IMMUNE, FALSE);
				SetWin_Enable( this, IDC_BUTTON_SPECIAL_IMMUNE_ADD, FALSE);
				SetWin_Enable( this, IDC_BUTTON_SPECIAL_IMMUNE_DEL, FALSE);
			}
			else
			{
				SetWin_Enable( this, IDC_STATIC_SPECIAL_IMMUNE, TRUE);
				SetWin_Enable( this, IDC_LIST_SPECIAL_IMMUNE, TRUE);
				SetWin_Enable( this, IDC_BUTTON_SPECIAL_IMMUNE_ADD, TRUE);
				SetWin_Enable( this, IDC_BUTTON_SPECIAL_IMMUNE_DEL, TRUE);
			}

			SetWin_Enable( this, IDC_EDIT_SPEC_VAR1, SW_HIDE );

			DWORD dwValue = (DWORD)sSpecAddon.sSPEC[dwSLevel].nVAR1;

			if( dwValue > 0 )
			{
				EMSPEC_ADDON eTemp = (EMSPEC_ADDON)dwValue;

				if( eTemp != EMSPECA_NULL )
				{
					CString strTemp = COMMENT::SPEC_ADDON[eTemp].c_str();

					m_listSpecImmune.SetItemData( m_listSpecImmune.AddString(strTemp), (DWORD)eTemp);
				}
			}						
		}
		break;
	case EMSPECA_DISGUISE    :
		{
			SetWin_Enable( this, IDC_STATIC_SPEC_VAR2, FALSE );
			SetWin_Enable( this, IDC_EDIT_SPEC_VAR2, FALSE );

			SetWin_ShowWindow( this, IDC_STATIC_TF_FORM_TYPE, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_COMBO_REMOVE_ACTION_TYPE, SW_SHOW ); 

			SetWin_Combo_Init(this, IDC_COMBO_REMOVE_ACTION_TYPE, COMMENT::TFCHAR_SET, GLTRANSFORMSET::g_nTransformChaSet );
			SetWin_Combo_Sel(this, IDC_COMBO_REMOVE_ACTION_TYPE, COMMENT::TFCHAR_SET[DWORD(sSpecAddon.sSPEC[dwSLevel].fVAR2)].c_str() );
			
			if( DWORD(sSpecAddon.sSPEC[dwSLevel].fVAR2) > 0 )
			{
				std::string strData = COMMENT::TFCHAR_SET[DWORD(sSpecAddon.sSPEC[dwSLevel].fVAR2)];
				std::string strTemp = strData;

				std::string::size_type iNum = strData.find(".chf");
									
				if( iNum != std::string::npos ) {
					boost::algorithm::erase_first( strData, ".chf" );		
					strTemp = strData;
				}
								
				SetWin_ShowWindow( this, IDC_STATIC_MAN, SW_SHOW ); 
				SetWin_ShowWindow( this, IDC_STATIC_WOMAN, SW_SHOW ); 

				std::string strM = strTemp + "_m.chf";
				std::string strW = strTemp + "_w.chf";

				SetWin_Text( this, IDC_STATIC_MAN, strM.c_str() );
				SetWin_Text( this, IDC_STATIC_WOMAN, strW.c_str() );
			}			
		}
		break;
	case EMSPECA_FEIGN_DEATH :
		{
			if( m_nSelectLevel != 0 )
			{
				SetWin_Enable( this, IDC_CHECK_TARGET , FALSE );
				SetWin_Enable( this, IDC_CHECK_BUF_DEL, FALSE );
			}
			else
			{
				SetWin_Enable( this, IDC_CHECK_TARGET , TRUE );
				SetWin_Enable( this, IDC_CHECK_BUF_DEL, TRUE );
			}

			SetWin_ShowWindow( this, IDC_CHECK_TARGET , SW_SHOW);
			SetWin_ShowWindow( this, IDC_CHECK_BUF_DEL, SW_SHOW);

			SetWin_Check ( this, IDC_CHECK_TARGET , sSpecAddon.sSPEC[dwSLevel].dwFLAG&FEIGN_DEATH_TARGETING_DISABLE );
			SetWin_Check ( this, IDC_CHECK_BUF_DEL, sSpecAddon.sSPEC[dwSLevel].dwFLAG&FEIGN_DEATH_BUF_DEL );
		}
		break;
	case EMSPECA_COUNTATTACK :
		{
			SetWin_ShowWindow( this, IDC_STATIC_COUNTATTACK, SW_SHOW);
			SetWin_ShowWindow( this, IDC_LIST_COUNTATTACK, SW_SHOW);
			SetWin_ShowWindow( this, IDC_BUTTON_CA_ADD, SW_SHOW);
			SetWin_ShowWindow( this, IDC_BUTTON_CA_DEL, SW_SHOW);
			SetWin_ShowWindow( this, IDC_CHECK_CA_DAMAGE, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CA_ALL, SW_SHOW ); 

			SetWin_Check ( this, IDC_CHECK_CA_DAMAGE, m_Item.m_sAPPLY.sCAData.sSPEC_CA[dwSLevel].bDamageApply );
			SetWin_Check ( this, IDC_CHECK_CA_ALL, m_Item.m_sAPPLY.sCAData.sSPEC_CA[dwSLevel].bAllApply );

			for( int j = 0; j < SKILL::MAX_ADDON; ++j )
			{
				EMSPEC_ADDON eTemp = m_Item.m_sAPPLY.sCAData.sSPEC_CA[dwSLevel].emSPEC[j];

				if( eTemp != EMSPECA_NULL )
				{
					CString strTemp = COMMENT::SPEC_ADDON[eTemp].c_str();

					m_listSpecCAOption.SetItemData( m_listSpecCAOption.AddString(strTemp), (DWORD)eTemp);
				}
			}

			if( m_Item.m_sAPPLY.sCAData.sSPEC_CA[dwSLevel].bAllApply )	{
				m_listSpecCAOption.EnableWindow(FALSE);
			}
			else {
				m_listSpecCAOption.EnableWindow(TRUE);
			}
		}
		break;
	case EMSPECA_NONBLOW:
	case EMSPECA_RECBLOW:
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Text( this, IDC_CHECK_CHK1, CGameTextMan::GetInstance().GetText("BLOW", EMBLOW_NUMB, CGameTextMan::EM_COMMENT_TEXT).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK2, CGameTextMan::GetInstance().GetText("BLOW", EMBLOW_STUN, CGameTextMan::EM_COMMENT_TEXT).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK3, CGameTextMan::GetInstance().GetText("BLOW", EMBLOW_STONE, CGameTextMan::EM_COMMENT_TEXT).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK4, CGameTextMan::GetInstance().GetText("BLOW", EMBLOW_BURN, CGameTextMan::EM_COMMENT_TEXT).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK5, CGameTextMan::GetInstance().GetText("BLOW", EMBLOW_FROZEN, CGameTextMan::EM_COMMENT_TEXT).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK6, CGameTextMan::GetInstance().GetText("BLOW", EMBLOW_MAD, CGameTextMan::EM_COMMENT_TEXT).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK7, CGameTextMan::GetInstance().GetText("BLOW", EMBLOW_POISON, CGameTextMan::EM_COMMENT_TEXT).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK8, CGameTextMan::GetInstance().GetText("BLOW", EMBLOW_CURSE, CGameTextMan::EM_COMMENT_TEXT).GetString() );
			}
			else
			{
				SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::BLOW[EMBLOW_NUMB].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK2, COMMENT::BLOW[EMBLOW_STUN].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK3, COMMENT::BLOW[EMBLOW_STONE].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK4, COMMENT::BLOW[EMBLOW_BURN].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK5, COMMENT::BLOW[EMBLOW_FROZEN].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK6, COMMENT::BLOW[EMBLOW_MAD].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK7, COMMENT::BLOW[EMBLOW_POISON].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK8, COMMENT::BLOW[EMBLOW_CURSE].c_str() );
			}

			SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK2, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK3, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK4, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK5, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK6, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK7, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK8, SW_SHOW ); 	

			SetWin_Check ( this, IDC_CHECK_CHK1, sSpecAddon.sSPEC[dwSLevel].dwFLAG&DIS_NUMB );
			SetWin_Check ( this, IDC_CHECK_CHK2, sSpecAddon.sSPEC[dwSLevel].dwFLAG&DIS_STUN );
			SetWin_Check ( this, IDC_CHECK_CHK3, sSpecAddon.sSPEC[dwSLevel].dwFLAG&DIS_STONE );

			SetWin_Check ( this, IDC_CHECK_CHK4, sSpecAddon.sSPEC[dwSLevel].dwFLAG&DIS_BURN );
			SetWin_Check ( this, IDC_CHECK_CHK5, sSpecAddon.sSPEC[dwSLevel].dwFLAG&DIS_FROZEN );

			SetWin_Check ( this, IDC_CHECK_CHK6, sSpecAddon.sSPEC[dwSLevel].dwFLAG&DIS_MAD );
			SetWin_Check ( this, IDC_CHECK_CHK7, sSpecAddon.sSPEC[dwSLevel].dwFLAG&DIS_POISON );
			SetWin_Check ( this, IDC_CHECK_CHK8, sSpecAddon.sSPEC[dwSLevel].dwFLAG&DIS_CURSE );
		}
		break;
	case EMSPECA_DEFENSE_SKILL_ACTIVE:
		SetWin_ShowWindow( this, IDC_EDIT_DEFENSE_SKILL_MID, SW_SHOW ); 
		SetWin_ShowWindow( this, IDC_EDIT_DEFENSE_SKILL_SID, SW_SHOW ); 
		SetWin_ShowWindow( this, IDC_EDIT_DEFENSE_SKILL_LEVEL, SW_SHOW ); 	
		SetWin_ShowWindow( this, IDC_STATIC_DEFENSE_LEVEL, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_DEFENSE_SKILL, SW_SHOW );	

		SetWin_Num_int ( this, IDC_EDIT_DEFENSE_SKILL_MID, sSpecAddon.sSPEC[dwSLevel].dwNativeID.wMainID );
		SetWin_Num_int ( this, IDC_EDIT_DEFENSE_SKILL_SID, sSpecAddon.sSPEC[dwSLevel].dwNativeID.wSubID );
		SetWin_Num_int ( this, IDC_EDIT_DEFENSE_SKILL_LEVEL, sSpecAddon.sSPEC[dwSLevel].dwFLAG );
		break;
	case EMSPECA_ONWARD:
		SetWin_ShowWindow( this, IDC_EDIT_ONWARD_SUBANI, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_ONWARD_SUBANI, SW_SHOW );
		SetWin_Num_int ( this, IDC_EDIT_ONWARD_SUBANI, sSpecAddon.sSPEC[dwSLevel].dwFLAG );
		break;
	case EMSPECA_EFFECT_REMOVE:
		{
			SetWin_ShowWindow( this, IDC_STATIC_REMOVE_ACTION_TYPE, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_COMBO_REMOVE_ACTION_TYPE, SW_SHOW ); 

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Combo_Sel(this, IDC_COMBO_REMOVE_ACTION_TYPE, CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE",sSpecAddon.sSPEC[dwSLevel].dwFLAG).GetString());
			}
			else
			{
				SetWin_Combo_Sel(this, IDC_COMBO_REMOVE_ACTION_TYPE, COMMENT::SKILL_ACTION_TYPE[sSpecAddon.sSPEC[dwSLevel].dwFLAG].c_str());
			}
		}
		break;
	case EMSPECA_DURATION_DAMAGE:
		{
			SetWin_ShowWindow( this, IDC_STATIC_DURATION_ATTB, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_COMBO_DURATION_ATTB, SW_SHOW ); 
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Combo_Sel(this, IDC_COMBO_REMOVE_ACTION_TYPE, CGameTextMan::GetInstance().GetCommentText("ELEMENT",sSpecAddon.sSPEC[dwSLevel].dwFLAG).GetString());
			}
			else
			{
				SetWin_Combo_Sel(this, IDC_COMBO_DURATION_ATTB, COMMENT::ELEMENT[sSpecAddon.sSPEC[dwSLevel].dwFLAG].c_str() );
			}
		}
		break;
	case EMSPECA_INCREASE_EFF:
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Text( this, IDC_CHECK_CHK1, CGameTextMan::GetInstance().GetCommentText("SKILL_APPLY",0).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK2, CGameTextMan::GetInstance().GetCommentText("SKILL_APPLY",1).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK3, CGameTextMan::GetInstance().GetCommentText("SKILL_APPLY",2).GetString() );
			}
			else
			{
				SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SKILL_APPLY[0].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK2, COMMENT::SKILL_APPLY[1].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK3, COMMENT::SKILL_APPLY[2].c_str() );
			}

			SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK2, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK3, SW_SHOW ); 		

			SetWin_Check ( this, IDC_CHECK_CHK1, sSpecAddon.sSPEC[dwSLevel].dwFLAG & SKILL::EMAPPLY_DWORD_MELEE );
			SetWin_Check ( this, IDC_CHECK_CHK2, sSpecAddon.sSPEC[dwSLevel].dwFLAG & SKILL::EMAPPLY_DWORD_RANGE);
			SetWin_Check ( this, IDC_CHECK_CHK3, sSpecAddon.sSPEC[dwSLevel].dwFLAG & SKILL::EMAPPLY_DWORD_MAGIC);	
		}
		break;
	case EMSPECA_INVISIBLE:			
		{
			SetWin_Enable( this, IDC_EDIT_SPEC_VAR2, SW_HIDE );	

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Text( this, IDC_CHECK_CHK1, CGameTextMan::GetInstance().GetCommentText("SPEC_INVISIBLE_USE_TYPE").GetString() );
				SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW );		

				SetWin_Text( this, IDC_RADIO_SPEC1, CGameTextMan::GetInstance().GetCommentText("SPEC_INVISIBLE_ANI_TYPE", EMSPEC_INVISIBLE_ANI_TYPE_NONE).GetString() );
				SetWin_Text( this, IDC_RADIO_SPEC2, CGameTextMan::GetInstance().GetCommentText("SPEC_INVISIBLE_ANI_TYPE", EMSPEC_INVISIBLE_ANI_TYPE_WALK).GetString() );
				SetWin_Text( this, IDC_RADIO_SPEC3, CGameTextMan::GetInstance().GetCommentText("SPEC_INVISIBLE_ANI_TYPE", EMSPEC_INVISIBLE_ANI_TYPE_RUN).GetString() );

				SetWin_ShowWindow( this, IDC_RADIO_SPEC1, SW_SHOW );
				SetWin_ShowWindow( this, IDC_RADIO_SPEC2, SW_SHOW ); 
				SetWin_ShowWindow( this, IDC_RADIO_SPEC3, SW_SHOW ); 
			}
			else
			{
				SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SPEC_INVISIBLE_USE_TYPE.c_str() );
				SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW );		

				SetWin_Text( this, IDC_RADIO_SPEC1, COMMENT::SPEC_INVISIBLE_ANI_TYPE[EMSPEC_INVISIBLE_ANI_TYPE_NONE].c_str() );
				SetWin_Text( this, IDC_RADIO_SPEC2, COMMENT::SPEC_INVISIBLE_ANI_TYPE[EMSPEC_INVISIBLE_ANI_TYPE_WALK].c_str() );
				SetWin_Text( this, IDC_RADIO_SPEC3, COMMENT::SPEC_INVISIBLE_ANI_TYPE[EMSPEC_INVISIBLE_ANI_TYPE_RUN].c_str() );

				SetWin_ShowWindow( this, IDC_RADIO_SPEC1, SW_SHOW );
				SetWin_ShowWindow( this, IDC_RADIO_SPEC2, SW_SHOW ); 
				SetWin_ShowWindow( this, IDC_RADIO_SPEC3, SW_SHOW ); 
			}
			switch ( DWORD(sSpecAddon.sSPEC[dwSLevel].fVAR2) )
			{
			case EMSPEC_INVISIBLE_ANI_TYPE_NONE:
				SetWin_Check ( this, IDC_RADIO_SPEC1, true );
				break;
			case EMSPEC_INVISIBLE_ANI_TYPE_WALK:
				SetWin_Check ( this, IDC_RADIO_SPEC2, true );
				break;
			case EMSPEC_INVISIBLE_ANI_TYPE_RUN:
				SetWin_Check ( this, IDC_RADIO_SPEC3, true );
				break;
			}
		}
		break;

	case EMSPECA_SWAPPOS:
		{
			SetWin_ShowWindow( this, IDC_RADIO_SPEC1, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_RADIO_SPEC2, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_RADIO_SPEC3, SW_SHOW ); 

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Text( this, IDC_RADIO_SPEC1, CGameTextMan::GetInstance().GetCommentText("SPEC_SWAPPOS_DIR_TYPE", EMSPEC_SWAPPOS_DIR_BOTH).GetString() );
				SetWin_Text( this, IDC_RADIO_SPEC2, CGameTextMan::GetInstance().GetCommentText("SPEC_SWAPPOS_DIR_TYPE", EMSPEC_SWAPPOS_DIR_SELF).GetString() );
				SetWin_Text( this, IDC_RADIO_SPEC3, CGameTextMan::GetInstance().GetCommentText("SPEC_SWAPPOS_DIR_TYPE", EMSPEC_SWAPPOS_DIR_TARGET).GetString() );
			}
			else
			{
				SetWin_Text( this, IDC_RADIO_SPEC1, COMMENT::SPEC_SWAPPOS_DIR_TYPE[EMSPEC_SWAPPOS_DIR_BOTH].c_str() );
				SetWin_Text( this, IDC_RADIO_SPEC2, COMMENT::SPEC_SWAPPOS_DIR_TYPE[EMSPEC_SWAPPOS_DIR_SELF].c_str() );
				SetWin_Text( this, IDC_RADIO_SPEC3, COMMENT::SPEC_SWAPPOS_DIR_TYPE[EMSPEC_SWAPPOS_DIR_TARGET].c_str() );
			}
			int		nResId = IDC_RADIO_SPEC1 + min( sSpecAddon.sSPEC[dwSLevel].dwFLAG, EMSPEC_SWAPPOS_DIR_TARGET );
			SetWin_Check ( this, nResId, true );
		}
		break;

	case EMSPECA_ILLUSION:
		SetWin_Num_int( this, IDC_EDIT_SPEC_VAR1, sSpecAddon.sSPEC[dwSLevel].nVAR1 );
		SetWin_Num_int( this, IDC_EDIT_SPEC_VAR2, sSpecAddon.sSPEC[dwSLevel].nVAR2 );    
		break;

	case EMSPECA_RELEASE_ENDURE:
		{
			SetWin_Enable( this, IDC_STATIC_SPEC_VAR2, SW_HIDE );
			SetWin_Enable( this, IDC_EDIT_SPEC_VAR2, SW_HIDE );		
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Text( this, IDC_CHECK_CHK1, CGameTextMan::GetInstance().GetCommentText("SPEC_STIGMA_TYPE", EMSPEC_RELEASE_ENDURE_TYPE_NORMAL).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK2, CGameTextMan::GetInstance().GetCommentText("SPEC_STIGMA_TYPE", EMSPEC_RELEASE_ENDURE_TYPE_SKILL).GetString() );
			}
			else
			{
				SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SPEC_STIGMA_TYPE[EMSPEC_RELEASE_ENDURE_TYPE_NORMAL].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK2, COMMENT::SPEC_STIGMA_TYPE[EMSPEC_RELEASE_ENDURE_TYPE_SKILL].c_str() );
			}

			SetWin_Check ( this, IDC_CHECK_CHK1, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_RELEASE_ENDURE_TYPE_DWORD_NORMAL );
			SetWin_Check ( this, IDC_CHECK_CHK2, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_RELEASE_ENDURE_TYPE_DWORD_SKILL );
		}
		break;
	case EMSPECA_ENDURE_VARY:
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Text( this, IDC_CHECK_CHK1, CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE", SKILL::EMACTION_TYPE_NORMAL).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK2, CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE", SKILL::EMACTION_TYPE_BUFF).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK3, CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE", SKILL::EMACTION_TYPE_DEBUFF).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK4, CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE", SKILL::EMACTION_TYPE_LIMIT).GetString() );
			}
			else
			{
				SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_NORMAL].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK2, COMMENT::SKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_BUFF].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK3, COMMENT::SKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_DEBUFF].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK4, COMMENT::SKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_LIMIT].c_str() );
			}

			SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK2, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK3, SW_SHOW );
			SetWin_ShowWindow( this, IDC_CHECK_CHK4, SW_SHOW );

			SetWin_Check ( this, IDC_CHECK_CHK1, sSpecAddon.sSPEC[dwSLevel].dwFLAG &
				SKILL::EMACTION_TYPE_DWORD_NORMAL );
			SetWin_Check ( this, IDC_CHECK_CHK2, sSpecAddon.sSPEC[dwSLevel].dwFLAG &
				SKILL::EMACTION_TYPE_DWORD_BUFF );
			SetWin_Check ( this, IDC_CHECK_CHK3, sSpecAddon.sSPEC[dwSLevel].dwFLAG &
				SKILL::EMACTION_TYPE_DWORD_DEBUFF);
			SetWin_Check ( this, IDC_CHECK_CHK4, sSpecAddon.sSPEC[dwSLevel].dwFLAG &
				SKILL::EMACTION_TYPE_DWORD_LIMIT);
		}
		break;
	case EMSPECA_IMMUNE:
		{
			SetWin_Enable( this, IDC_EDIT_SPEC_VAR1, SW_HIDE );
			SetWin_Enable( this, IDC_EDIT_SPEC_VAR2, SW_HIDE );
			SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SKILL_APPLY[SKILL::EMAPPLY_MELEE].c_str() );
			SetWin_Text( this, IDC_CHECK_CHK2, COMMENT::SKILL_APPLY[SKILL::EMAPPLY_RANGE].c_str() );
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Text( this, IDC_CHECK_CHK1, CGameTextMan::GetInstance().GetCommentText("SKILL_APPLY", SKILL::EMAPPLY_MELEE).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK2, CGameTextMan::GetInstance().GetCommentText("SKILL_APPLY", SKILL::EMAPPLY_RANGE).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK3, CGameTextMan::GetInstance().GetCommentText("SKILL_APPLY", SKILL::EMAPPLY_MAGIC).GetString() );

				SetWin_Text( this, IDC_CHECK_CHK5, 
					CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE", SKILL::EMACTION_TYPE_NORMAL).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK6, 
					CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE", SKILL::EMACTION_TYPE_BUFF).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK7, 
					CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE", SKILL::EMACTION_TYPE_DEBUFF).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK8, 
					CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE", SKILL::EMACTION_TYPE_LIMIT).GetString() );
			}
			else
			{
				SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SKILL_APPLY[SKILL::EMAPPLY_MELEE].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK2, COMMENT::SKILL_APPLY[SKILL::EMAPPLY_RANGE].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK3, COMMENT::SKILL_APPLY[SKILL::EMAPPLY_MAGIC].c_str() );

				SetWin_Text( this, IDC_CHECK_CHK5, 
					COMMENT::SKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_NORMAL].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK6, 
					COMMENT::SKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_BUFF].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK7, 
					COMMENT::SKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_DEBUFF].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK8, 
					COMMENT::SKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_LIMIT].c_str() );
			}

			SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK2, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK3, SW_SHOW ); 

			SetWin_ShowWindow( this, IDC_CHECK_CHK5, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK6, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK7, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK8, SW_SHOW );

			const DWORD dwVAR1 = DWORD(sSpecAddon.sSPEC[dwSLevel].fVAR1);		
			SetWin_Check ( this, IDC_CHECK_CHK1, dwVAR1 & SKILL::EMAPPLY_DWORD_MELEE );
			SetWin_Check ( this, IDC_CHECK_CHK2, dwVAR1 & SKILL::EMAPPLY_DWORD_RANGE );
			SetWin_Check ( this, IDC_CHECK_CHK3, dwVAR1 & SKILL::EMAPPLY_DWORD_MAGIC);

			const DWORD dwVAR2 = DWORD(sSpecAddon.sSPEC[dwSLevel].fVAR2);
			SetWin_Check ( this, IDC_CHECK_CHK5, dwVAR2 & SKILL::EMACTION_TYPE_DWORD_NORMAL);
			SetWin_Check ( this, IDC_CHECK_CHK6, dwVAR2 & SKILL::EMACTION_TYPE_DWORD_BUFF );
			SetWin_Check ( this, IDC_CHECK_CHK7, dwVAR2 & SKILL::EMACTION_TYPE_DWORD_DEBUFF );
			SetWin_Check ( this, IDC_CHECK_CHK8, dwVAR2 & SKILL::EMACTION_TYPE_DWORD_LIMIT);
		}		
		break;
	case EMSPECA_STIGMA:
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Text( this, IDC_CHECK_CHK1, CGameTextMan::GetInstance().GetCommentText("SPEC_STIGMA_TYPE", EMSPEC_STIGMA_TYPE_DAMAGE_NORMAL).GetString());
				SetWin_Text( this, IDC_CHECK_CHK2, CGameTextMan::GetInstance().GetCommentText("SPEC_STIGMA_TYPE", EMSPEC_STIGMA_TYPE_DAMAGE_SKILL).GetString());
				SetWin_Text( this, IDC_CHECK_CHK3, CGameTextMan::GetInstance().GetCommentText("SPEC_STIGMA_TYPE", EMSPEC_STIGMA_TYPE_BUFF_SKILL).GetString());
				SetWin_Text( this, IDC_CHECK_CHK4, CGameTextMan::GetInstance().GetCommentText("SPEC_STIGMA_TYPE", EMSPEC_STIGMA_TYPE_DEBUFF_SKILL).GetString());
				SetWin_Text( this, IDC_CHECK_CHK5, CGameTextMan::GetInstance().GetCommentText("SPEC_STIGMA_TYPE", EMSPEC_STIGMA_TYPE_LIMIT_SKILL).GetString());
			}
			else
			{
				SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SPEC_STIGMA_TYPE[EMSPEC_STIGMA_TYPE_DAMAGE_NORMAL].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK2, COMMENT::SPEC_STIGMA_TYPE[EMSPEC_STIGMA_TYPE_DAMAGE_SKILL].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK3, COMMENT::SPEC_STIGMA_TYPE[EMSPEC_STIGMA_TYPE_BUFF_SKILL].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK4, COMMENT::SPEC_STIGMA_TYPE[EMSPEC_STIGMA_TYPE_DEBUFF_SKILL].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK5, COMMENT::SPEC_STIGMA_TYPE[EMSPEC_STIGMA_TYPE_LIMIT_SKILL].c_str() );
			}
			SetWin_Check ( this, IDC_CHECK_CHK1, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_STIGMA_TYPE_DWORD_DAMAGE_NORMAL );
			SetWin_Check ( this, IDC_CHECK_CHK2, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_STIGMA_TYPE_DWORD_DAMAGE_SKILL );
			SetWin_Check ( this, IDC_CHECK_CHK3, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_STIGMA_TYPE_DWORD_BUFF_SKILL);
			SetWin_Check ( this, IDC_CHECK_CHK4, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_STIGMA_TYPE_DWORD_DEBUFF_SKILL);
			SetWin_Check ( this, IDC_CHECK_CHK5, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_STIGMA_TYPE_DWORD_LIMIT_SKILL );

			SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK2, SW_SHOW ); 
			// 		SetWin_ShowWindow( this, IDC_CHECK_CHK3, SW_SHOW ); // 버프
			// 		SetWin_ShowWindow( this, IDC_CHECK_CHK4, SW_SHOW ); // 디버프
			// 		SetWin_ShowWindow( this, IDC_CHECK_CHK5, SW_SHOW ); // 리미트 사용 하지 않음.
		}
		break;
	case EMSPECA_SKILLDELAY:
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Text( this, IDC_CHECK_CHK1, CGameTextMan::GetInstance().GetCommentText("SPEC_ASIGN_TYPE", EMSPEC_SKILL_ASING_BEFORE).GetString());
				SetWin_Text( this, IDC_CHECK_CHK2, CGameTextMan::GetInstance().GetCommentText("SPEC_ASIGN_TYPE", EMSPEC_SKILL_ASING_AFTER).GetString());
			}
			else
			{
				SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SPEC_ASIGN_TYPE[EMSPEC_SKILL_ASING_BEFORE].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK2, COMMENT::SPEC_ASIGN_TYPE[EMSPEC_SKILL_ASING_AFTER].c_str() );
			}
			SetWin_Check ( this, IDC_CHECK_CHK1, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_SKILL_ASING_DWORD_BEFORE );
			SetWin_Check ( this, IDC_CHECK_CHK2, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_SKILL_ASING_DWORD_AFTER);
			SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK2, SW_SHOW ); 
		}
		break;
	case EMSPECA_LINK_HP:
		{
			SetWin_Enable( this, IDC_STATIC_SPEC_VAR1, SW_HIDE );
			SetWin_Enable( this, IDC_STATIC_SPEC_VAR2, SW_HIDE );
			SetWin_Enable( this, IDC_EDIT_SPEC_VAR1, SW_HIDE );
			SetWin_Enable( this, IDC_EDIT_SPEC_VAR2, SW_HIDE );		

			// 아래의 COMMENT 값은 데이터에서 읽어오지 않음;
			SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SPEC_SKILL_CONTINUE.c_str() );
			SetWin_Check ( this, IDC_CHECK_CHK1, BOOL(sSpecAddon.sSPEC[dwSLevel].fVAR2) );
			SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW );
		}
		break;
    case EMSPECA_VEHICLE_GETOFF:
        SetWin_Enable( this, IDC_STATIC_SPEC_VAR2, SW_HIDE );
        SetWin_Enable( this, IDC_EDIT_SPEC_VAR2, SW_HIDE );		

        SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SPEC_SKILL_CONTINUE.c_str() );
        SetWin_Check ( this, IDC_CHECK_CHK1, BOOL(sSpecAddon.sSPEC[dwSLevel].fVAR2) );
        SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW );
        break;
	case EMSPECA_HALLUCINATE:		
		{	
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Text( this, IDC_CHECK_CHK1, CGameTextMan::GetInstance().GetCommentText("SPEC_HALLUCINATE_TYPE", EMSPEC_HALLUCINATE_TYPE_TRANSFORM).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK2, CGameTextMan::GetInstance().GetCommentText("SPEC_HALLUCINATE_TYPE", EMSPEC_HALLUCINATE_TYPE_EMOTICON).GetString() );
				SetWin_Text( this, IDC_CHECK_CHK3, CGameTextMan::GetInstance().GetCommentText("SPEC_HALLUCINATE_TYPE", EMSPEC_HALLUCINATE_TYPE_NONTARGETABLE).GetString() );
			}
			else
			{
				SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SPEC_HALLUCINATE_TYPE[EMSPEC_HALLUCINATE_TYPE_TRANSFORM].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK2, COMMENT::SPEC_HALLUCINATE_TYPE[EMSPEC_HALLUCINATE_TYPE_EMOTICON].c_str() );
				SetWin_Text( this, IDC_CHECK_CHK3, COMMENT::SPEC_HALLUCINATE_TYPE[EMSPEC_HALLUCINATE_TYPE_NONTARGETABLE].c_str() );
			}
			// 아래의 COMMENT 값은 데이터에서 읽어오지 않음;
			SetWin_Combo_Init(this, IDC_COMBO_REMOVE_ACTION_TYPE, COMMENT::TRANSFORM_SET, GLTRANSFORMSET::g_nTransformSet );
			SetWin_Combo_Sel(this, IDC_COMBO_REMOVE_ACTION_TYPE, COMMENT::TRANSFORM_SET[DWORD(sSpecAddon.sSPEC[dwSLevel].fVAR1)].c_str() );
			SetWin_Enable( this, IDC_STATIC_SPEC_VAR1, SW_HIDE );
			SetWin_Enable( this, IDC_EDIT_SPEC_VAR1, SW_HIDE );
			SetWin_Check ( this, IDC_CHECK_CHK1, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_HALLUCINATE_DWORD_TRANSFORM );
			SetWin_Check ( this, IDC_CHECK_CHK2, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_HALLUCINATE_DWORD_EMOTICON);
			SetWin_Check ( this, IDC_CHECK_CHK3, sSpecAddon.sSPEC[dwSLevel].dwFLAG & EMSPEC_HALLUCINATE_DWORD_NONTARGETABLE );		
			SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_CHECK_CHK2, SW_SHOW );
			SetWin_ShowWindow( this, IDC_CHECK_CHK3, SW_SHOW ); 		
			SetWin_ShowWindow( this, IDC_STATIC_REMOVE_ACTION_TYPE, SW_SHOW ); 
			SetWin_ShowWindow( this, IDC_COMBO_REMOVE_ACTION_TYPE, SW_SHOW ); 		
			SetWin_Combo_Init(this, IDC_COMBO_REMOVE_ACTION_TYPE, COMMENT::TRANSFORM_SET, GLTRANSFORMSET::g_nTransformSet );
			SetWin_Combo_Sel(this, IDC_COMBO_REMOVE_ACTION_TYPE, COMMENT::TRANSFORM_SET[DWORD(sSpecAddon.sSPEC[dwSLevel].fVAR1)].c_str() );
		}
		break;
	case EMSPECA_DOMINATION:		
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetWin_Text( this, IDC_RADIO_SPEC1, CGameTextMan::GetInstance().GetCommentText("SPEC_DOMINATE_TYPE", EMSPEC_DOMINATE_TYPE_SELF).GetString());
				SetWin_Text( this, IDC_RADIO_SPEC2, CGameTextMan::GetInstance().GetCommentText("SPEC_DOMINATE_TYPE", EMSPEC_DOMINATE_TYPE_CASTER).GetString());
				SetWin_Text( this, IDC_RADIO_SPEC3, CGameTextMan::GetInstance().GetCommentText("SPEC_DOMINATE_TYPE", EMSPEC_DOMINATE_TYPE_RANDOM).GetString());
				SetWin_Text( this, IDC_RADIO_SPEC4, CGameTextMan::GetInstance().GetCommentText("SPEC_DOMINATE_TYPE", EMSPEC_DOMINATE_TYPE_FIX).GetString());
			}
			else
			{
				SetWin_Text( this, IDC_RADIO_SPEC1, COMMENT::SPEC_DOMINATE_TYPE[EMSPEC_DOMINATE_TYPE_SELF].c_str() );
				SetWin_Text( this, IDC_RADIO_SPEC2, COMMENT::SPEC_DOMINATE_TYPE[EMSPEC_DOMINATE_TYPE_CASTER].c_str() );
				SetWin_Text( this, IDC_RADIO_SPEC3, COMMENT::SPEC_DOMINATE_TYPE[EMSPEC_DOMINATE_TYPE_RANDOM].c_str() );
				SetWin_Text( this, IDC_RADIO_SPEC4, COMMENT::SPEC_DOMINATE_TYPE[EMSPEC_DOMINATE_TYPE_FIX].c_str() );
			}
			SetWin_ShowWindow( this, IDC_RADIO_SPEC2, SW_SHOW );
			SetWin_ShowWindow( this, IDC_RADIO_SPEC3, SW_SHOW );
			SetWin_ShowWindow( this, IDC_RADIO_SPEC4, SW_SHOW );

			switch ( sSpecAddon.sSPEC[dwSLevel].dwFLAG )
			{
			case EMSPEC_DOMINATE_TYPE_CASTER:
				SetWin_Check ( this, IDC_RADIO_SPEC2, true );
				break;
			case EMSPEC_DOMINATE_TYPE_RANDOM:
				SetWin_Check ( this, IDC_RADIO_SPEC3, true );
				break;
			case EMSPEC_DOMINATE_TYPE_FIX:
				SetWin_Check ( this, IDC_RADIO_SPEC4, true );
				break;
			default:
				SetWin_Check ( this, IDC_RADIO_SPEC3, true );
				break;
			}
		}
		break;
	case EMSPECA_RANDOM_EXP_RATE:
		SetWin_ShowWindow( this, IDC_SPEC_VAR_LIST, SW_SHOW );
		UpdateSpecOptionSkillSpecial();
		break;
	case EMSPECA_SKILL_LINK:
		SetWin_ShowWindow( this, IDC_EDIT_DEFENSE_SKILL_MID, SW_SHOW );
		SetWin_ShowWindow( this, IDC_EDIT_DEFENSE_SKILL_SID, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_DEFENSE_SKILL, SW_SHOW );

		SetWin_Num_int ( this, IDC_EDIT_DEFENSE_SKILL_MID, sSpecAddon.sSPEC[dwSLevel].dwNativeID.wMainID );
		SetWin_Num_int ( this, IDC_EDIT_DEFENSE_SKILL_SID, sSpecAddon.sSPEC[dwSLevel].dwNativeID.wSubID );

		SetWin_ShowWindow( this, IDC_EDIT_TRIGGER_TYPE_LINK_MID, SW_SHOW );
		SetWin_ShowWindow( this, IDC_EDIT_TRIGGER_TYPE_LINK_SID, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_TRIGGER_TYPE_LINK, SW_SHOW );

		SetWin_Num_float( this, IDC_EDIT_SPEC_VAR1, sSpecAddon.sSPEC[dwSLevel].fVAR1 );
		SetWin_Num_int ( this, IDC_EDIT_TRIGGER_TYPE_LINK_MID, sSpecAddon.sSPEC[dwSLevel].dwLinkID.wMainID );
		SetWin_Num_int ( this, IDC_EDIT_TRIGGER_TYPE_LINK_SID, sSpecAddon.sSPEC[dwSLevel].dwLinkID.wSubID );
		break;
	case EMSPECA_AIRBORNE:
		SetWin_Text( this, IDC_CHECK_CHK1, COMMENT::SKILL_KEYWORD[SKILL::EMKEYWORD_PILEUP].c_str() );
		SetWin_ShowWindow( this, IDC_CHECK_CHK1, SW_SHOW ); 
		SetWin_Check ( this, IDC_CHECK_CHK1, sSpecAddon.sSPEC[dwSLevel].dwFLAG != 0);
		break;
	case EMSPECA_SUMMON_CHANGE_SKILLPAGE:
	case EMSPECA_SUMMON_EVENT_SKILL:
	case EMSPECA_SUMMON_TARGETING:
	case EMSPECA_SUMMON_RUNAWAY:
		break;
	case EMSPECA_DELAY_ACTION_SKILL:
		SetWin_Num_float( this, IDC_EDIT_SPEC_VAR1, sSpecAddon.sSPEC[dwSLevel].fVAR1 );
		SetWin_Num_float( this, IDC_EDIT_SPEC_VAR2, sSpecAddon.sSPEC[dwSLevel].fVAR2 );    
		break;
	case EMSPECA_CLONE:
		SetWin_Num_float( this, IDC_EDIT_SPEC_VAR1, sSpecAddon.sSPEC[dwSLevel].fVAR1 );
		SetWin_Num_float( this, IDC_EDIT_SPEC_VAR2, sSpecAddon.sSPEC[dwSLevel].fVAR2 );    
		break;
    case EMSPECA_MOVEVELO_LIMIT:
        SetWin_Num_float( this, IDC_EDIT_SPEC_VAR1, sSpecAddon.sSPEC[dwSLevel].fVAR1 );
        SetWin_Num_float( this, IDC_EDIT_SPEC_VAR2, sSpecAddon.sSPEC[dwSLevel].fVAR2 );    
        break;

	case EMSPECA_REMOVE_SKILL_EFFECT:
		{			
			SetWin_ShowWindow( this, IDC_STATIC_SPECIAL_IMMUNE, SW_SHOW);
			SetWin_ShowWindow( this, IDC_LIST_SPECIAL_IMMUNE, SW_SHOW);
			SetWin_ShowWindow( this, IDC_BUTTON_SPECIAL_IMMUNE_ADD, SW_SHOW);
			SetWin_ShowWindow( this, IDC_BUTTON_SPECIAL_IMMUNE_DEL, SW_SHOW); 

			SetWin_Text( this, IDC_STATIC_SPECIAL_IMMUNE, "Remove SkillEffect" );

			SetWin_Num_int( this, IDC_EDIT_SPEC_VAR1, sSpecAddon.sSPEC[dwSLevel].nVAR1 );

			bool bChecked = ( sSpecAddon.sSPEC[dwSLevel].nVAR2 >= 1 ) ? true : false;		
			SetWin_Check ( this, IDC_CHECK_CHK1, bChecked );

			if( m_nSelectLevel != 0 )
			{
				SetWin_Enable( this, IDC_STATIC_SPECIAL_IMMUNE, FALSE);
				SetWin_Enable( this, IDC_LIST_SPECIAL_IMMUNE, FALSE);
				SetWin_Enable( this, IDC_BUTTON_SPECIAL_IMMUNE_ADD, FALSE);
				SetWin_Enable( this, IDC_BUTTON_SPECIAL_IMMUNE_DEL, FALSE);
			}
			else
			{
				SetWin_Enable( this, IDC_STATIC_SPECIAL_IMMUNE, TRUE);
				SetWin_Enable( this, IDC_LIST_SPECIAL_IMMUNE, TRUE);
				SetWin_Enable( this, IDC_BUTTON_SPECIAL_IMMUNE_ADD, TRUE);
				SetWin_Enable( this, IDC_BUTTON_SPECIAL_IMMUNE_DEL, TRUE);
			}

			SetWin_Enable( this, IDC_EDIT_SPEC_VAR1, SW_HIDE );

			DWORD dwValue = (DWORD)sSpecAddon.sSPEC[dwSLevel].nVAR1;

			if( dwValue > 0 )
			{
				EMSPEC_ADDON eTemp = (EMSPEC_ADDON)dwValue;

				if( eTemp != EMSPECA_NULL )
				{
					CString strTemp = COMMENT::SPEC_ADDON[eTemp].c_str();

					m_listSpecImmune.SetItemData( m_listSpecImmune.AddString(strTemp), (DWORD)eTemp);
				}
			}
		}
		break;
	}

    SetWin_Text( this, IDC_STATIC_SPEC_TOOLTIP, 
        GLSkillToolTip::MakeTooltipSting_In_Skill_Special(
        GetWin_Num_float ( this, IDC_EDIT_AGE ),
        GetWin_Num_float ( this, IDC_EDIT_SPEC_VAR1 ) * COMMENT::SPEC_ADDON_VAR1_SCALE[sSpecAddon.emSPEC],
        GetWin_Num_float ( this, IDC_EDIT_SPEC_VAR2 ) * COMMENT::SPEC_ADDON_VAR2_SCALE[sSpecAddon.emSPEC],
        sSpecAddon.emSPEC,
        sSpecAddon.sSPEC[dwSLevel],
        m_Item.m_sBASIC.emIMPACT_TAR,
		true,
		m_Item.m_sAPPLY.sCAData.sSPEC_CA[dwSLevel] ).c_str()	
        );
}

void CSkillLevelData::OnBnClickedRadioSpec(const DWORD dwButtonIndex)
{
	DWORD dwSLevel = m_nSelectLevel;
	SKILL::SSPEC_ADDON sSpecAddon = m_Item.m_sAPPLY.vecSPEC[m_nSelectSpec];

	switch ( sSpecAddon.emSPEC )
	{
	case EMSPECA_INVISIBLE:
		sSpecAddon.sSPEC[dwSLevel].fVAR2 = float(dwButtonIndex);
		SetWin_Num_int( this, IDC_EDIT_SPEC_VAR2, dwButtonIndex );
		break;

	

	case EMSPECA_VEHICLE_GETOFF:
		sSpecAddon.sSPEC[dwSLevel].fVAR2 = float(dwButtonIndex);
		break;

    case EMSPECA_LINK_HP:
        sSpecAddon.sSPEC[dwSLevel].fVAR2 = float(dwButtonIndex);
        break;

	case EMSPECA_SWAPPOS:
		sSpecAddon.sSPEC[dwSLevel].dwFLAG = dwButtonIndex;
		break;
	}
}

BOOL CSkillLevelData::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if ( pMsg->message == WM_KEYUP )
	{
		if ( pMsg->wParam == VK_F11 )
		{
			// 저장.
			InverseUpdateSkillLevelData();

			// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
			if ( m_nSelectLevel > 0 )	m_nSelectLevel--;

			// 불러오기.
			UpdateSkillLevelData();
		}
		else if ( pMsg->wParam == VK_F12 )
		{
			// 저장.
			InverseUpdateSkillLevelData();

			if ( m_nSelectLevel < SKILL::MAX_LEVEL-1 )	m_nSelectLevel++;

			// 불러오기.
			UpdateSkillLevelData();
		}
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CSkillLevelData::UpdateSpecOptionSkillSpecial()
{
	const SKILL::SSPEC_ADDON sSpecAddon = m_Item.m_sAPPLY.vecSPEC[m_nSelectSpec];	

	//	특수기능 : 상태이상 방지(회복)
	switch ( sSpecAddon.emSPEC )
	{
	case EMSPECA_RANDOM_EXP_RATE:
		{
			m_listSpecOption.ResetContent();
			SKILL::ExperiencePointSettings* const pExpSettings(SKILL::ExperiencePointSettings::getInstance());

			const DWORD nTableIndex(GetWin_Num_int ( this, IDC_EDIT_SPEC_VAR1 ));

			const DWORD nItem(pExpSettings->getNItem(nTableIndex));
			if ( nItem == 0 )
			{
				m_listSpecOption.AddString("~~~~~~~~~~~~~~~~~~");
				m_listSpecOption.AddString("not exist table at");
				m_listSpecOption.AddString("/data/Glogicserver/scripts/");
				m_listSpecOption.AddString("SkillSpecial_ExperiencePoint.lua");
				m_listSpecOption.AddString("~~~~~~~~~~~~~~~~~~");
			}				
			else
			{
				m_listSpecOption.AddString("~~~~~~~~~~~~~~~~~~");
				m_listSpecOption.AddString("/data/Glogicserver/scripts/");
				m_listSpecOption.AddString("SkillSpecial_ExperiencePoint.lua");
				m_listSpecOption.AddString("~~~~~~~~~~~~~~~~~~");
				const SKILL::ExperiencePointSettings::Table& expTable = pExpSettings->getTable(nTableIndex);
				for ( DWORD _i(0) ; _i < nItem; ++_i )
				{					
					const float fValue[3] =
					{
						expTable.getSelectRate(_i),
						expTable.getMinRate(_i),
						expTable.getMaxRate(_i)
					};

					CString strOption;
					strOption.Format("%.2f / min %.2f, max %.2f", fValue[0], fValue[1], fValue[2]);
					m_listSpecOption.AddString ( strOption );
				}
			}			
		}
		break;
	}
}
void CSkillLevelData::UpdateToolTipSkillSpecial()
{
    DWORD dwSLevel = m_nSelectLevel;

    if( m_nSelectSpec < 0 )
        return;

    if( m_nSelectSpec >= (int)m_Item.m_sAPPLY.vecSPEC.size() )
        return;

    SKILL::SSPEC_ADDON sSpecAddon = m_Item.m_sAPPLY.vecSPEC[m_nSelectSpec];

    SetWin_Text( this, IDC_STATIC_SPEC_TOOLTIP, 
        GLSkillToolTip::MakeTooltipSting_In_Skill_Special(
        GetWin_Num_float ( this, IDC_EDIT_AGE ),
        GetWin_Num_float ( this, IDC_EDIT_SPEC_VAR1 ) * COMMENT::SPEC_ADDON_VAR1_SCALE[sSpecAddon.emSPEC],
        GetWin_Num_float ( this, IDC_EDIT_SPEC_VAR2 ) * COMMENT::SPEC_ADDON_VAR2_SCALE[sSpecAddon.emSPEC],
        sSpecAddon.emSPEC,
        sSpecAddon.sSPEC[dwSLevel],
        m_Item.m_sBASIC.emIMPACT_TAR,
		true,
		m_Item.m_sAPPLY.sCAData.sSPEC_CA[dwSLevel] ).c_str()
        );
}

void CSkillLevelData::UpdateToolTipSkillImpact()
{
    if ( m_nSelectImpact < 0 )
        return;
    if ( m_nSelectImpact >= (int)m_Item.m_sAPPLY.vecADDON.size() )
        return;


    SetWin_Text( this, IDC_STATIC_IMPACT_TOOLTIP, 
        GLSkillToolTip::MakeTooltipSting_In_Skill_Impact(
        GetWin_Num_float ( this, IDC_EDIT_AGE ),
        GetWin_Num_float ( this, IDC_EDIT_IMPACT_VAR ),
        GetWin_Num_float ( this, IDC_EDIT_IMPACT_VAR2) * GetWin_Num_float ( this, IDC_EDIT_IMPACT_VAR ),
        m_Item.m_sBASIC.emROLE,
        m_Item.m_sAPPLY.vecADDON[m_nSelectImpact].emADDON).c_str() 
        );
}

void CSkillLevelData::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 . 
#ifdef READTOOL_PARAM
	
	const int nSkipNum = 8;
	const int nSkipID[nSkipNum] = { IDC_BUTTON_PREV, IDC_BUTTON_NEXT, IDC_BUTTON_CANCEL, 
									IDC_BUTTON_OK, IDC_BUTTON_LEVEL_DATA_LEFT,
									IDC_BUTTON_LEVEL_DATA_RIGHT, IDC_LIST_IMPACT_ADDON_SEL,
									IDC_LIST_SPEC_TYPE_SEL };

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

void CSkillLevelData::OnBnClickedSpecialButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CTransformDlg transformDlg;

	transformDlg.m_sSPECIAL_SKILL = m_Item.m_sSPECIAL_SKILL;

	if( transformDlg.DoModal() == IDOK )
	{
		m_Item.m_sSPECIAL_SKILL = transformDlg.m_sSPECIAL_SKILL;
	}
}


void CSkillLevelData::OnBnClickedSpecialButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CCreatureSetDlg creatureSetDlg;

	creatureSetDlg.m_sSPECIAL_SKILL = m_Item.m_sSPECIAL_SKILL;
	creatureSetDlg.m_nSelectLevel   = m_nSelectLevel;

	if( creatureSetDlg.DoModal() == IDOK )
		m_Item.m_sSPECIAL_SKILL = creatureSetDlg.m_sSPECIAL_SKILL;
}

void CSkillLevelData::OnBnClickedButtonLevelDataLeft()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( m_nSelectLevel <= 0  ) return;

	InverseUpdateSkillLevelData();
	
	m_nSelectLevel--;

	UpdateSkillLevelData();
}

void CSkillLevelData::OnBnClickedButtonLevelDataRight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( m_nSelectLevel >= SKILL::MAX_LEVEL-1  ) return;

	InverseUpdateSkillLevelData();
	
	m_nSelectLevel++;

	UpdateSkillLevelData();
}

void CSkillLevelData::OnBnClickedButtonLevelDataBunchLeft()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( m_nSelectLevel <= 0  ) return;

	InverseUpdateSkillLevelData();

	m_nSelectLevel-=5;
	if ( m_nSelectLevel < 0 )
		m_nSelectLevel = 0;

	UpdateSkillLevelData();
}

void CSkillLevelData::OnBnClickedButtonLevelDataBunchRight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( m_nSelectLevel >= SKILL::MAX_LEVEL-1  ) return;

	InverseUpdateSkillLevelData();

	m_nSelectLevel+=5;
	if ( SKILL::MAX_LEVEL <= m_nSelectLevel )
		m_nSelectLevel = SKILL::MAX_LEVEL-1;

	UpdateSkillLevelData();
}


void CSkillLevelData::OnLbnSelchangeListImpactAddonSel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateImpactAddon();
	m_nSelectImpact = m_listImpactAddon.GetCurSel();
	UpdateImpactAddon();
}

void CSkillLevelData::OnLbnSelchangeListSpecTypeSel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateSpecAddon();
	m_nSelectSpec = m_listSpecAddon.GetCurSel();
	UpdateSpecAddon();
	SetReadMode();
}

void CSkillLevelData::OnLbnSelchangeListSpecCAOption()
{
	
}

void CSkillLevelData::OnLbnSelchangeListSpecVar()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CSkillLevelData::OnEnChangeEditSpecVar2()
{
    UpdateToolTipSkillSpecial();
}


void CSkillLevelData::OnBnClickedRadioSpec1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	OnBnClickedRadioSpec(0);
    UpdateToolTipSkillSpecial();

    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}

void CSkillLevelData::OnBnClickedRadioSpec2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedRadioSpec(1);
    UpdateToolTipSkillSpecial();

    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}

void CSkillLevelData::OnBnClickedRadioSpec3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedRadioSpec(2);
    UpdateToolTipSkillSpecial();

    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}

void CSkillLevelData::OnBnClickedRadioSpec4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedRadioSpec(3);
    UpdateToolTipSkillSpecial();

    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}


void CSkillLevelData::OnEnUpdateEditSpecVar2()
{
    // TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
    // CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
    // IParam 마스크와 OR 연산하여 설정된 ENM_SCROLL 플래그를 지정하여
    // 편집 컨트롤이 바뀐 텍스트를 표시하려고 함을 나타냅니다.

    // TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CSkillLevelData::OnEnChangeEditSpecVar1()
{
	UpdateSpecOptionSkillSpecial();
    UpdateToolTipSkillSpecial();
}

void CSkillLevelData::OnEnChangeEditAge()
{
    UpdateToolTipSkillSpecial();
    UpdateToolTipSkillImpact();
}

void CSkillLevelData::OnBnClickedCheckChk1()
{
    UpdateToolTipSkillSpecial();
    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}

void CSkillLevelData::OnBnClickedCheckChk2()
{
    UpdateToolTipSkillSpecial();
    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}

void CSkillLevelData::OnBnClickedCheckChk3()
{
    UpdateToolTipSkillSpecial();
    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}

void CSkillLevelData::OnBnClickedCheckChk4()
{
    UpdateToolTipSkillSpecial();
    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}

void CSkillLevelData::OnBnClickedCheckChk5()
{
    UpdateToolTipSkillSpecial();
    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}

void CSkillLevelData::OnBnClickedCheckChk6()
{
    UpdateToolTipSkillSpecial();
    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}

void CSkillLevelData::OnBnClickedCheckChk7()
{
    UpdateToolTipSkillSpecial();
    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}

void CSkillLevelData::OnBnClickedCheckChk8()
{
    UpdateToolTipSkillSpecial();
    InverseUpdateSpecAddon();
    UpdateSpecAddon();
    SetReadMode();
}

void CSkillLevelData::OnCbnSelchangeComboRemoveActionType()
{
    UpdateToolTipSkillSpecial();

	DWORD dwSLevel = m_nSelectLevel;

	if( m_nSelectSpec < 0 )
		return;

	if( m_nSelectSpec >= (int)m_Item.m_sAPPLY.vecSPEC.size() )
		return;

	SKILL::SSPEC_ADDON sSpecAddon = m_Item.m_sAPPLY.vecSPEC[m_nSelectSpec];

	switch( sSpecAddon.emSPEC )
	{
	case EMSPECA_DISGUISE :
		{
			sSpecAddon.sSPEC[dwSLevel].fVAR2 = float(GetWin_Combo_Sel(this, IDC_COMBO_REMOVE_ACTION_TYPE));
			SetWin_Num_float( this, IDC_EDIT_SPEC_VAR2, sSpecAddon.sSPEC[dwSLevel].fVAR2 );

			if( (int)sSpecAddon.sSPEC[dwSLevel].fVAR2 == 0 )
			{
				SetWin_ShowWindow( this, IDC_STATIC_MAN, SW_HIDE ); 
				SetWin_ShowWindow( this, IDC_STATIC_WOMAN, SW_HIDE ); 
			}
			else
			{
				if( DWORD(sSpecAddon.sSPEC[dwSLevel].fVAR2) > 0 )
				{
					std::string strData = COMMENT::TFCHAR_SET[DWORD(sSpecAddon.sSPEC[dwSLevel].fVAR2)];
					std::string strTemp = strData;

					std::string::size_type iNum = strData.find(".chf");

					if( iNum != std::string::npos ) {
						boost::algorithm::erase_first( strData, ".chf" );		
						strTemp = strData;
					}

					SetWin_ShowWindow( this, IDC_STATIC_MAN, SW_SHOW ); 
					SetWin_ShowWindow( this, IDC_STATIC_WOMAN, SW_SHOW ); 

					std::string strM = strTemp + "_m.chf";
					std::string strW = strTemp + "_w.chf";

					SetWin_Text( this, IDC_STATIC_MAN, strM.c_str() );
					SetWin_Text( this, IDC_STATIC_WOMAN, strW.c_str() );
				}	
			}
		}
		break;
	}
}

void CSkillLevelData::OnEnChangeEditImpactVar()
{
    UpdateToolTipSkillImpact();
}

void CSkillLevelData::OnTimer(UINT_PTR nIDEvent)
{
    switch(nIDEvent)
    {
    case TIMER_SKILLLEVELDATA_SAVE_STATE_ID:

        SetWin_Text ( this, IDC_EDIT_SAVE_STATE, "" );
        KillTimer ( TIMER_SKILLLEVELDATA_SAVE_STATE_ID );

        break;
    }

    CPropertyPage::OnTimer(nIDEvent);
}

void CSkillLevelData::OnBnClickedButtonSpecCaAdd()
{
	if( m_listSpecCAOption.GetCount() >= SKILL::MAX_ADDON || m_Item.m_sAPPLY.sCAData.sSPEC_CA[m_nSelectLevel].bAllApply ) return;

	CSelectDialog dlg;

	std::string szTemp[EMSPECA_CAK_MAX];

	for( int i = 0; i < EMSPECA_CAK_MAX; ++i )
	{
		if( COMMENT::SPEC_CA[i] > 0 && COMMENT::SPEC_CA[i] < EMSPECA_NSIZE )
		{
			szTemp[i] = COMMENT::SPEC_ADDON[COMMENT::SPEC_CA[i]].c_str();
		}
		else
		{
			szTemp[i] = "NULL_CA";
		}
	}

	dlg.SetStringList( szTemp, EMSPECA_CAK_MAX );	
	dlg.m_nSelect = EMSPECA_NULL;

	if ( dlg.DoModal() == IDOK )
	{
		int iRefN = (int)dlg.m_dwData;

		if( szTemp[iRefN].compare("NULL_CA") == 0) return;
		
		EMSPEC_ADDON eAD = (EMSPEC_ADDON)COMMENT::SPEC_CA[iRefN];

		const SKILL::SSPEC_ADDON sSpecAddon = m_Item.m_sAPPLY.vecSPEC[m_nSelectSpec];

		if( sSpecAddon.emSPEC == EMSPECA_COUNTATTACK )
		{						
			m_Item.m_sAPPLY.sCAData.sSPEC_CA[m_nSelectLevel].emSPEC[m_listSpecCAOption.GetCount()] = eAD;
			
			CString strNAME = COMMENT::SPEC_ADDON[eAD].c_str();

			m_listSpecCAOption.SetItemData ( m_listSpecCAOption.AddString ( strNAME ), eAD );

		}
	}
}

void CSkillLevelData::OnBnClickedButtonSpecCaDel()
{
	int iIdx = m_listSpecCAOption.GetCurSel();

	const SKILL::SSPEC_ADDON sSpecAddon = m_Item.m_sAPPLY.vecSPEC[m_nSelectSpec];

	if( sSpecAddon.emSPEC == EMSPECA_COUNTATTACK )
	{		
		m_listSpecCAOption.DeleteString(iIdx);

		int iCnt = m_listSpecCAOption.GetCount();

		m_Item.m_sAPPLY.sCAData.sSPEC_CA[m_nSelectLevel].resetemSpec();

		for( int i = 0; i < iCnt; ++i )
		{
			EMSPEC_ADDON eData = (EMSPEC_ADDON)m_listSpecCAOption.GetItemData(i);

			m_Item.m_sAPPLY.sCAData.sSPEC_CA[m_nSelectLevel].emSPEC[i] = eData;
		}
	}	
}

void CSkillLevelData::OnBnClickedCheckCADamage()
{
	UpdateToolTipSkillSpecial();
	InverseUpdateSpecAddon();
	UpdateSpecAddon();
	SetReadMode();
}

void CSkillLevelData::OnBnClickedCheckCAAll()
{
	UpdateToolTipSkillSpecial();
	InverseUpdateSpecAddon();
	UpdateSpecAddon();
	SetReadMode();
}

void CSkillLevelData::OnBnClickedButtonSpecImmuneAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strISPEC_ADDON[EMSPECA_NSIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("SPEC_ADDON", strISPEC_ADDON, CGameTextMan::EM_COMMENT_TEXT, EMSPECA_NSIZE);
		dlg.SetStringList( strISPEC_ADDON, EMSPECA_NSIZE );	
		dlg.m_nSelect = EMSPECA_NULL;
		if ( dlg.DoModal() == IDOK )
		{
			if ( dlg.m_dwData == EMSPECA_NULL )	return;

			m_listSpecImmune.ResetContent();

			SKILL::SSPEC_ADDON sAddon;
			sAddon.emSPEC = (EMSPEC_ADDON) dlg.m_dwData;

			m_Item.m_sAPPLY.vecSPEC[m_nSelectSpec].sSPEC[m_nSelectLevel].nVAR1 = (int)sAddon.emSPEC;

			CString strNAME = strISPEC_ADDON[sAddon.emSPEC].c_str();
			int nIndex = m_listSpecImmune.AddString ( strNAME );
			m_listSpecImmune.SetItemData ( nIndex, sAddon.emSPEC );

			SetWin_Num_int( this, IDC_EDIT_SPEC_VAR1, (int)sAddon.emSPEC );
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::SPEC_ADDON, EMSPECA_NSIZE );	
		dlg.m_nSelect = EMSPECA_NULL;
		if ( dlg.DoModal() == IDOK )
		{
			if ( dlg.m_dwData == EMSPECA_NULL )	return;

			m_listSpecImmune.ResetContent();

			SKILL::SSPEC_ADDON sAddon;
			sAddon.emSPEC = (EMSPEC_ADDON)dlg.m_dwData;
			
			m_Item.m_sAPPLY.vecSPEC[m_nSelectSpec].sSPEC[m_nSelectLevel].nVAR1 = (int)sAddon.emSPEC;

			CString strNAME = COMMENT::SPEC_ADDON[sAddon.emSPEC].c_str();
			int nIndex = m_listSpecImmune.AddString ( strNAME );
			m_listSpecImmune.SetItemData ( nIndex, sAddon.emSPEC );

			SetWin_Num_int( this, IDC_EDIT_SPEC_VAR1, (int)sAddon.emSPEC );
		}
	}
}

void CSkillLevelData::OnBnClickedButtonSpecImmuneDel()
{
	if( m_nSelectSpec < 0 || m_nSelectLevel < 0 ) return;

	m_listSpecImmune.ResetContent();

	m_Item.m_sAPPLY.vecSPEC[m_nSelectSpec].sSPEC[m_nSelectLevel].fVAR1 = 0;
}

void CSkillLevelData::OnBnClickedCheckTarget()
{
	UpdateToolTipSkillSpecial();
	InverseUpdateSpecAddon();
	UpdateSpecAddon();
	SetReadMode();
}

void CSkillLevelData::OnBnClickedCheckBufDel()
{
	UpdateToolTipSkillSpecial();
	InverseUpdateSpecAddon();
	UpdateSpecAddon();
	SetReadMode();
}

void CSkillLevelData::OnEnChangeEditSpecMid()
{
	InverseUpdateSpecAddon();
	UpdateToolTipSkillSpecial();
}

void CSkillLevelData::OnEnChangeEditSpecSid()
{
	InverseUpdateSpecAddon();
	UpdateToolTipSkillSpecial();
}
