// C:\tfs\RanAS\Ran\GMCharEdit\DoCharEdit\PropPages\PropertySoldItem.cpp : ���� �����Դϴ�.
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


// CPropertyDeletedItem ��ȭ �����Դϴ�.

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


// CPropertyDeletedItem �޽��� ó�����Դϴ�.

void CPropertyDeletedItem::OnNMDblclkSellitemList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}
