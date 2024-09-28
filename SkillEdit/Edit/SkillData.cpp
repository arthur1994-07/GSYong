// ItemProperty2.cpp : 구현 파일입니다.
//

#include "stdafx.h"


#include "../SkillEdit.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxTools/TextureManager.h"

#include "./SkillData.h"
#include "./SheetWithTab.h"
#include "../SelectDialog.h"

#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLStringTable.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinDataDummy.h"

// CSkillData 대화 상자입니다.


IMPLEMENT_DYNAMIC(CSkillData, CPropertyPage)
CSkillData::CSkillData( LOGFONT logfont )
	: CPropertyPage(CSkillData::IDD)
	, m_pFont ( NULL )
	, m_nSelEffElmt(0)
{
	m_bDlgInit = FALSE;
	
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CSkillData::~CSkillData()
{
	SAFE_DELETE ( m_pFont );
}

void CSkillData::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IMPACT_ADDON, m_listImpactAddon );
	DDX_Control(pDX, IDC_LIST_SPEC_TYPE, m_listSpecAddon );
	DDX_Control(pDX, IDC_LIST_ATTACKPOINT, m_listAttStrikeAni );
}


BEGIN_MESSAGE_MAP(CSkillData, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_SELFBODYEFFECT, OnBnClickedButtonSelfbodyeffect)
	ON_BN_CLICKED(IDC_BUTTON_TARGETBODYEFFECT, OnBnClickedButtonTargetbodyeffect)
	ON_BN_CLICKED(IDC_BUTTON_TARGETEFFECT, OnBnClickedButtonTargeteffect)
	ON_BN_CLICKED(IDC_BUTTON_ICONFILE, OnBnClickedButtonIconfile)
	ON_BN_CLICKED(IDC_BUTTON_SELFZONEEFFECT, OnBnClickedButtonSelfzoneeffect)
	ON_BN_CLICKED(IDC_BUTTON_SELFZONEEFFECT2, OnBnClickedButtonSelfzoneeffect2)
    ON_BN_CLICKED(IDC_BUTTON_SELFZONEEFFECT3, OnBnClickedButtonSelfzoneeffect3)
	ON_BN_CLICKED(IDC_BUTTON_TARGZONEEFFECT, OnBnClickedButtonTargzoneeffect)
	ON_BN_CLICKED(IDC_BUTTON_TARGZONEEFFECT2, OnBnClickedButtonTargzoneeffect2)
	ON_BN_CLICKED(IDC_BUTTON_TARGZONEEFFECT3, OnBnClickedButtonTargzoneeffect3)
	ON_BN_CLICKED(IDC_BUTTON_TARGETBODYEFFECT2, OnBnClickedButtonTargetbodyeffect2)
	ON_BN_CLICKED(IDC_BUTTON_HOLDOUT, OnBnClickedButtonHoldout)
	ON_EN_CHANGE(IDC_EDIT_SKILLNAME, OnEnChangeEditSkillname)
	ON_EN_CHANGE(IDC_EDIT_SKILLDESC, OnEnChangeEditSkilldesc)
	ON_BN_CLICKED(IDC_BUTTON_EFF_ELMT_LEFT, OnBnClickedButtonEffElmtLeft)
	ON_BN_CLICKED(IDC_BUTTON_EFF_ELMT_RIGHT, OnBnClickedButtonEffElmtRight)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_TYPE, OnBnClickedButtonSkillType)
	ON_BN_CLICKED(IDC_BUTTON_ELEMENT, OnBnClickedButtonElement)
	ON_BN_CLICKED(IDC_BUTTON_BLOW_TYPE, OnBnClickedButtonBlowType)
	ON_BN_CLICKED(IDC_BUTTON_SPECIAL, OnBnClickedButtonSpecial)
	ON_BN_CLICKED(IDC_BUTTON_MAINTYPE, OnBnClickedButtonMaintype)
	ON_BN_CLICKED(IDC_BUTTON_SUBTYPE, OnBnClickedButtonSubtype)
	ON_BN_CLICKED(IDC_BUTTON_ATTACKLEFT, OnBnClickedButtonAttackleft)
	ON_BN_CLICKED(IDC_BUTTON_ATTACKRIGHT, OnBnClickedButtonAttackright)
	ON_BN_CLICKED(IDC_BUTTON_IMPACT_ADDON_ADD, OnBnClickedButtonImpactAddonAdd)
	ON_BN_CLICKED(IDC_BUTTON_IMPACT_ADDON_DEL, OnBnClickedButtonImpactAddonDel)
	ON_BN_CLICKED(IDC_BUTTON_SPEC_TYPE_ADD, OnBnClickedButtonSpecTypeAdd)
	ON_BN_CLICKED(IDC_BUTTON_SPEC_TYPE_DEL, OnBnClickedButtonSpecTypeDel)
	ON_BN_CLICKED(IDC_BUTTON_VEHICLE_MAINTYPE, OnBnClickedButtonVehicleMaintype)
    ON_BN_CLICKED(IDC_BUTTON_TARGZONEEFFECT4, &CSkillData::OnBnClickedButtonTargzoneeffect4)
	ON_BN_CLICKED(IDC_CHECK_HIDDENWEAPON, OnBnClickedCheckHiddenWeapon)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CSkillData 메시지 처리기입니다.

void CSkillData::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveSkillTreePage ();
}

void CSkillData::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	InverseUpdateItems();	

	GLSkillMan::GetInstance().SetData ( m_Item.m_sBASIC.sNATIVEID.wMainID, m_Item.m_sBASIC.sNATIVEID.wSubID, new GLSKILL(m_Item) );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CGameTextMan::GetInstance().InsertString( strKeyName.c_str(), strName.c_str(), CGameTextMan::EM_SKILL_TEXT );
		CGameTextMan::GetInstance().InsertString( strKeyDesc.c_str(), strDesc.c_str(), CGameTextMan::EM_SKILL_TEXT );
	}
	else
	{
		GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::SKILL );
		GLStringTable::GetInstance().InsertString ( strKeyDesc, strDesc, GLStringTable::SKILL );
	}

	//m_pSheetTab->ActiveSkillTreePage ();

    SetWin_Text ( this, IDC_EDIT_SAVE_STATE, "Save Successed." );

    // SetTimer는 기존에 같은 ID 로 정의된 이벤트가 있으면
    // 그 이벤트를 죽이고 새로운 이벤트를 덮어씌으무로
    // 그에 따른 예외처리는 따로 하지 않습니다.
    SetTimer ( TIMER_SKILLDATA_SAVE_STATE_ID, SAVE_MESSAGE_APPEAR_TIME, NULL );
}

BOOL CSkillData::SetItem ( DWORD GUID )
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

