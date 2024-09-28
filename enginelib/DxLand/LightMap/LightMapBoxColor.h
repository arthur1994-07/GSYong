#pragma once

struct WORDWORD;
class DxLandMan;
class DxPieceManager;
class DxStaticMeshCollBase;

namespace sc
{
	class SerialFile;
};

class LightMapBoxColor
{
public:
	enum
	{
		CALCULATE_COUNT = 3,
		TEST_COUNT = 6,
		SAMPLING_LINE = 4,	// 4x4
	};

private:
	struct DAYNIGHT_BOX
	{
		D3DXVECTOR3 m_avPos[TEST_COUNT][SAMPLING_LINE*SAMPLING_LINE];
		D3DXVECTOR3 m_avNor[TEST_COUNT];

		D3DXVECTOR3	m_avCombine[TEST_COUNT];			// RAD 에서 쓰이거나 토탈 m_fBright 을 구하는데 쓰인다.

		D3DXVECTOR3 m_avDirectColor[TEST_COUNT];	// PS_2_0, PS_3_0 이하에서 쓰임.

		// AmbientCube 를 위함. PS_3_0
		D3DXVECTOR3 m_avIndirectColor[TEST_COUNT];			// PS_3_0 에서 IndirectLight. DirectionLight의 직접광과 일부 SpotLight의 직접광을 제외한 것.
		float m_afSpot1Color[TEST_COUNT];					// PS_3_0 에서 spotlight 를 위해 쓰임.
		float m_afSpot2Color[TEST_COUNT];					// PS_3_0 에서 spotlight 를 위해 쓰임.
		D3DXVECTOR3 m_avColorNoDynamicLight[TEST_COUNT];	// PS_3_0 에서 spotlight 를 위해 쓰임.	// Don`t Save. m_avIndirectColor를 만들기 위해 존재함.
		D3DXVECTOR3 m_avColorDirectionDirect[TEST_COUNT];	// TnL_PS_3_0_NM 에서 쓰임.
		
		D3DXVECTOR3 m_vVexelColor;

		// VoxelColor 를 위함. PS_3_0
		D3DXVECTOR3	m_vIndirectColorVC_HIGH;			// PS_3_0 에서 IndirectLight. TnL_PS_3_0_NM 에서 쓰임. 일부 SpotLight의 직접광을 제외한 것.
		D3DXVECTOR3	m_vIndirectColorVC_HIGH_HIGH;		// PS_3_0 에서 IndirectLight. DirectionLight의 직접광과 일부 SpotLight의 직접광을 제외한 것.
		float		m_fSpot1ColorVC;					// PS_3_0 에서 spotlight 를 위해 쓰임.
		float		m_fSpot2ColorVC;					// PS_3_0 에서 spotlight 를 위해 쓰임.
	};

	DAYNIGHT_BOX m_sDayNightBox;

public:
	void LightMapCalculationBoxColor( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshCollBase* pStaticMesh, DxPieceManager* pPieceManager, const WORDWORD& wwLight );
	void BakeRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, LPDIRECT3DTEXTUREQ pTextureSystem, float fReflectance, float fFogBlend, DWORD dwFogLength );
	void AddRadiosityColorForVoxel();
	const D3DXVECTOR3* GetCombineColor() const			{ return &m_sDayNightBox.m_avCombine[0]; }
	const D3DXVECTOR3* GetIndirectColor() const			{ return &m_sDayNightBox.m_avIndirectColor[0]; }
	const D3DXVECTOR3* GetDirectionDirectColor() const	{ return &m_sDayNightBox.m_avColorDirectionDirect[0]; }
	const float* GetSpot1Color() const					{ return &m_sDayNightBox.m_afSpot1Color[0]; }
	const float* GetSpot2Color() const					{ return &m_sDayNightBox.m_afSpot2Color[0]; }
	const D3DXVECTOR3& GetColorVoxel() const			{ return m_sDayNightBox.m_vVexelColor; }
	const D3DXVECTOR3& GetIndirectVC_HIGH() const		{ return m_sDayNightBox.m_vIndirectColorVC_HIGH; }
	const D3DXVECTOR3& GetIndirectVC_HIGH_HIGH() const	{ return m_sDayNightBox.m_vIndirectColorVC_HIGH_HIGH; }
	const float GetSpot1ColorVC() const					{ return m_sDayNightBox.m_fSpot1ColorVC; }
	const float GetSpot2ColorVC() const					{ return m_sDayNightBox.m_fSpot2ColorVC; }

