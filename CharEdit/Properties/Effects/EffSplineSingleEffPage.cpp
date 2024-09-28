// ./Properties/Effects/EffSplineSingleEffPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../../CharEdit.h"

#include "../../MainFrm.h"
#include "../../CharEditDoc.h"
#include "../../CharEditView.h"

#include "../SheetWithTabChar.h"

#include "../../../EngineLib/DxTools/DxInputDevice.h"

#include "EffSplineSingleEffPage.h"


extern std::string	g_strPICK_BONE;

// CEffSplineSingleEffPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffSplineSingleEffPage, CPropertyPage)

CEffSplineSingleEffPage::CEffSplineSingleEffPage()
	: CEffCharPage(CEffSplineSingleEffPage::IDD)
{

}

CEffSplineSingleEffPage::~CEffSplineSingleEffPage()
{
}

void CEffSplineSingleEffPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SPLINE_POINT, m_listSplinePoint);
}


BEGIN_MESSAGE_MAP(CEffSplineSingleEffPage, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON_APPLY, &CEffSplineSingleEffPage::OnBnClickedButtonApply)
    ON_BN_CLICKED(IDOK, &CEffSplineSingleEffPage::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CEffSplineSingleEffPage::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_SPOINT_INSERT, &CEffSplineSingleEffPage::OnBnClickedButtonSpointInsert)
    ON_BN_CLICKED(IDC_BUTTON_SPOINT_DELETE, &CEffSplineSingleEffPage::OnBnClickedButtonSpointDelete)
    ON_BN_CLICKED(IDC_BUTTON_SPOINT_UP, &CEffSplineSingleEffPage::OnBnClickedButtonSpointUp)
    ON_BN_CLICKED(IDC_BUTTON_SPOINT_DOWN, &CEffSplineSingleEffPage::OnBnClickedButtonSpointDown)
    ON_BN_CLICKED(IDC_BUTTON_SELECT, &CEffSplineSingleEffPage::OnBnClickedButtonSelect)
    ON_BN_CLICKED(IDC_BUTTON_GETBONENAME, &CEffSplineSingleEffPage::OnBnClickedButtonGetbonename)
    ON_LBN_SELCHANGE(IDC_LIST_SPLINE_POINT, &CEffSplineSingleEffPage::OnLbnSelchangeListSplinePoint)
END_MESSAGE_MAP()


// CEffSplineSingleEffPage 메시지 처리기입니다.

void CEffSplineSingleEffPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

    if ( pProp )
    {
		m_Property = *((EFFCHAR_PROPERTY_SPLINE_SINGLE_EFF*)pProp);
    }
	else
	{
		m_Property = EFFCHAR_PROPERTY_SPLINE_SINGLE_EFF();
	}

    // Effect가 없다면 설정함.
    if ( !m_pEffChar )
	{
		m_pEffChar = dynamic_cast<DxEffCharSplineSingleEff*>( DxEffCharMan::GetInstance().CreateEffect
			(
			DxEffCharSplineSingleEff::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
			) 
            );

		if ( !m_pEffChar )
		{
			MessageBox ( "이팩트 생성에 실패하였습니다.", "ERROR", MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );
	}

    DxEffCharSplineSingleEff* pEffChar = dynamic_cast<DxEffCharSplineSingleEff*>(m_pEffChar);

    // 내부데이터에 있는 것 Dialog에 적용함.
    {
        CWnd*   pWnd;
        char    szValue[256];

        pWnd = GetDlgItem ( IDC_EDIT_SCALE2 );
	    _gcvt ( m_Property.m_fScale, 7, szValue );
	    pWnd->SetWindowText ( szValue );

	    pWnd = GetDlgItem ( IDC_EDIT_SPEED );
	    _gcvt ( m_Property.m_fSpeed, 7, szValue );
	    pWnd->SetWindowText ( szValue );

        pWnd = GetDlgItem ( IDC_EDIT_EFF_NUMBER );
	    _gcvt ( m_Property.m_dwEffNumber, 7, szValue );
	    pWnd->SetWindowText ( szValue );

        if ( m_pEffChar )
        {
	        pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
	        pWnd->SetWindowText ( pEffChar->m_strSingleEffect.c_str() );

            pWnd = GetDlgItem ( IDC_EDIT_BONE_NAME2 );
	        pWnd->SetWindowText ( pEffChar->m_strBoneName.c_str() );
        }
        else
        {
            pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
            pWnd->SetWindowText ( _T("") );

            pWnd = GetDlgItem ( IDC_EDIT_BONE_NAME2 );
            pWnd->SetWindowText ( _T("") );
        }

        ResetSplinePoint( -1 );
    }

    // Note : 본을 렌더한다고 알려준다.
	pView->SetRenderTypeBone();
}

