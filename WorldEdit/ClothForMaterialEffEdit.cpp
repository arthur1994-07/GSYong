// ClothEffEdit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/DxEffect/Frame/DxEffectClothForMaterial.h"

#include "ClothForMaterialEffEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern std::string			g_strPICK_TEXTURE;
extern DWORD				g_dwCLOSED_POINT_INDEX;
extern D3DXVECTOR3			g_vCLOSED_POINT;

// CClothForMaterialEffEdit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CClothForMaterialEffEdit, CPropertyPage)
CClothForMaterialEffEdit::CClothForMaterialEffEdit()
	: CPropertyPage(CClothForMaterialEffEdit::IDD),
	m_pClothEff(NULL),
	m_bInitDlg(FALSE)
{
}

CClothForMaterialEffEdit::~CClothForMaterialEffEdit()
{
}

void CClothForMaterialEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_POINT, m_ListPoint);
}


BEGIN_MESSAGE_MAP(CClothForMaterialEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_RADIO_STOP, OnBnClickedRadioStop)
	ON_BN_CLICKED(IDC_RADIO_PLAY, OnBnClickedRadioPlay)
	ON_BN_CLICKED(IDC_BUTTON_POINTSELECT, OnBnClickedButtonPointselect)
	ON_BN_CLICKED(IDC_BUTTON_POINTDELETE, OnBnClickedButtonPointdelete)	
	ON_BN_CLICKED(IDC_BUTTON_WINDDIR, OnBnClickedButtonWinddir)
	ON_BN_CLICKED(IDC_CHECK_EACH_RENDER, OnBnClickedCheckEachRender)
END_MESSAGE_MAP()


// CClothForMaterialEffEdit �޽��� ó�����Դϴ�.
void CClothForMaterialEffEdit::SetInstance ( DxEffectClothForMaterial *pClothEff )
{
	m_pClothEff = pClothEff;
	m_OldClothProperty = m_ClothProperty = m_pClothEff->GetProperty();

	//	Note : �� ����.
	//
	CButton* pButton;
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pClothEff->GetName() );

	//	Note : ��� �׸� �����ֱ�
	//
	m_ListPoint.ResetContent ();

	//	Note : ���� �Ǿ��� �׸� ��Ÿ����
	//
	for ( DWORD i=0; i<m_pClothEff->GetdwNumLockIndex(); i++ )
	{
		char *szText = m_pClothEff->GetStrList ( i );

		int nIndex = m_ListPoint.AddString ( szText );
		m_ListPoint.SetItemData ( nIndex, m_pClothEff->GetdwVertex(i) );
	}

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_ClothProperty.m_szTexture );

	if ( m_ClothProperty.m_bPlay )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_PLAY );
		pButton->SetCheck ( 1 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_STOP );
		pButton->SetCheck ( 0 );
	}
	else
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_PLAY );
		pButton->SetCheck ( 0 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_STOP );
		pButton->SetCheck ( 1 );
	}

	if ( m_ClothProperty.m_bEachRender )
	{
		pButton = (CButton*)GetDlgItem ( IDC_CHECK_EACH_RENDER );
		pButton->SetCheck( TRUE );
	}
	else
	{
		pButton = (CButton*)GetDlgItem ( IDC_CHECK_EACH_RENDER );
		pButton->SetCheck( FALSE );
	}

	char szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_COEFFICIENT );
	_gcvt ( m_ClothProperty.m_fCoEfficient, 7, szValue );
	pWnd->SetWindowText ( szValue );	

	pWnd = GetDlgItem ( IDC_EDIT_WIND_FORCE );
	_gcvt ( m_ClothProperty.m_fWindForce, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_GRAVITY_FORCE );
	_gcvt ( m_ClothProperty.m_fGravity, 7, szValue );
	pWnd->SetWindowText ( szValue );

	_gcvt ( m_ClothProperty.m_vWindDirection.x, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRX)->SetWindowText(szValue);
	_gcvt ( m_ClothProperty.m_vWindDirection.y, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRY)->SetWindowText(szValue);
	_gcvt ( m_ClothProperty.m_vWindDirection.z, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRZ)->SetWindowText(szValue);

	m_bInitDlg = TRUE;
}

