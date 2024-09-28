#include "stdafx.h"

#pragma warning(disable:4311) // boost warning disable
#pragma warning(disable:4312) // boost warning disable

#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/Log/LogMan.h"

#include "../RanLogic/NpcTalk/NpcDialogueSet.h"
#include "../RanLogic/NpcTalk/NpcTalkMan.h"
#include "../RanLogic/Crow/GLCrowData.h"
#include "../RanLogic/GLogicData.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLUseFeatures.h"

#include "../RanLogicServer/DxServerInstance.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/Common/SUBPATH.h"

#include "./NpcAction.h"
#include "./Edit/SheetWithTab.h"
#include "./SimpleEdit.h"
#include "./DlgLog.h"
#include "./DlgExcelExport.h"
#include "./DlgExcelImport.h"
#include "./DlgNtkCheck.h"
#include "./NpcActionDlg.h"

#pragma warning(disable:4996)

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CNpcActionDlg 대화 상자



CNpcActionDlg::CNpcActionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNpcActionDlg::IDD, pParent)
    , m_pLogDlg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNpcActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNpcActionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENUITEM_NEW, OnMenuitemNew)
	ON_COMMAND(ID_MENUITEM_LOAD, OnMenuitemLoad)
	ON_COMMAND(ID_MENUITEM_SAVE, OnMenuitemSave)
	ON_COMMAND(ID_FILE_ALLTEXTEXPORT, OnMenuAllTextExport )
	ON_COMMAND(ID_FILE_ALLTEXTEXPORT2, OnMenuAllTextExport2 )
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CSV_SAVE_BUTTON, OnBnClickedCsvSaveButton)
	ON_BN_CLICKED(IDC_TEXTEXPORT_BUTTON, OnBnClickedTextexportButton)
    ON_BN_CLICKED(IDC_TEXTEXPORT_BUTTON_ALL, OnBnClickedTextexportButtonAll)
	ON_BN_CLICKED(IDC_TEXTIMPORT_BUTTON, OnBnClickedTextimportButton)
	ON_BN_CLICKED(IDC_CSVEXPORT_BUTTON, OnBnClickedCsvexportButton)
	ON_BN_CLICKED(IDC_CSVIMPORT_BUTTON, OnBnClickedCsvimportButton)
	ON_BN_CLICKED(IDC_ALLSAVE_BUTTON, OnBnClickedAllSaveButton)
	ON_BN_CLICKED(IDC_CSVEXPORT_BUTTON2, OnBnClickedCsvexportButton2)
	ON_BN_CLICKED(IDC_CSVIMPORT_BUTTON2, OnBnClickedCsvimportButton2)
    ON_BN_CLICKED(IDC_BUTTON_LOG_WINDOW, &CNpcActionDlg::OnBnClickedButtonLogWindow)
    ON_BN_CLICKED(IDC_BUTTON_CHECK_DATA, &CNpcActionDlg::OnBnClickedButtonCheckData)
    ON_BN_CLICKED(IDC_BUTTON_EXCEL_EXPORT, &CNpcActionDlg::OnBnClickedButtonExcelExport)
    ON_BN_CLICKED(IDC_BUTTON_EXCEL_IMPORT, &CNpcActionDlg::OnBnClickedButtonExcelImport)
    ON_BN_CLICKED(IDC_BUTTON_MERGE_DATA, &CNpcActionDlg::OnBnClickedButtonDataMerge)
    ON_BN_CLICKED(IDC_BUTTON_LOAD_TEST, &CNpcActionDlg::OnBnClickedButtonLoadTest)
    ON_BN_CLICKED(IDC_BUTTON_SAVE_TEST, &CNpcActionDlg::OnBnClickedButtonSaveTest)
	ON_BN_CLICKED(IDC_BUTTON_XMLCONVERT, &CNpcActionDlg::OnBnClickedButtonXmlConvert)
	ON_BN_CLICKED(IDC_BTN_EXCEL_STRTBL_IMPORT, OnBnClickedBtnExcelStrTblImport)
	ON_BN_CLICKED(IDC_BTN_EXCEL_STRTBL_EXPORT, OnBnClickedBtnExcelStrTblExport)
END_MESSAGE_MAP()


// CNpcActionDlg 메시지 처리기

BOOL CNpcActionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// Note : 실행파일의 경로를 찾아서 저장한다. - 반드시 CBasicWnd(CD3dApp) 이전에 수행해야 한다.
	//
	CString strAppPath;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAppPath = szPath;

	if ( !strAppPath.IsEmpty() )
	{
		DWORD dwFind = strAppPath.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			m_strAppPath = strAppPath.Left ( dwFind );
			
			if ( !m_strAppPath.IsEmpty() )
			if ( m_strAppPath.GetAt(0) == '"' )
				m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength() - 1 );

			strcpy ( m_szAppPath, m_strAppPath.GetString() );
		}
	}  	

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	MakeToolBar ();

	CRect SheetRect;
	CWnd* pWnd = NULL;

	//	Note : Sheet 1 를 만든다.
	//

	SetReadMode();

	pWnd = GetDlgItem ( IDC_STATIC_CONTROL );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	LOGFONT logfont = {0};

	CFont* pFont = GetFont();
	if (pFont)
        pFont->GetLogFont(&logfont);

	m_pSheetTab = new CsheetWithTab ( SheetRect, logfont, this );
	m_pSheetTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_pSheetTab->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	
	m_pSheetTab->SetActivePage ( NPCACTIONTABLE );

    m_pLogDlg = new DlgLog;
    m_pLogDlg->Create(IDD_DIALOG_LOG);
    m_pLogDlg->ShowWindow(SW_SHOW);
    sc::LogMan::getInstance()->attachEditControl(m_pLogDlg->GetEditWindow(), m_pLogDlg->GetEditWindow());

    // GLOGIC init
    CNpcActionApp* pApp = (CNpcActionApp*) AfxGetApp();
    GMTOOL::Create(m_strAppPath.GetString(), pApp->GetSubPath(), pApp->GetServiceProvider());
	// List update
	std::tr1::shared_ptr<NpcTalkMan> pTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
	if (pTalkMan)
	{
		std::vector<std::string> TalkList;
		pTalkMan->GetDlgList(TalkList);            
		m_pSheetTab->m_NpcActionTable.UpdateTalkFileList(TalkList);
	}
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CNpcActionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CNpcActionDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CNpcActionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNpcActionDlg::MakeToolBar ()
{
	if( !m_wndToolBar.Create(this) || 
        !m_wndToolBar.LoadToolBar(IDR_TOOLBAR1) )
    {
        TRACE("Failed to Create Toolbar\n");
        EndDialog(IDCANCEL);
    }

    CRect rcClientStart;
    CRect rcClientNow;
    GetClientRect(&rcClientStart);

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 
                   0, reposQuery, rcClientNow);

    CPoint ptOffset(rcClientNow.left - rcClientStart.left,
                    rcClientNow.top  - rcClientStart.top);

    CRect rcChild;
    CWnd* pwndChild = GetWindow(GW_CHILD);
    while( pwndChild )
    {
        pwndChild->GetWindowRect(rcChild);
        ScreenToClient(rcChild);
        rcChild.OffsetRect(ptOffset);
        pwndChild->MoveWindow(rcChild, FALSE);
        pwndChild = pwndChild->GetNextWindow();
    }

    CRect rcWindow;
    GetWindowRect(rcWindow);
    rcWindow.right += rcClientStart.Width() - rcClientNow.Width();
    rcWindow.bottom += rcClientStart.Height() - rcClientNow.Height();

    MoveWindow(rcWindow, FALSE);

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, 
                   AFX_IDW_CONTROLBAR_LAST, 0);
}

void CNpcActionDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SAFE_DELETE ( m_pSheetTab );
	GMTOOL::CleanUp();

	GLogicData::GetInstance().ClearData();

	CDialog::PostNcDestroy();
}

void CNpcActionDlg::OnMenuitemNew()
{
	std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
	spTalkMan->Clear();
	m_pSheetTab->m_NpcActionTable.DialogClear();
    /*
    spNpcDialogSet pDlgSet = m_pSheetTab->GetDialogueSet();
    if (!pDlgSet)
        return;

	m_pSheetTab->GetDialogueSet()->DoModified();
	SetWindowText( "NpcAction" );

	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_pSheetTab->GetDialogueSet()->RemoveAllDialogue ();
	m_pSheetTab->m_NpcActionTable.UpdateItems();
    */
}

void CNpcActionDlg::OnMenuitemLoad()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "NPC Conversation File (*.dat)|*.dat|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(
        TRUE,
        ".dat",
        NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
        szFilter,
		(CNpcActionDlg*) this);
	dlg.m_ofn.lpstrInitialDir = CNpcDialogueSet::GetPath();

	if (dlg.DoModal() == IDOK)
	{
        std::tr1::shared_ptr<NpcTalkMan> pTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
        if (pTalkMan)
        {            
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CGameTextMan::GetInstance().LoadText( RANPARAM::strNPCTalkText, CGameTextMan::EM_NPCTALK_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
			}

            if (pTalkMan->Load(dlg.GetPathName().GetString()))
                SetWindowText(dlg.GetPathName().GetString());
            else
                MessageBox("Failure to Load File.", "Failed", MB_OK|MB_ICONEXCLAMATION);
            spNpcDialogSet pDlgSet = m_pSheetTab->GetDialogueSet();
            if (pDlgSet)
                pDlgSet->DoModified();
            m_pSheetTab->m_NpcActionTable.UpdateItems();

            // List update
            std::vector<std::string> TalkList;
            pTalkMan->GetDlgList(TalkList);            
            if (pDlgSet)
                pDlgSet->RemoveAllDialogue();
            m_pSheetTab->m_NpcActionTable.UpdateTalkFileList(TalkList);
            MessageBox("읽기가 완료되었습니다.", MB_OK);
        }	
	}
}

bool CNpcActionDlg::DoSave()
{
    /*
	CString szFilter = "NPC Conversation File (*.ntk)|*.ntk|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".ntk",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CNpcActionDlg*)this);

	dlg.m_ofn.lpstrInitialDir = CNpcDialogueSet::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		BOOL bResult = m_pSheetTab->GetDialogueSet()->Save ( dlg.GetFileName().GetString() );
		if( bResult )
		{
			SetWindowText( dlg.GetFileName().GetString() );
		}
		else
		{
			MessageBox ( "Failure to Save File." );
			return false;
		}

		return true;
	}
	return false;
    */
    OnBnClickedButtonSaveTest();
    return true;
}

void CNpcActionDlg::OnMenuitemSave()
{
	DoSave ();
	
//	// npctalk test
//	AllSave();
}

void CNpcActionDlg::EnableMenu ( UINT uIDEnableItem, BOOL bEnable )
{
	CMenu* mmenu = GetMenu();
	CMenu* submenu = mmenu->GetSubMenu(0);
	
	if ( bEnable )
	{
		submenu->EnableMenuItem ( uIDEnableItem, MF_BYCOMMAND | MF_ENABLED );
	}
	else
	{
		submenu->EnableMenuItem ( uIDEnableItem, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	}
}

void CNpcActionDlg::EnableToobar ( UINT uIDEnableItem, BOOL bEnable )
{
	m_wndToolBar.GetToolBarCtrl().EnableButton ( uIDEnableItem, bEnable );
}

void CNpcActionDlg::OnClose()
{
    spNpcDialogSet spDlgSet = m_pSheetTab->GetDialogueSet();
	if (spDlgSet && spDlgSet->IsModify())
	{
		int nret = MessageBox ( "Press the 'Yes' button to save.", "Caution", MB_YESNOCANCEL );
		switch ( nret )
		{
		case IDYES:
			if (!DoSave())
                return;	//	실패시에는 종료하지 않음.
			break;

		case IDNO:
			//	저장하지 않고 종료.
			break;

		case IDCANCEL:
			//	취소시에는 종료하지 않음.
			return;
		};
	}

	if (m_pSheetTab && m_pSheetTab->GetActiveIndex() != NPCACTIONTABLE) 
	{
		MessageBox ( "Close the active window before quit.", "Caution", MB_OK );
		return;
	}

    sc::LogMan::getInstance()->releaseInstance();
    m_pLogDlg->ShowWindow(SW_HIDE);
    delete m_pLogDlg;
    m_pLogDlg = NULL;

	CDialog::OnClose();
}
void CNpcActionDlg::OnBnClickedCsvSaveButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//BOOL bResult = m_pSheetTab->GetDialogueSet()->SimpleEdit( (CWnd*)this );
	CSimpleEdit dlg;
	dlg.SetNpcDialogueSet(m_pSheetTab->GetDialogueSet());
	dlg.DoModal();

	m_pSheetTab->GetDialogueSet()->DoModified();
	m_pSheetTab->m_NpcActionTable.UpdateItems ();
}

void CNpcActionDlg::OnBnClickedTextexportButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	char szTempChar[1024];
	ZeroMemory( szTempChar, 1024 );
	GetWindowText( szTempChar, 1024 );
	_splitpath( szTempChar, NULL, NULL, szTempChar, NULL );
	strcat( szTempChar, ".ntt" );	

    spNpcDialogSet spDialogueSet = m_pSheetTab->GetDialogueSet();

    std::string strError = spDialogueSet->ExportText( szTempChar );
    if( !strError.empty() )
        MessageBox( strError.c_str() );
}

