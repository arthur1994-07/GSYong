// PropertyChaBase.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../../MainFrm.h"
#include "../../GMCharEdit.h"
#include "./PropertyChaBase.h"
#include "../PropertySheetCharEdit.h"

#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogicServer/DxServerInstance.h"
#include "../../RanLogic/Item/GLItemMan.h"

#include "../../Util/CGmCfg.h"

#include "../../Database/ADO/ADOManager.h"
#include "../../Database/ODBC/CGMOdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CPropertyChaBase 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropertyChaBase, CPropertyPage)
CPropertyChaBase::CPropertyChaBase()
	: CPropertyPage(CPropertyChaBase::IDD)
    , m_nUserNum(0)
    , m_nChaNumDB(0)
    , m_strChaName(_T(""))
    , m_nChaTribe(0)
    , m_nChaClass(0)
    , m_nChaLevel(0)
    , m_nChaPK(0)
    , m_nChaBright(0)
	, m_nChaCP(0)
    , m_llChaExp(0)
    , m_llChaMoney(0)
    , m_nStartGate(0)
    , m_nStartMapID(0)
    , m_fStartPosX(0)
    , m_fStartPosY(0)
    , m_fStartPosZ(0)
    , m_nSaveMapID(0)
    , m_fSavePosX(0)
    , m_fSavePosY(0)
    , m_fSavePosZ(0)
	, m_dwChaSchool(0)
	, m_dwChaFace(0)
	, m_dwChaHair(0)
	, m_nChaLiving(0)
    , m_nCallMapID(0)
    , m_fCallPosX(0)
    , m_fCallPosY(0)
    , m_fCallPosZ(0)
    , m_nInvenLine(0)
	, m_nChaNum(0)
	, m_nCurrentSlot_old(0)
	, m_nCurrentSlot(0)
	, m_nSelectSlot(0)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();

	ZeroMemory( m_nSlotState_old, sizeof(int)*EMCHAR_SLOT_DATA_SIZE );
	ZeroMemory( m_nSlotState, sizeof(int)*EMCHAR_SLOT_DATA_SIZE );
}

CPropertyChaBase::~CPropertyChaBase()
{
}

