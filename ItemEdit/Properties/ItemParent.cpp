// ItemProperty1.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include <string>

#include "../ItemEdit.h"
#include "ItemParent.h"
#include "SheetWithTab.h"

#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../../Enginelib/DxEffect/Char/DxEffcharData.h"
#include "../../Enginelib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../Enginelib/DxMeshs/DxSimpleMeshMan.h"

#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../../RanLogic/GLStringTable.h"
#include "../../RanLogic/Skill/GLSkill.h"
 #include "../../EngineLib/GUInterface/GameTextLoader.h"
 #include "../../Enginelib/GUInterface/GameTextControl.h"

#include "../../MobNpcEdit/SelectDialog.h"

#include "../../RanLogic/Activity/Activity.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/Item/ItemSet/GLItemSet.h"

#include "../Dialog/WearingDlg.h"
#include "../Dialog/GrindingSimulatorDlg.h"
#include "../IconPreview.h"
#include "../Dialog/SetItemInfo.h"
#include "../SelectCheckDialog.h"

// CItemParent ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CItemParent, CPropertyPage)

CItemParent::CItemParent( LOGFONT logfont )
	: CPropertyPage(CItemParent::IDD),
	m_pFont ( NULL )
{
	m_bDlgInit = FALSE;
	m_pItem = NULL;
	m_pDummyItem = new SITEM;

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);

	m_pIconPreview = new CIconPreview;
}

CItemParent::~CItemParent()
{
	SAFE_DELETE( m_pDummyItem );
	SAFE_DELETE( m_pFont );
	SAFE_DELETE( m_pIconPreview );
}

void CItemParent::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);	
}


BEGIN_MESSAGE_MAP(CItemParent, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_INVENFILE, OnBnClickedButtonInvenfile)
	ON_BN_CLICKED(IDC_BUTTON_FIELDFILE, OnBnClickedButtonFieldfile)
	ON_BN_CLICKED(IDC_BUTTON_GRINDING_SIMULATOR, OnBnClickedButtonGrindingSimulator)
	ON_BN_CLICKED(IDC_BUTTON_SKIN_EFFECTFILE, OnBnClickedButtonSkinEffectfile)
	ON_BN_CLICKED(IDC_BUTTON_EFFECTFILE, OnBnClickedButtonEffectfile)
	ON_BN_CLICKED(IDC_BUTTON_SKIN_TAR_EFFECTFILE, OnBnClickedButtonSkinTarEffectfile)
	ON_BN_CLICKED(IDC_BUTTON_GENERAL_EFFECTFILE,  OnBnClickedButtonGeneralEffectfile)
	ON_EN_CHANGE(IDC_EDIT_ITEMNAME, OnEnChangeEditItemname)
	ON_EN_CHANGE(IDC_EDIT_ITEMDESC, OnEnChangeEditItemdesc)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_RADIO_MAN, OnBnClickedRadioMan)
	ON_BN_CLICKED(IDC_RADIO_WOMAN, OnBnClickedRadioWoman)
	ON_BN_CLICKED(IDC_BUTTON_ITEM_TYPE, OnBnClickedButtonItemType)
	ON_BN_CLICKED(IDC_BUTTON_WEARING_SET, OnBnClickedButtonWearingSet)
	//ON_EN_CHANGE(IDC_EDIT_INVENICON_X, &CItemParent::OnEnChangeEditInveniconX)
	//ON_EN_CHANGE(IDC_EDIT_INVENICON_Y, &CItemParent::OnEnChangeEditInveniconY)
    ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CHECK_INFO, &CItemParent::OnBnClickedButtonCheckInfo)
	ON_BN_CLICKED(IDC_BUTTON_OPENDECOMPOSEITEM, &CItemParent::OnBnClickedButtonDecompose)
END_MESSAGE_MAP()


// CItemParent �޽��� ó�����Դϴ�.

BOOL CItemParent::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );
	
	m_bDlgInit = TRUE;
	InitDefaultCtrls ();
	UpdateItems ();

	SetReadMode();

	CWnd* pWnd = GetDlgItem( IDC_ICON_IMAGE );
	if( pWnd )
	{
		pWnd->SetWindowPos( NULL, 0, 0, CIconPreview::EIconSize, CIconPreview::EIconSize, SWP_NOMOVE );
		m_pIconPreview->AttachWnd( pWnd->GetSafeHwnd() );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CItemParent::PostNcDestroy()
{
	m_pIconPreview->DetachWnd();

	CPropertyPage::PostNcDestroy();
}

void CItemParent::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pItem = NULL;
	m_pSheetTab->ActiveItemSuitTreePage ();
}

void CItemParent::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if ( !InverseUpdateItems() )
	{		
		return;
	}

	CItemNode sItemNode;
	sItemNode.m_sItem = *m_pDummyItem;

	GLItemMan::GetInstance().InsertItem ( m_pDummyItem->sBasicOp.sNativeID, &sItemNode );

 	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
 	{
 		CGameTextMan::GetInstance().InsertString( strKeyName.c_str(), strName.c_str(), CGameTextMan::EM_ITEM_TEXT);
 		CGameTextMan::GetInstance().InsertString( strKeyDesc.c_str(), strDesc.c_str(), CGameTextMan::EM_ITEM_TEXT);
 	}
 	else
 	{
		GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::ITEM );
		GLStringTable::GetInstance().InsertString ( strKeyDesc, strDesc, GLStringTable::ITEM );
	}

	ShowSaveState();
}

void CItemParent::ShowSaveState()
{
    SetWin_Text ( this, IDC_EDIT_SAVE_STATE, "Save Successed." );

    // SetTimer�� ������ ���� ID �� ���ǵ� �̺�Ʈ�� ������
    // �� �̺�Ʈ�� ���̰� ���ο� �̺�Ʈ�� ���������
    // �׿� ���� ����ó���� ���� ���� �ʽ��ϴ�.
    SetTimer ( TIMER_ITEMPARENT_SAVE_STATE_ID, SAVE_MESSAGE_APPEAR_TIME, NULL );
}

void CItemParent::InitAllItem ()
{
	m_pItem = NULL;
}