void CNpcActionDlg::OnBnClickedTextexportButtonAll()
{
    std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
    if( spTalkMan )
        spTalkMan->ExportTextAll();
}

void CNpcActionDlg::OnBnClickedTextimportButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char szSectionChar[512];
	char szTempChar[512];
	ZeroMemory( szTempChar, 512 );
	ZeroMemory( szSectionChar, 512 );
	GetWindowText( szTempChar, 512 );

	_splitpath( szTempChar, NULL, NULL, NULL, szSectionChar );
	if( strlen( szSectionChar ) == 0 )
	{
		MessageBox( "NTK 파일을 로드하세요." );
		return;
	}

	_splitpath( szTempChar, NULL, NULL, szTempChar, NULL );
	strcat( szTempChar, ".ntt" );

    spNpcDialogSet spDialogueSet = m_pSheetTab->GetDialogueSet();

    std::string strError = spDialogueSet->ImportText( szTempChar );
    if( !strError.empty() )
        MessageBox( strError.c_str() );

    spDialogueSet->DoModified();

    m_pSheetTab->m_NpcActionTable.UpdateItems ();
}


void CNpcActionDlg::OnBnClickedCsvexportButton()
{
    /*
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char szTempChar[256];
	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	_splitpath( szTempChar, NULL, NULL, NULL, szTempChar );
	if( strlen( szTempChar ) == 0 )
	{
		MessageBox( "NTK 파일을 로드하세요." );
		return;
	}

	GetWindowText( szTempChar, 256 );
	_splitpath( szTempChar, NULL, NULL, szTempChar, NULL );
	strcat( szTempChar, ".csv" );
	FILE *pFile = _fsopen( szTempChar, "wt", _SH_DENYNO );
	if( pFile == NULL )
	{
		MessageBox( "파일 오픈 에러. CSV 파일이 열려있으면 닫아주세요" );
		return;
	}

	spNpcDialogSet spDialogueSet = m_pSheetTab->GetDialogueSet();
	// 전체 NPC 텍스트
	CNpcDialogueSet::DIALOGMAP* pDialogueMap = spDialogueSet->GetDialogueMap();
	CNpcDialogueSet::DIALOGMAP_ITER found;

	
	// 상단의 메뉴 이름
	char szMenuChar[]={"NPCMainID,NPCSubID,TotalAnswerNumber,ConditionLevel,ConditionLevelsize,ConditionClass,ConditionElemental,ConditionElementalSize,ConditionLivingScroe,ConditionLivingScoreSize,ConditionQuestMID,ConditionQuestSID,CondtionDonotQuest,ConditionRequestitemMID,ConditionRequestItemSID,ConditionCompletionQuest,ConditionSchool\n"};
	char szAnswerChar[]={"AnswerID,AnswerMainType,AnswerSubType,Buslistfilename,ExchangeFromItemMID,ExchangeFromItemSID,ExchangeToItemMID,ExchangeToItemSID,StartQuestID1,StartQuestID2,StartQuestID3,StartQuestID4,StartQuestID5,ProgressingQuestMID,ProgressingQuestSID,"};
	char szAnswerConChar[]={"ConditionLevel,ConditionLevelsize,ConditionClass,ConditionElemental,ConditionElementalSize,ConditionLivingScroe,ConditionLivingScoreSize,ConditionQuestMID,ConditionQuestSID,CondtionDonotQuest,ConditionRequestitemMID,ConditionRequestItemSID,ConditionCompletionQuest,ConditionSchool\n"};

	int i = 0, j = 0;

	// 전체 리스트 데이터
	for( found = pDialogueMap->begin(); found != pDialogueMap->end(); ++found )
	{
		CNpcDialogue* pNpcDialogue = (*found).second;
		for( i = 0; i < static_cast<int>( pNpcDialogue->GetDlgCaseVec().size() ); i++ )
		{
			// 텍스트 버퍼 초기화
			std::string strLine;
			ZeroMemory( szTempChar, 256 );
			//strLine.clear();

			// 메뉴 이름 저장
			fprintf( pFile, szMenuChar );

			// NPCMainID
			sprintf( szTempChar, "%d", pNpcDialogue->GetDlgNID() );
			strLine += szTempChar; strLine += ",";
			// NPCSubID 
			sprintf( szTempChar, "%d", i );
			strLine += szTempChar; strLine += ",";		
			// TotalAnswerNumber 
			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl() != NULL )
			{
				sprintf( szTempChar, "%d", pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() );
				strLine += szTempChar; strLine += ",";
			}else{
				strLine += "0"; strLine += ",";
			}

			// Save Condition
			if( pNpcDialogue->GetDlgCase( i )->GetCondition() != NULL )
			{
				pNpcDialogue->GetDlgCase( i )->GetCondition()->SAVECSVFILE( strLine );
			}
			strLine += "\n";
			fprintf( pFile, strLine.c_str() );

			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl() == NULL ) continue;
			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() == 0 ) continue;

			// 대답 메뉴 저장
			fprintf( pFile, szAnswerChar );
			fprintf( pFile, szAnswerConChar );
			CNpcTalkControl::NPCMAP_IT iter     = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->begin();
			CNpcTalkControl::NPCMAP_IT iter_end = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->end();	
			for ( ; iter!=iter_end; ++iter )
			{
				ZeroMemory( szTempChar, 128 );
				strLine.clear();

				SNpcTalk* pNpcTalk = (*iter).second;

				// AnswerID
				sprintf( szTempChar, "%d", pNpcTalk->m_dwNID );
				strLine += szTempChar; strLine += ",";
				// AnswerMainType
				sprintf( szTempChar, "%d", pNpcTalk->m_nACTION );
				strLine += szTempChar; strLine += ",";
				// AnswerSubType
				sprintf( szTempChar, "%d", pNpcTalk->m_dwACTION_NO );
				strLine += szTempChar; strLine += ",";
				// Buslistfilename

				if( pNpcTalk->m_strBusFile.size() == 0 )
				{
					strLine += "NULL"; strLine += ",";
				}else{
					strLine += pNpcTalk->m_strBusFile.c_str(); strLine += ",";
				}
				// ExchangeFromItemMID, ExchangeFromItemSID, ExchangeToItemMID, ExchangeToItemSID
				// Type: 기본기능
				// Data: 아이템 교환
				// Need Add New Item Con.
				if( pNpcTalk->m_nACTION == 2 && pNpcTalk->m_dwACTION_NO == 5 )
				{
					SNATIVEID nID;
					nID.dwID = pNpcTalk->m_dwACTION_PARAM1;
					sprintf( szTempChar, "%d", nID.wMainID );
					strLine += szTempChar; strLine += ",";
					sprintf( szTempChar, "%d", nID.wSubID );
					strLine += szTempChar; strLine += ",";

					nID.dwID = pNpcTalk->m_dwACTION_PARAM2;
					sprintf( szTempChar, "%d", nID.wMainID );
					strLine += szTempChar; strLine += ",";
					sprintf( szTempChar, "%d", nID.wSubID );
					strLine += szTempChar; strLine += ",";
				}else{
					sprintf( szTempChar, "NULL,NULL,NULL,NULL" );
					strLine += szTempChar; strLine += ",";
				}
				// StartQuestID 
				// Type: 퀘스트 시작
				// Data: 상관없음
				if( pNpcTalk->m_nACTION == 3 )
				{
					for ( int index = 0; index < MAX_QUEST_START; ++index )
					{
						if( pNpcTalk->m_dwQuestStartID[index] == UINT_MAX )
						{
							strLine += "NULL"; strLine += ",";
						}else{
							sprintf( szTempChar, "%d", pNpcTalk->m_dwQuestStartID[index] );
							strLine += szTempChar; strLine += ",";
						}
					}
				}else{
					sprintf( szTempChar, "NULL,NULL,NULL,NULL,NULL" );
					strLine += szTempChar; strLine += ",";
				}
				// ProgressingQuestMID, ProgressingQuestSID
				if( pNpcTalk->m_nACTION == 4 )
				{
					sprintf( szTempChar, "%d", pNpcTalk->m_dwACTION_PARAM1 );
					strLine += szTempChar; strLine += ",";
					sprintf( szTempChar, "%d", pNpcTalk->m_dwACTION_PARAM2 );
					strLine += szTempChar; strLine += ",";
				}else{
					sprintf( szTempChar, "NULL,NULL" );
					strLine += szTempChar; strLine += ",";
				}
				// Save Answer Condition
				if( pNpcTalk->GetCondition() != NULL )
					pNpcTalk->GetCondition()->SAVECSVFILE( strLine );
				strLine += "\n";

				fprintf( pFile, strLine.c_str() );
			}
		}
//		strLine += szTempChar;		

	}

	fclose( pFile );
    */
	MessageBox("더이상 지원되지 않습니다. Excel 을 이용하세요.");

}

