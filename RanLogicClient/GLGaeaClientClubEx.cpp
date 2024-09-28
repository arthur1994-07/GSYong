#include "pch.h"
#include "../SigmaCore/MsgPack/MinMsgPack.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Json/MinJson.h"
#include "../SigmaCore/Util/DateTime.h"

#include "../EngineLib/DxTools/DxClubMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"

#include "../MfcExLib/RanFilter.h"

#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/Msg/ClubMsg.h"
#include "../RanLogic/Msg/JsonMsgDefine.h"
#include "../RanLogic/Club/ClubLogManClient.h"

#include "./Club/GLClubClient.h"

#include "./Land/GLLandManClient.h"
#include "./Stage/DxGlobalStage.h"
#include "./StaticHoldItem.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

LONGLONG GLGaeaClient::ClubStorageMoney() const
{
    return m_pMyClub->GetStorageMoney();
}

LONGLONG GLGaeaClient::ClubIncomeMoney() const
{
    return m_pMyClub->GetIncomeMoney();
}

bool GLGaeaClient::ClubIncomeMoneyToStorageMoney()
{
    // Ŭ���� ���� ���� ��û.
    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->HaveMoneyAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    GLMSG::NET_CLUB_INCOME_RENEW_CA NetMsgReNew;
    NETSENDTOAGENT(&NetMsgReNew);

    return true;
}

void GLGaeaClient::MsgClubStorageUpdateMoney(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY* pNetMsg = (GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY*) nmg;
    m_pMyClub->SetStorageMoney(pNetMsg->lnMoney);
    m_pMyClub->SetIncomeMoney(pNetMsg->IncomeMoney);
		
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMoney, "-l, -l", pNetMsg->IncomeMoney, pNetMsg->lnMoney);
}

LONGLONG GLGaeaClient::CharacterInvenMoney() const
{
    return m_pCharacter->GetInvenMoney();
}

bool GLGaeaClient::ReqClubStorageSaveMoney(LONGLONG lnMoney)
{
	if (m_ServiceProvider == SP_INDONESIA)
		return false;

    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->HaveMoneyAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    if (!m_pMyClub->GetStorageState())
        return false;

    if (m_pCharacter->GetInvenMoney() < lnMoney)
        return false;

    if (m_pCharacter->ValidWindowOpen())
        return false;

    GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_CF SendData;
    SendData.SetMoney(lnMoney);
    //NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_CF)-sizeof(unsigned int));

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    NETSENDTOFIELD(NET_MSG_CLUB_STORAGE_SAVE_MONEY_CF, SendBuffer);

    return true;
}

bool GLGaeaClient::ReqClubStorageDrawMoney(LONGLONG lnMoney)
{
	if (m_ServiceProvider == SP_INDONESIA)
		return false;

    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->HaveMoneyAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }    

	if (lnMoney <= 0)
		return false;

    if (!m_pMyClub->GetStorageState())
        return false;

    if (m_pMyClub->GetStorageMoney() < lnMoney)
        return false;

    if (m_pCharacter->ValidWindowOpen())
        return false;

    GLMSG::NET_CLUB_STORAGE_DRAW_MONEY_CA SendData(lnMoney);
    //NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::NET_CLUB_STORAGE_DRAW_MONEY_CA)-sizeof(unsigned int));
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    NETSENDTOAGENT(NET_MSG_CLUB_STORAGE_DRAW_MONEY_CA, SendBuffer);

    return true;
}


