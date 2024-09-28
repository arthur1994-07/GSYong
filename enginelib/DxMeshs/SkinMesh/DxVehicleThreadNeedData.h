#pragma once

#include "./DxVehicleData.h"

class DxVehicleData;


struct DxVehicleThreadNeedData
{
	BOOL			m_bUpdataPassenger;						// UpdatePassengerPos 한번이라도 회전이 되야 정상 Matrix 를 가질 수 있다.
	SPassengerData	m_sPassengerData[MAX_PASSENGER_COUNT];

	void UpdatePassengerPos( const DxSkeletonMaintain* pSkeleton, float fScale );
	void Import( const DxVehicleData* pVehiclePartData );
	const D3DXMATRIX* GetPassengerMatrix( DWORD i ) const;

	DxVehicleThreadNeedData();
};