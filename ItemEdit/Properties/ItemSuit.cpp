// ItemProperty3.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../ItemEdit.h"
#include "../Properties/ItemSuit.h"
#include "../EngineLib/G-Logic/GLOGIC.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "../RanLogic/Item/GLItem.h"
#include "../RanLogic/Crow/GLCrowData.h"
#include "../RanLogic/Crow/GLCrowDataMan.h"
#include "../RanLogic/Character/GLCharDefine.h"
#include "../RanLogic/GLogicDataMan.h"

#include "../DlgItem.h"
#include "SheetWithTab.h"
#include ".\itemsuit.h"
#include "../InputDataDlg.h"

#include "../RanLogic/GLUseFeatures.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "../RanLogic/Pet/GLPet.h"
#include "../RanLogic/Vehicle/GLVEHICLE.h"
#include "../SelectDialog.h"
#include "../SelectCheckDialog.h"

// CItemSuit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CItemSuit, CPropertyPage)
CItemSuit::CItemSuit( LOGFONT logfont )
	: CPropertyPage(CItemSuit::IDD),
	m_pFont ( NULL )
{
	m_bDlgInit = FALSE;
	m_pItem = NULL;
	m_pDummyItem = new SITEM;

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CItemSuit::~CItemSuit()
{
	SAFE_DELETE ( m_pDummyItem );
	SAFE_DELETE ( m_pFont );

}


void CItemSuit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_listBOX);
	DDX_Control(pDX, IDC_LIST_BOX2, m_listBox_Rand);
	DDX_Control(pDX, IDC_EDIT_QUE_TIME, m_Edit_QueTime);
	DDX_Control(pDX, IDC_EDIT_QUE_EXP, m_Edit_QueExp);
	DDX_Control(pDX, IDC_EDIT_QUE_PARAM1, m_Edit_QueParam1);
	DDX_Control(pDX, IDC_EDIT_QUE_PARAM2, m_Edit_QueParam2);
	DDX_Control(pDX, IDC_LIST_MONSTER, m_list_Mob );
}


BEGIN_MESSAGE_MAP(CItemSuit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_BOX_ADD, OnBnClickedButtonBoxAdd)
	ON_BN_CLICKED(IDC_BUTTON_BOX_DEL, OnBnClickedButtonBoxDel)
	ON_BN_CLICKED(IDC_BUTTON_BOX_ADD2, OnBnClickedButtonBoxAdd2)
	ON_BN_CLICKED(IDC_BUTTON_BOX_DEL2, OnBnClickedButtonBoxDel2)
	ON_BN_CLICKED(IDC_BUTTON_BOX_UP1, OnBnClickedButtonBoxUp1)
	ON_BN_CLICKED(IDC_BUTTON_BOX_DW1, OnBnClickedButtonBoxDw1)
	ON_BN_CLICKED(IDC_BUTTON_BOX_UP2, OnBnClickedButtonBoxUp2)
	ON_BN_CLICKED(IDC_BUTTON_BOX_DW2, OnBnClickedButtonBoxDw2)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PETSKIN, OnBnClickedButtonAddPetskin)
	ON_BN_CLICKED(IDC_BUTTON_DEL_PETSKIN, OnBnClickedButtonDelPetskin)
	ON_BN_CLICKED(IDC_BUTTON_BOX_EDIT, OnBnClickedButtonBoxEdit)
	ON_BN_CLICKED(IDC_BUTTON_BOX_EDIT2, OnBnClickedButtonBoxEdit2)
	ON_BN_CLICKED(IDC_BUTTON_SUIT, OnBnClickedButtonSuit)
	ON_BN_CLICKED(IDC_BUTTON_ATT_TYPE, OnBnClickedButtonAttType)
	ON_BN_CLICKED(IDC_BUTTON_ADDON_NO1, OnBnClickedButtonAddonNo1)
	ON_BN_CLICKED(IDC_BUTTON_ADDON_NO2, OnBnClickedButtonAddonNo2)
	ON_BN_CLICKED(IDC_BUTTON_ADDON_NO3, OnBnClickedButtonAddonNo3)
	ON_BN_CLICKED(IDC_BUTTON_ADDON_NO4, OnBnClickedButtonAddonNo4)
	ON_BN_CLICKED(IDC_BUTTON_ADDON_NO5, OnBnClickedButtonAddonNo5)
	ON_BN_CLICKED(IDC_BUTTON_ADDON_NO6, OnBnClickedButtonAddonNo6)
	ON_BN_CLICKED(IDC_BUTTON_BLOW, OnBnClickedButtonBlow)
	ON_BN_CLICKED(IDC_BUTTON_QUESTION, OnBnClickedButtonQuestion)
	ON_BN_CLICKED(IDC_BUTTON_DRUG, OnBnClickedButtonDrug)
	ON_CBN_SELCHANGE(IDC_COMBO_VEHICLE_TYPE, OnCbnSelchangeComboVehicleType)
	ON_CBN_SELCHANGE(IDC_COMBO_PARTS_TYPE, OnCbnSelchangeComboPartsType)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_COSTUM, OnBnClickedCheckEnableCostum)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_UNIQUE, OnBnClickedCheckEnableUnique)
	ON_BN_CLICKED(IDC_BUTTON_ER, OnBnClickedDrugErList)

    ON_WM_TIMER()
//	ON_EN_CHANGE(IDC_SKILL_RATE, &CItemSuit::OnEnChangeSkillRate)
ON_CBN_SELCHANGE(IDC_SKILL_TARGET, &CItemSuit::OnCbnSelchangeSkillTarget)
END_MESSAGE_MAP()


// CItemSuit 메시지 처리기입니다.

BOOL CItemSuit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	m_list_Mob.SetExtendedStyle ( m_list_Mob.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_list_Mob.GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	const int nColumnCount = 4;	
	char* szColumnName1[nColumnCount] = { "Num", "Monser ID", "Scale", "Rate" };
	int nColumnWidthPercent1[nColumnCount] = { 15, 50, 17, 18 };

	for ( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = szColumnName1[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent1[i] ) / 100;
		m_list_Mob.InsertColumn (i, &lvColumn );
	}

	m_bDlgInit = TRUE;

    InitDefaultCtrls ();
	UpdateItems ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CItemSuit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pItem = NULL;
	m_pSheetTab->ActiveItemSuitTreePage ();
}

void CItemSuit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !InverseUpdateItems() ) return;

	CItemNode sItemNode;
	sItemNode.m_sItem = *m_pDummyItem;

	GLItemMan::GetInstance().InsertItem ( m_pDummyItem->sBasicOp.sNativeID, &sItemNode );

    ShowSaveState();
}

void CItemSuit::ShowSaveState()
{
    SetWin_Text ( this, IDC_EDIT_SAVE_STATE, "Save Successed." );

    // SetTimer는 기존에 같은 ID 로 정의된 이벤트가 있으면
    // 그 이벤트를 죽이고 새로운 이벤트를 덮어씌으무로
    // 그에 따른 예외처리는 따로 하지 않습니다.
    SetTimer ( TIMER_ITEMSUIT_SAVE_STATE_ID, SAVE_MESSAGE_APPEAR_TIME, NULL );
}

void CItemSuit::InitAllItem ()
{
	m_pItem = NULL;
}

BOOL CItemSuit::SetItem ( SITEM* pItem )
{
	if ( m_pItem )
	{
		return FALSE;
	}

	m_pItem = pItem;

	//	더미로 가져가기
	*m_pDummyItem = *m_pItem;

	if ( m_bDlgInit ) 
	{
		InitDefaultCtrls ();
		UpdateItems ();		
	}
	
	return TRUE;
}


