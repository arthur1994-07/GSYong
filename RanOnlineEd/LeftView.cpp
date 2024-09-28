// LeftView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Util/SystemInfo.h"
/*#include "../SigmaCore/File/Excel/MinExcel.h"*/
#include "../SigmaCore/File/TxtFile.h"
#include "../VersionManager/DatabaseTable.h"
#include "./StringEncrypt/DlgStringEncrypt.h"
#include "./SpecialActivity/DlgSpecialActivity.h"
#include "./ConnectionString/DlgConnString.h"
#include "./AssetToExcel/DlgAssetToExcel.h"
#include "RanOnlineEd.h"
#include "LeftView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// LeftView

IMPLEMENT_DYNCREATE(LeftView, CListView)

LeftView::LeftView()
{
    m_ToolName[TOOL_IDX_0] = "SpecialActivity";
    m_ToolName[TOOL_IDX_1] = "String Encrypt";
	m_ToolName[TOOL_IDX_2] = "ADO ConnString";
    m_ToolName[TOOL_IDX_3] = "File List to Excel";
    m_ToolName[TOOL_IDX_4] = "Asset List to Excel";
}

LeftView::~LeftView()
{
}

BEGIN_MESSAGE_MAP(LeftView, CListView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_MESSAGE( XTWM_OUTBAR_NOTIFY,  OnOutbarNotify )
    ON_NOTIFY(PGN_SCROLL, IDC_PAGER_CTRL, OnPagerScroll)
    ON_NOTIFY(PGN_CALCSIZE, IDC_PAGER_CTRL, OnPagerCalcSize)
END_MESSAGE_MAP()


// LeftView 진단입니다.

#ifdef _DEBUG
void LeftView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void LeftView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// LeftView 메시지 처리기입니다.

int LeftView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CListView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  여기에 특수화된 작성 코드를 추가합니다.
    // Create the pager control.
    if (!m_wndPager.Create(WS_CHILD|WS_VISIBLE|PGS_VERT, CRect(0,0,0,0), this, IDC_PAGER_CTRL))
    {
        TRACE0("Failed to create CPagerCtrl...\n");
        return -1;
    }

    // Create the OutlookBar control using m_wndPager as the
    // parent.
    if (!m_wndOutlookBar.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0,0,0,0), &m_wndPager, IDC_OUTBAR))
    {
        TRACE0("Failed to create COutlookBar...\n");
        return -1;
    }

    // Set the CWnd object you want messages sent to.
    m_wndOutlookBar.SetOwner(this);
    //m_wndOutlookBar.SetColors(RGB(0xff,0xff,0xff), RGB(0x3a,0x6e,0xa5));
    m_wndOutlookBar.SetColors(RGB(0xff,0xff,0xff), RGB(0xff,0xff,0xff));

    // Add items to the Outlook Bar control.
    m_wndOutlookBar.AddMenuItem(IDI_ICON_TOOL_TITLE, m_ToolName[TOOL_IDX_0].c_str());
    m_wndOutlookBar.AddMenuItem(IDI_ICON_TOOL_TITLE, m_ToolName[TOOL_IDX_1].c_str());
	m_wndOutlookBar.AddMenuItem(IDI_ICON_TOOL_TITLE, m_ToolName[TOOL_IDX_2].c_str());
    m_wndOutlookBar.AddMenuItem(IDI_ICON_TOOL_TITLE, m_ToolName[TOOL_IDX_3].c_str());
    m_wndOutlookBar.AddMenuItem(IDI_ICON_TOOL_TITLE, m_ToolName[TOOL_IDX_4].c_str());

//  m_wndOutlookBar.AddMenuItem(IDI_ICON_CONTACTS, _T("Quest"));
//  m_wndOutlookBar.AddMenuItem(IDI_ICON_TASKS,    _T("Map"));
//  m_wndOutlookBar.AddMenuItem(IDI_ICON_JOURNAL,  _T("Item"));
//  m_wndOutlookBar.AddMenuItem(IDI_ICON_NOTES,    _T("Monster"));
//  m_wndOutlookBar.AddMenuItem(IDI_ICON_DELETED,  _T("Npc Action"));
//  m_wndOutlookBar.AddMenuItem(IDI_ICON_PUBLIC, _T("Public"));
//  m_wndOutlookBar.AddMenuItem(IDI_ICON_DRAFTS, _T("Drafts"));
//  m_wndOutlookBar.AddMenuItem(IDI_ICON_OUTBOX, _T("Outbox"));
//  m_wndOutlookBar.AddMenuItem(IDI_ICON_SENT, _T("Sent"));

    // Insert menu items at a specific index.
