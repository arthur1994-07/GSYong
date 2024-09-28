// LevelLandGatePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "./LevelLandGatePage.h"
#include "./LevelSheetWithTab.h"
#include "../../EngineLib/DxLand/DxLandGateMan.h"
#include "../LevelMainFrm.h"
#include "../LevelEditDoc.h"
#include "../LevelEditView.h"
#include "../../RanLogicServer/FieldServer/GLGaeaServer.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "DlgEffectList.h"

// CLevelLandGatePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLevelLandGatePage, CPropertyPage)
CLevelLandGatePage::CLevelLandGatePage()
	: CPropertyPage(CLevelLandGatePage::IDD)
	, m_bDlgInit(FALSE)
	, m_pLandGate(NULL)
	, m_nSelectGateTo(0)
{
}

CLevelLandGatePage::~CLevelLandGatePage()
{
}

void CLevelLandGatePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GATE, m_listLandGate);
	DDX_Control(pDX, IDC_GATETO_LIST, m_listboxGateTo);
}


BEGIN_MESSAGE_MAP(CLevelLandGatePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_CHECK_REND, OnBnClickedCheckRend)
	ON_BN_CLICKED(IDC_BUTTON_CLONE, OnBnClickedButtonClone)	
    ON_BN_CLICKED(IDC_BUTTON_GATEEFF, &CLevelLandGatePage::OnBnClickedButtonGateEff)
    ON_BN_CLICKED(IDC_CHECK_GATEEFF, &CLevelLandGatePage::OnBnClickedCheckGateEff)
    ON_BN_CLICKED(IDC_BUTTON_GATEEFF_CLEAR, &CLevelLandGatePage::OnBnClickedButtonGateEffClear)
	ON_BN_CLICKED(IDC_BUTTON_ADD_GATE, &CLevelLandGatePage::OnBnClickedButtonAddGate)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_GATE, &CLevelLandGatePage::OnBnClickedButtonDeleteGate)	
	ON_LBN_SELCHANGE(IDC_GATETO_LIST, &CLevelLandGatePage::OnLbnSelchangeListGateTo)
END_MESSAGE_MAP()


// CLevelLandGatePage 메시지 처리기입니다.
// CLevelLandGatePage 메시지 처리기입니다.
BOOL CLevelLandGatePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_bDlgInit = TRUE;
	UpdateContrl ();

	CButton *pButton = (CButton *) GetDlgItem ( IDC_CHECK_REND );
	pButton->SetCheck ( DxLandGateMan::m_bREND );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLevelLandGatePage::ActiveLandGateEdit()
{
	UpdateContrl ();
}

void CLevelLandGatePage::ListtingLandGate ()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	CString StrOldSelect;
	int nIndex = m_listLandGate.GetCurSel ();
	if ( nIndex!=LB_ERR )
	{
		m_listLandGate.GetDlgItemText ( nIndex, StrOldSelect );
	}

	m_listLandGate.ResetContent ();

	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if ( !pGLLandMan )
		return;

	DxLandGateMan *pLandGateMan = pGLLandMan->GetLandGateMan ();
	if ( pLandGateMan == NULL )
		return;

	PDXLANDGATE pCur = pLandGateMan->GetListLandGate();
	while ( pCur )
	{
		int nIndex = m_listLandGate.AddString ( pCur->GetName() );
		m_listLandGate.SetItemData ( nIndex, DWORD(pCur) );

		pCur = pCur->m_pNext;
	}

	m_listLandGate.SelectString ( 0, StrOldSelect );	
}

void CLevelLandGatePage::UpdateContrl ()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();

	if ( !m_bDlgInit )	return;

	m_pLandGate = NULL;

	ViewContrls ( FALSE );
	ListtingLandGate ();

	pView->SetCurType ( _CUR_SELECT );
}

