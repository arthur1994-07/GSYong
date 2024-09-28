#include "stdafx.h"
#pragma warning (disable:4995)
#include <strsafe.h>
#include "../../Util/SystemInfo.h"
#pragma warning (default:4995)
#include "../../String/StringUtil.h"
#include "ExcelFormat.h"
#include "MinExcel.h"

//#include "../../DebugInclude.h"

namespace sc
{

Excel::Excel( const char* FileName, std::string& SaveFileName, WORD SheetNumber, bool bAutoName )
	: m_bSave(false)
{
    CreateBook();

	if ( NULL == FileName )
		m_FileName = L"Noname";
	else
        m_FileName = sc::string::ansiToUnicode(FileName);
	Init( sc::getAppPath(), bAutoName, SheetNumber );
    SaveFileName = sc::string::unicodeToAnsi(m_FileName);    
}

Excel::Excel( const std::string& FileName, std::string& SaveFileName, WORD SheetNumber, bool bAutoName )
	: m_bSave(false)
{
    CreateBook();

    m_FileName = sc::string::ansiToUnicode(FileName);
	Init( sc::getAppPath(), bAutoName, SheetNumber );
    SaveFileName = sc::string::unicodeToAnsi(m_FileName);
}

Excel::Excel(const std::string& FileName, std::string& PathString, std::string& SaveFileName, WORD SheetNumber, bool bAutoName)
: m_bSave(false)
{
	CreateBook();

	m_FileName = sc::string::ansiToUnicode(FileName);
	Init( PathString, bAutoName, SheetNumber );
	SaveFileName = sc::string::unicodeToAnsi(m_FileName);
}

Excel::Excel()
{
    CreateBook();
    //m_spXls = std::tr1::shared_ptr<YExcel::BasicExcel> (new YExcel::BasicExcel);
}

void Excel::CreateBook()
{
    m_pXls = xlCreateBook();

    if (m_pXls == 0)
    {
        return;
    }

    // licence key
    m_pXls->setKey("Soo young Lee", "windows-242226060bcfef0e61b06a61a6ldc2rf");

    TCHAR szLocaleName[LOCALE_NAME_MAX_LENGTH] = {0};
    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SENGLANGUAGE, szLocaleName, LOCALE_NAME_MAX_LENGTH);

    if (m_pXls->setLocale(szLocaleName) == false)
    {
        ErrorMessageBox();
    }
}

Excel::~Excel()
{
	Save();
    m_pXls->release();
}

void Excel::SetActiveSheet(int Sheet) 
{ 
    m_Sheet = Sheet;
    m_pXls->setActiveSheet(Sheet);
}

void Excel::Init(const std::string& strPath, bool bAutoName, WORD SheetNumber)
{
	m_Sheet = 0;
	m_Row = 0; //! 현재 Row
	m_Col = 0; //! 현재 Column

	//m_spXls = std::tr1::shared_ptr<YExcel::BasicExcel> (new YExcel::BasicExcel);
    m_pXls->release();
    CreateBook();

	if ( 0 == SheetNumber )
    {
        //m_spXls->New(3);
        for (int i=0; i<3; ++i)
            AddWorksheet(std::string(""));
    }
	else
    {
        //m_spXls->New((int) SheetNumber);
        for (int i=0; i<SheetNumber; ++i)
            AddWorksheet(std::string(""));
    }

	wchar_t szFileName[MAX_PATH] = {0};

	std::wstring strFullFileName(sc::string::ansiToUnicode(strPath));
	strFullFileName.append(L"\\");
	strFullFileName.append(m_FileName);

	if (bAutoName)
	{
		struct tm *today;
		//int nRetCode = 0;
		time_t ltime;

		// Use time structure to build a customized time string.
		// Use strftime to build a customized time string.
		::time(&ltime);
		today = ::localtime(&ltime);
		wcsftime(szFileName, MAX_PATH, L"%y%m%d_%H%M.xls", today);
		strFullFileName = strFullFileName + std::wstring(L"_");
	}
	else
	{
		StringCchCopyW(szFileName, MAX_PATH, L".xls");
	}	
	strFullFileName = strFullFileName + std::wstring(szFileName);
	m_FileName = strFullFileName;
    //AfxMessageBox(sc::string::unicodeToAnsi(strFullFileName).c_str(), MB_OK);
}

bool Excel::Load(const std::wstring& FileName)
{
    return Load(sc::string::unicodeToAnsi(FileName));
}