BOOL CItemParent::SetItem ( SITEM* pItem )
{
	if ( m_pItem )
	{
		return FALSE;
	}

	m_pItem = pItem;

	//	���̷� ��������
	*m_pDummyItem = *m_pItem;

	if ( m_bDlgInit ) 
	{
		InitDefaultCtrls ();
		UpdateItems ();		
	}

	return TRUE;
}

void CItemParent::UpdateItems ()
{
	//	���� ID
	SetWin_Num_int ( this, IDC_EDIT_MID, m_pDummyItem->sBasicOp.sNativeID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_SID, m_pDummyItem->sBasicOp.sNativeID.wSubID );

	//	������ �̸�
	SetWin_Text ( this, IDC_EDIT_ITEMNAME, m_pDummyItem->sBasicOp.strName.c_str() );

	//	������ ����
	SetWin_Combo_Sel ( this, IDC_COMBO_LEVEL, (int)m_pDummyItem->sBasicOp.emLevel );

	//	Trade
	SetWin_Check( this, IDC_CHECK_SELLENABLE, m_pDummyItem->sBasicOp.dwFlags&TRADE_SALE );
	SetWin_Check( this, IDC_CHECK_EXCHANGE, m_pDummyItem->sBasicOp.dwFlags&TRADE_EXCHANGE );
	SetWin_Check( this, IDC_CHECK_STORAGE, m_pDummyItem->sBasicOp.dwFlags&LIMIT_LOCKER );
	SetWin_Check( this, IDC_CHECK_THROW, m_pDummyItem->sBasicOp.dwFlags&TRADE_THROW );
	SetWin_Check( this, IDC_CHECK_GARBAGE, m_pDummyItem->sBasicOp.dwFlags&TRADE_GARBAGE );
	SetWin_Check( this, IDC_CHECK_EVENT_SGL, m_pDummyItem->sBasicOp.dwFlags&TRADE_EVENT_SGL );

    SetWin_Num_int ( this, IDC_EDIT_POINT_TR_COUNT, m_pDummyItem->sBasicOp.nRPTrade );
    
	
	SetWin_Check( this, IDC_CHECK_RESTRICT, m_pDummyItem->sBasicOp.dwFlags&ITEM_RESTRICT ); // by luxes.
	SetWin_Check( this, IDC_CHECK_DISGUISE, m_pDummyItem->sBasicOp.dwFlags&ITEM_DISGUISE );
	SetWin_Check( this, IDC_CHECK_TIMELMT, m_pDummyItem->sBasicOp.dwFlags&ITEM_TIMELMT );
    SetWin_Check( this, IDC_CHECK_BOX_WRAPPABLE, m_pDummyItem->sBasicOp.dwFlags&ITEM_BOX_WRAPPABLE );   // ������ ���� ���� ����;
    SetWin_Check( this, IDC_CHECK_SHOW_CONTENTS, m_pDummyItem->sBox.ShowContents );    
	SetWin_Check( this, IDC_CHECK_SPLIT, m_pDummyItem->sBasicOp.dwFlags&ITEM_SPLIT);

	//	���԰���
	SetWin_Num_LONGLONG ( this, IDC_EDIT_PRICE, m_pDummyItem->sBasicOp.dwBuyPrice );
	SetWin_Num_LONGLONG ( this, IDC_EDIT_PRICE2, m_pDummyItem->sBasicOp.dwSellPrice );

	//	������ ���� ��Ģ
	SetWin_Num_int ( this, IDC_EDIT_SPECID, m_pDummyItem->sGenerateOp.dwSpecID );
	SetWin_Num_int ( this, IDC_EDIT_TIME_SET, m_pDummyItem->sGenerateOp.dwLimitTime );
	SetWin_Num_int ( this, IDC_EDIT_TIME_LIMIT, m_pDummyItem->sGenerateOp.dwLimitTimeGen );

	//	������ �ε���.
	//SetWin_Num_int ( this, IDC_EDIT_INVENICON_X, m_pDummyItem->sBasicOp.sICONID.wMainID );
	//SetWin_Num_int ( this, IDC_EDIT_INVENICON_Y, m_pDummyItem->sBasicOp.sICONID.wSubID );

	//	���� ����.
	SetWin_Text ( this, IDC_EDIT_FIELDFILE, m_pDummyItem->GetFieldFile() );
	SetWin_Text ( this, IDC_EDIT_INVENFILE, m_pDummyItem->GetInventoryFile() );

	SetWin_Text ( this, IDC_EDIT_ITEMDESC, m_pDummyItem->sBasicOp.strComment.c_str() );
	
	//	��, ��� �Ӽ�.	
	SetWin_Check( this, IDC_RADIO_BRIGHT_LIGHT, m_pDummyItem->sBasicOp.emReqBright==BRIGHT_LIGHT);
	SetWin_Check( this, IDC_RADIO_BRIGHT_DARK,  m_pDummyItem->sBasicOp.emReqBright==BRIGHT_DARK );
	SetWin_Check( this, IDC_RADIO_BRIGHT_BOTH,  m_pDummyItem->sBasicOp.emReqBright==BRIGHT_BOTH );

	//	��밡�� ĳ����.
	SetWin_Check( this, IDC_CHECK_FIGHTER_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_FIGHTER_M );
	SetWin_Check( this, IDC_CHECK_ARMS_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ARMS_M );
	SetWin_Check( this, IDC_CHECK_ARCHER_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ARCHER_W );
	SetWin_Check( this, IDC_CHECK_SPIRIT_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_SPIRIT_W );

	SetWin_Check( this, IDC_CHECK_FIGHTER_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_FIGHTER_W );
	SetWin_Check( this, IDC_CHECK_ARMS_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ARMS_W );
	SetWin_Check( this, IDC_CHECK_ARCHER_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ARCHER_M );
	SetWin_Check( this, IDC_CHECK_SPIRIT_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_SPIRIT_M );

	SetWin_Check( this, IDC_CHECK_EXTREME_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_EXTREME_M );
	SetWin_Check( this, IDC_CHECK_EXTREME_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_EXTREME_W );

	SetWin_Check( this, IDC_CHECK_SCIENTIST_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_SCIENTIST_M );
	SetWin_Check( this, IDC_CHECK_SCIENTIST_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_SCIENTIST_W );

    SetWin_Check( this, IDC_CHECK_ASSASSIN_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ASSASSIN_M );
    SetWin_Check( this, IDC_CHECK_ASSASSIN_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ASSASSIN_W );

    SetWin_Check( this, IDC_CHECK_TRICKER_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_TRICKER_M );
    SetWin_Check( this, IDC_CHECK_TRICKER_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_TRICKER_W );

	SetWin_Check( this, IDC_CHECK_ACTOR_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ACTOR_M );
	SetWin_Check( this, IDC_CHECK_ACTOR_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ACTOR_W );

	SetWin_Check( this, IDC_RADIO_ALL, m_pDummyItem->sBasicOp.dwReqCharClass==GLCC_ALL_ACTOR);
	SetWin_Check( this, IDC_RADIO_MAN, m_pDummyItem->sBasicOp.dwReqCharClass==GLCC_TOOL_MAN );
	SetWin_Check( this, IDC_RADIO_WOMAN, m_pDummyItem->sBasicOp.dwReqCharClass==GLCC_TOOL_WOMAN );

	//	��밡�� �п�.
	SetWin_Check( this, IDC_CHECK_SCHOOL1, m_pDummyItem->sBasicOp.dwReqSchool&GLSCHOOL_00 );
	SetWin_Check( this, IDC_CHECK_SCHOOL2, m_pDummyItem->sBasicOp.dwReqSchool&GLSCHOOL_01 );
	SetWin_Check( this, IDC_CHECK_SCHOOL3, m_pDummyItem->sBasicOp.dwReqSchool&GLSCHOOL_02 );

	//	�䱸 ��ġ.
	SetWin_Num_int ( this, IDC_EDIT_NEEDLEVELDW, m_pDummyItem->sBasicOp.wReqLevelDW );
	SetWin_Num_int ( this, IDC_EDIT_NEEDLEVELUP, m_pDummyItem->sBasicOp.wReqLevelUP );

    //  �䱸 Ȱ��
    SetWin_Combo_Sel( this, IDC_COMBO_ACTIVITY_TYPE, m_pDummyItem->sBasicOp.emReqActivityType );
    SetWin_Num_int( this, IDC_EDIT_NEED_ACT_POINT_DW, m_pDummyItem->sBasicOp.wReqActPointDW );
    SetWin_Num_int( this, IDC_EDIT_NEED_ACT_POINT_UP, m_pDummyItem->sBasicOp.wReqActPointUP );

	//  �䱸 �⿩��
	SetWin_Num_int( this, IDC_EDIT_NEED_CONTRIBUTION_POINT, m_pDummyItem->sBasicOp.dwReqContribution );

	SetWin_Num_int ( this, IDC_EDIT_POWER, m_pDummyItem->sBasicOp.sReqStats.wPow );
	SetWin_Num_int ( this, IDC_EDIT_STRENGTH, m_pDummyItem->sBasicOp.sReqStats.wStr );
	SetWin_Num_int ( this, IDC_EDIT_SPIRIT, m_pDummyItem->sBasicOp.sReqStats.wSpi );
	SetWin_Num_int ( this, IDC_EDIT_DEXTERITY, m_pDummyItem->sBasicOp.sReqStats.wDex );
	SetWin_Num_int ( this, IDC_EDIT_INTELLIGENT, m_pDummyItem->sBasicOp.sReqStats.wInt );
	SetWin_Num_int ( this, IDC_EDIT_STAMINA, m_pDummyItem->sBasicOp.sReqStats.wSta );

	SetWin_Num_int ( this, IDC_EDIT_REQ_PA, m_pDummyItem->sBasicOp.wReqPA );
	SetWin_Num_int ( this, IDC_EDIT_REQ_SA, m_pDummyItem->sBasicOp.wReqSA );

	//	������ ����.
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		SetWin_Text(this, IDC_EDIT_ITEM_TYPE, CGameTextMan::GetInstance().GetCommentText("ITEMTYPE", m_pDummyItem->sBasicOp.emItemType));
	else
		SetWin_Text( this, IDC_EDIT_ITEM_TYPE, COMMENT::ITEMTYPE[m_pDummyItem->sBasicOp.emItemType].c_str() );	
	
	SetWin_Combo_Sel ( this, IDC_COMBO_COOL_TYPE, (int)m_pDummyItem->sBasicOp.emCoolType );	

	//	���� ���� Ƚ��.
	SetWin_Num_int ( this, IDC_EDIT_REMODEL_NUM, m_pDummyItem->sSuitOp.wReModelNum );
	
	//	�Ҹ���.
	SetWin_Check( this, IDC_CHECK_INSTANCE, m_pDummyItem->sDrugOp.bInstance );

	CTimeSpan cSPAN(m_pDummyItem->sDrugOp.tTIME_LMT);
	SetWin_Num_int ( this, IDC_EDIT_LMT_D, (int)cSPAN.GetDays() );
	SetWin_Num_int ( this, IDC_EDIT_LMT_H, cSPAN.GetHours() );
	SetWin_Num_int ( this, IDC_EDIT_LMT_M, cSPAN.GetMinutes() );

	SetWin_Text ( this, IDC_EDIT_SKIN_EFFECTFILE, m_pDummyItem->GetSelfBodyEffect() );
	SetWin_Text ( this, IDC_EDIT_TAR_EFFECTFILE, m_pDummyItem->GetTargBodyEffect() );
	SetWin_Text ( this, IDC_EDIT_EFFECTFILE, m_pDummyItem->GetTargetEffect() );
	SetWin_Text ( this, IDC_EDIT_GENERAL_EFFECTFILE, m_pDummyItem->GetGeneralEffect() );

	SetWin_Text ( this, IDC_EDIT_DECOMPOSEITEM,  m_pDummyItem->sSplit.szName );
	// ����ī�� ����?
	SetWin_Check ( this, IDC_CHECK_UNABLE_REMODEL_CARD, m_pDummyItem->sRandomOpt.bUnableRemodelCard );
	

	SetWin_Text ( this, IDC_EDIT_GRINDING_OPT, m_pDummyItem->sGrindingOp.strGRINDER_OPT_FILE.c_str() );
	SetWin_Text ( this, IDC_EDIT_RANDOM_OPT, m_pDummyItem->sRandomOpt.szNAME );
	SetWin_Text ( this, IDC_EDIT_PERIODEXTENSION, m_pDummyItem->sBasicOp.strItemPeriodExtension.c_str() );
						
	SetWin_Num_int ( this, IDC_EDIT_MAP_MID, m_pDummyItem->sBasicOp.sSubID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_MAP_SID, m_pDummyItem->sBasicOp.sSubID.wSubID );
	SetWin_Num_int ( this, IDC_EDIT_MAP_POSX, m_pDummyItem->sBasicOp.wPosX );
	SetWin_Num_int ( this, IDC_EDIT_MAP_POSY, m_pDummyItem->sBasicOp.wPosY );

	SetWin_Num_int ( this, IDC_EDIT_COOLTIME, m_pDummyItem->sBasicOp.dwCoolTime );

	SetWin_Check( this, IDC_CHECK_SEARCH, m_pDummyItem->sBasicOp.bSearch ); // by luxes.

	if( m_pDummyItem->sBasicOp.emItemType == ITEM_PRODUCT_BOOK )
	{
		SetWin_Num_int( this, IDC_EDIT_PRODUCT_BOOK, m_pDummyItem->sSkillBookOp.sSkill_ID.dwID );
	}
	else
	{
		SetWin_Num_int( this, IDC_EDIT_SKILL_MID, m_pDummyItem->sSkillBookOp.sSkill_ID.wMainID );
		SetWin_Num_int( this, IDC_EDIT_SKILL_SID, m_pDummyItem->sSkillBookOp.sSkill_ID.wSubID );
	}

	SetWin_Num_int ( this, IDC_EDIT_SKILL_LEVEL, m_pDummyItem->sSkillBookOp.wSkill_Level+1    );

    // ������
    SetWin_Num_int( this, IDC_EDIT_DURABILITY_MAX, m_pDummyItem->sSuitOp.wDurabilityMax ); // �ִ� ������
    SetWin_Num_int( this, IDC_EDIT_REPAIR_PRICE, m_pDummyItem->sSuitOp.dwRepairPrice ); // 1����Ʈ ������
	const SSETITEM* temp_SetItem(GLItemSet::GetInstance().GetInfoSetItem(m_pDummyItem->sBasicOp.sNativeID));
	if(temp_SetItem != NULL)
	{	
		if(temp_SetItem->dwSetID != 0)
		{
			SetWin_Num_int(this, IDC_EDIT_SETID, (int)temp_SetItem->dwSetID);
			SetWin_Text(this, IDC_EDIT_SETITEM_NAME, temp_SetItem->strName.c_str());
		}
	}
	else
	{
		SetWin_Num_int(this, IDC_EDIT_SETID, 65535);
		SetWin_Text(this, IDC_EDIT_SETITEM_NAME, NULL);
	}
	SetInvenIconImage();
}

