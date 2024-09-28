// AnimSetPage.cpp : implementation file
//

#include "stdafx.h"
#include "../charedit.h"

#include "../../RanLogic/GLUseFeatures.h"

#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/DxCommon/d3dapp.h"
#include "../EngineLib/DxCommon/dxutil.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "SheetWithTabAnim.h"

#include "AttBoneAniEditPage.h"
#include "AnimSetPage.h"
#include ".\animsetpage.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CAnimSetPage property page

IMPLEMENT_DYNCREATE(CAnimSetPage, CPropertyPage)

CAnimSetPage::CAnimSetPage() :
	CPropertyPage(CAnimSetPage::IDD),
	m_pSheetTab(NULL)
{
	//{{AFX_DATA_INIT(CAnimSetPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAnimSetPage::~CAnimSetPage()
{
}

void CAnimSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnimSetPage)
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ComboType);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_SUBTYPE, m_ComboSubType);
	DDX_Control(pDX, IDC_LIST_ATTANIMATION, m_List_AttAni);
}


BEGIN_MESSAGE_MAP(CAnimSetPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAnimSetPage)
	ON_BN_CLICKED(IDOK, OnButtonOk)
	ON_BN_CLICKED(IDOK3, &CAnimSetPage::OnBnClickedDontSaveOK)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_LOOP, OnBnClickedCheckLoop)	
	ON_BN_CLICKED(IDC_BUTTON_SOUND, OnBnClickedButtonSound)
	ON_BN_CLICKED(IDC_BUTTON_ANIDIV, OnBnClickedButtonAnidiv)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_BUTTON_STRIKE, OnBnClickedButtonStrike)
	ON_BN_CLICKED(IDC_BUTTON_ANIEFF, OnBnClickedButtonAnieff)
	ON_BN_CLICKED(IDC_CHECK_UPBODY, OnBnClickedCheckUpbody)
	ON_BN_CLICKED(IDC_BUTTON_ANI_SCALE, OnBnClickedButtonAniScale)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ATTANIMATION, OnNMDblclkListAttanimation)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_CFG, &CAnimSetPage::OnBnClickedButtonImportCfg)
	ON_BN_CLICKED(IDC_BUTTON_XFILE_CHANGE, &CAnimSetPage::OnBnClickedButtonXfileChange)
	ON_BN_CLICKED(IDC_BUTTON_ANI_JIGGLE_BLEND, OnBnClickedButtonAniJiggleBlend)
	ON_BN_CLICKED(IDC_CHECK_MOVE, &CAnimSetPage::OnBnClickedCheckMove)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimSetPage message handlers

