#include "../pch.h"

#include "../../EngineLib/G-Logic/GLDefine.h"

#include "../RanLogic/Crow/GLCrowData.h"

#include "./ActivitySalesItem.h"

//-----------------------------------------------------------------------------------------------//

SACTIVITYSALESITEM::SACTIVITYSALESITEM ()
: pITEMS        ( new GLInventory )
, pSELLPRICEMAP ( new NPCSELL_PRICE_MAP )
{
}

SACTIVITYSALESITEM::~SACTIVITYSALESITEM ()
{
}

const bool SACTIVITYSALESITEM::Load ( const std::string& strFileName )
{
    if ( strFileName.empty() || strFileName == "" )
    {
        return false;
    }

    strFILENAME = strFileName;

    if ( !SaleInvenLoadFile ( strFileName.c_str(), (*pITEMS), strSALETYPE, (*pSELLPRICEMAP) ) )
    {
        return false;
    }

    return true;
}
