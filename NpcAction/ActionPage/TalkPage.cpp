// TalkPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../NpcAction.h"
#include "./TalkPage.h"
#include "../Edit/SheetWithTab.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/NpcTalk/NpcTalkCondition.h"
#include "../../RanLogic//NpcTalk/NpcDialogueSet.h"
#include "../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../RanLogic/NpcTalk/NpcDialogueCase.h"
#include "../../RanLogic/NpcTalk/NpcTalkControl.h"
#include "./HaveItemDialog.h"
#include "../Dialogue/DlgNpcTalk.h"
#include "../Dialogue/MobSearchDialog.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/SingleobjectTextControl.h"

#define MAX_RANDOM_TIME		999

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CTalkPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTalkPage, CPropertyPage)
CTalkPage::CTalkPage( LOGFONT logfont )
	: CPropertyPage(CTalkPage::IDD)
    , m_pSheetTab(NULL)
    , m_CallPage(0)
    , m_bModify(FALSE)    
    , m_pTalkControl(NULL)
    , m_pFont(NULL)
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CTalkPage::~CTalkPage()
{
	SAFE_DELETE ( m_pFont );
}

void CTalkPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_QUEST_START, m_comboQUESTSTART[0]);
	DDX_Control(pDX, IDC_COMBO_QUEST_START2, m_comboQUESTSTART[1]);
	DDX_Control(pDX, IDC_COMBO_QUEST_START3, m_comboQUESTSTART[2]);
	DDX_Control(pDX, IDC_COMBO_QUEST_START4, m_comboQUESTSTART[3]);
	DDX_Control(pDX, IDC_COMBO_QUEST_START5, m_comboQUESTSTART[4]);
	DDX_Control(pDX, IDC_COMBO_QUEST_PROG, m_comboQUEST_PROC);
	DDX_Control(pDX, IDC_COMBO_QUEST_PROG_STEP, m_comboQUEST_PROC_STEP);
	DDX_Control(pDX, IDC_LIST_CASE_RANDOM, m_ctrlRandomID);
	DDX_Control(pDX, IDC_LIST_HAVE_ITEM, m_ctrNeedItem);
	DDX_Control(pDX, IDC_LIST_TRADE_ITEM, m_ctrTradeItem);
}


BEGIN_MESSAGE_MAP(CTalkPage, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION_TYPE, OnCbnSelchangeComboActionType)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION_DATA, OnCbnSelchangeComboActionData)
	ON_BN_CLICKED(IDC_BUTTON_CONDITON_SET, OnBnClickedButtonConditonSet)
	ON_BN_CLICKED(IDC_BUTTON_CONDITON_RESET, OnBnClickedButtonConditonReset)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_CONTROL_RANGE( CBN_SELCHANGE, IDC_COMBO_QUEST_START, IDC_COMBO_QUEST_START5,OnCbnSelchangeComboQuestStart )
	ON_CBN_SELCHANGE(IDC_COMBO_QUEST_PROG, OnCbnSelchangeComboQuestProg)
	ON_CBN_SELCHANGE(IDC_COMBO_QUEST_PROG_STEP, OnCbnSelchangeComboQuestProgStep)
	ON_BN_CLICKED(IDC_BUTTON_BUS, OnBnClickedButtonBus)
	ON_BN_CLICKED(IDC_RANDOM_INSERT, OnBnClickedRandomInsert)
	ON_BN_CLICKED(IDC_RANDOM_DELETE, OnBnClickedRandomDelete)
	ON_BN_CLICKED(IDC_RANDOM_RESET, OnBnClickedRandomReset)
	ON_BN_CLICKED(IDC_BUTTON_TRADE_ITEM_ADD, OnBnClickedButtonTradeItemAdd)
	ON_BN_CLICKED(IDC_BUTTON_TRADE_ITEM_DEL, OnBnClickedButtonTradeItemDel)
	ON_BN_CLICKED(IDC_BUTTON_TRADE_ITEM_DELALL, OnBnClickedButtonTradeItemDelall)
	ON_BN_CLICKED(IDC_BUTTON_HIRE_SUMMON, OnBnClickedButtonHireSummon)
END_MESSAGE_MAP()


// CTalkPage 메시지 처리기입니다.

