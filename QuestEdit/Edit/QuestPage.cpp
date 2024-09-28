// QuestPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Skill/GLSkill.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/SingleobjectTextControl.h"

#include "../QuestEdit.h"
#include "../QuestEditDlg.h"
#include "../Dialogue/DlgItem.h"
#include "../Dialogue/DlgCustomItem.h"
#include "../Dialogue/DlgSkill.h"
#include "../Dialogue/DlgMap.h"
#include "../SelectDialog.h"
#include "./SheetWithTab.h"
#include "./QuestPage.h"

// CQuestPage 대화 상자입니다.
IMPLEMENT_DYNAMIC(CQuestPage, CPropertyPage)
CQuestPage::CQuestPage( LOGFONT logfont )
	: CPropertyPage(CQuestPage::IDD),
	m_bDlgInit(false),
	m_pSheetTab(NULL),
	m_pFont ( NULL )
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CQuestPage::~CQuestPage()
{
	SAFE_DELETE( m_pFont );
}

void CQuestPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTCTR_GIFT_ITEM, m_GiftItemList);    
}

BEGIN_MESSAGE_MAP(CQuestPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_ITEM_NEW, OnBnClickedButtonGiftItemNew)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_ITEM_EDIT, OnBnClickedButtonGiftItemEdit)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_ITEM_DEL, OnBnClickedButtonGiftItemDel)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_SKILL_NEW, OnBnClickedButtonGiftSkillNew)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_SKILL_DEL, OnBnClickedButtonGiftSkillDel)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_EDITCANCEL, OnBnClickedButtonEditcancel)
	ON_BN_CLICKED(IDC_BUTTON_EDITCOMPLATE, OnBnClickedButtonEditcomplate)
	ON_BN_CLICKED(IDC_USE_PROGRESS_CHECK, OnBnClickedUseProgressCheck)
	ON_CBN_SELCHANGE(IDC_STATE_COMBO1, OnCbnSelchangeStateCombo1)
	ON_CBN_SELCHANGE(IDC_STATE_COMBO2, OnCbnSelchangeStateCombo2)
	ON_CBN_SELCHANGE(IDC_STATE_COMBO3, OnCbnSelchangeStateCombo3)
	ON_BN_CLICKED(IDC_MAP_EDIT_BUTTON, OnBnClickedMapEditButton)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_RADIO_MAN, OnBnClickedRadioMan)
	ON_BN_CLICKED(IDC_RADIO_WOMAN, OnBnClickedRadioWoman)
	ON_BN_CLICKED(IDC_MAP_EDIT_BUTTON2, OnBnClickedMapEditButton2)
	ON_BN_CLICKED(IDC_MAP_EDIT_BUTTON3, OnBnClickedMapEditButton3)
    ON_BN_CLICKED(IDC_BUTTON_QUEST_AREA, &CQuestPage::OnBnClickedButtonQuestArea)
	ON_EN_CHANGE(IDC_EDIT_LIMIT_TIME, &CQuestPage::OnEnChangeEditLimitTime)
	ON_BN_CLICKED(IDC_CHECK_DAILY, &CQuestPage::OnBnClickedCheckDaily)
END_MESSAGE_MAP()


// CQuestPage 메시지 처리기입니다.
void CQuestPage::QuestStringtableSynchronization()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CSingleobjectTextControl::GetInstance().Clear();

		const int nMid = (int)CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.Mid();

		CString strNameKey, strDescKey;
		strNameKey.Format("QN_%03d_000", nMid);
		strDescKey.Format("QD_%03d_000", nMid);

		const CString strName = CGameTextMan::GetInstance().GetText(strNameKey.GetString(), 0, CGameTextMan::EM_QUEST_TEXT);
		const CString strDesc = CGameTextMan::GetInstance().GetText(strDescKey.GetString(), 0, CGameTextMan::EM_QUEST_TEXT);
		CSingleobjectTextControl::GetInstance().InsertString(strNameKey.GetString(),strName.GetString());
		CSingleobjectTextControl::GetInstance().InsertString(strDescKey.GetString(),strDesc.GetString());

		int i, nStepSize = CQuestEditDlg::m_sQuestNode.pQUEST->GetSTEPNUM();
		for ( i = 0; i < nStepSize; ++i )
		{
			strNameKey.Format("QN_%03d_000_P_%d", nMid, i);
			strDescKey.Format("QD_%03d_000_D_%d", nMid, i);
			const CString strProg_s = CGameTextMan::GetInstance().GetText(strNameKey.GetString(), 0, CGameTextMan::EM_QUEST_TEXT);
			const CString strDesc_s = CGameTextMan::GetInstance().GetText(strDescKey.GetString(), 0, CGameTextMan::EM_QUEST_TEXT);
			CSingleobjectTextControl::GetInstance().InsertString(strNameKey.GetString(),strProg_s.GetString());
			CSingleobjectTextControl::GetInstance().InsertString(strDescKey.GetString(),strDesc_s.GetString());
		}
	}
}

BOOL CQuestPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

    InitGiftItemList();

	m_bDlgInit = true;

	InitDefaultCtrls();	

	InitAttendenceType();

	SetReadMode();    

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CQuestPage::InitGiftItemList() 
{
    RECT rectCtrl;
    LONG lnWidth;
    LV_COLUMN lvColumn;

    m_GiftItemList.SetExtendedStyle(m_GiftItemList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_GiftItemList.GetClientRect(&rectCtrl);
    lnWidth = rectCtrl.right - rectCtrl.left;

    const int nColumnCount = 4;	
    char* szColumnName1[nColumnCount] = { "ID_Name", "Num", "Dam/Def", "E/F/I/P/S", };
    int nColumnWidthPercent1[nColumnCount] = { 40,12,18,30 };

    for (int i = 0; i < nColumnCount; ++i)
    {		
        lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.pszText = szColumnName1[i];
        lvColumn.iSubItem = i;
        lvColumn.cx = (lnWidth*nColumnWidthPercent1[i]) / 100;
        m_GiftItemList.InsertColumn(i, &lvColumn);
    }
}

void CQuestPage::InitDefaultCtrls()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strQUEST_PROGRESS[EMQP_SIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("QUEST_PROGRESS", strQUEST_PROGRESS, CGameTextMan::EM_COMMENT_TEXT, EMQP_SIZE);
		SetWin_Combo_Init(this, IDC_STATE_COMBO1, strQUEST_PROGRESS, EMQP_SIZE);
		SetWin_Combo_Init(this, IDC_STATE_COMBO2, strQUEST_PROGRESS, EMQP_SIZE);
		SetWin_Combo_Init(this, IDC_STATE_COMBO3, strQUEST_PROGRESS, EMQP_SIZE);
	}
	else
	{
		SetWin_Combo_Init(this, IDC_STATE_COMBO1, COMMENT::QUEST_PROGRESS, EMQP_SIZE);
		SetWin_Combo_Init(this, IDC_STATE_COMBO2, COMMENT::QUEST_PROGRESS, EMQP_SIZE);
		SetWin_Combo_Init(this, IDC_STATE_COMBO3, COMMENT::QUEST_PROGRESS, EMQP_SIZE);
	}

	return;
}

