#pragma once
#include "pch.h"

#include "../../enginelib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../Enginelib/DxMeshs/SkinMesh/DxVehicle.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxVehicleData.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkeletonMaintain.h"
#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxViewPort.h"

#include "./GLWidgetTextureCharVehicle.h"

GLWidgetTextureCharVehicle::GLWidgetTextureCharVehicle( LPDIRECT3DDEVICEQ pDevice )
: GLWidgetTextureChar( pDevice )
, m_pVehicle ( NULL )
, m_pVehicleData ( NULL)
, m_CamPosX ( 0 )
, m_CamPosY ( 0 )
, m_CamPosZ ( 0 )
, m_ObjectPosX ( 0 )
, m_ObjectPosY ( 0 )
, m_ObjectPosZ ( 0 )
, m_RotationY ( 0 )
{

}

GLWidgetTextureCharVehicle::~GLWidgetTextureCharVehicle( void )
{
	SAFE_DELETE( m_pVehicle );
}

bool GLWidgetTextureCharVehicle::OnCreate()
{
	if( false == GLWidgetTextureChar::OnCreate() )
		return false;

	//m_vViewPos = D3DXVECTOR3( 0.0f, 10.0f, -35.0f );
	m_vViewPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	return true;
}

void GLWidgetTextureCharVehicle::OnResetDevice()
{
	GLWidgetTextureChar::OnResetDevice();

	if( NULL == m_pVehicle )
		return;

	m_pVehicle->RestoreDeviceObjects( m_pDevice );
}

void GLWidgetTextureCharVehicle::OnLostDevice()
{
	GLWidgetTextureChar::OnLostDevice();

	if( NULL == m_pVehicle )
		return;

	m_pVehicle->InvalidateDeviceObjects();
}

void GLWidgetTextureCharVehicle::OnFrameMove( float fElapsedTime )
{
	if( NULL == m_pSkinChar )
		return;

	m_fTime += fElapsedTime;

	D3DXVECTOR3 vFromPt( m_CamPosX, m_CamPosY, m_CamPosZ );
	//D3DXVECTOR3 vLookatPt( 0.f, 0.f, 0.f );
	//D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
	m_vViewPos = vFromPt;
	D3DXVECTOR3 vLook( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &m_matView, &m_vViewPos, &vLook, &vUp );
	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI / 4.0f, m_fAspect, 0.1f, 2000.0f );
	D3DXMatrixIdentity( &m_matWorld );
	if( NULL != m_pVehicle )
	{
		D3DXMATRIX matCombinedScaleInv;
		GetDriverMatrix( matCombinedScaleInv );
		m_matWorld = matCombinedScaleInv;
		D3DXMatrixIdentity( &m_matVehicleWorld );
	}

	SelectAni( m_emAniMainType, m_emAniSubType );
	
	m_pSkinChar->FrameMove( m_fTime, fElapsedTime, FALSE, m_matWorld );

	if( NULL == m_pVehicle )
		return;
	m_pVehicle->SELECTANI(AN_PLACID,AN_SUB_NONE,NULL,0);
	m_pVehicle->FrameMove( m_fTime, fElapsedTime, FALSE, m_matVehicleWorld );
}