void CEffSplineSingleEffPage::OnBnClickedButtonApply()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
    DxEffCharSplineSingleEff* pEffChar = dynamic_cast<DxEffCharSplineSingleEff*>(m_pEffChar);

    // Dialog에 기록한것 내부데이터에 적용함.
    {
        CWnd* pWnd;
        CString strText;

        pWnd = GetDlgItem ( IDC_EDIT_SCALE2 );
	    pWnd->GetWindowText ( strText );
	    m_Property.m_fScale = (float)atof ( strText.GetString() );

        pWnd = GetDlgItem ( IDC_EDIT_SPEED );
	    pWnd->GetWindowText ( strText );
        m_Property.m_fSpeed = (float)atof ( strText.GetString() );

        pWnd = GetDlgItem ( IDC_EDIT_EFF_NUMBER );
	    pWnd->GetWindowText ( strText );
        m_Property.m_dwEffNumber = atoi ( strText.GetString() );

        // Single Effect 설정.
	    pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
	    pWnd->GetWindowText ( strText );
        pEffChar->m_strSingleEffect = strText.GetString();
    }

    // m_pEffChar에 셋팅.
    {
	    pEffChar->SetProperty ( &m_Property );
		pEffChar->CleanUp();
	    pEffChar->Create ( pView->GetD3dDevice() );
    }

    EffectApply();
}

void CEffSplineSingleEffPage::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    OnBnClickedButtonApply();

    ReturnPage();

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

void CEffSplineSingleEffPage::OnBnClickedCancel()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

void CEffSplineSingleEffPage::OnBnClickedButtonSpointInsert()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if( !m_pEffChar )
        return;

    DxEffCharSplineSingleEff* pEffChar = dynamic_cast<DxEffCharSplineSingleEff*>(m_pEffChar);

    int nSelectIndex(0);
    int nIndex = m_listSplinePoint.GetCurSel(); 
	if( nIndex == LB_ERR )	
    {
        nSelectIndex = pEffChar->m_vecSplinePoint.size()-1;
    }
    else
    {
        nSelectIndex = nIndex;
    }

    pEffChar->InsertSplinePoint( nSelectIndex );

    ResetSplinePoint( nSelectIndex+1 );
}

void CEffSplineSingleEffPage::OnBnClickedButtonSpointDelete()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if( !m_pEffChar )
        return;

    DxEffCharSplineSingleEff* pEffChar = dynamic_cast<DxEffCharSplineSingleEff*>(m_pEffChar);

	DWORD dwIndex = m_listSplinePoint.GetCurSel(); 
	if( dwIndex == LB_ERR )	
        return;

	if( pEffChar->DeleteSplinePoint( dwIndex ) )
	{
		dwIndex = ( dwIndex >= (pEffChar->m_vecSplinePoint.size()-1) ) ? pEffChar->m_vecSplinePoint.size()-1 : dwIndex;

		ResetSplinePoint( dwIndex );
	}
}

void CEffSplineSingleEffPage::OnBnClickedButtonSpointUp()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if( !m_pEffChar )	return;

    DxEffCharSplineSingleEff* pEffChar = dynamic_cast<DxEffCharSplineSingleEff*>(m_pEffChar);

	DWORD dwIndex = m_listSplinePoint.GetCurSel();
	if( dwIndex == LB_ERR )	
        return;

	int nPos = pEffChar->UpSplinePoint( dwIndex );

	ResetSplinePoint( nPos );
}

void CEffSplineSingleEffPage::OnBnClickedButtonSpointDown()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if( !m_pEffChar )	return;

    DxEffCharSplineSingleEff* pEffChar = dynamic_cast<DxEffCharSplineSingleEff*>(m_pEffChar);

	DWORD dwIndex = m_listSplinePoint.GetCurSel();
	if( dwIndex == LB_ERR )	
        return;

	int nPos = pEffChar->DownSplinePoint( dwIndex );

	ResetSplinePoint( nPos );
}

void CEffSplineSingleEffPage::ResetSplinePoint( int nCur )
{
	if( !m_pEffChar )	return;

    DxEffCharSplineSingleEff* pEffChar = dynamic_cast<DxEffCharSplineSingleEff*>(m_pEffChar);

	m_listSplinePoint.ResetContent();

	for( DWORD i=0; i<pEffChar->m_vecSplinePoint.size(); ++i )
	{
        TSTRINGSTREAM strStringStream;
        strStringStream << i;
		int nIndex = m_listSplinePoint.AddString( strStringStream.str().c_str() );
		m_listSplinePoint.SetItemData( nIndex, nIndex );
	}

	m_listSplinePoint.SetCurSel( nCur );

    // 목표 : Matrix 를 재 설정한다.
    OnLbnSelchangeListSplinePoint();
}