void CQuestPage::UpdateItems ()
{
	if (!m_bDlgInit)
        return;

	SetWin_Num_int ( this, IDC_EDIT_QUEST_NID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.dwID );
	SetWin_Text ( this, IDC_EDIT_QUEST_FILENAME, CQuestEditDlg::m_sQuestNode.strFILE.c_str() );
	SetWin_Num_int ( this, IDC_EDIT_EDITVERSION, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwEDITVER );

    //if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_dwQuestArea < COMMENT::QUEST_AREA_NUM )
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string> vecQUEST_AREA_NAME;
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("QUEST_AREA", vecQUEST_AREA_NAME, CGameTextMan::EM_COMMENT_TEXT);
		if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_dwQuestArea < vecQUEST_AREA_NAME.size() )
			SetWin_Text( this, IDC_EDIT_QUEST_AREA_NAME, vecQUEST_AREA_NAME[ CQuestEditDlg::m_sQuestNode.pQUEST->m_dwQuestArea ].c_str() );
		else
			SetWin_Text( this, IDC_EDIT_QUEST_AREA_NAME, "(Incorrect Data)" );
	}
	else
	{
		if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_dwQuestArea < COMMENT::QUEST_AREA_NAME.size() )
			SetWin_Text( this, IDC_EDIT_QUEST_AREA_NAME, COMMENT::QUEST_AREA_NAME[ CQuestEditDlg::m_sQuestNode.pQUEST->m_dwQuestArea ].c_str() );
		else
			SetWin_Text( this, IDC_EDIT_QUEST_AREA_NAME, "(Incorrect Data)" );
	}

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		const SNATIVEID& sNID = CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID;
		CString strNAME;	strNAME.Format("QN_%03d_%03d", sNID.Mid(), sNID.Sid());
		SetWin_Text (this, IDC_EDIT_TITLE, CSingleobjectTextControl::GetInstance().GetText(strNAME.GetString()).GetString());
		strNAME.Format("QD_%03d_%03d", sNID.Mid(), sNID.Sid());
		SetWin_Text (this, IDC_EDIT_COMMENT, CSingleobjectTextControl::GetInstance().GetText(strNAME.GetString()).GetString());
	}
	else
	{
		SetWin_Text ( this, IDC_EDIT_TITLE, CQuestEditDlg::m_sQuestNode.pQUEST->m_strTITLE.c_str() );
		SetWin_Text ( this, IDC_EDIT_COMMENT, CQuestEditDlg::m_sQuestNode.pQUEST->m_strCOMMENT.c_str() );
	}

	SetWin_Num_int ( this, IDC_EDIT_BEGIN_MONEY, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwBeginMoney );
	SetWin_Num_int ( this, IDC_EDIT_BEGIN_PARTYNUM, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwBeginPartyMemNum );

	SetWin_Num_int ( this, IDC_EDIT_LIMIT_TIME, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwLimitTime );
	SetWin_Num_int ( this, IDC_EDIT_LIMIT_PARTYNUM, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwLimitPartyMemNum );
	
	SetWin_Check ( this, IDC_CHECK_LIMIT_NONDIE, CQuestEditDlg::m_sQuestNode.pQUEST->m_bNonDie );

	SetWin_Check ( this, IDC_CHECK_DAILY, CQuestEditDlg::m_sQuestNode.pQUEST->IsDaily() );
	SetWin_Check ( this, IDC_CHECK_REPEAT, CQuestEditDlg::m_sQuestNode.pQUEST->IsREPEAT() );
	SetWin_Check ( this, IDC_CHECK_AGAIN, CQuestEditDlg::m_sQuestNode.pQUEST->IsAGAIN() );
	SetWin_Check ( this, IDC_CHECK_ONLYONE_PROG, CQuestEditDlg::m_sQuestNode.pQUEST->IsONLYONE() );
	SetWin_Check ( this, IDC_CHECK_GIVEUP, CQuestEditDlg::m_sQuestNode.pQUEST->IsGIVEUP() );

	SetWin_Check ( this, IDC_CHECK_PARTY, CQuestEditDlg::m_sQuestNode.pQUEST->IsPARTY() );
	SetWin_Check ( this, IDC_CHECK_PARTYQUEST, CQuestEditDlg::m_sQuestNode.pQUEST->IsPARTYQUEST() );

	SetWin_Num_LONGLONG ( this, IDC_EDIT_GIFT_EXP, CQuestEditDlg::m_sQuestNode.pQUEST->m_lnGiftEXP );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_MONEY, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftMONEY );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_ELEMENT, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftELEMENT );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_LIFEP, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftLIFEPOINT );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_DISPK, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftDisPK );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_SKILLP, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftSKILLPOINT );
	SetWin_Num_int ( this, IDC_EDIT_GIFT_STATP, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftSTATSPOINT );

	SetWin_Num_int ( this, IDC_EDIT_GIFT_QUEST, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftQUEST );

	SetWin_Check ( this, IDC_CHECK_FIGHETER_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_FIGHTER_M) );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ARMS_M) );
	SetWin_Check ( this, IDC_CHECK_ARCHER_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ARCHER_W) );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_SPIRIT_W) );

	/* 클래스 추가 */
	SetWin_Check ( this, IDC_CHECK_FIGHETER_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_FIGHTER_W) );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ARMS_W) );
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ARCHER_M) );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_SPIRIT_M) );

	SetWin_Check ( this, IDC_CHECK_EXTREME_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_EXTREME_M) );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_EXTREME_W) );
	
	SetWin_Check ( this, IDC_CHECK_SCIENTIST_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_SCIENTIST_M) );
	SetWin_Check ( this, IDC_CHECK_SCIENTIST_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_SCIENTIST_W) );

    SetWin_Check( this, IDC_CHECK_ASSASSIN_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ASSASSIN_M) );
    SetWin_Check( this, IDC_CHECK_ASSASSIN_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ASSASSIN_W) );

    SetWin_Check( this, IDC_CHECK_TRICKER_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_TRICKER_M) );
    SetWin_Check( this, IDC_CHECK_TRICKER_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_TRICKER_W) );

	SetWin_Check( this, IDC_CHECK_ACTOR_M, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ACTOR_M) );
	SetWin_Check( this, IDC_CHECK_ACTOR_W, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS&GLCC_ACTOR_W) );


	SetWin_Check( this, IDC_RADIO_ALL, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS==GLCC_ALL_ACTOR) );
	SetWin_Check( this, IDC_RADIO_MAN, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS==GLCC_MAN) );
	SetWin_Check( this, IDC_RADIO_WOMAN, (CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS==GLCC_WOMAN) );

	SetWin_Check( this, IDC_RADIO_SCHOOL1, false );
	SetWin_Check( this, IDC_RADIO_SCHOOL2, false );
	SetWin_Check( this, IDC_RADIO_SCHOOL3, false );
	SetWin_Check( this, IDC_RADIO_SCHOOL_ALL, false );


	WORD wSchool = CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wSCHOOL;
	switch ( wSchool ) 
	{
	case 0 :
		{
			SetWin_Check( this, IDC_RADIO_SCHOOL1, true );
		}
		break;
	case 1 :
		{
			SetWin_Check( this, IDC_RADIO_SCHOOL2, true );
		}
		break;
	case 2 :
		{
			SetWin_Check( this, IDC_RADIO_SCHOOL3, true );
		}
		break;
	case MAX_SCHOOL :
		{
			SetWin_Check( this, IDC_RADIO_SCHOOL_ALL, true );
		}
		break;
	}

	BOOL bEnable = CQuestEditDlg::m_sQuestNode.pQUEST->IsLIMITTIME();

	SetWin_Enable( this, IDC_RADIO_TIMEOVER1, bEnable );
	SetWin_Enable( this, IDC_RADIO_TIMEOVER2, bEnable );
	SetWin_Enable( this, IDC_CHECK_PAST_ON_ONLINE, bEnable );

	SetWin_Check( this, IDC_RADIO_TIMEOVER1, false );
	SetWin_Check( this, IDC_RADIO_TIMEOVER2, false );

	bool bTimeOver = CQuestEditDlg::m_sQuestNode.pQUEST->m_bTimeLimiorKeep;

	if (bTimeOver)
	{
		SetWin_Check( this, IDC_RADIO_TIMEOVER2, true );
	}
	else
	{
		SetWin_Check( this, IDC_RADIO_TIMEOVER1, true );
	}

	SetWin_Check( this, IDC_RADIO_AUTO_START_ATTENDANCE, false );
	SetWin_Check( this, IDC_RADIO_AUTO_START_NONE, false );

	SetWin_Check( this, IDC_CHECK_PAST_ON_ONLINE, (CQuestEditDlg::m_sQuestNode.pQUEST->m_bPastOnlyOnOline) );

	int nFlag = CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nJOIN;

	if ( nFlag == EMQJ_NONE )
	{
		SetWin_Check( this, IDC_RADIO_AUTO_START_NONE, true );
	} 
	else if ( nFlag == EMQJ_ATTENDANCE )
	{
		SetWin_Check( this, IDC_RADIO_AUTO_START_ATTENDANCE, true );
	}

	else if ( nFlag == EMQJ_ATTENDANCE )
	{
		SetWin_Check( this, IDC_RADIO_AUTO_START_JOIN_START, true );
	}

	SetWin_Num_int ( this, IDC_EDIT_START_ITEM_MID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidITEM.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_START_ITEM_SID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidITEM.wSubID );

	SetWin_Num_int ( this, IDC_EDIT_START_SKILL_MID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidSKILL.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_START_SKILL_SID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidSKILL.wSubID );

	SetWin_Num_int ( this, IDC_EDIT_START_LEVEL, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wLEVEL );

	SetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_MID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidMAP.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_SID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidMAP.wSubID );

	SetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_X, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wPosX );
	SetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_Y, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wPosY );

	//Attendance Only
	SetWin_Combo_Sel(this,IDC_COMBO_ATTENDANCE_TYPE,CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.nType);
	SetWin_Num_int ( this, IDC_EDIT_LEVEL_LIMIT_MIN, CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.wMinLevel );
	SetWin_Num_int ( this, IDC_EDIT_LEVEL_LIMIT_MAX, CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.wMaxLevel );

	DWORD dwDayofWeek = CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.dwDayofWeek;

	if( EMF_SUN & dwDayofWeek )
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_SUN, true );
	else
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_SUN, false );

	if( EMF_MON & dwDayofWeek )
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_MON, true );
	else
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_MON, false );

	if( EMF_TUE & dwDayofWeek )
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_TUE, true );
	else
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_TUE, false );

	if( EMF_WED & dwDayofWeek )
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_WED, true );
	else
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_WED, false );

	if( EMF_THU & dwDayofWeek )
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_THU, true );
	else
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_THU, false );

	if( EMF_FRI & dwDayofWeek )
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_FRI, true );
	else
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_FRI, false );

	if( EMF_SAT & dwDayofWeek )
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_SAT, true );
	else
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_SAT, false );

	if( (EMF_ALL & dwDayofWeek) == EMF_ALL )
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_ALL, true );
	else
		SetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_ALL, false );

	if( CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.bAdditionalQuest )
		SetWin_Check( this, IDC_CHECK_ADDITIONALQUEST, true );
	else
		SetWin_Check( this, IDC_CHECK_ADDITIONALQUEST, false );

	SetWin_Combo_Sel(this,IDC_COMBO_START_YEAR,CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.wStartYear);
	SetWin_Combo_Sel(this,IDC_COMBO_START_MONTH,CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.WStarMonth);
	SetWin_Combo_Sel(this,IDC_COMBO_START_DAY,CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.WstarDay);
	SetWin_Combo_Sel(this,IDC_COMBO_END_YEAR,CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.wEndYear);
	SetWin_Combo_Sel(this,IDC_COMBO_END_MONTH,CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.WEndMonth);
	SetWin_Combo_Sel(this,IDC_COMBO_END_DAY,CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.WEndDay);


    UpdateItemsGiftItem();
    UpdateItemsGiftSkill();
	UpdateProgress();
}

