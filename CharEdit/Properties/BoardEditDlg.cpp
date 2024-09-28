// BoardEditDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../charedit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxVehicleThreadNeedData.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinMeshManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../EngineLib/DxEffect/Char/dxeffchardata.h"
#include "../EngineLib/DxTools/DxViewPort.h"

#include "SheetWithTabChar.h"
#include "SheetWithTabFrame.h"
#include "SheetWithTabBone.h"
#include "SheetWithTabAnim.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "BoardEditDlg.h"
#include ".\boardeditdlg.h"

extern std::string	g_strPICK_BONE;
extern bool			g_bViewPassenger;


// CBoardEditDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CBoardEditDlg, CPropertyPage)
CBoardEditDlg::CBoardEditDlg()
	: CPropertyPage(CBoardEditDlg::IDD)
{
}

CBoardEditDlg::~CBoardEditDlg()
{
}

void CBoardEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EDITBONE, m_list_Passenger);
	DDX_Control(pDX, IDC_LIST_EDITCHAR, m_list_TestChar);
	DDX_Control(pDX, IDC_LIST_CHARANIM, m_list_TestAnim);
}


BEGIN_MESSAGE_MAP(CBoardEditDlg, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_CHARTEST, OnBnClickedCheckChartest)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_RADIO_POSITION, OnBnClickedRadioPosition)
	ON_BN_CLICKED(IDC_RADIO_ROTATION, OnBnClickedRadioRotation)
	ON_BN_CLICKED(IDC_BUTTON_GETBONE, OnBnClickedButtonGetbone)
	ON_BN_CLICKED(IDC_BUTTON_LOADTESTCHF, OnBnClickedButtonLoadtestchf)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_EDITBONE, OnNMDblclkListEditbone)
	ON_BN_CLICKED(IDC_BUTTON_TESTANIM, OnBnClickedButtonTestanim)
	ON_NOTIFY(NM_CLICK, IDC_LIST_EDITCHAR, OnNMClickListEditchar)
	//ON_BN_CLICKED(IDC_BUTTON_LOADTESTCHF2, OnBnClickedButtonLoadtestchf2)
	ON_BN_CLICKED(IDC_BUTTON_RESET1, OnBnClickedButtonReset1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VALUE2, OnDeltaposSpinValue2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VALUE1, OnDeltaposSpinValue1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VALUE3, OnDeltaposSpinValue3)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_LIST_EDITBONE, &CBoardEditDlg::OnNMClickListEditbone)
	ON_EN_CHANGE(IDC_EDIT_B_VALUE1, &CBoardEditDlg::OnEnChangeEditBValue1)
	ON_EN_CHANGE(IDC_EDIT_B_VALUE2, &CBoardEditDlg::OnEnChangeEditBValue2)
	ON_EN_CHANGE(IDC_EDIT_B_VALUE3, &CBoardEditDlg::OnEnChangeEditBValue3)
END_MESSAGE_MAP()


// CBoardEditDlg �޽��� ó�����Դϴ�.

