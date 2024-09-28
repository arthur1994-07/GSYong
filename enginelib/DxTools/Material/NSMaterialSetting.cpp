#include "pch.h"

#include "../../DxLand/DxPiece/DxSingleTexMesh.h"

#include "NSMaterialSetting.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSMaterialSetting
{
	DWORD SETTING::GetFlag( BOOL bLightMapUV_Empty ) const
	{
		DWORD dwFlags(0L);

		// Note : 카메라 충돌여부를 설정한다.
		if ( m_bCameraCollision )
		{
			dwFlags |= ENSTM_CAMCOLL;
		}
		else
		{
			dwFlags &= ~ENSTM_CAMCOLL;
		}

		// Note : 그림자 생성여부를 설정한다.
		if ( m_bCastShadow )
		{
			dwFlags &= ~ENSTM_DONT_CAST_SHADOW_LM;
		}
		else
		{
			dwFlags |= ENSTM_DONT_CAST_SHADOW_LM;
		}

		// Note : 렌더 Type 을 설정한다.
		dwFlags &= ~ENSTM_UNLIGHTMAP_MAX;
		if ( m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_AMBIENTCUBE )
		{
			dwFlags |= ENSTM_UNLIGHTMAP_AMBIENT_CUBE;
		}
		else if ( m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_VOXELCOLOR )
		{
			dwFlags |= ENSTM_UNLIGHTMAP_VOXEL_COLOR;
		}
		else if ( bLightMapUV_Empty )
		{
			dwFlags |= ENSTM_UNLIGHTMAP_AMBIENT_CUBE;
		}

		// Note : 데칼 생성여부를 설정한다.
		dwFlags &= ~ENSTM_DECAL_FULL;
		if ( m_emDecalType == NSMaterialSetting::DECAL_TYPE_ALPHABLEND )
		{
			dwFlags |= ENSTM_DECAL_ALPHABLEND;
		}
		else if ( m_emDecalType == NSMaterialSetting::DECAL_TYPE_ALPHATEST )
		{
			dwFlags |= ENSTM_DECAL_ALPHATEST;
		}
		else if ( m_emDecalType == NSMaterialSetting::DECAL_TYPE_MULTIPLY )
		{
			dwFlags |= ENSTM_DECAL_MULTIPLY;
		}
		else if ( m_emDecalType == NSMaterialSetting::DECAL_TYPE_ADD )
		{
			dwFlags |= ENSTM_DECAL_ADD;
		}

		// Note : 데칼 Order
		dwFlags &= ~ENSTM_DECAL_ORDER_MAX;
		if ( m_nDecalOrder == 3 )
		{
			dwFlags |= ENSTM_DECAL_ORDER_3;
		}
		else if ( m_nDecalOrder == 2 )
		{
			dwFlags |= ENSTM_DECAL_ORDER_2;
		}
		else
		{
			dwFlags |= ENSTM_DECAL_ORDER_1;
		}

		// Note : NormalMap
		if ( m_bDecalNormal )
		{
			dwFlags |= ENSTM_DECAL_NORMAL;
		}
		else
		{
			dwFlags &= ~ENSTM_DECAL_NORMAL;
		}

		// Note : CubeMap
		if ( m_bCubeMap )
		{
			dwFlags |= ENSTM_CUBEMAP;
		}
		else
		{
			dwFlags &= ~ENSTM_CUBEMAP;
		}

		// Note : SpotSpecMask
		if ( m_bMaskCubeMapSpecularMap )
		{
			dwFlags |= ENSTM_MASK_CUBEMAP_SPECULARMAP;
		}
		else
		{
			dwFlags &= ~ENSTM_MASK_CUBEMAP_SPECULARMAP;
		}

		// Note : NormalMap
		if ( m_bNormalMap )
		{
			dwFlags |= ENSTM_NORMALMAP;
		}
		else
		{
			dwFlags &= ~ENSTM_NORMALMAP;
		}

		return dwFlags;
	}
};

