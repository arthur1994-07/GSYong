// PropertyChaPutOnItem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../../MainFrm.h"
#include "../../GMCharEdit.h"
#include "./PropertyChaPutOnItem.h"
#include "../PropertySheetCharEdit.h"
#include "../../SubDialogs/DlgInvenAdd.h"

#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../RanLogicServer/DxServerInstance.h"
#include "../../RanLogicServer/Database/DbDefineLog.h"

#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/Item/GLItem.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../../Util/CGmCfg.h"

#include "../../Database/ADO/ADOManager.h"
#include "../../Database/ODBC/CGMOdbcManager.h"

#include ".\propertychaputonitem.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CPropertyChaPutOnItem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropertyChaPutOnItem, CPropertyPage)
CPropertyChaPutOnItem::CPropertyChaPutOnItem()
	: CPropertyPage(CPropertyChaPutOnItem::IDD)
	, m_nAvoidRate(0)
	, m_nDamage(0)
	, m_nDefense(0)

	, m_wDefenceStat ( 0 )

	, m_nHitRate(0)
	, m_nElectric(0)
	, m_nFire(0)
	, m_nIce(0)
	, m_nPoison(0)
	, m_nSpirit(0)

	, m_wReModel(0)
	, m_wUsedNum(0)
	, m_nRadio(0)
	, m_nMainID(0)
	, m_nSubID(0)
	, m_strName(_T(""))

	, m_strGenID("")

	, m_dwCOSTUME_MID(0)
	, m_dwCOSTUME_SID(0)

	, m_sLinkSkillID ( false )
	, m_wLinkSkillLevel ( 0 )
	, m_fLinkSkillOccurRate ( 0.f )

// 	, m_nRandomOpt1(0)
// 	, m_nRandomOpt2(0)
// 	, m_nRandomOpt3(0)
// 	, m_nRandomOpt4(0)
	, m_strCosTime(_T(""))
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CPropertyChaPutOnItem::~CPropertyChaPutOnItem()
{
}

void CPropertyChaPutOnItem::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

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

	DDX_Text(pDX, IDC_EDIT9, m_wUsedNum);

	DDX_Text(pDX, IDC_EDIT1, m_nMainID);
	DDX_Text(pDX, IDC_EDIT7, m_nSubID);
	DDX_Text(pDX, IDC_EDIT2, m_strName);
	DDX_Control(pDX, IDC_EDIT11, m_editTIME);

	DDX_Text(pDX, IDC_EDIT14, m_strGenID);

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

	DDX_Control(pDX, IDC_COMBO_RANDOM_OPT1, m_comboRandomOpt1);
	DDX_Control(pDX, IDC_COMBO_RANDOM_OPT2, m_comboRandomOpt2);
	DDX_Control(pDX, IDC_COMBO_RANDOM_OPT3, m_comboRandomOpt3);
	DDX_Control(pDX, IDC_COMBO_RANDOM_OPT4, m_comboRandomOpt4);
	DDX_Control(pDX, IDC_COMBO_RANDOM_OPT5, m_comboRandomOpt5);
	DDX_Control(pDX, IDC_COMBO_ADD_OPT1, m_comboAddOpt1);
	DDX_Control(pDX, IDC_COMBO_ADD_OPT2, m_comboAddOpt2);
	DDX_Control(pDX, IDC_COMBO_ADD_OPT3, m_comboAddOpt3);
	DDX_Control(pDX, IDC_COMBO_ADD_OPT4, m_comboAddOpt4);
	DDX_Control(pDX, IDC_COMBO_ADD_OPT5, m_comboAddOpt5);
	DDX_Control(pDX, IDC_COMBO_ADD_OPT6, m_comboAddOpt6);
// 	DDX_Text(pDX, IDC_EDIT_RANDOM_OPT1, m_nRandomOpt1);
// 	DDX_Text(pDX, IDC_EDIT_RANDOM_OPT2, m_nRandomOpt2);
// 	DDX_Text(pDX, IDC_EDIT_RANDOM_OPT3, m_nRandomOpt3);
// 	DDX_Text(pDX, IDC_EDIT_RANDOM_OPT4, m_nRandomOpt4);
	DDX_Text(pDX, IDC_EDIT12, m_strCosTime);
}


BEGIN_MESSAGE_MAP(CPropertyChaPutOnItem, CPropertyPage)
	
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)	
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_RADIO6, OnBnClickedRadio6)
	ON_BN_CLICKED(IDC_RADIO7, OnBnClickedRadio7)
	ON_BN_CLICKED(IDC_RADIO8, OnBnClickedRadio8)
	ON_BN_CLICKED(IDC_RADIO9, OnBnClickedRadio9)	
	ON_BN_CLICKED(IDC_RADIO10, OnBnClickedRadio10)
	ON_BN_CLICKED(IDC_RADIO11, OnBnClickedRadio11)	
	ON_BN_CLICKED(IDC_RADIO12, OnBnClickedRadio12)
	ON_BN_CLICKED(IDC_RADIO13, OnBnClickedRadio13)
	ON_BN_CLICKED(IDC_RADIO14, OnBnClickedRadio14)
	ON_BN_CLICKED(IDC_RADIO15, OnBnClickedRadio15)
	ON_BN_CLICKED(IDC_RADIO16, OnBnClickedRadio16)
	ON_BN_CLICKED(IDC_RADIO17, OnBnClickedRadio17)
	ON_BN_CLICKED(IDC_RADIO18, OnBnClickedRadio18)
	ON_BN_CLICKED(IDC_RADIO19, OnBnClickedRadio19)

	ON_BN_CLICKED(IDC_BTN_PUTON_DELETE, OnBnClickedBtnPutonDelete)
	ON_BN_CLICKED(IDC_BTN_PUTON_CHANGE, OnBnClickedBtnPutonChange)
	ON_BN_CLICKED(IDC_BTN_PUTONITEM_SAVE, OnBnClickedBtnPutonitemSave)
	ON_BN_CLICKED(IDC_BTN_DISGUISE, OnBnClickedBtnDisguise)	
	ON_BN_CLICKED(IDC_BTN_PUTON_Save, &CPropertyChaPutOnItem::OnBnClickedBtnPutonApply)