void CQuestPage::UpdateItemsGiftItem() 
{
    m_GiftItemList.DeleteAllItems();

    INVENQARRAY_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM.begin();
    INVENQARRAY_ITER iter_end = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM.end();
    for (int i=0 ; iter!=iter_end; ++iter, ++i)
    {
        const SITEMCUSTOM &sITEM = (*iter);

        CString strNAME = "UNFOUND";
        SITEM *pITEM = GLogicData::GetInstance().GetItem ( sITEM.GetNativeID() );
        if ( pITEM )
        {
            strNAME.Format( "%03d/%03d %s", pITEM->sBasicOp.sNativeID.wMainID, pITEM->sBasicOp.sNativeID.wSubID,
                pITEM->GetName() );
        }

        int nIndex = m_GiftItemList.InsertItem( i, strNAME );

        CString strTemp;
        strTemp.Format( "%hd", sITEM.wTurnNum );
        m_GiftItemList.SetItemText( nIndex, 1, strTemp );

        strTemp.Format( "%d/%d", (int)sITEM.cDAMAGE, (int)sITEM.cDEFENSE );
        m_GiftItemList.SetItemText( nIndex, 2, strTemp );

        strTemp.Format( "%d/%d/%d/%d/%d", (int)sITEM.cRESIST_ELEC, (int)sITEM.cRESIST_FIRE, 
            (int)sITEM.cRESIST_ICE, (int)sITEM.cRESIST_POISON, (int)sITEM.cRESIST_SPIRIT );
        m_GiftItemList.SetItemText( nIndex, 3, strTemp );
        m_GiftItemList.SetItemData ( nIndex, i );
    }
}

void CQuestPage::UpdateItemsGiftSkill() 
{
    CListBox* pListBox = (CListBox*) GetDlgItem(IDC_LIST_GIFT_SKILL);
    pListBox->ResetContent();

    DWQARRAY_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL.begin();
    DWQARRAY_ITER iter_end = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL.end();
    for (int i=0 ; iter!=iter_end; ++iter, ++i)
    {
        SNATIVEID sSKILLNID;
        sSKILLNID.dwID = (*iter);

        CString strNAME = "UNFOUND";
        GLSKILL* pSKILL = GLSkillMan::GetInstance().GetData(sSKILLNID);
        if (pSKILL)
            strNAME = pSKILL->GetName();

        int nIndex = pListBox->AddString(strNAME);
        pListBox->SetItemData(nIndex, i);
    }
}

