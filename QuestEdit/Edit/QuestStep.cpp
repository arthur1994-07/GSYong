// QuestStep.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../QuestEdit.h"
#include "./QuestStep.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Land/GLMapList.h"
#include "../../RanLogic/Crow/GLCrowData.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/SingleobjectTextControl.h"

#include "../QuestEditDlg.h"

#include "../Dialogue/DlgMap.h"
#include "../Dialogue/DlgNPC.h"
#include "../Dialogue/DlgItem.h"
#include "../Dialogue/DlgCustomItem.h"
#include "../Dialogue/DlgSkill.h"
#include "./SheetWithTab.h"
#include "../Dialogue/DlgMobGenItem.h"
//#include "../enginelib/GUInterface/GameTextControl.h"
#include "./QuestStep.h"

// CQuestStepPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CQuestStepPage, CPropertyPage)
CQuestStepPage::CQuestStepPage( LOGFONT logfont )
	: CPropertyPage(CQuestStepPage::IDD),
	m_dwCurStep(UINT_MAX),
	m_bDlgInit(false),
	m_pSheetTab(NULL),
	m_pFont ( NULL )
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CQuestStepPage::~CQuestStepPage()
{
	SAFE_DELETE( m_pFont ) ;
}

//void CQuestStepPage::DoDataExchange(CDataExchange* pDX)
//{
//	CPropertyPage::DoDataExchange(pDX);
//}


BEGIN_MESSAGE_MAP(CQuestStepPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TALKNPC, OnBnClickedButtonTalknpc)
	ON_BN_CLICKED(IDC_BUTTON_DIEMOB, OnBnClickedButtonDiemob)
	ON_BN_CLICKED(IDC_BUTTON_GUARDNPC, OnBnClickedButtonGuardnpc)
	ON_BN_CLICKED(IDC_BUTTON_REACH_MAP, OnBnClickedButtonReachMap)
	ON_BN_CLICKED(IDC_BUTTON_GUARD_MAP, OnBnClickedButtonGuardMap)
	ON_LBN_SELCHANGE(IDC_LIST_STEP, OnLbnSelchangeListStep)
	ON_BN_CLICKED(IDC_BUTTON_REQ_QITEM_NEW, OnBnClickedButtonReqQitemNew)
	ON_BN_CLICKED(IDC_BUTTON_REQ_QITEM_EDIT, OnBnClickedButtonReqQitemEdit)
	ON_BN_CLICKED(IDC_BUTTON_REQ_QITEM_DEL, OnBnClickedButtonReqQitemDel)
	ON_BN_CLICKED(IDC_BUTTON_DEL_QITEM_NEW, OnBnClickedButtonDelQitemNew)
	ON_BN_CLICKED(IDC_BUTTON_DEL_QITEM_DEL, OnBnClickedButtonDelQitemDel)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_STEP_NEW, OnBnClickedButtonStepNew)
	ON_BN_CLICKED(IDC_BUTTON_REQ_STEP_DEL, OnBnClickedButtonReqStepDel)
	ON_BN_CLICKED(IDC_BUTTON_TALKNPC_DEL, OnBnClickedButtonTalknpcDel)
	ON_BN_CLICKED(IDC_BUTTON_DIEMOB_DEL, OnBnClickedButtonDiemobDel)
	ON_BN_CLICKED(IDC_BUTTON_GUARDNPC_DEL, OnBnClickedButtonGuardnpcDel)
	ON_BN_CLICKED(IDC_BUTTON_REACH_MAP_DEL, OnBnClickedButtonReachMapDel)
	ON_BN_CLICKED(IDC_BUTTON_GUARD_MAP_DEL, OnBnClickedButtonGuardMapDel)
	ON_BN_CLICKED(IDC_BUTTON_STEP_OK, OnBnClickedButtonStepOk)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_QITEM_NEW, OnBnClickedButtonGiftQitemNew)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_QITEM_EDIT, OnBnClickedButtonGiftQitemEdit)
	ON_BN_CLICKED(IDC_BUTTON_GIFT_QITEM_DEL, OnBnClickedButtonGiftQitemDel)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_LBN_DBLCLK(IDC_LIST_STEP, OnLbnDblclkListStep)
	ON_BN_CLICKED(IDC_BUTTON_EDITCANCEL, OnBnClickedButtonEditcancel)
	ON_BN_CLICKED(IDC_BUTTON_EDITCOMPLATE, OnBnClickedButtonEditcomplate)
	ON_BN_CLICKED(IDC_STEP_MAP_EDIT_BUTTON, OnBnClickedStepMapEditButton)
	ON_BN_CLICKED(IDC_BUTTON_ITEMUSE, &CQuestStepPage::OnBnClickedButtonItemuse)
	ON_BN_CLICKED(IDC_BUTTON_ITEMUSE_DEL, &CQuestStepPage::OnBnClickedButtonItemuseDel)
	ON_BN_CLICKED(IDC_BUTTON_ITEMGRINDING, &CQuestStepPage::OnBnClickedButtonItemgrinding)
	ON_BN_CLICKED(IDC_BUTTON_ITEMGRINDING_DEL, &CQuestStepPage::OnBnClickedButtonItemgrindingDel)
	ON_BN_CLICKED(IDC_RADIO_ITEMGRINDING1, &CQuestStepPage::OnBnClickedRadioItemgrinding1)
	ON_BN_CLICKED(IDC_RADIO_ITEMGRINDING2, &CQuestStepPage::OnBnClickedRadioItemgrinding2)
	ON_BN_CLICKED(IDC_RADIO_ITEMGRINDING3, &CQuestStepPage::OnBnClickedRadioItemgrinding3)
	ON_BN_CLICKED(IDC_RADIO_ITEMGRINDING4, &CQuestStepPage::OnBnClickedRadioItemgrinding4)
	ON_BN_CLICKED(IDC_CHECK_TOKILLPLAYER, &CQuestStepPage::OnBnClickedCheckTokillplayer)
	ON_BN_CLICKED(IDC_BUTTON_CONFTING_DEL, &CQuestStepPage::OnBnClickedButtonConftingDel)
	ON_BN_CLICKED(IDC_RADIO_CONFTING1, &CQuestStepPage::OnBnClickedRadioConfting1)
	ON_BN_CLICKED(IDC_RADIO_CONFTING2, &CQuestStepPage::OnBnClickedRadioConfting2)
	ON_BN_CLICKED(IDC_RADIO_CONFTING3, &CQuestStepPage::OnBnClickedRadioConfting3)
	ON_BN_CLICKED(IDC_RADIO_CONFTING4, &CQuestStepPage::OnBnClickedRadioConfting4)
	ON_BN_CLICKED(IDC_RADIO_CONFTING5, &CQuestStepPage::OnBnClickedRadioConfting5)
	ON_BN_CLICKED(IDC_BUTTON_LEVEL_UP_DEL, &CQuestStepPage::OnBnClickedButtonLevelUpDel)
END_MESSAGE_MAP()

// CQuestStepPage 메시지 처리기입니다.
BOOL CQuestStepPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_NONE );
	
	m_bDlgInit = true;

	UpdateItems();	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CQuestStepPage::UpdateItems ()
{
	if ( !m_bDlgInit )														return;

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );

	pListBox->ResetContent();

	GLQUEST::VECQSTEP_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.begin();
	GLQUEST::VECQSTEP_ITER iter_end = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.end();
	for ( int i=0; iter!=iter_end; ++iter, ++i )
	{
		const GLQUEST_STEP &sSTEP = (*iter);

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("QN_%03d_%03d_P_%d", CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.wMainID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.wSubID, i);
			int nIndex = pListBox->AddString (CSingleobjectTextControl::GetInstance().GetText(strKey.GetString()).GetString());
			pListBox->SetItemData ( nIndex, i );
		}
		else
		{
			int nIndex = pListBox->AddString ( sSTEP.GetTITLE () );
			pListBox->SetItemData ( nIndex, i );
		}
	}

	SetWin_Num_int( this, IDC_EDIT_STEP_ALLNUM, pListBox->GetCount() );
	SetWin_Num_int( this, IDC_EDIT_STEP_NUM, m_dwCurStep );

	UpdateItems_Step ();
}