void CSkillData::UpdateItems ()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		SetWin_Combo_Sel ( this, IDC_COMBO_ROLE,	CGameTextMan::GetInstance().GetCommentText("SKILL_ROLE", m_Item.m_sBASIC.emROLE).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_APPLY,	CGameTextMan::GetInstance().GetCommentText("SKILL_APPLY",m_Item.m_sBASIC.emAPPLY).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_ACTION_TYPE, CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE",m_Item.m_sBASIC.emACTION_TYPE).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_IMPACT_TAR,	CGameTextMan::GetInstance().GetCommentText("IMPACT_TAR",m_Item.m_sBASIC.emIMPACT_TAR).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_IMPACT_REALM,	CGameTextMan::GetInstance().GetCommentText("IMPACT_REALM",m_Item.m_sBASIC.emIMPACT_REALM).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_IMPACT_SIDE, CGameTextMan::GetInstance().GetCommentText("IMPACT_SIDE",m_Item.m_sBASIC.emIMPACT_SIDE).GetString());
		SetWin_Text( this, IDC_EDIT_ATTACKLEFT,		CGameTextMan::GetInstance().GetCommentText("SKILLATTACK",m_Item.m_sBASIC.emUSE_LITEM).GetString());	
		SetWin_Text( this, IDC_EDIT_ATTACKRIGHT,	CGameTextMan::GetInstance().GetCommentText("SKILLATTACK",m_Item.m_sBASIC.emUSE_RITEM).GetString());	
		SetWin_Text( this, IDC_EDIT_SKILL_TYPE, CGameTextMan::GetInstance().GetCommentText("SKILL_TYPES",m_Item.m_sAPPLY.emBASIC_TYPE).GetString());	
		SetWin_Text( this, IDC_EDIT_ELEMENT, CGameTextMan::GetInstance().GetCommentText("ELEMENT",m_Item.m_sAPPLY.emELEMENT).GetString());	
		SetWin_Text( this, IDC_EDIT_BLOW_TYPE, CGameTextMan::GetInstance().GetCommentText("BLOW",m_Item.m_sAPPLY.emSTATE_BLOW).GetString());	
		m_listImpactAddon.ResetContent();
		m_listSpecAddon.ResetContent();
		for ( DWORD i=0 ; i < m_Item.m_sAPPLY.vecADDON.size(); ++i )
		{
			CString strNAME = CGameTextMan::GetInstance().GetCommentText("IMPACT_ADDON",m_Item.m_sAPPLY.vecADDON[i].emADDON);
			int nIndex = m_listImpactAddon.AddString ( strNAME );
			m_listImpactAddon.SetItemData ( nIndex, i );
		}
		for ( DWORD i=0 ; i < m_Item.m_sAPPLY.vecSPEC.size(); ++i )
		{
			CString strNAME = CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON", m_Item.m_sAPPLY.vecSPEC[i].emSPEC);
			int nIndex = m_listSpecAddon.AddString ( strNAME );
			m_listSpecAddon.SetItemData ( nIndex, i );
		}
		SetWin_Text( this, IDC_EDIT_SPECIAL,	CGameTextMan::GetInstance().GetCommentText("SPECIAL_SKILL_TYPE",m_Item.m_sSPECIAL_SKILL.emSSTYPE).GetString());
		SetWin_Text( this, IDC_EDIT_MAINTYPE,	CGameTextMan::GetInstance().GetCommentText("ANI_MAINTYPE",m_Item.m_sEXT_DATA.emANIMTYPE).GetString());
		SetWin_Text( this, IDC_EDIT_SUBTYPE,	CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_00",m_Item.m_sEXT_DATA.emANISTYPE).GetString());
		SetWin_Text( this, IDC_EDIT_VEHICLE_MAINTYPE, CGameTextMan::GetInstance().GetCommentText("ANI_VEHICLE_MAINTYPE",m_Item.m_sEXT_DATA.emANIVehicleMTYPE).GetString());
		SetWin_Text ( this, IDC_EDIT_EFF_ELMT, CGameTextMan::GetInstance().GetCommentText("ELEMENT",0).GetString());
		SetWin_Text ( this, IDC_EDIT_HOLDOUT, m_Item.m_sEXT_DATA.strHOLDOUT.c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGET_EFF01_POS1, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFPOS",m_Item.m_sEXT_DATA.emTARG_POSA).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGET_EFF01_POS2, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFPOS",m_Item.m_sEXT_DATA.emTARG_POSB).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF01_POS, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFPOS",m_Item.m_sEXT_DATA.emSELFZONE01_POS).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF02_POS, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFPOS",m_Item.m_sEXT_DATA.emSELFZONE02_POS).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF03_POS, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFPOS",m_Item.m_sEXT_DATA.emSELFZONE03_POS).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF01_POS, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFPOS",m_Item.m_sEXT_DATA.emTARGZONE01_POS).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF02_POS, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFPOS",m_Item.m_sEXT_DATA.emTARGZONE02_POS).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF03_POS, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFPOS",m_Item.m_sEXT_DATA.emTARGZONE03_POS).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF01, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFTIME",m_Item.m_sEXT_DATA.emSELFZONE01).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF02, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFTIME",m_Item.m_sEXT_DATA.emSELFZONE02).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF03, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFTIME",m_Item.m_sEXT_DATA.emSELFZONE03).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF01, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFTIME",m_Item.m_sEXT_DATA.emTARGZONE01).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF02, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFTIME",m_Item.m_sEXT_DATA.emTARGZONE02).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF03, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFTIME",m_Item.m_sEXT_DATA.emTARGZONE03).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFBODY_EFF01, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFTIME",m_Item.m_sEXT_DATA.emSELFBODY).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_TARBODY_EFF01, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFTIME",m_Item.m_sEXT_DATA.emTARGBODY01).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_TARBODY_EFF02, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFTIME",m_Item.m_sEXT_DATA.emTARGBODY02).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGET_EFF01, CGameTextMan::GetInstance().GetCommentText("SKILL_EFFTIME",m_Item.m_sEXT_DATA.emTARG).GetString());
		SetWin_Combo_Sel ( this, IDC_COMBO_REQ_BRIGHT, CGameTextMan::GetInstance().GetCommentText("BRIGHT",m_Item.m_sLEARN.emBRIGHT).GetString());
	}
	else
	{
		SetWin_Combo_Sel ( this, IDC_COMBO_ROLE, COMMENT::SKILL_ROLE[m_Item.m_sBASIC.emROLE].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_APPLY, COMMENT::SKILL_APPLY[m_Item.m_sBASIC.emAPPLY].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_ACTION_TYPE, COMMENT::SKILL_ACTION_TYPE[m_Item.m_sBASIC.emACTION_TYPE].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_IMPACT_TAR, COMMENT::IMPACT_TAR[m_Item.m_sBASIC.emIMPACT_TAR].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_IMPACT_REALM, COMMENT::IMPACT_REALM[m_Item.m_sBASIC.emIMPACT_REALM].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_IMPACT_SIDE, COMMENT::IMPACT_SIDE[m_Item.m_sBASIC.emIMPACT_SIDE].c_str() );
		SetWin_Text( this, IDC_EDIT_ATTACKLEFT, COMMENT::SKILLATTACK[m_Item.m_sBASIC.emUSE_LITEM].c_str() );	
		SetWin_Text( this, IDC_EDIT_ATTACKRIGHT, COMMENT::SKILLATTACK[m_Item.m_sBASIC.emUSE_RITEM].c_str() );	
		SetWin_Text( this, IDC_EDIT_SKILL_TYPE, COMMENT::SKILL_TYPES[m_Item.m_sAPPLY.emBASIC_TYPE].c_str() );	
		SetWin_Text( this, IDC_EDIT_ELEMENT, COMMENT::ELEMENT[m_Item.m_sAPPLY.emELEMENT].c_str() );	
		SetWin_Text( this, IDC_EDIT_BLOW_TYPE, COMMENT::BLOW[m_Item.m_sAPPLY.emSTATE_BLOW].c_str() );	
		m_listImpactAddon.ResetContent();
		m_listSpecAddon.ResetContent();
		for ( DWORD i=0 ; i < m_Item.m_sAPPLY.vecADDON.size(); ++i )
		{
			CString strNAME = COMMENT::IMPACT_ADDON[m_Item.m_sAPPLY.vecADDON[i].emADDON].c_str();

			int nIndex = m_listImpactAddon.AddString ( strNAME );
			m_listImpactAddon.SetItemData ( nIndex, i );
		}
		for ( DWORD i=0 ; i < m_Item.m_sAPPLY.vecSPEC.size(); ++i )
		{
			CString strNAME = COMMENT::SPEC_ADDON[m_Item.m_sAPPLY.vecSPEC[i].emSPEC].c_str();

			int nIndex = m_listSpecAddon.AddString ( strNAME );
			m_listSpecAddon.SetItemData ( nIndex, i );
		}
		SetWin_Text( this, IDC_EDIT_SPECIAL, COMMENT::SPECIAL_SKILL_TYPE[m_Item.m_sSPECIAL_SKILL.emSSTYPE].c_str() );
		SetWin_Text( this, IDC_EDIT_MAINTYPE, COMMENT::ANI_MAINTYPE[m_Item.m_sEXT_DATA.emANIMTYPE].c_str() );
		SetWin_Text( this, IDC_EDIT_SUBTYPE, COMMENT::ANI_SUBTYPE_00[m_Item.m_sEXT_DATA.emANISTYPE].c_str() );
		SetWin_Text( this, IDC_EDIT_VEHICLE_MAINTYPE, COMMENT::ANI_VEHICLE_MAINTYPE[m_Item.m_sEXT_DATA.emANIVehicleMTYPE].c_str() );
		SetWin_Text ( this, IDC_EDIT_EFF_ELMT, COMMENT::ELEMENT[EMELEMENT_SPIRIT].c_str() );
		SetWin_Text ( this, IDC_EDIT_HOLDOUT, m_Item.m_sEXT_DATA.strHOLDOUT.c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGET_EFF01, COMMENT::SKILL_EFFTIME[m_Item.m_sEXT_DATA.emTARG].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGET_EFF01_POS1, COMMENT::SKILL_EFFPOS[m_Item.m_sEXT_DATA.emTARG_POSA].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGET_EFF01_POS2, COMMENT::SKILL_EFFPOS[m_Item.m_sEXT_DATA.emTARG_POSB].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF01, COMMENT::SKILL_EFFTIME[m_Item.m_sEXT_DATA.emSELFZONE01].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF02, COMMENT::SKILL_EFFTIME[m_Item.m_sEXT_DATA.emSELFZONE02].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF03, COMMENT::SKILL_EFFTIME[m_Item.m_sEXT_DATA.emSELFZONE03].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF01_POS, COMMENT::SKILL_EFFPOS[m_Item.m_sEXT_DATA.emSELFZONE01_POS].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF02_POS, COMMENT::SKILL_EFFPOS[m_Item.m_sEXT_DATA.emSELFZONE02_POS].c_str());
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF03_POS, COMMENT::SKILL_EFFPOS[m_Item.m_sEXT_DATA.emSELFZONE03_POS].c_str());
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF01, COMMENT::SKILL_EFFTIME[m_Item.m_sEXT_DATA.emTARGZONE01].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF02, COMMENT::SKILL_EFFTIME[m_Item.m_sEXT_DATA.emTARGZONE02].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF03, COMMENT::SKILL_EFFTIME[m_Item.m_sEXT_DATA.emTARGZONE03].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF01_POS, COMMENT::SKILL_EFFPOS[m_Item.m_sEXT_DATA.emTARGZONE01_POS].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF02_POS, COMMENT::SKILL_EFFPOS[m_Item.m_sEXT_DATA.emTARGZONE02_POS].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF03_POS, COMMENT::SKILL_EFFPOS[m_Item.m_sEXT_DATA.emTARGZONE03_POS].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_SELFBODY_EFF01, COMMENT::SKILL_EFFTIME[m_Item.m_sEXT_DATA.emSELFBODY].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_TARBODY_EFF01, COMMENT::SKILL_EFFTIME[m_Item.m_sEXT_DATA.emTARGBODY01].c_str() );
		SetWin_Combo_Sel ( this, IDC_COMBO_TARBODY_EFF02, COMMENT::SKILL_EFFTIME[m_Item.m_sEXT_DATA.emTARGBODY02].c_str());
		SetWin_Combo_Sel ( this, IDC_COMBO_REQ_BRIGHT, COMMENT::BRIGHT[m_Item.m_sLEARN.emBRIGHT].c_str() );
	}
	SetWin_Num_int ( this, IDC_EDIT_MID, m_Item.m_sBASIC.sNATIVEID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_SID, m_Item.m_sBASIC.sNATIVEID.wSubID );

	SetWin_Text ( this, IDC_EDIT_SKILLNAME, m_Item.m_sBASIC.szNAME );
	SetWin_Num_int ( this, IDC_EDIT_GRADE, m_Item.m_sBASIC.dwGRADE );
	SetWin_Num_int ( this, IDC_EDIT_MAXLEVEL, m_Item.m_sBASIC.dwMAXLEVEL );
	SetWin_Num_int ( this, IDC_EDIT_TARGETRANGE, m_Item.m_sBASIC.wTARRANGE );
	SetWin_Check ( this, IDC_CHECK_GMSKILL, m_Item.m_sBASIC.bLearnView );
	SetWin_Check ( this, IDC_CHECK_EFFECT_REMOVE, m_Item.m_sBASIC.bNonEffectRemove );
	SetWin_Check ( this, IDC_CHECK_EFFECT_MOB_RATE, m_Item.m_sBASIC.bMobEffectRate ); // by luxes.
	SetWin_Check ( this, IDC_CHECK_RUNNINGCAST, m_Item.m_sBASIC.dwFlags & FLAG_SB_RUNNING_CAST );
    SetWin_Check ( this, IDC_CHECK_EFFECT_FORCE_VISIBLE, m_Item.m_sBASIC.dwFlags & FLAG_SB_EFFECT_FORCE_VISIBLE );

	SetWin_Text( this, IDC_EDIT_SKILL_TYPE, COMMENT::SKILL_TYPES[m_Item.m_sAPPLY.emBASIC_TYPE].c_str() );	

	SetWin_Text( this, IDC_EDIT_ELEMENT, COMMENT::ELEMENT[m_Item.m_sAPPLY.emELEMENT].c_str() );	
	SetWin_Text( this, IDC_EDIT_BLOW_TYPE, COMMENT::BLOW[m_Item.m_sAPPLY.emSTATE_BLOW].c_str() );	

	m_listImpactAddon.ResetContent();

	for ( DWORD i=0 ; i < m_Item.m_sAPPLY.vecADDON.size(); ++i )
	{
		CString strNAME = COMMENT::IMPACT_ADDON[m_Item.m_sAPPLY.vecADDON[i].emADDON].c_str();
		
		int nIndex = m_listImpactAddon.AddString ( strNAME );
		m_listImpactAddon.SetItemData ( nIndex, i );
	}

	m_listSpecAddon.ResetContent();

	for ( DWORD i=0 ; i < m_Item.m_sAPPLY.vecSPEC.size(); ++i )
	{
		CString strNAME = COMMENT::SPEC_ADDON[m_Item.m_sAPPLY.vecSPEC[i].emSPEC].c_str();
		
		int nIndex = m_listSpecAddon.AddString ( strNAME );
		m_listSpecAddon.SetItemData ( nIndex, i );
	}

	SetWin_Text( this, IDC_EDIT_SPECIAL, COMMENT::SPECIAL_SKILL_TYPE[m_Item.m_sSPECIAL_SKILL.emSSTYPE].c_str() );
	SetWin_Text( this, IDC_EDIT_MAINTYPE, COMMENT::ANI_MAINTYPE[m_Item.m_sEXT_DATA.emANIMTYPE].c_str() );
	SetWin_Text( this, IDC_EDIT_SUBTYPE, COMMENT::ANI_SUBTYPE_00[m_Item.m_sEXT_DATA.emANISTYPE].c_str() );
	SetWin_Text( this, IDC_EDIT_VEHICLE_MAINTYPE, COMMENT::ANI_VEHICLE_MAINTYPE[m_Item.m_sEXT_DATA.emANIVehicleMTYPE].c_str() );

	SetWin_Num_float ( this, IDC_EDIT_DAMAGEDELAY, m_Item.m_sEXT_DATA.fDELAY4DAMAGE );

	//	Note : element 속성이 무기에 종속적일 경우 액티브됨.
	m_nSelEffElmt = 0;
	SetWin_Enable ( this, IDC_BUTTON_EFF_ELMT_LEFT, m_Item.m_sAPPLY.emELEMENT==EMELEMENT_ARM );
	SetWin_Enable ( this, IDC_BUTTON_EFF_ELMT_RIGHT, m_Item.m_sAPPLY.emELEMENT==EMELEMENT_ARM );

	UpdateEffElmtLeft();
	SetWin_Check ( this, IDC_CHECK_TARGETEFFECT_ONE, m_Item.m_sEXT_DATA.bTARG_ONE );

	//SetWin_Num_int ( this, IDC_EDIT_ICON_MID, m_Item.m_sEXT_DATA.sICONINDEX.wMainID );
	//SetWin_Num_int ( this, IDC_EDIT_ICON_SID, m_Item.m_sEXT_DATA.sICONINDEX.wSubID );
	SetWin_Text ( this, IDC_EDIT_ICONFILE, m_Item.m_sEXT_DATA.strICONFILE.c_str() );

	SetWin_Text ( this, IDC_EDIT_SKILLDESC, m_Item.m_sEXT_DATA.strCOMMENTS.c_str() ); // by 경대

	SetWin_Num_int ( this, IDC_EDIT_REQ_SKILL_MID, m_Item.m_sLEARN.sSKILL.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_REQ_SKILL_SID, m_Item.m_sLEARN.sSKILL.wSubID );

	SetWin_Check( this, IDC_CHECK_HIDDENWEAPON, m_Item.m_sLEARN.bHiddenWeapon );

	if( m_Item.m_sLEARN.bHiddenWeapon )
	{
		SetWin_Enable ( this, IDC_EDIT_HIDDENWEAPON_MID, TRUE );
		SetWin_Enable ( this, IDC_EDIT_HIDDENWEAPON_SID, TRUE );
	}
	else
	{
		SetWin_Enable ( this, IDC_EDIT_HIDDENWEAPON_MID, FALSE );
		SetWin_Enable ( this, IDC_EDIT_HIDDENWEAPON_SID, FALSE );
	}

	//	글러브 착용 후 스킬 사용 시 아이템 변경ID
	SetWin_Num_int ( this, IDC_EDIT_HIDDENWEAPON_MID, m_Item.m_sLEARN.sHiddenWeapon.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_HIDDENWEAPON_SID, m_Item.m_sLEARN.sHiddenWeapon.wSubID );

	BOOL bTYPE_CURE = ( m_Item.m_sAPPLY.emBASIC_TYPE==SKILL::EMFOR_CURE );
	SetWin_Enable ( this, IDC_CHECK_PALSY, bTYPE_CURE );
	SetWin_Enable ( this, IDC_CHECK_SWOON, bTYPE_CURE );
	SetWin_Enable ( this, IDC_CHECK_PETRIFY, bTYPE_CURE );
	SetWin_Enable ( this, IDC_CHECK_BURN, bTYPE_CURE );

	SetWin_Enable ( this, IDC_CHECK_FROZEN, bTYPE_CURE );
	SetWin_Enable ( this, IDC_CHECK_DERANGE, bTYPE_CURE );
	SetWin_Enable ( this, IDC_CHECK_POISON, bTYPE_CURE );
	SetWin_Enable ( this, IDC_CHECK_CURSE, bTYPE_CURE );

	SetWin_Check ( this, IDC_CHECK_PALSY, m_Item.m_sAPPLY.dwCUREFLAG&DIS_NUMB );
	SetWin_Check ( this, IDC_CHECK_SWOON, m_Item.m_sAPPLY.dwCUREFLAG&DIS_STUN );
	SetWin_Check ( this, IDC_CHECK_PETRIFY, m_Item.m_sAPPLY.dwCUREFLAG&DIS_STONE );
	SetWin_Check ( this, IDC_CHECK_BURN, m_Item.m_sAPPLY.dwCUREFLAG&DIS_BURN );

	SetWin_Check ( this, IDC_CHECK_FROZEN, m_Item.m_sAPPLY.dwCUREFLAG&DIS_FROZEN );
	SetWin_Check ( this, IDC_CHECK_DERANGE, m_Item.m_sAPPLY.dwCUREFLAG&DIS_MAD );
	SetWin_Check ( this, IDC_CHECK_POISON, m_Item.m_sAPPLY.dwCUREFLAG&DIS_POISON );
	SetWin_Check ( this, IDC_CHECK_CURSE, m_Item.m_sAPPLY.dwCUREFLAG&DIS_CURSE );


	if( m_Item.m_sBASIC.dwFlags & FLAG_SB_RUNNING_CAST )
	{
		SetWin_Combo_Sel ( this, IDC_COMBO_EFFTARGET, 0 );
		SetWin_Num_float ( this, IDC_EDIT_RUNNINGCAST, m_Item.m_sAPPLY.fRunningEffTime );
	}
	else
	{
		SetWin_Combo_Sel ( this, IDC_COMBO_EFFTARGET, m_Item.m_sAPPLY.nEffectFrame );
		SetWin_Num_float ( this, IDC_EDIT_RUNNINGCAST, 0.0f );
	} //if..else

	SetWin_Combo_Sel ( this, IDC_COMBO_MULEFFTARGET, COMMENT::SKILL_EFFTARGETPOS[m_Item.m_sEXT_DATA.nMultiTatgetEffectFrame].c_str() );
	SetWin_Num_int ( this, IDC_EDIT_MULTITARGETSKILL_MID, m_Item.m_sEXT_DATA.idMultiTargetSkill.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_MULTITARGETSKILL_SID, m_Item.m_sEXT_DATA.idMultiTargetSkill.wSubID );

	SetWin_Num_int ( this, IDC_EDIT_DISGUISESKILL_MID, m_Item.m_sEXT_DATA.idDisguiseSkill.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_DISGUISESKILL_SID, m_Item.m_sEXT_DATA.idDisguiseSkill.wSubID );

	UpdateAnimInfo();
	SetReadMode();
}

