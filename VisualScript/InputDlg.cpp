#include "stdafx.h"

#include "./InputDlg.h"

#include "../SigmaCore/DebugInclude.h"

IMPLEMENT_DYNAMIC( CInputDlg, CDialog )

CInputDlg::CInputDlg( CWnd* pParent )
    : CDialog( IDD_INPUT_DLG, pParent )
{
}

VOID CInputDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );

    DDX_Text( pDX, IDC_INPUT_DLG_COMMENT, m_strComment );
    DDX_Text( pDX, IDC_INPUT_DLG_EDIT, m_strValue );
}

BOOL CInputDlg::OnInitDialog()
{
    SetWindowText( m_strTitle.GetString() );

    return CDialog::OnInitDialog();
}