#include "pch.h"

#include <process.h>
#include <algorithm>
#include <set>

#include "../../SigmaCore/File/FileFind.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/EBTime.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../BugTrap/BugTrap.h"
#include "../DxMeshs/SkinMesh/_new/NsSMeshSceneGraph.h"

#include "./Material/NSMaterialManager.h"
#include "./NsLoadingLagTester.h"
#include "./DebugSet.h"
#include "./PrivateResource.h"
#include "./RENDERPARAM.h"

#include "TextureManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    static BOOL g_bLOG_ONLY_MIPMAP = FALSE;
}

//////////////////////////////////////////////////////////////////////////
void CompletedTextureData::DataExchange(CompletedTextureData& rhs)
{
	if (this != &rhs)
	{
		std::swap(m_pD3DBaseTexture, rhs.m_pD3DBaseTexture);
		std::swap(m_strName, rhs.m_strName);
		std::swap(m_addData, rhs.m_addData);
		std::swap(m_imageInfo, rhs.m_imageInfo);
		std::swap(m_emTextureAlphaType, rhs.m_emTextureAlphaType);
		std::swap(m_textureResizeType, rhs.m_textureResizeType);
		std::swap(m_emCategory, rhs.m_emCategory);
		std::swap(m_bBackUpCache, rhs.m_bBackUpCache);
		std::swap(m_bLogWrite, rhs.m_bLogWrite);
	}
}

//////////////////////////////////////////////////////////////////////////
LPDIRECT3DTEXTUREQ CompletedTextureData::GetTexture() 
{
	//MIN_ASSERT(m_addData.m_type == TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D);
	//MIN_ASSERT(m_pD3DBaseTexture);

	if( (m_addData.m_type != TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D) && !m_bLogWrite )
	{
		m_bLogWrite = TRUE;
		TSTRINGSTREAM strStringStream;
		strStringStream << "m_addData.m_type == TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D";
		strStringStream << " - m_emCategory - ";
		strStringStream << m_emCategory;
		strStringStream << " - Name - ";
		strStringStream << m_strName;
		CDebugSet::ToLogFile( std::string( strStringStream.str().c_str() ) );
	}
	if ( !m_pD3DBaseTexture )
	{
		if ( !m_bLogWrite )
		{
			m_bLogWrite = TRUE;
			CDebugSet::ToLogFile( std::string("!m_pD3DBaseTexture <TEXUTRE_TYPE_2D>") );
		}
		return NULL;
	}

	return static_cast<LPDIRECT3DTEXTUREQ>(m_pD3DBaseTexture); 
}

LPDIRECT3DCUBETEXTUREQ CompletedTextureData::GetCubeTexture() 
{
	//MIN_ASSERT(m_addData.m_type == TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE);
	//MIN_ASSERT(m_pD3DBaseTexture);

	if( (m_addData.m_type != TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE) && !m_bLogWrite )
	{
		m_bLogWrite = TRUE;
		TSTRINGSTREAM strStringStream;
		strStringStream << "m_addData.m_type == TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE";
		strStringStream << " - m_emCategory - ";
		strStringStream << m_emCategory;
		strStringStream << " - Name - ";
		strStringStream << m_strName;
		CDebugSet::ToLogFile( std::string( strStringStream.str().c_str() ) );
	}
	if ( !m_pD3DBaseTexture )
	{
		if ( !m_bLogWrite )
		{
			m_bLogWrite = TRUE;
			CDebugSet::ToLogFile( std::string("!m_pD3DBaseTexture <TEXUTRE_TYPE_CUBE>") );
		}
		return NULL;
	}

	return static_cast<LPDIRECT3DCUBETEXTUREQ>(m_pD3DBaseTexture); 
}

//-------------------------------------------------------
// TextureManager
TextureManager& TextureManager::GetInstance()
{
	static TextureManager TM;
	return TM;
}

TextureManager::TextureManager()
{
	m_pD3DDevice = NULL; m_bForcelessThan1024 = false; m_bTextureAlphaTypeDebug = false;
	m_bEnableLog = true;
	m_deleteCacheTime = 1000 * 60 * 5;
}

void TextureManager::SetPath( const TSTRING& path )
{
	m_fileTree.CreateTree(path);
}

void TextureManager::RescanPath()
{
	m_fileTree.CreateTree(m_fileTree.GetPath());
}

