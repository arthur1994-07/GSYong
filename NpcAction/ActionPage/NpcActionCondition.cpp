// NpcActionCondition.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../NpcAction.h"
#include "./NpcActionCondition.h"
#include "../Edit/SheetWithTab.h"
#include "./HaveItemDialog.h"
#include "./SkillDialog.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Skill/GLSkill.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../Dialogue/DlgQuest.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include ".././RanLogic/GLStringTable.h"

// CNpcActionCondition 대화 상자입니다.

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CNpcActionCondition, CPropertyPage)
CNpcActionCondition::CNpcActionCondition( LOGFONT logfont )
	: CPropertyPage(CNpcActionCondition::IDD)	
	, m_pFont ( NULL )	
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);

	for( BYTE i = 0; i < MAX_NEEDITEM_COUNT; i++ )
	{
		m_TalkNeedItem[i].Reset();
	}
}

CNpcActionCondition::~CNpcActionCondition()
{
	SAFE_DELETE ( m_pFont );
}

void CNpcActionCondition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIS_SKILL, m_listDisSkill);
    DDX_Control(pDX, IDC_LIST_DIS_QUEST, m_listDisQuest);
    DDX_Control(pDX, IDC_LIST_SKILLFACT, m_listSkillFact);
	DDX_Control(pDX, IDC_COMBO_QUEST_STEP, m_comboSTEP);
	DDX_Control(pDX, IDC_LIST_COMQUEST, m_listComQuest);
	DDX_Control(pDX, IDC_COMBO_SCHOOL, m_comboSchool);
}


BEGIN_MESSAGE_MAP(CNpcActionCondition, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_ITEM_ADD, OnBnClickedButtonItemAdd)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_ADD, OnBnClickedButtonSkillAdd)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_DEL, OnBnClickedButtonSkillDel)
	ON_BN_CLICKED(IDC_BUTTON_ITEM_DEL, OnBnClickedButtonItemDel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_ITEM_DELALL, OnBnClickedButtonItemDelall)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_DELALL, OnBnClickedButtonSkillDelall)
	ON_CBN_SELCHANGE(IDC_COMBO_LEVEL_SIGN, OnCbnSelchangeComboLevelSign)
	ON_CBN_SELCHANGE(IDC_COMBO_ELEMENTAL_SIGN, OnCbnSelchangeComboElementalSign)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTIONPOINT_SIGN, OnCbnSelchangeComboActionpointSign)
	ON_BN_CLICKED(IDC_BUTTON_ITEM_QUEST_SELECT, OnBnClickedButtonItemQuestSelect)
	ON_BN_CLICKED(IDC_CHECK_TIME, OnBnClickedCheckTime)
	ON_BN_CLICKED(IDC_BUTTON_QUEST_DEL, OnBnClickedButtonQuestDel)
	ON_BN_CLICKED(IDC_BUTTON_DIS_SKILL_ADD, OnBnClickedButtonDisSkillAdd)
	ON_BN_CLICKED(IDC_BUTTON_DIS_SKILL_DEL, OnBnClickedButtonDisSkillDel)
	ON_BN_CLICKED(IDC_BUTTON_DIS_QUEST_ADD, OnBnClickedButtonDisQuestAdd)
    ON_BN_CLICKED(IDC_BUTTON_DIS_QUEST_DEL, OnBnClickedButtonDisQuestDel)
    ON_BN_CLICKED(IDC_BUTTON_SKILLFACT_ADD, OnBnClickedButtonSkillFactAdd)
    ON_BN_CLICKED(IDC_BUTTON_SKILLFACT_DEL, OnBnClickedButtonSkillFactDel)
	ON_CBN_SELCHANGE(IDC_COMBO_QUEST_STEP, OnCbnSelchangeComboQuestStep)
	ON_BN_CLICKED(IDC_BUTTON_COMQUEST_ADD, OnBnClickedButtonComquestAdd)
	ON_BN_CLICKED(IDC_BUTTON_COMQUEST_DEL, OnBnClickedButtonComquestDel)
	ON_BN_CLICKED(IDC_BUTTON_COMQUEST_DELALL, OnBnClickedButtonComquestDelall)
	ON_NOTIFY(NM_CLICK, IDC_LISTCONT_ITEM, OnNMClickListcontItem)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_RADIO_MAN, OnBnClickedRadioMan)
	ON_BN_CLICKED(IDC_RADIO_WOMAN, OnBnClickedRadioWoman)
END_MESSAGE_MAP()


// CNpcActionCondition 메시지 처리기입니다.

void CNpcActionCondition::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( m_CallPage );
}