void CAnimSetPage::SetAnimName ( CString sName )
{

	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");

	m_sAnimName = sName;

	CWnd *pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->SetWindowText ( m_sAnimName );

	const ANIMCONTNODE *pAnimNodeCur = m_pSheetTab->GetSkinAniContrl()->findanicontnode ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	const DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

	m_List_AttAni.DeleteAllItems();

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR || CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{

		const SAttachBoneAnimInfo *pAttachBoneAnimInfo = pAnimCont->GetAttachBoneAnimInfo();
		UpdateAttachAnim( pAttachBoneAnimInfo );

		m_List_AttAni.EnableWindow( TRUE );
	}else{
		m_List_AttAni.EnableWindow( FALSE );
	}


	if ( pAnimCont )
	{
		pWnd = GetDlgItem( IDC_EDIT_NAME_XFILE );
		pWnd->SetWindowText( pAnimCont->m_strXFile.c_str() );

		if( pAnimCont->m_UNITTIME == 0 )
		{
			AfxMessageBox( _T("m_UNITTIME 가 제로 입니다.. 문제 보임.") );
			CString EditString;
			EditString.Format ( "0" );
			GetDlgItem(IDC_EDIT_START)->SetWindowText(EditString);

			EditString.Format ( "0" );
			GetDlgItem(IDC_EDIT_END)->SetWindowText(EditString);

			EditString.Format ( "0" );
			GetDlgItem(IDC_STATIC_ANIEND)->SetWindowText(EditString);
		}
		else
		{
			CString EditString;
			EditString.Format ( "%d", pAnimCont->m_dwSTime/pAnimCont->m_UNITTIME );
			GetDlgItem(IDC_EDIT_START)->SetWindowText(EditString);

			EditString.Format ( "%d", pAnimCont->m_dwETime/pAnimCont->m_UNITTIME );
			GetDlgItem(IDC_EDIT_END)->SetWindowText(EditString);

			EditString.Format ( "%d", pAnimCont->m_dwETimeOrig/pAnimCont->m_UNITTIME );
			GetDlgItem(IDC_STATIC_ANIEND)->SetWindowText(EditString);
		}
	
		CButton *pButton;
		pButton = (CButton*) GetDlgItem ( IDC_CHECK_LOOP );
		pButton->SetCheck ( pAnimCont->m_dwFlag&ACF_LOOP );

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_MOVE );
		pButton->SetCheck ( pAnimCont->m_dwFlag&ACF_MOVE );

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_UPBODY );
		pButton->SetCheck ( pAnimCont->m_dwFlag&ACF_UPBODY );
	}

	DWORD nIndex;
	m_ComboType.ResetContent();

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		for ( DWORD i=0; i<AN_TYPE_SIZE; ++i )
		{
			nIndex = m_ComboType.AddString(COMMENT::GetAnimMainType(i));
			m_ComboType.SetItemData ( nIndex, i );
		}
		m_ComboSubType.ShowWindow(TRUE);
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		for ( DWORD i=0; i<AN_ATTBONE_SIZE; ++i )
		{
			// 아래의 COMMENT는 데이터에서 읽어오지않는다;
			nIndex = m_ComboType.AddString ( COMMENT::ANI_ATTBONE_MAINTYPE[i].c_str() );
			m_ComboType.SetItemData ( nIndex, i );
		}
		m_ComboSubType.ShowWindow(FALSE);
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		for ( DWORD i=0; i<AN_VEHICLE_SIZE; ++i )
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				nIndex = m_ComboType.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_VEHICLE_MAINTYPE",i).GetString());
			else
			nIndex = m_ComboType.AddString ( COMMENT::ANI_VEHICLE_MAINTYPE[i].c_str() );
			m_ComboType.SetItemData ( nIndex, i );
		}
		m_ComboSubType.ShowWindow(FALSE);
	}

	SetWin_Combo_Sel ( this, IDC_COMBO_TYPE, pAnimCont->m_MainType );

	m_ComboSubType.ResetContent();

	switch ( pAnimCont->m_MainType )
	{
	case AN_SKILL:
	case AN_SKILL_B:
	case AN_SKILL_C:
	case AN_SKILL_D:
	case AN_SKILL_E:
	case AN_SKILL_F:
    case AN_SKILL_G:
    case AN_SKILL_H:
	case AN_GESTURE:
		{
			for ( DWORD i=0; i<AN_SUB_00_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = m_ComboSubType.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_00", i).GetString());
				else
					nIndex = m_ComboSubType.AddString ( COMMENT::ANI_SUBTYPE_00[i].c_str() );

				m_ComboSubType.SetItemData ( nIndex, i );
				if ( pAnimCont->m_SubType == i )	m_ComboSubType.SetCurSel ( nIndex );
			}
		}
		break;
	case AN_SPEC:
		{
			for ( DWORD i=0; i<AN_SUB_00_SIZE; ++i )
			{
				// 아래의 COMMENT는 데이터에서 읽어오지 않는다;
				nIndex = m_ComboSubType.AddString ( COMMENT::ANI_SUBTYPE_02[i].c_str() );
				m_ComboSubType.SetItemData ( nIndex, i );

				if ( pAnimCont->m_SubType == i )	m_ComboSubType.SetCurSel ( nIndex );
			}
		}
		break;
	case AN_GATHERING:
		{
			for ( DWORD i=0; i<AN_SUB_00_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = m_ComboSubType.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_01", i).GetString());
				else
					nIndex = m_ComboSubType.AddString ( COMMENT::ANI_SUBTYPE_01[i].c_str() );
				m_ComboSubType.SetItemData ( nIndex, i );

				if ( pAnimCont->m_SubType == i )	m_ComboSubType.SetCurSel ( nIndex );
			}
		}
		break;
	case AN_BIKE_A:
	case AN_BIKE_B:
	case AN_BIKE_C:
	case AN_BIKE_D:
	case AN_CAR_A:
	case AN_CAR_B:
	case AN_CAR_C:
	case AN_CAR_D:
	case AN_REARCAR_A:
	case AN_REARCAR_B:
	case AN_REARCAR_C:
	case AN_REARCAR_D:
		{
			for ( DWORD i=0; i<AN_SUB_00_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = m_ComboSubType.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_03", i).GetString());
				else
					nIndex = m_ComboSubType.AddString ( COMMENT::ANI_SUBTYPE_03[i].c_str() );
				m_ComboSubType.SetItemData ( nIndex, i );

				if ( pAnimCont->m_SubType == i )	m_ComboSubType.SetCurSel ( nIndex );
			}
		}
		break;
	case AN_FLY:
		{
			for ( DWORD i=0; i<AN_SUB_FLY_SIZE; ++i )
			{
				// 아래의 COMMENT는 데이터에서 읽어오지 않는다;
				nIndex = m_ComboSubType.AddString ( COMMENT::ANI_SUBTYPE_FLY[i].c_str() );
				m_ComboSubType.SetItemData ( nIndex, i );

				if ( pAnimCont->m_SubType == i )	m_ComboSubType.SetCurSel ( nIndex );
			}
		}
		break;
	default:
		{
			for ( DWORD i=0; i<AN_SUB_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = m_ComboSubType.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE", i).GetString());
				else
					nIndex = m_ComboSubType.AddString ( COMMENT::ANI_SUBTYPE[i].c_str() );
				m_ComboSubType.SetItemData ( nIndex, i );

				if ( pAnimCont->m_SubType == i )	m_ComboSubType.SetCurSel ( nIndex );
			}
		}
		break;
	};
}