void CQuestStepPage::VisibleItems_Step ( bool bVisible )
{
	if ( !m_bDlgInit )														return;

	DWORD dwITEMS[] =
	{
		IDC_EDIT_STEP_TITLE,

		IDC_STATIC_COMMENT,
		IDC_EDIT_STEP_COMMENT,
		
		IDC_EDIT_TALKNPC_COMMENT,
		IDC_BUTTON_TALKNPC,
		IDC_EDIT_TALKNPC_NAME,
		IDC_BUTTON_TALKNPC_DEL,
		
		IDC_EDIT_DIEMOB_COMMENT,
		IDC_BUTTON_DIEMOB,
		IDC_EDIT_DIEMOB_NAME,
		IDC_BUTTON_DIEMOB_DEL,
		IDC_EDIT_DIEMOB_NUM,
		IDC_CHECK_TOKILLPLAYER,

		IDC_EDIT_ITEMUSE_COMMENT,
		IDC_BUTTON_ITEMUSE,
		IDC_EDIT_ITEMUSE_NAME,
		IDC_EDIT_ITEMUSE_ID,
		IDC_BUTTON_ITEMUSE_DEL,
		IDC_EDIT_ITEMUSE_NUM,

		IDC_EDIT_ITEMGRINDING_COMMENT,
		IDC_EDIT_ITEMGRINDING_NAME,
		IDC_EDIT_ITEMGRINDING_NUM,
		IDC_EDIT_ITEMGRINDING_ID,
		IDC_RADIO_ITEMGRINDING1,
		IDC_RADIO_ITEMGRINDING2,
		IDC_RADIO_ITEMGRINDING3,
		IDC_RADIO_ITEMGRINDING4,
		IDC_CHECK_CONSECUTION,
		IDC_BUTTON_ITEMGRINDING,
		IDC_BUTTON_ITEMGRINDING_DEL,
		IDC_EDIT_LEVEL_UP_COMMENT,
		IDC_BUTTON_LEVEL_UP_DEL,
		IDC_EDIT_LEVEL_UP_NUM,


		IDC_STATIC_CONFTING,
		IDC_RADIO_CONFTING1,
		IDC_RADIO_CONFTING2,
		IDC_RADIO_CONFTING3,
		IDC_RADIO_CONFTING4,
		IDC_RADIO_CONFTING5,
		IDC_EDIT_CONFTING_COMMENT,
		IDC_EDIT_CONFTING_NUM,
		IDC_BUTTON_CONFTING_DEL,

		IDC_EDIT_GUARDNPC_COMMENT,
		IDC_BUTTON_GUARDNPC,
		IDC_EDIT_GUARDNPC_NAME,
		IDC_BUTTON_GUARDNPC_DEL,

		IDC_EDIT_TALKNPC_NUM,
		IDC_EDIT_TALKNPC_NUM2,
		IDC_EDIT_TALKNPC_NUM3,

		IDC_EDIT_REQ_QITEM_COMMENT,
		IDC_STATIC_REQ_QITEM,
		IDC_LIST_REQ_QITEM,
		IDC_BUTTON_REQ_QITEM_NEW,
		IDC_BUTTON_REQ_QITEM_EDIT,
		IDC_BUTTON_REQ_QITEM_DEL,
		
		IDC_EDIT_REACH_MAP_COMMENT,
		IDC_BUTTON_REACH_MAP,
		IDC_EDIT_REACH_MAP_NAME,
		IDC_BUTTON_REACH_MAP_DEL,
		IDC_STATIC_REACH,
		IDC_EDIT_REACH_PX,
		IDC_EDIT_REACH_PY,
		IDC_STATIC_REACH_R,
		IDC_EDIT_REACH_RADIUS,

		IDC_EDIT_GUARD_MAP_COMMENT,
		IDC_BUTTON_GUARD_MAP,
		IDC_EDIT_GUARD_MAP_NAME,
		IDC_BUTTON_GUARD_MAP_DEL,
		IDC_STATIC_DEFENSE,
		IDC_EDIT_GUARD_PX,
		IDC_EDIT_GUARD_PY,
		IDC_STATIC_DEFENSE_R,
		IDC_EDIT_GUARD_RADIUS,
		IDC_STATIC_DEFENSE_T,
		IDC_EDIT_GUARD_TIME,

		IDC_STATIC_LEVEL,
		IDC_EDIT_LEVEL,

		IDC_STATIC_GIFT_QITEM,
		IDC_LIST_GIFT_QITEM,
		IDC_BUTTON_GIFT_QITEM_NEW,
		IDC_BUTTON_GIFT_QITEM_EDIT,
		IDC_BUTTON_GIFT_QITEM_DEL,

		IDC_STATIC_RESET_QITEM,
		IDC_LIST_DEL_QITEM,
		IDC_BUTTON_DEL_QITEM_NEW,
		IDC_BUTTON_DEL_QITEM_DEL,

		IDC_STEP_MAP_MID_EDIT,
		IDC_STEP_MAP_SID_EDIT,
		IDC_STEP_MAP_XPOS_EDIT,
		IDC_STEP_MAP_YPOS_EDIT,
		IDC_STEP_MAP_GATE_EDIT,
		IDC_STEP_MAPID_STATIC,
		IDC_STEP_XY_STATIC,
		IDC_STEP_GATE_STATIC,
		IDC_STEP_MAP_EDIT_BUTTON,

		IDC_STATIC_CONDITION,
		IDC_STATIC_MOVEMAP,

		IDC_BUTTON_STEP_OK
	};

	DWORD dwINV_ITEMS[] =
	{
		IDC_BUTTON_PREV,
		IDC_BUTTON_NEXT,
		IDC_BUTTON_EDITCANCEL,
		IDC_BUTTON_EDITCOMPLATE,

	};

	int nSize = (int) sizeof(dwITEMS)/sizeof(DWORD);

	DWORD dwITEMS_DISABLE[] =
	{
		IDC_LIST_STEP,
		IDC_BUTTON_STEP_NEW,
	};

	int nSizeDisable = (int) sizeof(dwITEMS_DISABLE)/sizeof(DWORD);

	for ( int i=0; i<nSize; ++i )
	{
		CWnd *pWnd = GetDlgItem ( dwITEMS[i] );
		if ( pWnd )
		{
			pWnd->ShowWindow ( bVisible );
		}
	}

	nSize = (int) sizeof(dwINV_ITEMS)/sizeof(DWORD);
	for ( int i=0; i<nSize; ++i )
	{
		CWnd *pWnd = GetDlgItem ( dwINV_ITEMS[i] );
		if ( pWnd )
		{
			pWnd->ShowWindow ( !bVisible );
		}
	}

	for ( int i=0; i<nSizeDisable; ++i )
	{
		CWnd *pWnd = GetDlgItem ( dwITEMS_DISABLE[i] );
		if ( pWnd )
		{
			pWnd->EnableWindow ( !bVisible );
		}
	}

	SetReadMode();
}