BOOL CNpcActionCondition::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );
	InitDefaultCtrls ();
	UpdateItems ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CNpcActionCondition::InitDefaultCtrls ()
{
	SetWin_Combo_Init ( this, IDC_COMBO_LEVEL_SIGN, strCONDITIONSIGN, CONDITIONSIGN_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_ELEMENTAL_SIGN, strCONDITIONSIGN, CONDITIONSIGN_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_ACTIONPOINT_SIGN, strCONDITIONSIGN, CONDITIONSIGN_SIZE );

	const int cComboTime = 2;
	int ControlTimeID[cComboTime] = 
	{
		IDC_COMBO_TIME_BEGIN,
		IDC_COMBO_TIME_END
	};
	
	CComboBox *pComboBox;
	for ( int i = 0; i < cComboTime; i++ )
	{
		pComboBox = (CComboBox*) GetDlgItem ( ControlTimeID[i] );
		pComboBox->Clear ();

		for ( int j = 0; j < 24; j++ )
		{
			CString strTime;
			strTime.Format ( "%02d시", j );
			int nIndex = pComboBox->AddString ( strTime );
			pComboBox->SetItemData ( nIndex, j );
		}
	}

	m_comboSchool.ResetContent();
	for ( DWORD i = 0; i < MAX_SCHOOL; i++ )
	{
		std::string strSCHOOLNAME = GLCONST_CHAR::strSCHOOLNAME[i];
		if ( strSCHOOLNAME.empty() )
		{
			int nIndex = m_comboSchool.AddString ( "All School" );
			m_comboSchool.SetItemData ( nIndex, MAX_SCHOOL );
			break;
		}

		int nIndex = m_comboSchool.AddString ( strSCHOOLNAME.c_str() );
		m_comboSchool.SetItemData ( nIndex, i );
	}

	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem ( IDC_LISTCONT_ITEM );

	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	pListCtrl->GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;



	const int nColumnCount = 2;	
	char* szColumnName1[nColumnCount] = { "Item", "Num" };

	int nColumnWidthPercent1[nColumnCount] = { 80, 20 };

	for ( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.pszText = szColumnName1[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent1[i] ) / 100;
		pListCtrl->InsertColumn (i, &lvColumn );
	}

}

BOOL CNpcActionCondition::InverseUpdateItems ()
{
	CString strTemp;
	CComboBox* pComboBox = NULL;

	
	m_DummyCondition.m_bUse = GetWin_Check ( this, IDC_CHECK_USE );
	
	m_DummyCondition.m_signLevel = (EM_CONDITIONSIGN) GetWin_Combo_Sel ( this, IDC_COMBO_LEVEL_SIGN );
	m_DummyCondition.m_wLevel = (WORD) GetWin_Num_int ( this, IDC_EDIT_LEVEL );

	m_DummyCondition.m_dwClass = GLCC_NONE;
	GetWin_Check ( this, IDC_CHECK_FIGHETER_M, m_DummyCondition.m_dwClass, GLCC_FIGHTER_M );
	GetWin_Check ( this, IDC_CHECK_ARMS_M, m_DummyCondition.m_dwClass, GLCC_ARMS_M );
	GetWin_Check ( this, IDC_CHECK_ARCHER_W, m_DummyCondition.m_dwClass, GLCC_ARCHER_W );
	GetWin_Check ( this, IDC_CHECK_SPIRIT_W, m_DummyCondition.m_dwClass, GLCC_SPIRIT_W );


	GetWin_Check ( this, IDC_CHECK_FIGHETER_W, m_DummyCondition.m_dwClass, GLCC_FIGHTER_W );
	GetWin_Check ( this, IDC_CHECK_ARMS_W, m_DummyCondition.m_dwClass, GLCC_ARMS_W );
	GetWin_Check ( this, IDC_CHECK_ARCHER_M, m_DummyCondition.m_dwClass, GLCC_ARCHER_M );
	GetWin_Check ( this, IDC_CHECK_SPIRIT_M, m_DummyCondition.m_dwClass, GLCC_SPIRIT_M );

	GetWin_Check ( this, IDC_CHECK_EXTREME_M, m_DummyCondition.m_dwClass, GLCC_EXTREME_M );
	GetWin_Check ( this, IDC_CHECK_EXTREME_W, m_DummyCondition.m_dwClass, GLCC_EXTREME_W );

	GetWin_Check ( this, IDC_CHECK_SCIENTIST_M, m_DummyCondition.m_dwClass, GLCC_SCIENTIST_M );
	GetWin_Check ( this, IDC_CHECK_SCIENTIST_W, m_DummyCondition.m_dwClass, GLCC_SCIENTIST_W );

    GetWin_Check( this, IDC_CHECK_ASSASSIN_M, m_DummyCondition.m_dwClass, GLCC_ASSASSIN_M );
    GetWin_Check( this, IDC_CHECK_ASSASSIN_W, m_DummyCondition.m_dwClass, GLCC_ASSASSIN_W );

    GetWin_Check( this, IDC_CHECK_TRICKER_M, m_DummyCondition.m_dwClass, GLCC_TRICKER_M );
    GetWin_Check( this, IDC_CHECK_TRICKER_W, m_DummyCondition.m_dwClass, GLCC_TRICKER_W );

	GetWin_Check( this, IDC_CHECK_ACTOR_M, m_DummyCondition.m_dwClass, GLCC_ACTOR_M );
	GetWin_Check( this, IDC_CHECK_ACTOR_W, m_DummyCondition.m_dwClass, GLCC_ACTOR_W );

	m_DummyCondition.m_wSchool = (WORD) GetWin_Combo_Sel ( this, IDC_COMBO_SCHOOL );

	m_DummyCondition.m_signElemental = (EM_CONDITIONSIGN) GetWin_Combo_Sel ( this, IDC_COMBO_ELEMENTAL_SIGN );
	m_DummyCondition.m_nElemental = GetWin_Num_int ( this, IDC_EDIT_ELEMENTAL );

	m_DummyCondition.m_signActionPoint = (EM_CONDITIONSIGN) GetWin_Combo_Sel ( this, IDC_COMBO_ACTIONPOINT_SIGN );
	m_DummyCondition.m_nActionPoint = GetWin_Num_int ( this, IDC_EDIT_ACTIONPOINT );

	m_DummyCondition.m_bTime = GetWin_Check ( this, IDC_CHECK_TIME );
	m_DummyCondition.m_dwTimeBegin = GetWin_Combo_Sel ( this, IDC_COMBO_TIME_BEGIN );
	m_DummyCondition.m_dwTimeEnd = GetWin_Combo_Sel ( this, IDC_COMBO_TIME_END );

	m_DummyCondition.m_dwMONEY = GetWin_Num_int ( this, IDC_EDIT_MONEY );
	m_DummyCondition.m_dwPARTYMEN_NUM = GetWin_Num_int ( this, IDC_EDIT_PARTY );

	return TRUE;
}