void CItemSuit::UpdateItems ()
{
	// 코스튬 염색 가능. by luxes.
	SetWin_Check( this, IDC_CHECK_ENABLE_COSTUM, m_pDummyItem->IsColorChange() );

	// 고유
	SetWin_Check( this, IDC_CHECK_ENABLE_UNIQUE, m_pDummyItem->IsUnique() );

	//	손 착용 설정
	SetWin_Check ( this, IDC_CHECK_TWOH, m_pDummyItem->sSuitOp.IsBOTHHAND() );
	SetWin_Check ( this, IDC_CHECK_BIGH, m_pDummyItem->sSuitOp.IsBIG() );
	SetWin_Check ( this, IDC_CHECK_BROOM, m_pDummyItem->sSuitOp.IsBROOM() );

	SetWin_Check ( this, IDC_RADIO_RIGHT, m_pDummyItem->sSuitOp.emHand==HAND_RIGHT );
	SetWin_Check ( this, IDC_RADIO_LEFT, m_pDummyItem->sSuitOp.emHand==HAND_LEFT );
	SetWin_Check ( this, IDC_RADIO_BOTH, m_pDummyItem->sSuitOp.emHand==HAND_BOTH );

	//	공격 거리
	SetWin_Num_int ( this, IDC_EDIT_ATT_RANGE, m_pDummyItem->sSuitOp.wAttRange );
	SetWin_Num_int ( this, IDC_EDIT_REQSP, m_pDummyItem->sSuitOp.wReqSP );

	//	기본 옵션
	SetWin_Num_int ( this, IDC_EDIT_HIT, m_pDummyItem->sSuitOp.nHitRate );
	SetWin_Num_int ( this, IDC_EDIT_AVOIDRATE, m_pDummyItem->sSuitOp.nAvoidRate );

	//	공격 방어
	SetWin_Num_int ( this, IDC_EDIT_DAMAGE_HIGH, m_pDummyItem->GetAttackDamage().GetHigh() );
	SetWin_Num_int ( this, IDC_EDIT_DAMAGE_LOW, m_pDummyItem->GetAttackDamage().GetLow() );
	SetWin_Num_int ( this, IDC_EDIT_DEFENSE, m_pDummyItem->GetDefence() );

	//	공격 방어 랜덤세트 매니저 인덱스
	SetWin_Num_int ( this, IDC_EDIT_DAMAGE_RSET_TYPE, m_pDummyItem->GetBasicAttackDamageType() );
	SetWin_Num_int ( this, IDC_EDIT_DAMAGE_RSET_INDEX, m_pDummyItem->GetBasicAttackDamageIndex() );
	SetWin_Num_int ( this, IDC_EDIT_DEFENSE_RSET_TYPE, m_pDummyItem->GetBasicDefenceType() );
	SetWin_Num_int ( this, IDC_EDIT_DEFENSE_RSET_INDEX, m_pDummyItem->GetBasicDefenceIndex() );

	//	공격 방어 개조가능 여부
	SetWin_Check( this, IDC_CHECK_DAMAGE_EDITABLE, m_pDummyItem->IsEditableBasicAttackDamage() );
	SetWin_Check( this, IDC_CHECK_DEFENSE_EDITABLE, m_pDummyItem->IsEditableBasicDefence() );

	// 업그레이드 수치
	SetWin_Num_int ( this, IDC_EDIT_GRADE_1, m_pDummyItem->sBasicOp.wGradeAttack );
	SetWin_Num_int ( this, IDC_EDIT_GRADE_2, m_pDummyItem->sBasicOp.wGradeDefense );

	// 경험치 배수
	SetWin_Num_float ( this, IDC_EDIT_X_EXP, m_pDummyItem->sBasicOp.fExpMultiple );

	
	//	상태이상;
	BOOL bSTATE_BLOW = ( m_pDummyItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE );
	BOOL bBLOW_VAR1(TRUE), bBLOW_VAR2(TRUE);
	char* szBLOW_VAR1, *szBLOW_VAR2;	szBLOW_VAR1 = szBLOW_VAR2 = NULL;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		// 착용타입;
		SetWin_Text(this, IDC_EDIT_SUIT, CGameTextMan::GetInstance().GetCommentText("ITEMSUIT", m_pDummyItem->sSuitOp.emSuit));
		//	공격 타입;
		SetWin_Text(this, IDC_EDIT_ATT_TYPE, CGameTextMan::GetInstance().GetCommentText("ITEMATTACK", m_pDummyItem->sSuitOp.emAttack));
		//	소모품 설정;
		SetWin_Text (this, IDC_EDIT_DRUG_TYPE, CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", m_pDummyItem->sDrugOp.emDrug));	
		SetWin_Text (this, IDC_STATIC_DRUG_VAR, CGameTextMan::GetInstance().GetCommentText("ITEMDRUG_VAR", m_pDummyItem->sDrugOp.emDrug));
		//	추가 옵션;
		SetWin_Text(this, IDC_EDIT_ADDON_TYPE1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", m_pDummyItem->sSuitOp.sADDON[0].emTYPE));
		SetWin_Text(this, IDC_EDIT_ADDON_TYPE2, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", m_pDummyItem->sSuitOp.sADDON[1].emTYPE));
		SetWin_Text(this, IDC_EDIT_ADDON_TYPE3, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", m_pDummyItem->sSuitOp.sADDON[2].emTYPE));
		SetWin_Text(this, IDC_EDIT_ADDON_TYPE4, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", m_pDummyItem->sSuitOp.sADDON[3].emTYPE));
		SetWin_Text(this, IDC_EDIT_ADDON_TYPE5, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", m_pDummyItem->sSuitOp.sADDON[4].emTYPE));
		SetWin_Text(this, IDC_EDIT_ADDON_TYPE6, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", m_pDummyItem->sSuitOp.sADDON[5].emTYPE));
		
		//	상태이상;
		szBLOW_VAR1 = const_cast<char*>(CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1", m_pDummyItem->sSuitOp.sBLOW.emTYPE).GetString());
		szBLOW_VAR2 = const_cast<char*>(CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2", m_pDummyItem->sSuitOp.sBLOW.emTYPE).GetString());
		if ( !strcmp(szBLOW_VAR1, CGameTextMan::GetInstance().GetCommentText("BLOW",0)) )		bBLOW_VAR1 = FALSE;
		if ( !strcmp(szBLOW_VAR2, CGameTextMan::GetInstance().GetCommentText("BLOW",0)) )		bBLOW_VAR2 = FALSE;
	
		SetWin_Text ( this, IDC_EDIT_BLOW_TYPE, CGameTextMan::GetInstance().GetCommentText("BLOW",m_pDummyItem->sSuitOp.sBLOW.emTYPE));
		SetWin_Text ( this, IDC_EDIT_QUESTION_TYPE, CGameTextMan::GetInstance().GetCommentText("ITEM_QUE_TYPE",m_pDummyItem->sQuestionItem.emType));
		SetWin_Text ( this, IDC_STATIC_QUEVAR1, CGameTextMan::GetInstance().GetCommentText("ITEM_QUE_VAR1",m_pDummyItem->sQuestionItem.emType));
		SetWin_Text ( this, IDC_STATIC_QUEVAR2, CGameTextMan::GetInstance().GetCommentText("ITEM_QUE_VAR2",m_pDummyItem->sQuestionItem.emType));
		SetWin_Combo_Sel ( this, IDC_COMBO_GRINDING_TYPE, atoi(CGameTextMan::GetInstance().GetCommentText("GRINDING_TYPE",m_pDummyItem->sGrindingOp.emTYPE).GetString()));
		SetWin_Combo_Sel ( this, IDC_COMBO_GRINDING_LEVEL, atoi(CGameTextMan::GetInstance().GetCommentText("GRINDING_LEVEL",m_pDummyItem->sGrindingOp.emGRINDER_TYPE).GetString()));

		std::string strBIKET[EMBIKE_NSIZE], strBOARDT[EMBIKE_NSIZE], strCART[EMBIKE_NSIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("BIKETYPE", strBIKET, CGameTextMan::EM_COMMENT_TEXT, EMBIKE_NSIZE );
		CGameTextMan::GetInstance().LoadHelpCommentString("BOARDTYPE",strBOARDT, CGameTextMan::EM_COMMENT_TEXT, EMBIKE_NSIZE );
		CGameTextMan::GetInstance().LoadHelpCommentString("CARTYPE", strCART, CGameTextMan::EM_COMMENT_TEXT, EMBIKE_NSIZE );
		switch (m_pDummyItem->sVehicle.emVehicleType)
		{
		case VEHICLE_TYPE_BIKE:		SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, strBIKET, EMBIKE_NSIZE );	break;
		case VEHICLE_TYPE_CAR:		SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, strBOARDT, EMBIKE_NSIZE );	break;
		case VEHICLE_TYPE_REARCAR:	SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, strCART, EMBIKE_NSIZE ); break;
		default:					SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, strBOARDT, EMBIKE_NSIZE );	break;
		}
	}
	else
	{
		//	착용 타입
		SetWin_Text( this, IDC_EDIT_SUIT, COMMENT::ITEMSUIT[m_pDummyItem->sSuitOp.emSuit].c_str() );
		//	공격 타입;
		SetWin_Text( this, IDC_EDIT_ATT_TYPE, COMMENT::ITEMATTACK[m_pDummyItem->sSuitOp.emAttack].c_str() );
		//	소모품 설정;
		SetWin_Text ( this, IDC_EDIT_DRUG_TYPE, COMMENT::ITEMDRUG[m_pDummyItem->sDrugOp.emDrug].c_str() );	
		SetWin_Text ( this, IDC_STATIC_DRUG_VAR, COMMENT::ITEMDRUG_VAR[m_pDummyItem->sDrugOp.emDrug].c_str() );
		//	추가 옵션;
		SetWin_Text( this, IDC_EDIT_ADDON_TYPE1, COMMENT::ITEMADDON[m_pDummyItem->sSuitOp.sADDON[0].emTYPE].c_str() );
		SetWin_Text( this, IDC_EDIT_ADDON_TYPE2, COMMENT::ITEMADDON[m_pDummyItem->sSuitOp.sADDON[1].emTYPE].c_str() );
		SetWin_Text( this, IDC_EDIT_ADDON_TYPE3, COMMENT::ITEMADDON[m_pDummyItem->sSuitOp.sADDON[2].emTYPE].c_str() );
		SetWin_Text( this, IDC_EDIT_ADDON_TYPE4, COMMENT::ITEMADDON[m_pDummyItem->sSuitOp.sADDON[3].emTYPE].c_str() );
		SetWin_Text( this, IDC_EDIT_ADDON_TYPE5, COMMENT::ITEMADDON[m_pDummyItem->sSuitOp.sADDON[4].emTYPE].c_str() );
		SetWin_Text( this, IDC_EDIT_ADDON_TYPE6, COMMENT::ITEMADDON[m_pDummyItem->sSuitOp.sADDON[5].emTYPE].c_str() );
		
		//	상태이상;
		szBLOW_VAR1 = const_cast<char*>(COMMENT::BLOW_VAR1[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str());
		szBLOW_VAR2 = const_cast<char*>(COMMENT::BLOW_VAR2[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str());
		if ( !strcmp(szBLOW_VAR1,COMMENT::BLOW_VAR1[EMBLOW_NONE].c_str()) )		bBLOW_VAR1 = FALSE;
		if ( !strcmp(szBLOW_VAR2,COMMENT::BLOW_VAR2[EMBLOW_NONE].c_str()) )		bBLOW_VAR2 = FALSE;

		SetWin_Text ( this, IDC_EDIT_BLOW_TYPE, COMMENT::BLOW[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str() );
		SetWin_Text( this, IDC_EDIT_QUESTION_TYPE, COMMENT::ITEM_QUE_TYPE[m_pDummyItem->sQuestionItem.emType].c_str() );
		SetWin_Text ( this, IDC_STATIC_QUEVAR1, COMMENT::ITEM_QUE_VAR1[m_pDummyItem->sQuestionItem.emType].c_str() );
		SetWin_Text ( this, IDC_STATIC_QUEVAR2, COMMENT::ITEM_QUE_VAR2[m_pDummyItem->sQuestionItem.emType].c_str() );

		SetWin_Combo_Sel ( this, IDC_COMBO_GRINDING_TYPE, COMMENT::GRINDING_TYPE[m_pDummyItem->sGrindingOp.emTYPE] );
		SetWin_Combo_Sel ( this, IDC_COMBO_GRINDING_LEVEL, COMMENT::GRINDING_LEVEL[m_pDummyItem->sGrindingOp.emGRINDER_TYPE] );

		switch (m_pDummyItem->sVehicle.emVehicleType)
		{
		case VEHICLE_TYPE_BIKE:		SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, COMMENT::BIKETYPE, EMBIKE_NSIZE );	break;
		case VEHICLE_TYPE_CAR:		SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, COMMENT::CARTYPE, EMBIKE_NSIZE );	break;
		case VEHICLE_TYPE_REARCAR:	SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, COMMENT::REARCARTYPE, EMBIKE_NSIZE ); break;
		default:					SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, COMMENT::BOARDTYPE, EMBIKE_NSIZE );	break;
		}

		SetWin_Combo_Init( this, IDC_SKILL_TARGET, COMMENT::SKILL_LINK_TYPE, EMITEM_SKILL_SIZE );

	}
	SetWin_Text ( this, IDC_STATIC_BLOWVAR1, szBLOW_VAR1 );
	SetWin_Text ( this, IDC_STATIC_BLOWVAR2, szBLOW_VAR2 );
	SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE1, bBLOW_VAR1 );
	SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE2, bBLOW_VAR2 );

	SetWin_Num_int ( this, IDC_EDIT_INSTANCE_VOLUME, m_pDummyItem->sDrugOp.dwCureVolume );
	SetWin_Num_int ( this, IDC_EDIT_INSTANCE_NUM, m_pDummyItem->sDrugOp.wPileNum );

	CTimeSpan cSPAN(m_pDummyItem->sDrugOp.tDuration);
	//SetWin_Num_int ( this, IDC_EDIT_COSTUME_TIME, m_pDummyItem->sDrugOp.tDuration );

	CWnd *pWnd = GetDlgItem(IDC_EDIT_COSTUME_TIME);

	switch( m_pDummyItem->sDrugOp.emDrug )
	{
	case ITEM_DRUG_TEMP_ENCHANT :
	case ITEM_DRUG_COSTUME_STATS : 
		{
			SetWin_Num_int ( this, IDC_EDIT_COSTUME_TIME, (int)cSPAN.GetTotalSeconds() ); 
			
			if( pWnd ) pWnd->EnableWindow(TRUE); 
		}
		break;
	default :
		{
			SetWin_Num_int ( this, IDC_EDIT_COSTUME_TIME, 0 ); 

			if( pWnd ) pWnd->EnableWindow(FALSE); 
		}
		break;
	}

	CWnd *pWnd1 = GetDlgItem(IDC_BUTTON_ER);

	if( m_pDummyItem->sDrugOp.emDrug == ITEM_DRUG_EFFECT_RELEASE )
	{
		if( pWnd1 ) pWnd1->EnableWindow(TRUE);
	}
	else
	{
		if( pWnd1 ) pWnd1->EnableWindow(FALSE);
	}
		
	SetWin_Check ( this, IDC_CHECK_CURE_RATIO, m_pDummyItem->sDrugOp.bRatio );

	SetWin_Check ( this, IDC_CHECK_PALSY, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_NUMB );
	SetWin_Check ( this, IDC_CHECK_SWOON, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_STUN );
	SetWin_Check ( this, IDC_CHECK_PETRIFY, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_STONE );
	SetWin_Check ( this, IDC_CHECK_BURN, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_BURN );
	SetWin_Check ( this, IDC_CHECK_FROZEN, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_FROZEN );

	SetWin_Check ( this, IDC_CHECK_DERANGE, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_MAD );
	SetWin_Check ( this, IDC_CHECK_POISON, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_POISON );
	SetWin_Check ( this, IDC_CHECK_CURSE, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_CURSE );

	//	저항치
	SetWin_Num_int ( this, IDC_EDIT_SPIRIT, m_pDummyItem->sSuitOp.sResist.nSpirit );
	SetWin_Num_int ( this, IDC_EDIT_FIRE, m_pDummyItem->sSuitOp.sResist.nFire );
	SetWin_Num_int ( this, IDC_EDIT_ELECTRIC, m_pDummyItem->sSuitOp.sResist.nElectric );
	SetWin_Num_int ( this, IDC_EDIT_ICE, m_pDummyItem->sSuitOp.sResist.nIce );
	SetWin_Num_int ( this, IDC_EDIT_POISON, m_pDummyItem->sSuitOp.sResist.nPoison );

	INT nAddonID[ITEM::SSUIT::ADDON_SIZE] = {IDC_EDIT_ADDON_VAR1
		, IDC_EDIT_ADDON_VAR2
		, IDC_EDIT_ADDON_VAR3
		, IDC_EDIT_ADDON_VAR4
		, IDC_EDIT_ADDON_VAR5
		, IDC_EDIT_ADDON_VAR6 };
	for ( INT i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
	{
		const DWORD dwType = static_cast< const DWORD >( m_pDummyItem->sSuitOp.sADDON[i].emTYPE );
		const int nValue = m_pDummyItem->sSuitOp.GetAddonValue_CompareExchange< const int >( i, ITEM::bAddonValueInteger[dwType] );
		const float fValue = m_pDummyItem->sSuitOp.GetAddonValue_CompareExchange< const float >( i, ITEM::bAddonValueInteger[dwType] );

		if ( ITEM::bAddonValueInteger[dwType] )
			SetWin_Num_int ( this, nAddonID[i], nValue );
		else
			SetWin_Num_float ( this, nAddonID[i], fValue );
	}
	SetWin_Text( this, IDC_EDIT_ADDON_RANDOMSCRIPT, m_pDummyItem->sSuitOp.szRandomAddonScript );

	SetWin_Enable ( this, IDC_EDIT_BLOW_LIFE, bSTATE_BLOW );
	SetWin_Enable ( this, IDC_EDIT_BLOW_RATE, bSTATE_BLOW );	
	
	SetWin_Num_float ( this, IDC_EDIT_BLOW_RATE, m_pDummyItem->sSuitOp.sBLOW.fRATE );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_LIFE, m_pDummyItem->sSuitOp.sBLOW.fLIFE );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE1, m_pDummyItem->sSuitOp.sBLOW.fVAR1 );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE2, m_pDummyItem->sSuitOp.sBLOW.fVAR2 );

	// Question Item by 경대
	BOOL bTIME(false);
	BOOL bEXP( m_pDummyItem->sQuestionItem.emType==QUESTION_EXP_GET );
	BOOL bPARAM1(false);
	BOOL bPARAM2(false);

	switch( m_pDummyItem->sQuestionItem.emType )
	{
	case QUESTION_SPEED_UP:
	case QUESTION_ATTACK_UP:
	case QUESTION_EXP_UP:
	case QUESTION_LUCKY:
	case QUESTION_SPEED_UP_M:
	case QUESTION_ATTACK_UP_M:
		bPARAM1 = bTIME = true;
		break;

	case QUESTION_CRAZY:
	case QUESTION_MADNESS:
		bTIME = bPARAM1 = bPARAM2 = true;
		break;

	case QUESTION_BOMB:
	case QUESTION_HEAL:
		bPARAM1 = true;
		break;

	case QUESTION_MOBGEN:
		bPARAM1 = bPARAM2 = true;
		break;
	};

	SetWin_Enable( this, IDC_EDIT_QUE_TIME, bTIME );
	SetWin_Enable( this, IDC_EDIT_QUE_EXP, bEXP );
	SetWin_Enable( this, IDC_EDIT_QUE_PARAM1, bPARAM1 );
	SetWin_Enable( this, IDC_EDIT_QUE_PARAM2, bPARAM2 );

	SetWin_Num_float( this, IDC_EDIT_QUE_TIME, m_pDummyItem->sQuestionItem.fTime );
	SetWin_Num_float( this, IDC_EDIT_QUE_EXP, m_pDummyItem->sQuestionItem.fExp );
	SetWin_Num_int( this, IDC_EDIT_QUE_PARAM1, static_cast<int>( m_pDummyItem->sQuestionItem.wParam1 ) );
	SetWin_Num_int( this, IDC_EDIT_QUE_PARAM2, static_cast<int>( m_pDummyItem->sQuestionItem.wParam2 ) );
	//////////////////////////////////////////////////////////////////////////////////////////////

	SetWin_Check ( this, IDC_RADIO_GRIND_ARM, m_pDummyItem->sGrindingOp.emCLASS==EMGRINDING_CLASS_ARM );
	SetWin_Check ( this, IDC_RADIO_GRIND_CLOTH, m_pDummyItem->sGrindingOp.emCLASS==EMGRINDING_CLASS_CLOTH );

	// PET
	SetWin_Combo_Sel ( this, IDC_COMBO_PET_TYPE, (int)m_pDummyItem->sPet.emPetType );
	SetWin_Num_int ( this, IDC_EDIT_PET_MID, m_pDummyItem->sPet.sPetID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_PET_SID, m_pDummyItem->sPet.sPetID.wSubID );

	// 탈것
	SetWin_Combo_Sel ( this, IDC_COMBO_VEHICLE_TYPE, (int)m_pDummyItem->sVehicle.emVehicleType );
	SetWin_Combo_Sel ( this, IDC_COMBO_PARTS_TYPE, (int)m_pDummyItem->sVehicle.emPartsType );
	SetWin_Check ( this, IDC_CHECK_NOT_USE_PARTS, m_pDummyItem->sVehicle.bNotUseParts );

	SetWin_Combo_Sel ( this, IDC_SKILL_TARGET, (int)m_pDummyItem->sItemSkill.emSkillTarget );
	SetWin_Num_float( this, IDC_SKILL_RATE, m_pDummyItem->sItemSkill.fOccuRate );
	SetWin_Num_int ( this, IDC_SKILL_MID, m_pDummyItem->sItemSkill.sSkillID.wMainID );
	SetWin_Num_int ( this, IDC_SKILL_SID, m_pDummyItem->sItemSkill.sSkillID.wSubID  );
	SetWin_Num_int ( this, IDC_SKILL_LV, m_pDummyItem->sItemSkill.wSkillLv  );

	SetWin_Num_int ( this, IDC_EDIT_LINKSKILL_RSET_INDEX, m_pDummyItem->sItemSkill.nLinkSkillIndex );

	TypeOfUpdateItems();

	SetReadMode();
}
bool CItemSuit::InverseUpdateItems ()
{
	// 코스튬 염색 가능. by luxes.
	m_pDummyItem->sSuitOp.SETSTATE(EMITEM_SUIT_CHANGE_COLOR, GetWin_Check( this, IDC_CHECK_ENABLE_COSTUM ));

	// 고유
	m_pDummyItem->sSuitOp.SETSTATE(EMITEM_SUIT_UNIQUE, GetWin_Check( this, IDC_CHECK_ENABLE_UNIQUE ));

	//	손 착용 설정
	GetWin_Check ( this, IDC_CHECK_TWOH, m_pDummyItem->sSuitOp.dwHAND, ITEM::EMHAND_BOTHHAND );
	GetWin_Check ( this, IDC_CHECK_BIGH, m_pDummyItem->sSuitOp.dwHAND, ITEM::EMHAND_BIG );
	GetWin_Check ( this, IDC_CHECK_BROOM, m_pDummyItem->sSuitOp.dwHAND, ITEM::EMHAND_BROOM );

	if ( GetWin_Check ( this, IDC_RADIO_RIGHT ) )	m_pDummyItem->sSuitOp.emHand = HAND_RIGHT;
	if ( GetWin_Check ( this, IDC_RADIO_LEFT ) )	m_pDummyItem->sSuitOp.emHand = HAND_LEFT;
	if ( GetWin_Check ( this, IDC_RADIO_BOTH ) )	m_pDummyItem->sSuitOp.emHand = HAND_BOTH;

	//	공격 거리
	m_pDummyItem->sSuitOp.wAttRange = GetWin_Num_int ( this, IDC_EDIT_ATT_RANGE );

	m_pDummyItem->sSuitOp.wReqSP = GetWin_Num_int ( this, IDC_EDIT_REQSP );

	//	기본 옵션
	m_pDummyItem->sSuitOp.nHitRate = GetWin_Num_int ( this, IDC_EDIT_HIT );
	m_pDummyItem->sSuitOp.nAvoidRate = GetWin_Num_int ( this, IDC_EDIT_AVOIDRATE );

	//	공격 방어
	m_pDummyItem->sSuitOp.gdDamage.wHigh = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_HIGH );
	m_pDummyItem->sSuitOp.gdDamage.wLow = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_LOW );
	m_pDummyItem->sSuitOp.nDefense = GetWin_Num_int ( this, IDC_EDIT_DEFENSE );

	//	공격 방어 랜덤세트 매니저 인덱스
	m_pDummyItem->sSuitOp.nBasicAttackDamageType = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_RSET_TYPE );
	m_pDummyItem->sSuitOp.nBasicAttackDamageIndex = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_RSET_INDEX );
	m_pDummyItem->sSuitOp.nBasicDefenceType = GetWin_Num_int ( this, IDC_EDIT_DEFENSE_RSET_TYPE );
	m_pDummyItem->sSuitOp.nBasicDefenceIndex = GetWin_Num_int ( this, IDC_EDIT_DEFENSE_RSET_INDEX );

	//	공격 방어 개조가능 여부
	m_pDummyItem->sSuitOp.bEditableBasicAttackDamage = ( GetWin_Check( this, IDC_CHECK_DAMAGE_EDITABLE ) == TRUE ) ? true : false;
	m_pDummyItem->sSuitOp.bEditableBasicDefence = ( GetWin_Check( this, IDC_CHECK_DEFENSE_EDITABLE ) == TRUE ) ? true : false;

	// 업그레이드 수치
	m_pDummyItem->sBasicOp.wGradeAttack = GetWin_Num_int( this, IDC_EDIT_GRADE_1 );
	m_pDummyItem->sBasicOp.wGradeDefense = GetWin_Num_int( this, IDC_EDIT_GRADE_2 );

	// 경험치 배수
	m_pDummyItem->sBasicOp.fExpMultiple = GetWin_Num_float( this, IDC_EDIT_X_EXP );

	//	소모품 설정
	if(m_pDummyItem->sDrugOp.emDrug == ITEM_DRUG_EXP_CAPSULE_TYPE1 || m_pDummyItem->sDrugOp.emDrug == ITEM_DRUG_EXP_CAPSULE_TYPE2 )
	{
		if( GetWin_Num_int ( this, IDC_EDIT_INSTANCE_NUM ) != 1 )
		{
			MessageBox ( "경험치 캡슐은 겹침을 사용할 수 없습니다. (최대겹침을 1로 셋팅)", "Caution", MB_OK | MB_ICONEXCLAMATION );
			return false;
		}
	}

	if(m_pDummyItem->sDrugOp.emDrug == ITEM_DRUG_EXP_COMPERSSOR_TYPE1 || m_pDummyItem->sDrugOp.emDrug == ITEM_DRUG_EXP_COMPERSSOR_TYPE2 )
	{
		if ( m_pDummyItem->sBox.vecItems.size() <= 0 )
		{
			MessageBox ( "경험치 압축 아이템은 경험치 캡슐을 Box아이템으로 지정해야 합니다.", "Caution", MB_OK | MB_ICONEXCLAMATION );
			return false;
		}
	}

	m_pDummyItem->sDrugOp.wPileNum = GetWin_Num_int ( this, IDC_EDIT_INSTANCE_NUM );
	m_pDummyItem->sDrugOp.dwCureVolume = GetWin_Num_int ( this, IDC_EDIT_INSTANCE_VOLUME );

	CTimeSpan cSPAN(0,0,0,GetWin_Num_int ( this, IDC_EDIT_COSTUME_TIME ));

	if(m_pDummyItem->sDrugOp.emDrug == ITEM_DRUG_COSTUME_STATS || m_pDummyItem->sDrugOp.emDrug == ITEM_DRUG_TEMP_ENCHANT )
		m_pDummyItem->sDrugOp.tDuration = cSPAN.GetTimeSpan();
	else
		m_pDummyItem->sDrugOp.tDuration = 0;

	//m_pDummyItem->sDrugOp.tDuration = GetWin_Num_int ( this, IDC_EDIT_COSTUME_TIME );

	m_pDummyItem->sDrugOp.bRatio = GetWin_Check ( this, IDC_CHECK_CURE_RATIO );

	DWORD &dwCURE = m_pDummyItem->sDrugOp.dwCureDISORDER;
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_PALSY ), dwCURE, DIS_NUMB );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SWOON ), dwCURE, DIS_STUN );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_PETRIFY ), dwCURE, DIS_STONE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_BURN ), dwCURE, DIS_BURN );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_FROZEN ), dwCURE, DIS_FROZEN );
	
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_DERANGE ), dwCURE, DIS_MAD );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_POISON ), dwCURE, DIS_POISON );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CURSE ), dwCURE, DIS_CURSE );

	//	저항치
	m_pDummyItem->sSuitOp.sResist.nSpirit = GetWin_Num_int ( this, IDC_EDIT_SPIRIT );
	m_pDummyItem->sSuitOp.sResist.nFire = GetWin_Num_int ( this, IDC_EDIT_FIRE );
	m_pDummyItem->sSuitOp.sResist.nElectric = GetWin_Num_int ( this, IDC_EDIT_ELECTRIC );
	m_pDummyItem->sSuitOp.sResist.nIce = GetWin_Num_int ( this, IDC_EDIT_ICE );
	m_pDummyItem->sSuitOp.sResist.nPoison = GetWin_Num_int ( this, IDC_EDIT_POISON );

	//	추가 옵션
	INT nAddonID[ITEM::SSUIT::ADDON_SIZE] = {IDC_EDIT_ADDON_VAR1
		, IDC_EDIT_ADDON_VAR2
		, IDC_EDIT_ADDON_VAR3
		, IDC_EDIT_ADDON_VAR4
		, IDC_EDIT_ADDON_VAR5
		, IDC_EDIT_ADDON_VAR6 };
	for ( INT i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
	{
		const DWORD dwType = static_cast< const DWORD >( m_pDummyItem->sSuitOp.sADDON[i].emTYPE );
		if ( ITEM::bAddonValueInteger[dwType] )
			m_pDummyItem->sSuitOp.sADDON[i].nVALUE = GetWin_Num_int( this, nAddonID[i] );
		else
			m_pDummyItem->sSuitOp.sADDON[i].fVALUE = GetWin_Num_float( this, nAddonID[i] );
	}

	StringCchCopy ( m_pDummyItem->sSuitOp.szRandomAddonScript, ITEM::SSUIT::STRING_LENGTH, GetWin_Text(this,IDC_EDIT_ADDON_RANDOMSCRIPT).GetString() );

	//	상태이상
	m_pDummyItem->sSuitOp.sBLOW.fRATE = GetWin_Num_float ( this, IDC_EDIT_BLOW_RATE );
	m_pDummyItem->sSuitOp.sBLOW.fLIFE = GetWin_Num_float ( this, IDC_EDIT_BLOW_LIFE );
	m_pDummyItem->sSuitOp.sBLOW.fVAR1 = GetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE1 );
	m_pDummyItem->sSuitOp.sBLOW.fVAR2 = GetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE2 );

	//	Question Item by 경대
	m_pDummyItem->sQuestionItem.fTime	= GetWin_Num_float ( this, IDC_EDIT_QUE_TIME );
	m_pDummyItem->sQuestionItem.fExp	= GetWin_Num_float ( this, IDC_EDIT_QUE_EXP );
	m_pDummyItem->sQuestionItem.wParam1	= static_cast<WORD>( GetWin_Num_int ( this, IDC_EDIT_QUE_PARAM1 ) );
	m_pDummyItem->sQuestionItem.wParam2	= static_cast<WORD>( GetWin_Num_int ( this, IDC_EDIT_QUE_PARAM2 ) );

	ITEM::SSATE_BLOW &sBLOW = m_pDummyItem->sSuitOp.sBLOW;
	switch ( sBLOW.emTYPE )
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

	if ( GetWin_Check ( this, IDC_RADIO_GRIND_ARM ) )	m_pDummyItem->sGrindingOp.emCLASS = EMGRINDING_CLASS_ARM;
	if ( GetWin_Check ( this, IDC_RADIO_GRIND_CLOTH ) )	m_pDummyItem->sGrindingOp.emCLASS = EMGRINDING_CLASS_CLOTH;

	m_pDummyItem->sGrindingOp.emTYPE = (EMGRINDING_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_GRINDING_TYPE );
	m_pDummyItem->sGrindingOp.emGRINDER_TYPE = (EMGRINDER_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_GRINDING_LEVEL );

	// PET
	m_pDummyItem->sPet.sPetID.wMainID = GetWin_Num_int ( this, IDC_EDIT_PET_MID );
	m_pDummyItem->sPet.sPetID.wSubID = GetWin_Num_int ( this, IDC_EDIT_PET_SID );

	CComboBox *pCombo = (CComboBox*) ( this->GetDlgItem(IDC_COMBO_PET_TYPE) );
	if( !pCombo )
	{
		return false;
	}
	if (  pCombo->GetCurSel () == PETTYPE_NONE )
	{
		MessageBox ( "An usable PET TYPE is not selected.", "Caution", MB_OK | MB_ICONEXCLAMATION );
		return false;
	}
	else
	{
		m_pDummyItem->sPet.emPetType = (int) GetWin_Combo_Sel ( this, IDC_COMBO_PET_TYPE );
	}	

	// 탈것 타입
	m_pDummyItem->sVehicle.emVehicleType = (VEHICLE_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_VEHICLE_TYPE );

	m_pDummyItem->sVehicle.emPartsType = (EMPARTS_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_PARTS_TYPE );
	if ( GetWin_Check ( this, IDC_CHECK_NOT_USE_PARTS ) )
	{
		m_pDummyItem->sVehicle.bNotUseParts = TRUE;
	}
	else
	{
		m_pDummyItem->sVehicle.bNotUseParts = FALSE;
	}
		
	m_pDummyItem->sItemSkill.emSkillTarget    = (EMITEM_SKILL_LINK)GetWin_Combo_Sel ( this, IDC_SKILL_TARGET );
	m_pDummyItem->sItemSkill.fOccuRate		  = GetWin_Num_float ( this, IDC_SKILL_RATE );
	m_pDummyItem->sItemSkill.sSkillID.wMainID = GetWin_Num_int ( this, IDC_SKILL_MID );
	m_pDummyItem->sItemSkill.sSkillID.wSubID  = GetWin_Num_int ( this, IDC_SKILL_SID );
	m_pDummyItem->sItemSkill.wSkillLv		  = GetWin_Num_int ( this, IDC_SKILL_LV );

	m_pDummyItem->sItemSkill.nLinkSkillIndex	= GetWin_Num_int ( this, IDC_EDIT_LINKSKILL_RSET_INDEX );
	
	if( m_pDummyItem->sBasicOp.emItemType != ITEM_SUIT )
	{
		m_pDummyItem->sItemSkill.reset();
	}

	if ( !TypeOfInverseUpdateItems() ) return false;

	return true;
}


