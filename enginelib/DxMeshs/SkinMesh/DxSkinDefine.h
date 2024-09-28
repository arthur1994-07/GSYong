#pragma once

#define USE_ANI_QUATERNION	// Skin.. Use Quaternion ~!!
//#define USE_SKINMESH_LOD	// SkinMesh LOD

struct SKIN_SPEC_SSS_INFO
{
	D3DXVECTOR4		m_vFaceSpecColor_Power;		// xyz-Color, w-Power
	D3DXVECTOR4		m_vFaceSpec2Color_Power;	// xyz-Color, w-Power
	D3DXVECTOR4		m_vBodySpecColor_Power;		// xyz-Color, w-Power
	D3DXVECTOR4		m_vBodySpec2Color_Power;	// xyz-Color, w-Power
	D3DXVECTOR4		m_vSSS_Color;				// xyz-Color, w-Disable

	SKIN_SPEC_SSS_INFO()
		: m_vFaceSpecColor_Power( 0.113f, 0.070f, 0.043f, 64.f )		//29,18,11
		, m_vFaceSpec2Color_Power( 0.000f, 0.000f, 0.000f, 1.f )		//
		, m_vBodySpecColor_Power( 0.072f, 0.054f, 0.044f, 64.f )		//
		, m_vBodySpec2Color_Power( 0.360f, 0.270f, 0.227f, 512.f )	//92,69,58
		, m_vSSS_Color( 0.1f, 0.f, 0.f, 1.f )
	{
	}
};

struct SKIN_SPEC_SSS_INFO_REFERENCE
{
	const D3DXVECTOR4*		m_rSpecColor_Power;		// xyz-Color, w-Power
	const D3DXVECTOR4*		m_rSpec2Color_Power;	// xyz-Color, w-Power
	const D3DXVECTOR4*		m_rSSS_Color;			// xyz-Color, w-Disable

	SKIN_SPEC_SSS_INFO_REFERENCE( const D3DXVECTOR4* pSpecColor_Power, const D3DXVECTOR4* pSpec2Color_Power, const D3DXVECTOR4* pSSS_Color )
		: m_rSpecColor_Power(pSpecColor_Power)
		, m_rSpec2Color_Power(pSpec2Color_Power)
		, m_rSSS_Color(pSSS_Color)
	{
	}
};