BOOL CTalkPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

    CComboBox* pComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_TYPE );
	for ( int i = 0; i < SNpcTalk::EM_TALK_SIZE; i++ )
	{
		int nIndex = pComboBox->InsertString ( i, SNpcTalk::strACTIONTYPE[i].c_str() );
		pComboBox->SetItemData ( nIndex, i );
	}


	m_ctrlRandomID.SetExtendedStyle (
		m_ctrlRandomID.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	RECT	rect;
	m_ctrlRandomID.GetClientRect ( &rect );
	LONG	Width = rect.right - rect.left;

	LV_COLUMN	lvcolumn;
	const int	ColumnCount = 2;	
	char*		ColumnName[ColumnCount] = { "ID", "Desc" };
	int			ColumnWidthPercent[ColumnCount] = { 20, 80 };

	for ( int i = 0; i < ColumnCount; i++ )
	{		
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM
			| LVCF_TEXT | LVCF_WIDTH;// | LVCF_IMAGE;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = ColumnName[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = ( Width * ColumnWidthPercent[i] ) / 100;
		m_ctrlRandomID.InsertColumn (i, &lvcolumn);		
	}

	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	m_ctrNeedItem.GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;



	const int nColumnCount = 2;	
	char* szColumnName1[nColumnCount] = { "Item", "Num" };

	int nColumnWidthPercent1[nColumnCount] = { 80, 20 };

	for ( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = szColumnName1[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent1[i] ) / 100;
		m_ctrNeedItem.InsertColumn (i, &lvColumn );
		m_ctrTradeItem.InsertColumn (i, &lvColumn );
	}




	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTalkPage::UpdateItems ()
{
	CComboBox* pComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_TYPE );

	if ( !m_bModify )
	{
		DWORD BackupAction   = SNpcTalk::EM_DO_NOTHING;		
		DWORD BackupActionID = UINT_MAX;
		if( m_Talk.m_nACTION != SNpcTalk::EM_DO_NOTHING &&
			m_Talk.m_dwACTION_NO != UINT_MAX )
		{
			BackupAction   = m_Talk.m_nACTION;		
			BackupActionID = m_Talk.m_dwACTION_NO;
		}		

		m_Talk.Init ();
		m_Talk.m_dwNID = m_pTalkControl->MakeTalkNID ();
		m_Talk.m_nACTION	 = BackupAction;
		m_Talk.m_dwACTION_NO = BackupActionID;

		CString strTemp;

		strTemp.Format ( "%d", m_Talk.m_dwNID );		 
		GetDlgItem ( IDC_EDIT_TALKID )->SetWindowText ( strTemp );

//		pComboBox->SetCurSel ( 0 );
		pComboBox->SetCurSel ( m_Talk.m_nACTION );

		//	콤보박스도 자동으로 선택되어있도록 하기 위함임
		BackupActionID = m_Talk.m_dwACTION_NO;
		OnCbnSelchangeComboActionType ();

		CComboBox* pDataComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
		m_Talk.m_dwACTION_NO = BackupActionID;

		int nSize = pDataComboBox->GetCount ();
		for ( int i=0; i<nSize; i++ )
		{
			DWORD dwNO = (DWORD) pDataComboBox->GetItemData ( i );
			if ( dwNO==m_Talk.m_dwACTION_NO )
			{
				pDataComboBox->SetCurSel ( i );
				break;
			}
		}

		SetWndView ( m_Talk.m_nACTION, m_Talk.m_dwACTION_NO );

	}
	else
	{
		CString strTemp;

		strTemp.Format ( "%d", m_Talk.m_dwNID );		 
		GetDlgItem ( IDC_EDIT_TALKID )->SetWindowText ( strTemp );

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("%s_%d_%d_%d"
				, m_spNpcDialogueSet->m_TalkName.c_str()
				, m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID() 
				, m_pSheetTab->m_NpcActionAnswer.m_dwItemIndex
				, m_Talk.m_dwNID );
			// const char* szTalkText = CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT).GetString();
			const char* szTalkText = CSingleobjectTextControl::GetInstance().GetText(strKey.GetString()).GetString();

			GetDlgItem ( IDC_EDIT_TALK )->SetWindowText ( szTalkText );
		}
		else
		{
			GetDlgItem ( IDC_EDIT_TALK )->SetWindowText ( m_Talk.GetTalk() );		
		}

		pComboBox->SetCurSel ( m_Talk.m_nACTION );

		//	콤보박스도 자동으로 선택되어있도록 하기 위함임
		DWORD BackupActionID = m_Talk.m_dwACTION_NO;
		OnCbnSelchangeComboActionType ();
		
		CComboBox* pDataComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
		m_Talk.m_dwACTION_NO = BackupActionID;

		int nSize = pDataComboBox->GetCount ();
		for ( int i=0; i<nSize; i++ )
		{
			DWORD dwNO = (DWORD) pDataComboBox->GetItemData ( i );
			if ( dwNO==m_Talk.m_dwACTION_NO )
			{
				pDataComboBox->SetCurSel ( i );
				break;
			}
		}

		GetDlgItem( IDC_EDIT_BUS )->SetWindowText( m_Talk.m_strBusFile.c_str() );

		GetDlgItem( IDC_EDIT_HIRE_SUMMON_MID )->SetWindowText( sc::string::format( "%1%", m_Talk.m_sHireSummon.Mid() ).c_str() );

		GetDlgItem( IDC_EDIT_HIRE_SUMMON_SID )->SetWindowText( sc::string::format( "%1%", m_Talk.m_sHireSummon.Sid() ).c_str() );

		std::string strCondition;
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition = m_Talk.GetCondition();
		if (spCondition)
            spCondition->GetConditionText(strCondition);
		GetDlgItem(IDC_EDIT_CONDITION)->SetWindowText(strCondition.c_str());

		SetWndView(m_Talk.m_nACTION, m_Talk.m_dwACTION_NO);
	}
}

