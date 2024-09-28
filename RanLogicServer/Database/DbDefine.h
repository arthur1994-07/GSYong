#ifndef GL_DB_DEFINE_H_
#define GL_DB_DEFINE_H_

#pragma once

//! 2011-03-21 Jgkim
namespace db 
{

    enum SERVER_TYPE
    {
        SESSION_SERVER = 0,
        LOGIN_SERVER,
        AGENT_SERVER,
        FIELD_SERVER,
        CACHE_SERVER,
		MATCH_SERVER,
		INSTANCE_SERVER,
        EMULATOR_SERVER,
		AUTH_SERVER,
		WORLDBATTLE_SERVER,
    };

    enum DB_TYPE
    {
        GAME_DB = 0,
        USER_DB,
        LOG_DB,
        WEB_DB,
        SHOP_DB,
        CLUB_LOG_DB,
		AUTH_DB,
		TEXASHOLDEM_DB, 
		WORLDBATTLE_RELATED_DB,
		RANMOBILE_DB,
    };

    enum EM_BILLING_SELL_TYPE
    {
        SELL_POINT_SHOP, //! PointShop ¿¡¼­ ÆÇ¸ÅµÊ
        SELL_WEB_SHOP,   //! Web ¿¡¼­ ÆÇ¸ÅµÊ
    };
} // namespace db 
#endif // GL_DB_DEFINE_H_