void CItemSuit::TypeOfUpdateItems ()
{
	m_listBOX.ShowWindow( SW_HIDE );
	m_listBox_Rand.ShowWindow( SW_HIDE );
	
	SetWin_ShowWindow( this, IDC_STATIC_PRESENT, SW_HIDE ); 
	SetWin_ShowWindow( this, IDC_STATIC_RANDOM, SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_BOX_UP1, SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_BOX_DW1, SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_BOX_UP2, SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_BOX_DW2, SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_BOX_ADD, SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_BOX_DEL, SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_BOX_EDIT, SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_BOX_ADD2, SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_BOX_DEL2, SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_BOX_EDIT2, SW_HIDE );

	m_list_Mob.ShowWindow( SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_ADD_PETSKIN, SW_HIDE );
	SetWin_ShowWindow( this, IDC_BUTTON_DEL_PETSKIN, SW_HIDE );
	SetWin_ShowWindow( this, IDC_STATIC_PETSKINPACK, SW_HIDE );
	SetWin_ShowWindow( this, IDC_STATIC_MOB_SCALE, SW_HIDE );
	SetWin_ShowWindow( this, IDC_EDIT_MOB_SCALE, SW_HIDE );
	SetWin_ShowWindow( this, IDC_STATIC_MOB_SKINTIME, SW_HIDE );
	SetWin_ShowWindow( this, IDC_EDIT_MOB_SKINTIME, SW_HIDE );

	SetWin_ShowWindow( this, IDC_SKILL_GROUP  , SW_HIDE );
	SetWin_ShowWindow( this, IDC_STATIC_TARGET, SW_HIDE );
	SetWin_ShowWindow( this, IDC_SKILL_TARGET , SW_HIDE );
	SetWin_ShowWindow( this, IDC_STATIC_RATE  , SW_HIDE );
	SetWin_ShowWindow( this, IDC_SKILL_RATE   , SW_HIDE );
	SetWin_ShowWindow( this, IDC_STATIC_MSID  , SW_HIDE );
	SetWin_ShowWindow( this, IDC_SKILL_MID    , SW_HIDE );
	SetWin_ShowWindow( this, IDC_SKILL_SID    , SW_HIDE );
	SetWin_ShowWindow( this, IDC_STATIC_LV    , SW_HIDE );
	SetWin_ShowWindow( this, IDC_SKILL_LV     , SW_HIDE );
	SetWin_ShowWindow( this, IDC_EDIT_LINKSKILL_RSET_INDEX, SW_HIDE );


	if( m_pDummyItem->sBasicOp.emItemType == ITEM_BOX ||
		m_pDummyItem->sBasicOp.emItemType == ITEM_PREMIUMSET || 
		m_pDummyItem->sBasicOp.emItemType == ITEM_LOCK_BOX ||
		m_pDummyItem->sBasicOp.emItemType == ITEM_SELECTIVEFORM_BOX ||
		m_pDummyItem->sBasicOp.emItemType == ITEM_EXP_COMPERSSOR ) 
	{
		m_listBOX.ShowWindow( SW_SHOW );
		
		SetWin_ShowWindow( this, IDC_STATIC_PRESENT, SW_SHOW ); 
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_UP1, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DW1, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_ADD, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DEL, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_EDIT, SW_SHOW );

		InitBox ();
		
	}
	if( m_pDummyItem->sBasicOp.emItemType == ITEM_RANDOMITEM ||
		m_pDummyItem->sBasicOp.emItemType == ITEM_LOCK_BOX )
	{
		m_listBox_Rand.ShowWindow( SW_SHOW );
		
		SetWin_ShowWindow( this, IDC_STATIC_RANDOM, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_UP2, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DW2, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_ADD2, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_EDIT2, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DEL2, SW_SHOW );
		
		InitBox_Rand();
	}
    else if( m_pDummyItem->sBasicOp.emItemType == ITEM_PET_SKIN_PACK )
	{

		m_list_Mob.ShowWindow( SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_ADD_PETSKIN, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_DEL_PETSKIN, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_PETSKINPACK, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_MOB_SCALE, SW_SHOW );
		SetWin_ShowWindow( this, IDC_EDIT_MOB_SCALE, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_MOB_SKINTIME, SW_SHOW );
		SetWin_ShowWindow( this, IDC_EDIT_MOB_SKINTIME, SW_SHOW );

		SetWin_Num_int( this, IDC_EDIT_MOB_SKINTIME, m_pDummyItem->sPetSkinPack.dwPetSkinTime );

		InitPetSkinPack();
	}
	
	if( m_pDummyItem->sBasicOp.emItemType == ITEM_SUIT )
	{
		SetWin_ShowWindow( this, IDC_SKILL_GROUP  , SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_TARGET, SW_SHOW );
		SetWin_ShowWindow( this, IDC_SKILL_TARGET , SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_RATE  , SW_SHOW );
		SetWin_ShowWindow( this, IDC_SKILL_RATE   , SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_MSID , SW_SHOW );
		SetWin_ShowWindow( this, IDC_SKILL_MID   , SW_SHOW );
		SetWin_ShowWindow( this, IDC_SKILL_SID   , SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_LV    , SW_SHOW );
		SetWin_ShowWindow( this, IDC_SKILL_LV   , SW_SHOW );
		SetWin_ShowWindow( this, IDC_EDIT_LINKSKILL_RSET_INDEX, SW_SHOW );
	}

	this->ListBoxNameUpdate();
}

