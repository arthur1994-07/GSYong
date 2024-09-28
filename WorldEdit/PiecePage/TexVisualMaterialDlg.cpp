// PiecePage/TexVisualMaterialDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../VisualMaterialLib/Manager/VisualMaterialFX.h"
#include "../VisualMaterialLib/Manager/DxVMManager.h"

#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/DxTools/TextureManager.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"

#include "TexVisualMaterialDlg.h"


// CTexVisualMaterialDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTexVisualMaterialDlg, CDialog)

CTexVisualMaterialDlg::CTexVisualMaterialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTexVisualMaterialDlg::IDD, pParent)
    , m_dwUsedTexture(0)
{

}

CTexVisualMaterialDlg::~CTexVisualMaterialDlg()
{
}

void CTexVisualMaterialDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_PARAMETER, m_ctrlParameter);
}


BEGIN_MESSAGE_MAP(CTexVisualMaterialDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_VM_FILE, &CTexVisualMaterialDlg::OnBnClickedButtonVmFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_0, &CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture0)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_1, &CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture1)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_2, &CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture2)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_3, &CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture3)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_4, &CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture4)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_5, &CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture5)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_6, &CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture6)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_0, &CTexVisualMaterialDlg::OnBnClickedButtonResetTexture0)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_1, &CTexVisualMaterialDlg::OnBnClickedButtonResetTexture1)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_2, &CTexVisualMaterialDlg::OnBnClickedButtonResetTexture2)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_3, &CTexVisualMaterialDlg::OnBnClickedButtonResetTexture3)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_4, &CTexVisualMaterialDlg::OnBnClickedButtonResetTexture4)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_5, &CTexVisualMaterialDlg::OnBnClickedButtonResetTexture5)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_6, &CTexVisualMaterialDlg::OnBnClickedButtonResetTexture6)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PARAMETER, &CTexVisualMaterialDlg::OnLvnItemchangedListParameter)
    ON_BN_CLICKED(ID_BUTTON_PARAMETER_INSERT, &CTexVisualMaterialDlg::OnBnClickedButtonParameterInsert)
    ON_BN_CLICKED(ID_BUTTON_PARAMETER_MODIFY, &CTexVisualMaterialDlg::OnBnClickedButtonParameterModify)
    ON_BN_CLICKED(ID_BUTTON_PARAMETER_DELETE, &CTexVisualMaterialDlg::OnBnClickedButtonParameterDelete)
    ON_BN_CLICKED(IDC_CHECK_NIGHT, &CTexVisualMaterialDlg::OnBnClickedCheckNight)
END_MESSAGE_MAP()


// CTexVisualMaterialDlg �޽��� ó�����Դϴ�.