void CAnimSetPage::UpdateAttachAnim( const SAttachBoneAnimInfo *pAttachBoneAnimInfo )
{
	m_List_AttAni.DeleteAllItems();
	CString strTemp;

	int		     nMaxSize          = 0;
	const char (*ppstrBuffer)[64] = NULL;

	switch ( CCharEditView::m_emObjType )
	{
	case EOBJ_SKINCHAR: 
		nMaxSize    = PIECE_SAVE_SIZE;
		ppstrBuffer = DxSkinPiece::m_szPIECETYPE;
		break;
	case EOBJ_VEHICLE:	
		nMaxSize    = PART_V_SIZE;
		ppstrBuffer = DxSkinPiece::m_szV_PARTSTYPE;
		break;
	}

	for( int i = 0; i < nMaxSize; i++ )
	{
		m_List_AttAni.InsertItem( i, ppstrBuffer[i], NULL );

		if( pAttachBoneAnimInfo[i].bUseStrikeAni )
		{
			m_List_AttAni.SetItemText( i, 1, "타격 이펙트 사용" );
		}else{
			SAttachBoneAnimBasicInfo sBasicInfo = pAttachBoneAnimInfo[i].sBasicAni;
			// 아래의 COMMENT는 데이터에서 읽어오지 않는다;
			m_List_AttAni.SetItemText( i, 1, COMMENT::ANI_ATTBONE_MAINTYPE[sBasicInfo.emAttAniType].c_str() );
			strTemp.Format( "%d", sBasicInfo.nStartAnim );
			m_List_AttAni.SetItemText( i, 2, strTemp );
		}
	}

}

void CAnimSetPage::OnBnClickedCheckLoop()
{
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");

	DxSkinAnimation *pAnimcont = m_pSheetTab->GetSkinAniContrl()->findanicont_none_const( m_sAnimName.GetString() );

	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_LOOP );
	
	if ( pButton->GetCheck () ) pAnimcont->m_dwFlag |= ACF_LOOP;
	else						pAnimcont->m_dwFlag &= ~ACF_LOOP;

	m_pSheetTab->ApplyAniContrl ();
}

void CAnimSetPage::OnBnClickedCheckMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DxSkinAnimation *pAnimcont = m_pSheetTab->GetSkinAniContrl()->findanicont_none_const( m_sAnimName.GetString() );

	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_MOVE );

	if ( pButton->GetCheck () ) pAnimcont->m_dwFlag |= ACF_MOVE;
	else						pAnimcont->m_dwFlag &= ~ACF_MOVE;

	m_pSheetTab->ApplyAniContrl ();
}