bool Excel::Load(const std::string& FileName)
{
    //return m_spXls->Load(FileName.c_str());
    return m_pXls->load(FileName.c_str());
}

bool Excel::Save()
{
	if (!m_bSave)
	{
		//m_bSave = m_spXls->SaveAs(m_FileName.c_str());
        std::string FileName = sc::string::unicodeToAnsi(m_FileName);
        m_bSave = m_pXls->save(FileName.c_str());
		return m_bSave;
	}
	else
	{
		return true;
	}
}
/*
template <typename T>
void Excel::SetCell(int row, int col, T val)
{
    libxl::Sheet* pSheet = m_pXls->getSheet(0);
    if (pSheet)
    {
        if (!pSheet->writeNum(row, col, static_cast<double> (val)));
            ErrorMessageBox();
    }
}
*/
/*
void Excel::SetCell(int row, int col, int val)
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet((int) 0);
    if (pSheet)
        pSheet->Cell(row,col)->Set(val);
}
*/
/*
void Excel::SetCell(int row, int col, double val)
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet((int) 0);
    if (pSheet)
        pSheet->Cell(row,col)->Set(val);
}
*/
/*
template <>
void Excel::SetCell(int row, int col, const char* szStr)
{
//     YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet((int) 0);
//     if (pSheet && szStr)
//     {
//         std::string TempStr(szStr);
//         std::wstring UnicodeStr = sc::string::ansiToUnicode(TempStr);
//         pSheet->Cell(row,col)->SetWString(UnicodeStr.c_str());
//     }
  
    libxl::Sheet* pSheet = m_pXls->getSheet(0);
    if (pSheet && szStr)
        if (!pSheet->writeStr(row, col, szStr))
            ErrorMessageBox();
}
*/
/*
template <>
void Excel::SetCell(int row, int col, const wchar_t* str)
{    
//     YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet((int) 0);
//     if (pSheet && str)
//         pSheet->Cell(row,col)->SetWString( str );
 
    if (str)
    {
        std::wstring Temp(str);
        std::string AnsiStr = sc::string::unicodeToAnsi(Temp);
        SetCell(row, col, AnsiStr.c_str());
    }
}
*/
/*
template <>
void Excel::SetCell(int row, int col, const std::string& str)
{
//     YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet((int) 0);
//     if (pSheet)
//     {
//         std::wstring UnicodeStr = sc::string::ansiToUnicode(str);
//         pSheet->Cell(row,col)->SetWString(UnicodeStr.c_str());
//     }
 
    SetCell(row, col, str.c_str());
}
*/
/*
template <typename T>
void Excel::SetCell(int Sheet, int row, int col, T val)
{
    libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
    if (pSheet)
        if (!pSheet->writeNum(row, col, static_cast<double> (val)))
            ErrorMessageBox();
}
*/
/*
void Excel::SetCell(int Sheet, int row, int col, unsigned short val)
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
    if (pSheet)
        pSheet->Cell(row,col)->Set( (int) val);
}
*/
/*
void Excel::SetCell(int Sheet, int row, int col, int val)
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
    if (pSheet)
        pSheet->Cell(row,col)->Set(val);
}
*/
/*
void Excel::SetCell(int Sheet, int row, int col, float val)
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
    if (pSheet)
        pSheet->Cell(row,col)->Set( (double) val);
}
*/
/*
void Excel::SetCell(int Sheet, int row, int col, double val)
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
    if (pSheet)
        pSheet->Cell(row,col)->Set(val);
}
*/
/*
template <>
void Excel::SetCell(int Sheet, int row, int col, const char* szStr )
{    
//     YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
//     if (pSheet && szStr)
//     {
//         std::string TempStr(szStr);
//         std::wstring UnicodeStr = sc::string::ansiToUnicode(TempStr);
//         pSheet->Cell(row,col)->SetWString(UnicodeStr.c_str());
//     }
 
    libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
    if (pSheet)
        if (!pSheet->writeStr(row, col, szStr))
            ErrorMessageBox();
}
*/
/*
template <>
void Excel::SetCell(int Sheet, int row, int col, const wchar_t* str)
{    
//     YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
//     if (pSheet && str)
//         pSheet->Cell(row,col)->SetWString( str );
 
    std::wstring Temp(str);
    std::string AnsiStr = sc::string::unicodeToAnsi(Temp);
    SetCell(Sheet, row, col, AnsiStr.c_str());
}
*/
/*
template <>
void Excel::SetCell(int Sheet, int row, int col, const std::string& str)
{    
//     YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
//     std::wstring UnicodeStr = sc::string::ansiToUnicode(str);
//     if (pSheet)
//         pSheet->Cell(row,col)->SetWString(UnicodeStr.c_str());
 
    SetCell(Sheet, row, col, str.c_str());
}
*/
/*
template <>
void Excel::SetCell(int row, int col, const std::wstring& str)
{
//  	YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet((int) 0);
// 	if ( NULL != pSheet )
// 		pSheet->Cell(row,col)->SetWString( str.c_str() );
    std::string Temp = sc::string::unicodeToAnsi(str);
    SetCell(row, col, Temp.c_str());
}
*/
/*
template <>
void Excel::SetCell(int Sheet, int row, int col, const std::wstring& str)
{
//     YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet( Sheet );
// 	if ( NULL != pSheet )
// 		pSheet->Cell(row,col)->SetWString( str.c_str() );
    
    std::string Temp = sc::string::unicodeToAnsi(str);
    SetCell(Sheet, row, col, Temp.c_str());
}
*/
bool Excel::RenameWorksheet(int WorkSheet, const std::wstring& WorkSheetName)
{
    /*
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(WorkSheet);
    if (pSheet)
        return pSheet->Rename(WorkSheetName.c_str());
    else
        return false;
    */
    std::string Temp = sc::string::unicodeToAnsi(WorkSheetName);
    return RenameWorksheet(WorkSheet, Temp);
}