BOOL CTexVisualMaterialDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // IDC ����
    DWORD dwStatic[] = { IDC_STATIC_0, IDC_STATIC_1, IDC_STATIC_2, IDC_STATIC_3, IDC_STATIC_4, IDC_STATIC_5, IDC_STATIC_6 };
    DWORD dwSrcTexture[] = { IDC_EDIT_SRC_TEXTURE_0, IDC_EDIT_SRC_TEXTURE_1, IDC_EDIT_SRC_TEXTURE_2, IDC_EDIT_SRC_TEXTURE_3,
                                IDC_EDIT_SRC_TEXTURE_4, IDC_EDIT_SRC_TEXTURE_5, IDC_EDIT_SRC_TEXTURE_6 };
    DWORD dwDestTexture[] = { IDC_EDIT_DEST_TEXTURE_0, IDC_EDIT_DEST_TEXTURE_1, IDC_EDIT_DEST_TEXTURE_2, IDC_EDIT_DEST_TEXTURE_3,
                                IDC_EDIT_DEST_TEXTURE_4, IDC_EDIT_DEST_TEXTURE_5, IDC_EDIT_DEST_TEXTURE_6 };
    DWORD dwOpenTexture[] = { IDC_BUTTON_OPEN_TEXTURE_0, IDC_BUTTON_OPEN_TEXTURE_1, IDC_BUTTON_OPEN_TEXTURE_2, IDC_BUTTON_OPEN_TEXTURE_3, 
                                IDC_BUTTON_OPEN_TEXTURE_4, IDC_BUTTON_OPEN_TEXTURE_5, IDC_BUTTON_OPEN_TEXTURE_6 };
    DWORD dwResetTexture[] = { IDC_BUTTON_RESET_TEXTURE_0, IDC_BUTTON_RESET_TEXTURE_1, IDC_BUTTON_RESET_TEXTURE_2, IDC_BUTTON_RESET_TEXTURE_3, 
                                IDC_BUTTON_RESET_TEXTURE_4, IDC_BUTTON_RESET_TEXTURE_5, IDC_BUTTON_RESET_TEXTURE_6 };

    for ( DWORD i=0; i<MAX_USE_TEXTURE_VM; ++i )
    {
        m_dwStatic[i] = dwStatic[i];
        m_dwSrcTexture[i] = dwSrcTexture[i];
        m_dwDestTexture[i] = dwDestTexture[i];
        m_dwOpenTexture[i] = dwOpenTexture[i];
        m_dwResetTexture[i] = dwResetTexture[i];
    }

    RECT listRect;
	m_ctrlParameter.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[2] = { int(nListWidth*1.05f), int(nListWidth*0.55f) };

	m_ctrlParameter.SetExtendedStyle ( m_ctrlParameter.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_ctrlParameter.InsertColumn ( 0, _T("Parameter Name"),	LVCFMT_CENTER, nColWidth[0] );

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	GetDlgItem( IDC_EDIT_VM_FILE )->SetWindowText( m_strFX.c_str() );

    CWnd* pWnd(NULL);
    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X );
    pWnd->SetWindowText( _T("1.0") );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y );
    pWnd->SetWindowText( _T("1.0") );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z );
    pWnd->SetWindowText( _T("1.0") );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X2 );
    pWnd->SetWindowText( _T("1.0") );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y2 );
    pWnd->SetWindowText( _T("1.0") );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z2 );
    pWnd->SetWindowText( _T("1.0") );

    ShowWindowDlg();

    // Dialog ����
    UpdateLisCtrl( -1 );

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CTexVisualMaterialDlg::ShowWindowDlg()
{
    BOOL bShowWindow(FALSE);
    for ( DWORD i=0; i<MAX_USE_TEXTURE_VM; ++i )
    {
        if ( i < m_dwUsedTexture )
            bShowWindow = TRUE;
        else
            bShowWindow = FALSE;

        GetDlgItem( m_dwStatic[i] )->ShowWindow( bShowWindow );
        GetDlgItem( m_dwSrcTexture[i] )->ShowWindow( bShowWindow );
        GetDlgItem( m_dwDestTexture[i] )->ShowWindow( bShowWindow );
        GetDlgItem( m_dwOpenTexture[i] )->ShowWindow( bShowWindow );
        GetDlgItem( m_dwResetTexture[i] )->ShowWindow( bShowWindow );

        GetDlgItem( m_dwSrcTexture[i] )->SetWindowText( m_strTextureSRC[i].c_str() );
        GetDlgItem( m_dwDestTexture[i] )->SetWindowText( m_strTextureDEST[i].c_str() );
    }
}

void CTexVisualMaterialDlg::OnBnClickedButtonVmFile()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CString szFilter = _T("Visual Material ( *.vm ) |*.vm");
	CFileDialog dlg( TRUE, ".vm", NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
    CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();
	dlg.m_ofn.lpstrInitialDir = pPath->FullPathVisualMaterial().c_str(); // SUBPATH::FULL_PATH_VISUAL_MATERIAL;

	if( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem( IDC_EDIT_VM_FILE );
		pWnd->SetWindowText( FilePath );

		m_strFX = FilePath.GetString();

        // �ʱ�ȭ
        for ( DWORD i=0; i<MAX_USE_TEXTURE_VM; ++i )
        {
            m_strTextureSRC[i] = _T("");
            m_strTextureDEST[i] = _T("");
        }

        // �� ����
        std::tr1::shared_ptr<VisualMaterialFX> spVMFX = NS_VM_MANAGER::Load( pView->GetD3DDevice(), m_strFX, NS_VM_MANAGER::EMRT_OBJECT );

        for ( DWORD i=0; i<spVMFX->GetTextureSize(); ++i )
        {
            if ( spVMFX->GetTextureName( i ) )
            {
                m_strTextureSRC[i] = spVMFX->GetTextureName( i );
                m_strTextureDEST[i] = spVMFX->GetTextureName( i );
            }
            else
            {
                m_strTextureSRC[i] = _T("");
                m_strTextureDEST[i] = _T("");
            }
        }

        // ShowWindowDlg
        m_dwUsedTexture = spVMFX->GetTextureSize();
        ShowWindowDlg();

        UpdateLisCtrl( -1 );
	}
}

void CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture0()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 0 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture1()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 1 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture2()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 2 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture3()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 3 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture4()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 4 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture5()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 5 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture6()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 6 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonResetTexture0()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 0 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonResetTexture1()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 1 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonResetTexture2()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 2 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonResetTexture3()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 3 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonResetTexture4()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 4 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonResetTexture5()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 5 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonResetTexture6()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 6 );
}

