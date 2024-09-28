// EtcFuncPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "../LevelMainFrm.h"
#include "../LevelEditDoc.h"
#include "../LevelEditView.h"

#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "./MobSchedulePage.h"
#include "./LevelSheetWithTab.h"
#include "../../EngineLib/DxTools/DxMethods.h"

#include "afxdlgs.h"
#include "../../RanLogic/Crow/GLMobSchedule.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

#include "./EtcFuncPage.h"


// CEtcFuncPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEtcFuncPage, CPropertyPage)
CEtcFuncPage::CEtcFuncPage()
	: CPropertyPage(CEtcFuncPage::IDD),
	m_pSheetTab(NULL),
	m_bInit(FALSE),
	m_vMinLandPos( 0.0f, 0.0f, 0.0f ),
	m_vMaxLandPos( 0.0f, 0.0f, 0.0f ),
	m_bUseCircle(false),
	m_dwSelectNum(255)
{
}

CEtcFuncPage::~CEtcFuncPage()
{
}

void CEtcFuncPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LANDEFF, m_comob_LandEff);
	DDX_Control(pDX, IDC_LIST_LANDEFF, m_list_LandEff);
}


BEGIN_MESSAGE_MAP(CEtcFuncPage, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_LANDEFF, OnBnClickedCheckLandeff)
	ON_BN_CLICKED(IDC_CHECK_LIMITTIME, OnBnClickedCheckLimittime)
	ON_BN_CLICKED(IDC_CHECK_CONCAM, OnBnClickedCheckConcam)
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM, OnBnClickedButtonConfirm)
	ON_BN_CLICKED(IDC_BUTTON_CONCAM, OnBnClickedButtonConcam)
	ON_BN_CLICKED(IDC_BUTTON_LANDEFF1, OnBnClickedButtonLandeff1)
	ON_BN_CLICKED(IDC_BUTTON_LANDEFF2, OnBnClickedButtonLandeff2)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LANDEFF, OnNMClickListLandeff)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LANDEFF, OnNMDblclkListLandeff)
	ON_BN_CLICKED(IDC_CHECK_CIRCLE, OnBnClickedCheckCircle)
	ON_BN_CLICKED(IDC_CHECK_REENTRYLIMIT, OnBnClickedCheckReentrylimit)
END_MESSAGE_MAP()