//  m_wndOutlookBar.InsertMenuItem(1, IDI_ICON_INBOX,    _T("Inbox"));
//  m_wndOutlookBar.InsertMenuItem(2, IDI_ICON_CALENDAR, _T("Calendar"));

    // Set the child HWND to COutlookBar, and button size to 15.
    m_wndPager.SetChild(m_wndOutlookBar.GetSafeHwnd());
    m_wndPager.SetButtonSize(15);
    return 0;
}

void LeftView::OnSize(UINT nType, int cx, int cy)
{
    CListView::OnSize(nType, cx, cy);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    if (m_wndPager.GetSafeHwnd())
        m_wndPager.MoveWindow(0,0,cx,cy);
}

void LeftView::OnPagerCalcSize(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMPGCALCSIZE* pNMPGCalcSize = (NMPGCALCSIZE*)pNMHDR;
    *pResult = 0;
    switch(pNMPGCalcSize->dwFlag)
    {
    case PGF_CALCWIDTH:
        break;

    case PGF_CALCHEIGHT:
        pNMPGCalcSize->iHeight = m_wndOutlookBar.GetCount()
            *(::GetSystemMetrics(SM_CYICON)*2);
        break;
    }
}

void LeftView::OnPagerScroll(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMPGSCROLL* pNMPGScroll = (NMPGSCROLL*)pNMHDR;
    *pResult = 0;
}

LRESULT LeftView::OnOutbarNotify(UINT lParam, LONG wParam)
{
    switch( wParam ) // control id.
    {
    case IDC_OUTBAR:
        {
            // Get the menu item.
            CXTMenuListBox::CONTENT_ITEM* pContentItems = m_wndOutlookBar.GetMenuItem((int) lParam);
            ASSERT(pContentItems);
			if (!pContentItems)
				return 0;

            if (pContentItems->m_nIndex == TOOL_IDX_0)
            {
                DlgSpecialActivity Dlg;
                Dlg.DoModal();
            }
            else if (pContentItems->m_nIndex == TOOL_IDX_1)
            {
                DlgStringEncrypt Dlg;
                Dlg.DoModal();
            }
			else if (pContentItems->m_nIndex == TOOL_IDX_2)
            {
                DlgConnString Dlg;
                Dlg.DoModal();
            }
            else if (pContentItems->m_nIndex == TOOL_IDX_3)
            {
                MakeFileListToExcel();
            }
            else if (pContentItems->m_nIndex == TOOL_IDX_4)
            {
                DlgAssetToExcel Dlg;
                Dlg.DoModal();
            }            
            else
            {
                AfxMessageBox("Unknown Tool Name");
            }
        }
        break;
    }
    return 0;
}

void LeftView::MakeFileListToExcel()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    CString szFilter = "Ran Online file list (*.bin)|*.bin|";

    //	Note : 파일 오픈 다이알로그를 만듬.
    CFileDialog dlg(
        TRUE,
        ".bin",
        NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
        szFilter,
        (LeftView*) this);
    
    dlg.m_ofn.lpstrInitialDir = sc::getAppPath().c_str();

    if (dlg.DoModal() == IDOK)
    {
        std::string FilePath(dlg.GetPathName().GetString());
        CFile FileList(FilePath.c_str(), CFile::modeRead);

        RanFileList Version;
        FileList.Read(&Version, sizeof(RanFileList));

        RanFileList TotlaCount;
        FileList.Read(&TotlaCount, sizeof(RanFileList));

        //std::string ExcelFilePath;        
        //sc::Excel* pXls = new sc::Excel("filelist", ExcelFilePath, 1, true);

        std::string ResultFile = sc::getAppPath() + "//filelist.csv";
        sc::TxtFile* pListFile = new sc::TxtFile(ResultFile, true);
        for (size_t i=0; i<TotlaCount.nVersion; ++i)
        {
            RanFileList FileInfo;
            FileList.Read(&FileInfo, sizeof(RanFileList));

            std::string TxtWrite(
                sc::string::format(
                    "%1%, %2%, %3%",
                    FileInfo.szSubPath,
                    FileInfo.szFileName,
                    FileInfo.nVersion));
            pListFile->write(TxtWrite);
        }
        delete pListFile;
        
        MessageBox(ResultFile.c_str(), "저장되었습니다.", MB_OK);
    }    
}