BOOL CItemParent::InverseUpdateItems ()
{
	//	���� ID
	m_pDummyItem->sBasicOp.sNativeID.wMainID = GetWin_Num_int ( this, IDC_EDIT_MID );
	m_pDummyItem->sBasicOp.sNativeID.wSubID = GetWin_Num_int ( this, IDC_EDIT_SID );

	//	������ �̸�
	strKeyName = GetWin_Text ( this, IDC_EDIT_ITEMNAME ).GetString();
	strName	= GetWin_Text ( this, IDC_EDIT_NAME ).GetString();

	//	������ ����
	m_pDummyItem->sBasicOp.emLevel = (EMITEMLEVEL) GetWin_Combo_Sel ( this, IDC_COMBO_LEVEL );

	//	Trade
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SELLENABLE ), m_pDummyItem->sBasicOp.dwFlags, TRADE_SALE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EXCHANGE ), m_pDummyItem->sBasicOp.dwFlags, TRADE_EXCHANGE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_STORAGE ), m_pDummyItem->sBasicOp.dwFlags, LIMIT_LOCKER );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_THROW ), m_pDummyItem->sBasicOp.dwFlags, TRADE_THROW );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_GARBAGE ), m_pDummyItem->sBasicOp.dwFlags, TRADE_GARBAGE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EVENT_SGL ), m_pDummyItem->sBasicOp.dwFlags, TRADE_EVENT_SGL );

    m_pDummyItem->sBasicOp.nRPTrade =  GetWin_Num_int( this, IDC_EDIT_POINT_TR_COUNT );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_RESTRICT ), m_pDummyItem->sBasicOp.dwFlags, ITEM_RESTRICT ); // by luxes.
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_DISGUISE ), m_pDummyItem->sBasicOp.dwFlags, ITEM_DISGUISE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TIMELMT ), m_pDummyItem->sBasicOp.dwFlags, ITEM_TIMELMT );
    SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_BOX_WRAPPABLE ), m_pDummyItem->sBasicOp.dwFlags, ITEM_BOX_WRAPPABLE );  // �������� ���� ���� ����;
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SPLIT ), m_pDummyItem->sBasicOp.dwFlags, ITEM_SPLIT );  // ���� ���� ����;

    m_pDummyItem->sBox.ShowContents = GetWin_Check( this, IDC_CHECK_SHOW_CONTENTS ) ? true : false;

	//	���԰���
	m_pDummyItem->sBasicOp.dwBuyPrice  = GetWin_Num_LONGLONG ( this, IDC_EDIT_PRICE );
	//  �ǸŰ���
	m_pDummyItem->sBasicOp.dwSellPrice = GetWin_Num_LONGLONG ( this, IDC_EDIT_PRICE2 );

	//	������ ���� ��Ģ
	m_pDummyItem->sGenerateOp.dwSpecID = GetWin_Num_int ( this, IDC_EDIT_SPECID );
	m_pDummyItem->sGenerateOp.dwLimitTime = GetWin_Num_int ( this, IDC_EDIT_TIME_SET );
	m_pDummyItem->sGenerateOp.dwLimitTimeGen = GetWin_Num_int ( this, IDC_EDIT_TIME_LIMIT );

	//	������ �ε���.
	//m_pDummyItem->sBasicOp.sICONID.wMainID = GetWin_Num_int ( this, IDC_EDIT_INVENICON_X );
	//m_pDummyItem->sBasicOp.sICONID.wSubID = GetWin_Num_int ( this, IDC_EDIT_INVENICON_Y );

	//	���� ����.
	m_pDummyItem->sBasicOp.strFieldFile = GetWin_Text ( this, IDC_EDIT_FIELDFILE ).GetString();
	m_pDummyItem->sBasicOp.strInventoryFile = GetWin_Text ( this, IDC_EDIT_INVENFILE ).GetString();

	strKeyDesc = GetWin_Text ( this, IDC_EDIT_ITEMDESC ).GetString();
	CString strComment = GetWin_Text ( this, IDC_EDIT_COMMENT );
	if ( strComment.GetLength()+1 < ITEM_SZCOMMENT ) strDesc = strComment.GetString();

	//	��, ��� �Ӽ�.
	if ( GetWin_Check ( this, IDC_RADIO_BRIGHT_LIGHT) )	m_pDummyItem->sBasicOp.emReqBright = BRIGHT_LIGHT;
	if ( GetWin_Check ( this, IDC_RADIO_BRIGHT_DARK ) )	m_pDummyItem->sBasicOp.emReqBright = BRIGHT_DARK;
	if ( GetWin_Check ( this, IDC_RADIO_BRIGHT_BOTH ) )	m_pDummyItem->sBasicOp.emReqBright = BRIGHT_BOTH;

	//	��밡�� ĳ����.
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_FIGHTER_M ),m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_FIGHTER_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARMS_M ),   m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ARMS_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARCHER_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ARCHER_W );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SPIRIT_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_SPIRIT_W );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_FIGHTER_W ),m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_FIGHTER_W );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARMS_W ),   m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ARMS_W );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARCHER_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ARCHER_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SPIRIT_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_SPIRIT_M );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EXTREME_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_EXTREME_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EXTREME_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_EXTREME_W );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCIENTIST_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_SCIENTIST_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCIENTIST_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_SCIENTIST_W );

    SetCheck_Flags( GetWin_Check ( this, IDC_CHECK_ASSASSIN_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ASSASSIN_M );
    SetCheck_Flags( GetWin_Check ( this, IDC_CHECK_ASSASSIN_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ASSASSIN_W );

    SetCheck_Flags( GetWin_Check ( this, IDC_CHECK_TRICKER_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_TRICKER_M );
    SetCheck_Flags( GetWin_Check ( this, IDC_CHECK_TRICKER_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_TRICKER_W );

	SetCheck_Flags( GetWin_Check ( this, IDC_CHECK_ACTOR_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ACTOR_M );
	SetCheck_Flags( GetWin_Check ( this, IDC_CHECK_ACTOR_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ACTOR_W );

	//	��밡�� �п�.
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCHOOL1 ), m_pDummyItem->sBasicOp.dwReqSchool, GLSCHOOL_00 );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCHOOL2 ), m_pDummyItem->sBasicOp.dwReqSchool, GLSCHOOL_01 );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCHOOL3 ), m_pDummyItem->sBasicOp.dwReqSchool, GLSCHOOL_02 );

	if ( m_pDummyItem->sBasicOp.dwReqCharClass == GLCC_NONE )
	{
		MessageBox ( "An usable class is not selected.", "Caution", MB_OK | MB_ICONEXCLAMATION );
		return FALSE;
	}

	//	�䱸 ��ġ.
	m_pDummyItem->sBasicOp.wReqLevelDW = GetWin_Num_int ( this, IDC_EDIT_NEEDLEVELDW );
	m_pDummyItem->sBasicOp.wReqLevelUP = GetWin_Num_int ( this, IDC_EDIT_NEEDLEVELUP );

    m_pDummyItem->sBasicOp.emReqActivityType = GetWin_Combo_Sel ( this, IDC_COMBO_ACTIVITY_TYPE );
    m_pDummyItem->sBasicOp.wReqActPointDW = GetWin_Num_int ( this, IDC_EDIT_NEED_ACT_POINT_DW );
	m_pDummyItem->sBasicOp.wReqActPointUP = GetWin_Num_int ( this, IDC_EDIT_NEED_ACT_POINT_UP );

	m_pDummyItem->sBasicOp.dwReqContribution = (DWORD) GetWin_Num_int ( this, IDC_EDIT_NEED_CONTRIBUTION_POINT );

	m_pDummyItem->sBasicOp.sReqStats.wPow = GetWin_Num_int ( this, IDC_EDIT_POWER );
	m_pDummyItem->sBasicOp.sReqStats.wStr = GetWin_Num_int ( this, IDC_EDIT_STRENGTH );
	m_pDummyItem->sBasicOp.sReqStats.wSpi = GetWin_Num_int ( this, IDC_EDIT_SPIRIT );
	m_pDummyItem->sBasicOp.sReqStats.wDex = GetWin_Num_int ( this, IDC_EDIT_DEXTERITY );
	m_pDummyItem->sBasicOp.sReqStats.wInt = GetWin_Num_int ( this, IDC_EDIT_INTELLIGENT );
	m_pDummyItem->sBasicOp.sReqStats.wSta = GetWin_Num_int ( this, IDC_EDIT_STAMINA );

	m_pDummyItem->sBasicOp.wReqPA = GetWin_Num_int ( this, IDC_EDIT_REQ_PA );
	m_pDummyItem->sBasicOp.wReqSA = GetWin_Num_int ( this, IDC_EDIT_REQ_SA );

	//	������ ����.
	m_pDummyItem->sBasicOp.emCoolType = (EMCOOL_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_COOL_TYPE );

	//	���� ���� Ƚ��.
	m_pDummyItem->sSuitOp.wReModelNum = GetWin_Num_int ( this, IDC_EDIT_REMODEL_NUM );
	if ( m_pDummyItem->sSuitOp.wReModelNum > 0xff )
		m_pDummyItem->sSuitOp.wReModelNum = 0xff;
	
	//	�Ҹ���.
	m_pDummyItem->sDrugOp.bInstance = GetWin_Check ( this, IDC_CHECK_INSTANCE );

	LONG lDays = GetWin_Num_int ( this, IDC_EDIT_LMT_D );
	int nHours = GetWin_Num_int ( this, IDC_EDIT_LMT_H );
	int nMins = GetWin_Num_int ( this, IDC_EDIT_LMT_M );
	
	CTimeSpan cSPAN(lDays,nHours,nMins,0);
	m_pDummyItem->sDrugOp.tTIME_LMT = cSPAN.GetTimeSpan();

	m_pDummyItem->sBasicOp.strSelfBodyEffect = GetWin_Text(this,IDC_EDIT_SKIN_EFFECTFILE).GetString();
	m_pDummyItem->sBasicOp.strTargBodyEffect = GetWin_Text(this,IDC_EDIT_TAR_EFFECTFILE).GetString();
	m_pDummyItem->sBasicOp.strTargetEffect = GetWin_Text(this,IDC_EDIT_EFFECTFILE).GetString();
	m_pDummyItem->sBasicOp.strGeneralEffect = GetWin_Text(this,IDC_EDIT_GENERAL_EFFECTFILE).GetString();

	StringCchCopy ( m_pDummyItem->sRandomOpt.szNAME, RandomOption::LENGTH_NAME, GetWin_Text(this,IDC_EDIT_RANDOM_OPT).GetString() );
	StringCchCopy ( m_pDummyItem->sSplit.szName, ITEM_SZNAME, GetWin_Text(this,IDC_EDIT_DECOMPOSEITEM).GetString() );
	
	m_pDummyItem->sBasicOp.strItemPeriodExtension = GetWin_Text(this,IDC_EDIT_PERIODEXTENSION).GetString();

	// ����ī�� ����?
	 m_pDummyItem->sRandomOpt.bUnableRemodelCard = GetWin_Check ( this, IDC_CHECK_UNABLE_REMODEL_CARD );
	//
	m_pDummyItem->sGrindingOp.strGRINDER_OPT_FILE = GetWin_Text ( this, IDC_EDIT_GRINDING_OPT ).GetString();

	m_pDummyItem->sBasicOp.sSubID.wMainID = GetWin_Num_int ( this, IDC_EDIT_MAP_MID );
	m_pDummyItem->sBasicOp.sSubID.wSubID = GetWin_Num_int ( this, IDC_EDIT_MAP_SID );
	m_pDummyItem->sBasicOp.wPosX = GetWin_Num_int ( this, IDC_EDIT_MAP_POSX );
	m_pDummyItem->sBasicOp.wPosY = GetWin_Num_int ( this, IDC_EDIT_MAP_POSY );

	m_pDummyItem->sBasicOp.dwCoolTime = GetWin_Num_int ( this, IDC_EDIT_COOLTIME );

    m_pDummyItem->sBasicOp.bSearch = GetWin_Check ( this, IDC_CHECK_SEARCH ) ? true : false; // by luxes.

	if( m_pDummyItem->sBasicOp.emItemType == ITEM_PRODUCT_BOOK )
	{
		m_pDummyItem->sSkillBookOp.sSkill_ID.dwID = GetWin_Num_int( this, IDC_EDIT_PRODUCT_BOOK );
	}
	else
	{
		m_pDummyItem->sSkillBookOp.sSkill_ID.wMainID = GetWin_Num_int( this, IDC_EDIT_SKILL_MID );
		m_pDummyItem->sSkillBookOp.sSkill_ID.wSubID = GetWin_Num_int( this, IDC_EDIT_SKILL_SID );
	}

	WORD wSkillLevel = GetWin_Num_int ( this, IDC_EDIT_SKILL_LEVEL );

	if ( wSkillLevel > SKILL::MAX_LEVEL )
	{
		wSkillLevel = SKILL::MAX_LEVEL - 1;
	}
	else if ( wSkillLevel <= 0 )
	{
		wSkillLevel = 0;
	}
	else
	{
		wSkillLevel -= 1;
	}

	m_pDummyItem->sSkillBookOp.wSkill_Level = wSkillLevel;

    // ������
    m_pDummyItem->sSuitOp.wDurabilityMax = GetWin_Num_int( this, IDC_EDIT_DURABILITY_MAX ); // �ִ� ������
    m_pDummyItem->sSuitOp.dwRepairPrice = GetWin_Num_int( this, IDC_EDIT_REPAIR_PRICE ); // 1����Ʈ ������

	return TRUE;
}

