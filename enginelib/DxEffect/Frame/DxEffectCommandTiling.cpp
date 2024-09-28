#include "pch.h"

#include "../../DxMeshs/SkinMesh/DxMaterial.h"
#include "../../DxTools/TextureManager.h"

#include "DxEffectCommandTiling.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//-------------------------------------------------------------------------
// 부모클래스
CommandTiling::CommandTiling()
{
}

CommandTiling::~CommandTiling()
{
}

//-------------------------------------------------------------------------
// Composite Command - 다중으로 가질 수 있도록 함.
CommandTiling_Composite::CommandTiling_Composite()
{
}

CommandTiling_Composite::~CommandTiling_Composite()
{
    m_vecspCommand.clear();
}

// Undo
void CommandTiling_Composite::Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling )
{
    for ( DWORD i=0; i<m_vecspCommand.size(); ++i )
    {
        m_vecspCommand[i]->Undo( pd3dDevice, pEffectTiling );
    }
}

// Redo
void CommandTiling_Composite::Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling )
{
    for ( DWORD i=0; i<m_vecspCommand.size(); ++i )
    {
        m_vecspCommand[i]->Redo( pd3dDevice, pEffectTiling );
    }
}

// 작업된다.
void CommandTiling_Composite::Insert( std::tr1::shared_ptr<CommandTiling> spCommand )
{
    m_vecspCommand.push_back( spCommand );
}

//-------------------------------------------------------------------------
// 매터리얼을 변경한다.
CommandTiling_MaterialChange::CommandTiling_MaterialChange( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling::MATERIALMAP* pmapMaterial, DWORD dwPointEX )
{
	m_dwPointEX = dwPointEX;
    SetMaterial( pd3dDevice, &m_mapMaterials_Prev, pmapMaterial );
}

CommandTiling_MaterialChange::~CommandTiling_MaterialChange()
{
}

// Undo
void CommandTiling_MaterialChange::Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling )
{
    SetMaterial( pd3dDevice, &pEffectTiling->m_mapMaterials, &m_mapMaterials_Prev );
}

// Redo
void CommandTiling_MaterialChange::Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling )
{
    SetMaterial( pd3dDevice, &pEffectTiling->m_mapMaterials, &m_mapMaterials );
}

// New
void CommandTiling_MaterialChange::SetMaterial( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling::MATERIALMAP* pmapMaterial )
{
    SetMaterial( pd3dDevice, &m_mapMaterials, pmapMaterial );
}

// SetMaterial
void CommandTiling_MaterialChange::SetMaterial( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling::MATERIALMAP* pmapMaterialDest, DxEffectTiling::MATERIALMAP* pmapMaterialSrc )
{
    (*pmapMaterialDest).clear();

    DxEffectTiling::MATERIALMAP_ITER iter		= (*pmapMaterialSrc).begin();
    DxEffectTiling::MATERIALMAP_ITER iter_end	= (*pmapMaterialSrc).end();

    for ( ; iter!=(*pmapMaterialSrc).end(); ++iter )
    {
        std::tr1::shared_ptr<DxEffectTiling::MATERIALEX> spMaterials( new DxEffectTiling::MATERIALEX(m_dwPointEX) );
        spMaterials->strTexture = ((*iter).second)->strTexture;

        spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
            spMaterials->strTexture,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_EFFECT,
			false,
			FALSE );

        if( spMaterials->m_textureRes.GetTexture() )
            spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// 알파 쓰는지 안쓰는지 체크

        // listFace
        {
            DxEffectTiling::FACELIST_ITER _iter = (*iter).second->listFace.begin();
            for ( ; _iter!=(*iter).second->listFace.end(); ++_iter )
            {
                spMaterials->listFace.push_back ( (*_iter) );
            }
        }

        // listTriangle
        {
            DxEffectTiling::TRIANGLELIST_ITER _iter = (*iter).second->listTriangle.begin();
		    for ( ; _iter!=(*iter).second->listTriangle.end(); ++_iter )
		    {
                spMaterials->listTriangle.push_back ( (*_iter) );
		    }
        }

        (*pmapMaterialDest).insert ( std::make_pair((*iter).first,spMaterials) );		// 삽입
    }
}