void CTalkPage::SetTalkData(spNpcDialogSet pNpcDialogueSet, CNpcTalkControl* pTalkControl)
{
	assert ( pTalkControl && "널이 설정되고 있습니다." );
	assert ( pNpcDialogueSet && "널이 설정되고 있습니다." );

	m_spNpcDialogueSet = pNpcDialogueSet;
	m_pTalkControl = pTalkControl;
}

void CTalkPage::SetModifyData(BOOL bModify, std::tr1::shared_ptr<SNpcTalk> spTalk)
{
	m_bModify = bModify;
	if ( bModify )
	{
		m_spOrgTalk = spTalk;
		m_Talk = *spTalk.get();
	}
}

void CTalkPage::OnCbnSelchangeComboActionType()
{
	CComboBox* pDataComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
	pDataComboBox->ResetContent ();

	CComboBox* pTypeComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_TYPE );
	int nIndex = pTypeComboBox->GetCurSel ();
	m_Talk.m_nACTION = (int)pTypeComboBox->GetItemData ( nIndex );
    
	switch ( nIndex )
	{
	case SNpcTalk::EM_DO_NOTHING:
		{
		}
		break;

	case SNpcTalk::EM_PAGE_MOVE:
	case SNpcTalk::EM_QUEST_START:
	case SNpcTalk::EM_QUEST_STEP:
		{
			CNpcDialogueSet::DIALOGMAP* pMap = m_spNpcDialogueSet->GetDialogueMap ();
			if ( pMap->size() )
			{
				CNpcDialogueSet::DIALOGMAP_ITER iter = pMap->begin();
				CNpcDialogueSet::DIALOGMAP_ITER iter_end = pMap->end();	
				for ( ; iter!=iter_end; ++iter )
				{
                    std::tr1::shared_ptr<CNpcDialogue> spDialogue = iter->second;
					
					DWORD dwIndex = spDialogue->FindNonCondition();
					if ( dwIndex==UINT_MAX )	dwIndex = 0;

					std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = spDialogue->GetDlgCase(dwIndex);
					if (!spDlgCase)
                        continue;

					CString strTemp;
					strTemp.Format ( "[%03d] %s", spDialogue->GetDlgNID(), spDlgCase->GetBasicTalk ().c_str() );
					int nItem = pDataComboBox->AddString ( strTemp );
					pDataComboBox->SetItemData ( nItem, spDialogue->GetDlgNID() );
				}
				pDataComboBox->SetCurSel ( 0 );

				OnCbnSelchangeComboActionData ();

				SetWndView ( m_Talk.m_nACTION, m_Talk.m_dwACTION_NO );
			}
		}
		break;
	case SNpcTalk::EM_BASIC:
		{
			CComboBox* pComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
			for ( int i = 0; i < SNpcTalk::EM_BASIC_SIZE; i++ )
			{
				int nIndex = pComboBox->InsertString ( i, SNpcTalk::szBASICDESC[i].c_str() );
				pComboBox->SetItemData ( nIndex, i );
			}
			pDataComboBox->SetCurSel ( 0 );

			OnCbnSelchangeComboActionData ();

			SetWndView ( m_Talk.m_nACTION, m_Talk.m_dwACTION_NO );
		}
		break;
	};
}

void CTalkPage::OnCbnSelchangeComboActionData()
{
	CComboBox* pDataComboBox = ( CComboBox* ) GetDlgItem ( IDC_COMBO_ACTION_DATA );
	int nIndex = pDataComboBox->GetCurSel ();
	m_Talk.m_dwACTION_NO = (DWORD)pDataComboBox->GetItemData ( nIndex );

	SetWndView ( m_Talk.m_nACTION, m_Talk.m_dwACTION_NO );
}

void CTalkPage::OnBnClickedButtonConditonSet()
{
	if ( !m_Talk.GetCondition() )
		m_Talk.SetCondition ( SNpcTalkCondition() );

	m_pSheetTab->ActiveActionCondition ( NPCACTIONTALK, m_Talk.GetCondition() );
}

