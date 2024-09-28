#include "stdafx.h"
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/Util/MinMessageBox.h"

#include "XmlConfig.h"

#include "../SigmaCore/DebugInclude.h"

XmlConfig::XmlConfig()
{
    m_pXml = new ticpp::Document;
}

XmlConfig::~XmlConfig()
{
    delete m_pXml;
}

bool XmlConfig::Load(const char* filename)
{
    try
    {
        std::string AppPath = sc::getAppPath();
        AppPath.append("\\");
        AppPath.append(filename);        
        m_pXml->LoadFile(AppPath, TIXML_ENCODING_UTF8);
        return Parse();
    }
    catch (ticpp::Exception& ex)
    {
        std::string ErrMsg(ex.what());
        sc::ErrorMessageBox(ErrMsg, false);
        return false;
    }
}

bool XmlConfig::Parse()
{
    try
    {
        ticpp::Element* pElem = NULL;
        
        // Target path
        pElem = (ticpp::Element*) m_pXml->FirstChild("VERSION_MANAGER")->FirstChild("TARGET_PATH");
        m_TargetPath = pElem->GetText(true);

        // DB
        pElem = (ticpp::Element*) m_pXml->FirstChild("VERSION_MANAGER")->FirstChild("DB")->FirstChild("DB_SOURCE");
        m_DbSource = pElem->GetText(true);

        pElem = (ticpp::Element*) m_pXml->FirstChild("VERSION_MANAGER")->FirstChild("DB")->FirstChild("NAME");
        m_DbName = pElem->GetText(true);

        pElem = (ticpp::Element*) m_pXml->FirstChild("VERSION_MANAGER")->FirstChild("DB")->FirstChild("USER");
        m_DbUserId = pElem->GetText(true);

        pElem = (ticpp::Element*) m_pXml->FirstChild("VERSION_MANAGER")->FirstChild("DB")->FirstChild("PASSWORD");
        m_DbUserPassword = pElem->GetText(true);

        // FTP
        ticpp::Element* pParent = (ticpp::Element*) m_pXml->FirstChild("VERSION_MANAGER")->FirstChild("FTP");
        if (pParent)
        {
            ticpp::Element* pChild = NULL;
            for (pChild = (ticpp::Element*) pParent->FirstChild(false); pChild; pChild = (ticpp::Element*) pChild->NextSiblingElement(false))
            {
                std::string Alement = pChild->Value();
                if (Alement == "IP")
                {
                    std::string IpAddress = pChild->GetText(true);
                    m_vFtpIP.push_back(CString(IpAddress.c_str()));
                }
                else if (Alement == "PORT")
                {
                    pChild->GetTextOrDefault<int, int> (&m_nFtpPort, 21);
                }
                else if (Alement == "USER")
                {
                    m_FtpUserName = pChild->GetText(true);
                }
                else if (Alement == "PASSWORD")
                {
                    m_FtpPassword = pChild->GetText(true);
                }
                else
                {
                    // unknown
                }
            }
        }

        return true;
    }
    catch (ticpp::Exception& ex)
    {        
        std::string ErrLog(ex.what());
        ErrLog.append(" ");
        ErrLog.append("XmlConfig parse error.");
        sc::ErrorMessageBox(ErrLog, false);
        return false;
    }
}