void CAnimSetPage::OnButtonOk() 
{
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");

	DWORD dwUnitTime;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	const char* pName = m_sAnimName.GetString();
	DxSkinAnimation *pAnimcont = m_pSheetTab->GetSkinAniContrl()->findanicont_none_const( pName );

	pAnimcont->m_MainType = (EMANI_MAINTYPE) m_ComboType.GetCurSel();
	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		pAnimcont->m_SubType = (EMANI_SUBTYPE) m_ComboSubType.GetCurSel();
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		pAnimcont->m_SubType = AN_SUB_NONE;
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		pAnimcont->m_SubType = AN_SUB_NONE;
	}

	CWnd *pWnd;
	CString Str;

	pWnd = GetDlgItem ( IDC_EDIT_START );
	pWnd->GetWindowText ( Str );
	int nAniStart = atoi ( Str.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_END );
	pWnd->GetWindowText ( Str );
	int nAniEnd = atoi ( Str.GetString() );

	if ( nAniStart < nAniEnd )
	{
		pAnimcont->m_dwSTime = nAniStart * pAnimcont->m_UNITTIME;
		pAnimcont->m_dwETime = nAniEnd * pAnimcont->m_UNITTIME;

		if ( pAnimcont->m_dwETime > pAnimcont->m_dwETimeOrig )
			pAnimcont->m_dwETime = pAnimcont->m_dwETimeOrig;
	}

	m_pSheetTab->ApplyAniContrl ();
	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		pView->GetSkinChar()->m_spSkinAniControlThread->SELECTANI_EDIT( pName );
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		pView->GetAttBone()->m_spSkinAniControlThread->SELECTANI_EDIT( pName );
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		pView->GetVehicle()->m_spSkinAniControlThread->SELECTANI_EDIT( pName );
	}

	m_sAnimName.ReleaseBuffer();

	const DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM();
	if ( !pSelectedAniCont )
		goto to_exit;

	dwUnitTime = pSelectedAniCont->m_UNITTIME;
	if ( dwUnitTime > 0 && pAnimcont == pSelectedAniCont )
	{
		if ( m_pSheetTab->GetSkinAniContrl()->GETCURTIME() < pAnimcont->m_dwSTime )
			m_pSheetTab->GetSkinAniContrl()->SETCURTIME ( (float)pAnimcont->m_dwSTime );

		pFrame->SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
			pSelectedAniCont->m_dwETimeOrig/dwUnitTime, dwUnitTime );
	}




	//////////////////////////////////////////////////////////////////////////
	// m_vAnimationMoveOffset 을 얻기 위해 작업됨.
	//////////////////////////////////////////////////////////////////////////

	pAnimcont->m_vAnimationMoveOffset = D3DXVECTOR3( 0.f, 0.f, 0.f );

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		if ( pView->GetSkinChar()->IsAnimationMove() )
		{
			D3DXMATRIX matIdentity;
			D3DXMatrixIdentity( &matIdentity );
			pView->GetSkinChar()->m_spSkinAniControlThread->SETCURTIME_ALL_FOR_EDIT( pAnimcont->m_dwETime - pAnimcont->m_UNITTIME );
			pView->GetSkinChar()->FrameMove( 0.f, 0.0f, TRUE, matIdentity );

			//////////////////////////////////////////////////////////////////////////
			// NSSkinAniThread
			NSSkinAniThread::CalcluateSkinAniControlForCharEditer( 0.f );

			pAnimcont->m_vAnimationMoveOffset = pView->GetSkinChar()->GetAnimationMoveOffsetFull_EDIT();
		}
	}

	//////////////////////////////////////////////////////////////////////////




	//	Note : 에니메이션 설정을 파일에 저장.
	// 
	// DxSkinAnimationManager::GetInstance().SaveAnimConInfo( pAnimcont->m_strXFile.c_str(), pAnimcont->m_strCfgFile.c_str() );
	DxSkinAnimationManager::GetInstance().SaveAnimConInfo( pAnimcont->m_strXFile, pAnimcont->m_strCfgFile );

to_exit:
	m_pSheetTab->SetActivePage ( 0 );
}

void CAnimSetPage::OnBnClickedDontSaveOK()
{
	m_pSheetTab->SetActivePage ( 0 );
}

void CAnimSetPage::OnBnClickedButtonSound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	const ANIMCONTNODE *pAnimNodeCur = m_pSheetTab->GetSkinAniContrl()->findanicontnode ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;

	m_pSheetTab->SoundSetActive ( &pAnimNodeCur->m_pDxSkinAni->m_ChaSoundData );
}

