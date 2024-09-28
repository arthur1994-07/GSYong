// EffAniSinglePage.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditdoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "SheetWithTabAnim.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimationManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"

#include "EffAniSinglePage.h"
#include ".\effanisinglepage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffAniSinglePage ��ȭ �����Դϴ�.

extern	ANIMATE				g_Animate;


IMPLEMENT_DYNAMIC(CEffAniSinglePage, CPropertyPage)
CEffAniSinglePage::CEffAniSinglePage()
	: CPropertyPage(CEffAniSinglePage::IDD),
	m_pEffAni(NULL)
{
	D3DXMatrixIdentity( &m_matWorld );
}

CEffAniSinglePage::~CEffAniSinglePage()
{
}

void CEffAniSinglePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_POS_1, m_comboBip_1);
	DDX_Control(pDX, IDC_COMBO_POS_2, m_comboBip_2);
}


BEGIN_MESSAGE_MAP(CEffAniSinglePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnBnClickedButtonSelect)
	ON_CBN_SELCHANGE(IDC_COMBO_POS_1, OnCbnSelchangeComboPos1)
	ON_CBN_SELCHANGE(IDC_COMBO_POS_2, OnCbnSelchangeComboPos2)
	ON_BN_CLICKED(IDC_CHECK_TIMEDAY, OnBnClickedCheckTimeday)
	ON_BN_CLICKED(IDC_CHECK_TIMENIGHT, OnBnClickedCheckTimenight)
	ON_BN_CLICKED(IDC_CHECK_SKYFINE, OnBnClickedCheckSkyFine)
	ON_BN_CLICKED(IDC_CHECK_SKYRAIN, OnBnClickedCheckSkyrain)
	ON_BN_CLICKED(IDC_CHECK_SKYSNOW, OnBnClickedCheckSkysnow)
	ON_BN_CLICKED(IDC_CHECK_SKYLEAVES, OnBnClickedCheckSkyleaves)
	ON_BN_CLICKED(IDC_CHECK_CHARMOVE, OnBnClickedCheckCharmove)
	ON_BN_CLICKED(IDC_RADIO_MATRIX, OnBnClickedRadioMatrix)
	ON_BN_CLICKED(IDC_RADIO_POINT, OnBnClickedRadioPoint)
	ON_BN_CLICKED(IDC_RADIO_RANGE, OnBnClickedRadioRange)
	ON_BN_CLICKED(IDC_RADIO_DIRECT, OnBnClickedRadioDirect)
	ON_BN_CLICKED(IDC_CHECK_NEVER_DIE, &CEffAniSinglePage::OnBnClickedCheckNeverDie)
END_MESSAGE_MAP()


// CEffAniSinglePage �޽��� ó�����Դϴ�.
BOOL CEffAniSinglePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CEffAniSinglePage::SetInitDialog()
{
	int nIndex;

	const ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	
		return;
	const DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

	BoneTransMap::BONE_TRANS_MAP_CITER	iter;
	const BoneTransMap& BoneMap = pAnimCont->m_skeletonRes.get()->m_skeletonPart.BoneMap;

	m_comboBip_1.ResetContent();
	m_comboBip_2.ResetContent();

	int nCount = 0;
	for ( iter=BoneMap.cbegin(); iter!=BoneMap.cend(); ++iter, ++nCount )
	{
		nIndex = m_comboBip_1.AddString ( (*iter).first.c_str() );
		m_comboBip_1.SetItemData ( nIndex, reinterpret_cast<DWORD_PTR>( (*iter).second ) );

		nIndex = m_comboBip_2.AddString ( (*iter).first.c_str() );
		m_comboBip_2.SetItemData ( nIndex, reinterpret_cast<DWORD_PTR>( (*iter).second ) );
	}
}

void CEffAniSinglePage::SetEtc ( CString sAnimName, CsheetWithTabAnim *pSheetTabAni )
{
	m_sAnimName = sAnimName;
	m_pSheetTabAni = pSheetTabAni;

	SetInitDialog();
}

