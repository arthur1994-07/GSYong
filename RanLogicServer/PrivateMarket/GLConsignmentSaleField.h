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
    //! ������ �ڽ� ������ ��� ��û
    bool ReqGetList( NET_MSG_GENERIC* nmg, GLChar* pChar );

    //! ��Ź�Ǹ� ������ ��� DB�� ������ ���, ĳ���� ������ ������.
    bool ReqItemAdd( NET_MSG_GENERIC* nmg, GLChar* pChar );

    //! ��Ź�Ǹ� ������ ��� ��� ��û
    bool ReqItemSendBack( NET_MSG_GENERIC* nmg, GLChar* pChar );

    //! ��Ź�Ǹ� ������ ��� ��� ����( ĳ�� -> �ʵ� )
    bool AckItemSendBack( NET_MSG_GENERIC* nmg );

    //! ��Ź�Ǹ� ������ ��� ��� ����( �ʵ� DB -> �ʵ� )
    bool AckItemSendBackDB( NET_MSG_GENERIC* nmg );

    //! ĳ�������� ������ ���� ��û
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
    std::map<DWORD, int>   m_mapUserSlotCount; // ������ ������ ��� ����

private:
    GLGaeaServer* m_pGaeaServer;
};