void CAnimSetPage::OnBnClickedButtonAnidiv()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->AniDivActive ( m_sAnimName );
}

void CAnimSetPage::OnCbnSelchangeComboType()
{
	if( CCharEditView::m_emObjType != EOBJ_SKINCHAR ) return;

	int nIndex = m_ComboType.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	EMANI_MAINTYPE MainType = (EMANI_MAINTYPE) m_ComboType.GetCurSel();

	const DxSkinAnimation *pAnimCont = m_pSheetTab->GetSkinAniContrl()->findanicont ( m_sAnimName.GetString() );

	m_ComboSubType.ResetContent();

	switch ( MainType )
	{
	case AN_SKILL:
	case AN_SKILL_B:
	case AN_SKILL_C:
	case AN_SKILL_D:
	case AN_SKILL_E:
	case AN_SKILL_F:
    case AN_SKILL_G:
    case AN_SKILL_H:
	case AN_GESTURE:
	case AN_SPEC:
		{
			for ( DWORD i=0; i<AN_SUB_00_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = m_ComboSubType.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_00", i).GetString());
				else
					nIndex = m_ComboSubType.AddString ( COMMENT::ANI_SUBTYPE_00[i].c_str() );

				m_ComboSubType.SetItemData ( nIndex, i );
				if ( pAnimCont->m_SubType == i )	m_ComboSubType.SetCurSel ( nIndex );
			}
		}
		break;
	case AN_GATHERING:
		{
			for ( DWORD i=0; i<AN_SUB_00_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = m_ComboSubType.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_01", i).GetString());
				else
					nIndex = m_ComboSubType.AddString ( COMMENT::ANI_SUBTYPE_01[i].c_str() );
				m_ComboSubType.SetItemData ( nIndex, i );

				if ( pAnimCont->m_SubType == i )	m_ComboSubType.SetCurSel ( nIndex );
			}
		}
		break;
	case AN_FLY:
		{
			for ( DWORD i=0; i<AN_SUB_FLY_SIZE; ++i )
			{
				// 아래의 COMMENT는 데이터에서 읽어오지 않는다;
				nIndex = m_ComboSubType.AddString ( COMMENT::ANI_SUBTYPE_FLY[i].c_str() );
				m_ComboSubType.SetItemData ( nIndex, i );

				if ( pAnimCont->m_SubType == i )	m_ComboSubType.SetCurSel ( nIndex );
			}
		}
		break;
	case AN_BIKE_A:
	case AN_BIKE_B:
	case AN_BIKE_C:
	case AN_BIKE_D:
	case AN_CAR_A:
	case AN_CAR_B:
	case AN_CAR_C:
	case AN_CAR_D:
	case AN_REARCAR_A:
	case AN_REARCAR_B:
	case AN_REARCAR_C:
	case AN_REARCAR_D:
		{
			for ( DWORD i=0; i<AN_SUB_00_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = m_ComboSubType.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_03", i).GetString());
				else
					nIndex = m_ComboSubType.AddString ( COMMENT::ANI_SUBTYPE_03[i].c_str() );
				m_ComboSubType.SetItemData ( nIndex, i );

				if ( pAnimCont->m_SubType == i )	m_ComboSubType.SetCurSel ( nIndex );
			}
		}
		break;
	default:
		{
			for ( DWORD i=0; i<AN_SUB_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = m_ComboSubType.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE", i).GetString());
				else
					nIndex = m_ComboSubType.AddString ( COMMENT::ANI_SUBTYPE[i].c_str() );
				m_ComboSubType.SetItemData ( nIndex, i );

				if ( pAnimCont->m_SubType == i )	m_ComboSubType.SetCurSel ( nIndex );
			}
		}
		break;
	};
}

void CAnimSetPage::OnBnClickedButtonStrike()
{
	m_pSheetTab->AniStrike ( m_sAnimName );
}



void CAnimSetPage::OnBnClickedButtonAnieff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->AniEff ( m_sAnimName );
}

void CAnimSetPage::OnBnClickedCheckUpbody()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	assert(m_pSheetTab&&_T("SheetTab의 포인터가 지정되지 않음.") );
	assert(m_pSheetTab->GetSkinAniContrl()&&_T("Skin Animation Control의 포인터가 지정되지 않음.") );

	DxSkinAnimation *pAnimcont = m_pSheetTab->GetSkinAniContrl()->findanicont_none_const( m_sAnimName.GetString() );

	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_UPBODY );
	
	if ( pButton->GetCheck () ) pAnimcont->m_dwFlag |= ACF_UPBODY;
	else						pAnimcont->m_dwFlag &= ~ACF_UPBODY;

	m_pSheetTab->ApplyAniContrl ();
}

