// EffAniTracePage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"
#include "../MainFrm.h"
#include "../CharEditdoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"

#include "SheetWithTabChar.h"
#include "SheetWithTabAnim.h"

#include "EffAniTracePage.h"
#include ".\effanitracepage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffAniTracePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffAniTracePage, CPropertyPage)
CEffAniTracePage::CEffAniTracePage()
	: CPropertyPage(CEffAniTracePage::IDD),
	m_pEffAni(NULL)
{
}

CEffAniTracePage::~CEffAniTracePage()
{
}

void CEffAniTracePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffAniTracePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEX, OnBnClickedButtonTex)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CEffAniTracePage 메시지 처리기입니다.
void CEffAniTracePage::SetEtc( CString sAnimName, CsheetWithTabAnim *pSheetTabAni )
{
	m_sAnimName = sAnimName;
	m_pSheetTabAni = pSheetTabAni;
}

void CEffAniTracePage::SetProperty( EFFANI_PROPERTY *pProp )
{
	if( pProp )	m_Property = *((EFFANI_PROPERTY_TRACE*)pProp);
	else		return;

	char	szValue[256];
	CWnd	*pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_SIZE );
	_gcvt ( m_Property.m_fScale, 7, szValue );
	pWnd->SetWindowText ( szValue );
}

void CEffAniTracePage::OnBnClickedButtonTex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CEffAniTracePage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	
	CWnd*		pWnd;
	CString		strTexture;

	pWnd = GetDlgItem( IDC_EDIT_SIZE );
	pWnd->GetWindowText( strTexture );
	m_Property.m_fScale = (float) atof( strTexture.GetString() );

	SetProperty ( &m_Property );

	if ( !m_pEffAni )
	{
		m_pEffAni = DxEffAniMan::GetInstance().CreateEffect
		(
			DxEffAniData_Trace::TYPEID,
			&m_Property
		);

		if ( !m_pEffAni )
		{
			MessageBox ( "이팩트 생성에 실패하였습니다.", "ERROR", MB_OK );
			return;
		}

		m_Property = *((EFFANI_PROPERTY_TRACE*)m_pEffAni->GetProperty ());

		ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const( m_sAnimName.GetString() );
		if ( !pAnimNodeCur )	return;
		DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

		pAnimCont->m_listEffAni.push_back( m_pEffAni );		// 효과를 삽입한다.
	}
	else
	{
		m_pEffAni->SetProperty ( &m_Property );
		m_pEffAni->Create ( pView->GetD3dDevice() );
	}
}

void CEffAniTracePage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

	m_pEffAni = NULL;
	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_CHARFILE );
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_ATTBONEFILE );
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_VEHICLE );
	}
}

void CEffAniTracePage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pEffAni = NULL;
	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_CHARFILE );
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_ATTBONEFILE );
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_VEHICLE );
	}
}