BOOL CBoardEditDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	RECT listRect;
	m_list_Passenger.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );

	int nColWidth[2] = { int(nListWidth*0.5f), int(nListWidth*0.49f) };

	m_list_Passenger.SetExtendedStyle ( m_list_Passenger.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_list_Passenger.InsertColumn ( 0, _T("Seat"),	LVCFMT_CENTER, nColWidth[0] );
	m_list_Passenger.InsertColumn ( 1, _T("Bone"),	LVCFMT_CENTER, nColWidth[1] );

	int nColWidth2[3] = { int(nListWidth*0.2f), int(nListWidth*0.4f), int(nListWidth*0.4f) };

	m_list_TestChar.SetExtendedStyle( m_list_Passenger.GetExtendedStyle() );
	m_list_TestChar.InsertColumn ( 0, _T("Seat"),	LVCFMT_CENTER, nColWidth2[0] );
	m_list_TestChar.InsertColumn ( 1, _T("CHF"),	LVCFMT_CENTER, nColWidth2[1] );
	m_list_TestChar.InsertColumn ( 2, _T("Anim"),	LVCFMT_CENTER, nColWidth2[2] );

	//m_nEditPassenger = -1;

	SetTimer( 0, 1, NULL );

	SetWin_Num_float( this, IDC_EDIT_TERM, 1.0f );

	// �ʱ�ȭ
	{
		CString strPassenger[5];
		strPassenger[0] = "Driver";
		strPassenger[1] = "Passenger01";
		strPassenger[2] = "Passenger02";
		strPassenger[3] = "Passenger03";
		strPassenger[4] = "Passenger04";

		CString strTemp;
		m_list_Passenger.DeleteAllItems();
		for( DWORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
		{
			m_list_Passenger.InsertItem( i, strPassenger[i].GetString() );
		}

		strPassenger[0] = "Driv";
		strPassenger[1] = "Pass1";
		strPassenger[2] = "Pass2";
		strPassenger[3] = "Pass3";
		strPassenger[4] = "Pass4";

		m_list_TestChar.DeleteAllItems();
		for( DWORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
		{
			m_list_TestChar.InsertItem( i, strPassenger[i].GetString() );
		}
	}

	UpdatePassengerDataList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CBoardEditDlg::ResetPage ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();
}

void CBoardEditDlg::OnBnClickedButtonGetbone()
{
	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		MessageBox( "����Ʈ���� ����Ʈ�� ž���ڸ� ���ϼ���", "�˸�", MB_OK );
		return;
	}

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame	  *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();

	CWnd* pWnd = GetDlgItem( IDC_EDIT_BONENAME );
	pWnd->SetWindowText( g_strPICK_BONE.c_str() );

	const DxSkeleton    *pSkeleton = pVehicle->m_spSkinAniControlThread->GetSkeletonForEdit();

	D3DXMATRIX matBase;
	D3DXMatrixIdentity( &matBase );
	if( pSkeleton )
	{
		const DxBoneTrans* pBoneTran = pSkeleton->FindBone_t( g_strPICK_BONE.c_str() );
		if( pBoneTran )
		{
			pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].Init();
			pVehicleData->m_sPassengerData[nIndex].Init();

			pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].strBoneName = g_strPICK_BONE.c_str();
			pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].matWorld    = pBoneTran->matCombined;			

			pVehicleData->m_sPassengerData[nIndex].strBoneName = g_strPICK_BONE.c_str();
			pVehicleData->m_sPassengerData[nIndex].matWorld    = pBoneTran->matCombined;

			CheckRadioButton( IDC_RADIO_POSITION, IDC_RADIO_SCALE, IDC_RADIO_POSITION );
		}
	}

	UpdateValueData();
	//UpdatePassengerDataList();
}

void CBoardEditDlg::UpdatePassengerDataList()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();


	CString strTemp;
	for( DWORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
	{
		//m_list_Passenger.InsertItem( i, strPassenger[i].GetString() );
		m_list_Passenger.SetItemText ( i, 1, pVehicleData->m_sPassengerData[i].strBoneName.c_str() );
	}


	for( DWORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
	{
		DxSkinChar *pSkinPassenger = pView->GetSkinPassenger(i);
		//m_list_TestChar.InsertItem( i, strPassenger[i].GetString() );
		m_list_TestChar.SetItemText ( i, 1, pSkinPassenger->GetFileName() );
		
		if ( pSkinPassenger->m_spSkinAniControlThread->GETCURANIM() ) 
			m_list_TestChar.SetItemText ( i, 2, pSkinPassenger->m_spSkinAniControlThread->GETCURANIM()->m_strCfgFile.c_str() );
		else
			m_list_TestChar.SetItemText( i, 2, "" );
	}
}


void CBoardEditDlg::OnBnClickedCheckChartest()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    g_bViewPassenger = GetWin_Check( this, IDC_CHECK_CHARTEST ) ? true : false;
	
}

void CBoardEditDlg::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame	  *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();

	for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
	{
		pVehicleData->m_sPassengerData[i] = pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[i];
		
	}



	CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
	DlgEdit.GetSheetChar()->m_VehiclePage.SetBoardEditTab(FALSE);
}