void CSkillData::UpdateAnimInfo(void)
{
	SetWin_Text( this, IDC_EDIT_ANIMINFO_CFGNAME_M, "" );
	SetWin_Text( this, IDC_EDIT_ANIMFRAME_START_M, "" );
	SetWin_Text( this, IDC_EDIT_ANIMFRAME_MAX_M, "" );
	SetWin_Text( this, IDC_EDIT_ANIMFRAME_END_M, "" );
	SetWin_Text( this, IDC_EDIT_ANIMINFO_CFGNAME_W, "" );
	SetWin_Text( this, IDC_EDIT_ANIMFRAME_START_W, "" );
	SetWin_Text( this, IDC_EDIT_ANIMFRAME_MAX_W, "" );
	SetWin_Text( this, IDC_EDIT_ANIMFRAME_END_W, "" );
	SetWin_Enable ( this, IDC_EDIT_ANIMFRAME_START_M, false );
	SetWin_Enable ( this, IDC_EDIT_ANIMFRAME_MAX_M, false );
	SetWin_Enable ( this, IDC_EDIT_ANIMFRAME_END_M, false );
	SetWin_Enable ( this, IDC_EDIT_ANIMFRAME_START_W, false );
	SetWin_Enable ( this, IDC_EDIT_ANIMFRAME_MAX_W, false );
	SetWin_Enable ( this, IDC_EDIT_ANIMFRAME_END_W, false );
	m_listAttStrikeAni.ResetContent();
	// m_listAttStrikeAni.EnableWindow(FALSE);
	EMCLASSINDEX emClassIndex = GLCONST_CHAR::GetClassInfoUsingSkill(m_Item.m_sBASIC.GetId());
	if ( emClassIndex == GLCL_NUM_CLASS || emClassIndex == GLCL_ETC )
		return;

	// Man
	{
		EMCHARINDEX emCharIndex = CharClassToIndex(CharSexandClassIndexToCharClass(1, emClassIndex));

		SANIMCONINFO* pAnimInfo = GLCONST_CHAR::cCONSTCLASS[emCharIndex].m_pSkinDataDummy->FindAniInfo(m_Item.m_sEXT_DATA.emANIMTYPE, m_Item.m_sEXT_DATA.emANISTYPE);
		if ( pAnimInfo != NULL )
		{
			SetWin_Text( this, IDC_EDIT_ANIMINFO_CFGNAME_M, pAnimInfo->m_strCfgFile.c_str() );
			SetWin_Num_int( this, IDC_EDIT_ANIMFRAME_START_M, pAnimInfo->m_dwSTime/pAnimInfo->m_UNITTIME );
			SetWin_Num_int( this, IDC_EDIT_ANIMFRAME_MAX_M, pAnimInfo->m_dwETime/pAnimInfo->m_UNITTIME );
			SetWin_Num_int( this, IDC_EDIT_ANIMFRAME_END_M, pAnimInfo->m_dwETimeOrig/pAnimInfo->m_UNITTIME );

			m_listAttStrikeAni.AddString("====Man AttackPoint=======");
			for ( INT i = 0; i < pAnimInfo->m_wStrikeCount; ++i )
			{
				CString Str;
				Str.Format ("[%d/%d] 부위[%s] ", i, pAnimInfo->m_sStrikeEff[i].m_dwFrame/pAnimInfo->m_UNITTIME, 
					DxSkinPiece::m_szPIECETYPE[pAnimInfo->m_sStrikeEff[i].m_emPiece]);

				m_listAttStrikeAni.AddString(Str.GetString());
			}
		}
	}

	// Woman
	{
		EMCHARINDEX emCharIndex = CharClassToIndex(CharSexandClassIndexToCharClass(2, emClassIndex));

		SANIMCONINFO* pAnimInfo = GLCONST_CHAR::cCONSTCLASS[emCharIndex].m_pSkinDataDummy->FindAniInfo(m_Item.m_sEXT_DATA.emANIMTYPE, m_Item.m_sEXT_DATA.emANISTYPE);
		if ( pAnimInfo != NULL )
		{
			SetWin_Text( this, IDC_EDIT_ANIMINFO_CFGNAME_W, pAnimInfo->m_strCfgFile.c_str() );
			SetWin_Num_int( this, IDC_EDIT_ANIMFRAME_START_W, pAnimInfo->m_dwSTime/pAnimInfo->m_UNITTIME );
			SetWin_Num_int( this, IDC_EDIT_ANIMFRAME_MAX_W, pAnimInfo->m_dwETime/pAnimInfo->m_UNITTIME );
			SetWin_Num_int( this, IDC_EDIT_ANIMFRAME_END_W, pAnimInfo->m_dwETimeOrig/pAnimInfo->m_UNITTIME );

			INT nCount(m_listAttStrikeAni.GetCount());
			m_listAttStrikeAni.AddString("====Woman AttackPoint====");
			for ( INT i = 0; i < pAnimInfo->m_wStrikeCount; ++i )
			{
				CString Str;
				Str.Format ("[%d/%d] 부위[%s] ",  i, pAnimInfo->m_sStrikeEff[i].m_dwFrame/pAnimInfo->m_UNITTIME, 
					DxSkinPiece::m_szPIECETYPE[pAnimInfo->m_sStrikeEff[i].m_emPiece]);

				m_listAttStrikeAni.AddString(Str.GetString());
			}
		}
	}
}