END_MESSAGE_MAP()


// CPropertyChaPutOnItem 메시지 처리기입니다.

BOOL CPropertyChaPutOnItem::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();

	GetData(pSheet->m_pChar);
	SetData();

	CButton* pRadio = (CButton*) GetDlgItem(IDC_RADIO2);
	pRadio->SetCheck(BST_CHECKED);

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
	}

	// Add Option;
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

	// Random Option;
	m_comboRandomOpt1.ResetContent();
	m_comboRandomOpt2.ResetContent();
	m_comboRandomOpt3.ResetContent();
	m_comboRandomOpt4.ResetContent();
	m_comboRandomOpt5.ResetContent();
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
		else
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
	}

	UpdateData(FALSE);

	OnBnClickedRadio2();

	if( pSheet->m_pChar->m_emClass == GLCC_EXTREME_M || 
		pSheet->m_pChar->m_emClass == GLCC_EXTREME_W )
	{
		CButton* pExRightHand = (CButton*) GetDlgItem( IDC_RADIO12 );
		pExRightHand->EnableWindow( TRUE );

		CButton* pExLeftHand = (CButton*) GetDlgItem( IDC_RADIO13 );
		pExLeftHand->EnableWindow( TRUE );
	}
	else
	{
		CButton* pExRightHand = (CButton*) GetDlgItem( IDC_RADIO12 );
		pExRightHand->EnableWindow( FALSE );

		CButton* pExLeftHand = (CButton*) GetDlgItem( IDC_RADIO13 );
		pExLeftHand->EnableWindow( FALSE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPropertyChaPutOnItem::GetData(GLCHARLOGIC* pChar)
{
	m_vPutOnItem.erase(m_vPutOnItem.begin(), m_vPutOnItem.end());

	int nMaxSize = EMSLOT(SLOT_TSIZE);

	for (int i=0; i<nMaxSize; i++)
	{
		m_vPutOnItem.push_back(pChar->m_PutOnItems[i]);
	}	
}

void CPropertyChaPutOnItem::SetData( SITEMCUSTOM sITEM )
{  
	SITEMCUSTOM* pItem = &m_vPutOnItem[m_nRadio];

	if( sITEM.GetNativeID() != NATIVEID_NULL() )
		pItem = &sITEM;

	m_comboLinkSkillTarget.EnableWindow(FALSE);

	m_comboAddOpt1.EnableWindow(FALSE);
	m_comboAddOpt2.EnableWindow(FALSE);
	m_comboAddOpt3.EnableWindow(FALSE);
	m_comboAddOpt4.EnableWindow(FALSE);
	m_comboAddOpt5.EnableWindow(FALSE);
	m_comboAddOpt6.EnableWindow(FALSE);

	m_comboRandomOpt1.EnableWindow(FALSE);
	m_comboRandomOpt2.EnableWindow(FALSE);
	m_comboRandomOpt3.EnableWindow(FALSE);
	m_comboRandomOpt4.EnableWindow(FALSE);
	m_comboRandomOpt5.EnableWindow(FALSE);

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		m_comboLinkSkillTarget.SelectString(
			-1, CGameTextMan::GetInstance().GetCommentText(
			"SKILL_LINK_APPLY_TYPE", 0 ).GetString() );
	}
	else
	{
		m_comboLinkSkillTarget.SelectString(
			-1, COMMENT::SKILL_LINK_TYPE[0].c_str() );
	}


	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		m_comboAddOpt1.SelectString(-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", 0).GetString());
		m_comboAddOpt2.SelectString(-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", 0).GetString());
		m_comboAddOpt3.SelectString(-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", 0).GetString());
		m_comboAddOpt4.SelectString(-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", 0).GetString());
		m_comboAddOpt5.SelectString(-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", 0).GetString());
		m_comboAddOpt6.SelectString(-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", 0).GetString());
	}
	else
	{
		m_comboAddOpt1.SelectString ( -1, COMMENT::ITEMADDON[0].c_str() );
		m_comboAddOpt2.SelectString ( -1, COMMENT::ITEMADDON[0].c_str() );
		m_comboAddOpt3.SelectString ( -1, COMMENT::ITEMADDON[0].c_str() );
		m_comboAddOpt4.SelectString ( -1, COMMENT::ITEMADDON[0].c_str() );
		m_comboAddOpt5.SelectString ( -1, COMMENT::ITEMADDON[0].c_str() );
		m_comboAddOpt6.SelectString ( -1, COMMENT::ITEMADDON[0].c_str() );
	}

    // 다른 라디오버튼을 선택할때 초기화가 안되서 추가.
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		m_comboRandomOpt1.SelectString(-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", 0).GetString());
		m_comboRandomOpt2.SelectString(-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", 0).GetString());
		m_comboRandomOpt3.SelectString(-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", 0).GetString());
		m_comboRandomOpt4.SelectString(-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", 0).GetString());
		m_comboRandomOpt5.SelectString(-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", 0).GetString());
	}
	else
	{
		m_comboRandomOpt1.SelectString ( -1, COMMENT::ITEM_RANDOM_OPT[0].c_str() );
		m_comboRandomOpt2.SelectString ( -1, COMMENT::ITEM_RANDOM_OPT[0].c_str() );
		m_comboRandomOpt3.SelectString ( -1, COMMENT::ITEM_RANDOM_OPT[0].c_str() );
		m_comboRandomOpt4.SelectString ( -1, COMMENT::ITEM_RANDOM_OPT[0].c_str() );
		m_comboRandomOpt5.SelectString ( -1, COMMENT::ITEM_RANDOM_OPT[0].c_str() );
	}

	GetDlgItem(IDC_EDIT_DAMAGE_LOW)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_DAMAGE_HIGH)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_DEFENSE)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_LINKSKILL_MID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LINKSKILL_SID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LINKSKILL_LEVEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LINKSKILL_PROBABILITY)->EnableWindow(FALSE);

	SetWin_Num_int( this, IDC_EDIT_ADD_OPT1, 0 );
	SetWin_Num_int( this, IDC_EDIT_ADD_OPT2, 0 );
	SetWin_Num_int( this, IDC_EDIT_ADD_OPT3, 0 );
	SetWin_Num_int( this, IDC_EDIT_ADD_OPT4, 0 );
	SetWin_Num_int( this, IDC_EDIT_ADD_OPT5, 0 );
	SetWin_Num_int( this, IDC_EDIT_ADD_OPT6, 0 );
	GetDlgItem(IDC_EDIT_ADD_OPT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ADD_OPT2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ADD_OPT3)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ADD_OPT4)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ADD_OPT5)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ADD_OPT6)->EnableWindow(FALSE);

	SetWin_Num_int( this, IDC_EDIT_RANDOM_OPT1, 0 );
	SetWin_Num_int( this, IDC_EDIT_RANDOM_OPT2, 0 );
	SetWin_Num_int( this, IDC_EDIT_RANDOM_OPT3, 0 );
	SetWin_Num_int( this, IDC_EDIT_RANDOM_OPT4, 0 );
	SetWin_Num_int( this, IDC_EDIT_RANDOM_OPT5, 0 );
	GetDlgItem(IDC_EDIT_RANDOM_OPT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_RANDOM_OPT2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_RANDOM_OPT3)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_RANDOM_OPT4)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_RANDOM_OPT5)->EnableWindow(FALSE);


	GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT25)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT31)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT24)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT8)->EnableWindow(FALSE);

	// VALID_SLOT_ITEM
	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	
	if( EMSLOT( m_nRadio ) == SLOT_RHAND_S ||
		EMSLOT( m_nRadio ) == SLOT_LHAND_S )
	{
		pSheet->m_pChar->SetUseArmSub( TRUE );
	}
	else
	{
		pSheet->m_pChar->SetUseArmSub( FALSE );
	}
	
	if (pItem->GetNativeID() != NATIVEID_NULL() )
	{
		SITEM *pItemData = GLogicData::GetInstance().GetItem ( pItem->GetNativeID() );
		if ( pItemData == NULL )
		{
			SNATIVEID ErrorItem = pItem->GetNativeID();

			//std::string ErrorLog(
			//	sc::string::format(
			//		"Current data is outdated. You must be try data update,\nThis message cause by ( %d, %d ) item data was not exist.",
			//		ErrorItem.wMainID,
			//		ErrorItem.wSubID)
			//	);
			//	
			CString strErrorLog;
			strErrorLog.Format(
				"Current data is outdated. You must be try data update,\nThis message cause by ( %d, %d ) item data was not exist.",
				ErrorItem.wMainID,
				ErrorItem.wSubID);

			MessageBox( strErrorLog, "ERROR", MB_OK );

			return;
		}
		m_strName = pItemData->GetName();

		m_nMainID = pItem->Mid();
		m_nSubID  = pItem->Sid();

		m_nDamage    = pItem->cDAMAGE;
		m_nDefense   = pItem->cDEFENSE;

		m_sDamageStat = pItem->GetBasicAttackDamage();
		m_wDefenceStat = pItem->GetBasicDefence();
		
		m_nElectric = pItem->cRESIST_ELEC;
		m_nFire     = pItem->cRESIST_FIRE;
		m_nIce      = pItem->cRESIST_ICE;
		m_nPoison   = pItem->cRESIST_POISON;
		m_nSpirit   = pItem->cRESIST_SPIRIT;
		
		m_wUsedNum = pItem->wTurnNum;

		CTime cTIME ( pItem->tBORNTIME );
		CString strTIME = cTIME.Format ( "%Y/%m/%d %H:%M" );
		m_editTIME.SetWindowText ( strTIME );

		CTime cCosTIME (pItem->tDISGUISE );
		CString strCosTIME = cCosTIME.Format ( "%Y/%m/%d %H:%M" );
		m_strCosTime = strCosTIME;

		m_dwCOSTUME_MID = pItem->nidDISGUISE.wMainID;
		m_dwCOSTUME_SID = pItem->nidDISGUISE.wSubID;

		std::string strGuid = sc::string::uuidToString( ( UUID )pItem->guid );
		m_strGenID.Format ( "%d/%d/%d/%s",
			pItem->cGenType,
			// pItem->cChnID,
			// pItem->cFieldID,
			0, 0,
			strGuid.c_str() );

		// Link Skill;
		{
			m_sLinkSkillID = pItem->GetLinkSkillID();
			m_wLinkSkillLevel = pItem->GetLinkSkillLevel();
			m_fLinkSkillOccurRate = pItem->GetLinkSkillOccurRate();

			int nType = static_cast< int >( m_vPutOnItem[m_nRadio].GetLinkSkillTarget() );
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
			int nType = 0;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 0 );
				m_comboAddOpt1.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 1 );
				m_comboAddOpt2.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 2 );
				m_comboAddOpt3.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 3 );
				m_comboAddOpt4.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 4 );
				m_comboAddOpt5.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 5 );
				m_comboAddOpt6.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEMADDON", nType).GetString());
			}
			else
			{
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 0 );
				m_comboAddOpt1.SelectString ( nType-1, COMMENT::ITEMADDON[nType].c_str() );
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 1 );
				m_comboAddOpt2.SelectString ( nType-1, COMMENT::ITEMADDON[nType].c_str() );
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 2 );
				m_comboAddOpt3.SelectString ( nType-1, COMMENT::ITEMADDON[nType].c_str() );
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 3 );
				m_comboAddOpt4.SelectString ( nType-1, COMMENT::ITEMADDON[nType].c_str() );
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 4 );
				m_comboAddOpt5.SelectString ( nType-1, COMMENT::ITEMADDON[nType].c_str() );
				nType = m_vPutOnItem[m_nRadio].sAddonInfo.GetOptType( 5 );
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
				nType = m_vPutOnItem[ m_nRadio ].sAddonInfo.GetOptType( i );
				if ( true == ITEM::bAddonValueInteger[ nType ] )
					SetWin_Num_int ( this, nDlgID_AddOption[ i ], pItem->sAddonInfo.m_sADDON[i].nVALUE );
				else
					SetWin_Num_float ( this, nDlgID_AddOption[ i ], pItem->sAddonInfo.m_sADDON[i].fVALUE );
			}
		}

		// Random Option;
		{
			int nType = 0;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				nType = m_vPutOnItem[m_nRadio].GETOptTYPE1();
				m_comboRandomOpt1.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", nType).GetString());
				nType = m_vPutOnItem[m_nRadio].GETOptTYPE2();
				m_comboRandomOpt2.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", nType).GetString());
				nType = m_vPutOnItem[m_nRadio].GETOptTYPE3();
				m_comboRandomOpt3.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", nType).GetString());
				nType = m_vPutOnItem[m_nRadio].GETOptTYPE4();
				m_comboRandomOpt4.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", nType).GetString());
				nType = m_vPutOnItem[m_nRadio].GETOptTYPE5();
				m_comboRandomOpt5.SelectString(nType-1, CGameTextMan::GetInstance().GetCommentText("ITEM_RANDOM_OPT", nType).GetString());
			}
			else
			{
				nType = m_vPutOnItem[m_nRadio].GETOptTYPE1();
				m_comboRandomOpt1.SelectString ( nType-1, COMMENT::ITEM_RANDOM_OPT[nType].c_str() );
				nType = m_vPutOnItem[m_nRadio].GETOptTYPE2();
				m_comboRandomOpt2.SelectString ( nType-1, COMMENT::ITEM_RANDOM_OPT[nType].c_str() );
				nType = m_vPutOnItem[m_nRadio].GETOptTYPE3();
				m_comboRandomOpt3.SelectString ( nType-1, COMMENT::ITEM_RANDOM_OPT[nType].c_str() );
				nType = m_vPutOnItem[m_nRadio].GETOptTYPE4();
				m_comboRandomOpt4.SelectString ( nType-1, COMMENT::ITEM_RANDOM_OPT[nType].c_str() );
				nType = m_vPutOnItem[m_nRadio].GETOptTYPE5();
				m_comboRandomOpt5.SelectString ( nType-1, COMMENT::ITEM_RANDOM_OPT[nType].c_str() );
			}


			SetWin_Num_int( this, IDC_EDIT_RANDOM_OPT1, pItem->randomOption.getValue(0) );
			SetWin_Num_int( this, IDC_EDIT_RANDOM_OPT2, pItem->randomOption.getValue(1) );
			SetWin_Num_int( this, IDC_EDIT_RANDOM_OPT3, pItem->randomOption.getValue(2) );
			SetWin_Num_int( this, IDC_EDIT_RANDOM_OPT4, pItem->randomOption.getValue(3) );
			SetWin_Num_int( this, IDC_EDIT_RANDOM_OPT5, pItem->randomOption.getValue(4) );
		}
		
		// 코스튬 기간 변경용으로 만들었지만, 지금은 생성시간 변경용으로 사용한다
		m_dlgItemDisguise.m_cTime = CTime( m_vPutOnItem[ m_nRadio ].tBORNTIME );
	}
	else
	{
		m_strName = "";

		m_nMainID = 0;
		m_nSubID  = 0;

		m_nDamage    = 0;
		m_nDefense   = 0;

		m_sDamageStat.dwData = 0;
		m_wDefenceStat = 0;
		
		m_nElectric = 0;
		m_nFire     = 0;
		m_nIce      = 0;
		m_nPoison   = 0;
		m_nSpirit	= 0;
		
		m_wUsedNum = 0;

		m_editTIME.SetWindowText ( "" );

		m_dwCOSTUME_MID = 0;
		m_dwCOSTUME_SID = 0;

		m_sLinkSkillID.Reset();
		m_wLinkSkillLevel = 0;
		m_fLinkSkillOccurRate = 0.f;

		m_strGenID = "";
	}

	UpdateData(FALSE);
}
///////
/*
EMSLOT
	SLOT_HEADGEAR	= 0,	//	모자
	SLOT_UPPER		= 1,	//	상체
	SLOT_LOWER		= 2,	//	하체
	SLOT_HAND		= 3,	//	손
	SLOT_FOOT		= 4,	//	발

	SLOT_RHAND		= 5,	//	오른손 도구
	SLOT_LHAND		= 6,	//	왼손 도구

	SLOT_NECK		= 7,	//	목걸이
	SLOT_WRIST		= 8,	//	손목

	SLOT_RFINGER	= 9,	//	오른손 손가락
	SLOT_LFINGER	= 10,	//	왼손 손가락
*/


