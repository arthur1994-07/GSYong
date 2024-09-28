#pragma once

#include "../../SigmaCore/gassert.h"
#include "../s_NetGlobal.h"
#include "../../RanLogic/FSM/GLCharStateDefine.h"

namespace GLMSG
{
    /**
        캐릭터의 상태 낱개 전달

     */
    struct NET_CHAR_STATE_FC
    {
        CharStateIndex  idx;    //@ state 구분
        CharState       state;  //@ 현재 상태

        MSGPACK_DEFINE(idx, state);
        NET_CHAR_STATE_FC() : idx(CHAR_STATE_INDEX_NONE), state(CHAR_STATE_DEFAULT) {}
    };

} // end namespace