void CNpcActionDlg::OnBnClickedCsvimportButton()
{
    /*
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char szTempChar[256];


	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	_splitpath( szTempChar, NULL, NULL, NULL, szTempChar );

	if( strlen( szTempChar ) == 0 )
	{
		MessageBox( "NTK 파일을 로드하세요." );
		return;
	}
	GetWindowText( szTempChar, 256 );
	_splitpath( szTempChar, NULL, NULL, szTempChar, NULL );
	strcat( szTempChar, ".csv" );
	sc::CStringFile StrFile( 10240 );
	if( !StrFile.Open ( szTempChar ) )
	{
		MessageBox( "익스포트된 CSV 데이터가 없습니다." );
		return;
	}

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;
	CStringArray beforeStrArray;
	std::string	strSection;

	bool	bAddAnswer = FALSE;

	int iNpcNumber = -1;
	int iTextNumber = -1;
	int iAnswerNumber = -1;
	int iLineNumber = 0;

	// 로드한 데이터 셋팅
	spNpcDialogSet spDialogueSet = m_pSheetTab->GetDialogueSet();
	
	while( StrFile.GetNextLine( strLine ) )
	{
		iLineNumber++;

		STRUTIL::StringSeparate( strLine, StrArray );

		strSection = StrArray[0];
		
		if( strSection == "NPCMainID" )
		{
			bAddAnswer = FALSE;
			continue;
		}else if( strSection == "AnswerID" )
		{
			bAddAnswer = TRUE;
			iAnswerNumber = -1;
			beforeStrArray.Copy( StrArray );
			continue;
		}

	
		if( bAddAnswer == FALSE )
		{
			// 대답이 아닌 기본 데이터일 경우
			iNpcNumber  = atoi(StrArray[0]);
			iTextNumber = atoi(StrArray[1]);

			CNpcDialogue* pNpcDialogue = spDialogueSet->GetDialogue(iNpcNumber);
			
			// Condition에 대한 정보를 셋팅한다.
			if( pNpcDialogue->GetDlgCase(iTextNumber)->GetCondition() != NULL )
			{
				if( pNpcDialogue->GetDlgCase(iTextNumber)->GetCondition()->LOADCSVFILE( StrArray, 3, iLineNumber ) == FALSE )
					goto load_error;
			}
			if( pNpcDialogue->GetDlgCase(iTextNumber)->GetTalkControl() != NULL )
			{
				if( pNpcDialogue->GetDlgCase(iTextNumber)->GetTalkControl()->GetTalkMap()->size() != atoi(StrArray[2]) )
				{
					sprintf( szTempChar, "CSV의 대답 갯수와 데이터의 대답 갯수가 다릅니다.\n %d번째 줄", iLineNumber );
					MessageBox( szTempChar, "오류" );
					goto load_error;
				}
			}
		}else if( bAddAnswer == TRUE )
		{
			
			char *szAnswerChar[] = { {"AnswerID"}, {"AnswerMainType"}, {"AnswerSubType"}, {"Buslistfilename"}, 
									{"ExchangeFromItemMID"}, {"ExchangeFromItemSID"}, {"ExchangeToItemMID"}, 
									{"ExchangeToItemSID"}, {"StartQuestID"}, {"ProgressingQuestMID"}, {"ProgressingQuestSID" }};
			// 대답일 경우 
			int iCsvCur = 0;
			iAnswerNumber++;
			CNpcDialogue* pNpcDialogue = spDialogueSet->GetDialogue(iNpcNumber);
			if( pNpcDialogue->GetDlgCase(iTextNumber)->GetTalkControl() == NULL ) continue;
			SNpcTalk* pNpcTalk = pNpcDialogue->GetDlgCase( iTextNumber )->GetTalkControl()->GetTalk2(iAnswerNumber);
			// AnswerID
			pNpcTalk->m_dwNID       = atoi(StrArray[iCsvCur++]);
			// AnswerMainType
			pNpcTalk->m_nACTION		= atoi(StrArray[iCsvCur++]);
			// AnswerSubType
			pNpcTalk->m_dwACTION_NO = atoi(StrArray[iCsvCur++]);
			// Buslistfilename
			if( StrArray[iCsvCur++] != "NULL" )
				pNpcTalk->m_strBusFile	= StrArray[iCsvCur];
			else
				pNpcTalk->m_strBusFile.clear();

			std::string strTemp;
			// ExchangeFromItemMID, ExchangeFromItemSID, ExchangeToItemMID, ExchangeToItemSID
			if( pNpcTalk->m_nACTION == SNpcTalk::EM_BASIC && pNpcTalk->SNpcTalk::EM_ITEM_TRADE == 5 )
			{
				
				SNATIVEID nID = SNATIVEID(false);

				// ExchangeFromItemMID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{					 
				}else{
					for( int i = 0; i < static_cast<int>( strTemp.size() ); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf( szTempChar, "ExchangeFromItemMID가 문자가 들어가 있습니다.\n %d번째 줄", iLineNumber );
							MessageBox( szTempChar, "오류" );
							goto load_error;
						}
					}
					nID.wMainID = atoi(strTemp.c_str());
				}

				// ExchangeFromItemSID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{
				}else{
					for( int i = 0; i < static_cast<int>( strTemp.size() ); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf( szTempChar, "ExchangeFromItemSID가 문자가 들어가 있습니다.\n %d번째 줄", iLineNumber );
							MessageBox( szTempChar, "오류" );
							goto load_error;
						}
					}
					nID.wSubID = atoi(strTemp.c_str());
				}
				pNpcTalk->m_dwACTION_PARAM1 = nID.dwID;

				nID = SNATIVEID(false);

				// ExchangeToItemMID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{					
				}else{
					for( int i = 0; i < static_cast<int>( strTemp.size() ); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf( szTempChar, "ExchangeToItemMID가 문자가 들어가 있습니다.\n %d번째 줄", iLineNumber );
							MessageBox( szTempChar, "오류" );
							goto load_error;
						}
					}
					nID.wMainID = atoi(strTemp.c_str());
				}

				// ExchangeToItemSID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{					
				}else{
					for( int i = 0; i < static_cast<int>( strTemp.size() ); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf( szTempChar, "ExchangeToItemSID가 문자가 들어가 있습니다.\n %d번째 줄", iLineNumber );
							MessageBox( szTempChar, "오류" );
							goto load_error;
						}
					}
					nID.wSubID = atoi(strTemp.c_str());
				}
				pNpcTalk->m_dwACTION_PARAM2 = nID.dwID;
			}else{
				iCsvCur += 4;
			}
			// StartQuestID
			if( pNpcTalk->m_nACTION == SNpcTalk::EM_QUEST_START )
			{
				std::string strTemp2 = beforeStrArray[8];
				if( beforeStrArray[8] == "StartQuestID1" || beforeStrArray[8] == " StartQuestID1" )
				{
					for ( int index = 0; index < MAX_QUEST_START; ++index )
					{
						strTemp = StrArray[iCsvCur++];
						if( strTemp == "NULL" || strTemp == " NULL" )
						{
							pNpcTalk->m_dwQuestStartID[index] = UINT_MAX;
							continue;
						}else{	
							for( int i = 0; i < static_cast<int>( strTemp.size() ); i++ )
							{
								if( !isdigit( strTemp[i] ) )
								{
									sprintf( szTempChar, "StartQuestID가 문자가 들어가 있습니다.\n %d번째 줄", iLineNumber );
									MessageBox( szTempChar, "오류" );
									goto load_error;
								}
							}
							pNpcTalk->m_dwQuestStartID[index] = atoi(strTemp.c_str());
						}
					}
				}else if( beforeStrArray[8] == "StartQuestID" || beforeStrArray[8] == " StartQuestID" )
				{
					strTemp = StrArray[iCsvCur++];
					if( strTemp == "NULL" || strTemp == " NULL" )
					{
					}else{	
						for( int i = 0; i < static_cast<int>( strTemp.size() ); i++ )
						{
							if( !isdigit( strTemp[i] ) )
							{
								sprintf( szTempChar, "StartQuestID가 문자가 들어가 있습니다.\n %d번째 줄", iLineNumber );
								MessageBox( szTempChar, "오류" );
								goto load_error;
							}
						}
						pNpcTalk->m_dwQuestStartID[0] = atoi(strTemp.c_str());
					}

				}
			}else{
				iCsvCur += MAX_QUEST_START;
			}
			// ProgressingQuestMID ProgressingQuestSID
			if( pNpcTalk->m_nACTION == SNpcTalk::EM_QUEST_STEP )
			{
				// ProgressingQuestMID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{
					pNpcTalk->m_dwACTION_PARAM1 = UINT_MAX;
				}else{
					for( int i = 0; i < static_cast<int>( strTemp.size() ); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf( szTempChar, "ProgressingQuestMID가 문자가 들어가 있습니다.\n %d번째 줄", iLineNumber );
							MessageBox( szTempChar, "오류" );
							goto load_error;
						}
					}
					pNpcTalk->m_dwACTION_PARAM1 = atoi(strTemp.c_str());
				}
				// ProgressingQuestSID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{
					pNpcTalk->m_dwACTION_PARAM2 = UINT_MAX;
				}else{
					for( int i = 0; i < static_cast<int>( strTemp.size() ); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf( szTempChar, "ProgressingQuestSID가 문자가 들어가 있습니다.\n %d번째 줄", iLineNumber );
							MessageBox( szTempChar, "오류" );
							goto load_error;
						}
					}
					pNpcTalk->m_dwACTION_PARAM2 = atoi(strTemp.c_str());
				}

			}else{
				iCsvCur += 2;
			}
			if( pNpcTalk->GetCondition() != NULL )
			{
				if( pNpcTalk->GetCondition()->LOADCSVFILE( StrArray, iCsvCur, iLineNumber ) == FALSE )
					goto load_error;
			}

			pNpcDialogue->GetDlgCase( iTextNumber )->GetTalkControl()->AddTalk(pNpcTalk);
            
		}

	
	}
	MessageBox( "Import가 성공했습니다." );
	return;
load_error:
	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	m_pSheetTab->GetDialogueSet()->Load ( szTempChar );
	MessageBox( "Import가 실패했습니다." );
    */
    MessageBox("더이상 지원되지 않습니다. Excel 을 이용하세요.");
}

