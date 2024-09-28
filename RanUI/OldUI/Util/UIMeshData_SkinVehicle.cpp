
#include "stdafx.h"

#include "UIMeshData_SkinVehicle.h"

#include "../../../EngineLib/DxMeshs/SkinMesh/DxVehicle.h"
#include "../../../EngineLib/DxMeshs/SkinMesh/DxVehicleData.h"
#include "../../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
	  


CUIMeshData_SkinVehicle::CUIMeshData_SkinVehicle()
	: m_fTime(0.f)
{
	m_pVehicle = NULL;
}

CUIMeshData_SkinVehicle::~CUIMeshData_SkinVehicle()
{
	SAFE_DELETE(m_pVehicle);
}

HRESULT CUIMeshData_SkinVehicle::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pd3dDevice )
	{
		return E_FAIL;
	}

	if ( m_pVehicle )
	{
		m_pVehicle->InitDeviceObjects( pd3dDevice );      
	}

	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT CUIMeshData_SkinVehicle::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pd3dDevice )
	{
		return E_FAIL;
	}

	if ( !m_pVehicle )
	{
		return S_OK;        
	}

	return m_pVehicle->RestoreDeviceObjects(pd3dDevice);

}

HRESULT CUIMeshData_SkinVehicle::InvalidateDeviceObjects()
{
	if ( !m_pVehicle )
	{
		return E_FAIL;        
	}

	return m_pVehicle->InvalidateDeviceObjects();
}

HRESULT CUIMeshData_SkinVehicle::DeleteDeviceObjects()
{
	if ( !m_pVehicle )
	{
		return E_FAIL;        
	}

	return m_pVehicle->DeleteDeviceObjects();
}

const BOOL CUIMeshData_SkinVehicle::Load( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& szFile )
{
	if ( !m_pd3dDevice )
	{
		return FALSE;
	}

	DxVehicleData* pVehicleData = DxVehicleDataContainer::GetInstance().LoadVehicleData( szFile.c_str(), EMSCD_ZERO );

	if ( !pVehicleData )
		return FALSE;

	m_pVehicleData = pVehicleData;
	SAFE_DELETE(m_pVehicle);
	m_pVehicle = new DxVehicle;

	if ( !m_pVehicle )
	{
		return FALSE;
	}

	m_pVehicle->SetVehiclePartData( pVehicleData, m_pd3dDevice, TRUE );

	return TRUE;
}

void CUIMeshData_SkinVehicle::FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
{
	if ( !pd3dDevice )
	{
		return;
	}

	m_fTime += fElapsedTime;
	m_pVehicle->FrameMove( m_fTime, fElapsedTime, FALSE, m_matWorld, TRUE, FALSE);
}

void CUIMeshData_SkinVehicle::Render( LPDIRECT3DDEVICEQ pd3dDevice )
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
	m_pVehicle->SetPosition(vPosition);

	pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

	D3DXMATRIXA16 matTrans(m_matWorld);

	DxViewPort::GetInstance().SetFromPt( m_vCameraFromPt );

	// View 와 Projection 등 셋팅.
	{
		NSMaterialSkinManager::FrameMove( m_matView, m_matProj );
	}

	// Render
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		m_pVehicle->RenderFixed( pd3dDevice, TRUE, 1.f );
	}
	else
	{
		NSMaterialSkinManager::SetMob( FALSE );

		m_pVehicle->SetPhysX_Cloth( FALSE );
		m_pVehicle->RenderMaterialOpaque ( pd3dDevice, 1.f );
		m_pVehicle->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
		m_pVehicle->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
	}

	// 되돌린다.
	{
		NSMaterialSkinManager::FrameMove( DxViewPort::GetInstance().GetMatView(), DxViewPort::GetInstance().GetMatProj() );
	}
}

HRESULT CUIMeshData_SkinVehicle::SetPart( const TCHAR* szFile,LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pd3dDevice )
	{
		return E_FAIL;
	}

	if ( !m_pVehicle )
	{   
		return E_FAIL;
	}

	return m_pVehicle->SetPart( szFile, m_pd3dDevice);
}

void CUIMeshData_SkinVehicle::ResetVehiclePart( EMPIECEVEHICLE i )
{
	if ( !m_pVehicle )
	{   
		return;
	}

	m_pVehicle->ResetVehiclePart( (int)i );
}

const BOOL CUIMeshData_SkinVehicle::SelectAni( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE*/, DWORD dwFlags/*=NULL*/, DWORD dwSelect/*=0 */ )
{
	if ( !m_pVehicle )
	{
		return FALSE;
	}

	return m_pVehicle->IsANI_SELECTANI(MType, SType, dwFlags, dwSelect);
}

const BOOL CUIMeshData_SkinVehicle::SetMainColor( int emPart, const DWORD dwColor )
{
	if ( !m_pVehicle )
	{
		return FALSE;
	}

	PDXCHARPART pVehiclePart = m_pVehicle->GetVehiclePart( emPart );

	if ( !pVehiclePart )
	{
		return FALSE;
	}

	pVehiclePart->SetHairColor( dwColor );

	return TRUE;
}

const BOOL CUIMeshData_SkinVehicle::SetSubColor( int emPart, const DWORD dwColor )
{
	if ( !m_pVehicle )
	{
		return FALSE;
	}

	PDXCHARPART pVehiclePart = m_pVehicle->GetVehiclePart( emPart );

	if ( !pVehiclePart )
	{
		return FALSE;
	}

	pVehiclePart->SetSubColor( dwColor );

	return TRUE;
}

void CUIMeshData_SkinVehicle::GetDefaultOverlayColor( int emPart, DWORD& dwMainColor, DWORD& dwSubColor )
{
	if ( !m_pVehicle )
	{
		return;
	}

	PDXCHARPART pVehiclePart = m_pVehicle->GetVehiclePart( emPart );

	if ( !pVehiclePart )
	{
		return;
	}

	pVehiclePart->GetDefaultOverlayColor( dwMainColor, dwSubColor );
}

const float CUIMeshData_SkinVehicle::GetHeight()
{
	if ( !m_pVehicle )
	{
		return 0.0f;
	}

	D3DXVECTOR3 vMax,vMin;
	m_pVehicle->GetAABBBox( vMax, vMin );
	return (vMax.y - vMin.y);
}

void CUIMeshData_SkinVehicle::GetDriverMatrix( D3DXMATRIX& matCombinedScaleInv )
{
	D3DXMatrixIdentity(&matCombinedScaleInv);
	if ( m_pVehicleData->m_sPassengerData[0].strBoneName.empty() )
		return;	

	//m_pVehicle->UpdatePassengerPos();
	const SPassengerData* pPassengerData = m_pVehicle->GetPassengerData(0);
	if ( pPassengerData )
	{
		matCombinedScaleInv = pPassengerData->matWorld;
	}
	//matCombinedScaleInv = m_pVehicle->m_sPassengerData[ 0 ].matWorld;
}




