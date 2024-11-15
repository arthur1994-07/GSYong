#include "pch.h"
#include "Archive.h"
#include "direct.h"
#include "shlwapi.h"

#include <vector>

#include "../G-Logic/GLogic.h"
#include "../DxLand/DxSetLandMan.h"
#include "../DxLand/DxLandMan.h"
#include "../DxLand/DxPiece/DxPieceEdit.h"
#include "../DxLand/DxPiece/DxPieceContainer.h"
#include "../DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../DxMeshs/SkinMesh/_new/DxSkinMesh9_NORMAL.h"
#include "../DxMeshs/SkinMesh/DxVehicleData.h"
#include "../DxMeshs/FrameMesh/DxReplaceContainer.h"
#include "../DxTools/PrivateResource.h"
#include "../DxTools/Material/NSMaterialManager.h"
#include "../DxEffect/Single/DxEffSinglePropGMan.h"

#include "../../SigmaCore/Util/Util.h"
#include "../../SigmaCore/File/FileFind.h"
#include "../../SigmaCore/File/SerialArchiveFile.h"
#include "../../SigmaCore/String/StringUtils.h"

using namespace	Archive;

BOOL						g_bImportMobile(FALSE);

static LPDIRECT3DDEVICEQ    s_pDevice   = 0;
static const TCHAR*         s_pData     = 0;
static const TCHAR*         s_pArchive  = 0;
static SPFileFindTree       s_spFileFinder;

/**
    제귀적으로 pToken 으로 부터 폴더를 추출해 strDest 에 폴더를 생성 한다.

 */
static void RecursiveMakeFolder(const TCHAR* pToken, TSTRING& strDest)
{
    if (pToken != 0)
    {
        strDest = strDest + _T("\\") + pToken;

        if (_tcschr(pToken, _T('.')) == 0)
        {
            _tmkdir(strDest.c_str());
            RecursiveMakeFolder(_tcstok(0, _T("\\")), strDest);
        }
    }
}

/**
	p1 에서 p2 의 문자열을 찾아 비교하여 달라지는 인덱스를 찾는다.
    대소문자는 구분하지 않는다.

    ex) p1 : ABCDEFG, p2 : CDJGH, 결과 : 4
        ABCDEFG 에서 CDJGH 와 같은 부분 CD를 찾고 달라지는 E 의 인덱스 4를 리턴

	\param p1 찾을 대상 문자열
	\param p2 찾을 문자열
	\return 달라지는 인덱스, 찾을 문자열이 아예 없다면 0
 */
static size_t FindEndString( TCHAR* p1, TCHAR* p2)
{  
    size_t i = 0, j = 0;    

    while (p1[i])
    {
        if (_totlower(p1[i++]) == _totlower(p2[j]))
        {
            while (p2[j++])
            {
                if ( p1[i] == 0 || _totlower(p1[i++]) != _totlower(p2[j]) )
                {
                    return i;
                }
            }
        }
    }

    return i;
}

/**
	추출 폴더로 부터 기준 폴더로 부터 달라지는 폴더를
    생성 폴더에 생성 한다.
    
    pSrc : c:\data\texture
    pPivot : c:\data
    strDest : c:\temp

    결과물 : c:\temp\texture

    데이터를 동일한 폴더구조로 임의의 폴더에 복사하고 싶을 떄
    폴더의 생성 목적

	\param pSrc 추출 폴더
	\param pPivot 기준 폴더
	\param strDest 생성 폴더, 생성되는 폴더를 삽입 해 준다.
 */
static void MakeFolder(TCHAR* pSrc, TCHAR* pPivot, TSTRING& strDest)
{
	size_t n = FindEndString(pSrc, pPivot);

    if (n != 0)
    {
        RecursiveMakeFolder(_tcstok(pSrc + n, _T("\\")), strDest);
    }
}

