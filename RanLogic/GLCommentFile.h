#pragma once

#include <string>

class CGLCommentFile
{
public:
    bool LOADFILE( const std::string& strName, bool bToolMode );

public:
	bool LOADFILE_INI( const std::string& strPath, bool bToolMode );

public:
	bool SAVEFILE_LUA( const std::string& strPath );
	bool LOADFILE_LUA( const std::string& strPath );
};