void CQuestStepPage::UpdateItems_Step ()
{
	if ( !m_bDlgInit )														return;

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );
	int nIndex = pListBox->GetCurSel();
	if ( nIndex==LB_ERR )
	{
		VisibleItems_Step(false);
		return;
	}

	DWORD dwStep = (DWORD) pListBox->GetItemData ( nIndex );
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= dwStep )
	{
		VisibleItems_Step(false);
		return;
	}

	VisibleItems_Step(true);

	SetWin_Num_int( this, IDC_EDIT_STEP_NUM, dwStep+1 );

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[dwStep];

	m_dwCurStep = dwStep;

	PCROWDATA pCROW(NULL);
	SMAPNODE* pNODE(NULL);

	{
		CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_REQ_QITEM );
		pListBox->ResetContent();
		GENMOBITEMARRAY_ITER iter = sSTEP.m_vecMOBGEN_QITEM.begin();
		GENMOBITEMARRAY_ITER iter_end = sSTEP.m_vecMOBGEN_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			const SGENQUESTITEM &sCUSTION = (*iter);

			CString strNAME = "UNFOUND";
			SITEM* pITEM = GLogicData::GetInstance().GetItem ( sCUSTION.sNID );
			if ( pITEM )
			{
				strNAME.Format( "%d/%d %s", pITEM->sBasicOp.sNativeID.wMainID, pITEM->sBasicOp.sNativeID.wSubID,
								pITEM->GetName() );
			}

			int nIndex = pListBox->AddString ( strNAME.GetString() );
			DWORD dwData = DWORD(iter-sSTEP.m_vecMOBGEN_QITEM.begin());
			pListBox->SetItemData ( nIndex, dwData );
		}
	}

	{
		CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_DEL_QITEM );
		pListBox->ResetContent();
		DWQARRAY_ITER iter = sSTEP.m_vecRESET_QITEM.begin();
		DWQARRAY_ITER iter_end = sSTEP.m_vecRESET_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			const DWORD &dwDATA = (*iter);

			CString strNAME = "UNFOUND";
			SITEM* pITEM = GLogicData::GetInstance().GetItem ( SNATIVEID(dwDATA) );
			if ( pITEM )	strNAME = pITEM->GetName();

			int nIndex = pListBox->AddString ( strNAME.GetString() );
			DWORD dwData = DWORD(iter-sSTEP.m_vecRESET_QITEM.begin());
			pListBox->SetItemData ( nIndex, dwData );
		}
	}

	{
		CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_GIFT_QITEM );
		pListBox->ResetContent();
		INVENQARRAY_ITER iter = sSTEP.m_vecGIFT_QITEM.begin();
		INVENQARRAY_ITER iter_end = sSTEP.m_vecGIFT_QITEM.end();
		for ( ; iter!=iter_end; ++iter )
		{
			const SITEMCUSTOM &sDATA = (*iter);

			CString strNAME = "UNFOUND";
			SITEM* pITEM = GLogicData::GetInstance().GetItem ( sDATA.GetNativeID() );
			if ( pITEM )	strNAME = pITEM->GetName();

			int nIndex = pListBox->AddString ( strNAME.GetString() );

			DWORD dwData = DWORD(iter-sSTEP.m_vecGIFT_QITEM.begin());
			pListBox->SetItemData ( nIndex, dwData );
		}
	}

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		strKey.Format("QN_%03d_%03d_P_%d", CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.wMainID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.wSubID, m_dwCurStep);
		SetWin_Text (this, IDC_EDIT_STEP_TITLE,CSingleobjectTextControl::GetInstance().GetText(strKey.GetString()).GetString());
		strKey.Format("QD_%03d_%03d_D_%d", CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.wMainID, CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.wSubID, m_dwCurStep);
		SetWin_Text (this, IDC_EDIT_STEP_COMMENT,CSingleobjectTextControl::GetInstance().GetText(strKey.GetString()).GetString());
	}
	else
	{
		SetWin_Text ( this, IDC_EDIT_STEP_TITLE, sSTEP.GetTITLE() );
		SetWin_Text ( this, IDC_EDIT_STEP_COMMENT, sSTEP.GetCOMMENT() );
	}	

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		//	이수조건 1, 대화할 npc;
		SNATIVEID sNPCID = sSTEP.m_dwNID_NPCTALK;
		CString strKey;
		strKey.Format("CN_%03d_%03d", sNPCID.Mid(), sNPCID.Sid());
		CString strCrowName		= CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_CROW_TEXT).GetString();
		CString strCommentText	= CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_NPCTALK, CGameTextMan::EM_COMMENT_TEXT).GetString();
		CString strText;				
		strText.Format("\'%s\'%s", strCrowName, strCommentText);
		SetWin_Text ( this
			, IDC_EDIT_TALKNPC_COMMENT
			, strText.GetString() );

		//	이수조건 2, MOB 발생 퀘스트 아이템;
		SetWin_Text ( this
			, IDC_EDIT_REQ_QITEM_COMMENT
			, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBGEN_ITEM, CGameTextMan::EM_COMMENT_TEXT).GetString() );

		//	이수조건 3, 죽여야할 mob;
		SetWin_Text ( this
			, IDC_EDIT_DIEMOB_COMMENT
			, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBKILL, CGameTextMan::EM_COMMENT_TEXT).GetString() );

		//	이수조건 4, 보호할 npc;
		SetWin_Text ( this
			, IDC_EDIT_GUARDNPC_COMMENT
			, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_NPCQUARD, CGameTextMan::EM_COMMENT_TEXT).GetString() );

		//	이수조건 5, 도달해야할 map, 위치;
		SetWin_Text ( this
			, IDC_EDIT_REACH_MAP_COMMENT
			, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_REACH_ZONE, CGameTextMan::EM_COMMENT_TEXT).GetString() );

		//	이수조건 6, 수성해야할 map, 위치.
		SetWin_Text ( this
			, IDC_EDIT_GUARD_MAP_COMMENT
			, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_GUARD_MAP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString() );

		//	이수조건 8, 사용해야할 아이템;
		SetWin_Text ( this
			, IDC_EDIT_ITEMUSE_COMMENT
			, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_USE, CGameTextMan::EM_COMMENT_TEXT).GetString() );

		// 이수조건 9, 연마해야할 아이템;
		SetWin_Text ( this
			, IDC_EDIT_ITEMGRINDING_COMMENT
			, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_GRINDING, CGameTextMan::EM_COMMENT_TEXT).GetString() );

		// 이수조건 11, Level Up;
		SetWin_Text ( this
			, IDC_EDIT_LEVEL_UP_COMMENT
			, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_LEVEL_UP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString() );
	}
	else
	{
		//	이수조건 1, 대화할 npc;
		SetWin_Text ( this, IDC_EDIT_TALKNPC_COMMENT, sSTEP.GetOBJ_NPCTALK() );

		//	이수조건 2, MOB 발생 퀘스트 아이템;
		SetWin_Text ( this, IDC_EDIT_REQ_QITEM_COMMENT, sSTEP.GetOBJ_MOBGEN_QITEM() );

		//	이수조건 3, 죽여야할 mob;
		SetWin_Text ( this, IDC_EDIT_DIEMOB_COMMENT, sSTEP.GetOBJ_MOBKILL() );

		//	이수조건 4, 보호할 npc;
		SetWin_Text ( this, IDC_EDIT_GUARDNPC_COMMENT, sSTEP.GetOBJ_NPCGUARD() );

		//	이수조건 5, 도달해야할 map, 위치;
		SetWin_Text ( this, IDC_EDIT_REACH_MAP_COMMENT, sSTEP.GetOBJ_REACH_ZONE() );

		//	이수조건 6, 수성해야할 map, 위치.
		SetWin_Text ( this, IDC_EDIT_GUARD_MAP_COMMENT, sSTEP.GetOBJ_DEFENSE_ZONE() );

		//	이수조건 8, 사용해야할 아이템;
		SetWin_Text ( this, IDC_EDIT_ITEMUSE_COMMENT, sSTEP.GetOBJ_ITEMUSE() );

		// 이수조건 9, 연마해야할 아이템;
		SetWin_Text ( this, IDC_EDIT_ITEMGRINDING_COMMENT, sSTEP.GetOBJ_ITEMGRINDING() );

		// 이수조건 11, Level Up;
		SetWin_Text ( this, IDC_EDIT_LEVEL_UP_COMMENT, sSTEP.GetOBJ_LEVEL_UP() );
	}

	CString strCROWNAME;
	pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_NPCTALK) );
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	else			strCROWNAME = "";

	SetWin_Text ( this, IDC_EDIT_TALKNPC_NAME, strCROWNAME.GetString() );

	SNATIVEID sID = SNATIVEID( sSTEP.m_dwNID_NPCTALK );
	CString strNpcNum;
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM, strNpcNum );

	//	이수조건 3, 죽여야할 mob;
	pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_MOBKILL) );
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	else			strCROWNAME = "";
	SetWin_Text ( this, IDC_EDIT_DIEMOB_NAME, strCROWNAME );
	SetWin_Num_int ( this, IDC_EDIT_DIEMOB_NUM, sSTEP.m_dwNUM_MOBKILL );
	SetWin_Check ( this, IDC_CHECK_TOKILLPLAYER, sSTEP.m_bIsPLAYERKILL );

	sID = SNATIVEID( sSTEP.m_dwNID_MOBKILL );
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM2, strNpcNum );

	//	이수조건 4, 보호할 npc;
	pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_NPCGUARD) );
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	else			strCROWNAME = "";
	SetWin_Text ( this, IDC_EDIT_GUARDNPC_NAME, strCROWNAME );

	sID = SNATIVEID( sSTEP.m_dwNID_NPCGUARD );
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM3, strNpcNum );

	//	이수조건 5, 도달해야할 map, 위치.;
	CString strReachMap;
	pNODE = CQuestEditDlg::m_sMapList.FindMapNode ( sSTEP.m_sMAPID_REACH );
	if (pNODE)
        //strReachMap = pNODE->m_MapID.c_str();
		strReachMap = ID2GAMEINTEXT( pNODE->m_MapID.c_str());

	SetWin_Text ( this, IDC_EDIT_REACH_MAP_NAME, strReachMap );

	SetWin_Num_int ( this, IDC_EDIT_REACH_PX, sSTEP.m_wPOSX_REACH );
	SetWin_Num_int ( this, IDC_EDIT_REACH_PY, sSTEP.m_wPOSY_REACH );
	SetWin_Num_int ( this, IDC_EDIT_REACH_RADIUS, sSTEP.m_wRADIUS_REACH );

	//	이수조건 6, 수성해야할 map, 위치.
	CString strDefenseMap;
	pNODE = CQuestEditDlg::m_sMapList.FindMapNode ( sSTEP.m_sMAPID_DEFENSE );
	if (pNODE)
        strDefenseMap = pNODE->m_MapID.c_str();

	SetWin_Text ( this, IDC_EDIT_GUARD_MAP_NAME, strDefenseMap );
	
	SetWin_Num_int ( this, IDC_EDIT_GUARD_PX, sSTEP.m_wPOSX_DEFENSE );
	SetWin_Num_int ( this, IDC_EDIT_GUARD_PY, sSTEP.m_wPOSY_DEFENSE );
	SetWin_Num_int ( this, IDC_EDIT_GUARD_RADIUS, sSTEP.m_wRADIUS_DEFENSE );
	SetWin_Num_float ( this, IDC_EDIT_GUARD_TIME, sSTEP.m_fDEFENSE_TIME );

	//	이수조건 7, 도달해야할 레벨.
	SetWin_Num_int ( this, IDC_EDIT_LEVEL, sSTEP.m_wLevel );

	//	이수조건 8, 사용해야할 아이템
	//sSTEP.m_dwNUM_ITEMUSE = (DWORD) GetWin_Num_int ( this, IDC_EDIT_ITEMUSE_NUM );

	SITEM* pITEM = GLogicData::GetInstance().GetItem ( SNATIVEID(sSTEP.m_dwNID_ITEMUSE) );
	if ( pITEM )
		strCROWNAME = pITEM->GetName();
	else
		strCROWNAME = "";

	SetWin_Text ( this, IDC_EDIT_ITEMUSE_NAME, strCROWNAME );
	SetWin_Num_int ( this, IDC_EDIT_ITEMUSE_NUM, sSTEP.m_dwNUM_ITEMUSE );

	sID = SNATIVEID( sSTEP.m_dwNID_ITEMUSE );
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_ITEMUSE_ID, strNpcNum );

	// 이수조건 9, 연마해야할 아이템
	//sSTEP.m_dwNUM_ITEMGRINDING = (DWORD) GetWin_Num_int ( this, IDC_EDIT_ITEMGRINDING_NUM );
	//SetWin_Num_int ( this, IDC_EDIT_DIEMOB_NUM, sSTEP.m_dwNUM_MOBKILL );

	pITEM = GLogicData::GetInstance().GetItem ( SNATIVEID(sSTEP.m_dwNID_ITEMGRINDING) );
	if ( pITEM )
		strCROWNAME = pITEM->GetName();
	else
		strCROWNAME = "";

	SetWin_Text ( this, IDC_EDIT_ITEMGRINDING_NAME, strCROWNAME );
	SetWin_Num_int ( this, IDC_EDIT_ITEMGRINDING_NUM, sSTEP.m_dwNUM_ITEMGRINDING );

	sID = SNATIVEID( sSTEP.m_dwNID_ITEMGRINDING );
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_ITEMGRINDING_ID, strNpcNum );

	SetWin_Check( this, IDC_RADIO_ITEMGRINDING1, false );
	SetWin_Check( this, IDC_RADIO_ITEMGRINDING2, false );
	SetWin_Check( this, IDC_RADIO_ITEMGRINDING3, false );
	SetWin_Check( this, IDC_RADIO_ITEMGRINDING4, false );

	bool bCheckConsecution = sSTEP.m_bConsecution;
	SetWin_Check( this, IDC_CHECK_CONSECUTION, bCheckConsecution );


	DWORD dwItemType = sSTEP.m_dwItemType;
	switch ( dwItemType ) 
	{
	case EMGRINDING_NONE :
		{
			SetWin_Check( this, IDC_RADIO_ITEMGRINDING1, true );
		}
		break;
	case EMGRINDING_WEAPON :
		{
			SetWin_Check( this, IDC_RADIO_ITEMGRINDING2, true );
		}
		break;
	case EMGRINDING_ARMOR :
		{
			SetWin_Check( this, IDC_RADIO_ITEMGRINDING3, true );
		}
		break;
	case EMGRINDING_ALL :
		{
			SetWin_Check( this, IDC_RADIO_ITEMGRINDING4, true );
		}
		break;
	}

	// 이수조건 10, 대련

	SetWin_Check( this, IDC_RADIO_CONFTING1, false );
	SetWin_Check( this, IDC_RADIO_CONFTING2, false );
	SetWin_Check( this, IDC_RADIO_CONFTING3, false );
	SetWin_Check( this, IDC_RADIO_CONFTING4, false );
	SetWin_Check( this, IDC_RADIO_CONFTING5, false );

	SetWin_Text ( this, IDC_EDIT_CONFTING_COMMENT, sSTEP.GetOBJ_CONFTING() );
	SetWin_Num_int ( this, IDC_EDIT_CONFTING_NUM, sSTEP.m_dwNUM_CONFTING);

	DWORD dwType_CONFTING = sSTEP.m_dwType_CONFTING;

	if ( EMCONFT_NONE == dwType_CONFTING )
	{
		SetWin_Check( this, IDC_RADIO_CONFTING1, true );
		SetWin_Enable( this, IDC_EDIT_CONFTING_COMMENT, false );
		SetWin_Enable( this, IDC_EDIT_CONFTING_NUM, false );
		SetWin_Enable( this, IDC_BUTTON_CONFTING_DEL, false );
	}
	else if ( EMCONFT_ONE == dwType_CONFTING)
	{
		SetWin_Check( this, IDC_RADIO_CONFTING2, true );
	}
	else if ( EMCONFT_PARTY == dwType_CONFTING && !sSTEP.m_bSCHOOL)
	{
		SetWin_Check( this, IDC_RADIO_CONFTING3, true );
	}
	else if ( EMCONFT_GUILD == dwType_CONFTING)
	{
		SetWin_Check( this, IDC_RADIO_CONFTING4, true );
	}
	else if ( EMCONFT_PARTY == dwType_CONFTING && sSTEP.m_bSCHOOL)
	{
		SetWin_Check( this, IDC_RADIO_CONFTING5, true );
	}

	// 이수조건 11, Level Up
	SetWin_Num_int ( this, IDC_EDIT_LEVEL_UP_NUM, sSTEP.m_dwNUM_LEVELUP);

	//  완료후 이동할 맵
	SetWin_Num_int ( this, IDC_STEP_MAP_MID_EDIT, sSTEP.m_stepMoveMap.nidMAP.wMainID );
	SetWin_Num_int ( this, IDC_STEP_MAP_SID_EDIT, sSTEP.m_stepMoveMap.nidMAP.wSubID );
	SetWin_Num_int ( this, IDC_STEP_MAP_XPOS_EDIT, sSTEP.m_stepMoveMap.wPosX );
	SetWin_Num_int ( this, IDC_STEP_MAP_YPOS_EDIT, sSTEP.m_stepMoveMap.wPosY );
	SetWin_Num_int ( this, IDC_STEP_MAP_GATE_EDIT, sSTEP.m_stepMoveMap.dwGateID );
}

