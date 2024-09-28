// C:\tfs\RanAS\Ran\GMCharEdit\DoCharEdit\PropPages\PropertySoldItem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../../GMCharEdit.h"
#include "../../MainFrm.h"
#include "./PropertyDeletedItem.h"
#include "../PropertySheetCharEdit.h"
#include "../../SubDialogs/DlgInvenElementEdit.h"
#include "../../SubDialogs/DlgInvenAdd.h"

#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Item/GLItem.h"
#include "../../RanLogic/Item/GLItemMan.h"

#include "../../RanLogicServer/Database/DbDefineLog.h"
#include "../../RanLogicServer/DxServerInstance.h"

#include "../../Util/CGmCfg.h"

#include "../../Database/ADO/ADOManager.h"
#include "../../Database/ODBC/CGMOdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// CPropertyDeletedItem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropertyDeletedItem, CPropertyPage)

CPropertyDeletedItem::CPropertyDeletedItem()
	: CPropertyPage(CPropertyDeletedItem::IDD)
{

}

CPropertyDeletedItem::~CPropertyDeletedItem()
{
}

void CPropertyDeletedItem::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertyDeletedItem, CPropertyPage)
	ON_NOTIFY(NM_DBLCLK, IDC_SELLITEM_LIST, &CPropertyDeletedItem::OnNMDblclkSellitemList)
END_MESSAGE_MAP()


// CPropertyDeletedItem 메시지 처리기입니다.

void CPropertyDeletedItem::OnNMDblclkSellitemList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