void CBoardEditDlg::OnBnClickedRadioPosition()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	
	UpdateValueData();
}

void CBoardEditDlg::OnBnClickedRadioRotation()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	UpdateValueData();
}



void CBoardEditDlg::OnBnClickedButtonLoadtestchf()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos = m_list_TestChar.GetFirstSelectedItemPosition();
	int nIndex = m_list_TestChar.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		MessageBox( "����Ʈ���� ����Ʈ�� ž���ڸ� ���ϼ���", "�˸�", MB_OK );
		return;
	}
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	DxSkinChar *pSkinPassenger = pView->GetSkinPassenger(nIndex);

	

	CString szFilter = "Charactor File (*.chf)|*.chf|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".chf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CFramePage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		pSkinPassenger->ClearAll();

		DxSkinCharData* pSkinCharData = new DxSkinCharData;
		if (pSkinCharData->LoadFile(dlg.GetFileName().GetString(), pd3dDevice, false ))
		{
			pSkinPassenger->SetCharData(pSkinCharData, pd3dDevice, false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
		}
		SAFE_DELETE(pSkinCharData);
	}


	UpdatePassengerDataList();
}


void CBoardEditDlg::OnNMDblclkListEditbone(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();

	//OnBnClickedButtonSetedit

	*pResult = 0;
}

void CBoardEditDlg::OnBnClickedButtonTestanim()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos = m_list_TestChar.GetFirstSelectedItemPosition();
	int nIndex = m_list_TestChar.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		MessageBox( "����Ʈ���� ž���ڸ� ���ϼ���", "�˸�", MB_OK );
		return;
	}

	int nIndex2 = m_list_TestAnim.GetCurSel();
	if( nIndex2 == LB_ERR )
	{
		MessageBox( "����Ʈ���� �ִϸ��̼��� ���ϼ���", "�˸�", MB_OK );
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinChar *pSkinPassenger = pView->GetSkinPassenger(nIndex);

	CString sItemText;
	m_list_TestAnim.GetText ( nIndex2, sItemText );
	pSkinPassenger->SELECTANI_EDIT( sItemText.GetString() );
	pSkinPassenger->SETCURTIME( 0.0f );


	//pSkinPassenger->FrameMove( 0.0f, 0.0f,  );

	UpdatePassengerDataList();
}

void CBoardEditDlg::OnNMClickListEditchar(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos = m_list_TestChar.GetFirstSelectedItemPosition();
	int nIndex = m_list_TestChar.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		MessageBox( "����Ʈ���� ž���ڸ� ���ϼ���", "�˸�", MB_OK );
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinChar *pSkinPassenger = pView->GetSkinPassenger(nIndex);

	m_list_TestAnim.ResetContent();

	const DxSkinAniControl::VECNODE& vecANI = pSkinPassenger->m_spSkinAniControlThread->GetAniList ();
	for ( size_t n=0; n<vecANI.size(); ++n )
	{
		m_list_TestAnim.AddString ( vecANI[n]->m_pDxSkinAni->m_strCfgFile.c_str() );
	}


	*pResult = 0;
}

//void CBoardEditDlg::OnBnClickedButtonSetedit()
//{
//	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
//	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
//	if( nIndex == LB_ERR )
//	{
//		MessageBox( "����Ʈ���� ����Ʈ�� ž���ڸ� ���ϼ���", "�˸�", MB_OK );
//		return;
//	}
//
//	SetWin_Num_int( this, IDC_EDIT_SETNUM, nIndex );
//	m_nEditPassenger = nIndex;
//
//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
//	DxVehicle	  *pVehicle = pView->GetVehicle();
//
//	SetWin_Text( this,  IDC_EDIT_BONENAME, pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].strBoneName.c_str() );
//	CheckRadioButton( IDC_RADIO_POSITION, IDC_RADIO_SCALE, IDC_RADIO_POSITION );
//
//	UpdateValueData();
//}
//
//void CBoardEditDlg::OnBnClickedButtonLoadtestchf2()
//{
//	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	POSITION pos = m_list_TestChar.GetFirstSelectedItemPosition();
//	int nIndex = m_list_TestChar.GetNextSelectedItem(pos);
//	if( nIndex == LB_ERR )
//	{
//		MessageBox( "����Ʈ���� ����Ʈ�� ž���ڸ� ���ϼ���", "�˸�", MB_OK );
//		return;
//	}
//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
//	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
//	DxSkinChar *pSkinPassenger = pView->GetSkinPassenger(nIndex);
//	CString strName = pSkinPassenger->m_szFileName;
//
//	pSkinPassenger->ClearAll();
//
//	DxSkinCharData* pSkinCharData = new DxSkinCharData;
//	if( pSkinCharData->LoadFile ( strName.GetString(), pd3dDevice, false ) )
//	{
//		pSkinPassenger->SetCharData( pSkinCharData, pd3dDevice, false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
//	}
//	SAFE_DELETE( pSkinCharData );
//
//
//	UpdatePassengerDataList();
//}

