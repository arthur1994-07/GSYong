// RequirePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../../RanLogic/GLLevelFile.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Quest/GLQuestMan.h"

#include "../../RanLogicServer/FieldServer/GLGaeaServer.h"
#include "../../RanLogicServer/Land/GLLandMan.h"

#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../LevelEdit.h"
#include "../LevelEditDoc.h"
#include "../LevelEditView.h"
#include "../Dialogue/DlgQuest.h"

#include "./RequirePage.h"
#include "./LevelSheetWithTab.h"
#include "./RequirePage.h"
#include "LevelMainFrm.h"

// CRequirePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRequirePage, CPropertyPage)
CRequirePage::CRequirePage()
	: CPropertyPage(CRequirePage::IDD),
	m_pSheetTab(NULL),
	m_bInit(FALSE)
{
}

CRequirePage::~CRequirePage()
{
}

void CRequirePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRequirePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_SET, OnBnClickedButtonSaveSet)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_ITEM, OnBnClickedButtonItem)
	ON_BN_CLICKED(IDC_BUTTON_SKILL, OnBnClickedButtonSkill)
	ON_BN_CLICKED(IDC_BUTTON_RESET_ITEM, OnBnClickedButtonResetItem)
	ON_BN_CLICKED(IDC_BUTTON_RESET_SKILL, OnBnClickedButtonResetSkill)
	ON_BN_CLICKED(IDC_BUTTON_RESET_QUEST_COM, OnBnClickedButtonResetQuestCom)
	ON_BN_CLICKED(IDC_BUTTON_RESET_QUEST_PROC, OnBnClickedButtonResetQuestProc)
	ON_BN_CLICKED(IDC_BUTTON_COM_QUEST, OnBnClickedButtonComQuest)
	ON_BN_CLICKED(IDC_BUTTON_ACT_QUEST, OnBnClickedButtonActQuest)
	ON_CBN_SELCHANGE(IDC_COMBO_LEVEL, OnCbnSelchangeComboLevel)
	ON_BN_CLICKED(IDC_CHECK_FORCE_EXIT, &CRequirePage::OnBnClickedCheckForceExit)
	ON_BN_CLICKED(IDC_BUTTON_ITEM2, &CRequirePage::OnBnClickedButtonItem2)
	ON_BN_CLICKED(IDC_BUTTON_ITEM3, &CRequirePage::OnBnClickedButtonItem3)
	ON_BN_CLICKED(IDC_BUTTON_ITEM4, &CRequirePage::OnBnClickedButtonItem4)
	ON_BN_CLICKED(IDC_BUTTON_ITEM5, &CRequirePage::OnBnClickedButtonItem5)
	ON_BN_CLICKED(IDC_BUTTON_RESET_ITEM2, &CRequirePage::OnBnClickedButtonResetItem2)
	ON_BN_CLICKED(IDC_BUTTON_RESET_ITEM3, &CRequirePage::OnBnClickedButtonResetItem3)
	ON_BN_CLICKED(IDC_BUTTON_RESET_ITEM4, &CRequirePage::OnBnClickedButtonResetItem4)
	ON_BN_CLICKED(IDC_BUTTON_RESET_ITEM5, &CRequirePage::OnBnClickedButtonResetItem5)
END_MESSAGE_MAP()


void CRequirePage::SetReqItem ( SNATIVEID sNID, int iBtnNum )
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	switch( iBtnNum )
	{
	case IDC_BUTTON_ITEM  : pREQUIRE->m_sItemID = sNID;		    break;
	case IDC_BUTTON_ITEM2 : pREQUIRE->m_sItemIDMulti[0] = sNID; break;
	case IDC_BUTTON_ITEM3 : pREQUIRE->m_sItemIDMulti[1] = sNID; break;
	case IDC_BUTTON_ITEM4 : pREQUIRE->m_sItemIDMulti[2] = sNID; break;
	case IDC_BUTTON_ITEM5 : pREQUIRE->m_sItemIDMulti[3] = sNID; break;
	}
	
}

void CRequirePage::SetReqSkill ( SNATIVEID sNID )
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	pREQUIRE->m_sSkillID = sNID;
}