void CClothForMaterialEffEdit::UpdatePage()
{
	if( !m_bInitDlg )		return;

	GetDlgItem( IDC_EDIT_CLICK_TEX )->SetWindowText( g_strPICK_TEXTURE.c_str() );
}

void CClothForMaterialEffEdit::OnBnClickedButtonTexture()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CString StrInitDir = pFrame->m_strAppPath + "\\Texture";
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText ( FilePath );

//		char *szTexture = FilePath.GetString (256);
//		m_pAnisoEff->SetTexture ( szTexture );
	}
}

void CClothForMaterialEffEdit::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_pClothEff )	return;

	CWnd *pWnd;

	CString strTexture;
	float fTemp;

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	StringCchCopy( m_ClothProperty.m_szTexture, MAX_PATH, strTexture.GetString() );
	
	CString strText;

	pWnd = GetDlgItem ( IDC_EDIT_COEFFICIENT );
	pWnd->GetWindowText ( strText );
	m_ClothProperty.m_fCoEfficient = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_WIND_FORCE );
	pWnd->GetWindowText ( strText );
	fTemp = (float)atof ( strText.GetString() );
	if( fTemp > 5.0f )
		m_ClothProperty.m_fWindForce = 5.0f;
	else
		m_ClothProperty.m_fWindForce = fTemp;

	pWnd = GetDlgItem ( IDC_EDIT_GRAVITY_FORCE );
	pWnd->GetWindowText ( strText );
	m_ClothProperty.m_fGravity = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DIRX );
	pWnd->GetWindowText ( strText );
	m_ClothProperty.m_vWindDirection.x = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DIRY );
	pWnd->GetWindowText ( strText );
	m_ClothProperty.m_vWindDirection.y = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DIRZ );
	pWnd->GetWindowText ( strText );
	m_ClothProperty.m_vWindDirection.z = (float)atof ( strText.GetString() );

	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_PLAY );
	pButton->SetCheck ( 1 );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_STOP );
	pButton->SetCheck ( 0 );


	//	Note : ������ ������ ��ü�� ���� ��Ű�� ���̵� ��ư�� PLAY ����.
	//
	m_ClothProperty.m_bPlay = true;

	//// m_pLockIndex ���� ����.
	//m_pClothEff->SetIndexLock();

	// m_pLockIndex ���� ����.
	DWORD dwTempNumLockIndex = 0L;
	WORD* pTempLockIndex = NULL;
	if( m_pClothEff->m_pLockIndex )
	{
		dwTempNumLockIndex = m_pClothEff->m_dwNumLockIndex;
		pTempLockIndex = new WORD[m_pClothEff->m_dwNumLockIndex];
		memcpy( pTempLockIndex, m_pClothEff->m_pLockIndex, sizeof(WORD)*m_pClothEff->m_dwNumLockIndex );
	}

	m_pClothEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pClothEff->Create ( pView->GetD3DDevice() );

	m_pClothEff->BaseBuffToDynamicBuff ( pView->GetD3DDevice() );

	// m_pLockIndex �����´�.
	if( pTempLockIndex )
	{
		m_pClothEff->m_dwNumLockIndex = dwTempNumLockIndex;
		m_pClothEff->m_pLockIndex = new WORD[m_pClothEff->m_dwNumLockIndex];
		memcpy( m_pClothEff->m_pLockIndex, pTempLockIndex, sizeof(WORD)*m_pClothEff->m_dwNumLockIndex );
	}
	
	
	//// m_pLockIndex �����´�.
	//m_pClothEff->GetIndexLock();

	//	Note : �Ӽ��� ����.
	//
	m_pClothEff->SetProperty ( m_ClothProperty );
}

