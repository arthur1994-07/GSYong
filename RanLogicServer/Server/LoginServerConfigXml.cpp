#include "pch.h"
#pragma warning(disable:4267)
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../../SigmaCore/Encrypt/DbEncrypt.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Net/NetUtil.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "./ServerConfigHelper.h"
#include "./LoginServerConfigXml.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

LoginServerConfigXml::LoginServerConfigXml()
{
    m_PatchVersion = 0;
    m_LauncherVersion = 0;
    m_nServerChannel = 0;
}

LoginServerConfigXml::~LoginServerConfigXml()
{
}

bool LoginServerConfigXml::Parse()
{
    if (!ServerConfigXml::Parse())
        return false;
    
    if (!ServerConfigHelper::ParseServerChannel(m_pXml, m_nServerChannel))
        return false;

    if (!ServerConfigHelper::ParseSessionServer(m_pXml, m_SessionServer))
        return false;

    ParseAllowIp();
    return true;
}

bool LoginServerConfigXml::ParseAllowIp()
{
    return ParseAllowIp("ALLOW_IP", m_AllowIPType1);
}

bool LoginServerConfigXml::ParseAllowIp(const std::string& ElmtName, MAP_ALLOW_IP& MapIp)
{
    try
    {
        ticpp::Element* pParent = (ticpp::Element*) m_pXml->FirstChild("RAN_SERVER")->FirstChild(ElmtName);
        if (pParent)
        {
            ticpp::Element* pChild = NULL;
            for (pChild = (ticpp::Element*) pParent->FirstChild(false);
                pChild;
                pChild = (ticpp::Element*) pChild->NextSiblingElement(false))
            {
                std::string Alement = pChild->Value();
                if (Alement == "IP")
                {
                    // Start
                    std::string IpStart;
                    pChild->GetAttribute("START", &IpStart);

                    // End
                    std::string IpEnd;
                    pChild->GetAttribute("END", &IpEnd);                   

                    MakeIpList(IpStart, IpEnd, MapIp);
                }
            }
        }
        return true;
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        sc::writeLogError(std::string(ex.what()));
        sc::writeLogError(
            sc::string::format(
            "Check! RAN_SERVER/%1%", ElmtName));
        return true;
    }
}

bool LoginServerConfigXml::MakeIpList(std::string IpStart, std::string IpEnd, MAP_ALLOW_IP& MapIp)
{
    try
    {
        sc::string::trim(IpStart);
        sc::string::trim(IpEnd);

        typedef std::vector<std::string> splitVector;
        splitVector SplitStart;
        splitVector SplitEnd;

        // '.' 토큰분리
        boost::split(SplitStart, IpStart, boost::algorithm::is_any_of("."));
        boost::split(SplitEnd,   IpEnd,   boost::algorithm::is_any_of("."));

        int Start = boost::lexical_cast<int> (SplitStart[3]);
        int End   = boost::lexical_cast<int> (SplitEnd[3]);

        if (Start > End)
        {
            int Temp = Start;
            Start = End;
            End = Temp;
        }

        std::string Info(
            sc::string::format(
                "Allow Ip %1%.%2%.%3%.%4% To %1%.%2%.%3%.%5%",
                SplitStart[0],
                SplitStart[1],
                SplitStart[2],
                Start,
                End));
        sc::writeLogInfo(Info);

        for (int i=Start; i<=End; ++i)
        {
            std::string IpAddress(
                sc::string::format(
                "%1%.%2%.%3%.%4%",
                SplitStart[0],
                SplitStart[1],
                SplitStart[2],
                i));
            //size_t HashValue = m_StringHash( IpAddress );
            m_AllowIPType1.insert(MAP_ALLOW_IP_VALUE(IpAddress, IpAddress));
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        sc::writeLogError(std::string(e.what()));        
        return false;
    }
}

bool LoginServerConfigXml::IsAllowIp(const std::string& strAllowIP) const
{ 
    MAP_ALLOW_IP_CITER iter = m_AllowIPType1.find(strAllowIP);
    if (iter != m_AllowIPType1.end())
        return true;
    else
        return false;
}