// CRequirePage 메시지 처리기입니다.
void CRequirePage::UpdatePage ()
{
	if (!m_bInit)
        return;

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;
	
	SetWin_Check ( this, IDC_CHECK_FORCE_EXIT, pREQUIRE->m_bForceExit );
	SetWin_Check ( this, IDC_CHECK_PARTY_AGREE, pREQUIRE->m_bPartyMbr );

	SetWin_Num_int ( this, IDC_EDIT_LEVEL, pREQUIRE->m_wLevel );
	SetWin_Num_int ( this, IDC_EDIT_LEVEL2, pREQUIRE->m_wLevel2 );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		SetWin_Combo_Sel (this, IDC_COMBO_LEVEL, 
			CGameTextMan::GetInstance().GetCommentText("CDT_SIGN", pREQUIRE->m_signLevel).GetString());
	}
	else
	{
		SetWin_Combo_Sel (this, IDC_COMBO_LEVEL, COMMENT::CDT_SIGN[pREQUIRE->m_signLevel]);
	}


	if( pREQUIRE->m_signLevel == EMSIGN_FROMTO )
	{
		SetWin_Enable( this, IDC_EDIT_LEVEL2, TRUE );
	}else{
		SetWin_Enable( this, IDC_EDIT_LEVEL2, FALSE );
	}
	
	SetWin_Num_int ( this, IDC_EDIT_ITEM_MID, pREQUIRE->m_sItemID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_ITEM_SID, pREQUIRE->m_sItemID.wSubID );

	SetWin_Num_int ( this, IDC_EDIT_ITEM_MID2, pREQUIRE->m_sItemIDMulti[0].wMainID );
	SetWin_Num_int ( this, IDC_EDIT_ITEM_SID2, pREQUIRE->m_sItemIDMulti[0].wSubID );
	SetWin_Num_int ( this, IDC_EDIT_ITEM_MID3, pREQUIRE->m_sItemIDMulti[1].wMainID );
	SetWin_Num_int ( this, IDC_EDIT_ITEM_SID3, pREQUIRE->m_sItemIDMulti[1].wSubID );
	SetWin_Num_int ( this, IDC_EDIT_ITEM_MID4, pREQUIRE->m_sItemIDMulti[2].wMainID );
	SetWin_Num_int ( this, IDC_EDIT_ITEM_SID4, pREQUIRE->m_sItemIDMulti[2].wSubID );
	SetWin_Num_int ( this, IDC_EDIT_ITEM_MID5, pREQUIRE->m_sItemIDMulti[3].wMainID );
	SetWin_Num_int ( this, IDC_EDIT_ITEM_SID5, pREQUIRE->m_sItemIDMulti[3].wSubID );

	CString strItemName;
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( pREQUIRE->m_sItemID );
	if ( pItem )	strItemName = pItem->GetName();
	else			strItemName = ("None");

	SetWin_Text ( this, IDC_EDIT_ITEM_NAME, strItemName.GetString() );

	pItem = GLogicData::GetInstance().GetItem ( pREQUIRE->m_sItemIDMulti[0] );
	if ( pItem )	strItemName = pItem->GetName();
	else			strItemName = ("None");

	SetWin_Text ( this, IDC_EDIT_ITEM_NAME2, strItemName.GetString() );

	pItem = GLogicData::GetInstance().GetItem ( pREQUIRE->m_sItemIDMulti[1] );
	if ( pItem )	strItemName = pItem->GetName();
	else			strItemName = ("None");

	SetWin_Text ( this, IDC_EDIT_ITEM_NAME3, strItemName.GetString() );

	pItem = GLogicData::GetInstance().GetItem ( pREQUIRE->m_sItemIDMulti[2] );
	if ( pItem )	strItemName = pItem->GetName();
	else			strItemName = ("None");

	SetWin_Text ( this, IDC_EDIT_ITEM_NAME4, strItemName.GetString() );

	pItem = GLogicData::GetInstance().GetItem ( pREQUIRE->m_sItemIDMulti[3] );
	if ( pItem )	strItemName = pItem->GetName();
	else			strItemName = ("None");

	SetWin_Text ( this, IDC_EDIT_ITEM_NAME5, strItemName.GetString() );

	SetWin_Num_int ( this, IDC_EDIT_SKILL_MID, pREQUIRE->m_sSkillID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_SKILL_SID, pREQUIRE->m_sSkillID.wSubID );

	CString strSkillName;
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pREQUIRE->m_sSkillID );
	if ( pSkill )	strSkillName = pSkill->GetName();
	else			strSkillName = ("None");

	SetWin_Text ( this, IDC_EDIT_SKILL_NAME, strSkillName.GetString() );

	GLQUEST *pQuest;
	CString strQuest;

	SetWin_Text ( this, IDC_EDIT_PROCQUEST, strQuest.GetString() );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
		pQuest = GLQuestMan::GetInstance().Find ( pREQUIRE->m_sComQuestID.dwID );
		strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
		if ( pQuest )	strQuest.Format ( "[%d] %s", pREQUIRE->m_sComQuestID.dwID, CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString());
		else			strQuest = ("None");

		SetWin_Text ( this, IDC_EDIT_COMQUEST, strQuest.GetString() );
		
		pQuest = GLQuestMan::GetInstance().Find ( pREQUIRE->m_sActQuestID.dwID );
		strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
		if ( pQuest )	strQuest.Format ( "[%d] %s", pREQUIRE->m_sActQuestID.dwID, CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString());
		else			strQuest = ("None");

		SetWin_Text ( this, IDC_EDIT_PROCQUEST, strQuest.GetString() );

		SetWin_Combo_Sel (this, IDC_COMBO_LIVING, 
			CGameTextMan::GetInstance().GetCommentText("CDT_SIGN", pREQUIRE->m_signLevel).GetString());
		SetWin_Combo_Sel (this, IDC_COMBO_BRIGHT, 
			CGameTextMan::GetInstance().GetCommentText("CDT_SIGN", pREQUIRE->m_signBright).GetString());
	}
	else
	{
		pQuest = GLQuestMan::GetInstance().Find ( pREQUIRE->m_sComQuestID.dwID );
		if ( pQuest )	strQuest.Format ( "[%d] %s", pREQUIRE->m_sComQuestID.dwID, pQuest->GetTITLE() );
		else			strQuest = ("None");
		
		SetWin_Text ( this, IDC_EDIT_COMQUEST, strQuest.GetString() );

		pQuest = GLQuestMan::GetInstance().Find ( pREQUIRE->m_sActQuestID.dwID );
		if ( pQuest )	strQuest.Format ( "[%d] %s", pREQUIRE->m_sActQuestID.dwID, pQuest->GetTITLE() );
		else			strQuest = ("None");

		SetWin_Text ( this, IDC_EDIT_PROCQUEST, strQuest.GetString() );

		SetWin_Combo_Sel ( this, IDC_COMBO_LIVING, COMMENT::CDT_SIGN[pREQUIRE->m_signLiving] );
		SetWin_Combo_Sel ( this, IDC_COMBO_BRIGHT, COMMENT::CDT_SIGN[pREQUIRE->m_signBright] );
	}
	SetWin_Num_int ( this, IDC_EDIT_LIVING, pREQUIRE->m_nLiving );
	SetWin_Num_int ( this, IDC_EDIT_BRIGHT, pREQUIRE->m_nBright );
}