bool GLGaeaClient::ReqGetClubStorage()
{
    if (!m_pCharacter->IsValidBody())
        return false;

    if (!m_pMyClub->IsValid())
        return false;

    //if (m_pMyClub->MasterCharDbNum() != m_CharDbNum)
    //    return S_FALSE;
    if (!m_pMyClub->HaveStorageAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

	GLLandManClient* pLand = GetActiveMap();
	if ( !pLand )	
		return false;

	// http://crm.mincoms.com/Ticket/TicketView/275
	// Ŭ����Ŀ ��� ���� ��û
	if ( pLand->m_bClubBattleInProgress )
	{
		return false;
	}

    // �̹� â�� ������ ��ȿ�ϴٸ� ����.
    if (!m_pMyClub->GetStorageState())
    {
        // ������ ��û.
        // �� �޽��� ���� �� �����ʿ��� Ŭ���� ���� ������ ��û�Ѵ�.
        GLMSG::NET_CLUB_GETSTORAGE_CA NetMsg;
        NETSENDTOAGENT(&NetMsg);
        return true;
    }
    else
    {
        return false;
    }
    //else
    //{
    //  // Ŭ���� ���� ���� ��û.
    //  GLMSG::NET_CLUB_INCOME_RENEW_CA NetMsgReNew;
    //  NETSENDTOAGENT(&NetMsgReNew);
    //}
}


bool GLGaeaClient::ClubStorageCheck( DWORD dwChannel )
{
    //GASSERT(MAX_CLUBSTORAGE>dwChannel);
    if (MAX_CLUBSTORAGE <= dwChannel)
        return false;

    if (!m_pCharacter->IsValidBody())
        return false;

    if (m_pCharacter->ValidWindowOpen())
        return false;	

    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->HaveStorageAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    if (!IsKEEP_CLUB_STORAGE(dwChannel))
        return false;

    return true;
}

/*
//! Ŭ�� â�� ������ �鶧, ������, ��ȯ�Ҷ�, ��ĥ��
bool GLGaeaClient::ReqClubStorageTo(DWORD dwChannel, WORD wPosX, WORD wPosY)
{
    if (!ClubStorageCheck(dwChannel))
        return false;

    // �ŷ� �ɼ� Ȯ��
    if (m_pCharacter->HaveHoldItem())
    {
        const SITEM* pItemData = GLogicData::GetInstance().GetItem(m_pCharacter->GET_HOLD_ITEM().sNativeID);
        if (!pItemData)
            return false;

        // �ŷ��ɼ�
        if (!pItemData->CanExchange())
            return false;
    }

    const SINVENITEM* pClubInven = m_pMyClub->FindPosItem(dwChannel, wPosX, wPosY);
    if (!m_pCharacter->HaveHoldItem() && !pClubInven)
        return false;

    if (m_pCharacter->HaveHoldItem () && pClubInven)
    {
        if (m_ServiceProvider == SP_VIETNAM)
        {
            if (m_pCharacter->GET_HOLD_ITEM().bVietnamGainItem)
                return false;
        }

        GLMSG::NET_CLUB_STORAGE_EX_HOLD_CA SendData;
        SendData.m_Channel = static_cast<WORD> (dwChannel);
        SendData.m_PosX = pClubInven->wPosX;
        SendData.m_PosY = pClubInven->wPosY;
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        NETSENDTOAGENT(NET_MSG_CLUB_ITEM_EXCHANGE_TO_HOLD_CA, SendBuffer, true);
    }
    else if (pClubInven)
    {
        GLMSG::NET_CLUB_STORAGE_TO_HOLD_CA SendData;
        SendData.m_Channel = static_cast<BYTE> (dwChannel);
        SendData.m_PosX = pClubInven->wPosX;
        SendData.m_PosY = pClubInven->wPosY;
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        NETSENDTOAGENT(NET_MSG_CLUB_STORAGE_TO_HOLD_CA, SendBuffer, true);
    }
    else if (m_pCharacter->HaveHoldItem())
    {
        if (m_ServiceProvider == SP_VIETNAM)
        {
            if (m_pCharacter->GET_HOLD_ITEM().bVietnamGainItem)
                return false;
        }

        // �޽��� �۽����� ��ȿ������ �̸� �˻���
        const SITEM* pItem = GLogicData::GetInstance().GetItem(m_pCharacter->GET_HOLD_ITEM().sNativeID);
        if (!pItem)
        {
            std::string ErrMsg(
                sc::string::format(
                "ReqClubStorageTo no item data Mid/Sid %1%/%2%",
                m_pCharacter->GET_HOLD_ITEM().sNativeID.wMainID,
                m_pCharacter->GET_HOLD_ITEM().sNativeID.wSubID));
            //GASSERT(pItem && ErrMsg.c_str());
            sc::writeLogError(ErrMsg);
            return false;
        }

        BOOL bOk = m_pMyClub->IsInsertable(dwChannel, wPosX, wPosY);
        if (!bOk)
        {
            // �ι��� ��������
            return false;
        }

        GLMSG::NET_CLUB_HOLD_TO_STORAGE_CF NetMsg;
        NetMsg.m_Channel = dwChannel;
        NetMsg.m_PosX = wPosX;
        NetMsg.m_PosY = wPosY;
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, NetMsg);
        NETSENDTOFIELD(NET_MSG_CLUB_HOLD_TO_STORAGE_CF, SendBuffer);
    }

    return true;
}
*/

//! ����ִ� �������� Ŭ��â�� ������
bool GLGaeaClient::ReqInvenItemToClub(DWORD dwChannel, WORD wPosX, WORD wPosY)
{
// 	if (m_ServiceProvider == SP_INDONESIA)
// 		return false;

    if (!ClubStorageCheck(dwChannel))
    {
        holditem::ReleaseHoldItem();

        return false;
    }

    if (!holditem::HaveHoldItem())
    {
        holditem::ReleaseHoldItem();

        return false;
    }

    const SINVENITEM* pClubItem = m_pMyClub->FindPosItem(dwChannel, wPosX, wPosY);
    if (pClubItem)
    {
        holditem::ReleaseHoldItem();

        return false;
    }

//     if (m_ServiceProvider == SP_VIETNAM)
//     {
//         if (holditem::GetHoldItem().bVietnamGainItem)
//         {
//             holditem::ReleaseHoldItem();
// 
//             return false;
//         }
//     }

    // �޽��� �۽����� ��ȿ������ �̸� �˻���
    const SITEM* pItem = GLogicData::GetInstance().GetItem(holditem::GetHoldItem().Id());
    if (!pItem)
    {
        holditem::ReleaseHoldItem();

        PrintMsgTextDlg(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(
                ID2GAMEINTEXT("INVALID_ITEM_DATA"),
                holditem::GetHoldItem().GetNativeID().Mid(),
                holditem::GetHoldItem().GetNativeID().Sid())); 

        return false;
    }

    //if (!pItem->CanExchange())
	if ( !( holditem::GetHoldItem().IsExchange() != NULL ) )
    {
        holditem::ReleaseHoldItem();

        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CAN_NOT_TRADE_ITEM"));
        return false;
    }

	// #3989 GmCharEdit ���� �ŷ��Ұ��� �־��� �������� Ŭ��â�� ���� �� �ִ� ���� ����
	if ( holditem::GetHoldItem().IsGM_GENITEM() )
	{
		holditem::ReleaseHoldItem();
		PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "CAN_NOT_TRADE_ITEM" ) );
		return false;
	}

    BOOL bOk = m_pMyClub->IsInsertable(dwChannel, wPosX, wPosY);
    if (!bOk)
    {
        holditem::ReleaseHoldItem();
        // Ŭ��â�� ��������
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_STORAGE_NO_SPACE"));
        return false;
    }

    switch ( holditem::WhenHoldItem() )
    {
    case holditem::INVENTORY:
        {
            GLMSG::NET_CLUB_INVEN_TO_STORAGE_CF SendData;
            SendData.m_Channel  = static_cast<BYTE> (dwChannel);
            SendData.m_PosX     = wPosX;
            SendData.m_PosY     = wPosY;
            SendData.m_HoldPosX = holditem::GetHoldPosX();
            SendData.m_HoldPosY = holditem::GetHoldPosY();
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);
            NETSENDTOFIELD(NET_MSG_CLUB_INVEN_TO_STORAGE_CF, SendBuffer);

        }
        break;

    case holditem::CLUB_STORAGE:
        {
//             if ( wPosX == holditem::GetHoldPosX() &&
//                 wPosY == holditem::GetHoldPosY() )
//             {
//                 holditem::ReleaseHoldItem();
// 
//                 return S_OK;
//             }

            DWORD dwHoldChannel, dwDummy;
            holditem::GetData( dwHoldChannel, dwDummy );

			// ä�α��� Ȯ���ؾ�, �ٸ� ä���ε� ���� ��ǥ�� �������� ���� ���ϴ� ���� ����
			if ( dwChannel == dwHoldChannel && wPosX == holditem::GetHoldPosX() && wPosY == holditem::GetHoldPosY() )
			{
				holditem::ReleaseHoldItem();
				return S_OK;
			}

            GLMSG::NET_CLUB_STORAGE_TO_CLUB_STORAGE_CA SendData;
            SendData.m_Channel = static_cast<WORD>(dwChannel);
            SendData.m_PosX = wPosX;
            SendData.m_PosY = wPosY;
            SendData.m_HoldChannel = static_cast<WORD>(dwHoldChannel);
            SendData.m_HoldPosX = holditem::GetHoldPosX();
            SendData.m_HoldPosY = holditem::GetHoldPosY();
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);

            NETSENDTOAGENT(NET_MSG_CLUB_STORAGE_TO_CLUB_STORAGE_CA, SendBuffer, true);
        }
        break;

    };

    holditem::ReleaseHoldItem();

    return true;
}