void CLevelLandGatePage::ViewContrls ( BOOL bView, BOOL bClear )
{
	if ( !m_bDlgInit )	return;

	DWORD dwButtons[] = { IDC_BUTTON_EDIT, IDC_BUTTON_DEL, IDC_BUTTON_NEW, IDC_BUTTON_CLONE };
	DWORD dwNumButtons = sizeof(dwButtons) / sizeof(DWORD);

	DWORD dwContrls[] = 
	{ 
		IDC_EDIT_NAME, 
		IDC_STATIC_GATEID2, 
		IDC_STATIC_MAPID,
		IDC_STATIC_GATEFLAGS, 
		IDC_CHECK_OUT,
		IDC_CHECK_IN,
		IDC_EDIT_GATEID2, 
		IDC_STATIC_GATEID, 
		IDC_EDIT_TOGATEID, 
		IDC_STATIC_GATEID, 
		IDC_EDIT_MID, 
		IDC_EDIT_SID, 
		IDC_STATIC_CREATEPOS,
		IDC_EDIT_GEN_DX, 
		IDC_EDIT_GEN_DY, 
		IDC_BUTTON_ADD, 
		IDC_BUTTON_APPLY,
		IDC_STATIC_GATEEFF,
		IDC_EDIT_GATEEFF,
		IDC_BUTTON_GATEEFF,
		IDC_BUTTON_GATEEFF_CLEAR,
		IDC_CHECK_GATEEFF,
		IDC_GATETO_LIST,
	};
	DWORD dwNumContrls = sizeof(dwContrls) / sizeof(DWORD);

	CWnd *pWnd;
	CButton *pButton;

	for ( DWORD i=0; i < dwNumButtons; i++ )
	{
		pWnd = GetDlgItem ( dwButtons[i] );
		if(pWnd) pWnd->EnableWindow ( !bView );
	}

	for ( DWORD i=0; i < dwNumContrls; i++ )
	{
		pWnd = GetDlgItem ( dwContrls[i] );
		if(pWnd) pWnd->ShowWindow ( bView );
	}

	m_listboxGateTo.ResetContent();
	if ( bClear )
	{
		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_GATEID2 );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_XML_NAME_KEY );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_XML_NAME_INDEX );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_MID );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_SID );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_TOGATEID );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_GEN_DX );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_GEN_DY );
		pWnd->SetWindowText ( "" );
	}
	else if ( m_pLandGate )
	{
		char szValue[256];
		CString strTemp;

		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText ( m_pLandGate->GetName() );		

		pButton = (CButton *) GetDlgItem ( IDC_CHECK_OUT );
		pButton->SetCheck ( m_pLandGate->GetFlags()&DxLandGate::GATE_OUT );

		pButton = (CButton *) GetDlgItem ( IDC_CHECK_IN );
		pButton->SetCheck ( m_pLandGate->GetFlags()&DxLandGate::GATE_IN );

		//	Note : Gate ID
		pWnd = GetDlgItem ( IDC_EDIT_GATEID2 );
		strTemp.Format ( "%d", m_pLandGate->GetGateID() );
		pWnd->SetWindowText ( strTemp );

		for ( DWORD _i(0); _i < DxLandGate::GATETO_NSIZE; ++_i )
		{
			const SNATIVEID gateToMapID(m_pLandGate->GetToMapID(_i));
			if ( gateToMapID == SNATIVEID(false) )
				break;

			strTemp.Format ( "%d/%d, %d, \"%s\"(%d)",  gateToMapID.wMainID, gateToMapID.wSubID, m_pLandGate->GetToGateID(_i), m_pLandGate->GetGateNameKey(_i), m_pLandGate->GetGateNameIndex(_i));
			m_listboxGateTo.AddString(strTemp);
		}
		m_listboxGateTo.SetCurSel(0);

		pWnd = GetDlgItem ( IDC_EDIT_MID );
		strTemp.Format ( "%d", m_pLandGate->GetToMapID(0).wMainID );
		pWnd->SetWindowText ( strTemp );

		pWnd = GetDlgItem ( IDC_EDIT_SID );
		strTemp.Format ( "%d", m_pLandGate->GetToMapID(0).wSubID );
		pWnd->SetWindowText ( strTemp );		

		pWnd = GetDlgItem ( IDC_EDIT_TOGATEID );
		strTemp.Format ( "%d", m_pLandGate->GetToGateID(0) );
		pWnd->SetWindowText ( strTemp );		

		pWnd = GetDlgItem ( IDC_EDIT_XML_NAME_INDEX );
		strTemp.Format ( "%d", m_pLandGate->GetGateNameIndex(0) );
		pWnd->SetWindowText ( strTemp );		

		pWnd = GetDlgItem ( IDC_EDIT_XML_NAME_KEY );
		pWnd->SetWindowText ( m_pLandGate->GetGateNameKey(0) );		
		

		

		//	Note : 분할 간격.
		D3DXVECTOR2 vDiv = m_pLandGate->GetDiv ();
		pWnd = GetDlgItem ( IDC_EDIT_GEN_DX );
		_gcvt ( vDiv.x, 7, szValue );
		pWnd->SetWindowText ( szValue );

		pWnd = GetDlgItem ( IDC_EDIT_GEN_DY );
		_gcvt ( vDiv.y, 7, szValue );
		pWnd->SetWindowText ( szValue );

        SetGateEffect();
	}
}