void CQuestPage::InverseUpdateItems ()
{
	if ( !m_bDlgInit )	return;

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.dwID = (DWORD) GetWin_Num_int ( this, IDC_EDIT_QUEST_NID );
	CQuestEditDlg::m_sQuestNode.strFILE = GetWin_Text ( this, IDC_EDIT_QUEST_FILENAME );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwEDITVER = GetWin_Num_int ( this, IDC_EDIT_QUEST_FILENAME );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwEDITVER = GetWin_Num_int ( this, IDC_EDIT_EDITVERSION );	
	

	CQuestEditDlg::m_sQuestNode.pQUEST->m_strTITLE = GetWin_Text ( this, IDC_EDIT_TITLE ).GetString();
	CQuestEditDlg::m_sQuestNode.pQUEST->m_strCOMMENT = GetWin_Text ( this, IDC_EDIT_COMMENT ).GetString();

	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwBeginMoney = (DWORD) GetWin_Num_int ( this, IDC_EDIT_BEGIN_MONEY );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwBeginPartyMemNum = (DWORD) GetWin_Num_int ( this, IDC_EDIT_BEGIN_PARTYNUM );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwLimitTime = (DWORD ) GetWin_Num_int ( this, IDC_EDIT_LIMIT_TIME );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwLimitPartyMemNum = (DWORD) GetWin_Num_int ( this, IDC_EDIT_LIMIT_PARTYNUM );
	
	CQuestEditDlg::m_sQuestNode.pQUEST->m_bNonDie = GetWin_Check ( this, IDC_CHECK_LIMIT_NONDIE ) != FALSE;

	GetWin_Check ( this, IDC_CHECK_DAILY, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_DAILY );
	GetWin_Check ( this, IDC_CHECK_REPEAT, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_REPEAT );
	GetWin_Check ( this, IDC_CHECK_AGAIN, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_AGAIN );
	GetWin_Check ( this, IDC_CHECK_ONLYONE_PROG, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_ONLYONE );
	GetWin_Check ( this, IDC_CHECK_GIVEUP, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_GIVEUP );

	GetWin_Check ( this, IDC_CHECK_PARTY, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_PARTY );
	GetWin_Check ( this, IDC_CHECK_PARTYQUEST, CQuestEditDlg::m_sQuestNode.pQUEST->m_dwFlags, EMQF_PARTYQUEST );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_lnGiftEXP = GetWin_Num_LONGLONG ( this, IDC_EDIT_GIFT_EXP );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftMONEY = GetWin_Num_int ( this, IDC_EDIT_GIFT_MONEY );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftELEMENT = GetWin_Num_int ( this, IDC_EDIT_GIFT_ELEMENT );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftLIFEPOINT = GetWin_Num_int ( this, IDC_EDIT_GIFT_LIFEP );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftDisPK = GetWin_Num_int ( this, IDC_EDIT_GIFT_DISPK );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftSKILLPOINT = GetWin_Num_int ( this, IDC_EDIT_GIFT_SKILLP );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftSTATSPOINT = GetWin_Num_int ( this, IDC_EDIT_GIFT_STATP );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_dwGiftQUEST = GetWin_Num_int ( this, IDC_EDIT_GIFT_QUEST );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS = GLCC_NONE;
	GetWin_Check ( this, IDC_CHECK_FIGHETER_M, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_FIGHTER_M );
	GetWin_Check ( this, IDC_CHECK_ARMS_M, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ARMS_M );
	GetWin_Check ( this, IDC_CHECK_ARCHER_W, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ARCHER_W );
	GetWin_Check ( this, IDC_CHECK_SPIRIT_W, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_SPIRIT_W );

	/* 새 클래스 추가 */
	GetWin_Check ( this, IDC_CHECK_FIGHETER_W, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_FIGHTER_W );
	GetWin_Check ( this, IDC_CHECK_ARMS_W, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ARMS_W );
	GetWin_Check ( this, IDC_CHECK_ARCHER_M, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ARCHER_M );
	GetWin_Check ( this, IDC_CHECK_SPIRIT_M, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_SPIRIT_M );

	GetWin_Check ( this, IDC_CHECK_EXTREME_M, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_EXTREME_M );
	GetWin_Check ( this, IDC_CHECK_EXTREME_W, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_EXTREME_W );
	
	GetWin_Check ( this, IDC_CHECK_SCIENTIST_M, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_SCIENTIST_M );
	GetWin_Check ( this, IDC_CHECK_SCIENTIST_W, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_SCIENTIST_W );

    GetWin_Check( this, IDC_CHECK_ASSASSIN_M, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ASSASSIN_M );
    GetWin_Check( this, IDC_CHECK_ASSASSIN_W, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ASSASSIN_W );

    GetWin_Check( this, IDC_CHECK_TRICKER_M, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_TRICKER_M );
    GetWin_Check( this, IDC_CHECK_TRICKER_W, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_TRICKER_W );

	GetWin_Check( this, IDC_CHECK_ACTOR_M, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ACTOR_M );
	GetWin_Check( this, IDC_CHECK_ACTOR_W, CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.dwCLASS, GLCC_ACTOR_W );

	if ( GetWin_Combo_Sel ( this, IDC_RADIO_SCHOOL1 ) ) CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wSCHOOL = 0;
	else if ( GetWin_Combo_Sel ( this, IDC_RADIO_SCHOOL2 ) ) CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wSCHOOL = 1;
	else if ( GetWin_Combo_Sel ( this, IDC_RADIO_SCHOOL3 ) ) CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wSCHOOL = 2;
	else if ( GetWin_Combo_Sel ( this, IDC_RADIO_SCHOOL_ALL ) ) CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wSCHOOL = MAX_SCHOOL;

	if ( GetWin_Check ( this, IDC_RADIO_TIMEOVER1 ) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_bTimeLimiorKeep = false;
	}
	else if ( GetWin_Check ( this, IDC_RADIO_TIMEOVER2 ) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_bTimeLimiorKeep = true;
	}
 
	CQuestEditDlg::m_sQuestNode.pQUEST->m_bPastOnlyOnOline = GetWin_Check(this, IDC_CHECK_PAST_ON_ONLINE);

	int nFlag = CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nJOIN;

	if ( GetWin_Check( this, IDC_RADIO_AUTO_START_NONE) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nJOIN = EMQJ_NONE;
	}
	else if ( GetWin_Check( this, IDC_RADIO_AUTO_START_ATTENDANCE))
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nJOIN = EMQJ_ATTENDANCE;
	}
	else if ( GetWin_Check( this, IDC_RADIO_AUTO_START_JOIN_START) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nJOIN = EMQJ_JOIN;
	}

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidITEM.wMainID = GetWin_Num_int ( this, IDC_EDIT_START_ITEM_MID );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidITEM.wSubID = GetWin_Num_int ( this, IDC_EDIT_START_ITEM_SID );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidSKILL.wMainID = GetWin_Num_int ( this, IDC_EDIT_START_SKILL_MID );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidSKILL.wSubID = GetWin_Num_int ( this, IDC_EDIT_START_SKILL_SID );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wLEVEL = GetWin_Num_int ( this, IDC_EDIT_START_LEVEL );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidMAP.wMainID = GetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_MID );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.nidMAP.wSubID = GetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_SID );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wPosX = GetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_X );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sSTARTOPT.wPosY = GetWin_Num_int ( this, IDC_EDIT_START_QUEST_MAP_Y );

	//Attendance Only
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.nType = GetWin_Combo_Sel(this,IDC_COMBO_ATTENDANCE_TYPE);
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.wMinLevel = GetWin_Num_int ( this, IDC_EDIT_LEVEL_LIMIT_MIN );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.wMaxLevel = GetWin_Num_int ( this, IDC_EDIT_LEVEL_LIMIT_MAX );

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.dwDayofWeek = 0;

	if ( GetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_SUN) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.dwDayofWeek |= EMF_SUN;
	}

	if ( GetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_MON) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.dwDayofWeek |= EMF_MON;
	}

	if ( GetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_TUE) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.dwDayofWeek |= EMF_TUE;
	}

	if ( GetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_WED) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.dwDayofWeek |= EMF_WED;
	}

	if ( GetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_THU) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.dwDayofWeek |= EMF_THU;
	}

	if ( GetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_FRI) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.dwDayofWeek |= EMF_FRI;
	}

	if ( GetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_SAT) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.dwDayofWeek |= EMF_SAT;
	}

	if ( GetWin_Check( this, IDC_CHECK_DAYOFWEEK_PRIORITY_ALL) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.dwDayofWeek |= EMF_ALL;
	}

	if( GetWin_Check( this, IDC_CHECK_ADDITIONALQUEST) )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.bAdditionalQuest = true;
	}

	CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.wStartYear = atoi( GetWin_Text(this,IDC_COMBO_START_YEAR) );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.WStarMonth = atoi( GetWin_Text(this,IDC_COMBO_START_MONTH) );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.WstarDay = atoi( GetWin_Text(this,IDC_COMBO_START_DAY) );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.wEndYear = atoi( GetWin_Text(this,IDC_COMBO_END_YEAR) );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.WEndMonth = atoi( GetWin_Text(this,IDC_COMBO_END_MONTH) );
	CQuestEditDlg::m_sQuestNode.pQUEST->m_sAttendanceOption.WEndDay = atoi( GetWin_Text(this,IDC_COMBO_END_DAY) );

	InverseUpdatePrograss();
}

