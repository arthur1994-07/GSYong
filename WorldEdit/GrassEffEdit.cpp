// GrassEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "ColorPickerDlg.h"
#include "SheetWithTab.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxEffect/Frame/DxEffectGrass.h"

#include "GrassEffEdit.h"
#include ".\grasseffedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern D3DXVECTOR3 g_vDYNAMIC_POINT;


// GrassEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGrassEffEdit, CPropertyPage)
CGrassEffEdit::CGrassEffEdit()
	: CPropertyPage(CGrassEffEdit::IDD)
    , m_emBlushMode(BLUSH_INIT)
{
}

CGrassEffEdit::~CGrassEffEdit()
{
}

void CGrassEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_POINT, m_ListPoint);
}


BEGIN_MESSAGE_MAP(CGrassEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_POINTSELECT, OnBnClickedButtonPointselect)
	ON_BN_CLICKED(IDC_BUTTON_POINTDELETE, OnBnClickedButtonPointdelete)
	ON_LBN_SELCHANGE(IDC_LIST_POINT, OnLbnSelchangeListPoint)
	ON_BN_CLICKED(IDC_BUTTON_POINTRESET, OnBnClickedButtonPointreset)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE, OnBnClickedButtonDiffuse)
    ON_BN_CLICKED(IDC_BUTTON_PART_DELETE, &CGrassEffEdit::OnBnClickedButtonPartDelete)
END_MESSAGE_MAP()


// GrassEffEdit 메시지 처리기입니다.
void CGrassEffEdit::SetInstance ( DxEffectGrass *pGrassEff )
{
	m_pGrassEff = pGrassEff;
	m_OldProperty = m_Property = m_pGrassEff->GetProperty();

	//	Note : 값 설정.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pGrassEff->GetName() );


	//	Note : 모든 항목 지워주기
	//
	m_ListPoint.ResetContent ();

	//	Note : 선택 되었던 항목 나타내기
	//
	for ( DWORD i=0; i<m_pGrassEff->GetBassGrassNum(); i++ )
	{
		char *szText = m_pGrassEff->GetStrList ( i );

		int nIndex = m_ListPoint.AddString ( szText );
		m_ListPoint.SetItemData ( nIndex, nIndex );
	}


	char szValue[256];

	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	_gcvt ( m_Property.m_fPower, 7, szValue );
	pWnd->SetWindowText ( szValue );


	int nIndex = m_ListPoint.GetCurSel ();
	pWnd = GetDlgItem ( IDC_EDIT_LENGTH );
	if ( nIndex == -1 )						_gcvt ( 50.f, 7, szValue );
	else
	{
		float fLength = m_pGrassEff->GetfLength ( nIndex );
		_gcvt ( fLength, 7, szValue );
	}
	pWnd->SetWindowText ( szValue );


	pWnd = GetDlgItem ( IDC_EDIT_WIDTH1 );
	if ( nIndex == -1 )						_gcvt ( 5.f, 7, szValue );
	else
	{
		float fWidth = m_pGrassEff->GetfWidth ( nIndex );
		_gcvt ( fWidth, 7, szValue );
	}
	pWnd->SetWindowText ( szValue );


	pWnd = GetDlgItem ( IDC_EDIT_HEIGHT1 );
	if ( nIndex == -1 )						_gcvt ( 5.f, 7, szValue );
	else
	{
		float fHeight = m_pGrassEff->GetfHeight ( nIndex );
		_gcvt ( fHeight, 7, szValue );
	}
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_THIS_NUM );
	if ( nIndex == -1 )						_gcvt ( 50, 7, szValue );
	else
	{
		DWORD dwNum = m_pGrassEff->GetdwObjects ( nIndex );
		_gcvt ( dwNum, 7, szValue );
	}
	pWnd->SetWindowText ( szValue );

    // 브러쉬가 어떤 상태인지 셋팅함.
    SetBlushDlg();

    CWorldEditView::m_ViewType = EFFECT_GRASS_VIEW;


	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_Property.m_szTexture );
}

void CGrassEffEdit::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText ( FilePath );
	}
}

void CGrassEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_emBlushMode = BLUSH_INIT;

	CString strText;

	CWnd *pWnd;

	//	Note : 전체적인 것에 대한 셋팅
	//
	pWnd = GetDlgItem ( IDC_EDIT_POWER );
	pWnd->GetWindowText ( strText );
	m_Property.m_fPower = (float)atof ( strText.GetString() );

	CString strTexture;
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	strcpy ( m_Property.m_szTexture, strTexture.GetString() );

	//	Note : 속성을 적용.
	//
	m_pGrassEff->SetProperty ( m_Property );
	m_pGrassEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pGrassEff->Create ( pView->GetD3DDevice() );

    // 브러쉬가 어떤 상태인지 셋팅함.
    SetBlushDlg();

	// 잔디 생성
	//m_pGrassEff->MakeGrassMeshs ( pView->GetD3DDevice() );

}

void CGrassEffEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();
}

void CGrassEffEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_emBlushMode = BLUSH_INIT;

	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 물속성을 되돌림.
	//
	m_pGrassEff->SetProperty ( m_OldProperty );
	m_pGrassEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pGrassEff->Create ( pView->GetD3DDevice() );
}

void CGrassEffEdit::OnBnClickedButtonPointselect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_emBlushMode = BLUSH_INIT;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CString strText;

	CWnd *pWnd;

	const D3DXVECTOR3& vCollPos = pView->GetCollisionPos();
	D3DXVECTOR3 vPoint = vCollPos;
	//D3DXVECTOR3 vPoint = pView->GetClosedPoint ();
	DWORD dwPointIndex = m_pGrassEff->GetBassGrassNum();

	pWnd = GetDlgItem ( IDC_EDIT_THIS_NUM);
	pWnd->GetWindowText ( strText );
	DWORD dwObjects = (DWORD)atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_WIDTH1 );
	pWnd->GetWindowText ( strText );
	float fWidth = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_HEIGHT1 );
	pWnd->GetWindowText ( strText );
	float fHeight = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_LENGTH );
	pWnd->GetWindowText ( strText );
	float fLength = (float)atof ( strText.GetString() );

	CString Str;
	Str.Format ( "[%d] %4.2f, %4.2f, %4.2f, %4.2f", dwPointIndex, vPoint.x, vPoint.y, vPoint.z, fLength );
	
	int nIndex = m_ListPoint.AddString ( Str );
	m_ListPoint.SetItemData ( nIndex, nIndex );

	// 점을 만들면 그 점을 생성 
	m_pGrassEff->AddParticle ( pView->GetD3DDevice(), vPoint, dwObjects, fWidth, fHeight, fLength );

	// 잔디 생성
	//m_pGrassEff->AddGrass( pView->GetD3DDevice() );
	//m_pGrassEff->MakeGrassMeshs ( pView->GetD3DDevice() );

	// 선택 하도록 만듬 
	m_ListPoint.SetCurSel ( nIndex );

    // 브러쉬가 어떤 상태인지 셋팅함.
    SetBlushDlg();
}

void CGrassEffEdit::OnBnClickedButtonPointdelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_emBlushMode = BLUSH_INIT;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	//	선택한 것 번호 알기
	int nIndex = m_ListPoint.GetCurSel ();
	if ( nIndex == -1 )	return;

	// 점을 만들면 그 점을 생성 
	m_pGrassEff->DelParticle ( pView->GetD3DDevice(), nIndex );

	// 잔디 생성
//	m_pGrassEff->MakeGrassMeshs ( pView->GetD3DDevice() );

	m_ListPoint.DeleteString ( nIndex );

	//	Note : 모든 항목 지워주기
	//
	m_ListPoint.ResetContent ();

	//	Note : 선택 되었던 항목 나타내기
	//
	for ( DWORD i=0; i<m_pGrassEff->GetBassGrassNum(); i++ )
	{
		char *szText = m_pGrassEff->GetStrList ( i );

		int Index = m_ListPoint.AddString ( szText );
		m_ListPoint.SetItemData ( Index, Index );
	}

	// 선택 하도록 만듬 
	if ( nIndex-1 == -1 ) return;
	m_ListPoint.SetCurSel ( nIndex-1 );

    // 브러쉬가 어떤 상태인지 셋팅함.
    SetBlushDlg();
}

void CGrassEffEdit::OnLbnSelchangeListPoint()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CString strText;

	CWnd *pWnd;

	//	선택한 것 번호 알기
	int nIndex = m_ListPoint.GetCurSel ();
	if ( nIndex == -1 )	return;

	char	szValue[256];

	pWnd = GetDlgItem ( IDC_EDIT_THIS_NUM);
	_gcvt ( m_pGrassEff->GetdwObjects( nIndex ), 7, szValue );
	pWnd->SetWindowText ( szValue );

	float fTemp;

	pWnd = GetDlgItem ( IDC_EDIT_WIDTH1 );
	fTemp = m_pGrassEff->GetfWidth ( nIndex );
	_gcvt ( fTemp, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_HEIGHT1 );
	fTemp = m_pGrassEff->GetfHeight ( nIndex );
	_gcvt ( fTemp, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_LENGTH );
	fTemp = m_pGrassEff->GetfLength ( nIndex );
	_gcvt ( fTemp, 7, szValue );
	pWnd->SetWindowText ( szValue );

	// 
	D3DXVECTOR3 LookatPt = m_pGrassEff->GetVector3 ( nIndex );
	D3DXVECTOR3 FromPt = LookatPt;
	FromPt.x += 70.f;
	FromPt.y += 130.f;

	D3DXVECTOR3 Up = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
	DxViewPort::GetInstance().SetViewTrans ( FromPt, LookatPt, Up );
}

