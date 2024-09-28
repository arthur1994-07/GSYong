#pragma once

#include <vector>
#include <boost/tr1/memory.hpp>

#include "../TextureManager.h"

struct Material;
struct MaterialSmall;
struct MATERIAL_DATA;

class MaterialMesh
{
private:
	DWORD						m_dwVer;		// Material 변경이 있으면 버젼이 올라가서 Mesh를 새로 업데이트 한다.
	std::vector<D3DXVECTOR2>	m_vecLightMapUV;

	LPD3DXMESH					m_pMesh;		// 기본적인 렌더링 시

	DWORD						m_dwVertices;	// Instancing Rendering 시
	DWORD						m_dwFaces;		// Instancing Rendering 시
	DWORD						m_dwVertSize;	// Instancing Rendering 시
	LPDIRECT3DVERTEXBUFFER9		m_rVB;
	LPDIRECT3DINDEXBUFFER9		m_rIB;

public:
	void Insert( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMeshSRC, LPD3DXATTRIBUTERANGE pAttribTable, DWORD dwAttIndex );

	BOOL Insert( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMeshSRC );

	void SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate );

	BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, 
				const MATERIAL_DATA& sMaterial_Data,
				std::tr1::shared_ptr<MaterialMesh> spSrcMesh );

	BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, 
				const MATERIAL_DATA& sMaterial_Data,
				LPD3DXMESH pOcMesh );

	BOOL RenderPiece_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, 
								ID3DXEffect* pFX, 
								const MATERIAL_DATA& sMaterial_Data,
								LPD3DXMESH pOcMesh );

public:
	MaterialMesh();
	~MaterialMesh();
};

class MaterialMeshArray
{
private:
	typedef std::vector< std::tr1::shared_ptr<MaterialMesh> >	VEC_MATERIALMESH;
	typedef VEC_MATERIALMESH::iterator						    VEC_MATERIALMESH_ITER;
	typedef VEC_MATERIALMESH::const_iterator				    VEC_MATERIALMESH_CITER;
	typedef VEC_MATERIALMESH::value_type						VEC_MATERIALMESH_TYPE;
	VEC_MATERIALMESH    m_vecMaterialMesh;
	VEC_MATERIALMESH    m_vecMaterialMeshBackUp;

public:
	void Insert( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMeshSRC );

	BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, 
				DWORD dwAttIndex, 
				const MATERIAL_DATA& sMaterial_Data,
				DWORD dwPass,
				BOOL bForwardPass );	// bForwardPass : TRUE - Pass2,Water,CustomAlpha 등 Deffered 로 렌더링 안하는 것들..

public:
	MaterialMeshArray();
	~MaterialMeshArray();
};

namespace NSMATERIALMESH
{
	extern int g_nCreateCount;	// WorldEdit 에서 확인용 데이터

	BOOL CreateMesh
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		const Material* pMaterial, 
		const MaterialSmall* pMaterialSmall, 
		const std::vector<D3DXVECTOR2>& vecLightMapUV, 
		LPD3DXMESH pSrcMesh, 
		LPD3DXMESH& pMeshOUT 
	);
};