BOOL CSkillData::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	m_bDlgInit = TRUE;

	InitDefaultCtrls ();
	UpdateItems ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSkillData::InverseUpdateItems ()
{
	m_Item.m_sBASIC.sNATIVEID.wMainID = GetWin_Num_int ( this, IDC_EDIT_MID );
	m_Item.m_sBASIC.sNATIVEID.wSubID = GetWin_Num_int ( this, IDC_EDIT_SID );

	strKeyName = GetWin_Text ( this, IDC_EDIT_SKILLNAME ).GetString();
	strName	= GetWin_Text ( this, IDC_EDIT_NAME ).GetString();

	m_Item.m_sBASIC.dwGRADE = GetWin_Num_int ( this, IDC_EDIT_GRADE );
	m_Item.m_sBASIC.dwMAXLEVEL = GetWin_Num_int ( this, IDC_EDIT_MAXLEVEL );
	if ( m_Item.m_sBASIC.dwMAXLEVEL > SKILL::MAX_LEVEL )	m_Item.m_sBASIC.dwMAXLEVEL = SKILL::MAX_LEVEL;

	m_Item.m_sBASIC.emROLE = static_cast<SKILL::EMROLE> ( GetWin_Combo_Sel ( this, IDC_COMBO_ROLE ) );
	m_Item.m_sBASIC.emAPPLY = static_cast<SKILL::EMAPPLY> ( GetWin_Combo_Sel ( this, IDC_COMBO_APPLY ) );
	m_Item.m_sBASIC.emACTION_TYPE = static_cast<SKILL::EMACTION_TYPE> ( GetWin_Combo_Sel ( this, IDC_COMBO_ACTION_TYPE ) );	

	m_Item.m_sBASIC.emIMPACT_TAR = static_cast<EMIMPACT_TARGET> ( GetWin_Combo_Sel ( this, IDC_COMBO_IMPACT_TAR ) );
	m_Item.m_sBASIC.emIMPACT_REALM = static_cast<EMIMPACT_REALM> ( GetWin_Combo_Sel ( this, IDC_COMBO_IMPACT_REALM ) );
	m_Item.m_sBASIC.emIMPACT_SIDE = static_cast<EMIMPACT_SIDE> ( GetWin_Combo_Sel ( this, IDC_COMBO_IMPACT_SIDE ) );

	m_Item.m_sBASIC.bLearnView = (0!=GetWin_Check( this, IDC_CHECK_GMSKILL ));
	m_Item.m_sBASIC.bNonEffectRemove = (0!=GetWin_Check( this, IDC_CHECK_EFFECT_REMOVE ));
	m_Item.m_sBASIC.bMobEffectRate = (0!=GetWin_Check( this, IDC_CHECK_EFFECT_MOB_RATE )); // by luxes.
	m_Item.m_sBASIC.dwFlags = 0;
	m_Item.m_sBASIC.dwFlags |= (GetWin_Check( this, IDC_CHECK_RUNNINGCAST ) ? FLAG_SB_RUNNING_CAST : 0); // by paran
    m_Item.m_sBASIC.dwFlags |= (GetWin_Check( this, IDC_CHECK_EFFECT_FORCE_VISIBLE ) ? FLAG_SB_EFFECT_FORCE_VISIBLE : 0);

	m_Item.m_sBASIC.wTARRANGE = GetWin_Num_int ( this, IDC_EDIT_TARGETRANGE );

	InverseUpdateEffElmtLeft();

	m_Item.m_sEXT_DATA.strHOLDOUT	= GetWin_Text ( this, IDC_EDIT_HOLDOUT );

	m_Item.m_sEXT_DATA.fDELAY4DAMAGE = GetWin_Num_float ( this, IDC_EDIT_DAMAGEDELAY );

	m_Item.m_sEXT_DATA.emTARG = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_TARGET_EFF01 );
	m_Item.m_sEXT_DATA.emTARG_POSA = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_TARGET_EFF01_POS1 );
	m_Item.m_sEXT_DATA.emTARG_POSB = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_TARGET_EFF01_POS2 );
	m_Item.m_sEXT_DATA.bTARG_ONE = FALSE != GetWin_Check ( this, IDC_CHECK_TARGETEFFECT_ONE );

	m_Item.m_sEXT_DATA.emSELFZONE01 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF01 );
	m_Item.m_sEXT_DATA.emSELFZONE02 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF02 );
	m_Item.m_sEXT_DATA.emSELFZONE03 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF03 );

	m_Item.m_sEXT_DATA.emSELFZONE01_POS = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF01_POS );
	m_Item.m_sEXT_DATA.emSELFZONE02_POS = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF02_POS );
	m_Item.m_sEXT_DATA.emSELFZONE03_POS = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_SELFZONE_EFF03_POS );

	m_Item.m_sEXT_DATA.emTARGZONE01 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF01 );
	m_Item.m_sEXT_DATA.emTARGZONE02 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF02 );
	m_Item.m_sEXT_DATA.emTARGZONE03 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF03 );

	m_Item.m_sEXT_DATA.emTARGZONE01_POS = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF01_POS );
	m_Item.m_sEXT_DATA.emTARGZONE02_POS = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF02_POS );
	m_Item.m_sEXT_DATA.emTARGZONE03_POS = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_TARGZONE_EFF03_POS );

	m_Item.m_sEXT_DATA.emSELFBODY = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_SELFBODY_EFF01 );

	m_Item.m_sEXT_DATA.emTARGBODY01 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_TARBODY_EFF01 );
	m_Item.m_sEXT_DATA.emTARGBODY02 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_TARBODY_EFF02 );

	//m_Item.m_sEXT_DATA.sICONINDEX.wMainID = static_cast<WORD> ( GetWin_Num_int ( this, IDC_EDIT_ICON_MID ) );
	//m_Item.m_sEXT_DATA.sICONINDEX.wSubID = static_cast<WORD> ( GetWin_Num_int ( this, IDC_EDIT_ICON_SID ) );
	m_Item.m_sEXT_DATA.strICONFILE = GetWin_Text ( this, IDC_EDIT_ICONFILE );

	strKeyDesc = GetWin_Text ( this, IDC_EDIT_SKILLDESC ).GetString(); // by 경대
	strDesc = GetWin_Text ( this, IDC_EDIT_COMMENT );
	
	m_Item.m_sLEARN.emBRIGHT = static_cast<EMBRIGHT> ( GetWin_Combo_Sel ( this, IDC_COMBO_REQ_BRIGHT ) );

	m_Item.m_sLEARN.sSKILL.wMainID = GetWin_Num_int ( this, IDC_EDIT_REQ_SKILL_MID );
	m_Item.m_sLEARN.sSKILL.wSubID = GetWin_Num_int ( this, IDC_EDIT_REQ_SKILL_SID );

	m_Item.m_sLEARN.bHiddenWeapon = GetWin_Check( this, IDC_CHECK_HIDDENWEAPON );

	//	글러브 착용 후 스킬 사용 시 아이템 변경ID
	m_Item.m_sLEARN.sHiddenWeapon.wMainID = GetWin_Num_int ( this, IDC_EDIT_HIDDENWEAPON_MID );
	m_Item.m_sLEARN.sHiddenWeapon.wSubID = GetWin_Num_int ( this, IDC_EDIT_HIDDENWEAPON_SID );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_PALSY ), m_Item.m_sAPPLY.dwCUREFLAG, DIS_NUMB );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SWOON ), m_Item.m_sAPPLY.dwCUREFLAG, DIS_STUN );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_PETRIFY ), m_Item.m_sAPPLY.dwCUREFLAG, DIS_STONE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_BURN ), m_Item.m_sAPPLY.dwCUREFLAG, DIS_BURN );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_FROZEN ), m_Item.m_sAPPLY.dwCUREFLAG, DIS_FROZEN );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_DERANGE ), m_Item.m_sAPPLY.dwCUREFLAG, DIS_MAD );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_POISON ), m_Item.m_sAPPLY.dwCUREFLAG, DIS_POISON );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CURSE ), m_Item.m_sAPPLY.dwCUREFLAG, DIS_CURSE );

	m_Item.m_sBASIC.emIMPACT_TAR = static_cast<EMIMPACT_TARGET> ( GetWin_Combo_Sel ( this, IDC_COMBO_IMPACT_TAR ) );

	if( m_Item.m_sBASIC.dwFlags & FLAG_SB_RUNNING_CAST )
		m_Item.m_sAPPLY.fRunningEffTime					= GetWin_Num_float( this, IDC_EDIT_RUNNINGCAST );
	else
		m_Item.m_sAPPLY.nEffectFrame					= (int)GetWin_Combo_Sel( this, IDC_COMBO_EFFTARGET );

	m_Item.m_sEXT_DATA.nMultiTatgetEffectFrame		= (int)GetWin_Combo_Sel( this, IDC_COMBO_MULEFFTARGET );
	m_Item.m_sEXT_DATA.idMultiTargetSkill.wMainID	= (int)GetWin_Num_int( this, IDC_EDIT_MULTITARGETSKILL_MID );
	m_Item.m_sEXT_DATA.idMultiTargetSkill.wSubID	= (int)GetWin_Num_int( this, IDC_EDIT_MULTITARGETSKILL_SID );

	m_Item.m_sEXT_DATA.idDisguiseSkill.wMainID	= (int)GetWin_Num_int( this, IDC_EDIT_DISGUISESKILL_MID );
	m_Item.m_sEXT_DATA.idDisguiseSkill.wSubID	= (int)GetWin_Num_int( this, IDC_EDIT_DISGUISESKILL_SID );

	if( m_Item.m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_NONE )
		m_Item.m_sSPECIAL_SKILL.ResetAll();
}