private:
	void SetVoxelColor_LightMap( DAYNIGHT_BOX& sDataBox );

public:
	LightMapBoxColor( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const D3DXMATRIX& matWorld );
	LightMapBoxColor( LightMapBoxColor* pSrc );
	~LightMapBoxColor();
};

class LightMapBoxColorFullData
{
private:
	enum
	{
		VERSION = 0x104,
	};

public:
	D3DXVECTOR4			m_vVoxelColor;											// Save, Load 되는 데이터	(x,y,z 만)
	D3DXVECTOR4			m_vIndirectVC_HIGH;										// Save, Load 되는 데이터	(x,y,z 만)
	D3DXVECTOR4			m_vIndirectVC_HIGH_HIGH;								// Save, Load 되는 데이터	(x,y,z 만)
	D3DXVECTOR2			m_vVC_Spot1_Spot2;										// Save, Load 되는 데이터	(x,y 만)
	D3DXVECTOR3			m_avCombineColor[LightMapBoxColor::TEST_COUNT];			// Save, Load 되는 데이터	(x,y,z 만)
	D3DXVECTOR3			m_avIndirectColor[LightMapBoxColor::TEST_COUNT];		// Save, Load 되는 데이터	(x,y,z 만)
	D3DXVECTOR3			m_avDirectionDirectColor[LightMapBoxColor::TEST_COUNT];	// Save, Load 되는 데이터	(x,y,z 만)
	float				m_afSpot1Color[LightMapBoxColor::TEST_COUNT];			// Save, Load 되는 데이터
	float				m_afSpot2Color[LightMapBoxColor::TEST_COUNT];			// Save, Load 되는 데이터
	D3DXVECTOR4			m_avAmbientCube[LightMapBoxColor::TEST_COUNT];
	D3DXVECTOR4			m_avIndirectAC_HIGH[LightMapBoxColor::TEST_COUNT];
	D3DXVECTOR4			m_avIndirectAC_HIGH_HIGH[LightMapBoxColor::TEST_COUNT];

public:
	// RAD 렌더시 호출된다.
	void FrameMoveRAD( D3DXVECTOR4& vVoxelColorUsed, D3DXVECTOR4 avAmbientCube[] );

	// Piece 갯수는 많지만 1fps 에 1개씩만 호출되도록 되어있다.
	void FrameMoveStep( D3DXVECTOR4** ppAmbientCube, 
						float** ppSpot1Color, 
						float** ppSpot2Color,
						D3DXVECTOR4** ppVoxelColor,
						D3DXVECTOR2** ppSpot1_Spot2 );

	void LightMapCalculationBoxColor( LightMapBoxColor* pLightMapBoxColor );
	void AddRadiosityColorForVoxel( LightMapBoxColor* pLightMapBoxColor );

	void Import( LightMapBoxColorFullData* pSrc );

	//////////////////////////////////////////////////////////////////////////
	const D3DXVECTOR4* GetAmbientCube() const;

private:
	void SetIndirectVCAC();

public:
	void Save( sc::SerialFile& SFile );
	void Load( sc::SerialFile& SFile );
	void LoadPrev( sc::SerialFile& SFile );
	void LoadPrev2( sc::SerialFile& SFile );

public:
	LightMapBoxColorFullData();
	~LightMapBoxColorFullData();
};