//-------------------------------------------------------------------------
// 타일에 그림을 변경한다.
CommandTiling_TileChange::CommandTiling_TileChange( DWORD dwSize, DxEffectTiling::MATERIALMAP* pmapMaterials_Prev, DxEffectTiling::MATERIALMAP* pmapMaterials_Now )
{
    m_dwSize = dwSize;

	DxEffectTiling::MATERIALMAP_ITER iter = pmapMaterials_Prev->begin();
	for ( ; iter != pmapMaterials_Prev->end(); ++iter )
	{
		std::tr1::shared_ptr<DxEffectTiling::MATERIALEX> spMaterials( new DxEffectTiling::MATERIALEX(m_dwSize) );
		memcpy( spMaterials->m_pPointUseColor, (*iter).second->m_pPointUseColor, sizeof(DxEffectTiling::POINT_USE_COLOR)*m_dwSize );
		spMaterials->listTriangle = (*iter).second->listTriangle;
		m_mapMaterials_Prev.insert( std::make_pair( (*iter).first, spMaterials ) );
	}

	iter = pmapMaterials_Now->begin();
	for ( ; iter != pmapMaterials_Now->end(); ++iter )
	{
		std::tr1::shared_ptr<DxEffectTiling::MATERIALEX> spMaterials( new DxEffectTiling::MATERIALEX(m_dwSize) );
		memcpy( spMaterials->m_pPointUseColor, (*iter).second->m_pPointUseColor, sizeof(DxEffectTiling::POINT_USE_COLOR)*m_dwSize );
		spMaterials->listTriangle = (*iter).second->listTriangle;
		m_mapMaterials_Now.insert( std::make_pair( (*iter).first, spMaterials ) );
	}
}

CommandTiling_TileChange::~CommandTiling_TileChange()
{
}

// Undo
void CommandTiling_TileChange::Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling )
{
	DxEffectTiling::MATERIALMAP_ITER iter = m_mapMaterials_Prev.begin();
	for ( ; iter != m_mapMaterials_Prev.end(); ++iter )
	{
		DxEffectTiling::MATERIALMAP_ITER iter_ORIG = pEffectTiling->m_mapMaterials.find( (*iter).first );
		(*iter_ORIG).second->listTriangle = (*iter).second->listTriangle;
		memcpy( (*iter_ORIG).second->m_pPointUseColor, (*iter).second->m_pPointUseColor, sizeof(DxEffectTiling::POINT_USE_COLOR)*m_dwSize );
	}
}

// Redo
void CommandTiling_TileChange::Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling )
{
	DxEffectTiling::MATERIALMAP_ITER iter = m_mapMaterials_Now.begin();
	for ( ; iter != m_mapMaterials_Now.end(); ++iter )
	{
		DxEffectTiling::MATERIALMAP_ITER iter_ORIG = pEffectTiling->m_mapMaterials.find( (*iter).first );
		(*iter_ORIG).second->listTriangle = (*iter).second->listTriangle;
		memcpy( (*iter_ORIG).second->m_pPointUseColor, (*iter).second->m_pPointUseColor, sizeof(DxEffectTiling::POINT_USE_COLOR)*m_dwSize );
	}
}

//-------------------------------------------------------------------------
// 브러쉬 타일을 변경한다.
CommandTiling_SelectMaterial::CommandTiling_SelectMaterial( int nSelectMaterial_Prev )
    : m_nSelectMaterial_Prev( nSelectMaterial_Prev )
    , m_nSelectMaterial(0)
{
}

CommandTiling_SelectMaterial::~CommandTiling_SelectMaterial()
{
}

void CommandTiling_SelectMaterial::SelectMaterial( int nSelectMaterial )
{
    m_nSelectMaterial = nSelectMaterial;
}

// Undo
void CommandTiling_SelectMaterial::Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling )
{
    pEffectTiling->SetCurMaterial( m_nSelectMaterial_Prev );
}

// Redo
void CommandTiling_SelectMaterial::Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling )
{
    pEffectTiling->SetCurMaterial( m_nSelectMaterial );
}

//-------------------------------------------------------------------------
// Command 관리 클래스
CommandTilingManager::CommandTilingManager()
    : m_nIndex(-1)
{
}

CommandTilingManager::~CommandTilingManager()
{
    CleanUp();
}

// 정리
void CommandTilingManager::CleanUp()
{
    m_nIndex = -1;
    m_vecspCommand.clear();
}

// 작업된다.
void CommandTilingManager::Insert( std::tr1::shared_ptr<CommandTiling> spCommand )
{
    m_vecspCommand.resize(m_nIndex+1);
    m_vecspCommand.push_back( spCommand );
    m_nIndex = static_cast<int> (m_vecspCommand.size()-1);
}

// Undo
void CommandTilingManager::Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling )
{
    if ( m_nIndex < 0 )
        return;

    // 작업 후
    m_vecspCommand[m_nIndex]->Undo( pd3dDevice, pEffectTiling );
    --m_nIndex;
}

// Redo
void CommandTilingManager::Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling )
{
    if ( (m_nIndex+1) >= static_cast<int>(m_vecspCommand.size()) )
        return;

    // 작업 후
    ++m_nIndex;
    m_vecspCommand[m_nIndex]->Redo( pd3dDevice, pEffectTiling );;
}