void CSkillData::OnBnClickedButtonNext()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//InverseUpdateItems();

	//GLSkillMan::GetInstance().SetData ( m_Item.m_sBASIC.sNATIVEID.wMainID, m_Item.m_sBASIC.sNATIVEID.wSubID, new GLSKILL(m_Item) );

	//GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::SKILL );
	//GLStringTable::GetInstance().InsertString ( strKey Desc, strDesc, GLStringTable::SKILL );

	DWORD CombineID =
	(
		(DWORD)(m_Item.m_sBASIC.sNATIVEID.wMainID & eMASK_0x0000FFFF) << eSHIFT_16BIT |
		(DWORD)(m_Item.m_sBASIC.sNATIVEID.wSubID  & eMASK_0x0000FFFF) /*<< eSHIFT_16BIT |*/
	);

	m_pSheetTab->m_SkillLevelDataPage.SetCallPage ( m_CallPage );
	m_pSheetTab->m_SkillLevelDataPage.SetItem ( CombineID );
	m_pSheetTab->SetActivePage ( SKILLLEVELDATAPAGE );
}

void CSkillData::InitDefaultCtrls ()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string 
			strSKILL_ROLE[SKILL::EMROLE_NSIZE],
			strSKILL_APPLY[SKILL::EMAPPLY_NSIZE],
			strSKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_NSIZE],
			strIMPACT_TAR[EMIMPACT_TARGET_NSIZE],
			strIMPACT_REALM[EMIMPACT_REALM_NSIZE],
			strIMPACT_SIDE[EMIMPACT_SIDE_NSIZE],
			strBRIGHT[BRIGHT_SIZE],
			strSKILL_EFFTIME[SKILL::EMTIME_NSIZE],
			strSKILL_EFFPOS[SKILL::EMPOS_NSIZE],
			strSKILL_EFFTARGETPOS[SKILL::MAX_EFFTARGETPOS];
		CGameTextMan::GetInstance().LoadHelpCommentString( "SKILL_ROLE",		strSKILL_ROLE,				CGameTextMan::EM_COMMENT_TEXT, SKILL::EMROLE_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString( "SKILL_APPLY",		strSKILL_APPLY,			CGameTextMan::EM_COMMENT_TEXT, SKILL::EMAPPLY_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString( "SKILL_ACTION_TYPE",	strSKILL_ACTION_TYPE,		CGameTextMan::EM_COMMENT_TEXT, SKILL::EMACTION_TYPE_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString( "IMPACT_TAR",		strIMPACT_TAR,				CGameTextMan::EM_COMMENT_TEXT, EMIMPACT_TARGET_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString( "IMPACT_REALM",		strIMPACT_REALM,			CGameTextMan::EM_COMMENT_TEXT, EMIMPACT_REALM_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString( "IMPACT_SIDE",		strIMPACT_SIDE,			CGameTextMan::EM_COMMENT_TEXT, EMIMPACT_SIDE_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString( "BRIGHT",			strBRIGHT,					CGameTextMan::EM_COMMENT_TEXT, BRIGHT_SIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString( "SKILL_EFFTIME",		strSKILL_EFFTIME,			CGameTextMan::EM_COMMENT_TEXT, SKILL::EMTIME_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString( "SKILL_EFFPOS",		strSKILL_EFFPOS,			CGameTextMan::EM_COMMENT_TEXT, SKILL::EMPOS_NSIZE);
		SetWin_Combo_Init ( this, IDC_COMBO_ROLE, strSKILL_ROLE, SKILL::EMROLE_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_APPLY, strSKILL_APPLY, SKILL::EMAPPLY_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_ACTION_TYPE, strSKILL_ACTION_TYPE, SKILL::EMACTION_TYPE_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_IMPACT_TAR, strIMPACT_TAR, EMIMPACT_TARGET_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_IMPACT_REALM, strIMPACT_REALM, EMIMPACT_REALM_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_IMPACT_SIDE, strIMPACT_SIDE, EMIMPACT_SIDE_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_REQ_BRIGHT, strBRIGHT, BRIGHT_SIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGET_EFF01, strSKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF01, strSKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF02, strSKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF03, strSKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF01, strSKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF02, strSKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF03, strSKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFBODY_EFF01, strSKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARBODY_EFF01, strSKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARBODY_EFF02, strSKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGET_EFF01_POS1, strSKILL_EFFPOS, SKILL::EMPOS_NSIZE ); 
		SetWin_Combo_Init ( this, IDC_COMBO_TARGET_EFF01_POS2, strSKILL_EFFPOS, SKILL::EMPOS_NSIZE ); 
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF01_POS, strSKILL_EFFPOS, SKILL::EMPOS_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF02_POS, strSKILL_EFFPOS, SKILL::EMPOS_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF03_POS, strSKILL_EFFPOS, SKILL::EMPOS_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF01_POS, strSKILL_EFFPOS, SKILL::EMPOS_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF02_POS, strSKILL_EFFPOS, SKILL::EMPOS_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF03_POS, strSKILL_EFFPOS, SKILL::EMPOS_NSIZE );
	}
	else
	{
		SetWin_Combo_Init ( this, IDC_COMBO_ROLE, COMMENT::SKILL_ROLE, SKILL::EMROLE_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_APPLY, COMMENT::SKILL_APPLY, SKILL::EMAPPLY_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_ACTION_TYPE, COMMENT::SKILL_ACTION_TYPE, SKILL::EMACTION_TYPE_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_IMPACT_TAR, COMMENT::IMPACT_TAR, EMIMPACT_TARGET_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_IMPACT_REALM, COMMENT::IMPACT_REALM, EMIMPACT_REALM_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_IMPACT_SIDE, COMMENT::IMPACT_SIDE, EMIMPACT_SIDE_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_REQ_BRIGHT, COMMENT::BRIGHT, BRIGHT_SIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGET_EFF01, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGET_EFF01_POS1, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE ); 
		SetWin_Combo_Init ( this, IDC_COMBO_TARGET_EFF01_POS2, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE ); 
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF01, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF02, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF03, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF01_POS, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF02_POS, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFZONE_EFF03_POS, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF01, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF02, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF03, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF01_POS, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF02_POS, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARGZONE_EFF03_POS, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SELFBODY_EFF01, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARBODY_EFF01, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_TARBODY_EFF02, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
	}
	SetWin_Combo_Init ( this, IDC_COMBO_EFFTARGET, COMMENT::SKILL_EFFTARGETPOS, SKILL::MAX_EFFTARGETPOS );
	SetWin_Combo_Init ( this, IDC_COMBO_MULEFFTARGET, COMMENT::SKILL_EFFTARGETPOS, SKILL::MAX_EFFTARGETPOS );
	GetDlgItem ( IDC_BUTTON_PREV )->EnableWindow ( FALSE );
}

void CSkillData::OnBnClickedButtonSelfbodyeffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect File (*.effskin_a)|*.effskin_a|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillData*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_SELFBODYEFFECT )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CSkillData::OnBnClickedButtonTargetbodyeffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect File (*.effskin_a)|*.effskin_a|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillData*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_TARGETBODYEFFECT )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CSkillData::OnBnClickedButtonTargeteffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillData*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_TARGETEFFECT )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CSkillData::OnBnClickedButtonIconfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Icon File (*.dds,*.png)|*.dds;*.png|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".png",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillData*)this);

	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_ICONFILE )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CSkillData::OnBnClickedButtonSelfzoneeffect()
{
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillData*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_SELFZONEEFFECT )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CSkillData::OnBnClickedButtonSelfzoneeffect2()
{
    CString szFilter = "Effect File (*.egp)|*.egp|";

    //	Note : 파일 오픈 다이알로그를 만듬.
    CFileDialog		dlg( TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, (CSkillData*)this );
    dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
    if ( dlg.DoModal() == IDOK )
    {
        GetDlgItem ( IDC_EDIT_SELFZONEEFFECT2 )->SetWindowText( dlg.GetFileName().GetString() );		
    }
} //CSkillData::OnBnClickedButtonSelfzoneeffect2

void CSkillData::OnBnClickedButtonSelfzoneeffect3()
{
    CString szFilter = "Effect File (*.egp)|*.egp|";

    //	Note : 파일 오픈 다이알로그를 만듬.
    CFileDialog		dlg( TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, (CSkillData*)this );
    dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
    if ( dlg.DoModal() == IDOK )
    {
        GetDlgItem ( IDC_EDIT_SELFZONEEFFECT4 )->SetWindowText( dlg.GetFileName().GetString() );		
    }
} //CSkillData::OnBnClickedButtonSelfzoneeffect3

void CSkillData::OnBnClickedButtonTargzoneeffect()
{
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillData*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_TARGZONEEFFECT )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CSkillData::OnBnClickedButtonTargzoneeffect2()
{
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillData*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_TARGZONEEFFECT2 )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CSkillData::OnBnClickedButtonTargzoneeffect3()
{
	CString szFilter = "Effect File (*.egp)|*.egp|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog		dlg( TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, (CSkillData*)this );
	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_TARGZONEEFFECT3 )->SetWindowText( dlg.GetFileName().GetString() );		
	}
} //CSkillData::OnBnClickedButtonTargzoneeffect3

