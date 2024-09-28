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

		D3DXVECTOR3	m_avCombine[TEST_COUNT];			// RAD ���� ���̰ų� ��Ż m_fBright �� ���ϴµ� ���δ�.

		D3DXVECTOR3 m_avDirectColor[TEST_COUNT];	// PS_2_0, PS_3_0 ���Ͽ��� ����.

		// AmbientCube �� ����. PS_3_0
		D3DXVECTOR3 m_avIndirectColor[TEST_COUNT];			// PS_3_0 ���� IndirectLight. DirectionLight�� �������� �Ϻ� SpotLight�� �������� ������ ��.
		float m_afSpot1Color[TEST_COUNT];					// PS_3_0 ���� spotlight �� ���� ����.
		float m_afSpot2Color[TEST_COUNT];					// PS_3_0 ���� spotlight �� ���� ����.
		D3DXVECTOR3 m_avColorNoDynamicLight[TEST_COUNT];	// PS_3_0 ���� spotlight �� ���� ����.	// Don`t Save. m_avIndirectColor�� ����� ���� ������.
		D3DXVECTOR3 m_avColorDirectionDirect[TEST_COUNT];	// TnL_PS_3_0_NM ���� ����.
		
		D3DXVECTOR3 m_vVexelColor;

		// VoxelColor �� ����. PS_3_0
		D3DXVECTOR3	m_vIndirectColorVC_HIGH;			// PS_3_0 ���� IndirectLight. TnL_PS_3_0_NM ���� ����. �Ϻ� SpotLight�� �������� ������ ��.
		D3DXVECTOR3	m_vIndirectColorVC_HIGH_HIGH;		// PS_3_0 ���� IndirectLight. DirectionLight�� �������� �Ϻ� SpotLight�� �������� ������ ��.
		float		m_fSpot1ColorVC;					// PS_3_0 ���� spotlight �� ���� ����.
		float		m_fSpot2ColorVC;					// PS_3_0 ���� spotlight �� ���� ����.
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
	D3DXVECTOR4			m_vVoxelColor;											// Save, Load �Ǵ� ������	(x,y,z ��)
	D3DXVECTOR4			m_vIndirectVC_HIGH;										// Save, Load �Ǵ� ������	(x,y,z ��)
	D3DXVECTOR4			m_vIndirectVC_HIGH_HIGH;								// Save, Load �Ǵ� ������	(x,y,z ��)
	D3DXVECTOR2			m_vVC_Spot1_Spot2;										// Save, Load �Ǵ� ������	(x,y ��)
	D3DXVECTOR3			m_avCombineColor[LightMapBoxColor::TEST_COUNT];			// Save, Load �Ǵ� ������	(x,y,z ��)
	D3DXVECTOR3			m_avIndirectColor[LightMapBoxColor::TEST_COUNT];		// Save, Load �Ǵ� ������	(x,y,z ��)
	D3DXVECTOR3			m_avDirectionDirectColor[LightMapBoxColor::TEST_COUNT];	// Save, Load �Ǵ� ������	(x,y,z ��)
	float				m_afSpot1Color[LightMapBoxColor::TEST_COUNT];			// Save, Load �Ǵ� ������
	float				m_afSpot2Color[LightMapBoxColor::TEST_COUNT];			// Save, Load �Ǵ� ������
	D3DXVECTOR4			m_avAmbientCube[LightMapBoxColor::TEST_COUNT];
	D3DXVECTOR4			m_avIndirectAC_HIGH[LightMapBoxColor::TEST_COUNT];
	D3DXVECTOR4			m_avIndirectAC_HIGH_HIGH[LightMapBoxColor::TEST_COUNT];

public:
	// RAD ������ ȣ��ȴ�.
	void FrameMoveRAD( D3DXVECTOR4& vVoxelColorUsed, D3DXVECTOR4 avAmbientCube[] );

	// Piece ������ ������ 1fps �� 1������ ȣ��ǵ��� �Ǿ��ִ�.
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