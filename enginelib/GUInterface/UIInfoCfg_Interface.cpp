#include "pch.h"

#include "../../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "UIInfoCfg_Interface.h"
#include "UIInfoCfg.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void NSUIINFOCFG::CONTROL::writeLogError( const std::string& strError )
{
	sc::writeLogError( strError );
}

//-----------------------------------------------------------------------------

IUIInfoCfg::IUIInfoCfg ()
{
    //blank
}

IUIInfoCfg::~IUIInfoCfg ()
{
    //blank
}

IUIInfoCfg* IUIInfoCfg::GetInstance ()
{
    return &CUIInfoCfg::GetInstance ();
}
