
// bjju.StudentRecord

#include "StdAfx.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../Util/UIPageFrame.h"
#include "../Util/UIPage.h"

#include "./StudentRecordWindow.h"
#include "./StudentRecordWindowRequest.h"

#include "./ActivityWindowPage.h"
#include "./AttendancePage.h"

#include "./ActivityHelpWindow.h"
#include "./ActivitySalesItemWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CStudentRecordWindow::UpdatePageRequest ( UIGUID ControlID, const SPAGEREQUEST& sRequest )
{
    switch ( sRequest.dwID )
    {
    case NS_STUDENTRECORDWINDOW::PAGE_REQ_ACTIVITY_HELPWINDOW_OPEN :
        {
            CloseModalWindow();

            m_pOpenModal = m_pActivityHelp;
            m_pOpenModal->SetVisibleSingle( TRUE );
        }
        break;

    case NS_STUDENTRECORDWINDOW::PAGE_REQ_ACTIVITY_SALESITEMWINDOW_OPEN :
        {
            CloseModalWindow();

            m_pActivitySalesItem->InitSalesList ( CActivitySalesItemWindow::EMSALESTYPE(sRequest.dwARG1) );

            m_pOpenModal = m_pActivitySalesItem;
            m_pOpenModal->SetVisibleSingle( TRUE );
        }
        break;
    };
}
