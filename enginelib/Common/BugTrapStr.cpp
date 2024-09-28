#include "pch.h"
#include "./BugTrapStr.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace BUG_TRAP
{
    std::string Email(EMSERVICE_PROVIDER Type)
    {
        return std::string("randev2@mincoms.com");
    }

    std::string ServerAddress(EMSERVICE_PROVIDER Type)
    {
        return std::string("ranbugreport.mincoms.co.kr");
    }

    SHORT Port(EMSERVICE_PROVIDER Type)
    {
        return 8008;
    }

    std::string Country(EMSERVICE_PROVIDER Type)
    {
        std::string StrCountry;
        switch (Type)
        {
        case SP_OFFICE_TEST:     StrCountry = "_OFFICE"; break;
        case SP_KOREA:       StrCountry = "_KR";     break;
        case SP_TAIWAN:      StrCountry = "_TW";     break;
        case SP_CHINA:       StrCountry = "_CH";     break;    
        case SP_JAPAN:       StrCountry = "_JP";     break;
        case SP_PHILIPPINES:       StrCountry = "_PH";     break;
        case SP_THAILAND:    StrCountry = "_TH";     break;
        case SP_GS:          StrCountry = "_GS";     break;
        case SP_GLOBAL:      StrCountry = "_GS";     break;
        case SP_MALAYSIA:    StrCountry = "_MY";     break;
        case SP_MALAYSIA_EN: StrCountry = "_MYE";    break;            
        case SP_INDONESIA:   StrCountry = "_ID";     break;
        case SP_VIETNAM:     StrCountry = "_VN";     break;
        case SP_HONGKONG:    StrCountry = "_HK";     break;
        case SP_KOREA_TEST:  StrCountry = "_KRT";    break;
		case SP_EU:          StrCountry = "_EU";     break;
		case SP_US:          StrCountry = "_US";     break;
        default:             StrCountry = "_UNKOWN"; break;
        }
        return StrCountry;
    }

    std::string SupportUrl(EMSERVICE_PROVIDER Type)
    {
        std::string Url;
        switch (Type)
        {
        case SP_OFFICE_TEST:     Url = "http://www.ran-online.co.kr"; break;
        case SP_KOREA:       Url = "http://www.ran-online.co.kr";     break;
        case SP_TAIWAN:      Url = "http://www.yong-online.com.tw";     break;
        case SP_CHINA:       Url = "http://lx.5u56.com";     break;    
        case SP_JAPAN:       Url = "http://ranonline.jp";     break;
        case SP_PHILIPPINES:       Url = "http://ranonline.e-games.com.ph";     break;
        case SP_THAILAND:    Url = "http://www.ran.in.th/";     break;
        case SP_GS:          Url = "http://www.ran-world.com";     break;
        case SP_GLOBAL:      Url = "http://www.ran-world.com";     break;
        case SP_MALAYSIA:    Url = "http://www.myrosso.com";     break;
        case SP_MALAYSIA_EN: Url = "http://www.myrosso.com";    break;            
        case SP_INDONESIA:   Url = "http://ran.vtconline.co.id";     break;
        case SP_VIETNAM:     Url = "http://www.ranonline.com.vn";     break;
        case SP_HONGKONG:    Url = "http://www.yong-online.com.hk/";     break;
        case SP_KOREA_TEST:  Url = "http://www.ran-online.co.kr";    break;
		case SP_EU:          Url = "http://www.games-masters.com";     break;
		case SP_US:          Url = "http://www.gamesamba.com";     break;
        default:             Url = "http://www.ran-world.com"; break;
        }
        return Url;
    }
}
