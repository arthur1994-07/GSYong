// AutoLevelPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#define _USE_TBB
#include "../SpecialTool.h"
#include "./AutoLevelPage.h"
#include "../SheetWithTab.h"
#include "../SpecialToolDlg.h"

#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/GameTextLoader.h"
#include "../../EngineLib/DxTools/DxFontDefine.h"
#include "../../EngineLib/DxTools/DxFontMan.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogicServer/Land/GLLandMan.h"
#include "../../RanLogicServer/FieldServer/GLGaeaServer.h"


// CAutoLevelPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAutoLevelPage, CPropertyPage)
CAutoLevelPage::CAutoLevelPage( LOGFONT logfont )
	: CPropertyPage(CAutoLevelPage::IDD)
	, m_pFont(NULL)
	, m_bEdit(false)
    , m_pAutoLevel(NULL)
{
	CSpecialToolApp* pFrame = (CSpecialToolApp*)AfxGetMainWnd();
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);

    m_pAutoLevel = new GLAutoLevelMan(NULL);
}

CAutoLevelPage::~CAutoLevelPage()
{
    SAFE_DELETE(m_pAutoLevel);
}

void CAutoLevelPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_AUTO_LEVEL, m_cAutoLevelList);
	DDX_Control(pDX, IDC_LIST_AUTOLEVEL_CMD, m_cAutoLevelCMDList);
}


BEGIN_MESSAGE_MAP(CAutoLevelPage, CPropertyPage)
	ON_LBN_SELCHANGE(IDC_LIST_AUTO_LEVEL, OnLbnSelchangeListAutoLevel)
	ON_BN_CLICKED(IDC_BUTTON_AUTOLEVEL_LOADOLD, OnBnClickedButtonAutolevelLoadold)
	ON_BN_CLICKED(IDC_BUTTON_AUTOLEVEL_ADD, OnBnClickedButtonAutolevelAdd)
	ON_BN_CLICKED(IDC_BUTTON_AUTOLEVEL_DEL, OnBnClickedButtonAutolevelDel)
	//ON_BN_CLICKED(IDC_BUTTON_AUTOLEVEL_CONVERT_TO_LAYER, OnBnClickedButtonAutolevelConvert)
	ON_BN_CLICKED(IDC_BUTTON_AUTOLEVEL_SET, OnBnClickedButtonAutolevelSet)
	ON_BN_CLICKED(IDC_BUTTON_COMMNAD_ADD, OnBnClickedButtonCommnadAdd)
	ON_BN_CLICKED(IDC_BUTTON_COMMNAD_DEL, OnBnClickedButtonCommnadDel)
	ON_BN_CLICKED(IDC_BUTTON_COMMNAD_UP, OnBnClickedButtonCommnadUp)
	ON_BN_CLICKED(IDC_BUTTON_COMMNAD_DOWN, OnBnClickedButtonCommnadDown)
	ON_CBN_SELCHANGE(IDC_COMBO_COMMAND, OnCbnSelchangeComboCommand)
	ON_BN_CLICKED(IDC_BUTTON_AUTOLEVEL_CSVSAVE, OnBnClickedButtonAutolevelCsvsave)
	ON_BN_CLICKED(IDC_BUTTON_AUTOLEVEL_CSVLOAD, OnBnClickedButtonAutolevelCsvload)
	ON_NOTIFY(NM_CLICK, IDC_LIST_AUTOLEVEL_CMD, OnNMClickListAutolevelCmd)
	ON_BN_CLICKED(IDC_BUTTON_COMMNAD_EDIT, OnBnClickedButtonCommnadEdit)
	ON_BN_CLICKED(IDC_BUTTON_CONVERT_XML, OnBnClickedButtonConvertXml)
END_MESSAGE_MAP()


// CAutoLevelPage 메시지 처리기입니다.

