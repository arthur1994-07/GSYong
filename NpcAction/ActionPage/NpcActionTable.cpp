// NpcActionTable.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <boost/lexical_cast.hpp>
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../RanLogic/NpcTalk/NpcTalkMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../EngineLib/Common/SUBPATH.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../MfcExLib/ExLibs/FolderDlg.h"

#include "../Edit/SheetWithTab.h"
#include "../DlgTalkFileAdd.h"
#include "../NpcAction.h"
#include "./NpcActionTable.h"

// CNpcActionTable 대화 상자입니다.

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CNpcActionTable, CPropertyPage)
CNpcActionTable::CNpcActionTable( LOGFONT logfont )
	: CPropertyPage(CNpcActionTable::IDD)
	, m_pFont(NULL)
    , m_SelectedTalkFileIndex(-1)
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CNpcActionTable::~CNpcActionTable()
{
	SAFE_DELETE ( m_pFont );
}

void CNpcActionTable::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_ACTION, m_ctrlActionTable);
    DDX_Control(pDX, IDC_LIST_TALK_FILE, m_TalkFileList);
}


BEGIN_MESSAGE_MAP(CNpcActionTable, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_COPY, OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_CSVIMPORT_BUTTON, OnBnClickedCsvimportButton)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ACTION, OnNMDblclkListAction)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TALK_FILE, &CNpcActionTable::OnLvnItemchangedListTalkFile)
    ON_BN_CLICKED(IDC_BUTTON_ADD_TALK_FILE, &CNpcActionTable::OnBnClickedButtonAddTalkFile)
    ON_BN_CLICKED(IDC_BUTTON_DELETE_TALK_FILE, &CNpcActionTable::OnBnClickedButtonDeleteTalkFile)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TALK_FILE, OnNMRClickNTKList)
END_MESSAGE_MAP()


// CNpcActionTable 메시지 처리기입니다.


BOOL CNpcActionTable::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

    // ------------------------------------------------------------------------
	m_ctrlActionTable.SetExtendedStyle(m_ctrlActionTable.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
    
    RECT	rect;
	m_ctrlActionTable.GetClientRect(&rect);
	LONG Width = rect.right - rect.left;

	LV_COLUMN	lvcolumn;
	const int	ColumnCount = 5;
	char*		ColumnName[ColumnCount] = { "ID", "Basic Conversation", "Case", "Answer", "Total" };
	int			ColumnWidthPercent[ColumnCount] = { 6, 70, 7, 7, 7 };

	for (int i=0; i<ColumnCount; ++i)
	{		
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;// | LVCF_IMAGE;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = ColumnName[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = ( Width * ColumnWidthPercent[i] ) / 100;
		m_ctrlActionTable.InsertColumn(i, &lvcolumn);
	}

    // ------------------------------------------------------------------------
    m_TalkFileList.SetExtendedStyle(m_TalkFileList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
    
    LV_COLUMN Filelvcolumn;
    Filelvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;// | LVCF_IMAGE;
    Filelvcolumn.fmt = LVCFMT_LEFT;
    Filelvcolumn.pszText = "Name";
    Filelvcolumn.iSubItem = 1;
    Filelvcolumn.cx = 150;
    m_TalkFileList.InsertColumn(0, &Filelvcolumn);

    // ------------------------------------------------------------------------
	SetReadMode();
	UpdateItems();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CNpcActionTable::OnBnClickedButtonInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    std::tr1::shared_ptr<CNpcDialogue> sp((CNpcDialogue*) 0);
	m_pSheetTab->ActiveActionPage(sp);
}

spNpcDialogSet CNpcActionTable::GetDialogueSet()
{
	return m_spDialogueSet;
}

void CNpcActionTable::UpdateItems()
{
	m_ctrlActionTable.DeleteAllItems();

	CString strTemp;
    int AnswerTotal = 0;

    if (!m_spDialogueSet)
        return;
	
    CNpcDialogueSet::DIALOGMAP* pMap = m_spDialogueSet->GetDialogueMap();
	CNpcDialogueSet::DIALOGMAP_ITER iter = pMap->begin();
	CNpcDialogueSet::DIALOGMAP_ITER iter_end = pMap->end();
	for (int nIndex = 0; iter!=iter_end; ++iter, nIndex++)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue = iter->second;

		DWORD dwIndex = spDialogue->FindNonCondition();
		if (dwIndex==UINT_MAX)
            dwIndex = 0;

		std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = spDialogue->GetDlgCase(dwIndex);
		if (!spDlgCase)
		{			
			pMap->erase(iter++);
			--nIndex;
			continue;			
		}

		strTemp.Format("%d", spDialogue->GetDlgNID());
		int nItem = m_ctrlActionTable.InsertItem(nIndex, strTemp);

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("%s_%d_%d", m_spDialogueSet->m_TalkName.c_str(), spDialogue->GetDlgNID(), 0);
			const char* szTalkText = CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT).GetString();

			m_ctrlActionTable.SetItemText(nIndex, 1, szTalkText);
		}
		else
		{
			m_ctrlActionTable.SetItemText(nIndex, 1, spDlgCase->GetBasicTalk().c_str());
		}

        const CNpcDialogue::DLGCASE& Case = spDialogue->GetDlgCaseVec();
        std::string CaseSize = boost::lexical_cast<std::string> ((unsigned int) Case.size());
        m_ctrlActionTable.SetItemText(nIndex, 2, CaseSize.c_str());

        int AnswerCount = 0;			
        for (size_t i=0; i<Case.size(); ++i)
        {
            std::tr1::shared_ptr<CNpcDialogueCase> spCase = Case[i];
            if (spCase)
                AnswerCount += spCase->GetTalkNum();
        }

        std::string strAnswerCount = boost::lexical_cast<std::string> (AnswerCount);
        m_ctrlActionTable.SetItemText(nIndex, 3, strAnswerCount.c_str());

        AnswerTotal += AnswerCount;
        std::string strAnswerTotalCount = boost::lexical_cast<std::string> (AnswerTotal);
        m_ctrlActionTable.SetItemText(nIndex, 4, strAnswerTotalCount.c_str());

        m_ctrlActionTable.SetItemData(nItem, (DWORD_PTR) spDialogue->GetDlgNID());
	}
}