/**
 	파일 경로로 부터 확장자만 변경한다.

*/
static void PathToChangeExt(const TCHAR* pStr, const TCHAR* pExt, TCHAR* pDest, int nDestSize)
{
    TCHAR temp[4][MAX_PATH] = {0};
    _tsplitpath_s(pStr, temp[0], MAX_PATH, temp[1], MAX_PATH, temp[2], MAX_PATH, temp[3], MAX_PATH);
    _tmakepath_s(pDest, nDestSize, temp[0], temp[1], temp[2], pExt);
}

/**
 	파일 확장자가 동일한지 체크 한다.
	
 */
static bool IsSameExt(const TCHAR* pFile, const TCHAR* pExt)
{
    return sc::string::getFileExt(pFile).CompareNoCase(pExt) == 0;
}

/**
    입력되는 파일명을 점검 한다.
    입력되는 파일명에 추가적으로 임의의 체크를 수행할 경우 사용 된다.
    현재는 암호화된 파일명을 출력하는 용도로 사용 된다.

 */
static bool InspectExt(const TCHAR* pFile, TCHAR* pOut, int nMaxString)
{
    if ( IsSameExt(pFile, _T("x")) )
    {
        // 암호화된 x 파일
        PathToChangeExt(pFile, _T("mxf"), pOut, nMaxString);
    }
    else if ( IsSameExt(pFile, _T("dds")) )
    {
        // 암호회된 텍스처
        PathToChangeExt(pFile, _T("mtf"), pOut, nMaxString);
    }
    else
    {
        return false;
    }

    return true;
}

/**
    파일을 복사 한다.
	기존 CopyFile 의 실제 복사부분.

	\param pFile 복사할 순수 파일명
	\param strOutput 복사할 폴더
	\param pPostCallback 복사를 수행한 후 호출할 콜백 함수
	\param strStack 복사를 수행한 파일들, 한번 복사한 파일을 다시 복사하지 않게 하기 위한 용도로 사용 된다.
	\return 복사 성공 유무
 */
static bool CopyFileCore(const TCHAR* pFile, TSTRING strOutput, boost::function2<void, const TCHAR*, StrContainer> pPostCallback, StrContainer& strStack)
{
	TSTRING strTemp = pFile;

	if (strStack.find(strTemp) != strStack.end())
	{
		return false;
	}

	TCHAR szTempPath[MAX_PATH] = {0}, szAppPath[MAX_PATH] = {0};
	_tcsncpy(szTempPath, strTemp.c_str(), MAX_PATH);
	_tcsncpy(szAppPath, s_pData, MAX_PATH);

	BOOL bSuccess(FALSE);

	if ( g_bImportMobile )
	{
		std::string strFullName(pFile);

		size_t pos;
		while ((pos = strFullName.find_first_of("\\")) != std::string::npos)
		{
			strFullName.replace(pos, 1, "/");
		}

		size_t p = strFullName.find_last_of("/");
		std::string workdir;
		if (p != strFullName.npos)
		{
			workdir = strFullName.substr( p+1 );
		}

		// 소문자 변환.
		std::transform( workdir.begin(), workdir.end(), workdir.begin(), tolower);

		CString strExt = STRUTIL::GetExtName( workdir.c_str() );
		if ( strExt == ".x" || strExt == ".mxf" )
		{
			// 복사 안해도 됨.
		}
		else
		{
			strOutput += "/";
			strOutput += workdir;
			bSuccess = ::CopyFile(strTemp.c_str(), strOutput.c_str(), FALSE);
		}
	}
	else
	{
		MakeFolder(szTempPath, szAppPath, strOutput);
		bSuccess = ::CopyFile(strTemp.c_str(), strOutput.c_str(), FALSE);
	}

	strStack.insert(strTemp);

	pPostCallback(strTemp.c_str(), strStack);

	return bSuccess;
}

/**
    파일을 복사 한다.

	\param pFile 복사할 순수 파일명
	\param strOutput 복사할 폴더
	\param pPostCallback 복사를 수행한 후 호출할 콜백 함수
	\param strStack 복사를 수행한 파일들, 한번 복사한 파일을 다시 복사하지 않게 하기 위한 용도로 사용 된다.
	\return 복사 성공 유무
 */