void CEffAniSinglePage::SetProperty ( EFFANI_PROPERTY *pProp )
{
	CWnd*		pWnd;
	CButton*	pButton;

	if ( pProp )
	{
		m_Property = *((EFFANI_PROPERTY_SINGLE*)pProp);
	}
	else
	{
		m_Property = EFFANI_PROPERTY_SINGLE();
	}

	int nIndex;
	if ( strlen(m_Property.m_szBipFirst) )
	{
		nIndex = m_comboBip_1.FindString ( 0, m_Property.m_szBipFirst );
	}
	else	nIndex = 0;
	m_comboBip_1.SetCurSel ( nIndex );

	if ( strlen(m_Property.m_szBipSecond) )
	{
		nIndex = m_comboBip_2.FindString ( 0, m_Property.m_szBipSecond );
	}
	else	nIndex = 0;
	m_comboBip_2.SetCurSel ( nIndex );

	BOOL	bPoint, bRange, bDirect, bMatrix;					// ���� ��ư	---------
	if ( m_Property.m_dwFlags & USEDRAWPOINT )
	{
		bPoint = TRUE;
		bRange = FALSE;
		bDirect = FALSE;
		bMatrix = FALSE;
	}
	else if ( m_Property.m_dwFlags & USEDRAWRANGE )
	{
		bPoint = FALSE;
		bRange = TRUE;
		bDirect = FALSE;
		bMatrix = FALSE;
	}
	else if ( m_Property.m_dwFlags & USEDRAWDIRECT )
	{
		bPoint = FALSE;
		bRange = FALSE;
		bDirect = TRUE;
		bMatrix = FALSE;
	}
	else if ( m_Property.m_dwFlags & USEDRAWMATRIX )
	{
		bPoint = FALSE;
		bRange = FALSE;
		bDirect = FALSE;
		bMatrix = TRUE;
	}

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_POINT );
	pButton->SetCheck ( bPoint );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_RANGE );
	pButton->SetCheck ( bRange );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_DIRECT );
	pButton->SetCheck ( bDirect );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_MATRIX );
	pButton->SetCheck ( bMatrix );



	pButton = (CButton*) GetDlgItem ( IDC_CHECK_TIMEDAY );	// üũ �ڽ�	---------
	pButton->SetCheck ( m_Property.m_dwFlags&USETIMEDAY );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_TIMENIGHT );
	pButton->SetCheck ( m_Property.m_dwFlags&USETIMENIGHT );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SKYFINE );
	pButton->SetCheck ( m_Property.m_dwFlags&USESKYFINE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SKYRAIN );
	pButton->SetCheck ( m_Property.m_dwFlags&USESKYRAIN );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SKYSNOW );
	pButton->SetCheck ( m_Property.m_dwFlags&USESKYSNOW );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SKYLEAVES );
	pButton->SetCheck ( m_Property.m_dwFlags&USESKYLEAVES );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_CHARMOVE );
	pButton->SetCheck ( m_Property.m_dwFlags&USECHARMOVE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_NEVER_DIE );
	pButton->SetCheck ( m_Property.m_dwFlags&USEEFFLOOP );


	char	szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_FACT );
	_gcvt ( m_Property.m_fFact, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE );
	_gcvt ( m_Property.m_fScale, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
	pWnd->SetWindowText ( m_Property.m_szEff );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView* pView = (CCharEditView*)pFrame->GetActiveView();

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinChar* pSkinChar = pView->GetSkinChar ();
		pSkinChar->m_spSkinAniControlThread->SETCURKEYTIME_FOR_EDIT( m_Property.m_dwFrame );
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone* pAttBone = pView->GetAttBone();
		pAttBone->m_spSkinAniControlThread->SETCURKEYTIME_FOR_EDIT( m_Property.m_dwFrame );
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle* pVehicle = pView->GetVehicle();
		pVehicle->m_spSkinAniControlThread->SETCURKEYTIME_FOR_EDIT( m_Property.m_dwFrame );
	}

	CDlgTimeLine *pDlgTimeLine = &pFrame->m_cDlgTimeLine;
	pDlgTimeLine->SetTimeState ( m_Property.m_dwFrame );

	FrameMarking ();

	SetMatrixAndActiveEditMatrix();
}