void CTexVisualMaterialDlg::OnBnClickedButtonOpenTexture( DWORD i )
{
    CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg( TRUE, ".BMP", NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if( dlg.DoModal() == IDOK )
	{
		CWnd *pWnd;
		pWnd = GetDlgItem( m_dwDestTexture[i] );
		pWnd->SetWindowText( dlg.GetFileName() );

		m_strTextureDEST[i] = dlg.GetFileName();
	}
}

void CTexVisualMaterialDlg::OnBnClickedButtonResetTexture( DWORD i )
{
    m_strTextureDEST[i] = m_strTextureSRC[i];

    CWnd *pWnd;
    pWnd = GetDlgItem( m_dwDestTexture[i] );
    pWnd->SetWindowText( m_strTextureDEST[i].c_str() );
}

void CTexVisualMaterialDlg::OnLvnItemchangedListParameter(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    *pResult = 0;

    POSITION pos = m_ctrlParameter.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		return;
	}

	int nSize = m_ctrlParameter.GetSelectedCount();
	if( nSize==0 )	return;

    int nItem = m_ctrlParameter.GetNextSelectedItem( pos );

    MAP_PARAMETER_NAME_DATA2_CITER iter = m_mapParameter.begin();
    for ( int i=0; iter!=m_mapParameter.end(); ++iter, ++i )
    {
        if ( i == nItem )
        {
            CString Str;
            CWnd* pWnd(NULL);
            CButton* pButton(NULL);
            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_NAME );
            pWnd->SetWindowText( (*iter).first.c_str() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X );
            Str.Format ( "%3.2f", (*iter).second.m_vParameter.x );
            pWnd->SetWindowText( Str.GetString() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y );
            Str.Format ( "%3.2f", (*iter).second.m_vParameter.y );
            pWnd->SetWindowText( Str.GetString() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z );
            Str.Format ( "%3.2f", (*iter).second.m_vParameter.z );
            pWnd->SetWindowText( Str.GetString() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X2 );
            Str.Format ( "%3.2f", (*iter).second.m_vNight.x );
            pWnd->SetWindowText( Str.GetString() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y2 );
            Str.Format ( "%3.2f", (*iter).second.m_vNight.y );
            pWnd->SetWindowText( Str.GetString() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z2 );
            Str.Format ( "%3.2f", (*iter).second.m_vNight.z );
            pWnd->SetWindowText( Str.GetString() );

            pButton = (CButton*)GetDlgItem( IDC_CHECK_NIGHT );
            if ( (*iter).second.m_bNight )
            {
                pButton->SetCheck( TRUE );
            }
            else
            {
                pButton->SetCheck( FALSE );
            }
        }
    }    
}

void CTexVisualMaterialDlg::OnBnClickedButtonParameterInsert()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    
    // ParameterName �� ������ �۾��� �� ����.
    {
        CString Str;
        CWnd* pWnd(NULL);
        pWnd = GetDlgItem( IDC_EDIT_PARAMETER_NAME );
        pWnd->GetWindowText( Str );

        // V547 Expression 'Str.GetString() == ""' is always false. To compare strings you should use strcmp() function.
        if (Str.IsEmpty())
        {
            // �۾��� �� ����.
           return;
        }
    }

    // Dialog ���� Data�� �����´�.
    TSTRING strParameterName;
    ParameterData sData;
    GetPARAMETER_NAME_DATA_IntoDialog( strParameterName, sData );

    // �߰�
    m_mapParameter.insert( std::make_pair( strParameterName, sData ) );

    // Dialog ����
    DWORD dwCount(0);
    MAP_PARAMETER_NAME_DATA2_CITER iter = m_mapParameter.begin();
    for ( ; iter!=m_mapParameter.end(); ++iter, ++dwCount )
    {
        if ( (*iter).first == strParameterName )
            break;        
    }
    UpdateLisCtrl( dwCount );
}