void CLevelLandGatePage::OnBnClickedButtonNew()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
        return;

	DxLandGateMan *pLandGateMan = pGLLandMan->GetLandGateMan ();
	if ( pLandGateMan == NULL )
		return;

	PDXLANDGATE pLandGate;
	pLandGate = new DxLandGate;

	char szNewName[DxLandGate::MAX_SZNAME];
	
	int i=0;
	do
	{
		sprintf ( szNewName, "%s[%d]", "NoName", i++ );
	} while ( pLandGateMan->FindLandGate(szNewName) );

	pLandGate->SetName ( szNewName );

	DxLandGate* pCur = pLandGateMan->GetListLandGate ();
	
	DWORD dwGateID = 0;
	while ( pLandGateMan->FindLandGate(dwGateID) )
	{
		dwGateID++;
	} 

	pLandGate->SetGateID ( dwGateID );

	const D3DXVECTOR3& vCollPos = pView->GetCollisionPos();
	pLandGate->SetPos ( vCollPos );

	m_pLandGate = pLandGate;
	pView->ActiveEditBox ( m_pLandGate->GetMax(), m_pLandGate->GetMin() );
	pLandGateMan->AddLandGate ( m_pLandGate );
	pLandGateMan->BuildTree ();

	ViewContrls ( TRUE );
	ListtingLandGate ();

	m_listLandGate.SelectString ( 0, m_pLandGate->GetName() );
}

void CLevelLandGatePage::OnBnClickedButtonDel()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
        return;

	DxLandGateMan *pLandGateMan = pGLLandMan->GetLandGateMan ();
	if ( pLandGateMan == NULL )
		return;

	int nIndex = m_listLandGate.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	PDXLANDGATE pLandGate = (PDXLANDGATE) m_listLandGate.GetItemData ( nIndex );

	pLandGateMan->DelLandGate ( pLandGate->GetName() );
	pLandGateMan->BuildTree ();

	//	현제 편집중인 이팩트일경우.
	if ( pLandGate== m_pLandGate )
	{
		m_pLandGate = NULL;
		ViewContrls ( FALSE );
		UpdateContrl ();
	}
	else
	{
		ListtingLandGate ();
	}
}

void CLevelLandGatePage::OnBnClickedButtonEdit()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();

	int nIndex = m_listLandGate.GetCurSel ();
	if ( nIndex==LB_ERR )	return;
	
	m_pLandGate = (PDXLANDGATE) m_listLandGate.GetItemData ( nIndex );
	pView->ActiveEditBox ( m_pLandGate->GetMax(), m_pLandGate->GetMin() );

	ViewContrls ( TRUE );

	DxViewPort::GetInstance().CameraJump ( (m_pLandGate->GetMax()+m_pLandGate->GetMin())/2.0f );
}