void CNpcActionTable::OnBnClickedButtonModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION	pos = m_ctrlActionTable.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlActionTable.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return ;
	}

	DWORD NativeID = (DWORD) m_ctrlActionTable.GetItemData ( nItem );	
	m_pSheetTab->ActiveActionPage(m_spDialogueSet->GetDialogue(NativeID));
}

void CNpcActionTable::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION	pos = m_ctrlActionTable.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlActionTable.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return ;
	}

	DWORD NativeID = (DWORD) m_ctrlActionTable.GetItemData ( nItem );

    std::tr1::shared_ptr<CNpcDialogue> spDialogue = m_spDialogueSet->GetDialogue(NativeID);
	
	DWORD dwIndex = spDialogue->FindNonCondition();
	if (dwIndex == UINT_MAX)
        dwIndex = 0;

	CString strBasicTalk;
	std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = spDialogue->GetDlgCase(dwIndex);
	if (spDlgCase)
        strBasicTalk = spDlgCase->GetBasicTalk().c_str();

	CString strTemp;
	strTemp.Format ( "Do you want to delete?\n[%03d] %s", NativeID, strBasicTalk );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		m_spDialogueSet->DelDialogue(NativeID);
		UpdateItems ();
	}
}

void CNpcActionTable::OnBnClickedButtonCopy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION	pos = m_ctrlActionTable.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlActionTable.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )	return;

	DWORD dwNativeID = (DWORD) m_ctrlActionTable.GetItemData ( nItem );
    std::tr1::shared_ptr<CNpcDialogue> spDialogue = m_spDialogueSet->GetDialogue(dwNativeID);
	if (!spDialogue)
        return;

	DWORD dwNID = m_spDialogueSet->MakeDlgNID ();

    std::tr1::shared_ptr<CNpcDialogue> spNewDialogue(new CNpcDialogue);
	*spNewDialogue.get() = *spDialogue.get();
	spNewDialogue->SetDlgNID(dwNID);
	m_spDialogueSet->AddDialogue(spNewDialogue);
	UpdateItems ();
}

void CNpcActionTable::OnBnClickedCsvimportButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CNpcActionTable::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	const int nSkipNum = 3;
	const int nSkipID[nSkipNum] = 
    { 
        IDC_LIST_ACTION, 
        IDC_BUTTON_MODIFY,
        IDC_LIST_TALK_FILE
    };

	int nID = 0;
	bool bOK = false;

	CWnd* pChildWnd = GetWindow(GW_CHILD);

	while (pChildWnd)
	{
		bOK = false;
		nID = pChildWnd->GetDlgCtrlID();	

		for ( int i = 0; i < nSkipNum; ++i )
		{
			if ( nID == nSkipID[i] )
			{
				bOK = true;	
				break;
			}
		}

		if ( !bOK )	pChildWnd->EnableWindow( FALSE );

		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}