void CBoardEditDlg::UpdateValueData( bool bSet /* = TRUE */ )
{
	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();

	if( !pVehicle || !pVehicleData ) return;

	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		if( bSet )
		{
			SetWin_Num_float( this, IDC_EDIT_B_VALUE1, pVehicleData->m_sPassengerData[nIndex].vPos.x );
			SetWin_Num_float( this, IDC_EDIT_B_VALUE2, pVehicleData->m_sPassengerData[nIndex].vPos.y );
			SetWin_Num_float( this, IDC_EDIT_B_VALUE3, pVehicleData->m_sPassengerData[nIndex].vPos.z );
		}else{
			pVehicleData->m_sPassengerData[nIndex].vPos.x = GetWin_Num_float( this, IDC_EDIT_B_VALUE1 );
			pVehicleData->m_sPassengerData[nIndex].vPos.y = GetWin_Num_float( this, IDC_EDIT_B_VALUE2 );
			pVehicleData->m_sPassengerData[nIndex].vPos.z = GetWin_Num_float( this, IDC_EDIT_B_VALUE3 );
			pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex] = pVehicleData->m_sPassengerData[nIndex];
		}		
	}else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		if( bSet )
		{
			SetWin_Num_float( this, IDC_EDIT_B_VALUE1, pVehicleData->m_sPassengerData[nIndex].vRotate.x );
			SetWin_Num_float( this, IDC_EDIT_B_VALUE2, pVehicleData->m_sPassengerData[nIndex].vRotate.y );
			SetWin_Num_float( this, IDC_EDIT_B_VALUE3, pVehicleData->m_sPassengerData[nIndex].vRotate.z );
		}else{
			pVehicleData->m_sPassengerData[nIndex].vRotate.x = GetWin_Num_float( this, IDC_EDIT_B_VALUE1 );
			pVehicleData->m_sPassengerData[nIndex].vRotate.y = GetWin_Num_float( this, IDC_EDIT_B_VALUE2 );
			pVehicleData->m_sPassengerData[nIndex].vRotate.z = GetWin_Num_float( this, IDC_EDIT_B_VALUE3 );
			pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex] = pVehicleData->m_sPassengerData[nIndex];
		}		
	}
	
}


void CBoardEditDlg::OnBnClickedButtonReset1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();


	float fDelta = GetWin_Num_float( this, IDC_EDIT_TERM );
	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].vPos = 
		pVehicleData->m_sPassengerData[nIndex].vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].vRotate = 
			pVehicleData->m_sPassengerData[nIndex].vRotate = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}

	UpdateValueData();
}

void CBoardEditDlg::OnDeltaposSpinValue1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();


	float fDelta = GetWin_Num_float( this, IDC_EDIT_TERM );
	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].vPos.x     += (float)pNMUpDown->iDelta * fDelta;
		pVehicleData->m_sPassengerData[nIndex].vPos.x += (float)pNMUpDown->iDelta * fDelta;
	}else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].vRotate.x     += (float)pNMUpDown->iDelta * fDelta;
		pVehicleData->m_sPassengerData[nIndex].vRotate.x += (float)pNMUpDown->iDelta * fDelta;
	}

	*pResult = 0;

	UpdateValueData();
}