void CItemParent::OnBnClickedButtonNext()
{
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//if ( !InverseUpdateItems() )
	//{
	//	return;
	//}

	//CItemNode sItemNode;
	//sItemNode.m_sItem = *m_pDummyItem;

	//GLItemMan::GetInstance().InsertItem ( m_pDummyItem->sBasicOp.sNativeID, &sItemNode );

	//GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::ITEM );
	//GLStringTable::GetInstance().InsertString ( strKeyDesc, strDesc, GLStringTable::ITEM );

	m_pSheetTab->ActiveItemSuitPage ( m_CallPage, m_pItem );
	m_pItem = NULL;
}

void CItemParent::InitDefaultCtrls ()
{
	GetDlgItem ( IDC_BUTTON_PREV )->EnableWindow ( FALSE );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strITEMLEVEL[EMITEM_LEVEL_NSIZE], strCOOLTYPE[EMCOOL_SIZE], strACTIVITY[ACTIVITY_CLASS_SIZE];	
		CGameTextMan::GetInstance().LoadHelpCommentString("ITEMLEVEL", strITEMLEVEL, CGameTextMan::EM_COMMENT_TEXT, EMITEM_LEVEL_NSIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("COOLTYPE", strCOOLTYPE, CGameTextMan::EM_COMMENT_TEXT, EMCOOL_SIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("ACTIVITY_CLASS", strACTIVITY, CGameTextMan::EM_COMMENT_TEXT, ACTIVITY_CLASS_SIZE);
		SetWin_Combo_Init ( this, IDC_COMBO_LEVEL,		strITEMLEVEL,	EMITEM_LEVEL_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_COOL_TYPE,	strCOOLTYPE,	EMCOOL_SIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_ACTIVITY_TYPE, strACTIVITY,	ACTIVITY_CLASS_SIZE );
	}
	else
	{
		SetWin_Combo_Init ( this, IDC_COMBO_LEVEL, COMMENT::ITEMLEVEL, EMITEM_LEVEL_NSIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_COOL_TYPE, COMMENT::COOLTYPE, EMCOOL_SIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_ACTIVITY_TYPE, COMMENT::ACTIVITY_CLASS, ACTIVITY_CLASS_SIZE );
	}
}

void CItemParent::SetInvenIconImage()
{
	CString strFileName = GetWin_Text( this, IDC_EDIT_INVENFILE );
	m_pIconPreview->SetFileName( strFileName );

	//SNATIVEID sICONID;
	//sICONID.wMainID = GetWin_Num_int( this, IDC_EDIT_INVENICON_X );
	//sICONID.wSubID = GetWin_Num_int( this, IDC_EDIT_INVENICON_Y );
	//m_pIconPreview->SetIconID( sICONID );
}

void CItemParent::OnBnClickedButtonInvenfile()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg,*.png)|*.bmp;*.dds;*.tga;*.jpg;*.png|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".png",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemParent*)this);

	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() != IDOK )
		return;

	SetWin_Text ( this, IDC_EDIT_INVENFILE, dlg.GetFileName().GetString() );

	SetInvenIconImage();
}