void CLevelLandGatePage::OnBnClickedButtonApply()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	DxLandMan* pLandMan = pView->GetLandMan();
	if (!pGLLandMan)
        return;

	DxLandGateMan *pLandGateMan = pGLLandMan->GetLandGateMan ();
	if ( pLandGateMan == NULL )
		return;

	CWnd *pWnd;
	CButton *pButton;

	char szValue[256];
	char szName[DxLandGate::MAX_SZNAME];
	char szToGateNameKey[DxLandGate::MAX_SZNAME];
	DWORD dwFlags = NULL;
	DWORD GateID = 0;
	SNATIVEID TOMapID;
	DWORD ToGateID = 0;
	D3DXVECTOR2 vDIv(0,0);

	pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->GetWindowText ( szName, DxLandGate::MAX_SZNAME );

	pButton = (CButton *) GetDlgItem ( IDC_CHECK_OUT );
	if ( pButton->GetCheck() )
		dwFlags |= DxLandGate::GATE_OUT;

	pButton = (CButton *) GetDlgItem ( IDC_CHECK_IN );
	if ( pButton->GetCheck() )
		dwFlags |= DxLandGate::GATE_IN;

	pWnd = GetDlgItem ( IDC_EDIT_GATEID2 );
	pWnd->GetWindowText ( szValue, 256 );
	GateID = (DWORD) atoi ( szValue );	


	pWnd = GetDlgItem ( IDC_EDIT_MID );
	pWnd->GetWindowText ( szValue, 256 );
	const WORD wMainID((WORD)atoi(szValue));

	pWnd = GetDlgItem ( IDC_EDIT_SID );
	pWnd->GetWindowText ( szValue, 256 );
	const WORD wSubID((WORD)atoi(szValue));

	//	Note : To Gate ID
	pWnd = GetDlgItem ( IDC_EDIT_TOGATEID );
	pWnd->GetWindowText ( szValue, 256 );
	const DWORD dwGateID((DWORD)atoi(szValue));

	pWnd = GetDlgItem ( IDC_EDIT_XML_NAME_INDEX );
	pWnd->GetWindowText ( szValue, 256 );		
	const DWORD dwNameIndex((DWORD)atoi(szValue));

	pWnd = GetDlgItem ( IDC_EDIT_XML_NAME_KEY );
	pWnd->GetWindowText(szToGateNameKey, 256);

	//	Note : 분할 간격.
	pWnd = GetDlgItem ( IDC_EDIT_GEN_DX );
	pWnd->GetWindowText ( szValue, 256 );
	vDIv.x = (float) atof(szValue);

	pWnd = GetDlgItem ( IDC_EDIT_GEN_DY );
	pWnd->GetWindowText ( szValue, 256 );
	vDIv.y = (float) atof(szValue);	

	const int nSelectIndex(m_listboxGateTo.GetCurSel());
	if ( nSelectIndex != -1 )
	{
		if ( m_pLandGate->SetToGateMapID(SNATIVEID(wMainID, wSubID), dwGateID, dwNameIndex, szToGateNameKey, nSelectIndex) )
		{
			CString strTemp;
			m_listboxGateTo.ResetContent ();
			for ( DWORD _i(0); _i < m_pLandGate->GetNGateToMapID(); ++_i )
			{
				const DWORD dwGateToID(m_pLandGate->GetToGateID(_i));
				const SNATIVEID gateToMapID(m_pLandGate->GetToMapID(_i));
				if ( gateToMapID == SNATIVEID(false) )
					break;

				strTemp.Format ( "%d/%d, %d, \"%s\"(%d)",  gateToMapID.wMainID, gateToMapID.wSubID, dwGateToID, m_pLandGate->GetGateNameKey(_i), m_pLandGate->GetGateNameIndex(_i));
				m_listboxGateTo.AddString(strTemp);
			}
		}
	}


	BOOL bExist = FALSE;
	DxLandGate* pCur = pLandGateMan->GetListLandGate ();
	while ( pCur )
	{
		if ( pCur!=m_pLandGate )
		{
			if ( !strcmp(pCur->GetName(),szName) )
			{
				MessageBox ( "ERROR : Same name is already registered.", "Caution" );
				bExist = TRUE;
				return;
			}

			if ( pCur->GetGateID() == GateID )
			{
				MessageBox ( "ERROR : Same ID is already registered.", "Caution" );
				bExist = TRUE;
				return;
			}
		}

		pCur = pCur->m_pNext;
	}

	DxEditBox* pEditBox = pView->GetEditBox();

	m_pLandGate->SetName ( szName );
	m_pLandGate->SetBox ( pEditBox->m_vMax, pEditBox->m_vMin );
	m_pLandGate->SetGate ( dwFlags, GateID, vDIv );
	
	if ( !pLandMan->GetNaviMesh()->IsVailedCollision() )
	{
		if ( !pLandMan->GetNaviMesh()->IsVailedCollision() )
		{
			MessageBox ( "ERROR : Start locaion can not be calculated because there is no navigation mesh", "Caution" );
			return;
		}
	}

	m_pLandGate->MakeStartPos ( pLandMan->GetNaviMesh().get() );
	pLandGateMan->BuildTree ();

	m_listLandGate.SelectString ( 0, m_pLandGate->GetName() );
	ListtingLandGate ();
}

