// EffectNamePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "EffectNamePage.h"
#include "SheetWithTabAnim.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"
#include "SheetWithTabChar.h"
#include "SheetWithTabFrame.h"
#include "SheetWithTabBone.h"
#include "SheetWithTabAnim.h"
#include "../EngineLib/DxTools/DxMethods.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"
#include ".\effectnamepage.h"

#ifdef DEF_SKIN_SYNC
#include "../EngineLib/DxEffect/Char/DxEffCharSingle.h"
#endif

// CEffectNamePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffectNamePage, CPropertyPage)
CEffectNamePage::CEffectNamePage()
	: CPropertyPage(CEffectNamePage::IDD)
{
}

CEffectNamePage::~CEffectNamePage()
{
}

void CEffectNamePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NAME, m_EffNameList);
}


BEGIN_MESSAGE_MAP(CEffectNamePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_RESET_EFFECT, OnBnClickedButtonResetEffect)
	ON_BN_CLICKED(IDC_BUTTON_PARTS_SAVE, OnBnClickedButtonPartsSave)
END_MESSAGE_MAP()


void CEffectNamePage::SetData()
{
    if ( m_hWnd == NULL )
        return;

	m_EffNameList.DeleteAllItems();

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
		LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
		DxSkinChar *pSkinChar = pView->GetSkinChar();
		DxSkinCharData *pSkinCharData = pView->GetSkinCharData();

		if( !pSkinCharData )
			return;

		DxEffChar* pEffCur = NULL;
		BOOL bName = FALSE;
		INT nIndex = 0;
		CString strCombine;
		for( INT i = 0; i < PIECE_SIZE; i++ )
		{
			bName = FALSE;
			DxSkinPieceBase* pSkinPieceBase = pSkinCharData->GetPiece( pd3dDevice, i, EMSCD_ZERO );
			if( !pSkinPieceBase )
				continue;

			if ( pSkinPieceBase->m_emSkinPieceType != DxSkinPieceBase::SKIN_PIECE )
				continue;

			DxSkinPiece* pSkinPiece = dynamic_cast<DxSkinPiece*>( pSkinPieceBase );

			GLEFFCHAR_VEC_ITER iter		= pSkinPiece->m_vecEFFECT.begin ();
			GLEFFCHAR_VEC_ITER iter_end = pSkinPiece->m_vecEFFECT.end ();
			for ( ; iter!=iter_end; )
			{
				GLEFFCHAR_VEC_ITER iter_cur = iter++;
				pEffCur = (*iter_cur);

				EFFCHAR_TYPES EffType = ( EFFCHAR_TYPES )pEffCur->GetTypeID();

				if( !bName )
				{
					m_EffNameList.InsertItem ( nIndex, DxSkinPiece::m_szPIECETYPE[i], NULL );
					m_EffNameList.SetItemData ( nIndex, DWORD( i ) );
					if( EffType == EMEFFCHAR_SINGLE )
					{
						DxEffCharSingle* pEffCharSingle = ( DxEffCharSingle* ) pEffCur;
						strCombine.Format(_T("%s %s"),pEffCur->GetName(), pEffCharSingle->GetFileName() );				
						m_EffNameList.SetItemText ( nIndex, 1, strCombine.GetString() );
					}
					else
					{
						m_EffNameList.SetItemText ( nIndex, 1, pEffCur->GetName() );
					}
					
					
					bName = TRUE;
					nIndex++;
				}
				else
				{
					m_EffNameList.InsertItem ( nIndex, _T(""), NULL );
					m_EffNameList.SetItemData ( nIndex, DWORD( i ) );
					if( EffType == EMEFFCHAR_SINGLE )
					{
						DxEffCharSingle* pEffCharSingle = ( DxEffCharSingle* ) pEffCur;
						strCombine.Format(_T("%s ( %s )"),pEffCur->GetName(), pEffCharSingle->GetFileName() );				
						m_EffNameList.SetItemText ( nIndex, 1, strCombine.GetString() );
					}
					else
					{
						m_EffNameList.SetItemText ( nIndex, 1, pEffCur->GetName() );
					}
					nIndex++;
				}
			}
		}
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
		LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
		DxAttBone *pAttBone = pView->GetAttBone();
		DxAttBoneData *pAttBoneData = pView->GetAttBoneData();

		if( !pAttBoneData )
			return;

		DxEffChar* pEffCur = NULL;
		BOOL bName = FALSE;
		INT nIndex = 0;
		CString strCombine;
		for( INT i = 0; i < ATTBONE_SIZE; i++ )
		{
			bName = FALSE;
			DxSkinPiece *pSkinPiece = pAttBoneData->GetAttBonePiece( pd3dDevice, i );
			if( !pSkinPiece )
				continue;

			GLEFFCHAR_VEC_ITER iter		= pSkinPiece->m_vecEFFECT.begin ();
			GLEFFCHAR_VEC_ITER iter_end = pSkinPiece->m_vecEFFECT.end ();
			for ( ; iter!=iter_end; )
			{
				GLEFFCHAR_VEC_ITER iter_cur = iter++;
				pEffCur = (*iter_cur);

				EFFCHAR_TYPES EffType = ( EFFCHAR_TYPES )pEffCur->GetTypeID();

				if( !bName )
				{
					m_EffNameList.InsertItem ( nIndex, DxSkinPiece::m_szPIECETYPE[i], NULL );
					m_EffNameList.SetItemData ( nIndex, DWORD( i ) );
					if( EffType == EMEFFCHAR_SINGLE )
					{
						DxEffCharSingle* pEffCharSingle = ( DxEffCharSingle* ) pEffCur;
						strCombine.Format(_T("%s %s"),pEffCur->GetName(), pEffCharSingle->GetFileName() );				
						m_EffNameList.SetItemText ( nIndex, 1, strCombine.GetString() );
					}
					else
					{
						m_EffNameList.SetItemText ( nIndex, 1, pEffCur->GetName() );
					}


					bName = TRUE;
					nIndex++;
				}
				else
				{
					m_EffNameList.InsertItem ( nIndex, _T(""), NULL );
					m_EffNameList.SetItemData ( nIndex, DWORD( i ) );
					if( EffType == EMEFFCHAR_SINGLE )
					{
						DxEffCharSingle* pEffCharSingle = ( DxEffCharSingle* ) pEffCur;
						strCombine.Format(_T("%s ( %s )"),pEffCur->GetName(), pEffCharSingle->GetFileName() );				
						m_EffNameList.SetItemText ( nIndex, 1, strCombine.GetString() );
					}
					else
					{
						m_EffNameList.SetItemText ( nIndex, 1, pEffCur->GetName() );
					}
					nIndex++;
				}
			}
		}
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
		LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
		DxVehicle *pVehicle = pView->GetVehicle();
		DxVehicleData *pVehicleData = pView->GetVehicleData();

		if( !pVehicleData )
			return;

		DxEffChar* pEffCur = NULL;
		BOOL bName = FALSE;
		INT nIndex = 0;
		CString strCombine;
		for( INT i = 0; i < PART_V_SIZE; i++ )
		{
			bName = FALSE;
			DxSkinPieceBase* pSkinPieceBase = pVehicleData->GetVehiclePart( pd3dDevice, i );
			if( !pSkinPieceBase )
				continue;

			if ( pSkinPieceBase->m_emSkinPieceType != DxSkinPieceBase::SKIN_PIECE )
				continue;

			DxSkinPiece* pSkinPiece = dynamic_cast<DxSkinPiece*>( pSkinPieceBase );

			GLEFFCHAR_VEC_ITER iter		= pSkinPiece->m_vecEFFECT.begin ();
			GLEFFCHAR_VEC_ITER iter_end = pSkinPiece->m_vecEFFECT.end ();
			for ( ; iter!=iter_end; )
			{
				GLEFFCHAR_VEC_ITER iter_cur = iter++;
				pEffCur = (*iter_cur);

				EFFCHAR_TYPES EffType = ( EFFCHAR_TYPES )pEffCur->GetTypeID();

				if( !bName )
				{
					m_EffNameList.InsertItem ( nIndex, DxSkinPiece::m_szPIECETYPE[i], NULL );
					m_EffNameList.SetItemData ( nIndex, DWORD( i ) );
					if( EffType == EMEFFCHAR_SINGLE )
					{
						DxEffCharSingle* pEffCharSingle = ( DxEffCharSingle* ) pEffCur;
						strCombine.Format(_T("%s %s"),pEffCur->GetName(), pEffCharSingle->GetFileName() );				
						m_EffNameList.SetItemText ( nIndex, 1, strCombine.GetString() );
					}
					else
					{
						m_EffNameList.SetItemText ( nIndex, 1, pEffCur->GetName() );
					}


					bName = TRUE;
					nIndex++;
				}
				else
				{
					m_EffNameList.InsertItem ( nIndex, _T(""), NULL );
					m_EffNameList.SetItemData ( nIndex, DWORD( i ) );
					if( EffType == EMEFFCHAR_SINGLE )
					{
						DxEffCharSingle* pEffCharSingle = ( DxEffCharSingle* ) pEffCur;
						strCombine.Format(_T("%s ( %s )"),pEffCur->GetName(), pEffCharSingle->GetFileName() );				
						m_EffNameList.SetItemText ( nIndex, 1, strCombine.GetString() );
					}
					else
					{
						m_EffNameList.SetItemText ( nIndex, 1, pEffCur->GetName() );
					}
					nIndex++;
				}
			}
		}
	}
}