void CNpcActionCondition::UpdateItems ()
{
	
	SetWin_Check ( this, IDC_CHECK_USE, m_DummyCondition.m_bUse );
	SetWin_Enable ( this, IDC_EDIT_LEVEL, m_DummyCondition.IsTERM_LEVEL() );

	SetWin_Enable ( this, IDC_EDIT_ELEMENTAL, m_DummyCondition.IsTERM_ELEMENTAL() );

	SetWin_Enable ( this, IDC_EDIT_ACTIONPOINT, m_DummyCondition.IsTERM_ACTIONP() );

	SetWin_Check ( this, IDC_CHECK_TIME, m_DummyCondition.m_bTime );
	SetWin_Enable ( this, IDC_COMBO_TIME_BEGIN, m_DummyCondition.IsTERM_TIME() );
	SetWin_Enable ( this, IDC_COMBO_TIME_END, m_DummyCondition.IsTERM_TIME() );

	SetWin_Enable ( this, IDC_BUTTON_ITEM_DEL, m_DummyCondition.IsTERM_ITEM() );
	SetWin_Enable ( this, IDC_BUTTON_ITEM_DELALL, m_DummyCondition.IsTERM_ITEM() );

	SetWin_Enable ( this, IDC_BUTTON_SKILL_DEL, m_DummyCondition.IsTERM_SKILL() );
	SetWin_Enable ( this, IDC_BUTTON_SKILL_DELALL, m_DummyCondition.IsTERM_SKILL() );


	SetWin_Num_int ( this, IDC_EDIT_LEVEL, m_DummyCondition.m_wLevel );
	SetWin_Combo_Sel ( this, IDC_COMBO_LEVEL_SIGN, strCONDITIONSIGN[m_DummyCondition.m_signLevel].c_str() );

	SetWin_Check ( this, IDC_CHECK_FIGHETER_M, (m_DummyCondition.m_dwClass&GLCC_FIGHTER_M) );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, (m_DummyCondition.m_dwClass&GLCC_ARMS_M) );
	SetWin_Check ( this, IDC_CHECK_ARCHER_W, (m_DummyCondition.m_dwClass&GLCC_ARCHER_W) );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, (m_DummyCondition.m_dwClass&GLCC_SPIRIT_W) );

	SetWin_Check ( this, IDC_CHECK_FIGHETER_W, (m_DummyCondition.m_dwClass&GLCC_FIGHTER_W) );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, (m_DummyCondition.m_dwClass&GLCC_ARMS_W) );
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, (m_DummyCondition.m_dwClass&GLCC_ARCHER_M) );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, (m_DummyCondition.m_dwClass&GLCC_SPIRIT_M) );

	SetWin_Check ( this, IDC_CHECK_EXTREME_M, (m_DummyCondition.m_dwClass&GLCC_EXTREME_M) );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, (m_DummyCondition.m_dwClass&GLCC_EXTREME_W) );

	SetWin_Check ( this, IDC_CHECK_SCIENTIST_M, (m_DummyCondition.m_dwClass&GLCC_SCIENTIST_M) );
	SetWin_Check ( this, IDC_CHECK_SCIENTIST_W, (m_DummyCondition.m_dwClass&GLCC_SCIENTIST_W) );

    SetWin_Check( this, IDC_CHECK_ASSASSIN_M, (m_DummyCondition.m_dwClass&GLCC_ASSASSIN_M) );
    SetWin_Check( this, IDC_CHECK_ASSASSIN_W, (m_DummyCondition.m_dwClass&GLCC_ASSASSIN_W) );

    SetWin_Check( this, IDC_CHECK_TRICKER_M, (m_DummyCondition.m_dwClass&GLCC_TRICKER_M) );
    SetWin_Check( this, IDC_CHECK_TRICKER_W, (m_DummyCondition.m_dwClass&GLCC_TRICKER_W) );

	SetWin_Check( this, IDC_CHECK_ACTOR_M, (m_DummyCondition.m_dwClass&GLCC_ACTOR_M) );
	SetWin_Check( this, IDC_CHECK_ACTOR_W, (m_DummyCondition.m_dwClass&GLCC_ACTOR_W) );


	SetWin_Check( this, IDC_RADIO_ALL, (m_DummyCondition.m_dwClass==GLCC_ALL_ACTOR) );
	SetWin_Check( this, IDC_RADIO_MAN, (m_DummyCondition.m_dwClass==GLCC_MAN) );
	SetWin_Check( this, IDC_RADIO_WOMAN, (m_DummyCondition.m_dwClass==GLCC_WOMAN) );

	std::string strSCHOOLNAME = "All School";
	if ( m_DummyCondition.m_wSchool!=MAX_SCHOOL )		strSCHOOLNAME = GLCONST_CHAR::strSCHOOLNAME[m_DummyCondition.m_wSchool];
	SetWin_Combo_Sel ( this, IDC_COMBO_SCHOOL, strSCHOOLNAME.c_str() );

	SetWin_Num_int ( this, IDC_EDIT_ELEMENTAL, m_DummyCondition.m_nElemental );
	SetWin_Combo_Sel ( this, IDC_COMBO_ELEMENTAL_SIGN, strCONDITIONSIGN[m_DummyCondition.m_signElemental].c_str() );

	SetWin_Num_int ( this, IDC_EDIT_ACTIONPOINT, m_DummyCondition.m_nActionPoint );
	SetWin_Combo_Sel ( this, IDC_COMBO_ACTIONPOINT_SIGN, strCONDITIONSIGN[m_DummyCondition.m_signActionPoint].c_str() );

	SetWin_Combo_Sel ( this, IDC_COMBO_TIME_BEGIN, m_DummyCondition.m_dwTimeBegin );
	SetWin_Combo_Sel ( this, IDC_COMBO_TIME_END, m_DummyCondition.m_dwTimeEnd );



	SNATIVEID sID;
	CListCtrl* pListCtrl = NULL;
	{
		pListCtrl = (CListCtrl*) GetDlgItem ( IDC_LISTCONT_ITEM );
		pListCtrl->DeleteAllItems();		

		SNpcTalkCondition::HAVEITEM_ITER iter = m_DummyCondition.m_vecHaveItem.begin();
		SNpcTalkCondition::HAVEITEM_ITER iter_end = m_DummyCondition.m_vecHaveItem.end();
		int nCount = 0;
		for ( ; iter!=iter_end; ++iter )
		{		
			SITEM_NUM sItemNum = (*iter);
			sID = sItemNum.sNativeID;

			CString strNAME = "NO ITEM";
			SITEM *pItem = GLogicData::GetInstance().GetItem ( sID.wMainID, sID.wSubID );
			if ( pItem )	strNAME = pItem->GetName();

			CString strTemp;
			strTemp.Format ( "[%04d/%04d] %s", sID.wMainID,
				sID.wSubID, strNAME );

//			int nIndex = pListCtrl->AddString ( strTemp );
			int nIndex = pListCtrl->InsertItem( nCount, strTemp );
			pListCtrl->SetItemData ( nIndex, sID.dwID );

			CString strNum;
			strNum.Format( "%d", sItemNum.dwNum );

			pListCtrl->SetItemText( nIndex, 1, strNum );

			nCount++;
		}	

		

		if( m_CallPage == NPCACTIONTALK )
		{
			if( m_pSheetTab->m_NpcActionTalk.GetActionType() == SNpcTalk::EM_BASIC &&
				m_pSheetTab->m_NpcActionTalk.GetActionNO() == SNpcTalk::EM_ITEM_TRADE )
			{
				bool bFind = FALSE;
				DWORD i, j;
				for( i = 0; i < m_DummyCondition.m_vecHaveItem.size(); i++  )
				{	
					bFind = FALSE;
					for( j = 0; j < MAX_NEEDITEM_COUNT; j++ )
					{
						if( m_TalkNeedItem[j] == m_DummyCondition.m_vecHaveItem[i] )
						{
							bFind = TRUE;
							break;
						}
					}
					if( bFind )
					{
						pListCtrl->SetCheck(i,TRUE);
					}else{
						pListCtrl->SetCheck(i,FALSE);
					}
				}
			}
		}

		for( BYTE i = 0; i < MAX_NEEDITEM_COUNT; i++ )
		{
			m_TalkNeedItem[i].Reset();
		}
	}
	

	CListBox* pListBox = NULL;
	{
		pListBox = (CListBox*) GetDlgItem ( IDC_LIST_SKILL );
		pListBox->ResetContent();

		SNpcTalkCondition::LEARNSKILL_ITER iter = m_DummyCondition.m_vecLearnSkill.begin();
		SNpcTalkCondition::LEARNSKILL_ITER iter_end = m_DummyCondition.m_vecLearnSkill.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SSkillCondition sSkillCondition = (*iter);
			sID.dwID = sSkillCondition.dwNID;

			CString strNAME = "NO SKILL";
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sID );
			if ( pSkill )	strNAME = pSkill->GetName();

			CString strTemp;
			strTemp.Format ( "[%d/%d] %s Lev.%d", sID.wMainID,
				sID.wSubID, strNAME, sSkillCondition.wLevel );

			int nIndex = pListBox->AddString ( strTemp );
			pListBox->SetItemData ( nIndex, sSkillCondition.dwNID );
		}
	}

	SetWin_Text ( this, IDC_EDIT_QUEST, "" );
	m_comboSTEP.ResetContent();

	if ( m_DummyCondition.IsTERM_QUEST() )
	{
		CString strNAME = "OFF QUEST";

		GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( m_DummyCondition.m_dwQUEST_NID );
		if ( pQuest )
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
				strNAME.Format ( "[%d] %s", CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
			}
			else
				strNAME.Format ( "[%d] %s", m_DummyCondition.m_dwQUEST_NID, pQuest->GetTITLE() );
		}
		SetWin_Text ( this, IDC_EDIT_QUEST, strNAME );

		if ( pQuest )
		{
			GLQUEST::VECQSTEP& sSTEP = pQuest->GetSTEP();
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				for (INT i = 0; i < (INT)sSTEP.size(); ++i)
				{
					CString strKey;
					strKey.Format("QN_%03d_%03d_P_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, i);
					CString strStepStr = CGameTextMan::GetInstance().GetQuestText(strKey.GetString());
					int nIndex = m_comboSTEP.AddString (strStepStr.GetString());
					m_comboSTEP.SetItemData ( nIndex, nIndex );
				}
			}
			else
			{
				GLQUEST::VECQSTEP_ITER iter = sSTEP.begin();
				GLQUEST::VECQSTEP_ITER iter_end = sSTEP.end();
				for ( ; iter!=iter_end; ++iter )
				{
					GLQUEST_STEP &sSTEP = (*iter);
					int nIndex = m_comboSTEP.AddString ( sSTEP.GetTITLE() );
					m_comboSTEP.SetItemData ( nIndex, nIndex );
				}
			}
		}

		m_comboSTEP.SetCurSel ( m_DummyCondition.m_dwQUEST_STEP );
	}

	{
		m_listComQuest.ResetContent();

		SNpcTalkCondition::PROCQUEST_ITER iter = m_DummyCondition.m_vecCompleteQuest.begin();
		SNpcTalkCondition::PROCQUEST_ITER iter_end = m_DummyCondition.m_vecCompleteQuest.end();
		for ( ; iter!=iter_end; ++iter )
		{
			DWORD dwID = (*iter);

			CString strNAME = "OFF QUEST";
			GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwID );
			if ( pQuest )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					CString strKey;
					strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
					strNAME.Format ( "[%d] %s", CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
				}
				else
					strNAME.Format ( "[%d] %s", dwID, pQuest->GetTITLE() );
			}

			int nIndex = m_listComQuest.AddString ( strNAME );
			m_listComQuest.SetItemData ( nIndex, dwID );
		}
	}

	{
		m_listDisSkill.ResetContent();

		SNpcTalkCondition::PROCSKILL_ITER iter = m_DummyCondition.m_vecDisSkill.begin();
		SNpcTalkCondition::PROCSKILL_ITER iter_end = m_DummyCondition.m_vecDisSkill.end();
		for ( ; iter!=iter_end; ++iter )
		{
			DWORD dwID = (*iter);

			CString strNAME = "OFF SKILL";
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( SNATIVEID(dwID) );
			if ( pSkill )	strNAME = pSkill->GetName();

			int nIndex = m_listDisSkill.AddString ( strNAME );
			m_listDisSkill.SetItemData ( nIndex, dwID );
		}
	}

	{
		m_listDisQuest.ResetContent();

		SNpcTalkCondition::PROCQUEST_ITER iter = m_DummyCondition.m_vecDisQuest.begin();
		SNpcTalkCondition::PROCQUEST_ITER iter_end = m_DummyCondition.m_vecDisQuest.end();
		for ( ; iter!=iter_end; ++iter )
		{
			DWORD dwID = (*iter);

			CString strNAME = "OFF QUEST";
			GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwID );
			if ( pQuest )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					CString strKey;
					strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
					strNAME.Format ( "[%d] %s", CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
				}
				else
					strNAME.Format ( "[%d] %s", dwID, pQuest->GetTITLE() );
			}

			int nIndex = m_listDisQuest.AddString ( strNAME );
			m_listDisQuest.SetItemData ( nIndex, dwID );
		}
	}

    {
        m_listSkillFact.ResetContent();

        SNpcTalkCondition::SKILLFACT_ITER iter = m_DummyCondition.m_vecSkillFact.begin();
        SNpcTalkCondition::SKILLFACT_ITER iter_end = m_DummyCondition.m_vecSkillFact.end();
        for ( ; iter!=iter_end; ++iter )
        {
            SSkillCondition sSkillCondition = (*iter);
            sID.dwID = sSkillCondition.dwNID;

            CString strNAME = "NO SKILL";
            PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sID );
            if ( pSkill )	strNAME = pSkill->GetName();

            CString strTemp;
            strTemp.Format ( "[%d/%d] %s Lev.%d", sID.wMainID,
                sID.wSubID, strNAME, sSkillCondition.wLevel );

            int nIndex = m_listSkillFact.AddString ( strTemp );
            m_listSkillFact.SetItemData ( nIndex, sSkillCondition.dwNID );
        }
    }


	SetWin_Num_int ( this, IDC_EDIT_MONEY, m_DummyCondition.m_dwMONEY );
	SetWin_Num_int ( this, IDC_EDIT_PARTY, m_DummyCondition.m_dwPARTYMEN_NUM );
}