//! Ŭ��â�� �ִ� �������� �տ� �鶧
bool GLGaeaClient::ReqClubItemToHoldItem(DWORD dwChannel, WORD wPosX, WORD wPosY)
{
	if (m_ServiceProvider == SP_INDONESIA)
		return false;

    if (!ClubStorageCheck(dwChannel))
    {
        holditem::ReleaseHoldItem();

        return false;
    }

    if (holditem::HaveHoldItem())
    {
        holditem::ReleaseHoldItem();

        return false;
    }

    const SINVENITEM* pClubItem = m_pMyClub->FindPosItem(dwChannel, wPosX, wPosY);
    if (!pClubItem)
    {
        holditem::ReleaseHoldItem();

        return false;
    }

    holditem::HoldItem( holditem::CLUB_STORAGE, pClubItem->CustomItem(), wPosX, wPosY );
    holditem::SetData( dwChannel );

    return true;
}

//! ��� �ִ� �������� Ŭ��â�� �ִ� �����۰� ��ȯ
bool GLGaeaClient::ReqClubItemExchangeToHoldItem(DWORD dwChannel, WORD wPosX, WORD wPosY)
{
	if (m_ServiceProvider == SP_INDONESIA)
		return false;

    if (!ClubStorageCheck(dwChannel))
    {
        holditem::ReleaseHoldItem();

        return false;
    }

    if (!holditem::HaveHoldItem())
    {
        holditem::ReleaseHoldItem();

        return false;
    }
    
    const SITEM* pItemData = GLogicData::GetInstance().GetItem(holditem::GetHoldItem().Id());
    if (!pItemData)
    {
        holditem::ReleaseHoldItem();

        return false;
    }

    // �ŷ��ɼ�
    //if (!pItemData->CanExchange())
	if ( !( holditem::GetHoldItem().IsExchange() != NULL ) )
    {
        holditem::ReleaseHoldItem();

        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CAN_NOT_TRADE_ITEM"));
        return false;
    }

    const SINVENITEM* pClubInven = m_pMyClub->FindPosItem(dwChannel, wPosX, wPosY);
    if (!pClubInven)
    {
        holditem::ReleaseHoldItem();

        return false;
    }

    if (pClubInven)
    {
        if (m_ServiceProvider == SP_VIETNAM)
        {
            if (holditem::GetHoldItem().bVietnamGainItem)
            {
                holditem::ReleaseHoldItem();

                return false;
            }
        }

        switch ( holditem::WhenHoldItem() )
        {
        case holditem::INVENTORY:
            {
                // Note : agent�� field �� ������� �������� ��ȯ�ؾ��ؼ�, ������ ���� ������ ����.
                /*GLMSG::NET_CLUB_STORAGE_EX_INVEN_CA SendData;
                SendData.m_Channel = static_cast<WORD>(dwChannel);
                SendData.m_ClubStoragePosX = pClubInven->wPosX;
                SendData.m_ClubStoragePosY = pClubInven->wPosY;
                SendData.m_InvenPosX = holditem::GetHoldPosX();
                SendData.m_InvenPosY = holditem::GetHoldPosY();
                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, SendData);
                NETSENDTOAGENT(NET_MSG_CLUB_ITEM_EXCHANGE_TO_INVEN_CA, SendBuffer, true);*/
            }
            break;

        case holditem::CLUB_STORAGE:
            {
//                 if ( wPosX == holditem::GetHoldPosX() &&
//                     wPosY == holditem::GetHoldPosY() )
//                 {
//                     holditem::ReleaseHoldItem();
// 
//                     return S_OK;
//                 }

                DWORD dwHoldChannel, dwDummy;
                holditem::GetData( dwHoldChannel, dwDummy );

				// ä�α��� Ȯ���ؾ�, �ٸ� ä���ε� ���� ��ǥ��� ���ϴ� ���� ����
				if ( dwChannel == dwHoldChannel && wPosX == holditem::GetHoldPosX() && wPosY == holditem::GetHoldPosY() )
				{
					holditem::ReleaseHoldItem();
					return S_OK;
				}

                GLMSG::NET_CLUB_STORAGE_EX_CLUB_STORAGE_CA SendData;
                SendData.m_Channel = static_cast<WORD>(dwChannel);
                SendData.m_PosX = pClubInven->wPosX;
                SendData.m_PosY = pClubInven->wPosY;
                SendData.m_HoldChannel = static_cast<WORD>(dwHoldChannel);
                SendData.m_HoldPosX = holditem::GetHoldPosX();
                SendData.m_HoldPosY = holditem::GetHoldPosY();
                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, SendData);

                NETSENDTOAGENT(NET_MSG_CLUB_STORAGE_EX_CLUB_STORAGE_CA, SendBuffer, true);
            }
            break;
        };

        holditem::ReleaseHoldItem();

        return true;
    }
    else
    {
        holditem::ReleaseHoldItem();

        return false;
    }
}
/*
bool GLGaeaClient::ReqClubStorageSplit(DWORD dwChannel, WORD wPosX, WORD wPosY, WORD wSplitNum)
{
    if (!IsClubStorageSplitItem(dwChannel,wPosX,wPosY))
        return false;

    const SINVENITEM* pInvenItem = m_pMyClub->FindPosItem(dwChannel, wPosX, wPosY);
    if (!pInvenItem)
        return false;

    if (pInvenItem->sItemCustom.wTurnNum <= wSplitNum)
        return false;

    //! ������ �и� �޽��� ����.
    GLMSG::SNET_CLUB_STORAGE_SPLIT NetMsg;
    NetMsg.dwChannel = dwChannel;
    NetMsg.wPosX = wPosX;
    NetMsg.wPosY = wPosY;
    NetMsg.wSplit = wSplitNum;
    NETSENDTOAGENT(&NetMsg);

    return true;
}
*/
/*
bool GLGaeaClient::IsClubStorageSplitItem(DWORD dwChannel, WORD wPosX, WORD wPosY)
{
    //GASSERT(MAX_CLUBSTORAGE>dwChannel);
    if (MAX_CLUBSTORAGE <= dwChannel)
        return false;

    if (!m_pCharacter->IsValidBody())
        return false;

    if (m_pCharacter->ValidWindowOpen())
        return false;		

    if (!IsKEEP_CLUB_STORAGE(dwChannel))
        return false;

    const SINVENITEM* pInvenItem = m_pMyClub->FindPosItem(dwChannel, wPosX, wPosY);
    if (!pInvenItem)
        return false;

    //! ������ ���� ��������.
    const SITEM* pItem = GLogicData::GetInstance().GetItem(pInvenItem->sItemCustom.sNativeID);
    if (!pItem)
        return false;

    bool bSPLIT(false);
    bSPLIT = (pItem->ISPILE());
    if (!bSPLIT)
        return false;
    bSPLIT = (pInvenItem->sItemCustom.wTurnNum > 1);
    return bSPLIT;
}
*/
bool GLGaeaClient::IsKEEP_CLUB_STORAGE(DWORD dwCHANNEL)
{
    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->HaveStorageAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    if (!m_pMyClub->GetStorageState())
        return false;

    if (m_pMyClub->GetRank() < dwCHANNEL)
        return false;

    return true;
}

