#pragma once

#include "../../RanLogic/Market/GLConsignmentSaleDefine.h"
#include "../../RanLogic/GLogicData.h"

struct NET_MSG_GENERIC;

class GLGaeaServer;
class GLChar;

namespace db
{
    class IDbManager;
}


class GLConsignmentSaleField
{
public:
    GLConsignmentSaleField( GLGaeaServer* pGaeaServer ) : m_pGaeaServer( pGaeaServer ) { }
    //! 유저의 자신 아이템 목록 요청
    bool ReqGetList( NET_MSG_GENERIC* nmg, GLChar* pChar );

    //! 위탁판매 아이템 등록 DB에 아이템 등록, 캐쉬로 정보를 날린다.
    bool ReqItemAdd( NET_MSG_GENERIC* nmg, GLChar* pChar );

    //! 위탁판매 아이템 등록 취소 요청
    bool ReqItemSendBack( NET_MSG_GENERIC* nmg, GLChar* pChar );

    //! 위탁판매 아이템 등록 취소 응답( 캐쉬 -> 필드 )
    bool AckItemSendBack( NET_MSG_GENERIC* nmg );

    //! 위탁판매 아이템 등록 취소 응답( 필드 DB -> 필드 )
    bool AckItemSendBackDB( NET_MSG_GENERIC* nmg );

    //! 캐쉬서버의 아이템 구매 요청
    bool ReqItemPurchase( NET_MSG_GENERIC* nmg );

	bool ReqItemPayMoney( NET_MSG_GENERIC* nmg );

	bool ReqSendPostAndLog( NET_MSG_GENERIC* nmg );

	bool AckItemPurchaseDB( NET_MSG_GENERIC* nmg );
    
private:
    void GetPostInfo( DWORD dwSender, DWORD dwRecver, SPOSTINFO& PostInfo OUT );
    void GetPostInfo( 
        std::string     PostTitle,
        std::string     PostContent,
        std::string     SendChaName,
        std::string     RecvChaName,
        DWORD           SendChaNum,
        DWORD           RecvChaNUm,
        DWORD           SendUserNum,
        DWORD           RecvUserNum,
        SPOSTINFO&      PostInfo OUT );

private:
    std::map<DWORD, int>   m_mapUserSlotCount; // 유저의 아이템 목록 갯수

private:
    GLGaeaServer* m_pGaeaServer;
};