CMaterialSetting::CMaterialSetting()
{

}

CMaterialSetting::~CMaterialSetting()
{

}

// 정보 삭제
void CMaterialSetting::CleanUp()
{
	m_mapSetting.clear();
}

// Default Material Setting 을 위해 Material 이름들을 입력해놓는다.
void CMaterialSetting::Insert( const VEC_TSTRING& vecMaterialName )
{
	for ( DWORD i=0; i<vecMaterialName.size(); ++i )
	{
		m_mapSetting.insert( std::make_pair( vecMaterialName[i], NSMaterialSetting::SETTING() ) );
	}
}

// Material 갯수
size_t CMaterialSetting::MaterialSize()
{
	return m_mapSetting.size();
}

// Material 정보
BOOL CMaterialSetting::GetMaterial( size_t nID, TSTRING& strName, NSMaterialSetting::SETTING& sSetting )
{
	if ( nID >= m_mapSetting.size() )
		return FALSE;

	size_t nCount(0);
	NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.begin();
	for ( ; iter!=m_mapSetting.end(); ++iter, ++ nCount )
	{
		if ( nID == nCount )
		{
			strName = (*iter).first;
			sSetting = (*iter).second;
			return TRUE;
		}
	}
	return FALSE;
}

// Material 정보
const NSMaterialSetting::SETTING* CMaterialSetting::GetMaterialSetting( const TSTRING& strName ) const
{
	NSMaterialSetting::MAP_SETTING_CITER iter = m_mapSetting.find( strName );
	if ( iter!=m_mapSetting.end() )
		return &(*iter).second;
	else
		return NULL;
}

// strName Material 의 정보를 sSetting으로 변경한다.
void CMaterialSetting::SetMaterialSetting( TSTRING& strName, NSMaterialSetting::SETTING& sSetting )
{
	NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
	if ( iter!=m_mapSetting.end() )
		(*iter).second = sSetting;
}

void CMaterialSetting::SaveSet( sc::SerialFile& SFile )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		DWORD dwSize = (DWORD)m_mapSetting.size();
		SFile << dwSize;

		NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.begin();
		for( ; iter!=m_mapSetting.end(); ++iter )
		{
			SFile << (*iter).first;
			SFile << (*iter).second.m_bCastShadow;
			SFile << (*iter).second.m_bCameraCollision;
			SFile << (*iter).second.m_bCubeMap;
			SFile << (*iter).second.m_fCubeMapValue;
			SFile << (*iter).second.m_fSpotSpecPower;
			SFile << (*iter).second.m_fSpotSpecMulti;
			SFile << (*iter).second.m_bMaskCubeMapSpecularMap;
			SFile << (*iter).second.m_bNormalMap;
			SFile << static_cast<DWORD>( (*iter).second.m_emUnLightMapType );
			SFile << static_cast<DWORD>( (*iter).second.m_emDecalType );
			SFile << (*iter).second.m_nDecalOrder;
			SFile << (*iter).second.m_bDecalNormal;
		}
	}
	SFile.EndBlock();
}

