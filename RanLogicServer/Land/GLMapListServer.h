#pragma once

#include <hash_map>
#include <string>

#include "../../SigmaCore/Container/MList.h"

#include "../../RanLogic/Land/GLMapList.h"
//#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/GLLevelFile.h"
#include "../../RanLogic/GLogicData.h"

class GLMapListServer : public GLMapList
{
public:
    GLMapListServer();
    virtual ~GLMapListServer();

public:
	HRESULT LoadMapList(
        BOOL bTool = FALSE,
        LPF_SERVER_INFO pServerInfo = NULL,
        int nServerNum = 0, const bool bEmulator = false);

    virtual const char* GetMapName(const SNATIVEID& sNativeID);
    virtual std::string GetMapNameStr(const SNATIVEID& MapID) const;
/*
    HRESULT LoadMapList_OLD(
        const TCHAR* szMapListFile,
        bool bToolMode,
        LPF_SERVER_INFO pServerInfo,
        int nServerNum = 0);
*/
};