void CClothForMaterialEffEdit::OnBnClickedButtonOk()
{
	//	Note : �Ӽ��� ����.
	//
	OnBnClickedButtonApply();

	m_pSheetTab->SetActiveBackUpPage( CLOTH4MATERIALINDEX );
	m_pClothEff = NULL;
}

void CClothForMaterialEffEdit::OnBnClickedButtonCancel()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	//	Note : �Ӽ��� �ǵ���.
	//
	m_pClothEff->SetProperty ( m_OldClothProperty );
	m_pClothEff->CleanUp ();
	m_pClothEff->Create ( pView->GetD3DDevice() );

	
	m_pSheetTab->SetActiveBackUpPage( CLOTH4MATERIALINDEX );
	m_pClothEff = NULL;
}

void CClothForMaterialEffEdit::OnBnClickedRadioStop()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	// ���� �������� �ʴ� ���·� �ǵ�����.
	m_ClothProperty.m_bPlay = false;
	m_pClothEff->SetProperty ( m_ClothProperty );
	m_pClothEff->BaseBuffToDynamicBuff ( pView->GetD3DDevice() );
}

void CClothForMaterialEffEdit::OnBnClickedRadioPlay()
{
	m_ClothProperty.m_bPlay = true;
	m_pClothEff->SetProperty ( m_ClothProperty );
}

void CClothForMaterialEffEdit::OnBnClickedButtonPointselect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
	D3DXVECTOR3 vPoint = g_vCLOSED_POINT;
	DWORD dwPointIndex = g_dwCLOSED_POINT_INDEX;

	CString Str;
	Str.Format ( "[%d] %4.2f, %4.2f, %4.2f", dwPointIndex, vPoint.x, vPoint.y, vPoint.z );
	
	if ( m_ListPoint.FindString ( 0, Str ) == LB_ERR )
	{
		int nIndex = m_ListPoint.AddString ( Str );
		m_ListPoint.SetItemData ( nIndex, dwPointIndex );

		//	Note : ������ Point�� "��" ��.
		//
		m_pClothEff->SetParticleLock ( dwPointIndex, TRUE );
	}
}

void CClothForMaterialEffEdit::OnBnClickedButtonPointdelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_ListPoint.GetCurSel ();
	if ( nIndex == -1 )	return;

	DWORD dwData = m_ListPoint.GetItemData ( nIndex );

	//	Note : ������ Point�� "��" �� ������.
	//
	m_pClothEff->SetParticleLock ( dwData, FALSE );

	m_ListPoint.DeleteString ( nIndex );
}


BOOL CClothForMaterialEffEdit::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if ( m_pClothEff )
	{
		char szValue[256];		

		_gcvt ( m_ClothProperty.m_vWindDirection.x, 7, szValue );
		GetDlgItem(IDC_EDIT_DIRX)->SetWindowText(szValue);
		_gcvt ( m_ClothProperty.m_vWindDirection.y, 7, szValue );
		GetDlgItem(IDC_EDIT_DIRY)->SetWindowText(szValue);
		_gcvt ( m_ClothProperty.m_vWindDirection.z, 7, szValue );
		GetDlgItem(IDC_EDIT_DIRZ)->SetWindowText(szValue);

		m_pClothEff->SetProperty ( m_ClothProperty );
	}

	return CPropertyPage::OnSetActive();
}

void CClothForMaterialEffEdit::OnBnClickedButtonWinddir()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) &m_ClothProperty.m_vWindDirection, CLOTH4MATERIALINDEX );
}

void CClothForMaterialEffEdit::OnBnClickedCheckEachRender()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_EACH_RENDER );

	if( pButton->GetCheck() )	m_ClothProperty.m_bEachRender = TRUE;
	else						m_ClothProperty.m_bEachRender = FALSE;

	m_pClothEff->SetProperty( m_ClothProperty );
}


