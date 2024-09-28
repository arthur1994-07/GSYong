#include "stdafx.h"

#include "../../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"

#include "./UIMeshData_SkinChar.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIMeshData_SkinChar::CUIMeshData_SkinChar ()
	: m_fTime(0.f)
{
	m_pSkinChar = NULL;

	m_bAttackMode = TRUE;
}

CUIMeshData_SkinChar::~CUIMeshData_SkinChar ()
{
    SAFE_DELETE(m_pSkinChar);
}

HRESULT CUIMeshData_SkinChar::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if ( !pd3dDevice )
    {
        return E_FAIL;
    }

    if ( m_pSkinChar )
    {
        m_pSkinChar->InitDeviceObjects( pd3dDevice );      
    }

    m_pd3dDevice = pd3dDevice;

    return S_OK;
}

HRESULT CUIMeshData_SkinChar::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if ( !pd3dDevice )
    {
        return E_FAIL;
    }

    if ( !m_pSkinChar )
    {
        return S_OK;        
    }

    return m_pSkinChar->RestoreDeviceObjects(pd3dDevice);
}

HRESULT CUIMeshData_SkinChar::InvalidateDeviceObjects ()
{
    if ( !m_pSkinChar )
    {
        return E_FAIL;        
    }

    return m_pSkinChar->InvalidateDeviceObjects();
}

HRESULT CUIMeshData_SkinChar::DeleteDeviceObjects ()
{
    if ( !m_pSkinChar )
    {
        return E_FAIL;        
    }

    return m_pSkinChar->DeleteDeviceObjects();
}

const BOOL CUIMeshData_SkinChar::Load ( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& szFile )
{
    if ( !m_pd3dDevice )
    {
        return FALSE;
    }

    DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( szFile.c_str(), pd3dDevice );
    if ( !pSkinChar )
        return E_FAIL;

    SAFE_DELETE(m_pSkinChar);
    m_pSkinChar = new DxSkinChar;

    if ( !m_pSkinChar )
    {
        return FALSE;
    }

    m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_JIGGLE_BONE );

    return TRUE;
}

void CUIMeshData_SkinChar::FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
{
    if ( !pd3dDevice )
    {
        return;
    }

	m_fTime += fElapsedTime;
    m_pSkinChar->FrameMove ( m_fTime, fElapsedTime, FALSE, m_matWorld, TRUE, FALSE, m_bAttackMode, FALSE );
}

void CUIMeshData_SkinChar::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if ( !pd3dDevice )
    {
        return;
    }

    // Note : Position 설정
    D3DXVECTOR3 vPosition;
    vPosition.x = m_matWorld._41;
    vPosition.y = m_matWorld._42;
    vPosition.z = m_matWorld._43;
    m_pSkinChar->SetPosition(vPosition);

    pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
    pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    D3DXMATRIXA16 matTrans(m_matWorld);

	DxViewPort::GetInstance().SetFromPt( m_vCameraFromPt );

    //m_pSkinChar->Render( pd3dDevice, TRUE, 1.f, matTrans );

	// View 와 Projection 등 셋팅.
	{
		NSMaterialSkinManager::FrameMove( m_matView, m_matProj );
	}

	// Render
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		m_pSkinChar->RenderFixed( pd3dDevice, TRUE, 1.f );
	}
	else
	{
		NSMaterialSkinManager::SetMob( FALSE );

		m_pSkinChar->SetPhysX_Cloth( FALSE );
		m_pSkinChar->RenderMaterialOpaque ( pd3dDevice, 1.f );
		m_pSkinChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
		m_pSkinChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
		m_pSkinChar->RenderMaterialEffect ( pd3dDevice, 1.f );
	}

	// 되돌린다.
	{
		NSMaterialSkinManager::FrameMove( DxViewPort::GetInstance().GetMatView(), DxViewPort::GetInstance().GetMatProj() );
	}
}