void CNpcActionCondition::OnBnClickedButtonItemAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateItems ();

	CHaveItemDialog dlg;
	if ( dlg.DoModal () == IDOK )
	{
		if ( dlg.GetItemID()!=UINT_MAX )
		{
			m_DummyCondition.ADDHAVEITEM ( dlg.GetItemID (), dlg.GetNum() );

			UpdateItems ();
		}
	}
}

void CNpcActionCondition::OnBnClickedButtonSkillAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateItems ();

	CSkillDialog dlg;
	if ( dlg.DoModal () == IDOK )
	{
		if ( dlg.GetSkillCondition().dwNID!=UINT_MAX )
		{
			m_DummyCondition.ADDLEARNSKILL ( dlg.GetSkillCondition () );

			UpdateItems ();
		}
	}
}

void CNpcActionCondition::OnBnClickedButtonSkillDel()
{
	InverseUpdateItems ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_SKILL );
	int nIndex = pListBox->GetCurSel ();
	if ( nIndex < 0 ) return ;
	DWORD NativeID = (DWORD) pListBox->GetItemData ( nIndex );
	SNATIVEID sID;
	sID.dwID = NativeID;

	CString strTemp;
	strTemp.Format ( "정말 삭제하시겠습니까?\n[%d/%d]", sID.wMainID, sID.wSubID );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_DummyCondition.DELLEARNSKILL ( NativeID );

		UpdateItems ();
	}
}