void CTalkPage::OnBnClickedButtonConditonReset()
{
    if ( MessageBox ( "Do you want to delete?", "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_Talk.ResetCondition ();
	}
}

void CTalkPage::OnBnClickedButtonOk()
{
	assert ( m_pTalkControl && "데이타 설정이 잘못되었습니다." );

	CString strTemp;

	GetDlgItem ( IDC_EDIT_HIRE_SUMMON_MID )->GetWindowText( strTemp );
	m_Talk.m_sHireSummon.wMainID = (WORD) atoi ( strTemp.GetString() );

	GetDlgItem ( IDC_EDIT_HIRE_SUMMON_SID )->GetWindowText( strTemp );
	m_Talk.m_sHireSummon.wSubID = (WORD) atoi ( strTemp.GetString() );

	GetDlgItem ( IDC_EDIT_TALKID )->GetWindowText( strTemp );
	m_Talk.m_dwNID = (DWORD) atoi ( strTemp.GetString() );

	GetDlgItem ( IDC_EDIT_BUS )->GetWindowText( strTemp );
	m_Talk.m_strBusFile = strTemp.GetString();

	GetDlgItem ( IDC_EDIT_TALK )->GetWindowText( strTemp );
	m_Talk.m_strTalk = strTemp.GetString();

	m_Talk.m_dwRandomTime = GetWin_Num_int( this, IDC_EDIT_RANDOM_TIME );
	
	if ( m_Talk.m_dwRandomTime > MAX_RANDOM_TIME ) m_Talk.m_dwRandomTime = MAX_RANDOM_TIME;

	if ( !strTemp.GetLength () )
	{
		MessageBox ( "설명부분이 비어있습니다.", "Caution", MB_OK | MB_ICONASTERISK );
		return;
	}

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		strKey.Format("%s_%d_%d_%d"
			, m_spNpcDialogueSet->m_TalkName.c_str()
			, m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID() 
			, m_pSheetTab->m_NpcActionAnswer.m_dwItemIndex
			, m_Talk.m_dwNID );
		CString strTalkText = GetWin_Text(this, IDC_EDIT_TALK).GetString();
		CSingleobjectTextControl::GetInstance().InsertString(strKey.GetString(), strTalkText.GetString());
	}

	if ( !m_bModify )
	{
		std::tr1::shared_ptr<SNpcTalk> spNewTalk(new SNpcTalk);
		*spNewTalk.get() = m_Talk;

		if ( m_pTalkControl->AddTalk ( spNewTalk ) )
		{
            spNpcDialogSet spDlgSet;
            std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase;
			m_pSheetTab->ActiveActionAnswer(spDlgSet, spDlgCase, m_pSheetTab->m_NpcActionAnswer.m_dwItemIndex);
		}
		else
		{
			//SAFE_DELETE ( spNewTalk );
			MessageBox ( "Registered the same ID already." );
		}
	}
	else
	{
		for ( int i = 0;  i< MAX_QUEST_START-1; ++i )
		{
			if ( !GetWin_Check( this, IDC_CHECK_QUEST_START2 + i ) ) 
				m_Talk.m_dwQuestStartID[i+1] = UINT_MAX;
		}

		std::tr1::shared_ptr<SNpcTalk> spTalk = m_pTalkControl->GetTalk ( m_Talk.m_dwNID );

		*spTalk.get() = m_Talk;

        spNpcDialogSet spDlgSet;
        std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase;
		m_pSheetTab->ActiveActionAnswer(spDlgSet, spDlgCase, m_pSheetTab->m_NpcActionAnswer.m_dwItemIndex);
	}
}

void CTalkPage::OnBnClickedButtonCancel()
{
    spNpcDialogSet spDlgSet;
    std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase;
	m_pSheetTab->ActiveActionAnswer(spDlgSet, spDlgCase, m_pSheetTab->m_NpcActionAnswer.m_dwItemIndex);
}