void CNpcActionDlg::OnBnClickedButtonExcelExport()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    DlgExcelExport Dlg;
    Dlg.DoModal();
}

void CNpcActionDlg::OnBnClickedButtonExcelImport()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CString szFilter = "Npc Talk Excel File (*.xls)|*.xls|";

    //	Note : 파일 오픈 다이알로그를 만듬.
    CFileDialog dlg(
        TRUE,
        ".xls",
        NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
        szFilter,
        (CNpcActionDlg*) this);
    dlg.m_ofn.lpstrInitialDir = CNpcDialogueSet::GetPath();

    if (dlg.DoModal() == IDOK)
    {
        DlgExcelImport DlgImport(dlg.GetPathName());
        DlgImport.DoModal();
        if (DlgImport.m_LoadSuccess)
        {            
            spNpcDialogSet pDlgSet = m_pSheetTab->GetDialogueSet();
            if (pDlgSet)
                pDlgSet->DoModified();
            m_pSheetTab->m_NpcActionTable.UpdateItems();

            // List update
            std::tr1::shared_ptr<NpcTalkMan> pTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
            std::vector<std::string> TalkList;
            pTalkMan->GetDlgList(TalkList);
            if (pDlgSet)
                pDlgSet->RemoveAllDialogue();
            m_pSheetTab->m_NpcActionTable.UpdateTalkFileList(TalkList);
        }
    }
}

