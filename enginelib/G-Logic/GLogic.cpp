#include "pch.h"
#include "../../SigmaCore/Compress/Unzipper.h"
#include "../../SigmaCore/Compress/ZipMan.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/SFileSystem.h"
#include "../../SigmaCore/File/StringMemory.h"

#include "GLogic.h"
#include "../DxMeshs/SkinMesh/DxSkinAnimationManager.h"
#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"
#include "../DxEffect/Char/DxEffcharData.h"
#include "../DxLand/DxLandMan.h"
#include "../DxMeshs/SkinMesh/DxSkinObject.h"
#include "../Common/SerialMemoryEngine.h"
#include "../../SigmaCore/File/SerialFile.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace GLOGIC
{
	std::string strPATH;
	std::string strPATH_SERVER;

	BOOL bGBStart = FALSE;
	BOOL bENGLIB_ZIPFILE = FALSE;
	BOOL bGLOGIC_ZIPFILE = FALSE;
	BOOL bGLOGIC_PACKFILE = FALSE;

	std::string	strANIMATION_ZIPFILE;
	std::string	strEFFECT_ZIPFILE;
	std::string	strEFFECT_CHAR_ZIPFILE;
	std::string	strMAP_ZIPFILE;
	std::string	strSKINOBJ_ZIPFILE;
	std::string	strGLOGIC_SERVER_ZIPFILE;

	//SFileSystem	sFileSystem;

    void SetGlogicZipFile(BOOL bZipFile)
    {
        bGLOGIC_ZIPFILE = bZipFile;
    }

    BOOL UseLogicZipFile()
    {
        return bGLOGIC_ZIPFILE;
    }

    void SetGlogicPackFile(BOOL bPackFile)
    {
        bGLOGIC_PACKFILE = bPackFile;
    }

    BOOL UseLogicPackFile()
    {
        return bGLOGIC_PACKFILE;
    }

    void SetEngineZipFile(BOOL bZipFile)
    {
        bENGLIB_ZIPFILE = bZipFile;
    }

    BOOL UseEngineZipFile()
    {
        return bENGLIB_ZIPFILE;
    }

	void SetEngFullPath()
	{
		strANIMATION_ZIPFILE = DxSkinAnimationManager::GetInstance().GetPath();
		strANIMATION_ZIPFILE += "Animation.rcc";
		
		strEFFECT_ZIPFILE = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
		strEFFECT_ZIPFILE += "Effect.rcc";

		strEFFECT_CHAR_ZIPFILE = DxEffcharDataMan::GetInstance().GetPath();
		strEFFECT_CHAR_ZIPFILE += "EffectChar.rcc";

		strMAP_ZIPFILE = DxLandMan::GetPath();
		strMAP_ZIPFILE += "Map.rcc";

		strSKINOBJ_ZIPFILE = DxSkinObject::GetPath();
		strSKINOBJ_ZIPFILE += "SkinObject.rcc";
	
		strGLOGIC_SERVER_ZIPFILE = GLOGIC::GetServerPath();
		strGLOGIC_SERVER_ZIPFILE += "GLogicServer.rcc";
/*
		CUnzipper::LOADFILE_RCC( strANIMATION_ZIPFILE );
		CUnzipper::LOADFILE_RCC( strEFFECT_ZIPFILE );
		CUnzipper::LOADFILE_RCC( strEFFECT_CHAR_ZIPFILE );
		CUnzipper::LOADFILE_RCC( strMAP_ZIPFILE );
		CUnzipper::LOADFILE_RCC( strSKINOBJ_ZIPFILE );
		CUnzipper::LOADFILE_RCC( strGLOGIC_SERVER_ZIPFILE );
*/
        ZipMan::GetInstance().LOADFILE_RCC( strANIMATION_ZIPFILE );
        ZipMan::GetInstance().LOADFILE_RCC( strEFFECT_ZIPFILE );
        ZipMan::GetInstance().LOADFILE_RCC( strEFFECT_CHAR_ZIPFILE );
        ZipMan::GetInstance().LOADFILE_RCC( strMAP_ZIPFILE );
        ZipMan::GetInstance().LOADFILE_RCC( strSKINOBJ_ZIPFILE );
        ZipMan::GetInstance().LOADFILE_RCC( strGLOGIC_SERVER_ZIPFILE );
	}

	void OpenPackFile(const char* szAppPath,const char* szRPFDataPath)
	{
		if (UseLogicPackFile())
		{
			std::string strPackDataPath;
			strPackDataPath = szAppPath;
			strPackDataPath += szRPFDataPath;

			// SFileSystem Open
            SetGlogicPackFile(sc::file::g_sFileSystem.OpenFileSystem(strPackDataPath.c_str()));
		}
	}

	void SetPath ( const char* szPath )
	{
		strPATH = szPath;
	}
	
	const char* GetPath ()
	{
		return strPATH.c_str();
	}

	void SetServerPath ( const char* szPath )
	{
		strPATH_SERVER = szPath;
	}

	const char* GetServerPath ()
	{
		return strPATH_SERVER.c_str();
	}

	sc::BaseStream* openfile_basestream(
        BOOL bZIP, 
        const std::string& ZipFileName, 
	    const char* szFullPath, 
        const std::string& FileName, 
		bool bENC,
		BOOL bPack)
	{
		sc::BaseStream *pBStream;
		if (bZIP)
		{
			// ZIP 파일 사용
			SerialMemoryEngine* pMemory = new SerialMemoryEngine;
			if( !pMemory->OpenFile(ZipFileName, FileName, bENC, bPack))
			{
				SAFE_DELETE(pMemory);
				return NULL;
			}
			pBStream = pMemory;
		}
		else
		{
			// 기존 방식으로 사용
			sc::SerialFile* pFile = new sc::SerialFile;
			if (!pFile->OpenFile(FOT_READ, szFullPath, bENC))
			{
				SAFE_DELETE(pFile);
				return NULL;
			}
			pBStream = pFile;
		}		
		return pBStream;
	}

    sc::CBaseString* openfile_basestring(
        BOOL bZIP, 
        const std::string& ZipFileName, 
        const char* szFullPath, 
        const char* szFile, 
        bool bENC,
        BOOL bPack)
	{
        sc::CBaseString *pBString;
		if ( bZIP )
		{
			// note : ZIP 파일 사용
            sc::CStringMemory *pMemory = new sc::CStringMemory;
			if( !pMemory->Open(ZipFileName, szFile, bENC, bPack ) )
			{
				SAFE_DELETE(pMemory);
				return NULL;
			}

			pBString = pMemory;
		}
		else
		{
			// note : 기존 방식으로 사용
			sc::CStringFile *pFile = new sc::CStringFile;
			if ( !pFile->Open( szFullPath, bENC ) )
			{
				SAFE_DELETE(pFile);
				return NULL;
			}

			pBString = pFile;
		}
		
		return pBString;
	}
}

