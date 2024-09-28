// NpcActionAnswer.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../NpcAction.h"
#include "./NpcActionAnswer.h"
#include "../Edit/SheetWithTab.h"
//#include "NpcTalkDialog.h"
#include "../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../EngineLib/Common/StlFunctions.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/SingleobjectTextControl.h"
//#include <strstream>
#include "../../MfcExLib/ExLibs/EtcFunction.h"


// CNpcActionAnswer ��ȭ �����Դϴ�.

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CNpcActionAnswer, CPropertyPage)
CNpcActionAnswer::CNpcActionAnswer( LOGFONT logfont )
	: CPropertyPage(CNpcActionAnswer::IDD)
    , m_pFont ( NULL )
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CNpcActionAnswer::~CNpcActionAnswer()
{	
	SAFE_DELETE ( m_pFont );
}

void CNpcActionAnswer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ANSWER, m_ctrlAnswer);
}


BEGIN_MESSAGE_MAP(CNpcActionAnswer, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_DELALL, OnBnClickedButtonDelall)
	ON_BN_CLICKED(IDC_BUTTON_CONDITON_EDIT, OnBnClickedButtonConditonEdit)
	ON_BN_CLICKED(IDC_BUTTON_CONDITON_DEL, OnBnClickedButtonConditonDel)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_ANSWER, OnNMDblclkListAnswer)
ON_BN_CLICKED(IDC_BUTTON_COPY, OnBnClickedButtonCopy)
END_MESSAGE_MAP()


// CNpcActionAnswer �޽��� ó�����Դϴ�.

void CNpcActionAnswer::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	InverseUpdateItems ();

	if (m_spOrgDialogueCase)
        *m_spOrgDialogueCase.get() = m_DummyDialogueCase;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strTalkKey;
		strTalkKey.Format( "%s_%d_%d"
			, m_spNpcDialogueSet->m_TalkName.c_str()
			, m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID()
			, m_dwItemIndex );
		
		CString strTalk = GetWin_Text(this, IDC_EDIT_DIALOGUE_BASIC_TALK);
		CSingleobjectTextControl::GetInstance().InsertString(strTalkKey.GetString(), strTalk.GetString());
	}

	m_pSheetTab->SetActivePage ( NPCACTIONPAGE );
	m_pSheetTab->m_NpcActionPage.UpdateItems ();
}

BOOL CNpcActionAnswer::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );
	m_ctrlAnswer.SetExtendedStyle (
		m_ctrlAnswer.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	RECT	rect;
	m_ctrlAnswer.GetClientRect ( &rect );
	LONG	Width = rect.right - rect.left;

	LV_COLUMN	lvcolumn;
	const int	ColumnCount = 4;	
	char*		ColumnName[ColumnCount] = { "ID", "Answer", "Action Type", "Action Data" };
	int			ColumnWidthPercent[ColumnCount] = { 10, 50, 20, 20 };

	for ( int i = 0; i < ColumnCount; i++ )
	{		
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM
			| LVCF_TEXT | LVCF_WIDTH;// | LVCF_IMAGE;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = ColumnName[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = ( Width * ColumnWidthPercent[i] ) / 100;
		m_ctrlAnswer.InsertColumn (i, &lvcolumn);		
	}

	SetReadMode();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CNpcActionAnswer::UpdateItems ()
{
	std::string strCondition;
	std::tr1::shared_ptr<SNpcTalkCondition> spCondition = m_DummyDialogueCase.GetCondition();
	if (spCondition)
        spCondition->GetConditionText( strCondition );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		strKey.Format("%s_%d_%d", m_spNpcDialogueSet->m_TalkName.c_str(), m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID(), m_dwItemIndex);
		// const char* szTalkText = CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT).GetString();
		const char* szTalkText = CSingleobjectTextControl::GetInstance().GetText(strKey.GetString()).GetString();
		SetWin_Text( this, IDC_EDIT_DIALOGUE_BASIC_TALK, szTalkText );
	}
	else
	{
		SetWin_Text ( this, IDC_EDIT_DIALOGUE_BASIC_TALK, m_DummyDialogueCase.GetBasicTalk().c_str() );
	}
	SetWin_Text ( this, IDC_EDIT_DIALOGUE_CONDITION, strCondition.c_str() );

	m_ctrlAnswer.DeleteAllItems ();

	TCHAR szTemp[32] = {0};

	CNpcTalkControl::NPCMAP* pMap = m_DummyTalkControl.GetTalkMap ();

	CNpcTalkControl::NPCMAP_IT iter = pMap->begin();
	CNpcTalkControl::NPCMAP_IT iter_end = pMap->end();	
	for ( int nIndex = 0; iter!=iter_end; ++iter, nIndex++ )
	{
		std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;

		//std::strstream strNID;
		//strNID << pNpcTalk->m_dwNID << std::ends;

		_snprintf( szTemp, 32, "%u", spNpcTalk->m_dwNID );

		int nItem = m_ctrlAnswer.InsertItem ( nIndex, szTemp );		
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("%s_%d_%d_%d"
				, m_spNpcDialogueSet->m_TalkName.c_str()
				, m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID()
				, m_dwItemIndex, spNpcTalk->m_dwNID);
			// const char* szTalkText = CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT).GetString();
			const char* szTalkText = CSingleobjectTextControl::GetInstance().GetText(strKey.GetString()).GetString();

			m_ctrlAnswer.SetItemText ( nIndex, 1, szTalkText );
		}
		else
		{
			m_ctrlAnswer.SetItemText ( nIndex, 1, spNpcTalk->GetTalk() );
		}
		m_ctrlAnswer.SetItemText ( nIndex, 2, SNpcTalk::strACTIONTYPE[spNpcTalk->m_nACTION].c_str() );


		std::string strACT_NO;
		switch ( spNpcTalk->m_nACTION )
		{
		case SNpcTalk::EM_PAGE_MOVE:
			m_spNpcDialogueSet->GetDlgText( spNpcTalk->m_dwACTION_NO, strACT_NO );
			break;

		case SNpcTalk::EM_BASIC:
			strACT_NO =  SNpcTalk::szBASICDESC[spNpcTalk->m_dwACTION_NO].c_str();
			break;

		case SNpcTalk::EM_QUEST_START:
			m_spNpcDialogueSet->GetDlgText( spNpcTalk->m_dwACTION_NO, strACT_NO );
			break;

		case SNpcTalk::EM_QUEST_STEP:
			m_spNpcDialogueSet->GetDlgText( spNpcTalk->m_dwACTION_NO, strACT_NO );
			break;

		default:
			//std::strstream strStream;
			//strStream << pNpcTalk->m_dwACTION_NO << std::ends;
			//strACT_NO = strStream.str();
			//strStream.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

			_snprintf( szTemp, 32, "%u", spNpcTalk->m_dwACTION_NO );
			strACT_NO = szTemp;
			break;
		};

		m_ctrlAnswer.SetItemText ( nIndex, 3, strACT_NO.c_str() );

		m_ctrlAnswer.SetItemData ( nItem, (DWORD_PTR)spNpcTalk->m_dwNID );

		//strNID.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}
}