void CLevelLandGatePage::OnBnClickedButtonAdd()
{
	OnBnClickedButtonApply();

	m_pLandGate = NULL;
	ViewContrls ( FALSE );
	UpdateContrl ();
}

void CLevelLandGatePage::OnBnClickedCheckRend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton = (CButton *) GetDlgItem ( IDC_CHECK_REND );
	DxLandGateMan::m_bREND = pButton->GetCheck();
}

void CLevelLandGatePage::OnBnClickedButtonClone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();

	int nIndex = m_listLandGate.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	PDXLANDGATE pLandGate = (PDXLANDGATE) m_listLandGate.GetItemData ( nIndex );

	OnBnClickedButtonNew();

	m_pLandGate->SetBox ( pLandGate->GetMax(), pLandGate->GetMin() );
	pView->ActiveEditBox ( pLandGate->GetMax(), pLandGate->GetMin() );
}

void CLevelLandGatePage::OnBnClickedButtonGateEff()
{
    CDlgEffectList kEffectList;

    if (m_pLandGate && kEffectList.DoModal())
    {
        PLANDEFF pEff = kEffectList.GetSelEffect();

        if (pEff)
        {
            m_pLandGate->SetEffName(pEff->GetEffectName());
            SetGateEffect();
        }
    }
}

void CLevelLandGatePage::SetGateEffect()
{
    CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
    CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
    if (!pGLLandMan || !m_pLandGate)
        return;

    CWnd* pWnd = 0;
    CString strTemp;
    bool bCheck = false;

    pWnd = GetDlgItem ( IDC_EDIT_GATEEFF );
    strTemp.Format ( "%s", m_pLandGate->GetEffName() );
    pWnd->SetWindowText ( strTemp );

    CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_GATEEFF);

    PLANDEFF pEff = pGLLandMan->FindLandEff( 0, m_pLandGate->GetEffName() );
    if (pEff)
    {
        bCheck = pEff->m_bRender;
    }

    if (pButton)
    {
        pButton->SetCheck(bCheck);
    }
}

void CLevelLandGatePage::OnBnClickedCheckGateEff()
{
    bool bCheck = false;
    CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_GATEEFF);
    
    if (pButton && pButton->GetCheck() == BST_CHECKED)
    {
        bCheck = true;
    }

    CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
    CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
    if (!pGLLandMan)
        return;

    if (m_pLandGate)
    {
        pGLLandMan->SetLandEffect(m_pLandGate->GetGateID(), bCheck);
    }
}

