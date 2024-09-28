#ifndef _GL_VEHICLE_FIELD_H_
#define _GL_VEHICLE_FIELD_H_

#pragma once

#include <boost/noncopyable.hpp>
#include "../../RanLogic/Vehicle/GLVEHICLE.h"

class GLGaeaServer;
/*
// �ϴ� ���߿� ��ġ�� ���ؼ� noncopyable �� ����� �ξ���.
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