void CMaterialSetting::LoadSet( sc::SerialFile& SFile )
{
	DWORD dwVer, dwBuffSize;
	SFile >> dwVer;
	SFile >> dwBuffSize;

	if ( VERSION==dwVer )
	{
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;
			SFile >> sSetting.m_bCameraCollision;
			SFile >> sSetting.m_bCubeMap;
			SFile >> sSetting.m_fCubeMapValue;
			SFile >> sSetting.m_fSpotSpecPower;
			SFile >> sSetting.m_fSpotSpecMulti;
			SFile >> sSetting.m_bMaskCubeMapSpecularMap;
			SFile >> sSetting.m_bNormalMap;

			DWORD dwRenderType;
			SFile >> dwRenderType;
			sSetting.m_emUnLightMapType = static_cast<NSMaterialSetting::UNLIGHTMAP_TYPE>( dwRenderType );

			DWORD dwDecal;
			SFile >> dwDecal;
			sSetting.m_emDecalType = static_cast<NSMaterialSetting::DECAL_TYPE>( dwDecal );

			SFile >> sSetting.m_nDecalOrder;

			SFile >> sSetting.m_bDecalNormal;

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}
		}
		return;
	}
	else if ( 0x0110==dwVer )
	{
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			BOOL bSpecular(FALSE);
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;
			SFile >> sSetting.m_bCameraCollision;
			SFile >> sSetting.m_bCubeMap;
			SFile >> sSetting.m_fCubeMapValue;
			SFile >> bSpecular;
			SFile >> sSetting.m_fSpotSpecPower;
			SFile >> sSetting.m_fSpotSpecMulti;
			SFile >> sSetting.m_bMaskCubeMapSpecularMap;
			SFile >> sSetting.m_bNormalMap;

			DWORD dwRenderType;
			SFile >> dwRenderType;
			sSetting.m_emUnLightMapType = static_cast<NSMaterialSetting::UNLIGHTMAP_TYPE>( dwRenderType );

			DWORD dwDecal;
			SFile >> dwDecal;
			sSetting.m_emDecalType = static_cast<NSMaterialSetting::DECAL_TYPE>( dwDecal );

			SFile >> sSetting.m_nDecalOrder;

			// 0x0111 에서는 작업할 필요가 없음.
			sSetting.m_fSpotSpecMulti = bSpecular ? sSetting.m_fSpotSpecMulti : 0.f;

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}
		}
		return;
	}
	else if ( 0x0109==dwVer )
	{
		BOOL bTemp(FALSE);
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;
			SFile >> sSetting.m_bCameraCollision;
			SFile >> sSetting.m_bCubeMap;
			SFile >> sSetting.m_fCubeMapValue;
			SFile >> bTemp;
			sSetting.m_bMaskCubeMapSpecularMap = bTemp;

			SFile >> sSetting.m_fSpotSpecPower;
			SFile >> sSetting.m_fSpotSpecMulti;
			SFile >> bTemp;
			sSetting.m_bMaskCubeMapSpecularMap = bTemp ? TRUE : sSetting.m_bMaskCubeMapSpecularMap;
			SFile >> sSetting.m_bNormalMap;

			DWORD dwRenderType;
			SFile >> dwRenderType;
			sSetting.m_emUnLightMapType = static_cast<NSMaterialSetting::UNLIGHTMAP_TYPE>( dwRenderType );

			DWORD dwDecal;
			SFile >> dwDecal;
			sSetting.m_emDecalType = static_cast<NSMaterialSetting::DECAL_TYPE>( dwDecal );

			SFile >> sSetting.m_nDecalOrder;

			sSetting.m_fSpotSpecMulti = bTemp ? sSetting.m_fSpotSpecMulti : 0.f;

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}

			//// - 형상 탭 Material Setting 목록 갱신이 안됨 : 기존 세팅 된 x파일을 새로 익스포트 시 기존값이 존재하거나, 불필요한 파일이 쌓입니다 ( 수정함 )
			//else
			//{
			//	m_mapSetting.insert ( std::make_pair(strName,sSetting) );
			//}
		}
		return;
	}
	else if ( 0x0108==dwVer )
	{
		BOOL bTemp(FALSE);
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;
			SFile >> sSetting.m_bCameraCollision;
			SFile >> sSetting.m_bCubeMap;
			SFile >> sSetting.m_fCubeMapValue;
			SFile >> bTemp;							
			sSetting.m_bMaskCubeMapSpecularMap = bTemp;

			SFile >> sSetting.m_fSpotSpecPower;
			SFile >> sSetting.m_fSpotSpecMulti;
			SFile >> bTemp;
			sSetting.m_bMaskCubeMapSpecularMap = bTemp ? TRUE : sSetting.m_bMaskCubeMapSpecularMap;

			DWORD dwRenderType;
			SFile >> dwRenderType;
			sSetting.m_emUnLightMapType = static_cast<NSMaterialSetting::UNLIGHTMAP_TYPE>( dwRenderType );

			DWORD dwDecal;
			SFile >> dwDecal;
			sSetting.m_emDecalType = static_cast<NSMaterialSetting::DECAL_TYPE>( dwDecal );

			SFile >> sSetting.m_nDecalOrder;

			sSetting.m_fSpotSpecMulti = bTemp ? sSetting.m_fSpotSpecMulti : 0.f;

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}

			//// - 형상 탭 Material Setting 목록 갱신이 안됨 : 기존 세팅 된 x파일을 새로 익스포트 시 기존값이 존재하거나, 불필요한 파일이 쌓입니다 ( 수정함 )
			//else
			//{
			//	m_mapSetting.insert ( std::make_pair(strName,sSetting) );
			//}
		}
		return;
	}
	else if ( 0x0107==dwVer )
	{
		BOOL bTemp(FALSE);
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;
			SFile >> sSetting.m_bCameraCollision;
			SFile >> sSetting.m_bCubeMap;
			SFile >> sSetting.m_fCubeMapValue;
			SFile >> bTemp;							
			sSetting.m_bMaskCubeMapSpecularMap = bTemp;

			DWORD dwRenderType;
			SFile >> dwRenderType;
			sSetting.m_emUnLightMapType = static_cast<NSMaterialSetting::UNLIGHTMAP_TYPE>( dwRenderType );

			DWORD dwDecal;
			SFile >> dwDecal;
			sSetting.m_emDecalType = static_cast<NSMaterialSetting::DECAL_TYPE>( dwDecal );

			SFile >> sSetting.m_nDecalOrder;

			sSetting.m_fSpotSpecMulti = bTemp ? sSetting.m_fSpotSpecMulti : 0.f;

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}

			//// - 형상 탭 Material Setting 목록 갱신이 안됨 : 기존 세팅 된 x파일을 새로 익스포트 시 기존값이 존재하거나, 불필요한 파일이 쌓입니다 ( 수정함 )
			//else
			//{
			//	m_mapSetting.insert ( std::make_pair(strName,sSetting) );
			//}
		}
		return;
	}
	else if ( 0x0106==dwVer )
	{
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;
			SFile >> sSetting.m_bCameraCollision;
			SFile >> sSetting.m_bCubeMap;
			SFile >> sSetting.m_fCubeMapValue;

			DWORD dwRenderType;
			SFile >> dwRenderType;
			sSetting.m_emUnLightMapType = static_cast<NSMaterialSetting::UNLIGHTMAP_TYPE>( dwRenderType );

			DWORD dwDecal;
			SFile >> dwDecal;
			sSetting.m_emDecalType = static_cast<NSMaterialSetting::DECAL_TYPE>( dwDecal );

			SFile >> sSetting.m_nDecalOrder;

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}

			//// - 형상 탭 Material Setting 목록 갱신이 안됨 : 기존 세팅 된 x파일을 새로 익스포트 시 기존값이 존재하거나, 불필요한 파일이 쌓입니다 ( 수정함 )
			//else
			//{
			//	m_mapSetting.insert ( std::make_pair(strName,sSetting) );
			//}
		}
		return;
	}
	else if ( 0x0105==dwVer )
	{
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;
			SFile >> sSetting.m_bCameraCollision;

			DWORD dwRenderType;
			SFile >> dwRenderType;
			sSetting.m_emUnLightMapType = static_cast<NSMaterialSetting::UNLIGHTMAP_TYPE>( dwRenderType );

			DWORD dwDecal;
			SFile >> dwDecal;
			sSetting.m_emDecalType = static_cast<NSMaterialSetting::DECAL_TYPE>( dwDecal );

			SFile >> sSetting.m_nDecalOrder;

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}

			//// - 형상 탭 Material Setting 목록 갱신이 안됨 : 기존 세팅 된 x파일을 새로 익스포트 시 기존값이 존재하거나, 불필요한 파일이 쌓입니다 ( 수정함 )
			//else
			//{
			//	m_mapSetting.insert ( std::make_pair(strName,sSetting) );
			//}
		}
		return;
	}
	else if ( 0x0104==dwVer )
	{
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;
			SFile >> sSetting.m_bCameraCollision;
			
			DWORD dwDecal;
			SFile >> dwDecal;
			sSetting.m_emDecalType = static_cast<NSMaterialSetting::DECAL_TYPE>( dwDecal );

			SFile >> sSetting.m_nDecalOrder;

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}

			//// - 형상 탭 Material Setting 목록 갱신이 안됨 : 기존 세팅 된 x파일을 새로 익스포트 시 기존값이 존재하거나, 불필요한 파일이 쌓입니다 ( 수정함 )
			//else
			//{
			//	m_mapSetting.insert ( std::make_pair(strName,sSetting) );
			//}
		}
		return;
	}
	else if ( 0x0103==dwVer )
	{
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;
			SFile >> sSetting.m_bCameraCollision;

			DWORD dwDecal;
			SFile >> dwDecal;
			sSetting.m_emDecalType = static_cast<NSMaterialSetting::DECAL_TYPE>( dwDecal );

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}

			//// - 형상 탭 Material Setting 목록 갱신이 안됨 : 기존 세팅 된 x파일을 새로 익스포트 시 기존값이 존재하거나, 불필요한 파일이 쌓입니다 ( 수정함 )
			//else
			//{
			//	m_mapSetting.insert ( std::make_pair(strName,sSetting) );
			//}
		}
		return;
	}
	else if ( 0x0102==dwVer )
	{
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;
			SFile >> sSetting.m_bCameraCollision;

			BOOL bDecal(FALSE);
			SFile >> bDecal;
			sSetting.m_emDecalType = bDecal ? NSMaterialSetting::DECAL_TYPE_ALPHABLEND : NSMaterialSetting::DECAL_TYPE_NULL;

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}

			//// - 형상 탭 Material Setting 목록 갱신이 안됨 : 기존 세팅 된 x파일을 새로 익스포트 시 기존값이 존재하거나, 불필요한 파일이 쌓입니다 ( 수정함 )
			//else
			//{
			//	m_mapSetting.insert ( std::make_pair(strName,sSetting) );
			//}
		}
		return;
	}
	else if ( 0x0101==dwVer )
	{
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;
			SFile >> sSetting.m_bCameraCollision;

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}

			//// - 형상 탭 Material Setting 목록 갱신이 안됨 : 기존 세팅 된 x파일을 새로 익스포트 시 기존값이 존재하거나, 불필요한 파일이 쌓입니다 ( 수정함 )
			//else
			//{
			//	m_mapSetting.insert ( std::make_pair(strName,sSetting) );
			//}
		}
		return;
	}
	else if ( 0x0100==dwVer )
	{
		TSTRING strName;
		NSMaterialSetting::SETTING sSetting;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> sSetting.m_bCastShadow;

			NSMaterialSetting::MAP_SETTING_ITER iter = m_mapSetting.find( strName );
			if ( iter != m_mapSetting.end() )
			{
				(*iter).second = sSetting;
			}

			//// - 형상 탭 Material Setting 목록 갱신이 안됨 : 기존 세팅 된 x파일을 새로 익스포트 시 기존값이 존재하거나, 불필요한 파일이 쌓입니다 ( 수정함 )
			//else
			//{
			//	m_mapSetting.insert ( std::make_pair(strName,sSetting) );
			//}
		}
		return;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
}