void CEffAniSinglePage::SetMatrixAndActiveEditMatrix()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	BoneTransMap* pBoneMap = NULL;

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinChar* pSkinChar = pView->GetSkinChar ();
		if( !pSkinChar ) return;
		pBoneMap = &pSkinChar->m_spSkinAniControlThread->GetSkeletonForEdit()->m_skeletonPart.BoneMap;
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone* pAttBone = pView->GetAttBone();
		if( !pAttBone ) return;
		pBoneMap = &pAttBone->m_spSkinAniControlThread->GetSkeletonForEdit()->m_skeletonPart.BoneMap;
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle* pVehicle = pView->GetVehicle();
		if( !pVehicle ) return;
		pBoneMap = &pVehicle->m_spSkinAniControlThread->GetSkeletonForEdit()->m_skeletonPart.BoneMap;
	}

	if( !pBoneMap )	return;



	std::string strname = m_Property.m_szBipFirst;
	BoneTransMap::BONE_TRANS_MAP_ITER iter = pBoneMap->find ( strname );
	if ( iter==pBoneMap->end() )			return;

	D3DXMatrixMultiply( &m_matWorld, &m_Property.m_matBase, &(*iter).second->matCombined );

	// Note : Ŀ�� Ȱ��ȭ
	pView->ActiveEditMatrix( &m_matWorld );
}

D3DXMATRIX SetRotateMatrix( BoneTransMap* BaseBoneMap, const D3DXMATRIX& matWorld, const TCHAR* pName )
{
	BoneTransMap* BoneMap = BaseBoneMap;

	std::string strname = pName;
	BoneTransMap::BONE_TRANS_MAP_ITER iter = BoneMap->find( strname );

	D3DXMATRIX matBase;
	D3DXMatrixIdentity( &matBase );
	if ( iter==BoneMap->end() )		return matBase;
	
	D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &(*iter).second->matCombined );
	D3DXMatrixMultiply( &matBase, &matWorld, &matInverse );

	return matBase;
}

void CEffAniSinglePage::OnBnClickedButtonApply()
{
	SetDxEffAniData( TRUE );
}