void GLWidgetTextureCharVehicle::OnRender()
{
	NSMaterialSkinManager::SetMob( FALSE );

	D3DXVECTOR3 vDiffuse(1.f,1.f,1.f);
	D3DXVECTOR3 PointLight(0.f,0.f,0.f);
	m_pSkinChar->SetWorldLight( vDiffuse, PointLight );

	// View 와 Projection 등 셋팅.
	{
		NSMaterialSkinManager::FrameMove( m_matView, m_matProj );
	}

	// Render
	if( NULL == m_pVehicle ) // 보드일 경우
	{
		m_matWorld._41 = m_ObjectPosX;
		m_matWorld._42 = m_ObjectPosY;
		m_matWorld._43 = m_ObjectPosZ;

		m_pDevice->SetTransform( D3DTS_VIEW, &m_matView );
		m_pDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
		m_pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

		D3DXMATRIX matR;
		D3DXMatrixRotationY( &matR, m_fRotate );
		m_matWorld =  matR * m_matWorld;
		//m_pSkinChar->Render( m_pDevice, FALSE, 0.0f, m_matWorld );

		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		{
			m_pSkinChar->RenderFixed( m_pDevice, FALSE, 1.f );
		}
		else
		{
			m_pSkinChar->SetPhysX_Cloth( FALSE );
			m_pSkinChar->RenderMaterialOpaque ( m_pDevice, 1.f );
			m_pSkinChar->RenderMaterialHardAlpha ( m_pDevice, 1.f );
			m_pSkinChar->RenderMaterialSoftAlpha ( m_pDevice, 1.f );
			m_pSkinChar->RenderMaterialEffect ( m_pDevice, 1.f );
		}
	}
	else
	{
		m_matVehicleWorld._41 = m_ObjectPosX;
		m_matVehicleWorld._42 = m_ObjectPosY;
		m_matVehicleWorld._43 = m_ObjectPosZ;

		m_pDevice->SetTransform( D3DTS_VIEW, &m_matView );
		m_pDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
		m_pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

		D3DXMATRIX matR;
		D3DXMatrixRotationY( &matR, m_fRotate );
		m_matVehicleWorld =  matR * m_matVehicleWorld;

		//m_pSkinChar->Render( m_pDevice, FALSE, 0.0f, m_matWorld );
		//m_pVehicle->Render( m_pDevice, 1.f, m_matVehicleWorld );

		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		{
			m_pSkinChar->RenderFixed( m_pDevice, FALSE, 1.f );
			m_pVehicle->RenderFixed( m_pDevice, FALSE, 1.f );
		}
		else
		{
			m_pSkinChar->SetPhysX_Cloth( FALSE );
			m_pSkinChar->RenderMaterialOpaque ( m_pDevice, 1.f );
			m_pSkinChar->RenderMaterialHardAlpha ( m_pDevice, 1.f );

			m_pVehicle->SetPhysX_Cloth( FALSE );
			m_pVehicle->RenderMaterialOpaque ( m_pDevice, 1.f );
			m_pVehicle->RenderMaterialHardAlpha ( m_pDevice, 1.f );

			m_pSkinChar->RenderMaterialSoftAlpha ( m_pDevice, 1.f );
			m_pVehicle->RenderMaterialSoftAlpha ( m_pDevice, 1.f );

			m_pSkinChar->RenderMaterialEffect ( m_pDevice, 1.f );
			m_pVehicle->RenderMaterialEffect ( m_pDevice, 1.f );
		}
	}

	// 되돌린다.
	{
		NSMaterialSkinManager::FrameMove( DxViewPort::GetInstance().GetMatView(), DxViewPort::GetInstance().GetMatProj() );
	}
}