void CTalkPage::SetWndView ( int nACTION, DWORD dwNO )
{
	BOOL bHIRESUMMON = ( nACTION == SNpcTalk::EM_BASIC && dwNO == SNpcTalk::EM_HIRE_SUMMON );
	{
		DWORD dwWNDID[] = {
			IDC_HIRE_SUMMON_STATIC,
			IDC_HIRE_SUMMON_MID_STATIC,
			IDC_HIRE_SUMMON_SID_STATIC,
			IDC_EDIT_HIRE_SUMMON_MID,
			IDC_EDIT_HIRE_SUMMON_SID,
			IDC_BUTTON_HIRE_SUMMON
		};

		DWORD dwWNDNUM = sizeof(dwWNDID)/sizeof(DWORD);

		for ( DWORD i=0; i<dwWNDNUM; ++i )
		{
			CWnd *pWnd = GetDlgItem ( dwWNDID[i] );
			if ( pWnd )
			{
				pWnd->ShowWindow ( bHIRESUMMON );
			}
		}
	}

	BOOL bTRADE = ( nACTION==SNpcTalk::EM_BASIC && dwNO == SNpcTalk::EM_ITEM_TRADE );
	{
		DWORD dwWNDID[] = {
			IDC_BUTTON_TRADE_ITEM_ADD,
			IDC_BUTTON_TRADE_ITEM_DEL,
			IDC_BUTTON_TRADE_ITEM_DELALL
		};

		DWORD dwWNDNUM = sizeof(dwWNDID)/sizeof(DWORD);

		for ( DWORD i=0; i<dwWNDNUM; ++i )
		{
			CWnd *pWnd = GetDlgItem ( dwWNDID[i] );
			if ( pWnd )
			{
				pWnd->ShowWindow ( bTRADE );
			}
		}

		m_ctrNeedItem.DeleteAllItems();
		m_ctrNeedItem.ShowWindow( SW_HIDE );
		m_ctrTradeItem.DeleteAllItems();
		m_ctrTradeItem.ShowWindow ( SW_HIDE );


		if ( bTRADE ) 
		{
			
			CString strNAME;			
			
			SITEM* pITEM = NULL;	

			m_ctrNeedItem.ShowWindow( SW_SHOW );
			m_ctrTradeItem.ShowWindow ( SW_SHOW );

			BYTE i;
			for( i = 0; i < (int)m_Talk.m_vecNeedItem.size(); i++ )
			{
				pITEM = GLogicData::GetInstance().GetItem ( m_Talk.m_vecNeedItem[i].sNativeID );
				
				if ( pITEM )
				{	
					strNAME.Format ( "[%04d/%04d] %s", m_Talk.m_vecNeedItem[i].sNativeID.wMainID,
						m_Talk.m_vecNeedItem[i].sNativeID.wSubID, pITEM->GetName() );

					m_ctrNeedItem.ShowWindow( SW_SHOW );
					int nIndex = m_ctrNeedItem.InsertItem( i, strNAME );

					CString strNum;
					strNum.Format( "%d", m_Talk.m_vecNeedItem[i].dwNum );

					m_ctrNeedItem.SetItemText( nIndex, 1, strNum );
				}
			}


			for( i = 0; i < m_Talk.m_vecTradeItem.size(); i++ )
			{
				pITEM = GLogicData::GetInstance().GetItem ( m_Talk.m_vecTradeItem[i].sNativeID );
				
				if ( pITEM )
				{	
					strNAME.Format ( "[%04d/%04d] %s", m_Talk.m_vecTradeItem[i].sNativeID.wMainID,
						m_Talk.m_vecTradeItem[i].sNativeID.wSubID, pITEM->GetName() );
					m_ctrTradeItem.ShowWindow( SW_SHOW );
					int nIndex = m_ctrTradeItem.InsertItem( i, strNAME );

					CString strNum;
					strNum.Format( "%d", m_Talk.m_vecTradeItem[i].dwNum );

					m_ctrTradeItem.SetItemText( nIndex, 1, strNum );
				}
			}

		}
	}

	BOOL bQSTART = ( nACTION==SNpcTalk::EM_QUEST_START );
	{
		DWORD dwWNDID[] = {
			IDC_STATIC_QUEST_START,
			IDC_COMBO_QUEST_START,
			IDC_COMBO_QUEST_START2,
			IDC_COMBO_QUEST_START3,
			IDC_COMBO_QUEST_START4,
			IDC_COMBO_QUEST_START5,
			IDC_CHECK_QUEST_START2,
			IDC_CHECK_QUEST_START3,
			IDC_CHECK_QUEST_START4,
			IDC_CHECK_QUEST_START5,
		};

		DWORD dwWNDNUM = sizeof(dwWNDID)/sizeof(DWORD);

		for ( DWORD i=0; i<dwWNDNUM; ++i )
		{
			CWnd *pWnd = GetDlgItem ( dwWNDID[i] );
			if ( pWnd )
			{
				pWnd->ShowWindow ( bQSTART );
			}
		}
		
		for ( int i = 0; i < MAX_QUEST_START; ++i )
		{
			m_comboQUESTSTART[i].ResetContent();
		}

		GLQuestMan::MAPQUEST &sQMAP = GLQuestMan::GetInstance().GetMap();
		GLQuestMan::MAPQUEST_ITER &iter = sQMAP.begin();
		GLQuestMan::MAPQUEST_ITER &iter_end = sQMAP.end();
		for ( ; iter!=iter_end; ++iter )
		{
			DWORD dwQUESTID = (*iter).first;
			GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );;

			if ( !pQUEST )	continue;

			CString strNAME;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
				strNAME.Format ( "[%03d] %s", dwQUESTID, CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
			}
			else
			{
				strNAME.Format ( "[%d] %s", dwQUESTID, pQUEST->GetTITLE() );
			}
			int nIndex;
						
			for ( int i = 0; i < MAX_QUEST_START; ++i )
			{
				nIndex = m_comboQUESTSTART[i].AddString ( strNAME );
				m_comboQUESTSTART[i].SetItemData ( nIndex, dwQUESTID );

				if ( dwQUESTID==m_Talk.m_dwQuestStartID[i] )
				{
					m_comboQUESTSTART[i].SetCurSel ( nIndex );
				}
			}			
		}
	}

	BOOL bQSTEP = ( nACTION==SNpcTalk::EM_QUEST_STEP );
	{
		DWORD dwWNDID[] = {
			IDC_STATIC_QUEST_STEP,
			IDC_COMBO_QUEST_PROG,
			IDC_STATIC_QUEST_STEP2,
			IDC_COMBO_QUEST_PROG_STEP
		};

		DWORD dwWNDNUM = sizeof(dwWNDID)/sizeof(DWORD);

		for ( DWORD i=0; i<dwWNDNUM; ++i )
		{
			CWnd *pWnd = GetDlgItem ( dwWNDID[i] );
			if ( pWnd )
			{
				pWnd->ShowWindow ( bQSTEP );
			}
		}

		m_comboQUEST_PROC.ResetContent();

		GLQuestMan::MAPQUEST &sQMAP = GLQuestMan::GetInstance().GetMap();
		GLQuestMan::MAPQUEST_ITER &iter = sQMAP.begin();
		GLQuestMan::MAPQUEST_ITER &iter_end = sQMAP.end();
		for ( ; iter!=iter_end; ++iter )
		{
			DWORD dwQUESTID = (*iter).first;
			GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );;
			if ( !pQUEST )	continue;

			CString strNAME;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
				strNAME.Format ( "[%d] %s", dwQUESTID, CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
			}
			else
			{
				strNAME.Format ( "[%d] %s", dwQUESTID, pQUEST->GetTITLE() );
			}

			int nIndex = m_comboQUEST_PROC.AddString ( strNAME );
			m_comboQUEST_PROC.SetItemData ( nIndex, dwQUESTID );

			if ( dwQUESTID==m_Talk.m_dwACTION_PARAM1 )
			{
				m_comboQUEST_PROC.SetCurSel ( nIndex );
			}
		}

		m_comboQUEST_PROC_STEP.ResetContent();
        if (m_Talk.QuestProgressID() != UINT_MAX)
        {
		    GLQUEST* pQuestData = GLQuestMan::GetInstance().Find (m_Talk.QuestProgressID());
		    if (pQuestData)
		    {
			    DWORD dwSTEPNUM = pQuestData->GetSTEPNUM();
			    for ( DWORD i=0; i<dwSTEPNUM; ++i )
			    {
				    GLQUEST_STEP *pSTEP = pQuestData->GetSTEP(i);
				    if (!pSTEP)
                        continue;

					CString strNAME;
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("QN_%03d_%03d_P_%d", pQuestData->m_sNID.wMainID, pQuestData->m_sNID.wSubID, i);
						strNAME.Format ( "[%d] %s", i,CGameTextMan::GetInstance().GetQuestText(strKey.GetString()));
					}
					else
					{						
						strNAME.Format ( "[%d] %s", i, pSTEP->GetTITLE() );
					}

				    int nIndex = m_comboQUEST_PROC_STEP.AddString ( strNAME );
				    m_comboQUEST_PROC_STEP.SetItemData ( nIndex, i );

				    if ( i==m_Talk.m_dwACTION_PARAM2 )
					    m_comboQUEST_PROC_STEP.SetCurSel ( nIndex );
			    }
		    }
        }
	}

	if( nACTION == SNpcTalk::EM_QUEST_START )
	{
		GetDlgItem( IDC_STATIC_QUEST_STEP3 )->ShowWindow( SW_SHOW );
	}
    else
    {
		GetDlgItem( IDC_STATIC_QUEST_STEP3 )->ShowWindow( SW_HIDE );
	}


	BOOL bRandom = ( nACTION==SNpcTalk::EM_BASIC && dwNO == SNpcTalk::EM_RANDOM_PAGE );
	{
		DWORD dwWNDID[] = {
			IDC_RANDOM_STATIC,
			IDC_LIST_CASE_RANDOM,
			IDC_EDIT_RANDOM_TIME,
			IDC_TIME_STATIC,
			IDC_RANDOM_INSERT,
			IDC_RANDOM_DELETE,
			IDC_RANDOM_RESET,
		};

		DWORD dwWNDNUM = sizeof(dwWNDID)/sizeof(DWORD);

		for ( DWORD i=0; i<dwWNDNUM; ++i )
		{
			CWnd *pWnd = GetDlgItem ( dwWNDID[i] );
			if ( pWnd )
			{
				pWnd->ShowWindow ( bRandom );
			}
		}

		m_ctrlRandomID.DeleteAllItems ();

		for ( int nIndex = 0; nIndex < MAX_RANDOM_PAGE; ++nIndex ) 
		{
			DWORD dwID = m_Talk.m_dwRandomPageID[nIndex];

			if ( dwID == UINT_MAX )
                break;

            std::tr1::shared_ptr<CNpcDialogue> spNpcDialogue = m_spNpcDialogueSet->GetDialogue( dwID );

			if (!spNpcDialogue)
                break;

			DWORD dwIndex = spNpcDialogue->FindNonCondition();
			if (dwIndex == UINT_MAX)
                dwIndex = 0;

			std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = spNpcDialogue->GetDlgCase(dwIndex);
			if (!spDlgCase)
                break;

			CString strID;
			strID.Format( "%u", dwID );

			int nItem = m_ctrlRandomID.InsertItem( nIndex, strID );			
			m_ctrlRandomID.SetItemText ( nIndex, 1, spDlgCase->GetBasicTalk().c_str() );
			m_ctrlRandomID.SetItemData ( nItem, (DWORD_PTR)dwID );
		}

		SetWin_Num_int( this, IDC_EDIT_RANDOM_TIME, m_Talk.m_dwRandomTime );

	}

	SetReadMode();

}