void CQuestStepPage::InverseUpdateItems ()
{
	if ( !m_bDlgInit )														return;

	InverseUpdateItems_Step ();
}

void CQuestStepPage::InverseUpdateItems_Step ()
{
	if ( !m_bDlgInit )														return;

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_strTITLE = GetWin_Text ( this, IDC_EDIT_STEP_TITLE ).GetString();
	sSTEP.m_strCOMMENT = GetWin_Text ( this, IDC_EDIT_STEP_COMMENT ).GetString();

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
	}
	else
	{
		//  이수조건 설명;
		//	이수조건 1, 대화할 npc;
		sSTEP.m_strOBJ_NPCTALK = GetWin_Text ( this, IDC_EDIT_TALKNPC_COMMENT );
		
		//	이수조건 2, MOB 발생 퀘스트 아이템;
		sSTEP.m_strOBJ_MOBGEN_QITEM = GetWin_Text ( this, IDC_EDIT_REQ_QITEM_COMMENT );

		//	이수조건 3, 죽여야할 mob;
		sSTEP.m_strOBJ_MOBKILL =GetWin_Text ( this, IDC_EDIT_DIEMOB_COMMENT );

		//	이수조건 4, 보호할 npc;
		sSTEP.m_strOBJ_NPCGUARD = GetWin_Text ( this, IDC_EDIT_GUARDNPC_COMMENT );

		//	이수조건 5, 도달해야할 map, 위치;
		sSTEP.m_strOBJ_REACH_ZONE = GetWin_Text ( this, IDC_EDIT_REACH_MAP_COMMENT );

		//	이수조건 6, 수성해야할 map, 위치;
		sSTEP.m_strOBJ_DEFENSE_ZONE = GetWin_Text ( this, IDC_EDIT_GUARD_MAP_COMMENT );

		//	이수조건 8, 사용해야할 아이템;
		sSTEP.m_strOBJ_ITEMUSE = GetWin_Text ( this, IDC_EDIT_ITEMUSE_COMMENT );

		//	이수조건 9, 연마해야할 아이템
		sSTEP.m_strOBJ_ITEMGRINDING = GetWin_Text ( this, IDC_EDIT_ITEMGRINDING_COMMENT );

		// 이수 조건 11, Level UP;
		sSTEP.m_strOBJ_LEVELUP = GetWin_Text ( this, IDC_EDIT_LEVEL_UP_COMMENT );
	}
	//	이수조건 3, 죽여야할 mob
	sSTEP.m_dwNUM_MOBKILL = (DWORD) GetWin_Num_int ( this, IDC_EDIT_DIEMOB_NUM );
	sSTEP.m_bIsPLAYERKILL = GetWin_Check(this, IDC_CHECK_TOKILLPLAYER);

	sSTEP.m_wPOSX_REACH = (WORD) GetWin_Num_int ( this, IDC_EDIT_REACH_PX );
	sSTEP.m_wPOSY_REACH = (WORD) GetWin_Num_int ( this, IDC_EDIT_REACH_PY );
	sSTEP.m_wRADIUS_REACH = (WORD) GetWin_Num_int ( this, IDC_EDIT_REACH_RADIUS );

	//	이수조건 6, 수성해야할 map, 위치.
	sSTEP.m_wPOSX_DEFENSE = (WORD) GetWin_Num_int ( this, IDC_EDIT_GUARD_PX );
	sSTEP.m_wPOSY_DEFENSE = (WORD) GetWin_Num_int ( this, IDC_EDIT_GUARD_PY );
	sSTEP.m_wRADIUS_DEFENSE = (WORD) GetWin_Num_int ( this, IDC_EDIT_GUARD_RADIUS );
	sSTEP.m_fDEFENSE_TIME = GetWin_Num_float ( this, IDC_EDIT_GUARD_TIME );

	//	이수조건 7, 도달해야할 레벨.
	sSTEP.m_wLevel = (WORD) GetWin_Num_int ( this, IDC_EDIT_LEVEL );

	//	이수조건 8, 사용해야할 아이템
	sSTEP.m_dwNUM_ITEMUSE = (DWORD) GetWin_Num_int ( this, IDC_EDIT_ITEMUSE_NUM );

	//	이수조건 9, 연마해야할 아이템
	sSTEP.m_dwNUM_ITEMGRINDING = (DWORD) GetWin_Num_int ( this, IDC_EDIT_ITEMGRINDING_NUM );
	sSTEP.m_bConsecution = GetWin_Check ( this, IDC_CHECK_CONSECUTION );

	if ( GetWin_Check ( this, IDC_RADIO_ITEMGRINDING1 ) )
		sSTEP.m_dwItemType = EMGRINDING_NONE;

	else if ( GetWin_Check ( this, IDC_RADIO_ITEMGRINDING2 ) )
		sSTEP.m_dwItemType = EMGRINDING_WEAPON;

	else if ( GetWin_Check ( this, IDC_RADIO_ITEMGRINDING3 ) )
		sSTEP.m_dwItemType = EMGRINDING_ARMOR;

	else if ( GetWin_Check ( this, IDC_RADIO_ITEMGRINDING4 ) )
		sSTEP.m_dwItemType = EMGRINDING_ALL;

	//	이수조건 10, 대련
	sSTEP.m_strOBJ_CONFTING = GetWin_Text ( this, IDC_EDIT_CONFTING_COMMENT );
	sSTEP.m_dwNUM_CONFTING = (DWORD) GetWin_Num_int ( this, IDC_EDIT_ITEMGRINDING_NUM );
	sSTEP.m_bSCHOOL = false;

	if ( GetWin_Check ( this, IDC_RADIO_CONFTING1 ) )
	{
		sSTEP.m_dwType_CONFTING = EMCONFT_NONE;
	}
	else if ( GetWin_Check ( this, IDC_RADIO_CONFTING2 ) )
		sSTEP.m_dwType_CONFTING = EMCONFT_ONE;

	else if ( GetWin_Check ( this, IDC_RADIO_CONFTING3) )
		sSTEP.m_dwType_CONFTING = EMCONFT_PARTY;

	else if ( GetWin_Check ( this, IDC_RADIO_CONFTING4 ) )
		sSTEP.m_dwType_CONFTING = EMCONFT_GUILD;

	else if ( GetWin_Check ( this, IDC_RADIO_CONFTING5 ) )
	{
		sSTEP.m_dwType_CONFTING = EMCONFT_PARTY;
		sSTEP.m_bSCHOOL = true;
	}

	sSTEP.m_dwNUM_CONFTING = (DWORD) GetWin_Num_int ( this, IDC_EDIT_CONFTING_NUM );

	// 이수 조건 11, Level UP
	sSTEP.m_dwNUM_LEVELUP = (DWORD) GetWin_Num_int ( this, IDC_EDIT_LEVEL_UP_NUM );

}

