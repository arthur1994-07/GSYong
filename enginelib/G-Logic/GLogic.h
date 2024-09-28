#pragma once

#include <string>

namespace sc {
    class BaseStream;
    class CBaseString;
}
//class SFileSystem;

namespace GLOGIC
{
	extern BOOL bGBStart;
	extern BOOL bENGLIB_ZIPFILE;
	extern BOOL bGLOGIC_ZIPFILE;
	extern BOOL bGLOGIC_PACKFILE;

	extern std::string	strANIMATION_ZIPFILE;
	extern std::string	strEFFECT_ZIPFILE;
	extern std::string	strEFFECT_CHAR_ZIPFILE;
	extern std::string	strMAP_ZIPFILE;
	extern std::string	strSKINOBJ_ZIPFILE;
	extern std::string	strGLOGIC_SERVER_ZIPFILE;

	//extern SFileSystem sFileSystem;

    void SetGlogicZipFile(BOOL bZipFile);
    BOOL UseLogicZipFile();

    void SetGlogicPackFile(BOOL bPackFile);
    BOOL UseLogicPackFile();

    void SetEngineZipFile(BOOL bZipFile);
    BOOL UseEngineZipFile();

	void SetEngFullPath();
	void OpenPackFile(const char* szAppPath,const char* szRPFDataPath);

	void SetPath(const char* szPath);
	const char* GetPath();

	void SetServerPath(const char* szPath);
	const char* GetServerPath();

	sc::BaseStream* openfile_basestream(
        BOOL bZIP, 
        const std::string& ZipFileName, 
        const char* szFullPath, 
        const std::string& FileName,
        bool bENC=false,
        BOOL bPack=FALSE);

    sc::CBaseString* openfile_basestring(
        BOOL bZIP, 
        const std::string& ZipFileName, 
        const char* szFullPath, 
        const char* szFile, 
        bool bENC=false,
        BOOL bPack=FALSE);
}

