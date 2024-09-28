#pragma once

#include "./GLWidgetTextureChar.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Vehicle/GLVEHICLE.h"
#include "../../EngineLib/DxMeshs/DxAniKeys.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"
class DxVehicle;

namespace
{
	//	const float CAMERA_POSITION_Y       = 0.0f;
	const float CAMERA_POSITION_Z       = -27.0f; 


	VEHICLE_TYPE GetVehicleType(const SNATIVEID& id)
	{
		if ( id.Mid() == SNATIVEID::ID_NULL && id.Sid() == SNATIVEID::ID_NULL)
			return VEHICLE_TYPE_NONE;

		const SITEM* pItem = GLogicData::GetInstance().GetItem ( id );
		const VEHICLE_TYPE vType = pItem->VehicleType();
		GASSERT( vType == VEHICLE_TYPE_BIKE || vType == VEHICLE_TYPE_BOARD || vType == VEHICLE_TYPE_CAR || vType == VEHICLE_TYPE_REARCAR );
		return pItem->VehicleType() ;
	}

	EMANI_MAINTYPE GetBikeMainAniType(const SNATIVEID& id)
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem ( id );
		GASSERT(pItem->VehicleType() == VEHICLE_TYPE_BIKE);
		return EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
	}

	EMANI_MAINTYPE GetMainAniType(const SNATIVEID& id)
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem ( id );
		//GASSERT(pItem->VehicleType() != VEHICLE_TYPE_BOARD);
		switch ( pItem->sVehicle.emVehicleType )
		{
		case VEHICLE_TYPE_BIKE:
			return EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
		case VEHICLE_TYPE_CAR:
			return EMANI_MAINTYPE( AN_CAR_A +  pItem->sVehicle.emPartsType );
		case VEHICLE_TYPE_REARCAR:
			return EMANI_MAINTYPE( AN_REARCAR_A +  pItem->sVehicle.emPartsType );
		}
		// Default
		return AN_GUARD_N;
	}
}

// 캐릭터 텍스처
class GLWidgetTextureCharVehicle : public GLWidgetTextureChar
{
public :
	GLWidgetTextureCharVehicle( LPDIRECT3DDEVICEQ pDevice );
	virtual ~GLWidgetTextureCharVehicle(void);

public :
	virtual bool OnCreate();
	virtual void OnResetDevice();
	virtual void OnLostDevice();
	virtual void OnFrameMove( float fElapsedTime );
	virtual void OnRender();

protected :
	DxVehicle*		m_pVehicle;
	DxVehicleData*	m_pVehicleData;

	D3DXMATRIX		m_matVehicleWorld;

	D3DXVECTOR3		m_vCameraFromPt;
	float m_CamPosX;
	float m_CamPosY;
	float m_CamPosZ;

	float m_ObjectPosX;
	float m_ObjectPosY;
	float m_ObjectPosZ;
	float m_RotationY;

public :
	struct VehicleData
	{
		SNATIVEID VehicleID;
		SNATIVEID Parts[ACCE_TYPE_SIZE];
		SVEHICLE_COLOR ColorArray[ACCE_TYPE_SIZE];


		VehicleData()
		{
			VehicleID = NATIVEID_NULL();
			for (int i = 0; i < ACCE_TYPE_SIZE; ++i)
			{
				Parts[i] = NATIVEID_NULL();
				ColorArray[i] = SVEHICLE_COLOR();
			}
		}

		bool operator==(const VehicleData& other) const
		{
			if( VehicleID != other.VehicleID )
				return false;

			for (int i = 0; i < ACCE_TYPE_SIZE; ++i)
			{
				if( Parts[i] != other.Parts[i] )
					return false;

				if( ColorArray[i] != other.ColorArray[i] )
					return false;
			}

			return true;
		}

		bool operator!=(const VehicleData& other) const
		{
			return !operator==(other);
		}
	};

public:
	BOOL SetSkinVehicle(const VehicleData& data);
	void GetDriverMatrix( D3DXMATRIX& matCombinedScaleInv );
	void SetCamPosX(float value);
	void SetCamPosY(float value);
	void SetCamPosZ(float value);

	void SetPosition(float x,float y,float z);
	void SetRotationY(float rot);

	void GetDefaultOverlayColor( int emPart, DWORD& dwMainColor, DWORD& dwSubColor );
	const BOOL SetMainColor( int emPart, const DWORD dwColor );
	const BOOL SetSubColor( int emPart, const DWORD dwColor );
};