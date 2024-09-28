#include "pch.h"

#include "./DxSkeletonMaintain.h"
#include "./DxVehicleData.h"

#include "DxVehicleThreadNeedData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxVehicleThreadNeedData::DxVehicleThreadNeedData()
	: m_bUpdataPassenger(FALSE)
{
	for ( int i=0; i<MAX_PASSENGER_COUNT; ++i )
	{
		m_sPassengerData[i].Init();
	}
}

void DxVehicleThreadNeedData::UpdatePassengerPos( const DxSkeletonMaintain* pSkeleton, float fScale )
{
	D3DXMATRIX matRotate, matTrans, matCombinedScaleInv;

	for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
	{
		if( m_sPassengerData[i].strBoneName.empty() ) 
			continue;

		D3DXMatrixRotationYawPitchRoll( &matRotate, D3DXToRadian(m_sPassengerData[i].vRotate.y),
			D3DXToRadian(m_sPassengerData[i].vRotate.x), 
			D3DXToRadian(m_sPassengerData[i].vRotate.z) );

		D3DXMatrixTranslation( &matTrans, m_sPassengerData[i].vPos.x, m_sPassengerData[i].vPos.y, m_sPassengerData[i].vPos.z );

		const DxBoneTransMaintain* pBoneTran = pSkeleton->FindBone( m_sPassengerData[i].strBoneName.c_str() );

		D3DXMatrixIdentity(&matCombinedScaleInv);
		matCombinedScaleInv._11 /= fScale;
		matCombinedScaleInv._22 /= fScale;
		matCombinedScaleInv._33 /= fScale;

		if( pBoneTran ) 
			m_sPassengerData[i].matWorld = matRotate * matTrans * matCombinedScaleInv * pBoneTran->m_matCombined;
	}

	m_bUpdataPassenger = TRUE;
}

void DxVehicleThreadNeedData::Import( const DxVehicleData* pVehiclePartData )
{
	for ( int i = 0; i<MAX_PASSENGER_COUNT; i++)
	{
		m_sPassengerData[i] = pVehiclePartData->m_sPassengerData[i];
	}
}

const D3DXMATRIX* DxVehicleThreadNeedData::GetPassengerMatrix( DWORD i ) const
{
	if ( i >= MAX_PASSENGER_COUNT )
		return NULL;

	if ( !m_bUpdataPassenger )
		return NULL;

	return &m_sPassengerData[i].matWorld;
}