void CTalkPage::OnCbnSelchangeComboQuestStart(UINT nID)
{	
	int dwNum =  4 - (IDC_COMBO_QUEST_START5 - nID );

	int nIndex = m_comboQUESTSTART[dwNum].GetCurSel();
	if ( nIndex==LB_ERR )	return;

	DWORD dwDATA = (DWORD) m_comboQUESTSTART[dwNum].GetItemData ( nIndex );
	m_Talk.m_dwQuestStartID[dwNum] = dwDATA;	
}

void CTalkPage::OnCbnSelchangeComboQuestProg()
{
	int nIndex = m_comboQUEST_PROC.GetCurSel();
	if ( nIndex==LB_ERR )	return;

	DWORD dwDATA = (DWORD) m_comboQUEST_PROC.GetItemData ( nIndex );
	m_Talk.m_dwACTION_PARAM1 = dwDATA;
}

void CTalkPage::OnCbnSelchangeComboQuestProgStep()
{
	int nIndex = m_comboQUEST_PROC_STEP.GetCurSel();
	if ( nIndex==LB_ERR )	return;

	DWORD dwDATA = (DWORD) m_comboQUEST_PROC_STEP.GetItemData ( nIndex );
	m_Talk.m_dwACTION_PARAM2 = dwDATA;
}