BOOL GLWidgetTextureCharVehicle::SetSkinVehicle(const VehicleData& data)
{
	// 메쉬 캐릭터가 반드시 있어야함
	GASSERT( m_pSkinChar );
	SAFE_DELETE( m_pVehicle );

	//
	//DeleteMeshData(MESH_ID_VEHICLE);
	//m_pMeshVehicleData = NULL;i

	//ResetItem(SLOT_VEHICLE);

	//	EMCHARINDEX emIndex = CharClassToIndex(pCharacter->m_emClass);   
	//m_BackUpVehicleData = data;

	if( GetVehicleType(data.VehicleID) == VEHICLE_TYPE_BOARD )
	{
		//////////////////////////////////////////////////////////////////////////

		float camY = 0;
		SetCamPosY(camY);
		SetCamPosZ(-35);

		float x=0,y=0,z=0;
		FLOAT fHeight = GetHeight();

		// + 2는 보드 크기로 추정한 하드코딩값
		// 남자는 20 여자는 17.91
		// 16 이 이상적인 위치로 보임
		SetPosition(x,-fHeight + (fHeight-16),z);

		float rot = -70;
		//SetRotationY(rot);
		SetRotate(rot);


		//////////////////////////////////////////////////////////////////////////
		SetItem( data.VehicleID , SLOT_VEHICLE );
	}
	else
	{
		//SAFE_DELETE( m_pVehicle );

		const SITEM* pItem = GLogicData::GetInstance().GetItem ( data.VehicleID );
		std::string vehicleName = pItem->GetWearingFile( ( EMCHARINDEX ) pItem->sVehicle.emPartsType);
		DxVehicleData* pVehicleData = DxVehicleDataContainer::GetInstance().LoadVehicleData( vehicleName.c_str(), EMSCD_ZERO );

		if ( !pVehicleData )
			return FALSE;

		m_pVehicleData = pVehicleData;


		m_pVehicle = new DxVehicle;
		m_pVehicle->SetVehiclePartData ( m_pVehicleData, m_pDevice, TRUE );

		////////////////////////////////////////////////////////////////////////
		float camY = 10;
		SetCamPosY(camY);

		SetCamPosZ(-35);

		FLOAT fHeight = GetHeight();
		float x=5;
		float y=-fHeight/2 + 3;
		float z=0;
		SetPosition(x,y,z);

		float rot = 70;
		//SetRotationY(rot);
		SetRotate(rot);
		////////////////////////////////////////////////////////////////////////

		//const SITEM* pItem = GLogicData::GetInstance().GetItem ( data.VehicleID );
		if ( pItem == NULL )
		{
			return FALSE;
		}
		//std::string vehicleName = pItem->GetWearingFile( ( EMCHARINDEX ) pItem->sVehicle.emPartsType);
		GASSERT( !vehicleName.empty() );
		if ( vehicleName.empty() )
		{
			return FALSE;
		}
		//m_pMeshVehicleData = LoadSkinVehicle(MESH_ID_VEHICLE,vehicleName);
		//m_pVehicle = LoadSkinVehicle(MESH_ID_VEHICLE,vehicleName);
		for ( INT i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; i++ )
		{
			if ( data.Parts[i] == NATIVEID_NULL() )
				continue;

			const SITEM* pITEM = GLogicData::GetInstance().GetItem ( data.Parts[i] );
			if ( !pITEM )
				continue;

			//m_pMeshVehicleData->SetPart( pITEM->GetWearingFile( ( EMCHARINDEX ) pItem->sVehicle.emPartsType), m_pd3dDevice);
			m_pVehicle->SetPart( pITEM->GetWearingFile( ( EMCHARINDEX ) pItem->sVehicle.emPartsType), m_pDevice);
		}

		for ( INT i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; i++ )
		{
			DWORD dwMainColor = 0, dwSubColor = 0;
			GetDefaultOverlayColor(i,dwMainColor,dwSubColor);
			if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
				dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
			{
				SetMainColor(i,dwMainColor);
				SetSubColor(i,dwSubColor);
			}
		}

		for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
		{
			if( data.ColorArray[i].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
				data.ColorArray[i].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
			{
				SetMainColor(i, data.ColorArray[i].dwMainColor );
				SetSubColor(i, data.ColorArray[i].dwSubColor );
			}									
		}
	}



	// 

	//	SetAnimationStop();
	//SetAnimationWalk();

	//////////////////////////////////////////////////////////////////////////

	//SetVehicleMatrix();

	//////////////////////////////////////////////////////////////////////////


	return TRUE;
}

void GLWidgetTextureCharVehicle::GetDriverMatrix( D3DXMATRIX& matCombinedScaleInv )
{
	const DxSkeletonMaintain* pSkeletonRes = m_pVehicle->GetSkeletonMaintain();
	if ( !pSkeletonRes )
		return;

	D3DXMatrixIdentity(&matCombinedScaleInv);
	if ( !m_pVehicleData || m_pVehicleData->m_sPassengerData[0].strBoneName.empty() )
		return;

	const SPassengerData sPassengerData = m_pVehicleData->m_sPassengerData[0];

	const float fScale = m_pVehicle->GetScale();

	D3DXMATRIX matScale, matRotate, matTrans;

	D3DXMatrixRotationYawPitchRoll( &matRotate, D3DXToRadian(sPassengerData.vRotate.y ),
		D3DXToRadian(sPassengerData.vRotate.x), 
		D3DXToRadian(sPassengerData.vRotate.z) );
	D3DXMatrixTranslation( &matTrans, sPassengerData.vPos.x,sPassengerData.vPos.y, sPassengerData.vPos.z );

	const DxBoneTransMaintain* pBoneTran = pSkeletonRes->FindBone( sPassengerData.strBoneName.c_str() );

	if( pBoneTran ) 
	{
		matCombinedScaleInv._11 /= fScale;
		matCombinedScaleInv._22 /= fScale;
		matCombinedScaleInv._33 /= fScale;

		matCombinedScaleInv = matRotate * matTrans * matCombinedScaleInv * pBoneTran->m_matCombined;
	}
}

void GLWidgetTextureCharVehicle::SetCamPosX( float value )
{
	m_CamPosX = value;
}

void GLWidgetTextureCharVehicle::SetCamPosY( float value )
{
	m_CamPosY = value;
}

void GLWidgetTextureCharVehicle::SetCamPosZ( float value )
{
	m_CamPosZ = value;
}

void GLWidgetTextureCharVehicle::SetPosition( float x,float y,float z )
{
	m_ObjectPosX = x;
	m_ObjectPosY = y;
	m_ObjectPosZ = z;
}

void GLWidgetTextureCharVehicle::SetRotationY( float rot )
{
	m_RotationY = rot;
}

void GLWidgetTextureCharVehicle::GetDefaultOverlayColor( int emPart, DWORD& dwMainColor, DWORD& dwSubColor )
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

const BOOL GLWidgetTextureCharVehicle::SetMainColor( int emPart, const DWORD dwColor )
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

const BOOL GLWidgetTextureCharVehicle::SetSubColor( int emPart, const DWORD dwColor )
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