void CItemParent::OnBnClickedButtonFieldfile()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Item File (*.x)|*.x|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemParent*)this);

	dlg.m_ofn.lpstrInitialDir = DxSimpleMeshMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_FIELDFILE, dlg.GetFileName().GetString() );
	}
}

void CItemParent::OnBnClickedButtonSkinEffectfile()
{
	CString szFilter = "Effect File (*.effskin)|*.effskin|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".effskin",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_SKIN_EFFECTFILE )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CItemParent::OnBnClickedButtonEffectfile()
{
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_EFFECTFILE )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CItemParent::OnBnClickedButtonSkinTarEffectfile()
{
	CString szFilter = "Effect File (*.effskin_a)|*.effskin_a|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_TAR_EFFECTFILE )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CItemParent::OnBnClickedButtonGeneralEffectfile()
{
	CString szFilter = "Effect File (*.egp)|*.egp|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_GENERAL_EFFECTFILE )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}
void CItemParent::OnEnChangeEditItemname()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	std::string szTemp;
 	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
 	{
 		szTemp = ID2ITEMTEXT( GetWin_Text( this, IDC_EDIT_ITEMNAME ).GetString() );
 	}
 	else
 	{
		szTemp =  GLStringTable::GetInstance().GetString(
			GetWin_Text( this, IDC_EDIT_ITEMNAME ).GetString(),
			GLStringTable::ITEM);
	}

	SetWin_Text ( this, IDC_EDIT_NAME, szTemp.empty() == false ? szTemp.c_str() : "");
}

void CItemParent::OnEnChangeEditItemdesc()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
 	std::string szTemp;
 	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
 	{
 		szTemp = ID2ITEMTEXT( GetWin_Text( this, IDC_EDIT_ITEMDESC ).GetString() );
 	}
 	else
 	{
		szTemp = GLStringTable::GetInstance().GetString(
			GetWin_Text( this, IDC_EDIT_ITEMDESC ).GetString(),
			GLStringTable::ITEM);
	}

	SetWin_Text ( this, IDC_EDIT_COMMENT, szTemp.empty() == false ? szTemp.c_str() : "");
}

void CItemParent::SetReadMode ()
{
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM

	const int nSkipNum = 7;
	const int nSkipID[nSkipNum] = { IDC_EDIT_NAME, IDC_EDIT_COMMENT, IDC_BUTTON_PREV,
									IDC_BUTTON_NEXT, IDC_BUTTON_CANCEL, IDC_BUTTON_OK, IDC_BUTTON_WEARING_SET };

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

void CItemParent::OnBnClickedRadioAll()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_ALL );

	SetWin_Check( this, IDC_CHECK_FIGHTER_M, bCheck );
	SetWin_Check( this, IDC_CHECK_ARMS_M, bCheck );
	SetWin_Check( this, IDC_CHECK_ARCHER_W, bCheck );
	SetWin_Check( this, IDC_CHECK_SPIRIT_W, bCheck );
	
	SetWin_Check( this, IDC_CHECK_FIGHTER_W, bCheck );
	SetWin_Check( this, IDC_CHECK_ARMS_W, bCheck );
	SetWin_Check( this, IDC_CHECK_ARCHER_M, bCheck );
	SetWin_Check( this, IDC_CHECK_SPIRIT_M, bCheck );
	
	SetWin_Check( this, IDC_CHECK_EXTREME_M, bCheck );
	SetWin_Check( this, IDC_CHECK_EXTREME_W, bCheck );

	SetWin_Check( this, IDC_CHECK_SCIENTIST_M, bCheck );
	SetWin_Check( this, IDC_CHECK_SCIENTIST_W, bCheck );

    SetWin_Check( this, IDC_CHECK_ASSASSIN_M, bCheck );
    SetWin_Check( this, IDC_CHECK_ASSASSIN_W, bCheck );

    SetWin_Check( this, IDC_CHECK_TRICKER_M, bCheck );
    SetWin_Check( this, IDC_CHECK_TRICKER_W, bCheck );

	SetWin_Check( this, IDC_CHECK_ACTOR_M, bCheck );
	SetWin_Check( this, IDC_CHECK_ACTOR_W, bCheck );
}