void CEffAniSinglePage::SetDxEffAniData( BOOL bSetKeyFrame )
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	
	CWnd*		pWnd;
	CButton*	pButton;
	CString		strTexture;
	const char*	szTexture;

	SetComboPos();
	OnCbnSelchangeComboPos2();

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_POINT );	// ���� ��ư	---------
	if ( pButton->GetCheck() )
	{
		m_Property.m_dwFlags |= USEDRAWPOINT;
		m_Property.m_dwFlags &= ~USEDRAWRANGE;
		m_Property.m_dwFlags &= ~USEDRAWDIRECT;
		m_Property.m_dwFlags &= ~USEDRAWMATRIX;
	}
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_RANGE );
	if ( pButton->GetCheck() )
	{
		m_Property.m_dwFlags &= ~USEDRAWPOINT;
		m_Property.m_dwFlags |= USEDRAWRANGE;
		m_Property.m_dwFlags &= ~USEDRAWDIRECT;
		m_Property.m_dwFlags &= ~USEDRAWMATRIX;
	}
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_DIRECT );
	if ( pButton->GetCheck() )
	{
		m_Property.m_dwFlags &= ~USEDRAWPOINT;
		m_Property.m_dwFlags &= ~USEDRAWRANGE;
		m_Property.m_dwFlags |= USEDRAWDIRECT;
		m_Property.m_dwFlags &= ~USEDRAWMATRIX;
	}
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_MATRIX );
	if ( pButton->GetCheck() )
	{
		m_Property.m_dwFlags &= ~USEDRAWPOINT;
		m_Property.m_dwFlags &= ~USEDRAWRANGE;
		m_Property.m_dwFlags &= ~USEDRAWDIRECT;
		m_Property.m_dwFlags |= USEDRAWMATRIX;
	}


	pWnd = GetDlgItem ( IDC_EDIT_FACT );
	pWnd->GetWindowText ( strTexture );
	m_Property.m_fFact = (float) atof ( strTexture.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE );
	pWnd->GetWindowText ( strTexture );
	m_Property.m_fScale = (float) atof ( strTexture.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
	pWnd->GetWindowText ( strTexture );
	szTexture = strTexture.GetString();
	strcpy ( m_Property.m_szEff, szTexture );


	//////////////////////////////////////////////////////////////////////////
	// �ִϸ��̼� �߿��� ������ ���� �ʴ� �������� �ϴ°� �°���.
	// �������� ���°� �´°� ����.
	if ( g_Animate == AM_STOP && bSetKeyFrame )
	{
		//////////////////////////////////////////////////////////////////////////
		// ���� Ŀ���� ���氡���� �ñ�.. �ᱹ ����Ű�� ���� �ִϸ��̼� Ű�� ���� ��츸 m_Property.m_matBase �� ����õȴ�.
		if ( pView->IsActiveEditMatrix() )
		{
			if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
			{
				DxSkinChar* pSkinChar = pView->GetSkinChar ();
				// Note : ��Ʈ������ ã�´�. & m_Property �༮�� ���ؼ� ���� �־�����Ѵ�.
				m_Property.m_matBase = SetRotateMatrix( &pSkinChar->m_spSkinAniControlThread->GetSkeletonForEdit()->m_skeletonPart.BoneMap, m_matWorld, m_Property.m_szBipFirst );
			}
			else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
			{
				DxAttBone* pAttBone = pView->GetAttBone();
				// Note : ��Ʈ������ ã�´�. & m_Property �༮�� ���ؼ� ���� �־�����Ѵ�.
				m_Property.m_matBase = SetRotateMatrix( &pAttBone->m_spSkinAniControlThread->GetSkeletonForEdit()->m_skeletonPart.BoneMap, m_matWorld, m_Property.m_szBipFirst );
			}
			else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
			{
				DxVehicle* pVehicle = pView->GetVehicle();
				// Note : ��Ʈ������ ã�´�. & m_Property �༮�� ���ؼ� ���� �־�����Ѵ�.
				m_Property.m_matBase = SetRotateMatrix( &pVehicle->m_spSkinAniControlThread->GetSkeletonForEdit()->m_skeletonPart.BoneMap, m_matWorld, m_Property.m_szBipFirst );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// m_Property.m_dwFrame ���õ�.
		DWORD dwUnitTime = 0;
		if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
		{
			DxSkinChar* pSkinChar = pView->GetSkinChar ();
			dwUnitTime = pSkinChar->m_spSkinAniControlThread->GETCURKEYTIME();
		}
		else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
		{
			DxAttBone* pAttBone = pView->GetAttBone();
			dwUnitTime = pAttBone->m_spSkinAniControlThread->GETCURKEYTIME();
		}
		else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
		{
			DxVehicle* pVehicle = pView->GetVehicle();
			dwUnitTime = pVehicle->m_spSkinAniControlThread->GETCURKEYTIME();
		}

		m_Property.m_dwFrame = dwUnitTime;
	}


	SetProperty ( &m_Property );

	if ( !m_pEffAni )
	{
		m_pEffAni = DxEffAniMan::GetInstance().CreateEffect
		(
			DxEffAniData_Single::TYPEID,
			&m_Property
		);

		if ( !m_pEffAni )
		{
			MessageBox ( "����Ʈ ������ �����Ͽ����ϴ�.", "ERROR", MB_OK );
			return;
		}

		m_Property = *((EFFANI_PROPERTY_SINGLE*)m_pEffAni->GetProperty ());

		ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
		if ( !pAnimNodeCur )	return;
		DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

		pAnimCont->m_listEffAni.push_back ( m_pEffAni );		// ȿ���� �����Ѵ�.
	}
	else
	{
		m_pEffAni->SetProperty ( &m_Property );
		m_pEffAni->Create ( pView->GetD3dDevice() );
	}

	FrameMarking ();

	// DxEffAniSingle �� ���� �ʴ� ���� ������ ��.
	{
		ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
		if ( !pAnimNodeCur )	
			return;
		DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

		if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
		{
			DxSkinChar* pSkinChar = pView->GetSkinChar ();
			DxEffAniPlayer* pEffAniPlayer = pSkinChar->m_spSkinAniControlThread->m_pEffAniPlayer;
			if ( pEffAniPlayer )
			{
				pEffAniPlayer->DeleteEffSingleEffLoop();
				pEffAniPlayer->CheckCreateEff( pAnimCont );
			}
		}
		else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
		{
			DxAttBone* pAttBone = pView->GetAttBone();
			DxEffAniPlayer* pEffAniPlayer = pAttBone->m_spSkinAniControlThread->m_pEffAniPlayer;
			if ( pEffAniPlayer )
			{
				pEffAniPlayer->DeleteEffSingleEffLoop();
				pEffAniPlayer->CheckCreateEff( pAnimCont );
			}
		}
		else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
		{
			DxVehicle* pVehicle = pView->GetVehicle();
			DxEffAniPlayer* pEffAniPlayer = pVehicle->m_spSkinAniControlThread->m_pEffAniPlayer;
			if ( pEffAniPlayer )
			{
				pEffAniPlayer->DeleteEffSingleEffLoop();
				pEffAniPlayer->CheckCreateEff( pAnimCont );
			}
		}
	}
}

void CEffAniSinglePage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetDxEffAniData( FALSE );

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

void CEffAniSinglePage::OnBnClickedCancel()
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

void CEffAniSinglePage::FrameMarking ()
{
	WORD	pIndices[1];

	pIndices[0] = (WORD)m_Property.m_dwFrame;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->m_cDlgTimeLine.SetFrameMarking ( 1, pIndices );
}

void CEffAniSinglePage::OnBnClickedButtonSelect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

void CEffAniSinglePage::SetComboPos()
{
	int nCurSel = m_comboBip_1.GetCurSel();
	DxBoneTrans* pBonTrans = reinterpret_cast<DxBoneTrans*>( m_comboBip_1.GetItemData ( nCurSel ) );

	strcpy ( m_Property.m_szBipFirst, pBonTrans->GetName().c_str() );

	//BoneTransMap::BONE_TRANS_MAP_CITER	iter;
	//BoneTransMap& BoneMap = pAnimCont->m_skeletonRes.get()->m_skeletonPart.BoneMap;

	//int nCount = 0;
	//for ( iter=BoneMap.begin(); iter!=BoneMap.end(); ++iter, ++nCount )
	//{
	//	if ( nCount == nCurSel )	strcpy ( m_Property.m_szBipFirst, (*iter).first.c_str() );
	//}
}

void CEffAniSinglePage::OnCbnSelchangeComboPos1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetComboPos();
	D3DXMatrixIdentity( &m_Property.m_matBase );	// ���� �ٲ������ �ʱ�ȭ�� �Ѵ�.

	SetMatrixAndActiveEditMatrix();
}

void CEffAniSinglePage::OnCbnSelchangeComboPos2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nCurSel = m_comboBip_2.GetCurSel();
	DxBoneTrans* pBonTrans = reinterpret_cast<DxBoneTrans*>( m_comboBip_1.GetItemData ( nCurSel ) );

	strcpy ( m_Property.m_szBipSecond, pBonTrans->GetName().c_str() );

	//BoneTransMap::BONE_TRANS_MAP_CITER	iter;
	//BoneTransMap& BoneMap = pAnimCont->m_skeletonRes.get()->m_skeletonPart.BoneMap;

	//int nCount = 0;
	//for ( iter=BoneMap.begin(); iter!=BoneMap.end(); ++iter, ++nCount )
	//{
	//	if ( nCount == nCurSel )	strcpy ( m_Property.m_szBipSecond, (*iter).first.c_str() );
	//}
}

