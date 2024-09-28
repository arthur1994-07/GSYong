// DlgNpcTalk.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../NpcAction.h"
#include "../Dialogue/DlgNpcTalk.h"
#include "../../RanLogic/NpcTalk/NpcDialogue.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgNpcTalk 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgNpcTalk, CDialog)
CDlgNpcTalk::CDlgNpcTalk( CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNpcTalk::IDD, pParent)
	, m_dwNpcID( UINT_MAX )
{
}

CDlgNpcTalk::CDlgNpcTalk(spNpcDialogSet pNpcDialogueSet, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNpcTalk::IDD, pParent)
	, m_dwNpcID( UINT_MAX )
	, m_spNpcDialogueSet ( pNpcDialogueSet ) 
{
}

CDlgNpcTalk::~CDlgNpcTalk()
{
}

void CDlgNpcTalk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NPCTALK_LIST, m_listNpcTalk);
}


BEGIN_MESSAGE_MAP(CDlgNpcTalk, CDialog)
END_MESSAGE_MAP()


// CDlgNpcTalk 메시지 처리기입니다.

BOOL CDlgNpcTalk::OnInitDialog()
{
	CDialog::OnInitDialog();

	if ( m_spNpcDialogueSet )
	{
		CNpcDialogueSet::DIALOGMAP* pMap = m_spNpcDialogueSet->GetDialogueMap ();
	
		CNpcDialogueSet::DIALOGMAP_ITER iter = pMap->begin();
		CNpcDialogueSet::DIALOGMAP_ITER iter_end = pMap->end();		

        CString strTemp;

		for ( int nIndex = 0; iter!=iter_end; ++iter, nIndex++ )
		{
            std::tr1::shared_ptr<CNpcDialogue> spDialogue = iter->second;

			DWORD dwIndex = spDialogue->FindNonCondition();
			if ( dwIndex==UINT_MAX )	dwIndex = 0;

			std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = spDialogue->GetDlgCase(dwIndex);
			if ( !spDlgCase )
			{
				
				pMap->erase( iter++ );
				--nIndex;
				continue;			
			}

			strTemp.Format ( "[%3d] %s", spDialogue->GetDlgNID(), spDlgCase->GetBasicTalk().c_str() );
			
			int nItem = m_listNpcTalk.AddString( strTemp );
			m_listNpcTalk.SetItemData ( nItem, spDialogue->GetDlgNID() );
		}

	}

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgNpcTalk::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	int nIndex = m_listNpcTalk.GetCurSel();		

	if ( nIndex > 0 || nIndex < m_listNpcTalk.GetCount() )
	{
		m_dwNpcID = m_listNpcTalk.GetItemData( nIndex );
	}

	CDialog::OnOK();
}

void CDlgNpcTalk::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::OnCancel();
}