void CNpcActionCondition::OnBnClickedButtonItemDel()
{
	InverseUpdateItems ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem ( IDC_LISTCONT_ITEM );
	int nIndex = (int)pListCtrl->GetFirstSelectedItemPosition() - 1;	
	if ( nIndex < 0 ) return ;
	DWORD NativeID = (DWORD) pListCtrl->GetItemData ( nIndex );
	SNATIVEID sID;
	sID.dwID = NativeID;

	CString strTemp;
	strTemp.Format ( "Do you want to delete?\n[%d/%d]", sID.wMainID, sID.wSubID );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_DummyCondition.DELHAVEITEM ( NativeID );


		UpdateItems ();
	}
}

BOOL CNpcActionCondition::SetCondition(std::tr1::shared_ptr<SNpcTalkCondition> spConditon)
{

	m_DummyCondition.Reset ();	

	m_spOrgCondition = spConditon;
	m_DummyCondition = (*spConditon);

	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem ( IDC_LISTCONT_ITEM );
	if( m_CallPage==NPCACTIONANSWER )
	{
		pListCtrl->SetExtendedStyle ( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );		
	}else{
		if( m_pSheetTab->m_NpcActionTalk.GetActionType() == SNpcTalk::EM_BASIC &&
			m_pSheetTab->m_NpcActionTalk.GetActionNO() == SNpcTalk::EM_ITEM_TRADE )
		{
			pListCtrl->SetExtendedStyle ( LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );
		}else{
			pListCtrl->SetExtendedStyle ( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );
		}

		for( BYTE i = 0; i < MAX_NEEDITEM_COUNT; i++ )
		{
			m_TalkNeedItem[i] = m_pSheetTab->m_NpcActionTalk.GetNeedItem(i);
		}
	}

	return TRUE;
}