void CTalkPage::OnBnClickedButtonBus()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "BUS Station List (*.ini)|*.ini|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".ini",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_BUS, dlg.GetFileName().GetString() );
	}
}

void CTalkPage::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	const int nSkipNum = 4;
	const int nSkipID[nSkipNum] = 
    { 
        IDC_EDIT_TALK, 
        IDC_BUTTON_OK, 
        IDC_BUTTON_CANCEL, 
        IDC_LIST_CASE_RANDOM
    };

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
void CTalkPage::OnBnClickedRandomInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CDlgNpcTalk pDlg( m_spNpcDialogueSet );

	if ( IDOK == pDlg.DoModal() )
	{
		DWORD nID = pDlg.m_dwNpcID;

		if (nID == UINT_MAX)
            return;			
		
        std::tr1::shared_ptr<CNpcDialogue> spNpcDialogue = m_spNpcDialogueSet->GetDialogue( nID );

		if (!spNpcDialogue)
            return;

		DWORD dwIndex = spNpcDialogue->FindNonCondition();
		if (dwIndex == UINT_MAX)
            dwIndex = 0;

		std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = spNpcDialogue->GetDlgCase(dwIndex);
		if (!spDlgCase)
            return;		

		for ( int i = 0; i < MAX_RANDOM_PAGE; ++i ) 
		{
			if ( m_Talk.m_dwRandomPageID[i] == UINT_MAX ) 
			{
				m_Talk.m_dwRandomPageID[i] = nID;
				break;
			}
		}

		UpdateItems();
	}

}

