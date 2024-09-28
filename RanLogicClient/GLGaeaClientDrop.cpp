#include "pch.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../EngineLib/DxTools/DebugSet.h"
/*#include "../RanLogic/Msg/WorldBattleMsg.h"*/

#include "./Char/CharNameDbNumCache.h"
#include "./Land/GLLandManClient.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::MsgDropMaterial(NET_MSG_GENERIC* pMsg) 
{
	if ( !m_pLandMClient )
	{
		// 필요한 정보이므로 내 자신에게 다시 쏜다.
		NETSENDTOMYSELF(pMsg);
		return;
	}

    GLMSG::SNETDROP_MATERIAL *pNetMsg = (GLMSG::SNETDROP_MATERIAL*) pMsg;
    SDROP_MATERIAL &sDropMaterial = pNetMsg->Data;
    if ( sDropMaterial.sMapID == m_pLandMClient->GetBaseMapID() )
    {
        m_pLandMClient->DropMaterial ( &sDropMaterial );
    }
    else
    {
        CDebugSet::ToListView ( "sDropMaterial.sMapID != m_pLandMClient->GetMapID()" );
    }
}

void GLGaeaClient::MsgDropCrow(NET_MSG_GENERIC* pMsg) 
{
	if ( !m_pLandMClient )
	{
		// 필요한 정보이므로 내 자신에게 다시 쏜다.
		NETSENDTOMYSELF(pMsg);
		return;
	}

    GLMSG::SNETDROP_CROW *pNetMsg = (GLMSG::SNETDROP_CROW*) pMsg;
    SDROP_CROW &sDropCrow = pNetMsg->Data;
    if ( sDropCrow.sMapID == m_pLandMClient->GetBaseMapID() )
    {
        //CDebugSet::ToListView ( "Drop Crow : [%d,%d], GLOB ID %d",
        //	sDropCrow.sNativeID.wMainID, sDropCrow.sNativeID.wSubID, sDropCrow.dwGlobID);

        m_pLandMClient->DropCrow ( &sDropCrow );
    }
    else
    {
        CDebugSet::ToListView ( "sDropCrow.sMapID != m_pLandMClient->GetMapID()" );
    }
}

void GLGaeaClient::MsgDropPc_Base(NET_MSG_GENERIC* pMsg) 
{
    // 패킷 분리 : DropPc정보설정 첫단계
    GLMSG::SNETDROP_PC_BASE *pNetMsg = (GLMSG::SNETDROP_PC_BASE*) pMsg;
    SDROP_CHAR_BASE& sDropChar = pNetMsg->m_Data;
    if (sDropChar.sMapID != m_pLandMClient->GetBaseMapID())
    {
        CDebugSet::ToListView("sDropChar.sMapID != m_pLandMClient->GetMapID()");
        return;
    }
    
    if (sDropChar.dwGaeaID == GETMYGAEAID())
    {
        // 자기 자신
        CDebugSet::ToListView("[ERROR] Drop Char : %s, %d 자기자신의 아이디.", sDropChar.szName, sDropChar.dwGaeaID);
        return;
    }

    std::tr1::shared_ptr<SDROP_CHAR> spDropChar(new SDROP_CHAR(sDropChar));
    LAND_DROP_CHAR_ITER iter = m_TempLandDropChar.find(sDropChar.dwCharID);
    if (iter != m_TempLandDropChar.end())
        m_TempLandDropChar.erase(iter);
    m_TempLandDropChar.insert(LAND_DROP_CHAR_VALUE(sDropChar.dwCharID, spDropChar));

    m_pChaDbNumNameCache->Add(std::string(sDropChar.szName), sDropChar.dwCharID);
}

void GLGaeaClient::MsgDropPc_StateBlow_SkillFact(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETDROP_PC_STATEBLOW_SKILLFACT* pNetMsg = (GLMSG::SNETDROP_PC_STATEBLOW_SKILLFACT*) pMsg;

    // 패킷 분리 : DropPc정보설정의 중간단계
    
    LAND_DROP_CHAR_ITER iter = m_TempLandDropChar.find(pNetMsg->m_ChaDbNum);
    if (iter == m_TempLandDropChar.end())
        return;
    
    std::tr1::shared_ptr<SDROP_CHAR> spDropChar = iter->second;
    if (spDropChar->m_Base.sMapID == m_pLandMClient->GetBaseMapID())
    {        
        if (spDropChar->m_Base.dwGaeaID != GETMYGAEAID())
        {
            spDropChar->SetStateBlows(pNetMsg->sSTATEBLOWS);
            spDropChar->SetSkillFact(pNetMsg->sSKILLFACT);              
        }
    }
}