void CNpcActionCondition::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateItems ();

	if ( m_spOrgCondition )
	{        
		*m_spOrgCondition.get() = m_DummyCondition;

		// 비정상적으로 셋팅된 조건이다.
		// 지금은 이미 많은 파일들이 생성되어 있으므로 변경 불가능..
		// 다음 프로젝트때도 NpcAction을 그대로 사용할 경우, 빈 조건은 자동으로 삭제하도록 해야함.
		if( m_spOrgCondition->EmptyCondition() )
		{
			CString strTemp;
			strTemp.Format( "비어있는 조건이 셋팅되었습니다.\n의도된 것이 아니라면, Del 버튼 눌러서 삭제해야 합니다." );
			MessageBox( strTemp, "Warning Code 1226", MB_OK );
		}
	}

	if( m_CallPage==NPCACTIONTALK )
	{
		if( m_pSheetTab->m_NpcActionTalk.GetActionType() == SNpcTalk::EM_BASIC &&
			m_pSheetTab->m_NpcActionTalk.GetActionNO() == SNpcTalk::EM_ITEM_TRADE )
		{
			CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem ( IDC_LISTCONT_ITEM );
			int nItemCount = pListCtrl->GetItemCount(), i, nIndex = 0;
			for( i = 0; i < nItemCount; i++  )
			{		
				if( pListCtrl->GetCheck(i) ) nIndex++;

				if( nIndex > MAX_NEEDITEM_COUNT )
				{
					MessageBox( "Request Item에 5개 이상 체크를 하면 안됩니다.", "경고", MB_OK );
					return;
				}
			}
			if( nIndex == 0 )
			{
				MessageBox( "Request Item에 1개 이상 체크를 해야 합니다.", "경고", MB_OK );
				return;
			}


			nIndex = 0;
			m_pSheetTab->m_NpcActionTalk.ResetNeedItem();
		
			for( i = 0; i < nItemCount; i++  )
			{		
				if( pListCtrl->GetCheck(i) ) 
				{
					m_pSheetTab->m_NpcActionTalk.InsertNeedItem( m_DummyCondition.m_vecHaveItem[i] );
					nIndex++;
				}
			}
		}
	}

	m_pSheetTab->SetActivePage ( m_CallPage );
	if ( m_CallPage==NPCACTIONANSWER )		m_pSheetTab->m_NpcActionAnswer.UpdateItems ();
	else if ( m_CallPage==NPCACTIONTALK )	m_pSheetTab->m_NpcActionTalk.UpdateItems();
}

void CNpcActionCondition::OnBnClickedButtonItemDelall()
{
	InverseUpdateItems ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTemp;
	strTemp.Format ( "Do you want to delete 'list'?" );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_DummyCondition.m_vecHaveItem.clear ();

		UpdateItems ();
	}
}