// TAG_UpdateResourceManager_4ca
void TextureManager::ProcessCompleteData( 
	SP_QUEUE_DATA_VECTOR& dataVec,
	NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
	DWORD dwPrevTick, 
	DWORD nWorkTime )
{
	SP_QUEUE_DATA_VECTOR::iterator it = dataVec.begin();
    while( it != dataVec.end() )
    {
		DWORD dwTick( timeGetTime() );

        const SP_QUEUE_DATA& spResData = *it;
        ++it;

		MIN_ASSERT(m_pD3DDevice);
		MIN_ASSERT(!spResData->m_fileName.empty());
		MIN_ASSERT(spResData->m_spRawData != NULL);

		TSTRING fileName = spResData->m_fileName; 
		SP_RAW_DATA& rawData = spResData->m_spRawData;
		UINT rawDataSize = spResData->m_rawDataSize;
		const TEXTURE_ADD_DATA& addData = spResData->m_addData;

		HRESULT hr = S_OK;
		D3DXIMAGE_INFO imageInfo;
		IDirect3DBaseTexture9* pD3DBaseTexture = NULL;
		SP_COMPLETED_DATA spD3DTexture;

		D3DSURFACE_DESC d3dSurfaceDesc;
		d3dSurfaceDesc.Format = D3DFMT_UNKNOWN;

		TEXTURE_ALPHA emType = EMTT_NORMAL;
		//////////////////////////////////////////////////////////////////////////
		// 큐브 텍스쳐
		if( addData.m_type == TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE )
		{
			LPDIRECT3DCUBETEXTUREQ pD3DCubeTexture = NULL;
			hr = D3DXCreateCubeTextureFromFileInMemoryEx(
				m_pD3DDevice,
				rawData.get(),
				rawDataSize,
				D3DX_DEFAULT,
				addData.m_mipMap,
				0,
				(D3DFORMAT) addData.m_d3dFormat,
				D3DPOOL_MANAGED,
				D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
				D3DX_FILTER_BOX|D3DX_FILTER_MIRROR,
				0,
				&imageInfo,
				NULL,
				&pD3DCubeTexture );
			if ( FAILED(hr) )
			{
				hr = D3DXCreateCubeTextureFromFileInMemoryEx(
					m_pD3DDevice,
					rawData.get(),
					rawDataSize,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					0,
					(D3DFORMAT) addData.m_d3dFormat,
					D3DPOOL_MANAGED,
					D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
					D3DX_FILTER_BOX|D3DX_FILTER_MIRROR,
					0,
					&imageInfo,
					NULL,
					&pD3DCubeTexture );
				if ( FAILED(hr) )
				{
                    CDebugSet::ToLogFile( std::string( _T( "Failed D3DXCreateCubeTextureFromFileInMemoryEx" ) ) );
					break;
				}
			}

			if ( imageInfo.ResourceType != D3DRTYPE_CUBETEXTURE )
			{
				std::string strTemp;
				strTemp = " D3DRTYPE_CUBETEXTURE != (src)";

				switch ( imageInfo.ResourceType )
				{
				case D3DRTYPE_SURFACE:
					strTemp += "D3DRTYPE_SURFACE";
					break;
				case D3DRTYPE_VOLUME:
					strTemp += "D3DRTYPE_VOLUME";
					break;
				case D3DRTYPE_TEXTURE:
					strTemp += "D3DRTYPE_TEXTURE";
					break;
				case D3DRTYPE_VOLUMETEXTURE:
					strTemp += "D3DRTYPE_VOLUMETEXTURE";
					break;
				case D3DRTYPE_CUBETEXTURE:
					strTemp += "D3DRTYPE_CUBETEXTURE";
					break;
				case D3DRTYPE_VERTEXBUFFER:
					strTemp += "D3DRTYPE_VERTEXBUFFER";
					break;
				case D3DRTYPE_INDEXBUFFER:
					strTemp += "D3DRTYPE_INDEXBUFFER";
					break;
				default:
					break;
				}

				strTemp += " - ";
				strTemp += fileName;

				CDebugSet::ToLogFile( strTemp );

				//SAFE_RELEASE( pD3DCubeTexture );
				//break;
			}

			pD3DCubeTexture->GetLevelDesc( 0, &d3dSurfaceDesc );
			pD3DBaseTexture = pD3DCubeTexture;
		}
		else if( addData.m_type == TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D)
		{
			BoxSize boxSize = GetForceLessSize(fileName,&imageInfo,addData.m_resizeType,rawData.get(),rawDataSize);

			if ( addData.m_emCategory == TEXTURE_CHAR || addData.m_emCategory == TEXTURE_LAND ||
				addData.m_emCategory == TEXTURE_VISUALMATERIAL || addData.m_emCategory == TEXTURE_MATERIALS )
			{
				if ( RENDERPARAM::g_emTextureQuality == TEXTURE_QUALITY_MID )
				{
					boxSize.width /= 2;
					boxSize.height /= 2;
				}
				else if( RENDERPARAM::g_emTextureQuality == TEXTURE_QUALITY_LOW )
				{
					boxSize.width /= 4;
					boxSize.height /= 4;
				}
			}

			LPDIRECT3DTEXTUREQ pD3DTexture = NULL;
			hr = D3DXCreateTextureFromFileInMemoryEx(
				m_pD3DDevice,
				rawData.get(),
				rawDataSize,
				boxSize.width,
				boxSize.height,
				addData.m_mipMap,
				0,
				(D3DFORMAT)addData.m_d3dFormat,
				D3DPOOL_MANAGED,
				D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
				D3DX_FILTER_BOX|D3DX_FILTER_MIRROR,
				0,
				&imageInfo,
				NULL,
				&pD3DTexture );
			if ( FAILED(hr) )
			{
				hr = D3DXCreateTextureFromFileInMemoryEx(
					m_pD3DDevice,
					rawData.get(),
					rawDataSize,
					boxSize.width,
					boxSize.height,
					D3DX_DEFAULT,
					0,
					(D3DFORMAT)addData.m_d3dFormat,
					D3DPOOL_MANAGED,
					D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
					D3DX_FILTER_BOX|D3DX_FILTER_MIRROR,
					0,
					&imageInfo,
					NULL,
					&pD3DTexture );
				if ( FAILED(hr) )
				{
					CDebugSet::ToLogFile( std::string( _T( "Failed D3DXCreateTextureFromFileInMemoryEx" ) ) );
					break;
				}				
			}

			if ( imageInfo.ResourceType != D3DRTYPE_TEXTURE )
			{
				std::string strTemp;
				strTemp = " D3DRTYPE_TEXTURE != (src)";

				switch ( imageInfo.ResourceType )
				{
				case D3DRTYPE_SURFACE:
					strTemp += "D3DRTYPE_SURFACE";
					break;
				case D3DRTYPE_VOLUME:
					strTemp += "D3DRTYPE_VOLUME";
					break;
				case D3DRTYPE_TEXTURE:
					strTemp += "D3DRTYPE_TEXTURE";
					break;
				case D3DRTYPE_VOLUMETEXTURE:
					strTemp += "D3DRTYPE_VOLUMETEXTURE";
					break;
				case D3DRTYPE_CUBETEXTURE:
					strTemp += "D3DRTYPE_CUBETEXTURE";
					break;
				case D3DRTYPE_VERTEXBUFFER:
					strTemp += "D3DRTYPE_VERTEXBUFFER";
					break;
				case D3DRTYPE_INDEXBUFFER:
					strTemp += "D3DRTYPE_INDEXBUFFER";
					break;
				default:
					break;
				}

				strTemp += " - ";
				strTemp += fileName;

				CDebugSet::ToLogFile( strTemp );

				//SAFE_RELEASE( pD3DTexture );
				//break;
			}

			pD3DTexture->GetLevelDesc( 0, &d3dSurfaceDesc );
			pD3DBaseTexture = pD3DTexture;
		}
		else
			MIN_ASSERT(false);

		size_t idx;
		switch ( d3dSurfaceDesc.Format )
		{
		case D3DFMT_DXT1:
			idx = fileName.find( _T("_a.") );
			if( idx!=TSTRING::npos )	
			{
				emType = EMTT_ALPHA_HARD;
				break;
			}
			idx = fileName.find( _T("_a1.") );
			if( idx!=TSTRING::npos )
			{
				emType = EMTT_ALPHA_HARD;
				break;
			}
			idx = fileName.find( _T("_a2.") );
			if( idx!=TSTRING::npos )
			{
				emType = EMTT_ALPHA_HARD;
				break;
			}

			emType = EMTT_NORMAL;
			break;

		case D3DFMT_DXT5:
			emType = EMTT_ALPHA_SOFT;

			idx = fileName.find( _T("_a.") );
			if( idx!=TSTRING::npos )	
			{
				emType = EMTT_ALPHA_SOFT;
				break;
			}
			idx = fileName.find( _T("_a1.") );
			if( idx!=TSTRING::npos )
			{
				emType = EMTT_ALPHA_SOFT01;
				break;
			}
			idx = fileName.find( _T("_a2.") );
			if( idx!=TSTRING::npos )
			{
				emType = EMTT_ALPHA_SOFT02;
				break;
			}
			idx = fileName.find( _T("_v.") );
			if( idx!=TSTRING::npos )
			{
				emType = EMTT_NORMAL;
				break;
			}
			break;

		case D3DFMT_DXT2:
		case D3DFMT_DXT3:
		case D3DFMT_DXT4:
		case D3DFMT_A8R8G8B8:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
		case D3DFMT_A8R3G3B2:
		case D3DFMT_A8:
			emType = EMTT_ALPHA_HARD;

			idx = fileName.find( _T("_v.") );
			if( idx!=TSTRING::npos )
			{
				emType = EMTT_NORMAL;
				break;
			}
			break;

		case D3DFMT_X8R8G8B8:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_X4R4G4B4:
		case D3DFMT_R8G8B8:
		case D3DFMT_R5G6B5:
		case D3DFMT_R3G3B2:
		default:
			emType = EMTT_NORMAL;
			break;
		};

   //     // ALPHA_HARD 라면 MIPMAP 을 1 짜리로 생성한다.
   //     if( emType == EMTT_ALPHA_HARD &&
   //         addData.m_type == TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D )
   //     {
   //         SAFE_RELEASE( pD3DBaseTexture );

   //         BoxSize boxSize = GetForceLessSize(fileName,&imageInfo,addData.m_resizeType,rawData.get(),rawDataSize);

			//LPDIRECT3DTEXTUREQ pD3DTexture = NULL;
			//hr = D3DXCreateTextureFromFileInMemoryEx(
			//	m_pD3DDevice,
			//	rawData.get(),
			//	rawDataSize,
			//	boxSize.width,
			//	boxSize.height,
			//	1,
			//	0,
			//	(D3DFORMAT)addData.m_d3dFormat,
			//	D3DPOOL_MANAGED,
			//	D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
			//	D3DX_FILTER_BOX|D3DX_FILTER_MIRROR,
			//	0,
			//	&imageInfo,
			//	NULL,
			//	&pD3DTexture );
			//if ( FAILED(hr) )
			//{
			//	hr = D3DXCreateTextureFromFileInMemoryEx(
			//		m_pD3DDevice,
			//		rawData.get(),
			//		rawDataSize,
			//		boxSize.width,
			//		boxSize.height,
			//		D3DX_DEFAULT,
			//		0,
			//		(D3DFORMAT)addData.m_d3dFormat,
			//		D3DPOOL_MANAGED,
			//		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
			//		D3DX_FILTER_BOX|D3DX_FILTER_MIRROR,
			//		0,
			//		&imageInfo,
			//		NULL,
			//		&pD3DTexture );
			//	if ( FAILED(hr) )
			//	{
			//		CDebugSet::ToLogFile( std::string( _T( "Failed D3DXCreateTextureFromFileInMemoryEx" ) ) );
			//		break;
			//	}				
			//}

			//pD3DTexture->GetLevelDesc( 0, &d3dSurfaceDesc );
			//pD3DBaseTexture = pD3DTexture;
   //     }

		//////////////////////////////////////////////////////////////////////////
		// 드디어 택스쳐 정보 완성

		pCompletedDataVec->push_back(
			NAME_N_SP_COMPLETED_DATA(
			    fileName,
			    SP_COMPLETED_DATA(
				    new CompletedTextureData(
                        pD3DBaseTexture,
						fileName,
                        addData,
                        imageInfo,
                        emType,
                        addData.m_resizeType,
                        addData.m_emCategory ) ) ) );

        TextureMemory::OnCreate( pD3DBaseTexture, addData.m_emCategory );

		//////////////////////////////////////////////////////////////////////////

		TSTRING formatName;
		if ( m_bTextureAlphaTypeDebug && !g_bLOG_ONLY_MIPMAP )
		{
			switch ( d3dSurfaceDesc.Format )
			{
				// 데이터 정리를 위한 것
			case D3DFMT_DXT1:
			case D3DFMT_DXT2:
			case D3DFMT_DXT3:
			case D3DFMT_DXT4:
			case D3DFMT_DXT5:
				break;

				//////////////////////////////////////////////////////////////////////////
			case D3DFMT_A8R8G8B8:
				formatName = _T("D3DFMT_A8R8G8B8");
				break;
			case D3DFMT_A1R5G5B5:
				formatName = _T("D3DFMT_A1R5G5B5");
				break;
			case D3DFMT_A4R4G4B4:
				formatName = _T("D3DFMT_A4R4G4B4");
				break;
			case D3DFMT_A8R3G3B2:
				formatName = _T("D3DFMT_A8R3G3B2");
				break;
			case D3DFMT_A8:
				formatName = _T("D3DFMT_A8");
				break;
			case D3DFMT_X8R8G8B8:
				formatName = _T("D3DFMT_X8R8G8B8");
				break;
			case D3DFMT_X1R5G5B5:
				formatName = _T("D3DFMT_X1R5G5B5");
				break;
			case D3DFMT_X4R4G4B4:
				formatName = _T("D3DFMT_X4R4G4B4");
				break;
			case D3DFMT_R8G8B8:
				formatName = _T("D3DFMT_R8G8B8");
				break;
			case D3DFMT_R5G6B5:
				formatName = _T("D3DFMT_R5G6B5");
				break;
			case D3DFMT_R3G3B2:
				formatName = _T("D3DFMT_R3G3B2");
				break;
			default:
				{
					formatName = _T(" ETC ");
					break;
				}
				break;
			};
		}

		if( !formatName.empty() )
		{
            CDebugSet::ToLogFile(
                sc::string::format( _T("compress || Format : %1% || 파일이름 : %2% \n"),
				formatName,
                fileName ) );
		}

        BOOL bMipMapError(FALSE);
        if ( addData.m_mipMap == 0 )
        {
            // 원하는 건 MipMap 전부 생성인데, 파일에는 밉맵이 안 만들어져 있다면, 만들어 달라고 요청.
            if ( imageInfo.MipLevels == 1 )
            {
                bMipMapError = TRUE;
            }
        }
        else
        {
            // 특정 수치까지는 MipMap이 보이도록 작업, 파일이 그것까지 만족 안하면, 만들어 달라고 요청.
            if( imageInfo.MipLevels < addData.m_mipMap )
            {
                bMipMapError = TRUE;
            }
        }

		// [shhan][2014.09.05] mipmap 문제는 지속적 로그가 나오게 해서 패치가 되도록 유도한다.
        if( bMipMapError )
		{
			//switch ( d3dSurfaceDesc.Format )
			//{
			//	// 
			//case D3DFMT_DXT1:
			//case D3DFMT_DXT2:
			//case D3DFMT_DXT3:
			//case D3DFMT_DXT4:
			//case D3DFMT_DXT5:

//			// [shhan][2016.09.09] 포기다. 도대체가 패치를 안한다.
//#ifdef _RELEASED
//			// ReleaseD 때는 정상적으로 txt(내문서의 \RanOnline\errlog), csv 에 나오도록 한다.
//			CDebugSet::ToLogFile( sc::string::format( _T(" create mipmap : %1% \n"), fileName ), TRUE );
//#else
//			// 배포버젼에서는 내문서의 \RanOnline\errlog에 txt 만 되고, csv 에는 저장 안하도록 한다.
//			CDebugSet::ToLogFile( sc::string::format( _T(" create mipmap : %1% \n"), fileName ), FALSE );
//#endif

			//	break;

			//	// dds 포멧이 아닌 경우는 밉맵생성을 요청하지 말자.
			//	// bmp tga 등이 나올수 있다.
			//default:
			//	break;
		}

#ifdef _RELEASED
		if ( NSLoadingLagTester::g_TextureManager_ProcessCompleteData < timeGetTime() - dwTick )
		{
			NSLoadingLagTester::g_TextureManager_ProcessCompleteData = timeGetTime() - dwTick;
			NSLoadingLagTester::g_strTextureManager_ProcessCompleteData = fileName;
		}
#endif

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("Texture Load %1% - %2%", fileName, dwTick ) );

		if ( timeGetTime() - dwPrevTick > nWorkTime )
		{
			break;
		}
    }

    if( !dataVec.empty() )
        dataVec.erase( dataVec.begin(), it );
}

