#pragma once

#include <string>
#include <vector>

//#import "C:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF", "adoEOF") rename("BOF", "adoBOF")
//#import "C:\Program Files\Common Files\System\ADO\msado21.tlb" rename ("EOF", "adoEOF") no_namespace
//#import "C:\Program Files\Common Files\System\ADO\msado27.tlb" rename ("EOF", "adoEOF") no_namespace

class RowData
{
protected:
    std::vector<std::string> m_Data;

public:
    RowData()
    {
    }

    RowData(const std::vector<std::string>& Data)        
        : m_Data(Data)
    {
    }

    size_t GetSize() const { return m_Data.size(); }
    std::string GetData(size_t i) const
    {
        if (i >= m_Data.size())
            return std::string("");
        else
            return m_Data[i];
    }
};

class ExcelSheet
{
public:
    ExcelSheet();
    ~ExcelSheet();

protected:
    std::string m_SheetName;
    std::vector<std::string> m_Header;
    std::vector<RowData> m_Data;

public:
    std::string GetName() const { return m_SheetName; }
    size_t GetHeaderSize() const { return m_Header.size(); }
    void SetName(const std::string& SheetName);
    void SetHeader(const std::vector<std::string>& Header);
    void AddHeader(const std::string& Header)
    {
        m_Header.push_back(Header);
    }

    void AddData(const std::vector<std::string>& Data);
    std::string GetHeader(size_t i) const
    {
        if (i >= m_Header.size())
            return std::string("");
        else
            return m_Header[i];
    }

    size_t GetRowSize() const { return m_Data.size(); }

    RowData GetRowData(size_t i) const
    {
        if (i >= m_Data.size())
            return RowData();
        else
            return m_Data[i];
    }
};

class ExcelAdo
{
public:
    enum EM_MODE 
    { 
        READ,
        WRITE,
    };

public:
    ExcelAdo(const std::string& FileName, EM_MODE Mode);
    ~ExcelAdo();

protected:
    EM_MODE m_Mode;
    std::string m_ConnectionString;    
    bool m_bSave;
    _ConnectionPtr m_pCon;
    std::vector<ExcelSheet> m_Data;
    
public:
    void AddWorksheet();
    void RenameWorksheet(WORD Sheet, const std::string& SheetName);
    void SetWorksheetHeader(WORD Sheet, const std::vector<std::string>& Header);
    void AddWorksheetHeader(WORD Sheet, const std::string& Header);

    WORD AddSheet(const std::string& SheetName, const std::vector<std::string>& Header);
    void AddRow(WORD Sheet, const std::vector<std::string>& Data);    
    void Save();
    size_t GetWorkSheetTotalRow(WORD Sheet);

protected:
    void SaveSheet();
    void SaveSheet(WORD Sheet);
};