bool CItemSuit::TypeOfInverseUpdateItems ()
{
	if( m_pDummyItem->sBasicOp.emItemType == ITEM_PET_SKIN_PACK )
	{
		if( !m_pDummyItem->sPetSkinPack.VALID() )
		{
			MessageBox( "Pet Skin Pack : Select Monster", "Error" );
			return false;
		}

		float fPercent = 0.0f;
		for( int i = 0; i < static_cast<int>( m_pDummyItem->sPetSkinPack.vecPetSkinData.size() ); i++ )
		{
			fPercent += m_pDummyItem->sPetSkinPack.vecPetSkinData[i].fRate;
		}

		if( fPercent != 100.0f )
		{
			MessageBox( "Pet Skin Pack : Rate is Not 100%", "Error" );
			return false;
		}

		m_pDummyItem->sPetSkinPack.dwPetSkinTime = (DWORD)GetWin_Num_int ( this, IDC_EDIT_MOB_SKINTIME );

		if( m_pDummyItem->sPetSkinPack.dwPetSkinTime == 0 )
		{
			MessageBox( "Pet Skin Pack : PetSkinTime is 0", "Error" );
			return false;
		}
	}

	return true;
}

void CItemSuit::OnBnClickedButtonPrev()
{
	//// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//if( !InverseUpdateItems() ) return;

	//CItemNode sItemNode;
	//sItemNode.m_sItem = *m_pDummyItem;

	//GLItemMan::GetInstance().InsertItem ( m_pDummyItem->sBasicOp.sNativeID, &sItemNode );

	m_pSheetTab->ActiveItemParentPage ( m_CallPage, m_pItem );
	m_pItem = NULL;
}