void CPropertyChaPutOnItem::OnBnClickedRadio2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_HEADGEAR;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO2 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_UPPER;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO1 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_LOWER;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO3 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_HAND;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO4 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_FOOT;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO5 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_WRIST;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO9 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_NECK;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO8 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_RHAND;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO6 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_LHAND;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO7 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio11()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_RFINGER;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO11 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_LFINGER;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO10 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio12()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = INT ( SLOT_RHAND_S );
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO12 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio13()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_LHAND_S;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO13 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedRadio14()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_WAISTBAND;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO14 );
	SetData();
}
void CPropertyChaPutOnItem::OnBnClickedRadio15()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_DECORATION;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO15 );
	SetData();
}
void CPropertyChaPutOnItem::OnBnClickedRadio16()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_EARRINGS;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO16 );
	SetData();
}
void CPropertyChaPutOnItem::OnBnClickedRadio17()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_RACCESSORIES;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO17 );
	SetData();
}
void CPropertyChaPutOnItem::OnBnClickedRadio18()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_LACCESSORIES;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO18 );
	SetData();
}
void CPropertyChaPutOnItem::OnBnClickedRadio19()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nRadio = SLOT_VEHICLE;
	CheckRadioButton( IDC_RADIO2, IDC_RADIO19, IDC_RADIO19 );
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedBtnPutonDelete()
{ 
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();

	///////////////////////////////////////////////////////////////////////////
	// GM Log
	CString strUserIP = m_pDBMan->GetLocalIP();
	int nGmNum = pFrame->m_nUserNum;
	SITEMCUSTOM sItem_old = pSheet->m_pChar->GET_SLOT_ITEM(EMSLOT(m_nRadio));
	if ( NATIVEID_NULL() == sItem_old.GetNativeID() )
	{
		MessageBox( _T( "item is NATIVEID_NULL" ) );
		return;
	}
	std::strstream strGmCmd;
	strGmCmd << "Character(" << pSheet->m_pChar->m_CharDbNum << ")";
	strGmCmd << " Delete PutOnItem Item(" << sItem_old.Mid() << "," << sItem_old.Sid() << ")";				
	strGmCmd << '\0';
	m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
	strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	///////////////////////////////////////////////////////////////////////////
	
	m_pDBMan->ItemDelete( sItem_old.GetGuid(), sItem_old.GetDbState(), 2889 );

	pSheet->m_pChar->RELEASE_SLOT_ITEM(EMSLOT(m_nRadio));
	m_vPutOnItem[m_nRadio] = NATIVEID_NULL();
	SetData();
}