void CQuestStepPage::OnBnClickedButtonStepNew()
{
	//	Note : 이전 정보 백업.
	//
	InverseUpdateItems_Step ();
	m_dwCurStep = UINT_MAX;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		const SNATIVEID sNID = CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID;
		const int nStepSize = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size();

		CString strProg, strDesc;
		strProg.Format("QN_%03d_%03d_P_%d", sNID.Mid(), sNID.Sid(), nStepSize);
		strDesc.Format("QD_%03d_%03d_D_%d", sNID.Mid(), sNID.Sid(), nStepSize);

		CSingleobjectTextControl::GetInstance().InsertString(strProg.GetString(), strProg.GetString());
		CSingleobjectTextControl::GetInstance().InsertString(strDesc.GetString(), strDesc.GetString());
	}
	
	//	Note : 새로운 Step 넣기.
	CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.push_back ( GLQUEST_STEP() );

	//	Note : 리스트 갱신.
	UpdateItems ();

	//	Note : 새로 추가된 Step 선택.
	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );

	DWORD dwCurSel = (DWORD) CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size()-1;
	pListBox->SetCurSel(dwCurSel);

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonReqStepDel()
{
	InverseUpdateItems ();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );
	int nIndex = pListBox->GetCurSel();
	if ( nIndex==LB_ERR )							return;

	DWORD dwStep = (DWORD) pListBox->GetItemData ( nIndex );
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= dwStep )	return;

	//	Note : 이전 정보 백업.
	//
	InverseUpdateItems_Step ();
	m_dwCurStep = UINT_MAX;

	pListBox->SetCurSel(LB_ERR);

	int nReturn = MessageBox ( "Do you want to delete Quest Step of present?", "Caution", MB_YESNO );
	if ( nReturn==IDYES )
	{
		GLQUEST::VECQSTEP_ITER iter = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.begin()+dwStep;
		CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.erase ( iter, iter+1 );

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			const SNATIVEID sNID = CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID;
			const int nStepSize = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size();

			CString strProg, strDesc;
			strProg.Format("QN_%03d_%03d_P_%d", sNID.Mid(), sNID.Sid(), nIndex);
			strDesc.Format("QD_%03d_%03d_D_%d", sNID.Mid(), sNID.Sid(), nIndex);

			CSingleobjectTextControl::GetInstance().DeleteString(strProg.GetString());
			CSingleobjectTextControl::GetInstance().DeleteString(strDesc.GetString());
		}
	}

	UpdateItems ();
}