void CPropertyChaBase::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT2, m_nUserNum);
    DDX_Text(pDX, IDC_EDIT23, m_nChaNumDB);
    DDX_Text(pDX, IDC_EDIT5, m_strChaName);
    DDX_Text(pDX, IDC_EDIT30, m_nChaTribe);
    DDX_Text(pDX, IDC_EDIT29, m_nChaClass);
    DDX_Text(pDX, IDC_EDIT28, m_nChaLevel);
    DDX_Text(pDX, IDC_EDIT26, m_nChaPK);
    DDX_Text(pDX, IDC_EDIT24, m_nChaBright);
    DDX_Text(pDX, IDC_EDIT25, m_chaHP.nNow);
	DDX_Text(pDX, IDC_EDIT4, m_chaMP.nNow);
    DDX_Text(pDX, IDC_EDIT43, m_nChaCP);    
    DDX_Text(pDX, IDC_EDIT27, m_llChaExp);
    DDX_Text(pDX, IDC_EDIT18, m_llChaMoney);

	{
		UINT nTempValue = static_cast< UINT >( m_sCharSlotData[ m_nSelectSlot ].m_sStats.wPow );
		DDX_Text(pDX, IDC_EDIT6, nTempValue );
		m_sCharSlotData[ m_nSelectSlot ].m_sStats.wPow = static_cast< WORD >( nTempValue );
	}

	{
		UINT nTempValue = static_cast< UINT >( m_sCharSlotData[ m_nSelectSlot ].m_sStats.wStr );
		DDX_Text(pDX, IDC_EDIT7, nTempValue );
		m_sCharSlotData[ m_nSelectSlot ].m_sStats.wStr = static_cast< WORD >( nTempValue );
	}

	{
		UINT nTempValue = static_cast< UINT >( m_sCharSlotData[ m_nSelectSlot ].m_sStats.wSta );
		DDX_Text(pDX, IDC_EDIT8, nTempValue );
		m_sCharSlotData[ m_nSelectSlot ].m_sStats.wSta = static_cast< WORD >( nTempValue );
	}

	{
		UINT nTempValue = static_cast< UINT >( m_sCharSlotData[ m_nSelectSlot ].m_sStats.wDex );
		DDX_Text(pDX, IDC_EDIT9, nTempValue );
		m_sCharSlotData[ m_nSelectSlot ].m_sStats.wDex = static_cast< WORD >( nTempValue );
	}

	{
		UINT nTempValue = static_cast< UINT >( m_sCharSlotData[ m_nSelectSlot ].m_sStats.wSpi );
		DDX_Text(pDX, IDC_EDIT10, nTempValue );
		m_sCharSlotData[ m_nSelectSlot ].m_sStats.wSpi = static_cast< WORD >( nTempValue );
	}

	{
		UINT nTempValue = static_cast< UINT >( m_sCharSlotData[ m_nSelectSlot ].m_sStats.wInt );
		DDX_Text(pDX, IDC_EDIT11, nTempValue );
		m_sCharSlotData[ m_nSelectSlot ].m_sStats.wInt = static_cast< WORD >( nTempValue );
	}

	{
		UINT nTempValue = static_cast< UINT >( m_sCharSlotData[ m_nSelectSlot ].m_wStatsPoint );
		DDX_Text(pDX, IDC_EDIT12, nTempValue );
		m_sCharSlotData[ m_nSelectSlot ].m_wStatsPoint = static_cast< WORD >( nTempValue );
	}

    DDX_Text(pDX, IDC_EDIT17, m_nStartGate);
    DDX_Text(pDX, IDC_EDIT13, m_nStartMapID);
    DDX_Text(pDX, IDC_EDIT14, m_fStartPosX);
    DDX_Text(pDX, IDC_EDIT19, m_fStartPosY);
    DDX_Text(pDX, IDC_EDIT20, m_fStartPosZ);
    DDX_Text(pDX, IDC_EDIT15, m_nSaveMapID);
    DDX_Text(pDX, IDC_EDIT16, m_fSavePosX);
    DDX_Text(pDX, IDC_EDIT21, m_fSavePosY);
    DDX_Text(pDX, IDC_EDIT22, m_fSavePosZ);
    DDX_Text(pDX, IDC_EDIT1, m_dwChaSchool);
    DDX_Text(pDX, IDC_EDIT31, m_dwChaFace);
    DDV_MinMaxUInt(pDX, m_dwChaFace, 0, 100);
    DDX_Text(pDX, IDC_EDIT32, m_dwChaHair);
    DDV_MinMaxUInt(pDX, m_dwChaHair, 0, 100);
    DDX_Text(pDX, IDC_EDIT33, m_nChaLiving);
    DDX_Text(pDX, IDC_EDIT34, m_sCharSlotData[ m_nSelectSlot ].m_dwSkillPoint );
    DDX_Text(pDX, IDC_EDIT35, m_nCallMapID);
    DDX_Text(pDX, IDC_EDIT36, m_fCallPosX);
    DDX_Text(pDX, IDC_EDIT37, m_fCallPosY);
    DDX_Text(pDX, IDC_EDIT38, m_fCallPosZ);
    DDX_Text(pDX, IDC_EDIT39, m_nInvenLine);
	DDV_MinMaxInt(pDX, m_nInvenLine, 0, GLCONST_CHAR::wInvenExtendLine);
	DDX_Text(pDX, IDC_EDIT_CURRENT_SLOT, m_nCurrentSlot);
	DDX_Text(pDX, IDC_EDIT_SLOTSTATE, m_nSlotState[ m_nSelectSlot ] );
}