void CAnimSetPage::OnBnClickedButtonAniScale()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->m_cDlgEdit.SetAcitveSheetPage( CHARTAB_ANIBONESCALE );
	pFrame->m_cDlgEdit.GetSheetChar()->m_AniBoneScale.SetInstance( m_sAnimName, m_pSheetTab );
}

void CAnimSetPage::OnNMDblclkListAttanimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION	pos = m_List_AttAni.GetFirstSelectedItemPosition ();	
	int nIndex = m_List_AttAni.GetNextSelectedItem ( pos );	
	if( nIndex == -1 ) return;

	ANIMCONTNODE *pAnimNodeCur = m_pSheetTab->GetSkinAniContrl()->findanicontnode_none_const( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	SAttachBoneAnimInfo *pAttachBoneAnimInfo = pAnimCont->m_sAttBoneAnimInfo;

	CAttBoneAniEditPage AttBoneAniEditPage( CCharEditView::m_emObjType, nIndex, pAttachBoneAnimInfo[nIndex], m_sAnimName, m_pSheetTab->GetSkinAniContrl() );
	if( AttBoneAniEditPage.DoModal() == IDOK )
	{
		pAttachBoneAnimInfo[nIndex] = AttBoneAniEditPage.GetAttachBoneAnimInfo();
		UpdateAttachAnim( pAttachBoneAnimInfo );
	}

	*pResult = 0;
}

BOOL CAnimSetPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_List_AttAni.SetExtendedStyle ( m_List_AttAni.GetExtendedStyle() |	LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	RECT listRect;
	m_List_AttAni.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[3] = { int(nListWidth*0.4f), int(nListWidth*0.38f), int(nListWidth*0.2f) };

	m_List_AttAni.InsertColumn ( 0, _T("종류"),	LVCFMT_CENTER, nColWidth[0] );
	m_List_AttAni.InsertColumn ( 1, _T("연결동작"),	LVCFMT_CENTER, nColWidth[1] );
	m_List_AttAni.InsertColumn ( 2, _T("시작"),	LVCFMT_CENTER, nColWidth[2] );
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAnimSetPage::OnBnClickedButtonImportCfg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice();


	//	Note : 파일 필터를 정의한다.
	CString szFilter = _T("Skin Animation cfg Filw |*.cfg|");

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,_T(".cfg"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinAnimationManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		CString Str;
		CWnd* pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->GetWindowText( Str );

		ANIMCONTNODE *pAnimNodeDEST = m_pSheetTab->GetSkinAniContrl()->findanicontnode_none_const( Str.GetString() );
		if ( !pAnimNodeDEST )	
			return;

		pAnimNodeDEST->Import_OnlySetting( pd3dDevice, sFileName );

		// Interface 정리
		SetAnimName( Str.GetString() );
	}
}

void CAnimSetPage::OnBnClickedButtonXfileChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice();


	//	Note : 파일 필터를 정의한다.
	CString szFilter = _T("Skin Animation xFile |*.X|");

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,_T(".x"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinAnimationManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		//sFileName.MakeLower ();

		CWnd* pWnd = GetDlgItem( IDC_EDIT_NAME_XFILE );
		pWnd->SetWindowText( sFileName );

		CString Str;
		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->GetWindowText( Str );

		ANIMCONTNODE *pAnimNodeCur = m_pSheetTab->GetSkinAniContrl()->findanicontnode_none_const( Str.GetString() );
		if ( !pAnimNodeCur )	return;
		DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

		if( pAnimCont )
		{
			pAnimCont->ChangeXfile( pd3dDevice, sFileName.GetString() );
		}
	}
}

void CAnimSetPage::OnBnClickedButtonAniJiggleBlend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->m_cDlgEdit.SetAcitveSheetPage( CHARTAB_ANIJIGGLEBLEND );
	pFrame->m_cDlgEdit.GetSheetChar()->m_AniJiggleBlend.SetInstance( m_sAnimName, m_pSheetTab );
}
