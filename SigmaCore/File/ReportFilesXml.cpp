#include "stdafx.h"
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../String/StringUtil.h"
#include "../Xml/2.5.3/ticpp.h"
#include "../Util/SystemInfo.h"
#include "../Util/MinMessageBox.h"

#include "./ReportFilesXml.h"

#include "../DebugInclude.h"

namespace sc
{
    ReportFilesXml::ReportFilesXml()
    {
        m_pXml = new ticpp::Document;
    }

    ReportFilesXml::~ReportFilesXml()
    {
        delete m_pXml;
    }
    
    bool ReportFilesXml::Load(const std::string& FileName)
    {
        try
        {            
            m_pXml->LoadFile(FileName, TIXML_ENCODING_UTF8);
            return Parse();
        }
        catch (ticpp::Exception& ex)
        {
            std::string ErrMsg(ex.what());
            sc::ErrorMessageBox(ErrMsg, false);
            return false;
        }
    }

    bool ReportFilesXml::Parse()
    {
        try
        {
            // Ban files
            ticpp::Element* pParent = (ticpp::Element*) m_pXml->FirstChild("REPORT_FILES")->FirstChild("FILTER_FILES");
            if (pParent)
            {
                ticpp::Element* pChild = NULL;
                for (pChild = (ticpp::Element*) pParent->FirstChild(false); pChild; pChild = (ticpp::Element*) pChild->NextSiblingElement(false))
                {
                    std::string Alement = pChild->Value();
                    if (Alement == "FILE")
                    {
                        std::string FileName = pChild->GetText(true);
                        sc::string::lowerString(FileName);
                        m_BanFiles.push_back(FileName);
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

} // namespace sc