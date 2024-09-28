#ifndef _INTERFACE_BASE_TYPE_H_
#define _INTERFACE_BASE_TYPE_H_

#pragma once

namespace UI
{
    struct SCOLORPANNELGROUP_INFO
    {
        enum {
            MAX_PANNEL = 2,
        };

        std::string        strLABEL;
        DWORD              dwCOLOR[MAX_PANNEL];
        DWORD              dwNUMPANNEL;
        BOOL               bENABLE;

        SCOLORPANNELGROUP_INFO ()
            : dwNUMPANNEL(0)
            , bENABLE(TRUE)
        {
            memset(dwCOLOR, 0, sizeof(dwCOLOR) );
        }
    };

    typedef std::vector<UI::SCOLORPANNELGROUP_INFO> SPANNELGROUPINFOARRAY;

	void StringSplit( const std::string& strOrigin, const std::string& strKey, std::vector<std::string>& vecStrResult );
};

#endif