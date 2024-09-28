// SimpleEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "./NpcAction.h"
#include "./NpcActionDlg.h"

#include "../RanLogic/NpcTalk/NpcTalkControl.h"
#include "../RanLogic/NpcTalk/NpcTalk.h"
#include "../RanLogic/NpcTalk/NpcDialogue.h"
#include "../RanLogic/NpcTalk/NpcDialogueSet.h"
#include "./SimpleEdit.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CSimpleEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSimpleEdit, CDialog)
CSimpleEdit::CSimpleEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CSimpleEdit::IDD, pParent)
{
	//m_spDialogueSet = NULL;
	//m_spNpcDialogue = NULL;

	m_OldGetPos     = 0;
	m_OldTextGetPos = 0;
	m_dwSelectCase = 0;
}

CSimpleEdit::~CSimpleEdit()
{
}


void CSimpleEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BASIC_SLIDER, m_Basic_Slider);
	DDX_Control(pDX, IDC_BASIC_EDIT, m_Basic_Edit);
	DDX_Control(pDX, IDC_BASICCONV_EDIT, m_BasicConv_Edit);
	DDX_Control(pDX, IDC_BASICCONV_EDIT2, m_Text_Edit);
	DDX_Control(pDX, IDC_ANSWER_LIST, m_Answer_List);
	DDX_Control(pDX, IDC_ANSWER_EDIT, m_Answer_Edit);
	DDX_Control(pDX, IDC_ANSWERID_EDIT, m_AnswerID_Edit);
	DDX_Control(pDX, IDC_ANSWERTYPE_EDIT, m_AnswerType_Edit);
	DDX_Control(pDX, IDC_ANSWERDATA_EDIT, m_AnswerData_Edit);
	DDX_Control(pDX, IDC_TEXT_SLIDER, m_Text_Slider);

	SetSimpleData();

	DDX_Control(pDX, IDC_TEXT_EDIT, m_TextId_Edit);
}

