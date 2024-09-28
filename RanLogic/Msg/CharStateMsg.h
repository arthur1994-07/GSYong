#pragma once

#include "../../SigmaCore/gassert.h"
#include "../s_NetGlobal.h"
#include "../../RanLogic/FSM/GLCharStateDefine.h"

namespace GLMSG
{
    /**
        ĳ������ ���� ���� ����

     */
    struct NET_CHAR_STATE_FC
    {
        CharStateIndex  idx;    //@ state ����
        CharState       state;  //@ ���� ����

        MSGPACK_DEFINE(idx, state);
        NET_CHAR_STATE_FC() : idx(CHAR_STATE_INDEX_NONE), state(CHAR_STATE_DEFAULT) {}
    };

} // end namespace