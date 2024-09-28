#pragma once

#include "./DxVehicleData.h"

class DxVehicleData;


struct DxVehicleThreadNeedData
{
	BOOL			m_bUpdataPassenger;						// UpdatePassengerPos �ѹ��̶� ȸ���� �Ǿ� ���� Matrix �� ���� �� �ִ�.
	SPassengerData	m_sPassengerData[MAX_PASSENGER_COUNT];

	void UpdatePassengerPos( const DxSkeletonMaintain* pSkeleton, float fScale );
	void Import( const DxVehicleData* pVehiclePartData );
	const D3DXMATRIX* GetPassengerMatrix( DWORD i ) const;

	DxVehicleThreadNeedData();
};