bool Excel::RenameWorksheet(int WorkSheet, const std::string& WorkSheetName)
{
    /*
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(WorkSheet);
    if (pSheet)
        return pSheet->Rename(WorkSheetName.c_str());
    else
        return false;
    */
    libxl::Sheet* pSheet = m_pXls->getSheet(WorkSheet);
    if (pSheet)
    {
        pSheet->setName(WorkSheetName.c_str());
        return true;
    }
    else
    {
        return false;
    }
}

void Excel::AddWorksheet(std::string& SheetName)
{
    //m_spXls->AddWorksheet(-1);
    int SheetCount = WorksheetCount();
    if (SheetName.empty())
        SheetName = sc::string::format("Sheet%1%", SheetCount);
    libxl::Sheet* pSheet = m_pXls->insertSheet(SheetCount, SheetName.c_str());
    if (!pSheet)
        ErrorMessageBox();
}

void Excel::ErrorMessageBox() const
{
    AfxMessageBox(m_pXls->errorMessage(), MB_OK|MB_ICONEXCLAMATION);
}

int Excel::WorksheetCount()
{
    //return m_spXls->GetTotalWorkSheets();
    return m_pXls->sheetCount();
}

///< Total number of rows in current Excel worksheet.
int Excel::WorkSheetTotalRow(int SheetIndex)
{
    /*
    YExcel::BasicExcelWorksheet* pWorkSheet = m_spXls->GetWorksheet(SheetIndex);
    if (pWorkSheet)
        return pWorkSheet->GetTotalRows();
    else
        return -1;
    */
    libxl::Sheet* pSheet = m_pXls->getSheet(SheetIndex);
    if (pSheet)
        return pSheet->lastRow();
    else
        return -1;
}