void GLGaeaClient::MsgClubStorageReset(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_STORAGE_RESET* pNetMsg = (GLMSG::SNET_CLUB_STORAGE_RESET*) nmg;
    m_pMyClub->RESET_STORAGE();
}

void GLGaeaClient::MsgClubStorageSendEnd(NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CLUB_STORAGE_SEND_END* pPacket = (GLMSG::NET_CLUB_STORAGE_SEND_END*) nmg;
    m_pMyClub->SetStorageState(true);
}

void GLGaeaClient::MsgClubGetStorageItem(NET_MSG_GENERIC* nmg)
{
    // NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM    
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    GLMSG::SNET_CLUB_GETSTORAGE_ITEM RecvData;
    if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
    {
        m_pMyClub->InsertItem(
            RecvData.m_ItemData.sItemCustom,
            RecvData.m_Channel,
            RecvData.m_ItemData.wPosX,
            RecvData.m_ItemData.wPosY);
    }
}

void GLGaeaClient::MsgClubStorageInsert(NET_MSG_GENERIC* nmg)
{
    //GLMSG::SNET_CLUB_STORAGE_INSERT *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_INSERT *)nmg;
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;    
    GLMSG::SNET_CLUB_STORAGE_INSERT RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    const SINVENITEM& sInvenItem = RecvData.Data;
    const DWORD dwChannel = RecvData.dwChannel;

    m_pMyClub->InsertItem(sInvenItem.sItemCustom, dwChannel, sInvenItem.wPosX, sInvenItem.wPosY);

    // ��ȿ������ ȣ��, �ϴ� �ּ�:2012-05-23 jgkim
    // PET
    // ��ī�� ������ ��û�Ѵ�.
    /*
    const SITEM* pItem = GLogicData::GetInstance().GetItem ( sInvenItem.sItemCustom.sNativeID );
    if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && sInvenItem.sItemCustom.PetDbNum() != 0 )
    {
        GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(sInvenItem.sItemCustom.PetDbNum());
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }
    */
}

