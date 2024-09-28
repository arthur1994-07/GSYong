#ifndef SServerPortalMessage_H
#define SServerPortalMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	시스템에 포탈(이동)을 요청 한다.
    클라이언트에 의해 포탈 요청이 왔을 때 호출 된다.

    포탈이 오픈되어 있다면 정상 작동 된다.
    이렇게 함으로써 임의의 순간 포탈(해킹등)을 막고
    우리가 원하는 순간에만 포탈이 되도록 처리할 수 있다.
	
 */
struct SServerPortalMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwFromMapID;  //@ 이동 요청 맵 ID
    DWORD dwFromGateID; //@ 이동 요청 게이트 ID
    DWORD dwToMapID;    //@ 이동 할 맵 ID
    DWORD dwToGateID;   //@ 이동 할 게이트 ID
};

} // end namespace

#endif