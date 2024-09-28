#ifndef _RAN_LOGIC_JSON_MSG_DEFINE_H_
#define _RAN_LOGIC_JSON_MSG_DEFINE_H_

#pragma once

//! Json message 2012-02-16 jgkim
namespace jm
{
    enum EM_MSG_TYPE
    {
        MSG_WORLD_BATTLE_CLUB_INFO = 1,
        MSG_WORLD_BATTLE_CLUB_INFO_END,        
        MSG_CLUB_MEMBER_CD_CERTIFY_CA, //! Client->Agent     
		MSG_GM_COMMAND_LOG_CA, 
    };

} // namespace jm

#endif // _RAN_LOGIC_JSON_MSG_DEFINE_H_