void CItemSuit::InitDefaultCtrls ()
{
	GetDlgItem ( IDC_BUTTON_NEXT )->EnableWindow ( FALSE );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string	strGRIDINGTYPE[EMGRINDING_NSIZE], strGRIDINGLEVEL[EMGRINDER_SIZE], strVEHICLETYPE[VEHICLE_TYPE_SIZE], strBIKETPYE[EMBIKE_SIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("GRINDING_TYPE", strGRIDINGTYPE, CGameTextMan::EM_COMMENT_TEXT, EMGRINDING_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("GRINDING_LEVEL", strGRIDINGLEVEL, CGameTextMan::EM_COMMENT_TEXT, EMGRINDER_SIZE);
		SetWin_Combo_Init ( this, IDC_COMBO_GRINDING_TYPE, strGRIDINGTYPE, EMGRINDING_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_GRINDING_LEVEL, strGRIDINGLEVEL, EMGRINDER_SIZE );	
		// PET;
		std::vector<std::string> vecPETTYPE;
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString( "PET_DEFINE", vecPETTYPE, CGameTextMan::EM_COMMENT_TEXT);
		SetWin_Combo_Init ( this, IDC_COMBO_PET_TYPE, &vecPETTYPE[0], vecPETTYPE.size() );
		// VEHICLE;
		CGameTextMan::GetInstance().LoadHelpCommentString("GRINDING_TYPE", strVEHICLETYPE, CGameTextMan::EM_COMMENT_TEXT, VEHICLE_TYPE_SIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("GRINDING_LEVEL", strBIKETPYE, CGameTextMan::EM_COMMENT_TEXT, EMBIKE_SIZE);
		SetWin_Combo_Init ( this, IDC_COMBO_VEHICLE_TYPE, strVEHICLETYPE, VEHICLE_TYPE_SIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, strBIKETPYE, EMBIKE_SIZE );
	}
	else
	{
		SetWin_Combo_Init ( this, IDC_COMBO_GRINDING_TYPE, COMMENT::GRINDING_TYPE, EMGRINDING_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_GRINDING_LEVEL, COMMENT::GRINDING_LEVEL, EMGRINDER_SIZE );
		// PET
		SetWin_Combo_Init ( this, IDC_COMBO_PET_TYPE, &COMMENT::PET_TYPE[0], COMMENT::PET_DEFINE.size() );
		// VEHICLE
		SetWin_Combo_Init ( this, IDC_COMBO_VEHICLE_TYPE, COMMENT::VEHICLE_TYPE, VEHICLE_TYPE_SIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, COMMENT::BIKETYPE, EMBIKE_SIZE );
	}

	SetWin_Text( this, IDC_EDIT_MOB_SCALE, "1.00" );
}

void CItemSuit::InitBox ()
{
	m_listBOX.ResetContent();

	int nSize = min(ITEM::SBOX::ITEM_SIZE, (int)m_pDummyItem->sBox.vecItems.size());
	for ( int i=0; i<nSize; ++i )
	{
		SNATIVEID sITEM = m_pDummyItem->sBox.vecItems[i].nidITEM;
		DWORD dwAMOUNT = m_pDummyItem->sBox.vecItems[i].dwAMOUNT;
		if ( sITEM==SNATIVEID(false) )		continue;

		CString strNAME = "[Error Item]";
		const SITEM* pItem = GLogicData::GetInstance().GetItem ( sITEM );
		if ( pItem )	strNAME.Format ( "[%d/%d] %d / %s", sITEM.wMainID, sITEM.wSubID, dwAMOUNT, pItem->GetName() ); 

		int nIndex = m_listBOX.AddString ( strNAME );
		m_listBOX.SetItemData ( nIndex, i );
	}
}

void CItemSuit::InitBox_Rand()
{
	m_listBox_Rand.ResetContent();

	std::vector<ITEM::SRANDOMITEM>::iterator iter = m_pDummyItem->sRandomBox.vecBOX.begin();
	std::vector<ITEM::SRANDOMITEM>::iterator iter_end = m_pDummyItem->sRandomBox.vecBOX.end();

	for ( ; iter!=iter_end; ++iter )
	{
		SNATIVEID sITEM = iter->nidITEM;
		float fRate = iter->fRATE;
		if ( sITEM==SNATIVEID(false) )		continue;

		CString strNAME = "[Error Item]";
		const SITEM* pItem = GLogicData::GetInstance().GetItem ( sITEM );
		if ( pItem )	strNAME.Format ( "[%d/%d] %.4f / %s", sITEM.wMainID, sITEM.wSubID, fRate, pItem->GetName() ); 

		int nIndex = m_listBox_Rand.AddString ( strNAME );
		m_listBox_Rand.SetItemData ( nIndex, sITEM.dwID );
	}
}

void CItemSuit::InitPetSkinPack()
{
	m_list_Mob.DeleteAllItems();

	int skinSize = (int)m_pDummyItem->sPetSkinPack.vecPetSkinData.size();
	char szTempChar[256];
	for ( int i = 0; i < skinSize; i++ )
	{
		ITEM::SPETSKINPACKITEMDATA sPetSkinPack = m_pDummyItem->sPetSkinPack.vecPetSkinData[i];
		PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sPetSkinPack.sMobID );

		sprintf( szTempChar, "%d", i );
		m_list_Mob.InsertItem( i, szTempChar );
		sprintf( szTempChar, "%s(%d/%d)", pCrowData->GetName(), sPetSkinPack.sMobID.wMainID, sPetSkinPack.sMobID.wSubID );
		m_list_Mob.SetItemText( i, 1, szTempChar );
		sprintf( szTempChar, "%.2f%%", sPetSkinPack.fScale );
		m_list_Mob.SetItemText( i, 2, szTempChar );
		sprintf( szTempChar, "%.2f%%", sPetSkinPack.fRate );
		m_list_Mob.SetItemText( i, 3, szTempChar );
	}
    

}

void CItemSuit::OnBnClickedButtonBoxAdd()
{
	CDlgItem dlgITEM;
	dlgITEM.m_bRateEnable = false;
	dlgITEM.m_bNumEnable = true;


	if ( IDOK == dlgITEM.DoModal() )
	{
		SNATIVEID sITEM;
		
		std::map<DWORD, DWORD>::iterator iter = dlgITEM.m_SelMap.begin();		
		for ( ; iter !=  dlgITEM.m_SelMap.end(); ++iter )
		{
			sITEM.dwID = iter->first;
			m_pDummyItem->sBox.INSERT ( sITEM, iter->second );
			InitBox ();
		}

	}
}

void CItemSuit::OnBnClickedButtonBoxDel()
{
	int nIndex = m_listBOX.GetCurSel();
	if ( nIndex==LB_ERR )	return;

	DWORD_PTR dwID = m_listBOX.GetItemData ( nIndex );

	m_pDummyItem->sBox.DEL ( (int)dwID );

	m_listBOX.DeleteString ( nIndex );

	InitBox ();
}

// 추가 : 2004.9.6 By 경대
//
void CItemSuit::OnBnClickedButtonBoxAdd2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgItem dlgITEM;
	dlgITEM.m_bRateEnable = true;
	dlgITEM.m_bNumEnable = false;

	if ( IDOK == dlgITEM.DoModal() )
	{
		SNATIVEID sITEM;
		
		std::map<DWORD, DWORD>::iterator iter = dlgITEM.m_SelMap.begin();		
		for ( ; iter !=  dlgITEM.m_SelMap.end(); ++iter )
		{
			sITEM.dwID = iter->first;
			float fRate = (float)iter->second / 100.0f;
			m_pDummyItem->sRandomBox.INSERT ( sITEM, fRate );
			InitBox ();
		}


	}

	InitBox_Rand();
}

