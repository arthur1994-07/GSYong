
#include "stdafx.h"

#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"

#include "../CharEdit.h"
#include "../CharEditView.h"
#include "../MainFrm.h"
#include "./MultianimPage.h"

#include "../../RanLogic/FlyCamera/GLFlyCameraKeyFrameControl.h"

#include "../Properties/sheetWithTabChar.h"
#include "../EngineLib/DxTools/NSCharAniSequence.h"
#include "../Dialog/ResolutionDlg.h"

extern ANIMATE g_Animate;

IMPLEMENT_DYNAMIC(CMultianimPage, CPropertyPage)
CMultianimPage::CMultianimPage(void)
: CPropertyPage(IDD)
, m_pSheetTab(NULL)
, m_nAnimIndex(0)
{
	m_ListAnimation.SetPopupID(IDR_POPUP_MULTIANIM, LIST_NOTIFYMSG);
}

CMultianimPage::~CMultianimPage(void)
{
}

void CMultianimPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ANIMATION, m_ListAnimation);

	CWnd* pWnd = GetDlgItem( IDC_BUTTON_LIST_ALIGN_UP );
	pWnd->SetWindowText( "△" );

	pWnd = GetDlgItem( IDC_BUTTON_LIST_ALIGN_DOWN );
	pWnd->SetWindowText( "▽" );

	pWnd = GetDlgItem( IDC_BUTTON_LIST_PLAY );
	pWnd->SetWindowText( "▶" );
}

BEGIN_MESSAGE_MAP(CMultianimPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_LIST_ALIGN_UP, OnBtnClickListAlignUp)
	ON_BN_CLICKED(IDC_BUTTON_LIST_ALIGN_DOWN, OnBtnClickListAlignDown)
	ON_BN_CLICKED(IDC_BUTTON_LIST_PLAY, OnBtnClickPlay)
	ON_BN_CLICKED(IDC_BUTTON_LIST_ITEMDELETE, OnBtnClickDelete)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_ANI_SEQUENCE, OnBnClickedButtonCreateAniSequence)
	ON_MESSAGE(LIST_NOTIFYMSG, OnAnimationCont)
END_MESSAGE_MAP()

BOOL CMultianimPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();
	return TRUE;
}

void CMultianimPage::OnBtnClickListAlignUp(void)
{
	const INT nCurSel(m_ListAnimation.GetCurSel());
	if ( m_ListAnimation.GetSelCount() != 1 || nCurSel == 0 )
		return;

	CString strSel, strTarget;
	m_ListAnimation.GetText(nCurSel-1, strTarget);
	m_ListAnimation.GetText(nCurSel, strSel);
	m_ListAnimation.DeleteString(nCurSel-1);
	m_ListAnimation.DeleteString(nCurSel-1);
	m_ListAnimation.InsertString(nCurSel-1, strTarget);
	m_ListAnimation.InsertString(nCurSel-1, strSel);
	m_ListAnimation.SetCurSel(nCurSel-1);
}

void CMultianimPage::OnBtnClickListAlignDown(void)
{
	const INT nCurSel(m_ListAnimation.GetCurSel());
	if ( m_ListAnimation.GetSelCount() != 1 || nCurSel == 0 )
		return;

	CString strSel, strTarget;
	m_ListAnimation.GetText(nCurSel+1, strTarget);
	m_ListAnimation.GetText(nCurSel, strSel);
	m_ListAnimation.DeleteString(nCurSel);
	m_ListAnimation.DeleteString(nCurSel);
	m_ListAnimation.InsertString(nCurSel, strSel);
	m_ListAnimation.InsertString(nCurSel, strTarget);
	m_ListAnimation.SetCurSel(nCurSel+1);
}

void CMultianimPage::OnBtnClickPlay(void)
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinChar *pSkinChar = pView->GetSkinChar();

	m_nAnimIndex = 0;

	if ( pSkinChar && m_ListAnimation.GetCount() > 0 )
	{
		CString strFirAnim;
		m_ListAnimation.GetText(0, strFirAnim);
		pSkinChar->m_spSkinAniControlThread->SELECTANI_EDIT(strFirAnim.GetString(), FALSE, EMANI_ONLYZEROFRAMESTART|EMANI_IGNORELOOP);
	}

	g_Animate = AM_MULTIPLAY;
}

