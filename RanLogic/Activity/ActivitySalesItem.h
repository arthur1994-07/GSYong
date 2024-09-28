#pragma once

//-----------------------------------------------------------------------------------------------//

#include <string>
#include <map>
#include <boost/tr1/memory.hpp>

//-----------------------------------------------------------------------------------------------//

class GLInventory;

//-----------------------------------------------------------------------------------------------//

struct SACTIVITYSALESITEM
{
    typedef std::map<DWORD,LONGLONG> NPCSELL_PRICE_MAP;

    std::string				             strFILENAME;
    std::string				             strSALETYPE;
    std::tr1::shared_ptr<NPCSELL_PRICE_MAP> pSELLPRICEMAP;
    std::tr1::shared_ptr<GLInventory>		 pITEMS;

    SACTIVITYSALESITEM  ();
    ~SACTIVITYSALESITEM ();

    const bool Load ( const std::string& strFileName );
};