// 추가 : 2004.9.6 By 경대
//
void CItemSuit::OnBnClickedButtonBoxDel2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_listBox_Rand.GetCurSel();
	if ( nIndex==LB_ERR )	return;

	m_pDummyItem->sRandomBox.DEL ( nIndex );

	m_listBox_Rand.DeleteString ( nIndex );

	InitBox_Rand();
}

void CItemSuit::OnBnClickedButtonBoxUp1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = m_listBOX.GetCurSel();
	int nCnt = m_listBOX.GetCount();

	if ( nSel <=0 || nSel >= nCnt ) return;

	ITEM::SBOX_ITEM sSelItem;

	sSelItem = m_pDummyItem->sBox.vecItems[nSel];
	m_pDummyItem->sBox.vecItems[nSel] = m_pDummyItem->sBox.vecItems[nSel-1];
	m_pDummyItem->sBox.vecItems[nSel-1] = sSelItem;

	InitBox();

	m_listBOX.SetCurSel( nSel-1);
}

void CItemSuit::OnBnClickedButtonBoxDw1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nSel = m_listBOX.GetCurSel();
	int nCnt = m_listBOX.GetCount();

	if ( nSel <0 || nSel >= nCnt-1 ) return;

	ITEM::SBOX_ITEM sSelItem;

	sSelItem = m_pDummyItem->sBox.vecItems[nSel];
	m_pDummyItem->sBox.vecItems[nSel] = m_pDummyItem->sBox.vecItems[nSel+1];
	m_pDummyItem->sBox.vecItems[nSel+1] = sSelItem;

	InitBox();
	m_listBOX.SetCurSel( nSel+1);
}

void CItemSuit::OnBnClickedButtonBoxUp2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = m_listBox_Rand.GetCurSel();
	int nCnt = m_listBox_Rand.GetCount();

	if ( nSel <=0 || nSel >= nCnt ) return;

	ITEM::SRANDOMITEM sSelItem;	

	sSelItem = m_pDummyItem->sRandomBox.vecBOX[nSel];
	m_pDummyItem->sRandomBox.vecBOX[nSel] = m_pDummyItem->sRandomBox.vecBOX[nSel-1];
	m_pDummyItem->sRandomBox.vecBOX[nSel-1] = sSelItem;

	InitBox_Rand();

	m_listBox_Rand.SetCurSel( nSel-1 );
}

void CItemSuit::OnBnClickedButtonBoxDw2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = m_listBox_Rand.GetCurSel();
	int nCnt = m_listBox_Rand.GetCount();

	if ( nSel <0 || nSel >= nCnt-1 ) return;

	ITEM::SRANDOMITEM sSelItem;	

	sSelItem = m_pDummyItem->sRandomBox.vecBOX[nSel];
	m_pDummyItem->sRandomBox.vecBOX[nSel] = m_pDummyItem->sRandomBox.vecBOX[nSel+1];
	m_pDummyItem->sRandomBox.vecBOX[nSel+1] = sSelItem;

	InitBox_Rand();

	m_listBox_Rand.SetCurSel( nSel+1 );
}



void CItemSuit::OnBnClickedButtonAddPetskin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgItem dlgMonster;
	dlgMonster.m_bRateEnable = true;
	dlgMonster.m_bNumEnable  = false;
	dlgMonster.m_bUseMonster = true;

	if( m_pDummyItem->sPetSkinPack.vecPetSkinData.size() >= 10 )
	{
		MessageBox( "Max Monster Error - 10", "Error" );
		return;
	}


	if ( IDOK == dlgMonster.DoModal() )
	{
		CString str = GetWin_Text( this, IDC_EDIT_MOB_SCALE );
		SNATIVEID mobID;
		mobID.wMainID = dlgMonster.m_wMonsterMid;
		mobID.wSubID  = dlgMonster.m_wMonsterSid;
		m_pDummyItem->sPetSkinPack.INSERT( mobID, dlgMonster.m_fRate, (float)atof(str.GetString()) );

		InitPetSkinPack();        		
	}
}


void CItemSuit::OnBnClickedButtonDelPetskin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelect = m_list_Mob.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 ) return;

	m_pDummyItem->sPetSkinPack.DEL( nSelect );

	InitPetSkinPack();
}

void CItemSuit::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	const int nSkipNum = 4;
	const int nSkipID[nSkipNum] = { IDC_BUTTON_PREV, IDC_BUTTON_NEXT, IDC_BUTTON_CANCEL, IDC_BUTTON_OK };

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


void CItemSuit::OnBnClickedButtonBoxEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nCurSel = m_listBOX.GetCurSel();
	if ( nCurSel < 0 )	return;
	
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( m_pDummyItem->sBox.vecItems[nCurSel].nidITEM );
	if ( !pItem ) return;
	
	CInputDataDlg dlg;
	dlg.m_dwNum = m_pDummyItem->sBox.vecItems[nCurSel].dwAMOUNT;
	dlg.m_fRate = 0.0f;
	dlg.m_bNum = true;
	dlg.m_strItemName = pItem->GetName();
	dlg.m_wMainID = pItem->sBasicOp.sNativeID.wMainID;
	dlg.m_wSubID = pItem->sBasicOp.sNativeID.wSubID;
	
	if ( dlg.DoModal() == IDOK )
	{
		if ( pItem->sDrugOp.wPileNum < dlg.m_dwNum )
		{
			std::string strErr( sc::string::format("\'%1%\' beyond the COUNT of it's limit, limit %2%", dlg.m_strItemName, pItem->sDrugOp.wPileNum) );
			MessageBox( strErr.c_str() );
			dlg.m_dwNum = pItem->sDrugOp.wPileNum;
		}
		m_pDummyItem->sBox.vecItems[nCurSel].dwAMOUNT = dlg.m_dwNum;
		InitBox();
	}
}

