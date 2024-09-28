#include "stdafx.h"
#include "../CharEdit.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"
#include "AssetDlg.h"

IMPLEMENT_DYNAMIC(CAssetDlg, CDialog)

CAssetDlg::CAssetDlg(CWnd* pParent) :
    CDialog(CAssetDlg::IDD, pParent)
,   CSettingIni(_T("CharEdit.ini"), _T("AssetDlg"))
{

}

CAssetDlg::~CAssetDlg()
{
    
}

void CAssetDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_ASSET, m_kTreeCtrl);
    DDX_Control(pDX, IDC_BUTTON1, m_kBtnExport);
}

BEGIN_MESSAGE_MAP(CAssetDlg, CDialog)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON1, &CAssetDlg::OnBnClickedExport)    
END_MESSAGE_MAP()

BOOL CAssetDlg::OnInitDialog()
{
    CDialog::OnInitDialog();       

    InitSizer();

    LoadPosition(this);

    ShowWindow(SW_SHOW);

    return TRUE;
}

void CAssetDlg::InitSizer()
{
    m_kSizer.Init(this);
    m_kSizer.Insert(&m_kTreeCtrl);
    m_kSizer.Insert(&m_kBtnExport, CControlSizer::ST_REPOS, CControlSizer::ST_REPOS);
}

void CAssetDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    m_kSizer.OnSize(nType, cx, cy);
    m_kTreeCtrl.RedrawSelectedItem();
}

void CAssetDlg::OnBnClickedExport()
{
    TCHAR* strFilter = _T("Text Files(*.txt) | *.txt | |");
    CFileDialog kDlgFile(FALSE, _T("txt"), NULL, NULL, strFilter);

    if (kDlgFile.DoModal() == IDOK)
    {
        CString cTemp = kDlgFile.GetPathName();

        std::fstream fs;
        fs.open(cTemp.GetBuffer(), std::ios::out);
        RecursiveExport(m_kTreeCtrl.GetRootItem(), 0, fs);
        fs.close();
    }
}

void CAssetDlg::RecursiveExport(HTREEITEM hNode, int nDepth, std::fstream& fs)
{
    if (hNode != 0)
    {
        CString strText = m_kTreeCtrl.GetItemText(hNode);

        if (strText.IsEmpty() == false)
        {
            for (int i = 0; i < nDepth; ++i)
            {
                fs << _T("\t");
            }

            fs << strText.GetBuffer() << _T("\n");
        }
        
        RecursiveExport(m_kTreeCtrl.GetChildItem(hNode), nDepth + 1, fs);
        RecursiveExport(m_kTreeCtrl.GetNextSiblingItem(hNode), nDepth, fs);
    }
}

HTREEITEM CAssetDlg::Insert(HTREEITEM hParent, const TCHAR* pName, bool bExpand)
{
    HTREEITEM hItem = 0;

    if (pName && _tcsclen(pName) > 0)
    {
        hItem = m_kTreeCtrl.InsertItem(pName, hParent);        

        if (bExpand)
        {
            m_kTreeCtrl.Expand(hParent, TVE_EXPAND);
        }
    }

    return hItem;
}

void CAssetDlg::Clear()
{
    m_kTreeCtrl.DeleteAllItems();
}