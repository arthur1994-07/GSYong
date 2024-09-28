#include "pch.h"
#include "../SigmaCore/File/Excel/MinExcel.h"

#include "./Friend/GLFriendClient.h"
#include "./Party/GLPartyManagerClient.h"
#include "./Party/GLPartyTenderManagerClient.h"
#include "./Post/GLPostClient.h"
#include "./SNS/GLSNSClient.h"
#include "./Trade/GLTradeClient.h"
#include "./Tutorial/GLBaseTutorial.h"
#include "./GLGaeaClient.h"
#include "./GroupChat/GroupChatClient.h"
#include "./Event/EventClient.h"
#include "./Land/Faction/FactionManagerClient.h"


// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLFriendClient* GLGaeaClient::GetFriendClient()
{
    return m_pFriendClient;
}

MiniGame::GLManagerClient* const GLGaeaClient::GetMiniGameManager(void) const
{
	return m_pMiniGameManager;
}
GLEvent::ManagerClient* const GLGaeaClient::GetEventManager(void)
{
	return m_pEventManager;
}
InstanceSystem::ManagerClient* const GLGaeaClient::GetInstanceManager(void) const
{
	return m_pInstanceManager;
}

GLPartyClient* const GLGaeaClient::GetMyPartyClient(void)
{
	// 클라이언트의 경우 미리 생성 해 놓기 때문에 NULL일 수 없음;
	// isValid() 혹은 GetMember() 함수 등으로 유효성 검사 해야 함;
    return m_pPartyManagerClient->GetMyParty();
}

GLPartyManagerClient* const GLGaeaClient::GetPartyManager(void)
{
	return m_pPartyManagerClient;
}

GLPartyTenderManagerClient* GLGaeaClient::GetPartyTenderManager(void)
{
	return m_pPartyTenderManagerClient;
}
GLGroupChat* GLGaeaClient::GetChatGroup()
{
	return m_pChatGroup;
}

PrivateMarketClient* GLGaeaClient::GetPrivateMarketClient()
{
	return m_pPrivateMarketSearchBuyData;
}

ConsignmentSaleClient* GLGaeaClient::GetConsignmentSale()
{
    return m_pConsignmentSale;
}

GLPostClient* GLGaeaClient::GetPostClient()
{
    return m_pPostClient;
}

GLSNSClient* GLGaeaClient::GetSNSClient()
{
    return m_pSNSClient;
}

GLTradeClient* GLGaeaClient::GetTradeClient()
{
    return m_pTradeClient;
}

GLBaseTutorial* GLGaeaClient::GetTutorial()
{
    return m_pBaseTutorial;
}

pointshop::PointShopClient* GLGaeaClient::GetPointShopClient()
{
    return m_pPointShop;
}

Faction::ManagerClient* GLGaeaClient::GetFactionClient ()
{
	GLLandManClient* pLandMan = GetLandManClient();
	if ( NULL == pLandMan )
		return NULL;

	Faction::ManagerClient* pFactionMan =
		pLandMan->GetFactionManager();
	if ( NULL == pFactionMan )
		return NULL;
	
	return pFactionMan;
}

const DamageRecord::Manager* const GLGaeaClient::GetDamageRecord(void) const
{
	return m_pDamageRecord;
}

void GLGaeaClient::SaveCrowListToFile()
{
    PrintMsgText(NS_UITEXTCOLOR::DEFAULT, "Wait...");
    std::string SaveFileName;
    sc::Excel ExcelFile("CrowList", SaveFileName, 0, true);
    GLogicData::GetInstance().SaveCrowDataSimple(ExcelFile, 0, "CrowList");
    PrintMsgText(NS_UITEXTCOLOR::DEFAULT, SaveFileName);
}