void CLevelLandGatePage::OnBnClickedButtonGateEffClear()
{
    if (m_pLandGate)
    {
        m_pLandGate->SetEffName(_T(""));
        SetGateEffect();
    }
}
void CLevelLandGatePage::OnLbnSelchangeListGateTo()
{
	m_nSelectGateTo = m_listboxGateTo.GetCurSel();	

	CWnd *pWnd;
	CString strTemp;
	pWnd = GetDlgItem ( IDC_EDIT_MID );
	strTemp.Format ( "%d", m_pLandGate->GetToMapID(m_nSelectGateTo).wMainID );
	pWnd->SetWindowText ( strTemp );	

	pWnd = GetDlgItem ( IDC_EDIT_SID );
	strTemp.Format ( "%d", m_pLandGate->GetToMapID(m_nSelectGateTo).wSubID );
	pWnd->SetWindowText ( strTemp );

	pWnd = GetDlgItem ( IDC_EDIT_TOGATEID );
	strTemp.Format ( "%d", m_pLandGate->GetToGateID(m_nSelectGateTo) );
	pWnd->SetWindowText ( strTemp );	

	pWnd = GetDlgItem ( IDC_EDIT_XML_NAME_INDEX );
	strTemp.Format ( "%d", m_pLandGate->GetGateNameIndex(m_nSelectGateTo) );
	pWnd->SetWindowText ( strTemp );		

	pWnd = GetDlgItem ( IDC_EDIT_XML_NAME_KEY );
	pWnd->SetWindowText ( m_pLandGate->GetGateNameKey(m_nSelectGateTo) );
}

void CLevelLandGatePage::OnBnClickedButtonAddGate()
{
	if ( m_pLandGate == NULL )
		return;

	char szValue[256];
	CString strTemp;
	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_MID );
	pWnd->GetWindowText ( szValue, 256 );
	const WORD wMainID((WORD)atoi(szValue));

	pWnd = GetDlgItem ( IDC_EDIT_SID );
	pWnd->GetWindowText ( szValue, 256 );
	const WORD wSubID((WORD)atoi(szValue));

	//	Note : To Gate ID
	pWnd = GetDlgItem ( IDC_EDIT_TOGATEID );
	pWnd->GetWindowText ( szValue, 256 );
	const DWORD dwGateID((DWORD)atoi(szValue));

	pWnd = GetDlgItem ( IDC_EDIT_XML_NAME_INDEX );
	pWnd->GetWindowText ( szValue, 256 );		
	const DWORD dwNameIndex((DWORD)atoi(szValue));

	pWnd = GetDlgItem ( IDC_EDIT_XML_NAME_KEY );
	pWnd->GetWindowText(szValue, 256);

	const DWORD nIndex(m_pLandGate->AddToGateMapID(SNATIVEID(wMainID, wSubID), dwGateID, dwNameIndex, szValue));
	if ( nIndex == -1 )
		return;

	strTemp.Format ( "%d/%d, %d, \"%s\"(%d)",  wMainID, wSubID, dwGateID, szValue, dwNameIndex);
	m_listboxGateTo.AddString(strTemp);
}
void CLevelLandGatePage::OnBnClickedButtonDeleteGate()
{
	if ( m_pLandGate == NULL )
		return;

	const int nIndex(m_listboxGateTo.GetCurSel());
	if ( m_pLandGate->ResetToGateMapID(nIndex) == false )
		return;

	CString strTemp;
	m_listboxGateTo.ResetContent ();
	for ( DWORD _i(0); _i < m_pLandGate->GetNGateToMapID(); ++_i )
	{
		const DWORD dwGateToID(m_pLandGate->GetToGateID(_i));
		const SNATIVEID gateToMapID(m_pLandGate->GetToMapID(_i));
		if ( gateToMapID == SNATIVEID(false) )
			break;

		strTemp.Format ( "%d/%d, %d, \"%s\"(%d)",  gateToMapID.wMainID, gateToMapID.wSubID, dwGateToID, m_pLandGate->GetGateNameKey(_i), m_pLandGate->GetGateNameIndex(_i));
		m_listboxGateTo.AddString(strTemp);
	}
}