void CPropertyChaPutOnItem::OnBnClickedBtnPutonChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}


	CDlgInvenAdd dlgAdd;
	if (dlgAdd.DoModal() == IDOK)
	{
		CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();

		m_NewItem = SITEMCUSTOM(dlgAdd.m_vItem[dlgAdd.m_vSelected[0]].sBasicOp.sNativeID);

		// 아이템 에디트에서 입력한 개조 등급 적용
		m_NewItem.cDAMAGE = (BYTE)dlgAdd.m_vItem[ dlgAdd.m_vSelected[0] ].sBasicOp.wGradeAttack;
		m_NewItem.cDEFENSE = (BYTE)dlgAdd.m_vItem[ dlgAdd.m_vSelected[0] ].sBasicOp.wGradeDefense;

		if (EMSLOTCHECK_OK==pSheet->m_pChar->CHECKSLOT_ITEM(m_NewItem.GetNativeID(), EMSLOT(m_nRadio))) // 들어갈수 있는 아이템이면
		{
			//GetData(pSheet->m_pChar);
			SetData( m_NewItem );

			GetDlgItem(IDC_EDIT_DAMAGE_LOW)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_DAMAGE_HIGH)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_DEFENSE)->EnableWindow(TRUE);

			m_comboLinkSkillTarget.EnableWindow(TRUE);

			GetDlgItem(IDC_EDIT_LINKSKILL_MID)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_LINKSKILL_SID)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_LINKSKILL_LEVEL)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_LINKSKILL_PROBABILITY)->EnableWindow(TRUE);

			m_comboAddOpt1.EnableWindow(TRUE);
			m_comboAddOpt2.EnableWindow(TRUE);
			m_comboAddOpt3.EnableWindow(TRUE);
			m_comboAddOpt4.EnableWindow(TRUE);
			m_comboAddOpt5.EnableWindow(TRUE);
			m_comboAddOpt6.EnableWindow(TRUE);

			GetDlgItem(IDC_EDIT_ADD_OPT1)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_ADD_OPT2)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_ADD_OPT3)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_ADD_OPT4)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_ADD_OPT5)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_ADD_OPT6)->EnableWindow(TRUE);

			m_comboRandomOpt1.EnableWindow(TRUE);
			m_comboRandomOpt2.EnableWindow(TRUE);
			m_comboRandomOpt3.EnableWindow(TRUE);
			m_comboRandomOpt4.EnableWindow(TRUE);
			m_comboRandomOpt5.EnableWindow(TRUE);

			GetDlgItem(IDC_EDIT_RANDOM_OPT1)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_RANDOM_OPT2)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_RANDOM_OPT3)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_RANDOM_OPT4)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_RANDOM_OPT5)->EnableWindow(TRUE);



			GetDlgItem(IDC_EDIT6)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT4)->EnableWindow(TRUE);

			GetDlgItem(IDC_EDIT25)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT31)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT24)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT8)->EnableWindow(TRUE);

		}

		
	}

}