void CNpcActionCondition::OnBnClickedButtonSkillDelall()
{
	InverseUpdateItems ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTemp;
	strTemp.Format ( "Do you want to delete 'list'?" );

    if ( MessageBox ( strTemp, "질문", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_DummyCondition.m_vecLearnSkill.clear ();

		UpdateItems ();
	}
}

void CNpcActionCondition::OnCbnSelchangeComboLevelSign()
{
	m_DummyCondition.m_signLevel = (EM_CONDITIONSIGN) GetWin_Combo_Sel ( this, IDC_COMBO_LEVEL_SIGN );
	SetWin_Enable ( this, IDC_EDIT_LEVEL, m_DummyCondition.IsTERM_LEVEL() );
}

void CNpcActionCondition::OnCbnSelchangeComboElementalSign()
{
	m_DummyCondition.m_signElemental = (EM_CONDITIONSIGN) GetWin_Combo_Sel ( this, IDC_COMBO_ELEMENTAL_SIGN );
	SetWin_Enable ( this, IDC_EDIT_ELEMENTAL, m_DummyCondition.IsTERM_ELEMENTAL() );
}

void CNpcActionCondition::OnCbnSelchangeComboActionpointSign()
{
	m_DummyCondition.m_signActionPoint = (EM_CONDITIONSIGN) GetWin_Combo_Sel ( this, IDC_COMBO_ACTIONPOINT_SIGN );
	SetWin_Enable ( this, IDC_EDIT_ACTIONPOINT, m_DummyCondition.IsTERM_ACTIONP() );
}

void CNpcActionCondition::OnBnClickedCheckTime()
{
	m_DummyCondition.m_bTime = GetWin_Check ( this, IDC_CHECK_TIME );
	SetWin_Enable ( this, IDC_COMBO_TIME_BEGIN, m_DummyCondition.IsTERM_TIME() );
	SetWin_Enable ( this, IDC_COMBO_TIME_END, m_DummyCondition.IsTERM_TIME() );
}

void CNpcActionCondition::OnBnClickedButtonItemQuestSelect()
{
	InverseUpdateItems ();

	CDlgQuest cDlgQuest;
	if ( cDlgQuest.DoModal () == IDOK )
	{
		m_DummyCondition.m_dwQUEST_NID = cDlgQuest.m_dwQuestID;
		m_DummyCondition.m_dwQUEST_STEP = 0;

		UpdateItems ();
	}
}

void CNpcActionCondition::OnBnClickedButtonQuestDel()
{
	InverseUpdateItems ();

	m_DummyCondition.RESET_QUEST ();

	SetWin_Text ( this, IDC_EDIT_QUEST, "" );
	m_comboSTEP.ResetContent();

	UpdateItems ();
}

void CNpcActionCondition::OnBnClickedButtonDisSkillAdd()
{
	InverseUpdateItems ();

	CSkillDialog dlg;
	if ( dlg.DoModal () == IDOK )
	{
		SSkillCondition &sSKILL = dlg.GetSkillCondition ();

		if ( sSKILL.dwNID!=UINT_MAX )
		{
			m_DummyCondition.ADDDISSKILL ( sSKILL.dwNID );

			UpdateItems ();
		}
	}
}

void CNpcActionCondition::OnBnClickedButtonDisSkillDel()
{
	InverseUpdateItems ();

	int nIndex = m_listDisSkill.GetCurSel ();
	if ( LB_ERR==nIndex )	return;

	DWORD dwData = (DWORD) m_listDisSkill.GetItemData ( nIndex );
	m_DummyCondition.DELDISSKILL ( dwData );

	UpdateItems ();
}

void CNpcActionCondition::OnBnClickedButtonDisQuestAdd()
{
	InverseUpdateItems ();

	CDlgQuest cDlgQuest;
	if ( cDlgQuest.DoModal () == IDOK )
	{
		if ( cDlgQuest.m_dwQuestID!=UINT_MAX )
		{
			m_DummyCondition.ADDDISQUEST ( cDlgQuest.m_dwQuestID );

			UpdateItems ();
		}
	}
}

void CNpcActionCondition::OnBnClickedButtonDisQuestDel()
{
	int nIndex = m_listDisQuest.GetCurSel ();
	if ( LB_ERR==nIndex )	return;

	InverseUpdateItems ();

	DWORD dwData = (DWORD) m_listDisQuest.GetItemData ( nIndex );
	m_DummyCondition.DELDISQUEST ( dwData );

	UpdateItems ();
}

void CNpcActionCondition::OnBnClickedButtonSkillFactAdd()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    InverseUpdateItems ();

    CSkillDialog dlg;
    if ( dlg.DoModal () == IDOK )
    {
        if ( dlg.GetSkillCondition().dwNID!=UINT_MAX )
        {
            m_DummyCondition.ADDSKILLFACT ( dlg.GetSkillCondition () );

            UpdateItems ();
        }
    }
}

void CNpcActionCondition::OnBnClickedButtonSkillFactDel()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int nIndex = m_listSkillFact.GetCurSel();
    if ( LB_ERR==nIndex )	return;

    InverseUpdateItems ();

    DWORD NativeID = (DWORD) m_listSkillFact.GetItemData ( nIndex );

    SNATIVEID sID;
    sID.dwID = NativeID;

    CString strTemp;
    strTemp.Format ( "정말 삭제하시겠습니까?\n[%d/%d]", sID.wMainID, sID.wSubID );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
    {
        m_DummyCondition.DELSKILLFACT ( NativeID );

        UpdateItems ();
    }
}

void CNpcActionCondition::OnCbnSelchangeComboQuestStep()
{
	m_DummyCondition.m_dwQUEST_STEP = m_comboSTEP.GetCurSel ();
}

