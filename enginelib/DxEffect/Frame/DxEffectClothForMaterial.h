// DxEffectClothForMaterial.h: interface for the DxEffectAniso class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../../DxTools/DxSpring.h"
#include "../../DxTools/DxWind.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "./DxEffectFrame.h"

struct DxFrame;
struct Material;
struct MaterialSmall;
struct MATERIAL_DATA;
struct PieceParameter;
class CMaterialSetting;

struct CLOTH_FOR_MATERIAL_PROPERTY		// Ver.104
{
	BOOL			m_bPlay;
	BOOL			m_bEachRender;
	int				m_iNodeCount;
	float			m_fCoEfficient;			//탄성 계수    
	D3DXVECTOR3		m_vWindDirection;		//바람의 방향 벡터 
	float			m_fWindForce;			//바람의 세기 	
	float			m_fGravity;				//중력 부분	
	char			m_szTexture[MAX_PATH];

	CLOTH_FOR_MATERIAL_PROPERTY()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	}
};


class DxEffectClothForMaterial : public DxEffectBase
{
	//	Note : 이펙트 타입 정의.
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }
	virtual BOOL  GetEachRender()	{ return m_bEachRender; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );

public:
	struct PARTICLE
	{		
		D3DXVECTOR3   vPosition;
		D3DXVECTOR3   vVelocity;
		D3DXVECTOR3   vAccelerate;
		D3DXVECTOR3   vForce;
		BOOL          bLocked;
	};

	struct NODE
	{
		WORD Index1;
		WORD Index2;
		float fLength;
	};

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			CLOTH_FOR_MATERIAL_PROPERTY	m_Property;
		};

		struct
		{
			BOOL			m_bPlay;
			BOOL			m_bEachRender;
			int				m_iNodeCount;
			float			m_fCoEfficient;          //탄성 계수    			
			D3DXVECTOR3		m_vWindDirection;		 //바람의 방향 벡터 
			float			m_fWindForce;            //바람의 세기 			
			float			m_fGravity;				 //중력 부분	
			char			m_szTexture[MAX_PATH];
		};
	};

public:
	void SetProperty ( CLOTH_FOR_MATERIAL_PROPERTY& Property )
	{
		m_Property = Property;
	}
	CLOTH_FOR_MATERIAL_PROPERTY& GetProperty () { return m_Property; }

public:
	DWORD		m_dwNumLockIndex;
	WORD		*m_pLockIndex;

protected:
	LPD3DXMESH	m_pLocalMeshSRC;	// 원본
	DWORD		m_dwNumVertices;
	DWORD		m_dwNumIndices;

	BYTE*						m_pVertArray;	// 변동
	DWORD						m_dwVer;		// Material 변경이 있으면 버젼이 올라가서 Mesh를 새로 업데이트 한다.
	std::vector<D3DXVECTOR2>	m_vecLightMapUV;
	const MaterialSmall*		m_rMaterialSmall;	// 참조값.

	TextureResource		m_pMaskTexRes;

	NODE		*m_pNodeTable;
	PARTICLE	*m_pParticle;

	WORD		*m_pTempLockIndex;

	DxSpring	m_Spring;
	DxWind		m_Wind;      //바람 클래스 

	float		m_fTime;	

	int			m_iWindMin;	
	D3DXMATRIX	m_matWorld;
	float		m_fWindConstant;

	float		m_fSumTime;

	//BOOL				m_bAlphaMap;
	TextureResource m_textureRes;
	BOOL		m_bCheckTex;

	TSTRING		m_strMaterialName;
	BOOL		m_bVertexColor;

	DWORD		m_dwFlags;
	float		m_fCubeMapValue;
	float		m_fSpotSpecPower;
	float		m_fSpotSpecMulti;

protected:
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

	void ThreadLoadingTexture ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : 
	//
public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	void XFileLoad( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pLocalMesh, BOOL bLightMapUV, DWORD dwLightMapIndex );
	void CreateNodeTable( D3DXVECTOR3 *pVertices, DWORD dwNumIndices, WORD *pIndices );
	void SetParticle( int index, D3DXVECTOR3 &vPosition );
	BOOL CreateVertex( DWORD dwNumVertices, D3DXVECTOR3* pVertices );	
	void DeleteClothData( int a );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );	
	virtual HRESULT RenderMAT ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, PieceParameter* pParameter, BOOL bMPShading, BOOL bReflect );
	virtual HRESULT RenderMATForPSF ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, PieceParameter* pParameter );
	virtual void	RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	void RefreshMesh( LPDIRECT3DDEVICEQ pd3dDevice, const Material* pMaterial, const MaterialSmall* pMaterialSmall );
	void RenderTool( LPDIRECT3DDEVICEQ pd3dDevice, MATERIAL_DATA sMaterial_Data, const CMaterialSetting* pMaterialSetting, const MaterialSmall* pMaterialSmall );

	void RenderGameRoot( LPDIRECT3DDEVICEQ pd3dDevice, 
						MATERIAL_DATA sMaterial_Data, 
						const MaterialSmall* pMaterialSmall,
						NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode,
						PieceParameter* pParameter,
						BOOL bLightMapTest, 
						BOOL bMPShading,
						BOOL bPSF );

	void RenderGame( LPDIRECT3DDEVICEQ pd3dDevice, 
					MATERIAL_DATA sMaterial_Data, 
					const MaterialSmall* pMaterialSmall,
					LPD3DXEFFECT pFX,
					NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode,
					PieceParameter* pParameter,
					size_t unPass,
					BOOL bPSF );

protected:
	HRESULT MeshUpdate ( D3DVERTEXELEMENT9* pELMT );
	void ComputeNormal(int nVertices,  BYTE *pVertices, DWORD dwVertSize, DWORD dwNormalOffset, int nIndices, WORD *pIndices  );
	//inline  float Random( int nMin, int nMax );

public:
	void GetIndexLock();
	DWORD GetdwNumLockIndex() { return m_dwNumLockIndex; }
	char* GetStrList( DWORD i );
	DWORD GetdwVertex( DWORD i );
	NODE* GetNodeTable () { return m_pNodeTable; }
	int GetNodeTableSize () { return m_iNodeCount; }

public:
	void SetIndexLock();
	void BaseBuffToDynamicBuff ( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetParticleLock( int index , BOOL bLook );	

private:
	void CreateVertexAndNodeTable( LPDIRECT3DDEVICEQ pd3dDevice, const LPD3DXMESH pLocalMesh );

public:
	DxEffectClothForMaterial();
	virtual ~DxEffectClothForMaterial();

protected:
	virtual void SaveBuffer ( sc::SerialFile &SFile );
	virtual void LoadBufferSet ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneDataMAT ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice, const CMaterialSetting* pMaterialSetting );

	//	Note : OBJAABB
	//
public:
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
};