void CSimpleEdit::SetSimpleData()
{

	m_Basic_Slider.SetRange( 0, (int)m_spDialogueSet->GetDialogueMap()->size()-1 );

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;

	lvColumn.pszText = "ID";
	lvColumn.iSubItem = 0;
	lvColumn.cx = 30 ;
	m_Answer_List.InsertColumn( 0, &lvColumn );

	lvColumn.pszText = "Answer";
	lvColumn.iSubItem = 1;
	lvColumn.cx = 170 ;
	m_Answer_List.InsertColumn( 1, &lvColumn );

	lvColumn.pszText = "Action Type";
	lvColumn.iSubItem = 2;
	lvColumn.cx = 100 ;
	m_Answer_List.InsertColumn( 2, &lvColumn );

	lvColumn.pszText = "Action Data";
	lvColumn.iSubItem = 3;
	lvColumn.cx = 350 ;
	m_Answer_List.InsertColumn( 3, &lvColumn );

	m_Answer_List.SetExtendedStyle ( m_Answer_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_OldGetPos     = -1;
	m_OldTextGetPos = -1;
}

void CSimpleEdit::UpdateAnswerList( DWORD dwCaseIndex )
{
	char szTempChar[128];
	ZeroMemory( szTempChar, 128 );

	m_dwSelectCase = dwCaseIndex;

	m_Answer_List.DeleteAllItems();
	if( m_spNpcDialogue->GetDlgCase( dwCaseIndex )->GetTalkControl() == NULL )
        return;
	if( m_spNpcDialogue->GetDlgCase( dwCaseIndex )->GetTalkControl()->GetTalkMap()->size() == 0 )
        return;
	CNpcTalkControl::NPCMAP_IT iter     = m_spNpcDialogue->GetDlgCase( dwCaseIndex )->GetTalkControl()->GetTalkMap()->begin();
	CNpcTalkControl::NPCMAP_IT iter_end = m_spNpcDialogue->GetDlgCase( dwCaseIndex )->GetTalkControl()->GetTalkMap()->end();	

	int iCount = 0;
	for ( ; iter!=iter_end; ++iter )
	{
        std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;

		sprintf( szTempChar, "%d", spNpcTalk->m_dwNID );
		int nItem = m_Answer_List.InsertItem( iCount, szTempChar );
		m_Answer_List.SetItemData ( nItem, (DWORD_PTR)spNpcTalk->m_dwNID );
		m_Answer_List.SetItemText ( iCount, 1, spNpcTalk->GetTalk() );	//	ID
		m_Answer_List.SetItemText ( iCount, 2, SNpcTalk::strACTIONTYPE[spNpcTalk->m_nACTION].c_str() );	//	ID

		std::string strACT_NO;
		switch ( spNpcTalk->m_nACTION )
		{
		case SNpcTalk::EM_PAGE_MOVE:
			m_spDialogueSet->GetDlgText ( spNpcTalk->m_dwACTION_NO, strACT_NO );
			break;

		case SNpcTalk::EM_BASIC:
			strACT_NO = SNpcTalk::szBASICDESC[spNpcTalk->m_dwACTION_NO].c_str();
			break;

		case SNpcTalk::EM_QUEST_START:
			m_spDialogueSet->GetDlgText ( spNpcTalk->m_dwACTION_NO, strACT_NO );
			break;

		case SNpcTalk::EM_QUEST_STEP:
			m_spDialogueSet->GetDlgText ( spNpcTalk->m_dwACTION_NO, strACT_NO );
			break;

		default:
			_snprintf( szTempChar, 32, "%u", spNpcTalk->m_dwACTION_NO );
			strACT_NO = szTempChar;
			break;
		};

		m_Answer_List.SetItemText ( iCount, 3, strACT_NO.c_str() );

		iCount++;
	}

	UpdateAnswerData( 1 );
}

void CSimpleEdit::UpdateAnswerData( int nIndex  )
{
	CNpcTalkControl::NPCMAP_IT iter     = m_spNpcDialogue->GetDlgCase( m_dwSelectCase )->GetTalkControl()->GetTalkMap()->begin();
	CNpcTalkControl::NPCMAP_IT iter_end = m_spNpcDialogue->GetDlgCase( m_dwSelectCase )->GetTalkControl()->GetTalkMap()->end();	

	int iCount = 1;
	for ( ; iter!=iter_end; ++iter )
	{
		if( nIndex == iCount )
		{
			std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;
			char szTempChar[8];
			sprintf( szTempChar, "%d", spNpcTalk->m_dwNID );
			m_AnswerID_Edit.SetWindowText( szTempChar ) ;
			m_Answer_Edit.SetWindowText( spNpcTalk->GetTalk() ) ;
			m_AnswerType_Edit.SetWindowText( SNpcTalk::strACTIONTYPE[spNpcTalk->m_nACTION].c_str() );

			std::string strACT_NO;
			switch ( spNpcTalk->m_nACTION )
			{
			case SNpcTalk::EM_PAGE_MOVE:
				m_spDialogueSet->GetDlgText ( spNpcTalk->m_dwACTION_NO, strACT_NO );
				break;

			case SNpcTalk::EM_BASIC:
				strACT_NO = SNpcTalk::szBASICDESC[spNpcTalk->m_dwACTION_NO].c_str();
				break;

			case SNpcTalk::EM_QUEST_START:
				m_spDialogueSet->GetDlgText ( spNpcTalk->m_dwACTION_NO, strACT_NO );
				break;

			case SNpcTalk::EM_QUEST_STEP:
				m_spDialogueSet->GetDlgText ( spNpcTalk->m_dwACTION_NO, strACT_NO );
				break;

			default:
				_snprintf( szTempChar, 32, "%u", spNpcTalk->m_dwACTION_NO );
				strACT_NO = szTempChar;
				break;
			};

			m_AnswerData_Edit.SetWindowText( strACT_NO.c_str() );
			break;
		}
		iCount++;

	}
}

void CSimpleEdit::ApplyNPCData()
{
	char szTempChar[512];
	ZeroMemory( szTempChar, 512 );
	m_Text_Edit.GetWindowText( szTempChar, 512 );
	m_spNpcDialogue->GetDlgCase( m_dwSelectCase )->SetBasicTalk( szTempChar );

	if( m_dwSelectCase == 0 )
	{
		m_BasicConv_Edit.SetWindowText( szTempChar );
	}
}

void CSimpleEdit::ApplyAnswerData()
{
	CNpcTalkControl::NPCMAP_IT iter     = m_spNpcDialogue->GetDlgCase( m_dwSelectCase )->GetTalkControl()->GetTalkMap()->begin();
	CNpcTalkControl::NPCMAP_IT iter_end = m_spNpcDialogue->GetDlgCase( m_dwSelectCase )->GetTalkControl()->GetTalkMap()->end();	


	int nIndex = (int)m_Answer_List.GetFirstSelectedItemPosition();
	int iCount = 1;
	for ( ; iter!=iter_end; ++iter )
	{
		if( nIndex == iCount )
		{
			char szTempChar[512];
			ZeroMemory( szTempChar, 512 );
			m_Answer_Edit.GetWindowText( szTempChar, 512 );

			std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;
			spNpcTalk->SetTalk( szTempChar );
		}
		iCount++;
	}

	UpdateAnswerList( m_dwSelectCase );
}

void CSimpleEdit::SetNPCData(std::tr1::shared_ptr<CNpcDialogue> spNpcDialogue)
{
	m_spNpcDialogue = spNpcDialogue;

	// 현재 선택된 Basic Npc 대화에 ID를 출력한다.
	char szTempChar[16];
	sprintf( szTempChar, "%d", m_spNpcDialogue->GetDlgNID() );
	m_Basic_Edit.SetWindowText( szTempChar );

	// Basic Converstation의 0번째를 출력한다.
	m_BasicConv_Edit.SetWindowText( m_spNpcDialogue->GetDlgCase( 0 )->GetBasicTalk().c_str() );

	// Text Of Converstation의 갯수를 구하고 갯수만큼 라디오 박스를 보이게 한다.
	//int iNum = (int)m_pNpcDialogue->GetDlgCaseVec().size() + IDC_TEXTID_RADIO1;

	//CheckRadioButton( IDC_TEXTID_RADIO1, iNum, IDC_TEXTID_RADIO1 );
	//int i;
	//for( i = IDC_TEXTID_RADIO1; i <= IDC_TEXTID_RADIO11; i++ )
	//{
	//	CButton* pRadioButton = (CButton*)GetDlgItem(i);
	//	if( i < iNum )
	//	{
	//		pRadioButton->ShowWindow( SW_SHOW );
	//	}else{
	//		pRadioButton->ShowWindow( SW_HIDE );
	//	}
	//}
	m_OldTextGetPos = -1;

	m_Text_Slider.SetRange( 0, (int)m_spNpcDialogue->GetDlgCaseVec().size() - 1 ); 
	m_Text_Edit.SetWindowText( m_spNpcDialogue->GetDlgCase( 0 )->GetBasicTalk().c_str() );
	m_Text_Slider.SetPos( 0 );

	// 대답 리스트에 대답들을 넣는다.
	UpdateAnswerList( 0 );

//	OnNMCustomdrawTextSlider( NULL, NULL );
	


	
}


BEGIN_MESSAGE_MAP(CSimpleEdit, CDialog)


	ON_NOTIFY(NM_CUSTOMDRAW, IDC_BASIC_SLIDER, OnNMCustomdrawBasicSlider)
	ON_NOTIFY(NM_CLICK, IDC_ANSWER_LIST, OnNMClickAnswerList)
//	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnBnClickedApplyButton)
	ON_BN_CLICKED(IDC_ANSWERAPPLY_BUTTON, OnBnClickedAnswerapplyButton)
	ON_BN_CLICKED(IDC_TEXTAPPLY_BUTTON, OnBnClickedTextapplyButton)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TEXT_SLIDER, OnNMCustomdrawTextSlider)