void CSkillData::OnBnClickedButtonTargetbodyeffect2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect File (*.effskin_a)|*.effskin_a|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillData*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_TARGETBODYEFFECT2 )->SetWindowText( dlg.GetFileName().GetString() );		
	}	
}

void CSkillData::OnBnClickedButtonHoldout()
{
	CString szFilter = "Effect File (*.egp)|*.egp|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillData*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_HOLDOUT, dlg.GetFileName().GetString() );
	}
}

void CSkillData::OnEnChangeEditSkillname() // by 경대
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	std::string szTemp;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		szTemp = ID2SKILLTEXT( GetWin_Text( this, IDC_EDIT_SKILLNAME ).GetString() );
	}
	else
	{
		szTemp = GLStringTable::GetInstance().GetString(
			GetWin_Text( this, IDC_EDIT_SKILLNAME ).GetString(),
			GLStringTable::SKILL);
	}
	
	SetWin_Text ( this, IDC_EDIT_NAME, szTemp.empty() == false ? szTemp.c_str() : "");
}

void CSkillData::OnEnChangeEditSkilldesc()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	std::string szTemp;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		szTemp = ID2SKILLTEXT( GetWin_Text( this, IDC_EDIT_SKILLDESC ).GetString() );
	}
	else
	{
		szTemp = GLStringTable::GetInstance().GetString(
			GetWin_Text( this, IDC_EDIT_SKILLDESC ).GetString(),
			GLStringTable::SKILL);
	}

	SetWin_Text ( this, IDC_EDIT_COMMENT, szTemp.empty() == false ? szTemp.c_str() : "");
}

void CSkillData::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	const int nSkipNum = 8;
	const int nSkipID[nSkipNum] = { IDC_EDIT_NAME, IDC_EDIT_COMMENT, IDC_BUTTON_PREV, 
									IDC_BUTTON_NEXT, IDC_BUTTON_CANCEL, IDC_BUTTON_OK,
									IDC_BUTTON_EFF_ELMT_LEFT, IDC_BUTTON_EFF_ELMT_RIGHT };

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


void CSkillData::OnBnClickedButtonEffElmtLeft()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if ( m_nSelEffElmt <= 0  ) return;

	InverseUpdateEffElmtLeft();
	
	m_nSelEffElmt--;

	UpdateEffElmtLeft();
}

void CSkillData::OnBnClickedButtonEffElmtRight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( m_nSelEffElmt >= EMELEMENT_MAXNUM-1  ) return;

	InverseUpdateEffElmtLeft();

	m_nSelEffElmt++;
	
	UpdateEffElmtLeft();
}

void CSkillData::UpdateEffElmtLeft()
{
	EMELEMENT emELMT = (EMELEMENT) m_nSelEffElmt;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		SetWin_Text ( this, IDC_EDIT_EFF_ELMT, CGameTextMan::GetInstance().GetCommentText("ELEMENT",emELMT));
	else
		SetWin_Text ( this, IDC_EDIT_EFF_ELMT, COMMENT::ELEMENT[emELMT].c_str() );

	SetWin_Text ( this, IDC_EDIT_SELFZONEEFFECT, m_Item.m_sEXT_DATA.GETSELFZONE01(emELMT) );
	SetWin_Text ( this, IDC_EDIT_SELFZONEEFFECT2, m_Item.m_sEXT_DATA.GETSELFZONE02(emELMT) );
	SetWin_Text ( this, IDC_EDIT_SELFZONEEFFECT4, m_Item.m_sEXT_DATA.GETSELFZONE03(emELMT) );
	SetWin_Text ( this, IDC_EDIT_TARGZONEEFFECT, m_Item.m_sEXT_DATA.GETTARGZONE01(emELMT) );
	SetWin_Text ( this, IDC_EDIT_TARGZONEEFFECT2, m_Item.m_sEXT_DATA.GETTARGZONE02(emELMT) );
	SetWin_Text ( this, IDC_EDIT_TARGZONEEFFECT3, m_Item.m_sEXT_DATA.GETTARGZONE03(emELMT) );

	SetWin_Text ( this, IDC_EDIT_SELFBODYEFFECT, m_Item.m_sEXT_DATA.GETSELFBODY(emELMT) );
	SetWin_Text ( this, IDC_EDIT_TARGETBODYEFFECT, m_Item.m_sEXT_DATA.GETTARGBODY01(emELMT) );
	SetWin_Text ( this, IDC_EDIT_TARGETBODYEFFECT2, m_Item.m_sEXT_DATA.GETTARGBODY02(emELMT) );
	SetWin_Text ( this, IDC_EDIT_TARGETEFFECT, m_Item.m_sEXT_DATA.GETTARG(emELMT) );
}

void CSkillData::InverseUpdateEffElmtLeft()
{
	EMELEMENT emELMT = (EMELEMENT) m_nSelEffElmt;

	m_Item.m_sEXT_DATA.strSELFZONE01[emELMT] = GetWin_Text ( this, IDC_EDIT_SELFZONEEFFECT );
	m_Item.m_sEXT_DATA.strSELFZONE02[emELMT] = GetWin_Text ( this, IDC_EDIT_SELFZONEEFFECT2 );
	m_Item.m_sEXT_DATA.strSELFZONE03[emELMT] = GetWin_Text ( this, IDC_EDIT_SELFZONEEFFECT4 );
	m_Item.m_sEXT_DATA.strTARGZONE01[emELMT] = GetWin_Text ( this, IDC_EDIT_TARGZONEEFFECT );
	m_Item.m_sEXT_DATA.strTARGZONE02[emELMT] = GetWin_Text ( this, IDC_EDIT_TARGZONEEFFECT2 );
	m_Item.m_sEXT_DATA.strTARGZONE03[emELMT] = GetWin_Text ( this, IDC_EDIT_TARGZONEEFFECT3 );

	m_Item.m_sEXT_DATA.strSELFBODY[emELMT] = GetWin_Text ( this, IDC_EDIT_SELFBODYEFFECT );
	m_Item.m_sEXT_DATA.strTARGBODY01[emELMT] = GetWin_Text ( this, IDC_EDIT_TARGETBODYEFFECT );
	m_Item.m_sEXT_DATA.strTARGBODY02[emELMT] = GetWin_Text ( this, IDC_EDIT_TARGETBODYEFFECT2 );
	m_Item.m_sEXT_DATA.strTARG[emELMT] = GetWin_Text ( this, IDC_EDIT_TARGETEFFECT );

}

void CSkillData::OnBnClickedButtonSkillType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string> vecSKILL_TYPES;
		std::string strSKILL_TYPES[SKILL::FOR_TYPE_SIZE];
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("SKILL_TYPES", vecSKILL_TYPES, CGameTextMan::EM_COMMENT_TEXT);
		for ( INT i = 0; i<SKILL::FOR_TYPE_SIZE; ++i )
			strSKILL_TYPES[i] = vecSKILL_TYPES[i];

		dlg.SetStringList( strSKILL_TYPES, SKILL::FOR_TYPE_SIZE );
		dlg.m_nSelect = m_Item.m_sAPPLY.emBASIC_TYPE;
		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sAPPLY.emBASIC_TYPE = (SKILL::EMTYPES) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_SKILL_TYPE, strSKILL_TYPES[m_Item.m_sAPPLY.emBASIC_TYPE].c_str() );	
			BOOL bTYPE_CURE = ( m_Item.m_sAPPLY.emBASIC_TYPE==SKILL::EMFOR_CURE );
			SetWin_Enable(this,IDC_CHECK_PALSY, bTYPE_CURE);
			SetWin_Enable(this,IDC_CHECK_SWOON, bTYPE_CURE);
			SetWin_Enable(this,IDC_CHECK_PETRIFY, bTYPE_CURE);
			SetWin_Enable(this,IDC_CHECK_BURN, bTYPE_CURE);
			SetWin_Enable(this,IDC_CHECK_FROZEN, bTYPE_CURE);
			SetWin_Enable(this,IDC_CHECK_DERANGE, bTYPE_CURE);
			SetWin_Enable(this,IDC_CHECK_POISON, bTYPE_CURE);
			SetWin_Enable(this,IDC_CHECK_CURSE, bTYPE_CURE);
			SetReadMode();
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::SKILL_TYPES, SKILL::FOR_TYPE_SIZE );	
		dlg.m_nSelect = m_Item.m_sAPPLY.emBASIC_TYPE;
		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sAPPLY.emBASIC_TYPE = (SKILL::EMTYPES) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_SKILL_TYPE, COMMENT::SKILL_TYPES[m_Item.m_sAPPLY.emBASIC_TYPE].c_str() );	
			BOOL bTYPE_CURE = ( m_Item.m_sAPPLY.emBASIC_TYPE==SKILL::EMFOR_CURE );
			SetWin_Enable ( this, IDC_CHECK_PALSY, bTYPE_CURE );
			SetWin_Enable ( this, IDC_CHECK_SWOON, bTYPE_CURE );
			SetWin_Enable ( this, IDC_CHECK_PETRIFY, bTYPE_CURE );
			SetWin_Enable ( this, IDC_CHECK_BURN, bTYPE_CURE );
			SetWin_Enable ( this, IDC_CHECK_FROZEN, bTYPE_CURE );
			SetWin_Enable ( this, IDC_CHECK_DERANGE, bTYPE_CURE );
			SetWin_Enable ( this, IDC_CHECK_POISON, bTYPE_CURE );
			SetWin_Enable ( this, IDC_CHECK_CURSE, bTYPE_CURE );
			SetReadMode();
		}
	}
}

void CSkillData::OnBnClickedButtonElement()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strELEMENT[EMELEMENT_MAXNUM2];
		CGameTextMan::GetInstance().LoadHelpCommentString("ELEMENT", strELEMENT, CGameTextMan::EM_COMMENT_TEXT, EMELEMENT_MAXNUM2);
		dlg.SetStringList( strELEMENT, EMELEMENT_MAXNUM2 );	
		dlg.m_nSelect = m_Item.m_sAPPLY.emELEMENT;
		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sAPPLY.emELEMENT = (EMELEMENT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_ELEMENT, strELEMENT[m_Item.m_sAPPLY.emELEMENT].c_str() );	
			BOOL bTYPE_CURE = ( m_Item.m_sAPPLY.emBASIC_TYPE==SKILL::EMFOR_CURE );
			SetWin_Enable ( this, IDC_BUTTON_EFF_ELMT_LEFT, m_Item.m_sAPPLY.emELEMENT==EMELEMENT_ARM );
			SetWin_Enable ( this, IDC_BUTTON_EFF_ELMT_RIGHT, m_Item.m_sAPPLY.emELEMENT==EMELEMENT_ARM );
			SetReadMode();
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::ELEMENT, EMELEMENT_MAXNUM2 );	
		dlg.m_nSelect = m_Item.m_sAPPLY.emELEMENT;
		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sAPPLY.emELEMENT = (EMELEMENT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_ELEMENT, COMMENT::ELEMENT[m_Item.m_sAPPLY.emELEMENT].c_str() );	
			BOOL bTYPE_CURE = ( m_Item.m_sAPPLY.emBASIC_TYPE==SKILL::EMFOR_CURE );
			SetWin_Enable ( this, IDC_BUTTON_EFF_ELMT_LEFT, m_Item.m_sAPPLY.emELEMENT==EMELEMENT_ARM );
			SetWin_Enable ( this, IDC_BUTTON_EFF_ELMT_RIGHT, m_Item.m_sAPPLY.emELEMENT==EMELEMENT_ARM );
			SetReadMode();
		}
	}
}

