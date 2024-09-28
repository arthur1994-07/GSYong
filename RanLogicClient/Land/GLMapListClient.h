#pragma once

#include "../../RanLogic/Land/GLMapList.h"

struct F_SERVER_INFO;

class GLMapListClient : public GLMapList
{
public:
	HRESULT LoadMapList(
        BOOL bTool = FALSE,
        F_SERVER_INFO* pServerInfo = NULL,
        int nServerNum = 0);

    virtual const char* GetMapName(const SNATIVEID& sNativeID);
    virtual std::string GetMapNameStr(const SNATIVEID& MapID) const;

public:
	GLMapListClient();
	virtual ~GLMapListClient();
};