void CQuestStepPage::OnLbnSelchangeListStep()
{
}

void CQuestStepPage::OnLbnDblclkListStep()
{
	m_dwCurStep = UINT_MAX;

	//	Note : 이전 정보 백업.
	//
	InverseUpdateItems_Step ();

	//	Note : 현제 선택 정보 표시.
	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonTalknpc()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgNPC sDlgNpc;
	sDlgNpc.m_bMob = false;
	if ( IDOK==sDlgNpc.DoModal() )
	{
		sSTEP.m_dwNID_NPCTALK = sDlgNpc.m_nidNPC.dwID;
	}

	PCROWDATA pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_NPCTALK) );

	CString strCROWNAME = "";
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	SetWin_Text ( this, IDC_EDIT_TALKNPC_NAME, strCROWNAME );

	SNATIVEID sID = SNATIVEID( sSTEP.m_dwNID_NPCTALK );
	CString strNpcNum;
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM, strNpcNum );
}

void CQuestStepPage::OnBnClickedButtonDiemob()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgNPC sDlgNpc;
	sDlgNpc.m_bMob = true;
	if ( IDOK==sDlgNpc.DoModal() )
	{
		sSTEP.m_dwNID_MOBKILL = sDlgNpc.m_nidNPC.dwID;
	}

	PCROWDATA pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_MOBKILL) );

	CString strCROWNAME = "";
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	SetWin_Text ( this, IDC_EDIT_DIEMOB_NAME, strCROWNAME );

	SNATIVEID sID = SNATIVEID( sSTEP.m_dwNID_MOBKILL );
	CString strNpcNum;
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM2, strNpcNum );
}

void CQuestStepPage::OnBnClickedButtonGuardnpc()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];


	CDlgNPC sDlgNpc;
	sDlgNpc.m_bMob = false;
	if ( IDOK==sDlgNpc.DoModal() )
	{
		sSTEP.m_dwNID_NPCGUARD = sDlgNpc.m_nidNPC.dwID;
	}

	PCROWDATA pCROW = GLCrowDataMan::GetInstance().GetCrowData ( SNATIVEID(sSTEP.m_dwNID_NPCGUARD) );
	
	CString strCROWNAME = "";
	if ( pCROW )	strCROWNAME = pCROW->GetName();
	SetWin_Text ( this, IDC_EDIT_GUARDNPC_NAME, strCROWNAME );

	SNATIVEID sID = SNATIVEID( sSTEP.m_dwNID_NPCGUARD );
	CString strNpcNum;
	if( sID != NATIVEID_NULL() )
		strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
	else
		strNpcNum = "";
	SetWin_Text( this, IDC_EDIT_TALKNPC_NUM3, strNpcNum );
}

void CQuestStepPage::OnBnClickedButtonReachMap()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )
        return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
        return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgMap sDlgMap;
	if ( IDOK==sDlgMap.DoModal() )
	{
		sSTEP.m_sMAPID_REACH = sDlgMap.m_nidMAP;
	}

	CString strReachMap;
	SMAPNODE* pNODE = CQuestEditDlg::m_sMapList.FindMapNode ( sSTEP.m_sMAPID_REACH );
	if (pNODE)
        //strReachMap = pNODE->m_MapID.c_str();
		strReachMap = ID2GAMEINTEXT( pNODE->m_MapID.c_str());

	SetWin_Text ( this, IDC_EDIT_REACH_MAP_NAME, strReachMap );
}

void CQuestStepPage::OnBnClickedButtonGuardMap()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )
        return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
        return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgMap sDlgMap;
	if ( IDOK==sDlgMap.DoModal() )
	{
		sSTEP.m_sMAPID_DEFENSE = sDlgMap.m_nidMAP;
	}

	CString strDefenseMap;
	SMAPNODE* pNODE = CQuestEditDlg::m_sMapList.FindMapNode ( sSTEP.m_sMAPID_DEFENSE );
	if (pNODE)
	{
        //strDefenseMap = pNODE->m_MapID.c_str();
		strDefenseMap = ID2GAMEINTEXT( pNODE->m_MapID.c_str());
	}

	SetWin_Text ( this, IDC_EDIT_GUARD_MAP_NAME, strDefenseMap );
}

void CQuestStepPage::OnBnClickedButtonReqQitemNew()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )
        return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
        return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgItem sDlgItem;
	if ( IDOK == sDlgItem.DoModal () )
	{
		SGENQUESTITEM sGENITEM;
		sGENITEM.sNID = sDlgItem.m_nidITEM;

		SITEM* pITEM = GLogicData::GetInstance().GetItem ( sGENITEM.sNID );
		if ( !pITEM )
            return;

		if ( pITEM->ISPILE() )	sGENITEM.wNUM = pITEM->GETAPPLYNUM();

		sSTEP.MOBGENITEM_PUSH(sGENITEM);

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonReqQitemEdit()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )
        return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
        return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_REQ_QITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR==nIndex )
        return;

	DWORD dwData = (DWORD) pListBox->GetItemData ( nIndex );

	SGENQUESTITEM &sGENITEM = sSTEP.m_vecMOBGEN_QITEM[dwData];

	CDlgMobGenItem sDlgGenItem;
	sDlgGenItem.m_sGENQITEM = sGENITEM;

	if ( IDOK==sDlgGenItem.DoModal() )
	{
		sGENITEM = sDlgGenItem.m_sGENQITEM;
	}
}

void CQuestStepPage::OnBnClickedButtonReqQitemDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )
        return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
        return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_REQ_QITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR==nIndex )
        return;

	DWORD dwData = (DWORD) pListBox->GetItemData ( nIndex );

	SGENQUESTITEM &sGENITEM = sSTEP.m_vecMOBGEN_QITEM[dwData];

	CString strNAME = "UNFOUND";
	SITEM* pITEM = GLogicData::GetInstance().GetItem ( sGENITEM.sNID );
	if ( pITEM )
        strNAME = pITEM->GetName();

	if ( IDYES==MessageBox ( "Do you want to delete 'Item'?", strNAME, MB_YESNO ) )
	{
		sSTEP.MOBGENITEM_ERASE ( dwData );

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonDelQitemNew()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )
        return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
        return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgItem sDlgItem;
	if ( IDOK == sDlgItem.DoModal () )
	{
		sSTEP.m_vecRESET_QITEM.push_back ( sDlgItem.m_nidITEM.dwID );

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonDelQitemDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )
        return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
        return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_DEL_QITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR==nIndex )	return;

	DWORD dwData = (DWORD) pListBox->GetItemData ( nIndex );

	SNATIVEID sRESET_NID(sSTEP.m_vecRESET_QITEM[dwData]);

	CString strNAME = "UNFOUND";
	SITEM* pITEM = GLogicData::GetInstance().GetItem ( sRESET_NID );
	if ( pITEM )	strNAME = pITEM->GetName();

	if ( IDYES==MessageBox ( "Do you want to delete 'Item'?", strNAME, MB_YESNO ) )
	{
		sSTEP.RESETITEM_ERASE ( dwData );

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonPrev()
{
	InverseUpdateItems();
	GLQuestMan::GetInstance().DoModified ();

	//	이전 페이지로 이동.
	m_pSheetTab->ActiveQuestPage( QUESTSTEP_PAGE, NULL );
}


void CQuestStepPage::OnBnClickedButtonTalknpcDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_dwNID_NPCTALK = UINT_MAX;

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonDiemobDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_dwNID_MOBKILL = UINT_MAX;

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonGuardnpcDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_dwNID_NPCGUARD = UINT_MAX;

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonReachMapDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_sMAPID_REACH = SNATIVEID(false);

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonGuardMapDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_sMAPID_DEFENSE = SNATIVEID(false);

	UpdateItems_Step ();
}