void CEffectNamePage::ResetEffect()
{
	m_EffNameList.DeleteAllItems();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{

		DxSkinChar *pSkinChar = pView->GetSkinChar();
		DxSkinCharData *pSkinCharData = pView->GetSkinCharData();

		if( !pSkinCharData )
			return;

		DxEffChar* pEffCur = NULL;
		BOOL bName = FALSE;
		INT nIndex = 0;
		CString strCombine;
		for( INT i = 0; i < PIECE_SIZE; i++ )
		{
			bName = FALSE;
			DxSkinPieceBase *pSkinPieceBase = pSkinCharData->GetPiece( pd3dDevice, i, EMSCD_ZERO );
			if( !pSkinPieceBase )
				continue;

			if ( pSkinPieceBase->m_emSkinPieceType != DxSkinPieceBase::SKIN_PIECE )
				continue;

			DxSkinPiece *pSkinPiece = dynamic_cast<DxSkinPiece*>( pSkinPieceBase );

			pSkinPiece->m_vecEFFECT.clear();
		}

		pSkinChar->SetCharData ( pSkinCharData, pView->GetD3dDevice(), false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone *pAttBone = pView->GetAttBone();
		DxAttBoneData *pAttBoneData = pView->GetAttBoneData();

		if( !pAttBoneData )
			return;

		DxEffChar* pEffCur = NULL;
		BOOL bName = FALSE;
		INT nIndex = 0;
		CString strCombine;
		for( INT i = 0; i < ATTBONE_SIZE; i++ )
		{
			bName = FALSE;
			DxSkinPiece *pSkinPiece = pAttBoneData->GetAttBonePiece( pd3dDevice, i );
			if( !pSkinPiece )
				continue;

			pSkinPiece->m_vecEFFECT.clear();
		}

		pAttBone->SetAttBoneData ( pAttBoneData, pView->GetD3dDevice() );
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle *pVehicle = pView->GetVehicle();
		DxVehicleData *pVehicleData = pView->GetVehicleData();

		if( !pVehicleData )
			return;

		DxEffChar* pEffCur = NULL;
		BOOL bName = FALSE;
		INT nIndex = 0;
		CString strCombine;
		for( INT i = 0; i < PART_V_SIZE; i++ )
		{
			bName = FALSE;
			DxSkinPieceBase* pSkinPieceBase = pVehicleData->GetVehiclePart( pd3dDevice, i );
			if( !pSkinPieceBase )
				continue;

			if ( pSkinPieceBase->m_emSkinPieceType != DxSkinPieceBase::SKIN_PIECE )
				continue;

			DxSkinPiece* pSkinPiece = dynamic_cast<DxSkinPiece*>( pSkinPieceBase );
			pSkinPiece->m_vecEFFECT.clear();
		}

		pVehicle->SetVehiclePartData ( pVehicleData, pView->GetD3dDevice() );
	}
}

// CEffectNamePage 메시지 처리기입니다.

BOOL CEffectNamePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	RECT listRect;
	m_EffNameList.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[3] = { int(nListWidth*0.3f), int(nListWidth*1.0f) };

	m_EffNameList.SetExtendedStyle ( m_EffNameList.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_EffNameList.InsertColumn ( 0, _T("위치"),	LVCFMT_CENTER, nColWidth[0] );
	m_EffNameList.InsertColumn ( 1, _T("이펙트"),	LVCFMT_LEFT, nColWidth[1] );

	//for ( int i=0; i<PIECE_REV01; ++i )
	//{
	//	m_PieceList.InsertItem ( i, DxSkinPiece::m_szPIECETYPE[i], NULL );
	//	m_PieceList.SetItemData ( i, DWORD(i) );
	//	m_PieceList.SetItemText ( i, 1, "미지정" );
	//}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffectNamePage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinChar *pSkinChar = pView->GetSkinChar();
		m_pSheetTab->SetPages( pView->GetSkinCharData()->GetSkinAniControlData().get(), pSkinChar->m_spSkinAniControlThread.get() );
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone *pAttBone = pView->GetAttBone();
		m_pSheetTab->SetPages( NULL, pAttBone->m_spSkinAniControlThread.get() );
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle *pVehicle = pView->GetVehicle();
		m_pSheetTab->SetPages( NULL, pVehicle->m_spSkinAniControlThread.get() );
	}	
}

