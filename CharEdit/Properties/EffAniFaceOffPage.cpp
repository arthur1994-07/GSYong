// EffAniFAceOffPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditdoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "SheetWithTabAnim.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"

#include "EffAniFAceOffPage.h"


// CEffAniFaceOffPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffAniFaceOffPage, CPropertyPage)
CEffAniFaceOffPage::CEffAniFaceOffPage()
	: CPropertyPage(CEffAniFaceOffPage::IDD)
{
}

CEffAniFaceOffPage::~CEffAniFaceOffPage()
{
}

void CEffAniFaceOffPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffAniFaceOffPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_TEX, OnBnClickedButtonTex)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CEffAniFaceOffPage �޽��� ó�����Դϴ�.
void CEffAniFaceOffPage::SetEtc( CString sAnimName, CsheetWithTabAnim *pSheetTabAni )
{
	m_sAnimName = sAnimName;
	m_pSheetTabAni = pSheetTabAni;
}

void CEffAniFaceOffPage::SetProperty( EFFANI_PROPERTY *pProp )
{
	if ( pProp )
	{
		m_Property = *((EFFANI_PROPERTY_FACEOFF*)pProp);
	}
	else
	{
		m_Property = EFFANI_PROPERTY_FACEOFF();
	}

	CWnd	*pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText( m_Property.m_szTexture );
}

void CEffAniFaceOffPage::OnBnClickedButtonTex()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText( dlg.GetFileName().GetString() );
	}
}

void CEffAniFaceOffPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	
	CWnd*		pWnd;
	CString		strTexture;

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText( strTexture );
	StringCchCopy( m_Property.m_szTexture, 32, strTexture.GetString() );

	SetProperty ( &m_Property );

	if ( !m_pEffAni )
	{
		m_pEffAni = DxEffAniMan::GetInstance().CreateEffect
		(
			DxEffAniData_FaceOff::TYPEID,
			&m_Property
		);

		if ( !m_pEffAni )
		{
			MessageBox ( _T("����Ʈ ������ �����Ͽ����ϴ�."), _T("ERROR"), MB_OK );
			return;
		}

		m_Property = *((EFFANI_PROPERTY_FACEOFF*)m_pEffAni->GetProperty ());

		ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const( m_sAnimName.GetString() );
		if ( !pAnimNodeCur )	return;
		DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

		pAnimCont->m_listEffAni.push_back( m_pEffAni );		// ȿ���� �����Ѵ�.
	}
	else
	{
		m_pEffAni->SetProperty ( &m_Property );
		m_pEffAni->Create ( pView->GetD3dDevice() );
	}
}

void CEffAniFaceOffPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

void CEffAniFaceOffPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
