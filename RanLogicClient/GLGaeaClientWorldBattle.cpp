#include "pch.h"
#include "../SigmaCore/Json/MinJson.h"

#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "../RanLogic/Msg/WorldBattleMsg.h"
#include "../RanLogic/Msg/GuidanceMsg.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/GLLevelFile.h"

#include "./Stage/DxGlobalStage.h"
#include "./Land/GLLandManClient.h"
#include "./Club/ClubCountryClientMan.h"
#include "./GLGaeaClient.h"
#include "./InterfaceBase.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::ReqClubCountryInfo()
{
    wb::NET_CLUB_COUNTRY_COUNT_CA MsgCA;
    NETSENDTOAGENT(&MsgCA);
    // 정보를 가져오고 있습니다. 잠시 기다려 주십시오.
}

void GLGaeaClient::MsgClubCountryCountStartAC(NET_MSG_GENERIC* pMsg)
{
    //wb::NET_CLUB_COUNTRY_INFO_START_AC* pPacket = (wb::NET_CLUB_COUNTRY_INFO_START_AC*) pMsg;
}

void GLGaeaClient::MsgClubCountryCountAC(NET_MSG_GENERIC* pMsg)
{
    wb::NET_CLUB_COUNTRY_COUNT_AC* pPacket = (wb::NET_CLUB_COUNTRY_COUNT_AC*) pMsg;
    const wb::COUNTRY_CLUB_COUNT_INFO& Info = pPacket->m_Data;
    GLogicData::GetInstance().wbSetCountryInfo(Info.m_Country, Info.m_ClubCount);
}

void GLGaeaClient::MsgClubCountryCountEndAC(NET_MSG_GENERIC* pMsg)
{
    //wb::NET_CLUB_COUNTRY_COUNT_END_AC* pPacket = (wb::NET_CLUB_COUNTRY_COUNT_END_AC*) pMsg;
    // 정보가져오기끝. UI 리프래쉬
    GetGlobalStage()->ShowCountryClubCountInSelectCountryWindow();
}

void GLGaeaClient::ReqClubCountryInfo(wb::EM_COUNTRY emCountry)
{
    if ( GetServiceProvider() == SP_WORLD_BATTLE )
    {
        if (!GLogicData::GetInstance().wbIsExistCountry(emCountry))
        {
            PrintMsgText(
                NS_UITEXTCOLOR::DISABLE,
                std::string(
                    sc::string::format(
                        "Unknown country code %1%", emCountry)));
        }
    }
    else
    {
        wb::NET_CLUB_COUNTRY_CA MsgCA(emCountry);
        NETSENDTOAGENT(&MsgCA);
        // 정보를 가져오고 있습니다. 잠시 기다려 주십시오.
    }
}

void GLGaeaClient::SetServerGroup(int ServerGroup) 
{
    if (m_ServerGroup != ServerGroup)
    {
        m_ServerGroup = ServerGroup;
        m_pClubCountryMan->Reset();
    }
}

void GLGaeaClient::MsgClubCountryDataAC(const json_spirit::mObject& Obj)
{
    int CountryCode=0;         sc::Json::find_value(Obj, "1", CountryCode);
    DWORD ClubDbNum=0;         sc::Json::find_value(Obj, "2", ClubDbNum);
    std::string ClubName;      sc::Json::find_value(Obj, "3", ClubName);
    DWORD ClubRank=0;          sc::Json::find_value(Obj, "4", ClubRank);
    int MasterLevel=0;         sc::Json::find_value(Obj, "5", MasterLevel);
    std::string MasterName;    sc::Json::find_value(Obj, "6", MasterName);
    __time64_t OrganizeDate=0; sc::Json::find_value(Obj, "7", OrganizeDate);
    DWORD MemberNum=0;         sc::Json::find_value(Obj, "8", MemberNum);
    std::string ClubNotice;    sc::Json::find_value(Obj, "9", ClubNotice);

    m_pClubCountryMan->AddClub(
        wb::EM_COUNTRY(CountryCode),
        ClubDbNum,
        ClubName,
        ClubRank,
        MasterLevel,
        MasterName,
        OrganizeDate,
        MemberNum,
        ClubNotice);
}

void GLGaeaClient::MsgClubCountryDataEndAC(const json_spirit::mObject& Obj)
{    
    //int CountryCode=0; sc::Json::find_value(Obj, "1", CountryCode);
    // UI Refresh
    GetGlobalStage()->ReloadClubListInSelectClubWindow();
}

//! 유저 국가의 클럽 기본정보를 가져온다.
void GLGaeaClient::SearchClubCountry(const std::string& SearchStr, const std::vector<wb::search::OrderBy>& Order, std::vector<wb::CLUB_INFO_BASE>& vInfo)
{
    m_pClubCountryMan->Search(SearchStr, Order, vInfo);
}

