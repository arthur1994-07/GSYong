#pragma once

namespace gamelogic
{
    namespace notify
    {
        enum NOTIFY_TYPE
        {	
            UNKNOWN,
            FRIEND_ADD,         //! 친구 추가
            GROUPCHAR_INVITE,   //! 그룹채팅 초대
            CLUB_INVITE,        //! 클럽 초대
            PARTY_INVITE,       //! 파티 초대
        };
    } // namespace notify

} // namespace gamelogic