void CTalkPage::OnBnClickedRandomDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nItem = -1;

	nItem = m_ctrlRandomID.GetNextItem( nItem, LVIS_SELECTED );
	if ( nItem < 0 || nItem >= MAX_RANDOM_PAGE ) return;	

	for ( int i = nItem+1; i < MAX_RANDOM_PAGE; ++i  ) 
	{
		m_Talk.m_dwRandomPageID[i-1] = m_Talk.m_dwRandomPageID[i];
	}
	
	m_Talk.m_dwRandomPageID[MAX_RANDOM_PAGE-1] = UINT_MAX;

	UpdateItems();

}

void CTalkPage::OnBnClickedRandomReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	for ( int i = 0; i < MAX_RANDOM_PAGE; ++i ) 
	{
		m_Talk.m_dwRandomPageID[i] = UINT_MAX;
	}

	UpdateItems();
}

void CTalkPage::OnBnClickedButtonTradeItemAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CHaveItemDialog dlg;
	if ( dlg.DoModal () == IDOK )
	{
		if ( dlg.GetItemID()!=UINT_MAX )
		{
			DWORD dwNID = dlg.GetItemID ();

			SNpcTalk::VEC_ITEM_NUM_ITER iter = m_Talk.m_vecTradeItem.begin();
			SNpcTalk::VEC_ITEM_NUM_ITER iter_end = m_Talk.m_vecTradeItem.end();

			for ( ; iter != iter_end; ++iter )
			{
				SITEM_NUM sItemNum = (SITEM_NUM)(*iter);
				if ( sItemNum.sNativeID.dwID == dwNID ) return;
			}

			if ( m_Talk.m_vecTradeItem.size() >= MAX_NEEDITEM_COUNT )
			{
				MessageBox ( "MAX_NEEDITEM_COUNT ERROR" );
				return;
			}

			SITEM_NUM sItemNumNew;
			sItemNumNew.sNativeID = dwNID;
			sItemNumNew.dwNum = dlg.GetNum();

			m_Talk.m_vecTradeItem.push_back ( sItemNumNew );
			std::sort ( m_Talk.m_vecTradeItem.begin(), m_Talk.m_vecTradeItem.end(), HAVE_ITEM_CMP() );			

			SITEM*	pITEM = GLogicData::GetInstance().GetItem ( sItemNumNew.sNativeID );
			CString strNAME;
				
			if ( pITEM )
			{	
				strNAME.Format ( "[%04d/%04d] %s", sItemNumNew.sNativeID.wMainID,
					sItemNumNew.sNativeID.wSubID, pITEM->GetName() );
				
				int nIndex = m_ctrTradeItem.InsertItem( 0, strNAME );

				CString strNum;
				strNum.Format( "%d", sItemNumNew.dwNum );

				m_ctrTradeItem.SetItemText( nIndex, 1, strNum );
			}
		}
	}
}

void CTalkPage::OnBnClickedButtonTradeItemDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = (int)m_ctrTradeItem.GetFirstSelectedItemPosition() -1;	
	if ( nIndex < 0 ) return ;
	if ( nIndex >= (int)m_Talk.m_vecTradeItem.size() ) return;

	CString strTemp; 
	strTemp.Format ( "Do you want to delete?\n" );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_Talk.m_vecTradeItem.erase( m_Talk.m_vecTradeItem.begin()+nIndex );
		m_ctrTradeItem.DeleteItem( nIndex );
	}
}

void CTalkPage::OnBnClickedButtonTradeItemDelall()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTemp;
	strTemp.Format ( "Do you want to delete 'list'?" );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_Talk.m_vecTradeItem.clear ();

		m_ctrTradeItem.DeleteAllItems();
	}

}

void CTalkPage::OnBnClickedButtonHireSummon()
{
	CMobSearchDialog dlg;
	if ( dlg.DoModal () == IDOK )
	{
		const SNATIVEID& sSummonMobID = dlg.GetSummonMobID();

		GetDlgItem( IDC_EDIT_HIRE_SUMMON_MID )->
			SetWindowText( sc::string::format(
			"%1%", sSummonMobID.Mid() ).c_str() );

		GetDlgItem( IDC_EDIT_HIRE_SUMMON_SID )->
			SetWindowText( sc::string::format(
			"%1%", sSummonMobID.Sid() ).c_str() );
	}
}

SITEM_NUM CTalkPage::GetNeedItem( DWORD dwItemNum )
{ 
	if ( dwItemNum < m_Talk.m_vecNeedItem.size() )
	{
		return m_Talk.m_vecNeedItem[dwItemNum];
	}

	return SITEM_NUM();
}
void CTalkPage::InsertNeedItem( SITEM_NUM sItemNum ) 
{ 
	m_Talk.m_vecNeedItem.push_back( sItemNum );
}