void CQuestPage::OnBnClickedButtonGiftItemNew()
{
	InverseUpdateItems();

	CDlgItem sDlgItem;
	if ( IDOK == sDlgItem.DoModal () )
	{
		SITEMCUSTOM sITEMCUSTOM(sDlgItem.m_nidITEM);

		SITEM* pITEM = GLogicData::GetInstance().GetItem ( sITEMCUSTOM.GetNativeID() );
		if ( !pITEM )	return;

		if ( pITEM->ISPILE() )	sITEMCUSTOM.wTurnNum = pITEM->GETAPPLYNUM();

		CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM.push_back(sITEMCUSTOM);

		UpdateItems ();
	}
}

void CQuestPage::OnBnClickedButtonGiftItemEdit()
{
	InverseUpdateItems();

	int nSelect = m_GiftItemList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 ) return;

	DWORD dwIndex = (DWORD)m_GiftItemList.GetItemData( nSelect );
	SITEMCUSTOM &sITEM = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM[dwIndex];

	CDlgCustomItem sDlgCustom;
	sDlgCustom.m_sITEMCUSTOM = sITEM;

	if ( IDOK==sDlgCustom.DoModal() )
	{
		sITEM = sDlgCustom.m_sITEMCUSTOM;
		UpdateItems ();
	}
}

void CQuestPage::OnBnClickedButtonGiftItemDel()
{
	InverseUpdateItems();

	int nSelect = m_GiftItemList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 ) return;

	DWORD dwIndex = (DWORD)m_GiftItemList.GetItemData( nSelect );
	SITEMCUSTOM &sITEM = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM[dwIndex];

	CString strNAME = "UNFOUND";
	SITEM *pITEM = GLogicData::GetInstance().GetItem ( sITEM.GetNativeID() );
	if ( pITEM )	strNAME = pITEM->GetName();

	int nReturn = MessageBox ( "Do you want to delete 'Item'?", strNAME, MB_YESNO );
	if ( nReturn==IDYES )
	{
		INVENQARRAY_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM.begin()+dwIndex;
		CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftITEM.erase ( iter, iter+1 );

		UpdateItems ();
	}
}

void CQuestPage::OnBnClickedButtonGiftSkillNew()
{
	InverseUpdateItems();

	CDlgSkill sDlgSkill;

	if ( IDOK==sDlgSkill.DoModal() )
	{
		CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL.push_back ( sDlgSkill.m_nidSKILL.dwID );

		UpdateItems ();
	}
}

void CQuestPage::OnBnClickedButtonGiftSkillDel()
{
	InverseUpdateItems();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_GIFT_SKILL );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR == nIndex )	return;
	
	DWORD dwIndex = (DWORD) pListBox->GetItemData ( nIndex );
	DWORD &dwSKILL_NID = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL[dwIndex];

	SNATIVEID sSKILLNID;
	sSKILLNID.dwID = dwSKILL_NID;
	
	CString strNAME = "UNFOUND";
	PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( sSKILLNID );
	if ( pSKILL )	strNAME = pSKILL->GetName();

	int nReturn = MessageBox ( "Do you want to delete 'Skill'?", strNAME, MB_YESNO );
	if ( nReturn==IDYES )
	{
		DWQARRAY_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL.begin()+dwIndex;
		CQuestEditDlg::m_sQuestNode.pQUEST->m_vecGiftSKILL.erase ( iter, iter+1 );

		UpdateItems ();
	}
}


void CQuestPage::OnBnClickedButtonPrev()
{
	//	아무것도 하지 않는다.
}

void CQuestPage::OnBnClickedButtonNext()
{
	InverseUpdateItems();
	GLQuestMan::GetInstance().DoModified ();

	//	다음 페이지로 이동.
	m_pSheetTab->ActiveQuestStepPage( QUEST_PAGE, NULL );
}

void CQuestPage::OnBnClickedButtonEditcancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	m_pSheetTab->ActiveQuestTreePage ();
}

void CQuestPage::OnBnClickedButtonEditcomplate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	InverseUpdateItems();

	GLQUEST_PROGRESS& ProgressEvent = CQuestEditDlg::m_sQuestNode.pQUEST->m_ProgressEvent;

	if( ProgressEvent.bUseProgressEvent )
	{
		if( ProgressEvent.dwFailProgress != EMQP_NON_EVENT )
		{
			if( ProgressEvent.dwfailType == EMFAIL_NONE )
			{
				MessageBox( "Select FailType", "Error" );
				return;
			}
		}

		if( ProgressEvent.dwStartProgress == EMQP_SELECT_MOVE )
		{
			if ( ProgressEvent.startMap.nidMAP.dwID == UINT_MAX )
			{
				MessageBox( "Map Select ", "ProgressEvent StartMap" );
				return;
			}
			else
			{
				if ( ( ProgressEvent.startMap.wPosX == USHRT_MAX || 
					   ProgressEvent.startMap.wPosY == USHRT_MAX ) && 
					   ProgressEvent.startMap.dwGateID == 0 )
				{
					MessageBox( "only one of Pos and Gate settings", "ProgressEvent StartMap" );
					return;
				}

				if( ProgressEvent.startMap.wPosX != USHRT_MAX &&
					ProgressEvent.startMap.wPosY != USHRT_MAX &&
					ProgressEvent.startMap.dwGateID != 0 )
				{
					MessageBox( "only one of Pos and Gate settings", "ProgressEvent StartMap" );
					return;					
				}
				
			}		
		}

		if( ProgressEvent.dwEndProgress == EMQP_SELECT_MOVE )
		{
			if ( ProgressEvent.endMap.nidMAP.dwID == UINT_MAX )
			{
				MessageBox( "Map Select ", "ProgressEvent EndMap" );
				return;
			}
			else
			{
				if ( ( ProgressEvent.endMap.wPosX == USHRT_MAX || 
					   ProgressEvent.endMap.wPosY == USHRT_MAX ) && 
					   ProgressEvent.endMap.dwGateID == 0 )
				{
					MessageBox( "only one of Pos and Gate settings", "ProgressEvent EndMap" );
					return;
				}

				if( ProgressEvent.endMap.wPosX != USHRT_MAX &&
					ProgressEvent.endMap.wPosY != USHRT_MAX &&
					ProgressEvent.endMap.dwGateID != 0 )
				{
					MessageBox( "only one of Pos and Gate settings", "ProgressEvent EndMap" );
					return;					
				}
				
			}		
		}

		if( ProgressEvent.dwFailProgress == EMQP_SELECT_MOVE )
		{
			if ( ProgressEvent.failMap.nidMAP.dwID == UINT_MAX )
			{
				MessageBox( "Map Select ", "ProgressEvent FailMap" );
				return;
			}
			else
			{
				if ( ( ProgressEvent.failMap.wPosX == USHRT_MAX || 
					   ProgressEvent.failMap.wPosY == USHRT_MAX ) && 
					   ProgressEvent.failMap.dwGateID == 0 )
				{
					MessageBox( "only one of Pos and Gate settings", "ProgressEvent FailMap" );
					return;
				}

				if( ProgressEvent.failMap.wPosX != USHRT_MAX &&
					ProgressEvent.failMap.wPosY != USHRT_MAX &&
					ProgressEvent.failMap.dwGateID != 0 )
				{
					MessageBox( "only one of Pos and Gate settings", "ProgressEvent FailMap" );
					return;					
				}
				
			}		
		}
	}

	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos = m_mapQuestMap.find( CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.dwID );
	if ( iter_pos == m_mapQuestMap.end() ) return;

	
	if ( iter_pos->second.strFILE != CQuestEditDlg::m_sQuestNode.strFILE )
	{
		
		CString strTemp = GLQuestMan::GetInstance().GetPath();

		strTemp += iter_pos->second.strFILE.c_str();
		if ( !DeleteFile( strTemp ) )
		{
			MessageBox( "Not Found File",strTemp, MB_OK ) ;
		}

		iter_pos->second.strFILE = CQuestEditDlg::m_sQuestNode.strFILE;
	}	
	
    *(iter_pos->second.pQUEST) = *(CQuestEditDlg::m_sQuestNode.pQUEST);

	iter_pos->second.pQUEST->SAVE( iter_pos->second.strFILE.c_str() );
	
	// String Table Save;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		const int nMid = CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.Mid();
		CString strQstKey_Name, strQstKey_Desc;
		strQstKey_Name.Format("QN_%03d_000", nMid);
		strQstKey_Desc.Format("QD_%03d_000", nMid);
		CString strQuestName = GetWin_Text(this, IDC_EDIT_TITLE);
		CString strQuestDesc = GetWin_Text(this, IDC_EDIT_COMMENT);
		
		CGameTextMan::GetInstance().InsertString(
			strQstKey_Name.GetString()
			, strQuestName.GetString()
			, CGameTextMan::EM_QUEST_TEXT);
		
		CGameTextMan::GetInstance().InsertString(
			strQstKey_Desc.GetString()
			, strQuestDesc.GetString()
			, CGameTextMan::EM_QUEST_TEXT);

		int i, nStepSize = CQuestEditDlg::m_sQuestNode.pQUEST->GetSTEPNUM();
		for ( i = 0; i < nStepSize; ++i )
		{
			CString strProgKey;	strProgKey.Format("QN_%03d_000_P_%d", nMid, i);
			CString strDescKey;	strDescKey.Format("QD_%03d_000_D_%d", nMid, i);
			const CString strProg_s = CSingleobjectTextControl::GetInstance().GetText(strProgKey.GetString());
			const CString strDesc_s = CSingleobjectTextControl::GetInstance().GetText(strDescKey.GetString());
			CGameTextMan::GetInstance().InsertString(strProgKey.GetString(), strProg_s.GetString(), CGameTextMan::EM_QUEST_TEXT);
			CGameTextMan::GetInstance().InsertString(strDescKey.GetString(), strDesc_s.GetString(), CGameTextMan::EM_QUEST_TEXT);
		}
	}

	m_pSheetTab->ActiveQuestTreePage ();
}