BOOL CAutoLevelPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );	

	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	m_cAutoLevelCMDList.SetExtendedStyle ( m_cAutoLevelCMDList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_cAutoLevelCMDList.GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	const int nColumnCount = 3;	
	char* szColumnName1[nColumnCount] = { "Time", "Command", "Msg" };

	int nColumnWidthPercent1[nColumnCount] = { 7, 20, 73 };

	for ( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.pszText = szColumnName1[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent1[i] ) / 100;
		m_cAutoLevelCMDList.InsertColumn (i, &lvColumn );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAutoLevelPage::Init()
{
	m_pAutoLevel->CleanUp();
	m_pAutoLevel->LOAD( std::string("AutoLevelSet.alsf") );

	SetWin_Combo_Init ( this, IDC_COMBO_COMMAND, strCommandList, CMD_SIZE );
	SetWin_Combo_Sel( this, IDC_COMBO_COMMAND, CMD_MSG );
	SetWin_Text( this, IDC_EDIT_CMD_MSG, strCommandHelp[CMD_MSG].c_str() );

	UpdateItems();
	m_bEdit = false;
}

void CAutoLevelPage::UpdateItems ()
{
	m_cAutoLevelList.ResetContent();

	GLAutoLevelVector& _autoLevelVector = m_pAutoLevel->m_autoLevelVector;

	for ( GLAutoLevelVectorIter _iteratorLevel(_autoLevelVector.begin()); _iteratorLevel != _autoLevelVector.end(); ++_iteratorLevel )
	{
		GLAutoLevel& glAutoLevel = *_iteratorLevel;
		m_cAutoLevelList.AddString( glAutoLevel.m_strName.c_str() );
	}

	m_AutoLevel = GLAutoLevel();

	AutoLevelUpdateItems();
	AutoLevelCommandUpdateItems();


}

void CAutoLevelPage::AutoLevelUpdateItems()
{
	SetWin_Check( this, IDC_CHECK_EVENT_USE, m_AutoLevel.IsUse() );
	SetWin_Check( this, IDC_CHECK_EVENT_PVP, m_AutoLevel.m_bEvent );

	SetWin_Text( this, IDC_EDIT_EVENT_NAME, m_AutoLevel.m_strName.c_str() );

	SetWin_Check( this, IDC_CHECK_DAY1, m_AutoLevel.m_dwDay[0] );
	SetWin_Check( this, IDC_CHECK_DAY2, m_AutoLevel.m_dwDay[1] );
	SetWin_Check( this, IDC_CHECK_DAY3, m_AutoLevel.m_dwDay[2] );
	SetWin_Check( this, IDC_CHECK_DAY4, m_AutoLevel.m_dwDay[3] );
	SetWin_Check( this, IDC_CHECK_DAY5, m_AutoLevel.m_dwDay[4] );
	SetWin_Check( this, IDC_CHECK_DAY6, m_AutoLevel.m_dwDay[5] );
	SetWin_Check( this, IDC_CHECK_DAY7, m_AutoLevel.m_dwDay[6] );

	SNATIVEID sNid(m_AutoLevel.m_dwMapID);
	SetWin_Num_int( this, IDC_EDIT_EVENT_MAP_MID, sNid.wMainID );
	SetWin_Num_int( this, IDC_EDIT_EVENT_MAP_SID, sNid.wSubID );

	SetWin_Num_int( this, IDC_EDIT_EVENT_MAP_GATE, m_AutoLevel.m_dwGateID );

	sNid.dwID = m_AutoLevel.m_dwHallMapID;
	SetWin_Num_int( this, IDC_EDIT_EVENT_HALLMAP_MID, sNid.wMainID );
	SetWin_Num_int( this, IDC_EDIT_EVENT_HALLMAP_SID, sNid.wSubID );

	SetWin_Num_int( this, IDC_EDIT_STARTTIME_HOUR, m_AutoLevel.m_StartTime.nHour );
	SetWin_Num_int( this, IDC_EDIT_STARTTIME_MIN, m_AutoLevel.m_StartTime.nMinute );

	SetWin_Num_int( this, IDC_EDIT_ENDTIME_HOUR, m_AutoLevel.m_EndTime.nHour );
	SetWin_Num_int( this, IDC_EDIT_ENDTIME_MIN, m_AutoLevel.m_EndTime.nMinute );

	
	SetWin_Num_int( this, IDC_EDIT_POS1_X, m_AutoLevel.m_dwOutPos[0].m_dwPOS_X );
	SetWin_Num_int( this, IDC_EDIT_POS1_Y, m_AutoLevel.m_dwOutPos[0].m_dwPOS_Y );

	SetWin_Num_int( this, IDC_EDIT_POS2_X, m_AutoLevel.m_dwOutPos[1].m_dwPOS_X );
	SetWin_Num_int( this, IDC_EDIT_POS2_Y, m_AutoLevel.m_dwOutPos[1].m_dwPOS_Y );

	SetWin_Num_int( this, IDC_EDIT_POS3_X, m_AutoLevel.m_dwOutPos[2].m_dwPOS_X );
	SetWin_Num_int( this, IDC_EDIT_POS3_Y, m_AutoLevel.m_dwOutPos[2].m_dwPOS_Y );

	SetWin_Num_int( this, IDC_EDIT_POS4_X, m_AutoLevel.m_dwOutPos[3].m_dwPOS_X );
	SetWin_Num_int( this, IDC_EDIT_POS4_Y, m_AutoLevel.m_dwOutPos[3].m_dwPOS_Y );

	SetWin_Num_int( this, IDC_EDIT_POS5_X, m_AutoLevel.m_dwOutPos[4].m_dwPOS_X );
	SetWin_Num_int( this, IDC_EDIT_POS5_Y, m_AutoLevel.m_dwOutPos[4].m_dwPOS_Y );

	//	Init
	SetWin_Combo_Init ( this, IDC_COMBO_COMMAND, strCommandList, CMD_SIZE );
	SetWin_Combo_Sel( this, IDC_COMBO_COMMAND, CMD_MSG );
	SetWin_Text( this, IDC_EDIT_CMD_MSG, strCommandHelp[CMD_MSG].c_str() );
	SetWin_Text( this, IDC_EDIT_CMDTIME_HOUR, "" );
	SetWin_Text( this, IDC_EDIT_CMDTIME_MIN, "" );

}

void CAutoLevelPage::AutoLevelCommandUpdateItems() 
{
	m_cAutoLevelCMDList.DeleteAllItems();
	int nSize = m_AutoLevel.m_vecLevScript.size();

	for ( int i = 0; i < nSize; ++i )
	{
		GLLEV_SCRIPT& slevScript = m_AutoLevel.m_vecLevScript[i];
		std::string strScriptCMD = m_AutoLevel.GetScriptTextCommnad( slevScript );
		std::string strScriptMsg;			
		if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
			strScriptMsg = CGameTextMan::GetInstance().GetAutoLevelSetText(m_AutoLevel.m_strName.c_str(), i).GetString();
		else
			strScriptMsg = strScriptMsg = slevScript.strArgument;
			
		CString strTime;
		strTime.Format( "%02d/%02d", slevScript.levTime.nHour, slevScript.levTime.nMinute );

		int nIndex = m_cAutoLevelCMDList.InsertItem( i, strTime );
		if ( nIndex == -1 ) continue;

//      m_cAutoLevelCMDList.SetItemData( nIndex, pMapNode->sNativeID.dwID );

		m_cAutoLevelCMDList.SetItemText( nIndex, 1, strScriptCMD.c_str() );
		m_cAutoLevelCMDList.SetItemText( nIndex, 2, strScriptMsg.c_str() );
	}
}

void CAutoLevelPage::InverseUpdateItems()
{
}

void CAutoLevelPage::InverseAutoLevelUpdateItems()
{
	m_AutoLevel.SetUse( (bool)GetWin_Check( this, IDC_CHECK_EVENT_USE ) );
	m_AutoLevel.m_bEvent = (bool)GetWin_Check( this, IDC_CHECK_EVENT_PVP );

	m_AutoLevel.m_strName = GetWin_Text( this, IDC_EDIT_EVENT_NAME );

	m_AutoLevel.m_dwDay[0] = GetWin_Check( this, IDC_CHECK_DAY1 );
	m_AutoLevel.m_dwDay[1] = GetWin_Check( this, IDC_CHECK_DAY2 );
	m_AutoLevel.m_dwDay[2] = GetWin_Check( this, IDC_CHECK_DAY3 );
	m_AutoLevel.m_dwDay[3] = GetWin_Check( this, IDC_CHECK_DAY4 );
	m_AutoLevel.m_dwDay[4] = GetWin_Check( this, IDC_CHECK_DAY5 );
	m_AutoLevel.m_dwDay[5] = GetWin_Check( this, IDC_CHECK_DAY6 );
	m_AutoLevel.m_dwDay[6] = GetWin_Check( this, IDC_CHECK_DAY7 );

	SNATIVEID sNid;
	sNid.wMainID = GetWin_Num_int( this, IDC_EDIT_EVENT_MAP_MID );
	sNid.wSubID = GetWin_Num_int( this, IDC_EDIT_EVENT_MAP_SID );
	m_AutoLevel.m_dwMapID = sNid.dwID;

	m_AutoLevel.m_dwGateID = GetWin_Num_int( this, IDC_EDIT_EVENT_MAP_GATE );

	sNid.wMainID = GetWin_Num_int( this, IDC_EDIT_EVENT_HALLMAP_MID );
	sNid.wSubID = GetWin_Num_int( this, IDC_EDIT_EVENT_HALLMAP_SID );	
	m_AutoLevel.m_dwHallMapID = sNid.dwID;

	m_AutoLevel.m_StartTime.nHour = GetWin_Num_int( this, IDC_EDIT_STARTTIME_HOUR );	
	m_AutoLevel.m_StartTime.nMinute = GetWin_Num_int( this, IDC_EDIT_STARTTIME_MIN );	

	m_AutoLevel.m_EndTime.nHour = GetWin_Num_int( this, IDC_EDIT_ENDTIME_HOUR );	
	m_AutoLevel.m_EndTime.nMinute = GetWin_Num_int( this, IDC_EDIT_ENDTIME_MIN );	

	m_AutoLevel.m_dwOutPos[0].m_dwPOS_X = GetWin_Num_int( this, IDC_EDIT_POS1_X );
	m_AutoLevel.m_dwOutPos[0].m_dwPOS_Y = GetWin_Num_int( this, IDC_EDIT_POS1_Y );

	m_AutoLevel.m_dwOutPos[1].m_dwPOS_X = GetWin_Num_int( this, IDC_EDIT_POS2_X );
	m_AutoLevel.m_dwOutPos[1].m_dwPOS_Y = GetWin_Num_int( this, IDC_EDIT_POS2_Y );

	m_AutoLevel.m_dwOutPos[2].m_dwPOS_X = GetWin_Num_int( this, IDC_EDIT_POS3_X );
	m_AutoLevel.m_dwOutPos[2].m_dwPOS_Y = GetWin_Num_int( this, IDC_EDIT_POS3_Y );

	m_AutoLevel.m_dwOutPos[3].m_dwPOS_X = GetWin_Num_int( this, IDC_EDIT_POS4_X );
	m_AutoLevel.m_dwOutPos[3].m_dwPOS_Y = GetWin_Num_int( this, IDC_EDIT_POS4_Y );

	m_AutoLevel.m_dwOutPos[4].m_dwPOS_X = GetWin_Num_int( this, IDC_EDIT_POS5_X );
	m_AutoLevel.m_dwOutPos[4].m_dwPOS_Y = GetWin_Num_int( this, IDC_EDIT_POS5_Y );
}

bool CAutoLevelPage::NewFile()
{
	m_pAutoLevel->CleanUp();
	UpdateItems();
	return true;
}

bool CAutoLevelPage::LoadFile()
{
	CString szFilter = "AutoLevel Setting File (*.alsf)|*.alsf|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".alsf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetServerPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		std::string strFileName = dlg.GetFileName();
		hr = m_pAutoLevel->LOAD ( strFileName );

		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Load File" );
			return false;
		}

		UpdateItems();
		m_bEdit = false;
	}

	return true;
}

