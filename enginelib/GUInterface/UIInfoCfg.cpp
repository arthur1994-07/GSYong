#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"


#include "../EngineLib/Common/SUBPATH.h"

#include "UIInfoCfg.h"
#include "shlobj.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define UIINFOCFG_FILENAME "%1%_%2%_UIInfo.xml"

CUIInfoCfg& CUIInfoCfg::GetInstance()
{
    static CUIInfoCfg Instance;
    return Instance;
}

CUIInfoCfg::CUIInfoCfg()
    : m_dwVersion(0)
    , m_pInfo(NULL)
{
}

CUIInfoCfg::~CUIInfoCfg()
{
    Clear();
}

const bool CUIInfoCfg::Load(const SUBPATH* pPath)
{
    std::string strFilePath = GetFilePath(pPath);
    if ( strFilePath.empty() )
    {
        GASSERT( !"CUIInfoCfg::Load() - strFilePath is empty");
        return false;
    }

    try {
        m_XmlDoc.LoadFile(strFilePath, TIXML_ENCODING_UTF8);
    }
    catch (...)
    {
        // 로드에 실패하면 생성
        if (!Create())
        {
            // Report the error
            sc::writeLogError(std::string("CUIInfoCfg::Load () - failed")); 
            return false;
        }
    }

    try
    {
        ticpp::Element* pInterface = (ticpp::Element*) m_XmlDoc.FirstChild( "INTERFACE" );
        {
            ticpp::Element* pElem = (ticpp::Element*) pInterface->FirstChild( "VERSION" );
            pElem->GetTextOrDefault<DWORD, DWORD>(&m_dwVersion, 0);

            if ( m_dwVersion == 0 )
            {
                // Report the error
                sc::writeLogError(std::string("CUIInfoCfg::Load () - \"VERSION\" not exist")); 
            }

            m_pInfo = (ticpp::Element*) pInterface->FirstChild( "INFO" );
        }
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        sc::writeLogError(std::string("CUIInfoCfg::Load () - failed"));
        return false;
    }

    return true;
}

const bool CUIInfoCfg::Save (const SUBPATH* pPath)
{
    std::string strFilePath = GetFilePath(pPath);
    if ( strFilePath.empty() )
    {
        GASSERT( !"CUIInfoCfg::Save() - strFilePath is empty");
        return false;
    }

    try {
        m_XmlDoc.SaveFile(strFilePath);
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        sc::writeLogError(std::string("CUIInfoCfg::Save () - failed"));
        return false;
    }

    return true;
}

void CUIInfoCfg::Clear ()
{
    m_dwVersion = 0;
    m_pInfo     = NULL;
    m_XmlDoc.Clear();
}

const bool CUIInfoCfg::Create ()
{
    Clear ();

    try {
        ticpp::Declaration cDecl("1.0", "UTF-8", "");
        m_XmlDoc.LinkEndChild( &cDecl );

        // INTERFACE
        ticpp::Element cEMInterface( "INTERFACE" );
        m_XmlDoc.LinkEndChild( &cEMInterface );
        {
            // VERSION
            ticpp::Element cEMVersion( "VERSION" );
            cEMInterface.LinkEndChild( &cEMVersion );
            {
                cEMVersion.LinkEndChild( &ticpp::Text(UIINFO_VERSION) );
            }

            // INFO
            ticpp::Element cEMInfo( "INFO" );
            cEMInterface.LinkEndChild( &cEMInfo );
        }

        return true;
    }   
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        sc::writeLogError(std::string("CUIInfoCfg::Create () - failed"));
    }

    return false;
}

std::string CUIInfoCfg::GetFilePath(const SUBPATH* pPath)
{
    std::string strUserID = m_AddPathName;
    if ( strUserID.empty() )
    {
        return std::string();
    }
    // 서버넘버 붙이자
    TCHAR szPROFILE[MAX_PATH]={0};
    SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

    std::string strFilePath;
    strFilePath += szPROFILE;
    strFilePath += pPath->UiInfoRoot(); //SUBPATH::UIINFO_ROOT;
    strFilePath += sc::string::format(UIINFOCFG_FILENAME, m_AddPathNameServerNum, strUserID);

    return strFilePath;
}

NSUIINFOCFG::CONTROL CUIInfoCfg::LoadControlInfo ( const std::string& strID )
{
    if ( !m_pInfo )
    {
        return NSUIINFOCFG::CONTROL(NULL);
    }

    try {
        ticpp::Element* pElem = (ticpp::Element*)m_pInfo->FirstChild( strID, false );
        return NSUIINFOCFG::CONTROL(pElem);
    }
    catch (ticpp::Exception& ex) {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        sc::writeLogError(std::string("CUIInfoCfg::LoadControlInfo () - failed"));
    }

    return NSUIINFOCFG::CONTROL(NULL);
}

NSUIINFOCFG::CONTROL CUIInfoCfg::SaveControlInfo ( const std::string& strID, const DWORD dwVersion )
{
    if ( !m_pInfo )
    {
        return NSUIINFOCFG::CONTROL(NULL);
    }

    try {
        ticpp::Element* pElem = (ticpp::Element*)m_pInfo->FirstChild( strID, false );

		// 해당 원소가 없으면 생성
		ticpp::Element cElem( strID );

        if ( !pElem ) 
        {
            pElem = (ticpp::Element*)m_pInfo->LinkEndChild( &cElem );
        }

        // 버전을 세팅
        pElem->SetAttribute( "VERSION", dwVersion );

        return NSUIINFOCFG::CONTROL(pElem, dwVersion);
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        sc::writeLogError(std::string("CUIInfoCfg::SaveControlInfo () - failed"));
    }

    return NSUIINFOCFG::CONTROL(NULL);
}

/*
BOOL CInterfaceCfgXml::Parse_XML( MAP_UI_CFG & pData, const char* lpszFilename )
{
    ASSERT( lpszFilename );

    if( !m_XmlDoc.LoadFile(lpszFilename) )
    {
        return FALSE;
    }

    try
    {
        if( !_ParseConfig( pData ) )	throw;

        return TRUE;
    }
    catch(...)
    {
        return FALSE;
    }	
}
*/