//! 국가명으로 검색한다
void GLGaeaClient::wbSearch(const std::string& SearchWord, std::vector<wb::COUNTRY_INFO>& Result)
{
    GLogicData::GetInstance().wbSearch(SearchWord, Result);
}

//! 설정된 전체국가를 가져온다
void GLGaeaClient::wbSearch(std::vector<wb::COUNTRY_INFO>& Result)
{
    GLogicData::GetInstance().wbSearch(Result);
}

bool GLGaeaClient::wbIsExistCountry(wb::EM_COUNTRY CountryCode) const
{
    return GLogicData::GetInstance().wbIsExistCountry(CountryCode);
}

//! 국가 정보를 가져온다
const wb::COUNTRY_INFO& GLGaeaClient::wbGetData(wb::EM_COUNTRY CountryCode) const
{
    return GLogicData::GetInstance().wbGetData(CountryCode);
}

//! 국기 이미지
const std::string& GLGaeaClient::wbGetFlagTexture(wb::EM_COUNTRY CountryCode) const
{
    return GLogicData::GetInstance().wbGetFlagTexture(CountryCode);
}

//! 국기 이미지. 국기 폭/높이
WORD GLGaeaClient::wbGetFlagWidth() const
{
    return GLogicData::GetInstance().wbGetFlagWidth();
}

WORD GLGaeaClient::wbGetFlagHeight() const
{
    return GLogicData::GetInstance().wbGetFlagHeight();
}

//! 국기 이미지. 국기 위치
const wb::FLAG_TEXTURE_POSITION& GLGaeaClient::wbGetFlagTexturePos(wb::EM_COUNTRY CountryCode) const
{
    return GLogicData::GetInstance().wbGetFlagTexturePos(CountryCode);
}

//! 국가/국가별 클럽숫자
void GLGaeaClient::wbSetCountryInfo(wb::EM_COUNTRY CountryCode, WORD ClubCount)
{
    GLogicData::GetInstance().wbSetCountryInfo(CountryCode, ClubCount);
}

void GLGaeaClient::ChangeUserCountry(wb::EM_COUNTRY NewCountry)
{    
    if (m_UserCountry != wb::UNKNOWN_COUNTRY)
        return;

    if (NewCountry == wb::UNKNOWN_COUNTRY)
        return;

    if (m_UserCountry == NewCountry)
        return;

    if (!wbIsExistCountry(NewCountry))
        return;

    wb::NET_USER_CHANGE_COUNTRY_CA MsgCA(NewCountry);
    NETSENDTOAGENT(&MsgCA);
}

void GLGaeaClient::MsgUserChangeCountryAC(NET_MSG_GENERIC* pMsg)
{
    wb::NET_USER_CHANGE_COUNTRY_AC* pPacket = (wb::NET_USER_CHANGE_COUNTRY_AC*) pMsg;
    const wb::COUNTRY_INFO& CountryInfo = wbGetData(pPacket->m_Country);
        
    switch (pPacket->m_Result)
    {
    case wb::USER_COUNTRY_CHANGE_SUCCESS:
        {
            SetUserCountry(pPacket->m_Country);

            DoModalOutInterface(
                sc::string::format(ID2GAMEEXTEXT("WB_CHANGE_COUNTRY_SUCCESS"), CountryInfo.m_FullName),
                UI::MODAL_NOTITLE, UI::OK, OUTER_MODAL_SUCCESS_CHANGE_COUNTRY );
        }
        break;
    case wb::USER_COUNTRY_CHANGE_USER_UNKNOWN_COUNTRY:
        DoModalOutInterface(
            sc::string::format(ID2GAMEEXTEXT("WB_CHANGE_COUNTRY_FAIL"), CountryInfo.m_FullName),
            UI::MODAL_NOTITLE, UI::OK, OUTER_MODAL_FAIL_CHANGE_COUNTRY );
        break;
    case wb::USER_COUNTRY_CHANGE_NEW_UNKNOWN_COUNTRY:
        DoModalOutInterface(
            sc::string::format(ID2GAMEEXTEXT("WB_CHANGE_COUNTRY_FAIL"), CountryInfo.m_FullName),
            UI::MODAL_NOTITLE, UI::OK, OUTER_MODAL_FAIL_CHANGE_COUNTRY );
        break;
    case wb::USER_COUNTRY_CHANGE_SAME_COUNTRY:
        DoModalOutInterface(
            sc::string::format(ID2GAMEEXTEXT("WB_CHANGE_COUNTRY_FAIL"), CountryInfo.m_FullName),
            UI::MODAL_NOTITLE, UI::OK, OUTER_MODAL_FAIL_CHANGE_COUNTRY );
        break;
    case wb::USER_COUNTRY_CHANGE_NOT_EXIST_COUNTRY:
        DoModalOutInterface(
            sc::string::format(ID2GAMEEXTEXT("WB_CHANGE_COUNTRY_FAIL"), CountryInfo.m_FullName),
            UI::MODAL_NOTITLE, UI::OK, OUTER_MODAL_FAIL_CHANGE_COUNTRY );
        break;
    default:
        DoModalOutInterface(
            sc::string::format(ID2GAMEEXTEXT("WB_CHANGE_COUNTRY_FAIL"), CountryInfo.m_FullName),
            UI::MODAL_NOTITLE, UI::OK, OUTER_MODAL_FAIL_CHANGE_COUNTRY );
        break;
    }
}