void CEffSplineSingleEffPage::OnBnClickedButtonSelect()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CString szFilter = "Effect Group ( *.egp ) |*.egp";
	CFileDialog dlg ( TRUE, ".egp", "untitled",
		OFN_HIDEREADONLY|OFN_READONLY, szFilter, this );

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString FileName = dlg.GetFileName ();

		CWnd *pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
		pWnd->SetWindowText ( FileName );
	}
}

void CEffSplineSingleEffPage::OnBnClickedButtonGetbonename()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if ( !m_pEffChar )
        return;

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
    DxEffCharSplineSingleEff* pEffChar = dynamic_cast<DxEffCharSplineSingleEff*>(m_pEffChar);

    // 복사
	pEffChar->m_strBoneName = g_strPICK_BONE;

	CWnd* pWnd = GetDlgItem( IDC_EDIT_BONE_NAME2 );
	pWnd->SetWindowText( pEffChar->m_strBoneName.c_str() );

	pEffChar->SetProperty ( &m_Property );
	pEffChar->Create ( pView->GetD3dDevice() );
}

void CEffSplineSingleEffPage::OnLbnSelchangeListSplinePoint()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
    DxEffCharSplineSingleEff* pEffChar = dynamic_cast<DxEffCharSplineSingleEff*>(m_pEffChar);

    pView->DeActiveEditMatrix();

    DWORD dwIndex = m_listSplinePoint.GetCurSel();
	if( dwIndex == LB_ERR )	
        return;

    if ( dwIndex >= pEffChar->m_vecSplinePoint.size() )
        return;
    
    DxBoneTrans* pBoneTran(NULL);
    DxSkinPiece* pSkinPiece = pView->GetSkinPiece();
    if ( pSkinPiece && pSkinPiece->m_skeletonRes.IsValid() )
    {
        pBoneTran = pSkinPiece->m_skeletonRes.get()->FindBone_t( pEffChar->m_strBoneName.c_str() );
    }

    if( pBoneTran )
    {
        D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
        D3DXMatrixIdentity( &m_matWorld );
        D3DXVec3TransformCoord( &vPos, &pEffChar->m_vecSplinePoint[dwIndex], &pBoneTran->matCombined );
        m_matWorld._41 = vPos.x;
        m_matWorld._42 = vPos.y;
        m_matWorld._43 = vPos.z;
	}
    else
    {
        D3DXMatrixIdentity( &m_matWorld );
        m_matWorld._41 = pEffChar->m_vecSplinePoint[dwIndex].x;
        m_matWorld._42 = pEffChar->m_vecSplinePoint[dwIndex].y;
        m_matWorld._43 = pEffChar->m_vecSplinePoint[dwIndex].z;
    }

	pView->ActiveEditMatrix( &m_matWorld );
}

BOOL CEffSplineSingleEffPage::OnKillActive()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	pView->DeActiveEditMatrix();

    return CPropertyPage::OnKillActive();
}

void CEffSplineSingleEffPage::UpdateKeyBoardMouse()
{
    if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_UP )
    {
        UpdatePageMouseLClickUP();
    }
}

void CEffSplineSingleEffPage::UpdatePageMouseLClickUP()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
    DxEffCharSplineSingleEff* pEffChar = dynamic_cast<DxEffCharSplineSingleEff*>(m_pEffChar);

    if ( !pView->Pick() )
            return;

    DWORD dwIndex = m_listSplinePoint.GetCurSel();
	if( dwIndex == LB_ERR )	
        return;

	if( !pEffChar )
		return;

    if ( dwIndex >= pEffChar->m_vecSplinePoint.size() )
        return;
    
    DxBoneTrans* pBoneTran(NULL);
    DxSkinPiece* pSkinPiece = pView->GetSkinPiece();
    if ( pSkinPiece && pSkinPiece->m_skeletonRes.IsValid() )
    {
        pBoneTran = pSkinPiece->m_skeletonRes.get()->FindBone_t( pEffChar->m_strBoneName.c_str() );
    }

    if( pBoneTran )
    {
        D3DXMATRIX matInverse;
	    D3DXMatrixInverse( &matInverse, NULL, &pBoneTran->matCombined );
	    D3DXMatrixMultiply( &matInverse, &m_matWorld, &matInverse );

        pEffChar->m_vecSplinePoint[dwIndex].x = matInverse._41;
        pEffChar->m_vecSplinePoint[dwIndex].y = matInverse._42;
        pEffChar->m_vecSplinePoint[dwIndex].z = matInverse._43;
	}
    else
    {
        pEffChar->m_vecSplinePoint[dwIndex].x = m_matWorld._41;
        pEffChar->m_vecSplinePoint[dwIndex].y = m_matWorld._42;
        pEffChar->m_vecSplinePoint[dwIndex].z = m_matWorld._43;
    }

    // Make Round Nonuniform Spline
    pEffChar->MakeRNS();
}