// TAG_UpdateResourceManager_4cc
void TextureManager::ProcessCompleteDataBackUpCache( NAME_N_SP_COMPLETED_DATA_VECTOR& CompletedDataVec )
{
	NAME_N_SP_COMPLETED_DATA_VECTOR::iterator iter = CompletedDataVec.begin();
	while( iter != CompletedDataVec.end() )
	{
		if ( (*iter).second->IsBackUpCache() )
		{
			BackUpCache( (*iter).first );
		}
		++iter;
	}
}

// TAG_LoadResourceManager_1
TextureManager::Resource 
TextureManager::LoadTexture(
    const TSTRING& pathFileName,
    bool bMustDirectory,
    TEXTURE_ADD_DATA::TYPE textureType,
    DWORD mipMap,
    D3DFORMAT d3dFormat,
    TEXTURE_RESIZE resizeType,
    TEXTURE_CATEGORY emCategory,
    bool bBackgroundLoading,
	BOOL bBackUpCache)
{
	if( pathFileName.size() <= 4 )
		return Resource();

    TSTRING strFilePath = pathFileName;
    sc::string::lowerString( strFilePath );

	TEXTURE_ADD_DATA addData( textureType, mipMap, d3dFormat, resizeType, emCategory, bBackUpCache ); 
	Resource resource;

	if ( emCategory == TEXTURE_MATERIALS )
	{
		resource = LoadData(strFilePath,strFilePath,bMustDirectory,addData,bBackgroundLoading);
	}
	else
	{
		FindAndChangeExtMTF( strFilePath );
		resource = LoadData(strFilePath,bMustDirectory,addData,bBackgroundLoading);
	}

	if( m_bEnableLog )
	{
		if( resource.IsValid() )
		{
			//		if( resource.GetAddData() != addData )
			// 일단 밉맵만 문제가 있으므로 비교대상에선 일단 빼자
			if( resource.GetAddData().m_type != addData.m_type ||
				resource.GetAddData().m_d3dFormat != addData.m_d3dFormat ||
				resource.GetAddData().m_resizeType != addData.m_resizeType )
			{
                if( resource.GetAddData().m_resizeType != TEXTURE_RESIZE_NONE &&
                    addData.m_resizeType != TEXTURE_RESIZE_NONE )
                {
				    CDebugSet::ToLogFile(
					    sc::string::format(
					    _T("\noriginal data :  fileName : %1% Format : %2% Mipmap : %3% Resize : %4%\n\
        //                   new      data :  fileName : %5% Format : %6% Mipmap : %7% Resize : %8%"),
					    resource.GetFileName(),resource.GetAddData().m_d3dFormat,
					    resource.GetAddData().m_mipMap,resource.GetAddData().m_resizeType,
					    pathFileName,addData.m_d3dFormat,addData.m_mipMap,addData.m_resizeType) );
                }
			}
		}

		// BackgroundLoading도 아닌데, FALSE 라면 파일이 없다는 뜻임.
		if ( !bBackgroundLoading && !resource.IsValid() )
		{
			CDebugSet::ToLogFile(
				sc::string::format(
				_T(" file open fail : %1%"),
				resource.GetFileName()) );
		}
	}

	return resource;
}