void CQuestStepPage::OnBnClickedButtonStepOk()
{
	CListBox *pListStepBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );
	int nIndex = pListStepBox->GetCurSel();
	if ( nIndex==LB_ERR )							return;

	DWORD dwStep = (DWORD) pListStepBox->GetItemData ( nIndex );
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= dwStep )	return;

	// 완료할 맵 정보가 제대로 된 것 인지 체크
	GLQUEST_PROGRESS_MAP checkMap;
	checkMap.nidMAP.wMainID = GetWin_Num_int ( this, IDC_STEP_MAP_MID_EDIT );
	checkMap.nidMAP.wSubID   = GetWin_Num_int ( this, IDC_STEP_MAP_SID_EDIT );
	checkMap.wPosX		   = GetWin_Num_int ( this, IDC_STEP_MAP_XPOS_EDIT );
	checkMap.wPosY		   = GetWin_Num_int ( this, IDC_STEP_MAP_YPOS_EDIT );
	checkMap.dwGateID	   = GetWin_Num_int ( this, IDC_STEP_MAP_GATE_EDIT );


	if( checkMap.nidMAP.wMainID != USHRT_MAX &&	checkMap.nidMAP.wSubID  != USHRT_MAX )
	{
		if( (checkMap.wPosX == USHRT_MAX || checkMap.wPosY == USHRT_MAX ) && checkMap.dwGateID == 0 )
		{
			MessageBox( "only one of Pos and Gate settings", "Error" );
			return;
		}

		if( checkMap.wPosX != USHRT_MAX && checkMap.wPosY != USHRT_MAX && checkMap.dwGateID != 0 )
		{
			MessageBox( "only one of Pos and Gate settings", "Error" );
			return;
		}

		if( checkMap.nidMAP.wMainID  != UINT_MAX &&
			checkMap.nidMAP.wSubID   != UINT_MAX &&
			(checkMap.wPosX != UINT_MAX &&
			checkMap.wPosY != UINT_MAX ||
			checkMap.dwGateID != 0 ) )
			CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep].m_stepMoveMap = checkMap;
	}

	// Stringtable update;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		const SNATIVEID sNID = CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID;
		const int nStepSize = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size();

		CString strProg, strDesc;
		strProg.Format("QN_%03d_%03d_P_%d", sNID.Mid(), sNID.Sid(), nIndex);
		strDesc.Format("QD_%03d_%03d_D_%d", sNID.Mid(), sNID.Sid(), nIndex);

		CString strStepTitle = GetWin_Text(this, IDC_EDIT_STEP_TITLE);
		CString strStepComment = GetWin_Text(this, IDC_EDIT_STEP_COMMENT);
		CSingleobjectTextControl::GetInstance().InsertString(strProg.GetString(), strStepTitle.GetString());
		CSingleobjectTextControl::GetInstance().InsertString(strDesc.GetString(), strStepComment.GetString());
	}


	//	Note : 이전 정보 백업.
	//
	InverseUpdateItems_Step ();
	m_dwCurStep = UINT_MAX;
	
	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_REQ_QITEM );
	pListBox->SetCurSel(LB_ERR);

	//	Note : 리스트 갱신.
	UpdateItems ();
}

void CQuestStepPage::OnBnClickedButtonGiftQitemNew()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )							return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgItem sDlgItem;
	if ( IDOK == sDlgItem.DoModal () )
	{ 
		SITEMCUSTOM sCUSTOM(sDlgItem.m_nidITEM);

		sSTEP.m_vecGIFT_QITEM.push_back ( sCUSTOM );

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonGiftQitemEdit()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )											return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_GIFT_QITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR==nIndex )	return;

	DWORD dwData = (DWORD) pListBox->GetItemData ( nIndex );

	SITEMCUSTOM &sCUSTOM = sSTEP.m_vecGIFT_QITEM[dwData];

	CDlgCustomItem sDlgCustom;
	sDlgCustom.m_sITEMCUSTOM = sCUSTOM;

	if ( IDOK==sDlgCustom.DoModal() )
	{
		sCUSTOM = sDlgCustom.m_sITEMCUSTOM;
	}
}

void CQuestStepPage::OnBnClickedButtonGiftQitemDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )							return;
	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CListBox *pListBox = (CListBox *) GetDlgItem ( IDC_LIST_GIFT_QITEM );
	int nIndex = pListBox->GetCurSel();
	if ( LB_ERR==nIndex )	return;

	DWORD dwData = (DWORD) pListBox->GetItemData ( nIndex );

	const SITEMCUSTOM sCUSTOM = sSTEP.m_vecGIFT_QITEM[dwData];

	CString strNAME = "UNFOUND";
	SITEM* pITEM = GLogicData::GetInstance().GetItem ( sCUSTOM.GetNativeID() );
	if ( pITEM )	strNAME = pITEM->GetName();

	if ( IDYES==MessageBox ( "Do you want to delete 'Item'?", strNAME, MB_YESNO ) )
	{
		sSTEP.GIFTITEM_ERASE ( dwData );

		UpdateItems_Step ();
	}
}

void CQuestStepPage::OnBnClickedButtonUp()
{
	InverseUpdateItems ();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );
	int nIndex = pListBox->GetCurSel();
	if ( nIndex==LB_ERR )							return;

	DWORD dwID = (DWORD) pListBox->GetItemData ( nIndex );

	dwID = CQuestEditDlg::m_sQuestNode.pQUEST->ToUpStep(dwID);

	pListBox->SetCurSel ( dwID );

	//	Note : 리스트 갱신.
	UpdateItems ();
	UpdateItems_Step ();

	pListBox->SetCurSel ( dwID );
}

void CQuestStepPage::OnBnClickedButtonDown()
{
	InverseUpdateItems ();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_STEP );
	int nIndex = pListBox->GetCurSel();
	if ( nIndex==LB_ERR )							return;

	DWORD dwID = (DWORD) pListBox->GetItemData ( nIndex );

	dwID = CQuestEditDlg::m_sQuestNode.pQUEST->ToDownStep(dwID);

	pListBox->SetCurSel ( dwID );

	//	Note : 리스트 갱신.
	UpdateItems ();
	UpdateItems_Step ();

	pListBox->SetCurSel ( dwID );
}



void CQuestStepPage::OnBnClickedButtonEditcancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveQuestTreePage ();
}

void CQuestStepPage::OnBnClickedButtonEditcomplate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateItems();

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

	// Stringtable update;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
// 		const SNATIVEID sNID = CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID;
 		const int nStepSize = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size()
			, sMID = CQuestEditDlg::m_sQuestNode.pQUEST->m_sNID.Mid();
 
		for ( int nIndex = 0; nIndex < nStepSize; ++nIndex )
		{
			CString strProg, strDesc;
			strProg.Format("QN_%03d_000_P_%d", sMID, nIndex);
			strDesc.Format("QD_%03d_000_D_%d", sMID, nIndex);
			CGameTextMan::GetInstance().InsertString(strProg.GetString()
				, CSingleobjectTextControl::GetInstance().GetText(strProg.GetString()).GetString()
				, CGameTextMan::EM_QUEST_TEXT);
			CGameTextMan::GetInstance().InsertString(strDesc.GetString()
				, CSingleobjectTextControl::GetInstance().GetText(strDesc.GetString()).GetString()
				, CGameTextMan::EM_QUEST_TEXT);
		}
	}

	m_pSheetTab->ActiveQuestTreePage ();
}

