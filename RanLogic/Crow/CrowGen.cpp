#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "./CrowGen.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SCROWGEN::SCROWGEN()
    : m_dwGenMoney(0)
    , m_sGenItemID(NATIVEID_NULL())
    , m_wGenMoney_Rate(0)
    , m_wGenItem_Rate(0)
{
    m_vecGeneralGenItem.clear();
    m_vecSpecialGenItem.clear();
}

SCROWGEN::SCROWGEN(const SCROWGEN& value)
{
    operator = ( value );
}

void SCROWGEN::Assign(SCROWGEN_100& OldCrowGen)
{
    m_dwGenMoney = OldCrowGen.m_dwGenMoney;
    m_wGenMoney_Rate = OldCrowGen.m_wGenMoney_Rate;

    m_sGenItemID = OldCrowGen.m_sGenItemID;
    m_wGenItem_Rate = OldCrowGen.m_wGenItem_Rate;
}

void SCROWGEN::Assign(SCROWGEN_101& OldCrowGen)
{
    m_dwGenMoney = OldCrowGen.m_dwGenMoney;
    m_wGenMoney_Rate = OldCrowGen.m_wGenMoney_Rate;

    m_sGenItemID = OldCrowGen.m_sGenItemID;
    m_wGenItem_Rate = OldCrowGen.m_wGenItem_Rate;

    std::string strGeneralGenItem = stringformat( "%1%", OldCrowGen.m_szGenItem );
    AddGeneralGenItemFileName( strGeneralGenItem );
}

SCROWGEN& SCROWGEN::operator = (const SCROWGEN &value)
{
    m_dwGenMoney = value.m_dwGenMoney;
    m_wGenMoney_Rate = value.m_wGenMoney_Rate;

    m_sGenItemID = value.m_sGenItemID;
    m_wGenItem_Rate = value.m_wGenItem_Rate;

    m_vecGeneralGenItem = value.m_vecGeneralGenItem;
    m_vecSpecialGenItem = value.m_vecSpecialGenItem;

    return *this;
}

bool SCROWGEN::LOAD_0102(sc::BaseStream& SFile)
{

    SFile >> m_dwGenMoney;
    SFile >> m_sGenItemID.dwID;
    SFile >> m_wGenMoney_Rate;	
    SFile >> m_wGenItem_Rate;	

    std::string strSpecialGenItem;
    std::string strGenItem;

    SFile >> strGenItem;
    SFile >> strSpecialGenItem;

    AddGeneralGenItemFileName( strGenItem );
    AddSpecialGenItemFileName( strSpecialGenItem );

    return true;
}

bool SCROWGEN::LOAD_0103(sc::BaseStream& SFile )
{
    SFile >> m_wGenItem_Rate;
    SFile >> m_wGenMoney_Rate;	
    SFile >> m_sGenItemID.dwID;
    SFile >> m_dwGenMoney;

    std::string strSpecialGenItem;
    std::string strGenItem;

    SFile >> strSpecialGenItem;
    SFile >> strGenItem;

    AddGeneralGenItemFileName( strGenItem );
    AddSpecialGenItemFileName( strSpecialGenItem );


    return true;
}

bool SCROWGEN::LOAD(sc::BaseStream& SFile )
{
    m_vecGeneralGenItem.clear();
    m_vecSpecialGenItem.clear();

    SFile >> m_wGenItem_Rate;
    SFile >> m_wGenMoney_Rate;	
    SFile >> m_sGenItemID.dwID;
    SFile >> m_dwGenMoney;

    std::string strGeneralGenItem;
    std::string strSpecialGenItem;

    SFile >> strGeneralGenItem;
    SFile >> strSpecialGenItem;

    std::vector<std::string> splitVector;
    boost::split(splitVector, strGeneralGenItem, boost::algorithm::is_any_of(",; "), boost::algorithm::token_compress_on);

    BOOST_FOREACH( const std::string& strIter, splitVector )
        AddGeneralGenItemFileName( strIter );

    boost::split(splitVector, strSpecialGenItem, boost::algorithm::is_any_of(",; "), boost::algorithm::token_compress_on);
    BOOST_FOREACH( const std::string& strIter, splitVector )
        AddGeneralGenItemFileName( strIter );

    return true;
}