void CItemSuit::OnBnClickedButtonBoxEdit2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCurSel = m_listBox_Rand.GetCurSel();
	if ( nCurSel < 0 )	return;

	const SITEM* pItem = GLogicData::GetInstance().GetItem ( m_pDummyItem->sRandomBox.vecBOX[nCurSel].nidITEM );
	if ( !pItem ) return;
	
	CInputDataDlg dlg;
	dlg.m_dwNum = 0;
	dlg.m_fRate = m_pDummyItem->sRandomBox.vecBOX[nCurSel].fRATE;
	dlg.m_bNum = false;
	dlg.m_strItemName = pItem->GetName();
	dlg.m_wMainID = pItem->sBasicOp.sNativeID.wMainID;
	dlg.m_wSubID = pItem->sBasicOp.sNativeID.wSubID;
	
	if ( dlg.DoModal() == IDOK )
	{
		m_pDummyItem->sRandomBox.vecBOX[nCurSel].fRATE = dlg.m_fRate;
		InitBox_Rand();
	}
}

void CItemSuit::OnBnClickedButtonSuit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;
	
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strITEMSUIT[SUIT_NSIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("ITEMSUIT", strITEMSUIT, CGameTextMan::EM_COMMENT_TEXT, SUIT_NSIZE);
		dlg.SetStringList( strITEMSUIT, SUIT_NSIZE );
		dlg.m_nSelect = m_pDummyItem->sSuitOp.emSuit;

		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sSuitOp.emSuit = (EMSUIT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_SUIT, strITEMSUIT[m_pDummyItem->sSuitOp.emSuit].c_str() );
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::ITEMSUIT, SUIT_NSIZE );	
		dlg.m_nSelect = m_pDummyItem->sSuitOp.emSuit;

		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sSuitOp.emSuit = (EMSUIT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_SUIT, COMMENT::ITEMSUIT[m_pDummyItem->sSuitOp.emSuit].c_str() );
		}
	}
}

void CItemSuit::OnBnClickedButtonAttType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strITEMATTACK[ITEMATT_NSIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("ITEMATTACK", strITEMATTACK, CGameTextMan::EM_COMMENT_TEXT, ITEMATT_NSIZE);
		dlg.SetStringList( strITEMATTACK, ITEMATT_NSIZE );
		dlg.m_nSelect = m_pDummyItem->sSuitOp.emSuit;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sSuitOp.emAttack = (GLITEM_ATT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_ATT_TYPE, strITEMATTACK[m_pDummyItem->sSuitOp.emAttack].c_str() );
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::ITEMATTACK, ITEMATT_NSIZE );	
		dlg.m_nSelect = m_pDummyItem->sSuitOp.emAttack;

		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sSuitOp.emAttack = (GLITEM_ATT) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_ATT_TYPE, COMMENT::ITEMATTACK[m_pDummyItem->sSuitOp.emAttack].c_str() );
		}
	}
}
void CItemSuit::OnBnClickedButtonAddonNo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectAddonType( 0, IDC_EDIT_ADDON_TYPE1 );
}

void CItemSuit::OnBnClickedButtonAddonNo2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectAddonType( 1, IDC_EDIT_ADDON_TYPE2 );
}

void CItemSuit::OnBnClickedButtonAddonNo3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectAddonType( 2, IDC_EDIT_ADDON_TYPE3 );
}

void CItemSuit::OnBnClickedButtonAddonNo4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectAddonType( 3, IDC_EDIT_ADDON_TYPE4 );
}

void CItemSuit::OnBnClickedButtonAddonNo5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectAddonType( 4, IDC_EDIT_ADDON_TYPE5 );
}

void CItemSuit::OnBnClickedButtonAddonNo6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SelectAddonType( 5, IDC_EDIT_ADDON_TYPE6 );
}

void CItemSuit::SelectAddonType( int nIndex, int nEditID )
{
	CSelectDialog dlg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strITEMADDON[EMADD_SIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("ITEMADDON", strITEMADDON, CGameTextMan::EM_COMMENT_TEXT, EMADD_SIZE);
		dlg.SetStringList( strITEMADDON, EMADD_SIZE );
		dlg.m_nSelect = m_pDummyItem->sSuitOp.sADDON[nIndex].emTYPE;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sSuitOp.sADDON[nIndex].emTYPE = (EMITEM_ADDON) dlg.m_dwData;
			SetWin_Text( this, nEditID, strITEMADDON[m_pDummyItem->sSuitOp.sADDON[nIndex].emTYPE].c_str() );
		}
	}
	else
	{
		dlg.SetStringList( COMMENT::ITEMADDON, EMADD_SIZE );	
		dlg.m_nSelect = m_pDummyItem->sSuitOp.sADDON[nIndex].emTYPE;


		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sSuitOp.sADDON[nIndex].emTYPE = (EMITEM_ADDON) dlg.m_dwData;
			SetWin_Text( this, nEditID, COMMENT::ITEMADDON[m_pDummyItem->sSuitOp.sADDON[nIndex].emTYPE].c_str() );
		}
	}
}

void CItemSuit::OnBnClickedButtonBlow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{		
		std::vector<std::string> vecBLOW_ALL, vecBLOW_VAR1, vecBLOW_VAR2;
		std::string strBLOW[EMBLOW_SIZE], strBLOW_VAR1[EMBLOW_SIZE], strBLOW_VAR2[EMBLOW_SIZE];
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("BLOW", vecBLOW_ALL, CGameTextMan::EM_COMMENT_TEXT);
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("BLOW_VAR1", vecBLOW_VAR1, CGameTextMan::EM_COMMENT_TEXT);
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("BLOW_VAR2", vecBLOW_VAR2, CGameTextMan::EM_COMMENT_TEXT);
		for ( INT i = 0; i < EMBLOW_SIZE; ++i )
		{
			strBLOW[i] = vecBLOW_ALL[i];
			strBLOW_VAR1[i] = vecBLOW_VAR1[i];
			strBLOW_VAR2[i] = vecBLOW_VAR2[i];
		}

		dlg.SetStringList(strBLOW, EMBLOW_SIZE);
		dlg.m_nSelect = m_pDummyItem->sSuitOp.sBLOW.emTYPE;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sSuitOp.sBLOW.emTYPE = (EMSTATE_BLOW) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_BLOW_TYPE, strBLOW[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str() );

			BOOL bSTATE_BLOW = ( m_pDummyItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE );
			BOOL bBLOW_VAR1(TRUE), bBLOW_VAR2(TRUE);
			const char *szBLOW_VAR1 = strBLOW_VAR1[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str();
			const char *szBLOW_VAR2 = strBLOW_VAR2[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str();

			if ( !strcmp(szBLOW_VAR1,strBLOW_VAR1[EMBLOW_NONE].c_str()) )		bBLOW_VAR1 = FALSE;
			if ( !strcmp(szBLOW_VAR2,strBLOW_VAR2[EMBLOW_NONE].c_str()) )		bBLOW_VAR2 = FALSE;
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
		dlg.m_nSelect = m_pDummyItem->sSuitOp.sBLOW.emTYPE;

		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sSuitOp.sBLOW.emTYPE = (EMSTATE_BLOW) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_BLOW_TYPE, COMMENT::BLOW[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str() );

			BOOL bSTATE_BLOW = ( m_pDummyItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE );
			BOOL bBLOW_VAR1(TRUE), bBLOW_VAR2(TRUE);
			const char *szBLOW_VAR1 = COMMENT::BLOW_VAR1[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str();
			const char *szBLOW_VAR2 = COMMENT::BLOW_VAR2[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str();

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

void CItemSuit::OnBnClickedButtonQuestion()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{		
		std::vector<std::string> vecITEM_QUE_TYPE, vecITEM_QUE_VAR1, vecITEM_QUE_VAR2;
		std::string strITEM_QUE_TYPE[QUESTION_SIZE], strITEM_QUE_VAR1[QUESTION_SIZE], strITEM_QUE_VAR2[QUESTION_SIZE];
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("ITEM_QUE_TYPE", vecITEM_QUE_TYPE, CGameTextMan::EM_COMMENT_TEXT);
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("ITEM_QUE_VAR1", vecITEM_QUE_VAR1, CGameTextMan::EM_COMMENT_TEXT);
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("ITEM_QUE_VAR2", vecITEM_QUE_VAR2, CGameTextMan::EM_COMMENT_TEXT);
		for ( INT i = 0; i < QUESTION_SIZE; ++i )
		{
			strITEM_QUE_TYPE[i] = vecITEM_QUE_TYPE[i];
			strITEM_QUE_VAR1[i] = vecITEM_QUE_VAR1[i];
			strITEM_QUE_VAR2[i] = vecITEM_QUE_VAR2[i];
		}
		dlg.SetStringList( strITEM_QUE_TYPE, QUESTION_SIZE );	
		dlg.m_nSelect = m_pDummyItem->sQuestionItem.emType;

		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sQuestionItem.emType = (EMITEM_QUESTION) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_QUESTION_TYPE, strITEM_QUE_TYPE[m_pDummyItem->sQuestionItem.emType].c_str() );	
		}
		BOOL bTIME(false);
		BOOL bEXP( m_pDummyItem->sQuestionItem.emType==QUESTION_EXP_GET );
		BOOL bPARAM1(false);
		BOOL bPARAM2(false);

		switch( m_pDummyItem->sQuestionItem.emType )
		{
		case QUESTION_SPEED_UP:
		case QUESTION_ATTACK_UP:
		case QUESTION_EXP_UP:
		case QUESTION_LUCKY:
		case QUESTION_SPEED_UP_M:
		case QUESTION_ATTACK_UP_M:
			bPARAM1 = bTIME = true;
			break;

		case QUESTION_CRAZY:
		case QUESTION_MADNESS:
			bTIME = bPARAM1 = bPARAM2 = true;
			break;

		case QUESTION_BOMB:
		case QUESTION_HEAL:
			bPARAM1 = true;
			break;

		case QUESTION_MOBGEN:
			bPARAM1 = bPARAM2 = true;
			break;
		};

		const char *szQUE_VAR1 = strITEM_QUE_VAR1[m_pDummyItem->sQuestionItem.emType].c_str();
		const char *szQUE_VAR2 = strITEM_QUE_VAR2[m_pDummyItem->sQuestionItem.emType].c_str();

		SetWin_Text ( this, IDC_STATIC_QUEVAR1, szQUE_VAR1 );
		SetWin_Text ( this, IDC_STATIC_QUEVAR2, szQUE_VAR2 );

		SetWin_Enable( this, IDC_EDIT_QUE_TIME, bTIME );
		SetWin_Enable( this, IDC_EDIT_QUE_EXP, bEXP );
		SetWin_Enable( this, IDC_EDIT_QUE_PARAM1, bPARAM1 );
		SetWin_Enable( this, IDC_EDIT_QUE_PARAM2, bPARAM2 );

		SetReadMode();
	}
	else
	{
		dlg.SetStringList( COMMENT::ITEM_QUE_TYPE, QUESTION_SIZE );	
		dlg.m_nSelect = m_pDummyItem->sQuestionItem.emType;

		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sQuestionItem.emType = (EMITEM_QUESTION) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_QUESTION_TYPE, COMMENT::ITEM_QUE_TYPE[m_pDummyItem->sQuestionItem.emType].c_str() );	

			BOOL bTIME(false);
			BOOL bEXP( m_pDummyItem->sQuestionItem.emType==QUESTION_EXP_GET );
			BOOL bPARAM1(false);
			BOOL bPARAM2(false);

			switch( m_pDummyItem->sQuestionItem.emType )
			{
			case QUESTION_SPEED_UP:
			case QUESTION_ATTACK_UP:
			case QUESTION_EXP_UP:
			case QUESTION_LUCKY:
			case QUESTION_SPEED_UP_M:
			case QUESTION_ATTACK_UP_M:
				bPARAM1 = bTIME = true;
				break;

			case QUESTION_CRAZY:
			case QUESTION_MADNESS:
				bTIME = bPARAM1 = bPARAM2 = true;
				break;

			case QUESTION_BOMB:
			case QUESTION_HEAL:
				bPARAM1 = true;
				break;

			case QUESTION_MOBGEN:
				bPARAM1 = bPARAM2 = true;
				break;
			};

			const char *szQUE_VAR1 = COMMENT::ITEM_QUE_VAR1[m_pDummyItem->sQuestionItem.emType].c_str();
			const char *szQUE_VAR2 = COMMENT::ITEM_QUE_VAR2[m_pDummyItem->sQuestionItem.emType].c_str();

			SetWin_Text ( this, IDC_STATIC_QUEVAR1, szQUE_VAR1 );
			SetWin_Text ( this, IDC_STATIC_QUEVAR2, szQUE_VAR2 );

			SetWin_Enable( this, IDC_EDIT_QUE_TIME, bTIME );
			SetWin_Enable( this, IDC_EDIT_QUE_EXP, bEXP );
			SetWin_Enable( this, IDC_EDIT_QUE_PARAM1, bPARAM1 );
			SetWin_Enable( this, IDC_EDIT_QUE_PARAM2, bPARAM2 );

			SetReadMode();
		}
	}
}