bool
TextureManager::ReLoadTexture(
							  const TSTRING& pathFileName,
							  Resource& kRes,
							  bool bMustDirectory,
							  TEXTURE_ADD_DATA::TYPE textureType,
							  DWORD mipMap,
							  D3DFORMAT d3dFormat,
							  TEXTURE_RESIZE resizeType,
							  TEXTURE_CATEGORY emCategory,
							  bool bBackgroundLoading,
							  BOOL bBackUpCache)
{
	if( pathFileName.size() <= 4 )
		return false;

	if (kRes.IsValid() == false)
	{
		return false;
	}

	TSTRING strFilePath = pathFileName;
	sc::string::lowerString( strFilePath );

	FindAndChangeExtMTF( strFilePath );

	TEXTURE_ADD_DATA addData( textureType, mipMap, d3dFormat, resizeType, emCategory, bBackUpCache ); 
	return ReLoadData(strFilePath,kRes,bMustDirectory,addData,bBackgroundLoading);
}

bool TextureManager::ReLoadData(const TSTRING& pathFileName,Resource& kRes,bool bMustDirectory,const TEXTURE_ADD_DATA& addData, bool bBackgroundLoading)
{
	TSTRING lowPathFileName = pathFileName;
	TSTRING fileName;

	fileName = QueryPathFileNameHelper( lowPathFileName, lowPathFileName, lowPathFileName );

	UINT nDataSize = 0;
	CTypeSmartPtr spCTypeRawData = CFileSystem::getInstance().getFile( fileName, nDataSize );

	if( spCTypeRawData != NULL )
	{
		SP_RAW_DATA& spRawData = spCTypeRawData.GetSPMemory();		

        SP_QUEUE_DATA spQueueData;
        if ( addData.m_emCategory == TEXTURE_MATERIALS )
        {
            spQueueData = SP_QUEUE_DATA( new QUEUE_DATA( fileName, fileName, bMustDirectory, addData ));
        }
        else
        {
            spQueueData = SP_QUEUE_DATA(new QUEUE_DATA( fileName, bMustDirectory, addData ));
        }
		//SP_QUEUE_DATA spQueueData( new QUEUE_DATA( fileName, bMustDirectory, addData ) );
		spQueueData->m_rawDataSize = nDataSize;
		spQueueData->m_spRawData = spRawData;

		ResourceQueue<TEXTURE_ADD_DATA>::SP_QUEUE_DATA_VECTOR completedRawDataVec;
		completedRawDataVec.push_back( spQueueData );

		DWORD nWorkTime = ULONG_MAX;
		NAME_N_SP_COMPLETED_DATA_VECTOR completedDataVec; 
		completedDataVec.reserve( completedRawDataVec.size() );

		ProcessCompleteData( completedRawDataVec, &completedDataVec, 0, nWorkTime );

		if (completedDataVec.empty() == false)
		{
			kRes.Get()->DataExchange( *(completedDataVec[0].second.get()) );
			return true;
		}
	}

	return false;
}

