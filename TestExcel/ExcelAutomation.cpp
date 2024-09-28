#include "stdafx.h"
#include <ole2ver.h>
#include "./ExcelAutomation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ExcelAutomation::ExcelAutomation()
    : m_pdispExcelApp(NULL)
{
    InitOLE();
    StartExcel();
}

ExcelAutomation::~ExcelAutomation()
{
    if (m_pdispExcelApp)
        m_pdispExcelApp->Release();
}


BOOL ExcelAutomation::InitOLE()
{
    DWORD dwOleVer = CoBuildVersion();
    // check the OLE library version
    if (rmm != HIWORD(dwOleVer)) 
    {
        MessageBox(NULL, _T("Incorrect version of OLE libraries."), "Failed", MB_OK | MB_ICONSTOP);
        return FALSE;
    }

    // could also check for minor version, but this application is
    // not sensitive to the minor version of OLE
    // initialize OLE, fail application if we can't get OLE to init.
    if (FAILED(OleInitialize(NULL))) 
    {
        MessageBox(NULL, _T("Cannot initialize OLE."), "Failed", MB_OK | MB_ICONSTOP);
        return FALSE;
    }
    return TRUE;
}

BOOL ExcelAutomation::StartExcel()
{
	CLSID clsExcelApp;

	// if Excel is already running, return with current instance
	if (m_pdispExcelApp != NULL)
		return TRUE;

	/* Obtain the CLSID that identifies EXCEL.APPLICATION
	 * This value is universally unique to Excel versions 5 and up, and
	 * is used by OLE to identify which server to start.  We are obtaining
	 * the CLSID from the ProgID.
	 */
	if (FAILED(CLSIDFromProgID(L"Excel.Application", &clsExcelApp))) 
	{
		MessageBox(NULL, _T("Cannot obtain CLSID from ProgID"), "Failed", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// start a new copy of Excel, grab the IDispatch interface
	if (FAILED(CoCreateInstance(clsExcelApp, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void**)&m_pdispExcelApp))) 
	{
		MessageBox(NULL, _T("Cannot start an instance of Excel for Automation."), "Failed", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	return TRUE;

}
