#ifndef _EXCEL_AUTOMATION_H_
#define _EXCEL_AUTOMATION_H_

#pragma once

class ExcelAutomation
{
public:
    ExcelAutomation();
    ~ExcelAutomation();

private:
    IDispatch* m_pdispExcelApp;

private:
    BOOL InitOLE();
    BOOL StartExcel();

public:

};

#endif // _EXCEL_AUTOMATION_H_