#endif
}

void CNpcActionTable::OnNMDblclkListAction(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	NMITEMACTIVATE* pNMIA = (NMITEMACTIVATE*) pNMHDR;
	if (pNMIA->iItem == -1)
		return;

	DWORD NativeID = (DWORD) m_ctrlActionTable.GetItemData(pNMIA->iItem);
	m_pSheetTab->ActiveActionPage(m_spDialogueSet->GetDialogue( NativeID));
}

void CNpcActionTable::UpdateTalkFileList(const std::vector<std::string>& TalkList)
{
    m_TalkFileList.DeleteAllItems();
    for (size_t i=0; i<TalkList.size(); ++i)
    {
        m_TalkFileList.InsertItem((int) i, TalkList[i].c_str());
    }
}

void CNpcActionTable::TalkFileSelected(int Item)
{
    if (Item == -1)
        return;

    CString TalkFileName = m_TalkFileList.GetItemText(Item, 0);

    std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
    spNpcDialogSet spDlgSet = spTalkMan->Find(std::string(TalkFileName.GetString()));
    if (spDlgSet)
    {
        m_spDialogueSet = spDlgSet;
        m_spDialogueSet->DoModified();
        UpdateItems();
        m_SelectedTalkFileIndex = Item;
    }
}

void CNpcActionTable::OnLvnItemchangedListTalkFile(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    
    // 개별 항목이 새롭게 선택되어진 경우
    if (pNMLV->uNewState)
    {
        int index = pNMLV->iItem;
        // 개별 항목의 현재 상태가 선택되어진 상태일 경우
        if (LVIS_SELECTED == m_TalkFileList.GetItemState(index, LVIS_SELECTED))
        {
            TalkFileSelected(index);
        }
    }
    *pResult = 0;
}

void CNpcActionTable::OnBnClickedButtonAddTalkFile()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    DlgTalkFileAdd Dlg;
    if (Dlg.DoModal() == IDOK)
    {
        std::string NewTalkName(Dlg.m_TalkName.GetString());
        sc::string::lowerString(NewTalkName);
        std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
        if (spTalkMan)
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = spTalkMan->Find(NewTalkName);
            if (spDlgSet)
            {
                MessageBox("사용할 수 없는 이름입니다. 이미 존재하는 이름인지 확인하세요.", "Success", MB_OK|MB_ICONEXCLAMATION);
            }
            else
            {
                spNpcDialogSet NewDlgSet(new CNpcDialogueSet);
                NewDlgSet->SetDlgSetName(NewTalkName);
                
                spTalkMan->AddDlgSet(NewTalkName, NewDlgSet);
                
                m_TalkFileList.DeleteAllItems();
                std::vector<std::string> TalkList;
                spTalkMan->GetDlgList(TalkList);                
                UpdateTalkFileList(TalkList);
            }
        }
    }
}

void CNpcActionTable::OnBnClickedButtonDeleteTalkFile()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_SelectedTalkFileIndex == -1)
        return;
    
    CString TalkFileName = m_TalkFileList.GetItemText(m_SelectedTalkFileIndex, 0);
    std::string DeleteTalkFileName(std::string(TalkFileName.GetString()));
    std::string Msg;
    SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().IsUsingTalkFile(DeleteTalkFileName);
    if (pCrowData)
    {
        Msg = sc::string::format(
                "%1% 는 %2% (%3%/%4%)에서 사용중입니다. 정말 삭제하시겠습니까?",
                DeleteTalkFileName,
                pCrowData->GetName(),
                pCrowData->GetId().wMainID,
                pCrowData->GetId().wSubID);        
    }
    else
    {
        Msg = sc::string::format(
                "%1% 를 정말 삭제하시겠습니까?",
                DeleteTalkFileName);
    }

    if (IDNO == MessageBox(Msg.c_str(), "Confirm", MB_YESNO))
        return;

    std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
    if (spTalkMan)
        spTalkMan->DeleteDlgSet(DeleteTalkFileName);

    m_TalkFileList.DeleteAllItems();
    std::vector<std::string> TalkList;
    spTalkMan->GetDlgList(TalkList);                
    UpdateTalkFileList(TalkList);
}