// IDirect3DBaseTexture9* 
// TextureManager::LoadTextureNoManaged(const TSTRING& pathFileName,
// 							bool bMustDirectory,
// 							TEXTURE_ADD_DATA::TYPE textureType,
// 							DWORD mipMap,
// 							D3DFORMAT d3dFormat,
// 							TEXTURE_RESIZE resizeType)
// {
// 	TEXTURE_ADD_DATA addData(textureType,mipMap,d3dFormat,resizeType); 
// 	Resource res = LoadData(pathFileName,bMustDirectory,addData,false);
// 	DeleteData(res);
// 
// 	if( textureType == TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D)
// 		return res.ReleaseTexture();
// 
// 	else if( textureType == TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE)
// 		return res.ReleaseCubeTexture();
// 
// 	MIN_ASSERT(false);
// 	return NULL;
// }

//////////////////////////////////////////////////////////////////////////

void TextureManager::ReleaseTexture( LPDIRECT3DTEXTURE9& pTexture, TEXTURE_CATEGORY emCategory )
{
    if( !pTexture )
        return;

    TextureMemory::OnRelease( pTexture, emCategory );
    pTexture->Release();
    pTexture = NULL;
}

void TextureManager::ReleaseTexture( LPDIRECT3DCUBETEXTURE9& pTexture, TEXTURE_CATEGORY emCategory )
{
    if( !pTexture )
        return;

    TextureMemory::OnRelease( pTexture, emCategory );
    pTexture->Release();
    pTexture = NULL;
}