void CBoardEditDlg::OnDeltaposSpinValue2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();


	float fDelta = GetWin_Num_float( this, IDC_EDIT_TERM );
	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].vPos.y     += (float)pNMUpDown->iDelta * fDelta;
		pVehicleData->m_sPassengerData[nIndex].vPos.y += (float)pNMUpDown->iDelta * fDelta;
	}else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].vRotate.y     += (float)pNMUpDown->iDelta * fDelta;
		pVehicleData->m_sPassengerData[nIndex].vRotate.y += (float)pNMUpDown->iDelta * fDelta;
	}

	*pResult = 0;

	UpdateValueData();
}

void CBoardEditDlg::OnDeltaposSpinValue3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();


	float fDelta = GetWin_Num_float( this, IDC_EDIT_TERM );
	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].vPos.z     += (float)pNMUpDown->iDelta * fDelta;
		pVehicleData->m_sPassengerData[nIndex].vPos.z += (float)pNMUpDown->iDelta * fDelta;
	}else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex].vRotate.z     += (float)pNMUpDown->iDelta * fDelta;
		pVehicleData->m_sPassengerData[nIndex].vRotate.z += (float)pNMUpDown->iDelta * fDelta;
	}

	*pResult = 0;

	UpdateValueData();
}

//void CBoardEditDlg::OnTimer(UINT nIDEvent)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//	UpdateValueData( FALSE );
//
//	CPropertyPage::OnTimer(nIDEvent);
//}

void CBoardEditDlg::OnNMClickListEditbone(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();

	if( !pVehicle || !pVehicleData ) 
		return;
	
	CWnd* pWnd = GetDlgItem( IDC_EDIT_BONENAME );
	pWnd->SetWindowText( pVehicleData->m_sPassengerData[nIndex].strBoneName.c_str() );

	UpdateValueData();
}

void CBoardEditDlg::OnEnChangeEditBValue1()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();

	if( !pVehicle || !pVehicleData ) 
		return;

	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		pVehicleData->m_sPassengerData[nIndex].vPos.x = GetWin_Num_float( this, IDC_EDIT_B_VALUE1 );
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex] = pVehicleData->m_sPassengerData[nIndex];
	}
	else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		pVehicleData->m_sPassengerData[nIndex].vRotate.x = GetWin_Num_float( this, IDC_EDIT_B_VALUE1 );
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex] = pVehicleData->m_sPassengerData[nIndex];
	}
}

void CBoardEditDlg::OnEnChangeEditBValue2()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();

	if( !pVehicle || !pVehicleData ) 
		return;

	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		pVehicleData->m_sPassengerData[nIndex].vPos.y = GetWin_Num_float( this, IDC_EDIT_B_VALUE2 );
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex] = pVehicleData->m_sPassengerData[nIndex];
	}
	else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		pVehicleData->m_sPassengerData[nIndex].vRotate.y = GetWin_Num_float( this, IDC_EDIT_B_VALUE2 );
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex] = pVehicleData->m_sPassengerData[nIndex];
	}
}

void CBoardEditDlg::OnEnChangeEditBValue3()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	POSITION pos = m_list_Passenger.GetFirstSelectedItemPosition();
	int nIndex = m_list_Passenger.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR )
	{
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxVehicle	  *pVehicle = pView->GetVehicle();
	DxVehicleData *pVehicleData = pView->GetVehicleData();

	if( !pVehicle || !pVehicleData ) 
		return;

	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		pVehicleData->m_sPassengerData[nIndex].vPos.z = GetWin_Num_float( this, IDC_EDIT_B_VALUE3 );
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex] = pVehicleData->m_sPassengerData[nIndex];
	}
	else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		pVehicleData->m_sPassengerData[nIndex].vRotate.z = GetWin_Num_float( this, IDC_EDIT_B_VALUE3 );
		pVehicle->m_spVehicleThreadNeedData->m_sPassengerData[nIndex] = pVehicleData->m_sPassengerData[nIndex];
	}
}
