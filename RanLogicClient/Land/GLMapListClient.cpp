#include "../pch.h"

#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/basestring.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"


#include "../../RanLogic/s_NetGlobal.h"
#include "./GLMapListClient.h"
//#include "./GLGaeaServer.h"


#include "../../SigmaCore/DebugInclude.h"


GLMapListClient::GLMapListClient()
{
}

GLMapListClient::~GLMapListClient()
{
}

HRESULT GLMapListClient::LoadMapList(
    BOOL bTool,
    F_SERVER_INFO* pServerInfo,
    int nServerNum)
{
    HRESULT hr = GLMapList::LoadMapList( bTool, pServerInfo, nServerNum );
    if ( FAILED(hr) ) 
        return E_FAIL;

	return S_OK;
}

const char* GLMapListClient::GetMapName( const SNATIVEID& sNativeID )
{
	if (!GLMapList::GetMapName(sNativeID))
	{
		static CString strEmpty; // MEMO : 에러시 반환되는 빈 스트링
		return strEmpty;
	}
    return ID2GAMEINTEXT( GLMapList::GetMapName(sNativeID) );
}

std::string GLMapListClient::GetMapNameStr( const SNATIVEID& MapID ) const
{
    
    return ID2GAMEINTEXT( GLMapList::GetMapNameStr(MapID).c_str() );
}
