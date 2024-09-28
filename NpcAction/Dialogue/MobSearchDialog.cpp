
#include "stdafx.h"

#include "../NpcAction.h"
#include "./MobSearchDialog.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/GLStringTable.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CMobSearchDialog, CDialog)

CMobSearchDialog::CMobSearchDialog ( CWnd* pParent )
: CDialog ( CMobSearchDialog::IDD, pParent )
, m_sSummonMobID( SNATIVEID( false ) )
{
}

CMobSearchDialog::~CMobSearchDialog ()
{
}

void CMobSearchDialog::DoDataExchange ( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
}


BEGIN_MESSAGE_MAP(CMobSearchDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
END_MESSAGE_MAP()

void CMobSearchDialog::OnBnClickedButtonCancel()
{
	m_sSummonMobID = SNATIVEID( false );

	OnCancel();
}

void CMobSearchDialog::OnBnClickedButtonOk()
{
	m_bConfirm = TRUE;

	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_MOB );
	int nIndex = pListBox->GetCurSel ();
	if ( nIndex == LB_ERR )
		return;

	m_sSummonMobID.dwID = (DWORD) pListBox->GetItemData ( nIndex );

	OnOK();
}

BOOL CMobSearchDialog::OnInitDialog ()
{
	CDialog::OnInitDialog();

	m_bConfirm = FALSE;	

	//	현재 밖으로 드러난 아이템들 모두 로드하기
	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_MOB );
	for ( WORD i=0; i<GLCrowDataMan::MAX_CROW_MID; ++i )
	{
		for ( WORD j=0; j<GLCrowDataMan::MAX_CROW_SID; ++j )
		{
			PCROWDATA pCrowData =
				GLCrowDataMan::GetInstance().GetCrowData ( i, j );
			if ( pCrowData )
			{
				CString strCrowName = GetFormatName ( pCrowData );
				int nIndex = pListBox->AddString ( strCrowName.GetString() );
				pListBox->SetItemData ( nIndex, (DWORD_PTR)pCrowData->GetId().dwID );
			}
		}
	}

	return TRUE;
}

const SNATIVEID& CMobSearchDialog::GetSummonMobID ()
{
	assert ( m_bConfirm && "쓰레기 값이 넘어갑니다." );

	return m_sSummonMobID;
}

const char* CMobSearchDialog::GetFormatName ( PCROWDATA pItem )
{
	GASSERT ( pItem );

	static	CString strItem;
	strItem.Format ( "[%04d/%02d] %s",
		pItem->sNativeID.wMainID,
		pItem->sNativeID.wSubID,
		pItem->GetName() );

	CString strNewName;
	strNewName.Format( "CN_%03d_%03d",
		pItem->sNativeID.wMainID,
		pItem->sNativeID.wSubID );
	StringCchCopy( pItem->m_szName, CHAR_SZNAME, strNewName );

	return strItem.GetString ();
}