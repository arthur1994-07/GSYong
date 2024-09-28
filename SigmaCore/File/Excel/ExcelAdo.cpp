#include "stdafx.h"
#include "../../String/StringFormat.h"
#include "../../String/StringUtil.h"
#include "../../Database/Ado/adoclass.h"
#include "./ExcelAdo.h"

#include "../../DebugInclude.h"

inline void TESTHR(HRESULT x) {if FAILED(x) _com_issue_error(x);};


ExcelSheet::ExcelSheet()
{
}

ExcelSheet::~ExcelSheet()
{
}

void ExcelSheet::SetName(const std::string& SheetName)
{
    std::string TempSheetName(SheetName);
    if (std::string::npos != TempSheetName.find(" "))
    {
        std::string ErrorMsg(
            sc::string::format(
                "%1% 시트 이름에 공백을 포함하면 안됩니다.",
                TempSheetName));
        AfxMessageBox(ErrorMsg.c_str(), MB_OK);
    }

    // 공백제거
    sc::string::trim(TempSheetName, " ");
    m_SheetName = TempSheetName;
}

void ExcelSheet::SetHeader(const std::vector<std::string>& Header)
{
    m_Header.clear();
    m_Header = Header;
}

void ExcelSheet::AddData(const std::vector<std::string>& Data)
{
    if (Data.size() > m_Header.size())
    {
        std::string ErrorMsg(
            sc::string::format(
                "%1% 컬럼 크기 초과 %2% > %3%",
                m_SheetName,
                Data.size(),
                m_Header.size()));
        AfxMessageBox(ErrorMsg.c_str(), MB_OK);        
    }
    else
    {
        RowData InsertData(Data);
        m_Data.push_back(InsertData);
    }
}

ExcelAdo::ExcelAdo(const std::string& FileName, EM_MODE Mode)
    : m_Mode(Mode)
    , m_bSave(false)
{    
}

ExcelAdo::~ExcelAdo()
{
    if (m_Mode == WRITE && !m_bSave)
        Save();
}

void ExcelAdo::AddRow(WORD Sheet, const std::vector<std::string>& Data)
{
    if (Sheet >= m_Data.size())
    {
        std::string ErrorMsg(
            sc::string::format(
                "존재하지 않는 시트입니다. %1% >= %2%",
                Sheet,
                m_Data.size()));
        AfxMessageBox(ErrorMsg.c_str(), MB_OK);
    }
    else
    {
        m_Data[Sheet].AddData(Data);
    }
}

void ExcelAdo::AddWorksheet()
{
    std::string WorkSheetName(
        sc::string::format(
            "WorkSheet%1%",
            m_Data.size() + 1));
    std::vector<std::string> ColumnName;
    AddSheet(WorkSheetName, ColumnName);
}

size_t ExcelAdo::GetWorkSheetTotalRow(WORD Sheet)
{
    if (Sheet >= m_Data.size())
    {
        std::string ErrMsg(
            sc::string::format(
            "존재하지않는 시트입니다. %1% > %2%",
            Sheet,
            m_Data.size()));
        AfxMessageBox(ErrMsg.c_str(), MB_OK);
        return 0;
    }
    else
    {
        return m_Data[Sheet].GetRowSize();
    }
}

void ExcelAdo::RenameWorksheet(WORD Sheet, const std::string& SheetName)
{
    if (Sheet >= m_Data.size())
    {
        std::string ErrMsg(
            sc::string::format(
                "존재하지않는 시트입니다. %1% > %2%",
                Sheet,
                m_Data.size()));
        AfxMessageBox(ErrMsg.c_str(), MB_OK);
    }
    else
    {
        m_Data[Sheet].SetName(SheetName);
    }
}

WORD ExcelAdo::AddSheet(const std::string& SheetName, const std::vector<std::string>& Header)
{
    ExcelSheet Sheet;
    Sheet.SetName(SheetName);
    Sheet.SetHeader(Header);
    m_Data.push_back(Sheet);
    return static_cast<WORD> (m_Data.size() - 1);
}

