#pragma once

namespace gamelogic
{
    namespace notify
    {
        enum NOTIFY_TYPE
        {	
            UNKNOWN,
            FRIEND_ADD,         //! ģ�� �߰�
            GROUPCHAR_INVITE,   //! �׷�ä�� �ʴ�
            CLUB_INVITE,        //! Ŭ�� �ʴ�
            PARTY_INVITE,       //! ��Ƽ �ʴ�
        };
    } // namespace notify

} // namespace gamelogic