void CEffAniSinglePage::OnBnClickedCheckTimeday()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_TIMEDAY );
	if ( pButton->GetCheck() )	m_Property.m_dwFlags |= USETIMEDAY;
	else						m_Property.m_dwFlags &= ~USETIMEDAY;
}

void CEffAniSinglePage::OnBnClickedCheckTimenight()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_TIMENIGHT );
	if ( pButton->GetCheck() )	m_Property.m_dwFlags |= USETIMENIGHT;
	else						m_Property.m_dwFlags &= ~USETIMENIGHT;
}

void CEffAniSinglePage::OnBnClickedCheckSkyFine()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SKYFINE );
	if ( pButton->GetCheck() )	m_Property.m_dwFlags |= USESKYFINE;
	else						m_Property.m_dwFlags &= ~USESKYFINE;
}

void CEffAniSinglePage::OnBnClickedCheckSkyrain()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SKYRAIN );
	if ( pButton->GetCheck() )	m_Property.m_dwFlags |= USESKYRAIN;
	else						m_Property.m_dwFlags &= ~USESKYRAIN;
}

void CEffAniSinglePage::OnBnClickedCheckSkysnow()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SKYSNOW );
	if ( pButton->GetCheck() )	m_Property.m_dwFlags |= USESKYSNOW;
	else						m_Property.m_dwFlags &= ~USESKYSNOW;
}

void CEffAniSinglePage::OnBnClickedCheckSkyleaves()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SKYLEAVES );
	if ( pButton->GetCheck() )	m_Property.m_dwFlags |= USESKYLEAVES;
	else						m_Property.m_dwFlags &= ~USESKYLEAVES;
}

