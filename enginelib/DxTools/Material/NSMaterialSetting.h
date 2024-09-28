#pragma once

#include "../../SigmaCore/File/SerialFile.h"
#include "../DxVertexFVF.h"

namespace NSMaterialSetting
{
	enum UNLIGHTMAP_TYPE
	{
		RENDER_TYPE_LIGHTMAP = 0,
		RENDER_TYPE_AMBIENTCUBE = 1,
		RENDER_TYPE_VOXELCOLOR = 2,
	};

	enum DECAL_TYPE
	{
		DECAL_TYPE_NULL = 0,
		DECAL_TYPE_ALPHABLEND = 1,
		DECAL_TYPE_ALPHATEST = 2,
		DECAL_TYPE_MULTIPLY = 3,
		DECAL_TYPE_ADD = 4,
	};

	struct SETTING
	{
		BOOL			m_bCastShadow;
		BOOL			m_bCameraCollision;
		BOOL			m_bCubeMap;
		float			m_fCubeMapValue;
		float			m_fSpotSpecPower;
		float			m_fSpotSpecMulti;
		BOOL			m_bMaskCubeMapSpecularMap;
		UNLIGHTMAP_TYPE	m_emUnLightMapType;
		DECAL_TYPE		m_emDecalType;
		int				m_nDecalOrder;
		BOOL			m_bDecalNormal;
		BOOL			m_bNormalMap;

		SETTING()
			: m_bCastShadow(TRUE)
			, m_bCameraCollision(TRUE)
			, m_bCubeMap(FALSE)
			, m_fCubeMapValue(0.1f)
			, m_fSpotSpecPower(256.f)
			, m_fSpotSpecMulti(0.f)
			, m_bMaskCubeMapSpecularMap(FALSE)
			, m_emUnLightMapType(RENDER_TYPE_LIGHTMAP)
			, m_emDecalType(DECAL_TYPE_NULL)
			, m_nDecalOrder(1)
			, m_bDecalNormal(FALSE)
			, m_bNormalMap(FALSE)
		{
		}

		DWORD GetFlag( BOOL bLightMapUV_Empty )  const;
	};
	typedef std::map<TSTRING,SETTING>	MAP_SETTING;
	typedef MAP_SETTING::iterator		MAP_SETTING_ITER;
	typedef MAP_SETTING::const_iterator	MAP_SETTING_CITER;
};

class CMaterialSetting
{
private:
	enum
	{
		VERSION = 0x111,
	};

	NSMaterialSetting::MAP_SETTING	m_mapSetting;

public:
	// ���� ����
	void CleanUp();

	// Default Material Setting �� ���� Material �̸����� �Է��س��´�.
	void Insert( const VEC_TSTRING& vecMaterialName );

	// Material ����
	size_t MaterialSize();

	// Material ����
	BOOL GetMaterial( size_t nID, TSTRING& strName, NSMaterialSetting::SETTING& sSetting );

	// Material ����
	const NSMaterialSetting::SETTING* GetMaterialSetting( const TSTRING& strName )  const;

	// strName Material �� ������ sSetting���� �����Ѵ�.
	void SetMaterialSetting( TSTRING& strName, NSMaterialSetting::SETTING& sSetting );

	void SaveSet( sc::SerialFile& SFile );
	void LoadSet( sc::SerialFile& SFile );

//private:
//	friend class CTextureSetDlg;

public:
	CMaterialSetting();
	~CMaterialSetting();
};