//void GLGaeaClient::SetFlagWinnerClubCountry( DWORD dwGuidanceID, wb::EM_COUNTRY emCountry )
//{
//	if (!GLogicData::GetInstance().wbIsExistCountry(emCountry))
//	{
//        if ( GetServiceProvider() == SP_WORLD_BATTLE )
//		    PrintMsgText(
//			    NS_UITEXTCOLOR::DISABLE,
//			    std::string(
//			    sc::string::format(
//			    "Unknown country code %1%", emCountry)));
//	}
//	else
//	{
//		const wb::COUNTRY_INFO& sCountryInfo = GLogicData::GetInstance().wbGetData(emCountry);
//
//		float fTextureSizeX = static_cast<float>(GLogicData::GetInstance().wbGetTextureWidth());
//		float fTextureSizeY = static_cast<float>(GLogicData::GetInstance().wbGetTextureHeight());
//
//		float fFlagSizeX = static_cast<float>(wbGetFlagWidth());
//		float fFlagSizeY = static_cast<float>(wbGetFlagHeight());
//
//		D3DXVECTOR4 vFlagConvert
//		(
//			(sCountryInfo.m_Pos.m_Left+0.5f)/fTextureSizeX,	// 깃발 U 시작점
//			(sCountryInfo.m_Pos.m_Top+0.5f)/fTextureSizeY,	// 깃발 V 시작점
//			(fFlagSizeX-1.f)/fTextureSizeX,							// 깃발 U 곱 연산
//			(fFlagSizeY-1.f)/fTextureSizeY							// 깃발 U 곱 연산
//		);
//
//		m_pLandMClient->SetFlagWinnerClubCountry( dwGuidanceID, sCountryInfo.m_Pos.m_TextureName, vFlagConvert );
//	}
//}

void GLGaeaClient::MsgWbCbSetFlagFACBRD(NET_MSG_GENERIC* pMsg)
{
	wb::SNET_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD* pPacket = (wb::SNET_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD*) pMsg;
	//SetFlagWinnerClubCountry( pPacket->dwGuidanceID, static_cast<wb::EM_COUNTRY>(pPacket->dwCountryCode) );
}

void GLGaeaClient::ReqGuidanceInfo()
{
    NET_GM_GUIDANCE_INFO_CA MsgCA;
    NETSENDTOAGENT(&MsgCA);
}

void GLGaeaClient::MsgGmGuidanceInfoAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_GM_GUIDANCE_INFO_AC Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return;

    std::string Msg;

    Msg = sc::string::format("Guidance ID : %1% %2%", Data.m_Id, Data.m_GuidName); // 관리 id
    PrintMsgText(NS_UITEXTCOLOR::DEFAULT, Msg);

    Msg = sc::string::format("Club : %1% %2%", Data.m_ClubId, Data.m_ClubName);
    PrintMsgText(NS_UITEXTCOLOR::DEFAULT, Msg);

    const wb::COUNTRY_INFO& CountryData = wbGetData(wb::EM_COUNTRY(Data.m_Country));
    Msg = sc::string::format("Country : %1% %2%", Data.m_Country, CountryData.m_FullName);
    PrintMsgText(NS_UITEXTCOLOR::DEFAULT, Msg);

    Msg = sc::string::format("Commission : %1%", Data.m_Commission);
    PrintMsgText(NS_UITEXTCOLOR::DEFAULT, Msg);

    for (size_t i=0; i<Data.m_GuidMap.size(); ++i)
        PrintMsgText(NS_UITEXTCOLOR::DEFAULT, GetMapNameStr(SNATIVEID(Data.m_GuidMap[i])));
}

void GLGaeaClient::ReqGuidanceChange(DWORD GuidId, DWORD ClubDbNum)
{
    NET_GM_GUIDANCE_CHANGE_CA MsgCA(GuidId, ClubDbNum);
    NETSENDTOAGENT(&MsgCA);
}

void  GLGaeaClient::ReqGuidanceOnOff(DWORD GuidId, int nStateFlag)
{
	NET_GM_GUIDANCE_ON_OFF_CA NetMsgCA(GuidId, nStateFlag);
	NETSENDTOAGENT(&NetMsgCA);
}

void GLGaeaClient::ToggleJoinWorldBattleButton(bool bOn)
{
	//!! SF_TODO
	//if( m_pInterface != NULL )
	//	m_pInterface->ToggleJoinWorldBattleButton(bOn);
}