void CItemParent::OnBnClickedRadioMan()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_MAN );

	SetWin_Check( this, IDC_CHECK_ARCHER_W, false );
	SetWin_Check( this, IDC_CHECK_SPIRIT_W, false );	
	SetWin_Check( this, IDC_CHECK_FIGHTER_W, false );
	SetWin_Check( this, IDC_CHECK_ARMS_W, false );	
	SetWin_Check( this, IDC_CHECK_EXTREME_W, false );
	SetWin_Check( this, IDC_CHECK_SCIENTIST_W, false );
    SetWin_Check( this, IDC_CHECK_ASSASSIN_W, false );
    SetWin_Check( this, IDC_CHECK_TRICKER_W, false );
	SetWin_Check( this, IDC_CHECK_ACTOR_W, false );

	SetWin_Check( this, IDC_CHECK_FIGHTER_M, bCheck );
	SetWin_Check( this, IDC_CHECK_ARMS_M, bCheck );	
	SetWin_Check( this, IDC_CHECK_ARCHER_M, bCheck );
	SetWin_Check( this, IDC_CHECK_SPIRIT_M, bCheck );	
	SetWin_Check( this, IDC_CHECK_EXTREME_M, bCheck );
	SetWin_Check( this, IDC_CHECK_SCIENTIST_M, bCheck );	
    SetWin_Check( this, IDC_CHECK_ASSASSIN_M, bCheck );
    SetWin_Check( this, IDC_CHECK_TRICKER_M, bCheck );
	SetWin_Check( this, IDC_CHECK_ACTOR_M, bCheck );

}

