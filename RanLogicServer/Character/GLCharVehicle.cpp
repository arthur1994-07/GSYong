#include "../pch.h"
#include "../Vehicle/GLVehicleField.h"
#include "./GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLChar::VehicleActiveValue() const
{
    return m_pVehicle->IsActiveValue();
}

void GLChar::VehicleActiveValueSet(bool bActiveValue)
{
    m_pVehicle->SetActiveValue(bActiveValue);
}

BOOL GLChar::VehicleIsNotEnoughFull() const
{
    return m_pVehicle->IsNotEnoughFull();
}

DWORD GLChar::VehiclePassengerId(size_t Index) const
{
    return m_pVehicle->PassengerId(Index);
}

void GLChar::VehiclePassengerIdSet(size_t Index, DWORD GaeaId)
{
    m_pVehicle->PassengerIdSet(Index, GaeaId);
}

void GLChar::VehiclePassengerIdReset(size_t Index)
{
    m_pVehicle->PassengerIdReset(Index);
}

void GLChar::VehicleAssign(const GLVEHICLE& sVehicleData)
{
    m_pVehicle->ASSIGN(sVehicleData);
}

DWORD GLChar::VehicleOwnerDbNum() const
{
    return m_pVehicle->OwnerDbNum();
}

void GLChar::VehicleOwnerDbNumSet(DWORD ChaDbNum)
{
    m_pVehicle->OwnerDbNumSet(ChaDbNum);
}

//! 소환을 요청한 탈것 아이템의 Mid/Sid
SNATIVEID GLChar::VehicleId() const 
{ 
    return m_pVehicle->Id();
}

void GLChar::VehicleIdSet(const SNATIVEID& VehicleId) 
{ 
    m_pVehicle->IdSet(VehicleId);
}

DWORD GLChar::VehicleDbNum() const
{
    return m_pVehicle->DbNum();
}

void GLChar::VehicleDbNumSet(DWORD DbNum)
{
    m_pVehicle->DbNumSet(DbNum);
}

void GLChar::VehicleUpdateTimeLmtItem(GLChar* pChar)
{
    //m_pVehicle->UpdateTimeLmtItem(pChar);
    GLVEHICLE_FIELD::UpdateTimeLmtItem(m_pGaeaServer, pChar, m_pVehicle);
}

SNATIVEID GLChar::VehiclePutOnItemId(VEHICLE_ACCESSORYTYPE Type) const
{
    return m_pVehicle->PutOnItemId(Type);
}

SITEMCUSTOM GLChar::VehiclePutOnItem(VEHICLE_ACCESSORYTYPE Type) const
{
    return m_pVehicle->PutOnItem(Type);
}

const SITEMCUSTOM& GLChar::VehiclePutOnItemRef(VEHICLE_ACCESSORYTYPE Type) const
{
    return m_pVehicle->PutOnItemRef(Type);
}

VEHICLE_TYPE GLChar::VehicleType() const
{
    return m_pVehicle->Type();
}

void GLChar::VehicleTypeSet(VEHICLE_TYPE emType)
{
    m_pVehicle->TypeSet(emType);
}

int GLChar::VehicleFull() const
{
    return m_pVehicle->Full();
}

DWORD GLChar::VehicleBoosterId() const
{
    return m_pVehicle->BoosterId();
}

void GLChar::VehiclePutOnItemReset( VEHICLE_ACCESSORYTYPE Type, bool DbUpdate /*= false*/ )
{
	const SITEMCUSTOM& sItemCustom = VehiclePutOnItemRef( Type );
	if ( sItemCustom.GetNativeID() == NATIVEID_NULL() )
	{
		return;
	}

	sc::MGUID guid = sItemCustom.GetGuid();
	int DbState = sItemCustom.GetDbState();

    m_pVehicle->PutOnItemReset( Type );

	if ( DbUpdate )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemDelete( guid, DbState, VEHICLE_PUTON_ITEM_RESET ) ) );
	}
}

void GLChar::VehiclePutOnItemUpdate( EMSUIT emSuit, bool JustInvenTypeUpdate /*= false*/ )
{
	if ( !m_pVehicle )
	{
		return;
	}

	VEHICLE_ACCESSORYTYPE Type = ACCE_TYPE_SKIN;
	if ( m_pVehicle->GetAccessoryType( emSuit, Type ) )
	{
		VehiclePutOnItemUpdate( Type, JustInvenTypeUpdate );
	}
}

void GLChar::VehiclePutOnItemUpdate( VEHICLE_ACCESSORYTYPE Type, bool JustInvenTypeUpdate /*= false*/ )
{
	DWORD VehicleNum = VehicleDbNum();
	if ( GAEAID_NULL == VehicleNum )
	{
		return;
	}

	SITEMCUSTOM& sItemCustom = const_cast< SITEMCUSTOM& >( VehiclePutOnItemRef( Type ) );
	if ( sItemCustom.GetNativeID() == NATIVEID_NULL() )
	{
		return;
	}

	SINVENITEM_SAVE sItem( sItemCustom, static_cast< WORD >( Type ), 0 );

	int DbState = sItemCustom.GetDbState();
	if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemInsert( VehicleNum, INVEN_VEHICLE, sItem, 10987 ) ) );

		sItemCustom.SetDbState( db::DB_UPDATE, true );
	}
	else
	{
		if ( JustInvenTypeUpdate )
		{
			// 저장 위치 및 좌표만 변경된다.
			m_pGaeaServer->UpdateItemInvenType( VehicleNum, sItemCustom, INVEN_VEHICLE, sItem.wPosX, sItem.wPosY );
		}
		else
		{
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemUpdate( VehicleNum, INVEN_VEHICLE, sItem, 10987 ) ) );
		}
	}
}

SVEHICLE_COLOR GLChar::VehicleColor(VEHICLE_ACCESSORYTYPE Type) const
{
    return m_pVehicle->Color(Type);
}

void GLChar::VehicleItemUpdate()
{
    m_pVehicle->ITEM_UPDATE();
}

float GLChar::VehicleSpeedRate() const 
{ 
    return m_pVehicle->GetSpeedRate();
}

float GLChar::VehicleSpeedVol() const
{ 
    return m_pVehicle->GetSpeedVol();
}