void ExcelAdo::SetWorksheetHeader(WORD Sheet, const std::vector<std::string>& Header)
{
    if (Sheet >= m_Data.size())
    {
        std::string ErrMsg(
            sc::string::format(
               "존재하지않는 시트입니다. %1% > %2%",
                Sheet,
                m_Data.size()));
        AfxMessageBox(ErrMsg.c_str(), MB_OK);
    }
    else
    {
        m_Data[Sheet].SetHeader(Header);
    }
}

void ExcelAdo::AddWorksheetHeader(WORD Sheet, const std::string& Header)
{
    if (Sheet >= m_Data.size())
    {
        std::string ErrMsg(
            sc::string::format(
            "존재하지않는 시트입니다. %1% > %2%",
            Sheet,
            m_Data.size()));
        AfxMessageBox(ErrMsg.c_str(), MB_OK);
    }
    else
    {
        m_Data[Sheet].AddHeader(Header);
    }
}

void ExcelAdo::Save()
{
    try
    {
        ::CoInitialize(NULL);
        m_ConnectionString = "Provider=Microsoft.JET.OLEDB.4.0;Data Source=d:\\data.xls;Extended Properties=\"Excel 8.0\"";
        
        TESTHR(m_pCon.CreateInstance(__uuidof(Connection)));
        TESTHR(m_pCon->Open(m_ConnectionString.c_str(), "", "", NULL));

        SaveSheet();

        ::CoUninitialize();
    }
    catch(_com_error &e)
    {        
        _bstr_t bstrDescription(e.Description());      
        CharToOem(bstrDescription, bstrDescription);
        AfxMessageBox(bstrDescription, MB_OK);
        //std::cout << bstrDescription << std::endl;

        ::CoUninitialize();
    }    
}

void ExcelAdo::SaveSheet()
{
    for (size_t i=0; i<m_Data.size(); ++i)
        SaveSheet(static_cast<WORD> (i));
}

void ExcelAdo::SaveSheet(WORD Sheet)
{
    std::string CreateQuery;

    try
    {
        const ExcelSheet& SheetData = m_Data[Sheet];

        CreateQuery.append("CREATE TABLE ");
        CreateQuery.append(SheetData.GetName());
        CreateQuery.append("(");
        for (size_t i=0; i<SheetData.GetHeaderSize(); ++i)
        {
            CreateQuery.append(SheetData.GetHeader(i));
            CreateQuery.append(" varchar");
            if (i < (SheetData.GetHeaderSize()-1))
                CreateQuery.append(", ");

        }
        CreateQuery.append(")");

        _CommandPtr pCmd = NULL;
        _RecordsetPtr pRec = NULL;

        TESTHR(pCmd.CreateInstance(__uuidof(Command)));
        pCmd->ActiveConnection = m_pCon;
        pCmd->CommandText = CreateQuery.c_str();
        pCmd->Execute(NULL, NULL, adCmdText);

        std::string SelectQuery("SELECT * FROM ");
        SelectQuery.append(SheetData.GetName());

        TESTHR(pRec.CreateInstance(__uuidof(Recordset)));
        pRec->Open(SelectQuery.c_str(), _variant_t((IDispatch*) m_pCon), adOpenKeyset, adLockOptimistic, adCmdText);

        size_t RowSize = SheetData.GetRowSize();
        for (size_t i=0; i<RowSize; ++i)
        {
            RowData rowData = SheetData.GetRowData(i);
            TESTHR(pRec->AddNew());

            for (size_t j=0; j<rowData.GetSize(); ++j)
            {
                if (j < SheetData.GetHeaderSize())
                {
                    std::string ColumnData = rowData.GetData(j);
                    sc::string::SqlInjectionCheck(ColumnData);
                    pRec->Fields->GetItem(SheetData.GetHeader(j).c_str())->Value = _variant_t(ColumnData.c_str());
                }
            }
        }

        if (RowSize > 0)
        {
            TESTHR(pRec->Update());
            TESTHR(pRec->Close()); 
        }
    }
    catch(_com_error &e)
    {        
        _bstr_t bstrDescription(e.Description());      
        CharToOem(bstrDescription, bstrDescription);
        std::string ErrMsg(bstrDescription);
        ErrMsg.append(" ");
        ErrMsg.append(CreateQuery);
        AfxMessageBox(ErrMsg.c_str(), MB_OK);
        //std::cout << bstrDescription << std::endl;        
    }   
}