void CGrassEffEdit::OnBnClickedButtonPointreset()
{
    m_emBlushMode = BLUSH_INIT;

	D3DXVECTOR3		vTempPos;
	//	선택한 것 번호 알기
	int nIndex = m_ListPoint.GetCurSel ();
	if ( nIndex == -1 )	return;

	vTempPos = m_pGrassEff->GetVector3 ( nIndex );

	//	지운다.
	//
	OnBnClickedButtonPointdelete ();

	// 생성 작업 
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CString strText;

	CWnd *pWnd;

	D3DXVECTOR3 vPoint = vTempPos;
	DWORD dwPointIndex = m_pGrassEff->GetBassGrassNum();

	pWnd = GetDlgItem ( IDC_EDIT_THIS_NUM);
	pWnd->GetWindowText ( strText );
	DWORD dwObjects = (DWORD)atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_WIDTH1 );
	pWnd->GetWindowText ( strText );
	float fWidth = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_HEIGHT1 );
	pWnd->GetWindowText ( strText );
	float fHeight = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_LENGTH );
	pWnd->GetWindowText ( strText );
	float fLength = (float)atof ( strText.GetString() );

	CString Str;
	Str.Format ( "[%d] %4.2f, %4.2f, %4.2f, %4.2f", dwPointIndex, vPoint.x, vPoint.y, vPoint.z, fLength );
	
	nIndex = m_ListPoint.AddString ( Str );
	m_ListPoint.SetItemData ( nIndex, nIndex );

	// 점을 만들면 그 점을 생성 
	m_pGrassEff->AddParticle ( pView->GetD3DDevice(), vPoint, dwObjects, fWidth, fHeight, fLength );

	// 잔디 생성
	//m_pGrassEff->AddGrass( pView->GetD3DDevice() );
	//m_pGrassEff->MakeGrassMeshs ( pView->GetD3DDevice() );

	

	// 선택 하도록 만듬 
	m_ListPoint.SetCurSel ( nIndex );

    // 브러쉬가 어떤 상태인지 셋팅함.
    SetBlushDlg();
}
void CGrassEffEdit::OnBnClickedButtonDiffuse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD dwColor = m_Property.m_cColor;

	COLORREF retColor;
	COLORREF Color = RGB ( ((dwColor&0xff0000)>>16), ((dwColor&0xff00)>>8), dwColor&0xff );
	CColorPickerDlg colorDlg ( Color );

	if ( IDOK == colorDlg.DoModal () )
	{
		int	R,G,B;
		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );

		dwColor = 0xff<<24;
		dwColor += R<<16;
		dwColor += G<<8;
		dwColor += B;

		m_Property.m_cColor = dwColor;

		m_pGrassEff->SetProperty ( m_Property );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	}
}

void CGrassEffEdit::OnBnClickedButtonPartDelete()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if ( m_emBlushMode == BLUSH_PART_DELETE )
    {
        m_emBlushMode = BLUSH_INIT;
    }
    else
    {
        m_emBlushMode = BLUSH_PART_DELETE;
    }

    // 브러쉬가 어떤 상태인지 셋팅함.
    SetBlushDlg();
}

void CGrassEffEdit::SetBlushDlg()
{
    CWnd* pWnd = GetDlgItem( IDC_BUTTON_PART_DELETE );
    if ( m_emBlushMode == BLUSH_PART_DELETE )
    {
        pWnd->SetWindowText( _T("부분삭제 ON") );
    }
    else
    {
        pWnd->SetWindowText( _T("부분삭제 OFF") );
    }
}

void CGrassEffEdit::UpdateKeyBoardMouse()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

    if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_DOWNED )
    {
        if ( !pView->Pick() )
            return;

        if ( m_emBlushMode != BLUSH_PART_DELETE )
            return;

        // 일정범위의 현재 위치를 삭제하자.
        m_pGrassEff->DeleteGrass( g_vDYNAMIC_POINT );
    }
}