void CNpcActionAnswer::SetDialogueCase(spNpcDialogSet pNpcDialogueSet, std::tr1::shared_ptr<CNpcDialogueCase> spDialogueCase, const DWORD dwItemIndex )
{	
	assert ( spDialogueCase && "CNpcActionAnswer::SetDialogueCase () �� ������ �߸��Ǿ����ϴ�." );	

	m_DummyTalkControl.RemoveTalk ();
	m_DummyDialogueCase.Reset ();

	m_spNpcDialogueSet = pNpcDialogueSet;
	m_spOrgDialogueCase = spDialogueCase;
	m_DummyDialogueCase = *spDialogueCase.get();

	if ( m_DummyDialogueCase.GetTalkControl () )
	{
		m_DummyTalkControl = *(m_DummyDialogueCase.GetTalkControl ());
	}		

	m_dwItemIndex = dwItemIndex;
	UpdateItems ();
}

void CNpcActionAnswer::OnBnClickedButtonAdd()
{
	InverseUpdateItems();
    std::tr1::shared_ptr<SNpcTalk> spTalk(new SNpcTalk);

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strInsertKey;
		strInsertKey.Format("%s_%d_%d_%d"
			, m_spNpcDialogueSet->m_TalkName.c_str()
			, m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID(), m_dwItemIndex, m_ctrlAnswer.GetItemCount()+1);
		// CGameTextMan::GetInstance().InsertString(strInsertKey.GetString(), "skip", CGameTextMan::EM_NPCTALK_TEXT);
		CSingleobjectTextControl::GetInstance().InsertString(strInsertKey.GetString(), "skip");
	}
	m_pSheetTab->ActiveActionTalk(NPCACTIONANSWER, m_pSheetTab->GetDialogueSet(), &m_DummyTalkControl, FALSE, spTalk);
}

void CNpcActionAnswer::OnBnClickedButtonModify()
{
	POSITION	pos = m_ctrlAnswer.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlAnswer.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return ;
	}

	DWORD NativeID = (DWORD) m_ctrlAnswer.GetItemData ( nItem );

	m_pSheetTab->ActiveActionTalk ( NPCACTIONANSWER, m_pSheetTab->GetDialogueSet(), &m_DummyTalkControl, TRUE, m_DummyTalkControl.GetTalk(NativeID) );
}

void CNpcActionAnswer::OnBnClickedButtonDel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION	pos = m_ctrlAnswer.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlAnswer.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return;
	}

	DWORD NativeID = (DWORD) m_ctrlAnswer.GetItemData ( nItem );
	CString BasicTalk = m_DummyTalkControl.GetTalk ( NativeID )->GetTalk();
	CString strTemp;
	strTemp.Format ( "Do you want to delete?\n[%03d] %s", NativeID, BasicTalk );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_DummyTalkControl.DelTalk ( NativeID );
		UpdateItems ();
	}
}