void CPropertyChaPutOnItem::OnBnClickedBtnPutonitemSave()
{
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;

	// 코스튬 기간 변경용으로 만들었지만, 지금은 생성시간 변경용으로 사용한다
	int nMaxSize = EMSLOT( SLOT_TSIZE );
	for (int i=0; i<nMaxSize; ++i)
		pChar->m_PutOnItems[i].tBORNTIME = m_vPutOnItem[i].tBORNTIME;

	m_pDBMan->SaveCharacterPutOnItem(pChar);

	///////////////////////////////////////////////////////////////////////////
	// GM Log
	CString strUserIP = m_pDBMan->GetLocalIP();
	int nGmNum = pFrame->m_nUserNum;
	std::strstream strGmCmd;
	strGmCmd << "Character(" << pChar->m_CharDbNum << ") PutOnItem Save";
	strGmCmd << '\0';
	m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
	strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	///////////////////////////////////////////////////////////////////////////

	for ( unsigned int i=0 ; i<m_vecWriteLogItems.size() ; i++ )
	{
		SITEMCUSTOM& sItemCustom = m_vecWriteLogItems[i];
		SNATIVEID sOrigNID(0, 0);
		if ( sItemCustom.IsBoxWrapped() )
		{
			sOrigNID = sItemCustom.GetBoxWrappedItemID();
		}

		// 랜덤옵션 로그
		LOG_RANDOM_OPTION LogRandomOption(
			sItemCustom.Mid(),
			sItemCustom.Sid(),
			0,
			0,
			0,
			sItemCustom.cGenType,
			sItemCustom.guid,
			sItemCustom.GETOptTYPE1(),
			sItemCustom.GETOptTYPE2(),
			sItemCustom.GETOptTYPE3(),
			sItemCustom.GETOptTYPE4(),
			sItemCustom.GETOptTYPE5(),
			sItemCustom.randomOption.getValue(0),
			sItemCustom.randomOption.getValue(1),
			sItemCustom.randomOption.getValue(2),
			sItemCustom.randomOption.getValue(3),
			sItemCustom.randomOption.getValue(4),
			static_cast<BYTE>(sItemCustom.getOptCount(0)),
			static_cast<BYTE>(sItemCustom.getOptCount(1)),
			static_cast<BYTE>(sItemCustom.getOptCount(2)),
			static_cast<BYTE>(sItemCustom.getOptCount(3)), 
			static_cast<BYTE>(sItemCustom.getOptCount(4)), 
			sc::time::GetCurrentTime(),
			0);

		LOG_ITEM_EXCHANGE logItemExchange(
			sItemCustom.Mid(),
			sItemCustom.Sid(),
			0,
			0,
			0,
			sItemCustom.cGenType,
			sItemCustom.guid,

			gamelog::ID_CHAR,
			0,
			gamelog::ID_CHAR,
			pChar->m_CharDbNum,
			ITEM_ROUTE_CHAR,
			sItemCustom.wTurnNum,

			sItemCustom.nidDISGUISE.Mid(),
			sItemCustom.nidDISGUISE.Sid(),

			sItemCustom.cDAMAGE,
			sItemCustom.cDEFENSE,

			sItemCustom.cRESIST_FIRE,
			sItemCustom.cRESIST_ICE,
			sItemCustom.cRESIST_ELEC,
			sItemCustom.cRESIST_POISON,
			sItemCustom.cRESIST_SPIRIT,


			0,
			0,
			0,
			0,
			sOrigNID.Mid(),
			sOrigNID.Sid(),
			LOG_ITEM_CHAR_PUTON_CURRENT,
			sc::time::GetCurrentTime() );

		m_pDBMan->WriteLogRandomItem(LogRandomOption);
		m_pDBMan->WriteLogItemExchange(logItemExchange);
	}
	m_vecWriteLogItems.clear();

}