void CNpcActionTable::OnNMRClickNTKList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	CMenu Menu;
	DWORD SelectionMade;
	VERIFY ( Menu.LoadMenu ( IDR_MENU_LISTDROPDWON ) );

	int pMenuID = 0;
	CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
	ASSERT ( pPopup != NULL );

	POINT pp;
	GetCursorPos (&pp);
	SelectionMade = pPopup->TrackPopupMenu (
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		pp.x, pp.y, this );

	pPopup->DestroyMenu ();

	//HTREEITEM htreeitemCur = m_TalkFileList.GetSelectedItem();

	//if( htreeitemCur == NULL ) return;
	//DWORD dwTreeCurData = m_treeEffectGroup.GetItemData ( htreeitemCur );

	//EFF_PROPERTY* pParentProp = (EFF_PROPERTY*) dwTreeCurData;
	
	std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
	if ( !spTalkMan )
		return ;

	switch ( SelectionMade )
	{
	case ID_MENU_NTKIMPORT:
		{
			CFileDialog dlg ( TRUE, ".ntk", NULL,
				OFN_HIDEREADONLY|OFN_READONLY, "Npc Talk File(*.ntk) |*ntk" );

			dlg.m_ofn.lpstrInitialDir = m_spDialogueSet->GetPath();
			if ( dlg.DoModal() != IDOK )
				return ;

			CString strTalkFileName = dlg.GetFileName().GetString();
// 			const INT iPOS = strTalkFileName.ReverseFind('.');
// 			strTalkFileName = strTalkFileName.Left(iPOS);
			std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = spTalkMan->Find(strTalkFileName.GetString());
			if (spDlgSet)
			{
				// 존재한다면 삭제 여부를 묻는다;
				if ( IDYES == AfxMessageBox("already exist data, overwrite?", MB_YESNO) )
				{
					spDlgSet->RemoveAllDialogue();
					spTalkMan->DeleteDlgSet(strTalkFileName.GetString());
				}
				else
					return ;
			}
			else
			{
				std::tr1::shared_ptr<CNpcDialogueSet> spNewTalk(new CNpcDialogueSet);
				spDlgSet = spNewTalk;
			}

			CString strTargetCheckPath = dlg.GetPathName().GetString();
			strTargetCheckPath = strTargetCheckPath.MakeLower();
			CString strStandardCheckPath;
			strStandardCheckPath.Format("%s%s%s", sc::getAppPath().c_str(), GLogicData::GetInstance().GetPath()->NtkFileRoot().c_str(),dlg.GetFileName().GetString());
			strStandardCheckPath = strStandardCheckPath.MakeLower();
			if ( strStandardCheckPath.Compare(strTargetCheckPath) != 0 || spDlgSet->Load(dlg.GetFileName().GetString()) == FALSE )
			{
				CString str;
				str.Format("Ntk File should be \"%s%s\"",sc::getAppPath().c_str(), GLogicData::GetInstance().GetPath()->NtkFileRoot().c_str());
				AfxMessageBox(str.GetString(), MB_OK);
				return ;
			}
			spDlgSet->SetDlgSetName(dlg.GetFileName().GetString());

			spTalkMan->AddDlgSet(dlg.GetFileName().GetString(), spDlgSet);

			m_TalkFileList.DeleteAllItems();
			std::vector<std::string> vecTalkList;
			spTalkMan->GetDlgList(vecTalkList);
			UpdateTalkFileList(vecTalkList);
			MessageBox("Import Success", "Success", MB_OK|MB_ICONEXCLAMATION);
		}
		break;
	case ID_MENU_NTKEXPORT:
		{	
			CString strRootPath;
			strRootPath.Format("%s%s", sc::getAppPath().c_str(), GLogicData::GetInstance().GetPath()->NtkFileRoot().c_str());
			CFolderDlg dlg("Select Folder", strRootPath.GetString(), ::AfxGetMainWnd());
			if ( dlg.DoModal() != IDOK )
				return ;

			CString strPath = dlg.GetSelectFolderPath();
			const INT iSelCnt = m_TalkFileList.GetSelectedCount();
			POSITION vSelPos = m_TalkFileList.GetFirstSelectedItemPosition();
			for (INT i = 0; i < iSelCnt; ++i)
			{
				const INT iSelItem = m_TalkFileList.GetNextSelectedItem(vSelPos);
				CString strItemName = m_TalkFileList.GetItemText(iSelItem, 0);
				spNpcDialogSet spDlgSet = spTalkMan->Find(std::string(strItemName.GetString()));
				CString strSavePath;
				strSavePath.Format( "%s/%s", strPath.GetString(), strItemName.GetString());
				spDlgSet->Save(strSavePath.GetString());
			}
			MessageBox("Export Success", "Success", MB_OK|MB_ICONEXCLAMATION);
		}
		break;
	}
}

void CNpcActionTable::DialogClear()
{
	m_TalkFileList.DeleteAllItems();
	UpdateItems();
}