// CEtcFuncPage �޽��� ó�����Դϴ�.
BOOL CEtcFuncPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_bInit = TRUE;


	m_comob_LandEff.ResetContent();
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		for( int i = 0; i < EMLANDEFFECT_SIZE; i++ )
		{
			m_comob_LandEff.AddString(CGameTextMan::GetInstance().GetCommentText("LANDEFFECT_TYPE", i));
		}
	}
	else
	{
		for( int i = 0; i < EMLANDEFFECT_SIZE; i++ )
		{
			m_comob_LandEff.AddString( COMMENT::LANDEFFECT_TYPE[i].c_str() );
		}
	}

	m_comob_LandEff.SetCurSel(0);


	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;

	lvColumn.pszText = "EffType";
	lvColumn.iSubItem = 0;
	lvColumn.cx = 80 ;
	m_list_LandEff.InsertColumn( 0, &lvColumn );

	lvColumn.pszText = "Rate";
	lvColumn.iSubItem = 1;
	lvColumn.cx = 60 ;
	m_list_LandEff.InsertColumn( 1, &lvColumn );

	lvColumn.pszText = "Color";
	lvColumn.iSubItem = 2;
	lvColumn.cx = 60 ;
	m_list_LandEff.InsertColumn( 2, &lvColumn );

	m_list_LandEff.SetExtendedStyle ( m_list_LandEff.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	SetWin_Text( this, IDC_EDIT_LANDEFF, "0" );


	UpdatePage();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CEtcFuncPage::SetEnableFunction( DWORD dwEtcFunc, BOOL bEnable )
{
	switch( dwEtcFunc )
	{ 
	case EMETCFUNC_LIMITTIME:		// �ð� ����
		SetWin_Check( this, IDC_CHECK_LIMITTIME, bEnable );
		SetWin_Enable( this, IDC_EDIT_LIMITTIME1, bEnable ); // �ִ�ð�
		SetWin_Enable( this, IDC_EDIT_LIMITTIME2, bEnable ); // �˸� ����
		break;
	case EMETCFUNC_CONTROLCAM:		// ī�޶� ����
		SetWin_Check( this, IDC_CHECK_CONCAM, bEnable );
		SetWin_Enable( this, IDC_EDIT_CONCAM1, bEnable );	  // UP
		SetWin_Enable( this, IDC_EDIT_CONCAM2, bEnable );	  // DOWN
		SetWin_Enable( this, IDC_EDIT_CONCAM3, bEnable );	  // LEFT
		SetWin_Enable( this, IDC_EDIT_CONCAM4, bEnable );	  // RIGHT
		SetWin_Enable( this, IDC_EDIT_CONCAM8, bEnable );	  // FAR
		SetWin_Enable( this, IDC_EDIT_CONCAM9, bEnable );	  // NEAR
		SetWin_Enable( this, IDC_BUTTON_CONCAM, bEnable );	  // Set Center Pos
		SetWin_Enable( this, IDC_CHECK_CONCAM2, bEnable );	  // TEST
		if( bEnable )
		{
			DxViewPort::GetInstance().SetGameCamera();
		}else{
			DxViewPort::GetInstance().SetToolCamera();
		}
		break;
	case EMETCFUNC_LANDEFFECT:		// ���� ȿ��
		SetWin_Check( this, IDC_CHECK_LANDEFF, bEnable );
		SetWin_Enable( this, IDC_BUTTON_LANDEFF1, bEnable ); // �߰�
		SetWin_Enable( this, IDC_BUTTON_LANDEFF2, bEnable ); // ����
		SetWin_Enable( this, IDC_COMBO_LANDEFF, bEnable );   // ȿ�� ����
		SetWin_Enable( this, IDC_LIST_LANDEFF, bEnable );	 // ȿ�� ����Ʈ
		SetWin_Enable( this, IDC_CHECK_CIRCLE, bEnable );
		break;
	case EMETCFUNC_MOBCHASE:		// ���� ����
		SetWin_Check( this, IDC_CHECK_MOBCHASE, bEnable );
		break;
	case EMETCFUNC_REGENONCE:
		SetWin_Check( this, IDC_CHECK_REGENONCE, bEnable );
		break;
	case EMETCFUNC_REENTRYLIMIT:
		SetWin_Check( this, IDC_CHECK_REENTRYLIMIT, bEnable );
		SetWin_Enable( this, IDC_EDIT_REENTRYLIMIT, bEnable );
		break;
	}
		
}


void CEtcFuncPage::UpdatePage()
{
	if (!m_bInit)
        return;

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	GLLandMan* pGLLandMan	  = pGaeaServer->GetRootMap();
	SLEVEL_ETC_FUNC* pEtcFunc = &pGLLandMan->m_sLevelEtcFunc;
	for( int i = 0; i < EMETCFUNC_SIZE; i++ )
	{
		SetEnableFunction( (EMETCFUNC)i, pEtcFunc->m_bUseFunction[i] );
	}

	CString strTemp;


	// Update Limit Time
	strTemp.Format( "%d", pEtcFunc->m_sLimitTime.nLimitMinute );
	SetWin_Text( this, IDC_EDIT_LIMITTIME1, strTemp.GetString() );
	strTemp.Format( "%d", pEtcFunc->m_sLimitTime.nTellTermMinute );
	SetWin_Text( this, IDC_EDIT_LIMITTIME2, strTemp.GetString() );

	
	// Update Control Camera
	strTemp.Format( "%.2f", pEtcFunc->m_sControlCam.fUp );
	SetWin_Text( this, IDC_EDIT_CONCAM1, strTemp.GetString() );
	strTemp.Format( "%.2f", pEtcFunc->m_sControlCam.fDown );
	SetWin_Text( this, IDC_EDIT_CONCAM2, strTemp.GetString() );
	strTemp.Format( "%.2f", pEtcFunc->m_sControlCam.fRight );
	SetWin_Text( this, IDC_EDIT_CONCAM3, strTemp.GetString() );
	strTemp.Format( "%.2f", pEtcFunc->m_sControlCam.fLeft );
	SetWin_Text( this, IDC_EDIT_CONCAM4, strTemp.GetString() );
	strTemp.Format( "%.2f", pEtcFunc->m_sControlCam.fFar );
	SetWin_Text( this, IDC_EDIT_CONCAM8, strTemp.GetString() );
	strTemp.Format( "%.2f", pEtcFunc->m_sControlCam.fNear );
	SetWin_Text( this, IDC_EDIT_CONCAM9, strTemp.GetString() );
	strTemp.Format( "%.2f", pEtcFunc->m_sControlCam.vCamPos.x );
	SetWin_Text( this, IDC_EDIT_CONCAM5, strTemp.GetString() );
	strTemp.Format( "%.2f", pEtcFunc->m_sControlCam.vCamPos.y );
	SetWin_Text( this, IDC_EDIT_CONCAM6, strTemp.GetString() );
	strTemp.Format( "%.2f", pEtcFunc->m_sControlCam.vCamPos.z );
	SetWin_Text( this, IDC_EDIT_CONCAM7, strTemp.GetString() );

	// Update Re-entry Limit TIme
	strTemp.Format( "%d", pEtcFunc->m_dwReEntryTime );
	SetWin_Text( this, IDC_EDIT_REENTRYLIMIT, strTemp.GetString() );

	// Update Land Effect
	m_vecLandEffect = pEtcFunc->m_vecLandEffect;

	UpdateLandEffectList();

	
}

void CEtcFuncPage::UpdateLandEffectList()
{
	CString strTemp;
	m_list_LandEff.DeleteAllItems();
	for( unsigned int i = 0; i < m_vecLandEffect.size(); i++ )
	{
		SLANDEFFECT landEffect = m_vecLandEffect[i];
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			CGameTextMan::GetInstance().GetCommentText("LANDEFFECT_TYPE", (int)landEffect.emLandEffectType );
		else
			m_list_LandEff.InsertItem( i, COMMENT::LANDEFFECT_TYPE[(int)landEffect.emLandEffectType].c_str() );

		strTemp.Format( "%.3f", landEffect.fValue );
		m_list_LandEff.SetItemText( i, 1, strTemp.GetString() );

		switch( landEffect.emLandEffectType ) 
		{
			case EMLANDEFFECT_ATK_SPEED:  // ������
				m_list_LandEff.SetItemText( i, 2, "������" );
				break;
			case EMLANDEFFECT_MOVE_SPEED: // ��Ȳ��
				m_list_LandEff.SetItemText( i, 2, "��Ȳ��" );
				break;
			case EMLANDEFFECT_MP_RATE:	  // �����
				m_list_LandEff.SetItemText( i, 2, "�����" );
				break;
			case EMLANDEFFECT_HP_RATE:    // �ʷϻ�
				m_list_LandEff.SetItemText( i, 2, "�ʷϻ�" );
				break;
			case EMLANDEFFECT_RECOVER_RATE: // �Ķ���
				m_list_LandEff.SetItemText( i, 2, "�Ķ���" );
				break;
			case EMLANDEFFECT_DAMAGE_RATE:	// ����
				m_list_LandEff.SetItemText( i, 2, "����" );
				break;
			case EMLANDEFFECT_DEFENSE_RATE:	// �����
				m_list_LandEff.SetItemText( i, 2, "�����" );
				break;
			case EMLANDEFFECT_RESIST_RATE:	// ȸ��
				m_list_LandEff.SetItemText( i, 2, "ȸ��" );
				break;
			case EMLANDEFFECT_CANCEL_ALLBUFF: // ������
				m_list_LandEff.SetItemText( i, 2, "������" );
				break;
			case EMLANDEFFECT_SAFE_ZONE: // ������
				m_list_LandEff.SetItemText( i, 2, "�ϴû�" );
				break;
		}
	}
}

void CEtcFuncPage::SetEtcFuncData()
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan	  = pGaeaServer->GetRootMap();
	SLEVEL_ETC_FUNC* pEtcFunc = &pGLLandMan->m_sLevelEtcFunc;

	CString strTemp;
	if( GetWin_Check( this, IDC_CHECK_LIMITTIME ) )
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_LIMITTIME] = TRUE;
		
		strTemp = GetWin_Text( this, IDC_EDIT_LIMITTIME1 );
		pEtcFunc->m_sLimitTime.nLimitMinute	  = atoi(strTemp.GetString());
		strTemp = GetWin_Text( this, IDC_EDIT_LIMITTIME2 );
		pEtcFunc->m_sLimitTime.nTellTermMinute = atoi(strTemp.GetString());
	}else{
		pEtcFunc->m_bUseFunction[EMETCFUNC_LIMITTIME] = FALSE;
		pEtcFunc->m_sLimitTime.Init();
	}

	if( GetWin_Check( this, IDC_CHECK_CONCAM ) )
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_CONTROLCAM] = TRUE;

		strTemp = GetWin_Text( this, IDC_EDIT_CONCAM1 );
		pEtcFunc->m_sControlCam.fUp	  = atof(strTemp.GetString());
		strTemp = GetWin_Text( this, IDC_EDIT_CONCAM2 );
		pEtcFunc->m_sControlCam.fDown	  = atof(strTemp.GetString());
		strTemp = GetWin_Text( this, IDC_EDIT_CONCAM3 );
		pEtcFunc->m_sControlCam.fRight	  = atof(strTemp.GetString());
		strTemp = GetWin_Text( this, IDC_EDIT_CONCAM4 );
		pEtcFunc->m_sControlCam.fLeft	  = atof(strTemp.GetString());
		strTemp = GetWin_Text( this, IDC_EDIT_CONCAM8 );
		pEtcFunc->m_sControlCam.fFar	  = atof(strTemp.GetString());
		strTemp = GetWin_Text( this, IDC_EDIT_CONCAM9 );
		pEtcFunc->m_sControlCam.fNear	  = atof(strTemp.GetString());


		strTemp = GetWin_Text( this, IDC_EDIT_CONCAM5 );
		pEtcFunc->m_sControlCam.vCamPos.x = atof(strTemp.GetString());
		strTemp = GetWin_Text( this, IDC_EDIT_CONCAM6 );
		pEtcFunc->m_sControlCam.vCamPos.y = atof(strTemp.GetString());
		strTemp = GetWin_Text( this, IDC_EDIT_CONCAM7 );
		pEtcFunc->m_sControlCam.vCamPos.z = atof(strTemp.GetString());

	}else{
		pEtcFunc->m_bUseFunction[EMETCFUNC_CONTROLCAM] = FALSE;
		pEtcFunc->m_sControlCam.Init();
	}

	if( GetWin_Check( this, IDC_CHECK_MOBCHASE ) )
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_MOBCHASE] = TRUE;
	}else{
		pEtcFunc->m_bUseFunction[EMETCFUNC_MOBCHASE] = FALSE;
	}

	if( GetWin_Check( this, IDC_CHECK_LANDEFF ) )
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_LANDEFFECT] = TRUE;
		pEtcFunc->m_vecLandEffect = m_vecLandEffect;
	}else{
		pEtcFunc->m_bUseFunction[EMETCFUNC_LANDEFFECT] = FALSE;
		pEtcFunc->m_vecLandEffect.clear();
	}

	if( GetWin_Check( this, IDC_CHECK_REGENONCE ) )
	{
		pEtcFunc->m_bUseFunction[ EMETCFUNC_REGENONCE ] = TRUE;
	}
	else
	{
		pEtcFunc->m_bUseFunction[ EMETCFUNC_REGENONCE ] = FALSE;
	}

	if( GetWin_Check( this, IDC_CHECK_REENTRYLIMIT ) )
	{
		pEtcFunc->m_bUseFunction[ EMETCFUNC_REENTRYLIMIT ] = TRUE;
		strTemp = GetWin_Text( this, IDC_EDIT_REENTRYLIMIT );
		pEtcFunc->m_dwReEntryTime	  = atoi(strTemp.GetString());
	}else{
		pEtcFunc->m_bUseFunction[ EMETCFUNC_REENTRYLIMIT ] = FALSE;
	}

}