///< Total number of columns in current Excel worksheet.
int Excel::WorkSheetTotalCols(int SheetIndex)
{
    /*
    YExcel::BasicExcelWorksheet* pWorkSheet = m_spXls->GetWorksheet(SheetIndex);
    if (pWorkSheet)
        return pWorkSheet->GetTotalCols();
    else
        return -1;
    */
    libxl::Sheet* pSheet = m_pXls->getSheet(SheetIndex);
    if (pSheet)
        return pSheet->lastCol();
    else
        return -1;
}
/*
template <typename T>
bool Excel::GetValue(int Sheet, int row, int col, T& Value) const
{
    libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
    if (pSheet)
    {
        double Temp = pSheet->readNum(row, col);
        Value = static_cast<T> (Temp);
        return true;
    }
    else
    {
        return false;
    }
}
*/
/*
bool Excel::GetValue(int Sheet, int row, int col, short& Value) const
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
    if (pSheet)
    {
        int TempValue = 0;
        if (pSheet->Cell(row,col)->Get(TempValue))
        {
            Value = static_cast<short> (TempValue);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
*/
/*
bool Excel::GetValue(int Sheet, int row, int col, int& Value) const
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
    if (pSheet)
        return pSheet->Cell(row,col)->Get(Value);
    else
        return false;
}
*/
/*
bool Excel::GetValue(int Sheet, int row, int col, unsigned short& Value) const
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
    if (pSheet)
    {
        int TempValue = 0;
        if (pSheet->Cell(row,col)->Get(TempValue))
        {
            Value = static_cast<unsigned short> (TempValue);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
*/
/*
bool Excel::GetValue(int Sheet, int row, int col, unsigned long& Value) const
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
    if (pSheet)
    {
        int TempValue = 0;
        if (pSheet->Cell(row,col)->Get(TempValue))
        {
            Value = static_cast<unsigned long> (TempValue);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
*/
/*
bool Excel::GetValue(int Sheet, int row, int col, double& Value) const
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
    if (pSheet)
    {
        double TempValue = 0.0;
        if (pSheet->Cell(row,col)->Get(TempValue))
        {
            Value = TempValue;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
*/
/*
bool Excel::GetValue(int Sheet, int row, int col, float& Value) const
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
    if (pSheet)
    {
        double TempValue = 0.0;
        if (pSheet->Cell(row,col)->Get(TempValue))
        {
            Value = static_cast<float> (TempValue);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
*/
/*
bool Excel::GetValue(int Sheet, int row, int col, __time64_t& Value) const
{
    YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
    if (pSheet)
    {
        int TempValue = 0;
        if (pSheet->Cell(row,col)->Get(TempValue))
        {
            Value = static_cast<__time64_t> (TempValue);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
*/
/*
template <>
bool Excel::GetValue(int Sheet, int row, int col, bool& Value) const
{   
//     YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);    
//     if (pSheet)
//     {
//         int TempValue = 0;
//         if (pSheet->Cell(row,col)->Get(TempValue))
//         {
//             Value = (TempValue == 0) ? false : true;
//             return true;
//         }
//         else
//         {
//             return false;
//         }
//     }
//     else
//     {
//         return false;
//     }
    
    libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
    if (pSheet)
    {
        double Temp = pSheet->readNum(row, col);
        Value = Temp == 0 ? false : true;
        return true;
    }
    else
    {
        return false;
    }
}
*/
/*
template <>
bool Excel::GetValue(int Sheet, int row, int col, std::string& Value) const
{    
//     YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
//     if (pSheet)
//     {
//         YExcel::BasicExcelCell* pCell = pSheet->Cell(row,col);
//         if (!pCell)
//             return false;
// 
//         if (YExcel::BasicExcelCell::STRING == pCell->Type())
//         {
//             Value = pCell->GetString();
//             return true;
//         }
//         else if (YExcel::BasicExcelCell::WSTRING == pCell->Type())
//         {
//             std::wstring TempStr(pCell->GetWString());
//             Value = sc::string::unicodeToAnsi(TempStr);
//             return true;
//         }
//         else
//         {
//             return false;       
//         }
//     }
//     else
//     {
//         return false;
//     }

    libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
    if (pSheet)
    {
        const CHAR* szStr = pSheet->readStr(row, col);
        if (szStr)
        {
            Value = szStr;
            return true;
        }
        else
        {
            ErrorMessageBox();
            return false;
        }
    }
    else
    {
        return false;
    }
}
*/
/*
template <>
bool Excel::GetValue(int Sheet, int row, int col, std::wstring& Value) const
{    
//     YExcel::BasicExcelWorksheet* pSheet = m_spXls->GetWorksheet(Sheet);
//     if (pSheet)
//     {
//         YExcel::BasicExcelCell* pCell = pSheet->Cell(row,col);
//         if (!pCell)
//             return false;
// 
//         if (YExcel::BasicExcelCell::WSTRING == pCell->Type())
//         {
//             Value = pCell->GetWString();
//             return true;
//         }
//         else if (YExcel::BasicExcelCell::STRING == pCell->Type())
//         {
//             std::string TempStr(pCell->GetString());
//             Value = sc::string::ansiToUnicode(TempStr);
//             return true;
//         }
//         else
//         {
//             return false;
//         }
//     }
//     else
//     {
//         return false;
//     }
    
    std::string Temp;
    if (GetValue(Sheet, row, col, Temp))
    {
        Value = sc::string::ansiToUnicode(Temp);
    }
    else
    {
        return false;
    }
}
*/
} // namespace sc