void CRequirePage::InversePage ()
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;
	
	pREQUIRE->m_bForceExit = GetWin_Check ( this, IDC_CHECK_FORCE_EXIT );
	pREQUIRE->m_bPartyMbr = GetWin_Check ( this, IDC_CHECK_PARTY_AGREE ) != FALSE;

	pREQUIRE->m_wLevel = (WORD) GetWin_Num_int ( this, IDC_EDIT_LEVEL );
	pREQUIRE->m_signLevel = (EMCDT_SIGN) GetWin_Combo_Sel ( this, IDC_COMBO_LEVEL );

	if( pREQUIRE->m_signLevel == EMSIGN_FROMTO ) pREQUIRE->m_wLevel2 = (WORD) GetWin_Num_int ( this, IDC_EDIT_LEVEL2 );
	
	pREQUIRE->m_sItemID.wMainID = (WORD) GetWin_Num_int ( this, IDC_EDIT_ITEM_MID );
	pREQUIRE->m_sItemID.wSubID = (WORD) GetWin_Num_int ( this, IDC_EDIT_ITEM_SID );

	pREQUIRE->m_sItemIDMulti[0].wMainID = (WORD) GetWin_Num_int ( this, IDC_EDIT_ITEM_MID2 );
	pREQUIRE->m_sItemIDMulti[0].wSubID  = (WORD) GetWin_Num_int ( this, IDC_EDIT_ITEM_SID2 );
	pREQUIRE->m_sItemIDMulti[1].wMainID = (WORD) GetWin_Num_int ( this, IDC_EDIT_ITEM_MID3 );
	pREQUIRE->m_sItemIDMulti[1].wSubID  = (WORD) GetWin_Num_int ( this, IDC_EDIT_ITEM_SID3 );
	pREQUIRE->m_sItemIDMulti[2].wMainID = (WORD) GetWin_Num_int ( this, IDC_EDIT_ITEM_MID4 );
	pREQUIRE->m_sItemIDMulti[2].wSubID  = (WORD) GetWin_Num_int ( this, IDC_EDIT_ITEM_SID4 );
	pREQUIRE->m_sItemIDMulti[3].wMainID = (WORD) GetWin_Num_int ( this, IDC_EDIT_ITEM_MID5 );
	pREQUIRE->m_sItemIDMulti[3].wSubID  = (WORD) GetWin_Num_int ( this, IDC_EDIT_ITEM_SID5 );

	pREQUIRE->m_sSkillID.wMainID = (WORD) GetWin_Num_int ( this, IDC_EDIT_SKILL_MID );
	pREQUIRE->m_sSkillID.wSubID = (WORD) GetWin_Num_int ( this, IDC_EDIT_SKILL_SID );

	pREQUIRE->m_nLiving = GetWin_Num_int ( this, IDC_EDIT_LIVING );
	pREQUIRE->m_signLiving = (EMCDT_SIGN) GetWin_Combo_Sel ( this, IDC_COMBO_LIVING );

	pREQUIRE->m_nBright = GetWin_Num_int ( this, IDC_EDIT_BRIGHT );
	pREQUIRE->m_signBright = (EMCDT_SIGN) GetWin_Combo_Sel ( this, IDC_COMBO_BRIGHT );
}