bool CAutoLevelPage::LoadFileOld()
{
	CString szFilter = "AutoLevel Setting File (*.ini)|*.ini|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".ini",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetServerPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		std::string strFileName = dlg.GetFileName();
		hr = m_pAutoLevel->LOAD_OLD ( strFileName, true );

		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Load File OLD" );
			return false;
		}

		UpdateItems();
		m_bEdit = false;
	}


	return true;
}

bool CAutoLevelPage::SaveFile()
{
	CString szFilter = "AutoLevel Setting File (*.alsf)|*.alsf|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".alsf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetServerPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		std::string strFileName = dlg.GetFileName();
		hr = m_pAutoLevel->SAVE( strFileName );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save File" );
			return false;
		}

		m_bEdit = false;
	}

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate())
		CGameTextMan::GetInstance().SaveXML(RANPARAM::strAutoLevelSetText.GetString(), CGameTextMan::EM_AUTOLEVELSET_TEXT, FALSE);

	return true;
}

bool CAutoLevelPage::CsvSaveAll()
{
	CString szFilter = "AutoLevelMan Csv File (*.csv)|*.csv|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".csv",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetServerPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		std::string strFileName = dlg.GetFileName();
		hr = m_pAutoLevel->CSV_SAVE( strFileName );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "GLAutoLevelMan::CSV_SAVE_TIME, Error GLAutoLevelMan", "", MB_OK );
			return false;
		}

		return true;
	}

	return true;

}

bool CAutoLevelPage::CsvSave()
{
	CString szFilter = "AutoLevel Csv File (*.csv)|*.csv|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".csv",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetServerPath ();
	if ( dlg.DoModal() == IDOK )
	{
		std::string strFileName = dlg.GetFileName();
	
		std::fstream streamFILE;
		streamFILE.open ( strFileName.c_str(), std::ios_base::out );
		bool bOK = m_AutoLevel.CSV_SAVE( streamFILE );
		streamFILE.close();
		
		if ( !bOK )
		{
			MessageBox ( "GLAutoLevel::CSV_SAVE, Error GLAutoLevel", "", MB_OK );
			return false;
		}

		return true;
	}

	return true;	
}