bool SCROWGEN::SAVE(sc::SerialFile& SFile)
{
    SFile << m_wGenItem_Rate;
    SFile << m_wGenMoney_Rate;
    SFile << m_sGenItemID.dwID;
    SFile << m_dwGenMoney;

    std::string strGenItem = "";
    BOOST_FOREACH( const std::string& strIter, m_vecGeneralGenItem )
    {
        if( strIter.empty() )
            continue;

        strGenItem += strIter;
        strGenItem += ";";
    }
    SFile << strGenItem;

    strGenItem = "";
    BOOST_FOREACH( const std::string& strIter, m_vecSpecialGenItem )
    {
        if( strIter.empty() )
            continue;

        strGenItem += strIter;
        strGenItem += ";";
    }
    SFile << strGenItem;

    return true;
}

VOID SCROWGEN::SaveCsvHead ( std::fstream &SFile )
{
    SFile << "dwGenMoney" << ",";
    SFile << "sGenItemID wMainID" << ",";
    SFile << "sGenItemID wSubID" << ",";
    SFile << "wGenMoney_Rate" << ",";
    SFile << "wGenItem_Rate" << ",";

    SFile << "strGenItem" << ",";
    SFile << "strSpecialGenItem" << ",";
}

VOID SCROWGEN::SaveCsv ( std::fstream &SFile )
{
    SFile << m_dwGenMoney << ",";
    SFile << m_sGenItemID.wMainID << ",";
    SFile << m_sGenItemID.wSubID << ",";
    SFile << m_wGenMoney_Rate << ",";
    SFile << m_wGenItem_Rate << ",";

    {
        CString strGeneralGenItem = "";
        BOOST_FOREACH( std::string& strIter, m_vecGeneralGenItem )
            strGeneralGenItem.AppendFormat("%s ;", strIter.c_str() );

        STRUTIL::OutputStrCsv( SFile, std::string(strGeneralGenItem.GetString()) );
    }

    {
        CString strSpecialGenItem = "";
        BOOST_FOREACH( std::string& strIter, m_vecSpecialGenItem )
            strSpecialGenItem.AppendFormat("%s ;", strIter.c_str() );

        STRUTIL::OutputStrCsv( SFile, std::string(strSpecialGenItem.GetString()) );
    }

}

VOID SCROWGEN::LoadCsv(CStringArray &StrArray, int& iCsvCur)
{
    m_vecGeneralGenItem.clear();
    m_vecSpecialGenItem.clear();

    m_dwGenMoney = (DWORD)atol( StrArray[ iCsvCur++ ] );
    m_sGenItemID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
    m_sGenItemID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
    m_wGenMoney_Rate = (WORD)atoi( StrArray[ iCsvCur++ ] );
    m_wGenItem_Rate = (WORD)atoi( StrArray[ iCsvCur++ ] );

    {
        std::string strGeneralGenItem;
        STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strGeneralGenItem );

        std::vector<std::string> splitVector;
        boost::split(splitVector, strGeneralGenItem, boost::algorithm::is_any_of(",; "), boost::algorithm::token_compress_on);
        BOOST_FOREACH( const std::string& strIter, splitVector )
            AddGeneralGenItemFileName( strIter );
    }

    {
        std::string strSpecialGenItem;
        STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSpecialGenItem );

        std::vector<std::string> splitVector;
        boost::split(splitVector, strSpecialGenItem, boost::algorithm::is_any_of(",; "), boost::algorithm::token_compress_on);
        BOOST_FOREACH( const std::string& strIter, splitVector )
            AddSpecialGenItemFileName( strIter );
    }

}

void SCROWGEN::AddGeneralGenItemFileName( const std::string& strFileName )
{
    if( strFileName.empty() != true )
        m_vecGeneralGenItem.push_back( strFileName );
}

void SCROWGEN::AddSpecialGenItemFileName( const std::string& strFileName )
{
    if( strFileName.empty() != true )
        m_vecSpecialGenItem.push_back( strFileName );
}