void CEffAniSinglePage::OnBnClickedCheckCharmove()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_CHARMOVE );
	if ( pButton->GetCheck() )	m_Property.m_dwFlags |= USECHARMOVE;
	else						m_Property.m_dwFlags &= ~USECHARMOVE;
}

void CEffAniSinglePage::OnBnClickedRadioPoint()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_Property.m_dwFlags |= USEDRAWPOINT;
	m_Property.m_dwFlags &= ~USEDRAWRANGE;
	m_Property.m_dwFlags &= ~USEDRAWDIRECT;
	m_Property.m_dwFlags &= ~USEDRAWMATRIX;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	pView->DeActiveEditMatrix();
}

void CEffAniSinglePage::OnBnClickedRadioMatrix()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_Property.m_dwFlags &= ~USEDRAWPOINT;
	m_Property.m_dwFlags &= ~USEDRAWRANGE;
	m_Property.m_dwFlags &= ~USEDRAWDIRECT;
	m_Property.m_dwFlags |= USEDRAWMATRIX;

	SetMatrixAndActiveEditMatrix();
}

void CEffAniSinglePage::OnBnClickedRadioRange()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_Property.m_dwFlags &= ~USEDRAWPOINT;
	m_Property.m_dwFlags |= USEDRAWRANGE;
	m_Property.m_dwFlags &= ~USEDRAWDIRECT;
	m_Property.m_dwFlags &= ~USEDRAWMATRIX;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	pView->DeActiveEditMatrix();
}

void CEffAniSinglePage::OnBnClickedRadioDirect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_Property.m_dwFlags &= ~USEDRAWPOINT;
	m_Property.m_dwFlags &= ~USEDRAWRANGE;
	m_Property.m_dwFlags |= USEDRAWDIRECT;
	m_Property.m_dwFlags &= ~USEDRAWMATRIX;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	pView->DeActiveEditMatrix();
}


BOOL CEffAniSinglePage::OnKillActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	pView->DeActiveEditMatrix();

	return CPropertyPage::OnKillActive();
}

void CEffAniSinglePage::OnBnClickedCheckNeverDie()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_NEVER_DIE );
	if ( pButton->GetCheck() )	m_Property.m_dwFlags |= USEEFFLOOP;
	else						m_Property.m_dwFlags &= ~USEEFFLOOP;
}

// User �� TimeLine �� �ð��� �����Ѵٸ� �׿� ������ �۾��� �Ѵ�.
void CEffAniSinglePage::ChangeTimeOfTimeLine( DWORD dwToTime )
{
	// ���� Frame ���� Matrix �� ������ �� �ֵ��� ����.
	if ( m_Property.m_dwFrame == dwToTime )
	{
		SetMatrixAndActiveEditMatrix();
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
		pView->DeActiveEditMatrix();
	}
}

// User �� CDlgTimeLine �� OnButtonPlay �� �����ٸ� �׿� ������ �۾��� �Ѵ�.
void CEffAniSinglePage::OnButtonPlayOfCDlgTimeLine()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	if ( g_Animate == AM_PLAY )
	{
		// 2 Frame ������ ���� ������ �Ѵ�.
		// DxEffAniSingle �� Matrix ��ġ ����� ���� Ȯ���� �ϱ� ����.
		float fPrevTime(0.f);

		const DxSkeleton* pSkeleton(NULL);
		switch( pView->m_emObjType )
		{
		case EOBJ_SKINCHAR:
			{
				DxSkinChar *pSkinChar = pView->GetSkinChar();
				fPrevTime = pSkinChar->GETCURTIME() - (pSkinChar->GETUNITTIME()*2.f);
				pSkinChar->SETCURTIME ( fPrevTime < 0.f ? 0.f : fPrevTime );
			}
			break;
		case EOBJ_ATTBONE:
			{
				DxAttBone *pAttBone = pView->GetAttBone();
				fPrevTime = pAttBone->GETCURTIME() - (pAttBone->GETUNITTIME()*2.f);
				pAttBone->SETCURTIME ( fPrevTime < 0.f ? 0.f : fPrevTime );
			}
			break;
		case EOBJ_VEHICLE:
			{
				DxVehicle *pVehicle = pView->GetVehicle();
				fPrevTime = pVehicle->GETCURTIME() - (pVehicle->GETUNITTIME()*2.f);
				pVehicle->SETCURTIME ( fPrevTime < 0.f ? 0.f : fPrevTime );
			}
			break;
		default:
			break;
		}
	}
}