void CMultianimPage::OnBtnClickDelete(void)
{
	const INT nSelCount(m_ListAnimation.GetSelCount());
	if ( nSelCount <= 0 )
		return;

	CArray<INT, INT> arrSel;
	arrSel.SetSize(nSelCount);
	m_ListAnimation.GetSelItems(nSelCount, arrSel.GetData());
	for ( INT i = 0; i < nSelCount; ++i )
	{
		const INT nSel(arrSel[i]-i);
		m_ListAnimation.DeleteString(nSel);
	}
}

void CMultianimPage::OnBnClickedButtonCreateAniSequence(void)
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIMATION );

	// 2개 이상일 경우 작동시키지 않는다.
	if( pListBox->GetSelCount() >= 2 )
	{
		MessageBox( _T("2개이상 선택되어서 작동이 되지 않습니다.") );
		return;
	}

//	DWORD nIndex = pListBox->GetCurSel();
//	if ( nIndex == -1 ) return;

//	CString sItemText;
//	pListBox->GetText ( nIndex, sItemText );

	CResolutionDlg sDlg;
	sDlg.m_dwResolutionINDEX	= 0;
	sDlg.m_dwAA_INDEX			= 4;
	sDlg.m_dwFPS				= 60;
	sDlg.m_fAddTime				= 0.5f;
	sDlg.m_fColorPower			= 1.3f;

	if( sDlg.DoModal() == IDOK )
	{
		const INT nMaxCount(m_ListAnimation.GetCount());
		std::vector<std::string> vecAnimname;
		for ( INT i = 0; i < nMaxCount; ++i )
		{
			CString strAnim;
			m_ListAnimation.GetText(i, strAnim);
			vecAnimname.push_back(strAnim.GetString());
		}

		CsheetWithTabChar* pSheetWithTab = pFrame->m_cDlgEdit.GetSheetChar();
		GLFlyCameraKeyFrameControl* pFlyCam = &pSheetWithTab->m_FlyCameraPage.GetKeyFrameCtrl();

		DWORD dwAnimSeqFlag = sDlg.m_bOnlyCurrentFrame ? NSCharAniSequence::EMANISEQ_ONLYCURRENTFRAME : 0;
		dwAnimSeqFlag |= NSCharAniSequence::EMANISEQ_MULTIANIM;

		NSCharAniSequence::Create
			( 
			pView->GetD3dDevice(),
			pView->GetEngineDevice(),
			pView->GetSkinCharData(),
			pView->GetSkinCharData()->GetFileName().c_str(), 
			vecAnimname,
			pView->GetLandMan(),
			sDlg.m_dwResolutionINDEX,
			sDlg.m_dwAA_INDEX,
			sDlg.m_dwFPS,
			sDlg.m_fAddTime,
			sDlg.m_fColorPower,
			pView->GetSkinMatrix(),
			dwAnimSeqFlag,
			pView->GetSkinChar()->GETCURKEYTIME(),
			CCharEditView::m_emObjType == EOBJ_FLYCAMERA ? pFlyCam : NULL
			);
	}
}

LRESULT CMultianimPage::OnAnimationCont(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
	case ID_POPUP_MULTIANIM_DELETE:
		{
			OnBtnClickDelete();
		}
		break;
	}
	return S_OK;
}

void CMultianimPage::AddAnim(CString strAnimName)
{
	// 중복 출력이 필요할 수도 있다;
	// if ( m_ListAnimation.FindString( 0, strAnimName ) == LB_ERR )
	{
		m_ListAnimation.AddString(strAnimName.GetString());
	}
}

CString CMultianimPage::GetAnimName(INT nIndex)
{
	CString strTmp;
	const INT nMaxCount(m_ListAnimation.GetCount());
	if ( nMaxCount <= nIndex)
		return strTmp;

	m_ListAnimation.GetText(nIndex, strTmp);
	return strTmp;
}

CString CMultianimPage::GetNextAnimName(void)
{
	const INT nMaxCount(m_ListAnimation.GetCount());
	++m_nAnimIndex;
	if ( nMaxCount <= m_nAnimIndex )
	{
		m_nAnimIndex = 0;
		if ( nMaxCount == 0 )
		{
			return "";
		}
	}

	CString str;
	m_ListAnimation.GetText(m_nAnimIndex, str);
	return str.GetString();
}