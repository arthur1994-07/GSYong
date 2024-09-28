#pragma once

#include <string>

class CGLPetCommentFile
{
public:
    bool LOADFILE( const std::string& strName, bool bToolMode );

public:
    bool LOADFILE_LUA( const std::string& strPath );

private:
    void LoadHelp( const std::string& strTitle, std::string* pList, int nCount );
    bool ParseCommentLua();
};