void CNpcActionDlg::GetNtkList(std::vector<std::string>& vFileList)
{
    std::string FilePath(CNpcDialogueSet::GetPath());
    FilePath.append("\\*.ntk");

    CFileFind finder;
    BOOL bWorking = finder.FindFile(FilePath.c_str());
    while (bWorking)
    {
        bWorking = finder.FindNextFile();
        CString FileName = finder.GetFileName();
        CString Path = finder.GetFilePath();
        std::string NtkFileName(FileName.GetString());
        vFileList.push_back(NtkFileName);
    }
}

void CNpcActionDlg::OnBnClickedCsvexportButton2()
{
    /*
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char szTempChar[256];
	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	_splitpath( szTempChar, NULL, NULL, NULL, szTempChar );
	if( strlen( szTempChar ) == 0 )
	{
		MessageBox( "NTK 파일을 로드하세요." );
		return;
	}

	GetWindowText( szTempChar, 256 );
	_splitpath( szTempChar, NULL, NULL, szTempChar, NULL );
	strcat( szTempChar, "_@" );
	strcat( szTempChar, ".csv" );
	FILE *pFile = _fsopen( szTempChar, "wt", _SH_DENYNO );
	if( pFile == NULL )
	{
		MessageBox( "파일 오픈 에러. CSV 파일이 열려있으면 닫아주세요" );
		return;
	}

	spNpcDialogSet spDialogueSet = m_pSheetTab->GetDialogueSet();
	// 전체 NPC 텍스트
	CNpcDialogueSet::DIALOGMAP* pDialogueMap = spDialogueSet->GetDialogueMap();
	CNpcDialogueSet::DIALOGMAP_ITER found;


	// 상단의 메뉴 이름
	char szMenuChar[] = { "NPCMainID, NPCSubID, ConditionClass\n" };
	char szAnswerChar[] = { "AnswerID, ConditionClass\n" };

	int i = 0, j = 0;

	// 전체 리스트 데이터
	for( found = pDialogueMap->begin(); found != pDialogueMap->end(); ++found )
	{
		CNpcDialogue* pNpcDialogue = (*found).second;
		for( i = 0; i < static_cast<int>( pNpcDialogue->GetDlgCaseVec().size() ); i++ )
		{
			// 텍스트 버퍼 초기화
			std::string strLine;
			ZeroMemory( szTempChar, 256 );
			//strLine.clear();

			// 메뉴 이름 저장
			fprintf( pFile, szMenuChar );

			// NPCMainID
			sprintf( szTempChar, "%d", pNpcDialogue->GetDlgNID() );
			strLine += szTempChar; strLine += ",";
			// NPCSubID 
			sprintf( szTempChar, "%d", i );
			strLine += szTempChar; strLine += ",";		

			// ConditionClass
			if( pNpcDialogue->GetDlgCase( i )->GetCondition() != NULL )
			{
				sprintf( szTempChar, "%d", pNpcDialogue->GetDlgCase( i )->GetCondition()->m_dwClass );
				strLine += szTempChar; strLine += ",";
			}
			strLine += "\n";
			fprintf( pFile, strLine.c_str() );

			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl() == NULL ) continue;
			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() == 0 ) continue;

			// 대답 메뉴 저장
			fprintf( pFile, szAnswerChar );
			CNpcTalkControl::NPCMAP_IT iter     = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->begin();
			CNpcTalkControl::NPCMAP_IT iter_end = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->end();	
			for ( ; iter!=iter_end; ++iter )
			{
				ZeroMemory( szTempChar, 128 );
				strLine.clear();

				SNpcTalk* pNpcTalk = (*iter).second;

				// AnswerID
				sprintf( szTempChar, "%d", pNpcTalk->m_dwNID );
				strLine += szTempChar; strLine += ",";

				// ConditionClass
				if( pNpcTalk->GetCondition() != NULL )
				{
					sprintf( szTempChar, "%d", pNpcTalk->GetCondition()->m_dwClass );
					strLine += szTempChar; strLine += ",";
				}
				strLine += "\n";

				fprintf( pFile, strLine.c_str() );
			}
		}	
	}

	fclose( pFile );

	MessageBox( "Export가 성공했습니다." );
    */
    MessageBox("더이상 지원되지 않습니다. Excel 을 이용하세요.");
}

void CNpcActionDlg::OnBnClickedCsvimportButton2()
{
    /*
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char szTempChar[256];


	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	_splitpath( szTempChar, NULL, NULL, NULL, szTempChar );

	if( strlen( szTempChar ) == 0 )
	{
		MessageBox( "NTK 파일을 로드하세요." );
		return;
	}
	GetWindowText( szTempChar, 256 );
	_splitpath( szTempChar, NULL, NULL, szTempChar, NULL );
	strcat( szTempChar, "_@" );
	strcat( szTempChar, ".csv" );
	sc::CStringFile StrFile( 10240 );
	if( !StrFile.Open ( szTempChar ) )
	{
		MessageBox( "익스포트된 CSV 데이터가 없습니다." );
		return;
	}

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;
	std::string	strSection;

	bool	bAddAnswer = FALSE;

	int iNpcNumber = -1;
	int iTextNumber = -1;
	int iAnswerNumber = -1;
	int iLineNumber = 0;

	// 로드한 데이터 셋팅
	spNpcDialogSet spDialogueSet = m_pSheetTab->GetDialogueSet();

	while( StrFile.GetNextLine( strLine ) )
	{
		iLineNumber++;


		STRUTIL::StringSeparate( strLine, StrArray );

		strSection = StrArray[0];

		if( strSection == "NPCMainID" )
		{
			bAddAnswer = FALSE;
			continue;
		}else if( strSection == "AnswerID" )
		{
			bAddAnswer = TRUE;
			iAnswerNumber = -1;
			continue;
		}


		if( bAddAnswer == FALSE )
		{
			// 대답이 아닌 기본 데이터일 경우
			iNpcNumber  = atoi(StrArray[0]);
			iTextNumber = atoi(StrArray[1]);

			CNpcDialogue* pNpcDialogue = spDialogueSet->GetDialogue(iNpcNumber);

			// Condition에 대한 정보를 셋팅한다.
			if( pNpcDialogue->GetDlgCase(iTextNumber)->GetCondition() != NULL )
			{
				pNpcDialogue->GetDlgCase(iTextNumber)->GetCondition()->m_dwClass = atoi(StrArray[2]);					
			}else{
//				goto load_error;
			}
		}else if( bAddAnswer == TRUE )
		{
			// 대답일 경우 
			iAnswerNumber++;
			CNpcDialogue* pNpcDialogue = spDialogueSet->GetDialogue(iNpcNumber);
			if( pNpcDialogue->GetDlgCase(iTextNumber)->GetTalkControl() == NULL ) continue;
			SNpcTalk* pNpcTalk = pNpcDialogue->GetDlgCase( iTextNumber )->GetTalkControl()->GetTalk2(iAnswerNumber);
			if( pNpcTalk->GetCondition() != NULL )
			{
				pNpcTalk->GetCondition()->m_dwClass = atoi(StrArray[1]);
			}else{
//				goto load_error;
			}

			pNpcDialogue->GetDlgCase( iTextNumber )->GetTalkControl()->AddTalk(pNpcTalk);

		}


	}
	MessageBox( "Import가 성공했습니다." );
	return;
load_error:
	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	m_pSheetTab->GetDialogueSet()->Load ( szTempChar );
	MessageBox( "Import가 실패했습니다." );
    */
    MessageBox("더이상 지원되지 않습니다. Excel 을 이용하세요.");
}


