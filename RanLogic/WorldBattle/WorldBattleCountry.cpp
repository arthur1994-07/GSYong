#include "pch.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"

#include "../GLogicFile.h"
#include "./WorldBattleCountry.h"


#include "../../SigmaCore/DebugInclude.h"

//! namespace world battle
namespace wb
{

Country::Country()
    : m_TextureWidth(0)
    , m_TextureHeight(0)
    , m_FlagWidth(0)
    , m_FlagHeight(0)
{   
}

Country::~Country()
{
    if (m_SQLite.IsOpen())
        m_SQLite.Close();

    m_Data.clear();
}

bool Country::MakeDbFile()
{    
    // memory db 로 관리한다.
    if (SQLITE_OK != m_SQLite.Open(":memory:"))
    {
        sc::ErrorMessageBox(std::string("Country::MakeDbFile Open failed"), false);
        return false;
    }

    // Table 생성
    const char* CreateTable = "\
CREATE TABLE wbCountryInfo( \
    CountryCode     INTEGER NOT NULL, \
    CountryFullName TEXT NOT NULL, \
    Country2Name    TEXT NOT NULL, \
    Country3Name    TEXT NOT NULL, \
    CountryDesc     TEXT NOT NULL, \
    NationalAnthem  TEXT NOT NULL, \
    FlagTexture     TEXT NOT NULL, \
    PosLeft         INTEGER NOT NULL, \
    PosTop          INTEGER NOT NULL, \
    PosRight        INTEGER NOT NULL, \
    PosBottom       INTEGER NOT NULL, \
    ClubCount       INTEGER NOT NULL, \
    PRIMARY KEY(CountryCode)); \
CREATE UNIQUE INDEX Idx_CountryCode ON wbCountryInfo (CountryCode); \
CREATE INDEX Idx_CountryFullName ON wbCountryInfo (CountryFullName); \
CREATE INDEX Idx_Country2Name    ON wbCountryInfo (Country2Name); \
CREATE INDEX Idx_Country3Name    ON wbCountryInfo (Country3Name);" ;

    if (SQLITE_OK != m_SQLite.ExecuteSQL(CreateTable))
    {
        sc::ErrorMessageBox(std::string("Country::MakeDbFile Create Table failed"), false);
        return false;
    }
    else
    {
        return true;
    }
}

bool Country::Load(EMSERVICE_PROVIDER Sp, const std::string& FileName)
{
    if (!MakeDbFile())
        return false;

    try
    {   
        sc::lua_init();
        if (!sc::lua_doFile(FileName))
        {
            std::string ErrMsg(
                sc::string::format(
                    "%1% file missing.(error 6756)",
                    FileName));
            sc::ErrorMessageBox(ErrMsg, false);
            return false;
        }

        // Texture width/height
        LuaPlus::LuaObject ImageLow = sc::lua_getGlobalFromName("wbCountryTextureSize");
        for (LuaPlus::LuaTableIterator Iter(ImageLow); Iter; Iter.Next())
        {            
            m_TextureWidth = static_cast<WORD> (Iter.GetValue().GetInteger()); Iter.Next();
            m_TextureHeight = static_cast<WORD> (Iter.GetValue().GetInteger());
        }

        // Flag width/height
        ImageLow = sc::lua_getGlobalFromName("wbCountryFlagSize");
        for (LuaPlus::LuaTableIterator Iter(ImageLow); Iter; Iter.Next())
        {            
            m_FlagWidth = static_cast<WORD> (Iter.GetValue().GetInteger()); Iter.Next();
            m_FlagHeight = static_cast<WORD> (Iter.GetValue().GetInteger());
        }

		// GuidanceID Effect
		// -- { 선도지역번호, L"Effect이름" }
		LuaPlus::LuaObject GuidEffect = sc::lua_getGlobalFromName("wbEffectGuidanceID");
		if( GuidEffect.IsTable() )
		{
			for (LuaPlus::LuaTableIterator Iter(GuidEffect); Iter; Iter.Next())
			{
				for (LuaPlus::LuaTableIterator IterData(Iter.GetValue()); IterData; IterData.Next())
				{
					DWORD dwGuidanceID = static_cast<DWORD>(IterData.GetValue().GetInteger());	IterData.Next();
					std::wstring strEffectNameW((const wchar_t*) IterData.GetValue().GetWString());

					TSTRING strEffectName = sc::string::unicodeToAnsi(strEffectNameW);
					m_mapEffNameGUID.insert( MAP_EFFNAME_GUID_VALUE(strEffectName.c_str(),dwGuidanceID) );
				}
			}
		}

        // Country info
        // L"국가이름", L"국가이름 두글자", L"국가이름 세글자", 국가코드, L"설명", L"국가", L"국기 이미지 파일", 이미지 좌상단 좌표 X, 이미지 좌상단 좌표 Y }
        LuaPlus::LuaObject CountryInfo = sc::lua_getGlobalFromName("wbCountryInfo");
        for (LuaPlus::LuaTableIterator Iter(CountryInfo); Iter; Iter.Next())
        {
            for (LuaPlus::LuaTableIterator IterData(Iter.GetValue()); IterData; IterData.Next())
            {
                std::wstring CountryFullName((const wchar_t*) IterData.GetValue().GetWString()); IterData.Next();
                std::wstring Country2Name((const wchar_t*) IterData.GetValue().GetWString()); IterData.Next();
                std::wstring Country3Name((const wchar_t*) IterData.GetValue().GetWString()); IterData.Next();
                WORD CountryUniqueCode = static_cast<int> (IterData.GetValue().GetInteger()); IterData.Next();
                std::wstring CountryDesc((const wchar_t*) IterData.GetValue().GetWString()); IterData.Next();
                std::wstring NationalAnthem((const wchar_t*) IterData.GetValue().GetWString()); IterData.Next();
                std::wstring FlagImageFile((const wchar_t*) IterData.GetValue().GetWString()); IterData.Next();

                WORD PosX = static_cast<WORD>(IterData.GetValue().GetInteger()); IterData.Next();
                WORD PosY = static_cast<WORD>(IterData.GetValue().GetInteger()); 

                InsertInfo(
                    EM_COUNTRY(CountryUniqueCode),
                    CountryFullName,
                    Country2Name,
                    Country3Name,
                    CountryDesc,
                    NationalAnthem,
                    FlagImageFile,
                    PosX, 
                    PosY);
            }
        }

        return true;

    }
    catch (const LuaPlus::LuaException& e)
    {
        std::string ErrMsg(
            sc::string::format(
                "%1%, %2%",
                FileName,
                e.GetErrorMessage()));
        sc::ErrorMessageBox(ErrMsg, false);
        return false;
    }
}

bool Country::InsertInfo(
    EM_COUNTRY CountryUniqueCode,
    const std::wstring& CountryFullName,
    const std::wstring& Country2Name,
    const std::wstring& Country3Name,
    const std::wstring& CountryDesc,
    const std::wstring& NationalAnthem,
    const std::wstring& FlagImageFile,
    WORD PosX,
    WORD PosY)
{
    COUNTRY_DATA_ITER iter = m_Data.find(CountryUniqueCode);
    if (iter != m_Data.end())
    {
//         std::string ErrMsg(
//             sc::string::format(
//                 "Same country code exist. code %1%", static_cast<int> (CountryUniqueCode)));
//         sc::ErrorMessageBox(ErrMsg, false);
        return false;
    }

    COUNTRY_INFO CountryCode;

    CountryCode.m_Code = CountryUniqueCode;

    CountryCode.m_FullName = sc::string::unicodeToAnsi(CountryFullName);
    CountryCode.m_Alpha2Code = sc::string::unicodeToAnsi(Country2Name);
    CountryCode.m_Alpha3Code = sc::string::unicodeToAnsi(Country3Name);
    CountryCode.m_Desc = sc::string::unicodeToAnsi(CountryDesc);
    CountryCode.m_Anthem = sc::string::unicodeToAnsi(NationalAnthem);

    CountryCode.m_Pos.m_TextureName = sc::string::unicodeToAnsi(FlagImageFile);    
    CountryCode.m_Pos.m_Left   = PosX;
    CountryCode.m_Pos.m_Top    = PosY;
    CountryCode.m_Pos.m_Right  = PosX+m_FlagWidth;
    CountryCode.m_Pos.m_Bottom = PosY+m_FlagHeight;

    m_Data.insert(COUNTRY_DATA_VALUE(CountryUniqueCode, CountryCode));

    sc::string::replaceForDb(CountryCode.m_FullName);
    sc::string::replaceForDb(CountryCode.m_Alpha2Code);
    sc::string::replaceForDb(CountryCode.m_Alpha3Code);
    sc::string::replaceForDb(CountryCode.m_Desc);
    sc::string::replaceForDb(CountryCode.m_Anthem);
    sc::string::replaceForDb(CountryCode.m_Pos.m_TextureName);

    std::string Query(
        sc::string::format(
        "INSERT INTO wbCountryInfo( \
        CountryCode, CountryFullName, Country2Name, Country3Name, CountryDesc, NationalAnthem, FlagTexture, \
        PosLeft, PosTop, PosRight, PosBottom, ClubCount) VALUES \
        (%1%, '%2%', '%3%', '%4%', '%5%', '%6%', '%7%', \
        %8%, %9%, %10%, %11%, %12%);",
        static_cast<int> (CountryCode.m_Code),
        CountryCode.m_FullName,
        CountryCode.m_Alpha2Code,
        CountryCode.m_Alpha3Code,
        CountryCode.m_Desc,
        CountryCode.m_Anthem,
        CountryCode.m_Pos.m_TextureName,
        CountryCode.m_Pos.m_Left,
        CountryCode.m_Pos.m_Top,
        CountryCode.m_Pos.m_Right,
        CountryCode.m_Pos.m_Bottom,
        CountryCode.m_ClubCount));
    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
    {
        sc::ErrorMessageBox(Query, false);
        return false;
    }
    else
    {        
        return true;
    }
}

//! 국가명으로 검색한다
void Country::Search(const std::string& SearchWord, std::vector<COUNTRY_INFO>& Result)
{
    std::string Query(
        sc::string::format("\
            SELECT CountryCode, CountryFullName, Country2Name, Country3Name, CountryDesc, ClubCount, NationalAnthem, FlagTexture, \
            PosLeft, PosTop, PosRight, PosBottom FROM wbCountryInfo \
            WHERE CountryFullName LIKE '%%%1%%%' OR Country2Name LIKE '%%%1%%%' OR Country3Name LIKE '%%%1%%%' ORDER BY CountryFullName;",
            SearchWord));
    SQLite::TablePtr pTable(m_SQLite.QuerySQL2(Query));
    if (pTable.m_pTable)
    {
        SQLite::Table& rTable = pTable();
        int nRowCount = rTable.GetRowCount();
        if (nRowCount <= 0)
            return;

        for (int i=0; i<nRowCount; ++i)
        {
            COUNTRY_INFO CountryCode;

            CountryCode.m_Code       = static_cast<EM_COUNTRY> (rTable.GetValue<int> (0));
            CountryCode.m_FullName   = rTable.GetValue<std::string> (1);
            CountryCode.m_Alpha2Code = rTable.GetValue<std::string> (2);
            CountryCode.m_Alpha3Code = rTable.GetValue<std::string> (3);
            CountryCode.m_Desc       = rTable.GetValue<std::string> (4);
            CountryCode.m_ClubCount  = rTable.GetValue<WORD> (5);
            CountryCode.m_Anthem     = rTable.GetValue<std::string> (6);

            CountryCode.m_Pos.m_TextureName = rTable.GetValue<std::string> (7);
            CountryCode.m_Pos.m_Left   = rTable.GetValue<WORD> (8);
            CountryCode.m_Pos.m_Top    = rTable.GetValue<WORD> (9);
            CountryCode.m_Pos.m_Right  = rTable.GetValue<WORD> (10);
            CountryCode.m_Pos.m_Bottom = rTable.GetValue<WORD> (11);
            
            Result.push_back(CountryCode);
            rTable.GoNext();
        }
    }
}

void Country::Search(std::vector<COUNTRY_INFO>& Result)
{
    std::string Query(
        std::string("\
            SELECT CountryCode, CountryFullName, Country2Name, Country3Name, CountryDesc, ClubCount, NationalAnthem, FlagTexture, \
            PosLeft, PosTop, PosRight, PosBottom FROM wbCountryInfo ORDER BY CountryFullName;"));
    SQLite::TablePtr pTable(m_SQLite.QuerySQL2(Query));
    if (pTable.m_pTable)
    {
        SQLite::Table& rTable = pTable();
        int nRowCount = rTable.GetRowCount();
        if (nRowCount <= 0)
            return;

        for (int i=0; i<nRowCount; ++i)
        {
            COUNTRY_INFO CountryCode;

            CountryCode.m_Code       = static_cast<EM_COUNTRY> (rTable.GetValue<int> (0));
            CountryCode.m_FullName   = rTable.GetValue<std::string> (1);
            CountryCode.m_Alpha2Code = rTable.GetValue<std::string> (2);
            CountryCode.m_Alpha3Code = rTable.GetValue<std::string> (3);
            CountryCode.m_Desc       = rTable.GetValue<std::string> (4);
            CountryCode.m_ClubCount  = rTable.GetValue<WORD> (5);
            CountryCode.m_Anthem     = rTable.GetValue<std::string> (6);

            CountryCode.m_Pos.m_TextureName = rTable.GetValue<std::string> (7);
            CountryCode.m_Pos.m_Left   = rTable.GetValue<WORD> (8);
            CountryCode.m_Pos.m_Top    = rTable.GetValue<WORD> (9);
            CountryCode.m_Pos.m_Right  = rTable.GetValue<WORD> (10);
            CountryCode.m_Pos.m_Bottom = rTable.GetValue<WORD> (11);

            Result.push_back(CountryCode);
            rTable.GoNext();
        }
    }
}

//! 저해상도 이미지. 국기 위치
const FLAG_TEXTURE_POSITION& Country::GetFlagTexturePos(EM_COUNTRY CountryCode) const
{
    COUNTRY_DATA_CITER iter = m_Data.find(CountryCode);
    if (iter != m_Data.end())
    {
        const COUNTRY_INFO& Info = iter->second;
        return Info.m_Pos;
    }
    else
    {
        static FLAG_TEXTURE_POSITION UnknownPos;
        return UnknownPos;
    }
}


const COUNTRY_INFO& Country::GetData(EM_COUNTRY CountryCode) const
{
    COUNTRY_DATA_CITER iter = m_Data.find(CountryCode);
    if (iter != m_Data.end())
    {     
        return iter->second;
    }
    else
    {
        static COUNTRY_INFO UnknownCountry;
        return UnknownCountry;
    }
}

bool Country::IsExist(EM_COUNTRY CountryCode) const
{
    COUNTRY_DATA_CITER iter = m_Data.find(CountryCode);
    if (iter != m_Data.end())
        return true;
    else
        return false;
}

//! 국가/국가별 클럽숫자
void Country::SetCountryClubInfo(wb::EM_COUNTRY CountryCode, WORD ClubCount)
{
    std::string Query(
        sc::string::format(
            "UPDATE wbCountryInfo SET ClubCount=%1% WHERE CountryCode=%2%;",
            ClubCount,
            static_cast<int> (CountryCode)));
    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
        sc::ErrorMessageBox(Query, false);

    COUNTRY_DATA_ITER iter = m_Data.find(CountryCode);
    if (iter != m_Data.end())
    {
        COUNTRY_INFO* pInfo = &iter->second;
        pInfo->m_ClubCount = ClubCount;
    }
}

const std::string& Country::GetFlagTexture(EM_COUNTRY CountryCode) const
{
    const COUNTRY_INFO& Info = Country::GetData(CountryCode);
    return Info.m_Pos.m_TextureName;
}

//! Effect 가 어느 선도 구역을 대표하는가~?
BOOL Country::GetGuidanceID( const TSTRING& strEffName, DWORD& dwGuidanceID_OUT ) const
{
	MAP_EFFNAME_GUID_CITER citer = m_mapEffNameGUID.find( strEffName );
	if ( citer!=m_mapEffNameGUID.end() )
	{
		dwGuidanceID_OUT = (*citer).second;
		return TRUE;
	}

	return FALSE;
}

} // namespace wb