void CEffectNamePage::OnBnClickedButtonResetEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ResetEffect();
	SetData();
}

void CEffectNamePage::OnBnClickedButtonPartsSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	DxSkinChar *pSkinChar = pView->GetSkinChar();
	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();

	int nSelect = m_EffNameList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 )
	{
		MessageBox ( "리스트에 선택된 파츠가 없습니다.", NULL, MB_OK );
		return;
	}

	DWORD dwKey = (DWORD)m_EffNameList.GetItemData( nSelect );

	DxSkinPieceBase *pSkinPieceBase = pSkinCharData->GetPiece( pd3dDevice, dwKey, EMSCD_ZERO );
	if( !pSkinPieceBase )
		return;

	if ( pSkinPieceBase->m_emSkinPieceType != DxSkinPieceBase::SKIN_PIECE )
		return;

	DxSkinPiece *pSkinPiece = dynamic_cast<DxSkinPiece*>( pSkinPieceBase );

	CString StrFileName = _T("untitle.cps");
	CString szFilter = "Skin Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".cps",StrFileName,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkinObjPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		const char *szFileName = sFileName.GetString ();

		//	Note : 스킨 조각 세이브.
		pSkinPiece->SavePiece ( szFileName, TRUE );

		sFileName.ReleaseBuffer ();
	}
}
