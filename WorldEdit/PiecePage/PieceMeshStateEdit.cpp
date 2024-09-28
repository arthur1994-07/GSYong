// PiecePage/PieceMeshStateEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../WorldEdit.h"
#include "../sheetWithTab.h"

#include "../../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"

#include "PieceMeshStateEdit.h"


// CPieceMeshStateEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPieceMeshStateEdit, CPropertyPage)

CPieceMeshStateEdit::CPieceMeshStateEdit()
	: CPropertyPage(CPieceMeshStateEdit::IDD)
	, m_pDxFrame(NULL)
{

}

CPieceMeshStateEdit::~CPieceMeshStateEdit()
{
}

void CPieceMeshStateEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPieceMeshStateEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CPieceMeshStateEdit::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CPieceMeshStateEdit::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_LIGHTMAP, &CPieceMeshStateEdit::OnBnClickedCheckEnableLightmap)
END_MESSAGE_MAP()


// CPieceMeshStateEdit 메시지 처리기입니다.

BOOL CPieceMeshStateEdit::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CPropertyPage::OnSetActive();
}

void CPieceMeshStateEdit::SetInstance ( DxFrame *pDxFrame )
{
	m_pDxFrame = pDxFrame;

	if ( !pDxFrame )
		return;

	CWnd *pWnd;
	if ( pDxFrame->szName )
	{
		pWnd = GetDlgItem ( IDC_EDIT_FRAMENAME );
		pWnd->SetWindowText ( pDxFrame->szName );
	}

	if ( !pDxFrame->pmsMeshs )
		return;

	CString Str;

	CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_ENABLE_LIGHTMAP ) );
	pButton->SetCheck( pDxFrame->pmsMeshs->m_bEnableLightMap );

	pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_UV_INDEX );
	Str.Format ( _T("%d"), pDxFrame->pmsMeshs->m_dwLightMapIndex );
	pWnd->SetWindowText ( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_LM_TEXTURE_SIZE_X );
	Str.Format( _T("%d"), pDxFrame->pmsMeshs->m_wwLightMapUserUV_TextureSize.wA );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_LM_TEXTURE_SIZE_Y );
	Str.Format( _T("%d"), pDxFrame->pmsMeshs->m_wwLightMapUserUV_TextureSize.wB );
	pWnd->SetWindowText( Str.GetString() );
}

void CPieceMeshStateEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !m_pDxFrame )
		return;

	CString strText;
	CWnd *pWnd;

	pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_UV_INDEX );
	pWnd->GetWindowText( strText );
	m_pDxFrame->pmsMeshs->m_dwLightMapIndex = _tstoi( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_LM_TEXTURE_SIZE_X );
	pWnd->GetWindowText( strText );
	m_pDxFrame->pmsMeshs->m_wwLightMapUserUV_TextureSize.wA = _tstoi( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_LM_TEXTURE_SIZE_Y );
	pWnd->GetWindowText( strText );
	m_pDxFrame->pmsMeshs->m_wwLightMapUserUV_TextureSize.wB = _tstoi( strText.GetString() );
}


void CPieceMeshStateEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

	m_pSheetTab->SetActiveBackUpPage( PIECEMESHSTATEPAGEINDEX );
}

void CPieceMeshStateEdit::OnBnClickedCheckEnableLightmap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !m_pDxFrame )
		return;

	CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_ENABLE_LIGHTMAP ) );
	m_pDxFrame->pmsMeshs->m_bEnableLightMap = pButton->GetCheck() ? TRUE : FALSE;
}