void CItemSuit::OnBnClickedButtonDrug()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSelectDialog dlg;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string> vecDRUG, vecDRUG_VAR;
		std::string strITEMDRUG[ITEM_DRUG_SIZE], strITEMDRUG_VAR[ITEM_DRUG_SIZE];
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("ITEMDRUG", vecDRUG, CGameTextMan::EM_COMMENT_TEXT);
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("ITEMDRUG_VAR", vecDRUG_VAR, CGameTextMan::EM_COMMENT_TEXT);
		for ( INT i = 0; i < EMITEM_DRUG::ITEM_DRUG_SIZE; ++i )
		{
			strITEMDRUG[i] = vecDRUG[i];
			strITEMDRUG_VAR[i] = vecDRUG_VAR[i];
		}

		dlg.m_nItemType = m_pDummyItem->sBasicOp.emItemType;
		dlg.SetStringListII( strITEMDRUG, ITEM_DRUG_SIZE );	
		dlg.m_nSelect = m_pDummyItem->sDrugOp.emDrug;
		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sDrugOp.emDrug = (EMITEM_DRUG) dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_DRUG_TYPE, strITEMDRUG[m_pDummyItem->sDrugOp.emDrug].c_str() );
			SetWin_Text( this, IDC_STATIC_DRUG_VAR, strITEMDRUG_VAR[m_pDummyItem->sDrugOp.emDrug].c_str()  );
		}
	}
	else
	{
		dlg.m_nItemType = m_pDummyItem->sBasicOp.emItemType;
		dlg.SetStringListII( COMMENT::ITEMDRUG, ITEM_DRUG_SIZE );	
		dlg.m_nSelect = m_pDummyItem->sDrugOp.emDrug;

		if ( dlg.DoModal() == IDOK )
		{
			m_pDummyItem->sDrugOp.emDrug = (EMITEM_DRUG) dlg.m_dwData;

			SetWin_Text( this, IDC_EDIT_DRUG_TYPE, COMMENT::ITEMDRUG[m_pDummyItem->sDrugOp.emDrug].c_str() );
			SetWin_Text( this, IDC_STATIC_DRUG_VAR, COMMENT::ITEMDRUG_VAR[m_pDummyItem->sDrugOp.emDrug].c_str()  );

			CWnd *pWnd  = GetDlgItem(IDC_EDIT_COSTUME_TIME);
			CWnd *pWnd1 = GetDlgItem(IDC_BUTTON_ER);

			if( m_pDummyItem->sDrugOp.emDrug == ITEM_DRUG_COSTUME_STATS || m_pDummyItem->sDrugOp.emDrug == ITEM_DRUG_TEMP_ENCHANT )
			{
				if( pWnd  ) pWnd->EnableWindow(TRUE);
			}
			else
			{
				if( pWnd ) pWnd->EnableWindow(FALSE);
			}

			if( m_pDummyItem->sDrugOp.emDrug == ITEM_DRUG_EFFECT_RELEASE )
			{
				if( pWnd1 ) pWnd1->EnableWindow(TRUE);
			}
			else
			{
				if( pWnd1 ) pWnd1->EnableWindow(FALSE);
			}
			
		}
	}
}
void CItemSuit::OnCbnSelchangeComboVehicleType()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{	
		std::string strBIKETYPE[EMBIKE_NSIZE], strCARTYPE[EMBIKE_NSIZE],
			strREARCAR[EMBIKE_NSIZE], strBOARD[EMBIKE_NSIZE];

		CGameTextMan::GetInstance().LoadHelpCommentString("BIKETYPE", strBIKETYPE, CGameTextMan::EM_COMMENT_TEXT, EMBIKE_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("CARTYPE", strBIKETYPE, CGameTextMan::EM_COMMENT_TEXT, EMBIKE_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("REARCARTYPE", strBIKETYPE, CGameTextMan::EM_COMMENT_TEXT, EMBIKE_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("BOARDTYPE", strBIKETYPE, CGameTextMan::EM_COMMENT_TEXT, EMBIKE_NSIZE);

		switch ( static_cast<VEHICLE_TYPE>(GetWin_Combo_Sel(this,IDC_COMBO_VEHICLE_TYPE)) )
		{
		case VEHICLE_TYPE_BIKE:		SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, strBIKETYPE, EMBIKE_NSIZE );break;
		case VEHICLE_TYPE_CAR:		SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, strCARTYPE, EMBIKE_NSIZE );	break;
		case VEHICLE_TYPE_REARCAR:	SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, strREARCAR, EMBIKE_NSIZE );	break;
		default:					SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, strBOARD, EMBIKE_NSIZE );	break;
		}
	}
	else
	{
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		if( (VEHICLE_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_VEHICLE_TYPE ) == VEHICLE_TYPE_BIKE )
		{
			SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, COMMENT::BIKETYPE, EMBIKE_NSIZE );
		}
		else if ( (VEHICLE_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_VEHICLE_TYPE ) == VEHICLE_TYPE_CAR )
		{
			SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, COMMENT::CARTYPE, EMBIKE_NSIZE );
		}
		else if ( (VEHICLE_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_VEHICLE_TYPE ) == VEHICLE_TYPE_REARCAR )
		{
			SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, COMMENT::REARCARTYPE, EMBIKE_NSIZE );
		}
		else
		{
			SetWin_Combo_Init ( this, IDC_COMBO_PARTS_TYPE, COMMENT::BOARDTYPE, EMBIKE_NSIZE );
		}
	}
}

void CItemSuit::OnCbnSelchangeComboPartsType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CItemSuit::OnBnClickedCheckEnableCostum()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDummyItem->sSuitOp.SETSTATE(EMITEM_SUIT_CHANGE_COLOR, GetWin_Check( this, IDC_CHECK_ENABLE_COSTUM ));
}

void CItemSuit::OnBnClickedCheckEnableUnique()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDummyItem->sSuitOp.SETSTATE(EMITEM_SUIT_UNIQUE, GetWin_Check( this, IDC_CHECK_ENABLE_UNIQUE ));
}

void CItemSuit::OnBnClickedDrugErList()
{
	CSelectCheckDlg sDlg;

	sDlg.m_dwCheckList = m_pDummyItem->sDrugOp.dwERList;

	if( sDlg.DoModal() == IDOK )
	{
		m_pDummyItem->sDrugOp.dwERList = sDlg.m_dwCheckList;
	}

}

void CItemSuit::OnTimer(UINT_PTR nIDEvent)
{
    switch(nIDEvent)
    {
    case TIMER_ITEMSUIT_SAVE_STATE_ID:

        SetWin_Text ( this, IDC_EDIT_SAVE_STATE, "" );
        KillTimer ( TIMER_ITEMSUIT_SAVE_STATE_ID );

        break;
    }

    CPropertyPage::OnTimer(nIDEvent);
}

void CItemSuit::ListBoxNameUpdate(void)
{
	const EMITEM_TYPE itemT(m_pDummyItem->BasicType());
	switch (itemT)
	{
	case ITEM_LOCK_BOX:
		{
			SetWin_Text ( this, IDC_STATIC_PRESENT, "열쇠목록" );
			SetWin_Text ( this, IDC_STATIC_RANDOM, "상품목록" );
		}
		break;
	default:
		{
			SetWin_Text ( this, IDC_STATIC_PRESENT, "선물상자" );
			SetWin_Text ( this, IDC_STATIC_RANDOM, "랜덤상자" );
		}
	}
}
//void CItemSuit::OnEnChangeSkillRate()
//{
//	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
//	// CPropertyPage::OnInitDialog() 함수를 재지정 
//	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
//	// 이 알림 메시지를 보내지 않습니다.
//
//	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//}

void CItemSuit::OnCbnSelchangeSkillTarget()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
