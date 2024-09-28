#pragma once

#include <vector>
#include "../DxVertexFVF.h"
#include "../Material/MaterialFieldDef.h"

class DxShadowLightNode;
struct DxShadowSpotLight;
struct CLIPVOLUME;

namespace sc
{
	class SerialFile;
};

// 실시간 그림자를 생성할 수 있는 SpotLight 정보들
struct ShadowLightData
{
	enum
	{
		VERSION = 0x102,
	};

	BOOL													m_bDirectionLight;
	std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>	m_vecShadowSpotLight;
	SET_DWORD												m_setEraseLightID;

	void CleanUp();
	void Import( const ShadowLightData* pData, const SET_DWORD& setEraseLightID );

	// Edit
	void Insert( std::tr1::shared_ptr<DxShadowSpotLight> spShadowSpotLight );
	void ResetID();
	void Delete( int nIndex );
	BOOL LightItemUp( int nIndex );
	BOOL LightItemDown( int nIndex );
	void Clone_MouseShift( LPD3DXMATRIX& pMatrix );

	// Save & Load
	void	Save ( sc::SerialFile &SFile );
	void	Load ( sc::SerialFile &SFile );

	ShadowLightData();
};

namespace NSShadowLight
{
	extern BOOL g_bRangeRender;

	// Edit
	const std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>& GetVecShadowSpotLight();

	// Game
	void SetActive( const ShadowLightData* pData );
	void MakeAABBTree();
	void Insert_mapDistanceLightID( float fDistance, DWORD dwID );
	void FrameMoveTool( float fElapsedTime );
	void FrameMove( float fElapsedTime );
	DWORD GetLightNumber();
	DWORD GetLightNumberALL();
	const DxShadowSpotLight* GetShadowSpotLight( int nIndex );
	const DxShadowSpotLight* GetShadowSpotLightALL( DWORD dwArrayID );
	void CleanUp();
	BOOL IsCascadeShadow();

	// 지형에 속한 Spot 들의 정보를 모아놓은 것 삭제함.
	// 실제 Spot 데이터를 지우는 것이 아님.
	void ClearRenderSpotID();

	void PreRender( const D3DXMATRIX& matView );
};

namespace NSShadowLightUtil
{
	DWORD GetPassFX_PS_3_0_TYPE_USED( PS_3_0_TYPE emPS_3_0_Type, WORDWORD wwLightID );
	int GetPassFX_PS_3_0_TYPE_USED_MPShading( PS_3_0_TYPE emPS_3_0_Type, WORDWORD wwLightID );
};