bool CAutoLevelPage::CsvLoadAll()
{
	CString szFilter = "AutoLevelMan Csv File (*.csv)|*.csv|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".csv",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetServerPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		std::string strFileName = dlg.GetFileName();
		hr = m_pAutoLevel->CSV_LOAD ( strFileName );

		if ( FAILED ( hr ) )
		{
			MessageBox ( "Error GLAutoLevelMan::CSV_LOAD" );
			return false;
		}

		UpdateItems();
		m_bEdit = false;
	}

	return true;
}

bool CAutoLevelPage::CsvLoad()
{
	CString szFilter = "AutoLevel Csv File (*.csv)|*.csv|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".csv",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetServerPath ();
	if ( dlg.DoModal() == IDOK )
	{
		std::string strFileName = dlg.GetFileName();
	
		sc::CStringFile StrFile( 10240 );
		if( !StrFile.Open ( strFileName.c_str() ) )
			return false;

		bool bOK = m_AutoLevel.CSV_LOAD( StrFile );
				
		if ( !bOK )
		{
			MessageBox ( "GLAutoLevel::CSV_SAVE, Error GLAutoLevel", "", MB_OK );
			return false;
		}
		
		AutoLevelUpdateItems();
		AutoLevelCommandUpdateItems();

		return true;
	}

	return true;	
	
}

void CAutoLevelPage::OnLbnSelchangeListAutoLevel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nIndex = m_cAutoLevelList.GetCurSel();
	int nListSize = m_cAutoLevelList.GetCount();

	if ( nIndex < 0 || nIndex >= nListSize )
		return;

	GLAutoLevel* pGlAutoLevel = m_pAutoLevel->GetAutoLevel( nIndex );
	if (!pGlAutoLevel)
        return;

	m_AutoLevel = *pGlAutoLevel;

	AutoLevelUpdateItems();	
	AutoLevelCommandUpdateItems();
}