void CItemParent::OnBnClickedRadioWoman()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_WOMAN );

	SetWin_Check( this, IDC_CHECK_ARCHER_W, bCheck );
	SetWin_Check( this, IDC_CHECK_SPIRIT_W, bCheck );	
	SetWin_Check( this, IDC_CHECK_FIGHTER_W, bCheck );
	SetWin_Check( this, IDC_CHECK_ARMS_W, bCheck );
	SetWin_Check( this, IDC_CHECK_EXTREME_W, bCheck );
	SetWin_Check( this, IDC_CHECK_SCIENTIST_W, bCheck );
    SetWin_Check( this, IDC_CHECK_ASSASSIN_W, bCheck );
    SetWin_Check( this, IDC_CHECK_TRICKER_W, bCheck );
	SetWin_Check( this, IDC_CHECK_ACTOR_W, bCheck );

	SetWin_Check( this, IDC_CHECK_FIGHTER_M, false );
	SetWin_Check( this, IDC_CHECK_ARMS_M, false );	
	SetWin_Check( this, IDC_CHECK_ARCHER_M, false );
	SetWin_Check( this, IDC_CHECK_SPIRIT_M, false );	
	SetWin_Check( this, IDC_CHECK_EXTREME_M, false );
	SetWin_Check( this, IDC_CHECK_SCIENTIST_M, false );
    SetWin_Check( this, IDC_CHECK_ASSASSIN_M, false );
    SetWin_Check( this, IDC_CHECK_TRICKER_M, false );
	SetWin_Check( this, IDC_CHECK_ACTOR_M, false );
}