void CQuestPage::SetReadMode ()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망  
#ifdef READTOOL_PARAM

	const int nSkipNum = 6;
	const int nSkipID[nSkipNum] = { IDC_EDIT_TITLE, IDC_EDIT_COMMENT, IDC_BUTTON_PREV,
									IDC_BUTTON_NEXT, IDC_BUTTON_EDITCANCEL, IDC_BUTTON_EDITCOMPLATE };

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
void CQuestPage::OnBnClickedUseProgressCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CQuestEditDlg::m_sQuestNode.pQUEST->m_ProgressEvent.bUseProgressEvent = GetWin_Check( this, IDC_USE_PROGRESS_CHECK );

	UpdateProgress();
}

void CQuestPage::UpdateProgress()
{
	
	GLQUEST_PROGRESS& ProgressEvent = CQuestEditDlg::m_sQuestNode.pQUEST->m_ProgressEvent;

	SetWin_Enable( this, IDC_STATE_COMBO1, FALSE );
	SetWin_Enable( this, IDC_STATE_COMBO2, FALSE );
	SetWin_Enable( this, IDC_STATE_COMBO3, FALSE );
	SetWin_Enable( this, IDC_FAIL_CHECK1, FALSE );
	SetWin_Enable( this, IDC_FAIL_CHECK2, FALSE );
	SetWin_Enable( this, IDC_FAIL_CHECK3, FALSE );

	SetWin_Enable( this, IDC_MAP_XPOS_EDIT, FALSE );
	SetWin_Enable( this, IDC_MAP_YPOS_EDIT, FALSE );
	SetWin_Enable( this, IDC_MAP_GATE_EDIT, FALSE );
	
	SetWin_Enable( this, IDC_MAP_XPOS_EDIT2, FALSE );
	SetWin_Enable( this, IDC_MAP_YPOS_EDIT2, FALSE );
	SetWin_Enable( this, IDC_MAP_GATE_EDIT2, FALSE );
	
	SetWin_Enable( this, IDC_MAP_XPOS_EDIT3, FALSE );
	SetWin_Enable( this, IDC_MAP_YPOS_EDIT3, FALSE );
	SetWin_Enable( this, IDC_MAP_GATE_EDIT3, FALSE );

	SetWin_Enable( this, IDC_MAP_EDIT_BUTTON, FALSE );
	SetWin_Enable( this, IDC_MAP_MID_EDIT, FALSE );
	SetWin_Enable( this, IDC_MAP_SID_EDIT, FALSE );

	SetWin_Enable( this, IDC_MAP_EDIT_BUTTON2, FALSE );
	SetWin_Enable( this, IDC_MAP_MID_EDIT2, FALSE );
	SetWin_Enable( this, IDC_MAP_SID_EDIT2, FALSE );

	SetWin_Enable( this, IDC_MAP_EDIT_BUTTON3, FALSE );
	SetWin_Enable( this, IDC_MAP_MID_EDIT3, FALSE );
	SetWin_Enable( this, IDC_MAP_SID_EDIT3, FALSE );

	SetWin_Check( this, IDC_USE_PROGRESS_CHECK, FALSE );


	if( ProgressEvent.bUseProgressEvent )
	{
		SetWin_Enable( this, IDC_STATE_COMBO1, TRUE );
		SetWin_Enable( this, IDC_STATE_COMBO2, TRUE );
		SetWin_Enable( this, IDC_STATE_COMBO3, TRUE );
		SetWin_Enable( this, IDC_FAIL_CHECK1, TRUE );
		SetWin_Enable( this, IDC_FAIL_CHECK2, TRUE );
		SetWin_Enable( this, IDC_FAIL_CHECK3, TRUE );

		SetWin_Check( this, IDC_USE_PROGRESS_CHECK, TRUE );
	}	

	
	if ( ProgressEvent.dwStartProgress == EMQP_SELECT_MOVE )
	{
		SetWin_Enable( this, IDC_MAP_EDIT_BUTTON, TRUE );
		SetWin_Enable( this, IDC_MAP_MID_EDIT, TRUE );
		SetWin_Enable( this, IDC_MAP_SID_EDIT, TRUE );

		SetWin_Enable( this, IDC_MAP_XPOS_EDIT, TRUE );
		SetWin_Enable( this, IDC_MAP_YPOS_EDIT, TRUE );
		SetWin_Enable( this, IDC_MAP_GATE_EDIT, TRUE );
	}


	if ( ProgressEvent.dwEndProgress == EMQP_SELECT_MOVE )
	{
		SetWin_Enable( this, IDC_MAP_EDIT_BUTTON2, TRUE );
		SetWin_Enable( this, IDC_MAP_MID_EDIT2, TRUE );
		SetWin_Enable( this, IDC_MAP_SID_EDIT2, TRUE );

		SetWin_Enable( this, IDC_MAP_XPOS_EDIT2, TRUE );
		SetWin_Enable( this, IDC_MAP_YPOS_EDIT2, TRUE );
		SetWin_Enable( this, IDC_MAP_GATE_EDIT2, TRUE );
	}


	if ( ProgressEvent.dwFailProgress == EMQP_SELECT_MOVE )
	{
		SetWin_Enable( this, IDC_MAP_EDIT_BUTTON3, TRUE );
		SetWin_Enable( this, IDC_MAP_MID_EDIT3, TRUE );
		SetWin_Enable( this, IDC_MAP_SID_EDIT3, TRUE );

		SetWin_Enable( this, IDC_MAP_XPOS_EDIT3, TRUE );
		SetWin_Enable( this, IDC_MAP_YPOS_EDIT3, TRUE );
		SetWin_Enable( this, IDC_MAP_GATE_EDIT3, TRUE );
	}

	SetWin_Check( this, IDC_FAIL_CHECK1, ProgressEvent.IsTIMEOVER() );
	SetWin_Check( this, IDC_FAIL_CHECK2, ProgressEvent.IsDIE() );
	SetWin_Check( this, IDC_FAIL_CHECK3, ProgressEvent.IsLEAVE() );

	SetWin_Combo_Sel( this, IDC_STATE_COMBO1, ProgressEvent.dwStartProgress );
	SetWin_Combo_Sel( this, IDC_STATE_COMBO2, ProgressEvent.dwEndProgress );
	SetWin_Combo_Sel( this, IDC_STATE_COMBO3, ProgressEvent.dwFailProgress );

	SetWin_Num_int( this, IDC_MAP_MID_EDIT,  ProgressEvent.startMap.nidMAP.wMainID );
	SetWin_Num_int( this, IDC_MAP_SID_EDIT,  ProgressEvent.startMap.nidMAP.wSubID );

	SetWin_Num_int( this, IDC_MAP_XPOS_EDIT,  ProgressEvent.startMap.wPosX );
	SetWin_Num_int( this, IDC_MAP_YPOS_EDIT,  ProgressEvent.startMap.wPosY );
	SetWin_Num_int( this, IDC_MAP_GATE_EDIT,  ProgressEvent.startMap.dwGateID );

	SetWin_Num_int( this, IDC_MAP_MID_EDIT2,  ProgressEvent.endMap.nidMAP.wMainID );
	SetWin_Num_int( this, IDC_MAP_SID_EDIT2,  ProgressEvent.endMap.nidMAP.wSubID );

	SetWin_Num_int( this, IDC_MAP_XPOS_EDIT2,  ProgressEvent.endMap.wPosX );
	SetWin_Num_int( this, IDC_MAP_YPOS_EDIT2,  ProgressEvent.endMap.wPosY );
	SetWin_Num_int( this, IDC_MAP_GATE_EDIT2,  ProgressEvent.endMap.dwGateID );

	SetWin_Num_int( this, IDC_MAP_MID_EDIT3,  ProgressEvent.failMap.nidMAP.wMainID );
	SetWin_Num_int( this, IDC_MAP_SID_EDIT3,  ProgressEvent.failMap.nidMAP.wSubID );

	SetWin_Num_int( this, IDC_MAP_XPOS_EDIT3,  ProgressEvent.failMap.wPosX );
	SetWin_Num_int( this, IDC_MAP_YPOS_EDIT3,  ProgressEvent.failMap.wPosY );
	SetWin_Num_int( this, IDC_MAP_GATE_EDIT3,  ProgressEvent.failMap.dwGateID );
}