void CAutoLevelPage::OnBnClickedButtonAutolevelLoadold()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "AutoLevel File (*.als)|*.als|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".als",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetServerPath ();
	if ( dlg.DoModal() == IDOK )
	{
		std::string strFileName = dlg.GetFileName();
		GLAutoLevel glAutoLevel;
		if ( glAutoLevel.LOAD_OLD( strFileName, m_pAutoLevel->GetCommandList(), true) == false )
		{
			MessageBox( "AutoLevel LoadOld Error" );
			return;
		}

		m_pAutoLevel->m_autoLevelVector.push_back(glAutoLevel);

		UpdateItems();
		m_bEdit = false;
	}
}
//
//// 전 국가 1.5R3 이상으로 변경 된 후에는 OnBnClickedButtonAutolevelConvert() 지울 것;
//void CAutoLevelPage::OnBnClickedButtonAutolevelConvert()
//{
////	const int _nResult(AfxMessageBox(_T("* 작업 시작 전에 glogic/~~~.lev, glogicSerer/AutoLevelSet.alsf 파일들을\n- 백업 해두시는 편이 좋습니다(CRM#1650 참조);\n- http://crm.mincoms.com/Ticket/TicketView/1650\n* 작업이 완료 되기까지 약간의 시간이 소요 됩니다;\n* [오류] 창이 뜰 경우 바로 R&D팀으로 알려 주시기 바랍니다;\n\n작업 완료 후 저장하기 창이 뜹니다;\n반드시 저장 해주세요;\n\n준비 되셨으면 [예(Y)] 버튼을 눌러 주세요;"), MB_YESNO | MB_ICONSTOP));
//	const int _nResult(AfxMessageBox(_T("* better to back-up these Files(check CRM#1650)\n- [glogic/~~~.lev, glogicSerer/AutoLevelSet.alsf];\n- http://crm.mincoms.com/Ticket/TicketView/1650\n- it takes a little time to complete;\n* if you see [Error] box, must call R&D, plz;\n\nyou'll see [Save Window] after complete\nAfter completion, you'll see a Save-Window;\nMUST SAVE IT!!!\nOnce you have checked it, press [Yes];"), MB_YESNO | MB_ICONSTOP));
//	if ( _nResult == IDNO )
//		return;
//
//	CSpecialToolApp* pApp = (CSpecialToolApp*) AfxGetApp();
//	GLGaeaServer* const _pGaeaServer(new GLGaeaServer(pApp->GetServiceProvider(), pApp->GetSubPath()));
//	if ( FAILED(_pGaeaServer->LoadMapList(FALSE, NULL, FIELDSERVER_MAX)) )
//	{
//		AfxMessageBox(_T("ERROR!! Failed to LOAD MapsList;"), MB_YESNO | MB_ICONSTOP);
//		return;
//	}
//	
//	ConvTemp::AutoLevelConverter _autoLevelConverter;
//	const unsigned int _nAutoLevelSize(m_pAutoLevel->GetNAutoLevel());
//	for ( unsigned int _nIndex(0); _nIndex < _nAutoLevelSize; ++_nIndex )
//	{
//		GLAutoLevel* const _pAutoLevel(m_pAutoLevel->GetAutoLevel(_nIndex));
//		if ( _pAutoLevel == 0 )
//		{
//			AfxMessageBox(_T("ERROR!! pGlAutoLevel = NULL"), MB_YESNO | MB_ICONSTOP);
//			return;
//		}
//
//		const DWORD _mapID(_pAutoLevel->m_dwMapID);
//		const DWORD _hallMapID(_pAutoLevel->m_dwHallMapID);
//
//		SMAPNODE* const _pHallMapNode(_pGaeaServer->FindMapNode(SNATIVEID(_hallMapID)));
//		SMAPNODE* const _pMapNode(_pGaeaServer->FindMapNode(SNATIVEID(_mapID)));
//		if ( _pHallMapNode == 0 || _pMapNode == 0 )
//		{
//			AfxMessageBox(_T("ERROR!! INVALID [mapID or hallMapID];"));
//			return;
//		}
//
//		_autoLevelConverter.addMap(_hallMapID, _pHallMapNode->m_LevelFile);
//		_autoLevelConverter.addMap(_mapID, _pMapNode->m_LevelFile);
//
//		const unsigned int _nElementSize(_pAutoLevel->m_vecLevScript.size());		
//		for ( unsigned int _j(0); _j < _nElementSize; ++_j )
//		{
//			GLLEV_SCRIPT& _element = _pAutoLevel->m_vecLevScript[_j];
//			std::string _message;
//			if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
//				_message = CGameTextMan::GetInstance().GetAutoLevelSetText(m_AutoLevel.m_strName.c_str(), _j).GetString();
//			else
//				_message = _element.strArgument;
//
//			_element.levTime -= _pAutoLevel->m_StartTime;
//			switch ( _element.emCommand )
//			{
//			case CMD_CALL_HALL_MAP:
//				{
//					const unsigned int _nLayer(_autoLevelConverter.addMap(_hallMapID, _message));
//					if ( _nLayer == -1 )
//					{
//						AfxMessageBox(_T("Already Converted;"), MB_OK | MB_ICONSTOP);
//						return;
//					}
//					_message = sc::string::format("%1%", _nLayer);
//				}
//				break;
//			case CMD_CALL_MAP:
//				{
//					const unsigned int _nLayer(_autoLevelConverter.addMap(_mapID, _message));
//					if ( _nLayer == -1 )
//					{
//						AfxMessageBox(_T("Already Converted;"), MB_OK | MB_ICONSTOP);
//						return;
//					}
//					_message = sc::string::format("%1%", _nLayer);
//				}
//				break;
//			}
//		}
//	}
//
//	CSpecialToolDlg* pDlg = (CSpecialToolDlg*)this->GetParent()->GetParent();
//	ConvTemp::AutoLevelMap& _autoLevelMap = _autoLevelConverter.autoLevelMap;
//	for ( ConvTemp::AutoLevelMapCIter _iteratorAutoLevel(_autoLevelMap.begin()); _iteratorAutoLevel != _autoLevelMap.end(); ++_iteratorAutoLevel )
//	{
//		const ConvTemp::AutoLevel& _autoLevel = _iteratorAutoLevel->second;
//
//		GLLandMan _land(_pGaeaServer, NULL, SNATIVEID());
//		_land.SetD3dDevice(pDlg->GetD3DDevice());
//		_land.ReSetMap();
//		const ConvTemp::LevelFileNameVector& _mapFileVector = _autoLevel.mapFileNameVector;
//		ConvTemp::LevelFileNameVectorCIter _iteratorLevelFile(_mapFileVector.begin());
//		if ( _iteratorLevelFile == _mapFileVector.end() )
//			continue;
//
//// 		CSpecialToolDlg* pDlg = ((CSpecialToolDlg*)this->GetParent()->GetParent());
//// 		LPDIRECT3DDEVICEQ lpd3ddevice = pDlg->GetD3DDevice();
//		const std::string& _rootLevelFileName = *_iteratorLevelFile;
//		if ( _land.LoadFile(_rootLevelFileName.c_str(), 0) == FALSE )
//		{
//			AfxMessageBox(_T("ERROR!! : Failed to LOAD [Level File;"), MB_OK | MB_ICONSTOP);
//			return;
//		}
//		++_iteratorLevelFile;
//
//		for ( ; _iteratorLevelFile != _mapFileVector.end(); ++_iteratorLevelFile )
//		{
//			const std::string& _levelFileName = *_iteratorLevelFile;
//			if ( _land.ImportLayer(_levelFileName.c_str(), 0) == FALSE )
//			{
//				AfxMessageBox(_T("ERROR!! : Failed to IMPORT [Level File];"), MB_OK | MB_ICONSTOP);
//				return;
//			}
//		}
//
//		if ( _land.SaveFile(_rootLevelFileName.c_str()) == FALSE )
//		{
//			AfxMessageBox(_T("ERROR!! : Failed to STRORE [Level File];"), MB_OK | MB_ICONSTOP);
//			return;
//		}
//	}
//
//	const bool _bSuccess(CAutoLevelPage::SaveFile());
//	if ( _bSuccess == true )
//	{
//		AfxMessageBox(_T("!!!!!Complete!!!!!"), MB_OK | MB_ICONSTOP);
//	}
//
//	UpdateItems();
//
//	delete _pGaeaServer;
//
//// 	pLand->ReSetMap();
//}

void CAutoLevelPage::OnBnClickedButtonAutolevelAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	GLAutoLevel glAutoLevel;
	glAutoLevel.m_strName = "New Event";
	
	m_pAutoLevel->m_autoLevelVector.push_back(glAutoLevel);
	
	UpdateItems();
	
	m_bEdit = true;
}

void CAutoLevelPage::OnBnClickedButtonAutolevelDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_cAutoLevelList.GetCurSel();
	int nSize = m_cAutoLevelList.GetCount();

	if ( nIndex < 0 || nIndex >= nSize ) return;

	if ( !m_pAutoLevel->DelAutoLevel( nIndex )) 
	{
		MessageBox( "AutoLevel Delete Error" );
		return;	
	}

	UpdateItems();

	m_bEdit = true;

}

void CAutoLevelPage::OnBnClickedButtonAutolevelSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nIndex = m_cAutoLevelList.GetCurSel();
	if ( nIndex < 0 ) return;

	InverseAutoLevelUpdateItems();

	int nResult = m_AutoLevel.CheckCommandTime();

	if ( nResult != -1 )
	{
		CString strTemp;
		strTemp.Format ( " CommandTime Error %d Line", nResult );
		MessageBox( strTemp );
		return;
	}

	GLAutoLevel* pGlAutoLevel = m_pAutoLevel->GetAutoLevel( nIndex );
	if (!pGlAutoLevel)
        return;

	*pGlAutoLevel = m_AutoLevel;

	UpdateItems();

	m_cAutoLevelList.SetCurSel( -1 );

	m_bEdit = true;
}