void CNpcActionAnswer::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SetActivePage ( NPCACTIONPAGE );
}


void CNpcActionAnswer::InverseUpdateItems ()
{
	CString strTalk = GetWin_Text ( this, IDC_EDIT_DIALOGUE_BASIC_TALK );

	m_DummyDialogueCase.SetBasicTalk ( strTalk.GetString() );

	m_DummyDialogueCase.ResetTalkControl ();
	if ( m_DummyTalkControl.GetTalkMap()->size () )
	{
		m_DummyDialogueCase.SetTalkControl ( m_DummyTalkControl );
	}
}
void CNpcActionAnswer::OnBnClickedButtonDelall()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strTemp;
	strTemp.Format ( "Do you want to delete all?" );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_DummyTalkControl.RemoveTalk ();

		UpdateItems ();
	}
}

void CNpcActionAnswer::OnBnClickedButtonConditonEdit()
{
	std::tr1::shared_ptr<SNpcTalkCondition> spConditon = m_DummyDialogueCase.GetCondition();
	if (!spConditon)
	{
		m_DummyDialogueCase.SetCondition ( SNpcTalkCondition() );
	}

	spConditon = m_DummyDialogueCase.GetCondition();

	m_pSheetTab->ActiveActionCondition ( NPCACTIONANSWER, spConditon );
}

void CNpcActionAnswer::OnBnClickedButtonConditonDel()
{
    if ( MessageBox ( "Do you want to delete?", "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_DummyDialogueCase.ResetCondition();
		UpdateItems ();
	}
}

void CNpcActionAnswer::OnBnClickedButtonUp()
{
	POSITION	pos = m_ctrlAnswer.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlAnswer.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return;
	}

	DWORD dwNID = (DWORD) m_ctrlAnswer.GetItemData ( nItem );

	DWORD dwNID_NEW = m_DummyTalkControl.ToUp ( dwNID );
	if( UINT_MAX != dwNID_NEW ) 
	{
		UpdateItems ();
		m_ctrlAnswer.SetItemState( --nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

	}

	//std::strstream strNID;
	//strNID << dwNID_NEW << std::ends;

	//LVFINDINFO info;

	//info.flags = LVFI_PARTIAL|LVFI_STRING;
	//info.psz = strNID.str();

	//int nIndex = m_ctrlAnswer.FindItem ( &info );
	//m_ctrlAnswer.SetSelectedColumn ( nIndex );

	//strNID.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
}

void CNpcActionAnswer::OnBnClickedButtonDown()
{
	POSITION	pos = m_ctrlAnswer.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlAnswer.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return;
	}

	DWORD dwNID = (DWORD) m_ctrlAnswer.GetItemData ( nItem );

	DWORD dwNID_NEW = m_DummyTalkControl.ToDown ( dwNID );

	if( UINT_MAX != dwNID_NEW ) 
	{
		UpdateItems ();
		m_ctrlAnswer.SetItemState( ++nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

	}

	//std::strstream strNID;
	//strNID << dwNID_NEW << std::ends;

	//LVFINDINFO info;

	//info.flags = LVFI_PARTIAL|LVFI_STRING;
	//info.psz = strNID.str();

	//int nIndex = m_ctrlAnswer.FindItem ( &info );
	//m_ctrlAnswer.SetSelectedColumn ( nIndex );

	//strNID.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
}


void CNpcActionAnswer::SetReadMode()
{
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM
	const int nSkipNum = 5;
	const int nSkipID[nSkipNum] = 
    { 
        IDC_BUTTON_OK, 
        IDC_BUTTON_CANCEL, 
        IDC_EDIT_DIALOGUE_BASIC_TALK, 
        IDC_LIST_ANSWER, 
        IDC_BUTTON_MODIFY
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

void CNpcActionAnswer::OnNMDblclkListAnswer(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	NMITEMACTIVATE* pNMIA = (NMITEMACTIVATE*) pNMHDR;

	if ( pNMIA->iItem == -1 )
	{
		return ;
	}

	DWORD NativeID = (DWORD) m_ctrlAnswer.GetItemData ( pNMIA->iItem );

	m_pSheetTab->ActiveActionTalk ( NPCACTIONANSWER, m_pSheetTab->GetDialogueSet(), &m_DummyTalkControl, TRUE, m_DummyTalkControl.GetTalk(NativeID) );
}

void CNpcActionAnswer::OnBnClickedButtonCopy()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION	pos = m_ctrlAnswer.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlAnswer.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return ;
	}

	DWORD NativeID = (DWORD) m_ctrlAnswer.GetItemData ( nItem );
	
	std::tr1::shared_ptr<SNpcTalk> spTalk = m_DummyTalkControl.GetTalk(NativeID);
	std::tr1::shared_ptr<SNpcTalk> spNewTalk(new SNpcTalk);
	*spNewTalk.get() = *spTalk.get();
	spNewTalk->m_dwNID = m_DummyTalkControl.MakeTalkNID ();
	m_DummyTalkControl.AddTalk( spNewTalk );

	UpdateItems ();
}