END_MESSAGE_MAP()






void CSimpleEdit::OnNMCustomdrawBasicSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if( m_spDialogueSet == NULL ) return;

	if( m_OldGetPos == m_Basic_Slider.GetPos() ) return;



	m_OldGetPos = m_Basic_Slider.GetPos();
	
	
	
	CNpcDialogueSet::DIALOGMAP* pDialogueMap = m_spDialogueSet->GetDialogueMap();
	CNpcDialogueSet::DIALOGMAP_ITER found;

	BYTE count = 0;
	for( found = pDialogueMap->begin(); found != pDialogueMap->end(); ++found )
	{
		if( count == m_Basic_Slider.GetPos() )
		{

			SetNPCData( (*found).second );
			break;
		}
		count++;
	}

}



void CSimpleEdit::OnNMCustomdrawTextSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if( m_OldTextGetPos == m_Text_Slider.GetPos() )
        return;

	if ( !m_spNpcDialogue )
        return;


	m_OldTextGetPos = m_Text_Slider.GetPos();

	m_Text_Edit.SetWindowText( m_spNpcDialogue->GetDlgCase( m_Text_Slider.GetPos() )->GetBasicTalk().c_str() );

	char szTempChar[8];
	ZeroMemory( szTempChar, 8 );
	sprintf( szTempChar, "%d", m_Text_Slider.GetPos() );

	m_TextId_Edit.SetWindowText( szTempChar );

	// 대답 리스트에 대답들을 넣는다.
	UpdateAnswerList( m_Text_Slider.GetPos() );
}



void CSimpleEdit::OnNMClickAnswerList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int nIndex = (int)m_Answer_List.GetFirstSelectedItemPosition();
	UpdateAnswerData( nIndex );


}

//void CSimpleEdit::OnBnClickedApplyButton()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	ApplyNPCData();
//}

void CSimpleEdit::OnBnClickedAnswerapplyButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ApplyAnswerData();
}

void CSimpleEdit::OnBnClickedTextapplyButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ApplyNPCData();
}
