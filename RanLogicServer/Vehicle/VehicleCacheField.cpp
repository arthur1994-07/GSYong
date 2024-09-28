#include "pch.h"
#include "../../SigmaCore/Util/Lock.h"
#include "./VehicleCacheField.h"

#include "../../SigmaCore/DebugInclude.h"

VehicleCacheField::VehicleCacheField()
{
    InitializeCriticalSectionAndSpinCount(&m_Lock, sc::CRITICAL_SECTION_SPIN_COUNT);
    m_CheckTimer = GetTickCount();
}

VehicleCacheField::~VehicleCacheField()
{
    DeleteCriticalSection(&m_Lock);
}

std::tr1::shared_ptr<GLVEHICLE> VehicleCacheField::Get(DWORD VehicleDbNum)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    VDATA_ITER iter = m_Data.find(VehicleDbNum);
    if (iter != m_Data.end())
        return iter->second;
    else
        return std::tr1::shared_ptr<GLVEHICLE> ((GLVEHICLE*) 0);
}

void VehicleCacheField::Add(DWORD VehicleDbNum, std::tr1::shared_ptr<GLVEHICLE> spVehicle)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    VDATA_ITER iter = m_Data.find(VehicleDbNum);
    if (iter != m_Data.end())
        m_Data.erase(iter);
    m_Data.insert(VDATA_VALUE(VehicleDbNum, spVehicle));
}

void VehicleCacheField::Add(DWORD VehicleDbNum, const GLVEHICLE* pVehicle)
{    
    std::tr1::shared_ptr<GLVEHICLE> spVehicle(new GLVEHICLE(*pVehicle));
    Add(VehicleDbNum, spVehicle);
}

void VehicleCacheField::FrameMove()
{
    DWORD CurTime = GetTickCount();
    if ((CurTime - m_CheckTimer) < CACHE_CHECK_TIME)
        return;

    m_CheckTimer = CurTime;

    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);
    
    for (VDATA_ITER iter=m_Data.begin(); iter!=m_Data.end(); )
    {
        std::tr1::shared_ptr<GLVEHICLE> spVehicle = iter->second;
        if ((CurTime - spVehicle->GetChacheTickCount()) > CACHE_CHECK_TIME)
            iter = m_Data.erase(iter);
        else
            ++iter;
    }
    
}