BOOL CRequirePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strCDT_SIGN[EMSIGN_SIZE];
		for ( INT i = 0; i < EMSIGN_SIZE; ++i )
			strCDT_SIGN[i] = CGameTextMan::GetInstance().GetCommentText("CDT_SIGN", i);

		SetWin_Combo_Init (this, IDC_COMBO_LEVEL,	strCDT_SIGN, EMSIGN_SIZE);
		SetWin_Combo_Init (this, IDC_COMBO_LIVING,	strCDT_SIGN, EMSIGN_SIZE);
		SetWin_Combo_Init (this, IDC_COMBO_BRIGHT,	strCDT_SIGN, EMSIGN_SIZE);
	}
	else
	{
		SetWin_Combo_Init ( this, IDC_COMBO_LEVEL, COMMENT::CDT_SIGN, EMSIGN_SIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_LIVING, COMMENT::CDT_SIGN, EMSIGN_SIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_BRIGHT, COMMENT::CDT_SIGN, EMSIGN_SIZE );
	}

	m_bInit = TRUE;

	UpdatePage ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CRequirePage::OnBnClickedButtonSaveSet()
{
	InversePage ();

	m_pSheetTab->SetActivePage ( MAINPAGEINDEX );
}


void CRequirePage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	UpdatePage ();
}

void CRequirePage::OnBnClickedButtonItem()
{
	InversePage ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	m_pSheetTab->ActiveItemTreePage ( REQUIREINDEX, pREQUIRE->m_sItemID, IDC_BUTTON_ITEM );
}