void CAutoLevelPage::OnBnClickedButtonCommnadAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	GLLEV_SCRIPT glevelScript;

	glevelScript.emCommand = (GLLEV_COMMAND)GetWin_Combo_Sel( this, IDC_COMBO_COMMAND );
	glevelScript.levTime.nHour = GetWin_Num_int( this, IDC_EDIT_CMDTIME_HOUR );
	glevelScript.levTime.nMinute = GetWin_Num_int( this, IDC_EDIT_CMDTIME_MIN );

	if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
	{
		CGameTextMan::GetInstance().InsertString(m_AutoLevel.m_strName.c_str(), GetWin_Text( this, IDC_EDIT_CMD_MSG ).GetString(), CGameTextMan::EM_AUTOLEVELSET_TEXT, (int)m_AutoLevel.m_vecLevScript.size());
	}
	else
	{
		glevelScript.strArgument = GetWin_Text( this, IDC_EDIT_CMD_MSG );
	}

	if ( !m_AutoLevel.CheckScript( glevelScript ) )
	{
		MessageBox( "Command Data Error", MB_OK );
		return;
	}

	m_AutoLevel.AddScript( glevelScript );

	AutoLevelCommandUpdateItems();

	int nSize = m_cAutoLevelCMDList.GetItemCount();
	m_cAutoLevelCMDList.SetItemState( nSize-1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

	CSize sz(0,9999);
	m_cAutoLevelCMDList.Scroll(sz);
}

void CAutoLevelPage::OnBnClickedButtonCommnadDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelect = m_cAutoLevelCMDList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 ) return;

	if ( IDNO == MessageBox ( "Delete?", "Caution", MB_YESNO ) )	return;

	if ( !m_AutoLevel.DelScript( nSelect ) )
	{
		MessageBox( "Delete Error", MB_OK );
		return;
	}

	int nPos = m_cAutoLevelCMDList.GetScrollPos( SB_VERT );

	AutoLevelCommandUpdateItems();

	CRect ref;
	m_cAutoLevelCMDList.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정
	
	CSize sz(0,nPos*ref.Height());
	m_cAutoLevelCMDList.Scroll(sz);
}

void CAutoLevelPage::OnBnClickedButtonCommnadUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelect = m_cAutoLevelCMDList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect <= 0 ) return;

	int nSize = m_AutoLevel.m_vecLevScript.size();
	if ( nSelect >= nSize ) return;
	
	GLLEV_SCRIPT sLevelScript = m_AutoLevel.m_vecLevScript[nSelect];
	m_AutoLevel.m_vecLevScript[nSelect] = m_AutoLevel.m_vecLevScript[nSelect-1];
	m_AutoLevel.m_vecLevScript[nSelect-1] = sLevelScript;

	int nPos = m_cAutoLevelCMDList.GetScrollPos( SB_VERT );
	
	int nTop = m_cAutoLevelCMDList.GetTopIndex();
	int nPage = m_cAutoLevelCMDList.GetCountPerPage();

	AutoLevelCommandUpdateItems();

	m_cAutoLevelCMDList.SetItemState( --nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

	//	스크롤바 설정
	{
		CRect ref;
		m_cAutoLevelCMDList.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정
		
		int nSize = 0;
		
		if ( nSelect < nTop+1 ) nSize = (nSelect-1) * ref.Height();
		else nSize = nPos * ref.Height();

		CSize sz( 0, nSize );
		m_cAutoLevelCMDList.Scroll(sz);
	}
}

void CAutoLevelPage::OnBnClickedButtonCommnadDown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelect = m_cAutoLevelCMDList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	int nSize = m_AutoLevel.m_vecLevScript.size();
	if( nSelect < 0 || nSelect >= nSize-1 ) return;

	GLLEV_SCRIPT sLevelScript = m_AutoLevel.m_vecLevScript[nSelect];
	m_AutoLevel.m_vecLevScript[nSelect] = m_AutoLevel.m_vecLevScript[nSelect+1];
	m_AutoLevel.m_vecLevScript[nSelect+1] = sLevelScript;

	int nPos = m_cAutoLevelCMDList.GetScrollPos( SB_VERT );

	int nTop = m_cAutoLevelCMDList.GetTopIndex();
	int nPage = m_cAutoLevelCMDList.GetCountPerPage();

	AutoLevelCommandUpdateItems();

	m_cAutoLevelCMDList.SetItemState( ++nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 
	
	
	//	스크롤바 설정
	{
		CRect ref;
		m_cAutoLevelCMDList.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정

		int nSize = 0;

		if ( nSelect+1 >= ( nTop + nPage ) ) nSize = ( nSelect-nPage+2 ) * ref.Height();
		else nSize = nPos * ref.Height();		

		CSize sz( 0, nSize );
		m_cAutoLevelCMDList.Scroll(sz);
	}
}

void CAutoLevelPage::OnCbnSelchangeComboCommand()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = GetWin_Combo_Sel( this, IDC_COMBO_COMMAND );

	if ( nIndex < 0 || nIndex >= CMD_SIZE )	return;

	SetWin_Text( this, IDC_EDIT_CMD_MSG, strCommandHelp[nIndex].c_str() );
}
void CAutoLevelPage::OnBnClickedButtonAutolevelCsvsave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_cAutoLevelList.GetCurSel();
	if ( nIndex < 0 ) return;
	
	CsvSave();
}

void CAutoLevelPage::OnBnClickedButtonAutolevelCsvload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_cAutoLevelList.GetCurSel();
	if ( nIndex < 0 ) return;

	CsvLoad();
}

void CAutoLevelPage::OnNMClickListAutolevelCmd(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	NMITEMACTIVATE* pNMIA = ( NMITEMACTIVATE* ) pNMHDR;

	if ( pNMIA->iItem != -1 )
	{
		int nSize = m_AutoLevel.m_vecLevScript.size();
		if ( nSize <= pNMIA->iItem )	return;
		
		GLLEV_SCRIPT sLevelScript = m_AutoLevel.m_vecLevScript[pNMIA->iItem];

		SetWin_Combo_Sel( this, IDC_COMBO_COMMAND, sLevelScript.emCommand );

		if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
			SetWin_Text( this, IDC_EDIT_CMD_MSG, CGameTextMan::GetInstance().GetAutoLevelSetText(m_AutoLevel.m_strName.c_str(), pNMIA->iItem).GetString() );
		else
			SetWin_Text( this, IDC_EDIT_CMD_MSG, sLevelScript.strArgument.c_str() );

		SetWin_Num_int( this, IDC_EDIT_CMDTIME_HOUR, sLevelScript.levTime.nHour );
		SetWin_Num_int( this, IDC_EDIT_CMDTIME_MIN, sLevelScript.levTime.nMinute );
	}

	*pResult = 0;
}