void GLGaeaClient::MsgClubStorageDelete(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_STORAGE_DELETE *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_DELETE *)nmg;
    const DWORD dwChannel = pNetMsg->dwChannel;
    /*
    // PET
    // ��ī�� ������ �������ش�.
    SINVENITEM* pInvenItem = m_sCLUB.m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
    if ( pInvenItem )
    {
    PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
    if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
    }
    */
    m_pMyClub->DeleteItem(dwChannel, pNetMsg->wPosX, pNetMsg->wPosY);
}

void GLGaeaClient::MsgClubStorageDelIns(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_STORAGE_DEL_AND_INS *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_DEL_AND_INS *)nmg;
    DWORD dwChannel = pNetMsg->dwChannel;
    
    // PET
    // ��ī�� ������ �������ش�.
    //SINVENITEM* pInvenItem = m_sCLUB.m_cStorage[dwChannel].GetItem ( pNetMsg->wDelX, pNetMsg->wDelX );
    //if ( pInvenItem )
    //{
    //    PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
    //    if ( iter != m_mapPETCardInfo.end() )
    //        m_mapPETCardInfo.erase ( iter );
    //}
    
    m_pMyClub->DeleteItem(dwChannel, pNetMsg->wDelX, pNetMsg->wDelY);
    m_pMyClub->InsertItem(pNetMsg->sInsert.sItemCustom, dwChannel, pNetMsg->sInsert.wPosX, pNetMsg->sInsert.wPosY);

    // PET
    // ��ī�� ������ ��û�Ѵ�.
    SITEM* pItem = GLogicData::GetInstance().GetItem ( pNetMsg->sInsert.sItemCustom.GetNativeID() );
    if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && pNetMsg->sInsert.sItemCustom.PetDbNum() != 0 )
    {
        GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(pNetMsg->sInsert.sItemCustom.PetDbNum());
        NETSENDTOFIELD(&NetMsg);
    }
}

void GLGaeaClient::MsgClubStorageUpdateItem(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM *)nmg;

    DWORD dwChannel = pNetMsg->dwChannel;
    SINVENITEM* pInvenItem = m_pMyClub->GetItem(dwChannel, pNetMsg->wPosX, pNetMsg->wPosY);
    if (pInvenItem)
        pInvenItem->sItemCustom.wTurnNum = pNetMsg->wTurnNum;
}
