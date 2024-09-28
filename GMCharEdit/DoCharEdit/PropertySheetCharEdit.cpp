// PropertySheetCharEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../../RanLogic/GLogicEx.h"
#include "../../RanLogic/Character/GLCharData.h"

#include "../GMCharEdit.h"
#include "../SubDialogs/DlgMakeCronCharacter.h"
#include "../Util/CGmCfg.h"
#include "../Util/CConsoleMessage.h"
#include "../Database/DatabaseTable.h"
#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"
#include "PropertySheetCharEdit.h"

#include "../DataManager/CDataManagerProduct.h"
#include "../DataManager/CDataManagerClubInven.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// CPropertySheetCharEdit

IMPLEMENT_DYNAMIC(CPropertySheetCharEdit, CPropertySheet)
CPropertySheetCharEdit::CPropertySheetCharEdit(int nChaNum, int nUserNumber, UINT nIDCaption, CWnd* pParentWnd /* = NULL */, UINT iSelectPage /* = 0 */, bool ShowRestore /* = false */)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    m_pChar = NULL;
    m_nChaNum = nChaNum;
    m_nUserNumber = nUserNumber;
    m_ShowRestore = ShowRestore;

    AddAllPages();
}

CPropertySheetCharEdit::CPropertySheetCharEdit(int nChaNum, int nUserNumber, LPCTSTR pszCaption, CWnd* pParentWnd /* = NULL */, UINT iSelectPage /* = 0 */, bool ShowRestore /* = fals */)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    m_pChar = NULL;
    m_nChaNum = nChaNum;
    m_nUserNumber = nUserNumber;
    m_ShowRestore = ShowRestore;

    AddAllPages();
}

void CPropertySheetCharEdit::AddAllPages()
{
    m_ChaUserInven0.m_nUserInvenNum = 0;
    m_ChaUserInven0.m_psp.pszTitle = _T("User Inven #1");
    m_ChaUserInven0.m_psp.dwFlags |= PSP_USETITLE;
    m_ChaUserInven0.m_psp.dwFlags &= ~PSP_HASHELP;

    m_ChaUserInven1.m_nUserInvenNum = 1;
    m_ChaUserInven1.m_psp.pszTitle = _T("User Inven #2");
    m_ChaUserInven1.m_psp.dwFlags |= PSP_USETITLE;
    m_ChaUserInven1.m_psp.dwFlags &= ~PSP_HASHELP;

    m_ChaUserInven2.m_nUserInvenNum = 2;
    m_ChaUserInven2.m_psp.pszTitle = _T("User Inven #3");
    m_ChaUserInven2.m_psp.dwFlags |= PSP_USETITLE;
    m_ChaUserInven2.m_psp.dwFlags &= ~PSP_HASHELP;

    m_ChaUserInven3.m_nUserInvenNum = 3;
    m_ChaUserInven3.m_psp.pszTitle = _T("User Inven #4");
    m_ChaUserInven3.m_psp.dwFlags |= PSP_USETITLE;
    m_ChaUserInven3.m_psp.dwFlags &= ~PSP_HASHELP;

    m_ChaUserInvenP.m_nUserInvenNum = 4;
    m_ChaUserInvenP.m_psp.pszTitle = _T("User Inven #P");
    m_ChaUserInvenP.m_psp.dwFlags |= PSP_USETITLE;
    m_ChaUserInvenP.m_psp.dwFlags &= ~PSP_HASHELP;

    AddPage(&m_ChaBase);
	AddPage(&m_ChaSkill);
	AddPage(&m_ChaPutOnItem);
    AddPage(&m_ChaQuest);
	
	//
	//mjeon.ado
	//
	// newly added features will be only supported by ADO.
	//
	if (CGmCfg::GetInstance()->m_nDBType == gmce::ADO)
	{
		AddPage(&m_ChaVehicle);
	}

    AddPage(&m_ChaInven);
    AddPage(&m_ChaUserInven0);
    AddPage(&m_ChaUserInven1);
    AddPage(&m_ChaUserInven2);
    AddPage(&m_ChaUserInven3);
    AddPage(&m_ChaUserInvenP);
	AddPage(&m_ChaItemMix);
	AddPage(&m_ChaClubInven);
}

CPropertySheetCharEdit::~CPropertySheetCharEdit()
{
	// 이제 유저락커 가져올 때 sp_ItemGetItemLocker 를 사용하지않기 때문에 아래 유저인벤옵션을 따로 변경해줄 필요가 없다.
// 	if ( m_pDBMan && m_pChar )
// 	{
// 		m_pDBMan->UpdateUserInvenOption( m_pChar->GetUserID(), 0 );
// 	}

    SAFE_DELETE(m_pChar);
}

BEGIN_MESSAGE_MAP(CPropertySheetCharEdit, CPropertySheet)
END_MESSAGE_MAP()


// CPropertySheetCharEdit 메시지 처리기입니다.

BOOL CPropertySheetCharEdit::OnInitDialog()
{
	SAFE_DELETE(m_pChar);

	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();

	//
	//mjeon.ado.Vehicle information will be processed in it's own PropertyPage(PropertyVehicle).
	//
    m_pChar = new GLCHARLOGIC;
    if (m_pDBMan->GetCharacterInfo(m_nChaNum, m_nUserNumber, m_pChar) == DB_ERROR)
	{
		MessageBox(_T("Database Error"));
	}
	else if (m_pDBMan->GetUserInven(m_pChar) == DB_ERROR)
	{
		MessageBox(_T("Database Error"));
	}
	else
	{

	}

	// 제조 Property;
	dm::CDataManagerProduct::Instance()->SetChaNum( m_nChaNum );
	dm::CDataManagerProduct::Instance()->SetDBManager( m_pDBMan );
	dm::CDataManagerProduct::Instance()->Initialize();
	//m_ChaItemMix.Initialize( m_nChaNum );

	// 클럽 인벤 Property;
	dm::CDataManagerClubInven::Instance()->SetChaNum( m_nChaNum );
	dm::CDataManagerClubInven::Instance()->SetDBManager( m_pDBMan );
	dm::CDataManagerClubInven::Instance()->Initialize();

    BOOL bResult = CPropertySheet::OnInitDialog();

    // TODO: 여기에 특수화된 코드를 추가합니다.
	// IDOK            - OK button
	// IDCANCEL        - Cancel button
	// ID_APPLY_NOW    - Apply button
	// IDHELP          - Help button
    if ( !m_ShowRestore )
    {
        GetDlgItem(IDOK)->SetWindowText(_T("Save"));
        GetDlgItem(IDCANCEL)->SetWindowText(_T("Cancel"));
    }
    else
    {
        GetDlgItem(IDOK)->SetWindowText(_T("Save"));
        GetDlgItem(IDCANCEL)->SetWindowText(_T("Restore"));
    }

    GetDlgItem(ID_APPLY_NOW)->ShowWindow(SW_HIDE);
    GetDlgItem(IDHELP)->ShowWindow(SW_HIDE);
  
    return bResult;
}