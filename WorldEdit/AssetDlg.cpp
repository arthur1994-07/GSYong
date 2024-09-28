#include "stdafx.h"
#include "WorldEdit.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "MainFrm.h"
#include "AssetDlg.h"
#include "SheetWithTab.h"
#include "State/WorldStateMachine.h"
#include "../enginelib/DxTools/DxViewPort.h"
#include "../enginelib/GUInterface/Cursor.h"
#include "../SigmaCore/File/FileUtil.h"

IMPLEMENT_DYNAMIC(CAssetDlg, CDialog)

CAssetDlg::CAssetDlg(CWnd* pParent) :
    CDialog(CAssetDlg::IDD, pParent)
,   CSettingIni(_T("WorldEdit.ini"), _T("AssetDlg"))
,   m_bSelectTree(true)
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
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ASSET, &CAssetDlg::OnTvnSelchangedTreeAsset)
END_MESSAGE_MAP()

BOOL CAssetDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    InitSizer();

    LoadPosition(this);

    ShowWindow(SW_HIDE);

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

HTREEITEM CAssetDlg::Insert(HTREEITEM hParent, const TCHAR* pName, bool bExpand, DWORD lParam, bool bFrontInsert)
{
    HTREEITEM hItem = 0;

    if (pName && _tcsclen(pName) > 0)
    {
        HTREEITEM hFirstChild = 0;

        if (bFrontInsert && hParent)
        {
            hFirstChild = m_kTreeCtrl.GetChildItem(hParent);
        }

        hItem = m_kTreeCtrl.InsertItem(pName, hParent, hFirstChild);

        m_kTreeCtrl.SetItemData(hItem, lParam);

        if (bFrontInsert && hItem && hFirstChild)
        {
            SwapItem(hItem, hFirstChild);
        }

        if (bExpand)
        {
            m_kTreeCtrl.Expand(hParent, TVE_EXPAND);
        }
    }

    return hItem;
}

HTREEITEM CAssetDlg::GetChild(HTREEITEM hItem)
{
    return m_kTreeCtrl.GetChildItem(hItem);
}

HTREEITEM CAssetDlg::GetNextSibling(HTREEITEM hItem)
{
    return m_kTreeCtrl.GetNextSiblingItem(hItem);
}

int CAssetDlg::GetSiblingSize(HTREEITEM hItem)
{    
    int i = 0;

    if (hItem)
    {
        RecursiveSiblingSize(hItem, ++i);
    }

    return i;
}

DWORD CAssetDlg::GetSiblingIndexFromSelected()
{
    HTREEITEM hSelectedItem = m_kTreeCtrl.GetSelectedItem();

    if (hSelectedItem)
    {
        HTREEITEM hParent = m_kTreeCtrl.GetParentItem(hSelectedItem);
        HTREEITEM hSibling = m_kTreeCtrl.GetChildItem(hParent);
        DWORD i = 0;

        while (hSibling)
        {
            if (hSibling == hSelectedItem)
            {
                return i;
            }

            hSibling = m_kTreeCtrl.GetNextSiblingItem(hSibling);

            ++i;
        }
    }

    return -1;
}

bool CAssetDlg::IsItemText(HTREEITEM hItem, const TCHAR* pText)
{
    CString str = m_kTreeCtrl.GetItemText(hItem);

    if (str.CompareNoCase(pText) == 0)
    {
        return true;
    }

    return false;
}

bool CAssetDlg::IsSelectItemText(const TCHAR* pText)
{
    HTREEITEM hItem = m_kTreeCtrl.GetSelectedItem();

    if (hItem)
    {
        return IsItemText(hItem, pText);
    }

    return false;
}

void CAssetDlg::SetItemText(HTREEITEM hItem, const TCHAR* pText)
{
    m_kTreeCtrl.SetItemText(hItem, pText);
}

void CAssetDlg::SetItemData(HTREEITEM hItem, DWORD dwData)
{
    m_kTreeCtrl.SetItemData(hItem, dwData);
}

void CAssetDlg::SwapItem(HTREEITEM hNode, HTREEITEM hNode2)
{
    DWORD dwNode = m_kTreeCtrl.GetItemData(hNode);
    DWORD dwNode2 = m_kTreeCtrl.GetItemData(hNode2);
    CString strNode = m_kTreeCtrl.GetItemText(hNode);
    CString strNode2 = m_kTreeCtrl.GetItemText(hNode2);

    m_kTreeCtrl.SetItemData(hNode, dwNode2);
    m_kTreeCtrl.SetItemData(hNode2, dwNode);
    m_kTreeCtrl.SetItemText(hNode, strNode2);
    m_kTreeCtrl.SetItemText(hNode2, strNode);
}