void CNpcActionDlg::OnBnClickedAllSaveButton()
{
    /*
	CString strPath = CNpcDialogueSet::GetPath ();
	CString strFind = strPath + "*.ntk";

	WIN32_FIND_DATA pInfo;
	lstrcpy( pInfo.cFileName, strFind.GetString() );

	HANDLE hContext = FindFirstFile( strFind.GetString(), &pInfo );
	if( hContext == INVALID_HANDLE_VALUE )
	{
		FindClose( hContext );
		return;
	} 

//	std::fstream streamFILE;
//	streamFILE.open( pInfo.cFileName, std::ios_base::out );
	CString strFile;

	do
	{
		if( m_pSheetTab->GetDialogueSet()->Load( pInfo.cFileName ) )
		{
			m_pSheetTab->GetDialogueSet()->DoModified();
			m_pSheetTab->m_NpcActionTable.UpdateItems   ();

			strFile = strPath + pInfo.cFileName;			
			if ( !m_pSheetTab->GetDialogueSet()->Save(strFile) )	MessageBox ( "error");
		}
	}
	while( FindNextFile( hContext, &pInfo ) != 0 );

	FindClose( hContext );

	MessageBox( "Ntk File All Save" );
    */
    MessageBox("더이상 지원되지 않습니다. Excel 을 이용하세요.");
}

void CNpcActionDlg::OnMenuAllTextExport()
{
	char szTempChar[1024];
	ZeroMemory( szTempChar, 1024 );

	CString strPath = CNpcDialogueSet::GetPath ();
	CString strFind = strPath + "*.ntk";

	WIN32_FIND_DATA pInfo;
	lstrcpy( pInfo.cFileName, strFind.GetString() );

	HANDLE hContext = FindFirstFile( strFind.GetString(), &pInfo );
	if( hContext == INVALID_HANDLE_VALUE )
	{
		FindClose( hContext );
		return;
	} 

	CString strFile;
	CString strTextFile = strPath + "NpcTalk.txt";

	FILE *pFile = _fsopen( strTextFile, "wt", _SH_DENYNO );
	if ( !pFile ) return;

	do
	{
		if( m_pSheetTab->GetDialogueSet()->Load( pInfo.cFileName ) )
		{
			m_pSheetTab->GetDialogueSet()->DoModified();
			m_pSheetTab->m_NpcActionTable.UpdateItems();

			spNpcDialogSet spDialogueSet = m_pSheetTab->GetDialogueSet();
			// 전체 NPC 텍스트
			CNpcDialogueSet::DIALOGMAP* pDialogueMap = spDialogueSet->GetDialogueMap();
			CNpcDialogueSet::DIALOGMAP_ITER found;

			UINT i;
			// 전체 리스트
			for( found = pDialogueMap->begin(); found != pDialogueMap->end(); ++found )
			{
                std::tr1::shared_ptr<CNpcDialogue> spNpcDialogue = found->second;
				
				fprintf( pFile, "%s\n", pInfo.cFileName );
				fprintf( pFile, "<START>\n" );
				sprintf( szTempChar, "<ID> %d\n\n", spNpcDialogue->GetDlgNID() );
				fprintf( pFile, szTempChar );

				// 텍스트 리스트
				for( i = 0; i < spNpcDialogue->GetDlgCaseVec().size(); i++ )
				{
					sprintf( szTempChar, "<TEXT> %d\n", i + 1 );
					fprintf( pFile, szTempChar );
					sprintf( szTempChar, "%s\r\n", spNpcDialogue->GetDlgCase( i )->GetBasicTalk().c_str() );
					fprintf( pFile, szTempChar );
					fprintf( pFile, "</TEXT>\n\n" );

					// 대답 리스트
					if( spNpcDialogue->GetDlgCase( i )->GetTalkControl() == NULL ) continue;
					if( spNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() == 0 ) continue;

					fprintf( pFile, "<ANSWER>\n" );
					CNpcTalkControl::NPCMAP_IT iter     = spNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->begin();
					CNpcTalkControl::NPCMAP_IT iter_end = spNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->end();	
					for ( ; iter!=iter_end; ++iter )
					{
						std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;
						sprintf( szTempChar, "%s\n", spNpcTalk->GetTalk() );
						fprintf( pFile, szTempChar );
					}
					fprintf( pFile, "</ANSWER>\n\n" );
				}

				fprintf( pFile, "<END>\n\n\n" );
			}
		}
	}
	while( FindNextFile( hContext, &pInfo ) != 0 );

	FindClose( hContext );

	fclose( pFile );
}


void CNpcActionDlg::OnMenuAllTextExport2()
{
	CString strPath = CNpcDialogueSet::GetPath ();
	CString strFind = strPath + "*.ntk";

	WIN32_FIND_DATA pInfo;
	lstrcpy( pInfo.cFileName, strFind.GetString() );

	HANDLE hContext = FindFirstFile( strFind.GetString(), &pInfo );
	if( hContext == INVALID_HANDLE_VALUE )
	{
		FindClose( hContext );
		return;
	} 

	CString strFile;

	do
	{
		if( m_pSheetTab->GetDialogueSet()->Load( pInfo.cFileName ) )
		{
			m_pSheetTab->GetDialogueSet()->DoModified();
			m_pSheetTab->m_NpcActionTable.UpdateItems();

			strFile = strPath + pInfo.cFileName;
			strFile = strFile.Left( strFile.ReverseFind('.'));
			strFile += ".txt";

			if ( !TextExport(strFile) ) MessageBox( strFile );

		}
	}
	while( FindNextFile( hContext, &pInfo ) != 0 );

	FindClose( hContext );
}

