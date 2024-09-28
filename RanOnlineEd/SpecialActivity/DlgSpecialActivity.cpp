#include "stdafx.h"
#include "./DlgSpecialActivity.h"
#include "./SheetWithTab.h"

#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../EngineLib/G-Logic/GLogic.h"


IMPLEMENT_DYNAMIC(DlgSpecialActivity, CDialog)

DlgSpecialActivity::DlgSpecialActivity(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSpecialActivity::IDD, pParent)
{

}

DlgSpecialActivity::~DlgSpecialActivity()
{
}

void DlgSpecialActivity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgSpecialActivity, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_NEW, &DlgSpecialActivity::OnBnClickedButtonNew)
    ON_BN_CLICKED(IDC_BUTTON_LOAD, &DlgSpecialActivity::OnBnClickedButtonLoad)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &DlgSpecialActivity::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_XML_EXPORT, &DlgSpecialActivity::OnBnClickedButtonXmlExport)
    ON_BN_CLICKED(IDC_BUTTON_XML_IMPORT, &DlgSpecialActivity::OnBnClickedButtonXmlImport)
    ON_BN_CLICKED(IDC_BUTTON_CSV_EXPORT, &DlgSpecialActivity::OnBnClickedButtonCsvExport)
END_MESSAGE_MAP()



BOOL DlgSpecialActivity::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect SheetRect;
	CWnd *pWnd;

	//	Note : Sheet 1 �� �����.
	//
	pWnd = GetDlgItem ( IDC_STATIC_CONTROL );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );


	LOGFONT logfont = {0};

	CFont* pFont = GetFont();
	if ( pFont ) pFont->GetLogFont( &logfont );

	m_pSheetTab = new CsheetWithTab ( SheetRect, logfont, this );
	m_pSheetTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_pSheetTab->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	
	m_pSheetTab->SetActivePage ( HIGH_PAGE );    

    return TRUE;
}

void DlgSpecialActivity::OnBnClickedButtonNew()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    ActivityBaseData::GetInstance()->CleanUp();
    m_pSheetTab->InitAllPage();
}

void DlgSpecialActivity::OnBnClickedButtonLoad()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CString szFilter = "Activity bin File (*.bin)|*.bin|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".bin",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(DlgSpecialActivity*)this);

	if ( dlg.DoModal() == IDOK )
    {
        std::string strPath = dlg.GetPathName();
		BOOL bOK = ActivityBaseData::GetInstance()->LoadBIN( strPath );
		
		if ( !bOK )
		{
			MessageBox ( "Failure to Load File" );
			return;
		}

        m_pSheetTab->InitAllPage();
	}
}

void DlgSpecialActivity::OnBnClickedButtonSave()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Activity bin File (*.bin)|*.bin|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".bin",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(DlgSpecialActivity*)this);

	if ( dlg.DoModal() == IDOK )
	{
        std::string strPath = dlg.GetPathName();

        BOOL bOK = ActivityBaseData::GetInstance()->CheckVerifier ();

        if ( !bOK )
        {
            MessageBox ( "Failure to Verify" );
            return;
        }

        bOK = ActivityBaseData::GetInstance()->SaveBIN ( strPath );

		if ( !bOK )
		{
			MessageBox ( "Failure to Save BIN File" );
			return;
		}

        std::string strCSVPath ( strPath.substr(0, strPath.find_last_of(".") ) + ".csv" );
        bOK = ActivityBaseData::GetInstance()->SaveCSV ( strCSVPath );

        if ( !bOK )
        {
            MessageBox ( "Failure to Save CSV File" );
            return;
        }

		return ;
	}
}

void DlgSpecialActivity::OnBnClickedButtonXmlExport()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Activity rcc File (*.xml)|*.xml|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".xml",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(DlgSpecialActivity*)this);

//    dlg.m_ofn.lpstrInitialDir = ActivityBaseData::GetInstance()->GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
        std::string strPath = dlg.GetPathName();

       BOOL  bOK = ActivityBaseData::GetInstance()->CheckVerifier ();

        if ( !bOK )
        {
            MessageBox ( "Failure to Verify" );
            return;
        }

		bOK = ActivityBaseData::GetInstance()->SaveXML ( strPath );

		if ( !bOK )
		{
			MessageBox ( "Failure to Export Xml File" );
			return;
		}

		return ;
	}
}

void DlgSpecialActivity::OnBnClickedButtonXmlImport()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CString szFilter = "Activity bin File (*.xml)|*.xml|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".xml",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(DlgSpecialActivity*)this);

	if ( dlg.DoModal() == IDOK )
    {
        std::string strPath = dlg.GetPathName();
		BOOL bOK = ActivityBaseData::GetInstance()->LoadXML( strPath );
		
		if ( !bOK )
		{
			MessageBox ( "Failure to Import Xml File" );
			return;
		}

        m_pSheetTab->InitAllPage();
	}
}

void DlgSpecialActivity::OnBnClickedButtonCsvExport()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CString szFilter = "Activity csv File (*.csv)|*.csv|";

    //	Note : ���� ���� ���̾˷α׸� ����.
    CFileDialog dlg(FALSE,".csv",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
        (DlgSpecialActivity*)this);

    if ( dlg.DoModal() == IDOK )
    {
        std::string strPath = dlg.GetPathName();
        BOOL bOK = ActivityBaseData::GetInstance()->SaveCSV ( strPath );

        if ( !bOK )
        {
            MessageBox ( "Failure to Export CSV File" );
            return;
        }

        return ;
    }
}