HTREEITEM CAssetDlg::Find(const TCHAR* pName)
{
    HTREEITEM hItem = m_kTreeCtrl.GetRootItem();
    return RecursiveFind(hItem, pName);
}

HTREEITEM CAssetDlg::RecursiveFind(HTREEITEM hItem, const TCHAR* pName)
{
    if (hItem)
    {
        CString strItem = m_kTreeCtrl.GetItemText(hItem);

        if (strItem.CompareNoCase(pName) == 0)
        {
            return hItem;
        }
        else
        {
            HTREEITEM hResult = RecursiveFind(m_kTreeCtrl.GetChildItem(hItem), pName);

            if (hResult)
            {
                return hResult;
            }

            hResult = RecursiveFind(m_kTreeCtrl.GetNextSiblingItem(hItem), pName);

            if (hResult)
            {
                return hResult;
            }
        }
    }

    return 0;
}

void CAssetDlg::RecursiveSiblingSize(HTREEITEM hItem, int& nCount)
{
    if (hItem)
    {
        hItem = GetNextSibling(hItem);

        if (hItem)
        {
            RecursiveSiblingSize(hItem, ++nCount);
        }
    }
}

bool CAssetDlg::Delete(const TCHAR* pName)
{
    HTREEITEM hItem = Find(pName);

    if (hItem)
    {
        m_kTreeCtrl.DeleteItem(hItem);
        return true;
    }

    return false;
}

void CAssetDlg::Delete(HTREEITEM hItem)
{
    if (hItem)
    {
        m_kTreeCtrl.DeleteItem(hItem);
    }
}

void CAssetDlg::DeleteSub(HTREEITEM hItem)
{
    hItem = m_kTreeCtrl.GetChildItem(hItem);

    while (hItem)
    {
        HTREEITEM hTemp = hItem;
        hItem = m_kTreeCtrl.GetNextSiblingItem(hTemp);
        m_kTreeCtrl.DeleteItem(hTemp);
    }
}

void CAssetDlg::Clear()
{
    m_kTreeCtrl.DeleteAllItems();
}

void CAssetDlg::SelectFrame(DxFrame* pFrame)
{
    HTREEITEM hItem = m_kTreeCtrl.FindItemFromData(reinterpret_cast<DWORD>(pFrame));

    if (hItem)
    {
        m_bSelectTree = false;
        m_kTreeCtrl.SelectItem(hItem);
        m_bSelectTree = true;
    }
}

HTREEITEM CAssetDlg::GetTypeTreeItem(HTREEITEM hItem)
{
    HTREEITEM hOuter = m_kTreeCtrl.GetParentItem(hItem);

    if (hOuter)
    {
        HTREEITEM hInner = GetTypeTreeItem(hOuter);

        if (hInner)
        {
            hItem = hInner;
        }
        else
        {
            hItem = hOuter;
        }
    }

    return hItem;
}

void CAssetDlg::OnTvnSelchangedTreeAsset(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView    = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
   
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());
    CWorldEditDoc* pDoc			= static_cast<CWorldEditDoc*>(pFrame->GetActiveDocument());
    DxLandMan* pLandMan	        = pView->GetLandMan();
    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    DxFrameMesh* pFrameMesh		= pSetLandMan->GetFrameMesh();
    DxOctree* pOctree           = pLandMan->GetOctree();

    DWORD dwData                = m_kTreeCtrl.GetItemData(pNMTreeView->itemNew.hItem);
    CString strItem             = m_kTreeCtrl.GetItemText(pNMTreeView->itemNew.hItem);

    HTREEITEM hRootItem         = GetTypeTreeItem(pNMTreeView->itemNew.hItem);
    CString strRootItem         = m_kTreeCtrl.GetItemText(hRootItem);

    // 루트 아이템 처리
    for (int i = FRAMEPAGEINDEX; i < EDITSHEET_NUM; ++i)
    {
        if (strRootItem.CompareNoCase(g_strEditSheet[i].GetBuffer()) == 0)
        {
            pFrame->m_cDlgEdit.SetAcitvePage(i);
            break;
        }
    }

    // 서브 아이템 선택 처리
    WorldStateMachine::SelectAsset(dwData);

    *pResult = 0;
}