void CRequirePage::OnBnClickedButtonSkill()
{
	InversePage ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	m_pSheetTab->ActiveSkillTreePage ( REQUIREINDEX, pREQUIRE->m_sSkillID );
}

void CRequirePage::OnBnClickedButtonResetItem()
{
	InversePage ();

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	pREQUIRE->m_sItemID = SNATIVEID(false);

	UpdatePage ();
}

void CRequirePage::OnBnClickedButtonResetSkill()
{
	InversePage ();

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	pREQUIRE->m_sSkillID = SNATIVEID(false);

	UpdatePage ();
}

void CRequirePage::OnBnClickedButtonResetQuestCom()
{
	InversePage ();

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	pREQUIRE->m_sComQuestID = SNATIVEID(false);

	UpdatePage ();
}

void CRequirePage::OnBnClickedButtonResetQuestProc()
{
	InversePage ();

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	pREQUIRE->m_sActQuestID = SNATIVEID(false);

	UpdatePage ();
}

void CRequirePage::OnBnClickedButtonComQuest()
{
	InversePage ();

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	CDlgQuest dlgQuest;
	if ( IDOK == dlgQuest.DoModal() )
	{
		pREQUIRE->m_sComQuestID = dlgQuest.m_dwQuestID;
	}

	UpdatePage ();
}

void CRequirePage::OnBnClickedButtonActQuest()
{
	InversePage ();

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	CDlgQuest dlgQuest;
	if ( IDOK == dlgQuest.DoModal() )
	{
		pREQUIRE->m_sActQuestID = dlgQuest.m_dwQuestID;
	}

	UpdatePage ();
}

void CRequirePage::OnCbnSelchangeComboLevel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( GetWin_Combo_Sel ( this, IDC_COMBO_LEVEL ) == EMSIGN_FROMTO )
	{
		SetWin_Enable( this, IDC_EDIT_LEVEL2, TRUE );
	}else{
		SetWin_Enable( this, IDC_EDIT_LEVEL2, FALSE );
	}
}

void CRequirePage::OnBnClickedCheckForceExit()
{
	InversePage();
	UpdatePage();
}

void CRequirePage::OnBnClickedButtonItem2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InversePage ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	m_pSheetTab->ActiveItemTreePage ( REQUIREINDEX, pREQUIRE->m_sItemIDMulti[0], IDC_BUTTON_ITEM2 );
}

void CRequirePage::OnBnClickedButtonItem3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InversePage ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	m_pSheetTab->ActiveItemTreePage ( REQUIREINDEX, pREQUIRE->m_sItemIDMulti[1], IDC_BUTTON_ITEM3 );
}

void CRequirePage::OnBnClickedButtonItem4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InversePage ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	m_pSheetTab->ActiveItemTreePage ( REQUIREINDEX, pREQUIRE->m_sItemIDMulti[2], IDC_BUTTON_ITEM4 );
}

void CRequirePage::OnBnClickedButtonItem5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InversePage ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	m_pSheetTab->ActiveItemTreePage ( REQUIREINDEX, pREQUIRE->m_sItemIDMulti[3], IDC_BUTTON_ITEM5 );
}

void CRequirePage::OnBnClickedButtonResetItem2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InversePage ();

	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	pREQUIRE->m_sItemIDMulti[0] = SNATIVEID(false);

	UpdatePage ();
}

void CRequirePage::OnBnClickedButtonResetItem3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InversePage ();

	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	pREQUIRE->m_sItemIDMulti[1] = SNATIVEID(false);

	UpdatePage ();
}

void CRequirePage::OnBnClickedButtonResetItem4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InversePage ();

	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	pREQUIRE->m_sItemIDMulti[2] = SNATIVEID(false);

	UpdatePage ();
}

void CRequirePage::OnBnClickedButtonResetItem5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InversePage ();

	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SLEVEL_REQUIRE* pREQUIRE = &pGLLandMan->m_sLevelRequire;

	pREQUIRE->m_sItemIDMulti[3] = SNATIVEID(false);

	UpdatePage ();
}