static bool CopyFile(const TCHAR* pFile, TSTRING strOutput, boost::function2<void, const TCHAR*, StrContainer> pPostCallback, StrContainer& strStack)
{
    TSTRING strTemp = s_spFileFinder->FindPathName(pFile);

    if (strTemp.empty())
    {
        TCHAR temp[MAX_PATH] = {0};

        if (InspectExt(pFile, temp, MAX_PATH))
        {
			// 확장자를 변환해서 해본다.
            if ( CopyFile(temp, strOutput, pPostCallback, strStack) )
			{
				return true;
			}
			else
			{
				// 확장자를 변환해도 없다면 이곳으로 와서 기본 이름으로 검사해본다.
				// 경로가 있을 때만 사용가능하다.
				TSTRING strlower(s_pData);
				sc::string::lowerString( strlower );
				std::string::size_type idx = TSTRING(pFile).find( strlower );
				if ( idx != std::string::npos )
				{
					// 경로 그대로 사용하는 것이 필요하다.
					return CopyFileCore( pFile, strOutput, pPostCallback, strStack );
				}
			}
        }
		else 
		{
			// 확장자를 변환해서 있다면 작동이 된다.
			// 경로가 있을 때만 사용가능하다.
			TSTRING strlower(s_pData);
			sc::string::lowerString( strlower );
			std::string::size_type idx = TSTRING(pFile).find( strlower );
			if ( idx != std::string::npos )
			{
				// 경로 그대로 사용하는 것이 필요하다.
				return CopyFileCore( pFile, strOutput, pPostCallback, strStack );
			}
		}
 
        return false;
    }

	// s_spFileFinder 에서 얻어와서 사용한다.
	return CopyFileCore( strTemp.c_str(), strOutput, pPostCallback, strStack );
}

/** 
    아카이브 후 처리를 수행 한다.
    아카이브 한 파일에 따라 추가로 처리해야 할 일을 수행 한다.
    우리는 확장자에 맞춰 아카이브를 추가적으로 수행 해 준다.    

    \param pArchive 이미 아카이브 한 파일
    \param strStack 읽은 파일들, 재귀적으로 접근을 막기 위함
 */    
