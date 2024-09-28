#ifndef SServerDieCrowMessage_H
#define SServerDieCrowMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../enginelib/G-Logic/GLDefine.h"

namespace TriggerSystem
{

/**	
	맵 크루 사망 메시지
	크루가 맵에서 사망했을 때 호출 된다.    
	
 */
struct SServerDieCrowMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD   dwCrowGaeaID;   //@ 크루 필드서버 가이아 ID, 크루는 DB ID 가 없다.
    EMCROW  eCromType;      //@ 크루 타입
    DWORD   dwKillerDbBum;  //@ 죽었을 때 죽인자의 DB ID
};

} // end namespace

#endif