void CEtcFuncPage::OnBnClickedButtonConfirm()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetEtcFuncData();
}

void CEtcFuncPage::OnBnClickedButtonConcam()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 &vLookPt = DxViewPort::GetInstance().GetLookatPt ();

	D3DXVECTOR3 vCamPos = vFromPt - vLookPt;

	float fLength = D3DXVec3Length( &vCamPos );
	if( fLength > 200.0f ) 
	{
		MessageBox( "ī�޶��� �Ÿ��� �ʹ� �ٴϴ�.", "�˸�", MB_OK );
		return;
	}

	CString strTemp;
	strTemp.Format( "%.2f", vCamPos.x );
	SetWin_Text( this, IDC_EDIT_CONCAM5, strTemp.GetString() );
	strTemp.Format( "%.2f", vCamPos.y );
	SetWin_Text( this, IDC_EDIT_CONCAM6, strTemp.GetString() );
	strTemp.Format( "%.2f", vCamPos.z );
	SetWin_Text( this, IDC_EDIT_CONCAM7, strTemp.GetString() );


}




void CEtcFuncPage::OnBnClickedButtonLandeff1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if( m_vMinLandPos == D3DXVECTOR3(0.0f,0.0f,0.0f) &&
		m_vMaxLandPos == D3DXVECTOR3(0.0f,0.0f,0.0f) )
	{
		MessageBox( "������ �����ؾ� �մϴ�.", "�˸�", MB_OK );
		return;
	}

	
	float fAddValue = atof(GetWin_Text(this,IDC_EDIT_LANDEFF).GetString());
	if( fAddValue == 0.0f ) 
	{
		MessageBox( "%�� 0�Դϴ�.", "�˸�", MB_OK );
		return;
		return;
	}

	SLANDEFFECT landEffect;
	landEffect.emLandEffectType = (EMLANDEFFECT_TYPE)m_comob_LandEff.GetCurSel();
	landEffect.fValue			= fAddValue;

	if( m_bUseCircle )
	{
		landEffect.vMinPos = landEffect.vMaxPos	= m_vMinLandPos;
        D3DXVECTOR2 vTemp = D3DXVECTOR2(m_vMaxLandPos.x,m_vMaxLandPos.z) - D3DXVECTOR2(m_vMinLandPos.x,m_vMinLandPos.z);
		landEffect.bCircleCheck = TRUE;
		landEffect.fLength = fabs(D3DXVec2Length(&vTemp));
	}else{
		D3DXVECTOR3 vMin( min( m_vMinLandPos.x, m_vMaxLandPos.x ), min( m_vMinLandPos.y, m_vMaxLandPos.y ), min( m_vMinLandPos.z, m_vMaxLandPos.z ) );
        D3DXVECTOR3 vMax( max( m_vMinLandPos.x, m_vMaxLandPos.x ), max( m_vMinLandPos.y, m_vMaxLandPos.y ), max( m_vMinLandPos.z, m_vMaxLandPos.z ) );
		landEffect.vMinPos			= vMin;
		landEffect.vMaxPos			= vMax;
	}

	m_vecLandEffect.push_back(landEffect);

	m_dwSelectNum = 255;

	UpdateLandEffectList();

	m_vMinLandPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vMaxLandPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	
}

