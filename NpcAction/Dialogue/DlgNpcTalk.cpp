// DlgNpcTalk.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../NpcAction.h"
#include "../Dialogue/DlgNpcTalk.h"
#include "../../RanLogic/NpcTalk/NpcDialogue.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgNpcTalk ��ȭ �����Դϴ�.

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


// CDlgNpcTalk �޽��� ó�����Դϴ�.

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

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgNpcTalk::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	int nIndex = m_listNpcTalk.GetCurSel();		

	if ( nIndex > 0 || nIndex < m_listNpcTalk.GetCount() )
	{
		m_dwNpcID = m_listNpcTalk.GetItemData( nIndex );
	}

	CDialog::OnOK();
}

void CDlgNpcTalk::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDialog::OnCancel();
}