static void PostArchive(const TCHAR* pArchive, StrContainer& strStack)
{
    boost::function<void (const TCHAR*)> fun = boost::bind(CopyFile, _1, s_pArchive, PostArchive, strStack);

    if ( IsSameExt(pArchive, _T("chf")) )
    {
        // 캐릭터
        DxSkinCharData kTemp;
        kTemp.LoadFile(pArchive, s_pDevice, false, fun);
    }
    else if ( IsSameExt(pArchive, _T("cps")) || 
              IsSameExt(pArchive, _T("vps")) || 
              IsSameExt(pArchive, _T("aps")) )
    {
        // 조각
        DxSkinPiece kTemp;
        kTemp.LoadPiece(pArchive, s_pDevice, FALSE, FALSE, fun);
    }
    else if ( IsSameExt(pArchive, _T("cfg")) )
    {
        // 애니매이션
        DxSkinAnimation kTemp;
        kTemp.LoadFile(pArchive, s_pDevice, fun);

        // 구버전 애니는 수동으로 파일명을 변경 하므로 강제로 로딩 시켜야 한다.
        DxAnimationList kAniTemp;
        DxSkeleton kSkeTemp;
        kAniTemp.LoadAnimList_BIN(kTemp.m_strBinFile, &kSkeTemp, fun);
    }
    else if ( IsSameExt(pArchive, _T("egp")) )
    {
        // 이펙터
        EFF_PROPGROUP kTemp;
        kTemp.LoadFile(pArchive, s_pDevice, fun);
    }
    else if ( IsSameExt(pArchive, _T("pie")) || IsSameExt(pArchive, _T("pie15")) )
    {
        // 지형 조각
        DxPieceEdit kTemp;
        TSTRING strFileName;
        sc::string::getFullFileName(pArchive, strFileName);
        kTemp.LoadPIE(s_pDevice, pArchive, DxPieceContainer::GetInstance().GetPieceEditPath(), strFileName.c_str(), fun);
    }
    else if ( IsSameExt(pArchive, _T("pis")) )
    {
        // 구버전 지형 조각
        DXREPLACEPIECE kTemp;
        TSTRING strFileName;
        sc::string::getFullFileName(pArchive, strFileName);
        kTemp.Load(strFileName.c_str(), s_pDevice, fun);
    }
    else if ( IsSameExt(pArchive, _T("abf")) )
    {
        // 어태치 본
        DxAttBoneData kTemp;
        kTemp.LoadFile(pArchive, s_pDevice, 0, 0, fun);

        // 애니 추출 문제로 로딩한 걸로 다시 추출 한다.
        Archive::Archive(&kTemp);
    }
	else if ( IsSameExt(pArchive, _T("vcf")) )
	{
		// Vehicle
		DxVehicleData kTemp;
		kTemp.LoadFile(pArchive, s_pDevice, 0, fun);

		// 애니 추출 문제로 로딩한 걸로 다시 추출 한다.
		Archive::Archive(&kTemp);
	}
    else if ( IsSameExt(pArchive, _T("x")) ||
              IsSameExt(pArchive, _T("mxf")) )
    {
        DxSkinMesh9_NORMAL kTemp;
        LPD3DXFRAME	pFrameRoot = 0;
        ID3DXAnimationController* pAnimController = 0;

        CAllocateHierarchy Alloc(&kTemp,pArchive,FALSE);

        if ( IsSameExt(pArchive, _T("mxf")) )
        {
            // mxf 는 암호화를 푼다.
            CPrivateResource cResource(CPrivateResource::ETypeSkin);

            if (cResource.LoadFileDec(pArchive) == false)
            {
                return;
            }

            D3DXLoadMeshHierarchyFromXInMemory(cResource.GetData(), cResource.GetSize(), D3DXMESH_MANAGED, s_pDevice, &Alloc, NULL, &pFrameRoot, &pAnimController);
        }
        else
        {
            D3DXLoadMeshHierarchyFromX(pArchive, D3DXMESH_MANAGED, s_pDevice, &Alloc, NULL, &pFrameRoot, &pAnimController);
        }

        SMeshContainer* pSkinCnt = kTemp.GetMeshClobalList();

        while (pSkinCnt)
        {
            if (pSkinCnt->m_pMaterialEx)
            {
                // x 파일에서 텍스처 추출
                fun(pSkinCnt->m_pMaterialEx->m_textureRes.GetFileName().c_str());
            }

            pSkinCnt = pSkinCnt->m_pGlobalNext;
        }

		if ( g_bImportMobile )
		{
			const TCHAR* pFile = pArchive;
			if ( pFile )
			{
				CString strNewName;
				STRUTIL::ChangeExt( pFile, strNewName, _T(".mobj") );

				// x 파일에서 텍스처 추출
				fun( strNewName.GetString() );
			}
		}

        if (pFrameRoot)
        {
            CAllocateHierarchy Alloc(NULL,pArchive,FALSE);
            D3DXFrameDestroy(pFrameRoot, &Alloc);
            pFrameRoot = NULL;
        }

        SAFE_RELEASE(pAnimController);
    }
	else if ( IsSameExt(pArchive, _T("mobj")) )
	{
	}
}

void Archive::PreArchive(LPDIRECT3DDEVICEQ pDevice, const TCHAR* pData, const TCHAR* pArchive)
{
    s_pDevice = pDevice;
    s_pData = pData;
    s_pArchive = pArchive;

    s_spFileFinder.reset(new sc::CFileFindTree);
    s_spFileFinder->CreateTree(pData);
}

void Archive::Archive(DxSetLandMan* pSetLandMan, const TCHAR* pPSFFile)
{
    if (pSetLandMan)
    {
        StrContainer strStack;

        pSetLandMan->SaveSet(s_pDevice, pPSFFile, boost::bind(CopyFile, _1, s_pArchive, PostArchive, strStack));
    }
}