HRESULT CUIMeshData_SkinChar::SetPiece( const TCHAR* szFile, DWORD dwFlag /* = 0x0 */, int nLevel/* =0 */, bool bCharacterData /* = FALSE */, BOOL bAttachSub /* = FALSE */, EMPIECECHAR* pOutPiecetype /* = NULL*/)
{
    if ( !m_pd3dDevice )
    {
        return E_FAIL;
    }

    if ( !m_pSkinChar )
    {   
        return E_FAIL;
    }

    return m_pSkinChar->SetPiece( szFile, m_pd3dDevice, dwFlag, nLevel, EMSCD_ALL_THREAD, bAttachSub, pOutPiecetype );
}

void CUIMeshData_SkinChar::ResetPiece ( const EMPIECECHAR emPiece )
{
    if ( !m_pSkinChar )
    {
        return;
    }

    m_pSkinChar->ResetPiece( (int)emPiece );
}

bool CUIMeshData_SkinChar::IsSettingPiece( const EMPIECECHAR emPiece )
{
	if ( !m_pSkinChar )
	{
		return false;
	}

	PDXCHARPART pPart = m_pSkinChar->GetPiece( (int)emPiece );
	return !pPart->m_strNameCPS.empty();
}

const BOOL CUIMeshData_SkinChar::SetEffect ( const TCHAR* szFile, const EMPIECECHAR emPieceChar, D3DXVECTOR3& vDir )
{
    if ( !m_pSkinChar )
    {
        return FALSE;
    }

    if ( !szFile )
    {
        return FALSE;
    }

    PDXCHARPART pCharPart = m_pSkinChar->GetPiece( (int)emPieceChar );

    if ( pCharPart && pCharPart->GetFileName()[0] )
    {
        DxEffcharDataMan::GetInstance().PutEffect ( pCharPart, szFile, &vDir );
        return TRUE;
    }

    PDXATTBONELINK pAttPart = m_pSkinChar->GetAttachBone( (int)emPieceChar );

    if ( pAttPart && pAttPart->GetFileName()[0] )
    {
        DxEffcharDataMan::GetInstance().PutEffect ( pAttPart, szFile, &vDir );
        return TRUE;
    }

    return FALSE;
}

const BOOL CUIMeshData_SkinChar::SelectAni ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwFlags, DWORD dwSelect )
{
    if ( !m_pSkinChar )
    {
        return FALSE;
    }

	if( m_pSkinChar->GETCURMTYPE() == MType && m_pSkinChar->GETCURSTYPE() == SType )
		return TRUE;

    return m_pSkinChar->IsANI_SELECTANI(MType, SType, dwFlags, dwSelect);
}

const BOOL CUIMeshData_SkinChar::SetMainColor ( const EMPIECECHAR emPiece, const DWORD dwColor )
{
    if ( !m_pSkinChar )
    {
        return FALSE;
    }

    PDXCHARPART pCharPart = m_pSkinChar->GetPiece( (int)emPiece );

    if ( !pCharPart )
    {
        return FALSE;
    }

    pCharPart->SetHairColor( dwColor );

    return TRUE;
}

const BOOL CUIMeshData_SkinChar::SetSubColor ( const EMPIECECHAR emPiece, const DWORD dwColor )
{
    if ( !m_pSkinChar )
    {
        return FALSE;
    }

    PDXCHARPART pCharPart = m_pSkinChar->GetPiece( (int)emPiece );

    if ( !pCharPart )
    {
        return FALSE;
    }

    pCharPart->SetSubColor( dwColor );

    return TRUE;
}

void CUIMeshData_SkinChar::GetDefaultOverlayColor ( const EMPIECECHAR emPiece, DWORD& dwMainColor, DWORD& dwSubColor )
{
    if ( !m_pSkinChar )
    {
        return;
    }

    PDXCHARPART pCharPart = m_pSkinChar->GetPiece( (int)emPiece );

    if ( !pCharPart )
    {
        return;
    }

    pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );
}

const float CUIMeshData_SkinChar::GetHeight ()
{
    if ( !m_pSkinChar )
    {
        return 0.0f;
    }

    D3DXVECTOR3 vMax,vMin;
    m_pSkinChar->GetAABBBox( vMax, vMin );
    return (vMax.y - vMin.y);
}

void CUIMeshData_SkinChar::SetPeaceMode( BOOL isPeace )
{
	m_bAttackMode = !isPeace;
}