void CSkillData::OnBnClickedButtonBlowType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string> vecBLOW;
		std::string strBLOW[EMBLOW_SIZE];
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("BLOW", vecBLOW, CGameTextMan::EM_COMMENT_TEXT);
		for ( INT i = 0; i < EMBLOW_SIZE; ++i )
			strBLOW[i] = vecBLOW[i];

		dlg.SetStringList( strBLOW, EMBLOW_SIZE );	
		dlg.m_nSelect = m_Item.m_sAPPLY.emSTATE_BLOW;
		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sAPPLY.emSTATE_BLOW = (EMSTATE_BLOW) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_BLOW_TYPE, strBLOW[m_Item.m_sAPPLY.emSTATE_BLOW].c_str() );	
		}
	}
	else
	{	
		dlg.SetStringList( COMMENT::BLOW, EMBLOW_SIZE );	
		dlg.m_nSelect = m_Item.m_sAPPLY.emSTATE_BLOW;
		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sAPPLY.emSTATE_BLOW = (EMSTATE_BLOW) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_BLOW_TYPE, COMMENT::BLOW[m_Item.m_sAPPLY.emSTATE_BLOW].c_str() );	
		}
	}
}

void CSkillData::OnBnClickedButtonImpactAddonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if ( m_Item.m_sAPPLY.vecADDON.size() >= SKILL::MAX_ADDON  ) return;

	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strIMPACT_ADDON[EIMPACTA_SIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("IMPACT_ADDON", strIMPACT_ADDON, CGameTextMan::EM_COMMENT_TEXT, EIMPACTA_SIZE);
		dlg.SetStringList( strIMPACT_ADDON, EIMPACTA_SIZE );	
		dlg.m_nSelect = EMIMPACTA_NONE;
		if ( dlg.DoModal() == IDOK )
		{
			if ( dlg.m_dwData == EMIMPACTA_NONE )	return;

			SKILL::SIMPACT_ADDON sAddon;
			sAddon.emADDON = (EMIMPACT_ADDON) dlg.m_dwData;
			m_Item.m_sAPPLY.vecADDON.push_back( sAddon );
			CString strNAME =  strIMPACT_ADDON[sAddon.emADDON].c_str();	
			int nIndex = m_listImpactAddon.AddString ( strNAME );
			m_listImpactAddon.SetItemData ( nIndex, m_Item.m_sAPPLY.vecADDON.size()-1 );
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::IMPACT_ADDON, EIMPACTA_SIZE );	
		dlg.m_nSelect = EMIMPACTA_NONE;
		if ( dlg.DoModal() == IDOK )
		{
			if ( dlg.m_dwData == EMIMPACTA_NONE )	return;

			SKILL::SIMPACT_ADDON sAddon;
			sAddon.emADDON = (EMIMPACT_ADDON) dlg.m_dwData;
			m_Item.m_sAPPLY.vecADDON.push_back( sAddon );

			CString strNAME =  COMMENT::IMPACT_ADDON[sAddon.emADDON].c_str();	
			int nIndex = m_listImpactAddon.AddString ( strNAME );
			m_listImpactAddon.SetItemData ( nIndex, m_Item.m_sAPPLY.vecADDON.size()-1 );
		}
	}
}

void CSkillData::OnBnClickedButtonImpactAddonDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_listImpactAddon.GetCurSel();
	if ( nIndex < 0 ) return;
	if ( nIndex > (int)m_Item.m_sAPPLY.vecADDON.size()-1 )	return;


	std::vector<SKILL::SIMPACT_ADDON>::iterator pos = m_Item.m_sAPPLY.vecADDON.begin();
	for ( int i = 0; pos != m_Item.m_sAPPLY.vecADDON.end(); ++pos, ++i )
	{
		if ( i == nIndex )
		{
			m_Item.m_sAPPLY.vecADDON.erase ( pos );
			break;
		}
	}

	m_listImpactAddon.DeleteString( nIndex );

}

void CSkillData::OnBnClickedButtonSpecTypeAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( m_Item.m_sAPPLY.vecSPEC.size() >= SKILL::MAX_ADDON  ) return;

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
			SKILL::SSPEC_ADDON sAddon;
			sAddon.emSPEC = (EMSPEC_ADDON) dlg.m_dwData;

			if( !IsAddEnableCountAttack(sAddon.emSPEC) ) return;
			if( IsInputListCountAttack(dlg.m_dwData) ) return;

			m_Item.m_sAPPLY.vecSPEC.push_back( sAddon );
			CString strNAME = strISPEC_ADDON[sAddon.emSPEC].c_str();
			int nIndex = m_listSpecAddon.AddString ( strNAME );
			m_listSpecAddon.SetItemData ( nIndex, m_Item.m_sAPPLY.vecSPEC.size()-1 );
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::SPEC_ADDON, EMSPECA_NSIZE );	
		dlg.m_nSelect = EMSPECA_NULL;
		if ( dlg.DoModal() == IDOK )
		{
			if ( dlg.m_dwData == EMSPECA_NULL )	return;
			SKILL::SSPEC_ADDON sAddon;
			sAddon.emSPEC = (EMSPEC_ADDON)dlg.m_dwData;

			if( !IsAddEnableCountAttack(sAddon.emSPEC) ) return;
			if( IsInputListCountAttack(dlg.m_dwData) ) return;

			switch( sAddon.emSPEC )
			{
			case EMSPECA_ACTIVATE_DAMAGE :
				{
					for( int i = 0; i < SKILL::MAX_LEVEL; ++i )
					{
						sAddon.sSPEC[i].nVAR1 = 0;
						sAddon.sSPEC[i].nVAR2 = 0;
					}				
				}
				break;
			case EMSPECA_ENTRANCE_EXIT :
			case EMSPECA_COUNTATTACK :
				{
					for( int i = 0; i < SKILL::MAX_LEVEL; ++i )
					{
						sAddon.sSPEC[i].fVAR1 = SNATIVEID::ID_NULL;
						sAddon.sSPEC[i].fVAR2 = SNATIVEID::ID_NULL;
					}				
				}
				break;
			case EMSPECA_PROVOKE :
			case EMSPECA_SCREEN_THROW :
				{
					for( int i = 0; i < SKILL::MAX_LEVEL; ++i )
					{
						sAddon.sSPEC[i].fVAR1 = SNATIVEID::ID_NULL;
					}
				}
				break;
			case EMSPECA_CAMERA_HIT :
				{
					for( int i = 0; i < SKILL::MAX_LEVEL; ++i )
					{
						sAddon.sSPEC[i].fVAR1 = SNATIVEID::ID_NULL;
						//sAddon.sSPEC[i].fVAR2 = 0;
					}
				}
				break;
			}

			m_Item.m_sAPPLY.vecSPEC.push_back( sAddon );
			CString strNAME = COMMENT::SPEC_ADDON[sAddon.emSPEC].c_str();
			int nIndex = m_listSpecAddon.AddString ( strNAME );
			m_listSpecAddon.SetItemData ( nIndex, m_Item.m_sAPPLY.vecSPEC.size()-1 );
		}
	}
}

bool CSkillData::IsInputListCountAttack(DWORD dwSpec)
{
	int nIndex = 0;
	int iCnt = m_listSpecAddon.GetCount();

	std::vector<SKILL::SSPEC_ADDON>::iterator pos = m_Item.m_sAPPLY.vecSPEC.begin();
	
	while( pos != m_Item.m_sAPPLY.vecSPEC.end())
	{
		if( (*pos).emSPEC == EMSPECA_COUNTATTACK && dwSpec == EMSPECA_COUNTATTACK)
		{
			return true;
		}

		++nIndex;
		++pos;
	}
	
	return false;
}
bool CSkillData::IsAddEnableCountAttack(EMSPEC_ADDON eSpec)
{
	DWORD dwEmRol = GetWin_Combo_Sel ( this, IDC_COMBO_ROLE );

	switch(dwEmRol)
	{
	case SKILL::EMROLE_NORMAL  : break;
	case SKILL::EMROLE_PASSIVE :
		{
			if( eSpec == EMSPECA_COUNTATTACK ) return false;
		}
		break;
	}

	return true;
}

void CSkillData::OnBnClickedButtonSpecTypeDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_listSpecAddon.GetCurSel();
	if ( nIndex < 0 ) return;
	if ( nIndex > (int)m_Item.m_sAPPLY.vecSPEC.size()-1 )	return;


	std::vector<SKILL::SSPEC_ADDON>::iterator pos = m_Item.m_sAPPLY.vecSPEC.begin();
	for ( int i = 0; pos != m_Item.m_sAPPLY.vecSPEC.end(); ++pos, ++i )
	{
		if ( i == nIndex )
		{
			if( (*pos).emSPEC == EMSPECA_COUNTATTACK )
			{
				m_Item.m_sAPPLY.sCAData.AllReset();
			}

			m_Item.m_sAPPLY.vecSPEC.erase ( pos );
			break;
		}
	}

	m_listSpecAddon.DeleteString( nIndex );
}



void CSkillData::OnBnClickedButtonSpecial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strSPECIAL_SKILL_TYPE[SKILL::EMSSTYPE_NSIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("SPECIAL_SKILL_TYPE", strSPECIAL_SKILL_TYPE, CGameTextMan::EM_COMMENT_TEXT, SKILL::EMSSTYPE_NSIZE);
		dlg.SetStringList( strSPECIAL_SKILL_TYPE, SKILL::EMSSTYPE_NSIZE );	
		dlg.m_nSelect = m_Item.m_sSPECIAL_SKILL.emSSTYPE;
		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sSPECIAL_SKILL.emSSTYPE = (SKILL::EMSPECIALSKILLTYPE) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_SPECIAL, strSPECIAL_SKILL_TYPE[m_Item.m_sSPECIAL_SKILL.emSSTYPE].c_str() );	
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::SPECIAL_SKILL_TYPE, SKILL::EMSSTYPE_NSIZE );	
		dlg.m_nSelect = m_Item.m_sSPECIAL_SKILL.emSSTYPE;
		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sSPECIAL_SKILL.emSSTYPE = (SKILL::EMSPECIALSKILLTYPE) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_SPECIAL, COMMENT::SPECIAL_SKILL_TYPE[m_Item.m_sSPECIAL_SKILL.emSSTYPE].c_str() );	
		}
	}
}