void CPropertyChaPutOnItem::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	OnBnClickedBtnPutonitemSave();

	CPropertyPage::OnOK();
}

void CPropertyChaPutOnItem::OnBnClickedBtnDisguise()
{
	// 코스튬 기간 변경용으로 만들었지만, 지금은 생성시간 변경용으로 사용한다
	if( m_dlgItemDisguise.DoModal() == IDOK )
	{
		m_vPutOnItem[ m_nRadio ].tBORNTIME = m_dlgItemDisguise.m_cTime.GetTime();
		CString strTime = m_dlgItemDisguise.m_cTime.Format( "%Y/%m/%d %H:%M" );
		m_editTIME.SetWindowText( strTime );

		UpdateData( FALSE );
	}
}


void CPropertyChaPutOnItem::OnBnClickedBtnPutonApply()
{
	if (MessageBox("Really Change?", "Put on item", MB_YESNO) == IDYES)
	{
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

		SITEMCUSTOM& sItem = m_NewItem;

		CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();

		if (EMSLOTCHECK_OK==pSheet->m_pChar->CHECKSLOT_ITEM(sItem.GetNativeID(), EMSLOT(m_nRadio))) // 들어갈수 있는 아이템이면
		{	
			// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			if (pFrame->m_nUserLevel < USER_GM1)
			{
				MessageBox(_T("Permission Denied"));
				return;
			}

			// GMCharEdit 에 대한 거래 제한 장치 국가별 요구 사항이 달라서 다르게 코딩한다.
#ifdef THAIWAN // 대만
			// Item can trade
			m_NewItem.SetGM_GEN2(pFrame->m_nUserNum);
#elif THAILAND // 태국
			// Item can't trade
			m_NewItem.SetGM_GEN(pFrame->m_nUserNum);
#elif KOREA // 한국
			if (MessageBox(_T("거래를 불가능하게 하시겠습니까?"), _T("Confirm"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				// Item can't trade
				m_NewItem.SetGM_GEN(pFrame->m_nUserNum);				
			}
			else
			{
				// Item can trade
				m_NewItem.SetGM_GEN2(pFrame->m_nUserNum);					
			}
#else
			sc::MGUID guid;
			sc::SeqUniqueGUID::Instance()->getGUID( guid );
			m_NewItem.GenerateBasicStat( false );
			m_NewItem.GenerateLinkSkill();
			m_NewItem.GenerateAddOption();
			m_NewItem.SetGM_GEN_RANDOMGENNUM( ( MessageBox(_T("Do you want item can not trade?"), _T("Confirm"), MB_ICONQUESTION | MB_YESNO) != IDYES ), guid );
#endif // GMCharEdit 에 대한 거래 제한 장치 국가별 요구 사항이 달라서 다르게 코딩한다.

			m_NewItem.SetDbState( db::DB_INSERT );

			for( int i=0; i<RandomOpt::NSIZE ; i++)
				sItem.randomOption.remove(0);


			int nIndex, nData;

			// Basic Stats;
			{
				GLPADATA sDamage;
				sDamage.wHigh = static_cast< WORD >( GetWin_Num_int( this, IDC_EDIT_DAMAGE_HIGH ) );
				sDamage.wLow = static_cast< WORD >( GetWin_Num_int( this, IDC_EDIT_DAMAGE_LOW ) );
				m_NewItem.SetBasicAttackDamage( sDamage );

				WORD wDefence = 0;
				wDefence = static_cast< WORD >( GetWin_Num_int( this, IDC_EDIT_DEFENSE ) );
				m_NewItem.SetBasicDefence( wDefence );
			}

			// Link Skill;
			{
				nIndex = m_comboLinkSkillTarget.GetCurSel();
				nData = static_cast< int >( m_comboLinkSkillTarget.GetItemData( nIndex ) );
				sItem.SetLinkSkillTarget( static_cast< EMITEM_SKILL_LINK >( nData ) );

				SNATIVEID sSkillID( false );
				sSkillID.wMainID = static_cast< WORD >( GetWin_Num_int( this, IDC_EDIT_LINKSKILL_MID ) );
				sSkillID.wSubID = static_cast< WORD >( GetWin_Num_int( this, IDC_EDIT_LINKSKILL_SID ) );
				m_NewItem.SetLinkSkillID( sSkillID );

				int nSkillLevel = 0;
				nSkillLevel = GetWin_Num_int( this, IDC_EDIT_LINKSKILL_LEVEL );
				m_NewItem.SetLinkSkillLevel( static_cast< WORD >( nSkillLevel ) );

				float fSkillOccurRate = 0.f;
				fSkillOccurRate = GetWin_Num_float( this, IDC_EDIT_LINKSKILL_PROBABILITY );
				m_NewItem.SetLinkSkillOccurRate( fSkillOccurRate );
			}

			// Add Option;
			{
				nIndex = m_comboAddOpt1.GetCurSel();
				nData = (int)m_comboAddOpt1.GetItemData( nIndex );
				sItem.sAddonInfo.m_sADDON[ 0 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
				if ( true == ITEM::bAddonValueInteger[ nData ] )
					sItem.sAddonInfo.m_sADDON[ 0 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT1 );
				else
					sItem.sAddonInfo.m_sADDON[ 0 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT1 );

				nIndex = m_comboAddOpt2.GetCurSel();
				nData = (int)m_comboAddOpt2.GetItemData( nIndex );
				sItem.sAddonInfo.m_sADDON[ 1 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
				if ( true == ITEM::bAddonValueInteger[ nData ] )
					sItem.sAddonInfo.m_sADDON[ 1 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT2 );
				else
					sItem.sAddonInfo.m_sADDON[ 1 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT2 );

				nIndex = m_comboAddOpt3.GetCurSel();
				nData = (int)m_comboAddOpt3.GetItemData( nIndex );
				sItem.sAddonInfo.m_sADDON[ 2 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
				if ( true == ITEM::bAddonValueInteger[ nData ] )
					sItem.sAddonInfo.m_sADDON[ 2 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT3 );
				else
					sItem.sAddonInfo.m_sADDON[ 2 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT3 );

				nIndex = m_comboAddOpt4.GetCurSel();
				nData = (int)m_comboAddOpt4.GetItemData( nIndex );
				sItem.sAddonInfo.m_sADDON[ 3 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
				if ( true == ITEM::bAddonValueInteger[ nData ] )
					sItem.sAddonInfo.m_sADDON[ 3 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT4 );
				else
					sItem.sAddonInfo.m_sADDON[ 3 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT4 );

				nIndex = m_comboAddOpt5.GetCurSel();
				nData = (int)m_comboAddOpt5.GetItemData( nIndex );
				sItem.sAddonInfo.m_sADDON[ 4 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
				if ( true == ITEM::bAddonValueInteger[ nData ] )
					sItem.sAddonInfo.m_sADDON[ 4 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT5 );
				else
					sItem.sAddonInfo.m_sADDON[ 4 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT5 );

				nIndex = m_comboAddOpt6.GetCurSel();
				nData = (int)m_comboAddOpt6.GetItemData( nIndex );
				sItem.sAddonInfo.m_sADDON[ 5 ].emTYPE = static_cast< EMITEM_ADDON >( nData );
				if ( true == ITEM::bAddonValueInteger[ nData ] )
					sItem.sAddonInfo.m_sADDON[ 5 ].nVALUE = GetWin_Num_int( this, IDC_EDIT_ADD_OPT6 );
				else
					sItem.sAddonInfo.m_sADDON[ 5 ].fVALUE = GetWin_Num_float( this, IDC_EDIT_ADD_OPT6 );
			}

			nIndex = m_comboRandomOpt1.GetCurSel();
			nData = (int) m_comboRandomOpt1.GetItemData ( nIndex );
			sItem.randomOption.assign(0, nData, GetWin_Num_int( this, IDC_EDIT_RANDOM_OPT1 ));

			nIndex = m_comboRandomOpt2.GetCurSel();
			nData = (int) m_comboRandomOpt2.GetItemData ( nIndex );
			sItem.randomOption.assign(1, nData, GetWin_Num_int( this, IDC_EDIT_RANDOM_OPT2 ));

			nIndex = m_comboRandomOpt3.GetCurSel();
			nData = (int) m_comboRandomOpt3.GetItemData ( nIndex );
			sItem.randomOption.assign(2, nData, GetWin_Num_int( this, IDC_EDIT_RANDOM_OPT3 ));

			nIndex = m_comboRandomOpt4.GetCurSel();
			nData = (int) m_comboRandomOpt4.GetItemData ( nIndex );
			sItem.randomOption.assign(3, nData, GetWin_Num_int( this, IDC_EDIT_RANDOM_OPT4 ));

			nIndex = m_comboRandomOpt5.GetCurSel();
			nData = (int) m_comboRandomOpt5.GetItemData ( nIndex );
			sItem.randomOption.assign(4, nData, GetWin_Num_int( this, IDC_EDIT_RANDOM_OPT5 ));

			// 아이템 에디트에서 입력한 개조 등급 적용
			m_NewItem.cDAMAGE			= atoi( GetWin_Text(this,IDC_EDIT6) );
			m_NewItem.cDEFENSE			= atoi( GetWin_Text(this,IDC_EDIT4) );

			m_NewItem.cRESIST_ELEC		= atoi( GetWin_Text(this,IDC_EDIT25) );
			m_NewItem.cRESIST_FIRE		= atoi( GetWin_Text(this,IDC_EDIT31) );
			m_NewItem.cRESIST_ICE		= atoi( GetWin_Text(this,IDC_EDIT24) );
			m_NewItem.cRESIST_POISON	= atoi( GetWin_Text(this,IDC_EDIT5) );
			m_NewItem.cRESIST_SPIRIT	= atoi( GetWin_Text(this,IDC_EDIT8) );

			///////////////////////////////////////////////////////////////////////////
			// GM Log
			CString strUserIP = m_pDBMan->GetLocalIP();
			int nGmNum = pFrame->m_nUserNum;
			SITEMCUSTOM sItem_old = pSheet->m_pChar->GET_SLOT_ITEM(EMSLOT(m_nRadio));
			std::strstream strGmCmd;
			strGmCmd << "Character(" << pSheet->m_pChar->m_CharDbNum << ")";
			strGmCmd << " Delete PutOnItem Item(" << sItem_old.Mid() << "," << sItem_old.Sid() << ")";				
			strGmCmd << '\0';
			m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
			strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
			///////////////////////////////////////////////////////////////////////////		

			// 기존에 있으면 지우고
			pSheet->m_pChar->RELEASE_SLOT_ITEM(EMSLOT(m_nRadio));
			// 교체한다.
			pSheet->m_pChar->SLOT_ITEM(m_NewItem, EMSLOT(m_nRadio));

			///////////////////////////////////////////////////////////////////////////
			// GM Log
			std::strstream strGmCmd2;
			strGmCmd2 << "Character(" << pSheet->m_pChar->m_CharDbNum << ")";
			strGmCmd2 << " Change PutOnItem Item(" << sItem.Mid() << "," << sItem.Sid() << ")";				
			strGmCmd2 << '\0';
			m_pDBMan->WriteGMLog(nGmNum, strGmCmd2, strUserIP);
			strGmCmd2.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
			///////////////////////////////////////////////////////////////////////////


			m_vecWriteLogItems.push_back( m_NewItem );
		}
		else
		{
			MessageBox("Put On Fail", "Put on item", MB_OK);
		}
	}


}