void CAutoLevelPage::OnBnClickedButtonCommnadEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelect = m_cAutoLevelCMDList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect <= 0 ) return;

	int nSize = m_AutoLevel.m_vecLevScript.size();
	if ( nSelect >= nSize ) return;

	GLLEV_SCRIPT glevelScript;

	glevelScript.emCommand = (GLLEV_COMMAND)GetWin_Combo_Sel( this, IDC_COMBO_COMMAND );
	glevelScript.levTime.nHour = GetWin_Num_int( this, IDC_EDIT_CMDTIME_HOUR );
	glevelScript.levTime.nMinute = GetWin_Num_int( this, IDC_EDIT_CMDTIME_MIN );


	if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
	{
		CGameTextMan::GetInstance().InsertString(m_AutoLevel.m_strName.c_str(), GetWin_Text( this, IDC_EDIT_CMD_MSG ).GetString(), CGameTextMan::EM_AUTOLEVELSET_TEXT, nSelect);
	}
	else
	{
		glevelScript.strArgument = GetWin_Text( this, IDC_EDIT_CMD_MSG );
	}

	if ( !m_AutoLevel.CheckScript( glevelScript ) )
	{
		MessageBox( "Command Data Error", MB_OK );
		return;
	}

	m_AutoLevel.m_vecLevScript[nSelect] = glevelScript;

	int nPos = m_cAutoLevelCMDList.GetScrollPos( SB_VERT );

	AutoLevelCommandUpdateItems();

	m_cAutoLevelCMDList.SetItemState( nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 
	
	
	//	스크롤바 설정
	{
		CRect ref;
		m_cAutoLevelCMDList.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정

		int nSize = nPos * ref.Height();		

		CSize sz( 0, nSize );
		m_cAutoLevelCMDList.Scroll(sz);
	}
}


namespace ConvTemp
{
	AutoLevel::AutoLevel(const DWORD& _mapID)
		: mapID(_mapID)
	{
	}
	AutoLevel::~AutoLevel(void)
	{
	}

	AutoLevelConverter::AutoLevelConverter(void)
	{
	}
	AutoLevelConverter::~AutoLevelConverter(void)
	{
	}	
	
	const unsigned int AutoLevelConverter::addMap(const DWORD& _mapID, const std::string& _levelFileName)
	{
		if ( _levelFileName.size() < 4 )
			return -1;

		AutoLevelMapIter _iteratorAutoLevel(this->autoLevelMap.find(_mapID));
		if ( _iteratorAutoLevel == this->autoLevelMap.end() )
		{
			this->autoLevelMap.insert(AutoLevelPair(_mapID, AutoLevel(_mapID)));
			_iteratorAutoLevel = this->autoLevelMap.find(_mapID);
		}

		AutoLevel& _autoLevel = _iteratorAutoLevel->second;

		LevelFileNameMap& _levelFileMap = _autoLevel.mapFileNameMap;
		LevelFileNameVector& _levelFileVector = _autoLevel.mapFileNameVector;
		LevelFileNameMapCIter _iteratorLevelFile(_levelFileMap.find(_levelFileName));
		if ( _iteratorLevelFile == _levelFileMap.end() )
		{
			const unsigned int _nIndex(_levelFileMap.size());
			_levelFileMap.insert(LevelFileNamePair(_levelFileName, _nIndex));
			_levelFileVector.push_back(_levelFileName);
			return _nIndex;
		}

		return _iteratorLevelFile->second;
	}
}

