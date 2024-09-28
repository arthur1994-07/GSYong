#ifndef _GL_VEHICLE_FIELD_H_
#define _GL_VEHICLE_FIELD_H_

#pragma once

#include <boost/noncopyable.hpp>
#include "../../RanLogic/Vehicle/GLVEHICLE.h"

class GLGaeaServer;
/*
// 일단 나중에 고치기 위해서 noncopyable 로 만들어 두었다.
class GLVehicleField : public GLVEHICLE, public boost::noncopyable
{
public:
    GLVehicleField(GLGaeaServer* pServer);
    virtual ~GLVehicleField();

private:
    GLGaeaServer* m_pGLGaeaServer;

public:
    void UpdateTimeLmtItem(GLChar* pChar);
};
*/

namespace GLVEHICLE_FIELD
{
    void UpdateTimeLmtItem( GLGaeaServer* pGLGaeaServer, GLChar* pChar, GLVEHICLE* pVehicle );
}

#endif // _GL_VEHICLE_FIELD_H_