void CItemParent::OnBnClickedButtonGrindingSimulator()
{
	m_pDummyItem->sGrindingOp.strGRINDER_OPT_FILE = GetWin_Text ( this, IDC_EDIT_GRINDING_OPT ).GetString();

	CGrindingSimulatorDlg dlg;
	dlg.SetGrinder( m_pDummyItem );
	dlg.DoModal();
}

void CItemParent::OnBnClickedButtonItemType()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
    CSelectDialog dlg;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strITEMTYPE[ITEM_NSIZE];	
		CGameTextMan::GetInstance().LoadHelpCommentString("ITEMTYPE", strITEMTYPE, CGameTextMan::EM_COMMENT_TEXT, ITEM_NSIZE);
		dlg.SetStringList( strITEMTYPE, ITEM_NSIZE );	
	}
	else
	{
		dlg.SetStringList( COMMENT::ITEMTYPE, ITEM_NSIZE );	
	}
	dlg.m_nSelect = m_pDummyItem->sBasicOp.emItemType;

	if ( dlg.DoModal() == IDOK )
	{
		InverseUpdateItems();
		m_pDummyItem->sBasicOp.emItemType = (EMITEM_TYPE) dlg.m_dwData;
		UpdateItems();
	}

	return;
}

void CItemParent::OnBnClickedButtonWearingSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWearingDlg dlg;

	dlg.m_emSuit = m_pDummyItem->sSuitOp.emSuit;
	dlg.m_emVehicleType = m_pDummyItem->sVehicle.emVehicleType;

	for ( int i = 0; i < GLCI_NUM_ACTOR; ++i )
	{
		dlg.m_strArrInventoryFile[i] = m_pDummyItem->sBasicOp.strArrInventoryFile[i];
	}

	for ( int i = 0; i < GLCI_NUM_ACTOR; ++i ) 
	{
		dlg.m_strWearingFile[i] = m_pDummyItem->sBasicOp.strWearingFile[i];
	}

	for ( int i = 0; i < GLCI_NUM_ACTOR; ++i ) 
	{
		dlg.m_strWearingFileEx[i] = m_pDummyItem->sBasicOp.strWearingFileEx[i];
	}
	
	dlg.m_strPetWearingFile = m_pDummyItem->sBasicOp.strPetWearingFile;
	if ( dlg.DoModal() == IDOK )
	{
		for ( int i = 0; i < GLCI_NUM_ACTOR; ++i ) 
		{
			m_pDummyItem->sBasicOp.strArrInventoryFile[i] = dlg.m_strArrInventoryFile[i];
		}

		for ( int i = 0; i < GLCI_NUM_ACTOR; ++i ) 
		{
			m_pDummyItem->sBasicOp.strWearingFile[i] = dlg.m_strWearingFile[i];
		}

		for ( int i = 0; i < GLCI_NUM_ACTOR; ++i ) 
		{
			m_pDummyItem->sBasicOp.strWearingFileEx[i] = dlg.m_strWearingFileEx[i];
		}
		
		m_pDummyItem->sBasicOp.strPetWearingFile = dlg.m_strPetWearingFile;
	}
	return;
}

void CItemParent::OnEnChangeEditInveniconX()
{
	SetInvenIconImage();
}

void CItemParent::OnEnChangeEditInveniconY()
{
	SetInvenIconImage();
}

void CItemParent::OnTimer(UINT_PTR nIDEvent)
{
    switch(nIDEvent)
    {
    case TIMER_ITEMPARENT_SAVE_STATE_ID:

        SetWin_Text ( this, IDC_EDIT_SAVE_STATE, "" );
        KillTimer ( TIMER_ITEMPARENT_SAVE_STATE_ID );

        break;
    }

    CPropertyPage::OnTimer(nIDEvent);
}

void CItemParent::OnBnClickedButtonCheckInfo()
{
	SetItemInfo dlg;
	dlg.SetNativeID(&m_pDummyItem->sBasicOp.sNativeID);
	dlg.DoModal();
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CItemParent::OnBnClickedButtonDecompose()
{
	CString szFilter = "GenItem Setting File (*.genitem)|*.genitem|";

	CFileDialog dlg(TRUE,".genitem",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_DECOMPOSEITEM,  dlg.GetFileName().GetString() );
	}
}