HRESULT TextureManager::CreateTexture(
    LPDIRECT3DDEVICEQ pDevice,
    UINT& uiWidth,
    UINT& uiHeight,
    UINT uiMipLevels,
    DWORD dwUsage,
    D3DFORMAT& emFormat,
    D3DPOOL emPool,
    LPDIRECT3DTEXTUREQ* ppTexture,
    TEXTURE_RESIZE resizeType,
    TEXTURE_CATEGORY emCategory )
{
	BoxSize boxSize = GetForceLessSize(uiWidth,uiHeight,resizeType,_T(""));
	uiWidth = boxSize.width;
	uiHeight = boxSize.height;

	HRESULT hr = D3DXCreateTexture(
        pDevice,
        uiWidth,
        uiHeight,
        uiMipLevels,
        dwUsage,
		emFormat,
        emPool,
        ppTexture );

    if( SUCCEEDED( hr ) )
        TextureMemory::OnCreate( *ppTexture, emCategory );

    return hr;
}

HRESULT TextureManager::CreateTextureFromFileEx(
    LPDIRECT3DDEVICE9 pDevice,
    LPCSTR pSrcFile,
    UINT& uiWidth,
    UINT& uiHeight,
    UINT uiMipLevels,
    DWORD dwUsage,
    D3DFORMAT& emFormat,
    D3DPOOL emPool,
    DWORD dwFilter,
    DWORD dwMipFilter,
    D3DCOLOR dwColorKey,
    D3DXIMAGE_INFO* pSrcInfo,
    PALETTEENTRY* pPalette,
    LPDIRECT3DTEXTUREQ* ppTexture,
    TEXTURE_RESIZE resizeType,
    TEXTURE_CATEGORY emCategory )
{
	if( uiWidth == D3DX_DEFAULT || uiHeight == D3DX_DEFAULT )
	{
		BoxSize boxSize = GetForceLessSize(pSrcFile,pSrcInfo,resizeType);
		uiWidth = boxSize.width;
		uiHeight = boxSize.height;
	}

	HRESULT hr = D3DXCreateTextureFromFileEx(
        pDevice,
        pSrcFile,
        uiWidth,
        uiHeight,
        uiMipLevels,
		dwUsage,
        emFormat,
        emPool,
        dwFilter,
        dwMipFilter,
        dwColorKey,
        pSrcInfo,
        pPalette,
        ppTexture );

    if( SUCCEEDED( hr ) )
        TextureMemory::OnCreate( *ppTexture, emCategory );

    return hr;
}