void CQuestPage::InverseUpdatePrograss()
{
	if ( !m_bDlgInit )	return;

	GLQUEST_PROGRESS& ProgressEvent = CQuestEditDlg::m_sQuestNode.pQUEST->m_ProgressEvent;
	
	ProgressEvent.bUseProgressEvent = GetWin_Check( this, IDC_USE_PROGRESS_CHECK );

	if ( ProgressEvent.bUseProgressEvent )
	{
		GetWin_Check( this, IDC_FAIL_CHECK1, ProgressEvent.dwfailType, EMFAIL_TIMEOVER );
		GetWin_Check( this, IDC_FAIL_CHECK2, ProgressEvent.dwfailType, EMFAIL_DIE );
		GetWin_Check( this, IDC_FAIL_CHECK2, ProgressEvent.dwfailType, EMFAIL_LEAVE );

		ProgressEvent.dwStartProgress = GetWin_Combo_Sel( this, IDC_STATE_COMBO1 );
		ProgressEvent.dwEndProgress = GetWin_Combo_Sel( this, IDC_STATE_COMBO2 );
		ProgressEvent.dwFailProgress = GetWin_Combo_Sel( this, IDC_STATE_COMBO3 );

		if ( ProgressEvent.dwStartProgress == EMQP_SELECT_MOVE )
		{
			ProgressEvent.startMap.nidMAP.wMainID = GetWin_Num_int( this, IDC_MAP_MID_EDIT );
			ProgressEvent.startMap.nidMAP.wSubID = GetWin_Num_int( this, IDC_MAP_SID_EDIT );

			ProgressEvent.startMap.wPosX = GetWin_Num_int( this, IDC_MAP_XPOS_EDIT );
			ProgressEvent.startMap.wPosY = GetWin_Num_int( this, IDC_MAP_YPOS_EDIT );
			ProgressEvent.startMap.dwGateID = GetWin_Num_int( this, IDC_MAP_GATE_EDIT );
		}

		if ( ProgressEvent.dwEndProgress == EMQP_SELECT_MOVE )
		{
			ProgressEvent.endMap.nidMAP.wMainID = GetWin_Num_int( this, IDC_MAP_MID_EDIT2 );
			ProgressEvent.endMap.nidMAP.wSubID = GetWin_Num_int( this, IDC_MAP_SID_EDIT2 );

			ProgressEvent.endMap.wPosX = GetWin_Num_int( this, IDC_MAP_XPOS_EDIT2 );
			ProgressEvent.endMap.wPosY = GetWin_Num_int( this, IDC_MAP_YPOS_EDIT2 );
			ProgressEvent.endMap.dwGateID = GetWin_Num_int( this, IDC_MAP_GATE_EDIT2 );
		}

		if ( ProgressEvent.dwFailProgress == EMQP_SELECT_MOVE )
		{
			ProgressEvent.failMap.nidMAP.wMainID = GetWin_Num_int( this, IDC_MAP_MID_EDIT3 );
			ProgressEvent.failMap.nidMAP.wSubID = GetWin_Num_int( this, IDC_MAP_SID_EDIT3 );

			ProgressEvent.failMap.wPosX = GetWin_Num_int( this, IDC_MAP_XPOS_EDIT3 );
			ProgressEvent.failMap.wPosY = GetWin_Num_int( this, IDC_MAP_YPOS_EDIT3 );
			ProgressEvent.failMap.dwGateID = GetWin_Num_int( this, IDC_MAP_GATE_EDIT3 );
		}
	}
	else
	{
		ProgressEvent.Init();
	}
}



void CQuestPage::OnCbnSelchangeStateCombo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	 
	BOOL bSel = ( EMQP_SELECT_MOVE == GetWin_Combo_Sel( this, IDC_STATE_COMBO1 ) );
	
	SetWin_Enable( this, IDC_MAP_EDIT_BUTTON, bSel );
	SetWin_Enable( this, IDC_MAP_MID_EDIT, bSel );
	SetWin_Enable( this, IDC_MAP_SID_EDIT, bSel );

	SetWin_Enable( this, IDC_MAP_XPOS_EDIT, bSel );
	SetWin_Enable( this, IDC_MAP_YPOS_EDIT, bSel );
	SetWin_Enable( this, IDC_MAP_GATE_EDIT, bSel );
}