void CTexVisualMaterialDlg::OnBnClickedButtonParameterModify()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

    // ParameterName �� ������ �۾��� �� ����.
    {
        CString Str;
        CWnd* pWnd(NULL);
        pWnd = GetDlgItem( IDC_EDIT_PARAMETER_NAME );
        pWnd->GetWindowText( Str );

        // always false
        if (Str.IsEmpty())
        {
            // �۾��� �� ����.
            return;
        }
    }

    POSITION pos = m_ctrlParameter.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		return;
	}

	int nSize = m_ctrlParameter.GetSelectedCount();
	if( nSize==0 )	return;

    int nItem = m_ctrlParameter.GetNextSelectedItem( pos );

    MAP_PARAMETER_NAME_DATA2_ITER iter = m_mapParameter.begin();
    for ( int i=0; iter!=m_mapParameter.end(); ++iter, ++i )
    {
        if ( i == nItem )
        {
            m_mapParameter.erase( iter );

            // Dialog ���� Data�� �����´�.
            TSTRING strParameterName;
            ParameterData sData;
            GetPARAMETER_NAME_DATA_IntoDialog( strParameterName, sData );

            // �߰�
            m_mapParameter.insert( std::make_pair( strParameterName, sData ) );

            // Dialog ����
            DWORD dwCount(0);
            MAP_PARAMETER_NAME_DATA2_CITER iter2 = m_mapParameter.begin();
            for ( ; iter2!=m_mapParameter.end(); ++iter2, ++dwCount )
            {
                if ( (*iter2).first == strParameterName )
                    break;        
            }
            UpdateLisCtrl( dwCount );

            return;
        }
    }
}

void CTexVisualMaterialDlg::OnBnClickedButtonParameterDelete()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

    POSITION pos = m_ctrlParameter.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		return;
	}

	int nSize = m_ctrlParameter.GetSelectedCount();
	if( nSize==0 )	return;

    int nItem = m_ctrlParameter.GetNextSelectedItem( pos );

    MAP_PARAMETER_NAME_DATA2_ITER iter = m_mapParameter.begin();
    for ( int i=0; iter!=m_mapParameter.end(); ++iter, ++i )
    {
        if ( i == nItem )
        {
            // ����
            m_mapParameter.erase( iter );

            // Dialog ����
            UpdateLisCtrl( m_mapParameter.size()-1 );

            return;
        }
    }
}

void CTexVisualMaterialDlg::OnBnClickedCheckNight()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CTexVisualMaterialDlg::GetPARAMETER_NAME_DATA_IntoDialog( TSTRING& strParameterName, ParameterData& sData )
{
    CString strText;
    CWnd* pWnd(NULL);
    CButton* pButton(NULL);
    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_NAME );
    pWnd->GetWindowText( strText );
    strParameterName = strText.GetString();

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X );
    pWnd->GetWindowText( strText );
    sData.m_vParameter.x = (float)atof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y );
    pWnd->GetWindowText( strText );
    sData.m_vParameter.y = (float)atof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z );
    pWnd->GetWindowText( strText );
    sData.m_vParameter.z = (float)atof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X2 );
    pWnd->GetWindowText( strText );
    sData.m_vNight.x = (float)atof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y2 );
    pWnd->GetWindowText( strText );
    sData.m_vNight.y = (float)atof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z2 );
    pWnd->GetWindowText( strText );
    sData.m_vNight.z = (float)atof ( strText.GetString() );

    pButton = (CButton*)GetDlgItem( IDC_CHECK_NIGHT );
    if ( pButton->GetCheck() )
    {
        sData.m_bNight = TRUE;
    }
    else
    {
        sData.m_bNight = FALSE;
    }
}

// bSelectLastItem - TRUE - �� ���������� �����ϵ��� �Ѵ�. / FALSE - ������ ������ ���� �����ϵ��� �Ѵ�.
// bSelectItem - �������� �����ϰ� �� ������ �� �� ������ �����Ѵ�.
void CTexVisualMaterialDlg::UpdateLisCtrl( int nSelectIndex )
{
    // ���ù����� ������ ���� ���
    if ( nSelectIndex >= m_ctrlParameter.GetItemCount() )
    {
        nSelectIndex = -1;
    }

    // ��� ����
	m_ctrlParameter.DeleteAllItems();

    // �ٽ� ���
    MAP_PARAMETER_NAME_DATA2_CITER iter = m_mapParameter.begin();
    for ( int i=0; iter!=m_mapParameter.end(); ++iter, ++i )
    {
        m_ctrlParameter.InsertItem( i, (*iter).first.c_str() );
    }

    // ���õ� ���� ���� ���� �۵���Ų��.
    if ( nSelectIndex != -1 )
    {
        m_ctrlParameter.SetSelectionMark( nSelectIndex );							// ���� ���õ�.
	    m_ctrlParameter.SetItemState( nSelectIndex, LVIS_SELECTED, LVIS_SELECTED);	// ȸ������ ǥ�õ�.
	    m_ctrlParameter.EnsureVisible( nSelectIndex, FALSE );						// ��ũ�� �ؿ� ������ �Ű���.
	    m_ctrlParameter.SetFocus();												    // Ǫ�������� ǥ�õ�.
    }
}