void CEtcFuncPage::OnBnClickedButtonLandeff2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	POSITION	pos   = m_list_LandEff.GetFirstSelectedItemPosition ();	
	int			nItem = m_list_LandEff.GetNextSelectedItem ( pos );	
	if( nItem == -1 ) return;
	m_vecLandEffect.erase( m_vecLandEffect.begin() + nItem );
	UpdateLandEffectList();

	if( m_dwSelectNum == nItem ) m_dwSelectNum = 255;


	int nIndex = m_list_LandEff.GetSelectedCount();
}


void CEtcFuncPage::OnNMClickListLandeff(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}

void CEtcFuncPage::OnNMDblclkListLandeff(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	POSITION	pos   = m_list_LandEff.GetFirstSelectedItemPosition ();	
	int			nItem = m_list_LandEff.GetNextSelectedItem ( pos );	
	if( nItem == -1 ) return;

	D3DXVECTOR3 vFromPt	  = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3 vNewLoockatPt, vUpVec(0.0f,1.0f,0.0f);

	SLANDEFFECT landEffect = m_vecLandEffect[nItem];    
	vNewLoockatPt.x = landEffect.vMinPos.x + ((landEffect.vMaxPos.x - landEffect.vMinPos.x) / 2.0f);
	vNewLoockatPt.z = landEffect.vMinPos.z + ((landEffect.vMaxPos.z - landEffect.vMinPos.z) / 2.0f);
	m_dwSelectNum = nItem;


	DxViewPort::GetInstance().CameraJump( vNewLoockatPt );

}

void CEtcFuncPage::OnBnClickedCheckCircle()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bUseCircle = GetWin_Check( this, IDC_CHECK_CIRCLE );
}


void CEtcFuncPage::OnBnClickedCheckLimittime()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetEnableFunction( EMETCFUNC_LIMITTIME, GetWin_Check( this, IDC_CHECK_LIMITTIME ) );	
}

void CEtcFuncPage::OnBnClickedCheckConcam()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetEnableFunction( EMETCFUNC_CONTROLCAM, GetWin_Check( this, IDC_CHECK_CONCAM ) );
}

void CEtcFuncPage::OnBnClickedCheckLandeff()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetEnableFunction( EMETCFUNC_LANDEFFECT, GetWin_Check( this, IDC_CHECK_LANDEFF ) );	
}

void CEtcFuncPage::OnBnClickedCheckReentrylimit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetEnableFunction( EMETCFUNC_REENTRYLIMIT, GetWin_Check( this, IDC_CHECK_REENTRYLIMIT ) );	
}