void CSkillData::OnBnClickedButtonMaintype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strANI_MAINTYPE[SKILL::EMSSTYPE_NSIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("ANI_MAINTYPE", strANI_MAINTYPE, CGameTextMan::EM_COMMENT_TEXT, AN_TYPE_SIZE);
		dlg.SetStringList( strANI_MAINTYPE, AN_TYPE_SIZE );	
		dlg.m_nSelect = m_Item.m_sEXT_DATA.emANIMTYPE;
		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sEXT_DATA.emANIMTYPE = (EMANI_MAINTYPE) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_MAINTYPE, strANI_MAINTYPE[m_Item.m_sEXT_DATA.emANIMTYPE].c_str() );
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::ANI_MAINTYPE, AN_TYPE_SIZE );	
		dlg.m_nSelect = m_Item.m_sEXT_DATA.emANIMTYPE;
		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sEXT_DATA.emANIMTYPE = (EMANI_MAINTYPE) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_MAINTYPE, COMMENT::ANI_MAINTYPE[m_Item.m_sEXT_DATA.emANIMTYPE].c_str() );
		}
	}
	UpdateAnimInfo();
}

void CSkillData::OnBnClickedButtonSubtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string> vecANI_SUB00, vecANI_SUB03;	
		std::string strANI_SUB00[AN_SUB_00_SIZE], strANI_SUB03[AN_SUB_00_SIZE];
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("ANI_SUBTYPE_00", vecANI_SUB00, CGameTextMan::EM_COMMENT_TEXT);
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("ANI_SUBTYPE_03", vecANI_SUB03, CGameTextMan::EM_COMMENT_TEXT);
		for ( INT i = 0; i < AN_SUB_00_SIZE; ++i )
		{
			strANI_SUB00[i] = vecANI_SUB00[i];
			strANI_SUB03[i] = vecANI_SUB03[i];
		}
		if( m_Item.m_sBASIC.sNATIVEID.wMainID == EMSKILL_BIKE )
		{
			dlg.SetStringList( strANI_SUB03, AN_SUB_00_SIZE );	
			dlg.m_nSelect = m_Item.m_sEXT_DATA.emANISTYPE;
		}
		else
		{
			dlg.SetStringList( strANI_SUB00, AN_SUB_00_SIZE );	
			dlg.m_nSelect = m_Item.m_sEXT_DATA.emANISTYPE;
		}
		if ( dlg.DoModal() == IDOK )
		{
			if( m_Item.m_sBASIC.sNATIVEID.wMainID == EMSKILL_BIKE )
			{
				m_Item.m_sEXT_DATA.emANISTYPE = (EMANI_SUBTYPE) dlg.m_dwData;
				SetWin_Text( this, IDC_EDIT_SUBTYPE, strANI_SUB03[m_Item.m_sEXT_DATA.emANISTYPE].c_str() );
			}
			else
			{
				m_Item.m_sEXT_DATA.emANISTYPE = (EMANI_SUBTYPE) dlg.m_dwData;
				SetWin_Text( this, IDC_EDIT_SUBTYPE, strANI_SUB00[m_Item.m_sEXT_DATA.emANISTYPE].c_str() );
			}
		}
	}
	else
	{
		if( m_Item.m_sBASIC.sNATIVEID.wMainID == EMSKILL_BIKE )
		{
			dlg.SetStringList( COMMENT::ANI_SUBTYPE_03, AN_SUB_00_SIZE );	
			dlg.m_nSelect = m_Item.m_sEXT_DATA.emANISTYPE;
		}
		else
		{
			dlg.SetStringList( COMMENT::ANI_SUBTYPE_00, AN_SUB_00_SIZE );	
			dlg.m_nSelect = m_Item.m_sEXT_DATA.emANISTYPE;
		}

		if ( dlg.DoModal() == IDOK )
		{
			if( m_Item.m_sBASIC.sNATIVEID.wMainID == EMSKILL_BIKE )
			{
				m_Item.m_sEXT_DATA.emANISTYPE = (EMANI_SUBTYPE) dlg.m_dwData;
				SetWin_Text( this, IDC_EDIT_SUBTYPE, COMMENT::ANI_SUBTYPE_03[m_Item.m_sEXT_DATA.emANISTYPE].c_str() );
			}
			else
			{
				m_Item.m_sEXT_DATA.emANISTYPE = (EMANI_SUBTYPE) dlg.m_dwData;
				SetWin_Text( this, IDC_EDIT_SUBTYPE, COMMENT::ANI_SUBTYPE_00[m_Item.m_sEXT_DATA.emANISTYPE].c_str() );
			}
		}
	}
	UpdateAnimInfo();
}

void CSkillData::OnBnClickedButtonAttackleft()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string> vecSKILLATTACK;
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("SKILLATTACK", vecSKILLATTACK, CGameTextMan::EM_COMMENT_TEXT);

		std::string strSKILLATTACK[SKILL::SKILLATT_NSIZE+1];
		int i, nSize = static_cast<int>(vecSKILLATTACK.size());
		for ( i = 0; i < nSize; ++i )
			strSKILLATTACK[i] = vecSKILLATTACK[i];

		dlg.SetStringList( strSKILLATTACK, SKILL::SKILLATT_NSIZE+1 );	
		dlg.m_nSelect = m_Item.m_sBASIC.emUSE_LITEM;

		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sBASIC.emUSE_LITEM = (SKILL::GLSKILL_ATT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_ATTACKLEFT, vecSKILLATTACK[m_Item.m_sBASIC.emUSE_LITEM].c_str() );	
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::SKILLATTACK, SKILL::SKILLATT_NSIZE+1 );	
		dlg.m_nSelect = m_Item.m_sBASIC.emUSE_LITEM;

		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sBASIC.emUSE_LITEM = (SKILL::GLSKILL_ATT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_ATTACKLEFT, COMMENT::SKILLATTACK[m_Item.m_sBASIC.emUSE_LITEM].c_str() );	
		}
	}
}

void CSkillData::OnBnClickedButtonAttackright()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string> vecSKILLATTACK;
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("SKILLATTACK", vecSKILLATTACK, CGameTextMan::EM_COMMENT_TEXT);
		std::string strSKILLATTACK[SKILL::SKILLATT_NSIZE+1];
		int i, nSize = static_cast<int>(vecSKILLATTACK.size());
		for ( i = 0; i < nSize; ++i )
			strSKILLATTACK[i] = vecSKILLATTACK[i];

		dlg.SetStringList( strSKILLATTACK, SKILL::SKILLATT_NSIZE+1 );	
		dlg.m_nSelect = m_Item.m_sBASIC.emUSE_RITEM;

		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sBASIC.emUSE_RITEM = (SKILL::GLSKILL_ATT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_ATTACKRIGHT, vecSKILLATTACK[m_Item.m_sBASIC.emUSE_RITEM].c_str() );	
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::SKILLATTACK, SKILL::SKILLATT_NSIZE+1 );	
		dlg.m_nSelect = m_Item.m_sBASIC.emUSE_RITEM;

		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sBASIC.emUSE_RITEM = (SKILL::GLSKILL_ATT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_ATTACKRIGHT, COMMENT::SKILLATTACK[m_Item.m_sBASIC.emUSE_RITEM].c_str() );
		}
	}
}
void CSkillData::OnBnClickedButtonVehicleMaintype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string> vecANI_VEHICLE_MAINTYPE;
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("ANI_VEHICLE_MAINTYPE", vecANI_VEHICLE_MAINTYPE, CGameTextMan::EM_COMMENT_TEXT);

		std::string strANI_VEHICLE_MAINTYPE[AN_VEHICLE_SIZE];
		int i, nSize = static_cast<int>(vecANI_VEHICLE_MAINTYPE.size());
		for ( i = 0; i < nSize; ++i )
			strANI_VEHICLE_MAINTYPE[i] = vecANI_VEHICLE_MAINTYPE[i];

		dlg.SetStringList( strANI_VEHICLE_MAINTYPE, AN_VEHICLE_SIZE );	
		dlg.m_nSelect = m_Item.m_sEXT_DATA.emANISTYPE;

		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sEXT_DATA.emANIVehicleMTYPE = (EMANI_MAINTYPE) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_VEHICLE_MAINTYPE, vecANI_VEHICLE_MAINTYPE[m_Item.m_sEXT_DATA.emANIVehicleMTYPE].c_str() );
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::ANI_VEHICLE_MAINTYPE, AN_VEHICLE_SIZE );	
		dlg.m_nSelect = m_Item.m_sEXT_DATA.emANISTYPE;

		if ( dlg.DoModal() == IDOK )
		{
			m_Item.m_sEXT_DATA.emANIVehicleMTYPE = (EMANI_MAINTYPE) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_VEHICLE_MAINTYPE, COMMENT::ANI_VEHICLE_MAINTYPE[m_Item.m_sEXT_DATA.emANIVehicleMTYPE].c_str() );
		}
	}
	UpdateAnimInfo();
}
void CSkillData::OnBnClickedButtonTargzoneeffect4()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CSkillData::OnBnClickedCheckHiddenWeapon()
{
	if( GetWin_Check( this, IDC_CHECK_HIDDENWEAPON ) )
	{
		SetWin_Enable ( this, IDC_EDIT_HIDDENWEAPON_MID, TRUE );
		SetWin_Enable ( this, IDC_EDIT_HIDDENWEAPON_SID, TRUE );
	}
	else
	{
		SetWin_Enable ( this, IDC_EDIT_HIDDENWEAPON_MID, FALSE );
		SetWin_Enable ( this, IDC_EDIT_HIDDENWEAPON_SID, FALSE );
	}
}

void CSkillData::OnTimer(UINT_PTR nIDEvent)
{
    switch(nIDEvent)
    {
    case TIMER_SKILLDATA_SAVE_STATE_ID:

        SetWin_Text ( this, IDC_EDIT_SAVE_STATE, "" );
        KillTimer ( TIMER_SKILLDATA_SAVE_STATE_ID );

        break;
    }

    CPropertyPage::OnTimer(nIDEvent);
}
