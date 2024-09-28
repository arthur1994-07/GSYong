#ifndef SServerSavePosMessage_H
#define SServerSavePosMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	위치를 저장할지를 묻는 메시지
    일반적으로 던전에서의 캐릭터 위치는 저장되지 않는다.    
	
 */
struct SServerSavePosMessage
	: public SMessage
{
    SServerSavePosMessage() : bSave(true) {}

	void Command(CTriggerInstance* pInstance);

    bool bSave; //@ 위치를 저장할지 유무
};

} // end namespace

#endif