void Archive::Archive(DxLandMan* pLandMan)
{
    if (pLandMan)
    {
        CString kFileName;
        StrContainer strStack;
        sc::SerialFile& sFile = pLandMan->GetSerialFile();

        const TCHAR* pFullPath = sFile.GetFileName();

        if (pFullPath && _tcsclen(pFullPath))
        {
            CString kPath, kExt;
            sc::string::getFilePathAndName(pFullPath, kPath, kFileName, kExt);
        }

        pLandMan->SaveFile(s_pDevice, kFileName.GetBuffer(), boost::bind(CopyFile, _1, s_pArchive, PostArchive, strStack));
    }
}

void Archive::Archive(DxSkinCharData* pSkinData)
{
    if (pSkinData)
    {
        StrContainer strStack;

        const TCHAR* pFileName = pSkinData->GetFileName().c_str();

        pSkinData->SaveFile(pFileName, s_pDevice, boost::bind(CopyFile, _1, s_pArchive, PostArchive, strStack));
    }
}

void Archive::Archive(DxSkinPiece* pSkinPiece)
{
    if (pSkinPiece)
    {
        StrContainer strStack;

        const TCHAR* pFileName = pSkinPiece->GetFileName();

        pSkinPiece->SavePiece(pFileName, TRUE, boost::bind(CopyFile, _1, s_pArchive, PostArchive, strStack));
    }
}

void Archive::Archive(DxVehicleData* pVehicleData)
{
    if (pVehicleData)
    {
        StrContainer strStack;

        const TCHAR* pFileName = pVehicleData->GetFileName();

        pVehicleData->SaveFile(pFileName, s_pDevice, boost::bind(CopyFile, _1, s_pArchive, PostArchive, strStack));
    }
}

void Archive::Archive(DxAttBoneData* pAttData)
{
    if (pAttData)
    {
        StrContainer strStack;

        const TCHAR* pFileName = pAttData->GetFileName();

        pAttData->SaveFile(pFileName, s_pDevice, boost::bind(CopyFile, _1, s_pArchive, PostArchive, strStack));
    }
}

void Archive::Archive(DxAttBoneLink* pAttBoneLink)
{
    if (pAttBoneLink)
    {
        StrContainer strStack;

        const TCHAR* pFileName = pAttBoneLink->GetFileName();

        pAttBoneLink->SaveABLFile(pFileName, s_pDevice, boost::bind(CopyFile, _1, s_pArchive, PostArchive, strStack));
    }
}

void Archive::Archive(EFF_PROPGROUP* pEffGroup)
{
	if (pEffGroup)
	{
		StrContainer strStack;

		const TCHAR* pFileName = pEffGroup->m_strFileName.c_str();

		pEffGroup->SaveFile(pFileName, boost::bind(CopyFile, _1, s_pArchive, PostArchive, strStack));
	}
}

bool Archive::CreateFileBaseStream(const TCHAR* PathName, const TCHAR* pFileName, const TCHAR* pZipFile, boost::function<void (const TCHAR*)> pArchive, SPBaseStream& spBase)
{
    if (pArchive)
    {
        spBase.reset(new sc::SerialArchiveFile(pArchive, pFileName));
    }
    else
    {
        spBase.reset(GLOGIC::openfile_basestream(GLOGIC::UseEngineZipFile(), pZipFile, PathName, pFileName));
    }

    if (spBase.get() == 0)
    {
        return false;
    }

    return true;
}

bool Archive::CreateSerialFile(boost::function<void (const TCHAR*)> pArchive, SPSerialFile& spFile)
{
    if (pArchive)
    {
        spFile.reset(new sc::SerialArchiveFile(pArchive));
    }
    else
    {
        spFile.reset(new sc::SerialFile());
    }

    if (spFile.get() == 0)
    {
        return false;
    }

    return true;
}