void CNpcActionCondition::OnBnClickedButtonComquestAdd()
{
	InverseUpdateItems ();

	CDlgQuest cDlgQuest;
	if ( cDlgQuest.DoModal () == IDOK )
	{
		m_DummyCondition.ADDCOMQUEST ( cDlgQuest.m_dwQuestID );

		UpdateItems ();
	}
}

void CNpcActionCondition::OnBnClickedButtonComquestDel()
{
	int nIndex = m_listComQuest.GetCurSel ();
	if ( LB_ERR==nIndex )	return;

	InverseUpdateItems ();

	DWORD dwData = (DWORD) m_listComQuest.GetItemData ( nIndex );
	m_DummyCondition.DELCOMQUEST ( dwData );

	UpdateItems ();
}

void CNpcActionCondition::OnBnClickedButtonComquestDelall()
{
	InverseUpdateItems ();

	CString strTemp;
	strTemp.Format ( "Do you want to delete 'list'?" );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_DummyCondition.m_vecCompleteQuest.clear ();

		UpdateItems ();
	}
}

void CNpcActionCondition::OnNMClickListcontItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem ( IDC_LISTCONT_ITEM );
	int nItemCount = pListCtrl->GetItemCount();

}

void CNpcActionCondition::OnBnClickedRadioAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_ALL );

	SetWin_Check ( this, IDC_CHECK_FIGHETER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARCHER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, bCheck );
	
	SetWin_Check ( this, IDC_CHECK_FIGHETER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, bCheck );
	
	SetWin_Check ( this, IDC_CHECK_EXTREME_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, bCheck );

	SetWin_Check ( this, IDC_CHECK_SCIENTIST_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_SCIENTIST_W, bCheck );	

    SetWin_Check ( this, IDC_CHECK_ASSASSIN_M, bCheck );
    SetWin_Check ( this, IDC_CHECK_ASSASSIN_W, bCheck );

    SetWin_Check ( this, IDC_CHECK_TRICKER_M, bCheck );
    SetWin_Check ( this, IDC_CHECK_TRICKER_W, bCheck );

	SetWin_Check ( this, IDC_CHECK_ACTOR_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ACTOR_W, bCheck );
}

void CNpcActionCondition::OnBnClickedRadioMan()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bCheck = GetWin_Check( this, IDC_RADIO_MAN );

	SetWin_Check( this, IDC_CHECK_ARCHER_W, false );
	SetWin_Check( this, IDC_CHECK_SPIRIT_W, false );	
	SetWin_Check( this, IDC_CHECK_FIGHETER_W, false );
	SetWin_Check( this, IDC_CHECK_ARMS_W, false );	
	SetWin_Check( this, IDC_CHECK_EXTREME_W, false );
	SetWin_Check( this, IDC_CHECK_SCIENTIST_W, false );
    SetWin_Check( this, IDC_CHECK_ASSASSIN_W, false );
    SetWin_Check( this, IDC_CHECK_TRICKER_W, false );

	SetWin_Check( this, IDC_CHECK_ACTOR_W, false );

	SetWin_Check( this, IDC_CHECK_FIGHETER_M, bCheck );
	SetWin_Check( this, IDC_CHECK_ARMS_M, bCheck );	
	SetWin_Check( this, IDC_CHECK_ARCHER_M, bCheck );
	SetWin_Check( this, IDC_CHECK_SPIRIT_M, bCheck );	
	SetWin_Check( this, IDC_CHECK_EXTREME_M, bCheck );
	SetWin_Check( this, IDC_CHECK_SCIENTIST_M, bCheck );	
    SetWin_Check( this, IDC_CHECK_ASSASSIN_M, bCheck );
    SetWin_Check( this, IDC_CHECK_TRICKER_M, bCheck );

	SetWin_Check( this, IDC_CHECK_ACTOR_M, bCheck );
}

void CNpcActionCondition::OnBnClickedRadioWoman()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bCheck = GetWin_Check( this, IDC_RADIO_WOMAN );

	SetWin_Check( this, IDC_CHECK_ARCHER_W, bCheck );
	SetWin_Check( this, IDC_CHECK_SPIRIT_W, bCheck );	
	SetWin_Check( this, IDC_CHECK_FIGHETER_W, bCheck );
	SetWin_Check( this, IDC_CHECK_ARMS_W, bCheck );
	SetWin_Check( this, IDC_CHECK_EXTREME_W, bCheck );
	SetWin_Check( this, IDC_CHECK_SCIENTIST_W, bCheck );	
    SetWin_Check( this, IDC_CHECK_ASSASSIN_W, bCheck );
    SetWin_Check( this, IDC_CHECK_TRICKER_W, bCheck );
	SetWin_Check( this, IDC_CHECK_ACTOR_W, bCheck );

	SetWin_Check( this, IDC_CHECK_FIGHETER_M, false );
	SetWin_Check( this, IDC_CHECK_ARMS_M, false );	
	SetWin_Check( this, IDC_CHECK_ARCHER_M, false );
	SetWin_Check( this, IDC_CHECK_SPIRIT_M, false );	
	SetWin_Check( this, IDC_CHECK_EXTREME_M, false );
	SetWin_Check( this, IDC_CHECK_SCIENTIST_M, false );	
    SetWin_Check( this, IDC_CHECK_ASSASSIN_M, false );
    SetWin_Check( this, IDC_CHECK_TRICKER_M, false );
	SetWin_Check( this, IDC_CHECK_ACTOR_M, false );
}