BEGIN_MESSAGE_MAP(CPropertyChaBase, CPropertyPage)
    ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(ID_BTN_BASE_SAVE, OnBnClickedBtnBaseSave)
	ON_BN_CLICKED(IDC_RADIO_CHARSLOT_A, OnBnClickedRadioCharSlotA)
	ON_BN_CLICKED(IDC_RADIO_CHARSLOT_B, OnBnClickedRadioCharSlotB)	
	ON_BN_CLICKED(IDC_RADIO_CHARSLOT_C, OnBnClickedRadioCharSlotC)
	ON_BN_CLICKED(IDC_RADIO_CHARSLOT_D, OnBnClickedRadioCharSlotD)
	ON_BN_CLICKED(IDC_RADIO_CHARSLOT_E, OnBnClickedRadioCharSlotE)
END_MESSAGE_MAP()


// CPropertyChaBase 메시지 처리기입니다.

void CPropertyChaBase::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CPropertyPage::OnShowWindow(bShow, nStatus);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.   
}

BOOL CPropertyChaBase::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    

    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
    SetData(pSheet->m_pChar);

	CButton* pRadio = static_cast< CButton* >( GetDlgItem( IDC_RADIO_CHARSLOT_A ) );
	if ( pRadio )
	{
		pRadio->SetCheck(BST_CHECKED);
	}

	switch ( m_nCurrentSlot )
	{
	case 0:
		OnBnClickedRadioCharSlotA();
		break;

	case 1:
		OnBnClickedRadioCharSlotB();
		break;

	case 2:
		OnBnClickedRadioCharSlotC();
		break;

	case 3:
		OnBnClickedRadioCharSlotD();
		break;

	case 4:
		OnBnClickedRadioCharSlotE();
		break;
	}

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPropertyChaBase::SetData(SCHARDATA2* pChar)
{
    CString strTemp;    
    
    m_nChaNumDB_old  = m_nChaNumDB = (int) pChar->m_CharDbNum;
    m_nUserNum_old   = m_nUserNum  = (int) pChar->GetUserID();
    
	m_strChaName.Format("%s", pChar->m_szName);
	m_strChaName_old = m_strChaName;

    m_nChaTribe_old  = m_nChaTribe  = (int) pChar->m_emTribe;
    m_nChaClass_old  = m_nChaClass  = (int) pChar->m_emClass;
    m_nChaLevel_old  = m_nChaLevel  = (int) pChar->m_wLevel;
    m_nChaPK_old     = m_nChaPK     = (int) pChar->m_wPK;
    m_nChaBright_old = m_nChaBright = (int) pChar->m_nBright;
    m_chaHP_old		= m_chaHP		= pChar->m_sHP;
    m_chaMP_old		= m_chaMP		= pChar->m_sMP;
	m_chaSP_old		= m_chaSP		= pChar->m_sSP;
	m_nChaCP_old		= m_nChaCP	= (int) pChar->m_sCP.dwData;

    m_llChaExp_old   = m_llChaExp   = pChar->m_sExperience.lnNow;
    m_llChaMoney_old = m_llChaMoney = pChar->GetInvenMoney();

    m_nStartGate_old   = m_nStartGate= (int) pChar->m_dwStartGate;

    m_nStartMapID_old  = m_nStartMapID=(int) pChar->m_sStartMapID.dwID;
    m_fStartPosX_old   = m_fStartPosX = pChar->m_vStartPos.x;
    m_fStartPosY_old   = m_fStartPosY = pChar->m_vStartPos.y;
    m_fStartPosZ_old   = m_fStartPosZ = pChar->m_vStartPos.z;

    m_nSaveMapID_old   = m_nSaveMapID = (int) pChar->m_sSaveMapID.dwID;
    m_fSavePosX_old    = m_fSavePosX  = pChar->m_vSavePos.x;
    m_fSavePosY_old    = m_fSavePosY  = pChar->m_vSavePos.y;
    m_fSavePosZ_old    = m_fSavePosZ  = pChar->m_vSavePos.z;

	m_nCallMapID_old   = m_nCallMapID = pChar->m_sLastCallMapID.dwID;
    m_fCallPosX_old    = m_fCallPosX  = pChar->m_vLastCallPos.x;
	m_fCallPosY_old    = m_fCallPosY  = pChar->m_vLastCallPos.y;
	m_fCallPosZ_old    = m_fCallPosZ  = pChar->m_vLastCallPos.z;

	m_dwChaSchool_old  = m_dwChaSchool = (DWORD) pChar->m_wSchool;
	m_dwChaFace_old    = m_dwChaFace   = (DWORD) pChar->m_wFace;
	m_dwChaHair_old    = m_dwChaHair   = (DWORD) pChar->m_wHair;
	m_nChaLiving_old   = m_nChaLiving  = pChar->m_nLiving;

    // 인벤토리 라인
    m_nInvenLine_old = m_nInvenLine = (int) pChar->m_wINVENLINE;

	m_nCurrentSlot_old = m_nCurrentSlot = static_cast< int >( pChar->m_wSlotIndex );

	for ( UINT i=0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
	{
		m_sCharSlotData_old[ i ] = m_sCharSlotData[ i ] = GLCharSlotMan::Instance()->GetSlotData( pChar->CharDbNum(), i );
		m_nSlotState_old[ i ] = m_nSlotState[ i ] = static_cast< int >( pChar->m_arrLockSlotState[ i ] );
	}

    UpdateData(FALSE);    
}

void CPropertyChaBase::GetData(SCHARDATA2* pChar)
{
    UpdateData(TRUE);
    
    pChar->m_CharDbNum = (DWORD) m_nChaNumDB;
	pChar->SetUserID( (DWORD)m_nUserNum );
	pChar->SetName(m_strChaName.GetString());

    pChar->m_emTribe    = EMTRIBE(m_nChaTribe);
    pChar->m_emClass    = EMCHARCLASS(m_nChaClass);
    pChar->m_wLevel     = (WORD) m_nChaLevel;
    pChar->m_wPK        = (WORD) m_nChaPK;
    pChar->m_nBright    = m_nChaBright;
    pChar->m_sHP			= m_chaHP;
    pChar->m_sMP			= m_chaMP;
	pChar->m_sSP			= m_chaSP;
    pChar->m_sCP.dwData = (DWORD) m_nChaCP;
    pChar->m_sExperience.lnNow = m_llChaExp;
	pChar->SetInvenMoney( m_llChaMoney );

    pChar->m_dwStartGate = (DWORD) m_nStartGate;
    pChar->m_sStartMapID.dwID = (DWORD) m_nStartMapID;

    pChar->m_vStartPos.x = m_fStartPosX;
    pChar->m_vStartPos.y = m_fStartPosY;
    pChar->m_vStartPos.z = m_fStartPosZ;

    pChar->m_sSaveMapID.dwID = (DWORD) m_nSaveMapID;

    pChar->m_vSavePos.x = m_fSavePosX;
    pChar->m_vSavePos.y = m_fSavePosY;
    pChar->m_vSavePos.z = m_fSavePosZ;

	pChar->m_wSchool = (WORD) m_dwChaSchool;
	pChar->m_wFace   = (WORD) m_dwChaFace;
	pChar->m_wHair   = (WORD) m_dwChaHair;
	pChar->m_nLiving = m_nChaLiving;

    pChar->m_sLastCallMapID.dwID  = (DWORD) m_nCallMapID;
    pChar->m_vLastCallPos.x       = m_fCallPosX;
	pChar->m_vLastCallPos.y       = m_fCallPosY;
	pChar->m_vLastCallPos.z       = m_fCallPosZ;

    pChar->m_wINVENLINE = (WORD) m_nInvenLine;

	pChar->m_wSlotIndex = static_cast< WORD >( m_nCurrentSlot );

	for ( UINT i=0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
	{
		GLCharSlotMan::Instance()->SaveCharSlot( m_nChaNumDB, i, m_sCharSlotData[ i ] );
		pChar->m_arrLockSlotState[ i ] = ( m_nSlotState[ i ] == 1 ) ? true : false;
	}
}

void CPropertyChaBase::OnBnClickedBtnBaseSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	///////////////////////////////////////////////////////////////////////////
	// GM Log
	CString strUserIP = m_pDBMan->GetLocalIP();

	int nGmNum = pFrame->m_nUserNum;
	std::strstream strGmCmd;
	strGmCmd << "Cha Old Data ";
	strGmCmd <<" " << m_nUserNum_old;
    strGmCmd <<" " << m_nChaNumDB_old;
    strGmCmd <<" " << m_strChaName_old.GetString();
    strGmCmd <<" " << m_nChaTribe_old;
    strGmCmd <<" " << m_nChaClass_old;
    strGmCmd <<" " << m_nChaLevel_old;
    strGmCmd <<" " << m_nChaPK_old;
    strGmCmd <<" " << m_nChaBright_old;
    strGmCmd <<" " << m_chaHP_old.nNow;
    strGmCmd <<" " << m_chaMP_old.nNow;
    strGmCmd <<" " << m_nChaCP_old;
    strGmCmd <<" " << m_llChaExp_old;
    strGmCmd <<" " << m_llChaMoney_old;
    strGmCmd <<" " << m_nStartGate_old;
    strGmCmd <<" " << m_nStartMapID_old;
    strGmCmd <<" " << m_fStartPosX_old;
    strGmCmd <<" " << m_fStartPosY_old;
    strGmCmd <<" " << m_fStartPosZ_old;
    strGmCmd <<" " << m_nSaveMapID_old;
    strGmCmd <<" " << m_fSavePosX_old;
    strGmCmd <<" " << m_fSavePosY_old;
    strGmCmd <<" " << m_fSavePosZ_old;	
	strGmCmd <<" " << m_dwChaSchool_old;
	strGmCmd <<" " << m_dwChaFace_old;
	strGmCmd <<" " << m_dwChaHair_old;
	strGmCmd <<" " << m_nChaLiving_old;   
    strGmCmd <<" " << m_nCallMapID_old;
    strGmCmd <<" " << m_fCallPosX_old;
    strGmCmd <<" " << m_fCallPosY_old;
    strGmCmd <<" " << m_fCallPosZ_old;
    strGmCmd <<" " << m_nInvenLine_old;
	strGmCmd <<" " << m_nCurrentSlot_old;

	for ( UINT i=0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
	{
		strGmCmd << " " << m_sCharSlotData_old[ i ].m_sStats.wPow;
		strGmCmd << " " << m_sCharSlotData_old[ i ].m_sStats.wStr;
		strGmCmd << " " << m_sCharSlotData_old[ i ].m_sStats.wSta;
		strGmCmd << " " << m_sCharSlotData_old[ i ].m_sStats.wSpi;
		strGmCmd << " " << m_sCharSlotData_old[ i ].m_sStats.wDex;
		strGmCmd << " " << m_sCharSlotData_old[ i ].m_sStats.wInt;
		strGmCmd << " " << m_sCharSlotData_old[ i ].m_wStatsPoint;
		strGmCmd << " " << m_sCharSlotData_old[ i ].m_dwSkillPoint; 
	}

	strGmCmd << '\0';	
	m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
	strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	///////////////////////////////////////////////////////////////////////////

	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;
	GetData(pChar);
	m_pDBMan->SaveCharacterBase(pChar);
	SetData(pChar);

	///////////////////////////////////////////////////////////////////////////
	// GM Log	
	std::strstream strGmCmd2;
	strGmCmd2 << "Cha New Data ";
	strGmCmd2 <<" " << m_nUserNum;
    strGmCmd2 <<" " << m_nChaNumDB;
    strGmCmd2 <<" " << m_strChaName.GetString();
    strGmCmd2 <<" " << m_nChaTribe;
    strGmCmd2 <<" " << m_nChaClass;
    strGmCmd2 <<" " << m_nChaLevel;
    strGmCmd2 <<" " << m_nChaPK;
    strGmCmd2 <<" " << m_nChaBright;
    strGmCmd2 <<" " << m_chaHP.nNow;
    strGmCmd2 <<" " << m_chaMP.nNow;
    strGmCmd2 <<" " << m_nChaCP;
    strGmCmd2 <<" " << m_llChaExp;
    strGmCmd2 <<" " << m_llChaMoney;
    strGmCmd2 <<" " << m_nStartGate;
    strGmCmd2 <<" " << m_nStartMapID;
    strGmCmd2 <<" " << m_fStartPosX;
    strGmCmd2 <<" " << m_fStartPosY;
    strGmCmd2 <<" " << m_fStartPosZ;
    strGmCmd2 <<" " << m_nSaveMapID;
    strGmCmd2 <<" " << m_fSavePosX;
    strGmCmd2 <<" " << m_fSavePosY;
    strGmCmd2 <<" " << m_fSavePosZ;	
	strGmCmd2 <<" " << m_dwChaSchool;
	strGmCmd2 <<" " << m_dwChaFace;
	strGmCmd2 <<" " << m_dwChaHair;
	strGmCmd2 <<" " << m_nChaLiving;  
    strGmCmd2 <<" " << m_nCallMapID;
    strGmCmd2 <<" " << m_fCallPosX;
    strGmCmd2 <<" " << m_fCallPosY;
    strGmCmd2 <<" " << m_fCallPosZ;
    strGmCmd2 <<" " << m_nInvenLine;
	strGmCmd2 <<" " << m_nCurrentSlot;

	for ( UINT i=0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
	{
		strGmCmd << " " << m_sCharSlotData[ i ].m_sStats.wPow;
		strGmCmd << " " << m_sCharSlotData[ i ].m_sStats.wStr;
		strGmCmd << " " << m_sCharSlotData[ i ].m_sStats.wSta;
		strGmCmd << " " << m_sCharSlotData[ i ].m_sStats.wSpi;
		strGmCmd << " " << m_sCharSlotData[ i ].m_sStats.wDex;
		strGmCmd << " " << m_sCharSlotData[ i ].m_sStats.wInt;
		strGmCmd << " " << m_sCharSlotData[ i ].m_wStatsPoint;
		strGmCmd << " " << m_sCharSlotData[ i ].m_dwSkillPoint; 
	}

	strGmCmd2 << '\0';
	m_pDBMan->WriteGMLog(nGmNum, strGmCmd2, strUserIP);
	strGmCmd2.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	///////////////////////////////////////////////////////////////////////////
}

void CPropertyChaBase::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	OnBnClickedBtnBaseSave();

	CPropertyPage::OnOK();
}

void CPropertyChaBase::OnBnClickedRadioCharSlotA ( void )
{
	m_nSelectSlot = 0;

	CheckRadioButton( IDC_RADIO_CHARSLOT_A, IDC_RADIO_CHARSLOT_E, IDC_RADIO_CHARSLOT_A );

	UpdateData(FALSE);
}

void CPropertyChaBase::OnBnClickedRadioCharSlotB ( void )
{
	m_nSelectSlot = 1;

	CheckRadioButton( IDC_RADIO_CHARSLOT_A, IDC_RADIO_CHARSLOT_E, IDC_RADIO_CHARSLOT_B );

	UpdateData(FALSE);
}

void CPropertyChaBase::OnBnClickedRadioCharSlotC ( void )
{
	m_nSelectSlot = 2;

	CheckRadioButton( IDC_RADIO_CHARSLOT_A, IDC_RADIO_CHARSLOT_E, IDC_RADIO_CHARSLOT_C );

	UpdateData(FALSE);
}

void CPropertyChaBase::OnBnClickedRadioCharSlotD ( void )
{
	m_nSelectSlot = 3;

	CheckRadioButton( IDC_RADIO_CHARSLOT_A, IDC_RADIO_CHARSLOT_E, IDC_RADIO_CHARSLOT_D );

	UpdateData(FALSE);
}

void CPropertyChaBase::OnBnClickedRadioCharSlotE ( void )
{
	m_nSelectSlot = 4;

	CheckRadioButton( IDC_RADIO_CHARSLOT_A, IDC_RADIO_CHARSLOT_E, IDC_RADIO_CHARSLOT_E );

	UpdateData(FALSE);
}