HRESULT TextureManager::CreateTextureFromFileInMemoryEx(
    LPDIRECT3DDEVICE9 pDevice,
    LPCVOID pSrcData,
    UINT uiSrcDataSize,
    UINT& uiWidth,
    UINT& uiHeight,
    UINT uiMipLevels,
    DWORD dwUsage,
    D3DFORMAT& emFormat,
    D3DPOOL emPool,
    DWORD dwFilter,
    DWORD dwMipFilter,
    D3DCOLOR dwColorKey,
    D3DXIMAGE_INFO* pSrcInfo,
    PALETTEENTRY* pPalette,
    LPDIRECT3DTEXTUREQ* ppTexture,
    TEXTURE_RESIZE resizeType,
    TEXTURE_CATEGORY emCategory )
{
	if( uiWidth == D3DX_DEFAULT || uiHeight == D3DX_DEFAULT )
	{
		BoxSize boxSize = GetForceLessSize(_T(""),pSrcInfo,resizeType,pSrcData,uiSrcDataSize);
		uiWidth = boxSize.width;
		uiHeight = boxSize.height;
	}

	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(
        pDevice,
        pSrcData,
        uiSrcDataSize,
        uiWidth,
        uiHeight,
		uiMipLevels,
        dwUsage,
        emFormat,
        emPool,
        dwFilter,
        dwMipFilter,
        dwColorKey,
        pSrcInfo,
        pPalette,
        ppTexture );

    if( SUCCEEDED( hr ) )
        TextureMemory::OnCreate( *ppTexture, emCategory );

    return hr;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



BoxSize TextureManager::GetForceLessSize(
	const TSTRING& fileName, 
	D3DXIMAGE_INFO* pImageInfo ,
	TEXTURE_RESIZE resizeType,
	const void* pRawData,
	UINT rawDataSize)
{
	D3DXIMAGE_INFO tempImageInfo;
	HRESULT hr = S_FALSE;
	if( !fileName.empty() && pRawData == NULL)
		D3DXGetImageInfoFromFile(fileName.c_str(),&tempImageInfo);
	else if( pRawData != NULL && rawDataSize > 0 )
		hr = D3DXGetImageInfoFromFileInMemory( pRawData, rawDataSize, &tempImageInfo);
	else
		MIN_ASSERT(false);

	if(hr != S_OK)
	{
		CDebugSet::ToLogFile(
			sc::string::format( _T("no exist or not Image file :  %1%"),fileName ) );

		return BoxSize(128,128);
	}

	UINT width = tempImageInfo.Width;
	UINT height = tempImageInfo.Height;

	if( pImageInfo )
		*pImageInfo = tempImageInfo;

	return GetForceLessSize(width,height,resizeType,fileName);
}

BoxSize TextureManager::GetForceLessSize( UINT width,UINT height,TEXTURE_RESIZE resizeType,const TSTRING& fileName)
{
	if( !m_bForcelessThan1024 || resizeType == TEXTURE_RESIZE_NONE )
		return BoxSize( width , height );

	//////////////////////////////////////////////////////////////////////////
	if( width > TEXTURE_RESIZE_1024 || height > TEXTURE_RESIZE_1024 )
	{
		CDebugSet::ToLogFile(
			sc::string::format( _T("resizing candidate file :  %1%"),fileName ) );
	}

	while( width > UINT(resizeType) || height > UINT(resizeType) )
	{
		width /= 2;
		height /= 2;
	}

	return BoxSize( width,height );
}

TEXTURE_ALPHA TextureManager::GetTexType(const TSTRING& strName )
{
	if ( strName.empty() )
		return EMTT_NORMAL;

	TSTRING lowName = strName;
	sc::string::lowerString( lowName );

    FindAndChangeExtMTF( lowName );

	return GetCompleteTex_TexType( lowName );
}

TEXTURE_ALPHA TextureManager::GetCompleteTex_TexType( const TSTRING& strName )
{
    CompletedTextureData* pCompletedData = GetCompletedData( strName );
    if ( !pCompletedData )
        return EMTT_NORMAL;

	return pCompletedData->GetEmTextType();
}


TSTRING TextureManager::QueryPathFileNameHelper(const TSTRING& pathFileName,
													 const TSTRING& path,const TSTRING& fileName)
{	
	// 풀패스가 존재하면 그냥 리턴 아님 알아낸다
	TSTRING fileTreeFullPathFileName = m_fileTree.FindPathName(fileName);
	sc::string::lowerString(fileTreeFullPathFileName);

	if( !fileTreeFullPathFileName.empty() )
		return fileTreeFullPathFileName;

	return pathFileName;
}

void TextureManager::FindAndChangeExtMTF( TSTRING& strFilePath )
{
    // 구버전 텍스쳐 매니저는 mtf 파일이 있으면 그것을 로딩하고
    // 없으면 넘어온 확장자 파일을 읽도록 되어있어서 맞춰줌
    {
        CString strChangeFileExt;
        STRUTIL::ChangeExt( strFilePath.c_str(), strChangeFileExt, _T( ".mtf" ) );

        TSTRING strFullPath = m_fileTree.FindPathName( TSTRING( strChangeFileExt.GetString() ) );
        if( !strFullPath.empty() )
            strFilePath = strFullPath;
    }
}

// 들어온 확장자와 mtf 모두 검색하도록 한다.
bool TextureManager::IsPathFile( const TSTRING& fileName )
{
	// 소문자로 변환.
	TSTRING strTextureNameLwr = fileName;
	sc::string::lowerString( strTextureNameLwr );	

	CString strChangeFileExt;
	STRUTIL::ChangeExt( strTextureNameLwr.c_str(), strChangeFileExt, _T( ".mtf" ) );

	TSTRING strFullPath = m_fileTree.FindPathName( TSTRING( strChangeFileExt.GetString() ) );
	if( strFullPath.empty() )
	{
		const TSTRING& strFilePath = m_fileTree.FindPathName( strTextureNameLwr );
		return strFilePath.empty() ? false : true;
	}
	else
	{
		return true;
	}
}

bool TextureManager::FindFilePath( TSTRING& strFilePath )
{
    // 소문자로 변환.
    TSTRING strTextureNameLwr = strFilePath;
    sc::string::lowerString( strTextureNameLwr );	

    const TSTRING& strOutPath = m_fileTree.FindPathName( strTextureNameLwr );

    if (strOutPath.empty() == false)
    {
        strFilePath = strOutPath;
        return true;
    }

    return false;
}

void TextureManager::Archive( boost::function<void (const TCHAR*)> fun )
{
    BOOST_FOREACH(COMPLTED_DATA_MAP::value_type& it, m_completedDataMap)
    {
        fun( m_fileTree.FindPathName(it.first).c_str() );
    }

    BOOST_FOREACH(COMPLTED_DATA_MAP::value_type& it, m_backupCacheCompletedDataMap)
    {
        fun( m_fileTree.FindPathName(it.first).c_str() );
    }
}

namespace TextureManagerUtil
{
    // 모든 텍스쳐의 Mipmap 을 검사한다.
    void AllTextureCheckMipMap_Detail( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName )
    {
        std::string::size_type idx;
        std::string strExtName;
        std::string strChar[2];
        std::string strNumber[3];

        {
            idx = strName.find_last_of('.');
            if ( idx == -1 )
                return;

            strExtName = strName.substr ( idx+1 );
            if ( strExtName == _T("svn-base") || strExtName == _T("db") || strExtName == _T("psd") || strExtName == _T("dat")|| strExtName == _T("zip") )
            {
                return;
            }

            // LightMap 은 검사에서 뺀다.
            if ( idx >= 5 )
            {
                strChar[0] = strName.substr ( idx-5, 1 );
                strChar[1] = strName.substr ( idx-4, 1 );
                strNumber[0] = strName.substr ( idx-3, 1 );
                strNumber[1] = strName.substr ( idx-2, 1 );
                strNumber[2] = strName.substr ( idx-1, 1 );
                if ( ( strChar[0] == "_" ) &&
                    ( strChar[1] == "D" || strChar[1] == "N" || strChar[1] == "d" || strChar[1] == "n" ) &&
                    ( strNumber[0] >= "0" && strNumber[0] <= "9" ) &&
                    ( strNumber[1] >= "0" && strNumber[1] <= "9" ) &&
                    ( strNumber[2] >= "0" && strNumber[2] <= "9" ) )
                {
                    return;
                }
            }

            // LightMap 은 검사에서 뺀다. 예전버전에서는 _d00 숫자가 2 자리였던 것이 있었다.
            if ( idx >= 4 )
            {
                strChar[0] = strName.substr ( idx-4, 1 );
                strChar[1] = strName.substr ( idx-3, 1 );
                strNumber[0] = strName.substr ( idx-2, 1 );
                strNumber[1] = strName.substr ( idx-1, 1 );
                if ( ( strChar[0] == "_" ) &&
                    ( strChar[1] == "D" || strChar[1] == "N" || strChar[1] == "d" || strChar[1] == "n" ) &&
                    ( strNumber[0] >= "0" && strNumber[0] <= "9" ) &&
                    ( strNumber[1] >= "0" && strNumber[1] <= "9" ) )
                {
                    return;
                }
            }

			// LightMap 은 검사에서 뺀다.
			if ( idx >= 5 )
			{
				strChar[0] = strName.substr ( idx-5, 1 );
				strChar[1] = strName.substr ( idx-4, 1 );
				strNumber[0] = strName.substr ( idx-3, 1 );
				strNumber[1] = strName.substr ( idx-2, 1 );
				strNumber[2] = strName.substr ( idx-1, 1 );
				if ( ( strChar[0] == "c" || strChar[0] == "C" || strChar[0] == "1" || strChar[0] == "2" || strChar[0] == "i" || strChar[0] == "I" ) &&
					( strChar[1] == "_" ) &&
					( strNumber[0] >= "0" && strNumber[0] <= "9" ) &&
					( strNumber[1] >= "0" && strNumber[1] <= "9" ) &&
					( strNumber[2] >= "0" && strNumber[2] <= "9" ) )
				{
					return;
				}

				if ( ( strChar[0] == "_" ) &&
					( strChar[1] == "c" ) &&
					( strNumber[0] >= "0" && strNumber[0] <= "9" ) &&
					( strNumber[1] >= "0" && strNumber[1] <= "9" ) &&
					( strNumber[2] >= "0" && strNumber[2] <= "9" ) )
				{
					return;
				}
			}

            TextureManager::GetInstance().LoadTexture(
                strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_ETC,
                false,
				FALSE );
        }
    }

    void AllTextureCheckMipMap_PathRecurse( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& strDir )
    {
        std::string ErrorMsg = sc::string::format( _T(" ---- Path: %1%"), strDir );
        sc::writeLogInfo(ErrorMsg);

        CFileFind finder;

        //	build a string with wildcards
        std::string strWildcard ( strDir );
        strWildcard += _T("\\*.*");

        //	start working for files
        BOOL bWorking = finder.FindFile ( strWildcard.c_str() );

        while ( bWorking )
        {
            bWorking = finder.FindNextFile ();

            //	skip . and .. files; otherwise, we'd
            //	recur infinitely!
            if ( finder.IsDots() )
                continue;

            //	if it's a directory, recursively search it
            if ( finder.IsDirectory() )
            {
                std::string strPath = finder.GetFilePath();
                AllTextureCheckMipMap_PathRecurse( pd3dDevice, strPath );
            }
            else
            {
                std::string strName(finder.GetFileName().GetString());
                sc::string::lowerString(strName);

                std::string strPath(finder.GetFilePath().GetString());

                AllTextureCheckMipMap_Detail( pd3dDevice, strName );
            }
        }

        finder.Close();
    }

    void AllTextureCheckMipMap( LPDIRECT3DDEVICEQ pd3dDevice )
    {
        g_bLOG_ONLY_MIPMAP = TRUE;

        AllTextureCheckMipMap_PathRecurse(
            pd3dDevice,
            TextureManager::GetInstance().GetPath() );

        g_bLOG_ONLY_MIPMAP = FALSE;
    }
}
