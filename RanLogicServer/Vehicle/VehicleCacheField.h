#pragma once

#include "../../RanLogic/Vehicle/GLVEHICLE.h"

//! 2012-07-16 jgkim
class VehicleCacheField
{
public:
    VehicleCacheField();
    ~VehicleCacheField();

    enum EM_TIMER
    {
        CACHE_CHECK_TIME = 1800000, //! 1000*60*30 30분
    };

protected:
    typedef std::map<DWORD, std::tr1::shared_ptr<GLVEHICLE> > VDATA;
    typedef VDATA::iterator       VDATA_ITER;
    typedef VDATA::const_iterator VDATA_CITER;
    typedef VDATA::value_type     VDATA_VALUE;

    VDATA m_Data;
    DWORD m_CheckTimer;

    CRITICAL_SECTION m_Lock;
    
public:
    std::tr1::shared_ptr<GLVEHICLE> Get(DWORD VehicleDbNum);
    void Add(DWORD VehicleDbNum, std::tr1::shared_ptr<GLVEHICLE> spVehicle);
    void Add(DWORD VehicleDbNum, const GLVEHICLE* pVehicle);

    //! 오래된 Cache Data 는 주기적으로 지운다
    void FrameMove();
};