void CQuestStepPage::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	const int nSkipNum = 14;
	const int nSkipID[nSkipNum] = { IDC_LIST_STEP, IDC_EDIT_STEP_TITLE, IDC_EDIT_STEP_COMMENT,
									IDC_EDIT_TALKNPC_COMMENT, IDC_EDIT_DIEMOB_COMMENT, IDC_EDIT_GUARDNPC_COMMENT,
									IDC_EDIT_REACH_MAP_COMMENT, IDC_EDIT_REQ_QITEM_COMMENT, IDC_EDIT_GUARD_MAP_COMMENT,
									IDC_BUTTON_STEP_OK, IDC_BUTTON_PREV, IDC_BUTTON_NEXT, IDC_BUTTON_EDITCANCEL, 
									IDC_BUTTON_EDITCOMPLATE };

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
void CQuestStepPage::OnBnClickedStepMapEditButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgMap mapSelect;
	mapSelect.DoModal();

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];
    sSTEP.m_stepMoveMap.nidMAP = mapSelect.m_nidMAP;
	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonItemuse()
{
	InverseUpdateItems();

	if ( m_dwCurStep==UINT_MAX )
		return;

	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgItem sDlgItem;

	if ( IDOK == sDlgItem.DoModal () )
	{
		sSTEP.m_dwNID_ITEMUSE = sDlgItem.m_nidITEM.dwID;

		SITEMCUSTOM sITEMCUSTOM(sDlgItem.m_nidITEM);

		SITEM* pITEM = GLogicData::GetInstance().GetItem ( sITEMCUSTOM.GetNativeID() );

		if ( !pITEM )
			return;

		//if ( pITEM->ISPILE() )	sITEMCUSTOM.wTurnNum = pITEM->GETAPPLYNUM();

		CString strCROWNAME = "";
		strCROWNAME = pITEM->GetName();
		SetWin_Text ( this, IDC_EDIT_ITEMUSE_NAME, strCROWNAME );

		SNATIVEID sID = SNATIVEID( sSTEP.m_dwNID_ITEMUSE );

		CString strNpcNum;
		if( sID != NATIVEID_NULL() )
			strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
		else
			strNpcNum = "";
		SetWin_Text( this, IDC_EDIT_ITEMUSE_ID, strNpcNum );

	}
}

void CQuestStepPage::OnBnClickedButtonItemuseDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )
		return;

	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_dwNID_ITEMUSE = UINT_MAX;

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedButtonItemgrinding()
{
	InverseUpdateItems();

	if ( m_dwCurStep==UINT_MAX )
		return;

	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	CDlgItem sDlgItem;

	if ( IDOK == sDlgItem.DoModal () )
	{
		sSTEP.m_dwNID_ITEMGRINDING = sDlgItem.m_nidITEM.dwID;

		SITEMCUSTOM sITEMCUSTOM(sDlgItem.m_nidITEM);

		SITEM* pITEM = GLogicData::GetInstance().GetItem ( sITEMCUSTOM.GetNativeID() );

		if ( !pITEM )
			return;

		CString strCROWNAME = "";
		strCROWNAME = pITEM->GetName();
		SetWin_Text ( this, IDC_EDIT_ITEMGRINDING_NAME, strCROWNAME );

		SNATIVEID sID = SNATIVEID( sSTEP.m_dwNID_ITEMGRINDING );

		CString strNpcNum;
		if( sID != NATIVEID_NULL() )
			strNpcNum.Format ( "%d/%d", sID.wMainID, sID.wSubID );
		else
			strNpcNum = "";
		SetWin_Text( this, IDC_EDIT_ITEMGRINDING_ID, strNpcNum );

	}
}

void CQuestStepPage::OnBnClickedButtonItemgrindingDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )
		return;

	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_dwNID_ITEMGRINDING = UINT_MAX;

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedRadioItemgrinding1()
{
	BOOL bEnable = true;
	//if ( GetWin_Combo_Sel ( this, IDC_RADIO_ITEMGRINDING1 ) )
	//	bEnable = true;

	SetWin_Enable( this, IDC_BUTTON_ITEMGRINDING, bEnable );
	SetWin_Enable( this, IDC_EDIT_ITEMGRINDING_NAME, bEnable );
	SetWin_Enable( this, IDC_EDIT_ITEMGRINDING_ID, bEnable );
}

void CQuestStepPage::OnBnClickedRadioItemgrinding2()
{
	BOOL bEnable = false;
	//if ( GetWin_Combo_Sel ( this, IDC_RADIO_ITEMGRINDING2 ) )
	//	bEnable = true;

	SetWin_Enable( this, IDC_BUTTON_ITEMGRINDING, bEnable );
	SetWin_Enable( this, IDC_EDIT_ITEMGRINDING_NAME, bEnable );
	SetWin_Enable( this, IDC_EDIT_ITEMGRINDING_ID, bEnable );
}

void CQuestStepPage::OnBnClickedRadioItemgrinding3()
{
	BOOL bEnable = false;
	//if ( GetWin_Combo_Sel ( this, IDC_RADIO_ITEMGRINDING3 ) )
	//	bEnable = true;

	SetWin_Enable( this, IDC_BUTTON_ITEMGRINDING, bEnable );
	SetWin_Enable( this, IDC_EDIT_ITEMGRINDING_NAME, bEnable );
	SetWin_Enable( this, IDC_EDIT_ITEMGRINDING_ID, bEnable );
}

void CQuestStepPage::OnBnClickedRadioItemgrinding4()
{
	BOOL bEnable = false;
	//if ( GetWin_Combo_Sel ( this, IDC_RADIO_ITEMGRINDING4 ) )
	//	bEnable = true;

	SetWin_Enable( this, IDC_BUTTON_ITEMGRINDING, bEnable );
	SetWin_Enable( this, IDC_EDIT_ITEMGRINDING_NAME, bEnable );
	SetWin_Enable( this, IDC_EDIT_ITEMGRINDING_ID, bEnable );
}

void CQuestStepPage::OnBnClickedCheckTokillplayer()
{
	BOOL bEnable = false;

	if ( GetWin_Check ( this, IDC_CHECK_TOKILLPLAYER ) )
		bEnable = false;
	else
		bEnable = true;

	SetWin_Enable( this, IDC_BUTTON_DIEMOB, bEnable );
	SetWin_Enable( this, IDC_EDIT_DIEMOB_NAME, bEnable );
	SetWin_Enable( this, IDC_EDIT_TALKNPC_NUM2, bEnable );
}

void CQuestStepPage::OnBnClickedButtonConftingDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )
		return;

	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_dwType_CONFTING = EMCONFT_NONE;

	UpdateItems_Step ();
}

void CQuestStepPage::OnBnClickedRadioConfting1()
{
	SetWin_Enable( this, IDC_EDIT_CONFTING_COMMENT, false );
	SetWin_Enable( this, IDC_EDIT_CONFTING_NUM, false );
	SetWin_Enable( this, IDC_BUTTON_CONFTING_DEL, false );
}

void CQuestStepPage::OnBnClickedRadioConfting2()
{
	SetWin_Enable( this, IDC_EDIT_CONFTING_COMMENT, true );
	SetWin_Enable( this, IDC_EDIT_CONFTING_NUM, true );
	SetWin_Enable( this, IDC_BUTTON_CONFTING_DEL, true );
}

void CQuestStepPage::OnBnClickedRadioConfting3()
{
	SetWin_Enable( this, IDC_EDIT_CONFTING_COMMENT, true );
	SetWin_Enable( this, IDC_EDIT_CONFTING_NUM, true );
	SetWin_Enable( this, IDC_BUTTON_CONFTING_DEL, true );
}

void CQuestStepPage::OnBnClickedRadioConfting4()
{
	SetWin_Enable( this, IDC_EDIT_CONFTING_COMMENT, true );
	SetWin_Enable( this, IDC_EDIT_CONFTING_NUM, true );
	SetWin_Enable( this, IDC_BUTTON_CONFTING_DEL, true );
}

void CQuestStepPage::OnBnClickedRadioConfting5()
{
	SetWin_Enable( this, IDC_EDIT_CONFTING_COMMENT, true );
	SetWin_Enable( this, IDC_EDIT_CONFTING_NUM, true );
	SetWin_Enable( this, IDC_BUTTON_CONFTING_DEL, true );
}

void CQuestStepPage::OnBnClickedButtonLevelUpDel()
{
	InverseUpdateItems ();

	if ( m_dwCurStep==UINT_MAX )
		return;

	if ( CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep.size() <= m_dwCurStep )
		return;

	GLQUEST_STEP &sSTEP = CQuestEditDlg::m_sQuestNode.pQUEST->m_vecProgStep[m_dwCurStep];

	sSTEP.m_dwNUM_LEVELUP = 0;

	UpdateItems_Step ();
}