bool CNpcActionDlg::TextExport( CString strFileName )
{
	char szTempChar[1024];
	ZeroMemory( szTempChar, 1024 );

	FILE *pFile = _fsopen( strFileName, "wt", _SH_DENYNO );
	if ( !pFile ) return false;

	spNpcDialogSet spDialogueSet = m_pSheetTab->GetDialogueSet();
	// 전체 NPC 텍스트
	CNpcDialogueSet::DIALOGMAP* pDialogueMap = spDialogueSet->GetDialogueMap();
	CNpcDialogueSet::DIALOGMAP_ITER found;

	UINT i;
	// 전체 리스트
	for( found = pDialogueMap->begin(); found != pDialogueMap->end(); ++found )
	{
        std::tr1::shared_ptr<CNpcDialogue> spNpcDialogue = found->second;
		
		fprintf( pFile, "<START>\n" );
		sprintf( szTempChar, "<ID> %d\n\n", spNpcDialogue->GetDlgNID() );
		fprintf( pFile, szTempChar );

		// 텍스트 리스트
		for( i = 0; i < spNpcDialogue->GetDlgCaseVec().size(); i++ )
		{
			sprintf( szTempChar, "<TEXT> %d\n", i + 1 );
			fprintf( pFile, szTempChar );
			sprintf( szTempChar, "%s\r\n", spNpcDialogue->GetDlgCase( i )->GetBasicTalk().c_str() );
			fprintf( pFile, szTempChar );
			fprintf( pFile, "</TEXT>\n\n" );

			// 대답 리스트
			if( spNpcDialogue->GetDlgCase( i )->GetTalkControl() == NULL ) continue;
			if( spNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() == 0 ) continue;

			fprintf( pFile, "<ANSWER>\n" );
			CNpcTalkControl::NPCMAP_IT iter     = spNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->begin();
			CNpcTalkControl::NPCMAP_IT iter_end = spNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->end();	
			for ( ; iter!=iter_end; ++iter )
			{
				std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;
				sprintf( szTempChar, "%s\n", spNpcTalk->GetTalk() );
				fprintf( pFile, szTempChar );
			}
			fprintf( pFile, "</ANSWER>\n\n" );
		}

		fprintf( pFile, "<END>\n\n\n" );
	}

	fclose( pFile );

	return true;
}



void CNpcActionDlg::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	SetWin_Enable( this, IDC_CSVIMPORT_BUTTON, false );
	SetWin_Enable( this, IDC_CSVIMPORT_BUTTON2, false );
	SetWin_Enable( this, IDC_ALLSAVE_BUTTON, false );

	EnableMenu( ID_MENUITEM_NEW, FALSE );
	EnableToobar( ID_MENUITEM_NEW, FALSE );

#endif
}

void CNpcActionDlg::OnBnClickedButtonLogWindow()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_pLogDlg)
        m_pLogDlg->ShowWindow(SW_SHOW);
}

void CNpcActionDlg::OnBnClickedButtonCheckData()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_pLogDlg)
        m_pLogDlg->ShowWindow(SW_SHOW);
    DlgNtkCheck DlgCheck;
    DlgCheck.DoModal();
}

void CNpcActionDlg::OnBnClickedButtonLoadTest()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
    if (spTalkMan)
    {
        std::string EmptyStr;
        spTalkMan->Load(EmptyStr);
        std::vector<std::string> TalkList;
        spTalkMan->GetDlgList(TalkList);
        spNpcDialogSet spDlgSet = m_pSheetTab->GetDialogueSet();
        if (spDlgSet)
            spDlgSet->RemoveAllDialogue();
        m_pSheetTab->m_NpcActionTable.UpdateTalkFileList(TalkList);
        MessageBox("읽기가 완료되었습니다.", MB_OK);
    }
}

void CNpcActionDlg::OnBnClickedButtonDataMerge()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    std::vector<std::string> vFileList;
    GetNtkList(vFileList);
    std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
    if (spTalkMan)
    {
        spTalkMan->MergeNtkFiles(vFileList);
        OnBnClickedButtonSaveTest();
    }
}

void CNpcActionDlg::OnBnClickedButtonSaveTest()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
    if (spTalkMan)
    {
        std::string SaveFileName;
        if (spTalkMan->Save(SaveFileName))
        {
            std::string Msg(
                sc::string::format(
                    "%1% 이 저장되었습니다.",
                    SaveFileName));
            MessageBox(Msg.c_str(), "Success", MB_OK);
        }
        else
        {
            std::string Msg("저장에 실패하였습니다. 이미 열려있지 않은지 확인하십시오.");
            MessageBox(Msg.c_str(), "Failed", MB_OK|MB_ICONEXCLAMATION);
        }
    }
}

void CNpcActionDlg::OnBnClickedButtonXmlConvert()
{
	std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();

	std::string FullPath(sc::getAppPath());
	FullPath.append(GLogicData::GetInstance().GetPath()->NtkFileRoot());
	FullPath.append("NpcTalk.xml");

	if ( spTalkMan->ConvertXml(FullPath) )
	{
		std::string Msg(
			sc::string::format(
			"%1% 에 저장되었습니다.",
			FullPath));
		MessageBox(Msg.c_str(), "Success", MB_OK);
	}
	else
	{
		std::string Msg("저장에 실패하였습니다.");
		MessageBox(Msg.c_str(), "Failed", MB_OK|MB_ICONEXCLAMATION);
	}
}

void CNpcActionDlg::OnBnClickedBtnExcelStrTblImport()
{
	CFileDialog dlg( TRUE, "*.*", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"All Files(*.*)|*.*|", (CWnd*)this );

	dlg.m_ofn.lpstrInitialDir = GLogicData::GetInstance().GetPath()->AppRoot().c_str();
	if ( dlg.DoModal() != IDOK )
		return ;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
		if ( spTalkMan->LoadStringTableExcel(dlg.GetPathName().GetString()) == S_OK )
		{
			std::vector<std::string> TalkList;
			spTalkMan->GetDlgList(TalkList);                
			m_pSheetTab->m_NpcActionTable.UpdateTalkFileList(TalkList);
			MessageBox( _T("String Table Excel Load Success"), _T("Success"), MB_OK );
		}
	}
}

void CNpcActionDlg::OnBnClickedBtnExcelStrTblExport()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
		std::string strSavePath;
		if ( spTalkMan->SaveStringTableExcel(strSavePath) == S_OK )
		{
			std::string OkMsg(
				sc::string::format("\"%1%\" Save Success!", strSavePath.c_str() ) );
			MessageBox( OkMsg.c_str(), _T("Success"), MB_OK );
		}
		else
		{
			MessageBox( _T("Save Fail"), _T("Fail"), MB_OK );
		}
	}
}