void CQuestPage::OnCbnSelchangeStateCombo2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bSel = ( EMQP_SELECT_MOVE == GetWin_Combo_Sel( this, IDC_STATE_COMBO2 ) );
	
	SetWin_Enable( this, IDC_MAP_EDIT_BUTTON2, bSel );
	SetWin_Enable( this, IDC_MAP_MID_EDIT2, bSel );
	SetWin_Enable( this, IDC_MAP_SID_EDIT2, bSel );

	SetWin_Enable( this, IDC_MAP_XPOS_EDIT2, bSel );
	SetWin_Enable( this, IDC_MAP_YPOS_EDIT2, bSel );
	SetWin_Enable( this, IDC_MAP_GATE_EDIT2, bSel );
}

void CQuestPage::OnCbnSelchangeStateCombo3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bSel = ( EMQP_SELECT_MOVE == GetWin_Combo_Sel( this, IDC_STATE_COMBO3 ) );
	
	SetWin_Enable( this, IDC_MAP_EDIT_BUTTON3, bSel );
	SetWin_Enable( this, IDC_MAP_MID_EDIT3, bSel );
	SetWin_Enable( this, IDC_MAP_SID_EDIT3, bSel );

	SetWin_Enable( this, IDC_MAP_XPOS_EDIT3, bSel );
	SetWin_Enable( this, IDC_MAP_YPOS_EDIT3, bSel );
	SetWin_Enable( this, IDC_MAP_GATE_EDIT3, bSel );
}

void CQuestPage::OnBnClickedMapEditButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgMap mapSelect;
	if ( mapSelect.DoModal() == IDOK )
	{
		SetWin_Num_int( this, IDC_MAP_MID_EDIT, mapSelect.m_nidMAP.wMainID );
		SetWin_Num_int( this, IDC_MAP_SID_EDIT, mapSelect.m_nidMAP.wSubID );
	}
}

void CQuestPage::OnBnClickedMapEditButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgMap mapSelect;
	if ( mapSelect.DoModal() == IDOK )
	{
		SetWin_Num_int( this, IDC_MAP_MID_EDIT2, mapSelect.m_nidMAP.wMainID );
		SetWin_Num_int( this, IDC_MAP_SID_EDIT2, mapSelect.m_nidMAP.wSubID );
	}

}

void CQuestPage::OnBnClickedMapEditButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgMap mapSelect;
	if ( mapSelect.DoModal() == IDOK )
	{
		SetWin_Num_int( this, IDC_MAP_MID_EDIT3, mapSelect.m_nidMAP.wMainID );
		SetWin_Num_int( this, IDC_MAP_SID_EDIT3, mapSelect.m_nidMAP.wSubID );
	}
}

void CQuestPage::OnBnClickedRadioAll()
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

void CQuestPage::OnBnClickedRadioMan()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_MAN );

	SetWin_Check ( this, IDC_CHECK_ARCHER_W, false );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, false );	
	SetWin_Check ( this, IDC_CHECK_FIGHETER_W, false );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, false );	
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, false );
	SetWin_Check ( this, IDC_CHECK_SCIENTIST_W, false );
	SetWin_Check ( this, IDC_CHECK_ASSASSIN_W, false );
	SetWin_Check ( this, IDC_CHECK_TRICKER_W, false );
	SetWin_Check ( this, IDC_CHECK_ACTOR_W, false );
	
	SetWin_Check ( this, IDC_CHECK_FIGHETER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, bCheck );	
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, bCheck );	
	SetWin_Check ( this, IDC_CHECK_EXTREME_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_SCIENTIST_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ASSASSIN_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_TRICKER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ACTOR_M, bCheck );
}

void CQuestPage::OnBnClickedRadioWoman()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_WOMAN );

	SetWin_Check ( this, IDC_CHECK_ARCHER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, bCheck );	
	SetWin_Check ( this, IDC_CHECK_FIGHETER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_SCIENTIST_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ASSASSIN_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_TRICKER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ACTOR_W, bCheck );

	SetWin_Check ( this, IDC_CHECK_FIGHETER_M, false );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, false );	
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, false );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, false );	
	SetWin_Check ( this, IDC_CHECK_EXTREME_M, false );
	SetWin_Check ( this, IDC_CHECK_SCIENTIST_M, false );
	SetWin_Check ( this, IDC_CHECK_ASSASSIN_M, false );
	SetWin_Check ( this, IDC_CHECK_TRICKER_M, false );
	SetWin_Check ( this, IDC_CHECK_ACTOR_M, false );
}

void CQuestPage::OnBnClickedButtonQuestArea()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    InverseUpdateItems ();

    CSelectDialog dlg;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string> vecQUEST_AREA_NAME;
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("QUEST_AREA", vecQUEST_AREA_NAME, CGameTextMan::EM_COMMENT_TEXT);
		dlg.SetStringList(vecQUEST_AREA_NAME);
		dlg.m_nSelect = (int)CQuestEditDlg::m_sQuestNode.pQUEST->m_dwQuestArea;
		if ( dlg.DoModal() == IDOK )
		{
			CQuestEditDlg::m_sQuestNode.pQUEST->m_dwQuestArea = dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_QUEST_AREA_NAME, vecQUEST_AREA_NAME[CQuestEditDlg::m_sQuestNode.pQUEST->m_dwQuestArea].c_str() );
		}
	}
	else
	{
		std::vector<std::string> vecTemp;
		vecTemp.reserve( COMMENT::QUEST_AREA_NAME.size() );
		for ( int i = 0; i < static_cast <int>(COMMENT::QUEST_AREA_NAME.size()); i++ )
			vecTemp.push_back( COMMENT::QUEST_AREA_NAME.at( i ) );

		dlg.SetStringList( vecTemp);
		dlg.m_nSelect = (int)CQuestEditDlg::m_sQuestNode.pQUEST->m_dwQuestArea;
		if ( dlg.DoModal() == IDOK )
		{
			CQuestEditDlg::m_sQuestNode.pQUEST->m_dwQuestArea = dlg.m_dwData;
			SetWin_Text( this, IDC_EDIT_QUEST_AREA_NAME, COMMENT::QUEST_AREA_NAME.at( CQuestEditDlg::m_sQuestNode.pQUEST->m_dwQuestArea ).c_str() );
		}
	}
    return;
}

void CQuestPage::OnEnChangeEditLimitTime()
{
	BOOL bEnable = GetWin_Num_int( this, IDC_EDIT_LIMIT_TIME ) != 0 ? TRUE:FALSE;

	SetWin_Enable( this, IDC_RADIO_TIMEOVER1, bEnable );
	SetWin_Enable( this, IDC_RADIO_TIMEOVER2, bEnable );
	SetWin_Enable( this, IDC_CHECK_PAST_ON_ONLINE, bEnable );
}

void CQuestPage::OnBnClickedCheckDaily()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CQuestPage::InitAttendenceType()
{
	// 아래 COMMENT는 데이터에서 읽어오지않음;
	SetWin_Combo_Init(this, IDC_COMBO_ATTENDANCE_TYPE, COMMENT::QUEST_ATTENDANCE_TYPE, EMATTENDANCE_TYPE_SIZE);

	SetWin_Combo_Init_IntVal(this,IDC_COMBO_START_YEAR,2100 + 1);
	SetWin_Combo_Init_IntVal(this,IDC_COMBO_END_YEAR,2100 + 1);

	SetWin_Combo_Init_IntVal(this,IDC_COMBO_START_MONTH,12 + 1);
	SetWin_Combo_Init_IntVal(this,IDC_COMBO_END_MONTH,12 + 1);

	SetWin_Combo_Init_IntVal(this,IDC_COMBO_START_DAY,31 + 1);
	SetWin_Combo_Init_IntVal(this,IDC_COMBO_END_DAY,31 + 1);

	return;
}