void CAutoLevelPage::OnBnClickedButtonConvertXml()
{
	const int iLangFlag = (int)language::ProvidelangToLangflag(RANPARAM::emProvideLangFlag);

	std::string strPath(GLOGIC::GetPath());
	strPath.append("AutoLevelSet.xml");
	TIXMLOLD::TiXmlDocument XmlDoc;

	if ( XmlDoc.LoadFile(strPath.c_str(), TIXMLOLD::TIXML_ENCODING_UTF8) )
	{
		// Root Element 삭제;
		XmlDoc.RemoveChild(XmlDoc.FirstChild()->NextSiblingElement("TEXT"));
	}
	else
	{
		TIXMLOLD::TiXmlDeclaration*	XmlDecl = new TIXMLOLD::TiXmlDeclaration( "1.0", "utf-8", "");
		XmlDoc.LinkEndChild( XmlDecl );
	}
	TIXMLOLD::TiXmlElement* pRootElem = new TIXMLOLD::TiXmlElement("TEXT");
	XmlDoc.LinkEndChild(pRootElem);
	
	char	szConv[512] = {0};
	wchar_t	wsConv[512] = {0};

	const int iCnt = m_pAutoLevel->m_autoLevelVector.size();
	for ( int i = 0; i < iCnt; ++i )
	{
		const GLAutoLevel& autoLevel = m_pAutoLevel->m_autoLevelVector[i];

		TIXMLOLD::TiXmlElement* pLevelSetElem = new TIXMLOLD::TiXmlElement("WORD");
		pLevelSetElem->SetAttribute("Ver", 1);

		_tcscpy(szConv, autoLevel.m_strName.c_str());
		MultiByteToWideChar(CD3DFontPar::nCodePage[iLangFlag], 
			0, szConv, 
			strlen(autoLevel.m_strName.c_str())+1,
			wsConv,sizeof(wsConv)/sizeof(wsConv[0]));			
		WideCharToMultiByte(CP_UTF8, 0, wsConv, -1
			, szConv, sizeof(szConv)/sizeof(szConv[0])
			, NULL,NULL );

		pLevelSetElem->SetAttribute("Id", szConv);

		const INT iScriptCount = static_cast<INT>(autoLevel.m_vecLevScript.size());
		for ( INT j = 0; j < iScriptCount; ++j )
		{
			char	szConv_msg[1024] = {0};
			wchar_t	wsConv_msg[512] = {0};

			TIXMLOLD::TiXmlElement* pScriptElem = new TIXMLOLD::TiXmlElement("VALUE");
			pScriptElem->SetAttribute("Lang", RANPARAM::strCountry.GetString());
			pScriptElem->SetAttribute("Index", j);

			const GLLEV_SCRIPT& levScript = autoLevel.m_vecLevScript[j];
			if ( strlen(levScript.strArgument.c_str()) != 0 )
			{
				_tcscpy(szConv_msg, levScript.strArgument.c_str());
			}
			else
			{
				_tcscpy(szConv_msg, "skip");
			}
			MultiByteToWideChar(CD3DFontPar::nCodePage[iLangFlag],
				0,szConv_msg, 
				strlen(szConv_msg)
				,wsConv_msg,sizeof(wsConv_msg)/sizeof(wsConv_msg[0]));			
			WideCharToMultiByte(CP_UTF8, 0, wsConv_msg, -1, szConv_msg,
				sizeof(szConv_msg)/sizeof(szConv_msg[0]), NULL,NULL );

			TIXMLOLD::TiXmlText* pMSGText = new TIXMLOLD::TiXmlText(szConv_msg);		
			
			pScriptElem->LinkEndChild(pMSGText);
			pLevelSetElem->LinkEndChild(pScriptElem);
		}
		pRootElem->LinkEndChild(pLevelSetElem);
	}
	XmlDoc.SaveFile(strPath);
	/*
	const INT iCount = m_pAutoLevel->m_autoLevelVector.size();
	// List Xml 저장;
	{
		std::string strPath(GLOGIC::GetPath());
		strPath.append("AutoLevelList.xml");

		TIXMLOLD::TiXmlDocument docLIST;
		if ( docLIST.LoadFile(strPath.c_str(), TIXMLOLD::TiXmlEncoding::TIXML_ENCODING_UTF8) )
		{
			// Root Element 삭제;
			docLIST.RemoveChild(docLIST.FirstChild()->NextSiblingElement("TEXT"));
		}
		else
		{
			TiXmlDeclaration*	XmlDecl = new TiXmlDeclaration( "1.0", "utf-8", "");
			docLIST.LinkEndChild( XmlDecl );
		}
		TIXMLOLD::TiXmlElement* pRootElem = new TIXMLOLD::TiXmlElement("TEXT");
		docLIST.LinkEndChild(pRootElem);

		TIXMLOLD::TiXmlElement* pListElem = new TIXMLOLD::TiXmlElement("SENTENSE");
		pListElem->SetAttribute("Ver", 1);
		pListElem->SetAttribute("Id", "AUTOLEVELSET_LIST");

		pRootElem->LinkEndChild(pListElem);
		
		for ( INT i = 0; i < iCount; ++i )
		{
			TIXMLOLD::TiXmlElement* pLevelElem = new TIXMLOLD::TiXmlElement("VALUE");
			pLevelElem->SetAttribute("Index", i);

			CString strFileName(m_pAutoLevel->m_autoLevelVector[i].m_strName.c_str());
			strFileName += ".xml";
			TIXMLOLD::TiXmlText* pText = new TIXMLOLD::TiXmlText(strFileName.GetString());
			pLevelElem->LinkEndChild(pText);
			pListElem->LinkEndChild(pLevelElem);
		}
		docLIST.SaveFile(strPath);
		docLIST.Clear();
	}
	//////////////////////////////////////////////////////////////////////////

	// Level File 개별 데이터 저장;
	{
		for ( INT i = 0; i < iCount; ++i )
		{
			const GLAutoLevel& autoLevel = m_pAutoLevel->m_autoLevelVector[i];

			CString strLevelFile(GLOGIC::GetPath());
			strLevelFile.Format("%s%s%s", 
				strLevelFile,
				autoLevel.m_strName.c_str(),
				".xml");

			TIXMLOLD::TiXmlDocument docLevel;
			if ( docLevel.LoadFile(strLevelFile.GetString()) )
				docLevel.RemoveChild(docLevel.FirstChild()->NextSiblingElement("TEXT"));
			else
			{
				TiXmlDeclaration*	XmlDecl = new TiXmlDeclaration( "1.0", "utf-8", "");
				docLevel.LinkEndChild( XmlDecl );
			}

			TIXMLOLD::TiXmlElement* pRootElem = new TIXMLOLD::TiXmlElement("TEXT");
			docLevel.LinkEndChild(pRootElem);

			
			const INT iScriptCount = static_cast<INT>(autoLevel.m_vecLevScript.size());
			for ( INT j = 0; j < iScriptCount; ++j )
			{
				TIXMLOLD::TiXmlElement* pListElem = new TIXMLOLD::TiXmlElement("SENTENSE");
				pListElem->SetAttribute("Ver", 1);
				pListElem->SetAttribute("Id", "AUTOLEVEL_SCRIPT_SENTENSE");

				TIXMLOLD::TiXmlElement* pScriptElem = new TIXMLOLD::TiXmlElement("VALUE");
				pScriptElem->SetAttribute("Lang", RANPARAM::strCountry.GetString());
				pScriptElem->SetAttribute("Index", j);

				const GLLEV_SCRIPT& levScript = autoLevel.m_vecLevScript[j];
				TIXMLOLD::TiXmlText* pMSGText = new TIXMLOLD::TiXmlText(levScript.strArgument.c_str());
				
				pScriptElem->LinkEndChild(pMSGText);
				pListElem->LinkEndChild(pScriptElem);
				pRootElem->LinkEndChild(pListElem);
			}
			docLevel.SaveFile(strLevelFile.GetString());
		}	
	}
*/
	AfxMessageBox(_T("Complete!"), MB_OK | MB_ICONSTOP);
}