void GLGaeaClient::MsgDropPc_PutOnItems(NET_MSG_GENERIC* pMsg)
{
    // 패킷 분리 : DropPc정보설정의 마지막단계
    
    GLMSG::SNETDROP_PC_PUT_ON_ITEMS* pNetMsg = (GLMSG::SNETDROP_PC_PUT_ON_ITEMS*) pMsg;

    LAND_DROP_CHAR_ITER iter = m_TempLandDropChar.find(pNetMsg->m_ChaDbNum);
    if (iter == m_TempLandDropChar.end())
        return;

    std::tr1::shared_ptr<SDROP_CHAR> spDropChar = iter->second;
    if (spDropChar->m_Base.sMapID == m_pLandMClient->GetBaseMapID())
    {        
        if (spDropChar->m_Base.dwGaeaID != GETMYGAEAID())
        {
            spDropChar->SetPutOnItems(pNetMsg->m_PutOnItems);
            m_pLandMClient->DropChar(spDropChar);   //PutOnItems가 DropPc정보의 마지막 패킷이여야 한다.
        }
    }

    if (spDropChar->m_Base.dwGuild != CLUB_NULL)
    {
        std::string ClubName(m_pClubCountryMan->GetClubName(spDropChar->m_Base.dwGuild));
        if (!ClubName.empty())        
        {
            std::tr1::shared_ptr<GLCharClient> pChar = m_pLandMClient->GetChar(spDropChar->m_Base.dwGaeaID);
            if (pChar)
                pChar->SetClubName(ClubName);                    
        }
        else
        {
            // 클럽 이름 정보 요청
            //wb::NET_REQ_DROP_CHAR_CLUB_NAME_CA MsgCA(spDropChar->m_Base.dwGuild, spDropChar->m_Base.dwGaeaID);
            //NETSENDTOAGENT(&MsgCA);
            m_TempClubReqSendData.Add( spDropChar->m_Base.dwGuild, spDropChar->m_Base.dwGaeaID );
        }
    }

    m_TempLandDropChar.erase(iter);
}

void GLGaeaClient::MsgDropPc_End(NET_MSG_GENERIC* pMsg)
{

    // 코드는 이쪽으로 이동되었다.
    // GLGaeaClient::ClubNameReq();

    return;
}


void GLGaeaClient::MsgUpdateDropCharClubNameAC(NET_MSG_GENERIC* pMsg)
{
    wb::NET_UPDATE_DROP_CHAR_CLUB_NAME_AC* pPacket = (wb::NET_UPDATE_DROP_CHAR_CLUB_NAME_AC*) pMsg;
    if (m_pLandMClient)
    {
        std::string ClubName(m_pClubCountryMan->GetClubName(pPacket->m_ClubDbNum));
        if (!ClubName.empty())
        {
            std::tr1::shared_ptr<GLCharClient> pChar = m_pLandMClient->GetChar(pPacket->m_GaeaId);
            if (pChar)
                pChar->SetClubName(ClubName);
        }
    }
}

void GLGaeaClient::MsgDropMoney(NET_MSG_GENERIC* pMsg) 
{
	if ( !m_pLandMClient )
	{
		// 필요한 정보이므로 내 자신에게 다시 쏜다.
		NETSENDTOMYSELF(pMsg);
		return;
	}

    // NET_MSG_DROP_MONEY
    // GLMSG::SNETDROP_MONEY *pNetMsg = reinterpret_cast<GLMSG::SNETDROP_MONEY *> (pMsg);
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::SNETDROP_MONEY RecvData;
    if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        m_pLandMClient->DropMoney(RecvData.lnAmount, RecvData.vPos, RecvData.dwGlobID, RecvData.fAge, RecvData.bCanGet);
}

void GLGaeaClient::MsgDropItem(NET_MSG_GENERIC* pMsg) 
{
	if ( !m_pLandMClient )
	{
		// 필요한 정보이므로 내 자신에게 다시 쏜다.
		NETSENDTOMYSELF(pMsg);
		return;
	}

    // NET_MSG_DROP_ITEM    
    // GLMSG::SNETDROP_ITEM* pNetMsg = reinterpret_cast<GLMSG::SNETDROP_ITEM*> (pMsg);
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::SNETDROP_ITEM DropItem;
    if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), DropItem))    
        m_pLandMClient->DropItem(&DropItem.Data);
}

void GLGaeaClient::MsgMapWhimsical(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNETPC_MAPWHIMSICALWEATHER *pNetMsg = reinterpret_cast<GLMSG::SNETPC_MAPWHIMSICALWEATHER *> (pMsg);
    DxWeatherMan::GetInstance()->ReceiveMapWhimsical ( pNetMsg->MapWeather.map_mID, pNetMsg->MapWeather.map_sID, 
        pNetMsg->MapWeather.dwWhimsicalWeather );
}

void GLGaeaClient::MsgWhimsical(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNETPC_WHIMSICALWEATHER *pNetMsg = reinterpret_cast<GLMSG::SNETPC_WHIMSICALWEATHER *> (pMsg);
    DxWeatherMan::GetInstance()->ReceiveWhimsical ( pNetMsg->dwWhimsical );
}

void GLGaeaClient::MsgMapWeather(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNETPC_MAPWEATHER *pNetMsg = reinterpret_cast<GLMSG::SNETPC_MAPWEATHER *> (pMsg);
    DWORD i;
    GLPeriod::GetInstance().ResetOneMapWeather();
    for( i = 0; i < pNetMsg->dwMapWeatherSize; i++ )
    {
        GLPeriod::GetInstance().SetOneMapActiveWeather( pNetMsg->MapWeather[i], FALSE );
    }
}

void GLGaeaClient::MsgWeather(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNETPC_WEATHER *pNetMsg = reinterpret_cast<GLMSG::SNETPC_WEATHER *> (pMsg);
    GLPeriod::GetInstance().SetWeather ( pNetMsg->dwWeather );
}

void GLGaeaClient::MsgPeriod(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNET_PERIOD *pNetMsg = reinterpret_cast<GLMSG::SNET_PERIOD *> (pMsg);
    GLPeriod::GetInstance().SetPeriod ( pNetMsg->sPRERIODTIME );
}

